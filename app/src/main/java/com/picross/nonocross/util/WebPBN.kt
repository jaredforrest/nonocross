package com.picross.nonocross.util

import android.util.Log
import arrow.core.*
import com.picross.nonocross.util.usergrid.GridData
import io.ktor.client.*
import io.ktor.client.call.*
import io.ktor.client.features.*
import io.ktor.client.request.forms.*
import io.ktor.client.statement.*
import io.ktor.http.*
import io.ktor.utils.io.charsets.*
import io.ktor.utils.io.core.*
import kotlinx.collections.immutable.PersistentList
import kotlinx.collections.immutable.toPersistentList
import kotlinx.coroutines.coroutineScope
import kotlinx.coroutines.launch
import org.jsoup.Jsoup
import java.nio.charset.Charset


suspend fun getWebPBNById(id: Int, client: HttpClient): Option<GridData> {

    Log.d("asdffdsafdsa", "fefefefasefwes")
    val w1252: Charset = charset("Windows-1252")
    val response: HttpResponse = try {
        client.submitForm(
            url = "https://webpbn.com/export.cgi",
            formParameters = Parameters.build {
                append("go", "1")
                append("id", "$id")
                append("fmt", "ss")
//                append("Connection", "close"
                append("Accept-Encoding", "identity")
            })
    } catch (e: Exception) {
        return none()
    }
    // Need to force ktor to use w1252 as charset
    val decoder = w1252.newDecoder()
    val input = response.receive<Input>()

    val bytes = decoder.decode(input)
//        val bytes = response.readText(w1252)
    //client.close()
    return when (val ret = parseNonFile(bytes)) {
        is Either.Left -> none()
        is Either.Right -> Some(ret.value)
    }
}

suspend fun getWebPBNsById(ids: List<Pair<String, Int>>): PersistentList<Pair<String, GridData>> {

    val client = HttpClient {
        install(HttpTimeout) {
            requestTimeoutMillis = 3000
        }
/*        install(Logging) {
            logger = Logger.DEFAULT
            level = LogLevel.HEADERS
        }*/
    }

    val emptyGridData = GridData(1, 1, listOf(), listOf())
    val myList = MutableList(ids.size) { i -> Pair(ids[i].first, emptyGridData) }
    //myList.forEach {
    //}

    coroutineScope {
        ids.forEachIndexed { index, it ->
            launch {
                myList[index] = Pair(
                    it.first,
                    when (val grid = getWebPBNById(ids[index].second, client)) {
                        is Some -> grid.value
                        is None -> GridData(1, 1, listOf(), listOf())
                    }
                )
            }
        }//.toPersistentList()
    }
    client.close()
    return myList.toPersistentList()
}


suspend fun getRandomWebPBNs(onlinePrefs: OnlinePrefs): Either<String, PersistentList<Pair<String, Int>>> {
    val w1252: Charset = charset("Windows-1252")

    val minSize = onlinePrefs.minSize * onlinePrefs.minSize * 25
    val maxSize = onlinePrefs.maxSize * onlinePrefs.maxSize * 25
    val minQuality = onlinePrefs.minQuality
    val maxQuality = onlinePrefs.maxQuality
    val allowUnratedQual = onlinePrefs.allowUnratedQual
    val minDifficulty = onlinePrefs.minDifficulty
    val maxDifficulty = onlinePrefs.maxDifficulty
    val allowUnratedDiff = onlinePrefs.allowUnratedDiff
    val sid = if (onlinePrefs.apiKey != "") onlinePrefs.apiKey else return "Invalid Api Key".left()

    val pageNo = 0
    val perPage = 10
    val baseUri = "https://webpbn.com/find.cgi"
//    val sid = "N2ZY_PC6eTN_ZDX5XYKAB-.1"
    //val uur = "https://webpbn.com/find.cgi?search=1&status=0&minid=&maxid=&title=&author=&minsize=0&maxsize=25&minqual=4&maxqual=20&unqual=0&mindiff=4&maxdiff=20&undiff=0&mincolor=2&maxcolor=2&uniq=0&guess=0&blots=2&showcreate=1&order=0&perpage=20&pageno=0&sid=3Y%3AOD83NH5.JVHI-E33045K3"
    val url =
        "https://webpbn.com/find.cgi?search=1&status=0&minid=&maxid=&title=&author=&minsize=$minSize&maxsize=$maxSize&minqual=$minQuality&maxqual=$maxQuality&unqual=${if (allowUnratedQual) "0" else "1"}&mindiff=$minDifficulty&maxdiff=$maxDifficulty&undiff=${if (allowUnratedDiff) "0" else "1"}&mincolor=2&maxcolor=2&uniq=0&guess=0&blots=2&showcreate=1&order=0&perpage=$perPage&pageno=$pageNo&sid=$sid"

    val client = HttpClient {
        install(HttpTimeout) {
            requestTimeoutMillis = 3000
        }
/*        install(Logging) {
            logger = Logger.DEFAULT
            level = LogLevel.HEADERS
        }*/
    }

    val response: HttpResponse = try {
        client.submitForm(
            url = baseUri,
            formParameters = Parameters.build {
                append("search", "1")
                append("status", "0")
                append("minid", "")
                append("maxid", "")
                append("title", "")
                append("author", "")
                append("minsize", "$minSize")
                append("maxsize", "$maxSize")
                append("minqual", "$minQuality")
                append("maxqual", "$maxQuality")
                append("unqual", if (allowUnratedQual) "0" else "1")
                append("mindiff", "$minDifficulty")
                append("maxdiff", "$maxDifficulty")
                append("undiff", if (allowUnratedDiff) "0" else "1")
                append("mincolor", "2")
                append("maxcolor", "2")
                append("uniq", "1")
                append("guess", "0")
                append("blots", "2")
                append("showcreate", "1")
                append("order", "0")
                append("perpage", "$perPage")
                append("pageno", "$pageNo")
                append("sid", sid)
            })
    } catch (e: Exception) {
        return e.toString().left()
    }

    Log.d("rrrll", url)

    // Need to force ktor to use w1252 as charset
    val decoder = w1252.newDecoder()
    val input = response.receive<Input>()

    val data = decoder.decode(input)
    //val data = response.readText(w1252)

//    Log.d("hrdvrf", data)
    val doc = Jsoup.parse(data, baseUri)//.connect(url).get()

    client.close()

    val table = try {
        doc?.select("table")?.get(3)?.select("tr")
    } catch (IndexOutOfBoundsException: Exception) {
        data.contains("expired", true)
        return "API key expired".left()
    }

    //if(table?.get(0)?.select("td")?.get(0)?.text()=="PREVIOUS PAGE OF RESULTS"){}
    //Log.d("ajiomnroua", table[0].select("td")?.get(0)?.text() ?: "we have failed")
    //Log.d("rrrrrrllll", url)

    val ids = table?.drop(1)
        ?.dropLast(1)
        ?.map { row -> row.select("td")[1] }
        ?.map {
            Pair(
                it.text(),
                it.select("a")[0].attr("href").substringAfter("id=").substringBefore('&').toInt()
            )
        }
        ?.toPersistentList()
        ?.right()
        ?: "Failed to Parse".left()
    //.toOption()
    return ids//Either.Right(ids)
}
