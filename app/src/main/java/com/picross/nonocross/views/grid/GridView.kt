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
import android.graphics.Canvas
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.View
import android.view.ViewConfiguration
import androidx.appcompat.app.AppCompatActivity
import com.picross.nonocross.LevelDetails
import com.picross.nonocross.R
import com.picross.nonocross.util.countColNums
import com.picross.nonocross.util.countRowNums

class GridView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0
) : View(context, attrs, defStyleAttr) {

    private val gridData = LevelDetails.gridData

    // Create grid of cells
    private lateinit var nonocrossGrid: List<List<Cell>>

    override fun onDraw(canvas: Canvas) {
        if (!this::nonocrossGrid.isInitialized) {
            nonocrossGrid = initializeGrid()
        }
        super.onDraw(canvas)
        drawCells(canvas, nonocrossGrid)
    }

    private val shortClickFun = { userShading: Int ->
        when (userShading) {
            -1, 1 -> 0
            0 -> 1
            else -> 0
        }
    }
    private val longClickFun = { userShading: Int ->
        when (userShading) {
            -1 -> 0
            0, 1 -> -1
            else -> 0
        }
    }
    private var mLongPressed = Runnable {
        clickFun = longClickFun
    }
    private var isFirstCell = true
    private var inCell = Pair(0, 0)
    private var cellShade = 0
    private var clickFun = shortClickFun

    @SuppressLint("ClickableViewAccessibility")
    override fun onTouchEvent(event: MotionEvent): Boolean {
        when (event.actionMasked) {
            MotionEvent.ACTION_DOWN -> {
                clickFun = shortClickFun
                loop@ for ((i, row) in nonocrossGrid.withIndex()) {
                    for ((j, cell) in row.withIndex()) {
                        if (cell.isInside(event.x, event.y)) {
                            handler.postDelayed(
                                mLongPressed,
                                ViewConfiguration.getLongPressTimeout().toLong()
                            )
                            inCell = Pair(i, j)
                            isFirstCell = true
                            break@loop
                        }
                    }
                }
            }
            MotionEvent.ACTION_UP -> {
                handler.removeCallbacks(mLongPressed)
                if (isFirstCell) {
                    nonocrossGrid[inCell.first][inCell.second].click(clickFun)
                    invalidate()
                    if(checkGridDone()) gameDoneAlert()
                }
            }

            MotionEvent.ACTION_MOVE -> {
                if (!nonocrossGrid[inCell.first][inCell.second].isInside(event.x, event.y)) {
                    if (isFirstCell) {
                        handler.removeCallbacks(mLongPressed)
                        isFirstCell = false
                        cellShade = nonocrossGrid[inCell.first][inCell.second].click(clickFun)
                        invalidate()
                        if (checkGridDone()) gameDoneAlert()
                    }
                    loop@ for ((i, row) in nonocrossGrid.withIndex()) {
                        for ((j, cell) in row.withIndex()) {
                            if (cell.isInside(event.x, event.y)) {
                                inCell = Pair(i, j)
                                nonocrossGrid[inCell.first][inCell.second].userShading = cellShade
                                invalidate()
                                if (checkGridDone()) gameDoneAlert()
                                break@loop
                            }
                        }
                    }
                }
            }
            MotionEvent.ACTION_CANCEL -> {
                handler.removeCallbacks(mLongPressed)
                isFirstCell = true
                cellShade = 0
            }
        }
        return true
    }

    private fun initializeGrid(): List<List<Cell>> {
        val cellLength =
            (this.measuredWidth - gridData.cols - 1 - 2 * ((gridData.cols - 1) / 5)) / gridData.cols
        return List(gridData.rows) { i ->
            List(gridData.cols) { j ->
                Cell(
                    1 + j * (cellLength + 1) + 2 * (j / 5),
                    1 + i * (cellLength + 1) + 2 * (i / 5),
                    cellLength,
                    getPadding(i, j)
                )
            }
        }
    }

    private fun drawCells(canvas: Canvas, nonocrossGrid: List<List<Cell>>) {
        for (row in nonocrossGrid) {
            for (cell in row) {
                cell.draw(canvas)
            }
        }
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
        val userGrid =
            List(nonocrossGrid.size) { i -> List(nonocrossGrid[0].size) { j -> nonocrossGrid[i][j].userShading } }
        val rowNums = List(gridData.rows) { i -> countRowNums(userGrid[i]) }
        val colNums = List(gridData.cols) { i -> countColNums(userGrid, i) }
        return rowNums == gridData.rowNums && colNums == gridData.colNums
    }

    /** When the game is finished show a dialog */
    private fun gameDoneAlert() {
        AlertDialog.Builder(context)
            .setTitle("Finished")
            .setMessage(R.string.level_complete)
            .setPositiveButton(
                R.string.go_back
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
        if(LevelDetails.isRandom){
            // restart activity to get new random grid
            (context as AppCompatActivity).recreate()
        } else{
            // reset grid
            nonocrossGrid.forEach { row -> row.forEach { it.userShading = 0 } }
            invalidate()
        }
    }

}