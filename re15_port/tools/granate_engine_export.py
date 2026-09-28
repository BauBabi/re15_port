#!/usr/bin/env python3
"""Exportiert das Welt-Modell der HANDGRANATE (Item 0x09) ins ENGINE-Format (MD1 + TIM) und
schreibt engine/src/gen/granate_prop.inc.

HERKUNFT — NICHTS ERFUNDEN (Runde 30, Nachtrag K, Dossier analysis/befunde_runde30/nachtrag-granate.md):
RE1.5 platziert die Hand Grenade nirgends (0 von 164 Item_aot_set in 206 RDTs), es gibt also kein
Raum-Prop einer Granate. Das einzige ausgelieferte Granatennetz ist das IN DER HAND:
    PLD/PL00W09.PLW  dir[2] = MD1 @0x5544..0x5F78  (Hand + Granate, 40 Flaechen, page 0x81/clut 0x7840)
                     dir[3] = TIM @0x5F78          (56x32 8bpp, CLUT (256,480) 256x1)
Das Netz teilt sich sauber in zwei Textur-Baender (Band-Regel main.c, WEAPON TEXTURE COMPOSITE,
byte-true FUN_80036b68 @0x80036c08): HAND v108..159 (Hautatlas) und WAFFE v224..255 (das dir[3]-Bild).
Genommen werden genau die WAFFEN-Flaechen (alle UV-v >= 200): 5 Dreiecke + 14 Vierecke, 24 Punkte,
kein Punkt mit der Hand geteilt (gemessen, plw_baender.txt).

FARBEN BYTE-TRUE: Das Original laedt aus dir[3] nur 16 CLUT-Eintraege an VRAM-x 224 der Zeile
481+acad5 (`ori v0,zero,0xe0` @0x80036c88, `addiu v0,v0,481` @0x80036c9c, Breite
`ori v0,zero,0x10` @0x80036ca8, LoadImage `jal 0x80068c88` @0x80036cb8); die Bildindizes sind
224..227. dir[3] traegt dieselben 16 Farben an 0..15 UND 224..239 (gemessen) - die volle CLUT
des dir[3] liefert fuer die Indizes 224..227 also dieselben Farben wie die Konsole.

UMBAU (nur Lage, keine Form):
  * Punkte und Normalen gedreht (x, y, z) -> (x, -z, y), dazu zentriert um die Mitte der Huelle
    (y 135..245 -> 190, z -23..87 -> 32). Die Drehung ist echt (Determinante +1), die
    Umlaufrichtung der Flaechen bleibt also erhalten. Folge: Laengsachse = Modell-X (wie die
    Sicherung, gen/sicherung_prop.inc), und die in der Hand OFFENE Seite (Handflaeche z = -23)
    zeigt nach +Y = UNTEN auf den Fachboden. Huelle danach x -77..77, y -55..55, z -55..55.
  * Flaechensaetze unveraendert (Z-Ordnung der Vierecke ist Originalbestand), Indizes nur
    auf die 24 Punkte / ihre Normalen umnummeriert.
  * UV-Werte unveraendert; page 0x0081 -> 0x0080 und clut 0x7840 -> 0x7800, weil die TIM des
    Props bei VRAM (0,0) / CLUT (0,480) liegt (wie alle Raum-Props, s. sicherung_engine_export).
  * TIM 128x256 8bpp wie die Props von ROOM1150; das 56x32-Bild von dir[3] liegt bei (72,224),
    genau dort, wo die UVs hinzeigen (u 72..127, v 224..255 = der Ort, an den der Port es auch
    in der Hand einkopiert: Slot-0-Texel (200,480) = page-Basis 128 + 72 / CLUT-Band 256 + 224).
    Die CLUT ist die von dir[3], Byte fuer Byte.

    python re15_port/tools/granate_engine_export.py
"""
import io
import os
import struct

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
PLW = os.path.join(REPO, "re15_port", "shared_assets", "PSX", "PLD", "PL00W09.PLW")
ZIEL = os.path.join(REPO, "re15_port", "engine", "src", "gen", "granate_prop.inc")

TEX_W, TEX_H = 128, 256          # TIM-Flaeche wie bei allen ROOM1150-Props
BILD_X, BILD_Y = 72, 224         # u_min / v_min des Waffen-Bands (s. Kopf)
CLUT_WORT = 0x7800               # (480 << 6) | 0
PAGE_WORT = 0x0080               # 8bpp, VRAM (0,0)
MITTE_Y, MITTE_Z = 190, 32       # Mitte der Waffen-Huelle y 135..245 / z -23..87
WAFFEN_BAND_V = 200              # alle UV-v >= 200 = Waffe (Hand-Band endet bei v 159)


def plw_teile(d):
    off, n = struct.unpack_from("<II", d, 0)
    dr = struct.unpack_from("<%dI" % n, d, off)
    return d[dr[2]:dr[3]], d[dr[3]:off]


def mesh_lesen(md1):
    base = 12
    (tv, tvc, tn, tnc, tf, tfc, tu,
     qv, qvc, qn, qnc, qf, qfc, qu) = struct.unpack_from("<14I", md1, 12)
    assert tv == qv and tn == qn, "erwartet geteilte Listen wie PL00W09"
    punkte = [struct.unpack_from("<4h", md1, base + tv + i * 8) for i in range(tvc)]
    normalen = [struct.unpack_from("<4h", md1, base + tn + i * 8) for i in range(tnc)]
    tris = [struct.unpack_from("<6H", md1, base + tf + i * 12) for i in range(tfc)]
    tuv = [md1[base + tu + i * 12: base + tu + i * 12 + 12] for i in range(tfc)]
    quads = [struct.unpack_from("<8H", md1, base + qf + i * 16) for i in range(qfc)]
    quv = [md1[base + qu + i * 16: base + qu + i * 16 + 16] for i in range(qfc)]
    return punkte, normalen, tris, tuv, quads, quv


def waffe(uvb, ecken):
    vs = [uvb[1 + 4 * k] for k in range(ecken)]
    return min(vs) >= WAFFEN_BAND_V


def uv_umsetzen(uvb, ecken):
    b = bytearray(uvb)
    struct.pack_into("<H", b, 2, CLUT_WORT)
    struct.pack_into("<H", b, 6, PAGE_WORT)
    return bytes(b)


def md1_bauen(md1):
    punkte, normalen, tris, tuv, quads, quv = mesh_lesen(md1)
    wt = [(t, u) for t, u in zip(tris, tuv) if waffe(u, 3)]
    wq = [(q, u) for q, u in zip(quads, quv) if waffe(u, 4)]
    pv = sorted({t[k] for t, _ in wt for k in (1, 3, 5)} | {q[k] for q, _ in wq for k in (1, 3, 5, 7)})
    pn = sorted({t[k] for t, _ in wt for k in (0, 2, 4)} | {q[k] for q, _ in wq for k in (0, 2, 4, 6)})
    mv = {a: i for i, a in enumerate(pv)}
    mn = {a: i for i, a in enumerate(pn)}

    def dreh_p(p):
        x, y, z = p[0], p[1] - MITTE_Y, p[2] - MITTE_Z
        return (x, -z, y)

    def dreh_n(n):
        return (n[0], -n[2], n[1])

    verts = [dreh_p(punkte[a]) for a in pv]
    norms = [dreh_n(normalen[a]) for a in pn]

    base, hdr_sz = 12, 56
    off_v = hdr_sz
    off_n = off_v + len(verts) * 8
    off_tf = off_n + len(norms) * 8
    off_tuv = off_tf + len(wt) * 12
    off_qf = off_tuv + len(wt) * 12
    off_quv = off_qf + len(wq) * 16
    ende = off_quv + len(wq) * 16
    d = bytearray(base + ende)
    struct.pack_into("<III", d, 0, 65, 0, 2)         # Kopf wie die Props von ROOM1150
    struct.pack_into("<14I", d, 12,
                     off_v, len(verts), off_n, len(norms), off_tf, len(wt), off_tuv,
                     off_v, len(verts), off_n, len(norms), off_qf, len(wq), off_quv)
    for i, p in enumerate(verts):
        struct.pack_into("<4h", d, base + off_v + i * 8, p[0], p[1], p[2], 0)
    for i, n in enumerate(norms):
        struct.pack_into("<4h", d, base + off_n + i * 8, n[0], n[1], n[2], 0)
    for i, (t, u) in enumerate(wt):
        struct.pack_into("<6H", d, base + off_tf + i * 12,
                         mn[t[0]], mv[t[1]], mn[t[2]], mv[t[3]], mn[t[4]], mv[t[5]])
        d[base + off_tuv + i * 12: base + off_tuv + i * 12 + 12] = uv_umsetzen(u, 3)
    for i, (q, u) in enumerate(wq):
        struct.pack_into("<8H", d, base + off_qf + i * 16,
                         mn[q[0]], mv[q[1]], mn[q[2]], mv[q[3]],
                         mn[q[4]], mv[q[5]], mn[q[6]], mv[q[7]])
        d[base + off_quv + i * 16: base + off_quv + i * 16 + 16] = uv_umsetzen(u, 4)
    return bytes(d), len(verts), len(norms), len(wt), len(wq), verts


def tim_bauen(tim):
    flag = struct.unpack_from("<I", tim, 4)[0]
    assert flag == 9, "erwartet 8bpp+CLUT"
    clen = struct.unpack_from("<I", tim, 8)[0]
    clut = tim[20:20 + 512]
    blen, bx, by, bw, bh = struct.unpack_from("<IHHHH", tim, 8 + clen)
    w, h = bw * 2, bh
    bild = tim[8 + clen + 12: 8 + clen + 12 + w * h]
    flaeche = bytearray(TEX_W * TEX_H)               # Rest Index 0 (wird nie gesampelt)
    for y in range(h):
        flaeche[(BILD_Y + y) * TEX_W + BILD_X: (BILD_Y + y) * TEX_W + BILD_X + w] = bild[y * w:(y + 1) * w]
    aus = bytearray()
    aus += struct.pack("<II", 0x10, 0x09)
    aus += struct.pack("<IHHHH", 12 + 512, 0, 480, 256, 1) + clut
    aus += struct.pack("<IHHHH", 12 + len(flaeche), 0, 0, TEX_W // 2, TEX_H) + bytes(flaeche)
    return bytes(aus), w, h


def carr(name, b, f):
    f.write("static const unsigned char %s[%d] = {" % (name, len(b)))
    for i, x in enumerate(b):
        f.write(("\n    " if i % 16 == 0 else "") + "0x%02x," % x)
    f.write("\n};\n\n")


def main():
    d = open(PLW, "rb").read()
    md1_roh, tim_roh = plw_teile(d)
    md1, nv, nn, nt, nq, verts = md1_bauen(md1_roh)
    tim, bw, bh = tim_bauen(tim_roh)
    xs = [p[0] for p in verts]; ys = [p[1] for p in verts]; zs = [p[2] for p in verts]
    os.makedirs(os.path.dirname(ZIEL), exist_ok=True)
    with io.open(ZIEL, "w", encoding="utf-8", newline="\n") as f:
        f.write("/* GENERIERT von re15_port/tools/granate_engine_export.py — NICHT HAND-EDITIEREN.\n"
                " *\n"
                " * Welt-Modell der HANDGRANATE (Item 0x09 \"Hand Grenade\") fuer den Hebetisch in\n"
                " * Irons' Buero (ROOM1150/1151 Prop 0, sub04). Quelle: PLD/PL00W09.PLW, die 19\n"
                " * Waffen-Flaechen aus dir[2] (@0x5544) und das Bild aus dir[3] (@0x5F78) —\n"
                " * Herleitung im Kopf des Werkzeugs und in analysis/befunde_runde30/nachtrag-granate.md.\n"
                " *\n"
                " * MD1: %d Punkte, %d Normalen, %d Dreiecke, %d Vierecke; Huelle x %d..%d y %d..%d z %d..%d\n"
                " *      (Laengsachse X, offene Handseite nach +Y = unten)\n"
                " * TIM: 128x256 8bpp, CLUT (0,480) = dir[3]-CLUT, dir[3]-Bild %dx%d bei (%d,%d)\n"
                " */\n\n" % (nv, nn, nt, nq, min(xs), max(xs), min(ys), max(ys), min(zs), max(zs),
                             bw, bh, BILD_X, BILD_Y))
        carr("re15_granate_md1", md1, f)
        carr("re15_granate_tim", tim, f)
    print("geschrieben: %s  (MD1 %d B: %d Punkte, %d Normalen, %d Dreiecke, %d Vierecke; TIM %d B)"
          % (os.path.relpath(ZIEL, REPO), len(md1), nv, nn, nt, nq, len(tim)))
    print("Huelle x %d..%d y %d..%d z %d..%d" % (min(xs), max(xs), min(ys), max(ys), min(zs), max(zs)))


if __name__ == "__main__":
    main()
