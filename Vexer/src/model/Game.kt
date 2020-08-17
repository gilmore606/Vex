package model

data class Game(
    val title: String,
    val author: String = "anonymous",
    val releaseDate: String = "unknown",
    val aspectRatio: ArrayList<Int>,
    val code: String,
    val controls: ArrayList<Control>,
    val images: ArrayList<Image>,
    val fonts: ArrayList<Font>,
    val instruments: ArrayList<Instrument>,
    val songs: ArrayList<Song>
)
