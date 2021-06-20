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
import android.os.Build
import android.os.VibrationEffect
import android.os.Vibrator
import androidx.preference.PreferenceManager
import com.picross.nonocross.views.grid.Cell.CellShade
import java.io.InputStream
import kotlin.random.Random
import com.picross.nonocross.LevelDetails as LD

fun generate(context: Context) {

    val preferences = PreferenceManager.getDefaultSharedPreferences(context)
    val columns = preferences.getInt("columns", 10)
    val rows = preferences.getInt("rows", 10)
    val difficulty = preferences.getInt("difficulty", 10)
    val seed = if (preferences.getBoolean("level seed enable", false)) preferences.getLong(
        "level seed",
        System.currentTimeMillis()
    ) else System.currentTimeMillis()

    LD.gridData = if (LD.isRandom) {
        // Difficulty is set by changing the proportion of filled to empty cell
        val random = Random(seed)

        GridData(
            rows,
            List(rows * columns) {
                if (random.nextInt(
                        0,
                        100
                    ) > 3 * difficulty + 46
                ) CellShade.SHADE else CellShade.EMPTY
            }).toGridData2()
    } else {
        openGridFile(context, LD.levelName)
    }
}

fun openGridFile(context: Context, chosenLevelName: String): GridData2 {
    val inputStream: InputStream = context.resources.assets.open("levels/$chosenLevelName")
    val buffer = ByteArray(inputStream.available())
    inputStream.read(buffer)
    val text = String(buffer)

    return parseNonFile(text)
}

fun vibrate(context: Context) {
    when {
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