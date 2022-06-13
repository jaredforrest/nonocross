package com.picross.nonocross.util

import android.content.Context
import android.net.Uri
import android.provider.OpenableColumns
import arrow.core.*
import com.picross.nonocross.util.usergrid.GridData
import kotlinx.collections.immutable.ImmutableList
import kotlinx.collections.immutable.toImmutableList

/** Takes a .non file (represented as a list of strings by each new line) and outputs the grid */
fun parseNonFile(text: String): Either<ParseError, GridData> {


    fun parseLine(line: String, data: Pair<UInt, UInt>): Either<ParseError.WidthHeightError, Pair<UInt, UInt>> =
        when (line.substringBefore(" ")) {
            "width" -> {
                line.substringAfter(" ").toUIntOrNull()?.let{
                    Pair(it, data.second).right()
                } ?: ParseError.WidthHeightError.WidthError.left()
            }
            "height" -> {
                line.substringAfter(" ").toUIntOrNull()?.let{
                    Pair(data.first, it).right()
                } ?: ParseError.WidthHeightError.HeightError.left()
            }

            else -> data.right()
        }

    tailrec fun heightWidthAcc(
        lines: ImmutableList<String>,
        data: Pair<UInt, UInt> = Pair(0u, 0u)
    ): Either<ParseError.WidthHeightError, Pair<ImmutableList<String>, Pair<UInt, UInt>>> =
        if (data.first != 0u && data.second != 0u) Pair(lines, data).right()
        else if (lines.isEmpty()) {
            if (data.first == 0u) {
                if (data.second == 0u) {
                    ParseError.WidthHeightError.BothError.left()
                } else {
                    ParseError.WidthHeightError.HeightError.left()
                }
            } else {
                ParseError.WidthHeightError.WidthError.left()
            }
        }
        else {
            when (val parse = parseLine(lines.first(), data)) {
                is Either.Left -> parse
                is Either.Right -> heightWidthAcc(
                        lines.drop(1),
                        parse.value
                    )
            }
        }

    fun parseNum(rowNum: String): Option<List<Int>> =
        if (rowNum == "0") Some(listOf())
        else {
            Some(rowNum.split(',').map { it.toIntOrNull() ?: return none() })
        }

    tailrec fun parseNums(
        lines: ImmutableList<String>,
        size: UInt,
        acc: MutableList<List<Int>> = mutableListOf()
    ): Option<Pair<ImmutableList<String>, List<List<Int>>>> = if (size == 0u) Some(Pair(lines, acc))
    else when (val next = parseNum(lines.first())) {
        is Some -> {
            acc.add(next.value)
            parseNums(
                lines.drop(1),
                size - 1u,
                acc
            )
        }
        is None -> None
    }

    fun parseLine2(
        lines: ImmutableList<String>,
        widthHeight: Pair<UInt, UInt>,
        data: Pair<List<List<Int>>, List<List<Int>>>
    ): Either<ParseError.RowsColsError, Triple<ImmutableList<String>, List<List<Int>>, List<List<Int>>>> =
        when (lines.first()) {
            "rows" -> {
                when (val output = parseNums(lines.drop(1), widthHeight.second)) {
                    is Some -> Triple(output.value.first, output.value.second, data.second).right()
                    is None -> ParseError.RowsColsError.RowError.left()
                }
            }
            "columns" -> {
                when (val output = parseNums(lines.drop(1), widthHeight.first)) {
                    is Some -> Triple(output.value.first, data.first, output.value.second).right()
                    is None -> ParseError.RowsColsError.ColError.left()
                }

            }
            else -> Triple(lines.drop(1), data.first, data.second).right()
        }

    tailrec fun rowColAcc(
        lines: ImmutableList<String>,
        widthHeight: Pair<UInt, UInt>,
        data: Pair<List<List<Int>>, List<List<Int>>> = Pair(
            listOf(),
            listOf()
        )
    ): Either<ParseError.RowsColsError, Pair<List<List<Int>>, List<List<Int>>>> =
        if ((data.first.isNotEmpty() && data.second.isNotEmpty()) || lines.isEmpty()) Either.Right(
            data
        )
        else {
            when (val output = parseLine2(lines, widthHeight, data)) {
                is Either.Right -> rowColAcc(
                    output.value.first,
                    widthHeight,
                    Pair(output.value.second, output.value.third)
                )
                is Either.Left -> output
            }
        }


    val lines = text.split('\n').toImmutableList()

    val (lines2, widthHeight) = when (val a = heightWidthAcc(lines)){
        is Either.Left -> return a
        is Either.Right -> a.value
    }
    val rowsCols = when(val b = rowColAcc(lines2, widthHeight)) {
        is Either.Right -> b.value
        is Either.Left -> return b
    }

    return Either.Right(
        GridData(
            widthHeight.first.toInt(),
            widthHeight.second.toInt(),
            rowsCols.first,
            rowsCols.second,
        )
    )
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
fun getNonFile(uri: Uri, context: Context): Either<ParseError, Pair<String, String>> =
    if (checkValidFile(uri, context)) {
        val filename = getFileNameFromUri(uri, context)
        val text = readTextFromUri(uri, context)

        Pair(filename, text).right()
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

sealed class ParseError() {
    sealed class WidthHeightError(): ParseError() {
        object WidthError : WidthHeightError(){
            override fun toString() = super.toString() + " Invalid Width"
        }
        object HeightError : WidthHeightError(){
            override fun toString() = super.toString() + " Invalid Height"
        }
        object BothError : WidthHeightError(){
            override fun toString() = super.toString() + " Invalid Width and Height"
        }
    }

    sealed class RowsColsError : ParseError(){
        object RowError : RowsColsError() {
            override fun toString() = super.toString() + " Invalid Row Numbers"
        }
        object ColError : RowsColsError() {
            override fun toString() = super.toString() + " Invalid Col Numbers"
        }
    }

    object FileTooBigError : ParseError() {
        override fun toString() = "File is too big"
    }

    override fun toString() = "Can't Open File"
}
