#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Exportiert die zwei Welt-Props auf Irons' Schreibtisch (ROOM1150/1151) ins
ENGINE-Format und schreibt sie als C-Einbindung engine/src/gen/irons_tisch_props.inc.

Runde 30, Thema E2 "Irons Diary: Welt-Prop + Memory Card".
Dossier: analysis/befunde_runde30/irons-diary-welt.md (Abschnitte 2.7, 3.8, 3.9, S2).

WARUM EINGEBACKENE BYTES UND KEIN ASSET-PATCH: die ausgelieferten RDTs bleiben
byte-true (dieselbe Linie wie gen/sicherung_prop.inc). Beide Gegenstaende kommen als
ZUSAETZLICHE Props portseitig dazu (obj_id 5/6; ROOM1150/1151 haben nOmodel = 4).

DIE VIER QUELLEN (jede gegen ihre md5 geprueft; stimmt eine nicht, bricht das Werkzeug ab):

  Dokument-MD1   info/re2leon/PL0/RDT/ROOM10E0.RDT @0x002320, 372 B
                 md5 cf9f316dd6ba0ecf599bea18a83c36d1 — RE2s Weltmodell von
                 "Secretary's diary B" (Obj_model_set sub00 @0x01CDA, Item_aot_set
                 @0x01D26 Item 0x70). UNVERAENDERT.
  Dokument-TIM   dieselbe Datei @0x015224, 17 440 B, md5 63bd93f2be1cc97066aa09afd3c67e4d.
                 UNVERAENDERT.
  Karten-MD1     re15_port/shared_assets/PSX/STAGE1/ROOM1110.RDT @0x0013D8, 156 B,
                 md5 93975479cdd85aa9e8e4232f6c8c8983 — das Keycard-Modell des Spiels
                 (RDT+0x30-Tabelle @0x200, Prop 2). UNVERAENDERT.
  Karten-TIM     KONSTRUIERT, kein Original-Asset: die Deckflaeche der Karte im
                 RE1.5-Item-Bild 0x21 (ITEM/ITPS.ITP @0x21*0x3000 = Datei 0x63000,
                 112x72) wird per Homographie in das UV-Feld des Keycard-Modells
                 (u 0..102 / v 0..63) entzerrt. TIM-Kopf 1:1 von der Keycard-TIM
                 (ROOM1110.RDT @0x02BAC8, 8736 B, md5 5ce475a418189da5baf3ecd1c81db83c).
                 Soll-md5 des Ergebnisses 885d91614f6ff1378cc0ea41328810b8 (Prototyp der
                 Ermittlung, analysis/befunde_runde30/irons-diary-welt/memcard.tim).
                 RE1.5 hat fuer Item 0x21 KEIN Welt-Modell (0 Item_aot_set in 240 RDTs)
                 und RE2 kein Item "Memory Card" — deshalb diese Konstruktion.

Verfahren fuer die Karten-TIM wortgleich mit dem Mess-Werkzeug der Ermittlung
analysis/befunde_runde30/werkzeuge/r30_idw_karte_modell.py (hierher uebernommen, wie
der Plan S2 verlangt). Die Schwelle L >= 70 ist KEINE Original-Konstante, sondern die
mittlere der drei durchgefahrenen (60/70/80 liefern dasselbe Viereck bis auf 1 bzw.
3 Pixel an zwei Ecken, Dossier 3.9).

    python re15_port/tools/irons_tisch_engine_export.py
"""
import hashlib
import io
import os
import struct
import sys

import numpy as np
import cv2
from PIL import Image

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HIER))
CD = os.path.join(REPO, "re15_port", "shared_assets", "PSX")
RE2 = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM10E0.RDT")
ZIEL = os.path.join(REPO, "re15_port", "engine", "src", "gen", "irons_tisch_props.inc")

QUELLEN = {
    # name: (datei, offset, laenge, md5)
    "diary_md1": (RE2, 0x002320, 372, "cf9f316dd6ba0ecf599bea18a83c36d1"),
    "diary_tim": (RE2, 0x015224, 17440, "63bd93f2be1cc97066aa09afd3c67e4d"),
    "karte_md1": (os.path.join(CD, "STAGE1", "ROOM1110.RDT"), 0x0013D8, 156,
                  "93975479cdd85aa9e8e4232f6c8c8983"),
    "keycard_tim": (os.path.join(CD, "STAGE1", "ROOM1110.RDT"), 0x02BAC8, 8736,
                    "5ce475a418189da5baf3ecd1c81db83c"),
}
KARTE_TIM_MD5 = "885d91614f6ff1378cc0ea41328810b8"

ITEM_MEMCARD = 0x21          # DEBUG.BIN Namenstabelle @0x499E -> @0x4BEA "Memory Card"
ITPS_BLOCK = 0x3000          # Lader LAB_8001e404 `ori v0,zero,0x3000` @0x8001e414
U_MAX, V_MAX = 102, 63       # UV-Feld des Keycard-MD1 (aus den UV-Saetzen gelesen)


def md5(b):
    return hashlib.md5(b).hexdigest()


def quelle(name):
    datei, off, n, soll = QUELLEN[name]
    d = open(datei, "rb").read()
    b = d[off:off + n]
    ist = md5(b)
    if len(b) != n or ist != soll:
        sys.exit("ABBRUCH: %s (%s @0x%X, %d B) md5 %s statt %s"
                 % (name, datei, off, len(b), ist, soll))
    return b


def itps_lesen(item):
    d = open(os.path.join(CD, "ITEM", "ITPS.ITP"), "rb").read()
    o = item * ITPS_BLOCK
    cl_len = struct.unpack_from("<I", d, o + 8)[0]
    clut = struct.unpack_from("<256H", d, o + 20)
    p = o + 8 + cl_len
    _il, _ix, _iy, iw, ih = struct.unpack_from("<IHHHH", d, p)
    w = iw * 2
    pix = np.frombuffer(d, np.uint8, w * ih, p + 12).reshape(ih, w)
    rgb = np.zeros((ih, w, 3), np.uint8)
    for y in range(ih):
        for x in range(w):
            c = clut[pix[y, x]]
            rgb[y, x] = ((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3)
    return rgb


def deckflaeche(rgb, schwelle=70):
    """Viereck der Karten-Deckflaeche: Hintergrund blau (B > R+20), Seitenflaeche dunkel."""
    R = rgb[:, :, 0].astype(int)
    B = rgb[:, :, 2].astype(int)
    L = rgb.astype(int).sum(axis=2) / 3.0
    karte = ~(B > R + 20)
    karte[:3, :] = False
    karte[-3:, :] = False
    karte[:, :3] = False
    karte[:, -3:] = False
    m = (karte & (L >= schwelle)).astype(np.uint8)
    _n, lab, st, _ = cv2.connectedComponentsWithStats(m, 8)
    k = 1 + int(np.argmax(st[1:, cv2.CC_STAT_AREA]))
    m = (lab == k).astype(np.uint8)
    cs, _ = cv2.findContours(m, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    aussen = max(cs, key=cv2.contourArea)
    hull = cv2.convexHull(aussen)
    m = np.zeros_like(m)
    cv2.drawContours(m, [aussen], -1, 1, thickness=cv2.FILLED)
    q = None
    for eps in np.arange(1.0, 12.0, 0.25):
        a = cv2.approxPolyDP(hull, eps, True)
        if len(a) == 4:
            q = a.reshape(4, 2).astype(float)
            break
    if q is None:
        sys.exit("ABBRUCH: kein Viereck in der Deckflaeche gefunden")
    return q, m


def ecken_ordnen(q):
    q = sorted(q.tolist(), key=lambda p: p[1])
    oben = sorted(q[:2], key=lambda p: p[0])
    unten = sorted(q[2:], key=lambda p: p[0])
    return oben[0], oben[1], unten[1], unten[0]


def karten_tim_bauen(ktim):
    rgb = itps_lesen(ITEM_MEMCARD)
    q, maske = deckflaeche(rgb, 70)
    ol, orr, ur, ul = ecken_ordnen(q)
    # (u=U_MAX,v=0)=Logo-Ende links, (U_MAX,V_MAX)=Logo rechts, (0,V_MAX)=SONY rechts,
    # (0,0)=SONY links: bei rot_y 3072 liegt die Karte fuer den Spieler vor dem Tisch
    # aufrecht (Dossier 2.4/3.9).
    ziel = np.array([[U_MAX, 0], [U_MAX, V_MAX], [0, V_MAX], [0, 0]], np.float32)
    quelle_pts = np.array([ol, orr, ur, ul], np.float32)
    hm = cv2.getPerspectiveTransform(ziel, quelle_pts)
    tex = np.zeros((64, 128, 3), np.uint8)
    alpha = np.zeros((64, 128), bool)
    for v in range(V_MAX + 1):
        for u in range(U_MAX + 1):
            p = hm @ np.array([u + 0.5, v + 0.5, 1.0])
            x = p[0] / p[2] - 0.5
            y = p[1] / p[2] - 0.5
            x0 = int(np.floor(x))
            y0 = int(np.floor(y))
            fx = x - x0
            fy = y - y0
            if x0 < 0 or y0 < 0 or x0 + 1 >= rgb.shape[1] or y0 + 1 >= rgb.shape[0]:
                continue
            xi = int(round(x))
            yi = int(round(y))
            if not maske[yi, xi]:
                continue
            c = (rgb[y0, x0] * (1 - fx) * (1 - fy) + rgb[y0, x0 + 1] * fx * (1 - fy)
                 + rgb[y0 + 1, x0] * (1 - fx) * fy + rgb[y0 + 1, x0 + 1] * fx * fy)
            tex[v, u] = np.clip(np.round(c), 0, 255)
            alpha[v, u] = True
    im = Image.fromarray(tex).quantize(colors=255, method=Image.MEDIANCUT, dither=Image.NONE)
    pal = im.getpalette()[:255 * 3]
    idx = np.array(im, np.uint8) + 1
    idx[~alpha] = 0                              # CLUT-Index 0 = PSX-Farbschluessel
    clut = [0x0000]
    for i in range(255):
        r, g, b = pal[3 * i] >> 3, pal[3 * i + 1] >> 3, pal[3 * i + 2] >> 3
        c = r | (g << 5) | (b << 10)
        if c == 0:
            c = 0x0400                           # nie 0x0000 ausser Index 0
        clut.append(c)
    cl_len = struct.unpack_from("<I", ktim, 8)[0]
    p = 8 + cl_len
    tim = bytes(ktim[:20]) + struct.pack("<256H", *clut) + bytes(ktim[p:p + 12]) + idx.tobytes()
    if len(tim) != len(ktim):
        sys.exit("ABBRUCH: Karten-TIM %d B statt %d B" % (len(tim), len(ktim)))
    return tim, int(alpha.sum())


def carr(name, b, f):
    f.write("static const unsigned char %s[%d] = {" % (name, len(b)))
    for i, x in enumerate(b):
        f.write(("\n    " if i % 16 == 0 else "") + "0x%02x," % x)
    f.write("\n};\n\n")


def main():
    diary_md1 = quelle("diary_md1")
    diary_tim = quelle("diary_tim")
    karte_md1 = quelle("karte_md1")
    ktim = quelle("keycard_tim")
    karte_tim, belegt = karten_tim_bauen(ktim)
    if md5(karte_tim) != KARTE_TIM_MD5:
        sys.exit("ABBRUCH: Karten-TIM md5 %s statt %s (andere numpy/cv2/PIL-Fassung?)"
                 % (md5(karte_tim), KARTE_TIM_MD5))
    print("Dokument MD1 %d B / TIM %d B, Karte MD1 %d B / TIM %d B (%d Texel belegt), alle md5 ok"
          % (len(diary_md1), len(diary_tim), len(karte_md1), len(karte_tim), belegt))
    os.makedirs(os.path.dirname(ZIEL), exist_ok=True)
    with io.open(ZIEL, "w", encoding="utf-8", newline="\n") as f:
        f.write("/* GENERIERT von re15_port/tools/irons_tisch_engine_export.py — NICHT HAND-EDITIEREN.\n"
                " *\n"
                " * Die zwei Welt-Props auf Irons' Schreibtisch (ROOM1150/1151), Runde 30 Thema E2.\n"
                " * Dossier: analysis/befunde_runde30/irons-diary-welt.md.\n"
                " *\n"
                " *   re15_irons_diary_md1  RE2 ROOM10E0.RDT @0x002320 (372 B, md5 cf9f316d...), unveraendert\n"
                " *   re15_irons_diary_tim  RE2 ROOM10E0.RDT @0x015224 (17440 B, md5 63bd93f2...), unveraendert\n"
                " *   re15_irons_karte_md1  RE1.5 ROOM1110.RDT @0x0013D8 (156 B, md5 93975479...), Keycard,\n"
                " *                         unveraendert\n"
                " *   re15_irons_karte_tim  KONSTRUIERT aus ITPS.ITP @0x63000 (Item-Bild 0x21), Kopf der\n"
                " *                         Keycard-TIM ROOM1110.RDT @0x02BAC8; md5 885d9161...\n"
                " */\n\n")
        carr("re15_irons_diary_md1", diary_md1, f)
        carr("re15_irons_diary_tim", diary_tim, f)
        carr("re15_irons_karte_md1", karte_md1, f)
        carr("re15_irons_karte_tim", karte_tim, f)
    print("geschrieben:", ZIEL)


if __name__ == "__main__":
    main()
