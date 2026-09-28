#!/usr/bin/env python3
"""Disassembliert ein Modul mit FREI gewaehlter Ladeadresse (z.B. DEBUG.BIN @0x800c0000),
ueber den Instruktionsdekoder des Skills re15-psx-disasm (dis_one).
Aufruf: python dis_modul.py <datei> <ladeadresse-hex> <adresse-hex> <n>"""
import struct, sys, os, importlib.util
HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
SK = os.path.join(ROOT, ".claude", "skills", "re15-psx-disasm", "scripts", "re15_disasm.py")
spec = importlib.util.spec_from_file_location("re15_disasm", SK)
mod = importlib.util.module_from_spec(spec)
_argv = sys.argv; sys.argv = [SK, "bytes", "0x80010000", "0"]
try:
    spec.loader.exec_module(mod)     # laeuft main() nur unter __main__
finally:
    sys.argv = _argv
path, base, addr, n = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16), int(sys.argv[4])
b = open(path, "rb").read()
print("; %s geladen @0x%08x (%d B)" % (path, base, len(b)))
for i in range(n):
    a = addr + 4 * i
    w = struct.unpack_from("<I", b, a - base)[0]
    s, _ = mod.dis_one(w, a)
    print("  %08x: %s" % (a, s))
