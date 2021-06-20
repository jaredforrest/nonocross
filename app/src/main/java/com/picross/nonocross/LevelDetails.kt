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

import com.picross.nonocross.util.GridData2
import com.picross.nonocross.util.UserGrid

object LevelDetails {
    var gridData = GridData2(0, 0, listOf(), listOf())//0, List(0) { CellShade.EMPTY })
    lateinit var userGrid: UserGrid


    var levelName = "Level 1"
    var isRandom = false

    var toggleCross = false

    fun isReady(): Boolean {
        return ::userGrid.isInitialized
    }
}