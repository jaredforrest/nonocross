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

import android.content.Intent
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.GridLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.picross.nonocross.levelselect.LevelSelectAdapter
import com.picross.nonocross.util.getAllLevels

class LevelSelectActivity : AppCompatActivity(), LevelSelectAdapter.StartGame {
    private lateinit var recyclerView: RecyclerView
    private lateinit var viewAdapter: RecyclerView.Adapter<*>
    private lateinit var viewManager: RecyclerView.LayoutManager

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_level_select)

        viewManager = GridLayoutManager(this, 3)
        viewAdapter = LevelSelectAdapter(getAllLevels(this),this, this)

        recyclerView = findViewById<RecyclerView>(R.id.my_recycler_view).apply {
            // use this setting to improve performance if you know that changes
            // in content do not change the layout size of the RecyclerView
            setHasFixedSize(true)

            // use a grid layout manager
            layoutManager = viewManager

            // specify an viewAdapter
            adapter = viewAdapter

        }
    }

    override fun startGame(levelName: String) {
        LevelDetails.isRandom = false
        LevelDetails.levelName = levelName
        val intent = Intent(this, GameActivity::class.java)
        startActivity(intent)
    }

}

fun getAllLevels(context: Context): List<String> {
    return context.resources.assets.list("levels")?.toList() ?: listOf()
}