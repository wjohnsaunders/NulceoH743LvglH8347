#!/usr/bin/env python3
"""Configure and build the Nucleo-H743ZI project via CMake presets.

The project's CMakePresets.json already wires up the Ninja generator and the
arm-none-eabi-gcc toolchain (cmake/gcc-arm-none-eabi.cmake), so this script
just dispatches to the matching preset.

Usage:
    python cbuild.py --config            # configure (Debug preset)
    python cbuild.py --config --release  # configure (Release preset)
    python cbuild.py                     # build (Debug preset)
    python cbuild.py --release           # build (Release preset)
    python cbuild.py --srec              # build the SREC (S19) file
"""

import argparse
import subprocess
import sys


def run(cmd):
    """Run a command, echoing it first. Exits with the command's status on failure."""
    print(f"$ {' '.join(cmd)}")
    result = subprocess.run(cmd)
    if result.returncode != 0:
        sys.exit(result.returncode)


def main():
    parser = argparse.ArgumentParser(
        description="Configure or build the project using CMake + Ninja + arm-none-eabi-gcc."
    )

    preset_group = parser.add_mutually_exclusive_group()
    preset_group.add_argument("--config", action="store_true", help="Run the CMake configure step. If not set, run the build step.")
    preset_group.add_argument("--flash", action="store_true", help="FLASH the build to the target")
    preset_group.add_argument("--srec", action="store_true", help="Build the SREC (S19) file from the ELF")
    parser.add_argument("--release", action="store_true", help="Use the Release preset")
    args = parser.parse_args()

    preset = "Release" if args.release else "Debug"

    if args.config:
        run(["cmake", "--preset", preset])
    elif args.flash:
        run(["cmake", "--build", "--preset", preset, "--target", "flash"])
    elif args.srec:
        run(["cmake", "--build", "--preset", preset, "--target", "srec"])
    else:
        run(["cmake", "--build", "--preset", preset])


if __name__ == "__main__":
    main()

