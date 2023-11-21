

$mingwBasePath =  '/d/a/_temp/msys64/mingw64'
$mingwBasePathWin =  'D:\a\_temp\msys64\mingw64'

cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=D:\a\_temp\msys64\mingw64\bin\gcc.exe -DCMAKE_CXX_COMPILER:FILEPATH=D:\a\_temp\msys64\mingw64\bin\g++.exe -S"$env:GITHUB_WORKSPACE" -B"$env:GITHUB_WORKSPACE/build" -G "MinGW Makefiles"
# This is a dirty hack, because the resource compiler did not create windows path correctly
(Get-Content -Path "build/CMakeFiles/imagec.dir/build.make") | ForEach-Object { $_ -replace "D:\\a\\imagec\\imagec\\src\\wx.rc", "D:/a/imagec/imagec/src/wx.rc" } | Set-Content -Path "build/CMakeFiles/imagec.dir/build.make"
cd build/build

ni "dlls" -ItemType Directory


#$jvmdll = "$env:JAVA_HOME"
#$dll = "\jre\bin\server\jvm.dll"
#$jvmdll = "$jvmdll$dll"
#Copy-Item -Path "$jvmdll" -Destination "./dlls" -Force

cd ../..
cmake --build build --target imagec --parallel 8
cd build/build

# Copy dlls listed by ldd
#$input = $(ldd imagec.exe)
#$result = ($(echo "$input" | grep -o '=> [^(]*' | cut -c 4-))
#
#ldd imagec.exe
#
#foreach ($item in $result) {
#    $item = $item -replace '/mingw64', $mingwBasePath
#
#    if ($item -match "^/") {
#        $item = $item -replace '/(.)/', '$1:/'
#        Write-Output $item
#        Copy-Item -Path "$item" -Destination "./dlls" -Force
#    }else{
#        echo "$item"
#    }
#}

$destinationDirectory = "./dlls"

Copy-Item -Path "$mingwBasePathWin\bin\*.dll" -Destination "$destinationDirectory" -Force


ls ./dlls

strip imagec.exe


# Write priv key to file
"$env:PRIV_KEY" | Set-Content -Path privkey.key
"$env:PUB_KEY" | Set-Content -Path pubkey.pem

# Create pfx certificate
openssl pkcs12 -inkey privkey.key -in pubkey.pem -export -out mycert.pfx
# Sign the exe
signtool sign /fd SHA256 /f "mycert.pfx" /t http://zeitstempel.dfn.de "imagec.exe"

# Cleanup
Remove-Item -Force mycert.pfx
Remove-Item -Force privkey.key
Remove-Item -Force pubkey.key


Compress-Archive -Path ./dlls -DestinationPath win-dlls.zip
Remove-Item -Recurse -Force ./dlls

cd ../..
