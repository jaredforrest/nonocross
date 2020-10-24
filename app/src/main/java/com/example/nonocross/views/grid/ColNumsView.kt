package com.example.nonocross.views.grid

import android.annotation.SuppressLint
import android.content.Context
import android.graphics.Canvas
import android.graphics.Paint
import android.view.View
import com.example.nonocross.util.GridData

@SuppressLint("ViewConstructor")
class ColNumsView(context: Context, private val gridData: GridData): View(context) {

    private val blackPaint: Paint = Paint().apply { setARGB(255, 0, 0, 0) }. apply { isAntiAlias = true }. apply { textAlign = Paint.Align.CENTER }

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)

        val cellLength = (this.measuredWidth - gridData.cols - 1 - 2 * ((gridData.cols - 1) / 5)) / gridData.cols
        blackPaint. apply { textSize = cellLength * 0.5F}
        var curLeft = cellLength.toFloat() * 0.5F

        for ((i, col) in gridData.colNums.withIndex()){
            var curBot = this.measuredHeight.toFloat() - cellLength.toFloat() * 0.2F  //((gridData.rows + 1) / 2 - col.size) * cellLength.toFloat() * 0.75F + cellLength.toFloat() * 0.5F
            for (num in col.reversed()){
                canvas.drawText(num.toString(), curLeft, curBot, blackPaint)
                curBot -= cellLength.toFloat() * 0.75F
            }
            curLeft += if ((i % gridData.cols + 1) % 5 == 0) {
                cellLength + 3
            } else {
                cellLength + 1
            }
        }

    }

}