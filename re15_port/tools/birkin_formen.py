#!/usr/bin/env python3
"""Beta-Weiterentwicklung des Birkin-Modells EM30 (Typ 0x30) und roter Affe erzeugen.

    python re15_port/tools/birkin_formen.py                 # EMDs schreiben
    python re15_port/tools/birkin_formen.py --blatt DIR     # + Ansichtsblaetter nach DIR

Ausgabe (vom PC-Port ueber RE15_BIRKIN_FORM geladen, platform/pc/main.c):
    re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD     Hangar 50A0/50A1 und Labor 50E0/50F1 (RE15_BIRKIN_FORM=hangar)

⛔ Seit v0.8.30 (Nutzerfreigabe 2026-11-08 "Naja, gehen wir so erst mal ins rennnen.") ist das Spiel-Asset EM30_HANGAR.EMD
die Sichel-Form (Pin release/em30_hangar.sha256), NICHT mehr die Ausgabe dieses Skripts. Auf dem Zweig r41/sichel_v8 ist es V8
(Koerperbau nach Nutzer-Skizze, sha256 fa31156c...); V8 baut re15_port/tools/r41_sichel/v8_kette.sh neu (V7-Kette Referenz 2 ->
--variante v4 -> re2kopf -> v6kopf -> v5koerper -> v6drei -> v7hand, dann v8/v8_bau.py; Dossier SF_sichel.md "V8 Koerperbau nach
Nutzer-Skizze (2026-11-08)"; V7 allein: v7_kette.sh, sha256 319bbd17). Dieses Skript OHNE --variante zu starten
ueberschreibt EM30_HANGAR.EMD mit dem Grundstand Referenz 2 (sha256 cfd1ac42...) - also nicht ins Spiel-Asset laufen lassen;
--variante NAME --aus DIR (ausserhalb von shared_assets) bleibt erlaubt und ist Schritt 1 der V7-Kette.

⛔ KEIN BYTE-TRUE-ASSET. Diese Form existiert auf der ausgelieferten Disc nicht. Sie ist NUR aus
Beta-Screenshots bekannt (Hangar C-2: Birkin packt Elza und schleudert sie). Jede Zahl in FORMEN
unten ist eine GESTALTUNGSENTSCHEIDUNG nach diesen Bildern, kein RE-Wert, und hat deshalb
absichtlich KEINE @0x-Adresse. Sie darf nirgends als Original-Konstante zitiert werden.

Die Bahnsteig-Form ist seit Nachtrag 38 kein umgehaltener EM30 mehr, sondern RE2-G4 als Vorlage:
tools/birkin_bahnsteig.py. umhalten()/haltung/gang bleiben hier als Werkzeug, keine Form nutzt sie derzeit.

Byte-true geerbt (unveraendert aus CDEMD0.EMS Blob 16 = EM30):
    EDD (alle 21 Clips), EMR-Hierarchie, Keyframes (Hangar), Dreieckstopologie, UVs,
    TPages, CLUT-Zuordnung, TIM-Pixel. Nur diese Dinge aendern sich:
      1. MD1-Vertices je Knochen  - skaliert um den Knochenursprung (Mesh-lokaler Raum)
      2. MD1-Normalen             - inverse-transponiert mitgefuehrt, Laenge beibehalten
      3. EMR-Bindoffsets          - Kindgelenk mit der Skalierung des Elternmeshes mitgezogen,
                                    damit die Glieder verbunden bleiben
      4. (faerbe_tim: CLUT-Einfaerbung, derzeit von keiner Form genutzt)
      5. Hangar (Runde 41 Spur SF): Schwanz als eigene Knochenkette 16-22 an Knochen 8 (schwanz_kette): EMR-Bank 0
         bekommt 7 Knochen und je Keyframe 7 x 36 Bit dahinter (die ersten 84 Byte jedes Keyframes bleiben EM30),
         Mesh 16 (knochenlose Hand mit Rohr) entfaellt, Meshes 16-22 = Glieder. Sektionen und Verzeichnis werden
         neu gerechnet. Vorher (bis Runde 41) war der Schwanz starre Geometrie an Mesh 8.
      6. Hangar: Umtexturieren einzelner Teile (umtexturieren: nur UV/CLUT/TPage, vorhandene Texel)
    Der MD1-Serializer schreibt unveraendertes EM30 byte-gleich zurueck (Assert in main), und
    re15_emd_parse_container liest die Datei ohne Aenderung (tests/unit/test_birkin_formen.c).

Knochen von EM30 (aus dem EMR gelesen, Parent in Klammern):
    0 Becken (Wurzel)   1-3 Bein rechts  4-6 Bein links   7 Bauch (0)   8 Brust (7)
    9 Kopf (8)          10-12 Arm links (8)               13-15 Arm rechts mit Sichel/Klaue (8)
    16-22 Schwanzkette (8, dann je Vorglied) - nur EM30_HANGAR
"""
import argparse, os, struct, sys
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import emd_ansichtsblatt as EA          # noqa: E402  (Leser: EMS-Kette, MD1, EMR, TIM)

AUS_DIR = os.path.join(EA.REPO, 're15_port', 'shared_assets', 'PSX', 'EMD')
TYP     = 0x30

# Gestaltung je Form (siehe Docstring: Nachbildung nach Screenshots, keine RE-Werte).
#   skal[b]  = (sx, sy, sz) im Mesh-lokalen Raum von Knochen b. Y laeuft bei den
#              Gliedmassen entlang des Knochens (Laenge), X/Z ist die Dicke.
#   rel[b]   = (dx, dy, dz) Zusatzverschiebung des Bindoffsets von b NACH dem Mitziehen.
#   tint     = None oder (Matrix 3x3 auf RGB, Helligkeit) fuer alle CLUT-Eintraege.
FORMEN = {
    # Bilder 2-10 (Hangar C-2): der vorhandene Koerper, aber oben massiger. Buckel und
    # Schultern breiter und hoeher, Kopf tiefer zwischen die Schultern gesunken, beide
    # Arme deutlich dicker und etwas laenger (Haende auf Kniehoehe), Beine unveraendert.
    'hangar': dict(
        datei='EM30_HANGAR.EMD',
        titel='Birkin Hangar-Form',
        # Runde 41 SF N2: Brust quer (z) 1,30 -> 1,15 und Arme in der Dicke 1,30-1,40 -> 1,17-1,23 (Anteil 0,57 der alten
        # Verdickung). Video 4 (von Hand nachgezeichnete Umrisse, SF_belege/nb2) ist schmaler als die Form: Hoehe/Breite
        # Video 1,70 im Mittel (h0/40/64/100/116/124), Port mit Haltung 1,47; Gittersuche (tools/r41_sichel/nb2/anpassen.py
        # form_variante): IoU gleich (0,667 -> 0,665), H/B 1,65. Schultern/Arme stehen im Video nicht seitlich ab.
        # Runde 41 SF Neuaufbau ab NB2 (2026-11-03, Nutzer: "Das 2. Bild von links ... sah doch, bis auf die Farbe im Vergleich
        # zum Original recht gut aus"): Koerpermasse wie NB2 (6a1e9430d). Uebernommen aus NB3 nur der Nutzerwunsch "linke
        # Klauenhand kleiner, nach dem Video": Klaue (12) 0,55/0,54/0,55 der EM30-Klaue (vorher NB2 1,171/1,15/1,171; Video
        # h124/h132 Hand ~40 x 30 Bildpunkte gegen ~106 x 60, Abnahme 1 5c) und Klauen-Unterarm (11) Laenge 0,80 (Dicke wie NB2).
        skal={
            7:  (1.15, 1.10, 1.15),
            8:  (1.30, 1.25, 1.15),
            9:  (1.05, 1.05, 1.05),
            10: (1.20, 1.12, 1.20), 11: (1.229, 0.80, 1.229), 12: (0.55, 0.54, 0.55),
            13: (1.20, 1.12, 1.20), 14: (1.229, 1.12, 1.229), 15: (1.171, 1.15, 1.171),
        },
        # Runde 41 SF N2 (Kopf): vorher (60, 170, 0) = Kopf 170 tiefer zwischen die Schultern (Beta-Screenshots). Video 4
        # zeigt die rote/helle Kopfkuppe AN der Oberkante des Umrisses (h0 2, h100 0, h116 3, h124 5 Bildpunkte darunter;
        # Port vorher 14-25, mit der geduckten Haltung 31-39) -> Kopf 350 hoeher im Brustraum (Gittersuche
        # tools/r41_sichel/nb2/kopfmass2.py: dy -1/8/8/4). Groesse unveraendert (sichtbare Breite Video 20-30, Port 19-23).
        rel={9: (60, -180, 0)},
        tint=None,
        # Runde 41 SF N2 (Haltung): geduckt, breitbeinig, Oberkoerper vor, Arme naeher am Koerper - fuer ALLE Clips der Bank 0
        # (ducken()). Werte = Koordinatensuche gegen die von Hand nachgezeichneten Video-Umrisse h0/h40/h64/h100/h116/h124
        # (SF_belege/nb2/umrisse_video.json; tools/r41_sichel/nb2/anpassen.py, Lage aus dem Video-Fusspunkt, Clip-Bild/Gier je
        # Bild frei): IoU nach Verschiebung 0,585 -> 0,667. GESTALTUNG, keine RE-Werte.
        # Neuaufbau ab NB2: Haltung wie NB2; aus NB3 nur der Klauen-Ellbogen 60 Grad nach vorn (Teil des Wunsches "linke
        # Klauenhand kleiner, nach dem Video": die kleine Klaue endet auf Oberschenkelhoehe wie im Video h124/h132).
        ducken=dict(becken=110, spreiz=340, knie_aussen=0.2, rumpf=3, brust=20, kopf=0, arm_halten=0.575, arm_innen=20,
                    ellbogen_links=60),
        # Nutzerhinweis 2026-10-30: "Die Hangar-Form hat auch direkt einen Tail am Ruecken."
        # Runde 41 Spur SF, Nutzerurteil 2026-11-02 "Der Schwanz ist viel zu starr": der Schwanz ist jetzt eine
        # eigene Kette aus 7 Knochen (16-22), angehaengt an Knochen 8 (Brust), Lage der Ruhegestalt wie bisher
        # (Mittellinie im Knochen-8-Raum, X vorn, Y unten). Gegen Complete Disc Video 4 geprueft (Teil T7 im
        # Dossier): in Clip 0x0B Bild 20 liegt er dort, wo Bild 1408-1420 das rot-weisse Glied zeigt (rechts
        # neben der Huefte bis zum Boden), von vorn haengt er zwischen den Beinen (1484-1500). Die Winkel je
        # Keyframe rechnet schwanz_bewegung (Nachschwingen aus der Rumpfbewegung, Gestaltung).
        schwanz_kette=dict(
            knochen=8,
            # Runde 41 SF NB4 (Abnahme 2 A3/A4): Laenge x1,1 um den Ansatz (Glied 424 -> ~466, gesamt 2968 -> ~3265). Video h22:
            # Schwanz sichtbar von (325,275) bis zum Boden (238,408) = 2556 Einheiten (Rueckprojektion Cut 12) plus der verdeckte
            # Teil Ansatz -> Austritt (>= 755); mit 7 x 424 war die Spitze h22 nicht erreichbar (50 Bildpunkte, Lauf nb4/t1).
            punkte=[(-380, 120, 0), (-798, 450, 66), (-1227, 934, 132), (-1612, 1506, 99),
                    (-1898, 2056, -22), (-2052, 2496, -132), (-2118, 2760, -176)],
            # Runde 41 SF NB3 (Abnahme 1 Mangel 1/3): Wurzel-Durchmesser 300 statt 600 (Video h20 ~20 Bildpunkte bei Tiefe
            # ~7000, h66-h70 15-18), 10 Seiten, gegliedertes Profil (Gelenk 0,8 / Mitte 1,05), Deckel in den Rumpf, Dorn
            # 4 x Endradius.
            # Runde 41 SF NB4 (Abnahme 2 A4: Dicke h20-h24 Port ~6-10, Video ~15-20 Bildpunkte = Durchmesser ~230-300 bei
            # Abstand ~7000): der vordere Schwanz bleibt dick (Radien 150 -> 84 statt 150 -> 26), Dorn 3 x Endradius.
            radien=[150, 146, 140, 132, 120, 102, 72],   # Lauf nb4/t10 mit ... 84: rechts h64-h70 825-964 rote Bildpunkte, Video 356-631
            glieder=7, seiten=10, ueberlapp=0.45,
            profil=[(0.0, 0.80), (0.12, 0.97), (0.45, 1.05), (0.80, 0.97), (1.0, 0.80)], dorn=3.0,
            # Runde 41 SF N1.5: rot-weiss gebaendert wie im Video (Schwanz h18-h28 Bild-links, h58-h70 Bild-rechts; vorher
            # Fleischtextur TPage 128 u 4-60 v 4-124). Bandstreifen TPage 130 u 0-42 v 92-130 (senkrechte hellere Streifen),
            # Laenge auf u (laengs='u') -> die Streifen liegen als Ringe um den Schwanz; eigene CLUT-Zeile 483 (cluts unten).
            # NB3: eigene gemalte Textur (schwanz_textur) in TPage 130 u 0-63 v 176-239, je Glied die ganze Hoehe.
            uv=dict(clut=483 << 6, page=130, u=(0, 63), v=(176, 239), je_glied=True),
            # Nachschwingen (schwanz_bewegung): Feder und Nachlauf Wurzel -> Spitze, Daempfung je Bild, Schwere, Grenzwinkel
            bewegung=dict(steif=(0.22, 0.08), traeg=(0.55, 0.92), daempfung=0.80, schwere=0.004, grenze=55),
        ),
        # Umtexturieren (nur UV/CLUT/TPage, vorhandene Texel, keine neuen Farben). Quelle der UVs = die
        # Ausdehnung der gewaehlten Faces; Ziel = Rechteck im TIM. Teile T3-T6 im Dossier.
        # Runde 41 SF N1.6: Oberschenkel-Vierecke (Mesh 1/4, Ringe bei y ~76 und ~818) an y 360 teilen, damit die Jeans
        # dort enden koennen (vorher reichte das Viereck 76..818 als Ganzes bis ans Knie). Form unveraendert.
        teilen=[dict(meshes=(1, 4), schnitt=360)],
        umtexturieren=[
            # T5 (Runde 41 SF) zurueckgenommen in N1.2/N1.7: das rot-weisse Glied h18-h28 war der Schwanz, der Unterarm
            # (Mesh 14) traegt wieder die EM30-Haut (im Video h96-h100 rotgrau (55,43,46) am Ellbogen).
            # T7 Sichel (Mesh 15): eigene CLUT-Zeile 484 (dunkel wie im Video h128-h132, cluts unten), UVs unveraendert.
            dict(meshes=(15,), clut=484 << 6),
            # T3 Kurze Hose: Jeans am Schienbein (Mesh 2/5, TPage 129) und am Oberschenkel unterhalb y 360
            # (Mesh 1/4, nach teilen) -> Beinhaut wie der untere Schienbeinteil (TPage 128, CLUT 0, u 19-55 v 55-115).
            # Video h120-h132: Jeans y 136..157, Haut 157..178 (PSX, Blau-/Hautprofil der Beine) = Hosenende bei 62 % von
            # Bund bis Knie; im Port Bund 145, Knie (Knochen 2) 187 -> Ende bei y 171 = Oberschenkel-lokal ~350.
            # Vorher (y_ab 520) traf die Regel nur das Kniedreieck 818-951, die Jeans reichten bis y 187 (Lauf n1).
            dict(meshes=(2, 5), nur_page=129, page=128, clut=30720, u=(19, 55), v=(55, 115)),
            dict(meshes=(1, 4), y_ab=360, page=128, clut=30720, u=(19, 55), v=(55, 115)),
            # T4 Fuesse rot statt schwarzer Schuhe (alle Hangar-Bilder; 1355/1470 rote Fuesse) -> Wundfleisch
            # der Schulter (TPage 128, CLUT 0, u 66-126 v 4-90)
            dict(meshes=(3, 6), page=128, clut=30720, u=(66, 126), v=(4, 90)),
            # T6 Kopf: kein blondes Menschengesicht im Video (vorn 1460-1500: rotes Fleisch oben in der
            # Mitte) -> dasselbe Wundfleisch; Runde 41 SF N1.10 (Nutzer: "Kopf ... noch nicht ganz akkurat"): eigene
            # CLUT-Zeile 485 (Kopf heller und weniger gesaettigt, rosa Lichter wie im Video, cluts unten).
            dict(meshes=(9,), page=128, clut=485 << 6, u=(66, 126), v=(4, 90)),
        ],
        # Runde 41 SF N1.4/N1.5/N1.7: CLUT-Arbeit (nur Farben der Paletten, Texel unveraendert; Werte = Videomessung,
        # abgeglichen an der Buehne tools/r41_sichel/farbmass.py, die das Port-Fenster nachweislich trifft).
        #  Haut (Zeilen 480-482): Video h40-h64 Rumpf L 66 / Rotanteil 0,33 / hell (78,62,61) / rot (72,40,40), Modell vorher
        #   L 94 / 0,16 / (114,91,87) / (82,40,40) -> blasse Eintraege x0,64, rote (r/g >= 1,6) satter; Jeans (b > r) bleiben.
        #  Zeile 483 Schwanz aus Zeile 482: dunkle Streifen rot, helle weisslich (Video 2-Mittel (92,54,55) 63 % /
        #   (137,90,91); Buehne danach (90,47,54) 64 % / (138,96,97)).
        #  Zeile 484 Sichel aus Zeile 482: dunkel, fast unbunt (Video h128-h132 (21,23,18) = 0,58 der Hauthelligkeit; der
        #   Gruenstich ist Videokompression - der dunkle Torbogen daneben misst ebenso (14,16,16)). Lauf n2 mit
        #   (38,46,32)/(76,90,64) wirkte im Fenster olivgruen -> unbunt.
        # Runde 41 SF NB3: gemalte Schwanztextur (schwanz_textur), Werte = Index in Zeile 483 je Laengenanteil des Glieds.
        schwanz_textur=dict(u=(0, 63), v=(176, 239), streifen=20, streifen_n=3, rauschen=14,
                            # Lauf nb3/e7 mit Band 196-202 (bis (172,124,120)): Schwanzkasten h64-h70 19 % hell (L > 110),
                            # p90 L 126; Video 2 %, p90 86 -> Band 150-158, schmaler.
                            # Runde 41 SF NB4 (Abnahme 2 2d: "Wuerstchenkette" - harte dunkle Gelenkringe, Video rot mit hellen
                            # Ringen und weichem Uebergang; A2 "matter"): Gelenk 12-38 -> 62-84, breitere Uebergaenge.
                            laenge=[(0.0, 62), (0.08, 92), (0.17, 150), (0.27, 156), (0.40, 116), (0.80, 108),
                                    (0.92, 84), (1.0, 64)]),
        cluts=dict(
            haut_zeilen=(0, 1, 2),
            haut=dict(dunkel=0.63, q0=1.2, q1=1.6, rot_hell=0.95, satt=0.82),   # NB2 (N1.8); NB3-NB5-Abdunklung nicht uebernommen
            # Schwanz: Schwellen 44/52 = Tal/Gipfel der Streifen (Spalten-L des Bandstreifens 41..63) -> klare Baender
            # Lauf n5 (echtes Fenster, Schwanz h20-h26 Bild-links): (99,66,66) 41 % / (176,128,128) - heller als das Video,
            # die Flanke steht dort im Licht -> helle Farbe x0,78, Rot satter; Lauf n6 (Schwellen 48/56): Farben getroffen
            # (88,50,50) / (142,96,96), aber nur 42 % rot (Video 63 %) -> Schwellen 52/58.
            # Zeile 483 Schwanz, Runde 41 SF NB3 (Abnahme 1 Mangel 1): die Zweifarb-Zeile (Schwellen 52/58) machte den Schwanz im
            # Fenster zu einem flachen rosa Rohr mit drei schwachen Ringen. Jetzt eine Farbtreppe fuer die gemalte Schwanztextur
            # (schwanz_textur): Gelenk dunkel -> rot (Video 2-Mittel (92,54,55)) -> rosa-weiss (Video hell (137,90,91), p90
            # (163,112,113), Glanz h66-h70 ~(200,150,150)).
            zusatz=[dict(verlauf=[(0, (0, 0, 0)), (1, (24, 11, 13)), (40, (50, 21, 23)), (110, (98, 44, 46)),
                                  (150, (124, 64, 64)), (200, (172, 124, 120)), (255, (212, 182, 176))]),
                    dict(aus=2, l0=20, l1=90, dunkel=(40, 40, 35), hell=(84, 81, 72)),   # Zeile 484 Sichel wie NB2
                    # Zeile 485 Kopf aus Zeile 480 (Wundfleisch, L-Quantile 30/70 = 93/126): Video h108-h132 Kopfbereich
                    # Median r/g ~1,5, 40-50 % kraeftig rot, Lichter rosa (p90 ~(104-123,77-104,77-105)), 1,3x so hell
                    # wie der Rumpf; vorher im Fenster (Lauf n10) r/g 2,0, 72 % kraeftig rot, so hell wie der Rumpf.
                    # Erster Stand (110,62,60)/(200,165,160), Schwellen 93/126: im Fenster blassrosa (Lauf k1). Gittersuche an der
                    # Buehne (Gang Clip 1 B34, Kopf = Differenz mit/ohne Mesh 9): (120,60,58)/(175,135,132) 109/140 -> r/g 1,55,
                    # 42 % kraeftig rot, Kopf/Rumpf 1,41; x0,85 fuer Kopf/Rumpf ~1,2 wie das Video.
                    # Runde 41 SF N2: Kopf 350 hoeher -> Kopf/Rumpf an der Buehne (Gang h100-h124) 1,07-1,09 statt 1,2 (Video
                    # 1,11-1,28) -> beide Farben x1,12.
                    dict(aus=0, l0=109, l1=140, dunkel=(114, 57, 55), hell=(167, 129, 125))],   # Zeile 485 Kopf wie NB2
        ),
    ),
}

u16, u32 = EA.u16, EA.u32


# ═══════════════════════════════════════════════════ Runde 41 SF "Schwanz V2": Varianten zur Wahl des Nutzers
# Formblatt V2 (analysis/befunde_runde41/SF_FORMBLATT_V2.md, Bildmessung am Original Video 4, keine RE-Werte). Die Varianten
# werden NICHT ins Spiel-Asset geschrieben (main ohne --variante schreibt weiter Referenz 2); --variante NAME --aus DIR legt
# sie fuer Buehne und Probelaeufe ab. Nach der Wahl des Nutzers wird die gewaehlte Variante zu FORMEN['hangar'].
#   V1 = Schwanz V2 + Kopf V2, Koerper wie Referenz 2;  V2 = dazu Koerper nach K1-K10 (Formblatt V2 Ae6/Ae8/Ae9).
#   Endung _spitz / _kralle = Schwanzende nach F2 (Nutzer waehlt).
SCHWANZ_V2 = dict(
    # Ruhe-Mittellinie im Modellraum der Gangpose Clip 1 Bild 0 (geduckte Haltung, X vorn, Y unten), kette_v2_punkte:
    modellraum=dict(clip=1, bild=0),
    # S2 Wurzelmitte 0,15-0,17 H unter OK; S5 0,23-0,28 H hinter dem hoechsten Buckelpunkt. Im Modellraum (Gang B0, ohne
    # Perspektive) 0,19 H, weil die Spielkamera von oben schaut: in der Seitenansicht h85 (Pose anpassen_v2) liegt die Wurzel
    # dann 0,16 H unter der Oberkante (mit 0,16 im Modellraum: 0,13 H).
    wurzel_h=(0.255, 0.19),
    pfad_h=[(0.08, 0.04), (0.14, 0.08), (0.17, 0.14), (0.19, 0.20), (0.21, 0.26), (0.22, 0.33), (0.21, 0.39),
            (0.19, 0.45), (0.15, 0.51)],                    # S6 Mittellinie R1 (h85), Spitze (0,15 0,51): 0,33 H ueber der Sohle
    laenge=3475,                   # S9: Laenge von Referenz 2 behalten (0,89 H)
    # PORT-WAHL (S9/Ae2 "den Rest durch Seitwaertslauf ... aufnehmen, nicht durch eine tiefere Spitze"): seitliche Wellen
    # im haengenden Teil (ab 0,38 der Kettenlaenge; von vorn hinter Becken und Beinen), Spitze 0,12 H zur rechten Seite (W) =
    # hinter das rechte Bein. Erster Stand (Welle ab der Wurzel, z = a sin 2 pi s) drehte das Wurzelglied 59 Grad aus der
    # Seitenebene: in der Seitenansicht h85 sah man die Wurzel von vorn (Scheibe), der Schwanz wirkte duenn.
    seitlich=dict(wellen=2.0, ab=0.38, spitze=-0.12),
    # Gelenk 16 400 E. im Buckel versenkt (sichtbar beginnt der Schwanz an der Wurzelmitte): weniger Restlaenge fuer die Welle.
    # Probe (sfv2/probe_welle.py, Vorderansicht h118/h125, sichtbare Schwanz-Bildpunkte 640er): ohne Versenkung, eine Welle
    # (+-620 E.): 722/612 - der Schwanz trat in Taillenhoehe beidseits aus dem Umriss (S15 verlangt: hinter dem Koerper);
    # 400 E. versenkt, eine Welle: 252/325; 400 E., zwei Wellen (+-325 E.): 0/0.
    versenkt=400,
    # S10 Dicke quer zur Mittellinie (Durchmesser in H, H = 3880 E.): 0,15-0,2 der Laenge 0,135; 0,3 0,10; 0,4 0,08;
    # 0,5 0,0675; 0,6-0,7 0,0575; 0,9 0,034 -> Radien in Einheiten (halber Durchmesser).
    # Mit den reinen Formblatt-Radien [(0,262) (0,2 262) (0,3 194) (0,4 155) (0,5 131) (0,65 112) (0,9 66)] mass die Seitenansicht
    # h85 0,117 / 0,057 / 0,034 H (Gliederprofil, Lage zur Kamera) -> x1,15 bis 0,65 der Laenge.
    # Mit 400 E. Versenkung beginnt der sichtbare Teil bei s = 0,115 der Kette: Anteile der SICHTBAREN Laenge f -> s = 0,115 +
    # 0,885 f (sonst mass h85 bei 0,2 nur 0,109 H, bei 0,9 0,019 H).
    radien_s=[(0.0, 300), (0.292, 300), (0.381, 223), (0.469, 178), (0.558, 151), (0.690, 128), (0.912, 68), (1.0, 54)],
    # Gliederprofil flacher als Referenz 2 (0,80/1,05), damit die gemessene Dicke dem Radius entspricht; Glieder bleiben sichtbar.
    profil=[(0.0, 0.90), (0.12, 0.98), (0.45, 1.03), (0.80, 0.98), (1.0, 0.90)],
    # S3: Wurzel ohne Stufe in den Buckel -> Wurzelglied mit Kragen (1,25) statt Einschnuerung (0,80).
    profil_wurzel=[(0.0, 1.15), (0.15, 1.06), (0.45, 1.02), (0.80, 0.98), (1.0, 0.92)],
    wurzel_v=0.22,                 # Kragen in der hellen Bandfarbe statt des dunklen Gelenkrings (Lauf: dunkler Block am Buckel)
    # Ae5 (Spitze im Gang 0,32-0,44 H frei halten): Nachschwingen ohne Schwere. Mit schwere 0,004 (Referenz 2) lagen die
    # Keyframe-Spitzen im Gang (Clip 1) bei 0,21-0,31 H statt der Ruhe 0,33 H.
    bewegung=dict(steif=(0.22, 0.08), traeg=(0.55, 0.92), daempfung=0.80, schwere=0.0, grenze=55),
)
SCHWANZ_V2_SPITZ = dict(ende='dorn', dorn=4.0,
                        profil_ende=[(0.0, 0.90), (0.12, 0.98), (0.45, 0.95), (0.80, 0.78), (1.0, 0.55)])
SCHWANZ_V2_KRALLE = dict(ende='kralle',
                         # KI-Seitenansicht h85: Verdickung am Ende 32-35 px = 0,06-0,07 H (Leser A 2.5) -> Klauenhand
                         # (Mesh 12 der Form, schon 0,55 x EM30) x1,4 quer / x1,2 laengs.
                         kralle=dict(mesh=12, skal=(1.4, 1.2, 1.4), gier=0.0, ueberlapp=40.0))
# Kopf V2 (Nutzer-Entscheid F4 2026-11-06, Quelle KI-Fassung; Werte tools/r41_sichel/v2/kopf_entwurf.py):
#   Flaeche TPage 128 u 0-39 v 156-219 (frei: kein Mesh nutzt Seite 0 unter v 155) = 40 x 64 Texel, Texeldichte 0,10 je
#   Einheit wie die Haut (Rumpf 0,05-0,09, Kopf EM30 0,091); Schulterauge u 42-48 v 156-164.
#   abbildung = Parallelprojektion der Vorderansicht h118 (Pose anpassen_v2, Fehler <= 0,8 Texel), blick = Blickrichtung dort.
#   KI birkin19 (Kopfbox 755-820 x 318-440): Auge oben (0,415 0,30), Durchmesser 0,15 der Kopfbreite; Maul x 0,11-0,88
#   y 0,41-0,80. Schulterauge KI (705-715, 360-372) -> Mesh 8 (128, -1156, -589), Normale (0,41 -0,90 0,12), 75 x 104 E.
KOPF_V2 = dict(
    page=128, clut=486, u=(0, 39), v=(156, 219),
    abbildung=[[-0.0007, 0.02925, 0.09677, 25.61496], [0.06312, 0.0739, -0.02374, 192.56861]],
    blick=(-0.775, 0.604, -0.187), schwelle=0.15,
    kuppe=0.22, rauschen=9,
    index=dict(kuppe=205, haut=160, rot=92, lippe=78, zahn=246, zahnschatten=205, innen=12, weiss=252),
    # Maul: KI-Lippenrand x 0,11-0,88; im Spielbild (Lauf sfv2/V1s h118/h128) wirkte das als rundes "O" mitten im Kopf ->
    # Lippen auf die innere KI-Oeffnung (x 0,22-0,78) und tiefer (y 0,47-0,90: Kopfbox im Spiel schliesst die Kuppe von oben ein).
    # Kopf-Nahleiste 05 (erster Stand): rundes dunkles "O" mit Zahnring -> KI zeigt ein senkrechtes Maul, Zaehne oben und
    # unten, rote Lippen; Inneres kleiner, Zaehne nur an Ober- und Unterkante (|sin| > 0,45).
    maul=dict(x=(0.28, 0.72), y=(0.44, 0.93), zahn_aussen=0.84, zahn_innen=0.56, zaehne=10, zahn_bogen=0.45),
    auge=dict(x=0.415, y=0.30, r=(3.0, 3.2)),
    schulterauge=dict(u=(42, 48), v=(156, 164), abstand=14.0, normale=(0.41, -0.90, 0.12),
                      ecken=[(86, -1181, -628), (76, -1175, -554), (180, -1138, -624), (170, -1132, -550)]),
)
# Zeile 486 Kopf V2: Verlauf dunkel -> rot -> Haut -> hell. Rot = Schwanzrot (Video (92,54,55), Zeile 483 (98,44,46)); Haut =
# NB2-Hautton (95,68,67) x1,2 (Kopf/Rumpf im Original 1,16-1,28, N1.10); helle Kuppe nach dem Original-Verhaeltnis (p90 h118
# (106,86,86) gegen Rumpf) im NB2-Ton; Maulinneres KI dunkelste 10 % (31,13,13); Zaehne/Augenweiss KI (172,113,119) bis
# (215,194,194), gedaempft. Keine Leuchtfarben.
KOPF_V2_ZEILE = dict(verlauf=[(0, (0, 0, 0)), (1, (22, 9, 9)), (30, (46, 17, 18)), (78, (92, 40, 42)), (92, (100, 46, 47)),
                              (130, (110, 66, 64)), (160, (114, 82, 80)), (205, (148, 120, 116)), (230, (164, 142, 136)),
                              (255, (190, 170, 162))])


def _variante(schwanz_ende, koerper=None):
    f = dict(FORMEN['hangar'])
    f['schwanz_kette'] = dict(f['schwanz_kette'], **SCHWANZ_V2, **schwanz_ende)
    f['kopf_v2'] = KOPF_V2
    f['cluts'] = dict(f['cluts'], zusatz=list(f['cluts']['zusatz']) + [KOPF_V2_ZEILE])
    if koerper:
        for k, v in koerper.items():
            f[k] = {**f[k], **v} if isinstance(v, dict) and k in ('skal', 'rel', 'ducken') else v
    return f


# Koerper V2 (Formblatt V2 K3-K7 / Ae6, Ae8, Ae9; Frage F1 an den Nutzer - nur in den V2-Varianten). Werte aus der Koordinaten-
# suche tools/r41_sichel/v2/koerper_suche.py gegen die Vorderansicht h118/h125 (Posen anpassen_v2, Spielkamera, H = Oberkante bis
# Sohle im selben Bild): Ziel Bund 0,42 / Saum 0,57 / Breite 0,61 (0,57-0,65) H unter OK, Krallen 0,33 (0,30-0,36) / Sichel 0,22
# (0,17-0,27) H ueber der Sohle. Referenz 2 dort: 0,47-0,49 / 0,65-0,68 / 0,65-0,68 / 0,39-0,44 / 0,15-0,19. Ergebnis der Suche:
# Oberschenkel x1,48, Schienbein x1,08 (laenger, die Jeans enden weiter bei 360 E. unter der Huefte), Rumpf (7/8) x0,90 laengs,
# Brust quer 1,15 -> 1,09, Becken 163 statt 110 tiefer, Arme 9 statt 20 Grad zur Mitte, Klauen-Ellbogen 20 statt 60 Grad, dazu
# Klauen-Unterarm 0,80 -> 0,92 (Krallen in der Suche noch 0,36-0,39 H), Wurzel aller Keyframes 420 hoeher (die Suche mass nur
# den Umriss: mit den laengeren Beinen stand der Koerper sonst 420 E. im Boden). GESTALTUNG nach der Bildmessung, keine RE-Werte.
_SK = FORMEN['hangar']['skal']
KOERPER_V2 = dict(
    skal={1: (1.0, 1.48, 1.0), 4: (1.0, 1.48, 1.0), 2: (1.0, 1.08, 1.0), 5: (1.0, 1.08, 1.0),
          7: (_SK[7][0], _SK[7][1] * 0.90, _SK[7][2]), 8: (_SK[8][0], _SK[8][1] * 0.90, 1.09),
          11: (_SK[11][0], 0.92, _SK[11][2])},
    ducken=dict(becken=163.0, arm_innen=9.0, ellbogen_links=20.0, wurzel_hoch=420.0),
)
VARIANTEN = {
    'v1_spitz':  lambda: _variante(SCHWANZ_V2_SPITZ),
    'v1_kralle': lambda: _variante(SCHWANZ_V2_KRALLE),
    'v2_spitz':  lambda: _variante(SCHWANZ_V2_SPITZ, KOERPER_V2),
    'v2_kralle': lambda: _variante(SCHWANZ_V2_KRALLE, KOERPER_V2),
    'v3':        None,          # Runde 41 SF V3 (Messblatt V3): baue_v3 - Kopf + Schwanz neu, Koerper Referenz 2
    'v4':        None,          # Runde 41 SF V4 (Messblatt V4, Nutzer-Beschreibung): _variante_v4 - Kopf + Schwanz + Farben
}



# ═══════════════════════════════════════════════════ Runde 41 SF "V3" (Messblatt V3, Ablehnung V1/V2 durch den Nutzer 2026-11-06)
# Nutzer 2026-11-06 zu V1/V2: "Die Augen sehen nicht mal ansatzweise aus wie im Original ... Der Grosse Schwanz auch nicht und hat
# komplett die falsche Farbe". Vorlage = KI-Fassung (Nutzerentscheid F4, PORT-WAHL, PSX-Machart); Masszahlen aus
# analysis/befunde_runde41/SF_MESSBLATT_V3.md (Zeilen K1-K15, Abschnitte 2-6, Riegel R1-R32). KEIN RE-Befund, keine @0x-Konstante.
# V3 = Koerper Referenz 2 (FORMEN['hangar'] unveraendert: Meshes 0-15, Skelett 0-15, EDD) + Kopfbild V3 (nur Textur auf Mesh 9,
# Zeile 486) + ein Schulterauge (1 Viereck auf Mesh 8) + Schwanz V3 (Knochen 16-22). Erzeugt nur mit --variante v3 --aus DIR.
SCHWANZ_V3 = dict(
    modellraum=dict(clip=1, bild=0),       # wie V2: Masse im Modellraum der Gangpose Clip 1 Bild 0 (geduckte Haltung)
    # Wurzel: Messblatt 5 "Wurzelmitte unter OK 0,10-0,16 H" (Pruefer h85/h83/h92 0,119/0,133/0,122) und "hinter Buckelmitte
    # Richtwert 0,12-0,20 H". Im Modellraum wie V2 (0,255 / 0,19 H): V1 traf damit den Austritt im Bild h85/h92 mit 0,136 / 0,115 H
    # (Messblatt 5 letzte Zeile, R27 0,10-0,16 erfuellt) - unveraendert uebernommen.
    wurzel_h=(0.255, 0.19),
    versenkt=400,                          # wie V2 (Gelenk 16 400 E. im Buckel, sichtbar ab der Wurzelmitte)
    laenge=3475,                           # Formblatt V2 S9, Laenge von Referenz 2 (Auftrag: "Laenge bleibt")
    # Ruhe-Mittellinie = GLATTER Bogen gleicher Kruemmung (Auftrag V3; Messblatt 4 "Glaette": KI max 28-33 Grad je 0,05 H,
    # 2-4 Knicke > 20 Grad; V1 hatte mit den zwei seitlichen S-Wellen Gelenkwinkel 100-126 Grad in den Keyframes und 100-147 Grad
    # im Spiel, Lauf sfv2/r/V1s debug.log, gemessen tools/r41_sichel/v3/gelenke.py + kf_gelenkwinkel.py). Startrichtung 20 Grad unter der Waagerechten
    # nach hinten (Messblatt 5 Austrittsrichtung: Richtwert 5-30 Grad, Pruefer 8-9, Leser A 30-40), Gesamtdrehung 100 Grad (Messblatt
    # 4: KI h85/h83/h92 124/118/83, Richtwert 80-125). Gleiche Kruemmung = Ruhe-Gelenkwinkel 100/7 ~ 14 Grad. Probe (tools/r41_sichel/v3/
    # probe_schwanz_v3.py, P_port.py h85/h92): 10/110 Grad -> max 35-40 Grad je 0,05 H am Wurzelbogen (R25 rot), 20/100 -> 24-27 Grad,
    # 15/105 -> 28-33 Grad mit 2-3 Knicken.
    bogen=dict(start_grad=20.0, dreh_grad=100.0),
    # Seitlich nur EIN monotoner Zug zur rechten Seite (W) statt zweier Wellen: z = spitze * H * u^2 (u = Anteil der sichtbaren
    # Laenge), Spitze hinter das rechte Bein (PORT-WAHL V2, Formblatt V2 S9/Ae2/S15 "von vorn hinter dem Koerper"). Die Bogenlaenge
    # in der Seitenebene wird so gewaehlt, dass die 7 Sehnen zusammen `laenge` ergeben (Bisektion). 0,16 H statt 0,12 H (V2): Probe
    # vorn (Buehne h118/h125, sichtbare Schwanzbildpunkte 640er, tools/r41_sichel/v3/vorn_probe.py): 0,12 -> 122 / 0, 0,16 -> 0 / 3,
    # 0,20 -> 0 / 185, 0 -> 310 / 219; Seitenriegel R15-R27 mit 0,16 unveraendert gruen (probe_schwanz_v3.py).
    seitlich=dict(spitze=-0.16),
    glieder=7, seiten=10,                  # R29: 7 x (10 Vierecke = 20 Dreiecke) + Deckel 10 + Spitze 10 = 160 <= 212 (EM30-Arm 13-15)
    ueberlapp=0.45,                        # Manschette in das Vorglied (Anteil des Radius, wie Referenz 2)
    # Dicke (Radius in E.) je Anteil s der Kette (0 = Gelenk 16, sichtbar ab s 400/3475 = 0,115). Ziel im Bild (Messblatt 3,
    # R21-R23, ab Austritt s0): 0,2 -> 0,15-0,20 H, 0,5 -> 0,07-0,10 H, 0,9 -> 0,028-0,045 H, stetig duenner bis 0,7 (R24).
    # Startwerte aus V1 (Radius 300 an der sichtbaren 0,2 ergab 0,140 H; 151 an 0,5 -> 0,083 H; 68 an 0,9 -> 0,025 H,
    # Messblatt 7 Punkt 7) hochgerechnet auf die Vorgabe 0,175 / 0,085 / 0,035 H; nachgestellt an der Messung P_port.py
    # (Iterationen im Dossier Abschnitt V3).
    radien_s=[(0.0, 380), (0.115, 380), (0.29, 350), (0.38, 300), (0.56, 200), (0.73, 130), (0.91, 82), (1.0, 68)],
    dorn=1.6,                              # Spitze: Kegel 1,6 x Endradius (spitz auslaufend; Ende Hand/Kralle = F2 offen)
    # Deckel am Glied 0 bleibt (Runde 2, Zahlen-Pruefer V3 r1: R31 Glied 0 0,046 an der Grenze 0,045, der Deckel traegt 10 Dreiecke
    # ohne Texelflaeche): Probe r2k ohne Deckel - Glied 0 0,054, aber R25 h85 34,1 statt 27,4 Grad (offenes Rohrende im Umriss am
    # Austritt), h83 42,6 statt 25,7 Grad (n2_schwanz_mass.py)
    deckel=True,
    # Textur (R31/R32, Auftrag "Texel aus der KOERPERHAUT desselben EM30-Atlas, Marmorierung laengs wie am Arm"): Hautstreifen der
    # Seite 2 (TPage 130), Haut-CLUT-Zeile 482 (Arme, Referenz 2), Streifen A u 44-83 v 30-88, OHNE die roten Querstreifen ab v 92
    # (Bild sfv3/bau/streifen.png aus tools/r41_sichel/v3/arm_uv_richtung.py). Zeilenmittel Streifen A v 52-88: L* 15,8-18,9, a* 8-11, b* 1-6 (tools/r41_sichel/v3/streifen_profil.py) = 0,6 x
    # Hauthelligkeit bei gleichem Farbton (Vorgabe L* 0,63, Messblatt 2 / R15); Streifen B (u 87-126) ist so hell wie die Haut
    # (L* 28-33) und roter (a* 13-16): im ersten Stand mit A/B im Wechsel L* 0,70-0,72 und Rest-Std a* 3,1-3,3 (R20 rot).
    # Laufrichtung: die Marmorierung hat keine Vorzugsrichtung (Strukturtensor L*, Gradientenanteil in v: Streifen A 0,49,
    # EM30-Oberarm Seite 0 0,48; tools/r41_sichel/v3/marmor_richtung.py). Gliedlaenge auf u, Umfang auf v (gespiegelt v0 -> v1 -> v0, PSX-uebliche
    # Spiegelung fuer die Texeldichte der Haut, R31 0,045-0,090): Rest-Std a* 1,7-1,9 (R20 <= 2,0); mit Laenge auf v wie am
    # Arm 2,2-2,6 (probe_schwanz_v3.py). Je Glied ein anderes Fenster/eine andere Laufrichtung (keine Wiederholung je Glied, R20).
    uv=dict(clut=482 << 6, page=130, spiegeln=True, laengs='u',
            fenster=[(44, 30, 83, 88), (83, 32, 44, 86), (46, 34, 81, 86), (81, 36, 48, 84), (48, 38, 79, 84), (79, 38, 50, 82),
                     (52, 42, 77, 82)]),
    bewegung=dict(steif=(0.22, 0.08), traeg=(0.55, 0.92), daempfung=0.80, schwere=0.0, grenze=55,    # wie V2 (Ae5)
                  # Runde 2 (Zahlen-Pruefer V3 r1 6.2: Pose h83 = Clip 11 Bild 85 mit 4 Knicken > 20 Grad, max 35,6 Grad je 0,05 H; die
                  # Buehne zeigt die Keyframes, das Spiel die Feder-Kette mit Knick-Grenze): dieselbe Knick-Grenze wie schwanz_r41.c
                  # glaetten (SW_GLATT_RAND / _MIN / _MAX in Grad) schon in den Keyframes, damit Buehne und Spiel dieselbe Kette zeigen
                  knick=dict(rand=3.0, min=12.0, max=16.0)),
)

# Kopf V3 (Messblatt V3 Abschnitt 1 und 6). Machart wie Kopf V2 (Textur auf der unveraenderten EM30-Kopfgeometrie, Mesh 9;
# gleiche Parallelprojektion KOPF_V2 'abbildung' in die freie Flaeche TPage 128 u 0-39 v 156-219), aber das Bild wird in der
# Messansicht "Kopf vorn nah" entworfen (nah_raster: 0,9 px/E., H = 0,9 x 3865 = 3479 px, Messblatt 8) und je Texel an der
# Stelle abgetastet, an der das Texel dort erscheint - so treffen Groesse und Lage die Riegel R1-R14 unabhaengig von der
# Verzerrung der Projektion. Masse in H (Messblatt-Vorgabe), PORT-WAHL wo vermerkt.
#
# Runde 2 (Pruefung V3 r1 2026-11-06, analysis/befunde_runde41/SF_PRUEFUNG_V3_r1_sicht.md A2/A3/A4/B1/B2, _zahlen.md 5/6.4):
# KI-Belegzahlen aus eigener Messung tools/r41_sichel/v3/n2_ki_kopf.py (Ausgabe sfv3/n2/ki_kopf.json, Kopie in
# analysis/befunde_runde41/SF_belege/v3r2/ki_kopf.json), Fenster dort im Kopf beschrieben:
#   vorn birkin19 (h118, H 569) / birkin20 (h125, H 599):
#     Halo um das Kopfauge (Median L* je Ring, nur oberhalb Augenmitte + 0,01 H): r 0,010/0,015/0,020/0,025/0,030 H =
#       44,9/48,5/36,9/29,7/26,6 bzw. 41,5/47,4/42,1/27,2/20,1; ab 0,035 H 15,6-20,9 (n2_ki_kopf.py ringe).
#     Haut-Streuung (n2_ki_kopf.py streuung): Halo (r 0,0075-0,0175 H) L* P5/P50/P95 30,7/46,0/69,0 bzw. 20,3/44,7/69,2,
#       Std 12,2/13,5; aussen (r 0,025-0,05 H) 10,3/19,1/47,1 bzw. 10,0/18,6/43,3, Std 12,4/10,5.
#     Gesichtsfeld (x +-0,035 H, y -0,035..+0,02 H) / Nachbarhaut (x +-(0,09..0,15) H): 0,867 / 0,872.
#     Maul: Spalt-Anteil am Band 0,27 / 0,26, Spalt Lab [14,4 12,6 5,3] / [11,4 10,6 3,9]; Stummel 16 / 19, je ~0,0031 H, Lab
#       [49,6 22,4 6,8] / [50,2 20,7 6,2]; Glanz max(RGB) > 170: 33 / 29 px = 1,9 / 1,4 % des Bands, RGB (197,139,142) /
#       (199,147,150), Lab [63,6 22,4 7,2] / [66,0 20,0 6,2].
#   Seite birkin12 (h85, H 504): Auge 0,034 H hinter der Schnauzenspitze (388,112), Mitte (371,123), 0,014 x 0,010 H, Lab
#     [7,0 1,5 -0,2], L* Auge / Haut 0,27; Maulprofil (a* > 15) 0,044-0,101 H hinter der Spitze, 0,006-0,034 H unter dem Auge,
#     Lab [27,3 22,8 8,4], 5 Stummel Lab [56,1 20,1 8,4]; Schnauzenhaut oben Lab [43,4 4,5 1,0], unten [26,4 4,5 1,8].
# Helligkeitsbezug: alle KI-Farben werden mit KOPF_V3_HELL auf das Port-Licht gebracht (nur L*, a*/b* wie KI), damit das
# Gesichtsfeld zur Nachbarhaut steht wie in der KI (0,87). Wert = Probe (n2_maul_spiel.py feld_nachbar, Spielkamera h118/h125;
# n2_probe.py): Runde 1 (KI-Farben absolut) 1,35 / 1,28; 0,85 -> 1,04 / 1,01 (Feld L* 32,3 gegen Nachbarhaut 31,1: Sicht-Pruefer A3
# "nicht heller als Nachbarhaut + 5 L*" erfuellt); 0,75 -> 0,95 / 0,91, aber R3 0,0141 H, R4 1,33 und R13 a* +3,0 rot (n2_probe.py L).
KOPF_V3_HELL = 0.85
# Seitenflaechen des Kopfs liegen im RDT-Licht der Seitenkamera dunkler als die Vorderseite: eigener Faktor, damit die Schnauzenhaut
# zur Port-Haut steht wie in der KI (birkin12: oben L* 43,4 / Haut 29,4 = 1,48, unten 26,4 / 29,4 = 0,90; Messung n2_seite_mass.py
# L_oben_haut / L_unten_haut in h85/h92). Wert = Probe (Dossier SV3r2): 1,6 -> Rampenende (243,231,232) = weisser Fleck auf der
# lichtzugewandten Facette; 1,3 -> L* oben / Haut 1,30, unten / Haut 0,65-0,75 (KI 1,48 / 0,90; unten liegt im Port die schattige
# Unterseite, heller nur mit Leuchtfarben).
KOPF_V3_HELL_SEITE = 1.3


def _lab_rgb(L, a, b):
    """CIE-Lab (D65) -> sRGB 0..255, Umkehrung von _lab (gerundet, begrenzt)."""
    fy = (L + 16.0) / 116.0
    f = np.array([fy + a / 500.0, fy, fy - b / 200.0])
    d = 6.0 / 29.0
    x = np.where(f > d, f ** 3, 3 * d * d * (f - 4.0 / 29.0)) * np.array([0.95047, 1.0, 1.08883])
    M = np.array([[3.2404542, -1.5371385, -0.4985314], [-0.9692660, 1.8760108, 0.0415560], [0.0556434, -0.2040259, 1.0572252]])
    c = M @ x
    c = np.where(c <= 0.0031308, 12.92 * c, 1.055 * np.clip(c, 0, None) ** (1 / 2.4) - 0.055)
    return tuple(int(v) for v in np.clip(np.round(c * 255.0), 0, 255))


def _ki(L, a, b, hell=None):
    """KI-Farbe (Lab) im Port-Licht: L* x KOPF_V3_HELL, a*/b* unveraendert."""
    return _lab_rgb(L * (KOPF_V3_HELL if hell is None else hell), a, b)


def _rampe_lab(L0, L1, a, b, n=5):
    return [_ki(L0 + (L1 - L0) * i / (n - 1.0), a, b) for i in range(n)]


KOPF_V3 = dict(
    page=128, clut=486, u=(0, 39), v=(156, 219),
    abbildung=KOPF_V2['abbildung'], blick=KOPF_V2['blick'], schwelle=KOPF_V2['schwelle'],
    nah=dict(clip=1, bild=0, mitte=(150.0, -150.0, 0.0), massstab=0.9, H_px=0.9 * 3865, breite=1200, hoehe=960,
             cx=720, cy=480),                     # P_port_kopf.nah (960 breit, cx 480) um 240 px nach rechts erweitert:
                                                  # das Schulterauge liegt 0,117 H = 407 px links vom Kopfauge (K12)
    # Lage: Augenmitte bei 0,265 der Kopfhoehe (Messansicht). PORT-WAHL: KI K5 "in der Kopfbox y 0,32-0,35"; der Port-Kopf ist in
    # der Messansicht 0,165 H hoch und unten schmal (Kinn) - mit 0,33 laege die Maulmitte (0,052 H tiefer, K5) dort, wo der Kopf nur
    # ~0,075 H breit ist; bei 0,265 ist er an der Maulmitte 0,088-0,091 H breit (Vorgabe Maulbreite 0,075-0,095 H, R10).
    auge_y_anteil=0.265,
    auge=dict(b=0.022, h=0.0105,                  # K3/R3 Vorgabe 0,020 x 0,0105 H (KI 0,019-0,022 x 0,009-0,012), B/H 1,9-2,2 (K4). Breite =
                                                  # KI h125 0,022: mit 0,020 mass die Halbwert-Maske nach dem Verkleinern auf H 569 nur
                                                  # 0,0141-0,0176 H (die Mandelspitzen verschwimmen mit dem Lid; Probe 15, R3/R4)
              dx_maul=-0.008,                     # K5: 0,005 / 0,011 H B-links der Maulmitte
              pupille=(0.65, 0.90),               # Pupille = dunkelste 25 % (K6, L*/Haut <= 0,12): Kern 0,65 x 0,90 der Augen-Halbachsen
                                                  # (Probe 1 mit 0,55 x 0,80: L* Pupille/Haut 0,135, R8 rot)
              # K11: EIN heller Kern, KI 7 px bei H 569 = 2-3 x 3-4 px (Leser A) = 0,004-0,005 x 0,005-0,007 H, Lage x 0,50-0,59 /
              # y 0,62-0,72 der Augenbox. Runde 2 (Sicht-Pruefer A2: "ein weisser Glanzpixel" statt grauem Kern; Zahlen-Pruefer:
              # y 0,73 an der Grenze 0,8): Halbachsen 0,0022 x 0,0026 H = 0,0044 x 0,0052 H (KI-Mitte), Mitte x 0,55 / y 0,64
              # Mitte im Entwurf x 0,55 / y 0,58: das Texelraster (Auge 3 Texel hoch) verschiebt den Kern; gemessen (R7, n2_probe.py O)
              # x 0,49 / y 0,68 = KI-Mitte 0,545 / 0,68 (Runde 1: 0,73, 0,07 vor der Grenze 0,8).
              glanz=(0.55, 0.58, 0.0022, 0.0026),
              lid=0.0035),                        # K8: Lidrand = 2-px-Ring bei H 569 = 0,0035 H, L* Lid/Haut 0,78-0,95, kein heller Ring
    maul=dict(dy_auge=0.052,                      # K5/R12: Maulmitte 0,052 / 0,053 H unter der Augenmitte
              b=0.092, h=0.050,                   # Messblatt 6: Breite 0,084 / 0,092 H, Hoehe 0,042 / 0,048 H inkl. Zaehne (R10 Bbox der
                                                  # dunklen Flaeche + Zaehne). Probe 1 mit 0,086 x 0,036 / Kopfbreite 0,92: gemessen
                                                  # 0,0716 x 0,0382 H (R10 rot) - die Ellipsenenden sind flacher als die Schwelle.
                                                  # Runde 2: Hoehe = Oeffnung 0,036 H + 2 x Lippe 0,007 H; mit den Stummeln misst R10
                                                  # 0,0454 H (Messblatt 0,042-0,048). Probe r2h (n2_probe.py I/N) gegen 0,056: Oeffnung
                                                  # 0,042 H, schwarze Flaeche in der Spielkamera h125 31,6 statt 29,6 x 1e-4 H^2
              lippe=0.007,                        # Lippe oben/unten: KI birkin19 Zeilen 55-58 (4 px / H 569 = 0,007 H) ueber der oberen
                                                  # Stummelreihe (ab Zeile 59), L*-Raster im Dossier SV3r2
              max_kopfbreite=0.95,                # Messblatt 6: Breite / Kopfbreite 0,80 / 0,95 (oberer Wert)
              form=3.0,                           # Umriss |x|^3 + y^2 <= 1: volle Enden bis zu den Eckzaehnen (KI birkin19 x 165-167 /
                                                  # 208-212); Probe 11 mit der Ellipse (2): Enden spitz, R10 nur 0,068-0,076 H breit
              # Runde 2 (Sicht-Pruefer B1/B2: "aufgerissenes schwarzes Maul ohne Lippen, Zackenreihe"): Innen dunkelrot statt schwarz
              # (KI-Spaltfarbe), nur ein schmaler dunkler Spalt in der Mitte (KI-Anteil am Band 0,26-0,27), Glanz auf den Lippen,
              # Zaehne als stumpfe Stummel (Quadrat) statt Spitzen.
              # Spalt: KI-Zeilenprofil h118 (n2_ki_kopf.py zeilen): L* 9-13 von 0,046 bis 0,058 H unter dem Auge gegen 15-21 darueber
              # (h125 0,048-0,060) = 0,012 H von 0,042 H Oeffnung -> halbe Spalthoehe / halbe Oeffnung 0,29
              spalt=0.29,
              zaehne=(6, 4),                      # oben / unten. KI 16 / 19 Flecken (n2_ki_kopf, L* > 40) bzw. Pruefer 11/10 + 6/8;
                                                  # Sicht-Pruefer B2: "6-8 Stummel, vereinzelt"; Riegel R14 oben >= 5, unten >= 3
              zahn_b=0.0031,                      # Stummelbreite = KI-Stummel 0,0031-0,0032 H (Wurzel der mittleren Flaeche, n2_ki_kopf)
              zahn_l=0.0053,                      # Stummellaenge = KI "Stummel 2-3 px" (Sicht-Pruefer B2, birkin19/20) = 3 px / 569 = 0,0053 H;
                                                  # Probe r2f mit 0,0031 (quadratisch): obere Stummel erreichten die dunkle Flaeche nicht
                                                  # (Messansicht, Facettenlicht), R14 oben 1
              zahn_bereich=0.84,                  # Zaehne ueber 0,84 der Maulbreite (KI birkin19: Eckzaehne x 165-167 / 208-212 von 165-212)
              # Stummel sitzen mit halber Kante auf dem Rand der Oeffnung (Innenkante der Lippe; KI birkin19: Stummelreihen direkt unter
              # der Oberlippe y 59-67 und ueber der Unterlippe y 71-78); die Haelfte auf der Lippe zaehlt R14 als Zahn am Rand.
              zahn_ueber=0.0,                     # Stummel beginnen am Rand der Oeffnung (KI: Stummel ragen aus dem Zahnfleisch in die Oeffnung);
                                                  # Probe r2g (n2_probe.py F) mit 0,15: R14 oben 5 (Grenze), mit 0,3: oben 0-2
              # Glanz auf den Lippen: KI 1,4-1,9 % des Bands. Mund ~26 x 14 Texel (0,92 der Rechteckflaeche bei Form 3) = ~330 Texel ->
              # 5 Texel = 1,5 %. Lage: Lippenmitte (x in Maulhalbachsen), Halbmesser 0,0019 H = 1 Texel (Texelabstand im Kopfbild
              # 0,0035-0,004 H); drei oben, zwei unten (KI: Glanz vor allem auf der Oberlippe, birkin19 y 55-60).
              glanz_x=((-0.55, -0.05, 0.45), (-0.30, 0.25)), glanz_r=0.0019),
    # Grundhaut vorn (Runde 2, Sicht-Pruefer A3 "helle Maske": Gesichtsfeld/Nachbarhaut in der Spielkamera Port 1,28-1,35, KI 0,87):
    # Kuppe vom Scheitel bis an die Oberlippe, darunter die rote Zone. Kuppenrampe = KI-Halo P5..P95 (Zeile 486), Lage = KI-Median der
    # hellen Gesichtssaeule (n2_ki_kopf.py fein: 43,6 / 42,1 -> (42,85 - 25,5) / 43,6 = 0,40), Rauschgewicht = KI-Streuung auf
    # Texelmassstab (L* - Gauss 1 Texel = 0,0037 H: 6,22 / 5,23) / 43,6 = 0,13 (Runde 1: 0,55 / 0,25 ohne Beleg).
    # PORT-WAHL gegen die KI-Saeule (n2_ki_kopf.py saeule: Stirn ueber -0,0195 H L* 22-31, unter +0,0155 H bis zum Maul L* 15-21):
    # Probe r2c mit beiden Zonen - die schattige obere Facette und das dunkle Band unter dem Auge zaehlte R1 als zweites und drittes
    # Auge (R1 = 2 / 3), das Band verschob die Maulmessung (R10-R12 auf das Band). Die Gesamthelligkeit traegt KOPF_V3_HELL.
    # Rote Zone (K15: Maul + Kinn, Rampe Referenz 2 Zeile 485) ab der Maul-Oberkante: Mitte der Rampe, Streuung 7,2 L* (KI unter dem
    # Auge, Texelmassstab) / Rampenspanne der Zeile-485-Rampe (in kopf_v3_bild aus der Palette gerechnet).
    zonen=dict(kuppe_t=(0.40, 0.13), rot_mitte=0.5, rot_std_L=7.2),
    # Seitenansicht (Runde 2, Sicht-Pruefer A4: "seitlich kein Auge, kein Maul, flache lachsrosa Platte"): die Seitenflaechen von
    # Mesh 9 (nicht im Kopfbild vorn) bekommen ein eigenes Seitenbild per Parallelprojektion aus der Spielkamera der Seitenpose
    # (ROOM50A0 Schnitt 12, Pose sfv2/posen_seite.json h85/h92: Clip 1 Bild 26, Gier 556; die Gegenseite gespiegelt an der Ebene
    # durch das Kopfauge), Texeldichte wie das Kopfbild vorn (R31 Mesh 9 0,072 Texel/E.), Flaeche TPage 128 u 64-111 v 156-219
    # (frei: n2_frei.py). Flaechen mit Normale . Blick <= -0,10 (wie KOPF_V2 schwelle 0,15 um 0,05 weiter, damit die Kante zum
    # Kopfbild geschlossen ist; Probe im Dossier).
    seite=dict(raum='50A0', schnitt=12, clip=1, bild=26, gier=556, massstab=0.9, breite=900, hoehe=900, cx=450, cy=450,
               u=(64, 111), v=(156, 219), dichte=0.072, schwelle=0.10,
               auge_b=0.014, auge_h=0.010,                             # KI birkin12: Seitenauge 0,014 x 0,010 H
               maul_hinter=0.101, maul_h=0.028, zaehne=5,              # KI: Maulprofil bis 0,101 H hinter der Spitze, 0,006-0,034 H, 5 Stummel
               # Schnauzenhaut KI birkin12 (n2_ki_kopf.py schnauze, x 345-368): oben y 100-118 L* P5/P50/P95 18,0/46,7/57,9, Texel-
               # Streuung 3,73, a*/b* 4,0/1,0; unten y 119-128 15,7/24,6/40,9, Texel-Streuung 4,75, a*/b* 4,5/1,9 -> eigene Rampen (Zeile
               # 486 Index 32-36 / 38-42), Mitte (46,7 - 18,0) / 39,9 = 0,72 bzw. (24,6 - 15,7) / 25,2 = 0,35, Streuung 3,73 / 39,9 = 0,09
               # bzw. 4,75 / 25,2 = 0,19
               kuppe_t=(0.72, 0.09), haut_t=(0.35, 0.19)),
    schulterauge=dict(dx=-0.117, dy=0.058,        # K12: -0,116 / -0,120 H (B-links), +0,051 / +0,058 H tiefer als das Kopfauge. dy am
                                                  # oberen Rand (h125): bei 0,055 lag die dunkle EM30-Ader der Schulter (Mesh 8, Original-
                                                  # textur) im Messfenster und war dunkler als das Auge (Probe 3, L* min 1,7)
                      # K13: Halbwert 0,016 x 0,016 / 0,018 x 0,013 H, aequiv. 0,015 / 0,0145 H. Runde 2: mit 20 x 20 Texeln misst die
                      # Halbwertmaske das Auge groesser (Probe r2d mit 0,017 x 0,014: 1,38 x Kopfauge, R9 0,8-1,3) -> x 0,87 = 0,0148 x
                      # 0,0122 H (Mitte des R9-Bereichs 1,05)
                      b=0.0148, h=0.0122,
                      seite=0.062,                # Viereck 0,062 H: Auge + Hautring (R9 misst die Haut 4-7 px um das Auge bei H 569)
                                                  # und deckt das Messfenster (+-12 px bei H 569 = +-0,021 H) ab
                      # Runde 2 (Zahlen-Pruefer 6.4: 12 x 12 Texel = 0,032 Texel/E. auf dem Viereck, Haut 0,045-0,090): 20 x 20 Texel
                      # = 0,053 Texel/E. (Mitte des Hautbereichs R31), Flaeche TPage 128 u 42-61 v 156-175 (frei, Seitenbild ab u 64)
                      u=(42, 61), v=(156, 175), abstand=60.0,
                                                  # abstand (E., vor der Schulter): Probe 1 mit 10 E. stach die gewoelbte Schulter durch
                                                  # das ebene Viereck (nur ~1/3 sichtbar)
                      clut=480, lab_auge=(12.0, 4.5, 2.4), L_verh=0.375,   # Hautzeile 480 (Mesh 8 Seite 0); K14 Lab [12,0 4,5 2,4],
                                                  # L* Auge/Haut 0,385 / 0,36
                      # Ader-Schwelle: Texel dunkler als die Haelfte des Median-Hauttons gelten als Ader (Halbwertregel des Messblatts,
                      # P_kopf.auge "dunkel = L* < (Lmin + Haut)/2" mit Lmin ~ 0); Runde 1 hatte 0,6 ohne Beleg.
                      ader=0.5),
)
# Zeile 486 (Kopf V3): feste Eintraege; KI-Farben (Messblatt bzw. n2_ki_kopf) mit KOPF_V3_HELL ins Port-Licht, im TIM 15 Bit.
KOPF_V3_INDEX = dict(pupille=1, auge=2, lid=3, glanz=4, maul=5, maul_tief=6, zahn=7, schulterauge=8, lippe_glanz=9, lippe=27,
                     kuppe=(10, 14), haut=(16, 20), rot=(22, 26), seitenauge=28, seitenmaul=29, seitenzahn=30,
                     seite_oben=(32, 36), seite_unten=(38, 42))
KOPF_V3_ZEILE = dict(tabelle=[
    (1, _ki(2.6, 4.7, -0.6)),      # K6 Pupille h118 Lab [2,6 4,7 -0,6]
    (2, _ki(22.5, 6.5, -1.3)),     # K7 Auge gesamt h118 Lab [22,5 6,5 -1,3]
    (3, _ki(40.4, 7.1, -0.7)),     # K8 Lidrand h118 Lab [40,4 7,1 -0,7]
    (4, _ki(65.6, 3.8, -1.8)),     # K11 heller Kern: Mittel h118/h125 Lab [71,1 5,7 -2,2] / [60,1 1,8 -1,3] (grau, a* <= 7)
    (5, _lab_rgb(13.5, 13.7, 4.8)),  # Maul innen = Messblatt 6 innen Lab [13,5 13,7 4,8] absolut wie Runde 1 (R13 misst L* innen / Haut am
                                     # Auge = 0,20-0,40 und a* innen - a* Haut >= +4; Probe r2d mit x KOPF_V3_HELL: a* +2,6, R13 rot)
    (27, _lab_rgb(19.6, 17.1, 5.9)),  # Lippe = KI n2_ki_kopf lab_lippe h118/h125 Lab [20,1 17,7 5,9] / [19,2 16,5 5,9], absolut wie das
                                      # Maulinnere (Probe r2e mit x KOPF_V3_HELL: Lippe unter der R10-Schwelle, obere Stummel als Loch in der
                                      # dunklen Flaeche, R14 oben 2)
    (6, (31, 13, 13)),             # Spaltkern: KI Maul dunkelste 10 % (Dossier SV2, Kopf V2), absolut (schon dunkelster KI-Wert)
    (7, _lab_rgb(45.0, 17.0, 4.65)),  # Zaehne: Messblatt 6 Lab [50,0 17,0 4,4] / [49,4 17,0 4,9] (a*/b*), L* 45 = KI x 0,9. PORT-WAHL zwischen
                                      # KI x KOPF_V3_HELL (n2_probe.py B: obere Stummel auf der schattigen Facette unter der R14-Schwelle
                                      # Median + 15, oben 2) und KI absolut (n2_probe.py I: L* Zahn 43,4 an der Grenze Haut + 20 = 44,3)
    (8, (39, 29, 28)),             # K14 Schulterauge Lab [12,0 4,5 2,4] (Runde 1; das Viereck nutzt Zeile 480)
    (9, (198, 143, 146)),          # Lippenglanz KI h118/h125 RGB (197,139,142) / (199,147,150) absolut: Glanzlicht = Spiegelung der
                                   # Lichtquelle, nicht Hautton (mit KOPF_V3_HELL in der Spielkamera max(RGB) < 170, Probe r2d: 0 Glanzpunkte)
    # Halo/Kuppe (K9): KI P5..P95 der Halo-Ringe L* 25,5..69,1 (Mittel h118/h125), a* 7,0, b* 0,6 (Ringe 0,010-0,015 H)
    ] + [(10 + i, c) for i, c in enumerate(_rampe_lab(25.5, 69.1, 7.0, 0.6))] + [
    # Kopfhaut aussen: KI P5..P95 der Ringe 0,025-0,05 H L* 10,15..45,2, a* 7,0, b* 1,4 (Ringe 0,025-0,05 H)
    ] + [(16 + i, c) for i, c in enumerate(_rampe_lab(10.15, 45.2, 7.0, 1.4))] + [
    # rote Zone (K15, Original): Farben der Referenz-2-Kopfzeile 485 (dunkel (114,57,55)) x 0,90 / 1,0 / 1,1 / 1,2 / 1,3 (Runde 1)
    (22, (103, 51, 50)), (23, (114, 57, 55)), (24, (125, 63, 61)), (25, (137, 68, 66)), (26, (148, 74, 72)),
    (28, _ki(7.0, 1.5, -0.2, KOPF_V3_HELL_SEITE)),     # Seitenauge KI birkin12 Lab [7,0 1,5 -0,2]
    (29, _ki(27.3, 22.8, 8.4, KOPF_V3_HELL_SEITE)),    # Maulprofil KI birkin12 Lab [27,3 22,8 8,4]
    (30, _ki(56.1, 20.1, 8.4, KOPF_V3_HELL_SEITE)),    # Stummel seitlich KI birkin12 Lab [56,1 20,1 8,4]
    # Schnauzenhaut seitlich (KI birkin12, KOPF_V3 seite): oben L* 18,0..57,9 a* 4,0 b* 1,0; unten L* 15,7..40,9 a* 4,5 b* 1,9
    ] + [(32 + i, _ki(18.0 + 39.9 * i / 4.0, 4.0, 1.0, KOPF_V3_HELL_SEITE)) for i in range(5)] + [
    ] + [(38 + i, _ki(15.7 + 25.2 * i / 4.0, 4.5, 1.9, KOPF_V3_HELL_SEITE)) for i in range(5)] + [
])


def _variante_v3(schulterauge=None, seite_uv=None):
    f = dict(FORMEN['hangar'])
    f['schwanz_kette'] = dict(f['schwanz_kette'], **SCHWANZ_V3)
    # kopf_v2_uv: UVs Mesh 9 (Kopfbild vorn, Runde 2 auch Seitenbild) + Viereck auf Mesh 8
    f['kopf_v2'] = dict(KOPF_V3, schulterauge=schulterauge, seite_uv=seite_uv)
    f['kopf_v3'] = KOPF_V3
    f['cluts'] = dict(f['cluts'], zusatz=list(f['cluts']['zusatz']) + [KOPF_V3_ZEILE])
    return f


# ═══════════════════════════════════════════════════ Runde 41 SF "V4" (Messblatt V4, Nutzer-Beschreibung 2026-11-06 ~22:00)
# Nutzer: "Da du offenbar nicht in der Lage bist das beim Original Material vom 2 Beiner Birkin richtig zu erkennen, habe ich dir das
# mit Beschreibung1 und Beschreibung2 und Beschreibung1_Vorlage und Beschreibung2_Vorlage auch noch mal im Detail beschrieben. Ich
# will EXAKT diese Umsetzung und auch EXAKT seine Farben."  Vorlage = die Original-Videobilder n1485 (vorn) / n1435 (seitlich) mit der
# Beschriftung des Nutzers; Masszahlen aus analysis/befunde_runde41/SF_MESSBLATT_V4.md (Zeilen/Riegel in den Kommentaren). KEIN
# RE-Befund (die Sichel-Form gibt es auf der Disc nicht), keine @0x-Konstante. V4 = Koerper Referenz 2 (FORMEN['hangar'], Meshes 0-8 und
# 10-15 byte-gleich, Riegel G1) + Kopf V4 (Mesh 9: Form x skal, UVs nach Ansicht, Bild in TPage 128 v 160-223, CLUT-Zeile 486) +
# Schwanz V4 (Knochen 16-22: Gelenke im Bild der Vorlage-2-Kamera, Taille am Knick, Textur TPage 130 u 64-127 v 0-63, Zeile 487) +
# Farben (Texel/CLUT aus der Eichung tools/r41_sichel/v4/eichen_v4.py, Datei V4_FARBEN). Erzeugt nur mit --variante v4 --aus DIR.
V4_DIR = os.path.join(HIER, 'r41_sichel', 'v4')
V4_FARBEN = os.environ.get('RE15_V4_FARBEN', os.path.join(V4_DIR, 'farben_v4.npz'))   # Eichung (Messblatt V4 Abschnitt 5), committet
# V4 Runde 2: Koerpertexel und -zeilen 480/481/482/484 aus tools/r41_sichel/v4/r2_koerper_farben.py (Sicht-Pruefung V4 r1 1.4), committet
V4_KOERPER = os.environ.get('RE15_V4_KOERPER', os.path.join(V4_DIR, 'farben_koerper_v4.npz'))
# V4 Runde 5: Schwanztexel (Indizes 256 x 64, uv art 'achse') und Zeile 487 aus tools/r41_sichel/v4/r5_schwanz_textur.py (Projektion
# von Vorlage 2), committet; ersetzt die Schwanzbaender aus V4_FARBEN
V4_SCHWANZ = os.environ.get('RE15_V4_SCHWANZ', os.path.join(V4_DIR, 'farben_schwanz_v4.npz'))
# V4 Originaltexturen (Vorgabe des Orchestrators 2026-11-07 nach dem Nutzer: "Bei der Sichel Form V4, warum sind denn da die Farben so
# extrem scheisse? ... die Form baut von Farben und Texturen her ja nun wirklich auf die Urspruengliche Birkin Form auf"): ersetzt Messblatt
# V4 Abschnitt 5 (Farbeichung an den Videobildern, V4 Runde 1-5). RE15_V4_TEXTUR=ref2 (Standard): Koerper = Referenz 2 unveraendert
# (keine eigenen Koerperzeilen 488-491, keine Koerpertexel aus farben_koerper_v4.npz, keine geeichten Hautzeilen, keine Farbklammer), die CLUT
# bleibt bytegleich Referenz 2; Kopf (Mesh 9) und Schwanz (Meshes 16-22) zeigen auf die Referenz-2-Hautzeile V4_REF2_ZEILE und tragen Texel
# aus dem Referenz-2-Atlas (tools/r41_sichel/v4/ref2tex_v4.py, Datei V4_REF2TEX). RE15_V4_TEXTUR=eichung baut V4 Runde 5 (a762bd66) nach.
V4_TEXTUR = os.environ.get('RE15_V4_TEXTUR', 'ref2')
V4_REF2TEX = os.environ.get('RE15_V4_REF2TEX', os.path.join(V4_DIR, 'farben_ref2tex_v4.npz'))
# Zeile 480 = CLUT der Oberarm-/Rueckenhaut von Referenz 2 (Mesh 13 Oberarm (W)-rechts nur Zeile 480, Mesh 8 Rumpf Zeilen 480/481;
# Zaehlung ref2tex_v4.py atlas(), Protokoll SF_belege/v4/ref2tex_protokoll.json)
V4_REF2_ZEILE = 480
# Posen der beiden Vorlage-Kameras (Spielkamera 50A0 Schnitt 12): tools/r41_sichel/v4/posen_v4.json
V4_POSE_VORN = dict(clip=0, bild=9, gier=1737, x=-9279, z=-22511)    # "135": Silhouette n1485, IoU 0,619 (posen_v4.py)
V4_POSE_SEITE = dict(clip=1, bild=26, gier=556, x=-8070, z=-21202)   # "85" = sfv2/posen_seite.json "85" (Messblatt V4 Abschnitt 7)
SCHWANZ_V4 = dict(
    pose=V4_POSE_SEITE,                    # Ruhe-Pose = Vorlage 2 (Messblatt V4 Abschnitt 4 "Darstellung" Punkt 1)
    # T1 (Abschnitt 4 Zeile "Beginn"): Beginn = Unterseite des Ansatzes 0,15 H hinter dem Buckelscheitel, 0,767 H ueber der Sohle.
    # V4 Runde 2 (Pruefung V4 r1 Zahlen 4.1: Schwanz 11-12 px hinter den Nutzerpunkten, T1 0,218 / 0,727 H): Beginn wie gemessen; der
    # Koerper Referenz 2 (G1) reicht in Pose "85" auf dieser Hoehe 0,06 H weiter nach hinten als das Video (Rueckenkontur 0,21 H hinter
    # dem Scheitel), deshalb liegt der Ansatz (Gelenke 0-3, 'tiefe') zur Kamera hin VOR der Flanke und tritt dort aus dem Ruecken.
    beginn=(0.15, 0.767),
    # Mittellinie: Gelenke 16..22 und Ende des Glieds 22 (vor dem Dorn) im Bild der Vorlage-2-Kamera, (dx, dy) in H ab dem Beginn,
    # dx < 0 = nach hinten (Bild-links), dy > 0 = nach unten. Quelle Abschnitt 4: Achse "fast waagerecht nach hinten" (x 356 -> 298,
    # y 231 -> 247: am Beginn 0,047 H ueber ihm); Biegung bei dx -0,245 / dy +0,036 (Achse 70-90 Grad nach unten auf <= 0,10 H);
    # "Biegung -> Knick nahezu senkrecht"; Knick dx -0,265 / dy +0,257; Ende dx -0,245 / dy +0,397 (Endstueck 82-84 Grad, 6-8 Grad
    # wieder nach vorn). Gliederteilung nach Abschnitt 4 Punkt 2: Glied 16 vom Buckel bis zum Beginn (Gelenk 1 = Ende von Id 16, Riegel
    # T1 misst Id 16), 17-18 waagerechter Ansatz bis zur Biegung (Gelenk 3), 19-20 senkrecht bis zum Knick (Gelenk 5), 21-22 Endstueck.
    # Nachgestellt (tools/r41_sichel/v4/r2_form_suche.py, 5 Runden): Ziel je Punkt = Mitte aus Messblatt-Mass (relativ, Port-H 264) und
    # Nutzer-Linienende (absolut, Riegel TA), weil Port- und Video-Figur verschieden gross sind (H 264 / 253, relativ und absolut bis
    # 0,044 H auseinander); danach die Biegung von Hand geschaerft (Gelenk 2 flacher, Gelenk 3 nach hinten oben, Gelenk 4 senkrecht
    # darunter: Drehung nach dem Pruefer-Mass 59,9 -> 67,3 Grad, Original 66-70) und der Knick so gesetzt, dass das Endstueck 0,15 H
    # lang bleibt (r2_expt.py Laeufe b-e, Dossier V4 Runde 2).
    # V4 Runde 3 (Pruefung V4 r2 Zahlen 4.1, Entscheid O2: T2-Drehung nach EINEM Verfahren, t2_mass_v4.py = Pruefer-Mittellinie, Original
    # 65,8 / 69,5 / 70,1 Grad, Ziel 68,4 +-5; Runde 2 hatte damit 52,5 Grad, Biegung auf zwei Stufen 0,30-0,35 / 0,40-0,45 H): Gelenke 2-4,
    # Durchmesser 2-4 und Tiefe 2-3 nachgestellt mit tools/r41_sichel/v4/r3_form_suche.py (Saat 2, Schritt 78; T1-T6/TA mit Marge
    # 0,003 H, Protokoll SF_belege/v4/r3/r3_form_suche_saat2.log): 70,6 Grad. Der Ansatz faellt jetzt wie im Original schraeg zur
    # Biegung ab (Gelenk 2 0,016 H ueber dem Beginn, Gelenk 3 0,058 H darunter; Original-Mittellinie 20-27 Grad vor der Biegung).
    # V4 Runde 4 (Pruefung V4 r3 Zahlen 4.1/4.2, Sicht 1.3): Lage der Biegung nach dem O2-Werkzeug +0,100 H (Soll +0,036 +-0,03), Breite
    # nicht monoton (Gelenk 3 0,1599 > Gelenk 2), Aussenecke als Dorn, Oberkante x 276-292 6-16 px ueber dem Original. Gelenke 2-4,
    # Durchmesser 1-4 (monoton fallend erzwungen, Messblatt Abschnitt 4 Punkt 3) und Tiefe 2-3 per tools/r41_sichel/v4/r4_form_suche.py
    # (Saat 1, Schritt 253; Nebenbedingungen T1-T6/TA, T2 Lage+Drehung nach t2_mass_v4, T5 auch quer zur Mittellinie, Oberkante;
    # Ziel = Ober-/Unterkontur des waagerechten Teils wie die Leser-Maske; Protokoll SF_belege/v4/r4/r4_form_suche_saat1.log).
    # Gelenke 5-7 (Knick, Endstueck) danach mit RE15_R4_ENDE=1 r4_form_suche.py (Saat 22, Schritt 35: T4 dx -0,214 -> -0,215, Strafe 2,36 ->
    # 2,00; Protokoll SF_belege/v4/r4/r4_form_suche_ende22.log).
    # V4 Runde 5 (Pruefung V4 r4 Zahlen 4.1: T3 dx -0,234, Grenze -0,235; Entscheid O11): Knick und Endstueck (Gelenke 5-7) 0,0075 / 0,015 H
    # nach hinten, Knick 0,006 / Endstueck 0,002 H tiefer (Laeufe r5 Form, formprobe: T3 -0,240 / +0,253, T4 -0,217 / +0,390). Gelenk 3
    # (Biegung) aus tools/r41_sichel/v4/r5_form_suche.py Lauf "bauch" (RE15_R5_BAUCH=1, Saat 5, Schritt 46; Nebenbedingungen wie r4_form_suche
    # und TK, Protokoll SF_belege/v4/r5/r5_form_suche_bauch5.log).
    gelenke=[(0.12, -0.080), (-0.037, -0.040), (-0.1462, -0.0115), (-0.3003, 0.0743), (-0.2456, 0.1649), (-0.268, 0.2852),
             (-0.2543, 0.3405), (-0.2313, 0.3924)],
    # Durchmesser je Gelenk in H (Abschnitt 4 "breiter nach oben": vom Knick zum Ruecken f 0,1/0,3/0,5/0,7 = 0,03/0,065/0,125/0,14 H,
    # f 0,9 geht in den Buckel >= 0,09; Taille am Knick 0,024-0,032 H; unter dem Knick gleich breit 0,035 H). Gelenke 2-4 nachgestellt
    # (r2_form_suche.py nach f 0,7 / 0,5 / 0,3); Gelenk 1 0,13 H: Unterseite am Beginn (Achse 0,04 H darueber); Gelenk 0 im Buckel wie
    # Runde 1. Knick (Gelenk 5) 0,027 H: Mitte der gemessenen Taille 0,024-0,032 H (Runde 1: 0,022 lag darunter, Pruefung V4 r1 Zahlen 5);
    # gemessen 0,025-0,028 H und die schmalste Stelle (T6). Endstueck 0,038 H (gemessen 0,032-0,036 H, Ziel 0,035).
    # V4 Runde 3: Gelenke 2-4 aus r3_form_suche.py (Saat 2, Schritt 78, s. gelenke); T5 0,028 / 0,080 / 0,140 / 0,131 H
    # V4 Runde 4: Gelenke 1-4 aus r4_form_suche.py (Saat 1, Schritt 253), monoton fallend 0,139 -> 0,130 -> 0,079 -> 0,046 H
    # V4 Runde 5: Gelenk 3 0,0636 H (r5_form_suche Lauf "bauch", Saat 5, Schritt 46) - die Breite an der Biegung traegt jetzt der
    # Zwischenring 'bauch' (s. unten)
    durchmesser=[0.16, 0.1391, 0.1301, 0.0636, 0.0459, 0.027, 0.038, 0.038],
    # Versatz zur Kamera (H, Bildmassstab am Gelenk) - Ansatz vor der Flanke (s. beginn): Strahl durch (357,243) trifft Mesh 8 bei
    # Modell-z -475 (Mittelebene -134, kopf3d_v4.treffer); mit 0,05 H blieb der Ansatz hinter der Flanke (Malreihenfolge nach mittlerer
    # Tiefe wie die PSX-Ordnungstabelle, r2_expt Lauf a1), mit 0,12 H liegt er sichtbar davor (Lauf a2). Ab der Biegung 0 (Mittelebene).
    # PORT-WAHL (Pruefung V4 r2 Zahlen 4.4/5: als solche kennzeichnen): Gelenke 0/1 0,12 H aus r2_expt Lauf a1/a2 (Sichtbarkeit nach der
    # Malreihenfolge); Gelenke 2/3 V4 Runde 4 aus r4_form_suche.py (Saat 1, Schritt 253) 0,1463 / 0,0696 H. Nutzer-Wortlaut "Ich will
    # EXAKT diese Umsetzung" (Beschreibung2: Schwanz Beginn in den Ruecken ueberfuehrend).
    tiefe=[0.12, 0.12, 0.1463, 0.0696, 0.0, 0.0, 0.0, 0.0],
    dorn=0.8,                              # Spitze 0,8 x Endradius (Spitzenform nicht aufloesbar, Abschnitt 9 Punkt 2; PORT-WAHL kurz)
    # V4 Runde 4 (Sicht-Pruefung V4 r3 1.3 "Vierkant-Balken", Empfehlung "runder Querschnitt (mehr Seiten, wie Referenz 2)"): 12 Seiten.
    # M1: 7 x 24 + Deckel 12 + Spitze 12 = 192 Dreiecke <= 212 (EM30-Arm 13-15); 13 Seiten ergaeben 208, 14 Seiten 224 > 212.
    # V4 Runde 5 (Entscheid O8 "die Ober- und Unterkontur des waagerechten Teils darf nicht gerade sein (TK)", Messblatt Abschnitt 4 "von da
    # an breiter nach oben"): Zwischenring im Glied 2 (Gelenk 2 -> Biegung) bei 0,15 der Gliedlaenge, Radius 1,374 x Gelenkradius, Versatz
    # 0,011 x Radius nach aussen (kette_meshes_v4 'bauch'; r5_form_suche Lauf "bauch", Saat 5, Schritt 46: TK RMS 3,69 px (Original
    # 3,87-4,71), T5q f 0,5 0,110 H, T2 64,0 Grad / dx -0,259 dy +0,049). Dafuer 11 statt 12 Seiten - M1: 7 x 22 + Deckel 11 + Spitze 11
    # + Zwischenring 22 = 198 <= 212 (mit 12 Seiten 216).
    bauch=dict(glied=2, t=0.15, radius=1.374, aussen=0.011),
    glieder=7, seiten=11,
    ueberlapp=0.45, deckel=True,           # wie V3 (Manschette ins Vorglied, Deckel in den Rumpf)
    # Textur: eigene Flaeche TPage 130 u 64-127 v 0-255 (frei, Atlasbelegung Referenz 2: Seite 2 belegt u 0-47 und u 0-63 ab v 176),
    # Zeile 487; je Flaeche eine Farbklasse (Abschnitt 4 "zweitoenig"): Wurzel (Glied 16; Glied 17 ohne die Aussenkante), Aussenkante
    # des Bogens (Normale weg von der Bogenmitte, c > 0,5), Mitte (|c| <= 0,5: die zur Kamera gewandte Seite um die Achse), Unterseite
    # (c < -0,5), Endstueck unter dem Knick (Glieder 21-22); die Mitte getrennt fuer den waagerechten Ansatz mit Biegung (Glieder
    # 17-18, 'mitte') und das senkrechte Stueck bis zum Knick (Glieder 19-20, 'senkrecht'), weil das Original dort grauer und dunkler
    # ist (Achsenpunkte (310,243)/(292,262): a* 8,8 / 6,6; (289,280)/(288,300): a* 5,0 / 1,7); Baender: Wurzel 56 Zeilen (Glied 16 ist das
    # dickste, M4 >= 0,045 Texel/E.), die uebrigen je 40. V4 Runde 2 (Sicht-Pruefung V4 r1 1.3:
    # die Klasse "aussen" deckte die ganze breite Flaeche, im Original ist das helle Grau nur die Aussenkante, die Achse ist dunkel
    # mauve, L* 11,6 / a* 6,1 an 7 Achsenpunkten): neue Klasse "mitte". c = Normale . Richtung Bogenmitte -> Gliedmitte; die Grenze 0,5
    # = cos 60 Grad teilt die 10 Seiten in 4 / 2 / 4 (Aussenkante / Mitte / Unterseite), im Bild je etwa ein Drittel der Breite.
    # dichte = Texel je E. (M4: 0,045-0,091, EM30-Haut 0,046-0,087 -> Mitte 0,068): je Glied u-Spanne = Laenge x dichte, v-Spanne =
    # halber Umfang x dichte (Umfang gespiegelt wie V3), hoechstens 63 bzw. die Hoehe des Bands der Klasse.
    # V4 Runde 4 (Pruefung V4 r3 Sicht 1.3, Klasse A: "Vierkant-Balken" - Deckflaeche 38-42 L* und Seitenflaeche 11 L* als Plateaus mit
    # harter Kante, Querprofil-Stufe auf 3 px 16,6-19,9 L*, Original 5,4-9,6): 'verlauf' = Zeile je Ecke aus der Lage um den Umfang
    # (c), die Baender aussen/mitte/senkrecht/unten tragen einen Verlauf ueber c (eichen_v4.py, Schwanz-Verlauf); Glied 17 ohne die
    # Klasse 'wurzel' (sonst stiesse das Wurzelband ohne Verlauf an die Aussenkante). zeilen_je_c = Texelzeilen je Einheit c, je Band so
    # gewaehlt, dass jedes Glied in der M4-Spanne 0,045-0,091 Texel/E. bleibt (Dichte = Wurzel aus u-Dichte 0,068 x v-Dichte; die
    # duennen Glieder 19-20 im Band 'senkrecht' brauchen weniger Zeilen): Lauf r4 Dichte 1 mit 20/22/15/20 -> 0,045-0,086 (Glied 18 an der
    # Grenze), Lauf r4 Dichte 2 mit 22/24/14/22 -> 0,046-0,086. PORT-WAHL (Machart, Messblatt M4).
    # Grenze 0,5 wie Runde 3 (Klassen und Farben der Runde 3, T8/T9 bestanden); der Verlauf liegt nur an den Klassengrenzen
    # (eichen_v4.UEBERGANG). Laeufe r4 Eichung h-p mit 0,8 und durchgehendem Verlauf: nicht stabil an T8/T9 zu eichen (Dossier V4 Runde 4).
    # Zeilen je c fuer M4 (Lauf r4 Dichte 4: senkrecht 10 -> Glied 20 0,092 > 0,091, daher 9). PORT-WAHL (Machart, Messblatt M4),
    # Nutzer-Wortlaut "Ich will EXAKT diese Umsetzung und auch EXAKT seine Farben"; seit V4 Runde 5 (uv art 'achse') ohne Wirkung - nur noch fuer den Nachbau der Staende Runde 2-4.
    # V4 Runde 5 (Entscheid O8 "durchgehende Texturierung entlang der Achse, UV ueber die Glieder fortlaufend"): art 'achse' -
    # v 0-255 = Bogenlaenge (kette_meshes_v4 tx), u = Lage um den Umfang; die Texel malt tools/r41_sichel/v4/r5_schwanz_textur.py aus
    # Vorlage 2 (Datei V4_SCHWANZ). Die Klassen-/Band-Angaben (grenze, verlauf, baender) gelten nur ohne 'art' (Runde 2-4,
    # eichen_v4.py); sie bleiben stehen, damit aeltere Staende nachgebaut werden koennen. dichte 0,068 (u, wie Runde 2-4) und
    # v-Dichte 255 / 3593 E. = 0,071 ergeben je Glied sqrt(0,068 x 0,071) ~ 0,07 Texel/E. (Messblatt M4 0,045-0,091).
    uv=dict(art='achse', v=(0, 255), clut=487 << 6, page=130, u=(64, 127), dichte=0.068, grenze=0.5,
            verlauf=dict(zeilen_je_c=dict(aussen=22, mitte=24, senkrecht=9, unten=22)),
            baender=dict(wurzel=(0, 55), aussen=(56, 95), mitte=(96, 135), senkrecht=(136, 175), unten=(176, 215), ende=(216, 255))),
    # Nachschwingen wie V3 (Ae5), Knick-Grenze mit Ausnahme fuer die Biegung (ab 30 Grad Ruhewinkel). Dieselbe Regel im Spiel:
    # schwanz_r41.c glaetten() mit SW_GLATT_KNICK_AB, dort seit V4 Runde 2 fuer die V4-Kette (ungleiche Glieder, SW_GLATT_V4_VERH)
    # wirksam (Runde 1 lief glaetten() fuer V4 nicht, Pruefung V4 r1 Zahlen 3). PORT-WAHL wie V3.
    bewegung=dict(steif=(0.22, 0.08), traeg=(0.55, 0.92), daempfung=0.80, schwere=0.0, grenze=55,
                  knick=dict(rand=3.0, min=12.0, max=16.0, ab=30.0)),
)
KOPF_V4 = dict(
    # Form (Mesh 9 frei, Riegel G1): K10 Kopfbox vorn 0,159 x 0,172 H (0,14-0,18 x 0,15-0,20); Referenz 2 in Pose "135": 0,126 x 0,170 H.
    # Mesh-lokal x = vorn (Tiefe), y = Hoehe (y < 0 oben), z = Breite (z > 0 = (W)-links; Mittelebene z -29 = Nasenspitze (482,-126,-29),
    # die Ecken liegen paarweise symmetrisch dazu). V4 Runde 2 (r2_kopf_expt.py, Laeufe k6/k9/k10):
    #   Breite x1,55: K10-Breite 0,154 H (Runde 1 x1,4: 0,143 an der Untergrenze, Pruefung V4 r1 Zahlen K10/4.3: das Auge sass fast auf
    #   der Kontur). Tiefe x1,3: groesste Tiefe, bei der die K10-Hoehe <= 0,20 H bleibt (der Kopf zeigt in Pose "135" nach unten links,
    #   Tiefe wird dort Bildhoehe: x1,3 0,187 H, x1,5 0,203, x1,7 0,214); die Seitenbox des Originals (0,221 H2, Messblatt-Kopfbox
    #   seitlich x 412-467) ist damit nicht erreichbar (Port 0,170 H, Offen). Hoehe x0,9 (Runde 1, K10-Hoehe).
    skal=(1.3, 0.9, 1.55),
    # Auge (Kopfraum nach skal): der Punkt der (W)-rechten Gesichtsflaeche, den der Sehstrahl der Vorderkamera an der K4-Stelle trifft
    # (augenpunkt_v4.suche / r2_auge_suche.py: K4 0,049 H (B)-links, 0,107 H unter OK; in beiden Vorlage-Kameras sichtbar, Abstand
    # Treffer-Punkt 1,1 / 2,0 E.). Bezug fuer den Haarvorhang (vorbau) und die Augenzone vorn (eichen_v4.py).
    auge=(360.0, -306.0, -71.0),
    # Haarvorhang (Messblatt V4 Abschnitt 3: "die Haare haengen ueber die (W)-linke Gesichtshaelfte und bilden von der Seite die dunkle
    # Vorderkante", Auge 0,049 H HINTER der Vorderkante; Sicht-Pruefung V4 r1 1.2: Runde 1 bildete einen spitzen Keil, Spaltenhoehe an
    # der Spitze 18-21 px, gefordert eine fast senkrechte Vorderseite >= 0,12 H). Die Ecken der (W)-linken Gesichtshaelfte werden in
    # Richtung "Bild-x der Vorlage-2-Kamera" (n, Kopfraum der Pose "85") bis zur Ebene n . q = n . auge + vor x H / Bildmassstab
    # geschoben - im Bild von Vorlage 2 eine senkrechte Vorderkante. vor 0,06 H: S2-Mitte 0,049 H plus die Rundung der Kante (Lauf k5:
    # vor 0,049 -> Augenpunkt 0,039 H hinter der Box-Vorderkante, vor 0,06 -> 0,050). Gewicht = glatt in z (z0 -10 .. z1 30: erst ab der
    # ersten Eckenspalte (W)-links der Mittelebene (Ecken z 5/9/23); mit z0 -60 .. z1 -29 verdeckte der Vorhang das Auge in Vorlage 1,
    # Treffer 218 E. davor, Lauf "n"), glatt in x (xa 100 .. xb 250: Eckenreihen x 104 / 254 = Uebergang Hinterkopf -> Gesicht) und
    # in y bis zum Kinn (y1 40 .. y2 170: Eckenreihen y 77-81 / 168). Ergebnis (Lauf k7): Spaltenhoehen an der Vorderkante 31-36 px =
    # 0,12-0,14 H (Runde 1: 18-21 px), K10 unveraendert 0,154 x 0,187 H. PORT-WAHL (Eckenreihen), Quelle der Masse: Messblatt S2/S3.
    # V4 Runde 4 (Pruefung V4 r3 Zahlen S3 ROT: Abstand Haarkante -> Augenfleck 0,015-0,019 H, erlaubt 0,01 H): vor 0,06 -> 0,053 H (erster
    # Versuch, Lauf r4 Eichung f), Endstand 0,048 H (s. unten; V4 Runde 5, Pruefung V4 r4 Zahlen 5: der Kommentar nannte nur 0,053). Das
    # Fenster KS2 (Messblatt S3) liegt bei 0,84-0,96 der Kopfbox-Breite; der Augenfleck endet an der Naht zu den Vorderbild-Flaechen
    # (Runde 3: Vorderkante der Ellipse vorgezogen ohne Wirkung). Mit der Vorderkante des Vorhangs 0,007 H weiter hinten rueckt das
    # Fenster um 0,84 x 0,007 H an den Fleck: Lauf r4 Eichung f S3 Abstand 0,004 H, S2 0,054 H (Soll 0,03-0,07, Messblatt 0,049). Mit dem
    # Endstand der Kopfstruktur schwankte S3 bei 0,053 / 0,050 zwischen 0,004 und 0,011 H (Laeufe r4 Eichung h-o: ein Pixel Fleckrand);
    # 0,048 H: S3 0,004 H, S2 0,054 H, K10 unveraendert 0,154 x 0,187 H (Lauf r4 Eichung n). PORT-WAHL, Quelle der Masse Messblatt S2/S3,
    # Nutzer-Wortlaut "Ich will EXAKT diese Umsetzung und auch EXAKT seine Farben" (Beschreibung2: "Haare von der Seite" unmittelbar vor dem "Auge von der Seite").
    vorbau=dict(art='bild', vor=0.048, z0=-10.0, z1=30.0, xa=100.0, xb=250.0, y1=40.0, y2=170.0),
    clut=486, page=128,
    # Ansichten: Flaechen von Mesh 9 mit Blick zur Kamera (Normale . Blick >= schwelle) bekommen das Bild der Ansicht, in der sie
    # frontaler liegen (Parallelprojektion aus der Spielkamera): vorn = Pose "135" (Vorlage 1), seitlich = Pose "85" (Vorlage 2).
    # schwelle 0,05 = cos 87 Grad: noch streifender gesehene Flaechen gelten als abgewandt (sie sind im Bild unter 1 px breit); PORT-WAHL.
    # Runde 2: keine Haarstraehne mehr (Sicht-Pruefung V4 r1 1.2: das Viereck ragte als schwarzer 14 x 23 px Lappen vor den Kopf; die
    # Vorderkante bildet jetzt der Haarvorhang).
    straehne=None,
    vorn=dict(pose=V4_POSE_VORN, u=(0, 47), v=(160, 223)),
    seite=dict(pose=V4_POSE_SEITE, u=(56, 111), v=(160, 223)),     # TPage 128 v 160-255 frei (Atlasbelegung Referenz 2)
    schwelle=0.05,
    # (Eichlauf 10 mit Vorrang fuer das Vorderbild ab Normale . Blick >= 0,35: die streifend gesehenen Seitenflaechen teilten sich dann
    # Texel mit den Vorderflaechen, das Auge zerfiel (K1-K4 rot) - verworfen, keine Vorrangregel)
)


# V4 Runde 3 (Entscheid O1 des Orchestrators: Riegel G1 meint die Geometrie - Ecken, Normalen, Dreiecke, Knochen, Clips; die
# Texel-Zuordnung (UV/CLUT) der Koerper-Meshes darf geaendert werden, wenn eine Region sonst nicht in ihrer Farbe zu faerben ist):
# eigene CLUT-Zeilen fuer zwei Koerper-Meshes, deren Texel ein anderes Mesh mit anderer Zielfarbe teilt (r2_uv_belegung.py):
#   Mesh 10 = Oberarm (W)-links = (B)-rechts: teilt 5798 Texel mit Mesh 13 (Oberarm (W)-rechts) in Zeile 480; Messblatt V4 F8 16,4 L*
#            gegen F7 33,3 L* (Pruefung V4 r2 Zahlen 4.2) -> Zeile 489 (Startwert = Zeile 480).
#   Mesh 14 = Unterarm (W)-rechts = (B)-links: teilt 1832 Texel mit Mesh 11 (Unterarm (W)-links) in Zeile 482; Original-Box
#            298-325 x 262-340 8,8 / -3,4 / 3,2 L*, kein Pixel L* > 30, gegen Unterarm (B)-rechts 29,3 L* (Pruefung V4 r2 Sicht 1.4,
#            Klasse A) -> Zeile 488 (Startwert = Zeile 482).
# Die Zeilen 488/489 liegen hinter Kopf 486 / Schwanz 487 (dieselbe CLUT-Spalte; NB3 nutzte 488/489 schon fuer Klaue/Kappe).
#   Mesh 13 = Oberarm (W)-rechts = (B)-links (V4 Runde 4, Pruefung V4 r3 Sicht 1.5 Klasse A "Rot-Verteilung"): teilt 5074 Texel mit
#            Mesh 8 (Rumpf, rote Brustmitte) und 4023 mit Mesh 5 (Bein) in Zeile 480 - das Kachelziel gab Id 13 im aeusseren Drittel
#            Rotanteil 0,12, der Fit lieferte 0,36 (Lauf r4 Koerper 2, der geteilte Texel mittelt mit der Brust) -> Zeile 490 (Start 480).
#   Meshes 2/3/5/6 = Beine (V4 Runde 4, Riegel FS Unterschenkel): teilen in Zeile 480 2507 Texel (Mesh 5) mit dem Rumpf; mit der roten
#            Brustmitte (Kacheln) wurden die Unterschenkel rot (Rotanteil 0,30 statt 0,03, dE00 7,1, Lauf r4 Koerper 6) -> Zeile 491.
KOERPER_ZEILEN_V4 = {10: (489, 480), 14: (488, 482), 13: (490, 480),
                     2: (491, 480), 3: (491, 480), 5: (491, 480), 6: (491, 480)}       # Mesh -> (neue Zeile, Startzeile)


def _variante_v4():
    f = dict(FORMEN['hangar'])
    f['schwanz_kette'] = dict(f['schwanz_kette'], **SCHWANZ_V4)
    f['kopf_v4'] = KOPF_V4
    f['koerper_zeilen_v4'] = KOERPER_ZEILEN_V4
    if V4_TEXTUR == 'ref2':                    # V4 Originaltexturen: Koerper-UV/CLUT wie Referenz 2, Kopf/Schwanz in Zeile 480
        f['kopf_v4'] = dict(KOPF_V4, clut=V4_REF2_ZEILE)
        f['schwanz_kette'] = dict(f['schwanz_kette'], uv=dict(SCHWANZ_V4['uv'], clut=V4_REF2_ZEILE << 6))
        f['koerper_zeilen_v4'] = None
    return f


def koerper_zeilen_v4(meshes, zeilen):
    """V4 Runde 3 (O1): CLUT-Feld (Zeile, Spalte 0 wie EM30) der Flaechen eines Koerper-Meshes auf eine eigene Zeile; UV, Ecken,
    Normalen und Dreiecke bleiben (Riegel G1 Geometrie)."""
    for b, (z, _alt) in zeilen.items():
        m = meshes[b]
        for ul in (m['tuv'], m['quv']):
            for uv in ul:
                uv[2] = (z << 6) | (uv[2] & 0x3F)


_V4_KAM = {}


def _v4_kamera():
    import birkin_buehne as BB
    if not _V4_KAM:
        r = BB.Raum('50A0')
        s = r.schnitt(12)
        _V4_KAM['k'] = BB.Kamera(s['pos'], s['ziel'], s['fov'] >> 7)
        _V4_KAM['M'] = BB.abbildung_laden('50A0', 12)
    return _V4_KAM['k'], _V4_KAM['M']


def v4_proj(pose, P):
    """Punkt(e) im Modellraum der Pose (mo.geometrie) -> Videopunkt 640x480 der Spielkamera 50A0/12 (wie b2.projiziere)."""
    import birkin_buehne as BB
    kam, M = _v4_kamera()
    g = int(pose['gier'])
    s, c = EA.sin_q12(g) / 4096.0, EA.cos_q12(g) / 4096.0
    Y = np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])
    P = np.atleast_2d(np.asarray(P, float)) @ Y.T + np.array([pose['x'], pose.get('y', 0), pose['z']], float)
    q = P @ (kam.V.astype(float) / 4096.0).T + kam.t.astype(float)
    sx = BB.OFX + q[:, 0] * kam.h / q[:, 2]
    sy = BB.OFY + q[:, 1] * kam.h / q[:, 2]
    x, y = BB.abbilden(sx + BB.PHASE, sy + BB.PHASE, M)
    return np.stack([x, y], 1)


def v4_kamerapunkt(pose):
    """Kameraort im Modellraum der Pose."""
    kam, _M = _v4_kamera()
    g = int(pose['gier'])
    s, c = EA.sin_q12(g) / 4096.0, EA.cos_q12(g) / 4096.0
    Y = np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])
    return Y.T @ (np.array(kam.pos, float) - np.array([pose['x'], pose.get('y', 0), pose['z']], float))


def v4_rueck(pose, xy, z):
    """Videopunkt -> Modellpunkt der Pose auf der Ebene Modell-z = z (Newton mit numerischer Ableitung)."""
    p = np.array([0.0, -2000.0, float(z)])
    for _ in range(40):
        f0 = v4_proj(pose, p)[0]
        J = np.zeros((2, 2))
        for k in range(2):
            d = np.zeros(3); d[k] = 1.0
            J[:, k] = v4_proj(pose, p + d)[0] - f0
        st = np.linalg.solve(J, f0 - np.asarray(xy, float))
        p[:2] -= st
        if np.abs(st).max() < 1e-4:
            break
    return p


def _v4_kf(mo, pose):
    f0, nf = mo.clips[pose['clip']]
    return mo.frames[f0 + pose['bild'] % nf] & 0xFFF


def kette_v4_punkte(sw, mo, A):
    """Schwanz V4: Gelenke aus den Bildmassen der Vorlage 2 (SCHWANZ_V4 gelenke/durchmesser in H) per Rueckprojektion durch die
    Spielkamera auf die Mittelebene der Figur (Modell-z des Ankerknochens A in der Pose); Radien aus dem Bildmassstab am Gelenk.
    Bildgroessen der Figur aus den projizierten Ecken der Koerpermeshes 0-15: Oberkante, Sohle, H, Buckelscheitel (Mitte der Ecken in
    der obersten 0,01 H). Rueckgabe: Gelenke im Raum von A, Radien (E.), Info."""
    p = sw['pose']
    kf = _v4_kf(mo, p)
    V, _N, F = mo.geometrie(kf)
    koerper = sorted({i for f in F if f['bone'] < 16 for i in f['v']})
    xy = v4_proj(p, V[koerper])
    ok, so = float(xy[:, 1].min()), float(xy[:, 1].max())
    H = so - ok
    scheitel = float(xy[xy[:, 1] <= ok + 0.01 * H, 0].mean())
    bx, by = scheitel - sw['beginn'][0] * H, so - sw['beginn'][1] * H
    R, T = mo.pose(kf)
    Ra, Ta = R[A].astype(float) / 4096.0, T[A].astype(float)
    z = float(T[A][2])
    zk = 1.0 if v4_kamerapunkt(p)[2] > z else -1.0              # Modell-z-Richtung zur Kamera (seitliche Ansicht)
    pk, rad = [], []
    for i, ((dx, dy), d) in enumerate(zip(sw['gelenke'], sw['durchmesser'])):
        q = v4_rueck(p, (bx + dx * H, by + dy * H), z)
        e = v4_proj(p, np.vstack([q, q + np.array([0.0, 100.0, 0.0])]))
        pxu = float(np.linalg.norm(e[1] - e[0])) / 100.0
        tz = sw.get('tiefe', [0.0] * len(sw['gelenke']))[i]
        if tz:                                                   # Versatz zur Kamera (H, Bildmassstab am Gelenk), dann neu rueckprojizieren
            q = v4_rueck(p, (bx + dx * H, by + dy * H), z + zk * tz * H / pxu)
        pk.append(tuple(Ra.T @ (q - Ta)))
        rad.append(0.5 * d * H / pxu)
    return pk, rad, dict(H=H, ok=ok, sohle=so, scheitel=scheitel, beginn=(bx, by), z=z)


def kette_ruhe_v4(pk, rad):
    """Ruhegestalt aus den Gelenken direkt (keine Catmull-Rom-Neuverteilung: die Biegung und der Knick muessen auf Gelenke fallen,
    Messblatt V4 Abschnitt 4 Punkt 2)."""
    p = np.array(pk, float)
    n = len(p) - 1
    L = np.linalg.norm(np.diff(p, axis=0), axis=1)
    lokal, W = [], np.eye(3)
    for i in range(n):
        d = W.T @ (p[i + 1] - p[i])
        Li = _quant(_min_rot(np.array([0.0, 1.0, 0.0]), d))
        lokal.append(Li)
        W = W @ Li
    rel = [tuple(int(round(c)) for c in p[0])] + [(0, int(round(L[i - 1])), 0) for i in range(1, n)]
    s = np.r_[0, np.cumsum(L)] / L.sum()
    return dict(p=p, L=[int(round(x)) for x in L], lokal=lokal, rel=rel, radien=np.array(rad, float), s=s)


def kette_meshes_v4(sw, ruhe, welt, bogenmitte, ebene_n=None):
    """Schwanz V4: Kegelstuempfe wie kette_meshes_v3 (Gehrung an den Gelenken, Deckel, Spitze), Textur je Flaeche nach Farbklasse
    (SCHWANZ_V4 uv baender): Glied 0 'wurzel', Glieder 5-6 'ende', sonst 'aussen'/'unten' nach der Flaechennormale in der Ruhe-Pose:
    aussen = Normale zeigt weg von der Bogenmitte (welt[i] = (Drehung, Ursprung) des Glieds im Ankerraum, bogenmitte im Ankerraum)."""
    S, uv = sw['seiten'], sw['uv']
    out, n = [], sw['glieder']
    # V4 Runde 5 (uv art 'achse'): Bogenlaenge je Gliedanfang und gesamt (ab dem Deckel des Glieds 16, -ueberlapp x Radius 0)
    s_anf = np.r_[0.0, np.cumsum([float(x) for x in ruhe['L']])]
    ue0 = sw['ueberlapp'] * float(ruhe['radien'][0])
    s_ges = ue0 + float(s_anf[-1])
    ey = np.array([0.0, 1.0, 0.0])
    for i in range(n):
        Li, r0, r1 = ruhe['L'][i], ruhe['radien'][i], ruhe['radien'][i + 1]
        ue = sw['ueberlapp'] * r0
        m = dict(v=[], n=[], tf=[], tuv=[], qf=[], quv=[])
        t0 = ey if i == 0 else (ruhe['lokal'][i].T @ ey + ey)
        t1 = ey if i == n - 1 else (ey + ruhe['lokal'][i + 1] @ ey)
        ringe = []
        W, P0 = welt[i]
        achse_mitte = P0 + W @ np.array([0.0, Li / 2.0, 0.0])
        # V4 Runde 5 (Pruefung V4 r4 Zahlen 4.1 / Sicht 1.3, Entscheid O8: "Ober- und Unterkontur des waagerechten Teils nicht gerade",
        # Messblatt Abschnitt 4 "von da an breiter nach oben"): ein Zwischenring ('bauch') im Glied bauch['glied'] bei bauch['t'] der
        # Gliedlaenge, Radius bauch['radius'] x linear zwischen den Gelenkradien, Mitte um bauch['aussen'] x Radius nach aussen (weg von der
        # Bogenmitte, in der Bogenebene) - die Oberkontur bekommt die Rundung der Original-Bogenecke (Leser-Maske A, Spalten x 284-316).
        ringdef = [(np.array([0.0, -ue, 0.0]), t0, r0, 0.0)]
        bauch = sw.get('bauch')
        if bauch and i == bauch['glied']:
            ft = float(bauch['t'])
            rm = float(bauch['radius']) * ((1.0 - ft) * r0 + ft * r1)
            u_ = achse_mitte - bogenmitte
            if ebene_n is not None:
                u_ = u_ - ebene_n * float(u_ @ ebene_n)
            u_l = W.T @ u_
            u_l[1] = 0.0
            u_l = u_l / max(float(np.linalg.norm(u_l)), 1e-9)
            ringdef.append((np.array([0.0, ft * float(Li), 0.0]) + u_l * float(bauch['aussen']) * rm, ey, rm, ft))
        ringdef.append((np.array([0.0, float(Li), 0.0]), t1, r1, 1.0))
        for c0, t, r, _tt in ringdef:
            Rt = _min_rot(ey, t / np.linalg.norm(t))
            ring = []
            for k in range(S):
                w = 2 * np.pi * k / S
                q = Rt @ np.array([np.cos(w), 0.0, np.sin(w)])
                ring.append(len(m['v']))
                m['v'].append([int(round(x)) for x in c0 + q * r] + [0])
                m['n'].append([int(round(x)) for x in q * 4096] + [0])
            ringe.append(ring)

        vl = uv.get('verlauf')

        def c_wert(nrm_lokal):
            nw = W @ nrm_lokal
            u_ = achse_mitte - bogenmitte
            if ebene_n is not None:                # nur die Richtung in der Ebene des Bogens (der Ansatz liegt 'tiefe' vor der Flanke)
                u_ = u_ - ebene_n * float(u_ @ ebene_n)
                ga = W @ ey                        # V4 Runde 2: Aussennormale der Bogenlinie = Anteil quer zur Gliedachse
                u_ = u_ - ga * float(u_ @ ga)
            return float(nw @ u_) / max(float(np.linalg.norm(nw) * np.linalg.norm(u_)), 1e-9)

        def klasse(nrm_lokal):
            if i == 0:
                return 'wurzel'
            if i >= 5:
                return 'ende'
            c = c_wert(nrm_lokal)
            if 'mitte' not in uv['baender']:
                return 'aussen' if c > 0 else 'unten'
            if c > uv['grenze']:
                return 'aussen'
            if i == 1 and not vl:
                return 'wurzel'
            if c < -uv['grenze']:
                return 'unten'
            return 'senkrecht' if (i >= 3 and 'senkrecht' in uv['baender']) else 'mitte'

        us = min(uv['u'][1] - uv['u'][0], Li * uv['dichte'])
        vs_ = min(63.0, np.pi * 0.5 * (r0 + r1) * uv['dichte'])

        def tx(kl, k, t):
            if uv.get('art') == 'achse':
                # V4 Runde 5 (Entscheid O8, Sicht-Pruefung V4 r4 1.3: drei einfarbige Bloecke je Mesh mit Quernaehten 16,7 dE76): EINE
                # durchgehende Textur ueber alle Glieder. v = Bogenlaenge der Ruhe-Kette ab dem Deckel (Ring am Gliedanfang -ue, am Ende L),
                # fortlaufend ueber die Gliedgrenzen; u = Lage um den Umfang (1 - c) / 2 x Spanne des Rings (c wie die Klassen der Runde 4:
                # +1 Aussenkante, -1 Innenseite; Vorder- und Rueckseite teilen den Texel). Spanne = pi x Ringradius x dichte, hoechstens
                # die Breite der Flaeche - Texeldichte damit je Glied sqrt(dichte x v-Dichte) (Messblatt M4 0,045-0,091).
                wk = 2 * np.pi * (k % S) / S
                c = max(-1.0, min(1.0, c_wert(np.array([np.cos(wk), 0.0, np.sin(wk)]))))
                r_ = {rd[3]: rd[2] for rd in ringdef}.get(t, r1)
                s_ = ue0 + s_anf[i] + (-ue if t == 0.0 else t * float(Li))
                sp = min(float(uv['u'][1] - uv['u'][0]), np.pi * r_ * uv['dichte'])
                return (int(round(uv['u'][0] + 0.5 * (1.0 - c) * sp)),
                        int(round(uv['v'][0] + (uv['v'][1] - uv['v'][0]) * min(max(s_ / s_ges, 0.0), 1.0))))
            v0, v1 = uv['baender'][kl]
            if vl and kl in ('aussen', 'mitte', 'senkrecht', 'unten'):
                # V4 Runde 4 (Pruefung V4 r3 Sicht 1.3 Klasse A, "Vierkant-Balken"): Zeile = Lage um den Umfang (c der Eckennormale), nicht
                # mehr gespiegelter Umfangsindex. Die Baender tragen einen Verlauf ueber c (eichen_v4.py), benachbarte Flaechen
                # verschiedener Klassen treffen sich an der Klassengrenze in derselben Farbe -> keine harte Kante zwischen Deck- und Seitenflaeche
                wk = 2 * np.pi * (k % S) / S
                c = c_wert(np.array([np.cos(wk), 0.0, np.sin(wk)]))
                g_ = uv['grenze']
                c_hi = 1.0 if kl == 'aussen' else (-g_ if kl == 'unten' else g_)
                c_lo = g_ if kl == 'aussen' else (-1.0 if kl == 'unten' else -g_)
                c = min(max(c, c_lo), c_hi)
                zj = vl['zeilen_je_c']
                zj = zj[kl] if isinstance(zj, dict) else zj
                return (int(round(uv['u'][0] + us * t)), int(round(v0 + (c_hi - c) * zj)))
            a = 1.0 - abs(1.0 - 2.0 * (k % (S + 1)) / S)
            return (int(round(uv['u'][0] + us * t)), int(round(v0 + min(vs_, float(v1 - v0)) * a)))

        def orient(ia, ib, ic):
            P = [np.array(m['v'][j][:3], float) for j in (ia, ib, ic)]
            N = sum(np.array(m['n'][j][:3], float) for j in (ia, ib, ic))
            return np.cross(P[1] - P[0], P[2] - P[0]) @ N < 0
        for j_, k in [(j_, k) for j_ in range(len(ringe) - 1) for k in range(S)]:
            k2 = (k + 1) % S
            ta_, tb_ = ringdef[j_][3], ringdef[j_ + 1][3]
            a, b = ringe[j_][k], ringe[j_][k2]
            c, d = ringe[j_ + 1][k], ringe[j_ + 1][k2]
            w = 2 * np.pi * (k + 0.5) / S
            kl = klasse(np.array([np.cos(w), 0.0, np.sin(w)]))
            uvs = [tx(kl, k, ta_), tx(kl, k + 1, ta_), tx(kl, k, tb_), tx(kl, k + 1, tb_)]
            if not orient(a, b, c):
                a, b, c, d = b, a, d, c
                uvs = [uvs[1], uvs[0], uvs[3], uvs[2]]
            m['qf'].append([a, a, b, b, c, c, d, d])
            m['quv'].append([uvs[0][0], uvs[0][1], uv['clut'], uvs[1][0], uvs[1][1], uv['page'],
                             uvs[2][0], uvs[2][1], 0, uvs[3][0], uvs[3][1], 0])
        kegel = []
        if i == 0 and sw.get('deckel', True):
            kegel.append((ringe[0], [0, int(round(-ue - 0.8 * r0)), 0], [0, -4096, 0], 0.0))
        if i == n - 1:
            kegel.append((ringe[-1], [0, int(round(Li + sw['dorn'] * r1)), 0], [0, 4096, 0], 1.0))
        for ring, p, nn, t in kegel:
            sv = len(m['v'])
            m['v'].append(p + [0]); m['n'].append(nn + [0])
            for k in range(S):
                a, b = ring[k], ring[(k + 1) % S]
                if not orient(a, b, sv):
                    a, b = b, a
                w = 2 * np.pi * (k + 0.5) / S
                kl = klasse(np.array([np.cos(w), 0.0, np.sin(w)]))
                p0, p1 = tx(kl, k, t), tx(kl, k + 1, t)
                m['tf'].append([a, a, b, b, sv, sv])
                m['tuv'].append([p0[0], p0[1], uv['clut'], p1[0], p1[1], uv['page'], (p0[0] + p1[0]) // 2, p0[1], 0])
        out.append(m)
    return out


def kopf_v4_form_uv(meshes, kp, mo):
    """Kopf V4: Mesh 9 (mesh-lokal) mit kp['skal'] um den Knochenursprung skalieren (Normalen invers-transponiert, wie skaliere_md1),
    dann je Flaeche die Ansicht waehlen (vorn/seitlich: Normale . Richtung zur Kamera, groesserer Wert, >= schwelle) und die UVs als
    Parallelprojektion des Videobilds dieser Ansicht in die Flaeche u/v legen (gleicher Massstab in x und y, zentriert). Ergebnis in
    BAU_INFO['kopf_v4']: Abbildung je Ansicht (Bildbox, Massstab, Versatz) fuer die Eichung (Texel <-> Bildpunkt)."""
    m = meshes[9]
    sk = np.array(kp['skal'], float)
    vb = kp.get('vorbau')

    def glatt(t):
        t = np.clip(t, 0.0, 1.0)
        return t * t * (3 - 2 * t)
    if vb:
        # V4 Runde 2: Haarvorhang mit senkrechter Vorderkante IM BILD der Vorlage-2-Kamera. n = Richtung "Bild-x nach rechts" (vorn)
        # im Kopfraum der Pose "85" (Kamera-Zeile 0 -> Modell (Gier) -> Kopfraum (Knochen 9)); Ebene n . q = n . auge + vor (E.),
        # vor = kp['vorbau']['vor'] x H / Bildmassstab am Auge (H und Massstab aus den projizierten Koerperecken wie kette_v4_punkte).
        pose_s = kp['seite']['pose']
        kam, _M = _v4_kamera()
        g = int(pose_s['gier'])
        sg, cg = EA.sin_q12(g) / 4096.0, EA.cos_q12(g) / 4096.0
        Yg = np.array([[cg, 0, sg], [0, 1, 0], [-sg, 0, cg]])
        kf_s = _v4_kf(mo, pose_s)
        R_s, T_s = mo.pose(kf_s)
        R9s, T9s = R_s[9].astype(float) / 4096.0, T_s[9].astype(float)
        n_bild = R9s.T @ (Yg.T @ (kam.V[0].astype(float) / 4096.0))
        n_bild /= np.linalg.norm(n_bild)
        Vk, _Nk, Fk = mo.geometrie(kf_s)
        ecken = sorted({i for f in Fk if f['bone'] < 16 for i in f['v']})
        xy_k = v4_proj(pose_s, Vk[ecken])
        H_s = float(xy_k[:, 1].max() - xy_k[:, 1].min())
        a_m = np.array(kp['auge'], float) @ R9s.T + T9s
        e2 = v4_proj(pose_s, np.vstack([a_m, a_m + R9s @ n_bild * 100.0]))
        pxu = float(abs(e2[1, 0] - e2[0, 0])) / 100.0
        ebene_n = float(np.array(kp['auge'], float) @ n_bild) + vb['vor'] * H_s / pxu
        BAU_INFO['kopf_v4_vorhang'] = dict(n=n_bild.tolist(), ebene=ebene_n, H=H_s, pxu=pxu)
    for v in m['v']:
        q = np.array(v[:3], float) * sk
        if vb:
            # Haarvorhang (KOPF_V4 vorbau): Gewicht glatt in z, x und y; Schub entlang n bis zur Ebene (eine Richtung im Bild von Vorlage 2).
            # Verworfen (r2_kopf_expt): Schub entlang der Kopf-x-Achse (K10-Hoehe 0,209-0,272 H) und entlang der Blickrichtung der
            # Vorderkamera (die Rueckseite des Kopfes kam vor das Auge, Treffer 57-139 E. davor).
            w = glatt((q[2] - vb['z0']) / (vb['z1'] - vb['z0'])) * glatt((q[0] - vb['xa']) / (vb['xb'] - vb['xa'])) * \
                (1.0 - glatt((q[1] - vb['y1']) / (vb['y2'] - vb['y1'])))
            q = q + w * max(0.0, ebene_n - float(q @ n_bild)) * n_bild
        v[0], v[1], v[2] = (int(round(c)) for c in np.clip(q, -32768, 32767))
    for nn in m['n']:
        a = np.array(nn[:3], float); L = np.linalg.norm(a)
        if L:
            b = a / sk; b = b / np.linalg.norm(b) * L
            nn[0], nn[1], nn[2] = (int(round(c)) for c in b)
    ab = {}
    V9 = np.array([v[:3] for v in m['v']], float)
    for ans in ('vorn', 'seite'):
        pose = kp[ans]['pose']
        R, T = mo.pose(_v4_kf(mo, pose))
        R9, T9 = R[9].astype(float) / 4096.0, T[9].astype(float)
        xy = v4_proj(pose, V9 @ R9.T + T9)
        x0, y0 = xy.min(0); x1, y1 = xy.max(0)
        u0, u1 = kp[ans]['u']; v0, v1 = kp[ans]['v']
        s = min((u1 - u0) / (x1 - x0), (v1 - v0) / (y1 - y0))
        ou = u0 + 0.5 * ((u1 - u0) - s * (x1 - x0)); ov = v0 + 0.5 * ((v1 - v0) - s * (y1 - y0))
        ab[ans] = dict(R9=R9, T9=T9, xy=xy, x0=float(x0), y0=float(y0), s=float(s), ou=float(ou), ov=float(ov),
                       kam=v4_kamerapunkt(pose))
    wahl = {'vorn': 0, 'seite': 0, 'alt': 0}
    for fl, ul, n in ((m['tf'], m['tuv'], 3), (m['qf'], m['quv'], 4)):
        for i, f in enumerate(fl):
            idx = [f[2 * k + 1] for k in range(n)]
            N = sum(np.array(m['n'][f[2 * k]][:3], float) for k in range(n))
            N /= max(np.linalg.norm(N), 1e-9)
            best, sc = None, -9.0
            for ans, a in ab.items():
                mitte = (V9[idx] @ a['R9'].T + a['T9']).mean(0)
                blick = a['kam'] - mitte; blick /= np.linalg.norm(blick)
                c = float((a['R9'] @ N) @ blick)
                if c > sc:
                    best, sc = ans, c
            if sc < kp['schwelle']:
                # Rueckseite: in keiner Vorlage-Kamera sichtbar. Eichlauf 12: mit dem Referenz-2-Wundfleisch (Zeile 485, hell) bildeten
                # streifend sichtbare Rueckflaechen seitlich ueber dem Auge eigene helle Komponenten (S1) -> auch sie bekommen das Bild
                # der naeheren Ansicht (Zonenfarben)
                wahl['alt'] += 1
            a = ab[best]
            uv = list(ul[i])
            for k in range(n):
                x, y = a['xy'][idx[k]]
                uv[3 * k] = int(round(a['ou'] + (x - a['x0']) * a['s']))
                uv[3 * k + 1] = int(round(a['ov'] + (y - a['y0']) * a['s']))
            uv[2], uv[5] = kp['clut'] << 6, kp['page']
            ul[i] = uv
            wahl[best] += 1
    st = kp.get('straehne')
    if st:
        a = ab['seite']
        x0, y0 = a['xy'].min(0); x1, y1 = a['xy'].max(0)
        pose = kp['seite']['pose']
        R9, T9 = a['R9'], a['T9']
        mitte = (x0 + (x1 - x0) * np.mean(st['x']), y0 + (y1 - y0) * np.mean(st['y']))
        welt = V9 @ R9.T + T9
        j = int(np.argmin(np.linalg.norm(a['xy'] - np.array(mitte), axis=1)))
        zk = 1.0 if a['kam'][2] > welt[j][2] else -1.0
        z = welt[j][2] + zk * st['vor']
        ecken = []
        for fx, fy in ((st['x'][0], st['y'][0]), (st['x'][1], st['y'][0]), (st['x'][0], st['y'][1]), (st['x'][1], st['y'][1])):
            q = v4_rueck(pose, (x0 + (x1 - x0) * fx, y0 + (y1 - y0) * fy), z)
            ecken.append(R9.T @ (q - T9))
        E = np.array(ecken)
        nrm = R9.T @ np.array([0.0, 0.0, zk]); nrm /= np.linalg.norm(nrm)
        b0, n0 = len(m['v']), len(m['n'])
        for e in E:
            m['v'].append([int(round(c)) for c in e] + [0])
            m['n'].append([int(round(c)) for c in nrm * 4096] + [0])
        uvs = []
        for fx, fy in ((st['x'][0], st['y'][0]), (st['x'][1], st['y'][0]), (st['x'][0], st['y'][1]), (st['x'][1], st['y'][1])):
            x, y = x0 + (x1 - x0) * fx, y0 + (y1 - y0) * fy
            uvs.append((int(round(a['ou'] + (x - a['x0']) * a['s'])), int(round(a['ov'] + (y - a['y0']) * a['s']))))
        ia, ib, ic, idd = b0, b0 + 1, b0 + 2, b0 + 3
        if np.cross(E[1] - E[0], E[2] - E[0]) @ nrm >= 0:          # Windung wie EM30: Kreuzprodukt gegen die Normale
            ia, ib, ic, idd = ib, ia, idd, ic
            uvs = [uvs[1], uvs[0], uvs[3], uvs[2]]
        m['qf'].append([ia - b0 + n0, ia, ib - b0 + n0, ib, ic - b0 + n0, ic, idd - b0 + n0, idd])
        m['quv'].append([uvs[0][0], uvs[0][1], kp['clut'] << 6, uvs[1][0], uvs[1][1], kp['page'], uvs[2][0], uvs[2][1], 0,
                         uvs[3][0], uvs[3][1], 0])
    BAU_INFO['kopf_v4'] = dict(wahl=wahl, **{k: dict(x0=v['x0'], y0=v['y0'], s=v['s'], ou=v['ou'], ov=v['ov']) for k, v in ab.items()})


# V4 Runde 5 (Entscheide O9 "keine Farbe, die im Original-Bild an der Stelle nicht vorkommt" / O10 "kraeftiges Rot a* > 22 im Spielbild
# in der Groessenordnung des Videos"; Pruefung V4 r4 Sicht 1.5: h85 1170 / h100 531 Bildpunkte a* > 22 gegen 2 / 107 im Video): Obergrenze
# fuer a* der Kopf-, Schwanz- und Koerperzeilen. FARB_A_MAX = p99 von a* der Vorlage 1 (entblendet) im Oberkoerper-Polygon der Sicht-Pruefung
# (r4_mass.POLY_135; gemessen 21,7, p99,9 25,1, max 29,3). Eintraege darueber behalten L* und b*, a* wird auf die Grenze gesetzt
# (Entscheid O9: die Farbmenge des Originals). Ohne Wirkung, wenn RE15_V4_A_MAX=0.
FARB_A_MAX = float(os.environ.get('RE15_V4_A_MAX', '21.7'))


def farbklammer(rgb):
    """rgb (N x 3, 0..255) -> dieselben Farben mit a* <= FARB_A_MAX (L*, b* bleiben)."""
    rgb = np.asarray(rgb, float)
    if FARB_A_MAX <= 0:
        return rgb
    lab = _lab(rgb)
    out = rgb.copy()
    for i in np.nonzero(lab[:, 1] > FARB_A_MAX)[0]:
        out[i] = _lab_rgb(lab[i, 0], FARB_A_MAX, lab[i, 2])
    return out


def v4_ref2tex_einsetzen(tim, kp, sw):
    """V4 Originaltexturen (V4_TEXTUR 'ref2'): nur die Texel der freien Atlasflaechen Kopf vorn/seitlich (TPage 128 v 160-223) und Schwanz
    (TPage 130 u 64-127) aus V4_REF2TEX (Indizes in die Referenz-2-Zeile V4_REF2_ZEILE); CLUT-Block und alle uebrigen Texel bleiben
    bytegleich. Ohne Datei: Index 0 der Flaechen bleibt (Bau des ersten Stands fuer ref2tex_v4.py)."""
    tim = bytearray(tim)
    _cx, _cy, _cw, _ch, _c, ende = _clut(tim)
    iw, ih = struct.unpack_from('<HH', tim, ende + 8)
    breite = iw * 2
    pix = np.frombuffer(bytes(tim[ende + 12:ende + 12 + breite * ih]), np.uint8).reshape(ih, breite).copy()
    f = dict(np.load(V4_REF2TEX)) if os.path.exists(V4_REF2TEX) else {}
    pb = 128 * (kp['page'] & 0xF)
    for name, ans in (('kopf_vorn', kp['vorn']), ('kopf_seite', kp['seite'])):
        u0, u1 = ans['u']; v0, v1 = ans['v']
        if name in f:
            pix[v0:v1 + 1, pb + u0:pb + u1 + 1] = f[name]
    uv = sw['uv']
    pbs = 128 * (uv['page'] & 0xF)
    if 'schwanz' in f:
        pix[uv['v'][0]:uv['v'][1] + 1, pbs + uv['u'][0]:pbs + uv['u'][1] + 1] = f['schwanz']
    tim[ende + 12:ende + 12 + breite * ih] = pix.tobytes()
    return bytes(tim)


def v4_farben_einsetzen(tim, kp, sw, kz=None):
    """Eichung (V4_FARBEN, tools/r41_sichel/v4/eichen_v4.py) in den TIM: Texelbilder Kopf vorn/seitlich und Schwanzbaender (Indizes),
    CLUT-Zeilen 486 (Kopf) / 487 (Schwanz) und die geeichten Hautzeilen 480-485 (Koerperfarben F6-F19 nur ueber die CLUT, Texel der
    Meshes 0-15 unveraendert). Ohne Datei: Platzhalter (Index 128, Zeilen grau), damit die Eichung den ersten Stand rendern kann."""
    tim = bytearray(tim)
    cx, cy, cw, ch, c, ende = _clut(tim)
    iw, ih = struct.unpack_from('<HH', tim, ende + 8)
    breite = iw * 2
    pix = np.frombuffer(bytes(tim[ende + 12:ende + 12 + breite * ih]), np.uint8).reshape(ih, breite).copy()
    f = dict(np.load(V4_FARBEN)) if os.path.exists(V4_FARBEN) else {}
    if sw['uv'].get('art') == 'achse':               # V4 Runde 5: Schwanz aus der Projektion (r5_schwanz_textur.py), sonst Platzhalter
        fs = dict(np.load(V4_SCHWANZ)) if os.path.exists(V4_SCHWANZ) else {}
        f['schwanz'] = fs.get('schwanz', np.full((256, 64), 128, np.uint8))
        f['clut487'] = fs.get('clut487', np.stack([np.arange(256)] * 3, -1).astype(float))
    pb = 128 * (kp['page'] & 0xF)
    for name, ans in (('kopf_vorn', kp['vorn']), ('kopf_seite', kp['seite'])):
        u0, u1 = ans['u']; v0, v1 = ans['v']
        pix[v0:v1 + 1, pb + u0:pb + u1 + 1] = f.get(name, np.full((v1 - v0 + 1, u1 - u0 + 1), 128, np.uint8))
    uv = sw['uv']
    pbs = 128 * (uv['page'] & 0xF)
    pix[0:256, pbs + uv['u'][0]:pbs + uv['u'][1] + 1] = f.get('schwanz', np.full((256, 64), 128, np.uint8))
    grau = np.stack([np.arange(256)] * 3, -1).astype(float)
    zeilen = {kp['clut'] - cy: farbklammer(f.get('clut486', grau)), (uv['clut'] >> 6) - cy: farbklammer(f.get('clut487', grau))}
    neu = c.copy()
    if 'haut' in f:                                   # geeichte Zeilen 480.. (RGB 0..255 je Eintrag; transparente bleiben)
        for z in range(min(ch, f['haut'].shape[0])):
            neu[z] = _pack(np.asarray(f['haut'][z], float), c[z])
    # Runde 3 (O1): eigene Koerperzeilen (KOERPER_ZEILEN_V4) zuerst als Kopie ihrer Startzeile, dann ggf. aus farben_koerper_v4.npz
    hoch = max([ch] + [z + 1 for z in zeilen] + [zn - cy + 1 for zn, _a in (kz or {}).values()])
    alle = np.zeros((hoch, cw), np.int64)
    alle[:ch] = neu
    for zn, za in (kz or {}).values():
        alle[zn - cy] = neu[za - cy]
    if os.path.exists(V4_KOERPER):                    # Runde 2: Koerpertexel (Indizes) und ihre Zeilen, UVs/Primitive unveraendert (G1)
        fk = dict(np.load(V4_KOERPER))
        ki = fk['koerper_idx']
        m = ki >= 0
        pix[m] = ki[m].astype(np.uint8)
        for j, zn in enumerate(fk['koerper_zeilen_nr']):
            ziel = int(zn) - cy
            if ziel < hoch:
                alle[ziel] = _pack(farbklammer(fk['koerper_zeilen'][j]), alle[ziel] if ziel >= ch else c[ziel])
        for zn, za in (kz or {}).values():            # Datei ohne die eigene Zeile (Stand Runde 2): Kopie der geeichten Startzeile
            if zn not in set(int(x) for x in fk['koerper_zeilen_nr']):
                alle[zn - cy] = alle[za - cy]
    for z, rgb in zeilen.items():
        a = np.full(256, 0x7FFF, np.int64); a[0] = 0
        alle[z] = _pack(np.asarray(rgb, float), a)
    alle = alle.astype('<u2')
    blk = struct.pack('<I4H', 12 + alle.size * 2, cx, cy, cw, alle.shape[0]) + alle.tobytes()
    return bytes(tim[:8]) + blk + bytes(tim[ende:ende + 12]) + pix.tobytes() + bytes(tim[ende + 12 + breite * ih:])


# ═══════════════════════════════════════════════════ MD1: Vertex-/Normalen-Offsets
def md1_bloecke(md1):
    """Je Mesh die Byte-Offsets (relativ zum MD1-Anfang) der Vertex- und Normalenbloecke.
    Gleiches Layout wie EA.md1_parse; Tri- und Quad-Teil koennen auf DENSELBEN Block
    zeigen, darum wird ueber (Offset, Anzahl) dedupliziert."""
    obj, base, out = u32(md1, 8), 12, []
    for i in range(obj // 2):
        (tvo, tvc, tno, tnc, _tfo, _tfc, _tuo,
         qvo, qvc, qno, qnc, _qfo, _qfc, _quo) = struct.unpack_from('<14I', md1, 12 + i*56)
        vb = {(base + tvo, tvc), (base + qvo, qvc)}
        nb = {(base + tno, tnc), (base + qno, qnc)}
        out.append(([b for b in vb if b[1]], [b for b in nb if b[1]]))
    return out


def skaliere_md1(md1, skal):
    """Vertices v -> S v, Normalen n -> S^-1 n (auf die Ursprungslaenge gebracht)."""
    md1 = bytearray(md1)
    gesehen = set()                       # ein Block, den zwei Meshes teilen, nur einmal
    for b, (vbl, nbl) in enumerate(md1_bloecke(md1)):
        if b not in skal:
            continue
        s = np.array(skal[b], dtype=np.float64)
        for off, n in vbl:
            if (off, n) in gesehen:
                continue
            gesehen.add((off, n))
            for k in range(n):
                x, y, z, w = struct.unpack_from('<4h', md1, off + k*8)
                v = np.rint(np.array([x, y, z]) * s).astype(int)
                v = np.clip(v, -32768, 32767)
                struct.pack_into('<4h', md1, off + k*8, int(v[0]), int(v[1]), int(v[2]), w)
        for off, n in nbl:
            if (off, n) in gesehen:
                continue
            gesehen.add((off, n))
            for k in range(n):
                x, y, z, w = struct.unpack_from('<4h', md1, off + k*8)
                alt = np.array([x, y, z], dtype=np.float64)
                laenge = np.linalg.norm(alt)
                if laenge == 0:
                    continue
                neu = alt / s
                neu = neu / np.linalg.norm(neu) * laenge
                neu = np.clip(np.rint(neu).astype(int), -32768, 32767)
                struct.pack_into('<4h', md1, off + k*8, int(neu[0]), int(neu[1]), int(neu[2]), w)
    return bytes(md1)


# ═══════════════════════════════════════════════════ MD1: neu serialisieren + Schwanz
def md1_lesen(md1):
    """MD1 in bearbeitbare Listen. EM30 teilt je Mesh EINEN Vertex- und EINEN Normalenblock
    zwischen Tri- und Quad-Teil (tvo==qvo, tno==qno) - das wird geprueft und beibehalten."""
    ms = EA.md1_parse(md1)
    out = []
    for i, m in enumerate(ms):
        t = struct.unpack_from('<14I', md1, 12 + i*56)
        assert t[0] == t[7] and t[2] == t[9], 'Mesh %d: getrennte Vertexbloecke' % i
        out.append(dict(v=[list(x) for x in m.tv], n=[list(x) for x in m.tn],
                        tf=[list(x) for x in m.tf], tuv=[list(x) for x in m.tuv],
                        qf=[list(x) for x in m.qf], quv=[list(x) for x in m.quv]))
    return out


def md1_schreiben(meshes):
    """Gleiche Blockreihenfolge wie das Original: Tabelle, alle Vertexbloecke, alle
    Normalenbloecke, je Mesh Tri-/Quad-Faces, je Mesh Tri-/Quad-UVs. Unveraendertes EM30
    ergibt damit byte-gleich die Original-MD1 (siehe main --selbsttest)."""
    n = len(meshes)
    cur = n * 56
    vo, no, tfo, qfo, tuo, quo = [], [], [], [], [], []
    for m in meshes:
        vo.append(cur); cur += 8 * len(m['v'])
    for m in meshes:
        no.append(cur); cur += 8 * len(m['n'])
    for m in meshes:
        tfo.append(cur); cur += 12 * len(m['tf'])
        qfo.append(cur); cur += 16 * len(m['qf'])
    uv_start = cur
    for m in meshes:
        tuo.append(cur); cur += 12 * len(m['tf'])
        quo.append(cur); cur += 16 * len(m['qf'])
    out = bytearray(12 + cur)
    struct.pack_into('<III', out, 0, 12 + uv_start, 0, n * 2)
    for i, m in enumerate(meshes):
        nv, nn = len(m['v']), len(m['n'])
        struct.pack_into('<14I', out, 12 + i*56, vo[i], nv, no[i], nn, tfo[i], len(m['tf']), tuo[i],
                         vo[i], nv, no[i], nn, qfo[i], len(m['qf']), quo[i])
        for k, x in enumerate(m['v']):   struct.pack_into('<4h', out, 12 + vo[i] + k*8, *x)
        for k, x in enumerate(m['n']):   struct.pack_into('<4h', out, 12 + no[i] + k*8, *x)
        for k, x in enumerate(m['tf']):  struct.pack_into('<6H', out, 12 + tfo[i] + k*12, *x)
        for k, x in enumerate(m['qf']):  struct.pack_into('<8H', out, 12 + qfo[i] + k*16, *x)
        for k, x in enumerate(m['tuv']): struct.pack_into('<BBHBBHBBH', out, 12 + tuo[i] + k*12, *x)
        for k, x in enumerate(m['quv']): struct.pack_into('<BBHBBHBBHBBH', out, 12 + quo[i] + k*16, *x)
    return bytes(out)


def _catmull(p, n):
    """Catmull-Rom durch die Stuetzpunkte, n Zwischenschritte je Abschnitt."""
    p = [np.array(x, float) for x in p]
    p = [2*p[0] - p[1]] + p + [2*p[-1] - p[-2]]
    out = []
    for i in range(1, len(p) - 2):
        for k in range(n):
            t = k / n
            a, b, c, d = p[i-1], p[i], p[i+1], p[i+2]
            out.append(0.5 * ((2*b) + (-a + c)*t + (2*a - 5*b + 4*c - d)*t*t
                              + (-a + 3*b - 3*c + d)*t*t*t))
    out.append(p[-2])
    return out


def haenge_schwanz_an(meshes, sw):
    """Verjuengte Roehre entlang der Mittellinie + Spitze, in Mesh sw['knochen'].
    Windung wie im Original: cross(v1-v0, v2-v0) zeigt GEGEN die Normale (alle 347 Quads
    von EM30 erfuellen das). Normalen in der Originallaenge 4096."""
    m = meshes[sw['knochen']]
    mitte = _catmull(sw['punkte'], sw['unterteilung'])
    rad = np.interp(np.linspace(0, len(sw['radien']) - 1, len(mitte)),
                    np.arange(len(sw['radien'])), sw['radien'])
    S = sw['seiten']
    uv = sw['uv']
    v0, n0 = len(m['v']), len(m['n'])
    ringe = []
    oben = np.array([0.0, -1.0, 0.0])
    for i, c in enumerate(mitte):
        t = mitte[min(i+1, len(mitte)-1)] - mitte[max(i-1, 0)]
        t /= np.linalg.norm(t)
        a = np.cross(t, oben); a /= np.linalg.norm(a)
        b = np.cross(t, a)
        ring = []
        for k in range(S):
            w = 2*np.pi*k/S
            r = np.cos(w)*a + np.sin(w)*b
            p = c + r*rad[i]
            ring.append(len(m['v']))
            m['v'].append([int(round(x)) for x in p] + [0])
            m['n'].append([int(round(x)) for x in r*4096] + [0])
        ringe.append(ring)
    spitze_dir = mitte[-1] - mitte[-2]; spitze_dir /= np.linalg.norm(spitze_dir)
    spitze = mitte[-1] + spitze_dir * rad[-1] * 3
    sv = len(m['v'])
    m['v'].append([int(round(x)) for x in spitze] + [0])
    m['n'].append([int(round(x)) for x in spitze_dir*4096] + [0])

    def u_(k):  return int(round(uv['u'][0] + (uv['u'][1]-uv['u'][0]) * k / S))
    def v_(i):  return int(round(uv['v'][0] + (uv['v'][1]-uv['v'][0]) * i / (len(ringe)-1)))

    def orient(ia, ib, ic):
        P = [np.array(m['v'][j][:3], float) for j in (ia, ib, ic)]
        N = sum(np.array(m['n'][j][:3], float) for j in (ia, ib, ic))
        return np.cross(P[1]-P[0], P[2]-P[0]) @ N < 0

    for i in range(len(ringe) - 1):
        for k in range(S):
            k2 = (k + 1) % S
            a, b = ringe[i][k], ringe[i][k2]
            c, d = ringe[i+1][k], ringe[i+1][k2]
            uvs = [(u_(k), v_(i)), (u_(k+1), v_(i)), (u_(k), v_(i+1)), (u_(k+1), v_(i+1))]
            if not orient(a, b, c):           # Quad-Split (v0,v1,v2)+(v1,v3,v2): a<->b tauschen
                a, b, c, d = b, a, d, c
                uvs = [uvs[1], uvs[0], uvs[3], uvs[2]]
            m['qf'].append([a - v0 + n0, a, b - v0 + n0, b, c - v0 + n0, c, d - v0 + n0, d])
            m['quv'].append([uvs[0][0], uvs[0][1], uv['clut'], uvs[1][0], uvs[1][1], uv['page'],
                             uvs[2][0], uvs[2][1], 0, uvs[3][0], uvs[3][1], 0])
    letzte = ringe[-1]
    for k in range(S):
        a, b = letzte[k], letzte[(k + 1) % S]
        if not orient(a, b, sv):
            a, b = b, a
        m['tf'].append([a - v0 + n0, a, b - v0 + n0, b, sv - v0 + n0, sv])
        m['tuv'].append([u_(k), uv['v'][1], uv['clut'], u_(k+1), uv['v'][1], uv['page'],
                         (u_(k)+u_(k+1))//2, uv['v'][1], 0])
    assert v0 == n0
    return meshes


# ═══════════════════════════════════════════════════ Schwanzkette (eigene Knochen, Runde 41 Spur SF)
# Nutzerurteil 2026-11-02: "Der Schwanz ist viel zu starr". Statt starrer Geometrie an Mesh 8 bekommt der
# Schwanz eine eigene Knochenkette, ANGEHAENGT an den Knochenbaum (Knochen 16 .. 16+N-1), damit die Indizes
# 0-15 (KI, Szene Knochen 15, Clips) unveraendert bleiben. Jedes Glied hat sein Mesh (Mesh i == Knochen i),
# laeuft entlang seiner lokalen +Y und reicht mit einer Manschette (ueberlapp) in das vorige Glied, damit die
# Gelenke bei Biegung keine Luecke zeigen (E9 Bahnsteig-Form: optisch aus einem Stueck). Die Schwanzwinkel
# jedes Keyframes kommen aus einer Nachschwing-Rechnung ueber die Rumpfbewegung je Clip (schwanz_bewegung).
# Alle Zahlen hier sind Gestaltung nach dem Video (keine RE-Werte).

def _min_rot(a, b):
    """Kleinste Drehung, die Einheitsvektor a auf b dreht (Rodrigues)."""
    a = a / np.linalg.norm(a); b = b / np.linalg.norm(b)
    v = np.cross(a, b); c = float(a @ b); s = np.linalg.norm(v)
    if s < 1e-9:
        if c > 0:
            return np.eye(3)
        q = np.cross(a, [1.0, 0, 0]) if abs(a[0]) < 0.9 else np.cross(a, [0, 1.0, 0])
        q /= np.linalg.norm(q)
        return 2 * np.outer(q, q) - np.eye(3)
    k = v / s
    K = np.array([[0, -k[2], k[1]], [k[2], 0, -k[0]], [-k[1], k[0], 0]])
    return np.eye(3) + s * K + (1 - c) * (K @ K)


def _quant(L):
    """Lokale Drehung so, wie der Port sie aus dem Keyframe liest (12-Bit-Winkel, RotMatrix @0x80068130)."""
    return EA.mat_euler(*_zerlege(L)).astype(float) / 4096.0


def kette_ruhe(sw):
    """Ruhegestalt: Gelenkpunkte im Raum des Ankerknochens (gleichlange Abschnitte auf der Catmull-Rom-Linie),
    Gliedlaengen, Ruhe-Lokaldrehungen (quantisiert) und Bindversaetze der neuen Knochen."""
    linie = np.array(_catmull(sw['punkte'], 24))
    seg = np.linalg.norm(np.diff(linie, axis=0), axis=1)
    s = np.r_[0, np.cumsum(seg)]
    n = sw['glieder']
    ziel = np.linspace(0, s[-1], n + 1)
    p = np.array([np.interp(ziel, s, linie[:, k]) for k in range(3)]).T
    L = np.linalg.norm(np.diff(p, axis=0), axis=1)
    lokal, W = [], np.eye(3)
    for i in range(n):
        d = W.T @ (p[i+1] - p[i])
        Li = _quant(_min_rot(np.array([0.0, 1.0, 0.0]), d))
        lokal.append(Li)
        W = W @ Li
    rel = [tuple(int(round(c)) for c in p[0])] + [(0, int(round(L[i-1])), 0) for i in range(1, n)]
    if 'radien_s' in sw:                  # Schwanz V2: Radius je Laengenanteil (Formblatt V2 S10), Liste (s, r)
        ss, rr = zip(*sw['radien_s'])
        rad = np.interp(ziel / s[-1], ss, rr)
    else:
        rad = np.interp(ziel, np.linspace(0, s[-1], len(sw['radien'])), sw['radien'])
    return dict(p=p, L=[int(round(x)) for x in L], lokal=lokal, rel=rel, radien=rad, s=ziel / s[-1])


def kette_meshes(sw, ruhe, quelle=None):
    """Je Glied ein Rohr-Mesh im Knochenraum (Y = Laengsachse): Manschette bei -ueberlapp, Ringe bei 0, L/2, L;
    das letzte Glied endet in einer Spitze. Normalen radial in Laenge 4096, Windung wie haenge_schwanz_an.
    Runde 41 SF NB3 (Abnahme 1 Mangel 1): mit sw['profil'] (Liste (t, Radiusfaktor)) bekommt jedes Glied eigene Ringe
    entlang der Laenge (Gelenke schmal, Mitte bauchig -> gegliedertes Rohr wie im Video h64-h70), Glied 0 einen Deckel
    in den Rumpf (vorher offenes Rohrende = schwarzer Spalt an der Huefte) und das letzte Glied einen spitzen Dorn
    (sw['dorn'] x Endradius); die Textur laeuft dann je Glied ueber die ganze Bildhoehe (uv['je_glied'])."""
    S, uv = sw['seiten'], sw['uv']
    out = []
    n = sw['glieder']
    ende = sw.get('ende', 'dorn')
    for i in range(n):
        Li, r0, r1 = ruhe['L'][i], ruhe['radien'][i], ruhe['radien'][i+1]
        s0, s1 = ruhe['s'][i], ruhe['s'][i+1]
        # Schwanz V2: eigenes Profil fuer das Wurzelglied (Uebergang in den Buckel ohne Stufe, Formblatt V2 S3) und das
        # Endglied (spitz auslaufend, F2); sonst das Gliederprofil.
        profil = sw.get('profil_wurzel') if (i == 0 and sw.get('profil_wurzel')) else \
            sw.get('profil_ende') if (i == n - 1 and sw.get('profil_ende')) else sw.get('profil')
        if profil:
            ue = sw['ueberlapp'] * r0
            ringe_y = [-ue] + [t * Li for t, _f in profil]
            ringe_r = [profil[0][1] * r0] + [f * (r0 + (r1 - r0) * t) for t, f in profil]
            ringe_s = [sw.get('wurzel_v', 0.0) if i == 0 else 0.0] + [t for t, _f in profil]
        else:
            ue = sw['ueberlapp'] * r0 if i > 0 else 0.0
            ringe_y = ([-ue] if ue else []) + [0.0, Li / 2.0, float(Li)]
            ringe_r = ([r0] if ue else []) + [r0, (r0 + r1) / 2.0, r1]
            ringe_s = ([s0] if ue else []) + [s0, (s0 + s1) / 2.0, s1]
        m = dict(v=[], n=[], tf=[], tuv=[], qf=[], quv=[])
        ringe = []
        for y, r in zip(ringe_y, ringe_r):
            ring = []
            for k in range(S):
                w = 2 * np.pi * k / S
                nx, nz = np.cos(w), np.sin(w)
                ring.append(len(m['v']))
                m['v'].append([int(round(nx * r)), int(round(y)), int(round(nz * r)), 0])
                m['n'].append([int(round(nx * 4096)), 0, int(round(nz * 4096)), 0])
            ringe.append(ring)
        if i == n - 1:
            sv = len(m['v'])
            dorn = sw.get('dorn', 2.5) if ende != 'kralle' else 0.15
            m['v'].append([0, int(round(Li + dorn * r1)), 0, 0])
            m['n'].append([0, 4096, 0, 0])
        deckel = None
        if profil and i == 0:                      # Deckel in den Rumpf: Kegel vom Manschettenring nach -y
            deckel = len(m['v'])
            m['v'].append([0, int(round(-ue - 0.8 * r0)), 0, 0])
            m['n'].append([0, -4096, 0, 0])

        laengs = uv.get('laengs') == 'u'
        je_glied = uv.get('je_glied', False)
        def u_(k):  return int(round(uv['u'][0] + (uv['u'][1] - uv['u'][0]) * k / S))
        def v_(t):  return int(round(uv['v'][0] + (uv['v'][1] - uv['v'][0]) * t))

        def tx(k, t):
            """(u, v) fuer Seite k und Laengenanteil t. laengs='u' (Runde 41 SF N1.5): Laenge auf u, Umfang auf v, damit
            die senkrechten Streifen des Bandstreifens (TPage 130 u 0-42) als Ringe ueber dem Schwanz liegen."""
            if je_glied:                          # NB3: Umfang auf u, Gliedlaenge t (0..1) auf v
                return (u_(k), v_(min(1.0, max(0.0, t))))
            if laengs:
                return (int(round(uv['u'][0] + (uv['u'][1] - uv['u'][0]) * t)),
                        int(round(uv['v'][0] + (uv['v'][1] - uv['v'][0]) * k / S)))
            return (u_(k), v_(t))

        def orient(ia, ib, ic):
            P = [np.array(m['v'][j][:3], float) for j in (ia, ib, ic)]
            N = sum(np.array(m['n'][j][:3], float) for j in (ia, ib, ic))
            return np.cross(P[1] - P[0], P[2] - P[0]) @ N < 0
        for j in range(len(ringe) - 1):
            for k in range(S):
                k2 = (k + 1) % S
                a, b = ringe[j][k], ringe[j][k2]
                c, d = ringe[j+1][k], ringe[j+1][k2]
                uvs = [tx(k, ringe_s[j]), tx(k + 1, ringe_s[j]), tx(k, ringe_s[j+1]), tx(k + 1, ringe_s[j+1])]
                if not orient(a, b, c):
                    a, b, c, d = b, a, d, c
                    uvs = [uvs[1], uvs[0], uvs[3], uvs[2]]
                m['qf'].append([a, a, b, b, c, c, d, d])
                m['quv'].append([uvs[0][0], uvs[0][1], uv['clut'], uvs[1][0], uvs[1][1], uv['page'],
                                 uvs[2][0], uvs[2][1], 0, uvs[3][0], uvs[3][1], 0])
        if i == n - 1:
            letzte = ringe[-1]
            for k in range(S):
                a, b = letzte[k], letzte[(k + 1) % S]
                if not orient(a, b, sv):
                    a, b = b, a
                m['tf'].append([a, a, b, b, sv, sv])
                p0, p1 = tx(k, 1.0), tx(k + 1, 1.0)
                m['tuv'].append([p0[0], p0[1], uv['clut'], p1[0], p1[1], uv['page'],
                                 (p0[0] + p1[0]) // 2, (p0[1] + p1[1]) // 2, 0])
        if deckel is not None:
            erste = ringe[0]
            for k in range(S):
                a, b = erste[k], erste[(k + 1) % S]
                if not orient(a, b, deckel):
                    a, b = b, a
                m['tf'].append([a, a, b, b, deckel, deckel])
                p0, p1 = tx(k, 0.0), tx(k + 1, 0.0)
                m['tuv'].append([p0[0], p0[1], uv['clut'], p1[0], p1[1], uv['page'],
                                 (p0[0] + p1[0]) // 2, (p0[1] + p1[1]) // 2, 0])
        if i == n - 1 and ende == 'kralle':
            kralle_anhaengen(m, quelle[sw['kralle']['mesh']], sw['kralle'], Li)
        out.append(m)
    return out


def kralle_anhaengen(m, q, p, Li):
    """Schwanz V2, Ende 'Kralle' (Formblatt V2 S12/F2, KI-Seitenansicht h78-h97 "Hand mit roter Kralle"): eine Kopie der
    EM30-Klauenhand (Mesh 12 nach der Form-Skalierung, gleiche UVs/CLUT/TPage = Originaltexel) ans Schwanzende, Handgelenk
    am Ende des letzten Glieds, Finger entlang der Gliedachse (+y), Massstab p['skal'], Drehung p['gier'] Grad um y.
    Keine neue Geometrie-Art: die Dreiecke/Vierecke der Originalhand (32 + 26). PORT-WAHL."""
    w = np.radians(p.get('gier', 0.0))
    Ry = np.array([[np.cos(w), 0, np.sin(w)], [0, 1, 0], [-np.sin(w), 0, np.cos(w)]])
    sk = np.array(p['skal'], float)
    v0, n0 = len(m['v']), len(m['n'])
    for v in q['v']:
        x = Ry @ (np.array(v[:3], float) * sk) + np.array([0.0, Li - p.get('ueberlapp', 40.0), 0.0])
        m['v'].append([int(round(c)) for c in x] + [0])
    for nn in q['n']:
        x = Ry @ (np.array(nn[:3], float) / sk)
        x = x / max(np.linalg.norm(x), 1e-9) * 4096
        m['n'].append([int(round(c)) for c in x] + [0])
    for f in q['tf']:
        m['tf'].append([f[0] + n0, f[1] + v0, f[2] + n0, f[3] + v0, f[4] + n0, f[5] + v0])
    for f in q['qf']:
        m['qf'].append([f[0] + n0, f[1] + v0, f[2] + n0, f[3] + v0, f[4] + n0, f[5] + v0, f[6] + n0, f[7] + v0])
    m['tuv'].extend([list(x) for x in q['tuv']])
    m['quv'].extend([list(x) for x in q['quv']])


def kette_v2_punkte(sw, mo, A):
    """Schwanz V2 (Formblatt V2 S2-S9, GESTALTUNG nach der Bildmessung am Original, keine RE-Werte): Ruhe-Mittellinie im
    Modellraum der Pose sw['modellraum'] (X vorn, Y unten, Z links (W)) aus den Masszahlen in H:
      Wurzelmitte  sw['wurzel_h'] = (hinter dem hoechsten Koerperpunkt, unter der Oberkante) - S2/S5
      Pfad         sw['pfad_h'] = (hinten, unten) je Stuetzpunkt ab der Wurzelmitte - S6 (Haken, Spitze 0,15/0,51)
      Laenge       sw['laenge'] (S9, 3475 E. wie Referenz 2) - die Seitenprojektion des Pfads ist kuerzer; der Rest liegt
                   seitlich (S9/Ae2 "Seitwaertslauf"): S-Welle ab dem Anteil sw['seitlich']['ab'] der Pfadlaenge, a so gewaehlt, dass die 7 Sehnen
                   der Kette zusammen sw['laenge'] lang sind (Bisektion). PORT-WAHL: Wellenzahl w und Spitzenlage.
    Rueckgabe: Stuetzpunkte im Raum des Ankerknochens A (wie sw['punkte'])."""
    mr = sw['modellraum']
    f0, nf = mo.clips[mr['clip']]
    kf = mo.frames[f0 + mr['bild'] % nf] & 0xFFF
    V, _N, F = mo.geometrie(kf)
    koerper = sorted({i for f in F if f['bone'] < 16 for i in f['v']})
    K = V[koerper]
    oben, sohle = K[:, 1].min(), K[:, 1].max()
    H = sohle - oben
    xo = K[K[:, 1] <= oben + 0.01 * H, 0].mean()
    R, T = mo.pose(kf)
    Ra, Ta = R[A].astype(float) / 4096.0, T[A].astype(float)
    w0 = np.array([xo - sw['wurzel_h'][0] * H, oben + sw['wurzel_h'][1] * H, 0.0])
    pfad = [w0] + [w0 + np.array([-b * H, d * H, 0.0]) for b, d in sw['pfad_h']]
    if sw.get('versenkt'):
        # Gelenk 16 liegt im Buckel: das erste Stueck (sw['versenkt'] E.) laeuft gegen die Richtung des ersten Pfadabschnitts
        # in den Koerper; sichtbar beginnt der Schwanz an der Wurzelmitte w0 (Formblatt V2 S2/S3).
        r0 = pfad[1] - pfad[0]; r0 /= np.linalg.norm(r0)
        pfad = [w0 - r0 * sw['versenkt']] + pfad
    seg = np.r_[0, np.cumsum(np.linalg.norm(np.diff(np.array(pfad), axis=0), axis=1))]
    s = seg / seg[-1]
    se = sw['seitlich']

    ab = se.get('ab', 0.0)
    u = np.clip((s - ab) / (1.0 - ab), 0.0, 1.0)

    def punkte(a):
        # S-Welle ab dem Anteil `ab` (Wurzel und waagerechtes Stueck bleiben in der Seitenebene, Formblatt S3/S6), Steigung
        # 0 am Anfang und am Ende: z = a sin(2 pi u) sin(pi u) (+-0,77 a), dazu die Spitzenlage spitze H u^2.
        z = a * np.sin(2 * np.pi * se['wellen'] * u) * np.sin(np.pi * u) + se['spitze'] * H * u * u
        return [tuple(Ra.T @ (q + np.array([0.0, 0.0, zz]) - Ta)) for q, zz in zip(pfad, z)]

    def laenge(a):
        return sum(kette_ruhe(dict(sw, punkte=punkte(a)))['L'])
    lo, hi = 0.0, 3000.0
    if laenge(lo) >= sw['laenge']:
        return punkte(lo), dict(H=H, oben=oben, xo=xo, a=0.0)
    for _ in range(40):
        mi = 0.5 * (lo + hi)
        lo, hi = (mi, hi) if laenge(mi) < sw['laenge'] else (lo, mi)
    a = 0.5 * (lo + hi)
    return punkte(a), dict(H=H, oben=oben, xo=xo, a=a)



def kette_v3_punkte(sw, mo, A):
    """Schwanz V3 (Messblatt V3 Abschnitt 4/5, GESTALTUNG): Ruhe-Mittellinie als Kreisbogen gleicher Kruemmung in der
    Seitenebene (X vorn, Y unten) ab der Wurzelmitte (sw['wurzel_h'] wie V2), Startrichtung sw['bogen']['start_grad'] unter
    der Waagerechten nach hinten, Gesamtdrehung sw['bogen']['dreh_grad']; dazu ein monotoner Seitenzug z = spitze * H * u^2.
    Die Bogenlaenge in der Seitenebene wird per Bisektion so gewaehlt, dass die Kette (inkl. versenktem Stueck) sw['laenge']
    lang ist. Rueckgabe wie kette_v2_punkte: Stuetzpunkte im Raum des Ankerknochens A."""
    mr = sw['modellraum']
    f0, nf = mo.clips[mr['clip']]
    kf = mo.frames[f0 + mr['bild'] % nf] & 0xFFF
    V, _N, F = mo.geometrie(kf)
    koerper = sorted({i for f in F if f['bone'] < 16 for i in f['v']})
    K = V[koerper]
    oben, sohle = K[:, 1].min(), K[:, 1].max()
    H = sohle - oben
    xo = K[K[:, 1] <= oben + 0.01 * H, 0].mean()
    R, T = mo.pose(kf)
    Ra, Ta = R[A].astype(float) / 4096.0, T[A].astype(float)
    w0 = np.array([xo - sw['wurzel_h'][0] * H, oben + sw['wurzel_h'][1] * H, 0.0])
    p0 = np.radians(sw['bogen']['start_grad']); th = np.radians(sw['bogen']['dreh_grad'])
    u = np.linspace(0.0, 1.0, 29)
    zs = sw['seitlich']['spitze'] * H * u * u
    r0 = np.array([-np.cos(p0), np.sin(p0), 0.0])            # Startrichtung (hinten = -X, unten = +Y)

    def punkte(lb):
        rho = lb / th
        phi = p0 + th * u
        x = -rho * (np.sin(phi) - np.sin(p0))                # Integral von -cos(phi) ds
        y = rho * (np.cos(p0) - np.cos(phi))                 # Integral von sin(phi) ds
        pf = [w0 - r0 * sw['versenkt']] + [w0 + np.array([xx, yy, zz]) for xx, yy, zz in zip(x, y, zs)]
        return [tuple(Ra.T @ (q - Ta)) for q in pf]

    def laenge(lb):
        return sum(kette_ruhe(dict(sw, punkte=punkte(lb)))['L'])
    lo, hi = 500.0, 6000.0
    for _ in range(40):
        mi = 0.5 * (lo + hi)
        lo, hi = (mi, hi) if laenge(mi) < sw['laenge'] else (lo, mi)
    lb = 0.5 * (lo + hi)
    pk = punkte(lb)
    spitze = np.array(pk[-1]) @ Ra.T + Ta
    return pk, dict(H=H, oben=oben, xo=xo, a=lb, spitze_ueber_sohle_H=float((sohle - spitze[1]) / H))


def kette_meshes_v3(sw, ruhe):
    """Schwanz V3 (Messblatt V3 R29-R32, PSX-Machart): je Glied EIN Kegelstumpf mit sw['seiten'] Seiten von der Manschette
    (-ueberlapp * r0, im Vorglied) bis zum Gliedende (Radius r1), Glied 0 mit Deckel in den Rumpf, letztes Glied mit Spitze
    (sw['dorn'] x Endradius). Kein Gliederprofil (die KI zeigt einen glatten Bogen ohne Glieder). Textur je Glied aus dem
    eigenen Hautfenster uv['fenster'][i] (Umfang auf u, Laenge auf v, wie am EM30-Arm), Haut-CLUT uv['clut'].
    Normalen radial (Laenge 4096), Windung wie haenge_schwanz_an / kette_meshes (Kreuzprodukt gegen die Normale)."""
    S, uv = sw['seiten'], sw['uv']
    out, n = [], sw['glieder']
    # V4 Runde 5 (uv art 'achse'): Bogenlaenge je Gliedanfang und gesamt (ab dem Deckel des Glieds 16, -ueberlapp x Radius 0)
    s_anf = np.r_[0.0, np.cumsum([float(x) for x in ruhe['L']])]
    ue0 = sw['ueberlapp'] * float(ruhe['radien'][0])
    s_ges = ue0 + float(s_anf[-1])
    for i in range(n):
        Li, r0, r1 = ruhe['L'][i], ruhe['radien'][i], ruhe['radien'][i + 1]
        fu0, fv0, fu1, fv1 = uv['fenster'][i]
        ue = sw['ueberlapp'] * r0
        m = dict(v=[], n=[], tf=[], tuv=[], qf=[], quv=[])
        # Gehrung: Ringe an den Gelenken stehen senkrecht zur Winkelhalbierenden der Ruhe-Gelenkwinkel (Gliedrichtung des Nachbarn
        # aus ruhe['lokal']), damit der Umriss am Gelenk in Ruhe geschlossen ist (rechtwinklige Ringe: Kerbe aussen, Ueberstand
        # innen). Auf die Knick-Messung R25 hatte das keinen Einfluss (Probe 35-39 -> 38-42 Grad); deren Ursache war der Wurzelbogen
        # (siehe SCHWANZ_V3 bogen).
        ey = np.array([0.0, 1.0, 0.0])
        t0 = ey if i == 0 else (ruhe['lokal'][i].T @ ey + ey)
        t1 = ey if i == n - 1 else (ey + ruhe['lokal'][i + 1] @ ey)
        ringe = []
        for c0, t, r in ((np.array([0.0, -ue, 0.0]), t0, r0), (np.array([0.0, float(Li), 0.0]), t1, r1)):
            Rt = _min_rot(ey, t / np.linalg.norm(t))
            ring = []
            for k in range(S):
                w = 2 * np.pi * k / S
                q = Rt @ np.array([np.cos(w), 0.0, np.sin(w)])
                ring.append(len(m['v']))
                m['v'].append([int(round(x)) for x in c0 + q * r] + [0])
                m['n'].append([int(round(x)) for x in q * 4096] + [0])
            ringe.append(ring)

        def tx(k, t):
            a = 1.0 - abs(1.0 - 2.0 * k / S) if uv.get('spiegeln') else k / S
            if uv.get('laengs') == 'u':                   # Gliedlaenge auf u, Umfang auf v (Fenster (u_wurzel, v0, u_spitze, v1))
                return (int(round(fu0 + (fu1 - fu0) * t)), int(round(fv0 + (fv1 - fv0) * a)))
            return (int(round(fu0 + (fu1 - fu0) * a)), int(round(fv0 + (fv1 - fv0) * t)))

        def orient(ia, ib, ic):
            P = [np.array(m['v'][j][:3], float) for j in (ia, ib, ic)]
            N = sum(np.array(m['n'][j][:3], float) for j in (ia, ib, ic))
            return np.cross(P[1] - P[0], P[2] - P[0]) @ N < 0
        for k in range(S):
            k2 = (k + 1) % S
            a, b = ringe[0][k], ringe[0][k2]
            c, d = ringe[1][k], ringe[1][k2]
            uvs = [tx(k, 0.0), tx(k + 1, 0.0), tx(k, 1.0), tx(k + 1, 1.0)]
            if not orient(a, b, c):
                a, b, c, d = b, a, d, c
                uvs = [uvs[1], uvs[0], uvs[3], uvs[2]]
            m['qf'].append([a, a, b, b, c, c, d, d])
            m['quv'].append([uvs[0][0], uvs[0][1], uv['clut'], uvs[1][0], uvs[1][1], uv['page'],
                             uvs[2][0], uvs[2][1], 0, uvs[3][0], uvs[3][1], 0])
        kegel = []
        if i == 0 and sw.get('deckel', True):             # Deckel in den Rumpf (wie kette_meshes, Referenz 2)
            kegel.append((ringe[0], [0, int(round(-ue - 0.8 * r0)), 0], [0, -4096, 0], 0.0))
        if i == n - 1:                                    # Spitze
            kegel.append((ringe[1], [0, int(round(Li + sw['dorn'] * r1)), 0], [0, 4096, 0], 1.0))
        for ring, p, nn, t in kegel:
            sv = len(m['v'])
            m['v'].append(p + [0]); m['n'].append(nn + [0])
            for k in range(S):
                a, b = ring[k], ring[(k + 1) % S]
                if not orient(a, b, sv):
                    a, b = b, a
                m['tf'].append([a, a, b, b, sv, sv])
                p0, p1 = tx(k, t), tx(k + 1, t)
                m['tuv'].append([p0[0], p0[1], uv['clut'], p1[0], p1[1], uv['page'],
                                 (p0[0] + p1[0]) // 2, int(round(fv0 + (fv1 - fv0) * 0.5)), 0])
                # Scheitel von Deckel und Spitze auf der Fenstermitte (Umfang 0,5; Runde 1 hatte fuer die Spitze 0,92 ohne Beleg)
        out.append(m)
    return out


def schwanz_textur(tim, p):
    """Runde 41 SF NB3 (Abnahme 1 Mangel 1, GESTALTUNG nach Video 4): eigene Schwanztextur in die freie Flaeche der
    TPage 130 (u 256+u0.., v v0.., von keinem EM30-Mesh benutzt: Seite 130 belegt v 0-167) malen. Je Glied laeuft v
    ueber die ganze Hoehe: dunkles Gelenk -> helles rosa-weisses Band -> rotes Glied mit Laengsstreifen -> dunkles
    Gelenk. Die Indizes zeigen in eine eigene Verlaufszeile (cluts zusatz 'verlauf'), Index 0 bleibt ungenutzt
    (transparent). Struktur (Rauschen) aus der Helligkeit des Wundfleischs (TPage 128 u 66-126 v 4-90, Zeile 480),
    damit die Flaeche nicht glatt wirkt. Vorher: Bandstreifen u 0-42 v 92-130 mit Zweifarb-Zeile 483 -> flaches Rosa."""
    tim = bytearray(tim)
    cx, cy, cw, ch, c, ende = _clut(tim)
    iw, ih = struct.unpack_from('<HH', tim, ende + 8)
    breite = iw * 2
    pix = np.frombuffer(bytes(tim[ende + 12:ende + 12 + breite * ih]), np.uint8).reshape(ih, breite).copy()
    L0 = (_rgb(c[0]) @ np.array([0.299, 0.587, 0.114]))
    quelle = L0[pix[4:90, 66:126]]
    quelle = (quelle - quelle.mean()) / max(quelle.std(), 1e-6)
    u0, v0, bu, bv = 256 + p['u'][0], p['v'][0], p['u'][1] - p['u'][0] + 1, p['v'][1] - p['v'][0] + 1
    assert v0 >= 168 and v0 + bv <= ih and u0 + bu <= breite, 'Schwanztextur ausserhalb der freien Flaeche'
    stuetz_t, stuetz_w = zip(*p['laenge'])
    for y in range(bv):
        t = y / (bv - 1)
        basis = float(np.interp(t, stuetz_t, stuetz_w))
        for x in range(bu):
            streif = p['streifen'] * np.sin(2 * np.pi * x * p['streifen_n'] / bu) if 0.35 < t < 0.88 else 0.0
            rausch = p['rauschen'] * quelle[(y * 3) % quelle.shape[0], (x * 2) % quelle.shape[1]]
            pix[v0 + y, u0 + x] = int(np.clip(round(basis + streif + rausch), 1, 255))
    tim[ende + 12:ende + 12 + breite * ih] = pix.tobytes()
    return bytes(tim)


def emr_mit_kette(emr, sw, ruhe, winkel):
    """EMR-Bank 0 mit N angehaengten Knochen neu schreiben. Knochen 0-15: Bindversaetze, Kinderlisten und
    Keyframe-Bits unveraendert (16 x 36 Bit = 72 Byte, byteweise kopiert); Knochen 16 haengt am Anker,
    16+i am Vorgaenger. winkel[kf] = Liste von N (ax, ay, az) im 12-Bit-Format."""
    s = EA.emr_parse(bytes(emr))
    nb0, n = s.bone_count, sw['glieder']
    nb = nb0 + n
    tab0 = u16(emr, 0)
    kinder = []
    for b in range(nb0):
        cc, co = u16(emr, tab0 + b*4), u16(emr, tab0 + b*4 + 2)
        kinder.append(list(emr[tab0 + co:tab0 + co + cc]) if cc else [])
    kinder[sw['knochen']].append(nb0)
    for i in range(n):
        kinder.append([nb0 + i + 1] if i < n - 1 else [])
    rel = list(s.rel) + list(ruhe['rel'])
    tab = (8 + 6 * nb + 3) & ~3
    kb = bytearray()
    eintraege = []
    for b in range(nb):
        eintraege.append((len(kinder[b]), (4 * nb + len(kb)) if kinder[b] else 0))
        kb += bytes(kinder[b])
    kf_off = (tab + 4 * nb + len(kb) + 3) & ~3
    alt_ks = s.kf_size
    assert (nb0 * 36) % 8 == 0 and alt_ks == 12 + nb0 * 36 // 8, 'Altknochen nicht byteweise kopierbar'
    ks = 12 + (((nb * 36 + 7) // 8 + 3) & ~3)
    out = bytearray(kf_off + ks * s.kf_count)
    struct.pack_into('<4H', out, 0, tab, kf_off, nb, ks)
    for b, (x, y, z) in enumerate(rel):
        struct.pack_into('<3h', out, 8 + b*6, x, y, z)
    for b, (cc, co) in enumerate(eintraege):
        struct.pack_into('<HH', out, tab + b*4, cc, co)
    out[tab + 4*nb:tab + 4*nb + len(kb)] = kb
    for k in range(s.kf_count):
        alt = s.kf_data[k*alt_ks:(k+1)*alt_ks]
        o = kf_off + k*ks
        out[o:o + alt_ks] = alt
        for i, w in enumerate(winkel[k]):
            for j in range(3):
                _bits_schreiben(out, o + 12, (nb0 + i)*36 + j*12, w[j] & 0xFFF)
    return bytes(out)


def _exp(w):
    """Drehvektor -> Matrix (Rodrigues)."""
    t = np.linalg.norm(w)
    if t < 1e-12:
        return np.eye(3)
    k = w / t
    K = np.array([[0, -k[2], k[1]], [k[2], 0, -k[0]], [-k[1], k[0], 0]])
    return np.eye(3) + np.sin(t) * K + (1 - np.cos(t)) * (K @ K)


def _log(R):
    """Matrix -> Drehvektor."""
    c = np.clip((np.trace(R) - 1) / 2, -1.0, 1.0)
    t = np.arccos(c)
    if t < 1e-9:
        return np.zeros(3)
    w = np.array([R[2, 1] - R[1, 2], R[0, 2] - R[2, 0], R[1, 0] - R[0, 1]])
    return w * (t / (2 * np.sin(t))) if t < np.pi - 1e-6 else w / max(np.linalg.norm(w), 1e-9) * t


def schwanz_bewegung(emr, edd, sw, ruhe):
    """Nachschwing-Rechnung je Clip, Gelenk fuer Gelenk im Winkelraum ("Wackelknochen"): jedes Glied hat eine
    Abweichung th (Drehvektor, lokal hinter seiner Ruhedrehung) und eine Winkelgeschwindigkeit om. Dreht sich das
    Vorglied (bzw. die Brust), behaelt das Glied zum Anteil 'traeg' seine Weltlage (Nachlauf), die Feder 'steif'
    zieht es zur Ruhe zurueck, 'daempfung' nimmt die Schwingung heraus, 'schwere' zieht die Glieder nach unten.
    Weil jedes Glied am TATSAECHLICHEN Vorglied haengt, laeuft die Verzoegerung zur Spitze hin auf (Peitsche beim
    Angriff, Schwingen beim Gehen, Folgebewegung beim Werfen). Kreisende Clips (erstes ~ letztes Bild) laufen
    dreimal, gespeichert wird der letzte Umlauf; sonst Start in Ruhe. Rueckgabe: winkel[kf] = N x (ax, ay, az)
    12 Bit; ein Keyframe, den mehrere Bilder nutzen, nimmt das erste Bild."""
    mo = EA.Modell.__new__(EA.Modell)
    mo.skel = EA.emr_parse(bytes(emr))
    clips, frames = EA.edd_parse(bytes(edd))
    n, A = sw['glieder'], sw['knochen']
    dyn = sw['bewegung']
    k = np.linspace(dyn['steif'][0], dyn['steif'][1], n)
    f = np.linspace(dyn['traeg'][0], dyn['traeg'][1], n)
    grenze = np.radians(dyn['grenze'])
    unten = np.array([0.0, 1.0, 0.0])
    winkel = {}
    ruhe_w = [_zerlege(Li) for Li in ruhe['lokal']]
    posen = {}

    rel0 = np.array(ruhe['rel'][0], float)
    rad = ruhe['radien']

    def anker(kf):
        if kf not in posen:
            R, T = mo.pose(kf)
            posen[kf] = (R[A].astype(float) / 4096.0, T[A].astype(float))
        return posen[kf][0]

    def boden(kf, i, P, W, basis):
        """Glied i von P aus; liegt sein Ende unter y = -0,5 Radius (Boden, Y unten), in der Senkrechten
        hochdrehen (schleift auf dem Boden). Rueckgabe: neue Weltdrehung oder None."""
        d = W @ np.array([0.0, 1.0, 0.0])
        Li = ruhe['L'][i]
        grenze_y = -0.5 * rad[i + 1]
        if P[1] + Li * d[1] <= grenze_y:
            return None
        dy = np.clip((grenze_y - P[1]) / Li, -1.0, 1.0)
        h = d - d[1] * unten
        h = h / np.linalg.norm(h) if np.linalg.norm(h) > 1e-6 else np.array([-1.0, 0.0, 0.0])
        d2 = h * np.sqrt(max(0.0, 1 - dy * dy)) + unten * dy
        return _min_rot(d, d2) @ W

    for c, (f0, nf) in enumerate(clips):
        kfs = [frames[f0 + i] & 0xFFF for i in range(nf)]
        kreis = nf > 8 and np.linalg.norm(anker(kfs[0]) - anker(kfs[-1])) < 0.15
        th, om = np.zeros((n, 3)), np.zeros((n, 3))
        welt_alt = None
        for u in range(3 if kreis else 1):
            for kf in kfs:
                W = anker(kf)
                P = W @ rel0 + posen[kf][1]
                welt = []
                for i in range(n):
                    basis = W @ ruhe['lokal'][i]
                    if welt_alt is not None:     # Nachlauf: Anteil f der alten Weltlage behalten
                        th_traeg = _log(basis.T @ welt_alt[i])
                        th[i] = th[i] + f[i] * (th_traeg - th[i])
                    # Schwere: Drehmoment zur Senkrechten, im Gliedraum
                    achse = basis @ _exp(th[i]) @ np.array([0.0, 1.0, 0.0])
                    om[i] = dyn['daempfung'] * om[i] - k[i] * th[i] + dyn['schwere'] * (basis.T @ np.cross(achse, unten))
                    th[i] = th[i] + om[i]
                    t = np.linalg.norm(th[i])
                    if t > grenze:
                        th[i] *= grenze / t; om[i] *= 0.5
                    W = basis @ _exp(th[i])
                    kn = dyn.get('knick')
                    if kn and i > 0:
                        # Knick-Grenze wie schwanz_r41.c glaetten: Gelenkwinkel zum Vorglied hoechstens Ruhe-Gelenkwinkel + rand,
                        # begrenzt auf min..max; sonst in der Ebene beider Glieder auf die Grenze zurueckdrehen
                        y = np.array([0.0, 1.0, 0.0])
                        dv, dn = welt[i - 1] @ y, W @ y
                        ruhe_grad = np.degrees(np.arccos(np.clip((ruhe['lokal'][i] @ y) @ y, -1, 1)))
                        lim = np.radians(np.clip(ruhe_grad + kn['rand'], kn['min'], kn['max']))
                        if kn.get('ab') is not None and ruhe_grad > kn['ab']:
                            # Runde 41 SF V4 (Messblatt V4 Abschnitt 4 Punkt 2): ein Gelenk, dessen Ruhewinkel ueber kn['ab'] liegt
                            # (die Biegung 70-90 Grad), behaelt Ruhewinkel + rand; die 16-Grad-Grenze gilt fuer die uebrigen
                            # (wie schwanz_r41.c glaetten, SW_GLATT_KNICK_AB)
                            lim = np.radians(ruhe_grad + kn['rand'])
                        if float(dv @ dn) < np.cos(lim):
                            sv = dn - dv * float(dv @ dn)
                            sv = sv / np.linalg.norm(sv) if np.linalg.norm(sv) > 1e-9 else np.cross(dv, [0.0, 0.0, 1.0])
                            W = _min_rot(dn, dv * np.cos(lim) + sv * np.sin(lim)) @ W
                            th[i] = _log(basis.T @ W)
                    W2 = boden(kf, i, P, W, basis)
                    if W2 is not None:
                        W = W2
                        th[i] = _log(basis.T @ W)
                        om[i] *= 0.3
                    P = P + W @ np.array([0.0, ruhe['L'][i], 0.0])
                    welt.append(W)
                welt_alt = welt
                if u == (2 if kreis else 0) and kf not in winkel:
                    Wp, w = anker(kf), []
                    for i in range(n):
                        Li = Wp.T @ welt[i]
                        w.append(_zerlege(Li))
                        Wp = Wp @ _quant(Li)
                    winkel[kf] = w
    return [winkel.get(kk, ruhe_w) for kk in range(mo.skel.kf_count)]


def umtexturieren(meshes, regeln):
    """Faces auf ein anderes TIM-Rechteck legen: UVs der gewaehlten Faces linear von ihrer eigenen
    Ausdehnung auf (u, v) des Ziels abbilden, CLUT und TPage setzen. Filter je Regel:
    nur_page = nur Faces dieser TPage; y_ab = nur Faces, deren Vertices alle Mesh-lokal y >= y_ab haben."""
    for r in regeln:
        for b in r['meshes']:
            m = meshes[b]
            wahl = []                     # (Liste, Index, Eckenzahl)
            for art, fl, ul, n in (('t', m['tf'], m['tuv'], 3), ('q', m['qf'], m['quv'], 4)):
                for i, (f, uv) in enumerate(zip(fl, ul)):
                    if 'nur_page' in r and uv[5] != r['nur_page']:
                        continue
                    if 'y_ab' in r and min(m['v'][f[2*k+1]][1] for k in range(n)) < r['y_ab']:
                        continue
                    # Runde 41 SF NB3: weitere Filter (Mesh-lokal, Mittelwert der Ecken bzw. CLUT-Zeile der Flaeche)
                    if 'nur_clut' in r and (uv[2] >> 6) not in r['nur_clut']:
                        continue
                    mx = [sum(m['v'][f[2*k+1]][q] for k in range(n)) / n for q in range(3)]
                    if 'x_ab' in r and mx[0] < r['x_ab']:
                        continue
                    if 'z_max' in r and abs(mx[2]) > r['z_max']:
                        continue
                    if 'my_ab' in r and mx[1] < r['my_ab']:
                        continue
                    if 'my_bis' in r and mx[1] > r['my_bis']:
                        continue
                    if 'mz_bis' in r and mx[2] > r['mz_bis']:      # Runde 41 SF NB4: nur die rechte Seite (z <)
                        continue
                    wahl.append((ul, i, n))
            if not wahl:
                continue
            us = [ul[i][3*k] for ul, i, n in wahl for k in range(n)]
            vs = [ul[i][3*k+1] for ul, i, n in wahl for k in range(n)]
            su, sv = (min(us), max(us)), (min(vs), max(vs))

            def abb(x, s, t):
                return int(round(t[0] + (x - s[0]) * (t[1] - t[0]) / max(1, s[1] - s[0])))
            for ul, i, n in wahl:
                uv = list(ul[i])
                if 'u' in r:
                    for k in range(n):
                        uv[3*k], uv[3*k+1] = abb(uv[3*k], su, r['u']), abb(uv[3*k+1], sv, r['v'])
                uv[2], uv[5] = r['clut'], r.get('page', uv[5])
                ul[i] = uv
    return meshes


def teile_quer(meshes, regeln):
    """Runde 41 SF N1.6 (kurze Hose): Vierecke, die die Ebene y = schnitt (Mesh-lokal) kreuzen, an dieser Ebene in zwei
    Vierecke teilen. Die Form bleibt gleich (neue Ecken liegen auf den alten Kanten, Normalen und UVs linear dazwischen);
    danach kann umtexturieren(y_ab=schnitt) den unteren Teil allein umlegen. EM30-Viereck = (0,1,3)+(0,3,2): Kanten 0-1,
    1-3, 3-2, 2-0."""
    for r in regeln:
        y_s = r['schnitt']
        for b in r['meshes']:
            m = meshes[b]
            neu_f, neu_uv = [], []
            for f, uv in zip(m['qf'], m['quv']):
                vi, ni = f[1::2], f[0::2]
                y = [m['v'][j][1] for j in vi]
                oben = [k for k in range(4) if y[k] < y_s]
                if len(oben) != 2 or min(y) >= y_s or max(y) <= y_s:
                    neu_f.append(f); neu_uv.append(uv); continue
                if sorted(oben) in ([0, 1], [2, 3]):
                    kanten = [(0, 2), (1, 3)]           # Schnitt auf den Kanten 0-2 und 1-3
                elif sorted(oben) in ([0, 2], [1, 3]):
                    kanten = [(0, 1), (2, 3)]
                else:
                    raise AssertionError('Mesh %d: Viereck mit diagonaler Lage zur Schnittebene' % b)
                neue = {}
                for a, c in kanten:
                    t = (y_s - y[a]) / float(y[c] - y[a])
                    P = [int(round(m['v'][vi[a]][q] + t * (m['v'][vi[c]][q] - m['v'][vi[a]][q]))) for q in range(3)]
                    P[1] = y_s
                    N = np.array(m['n'][ni[a]][:3], float) * (1 - t) + np.array(m['n'][ni[c]][:3], float) * t
                    N = N / max(np.linalg.norm(N), 1e-9) * 4096
                    m['v'].append(P + [0]); m['n'].append([int(round(x)) for x in N] + [0])
                    U = (int(round(uv[3*a] + t * (uv[3*c] - uv[3*a]))), int(round(uv[3*a+1] + t * (uv[3*c+1] - uv[3*a+1]))))
                    neue[(a, c)] = (len(m['n']) - 1, len(m['v']) - 1, U)
                def ecke(k):
                    return (ni[k], vi[k], (uv[3*k], uv[3*k+1]))
                (a0, c0), (a1, c1) = kanten
                if kanten == [(0, 2), (1, 3)]:
                    teil = [[ecke(0), ecke(1), neue[(0, 2)], neue[(1, 3)]], [neue[(0, 2)], neue[(1, 3)], ecke(2), ecke(3)]]
                else:
                    teil = [[ecke(0), neue[(0, 1)], ecke(2), neue[(2, 3)]], [neue[(0, 1)], ecke(1), neue[(2, 3)], ecke(3)]]
                for q in teil:
                    neu_f.append([x for e in q for x in (e[0], e[1])])
                    neu_uv.append([q[0][2][0], q[0][2][1], uv[2], q[1][2][0], q[1][2][1], uv[5],
                                   q[2][2][0], q[2][2][1], 0, q[3][2][0], q[3][2][1], 0])
            m['qf'], m['quv'] = neu_f, neu_uv
    return meshes


# ═══════════════════════════════════════════════════ TIM: CLUT-Zeilen (Runde 41 SF N1.4-N1.7)
def _clut(tim):
    cs, cx, cy, cw, ch = struct.unpack_from('<I4H', tim, 8)
    c = np.array(struct.unpack_from('<%dH' % (cw * ch), tim, 20), dtype=np.int64).reshape(ch, cw)
    return cx, cy, cw, ch, c, 8 + cs


def _rgb(c):
    return np.stack([(c & 31) * 8, ((c >> 5) & 31) * 8, ((c >> 10) & 31) * 8], -1).astype(float)


def _pack(rgb, alt):
    q = np.clip(np.rint(rgb / 8.0), 0, 31).astype(np.int64)
    w = q[..., 0] | (q[..., 1] << 5) | (q[..., 2] << 10) | (alt & 0x8000)
    w = np.where(alt == 0, 0, np.where(w == 0, 1, w))      # 0 bleibt transparent, nichts wird neu transparent
    return w


def haut_zeile(c, p):
    """Haut dunkler und roter gefleckt (Video 4, N1.4): je Eintrag Rotgrad q = r/g; Blaues (b > r, Jeans) bleibt.
    blass = alt * dunkel; rot = (r*rot_hell, g*rot_hell*satt, b*rot_hell*satt); Mischung w = linear in q zwischen q0..q1."""
    rgb = _rgb(c); r, g, b = rgb[..., 0], rgb[..., 1], rgb[..., 2]
    q = r / np.maximum(g, 8.0)
    w = np.clip((q - p['q0']) / (p['q1'] - p['q0']), 0, 1)[..., None]
    blass = rgb * p['dunkel']
    rot = np.stack([r * p['rot_hell'], g * p['rot_hell'] * p['satt'], b * p['rot_hell'] * p['satt']], -1)
    neu = blass * (1 - w) + rot * w
    neu = np.where((b > r)[..., None], rgb, neu)
    return _pack(neu, c)


def band_zeile(c, p):
    """Zweifarbige Zeile nach Helligkeit des Texels: dunkle Eintraege -> p['dunkel'], helle -> p['hell'] (linear
    zwischen den Helligkeits-Quantilen l0..l1 der Zeile). Farben = Videomessung (N1.5 Schwanz, N1.7 Sichel)."""
    rgb = _rgb(c); L = rgb @ np.array([0.299, 0.587, 0.114])
    w = np.clip((L - p['l0']) / (p['l1'] - p['l0']), 0, 1)[..., None]
    neu = np.array(p['dunkel'], float) * (1 - w) + np.array(p['hell'], float) * w
    return _pack(neu, c)


def zusatz_zeile(orig, neu, zz):
    """Eine Zusatzzeile: 'verlauf' = eigene Farbtreppe ueber die Indizes (Index 0 transparent, Runde 41 SF NB3
    Schwanztextur); 'faktor' = Kopie der UMGEFAERBTEN Zeile zz['aus'] mal Faktor je Kanal (NB3 Lichtzonen: Rumpf/Beine
    dunkler, Adern entsaettigt); sonst band_zeile aus der Originalzeile (N1.5-N1.10)."""
    if 'tabelle' in zz:                             # Runde 41 SF V3: feste Eintraege (Index, RGB), Rest schwarz/transparent
        rgb = np.zeros((256, 3)); alt = np.zeros(256, np.int64)
        for i, c in zz['tabelle']:
            rgb[i] = c; alt[i] = 0x7FFF
        return _pack(rgb, alt)
    if 'verlauf' in zz:
        idx, farben = zip(*zz['verlauf'])
        k = np.arange(256)
        rgb = np.stack([np.interp(k, idx, [f[j] for f in farben]) for j in range(3)], -1)
        alt = np.full(256, 0x7FFF, np.int64); alt[0] = 0
        return _pack(rgb, alt)
    if 'faktor' in zz:
        quelle = _rgb(neu[zz['aus']])
        rgb = quelle * np.array(zz['faktor'], float)
        if 'blau_faktor' in zz:                   # Jeans (b > r) nur maessig dunkler (Video h124: Jeans deutlich blau)
            rgb = np.where((quelle[..., 2] > quelle[..., 0])[..., None], quelle * zz['blau_faktor'], rgb)
        if 'entsaettigen' in zz:                  # Rot zur Helligkeit ziehen (Adern), Anteil je Eintrag nach r/g
            L = rgb @ np.array([0.299, 0.587, 0.114])
            q = rgb[..., 0] / np.maximum(rgb[..., 1], 8.0)
            w = np.clip((q - 1.25) / 0.6, 0, 1)[..., None] * zz['entsaettigen']
            rgb = rgb * (1 - w) + np.stack([L * 1.25, L * 0.92, L * 0.9], -1) * w
        if 'blau_faktor' in zz:
            rgb = np.where((quelle[..., 2] > quelle[..., 0])[..., None], quelle * zz['blau_faktor'], rgb)
        return _pack(rgb, neu[zz['aus']])
    return band_zeile(orig[zz['aus']], zz)


def cluts_bauen(tim, cl):
    """Hautzeilen umfaerben und Zusatzzeilen (aus einer Quellzeile abgeleitet) anhaengen. Die Texel bleiben; nur der
    CLUT-Block waechst (h 3 -> 3 + n), Pixelblock unveraendert dahinter."""
    cx, cy, cw, ch, orig, ende = _clut(tim)
    c = orig.copy()
    for z in cl.get('haut_zeilen', ()):
        c[z] = haut_zeile(orig[z], cl['haut'])
        if cl['haut'].get('entsaettigen'):      # Runde 41 SF NB3: Adern entsaettigen (wie zusatz_zeile), Jeans bleiben
            rgb = _rgb(c[z]); L = rgb @ np.array([0.299, 0.587, 0.114])
            q = rgb[..., 0] / np.maximum(rgb[..., 1], 8.0)
            w = np.clip((q - 1.25) / 0.6, 0, 1)[..., None] * cl['haut']['entsaettigen']
            neu = rgb * (1 - w) + np.stack([L * 1.25, L * 0.92, L * 0.9], -1) * w
            neu = np.where((rgb[..., 2] > rgb[..., 0])[..., None], rgb, neu)
            c[z] = _pack(neu, orig[z])
        if 'hell' in cl['haut'] or 'kontrast' in cl['haut']:   # Runde 41 SF NB4: Helligkeit und Aderkontrast, Jeans bleiben
            rgb = _rgb(c[z]); jeans = (rgb[..., 2] > rgb[..., 0]) | (c[z] == 0)
            haut = ~jeans
            mitte = rgb[haut].mean(0) if haut.any() else np.zeros(3)
            k = cl['haut'].get('kontrast', 1.0)
            neu = (rgb * k + mitte * (1 - k)) * cl['haut'].get('hell', 1.0)
            li = cl['haut'].get('lichter')
            if li and haut.any():                 # Runde 41 SF NB5: hellste Hauteintraege -> rosa-weisses Licht
                L = neu @ np.array([0.299, 0.587, 0.114])
                lq = np.quantile(L[haut], li['q']); lm = L[haut].max()
                w = np.clip((L - lq) / max(lm - lq, 1.0), 0, 1)[..., None] * li['staerke']
                neu = neu * (1 - w) + np.array(li['farbe'], float) * w
            neu = np.where(jeans[..., None], rgb, neu)
            c[z] = _pack(neu, orig[z])
    extra = [zusatz_zeile(orig, c, zz) for zz in cl.get('zusatz', ())]
    alle = np.vstack([c] + [e[None] for e in extra]).astype('<u2')
    blk = struct.pack('<I4H', 12 + alle.size * 2, cx, cy, cw, alle.shape[0]) + alle.tobytes()
    return bytes(tim[:8]) + blk + bytes(tim[ende:])


# ═══════════════════════════════════════════════════ EMR: Bindoffsets mitziehen
def passe_emr_an(emr, skal, extra):
    """rel[b] liegt im Raum des Eltern-Knochens -> mit dessen Mesh-Skalierung mitziehen."""
    emr = bytearray(emr)
    s = EA.emr_parse(bytes(emr))
    for b in range(s.bone_count):
        p = s.parent[b]
        x, y, z = s.rel[b]
        if p >= 0 and p in skal:
            sx, sy, sz = skal[p]
            x, y, z = x*sx, y*sy, z*sz
        dx, dy, dz = extra.get(b, (0, 0, 0))
        x, y, z = (int(round(c)) for c in (x + dx, y + dy, z + dz))
        struct.pack_into('<3h', emr, 8 + b*6, x, y, z)
    return bytes(emr)


# ═══════════════════════════════════════════════════ TIM: CLUTs einfaerben
def faerbe_tim(tim, tint):
    """15-Bit-CLUT-Eintraege umfaerben. 0x0000 (PSX: voll transparent) und das STP-Bit
    bleiben erhalten; ein Eintrag, der durch das Umfaerben 0 wuerde, bleibt mindestens 1."""
    if tint is None:
        return tim
    mat, hell = tint
    tim = bytearray(tim)
    cs  = u32(tim, 8)
    cw, ch = u16(tim, 16), u16(tim, 18)
    for k in range(cw * ch):
        o = 20 + k*2
        c = u16(tim, o)
        if c == 0:
            continue
        rgb = np.array([c & 31, (c >> 5) & 31, (c >> 10) & 31], dtype=np.float64)
        neu = np.clip(np.rint(mat @ rgb * hell), 0, 31).astype(int)
        w = int(neu[0]) | (int(neu[1]) << 5) | (int(neu[2]) << 10) | (c & 0x8000)
        if w == 0:
            w = 1
        struct.pack_into('<H', tim, o, w)
    assert 20 + cw*ch*2 <= 8 + cs
    return bytes(tim)


# ═══════════════════════════════════════════════════ Zusammenbau
# ═══════════════════════════════════════════════════ EMR: Keyframes umhalten
def _rz(grad):
    a = np.radians(grad)
    return np.array([[np.cos(a), -np.sin(a), 0], [np.sin(a), np.cos(a), 0], [0, 0, 1.0]])


def _zerlege(m):
    """Umkehrung von EA.mat_euler (RotMatrix @0x80068130): m[0][2]=sy, m[1][2]=-cy*sx,
    m[2][2]=cy*cx, m[0][0]=cz*cy, m[0][1]=-sz*cy. Roundtrip-Fehler <= 1/4096 Umdrehung."""
    k = 4096 / (2*np.pi)
    ay = np.arcsin(np.clip(m[0, 2], -1, 1))
    ax = np.arctan2(-m[1, 2], m[2, 2])
    az = np.arctan2(-m[0, 1], m[0, 0])
    return [int(round(a*k)) & 0xFFF for a in (ax, ay, az)]


def _bits_schreiben(buf, start, bitoff, wert):
    for i in range(12):
        bi = start + ((bitoff + i) >> 3)
        mask = 1 << ((bitoff + i) & 7)
        buf[bi] = (buf[bi] | mask) if (wert >> i) & 1 else (buf[bi] & ~mask)


def _welt(skel, rel, lokal, wurzel):
    n = skel.bone_count
    R, T = [None]*n, [None]*n
    for b in range(n):
        p = skel.parent[b]
        if p < 0:
            R[b], T[b] = lokal[b], np.array(wurzel, float)
        else:
            R[b] = R[p] @ lokal[b]
            T[b] = R[p] @ np.array(rel[b], float) + T[p]
    return R, T


def _rx(grad):
    a = np.radians(grad)
    return np.array([[1, 0, 0], [0, np.cos(a), -np.sin(a)], [0, np.sin(a), np.cos(a)]])


def _beine_ik(H, A, L1, L2, pol):
    """Zwei-Glied-IK: Huefte H, Ziel A (Fussgelenk), Laengen L1/L2, Knie zur Seite pol. Rueckgabe Knie, Ziel (geklemmt)."""
    d_vec = A - H
    d = float(np.linalg.norm(d_vec))
    d_kl = float(np.clip(d, abs(L1 - L2) + 1.0, L1 + L2 - 1.0))
    u = d_vec / max(d, 1e-9)
    A = H + u * d_kl
    a = (L1 * L1 - L2 * L2 + d_kl * d_kl) / (2 * d_kl)
    h = np.sqrt(max(0.0, L1 * L1 - a * a))
    w = pol - (pol @ u) * u
    w = w / max(np.linalg.norm(w), 1e-9)
    return H + u * a + w * h, A


def ducken(emr, p):
    """Runde 41 SF Nachbesserung 2 (GESTALTUNG nach Video 4, keine RE-Werte): geduckte, breitbeinige Haltung fuer ALLE
    Keyframes der EMR-Bank 0 (Stand, Gang, Angriffe, Griff/Wurf; die Baenke 1/2 sind Opfer-Clips des Spielers und bleiben).
    Je Keyframe, gewichtet mit der Aufrichtung des Rumpfs a = R7[1,1] (Anteil w = (a - 0,35)/0,45 in [0,1]; liegend -> 0,
    damit Niederschlag/Tod/Aufstehen nicht durch den Boden knicken):
      Becken     um becken*w tiefer (Wurzel-Y des Keyframes, Y zeigt nach unten)
      Beine      Fussgelenke behalten ihre Lage im Modellraum (Gangbild, Bodenkontakt) und ruecken je spreiz*w nach aussen
                 (Beckenquerachse waagerecht); Oberschenkel/Schienbein per Zwei-Glied-IK, Knie zur Seite des alten Knies
                 + knie_aussen x Aussenrichtung; Fuss behaelt seine Weltdrehung (flach wie im Clip)
      Rumpf      Bauch (7) um rumpf*w Grad, Brust (8) um brust*w Grad nach vorn (Drehung um Z im Elternraum)
      Kopf       (9) um kopf*w Grad (Z, im Brustraum)
      Arme       Oberarme (10, 13) behalten zum Anteil arm_halten ihre alte Weltdrehung (haengen trotz vorgebeugtem Rumpf)
                 und ruecken um arm_innen*w Grad zur Koerpermitte (Drehung um X im Brustraum)."""
    emr = bytearray(emr)
    s = EA.emr_parse(bytes(emr))
    nb, ks, kf_off = s.bone_count, s.kf_size, u16(emr, 2)
    rel = [np.array(r, float) for r in s.rel]
    L1, L2 = float(np.linalg.norm(rel[2])), float(np.linalg.norm(rel[3]))
    beine = ((1, 2, 3, -1.0), (4, 5, 6, 1.0))         # Knochen 1-3 rechts (rel z -229), 4-6 links
    for k in range(s.kf_count):
        o = kf_off + k * ks
        sk = EA.Skel(); sk.kf_size, sk.kf_data = ks, bytes(emr[o:o + ks])
        lokal = [EA.mat_euler(*EA.kf_angles(sk, 0, b)).astype(float) / 4096 for b in range(nb)]
        wurzel = np.array(EA.kf_pos(sk, 0), float)
        # Schwanz V2 / Koerper V2: laengere Beine (skal 1/2/4/5) verlaengern auch die Bezugslage der Fussgelenke (T0 mit den neuen
        # Bindversaetzen): ohne Anheben stand der Koerper V2 420 E. im Boden (Sohle Clip 0 y +427, Clip 1 +404..+504 statt -8..+95).
        hoch = p.get('wurzel_hoch', 0.0)
        wurzel[1] -= hoch
        R0, T0 = _welt(s, rel, lokal, wurzel)
        w = float(np.clip((R0[7][1, 1] - 0.35) / 0.45, 0.0, 1.0))
        if w <= 0.0:
            if hoch:
                struct.pack_into('<h', emr, o + 2, int(max(-32768, min(32767, round(wurzel[1])))))
            continue
        neu = list(lokal)
        wurzel1 = wurzel + np.array([0.0, p['becken'] * w, 0.0])
        # Rumpf, Kopf
        neu[7] = _rz(p['rumpf'] * w) @ neu[7]
        neu[8] = _rz(p['brust'] * w) @ neu[8]
        neu[9] = _rz(p.get('kopf', 0.0) * w) @ neu[9]
        # Beine
        quer = R0[0][:, 2].copy(); quer[1] = 0.0
        quer = quer / max(np.linalg.norm(quer), 1e-9)
        vorn = R0[0][:, 0].copy()
        R1, T1 = _welt(s, rel, neu, wurzel1)
        for hb, kb, fb, seite in beine:
            H = T1[hb]
            A = T0[fb] + quer * seite * p['spreiz'] * w
            u0 = T0[fb] - T0[hb]; u0 = u0 / max(np.linalg.norm(u0), 1e-9)
            pol0 = (T0[kb] - T0[hb]) - ((T0[kb] - T0[hb]) @ u0) * u0
            pol0 = pol0 / np.linalg.norm(pol0) if np.linalg.norm(pol0) > 1e-3 * L1 else vorn
            pol = pol0 + quer * seite * p['knie_aussen'] * w
            K, A = _beine_ik(H, A, L1, L2, pol)
            ey = np.array([0.0, 1.0, 0.0])
            Wo = _min_rot(R0[hb] @ ey, K - H) @ R0[hb]
            Wu = _min_rot(R0[kb] @ ey, A - K) @ R0[kb]
            neu[hb] = R1[0].T @ Wo
            neu[kb] = Wo.T @ Wu
            neu[fb] = Wu.T @ R0[fb]
        # Arme: Weltdrehung zum Anteil arm_halten behalten, dann zur Mitte
        R1, T1 = _welt(s, rel, neu, wurzel1)
        for ab, innen in ((10, -1.0), (13, 1.0)):     # 10 links (rel z +), 13 rechts (rel z -)
            folgt = R1[8] @ lokal[ab]
            alt = R0[8] @ lokal[ab]
            W = folgt @ _exp(p['arm_halten'] * _log(folgt.T @ alt))
            wi = p.get('arm_innen_links', p['arm_innen']) if ab == 10 else p['arm_innen']   # NB4: Klauenarm eigener Wert
            neu[ab] = _rx(innen * wi * w) @ (R1[8].T @ W)
        # Runde 41 SF NB3 (Abnahme 1 Mangel 2): Klauenarm im Ellbogen nach vorn beugen, damit die (verkleinerte) Klaue
        # auf Hoehe des Oberschenkels endet (Video h124/h132 Handende y ~355-365 statt Knoechel). GESTALTUNG.
        knochen = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 13]
        if p.get('ellbogen_links'):
            neu[11] = _rz(-p['ellbogen_links'] * w) @ lokal[11]
            knochen.append(11)
        struct.pack_into('<h', emr, o + 2, int(max(-32768, min(32767, round(wurzel1[1])))))
        for b in knochen:
            for i, wi in enumerate(_zerlege(neu[b])):
                _bits_schreiben(emr, o + 12, b * 36 + i * 12, wi)
    return bytes(emr)


def _tiefster_punkt(R, T, verts):
    """Groesstes Y (Y zeigt nach unten) ueber alle Mesh-Vertices = Bodenkontakt."""
    return max(float((v @ R[b].T + T[b])[:, 1].max()) for b, v in verts.items())


def umhalten(vor, d, md1, haltung):
    """Alle Keyframes aller drei Baenke (EMR dir[2]/[4]/[6]) umhalten. Arbeitet auf den
    bereits angepassten Bindoffsets und der skalierten MD1, damit der Bodenkontakt zur
    fertigen Geometrie passt."""
    struktur = EA.emr_parse(bytes(vor[d[2]:d[3]]))
    rel = struktur.rel
    ks  = struktur.kf_size
    verts = {}
    for b, m in enumerate(EA.md1_parse(md1)[:struktur.bone_count]):
        if m.tv:
            verts[b] = np.array([v[:3] for v in m.tv], float)
    gang = _gang_plan(vor, d, struktur, haltung.get('gang'))
    for a, e in ((d[2], d[3]), (d[4], d[5]), (d[6], d[7])):
        kf_off = u16(vor, a + 2)
        n = (e - a - kf_off) // ks
        for k in range(n):
            o = a + kf_off + k*ks
            sk = EA.Skel(); sk.kf_size, sk.kf_data = ks, bytes(vor[o:o+ks])
            winkel = [EA.kf_angles(sk, 0, b) for b in range(struktur.bone_count)]
            wurzel = list(EA.kf_pos(sk, 0))
            lokal  = [EA.mat_euler(*w).astype(float) / 4096 for w in winkel]
            R0, T0 = _welt(struktur, rel, lokal, wurzel)
            # Anteil der Haltung = Aufrichtung des Rumpfs im Original (Y-Achse von Knochen 7
            # gegen die Senkrechte): aufrecht -> voll, liegend (Niederschlag/Tod) -> nichts,
            # sonst knickt ein schon liegender Rumpf um weitere 80 Grad durch.
            f = float(np.clip(R0[7][1, 1], 0.0, 1.0))
            neu = list(lokal)
            for b, g in haltung['kippen'].items():
                neu[b] = _rz(g * f) @ lokal[b]
            # Arme: Weltorientierung wie vorher -> haengen senkrecht vom gekippten Rumpf
            R1, _ = _welt(struktur, rel, neu, wurzel)
            for b in haltung['welt_halten']:
                p = struktur.parent[b]
                neu[b] = R1[p].T @ R0[p] @ lokal[b]
            for b, g in haltung['nachkippen'].items():
                neu[b] = _rz(g * f) @ neu[b]
            geaendert = set(haltung['kippen']) | set(haltung['welt_halten']) | set(haltung['nachkippen'])
            if a == d[2] and k in gang:
                # Vorderbein-Schritt: Zusatzdrehung um die Welt-Seitenachse Z (Laufrichtung +X),
                # danach Ellbogen-Knick in der Vorschwung-Phase.
                for arm, (zusatz, knick) in gang[k].items():
                    R1, _ = _welt(struktur, rel, neu, wurzel)
                    p = struktur.parent[arm]
                    neu[arm] = R1[p].T @ _rz(zusatz) @ R1[arm]
                    ell = haltung['gang']['arme'][arm]
                    neu[ell] = _rz(knick) @ neu[ell]
                    geaendert |= {arm, ell}
            R1, T1 = _welt(struktur, rel, neu, wurzel)
            # Bodenkontakt: im Lauf an den Hinterfuessen (sonst hoebe eine zurueckschwingende
            # Klaue den ganzen Koerper an), sonst am tiefsten Punkt des Modells.
            boden = verts
            if a == d[2] and k in gang:
                boden = {b: verts[b] for b in haltung['gang']['fuesse'] if b in verts}
            wurzel[1] += int(round(_tiefster_punkt(R0, T0, boden) - _tiefster_punkt(R1, T1, boden)))
            struct.pack_into('<h', vor, o + 2, max(-32768, min(32767, wurzel[1])))
            for b in geaendert:
                for i, w in enumerate(_zerlege(neu[b])):
                    _bits_schreiben(vor, o + 12, b*36 + i*12, w)


def _gang_plan(vor, d, struktur, gang):
    """Je Keyframe der Laufclips (Bank 0) und je Vorderbein: (Zusatzwinkel, Ellbogenknick) in
    Grad. Schwung p = Neigung der Original-Armachse in der Laufebene (atan2(x, y), Y = unten);
    der Zusatz verstaerkt die Abweichung vom Clip-Mittel, der Knick folgt der Schwung-
    Geschwindigkeit (nur vorwaerts, normiert auf das Clip-Maximum)."""
    if not gang:
        return {}
    clips, frames = EA.edd_parse(bytes(vor[d[1]:d[2]]))
    a = d[2]
    kf_off, ks = u16(vor, a + 2), struktur.kf_size
    plan = {}
    for c in gang['clips']:
        if c >= len(clips):
            continue
        f0, n = clips[c]
        kfs = [frames[f0 + i] & 0xFFF for i in range(n)]
        for arm in gang['arme']:
            pw = []
            for kf in kfs:
                o = a + kf_off + kf*ks
                sk = EA.Skel(); sk.kf_size, sk.kf_data = ks, bytes(vor[o:o+ks])
                lokal = [EA.mat_euler(*EA.kf_angles(sk, 0, b)).astype(float) / 4096
                         for b in range(struktur.bone_count)]
                R, _ = _welt(struktur, struktur.rel, lokal, EA.kf_pos(sk, 0))
                v = R[arm] @ np.array([0.0, 1.0, 0.0])
                pw.append(np.degrees(np.arctan2(v[0], v[1])))
            pw = np.array(pw)
            mitte = pw.mean()
            ges = np.roll(pw, -1) - np.roll(pw, 1)
            gmax = max(ges.max(), 1e-6)
            for i, kf in enumerate(kfs):
                zusatz = -(gang['faktor'] - 1.0) * (pw[i] - mitte)
                knick = gang['anheben'] * max(0.0, ges[i]) / gmax
                plan.setdefault(kf, {})[arm] = (zusatz, knick)
    return plan


def lade_em30():
    ems = open(EA.EMS_PFAD, 'rb').read()
    off, ln = EA.ems_entry(ems, EA.EMS_ORDER.index(TYP))
    return ems[off:off+ln]


def baue(blob, form):
    """Blob = [u32 dir_off][u32 n=9][Sektionen dir[0..8]][Verzeichnis n*4]. MD1 waechst durch den Schwanz,
    mit Schwanzkette auch die EMR-Bank 0 (dir[2]); alle folgenden Sektionen verschieben sich, das Verzeichnis
    wird aus den Sektionslaengen neu gerechnet."""
    d = EA.emd_dir(blob)
    dir_off, n = u32(blob, 0), u32(blob, 4)
    assert dir_off + n*4 == len(blob) and d[0] == 8 and n == 9
    # Verzeichnis EM30: dir[2]=EMR Bank 0 (Struktur), dir[7]=MD1, dir[8]=TIM
    emr_a, emr_e = d[2], d[3]
    md1_a, md1_e = d[7], d[8]
    vor = bytearray(blob[:md1_a])
    vor[emr_a:emr_e] = passe_emr_an(blob[emr_a:emr_e], form['skal'], form['rel'])
    md1 = skaliere_md1(blob[md1_a:md1_e], form['skal'])
    if form.get('haltung'):
        umhalten(vor, d, md1, form['haltung'])
    emr0 = bytes(vor[emr_a:emr_e])
    if form.get('ducken'):
        emr0 = ducken(emr0, form['ducken'])
    kette = form.get('schwanz_kette')
    if kette and kette.get('pfad_h'):
        # Schwanz V2: Stuetzpunkte aus den Formblatt-Masszahlen im Modellraum der fertigen Haltung (nach ducken); keine
        # Rueckdrehung wie unten, der Pfad ist schon in der geduckten Pose gemessen.
        mo = EA.Modell.__new__(EA.Modell)
        mo.skel = EA.emr_parse(emr0)
        mo.clips, mo.frames = EA.edd_parse(bytes(blob[d[1]:d[2]]))
        mo.meshes = EA.md1_parse(md1)
        pk, info = kette_v2_punkte(kette, mo, kette['knochen'])
        kette = dict(kette, punkte=pk)
        BAU_INFO['schwanz_v2'] = info
    elif kette and kette.get('gelenke'):
        # Schwanz V4 (Messblatt V4): Gelenke direkt aus den Bildmassen der Vorlage 2 (kette_v4_punkte), Modellraum der fertigen Haltung
        mo = EA.Modell.__new__(EA.Modell)
        mo.skel = EA.emr_parse(emr0)
        mo.clips, mo.frames = EA.edd_parse(bytes(blob[d[1]:d[2]]))
        mo.meshes = EA.md1_parse(md1)
        pk, rad, info = kette_v4_punkte(kette, mo, kette['knochen'])
        kette = dict(kette, punkte=pk, radien_v4=rad)
        BAU_INFO['schwanz_v2'] = info
        BAU_INFO['mo_v4'] = mo
    elif kette and kette.get('bogen'):
        # Schwanz V3 (Messblatt V3): glatter Bogen statt Haken mit Seitenwellen, Modellraum der fertigen Haltung wie V2.
        mo = EA.Modell.__new__(EA.Modell)
        mo.skel = EA.emr_parse(emr0)
        mo.clips, mo.frames = EA.edd_parse(bytes(blob[d[1]:d[2]]))
        mo.meshes = EA.md1_parse(md1)
        pk, info = kette_v3_punkte(kette, mo, kette['knochen'])
        kette = dict(kette, punkte=pk)
        BAU_INFO['schwanz_v2'] = info
    elif kette and form.get('ducken'):
        # Runde 41 SF N2: die Ruhegestalt des Schwanzes haengt im Brustraum; Rumpf+Brust kippen um (rumpf+brust) Grad nach
        # vorn -> Ruhelinie um denselben Winkel zurueckdrehen (um den Ansatz), damit er in der Welt haengt wie vorher.
        g = -(form['ducken']['rumpf'] + form['ducken']['brust'])
        p0 = np.array(kette['punkte'][0], float)
        kette = dict(kette, punkte=[tuple(p0 + _rz(g) @ (np.array(q, float) - p0)) for q in kette['punkte']])
    if form.get('schwanz') or form.get('umtexturieren') or kette:
        meshes = md1_lesen(md1)
        if form.get('teilen'):
            teile_quer(meshes, form['teilen'])
        if form.get('umtexturieren'):
            umtexturieren(meshes, form['umtexturieren'])
        if form.get('schwanz'):
            haenge_schwanz_an(meshes, form['schwanz'])
        if kette:
            # Mesh 16 (Hand mit Rohr, ohne Knochen) entfaellt: Mesh 16.. = Glieder der Kette (Mesh i == Knochen i).
            # Der Port tauscht Teil 15 nur bei genau 16 Knochen gegen Mesh 16 (main.c bk_rohr).
            ruhe = kette_ruhe_v4(kette['punkte'], kette['radien_v4']) if kette.get('radien_v4') is not None else kette_ruhe(kette)
            BAU_INFO['ruhe'] = ruhe
            if form.get('kopf_v2'):
                kopf_v2_uv(meshes, form['kopf_v2'])
            if form.get('kopf_v4'):
                kopf_v4_form_uv(meshes, form['kopf_v4'], BAU_INFO['mo_v4'])
            if form.get('koerper_zeilen_v4'):
                koerper_zeilen_v4(meshes, form['koerper_zeilen_v4'])
            if 'baender' in kette['uv']:                   # Schwanz V4: Farbklassen je Flaeche in der Ruhe-Pose
                mo4 = BAU_INFO['mo_v4']
                R, T = mo4.pose(_v4_kf(mo4, kette['pose']))
                W, P, welt = np.eye(3), np.array(ruhe['rel'][0], float), []
                for i in range(kette['glieder']):
                    W = W @ ruhe['lokal'][i]
                    welt.append((W.copy(), P.copy()))
                    P = P + W @ np.array([0.0, ruhe['L'][i], 0.0])
                # Bogenmitte (Ankerraum) = Kruemmungsmitte der Biegung: halber Weg vom Biegegelenk (Gelenk 3) zur Innenecke des L, das der
                # Schwanz nach Messblatt V4 Abschnitt 4 bildet (Ansatz waagerecht bis zur Biegung, dann senkrecht bis zum Knick): Innenecke
                # = x/z von Gelenk 2 (Mitte des waagerechten Stuecks), y von Gelenk 5 (Knick). Fuer das waagerechte Stueck zeigt "aussen"
                # damit nach oben, fuer das senkrechte nach hinten. ebene_n = Normale der Bogenebene (Gelenke 1, 3, 5): die Richtung
                # Bogenmitte -> Glied wird in diese Ebene gelegt (V4 Runde 2, der Ansatz liegt 'tiefe' vor der Flanke).
                bm = np.array([ruhe['p'][2][0], ruhe['p'][5][1], ruhe['p'][2][2]])
                bm = ruhe['p'][3] + 0.5 * (bm - ruhe['p'][3])
                en = np.cross(ruhe['p'][3] - ruhe['p'][1], ruhe['p'][5] - ruhe['p'][3])
                en = en / max(np.linalg.norm(en), 1e-9)
                meshes = meshes[:EA.emr_parse(emr0).bone_count] + kette_meshes_v4(kette, ruhe, welt, bm, en)
            elif 'fenster' in kette['uv']:                   # Schwanz V3: Kegelstuempfe mit Hautfenstern
                meshes = meshes[:EA.emr_parse(emr0).bone_count] + kette_meshes_v3(kette, ruhe)
            else:
                meshes = meshes[:EA.emr_parse(emr0).bone_count] + kette_meshes(kette, ruhe, meshes)
            emr0 = emr_mit_kette(emr0, kette, ruhe, schwanz_bewegung(emr0, blob[d[1]:d[2]], kette, ruhe))
        md1 = md1_schreiben(meshes)
    tim = faerbe_tim(blob[md1_e:dir_off], form['tint'])
    if form.get('cluts'):
        tim = cluts_bauen(tim, form['cluts'])
    if form.get('schwanz_textur'):
        tim = schwanz_textur(tim, form['schwanz_textur'])
    if form.get('kopf_v2') and not form.get('kopf_v3'):     # V3 malt nach dem Bau (kopf_v3_malen)
        tim = kopf_v2_textur(tim, form['kopf_v2'])
    if form.get('kopf_v4') and V4_TEXTUR == 'ref2':         # V4 Originaltexturen: nur Kopf-/Schwanztexel, CLUT = Referenz 2
        tim = v4_ref2tex_einsetzen(tim, form['kopf_v4'], form['schwanz_kette'])
    elif form.get('kopf_v4'):                               # V4 Runde 1-5: Texel/CLUT aus der Eichung (v4_farben_einsetzen)
        tim = v4_farben_einsetzen(tim, form['kopf_v4'], form['schwanz_kette'], form.get('koerper_zeilen_v4'))
    if len(tim) % 4:
        tim += bytes(4 - len(tim) % 4)
    sek = [bytes(vor[d[i]:d[i+1]]) for i in range(7)]
    sek[2] = emr0
    sek += [md1, tim]
    neu_dir, o = [], 8
    for x in sek:
        assert len(x) % 4 == 0
        neu_dir.append(o)
        o += len(x)
    neu = b''.join(sek)
    kopf = struct.pack('<II', 8 + len(neu), n)
    return kopf + neu + struct.pack('<%dI' % n, *neu_dir)


BAU_INFO = {}


# ═══════════════════════════════════════════════════ Kopf V2 (Runde 41 SF "Schwanz V2", Nutzer-Entscheid F4 2026-11-06)
# Nutzer 2026-11-06 zu F4: "ja, das soll mit uebernommen werden aus der KI Fassung, das soll so sein." und "natuerlich in der
# grafischen Qualitaet des spieles." -> PORT-WAHL nach Nutzer-Entscheid, Quelle KI-Fassung (kein Original-Beleg):
#   offenes Maul mit Zaehnen, ein Auge oben am Kopf, ein Auge auf der rechten Schulter (W).
# Original bleibt gueltig: helle Kuppe oben, rote Zone darunter (Formblatt V2 K1/K2).
# Machart wie die Originalmodelle: Maul, Zaehne und Kopfauge sind TEXTUR auf der vorhandenen Kopfgeometrie (Mesh 9, EM30-
# Dreiecke unveraendert), 8 bpp im TIM-Raster, Texeldichte wie die Haut; das Schulterauge ist EIN kleines Viereck (zwei
# Dreiecke) auf der Schulter. Farben: eigene CLUT-Zeile aus Hautton/Rot der vorhandenen Zeilen (keine Leuchtfarben).

def kopf_v2_uv(meshes, p):
    """Mesh 9: die der Kamera zugewandten Flaechen (Vorderansicht h118, Blickrichtung p['blick'] im Kopfraum) bekommen das
    Kopfbild per Parallelprojektion aus dieser Ansicht (u, v = p['abbildung'] @ (x, y, z, 1)); die abgewandten behalten das
    Wundfleisch (Zeile 485). So liegt das Bild dort, wo Original und KI es von vorn zeigen, ohne Spiegelbild hinten.
    Dazu das Schulterauge: ein Viereck (zwei Dreiecke) mit den Ecken p['schulterauge']['ecken'] im Brustraum (Mesh 8)."""
    m = meshes[9]
    A = np.array(p['abbildung'], float)
    d = np.array(p['blick'], float); d /= np.linalg.norm(d)
    u0, v0, u1, v1 = p['u'][0], p['v'][0], p['u'][1], p['v'][1]
    for fl, ul, n in ((m['tf'], m['tuv'], 3), (m['qf'], m['quv'], 4)):
        for i, f in enumerate(fl):
            P3 = [np.array(m['v'][f[2 * k + 1]][:3], float) for k in range(n)]
            N = sum(np.array(m['n'][f[2 * k]][:3], float) for k in range(n))
            if N @ d > -p.get('schwelle', 0.15) * np.linalg.norm(N):      # nicht zur Kamera: Wundfleisch behalten
                continue
            uv = list(ul[i])
            for k in range(n):
                q = A @ np.r_[P3[k], 1.0]
                uv[3 * k] = int(round(np.clip(q[0], u0, u1)))
                uv[3 * k + 1] = int(round(np.clip(q[1], v0, v1)))
            uv[2], uv[5] = p['clut'] << 6, p['page']
            ul[i] = uv
    su = p.get('seite_uv')
    if su:
        # Runde 2 (V3): Seitenflaechen (nicht im Kopfbild vorn) -> Seitenbild. Kamera-Seite: Normale . d <= -schwelle; Gegenseite:
        # Normale . d' <= -schwelle mit d' = d an der Ebene z = z_spiegel gespiegelt, UV vom gespiegelten Punkt (gleiche Texel).
        As = np.array(su['A'], float)
        ds = np.array(su['d'], float); ds /= np.linalg.norm(ds)
        dm = ds * np.array([1.0, 1.0, -1.0])
        zc = float(su['z_spiegel'])
        for fl, ul, n in ((m['tf'], m['tuv'], 3), (m['qf'], m['quv'], 4)):
            for i, f in enumerate(fl):
                N = sum(np.array(m['n'][f[2 * k]][:3], float) for k in range(n))
                N /= max(np.linalg.norm(N), 1e-9)
                if N @ d <= -p.get('schwelle', 0.15):          # Kopfbild vorn (oben)
                    continue
                P3 = [np.array(m['v'][f[2 * k + 1]][:3], float) for k in range(n)]
                if N @ ds <= -su['schwelle'] and N @ ds <= N @ dm:
                    spiegel = False
                elif N @ dm <= -su['schwelle']:
                    spiegel = True
                else:
                    continue
                uv = list(ul[i])
                for k in range(n):
                    q = P3[k].copy()
                    if spiegel:
                        q[2] = 2 * zc - q[2]
                    t = As @ np.r_[q, 1.0]
                    uv[3 * k] = int(round(np.clip(t[0], su['u'][0], su['u'][1])))
                    uv[3 * k + 1] = int(round(np.clip(t[1], su['v'][0], su['v'][1])))
                uv[2], uv[5] = p['clut'] << 6, p['page']
                ul[i] = uv
    a = p.get('schulterauge')
    if a:
        m8 = meshes[8]
        E = [np.array(e, float) for e in a['ecken']]           # Reihenfolge: oben links, oben rechts, unten links, unten rechts
        nrm = np.array(a['normale'], float); nrm /= np.linalg.norm(nrm)
        b0, n0 = len(m8['v']), len(m8['n'])
        for e in E:
            m8['v'].append([int(round(x)) for x in e + nrm * a.get('abstand_uv', a.get('abstand', 12.0))] + [0])
            m8['n'].append([int(round(x)) for x in nrm * 4096] + [0])
        ua, va = a['u'], a['v']
        uvs = [(ua[0], va[0]), (ua[1], va[0]), (ua[0], va[1]), (ua[1], va[1])]
        ia, ib, ic, idd = b0, b0 + 1, b0 + 2, b0 + 3
        P = [np.array(m8['v'][j][:3], float) for j in (ia, ib, ic)]
        fl = np.array(a.get('flaeche', nrm), float)          # V3: Windung nach der Flaechenrichtung, Licht nach a['normale']
        if np.cross(P[1] - P[0], P[2] - P[0]) @ fl >= 0:      # Windung wie EM30: Kreuzprodukt gegen die Normale
            ia, ib, ic, idd = ib, ia, idd, ic
            uvs = [uvs[1], uvs[0], uvs[3], uvs[2]]
        m8['qf'].append([ia - b0 + n0, ia, ib - b0 + n0, ib, ic - b0 + n0, ic, idd - b0 + n0, idd])
        m8['quv'].append([uvs[0][0], uvs[0][1], a.get('clut', p['clut']) << 6, uvs[1][0], uvs[1][1], p['page'],
                          uvs[2][0], uvs[2][1], 0, uvs[3][0], uvs[3][1], 0])
    return meshes


def kopf_v2_textur(tim, p):
    """Kopfbild in eine freie TIM-Flaeche malen (Indizes in die Zeile p['clut'], Verlauf dunkel -> rot -> Haut -> hell).
    Die Flaeche ist die Vorderansicht des Kopfs (h118, kopf_v2_uv), Anteile x/y = 0..1 der Kopfbreite/-hoehe im Bild:
    helle Kuppe oben (Original K1/K2), Hautstruktur aus dem Wundfleisch-Rauschen, rote Zone um und unter dem Maul (K2),
    offenes senkrechtes Maul mit Lippe, Zahnreihe und dunklem Inneren (KI, F4), rundes Auge oben (KI, F4); daneben das
    Feld des Schulterauges."""
    tim = bytearray(tim)
    cx, cy, cw, ch, c, ende = _clut(tim)
    iw, ih = struct.unpack_from('<HH', tim, ende + 8)
    breite = iw * 2
    pix = np.frombuffer(bytes(tim[ende + 12:ende + 12 + breite * ih]), np.uint8).reshape(ih, breite).copy()
    L0 = (_rgb(c[0]) @ np.array([0.299, 0.587, 0.114]))
    quelle = L0[pix[4:90, 66:126]]
    quelle = (quelle - quelle.mean()) / max(quelle.std(), 1e-6)
    pb = 128 * (p['page'] & 0xF)
    def flaeche(u, v):
        return pb + u[0], v[0], u[1] - u[0] + 1, v[1] - v[0] + 1
    I = p['index']
    u0, v0, bu, bv = flaeche(p['u'], p['v'])
    assert u0 + bu <= breite and v0 + bv <= ih
    yy, xx = np.mgrid[0:bv, 0:bu]
    ty, tx = yy / (bv - 1.0), xx / (bu - 1.0)
    rausch = p['rauschen'] * quelle[(yy * 3) % quelle.shape[0], (xx * 2) % quelle.shape[1]]
    img = np.interp(ty, [0.0, p['kuppe'], p['kuppe'] + 0.18, 1.0], [I['kuppe'], I['kuppe'], I['haut'], I['haut']]) + rausch
    mu = p['maul']
    mx, my = (mu['x'][0] + mu['x'][1]) / 2.0, (mu['y'][0] + mu['y'][1]) / 2.0
    rx, ry = (mu['x'][1] - mu['x'][0]) / 2.0, (mu['y'][1] - mu['y'][0]) / 2.0
    # rote Zone (K2): um das Maul und darunter bis zum Kopfende, seitlich weich
    rz = np.clip(1.0 - (np.abs(tx - mx) - rx * 1.25) / 0.12, 0, 1) * np.clip((ty - (mu['y'][0] - 0.10)) / 0.10, 0, 1)
    img = img * (1 - rz) + (I['rot'] + rausch * 0.8) * rz
    e = np.sqrt(((tx - mx) / rx) ** 2 + ((ty - my) / ry) ** 2)
    img[e <= 1.0] = I['lippe'] + rausch[e <= 1.0] * 0.5
    w = np.arctan2((ty - my) / ry, (tx - mx) / rx)
    zahn = (e <= mu['zahn_aussen']) & (e > mu['zahn_innen']) & (np.abs(np.sin(w)) > mu.get('zahn_bogen', 0.0))
    luecke = (np.floor((w + np.pi) / (2 * np.pi) * mu['zaehne']) % 2) == 1
    img[zahn & ~luecke] = I['zahn']
    img[zahn & luecke] = I['zahnschatten']
    img[(e <= mu['zahn_aussen']) & ~zahn & (e > mu['zahn_innen'])] = I['innen'] + 20     # Mundwinkel: dunkelrot
    img[e <= mu['zahn_innen']] = I['innen']
    au = p['auge']
    ea = np.sqrt(((xx - au['x'] * (bu - 1)) / au['r'][0]) ** 2 + ((yy - au['y'] * (bv - 1)) / au['r'][1]) ** 2)
    img[ea <= 1.35] = I['innen'] + 25
    img[ea <= 1.0] = I['weiss']
    img[ea <= 0.45] = I['innen']
    pix[v0:v0 + bv, u0:u0 + bu] = np.clip(np.round(img), 1, 255).astype(np.uint8)
    sa = p.get('schulterauge')
    if sa:
        su0, sv0, sbu, sbv = flaeche(sa['u'], sa['v'])
        yy, xx = np.mgrid[0:sbv, 0:sbu]
        es = np.sqrt(((xx - (sbu - 1) / 2.0) / (sbu / 2.0)) ** 2 + ((yy - (sbv - 1) / 2.0) / (sbv / 2.0)) ** 2)
        f = np.full((sbv, sbu), float(I['haut']))
        f[es <= 0.95] = I['innen'] + 25
        f[es <= 0.72] = I['weiss']
        f[es <= 0.30] = I['innen']
        pix[sv0:sv0 + sbv, su0:su0 + sbu] = np.clip(np.round(f), 1, 255).astype(np.uint8)
    tim[ende + 12:ende + 12 + breite * ih] = pix.tobytes()
    return bytes(tim)


# ═══════════════════════════════════════════════════ Kopf V3: Messansicht, Kopfbild, Schulterauge (Messblatt V3)
def nah_raster(mo, p):
    """Messansicht "Kopf vorn nah" (Messblatt V3 Abschnitt 8, P_port_kopf.nah): Pose p['clip']/p['bild'], Mitte = T[9] +
    p['mitte'], Blick von vorn (+X), rechts = +Z (= linke Seite (W)), unten = +Y, Massstab p['massstab'] px/E.
    Rasterung wie EA.render (Tiefenpuffer), je Bildpunkt: Face-Index (-1 leer), Knochen, Atlas-u/v (Gleitkomma, u inkl. TPage),
    Modellpunkt. Rueckgabe dict(F, fi, knochen, u, v, P, V, N, R, T)."""
    import birkin_buehne as BB
    kf = BB.kf_von_clip(mo, p['clip'], p['bild'])
    V, N, F = mo.geometrie(kf)
    R, T = mo.pose(kf)
    mitte = T[9].astype(float) + np.array(p['mitte'], float)
    Vm = V - mitte
    W, H, sc, cx, cy = p['breite'], p['hoehe'], p['massstab'], p['cx'], p['cy']
    # Runde 2: p['achsen'] = (rechts, unten, zur Kamera) im Modellraum (Seitenansicht aus der Spielkamera); ohne = Messansicht vorn
    ri, un, de = (np.array(x, float) for x in p.get('achsen', ((0, 0, 1), (0, 1, 0), (1, 0, 0))))
    sx, sy, sz = Vm @ ri * sc + cx, Vm @ un * sc + cy, Vm @ de
    fi = np.full((H, W), -1, np.int32)
    zb = np.full((H, W), -1e18)
    uu = np.zeros((H, W)); vv = np.zeros((H, W)); P = np.zeros((H, W, 3))
    for k, f in enumerate(F):
        i0, i1, i2 = f['v']
        x = np.array([sx[i0], sx[i1], sx[i2]]); y = np.array([sy[i0], sy[i1], sy[i2]]); z = np.array([sz[i0], sz[i1], sz[i2]])
        area = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
        if abs(area) < 1e-9:
            continue
        x0, x1 = max(int(np.floor(x.min())), 0), min(int(np.ceil(x.max())) + 1, W)
        y0, y1 = max(int(np.floor(y.min())), 0), min(int(np.ceil(y.max())) + 1, H)
        if x0 >= x1 or y0 >= y1:
            continue
        PX, PY = np.meshgrid(np.arange(x0, x1) + 0.5, np.arange(y0, y1) + 0.5)
        w0 = ((x[1] - x[0]) * (PY - y[0]) - (PX - x[0]) * (y[1] - y[0])) / area
        w1 = ((PX - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (PY - y[0])) / area
        w2 = 1.0 - w0 - w1
        innen = (w0 >= -1e-9) & (w1 >= -1e-9) & (w2 >= -1e-9)
        zz = w2 * z[0] + w1 * z[1] + w0 * z[2]
        sub = zb[y0:y1, x0:x1]
        hit = innen & (zz > sub)
        if not hit.any():
            continue
        (a0, b0), (a1, b1), (a2, b2) = f['uv']
        ub = (f['page'] & 0xF) * 128
        sub[hit] = zz[hit]
        fi[y0:y1, x0:x1][hit] = k
        uu[y0:y1, x0:x1][hit] = (w2 * (a0 + ub) + w1 * (a1 + ub) + w0 * (a2 + ub))[hit]
        vv[y0:y1, x0:x1][hit] = (w2 * b0 + w1 * b1 + w0 * b2)[hit]
        for j in range(3):
            P[y0:y1, x0:x1, j][hit] = (w2 * V[i0, j] + w1 * V[i1, j] + w0 * V[i2, j])[hit]
    kn = np.array([f['bone'] for f in F] + [-1])[fi]
    return dict(F=F, fi=fi, knochen=kn, u=uu, v=vv, P=P, V=V, N=N, R=R, T=T, kf=kf, mitte=mitte)


def kopf_v3_anker(r, p):
    """Lage von Kopfauge und Maul in der Messansicht (px), aus der Kopfbild-Flaeche auf Mesh 9 (KOPF_V3 u/v)."""
    Hp = p['nah']['H_px']
    pb = 128 * (p['page'] & 0xF)
    bild = (r['knochen'] == 9) & (r['u'] >= pb + p['u'][0]) & (r['u'] < pb + p['u'][1] + 1) & (r['v'] >= p['v'][0])
    ys = np.nonzero(bild.any(1))[0]
    oben, unten = int(ys.min()), int(ys.max())
    ay = oben + p['auge_y_anteil'] * (unten - oben)
    my = ay + p['maul']['dy_auge'] * Hp
    zeile = np.nonzero(bild[int(round(my))])[0]
    mx = 0.5 * (zeile.min() + zeile.max())
    kopfbreite = float(zeile.max() - zeile.min() + 1)
    ax = mx + p['auge']['dx_maul'] * Hp
    return dict(auge=(ax, ay), maul=(mx, my), kopfbreite_px=kopfbreite, oben=oben, unten=unten, maske=bild)


def _texel_lagen(r, u0, v0, bu, bv, maske):
    """Mittlere Bildlage (px) je Texel der Flaeche (u0..u0+bu-1, v0..v0+bv-1) in der Messansicht; nicht sichtbare Texel
    bekommen die Lage des naechsten sichtbaren Texels plus den Texelabstand mal mittlerem Bildschritt je Texel."""
    ys, xs = np.nonzero(maske)
    tu = np.floor(r['u'][ys, xs]).astype(int) - u0
    tv = np.floor(r['v'][ys, xs]).astype(int) - v0
    ok = (tu >= 0) & (tu < bu) & (tv >= 0) & (tv < bv)
    sx = np.zeros((bv, bu)); sy = np.zeros((bv, bu)); n = np.zeros((bv, bu))
    np.add.at(sx, (tv[ok], tu[ok]), xs[ok] + 0.5)
    np.add.at(sy, (tv[ok], tu[ok]), ys[ok] + 0.5)
    np.add.at(n, (tv[ok], tu[ok]), 1)
    hat = n > 0
    sx[hat] /= n[hat]; sy[hat] /= n[hat]
    hv, hu = np.nonzero(hat)
    du = np.median(np.abs(np.diff(sx, axis=1))[hat[:, 1:] & hat[:, :-1]]) if hat.sum() > 2 else 8.0
    dv = np.median(np.abs(np.diff(sy, axis=0))[hat[1:] & hat[:-1]]) if hat.sum() > 2 else 8.0
    for v, u in zip(*np.nonzero(~hat)):
        k = np.argmin((hv - v) ** 2 + (hu - u) ** 2)
        sx[v, u] = sx[hv[k], hu[k]] + (u - hu[k]) * du
        sy[v, u] = sy[hv[k], hu[k]] + (v - hv[k]) * dv
    return sx, sy, hat


def _lab(rgb):
    """sRGB 0..255 -> CIE-Lab (D65), wie tools/r41_sichel/v3/P_farbe.lab."""
    c = np.asarray(rgb, float) / 255.0
    c = np.where(c <= 0.04045, c / 12.92, ((c + 0.055) / 1.055) ** 2.4)
    M = np.array([[0.4124564, 0.3575761, 0.1804375], [0.2126729, 0.7151522, 0.0721750], [0.0193339, 0.1191920, 0.9503041]])
    x = c @ M.T / np.array([0.95047, 1.0, 1.08883])
    d = 6.0 / 29.0
    f = np.where(x > d ** 3, np.cbrt(x), x / (3 * d * d) + 4.0 / 29.0)
    return np.stack([116 * f[..., 1] - 16, 500 * (f[..., 0] - f[..., 1]), 200 * (f[..., 1] - f[..., 2])], -1)


def _rampe(rng, t):
    a, b = rng
    return np.clip(np.round(a + t * (b - a)), a, b).astype(int)


def _zone_t(rausch, zt):
    """Rampenlage je Abtastpunkt: Mitte zt[0] + KI-Streuung zt[1] x Hautrauschen (Std 1), siehe KOPF_V3 zonen."""
    return zt[0] + zt[1] * rausch


def kopf_v3_bild(X, Y, an, p, rausch):
    """Index der Zeile 486 je Abtastpunkt (X, Y in px der Messansicht). Zonen nach Messblatt V3 K1-K15 und Abschnitt 6,
    Runde 2 nach n2_ki_kopf (KOPF_V3 Kommentar)."""
    Hp, I = p['nah']['H_px'], KOPF_V3_INDEX
    ax, ay = an['auge']
    mx, my = an['maul']
    z, au, mu = p['zonen'], p['auge'], p['maul']
    # Grundhaut: helle Kuppe bis an die Oberlippe (KOPF_V3 zonen)
    idx = _rampe(I['kuppe'], _zone_t(rausch, z['kuppe_t']))
    mb = min(mu['b'] * Hp, mu['max_kopfbreite'] * an['kopfbreite_px'])
    mh = mu['h'] * Hp
    # rote Zone (K15: Maul + Kinn) ab der Maul-Oberkante bis zum Kinn
    tab = dict(KOPF_V3_ZEILE['tabelle'])
    dL = float(_lab(np.array(tab[I['rot'][1]], float))[0] - _lab(np.array(tab[I['rot'][0]], float))[0])
    rot = _rampe(I['rot'], z['rot_mitte'] + z['rot_std_L'] / dL * rausch)
    idx = np.where(Y >= my - mh / 2, rot, idx)
    # Maul: Umriss |x|^form + y^2 <= 1 = Lippe (KI dunkelrot, Glanz), darin die Oeffnung (Umriss um die Lippe kleiner, gleiche
    # Form) innen dunkelrot (Messblatt innen) mit Spaltkern |y| <= spalt der Oeffnung (KI-Anteil)
    ex = (X - mx) / (mb / 2)
    ey = (Y - my) / (mh / 2)
    aussen = np.abs(ex) ** mu['form'] + ey ** 2 <= 1.0
    lp = mu['lippe'] * Hp
    oh, ob = mh / 2 - lp, mb / 2                                        # halbe Hoehe / Breite der Oeffnung (Lippe nur oben/unten:
                                                                        # KI-Mundwinkel offen bis zu den Eckzaehnen x 165-167 / 208-212)
    eo_y = (Y - my) / oh
    innen = np.abs((X - mx) / ob) ** mu['form'] + eo_y ** 2 <= 1.0
    idx = np.where(aussen, I['lippe'], idx)
    idx = np.where(innen, I['maul'], idx)
    idx = np.where(innen & (np.abs(eo_y) <= mu['spalt']), I['maul_tief'], idx)
    # Glanz auf der Lippenmitte (KI 1,4-1,9 % des Bands)
    for gxs, sg in ((mu['glanz_x'][0], -1), (mu['glanz_x'][1], 1)):
        for gx in gxs:
            ry = np.sqrt(max(0.0, 1.0 - abs(gx) ** mu['form']))
            g = np.hypot(X - (mx + gx * mb / 2), Y - (my + sg * (ry * mh / 2 - lp / 2))) <= mu['glanz_r'] * Hp
            idx = np.where(g & aussen & ~innen, I['lippe_glanz'], idx)
    # Zaehne: stumpfe Stummel (Quadrat zahn_b x zahn_l), mit zahn_ueber ihrer Laenge ueber dem Rand der Oeffnung
    zb, zl = mu['zahn_b'] * Hp, mu['zahn_l'] * Hp
    for n_z, oben in ((mu['zaehne'][0], True), (mu['zaehne'][1], False)):
        for xk in mx + np.linspace(-0.5, 0.5, n_z) * 2 * ob * mu['zahn_bereich']:
            r_k = np.sqrt(max(0.0, 1.0 - abs((xk - mx) / ob) ** mu['form'])) * oh
            d_in = (Y - (my - r_k)) if oben else ((my + r_k) - Y)          # Abstand vom Rand der Oeffnung ins Innere
            zahn = (np.abs(X - xk) <= zb / 2) & (d_in >= -mu['zahn_ueber'] * zl) & (d_in <= (1 - mu['zahn_ueber']) * zl)
            idx = np.where(zahn, I['zahn'], idx)
    # Auge (Mandel = Ellipse b x h): Lidrand, Auge, Pupille, ein heller Kern
    ea = ((X - ax) / (au['b'] * Hp / 2)) ** 2 + ((Y - ay) / (au['h'] * Hp / 2)) ** 2
    el = ((X - ax) / (au['b'] * Hp / 2 + au['lid'] * Hp)) ** 2 + ((Y - ay) / (au['h'] * Hp / 2 + au['lid'] * Hp)) ** 2
    ep = ((X - ax) / (au['b'] * Hp / 2 * au['pupille'][0])) ** 2 + ((Y - ay) / (au['h'] * Hp / 2 * au['pupille'][1])) ** 2
    gx = ax + (au['glanz'][0] - 0.5) * au['b'] * Hp
    gy = ay + (au['glanz'][1] - 0.5) * au['h'] * Hp
    eg = ((X - gx) / (au['glanz'][2] * Hp)) ** 2 + ((Y - gy) / (au['glanz'][3] * Hp)) ** 2
    idx = np.where(el <= 1.0, I['lid'], idx)
    idx = np.where(ea <= 1.0, I['auge'], idx)
    idx = np.where(ep <= 1.0, I['pupille'], idx)
    idx = np.where(eg <= 1.0, I['glanz'], idx)
    return idx


def kopf_v3_seite_bild(X, Y, an, p, rausch):
    """Index der Zeile 486 je Abtastpunkt der Seitenansicht (px). an: auge/maul = projizierte Lage von Kopfauge und Mundwinkel,
    kante(y) = Vorderkante des Kopfs in der Zeile y, vor = +1/-1 (Bildrichtung nach vorn). Masse = KI birkin12 (KOPF_V3 seite)."""
    Hp, I, s = p['nah']['H_px'], KOPF_V3_INDEX, p['seite']
    z = p['zonen']
    ax, ay = an['auge']
    my = an['maul'][1]
    vor = an['vor']
    hinter = (an['kante'](Y) - X) * vor / Hp          # Abstand hinter der Vorderkante (H)
    # Grundhaut: bis zur Unterkante des Augenprofils helle Schnauzenhaut (KI oben), darunter dunklere (KI unten). PORT-WAHL: KI-Auge (birkin12 y 123) sitzt an der Grenze hell/dunkel
    # (oben y 100-118, unten y 119-128); Probe r2b mit der Grenze in der Augenzeile: das Auge lag halb auf der im Port-Licht dunklen
    # Unterseite und war in der Seitenkamera nicht zu sehen (n2_seite_mass.py: kein dunkler Fleck an der Vorderkante).
    yg = ay + s['auge_h'] / 2 * Hp
    idx = np.where(Y < yg, _rampe(I['seite_oben'], _zone_t(rausch, s['kuppe_t'])), _rampe(I['seite_unten'], _zone_t(rausch, s['haut_t'])))
    # Kinn/rote Zone unter dem Maulprofil wie vorn
    tab = dict(KOPF_V3_ZEILE['tabelle'])
    dL = float(_lab(np.array(tab[I['rot'][1]], float))[0] - _lab(np.array(tab[I['rot'][0]], float))[0])
    rot = _rampe(I['rot'], z['rot_mitte'] + z['rot_std_L'] / dL * rausch)
    mh = s['maul_h'] * Hp
    idx = np.where(Y >= my + mh / 2, rot, idx)
    # Maulprofil: vom Mundwinkel des Kopfbilds (an['maul'], projiziert) bis maul_hinter hinter der Vorderkante, Hoehe maul_h um die
    # Mundwinkelzeile (PORT-WAHL: an den Mundwinkel angeschlossen statt KI 0,044-0,101 H hinter der Spitze / 0,006-0,034 H unter dem
    # Auge - der Port-Mund liegt 0,052 H unter dem Auge (K5), die Kopfbild-Flaechen reichen von vorn bis an den Mundwinkel)
    h0 = (an['kante'](np.array([my]))[0] - an['maul'][0]) * vor / Hp
    band = (hinter >= h0) & (hinter <= s['maul_hinter']) & (np.abs(Y - my) <= mh / 2)
    # Maulprofil rot (KI a* > 15) ohne schwarzen Spalt (Probe r2i mit dem Spaltkern vorn: in der Seitenkamera eine schwarze Linie
    # quer ueber den Kopf, n2_seite_mass.py Zeilen 210-211 L* 6), Stummel in der Bandmitte (KI: helle Stummel im roten Zahnfleisch)
    idx = np.where(band, I['seitenmaul'], idx)
    zb = p['maul']['zahn_b'] * Hp
    for hk in h0 + np.linspace(0.15, 0.85, s['zaehne']) * (s['maul_hinter'] - h0):
        zahn = (np.abs(hinter - hk) * Hp <= zb / 2) & (np.abs(Y - my) <= p['maul']['zahn_l'] * Hp / 2)
        idx = np.where(zahn & band, I['seitenzahn'], idx)
    # Augenprofil: das Kopfauge (Kopfbild vorn) ist von der Seite an der Vorderkante sichtbar, aber verkuerzt; die Seitenflaeche
    # setzt es nach hinten fort: dunkle Mandel auge_b x auge_h (KI), deren Vorderende in der Mitte des projizierten Kopfauges liegt
    # (PORT-WAHL: KI-Auge 0,034 H hinter der Schnauzenspitze; der Port-Kopf hat keine Schnauze vor dem Auge)
    cx = ax - vor * s['auge_b'] / 2 * Hp
    ea = ((X - cx) / (s['auge_b'] * Hp / 2)) ** 2 + ((Y - ay) / (s['auge_h'] * Hp / 2)) ** 2
    vv = an.get('von_vorn') if s.get('auge_nur_seitlich', True) else None
    idx = np.where((ea <= 1.0) & ~(vv if vv is not None else np.zeros_like(ea, bool)), I['seitenauge'], idx)
    return idx


def _rausch_feld(tim_pix, pal0, shape):
    """Hautstruktur: Helligkeit des EM30-Wundfleischs (TPage 128 u 66-126 v 4-90, Zeile 480), normiert (Std 1), Texel 1:1
    (Runde 2: ohne die unbelegte Streckung y x 3 / x x 2 der Runde 1)."""
    L0 = pal0 @ np.array([0.299, 0.587, 0.114])
    q = L0[tim_pix[4:90, 66:126]]
    q = (q - q.mean()) / max(q.std(), 1e-6)
    # auf +-2 Std begrenzt: die Rauschgewichte sind KI-Standardabweichungen (Normalannahme, 95 % in +-2 Std); die dunklen Adern der
    # Wundfleischquelle (bis -4 Std) erzeugten sonst Flecken, die R1 als zweites Auge zaehlte (Probe r2e, Fleck (719,396) L* 11,6)
    q = np.clip(q, -2.0, 2.0)
    yy, xx = np.mgrid[0:shape[0], 0:shape[1]]
    return q[yy % q.shape[0], xx % q.shape[1]]


def kamera_achsen(raum, schnitt, gier):
    """(rechts, unten, zur Kamera) der Spielkamera raum/schnitt im Modellraum einer Figur mit Gier gier (wie b2.zeichne)."""
    import birkin_buehne as BB
    s = BB.Raum(raum).schnitt(schnitt)
    kam = BB.Kamera(s['pos'], s['ziel'], s['fov'] >> 7)
    sn, cs = EA.sin_q12(gier) / 4096.0, EA.cos_q12(gier) / 4096.0
    Y = np.array([[cs, 0, sn], [0, 1, 0], [-sn, 0, cs]])
    Mm = (kam.V.astype(float) / 4096.0) @ Y
    return Mm[0], Mm[1], -Mm[2]


def seite_nah(p):
    """Seitenansicht des Kopfs (nah_raster-Parameter): Spielkamera p['seite'], Mitte T[9], Massstab wie die Messansicht vorn."""
    s = p['seite']
    return dict(clip=s['clip'], bild=s['bild'], mitte=(0.0, 0.0, 0.0), massstab=s['massstab'], H_px=p['nah']['H_px'],
                breite=s['breite'], hoehe=s['hoehe'], cx=s['cx'], cy=s['cy'],
                achsen=kamera_achsen(s['raum'], s['schnitt'], s['gier']))


def _lokal(r, px, py):
    """Modellpunkt unter dem Bildpunkt (px, py) einer nah_raster-Ansicht im Kopfraum (Knochen 9)."""
    iy, ix = int(round(py)), int(round(px))
    R9, T9 = r['R'][9].astype(float) / 4096.0, r['T'][9].astype(float)
    return R9.T @ (r['P'][iy, ix] - T9)


def _ins_bild(r, p_nah, q):
    """Kopfraum-Punkt q -> Bildpunkt der nah_raster-Ansicht r (Parameter p_nah)."""
    R9, T9 = r['R'][9].astype(float) / 4096.0, r['T'][9].astype(float)
    ri, un, _de = (np.array(x, float) for x in p_nah.get('achsen', ((0, 0, 1), (0, 1, 0), (1, 0, 0))))
    w = R9 @ q + T9 - r['mitte']
    return w @ ri * p_nah['massstab'] + p_nah['cx'], w @ un * p_nah['massstab'] + p_nah['cy']


def seite_abbildung(blob, p):
    """Runde 2: Abbildung Kopfraum -> Texel (u, v) des Seitenbilds = Parallelprojektion der Seitenansicht (seite_nah), Texeldichte
    p['seite']['dichte'] je E.; Spiegelebene z = Kopfauge (Kopfraum). Die Lage wird so gelegt, dass alle Seitenflaechen (beide
    Seiten) in die Flaeche u/v passen."""
    mo = modell_aus_blob(blob)
    s = p['seite']
    rv = nah_raster(mo, p['nah'])
    an = kopf_v3_anker(rv, p)
    auge = _lokal(rv, *an['auge'])
    pn = seite_nah(p)
    kf = __import__('birkin_buehne').kf_von_clip(mo, pn['clip'], pn['bild'])
    R, T = mo.pose(kf)
    R9 = R[9].astype(float) / 4096.0
    ri, un, de = (np.array(x, float) for x in pn['achsen'])
    a_u, a_v = R9.T @ ri * s['dichte'], R9.T @ un * s['dichte']       # Texel je E. entlang Bild-rechts / Bild-unten
    d = -(R9.T @ de); d /= np.linalg.norm(d)                            # Blickrichtung im Kopfraum
    dm = d * np.array([1.0, 1.0, -1.0])
    dv = np.array(p['blick'], float); dv /= np.linalg.norm(dv)
    m = mo.meshes[9]
    pts = []
    for verts, norms, faces in ((m.tv, m.tn, m.tf), (m.qv, m.qn, m.qf)):
        for f in faces:
            n = len(f) // 2
            N = sum(np.array(norms[f[2 * k]][:3], float) for k in range(n)); N /= max(np.linalg.norm(N), 1e-9)
            if N @ dv <= -p['schwelle']:
                continue
            for k in range(n):
                q = np.array(verts[f[2 * k + 1]][:3], float)
                if N @ d <= -s['schwelle'] and N @ d <= N @ dm:
                    pts.append(q)
                elif N @ dm <= -s['schwelle']:
                    pts.append(np.array([q[0], q[1], 2 * auge[2] - q[2]]))
    P = np.array(pts)
    tu, tv = P @ a_u, P @ a_v
    bu, bv = tu.max() - tu.min(), tv.max() - tv.min()
    assert bu <= s['u'][1] - s['u'][0] and bv <= s['v'][1] - s['v'][0], 'Seitenbild passt nicht in die Flaeche (%.1f x %.1f)' % (bu, bv)
    u0 = s['u'][0] + 0.5 * ((s['u'][1] - s['u'][0]) - bu) - tu.min()
    v0 = s['v'][0] + 0.5 * ((s['v'][1] - s['v'][0]) - bv) - tv.min()
    A = [list(a_u) + [u0], list(a_v) + [v0]]
    BAU_INFO['seite_uv'] = dict(texel=(round(bu, 1), round(bv, 1)), d=tuple(round(x, 3) for x in d), z_spiegel=round(auge[2], 1))
    return dict(A=A, d=tuple(float(x) for x in d), z_spiegel=float(auge[2]), u=s['u'], v=s['v'], schwelle=s['schwelle'])


def kopf_v3_malen(blob, p):
    """Kopfbild, Seitenbild und Schulterauge in die TIM-Flaechen des fertigen Blobs malen (Indizes der Zeile p['clut'])."""
    mo = modell_aus_blob(blob)
    r = nah_raster(mo, p['nah'])
    an = kopf_v3_anker(r, p)
    blob = bytearray(blob)
    d = EA.emd_dir(bytes(blob))
    dir_off = u32(blob, 0)
    tim = bytes(blob[d[8]:dir_off])
    cx, cy, cw, ch, c, ende = _clut(tim)
    iw, ih = struct.unpack_from('<HH', tim, ende + 8)
    breite = iw * 2
    pix = np.frombuffer(tim[ende + 12:ende + 12 + breite * ih], np.uint8).reshape(ih, breite).copy()
    pix_alt = pix.copy()
    pb = 128 * (p['page'] & 0xF)
    u0, v0, bu, bv = pb + p['u'][0], p['v'][0], p['u'][1] - p['u'][0] + 1, p['v'][1] - p['v'][0] + 1
    X, Y, _hat = _texel_lagen(r, u0, v0, bu, bv, an['maske'])
    rausch = _rausch_feld(pix_alt, _rgb(c[0]), (bv, bu))
    pix[v0:v0 + bv, u0:u0 + bu] = kopf_v3_bild(X, Y, an, p, rausch).astype(np.uint8)
    info = dict(auge_px=tuple(an['auge']), maul_px=tuple(an['maul']), kopfbreite_px=an['kopfbreite_px'])
    if p.get('seite_uv'):
        # Seitenbild: Seitenansicht rastern, Kopfauge und Mundwinkel (Kopfbild vorn) dorthin projizieren, Vorderkante je Zeile
        s = p['seite']
        pn = seite_nah(p)
        rs = nah_raster(mo, pn)
        su0, sv0 = pb + s['u'][0], s['v'][0]
        sbu, sbv = s['u'][1] - s['u'][0] + 1, s['v'][1] - s['v'][0] + 1
        ms = (rs['knochen'] == 9) & (rs['u'] >= su0) & (rs['u'] < su0 + sbu) & (rs['v'] >= sv0) & (rs['v'] < sv0 + sbv)
        k9 = rs['knochen'] == 9
        ea = _ins_bild(rs, pn, _lokal(r, *an['auge']))
        mb = min(p['maul']['b'] * p['nah']['H_px'], p['maul']['max_kopfbreite'] * an['kopfbreite_px'])
        ecken = [_ins_bild(rs, pn, _lokal(r, an['maul'][0] + sg * 0.5 * mb, an['maul'][1])) for sg in (-1, 1)]
        nach_vorn = _ins_bild(rs, pn, _lokal(r, *an['auge']) - 100.0 * np.array(p['blick'], float) / np.linalg.norm(p['blick']))
        vor = 1.0 if nach_vorn[0] > ea[0] else -1.0
        ys = np.nonzero(k9.any(1))[0]
        kz = np.array([(np.nonzero(k9[y])[0].max() if vor > 0 else np.nonzero(k9[y])[0].min()) for y in ys], float)
        kante = lambda yy: np.interp(yy, ys, kz)
        mw = max(ecken, key=lambda e: e[0] * vor)                     # Mundwinkel zur Kamera hin
        an_s = dict(auge=ea, maul=mw, vor=vor, kante=kante)
        SX, SY, _h = _texel_lagen(rs, su0, sv0, sbu, sbv, ms)
        # Texel des Seitenbilds, die von vorn (Messansicht) zu sehen sind: dort kein Augenprofil (Probe r2d: das Profil am Kopfrand
        # zaehlte R1 von vorn als zweites Auge, Fleck (719,396) L* 11,6)
        vv = (r['knochen'] == 9) & (r['u'] >= su0) & (r['u'] < su0 + sbu) & (r['v'] >= sv0) & (r['v'] < sv0 + sbv)
        von_vorn = np.zeros((sbv, sbu), bool)
        von_vorn[np.floor(r['v'][vv]).astype(int) - sv0, np.floor(r['u'][vv]).astype(int) - su0] = True
        an_s['von_vorn'] = von_vorn
        pix[sv0:sv0 + sbv, su0:su0 + sbu] = kopf_v3_seite_bild(SX, SY, an_s, p, _rausch_feld(pix_alt, _rgb(c[0]), (sbv, sbu))).astype(np.uint8)
        info.setdefault('seite', {})['texel_von_vorn'] = int(von_vorn.sum())
        info['seite'] = dict(info.get('seite', {}), auge_px=tuple(round(float(x), 1) for x in ea), mundwinkel_px=tuple(round(float(x), 1) for x in mw),
                             vor=vor, kante_auge_px=round(float(kante(np.array([ea[1]]))[0]), 1), texel_sichtbar=int(_h.sum()))
    sa = p.get('schulterauge')
    if sa and sa.get('ecken'):
        su0, sv0 = pb + sa['u'][0], sa['v'][0]
        sbu, sbv = sa['u'][1] - sa['u'][0] + 1, sa['v'][1] - sa['v'][0] + 1
        m8 = (r['knochen'] == 8) & (r['u'] >= su0) & (r['u'] < su0 + sbu) & (r['v'] >= sv0) & (r['v'] < sv0 + sbv)
        SX, SY, _h = _texel_lagen(r, su0, sv0, sbu, sbv, m8)
        Hp = p['nah']['H_px']
        zx, zy = sa['ziel_px']
        es = ((SX - zx) / (sa['b'] * Hp / 2)) ** 2 + ((SY - zy) / (sa['h'] * Hp / 2)) ** 2
        # Haut des Vierecks = die EM30-Texel der Schulter darunter (Messansicht ohne Viereck, r0), Farbe auf die naechste
        # Farbe der Hautzeile sa['clut'] gebracht (Mesh 8 nutzt Zeile 480 auf Seite 0 und 481 auf Seite 1): das Viereck traegt
        # dieselbe Haut wie die Schulter. Probe 4 mit eigener Hautrampe: als flaches violettes Quadrat sichtbar.
        r0 = p['r0']
        zeile = sa['clut'] - cy
        Lz = _lab(_rgb(c[zeile]))
        f = np.zeros((sbv, sbu), int)
        for (yy, xx), _w in np.ndenumerate(SX):
            iy, ix = int(round(SY[yy, xx])), int(round(SX[yy, xx]))
            k = r0['fi'][iy, ix]
            if k < 0 or r0['knochen'][iy, ix] != 8:
                f[yy, xx] = f[yy, xx - 1] if xx else 0
                continue
            fc = r0['F'][k]
            t = pix_alt[int(r0['v'][iy, ix]), int(r0['u'][iy, ix])]
            farbe = _rgb(c[(fc['clut'] >> 6) - cy][t])
            dist = np.linalg.norm(Lz[1:] - _lab(farbe), axis=1)
            f[yy, xx] = 1 + int(np.argmin(dist))
        # Die dunkle EM30-Ader, die hier ueber die Schulter laeuft (Probe 5: L* min 1,7, mit dem Auge verbunden -> Auge 2,6 x so gross
        # wie das Kopfauge), im Viereck mit dem Median-Hautton uebermalen: die KI zeigt am Schulterauge glatte helle Haut (K14).
        Lf = Lz[f, 0]
        med = np.median(Lf)
        hautton = f.flat[int(np.argmin(np.abs(Lf - med)))]
        f = np.where(Lf < sa['ader'] * med, hautton, f)
        # Auge: Eintrag der Hautzeile mit der Helligkeit K14 "L* Auge/Haut 0,36-0,39" (Mitte 0,375) bezogen auf den Hautton des
        # Vierecks und dem Buntton der KI (Lab a* 4,5 b* 2,4). Probe 6 mit der KI-Farbe absolut (L* 12): Verhaeltnis 0,56 (R9 rot),
        # weil die Schulterhaut des Ports dunkler ist als die der KI.
        ziel = np.array([sa['L_verh'] * med, sa['lab_auge'][1], sa['lab_auge'][2]])
        auge = 1 + int(np.argmin(np.linalg.norm(Lz[1:] - ziel, axis=1)))
        f = np.where(es <= 1.0, auge, f)
        pix[sv0:sv0 + sbv, su0:su0 + sbu] = f.astype(np.uint8)
        info['schulterauge_index'] = auge
        info['schulterauge_px'] = (zx, zy)
    tim = tim[:ende + 12] + pix.tobytes() + tim[ende + 12 + breite * ih:]
    blob[d[8]:dir_off] = tim
    BAU_INFO['kopf_v3'] = info
    return bytes(blob)


def schulterauge_v3_ecken(blob, p):
    """Lage des Schulterauges (Messblatt V3 K12: relativ zum Kopfauge sa['dx'] H waagerecht (B-links), sa['dy'] H tiefer) auf der
    Oberflaeche von Mesh 8 in der Messansicht suchen; Viereck (Ecken im Brustraum, Knochen 8): Mitte = Flaechenpunkt unter dem
    Zielbildpunkt, Kanten entlang Bild-rechts/-unten in der Tangentialebene, Seitenlaenge sa['seite'] H."""
    mo = modell_aus_blob(blob)
    r = nah_raster(mo, p['nah'])
    an = kopf_v3_anker(r, p)
    sa, Hp, sc = p['schulterauge'], p['nah']['H_px'], p['nah']['massstab']
    zx, zy = an['auge'][0] + sa['dx'] * Hp, an['auge'][1] + sa['dy'] * Hp
    iy, ix = int(round(zy)), int(round(zx))
    assert r['knochen'][iy, ix] == 8, 'Schulterauge: Zielpunkt nicht auf Mesh 8 (%d)' % r['knochen'][iy, ix]
    f = r['F'][r['fi'][iy, ix]]
    Pw = r['P'][iy, ix]                                 # Modellraum (nah_raster speichert V, nicht V - mitte)
    nw = sum(r['N'][j] for j in f['n'])
    nw = nw / np.linalg.norm(nw)
    R8, T8 = r['R'][8].astype(float) / 4096.0, r['T'][8].astype(float)
    # Kanten in der Tangentialebene, die in der Messansicht genau waagerecht (Bild-x = +Z) bzw. senkrecht (Bild-y = +Y) liegen:
    # das Viereck erscheint dort als achsparalleles Quadrat der Seite sa['seite'] H (Probe 3: schraeges Viereck liess die
    # EM30-Ader oberhalb im Messfenster frei).
    rechts = np.array([-nw[2], 0.0, nw[0]]); rechts /= np.linalg.norm(rechts)
    if rechts[2] < 0:
        rechts = -rechts
    unten = np.array([-nw[1], nw[0], 0.0]); unten /= np.linalg.norm(unten)
    if unten[1] < 0:
        unten = -unten
    hb = 0.5 * sa['seite'] * Hp / sc                    # halbe Seitenlaenge im Bild, in E.
    rechts, unten = rechts * hb / abs(rechts[2]), unten * hb / abs(unten[1])
    h = 1.0
    # Abstand zur Schulter entlang der Blickrichtung der Messansicht (+X), nicht entlang der Normale: so bleibt die Mitte des
    # Vierecks auf dem Zielbildpunkt (Probe 2: Versatz entlang der Normale schob die Mitte 30 px tiefer, das Auge lag am Rand).
    Pc = Pw + np.array([1.0, 0.0, 0.0]) * sa['abstand']
    welt = [Pc - rechts * h - unten * h, Pc + rechts * h - unten * h, Pc - rechts * h + unten * h, Pc + rechts * h + unten * h]
    ecken = [tuple(int(x) for x in np.round(R8.T @ (q - T8))) for q in welt]
    # Normale fuer das Licht = Mittel der EM30-Eckennormalen von Mesh 8 im Umkreis der halben Viereck-Diagonale plus Abstand um die
    # Viereckmitte (Runde 2; Runde 1: 400 E. ohne Beleg): so wird das Viereck schattiert wie die Schulter darunter (Probe 9 mit der
    # Flaechennormale aus der Messansicht (0,90 0,34 0,28): im Spiellicht h118 fast schwarz, die Nachbarecken zeigen nach oben/aussen).
    m8 = modell_aus_blob(blob).meshes[8]
    qv = np.array([v[:3] for v in m8.qv], float); qn = np.array([n[:3] for n in m8.qn], float)
    E = np.array(ecken, float)
    mitte_l = np.mean(E, axis=0)
    umkreis = 0.5 * np.linalg.norm(E[0] - E[3]) + sa['abstand']
    nah = np.linalg.norm(qv - mitte_l, axis=1) < umkreis
    nl = (qn[nah] / np.linalg.norm(qn[nah], axis=1, keepdims=True)).sum(0)
    nl /= np.linalg.norm(nl)
    BAU_INFO['schulterauge_umkreis'] = (round(float(umkreis), 1), int(nah.sum()))
    return dict(sa, ecken=ecken, normale=tuple(float(x) for x in nl), flaeche=tuple(float(x) for x in R8.T @ nw), ziel_px=(zx, zy),
                abstand_uv=0.0)


def baue_v3(basis):
    """Variante V3: (1) Bau ohne Schulterauge, (2) Schulterauge auf Mesh 8 legen, Seitenbild-Abbildung rechnen und neu bauen,
    (3) Kopfbild, Seitenbild und Schulterauge malen."""
    blob = baue(basis, _variante_v3())
    sa = schulterauge_v3_ecken(blob, KOPF_V3)
    su = seite_abbildung(blob, KOPF_V3)
    r0 = nah_raster(modell_aus_blob(blob), KOPF_V3['nah'])          # Messansicht ohne Viereck (Hautvorlage fuer das Viereck)
    blob = baue(basis, _variante_v3(sa, su))
    return kopf_v3_malen(blob, dict(KOPF_V3, schulterauge=sa, seite_uv=su, r0=r0))

def modell_aus_blob(blob):
    """EA.Modell ohne EMS-Zugriff aus einem fertigen Blob aufbauen (fuer das Ansichtsblatt)."""
    mo = EA.Modell.__new__(EA.Modell)
    mo.blob, mo.typ, mo.blob_index, mo.blob_len = blob, TYP, EA.EMS_ORDER.index(TYP), len(blob)
    d = EA.emd_dir(blob)
    mo.meshes            = EA.md1_parse(blob[d[7]:d[8]])
    mo.skel              = EA.emr_parse(blob[d[2]:d[3]])
    mo.clips, mo.frames  = EA.edd_parse(blob[d[1]:d[2]])
    mo.tex, mo.pal, mo.clut_y = EA.tim_decode(blob[d[8]:])
    mo.texh, mo.texw     = mo.tex.shape
    return mo


# ═══════════════════════════════════════════════════ Roter Affe (ROOM4080/4081)
# Video 1 4:11-4:19 (farbtreu): der Affe in 4080 ist tief dunkelrot; gemessen an den roten Bildpunkten
# r/g ~ 2.1 gegen r/g ~ 1.7 der EM29-Textur, deren Fell braun-grau ist (r/g ~ 1.2). Gestaltung: das ganze
# Fell nach dunklem Weinrot ziehen (Zielwert r/g ~ 2.5-3, b ~ g), deutlich abdunkeln. Nur die CLUTs aendern sich.
ROT_AFFE = dict(typ=0x29, datei='EM29_ROT.EMD',
                tint=(np.array([[0.70, 0.40, 0.20],
                                [0.08, 0.20, 0.08],
                                [0.08, 0.14, 0.22]]), 0.62))


# ═══════════════════════════════════════════════════ Spider-Man der Hangar-Szene (ROOM50A0/50A1)
# Runde 41 SF NB3 (Abnahme 1 Mangel 14, "Aussehen ist entscheidend"): Video 4 h40-h65 zeigt den Spider-Man gelb-orange mit
# schwarzen Streifen (gelbe Bildpunkte r > g > b + 15, Median (83,64,38), p25 (71,53,28), p75 (105,84,52); nb3/em2b.py), die
# EM2B-Textur der Auslieferung ist dunkelbraun (mittlere Texel-Farbe (53,35,23) / (46,26,18)). Gestaltung: jeder CLUT-Eintrag
# nach seiner Helligkeit auf eine Treppe schwarz -> gelb-orange; kraeftig rote Eintraege (Augen, Maul, r/g > 2,2) bleiben.
# Lauf nb3/e7 mit gelb (232,170,66), l0 16, l1 72: gelbe Bildpunkte h40-h64 2980 (Video 11370), Median (91,68,28) (Video
# (83,64,38)) -> mehr Texel gelb (l0 10, l1 55), Ton (225,168,90).
# Runde 41 SF NB5 (Abnahme 3 B2 "breite DUNKLE Masse", B3 "oranges Bein"): mit dem Massstab 1,5 der Szene (birkin_formen.c
# BF_SM_MASSSTAB) gelbe Bildpunkte (Maske der Abnahme) im Ansprung h4-h12 5100-7600 je Bild, Video 1150-1900; Halten 4300-6200,
# Video 500-1450 -> weniger Texel bis ganz gelb (l0 16, l1 95), Ton unveraendert.
SPIDERMAN = dict(typ=0x2B, datei='EM2B_HANGAR.EMD', schwarz=(8, 6, 4), gelb=(225, 168, 90), l0=16.0, l1=95.0, gamma=0.85)


def baue_spiderman():
    ems = open(EA.EMS_PFAD, 'rb').read()
    off, ln = EA.ems_entry(ems, EA.EMS_ORDER.index(SPIDERMAN['typ']))
    blob = bytearray(ems[off:off+ln])
    d = EA.emd_dir(bytes(blob))
    dir_off = u32(blob, 0)
    tim = bytes(blob[d[-1]:dir_off])
    cx, cy, cw, ch, c, ende = _clut(tim)
    rgb = _rgb(c)
    L = rgb @ np.array([0.299, 0.587, 0.114])
    t = np.clip((L - SPIDERMAN['l0']) / (SPIDERMAN['l1'] - SPIDERMAN['l0']), 0, 1) ** SPIDERMAN['gamma']
    neu = (np.array(SPIDERMAN['schwarz'], float) * (1 - t[..., None]) + np.array(SPIDERMAN['gelb'], float) * t[..., None])
    rot = (rgb[..., 0] / np.maximum(rgb[..., 1], 8.0) > 2.2) & (rgb[..., 0] > 60)
    neu = np.where(rot[..., None], rgb, neu)
    w = _pack(neu, c).astype('<u2')
    blk = bytearray(tim)
    blk[20:20 + w.size * 2] = w.tobytes()
    blob[d[-1]:dir_off] = bytes(blk)
    return bytes(blob)


def baue_rot_affe():
    ems = open(EA.EMS_PFAD, 'rb').read()
    off, ln = EA.ems_entry(ems, EA.EMS_ORDER.index(ROT_AFFE['typ']))
    blob = bytearray(ems[off:off+ln])
    d = EA.emd_dir(bytes(blob))
    dir_off = u32(blob, 0)
    tim_a = d[-1]                                   # letzte Sektion = TIM (dir[8])
    blob[tim_a:dir_off] = faerbe_tim(bytes(blob[tim_a:dir_off]), ROT_AFFE['tint'])
    return bytes(blob)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--blatt', help='Verzeichnis fuer Ansichtsblaetter (PNG)')
    ap.add_argument('--clip', type=lambda s: int(s, 0), default=0)
    ap.add_argument('--aus', default=AUS_DIR, help='Zielverzeichnis der EMDs')
    ap.add_argument('--variante', action='append', help='Schwanz-V2-Variante(n) (VARIANTEN) statt der Formen schreiben; '
                    'nur mit --aus ausserhalb von shared_assets')
    a = ap.parse_args()
    if a.variante:
        assert os.path.abspath(a.aus) != os.path.abspath(AUS_DIR), 'Varianten nicht ins Spiel-Asset (Wahl des Nutzers offen)'
        basis = lade_em30()
        os.makedirs(a.aus, exist_ok=True)
        for name in a.variante:
            BAU_INFO.clear()
            blob = baue_v3(basis) if name == 'v3' else baue(basis, _variante_v4()) if name == 'v4' else baue(basis, VARIANTEN[name]())
            ziel = os.path.join(a.aus, 'EM30_%s.EMD' % name.upper())
            with open(ziel, 'wb') as f:
                f.write(blob)
            info = BAU_INFO.get('schwanz_v2', {})
            print('geschrieben: %s  (%d Byte, Variante %s; H %.0f, Welle %.0f, Glieder %s)' % (
                ziel, len(blob), name, info.get('H', 0), info.get('a', 0), BAU_INFO['ruhe']['L']))
        return

    affe = baue_rot_affe()
    ziel = os.path.join(a.aus, ROT_AFFE['datei'])
    os.makedirs(a.aus, exist_ok=True)
    with open(ziel, 'wb') as f:
        f.write(affe)
    print('geschrieben: %s  (%d Byte, roter Affe)' % (ziel, len(affe)))
    if a.blatt:
        mo = EA.Modell.__new__(EA.Modell)
        mo.blob, mo.typ, mo.blob_index, mo.blob_len = affe, 0x29, EA.EMS_ORDER.index(0x29), len(affe)
        dd = EA.emd_dir(affe)
        mo.meshes = EA.md1_parse(affe[dd[7]:dd[8]]); mo.skel = EA.emr_parse(affe[dd[2]:dd[3]])
        mo.clips, mo.frames = EA.edd_parse(affe[dd[1]:dd[2]])
        mo.tex, mo.pal, mo.clut_y = EA.tim_decode(affe[dd[8]:]); mo.texh, mo.texw = mo.tex.shape
        mo.titel = 'Roter Affe ROOM4080 - abgeleitet von EM29 (Typ 0x29)'
        mo.untertitel = 'NACHBILDUNG nach Video 1 4:11-4:19 - EM29_ROT.EMD - Basis CDEMD0.EMS Blob %d'
        mo.hinweis = 'NICHT byte-true: nur die CLUTs sind umgefaerbt (birkin_formen.py ROT_AFFE). Alles andere = EM29.'
        os.makedirs(a.blatt, exist_ok=True)
        EA.blatt(mo, 0, os.path.join(a.blatt, 'affe_rot.png'))
    # Neuaufbau ab NB2: kein EM2B_HANGAR - der Spider-Man traegt die Auslieferungs-Textur (wie NB2; NB3-Gelb nicht uebernommen).
    basis = lade_em30()
    d = EA.emd_dir(basis)
    roh = basis[d[7]:d[8]]
    assert md1_schreiben(md1_lesen(roh)) == roh, 'MD1-Serializer nicht byte-gleich zu EM30'
    os.makedirs(a.aus, exist_ok=True)
    for name, form in FORMEN.items():
        blob = baue(basis, form)
        ziel = os.path.join(a.aus, form['datei'])
        with open(ziel, 'wb') as f:
            f.write(blob)
        print('geschrieben: %s  (%d Byte, Form %s)' % (ziel, len(blob), name))
        if a.blatt:
            os.makedirs(a.blatt, exist_ok=True)
            mo = modell_aus_blob(blob)
            mo.titel      = '%s - abgeleitet von EM30 (Typ 0x30)' % form['titel']
            mo.untertitel = 'NACHBILDUNG nach Beta-Screenshots - ' + form['datei'] + ' - Basis CDEMD0.EMS Blob %d'
            mo.hinweis    = ('NICHT byte-true: Vertex-/Normalen-Skalierung, Bindoffsets und Farben sind Gestaltung '
                             '(birkin_formen.py FORMEN)' + (', Schwanz angehaengt an Mesh 8' if form.get('schwanz') else '')
                             + (', Keyframes umgehalten (Vierbein)' if form.get('haltung') else '')
                             + '. Unveraendert aus EM30: Topologie, UVs, Skelett-Hierarchie, Clip-Tabelle.')
            EA.blatt(mo, a.clip,
                     os.path.join(a.blatt, 'birkin_%s.png' % name))


if __name__ == '__main__':
    main()
