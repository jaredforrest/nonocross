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
import com.picross.nonocross.LevelDetails
import java.io.InputStream

fun generate(context: Context) {

    val grid = if (LevelDetails.isRandom) {
        // Difficulty is set by changing the proportion of filled to empty cell
        // ie. difficulty=5 -> listOf(0,1,1,1,1,1)
        // difficulty=10 -> listOf(0,1)
        val difficultyList =
            List(12 - LevelDetails.difficulty) { i -> if (i != 0) CellShading.SHADED else CellShading.EMPTY }

        List(LevelDetails.randomGridRowsCols.first) { List(LevelDetails.randomGridRowsCols.second) { difficultyList.random() } }
    } else {
        openGridFile(context, LevelDetails.levelName)
    }

    LevelDetails.gridData = GridData(grid)
}

fun openGridFile(context: Context, chosenLevelName: String): List<List<CellShading>> {
    val inputStream: InputStream = context.resources.assets.open("levels/$chosenLevelName")
    val buffer = ByteArray(inputStream.available())
    inputStream.read(buffer)
    val text = String(buffer)

    // Count height of column (number of rows) and width of row (number of columns)
    val rows = text.count { it == '\n' }
    val cols = text.count { it != '\n' } / rows

    return List(rows) { i -> List(cols) { j -> if (text[i * (cols + 1) + j] == '1') CellShading.SHADED else CellShading.EMPTY } }
}

fun getAllLevels(context: Context): List<String> {
    return context.resources.assets.list("levels")?.toList() ?: listOf()
}


data class GridData(val grid: List<List<CellShading>>) {
    val rows get() = this.grid.size
    val cols get() = this.grid[0].size

    // These are the numbers at the side/top of the row/column in-game
    val rowNums get() = List(grid.size) { i -> countCellNums(grid[i]) }
    val colNums get() = List(grid[0].size) { i -> countCellNums(grid.map { it[i] }) }

    /** Gets the width and height of the longest row and column */
    val longestRowNum
        get() =
            rowNums.fold(0,
                { acc, i -> (i.size + if (i.count { it >= 10 } > 0) 1 else 0).coerceAtLeast(acc) })
    val longestColNum get() = colNums.fold(0, { acc, i -> i.size.coerceAtLeast(acc) })

    private fun countCellNums(row: List<CellShading>): List<Int> {
        return (row.runningFold(0) { acc, cell ->
            if (cell == CellShading.SHADED) acc + 1
            else 0
        } + 0)
            .zipWithNext { a, b -> if (b == 0) a else 0 }
            .filterNot { it == 0 }
    }
}

enum class CellShading {
    CROSSED,
    SHADED,
    EMPTY
}

/** The cell is in the ith row and jth column like matrices */
data class CellPosition(val i: Int, val j: Int)