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
import com.google.android.material.floatingactionbutton.FloatingActionButton
import com.google.zxing.integration.android.IntentIntegrator
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

    override val levels =
        LevelDetails.customLevels //runBlocking { getAllCustomLevels(this@CustomLevelSelectActivity) }

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
                            val intentIntegrator = IntentIntegrator(this@CustomLevelSelectActivity)
                            intentIntegrator.setPrompt("Scan a QR Code")
                            intentIntegrator.setOrientationLocked(false)
                            gettyQR.launch(intentIntegrator.createScanIntent())
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
                    checkValidNonFile(
                        uri,
                        this@CustomLevelSelectActivity
                    ).applyNotError(baseContext) {
                        addCustomLevel(first, second, this@CustomLevelSelectActivity)
                        this@CustomLevelSelectActivity.recreate()
                    }
                }
            }
        }

    private val gettyQR =
        registerForActivityResult(ActivityResultContracts.StartActivityForResult()) {
            lifecycleScope.launch {
                val intentResult = IntentIntegrator.parseActivityResult(it.resultCode, it.data)
                if (intentResult.contents == null) {
                    Toast.makeText(baseContext, "Cancelled", Toast.LENGTH_SHORT).show()
                } else {
                    parseNonFile(intentResult.contents).applyNotError(baseContext) {
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
                                val fileName = if (temp == "") "Untitled Level" else temp
                                addCustomLevel(
                                    fileName.substringBefore('\n'),
                                    intentResult.contents,
                                    this@CustomLevelSelectActivity
                                )
                                this@CustomLevelSelectActivity.recreate()
                            }
                            .setNegativeButton(android.R.string.cancel, null)
                            .create()
                            .show()
                    }
                }
            }
        }
}

