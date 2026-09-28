# -*- coding: utf-8 -*-
"""Runde 30 / irons-diary-welt: welche ORIGINAL-Masken liegen ueber den zwei Ablagen?

Liest die sprite.pri-Sektion des Cuts direkt aus der RDT (Rechteck-Leser:
re15_port/tools/maske/original.py artist_rects) und listet JEDES Maskenrechteck, das die
Bildhuelle eines Props schneidet, mit RDT-Offset, Tiefe und der daraus folgenden Schwelle.

Regel (include/re15_pri.h, aus PSX.EXE): Maske OT-Index = depth (@0x80039658), Figur/Objekt
OT-Index = otz>>4 (@0x8002565c); verdeckt wird gdw. depth < Bucket.
   Dreiecke: Bucket = (1023*vz)>>16  (ZSF3 = 341 @0x80066c70)
   Vierecke: Bucket = vz>>6          (ZSF4 = 256 @0x80066c7c)
"""
import os, sys, struct
HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
sys.path.insert(0, os.path.join(REPO, 're15_port', 'tools', 'maske'))


def rects_mit_offset(rdt, cam, cut):
    po = struct.unpack_from('<I', rdt, cam + cut * 32 + 0x1C)[0]
    gc, mc = struct.unpack_from('<HH', rdt, po)
    if gc in (0, 0xFFFF) or mc == 0 or gc > 256:
        return po, gc, mc, []
    p = po + 4
    gn, gdx, gdy = [], [], []
    for _ in range(gc):
        n_, base, dx, dy = struct.unpack_from('<HHhh', rdt, p)
        gn.append(n_); gdx.append(dx); gdy.append(dy); p += 8
    out, gi, used = [], 0, 0
    for _ in range(sum(gn)):
        off = p
        sx, sy, dx, dy = rdt[p], rdt[p + 1], rdt[p + 2], rdt[p + 3]
        dep, size = struct.unpack_from('<HH', rdt, p + 4); p += 8
        if (size & 0xF000) == 0:
            w, h = struct.unpack_from('<HH', rdt, p); p += 4
        else:
            w = h = (size >> 12) * 8
        while gi < gc and used >= gn[gi]:
            gi += 1; used = 0
        ax, ay = (gdx[gi], gdy[gi]) if gi < gc else (0, 0)
        used += 1
        X = ((dx + ax + 0x8000) & 0xFFFF) - 0x8000
        Y = ((dy + ay + 0x8000) & 0xFFFF) - 0x8000
        out.append(dict(off=off, x=X, y=Y, w=w, h=h, dep=dep, grp=gi, roh=rdt[off:p].hex(' ')))
    return po, gc, mc, out


def main():
    rdt = open(os.path.join(REPO, 're15_port', 'shared_assets', 'PSX', 'STAGE1', 'ROOM1150.RDT'), 'rb').read()
    cam = struct.unpack_from('<I', rdt, 0x24)[0]
    print('Kameratabelle @0x%X' % cam)
    # Bildhuellen der Props aus den Rig-Laeufen (rig_auswertung.txt), 320er-Koordinaten
    huellen = {
        2: {'Dokument A': (146.3, 157.3, 123.3, 129.3), 'Dokument B': (148.3, 158.3, 121.3, 127.3),
            'Karte': (136.3, 143.3, 124.3, 128.3)},
        6: {'Dokument A': (177.3, 209.3, 118.3, 156.3), 'Dokument B': (182.0, 212.0, 116.0, 141.3),
            'Karte': (148.3, 168.3, 120.3, 145.3)},
    }
    for cut in (2, 6):
        po, gc, mc, R = rects_mit_offset(rdt, cam, cut)
        print('Cut %d: pri_offset 0x%X, Gruppen %d, Masken %d (gelesen %d)' % (cut, po, gc if gc != 0xFFFF else -1, mc, len(R)))
        if R:
            ds = sorted(set(r['dep'] for r in R))
            print('   vorkommende Tiefen:', ' '.join(str(d) for d in ds))
        for n, (x0, x1, y0, y1) in huellen[cut].items():
            tr = [r for r in R if r['x'] < x1 and r['x'] + r['w'] > x0 and r['y'] < y1 and r['y'] + r['h'] > y0]
            print('   %s, Bildhuelle x %.1f..%.1f y %.1f..%.1f: %d Maskenrechtecke' % (n, x0, x1, y0, y1, len(tr)))
            for r in tr:
                print('      @0x%05X  %s   Bild x %d..%d y %d..%d  Tiefe %d  -> verdeckt Vierecke ab vz >= %d, Dreiecke ab vz >= %.1f'
                      % (r['off'], r['roh'], r['x'], r['x'] + r['w'] - 1, r['y'], r['y'] + r['h'] - 1, r['dep'],
                         (r['dep'] + 1) * 64, (r['dep'] + 1) * 65536.0 / 1023.0))


if __name__ == '__main__':
    main()
