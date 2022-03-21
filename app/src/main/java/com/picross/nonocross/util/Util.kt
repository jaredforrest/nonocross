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
import android.widget.Toast
import androidx.preference.PreferenceManager
import arrow.core.Either
import arrow.core.None
import arrow.core.Option
import arrow.core.none
import com.picross.nonocross.LevelType
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
import java.util.*
import kotlin.random.Random

/* wHD is triple(width, height, difficulty) */
/*tailrec */suspend fun newUniqueRandomGrid(
    wHD: Triple<Int, Int, Int>,
    random: Random = Random
): Option<GridData> {
//    return if(!Thread.interrupted()) {
    var rec: List<CellShade>//newRandomGrid(wHD, random)
    var ret = none<GridData>()// = rec.checkUnique(wHD.second)
    coroutineScope {
        while (isActive && ret is None) {
            rec = newRandomGrid(wHD, random)
            ret = rec.checkUnique(wHD.second)
        }
    }
    return ret
    /*return if(isActive) {
        val rec = newRandomGrid(wHD, random)
        when (val ret = rec.checkUnique(wHD.second)) {
            is None -> newUniqueRandomGrid(wHD, random)
            is Some -> Some(ret.value)
        }
    } else none()*/
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

fun getOnlineGridPrefs(context: Context): OnlinePrefs {
    val preferences = PreferenceManager.getDefaultSharedPreferences(context)
    return OnlinePrefs(
        preferences.getInt("minCells", 0),
        preferences.getInt("maxCells", 20),
        preferences.getInt("minDiff", 4),
        preferences.getInt("maxDiff", 20),
        preferences.getBoolean("unratedDiff", true),
        preferences.getInt("minQual", 4),
        preferences.getInt("maxQual", 20),
        preferences.getBoolean("unratedQual", true),
        preferences.getString("apiKey", "") ?: "",
    )
}

fun readTextFromUri(uri: Uri, context: Context): String {
    return try {
        context.contentResolver.openInputStream(uri)?.readBytes()?.let { String(it) } ?: ""
    } catch (e: IOException) {
        ""
    }
}

suspend fun openGridFile(
    context: Context,
    chosenLevelName: String,
    customLevel: Boolean
): GridData {
    val text =
        if (customLevel) File(
            context.getDir("levels", Context.MODE_PRIVATE),
            chosenLevelName
        ).readText()
        else String(context.resources.assets.openNonBlocking("levels/$chosenLevelName").readBytes())
    Log.d("grid ios", text)

    return when (val grid = parseNonFile(text)) {
        is Either.Right -> grid.value
        is Either.Left -> throw Exception(grid.value.toString())//Exception("Error parsing level", grid.value)//GridData(0, 0, listOf(), listOf())
    }
}

fun addCustomLevel(
    filename: String,
    fileContents: String,
    context: Context,
    userGrid: UserGrid? = null
) {
    FileOutputStream(File(context.getDir("levels", Context.MODE_PRIVATE), filename)).use {
        it.write(fileContents.toByteArray())
    }
    if (userGrid != null) {
        saveCurrentGridState(context, LevelType.Custom(filename), userGrid)
    }
}


fun saveCurrentGridState(context: Context, lT: LevelType, userGrid: UserGrid) {
    val fileContents = userGrid.currentState.toByteArray()
    val saveDir = context.getDir("saves", Context.MODE_PRIVATE)
    when (lT) {
        is LevelType.Random, LevelType.Online -> {
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
    Log.d("cooloio", "an ereorereaer")
    when (this) {
        is Either.Left -> errorToast(
            baseContext,
            this.value.toString()
        )//Toast.makeText(baseContext,"Error: ${this.value} "/*$errorString" ${this.value.toString()}"*/, Toast.LENGTH_LONG).show()
        is Either.Right -> f(this.value)
    }
}

fun errorToast(context: Context, err: String) {
    Toast.makeText(context, "Error: $err ", Toast.LENGTH_LONG).show()
}

fun secondsToTime(seconds: Int): String {
    val hours: Int = seconds / 3600
    val minutes: Int = seconds % 3600 / 60
    val secs: Int = seconds % 60

    // Format the seconds into hours, minutes,
    // and seconds.
    return java.lang.String
        .format(
            Locale.getDefault(),
            "%d:%02d:%02d", hours,
            minutes, secs
        )
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
            (context.getSystemService(Context.VIBRATOR_SERVICE) as Vibrator).vibrate(
                VibrationEffect.createOneShot(
                    50,
                    VibrationEffect.DEFAULT_AMPLITUDE
                )
            )
        }
        else -> {
            (context.getSystemService(Context.VIBRATOR_SERVICE) as Vibrator).vibrate(50)
        }
    }
}

data class OnlinePrefs(
    val minSize: Int,
    val maxSize: Int,
    val minQuality: Int,
    val maxQuality: Int,
    val allowUnratedQual: Boolean,
    val minDifficulty: Int,
    val maxDifficulty: Int,
    val allowUnratedDiff: Boolean,
    val apiKey: String
)