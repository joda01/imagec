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
  # Install CMake, Git, Python, Conan
  #
  choco install -y cmake
  python -m ensurepip --default-pip
  python -m pip install --upgrade pip
  python -m pip install conan numpy


  #
  # Set ENV variables
  #

  # Bring cl.exe to path: C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe
  cmd.exe /c "call `"C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat`" && set > %temp%\vcvars.txt"
  Get-Content "$env:temp\vcvars.txt" | Foreach-Object {
    if ($_ -match "^(.*?)=(.*)$") {
      Set-Content "env:\$($matches[1])" $matches[2]
    }
  }

  $env:CUDA_PATH = "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.8"
  $env:CUDA_PATH_V12_8 = "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.8"
  $env:PATH = "$env:CUDA_PATH\bin;$env:CUDA_PATH\libnvvp;" + $env:PATH

  # Remove duplicates
  $env:PATH = ($env:PATH -split ';' | Select-Object -Unique) -join ';'
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


  Copy-Item $GITHUB_WORKSPACE\conan\profile_win "C:\Users\runneradmin\.conan2\profiles\default" -Force

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
  if ($WITH_CUDA -eq "True") {
    ls "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.8"
    ls "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Microsoft\VC\v170\BuildCustomizations"
  }

  Get-ChildItem Env:
  
  #
  # Call cmake
  #
  cmake .. `
    -G "Visual Studio 17 2022" `
    -DCMAKE_SH=CMAKE_SH-NOTFOUND `
    -DTAG_NAME="$TAG_NAME" `
    -DWITH_CUDA="$WITH_CUDA" `
    -DCMAKE_BUILD_TYPE="Release" `
    -DCMAKE_POLICY_DEFAULT_CMP0091=NEW `
    -DCMAKE_TOOLCHAIN_FILE="build/generators/conan_toolchain.cmake" `
    -DCUDA_TOOLKIT_ROOT_DIR="C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.8" `
    -DCUDAToolkit_ROOT="C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.8" `
    -DCMAKE_GENERATOR_TOOLSET="cuda=C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.8" `
    -DCMAKE_CUDA_COMPILER="C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.8/bin/nvcc.exe" `
    -DUSE_SYSTEM_NVTX:BOOL=ON
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
  $qtPackages = @(
      "Qt6Core.dll",
      "Qt6Gui.dll",
      "Qt6Widgets.dll",
      "Qt6Svg.dll"
  )

  foreach ($dll in $qtPackages) {
    Get-ChildItem -Path "$conanLibInstallPath\*" | Where-Object {$_.Name -like "qt*"} | ForEach-Object {
      $pathToCopyFrom = $conanLibInstallPath + "\" + $_.Name + "\p\bin\$dll"
      if (Test-Path "$pathToCopyFrom") {
        Copy-Item -Path $pathToCopyFrom -Destination "."  -Force
      }
    }
  }

  Get-ChildItem -Path "$conanLibInstallPath\*" | Where-Object {$_.Name -like "qt*"} | ForEach-Object {
    $pathToCopyFrom = $conanLibInstallPath + "\" + $_.Name + "\p\.\plugins\*"
    if (Test-Path "$pathToCopyFrom") {
      Copy-Item -Recurse -Path $pathToCopyFrom -Destination "./plugins"  -Force
    }
  }


  #
  # Copy torch/cuda files
  #
  $dllsToCopy = @(
      "torch_cpu.dll",
      "torch.dll",
      "c10.dll",
      "fbgemm.dll",
      "libiomp5md.dll",
      "uv.dll",
      "cupti64_2025.1.0.dll",
      "asmjit.dll"
  )

  if ($WITH_CUDA -eq "True") {
    $dllsToCopy += @(
      "torch_cuda.dll", 
      "c10_cuda.dll",
      "cudart64_12.dll",
      "cublas64_12.dll",
      "cublasLt64_12.dll",
      "cudnn64_9.dll",
      "cudnn_graph64_9.dll",
      "cudnn_heuristic64_9.dll ",
      "cudnn_engines_runtime_compiled64_9.dll",
      "cudnn_cnn64_9.dll",
      "cudnn_adv64_9.dll",
      "cudnn_ops64_9.dll",
      "cudnn_engines_precompiled64_9.dll",
      "cufft64_11.dll",
      "cusparse64_12.dll",
      "nvJitLink_120_0.dll",
      "cusolver64_11.dll"
      )
  } 

  foreach ($dll in $dllsToCopy) {
      Get-ChildItem -Path "$conanLibInstallPath\*" | Where-Object { $_.Name -like "libto*" } | ForEach-Object {
          $pathToCopyFrom = Join-Path $_.FullName "p\lib\$dll"
          if (Test-Path $pathToCopyFrom) {
              Copy-Item -Path $pathToCopyFrom -Destination "." -Force
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

  #
  # java
  #
  Copy-Item -Recurse -Path "$WORKING_DIR/resources/templates" ./templates
  cd java
  Copy-Item -Recurse -Path "$WORKING_DIR/resources/java/bioformats.jar" -Destination "."
  Copy-Item -Recurse -Path "$WORKING_DIR/resources/java/BioFormatsWrapper.class" -Destination "."
  Copy-Item -Recurse -Path "$WORKING_DIR/resources/java/jre_win.zip" -Destination "."
  Expand-Archive "jre_win.zip" -DestinationPath "."
  Remove-Item jre_win.zip
  cd ..
}


Install-Dependencies
Fetch-ExternalLibs
Build
Pack
