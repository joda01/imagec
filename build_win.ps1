param(
    [string]$TAG_NAME,
    [string]$CONAN_IMAGEC_ARTIFACTORY_PW,
    [string]$WITH_CUDA,
    [string]$GITHUB_WORKSPACE
)

Write-Host "Start Windows build ..."
Write-Host "TAG_NAME: $TAG_NAME"
Write-Host "CONAN_IMAGEC_ARTIFACTORY_PW: $CONAN_IMAGEC_ARTIFACTORY_PW"
Write-Host "WITH_CUDA: $WITH_CUDA"
Write-Host "GITHUB_WORKSPACE: $GITHUB_WORKSPACE"

#
#
#
function Install-Dependencies {
  #
  # Install CMake, Git, Python, Conan
  #
  choco install -y cmake git
  python -m ensurepip --default-pip
  python -m pip install --upgrade pip
  python -m pip install conan numpy

  #
  # Install CUDA Toolkit (no GPU required for compilation)
  #
  Invoke-WebRequest -Uri https://developer.download.nvidia.com/compute/cuda/12.8.0/network_installers/cuda_12.8.0_windows_network.exe -OutFile cuda_installer.exe
  Start-Process -Wait -FilePath cuda_installer.exe -ArgumentList '-s','nvcc_12.8','visual_studio_integration_12.8'
  Remove-Item cuda_installer.exe

  #
  # Set ENV variables
  #
  $env:CUDA_PATH = "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.8"
  $env:PATH = "$env:CUDA_PATH\bin;$env:CUDA_PATH\libnvvp;" + $env:PATH
}

#
#
#
function Fetch-ExternalLibs {
  conan remote remove conancenter
  conan remote add imageclibs https://imagec.org:4431/artifactory/api/conan/imageclibs
  conan remote login imageclibs writer -p $CONAN_IMAGEC_ARTIFACTORY_PW

  if (Test-Path "C:\Users\runneradmin\.conan2\profiles\default") {
      Write-Host "Loaded from cache"
  }
  else {
      conan profile detect
  }

  conan install . `
      --profile $GITHUB_WORKSPACE\conan\profile_win `
      --output-folder=build `
      --build=missing `
      -o:a "&:with_cuda=$WITH_CUDA"
}

#
#
#
function Build {
  Set-Location -Path "build"

  cmake .. `
    -G "Unix Makefiles" `
    -DTAG_NAME="$TAG_NAME" `
    -DWITH_CUDA="$WITH_CUDA" `
    -DCMAKE_BUILD_TYPE="Release" `
    -DCMAKE_POLICY_DEFAULT_CMP0091=NEW `
    -DCMAKE_TOOLCHAIN_FILE="build/Release/generators/conan_toolchain.cmake" `
    -DCUDA_TOOLKIT_ROOT_DIR="/usr/local/cuda" `
    -DCMAKE_CUDA_COMPILER="/usr/local/cuda/bin/nvcc"

  cmake --build . --config Release --target imagec --parallel 8
}

#
#
#
function Pack {
  cd build/build
  strip "D:\a\imagec\imagec\build\build\Release\imagec.exe"
  ni "output" -ItemType Directory
  cd output
  ni "plugins" -ItemType Directory
  ni "models" -ItemType Directory
  ni "java" -ItemType Directory
  Copy-Item -Path "D:\a\imagec\imagec\build\build\Release\imagec.exe" -Destination "imagec.exe"
          
  $conanLibInstallPath = "C:\Users\runneradmin\.conan2\p"

  Get-ChildItem -Path "$conanLibInstallPath\*" | Where-Object {$_.Name -like "qt*"} | ForEach-Object {
    $pathToCopyFrom = $conanLibInstallPath + "\" + $_.Name + "\p\.\plugins\*"
    if (Test-Path "$pathToCopyFrom") {
      Copy-Item -Recurse -Path $pathToCopyFrom -Destination "./plugins"  -Force
    }
  }

  Get-ChildItem -Path "$conanLibInstallPath\*" | Where-Object {$_.Name -like "qt*"} | ForEach-Object {
    $pathToCopyFrom = $conanLibInstallPath + "\" + $_.Name + "\p\bin\Qt6Core.dll"
    if (Test-Path "$pathToCopyFrom") {
      Copy-Item -Path $pathToCopyFrom -Destination "."  -Force
    }
  }

  Get-ChildItem -Path "$conanLibInstallPath\*" | Where-Object {$_.Name -like "qt*"} | ForEach-Object {
    $pathToCopyFrom = $conanLibInstallPath + "\" + $_.Name + "\p\bin\Qt6Gui.dll"
    if (Test-Path "$pathToCopyFrom") {
      Copy-Item -Path $pathToCopyFrom -Destination "."  -Force 
    }
  }
          
  Get-ChildItem -Path "$conanLibInstallPath\*" | Where-Object {$_.Name -like "qt*"} | ForEach-Object {
    $pathToCopyFrom = $conanLibInstallPath + "\" + $_.Name + "\p\bin\Qt6Widgets.dll"
    if (Test-Path "$pathToCopyFrom") {
      Copy-Item -Path $pathToCopyFrom -Destination "."  -Force 
    }
  }

  Get-ChildItem -Path "$conanLibInstallPath\*" | Where-Object {$_.Name -like "qt*"} | ForEach-Object {
    $pathToCopyFrom = $conanLibInstallPath + "\" + $_.Name + "\p\bin\Qt6Svg.dll"
    if (Test-Path "$pathToCopyFrom") {
      Copy-Item -Path $pathToCopyFrom -Destination "."  -Force 
    }
  }
  $dllsToCopy = @(
      "torch_cpu.dll",
      "torch.dll",
      "c10.dll"
  )

  if ($env:WITH_CUDA -eq 'True') {
    $dllsToCopy += @(
      "torch_cuda.dll", 
      "c10_cuda.dll",
      "libcudart-d0da41ae.so.11.0",
      "libcublas-3b81d170.so.11",
      "libcublasLt-b6d14a74.so.11",
      "libcudnn.so.9",
      "libcudnn_graph.so.9",
      "libcudnn_heuristic.so.9 ",
      "libcudnn_engines_runtime_compiled.so.9 .",
      "libcudnn_cnn.so.9",
      "libcudnn_engines_precompiled.so.9"
      )
  } 

  foreach ($dll in $dllsToCopy) {
      Get-ChildItem -Path "$conanLibInstallPath\*" | Where-Object { $_.Name -like "libtoc*" } | ForEach-Object {
          $pathToCopyFrom = Join-Path $_.FullName "p\lib\$dll"
          if (Test-Path $pathToCopyFrom) {
              Copy-Item -Path $pathToCopyFrom -Destination "." -Force
          }
      }
  }
          
          
  ls "C:\Windows\System32"
  Copy-Item -Path "C:\Windows\System32\vcruntime140.dll" -Destination "."  -Force
  Copy-Item -Path "C:\Windows\System32\vcruntime140_1.dll" -Destination "."  -Force
  Copy-Item -Path "C:\Windows\System32\msvcp140.dll" -Destination "."  -Force
  Copy-Item -Path "C:\Windows\System32\msvcp140_1.dll" -Destination "."  -Force
  Copy-Item -Path "C:\Windows\System32\msvcp140_2.dll" -Destination "."  -Force
  Copy-Item -Path "C:\Windows\System32\concrt140.dll" -Destination "."  -Force
  Copy-Item -Path "C:\Windows\System32\msvcp140_codecvt_ids.dll" -Destination "."  -Force
  Copy-Item -Path "C:\Windows\System32\vcomp140.dll" -Destination "."  -Force

  Copy-Item -Recurse -Path "$GITHUB_WORKSPACE/resources/templates" ./templates
  cd java
  Copy-Item -Recurse -Path "$GITHUB_WORKSPACE/resources/java/bioformats.jar" -Destination "."
  Copy-Item -Recurse -Path "$GITHUB_WORKSPACE/resources/java/BioFormatsWrapper.class" -Destination "."
  Copy-Item -Recurse -Path "$GITHUB_WORKSPACE/resources/java/jre_win.zip" -Destination "."
  Expand-Archive "jre_win.zip" -DestinationPath "."
  Remove-Item jre_win.zip
  cd ..
}

Install-Dependencies
Fetch-ExternalLibs
Build
Pack
