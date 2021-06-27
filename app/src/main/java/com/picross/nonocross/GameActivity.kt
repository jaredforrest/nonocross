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

import android.app.AlertDialog
import android.content.ClipData
import android.content.ClipboardManager
import android.content.Context
import android.os.Bundle
import android.view.View
import android.widget.TextView
import android.widget.Toast
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

        val undo: View = findViewById(R.id.undo)
        val clear: View = findViewById(R.id.clear)
        val info: View = findViewById(R.id.info)

        undo.setOnClickListener {
            (findViewById<GameView>(R.id.nonocrossGameView).getChildAt(2) as GridView).undo()
            findViewById<GameView>(R.id.nonocrossGameView).getChildAt(0).invalidate()
            findViewById<GameView>(R.id.nonocrossGameView).getChildAt(1).invalidate()
        }
        clear.setOnClickListener {
            (findViewById<GameView>(R.id.nonocrossGameView).getChildAt(2) as GridView).clear()
            findViewById<GameView>(R.id.nonocrossGameView).getChildAt(0).invalidate()
            findViewById<GameView>(R.id.nonocrossGameView).getChildAt(1).invalidate()
        }
        info.setOnClickListener {
            if (LevelDetails.isRandom) {
                val inflater = this.layoutInflater
                val dialogView: View = inflater.inflate(R.layout.seed_view, null)
                val seedDialog = AlertDialog.Builder(this)
                    .setView(dialogView)
                    .create()

                val seedText = dialogView.findViewById<TextView>(R.id.seed_text)
                val copySeedButton = dialogView.findViewById<TextView>(R.id.copy_seed_button)
                copySeedButton.setOnClickListener {
                    val clipboard =
                        getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
                    val clip: ClipData = ClipData.newPlainText(
                        "Random Seed",
                        LevelDetails.randomSeed.toString()
                    )
                    clipboard.setPrimaryClip(clip)
                    seedDialog.dismiss()
                    Toast.makeText(this, R.string.seed_copied, Toast.LENGTH_SHORT).show()
                    /*Snackbar.make(
                        findViewById(R.id.myCoordinatorLayout),
                        R.string.seed_copied,
                        LENGTH_SHORT
                    ).show()*/
                }
                seedText.text = resources.getString(R.string.long_seed, LevelDetails.randomSeed)
                seedDialog.show()
            }
        }

    }

    /*
    override fun onStop() {
        (findViewById<GameView>(R.id.nonocrossGameView).getChildAt(2) as GridView).clear()
        super.onStop()
    }
    */
}