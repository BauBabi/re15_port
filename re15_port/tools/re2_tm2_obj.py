#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""re2_tm2_obj.py — liest ein RE2-"TM2" (PSX-TMD-Variante) und schreibt es als Wavefront
OBJ zum Ansehen. Gebaut fuer info/re2leon/COMMON/DATA/ST_FILE.TM2, das Geometrie-Stueck
des FILE-Bildschirms.

⛔ Format selbst nachgerechnet an ST_FILE.TM2 (372 Bytes); jede Zahl ist ein Datei-Offset:

  @0x00  u32 0x00000114  = Offset des UV-Blocks (= 0x0C + obj.w7, siehe unten)
  @0x04  u32 0x00000000  = flags (0 -> Zeiger sind RELATIV zum Ende des 3-Wort-Kopfes,
                           also zu 0x0C; das ist die TMD-Regel)
  @0x08  u32 0x00000002  = Anzahl Objekte
  @0x0C  Objekt 0, 7 Worte: {vert=0x38, n_vert=8, norm=0x78, n_norm=6,
                             prim=0xA8, n_prim=0, uv=0x108}
  @0x28  Objekt 1, 7 Worte: {vert=0x38, n_vert=8, norm=0x78, n_norm=6,
                             prim=0xA8, n_prim=6, uv=0x108}
         Beide Objekte zeigen auf DIESELBE Geometrie; Objekt 0 meldet 0 Primitive.
         Gegenprobe der Aufteilung: 0x0C + 2*28 = 0x44 = genau der Beginn der Vertices.
  @0x44  8 Vertices, je 4x s16 (x, y, z, pad):
         (0,270,-204) (0,-270,-204) (-72,270,-204) (-72,-270,-204)
         (0,270, 204) (0,-270, 204) (-72,270, 204) (-72,-270, 204)
         => ein Kasten: X 72 dick, Y 540 hoch, Z 408 tief
  @0x84  6 Normalen, je 4x s16 (±4096 = ±1.0 in 1.12-Fixkomma)
  @0xB4  6 Primitive, je 4 Paare u16 (normal_index, vertex_index) = 4 Ecken ->
         6 Vierecke mit je einer Flaechennormale (= die 6 Seiten des Kastens).
         Gegenprobe: 6*4*4 = 96 Bytes = 0xB4..0x113, luecklos bis zum UV-Block.
  @0x114 6 x 16 Bytes Texturdaten: (u0,v0,clut) (u1,v1,tpage) (u2,v2,-) (u3,v3,-)
         Gegenprobe: 6*16 = 96 Bytes = 0x114..0x173 = Dateiende (372 Bytes).

⛔ Es gibt in RE2 KEINE Geometrie je Dokument. Dieser Kasten ist der Anzeige-Koerper des
   FILE-Bildschirms (Textur: ST_FILE.TIM, 8bpp 128x256, 3 CLUTs). Das "Aussehen" eines
   Dokuments steckt in seinem vorgerenderten 8bpp-Bild aus FILES.TIM, nicht in Dreiecken.
"""
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
DEFAULT = os.path.join(REPO, "info", "re2leon", "COMMON", "DATA", "ST_FILE.TM2")


def parse(d):
    uv_top, flags, nobj = struct.unpack_from("<III", d, 0)
    assert flags == 0, "flags != 0 -> absolute Zeiger, hier nicht behandelt"
    base = 0x0C
    objs = []
    for i in range(nobj):
        o = base + i * 28
        objs.append(struct.unpack_from("<7I", d, o))
    assert base + nobj * 28 == 0x44, \
        "Objekt-Tabelle endet bei 0x%x, Vertices beginnen aber woanders" % (
            base + nobj * 28)
    return uv_top, flags, nobj, objs, base


def main():
    src = sys.argv[1] if len(sys.argv) > 1 else DEFAULT
    dst = sys.argv[2] if len(sys.argv) > 2 else \
        os.path.join(REPO, "extracted_re2_dokumente", "modelle", "ST_FILE.obj")
    d = open(src, "rb").read()
    uv_top, flags, nobj, objs, base = parse(d)
    print("%s: %d Bytes, %d Objekte, uv_top=0x%03X" % (
        os.path.basename(src), len(d), nobj, uv_top))
    lines = ["# %s -> OBJ, erzeugt von re15_port/tools/re2_tm2_obj.py" % os.path.basename(src),
             "# PSX-Koordinaten unveraendert (Y zeigt nach unten wie auf der PSX)."]
    vbase = 0
    faces = []
    for i, (vt, nv, nt, nn, pt, npr, uvt) in enumerate(objs):
        print("  Objekt %d: vert=0x%03X n=%d  norm=0x%03X n=%d  prim=0x%03X n=%d  uv=0x%03X"
              % (i, vt, nv, nt, nn, pt, npr, uvt))
        if npr == 0:
            continue
        vs = [struct.unpack_from("<4h", d, base + vt + k * 8)[0:3] for k in range(nv)]
        for v in vs:
            lines.append("v %d %d %d" % v)
        for k in range(npr):
            o = base + pt + k * 16
            pairs = [struct.unpack_from("<2H", d, o + j * 4) for j in range(4)]
            vi = [p[1] for p in pairs]
            ni = pairs[0][0]
            assert all(p[0] == ni for p in pairs), "Primitiv %d: Normalen weichen ab" % k
            assert all(v < nv for v in vi), "Primitiv %d: Vertex-Index ausserhalb" % k
            # PSX-Quad-Reihenfolge (0,1,3,2) -> OBJ-Ringfolge
            r = [vi[0], vi[1], vi[3], vi[2]]
            faces.append("f " + " ".join(str(vbase + x + 1) for x in r))
            u = [struct.unpack_from("<2B", d, uv_top + k * 16 + j * 4) for j in range(4)]
            print("    Viereck %d: normal %d, verts %s, uv %s"
                  % (k, ni, vi, u))
        vbase += nv
    lines += faces
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    open(dst, "w", newline="\n").write("\n".join(lines) + "\n")
    print("-> %s (%d Vertices, %d Vierecke)" % (dst, vbase, len(faces)))


if __name__ == "__main__":
    main()
