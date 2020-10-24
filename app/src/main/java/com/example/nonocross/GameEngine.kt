package com.example.nonocross

import android.content.Context
import com.example.nonocross.util.GridData
import com.example.nonocross.util.generate

object GameEngine {


    fun createGrid(context: Context): GridData {

        // Create the grid from file and store it
        return generate(context)

    }

    /*fun endGame(nonocrossGrid: List<List<Cell>>) {
        for(row in nonocrossGrid){
            for(cell in row)
                //if(!cell.checkCell()){return}
        }
        println("Grid Solved")
    }*/

}

object LevelDetails {
    var levelName = "Level 1"
    var levelComplete = false
}

