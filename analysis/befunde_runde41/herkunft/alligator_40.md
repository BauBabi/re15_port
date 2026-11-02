# Alligator auf der 40%-Disc (Dossier, fortlaufend)

Stand: abgeschlossen 2026-11-01 (Zwischenstand 3 + Antwort unten). Neueste Erkenntnis steht unten. Quellen getrennt: DISC = Version_40%/, MOD = info/Re1.5/ (Magic Zombie Door),
PORT = re15_port/, RE2 = re15_port/shared_assets/RE2/ + info/re2leon/. Werkzeuge: alligator_werkzeuge/ (dd.py, reg_scan.py, ems_inv.py, iso_inv.py).

## Zwischenstand 1 (erste Messungen)

- KI-Code liegt auf der DISC. STAGE2.BIN registriert Typ 0x23 @0x80116f64 -> Wurzel 0x8010c448, Zustandstabelle @0x80118bc8, Kasten @0x80118b98
  = {1000,-720,0,2200,720,800}. Bereich 0x8010c080..0x80115750 und Daten 0x80118b98..0x80118c80 sind DISC == MOD (Bytevergleich). sicher.
- MOD-Unterschied STAGE2.BIN gesamt 325 Bytes: vier nop-Patches (@0x80106ad4/c34/da0, @0x80107b68) und Text ab 0x8011713c. Kein Byte im Alligator-Code.
- Modell: DISC CDEMD0/1.EMS haben genau 25 Bloecke, alle im TOC. Der "Alligator-Block 25 Sektor 1922" aus Commit e8fe868f6 steht NUR in der MOD-EMS
  (MOD 4732928 B, DISC 4048896 B). Der Port-Kommentar "liegt als Block 25 in CDEMD0.EMS" gilt also nicht fuer die Disc.
- Abbild: 52849 Sektoren, 334 Dateien, 333 gleich zum entpackten Baum, CAPCOM.STR fehlt nur im Baum. Frei: nur Sektoren 18-21 und 150 Nullsektoren am Ende. ZNULL.DAT = 37044000 Nullbytes.
- ROOM2090.RDT (DISC, 102044 B, 26.10.1996): 11 Kameras, 3 Objekte, keine Tuer, kein Sce_em_set.

## Zwischenstand 2

- Mod-Block 25 ist KEIN Capcom-RE1.5-Modell: Meshes 7..13 haben dieselben Vertexzahlen wie RE2 EM23 Mesh 1..7 (63/33/26/26/55/169/206), Bestskala 0,195-0,207, Abstand 5-8. Dazu README Z.91 + Z.41.
- Null Treffer fuer RE2-EM23-Vertexfolgen auf der ganzen Disc. 25 EMD + 1 Fehltreffer im EMS, keine EMD ausserhalb EMS/PLD/PLW, keine fremde MD1, keine Alligator-Textur in 62 RDT-TIMs und 25 EMS-TIMs.
- Tonbank: ROOM2090 Bank 1 ist leer (1 stilles Sample), Bank 0 = allgemeine Kanalbank. RE2-ENEMSE-Bank 17 und ROOM40A0: 0 Bloecke auf der Disc.
- GROSSER FUND: PSX.EXE (Disc + Mod gleich) enthaelt die Raumtabelle @0x8006ff34 (Index stage*1274 + raum*26, Code @0x8001d39c-c8): 208 = "WANI BOSS ENT", 209 = "WANI BOSS BEYA", 20A = "WANI BOSS BEYA", 207 = "KUMO TSUURO", 201 = "ZONBIE TSUURO". WANI = Krokodil/Alligator. Damit benennt Capcom selbst Raum 209 als Alligator-Bossraum.
- KI verlangt 22 Teile: Schleife @0x8010c7d8-848 (a3 = 0x15 ... 0) multipliziert die Knochenversaetze +0x2c/+0x30/+0x34 mit 2,5. RE2 EM23 INIT @0x801005c8-e0 laeuft ebenfalls ueber 0x16 Teile.

## Zwischenstand 3 (Fortsetzung 2026-11-01 17:30, alle Werte neu gemessen)

### Korrekturen an Zwischenstand 1/2
- Sektoren 18-21 sind NICHT frei: PVD (Sektor 16) nennt dort die ISO-Pfadtabellen (L 18, L-opt 19, M 20, M-opt 21, Groesse 206).
  Freie Daten im Abbild damit: nur 150 Nullsektoren 52699-52848. ZNULL.DAT = 37044000 B, alle Null (nachgezaehlt). sicher.
- RE2-Alligator-Tonbank war falsch zugeordnet (Bank 17 = Typ 0x16). Richtig laut RE2-Paartabelle @0x800a7400 (RE2-EXE Datei 0x97C00):
  Bank 22 = (0x23,0), 62 = (0x23,0x1f), 63 = (0x23,0x20). Neu gesucht (vag_gator.py, jede Byte-Lage, alle 332 Disc-Dateien):
  ENEMSE 22/62/63 je 0 Bloecke, RE2 ROOM40A0-Bank 0 Bloecke. Ergebnis bleibt: kein RE2-Alligator-Ton auf der Disc. sicher.

### KI (DISC STAGE2.BIN, gleich MOD an allen Stellen unten)
- Nur STAGE2 registriert 0x23 (reg_scan.py, alle 6 Overlays Disc+Mod): sw @0x80116f64 -> 0x8010c448. 0x22 -> 0x8010c080. sicher.
- 0x22 ist ein leeres Geruest: Routinen-Tabelle @0x80118a20 = 8 Eintraege, alle Blaetter "jr ra" (0x8010c0f4, Tabellen @0x80118a40/a80
  je 16 Stubs, @0x80118ac0/b18 je 22 Stubs). Kein EMD (TOC 0), kein Sce_em_set. sicher. Wozu es gedacht war: offen.
- 0x23-Aufbau: Routinen @0x80118bc8 = {INIT 0x8010c56c, AKTIV 0x8010c860, TREFFER 0x8010e570, TOD 0x8010e9e8, 4/5 0x8010ec3c, 6 0x8010ee38,
  LEICHE 0x8010eca4}. 4/5 verteilen auf 4 Subs @0x80118d1c, alle "jr ra"; 6 = "jr ra". Also kein Halte-/Fressgriff. sicher.
  AKTIV = Entscheider A @0x80118be8 (nur 0,2,4,6 mit Code) + Handlung B @0x80118c28 (0..10 mit Code, 11..15 "jr ra").
  Treffertabelle 22 Zeilen @0x80118c6c (0x8010e5d8 / 0x8010e748), Todestabelle 22 Zeilen @0x80118cc4 (alle 0x8010ea30). sicher.
- HP: Zeile @0x801175dc = 16 x 300 (Disc = Mod). sicher.
- 22 Teile: INIT-Schleife a3 = 0x15 @0x8010c760, a2 = 0xe1c = 21*172 @0x8010c7d8, Schritt -172 @0x8010c7f8, schreibt die Gelenkversaetze
  Teil+0x2c/0x30/0x34 als x*5>>1 = x2,5 zurueck (@0x8010c814/830/84c). Dieses Muster gibt es in keinem anderen Overlay (Disc+Mod). sicher.
  Deutung: die KI erwartet ein 22-Teile-Skelett, dessen Gelenke um 2,5 gestreckt werden. offen.
- Clips (+0x94; clips.py; Tabelle @0x80118d44 = {14,14,15,_, 8,8,9,_, 17,16,16}, Helfer 0x8010ee40, a1 @0x8010d050/dde4/df8c):
  INIT 0 (@c77c, Land) / 4 (@c590, Wasser); B0: 0; B1: 7; B2: 2,1; B3: 14/15; B4: 4,3; B5: 5; B6: 6; B7: 20 (@dcec); B8: 8/9; B9: 17/16;
  B10: 2,4; TREFFER: 10 (@e650), 11/12 (@e804/e828); TOD: 13 (@eaa8). Verlangt 19 verschiedene Clips, hoechster Index 20 -> Modell braucht
  mindestens 21 Clips. RE2 EM23 hat 12 (emd_tab.py). sicher.
- Toene: im ganzen Bereich 0x8010c448..0x8010ee9c kein Aufruf von 0x800453d0 (Gegner-Ton; Hund 20x, Kraehe 18x, Spinne 0x25 9x, Zombie 44x),
  0x80045024 oder Se_on 0x80041624 (jals.py). Die KI spielt keinen einzigen Ton. sicher.
- Entwicklungsspur: Wurzel ruft @0x8010c470 den Debugtext-Drucker 0x800279c8 mit "R0:%x" (STAGE2.BIN @0x80100194) und +0x4 (Routine).
  In allen 6 Stage-Overlays der einzige Aufruf, sonst nur TITLE.BIN (dbgprint.py). sicher. Deutung "in Arbeit": wahrscheinlich.
- Erreichbar? Nein. 360 Sce_em_set auf der Disc, Typ 0x22/0x23: 0 (emset.py). TOC 0x23 = 0 in CDEMD0 (EXE-Datei @0x637d0) und CDEMD1 (@0x63a10).
  Kein Raum fuehrt nach 2090 (tueren.py: keine Door_aot_set mit Ziel Stage-Index 1, Raum 08/09/0A). sicher.
- Vergleich RE2 (info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN): RE2 hat 4 Routinen (@0x8010461c), Disc 8; RE2-INIT laeuft auch ueber 0x16 Teile
  mit Schritt 172 (@0x801005d8/e0), skaliert aber nicht. Konstanten (konst.py): Gator-40 teilt 37 von 88 mit RE2-EM23 (Jaccard 0,14),
  aber 48 mit dem RE1.5-Hund (0,28). Die kennzeichnenden Werte 2200/720 (Kasten), 6000 (Reichweite @0x8010ce34), -1200 (Wasser-Y), 2644/1612
  (Kiefer-Teilversatz), 700 fehlen in RE2. Code ist RE1.5-Rahmenwerk, keine Vorstufe des RE2-Codes. wahrscheinlich.
  Gemeinsam: Typnummer 0x23, Abwasser-Stage, 22 Teile. Als Idee Vorlaeufer: wahrscheinlich.

### Raum
- PSX.EXE (Disc = Mod), Raumtabelle @0x8006ff34, Index 1274*Stage + 26*Raum (@0x8001d39c-d3c8): 208 @Datei 0x60cfe "WANI BOSS ENT",
  Start (-25432,-16344); 209 @0x60d18 und 20A @0x60d32 "WANI BOSS BEYA" (Alligator-Boss-Zimmer), Start (-5527,-24012). sicher.
  [Gegenprobe] "Disc = Mod" gilt fuer die Eintraege von Stufe 1, 2, 6, 7. Die Mod-EXE hat die Tabelle von 0x8007097e bis
  0x800714ba (Stufe 3 bis 5, 2103 Byte) ueberschrieben und den Zugriff (@0x8001d3cc ff.) umgebaut. Die WANI-Eintraege sind
  davon nicht betroffen; der Name stammt sicher von der Disc.
- Disc STAGE2 hat RDT nur fuer 2000/2001/2030/2070/2090/20B0/20B1; 208 nur BSS (7 Kameras: Treppe, Schaltraum mit Rechner, Galerie ueber
  einem Schacht), 20A gar nichts, 205 gar nichts. sicher.
- ROOM2090.RDT (Disc): main00 = 3 Obj_model_set, sonst nichts; sub00/sub01 leer. Kein Sce_em_set, keine Tuer, kein Text, kein Se_on.
  Obj0 @(-19600,-1800,-4350) = goldene Sechskant-Uhr (disc_room2090_obj0_uhr_x4.png); Obj1/Obj2 = Flaechen 21600x16200 mit dunkelgruener
  Textur bei Y -12600 und Y -1620. Kamera 05 zeigt an der Wand eine Pegellatte 1..7. Deutung Wasserflaechen/Pegel: wahrscheinlich.
- MOD ROOM2090: 2 Tueren (nach 2080, 20A0), 3 Aot_set, Item, 4 Objekte, 2x Sce_em_set Typ 0x25 (Spinne), kein 0x23. Obj0 der Mod ist
  byte-gleich zur Disc-Uhr (TIM 7ecc5520, MD1 5189d350), nur versetzt nach (5750,-1800,-16300): "Sherry's clock" (README Z.291) stammt
  von der Disc. Obj2/3 = neue Leichen (README Z.99/178). sicher.
- [Gegenprobe] Disc-209 gegen Mod-209: Kamerageometrie (RDT @0x24) 10 von 11 Kameras bytegleich, Kamera 10 nur Flag 1->0,
  die Mod ergaenzt Kameras 11 und 12. Hintergruende: 0 von 11 Disc-BSS-Bloecken (64 KB) in der Mod-BSS, ebenso 0 von 7 bei 208.
  56 von 99 KB-Bloecken der Disc-ROOM2090.RDT stecken bytegleich in der Mod-ROOM2090. Folge: Mods "neuer Raum 209" ist
  Capcoms Disc-Huelle (gleiche Kameras, Uhr, Teile der Daten) mit neu gerenderten Hintergruenden, Tueren und Leichen. sicher.
- README der Mod: Z.68 "Added new room 209 (Alligator Pool)" (17.02.2023), Z.246 neue Hintergruende fuer "Alligator Boss (room 209)" (2020),
  Z.292 "placeholder for ALLIGATOR EXIT (room 20A)", Z.356 Kanalraeume verbunden. Kein Satz zu einem Alligator-Gegner. sicher.
- Texte: TEXTE_40.md hat fuer Stage 2 nur eine Zeile (207, Rolltor). ワニ/鰐/わに/ALLIGATOR/GATOR/WANI in Disc-Kodierung, ASCII, Shift-JIS:
  einziger echter Treffer ist der EXE-Raumname (text_suche.py). sicher.
- Babyalligatoren 205: Disc hat keinen Raum 205. Mod README Z.91 (2023, "Experimental", El Rincon del Lobezno), Z.41 (2025, "Removed broken").
  Heutige Mod-2050/2051: 2x Typ 0x25; Mod-EMD 0x25 = Disc-Block 10 (md5 01d12827). Mod-Block 25 (RE2-EM23-Meshes, Skala ~0,2) hat keinen
  TOC-Eintrag und keinen Verweis in Mod-EXE, BIN, 205x/209x-RDT (Suche nach Laenge 183328 / Sektor 1922). Rest der Babyalligatoren: wahrscheinlich.
  Ihr Typ damals: offen.
- Port: Alligator in 2090 = eigener Spawn + RE2-EM23-Modell + eigener Bossablauf (enemy_ai_boss_gator.c Kopf: "NUTZER-DESIGN"). Nicht Disc, nicht Mod.

### 80%-Videos
- BEFUND.md nennt keinen Alligator. NCC aller Bilder (6 Videos, 2 B/s) gegen 531 Disc- und die Mod-Hintergruende: kein Bild >= 0,80 auf 208/209/20A
  (bestes 0,61, V1 1:49,5 auf 20803; Sichtung v1_104s.jpg: Polizeirevier). Blaetter b1/b2a-c/b4 durchgesehen: Kanalszenen nur Tunnel mit Zombies/Spinnen.
- V1 1:26,5-1:27,2 dunkle Leiter-Einstellung mit hellem Fleck und gruenem Boden (v1_087s_leiterschacht_hell.png): matchTemplate (mt87.py) gegen 1650
  Hintergruende, bestes D30903 0,76, D20900 erst Rang 29 (0,675). Nicht zuordenbar, kein Tier sichtbar. offen.
- Ergebnis: kein Alligator in den 80%-Videos. sicher fuer die gesichteten Bilder.

## Antwort
- MODELL: nein. Disc-TOC 0x22/0x23 = 0, 25 Bloecke alle im TOC, keine EM23-Vertexfolge (vert_suche.py erneut: 0), keine Alligator-Textur,
  kein Alligator-Ton, Abbild ohne versteckte Daten. sicher. Das Mod-Modell (Block 25) ist ein RE2-EM23-Umbau der Mod.
- KI: ja, aber tot. 0x23-Code in Disc-STAGE2 (0x8010c448-0x8010ee9c, 8 Routinen, 11 Handlungen, Land/Wasser, Biss = Umwerfen
  [Gegenprobe-Beleg: `ori v0,zero,2` + `sb v0,0x800aca58` @0x8010d274-7c und @0x8010e118-20; Wert 3 nur hinter `bgez` @0x8010d2a0],
  HP 300 [Beleg: `entity+0x9a = h[0x8011717c + typ*32 + (rand&0xf)*2]` @0x8010c6a0-d4, Typ 0x23 -> 0x801175dc],
  byte-gleich in der Mod. Verlangt mindestens 21 Clips, keinen Ton, druckt Debugtext, wird von keinem Raum gesetzt. sicher.
  Eigene RE1.5-Arbeit im RE1.5-Gegnerrahmen, kein RE2-Code. wahrscheinlich.
- RAUM: ja, als Huelle. Capcom nennt 208/209/20A in der EXE "WANI BOSS". Disc hat ROOM209.BSS + ROOM2090.RDT (Uhr, zwei Wasserflaechen,
  Pegellatte, keine Tuer, kein Gegner) und ROOM208.BSS ohne RDT. sicher. Mod: Tueren, Hintergruende, Leichen, kein Alligator.
- 80%-Videos: kein Alligator gesehen. sicher fuer die Stichprobe.
- Nebenbefund Port: Die Port-KI (enemy_ai_common.c) spielt im Lunge B[3] Clip 3; die Disc setzt dort 14/15 (@0x8010d04c, Tabelle @0x80118d44).
  Nicht geaendert.

## Aenderungsliste (Gegenprobe 2026-11-01, `gegenprobe.md`)

- Raum: "Raumtabelle Disc = Mod" eingeschraenkt (Mod ueberschreibt Stufe 3-5 der Tabelle; WANI-Eintraege unberuehrt).
- Raum: Vergleich Disc-209 gegen Mod-209 ergaenzt (Kameras gleich, Hintergruende neu): klaert README Z.68 "new room 209".
- Antwort/KI: Belege fuer "Biss = Umwerfen" und "HP 300" nachgetragen (vorher ohne Adresse).
- Toene: zusaetzlich Aufrufbaum bis Tiefe 3 (54 Funktionen) ohne Aufruf von 0x800453d0/0x80045024/0x80041624. bestaetigt.
- Header: STAGE*.BIN haben KEINEN Header (Datei = Adresse - 0x80100000): `R0:%x` steht genau @Datei 0x194, die Registrierung
  @0x80116f64 liest nur ohne Header als `sw v0,0x2c38(at)`. Die Offsets dieses Dossiers sind richtig; CLAUDE.md
  ("0x800-Byte-Header") ist fuer diese Dateien falsch.
- Alle uebrigen Kernaussagen selbst nachgemessen und bestaetigt (TOC 0x22/0x23 = 0, Archivabdeckung, Abbild = Baum,
  Registrierung nur STAGE2, Code/Daten Disc = Mod, 325-Byte-Diff, INIT-Schleife, Clip 20, Zensus, Tueren, Mod-Block 1922 = RE2-EM23).
