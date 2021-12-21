package com.picross.nonocross.util.usergrid

import android.util.Log
import arrow.core.Option
import arrow.core.Some
import arrow.core.none
import com.picross.nonocross.util.CellShade

/** Helper functions GridData*/

/** If it is unique return a GridData of it otherwise return None() */
fun List<CellShade>.checkUnique(height: Int): Option<GridData> {
    val rowNums = this.getRowNums(height)
    val colNums = this.getColNums(height)
    /* Convert the grid to a string jsolve.c can parse */
    val ast =
        rowNums.fold("")
        { acc, i -> acc + i.fold("") { acc2, j -> "$acc2$j " } + "0\n" } + "#\n" +
                colNums.fold("")
                { acc, i -> acc + i.fold("") { acc2, j -> "$acc2$j " } + "0\n" } + "#\n"

    Log.d("sfsf", ast)
    val check = CheckUnique.checkUniqueFromJNI(ast) == 1

    return if (check) Some(GridData(size / height, height, rowNums, colNums))
    else none<GridData>()
}

fun List<CellShade>.toGridData(height: Int): GridData {
    val rowNums = this.getRowNums(height)
    val colNums = this.getColNums(height)
    return GridData(size / height, height, rowNums, colNums)
}

fun List<CellShade>.getRowNums(height: Int) =
    List(height) { i -> this.row(i, height).countCellNums() }

fun List<CellShade>.getColNums(height: Int): List<List<Int>> {
    val width = if (height == 0) 0 else size / height
    return List(width) { i -> this.col(i, height).countCellNums() }
}


private fun List<CellShade>.countCellNums(): List<Int> {
    return (this.runningFold(0) { acc, cell ->
        if (cell == CellShade.SHADE) acc + 1
        else 0
    } + 0)
        .zipWithNext { a, b -> if (b == 0) a else 0 }
        .filterNot { it == 0 }
}

private fun List<CellShade>.row(index: Int, height: Int): List<CellShade> {
    val width = if (height == 0) 0 else size / height
    return this.subList(index * width, index * width + width)
}

private fun List<CellShade>.col(index: Int, height: Int): List<CellShade> {
    val width = if (height == 0) 0 else size / height
    return List(height) { j -> this[j * width + index] }
}


/** This object is purely to interact with the native code */
object CheckUnique {

    external fun checkUniqueFromJNI(string: String): Int

    init {
        System.loadLibrary("jsolve")
    }
}
