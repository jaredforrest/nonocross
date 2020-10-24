package com.example.nonocross.views.grid

import android.content.Context
import android.util.AttributeSet
import android.view.ViewGroup
import com.example.nonocross.GameEngine

//class GameView(context: Context, attributeSet: AttributeSet): ViewGroup(context, attributeSet) {
class GameView @JvmOverloads constructor(
        context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0
    ) : ViewGroup(context, attrs, defStyleAttr){

    private val gridData = GameEngine.createGrid(context)

    init {
        val colNumsView = ColNumsView(context, gridData)
        this.addView(colNumsView, 0)
        val rowNumsView = RowNumsView(context, gridData)
        this.addView(rowNumsView, 1)
        val nonocrossGridView = GridView(context, attrs, defStyleAttr, gridData)
        nonocrossGridView.setBackgroundColor(0xFF444444.toInt())
        this.addView(nonocrossGridView, 2)
    }

    override fun onLayout(changed: Boolean, l: Int, t: Int, r: Int, b: Int) {
        val minPadding = this.measuredWidth / 20

        val cellLength = ((this.measuredWidth - (minPadding*2)) - gridData.cols - 1 - 2 * ((gridData.cols - 1) / 5)) / (gridData.cols + (gridData.getLongestRowNum()+1)/2)
        val gridWidth  = cellLength * gridData.cols + gridData.cols + 1 + 2 * ((gridData.cols - 1) / 5)
        val gridHeight = cellLength * gridData.rows + gridData.rows + 1 + 2 * ((gridData.rows - 1) / 5)

        // First draw the column numbers

        // Finds longest column * half of cell length
        val colNumHeight = gridData.getLongestColNum() * (cellLength * 0.75F ).toInt()

        // (Finds longest row + 1) * half of cell length
        val rowNumWidth = gridData.getLongestRowNum() * (cellLength * 0.5F).toInt()

        val topPadding = (this.measuredHeight - colNumHeight - gridHeight) / 3
        val leftPadding = (this.measuredWidth - rowNumWidth - gridWidth) / 2

        val colNumsView = getChildAt(0)

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