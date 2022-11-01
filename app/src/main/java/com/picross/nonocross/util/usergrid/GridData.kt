package com.picross.nonocross.util.usergrid

import java.security.InvalidParameterException

data class GridData(
    val width: Int,
    val height: Int,
    val rowNums: List<List<Int>>,
    val colNums: List<List<Int>>,
) {

    init {
        if (height <= 0) throw InvalidParameterException("Height must be greater than 0")
        else if (width <= 0) throw InvalidParameterException("Width must be greater than 0")
    }

    /** Gets the width and height of the longest row and column */
    val longestRowNum =
        rowNums.fold(
            0
        ) { acc, i -> (i.size + if (i.count { it >= 10 } > 0) 1 else 0).coerceAtLeast(acc) }
    val longestColNum = colNums.fold(0) { acc, i -> i.size.coerceAtLeast(acc) }
}
