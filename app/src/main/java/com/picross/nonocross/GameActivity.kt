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
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import androidx.appcompat.widget.SwitchCompat
import com.picross.nonocross.views.grid.GameView
import com.picross.nonocross.views.grid.GridView

class GameActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_game)

        LevelDetails.toggleCross = false
        findViewById<SwitchCompat>(R.id.toggleCross).setOnCheckedChangeListener { _, isChecked ->
            LevelDetails.toggleCross = isChecked
        }
    }

    /*
    override fun onStop() {
        (findViewById<GameView>(R.id.nonocrossGameView).getChildAt(2) as GridView).clear()
        super.onStop()
    }
    */

    fun onClick(view: View) {
        when (view.id) {
            R.id.undo -> {
                (findViewById<GameView>(R.id.nonocrossGameView).getChildAt(2) as GridView).undo()
                findViewById<GameView>(R.id.nonocrossGameView).getChildAt(0).invalidate()
                findViewById<GameView>(R.id.nonocrossGameView).getChildAt(1).invalidate()
            }
            R.id.clear -> {
                (findViewById<GameView>(R.id.nonocrossGameView).getChildAt(2) as GridView).clear()
                findViewById<GameView>(R.id.nonocrossGameView).getChildAt(0).invalidate()
                findViewById<GameView>(R.id.nonocrossGameView).getChildAt(1).invalidate()
            }
        }
    }
}

