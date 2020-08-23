package model

data class SongNote(
        val tick: Int,
        val chan: Int,
        val event: String,
        val d1: Int,
        val d2: Int
)
