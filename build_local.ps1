

$mingwBasePath =  '/c/msys64/mingw64'
$mingwBasePathWin =  'C:\msys64\mingw64'

cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=C:\msys64\mingw64\bin\gcc.exe -DCMAKE_CXX_COMPILER:FILEPATH=C:\msys64\mingw64\bin\g++.exe -S"./" -B"./build" -G "MinGW Makefiles"
# This is a dirty hack, because the resource compiler did not create windows path correctly
(Get-Content -Path "build/CMakeFiles/imagec.dir/build.make") | ForEach-Object { $_ -replace "C:\\Users\\joachim\\Documents\\github\\imagec\\src\\wx.rcc", "C:/Users/joachim/Documents/github/imagec/src/wx.rcc" } | Set-Content -Path "build/CMakeFiles/imagec.dir/build.make"
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


ls ./dlls

strip imagec.exe

#Compress-Archive -Path ./dlls -DestinationPath win-dlls.zip

Remove-Item -Recurse -Force ./dlls

cd ../..
