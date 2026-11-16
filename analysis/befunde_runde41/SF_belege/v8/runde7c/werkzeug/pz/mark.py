"""Pruefer P5 (Zahlen) - eigene Marker-Assets. Kein Code der Bauer/Bank.
Jede Flaeche bekommt eine CLUT-Zeile nach Klasse; alle Eintraege der Zeile werden auf eine reine Markerfarbe gesetzt (STP-Bit bleibt).
Alpha-Sicherheit: in allen Staenden haben die Zeilen 480-482,484-486,488-492 keinen Eintrag 0 (gemessen, BERICHT Schritt 1), also
aendert das Umhaengen einer Flaeche auf eine dieser Zeilen keine Transparenz.
Klassen -> Zeile/Farbe (PSX 15 bit, R bits 0-4, G 5-9, B 10-14):
  0 Rest (andere Meshes)                 480 gruen   0x03E0
  1 Mesh 8, Schwerpunkt y8 >= -971       481 rot     0x001F
  2 Mesh 8, Schwerpunkt y8 <  -971 (R)   482 blau    0x7C00   (-971 = luecke_r5.Y_RUECKEN, Kopfgelenk EMR relpos Knochen 9)
  3 Mesh 9 Basis (V7-Praefix), x9 < 150  484 magenta 0x7C1F
  4 Mesh 9 Basis, Gesicht x9 >= 150      485 tuerkis 0x7FE0   (150 = luecke_r5.GESICHT_X)
  5 Mesh 9 angehaengt (Index >= V7)      486 gelb    0x03FF
Aufruf: python mark.py IN.EMD OUT_LM.EMD [--ohne OUT_OHNE.EMD]   (--ohne: angehaengte Mesh-9-Flaechen entartet = Modell ohne Anhang)
"""
import sys, struct
import numpy as np
sys.path.insert(0, __file__.rsplit('/', 1)[0] if '/' in __file__ else '.')
import pl

V7P = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/m/V7.EMD'
ZEILE = {0: 480, 1: 481, 2: 482, 3: 484, 4: 485, 5: 486}
FARBE = {480: 0x03E0, 481: 0x001F, 482: 0x7C00, 484: 0x7C1F, 485: 0x7FE0, 486: 0x03FF}
u32 = lambda b, o: struct.unpack_from('<I', b, o)[0]


def offs(b, mesh):
    do = u32(b, 0); md1 = u32(b, do + 4 * 7)
    f = struct.unpack_from('<14I', b, md1 + 12 + 56 * mesh); B = md1 + 12
    tfo, tfc, tuo, qfo, qfc, quo = f[4], f[5], f[6], f[11], f[12], f[13]
    return ([B + tfo + 12 * k for k in range(tfc)], [B + tuo + 12 * k for k in range(tfc)],
            [B + qfo + 16 * k for k in range(qfc)], [B + quo + 16 * k for k in range(qfc)])


def setz(bb, o, r):
    c = struct.unpack_from('<H', bb, o + 2)[0]; struct.pack_into('<H', bb, o + 2, (c & 0x3F) | (r << 6))


def klassen_flaechen(E, V7):
    """-> {(mesh, 't'|'q', idx): klasse}"""
    n7t, n7q = len(V7.meshes[9]['tf']), len(V7.meshes[9]['qf'])
    out = {}
    for mi, m in enumerate(E.meshes):
        for kind, V, F in (('t', m['tv'], m['tf']), ('q', m['qv'], m['qf'])):
            for k, f in enumerate(F):
                c = V[list(f[1::2])].mean(0)
                if mi == 8:
                    kl = 2 if c[1] < -971 else 1
                elif mi == 9:
                    if (kind == 't' and k >= n7t) or (kind == 'q' and k >= n7q):
                        kl = 5
                    else:
                        kl = 4 if c[0] >= 150 else 3
                else:
                    kl = 0
                out[(mi, kind, k)] = kl
    return out


def bauen(blob, ohne=False):
    E = pl.EMD(blob); V7 = pl.EMD(open(V7P, 'rb').read())
    b = bytearray(blob)
    do = u32(b, 0); tim = u32(b, do + 4 * 8)
    cw, ch = struct.unpack_from('<2H', b, tim + 8 + 8)
    co = lambda r: tim + 20 + 2 * cw * (r - 480)
    kl = klassen_flaechen(E, V7)
    n7t = len(V7.meshes[9]['tf']); n7q = len(V7.meshes[9]['qf'])
    for mi in range(len(E.meshes)):
        tf, tu, qf, qu = offs(b, mi)
        for k, o in enumerate(tu):
            setz(b, o, ZEILE[kl[(mi, 't', k)]])
        for k, o in enumerate(qu):
            setz(b, o, ZEILE[kl[(mi, 'q', k)]])
        if ohne and mi == 9:
            for lst, w, n0 in ((tf, 6, n7t), (qf, 8, n7q)):
                for k in range(n0, len(lst)):
                    o = lst[k]; f = list(struct.unpack_from('<%dH' % w, b, o))
                    for j in range(1, w, 2): f[j] = f[1]
                    struct.pack_into('<%dH' % w, b, o, *f)
    if not ohne:
        for r in range(480, 480 + ch):
            for k in range(cw):
                o = co(r) + 2 * k; v = struct.unpack_from('<H', b, o)[0]
                if v == 0:
                    continue
                struct.pack_into('<H', b, o, (v & 0x8000) | FARBE.get(r, 0x03E0))
    return bytes(b), kl


def ohne_anhang(blob):
    """Modell ohne angehaengte Mesh-9-Flaechen (entartet), Farben unveraendert."""
    E = pl.EMD(blob); V7 = pl.EMD(open(V7P, 'rb').read()); b = bytearray(blob)
    n7t = len(V7.meshes[9]['tf']); n7q = len(V7.meshes[9]['qf'])
    tf, tu, qf, qu = offs(b, 9)
    for lst, w, n0 in ((tf, 6, n7t), (qf, 8, n7q)):
        for k in range(n0, len(lst)):
            o = lst[k]; f = list(struct.unpack_from('<%dH' % w, b, o))
            for j in range(1, w, 2): f[j] = f[1]
            struct.pack_into('<%dH' % w, b, o, *f)
    return bytes(b)


if __name__ == '__main__':
    blob = open(sys.argv[1], 'rb').read()
    lm, kl = bauen(blob)
    open(sys.argv[2], 'wb').write(lm)
    from collections import Counter
    print(sys.argv[1], 'Klassen', sorted(Counter(kl.values()).items()))
    if '--ohne' in sys.argv:
        o = sys.argv[sys.argv.index('--ohne') + 1]
        open(o, 'wb').write(ohne_anhang(blob))
        lm0, _ = bauen(ohne_anhang(blob))
        open(o.replace('.EMD', '_LM.EMD'), 'wb').write(lm0)
        print('ohne ->', o)
