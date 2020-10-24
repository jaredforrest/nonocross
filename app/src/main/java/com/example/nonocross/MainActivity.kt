package com.example.nonocross

import android.content.Intent
import android.os.Bundle
import android.view.View
import androidx.appcompat.app.AppCompatActivity


class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
    }

    /** Called when the user taps the Start Game button */
    fun openLevelSelect(view: View) {
        val intent = Intent(this, LevelSelectActivity::class.java)
        startActivity(intent)
    }
}