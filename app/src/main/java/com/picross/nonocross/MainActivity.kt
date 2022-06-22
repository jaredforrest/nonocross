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
import android.content.SharedPreferences
import android.os.Bundle
import android.view.View
import android.view.View.INVISIBLE
import android.view.View.VISIBLE
import android.widget.NumberPicker
import androidx.activity.addCallback
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.appcompat.app.AppCompatDelegate
import androidx.lifecycle.lifecycleScope
import androidx.preference.PreferenceManager
import arrow.core.Either
import arrow.core.None
import arrow.core.Some
import com.picross.nonocross.util.*
import com.picross.nonocross.util.usergrid.GridData
import com.picross.nonocross.util.usergrid.UserGrid
import com.picross.nonocross.util.usergrid.toGridData
import kotlinx.collections.immutable.persistentListOf
import kotlinx.collections.immutable.plus
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.io.File


class MainActivity : AppCompatActivity() {

    private lateinit var preferences: SharedPreferences
    private lateinit var randomLevelButton: View
    private lateinit var levelSelectButton: View
    private lateinit var preferencesButton: View
    private lateinit var savedLevelSelect: View

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        initialize()
        File(this.getDir("saves", Context.MODE_PRIVATE), "default").mkdirs()
        File(this.getDir("saves", Context.MODE_PRIVATE), "custom").mkdirs()
    }

    override fun onResume() {
        super.onResume()
        initialize()
    }

    /** Called when the user taps the Start Level button */
    private fun openLevelSelect() {
        lifecycleScope.launch(Dispatchers.IO) {

            val levelNames = persistentListOf<String>() + (resources.assets.list("levels")?.toList() ?: persistentListOf())

            LevelDetails.levels = persistentListOf<Pair<String,GridData>>() + levelNames.map { levelName ->
                when(val gridData = openGridFile(this@MainActivity,levelName,false)){
                    is Either.Left -> {
                        runOnUiThread { errorToast(this@MainActivity, gridData.value.toString()) }
                        return@launch
                    }
                    is Either.Right ->
                        Pair(levelName, gridData.value)
                }
            }

            val intent = Intent(this@MainActivity, LevelSelectActivity::class.java)
            startActivity(intent)
        }
    }

    /** Called when the user taps the Custom Level button */
    private fun openCustomLevel() {
        lifecycleScope.launch(Dispatchers.IO) {

            val levelNames = persistentListOf<String>() + (getDir("levels", Context.MODE_PRIVATE).listFiles()?.map { it.name } ?: persistentListOf())

            LevelDetails.customLevels = persistentListOf<Pair<String, GridData>>() + levelNames.map { levelName ->
                when(val gridData = openGridFile(this@MainActivity,levelName,true)){
                    is Either.Left -> {
                        runOnUiThread { errorToast(this@MainActivity, gridData.value.toString()) }
                        return@launch
                    }
                    is Either.Right ->
                        Pair(levelName, gridData.value)
                }
            }

            val intent = Intent(this@MainActivity, CustomLevelSelectActivity::class.java)
            startActivity(intent)
        }
    }

    /** Called when the user taps the Random Level button */
    private fun openRandomLevel() {
        val inflater = this@MainActivity.layoutInflater
        val dialogView: View = inflater.inflate(R.layout.dialogue_random_grid, null)
        val rowsPicker = dialogView.findViewById<NumberPicker>(R.id.rows_picker)
        val colsPicker = dialogView.findViewById<NumberPicker>(R.id.cols_picker)
        val diffPicker = dialogView.findViewById<NumberPicker>(R.id.difficulty_picker)
        rowsPicker.maxValue = 25
        rowsPicker.minValue = 2
        rowsPicker.value = preferences.getInt("rows", 10)
        rowsPicker.wrapSelectorWheel = false
        colsPicker.maxValue = 25
        colsPicker.minValue = 2
        colsPicker.value = preferences.getInt("columns", 10)
        colsPicker.wrapSelectorWheel = false
        diffPicker.maxValue = 10
        diffPicker.minValue = 1
        diffPicker.value = preferences.getInt("difficulty", 5)
        diffPicker.wrapSelectorWheel = false
        AlertDialog.Builder(this@MainActivity)
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

                LevelDetails.levelType = LevelType.Random()

                if (preferences.getBoolean("uniqueLevel", true)) {
                    loadingMenu(true)

                    val randomJob = lifecycleScope.launch(Dispatchers.IO) {
                        when (val temp =
                            newUniqueRandomGrid(getRandomGridPrefs(this@MainActivity))) {
                            is Some -> {
                                LevelDetails.gridData = temp.value
                                LevelDetails.userGrid = UserGrid(LevelDetails.gridData, autoFill = true, resetComplete = preferences.getBoolean("resetComplete", true))
                                val intent = Intent(this@MainActivity, GameActivity::class.java)
                                startActivity(intent)
                            }
                            is None -> TODO()
                        }
                    }
                    onBackPressedDispatcher.addCallback(this@MainActivity) {
                        randomJob.cancel()
                        loadingMenu(false)
                        onBackPressedDispatcher.addCallback(this@MainActivity) { finish() }
                    }
                } else {
                    val wHD = getRandomGridPrefs(this)
                    LevelDetails.gridData = newRandomGrid(wHD).toGridData(wHD.second)
                    LevelDetails.userGrid = UserGrid(LevelDetails.gridData, autoFill = true, resetComplete = preferences.getBoolean("resetComplete", true))
                    val intent = Intent(this, GameActivity::class.java)
                    startActivity(intent)
                }

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

    private fun initialize() {
        setContentView(R.layout.activity_main)

        preferences = PreferenceManager.getDefaultSharedPreferences(this)
        when (preferences.getString("darkMode2", "System")) {
            "System" -> {
                AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_FOLLOW_SYSTEM)
            }
            "Dark" -> {
                AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_YES)
            }
            else -> {
                AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_NO)
            }
        }

        randomLevelButton = findViewById(R.id.random_level)
        levelSelectButton = findViewById(R.id.level_select)
        preferencesButton = findViewById(R.id.preferences)
        savedLevelSelect = findViewById(R.id.custom_level)

        randomLevelButton.setOnClickListener { openRandomLevel() }
        levelSelectButton.setOnClickListener { openLevelSelect() }
        preferencesButton.setOnClickListener { openPreferences() }
        savedLevelSelect.setOnClickListener { openCustomLevel() }

    }

    /** Enable gives a choice whether to show or hide the loading menu */
    private fun loadingMenu(enable: Boolean) {
        val visibility = if (enable) VISIBLE else INVISIBLE
        val invisibility = if (enable) INVISIBLE else VISIBLE
        randomLevelButton.visibility = invisibility
        levelSelectButton.visibility = invisibility
        preferencesButton.visibility = invisibility
        savedLevelSelect.visibility = invisibility
        findViewById<View>(R.id.game_text).visibility = invisibility
        findViewById<View>(R.id.indeterminateBar).visibility = visibility
    }

}