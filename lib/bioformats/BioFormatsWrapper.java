
// BioFormatsWrapper.java

import loci.formats.ImageReader;
import loci.common.DebugTools;
import loci.formats.IFormatReader;

public class BioFormatsWrapper {
    public static byte[] readImage(String imagePath, String directory, String series) {
        DebugTools.setRootLevel("OFF");

        try {
            // Create an appropriate reader for the format
            IFormatReader formatReader = new ImageReader();

            // Initialize the reader with the image file
            formatReader.setId(imagePath);
            formatReader.setSeries(Integer.parseInt(series));

            // Read the image data for the current channel, timepoint, and slice
            byte[] imageBytes = formatReader.openBytes(Integer.parseInt(directory));

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
    public static String getImageProperties(String imagePath, String directory, String series) {
        DebugTools.setRootLevel("OFF");

        String ret = "{}";

        try {
            // Create an appropriate reader for the format
            IFormatReader formatReader = new ImageReader();

            // Initialize the reader with the image file
            formatReader.setId(imagePath);
            formatReader.setSeries(Integer.parseInt(series));

            // OMEXMLMetadata omeMetadata = (OMEXMLMetadata)
            // formatReader.getMetadataStore();
            String channelOrder = "";
            for (int c = 0; c < formatReader.getSizeC(); c++) {
                String tOrder = "[";

                for (int t = 0; t < formatReader.getSizeT(); t++) {
                    String zOrder = "[";
                    for (int z = 0; z < formatReader.getSizeZ(); z++) {
                        zOrder = zOrder.concat(String.valueOf(formatReader.getIndex(z, c, t)));
                        if (z + 1 < formatReader.getSizeZ()) {
                            zOrder = zOrder.concat(",");
                        }
                    }
                    zOrder = zOrder.concat("]");
                    tOrder = tOrder.concat(zOrder);

                    if (t + 1 < formatReader.getSizeT()) {
                        zOrder = zOrder.concat(",");
                    }
                }
                tOrder = tOrder.concat("]");
                channelOrder = channelOrder.concat(tOrder);
                if (c + 1 < formatReader.getSizeC()) {
                    channelOrder = channelOrder.concat(",");
                }
            }

            ret = ("{");
            ret = ret.concat("\"width\":" + String.valueOf(formatReader.getSizeX()) + ",");
            ret = ret.concat("\"height\":" + String.valueOf(formatReader.getSizeY()) + ",");
            ret = ret.concat("\"bits\":" + String.valueOf(formatReader.getBitsPerPixel()) + ",");
            ret = ret.concat("\"ch\":" + String.valueOf(formatReader.getSizeC()) + ",");
            ret = ret.concat("\"planes\":" + String.valueOf(formatReader.getImageCount()) + ",");
            ret = ret.concat("\"tile_height\":" + String.valueOf(formatReader.getOptimalTileHeight()) + ",");
            ret = ret.concat("\"tile_width\":" + String.valueOf(formatReader.getOptimalTileWidth()) + ",");
            ret = ret.concat("\"series_count\": \"" + formatReader.getSeriesCount() + "\",");
            ret = ret.concat("\"dim_order\": \"" + formatReader.getDimensionOrder() + "\",");
            ret = ret.concat("\"orders\": [");
            ret = ret.concat(channelOrder);
            ret = ret.concat("]");
            ret = ret.concat("}");
            formatReader.close();

        } catch (Exception e) {
            e.printStackTrace();
        }
        return ret;
    }
}
