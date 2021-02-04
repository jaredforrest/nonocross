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
import com.picross.nonocross.R
import com.picross.nonocross.util.CellPosition
import com.picross.nonocross.util.CellShading
import com.picross.nonocross.util.GridData
import com.picross.nonocross.LevelDetails as LD

class GridView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0
) : View(context, attrs, defStyleAttr) {

    private val gridData = LD.gridData
    var cellLength = 0

    // Create grid of cells
    private lateinit var nonocrossGrid: List<List<Cell>>

    override fun onDraw(canvas: Canvas) {
        if (!this::nonocrossGrid.isInitialized) {
            nonocrossGrid = initializeGrid()
        }
        super.onDraw(canvas)
        drawCells(canvas, nonocrossGrid)
    }

    private var mLongPressed = Runnable {
        isLongPress = true
    }
    private var isFirstCell = true
    private var cellPos = CellPosition(0, 0)
    private var cellShade = CellShading.EMPTY
    private var isLongPress = false

    @SuppressLint("ClickableViewAccessibility")
    override fun onTouchEvent(event: MotionEvent): Boolean {
        when (event.actionMasked) {
            MotionEvent.ACTION_DOWN -> {
                isLongPress = false
                loop@ for ((i, row) in nonocrossGrid.withIndex()) {
                    for ((j, cell) in row.withIndex()) {
                        if (cell.isInside(event.x, event.y)) {
                            handler.postDelayed(
                                mLongPressed,
                                ViewConfiguration.getLongPressTimeout().toLong()
                            )
                            cellPos = CellPosition(i, j)
                            isFirstCell = true
                            break@loop
                        }
                    }
                }
            }
            MotionEvent.ACTION_UP -> {
                handler.removeCallbacks(mLongPressed)
                if (isFirstCell) {
                    nonocrossGrid[cellPos.i][cellPos.j].click(!(isLongPress.xor(LD.toggleCross)))
                    invalidate()
                    if (checkGridDone()) gameDoneAlert()
                }
            }

            MotionEvent.ACTION_MOVE -> {
                if (!nonocrossGrid[cellPos.i][cellPos.j].isInside(event.x, event.y)) {
                    if (isFirstCell) {
                        handler.removeCallbacks(mLongPressed)
                        isFirstCell = false
                        nonocrossGrid[cellPos.i][cellPos.j].click(!(isLongPress.xor(LD.toggleCross)))
                        cellShade = nonocrossGrid[cellPos.i][cellPos.j].userShading
                        invalidate()
                        if (checkGridDone()) gameDoneAlert()
                    }
                    loop@ for ((i, row) in nonocrossGrid.withIndex()) {
                        for ((j, cell) in row.withIndex()) {
                            if (cell.isInside(event.x, event.y)) {
                                cellPos = CellPosition(i, j)
                                nonocrossGrid[cellPos.i][cellPos.j].userShading = cellShade
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
                cellShade = CellShading.EMPTY
            }
        }
        return true
    }

    private fun initializeGrid(): List<List<Cell>> {
        /*val cellLength =
            (this.measuredWidth - gridData.cols - 1 - 2 * ((gridData.cols - 1) / 5)) / gridData.cols*/
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
            GridData(List(nonocrossGrid.size) { i -> List(nonocrossGrid[0].size) { j -> nonocrossGrid[i][j].userShading } })
        return userGridData.rowNums == gridData.rowNums && userGridData.colNums == gridData.colNums
    }

    /** When the game is finished show a dialog */
    private fun gameDoneAlert() {
        AlertDialog.Builder(context)
            .setTitle(R.string.finished)
            .setMessage(R.string.level_complete)
            .setPositiveButton(
                R.string.main_menu
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
            nonocrossGrid.forEach { row ->
                row.forEach { cell ->
                    cell.userShading = CellShading.EMPTY
                }
            }
            invalidate()
        }
    }

}