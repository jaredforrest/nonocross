package com.picross.nonocross.util

import android.content.Context
import android.content.SharedPreferences
import androidx.appcompat.app.AppCompatDelegate
import androidx.appcompat.app.AppCompatDelegate.NightMode
import androidx.core.content.edit
import androidx.preference.PreferenceManager

class Preferences(context: Context) {
    var sharedPrefs: SharedPreferences = PreferenceManager.getDefaultSharedPreferences(context)
        private set

    val vibrate: Boolean
        get() {
            return sharedPrefs.getBoolean("vibrate", false)
        }

    val fatFingerMode: Boolean
        get() {
            return sharedPrefs.getBoolean("fatFinger", true)
        }

    val uniqueLevel: Boolean
        get() {
            return sharedPrefs.getBoolean("uniqueLevel", true)
        }

    val resetComplete: Boolean
        get() {
            return sharedPrefs.getBoolean("resetComplete", true)
        }

    val enableZoom: Boolean
        get() {
            return sharedPrefs.getBoolean("enable_zoom", true)
        }

    val fillMode: FillMode
        get() {
            return when(sharedPrefs.getString("fillMode", "")) {
                "Lax" -> FillMode.Lax
                "Strict" -> FillMode.Strict
                else -> FillMode.Default
            }
        }

    enum class FillMode {
        Default,
        Lax,
        Strict
    }

    @NightMode
    val theme: Int
        get() {
            return when (sharedPrefs.getString("darkMode2", "")) {
                "Dark" -> AppCompatDelegate.MODE_NIGHT_YES
                "Light" -> AppCompatDelegate.MODE_NIGHT_NO
                else -> AppCompatDelegate.MODE_NIGHT_FOLLOW_SYSTEM
            }
        }

    val showHints: Boolean
        get() {
            return sharedPrefs.getBoolean("showBlueHints", false)
        }

    val showSaveWarning: Boolean
        get() {
            return sharedPrefs.getBoolean("saveWarn", true)
        }

    val trackTimer: Boolean
        get() {
            return sharedPrefs.getBoolean("tracktimer", true)
        }

    val showTimer: Boolean
        get() {
            return sharedPrefs.getBoolean("timer", true)
        }

    var randomGridWidth: Int
        get() {
            return sharedPrefs.getInt("columns", 10)
        }
        set(value) {
            sharedPrefs.edit {
                putInt("columns", value)
            }
        }

    var randomGridHeight: Int
        get() {
            return sharedPrefs.getInt("rows", 10)
        }
        set(value) {
            sharedPrefs.edit {
                putInt("rows", value)
            }
        }

    var randomGridDifficulty: Int
        get() {
            return sharedPrefs.getInt("difficulty", 5)
        }
        set(value) {
            sharedPrefs.edit {
                putInt("difficulty", value)
            }
        }
}