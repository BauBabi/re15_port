#!/usr/bin/env python3
"""Exportiert das Sicherungs-Modell ins ENGINE-Format (MD1 + TIM) und schreibt es
als C-Einbindung engine/src/gen/sicherung_prop.inc.

WARUM ALS EINGEBACKENE BYTES UND NICHT ALS ASSET-PATCH:
Der Port fasst die ausgelieferten RDTs NICHT an (scd_room_setup.c:342 "KEIN
Asset-Patch — die RDTs bleiben byte-true"). Die Sicherung ist neuer Inhalt, also
kommt sie als zusaetzliches Prop portseitig dazu: ROOM1150/1151 hat nOmodel=4, der
Prop-Pool fasst 17 (RE15_SCD_MAX_PROPS), also ist Index 4 frei — samt Texturslot
RE15_TIM_SLOT_PROP(4) = 8.

FORMAT 1:1 WIE DIE NACHBAR-PROPS DESSELBEN RAUMS (gemessen an ROOM1150 Prop 0..3):
  MD1-Kopf            41 00 00 00 | 00 00 00 00 | 02 00 00 00  (len=65, unk=0, obj=2)
  ein Mesh, tv==qv und tn==qn (geteilte Listen) — genau wie alle vier Originale
  Normalen auf Laenge 4096 normiert (Q12; gemessen: min=max=Mittel=4096)
  TIM flag=0x09 (8bpp+CLUT), CLUT 256 Eintraege @VRAM(0,480), Bild 128x256 @VRAM(0,0)
  UV-Woerter: clut=0x7800 ((480<<6)|0), page=0x80 (8bpp, VRAM 0,0)

⛔ HELLIGKEIT — WARUM DIE TEXTUR AUFGEHELLT WIRD:
Die Pixel stammen aus dem NACHT-Hintergrund von ROOM1050 und tragen dessen
Szenenbeleuchtung. Der Port beleuchtet Props aber selbst (re15_light_shade_vertex),
die Textur muss also die EIGENFARBE tragen, sonst ist die Sicherung in Irons' Buero
schwarz. Der Faktor ist GEMESSEN, nicht gewaehlt: hellstes Prozent der Prop-0-Textur
desselben Raums (Papierstapel, weisses Papier) = 738 von 765; hellstes Prozent der
Sicherungstextur = 257. 738/257 = 2.87.

⛔ CLUT-INDEX 0 BLEIBT FREI: Der PSX-Farbschluessel zeichnet Texel mit Index 0 und
CLUT[0]=0x0000 NICHT (Herleitung in main.c:1285-1305). Ein Bild, das Index 0 benutzt,
bekaeme Loecher — deshalb wird auf 255 Farben quantisiert und Index 0 leer gelassen.

    python re15_port/tools/sicherung_engine_export.py
"""
import io
import os
import struct
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sicherung_modell import (modell_bauen, textur_bauen, SEITEN,   # noqa: E402
                              L_KAPPE_U, L_KOERPER, L_KAPPE_O)

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
ZIEL = os.path.join(REPO, "re15_port", "engine", "src", "gen", "sicherung_prop.inc")

TEX_W, TEX_H = 128, 256      # TIM-Flaeche wie bei allen ROOM1150-Props
CLUT_WORT = 0x7800           # (480 << 6) | 0
PAGE_WORT = 0x0080           # 8bpp, VRAM (0,0)
HELLIGKEIT = 2.87            # 738 / 257, s. Kopf


def tim_bauen():
    """128x256 8bpp + 256er-CLUT. Die 32x128-Mantelflaeche liegt oben links."""
    tex, _ = textur_bauen()
    a = np.asarray(tex, np.float32) * HELLIGKEIT
    tex = Image.fromarray(np.clip(a, 0, 255).astype(np.uint8))

    flaeche = Image.new("RGB", (TEX_W, TEX_H), (0, 0, 0))
    flaeche.paste(tex, (0, 0))
    # 255 Farben: Index 0 bleibt fuer den Farbschluessel reserviert
    pal = flaeche.convert("P", palette=Image.ADAPTIVE, colors=255)
    idx = np.asarray(pal, np.uint8).astype(np.int32) + 1      # 1..255
    roh = np.asarray(pal.getpalette()[:255 * 3], np.uint8).reshape(255, 3)

    clut = [0x0000]                                            # Index 0 = Schluesselfarbe
    for r, g, b in roh:
        clut.append(((b >> 3) << 10) | ((g >> 3) << 5) | (r >> 3))
    while len(clut) < 256:
        clut.append(0)

    aus = bytearray()
    aus += struct.pack("<II", 0x10, 0x09)                      # magic, flag 8bpp+CLUT
    clutblock = struct.pack("<IHHHH", 12 + 512, 0, 480, 256, 1) + b"".join(
        struct.pack("<H", c) for c in clut)
    aus += clutblock
    bild = idx.astype(np.uint8).tobytes()                      # 128*256 Bytes
    aus += struct.pack("<IHHHH", 12 + len(bild), 0, 0, TEX_W // 2, TEX_H) + bild
    return bytes(aus)


def md1_bauen():
    """Ein Mesh, tv==qv / tn==qn wie die Originale. Y auf PSX gedreht, und das
    Modell LIEGEND + zentriert (Begruendung unten im Rumpf)."""
    v, vt, faces = modell_bauen()

    # PSX: +Y zeigt nach unten -> Y spiegeln.
    # ⛔ UND: fuer die Engine wird das Modell LIEGEND und ZENTRIERT exportiert.
    # Grund ist gemessen, nicht Geschmack: der freie Streifen auf der Deckflaeche des
    # Hebetisches ist nur 172 Einheiten tief (z 698..870, zwischen den Papierstapeln),
    # die Sicherung ist 406 lang — sie passt dort nur QUER. Das ueber Prop-Rotationen
    # zu loesen waere schwer kontrollierbar (Rotationsreihenfolge + Ursprung am Fuss);
    # liegend exportiert ist die Prop-Position schlicht die MITTE des Gegenstands.
    #   Laengsachse -> X (zentriert), Querschnitt -> (Y, Z).
    halb = (L_KAPPE_U + L_KOERPER + L_KAPPE_O) / 2.0
    verts = [(int(round(p[1] - halb)), int(round(p[0])), int(round(p[2]))) for p in v]

    # Vertex-Normalen aus den anliegenden Flaechen, auf 4096 normiert (Q12).
    acc = [np.zeros(3) for _ in verts]
    for fc in faces:
        ids = [a - 1 for a, _ in fc]
        p = [np.array(verts[i], float) for i in ids]
        n = np.cross(p[1] - p[0], p[2] - p[0])
        ln = np.linalg.norm(n)
        if ln < 1e-6:
            continue
        n /= ln
        for i in ids:
            acc[i] += n
    norms = []
    for n in acc:
        ln = np.linalg.norm(n)
        n = (n / ln) if ln > 1e-6 else np.array([0.0, -1.0, 0.0])
        norms.append(tuple(int(round(c * 4096)) for c in n))

    def uv(ti):
        u, vv = vt[ti - 1]
        return (min(255, max(0, int(round(u * 31)))),
                min(255, max(0, int(round((1.0 - vv) * 127)))))

    tris = [f for f in faces if len(f) == 3]
    quads = [f for f in faces if len(f) == 4]

    base = 12
    hdr_sz = 56
    off_v = hdr_sz                                   # relativ zu base
    off_n = off_v + len(verts) * 8
    off_tf = off_n + len(norms) * 8
    off_tuv = off_tf + len(tris) * 12
    off_qf = off_tuv + len(tris) * 12
    off_quv = off_qf + len(quads) * 16
    ende = off_quv + len(quads) * 16

    d = bytearray(base + ende)
    struct.pack_into("<III", d, 0, 65, 0, 2)         # wie die vier Originale
    struct.pack_into("<14I", d, 12,
                     off_v, len(verts), off_n, len(norms), off_tf, len(tris), off_tuv,
                     off_v, len(verts), off_n, len(norms), off_qf, len(quads), off_quv)
    for i, p in enumerate(verts):
        struct.pack_into("<4h", d, base + off_v + i * 8, p[0], p[1], p[2], 0)
    for i, n in enumerate(norms):
        struct.pack_into("<4h", d, base + off_n + i * 8, n[0], n[1], n[2], 0)
    for i, f in enumerate(tris):
        a, b, c = [x[0] - 1 for x in f]
        struct.pack_into("<6H", d, base + off_tf + i * 12, a, a, b, b, c, c)
        (u0, v0), (u1, v1), (u2, v2) = (uv(f[0][1]), uv(f[1][1]), uv(f[2][1]))
        struct.pack_into("<BBHBBHBBH", d, base + off_tuv + i * 12,
                         u0, v0, CLUT_WORT, u1, v1, PAGE_WORT, u2, v2, 0)
    for i, f in enumerate(quads):
        a, b, c, e = [x[0] - 1 for x in f]
        struct.pack_into("<8H", d, base + off_qf + i * 16, a, a, b, b, c, c, e, e)
        (u0, v0), (u1, v1), (u2, v2), (u3, v3) = (uv(f[0][1]), uv(f[1][1]),
                                                  uv(f[2][1]), uv(f[3][1]))
        struct.pack_into("<BBHBBHBBHBBH", d, base + off_quv + i * 16,
                         u0, v0, CLUT_WORT, u1, v1, PAGE_WORT, u2, v2, 0, u3, v3, 0)
    return bytes(d), len(verts), len(tris), len(quads)


def carr(name, b, f):
    f.write("static const unsigned char %s[%d] = {" % (name, len(b)))
    for i, x in enumerate(b):
        f.write(("\n    " if i % 16 == 0 else "") + "0x%02x," % x)
    f.write("\n};\n\n")


def main():
    md1, nv, nt, nq = md1_bauen()
    tim = tim_bauen()
    os.makedirs(os.path.dirname(ZIEL), exist_ok=True)
    with io.open(ZIEL, "w", encoding="utf-8") as f:
        f.write("/* GENERIERT von re15_port/tools/sicherung_engine_export.py — NICHT HAND-EDITIEREN.\n"
                " *\n"
                " * Welt-Modell der SICHERUNG (Item 0x40 \"Fuse\") fuer den Hebetisch in Irons'\n"
                " * Buero (ROOM1150/1151 Prop 0, sub04). Geometrie und Textur stammen aus den\n"
                " * ausgelieferten Hintergruenden ROOM1050 BG07/BG08 — Herleitung, Massstab und\n"
                " * Abnahme stehen in sicherung_modell.py / sicherung_abnahme.py.\n"
                " *\n"
                " * Format 1:1 wie die vier Original-Props desselben Raums: MD1-Kopf 65/0/2, ein\n"
                " * Mesh mit geteilten V/N-Listen, Normalen auf 4096 (Q12); TIM 8bpp+CLUT,\n"
                " * Bild 128x256 @VRAM(0,0), CLUT @VRAM(0,480), UV-clut 0x7800 / page 0x80.\n"
                " * Texturhelligkeit x%.2f = Eigenfarbe (der Port beleuchtet Props selbst).\n"
                " * CLUT-Index 0 bleibt frei (PSX-Farbschluessel).\n"
                " *\n"
                " * %d Punkte, %d Dreiecke, %d Vierecke.\n"
                " */\n\n" % (HELLIGKEIT, nv, nt, nq))
        carr("re15_sicherung_md1", md1, f)
        carr("re15_sicherung_tim", tim, f)
    print("MD1 %d B (%d Punkte, %d Tri, %d Quad), TIM %d B" % (len(md1), nv, nt, nq, len(tim)))
    print("geschrieben:", os.path.relpath(ZIEL, REPO))


if __name__ == "__main__":
    main()
