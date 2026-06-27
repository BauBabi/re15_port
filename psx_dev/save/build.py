#!/usr/bin/env python3
"""
Build script for RE1.5 PATCH.BIN custom C modules.

Pipeline:
  1. mipsel-none-elf-gcc compile .c → .o (relocatable MIPS object)
  2. mipsel-none-elf-ld link via .ld → .elf (placed at fixed RAM addr)
  3. mipsel-none-elf-objcopy extract .text → .bin (raw MIPS bytes)
  4. Print disasm for inspection

Output: save_stub.bin (raw bytes to embed in PATCH.BIN by patch_save_final.py)
"""

import os
import subprocess
import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).parent
SDK_BIN = Path(r"C:\PSn00bSDK\sdk\PSn00bSDK-0.24-win32\bin")
GCC      = SDK_BIN / "mipsel-none-elf-gcc.exe"
LD       = SDK_BIN / "mipsel-none-elf-ld.exe"
OBJCOPY  = SDK_BIN / "mipsel-none-elf-objcopy.exe"
OBJDUMP  = SDK_BIN / "mipsel-none-elf-objdump.exe"

CFLAGS = [
    "-c",
    "-mno-abicalls",     # No PIC/abi-call setup — we're freestanding
    "-fno-pic",
    "-G0",               # No gp-relative addressing — small text
    "-O2",
    "-nostdlib",
    "-ffreestanding",
    "-mips1",            # PSX is MIPS R3000A = MIPS-I ISA
    "-EL",               # Little-endian
    "-mabi=32",
    "-fno-builtin",
    "-fno-stack-protector",
    "-Wall",
]

LDFLAGS = [
    "-T", str(SCRIPT_DIR / "save_stub.ld"),
    "--no-warn-rwx-segments",
]


def run(cmd, cwd=None):
    print(f"  > {' '.join(str(c) for c in cmd)}")
    r = subprocess.run([str(c) for c in cmd], cwd=cwd, capture_output=True, text=True)
    if r.returncode != 0:
        print(f"  STDOUT: {r.stdout}")
        print(f"  STDERR: {r.stderr}")
        sys.exit(1)
    if r.stdout.strip():
        print(f"  {r.stdout.strip()}")
    if r.stderr.strip():
        print(f"  (stderr) {r.stderr.strip()}")
    return r


def main():
    os.chdir(SCRIPT_DIR)
    print("[1/4] Compile save_stub.c -> save_stub.o")
    run([GCC] + CFLAGS + ["-o", "save_stub.o", "save_stub.c"])

    print("\n[2/4] Link save_stub.o -> save_stub.elf (at 0x801F0E00)")
    run([LD] + LDFLAGS + ["-o", "save_stub.elf", "save_stub.o"])

    print("\n[3/4] Extract .text -> save_stub.bin (raw MIPS bytes)")
    run([OBJCOPY, "-O", "binary", "-j", ".text", "save_stub.elf", "save_stub.bin"])

    bin_size = (SCRIPT_DIR / "save_stub.bin").stat().st_size
    print(f"\n  save_stub.bin: {bin_size} bytes")

    print("\n[4/4] Disasm save_stub.elf")
    r = subprocess.run([str(OBJDUMP), "-d", "save_stub.elf"], capture_output=True, text=True)
    for line in r.stdout.splitlines():
        if line.strip():
            print(f"  {line}")

    print("\nSUCCESS! save_stub.bin ready to embed in PATCH.BIN.")
    print(f"  Embed at file offset 0xE00 (RAM 0x801F0E00).")
    print(f"  Hook RE1.5 caller to `jal 0x801F0E00`.")


if __name__ == "__main__":
    sys.exit(main())
