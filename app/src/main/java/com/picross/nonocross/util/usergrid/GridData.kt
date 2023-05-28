package com.picross.nonocross.util.usergrid

import java.security.InvalidParameterException

data class GridAttributes(
    var width: Int,
    var height: Int,
    var difficulty: Int?
) {
    val isInitialized
        get() = width != 0 && height != 0

    companion object {
        fun empty() = GridAttributes(0, 0, null)
    }
}
data class GridData(
    val attributes: GridAttributes,
    val rowNums: List<List<Int>>,
    val colNums: List<List<Int>>,
) {

    init {
        if (attributes.height <= 0) throw InvalidParameterException("Height must be greater than 0")
        else if (attributes.width <= 0) throw InvalidParameterException("Width must be greater than 0")
    }

    /** Gets the width and height of the longest row and column */
    val longestRowNum =
        rowNums.fold(
            0
        ) { acc, i -> (i.size + if (i.count { it >= 10 } > 0) 1 else 0).coerceAtLeast(acc) }
    val longestColNum = colNums.fold(0) { acc, i -> i.size.coerceAtLeast(acc) }
}
