package com.picross.nonocross.util.usergrid

import arrow.core.Either
import com.picross.nonocross.util.CellShade
import com.picross.nonocross.util.click
import kotlinx.collections.immutable.PersistentList
import kotlinx.collections.immutable.toPersistentList

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
        undoList.reset(grid)
        timeElapsed = 0u
        complete = false
    }

    private val undoList = UndoList(mutableListOf(), grid, mutableListOf())

    fun undo(): Either<UndoListException.NoMoreUndo, Unit> {
        return when (val _undoList = undoList.undo()) {
            is Either.Left -> _undoList
            is Either.Right -> {
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
                grid = undoList.data
                rowNums = grid.getRowNums(gridData.height)
                colNums = grid.getColNums(gridData.height)
                Either.Right(Unit)
            }
        }
    }

    fun undoAddStack() {
        undoList.addToList(grid)
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
