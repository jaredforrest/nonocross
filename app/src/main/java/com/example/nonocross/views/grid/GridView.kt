package com.example.nonocross.views.grid

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
import com.example.nonocross.LevelDetails
import com.example.nonocross.R
import com.example.nonocross.util.GridData


@SuppressLint("ViewConstructor")
//class GridView(context: Context, private val gridData: GridData) : View(context) {

class GridView @JvmOverloads constructor(
        context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0, private val gridData: GridData
    ) : View(context, attrs, defStyleAttr) {

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
                        if(checkGridDone()) gameDoneAlert()
                    }
                    loop@ for ((i, row) in nonocrossGrid.withIndex()) {
                        for ((j, cell) in row.withIndex()) {
                            if (cell.isInside(event.x, event.y)) {
                                inCell = Pair(i, j)
                                nonocrossGrid[inCell.first][inCell.second].userShading = cellShade
                                invalidate()
                                if(checkGridDone()) gameDoneAlert()
                                break@loop
                            }
                        }
                    }
                }
            }
            //MotionEvent.ACTION_CANCEL -> Log.d("touch ev", "you have been cancelled")
        }
        return true
    }

    private fun initializeGrid(): List<List<Cell>> {
        val cellLength =
            (this.measuredWidth - gridData.cols - 1 - 2 * ((gridData.cols - 1) / 5)) / gridData.cols
        return List(gridData.rows) { i ->
            List(gridData.cols) { j ->
                Cell(
                    gridData.grid[i][j],
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
        for (row in nonocrossGrid) {
            for (cell in row) {
                if (!cell.checkCell()) return false
            }
        }
        return true
    }

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
                    nonocrossGrid.forEach { row -> row.forEach { it.userShading = 0 } }
                    invalidate()
                }
                .setIcon(android.R.drawable.star_big_on)
                .show()
    }

}
