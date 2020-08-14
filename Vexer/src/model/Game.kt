package model

data class Game(
    val title: String,
    val author: String = "anonymous",
    val releaseDate: String = "unknown",
    val code: String,
    val aspectRatio: ArrayList<Int>,
    val controls: ArrayList<Control>,
    val sprites: ArrayList<Sprite>,
    val instruments: ArrayList<Instrument>,
    val songs: ArrayList<Song>
)
