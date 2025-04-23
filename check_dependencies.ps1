$exePath = "$PSScriptRoot\bin\Debug\debug_ui.exe"
$dlls = @(
    "SDL2.dll",
    "SDL2_ttf.dll", 
    "libprotobuf.dll",
    "zlib1.dll",
    "brotlicommon.dll",
    "brotlidec.dll",
    "bz2.dll",
    "freetype.dll",
    "libpng16.dll",
    "brotlienc.dll",
    "harfbuzz.dll"
)

Write-Host "Checking for required DLLs in bin\Debug directory..."
foreach ($dll in $dlls) {
    $dllPath = "$PSScriptRoot\bin\Debug\$dll"
    if (Test-Path $dllPath) {
        Write-Host "[OK] Found $dll"
    } else {
        Write-Host "[MISSING] $dll not found in bin\Debug"
    }
}

Write-Host "`nAttempting to run application..."
try {
    $process = Start-Process -FilePath $exePath -WorkingDirectory "$PSScriptRoot\bin\Debug" -PassThru
    Start-Sleep -Seconds 2
    if (-not $process.HasExited) {
        Write-Host "Application is running!"
    } else {
        Write-Host "Application exited with code: $($process.ExitCode)"
    }
}
catch {
    Write-Host "Error launching application: $_"
}