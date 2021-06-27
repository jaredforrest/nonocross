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
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.ViewGroup
import com.picross.nonocross.LevelDetails.gridData

class GameView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0
) : ViewGroup(context, attrs, defStyleAttr) {

    init {
        val colNumsView = ColNumsView(context)
        this.addView(colNumsView, 0)
        val rowNumsView = RowNumsView(context)
        this.addView(rowNumsView, 1)
        val nonocrossGridView = GridView(context, attrs, defStyleAttr)
        nonocrossGridView.setBackgroundColor(0xFF444444.toInt())
        this.addView(nonocrossGridView, 2)
    }

    override fun onLayout(changed: Boolean, l: Int, t: Int, r: Int, b: Int) {
        if (changed) {
            val minPadding = this.measuredWidth / 20

            // Given the width of this View find the max width of a Cell
            val cellWidth =
                ((this.measuredWidth - (minPadding * 2)) - gridData.width - 1 - (2 * (gridData.width - 1) / 5)) / (gridData.width + (gridData.longestRowNum + 1) / 2)
            // Given the width of this View find the max Height of a Cell
            val cellHeight =
                ((this.measuredHeight - (minPadding * 2)) - gridData.height - 1 - (2 * (gridData.height - 1) / 5)) / (gridData.height + (gridData.longestColNum + 1))

            val cellLength = cellWidth.coerceAtMost(cellHeight)

            val gridWidth =
                cellLength * gridData.width + gridData.width + 1 + 2 * ((gridData.width - 1) / 5)
            val gridHeight =
                cellLength * gridData.height + gridData.height + 1 + 2 * ((gridData.height - 1) / 5)

            // First draw the column numbers

            // Finds longest column * half of cell length
            val colNumHeight = gridData.longestColNum * (cellLength * 0.75F).toInt()

            // (Finds longest row + 1) * half of cell length
            val rowNumWidth = gridData.longestRowNum * (cellLength * 0.5F).toInt()

            val topPadding = (this.measuredHeight - colNumHeight - gridHeight) / 3
            val leftPadding = (this.measuredWidth - rowNumWidth - gridWidth) / 2

            val colNumsView = getChildAt(0)
            (colNumsView as ColNumsView).cellLength = cellLength

            // Get the maximum size of the child
            colNumsView.measure(
                MeasureSpec.makeMeasureSpec(gridWidth, MeasureSpec.EXACTLY),
                MeasureSpec.makeMeasureSpec(colNumHeight, MeasureSpec.EXACTLY)
            )
            // Create the layout
            colNumsView.layout(
                leftPadding + rowNumWidth,
                topPadding,
                leftPadding + rowNumWidth + colNumsView.measuredWidth,
                topPadding + colNumsView.measuredHeight
            )

            val rowNumsView = getChildAt(1)
            (rowNumsView as RowNumsView).cellLength = cellLength

            // Get the maximum size of the child
            rowNumsView.measure(
                MeasureSpec.makeMeasureSpec(rowNumWidth, MeasureSpec.EXACTLY),
                MeasureSpec.makeMeasureSpec(gridHeight, MeasureSpec.EXACTLY)
            )

            // Create the layout
            rowNumsView.layout(
                leftPadding,
                topPadding + colNumsView.measuredHeight,
                leftPadding + rowNumsView.measuredWidth,
                topPadding + colNumsView.measuredHeight + rowNumsView.measuredHeight
            )

            // Then draw the grid

            val nonocrossGridView = getChildAt(2)
            (nonocrossGridView as GridView).cellLength = cellLength

            // Get the maximum size of the child
            nonocrossGridView.measure(
                MeasureSpec.makeMeasureSpec(gridWidth, MeasureSpec.EXACTLY),
                MeasureSpec.makeMeasureSpec(gridHeight, MeasureSpec.EXACTLY)
            )
            // Create the layout
            nonocrossGridView.layout(
                leftPadding + rowNumsView.measuredWidth,
                topPadding + colNumsView.measuredHeight,
                leftPadding + rowNumsView.measuredWidth + nonocrossGridView.measuredWidth,
                topPadding + colNumsView.measuredHeight + nonocrossGridView.measuredHeight
            )
        }
    }

    override fun onInterceptTouchEvent(event: MotionEvent): Boolean {
        super.onInterceptTouchEvent(event)
        if ((event.actionMasked == MotionEvent.ACTION_UP)) {
            getChildAt(0).invalidate()
            getChildAt(1).invalidate()
        }
        return false
    }
}