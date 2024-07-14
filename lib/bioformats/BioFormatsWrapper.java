
// BioFormatsWrapper.java

import loci.common.services.ServiceFactory;
import loci.formats.ImageReader;
import loci.formats.in.OMEXMLReader;
import loci.formats.meta.IMetadata;
import loci.formats.services.OMEXMLService;
import ome.xml.meta.OMEXMLMetadata;
import ome.xml.model.primitives.PositiveInteger;
import loci.common.DebugTools;
import loci.formats.IFormatReader;

public class BioFormatsWrapper {
    public class ImageResult {
    } 


    public static byte[] readImage(String imagePath, int directory, int series) {
        DebugTools.setRootLevel("OFF");

        try {
            // Create an appropriate reader for the format
            IFormatReader formatReader = new ImageReader();

            // Initialize the reader with the image file
            formatReader.setId(imagePath);
            formatReader.setSeries(series);

            // Read the image data for the current channel, timepoint, and slice
            byte[] imageBytes = formatReader.openBytes(directory);

            //int bits = formatReader.getBitsPerPixel();
            //int height = formatReader.getSizeY();
            //int width = formatReader.getSizeX();

            // Process or display the image data as needed
            // For example, you can convert it to a BufferedImage or perform other image
            // Close the reader when done
            formatReader.close();
            return imageBytes;
        } catch (Exception e) {
            e.printStackTrace();
        }
        byte[] imageBytes = new byte[1];
        return imageBytes;
    }

    ///
    /// \brief Returns a JSON object with the image properties.
    /// The orders is a 3D array containg the index of the channels of the Z-Stack
    /// of the time stack
    /// [ # Channel 0
    /// [ # Time Frame 0
    /// [0,1,2] # Z-Index
    /// # Time Frame 1
    /// [0,1,2] # Z-Index
    /// ], # Channel 1
    /// [ # Time Frame 0
    /// [0,1,2] # Z-Index
    /// # Time Frame 1
    /// [0,1,2] # Z-Index
    /// ],
    /// ]
    ///
    /// Real world example:
    /// {"width":2048,"height":2048,"bits":16,"ch":5,"series_count":
    /// "2","dim_order": "XYCZT","orders":
    /// {"width":2048,"height":2048,"bits":16,"ch":5,"planes":15,"tile_height":2048,"tile_width":2048,"series_count":
    /// "2","dim_order": "XYCZT","orders":
    /// [[[0,5,10]],[[1,6,11]],[[2,7,12]],[[3,8,13]],[[4,9,14]]]}
    ///
    /// \author Joachim Danmayr
    /// \param[in] imagePath Path of the image to load
    /// \param[in] directory Index of the image plane to load (must be a positive
    /// integer ot zero)
    /// \return Image properties JSON
    ///
    public static String getImageProperties(String imagePath, int directory, int series) {
        DebugTools.setRootLevel("OFF");

        String omeXML = "";

        try {
            // Create a service factory
            ServiceFactory factory = new ServiceFactory();
            OMEXMLService service = factory.getInstance(OMEXMLService.class);
            // Create metadata object
            IMetadata metadata = service.createOMEXMLMetadata();


            // Create an appropriate reader for the format
            ImageReader formatReader = new ImageReader();
   



            // Initialize the reader with the image file
            formatReader.setMetadataStore(metadata);
            formatReader.setId(imagePath);
            formatReader.setSeries(series);
            omeXML = service.getOMEXML(metadata);
            omeXML = omeXML + "\n<JODA xmlns=\"https://www.imagec.org/\" TileWidht=\""+String.valueOf(formatReader.getOptimalTileHeight())+"\" TileHeight=\""+String.valueOf(formatReader.getOptimalTileWidth())+"\"></JODA>";
            formatReader.close();

        } catch (Exception e) {
            e.printStackTrace();
        }
        return omeXML;
    }
}
