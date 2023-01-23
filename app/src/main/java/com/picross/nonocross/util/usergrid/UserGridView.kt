package com.picross.nonocross.util.usergrid

import android.graphics.Canvas
import android.graphics.Paint
import arrow.core.Option
import arrow.core.Some
import arrow.core.none
import com.picross.nonocross.util.CellShade
import com.picross.nonocross.views.grid.CellView

class UserGridView(width: Int, height: Int, cellLength: Int, paintEmpty: Paint, paintShade: Paint, paintCross: Paint) {

    private val gridView = List(width * height) {
        CellView(
            it / width,
            it % width,
            cellLength,
            paintEmpty,
            paintShade,
            paintCross
        )
    }

    /** Returns position of Cell which contains coordinate */
    fun getCellAt(x: Float, y: Float): Option<Int> {
        val cellView = this.gridView.indexOfFirst { cellView -> cellView.isInside(x, y) }
        return if (cellView != -1) {
            Some(cellView)
        } else none()
    }

    fun isInside(index: Int, x: Float, y: Float) = gridView[index].isInside(x, y)

    fun draw(userGrid: List<CellShade>, canvas: Canvas) {
        for(i in gridView.indices)
            gridView[i].draw(userGrid[i], canvas)
    }

}

