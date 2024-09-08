
$mingwBasePathWin =  'D:\a\_temp\msys64\mingw64'
$mingwQtPlatformsPath =  'D:\a\_temp\msys64\mingw64\share\qt6\plugins\platforms'
$mingwQtImageFormatsPath =  'D:\a\_temp\msys64\mingw64\share\qt6\plugins\imageformats'

ni "$mingwBasePathWin\lib\cmake\duckdb" -ItemType Directory

Copy-Item -Path "$env:GITHUB_WORKSPACE\lib\libduckdb-windows-amd64\libduckdb.dll" -Destination "$mingwBasePathWin\bin" -Force
Copy-Item -Path "$env:GITHUB_WORKSPACE\lib\libduckdb-windows-amd64\libduckdb.dll.a" -Destination "$mingwBasePathWin\lib" -Force
Copy-Item -Path "$env:GITHUB_WORKSPACE\lib\libduckdb-windows-amd64\libduckdb_static.a" -Destination "$mingwBasePathWin\lib" -Force
Copy-Item -Path "$env:GITHUB_WORKSPACE\lib\libduckdb-windows-amd64\duckdb.hpp" -Destination "$mingwBasePathWin\include" -Force
Copy-Item -Path "$env:GITHUB_WORKSPACE\lib\libduckdb-windows-amd64\duckdb.h" -Destination "$mingwBasePathWin\include" -Force
Copy-Item -Path "$env:GITHUB_WORKSPACE\lib\libduckdb-windows-amd64\libparquet.dll.a" -Destination "$mingwBasePathWin\lib" -Force
Copy-Item -Path "$env:GITHUB_WORKSPACE\lib\libduckdb-windows-amd64\libparquet_extension.a" -Destination "$mingwBasePathWin\lib" -Force
Copy-Item -Recurse -Path "$env:GITHUB_WORKSPACE\lib\libduckdb-windows-amd64\duckdb" -Destination "$mingwBasePathWin\include" -Force
Copy-Item -Recurse -Path "$env:GITHUB_WORKSPACE\lib\libduckdb-windows-amd64\cmake\*.cmake" -Destination "$mingwBasePathWin\lib\cmake\duckdb" -Force
Copy-Item -Recurse -Path "$env:GITHUB_WORKSPACE\lib\libduckdb-windows-amd64\third_party\*.a" -Destination "$mingwBasePathWin\lib" -Force


ls "$mingwBasePathWin\lib\cmake"
ls "$mingwBasePathWin\lib\cmake\duckdb"

cmake --no-warn-unused-cli -DTAG_NAME="$env:TAG_NAME" -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=D:\a\_temp\msys64\mingw64\bin\gcc.exe -DCMAKE_CXX_COMPILER:FILEPATH=D:\a\_temp\msys64\mingw64\bin\g++.exe -S"$env:GITHUB_WORKSPACE" -B"$env:GITHUB_WORKSPACE/build" -G "MinGW Makefiles"
# This is a dirty hack, because the resource compiler did not create windows path correctly
cd build/build

ni "dlls" -ItemType Directory
ni "dlls\platforms" -ItemType Directory
ni "dlls\imageformats" -ItemType Directory
ni "dlls\templates\basic" -ItemType Directory
ni "dlls\templates\eva" -ItemType Directory
#$jvmdll = "$env:JAVA_HOME"
#$dll = "\jre\bin\server\jvm.dll"
#$jvmdll = "$jvmdll$dll"
#Copy-Item -Path "$jvmdll" -Destination "./dlls" -Force

cd ../..
cmake --build build --target imagec --config Release --parallel 8
cd build/build


$destinationDirectory = ".\dlls"
$destinationDirectoryPlatforms = ".\dlls\platforms"
$destinationDirectoryPlugins = ".\dlls\imageformats"
$destinationDirectoryTemplates = ".\dlls\templates"

Copy-Item -Path "$mingwBasePathWin\bin\*.dll" -Destination "$destinationDirectory" -Force
Copy-Item -Path "$mingwQtPlatformsPath\*.dll" -Destination "$destinationDirectoryPlatforms" -Force
Copy-Item -Path "$mingwQtImageFormatsPath\*.dll" -Destination "$destinationDirectoryPlugins" -Force
Copy-Item -Path "..\..\templates\basic\*.ictempl" -Destination "$destinationDirectoryTemplates\basic" -Force
Copy-Item -Path "..\..\templates\eva\*.ictempl" -Destination "$destinationDirectoryTemplates\eva" -Force



ls ./dlls

strip imagec.exe

Compress-Archive -Path ./dlls -DestinationPath win-dlls.zip
Remove-Item -Recurse -Force ./dlls

#
# Start signing process
#

# Write priv key to file
"$env:PRIV_KEY" | Set-Content -Path privkey.key
"$env:PUB_KEY" | Set-Content -Path pubkey.pem

# Create pfx certificate
openssl pkcs12 -password pass: -inkey privkey.key -in pubkey.pem -export -out mycert.pfx
# Sign the exe

& "C:/Program Files (x86)/Windows Kits/10/bin/10.0.22621.0/x64/signtool.exe" sign /fd SHA256 /td sha256 /f "mycert.pfx" /tr http://timestamp.digicert.com "imagec.exe"

# Cleanup
Remove-Item -Force mycert.pfx
Remove-Item -Force privkey.key
Remove-Item -Force pubkey.pem

cd ../..


# platforms/qwindows.dll
# platforms/qminimal.dll
