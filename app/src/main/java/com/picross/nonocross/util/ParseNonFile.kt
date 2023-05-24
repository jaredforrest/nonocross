package com.picross.nonocross.util

import android.content.Context
import android.net.Uri
import android.provider.OpenableColumns
import arrow.core.Either
import arrow.core.None
import arrow.core.Option
import arrow.core.Some
import arrow.core.left
import arrow.core.none
import arrow.core.right
import com.picross.nonocross.util.usergrid.GridAttributes
import com.picross.nonocross.util.usergrid.GridData
import kotlinx.collections.immutable.ImmutableList
import kotlinx.collections.immutable.toImmutableList

/** Takes a .non file (represented as a list of strings by each new line) and outputs the grid */
fun parseNonFile(text: String): Either<ParseError, GridData> {


    fun parseLine(line: String, data: GridAttributes): Either<ParseError.WidthHeightError, GridAttributes> {
        when (line.substringBefore(" ")) {
            "width" -> {
                line.substringAfter(" ").toIntOrNull()?.let {
                    data.width = it
                } ?: ParseError.WidthHeightError.WidthError.left()
            }

            "height" -> {
                line.substringAfter(" ").toIntOrNull()?.let {
                    data.height = it
                } ?: ParseError.WidthHeightError.HeightError.left()
            }

            "difficulty" -> {
                line.substringAfter(" ").toIntOrNull()?.let {
                    data.difficulty = it
                } ?: ParseError.WidthHeightError.HeightError.left()
            }
        }

        return data.right()
    }

    tailrec fun heightWidthAcc(
        lines: ImmutableList<String>,
        data: GridAttributes = GridAttributes.empty()
    ): Either<ParseError.WidthHeightError, Pair<ImmutableList<String>, GridAttributes>> =
        if (data.width > 0 && data.height > 0 && (data.difficulty ?: 0) > 0) {
            // All three values are set => Return data
            Pair(lines, data).right()
        } else if (lines.isEmpty() && data.width > 0 && data.height > 0) {
            // Reached end of file and width and height is set => Return data
            Pair(lines, data).right()
        } else if (lines.isEmpty()) {
            // Reached end of file but width and/or height not set => Error
            if (data.width == 0) {
                if (data.height == 0) {
                    ParseError.WidthHeightError.BothError.left()
                } else {
                    ParseError.WidthHeightError.HeightError.left()
                }
            } else {
                ParseError.WidthHeightError.WidthError.left()
            }
        } else {
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
        size: Int,
        acc: MutableList<List<Int>> = mutableListOf()
    ): Option<Pair<ImmutableList<String>, List<List<Int>>>> = if (size == 0) Some(Pair(lines, acc))
    else when (val next = parseNum(lines.first())) {
        is Some -> {
            acc.add(next.value)
            parseNums(
                lines.drop(1),
                size - 1,
                acc
            )
        }
        is None -> None
    }

    fun parseLine2(
        lines: ImmutableList<String>,
        attributes: GridAttributes,
        data: Pair<List<List<Int>>, List<List<Int>>>
    ): Either<ParseError.RowsColsError, Triple<ImmutableList<String>, List<List<Int>>, List<List<Int>>>> =
        when (lines.first()) {
            "rows" -> {
                when (val output = parseNums(lines.drop(1), attributes.height)) {
                    is Some -> Triple(output.value.first, output.value.second, data.second).right()
                    is None -> ParseError.RowsColsError.RowError.left()
                }
            }
            "columns" -> {
                when (val output = parseNums(lines.drop(1), attributes.width)) {
                    is Some -> Triple(output.value.first, data.first, output.value.second).right()
                    is None -> ParseError.RowsColsError.ColError.left()
                }

            }
            else -> Triple(lines.drop(1), data.first, data.second).right()
        }

    tailrec fun rowColAcc(
        lines: ImmutableList<String>,
        attributes: GridAttributes,
        data: Pair<List<List<Int>>, List<List<Int>>> = Pair(
            listOf(),
            listOf()
        )
    ): Either<ParseError.RowsColsError, Pair<List<List<Int>>, List<List<Int>>>> =
        if ((data.first.isNotEmpty() && data.second.isNotEmpty()) || lines.isEmpty()) Either.Right(
            data
        )
        else {
            when (val output = parseLine2(lines, attributes, data)) {
                is Either.Right -> rowColAcc(
                    output.value.first,
                    attributes,
                    Pair(output.value.second, output.value.third)
                )
                is Either.Left -> output
            }
        }


    val lines = text.split('\n').toImmutableList()

    val (lines2, attributes) = when (val a = heightWidthAcc(lines)){
        is Either.Left -> return a
        is Either.Right -> a.value
    }
    val rowsCols = when(val b = rowColAcc(lines2, attributes)) {
        is Either.Right -> b.value
        is Either.Left -> return b
    }

    return Either.Right(
        GridData(
            attributes,
            rowsCols.first,
            rowsCols.second,
        )
    )
}

fun GridData.toNonFile() = "width ${attributes.width}\nheight ${attributes.height}\ndifficulty ${attributes.difficulty}\n\nrows\n" + rowNums
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
