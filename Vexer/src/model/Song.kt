package model

data class Song(
    val id: Int,
    val name: String,
    val file: String = "",
    val notes: ArrayList<SongNote> = ArrayList(),
    val voices: ArrayList<String>,
    val volumes: ArrayList<Int> = ArrayList(),
    val pans: ArrayList<Int> = ArrayList()
)
