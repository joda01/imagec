
REM rm -f -r ./build
REM mkdir ./build
cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=C:\msys64\mingw64\bin\gcc.exe -DCMAKE_CXX_COMPILER:FILEPATH=C:\msys64\mingw64\bin\g++.exe -SC:/Users/Joachim/Documents/github/imagec -Bc:/Users/Joachim/Documents/github/imagec/build -G "MinGW Makefiles"