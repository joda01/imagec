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

### Supported pipelines

#### Counting

Allows to count `EVs`, `CELLs` and `NUCLEI` in the different channels of an image.
Single particle analysis is done and data are stored as detailed and summery in CSV report files.

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

docker build --target live -t joda001/imagec:v1.3.0 .
docker push  joda001/imagec:v1.3.0

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
pacman -S mingw-w64-x86_64-wxwidgets3.2-msw
pacman -S mingw-w64-x86_64-python-mingw-ldd
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
