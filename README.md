# imageC

ImageC is a high performance image analysis tool designed to analyse microscopy images in the biological sciences.
With the help of pipelines images can be analyzed via batch processing.

## Features

- Reads tiff and big tiff images.
- Multi channel batch analysis of images with up to 12 different channels per image.
- Can interpret the [OME-XML](https://docs.openmicroscopy.org/ome-model/5.6.3/ome-xml/) description stored in the image.
- AI based ROI (region of interest) detection.
- Supporting following channel types: `EV`, `CELL`, `NUCLEUS`, `TETRASPECK BEAD`.
- Automatic CSV report generation.
- Single vesicle/cell/nucleus analysis and detailed reporting.

### Supported pipelines

#### Counting

Allows to count `EVs`, `CELLs` and `NUCLEI` in the different channels of an image.
Single particle analysis is done and data are stored as detailed and summery in CSV report files.

#### Colocalization

**Planned**
Calculate the colocalization of EVs in multiple channels and store the results to a CSV report.

#### In cell counting

**Planned**
Counts the number of EVs found within a cell area and calculates the colocalization.

## Example pictures

*cell segmentation*
![doc/cell_segmentation.jpg](doc/cell_segmentation.jpg)


## Class diagram

![doc/class_diagram.drawio.svg](doc/class_diagram.drawio.svg)

## Todo

- [ ] Ignore result folder


## Debugging

 valgrind --tool=massif --log-file="filename" ./build/build/tests "[pipeline_test_spots]"


## Build


`docker build --target run -t joda001/imagec:latest .`

`docker run -p 7367:7367 -v /home:/home joda001/imagec`


## Compile for Windows

### Preparation

Install MSYS2 and following packages:

```
pacman -S --needed base-devel mingw-w64-x86_64-toolchain
pacman -S mingw-w64-x86_64-catch
pacman -S mingw-w64-x86_64-pugixml
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

### Compile

Execute `make.bat` and `build.bat`.
The EXE file will be placed in `build/build/imagec.exe`

pacman -S mingw-w64-x86_64-wxwidgets3.2-common
pacman -S mingw-w64-x86_64-wxwidgets3.2-gtk3
pacman -S mingw-w64-x86_64-wxwidgets3.2-gtk3-libs
pacman -S mingw-w64-x86_64-wxwidgets3.2-common-libs


pacman -S mingw-w64-x86_64-wxwidgets3.2-msw

`
mingw-ldd.exe  imagec.exe --dll-lookup-dirs C:\msys64\mingw64\bin
`

### Deploy for windows

Use `mingw-ldd.exe  imagec.exe --dll-lookup-dirs C:\msys64\mingw64\bin` to check dll dependencies.
Copy the listed dependencies to the EXE folder of the application.
wx-config --cxxflags --libs

-IC:/Program Files/Git/mingw64/lib/wx/include/msw-unicode-3.2 
-IC:/Program Files/Git/mingw64/include/wx-3.2 
-D_FILE_OFFSET_BITS=64 
-DWXUSINGDLL 
-D__WXMSW__
-LC:/Program Files/Git/mingw64/lib   
-pipe -Wl,--subsystem,windows 
-mwindows 
-lwx_mswu_xrc-3.2 
-lwx_mswu_html-3.2 
-lwx_mswu_qa-3.2 
-lwx_mswu_core-3.2 
-lwx_baseu_xml-3.2 
-lwx_baseu_net-3.2 
-lwx_baseu-3.2
