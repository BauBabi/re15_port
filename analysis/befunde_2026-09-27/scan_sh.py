#!/usr/bin/env python3
"""Vollscan: alle  sh rt,152(rs)  und  sh rt,158(rs)  in einem roh @0x80100000
geladenen RE2-Gegner-Overlay. Gibt je Treffer Kontext (+-6 Instruktionen) aus,
damit Wert und Basisregister (SELF vs. fremd) belegt werden koennen."""
import struct, sys, os

REGS = ["zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7",
        "s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"]
BASE = 0x80100000

def s16(v):
    return v - 0x10000 if v & 0x8000 else v

def dis(w, pc):
    op = w >> 26
    rs = (w >> 21) & 31; rt = (w >> 16) & 31; rd = (w >> 11) & 31
    sa = (w >> 6) & 31; fn = w & 63; imm = w & 0xFFFF
    R = REGS
    if w == 0: return "nop"
    if op == 0:
        if fn == 0x00: return f"sll {R[rd]},{R[rt]},{sa}"
        if fn == 0x02: return f"srl {R[rd]},{R[rt]},{sa}"
        if fn == 0x03: return f"sra {R[rd]},{R[rt]},{sa}"
        if fn == 0x04: return f"sllv {R[rd]},{R[rt]},{R[rs]}"
        if fn == 0x06: return f"srlv {R[rd]},{R[rt]},{R[rs]}"
        if fn == 0x07: return f"srav {R[rd]},{R[rt]},{R[rs]}"
        if fn == 0x08: return f"jr {R[rs]}"
        if fn == 0x09: return f"jalr {R[rd]},{R[rs]}"
        if fn == 0x10: return f"mfhi {R[rd]}"
        if fn == 0x12: return f"mflo {R[rd]}"
        if fn == 0x18: return f"mult {R[rs]},{R[rt]}"
        if fn == 0x19: return f"multu {R[rs]},{R[rt]}"
        if fn == 0x1a: return f"div {R[rs]},{R[rt]}"
        if fn == 0x1b: return f"divu {R[rs]},{R[rt]}"
        if fn == 0x20: return f"add {R[rd]},{R[rs]},{R[rt]}"
        if fn == 0x21: return f"addu {R[rd]},{R[rs]},{R[rt]}"
        if fn == 0x22: return f"sub {R[rd]},{R[rs]},{R[rt]}"
        if fn == 0x23: return f"subu {R[rd]},{R[rs]},{R[rt]}"
        if fn == 0x24: return f"and {R[rd]},{R[rs]},{R[rt]}"
        if fn == 0x25: return f"or {R[rd]},{R[rs]},{R[rt]}"
        if fn == 0x26: return f"xor {R[rd]},{R[rs]},{R[rt]}"
        if fn == 0x27: return f"nor {R[rd]},{R[rs]},{R[rt]}"
        if fn == 0x2a: return f"slt {R[rd]},{R[rs]},{R[rt]}"
        if fn == 0x2b: return f"sltu {R[rd]},{R[rs]},{R[rt]}"
        return f".word 0x{w:08x}"
    if op == 1:
        if rt == 0: return f"bltz {R[rs]},0x{pc+4+(s16(imm)<<2):08x}"
        if rt == 1: return f"bgez {R[rs]},0x{pc+4+(s16(imm)<<2):08x}"
        return f".word 0x{w:08x}"
    if op == 2: return f"j 0x{(pc & 0xF0000000) | ((w & 0x3FFFFFF) << 2):08x}"
    if op == 3: return f"jal 0x{(pc & 0xF0000000) | ((w & 0x3FFFFFF) << 2):08x}"
    if op == 4: return f"beq {R[rs]},{R[rt]},0x{pc+4+(s16(imm)<<2):08x}"
    if op == 5: return f"bne {R[rs]},{R[rt]},0x{pc+4+(s16(imm)<<2):08x}"
    if op == 6: return f"blez {R[rs]},0x{pc+4+(s16(imm)<<2):08x}"
    if op == 7: return f"bgtz {R[rs]},0x{pc+4+(s16(imm)<<2):08x}"
    if op == 8: return f"addi {R[rt]},{R[rs]},{s16(imm)}"
    if op == 9: return f"addiu {R[rt]},{R[rs]},{s16(imm)}"
    if op == 0x0a: return f"slti {R[rt]},{R[rs]},{s16(imm)}"
    if op == 0x0b: return f"sltiu {R[rt]},{R[rs]},{s16(imm)}"
    if op == 0x0c: return f"andi {R[rt]},{R[rs]},0x{imm:x}"
    if op == 0x0d: return f"ori {R[rt]},{R[rs]},0x{imm:x}"
    if op == 0x0e: return f"xori {R[rt]},{R[rs]},0x{imm:x}"
    if op == 0x0f: return f"lui {R[rt]},0x{imm:x}"
    if op == 0x12:
        if rs == 0: return f"mfc2 {R[rt]},cp2r{rd}"
        if rs == 2: return f"cfc2 {R[rt]},cp2c{rd}"
        if rs == 4: return f"mtc2 {R[rt]},cp2r{rd}"
        if rs == 6: return f"ctc2 {R[rt]},cp2c{rd}"
        return f"cop2 0x{w & 0x1FFFFFF:07x}"
    if op == 0x20: return f"lb {R[rt]},{s16(imm)}({R[rs]})"
    if op == 0x21: return f"lh {R[rt]},{s16(imm)}({R[rs]})"
    if op == 0x23: return f"lw {R[rt]},{s16(imm)}({R[rs]})"
    if op == 0x24: return f"lbu {R[rt]},{s16(imm)}({R[rs]})"
    if op == 0x25: return f"lhu {R[rt]},{s16(imm)}({R[rs]})"
    if op == 0x28: return f"sb {R[rt]},{s16(imm)}({R[rs]})"
    if op == 0x29: return f"sh {R[rt]},{s16(imm)}({R[rs]})"
    if op == 0x2b: return f"sw {R[rt]},{s16(imm)}({R[rs]})"
    if op == 0x32: return f"lwc2 cp2r{rt},{s16(imm)}({R[rs]})"
    if op == 0x3a: return f"swc2 cp2r{rt},{s16(imm)}({R[rs]})"
    return f".word 0x{w:08x}"

def main():
    path = sys.argv[1]
    targets = [int(x) for x in (sys.argv[2].split(",") if len(sys.argv) > 2 else ["152","158"])]
    ctx = int(sys.argv[3]) if len(sys.argv) > 3 else 6
    data = open(path, "rb").read()
    n = len(data) // 4
    words = struct.unpack("<%dI" % n, data[:n*4])
    hits = []
    for i, w in enumerate(words):
        if (w >> 26) == 0x29 and (w & 0xFFFF) in targets:
            hits.append(i)
    print(f"# {os.path.basename(path)}  size={len(data)} (0x{len(data):x})  "
          f"Treffer: {len(hits)}  fuer Offsets {targets}")
    for i in hits:
        a = BASE + i*4
        print(f"\n--- @0x{a:08x}   {dis(words[i], a)}")
        for j in range(max(0, i-ctx), min(n, i+ctx+1)):
            pc = BASE + j*4
            mark = ">>" if j == i else "  "
            print(f"  {mark} {pc:08x}: {words[j]:08x}  {dis(words[j], pc)}")

if __name__ == "__main__":
    main()
