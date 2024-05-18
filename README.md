# imageC

Designed for analyzing microscopy images in the biological sciences, ImageC is a powerful image analysis tool.

## Features

- Support for big tiff.
- Support for [Bio-Formats](https://www.openmicroscopy.org/bio-formats/) image reader.
- Multi channel batch analysis of images with up to 12 different channels per image.
- Can interpret the [OME-XML](https://docs.openmicroscopy.org/ome-model/5.6.3/ome-xml/) description stored in the image.
- AI or Threshold based ROI (region of interest) detection.
- Supporting channel types: `SPOT`, `CELL`, `NUCLEUS`, `TETRASPECK BEAD`.
- Automatic CSV report generation.
- Single vesicle/cell/nucleus analysis and detailed reporting.


## imageC output file format

imageC generates a `.tar.gz` file for each finished job.



## Example pictures

*cell segmentation*
![doc/cell_segmentation.jpg](doc/cell_segmentation.jpg)


## Class diagram

![doc/class_diagram.drawio.svg](doc/class_diagram.drawio.svg)

## Todo

- [ ] Ignore result folder
- [ ] Support for reference spot removal in preview.


## Debugging

 valgrind --tool=massif --log-file="filename" ./build/build/tests "[pipeline_test_spots]"
 valgrind --gen-suppressions=all --tool=massif --log-file="filename" ./build/build/tests "[pipeline_test_nucleus]"

## Build


### Deploy build docker image

docker build --target live -t joda001/imagec:live .
docker build --target build -t joda001/imagec:v1.7.8 .
docker push  joda001/imagec:v1.7.8

### Build for Windows

#### Preparation

Install MSYS2 and following packages:

```
pacman -S --needed base-devel mingw-w64-x86_64-toolchain
pacman -S mingw-w64-x86_64-catch
pacman -S mingw-w64-x86_64-pugixml
pacman -S mingw-w64-protobuf
pacman -S mingw-w64-x86_64-opencv
pacman -S mingw-w64-x86_64-nlohmann-json
pacman -S mingw-w64-x86_64-libtiff
pacman -S mingw-w64-x86_64-python-mingw-ldd
pacman -S mingw-w64-x86_64-qt6-base
pacman -S mingw-w64-x86_64-libxlsxwriter
```

Add following ENV variables:

```
C:\msys64\usr\bin
C:\msys64\mingw64\bin
```

Add powershell permissions:

`set-executionpolicy remotesigned`

#### Compile

Execute `make.ps1` and `build_local.ps1`.
The EXE file will be placed in `build/build/imagec.exe`

`
mingw-ldd.exe  imagec.exe --dll-lookup-dirs C:\msys64\mingw64\bin
strip.exe imagec.exe
`

### ONNX model reader

git clone https://github.com/onnx/onnx.git
protoc onnx/onnx.proto --cpp_out="out"


```
export LD_LIBRARY_PATH=/Documents/privat/github/imagec/imagec/build/build:$LD_LIBRARY_PATH
export QT_QPA_PLATFORM_PLUGIN_PATH=/Documents/privat/github/imagec/imagec/build/build/platforms

```
sudo apt-get install libxcb-xinerama0
sudo apt-get install libxkbcommon-x11-0
apt-get install --reinstall libxcb-xinerama0


sudo apt-get install libxcb-util-dev


apt-get install '^libxcb.*-dev' libx11-xcb-dev libglu1-mesa-dev libxrender-dev libxi-dev libxkbcommon-dev libxkbcommon-x11-dev


## TODO

- [ ] For tiled images, calculate real X/Y values of pixels in region of interest

----

## Used open source libs

Many thank's to the authors of following open source libraries I used:


Title                   | Link                                          | License
------                  |-------                                        |--------
nlohmann/json           |https://github.com/nlohmann/json.git           | MIT
zeux/pugixml            |https://github.com/zeux/pugixml                | MIT
protocolbuffers/protobuf|https://github.com/protocolbuffers/protobuf    | Google Inc.
opencv/opencv           |https://github.com/opencv/opencv.git           | Apache-2.0
qt6                     |https://code.qt.io/cgit/                       | LGPL-3.0
libtiff/libtiff         |https://gitlab.com/libtiff/libtiff.git         | Silicon Graphics, Inc.
jmcnamara/libxlsxwriter |https://github.com/jmcnamara/libxlsxwriter.git | FreeBSD
tukaani-project/xz      |https://github.com/tukaani-project/xz          | GPL-3.0
madler/zlib             |https://github.com/madler/zlib                 | Own
nih-at/libzip           |https://github.com/nih-at/libzip               | Own
duckdb/duckdb           |https://github.com/duckdb/duckdb               | MIT
ome/bioformats          |https://github.com/ome/bioformats              | GPL-2.0


Thank's to the authors of [imagej](https://github.com/imagej/imagej2) I ported some image processing algorithms from to C++.  

imageC is the follower of [evanalyzer](https://github.com/joda01/evanalyzer).
