package com.picross.nonocross.util

import kotlinx.collections.immutable.ImmutableList

fun <T> ImmutableList<T>.drop(n: Int): ImmutableList<T> {
    if(n == 0) return this
    n.coerceAtMost(size)
    return subList(n,size)
}