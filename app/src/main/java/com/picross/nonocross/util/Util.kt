/**This file is part of Nonocross.

Nonocross is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Nonocross is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Nonocross.  If not, see <https://www.gnu.org/licenses/>.*/
package com.picross.nonocross.util

import android.content.Context
import android.content.res.AssetManager
import android.net.Uri
import android.os.Build
import android.os.Build.VERSION_CODES.S
import android.os.VibrationEffect
import android.os.Vibrator
import android.os.VibratorManager
import android.util.Log
import android.util.TypedValue
import android.widget.Toast
import androidx.annotation.AttrRes
import androidx.annotation.ColorInt
import androidx.preference.PreferenceManager
import arrow.core.Either
import arrow.core.None
import arrow.core.Option
import arrow.core.Some
import arrow.core.none
import com.picross.nonocross.LevelType
import com.picross.nonocross.R
import com.picross.nonocross.util.usergrid.GridData
import com.picross.nonocross.util.usergrid.UserGrid
import com.picross.nonocross.util.usergrid.checkUnique
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.coroutineScope
import kotlinx.coroutines.isActive
import kotlinx.coroutines.withContext
import java.io.File
import java.io.FileOutputStream
import java.io.IOException
import kotlin.random.Random

/* wHD is triple(width, height, difficulty) */
suspend fun newUniqueRandomGrid(
    wHD: Triple<Int, Int, Int>,
    random: Random = Random
): Option<GridData> {
    var rec: List<CellShade>
    var ret = none<GridData>()
    coroutineScope {
        while (isActive && ret is None) {
            val ( width, height, _) = wHD
            rec = newRandomGrid(wHD, random)
            ret = rec.checkUnique(width, height)
        }
    }
    return ret
}

fun newRandomGrid(wHD: Triple<Int, Int, Int>, random: Random = Random) =
    List(wHD.second * wHD.first) {
        if (random.nextInt(0, 100) > 4 * wHD.third + 16) CellShade.SHADE
        else CellShade.EMPTY
    }

fun getRandomGridPrefs(context: Context): Triple<Int, Int, Int> {
    val preferences = PreferenceManager.getDefaultSharedPreferences(context)
    val width = preferences.getInt("columns", 10)
    val height = preferences.getInt("rows", 10)
    val difficulty = preferences.getInt("difficulty", 5)
    return Triple(width, height, difficulty)
}

fun readTextFromUri(uri: Uri, context: Context): String {
    return try {
        val stream = context.contentResolver.openInputStream(uri)
        val ret = stream?.readBytes()?.let { String(it) } ?: ""
        stream?.close()
        ret

    } catch (e: IOException) {
        ""
    }
}

suspend fun openGridFile(
    context: Context,
    chosenLevelName: String,
    customLevel: Boolean
): Either<ParseError, GridData> {
    val text =
        if (customLevel) File(
            context.getDir("levels", Context.MODE_PRIVATE),
            chosenLevelName
        ).readText()
        else String(context.resources.assets.openNonBlocking("levels/$chosenLevelName").readBytes())
    Log.d("grid ios", text)

    return parseNonFile(text)
}

tailrec fun addCustomLevel(
    _filename: String,
    fileContents: String,
    context: Context,
    userGrid: UserGrid? = null,
    copy: UInt = 0u
): String {
    val filename = (if(_filename.filterNot { it.isWhitespace() } == "")
        context.getString(R.string.untitled_level)
    else
        _filename) + if(copy != 0u) " ($copy)" else ""
    if(File(context.getDir("levels", Context.MODE_PRIVATE), filename).isFile){
        return addCustomLevel(_filename,fileContents,context,userGrid,copy.inc())
    }
    FileOutputStream(File(context.getDir("levels", Context.MODE_PRIVATE), filename)).use {
        it.write(fileContents.toByteArray())
    }
    if (userGrid != null) {
        saveCurrentGridState(context, LevelType.Custom(filename), userGrid)
    }
    return filename
}


fun saveCurrentGridState(context: Context, lT: LevelType, userGrid: UserGrid) {
    val fileContents = userGrid.toBytes().toByteArray()
    val saveDir = context.getDir("saves", Context.MODE_PRIVATE)
    when (lT) {
        is LevelType.Random -> {
            when (lT.levelName) {
                is None -> return
                is Some -> FileOutputStream(File(File(saveDir, "custom"), lT.levelName.value)).use {
                    it.write(fileContents)
                }
            }
        }
        is LevelType.Online -> {
            return
        }
        is LevelType.Custom -> {
            FileOutputStream(File(File(saveDir, "custom"), lT.levelName)).use {
                it.write(fileContents)
            }
        }
        is LevelType.Default -> {
            FileOutputStream(File(File(saveDir, "default"), lT.levelName)).use {
                it.write(fileContents)
            }
        }
    }
}

suspend fun AssetManager.openNonBlocking(s: String) =
    withContext(Dispatchers.IO) { open(s) }

// U is an error type/class
fun <T, U> Either<T, U>.applyNotError(baseContext: Context, f: U.() -> Unit) {
    when (this) {
        is Either.Right -> this.value.apply(f)
        is Either.Left -> errorToast(
            baseContext,
            this.value.toString()
        )
    }
}

fun errorToast(context: Context, err: String) {
    Toast.makeText(context, context.getString(R.string.error, err), Toast.LENGTH_LONG).show()
}

// qsecond is a quater-second
fun secondsToTime(qseconds: UInt): String {
    val seconds = qseconds / 4u
    val hours: UInt = seconds / 3600u
    val minutes: UInt = seconds % 3600u / 60u
    val secs: UInt = seconds % 60u

    // Format the seconds into hours, minutes,
    // and seconds.
    return ("$hours:" + if (minutes < 10u) "0$minutes:" else {
        "$minutes:"
    } + if (secs < 10u) "0$secs" else "$secs")
}

fun vibrate(context: Context) {
    when {
        Build.VERSION.SDK_INT >= S -> {
            val vibratorManager =
                context.getSystemService(Context.VIBRATOR_MANAGER_SERVICE) as VibratorManager
            vibratorManager.defaultVibrator.vibrate(
                VibrationEffect.createOneShot(
                    50,
                    VibrationEffect.DEFAULT_AMPLITUDE
                )
            )
        }
        Build.VERSION.SDK_INT >= 26 -> {
            @Suppress("DEPRECATION")
            (context.getSystemService(Context.VIBRATOR_SERVICE) as Vibrator).vibrate(
                VibrationEffect.createOneShot(
                    50,
                    VibrationEffect.DEFAULT_AMPLITUDE
                )
            )
        }
        else -> {
            @Suppress("DEPRECATION")
            (context.getSystemService(Context.VIBRATOR_SERVICE) as Vibrator).vibrate(50)
        }
    }
}

fun Context.getLevelName(index: Int): String {
    return if(index <= 3) getString(R.string.easy, index + 1)
    else getString(R.string.level, index - 3)
}

@ColorInt
fun Context.resolveThemedColor(@AttrRes colorAttr: Int, @ColorInt fallbackColor: Int = 0): Int {
    val tv = TypedValue()
    theme.resolveAttribute(colorAttr, tv, true)
    return if (tv.type >= TypedValue.TYPE_FIRST_COLOR_INT && tv.type <= TypedValue.TYPE_LAST_COLOR_INT) {
        tv.data
    } else {
        fallbackColor
    }
}