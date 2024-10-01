conan download "onnx/1.16.2" -r conancenter --only-recipe

$onnxPatchPath = "C:\Users\runneradmin\.conan2\p\onnx305a97e88c569\s"
$conanmetaPath = "C:\Users\runneradmin\.conan2\p\onnx305a97e88c569\e\conandata.yml"
$onnxSource = "C:\Users\runneradmin\.conan2\p\onnx305a97e88c569\s\src.tar.gz"
$unpackesFolderName = "C:\Users\runneradmin\.conan2\p\onnx305a97e88c569\s\onnx-1.16.2"

Invoke-WebRequest -Uri "https://github.com/onnx/onnx/archive/v1.16.2.tar.gz" -OutFile $onnxSource
tar -xvzf $onnxSource -C $onnxPatchPath
Rename-Item $unpackesFolderName "src"

if (-not (Test-Path -Path $onnxPatchPath)) {
    New-Item -Path $onnxPatchPath -ItemType Directory
    Write-Output "Folder Created Successfully!"
} else {
    Write-Output "Folder already exists!"
}

Copy-Item -Path conandata.yml -Destination $conanmetaPath -Force
Copy-Item -Recurse -Path patches -Destination $onnxPatchPath -Force
