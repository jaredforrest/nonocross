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
import androidx.core.content.res.ResourcesCompat
import com.picross.nonocross.R
import com.picross.nonocross.util.Cell

class CellView(
    val cell: Cell,
    val row: Int,
    val col: Int,
    private val cellLength: Int,
    context: Context
) {
    private val top = (1 + row * (cellLength + 1) + 2 * (row / 5)).toFloat()
    private val left = (1 + col * (cellLength + 1) + 2 * (col / 5)).toFloat()
    private val right = left + cellLength
    private val bottom = top + cellLength

    private val topBound = top - if (col % 5 == 4) 1.5 else 0.5
    private val leftBound = left - if (row % 5 == 4 && col != 0) 1.5 else 0.5
    private val rightBound = right + if (row % 5 == 4) 1.5 else 0.5
    private val bottomBound = bottom - if (col % 5 == 4 && col != 0) 1.5 else 0.5

    private val colorCross = ResourcesCompat.getColor(context.resources, R.color.colorCross, null)
    private val colorShade = ResourcesCompat.getColor(context.resources, R.color.colorShade, null)
    private val colorEmpty = ResourcesCompat.getColor(context.resources, R.color.colorEmpty, null)

    private val paintEmpty = Paint().apply { color = colorEmpty }
    private val paintShade = Paint().apply { color = colorShade }
    private val paintCross = Paint().apply { color = colorCross }
        .apply { strokeCap = Paint.Cap.ROUND }
        .apply { isAntiAlias = true }

    fun draw(canvas: Canvas) {
        when (cell.userShade) {
            Cell.CellShade.EMPTY -> canvas.drawRect(left, top, right, bottom, paintEmpty)
            Cell.CellShade.SHADE -> canvas.drawRect(left, top, right, bottom, paintShade)
            Cell.CellShade.CROSS -> drawCross(canvas)
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