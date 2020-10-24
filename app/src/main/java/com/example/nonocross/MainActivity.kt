package com.example.nonocross

import android.content.Intent
import android.os.Bundle
import android.view.View
import android.widget.NumberPicker
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity


class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
    }

    /** Called when the user taps the Start Level button */
    fun openLevelSelect(view: View) {
        val intent = Intent(this, LevelSelectActivity::class.java)
        startActivity(intent)
    }

    /** Called when the user taps the Random Level button */
    fun openRandomLevel(view: View) {

        val inflater = this.layoutInflater
        val dialogView: View = inflater.inflate(R.layout.dialogue_random_grid, null)
        val rowsPicker = dialogView.findViewById<NumberPicker>(R.id.rows_picker)
        rowsPicker.maxValue = 10
        rowsPicker.minValue = 2
        rowsPicker.value = 10
        rowsPicker.wrapSelectorWheel = false
        rowsPicker.setOnValueChangedListener {_,_,_->}
        val colsPicker = dialogView.findViewById<NumberPicker>(R.id.cols_picker)
        colsPicker.maxValue = 50
        colsPicker.minValue = 2
        colsPicker.value = 10
        colsPicker.wrapSelectorWheel = false
        colsPicker.setOnValueChangedListener { _, _, _ ->
            rowsPicker.maxValue = colsPicker.value
        }
        AlertDialog.Builder(this)
        .setTitle("Grid Size")
        .setMessage("Choose Number of Rows and Columns")
        .setView(dialogView)
        .setPositiveButton(
            android.R.string.ok
        ) { _, _ -> LevelDetails.randomGridRowsCols = Pair(rowsPicker.value, colsPicker.value)
            val intent = Intent(this, RandomGameActivity::class.java)
            startActivity(intent)
        }
        .setNegativeButton(
            android.R.string.cancel
        ) { _, _ -> }
        .create()
        .show()

    }
}