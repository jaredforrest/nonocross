package com.picross.nonocross.views

import com.google.android.material.slider.Slider

fun Slider.intValue(value: Int) {
    this.value = value.toFloat()
}

fun Slider.intValue() = value.toInt()