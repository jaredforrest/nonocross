package com.picross.nonocross.util.usergrid

import android.graphics.Canvas
import android.graphics.Paint
import arrow.core.*
import com.picross.nonocross.util.CellShade
import com.picross.nonocross.util.click
import com.picross.nonocross.views.grid.CellView
import kotlinx.collections.immutable.PersistentList
import kotlinx.collections.immutable.toPersistentList
import java.security.InvalidParameterException

class UserGridView(width: Int, height: Int, cellLength: Int, paintEmpty: Paint, paintShade: Paint, paintCross: Paint) {

    private val gridView = List(width * height) {
        CellView(
            it / width,
            it % width,
            cellLength,
            paintEmpty,
            paintShade,
            paintCross
        )
    }

    /** Returns position of Cell which contains coordinate */
    fun getCellAt(x: Float, y: Float): Option<Int> {
        val cellView = this.gridView.indexOfFirst { cellView -> cellView.isInside(x, y) }
        return if (cellView != -1) {
            Some(cellView)
        } else none()
    }

    fun isInside(index: Int, x: Float, y: Float) = gridView[index].isInside(x, y)

    fun draw(userGrid: List<CellShade>, canvas: Canvas) {
        for(i in gridView.indices)
            gridView[i].draw(userGrid[i], canvas)
    }

}

/** UserGrid is a 1D list encoding a 2D array (the grid) */
class UserGrid(private val gridData: GridData, initialState: ByteArray = byteArrayOf(), private val autoFill: Boolean,  resetComplete: Boolean) {

    var complete: Boolean
    var timeElapsed: UInt

    private val height = gridData.height
    val width = gridData.width
    val size = gridData.width * gridData.height

    var grid: PersistentList<CellShade>

    var rowNums : List<List<Int>>//get() = data.getRowNums(gridData.height)
    var colNums : List<List<Int>>//get() = data.getColNums(gridData.height)

    init {
        /** No save */
        if (initialState.isEmpty()) {
            timeElapsed = 0u
            complete = false
            grid = List(size) { CellShade.EMPTY }.toPersistentList()
            if(autoFill) {
                autoFill()
            }
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
            if(resetComplete && complete) {
                complete = false
                grid = List(size) { CellShade.EMPTY }.toPersistentList()
                timeElapsed = 0u
            } else {
                grid = initialState.drop(6).dropLast(1).map {
                    when (it) {
                        0b00.toByte() -> CellShade.EMPTY
                        0b01.toByte() -> CellShade.SHADE
                        0b10.toByte() -> CellShade.CROSS
                        else -> CellShade.EMPTY
                    }
                }.toPersistentList()
            }
            /** Version 1 saves */
        } else {
            timeElapsed = 0u//There is an overflow bug initialState.first().toUInt()
            complete = false
            grid = initialState.drop(1).map {
                when (it) {
                    0x00.toByte() -> CellShade.EMPTY
                    0x01.toByte() -> CellShade.SHADE
                    0x10.toByte() -> CellShade.CROSS
                    else -> CellShade.EMPTY
                }
            }.toPersistentList()
        }
        rowNums = grid.getRowNums(gridData.height)
        colNums = grid.getColNums(gridData.height)
    }

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
                    when (it) {
                        CellShade.EMPTY -> 0b00.toByte()
                        CellShade.SHADE -> 0b01.toByte()
                        CellShade.CROSS -> 0b10.toByte()
                    }
                } + listOf(2)

    fun checkDone(): Boolean {
        return rowNums == gridData.rowNums && colNums == gridData.colNums
    }

    fun clear() {
        grid = List(size) { CellShade.EMPTY }.toPersistentList()

        if(autoFill) {
            autoFill()
        }
        undoAddStack()
        rowNums = grid.getRowNums(gridData.height)
        colNums = grid.getColNums(gridData.height)
    }

    fun superClear() {
        clear()
        undoList = UndoList(none(), grid, none())
        timeElapsed = 0u
        complete = false
    }

    private var undoList = UndoList(none(), grid, none())

    fun undo(): Either<UndoListException.NoMoreUndo, Unit> {
        return when (val _undoList = undoList.undo()) {
            is Either.Left -> _undoList
            is Either.Right -> {
                undoList = _undoList.value
                grid = undoList.data
                rowNums = grid.getRowNums(gridData.height)
                colNums = grid.getColNums(gridData.height)
                Either.Right(Unit)
            }
        }
    }

    fun redo(): Either<UndoListException.NoMoreRedo, Unit> {
        return when (val _undoList = undoList.redo()) {
            is Either.Left -> _undoList
            is Either.Right -> {
                undoList = _undoList.value
                grid = undoList.data
                rowNums = grid.getRowNums(gridData.height)
                colNums = grid.getColNums(gridData.height)
                Either.Right(Unit)
            }
        }
    }

    fun undoAddStack() {
        undoList = undoList.addToList(grid)
    }

    private fun fillRow(row: Int, cellShade: CellShade) =
        copyInSlice(row * width until (row + 1) * width, cellShade, CellShade.EMPTY, 0)

    private fun fillCol(col: Int, cellShade: CellShade) =
        copyInSlice(col until col + height * width step width, cellShade, CellShade.EMPTY, 0)

    fun copyRowInRange(
        index1: Int,
        index2: Int,
        initShade: CellShade,
        mode: Int
    ) {
        val initCol = index1 % width
        val currCol = index2 % width
        val row = index1 / width
        val cellShade = grid[index1]
        val startCol = initCol.coerceAtMost(currCol)
        val endCol = initCol.coerceAtLeast(currCol)
        val slice = startCol + row * width..endCol + row * width
        copyInSlice(slice, cellShade, initShade, mode)
    }

    /** Fills a the cells in a column from initRow to currRow (both inclusive) */
    fun copyColInRange(
        index1: Int,
        index2: Int,
        initShade: CellShade,
        mode: Int
    ) {
        val initRow = index1 / width
        val currRow = index2 / width
        val col = index1 % width
        val cellShade = grid[index1]
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
            for(index in slice) {
                grid = grid.set(index, cellShade)
            }
        } else if (mode == 1) {
            for(index in slice){
                grid = grid.set(index, (if (grid[index] == CellShade.EMPTY) cellShade else grid[index]))
            }
        } else {
            for(index in slice){
                grid = grid.set(index, if (grid[index] == initShade) cellShade else grid[index])
            }
        }
        rowNums = grid.getRowNums(gridData.height)
        colNums = grid.getColNums(gridData.height)
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

    fun click(index: Int, toggleCross: Boolean) {
        grid = grid.set(index,grid[index].click(toggleCross))
        rowNums = grid.getRowNums(gridData.height)
        colNums = grid.getColNums(gridData.height)
    }

    fun countSecond() {
        if (!(complete))
            timeElapsed++
    }

    fun getShade(index: Int) = grid[index]

    fun copyShade(fromIndex: Int, toIndex: Int) {
        grid = grid.set(toIndex, grid[fromIndex])
        rowNums = grid.getRowNums(gridData.height)
        colNums = grid.getColNums(gridData.height)
    }

    fun sameRow(index1: Int, index2: Int) = (index1 / width == index2 / width)


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
    val data: PersistentList<CellShade>,
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

    fun addToList(data: PersistentList<CellShade>) = UndoList(Some(this), data, none())
}

sealed class UndoListException {
    object NoMoreRedo : UndoListException()
    object NoMoreUndo : UndoListException()
}
