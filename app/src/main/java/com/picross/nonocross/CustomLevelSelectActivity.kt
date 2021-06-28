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
import android.util.Log
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.GridLayoutManager
import androidx.recyclerview.widget.RecyclerView
import arrow.core.Either
import com.google.android.material.floatingactionbutton.FloatingActionButton
import com.picross.nonocross.levelselect.CustomLevelSelectAdapter
import com.picross.nonocross.util.addFile
import com.picross.nonocross.util.checkValidNonFile
import com.picross.nonocross.util.generate

class CustomLevelSelectActivity : AppCompatActivity(), CustomLevelSelectAdapter.StartGame {
    private lateinit var recyclerView: RecyclerView
    private lateinit var viewAdapter: RecyclerView.Adapter<*>
    private lateinit var viewManager: RecyclerView.LayoutManager

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_level_select)

        viewManager = GridLayoutManager(this, 3)
        viewAdapter = CustomLevelSelectAdapter(getAllCustomLevels(this), this, this)

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
            getContent.launch(
                "*/*"
            )
        }
    }

    override fun startGame(levelName: String) {
        LevelDetails.isRandom = false
        generate(this, levelName, true)
        val intent = Intent(this, GameActivity::class.java)
        startActivity(intent)
    }

    override fun removeLevel(level: String, position: Int) {
        recyclerView.removeViewAt(position)
        viewAdapter.notifyItemRemoved(position)
        viewAdapter.notifyDataSetChanged()
        this.deleteFile(level)
    }

    private val getContent =
        registerForActivityResult(ActivityResultContracts.GetContent()) { uri: Uri? ->
            when (val gridFile = checkValidNonFile(uri, this)) {
                is Either.Left -> Log.d("asdf", gridFile.value.toString())
                is Either.Right -> {
                    addFile(gridFile.value.first, gridFile.value.second, this)
                    this.recreate()
                }
            }
        }

    private fun getAllCustomLevels(context: Context) = context.fileList().toMutableList()
}