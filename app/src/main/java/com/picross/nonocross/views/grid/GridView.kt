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
import com.picross.nonocross.R
import com.picross.nonocross.util.*
import com.picross.nonocross.LevelDetails as LD

class GridView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0
) : View(context, attrs, defStyleAttr) {

    private val gridData = LD.gridData
    var cellLength = 0

    // Get Preferences
    private val sharedPreferences: SharedPreferences =
        PreferenceManager.getDefaultSharedPreferences(context)
    private val fatFingerMode = sharedPreferences.getBoolean("fatFinger", true)
    private val vibrateOn = sharedPreferences.getBoolean("vibrate", false)

    // Create grid of cells
    private lateinit var nonoGrid: List<List<Cell>>

    // Create undo stack
    private val undoStack = UndoStack(gridData.rows, gridData.cols)

    override fun onDraw(canvas: Canvas) {
        if (!this::nonoGrid.isInitialized) {
            nonoGrid = initializeGrid()
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
                if (!nonoGrid[cellPos.i][cellPos.j].isInside(event.x, event.y)) {
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

    /** first cell position */
    private var fCP = CellPosition(-1, -1)

    private var cellPos = CellPosition(-1, -1)
    private var isLongPress = false

    /** fill horizontally, if false fill vertically */
    private var fillHori = true

    private var mLongPressed = Runnable {
        nonoGrid[fCP.i][fCP.j].click(LD.toggleCross)
        invalidate()
        isLongPress = true
        if (vibrateOn) vibrate(context)
    }

    private fun initializeFill(x: Float, y: Float) {
        nonoGrid.getCellAt(x, y) { i, j ->
            handler.postDelayed(
                mLongPressed,
                ViewConfiguration.getLongPressTimeout().toLong()
            )
            undoStack.push(nonoGrid)

            cellPos = CellPosition(i, j)
            fCP = cellPos
            isFirstCell = true
            isLongPress = false
        }
    }

    private fun startFill(x: Float, y: Float) {
        if (isFirstCell) {
            isFirstCell = false
            handler.removeCallbacks(mLongPressed)
            if (!isLongPress) nonoGrid[fCP.i][fCP.j].click(!LD.toggleCross)
            invalidate()
            if (checkGridDone()) gameDoneAlert()

            nonoGrid.getCellAt(x, y) { i, _ -> fillHori = (i == fCP.i) }
        } else {
            nonoGrid.getCellAt(x, y) { i, j ->
                if (fatFingerMode) {
                    if (fillHori) {
                        nonoGrid[fCP.i][j].userShade = nonoGrid[fCP.i][fCP.j].userShade
                    } else {
                        nonoGrid[i][fCP.j].userShade = nonoGrid[fCP.i][fCP.j].userShade
                    }
                    cellPos = CellPosition(i, j)
                    invalidate()
                    if (checkGridDone()) gameDoneAlert()
                } else {
                    nonoGrid[i][j].userShade = nonoGrid[fCP.i][fCP.j].userShade
                    invalidate()
                }
            }
        }
    }

    private fun endFill() {
        handler.removeCallbacks(mLongPressed)
        if (isFirstCell and !isLongPress) {
            nonoGrid[fCP.i][fCP.j].click(!LD.toggleCross)

            invalidate()
            if (checkGridDone()) gameDoneAlert()
        }
    }


    private fun List<List<Cell>>.getCellAt(x: Float, y: Float, action: (Int, Int) -> Unit) {
        run loop@{
            this.forEachIndexed { i, row ->
                row.forEachIndexed { j, cell ->
                    if (cell.isInside(x, y)) {
                        action(i, j)
                        return@loop
                    }
                }
            }
        }
    }

    private fun initializeGrid(): List<List<Cell>> {
        return List(gridData.rows) { i ->
            List(gridData.cols) { j ->
                Cell(
                    1 + j * (cellLength + 1) + 2 * (j / 5),
                    1 + i * (cellLength + 1) + 2 * (i / 5),
                    cellLength,
                    getPadding(i, j),
                    context
                )
            }
        }
    }

    private fun drawCells(canvas: Canvas, nonocrossGrid: List<List<Cell>>) {
        nonocrossGrid.forEach { row -> row.forEach { cell -> cell.draw(canvas) } }
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
        val userGridData =
            GridData(List(nonoGrid.size) { i -> List(nonoGrid[0].size) { j -> nonoGrid[i][j].userShade } })
        return userGridData.rowNums == gridData.rowNums && userGridData.colNums == gridData.colNums
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
                resetGrid()
            }
            .show()
    }

    private fun resetGrid(){
        if (LD.isRandom) {
            // restart activity to get new random grid
            (context as AppCompatActivity).recreate()
        } else {
            // reset grid
            clear()
        }
    }

    fun undo() {
        nonoGrid = undoStack.pop(nonoGrid)
        invalidate()
    }

    fun clear() {
        undoStack.push(nonoGrid)
        nonoGrid.forEach { row ->
            row.forEach { cell ->
                cell.userShade = CellShade.EMPTY
            }
        }
        invalidate()
    }

}