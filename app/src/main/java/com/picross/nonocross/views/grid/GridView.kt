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
import com.picross.nonocross.util.UndoStack
import com.picross.nonocross.util.UserGrid
import com.picross.nonocross.util.vibrate
import com.picross.nonocross.LevelDetails as LD
import com.picross.nonocross.LevelDetails.userGrid as nonoGrid

class GridView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0
) : View(context, attrs, defStyleAttr) {

    override fun onLayout(changed: Boolean, left: Int, top: Int, right: Int, bottom: Int) {
        super.onLayout(changed, left, top, right, bottom)
        nonoGrid = initializeGrid()
        fC = nonoGrid[0, 0]
        aC = nonoGrid[0, 0]
    }

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)
        drawCells(canvas, nonoGrid)
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        when (event.actionMasked) {
            MotionEvent.ACTION_DOWN -> initializeFill(event.x, event.y)
            MotionEvent.ACTION_UP -> endFill()
            MotionEvent.ACTION_MOVE -> startFill(event.x, event.y)
            MotionEvent.ACTION_CANCEL -> handler.removeCallbacks(mLongPressed)
        }
        return true
    }

    private var isFirstCell = true
    private var isLongPress = false

    /** First cell */
    private lateinit var fC: Cell

    /** Active cell */
    private lateinit var aC: Cell

    /** If true fill horizontally, false fill vertically */
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
        // Only run if current cell has moved
        if (!aC.isInside(x, y)) {
            if (isFirstCell) {
                isFirstCell = false
                handler.removeCallbacks(mLongPressed)
                if (!isLongPress) fC.click(!LD.toggleCross)
                invalidate()

                nonoGrid.getCellAt(x, y) { cell -> fillHori = (cell.row == fC.row) }
            }
            nonoGrid.getCellAt(x, y) { cell ->
                if (!fatFingerMode) {
                    cell.userShade = fC.userShade
                } else {
                    if (fillHori) nonoGrid[fC.row, cell.col].userShade = fC.userShade
                    else nonoGrid[cell.row, fC.col].userShade = fC.userShade
                }
                invalidate()
                aC = cell
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
        val cell = this.grid.find { cell -> cell.isInside(x, y) }
        if (cell != null) {
            action(cell)
        }
    }

    private fun initializeGrid(): UserGrid {
        return UserGrid(gridData.rows, List(gridData.rows * gridData.cols) { index ->
            Cell(index / gridData.cols, index % gridData.cols, cellLength, context)
        })
    }

    private fun drawCells(canvas: Canvas, nonoGrid: UserGrid) {
        nonoGrid.grid.forEach { cell -> cell.draw(canvas) }
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
        return nonoGrid.rowNums == gridData.rowNums && nonoGrid.colNums == gridData.colNums
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
        (context as AppCompatActivity).recreate()
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