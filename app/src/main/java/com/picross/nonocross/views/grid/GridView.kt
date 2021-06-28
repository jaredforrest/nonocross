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

import android.annotation.SuppressLint
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
import com.picross.nonocross.util.UserGridView
import com.picross.nonocross.util.generate
import com.picross.nonocross.util.vibrate
import com.picross.nonocross.LevelDetails as LD

class GridView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0
) : View(context, attrs, defStyleAttr) {

    override fun onLayout(changed: Boolean, left: Int, top: Int, right: Int, bottom: Int) {
        super.onLayout(changed, left, top, right, bottom)
        fC = nonoGrid[0, 0]
        aC = nonoGrid[0, 0]
    }

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)
        nonoGrid.gridView.forEach { it.draw(canvas) }
    }

    @SuppressLint("ClickableViewAccessibility")
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
    private val nonoGrid by lazy { UserGridView(LD.userGrid, cellLength, context) }

    // Get Preferences
    private val sharedPreferences: SharedPreferences =
        PreferenceManager.getDefaultSharedPreferences(context)
    private val fatFingerMode = sharedPreferences.getBoolean("fatFinger", true)
    private val vibrateOn = sharedPreferences.getBoolean("vibrate", false)

    private var isFirstCell = true
    private var isLongPress = false

    /** First cell */
    private lateinit var fC: CellView

    /** Active cell */
    private lateinit var aC: CellView

    /** If true fill horizontally, false fill vertically */
    private var fillHori = true

    private var mLongPressed = Runnable {
        fC.cell.click(LD.toggleCross)
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
            nonoGrid.userGrid.undoAddStack()

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
                if (!isLongPress) fC.cell.click(!LD.toggleCross)
                invalidate()

                nonoGrid.getCellAt(x, y) { cell -> fillHori = (cell.row == fC.row) }
            }
            nonoGrid.getCellAt(x, y) { currCell ->
                if (!fatFingerMode) {
                    currCell.cell.userShade = fC.cell.userShade
                } else {
                    if (fillHori) nonoGrid.userGrid.copyRowInRange(
                        fC.row,
                        fC.col,
                        currCell.col,
                        fC.cell.userShade
                    )
                    else nonoGrid.userGrid.copyColInRange(
                        fC.col,
                        fC.row,
                        currCell.row,
                        fC.cell.userShade
                    )
                }
                invalidate()
                aC = currCell
            }
        }
    }

    private fun endFill() {
        handler.removeCallbacks(mLongPressed)
        if (isFirstCell and !isLongPress) {
            fC.cell.click(!LD.toggleCross)
            invalidate()
        }
        if (nonoGrid.userGrid.checkDone()) gameDoneAlert()
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
        if (LD.isRandom) generate(context)
        clear()
        (context as AppCompatActivity).recreate()
    }

    fun undo() {
        nonoGrid.userGrid.undo()
        invalidate()
    }

    fun clear() {
        nonoGrid.userGrid.undoAddStack()
        nonoGrid.userGrid.clear()
        invalidate()
    }

}