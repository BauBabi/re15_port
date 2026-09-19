#!/usr/bin/env python3
"""re2_zombie_mask_scan.py - Vollscan des RE2-Zombie-Overlays EMOVL10_S0.BIN (Runde 16,
Thema trefferhoehe) nach allen Stellen, die

  * word0-Bits 26..28 (Teile-Maske Beine/Rumpf/Kopf) schreiben: `lui rX,0xc00` / `lui rX,0x400` /
    `lui rX,0xf3ff` mit dem folgenden `sw ...,0(rY)`;
  * die Zielradien +0x9A (154) / +0x9C (156) / +0x1EE (494) und die Box +0x98 (152) / +0x9E (158)
    per sh schreiben.

Ausgabe: Adresse, Instruktion, und (bei sh) der Wert aus dem vorangehenden addiu, falls
erkennbar. Alles direkt aus den Bytes, ohne Ghidra.

Aufruf: python re15_port/tools/re2_zombie_mask_scan.py [repo-root] [overlay]
"""
import struct, sys, os

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = sys.argv[1] if len(sys.argv) > 1 else os.path.abspath(os.path.join(HERE, "..", ".."))
OVL = sys.argv[2] if len(sys.argv) > 2 else "EMOVL10_S0.BIN"
data = open(os.path.join(REPO, "info", "re2leon", "COMMON", "BIN", OVL), "rb").read()
BASE = 0x80100000
REGS = ["zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7",
        "s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"]

def w(i): return struct.unpack_from("<I", data, i)[0]
n = len(data) // 4
last_addiu = {}   # reg -> (addr, imm)

print("== lui-Schreiber der Teile-Maske (0xc00 / 0x400 / 0xf3ff) ==")
for i in range(n):
    ins = w(i * 4); a = BASE + i * 4
    op = ins >> 26; rt = (ins >> 16) & 31; imm = ins & 0xffff
    if op == 0x0f and imm in (0x0c00, 0x0400, 0xf3ff):
        # naechsten sw ...,0(rY) innerhalb 12 Instruktionen suchen
        sw = ""
        for j in range(i + 1, min(n, i + 13)):
            x = w(j * 4)
            if (x >> 26) == 0x2b and (x & 0xffff) == 0:
                sw = " -> sw %s,0(%s) @%08x" % (REGS[(x >> 16) & 31], REGS[(x >> 21) & 31], BASE + j * 4)
                break
        print("  %08x: lui %s,0x%x%s" % (a, REGS[rt], imm, sw))

print("== sh-Schreiber auf +0x9A/+0x9C/+0x1EE/+0x98/+0x9E ==")
for i in range(n):
    ins = w(i * 4); a = BASE + i * 4
    op = ins >> 26; rs = (ins >> 21) & 31; rt = (ins >> 16) & 31
    imm = struct.unpack("<h", struct.pack("<H", ins & 0xffff))[0]
    if op == 0x09:  # addiu
        last_addiu[rt] = (a, imm)
    if op == 0x29 and imm in (154, 156, 494, 152, 158):  # sh
        val = ""
        if rt in last_addiu and a - last_addiu[rt][0] <= 32:
            val = "  (addiu %s,%d @%08x)" % (REGS[rt], last_addiu[rt][1], last_addiu[rt][0])
        print("  %08x: sh %s,%d(%s)%s" % (a, REGS[rt], imm, REGS[rs], val))
