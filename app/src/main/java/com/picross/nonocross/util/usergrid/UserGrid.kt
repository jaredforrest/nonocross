package com.picross.nonocross.util.usergrid

import android.content.Context
import arrow.core.*
import com.picross.nonocross.util.Cell
import com.picross.nonocross.util.CellShade
import com.picross.nonocross.views.grid.CellView
import java.security.InvalidParameterException

class UserGridView(val userGrid: UserGrid, cellLength: Int, context: Context) {

    operator fun get(i: Int, j: Int) = gridView[i * userGrid.width + j]

    val gridView = userGrid.grid.mapIndexed { index, cell ->
        CellView(
            cell,
            index / userGrid.width,
            index % userGrid.width,
            cellLength,
            context
        )
    }

    fun getCellAt(x: Float, y: Float): Option<CellView> {
        val cellView = this.gridView.find { cellView -> cellView.isInside(x, y) }
        return if (cellView != null) {
            Some(cellView)
        } else none()
    }

}

/** UserGrid is a 1D list encoding a 2D array (the grid) */
class UserGrid(private val gridData: GridData, initialState: ByteArray = byteArrayOf()) {

    var complete: Boolean
    var paused = false
    var timeElapsed: UInt

    private val height = gridData.height
    val width = gridData.width
    private val size = gridData.width * gridData.height

    var grid: List<Cell>

    init {
        /** No save */
        if (initialState.isEmpty()) {
            timeElapsed = 0u
            complete = false
            grid = List(size) { Cell() }
            autoFill()
            /** Version 2 saves */
        } else if (initialState.last() == (2).toByte()) {
            timeElapsed = initialState[1].toUByte().toUInt() * 256u +
                    initialState[2].toUByte().toUInt() * 256u * 256u +
                    initialState[3].toUByte().toUInt() * 256u +
                    initialState[4].toUByte().toUInt()
            complete = when (initialState[5]) {
                0b1.toByte() -> true
                0b0.toByte() -> false
                else -> false
            }
            grid = initialState.drop(6).dropLast(1).map {
                when (it) {
                    0b00.toByte() -> Cell()
                    0b01.toByte() -> Cell(CellShade.SHADE)
                    0b10.toByte() -> Cell(CellShade.CROSS)
                    else -> Cell()
                }
            }
            /** Version 1 saves */
        } else {
            timeElapsed = initialState.first().toUInt()
            complete = false
            grid = initialState.drop(1).map {
                when (it) {
                    0x00.toByte() -> Cell()
                    0x01.toByte() -> Cell(CellShade.SHADE)
                    0x10.toByte() -> Cell(CellShade.CROSS)
                    else -> Cell(CellShade.EMPTY)
                }
            }
        }
    }

    operator fun get(i: Int, j: Int) = grid[i * width + j]

    private val data get() = grid.map { it.userShade }

    val currentState: List<Byte>
        /** First Byte is version code (2), 2-5 time elapsed, 6 whether complete, 7+ the grid, last byte also version code */
        get() = listOf(
            2,
            (timeElapsed / 256u / 256u / 256u).toUByte().toByte(),
            (timeElapsed / 256u / 256u).toUByte().toByte(),
            (timeElapsed / 256u).toUByte().toByte(),
            timeElapsed.toUByte().toByte()
        ) +
                (if (complete) 0b1 else 0b0) +
                grid.map {
                    when (it.userShade) {
                        CellShade.EMPTY -> 0b00.toByte()
                        CellShade.SHADE -> 0b01.toByte()
                        CellShade.CROSS -> 0b10.toByte()
                    }
                } + listOf(2)

    val rowNums get() = data.getRowNums(gridData.height)
    val colNums get() = data.getColNums(gridData.height)

    fun checkDone(): Boolean {
        return rowNums == gridData.rowNums && colNums == gridData.colNums
    }

    fun clear() {
        grid.forEach { it.userShade = CellShade.EMPTY }
        autoFill()
    }

    private var undoList = UndoList(none(), data, none())

    fun undo(): Either<UndoListException.NoMoreUndo, Unit> {
        return when (val _undoList = undoList.undo()) {
            is Either.Left -> _undoList
            is Either.Right -> {
                undoList = _undoList.value
                grid.forEachIndexed { i, cell ->
                    cell.userShade = undoList[i]
                }
                Either.Right(Unit)
            }
        }
    }

    fun redo(): Either<UndoListException.NoMoreRedo, Unit> {
        return when (val _undoList = undoList.redo()) {
            is Either.Left -> _undoList
            is Either.Right -> {
                undoList = _undoList.value
                grid.forEachIndexed { i, cell ->
                    cell.userShade = undoList[i]
                }
                Either.Right(Unit)
            }
        }
    }

    fun undoAddStack() {
        undoList = undoList.addToList(data)
    }

    private fun fillRow(row: Int, cellShade: CellShade) =
        copyInSlice(row * width until (row + 1) * width, cellShade, CellShade.EMPTY, 0)

    private fun fillCol(col: Int, cellShade: CellShade) =
        copyInSlice(col until col + height * width step width, cellShade, CellShade.EMPTY, 0)

    /** Fills a the cells in a Row from initCol to currCol (both inclusive) */
    fun copyRowInRange(
        row: Int,
        initCol: Int,
        currCol: Int,
        cellShade: CellShade,
        initShade: CellShade,
        mode: Int
    ) {
        val startCol = initCol.coerceAtMost(currCol)
        val endCol = initCol.coerceAtLeast(currCol)
        val slice = startCol + row * width..endCol + row * width
        copyInSlice(slice, cellShade, initShade, mode)
    }

    /** Fills a the cells in a column from initRow to currRow (both inclusive) */
    fun copyColInRange(
        col: Int,
        initRow: Int,
        currRow: Int,
        cellShade: CellShade,
        initShade: CellShade,
        mode: Int
    ) {
        val startRow = initRow.coerceAtMost(currRow)
        val endRow = initRow.coerceAtLeast(currRow)
        val slice = col + startRow * width..col + endRow * width step width
        copyInSlice(slice, cellShade, initShade, mode)
    }

    /** Mode 0 overwrite all, Mode 1 classic, Mode 2 never overwrite */
    private fun copyInSlice(
        slice: Iterable<Int>,
        cellShade: CellShade,
        initShade: CellShade,
        mode: Int
    ) {
        if (mode == 0 || (mode == 1 && cellShade == CellShade.EMPTY)) {
            grid.slice(slice).forEach { cell ->
                cell.userShade = cellShade
            }
        } else if (mode == 1) {
            grid.slice(slice).forEach { cell ->
                cell.userShade =
                    if (cell.userShade == CellShade.EMPTY) cellShade else cell.userShade
            }
        } else {
            grid.slice(slice).forEach { cell ->
                cell.userShade = if (cell.userShade == initShade) cellShade else cell.userShade
            }
        }
    }

    private fun autoFill() {
        gridData.rowNums.forEachIndexed { i, rowNum ->
            if (rowNum == emptyList<Int>()) fillRow(i, CellShade.CROSS)
            else if (rowNum == listOf(width)) fillRow(i, CellShade.SHADE)
        }
        gridData.colNums.forEachIndexed { i, colNum ->
            if (colNum == emptyList<Int>()) fillCol(i, CellShade.CROSS)
            else if (colNum == listOf(height)) fillCol(i, CellShade.SHADE)
        }
    }

    fun countSecond() {
        if (!(paused or complete))
            timeElapsed++
    }

}

data class GridData(
    val width: Int,
    val height: Int,
    val rowNums: List<List<Int>>,
    val colNums: List<List<Int>>,
) {

    init {
        if (height <= 0) throw InvalidParameterException("Height must be greater than 0")
        else if (width <= 0) throw InvalidParameterException("Width must be greater than 0")
    }

    /** Gets the width and height of the longest row and column */
    val longestRowNum =
        rowNums.fold(
            0
        ) { acc, i -> (i.size + if (i.count { it >= 10 } > 0) 1 else 0).coerceAtLeast(acc) }
    val longestColNum = colNums.fold(0) { acc, i -> i.size.coerceAtLeast(acc) }
}

/* this is like a linked list */
data class UndoList(
    val prev: Option<UndoList>,
    val data: List<CellShade>,
    val next: Option<UndoList>
) {

    operator fun get(i: Int) = data[i]

    fun undo() = when (prev) {
        is Some -> Either.Right(UndoList(prev.value.prev, prev.value.data, Some(this)))
        is None -> Either.Left(UndoListException.NoMoreUndo)
    }

    fun redo() = when (next) {
        is Some -> Either.Right(UndoList(Some(this), next.value.data, next.value.next))
        is None -> Either.Left(UndoListException.NoMoreRedo)
    }

    fun addToList(data: List<CellShade>) = UndoList(Some(this), data, none())
}

sealed class UndoListException {
    object NoMoreRedo : UndoListException()
    object NoMoreUndo : UndoListException()
}
