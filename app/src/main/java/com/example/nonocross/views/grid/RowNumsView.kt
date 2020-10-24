package com.example.nonocross.views.grid

import android.annotation.SuppressLint
import android.content.Context
import android.graphics.Canvas
import android.graphics.Paint
import android.view.View
import com.example.nonocross.util.GridData

@SuppressLint("ViewConstructor")
class RowNumsView(context: Context, private val gridData: GridData): View(context) {

    private val blackPaint: Paint = Paint().apply { setARGB(255, 0, 0, 0) }. apply { isAntiAlias = true }. apply { textAlign = Paint.Align.RIGHT }

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)

        val cellLength = (this.measuredHeight - gridData.rows - 1 - 2 * ((gridData.rows - 1) / 5)) / gridData.rows
        blackPaint. apply { textSize = cellLength * 0.5F }
        var curBot = cellLength * 0.75F

        for ((i, col) in gridData.rowNums.withIndex()){
            var curLeft = this.measuredWidth.toFloat() - cellLength.toFloat() * 0.2F
            for (num in col.reversed()){
                canvas.drawText(num.toString(), curLeft, curBot, blackPaint)
                curLeft -= if(num < 10) cellLength * 0.5F else cellLength * 0.7F
            }
            curBot += if ((i % gridData.cols + 1) % 5 == 0) {
                cellLength + 3
            } else {
                cellLength + 1
            }
        }

    }

}