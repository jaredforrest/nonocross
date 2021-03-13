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

import android.content.Context
import android.graphics.Canvas
import android.graphics.Paint
import android.view.View
import androidx.core.content.res.ResourcesCompat
import com.picross.nonocross.LevelDetails
import com.picross.nonocross.R

class RowNumsView(context: Context) : View(context) {

    var cellLength = 0
    private val gridData = LevelDetails.gridData
    private val colorNumber = ResourcesCompat.getColor(context.resources, R.color.colorText, null)
    private val colorBlue = ResourcesCompat.getColor(context.resources, R.color.colorShaded, null)
    private val blackPaint = Paint()
        .apply { color = colorNumber }
        .apply { isAntiAlias = true }
        .apply { textAlign = Paint.Align.RIGHT }
    private val bluePaint = Paint()
        .apply { color = colorBlue }
        .apply { isAntiAlias = true }
        .apply { textAlign = Paint.Align.RIGHT }

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)

        /*val cellLength =
            (this.measuredHeight - gridData.rows - 1 - 2 * ((gridData.rows - 1) / 5)) / gridData.rows*/
        blackPaint.apply { textSize = cellLength * 0.5F }
        bluePaint.apply { textSize = cellLength * 0.5F }
        var curBot = cellLength * 0.75F

        for ((i, col) in gridData.rowNums.withIndex()) {
            var curLeft = this.measuredWidth.toFloat() - cellLength.toFloat() * 0.2F
            for (num in col.reversed()) {
                canvas.drawText(num.toString(), curLeft, curBot, blackPaint)
                curLeft -= if (num < 10) cellLength * 0.5F else cellLength * 0.7F
            }
            curBot += if ((i % gridData.cols + 1) % 5 == 0) {
                cellLength + 3
            } else {
                cellLength + 1
            }
        }

    }

}