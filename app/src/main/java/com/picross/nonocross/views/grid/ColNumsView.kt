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
import androidx.core.content.res.ResourcesCompat
import androidx.preference.PreferenceManager
import com.picross.nonocross.GameActivity.TransformDetails.mScaleFactor
import com.picross.nonocross.GameActivity.TransformDetails.mTransX
import com.picross.nonocross.R
import kotlin.math.max
import kotlin.math.min
import com.picross.nonocross.LevelDetails as LD

class ColNumsView(context: Context) : View(context) {

    private val preferences: SharedPreferences =
        PreferenceManager.getDefaultSharedPreferences(context)
    private val blueHints = preferences.getBoolean("showBlueHints", false)

    var cellLength = 0
    private val colorNumber = ResourcesCompat.getColor(context.resources, R.color.colorText, null)
    private val colorBlue = ResourcesCompat.getColor(context.resources, R.color.colorShade, null)
    private val blackPaint = Paint()
        .apply { color = colorNumber }
        .apply { isAntiAlias = true }
        .apply { textAlign = Paint.Align.CENTER }
    private val bluePaint = Paint()
        .apply { color = colorBlue }
        .apply { isAntiAlias = true }
        .apply { textAlign = Paint.Align.CENTER }

    override fun onLayout(changed: Boolean, left: Int, top: Int, right: Int, bottom: Int) {
        super.onLayout(changed, left, top, right, bottom)
        if (changed) {
            mHeight = height
            blackPaint.apply { textSize = cellLength * 0.5F }
            bluePaint.apply { textSize = cellLength * 0.5F }
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

    private lateinit var blackTemplate: MutableList<Picture>
    private lateinit var blueTemplate: MutableList<Picture>
    var refreshTemplates = true

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)
        canvas.save()
        canvas.translate(mTransX, transY + mHeight * (1 - mScaleFactor))
        canvas.scale(mScaleFactor, mScaleFactor)
        if (refreshTemplates) {
            blackTemplate = getTemplate(width, height, blackPaint)
            blueTemplate = getTemplate(width, height, bluePaint)
            refreshTemplates = false
        }

        if (blueHints) {
            var counter = 0
            LD.gridData.colNums.forEachIndexed { i, col ->
                col.reversed().forEachIndexed { j, num ->
                    if (
                        LD.userGrid.colNums.getOrElse(i) { listOf(0) }.reversed()
                            .getOrElse(j) { 0 } == num
                    ) {
                        canvas.drawPicture(blueTemplate[counter])
                    } else {
                        canvas.drawPicture(blackTemplate[counter])
                    }
                    counter++
                }

            }
        } else {
            blackTemplate.forEach { canvas.drawPicture(it) }
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