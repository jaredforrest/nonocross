package com.example.nonocross

import android.content.Intent
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.GridLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.example.nonocross.util.getAllLevels
import com.example.nonocross.levelselect.LevelSelectAdapter

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
        LevelDetails.levelName = levelName
        val intent = Intent(this, GameActivity::class.java)
        startActivity(intent)
    }

}