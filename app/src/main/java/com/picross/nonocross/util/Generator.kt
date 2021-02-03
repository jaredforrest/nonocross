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
import com.picross.nonocross.views.grid.CellShading
import java.io.InputStream

fun generate(context: Context) {
    val grid: List<List<CellShading>>

    grid = if (LevelDetails.isRandom) {
        // Difficulty is set by changing the proportion of filled to empty cell
        // ie. difficulty=5 -> listOf(0,1,1,1,1,1)
        // difficulty=10 -> listOf(0,1)
        val difficultyList =
            List(12 - LevelDetails.difficulty) { i -> if (i != 0) CellShading.SHADED else CellShading.EMPTY }

        List(LevelDetails.randomGridRowsCols.first) { List(LevelDetails.randomGridRowsCols.second) { difficultyList.random() } }
    } else {
        openGridFile(context, LevelDetails.levelName)
    }
    val rowNums = List(grid.size) { i -> countRowNums(grid[i]) }

    val colNums = List(grid[0].size) { i -> countColNums(grid, i) }

    LevelDetails.gridData = GridData(grid, rowNums, colNums)
}

fun openGridFile(context: Context, chosenLevelName: String): List<List<CellShading>> {
    val inputStream: InputStream = context.resources.assets.open("levels/$chosenLevelName")
    val size: Int = inputStream.available()
    val buffer = ByteArray(size)
    inputStream.read(buffer)
    val text = String(buffer)

    // Count height of column (number of rows) and width of row (number of columns)
    val rows = text.count { it == '\n' }
    val cols = text.count { it != '\n' } / rows

    return List(rows) { i -> List(cols) { j -> if (text[i * (cols + 1) + j] == '1') CellShading.SHADED else CellShading.EMPTY } }
}

fun countRowNums(row: List<CellShading>): List<Int> {
    val rowNum = mutableListOf<Int>()
    var currNum = 0
    for (element in row) {
        if (element == CellShading.SHADED) {
            currNum++
        } else if (currNum != 0) {
            rowNum.add(currNum)
            currNum = 0
        }
    }
    if(currNum != 0){
        rowNum.add(currNum)
    }

    return rowNum
}

fun countColNums(grid: List<List<CellShading>>, currCol: Int): List<Int> {
    val rowNum = mutableListOf<Int>()
    var currNum = 0
    for (row in grid) {
        val element = row[currCol]
        if (element == CellShading.SHADED) {
            currNum++
        } else if (currNum != 0) {
            rowNum.add(currNum)
            currNum = 0
        }
    }
    if (currNum != 0) {
        rowNum.add(currNum)
    }

    return rowNum
}

fun getAllLevels(context: Context): List<String> {
    return context.resources.assets.list("levels")?.toList() ?: listOf()
}


data class GridData(
    val grid: List<List<CellShading>>,
    val rowNums: List<List<Int>>,
    val colNums: List<List<Int>>
) {
    val rows get() = this.grid.size
    val cols get() = this.grid[0].size

    /** Gets the width and height of the longest row and column */
    val longestRowNum
        get() =
            rowNums.fold(0,
                { acc, i -> (i.size + if (i.count { it >= 10 } > 0) 1 else 0).coerceAtLeast(acc) })
    val longestColNum get() = colNums.fold(0, { acc, i -> i.size.coerceAtLeast(acc) })
}