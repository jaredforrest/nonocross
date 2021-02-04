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
import com.picross.nonocross.LevelDetails
import com.picross.nonocross.R

class Cell(
    _left: Int,
    _top: Int,
    private val cellLength: Int,
    private val bigPadding: Int,
    context: Context
) {

    // Each Cell has at least 0.5px padding but some have 1.5px
    enum class BigPadding(val flag: Int) {
        LEFT(0x8), TOP(0x4), RIGHT(0x2), BOTTOM(0x1)
    }

    private val top = _top.toFloat()
    private val left = _left.toFloat()
    private val right = left + cellLength
    private val bottom = top + cellLength

    //var userShading = 0
    var userShading = CellShading.EMPTY

    private val colorCrossed =
        ResourcesCompat.getColor(context.resources, R.color.colorCrossed, null)
    private val colorShaded = ResourcesCompat.getColor(context.resources, R.color.colorShaded, null)
    private val colorEmpty = ResourcesCompat.getColor(context.resources, R.color.colorEmpty, null)

    private val redPaint: Paint = Paint().apply { color = colorCrossed }
        .apply { strokeCap = Paint.Cap.ROUND }
        .apply { isAntiAlias = true }

    fun draw(canvas: Canvas) {
        when (userShading) {
            CellShading.EMPTY -> drawCell(canvas, colorEmpty)
            CellShading.SHADED -> drawCell(canvas, colorShaded)
            CellShading.CROSSED -> drawCross(canvas)
        }
    }

    private fun drawCell(canvas: Canvas, cellColor: Int) {
        val paint = Paint().apply { color = cellColor }
        canvas.drawRect(left, top, right, bottom, paint)
    }

    private fun drawCross(canvas: Canvas) {
        drawCell(canvas, colorEmpty)
        redPaint.strokeWidth = cellLength / 15F
        canvas.drawLine(
            left + cellLength * 0.25F,
            top + cellLength * 0.25F,
            left + cellLength * 0.75F,
            top + cellLength * 0.75F,
            redPaint
        )
        canvas.drawLine(
            left + cellLength * 0.25F,
            top + cellLength * 0.75F,
            left + cellLength * 0.75F,
            top + cellLength * 0.25F,
            redPaint
        )
    }

    fun isInside(x: Float, y: Float): Boolean {
        return x in (left - if (bigPadding and BigPadding.LEFT.flag != 0) 1.5 else 0.5)..
                (right + if (bigPadding and BigPadding.RIGHT.flag != 0) 1.5 else 0.5) &&
                y in (top - if (bigPadding and BigPadding.TOP.flag != 0) 1.5 else 0.5)..
                (bottom - if (bigPadding and BigPadding.BOTTOM.flag != 0) 1.5 else 0.5)
    }

    fun click(isLongClick: Boolean) {
        userShading = if (!(isLongClick.xor(LevelDetails.toggleCross))) {
            when (userShading) {
                CellShading.CROSSED, CellShading.SHADED -> CellShading.EMPTY
                CellShading.EMPTY -> CellShading.SHADED
            }
        } else {
            when (userShading) {
                CellShading.CROSSED -> CellShading.EMPTY
                CellShading.EMPTY, CellShading.SHADED -> CellShading.CROSSED
            }
        }
    }
}

enum class CellShading {
    CROSSED,
    SHADED,
    EMPTY
}