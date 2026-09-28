#!/usr/bin/env python3
"""r30_karte3010_memscan.py - Adress-Zugriffe mit linearer Konstantenverfolgung.

Ergaenzt r30_karte3010_xref.py (das nur 'lui rX / op off(rX)' findet) um Zugriffe
ueber ein BASISREGISTER, z.B.  lui s2,0x800d / addiu s2,s2,23536 ... lbu v0,3586(s2).

Verfahren: linearer Lauf ueber das Textsegment; je Register der zuletzt bekannte
Konstantwert (lui, addiu/ori auf bekanntem Wert, addu mit zero). Ein Register wird
unbekannt, sobald es anders beschrieben wird. Funktionsgrenze (jr ra + Delay-Slot)
loescht alles. Sprungziele werden NICHT verfolgt -> das Ergebnis ist eine
Kandidatenliste (kann Fehltreffer UND Luecken haben); jeder Treffer wird von Hand
am Disassemblat geprueft.

Aufruf: r30_karte3010_memscan.py <adresse> [<adresse> ...]  [--exe re2|re15|Pfad]
"""
import struct, sys, os
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
R = ["zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7",
     "s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"]
MEM = {0x20:"lb",0x21:"lh",0x23:"lw",0x24:"lbu",0x25:"lhu",0x28:"sb",0x29:"sh",0x2b:"sw",
       0x22:"lwl",0x26:"lwr",0x2a:"swl",0x2e:"swr"}
LOAD = (0x20,0x21,0x23,0x24,0x25,0x22,0x26)

def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    exe = "re2"
    if "--exe" in sys.argv: exe = sys.argv[sys.argv.index("--exe") + 1]; args.remove(exe)
    p = {"re2": os.path.join(REPO, "info", "re2leon", "PSX.EXE"),
         "re15": os.path.join(REPO, "info", "Re1.5", "PSX.EXE")}.get(exe, exe)
    d = open(p, "rb").read()
    base = struct.unpack_from("<I", d, 0x18)[0]
    n = (len(d) - 0x800) // 4
    W = struct.unpack_from("<%dI" % n, d, 0x800)
    targets = set(int(a, 0) for a in args)
    print("; %s  Ziele: %s" % (p, ", ".join("0x%08X" % t for t in sorted(targets))))
    reg = {}
    pending_clear = 0
    for i, w in enumerate(W):
        a = base + i * 4
        op = w >> 26; rs = (w >> 21) & 31; rt = (w >> 16) & 31; rd = (w >> 11) & 31; fn = w & 63
        imm = w & 0xFFFF; simm = imm - 0x10000 if imm & 0x8000 else imm
        if op in MEM:
            if rs in reg:
                ea = (reg[rs] + simm) & 0xFFFFFFFF
                if ea in targets:
                    print("  0x%08X  %-4s %s,%d(%s)   ; EA=0x%08X  (%s)" % (
                        a, MEM[op], R[rt], simm, R[rs], ea, "LESEN" if op in LOAD else "SCHREIBEN"))
            if op in LOAD: reg.pop(rt, None)
        elif op == 0x0F:
            reg[rt] = (imm << 16) & 0xFFFFFFFF
        elif op == 0x09:
            if rs in reg:
                v = (reg[rs] + simm) & 0xFFFFFFFF
                reg[rt] = v
                if v in targets:
                    print("  0x%08X  addiu %s,%s,%d   ; ZEIGER=0x%08X" % (a, R[rt], R[rs], simm, v))
            elif rs == 0: reg[rt] = simm & 0xFFFFFFFF
            else: reg.pop(rt, None)
        elif op == 0x0D:
            if rs in reg: reg[rt] = reg[rs] | imm
            elif rs == 0: reg[rt] = imm
            else: reg.pop(rt, None)
        elif op == 0 and fn == 0x21:
            if rt == 0 and rs in reg: reg[rd] = reg[rs]
            elif rs == 0 and rt in reg: reg[rd] = reg[rt]
            else: reg.pop(rd, None)
        elif op == 0:
            if fn == 8 and rs == 31: pending_clear = 2
            elif fn not in (8, 9, 0x18, 0x19, 0x1a, 0x1b, 0x0c, 0x0d): reg.pop(rd, None)
        elif op in (0x0a, 0x0b, 0x0c, 0x0e, 0x08):
            reg.pop(rt, None)
        elif op == 3:   # jal: Aufrufer-gesicherte Register verfallen NACH dem Delay-Slot
            pending_clear = -2
        if pending_clear > 0:
            pending_clear -= 1
            if pending_clear == 0: reg.clear()
        elif pending_clear < 0:
            pending_clear += 1
            if pending_clear == 0:
                for r in (1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,24,25): reg.pop(r, None)
        reg.pop(0, None)

if __name__ == "__main__":
    main()
