cd build
Get-ChildItem -Path "C:\Users\Joachim\.conan2\p\b\*" | Where-Object {$_.Name -like "duck*"} | ForEach-Object {
  $pathToCopy = "C:\Users\Joachim\.conan2\p\b\"+$_.Name + "\p\include"
  Copy-Item -Path "C:\Users\Joachim\Documents\github\imagec\lib\libduckdb-windows-amd64\duckdb.hpp" -Destination $pathToCopy  -Force 
}

Get-ChildItem -Path "C:\Users\Joachim\.conan2\p\b\*" | Where-Object {$_.Name -like "duck*"} | ForEach-Object {
  $pathToCopy = "C:\Users\Joachim\.conan2\p\b\"+$_.Name + "\p\include"
  Copy-Item -Path "C:\Users\Joachim\Documents\github\imagec\lib\libduckdb-windows-amd64\duckdb.h" -Destination $pathToCopy  -Force   
}

Get-ChildItem -Path "C:\Users\Joachim\.conan2\p\b\*" | Where-Object {$_.Name -like "duck*"} | ForEach-Object {
  $pathToCopy = "C:\Users\Joachim\.conan2\p\b\"+$_.Name + "\p\include"
  Copy-Item  -Recurse -Path "C:\Users\Joachim\Documents\github\imagec\lib\libduckdb-windows-amd64\duckdb" -Destination $pathToCopy  -Force   
}

cmake .. -G "MinGW Makefiles" -DTAG_NAME="$TAG_NAME" -DCMAKE_SH=CMAKE_SH-NOTFOUND -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="build/Release/generators/conan_toolchain.cmake"
cmake --build . --config Release --target imagec --parallel 8
