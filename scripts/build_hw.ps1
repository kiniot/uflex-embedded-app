# Loads real WiFi/edge-gateway credentials from .env and runs PlatformIO for
# the esp32_hw target. Copy .env.example to .env and fill in real values
# before running this script; .env itself is git-ignored.

$ErrorActionPreference = "Stop"

$projectRoot = Join-Path $PSScriptRoot ".."
$envFile = Join-Path $projectRoot ".env"

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

# Run from the project root so PlatformIO finds platformio.ini regardless of the
# directory the script was invoked from.
Set-Location $projectRoot
pio run -e esp32_hw @args
