#!/usr/bin/env python3
"""
flash.py
--------
Flashes firmware using OpenOCD based on build/CMakeCache.txt:
- If TARGET_GROUP = production: normal flash (resets target and exits).
- If TARGET_GROUP = test: flash + semihosting (enables semihosting and resumes).
"""

import argparse
import re
import subprocess
import sys
from pathlib import Path


def get_target_group_from_cache(cache_file: Path) -> str:
    """Read the active TARGET_GROUP from build/CMakeCache.txt."""
    if not cache_file.exists():
        print(f"Error: {cache_file} not found.")
        print("Please configure and build the project with CMake first.")
        sys.exit(1)

    content = cache_file.read_text(encoding="utf-8", errors="ignore")
    match = re.search(r"^TARGET_GROUP:STRING=(production|test)$", content, re.MULTILINE)
    if match:
        return match.group(1)

    print(f"Warning: TARGET_GROUP not found in {cache_file}. Defaulting to 'production'.")
    return "production"


def find_firmware_hex(build_dir: Path, target_group: str) -> Path:
    """Find the main.hex firmware binary to flash."""
    candidates = [
        build_dir / "main.hex",
        build_dir / "tests" / "main.hex",
    ]
    for candidate in candidates:
        if candidate.exists():
            return candidate

    print(f"Error: Firmware binary not found in {build_dir}.")
    print("Please run 'cmake --build build' first.")
    sys.exit(1)


def main():
    workspace_dir = Path(__file__).resolve().parent.parent

    parser = argparse.ArgumentParser(description="Flash STM32 firmware using OpenOCD.")
    _ = parser.add_argument(
        "-b", "--build-dir",
        default="build",
        help="CMake build directory. Default: build"
    )
    _ = parser.add_argument(
        "-c", "--config",
        default="board/st_nucleo_f4.cfg",
        help="OpenOCD board configuration file. Default: board/st_nucleo_f4.cfg"
    )

    args = parser.parse_args()

    build_dir = (workspace_dir / args.build_dir).resolve()
    cache_file = build_dir / "CMakeCache.txt"

    target_group = get_target_group_from_cache(cache_file)
    print(f"[CMakeCache] Detected TARGET_GROUP='{target_group}'")

    hex_file = find_firmware_hex(build_dir, target_group)

    local_cfg = workspace_dir / args.config
    cfg_arg = str(local_cfg.resolve()) if local_cfg.exists() else args.config

    print(f"Flashing {target_group} firmware: {hex_file}")

    if target_group == "production":
        cmd = [
            "openocd",
            "-f", cfg_arg,
            "-c", f"program {hex_file.as_posix()} verify reset exit"
        ]
    else:
        cmd = [
            "openocd",
            "-f", cfg_arg,
            "-c", f"program {hex_file.as_posix()} verify reset; arm semihosting enable; resume"
        ]

    try:
        res = subprocess.run(cmd, check=False)
        sys.exit(res.returncode)
    except KeyboardInterrupt:
        print("\nFlashing/running interrupted by user.")
        sys.exit(130)
    except FileNotFoundError:
        print("Error: 'openocd' command not found. Ensure OpenOCD is installed and in your PATH.")
        sys.exit(1)


if __name__ == "__main__":
    main()
