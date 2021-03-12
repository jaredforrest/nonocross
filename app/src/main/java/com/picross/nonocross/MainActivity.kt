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
import android.content.SharedPreferences
import android.os.Bundle
import android.view.View
import android.widget.NumberPicker
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.preference.PreferenceManager


class MainActivity : AppCompatActivity() {

    private lateinit var preferences: SharedPreferences

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        preferences = PreferenceManager.getDefaultSharedPreferences(this)
    }

    fun onClick(view: View) {
        when (view.id) {
            R.id.random_level -> openRandomLevel()
            R.id.level_select -> openLevelSelect()
            R.id.preferences -> openPreferences()
        }
    }

    /** Called when the user taps the Start Level button */
    private fun openLevelSelect() {
        val intent = Intent(this, LevelSelectActivity::class.java)
        startActivity(intent)
    }

    /** Called when the user taps the Random Level button */
    private fun openRandomLevel() {
        val inflater = this.layoutInflater
        val dialogView: View = inflater.inflate(R.layout.dialogue_random_grid, null)
        val rowsPicker = dialogView.findViewById<NumberPicker>(R.id.rows_picker)
        val colsPicker = dialogView.findViewById<NumberPicker>(R.id.cols_picker)
        val diffPicker = dialogView.findViewById<NumberPicker>(R.id.difficulty_picker)
        rowsPicker.maxValue = 50
        rowsPicker.minValue = 2
        rowsPicker.value = preferences.getInt("rows", 10)
        rowsPicker.wrapSelectorWheel = false
        colsPicker.maxValue = 50
        colsPicker.minValue = 2
        colsPicker.value =
            preferences.getInt("columns", 10)
        colsPicker.wrapSelectorWheel = false
        diffPicker.maxValue = 10
        diffPicker.minValue = 1
        diffPicker.value = preferences.getInt("difficulty", 5)
        diffPicker.wrapSelectorWheel = false
        AlertDialog.Builder(this)
            .setTitle(R.string.grid_size)
            .setView(dialogView)
            .setPositiveButton(
                android.R.string.ok
            ) { _, _ ->
                val editor = preferences.edit()
                editor.putInt("columns", colsPicker.value)
                editor.putInt("rows", rowsPicker.value)
                editor.putInt("difficulty", diffPicker.value)
                editor.apply()

                LevelDetails.isRandom = true
                val intent = Intent(this, GameActivity::class.java)
                startActivity(intent)
            }
            .setNegativeButton(
                android.R.string.cancel
            ) { _, _ -> }
            .create()
            .show()
    }

    private fun openPreferences() {
        val intent = Intent(this, SettingsActivity::class.java)
        startActivity(intent)
    }
}