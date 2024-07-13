// Import necessary classes
importClass(Packages.ij.IJ);
importClass(Packages.ij.WindowManager);

// 
// \brief Converts images for https://github.com/mgiacomelli/VirtualHE/blob/master/Opentkcpp/Form1.h#L182
// \date 2024-07-11
// \author JD
//
// How to use
//1) Open your image
//2) Close all channels which are not used (Two channels are neccessary)
//3) Set file path for output file
//4) Run script
//
// Define file output path
var filePath = "/home/danmayr.joachim/Downloads/output_01.bin";

// Get the list of all open image IDs
var imageIDs = WindowManager.getIDList();

// Check if there are any open images
if (imageIDs == null) {
    IJ.log("No images are open.");
} else {

    var fos = new FileOutputStream(filePath);
    var dos = new DataOutputStream(fos);

    // Resize to 1024x1024 because the VirtualHE expectes this
    var targetWidth = 1024;
    var targetHeight = 1024;

    // Iterate over each open image
    for (var i = 0; i < imageIDs.length; i++) {
        // Get the image by ID
        var imp = WindowManager.getImage(imageIDs[i]);
        IJ.run(imp, "Size...", "width=" + targetWidth + " height=" + targetHeight + " constrain average interpolation=Bilinear");
        var ip = imp.getProcessor();
        var width = imp.getWidth();
        var height = imp.getHeight();
        for (var x = 0; x < width; x++) {
            for (var y = 0; y < height; y++) {
                // Get the pixel value at (x, y)
                var pixelValue = ip.getPixel(x, y);
                // Diviede by four because the VirtualHE expectes 12 bit ADC value
                var uint16Value = (pixelValue & 0xFFFF) / 4;
                dos.writeShort(uint16Value);

            }
        }

    }
}
