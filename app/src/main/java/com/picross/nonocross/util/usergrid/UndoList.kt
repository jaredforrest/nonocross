package com.picross.nonocross.util.usergrid

import arrow.core.*
import com.picross.nonocross.util.CellShade
import kotlinx.collections.immutable.PersistentList

/* this is not really like a linked list */
data class UndoList(
    val prev: MutableList<PersistentList<CellShade>>,
    var data: PersistentList<CellShade>,
    val next: MutableList<PersistentList<CellShade>>
) {

    operator fun get(i: Int) = data[i]

    fun undo() : Either<UndoListException.NoMoreUndo, Unit> {
        return try {
            val last = prev.removeLast()
            next.add(data)
            data = last
            Either.Right(Unit)
        } catch (err: NoSuchElementException) {
            Either.Left(UndoListException.NoMoreUndo)
        }
    }

    fun redo() : Either<UndoListException.NoMoreRedo, Unit> {
        return try {
            val last = next.removeLast()
            prev.add(data)
            data = last
            Either.Right(Unit)
        } catch (err: NoSuchElementException) {
            Either.Left(UndoListException.NoMoreRedo)
        }
    }

    fun addToList(new_data: PersistentList<CellShade>) {
        prev.add(data)
        data = new_data
        next.clear()
    }

    fun reset(new_data: PersistentList<CellShade>) {
        data = new_data
        prev.clear()
        next.clear()
    }
}

sealed class UndoListException {
    object NoMoreRedo : UndoListException()
    object NoMoreUndo : UndoListException()
}
