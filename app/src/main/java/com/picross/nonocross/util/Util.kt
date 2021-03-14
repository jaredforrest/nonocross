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
import com.picross.nonocross.views.grid.Cell
import com.picross.nonocross.views.grid.Cell.CellShade
import java.io.InputStream
import com.picross.nonocross.LevelDetails as LD

fun generate(context: Context) {

    val preferences = PreferenceManager.getDefaultSharedPreferences(context)
    val columns = preferences.getInt("columns", 10)
    val rows = preferences.getInt("rows", 10)
    val difficulty = preferences.getInt("difficulty", 10)

    LD.gridData = if (LD.isRandom) {
        // Difficulty is set by changing the proportion of filled to empty cell
        // ie. difficulty=5 -> listOf(0,1,1,1,1,1)
        // difficulty=10 -> listOf(0,1)
        val difficultyList =
            List(12 - difficulty) { i -> if (i != 0) CellShade.SHADE else CellShade.EMPTY }

        GridData(rows, List(rows * columns) { difficultyList.random() })
    } else {
        openGridFile(context, LD.levelName)
    }
}

fun openGridFile(context: Context, chosenLevelName: String): GridData {
    val inputStream: InputStream = context.resources.assets.open("levels/$chosenLevelName")
    val buffer = ByteArray(inputStream.available())
    inputStream.read(buffer)
    val rows = String(buffer).takeWhile { it != ' ' }
        .toInt()
    val text = String(buffer).dropWhile { it != ' ' }.drop(1)

    return GridData(rows, text.map { if (it == '1') CellShade.SHADE else CellShade.EMPTY })
}

data class GridData(val rows: Int, val grid: List<CellShade>) {

    private val size = grid.size
    val cols = if (rows == 0) 0 else size / rows

    // These are the numbers at the side/top of the row/column in-game
    val rowNums = List(rows) { i -> countCellNums(row(i)) }
    val colNums = List(cols) { i -> countCellNums(col(i)) }

    /** Gets the width and height of the longest row and column */
    val longestRowNum =
        rowNums.fold(0,
            { acc, i -> (i.size + if (i.count { it >= 10 } > 0) 1 else 0).coerceAtLeast(acc) })
    val longestColNum = colNums.fold(0, { acc, i -> i.size.coerceAtLeast(acc) })

    private fun countCellNums(row: List<CellShade>): List<Int> {
        return (row.runningFold(0) { acc, cell ->
            if (cell == CellShade.SHADE) acc + 1
            else 0
        } + 0)
            .zipWithNext { a, b -> if (b == 0) a else 0 }
            .filterNot { it == 0 }
    }

    fun isEmpty() = grid.isEmpty()

    private fun row(index: Int) = grid.subList(index * cols, index * cols + cols)
    private fun col(index: Int) = List(rows) { j -> grid[j * cols + index] }
}

data class UserGrid(val rows: Int, var grid: List<Cell>) {

    operator fun get(i: Int, j: Int) = grid[i * cols + j]

    fun clear() {
        grid = grid.map { cell ->
            cell.userShade = CellShade.EMPTY
            cell
        }
    }

    private val size = grid.size
    private val cols = size / rows

    val data get() = grid.map { cell -> cell.userShade }

    val rowNums get() = GridData(rows, data).rowNums
    val colNums get() = GridData(rows, data).colNums

    //private fun row(index: Int) =  data.subList(index * cols, index * cols + cols)
    //private fun col(index: Int) =  List(rows) { j -> data[j * cols + index] }
}

data class UndoStack(val gridSize: Int) {
    private val elements: MutableList<List<CellShade>> =
        MutableList(1) { List(gridSize) { CellShade.EMPTY } }

    fun push(item: UserGrid) =
        elements.add(item.data)

    fun pop(userGrid: UserGrid): UserGrid {
        val newUserGrid = UserGrid(userGrid.rows, userGrid.grid.mapIndexed { i, cell ->
            cell.userShade = elements.last()[i]
            cell
        })
        if (elements.size > 1) {
            elements.removeAt(elements.size - 1)
        }
        return newUserGrid
    }
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