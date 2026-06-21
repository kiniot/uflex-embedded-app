#!/usr/bin/env bash
#
# Loads real WiFi/edge-gateway credentials from .env and flashes the esp32_hw
# target onto a connected ESP32. Copy .env.example to .env and fill in real
# values before running this script; .env itself is git-ignored.
#
# Extra arguments are forwarded to PlatformIO, so you can pin the port or chain
# the serial monitor, for example:
#   ./scripts/upload_hw.sh --upload-port /dev/cu.usbserial-0001
#   ./scripts/upload_hw.sh -t upload -t monitor
#
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ENV_FILE="$PROJECT_ROOT/.env"

if [[ ! -f "$ENV_FILE" ]]; then
    echo "error: $ENV_FILE not found. Copy .env.example to .env and fill in real values." >&2
    exit 1
fi

set -a
# shellcheck disable=SC1090
source "$ENV_FILE"
set +a

pio run -e esp32_hw -t upload "$@"
