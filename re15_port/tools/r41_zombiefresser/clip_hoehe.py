#!/usr/bin/env python3
"""Runde 41 Spur I: Pose-Hoehe je Clip/Bild einer RE1.5-Gegnerbank (liegend vs stehend).

Bank 1 = dir[3] EDD + dir[4] Keyframe-Pool, Skelettstruktur aus dir[2] (emd_common.c
re15_emd_parse_container: Hauptbank = groesste EDD-Bank, Struktur dir[2], Pool re-pointed).
Bank 0 = dir[1]/dir[2]. Hoehe = kleinstes Welt-Y (PSX: -Y = oben) ueber alle Knochenursprunge;
'top' < -1500 = aufrecht, > -700 = liegend (Messung, keine Konstante im Spiel).
Aufruf: python clip_hoehe.py <typ> <bank> <clip> [<clip> ...]
"""
import os, sys
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
import emd_ansichtsblatt as A  # noqa: E402

def lade(typ, bank):
    m = A.Modell(typ)
    d = A.emd_dir(m.blob)
    if bank > 0:
        edd = m.blob[d[1 + 2*bank]:d[2 + 2*bank]]
        m.clips, m.frames = A.edd_parse(edd)
        pool = m.blob[d[2 + 2*bank]:]
        kfo = A.u16(pool, 2)
        m.skel.kf_data = pool[kfo:]
        m.skel.kf_count = len(m.skel.kf_data) // m.skel.kf_size
    return m

def top(m, kf):
    R, T = m.pose(kf)
    return min(int(t[1]) for t in T)

def main():
    typ = int(sys.argv[1], 0); bank = int(sys.argv[2]); clips = [int(c, 0) for c in sys.argv[3:]]
    m = lade(typ, bank)
    print(f"typ 0x{typ:02x} bank {bank}: {len(m.clips)} clips")
    for c in clips:
        first, n = m.clips[c]
        ws = m.frames[first:first+n]
        kfs = [w & 0xfff for w in ws]
        tops = [top(m, k) for k in kfs]
        step = max(1, n // 8)
        sample = ", ".join(f"f{i}:{tops[i]}" for i in range(0, n, step))
        print(f"  clip 0x{c:02x} ({c}) n={n} top f0={tops[0]} last={tops[-1]} min={min(tops)} max={max(tops)} | {sample}")

if __name__ == "__main__":
    main()
