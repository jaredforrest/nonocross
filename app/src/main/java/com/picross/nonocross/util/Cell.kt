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
package com.picross.nonocross.util

enum class CellShade {
    CROSS,
    SHADE,
    EMPTY
}

fun CellShade.click(invert: Boolean): CellShade {
    return if (invert) {
        when (this) {
            CellShade.CROSS, CellShade.SHADE -> CellShade.EMPTY
            CellShade.EMPTY -> CellShade.SHADE
        }
    } else {
        when (this) {
            CellShade.CROSS -> CellShade.EMPTY
            CellShade.EMPTY, CellShade.SHADE -> CellShade.CROSS
        }
    }
}
