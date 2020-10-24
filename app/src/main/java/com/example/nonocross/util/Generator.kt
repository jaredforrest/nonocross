package com.example.nonocross.util

import android.content.Context
import android.util.Log
import com.example.nonocross.LevelDetails
import java.io.InputStream

data class GridData(val grid: List<List<Int>>, val rowNums: List<List<Int>>, val colNums: List<List<Int>>){
    val rows get() = this.grid.size
    val cols get() = this.grid[0].size
    val getLongestRowNum = {rowNums.fold(0, { acc, i -> (i.size + if(i.count{it >= 10}>0) 1 else 0 ).coerceAtLeast(acc)})}
    val getLongestColNum = {colNums.fold(0, { acc, i -> i.size.coerceAtLeast(acc)})}
}

fun getAllLevels(context: Context): List<String>{
    val files = context.resources.assets.list("levels/")?.toList()
    return files ?: listOf()
}

fun generate(context: Context): GridData {

    //Log.d("All levels", getAllLevels(context).toString())

    val text = openGridFile(context)

    // Count height of column (number of rows) and width of row (number of columns)
    val (rows, cols) = getRowsAndCols(text)

    // +1 to account for \n
    val grid = List(rows) { i -> List(cols) { j -> text[i * (cols + 1) + j].toInt() - '0'.toInt() }}

    val rowNums = List(rows) { i -> countRowNums(grid[i]) }

    val colNums = List(cols) { i -> countColNums(grid, i) }

    Log.d("longcol", colNums.fold(0, { acc, i -> i.size.coerceAtLeast(acc)}).toString())

    return GridData(grid, rowNums, colNums)
}

fun getRowsAndCols(textGrid: String): Pair<Int, Int>{
    val rows = textGrid.count {it == '\n'} + 1
    val cols = textGrid.count {it != '\n'} / rows
    return Pair(rows, cols)
}

private fun countRowNums(row: List<Int>): List<Int> {
    val rowNum = mutableListOf<Int>()
    var currNum = 0
    for (element in row){
        if (element == 1) {
            currNum++
        } else if(currNum != 0){
            rowNum.add(currNum)
            currNum = 0
        }
    }
    if(currNum != 0){
        rowNum.add(currNum)
    }

    return rowNum
}

private fun countColNums(grid: List<List<Int>>,currCol: Int): List<Int> {
    val rowNum = mutableListOf<Int>()
    var currNum = 0
    for (row in grid){
        val element = row[currCol]
        if (element == 1) {
            currNum++
        } else if(currNum != 0){
            rowNum.add(currNum)
            currNum = 0
        }
    }
    if(currNum != 0){
        rowNum.add(currNum)
    }

    return rowNum
}

fun openGridFile(context: Context, chosenLevelName: String = LevelDetails.levelName): String {
    val inputStream: InputStream = context.resources.assets.open("levels/$chosenLevelName")
    val size: Int = inputStream.available()
    val buffer = ByteArray(size)
    inputStream.read(buffer)
    return String(buffer)
}