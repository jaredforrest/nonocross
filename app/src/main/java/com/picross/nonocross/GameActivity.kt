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

import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.view.View
import android.view.View.*
import android.widget.EditText
import android.widget.ImageView
import android.widget.TextView
import android.widget.Toast
import androidx.activity.OnBackPressedCallback
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.lifecycleScope
import androidx.preference.PreferenceManager
import arrow.core.Either
import arrow.core.None
import arrow.core.Some
import com.google.android.material.button.MaterialButtonToggleGroup
import com.google.zxing.BarcodeFormat
import com.google.zxing.MultiFormatWriter
import com.journeyapps.barcodescanner.BarcodeEncoder
import com.picross.nonocross.util.*
import com.picross.nonocross.util.usergrid.UserGrid
import com.picross.nonocross.util.usergrid.toGridData
import com.picross.nonocross.views.grid.ColNumsView
import com.picross.nonocross.views.grid.GameView
import com.picross.nonocross.views.grid.GridView
import com.picross.nonocross.views.grid.RowNumsView
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
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
    private lateinit var zoom: View

    private lateinit var colNumsView: ColNumsView
    private lateinit var rowNumsView: RowNumsView
    private lateinit var nonocrossGridView: GridView

    private val handler = Handler(Looper.getMainLooper())

    private var qrFirstClick = true

    private var running = true
    private lateinit var countSecond: Runnable

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_game)

        resetZoom()

        runTimer()

        val preferences = PreferenceManager.getDefaultSharedPreferences(this)

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
        zoom = findViewById(R.id.zoom)

        colNumsView = gameView.getChildAt(0) as ColNumsView
        rowNumsView = gameView.getChildAt(1) as RowNumsView
        nonocrossGridView = gameView.getChildAt(2) as GridView

        LD.toggleCross = false
        toggleGroup.check(R.id.toggleFill)
        toggleGroup.addOnButtonCheckedListener { _, checkedId, isChecked ->
            LD.toggleCross = (checkedId == R.id.toggleFill) xor isChecked
        }

        if (preferences.getBoolean("timer", false)) {
            timeView.visibility = VISIBLE
        }

        //progress.visibility = INVISIBLE
        undo.setOnClickListener {
            when (nonocrossGridView.undo()) {
                is Either.Right -> {
                    nonocrossGridView.invalidate()
                    rowNumsView.invalidate()
                    colNumsView.invalidate()
                }
                is Either.Left -> Toast.makeText(this, "Nothing to undo", Toast.LENGTH_SHORT).show()
            }
        }
        redo.setOnClickListener {
            when (nonocrossGridView.redo()) {
                is Either.Right -> {
                    nonocrossGridView.invalidate()
                    rowNumsView.invalidate()
                    colNumsView.invalidate()
                }
                is Either.Left -> Toast.makeText(this, "Nothing to redo", Toast.LENGTH_SHORT).show()
            }
        }
        clear.setOnClickListener {
            qrCodeImage.visibility = INVISIBLE
            nonocrossGridView.clear()
            resetZoom()
            rowNumsView.invalidate()
            colNumsView.invalidate()
        }

        zoom.setOnClickListener {
            resetZoom()
            nonocrossGridView.invalidate()
            rowNumsView.invalidate()
            colNumsView.invalidate()
        }



        if (LD.levelType is LevelType.Random) {
            refresh.visibility = VISIBLE
            refresh.setOnClickListener {
                resetGrid()
            }
        }

        if (LD.levelType is LevelType.Random || LD.levelType is LevelType.Online) {
            if (preferences.getBoolean("saveWarn", true)) {
                onBackPressedDispatcher.addCallback(this, confirmExit)
            }
            save.visibility = VISIBLE
            save.setOnClickListener {
                saveGrid()
            }
        }


        qrCode.setOnClickListener {
            if (qrCodeImage.visibility == VISIBLE) {
                qrCode.setImageResource(R.drawable.ic_baseline_qr_code_32)

                undo.visibility = VISIBLE
                redo.visibility = VISIBLE
                nonocrossGridView.visibility = VISIBLE
                rowNumsView.visibility = VISIBLE
                colNumsView.visibility = VISIBLE
                qrCodeImage.visibility = INVISIBLE
                running = true
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
                    val bitmap = barcodeEncoder.createBitmap(bitMatrix)
                    qrCodeImage.setImageBitmap(bitmap)
                    qrFirstClick = false
                }

                qrCode.setImageResource(R.drawable.ic_baseline_grid_on_32)
                running = false
                qrCodeImage.visibility = VISIBLE
                undo.visibility = INVISIBLE
                redo.visibility = INVISIBLE
                nonocrossGridView.visibility = INVISIBLE
                rowNumsView.visibility = INVISIBLE
                colNumsView.visibility = INVISIBLE

            }
        }

    }

    override fun onPause() {
        super.onPause()
        running = false
        saveCurrentGridState(this, LD.levelType, LD.userGrid)
    }

    override fun onResume() {
        super.onResume()
        running = true
    }

    fun resetGrid() {
        when (LD.levelType) {
            is LevelType.Random -> {
                if (PreferenceManager.getDefaultSharedPreferences(this)
                        .getBoolean("uniqueLevel", true)
                ) {
                    loadingMenu(true)

                    val randomJob = lifecycleScope.launch(Dispatchers.IO) {
                        //myThread = Thread(LD.myThreadGroup, {
                        when (val temp =
                            newUniqueRandomGrid(getRandomGridPrefs(this@GameActivity))) {
                            is Some -> runOnUiThread {
                                LD.gridData = temp.value
                                LD.userGrid = UserGrid(LD.gridData)
                                nonocrossGridView.updateNonoGrid()
                                gameView.refreshLayout()
                                rowNumsView.refreshTemplates = true
                                rowNumsView.invalidate()
                                colNumsView.refreshTemplates = true
                                colNumsView.invalidate()
                                resetZoom()
                                nonocrossGridView.invalidate()
                                LD.userGrid.timeElapsed = 0
                                handler.removeCallbacks(countSecond)
                                handler.post(countSecond)
                                timeView.visibility = VISIBLE
                                gameView.visibility = VISIBLE
                                undo.visibility = VISIBLE
                                redo.visibility = VISIBLE
                                clear.visibility = VISIBLE
                                save.visibility = VISIBLE
                                refresh.visibility = VISIBLE
                                qrFirstClick = true
                                qrCode.visibility = VISIBLE
                                toggleGroup.visibility = VISIBLE
                                progress.visibility = INVISIBLE
                            }
                            is None -> TODO()
                        }
                    }
                    val cancelLoad = object : OnBackPressedCallback(true) {
                        override fun handleOnBackPressed() {
                            randomJob.cancel()
                            loadingMenu(false)
                            this.isEnabled = false
                        }
                    }
                    onBackPressedDispatcher.addCallback(this, cancelLoad)
                    cancelLoad.isEnabled = false
                    //}, "Native Thread", 1024L * 1024L)
                    //myThread?.start()
                } else {
                    val wHD = getRandomGridPrefs(this)
                    LD.gridData = newRandomGrid(wHD).toGridData(wHD.second)
                    LD.userGrid = UserGrid(LD.gridData)
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
                LD.userGrid.timeElapsed = 0
                handler.removeCallbacks(countSecond)
                handler.post(countSecond)
            }
        }
    }

    private fun saveGrid(andQuit: Boolean = false) {
        val fileContents = LD.gridData.toNonFile()

        // dialog message view
        val constraintLayout = layoutInflater.inflate(R.layout.edit_text_layout, null)

        AlertDialog.Builder(this)
            .setTitle(R.string.level_name)
            .setMessage(R.string.enter_level_name)
            .setView(constraintLayout)
            .setPositiveButton(
                android.R.string.ok
            ) { _, _ ->
                val temp =
                    constraintLayout.findViewById<EditText>(R.id.edit_level_name).text.toString()
                val fileName = if (temp == "") "Untitled Level" else temp
                addCustomLevel(fileName, fileContents, this, LD.userGrid)
                if (andQuit) finish()
                save.visibility = GONE
                LD.levelType = LevelType.Random(Some(fileName))
                confirmExit.isEnabled = false
            }
            .setNegativeButton(android.R.string.cancel, null)
            .create()
            .show()
    }

    private fun runTimer() {

        countSecond = Runnable {

            timeView.text = secondsToTime(LD.userGrid.timeElapsed)

            if (running) {
                LD.userGrid.timeElapsed++
            }

            handler.postDelayed(countSecond, 1000)
        }

        handler.post(countSecond)
    }

    /** Enable gives a choice whether to show or hide the loading menu */
    private fun loadingMenu(enable: Boolean) {
        val visibility = if (enable) VISIBLE else INVISIBLE
        val invisibility = if (enable) INVISIBLE else VISIBLE
        toggleGroup.visibility = invisibility
        qrCode.visibility = invisibility
        qrCodeImage.visibility = invisibility
        gameView.visibility = invisibility
        timeView.visibility = invisibility
        undo.visibility = invisibility
        redo.visibility = invisibility
        clear.visibility = invisibility
        save.visibility = invisibility
        refresh.visibility = invisibility
        progress.visibility = visibility
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

    private val confirmExit = object : OnBackPressedCallback(true) {
        override fun handleOnBackPressed() {
            // dialog message view

            AlertDialog.Builder(this@GameActivity)
                .setTitle("Are you sure you want to exit?")
                .setMessage("Any unsaved data will be lost.")
                .setNeutralButton(
                    "Don't Save"
                ) { _, _ ->
                    finish()
                }
                .setPositiveButton("Save")
                { _, _ ->
                    saveGrid(true)
                }
                .setNegativeButton(android.R.string.cancel, null)
                .create()
                .show()
        }
    }
}