#conan install . --profile conan/profile_win_mingw --output-folder=build --build=missing
cd build
#cmake .. -G "MinGW Makefiles" -DTAG_NAME="$TAG_NAME" -DCMAKE_SH=CMAKE_SH-NOTFOUND -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="build/Release/generators/conan_toolchain.cmake"
cmake --build . --config Release --target imagec --parallel 8
exit
cd build
 ni "plugins" -ItemType Director
          $qtInstallPath = "C:\Users\Joachim\.conan2\p\b"

          Get-ChildItem -Path "$qtInstallPath\*" | Where-Object {$_.Name -like "qt*"} | ForEach-Object {
            $pathToCopyFrom = $qtInstallPath + "\" + $_.Name + "\p\.\plugins\*"
            Copy-Item -Recurse -Path $pathToCopyFrom -Destination "./plugins"  -Force 
          }

          Get-ChildItem -Path "$qtInstallPath\*" | Where-Object {$_.Name -like "qt*"} | ForEach-Object {
            $pathToCopyFrom = $qtInstallPath + "\" + $_.Name + "\p\bin\Qt6Core.dll"
            Copy-Item -Path $pathToCopyFrom -Destination "."  -Force 
          }

          Get-ChildItem -Path "$qtInstallPath\*" | Where-Object {$_.Name -like "qt*"} | ForEach-Object {
            $pathToCopyFrom = $qtInstallPath + "\" + $_.Name + "\p\bin\Qt6Gui.dll"
            Copy-Item -Path $pathToCopyFrom -Destination "."  -Force 
          }
          
          Get-ChildItem -Path "$qtInstallPath\*" | Where-Object {$_.Name -like "qt*"} | ForEach-Object {
            $pathToCopyFrom = $qtInstallPath + "\" + $_.Name + "\p\bin\Qt6Widgets.dll"
            Copy-Item -Path $pathToCopyFrom -Destination "."  -Force 
          }

          Get-ChildItem -Path "$qtInstallPath\*" | Where-Object {$_.Name -like "qt*"} | ForEach-Object {
            $pathToCopyFrom = $qtInstallPath + "\" + $_.Name + "\p\bin\Qt6Svg.dll"
            Copy-Item -Path $pathToCopyFrom -Destination "."  -Force 
          }


 $mingwPath = "C:\Users\Joachim\.conan2\p"
          Get-ChildItem -Path "$mingwPath\*" | Where-Object {$_.Name -like "mingwf*"} | ForEach-Object {
            $pathToCopyFrom = $mingwPath
            ls $pathToCopyFrom
            
            $pathToCopyFrom = $mingwPath + "\" + $_.Name
            ls $pathToCopyFrom
            
            $pathToCopyFrom = $mingwPath + "\" + $_.Name + "\p"
            ls $pathToCopyFrom
            
            $pathToCopyFrom = $mingwPath + "\" + $_.Name + "\p\x86_64-w64-mingw32"
            ls $pathToCopyFrom

            $pathToCopyFrom = $mingwPath + "\" + $_.Name + "\p\x86_64-w64-mingw32\lib"
            ls $pathToCopyFrom
            
            $pathToCopyFrom = $mingwPath + "\" + $_.Name + "\p\x86_64-w64-mingw32\lib\libatomic-1.dll"
            Copy-Item -Path $pathToCopyFrom -Destination "."  -Force 

            $pathToCopyFrom = $mingwPath + "\" + $_.Name + "\p\x86_64-w64-mingw32\lib\libgcc_s_seh-1.dll"
            Copy-Item -Path $pathToCopyFrom -Destination "."  -Force 

            $pathToCopyFrom = $mingwPath + "\" + $_.Name + "\p\x86_64-w64-mingw32\lib\libgfortran-5.dll"
            Copy-Item -Path $pathToCopyFrom -Destination "."  -Force 

            $pathToCopyFrom = $mingwPath + "\" + $_.Name + "\p\x86_64-w64-mingw32\lib\libgomp-1.dll"
            Copy-Item -Path $pathToCopyFrom -Destination "."  -Force 

            $pathToCopyFrom = $mingwPath + "\" + $_.Name + "\p\x86_64-w64-mingw32\lib\libquadmath-0.dll"
            Copy-Item -Path $pathToCopyFrom -Destination "."  -Force 

            $pathToCopyFrom = $mingwPath + "\" + $_.Name + "\p\x86_64-w64-mingw32\lib\libstdc++-6.dll"
            Copy-Item -Path $pathToCopyFrom -Destination "."  -Force 

            $pathToCopyFrom = $mingwPath + "\" + $_.Name + "\p\x86_64-w64-mingw32\lib\libwinpthread-1.dll"
            Copy-Item -Path $pathToCopyFrom -Destination "."  -Force 
          }
