cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=D:\a\_temp\msys64\mingw64\bin\gcc.exe -DCMAKE_CXX_COMPILER:FILEPATH=D:\a\_temp\msys64\mingw64\bin\g++.exe -S"$env:GITHUB_WORKSPACE" -B"$env:GITHUB_WORKSPACE/build" -G "MinGW Makefiles"
# This is a dirty hack, because the resource compiler did not create windows path correctly
(Get-Content -Path "build/CMakeFiles/imagec.dir/build.make") | ForEach-Object { $_ -replace "D:\\a\\imagec\\imagec\\src\\wx.rc", "D:/a/imagec/imagec/src/wx.rc" } | Set-Content -Path "build/CMakeFiles/imagec.dir/build.make"
cd build/build


$jvmdll = "$env:JAVA_HOME"
$dll = "\bin\server\jvm.dll"

ls $jvmdll


$jvmdll = "$jvmdll$dll"
echo "HO: $jvmdll"

Copy-Item -Path "$jvmdll" -Destination "./dlls" -Force


cmake --build build --target imagec --parallel 4

Remove-Item -Path "./dlls" -Recurse -Force


$input = $(ldd imagec.exe)
$result = ($(echo "$input" | grep -o '=> [^(]*' | cut -c 4-))

ldd imagec.exe
ni "dlls" -ItemType Directory


foreach ($item in $result) {
    $item = $item -replace '/mingw64', '/c/msys64/mingw64'

    if ($item -match "^/") {
        $item = $item -replace '/(.)/', '$1:/'
        Write-Output $item
        Copy-Item -Path "$item" -Destination "./dlls" -Force
    }else{
        echo "$item"
    }
}

ls ./dlls

strip imagec.exe

cd ../..
# cmake --build build --target tests --parallel 4
