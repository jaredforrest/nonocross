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
import android.os.Bundle
import android.view.View
import android.view.View.GONE
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.GridLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.picross.nonocross.levelselect.CustomLevelSelectAdapter
import java.io.File
import java.io.FileInputStream
import java.io.FileNotFoundException

class LevelSelectActivity : AppCompatActivity(), CustomLevelSelectAdapter.StartGame {
    private lateinit var recyclerView: RecyclerView
    private lateinit var viewAdapter: RecyclerView.Adapter<*>
    private lateinit var viewManager: RecyclerView.LayoutManager

    override val levels = LevelDetails.levels

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_level_select)

        viewManager = GridLayoutManager(this, 3)
        viewAdapter = CustomLevelSelectAdapter(this, this)

        recyclerView = findViewById<RecyclerView>(R.id.my_recycler_view).apply {
            // use this setting to improve performance if you know that changes
            // in content do not change the layout size of the RecyclerView
            setHasFixedSize(true)

            // use a grid layout manager
            layoutManager = viewManager

            // specify an viewAdapter
            adapter = viewAdapter
        }
        findViewById<View>(R.id.import_level_button).visibility = GONE
    }

    override fun startGame() {
        val intent = Intent(this, GameActivity::class.java)
        startActivity(intent)
    }

    override fun removeLevel(level: String, position: Int) {
    }

    override fun levelType(levelName: String): LevelType =
        LevelType.Default(levelName)

    override fun openSave(levelName: String): ByteArray {
        val saveDir = getDir("saves", Context.MODE_PRIVATE)
        val saveFile =
            try {
                FileInputStream(File(File(saveDir, "default"), levelName))
            } catch (e: FileNotFoundException) {
                return byteArrayOf()
            }
        return saveFile.readBytes()
    }

}