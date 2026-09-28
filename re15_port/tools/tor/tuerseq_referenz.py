#!/usr/bin/env python3
"""tuerseq_referenz.py - Bild-fuer-Bild-Referenz der RE2-Tuersequenz fuer den C-Test.

Laesst den Katalog-Simulator (tuerkatalog.VM, vom Skeptiker mit einem eigenen Simulator ueber
alle 146 Varianten nachgerechnet, analysis/tor_1170/04_tuerkatalog.skeptiker.md) laufen und
schreibt je Bild Lage und Drehung der Objekte 0..2 sowie die Bilder mit Se_on und Blende.

    python re15_port/tools/tor/tuerseq_referenz.py
    -> re15_port/tests/unit/gen/tuerseq_referenz.inc

Faelle: DOOR2E Variante 0 und 1 (echte RE2-Datei) und das Tor ROOM1170 Variante 0 und 1
(Modellteil aus engine/src/gen/tor_1170_door.inc, hier direkt aus tor_sequenz_bauen).
"""
import os
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import tuerkatalog as tk      # noqa: E402

PORT = os.path.abspath(os.path.join(HIER, "..", ".."))
AUS = os.path.join(PORT, "tests", "unit", "gen", "tuerseq_referenz.inc")
OBJ_N = 3


class TeilTuer(object):
    """Minimaler Door-Ersatz fuer tuerkatalog.VM aus einem Modellteil (RE2-Aufbau)."""

    def __init__(self, teil, idx=0):
        self.idx = idx
        self.teil = teil
        self.md1_rel, self.tim_rel = struct.unpack_from("<II", teil, 0)
        first = struct.unpack_from("<H", teil, 8)[0]
        self.n_scripts = first // 2
        self.scd_offs = list(struct.unpack_from("<%dH" % self.n_scripts, teil, 8))
        self.scd_end_rel = self.md1_rel - 8

    def scd_block(self):
        return bytearray(self.teil[8:8 + self.scd_end_rel])


def lauf(door, variante):
    vm = tk.VM(door, variant=variante, sound_ready_tick=0).run()
    bilder = []
    for st in vm.frames:
        z = []
        m = {s["obj"]: s for s in st}
        for i in range(OBJ_N):
            s = m.get(i)
            if s is None:
                z.append((0, 0, 0, 0, 0, 0, 0))
            else:
                z.append((1, s["pos"][0], s["pos"][1], s["pos"][2],
                          s["rot"][0] & 0xFFFF, s["rot"][1] & 0xFFFF, s["rot"][2] & 0xFFFF))
        bilder.append(z)
    tone = [t["tick"] for t in vm.sounds]
    blenden = [(f["tick"], 0 if f["op"] == "set" else 1) for f in vm.fades]
    return bilder, tone, blenden, vm.global248


def c_fall(name, bilder, tone, blenden, schliess):
    z = ["static const int32_t %s_bilder[%d][%d][7] = {" % (name, len(bilder), OBJ_N)]
    for b in bilder:
        z.append("    {" + ",".join("{%s}" % ",".join(str(v) for v in o) for o in b) + "},")
    z.append("};")
    z.append("static const int %s_tone[] = {%s};" % (name, ",".join(str(t) for t in tone) or "-1"))
    z.append("static const int %s_n_tone = %d;" % (name, len(tone)))
    z.append("static const int %s_blenden[][2] = {%s};" % (
        name, ",".join("{%d,%d}" % b for b in blenden) or "{-1,-1}"))
    z.append("static const int %s_n_blenden = %d;" % (name, len(blenden)))
    z.append("static const int %s_schliesston = %d;" % (name, schliess))
    return "\n".join(z)


def main():
    import tor_sequenz_bauen as tsb
    teile = []
    d2e = tk.Door(0x2E)
    for v in (0, 1):
        teile.append(("ref_door2e_v%d" % v,) + lauf(d2e, v))
    _, do2 = tsb.door2e()
    skripte = tsb.skripte_bauen(do2)
    md1_b, tim_b, _, _ = tsb.modell_bauen()
    teil, _, _ = tsb.modellteil(skripte, md1_b, tim_b)
    tor = TeilTuer(teil, 0)
    for v in (0, 1):
        teile.append(("ref_tor_v%d" % v,) + lauf(tor, v))
    os.makedirs(os.path.dirname(AUS), exist_ok=True)
    kopf = ["/* GENERIERT von re15_port/tools/tor/tuerseq_referenz.py - NICHT HAND-EDITIEREN.",
            " * Referenz des Katalog-Simulators (tuerkatalog.VM): je Bild {an,x,y,z,rx,ry,rz} fuer",
            " * die Objekte 0..%d; Bilder mit Se_on; Blenden (Bild, 0=set/1=adjust). */" % (OBJ_N - 1)]
    teile_c = [c_fall(*t) for t in teile]
    open(AUS, "w").write("\n".join(kopf) + "\n" + "\n".join(teile_c) + "\n")
    for t in teile:
        print("%-16s %d Bilder, Toene %s, Blenden %s, Schliesston %d" % (t[0], len(t[1]), t[2], t[3], t[4]))


if __name__ == "__main__":
    main()
