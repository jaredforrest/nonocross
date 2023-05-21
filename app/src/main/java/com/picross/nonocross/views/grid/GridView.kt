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
import android.app.AlertDialog
import android.content.Context
import android.content.DialogInterface
import android.content.SharedPreferences
import android.graphics.Canvas
import android.graphics.Paint
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.View
import android.view.ViewConfiguration
import androidx.appcompat.app.AppCompatActivity
import androidx.preference.PreferenceManager
import arrow.core.None
import com.picross.nonocross.GameActivity
import com.picross.nonocross.GameActivity.TransformDetails.mScaleFactor
import com.picross.nonocross.GameActivity.TransformDetails.mTransX
import com.picross.nonocross.GameActivity.TransformDetails.mTransY
import com.picross.nonocross.HighScoreManager
import com.picross.nonocross.LevelType
import com.picross.nonocross.R
import com.picross.nonocross.util.CellShade
import com.picross.nonocross.util.getRandomGridPrefs
import com.picross.nonocross.util.resolveThemedColor
import com.picross.nonocross.util.secondsToTime
import com.picross.nonocross.util.usergrid.UserGridView
import com.picross.nonocross.util.vibrate
import kotlin.math.pow
import kotlin.math.sqrt
import com.picross.nonocross.LevelDetails as LD


class GridView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0
) : View(context, attrs, defStyleAttr) {

    private val colorCross =  context.resolveThemedColor(R.attr.colorCross)
    private val colorShade =  context.resolveThemedColor(R.attr.colorPrimary)
    private val colorEmpty = context.resolveThemedColor(R.attr.colorSurface)
    private val preferences: SharedPreferences =
        PreferenceManager.getDefaultSharedPreferences(context)
    private val enableZoom = preferences.getBoolean("enable_zoom", true)

    private val paintEmpty = Paint().apply { color = colorEmpty }
    private val paintShade = Paint().apply { color = colorShade }
    private val paintCross = Paint().apply { color = colorCross }
        .apply { strokeCap = Paint.Cap.ROUND }
        .apply { isAntiAlias = true }

    private var oldScaleFactor = 1f

    private var oldTransX = 0f
    private var oldTransY = 0f

    private var initX1 = 0f
    private var initY1 = 0f
    private var initX2 = 0f
    private var initY2 = 0f
    private var initDist = 0f
    private var initXMid2 = 0f
    private var initYMid2 = 0f

    private var mWidth = width.toFloat()
    private var midpoint = (width / 2).toFloat()
//    private var mHeight = height


    @SuppressLint("DrawAllocation")
    override fun onLayout(changed: Boolean, left: Int, top: Int, right: Int, bottom: Int) {
        super.onLayout(changed, left, top, right, bottom)
        nonoGrid = UserGridView(LD.gridData.width, LD.gridData.height, cellLength, paintEmpty, paintShade, paintCross)
        mWidth = width.toFloat()
        midpoint = (width / 2).toFloat()
        //mHeight = height
    }

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)
        canvas.save()
        canvas.translate(mTransX, mTransY)
        canvas.scale(mScaleFactor, mScaleFactor)
        nonoGrid.draw(LD.userGrid.grid, canvas)
        canvas.restore()
    }


    @SuppressLint("ClickableViewAccessibility")
    override fun onTouchEvent(event: MotionEvent): Boolean {
        when (event.actionMasked) {
            MotionEvent.ACTION_DOWN -> {
                initializeFill(event.getX(0), event.getY(0))
            }
            MotionEvent.ACTION_MOVE -> if (event.pointerCount == 1) {
                startFill(event.getX(0), event.getY(0))
            } else if (enableZoom) {
                val currX1 = event.getX(0)
                val currY1 = event.getY(0)
                val currX2 = event.getX(1)
                val currY2 = event.getY(1)
                val currDist = sqrt((currX2 - currX1).pow(2) + (currY2 - currY1).pow(2))
                val scale = currDist / initDist
                val temp = scale * oldScaleFactor
                if (temp >= 1F) {
                    mScaleFactor = temp
                    mTransX =
                        (currX2 + currX1 - initXMid2) / (/*2 */ mScaleFactor) + (midpoint + scale * (oldTransX - midpoint))
                    //(scale * oldTransX)
                    mTransY =
                        (currY2 + currY1 - initYMid2) / (/*2 */ mScaleFactor) + (midpoint + scale * (oldTransY - midpoint))
                    //(scale * oldTransY)
                } else {
                    mTransX =
                        (currX2 + currX1 - initXMid2) /* 2*/ + (midpoint + (oldTransX - midpoint) / oldScaleFactor)
                    mTransY =
                        (currY2 + currY1 - initYMid2) /* 2*/ + (midpoint + (oldTransY - midpoint) / oldScaleFactor)
                }
//                    Log.d("efasefedfeeff2", "$midpoint $scale $mTransX $mTransY") //"$initX1 $initY1 $initX2 $initY2 $currX1 $currY1 $currX2 $currY2")//"$currDist $initDist")
                invalidate()

            }
            MotionEvent.ACTION_UP -> endFill()
            MotionEvent.ACTION_CANCEL -> handler.removeCallbacks(mLongPressed)
            MotionEvent.ACTION_POINTER_DOWN -> {
                if (event.pointerCount == 2) {
                    handler.removeCallbacks(mLongPressed)
                    isFirstCell = false

                    oldScaleFactor = mScaleFactor
                    oldTransX = mTransX
                    oldTransY = mTransY
                    initX1 = event.getX(0)
                    initY1 = event.getY(0)
                    initX2 = event.getX(1)
                    initY2 = event.getY(1)
                    initDist = sqrt((initX2 - initX1).pow(2) + (initY2 - initY1).pow(2))
                    initXMid2 = initX1 + initX2
                    initYMid2 = initY1 + initY2
                }
            }
        }
        return true
    }

    var cellLength = 0
    private lateinit var nonoGrid: UserGridView

    // Get Preferences
    private val sharedPreferences: SharedPreferences =
        PreferenceManager.getDefaultSharedPreferences(context)
    private val fatFingerMode = sharedPreferences.getBoolean("fatFinger", true)
    private val vibrateOn = sharedPreferences.getBoolean("vibrate", false)
    private val fillMode = when (val fM = sharedPreferences.getString("fillMode", "Default")) {
        "Lax" -> 0
        "Default" -> 1
        "Strict" -> 2
        else -> 1
    }

    private var isFirstCell = true
    private var isLongPress = false

    /** First cell */
    private var fC = 0

    /** Active cell */
    private var aC = 0

    /** First cell's initial user shading */
    private lateinit var initShade: CellShade

    /** If true fill horizontally, false fill vertically */
    private var fillHori = true

    private var mLongPressed = Runnable {
        LD.userGrid.click(fC, LD.toggleCross)
        invalidate()
        isLongPress = true
        if (vibrateOn) vibrate(context)
    }

    private fun initializeFill(x: Float, y: Float) {
        nonoGrid.getCellAt((x - mTransX) / mScaleFactor, (y - mTransY) / mScaleFactor).fold({}, {
            handler.postDelayed(
                mLongPressed,
                ViewConfiguration.getLongPressTimeout().toLong()
            )

            aC = it
            fC = it
            initShade = LD.userGrid.getShade(it)
            isFirstCell = true
            isLongPress = false
        })
    }

    private fun startFill(x: Float, y: Float) {
        // Only run if current cell has moved
        if (!nonoGrid.isInside(aC, (x - mTransX) / mScaleFactor, (y - mTransY) / mScaleFactor)) {
            if (isFirstCell) {
                isFirstCell = false
                handler.removeCallbacks(mLongPressed)
                if (!isLongPress) LD.userGrid.click(fC, !LD.toggleCross)
                invalidate()

                nonoGrid.getCellAt((x - mTransX) / mScaleFactor, (y - mTransY) / mScaleFactor)
                    .fold({}, { fillHori = (LD.userGrid.sameRow(it, fC)) })
            }
            nonoGrid.getCellAt((x - mTransX) / mScaleFactor, (y - mTransY) / mScaleFactor)
                .fold({}, {
                    if (!fatFingerMode) {
                        LD.userGrid.copyShade(fC,it)
                    } else {
                        if (fillHori) LD.userGrid.copyRowInRange(fC, it, initShade, fillMode)
                        else LD.userGrid.copyColInRange(fC, it, initShade, fillMode)
                    }
                    invalidate()
                    aC = it
                })
        }
    }

    private fun endFill() {
        handler.removeCallbacks(mLongPressed)
        if (isFirstCell and !isLongPress) {
            LD.userGrid.click(fC, !LD.toggleCross)
            invalidate()
        }
        LD.userGrid.undoAddStack()
        if (LD.userGrid.checkDone()) gameDoneAlert()
    }

    /** When the game is finished show a dialog */
    private fun gameDoneAlert() {
        val newHighScore =
            HighScoreManager.handleNewScore(context, LD.userGrid, getRandomGridPrefs(context).third)

        val timeMessage = if (newHighScore) R.string.level_complete_new_high_score else R.string.level_complete

        LD.userGrid.complete = true
        val done = AlertDialog.Builder(context)
            .setTitle(R.string.finished)
            .setMessage(
                if (LD.userGrid.timeElapsed > 0u) {
                    context.getString(
                        timeMessage,
                        secondsToTime(LD.userGrid.timeElapsed)
                    )
                } else {
                    context.getString(R.string.level_complete_no_time)
                }
            )
            .setPositiveButton(
                R.string.menu
            ) { _: DialogInterface, _: Int ->
                (context as AppCompatActivity).finish()
            }
            .setNegativeButton(
                R.string.reset
            ) { _: DialogInterface, _: Int ->
                if (vibrateOn) vibrate(context)
                resetGrid()
            }

        val lT = LD.levelType
        if(lT is LevelType.Random && lT.levelName is None) done.setNeutralButton(R.string.save){ _, _ ->
            (context as? GameActivity)?.saveGrid(false)
        }

        done.show()
    }

    private fun resetGrid() {
        (context as GameActivity).resetGrid()
    }

    fun undo() = LD.userGrid.undo()

    fun redo() = LD.userGrid.redo()

    fun clear() = LD.userGrid.clear()

    fun superClear() = LD.userGrid.superClear()

    fun updateNonoGrid() {
        nonoGrid = UserGridView(LD.gridData.width, LD.gridData.height, cellLength, paintEmpty, paintShade, paintCross)
    }

}