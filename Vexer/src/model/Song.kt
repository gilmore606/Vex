package model

data class Song(
    val id: String,
    val file: String,
    val voices: ArrayList<String>,
    val volumes: ArrayList<Int>,
    val pans: ArrayList<Int>
)
