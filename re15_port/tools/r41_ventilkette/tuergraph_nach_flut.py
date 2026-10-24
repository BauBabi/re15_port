#!/usr/bin/env python3
"""Tuergraph STAGE2 "nach der Flut" (Runde 41 Spur C, Nachbesserung 2, Orchestrator-Entscheid C-E2b).

Statisch aus den ausgelieferten RDTs (re15_port/shared_assets/PSX, Auslieferungsstand):
  A  alle Door_aot_set (0x3B) der STAGE2-Raeume + ROOM11A0 (main UND subs) mit Bedingungskontext
     (Ifel_ck / Else_ck, Ck-Zeilen mit SCD-Offset) und je Bedingungs-Bit allen Setzern Set(bank,bit)=1
     aus ALLEN RDTs; Elza-Variante (ROOMxxx1) Byte fuer Byte gegen Leons Satz verglichen. Dazu die Text-Plaetze
     (Aot_set sce 1), die in einem Bedingungsblock stehen (gesperrte Tuer = Text-Platz im WENN-Zweig).
  B  Zugangs-Zensus: jede Door_aot_set aller RDTs, deren Ziel 2080/2090/20A0/2070/2040/11A0 ist.
  C  Port-Eingriffe der Runde 41 in Tueren nach der Flut (Bit 103 ohne 107), aus engine/src/ventilkette_r41.c.
Walker = re15_port/tools/scd_dump_room.py (dieselbe Laengentabelle wie scd_vm.c s_opcode_sizes, variable
Laengen 0x2C/0x3B/0x50 nach pc[3]&0x80, Ifel_ck-Ziel pc+4+u16, Else_ck-Ziel pc+s16), hier ohne dessen main().
Door_aot_set-Nutzlast: [1] slot [2] sce [3] flags [4] floor [6..13] x z w d [14..19] Ankunft x y z [20..21] Blick
[22] Stage-Index (0 = STAGE1) [23] Raum [24] Cut [25] nfloor.

Aufruf: python tuergraph_nach_flut.py [--out <datei>]   (Wurzel = re15_port, aus dem Skriptort)
"""
import glob
import io
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.normpath(os.path.join(HIER, "..", ".."))
PSX = os.path.join(WURZEL, "shared_assets", "PSX")

_ns = {}
_src = io.open(os.path.join(WURZEL, "tools", "scd_dump_room.py"), encoding="utf-8").read()
_src = _src.rsplit("\nmain()", 1)[0]
exec(compile(_src, "scd_dump_room.py", "exec"), _ns)
op_size, rdt_section_end, section_regions = _ns["op_size"], _ns["rdt_section_end"], _ns["section_regions"]
u16, s16, u32, NAMES = _ns["u16"], _ns["s16"], _ns["u32"], _ns["NAMES"]


def ziel(stage_idx, room):
    return "%d%02X0" % (stage_idx + 1, room) if room < 0x10 else "%d%X0" % (stage_idx + 1, room)


def walk(d):
    """Je Region (label, [(pc, op, size, bloecke)]); bloecke = umgebende (art, von, bis, bedingungen)."""
    ms, ss = u32(d, 0x40), u32(d, 0x44)
    out = []
    for sec, tag in ((ms, "main"), (ss, "sub")):
        if sec == 0 or sec >= len(d):
            continue
        se = rdt_section_end(d, sec)
        for (o, e, idx) in section_regions(d, sec, se):
            start, end = sec + o, sec + e
            ops, pc, bloecke = [], start, []
            while pc < end:
                sz = op_size(d, pc)
                if sz is None or pc + sz > end:
                    break
                op = d[pc]
                bloecke = [b for b in bloecke if pc < b[2]]
                if op == 0x06:
                    bis = pc + 4 + u16(d, pc + 2)
                    conds, q = [], pc + 4
                    while q < end and d[q] in (0x21, 0x23, 0x3E, 0x51, 0x5E):
                        qs = op_size(d, q)
                        if qs is None:
                            break
                        if d[q] == 0x21:
                            conds.append("Ck(%d,%d)=%d @0x%05X" % (d[q + 1], d[q + 2], d[q + 3], q))
                        else:
                            conds.append("%s @0x%05X" % (NAMES.get(d[q], "?"), q))
                        q += qs
                    bloecke.append(("WENN", pc, bis, conds))
                elif op == 0x07:
                    bis = pc + s16(d, pc + 2)
                    vor = [b for b in bloecke if b[0] == "WENN" and b[2] == pc + 4]
                    conds = vor[-1][3] if vor else ["?"]
                    bloecke = [b for b in bloecke if b not in vor]
                    bloecke.append(("SONST", pc, bis, conds))
                ops.append((pc, op, sz, list(bloecke)))
                pc += sz
            out.append(("%s%02d" % (tag, idx), ops))
    return out


def kontext_text(bl):
    if not bl:
        return "unbedingt"
    t = []
    for art, von, bis, conds in bl:
        c = " UND ".join(conds) if conds else "?"
        t.append(("WENN " + c) if art == "WENN" else ("NICHT(" + c + ")"))
    return " | ".join(t)


def tuersatz(d, pc):
    b = d[pc:pc + 32]
    return dict(slot=b[1], sce=b[2], flags=b[3], floor=b[4], x=s16(b, 6), z=s16(b, 8), w=s16(b, 10), dd=s16(b, 12),
                nx=s16(b, 14), ny=s16(b, 16), nz=s16(b, 18), st=b[22], rm=b[23], cut=b[24], nfl=b[25])


def main():
    out_pfad = sys.argv[sys.argv.index("--out") + 1] if "--out" in sys.argv else None
    zeilen = []
    p = zeilen.append
    rdts = sorted(glob.glob(os.path.join(PSX, "STAGE*", "ROOM*.RDT")))
    walks, setzer = {}, {}
    for r in rdts:
        with open(r, "rb") as f:
            d = f.read()
        if len(d) < 0x60:          # Platzhalter-Dateien ohne RDT-Kopf (z. B. 4 Byte) sind keine Raeume
            continue
        name = os.path.basename(r)[:-4].upper()
        w = walk(d)
        walks[name] = (d, w)
        for lbl, ops in w:
            for pc, op, sz, bl in ops:
                if op == 0x22 and d[pc + 3] == 1:
                    setzer.setdefault((d[pc + 1], d[pc + 2]), []).append("%s %s @0x%05X" % (name, lbl, pc))
    p("# Tuergraph STAGE2 nach der Flut - erzeugt von re15_port/tools/r41_ventilkette/tuergraph_nach_flut.py")
    p("# Quelle: %d RDTs unter shared_assets/PSX (Auslieferungsstand)." % len(rdts))
    p("# Zeile: Raum Region @Offset slot sce floor Rechteck -> Ziel (Ankunft x,y,z) Cut nfloor | Bedingung | Setzer | Elza")
    p("")
    p("## A  Tueren der STAGE2-Raeume (Leon-Variante; Elza = ROOMxxx1 Byte fuer Byte verglichen) + ROOM11A0")
    raeume = ["ROOM20%X0" % i for i in range(0, 12)] + ["ROOM11A0"]
    for name in raeume:
        if name not in walks:
            continue
        d, w = walks[name]
        elza = name[:-1] + "1"
        p("=== %s" % name)
        for lbl, ops in w:
            for pc, op, sz, bl in ops:
                if op == 0x3B:
                    t = tuersatz(d, pc)
                    gleich = "Elza gleich"
                    if elza in walks and walks[elza][0][pc:pc + sz] != d[pc:pc + sz]:
                        gleich = "ELZA ABWEICHEND"
                    bits = []
                    for art, von, bis, conds in bl:
                        for c in conds:
                            if c.startswith("Ck("):
                                bk, bt = c[3:c.index(")")].split(",")[:2]
                                key = (int(bk), int(bt))
                                bits.append("(%d,%d)=1 gesetzt in: %s" % (key[0], key[1],
                                            ", ".join(setzer.get(key, ["NIRGENDS"]))))
                    p("  %s %s @0x%05X slot %d sce %d floor %d rect x %d..%d z %d..%d -> ROOM%s (%d,%d,%d) Cut %d nfloor %d"
                      " | %s%s | %s"
                      % (name, lbl, pc, t["slot"], t["sce"], t["floor"], t["x"], t["x"] + t["w"], t["z"], t["z"] + t["dd"],
                         ziel(t["st"], t["rm"]), t["nx"], t["ny"], t["nz"], t["cut"], t["nfl"], kontext_text(bl),
                         (" | " + "; ".join(bits)) if bits else "", gleich))
                elif op == 0x2C and d[pc + 2] == 1 and bl:
                    p("  %s %s @0x%05X TEXT slot %d floor %d rect x %d..%d z %d..%d msg %d | %s"
                      % (name, lbl, pc, d[pc + 1], d[pc + 4], s16(d, pc + 6), s16(d, pc + 6) + s16(d, pc + 10),
                         s16(d, pc + 8), s16(d, pc + 8) + s16(d, pc + 12), d[pc + 14], kontext_text(bl)))
    p("")
    p("## B  Zugangs-Zensus: alle Door_aot_set aller RDTs mit diesen Zielen")
    for zst, zrm in ((1, 0x08), (1, 0x09), (1, 0x0A), (1, 0x07), (1, 0x04), (0, 0x1A)):
        treffer = []
        for name, (d, w) in walks.items():
            for lbl, ops in w:
                for pc, op, sz, bl in ops:
                    if op == 0x3B and d[pc + 22] == zst and d[pc + 23] == zrm:
                        t = tuersatz(d, pc)
                        treffer.append("    %s %s @0x%05X slot %d sce %d floor %d -> (%d,%d,%d) nfloor %d | %s"
                                       % (name, lbl, pc, t["slot"], t["sce"], t["floor"], t["nx"], t["ny"], t["nz"],
                                          t["nfl"], kontext_text(bl)))
        p("  Ziel ROOM%s: %d Saetze" % (ziel(zst, zrm), len(treffer)))
        for z in sorted(treffer):
            p(z)
    p("")
    p("## C  Port-Eingriffe der Runde 41 (engine/src/ventilkette_r41.c) im Zustand Bits 103+100 ohne 107")
    p("  ROOM2000/20B0 slot 4 (Leiter -> 2050): Sperrtext 40 nur OHNE Bit 100 -> nach der Flut Tuer (install_2000)")
    p("  Tueren mit Ziel ROOM2000 -> ROOM20B0 (Cut-Zuordnung re15_ventilkette_r41_cut_20b0) ab Bit 103 (tueren_umlenken)")
    p("  ROOM2030 slot 0 (Ost-Tuer, Ring) -> ROOM20B0 Cut 11 ab Bit 103, auch mit Bit 107 (tueren_umlenken)")
    p("  ROOM20B0 slot 3 / ROOM2070 slots 1,2 (Ring) -> Sperrtext 41 / 40, Bit 103 ohne 107 (ring_sperren, C-E2)")
    p("  ROOM2030 Lift-Rufzonen oben slot 4 / 6 -> Original-Satz 1, Bit 103 ohne 107 (install_2030)")
    p("  Keine Port-Sperre der Runde 41 an Tueren von ROOM2040/2060/2070-Nordgang/11A0/20A0/2090/2080.")
    text = "\n".join(zeilen) + "\n"
    if out_pfad:
        with io.open(out_pfad, "w", encoding="utf-8", newline="\n") as f:
            f.write(text)
        print("geschrieben: %s (%d Zeilen)" % (out_pfad, len(zeilen)))
    else:
        sys.stdout.write(text)


main()
