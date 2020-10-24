package com.example.nonocross

import android.content.Context
import com.example.nonocross.util.GridData
import com.example.nonocross.util.generate

object GameEngine {


    fun createGrid(context: Context): GridData {

        // Create the grid from file and store it
        return generate(context)

    }
}

object LevelDetails {
    var levelName = "Level 1"
    var isRandom = false
    var randomGridRowsCols = Pair(10, 10)
}

