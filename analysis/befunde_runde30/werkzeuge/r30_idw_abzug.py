# -*- coding: utf-8 -*-
"""Runde 30 / irons-diary-welt: KONTROLLABZUG.

Zeichnet das RE2-Dokument-Prop (mesh03_cf9f316d.md1/.tim, unveraendert) und einen
flachen Karten-Platzhalter in die Hintergruende Cut 2 und Cut 6 von ROOM1150 - mit der
Sichtmatrix der ENGINE (kamera_1150.txt aus der Sonde) und derselben Prop-Drehmatrix wie
platform/pc/main.c pc_prop_rot_q12 (reines rot_y: [[c,0,s],[0,1,0],[-s,0,c]]).

NICHT byte-true und bewusst so: keine Raumbeleuchtung (der Port beleuchtet Props ueber
re15_light_shade_vertex), affine Texturabbildung, 4x ueberabgetastet. Der Abzug prueft
LAGE und GROESSE, nicht die Helligkeit.

    python r30_idw_abzug.py
"""
import os, sys, struct, math
import numpy as np
from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from r30_idw_geom import lade_kameras, AUS, REPO

SS = 4          # Ueberabtastung
ROT  = (152.5, 126.5)   # Mitte der roten Marke  (x 146..158, y 120..132 -> [146,159)x[120,133))
BLAU = (140.0, 126.5)   # Mitte der blauen Marke (x 135..144, y 121..131 -> [135,145)x[121,132))


def tim_laden(pfad):
    d = open(pfad, 'rb').read()
    cl_len, cx, cy, cw, ch = struct.unpack_from('<IHHHH', d, 8)
    cluts = [struct.unpack_from('<%dH' % cw, d, 20 + 2 * cw * i) for i in range(ch)]
    o = 8 + cl_len
    il, ix, iy, iw, ih = struct.unpack_from('<IHHHH', d, o)
    W = iw * 2
    pix = np.frombuffer(d, np.uint8, W * ih, o + 12).reshape(ih, W)
    return pix, cluts, cy


def md1_laden(pfad):
    m = open(pfad, 'rb').read()
    h = struct.unpack_from('<14I', m, 12)
    V = [struct.unpack_from('<3h', m, 12 + h[7] + 8 * i) for i in range(h[8])]
    Q = []
    for q in range(h[12]):
        idx = struct.unpack_from('<8H', m, 12 + h[11] + 16 * q)
        u = struct.unpack_from('<BBHBBHBBHBBH', m, 12 + h[13] + 16 * q)
        Q.append(dict(v=(idx[1], idx[3], idx[5], idx[7]),
                      uv=((u[0], u[1]), (u[3], u[4]), (u[6], u[7]), (u[9], u[10])),
                      clut=u[2], page=u[5]))
    return V, Q


def rot_y_q12(ry):
    a = (ry & 4095) / 4096.0 * 2 * math.pi
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])


def zeichne_dreieck(bild, zbuf, P, UV, tex_fn):
    (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) = P
    minx = max(int(math.floor(min(x0, x1, x2))), 0); maxx = min(int(math.ceil(max(x0, x1, x2))), bild.shape[1] - 1)
    miny = max(int(math.floor(min(y0, y1, y2))), 0); maxy = min(int(math.ceil(max(y0, y1, y2))), bild.shape[0] - 1)
    den = (y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2)
    if abs(den) < 1e-9:
        return
    for y in range(miny, maxy + 1):
        for x in range(minx, maxx + 1):
            px, py = x + 0.5, y + 0.5
            w0 = ((y1 - y2) * (px - x2) + (x2 - x1) * (py - y2)) / den
            w1 = ((y2 - y0) * (px - x2) + (x0 - x2) * (py - y2)) / den
            w2 = 1 - w0 - w1
            if w0 < 0 or w1 < 0 or w2 < 0:
                continue
            z = w0 * z0 + w1 * z1 + w2 * z2
            if z >= zbuf[y, x]:
                continue
            u = w0 * UV[0][0] + w1 * UV[1][0] + w2 * UV[2][0]
            v = w0 * UV[0][1] + w1 * UV[1][1] + w2 * UV[2][1]
            c = tex_fn(u, v)
            if c is None:
                continue
            zbuf[y, x] = z
            bild[y, x] = c


def prop_zeichnen(bild, zbuf, cam, pos, ry, V, Q, tex_fn):
    R = rot_y_q12(ry)
    Ri, T, H = cam['Ri'], cam['T'], cam['H']
    S = []
    for v in V:
        w = R @ np.array(v, float) + np.array(pos, float)
        c = np.floor((Ri @ w) / 4096.0) + T
        S.append((SS * (160.0 + H * c[0] / c[2]), SS * (120.0 + H * c[1] / c[2]), c[2]))
    for q in Q:
        a, b, c, d = q['v']
        for tri, uv in (((a, b, d), (0, 1, 3)), ((a, d, c), (0, 3, 2))):
            zeichne_dreieck(bild, zbuf, [S[i] for i in tri], [q['uv'][i] for i in uv], tex_fn)
    xs = [s[0] / SS for s in S]; ys = [s[1] / SS for s in S]
    return min(xs), max(xs), min(ys), max(ys)


def auf_ebene(cam, sx, sy, y):
    R = cam['R']; Rinv = np.linalg.inv(R)
    d = Rinv @ np.array([(sx - 160.0) / cam['H'], (sy - 120.0) / cam['H'], 1.0])
    o = Rinv @ (-cam['T'])
    s = (y - o[1]) / d[1]
    return o + s * d


def main():
    cams = lade_kameras()
    V, Q = md1_laden(os.path.join(REPO, 'extracted_re2_dokumente', 'weltmodelle', 'mesh03_cf9f316d.md1'))
    pix, cluts, cy = tim_laden(os.path.join(REPO, 'extracted_re2_dokumente', 'weltmodelle', 'mesh03_cf9f316d.tim'))

    def tex_diary(u, v):
        ui = min(max(int(u), 0), pix.shape[1] - 1); vi = min(max(int(v), 0), pix.shape[0] - 1)
        c = cluts[0][pix[vi, ui]]
        if c == 0:
            return None
        return ((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3)

    # Karten-Platzhalter: Geometrie des RE1.5-Keycard-Props (ROOM1110 Prop 2, 161 x 0 x 270),
    # einfarbig grau wie das ITPS-Bild 0x21 - NUR zur Groessen-/Lagekontrolle.
    KV = [(0, 0, 0), (0, 0, 270), (-161, 0, 270), (-161, 0, 0)]
    KV = [(x + 80, y, z - 135) for (x, y, z) in KV]          # auf die Mitte gezogen
    KQ = [dict(v=(0, 1, 3, 2), uv=((0, 0), (0, 1), (1, 0), (1, 1)))]

    def tex_karte(u, v):
        return (150, 150, 155)

    Y_PLATTE = -1520
    faelle = {
        'A_marke':     dict(diary=None, ry=3072),
        'B_klemmbrett': dict(diary=(-23813, Y_PLATTE - 13, -18296), ry=2990),
    }
    d_a = auf_ebene(cams[2], ROT[0], ROT[1], Y_PLATTE - 13)
    faelle['A_marke']['diary'] = tuple(int(round(v)) for v in d_a)
    k = auf_ebene(cams[2], BLAU[0], BLAU[1], Y_PLATTE)
    karte = tuple(int(round(v)) for v in k)
    print('Diary  A (Markenmitte rot, Buchmitte y=%d): %s' % (Y_PLATTE - 13, faelle['A_marke']['diary']))
    print('Diary  B (Mitte des gemalten Klemmbretts):   %s' % (faelle['B_klemmbrett']['diary'],))
    print('Karte    (Markenmitte blau, y=%d):          %s' % (Y_PLATTE, karte))

    for name, f in faelle.items():
        for cut in (2, 6):
            bg = Image.open(os.path.join(AUS, 'bg', 'ROOM115%02d.png' % cut)).convert('RGB')
            gross = np.array(bg.resize((320 * SS, 240 * SS), Image.NEAREST))
            zbuf = np.full(gross.shape[:2], 1e18)
            bb = prop_zeichnen(gross, zbuf, cams[cut], f['diary'], f['ry'], V, Q, tex_diary)
            bk = prop_zeichnen(gross, zbuf, cams[cut], karte, 3072, KV, KQ, tex_karte)
            klein = Image.fromarray(gross).resize((320, 240), Image.BOX)
            klein.save(os.path.join(AUS, '11_abzug_%s_cut%d.png' % (name, cut)))
            # Ansicht 4x mit Markenmitten
            an = Image.fromarray(gross)
            dr = ImageDraw.Draw(an)
            if cut == 2:
                for (mx, my), col in ((ROT, (255, 0, 0)), (BLAU, (0, 160, 255))):
                    dr.line((mx * SS - 8, my * SS, mx * SS + 8, my * SS), fill=col)
                    dr.line((mx * SS, my * SS - 8, mx * SS, my * SS + 8), fill=col)
            box = (90 * SS, 100 * SS, 230 * SS, 150 * SS) if cut == 2 else (60 * SS, 60 * SS, 300 * SS, 200 * SS)
            an.crop(box).save(os.path.join(AUS, '11_abzug_%s_cut%d_ausschnitt.png' % (name, cut)))
            print('%s Cut %d: Diary-Huelle x %.1f..%.1f y %.1f..%.1f (Mitte %.2f,%.2f) | Karte-Huelle x %.1f..%.1f y %.1f..%.1f (Mitte %.2f,%.2f)'
                  % (name, cut, bb[0], bb[1], bb[2], bb[3], (bb[0] + bb[1]) / 2, (bb[2] + bb[3]) / 2,
                     bk[0], bk[1], bk[2], bk[3], (bk[0] + bk[1]) / 2, (bk[2] + bk[3]) / 2))


if __name__ == '__main__':
    main()
