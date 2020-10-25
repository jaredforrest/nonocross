package com.example.nonocross.util

import android.content.Context
import com.example.nonocross.LevelDetails
import java.io.InputStream

data class GridData(
    val grid: List<List<Int>>,
    val rowNums: List<List<Int>>,
    val colNums: List<List<Int>>
) {
    val rows get() = this.grid.size
    val cols get() = this.grid[0].size
    val longestRowNum
        get() = rowNums.fold(
            0,
            { acc, i -> (i.size + if (i.count { it >= 10 } > 0) 1 else 0).coerceAtLeast(acc) })
    val longestColNum get() = colNums.fold(0, { acc, i -> i.size.coerceAtLeast(acc) })
}

fun generate(context: Context) {

    // rowsCols.first = No. of rows, rowsCols.second = No. of columns
    val rowsCols: Pair<Int, Int>
    val grid: List<List<Int>>

    if (LevelDetails.isRandom) {
        rowsCols = LevelDetails.randomGridRowsCols
        // Difficulty is set by changing the proportion of filled to empty cell
        // ie. difficulty=5 -> listOf(0,1,1,1,1,1)
        // difficulty=10 -> listOf(0,1)
        val difficultyList = List(12 - LevelDetails.difficulty) { i -> if (i != 0) 1 else 0 }

        grid = List(rowsCols.first) { List(rowsCols.second) { difficultyList.random() } }
    } else {
        val text = openGridFile(context, LevelDetails.levelName)

        // Count height of column (number of rows) and width of row (number of columns)
        rowsCols = getRowsAndCols(text)

        // +1 to account for \n
        grid =
            List(rowsCols.first) { i -> List(rowsCols.second) { j -> text[i * (rowsCols.second + 1) + j].toInt() - '0'.toInt() } }
    }
    val rowNums = List(rowsCols.first) { i -> countRowNums(grid[i]) }

    val colNums = List(rowsCols.second) { i -> countColNums(grid, i) }

    LevelDetails.gridData = GridData(grid, rowNums, colNums)
}

fun openGridFile(context: Context, chosenLevelName: String): String {
    val inputStream: InputStream = context.resources.assets.open("levels/$chosenLevelName")
    val size: Int = inputStream.available()
    val buffer = ByteArray(size)
    inputStream.read(buffer)
    return String(buffer)
}

fun getRowsAndCols(textGrid: String): Pair<Int, Int> {
    val rows = textGrid.count { it == '\n' } + 1
    val cols = textGrid.count { it != '\n' } / rows
    return Pair(rows, cols)
}

fun countRowNums(row: List<Int>): List<Int> {
    val rowNum = mutableListOf<Int>()
    var currNum = 0
    for (element in row) {
        if (element == 1) {
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

fun countColNums(grid: List<List<Int>>, currCol: Int): List<Int> {
    val rowNum = mutableListOf<Int>()
    var currNum = 0
    for (row in grid) {
        val element = row[currCol]
        if (element == 1) {
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

fun getAllLevels(context: Context): List<String> {
    return context.resources.assets.list("levels")?.toList() ?: listOf()
}