#!/usr/bin/env python3

import argparse
import json
import pathlib
import subprocess
import sys


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("map_file", type=pathlib.Path)
    parser.add_argument("firmware_file", type=pathlib.Path)
    parser.add_argument("--max-app-bytes", type=int, default=838_860)
    parser.add_argument("--max-dram-bytes", type=int, default=256_000)
    args = parser.parse_args()

    report = subprocess.run(
        [
            sys.executable,
            "-m",
            "esp_idf_size",
            "--format",
            "json2",
            str(args.map_file),
        ],
        check=True,
        capture_output=True,
        text=True,
    )
    size_data = json.loads(report.stdout)
    if size_data.get("version") != "1.2":
        raise SystemExit(f"Unsupported esp_idf_size JSON schema: {size_data.get('version')}")

    dram = next((region for region in size_data["layout"] if region["name"] == "DRAM"), None)
    if dram is None:
        raise SystemExit("ESP-IDF size report has no DRAM region")

    firmware_bytes = args.firmware_file.stat().st_size
    dram_bytes = dram["used"]
    print(f"Firmware image: {firmware_bytes} bytes (limit {args.max_app_bytes})")
    print(f"DRAM usage: {dram_bytes} bytes (limit {args.max_dram_bytes})")
    if firmware_bytes > args.max_app_bytes:
        raise SystemExit("Firmware exceeds the configured application image budget")
    if dram_bytes > args.max_dram_bytes:
        raise SystemExit("Firmware exceeds the configured DRAM budget")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
