package com.example.nonocross.util

fun printGrid(gridData: GridData) {

    for (i in gridData.rowNums.indices) {
        println(gridData.rowNums[i].toString())
    }
    for (i in gridData.colNums.indices) {
        println(gridData.colNums[i].toString())
    }

    for (i in gridData.grid.indices) {
        println(gridData.grid[i].toString())
    }
}