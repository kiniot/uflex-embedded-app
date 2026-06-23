#!/usr/bin/env python3
#
# Created by Salim Ramirez Mestanza on 21/06/26.
#
"""Decode a uFlex BLE motion-telemetry frame into human-readable values.

The firmware notifies a fixed 53-byte little-endian frame over the BLE
telemetry characteristic (see
lib/uflex/include/uflex/infrastructure/transport/ble_motion_telemetry_serializer.h).
This tool takes that frame as a hex string (for example, copied from nRF
Connect or LightBlue) and prints the three relative joint rotations as
quaternions and Euler angles, plus the actuator state and sequence number.

Usage:
    python3 scripts/decode_ble_telemetry.py "21FB 533F 6C73 ... 0069 00"
    echo "21FB533F...006900" | python3 scripts/decode_ble_telemetry.py

The input may contain spaces, newlines, commas, or a leading "0x"; any
non-hex characters are ignored before parsing.
"""

import argparse
import math
import struct
import sys

WIRE_SIZE_BYTES = 53
# 3 quaternions (12 float32) + ledColor + buzzerActive + vibrationActive + uint16 sequence.
WIRE_FORMAT = "<12f BBB H"

# Mirrors RgbLed::Color in lib/uflex/include/uflex/domain/actuators/rgb_led.h.
LED_COLORS = ["off", "red", "green", "blue", "yellow", "cyan", "magenta"]


def parse_hex(text):
    """Return the raw bytes from a loose hex string, validating the length."""
    cleaned = "".join(ch for ch in text if ch in "0123456789abcdefABCDEF")
    if len(cleaned) % 2 != 0:
        raise ValueError(
            f"odd number of hex digits ({len(cleaned)}); a byte needs two digits"
        )
    data = bytes.fromhex(cleaned)
    if len(data) != WIRE_SIZE_BYTES:
        raise ValueError(
            f"expected {WIRE_SIZE_BYTES} bytes, got {len(data)} "
            f"({len(cleaned)} hex digits)"
        )
    return data


def quaternion_to_euler_degrees(w, x, y, z):
    """Convert a (w, x, y, z) quaternion to (roll, pitch, yaw) in degrees."""
    roll = math.atan2(2.0 * (w * x + y * z), 1.0 - 2.0 * (x * x + y * y))

    sin_pitch = 2.0 * (w * y - z * x)
    sin_pitch = max(-1.0, min(1.0, sin_pitch))  # clamp out of float drift before asin
    pitch = math.asin(sin_pitch)

    yaw = math.atan2(2.0 * (w * z + x * y), 1.0 - 2.0 * (y * y + z * z))

    return math.degrees(roll), math.degrees(pitch), math.degrees(yaw)


def decode(data):
    """Unpack a 53-byte frame into a structured dict."""
    fields = struct.unpack(WIRE_FORMAT, data)
    quaternions = {
        "upper-middle": fields[0:4],
        "middle-lower": fields[4:8],
        "upper-lower": fields[8:12],
    }
    led_color, buzzer_active, vibration_active, sequence_number = fields[12:16]
    return {
        "quaternions": quaternions,
        "led_color": led_color,
        "buzzer_active": bool(buzzer_active),
        "vibration_active": bool(vibration_active),
        "sequence_number": sequence_number,
    }


def render(decoded):
    """Build a printable report from a decoded frame."""
    lines = []
    lines.append("uFlex BLE motion telemetry")
    lines.append("-" * 64)
    lines.append(
        f"{'joint':<14}{'w':>8}{'x':>8}{'y':>8}{'z':>8}"
        f"{'roll°':>8}{'pitch°':>8}{'yaw°':>8}"
    )
    for joint, (w, x, y, z) in decoded["quaternions"].items():
        roll, pitch, yaw = quaternion_to_euler_degrees(w, x, y, z)
        magnitude = math.sqrt(w * w + x * x + y * y + z * z)
        lines.append(
            f"{joint:<14}{w:>8.3f}{x:>8.3f}{y:>8.3f}{z:>8.3f}"
            f"{roll:>8.1f}{pitch:>8.1f}{yaw:>8.1f}"
        )
        if abs(magnitude - 1.0) > 0.05:
            lines.append(
                f"{'':<14}warning: non-unit quaternion (|q|={magnitude:.3f})"
            )
    lines.append("-" * 64)

    color_index = decoded["led_color"]
    color_name = (
        LED_COLORS[color_index]
        if color_index < len(LED_COLORS)
        else f"unknown({color_index})"
    )
    lines.append(f"led color       : {color_name} ({color_index})")
    lines.append(f"buzzer active   : {decoded['buzzer_active']}")
    lines.append(f"vibration active: {decoded['vibration_active']}")
    lines.append(f"sequence number : {decoded['sequence_number']}")
    return "\n".join(lines)


def main(argv=None):
    parser = argparse.ArgumentParser(
        description="Decode a uFlex 53-byte BLE motion-telemetry hex frame."
    )
    parser.add_argument(
        "hex",
        nargs="?",
        help="The frame as a hex string; reads from stdin when omitted.",
    )
    args = parser.parse_args(argv)

    raw = args.hex if args.hex is not None else sys.stdin.read()
    if not raw or not raw.strip():
        parser.error("no hex input provided (pass an argument or pipe via stdin)")

    try:
        data = parse_hex(raw)
    except ValueError as error:
        print(f"error: {error}", file=sys.stderr)
        return 1

    print(render(decode(data)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
