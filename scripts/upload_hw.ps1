# Loads real WiFi/edge-gateway credentials from .env and flashes the esp32_hw
# target onto a connected ESP32. Copy .env.example to .env and fill in real
# values before running this script; .env itself is git-ignored.
#
# Extra arguments are forwarded to PlatformIO, so you can pin the port or chain
# the serial monitor, for example:
#   .\scripts\upload_hw.ps1 --upload-port COM5
#   .\scripts\upload_hw.ps1 -t upload -t monitor

$ErrorActionPreference = "Stop"

$envFile = Join-Path $PSScriptRoot "..\.env"

if (-not (Test-Path $envFile)) {
    Write-Error "$envFile not found. Copy .env.example to .env and fill in real values."
    exit 1
}

Get-Content $envFile | ForEach-Object {
    $line = $_.Trim()
    if ($line -eq "" -or $line.StartsWith("#")) {
        return
    }
    if ($line -match '^\s*([^#=]+?)\s*=\s*(.*)$') {
        [System.Environment]::SetEnvironmentVariable($Matches[1], $Matches[2], "Process")
    }
}

pio run -e esp32_hw -t upload @args
