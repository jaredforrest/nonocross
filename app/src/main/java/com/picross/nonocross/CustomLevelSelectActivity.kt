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
import android.widget.EditText
import android.widget.Toast
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.appcompat.widget.PopupMenu
import androidx.lifecycle.lifecycleScope
import androidx.recyclerview.widget.GridLayoutManager
import androidx.recyclerview.widget.RecyclerView
import arrow.core.Either
import com.google.android.material.floatingactionbutton.FloatingActionButton
import com.google.zxing.client.android.Intents
import com.journeyapps.barcodescanner.ScanContract
import com.journeyapps.barcodescanner.ScanOptions
import com.picross.nonocross.levelselect.CustomLevelSelectAdapter
import com.picross.nonocross.util.*
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
        findViewById<FloatingActionButton>(R.id.import_level_button).setOnClickListener {
            PopupMenu(this, it).apply {
                setOnMenuItemClickListener { item ->
                    when (item.itemId) {
                        R.id.import_file -> {
                            getContent.launch("*/*")
                            true
                        }
                        R.id.import_qr -> {
                            val scanOptions = ScanOptions()
                            scanOptions.setPrompt("Scan a QR Code")
                            scanOptions.setOrientationLocked(false)
                            scanOptions.setDesiredBarcodeFormats("QR_CODE")
                            scanOptions.addExtra(Intents.Scan.SCAN_TYPE, Intents.Scan.MIXED_SCAN)
                            gettyQR.launch(scanOptions)
                            true
                        }
                        else -> false
                    }

                }
                inflate(R.menu.add_level_menu)
                show()
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
                    Toast.makeText(baseContext, "Cancelled", Toast.LENGTH_SHORT).show()
                } else {
                    parseNonFile(it.contents).applyNotError(baseContext) {
                        val constraintLayout =
                            layoutInflater.inflate(R.layout.edit_text_layout, null)

                        AlertDialog.Builder(this@CustomLevelSelectActivity)
                            .setTitle(R.string.level_name)
                            .setMessage(R.string.enter_level_name)
                            .setView(constraintLayout)
                            .setPositiveButton(
                                android.R.string.ok
                            ) { _, _ ->
                                val temp =
                                    constraintLayout.findViewById<EditText>(R.id.edit_level_name).text.toString()
                                val filename = addCustomLevel(
                                    temp.substringBefore('\n'),
                                    it.contents,
                                    this@CustomLevelSelectActivity
                                )
                                levels.add(Pair(filename,this))
                                viewAdapter.notifyItemInserted(levels.size - 1)
                            }
                            .setNegativeButton(android.R.string.cancel, null)
                            .create()
                            .show()
                    }
                }
        }
}

