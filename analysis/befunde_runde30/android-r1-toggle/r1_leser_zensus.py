#!/usr/bin/env python3
"""Zensus: wer liest im ORIGINAL (RE1.5 Auslieferungsstand) das R1-Bit?

R1 kommt im Original auf zwei Wegen an:
  (a) VIRTUELL: Bits 8 und 10 (0x0100 / 0x0400) der Woerter 0x800ac768 (HELD) und
      0x800ac76c (FLANKE); Quelle = Preset-Tabelle @0x80073dbc, Eintraege [8] und [10]
      = 0x0008 = PADR1 (Datei-Offset 0x645cc / 0x645d0 in info/Re1.5/PSX.EXE).
  (b) ROH: Bit 0x0008 der Woerter 0x800ac758 (HELD roh), 0x800ac75c (FLANKE roh),
      0x800ac760 (HELD roh, Halbwort; sh @0x80030564) und 0x800ac762 (FLANKE roh,
      Halbwort; sh @0x800305a0).
Module: PSX.EXE @t_addr, DEBUG.BIN @0x800c0000 (Inventar/Datei-Schirm), alle uebrigen
BIN @0x80100000 (Stage-Overlays, TITLE.BIN).

Verfahren (wortweise, mit Registerverfolgung je Funktion):
  * lui/addiu/ori bauen Konstanten in Registern auf (Adress-Basis),
  * lw/lhu/lh/lbu von einer der fuenf Adressen markiert das Zielregister als
    "traegt Pad-Wort <art>", Kopien (addu/or mit zero) erben die Marke,
  * jedes andere Schreiben in ein Register loescht dessen Marke,
  * `andi x,R,maske` mit markiertem R und R1-Bit in der Maske = Treffer,
  * `jr ra` beendet die Funktion (Marken weg), jal loescht die Caller-saved-Register.
ABDECKUNG: das Verfahren sieht KEINE Leser, die das Wort ueber den Stack oder ueber
ein Funktionsargument weiterreichen. Ein leerer Bereich ist darum kein Negativbeleg.
"""
import struct, os

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
EXE = os.path.join(ROOT, "info", "Re1.5", "PSX.EXE")
BINDIR = os.path.join(ROOT, "info", "Re1.5", "PSX", "BIN")

PAD = {0x800ac768: "VIRT-HELD", 0x800ac76c: "VIRT-FLANKE",
       0x800ac758: "ROH-HELD", 0x800ac75c: "ROH-FLANKE", 0x800ac760: "ROH-HELD16",
       0x800ac762: "ROH-FLANKE16"}
CALLER_SAVED = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,24,25]

def scan(buf, base, off0):
    n = (len(buf) - off0) // 4
    w = struct.unpack_from("<%dI" % n, buf, off0)
    const = {}   # reg -> konstante
    mark = {}    # reg -> (art, ladeadresse)
    hits = []
    i = 0
    while i < n:
        ins = w[i]
        op = ins >> 26
        rs = (ins >> 21) & 31
        rt = (ins >> 16) & 31
        rd = (ins >> 11) & 31
        fn = ins & 0x3f
        imm = ins & 0xffff
        simm = imm - 0x10000 if imm & 0x8000 else imm
        pc = base + 4 * i
        if op == 0x0f:                                   # lui
            const[rt] = (imm << 16) & 0xffffffff; mark.pop(rt, None)
        elif op in (0x09, 0x08):                         # addiu/addi
            if rs in const: const[rt] = (const[rs] + simm) & 0xffffffff
            else: const.pop(rt, None)
            mark.pop(rt, None)
        elif op == 0x0d:                                 # ori
            if rs in const: const[rt] = const[rs] | imm
            else: const.pop(rt, None)
            mark.pop(rt, None)
        elif op in (0x23, 0x25, 0x21, 0x24, 0x20):       # lw lhu lh lbu lb
            art = None
            if rs in const:
                art = PAD.get((const[rs] + simm) & 0xffffffff)
            const.pop(rt, None)
            if art: mark[rt] = (art, pc)
            else: mark.pop(rt, None)
        elif op == 0x0c:                                 # andi
            if rs in mark:
                art, la = mark[rs]
                r1 = (imm & 0x0500) if art.startswith("VIRT") else (imm & 0x0008)
                if r1: hits.append((la, art, pc, imm))
            const.pop(rt, None)
            if rt != rs: mark.pop(rt, None)
            else: mark.pop(rt, None)
        elif op == 0x00:                                 # SPECIAL
            if fn == 0x08 and rs == 31:                  # jr ra
                const.clear(); mark.clear()
            elif fn in (0x21, 0x25, 0x20) and (rt == 0 or rs == 0):   # move
                src = rs if rt == 0 else rt
                if src in mark: mark[rd] = mark[src]
                else: mark.pop(rd, None)
                if src in const: const[rd] = const[src]
                else: const.pop(rd, None)
            else:
                mark.pop(rd, None); const.pop(rd, None)
        elif op == 0x03:                                 # jal: Delay-Slot laeuft noch
            pass
        elif op in (0x0a, 0x0b, 0x0e):                   # slti sltiu xori
            mark.pop(rt, None); const.pop(rt, None)
        # jal-Nachlauf: nach dem Delay-Slot sind caller-saved Register tot
        if i >= 2 and (w[i-1] >> 26) == 0x03:
            for r in CALLER_SAVED:
                mark.pop(r, None); const.pop(r, None)
        i += 1
    return hits

def main():
    exe = open(EXE, "rb").read()
    t_addr = struct.unpack_from("<I", exe, 0x18)[0]
    rows = [("PSX.EXE", h) for h in scan(exe, t_addr, 0x800)]
    for fn in sorted(os.listdir(BINDIR)):
        if not fn.upper().endswith(".BIN"): continue
        b = open(os.path.join(BINDIR, fn), "rb").read()
        lo = 0x800c0000 if fn.upper() == "DEBUG.BIN" else 0x80100000
        for h in scan(b, lo, 0): rows.append((fn, h))
    print("t_addr = 0x%08x, EXE %d B" % (t_addr, len(exe)))
    print("Datei        Lade-Adr    Wort         andi-Adr    Maske")
    for fn, (la, art, aa, m) in rows:
        print("%-12s 0x%08x  %-11s  0x%08x  0x%04x" % (fn, la, art, aa, m))
    print("Treffer: %d" % len(rows))

main()
