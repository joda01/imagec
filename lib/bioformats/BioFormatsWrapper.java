// BioFormatsWrapper.java
import loci.formats.ImageReader;

public class BioFormatsWrapper {
    public static byte[] readImage(String filePath) {
        try {
            ImageReader reader = new ImageReader();
            reader.setId(filePath);
            reader.openBytes(0);
            byte[] pixelData = reader.openBytes(0);
            reader.close();

            return pixelData;
        } catch (Exception e) {
            e.printStackTrace();
        }
        byte[] empty = new byte[0];
        return empty;
        
    }
}
