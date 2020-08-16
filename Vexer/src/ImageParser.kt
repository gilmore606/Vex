import model.Image
import java.nio.ByteBuffer

class ImageParser(val config: Image, val fVerbose: Boolean = false) {


    fun parse() {

    }

    fun writeToFile(outFile: OutFile) {
        outFile.writeMarker("IMG")
        outFile.writeMarker(config.id.toString())
        // 2 bytes for float count
        outFile.write2ByteInt(config.data.size)
        outFile.writeFloats(config.data)
        println("wrote IMG " + config.id + " (" + config.data.size + " floats)")
    }
}
