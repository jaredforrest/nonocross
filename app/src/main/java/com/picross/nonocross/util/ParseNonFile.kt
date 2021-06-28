package com.picross.nonocross.util

import android.content.Context
import android.content.res.AssetFileDescriptor
import android.net.Uri
import android.provider.OpenableColumns
import android.util.Log
import arrow.core.Either
import arrow.core.None
import arrow.core.Option
import arrow.core.Some
import kotlinx.collections.immutable.*


/** Takes a .non file (represented as a list of strings by each new line) and outputs the grid */
fun parseNonFile(lines: String): Option<GridData2> {

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

    fun parseNum(rowNum: String): Option<ImmutableList<Int>> {
        return if (rowNum == "0") Some(persistentListOf())
        else {
            val plist = persistentListOf<Int>()
            val builder = plist.builder()
            builder.addAll(rowNum.split(',').map { it.toIntOrNull() ?: return None })
            builder.build()
            Some(rowNum.split(',').map { it.toInt() }.toPersistentList())
        }
    }

    tailrec fun parseNums(
        lines: String,
        size: Int,
        acc: ImmutableList<ImmutableList<Int>> = persistentListOf()
    ): Option<Pair<String, ImmutableList<ImmutableList<Int>>>> {
        return if (size == 0) Some(Pair(lines, acc))
        else when (val next = parseNum(lines.substringBefore('\n'))) {
            is Some -> parseNums(
                lines.substringAfter('\n'),
                size - 1,
                (acc as PersistentList) + persistentListOf(next.value)
            )
            is None -> None
        }
    }

    fun parseLine2(
        lines: String,
        widthHeight: Pair<Int, Int>,
        data: Pair<ImmutableList<ImmutableList<Int>>, ImmutableList<ImmutableList<Int>>>
    ): Option<Triple<String, ImmutableList<ImmutableList<Int>>, ImmutableList<ImmutableList<Int>>>> {
        return when (lines.substringBefore('\n')) {
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
    }

    tailrec fun rowColAcc(
        lines: String,
        widthHeight: Pair<Int, Int>,
        data: Pair<ImmutableList<ImmutableList<Int>>, ImmutableList<ImmutableList<Int>>> = Pair(
            persistentListOf(),
            persistentListOf()
        )
    ): Option<Triple<String, Pair<Int, Int>, Pair<ImmutableList<ImmutableList<Int>>, ImmutableList<ImmutableList<Int>>>>> {
        return if (lines == "" || (data.first.isNotEmpty() && data.second.isNotEmpty())) Some(
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
                is None -> None
            }
        }
    }

    val (lines2, widthHeight) = heightWidthAcc(lines)
    if (widthHeight.first == 0 || widthHeight.second == 0) return None
    val widthHeight2: Pair<Int, Int>
    val rowNumsColNums: Pair<ImmutableList<ImmutableList<Int>>, ImmutableList<ImmutableList<Int>>>
    when (val rowsCols = rowColAcc(lines2, widthHeight)) {
        is Some -> {
            widthHeight2 = rowsCols.value.second
            rowNumsColNums = rowsCols.value.third
        }
        is None -> return None
    }

    Log.d(
        "fdsasa", GridData2(
            widthHeight2.first,
            widthHeight2.second,
            rowNumsColNums.first,
            rowNumsColNums.second
        ).toString()
    )
    return Some(
        GridData2(
            widthHeight2.first,
            widthHeight2.second,
            rowNumsColNums.first,
            rowNumsColNums.second
        )
    )


}

/** Returns (FileName, File contents) */
fun checkValidNonFile(uri: Uri?, context: Context): Either<Error, Pair<String, String>> {
    if (uri == null) return Either.Left(Error.UriNull)
    val afd: AssetFileDescriptor = context.contentResolver.openAssetFileDescriptor(uri, "r")
        ?: return Either.Left(Error.FileReadError)
    if (afd.length > 10000 /* 10kb max file size */) {
        afd.close()
        return Either.Left(Error.FileTooBig)
    }
    afd.close()

    val filename = context.contentResolver.query(uri, null, null, null, null).use { cursor ->
        val nameIndex = cursor?.getColumnIndex(OpenableColumns.DISPLAY_NAME)
        cursor?.moveToFirst()
        if (nameIndex != null) cursor.getString(nameIndex) else ""
    }.substringBeforeLast('.')

    val text = readTextFromUri(uri, context)
    return when (filename) {
        "" -> Either.Left(Error.NoFileName)
        else ->
            when (parseNonFile(text)) {
                is Some -> Either.Right(Pair(filename, text))
                is None -> Either.Left(Error.FileParseError)
            }
    }
}

sealed class Error {
    object UriNull : Error()
    object FileReadError : Error()
    object FileTooBig : Error()
    object FileParseError : Error()
    object NoFileName : Error()
}