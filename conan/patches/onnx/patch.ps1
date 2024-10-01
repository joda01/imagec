conan download "onnx/1.16.2" -r conancenter --only-recipe

$onnxPatchPath = "C:\Users\runneradmin\.conan2\p\onnx305a97e88c569\s"
$conanmetaPath = "C:\Users\runneradmin\.conan2\p\onnx305a97e88c569\e\conandata.yml"

if (-not (Test-Path -Path $onnxPatchPath)) {
    New-Item -Path $onnxPatchPath -ItemType Directory
    Write-Output "Folder Created Successfully!"
} else {
    Write-Output "Folder already exists!"
}

Copy-Item -Path conandata.yml -Destination $conanmetaPath -Force
Copy-Item -Recurse -Path patches -Destination $onnxPatchPath -Force
