package com.picross.nonocross

import android.content.Context
import androidx.core.content.edit
import com.picross.nonocross.util.Preferences
import com.picross.nonocross.util.usergrid.UserGrid

object HighScoreManager {
    private const val NO_SCORE = -1L

    private fun preferenceKeyForHighScore(height: Int, width: Int, difficulty: Int) =
        "high_score-$height-$width-$difficulty"

    fun handleNewScore(context: Context, grid: UserGrid, difficulty: Int): Boolean {
        val key = preferenceKeyForHighScore(grid.height, grid.width, difficulty)
        val preferences = Preferences(context).sharedPrefs
        val currentHigh = preferences.getLong(key, NO_SCORE)
        val time = grid.timeElapsed.toLong()
        if (currentHigh == NO_SCORE || time < currentHigh) {
            preferences.edit {
                putLong(key, time)
            }
            return true
        }
        return false
    }

    fun getHighScore(context: Context, height: Int, width: Int, difficulty: Int): UInt? {
        val key = preferenceKeyForHighScore(height, width, difficulty)
        val preferences = Preferences(context).sharedPrefs
        val currentHigh = preferences.getLong(key, NO_SCORE)
        if (currentHigh == NO_SCORE) {
            return null
        }
        return currentHigh.toUInt()
    }
}