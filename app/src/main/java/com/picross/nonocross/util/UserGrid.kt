package com.picross.nonocross.util

import android.content.Context
import com.picross.nonocross.views.grid.Cell

/** UserGrid is a 1D list encoding a 2D array (the grid) */
data class UserGrid(val gridData: GridData2, val cellLength: Int, val context: Context) {

    private val rows = gridData.height

    var grid = List(gridData.height * gridData.width) { index ->
        Cell(index / gridData.width, index % gridData.width, cellLength, context)
    }

    private val size = grid.size
    private val cols = size / rows

    init {
        autoFill()
    }

    operator fun get(i: Int, j: Int) = grid[i * cols + j]

    private val data get() = grid.map { cell -> cell.userShade }

    val rowNums get() = GridData(rows, data).rowNums
    val colNums get() = GridData(rows, data).colNums

    fun checkDone(): Boolean {
        return rowNums == gridData.rowNums && colNums == gridData.colNums
    }

    fun clear() {
        grid.forEach { it.userShade = Cell.CellShade.EMPTY }
        autoFill()
    }

    private val undoStack = mutableListOf(data)

    fun undo() {
        copyCellShades(undoStack.last())
        if (undoStack.size > 1) undoStack.removeLast()
    }

    fun undoAddStack() {
        undoStack.add(data)
    }

    fun getCellAt(x: Float, y: Float, action: (Cell) -> Unit) {
        val cell = this.grid.find { cell -> cell.isInside(x, y) }
        if (cell != null) {
            action(cell)
        }
    }

    private fun fillRow(row: Int, cellShade: Cell.CellShade) =
        copyRowInRange(row, 0, cols - 1, cellShade)

    private fun fillCol(row: Int, cellShade: Cell.CellShade) =
        copyColInRange(row, 0, rows - 1, cellShade)

    /** Fills a the cells in a Row from initCol to currCol (both inclusive) */
    fun copyRowInRange(row: Int, initCol: Int, currCol: Int, cellShade: Cell.CellShade) {
        val startCol = initCol.coerceAtMost(currCol)
        val endCol = initCol.coerceAtLeast(currCol)
        grid.forEachIndexed { i, cell ->
            cell.userShade =
                if (i in (startCol + row * cols..endCol + row * cols)) cellShade else cell.userShade
        }
    }

    /** Fills a the cells in a column from initRow to currRow (both inclusive) */
    fun copyColInRange(col: Int, initRow: Int, currRow: Int, cellShade: Cell.CellShade) {
        val startRow = initRow.coerceAtMost(currRow)
        val endRow = initRow.coerceAtLeast(currRow)
        grid.forEachIndexed { i, cell ->
            cell.userShade =
                if (i in (col + startRow * cols..col + endRow * cols step cols)) cellShade else cell.userShade
        }
    }

    private fun copyCellShades(data: List<Cell.CellShade>) {
        grid.forEachIndexed { i, cell ->
            cell.userShade = data[i]
        }
    }

    private fun autoFill() {
        gridData.rowNums.forEachIndexed { i, rowNum ->
            if (rowNum == emptyList<Int>()) fillRow(i, Cell.CellShade.CROSS)
            else if (rowNum == listOf(cols)) fillRow(i, Cell.CellShade.SHADE)
        }
        gridData.colNums.forEachIndexed { i, colNum ->
            if (colNum == emptyList<Int>()) fillCol(i, Cell.CellShade.CROSS)
            else if (colNum == listOf(rows)) fillCol(i, Cell.CellShade.SHADE)
        }
    }
}

data class GridData(val height: Int, val grid: List<Cell.CellShade>) {

    private val size = grid.size
    val width = if (height == 0) 0 else size / height

    // These are the numbers at the side/top of the row/column in-game
    val rowNums = List(height) { i -> countCellNums(row(i)) }
    val colNums = List(width) { i -> countCellNums(col(i)) }


    private fun countCellNums(row: List<Cell.CellShade>): List<Int> {
        return (row.runningFold(0) { acc, cell ->
            if (cell == Cell.CellShade.SHADE) acc + 1
            else 0
        } + 0)
            .zipWithNext { a, b -> if (b == 0) a else 0 }
            .filterNot { it == 0 }
    }

    private fun row(index: Int) = grid.subList(index * width, index * width + width)
    private fun col(index: Int) = List(height) { j -> grid[j * width + index] }

    fun toGridData2() = GridData2(width, height, rowNums, colNums)
}

data class GridData2(
    val width: Int,
    val height: Int,
    val rowNums: List<List<Int>>,
    val colNums: List<List<Int>>
) {

    fun isEmpty() = (width == 0 || height == 0)

    /** Gets the width and height of the longest row and column */
    val longestRowNum =
        rowNums.fold(0,
            { acc, i -> (i.size + if (i.count { it >= 10 } > 0) 1 else 0).coerceAtLeast(acc) })
    val longestColNum = colNums.fold(0, { acc, i -> i.size.coerceAtLeast(acc) })
}
