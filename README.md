[![C++ Build](https://github.com/joda01/imagec/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/joda01/imagec/actions/workflows/cmake-multi-platform.yml)
![GitHub Release](https://img.shields.io/github/v/release/joda01/imagec)
![GitHub Downloads (all assets, latest release)](https://img.shields.io/github/downloads/joda01/imagec/latest/total)
![GitHub License](https://img.shields.io/github/license/joda01/imagec)


# imageC (EVAnalyzer 2)

is an open source application designed for high throughput analyzing of microscopy images in the biological sciences.  

With imageC image processing pipelines can be defined which are applied on a set of images to analyze.
The resulting data is stored in a file-based database (duckdb) and can either be viewed directly in imageC using the built-in reporting tool, or the data can be exported to other file formats.  

imageC (EVAnalayzer 2), the direct successor of [EVAnalayzer](https://github.com/joda01/evanalyzer) an imageJ plugin with more than 3000 downloads (June 2024), is a standalone application written in C++.
The main goals were to improve performance, allow the processing of big tiffs and improve usability.

## Feature comparison

|                                                           |imageC   |EVAnalyzer |
|-----------------------------------------------------------|-------  |-------    |
|[BioFormats support](https://github.com/ome/bioformats)    |x        |x          |
|[OME-XML support](https://docs.openmicroscopy.org/)        |x        |x          |
|XLSX report generation                                     |x        |x          |
|Max. channels                                              |10       |5          |
|Max image size                                             |no limit |2GB        |
|Support for big tiff                                       |x        |-          |
|AI based object detection                                  |x        |-          |
|Database based result processing                           |x        |-          |
|Heatmap generation                                         |x        |-          |
|Image density map generation                               |x        |-          |
|In image object marker                                     |x        |-          |
|Built-in reporting tool                                    |x        |-          |
|Side by side image comparison                              |x        |-          |

## Screenshots

![doc/screenshot_start.png](doc/screenshot_start.png)
![doc/screenshot_channel.png](doc/screenshot_channel.png)
![doc/screenshot_comparison.png](doc/screenshot_comparison.png)
![doc/screenshot_plate.png](doc/screenshot_plate.png)
![doc/screenshot_well.png](doc/screenshot_well.png)


## Using pyramid images

Covert to

`
bfconvert -no-upgrade -noflat -pyramid-resolutions 4 -pyramid-scale 2 -tilex 512 -tiley 512 /workspaces/imagec/test_local/bigtif/GMEV5minM1OT3_0001.btf /workspaces/imagec/test_local/bigtif/GMEV5minM1OT3_0001.ome.btf
`

-----

## Developers section

This section contains information needed for anyone who is interested and especially for those who would like to contribute.

### Database schema

All data generated during an analysis run are stored using the file based database duckdb.
Following database schema is used:

![doc/database_schema.drawio.svg](doc/database_schema.drawio.svg)

The data generated for each detected ROI are stored in the `objects` table.


#### Image ID coding

The `object.image_id` identifies an image unique per run.
This id is calculated by the `fnv1a` hash: `object.image_id = fnv1a(<ORIGINAL-IMAGE-PATH>)`

## Debugging

 valgrind --tool=massif --log-file="filename" ./build/build/tests "[pipeline_test_spots]"
 valgrind --gen-suppressions=all --tool=massif --log-file="filename" ./build/build/tests "[pipeline_test_nucleus]"


### Add new command

- Create new command in `backend/commands/image_functions`
- Add the new command to `backend/settings/pipeline/pipeline_step.hpp`
- Add the new command to `backend/settings/pipeline/pipeline_factory.hpp`
- Add the command tp `ui/container/dialog_command_selection.cpp`

## Build


### Deploy build docker image

docker build --target live -t joda001/imagec:live .
docker build --target build -t joda001/imagec:v1.7.x .
docker push  joda001/imagec:v1.7.x

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


## Used open source libs

Many thank's to the authors of following open source libraries I used:


Title                   | Link                                          | License
------                  |-------                                        |--------
nlohmann/json           |https://github.com/nlohmann/json.git           | MIT
zeux/pugixml            |https://github.com/zeux/pugixml                | MIT
protocolbuffers/protobuf|https://github.com/protocolbuffers/protobuf    | Google Inc.
opencv/opencv           |https://github.com/opencv/opencv.git           | Apache-2.0
qt6                     |https://code.qt.io/cgit/                       | LGPL-3.0
jmcnamara/libxlsxwriter |https://github.com/jmcnamara/libxlsxwriter.git | FreeBSD
duckdb/duckdb           |https://github.com/duckdb/duckdb               | MIT
ome/bioformats          |https://github.com/ome/bioformats              | GPL-2.0
microsoft/onnxruntime   |https://github.com/microsoft/onnxruntime       | MIT

Thank's to the authors of [imagej](https://github.com/imagej/imagej2) I ported some image processing algorithms from to C++.  
imageC is the follower of [evanalyzer](https://github.com/joda01/evanalyzer).



## For MacOS you have to do

chmod +x imagec.app/Contents/MacOS/imagec
xattr -dr com.apple.quarantine imagec.app
open imagec.app
