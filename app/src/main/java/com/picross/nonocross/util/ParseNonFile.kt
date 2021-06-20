package com.picross.nonocross.util

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