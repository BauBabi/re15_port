#!/usr/bin/env python3
"""
Build script for RE1.5 RE2-faithful port (Phase 3, 2026-05-18).

Compiles multi-file C with shared headers via PSn00bSDK gcc-mipsel.

Output: pb_re2_port.bin (raw bytes for embedding into PATCH.BIN at 0x801F0E00).

Source layout:
  re2_port/
    *.h         (re15_addrs, bios, libsnd, psx_types — shared headers)
    pb_save.c   (save FSM replacement — Phase 3.2)
    pb_*.c      (future subsystems)

Output sections (linker-placed at fixed RAM addresses):
  0x801F0E00  pb_save_entry  — FUN_80046540 state-2 hook target
  0x801F0E??  pb_save_main   — Phase 3.2 save flow
  ...

Currently builds with just pb_save.c (Phase 3.1 stub). As Phase 3.2+ adds
more .c files, append them to the SOURCES list below.
"""

import os
import subprocess
import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).parent
SDK_BIN = Path(r"C:\PSn00bSDK\sdk\PSn00bSDK-0.24-win32\bin")
GCC     = SDK_BIN / "mipsel-none-elf-gcc.exe"
LD      = SDK_BIN / "mipsel-none-elf-ld.exe"
OBJCOPY = SDK_BIN / "mipsel-none-elf-objcopy.exe"
OBJDUMP = SDK_BIN / "mipsel-none-elf-objdump.exe"

# Source files to compile (add to this list as Phase 3.2+ subsystems grow)
SOURCES = [
    "pb_save.c",
]

CFLAGS = [
    "-c",
    "-mno-abicalls",
    "-fno-pic",
    "-G0",
    "-O2",
    "-nostdlib",
    "-ffreestanding",
    "-mips1",
    "-EL",
    "-mabi=32",
    "-fno-builtin",
    "-fno-stack-protector",
    "-Wall",
    "-Wno-unused-function",
    "-I.",
]

LDFLAGS = [
    "-T", str(SCRIPT_DIR / "pb_re2_port.ld"),
    "--no-warn-rwx-segments",
    "-static",
]


def run(cmd, cwd=None):
    print(f"  > {' '.join(str(c) for c in cmd)}")
    r = subprocess.run([str(c) for c in cmd], cwd=cwd, capture_output=True, text=True)
    if r.returncode != 0:
        print(f"  STDOUT: {r.stdout}")
        print(f"  STDERR: {r.stderr}")
        sys.exit(1)
    if r.stderr.strip():
        print(f"  (stderr) {r.stderr.strip()}")
    return r


def main():
    os.chdir(SCRIPT_DIR)
    objs = []

    print(f"[1/{len(SOURCES)+3}] Compile sources -> .o")
    for src in SOURCES:
        obj = src.replace(".c", ".o")
        objs.append(obj)
        run([GCC] + CFLAGS + ["-o", obj, src])

    print(f"\n[{len(SOURCES)+1}/{len(SOURCES)+3}] Link -> pb_re2_port.elf")
    run([LD] + LDFLAGS + ["-o", "pb_re2_port.elf"] + objs)

    print(f"\n[{len(SOURCES)+2}/{len(SOURCES)+3}] Extract .text -> pb_re2_port.bin")
    run([OBJCOPY, "-O", "binary", "-j", ".text", "pb_re2_port.elf", "pb_re2_port.bin"])

    bin_size = (SCRIPT_DIR / "pb_re2_port.bin").stat().st_size
    print(f"\n  pb_re2_port.bin: {bin_size} bytes (budget: 512B at 0x801F0E00)")
    if bin_size > 512:
        print(f"  ERROR: exceeds 512B budget!")
        sys.exit(1)

    print(f"\n[{len(SOURCES)+3}/{len(SOURCES)+3}] Disasm pb_re2_port.elf")
    r = subprocess.run([str(OBJDUMP), "-d", "pb_re2_port.elf"], capture_output=True, text=True)
    for line in r.stdout.splitlines():
        if line.strip():
            print(f"  {line}")

    print("\nSUCCESS! pb_re2_port.bin ready to embed in PATCH.BIN at offset 0xE00.")


if __name__ == "__main__":
    sys.exit(main())
