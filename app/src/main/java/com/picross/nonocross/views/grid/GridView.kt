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
package com.picross.nonocross.views.grid

import android.app.AlertDialog
import android.content.Context
import android.content.DialogInterface
import android.content.SharedPreferences
import android.graphics.Canvas
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.View
import android.view.ViewConfiguration
import androidx.appcompat.app.AppCompatActivity
import androidx.preference.PreferenceManager
import com.picross.nonocross.LevelDetails.gridData
import com.picross.nonocross.R
import com.picross.nonocross.util.*
import com.picross.nonocross.LevelDetails as LD
import com.picross.nonocross.LevelDetails.userGrid as nonoGrid

class GridView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0
) : View(context, attrs, defStyleAttr) {

    private var firstDraw = true
    var cellLength = 0

    // Get Preferences
    private val sharedPreferences: SharedPreferences =
        PreferenceManager.getDefaultSharedPreferences(context)
    private val fatFingerMode = sharedPreferences.getBoolean("fatFinger", true)
    private val vibrateOn = sharedPreferences.getBoolean("vibrate", false)

    // Create undo stack
    private val undoStack = UndoStack(gridData.rows, gridData.cols)

    override fun onDraw(canvas: Canvas) {
        if (firstDraw) {
            nonoGrid = initializeGrid()
            firstDraw = false
        }
        super.onDraw(canvas)
        drawCells(canvas, nonoGrid)
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        when (event.actionMasked) {
            MotionEvent.ACTION_DOWN -> {
                initializeFill(event.x, event.y)
            }
            MotionEvent.ACTION_UP -> {
                endFill()
            }

            MotionEvent.ACTION_MOVE -> {
                // Only run if current cell has moved
                if (!aC.isInside(event.x, event.y)) {
                    startFill(event.x, event.y)
                }
            }
            MotionEvent.ACTION_CANCEL -> {
                handler.removeCallbacks(mLongPressed)
            }
        }
        return true
    }

    private var isFirstCell = true

    /** First cell */
    private lateinit var fC: Cell

    /** Active cell */
    private lateinit var aC: Cell
    private var isLongPress = false

    /** fill horizontally, if false fill vertically */
    private var fillHori = true

    private var mLongPressed = Runnable {
        fC.click(LD.toggleCross)
        invalidate()
        isLongPress = true
        if (vibrateOn) vibrate(context)
    }

    private fun initializeFill(x: Float, y: Float) {
        nonoGrid.getCellAt(x, y) { cell ->
            handler.postDelayed(
                mLongPressed,
                ViewConfiguration.getLongPressTimeout().toLong()
            )
            undoStack.push(nonoGrid)

            aC = cell
            fC = cell
            isFirstCell = true
            isLongPress = false
        }
    }

    private fun startFill(x: Float, y: Float) {
        if (isFirstCell) {
            isFirstCell = false
            handler.removeCallbacks(mLongPressed)
            if (!isLongPress) fC.click(!LD.toggleCross)
            invalidate()
            if (checkGridDone()) gameDoneAlert()

            nonoGrid.getCellAt(x, y) { cell -> fillHori = (cell.row == fC.row) }
        } else {
            nonoGrid.getCellAt(x, y) { cell ->
                if (fatFingerMode) {
                    if (fillHori) {
                        nonoGrid[fC.row][cell.col].userShade = fC.userShade
                    } else {
                        nonoGrid[cell.row][fC.col].userShade = fC.userShade
                    }
                    aC = cell
                    invalidate()
                    if (checkGridDone()) gameDoneAlert()
                } else {
                    cell.userShade = fC.userShade
                    invalidate()
                }
            }
        }
    }

    private fun endFill() {
        handler.removeCallbacks(mLongPressed)
        if (isFirstCell and !isLongPress) {
            fC.click(!LD.toggleCross)

            invalidate()
        }
        if (checkGridDone()) gameDoneAlert()
    }


    private fun UserGrid.getCellAt(x: Float, y: Float, action: (Cell) -> Unit) {
        run loop@{
            this.grid.forEach { row ->
                row.forEach { cell ->
                    if (cell.isInside(x, y)) {
                        action(cell)
                        return@loop
                    }
                }
            }
        }
    }

    private fun initializeGrid(): UserGrid {
        return UserGrid(List(gridData.rows) { i ->
            List(gridData.cols) { j ->
                Cell(
                    i,
                    j,
                    cellLength,
                    getPadding(i, j),
                    context
                )
            }
        }
        )
    }

    private fun drawCells(canvas: Canvas, nonoGrid: UserGrid) {
        nonoGrid.grid.forEach { row -> row.forEach { cell -> cell.draw(canvas) } }
    }

    private fun getPadding(i: Int, j: Int): Int {
        var x = 0
        if (i % 5 == 4) x += Cell.BigPadding.RIGHT.flag
        if (j % 5 == 4) x += Cell.BigPadding.TOP.flag
        if (i % 5 == 4 && j != 0) x += Cell.BigPadding.LEFT.flag
        if (j % 5 == 4 && j != 0) x += Cell.BigPadding.BOTTOM.flag
        return x
    }

    private fun checkGridDone(): Boolean {
        return nonoGrid.data.rowNums == gridData.rowNums && nonoGrid.data.colNums == gridData.colNums
    }

    /** When the game is finished show a dialog */
    private fun gameDoneAlert() {
        AlertDialog.Builder(context)
            .setTitle(R.string.finished)
            .setMessage(R.string.level_complete)
            .setPositiveButton(
                R.string.menu
            ) { _: DialogInterface, _: Int ->
                (context as AppCompatActivity).finish()
            }
            .setNegativeButton(
                R.string.reset
            ) { _: DialogInterface, _: Int ->
                if (vibrateOn) vibrate(context)
                resetGrid()
            }
            .show()
    }

    private fun resetGrid() {
        clear()
        //if (LD.isRandom) {
        // restart activity to get new random grid
        (context as AppCompatActivity).recreate()
        //}
    }

    fun undo() {
        nonoGrid = undoStack.pop(nonoGrid)
        invalidate()
    }

    fun clear() {
        undoStack.push(nonoGrid)
        nonoGrid.clear()
        invalidate()
    }

}

data class UserGrid(var grid: List<List<Cell>>) {

    fun clear() {
        grid = grid.map { row ->
            row.map { cell ->
                cell.userShade = CellShade.EMPTY
                cell
            }
        }
    }

    val data get() = GridData(grid.map { row -> row.map { cell -> cell.userShade } })

    operator fun get(i: Int) = grid[i]

}