#!/usr/bin/env python3
"""tuer_rest_plan.py - Runde 33 / Thema T: Bauplan fuer die 61 bisher nicht abgedeckten Tueren.

Aufruf
    python re15_port/tools/tueren/tuer_rest_plan.py          # plan.json + Tabelle schreiben

Eingabe
    analysis/befunde_runde31/tueren_03/zuordnung.json   (Runde 31: Seiten, Griffseite/-form, Abweichung)

Ausgabe
    analysis/befunde_runde33/tueren_rest/plan.json       (Quelle fuer tuer_archiv_bauen.py und
                                                         tuer_zuordnung_gen.py --eigen)
    analysis/befunde_runde33/tueren_rest/plan_tabelle.md (Tabelle je Tuer, in tueren_rest_plan.md)

⛔ PORT-WAHL, KEINE Original-Adresse: RE1.5 waehlt kein Tuerarchiv (Payload+12/+13 = 0 in 649 von
653 Door_aot_set, include/re15_door_seq.h). Jede Zeile hier ist eine Bauentscheidung aus dem
Bildvergleich der Runde 31 (Merkmale/Abweichung je Seite in zuordnung.json) + den RE2-Regeln aus
analysis/befunde_runde31/tueren_02_re2.md (Variante 2.4, Formfamilien 3, Tonfamilien 4).

Bau-Strategie (Leitlinie des Auftrags):
    Bewegung + Skripte + Ton + Griff-Mesh = BASIS-Archiv (RE2, unveraendert), gewaehlt nach
    gleicher Bewegungsart UND gemalter Griff-Form (dann braucht es keinen Griff-Tausch); die
    TEXTUR (TIM) wird neu: das Blatt (v 0..217, bei allen Standardblaettern dieselbe UV, gemessen
    mit do2_format.Md1: 07 06 08 1D 1B 0C 04 01 23 22 1A 13 24 25 15 00 Mesh 0 geometrie- und
    UV-gleich) aus der naechsten RE2-Gestaltung, bearbeitet (Merkmal weg, Farbe auf RE1.5).
"""
import json
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
ZUORDNUNG = os.path.join(REPO, "analysis", "befunde_runde31", "tueren_03", "zuordnung.json")
AUS = os.path.join(REPO, "analysis", "befunde_runde33", "tueren_rest")

# ----------------------------------------------------------------------------------------------
# Port-Archive (Kennung -> Basis-Archiv, Quelle der Blatt-Textur, Rezept, Ton).
# Ton = Tonteil des Basis-Archivs UNVERAENDERT; Familie laut tueren_02_re2.md 4 (Tabelle "Ton je
# Archiv", Familien F1..F7 = bytegleiche Tonteile).
# ----------------------------------------------------------------------------------------------
ARCHIVE = {
    "P07G": dict(basis="DOOR07", blatt="DOOR07", ton="F3 (06 07 08 22 2F, Blechtuer)",
                 rezept="Lueftungsschlitze + Flecken weg, Farbe = Median der RE1.5-Blaetter (G1); Stufe 2: "
                        "silbernes Rechteckschild unter dem Druecker gemalt (gemessen, Pilot-Punkt a)",
                 griff="Druecker flach (DOOR07 Mesh 1, gemalt: Druecker auf Rechteckschild)", stufe=1),
    "P07T": dict(basis="DOOR07", blatt="DOOR07", ton="F3 (06 07 08 22 2F, Blechtuer)",
                 rezept="wie P07G, Farbe = Median der drei Treppenhausseiten ROOM1060 (hell graugruen gemalt, "
                        "Gegenseiten dunkel; Pilot-Punkt c: je Seite ein Archiv)",
                 griff="Druecker flach (DOOR07 Mesh 1) + gemaltes Rechteckschild", stufe=2),
    "P06F": dict(basis="DOOR06", blatt="DOOR08", ton="F3 (Stahltuer)",
                 rezept="DOOR08-Blatt: Nieten/Rost weg, umlaufende gerundete Randnut + breit gerahmte Felder, hellgrau",
                 griff="Buegel-/Stangengriff senkrecht (DOOR06 Mesh 1)", stufe=2),
    "P06U": dict(basis="DOOR06", blatt="DOOR08", ton="F3 (Stahltuer)",
                 rezept="wie P06F, aber gepraegte UNGLEICHE Felder (oben hoch, unten fast quadratisch, S188)",
                 griff="Stangengriff senkrecht (DOOR06 Mesh 1)", stufe=2),
    "P1B3": dict(basis="DOOR1B", blatt="DOOR23", ton="F5 (1B 30, Doppeltuer)",
                 rezept="DOOR23-Blatt (achteckiges Profil + Ausbuchtung) je Fluegel; Griff-Tausch Spender DOOR23 (Riegelstange quer)",
                 griff="Riegelstange quer (Spender DOOR23 Mesh 1 am DOOR23-Anhaengepunkt: Versatz)", stufe=2,
                 tausch=dict(spender="DOOR23", am_spender_anker=True)),
    "P1BD": dict(basis="DOOR1B", blatt="DOOR23", ton="F5 (1B 30, Doppeltuer)",
                 rezept="wie P1B3, Farbe der ROOM2070-Seiten (kaltes blaugruenes Licht, dunkel gemalt) - je Seite ein Archiv",
                 griff="Riegelstange quer (Spender DOOR23 Mesh 1 am DOOR23-Anhaengepunkt: Versatz)", stufe=2,
                 tausch=dict(spender="DOOR23", am_spender_anker=True)),
    "P1DG": dict(basis="DOOR1D", blatt="DOOR1D", ton="eigen DOOR1D (Blech-Doppeltuer, V2/V3)",
                 rezept="Lueftungsgitter + Griffkasten weg, glattes Blech, Farbe nach RE1.5 (graugruen/blaugrau)",
                 griff="Druecker flach WAAGERECHT: Griff-Tausch Mesh + Grund-Drehung DOOR07 (silbern aus P07G) "
                       "am 1D-Anhaengepunkt - DOOR1D steht in Ruhe um x -780 gekippt (Griffkasten), RE1.5 malt "
                       "waagerechte Druecker", stufe=1,
                 tausch=dict(spender="DOOR07", spender_eigen="P1DG")),
    "P1DK": dict(basis="DOOR1D", blatt="DOOR1A", ton="eigen DOOR1D",
                 rezept="Stahlrahmen je Fluegel mit DREI Feldern ohne Glas (DOOR1A-Teilung, Kartenleser weg), dunkelgrau (T014)",
                 griff="Druecker flach WAAGERECHT (Selbst-Tausch der Grund-Drehung DOOR07, wie P1DG)", stufe=2,
                 tausch=dict(spender="DOOR07", spender_eigen="P1DK")),
    "P1DL": dict(basis="DOOR1D", blatt="DOOR1A", ton="eigen DOOR1D",
                 rezept="Stahlrahmen je Fluegel mit hohem Feld oben + Querriegel + unterem Feld (T045), grau",
                 griff="Druecker flach WAAGERECHT (Selbst-Tausch der Grund-Drehung DOOR07, wie P1DG)", stufe=2,
                 tausch=dict(spender="DOOR07", spender_eigen="P1DL")),
    "P04B": dict(basis="DOOR04", blatt="DOOR04", ton="F2 (01 04 09 11, Holz)",
                 rezept="Blau -> braunes Holz (Farbton, Lage der Kassetten und Maserung bleiben)",
                 griff="Stangengriff lang Messing (DOOR04 Mesh 1, gemalt gleich)", stufe=2),
    "P0CD": dict(basis="DOOR0C", blatt="DOOR0C", ton="eigen DOOR0C (Holz-Doppeltuer mit Glas)",
                 rezept="helles gelbbraunes Holz, je Fluegel hohes schmales Drahtglasfenster, Panikstange gemalt",
                 griff="Stangengriff senkrecht (DOOR0C Mesh 1)", stufe=2),
    "P16M": dict(basis="DOOR16", blatt="DOOR16", ton="eigen DOOR16 (4 Sprossen, kein Schliesston)",
                 rezept="Silberrohr -> Messing (goldgelb, ROOM12607.bmp), Lichtverlauf der Rohre bleibt",
                 griff="-", stufe=1),
    "P16R": dict(basis="DOOR16", blatt="DOOR16", ton="eigen DOOR16",
                 rezept="Silberrohr -> dunkles Rostbraun (ROOM11A00.bmp)", griff="-", stufe=2),
    "P1EL": dict(basis="DOOR1E", blatt="DOOR1E", ton="eigen DOOR1E (Klappe)",
                 rezept="senkrechte Staebe -> waagerechte Lamellen (Durchsicht-Texel 0 wie im Original)",
                 griff="-", stufe=2),
    "P1EU": dict(basis="DOOR1E", blatt="DOOR1E", ton="eigen DOOR1E",
                 rezept="Staebe nur im unteren Drittel, oben dunkle Oeffnung", griff="-", stufe=2),
    "P25G": dict(basis="DOOR25", blatt="DOOR25", ton="eigen DOOR25 (Hubtuer)",
                 rezept="Schild SHAFT TYPE-L, Warnaufkleber, Rippen weg -> glattes graues Blech",
                 griff="-", stufe=2),
    "P14A": dict(basis="DOOR14", blatt="DOOR14", ton="eigen DOOR14 (Schiebe-Gittertor)",
                 rezept="groebere Rauten, hellgrauer Rahmen", griff="Griffplatte (DOOR14 Mesh 1)", stufe=2),
    "P2DS": dict(basis="DOOR2D", blatt="DOOR2D", ton="eigen DOOR2D (Hubbuehne)",
                 rezept="Bedientafel + rote Lampe weg (Texel 0 = nicht gezeichnet), Warnrand bleibt",
                 griff="-", stufe=2),
    "P07R": dict(basis="DOOR07", blatt="DOOR22", ton="F3 (= DOOR22-Ton, bytegleich)",
                 rezept="DOOR22-Blatt ohne Nietrand und ohne gemalten D-Riegel, rostbraun",
                 griff="Druecker flach (DOOR07 Mesh 1)", stufe=2),
    "P1AP": dict(basis="DOOR1A", blatt="DOOR23", ton="F4 (15 1A 23 = DOOR23-Ton)",
                 rezept="DOOR23-Profil, Ausbuchtung am Griff gerade gezogen, braun",
                 griff="Druecker auf Kasten (DOOR1A Mesh 1)", stufe=2),
    "P1DO": dict(basis="DOOR1D", blatt="DOOR1D", ton="eigen DOOR1D",
                 rezept="Lueftungsgitter -> schmales dunkles Schild + gelbes Warndreieck, orange",
                 griff="Druecker flach im Griffkasten (DOOR1D Mesh 1)", stufe=2),
    "P26W": dict(basis="DOOR26", blatt="DOOR26", ton="F7 (26 31, Schott)",
                 rezept="einteiliges Schott dunkel, zwei gelb-schwarze Warnstreifen, TYPE-P-Schilder weg, Handrad rot",
                 griff="Handrad (DOOR26 Mesh 3)", stufe=2),
    "P24B": dict(basis="DOOR24", blatt="DOOR24", ton="F6 (24 29)",
                 rezept="Aushang weg, Sichtfenster flacher/schmaler", griff="Druecker schraeg (DOOR24 Mesh 1)", stufe=2),
    "P07D": dict(basis="DOOR07", blatt="DOOR07", ton="F3",
                 rezept="schwarzgrau glatt, kleines Feld mit abgeschnittener Ecke, Piktogramm Frau, blaues Schild",
                 griff="Druecker flach (DOOR07 Mesh 1)", stufe=2),
    "P07H": dict(basis="DOOR07", blatt="DOOR07", ton="F3",
                 rezept="wie P07D, Piktogramm Mann", griff="Druecker flach (DOOR07 Mesh 1)", stufe=2),
    "P27S": dict(basis="DOOR27", blatt="DOOR27", ton="eigen DOOR27 (Labor-Schiebetuer)",
                 rezept="Laborseite ROOM5060: linkes Feld gelbes Strahlenwarnschild, rechts Fensterkasten + rotes Dreieck, blaugrau",
                 griff="-", stufe=2),
    "P27K": dict(basis="DOOR27", blatt="DOOR27", ton="eigen DOOR27 (Labor-Schiebetuer)",
                 rezept="Gangseite ROOM5040/5120: quadratisches Sichtfenster, rotes Dreieck, gelb-schwarzer Aufkleber, "
                        "schmales Feld rechts (anders gemalt als die Laborseite: je Seite ein Archiv)",
                 griff="-", stufe=2),
    "P27O": dict(basis="DOOR27", blatt="DOOR27", ton="eigen DOOR27 (Labor-Schiebetuer)",
                 rezept="Gangseite ROOM5120 (S305): Malerei wie P27K, aber orange gemalt (Raumlicht) - je Seite ein Archiv",
                 griff="-", stufe=2),
    "P1AZ": dict(basis="DOOR1A", blatt="DOOR1A", ton="F4",
                 rezept="zwei Felder + senkrechte Rahmenleiste rechts + dunkles Rechteck oben, graugruen",
                 griff="Druecker (DOOR1A Mesh 1)", stufe=2),
    "P07M": dict(basis="DOOR07", blatt="DOOR14", ton="F3",
                 rezept="Holz-/Rostrahmen, oben + unten Maschendraht (DOOR14-Raute, Loecher Texel 0), Mittelriegel",
                 griff="Druecker flach (DOOR07 Mesh 1)", stufe=2),
}

# RE2-Archive ohne Objekt (tueren_02_re2.md 1.1 "Ohne Objekt": nur Blende + Ton)
OBJEKTLOS = "RE2 objektlos DOOR36 (Blende + zwei kurze Einsaetze, Bild 80/140; Wahl nach Huellkurve, tools/tueren/tuer_g12_ton.py)"
OBJEKTLOS_BASIS = "DOOR36"

# Seiten, die anders gemalt sind als ihre Gegenseite(n): eigenes Archiv je Seite (Stufe 2, Pilot-Punkt c;
# wie T131 in Runde 31). Alle anderen Seiten tragen das Archiv ihrer Tuer.
SEITE_ARCHIV = {"S023": "P07T", "S024": "P07T", "S025": "P07T",   # Treppenhaus ROOM1060, hell graugruen
                "S269": "P27K", "S305": "P27O",
                "S155": "P1BD", "S156": "P1BD", "S157": "P1BD", "S158": "P1BD"}   # ROOM2070: dunkel                   # Gangseite der P-4-Labortuer

# ----------------------------------------------------------------------------------------------
# Gruppen: Tuer -> Archiv (+ Begruendung). Reihenfolge = Tabelle.
# ----------------------------------------------------------------------------------------------
GRUPPEN = [
    ("G1", "Glatte dunkle STAGE1-Stahltuer (~DOOR07 ohne Schlitze)", "P07G",
     "T000 T001 T002 T003 T004 T006 T012 T013 T015 T016 T021 T022",
     "DOOR07 = Blechtuer mit Druecker flach (gemalte Griff-Form gleich, kein Tausch), Ton F3 Blech; "
     "Abweichung nur Lueftungsschlitze + graue Flecken + Farbe -> reine Textur"),
    ("G2", "Fabrik-Stahltuer mit zwei gerahmten Feldern + Stangengriff (~DOOR08)", "P06F",
     "T098 T103 T105 T106 T107 T094",
     "Gestaltung naechst DOOR08, Griff-Form Stange -> Basis DOOR06 (Buegelgriff, gleiche Tonfamilie F3 wie "
     "DOOR08) statt Griff-Tausch; Blatt aus DOOR08 bearbeitet. T094 (graue Stahltuer, zwei gerahmte Felder, "
     "Gegenseite S206 'wie S189/S213') gleiche Gestalt; S204 (T106, Platzhalterraum ohne Blatt) bekommt das "
     "Archiv der Tuer (Regel 2 T3, Beta->Retail)"),
    ("G2b", "Fabrik-Stahltuer mit ungleichen gepraegten Feldern (S188)", "P06U", "T096",
     "wie G2, aber andere Feldteilung (oben hoch, unten fast quadratisch)"),
    ("G3", "DOOR23-Panzer-Doppeltuer", "P1B3", "T071 T076 T081 T082",
     "jeder Fluegel = DOOR23-Gestaltung, aber zweifluegelig; beide Fluegel gehen -> 1B (RE2s einzige "
     "Stahl-Doppeltuer mit beiden Fluegeln, Paar 2/3); Riegelstange per Griff-Tausch (Spender DOOR23)"),
    ("G4", "glatte Stahl-Doppeltuer mit zwei Drueckern", "P1DG", "T026 T054",
     "Doppeltuer mit Druecker flach = DOOR1D V2/V3 (Blech, so schon an T097); Blatt ohne Lueftungsgitter"),
    ("G4b", "Stahlrahmen-Doppeltuer mit drei Feldern + zwei Drueckern", "P1DK", "T014",
     "wie G4, Blatt mit Feldern (Aufteilung aus DOOR1A, ohne Glas; S030 ROOM1090 c03: drei Felder je Fluegel)"),
    ("G4c", "Stahlrahmen-Doppeltuer mit hohem Feld + Querriegel", "P1DL", "T045",
     "wie G4b, aber zwei Felder (S080 'hohes Feld oben und Querriegel', S121 'oberes und unteres Feld')"),
    ("G5", "Holz-Doppeltuer mit Kassetten + langen Messingstangen", "P04B", "T035",
     "DOOR04 hat dieselbe Aufteilung + dieselben Stangengriffe, nur blau -> Umfaerben; Ton F2 = Holz"),
    ("G5b", "helle Holz-Doppeltuer mit Drahtglas + Panikstange", "P0CD", "T025",
     "DOOR0C = Holz-Doppeltuer, beide Fluegel, senkrechte Stangengriffe an der Fuge (gemalt); Holzton 0C"),
    ("G6", "Messingleiter ROOM1260", "P16M", "T023 T052 T053",
     "DOOR16-Bewegung/Ton; Schachtseiten folgen ihrer Leiter (T3 3.1); nur Farbe"),
    ("G6b", "Rostleiter ROOM11A0", "P16R", "T047",
     "wie G6, Farbe rostbraun; Holme flach statt rund bleibt Mesh (Textur-only)"),
    ("G7", "Lamellen-Lueftung", "P1EL", "T029 T078",
     "DOOR1E-Klappe; T078 nur die Seite S154 (S146 ist gleich DOOR1E und gebaut)"),
    ("G7b", "Lueftungsoeffnung mit Staeben unten", "P1EU", "T080", "DOOR1E-Klappe, Staebe nur unten"),
    ("G8", "Aufzugtuer in Nische (Kabine ROOM1080)", "P25G", "T011 T017 T018",
     "Hubtuer DOOR25 (RE2-Aufzugtuer TYPE-L, beide Seiten V0); nur die begehbare Seite (Kabinenseite = Skript)"),
    ("G9", "Lastenaufzug-Gittertor", "P14A", "T102", "Schiebe-Maschendrahttor DOOR14 (Klasse gleich)"),
    ("G10", "Lastenaufzug offener Schacht", "P2DS", "T112", "Hubbuehne DOOR2D (V4 hinauf), Tafel/Lampe weg"),
    ("G11a", "Rost-Stahltuer ROOM11A0 (Selbst-Tuer)", "P07R", "T049",
     "DOOR22-Gestaltung hat keinen Griff-Mesh (Riegel gemalt) -> Basis DOOR07 (Druecker, Ton F3 = DOOR22-Ton)"),
    ("G11b", "braune Stahltuer, Profil ohne Ausbuchtung, Druecker auf Platte", "P1AP", "T050 T072 T073",
     "DOOR23-Profil, Griff Druecker -> Basis DOOR1A (Druecker auf Kasten, Ton F4 = DOOR23-Ton)"),
    ("G11c", "orange Blechtuer mit Warndreieck", "P1DO", "T074", "DOOR1D mit Griffkasten + Sockel, Gitter -> Schild"),
    ("G11d", "dunkle Stahltuer mit Warnstreifen + Handrad", "P26W", "T084",
     "Tuer mit Handrad = Schott DOOR26 einteilig (V2/V3); S165 (schwarze Oeffnung) bekommt das Archiv der Tuer"),
    ("G11e", "beige Labortuer", "P24B", "T095", "DOOR24 ohne Aushang"),
    ("G11f", "Toilettentuer Damen ROOM4050", "P07D", "T123", "glatte dunkle Tuer mit Druecker = DOOR07-Basis"),
    ("G11g", "Toilettentuer Herren ROOM4050", "P07H", "T126", "wie G11f"),
    ("G11h", "P-4-Labortuer mit Strahlenzeichen", "P27S", "T142", "zweiteilige Labor-Schiebetuer DOOR27"),
    ("G11i", "Zug-Innentuer (einseitig, S322)", "P1AZ", "T161",
     "Rahmenleisten + Druecker = DOOR1A; nur S322 (S314 ist gleich DOOR2A und gebaut)"),
    ("G11j", "Maschendraht-Gittertuer (einseitig, S087)", "P07M", "T048",
     "Drehtuer mit Druecker = DOOR07-Basis, Maschendraht aus DOOR14; nur S087 (S159 ist gleich DOOR0A)"),
    ("G12", "Durchgang ohne Blatt", "OBJEKTLOS", "T019 T085 T086 T117",
     "RE2 kennt keinen begehbaren Durchgang ohne Tuerobjekt (534 Seiten); ohne Objekt zeigt RE2 nur Blende + "
     "Ton (objektlose Archive, nur an Skript-Uebergaengen: ROOM2080 DOOR32, ROOM2140 DOOR34, ROOM4100 DOOR20/36)"),
    ("G13", "ausgenommen: Tor (fertig)", "-", "T041", "eigene Sequenz v0.8.16"),
    ("G14", "ausgenommen: inert (sce 0, nie scharf)", "-", "T064 T065 T066",
     "ROOM1250 Slot 0-2: pc[2] = sce 0 -> Handler 0 @0x8004305C (Tabelle @0x8007469c) tut nichts, nie per "
     "Aot_reset umgetypt (shots/aot_sce_census.md); im Spiel nie begehbar"),
]

# Sonder-Variantenregeln je Basis (tueren_02_re2.md 2.4 / T3 Regel 4)
DOPPEL_1D = {"A": 2, "B": 3}     # 1D Doppeltuer Seite A V2 / B V3 (wie T097)
DOPPEL_1B = {"A": 3, "B": 2}     # 1B Seite A weg V3 / B hin V2 (wie T028)
DOPPEL_0C = {"A": 1, "B": 0}     # 0C V1 weg / V0 hin: Seite A weg, B hin
STANDARD = ("DOOR07", "DOOR06", "DOOR1A", "DOOR24")


def variante(archiv, tuer, seite, idx, seiten):
    """PORT-WAHL nach tueren_02_re2.md 2.4; Rueckgabe (Variante, Herkunft)."""
    b = ARCHIVE[archiv]["basis"]
    g = seite["griff_seite"]
    if b in STANDARD or (b == "DOOR1D" and archiv == "P1DO"):
        if g == "links":
            return 0, "Griff links -> V0 (T2 2.4 Punkt 1)"
        if g == "rechts":
            return 1, "Griff rechts -> V1 (T2 2.4 Punkt 1)"
        # unsichtbar: Komplement der Gegenseite, sonst Seite A V0 / B V1; Zwilling wie Hauptseite
        if seite.get("zwilling_von"):
            return None, "Zwilling"
        andere = [s for s in seiten if s["id"] != seite["id"] and not s.get("zwilling_von")
                  and s["griff_seite"] in ("links", "rechts")]
        if andere:
            v = 1 if andere[0]["griff_seite"] == "links" else 0
            return v, "Griff unsichtbar -> Komplement zu %s" % andere[0]["id"]
        return (0 if idx == 0 else 1), "auf keiner Seite sichtbar -> Seite A V0 / B V1"
    if b == "DOOR1D":
        return DOPPEL_1D["A" if idx == 0 else "B"], "1D-Doppeltuer Seite %s (wie T097)" % "AB"[min(idx, 1)]
    if b == "DOOR1B":
        return DOPPEL_1B["A" if idx == 0 else "B"], "1B beide Fluegel Seite %s (wie T028)" % "AB"[min(idx, 1)]
    if b == "DOOR0C":
        return DOPPEL_0C["A" if idx == 0 else "B"], "0C beide Fluegel Seite %s weg/hin" % "AB"[min(idx, 1)]
    if b == "DOOR04":
        return (2, "Fluegel rechts -> V2 (T2 2.4 Punkt 2)") if g == "rechts" else (3, "Fluegel links -> V3")
    if b == "DOOR16":
        auf = seite.get("_auf")
        return (4, "hinauf -> V4") if auf else (5, "hinab -> V5")
    if b == "DOOR2D":
        return 4, "hinauf (ROOM4000 -> ROOM3070 Level-1 nach oben) -> V4 (tueren_04_bau.md 1.3)"
    if b == "DOOR26":
        if g == "links":
            return 3, "einteilig, Rad links -> V3 (T2 2.4 Punkt 3)"
        return 2, "Rad unsichtbar -> Komplement V2"
    if b in ("DOOR1E", "DOOR25", "DOOR27"):
        return 0, "%s nur V0 (RE2 beide Seiten V0 bzw. V0..V3 gleich)" % b
    if b == "DOOR14":
        return 0, "Griff unsichtbar -> V0 (Seite A)"
    raise SystemExit("keine Variantenregel fuer %s" % b)


# Leitern: hinauf/hinab je Seite (Ziel liegt hoeher/tiefer; aus den Merkmalen der Runde 31)
LEITER_AUF = {"S130": True, "S038": False, "S128": True, "S095": False, "S131": True, "S099": False,
              "S088": True, "S173": False}
EINSEITIG_GEBAUT = {"S159", "S146", "S314"}   # schon abgedeckt (Runde 31), bleiben unberuehrt


def main():
    z = json.load(open(ZUORDNUNG, encoding="utf-8"))
    tueren = {t["id"]: t for t in z["tueren"]}
    plan = {"stand": "Runde 33 Stufe 2", "quelle": os.path.relpath(ZUORDNUNG, REPO).replace("\\", "/"),
            "archive": ARCHIVE, "gruppen": [], "tueren": {}}
    gesehen = set()
    zeilen = ["| Gruppe | Tuer | Raeume | Seiten (Variante) | Port-Archiv | Basis (Bewegung/Ton/Griff) | Blatt aus | Stufe |",
              "|---|---|---|---|---|---|---|---|"]
    for gid, titel, arch, ids, grund in GRUPPEN:
        ids = ids.split()
        plan["gruppen"].append(dict(id=gid, titel=titel, archiv=arch, tueren=ids, grund=grund))
        for tid in ids:
            if tid in gesehen:
                raise SystemExit("%s doppelt" % tid)
            gesehen.add(tid)
            t = tueren[tid]
            if t["status"] == "abgedeckt":
                raise SystemExit("%s ist schon abgedeckt" % tid)
            eintrag = dict(gruppe=gid, archiv=arch, grund_r31=t["grund"], seiten=[])
            if arch == "OBJEKTLOS":
                eintrag["basis"] = OBJEKTLOS_BASIS
            seiten = [s for s in t["seiten"] if s.get("begehbar")]
            haupt = {}
            for i, s in enumerate(seiten):
                if s["id"] in EINSEITIG_GEBAUT:
                    eintrag["seiten"].append(dict(id=s["id"], raum=s["raum"], bau=False,
                                                  herkunft="gleich (Runde 31, schon gebaut)"))
                    continue
                if arch in ARCHIVE:
                    s = dict(s)
                    s["_auf"] = LEITER_AUF.get(s["id"])
                    idx = len([e for e in eintrag["seiten"] if e.get("bau") and not e.get("zwilling")])
                    arch_s = SEITE_ARCHIV.get(s["id"], arch)
                    v, h = variante(arch_s, t, s, idx, seiten)
                    zw = s.get("zwilling_von")
                    if v is None or (zw and zw in haupt):
                        v, h = haupt[zw], "Zwilling von %s" % zw
                    haupt[s["id"]] = v
                    e_s = dict(id=s["id"], raum=s["raum"], bau=True, variante=v,
                               herkunft=h, zwilling=bool(zw), griff=s["griff_seite"])
                    if arch_s != arch:
                        e_s["archiv"] = arch_s
                    eintrag["seiten"].append(e_s)
                elif arch == "OBJEKTLOS":
                    # objektloses RE2-Archiv hat nur V0 (Skript 0 verteilt 0, tueren_02_re2.md 1.1)
                    eintrag["seiten"].append(dict(id=s["id"], raum=s["raum"], bau=True, variante=0,
                                                  herkunft="kein Tuerobjekt -> %s V0 (Blende + Ton)" % OBJEKTLOS_BASIS))
                else:
                    eintrag["seiten"].append(dict(id=s["id"], raum=s["raum"], bau=False,
                                                  herkunft="kein Tuerobjekt"))
            plan["tueren"][tid] = eintrag
            raeume = " <-> ".join(sorted({s["raum"] for s in seiten}))
            sv = "; ".join("%s%s%s" % (e["id"], (" V%d" % e["variante"]) if "variante" in e else
                           (" (gebaut R31)" if not e["bau"] and "gleich" in e["herkunft"] else ""),
                           (" " + e["archiv"]) if e.get("archiv") else "")
                           for e in eintrag["seiten"])
            a = ARCHIVE.get(arch)
            zeilen.append("| %s | %s | %s | %s | %s | %s | %s | %s |" % (
                gid, tid, raeume, sv, arch,
                ("%s: %s; %s" % (a["basis"], a["ton"], a["griff"])) if a else (OBJEKTLOS if arch == "OBJEKTLOS" else "-"),
                a["blatt"] if a else "-", (a["stufe"] if a else "-")))
    alle = {t["id"] for t in z["tueren"] if t["status"] != "abgedeckt"}
    if gesehen != alle:
        raise SystemExit("Plan deckt nicht alle 61: fehlt %s, zuviel %s" % (sorted(alle - gesehen), sorted(gesehen - alle)))
    os.makedirs(AUS, exist_ok=True)
    with open(os.path.join(AUS, "plan.json"), "w", encoding="utf-8") as f:
        json.dump(plan, f, ensure_ascii=False, indent=1)
    with open(os.path.join(AUS, "plan_tabelle.md"), "w", encoding="utf-8") as f:
        f.write("\n".join(zeilen) + "\n")
    print("Plan: %d Tueren, %d Gruppen, %d Port-Archive" % (len(gesehen), len(GRUPPEN), len(ARCHIVE)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
