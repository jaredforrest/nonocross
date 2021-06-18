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
        nonoGrid.grid.forEach { it.draw(canvas) }
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        when (event.actionMasked) {
            MotionEvent.ACTION_DOWN -> initializeFill(event.x, event.y)
            MotionEvent.ACTION_MOVE -> startFill(event.x, event.y)
            MotionEvent.ACTION_UP -> endFill()
            MotionEvent.ACTION_CANCEL -> handler.removeCallbacks(mLongPressed)
        }
        return true
    }

    var cellLength = 0

    // Get Preferences
    private val sharedPreferences: SharedPreferences =
        PreferenceManager.getDefaultSharedPreferences(context)
    private val fatFingerMode = sharedPreferences.getBoolean("fatFinger", true)
    private val autoCross = sharedPreferences.getBoolean("autoCross", false)
    private val vibrateOn = sharedPreferences.getBoolean("vibrate", false)

    // Create undo stack
    private val undoStack = UndoStack(gridData.rows * gridData.cols)

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
            undoStack.push(nonoGrid.data)

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
                    if (fillHori) nonoGrid.copyRowInRange(
                        fC.row,
                        fC.col,
                        cell.col,
                        fC.userShade
                    )
                    else nonoGrid.copyColInRange(
                        fC.col,
                        fC.row,
                        cell.row,
                        fC.userShade
                    )
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
        val grid = UserGrid(gridData.rows, List(gridData.rows * gridData.cols) { index ->
            Cell(index / gridData.cols, index % gridData.cols, cellLength, context)
        })
        return if (autoCross) autoFill(grid) else grid
    }

    private fun autoFill(grid: UserGrid): UserGrid {
        gridData.rowNums.forEachIndexed { i, rowNum ->
            if (rowNum == emptyList<Int>()) grid.crossRow(i)
            else if (rowNum == listOf(gridData.cols)) grid.fillRow(i)
        }
        gridData.colNums.forEachIndexed { i, colNum ->
            if (colNum == emptyList<Int>()) grid.crossCol(i)
            else if (colNum == listOf(gridData.rows)) grid.fillCol(i)
        }
        return grid
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
        nonoGrid.data = undoStack.pop()
        invalidate()
    }

    fun clear() {
        undoStack.push(nonoGrid.data)
        nonoGrid.clear()
        invalidate()
    }

}