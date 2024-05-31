

$mingwBasePath =  '/c/msys64/mingw64'
$mingwBasePathWin =  'C:\msys64\mingw64'

Copy-Item -Path ".\lib\libduckdb-windows-amd64\libduckdb.dll" -Destination "$mingwBasePathWin\bin" -Force
Copy-Item -Path ".\lib\libduckdb-windows-amd64\libduckdb.dll.a" -Destination "$mingwBasePathWin\lib" -Force
Copy-Item -Path ".\lib\libduckdb-windows-amd64\libduckdb_static.a" -Destination "$mingwBasePathWin\lib" -Force
Copy-Item -Path ".\lib\libduckdb-windows-amd64\duckdb.hpp" -Destination "$mingwBasePathWin\include" -Force
Copy-Item -Path ".\lib\libduckdb-windows-amd64\duckdb.h" -Destination "$mingwBasePathWin\include" -Force
Copy-Item -Path ".\lib\libduckdb-windows-amd64\libparquet.dll.a" -Destination "$mingwBasePathWin\lib" -Force
Copy-Item -Path ".\lib\libduckdb-windows-amd64\libparquet_extension.a" -Destination "$mingwBasePathWin\lib" -Force
Copy-Item -Recurse -Path ".\lib\libduckdb-windows-amd64\duckdb" -Destination "$mingwBasePathWin\include" -Force
Copy-Item -Recurse -Path ".\lib\libduckdb-windows-amd64\cmake_local\*.cmake" -Destination "$mingwBasePathWin\lib\cmake\duckdb" -Force
Copy-Item -Recurse -Path ".\lib\libduckdb-windows-amd64\third_party\*.a" -Destination "$mingwBasePathWin\lib" -Force


cmake --no-warn-unused-cli -DTAG_NAME="$env:TAG_NAME" -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=C:\msys64\mingw64\bin\gcc.exe -DCMAKE_CXX_COMPILER:FILEPATH=C:\msys64\mingw64\bin\g++.exe -S"./" -B"./build" -G "MinGW Makefiles"
# This is a dirty hack, because the resource compiler did not create windows path correctly
cd build/build

ni "dlls" -ItemType Directory


#$jvmdll = "$env:JAVA_HOME"
#$dll = "\jre\bin\server\jvm.dll"
#$jvmdll = "$jvmdll$dll"
#Copy-Item -Path "$jvmdll" -Destination "./dlls" -Force

cd ../..
cmake --build build --target imagec --parallel 2
cd build/build

$input = $(ldd imagec.exe)
$result = ($(echo "$input" | grep -o '=> [^(]*' | cut -c 4-))

foreach ($item in $result) {
    $item = $item -replace '/mingw64', $mingwBasePath

    if ($item -match "^/") {
        $item = $item -replace '/(.)/', '$1:/'
        Write-Output $item
        Copy-Item -Path "$item" -Destination "./dlls" -Force
    }else{
        echo "$item"
    }
}

$destinationDirectory = "./dlls"

Copy-Item -Path "$mingwBasePathWin\bin\*.dll" -Destination "$destinationDirectory" -Force


#ls ./dlls

strip imagec.exe

#Compress-Archive -Path ./dlls -DestinationPath win-dlls.zip

Remove-Item -Recurse -Force ./dlls

cd ../..
