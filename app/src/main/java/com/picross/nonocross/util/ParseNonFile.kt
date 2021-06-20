package com.picross.nonocross.util

import android.util.Log

/** Takes a .non file (represented as a list of strings by each new line) and outputs the grid */
fun parseNonFile(lines: String): GridData2 {

    fun parseLine(line: String, data: Pair<Int, Int>): Pair<Int, Int> {
        return when (line.substringBefore(" ")) {
            "width" -> Pair(line.substringAfter(" ").toInt(), data.second)
            "height" -> Pair(data.first, line.substringAfter(" ").toInt())
            else -> data
        }
    }

    tailrec fun heightWidthAcc(
        lines: String,
        data: Pair<Int, Int> = Pair(0, 0)
    ): Pair<String, Pair<Int, Int>> {
        return if (lines == "" || (data.first != 0 && data.second != 0)) Pair(lines, data)
        else {
            heightWidthAcc(
                lines.dropWhile { it != '\n' }.drop(1),
                parseLine(lines.takeWhile { it != '\n' }, data)
            )
        }
    }

    fun parseNum(rowNum: String): List<Int> {
        return if (rowNum == "0") listOf()
        else rowNum.split(',').map { it.toInt() }
    }

    tailrec fun parseNums(
        lines: String,
        size: Int,
        acc: List<List<Int>> = listOf()
    ): Pair<String, List<List<Int>>> {
        Log.d("lines num", "$size\n$lines ")
        return if (size == 0) Pair(lines, acc)
        else parseNums(
            lines.substringAfter('\n'),
            size - 1,
            acc + listOf(parseNum(lines.substringBefore('\n')))
        )
    }

    fun parseLine2(
        lines: String,
        widthHeight: Pair<Int, Int>,
        data: Pair<List<List<Int>>, List<List<Int>>>
    ): Triple<String, List<List<Int>>, List<List<Int>>> {
        return when (lines.substringBefore('\n')) {
            "rows" -> {
                val output = parseNums(lines.substringAfter('\n'), widthHeight.second)
                Triple(output.first, output.second, data.second)
            }
            "columns" -> {
                Log.d("liness", lines.substringBefore('\n'))
                val output = parseNums(lines.substringAfter('\n'), widthHeight.first)
                Triple(output.first, data.first, output.second)
            }
            else -> Triple(lines.substringAfter('\n'), data.first, data.second)
        }
    }

    tailrec fun rowColAcc(
        lines: String,
        widthHeight: Pair<Int, Int>,
        data: Pair<List<List<Int>>, List<List<Int>>> = Pair(
            listOf(),
            listOf()
        )
    ): Triple<String, Pair<Int, Int>, Pair<List<List<Int>>, List<List<Int>>>> {
        return if (lines == "" || (data.first.isNotEmpty() && data.second.isNotEmpty())) Triple(
            lines,
            widthHeight,
            data
        )
        else {
            val output = parseLine2(lines, widthHeight, data)
            rowColAcc(output.first, widthHeight, Pair(output.second, output.third))
        }
    }

    val (lines2, widthHeight) = heightWidthAcc(lines)
    //if(widthHeight.first == 0 || widthHeight.second == 0) exitProcess(1)
    val (_, widthHeight2, rowNumscolNums) = rowColAcc(lines2, widthHeight)

    return GridData2(
        widthHeight2.first,
        widthHeight2.second,
        rowNumscolNums.first,
        rowNumscolNums.second
    )


}


val cool =
    """
catalogue "webpbn.com #529"
title "Swing"
by "Jan Wolter"
copyright "© 2006 Jan Wolter"
license CC-BY-3.0
width 46
height 45

rows
7,1,1,1,1,1
3,1,3,1,4,1,4,1,5,1,5,1,2
1,1,1,3,1,4,1,4,1,5,1,5,1,2
2,1,2,1,1,1,1,6,2
3,30,1,5
1,5,8,1,1,7,1,1,3
3,4,8,1,5,1,2
3,20,6,6
3,3,7,2,5,1
3,3,1,1,9,1,1,5,6
2,3,8,1,3,4,2
5,3,1,10,4,5,2
1,2,3,8,4,6
2,2,3,11,10
2,2,3,10,7
2,3,1,7,12,2
2,3,1,4,11,2
4,1,2,1,11,2
9,1,2,9
6,2,1,4,11
2,5,1,2,6,6
6,2,4,8,4
4,2,16,1
2,2,15,2
3,2,15,4
3,3,13,4
4,12,9
1,9,10
2,1,17,7,2
2,2,8,3,8,2
2,3,6,3,8,2
2,4,5,4,7,2
2,5,5,4,6
4,4,5,4,9
1,4,6,4,4
4,3,6,4,3,2
2,1,2,7,4,4,2
2,2,2,9,5,5,2
2,2,2,10,6,6
3,2,1,9,18
8,4,23
1,2,1,2,2,1,1,1,2
2,1,4,2,1,4,1,5,1,3,1,2
2,1,5,4,4,1,5,1,3,1,2
1,10,1,1,1

columns
7,1,1,1,1,1
2,2,4,1,4,1,5,1,4,1,4,1,2
3,1,4,1,4,1,14,4,1,2
1,1,5,1,2,3,4,1
3,13,1,10
1,9,4
6,7,2,2
8,4,1,4
2,8,3,2,5,3
10,1,3,7,2
8,6,2,8,1,2
1,1,2,2,8,1,1
2,1,1,1,2,1,3,1,3,3,1
2,1,1,1,5,4,2,1
2,1,1,1,1,7,2,1
2,1,1,2,9,1,2,1
4,6,12,1,3
16,13,3,2
12,21,2
2,13,23
2,14,19
2,14,20,2
2,13,7,2,8,2
12,8,1,7,2
5,1,1,1,2,8,1,5,2
2,1,1,1,9,1,1,4
2,1,1,1,6,1,3,5
2,2,1,5,6,2
2,1,3,1,3,7,3,2
2,3,2,1,1,2,4,4,2
2,2,1,1,2,3,1,8,2
9,3,1,7,2
12,4,1,6,2
7,4,1,2,5
2,6,6,5,6
8,8,6,3
3,10,8,4,2
5,11,9,5,2
3,1,12,16,2
3,1,12,16
5,2,13,21
6,1,3,3,1,1
5,1,3,1,3,1,1,2,1,4,1,3,1,3

5,1,3,1,3,1,4,1,4,1,3,1,3
1,1,1,1,1,1
1

goal "111111100010000001000000100000001000000010000111010111010111101011110101111101011111010110101010111010111101011110101111101011111010110110100110010000001000000100000001001111110110111000111111111111111111111111111111010111110100000111110000011111111010101111111010101110111000011110000011111111000001011111001011000011100011111111111111111111000111111000111111011100011100000001111111000110011111000001000011100001110101011111111101010111110001111110000110001110000011111111000010011100011110110111110001110100011111111110001111000111110110000010000110111011111111000011110001111110000011011000111000111111111110000000001111111111011011000011100111111111100000000011111110000011011100000010001111111000001111111111110110011011100000010000011110000000111111111110110000011110000010000001100010000111111111110110111111111000010000000000110000001111111110000000011111100110010000001111000000011111111111011011111000100011000011111100000000111111000011111100001100111100111111110000000000011110011110000011001111111111111111000000000000010011000000110001111111111111110000000000000110011100001100011111111111111100000000000011110001110011100011111111111110000000000001111000111100000000111111111111000000000000111111111001000000000011111111100000000011111111110000011000001000001111111111111111100011111110110011000001100000111111110000111000111111110110011000001110000011111100001110000111111110110011000001111000001111100011110000011111110110001100001111100001111100001111000001111110000111100000111100000111110000111100000111111111000100000111100000111111000011110000011110000011110000111000000111111000001111000001110110011010000011000000111111100000111100011110110011011000011000001111111110001111100111110110011011000001100011111111110011111101111110000000011100000110101111111110111111111111111111111111110001111000111111111111111111111110000000010011010001100110000001000000010000010110011010111100000110010111101011111010111010110011010111110000011110111101011111010111010110000010000011111111110000001000000010000010000"
        """.trimIndent()