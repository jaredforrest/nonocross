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

import android.annotation.SuppressLint
import android.content.Context
import android.content.SharedPreferences
import android.graphics.Canvas
import android.graphics.Paint
import android.graphics.Picture
import android.view.MotionEvent
import android.view.View
import androidx.preference.PreferenceManager
import com.picross.nonocross.GameActivity.TransformDetails.mScaleFactor
import com.picross.nonocross.GameActivity.TransformDetails.mTransX
import com.picross.nonocross.R
import com.picross.nonocross.util.resolveThemedColor
import kotlin.math.max
import kotlin.math.min
import com.picross.nonocross.LevelDetails as LD

class ColNumsView(context: Context) : View(context) {

    private val preferences: SharedPreferences =
        PreferenceManager.getDefaultSharedPreferences(context)
    private val showHints = preferences.getBoolean("showBlueHints", false)

    var cellLength = 0
    private val colorNumber = context.resolveThemedColor(R.attr.colorOnSurface)
    private val colorShade = context.resolveThemedColor(R.attr.colorPrimary)
    private val colorCross = context.resolveThemedColor(R.attr.colorCross)
    private val paintNumber = Paint()
        .apply { color = colorNumber }
        .apply { isAntiAlias = true }
        .apply { textAlign = Paint.Align.CENTER }
    private val paintShade = Paint()
        .apply { color = colorShade }
        .apply { isAntiAlias = true }
        .apply { textAlign = Paint.Align.CENTER }
    private val paintCross = Paint()
        .apply { color = colorCross }
        .apply { isAntiAlias = true }
        .apply { textAlign = Paint.Align.CENTER }

    override fun onLayout(changed: Boolean, left: Int, top: Int, right: Int, bottom: Int) {
        super.onLayout(changed, left, top, right, bottom)
        if (changed) {
            mHeight = height
            paintNumber.apply { textSize = cellLength * 0.5F }
            paintShade.apply { textSize = cellLength * 0.5F }
            paintCross.apply { textSize = cellLength * 0.5F }
        }
    }

    private var initY = 0f
    private var transY = 0f
    private var oldTransY = 0f
    private var mHeight = height

    @SuppressLint("ClickableViewAccessibility")
    override fun onTouchEvent(event: MotionEvent?): Boolean {
        when (event?.actionMasked) {
            MotionEvent.ACTION_DOWN -> {
                initY = event.getY(0)
                oldTransY = transY
            }
            MotionEvent.ACTION_MOVE -> {
                transY = max(
                    min(
                        event.getY(0) - initY + oldTransY,
                        (LD.gridData.longestColNum * cellLength * 0.75f) * mScaleFactor - mHeight
                    ), 0f
                )
                invalidate()
            }
        }
        return true
    }

    private lateinit var templateNumber: MutableList<Picture>
    private lateinit var templateShade: MutableList<Picture>
    private lateinit var templateCross: MutableList<Picture>
    var refreshTemplates = true

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)
        canvas.save()
        canvas.translate(mTransX, transY + mHeight * (1 - mScaleFactor))
        canvas.scale(mScaleFactor, mScaleFactor)
        if (refreshTemplates) {
            templateNumber = getTemplate(width, height, paintNumber)
            templateShade = getTemplate(width, height, paintShade)
            templateCross = getTemplate(width, height, paintCross)
            refreshTemplates = false
        }

        if (showHints) {
            var counter = 0
            LD.gridData.colNums.forEachIndexed { i, col ->
                col.reversed().forEachIndexed { j, num ->
                    if (
                        LD.userGrid.colNums.getOrElse(i) { listOf(0) }.reversed()
                            .getOrElse(j) { 0 } == num
                    ) {
                        canvas.drawPicture(templateShade[counter])
                    }
                    else if (
                        LD.userGrid.colNums.getOrElse(i) { listOf(0) }.reversed()
                            .getOrElse(j) { 0 } > num
                    ) {
                        canvas.drawPicture(templateCross[counter])
                    }
                    else {
                        canvas.drawPicture(templateNumber[counter])
                    }
                    counter++
                }

            }
        } else {
            templateNumber.forEach { canvas.drawPicture(it) }
        }

        canvas.restore()
    }

    private fun getTemplate(width: Int, height: Int, paint: Paint): MutableList<Picture> {
        val pictures: MutableList<Picture> = mutableListOf()
        var curBot: Float

        var curLeft: Float = cellLength.toFloat() * 0.5F

        LD.gridData.colNums.forEachIndexed { i, col ->
            curBot = this.measuredHeight.toFloat() - cellLength.toFloat() * 0.2F
            col.reversed().forEach { num ->

                pictures.add(Picture())
                val canvas = pictures.last().beginRecording(width, height)
                canvas.drawText(num.toString(), curLeft, curBot, paint)
                pictures.last().endRecording()

                curBot -= cellLength.toFloat() * 0.75F
            }
            curLeft += if ((i % LD.gridData.width + 1) % 5 == 0) {
                cellLength + 3
            } else {
                cellLength + 1
            }
        }

        return pictures
    }

}