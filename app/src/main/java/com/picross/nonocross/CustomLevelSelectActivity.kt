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

import android.content.Context
import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.widget.Toast
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.lifecycleScope
import androidx.recyclerview.widget.GridLayoutManager
import androidx.recyclerview.widget.RecyclerView
import arrow.core.Either
import com.google.zxing.client.android.Intents
import com.journeyapps.barcodescanner.ScanContract
import com.journeyapps.barcodescanner.ScanOptions
import com.leinardi.android.speeddial.SpeedDialView
import com.picross.nonocross.levelselect.CustomLevelSelectAdapter
import com.picross.nonocross.util.addCustomLevel
import com.picross.nonocross.util.applyNotError
import com.picross.nonocross.util.errorToast
import com.picross.nonocross.util.getNonFile
import com.picross.nonocross.util.parseNonFile
import com.picross.nonocross.views.LevelSaveDialog
import kotlinx.coroutines.launch
import java.io.File
import java.io.FileInputStream
import java.io.FileNotFoundException


class CustomLevelSelectActivity : AppCompatActivity(), CustomLevelSelectAdapter.StartGame {
    private lateinit var recyclerView: RecyclerView
    private lateinit var viewAdapter: RecyclerView.Adapter<*>
    private lateinit var viewManager: RecyclerView.LayoutManager

    override val isCustom = true

    override var levels = LevelDetails.customLevels

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_level_select)

        viewManager = GridLayoutManager(this, 3)
        viewAdapter = CustomLevelSelectAdapter(this, this)

        recyclerView = findViewById<RecyclerView>(R.id.my_recycler_view).apply {
            // use this setting to improve performance if you know that changes
            // in content do not change the layout size of the RecyclerView
            setHasFixedSize(false)

            // use a grid layout manager
            layoutManager = viewManager

            // specify an viewAdapter
            adapter = viewAdapter

        }
        val fab = findViewById<SpeedDialView>(R.id.import_level_button)
        fab.inflate(R.menu.add_level_menu)
        fab.setOnActionSelectedListener { item ->
            when (item.id) {
                R.id.import_file -> {
                    getContent.launch("*/*")
                    true
                }
                R.id.import_qr -> {
                    val scanOptions = ScanOptions()
                    scanOptions.setPrompt(getString(R.string.scan_a_qr_code))
                    scanOptions.setOrientationLocked(false)
                    scanOptions.setDesiredBarcodeFormats(ScanOptions.QR_CODE)
                    scanOptions.addExtra(Intents.Scan.SCAN_TYPE, Intents.Scan.MIXED_SCAN)
                    gettyQR.launch(scanOptions)
                    true
                }
                else -> false
            }
        }
    }

    override fun startGame() {
        val intent = Intent(this, GameActivity::class.java)
        startActivity(intent)
    }

    override fun removeLevel(level: String, position: Int) {
        recyclerView.removeViewAt(position)
        viewAdapter.notifyItemRemoved(position)
        val levelFile = File(this.getDir("levels", MODE_PRIVATE), level)
        val saveFile = File(this.getDir("custom", MODE_PRIVATE), level)
        levelFile.delete()
        saveFile.delete()
    }

    override fun levelType(levelName: String): LevelType =
        LevelType.Custom(levelName)

    override fun openSave(levelName: String): ByteArray {
        val saveDir = getDir("saves", Context.MODE_PRIVATE)
        val saveFile =
            try {
                FileInputStream(File(File(saveDir, "custom"), levelName))
            } catch (e: FileNotFoundException) {
                return byteArrayOf()
            }
        return saveFile.readBytes()
    }

    private val getContent =
        registerForActivityResult(ActivityResultContracts.GetContent()) { uri: Uri? ->
            lifecycleScope.launch {
                if (uri != null) {
                    getNonFile(uri, this@CustomLevelSelectActivity)
                        .applyNotError(baseContext) {
                            when(val gridData = parseNonFile(second)) {
                                is Either.Right -> {
                                    val filename = addCustomLevel(first, second, this@CustomLevelSelectActivity)
                                    levels.add(Pair(filename, gridData.value))
                                    viewAdapter.notifyItemInserted(levels.size - 1)
                                }
                                is Either.Left ->
                                    errorToast(this@CustomLevelSelectActivity, gridData.value.toString())
                        }
                    }
                }
            }
        }

    private val gettyQR =
        registerForActivityResult(ScanContract()) {
                if (it.contents == null) {
                    Toast.makeText(baseContext, R.string.cancelled, Toast.LENGTH_SHORT).show()
                } else {
                    parseNonFile(it.contents).applyNotError(baseContext) {
                        LevelSaveDialog.showDialog(this@CustomLevelSelectActivity, layoutInflater) { name ->
                            val filename = addCustomLevel(
                                name.substringBefore('\n'),
                                it.contents,
                                this@CustomLevelSelectActivity
                            )
                            levels.add(Pair(filename,this))
                            viewAdapter.notifyItemInserted(levels.size - 1)
                        }
                    }
                }
        }
}

