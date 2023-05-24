package com.picross.nonocross.util.usergrid

import arrow.core.Option
import arrow.core.Some
import arrow.core.none
import com.picross.nonocross.util.CellShade

/** This object is purely to interact with the native code */
/*object CheckUnique {

    external fun checkUniqueFromJNI(string: String): Int

    init {
        System.loadLibrary("jsolve")
    }
}*/

/** This object is purely to interact with the native code */
object CheckUnique {

    external fun checkUniqueFromJNI(height0: Int, width0: Int, rowNumCount0: IntArray, colNumCount0: IntArray, rowNumsFlat: IntArray, colNumsFlat: IntArray): Int

    init {
        System.loadLibrary("jsolve")
    }
}

/** If it is unique return a GridData of it otherwise return None() */
fun List<CellShade>.checkUnique(attributes: GridAttributes): Option<GridData> {
    val rowNums = this.getRowNums(attributes.width, attributes.height)
    val rowNumCount = rowNums.map { it.size }.toIntArray()
    val colNums = this.getColNums(attributes.width, attributes.height)
    val colNumCount = colNums.map { it.size }.toIntArray()
    val check = CheckUnique.checkUniqueFromJNI(attributes.height, attributes.width, rowNumCount, colNumCount, rowNums.flatten().toIntArray(), colNums.flatten().toIntArray()) == 1

    return if (check) Some(GridData(attributes, rowNums, colNums))
    else none<GridData>()
}


/** If it is unique return a GridData of it otherwise return None() */
/*fun List<CellShade>.checkUnique(width: Int, height: Int): Option<GridData> {
    val rowNums = this.getRowNums(width, height)
    val colNums = this.getColNums(width, height)
    /* Convert the grid to a string jsolve.c can parse */
    val ast =
        rowNums.fold("")
        { acc, i -> acc + i.fold("") { acc2, j -> "$acc2$j " } + "0\n" } + "#\n" +
                colNums.fold("")
                { acc, i -> acc + i.fold("") { acc2, j -> "$acc2$j " } + "0\n" } + "#\n"

    Log.d("sfsf", ast)
    val check = CheckUnique.checkUniqueFromJNI(ast) == 1

    return if (check) Some(GridData(size / height, height, rowNums, colNums))
    else none<GridData>()
}*/
