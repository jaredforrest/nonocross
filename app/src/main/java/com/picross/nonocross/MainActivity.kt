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
import android.os.Build
import android.os.Bundle
import android.view.View
import android.view.View.INVISIBLE
import android.view.View.VISIBLE
import androidx.activity.addCallback
import androidx.appcompat.app.AppCompatActivity
import androidx.appcompat.app.AppCompatDelegate
import androidx.core.content.pm.ShortcutInfoCompat
import androidx.core.content.pm.ShortcutManagerCompat
import androidx.core.graphics.drawable.IconCompat
import androidx.lifecycle.lifecycleScope
import arrow.core.Either
import arrow.core.None
import arrow.core.Some
import com.google.android.material.dialog.MaterialAlertDialogBuilder
import com.google.android.material.slider.Slider
import com.picross.nonocross.databinding.DialogueRandomGridBinding
import com.picross.nonocross.util.Preferences
import com.picross.nonocross.util.errorToast
import com.picross.nonocross.util.getRandomGridPrefs
import com.picross.nonocross.util.newRandomGrid
import com.picross.nonocross.util.newUniqueRandomGrid
import com.picross.nonocross.util.openGridFile
import com.picross.nonocross.util.secondsToTime
import com.picross.nonocross.util.usergrid.GridData
import com.picross.nonocross.util.usergrid.UserGrid
import com.picross.nonocross.util.usergrid.toGridData
import com.picross.nonocross.views.intValue
import kotlinx.collections.immutable.persistentListOf
import kotlinx.collections.immutable.plus
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.io.File


class MainActivity : AppCompatActivity() {

    private lateinit var preferences: Preferences
    private lateinit var randomLevelButton: View
    private lateinit var levelSelectButton: View
    private lateinit var preferencesButton: View
    private lateinit var savedLevelSelect: View

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        initialize()
        File(this.getDir("saves", Context.MODE_PRIVATE), "default").mkdirs()
        File(this.getDir("saves", Context.MODE_PRIVATE), "custom").mkdirs()
        handleIntent(intent)
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N_MR1) {
            ShortcutManagerCompat.addDynamicShortcuts(this, listOf(getShortcutInfo()))
        }
    }

    override fun onResume() {
        super.onResume()
        initialize()
    }

    override fun onNewIntent(intent: Intent) {
        super.onNewIntent(intent)
        handleIntent(intent)
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
        val binding = DialogueRandomGridBinding.inflate(this@MainActivity.layoutInflater)
        binding.rowsSlider.intValue(preferences.randomGridHeight)
        binding.columnsSlider.intValue(preferences.randomGridWidth)
        binding.difficultySlider.intValue(preferences.randomGridDifficulty)

        fun updateTextViews() {
            val highScore = HighScoreManager.getHighScore(
                this,
                binding.rowsSlider.intValue(),
                binding.columnsSlider.intValue(),
                binding.difficultySlider.intValue()
            )
            binding.highscoreTextView.text = if (highScore == null) {
                getString(R.string.no_high_score)
            } else {
                getString(R.string.high_score, secondsToTime(highScore))
            }

            binding.rowsTextView.text = getString(R.string.rows, binding.rowsSlider.intValue())
            binding.columnsTextView.text = getString(R.string.columns, binding.columnsSlider.intValue())
            binding.difficultyTextView.text = getString(R.string.difficulty, binding.difficultySlider.intValue())
        }

        val listener = Slider.OnChangeListener { _, _, _ ->
            updateTextViews()
        }

        binding.rowsSlider.addOnChangeListener(listener)
        binding.columnsSlider.addOnChangeListener(listener)
        binding.difficultySlider.addOnChangeListener(listener)
        updateTextViews()

        MaterialAlertDialogBuilder(this@MainActivity)
            .setTitle(R.string.random_grid)
            .setView(binding.root)
            .setPositiveButton(
                android.R.string.ok
            ) { _, _ ->
                preferences.randomGridWidth = binding.columnsSlider.intValue()
                preferences.randomGridHeight = binding.rowsSlider.intValue()
                preferences.randomGridDifficulty = binding.difficultySlider.intValue()

                startRandomLevel()
            }
            .setNegativeButton(
                android.R.string.cancel
            ) { _, _ -> }
            .create()
            .show()
    }

    private fun startRandomLevel() {
        LevelDetails.levelType = LevelType.Random()
        if (preferences.uniqueLevel) {
            loadingMenu(true)

            val randomJob = lifecycleScope.launch(Dispatchers.IO) {
                when (val temp =
                    newUniqueRandomGrid(getRandomGridPrefs(this@MainActivity))) {
                    is Some -> {
                        LevelDetails.gridData = temp.value
                        LevelDetails.userGrid = UserGrid(LevelDetails.gridData, autoFill = true, resetComplete = preferences.resetComplete)
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
            val gridAttributes = getRandomGridPrefs(this)
            LevelDetails.gridData = newRandomGrid(gridAttributes).toGridData(gridAttributes)
            LevelDetails.userGrid = UserGrid(LevelDetails.gridData, autoFill = true, resetComplete = preferences.resetComplete)
            val intent = Intent(this, GameActivity::class.java)
            startActivity(intent)
        }
    }

    private fun openPreferences() {
        val intent = Intent(this, SettingsActivity::class.java)
        startActivity(intent)
    }

    private fun handleIntent(intent: Intent) {
        if (intent.action == ACTION_RANDOM) {
            startRandomLevel()
        }
    }

    private fun initialize() {
        setContentView(R.layout.activity_main)

        preferences = Preferences(this)
        AppCompatDelegate.setDefaultNightMode(preferences.theme)

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

    private fun getShortcutInfo(): ShortcutInfoCompat {
        val intent = Intent(this, MainActivity::class.java)
            .setAction(ACTION_RANDOM)
        return ShortcutInfoCompat.Builder(this, "random")
            .setIntent(intent)
            .setShortLabel(getString(R.string.random_grid_short))
            .setLongLabel(getString(R.string.random_grid))
            .setIcon(IconCompat.createWithResource(this, R.mipmap.ic_launcher))
            .setAlwaysBadged()
            .build()
    }

    companion object {
        private const val ACTION_RANDOM = "action_random"
    }

}