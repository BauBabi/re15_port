#!/usr/bin/env python3
"""zensus_tueren.py - Zensus aller RE1.5-Tuerseiten (Runde 31, T1, Schritt 1).

Liest alle ausgelieferten RDTs (re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT), jeden
Door_aot_set (0x3B, 32/40 B; Format in zensus_lib.py mit Adressen), markiert, ob die ENGINE
ihn beim Betreten aufstellt (re15_port/tools/engine_tueren.txt), fuehrt Leon/Elza-Varianten
zu TUERSEITEN zusammen, paart Seiten zu PHYSISCHEN TUEREN und ordnet eine Kategorie zu.

Schreibt build/r31_tueren/t1/zensus_saetze.json (Rohsaetze, Seiten, Tueren).

Aufruf: python re15_port/tools/tueren/zensus_tueren.py [--liste]
"""
import collections
import json
import math
import os
import re
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import zensus_lib as L    # noqa: E402

ENGINE_TXT = os.path.join(L.REPO, "re15_port", "tools", "engine_tueren.txt")
ZONEN_H = os.path.join(L.REPO, "re15_port", "engine", "src", "re15_map_zones.h")
PAAR_GRENZE = 2500          # Messwerkzeug: Summe der beiden Spawn-Abstaende fuer eine Paarung,
                            # Wahl nach der gemessenen Verteilung (Dossier §3), kein Spielwert


# ----------------------------------------------------------------------------
def engine_liste():
    """engine_tueren.txt -> {(raum, ziel, cx, cz, band): zeile}. Spalten laut Kopf der Datei:
    raum ziel trigger_x trigger_z spawn_x spawn_z band. Schluessel (raum, ziel, trigger);
    trigger = Mittelpunkt wie der Port
    ihn aus dem Rechteck rechnet (scd_vm.c op_door_aot_set: cx = x + w/2, C-Division)."""
    aus = {}
    for z in open(ENGINE_TXT, encoding="utf-8"):
        z = z.strip()
        if not z or z.startswith("#"):
            continue
        t = z.split()
        # Die Band-Spalte wird NICHT verglichen: die Datei traegt in allen 230 Zeilen Band 0,
        # auch fuer Tueren mit pc[4] = 1..8 (z.B. ROOM1060 -> ROOM1120, Satz-Band 8) - sie
        # stammt aus dem Stand vor der Band-Korrektur (test_map_uebergang.c:195-201).
        aus[(t[0].upper(), t[1].upper(), int(t[2]), int(t[3]))] = z
    return aus


def cdiv2(v):
    return int(v / 2)          # C: gegen null


def port_schluessel(raum, s):
    """Genau die Groessen, die der Port aus pc+6.. und pc+22/23 liest (auch bei Viereck-
    Saetzen, die er falsch liest - so steht es in engine_tueren.txt)."""
    d = s["_d"]
    import struct
    x, z, w, dp = struct.unpack_from("<hhhh", d, s["pc"] + 6)
    ziel = "%X%02X%X" % (d[s["pc"] + 22] + 1, d[s["pc"] + 23], int(raum[3], 16))
    return (raum, ziel, x + cdiv2(w), z + cdiv2(dp))


# ----------------------------------------------------------------------------
def kartenseiten():
    """Raum(4-stellig) -> [(wx0,wz0,wx1,wz1,page)] aus s_map_zones, und
    (Raum, Band) -> page aus s_map_floors. Quelle re15_map_zones.h (Port-Tabelle,
    erzeugt von tools/gen_map_zones.py; Seiten = Originalblaetter, 0 B1, 1 B2, 2 1F,
    3 2F, 4 3F, 5 Dach - re15_inv_screen.c Etagen-Beischrift)."""
    src = open(ZONEN_H, encoding="utf-8").read()
    zonen = collections.defaultdict(list)
    blk = src.split("s_map_zones[] = {", 1)[1].split("};", 1)[0]
    for m in re.finditer(r"\{\s*0x([0-9A-Fa-f]{4}),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(\d+),", blk):
        zonen[m.group(1).upper()].append(tuple(int(m.group(i)) for i in range(2, 7)))
    etagen = {}
    blk = src.split("s_map_floors[] = {", 1)[1].split("};", 1)[0]
    for m in re.finditer(r"\{\s*0x([0-9A-Fa-f]{4}),\s*(\d+),\s*(\d+),\s*(\d+),", blk):
        etagen[(m.group(1).upper(), int(m.group(3)))] = int(m.group(4))
    return zonen, etagen


def seite_blatt(raum4, band, x, z, zonen, etagen):
    if (raum4, band) in etagen:
        return etagen[(raum4, band)]
    zs = zonen.get(raum4, [])
    if not zs:
        return None
    best = None
    for (x0, z0, x1, z1, pg) in zs:
        dx = max(x0 - x, 0, x - x1)
        dz = max(z0 - z, 0, z - z1)
        e = math.hypot(dx, dz)
        if best is None or e < best[0]:
            best = (e, pg)
    return best[1]


# ----------------------------------------------------------------------------
def saetze_lesen():
    aus, stubs = [], []
    for p in L.re15_rdts():
        d = open(p, "rb").read()
        nm = os.path.basename(p)[4:8].upper()
        if len(d) < 0x100:
            stubs.append((nm, len(d)))
            continue
        for s in L.re15_tueren(d):
            s["raum"] = nm
            s["datei"] = L.rel(p)
            s["_d"] = d
            aus.append(s)
    return aus, stubs


def seiten_bilden(saetze, eng):
    """Leon/Elza-Varianten und Mehrfach-Saetze zusammenfuehren:
    gleicher Grundraum + Band + Geometrie + Zielraum = dieselbe Tuerseite."""
    seiten = collections.OrderedDict()
    for s in saetze:
        basis = s["raum"][:3]
        ziel = L.raumname(s["ziel_stage"], s["ziel_raum"])
        geo = tuple(s["pts"])
        key = (basis, s["band"], geo, ziel)
        if key not in seiten:
            seiten[key] = dict(basis=basis, band=s["band"], pts=[list(p) for p in s["pts"]],
                               form=s["form"], rect=list(s["rect"]) if s["rect"] else None,
                               ziel_basis=ziel, saetze=[])
        pk = port_schluessel(s["raum"], s)
        s["engine"] = (pk in eng) if s["raum"][3] == "0" else None
        s["port_schluessel"] = list(pk)
        seiten[key]["saetze"].append(s)
    aus = []
    for i, (key, v) in enumerate(seiten.items()):
        ss = v["saetze"]
        v["id"] = "S%03d" % i
        v["varianten"] = sorted(set(s["raum"][3] for s in ss))
        v["ziele"] = sorted(set((tuple(s["ziel"]), s["ziel_dir"], s["ziel_cut"], s["ziel_band"]) for s in ss))
        v["selbst"] = v["ziel_basis"] == v["basis"]
        pts = v["pts"]
        xs = [p[0] for p in pts]
        zs = [p[1] for p in pts]
        v["nullflaeche"] = (max(xs) - min(xs) == 0 and max(zs) - min(zs) == 0)
        v["mitte"] = [sum(xs) / 4.0, sum(zs) / 4.0]
        leon = [s for s in ss if s["raum"][3] == "0"]
        if leon:
            v["engine"] = any(s["engine"] for s in leon)
        else:
            v["engine"] = None           # nur Elza-Datei: engine_tueren.txt deckt xxx1 nicht ab
        v["sce0_alle"] = all(s["sce"] == 0 for s in ss)
        v["main00"] = any(s["skript"] == "main00" for s in ss)
        v["archiv_variante"] = sorted(set((s["archiv"], s["variante"]) for s in ss))
        aus.append(v)
    return aus


