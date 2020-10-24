package com.example.nonocross

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle

class RandomGameActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        LevelDetails.isRandom = true
        setContentView(R.layout.activity_game)
    }

    override fun onDestroy() {
        super.onDestroy()
        LevelDetails.isRandom = false
    }
}