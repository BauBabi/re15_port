#!/usr/bin/env python3
"""zensus_paare.py - Tuerseiten zu physischen Tueren paaren und Kategorien zuordnen
(Runde 31, T1). Baut auf zensus_tueren.py (Saetze, Seiten) auf.

Aufruf: python re15_port/tools/tueren/zensus_paare.py [--liste]
"""
import collections
import os
import re
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import zensus_lib as L        # noqa: E402
import zensus_tueren as Z     # noqa: E402

PAAR_GRENZE = Z.PAAR_GRENZE


# ----------------------------------------------------------------------------
# Zwillingsraeume: ein Grundraum, dessen Seiten Flaeche/Band/Ziel mit >= 2 Seiten eines
# anderen Grundraums teilen (z.B. ROOM20B = ROOM200 geflutet). Hauptraum = der Raum, auf den
# die Tueren der Nachbarn zeigen.
# ----------------------------------------------------------------------------
def zwillinge(seiten):
    schl = collections.defaultdict(list)
    for s in seiten:
        schl[(s["band"], tuple(map(tuple, s["pts"])), s["ziel_basis"])].append(s)
    zaehl = collections.Counter()
    for k, ss in schl.items():
        raeume = sorted(set(x["basis"] for x in ss))
        for i in range(len(raeume)):
            for j in range(i + 1, len(raeume)):
                zaehl[(raeume[i], raeume[j])] += 1
    ziel_von = collections.Counter(s["ziel_basis"] for s in seiten)
    alias = {}
    for (a, b), n in zaehl.items():
        if n < 2:
            continue
        haupt, zw = (a, b) if ziel_von[a] >= ziel_von[b] else (b, a)
        alias[zw] = haupt

    # Seiten mit gleicher Flaeche + Band in (Zwillings-)gleichem Raum und mit (Zwillings-)
    # gleichem Ziel sind EINE Tuerseite: Hauptseite = Seite im Hauptraum mit Ziel im Hauptraum.
    def r(x):
        return alias.get(x, x)
    gruppen = collections.defaultdict(list)
    for s in seiten:
        gruppen[(r(s["basis"]), s["band"], tuple(map(tuple, s["pts"])), r(s["ziel_basis"]))].append(s)
    for k, ss in gruppen.items():
        if len(ss) < 2:
            continue
        ss.sort(key=lambda x: (x["basis"] in alias, x["ziel_basis"] in alias, x["id"]))
        for x in ss[1:]:
            x["zwilling_von"] = ss[0]["id"]
    return alias


def paaren(seiten, alias):
    """1. Regelpaar: A->B mit B->A, Ankunft von B->A nahe am Rechteck von A->B und umgekehrt;
          Guete = Summe beider Abstaende (0 = Ankunft im Rechteck), Grenze PAAR_GRENZE.
       2. Skript-Gegenseite: B->A ist ein Skript-Uebergang (Flaeche 0) - dann zaehlt nur
          der Abstand seiner Ankunft zum Rechteck von A->B.
       3. Einziges Paar: zwischen zwei Raeumen gibt es in jeder Richtung genau eine freie
          Seite -> gepaart ohne Grenze (Marke 'einzig').
       Zwillingsraeume werden dabei durch ihren Hauptraum ersetzt."""
    def raum(x):
        return alias.get(x, x)
    kand = []
    haupt = [s for s in seiten if "zwilling_von" not in s]
    for a in haupt:
        for b in haupt:
            if a is b or raum(b["basis"]) != raum(a["ziel_basis"]) or raum(a["basis"]) != raum(b["ziel_basis"]):
                continue
            if a["nullflaeche"] and b["nullflaeche"]:
                continue
            da = min(L.abstand_viereck(a["pts"], z[0][0], z[0][2]) for z in b["ziele"]) if not a["nullflaeche"] else None
            db = min(L.abstand_viereck(b["pts"], z[0][0], z[0][2]) for z in a["ziele"]) if not b["nullflaeche"] else None
            if da is None or db is None:
                g = da if da is not None else db
                kand.append((g, da, db, a["id"], b["id"], "skript"))
            else:
                kand.append((da + db, da, db, a["id"], b["id"], "regel"))
    kand.sort(key=lambda k: (k[5] != "regel", k[0]))
    frei = {s["id"] for s in haupt}
    paare = []
    for (g, da, db, ia, ib, art) in kand:
        if ia in frei and ib in frei and g <= PAAR_GRENZE:
            frei.discard(ia)
            frei.discard(ib)
            paare.append(dict(a=ia, b=ib, guete=g, da=da, db=db, art=art))
    by = {s["id"]: s for s in seiten}
    rest = collections.defaultdict(list)
    for i in sorted(frei):
        s = by[i]
        if not s["nullflaeche"]:
            rest[(raum(s["basis"]), raum(s["ziel_basis"]))].append(i)
    for (ra, rb), ids in sorted(rest.items()):
        if ra >= rb or len(ids) != 1:
            continue
        gegen = rest.get((rb, ra), [])
        if len(gegen) == 1 and ids[0] in frei and gegen[0] in frei:
            a, b = by[ids[0]], by[gegen[0]]
            da = min(L.abstand_viereck(a["pts"], z[0][0], z[0][2]) for z in b["ziele"])
            db = min(L.abstand_viereck(b["pts"], z[0][0], z[0][2]) for z in a["ziele"])
            frei.discard(a["id"])
            frei.discard(b["id"])
            paare.append(dict(a=a["id"], b=b["id"], guete=da + db, da=da, db=db, art="einzig"))
    return paare, kand


# ----------------------------------------------------------------------------
# Fahrstuhl-Raeume: die Fahrt-Signaturen aus engine/src/gen/re15_elev_se.inc
# (tools/gen_re15_elev_anchors.py: SIG1 32 B, SIG2 28 B, 0 Fehltreffer in 240 RDT)
# ----------------------------------------------------------------------------
def fahrstuhl_raeume():
    inc = open(os.path.join(L.REPO, "re15_port", "engine", "src", "gen", "re15_elev_se.inc"),
               encoding="utf-8").read()
    sigs = []
    for m in re.finditer(r"\{\s*/\*\s*(SIG\d[^*]*)\*/\s*(\d+),\s*0x[0-9A-Fa-f]+,\s*\{(.*?)\}\s*\}", inc, re.S):
        n = int(m.group(2))
        bts = bytes(int(v, 16) for v in re.findall(r"0x([0-9a-fA-F]{2})", m.group(3)))[:n]
        sigs.append((m.group(1).strip(), bts))
    assert len(sigs) == 2, sigs
    aus = {}
    for p in L.re15_rdts():
        d = open(p, "rb").read()
        for nm, b in sigs:
            if b in d:
                aus[os.path.basename(p)[4:7].upper()] = nm
    return aus


# ----------------------------------------------------------------------------
# Kategorien. Datenkriterien (in dieser Reihenfolge):
#   selbst   Zielraum == eigener Raum (Selbst-Tuer, z.B. das Tor ROOM1170)
#   aufzug   einer der beiden Raeume traegt eine Fahrt-Signatur (Fahrstuhlkabine)
#   skript   nur Skript-Uebergaenge (Flaeche 0), keine begehbare Tuer -> Sonstiges
#   etage    die beiden Seiten liegen auf verschiedenen Kartenblaettern (Etagen/Bereiche)
#            -> Kandidat Treppe/Leiter/Rolltor, per Sicht zu entscheiden (SICHT)
#   normal   sonst
# SICHT: nach Ansicht der Kontaktboegen gesetzt: (Raum, Mitte x, Mitte z) -> (Kategorie, Grund).
# ----------------------------------------------------------------------------
SICHT = {}


def kategorie(tuer, by, lifte):
    ss = [by[i] for i in tuer["seiten"]]
    raeume = set(s["basis"] for s in ss) | set(s["ziel_basis"] for s in ss)
    for s in ss:
        k = (s["basis"], int(round(s["mitte"][0])), int(round(s["mitte"][1])))
        if k in SICHT:
            return SICHT[k][0], "Sicht: " + SICHT[k][1]
    lift = sorted(r for r in raeume if r in lifte)
    if lift:
        return "aufzug", "Fahrt-Signatur %s in ROOM%s" % (lifte[lift[0]], lift[0])
    if all(s["nullflaeche"] for s in ss):
        return "skript", "Flaeche 0: Skript-Uebergang (Aot_on), keine begehbare Tuer%s" % (
            " (Ziel = eigener Raum)" if all(s["selbst"] for s in ss) else "")
    if all(s["selbst"] for s in ss):
        return "selbst", "Zielraum = eigener Raum"
    bl = set()
    for s in ss:
        if s["blatt"] is not None and s["ziel_blatt"] is not None and s["blatt"] != s["ziel_blatt"]:
            bl.add((s["blatt"], s["ziel_blatt"]))
    if bl:
        return "etage", "Kartenblatt %s" % sorted(bl)
    return "normal", "Daten ohne Besonderheit"


def bauen(argv=()):
    eng = Z.engine_liste()
    saetze, stubs = Z.saetze_lesen()
    seiten = Z.seiten_bilden(saetze, eng)
    alias = zwillinge(seiten)
    paare, kand = paaren(seiten, alias)
    zonen, etagen = Z.kartenseiten()
    namen = L.raumnamen()
    lifte = fahrstuhl_raeume()
    by = {s["id"]: s for s in seiten}
    for s in seiten:
        s["blatt"] = Z.seite_blatt(s["basis"] + "0", s["band"], s["mitte"][0], s["mitte"][1], zonen, etagen)
        z0 = s["ziele"][0]
        s["ziel_blatt"] = Z.seite_blatt(s["ziel_basis"] + "0", z0[3], z0[0][0], z0[0][2], zonen, etagen)
        s["raumname"] = namen.get(s["basis"], "")
        s["zielname"] = namen.get(s["ziel_basis"], "")
    tueren = []
    for p in paare:
        tueren.append(dict(seiten=[p["a"], p["b"]], paar=p))
    belegt = {x for t in tueren for x in t["seiten"]}
    for s in seiten:
        if s["id"] not in belegt and "zwilling_von" not in s:
            tueren.append(dict(seiten=[s["id"]], paar=None))
    in_tuer = {}
    for t in tueren:
        for i in t["seiten"]:
            in_tuer[i] = t
    for s in seiten:
        if "zwilling_von" in s:
            t = in_tuer[s["zwilling_von"]]
            t.setdefault("zwillinge", []).append(s["id"])
    tueren.sort(key=lambda t: (by[t["seiten"][0]]["basis"], by[t["seiten"][0]]["mitte"]))
    for i, t in enumerate(tueren):
        t["id"] = "T%03d" % i
        t["kategorie"], t["kriterium"] = kategorie(t, by, lifte)
        alle = [by[i] for i in t["seiten"] + t.get("zwillinge", [])]
        # physisch = mindestens eine begehbare Seite (Flaeche > 0)
        t["physisch"] = any(not s["nullflaeche"] for s in alle)
        # inert: jeder Satz jeder Seite hat sce = 0 (Scan springt ueber sce, @0x80042f74;
        # Port: scd_vm.c op_door_aot_set setzt dann Typ NONE)
        t["inert"] = all(s["sce0_alle"] for s in alle)
        t["engine"] = any(s["engine"] for s in alle)
        for sid in t["seiten"]:
            by[sid]["tuer"] = t["id"]
            gegen = [x for x in t["seiten"] if x != sid]
            by[sid]["gegenseite"] = gegen[0] if gegen else None
        for sid in t.get("zwillinge", []):
            by[sid]["tuer"] = t["id"]
            by[sid]["gegenseite"] = by[by[sid]["zwilling_von"]]["gegenseite"]
    getroffen = collections.Counter()
    for s in saetze:
        if s["engine"]:
            getroffen[tuple(s["port_schluessel"])] += 1
    fehlend = [k for k in eng if k not in getroffen]
    for s in saetze:
        s.pop("_d", None)
    return dict(saetze=saetze, seiten=seiten, tueren=tueren, paare=paare, alias=alias, lifte=lifte,
                stubs=stubs, engine_fehlend=fehlend, engine_zeilen=len(eng))


def bericht(Zs, liste=False):
    seiten, tueren = Zs["seiten"], Zs["tueren"]
    by = {s["id"]: s for s in seiten}
    kat = collections.Counter(t["kategorie"] for t in tueren)
    print("RDT-Dateien: %d, davon 4-Byte-Stubs: %d" % (len(L.re15_rdts()), len(Zs["stubs"])))
    print("Door_aot_set-Saetze: %d (Viereck: %d)" % (len(Zs["saetze"]), sum(1 for s in Zs["saetze"] if s["form"] == "viereck")))
    print("Tuerseiten: %d (Flaeche 0: %d, Zwillingsseiten: %d); Engine-Zeilen: %d, ohne Satz: %d" % (
        len(seiten), sum(1 for s in seiten if s["nullflaeche"]), sum(1 for s in seiten if "zwilling_von" in s),
        Zs["engine_zeilen"], len(Zs["engine_fehlend"])))
    print("Zwillingsraeume:", Zs["alias"])
    print("Fahrstuhlraeume:", Zs["lifte"])
    print("Paare: %d (%s)" % (len(Zs["paare"]), dict(collections.Counter(p["art"] for p in Zs["paare"]))))
    print("Tueren: %d, davon physisch %d; Kategorien %s" % (len(tueren), sum(t["physisch"] for t in tueren), dict(kat)))
    if liste:
        for t in tueren:
            ss = [by[i] for i in t["seiten"]]
            print(t["id"], t["kategorie"], "|", " <-> ".join("%s:%s b%d (%d,%d)->%s" % (
                s["id"], s["basis"], s["band"], s["mitte"][0], s["mitte"][1], s["ziel_basis"]) for s in ss),
                "| zw", t.get("zwillinge"), "|", t["kriterium"], "|", (t["paar"] or {}).get("art"),
                round((t["paar"] or {}).get("guete") or 0))


if __name__ == "__main__":
    bericht(bauen(sys.argv[1:]), "--liste" in sys.argv)
