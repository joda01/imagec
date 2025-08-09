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
  # Install CUDA Toolkit (no GPU required for compilation)
  #
  if ($WITH_CUDA -eq "True") {
    try {
      Write-Host "Downloading CUDA installer..."
      Invoke-WebRequest -Uri https://developer.download.nvidia.com/compute/cuda/12.8.0/network_installers/cuda_12.8.0_windows_network.exe -OutFile cuda_installer.exe
      Write-Host "Starting installer..."
      Start-Process -Wait -FilePath cuda_installer.exe -ArgumentList '-s'
      Remove-Item cuda_installer.exe
    }
    catch {
        Write-Host "Error occurred: $_"
        exit 1
    }
  }else{
    Write-Host "CUDA disabled..."
  }

  #
  # Set ENV variables
  #
  $env:CUDA_PATH = "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.8"
  $env:PATH = "$env:CUDA_PATH\bin;$env:CUDA_PATH\libnvvp;" + $env:PATH


  #
  # Install CMake, Git, Python, Conan
  #
  choco install -y cmake
  python -m ensurepip --default-pip
  python -m pip install --upgrade pip
  python -m pip install conan numpy
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
    -G "Visual Studio 17 2022" `
    -DCMAKE_SH=CMAKE_SH-NOTFOUND `
    -DTAG_NAME="$TAG_NAME" `
    -DWITH_CUDA="$WITH_CUDA" `
    -DCMAKE_BUILD_TYPE="Release" `
    -DCMAKE_TOOLCHAIN_FILE="build/generators/conan_toolchain.cmake" `
    -DCUDA_TOOLKIT_ROOT_DIR="C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.8" `
    -DCMAKE_CUDA_COMPILER="C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.8/bin/nvcc.exe"
  cmake --build . --config Release --target imagec --parallel 8

  cd ..
}

#
#
#
function Pack {
  $WORKING_DIR = (Get-Location).Path
  $conanLibInstallPath = "C:\Users\runneradmin\.conan2\p"

  Set-Location -Path "build\build"
  ni "output" -ItemType Directory
  Set-Location -Path "output"
  ni "plugins" -ItemType Directory
  ni "models" -ItemType Directory
  ni "java" -ItemType Directory

  strip "$WORKING_DIR\build\build\Release\imagec.exe"
  Copy-Item -Path "$WORKING_DIR\build\build\Release\imagec.exe" -Destination "imagec.exe"

  #
  # Copy qt files
  #
  $qtDlls = @(
      "Qt6Core.dll",
      "Qt6Gui.dll",
      "Qt6Widgets.dll",
      "Qt6Svg.dll"
  )

  foreach ($dll in $qtPackages) {
    Get-ChildItem -Path "$conanLibInstallPath\*" | Where-Object {$_.Name -like "qt*"} | ForEach-Object {
      $pathToCopyFrom = $conanLibInstallPath + "\" + $_.Name + "\p\bin\$dll"
      if (Test-Path "$pathToCopyFrom") {
        Move-Item -Path $pathToCopyFrom -Destination "."  -Force
      }
    }
  }

  Get-ChildItem -Path "$conanLibInstallPath\*" | Where-Object {$_.Name -like "qt*"} | ForEach-Object {
    $pathToCopyFrom = $conanLibInstallPath + "\" + $_.Name + "\p\.\plugins\*"
    if (Test-Path "$pathToCopyFrom") {
      Move-Item -Recurse -Path $pathToCopyFrom -Destination "./plugins"  -Force
    }
  }


  #
  # Copy torch/cuda files
  #
  $dllsToCopy = @(
      "torch_cpu.dll",
      "torch.dll",
      "c10.dll"
  )

  if ($env:WITH_CUDA -eq 'True') {
    $dllsToCopy += @(
      "torch_cuda.dll", 
      "c10_cuda.dll",
      "cudart-218eec4c.dll",
      "cublas-f6c022dc.dll",
      "cublasLt-4ef47ce6.dll",
      "cudnn.dll",
      "cudnn_graph.dll",
      "cudnn_heuristic.dll ",
      "cudnn_engines_runtime_compiled.dll .",
      "cudnn_cnn.dll",
      "cudnn_engines_precompiled.dll"
      )
  } 

  foreach ($dll in $dllsToCopy) {
      Get-ChildItem -Path "$conanLibInstallPath\*" | Where-Object { $_.Name -like "libtoc*" } | ForEach-Object {
          $pathToCopyFrom = Join-Path $_.FullName "p\lib\$dll"
          if (Test-Path $pathToCopyFrom) {
              Move-Item -Path $pathToCopyFrom -Destination "." -Force
          }
      }
  }
          
  #
  # Copy system dlls
  #
  Copy-Item -Path "C:\Windows\System32\vcruntime140.dll" -Destination "."  -Force
  Copy-Item -Path "C:\Windows\System32\vcruntime140_1.dll" -Destination "."  -Force
  Copy-Item -Path "C:\Windows\System32\msvcp140.dll" -Destination "."  -Force
  Copy-Item -Path "C:\Windows\System32\msvcp140_1.dll" -Destination "."  -Force
  Copy-Item -Path "C:\Windows\System32\msvcp140_2.dll" -Destination "."  -Force
  Copy-Item -Path "C:\Windows\System32\concrt140.dll" -Destination "."  -Force
  Copy-Item -Path "C:\Windows\System32\msvcp140_codecvt_ids.dll" -Destination "."  -Force
  Copy-Item -Path "C:\Windows\System32\vcomp140.dll" -Destination "."  -Force

  Move-Item -Recurse -Path "$WORKING_DIR/resources/templates" ./templates
  cd java
  Move-Item -Recurse -Path "$WORKING_DIR/resources/java/bioformats.jar" -Destination "."
  Move-Item -Recurse -Path "$WORKING_DIR/resources/java/BioFormatsWrapper.class" -Destination "."
  Move-Item -Recurse -Path "$WORKING_DIR/resources/java/jre_win.zip" -Destination "."
  Expand-Archive "jre_win.zip" -DestinationPath "."
  Remove-Item jre_win.zip
  cd ..
}

#
# Bring cl.exe to path
# C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe
#
cmd.exe /c "call `"C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat`" && set > %temp%\vcvars.txt"
Get-Content "$env:temp\vcvars.txt" | Foreach-Object {
  if ($_ -match "^(.*?)=(.*)$") {
    Set-Content "env:\$($matches[1])" $matches[2]
  }
}

Write-Output $env:PATH

Install-Dependencies
Fetch-ExternalLibs
Build
Pack
