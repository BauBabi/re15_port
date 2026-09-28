#!/usr/bin/env python3
"""r30_karte3010_hint_zensus.py - Zensus des RE2-SCD-Opcodes 0x84 (Karten-Hinweis).

Laeuft opcode-exakt (tools/re2_sicherung/re2_scd_walk.py, Laengen aus den 143
Handlern ab 0x800A74C8) ueber ALLE SCD-Bloecke aller RE2-Leon-RDTs, direkt aus
den RDT-Bytes (Kopfworte +0x48 = Init-SCD, +0x4C = Exec-SCD), und meldet jeden
Record 0x84 mit RDT-Datei-Offset, Operand, den 12 Records davor und dem
Raum-EDT-Satz 0x2B (der Blinkton 0x022B0000 @0x8006F234).
"""
import struct, sys, os, glob
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "tools", "re2_sicherung"))
import re2_scd_walk as W

# Berichtigung gegenueber tools/re2_sicherung/re2_scd_lens.py: der automatische Scan
# liefert fuer Switch (0x13) die Laenge 2, weil er den 'addiu a3,a3,2' des
# Default-/Eswitch-Zweigs (@0x800540B0 / @0x800540E0) trifft. Der Handler @0x80054020
# liest aber  lhu t0,2(a3) @0x80054038  (u16 Blocklaenge bei +2),
#             lbu a2,1(a3) @0x8005403C  (Variablen-Index bei +1)  und schiebt
#             addiu a3,a3,4 @0x80054040  -> der Switch-Record ist 4 Byte.
# Mit Laenge 2 wird das Laengenwort als Opcode gelesen ('13 1a 84 00' -> falscher
# Treffer 0x84).
W.LENS[0x13] = 4

def blocks(d, base):
    """SCD-Block = u16-Offsettabelle; erster Eintrag = Tabellenlaenge."""
    if base == 0: return []
    first = struct.unpack_from("<H", d, base)[0]
    n = first // 2
    offs = [struct.unpack_from("<H", d, base + i * 2)[0] for i in range(n)]
    return [(i, base + o) for i, o in enumerate(offs)]

def main():
    rdts = sorted(glob.glob(os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM*.RDT")))
    total = 0; desync = 0; nblk = 0
    for p in rdts:
        d = open(p, "rb").read()
        o = struct.unpack_from("<23I", d, 8)
        edt = d[o[0]:o[1]] if o[0] else b""
        ends = sorted(set(x for x in o if x) | {len(d)})
        for kind, base in (("main", o[16]), ("sub", o[17])):
            bl = blocks(d, base)
            if not bl: continue
            # Blockende = naechster Blockanfang bzw. naechste RDT-Sektion
            starts = sorted(set(a for _, a in bl))
            sec_end = min(e for e in ends if e > base)
            for idx, a in bl:
                nxt = [s for s in starts if s > a]
                end = nxt[0] if nxt else sec_end
                recs, st = W.walk(d[a:end])
                nblk += 1
                if st != "ok": desync += 1
                for k, (off, op, r) in enumerate(recs):
                    if op == 0x84:
                        total += 1
                        e2b = edt[0x2B * 4:0x2B * 4 + 4].hex(" ") if len(edt) >= 0x2C * 4 else "-"
                        print("%s %s%02d +0x%04X @Datei 0x%05X  %s  hint=%d  EDT[0x2B]=%s  (%s)" % (
                            os.path.basename(p), kind, idx, off, a + off, r.hex(" "), r[1], e2b, st))
                        if "-v" in sys.argv:
                            for (o2, op2, r2) in recs[max(0, k - 12):k + 2]:
                                print("      +0x%04X %-16s %s" % (o2, W.NAMES.get(op2, "?"), r2.hex(" ")))
    print("--- %d RDTs, %d SCD-Bloecke, %d desynchron, %d Records 0x84 ---" % (len(rdts), nblk, desync, total))

if __name__ == "__main__":
    main()
