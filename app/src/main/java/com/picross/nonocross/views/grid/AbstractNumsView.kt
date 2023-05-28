package com.picross.nonocross.views.grid

import android.content.Context
import android.graphics.Paint
import android.graphics.Picture
import android.view.View
import com.picross.nonocross.R
import com.picross.nonocross.util.Preferences
import com.picross.nonocross.util.resolveThemedColor

abstract class AbstractNumsView(context: Context) : View(context) {
    private val preferences = Preferences(context)
    protected val showHints = preferences.showHints
    protected val enableZoom = preferences.enableZoom

    protected val paintNumber = Paint()
        .apply { color = context.resolveThemedColor(R.attr.colorOnSurface) }
        .apply { isAntiAlias = true }
        .apply { textAlign = Paint.Align.CENTER }
    protected val paintValidHint = Paint()
        .apply { color = context.resolveThemedColor(R.attr.colorShade) }
        .apply { isAntiAlias = true }
        .apply { textAlign = Paint.Align.CENTER }
    protected val paintErrorHint = Paint()
        .apply { color = context.resolveThemedColor(R.attr.colorError) }
        .apply { isAntiAlias = true }
        .apply { textAlign = Paint.Align.CENTER }

    var cellLength = 0

    override fun onLayout(changed: Boolean, left: Int, top: Int, right: Int, bottom: Int) {
        super.onLayout(changed, left, top, right, bottom)
        if (changed) {
            paintNumber.apply { textSize = cellLength * 0.5F }
            paintValidHint.apply { textSize = cellLength * 0.5F }
            paintErrorHint.apply { textSize = cellLength * 0.5F }
        }
    }

    protected lateinit var templateNumber: MutableList<Picture>
    protected lateinit var templateValidHint: MutableList<Picture>
    protected lateinit var templateErrorHint: MutableList<Picture>
    var refreshTemplates = true
}