
// BioFormatsWrapper.java

import loci.common.services.ServiceFactory;
import loci.formats.ImageReader;
import loci.formats.in.MetadataOptions;
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

    public static int[] readImageInfo(String imagePath, int series, int resolution) {
        try {
            // Create an appropriate reader for the format
            IFormatReader formatReader = new ImageReader();
            formatReader.setFlattenedResolutions(false); // Activate pyramids

            // Initialize the reader with the image file
            formatReader.setId(imagePath);
            formatReader.setSeries(series);
            if (resolution >= formatReader.getResolutionCount()) {
                resolution = formatReader.getResolutionCount() - 1;
            }
            formatReader.setResolution(resolution);

            int[] imageBytes = new int[9];
            imageBytes[0] = formatReader.getSizeY(); // Height
            imageBytes[1] = formatReader.getSizeX(); // Width
            imageBytes[2] = formatReader.getOptimalTileHeight();
            imageBytes[3] = formatReader.getOptimalTileWidth();
            imageBytes[4] = formatReader.getBitsPerPixel();
            imageBytes[5] = formatReader.getRGBChannelCount();
            imageBytes[6] = formatReader.getResolutionCount();
            imageBytes[7] = formatReader.isInterleaved() == true ? 1 : 0; // if interleaved [R1, G1, B1, R2, G2, B2,
                                                                          // ..., Rn, Gn, Bn]
            imageBytes[8] = formatReader.isLittleEndian() == true ? 1 : 0;
            return imageBytes;
        } catch (Exception e) {
            e.printStackTrace();
        }
        int[] imageBytes = new int[1];
        return imageBytes;
    }

    public static byte[] readImage(String imagePath, int series, int resolution, int z, int c, int t) {
        DebugTools.setRootLevel("OFF");

        try {
            // Create an appropriate reader for the format
            IFormatReader formatReader = new ImageReader();
            formatReader.setFlattenedResolutions(false); // Activate pyramids

            // Initialize the reader with the image file
            formatReader.setId(imagePath);
            formatReader.setSeries(series);
            if (resolution >= formatReader.getResolutionCount()) {
                resolution = formatReader.getResolutionCount() - 1;
            }
            formatReader.setResolution(resolution);

            // Read the image data for the current channel, timepoint, and slice
            byte[] imageBytes = formatReader.openBytes(formatReader.getIndex(z, c, t));

            formatReader.close();
            return imageBytes;
        } catch (Exception e) {
            e.printStackTrace();
        }
        byte[] imageBytes = new byte[1];
        return imageBytes;
    }

    public static byte[] readImageTile(String imagePath, int series, int resolution, int z, int c, int t, int x, int y,
            int width, int height) {
        DebugTools.setRootLevel("OFF");

        try {
            // Create an appropriate reader for the format
            IFormatReader formatReader = new ImageReader();
            formatReader.setFlattenedResolutions(false); // Activate pyramids

            // Initialize the reader with the image file
            formatReader.setId(imagePath);
            formatReader.setSeries(series);
            if (resolution >= formatReader.getResolutionCount()) {
                resolution = formatReader.getResolutionCount() - 1;
            }
            formatReader.setResolution(resolution);

            // Read the image data for the current channel, timepoint, and slice
            byte[] imageBytes = formatReader.openBytes(formatReader.getIndex(z, c, t), x, y, width, height);
            formatReader.close();
            return imageBytes;
        } catch (Exception e) {
            e.printStackTrace();
        }
        byte[] imageBytes = new byte[1];
        return imageBytes;
    }

    /// https://docs.openmicroscopy.org/ome-model/6.2.2/ome-tiff/specification.html
    public static String getImageProperties(String imagePath, int tmp/* series */) {
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
            formatReader.setFlattenedResolutions(false); // Activate pyramids

            // Initialize the reader with the image file
            formatReader.setMetadataStore(metadata);
            formatReader.setId(imagePath);
            omeXML = service.getOMEXML(metadata);
            omeXML = omeXML + "\n<JODA xmlns=\"https://www.imagec.org/\" SeriesCount=\""
                    + String.valueOf(formatReader.getSeriesCount()) + "\">";

            for (int series = 0; series < formatReader.getSeriesCount(); series++) {
                formatReader.setSeries(series);
                omeXML = omeXML + "\n<Series idx=\"" + String.valueOf(series) + "\" ResolutionCount=\""
                        + String.valueOf(formatReader.getResolutionCount()) + "\">";

                String format = formatReader.getFormat().toLowerCase();
                int optimalTileWidth = formatReader.getOptimalTileWidth();
                int optimalTileHeight = formatReader.getOptimalTileHeight();
                if (format.contains("jpeg")) {
                    optimalTileWidth = formatReader.getSizeX();
                    optimalTileHeight = formatReader.getSizeY();
                }

                for (int n = 0; n < formatReader.getResolutionCount(); n++) {
                    formatReader.setResolution(n);
                    omeXML += "<PyramidResolution idx=\"" + String.valueOf(n) + "\" width=\""
                            + String.valueOf(formatReader.getSizeX()) + "\" height=\""
                            + String.valueOf(formatReader.getSizeY()) + "\" TileWidth=\""
                            + String.valueOf(optimalTileWidth) + "\" TileHeight=\""
                            + String.valueOf(optimalTileHeight) + "\" BitsPerPixel=\""
                            + String.valueOf(formatReader.getBitsPerPixel()) + "\" RGBChannelCount=\""
                            + String.valueOf(formatReader.getRGBChannelCount()) + "\" IsInterleaved=\""
                            + String.valueOf(formatReader.isInterleaved() == true ? 1 : 0) + "\" IsLittleEndian=\""
                            + String.valueOf(formatReader.isLittleEndian() == true ? 1 : 0) + "\"/>";

                }
                omeXML = omeXML + "</Series>\n";
            }
            omeXML += "</JODA>";

            formatReader.close();

        } catch (Exception e) {
            if (!imagePath.endsWith("warmup")) {
                e.printStackTrace();
            }
        }
        return omeXML;
    }
}
