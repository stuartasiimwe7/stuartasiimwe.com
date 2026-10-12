"""Capture actual serial readings; never synthesize missing measurements."""
import argparse
import csv
from datetime import datetime, timezone
import serial


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("port")
    parser.add_argument("output")
    args = parser.parse_args()
    # Exclusive creation prevents overwriting previous evidence.
    with open(args.output, "x", newline="") as output, serial.Serial(
        args.port, 115200, timeout=2
    ) as device:
        writer = csv.writer(output)
        writer.writerow(["host_utc", "uptime_ms", "temperature_c", "relative_humidity_pct", "status"])
        try:
            while True:
                line = device.readline().decode("utf-8", errors="replace").strip()
                if not line or line.startswith("#") or line.startswith("uptime_ms,"):
                    continue
                fields = next(csv.reader([line]))
                if len(fields) != 4 or not fields[0].isdigit():
                    print("Ignored serial diagnostic:", line)
                    continue
                writer.writerow([datetime.now(timezone.utc).isoformat(), *fields])
                output.flush()
        except KeyboardInterrupt:
            pass


if __name__ == "__main__":
    main()
