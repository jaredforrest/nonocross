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

class Cell(
    _left: Int,
    _top: Int,
    private val cellLength: Int,
    private val bigPadding: Int
) {

    // Each Cell has at least 0.5px padding but some have 1.5px
    enum class BigPadding(val flag: Int) {
        LEFT(0x8), TOP(0x4), RIGHT(0x2), BOTTOM(0x1)
    }

    private var top = _top.toFloat()
    private var left = _left.toFloat()
    private val right = left + cellLength
    private val bottom = top + cellLength
    var userShading = 0

    private val whitePaint: Paint = Paint().apply { setARGB(255, 255, 255, 255) }
    private val bluePaint: Paint = Paint().apply { setARGB(255, 18, 134, 255) }
    private val redPaint: Paint = Paint().apply { setARGB(255, 239, 51, 64) }
        .apply { strokeCap = Paint.Cap.ROUND }
        .apply { isAntiAlias = true }

    fun draw(canvas: Canvas) {
        when (userShading) {
            0 -> drawCell(canvas, whitePaint)
            1 -> drawCell(canvas, bluePaint)
            -1 -> drawCross(canvas)
        }
    }

    private fun drawCell(canvas: Canvas, paint: Paint) {
        canvas.drawRect(left, top, right, bottom, paint)
    }

    private fun drawCross(canvas: Canvas) {
        drawCell(canvas, whitePaint)
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

    fun click(lambda: (Int) -> Int): Int {
        userShading = lambda(userShading)
        return userShading
    }
}