package com.picross.nonocross.util

import android.content.Context
import android.net.Uri
import android.provider.OpenableColumns
import arrow.core.*
import arrow.core.computations.either
import com.picross.nonocross.util.usergrid.GridData


/** Takes a .non file (represented as a list of strings by each new line) and outputs the grid */
suspend fun parseNonFile(lines: String): Either<ParseError, GridData> {

    fun parseLine(line: String, data: Pair<Int, Int>): Pair<Int, Int> =
        when (line.substringBefore(" ")) {
            "width" -> Pair(line.substringAfter(" ").toInt(), data.second)
            "height" -> Pair(data.first, line.substringAfter(" ").toInt())
            else -> data
        }

    tailrec fun heightWidthAcc(
        lines: String,
        data: Pair<Int, Int> = Pair(0, 0)
    ): Either<ParseError.WidthHeightError, Pair<String, Pair<Int, Int>>> =
        if (data.first != 0 && data.second != 0) Either.Right(Pair(lines, data))
        else if (lines == "") Either.Left(ParseError.WidthHeightError)
        else {
            heightWidthAcc(
                lines.dropWhile { it != '\n' }.drop(1),
                parseLine(lines.takeWhile { it != '\n' }, data)
            )
        }

    fun parseNum(rowNum: String): Option<List<Int>> =
        if (rowNum == "0") Some(listOf())
        else {
            Some(rowNum.split(',').map { it.toIntOrNull() ?: return none() })
        }

    tailrec fun parseNums(
        lines: String,
        size: Int,
        acc: List<List<Int>> = listOf()
    ): Option<Pair<String, List<List<Int>>>> = if (size == 0) Some(Pair(lines, acc))
    else when (val next = parseNum(lines.substringBefore('\n'))) {
        is Some -> parseNums(
            lines.substringAfter('\n'),
            size - 1,
            acc + listOf(next.value)
        )
        is None -> None
    }

    fun parseLine2(
        lines: String,
        widthHeight: Pair<Int, Int>,
        data: Pair<List<List<Int>>, List<List<Int>>>
    ): Option<Triple<String, List<List<Int>>, List<List<Int>>>> =
        when (lines.substringBefore('\n')) {
            "rows" -> {
                when (val output = parseNums(lines.substringAfter('\n'), widthHeight.second)) {
                    is Some -> Some(Triple(output.value.first, output.value.second, data.second))
                    is None -> None
                }
            }
            "columns" -> {
                when (val output = parseNums(lines.substringAfter('\n'), widthHeight.first)) {
                    is Some -> Some(Triple(output.value.first, data.first, output.value.second))
                    is None -> None
                }

            }
            else -> Some(Triple(lines.substringAfter('\n'), data.first, data.second))
        }

    tailrec fun rowColAcc(
        lines: String,
        widthHeight: Pair<Int, Int>,
        data: Pair<List<List<Int>>, List<List<Int>>> = Pair(
            listOf(),
            listOf()
        )
    ): Either<ParseError.RowsColsError, Triple<String, Pair<Int, Int>, Pair<List<List<Int>>, List<List<Int>>>>> =
        if (lines == "" || (data.first.isNotEmpty() && data.second.isNotEmpty())) Either.Right(
            Triple(
                lines,
                widthHeight,
                data
            )
        )
        else {
            when (val output = parseLine2(lines, widthHeight, data)) {
                is Some -> rowColAcc(
                    output.value.first,
                    widthHeight,
                    Pair(output.value.second, output.value.third)
                )
                is None -> Either.Left(ParseError.RowsColsError)
            }
        }


    return either {
        val (lines2, widthHeight) = heightWidthAcc(lines).bind()
        val rowsCols = rowColAcc(lines2, widthHeight).bind()
        val widthHeight2 = rowsCols.second
        val rowNumsColNums = rowsCols.third
        GridData(
            widthHeight2.first,
            widthHeight2.second,
            rowNumsColNums.first,
            rowNumsColNums.second,
        )

    }
}

fun GridData.toNonFile() = "width ${width}\nheight ${height}\n\nrows\n" + rowNums
    .fold("") { acc, i ->
        if (i.isEmpty()) "${acc}0\n" else acc + i.fold("") { acc2, j -> "$acc2$j," }
            .dropLast(1) + "\n"
    } + "\n" + "columns\n" + colNums
    .fold("") { acc, i ->
        if (i.isEmpty()) "${acc}0\n" else acc + i.fold("") { acc2, j -> "$acc2$j," }
            .dropLast(1) + "\n"
    }


/** Returns (FileName, File contents) */
suspend fun checkValidNonFile(uri: Uri, context: Context): Either<FileError, Pair<String, String>> =
    if (checkValidFile(uri, context)) either {
        val filename = getFileNameFromUri(uri, context)
        val text = readTextFromUri(uri, context)

        parseNonFile(text).bind()
        Pair(filename, text)
    } else {
        ParseError.FileTooBigError.left()
    }

fun getFileNameFromUri(uri: Uri, context: Context): String =
    context.contentResolver.query(uri, null, null, null, null).use { cursor ->
        val nameIndex = cursor?.getColumnIndex(OpenableColumns.DISPLAY_NAME)
        cursor?.moveToFirst()
        if (nameIndex != null) cursor.getString(nameIndex) else ""
    }.substringBeforeLast('.')

fun checkValidFile(uri: Uri, context: Context) = (context.contentResolver.openAssetFileDescriptor(
    uri,
    "r"
)?.length ?: 10001) < 10000 /* 10kb max file size */

sealed class FileError {
/*    object FileReadError : FileError()
    object FileTooBig : FileError()
    data class FileParseError(val parseError: ParseError) : FileError()*/
}

sealed class ParseError : FileError() {
    object WidthHeightError : ParseError()
    object RowsColsError : ParseError()
    object FileTooBigError : ParseError() {
        override fun toString() = "File is too big"
    }

    override fun toString() = "Invalid File/QR Code"
}
