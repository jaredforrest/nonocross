/**This file is part of Nonocross.

Nonocross is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Nonocross is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Nonocross.  If not, see <https://www.gnu.org/licenses/>.*/
package com.picross.nonocross

import arrow.core.Option
import arrow.core.none
import com.picross.nonocross.util.usergrid.GridAttributes
import com.picross.nonocross.util.usergrid.GridData
import com.picross.nonocross.util.usergrid.UserGrid
import kotlinx.collections.immutable.PersistentList

object LevelDetails {
    var gridData =
        GridData(GridAttributes(1, 1, null), listOf(listOf(2), listOf(2)), listOf(listOf(2), listOf(2)))//: GridData
    lateinit var userGrid: UserGrid
    //var levelName

    var levelType: LevelType = LevelType.Random()

    var toggleCross = false

    /*fun isReady(): Boolean {
        return ::userGrid.isInitialized
    }*/

    /* Not sure if this is the best place to define this
    It is used for generating a unique random level
     */
    //val myThreadGroup = ThreadGroup("Native Thread Group")

    lateinit var levels: PersistentList<Pair<String, GridData>>
    lateinit var customLevels: PersistentList<Pair<String, GridData>>

}

sealed class LevelType {

    data class Random(
        val levelName: Option<String> = none()
    ) : LevelType()

    data class Custom(
        val levelName: String
    ) : LevelType()


    data class Default(
        val levelName: String
    ) : LevelType()

    data class Online(
        val levelName: Option<String> = none()
    ) : LevelType()
}