# -*- coding: utf-8 -*-
"""Runde 30 / irons-diary-welt: PROTOTYP eines Welt-Modells fuer die Memory Card (Item 0x21).

RE1.5 hat fuer Item 0x21 KEIN Welt-Modell (Zensus: 0 Item_aot_set in 240 RDTs). Vorhanden ist:
  (a) das Item-Bild ITEM/ITPS.ITP @0x21*0x3000 = Datei 0x63000 (112x72, 8bpp + CLUT):
      eine gerenderte PlayStation-Speicherkarte in Schraegansicht,
  (b) das Welt-Modell "Karte" des Spiels: das Keycard-MD1 (156 B, md5 93975479...,
      2 Dreiecke, 161 x 0 x 270), neunmal platziert (ROOM1011, 1110/1111, 1190/1191,
      3040/3041), mit einer 128x64-TIM, deren UV-Feld u 0..102 / v 0..63 ist.

Dieses Werkzeug nimmt (b) UNVERAENDERT als Geometrie und gewinnt die Textur aus (a): die
Deckflaeche der Karte wird im Item-Bild als Viereck bestimmt und per Homographie in das
UV-Feld des Keycard-Modells entzerrt. Das ist eine KONSTRUKTION aus vorhandener Kunst, kein
Original-Asset - so ist sie im Dossier gekennzeichnet.

TIM-Format 1:1 wie die Keycard-TIM von ROOM1110 Prop 2 (Kopf wird von dort uebernommen):
  flag 0x09, CLUT 256 Eintraege, Bild 64 Worte x 64 = 128x64, 8736 B.
CLUT-Index 0 = 0x0000 = PSX-Farbschluessel (nicht gezeichnet): die runden Ecken der Karte
bleiben durchsichtig. Kein anderer CLUT-Eintrag darf 0x0000 sein.

    python r30_idw_karte_modell.py
"""
import os, sys, struct
import numpy as np
import cv2
from PIL import Image

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
CD = os.path.join(REPO, 're15_port', 'shared_assets', 'PSX')
AUS = os.path.join(REPO, 'build', 'r30_irons-diary-welt', 'karte')
sys.path.insert(0, os.path.join(REPO, 're15_port', 'tools'))
import rdt_objekt_ansicht as RO

ITEM = 0x21
U_MAX, V_MAX = 102, 63          # UV-Feld des Keycard-MD1 (gelesen, s. main)


def itps_lesen(item):
    d = open(os.path.join(CD, 'ITEM', 'ITPS.ITP'), 'rb').read()
    o = item * 0x3000
    cl_len = struct.unpack_from('<I', d, o + 8)[0]
    clut = struct.unpack_from('<256H', d, o + 20)
    p = o + 8 + cl_len
    il, ix, iy, iw, ih = struct.unpack_from('<IHHHH', d, p)
    W = iw * 2
    pix = np.frombuffer(d, np.uint8, W * ih, p + 12).reshape(ih, W)
    rgb = np.zeros((ih, W, 3), np.uint8)
    for y in range(ih):
        for x in range(W):
            c = clut[pix[y, x]]
            rgb[y, x] = ((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3)
    return rgb, o


def deckflaeche(rgb):
    """Viereck der Karten-Deckflaeche im Item-Bild. Trennung ueber zwei Eigenschaften des
    Bildes selbst: der Hintergrund ist blau (B > R+20), die Seitenflaeche der Karte dunkel."""
    h, w = rgb.shape[:2]
    R = rgb[:, :, 0].astype(int); B = rgb[:, :, 2].astype(int)
    L = rgb.astype(int).sum(axis=2) / 3.0
    karte = ~(B > R + 20)
    karte[:3, :] = False; karte[-3:, :] = False; karte[:, :3] = False; karte[:, -3:] = False  # Bildrahmen
    ergebnis = {}
    for s in (60, 70, 80):
        m = (karte & (L >= s)).astype(np.uint8)
        n, lab, st, _ = cv2.connectedComponentsWithStats(m, 8)
        k = 1 + int(np.argmax(st[1:, cv2.CC_STAT_AREA]))
        m = (lab == k).astype(np.uint8)
        cs, _ = cv2.findContours(m, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        aussen = max(cs, key=cv2.contourArea)
        hull = cv2.convexHull(aussen)
        # Loecher schliessen: das dunkle Logo liegt IN der Deckflaeche und fiele sonst unter
        # die Helligkeitsschwelle (gemessen: 67 durchsichtige Texel mitten im Logo).
        m = np.zeros_like(m); cv2.drawContours(m, [aussen], -1, 1, thickness=cv2.FILLED)
        q = None
        for eps in np.arange(1.0, 12.0, 0.25):
            a = cv2.approxPolyDP(hull, eps, True)
            if len(a) == 4:
                q = a.reshape(4, 2).astype(float); break
        ergebnis[s] = (q, int(m.sum()), m)
    return ergebnis


def ecken_ordnen(q):
    """-> (oben-links, oben-rechts, unten-rechts, unten-links) im BILD. 'oben' = Logo-Ende."""
    q = sorted(q.tolist(), key=lambda p: p[1])
    oben = sorted(q[:2], key=lambda p: p[0]); unten = sorted(q[2:], key=lambda p: p[0])
    return oben[0], oben[1], unten[1], unten[0]


def main():
    os.makedirs(AUS, exist_ok=True)
    rgb, off = itps_lesen(ITEM)
    print('ITPS.ITP Item 0x%02X @Datei 0x%05X: %dx%d' % (ITEM, off, rgb.shape[1], rgb.shape[0]))
    erg = deckflaeche(rgb)
    for s, (q, n, m) in erg.items():
        print('   Schwelle L>=%d: Deckflaeche %d Pixel, Viereck %s' % (s, n, None if q is None else
              ' '.join('(%.1f,%.1f)' % tuple(p) for p in ecken_ordnen(q))))
    q, n, maske = erg[70]
    ol, orr, ur, ul = ecken_ordnen(q)
    kl = lambda a, b: float(np.hypot(a[0] - b[0], a[1] - b[1]))
    print('   Kanten im Bild: oben %.1f unten %.1f links %.1f rechts %.1f'
          % (kl(ol, orr), kl(ul, ur), kl(ol, ul), kl(orr, ur)))

    # Keycard-Modell des Spiels, unveraendert
    d = open(os.path.join(CD, 'STAGE1', 'ROOM1110.RDT'), 'rb').read()
    ktim, kmd1 = RO.props_lesen(d)[2]
    open(os.path.join(AUS, 'memcard.md1'), 'wb').write(kmd1)
    h = struct.unpack_from('<14I', kmd1, 12)
    V = [struct.unpack_from('<3h', kmd1, 12 + h[0] + 8 * i) for i in range(h[1])]
    uvs = []
    for t in range(h[5]):
        idx = struct.unpack_from('<6H', kmd1, 12 + h[4] + 12 * t)
        u = struct.unpack_from('<BBHBBHBBH', kmd1, 12 + h[6] + 12 * t)
        for k, (vi, uu, vv) in enumerate(((idx[1], u[0], u[1]), (idx[3], u[3], u[4]), (idx[5], u[6], u[7]))):
            uvs.append((V[vi], (uu, vv)))
    print('Keycard-MD1 %d B: Punkte %s' % (len(kmd1), V))
    print('   Punkt -> UV:', sorted(set(uvs)))

    # Zuordnung Textur -> Karte. Im Modell laeuft u mit Modell-Z (0..270), v mit Modell-X
    # (-161 bei v=0 .. 0 bei v=63). Bei rot_y=3072 zeigt Modell-Z vom Betrachter WEG und
    # Modell-X nach Bild-rechts. Die Karte soll fuer den Spieler vor dem Tisch aufrecht liegen
    # (Logo oben = fern, "SONY" unten = nah), also:
    #   (u=U_MAX, v=0)     = Logo-Ende links   = ol
    #   (u=U_MAX, v=V_MAX) = Logo-Ende rechts  = orr
    #   (u=0,     v=V_MAX) = SONY-Ende rechts  = ur
    #   (u=0,     v=0)     = SONY-Ende links   = ul
    ziel = np.array([[U_MAX, 0], [U_MAX, V_MAX], [0, V_MAX], [0, 0]], np.float32)
    quelle = np.array([ol, orr, ur, ul], np.float32)
    Hm = cv2.getPerspectiveTransform(ziel, quelle)           # Textur -> Bild
    tex = np.zeros((64, 128, 3), np.uint8); alpha = np.zeros((64, 128), bool)
    for v in range(V_MAX + 1):
        for u in range(U_MAX + 1):
            p = Hm @ np.array([u + 0.5, v + 0.5, 1.0]); x = p[0] / p[2] - 0.5; y = p[1] / p[2] - 0.5
            x0 = int(np.floor(x)); y0 = int(np.floor(y)); fx = x - x0; fy = y - y0
            if x0 < 0 or y0 < 0 or x0 + 1 >= rgb.shape[1] or y0 + 1 >= rgb.shape[0]:
                continue
            xi = int(round(x)); yi = int(round(y))
            if not maske[yi, xi]:
                continue                                       # runde Ecke / ausserhalb
            c = (rgb[y0, x0] * (1 - fx) * (1 - fy) + rgb[y0, x0 + 1] * fx * (1 - fy)
                 + rgb[y0 + 1, x0] * (1 - fx) * fy + rgb[y0 + 1, x0 + 1] * fx * fy)
            tex[v, u] = np.clip(np.round(c), 0, 255); alpha[v, u] = True
    print('Textur: %d von %d Texeln des UV-Felds belegt' % (int(alpha.sum()), (U_MAX + 1) * (V_MAX + 1)))

    # 255 Farben + Index 0 als Farbschluessel
    im = Image.fromarray(tex).quantize(colors=255, method=Image.MEDIANCUT, dither=Image.NONE)
    pal = im.getpalette()[:255 * 3]
    idx = np.array(im, np.uint8) + 1
    idx[~alpha] = 0
    clut = [0x0000]
    for i in range(255):
        r, g, b = pal[3 * i] >> 3, pal[3 * i + 1] >> 3, pal[3 * i + 2] >> 3
        c = r | (g << 5) | (b << 10)
        if c == 0:
            c = 0x0400                                         # nie 0x0000 ausser Index 0
        clut.append(c)
    # TIM-Kopf von der Keycard-TIM
    cl_len = struct.unpack_from('<I', ktim, 8)[0]
    kopf_clut = ktim[:20]
    p = 8 + cl_len
    kopf_bild = ktim[p:p + 12]
    tim = bytearray(kopf_clut) + struct.pack('<256H', *clut) + bytearray(kopf_bild) + idx.tobytes()
    assert len(tim) == len(ktim), (len(tim), len(ktim))
    open(os.path.join(AUS, 'memcard.tim'), 'wb').write(tim)
    print('geschrieben: memcard.md1 (%d B, byte-gleich Keycard ROOM1110 Prop 2), memcard.tim (%d B)'
          % (len(kmd1), len(tim)))
    L = tex.astype(int).sum(axis=2)[alpha] / 3.0
    print('Textur-Helligkeit: Mittel %.0f, hellstes Prozent %.0f' % (L.mean(), np.percentile(L, 99)))

    # Vorschau
    vor = np.zeros((64, 128, 3), np.uint8); vor[:] = (40, 0, 60)
    for v in range(64):
        for u in range(128):
            c = clut[idx[v, u]]
            if idx[v, u]:
                vor[v, u] = ((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3)
    Image.fromarray(vor).resize((512, 256), Image.NEAREST).save(os.path.join(AUS, 'memcard_tim_x4.png'))
    q4 = Image.fromarray(rgb).resize((rgb.shape[1] * 4, rgb.shape[0] * 4), Image.NEAREST)
    from PIL import ImageDraw
    dr = ImageDraw.Draw(q4)
    pts = [tuple(4 * (np.array(p) + 0.5)) for p in (ol, orr, ur, ul)]
    dr.polygon(pts, outline=(255, 255, 0))
    q4.save(os.path.join(AUS, 'itps21_deckflaeche_x4.png'))


if __name__ == '__main__':
    main()
