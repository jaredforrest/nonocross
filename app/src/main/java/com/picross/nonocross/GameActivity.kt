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
package com.picross.nonocross

import android.content.res.Configuration
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.view.View
import android.view.View.INVISIBLE
import android.view.View.VISIBLE
import android.widget.ImageView
import android.widget.TextView
import android.widget.Toast
import androidx.activity.OnBackPressedCallback
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.isVisible
import androidx.lifecycle.lifecycleScope
import arrow.core.None
import arrow.core.Some
import com.google.android.material.button.MaterialButtonToggleGroup
import com.google.android.material.dialog.MaterialAlertDialogBuilder
import com.google.zxing.BarcodeFormat
import com.google.zxing.MultiFormatWriter
import com.journeyapps.barcodescanner.BarcodeEncoder
import com.picross.nonocross.util.Preferences
import com.picross.nonocross.util.addCustomLevel
import com.picross.nonocross.util.getRandomGridPrefs
import com.picross.nonocross.util.invertBitmap
import com.picross.nonocross.util.newRandomGrid
import com.picross.nonocross.util.newUniqueRandomGrid
import com.picross.nonocross.util.saveCurrentGridState
import com.picross.nonocross.util.secondsToTime
import com.picross.nonocross.util.toNonFile
import com.picross.nonocross.util.usergrid.UserGrid
import com.picross.nonocross.util.usergrid.toGridData
import com.picross.nonocross.views.LevelSaveDialog
import com.picross.nonocross.views.grid.ColNumsView
import com.picross.nonocross.views.grid.GameView
import com.picross.nonocross.views.grid.GridView
import com.picross.nonocross.views.grid.RowNumsView
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlin.properties.Delegates
import com.picross.nonocross.LevelDetails as LD


class GameActivity : AppCompatActivity() {
    private lateinit var toggleGroup: MaterialButtonToggleGroup
    private lateinit var undo: View
    private lateinit var redo: View
    private lateinit var clear: View
    private lateinit var refresh: View
    lateinit var save: View
    private lateinit var qrCode: ImageView
    private lateinit var qrCodeImage: ImageView
    private lateinit var progress: View
    private lateinit var gameView: GameView
    private lateinit var timeView: TextView
    private lateinit var resetZoomButton: View

    private lateinit var colNumsView: ColNumsView
    private lateinit var rowNumsView: RowNumsView
    private lateinit var nonocrossGridView: GridView

    private lateinit var preferences: Preferences
    private var showTime by Delegates.notNull<Boolean>()
    private var trackTime by Delegates.notNull<Boolean>()

    private val handler = Handler(Looper.getMainLooper())

    private var qrFirstClick = true

    private lateinit var countSecond: Runnable
    private var running = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_game)

        resetZoom()

        preferences = Preferences(this)
        trackTime = preferences.trackTimer
        showTime = preferences.showTimer
        val saveWarn = preferences.showSaveWarning

        toggleGroup = findViewById(R.id.toggleGroup)
        undo = findViewById(R.id.undo)
        redo = findViewById(R.id.redo)
        clear = findViewById(R.id.clear)
        refresh = findViewById(R.id.new_grid)
        save = findViewById(R.id.save)
        qrCode = findViewById(R.id.qrcode)
        qrCodeImage = findViewById(R.id.img_result_qr)
        progress = findViewById(R.id.indeterminateBar)
        gameView = findViewById(R.id.nonocrossGameView)
        timeView = findViewById(R.id.time_view)
        resetZoomButton = findViewById(R.id.zoom)
        resetZoomButton.isVisible = preferences.enableZoom

        colNumsView = gameView.getChildAt(0) as ColNumsView
        rowNumsView = gameView.getChildAt(1) as RowNumsView
        nonocrossGridView = gameView.getChildAt(2) as GridView

        LD.toggleCross = false
        toggleGroup.check(R.id.toggleFill)
        toggleGroup.addOnButtonCheckedListener { _, checkedId, isChecked ->
            LD.toggleCross = (checkedId == R.id.toggleFill) xor isChecked
        }

        if (trackTime) {
            runTimer()
            if (showTime) {
                timeView.visibility = VISIBLE
            }
        }

        undo.setOnClickListener {
            when (nonocrossGridView.undo()) {
                is None -> {
                    nonocrossGridView.invalidate()
                    rowNumsView.invalidate()
                    colNumsView.invalidate()
                }
                is Some -> Toast.makeText(this, R.string.nothing_to_undo, Toast.LENGTH_SHORT).show()
            }
        }
        redo.setOnClickListener {
            when (nonocrossGridView.redo()) {
                is None -> {
                    nonocrossGridView.invalidate()
                    rowNumsView.invalidate()
                    colNumsView.invalidate()
                }
                is Some -> Toast.makeText(this, R.string.nothing_to_redo, Toast.LENGTH_SHORT).show()
            }
        }
        clear.setOnClickListener {
            qrCodeImage.visibility = INVISIBLE
            nonocrossGridView.clear()
            resetZoom()
            nonocrossGridView.invalidate()
            rowNumsView.invalidate()
            colNumsView.invalidate()
        }

        clear.setOnLongClickListener {
            qrCodeImage.visibility = INVISIBLE
            stopTimer()
            nonocrossGridView.superClear()
            resetZoom()
            nonocrossGridView.invalidate()
            rowNumsView.invalidate()
            colNumsView.invalidate()
            runTimer()
            true
        }

        resetZoomButton.setOnClickListener {
            resetZoom()
            nonocrossGridView.invalidate()
            rowNumsView.invalidate()
            colNumsView.invalidate()
        }

        if (LD.levelType is LevelType.Random) {
            refresh.visibility = VISIBLE
            refresh.setOnClickListener {
                if (!LD.userGrid.initial && saveWarn) {
                    confirmReset()
                } else {
                    resetGrid()
                }
            }
        }

        if (LD.levelType is LevelType.Random || LD.levelType is LevelType.Online) {
            if (saveWarn) {
                onBackPressedDispatcher.addCallback(this, confirmExit)
            }
            save.visibility = VISIBLE
            save.setOnClickListener {
                saveGrid()
            }
        }

        onBackPressedDispatcher.addCallback(this, leaveQR)

        qrCode.setOnClickListener {
            if (qrCodeImage.visibility == VISIBLE) {
                leaveQR.isEnabled = false
                qrCode.setImageResource(R.drawable.ic_baseline_qr_code_32)
                hideShowBoard(false)
                qrCodeImage.visibility = INVISIBLE
                runTimer()
            } else {
                if (qrFirstClick) {
                    val content = LD.gridData.toNonFile()

                    val multiFormatWriter = MultiFormatWriter()
                    val width = resources.displayMetrics.widthPixels
                    val height = resources.displayMetrics.heightPixels
                    val size = (width.coerceAtMost(height) * 0.8).toInt()
                    val bitMatrix =
                        multiFormatWriter.encode(content, BarcodeFormat.QR_CODE, size, size)
                    val barcodeEncoder = BarcodeEncoder()
                    val bitmap = when (this.resources.configuration.uiMode.and(Configuration.UI_MODE_NIGHT_MASK)) {
                        Configuration.UI_MODE_NIGHT_YES -> invertBitmap(barcodeEncoder.createBitmap(bitMatrix))
                        else -> barcodeEncoder.createBitmap(bitMatrix)
                    }
                    qrCodeImage.setImageBitmap(bitmap)
                    qrFirstClick = false
                }
                qrCode.setImageResource(R.drawable.ic_baseline_grid_on_32)
                stopTimer()
                hideShowBoard(true)
                qrCodeImage.visibility = VISIBLE
                leaveQR.isEnabled = true
            }
        }

    }

    override fun onPause() {
        stopTimer()
        super.onPause()
        saveCurrentGridState(this, LD.levelType, LD.userGrid)
    }

    override fun onResume() {
        super.onResume()
        runTimer()
    }

    fun resetGrid() {
        when (LD.levelType) {
            is LevelType.Random -> {
                if (preferences.uniqueLevel) {
                    hideShowBoard(true)
                    qrCode.visibility = INVISIBLE
                    progress.visibility = VISIBLE

                    val randomJob = lifecycleScope.launch(Dispatchers.IO) {
                        //myThread = Thread(LD.myThreadGroup, {
                        when (val temp =
                            newUniqueRandomGrid(getRandomGridPrefs(this@GameActivity))) {
                            is Some -> runOnUiThread {
                                LD.gridData = temp.value
                                LD.userGrid = UserGrid(LD.gridData, autoFill = true, resetComplete = preferences.resetComplete)
                                LD.levelType = LevelType.Random()
                                nonocrossGridView.updateNonoGrid()
                                gameView.refreshLayout()
                                rowNumsView.refreshTemplates = true
                                rowNumsView.invalidate()
                                colNumsView.refreshTemplates = true
                                colNumsView.invalidate()
                                resetZoom()
                                nonocrossGridView.invalidate()
                                LD.userGrid.timeElapsed = 0u
                                stopTimer()
                                runTimer()
                                hideShowBoard(false)
                                qrFirstClick = true
                                qrCode.visibility = VISIBLE
                                progress.visibility = INVISIBLE
                            }
                            is None -> TODO()
                        }
                    }
                    val cancelLoad = object : OnBackPressedCallback(true) {
                        override fun handleOnBackPressed() {
                            randomJob.cancel()
                            hideShowBoard(false)
                            qrCode.visibility = VISIBLE
                            progress.visibility = INVISIBLE
                            this.isEnabled = false
                        }
                    }
                    onBackPressedDispatcher.addCallback(this, cancelLoad)
                    cancelLoad.isEnabled = false
                    //}, "Native Thread", 1024L * 1024L)
                    //myThread?.start()
                } else {
                    val attributes = getRandomGridPrefs(this)
                    LD.gridData = newRandomGrid(attributes).toGridData(attributes)
                    LD.userGrid = UserGrid(LD.gridData, autoFill = true, resetComplete = preferences.resetComplete)
                    nonocrossGridView.updateNonoGrid()
                    gameView.refreshLayout()
                    rowNumsView.invalidate()
                    colNumsView.invalidate()
                    nonocrossGridView.invalidate()
                }
            }

            else -> {
                nonocrossGridView.clear()
                rowNumsView.invalidate()
                colNumsView.invalidate()
                nonocrossGridView.invalidate()
            }
        }
    }

    fun saveGrid(andQuit: Boolean = false) {
        val fileContents = LD.gridData.toNonFile()

        LevelSaveDialog.showDialog(this, layoutInflater) { name ->
            addCustomLevel(name, fileContents, this, LD.userGrid)
            if (andQuit) finish()
            save.visibility = INVISIBLE
            LD.levelType = LevelType.Random(Some(name))
            confirmExit.isEnabled = false
        }
    }

    private fun runTimer() {

        if(trackTime && !running) {
            timeView.text = secondsToTime(LD.userGrid.timeElapsed)

            countSecond = Runnable {

                LD.userGrid.countSecond()
                timeView.text = secondsToTime(LD.userGrid.timeElapsed)

                handler.postDelayed(countSecond, 250)
            }

            handler.postDelayed(countSecond, 250)
            running = true
        }
    }

    private fun stopTimer() {
        if(running) {
            handler.removeCallbacks(countSecond)
            running = false
        }
    }

    /** Enable gives a choice whether to show or hide most of the ui elements */
    private fun hideShowBoard(enable: Boolean) {
        val invisibility = if (enable) INVISIBLE else VISIBLE
        toggleGroup.visibility = invisibility
        gameView.visibility = invisibility
        if (trackTime && showTime) {
            timeView.visibility = invisibility
        }
        undo.visibility = invisibility
        redo.visibility = invisibility
        clear.visibility = invisibility
        save.visibility = invisibility
        refresh.visibility = invisibility
        if(preferences.enableZoom) {
            resetZoomButton.visibility = invisibility
        }
    }

    private fun resetZoom() {
        TransformDetails.mScaleFactor = 1f
        TransformDetails.mTransX = 0f
        TransformDetails.mTransY = 0f
    }

    object TransformDetails {
        var mScaleFactor = 1f
        var mTransX = 0f
        var mTransY = 0f
    }

    private fun confirmReset() {
        MaterialAlertDialogBuilder(this@GameActivity)
            .setTitle(R.string.confirm_new)
            .setMessage(R.string.lose_unsaved_data)
            .setNeutralButton(R.string.not_save) { _, _ ->
                resetGrid()
            }
            .setPositiveButton(R.string.save) { _, _ ->
                saveGrid(true)
            }
            .setNegativeButton(android.R.string.cancel, null)
            .create()
            .show()
    }

    private val confirmExit = object : OnBackPressedCallback(true) {
        override fun handleOnBackPressed() {
            // dialog message view

            MaterialAlertDialogBuilder(this@GameActivity)
                .setTitle(R.string.confirm_exit)
                .setMessage(R.string.lose_unsaved_data)
                .setNeutralButton(R.string.not_save) { _, _ ->
                    finish()
                }
                .setPositiveButton(R.string.save) { _, _ ->
                    saveGrid(true)
                }
                .setNegativeButton(android.R.string.cancel, null)
                .create()
                .show()
        }
    }

    private val leaveQR = object : OnBackPressedCallback(false) {
        override fun handleOnBackPressed() {
            qrCode.setImageResource(R.drawable.ic_baseline_qr_code_32)
            hideShowBoard(false)
            qrCodeImage.visibility = INVISIBLE
            runTimer()
            isEnabled = false
        }
    }
}