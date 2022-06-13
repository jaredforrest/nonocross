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

import android.graphics.Canvas
import android.graphics.Paint
import com.picross.nonocross.util.CellShade

class CellView(
//    var cellShade: CellShade,
    row: Int,
    col: Int,
    private val cellLength: Int,
    private val paintEmpty: Paint,
    private val paintShade: Paint,
    private val paintCross: Paint
) {
    private val top = (1 + row * (cellLength + 1) + 2 * (row / 5)).toFloat()
    private val left = (1 + col * (cellLength + 1) + 2 * (col / 5)).toFloat()
    private val right = left + cellLength
    private val bottom = top + cellLength

    private val topBound = top - if (col % 5 == 4) 1.5 else 0.5
    private val leftBound = left - if (row % 5 == 4 && col != 0) 1.5 else 0.5
    private val rightBound = right + if (row % 5 == 4) 1.5 else 0.5
    private val bottomBound = bottom - if (col % 5 == 4 && col != 0) 1.5 else 0.5


    fun draw(cellShade: CellShade, canvas: Canvas) {
        when (cellShade) {
            CellShade.EMPTY -> canvas.drawRect(left, top, right, bottom, paintEmpty)
            CellShade.SHADE -> canvas.drawRect(left, top, right, bottom, paintShade)
            CellShade.CROSS -> drawCross(canvas)
        }
    }

    private fun drawCross(canvas: Canvas) {
        paintCross.apply { strokeWidth = cellLength / 15F }
        canvas.drawRect(left, top, right, bottom, paintEmpty)
        canvas.drawLine(
            left + cellLength * 0.25F,
            top + cellLength * 0.25F,
            left + cellLength * 0.75F,
            top + cellLength * 0.75F,
            paintCross
        )
        canvas.drawLine(
            left + cellLength * 0.25F,
            top + cellLength * 0.75F,
            left + cellLength * 0.75F,
            top + cellLength * 0.25F,
            paintCross
        )
    }

    fun isInside(x: Float, y: Float): Boolean {
        return x in leftBound..rightBound && y in topBound..bottomBound
    }
}