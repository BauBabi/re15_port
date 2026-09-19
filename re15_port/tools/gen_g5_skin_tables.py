#!/usr/bin/env python3
"""Generate engine/src/gen/g5_skin_tables.inc -- die Skinning-/Naht-/Augen-Tabellen des
RE2-G5 (EM036) und seiner Tentakel (EM037) aus dem RE2-Gegnerarchiv.

Quelle: shared_assets/RE2/CDEMD0.EMS, Record "AI-Overlay-Kopie Slot 0" (vorgelinkt
@0x80100000) je kind -- Sektor-TOC wie engine/src/gen/re2_ems_toc.inc (Index
(kind-0x10)*4 + 1, RE2_EMS_REC_OVL_SLOT0 in re2_ems.h):
    EM036 (kind 0x36): EMS @0x6C0000, 23476 B  -> Datei-Offset = 0x6C0000 + (addr - 0x80100000)
    EM037 (kind 0x37): EMS @0x708000, 23024 B  -> Datei-Offset = 0x708000 + (addr - 0x80100000)

2-Bone-Skinning-Tabellenformat (FUN_80019628 Init / FUN_800197f4 je Frame, RE2-EXE;
Dossier analysis/befunde_2026-09-19/birkin-g5.md 2.2b):
    +0  u8  part_a        +1 u8 part_b       +2 u16 count
    +4/+8/+C/+10/+1C/+20/+24  Pufferzeiger (vom Init gefuellt, hier 0)
    +14 SVECTOR off_a     +2C SVECTOR off_b  +34 ptr Paarliste {u16 vertex, u16 weight}
EM036: Tabelle 1 @0x801059d8 (Kopf/Rumpf, 186 Paare @0x801056f0), Tabelle 2 @0x80105a44
       (Arm, 13 Paare @0x80105a10; off_b = (-800,-500,0) @0x80105a70 -- Skeptiker #3).
EM037: Naht 1 @0x801058b8 (Parts 0/1, 30 Paare @0x80105840), Naht 2 @0x80105940
       (1/2, 20 Paare @0x801058f0), Naht 3 @0x801059b8 (2/3, 16 Paare @0x80105978)
       (Dossier 2.3 "Schlauch-Skinning", Aufrufer 0x80104C84 Init / 0x80104CF0 je Frame).
Augen-UV-Records (EM036, Dossier 2.2d): @0x80105610 / @0x8010562C, 28 B:
    +0 s8 v_pos +1 s8 u_pos +2 s8 v_ziel +3 s8 u_ziel +4 s16 v_vel +6 s16 u_vel
    +8 s16 v_step +A s16 u_step +C ptr Tri-Liste +10 ptr Quad-Liste +14 u8 Part
    +15 s8 v_max +16 s8 v_min +17 s8 u_max +18 s8 u_min
    Listen {u8 count, u8 prim_index...} @0x801055EC (Tris Auge 0), @0x801055F8 (Quads 0),
    @0x801055FC (Tris 1), @0x80105608 (Quads 1).

Alle Bytes werden beim Generieren gegen die im Dossier/Skeptiker zitierten Anker geprueft
(assert), damit ein verschobenes Archiv nicht still falsche Tabellen erzeugt.
"""
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.abspath(os.path.join(HERE, ".."))
EMS = os.path.join(PORT, "shared_assets", "RE2", "CDEMD0.EMS")
OUT = os.path.join(PORT, "engine", "src", "gen", "g5_skin_tables.inc")
LOAD = 0x80100000
OVL = {0x36: (0x6C0000, 23476), 0x37: (0x708000, 23024)}


def rd(ems, kind, addr, n):
    base, size = OVL[kind]
    off = addr - LOAD
    assert 0 <= off and off + n <= size, "Adresse %#x ausserhalb des Overlays %#x" % (addr, kind)
    return ems[base + off: base + off + n]


def s16(b, o):
    return struct.unpack_from("<h", b, o)[0]


def skin_table(ems, kind, addr):
    t = rd(ems, kind, addr, 0x38)
    part_a, part_b, count = t[0], t[1], struct.unpack_from("<H", t, 2)[0]
    off_a = (s16(t, 0x14), s16(t, 0x16), s16(t, 0x18))
    off_b = (s16(t, 0x2C), s16(t, 0x2E), s16(t, 0x30))
    pairs_addr = struct.unpack_from("<I", t, 0x34)[0]
    p = rd(ems, kind, pairs_addr, count * 4)
    pairs = [(struct.unpack_from("<H", p, i * 4)[0], struct.unpack_from("<H", p, i * 4 + 2)[0])
             for i in range(count)]
    return dict(addr=addr, part_a=part_a, part_b=part_b, count=count, off_a=off_a,
                off_b=off_b, pairs_addr=pairs_addr, pairs=pairs)


def prim_list(ems, kind, addr):
    n = rd(ems, kind, addr, 1)[0]
    return list(rd(ems, kind, addr + 1, n))


def main():
    ems = open(EMS, "rb").read()
    # Anker (selbst gelesen, Dossier 2.2b / Skeptiker #3 / Dossier 2.3)
    assert rd(ems, 0x36, 0x801059d8, 4) == b"\x01\x00\xba\x00"
    assert rd(ems, 0x36, 0x80105a44, 4) == b"\x01\x03\x0d\x00"
    assert rd(ems, 0x36, 0x80105a70, 4) == b"\xe0\xfc\x0c\xfe"
    assert rd(ems, 0x37, 0x801058b8, 4) == b"\x00\x01\x1e\x00"
    assert rd(ems, 0x37, 0x80105940, 4) == b"\x01\x02\x14\x00"
    assert rd(ems, 0x37, 0x801059b8, 4) == b"\x02\x03\x10\x00"
    assert rd(ems, 0x36, 0x801055ec, 10) == bytes([9, 3, 4, 0x7a, 0x7b, 0x85, 0x96, 0xc2, 0xc9, 0xcd])

    g5 = [skin_table(ems, 0x36, 0x801059d8), skin_table(ems, 0x36, 0x80105a44)]
    tent = [skin_table(ems, 0x37, 0x801058b8), skin_table(ems, 0x37, 0x80105940),
            skin_table(ems, 0x37, 0x801059b8)]
    assert g5[0]["off_a"] == (-2146, -807, 0) and g5[0]["off_b"] == (0, 0, 0)
    assert g5[1]["off_a"] == (-2146, -807, 0) and g5[1]["off_b"] == (-800, -500, 0)
    assert tent[0]["off_b"] == (-3750, 0, 0) and tent[1]["off_b"] == (-6753, 0, 0)
    assert tent[2]["off_b"] == (-8261, 0, 0)
    hist = {}
    for v, w in g5[0]["pairs"]:
        hist[w] = hist.get(w, 0) + 1
    assert hist[0] == 122 and hist[4096] == 30 and sum(hist.values()) == 186, hist
    assert all(w == 0 for _, w in g5[1]["pairs"])

    eyes = []
    for rec_addr in (0x80105610, 0x8010562c):
        r = rd(ems, 0x36, rec_addr, 28)
        tri_addr, quad_addr = struct.unpack_from("<II", r, 0x0C)
        part = r[0x14]
        lim = struct.unpack_from("<bbbb", r, 0x15)
        assert part == 2 and lim == (15, -15, 15, -15), (part, lim)
        eyes.append(dict(addr=rec_addr, tri_addr=tri_addr, quad_addr=quad_addr,
                         tris=prim_list(ems, 0x36, tri_addr), quads=prim_list(ems, 0x36, quad_addr),
                         part=part, lim=lim))

    out = []
    w = out.append
    w("/* AUTO-GENERATED by tools/gen_g5_skin_tables.py -- DO NOT EDIT.")
    w(" * Quelle: shared_assets/RE2/CDEMD0.EMS, AI-Overlay-Kopien EM036 @0x6C0000 / EM037 @0x708000")
    w(" * (RE2-Adressen = 0x80100000 + Record-Offset). Jede Tabelle traegt ihre RE2-Adresse. */")
    w("")

    def emit_tab(name, tabs, kind):
        for i, t in enumerate(tabs):
            w("/* %s[%d]: Tabelle @0x%08x, Paare @0x%08x (%d x {u16 vertex, u16 weight}) */"
              % (name, i, t["addr"], t["pairs_addr"], t["count"]))
            w("static const uint16_t %s_pairs_%d[%d][2] = {" % (name, i, t["count"]))
            row = []
            for v, wt in t["pairs"]:
                row.append("{%d,%d}" % (v, wt))
                if len(row) == 8:
                    w("    " + ", ".join(row) + ",")
                    row = []
            if row:
                w("    " + ", ".join(row) + ",")
            w("};")
        w("static const re15_g5_skin_tab_t %s[%d] = {" % (name, len(tabs)))
        for i, t in enumerate(tabs):
            w("    { %d, %d, %d, {%d,%d,%d}, {%d,%d,%d}, %s_pairs_%d },   /* @0x%08x */"
              % (t["part_a"], t["part_b"], t["count"], t["off_a"][0], t["off_a"][1], t["off_a"][2],
                 t["off_b"][0], t["off_b"][1], t["off_b"][2], name, i, t["addr"]))
        w("};")
        w("")

    emit_tab("s_g5_skin_tabs", g5, 0x36)
    emit_tab("s_g5_tent_skin_tabs", tent, 0x37)

    for i, e in enumerate(eyes):
        w("/* Auge %d: Record @0x%08x, Tris @0x%08x, Quads @0x%08x, Part %d, Klemmen v %+d/%+d u %+d/%+d */"
          % (i, e["addr"], e["tri_addr"], e["quad_addr"], e["part"], e["lim"][0], e["lim"][1],
             e["lim"][2], e["lim"][3]))
        w("static const uint8_t s_g5_eye_tris_%d[%d] = { %s };" % (i, len(e["tris"]),
                                                                    ", ".join(str(x) for x in e["tris"])))
        w("static const uint8_t s_g5_eye_quads_%d[%d] = { %s };" % (i, len(e["quads"]),
                                                                     ", ".join(str(x) for x in e["quads"])))
    w("static const re15_g5_eye_lists_t s_g5_eye_lists[2] = {")
    for i, e in enumerate(eyes):
        w("    { s_g5_eye_tris_%d, %d, s_g5_eye_quads_%d, %d, %d, %d, %d, %d, %d },"
          % (i, len(e["tris"]), i, len(e["quads"]), e["part"], e["lim"][0], e["lim"][1], e["lim"][2],
             e["lim"][3]))
    w("};")
    w("")
    open(OUT, "w", newline="\n").write("\n".join(out))
    print("geschrieben:", OUT, "| G5-Paare", [t["count"] for t in g5], "| Naht-Paare",
          [t["count"] for t in tent], "| Augen", [(len(e["tris"]), len(e["quads"])) for e in eyes])


if __name__ == "__main__":
    sys.exit(main())
