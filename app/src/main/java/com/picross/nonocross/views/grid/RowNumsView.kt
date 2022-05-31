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
import com.picross.nonocross.GameActivity.TransformDetails.mTransY
import com.picross.nonocross.R
import kotlin.math.max
import kotlin.math.min
import com.picross.nonocross.LevelDetails as LD

class RowNumsView(context: Context) : View(context) {

    private val preferences: SharedPreferences =
        PreferenceManager.getDefaultSharedPreferences(context)
    private val blueHints = preferences.getBoolean("showBlueHints", false)

    var cellLength = 0
    private val colorNumber = ResourcesCompat.getColor(context.resources, R.color.colorText, null)
    private val colorBlue = ResourcesCompat.getColor(context.resources, R.color.colorShade, null)
    private val blackPaint = Paint()
        .apply { color = colorNumber }
        .apply { isAntiAlias = true }
        .apply { textAlign = Paint.Align.RIGHT }
    private val bluePaint = Paint()
        .apply { color = colorBlue }
        .apply { isAntiAlias = true }
        .apply { textAlign = Paint.Align.RIGHT }

    override fun onLayout(changed: Boolean, left: Int, top: Int, right: Int, bottom: Int) {
        super.onLayout(changed, left, top, right, bottom)
        if (changed) {
            blackPaint.apply { textSize = cellLength * 0.5F }
            bluePaint.apply { textSize = cellLength * 0.5F }
        }
    }

    private var initX = 0f
    private var transX = 0f
    private var oldTransX = 0f
    private var mWidth = width

    @SuppressLint("ClickableViewAccessibility")
    override fun onTouchEvent(event: MotionEvent?): Boolean {
        when (event?.actionMasked) {
            MotionEvent.ACTION_DOWN -> {
                initX = event.getX(0)
                oldTransX = transX
            }
            MotionEvent.ACTION_MOVE -> {
                transX = max(
                    min(
                        event.getX(0) - initX + oldTransX,
                        (LD.gridData.longestRowNum * cellLength * 0.3f) * mScaleFactor - mWidth
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
        canvas.translate(width * (1 - mScaleFactor) + transX, mTransY)
        canvas.scale(mScaleFactor, mScaleFactor)
        if (refreshTemplates) {
            blackTemplate = getTemplate(width, height, blackPaint)
            blueTemplate = getTemplate(width, height, bluePaint)
            refreshTemplates = false
        }

        if (blueHints) {
            var counter = 0
            LD.gridData.rowNums.forEachIndexed { i, col ->
                col.reversed().forEachIndexed { j, num ->
                    if (
                        LD.userGrid.rowNums.getOrElse(i) { listOf(0) }.reversed()
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
        var curLeft: Float

        var curBot: Float = cellLength * 0.75F

        LD.gridData.rowNums.forEachIndexed { i, col ->
            curLeft = this.measuredWidth.toFloat() - cellLength.toFloat() * 0.2F
            col.reversed().forEach { num ->

                pictures.add(Picture())
                val canvas = pictures.last().beginRecording(width, height)
                canvas.drawText(num.toString(), curLeft, curBot, paint)
                pictures.last().endRecording()

                curLeft -= if (num < 10) cellLength * 0.5F else cellLength * 0.7F
            }
            curBot += if ((i % LD.gridData.width + 1) % 5 == 0) {
                cellLength + 3
            } else {
                cellLength + 1
            }
        }

        return pictures
    }

}