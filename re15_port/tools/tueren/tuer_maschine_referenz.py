#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""tuer_maschine_referenz.py - Referenz des Katalog-Simulators fuer JEDE benutzte Archiv-Variante.

Runde 31, Stufe 4 (analysis/befunde_runde31/tueren_04_bau.md Abschnitt 2). Fuer jedes Paar
(Archiv, Variante), das die Zuordnung (engine/src/gen/tuer_zuordnung.inc bzw. zuordnung.json)
benutzt:
  1. prueft, dass Skript 0 die Variante wirklich VERTEILT: ein Switch auf var 0x0C mit einem Case
     genau dieses Werts (nicht der Default-Zweig) - aus den Skriptbytes selbst gelesen;
  2. faehrt den Katalog-Simulator (tools/tor/tuerkatalog.py VM, vom Skeptiker nachgerechnet) bis
     zum Ende und schreibt je Bild eine Pruefsumme ueber alle 10 Objekte (an, Mesh, Flags, Lage,
     Drehung), die Bilder mit Se_on, die Bildzahl und den Schliesston-Merker.

unit_r31_maschine (tests/unit/probe_r31_tueren.c) faehrt dieselben Paare mit
engine/src/door_seq_common.c aus den Kopien in shared_assets/RE2/DOOR und vergleicht Bild fuer Bild.

    python re15_port/tools/tueren/tuer_maschine_referenz.py
    -> re15_port/tests/unit/gen/r31_tuer_referenz.inc
"""
import json
import os
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.abspath(os.path.join(HIER, "..", ".."))
REPO = os.path.abspath(os.path.join(PORT, ".."))
sys.path.insert(0, os.path.join(PORT, "tools", "tor"))
import tuerkatalog as tk  # noqa: E402

ZUORDNUNG = os.path.join(REPO, "analysis", "befunde_runde31", "tueren_03", "zuordnung.json")
AUS = os.path.join(PORT, "tests", "unit", "gen", "r31_tuer_referenz.inc")


def fnv(h, w):
    """FNV-1a ueber ein 32-Bit-Wort (4 Bytes, little-endian) - identisch in probe_r31_tueren.c."""
    for k in range(4):
        h ^= (w >> (8 * k)) & 0xFF
        h = (h * 16777619) & 0xFFFFFFFF
    return h


def bild_summe(objs):
    h = 2166136261
    m = {o["obj"]: o for o in objs}
    for i in range(10):
        o = m.get(i)
        h = fnv(h, i)
        if o is None:
            h = fnv(h, 0)
            continue
        h = fnv(h, 1)
        h = fnv(h, o["mesh"] & 0xFFFF)
        h = fnv(h, o["flags"] & 0xFFFF)
        for v in o["pos"]:
            h = fnv(h, v & 0xFFFFFFFF)
        for v in o["rot"]:
            h = fnv(h, v & 0xFFFF)
    return h


def verteilt(door, variante):
    """Case-Werte des Switch auf var 0x0C in Skript 0 (Bytes selbst gelesen)."""
    blk = door.scd_block()
    a, b = door.script_range(0)
    pc = a
    faelle = []
    while pc < b:
        ins = tk.decode_op(blk, pc)
        if ins["op"] == 0x13 and ins["f"]["var"] == 0x0C:
            p = pc + 4
            while p < b:
                o2 = blk[p]
                if o2 == 0x14:
                    faelle.append(struct.unpack_from("<h", blk, p + 4)[0])
                    p += 6 + struct.unpack_from("<H", blk, p + 2)[0]
                    continue
                break
            return faelle, pc
        if ins["len"] <= 0:
            break
        pc += ins["len"]
    return faelle, None


PLAN33 = os.path.join(REPO, "analysis", "befunde_runde33", "tueren_rest", "plan.json")
ARCH33 = os.path.join(REPO, "analysis", "befunde_runde33", "tueren_rest", "archive.json")
AUS33 = os.path.join(PORT, "tests", "unit", "gen", "r33_tuer_referenz.inc")


def paare_r31():
    d = json.load(open(ZUORDNUNG, encoding="utf-8"))
    return sorted({(int(s["archiv"][4:], 16), s["variante"]) for t in d["tueren"] for s in t["seiten"]
                   if s.get("abgedeckt")})


def paare_r33():
    """Runde 33: (Basis, Variante) jeder gebauten Seite (Port-Archiv oder G12 objektlos), die die Runde 31
    NICHT schon gegen den Simulator pinnt."""
    plan = json.load(open(PLAN33, encoding="utf-8"))
    basis = {e["kennung"]: e["basis_nr"] for e in json.load(open(ARCH33, encoding="utf-8"))["archive"]}
    out = set()
    for e in plan["tueren"].values():
        for s in e["seiten"]:
            if not (s.get("bau") and "variante" in s):
                continue
            a = s.get("archiv", e["archiv"])
            nr = int(e["basis"][4:], 16) if a == "OBJEKTLOS" else basis[a]
            out.add((nr, s["variante"]))
    return sorted(out - set(paare_r31()))


def main():
    if "--r33" in sys.argv:
        return schreiben(paare_r33(), AUS33, "r33")
    return schreiben(paare_r31(), AUS, "r31")


def schreiben(paare, aus, praefix):
    L = ["/* GENERIERT von re15_port/tools/tueren/tuer_maschine_referenz.py - NICHT HAND-EDITIEREN.",
         " * Katalog-Simulator (tools/tor/tuerkatalog.py) je benutzter Archiv-Variante: Bildzahl,",
         " * Schliesston-Merker, Se_on-Bilder, je Bild FNV-1a ueber die 10 Objekte (an, Mesh, Flags,",
         " * Lage, Drehung). Case-Werte = Switch var 0x0C in Skript 0 (Datei-Offset relativ zur",
         " * Skripttabelle des Modellteils). */"]
    fehler = 0
    koerper = []
    for (ar, v) in paare:
        door = tk.Door(ar)
        faelle, sw = verteilt(door, v)
        ok = v in faelle
        if not ok:
            fehler += 1
        vm = tk.VM(door, variant=v, sound_ready_tick=0).run()
        summen = [bild_summe(fr) for fr in vm.frames]
        tone = [t["tick"] for t in vm.sounds]
        name = "ref_%02x_v%d" % (ar, v)
        L.append("/* DOOR%02X V%d: Switch var 0x0C @0x%04X Cases %s -> %s; %d Bilder, Se_on %s, Schliesston %d, Notizen %s */" % (
            ar, v, sw if sw is not None else 0, faelle, "verteilt" if ok else "NICHT VERTEILT", len(summen), tone,
            vm.global248, sorted(vm.notes) or "-"))
        L.append("static const uint32_t %s_summen[%d] = {" % (name, len(summen)))
        for i in range(0, len(summen), 8):
            L.append("    " + ", ".join("0x%08Xu" % x for x in summen[i:i + 8]) + ",")
        L.append("};")
        L.append("static const int %s_tone[] = {%s};" % (name, ", ".join(str(t) for t in tone) or "-1"))
        koerper.append("    { 0x%02X, %d, %d, %s_summen, %d, %s_tone, %d, %d }," % (
            ar, v, 1 if ok else 0, name, len(summen), name, len(tone), vm.global248))
        print("DOOR%02X V%d: Cases %s %s, %d Bilder, Se_on %s, Schliesston %d, Notizen %s" % (
            ar, v, faelle, "ok" if ok else "NICHT VERTEILT", len(summen), tone, vm.global248, sorted(vm.notes)))
    L.append("typedef struct { int archiv, variante, verteilt; const uint32_t *summen; int n_bilder;")
    L.append("                 const int *tone; int n_tone; int schliesston; } %s_ref_t;" % praefix)
    L.append("static const %s_ref_t %s_refs[%d] = {" % (praefix, praefix, len(koerper)))
    L += koerper
    L.append("};")
    os.makedirs(os.path.dirname(aus), exist_ok=True)
    with open(aus, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(L) + "\n")
    print("Paare %d, nicht verteilt %d -> %s" % (len(paare), fehler, aus))
    return 1 if fehler else 0


if __name__ == "__main__":
    sys.exit(main())
