#!/usr/bin/env python3
"""Gator-SE-Korrektur: Paar-Tabelle @0x800A7400 dumpen + EM23-Spawn-Zensus ueber alle RDTs."""
import struct, sys, os, glob
REPO = "c:/workspace/git/reAi_v2"
sys.path.insert(0, os.path.join(REPO, "analysis", "nutzer_batch_2026-08-27", "tools"))
from re2_scd_lens import TABLE   # RE2-SCD-Opcode-Laengen (aus den EXE-Handlern abgeleitet)

EXE = os.path.join(REPO, "info", "re2leon", "PSX.EXE")
D = open(EXE, "rb").read()
TADDR = struct.unpack_from("<I", D, 0x18)[0]
def fo(a): return 0x800 + (a - TADDR)

print("t_addr = 0x%08X  (Datei-Offset von 0x800A7400 = 0x%X)" % (TADDR, fo(0x800A7400)))
print("\n=== PAAR-TABELLE @0x800A7400 (Zeile = Bank; {sound_a, sound_b}; Ende bei a==0xFF) ===")
rows = []
a = fo(0x800A7400)
i = 0
while True:
    k0, k1 = D[a + i*2], D[a + i*2 + 1]
    if k0 == 0xFF:
        print("Zeile %2d: TERMINATOR {0x%02X,0x%02X} @Datei 0x%X" % (i, k0, k1, a + i*2))
        break
    rows.append((k0, k1))
    print("Zeile %2d: {0x%02X, 0x%02X} @Datei 0x%X" % (i, k0, k1, a + i*2))
    i += 1
    if i > 200: break

# Welche Zeilen enthalten 0x16?
print("\nZeilen mit 0x16:", [(n, "{0x%02X,0x%02X}" % r) for n, r in enumerate(rows) if 0x16 in r])

# ---- RDT-Zensus: Sce_em_set (0x44, 22 B, +3=kind, +7=sound) via echtem SCD-Walk --------
def scd_subs(data, scd_off, bound):
    """liefert Liste (sub_index, abs_start, abs_end)"""
    if scd_off == 0 or scd_off >= len(data): return []
    first = struct.unpack_from("<H", data, scd_off)[0]
    if first == 0 or first % 2 or first > 0x800: return []
    n = first // 2
    offs = []
    for k in range(n):
        o = struct.unpack_from("<H", data, scd_off + k*2)[0]
        offs.append(o)
    subs = []
    for k in range(n):
        st = scd_off + offs[k]
        en = scd_off + (offs[k+1] if k+1 < n else bound - scd_off)
        if st > len(data): continue
        subs.append((k, st, min(en, len(data))))
    return subs

def walk(data, st, en):
    """linearer Opcode-Walk; liefert (ok, [(abs_off, opcode)])"""
    out = []
    p = st
    while p < en:
        op = data[p]
        if op == 0x00 and p + 1 >= en:  # trailing NOP/pad
            out.append((p, op)); break
        ln = TABLE[op] if op < len(TABLE) else 0
        if ln == 0:
            return (False, out, p, op)
        out.append((p, op))
        if op == 0x13:  # switch: Header 4 B, dann Case-Bloecke -- linear weiter geht meist gut
            pass
        p += ln
    return (True, out, p, None)

rdts = sorted(glob.glob(os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "*.RDT")))
print("\n=== RDT-Zensus (%d Dateien): Opcode 0x44 (Sce_em_set) ===" % len(rdts))
em23_rooms = {}
kind_sound = {}   # kind -> {sound: count}
walk_fail = 0
for path in rdts:
    data = open(path, "rb").read()
    name = os.path.basename(path)
    offs = struct.unpack_from("<23I", data, 8)
    recs = []
    for scd_idx in (16, 17):
        so = offs[scd_idx]
        if so == 0 or so >= len(data): continue
        # bound = kleinstes anderes Offset > so, sonst Dateiende
        bigger = [o for o in offs if o > so] + [len(data)]
        bound = min(bigger)
        for (k, st, en) in scd_subs(data, so, bound):
            ok, ops, endp, badop = walk(data, st, en)
            if not ok: walk_fail += 1
            for (o, op) in ops:
                if op == 0x44 and o + 22 <= len(data):
                    r = data[o:o+22]
                    recs.append((scd_idx, k, o, r))
    for (scd_idx, sub, o, r) in recs:
        kind, snd = r[3], r[7]
        kind_sound.setdefault(kind, {}).setdefault(snd, 0)
        kind_sound[kind][snd] += 1
        if kind == 0x23:
            em23_rooms.setdefault(name, []).append((scd_idx, sub, o, r))
    if any(r[3][3] == 0x23 for r in recs):
        print("\n-- %s: ALLE 0x44-Records (scd, sub, off, kind, sound, hex):" % name)
        for (scd_idx, sub, o, r) in recs:
            print("   scd%d sub%02d @0x%05X kind=0x%02X sound=0x%02X  %s"
                  % (scd_idx, sub, o, r[3], r[7], r.hex()))

print("\nWalk-Abbrueche (unbekannter Opcode): %d" % walk_fail)
print("\n=== kind->sound Statistik (nur kinds mit >=1 Record) ===")
for kind in sorted(kind_sound):
    print("kind 0x%02X: %s" % (kind, {("0x%02X" % s): c for s, c in sorted(kind_sound[kind].items())}))
