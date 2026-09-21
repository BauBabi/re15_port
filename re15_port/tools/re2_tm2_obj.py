#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""re2_tm2_obj.py — liest die RE2-"TM2"-Geometrie (Capcom-Variante des PSX-TMD) und
schreibt sie als Wavefront-OBJ zum Ansehen. Behandelt beide Traeger:

  info/re2leon/COMMON/DATA/ST_FILE.TM2   372 B — reine Geometrie, EIN Block
  info/re2leon/COMMON/DATA/ST_FILE.TS  21752 B — TIM + ZWEI Geometrieblocks;
                                       das ist die Datei, die das Spiel wirklich laedt

⛔ WELCHE DATEI DAS SPIEL BENUTZT — wichtig, weil der Name in die Irre fuehrt:
Der Speicherblock heisst zwar "FILE TIM TM2" (String @0x80011C20), geladen wird damit
aber die **.TS**, nicht .TIM+.TM2:
    8006C75C  li    a0,0xdf                     ; CD-Datei-Id 223 = ST_FILE.TS
    8006C768  addiu a3=>s_FILE_TIM_TM2_80011c20,a3,0x1c20
    8006C76C  jal   FUN_80012FB8                ; Ziel 0x801A0000
Dieselbe Id 0xdf auch beim Nachladen @0x8006D11C und @0x8006D21C.
`ST_FILE.TM2` ist ein auf der Disc liegengebliebenes Build-Zwischenprodukt: seine
8 Vertices sind byte-identisch die ersten 8 der 36 von TS-Block A (hier per `assert`
geprueft), und keine Stelle der EXE laedt die Id 221/222 (ST_FILE.TIM/.TM2).

⛔ FORMAT — am Decompilat von `get_tmd_addr` festgemacht (die Funktion mit den
PsyQ-Debug-Strings "analizing TMD..." @0x800126E8 und "\tid=%08X, flags=%d, nobj=%d,
objid=%d" @0x800126FC):
      piVar2   = param_1 + param_2*7 + 3;          // 7 Worte je Objekt, ab +0x0C
      *param_4 = (int)param_1 + *piVar2   + 0xc;   // Vertices
      *param_5 = (int)param_1 + piVar2[2] + 0xc;   // Normalen
      *param_3 = (int)param_1 + piVar2[4] + 0xc;   // Index-Array
      return     piVar2[5];                        // Anzahl Primitive
Also je Block:
  +0x00  u32  Offset des Paketbereichs (= 0x0C + obj[0].pkt)
  +0x04  u32  flags = 0  -> alle Zeiger relativ zu +0x0C
  +0x08  u32  nobj
  +0x0C  nobj x 7 Worte {vert, n_vert, norm, n_norm, idx, n_prim, pkt}
Das 7. Wort ist NICHT `scale` wie im PsyQ-TMD, sondern der Zeiger auf die
GPU-Paket-Vorlagen. RE2 haelt Index und Paket in zwei parallelen Arrays statt in
einem verschraenkten TMD-Primitiv-Record.

⛔ GERADE Objekt-Nummer = Dreiecke, UNGERADE = Vierecke. Festgeschrieben in
`FUN_80076B60`: `iVar2 = (uVar8 & 1) + 3;` (Schrittweite 3 oder 4 Worte je Paket,
`piVar6 += 7` je Objekt-Deskriptor). Dieselbe Paritaet gilt fuer das Index-Array:
3 bzw. 4 Paare `(normal_index, vertex_index)` je Primitiv.

Gegenprobe, die das Format erzwingt (laeuft hier als `assert`, alle drei Blocks gruen):
  ST_FILE.TM2  0 Dreiecke + 6 Vierecke; Index 0xB4..0x113 = 6*16 B; Paket 0x114..0x173
               = 6*16 B = Dateiende (372 B). Kein unerklaertes Byte.
  TS Block A   3 Dreiecke + 29 Vierecke; Index-Abstand obj1-obj0 = 0x27C-0x258 = 3*12 B;
               Paket 3*12 + 29*16 = 500 B = 0x64C-0x458. 36 Vertices, 32 Normalen.
  TS Block B   0 Dreiecke + 1 Viereck; 4 Vertices, 1 Normale; 16 B Paket.
  Und 0x4E20 + 0x64C + 0x8C = 0x54F8 = 21752 = Dateilaenge von ST_FILE.TS.

⛔ Es gibt in RE2 KEINE Geometrie je Dokument. Diese Koerper sind das Moebelstueck des
FILE-Bildschirms: Block A die drei Reihen/Regale, Block B die Karteikarte, die 24-mal
im Karussell steht. Das Aussehen eines Dokuments steckt in seinem vorgerenderten
8bpp-Bild aus FILES.TIM (siehe re2_dokumente_extrakt.py), nicht in Dreiecken.
"""
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
DATA = os.path.join(REPO, "info", "re2leon", "COMMON", "DATA")
OUTDIR = os.path.join(REPO, "extracted_re2_dokumente", "modelle")

# (Datei, Blockname, Basis-Offset im File)
BLOCKS = [
    ("ST_FILE.TM2", "TM2", 0x0000),
    ("ST_FILE.TS", "TS_BlockA", 0x4E20),
    ("ST_FILE.TS", "TS_BlockB", 0x4E20 + 0x64C),
]


def parse_block(d, base):
    pkt_top, flags, nobj = struct.unpack_from("<III", d, base)
    assert flags == 0, "flags=%d != 0 (absolute Zeiger werden hier nicht behandelt)" % flags
    tab = base + 0x0C
    objs = [struct.unpack_from("<7I", d, tab + i * 28) for i in range(nobj)]
    assert pkt_top == 0x0C + objs[0][6], \
        "word0 0x%X passt nicht zu 0x0C+obj0.pkt 0x%X" % (pkt_top, 0x0C + objs[0][6])
    assert tab + nobj * 28 == base + 0x0C + objs[0][0], \
        "Objekttabelle endet nicht dort, wo die Vertices beginnen"
    return pkt_top, nobj, objs, tab


def dump(path, name, base, outdir):
    d = open(path, "rb").read()
    pkt_top, nobj, objs, tab = parse_block(d, base)
    print("\n%-10s @0x%05X  word0=0x%03X  nobj=%d" % (name, base, pkt_top, nobj))
    nv = objs[0][1]
    vs = [struct.unpack_from("<4h", d, tab + objs[0][0] + k * 8)[0:3] for k in range(nv)]
    lines = ["# %s, Block %s -> OBJ (re15_port/tools/re2_tm2_obj.py)" % (
        os.path.basename(path), name),
        "# PSX-Koordinaten unveraendert (Y zeigt nach unten wie auf der PSX)."]
    for v in vs:
        lines.append("v %d %d %d" % v)
    n_tri = n_quad = 0
    for i, (vt, n_v, nt, n_n, it, npr, pt) in enumerate(objs):
        ecken = 3 if (i & 1) == 0 else 4          # FUN_80076B60: (obj & 1) + 3
        art = "Dreiecke" if ecken == 3 else "Vierecke"
        print("  obj%d vert=0x%03X n=%2d  norm=0x%03X n=%2d  idx=0x%03X n_prim=%2d "
              "pkt=0x%03X  -> %2d %s" % (i, vt, n_v, nt, n_n, it, npr, pt, npr, art))
        assert n_v == nv, "Objekte teilen sich die Vertices nicht"
        for k in range(npr):
            o = tab + it + k * ecken * 4
            paare = [struct.unpack_from("<2H", d, o + j * 4) for j in range(ecken)]
            vi = [p[1] for p in paare]
            assert all(x < nv for x in vi), "obj%d prim%d: Vertex-Index ausserhalb" % (i, k)
            assert all(p[0] < n_n for p in paare), \
                "obj%d prim%d: Normalen-Index ausserhalb" % (i, k)
            # PSX-Reihenfolge 0,1,3,2 -> OBJ-Ringfolge; Dreieck bleibt 0,1,2
            r = [vi[0], vi[1], vi[3], vi[2]] if ecken == 4 else vi
            lines.append("f " + " ".join(str(x + 1) for x in r))
        n_tri += npr if ecken == 3 else 0
        n_quad += npr if ecken == 4 else 0
    # Lueckenlosigkeit des Paketbereichs
    pkt_bytes = n_tri * 12 + n_quad * 16
    print("  %d Vertices, %d Normalen, %d Dreiecke + %d Vierecke, %d B Pakete"
          % (nv, objs[0][3], n_tri, n_quad, pkt_bytes))
    os.makedirs(outdir, exist_ok=True)
    dst = os.path.join(outdir, "%s.obj" % name)
    open(dst, "w", newline="\n").write("\n".join(lines) + "\n")
    print("  -> %s" % dst)
    return vs, pkt_bytes, tab + pkt_top - 0x0C


def main():
    outdir = sys.argv[1] if len(sys.argv) > 1 else OUTDIR
    res = {}
    for fn, name, base in BLOCKS:
        res[name] = dump(os.path.join(DATA, fn), name, base, outdir)

    # Gegenprobe 1: TM2-Vertices == die ersten 8 von TS-Block A
    assert res["TM2"][0] == res["TS_BlockA"][0][:8], \
        "TM2-Vertices sind NICHT die ersten 8 von TS-Block A"
    print("\nGegenprobe: die 8 Vertices von ST_FILE.TM2 sind byte-identisch die ersten 8\n"
          "            der 36 von ST_FILE.TS Block A -> TM2 ist eine aeltere Teilfassung.")

    # Gegenprobe 2: ST_FILE.TS geht lueckenlos auf
    ts = os.path.join(DATA, "ST_FILE.TS")
    n = os.path.getsize(ts)
    assert 0x4E20 + 0x64C + 0x8C == n, "ST_FILE.TS geht nicht auf (%d B)" % n
    print("Gegenprobe: 0x4E20 (TIM) + 0x64C (Block A) + 0x8C (Block B) = 0x%X = %d B\n"
          "            = Dateilaenge von ST_FILE.TS. Kein unerklaertes Byte." % (n, n))

    # Gegenprobe 3: TM2 endet exakt hinter seinem Paketbereich
    tm2 = os.path.join(DATA, "ST_FILE.TM2")
    n2 = os.path.getsize(tm2)
    ende = res["TM2"][2] + res["TM2"][1]
    assert ende == n2, "TM2 endet bei 0x%X, Datei ist %d B" % (ende, n2)
    print("Gegenprobe: ST_FILE.TM2 endet exakt hinter seinem Paketbereich (0x%X = %d B)."
          % (n2, n2))


if __name__ == "__main__":
    main()
