$MSBuildPath = "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
$SolutionPath = "$PSScriptRoot\FidgetReactor.sln"

Write-Host "MSBuild Path: $MSBuildPath"
Write-Host "Solution Path: $SolutionPath"

if (-Not (Test-Path $MSBuildPath)) {
    Write-Host "ERROR: MSBuild not found at specified path!"
    exit 1
}

if (-Not (Test-Path $SolutionPath)) {
    Write-Host "ERROR: Solution file not found!"
    exit 1
}

Write-Host "Building solution..."
& "$MSBuildPath" "$SolutionPath" /p:Configuration=Debug /p:Platform=x64 /verbosity:normal
$buildResult = $LASTEXITCODE

if ($buildResult -ne 0) {
    Write-Host "Build failed with exit code: $buildResult"
    exit $buildResult
}

Write-Host "Build completed successfully!"