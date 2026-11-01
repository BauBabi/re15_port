# Gegenprobe Herkunft Birkin und Alligator (unabhaengig, 2026-11-01)

Geprueft: `birkin_modelle.md`, `birkin_videos.md`, `alligator_40.md` samt Bildern. Alles unten selbst gemessen, mit eigenen
Werkzeugen (`gegenprobe_werkzeuge/`, eigener Capstone-Disassembler `gpdis.py`, kein re15_disasm.py) und eigenen Standbildern
(`gegenprobe_bilder/`). Trennung: **Disc** = `Version_40%/`, **Mod** = `info/Re1.5/`, **Port** = `re15_port/`, **RE2** = `re15_port/shared_assets/RE2/`.
Kein Spielcode, kein Commit; Aenderungen nur in diesem Ordner (Aenderungslisten am Ende der drei Dossiers).

## Antwort Frage 1: Birkin des 2. Kampfes

Gemeint ist der Birkin vor der Mutation in 5080/5090 (Leon) bzw. 5081/5091 (Elza). Das ist im Port und in der Mod das Modell
**M1** (`info/Re1.5/PSX/EMD/CDEMD1.EMS` Sektor 2015, 228460 B, md5 819acc8c). Es ist ein Fan-Modell der Mod (README Z.92).
Auf der unveraenderten 40%-Disc gibt es weder M1 noch einen zweiten Birkin-Kampf. Die 80%-Videos zeigen keinen Birkin in M1-Gestalt.
Wo sie einen Birkin in Menschengestalt zeigen (307, 50A), sieht er aus wie das Disc-Modell D0. Am Bahnsteig zeigen sie nur die
Riesenform mit Schwanz. M1 ist aber nicht aus dem Nichts erfunden: Skelett, alle Bewegungen, Oberkoerper, Kopf und Arme sind
Capcoms Disc-D0, Beine, Fuesse und Klaue kommen aus RE2-G2. Becken, Teile der Klaue und der Textur sind neu.
Lesart (b), der unveraenderte Capcom-Birkin: D0. Die Disc hat ihn im Kampf 307, und ihr STAGE5-Code haette ihn auch in Stufe 5 geladen.

## Antwort Frage 2: Alligator auf der 40%-Disc

**KI ja, Modell nein.** Die Disc-STAGE2.BIN enthaelt eine vollstaendige Alligator-KI (Typ 0x23, 0x8010c448..0x8010ee9c), byte-gleich
in der Mod. Kein Disc-Raum setzt sie, und es gibt kein Modell: TOC 0x23 ist in beiden Archiven 0, die Archive enthalten keine Daten
ohne TOC, und das Rohabbild hat keine versteckten Daten. Raum 209 existiert auf der Disc nur als Huelle: Capcom nennt 208/209/20A
"WANI BOSS" (WANI = Krokodil), ROOM2090.RDT liegt ohne Tuer und ohne Gegner vor. Das Alligator-Modell der Mod (Block Sektor 1922,
ohne TOC) ist ein RE2-EM23-Umbau. Der Port-Alligator in 2090 nutzt RE2-EM23 und einen eigenen Ablauf.

## Bestaetigt (eigene Messung, Grad)

| Aussage | Eigener Beleg | Grad |
|---|---|---|
| TOC-Werte Disc/Mod | `toc.py`: Disc Leon 0x30 @0x80073038 (Datei 0x63838) `ef 04 00 00 64 74 03 00`, Elza @0x80073278 `f2 04 ..`; Mod Elza `df 07 00 00 6c 7c 03 00`; TOC 0x22/0x23 ueberall 0 | sicher |
| Abschnitts-md5 D0/D1/M0/M1k/M1/M0x | `sec.py`, Tabelle 3.1 in allen Werten gleich; Port-CDEMD1 Sektor 2015 = M1 | sicher |
| Lader | `lw v1,0x800aca3c` @0x8002232c, `bltz v1` @0x80022334: >=0 Datei 0x26 + TOC 0x80072f38, <0 Datei 0x27 + TOC 0x80073178; Disc = Mod | sicher |
| Archivwahl im Skript | Set-Handler @0x8003fdd0: Bankzeiger[1] @0x80074668 = 0x800aca3c, `srlv 0x80000000` -> `22 01 00 01` setzt Bit 31. Mod 5080 @0x742, 5090 @0x1246 | sicher |
| Typ = Byte 2 des Sce_em_set | Handler @0x800420a0: `lbu v0,0(s2)` (s2 = pc+2) @0x800422b4 -> `sb v0,8(s0)` @0x800422bc | sicher |
| Zensus Disc | `zen2.py` (nur SCD-Abschnitte): 6 echte Birkin-Saetze (3070 @0x3064/0x30ac, 3071 @0x3072/0x30be/0x30d6, 3080 @0x78c Typ 0x36). 6040 @0x5d4 = Zeigertabelle | sicher |
| Zensus Mod = Port | 13 Saetze wie Dossier; alle 250 RDT/EMS/BIN der Mod = `shared_assets` | sicher |
| M1 nicht auf der Disc | Disc-TOC deckt beide Archive lueckenlos (`cover.py`); M1-TIM 0/60 Stichproben in 334 Dateien, 0/63 im Rohabbild; MD1 34/60 nicht gefunden | sicher |
| Abbild = Baum | `img.py`: 334 Dateien im ISO, 333 bytegleich, CAPCOM.STR fehlt nur im Baum | sicher |
| M1-Bauweise | `md1.py`/`g2.py`: Meshes 8-14 bytegleich D0, Mesh 16 = Mesh 15, Keyframes 871/871 in D0, Beine 1-6 <= 2 Einheiten an G2 12-17 | sicher |
| Raumnamen Disc | `namen.py`: 208 `WANI BOSS ENT`, 209/20A `WANI BOSS BEYA`, 307 `SOUKO`, 308 `SOUKO ELEVATOR`, 508/509/50E/50F leer | sicher |
| Gator-Code Disc = Mod | `reg.py`: 0x22/0x23 nur STAGE2; Bereich 0x8010c080..0x80115750 und Daten 0x80118b98..0x80118d60 gleich; Gesamtdiff 325 B | sicher |
| Gator-Details | Routinen @0x80118bc8, Kasten {1000,-720,0,2200,720,800}, HP 16x300, INIT a3=0x15 @0x8010c760, Clip 20 @0x8010dcec, Debugtext @0x8010c470 | sicher |
| Kein Ton im Gator-Code | `tiefe.py`: Aufrufbaum Tiefe 3, 54 Funktionen, kein 0x800453d0/0x80045024/0x80041624 | sicher (ohne jalr) |
| Keine Tuer nach 208/209/20A | `door.py`: Disc-Ziele in Stufe 2 nur 200, 201, 202, 203, 207 | sicher |
| Mod-Block 1922 = RE2-EM23 | Meshes 7..13 = EM23 1..7 (63/33/26/26/55/169/206) | sicher |
| Videos 307 = D0-Gestalt | CD3 0:43-0:52 (blasse Haut, lange Jeans, Schuhe), B 3:11 (Menschengesicht), B 13:50 (heller Ruecken, lange Jeans) | sicher fuer die Gestalt |
| 50A = D0-artig | CD4 0:51-0:57: helle Haut mit roten Flecken, blaue Jeans, Stange; Beinlaenge nicht ablesbar | wahrscheinlich |
| Bahnsteig #143/#145 keine Menschengestalt | A 1:54.5/1:56.1, eigene Lupe; Nutzer-Lesung (Schwanz) | sicher fuer die Bilder |
| Kein Alligator in den Videos | eigene Grobsichtung B (alle 6 s), CD2-Bogen, B 7:43/10:37/12:51 = brennende Zombies | sicher fuer die Stichprobe |
| Mod-Kamera in `buehne.py` | ROOM50A0 Kamerageometrie Disc = Mod (nur Zeigerwort +40) | sicher, unschaedlich |
| Header | STAGE*.BIN ohne Header: `R0:%x` @Datei 0x194, `sw v0,0x2c38(at)` @0x80116f64 nur ohne Header | sicher |

## Widerlegt oder berichtigt

1. **"In STAGE5/6 kein Birkin"** (`birkin_modelle.md` 1.1) war nur in den RDTs gesucht. Die Disc-STAGE5.BIN registriert Typ 0x30
   (`sw v0,0x2c6c(at)` @0x8011dd68 -> 0x80116a44). Ihr Modul 0x80116a44..0x8011bf38 ist das STAGE3-Modul, um 0x814 verschoben
   (`cmp35.py`: 5437 Woerter, 0 echt verschieden). Ein Birkin in Stufe 5 war im 40%-Code vorbereitet und haette D0/D1 geladen. Sicher.
2. **"M1 frei erfunden" / "Bytes aus RE2 G2/G3"** (`birkin_videos.md`): Zu G3 gibt es keinen Mesh-Treffer, und die 16 G3-Kacheln liegen
   im Grundrauschen (G4 11, G1 6). M1 besteht aus D0, G2 und neuen Teilen. Sicher.
3. **"Meshes 3/6 identisch mit G2, Beine +-1"** (`birkin_videos.md`): Es sind <= 1 Einheit (6/16 Punkte exakt) bzw. <= 2 Einheiten. Sicher.
4. **"D0 sicher (Kopf mittig, M1 hat keinen)"** bei B 3:11: Die Begruendung ist falsch, denn M1 hat das D0-Kopfmesh 9 mittig.
   Das Urteil haelt wegen des Menschengesichts. Sicher.
5. **"Raumtabelle Disc = Mod"** (`alligator_40.md`): Die Mod-EXE ueberschreibt 0x8007097e..0x800714ba (Stufe 3-5). Die WANI-Eintraege sind gleich. Sicher.
6. **"Biss = Umwerfen", "HP 300"** standen ohne Adresse; Belege nachgetragen. Sicher.
7. Versehentlich Mod statt Disc gelesen? Nur in `buehne.py` (50A-Kamera aus der Mod), und dort ohne Folge. shared_assets statt Disc: nirgends.
   Fan-Modell als Beleg: nicht; M1 wird ueberall als Mod gefuehrt.

## Offen

- 50E (CD4 0:20.9-0:26.2): Modell im Gruenlicht nicht bestimmbar.
- 50A: Jeanslaenge bei 320x240 nicht ablesbar; Farbmessung nur wahrscheinlich (80%-Licht unbekannt).
- Video-D0 heisst D0-Gestalt. Ob der 80%-Build bytegleich D0 nutzt, ist aus Video nicht pruefbar.
- Wozu Typ 0x22 (leeres Geruest in STAGE2) gedacht war.
- Port-Kommentar `enemy_ai_boss_gator.c:267` nennt ENEMSE-Bank 17 fuer EM23; `alligator_40.md` sagt Bank 22. Nicht gegengeprueft (Port, nicht Frage).
- Die drei Videos in `documentation/` (`documentation.mp4`, `missing_boss1/2.mp4`) sind Port-Aufnahmen des Nutzers (Mod-Hintergruende,
  RE2-Alligator) und kein 80%-Material; zu Recht nicht verwendet.
