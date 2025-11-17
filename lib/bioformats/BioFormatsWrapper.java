
// BioFormatsWrapper.java

import loci.common.services.ServiceFactory;
import loci.formats.ImageReader;
import loci.formats.in.MetadataOptions;
import loci.formats.in.OMEXMLReader;
import loci.formats.meta.IMetadata;
import loci.formats.services.OMEXMLService;
import ome.xml.meta.OMEXMLMetadata;
import ome.xml.model.primitives.PositiveInteger;

import java.io.File;
import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Map;

import loci.common.DebugTools;
import loci.formats.IFormatReader;
import loci.formats.Memoizer;

public class BioFormatsWrapper {

    IFormatReader formatReader = new Memoizer(new ImageReader(), 1, null);
    OMEXMLService service;

    public BioFormatsWrapper(String imagePath) {
        try {
            ServiceFactory factory = new ServiceFactory();
            service = factory.getInstance(OMEXMLService.class);
            IMetadata metadata = service.createOMEXMLMetadata();
            formatReader.setMetadataStore(metadata);
            formatReader.setFlattenedResolutions(false);
            formatReader.setId(imagePath);
        } catch (Exception e) {
            if (!imagePath.endsWith("warmup")) {
                e.printStackTrace();
            }
        }
    }

    public void close() {
        try {
            formatReader.close();
        } catch (Exception e) {

        }
    }

    public void readImage(ByteBuffer targetBuffer, String imagePath, int series, int resolution, int z, int c,
            int t) {

        DebugTools.setRootLevel("OFF");

        try {
            formatReader.setSeries(series);
            if (resolution >= formatReader.getResolutionCount()) {
                resolution = formatReader.getResolutionCount() - 1;
            }
            formatReader.setResolution(resolution);
            byte[] imageBytes = formatReader.openBytes(formatReader.getIndex(z, c, t));
            targetBuffer.put(imageBytes);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void readImageTile(ByteBuffer targetBuffer, String imagePath, int series, int resolution, int z,
            int c, int t, int x, int y,
            int width, int height) {
        DebugTools.setRootLevel("OFF");
        try {
            // Create an appropriate reader for the format
            formatReader.setSeries(series);
            if (resolution >= formatReader.getResolutionCount()) {
                resolution = formatReader.getResolutionCount() - 1;
            }
            formatReader.setResolution(resolution);
            // Read the image data for the current channel, timepoint, and slice
            byte[] imageBytes = formatReader.openBytes(formatReader.getIndex(z, c, t), x, y, width, height);
            targetBuffer.put(imageBytes);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /// https://docs.openmicroscopy.org/ome-model/6.2.2/ome-tiff/specification.html
    public String getImageProperties(String imagePath, int tmp/* series */) {
        DebugTools.setRootLevel("OFF");
        String omeXML = "";
        try {
            // Create a service factory
            int seriesCount = formatReader.getSeriesCount();
            omeXML = service.getOMEXML((IMetadata) formatReader.getMetadataStore());
            omeXML = omeXML + "\n<JODA xmlns=\"https://www.imagec.org/\" SeriesCount=\""
                    + String.valueOf(seriesCount) + "\">";

            for (int series = 0; series < seriesCount; series++) {
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
        } catch (Exception e) {
            if (!imagePath.endsWith("warmup")) {
                e.printStackTrace();
            }
        }
        return omeXML;
    }
}
