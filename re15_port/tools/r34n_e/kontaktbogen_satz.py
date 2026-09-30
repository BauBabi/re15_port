# -*- coding: utf-8 -*-
"""Runde 34 Nacht, Spur E: Kontaktbogen der gesetzten Seiten FILE26..FILE29 im RE2-Schirmlayout.

Liest die GESCHRIEBENEN TIM-Dateien (nicht das Protokoll des Satzwerkzeugs) und zeichnet jede
Leserseite so, wie RE2s Leser sie zeigt: Grund schwarz, Illustration (title_paper ab v = H) bei
(100,60), Textseite bei (25,30) (RE2 @0x80071d8c-94, @0x80072584-94, @0x80076170-84; Dossier
irons-diary-dokument.md 3.4). Dazu je Dokument das Weltmodell (Render aus
extracted_re2_dokumente/weltmodelle/*_a.png) neben dem Papier - fuer die Frage "passen Modell
und Papier zusammen?".

Aufruf: python re15_port/tools/r34n_e/kontaktbogen_satz.py <satz-wurzel> <ausgabe.png> [--skala 1]
"""
import argparse
import os
import struct

from PIL import Image, ImageDraw

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
WELT = os.path.join(REPO, "extracted_re2_dokumente", "weltmodelle")

DOKS = [  # (Bildsatz, Weltmodell-Render, Beschriftung)
    (26, "mesh00_0541704e_a.png", "Dok 1 FILE26 Police Officer (Papier FILE00, Modell mesh00)"),
    (27, "mesh03_cf9f316d_a.png", "Dok 2 FILE27 Elliot's Diary (Papier FILE08, Modell mesh03)"),
    (28, "mesh01_ae2d0a30_a.png", "Dok 3 FILE28 Marvin's Notes (Papier FILE02, Modell mesh01)"),
    (29, "mesh04_cab7b32d_a.png", "Dok 4 FILE29 Armory Notice (Papier FILE06, Modell mesh04)"),
]


def tim(pfad):
    b = open(pfad, "rb").read()
    magic, flags = struct.unpack_from("<II", b, 0)
    assert magic == 0x10, pfad
    o = 8
    clen, cx, cy, cw, ch = struct.unpack_from("<IHHHH", b, o)
    clut = list(struct.unpack_from("<%dH" % (cw * ch), b, o + 12))
    o += clen
    ilen, ix, iy, iw, ih = struct.unpack_from("<IHHHH", b, o)
    d = b[o + 12:o + ilen]
    bpp = 4 if (flags & 7) == 0 else 8
    W = iw * 4 if bpp == 4 else iw * 2
    px = []
    for y in range(ih):
        if bpp == 4:
            px.append([(d[y * (W // 2) + x // 2] >> (4 * (x & 1))) & 15 for x in range(W)])
        else:
            px.append(list(d[y * W:(y + 1) * W]))
    return dict(W=W, H=ih, px=px, clut=clut)


def rgb(c):
    return ((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3)


def schirm(seite, papier):
    s = Image.new("RGB", (320, 240), (0, 0, 0))
    H = seite["H"]
    for v in range(256 - H):
        for u in range(128):
            c = papier["clut"][papier["px"][H + v][u]]
            if c != 0:                               # PSX: Farbe 0x0000 = durchsichtig
                s.putpixel((100 + u, 60 + v), rgb(c))
    for y in range(H):
        for x in range(seite["W"]):
            v = seite["px"][y][x]
            if v and seite["clut"][v] != 0 and 0 <= 25 + x < 320 and 0 <= 30 + y < 240:
                s.putpixel((25 + x, 30 + y), rgb(seite["clut"][v]))
    return s


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("wurzel")
    ap.add_argument("aus")
    ap.add_argument("--skala", type=int, default=1)
    a = ap.parse_args()
    reihen = []
    for doc, welt, text in DOKS:
        d = os.path.join(a.wurzel, "FILE%02d" % doc)
        papier = tim(os.path.join(d, "FILE%02d_title_paper.TIM" % doc))
        bilder = [schirm(tim(os.path.join(d, "FILE%02d_title_page.TIM" % doc)), papier)]
        n = 1
        while os.path.exists(os.path.join(d, "FILE%02d_p%02d_page.TIM" % (doc, n))):
            bilder.append(schirm(tim(os.path.join(d, "FILE%02d_p%02d_page.TIM" % (doc, n))), papier))
            n += 1
        w = Image.open(os.path.join(WELT, welt)).convert("RGB").resize((240, 240))
        reihen.append((text + " - %d Leserseiten (Titel + %d), max_page %d" % (n, n - 1, n - 1), w, bilder))
    breit = max(240 + 8 + len(b) * 328 for _, _, b in reihen)
    hoch = sum(260 for _ in reihen)
    bogen = Image.new("RGB", (breit, hoch), (40, 40, 40))
    dr = ImageDraw.Draw(bogen)
    y = 0
    for text, w, bilder in reihen:
        dr.text((4, y + 2), text, fill=(255, 255, 0))
        bogen.paste(w, (0, y + 16))
        for i, b in enumerate(bilder):
            bogen.paste(b, (248 + i * 328, y + 16))
        y += 260
    if a.skala != 1:
        bogen = bogen.resize((bogen.width * a.skala, bogen.height * a.skala), Image.NEAREST)
    bogen.save(a.aus)
    print("Kontaktbogen %dx%d -> %s" % (bogen.width, bogen.height, a.aus))


if __name__ == "__main__":
    main()
