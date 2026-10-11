#!/usr/bin/env python3
"""r36_tueren_bauen.py - Runde 36 Spur G: Tuersequenz-Zuordnung + Textur P0CD nachfuehren.

Was es tut (analysis/befunde_runde36/G_tueren.md):
  1. plan.json (analysis/befunde_runde33/tueren_rest/plan.json, Quelle des Runde-33-Generators):
       T011/T017/T018 (ROOM1040/10C0/1120 -> Fahrstuhl ROOM1080)  archiv "RE2", basis DOOR27, V0
       T084 (ROOM2090 <-> ROOM20A0, grosse Tuer)                    archiv "RE2", basis DOOR26, V0 / V0
     "RE2" = RE2-Archiv UNVERAENDERT (eigen 0), tuer_archiv_bauen.zeilen_bauen behandelt es wie "OBJEKTLOS".
  2. --p0cd: Port-Archiv P0CD mit dem Runde-36-Rezept neu (tuer_rezepte.REZEPTE["P0CD"], r36_rezept_p0cd.py)
     -> shared_assets/RE15DOOR/P0CD.DO2 (Tonteil/SCD/MD1 bytegleich DOOR0C, nur die TIM neu).
  3. engine/src/gen/re15_tuer_eigen.inc: NUR die betroffenen Zeilen (Seiten S013 S040 S057 S164 S165) und der
     Archiv-Eintrag P0CD werden im Format von tuer_archiv_bauen.inc_schreiben neu geschrieben. Ein voller
     Generator-Lauf braucht die Runde-31/33-Ausschnitte und extracted/-Hintergruende des Hauptbaums (im
     Arbeitsbaum nicht bytegleich vorhanden) - deshalb hier gezielt; plan.json + Generator ergeben dieselben
     Zeilen, wenn man ihn dort laufen laesst.

⛔ PORT-WAHL (Zuordnung Seite -> Archiv), Belege der Wahl im Dossier:
  DOOR27 = RE2-Fahrstuhltuer: RE2 ROOM6010 (A-2-Kabine) main00 @0x004C2/@0x004E2/@0x00502 DOOR27 V0,
           Gegenseiten ROOM6020 @0x00872, ROOM6060 @0x00E7C DOOR27 V0; zwei Fluegel schieben aus der Mitte
           auseinander = RE1.5-Kabinentuer ROOM10800.
  DOOR26 V0 = RE2-Gross-Schott zweiteilig: DOOR26 Skript 3 @Datei 0x05914 stellt obj 0 (mesh 0) UND obj 1
           (mesh 1) auf; RE2 nutzt es beidseitig V0 (6020<->6030 @0x008AC/@0x029CA, 6030<->6040, 60C0<->60F0,
           6140<->6150, 6160<->6170). RE1.5-Flaeche ROOM2090.RDT @0x9B6 3500 breit (Standard 2000).

Aufruf:  python r36_tueren_bauen.py [--p0cd] [--pruefen]
"""
import argparse
import json
import os
import re
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import tuer_archiv_bauen as tab   # noqa: E402

SEITEN_R36 = ("S013", "S040", "S057", "S164", "S165")

HERKUNFT_AUFZUG = ("Runde 36 Spur G: RE2-Fahrstuhltuer DOOR27 V0 unveraendert (RE2 ROOM6010 A-2-Kabine "
                   "@0x004C2/@0x004E2/@0x00502 V0, Gegenseiten 6020 @0x00872 / 6060 @0x00E7C V0); RE1.5 "
                   "ROOM10800 malt eine zweifluegelige, mittig oeffnende Schiebetuer")
HERKUNFT_SCHOTT_A = ("Runde 36 Spur G: Gross-Schott zweiteilig DOOR26 V0 unveraendert (Skript 3 @Datei 0x05914 "
                     "obj 0 + obj 1, Rad links); Flaeche 3500 breit = zwei Fluegel")
HERKUNFT_SCHOTT_B = ("Runde 36 Spur G: Gegenseite DOOR26 V0 wie RE2 (6020/6030/6040/60C0/60F0/6140/6150/"
                     "6160/6170 beidseitig V0)")


def plan_r36(plan):
    """Runde-36-Wahl in plan.json eintragen (idempotent). -> Liste der geaenderten Tueren."""
    neu = []
    for tid in ("T011", "T017", "T018"):
        e = plan["tueren"][tid]
        e["archiv"] = "RE2"
        e["basis"] = "DOOR27"
        e["runde36"] = "Fahrstuhl ROOM1080: vorher P25G (DOOR25 Hubtuer, faehrt hoch)"
        for sp in e["seiten"]:
            sp["variante"] = 0
            sp["herkunft"] = HERKUNFT_AUFZUG
        neu.append(tid)
    e = plan["tueren"]["T084"]
    e["archiv"] = "RE2"
    e["basis"] = "DOOR26"
    e["runde36"] = "grosse Tuer ROOM2090: vorher P26W V3/V2 (einteilig)"
    for sp in e["seiten"]:
        sp["variante"] = 0
        sp["herkunft"] = HERKUNFT_SCHOTT_A if sp["id"] == "S164" else HERKUNFT_SCHOTT_B
    neu.append("T084")
    return neu


def zeile_text(z, idx):
    """Zwei Zeilen wie tuer_archiv_bauen.inc_schreiben (Kommentar + Daten)."""
    ofs = ", ".join("%s@0x%X %s Slot %d" % (x[0], x[1], x[3], x[2]) for x in z["saetze"])
    k = "    /* %s %s ROOM%04X -> %s | %s | %s (Basis DOOR%02X) V%d (%s) */" % (
        z["seite"], z["tuer"], z["raum"], z["ziel"], ofs, z["archiv"], z["basis"], z["variante"],
        z["herkunft"].replace("*/", "* /"))
    d = "    { 0x%04X, %d, %d, %6d, %6d, %5d, %5d, {%d, %d, %d, %d}, {%d, %d, %d, %d}, 0x%02X, %d, 0, 0x%02X, %d, %d, 0x%05X, %d }," % (
        z["raum"], z["form"], z["band"], z["x"], z["z"], z["hw"], z["hh"], *z["qx"], *z["qz"], z["basis"],
        z["variante"], z["spender"], int(z["seite"][1:]), int(z["tuer"][1:]), z["saetze"][0][1], idx.get(z["archiv"], 0))
    return k, d


def inc_zeilen_ersetzen(text, zeilen, idx):
    """Ersetzt je (Seite, Raum) das Paar Kommentar+Daten in re15_tuer_zeilen_eigen[]. Anzahl bleibt."""
    zeilen_txt = text.split("\n")
    ersetzt = 0
    for z in zeilen:
        k, d = zeile_text(z, idx)
        muster = re.compile(r"^    /\* %s %s ROOM%04X -> " % (z["seite"], z["tuer"], z["raum"]))
        treffer = [i for i, l in enumerate(zeilen_txt) if muster.match(l)]
        if len(treffer) != 1:
            raise SystemExit("%s ROOM%04X: %d Kommentarzeilen in der .inc" % (z["seite"], z["raum"], len(treffer)))
        i = treffer[0]
        if not zeilen_txt[i + 1].startswith("    { 0x%04X," % z["raum"]):
            raise SystemExit("%s ROOM%04X: Datenzeile fehlt hinter dem Kommentar" % (z["seite"], z["raum"]))
        zeilen_txt[i], zeilen_txt[i + 1] = k, d
        ersetzt += 1
    return "\n".join(zeilen_txt), ersetzt


def inc_archiv_ersetzen(text, e, nr):
    zeilen_txt = text.split("\n")
    muster = re.compile(r'^    \{ "%s", ' % e["kennung"])
    treffer = [i for i, l in enumerate(zeilen_txt) if muster.match(l)]
    if len(treffer) != 1:
        raise SystemExit("%s: %d Archivzeilen" % (e["kennung"], len(treffer)))
    i = treffer[0]
    zeilen_txt[i - 1] = "    /* %d: %s = %s + Textur '%s' (Ton %s) */" % (nr, e["kennung"], e["basis"],
                                                                         e["rezept"].replace("*/", "* /"), e["ton_familie"])
    zeilen_txt[i] = "    { \"%s\", 0x%02X, %d, %d, %d, %d, 0x%08Xu, %d }," % (
        e["kennung"], e["basis_nr"], e["ton"], e["modell"], e["sektor"], e["datei"], e["fnv"], e.get("md1_eigen", 0))
    return "\n".join(zeilen_txt)


KOPF_R36 = (" * Runde 36 Spur G (tools/tueren/r36_tueren_bauen.py, analysis/befunde_runde36/G_tueren.md): Zeilen S013 S040\n"
            " * S057 (Fahrstuhl -> DOOR27 V0) und S164 S165 (grosse Tuer 2090 -> DOOR26 V0) sowie der Eintrag P0CD (neue\n"
            " * Textur) gezielt nachgefuehrt; plan.json traegt dieselbe Wahl (archiv \"RE2\").")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--p0cd", action="store_true", help="P0CD mit dem Runde-36-Rezept neu bauen")
    ap.add_argument("--pruefen", action="store_true", help="nichts schreiben")
    a = ap.parse_args()

    plan = json.load(open(tab.PLAN, encoding="utf-8"))
    geaendert = plan_r36(plan)
    zeilen = [z for z in tab.zeilen_bauen(plan, {}) if z["seite"] in SEITEN_R36]
    print("Runde-36-Zeilen: %d (%s)" % (len(zeilen), ", ".join(sorted({z["seite"] for z in zeilen}))))
    for z in zeilen:
        print("  %s %s ROOM%04X band %d (%d,%d,%d,%d) -> DOOR%02X V%d" % (z["seite"], z["tuer"], z["raum"], z["band"],
                                                                       z["x"], z["z"], z["hw"], z["hh"], z["basis"], z["variante"]))
    if len(zeilen) != 10:
        raise SystemExit("erwartet 10 Zeilen (5 Seiten x xxx0/xxx1), gefunden %d" % len(zeilen))

    text = open(tab.AUS_INC, encoding="utf-8").read()
    # Index der Archive (Reihenfolge der Tabelle bleibt): fuer eigen != 0 (hier nicht benoetigt)
    kennungen = re.findall(r'^    \{ "(\w+)", 0x', text, re.M)
    idx = {k: i + 1 for i, k in enumerate(kennungen)}
    text, n = inc_zeilen_ersetzen(text, zeilen, idx)
    print("ersetzt: %d Zeilen" % n)

    if a.p0cd:
        tab._rezepte_stufe2()
        e = tab.archiv_bauen("P0CD", plan["archive"]["P0CD"], schreiben=not a.pruefen)
        print("P0CD: %d B, fnv %08X, Palette %d (quantisiert %s)" % (e["datei"], e["fnv"],
                                                                     e["palette"]["palette_belegt"], e["palette"]["quantisiert"]))
        text = inc_archiv_ersetzen(text, e, idx["P0CD"])
        prot = os.path.join(tab.REPO, "analysis", "befunde_runde36", "G_p0cd_protokoll.json")
        if not a.pruefen:
            with open(prot, "w", encoding="utf-8") as f:
                json.dump({k: e[k] for k in ("kennung", "basis", "datei", "fnv", "sha1", "basis_sha1", "rezept",
                                             "protokoll", "palette")}, f, ensure_ascii=False, indent=1)

    if KOPF_R36.split("\n")[0] not in text:
        text = text.replace(" * (Tonteil, Modellteil, Sektor), fnv = FNV-1a 32 ueber die ganze Datei. */",
                            " * (Tonteil, Modellteil, Sektor), fnv = FNV-1a 32 ueber die ganze Datei.\n" + KOPF_R36 + " */", 1)
    alt = " * Wahl nach Huellkurve (tools/tueren/tuer_g12_ton.py, analysis/befunde_runde33/tueren_rest_bau.md). */"
    if alt in text:
        text = text.replace(alt, " * Wahl nach Huellkurve (tools/tueren/tuer_g12_ton.py, analysis/befunde_runde33/tueren_rest_bau.md).\n"
                            " * Runde 36 Spur G: eigen = 0 auch fuer Seiten mit RE2-Archiv UNVERAENDERT (plan.json archiv \"RE2\":\n"
                            " * Fahrstuhl ROOM1080 = DOOR27 V0, Gross-Schott ROOM2090/20A0 = DOOR26 V0; analysis/befunde_runde36/G_tueren.md). */", 1)
    if a.pruefen:
        print("(--pruefen: nichts geschrieben)")
        return 0
    with open(tab.AUS_INC, "w", encoding="utf-8", newline="\n") as f:
        f.write(text)
    with open(tab.PLAN, "w", encoding="utf-8", newline="\n") as f:
        json.dump(plan, f, ensure_ascii=False, indent=1)
        f.write("\n")
    print("geschrieben:", tab.AUS_INC, tab.PLAN, "Tueren:", geaendert)
    return 0


if __name__ == "__main__":
    sys.exit(main())
