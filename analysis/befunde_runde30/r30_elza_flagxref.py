#!/usr/bin/env python3
"""Runde 30 / Thema G: Wer fasst die Flag-Bank 3 (0x800b0ff8..0x800b1017) DIREKT an?

Die SCD-Opcodes Ck/Set gehen ueber die Zeigertabelle 0x80074664[zone] (indirekt) und
tauchen hier deshalb NICHT auf - gesucht sind fest verdrahtete EXE-/Overlay-Zugriffe,
also genau die Frage "setzt die EXE vor dem ersten Raum ein Story-Flag?".

⛔ Der erste Wurf meldete 29 Scheintreffer (`lui v0,0x800b / lw v0,..(v0) / ori v0,v0,0x1000`
= ein Bit auf einem GELADENEN Wert, keine Adresse). Die Spur wird jetzt abgebrochen, sobald
eine andere Instruktion das Basisregister ueberschreibt.

Methode: jede Load/Store/addiu-Instruktion mit Immediate im Fenster und Basisregister R,
zu der in den 12 Instruktionen davor ein `lui R,<hi>` steht. Durchsucht PSX.EXE
(Text @Datei 0x800, t_addr aus dem Kopf) und alle Overlays unter PSX/BIN (roh,
0x80100000; DEBUG.BIN 0x800c0000).

Aufruf: r30_elza_flagxref.py <lo-hex> <hi-hex>      z.B. 800b0ff8 800b1017
"""
import struct, sys, os, glob

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..")
R = ['zero','at','v0','v1','a0','a1','a2','a3','t0','t1','t2','t3','t4','t5','t6','t7',
     's0','s1','s2','s3','s4','s5','s6','s7','t8','t9','k0','k1','gp','sp','s8','ra']
OPN = {0x09:"addiu",0x20:"lb",0x21:"lh",0x23:"lw",0x24:"lbu",0x25:"lhu",
       0x28:"sb",0x29:"sh",0x2B:"sw",0x0D:"ori"}

def scan(name, data, base, skip, lo, hi):
    n = 0
    words = struct.unpack_from("<%dI" % ((len(data) - skip) // 4), data, skip)
    for i, w in enumerate(words):
        op = w >> 26
        if op not in OPN: continue
        rs = (w >> 21) & 31; rt = (w >> 16) & 31
        imm = w & 0xFFFF
        simm = imm - 0x10000 if imm & 0x8000 else imm
        for k in range(i - 1, max(-1, i - 13), -1):
            v = words[k]
            vop = v >> 26
            # schreibt eine ANDERE Instruktion das Basisregister, ist die lui-Spur tot
            if vop == 0 and ((v >> 11) & 31) == rs and (v & 0x3F) not in (0x08, 0x09):
                break
            if vop in (0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x20,0x21,0x23,0x24,0x25)                and ((v >> 16) & 31) == rs:
                break
            if vop == 0x0F and ((v >> 16) & 31) == rs:
                hiw = v & 0xFFFF
                addr = ((hiw << 16) + (imm if op == 0x0D else simm)) & 0xFFFFFFFF
                if lo <= addr <= hi:
                    print("%-10s %08x: %-5s %s,%d(%s)   -> 0x%08x" %
                          (name, base + i * 4, OPN[op], R[rt], simm, R[rs], addr))
                    n += 1
                break
    return n

def main():
    lo = int(sys.argv[1], 16); hi = int(sys.argv[2], 16)
    tot = 0
    exe = open(os.path.join(ROOT, "info/Re1.5/PSX.EXE"), "rb").read()
    t_addr = struct.unpack_from("<I", exe, 0x18)[0]
    tot += scan("PSX.EXE", exe, t_addr, 0x800, lo, hi)
    for p in sorted(glob.glob(os.path.join(ROOT, "info/Re1.5/PSX/BIN/*.BIN"))):
        nm = os.path.basename(p)
        d = open(p, "rb").read()
        base = 0x800c0000 if nm.upper() == "DEBUG.BIN" else 0x80100000
        tot += scan(nm, d, base, 0, lo, hi)
    print("SUMME Treffer: %d  (Fenster 0x%08x..0x%08x)" % (tot, lo, hi))

main()
