#!/usr/bin/env bash
#
# Loads real WiFi/edge-gateway credentials from .env and runs PlatformIO for
# the esp32_hw target. Copy .env.example to .env and fill in real values
# before running this script; .env itself is git-ignored.
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

# Run from the project root so PlatformIO finds platformio.ini regardless of the
# directory the script was invoked from.
cd "$PROJECT_ROOT"
pio run -e esp32_hw "$@"
