# SF Leser V3 B: Kopf/Augen/Maul und Schwanz der Sichel-Form, Messung an den KI-Videobildern (2026-11-06)

Kein RE-Befund, keine @0x-Konstante. Gestaltungsquelle KI (PORT-WAHL, Nutzerentscheid F4). Nichts committet.

## 0. Quelle, Verfahren, Konventionen

- KI-Bilder: `Version_80%/Resident Evil 1.5 Complete Disc Video 4 (480p Upscale) (480p)_slp.mp4`, mit ffmpeg
  (`select=between(n,1350,1481)`, `-vsync 0`, `-start_number 1351`) nach
  `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/ki/n1351..n1482.png` (1280x960).
  Original genauso nach `.../sfv3/orig/` (640x480), Schwarzwert 17 abgezogen und auf 0..255 gestreckt.
- Zuordnung gegengeprueft (`b_zuordnung.py`, Template-Match der Freisteller): birkin12 = n1435 (RMS 0,0, Nachbarn 12,9-30,0),
  birkin19 = n1468 (0,0; 11,7-23,8), birkin20 = n1475 (0,0; 15,4-30,0). Die Freisteller wurden sonst NICHT verwendet.
- Freistellung: Hintergrund als dichteste Zeitgruppe je Pixel ueber 66 Bilder (`b_hintergrund.py`, der Median zeigt in x 640-930
  noch die Figur), dann CIE-Lab-Abstand dE (OpenCV float Lab, sRGB/D65) und Rotkriterium (`b_schwanz.py`, Kopfdaten lokal).
- H je Bild: Oberkante Buckel bis Sohle. h85 = 504 (Vorgabe; eigene OK-Messung y 370 statt 367), h118 = 569 (Vorgabe; eigene
  OK y 316). h88 503, h92 509, h95 507 (OK 368/366/364 aus `b_hoehe.py`, Sohle = Schuhrot + 8 px Korrektur aus h118: 876 statt 884;
  h88 Sohle verdeckt, wie h85 angesetzt) - geschaetzt +-8 px. h125: Kopfoberkante y 292 ueber dem Buckel, Sohle 879+8 -> **H 595 (geschaetzt +-8)**.
- Seiten: (W) = aus Sicht des Wesens, (B) = Betrachter. Von vorn B-links = W-rechts.
- Flaechen fuer Farben 2 px erodiert. "rot" = Lab a* > 12 (zusaetzlich HSV-Rot in den JSON-Dateien).
- Werkzeuge: `re15_port/tools/r41_sichel/v3/b_*.py` (zuordnung, hintergrund, hoehe, schwanz, farben, original, halten, muster,
  auge, maul, kopf, port_schwanz). Rohwerte: `.../sfv3/schwanz/*.json`, `.../sfv3/schwanz/muster.jsonl`, `.../sfv3/auge/augen.jsonl`.
- Belegbilder: `analysis/befunde_runde41/SF_belege/v3_lesung/B_1..B_6*.png`.

## 1. Augen und Kopf

| Mass | Wert KI (Bild, Koordinaten 1280x960) | Original-Video | Port V1 | Sicherheit |
|---|---|---|---|---|
| A1 Augen auf dem Kopf | **1** (h118 Mitte (792,356.5); h125 (789.5,335); Seite h85 (919.5,490.5)). Verfolgung per NCC (Vorlage h118): h103-h127 0,72-1,00, h91-h99 in der Seitenansicht 0,70-0,84 bis zum Fleck in h85 -> dasselbe Auge | nicht aufloesbar (320x240) | 1 Kopfauge + 1 Schulterauge (`08_rundum_V1.jpg` Kopf vorn nah (1175,766) und (1003,786)) | sicher |
| A2 Lage in der Kopfbox (x/B, y/H) | h118: **0,36 / 0,36** (Box x 768-833, y 317-427); h125: 0,32 / 0,35 (Box x 770-830, y 292-415) | - | Auge in der oberen Haelfte der rosa Kopfplatte | Box: geschaetzt +-3 px (Faltenminimum/Schatten, `b_kopf.py`) |
| A2 Lage zur Maulmitte | h118: dx -1,5 px, **dy -28,5 px = 0,050 H** ueber der Maulmitte (793.5,385); h125: dx -6 px, dy -33 px = 0,055 H | - | ueber dem Maul-Oval | wahrscheinlich |
| A3 Auge gesamt | h118: dunkler Kern **10 x 5 px** (x 787-796, y 354-358), Aequivalentdurchmesser 6,7 px = **0,012 H = 0,10 Kopfbreite**; mit Lidschatten (L* < 35) 14 x 8 px = 0,025 H = 0,21 Kopfbreite. h125: 11 x 6 px, 8,3 px = 0,014 H = 0,14 Kopfbreite. Mandelform waagerecht (B/H 2,0) | - | Ringauge 36 x 39 px, Aequivalent 38,8 px = **0,25 Kopfbreite** (Kopfplatte ~155 px), rund (B/H 0,9) | sicher (Kern), geschaetzt (Lid) |
| A3 Pupille | h118: L* < 15: 22 px -> 5,3 px = 0,009 H = 0,08 Kopfbreite; h125: 32 px -> 6,4 px | - | dunkle Mitte 590 px (rundum) | sicher |
| A4 Farbe Pupille | h118 RGB (27,14,19), Lab [5.8, 6.5, -0.8]; h125 (28,19,23) [7.3, 4.9, -1.4] | - | (37,17,15) [7.8, 9.5, 4.9] | sicher |
| A4 Farbe Auge gesamt | h118 (63,50,56) [21.1, 6.3, -1.2]; h125 (54,45,50) [19.0, 4.4, -1.3] | - | (86,68,65) [29.5, 7.6, 4.8] | sicher |
| A4 Lidrand (2 px Ring) | h118 (110,95,100) [41.4, 7.1, -0.5]; h125 (99,84,89) [36.9, 7.2, -0.8] | - | (100,75,74) [34.4, 10.0, 4.8] | sicher |
| A4 Haut um das Auge | h118 (129,109,111) [47.4, 8.1, 1.7]; h125 (118,101,104) [43.9, 7.4, 1.0] | - | (95,70,68) [32.3, 10.6, 5.4] | sicher |
| A4 Helligkeit Auge/Haut | **0,45** (h118), 0,43 (h125), 0,32 (h85) | - | **0,92** (rundum), 0,95 (05_kopf_nah V1 h118) | sicher |
| A4 helle Sclera? | **nein**. L* > 80: 2,9 % (h118, 1 px), 0 % (h125); L* > Haut+15: 14 % / 6 % = nur das Glanzlicht | - | **ja, heller Ring**: L* > Haut+15: 40 % (rundum), 47 % (05 V1 h118); L* > 80: 0 % (Renderhelligkeit) | sicher |
| A5 Glanzlicht | **ja**: 6 px (h118) um (792.0,356.8), L* max 82; 3 px (h125) um (790,335), L* max 78; mittig in der Pupille, ~2 x 3 px = 0,004 H | - | kein Punktglanz, dafuer weisser Ring + helles Kreuz in der Pupille | sicher |
| A6 Schulterauge | **ja**, Schulter **W-rechts (= B-links von vorn)**. h118 Mitte (728,385.5): 64 px = 0,11 H seitlich, 29 px = 0,05 H tiefer als das Kopfauge, 0,12 H unter OK. Kern 14 x 9 px, Aequivalent 11,1 px = **0,020 H** (h125: 12,2 px). Pupille (34,21,21) [8.8, 5.9, 2.5]; Lid (79,64,64) [28.6, 6.3, 2.9]; Haut (98,81,81) [35.9, 7.3, 2.7]; Auge/Haut 0,42-0,44; kein Glanz (L* max 37), kein Weiss. Sichtbar h103-h132 (NCC 0,80-1,00); Seitenansicht h91-h99 nur NCC 0,62-0,72 (nicht eindeutig) | - | ja, B-links, gleiches Ringauge wie am Kopf (Aequivalent 38,9 px, Auge/Haut 0,93) | sicher (Lage), geschaetzt (Seitenansicht) |
| A7 Maul Groesse | Zahnreihen-Huelle h118 x 774-813, y 374-396: **40 x 23 px = 0,070 x 0,040 H = 0,61 x 0,21 der Kopfbox**; h125 x 774-817, y 356-380: 43 x 25 px (0,072 x 0,042 H). Leicht schraeg (obere Zahnreihe h118 von (774,374.5) nach (813,381): ~10 Grad) | - | Oval 45 x 77 px in `05_kopf_nah.jpg` V1 h118 (x4-Ausschnitt), B/H **0,58** | geschaetzt (Rand im Dunkel, Huelle aus Zahnflecken) |
| A7 Form | **waagerecht breit, Seitenverhaeltnis 1,7** (offener Spalt quer ueber den Kopf, darunter rote Kehlblase) | - | **senkrechtes Oval** (0,58) | sicher |
| A7 Farbe innen | (63,34,36) [16.8, 13.9, 4.6] (h118, Fenster ohne Zahnflecken); h125 (60,32,34) [15.9, 13.2, 4.6] | - | dunkel, roter Rand | sicher |
| A7 Zaehne | viele kleine helle Flecken auf beiden Kiefern: h118 oben 5-6 (x 774, 780, 785, 800, 813), unten 4-6; h125 oben 5, unten 3-4. Je Fleck 1-3 px (0,002-0,005 H) = Form nicht aufloesbar (Punkte/Spitzen). Farbe (172,133,134) [58.9, 15.6, 4.9] | - | 2 grosse weisse Zaehne (1 oben, 1 unten) | Zahl geschaetzt +-2, Lage sicher |
| A8 Kopfbox | h118 **66 x 111 px = 0,116 x 0,195 H**; h125 61 x 124 px = 0,103 x 0,208 H | nicht trennbar | Kopfplatte rosa-hell | geschaetzt +-3 px |
| A8 helle Kuppe / rote Zone | Baender je 0,02 H ab Kopfoberkante (Anteil L* > 40 / a* > 12), h118: 0,00: 0,07/0,23 (rote Scheitellinie); 0,04-0,06: **0,20-0,26 / 0,00 (Kuppe)**; ab 0,10: 0,03/0,48 bis 0,16: 0,03/0,76 (Maul + Kehle). h125 gleich (0,04-0,06: 0,22-0,24/0,00; 0,14-0,16: 0,06/0,80). Kuppe rechts (B) im Schatten | - | Kopfplatte einheitlich rosa | sicher |

## 2. Schwanz

Zwei verschiedene Erscheinungen, beide gemessen:
- **Seitenbogen h83-h98** (Nutzerdeutung 2026-11-05 = Schwanz, "der grosse Schwanz"). KI malt ans Ende eine Hand (h98 Finger sichtbar).
- **Halten h61-h70 vorn** (B-rechts = W-links, hinter der Huefte schraeg zum Boden): rot.

Messbar frei: h85, h88, h92, h95. h83 (Truemmer im Bild unten) und h98 (Schwanz liegt am Arm an, keine Kerbe) **nicht trennbar**.

| Mass | Wert KI (Bild, Koordinaten) | Wert Original-Video | Port V1 | Sicherheit |
|---|---|---|---|---|
| S1 Lab Schwanz (Seitenbogen) | h85 **[17.3, 6.1, 1.4]** RGB (52,40,41); h88 [16.4, 7.0, 1.6]; h92 [12.6, 7.6, 2.3]; h95 [11.4, 8.9, 2.4] | h85 [11.9, 4.1, 1.9]; h88 [11.0, 5.4, 2.2]; h92 [6.7, 5.3, 2.5] | **[21.8, 19.9, 8.4]** RGB (83,40,41) (`08_rundum_V1.jpg` 270 Grad) | sicher |
| S1 Lab Haut Ruecken/Buckel | h85 [40.3, 12.7, 2.2]; h88 [43.1, 11.7, 1.8]; h92 [45.4, 11.6, 2.4]; h95 [41.5, 10.9, 1.7] | h85 [37.9, 12.6, 4.0]; h88 [41.9, 11.5, 3.4]; h92 [44.1, 10.5, 4.4] | Flanke [17.1, 7.8, 3.3] | sicher |
| S1 Lab Oberarm vorn (W-rechts) | h85 [21.6, 5.7, 2.1] (Schatten); h88 [33.3, 11.5, 3.2]; h92 [36.5, 12.2, 3.7] | h85 [18.3, 5.1, 2.8]; h88 [30.7, 11.1, 5.3]; h92 [36.0, 11.6, 6.3] | Oberarm [8.5, 7.6, 1.9] | sicher |
| S1 Lab Jeans / Sichel / Bein | Jeans h92 [9.8, -0.1, -5.3]; Sichel h92 [7.5, -1.9, 2.4]; Bein h92 [27.4, 4.5, 0.0] | Jeans [3.7, 0.0, -4.2]; Sichel [1.8, -1.5, 1.4]; Bein [23.1, 3.7, 0.2] | - | sicher |
| S1 Delta-E Schwanz-Haut | Ruecken 23,9 / 27,1 / 33,1 / 30,2; Oberarm 4,4 (h85 Schatten) / 17,6 / 24,4; Bein 13,2 / 15,3; Jeans 11,7 / 11,2; Sichel 10,8 / 10,7 | Ruecken 27,4 / 31,5 / 37,8; Oberarm 6,6 / 20,8 / 30,2; Bein 15,9 / 16,6; Jeans 10,7 / 9,0; Sichel 8,5 | Flanke 13,9; Oberarm 19,3 | sicher |
| S1 Farbton / Buntheit | Farbton (atan b/a) Schwanz 13-17 Grad, Ruecken 9-11 Grad (**gleiche Farbfamilie**); Buntheit Schwanz 6,2-9,2, Ruecken 11,1-12,9 (**Schwanz 0,48-0,83 x so bunt wie die Haut**) | Schwanz 25 Grad / 4,5-5,9; Ruecken 18-23 Grad / 11-13 | Schwanz 23 Grad / **21,6**; Flanke 23 Grad / 8,5 (**2,5 x so bunt wie die eigene Haut**) | sicher |
| S1 Anteil rot (a* > 12) | Schwanz 13-25 %, Ruecken 19-39 % | - | Schwanz **96 %**, Flanke 8 % | sicher |
| S1 Helligkeit L* Schwanz/Haut | /Ruecken **0,43** (h85), 0,38, 0,28, 0,27; /Oberarm h88-h92 0,49-0,34 | /Ruecken 0,32 / 0,26 / 0,15 | /Flanke **1,27** (heller als die Haut) | sicher |
| S1 Halten h64/h68 (vorn) | **rot**: [24.8, 23.2, 8.3] / [25.9, 25.1, 8.9], a* > 20 bei 66-71 %; Haut Oberarm B-rechts [22.9, 8.3, 2.2]; dE 16,2 / 19,1 | **ebenfalls rot**: [22.5, 21.1, 10.4] / [23.1, 23.6, 11.2], a* > 20 bei 54-70 %; dE zur Haut 14,8 / 18,7 | - | sicher (Farbe), Deutung als derselbe Schwanz offen |
| S2 Muster | **feine Marmorierung, keine Ringbaender**: Ringanteil (Zeilenmittel nach Entfernen des Rundungsschattens) 0,21 / 0,18 / 0,29 / 0,24; AKF laengs erstes Maximum -0,23 (Lag 18) / -0,23 (17) / 0,01 (33) / 0,09 (35) -> keine Periode; Korrelationslaenge 3 px laengs und quer (Ruecken 3-4 px); Querprofil einseitig hell (L* 8 -> 23 ueber die Breite = Licht von oben); Laengsstreifen keine (Spaltenanteil 0,27-0,50 = Rundungsschatten) | Marmor nicht aufloesbar, Schwanz gleichmaessig dunkel | **Ringbaender quer**: Ringanteil **0,39**; AKF-Maximum 0,04 bei 30 px (0,075 H); im gestreckten Bild durchgehende Querbaender | sicher (Ringanteil), Deutung "Marmor" geschaetzt (Augenschein B_2) |
| S3 Dicke bei s = 0,1/0,2/0,3/0,5/0,7/0,9 (in H) | **h85: 0,204 / 0,149 / 0,123 / 0,079 / 0,054 / 0,030** (103/75/62/40/27/15 px); h88: 0,193 / 0,153 / 0,127 / 0,078 / 0,022* / 0,026; h92: 0,138 / 0,124 / 0,094 / 0,063 / 0,033 / 0,030; h95: 0,097 / 0,083 / 0,059 / 0,039 / 0,057 / 0,028. (* Maske an der Sockelleiste eingeschnuert) | - | 0,120 / 0,165 / 0,175 / 0,122 / 0,075 / 0,035 (Kachel-H 400 px geschaetzt) | sicher h85/h92, h88 bei 0,7 Messfehler |
| S3 relativ zum Oberarm | Oberarm h92 waagerecht x 703-752 bei y 500 = 50 px, senkrecht ~45-50 px = 0,09-0,10 H (geschaetzt). Schwanz h85: 2,1 / 1,5 / 1,2 / 0,8 / 0,5 / 0,3 Oberarmbreiten; h92: 1,4 / 1,3 / 1,0 / 0,6 / 0,3 / 0,3 | - | Dicke nimmt bis 0,3 zu (1,2 -> 1,75 Oberarm) | geschaetzt (Oberarm) |
| S4 Glaette | Tangentenwinkel (Mittellinie Gauss 0,015 H): h85 177 -> 134 (0,2) -> 121 (0,5) -> 91 (0,55) -> 79 (0,75) -> 59 Grad (Spitze), Gesamtdrehung 118 Grad; h88 Gesamtdrehung 120, h92 53, h95 6 Grad. **Max je 0,05 H: 35 / 46 / 37 / 20 Grad**. Wiederkehrender Bogen bei s 0,65-0,75 (h88 46, h92 37 Grad) = Handgelenk der KI-Hand | nicht aufloesbar | max 27,6 Grad je 0,05 H bei s 0,15 | sicher |
| S4 Knicke (> 20 Grad auf < 0,05 H) | h85 3 (Wurzeluebergang s 0,05; s 0,5; s 0,72), h88 2, h92 2, h95 1 | - | 2 | wahrscheinlich (h85 s 0,5 Maskenrand) |
| S5 Ende Seite | Knauf/Hand bei s 0,7-0,8, danach **spitz auslaufende rote Kralle**: h85 Spitze (600,710), Breite 0,95: 14 px = 0,028 H; Endstueck s >= 0,85: [11.1, 11.7, 3.7] RGB (46,23,25), Rotanteil HSV 97 %; h98: Finger sichtbar (Hand). Verjuengtes Ende (Breite < 0,035 H) ab s ~0,8 = **~0,14 H** (h85: 0,2 x 0,70 H) | Ende nicht aufloesbar | Spitze ohne Hand, salmrot | sicher (Seite KI) |
| S5 Ende vorn (Halten) | h64-h70: **spitz**, rot mit hellem Querband vor der Spitze (Beleg B_5) | rot | - | sicher (KI), Bilder uneinig mit der Seitenansicht |
| S6 Wurzel | Schnitt Kerbe -> naechster Oberkonturpunkt: h85 (712,481)-(685,387), Laenge **97 px = 0,19 H**, Mitte (698.5,434) = **0,13 H unter OK, 0,19 H hinter dem Buckelscheitel (x 794)**; h88 (715,484)-(681,388) 0,20 H, Mitte 0,13 H unter OK. h92/h95 liegt der engste Schnitt tiefer und weiter hinten ((666,454), (636,486); 0,17 / 0,12 H). Austrittsrichtung h85 175-177 Grad = waagerecht nach hinten; Rueckenlinie Gegenpunkt -> Buckelscheitel 9 Grad -> **Austritt in Verlaengerung der Rueckenlinie (~5-10 Grad)**; Oberseite ohne Stufe in den Buckel | - | Gelenk 0,13 H unter OK (Dossier V2, L) | sicher (h85/h88), Projektion |
| S Laenge (Bild) | h85 0,70 H, h88 0,64 H, h92 0,47 H, h95 0,44 H (Projektion, Schwung) | - | 0,79 H (Kachel) | sicher als Bildmass |

**Original-Video (n1435/n1438/n1442, Beleg B_4):** Der Bogen ist im Original dieselbe Farbfamilie wie die Haut (Farbton 25 gegen 18-23 Grad),
aber dunkel und matt: L* 6,7-11,9, Buntheit 4,5-5,9 gegen Ruecken L* 38-44, Buntheit 11-13; Delta-E zum Ruecken 27-38, zum
beschatteten Oberarm h85 6,6. Kein eigener Farbton, keine Rot-Weiss-Baender im Bogen. Delta-E KI-Bogen gegen Original-Bogen h85: 5,8.
Die KI gibt also die Farbe des Originals wieder (etwas heller). Rot ist im Original wie in der KI nur das Glied beim Halten h64/h68
(a* 21-24) - ob das derselbe Koerperteil ist, entscheidet keine Quelle (offen).

## 3. Abweichungen Port V1 gegen Vorlage (KI)

1. **Augenbild falsch**: V1 = weisser Ring + dunkle Pupille + helles Kreuz, rund, Helligkeit Auge/Haut 0,92-0,95, heller Anteil 40-47 %.
   KI = dunkle waagerechte Mandel (B/H 2,0), Auge/Haut 0,43-0,45, **keine Sclera** (L* > 80: 0-3 %), nur ein Punktglanz von 3-6 px
   (L* 78-82) mittig in der Pupille, Lidrand dunkler als die Haut ([41, 7, -0.5] gegen [47, 8, 1.7]).
2. **Augengroesse**: V1 Kopfauge 0,25 Kopfbreite; KI 0,10 (Kern) bis 0,21 (mit Lid). Das KI-Schulterauge ist GROESSER als das
   Kopfauge (11-12 gegen 7-8 px); in V1 sind beide gleich.
3. Augenzahl stimmt (1 Kopf + 1 Schulter W-rechts); Lage Kopfauge stimmt grob (ueber der Maulmitte, obere Kopfhaelfte).
4. **Maulform falsch**: V1 senkrechtes Oval (B/H 0,58) mit 2 grossen Zaehnen; KI waagerechter breiter Spalt (B/H 1,7,
   0,61 Kopfbreite), dunkelrot [16.8, 13.9, 4.6], mit je ~5 kleinen Zahnpunkten oben und 3-6 unten, darunter rote Kehle.
5. **Kopffarbe**: V1 Kopfplatte einheitlich rosa; KI helle Kuppe nur 0,04-0,08 H unter der Kopfoberkante, rote Scheitellinie oben,
   ab 0,10 H rot (48-80 % a* > 12).
6. **Schwanzfarbe falsch**: V1 salmrot, a* 19,9, Buntheit 21,6 = 2,5 x die eigene Haut, 96 % rote Pixel, heller als die Haut (1,27).
   KI-Seitenbogen: Haut-Farbfamilie (Farbton 13-17 Grad), Buntheit 6-9 = 0,5-0,8 x Haut, 13-25 % rote Pixel, **dunkler** als die
   Haut (L* 0,27-0,43 x Ruecken). Original bestaetigt (L* 0,15-0,32 x Ruecken, Buntheit 4,5-5,9).
7. **Muster falsch**: V1 Ringbaender quer (Ringanteil 0,39, CLUT-Zeile 486 laut Auftrag); KI feine Marmorierung ohne Periode
   (Ringanteil 0,18-0,29, AKF ohne positives Maximum), Licht einseitig von oben.
8. **Dickenverlauf**: V1 wird bis s 0,3 dicker (0,12 -> 0,175 H) und ist bei 0,5 noch 0,12 H; KI ist an der Wurzel am dicksten
   (h85 0,20 H bei 0,1, 0,15 bei 0,2) und verjuengt stetig (0,08 bei 0,5, 0,03 bei 0,9).
9. **Ende**: V1 glatte Spitze; KI Seite: Handgelenk-Bogen 37-46 Grad je 0,05 H bei s 0,65-0,75 und rote Kralle/Finger (h98),
   KI vorn (Halten): spitz.
10. Wurzel: KI 0,13 H unter OK, Austritt waagerecht in Verlaengerung der Rueckenlinie, ohne Stufe; Port-Wurzel laut Dossier V2
    ebenfalls 0,13 H (nicht nachgemessen).

## 4. Offen

- Ist das rote Glied beim Halten (h61-h70, KI und Original rot) derselbe Koerperteil wie der hautfarbene Seitenbogen? Keine Quelle entscheidet.
- Ende des Seitenbogens: KI zeigt eine Hand mit Fingern (h98) und rote Kralle (h85); vorn eine Spitze. Original loest es nicht auf.
- Zahnform: 1-3-px-Flecken, nicht aufloesbar.
- Kopfboxraender und Oberarmbreite sind an der Lupe gesetzt (geschaetzt +-3 px).
- Port V1 nur aus den Leisten gemessen (`08_rundum_V1.jpg` neutrales Licht, `05_kopf_nah.jpg`), nicht neu gerendert; Kachel-H 400 px geschaetzt.
