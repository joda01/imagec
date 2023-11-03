// BioFormatsWrapper.java
import loci.formats.ImageReader;

public class BioFormatsWrapper {
    public static byte[] readImage(String filePath) {
       byte[] test = new byte[5];
       test[0]  =1;
       test[1]  =2;
       test[2]  =3;
       test[3]  =4;
        return test;


        //try {
        //    ImageReader reader = new ImageReader();
        //    reader.setId(filePath);
        //    reader.openBytes(0);
        //    byte[] pixelData = reader.openBytes(0);
        //    reader.close();
//
        //    return pixelData;
        //} catch (Exception e) {
        //    e.printStackTrace();
        //}
        //byte[] empty = new byte[2];
        //return empty;
        
    }
}
