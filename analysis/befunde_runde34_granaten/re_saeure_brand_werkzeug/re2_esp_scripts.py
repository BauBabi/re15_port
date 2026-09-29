#!/usr/bin/env python3
"""Runde 34 / re_saeure_brand: RE2-ESP-Skripte (CORE00.ESP oder Raum-ESP-Block) vollstaendig
auflisten: je Bank die 8 Skripte (SUB&7), je Skript die Parts, je Part die 24-Byte-Steps mit
Opcode A / Opcode B (Opcode-Tabelle RE2 @0x8009D868, 96 Eintraege).

Format (Quelle: analysis/konstruktion_2026-08-23/re2-fx-system.md §1b/§1d, dort gegen
FUN_8001bca0/FUN_8001bf10/FUN_8001dc30 belegt; hier nur gelesen, nicht neu behauptet):
  Datei: 8 Id-Bytes (0xFF = Ende); Offsets RUECKWAERTS ab letztem Wort (FUN_8001bca0).
  Bank: +0 u16 n1 (Anim), +2 u16 n2 (UV), +4 clut, +6 tpage, n1*8 Anim, n2*4 UV,
        dann 8 x u16 Skript-Offsets (in WORTEN ab Tabellenstart; 0 = leer).
  Skript: u16 nparts, u16 pad; je Part: u16 nsteps, u16 pad, nsteps * 24 B Steps.
  Step: [0] OpA [1] OpB [2] Anim-Start [3] op-spez. [4] u16 X-Aspekt [6] u16 Y-Aspekt
        [8..A] s8 Beschl. [B] s8 Step-Delta [C,E,10] s16 Geschw. [12] u16 Status
        [14] u16 TPage-OR [16] u16 Random-Range.
Aufruf: re2_esp_scripts.py [datei] [--nur-op 15,47,48,49,40]
"""
import os, sys, struct
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))


def banks(d):
    ids = []
    for b in d[:8]:
        if b == 0xFF:
            break
        ids.append(b)
    end = ((len(d) + 3) & ~3) - 4
    out = []
    for i, bid in enumerate(ids):
        off = struct.unpack_from('<i', d, end - 4 * i)[0]
        out.append((bid, off))
    return out


def dump(d, only_ops=None):
    lines = []
    for bid, off in banks(d):
        n1, n2, clut, tp = struct.unpack_from('<HHHH', d, off)
        st = off + 8 + 8 * n1 + 4 * n2
        tab = [struct.unpack_from('<H', d, st + 2 * k)[0] for k in range(8)]
        hdr = 'Bank %d @0x%04X n1=%d n2=%d clut=0x%04X tpage=0x%04X Skripttabelle @0x%04X %s' % (
            bid, off, n1, n2, clut, tp, st, tab)
        blines = []
        for k in range(8):
            if tab[k] == 0:
                continue
            p = st + tab[k] * 4
            nparts = struct.unpack_from('<H', d, p)[0]
            q = p + 4
            slines = ['  Skript %d (SUB&7=%d) @0x%04X: %d Part(s)' % (k, k, p, nparts)]
            hit = False
            for part in range(nparts):
                ns = struct.unpack_from('<H', d, q)[0]
                slines.append('    Part %d @0x%04X: %d Step(s)' % (part, q, ns))
                q += 4
                for s in range(ns):
                    b = d[q:q + 24]
                    opa, opb, an, o3 = b[0], b[1], b[2], b[3]
                    ax, ay = struct.unpack_from('<HH', b, 4)
                    acc = struct.unpack_from('<bbb', b, 8)
                    dl = struct.unpack_from('<b', b, 11)[0]
                    vel = struct.unpack_from('<hhh', b, 12)
                    stt, tpo, rr = struct.unpack_from('<HHH', b, 18)
                    if only_ops is None or opa in only_ops or opb in only_ops:
                        hit = True
                    slines.append('      S%02d @0x%04X OpA=%-3d OpB=%-3d anim=%-3d b3=%-3d asp=(%d,%d) acc=%s d=%d vel=%s st=0x%04X tp=0x%04X rr=%d  [%s]'
                                  % (s, q, opa, opb, an, o3, ax, ay, acc, dl, vel, stt, tpo, rr, b.hex()))
                    q += 24
            if only_ops is None or hit:
                blines += slines
        if blines:
            lines.append(hdr)
            lines += blines
    return lines


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    only = None
    for a in sys.argv[1:]:
        if a.startswith('--nur-op'):
            only = set(int(x, 0) for x in a.split('=', 1)[1].split(','))
    path = args[0] if args else os.path.join(REPO, 'info', 're2leon', 'COMMON', 'DATA', 'CORE00.ESP')
    d = open(path, 'rb').read()
    print('; %s (%d B)' % (path, len(d)))
    for l in dump(d, only):
        print(l)


if __name__ == '__main__':
    main()
