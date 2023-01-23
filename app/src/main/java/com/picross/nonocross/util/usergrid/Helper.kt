package com.picross.nonocross.util.usergrid

import com.picross.nonocross.util.CellShade

/** Helper functions GridData*/

fun List<CellShade>.toGridData(width: Int, height: Int) =
    GridData(width, height, this.getRowNums(width, height), this.getColNums(width, height))

fun List<CellShade>.getRowNums(width: Int, height: Int): List<List<Int>> {
    return List(height) { i -> this.subList(i * width, i * width + width).countCellNums() }
}

fun List<CellShade>.getColNums(width: Int, height: Int): List<List<Int>> {
    return List(width) { i -> List(height) { j -> this[j * width + i]}.countCellNums() }
}

private fun Iterable<CellShade>.countCellNums(): List<Int> {
    return (this.runningFold(0) { acc, cell ->
        if (cell == CellShade.SHADE) acc + 1
        else 0
    } + 0)
        .zipWithNext { a, b -> if (b == 0) a else 0 }
        .filterNot { it == 0 }
}
