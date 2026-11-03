# G5-Auftritt nach dem Rausschiessen (Runde 41, Zweig r41/g5auftritt)

Auftrag (Wortlaut Nutzer, 2026-11-02): "Die finale Mutationsstelle beim Endboss trifft es noch nicht ganz. nach dem raus schiessen
von Birkin aus dem Zug, sollst du die letzte Stufe GENAUSO machen wie bei Resident Evil 2. Bei Resident Evil 2 ist es so - man hat
eine Grossaufnahme an seitlich der offenen Stelle des Zuges, und dann kommen da zunaechst die Tentakel vom finalen Birkin G5, und
der zieht ihn dann rein. Schau es dir an!"

Stand: Nachbesserung 3 (Abschnitt 9; Masse in Schnitt 16/17 mit Gewicht 1 gezeichnet, Vorkampf-Puls nach RE2 @0x801044FC; erledigt
8.9 erster Punkt "Masse anders gedreht/hoeher"). Davor Nachbesserung 2 (Abschnitt 8; ersetzt in 2.2 die PORT-WAHL "Lichtsatz 16/17 = Cut 11" durch RE2-Lichtsatz 9/10 und in 7.5 die
offenen Punkte Pistolenlauf, G5-Richtung, Speer-Phase, Uebergang Kampf). Davor Nachbesserung 1 nach Abnahme 1 (Abschnitt 7; ersetzt in
2.1/2.2/3/4 "Leon steht", die Spielerlagen 1400/13873 im Cut-12-Teil und "XA-Ton nicht gebaut"). Belege: `G5_belege/`, `G5_belege/nb1/`,
`G5_belege/nb2/`, `G5_belege/nb3/`.

## 1. Was RE2 tut (selbst gelesen)

### 1.1 Raum, Skript, Ausloeser

RE2 Leon `info/re2leon/PL0/RDT/ROOM7040.RDT` (Stage 7 Raum 04, Zug-Endkampf, `nDoor 0`). Skript-Dump:
`python re15_port/tools/r34n_c/re2_scd_dump.py ROOM7040` (Laengen aus re2_scd_lens.py).

- MAIN sub00 @0x01190 `06 00 0a 01 | 21 04 89 01` If Ck(4,0x89,1): G5-Zweig. @0x011AC `44 00 00 36 00 80 00 27 00 ff 00 83 00 00 00 83 ...`
  Sce_em_set Slot 0 Typ 0x36 auf (-32000,0,-32000) (Parkplatz), @0x011C2..@0x01204 vier Tentakel Typ 0x37 (Slot 1 mit +0x10E = 1).
  @0x01292 `04 09 18 02` Evt_exec sub02.
- sub02 @0x0139A `11 00 0a 00 | 02 00 | 12 06 | 3e 00 0b 02 90 e8` = warten, bis Spieler-X (Member 0x0B) die Marke -6000 passiert,
  dann @0x013A8 `03 00 18 06` Evt_chain sub06.
- sub06 @0x01778 = die Szene. Vor der Armierung (Teil, der im Port durch Rueckstoss/Sturz der Riesenform ersetzt ist): Cut 3 (Hinterwagen),
  Deckenplatte Obj 2 faellt (sub03), Dach-Tentakel Slot 1 auf (-12800,-7800,-23570) mit Member_set(0x17,1) = Clip 13 (EM37 sub15,
  eigener +0x1D4 `lhu v0,0x1d4(s0)` @0x801040f0), Cut 4, Cut 9 @0x01892, Leon rennt von -3800 nach 1400 (Plc_dest @0x018B0 `40 00 05 20 78 05 ee a3`).

### 1.2 Ab der Armierung (Bild 0 = sub06 @0x018DE)

`2e 03 00 | 34 17 01 00` @0x018DA/@0x018DE: Work_set(Gegner 0) + Member_set(0x17,1) = G5 +0x1D4 Bit 0 (Sprungtabelle @0x80011228[23]
-> `sh a2,468(a0)` @0x80055d90). Das gibt [T0] @0x801011b0-b8 frei; [T0] stellt den G5 selbst auf X -9000 / Z -23400 (@0x801011d0/@0x801011d8).

Zeitachse der Szene in Bildern ab der Armierung (Sleep n = n Bilder; Wsleeping wartet auf das XA-Bit `lw v0,-0x428` = 0x800cfbd8,
`andi v0,v0,0x20` @0x80053ac0-d0):

| Bild | sub06 | Befehl | Wirkung |
|---|---|---|---|
| 0 | @0x018DE | Member_set(0x17,1) | G5 armiert |
| 0..1 | @0x018E6 | Evt_next | 1 |
| 1..12 | @0x018F0 | Sleep 11 | 11 |
| | @0x018F4..@0x01900 | Sce_bgm_control x3 | Kampfmusik |
| 12..72 | @0x0190A | Sleep 60 | 60 |
| 72..132 | @0x0191C | Sleep 60 | 60 |
| 132..395 | @0x01920 `59 00 0b 00` + @0x01924 Wsleep | Xa_on(0,0x0B) | 263 (s. 1.4) |
| 395..437 | @0x01926 | Sleep 42 | 42 |
| **437** | @0x0192A / @0x01932 / @0x0193A | Pos_set(6500,0,-23570), Dir_set(0,0x800,0), **Cut_chg(10)** | |
| 437..438 | @0x01944 | Evt_next | 1 |
| 438..775 | @0x01946 `59 00 0c 00` + Wsleep | Xa_on(0,0x0C) | 337 (s. 1.4) |
| 775..807 | @0x0194C | Sleep 32 | 32 |
| **807** | @0x01950 / @0x01956 | **Cut_chg(5)**, Pos_set(11340,0,-23570) | |
| 807..1162 | @0x0195E `09 0a 63 01` | Sleep 355 | 355 |
| **1162** | @0x01962 .. @0x01987 | **Cut_chg(6)**, Pos_set(14850,0,-23570), Dir_set 0x800, **Plc_ret**, Set(1,4,1), **Cut_auto(1)**, Set(2,7,0), Set(1,0x1B,0) | Kampf |

Die Szene endet 36 Bilder nach dem Ende des G5-Intros (Sub 2 dauert 1126 Bilder, s. 1.3): RE2 hat sie genau auf den Auftritt geschnitten.

### 1.3 Was der G5 waehrend der Schnitte tut (Overlay CDEMD0.EMS Typ 0x36, Tabelle @0x801055CC, Dossier befunde_runde6 §3)

| Bild | Zustand | Clip (Bilder) | Ton | Tentakel |
|---|---|---|---|---|
| 0 | [T0] 0x80101164 | 1 | SE 10 | |
| 1..90 | [T1] 0x80101214 | 1 | SE 9 je Arm, SE 11 bei 90 | t=10 Arm 0, t=30 Arm 2, t=40 Arm 3 treten aus (+0x06 = 1 @0x80101240/68/90; EM37 sub8 Clip 14/16/15 je 30 Bilder, scaleX 0 -> 4096 in 16 Bildern @0x80101DAC); t=90 alle vier Beben (+0x06 = 6 @0x801012B8-E0) |
| 91..271 | [T2] 0x801012EC | 1 (180) | | **Root-Spur +7014: der Koerper zieht sich durch die Tuer** (X -9000 -> -1986) |
| 271 | [T3] | 3 | SE 9 | |
| 272..362 | [T4] | 3 | t=30 SE 0, 90 SE 10 | t=30 alle einziehen (+0x06 = 8) |
| 362..472 | [T5] | 3 | SE 0 je Arm, 110 SE 11 | Speer 0x901 an Arm 0/1/2/3 bei t=20/40/45/56 |
| 472..652 | [T6] | 4 (180) | | Root-Spur +3946 (X -> 1960) |
| 652..803 | [T7]/[T8] | 2 (150) | SE 10 | |
| 803..914 | [T9]/[T10] | 0 | SE 9, SE 0 je Arm | Wedeln 0xA01 t=20/35/40/50 |
| 914..1065 | [T11]/[T12] | 2 (150) | SE 10 | 0x40A01 an Arm 0 und 2 |
| 1065..1126 | [T13]/[T14] | 0 | SE 9, SE 10 | Einziehen 0x601, Puls an, dann Sub 1 (Kampf) |

Also: **Schnitt 9 (Bild 0..437) zeigt erst die drei Tentakel (Bild 10/30/40), dann das Hereinziehen (Bild 91..271), das Aufrichten
und das Einziehen der Arme.** Schnitt 10 zeigt die Speere und das Herankriechen (+3946), Schnitt 5 das Wedeln bis zum Kampf.

### 1.4 XA-Dauern (Ton der Szene)

`Xa_on` 0x59 Handler @0x80058028 -> FUN_800129b4(Kanal 0, Nummer): Raum-Stimmen ueber den Zeiger 0x800ce544, den STAGE7.BIN beim
Laden setzt (`lui v0,0x8012 / addiu v0,v0,-0x5ebc / sw v0,-0x1abc(at)` = 0x8011a144, Ladeadresse 0x8011a000) -> Datei
`info/re2leon/COMMON/BIN/STAGE7.BIN` @0x144, 16 Eintraege {Kanal, Block, Laenge}. Datei VOICE07.XAS (`80010818`-Tabelle, Stage-Index).

| Xa | Eintrag (STAGE7.BIN) | Kanal/Block/Laenge | Sektoren | Dauer | Bilder |
|---|---|---|---|---|---|
| 0x0B | @0x186 | 4 / 0 / 84 | 84*16 - 0x1E = 1314 | 8,76 s | 262,8 -> 263 |
| 0x0C | @0x18C | 1 / 0 / 107 | 107*16 - 30 = 1682 | 11,21 s | 336,4 -> 337 |

Ende-Sektor = Start - 0x1E + (Block+Laenge)*16 + Kanal (@0x80012af4-b10), Abbruch bei Lage >= Ende (`sltu` @0x80012e3c); Laufwerksmodus
0xC8 (@0x8009a415: Doppeltempo + XA) = 150 Sektoren/s. Die Suchzeit des Laufwerks ist nicht enthalten. Inhalt: Kanal 4 Abschnitt 1 und
Kanal 1 Abschnitt 1 von VOICE07 (im Repo als bearbeitete Wav unter `info/re2leon/PL0/VOICE/William/VOICE07.XAS[4]_01.wav` / `[1]_01.wav`,
Birkin-Laute). Im Port **nicht gebaut** (s. 6): die Zeiten gelten, der Ton fehlt.

### 1.5 Kameras (RID @0x64, 32 Byte je Schnitt, H = fov >> 7)

| RE2-Schnitt | Datei | Auge | Ziel | H | Bild |
|---|---|---|---|---|---|
| 9 | @0x184 `00 00 b7 73 8c 0a 00 00 8e fc ff ff 84 8e ff ff ...` | (2700,-882,-29052) | (-1584,-4608,-14850) | 231 | **Grossaufnahme schraeg von unten auf die offene Wagentuer (Westende), links die Tuer, rechts die Nordwand** |
| 10 | @0x1A4 | (8946,-936,-25902) | (-1710,-3546,-21690) | 208 | Blick nach Westen auf das Wagenende, niedrig |
| 5 | @0x104 | (14220,-1854,-24606) | (3582,-2196,-23076) | 208 | Blick nach Westen durch den Wagen |
| 6 | @0x124 | (21042,-882,-24390) | (11898,-3150,-23076) | 208 | Blick nach Westen vom Ostende |

Hintergruende: `info/re2leon/COMMON/BSS/ROOM704.BSS`, je Schnitt 0x10000 Byte (Kopf `.. 00 38 01 00 03 00` = derselbe MDEC-Kopf wie RE1.5).
Masken Schnitt 9: sprite.pri-Abschnitt @0xDD8 (bis @0xF2C = Zeiger von Schnitt 10), 2 Gruppen, 32 Masken; Atlas im Chunk, SLD-Laenge
0xCF40 aus `STAGE7.BIN` @0xE6 (Tabelle @0x54 = Zeiger 0x8011a054 -> 0x800cfae8, Zeile 4 = Raum 704, Spalte 9), am Chunk-Ende
`present = 1`, SLD @0x7A00, entpackt 0x8220 Byte (dieselbe Form wie RE1.5 FUN_80021d68).

Projektion der Wagengeometrie in Schnitt 9 (Integer-LookAt wie `re15_camera_build_view`): die Tuermitte (x -5683, z -23482) liegt links
ausserhalb (sx -33), der Nordpfosten (z -21990) am linken Rand (sx 8), die Nordwestecke bei sx 50. Die dunkle Oeffnung im Bild liegt dort, wo
die Massenfront des G5 bei [T0] steht (Ursprung -9000 + 4494 = -4506, Kontaktblatt `G5_belege/re2/re2_kontakt.jpg`): genau dort kommt er herein.
RE2-RVD (`room7040/zone.rvd`): Schnitt 9 und 10 haben nur die Ankerzone (from 9/10 -> 0), keinen Wechselstreifen, sind also reine
Szenenkameras; die Spielkameras des Wagens sind 4 (= RE1.5 Cut 11), 5, 6, 12 (= RE1.5 Cut 13), 14.

Masken Schnitt 9: RE2 liest das Groessenfeld anders als RE1.5. RE2 FUN_80049b1c `lhu v1,-0x2(a2)` @0x80049c10, `bne v1,zero` @0x80049c18:
Feld 0 = Rechteck, Breite/Hoehe folgen (12 Byte, @0x80049c20-28), sonst Quadrat mit Seite = Feld (8 Byte, @0x80049c3c-44). RE1.5 FUN_800392d4:
Rechteck nur bei (Feld & 0xF000) == 0, Quadratseite (Feld >> 12) * 8. Mit der RE1.5-Regel ergaben die RE2-Bytes 30 statt 32 Masken; mit der
RE2-Regel genau 32 und genau 340 Byte. Der Masken-Atlas ist mit RE2s eigenem Verfahren gepackt (FUN_8002bdf4 @0x8002bea0 -> FUN_80010000
Lesezeiger 0x1f800006, FUN_80010084 Zustandsmaschine @0x80010170: `andi s4,0x10` @0x800101ac Literal-Zweig, 0xFF Ende @0x80010200-04,
Literal-Laenge `((s4|0xFFE0)^0xFFFF)+1` @0x800103ac-b4, Rueckverweis `((s4&0xE0)|~0xFF)<<3 | b` @0x8001028c-9c, Laenge `s4&0xF (+b bei 0xF)+3`
@0x800102a4-30c); RE1.5s Entpacker (FUN_800c47e8, Port sld_common.c) bricht am RE2-Strom ab (Rueckverweis vor den Anfang). Das entpackte TIM
(33312 Byte, 256x128, 8 bpp) ist byte-gleich mit `ROOM7040009.tim` aus dem fremden Extraktionswerkzeug.

**Kein FMV.** `R704L.STR[0]` ist der Abspann (Zug fliegt durch den Tunnel, Explosion, Morgen), `[2]` das Logo. Der Auftritt ist Engine.

### 1.6 RE2-Wagen = RE1.5-Wagen C

ROOM5090 SCA [21]/[25] (x -5683, Luecke z -24974..-21990) sind die Eckbloecke von room7040 [2]/[4] (befunde_runde3 g5-optik §5); der
G5-Startpunkt -9000 ist daher direkt uebertragbar (enemy_ai_boss_g5.c @0x801011d0). RE1.5 hat aber **keine** Kamera wie RE2-Schnitt 9:
Cut 11 (7626,-3256,-25360) -> (-1504,-986,-21970) blickt frontal aus 13 m auf das Wagenende (Gegenstueck zu RE2-Schnitt 4/10), Cut 12
blickt aus 18526 nach Westen (Gegenstueck 5/6), Cut 15 blickt vom Westende nach Osten.

## 2. Was der Port jetzt tut

### 2.1 Ablauf (engine/src/g5auftritt_r41.c, Kopf include/re15_g5auftritt_r41.h)

Bild 0 ist das KI-Bild der Armierung (RE2 sub06 @0x018DE); der G5 laeuft in diesem Bild [T0] und danach sein unveraendertes Intro
(enemy_ai_boss_g5.c Sub 2).

| Bild | RE2 | Port | Kamera Port | Spieler Port |
|---|---|---|---|---|
| 0 | Set(2,7,1)/(1,0x1B,1) @0x01778/@0x0177C, Cut_auto(0) @0x01780, Schnitt 9 (@0x01892), Member_set(0x17,1) @0x018DE | Sperre + Szenenbalken (re15_cine_active), Zonen aus, Typwechsel + Armierung grid 0x13 | **16 = RE2 Schnitt 9** (RE2-Kamera, RE2-Hintergrund, RE2-Masken) | (1400, -23570), Gier 0x800 |
| 437 | Pos_set(6500) @0x0192A, Dir_set 0x800 @0x01932, Cut_chg(10) @0x0193A | dasselbe | **17 = RE2 Schnitt 10** (RE2-Kamera, RE2-Hintergrund, keine Masken) | (6500, -23570) |
| 807 | Cut_chg(5) @0x01950, Pos_set(11340) @0x01956 | Cut 12 | 12 (RE1.5) | (13873, -23570) PORT-WAHL |
| 1162 | Cut_chg(6), Pos_set(14850) @0x0196C, Dir_set, Plc_ret @0x0197C, Cut_auto(1) @0x01981, Set(2,7,0)/(1,0x1B,0) @0x01983/@0x01987 | Pos_set(13873), Plc_ret wie op_plc_ret (RE1.5 0x80041f88), Zonen an, Sperre aus | Zonenschnitt der Lage (12) | (13873, -23570) |

### 2.2 PORT-WAHL (alle im Code markiert)

| Punkt | Wahl | Grund |
|---|---|---|
| Schnitte 9 und 10 | RE2-Kamera mit RE2-Hintergrund und RE2-Masken als Szenen-Schnitte 16/17 | RE1.5 hat keine Kamera dieser Art; RE2 fuehrt beide nur fuer die Szene (RVD nur Ankerzone). Der Wagen ist an der Stelle zellengleich (SCA [21]/[25] = [2]/[4], Kamera und Masken passen zur Geometrie). Erster Versuch Schnitt 10 = Cut 11: Leon auf RE2s 6500 lag 1126 vor der Kamera rechts ausser Bild, der G5 fuellte das Bild ab Bild ~630 ganz (Lauf leon, Abschnitt 3). Folge: zwei Einstellungen im grauen RE2-Wagen zwischen den orangen RE1.5-Bildern. |
| Schnitte 5 und 6 | RE1.5 Cut 12 | RE2s Spielkameras; Cut 12 (18526 -> Westen) ist RE1.5s Kamera desselben Blicks. |
| Spielerlage ab Bild 807 und am Ende | 13873 statt 11340 (Kamera 5) / 14850 (Kamera 6) | Eine RE1.5-Kamera fuer beide RE2-Einstellungen; 14850 laege im Zonenstreifen 12 -> 13 (RVD @0x4CC) und die Kampfkamera sprange nach Osten. 13873 = Ostkante RVD @0x4F4 (13 -> 12). Zweite Setzung am Ende wie RE2 @0x0196C: sie nimmt zurueck, was Arme und Koerper in den 355 Bildern verschoben haben (gemessen bis 14957). |
| Lichtsatz 16/17 | ~~der von Cut 11~~ -> RE2 Lichtsatz 9/10 (Nachbesserung 2, 8.5) | ~~RE2s Lichtsaetze sind fuer RE2-Figuren~~ - der Satz gehoert zur Kamera; Cut 11 hat keine Lichter (nur Umgebung). |
| Spieler-Gesten | Leon steht (Pose 200) | RE2s Plc_motion/Plc_neck/Plc_cnt (@0x018E7..@0x01912, @0x01940) sind PL0-Clips von RE2-Leon. |
| Teil vor der Armierung | entfaellt | Deckenplatte (sub03), Dach-Tentakel (Slot 1, sub15 Clip 13), Lauf aus dem Hinterwagen: im Port die Rueckstoss-Szene der Form-Spur. |
| Pause vor dem Typwechsel | entfaellt (vorher 45 Bilder BF_RS_PAUSE) | RE2 hat zwischen dem Verschwinden der Masse und der Armierung keine Pause. |
| XA-Ton 0x0B / 0x0C | nicht gebaut | Zeiten gelten (Dossier 1.4), der Port spielt keine RE2-XA. |
| Set(1,4,0)/(1,4,1) @0x0188C/@0x0197D | nicht gesetzt | kein Leser im Port. |
| Szenenbalken | RE1.5-Balken ueber Flag (1,27) | RE2 setzt dieselben Flags (@0x0177C); der Port zeigt sie wie jede RE1.5-Szene. |

### 2.3 Schnittstelle zur Form-Spur (r41/form)

`void re15_bf_g5_auftritt_start(int slot)` (include/re15_g5auftritt_r41.h). **Genau einmal** rufen, sobald die Riesenform aus dem Zug
verschwunden ist; `slot` ist ihr Aktor (Typ 0x30). Danach gehoeren Slot, Kamera und Spieler der G5-Szene; die Form-Spur fasst sie nicht mehr an.
Heute ruft sie die Rueckstoss-Szene in Phase 2 (`birkin_formen.c`, `case 2`), also im ersten Tick nach `e->no_draw = 1`. Im Zweig r41/form
fuehrt Phase 3 (Sturz-Clip 12) in Phase 2; beim Zusammenfuehren bleibt das so, `case 2` dieses Zweigs ersetzt den alten Rumpf (Pause 45,
`re15_bf_morph_zu_g5`, `s_kam_g5`). Die Form-Spur muss nichts aendern; will sie spaeter direkt aus dem Sturzende rufen, ersetzt sie
`s_rs.phase = 2` durch den Aufruf. `re15_bf_morph_zu_g5` bleibt (der Start ruft ihn).

Beruehrte Stellen fuer die Integration: birkin_formen.c (Include, `re15_g5a_reset` in `re15_bf_reset`, `re15_g5a_tick` statt des
`s_kam_g5`-Blocks, `case 2`), r41/form aendert in derselben Datei die Zeilen um `case 1`/`case 3` und `re15_bf_riesenform_raum` (kein
Ueberlapp). test_birkin_szenen.c: hier nur die Kamera-Erwartungen nach dem Morph (Zeilen nach `CHECK(versteckt, ...)`), r41/form aendert
davor.

## 3. Sichtbeleg (echtes Fenster)

Laeufe: eigene exe-Kopie, gdigrab `hwnd=` der eigenen PID, 10 Bilder/s, kein AUTOSHOT, kein SOFTWARE_RENDER, kein Messhaken. Eingabe wie
`integration_r41_birkin_pistole` (Debug-Sprung, Pistole, Zielen und Feuer gehalten); die Rueckstoss-Szene kommt aus dem echten Kampf.
Debug-Protokoll beider Laeufe: `[g5auftritt] F1558 ... Armierung, Schnitt 16`, `Schnitt 16: Hintergrund RE2/R704_CUT09.BSS rv=0, Masken-Atlas
33312 Byte`, `Schnitt 17: ... R704_CUT10.BSS rv=0`, `F2720 Szenenende Bild 1162: G5 Sub 1 Ph 1, Spieler x 13873, Cut 12`.

| Datei | Inhalt |
|---|---|
| `G5_belege/re2/re2_kontakt.jpg` | RE2: dieselben 14 Schluesselbilder, RE2-Kamera + RE2-Hintergrund, Marken fuer Leon (RE2-Pos_set), Massenfront und Tentakelspitzen (Simulation des RE2-Overlays). Werkzeug `tools/r41_g5auftritt/re2_kontakt.py`, Spur aus `RE15_G5A_SPUR=<pfad> test_r41_g5auftritt` |
| `G5_belege/port_leon.jpg`, `port_elza.jpg` | Port 5090 Leon / 5091 Elza, dieselben Bilder (`tools/r41_g5auftritt/port_kontakt.py`, Schnittkanten im Video gemessen) |
| `G5_belege/vergleich_re2_port.jpg` | Zeile je Schluesselbild: RE2 / Leon / Elza |
| `G5_belege/port_*_bild0045.jpg`, `_bild0180.jpg`, `_bild0560.jpg` | Einzelbilder: Arme draussen, Koerper kommt herein, Schnitt 10 |
| `G5_belege/verworfen_schnitt10_als_cut11.jpg` | erster Lauf mit Cut 11 fuer RE2-Schnitt 10: Leon ausser Bild, G5 fuellt das Bild (s. 2.2) |

Zu sehen (beide Figuren gleich): Bild 0 Schnitt 16, die offene Stelle links; ab Bild 12 greift der erste Arm von links ueber den Kopf der
Figur, Bild 45 drei Arme; ab Bild ~100 schiebt sich die Masse unten links herein und waechst nach rechts (Bild 180, 271); Bild 437 Schnitt 17:
Figur rechts, der G5 kommt aus der Tiefe heran (Bild 560, 700); Bild 807 Cut 12: Figur von hinten, G5 vorn; Bild 1162 frei, Cut 12.

Was im Port anders aussieht als in RE2: der RE1.5-Raum bringt seine eigenen Dinge mit, sie erscheinen auch in den RE2-Bildern: das Feuer
des Wagens (RE1.5-ESP am Boden), in 5090 die Frau im Kittel (Typ 0x4D, Spawn @0x122A auf (600,0,-25300), 5091 ohne). Die Szenenbalken sind
die RE1.5-Balken. Der G5 schiebt die Figur in Schnitt 16 zur Seite (1400 -> 834 -> 2712), wie in RE2, wo der Pos_set ebenfalls vor derselben KI
steht.

## 4. Tests

- **neu** `unit_r41_g5auftritt` (`tests/unit/test_r41_g5auftritt.c`): Daten (Chunk/PRI/Kamera gegen die RE2-Bytes, Atlas = entpacktes RE2-TIM,
  RE1.5-Entpacker und RE1.5-Maskenregel lesen die RE2-Daten nachweislich nicht, RE2-Regel 32 Masken mit Groessen 32x40 / 32x32); Ablauf
  5090 und 5091 ueber die Schnittstelle: Szenenbild = KI-Bild, Schnitt 16/17/12 an den Kanten 437/807, Zonen aus und Sperre bis 1162,
  danach frei; erster Arm Bild 11 (RE2 [T1] t=10), Hereinziehen ab Bild 104 (> 91), Ursprung durch die Tuer Bild 153, alles in Schnitt 16;
  Armspitzen und Massenfront im Bild von Schnitt 16, Leon im Bild von Schnitt 17; Spielerlagen; Szenenende Cut 12, G5 in Sub 1.
- **geaendert** `unit_birkin_szenen`: nach dem Morph Schnitt 16 statt Cut 11; Zonen frei am Szenenende (Bild 1162) statt mit [T2]; Ende Cut 12
  statt 14 (die Szene setzt Leon selbst). Die Abnahme-2-Gestaltung (Freigabe mit [T2] fuer den frei laufenden Spieler) ist damit ersetzt.
- **geaendert** `integration_r41_birkin_pistole`: `RE15_EXIT_AT` 3100 -> 4200. Gemessen im Fensterlauf: G5-HP 600 bis zum Szenenende F2720,
  dann 540 (F2800), 405 (F3000), 270 (F3200), 135 (F3400) - der Spieler kann 1162 Bilder nicht schiessen.
- Laeufe (ctest, `build/`): `unit_r41_g5auftritt`, `unit_birkin_szenen`, `unit_birkin_bahnsteig`, `unit_birkin_formen` gruen.
  `integration_r41_birkin_pistole` gruen (381 s; Leon und Elza: Rueckstoss 1, G5 1, G5 tot 1), `integration_r41_g5_tod` gruen (458 s; HP 0,
  Tod bei HP 0, Titel), `integration_r41_g5_westen` gruen (321 s; 0 Bildenden in fester Zelle). ⛔ Der Weg-Riegel von g5_westen ("Eingabe wirkt,
  Weg ab F1600 >= 300") misst jetzt in allen drei Laeufen 4955 - das ist die Versetzung der Szene (6500 -> 13873), nicht die Eingabe: F1600 liegt
  heute in der gesperrten Szene. Der Riegel ist damit fuer seinen Zweck leer; er muesste ab dem Szenenende messen und die Tastenfolge um
  1162 Bilder spaeter setzen (Spur R, nicht geaendert).
- Gegenprobe gruen (19/19): `unit_r17_birkin_1zu1_pin`, `unit_r18_birkin_restluecken_pin`, `unit_r20_birkin_push_pin`,
  `integration_r35_zgirl_5090` (alle mit RE15_ENDKAMPF_NUR_G5, ohne Rueckstoss, die Szene laeuft dort nicht), `unit_birkin_ai`,
  `unit_birkin_opfer`, `unit_r41_birkin_wand_*`, `unit_sld_atlas`, `unit_pri_*`.

## 5. Commits (Zweig r41/g5auftritt)

- `6d8e65214` feat(r41-g5auftritt): Szene, Szenen-Schnitte 16/17, RE2-Daten, Riegel, Dossier, Belege.
- Folge-Commit: Dossier-Nachtrag (Testlaeufe, Riegel g5_westen).

## 6. Offen

- **RE2 nicht im Emulator aufgenommen.** Es gibt keinen RE2-Spielstand im Zug (Leon B, Flag 4:0x89 gesetzt). `re2leon.cue` liegt unter
  `C:/Users/mjoedicke/Downloads/ePSXe2018/`; der Weg waere ein RE2-Savestate in einem beliebigen Raum + RAM-Sprung nach Stage 7 Raum 04 mit
  gesetztem Flag. Das Kontaktblatt zeigt RE2-Kamera, RE2-Hintergrund und RE2-Ablauf, die Figuren nur als Marken.
- **XA-Ton der Szene** (Xa 0x0B Kanal 4, Xa 0x0C Kanal 1 aus VOICE07.XAS, Birkin-Laute) fehlt; die Zeiten gelten. Die bearbeiteten Wav
  im Repo (`info/re2leon/PL0/VOICE/William/VOICE07.XAS[4]_01.wav`, `[1]_01.wav`, 48 kHz / 24 bit) sind nicht die RE2-Bytes.
- **RE2-Spieler-Gesten** (Umdrehen, Hochschauen) nicht uebertragen, Leon/Elza stehen.
- **Bild der Szene im RE1.5-Raum:** Feuer und (5090) die Frau im Kittel stehen auch in den RE2-Einstellungen; zwei Einstellungen im grauen
  RE2-Wagen zwischen den orangen RE1.5-Bildern.
- **Kampfbeginn naeher als RE2:** Spieler am Ende 13873 statt 14850 (977 naeher am G5), s. 2.2.
- **PSX-Bau:** die Szenen-Schnitte 16/17 laedt nur die PC-Plattform (bg_pc.c); der PSX-Zweig haette keinen Hintergrund (bekannte PSX-Luecke).

## 7. Nachbesserung 1 (Abnahme 1: NICHT BESTANDEN, Massstab = echte RE2-Aufnahme)

Abnahme: `G5_abnahme_1.md`. RE2-Referenz: echte Aufnahme + 26 Savestates der Abnahme (Spieler `*(0x800cfe14)`, Gegner-Slot i
`*(0x800cfe1c+4i)`, Lage +0x38/+0x3C/+0x40, Routinewort +0x04, Clip +0x14C, Bild +0x14D, Blende +0x14E, Flags +0x1CC, gezeichnetes Bild
+0x178; Arm-Parts `*(e+0x198)`, Stride 172, Winkel +104, Weltlage +92/96/100). Lesewerkzeuge: `G5_belege/nb1/werkzeug/` (re2ss.py,
tent_ss3.py, tent_kf.py, leon_ss.py, edd37.py, pl00_spur.py). EM37-Overlay liegt im RAM ab 0x8010D000 (Datei `CDEMD0_EM37_ai0.BIN`,
Bytes ab +0x200 gleich); Adressen hier wie im ganzen Dossier = Link-Adressen @0x8010xxxx.

### 7.1 Je Mangel: Ursache, Behebung, Messung

| Mangel | Ursache (Adresse/Bytes) | Behebung | vorher -> nachher |
|---|---|---|---|
| M1 Figur in der Grossaufnahme | Port stellte Leon 437 Bilder auf 1400 (Pose Stehen). RE2: @0x018EA `3f 00 0f 80` Plc_motion = Szenenbank +0x190 Clip 15 (12 Bilder, Wurzelspur 0, gelesen RAM s04), @0x01906 `3f 0c 01 04` = Spieler-Routine 4 Sub 0x0C (Tabelle @0x800a8d90[12] = 0x80066498 -> FUN_80066dec(e,+0x180,+0x184)) Clip 1 = Lauf (20 Bilder, Spur 189 -> 4336 = 216,8/Bild); RAM s03/s04/s05 x 5736 / 16308 / 20066 (Ostwand). Schnitt 10: @0x01940 `3f 0b 01 54` = Sub 0x0B (0x80066478 -> FUN_80066dec(e,+0x108,+0x17C) Basisbank) Clip 1 = Rueckwaertsgang (34 Bilder, Spur -65 -> -1908), Flags 0x54: 0x10 Halbtakt (@0x80065d28-3c `andi 0x10`/`andi 0x20`, Kippen @0x80065d6c-78 `xori 0x20`), 0x04 Schleife, 0x40 ohne Blende; Weg aus der Spur (`jal 0x80015e7c` @0x80066e68/@0x80066ed4, `jal 0x80015350` @0x80066f3c/@0x80066f54); RAM s12..s19 6565 -> 16038 (27,5/Bild) | Bild 1..11 steht Leon (PORT-WAHL: Szenenclip ohne RE1.5-Gegenstueck), Bild 12 RE1.5-Plc_dest Lauf (Modus 5, W01 Clip 0, Tempo 200 @0x80073f24) bis zur RE2-Endlage 20066 (PORT-WAHL); Bild 437 Pos_set 6500/Dir 0x800 + Plc_motion(0x0B,1,0x54) mit RE1.5 PL00 Clip 1 (Bytes = RE2-Basisclip 1: 34 Bilder, Spur -65 .. -1908, PL00.PLD dir[0]/dir[1]), Halbtakt ueber den vorhandenen Spieler-Takt (@0x80030670-80), Weg aus der Wurzelspur wie FUN_80015e7c (Vorwert 0 bei Bild 0 @0x80015f04-1c); Cut 12 ab 15646 (PORT-WAHL: RE2-Abstand 14220-11340 = 2880 vor dem Auge, auf Cut 12 x 18526 uebertragen) | Leon in Schnitt 16 bis Bild 436 im Bild -> zuletzt Bild 29 (RE2: aus dem Bild 0,57 s nach der Armierung); Schnitt 17: stand 370 Bilder -> geht rueckwaerts hinaus, zuletzt im Bild 479 (42 Bilder = 1,4 s, RE2 "1-2 s"), x 6500 -> 16964 bis Bild 806 (RE2 s19 16038); Cut 12: hinaus bis Bild 877 |
| M2 gerade Tentakel | Port zeichnete (motion, anim_frame). RE2 zeigt die Pose des letzten Anim-Schritts: FUN_8002959c `sw a2,376(a0)` @0x800295f8 (+0x178 = Bild +0x14D vor dem Vorschub), Vorschub erst @0x80029b28-4c; Austritt sub8: Schritt nur ph2 @0x80101DB8 (a3 0x100) und ph8 @0x80101FB4 (a3 0x200), ph4 @0x80101E24-88 und ph6 @0x80101E94-F9C ohne Schritt (Phasentabelle @0x8010004C). ph3 @0x80101E04-08 `lui v0,0x7`/`sw v0,332(s1)` setzt Clip 0 + 7 Blendstufen, posiert aber nicht. RAM s04/s06: Arm 0 +0x178 -> EDD+0xC80 = Clip 14 Bild 29 = kf 776, Part-Winkel == kf 776 (part2 (343,-41,-1095), part3 (366,289,-634)); Clip 0 = kf 0 = alle Winkel 0 = gerade Stange. Dazu: Port lief 0x37 auch im globalen Advancer (Doppelschritt), Blendstufen (Wort-Byte 2 / a3) fehlten, Kind-Kollision und Spitze nahmen UNGESTRECKTE Part-Lagen (RAM s12 Arm 3 scaleX 2400: \|p3-p0\| = 4833 = 8249*2400/4096 -> gestreckt) | Zeichenbild-Merker (+0x178) im Arm-Schritt, Zeichner und Kettenrechnung nehmen ihn (main.c, tent_posen); kein Schritt in sub8 ph4/ph6; ph3 faellt in ph4 durch (kein Sprung @0x80101E20); Clip-Wort-Blende an allen 21 Store-Stellen, a3 an allen 22 Schritt-Stellen (Adressen im Code); +0x14E-Abbau nach dem Mischen (@0x800299c0-cc); 0x37 in re15_type_self_advances_anim; gestreckte Kind-Kollision/Spitze | Gelenklagen relativ zu Part 0 gegen RE2 s04 (Arme 0/2/3, 123 Vergleiche Bild 80..120): vorher gerade Stange (Clip 0), nachher max. Abweichung 116 Einheiten (mit G5-Richtung 0 waeren es 12, s. 7.5); Leon in Schnitt 17 vorher bis 2400 seitlich geschoben, nachher 233 (RE2 s12->s13: 57) |
| M3 G5 800 zu weit oestlich | (Agent-Befund, nachgeprueft) FUN_80015e7c setzt den Vorwert bei Bild 0 auf 0 (`lbu v0,333(t0)` @0x80015f04 `4d 01 02 91`, `sw zero,524(t0)` @0x80015f14, `sh zero,528(t0)` @0x80015f1c); Clip 1 Bild 0 traegt speed.x -800. Port "verankerte" den Vorwert auf diese -800 | Anker weg, Vorwert 0 bei Bild 0 | Bild 129: -9000 -> -9525 (RE2 s05 -9525), Bild 270: -1986 -> -2786 (RE2 -2786), nach [T6]: 1960 -> 1160 (RE2 1160) |
| M4 Pfosten verdeckt nicht | RE2: Figuren-OT-Index = otz>>3 (@0x80027d60 `srl v0,v0,1` + @0x80027d64 `andi 0xffc`), Masken auf Index = depth (@0x80049dac `sll a0,a0,2`), Masken vor den Figuren eingehaengt (@0x80026770 `jal 0x80049ca8` vor @0x8002689c `jal 0x80027160`), ZSF3 341 (@0x8008d29c). Port nahm die RE1.5-Regel (otz>>4, Figur oben) | RE2-Schluessel depth*32768/1023 fuer die R704-Masken (re15_pri_mask_camera_z_re2, render_pc.c Schalter) | Pfostenmaske depth 100 verdeckt ab vz 3204 statt 6471; Masse links im Bild in 15 von 15 Messbildern hinter dem Pfosten |
| M4b Ein-Bild-Sprung | (Agent-Befund, nachgeprueft) Zeichner posierte nach dem Vorschub: am Clipende anim_frame 0 mit altem Clip; [T3]/[T7]/[T9]/[T11]/[T13] fallen in RE2 im selben Takt in die naechste Phase | G5-Zeichenbild-Merker (+0x178), Durchfaelle | Wurzel-y-Sprung Bild 270: 1329 -> groesster Schritt im Intro 56 |
| M5 Ton | Xa_on @0x01920/@0x01946, Tabelle STAGE7.BIN @0x186/@0x18C, rohe Sektoren `info/re2leon/PL0/VOICE/VOICE07.XAS` (bytegleich Disc). Sce_bgm_control @0x018F4..: Handler @0x80057e20 -> FUN_8005b6f0, wirkt auf MAIN11 Programme 0/1 (Lautstaerke 127 -> 79, RAM s02->s03 genau 4 Bytes), der dritte ist No-op (SUB-Platz 0xFF @0x8005b74c) - kein Stueckwechsel, in der Aufnahme nicht hoerbar | Xa 0x0B Bild 132, Xa 0x0C Bild 438 aus R704_XA0B/0C.XAS (Kanal 4 / 1 bytegleich geschnitten, tools/r41_g5auftritt/re2_xa_schnitt.py), mono-Dekoder; Musik: PORT-WAHL keine Aenderung (RE1.5 spielt SUB_17, MAIN11 laeuft im Port nicht) | Birkin-Laute hoerbar ab Bild 132 / 438 (RE2: +4,44 s / mit Schnitt 10) |
| M6 Frau/Feuer | - | PORT-WAHL: 0x4D in 16/17 no_draw, ab Cut 12 wie vorher; Raum-ESP (17 Sce_espr_on main00 @0x0110E..@0x0120E) in 16/17 nicht gezeichnet (main.c pc_draw_effects) | Frau in 16/17 0 Bilder sichtbar |
| M7 Vorlauf | RE2 sub06 @0x0187C Pos_set(-3800), @0x01884 Dir_set 0, @0x01892 Cut_chg(9), @0x018A2 Plc_neck, @0x018AC Sleep 20, @0x018B0 `40 00 05 20 78 05 ee a3` Plc_dest, @0x018B8-C3 Warten auf Ck(5,0x20), @0x018C7 Plc_motion, @0x018CC Plc_neck, @0x018D6 Sleep 11 | Vorlauf vor der Armierung: Schnitt 16, Leon von -3800 per RE1.5-Plc_dest (dieselben Bytes) nach 1400, Nacken wie RE2, Sleep 11; G5 bis dahin Typ 0x36 unarmiert auf dem RE2-Parkplatz (-32000,0,-32000) (@0x011AC) | Vorlauf 0 -> 59 Bilder (RE2 61,47 -> 63,80 s = 70 Bilder) |
| M8 Laenge | RE2 echt 15,84/13,12 s = Bildzahl + CD-Standbilder (1,10 s vor Schnitt 10, 0,74 s vor Schnitt 5, Agent-Messung) | Bildzahlen 437/807/1162 bleiben (Lesefenster); Vorlauf +59 Bilder | Gesamtdauer ab Schnitt 9: s. 7.3 |
| M9 Paket-Gate | `release/make_package.sh` | R704_CUT09.BSS/.PRI/_PRI.TIM, R704_CUT10.BSS/.PRI, R704_XA0B/0C.XAS in der Pflicht-/cmp-Liste | - |
| M10 Lesen hinter der RID-Tabelle | main.c `[pri]`-Zeile und `[F1700-leon-root]` lasen active_cuts[16/17] | Szenenschnitt -> RE2-Satz bzw. 0 | `pri_offset=0x26DE0E74` -> 0 "(RE2-Szenenschnitt)" |
| M11 g5_westen zweckentleert | Messung ab F1600 lag in der Szene | Messung ab Szenenende (state.log pm 2 -> !=2), Tasten 41 s spaeter, EXIT_AT 3540 | s. 7.4 |
| M12 PSX | bg_pc.c laedt 16/17 nur auf PC | unveraendert, offen | - |

### 7.2 PORT-WAHLEN (neu oder geaendert, alle im Code markiert)

| Punkt | Wahl | Grund |
|---|---|---|
| Bild 1..11 | Leon steht | RE2-Szenenbank-Clip 15 (Bank +0x190 = RDT, 12 Bilder, ohne Weg) hat kein RE1.5-Gegenstueck |
| Lauf ab Bild 12 | RE1.5-Plc_dest Lauf bis x 20066 | RE2-Lauf ist Bank +0x180 Clip 1 mit Wurzelspur; RE1.5 W01 Clip 0 laeuft auf der Stelle mit Tempo 200 (@0x80073f24). Ziel = RE2-RAM-Endlage s05..s11 |
| Cut-12-Teil ab Bild 807 | x 15646 | RE2-Abstand vor Kamera 5 (2880) auf Cut 12 uebertragen, Leon verlaesst das Bild nach derselben Zeit |
| Szenenende | x 13873 (unveraendert) | Abnahme 1: vertretbar |
| Frau / Raumfeuer | in 16/17 unsichtbar | RE2 zeigt dort niemanden ausser Leon |
| Musik | keine Aenderung | RE2s Befehle wirken nur auf MAIN11 (Lautstaerke zweier Programme), die der Port hier nicht spielt; in der Aufnahme kein hoerbarer Wechsel |


### 7.3 Sichtbeleg (echtes Fenster, kein AUTOSHOT/SOFTWARE_RENDER)

Laeufe: `G5_belege/nb1/werkzeug/run_port.sh` (eigene exe-Kopie, gdigrab `hwnd=` der eigenen PID, 20 B/s, Eingabe wie
integration_r41_birkin_pistole, EXIT_AT 3000). Protokoll beider Laeufe: `F1558 Vorlauf, Schnitt 16`, `F1613 Armierung nach 55
Vorlaufbildern, Spieler x 1208`, `F2775 Szenenende Bild 1162: G5 Sub 1 Ph 1, Spieler x 13873, Cut 12`. Zuordnung Videozeit <-> Spielbild
ueber die Schnittkanten (Farbstich RE1.5 orange / RE2 grau; `zeitachse2.py`, Kante 16 -> 17 vorhergesagt und im Video getroffen).
RE2-Zeit: Bild 0 = 63,48 s (nicht 63,80 s wie in Abnahme 1.2): RE2-RAM s03 (G5 +0x158 = 31 -> Bild 32) Leon x 5736 = 1400 + 20 * 216,8,
also Laufbeginn Bild 12 (@0x01906); er verlaesst das Bild bei 64,37 s mit x ~4600 -> Bild 27 -> Bild 0 = 63,48 s. Der in Abnahme 1 bei
63,80 s gesehene "Laufbeginn" ist Bild 12. Schnitt 10/5 nach den gemessenen Kanten 79,64 / 92,76 s.

| Datei | Inhalt |
|---|---|
| `G5_belege/nb1/vergleich_re2_leon_elza_nb1_1.jpg` | RE2 echt / Port Leon / Port Elza an Bild -40 (Vorlauf), 0, 12, 30, 45, 100, 180, 240 |
| `G5_belege/nb1/vergleich_re2_leon_elza_nb1_2.jpg` | Bild 265, 271, 360, 470 und 560 (Schnitt 10/17), 840 und 1000 (Schnitt 5/Cut 12) |
| `G5_belege/nb1/leiste_bild0030.jpg` .. `leiste_bild0840.jpg` | Einzelleisten |
| `G5_belege/nb1/port_leon_szene_mit_ton.mp4` | Port Leon, Spielbild 1538 .. 2815 (Vorlauf bis Szenenende), Bild auf Spielzeit umgerechnet, Ton bildgenau aus RE15_AUDIO_CAP_SYNC (eigener Lauf, gleiche Spielbilder) |

Zu sehen: Vorlauf leere Tuer (RE2 zeigt Leon dort schon am linken Rand, Port erst ab ~Bild -25); Bild 0 Leon/Elza in der Mitte;
Bild 30 leer wie RE2; Bild 45/100 drei eingerollte Arme an Rahmen, Decke und Boden wie RE2 (vorher gerade Stangen); Bild 180/240 Masse
links unten hinter dem Pfosten; Bild 265/271 kein Sprung; kein Feuer und keine Frau in 16/17; Schnitt 17 Leon/Elza rechts vorn von
hinten, rueckwaerts hinaus; Cut 12 Figur von hinten, rueckwaerts auf die Kamera zu.
Ton: Kreuzkorrelation des Mitschnitts mit den RE2-Strecken (`ton_versatz.py`): Xa 0x0B beginnt genau bei Spielbild F_arm+132,
Xa 0x0C bei F_arm+438 (Korrelation 0,950 / 0,917).

### 7.4 Tests

| Lauf | Ergebnis |
|---|---|
| `unit_r41_g5auftritt` (neu gebaut: Bildtakt wie main.c, Walker + Spieler-Takt + KI) | gruen, 5090 und 5091: Vorlauf 59 Bilder, Armierung bei x 1200; M1 Leon zuletzt in Schnitt 16 bei Bild 29 (Riegel < 45), Schnitt 17 hinaus bei 479 (Riegel < 527), x 6500 -> 16928 (Riegel 8000..12000 Weg), Cut 12 hinaus bei 877; M2 123 Gelenkvergleiche gegen RE2 s04, max. 116 (Riegel 250); M3 G5 Bild 129 -9525, 270 -2786, 800 1160 (Riegel +-10); M4b groesster Wurzel-y-Schritt im Intro 56 (Riegel 120; vorher 1329); M4 Masse in 15/15 Bildern hinter der Pfostenmaske; M6 Frau 0 Bilder in 16/17, Cut 12 wie vorher; Xa bei 132/438 |
| Alle Nicht-Integrationstests (-j 8) | 1073/1074 mit G5-Richtung 0; einziger Fehlschlag unit_r35_granate -> Richtung zurueckgenommen (7.5), danach Granate gruen und die 31 G5/Birkin/Tentakel/Granate-Tests gruen |
| Geaenderte fremde Riegel | probe_g5_tentakel (Spieler an Part 3 statt an die Spitze; Treffer hing an der rng-Variante), probe_p2_birkin_g5 (Intro-Ende 1160), probe_r20_birkin_push (Arm-Kind-Kollision getrennt gezaehlt, Begehbarkeit weiter fuer alle Bilder), test_r41_birkin_wand (Laufprobe nach 10 statt 30 Kontaktbildern), test_birkin_szenen / test_birkin_bahnsteig (Vorlauf vor der Armierung) |
| Integration (build/, -j 3) | gruen: integration_r41_g5_tod (279 s), integration_r41_g5_westen (446 s, neu ab Szenenende gemessen), integration_r35_zgirl_5090, integration_pri_masken, integration_r36_pri_schatten. **ROT: integration_r41_birkin_pistole** - Szene, Rueckstoss, Mutation, Form-HP wie vorher, aber Leon (45 HP am Szenenende, steht und feuert) stirbt bei ~F3550, der G5 steht dann bei 105 HP (600 -> 510 F2908 -> 300 F3208 -> 120 F3508). Gegenprobe mit 0x37 wieder im globalen Advancer: ebenfalls rot (Tod ~F3400) - der Doppelschritt ist nicht die Ursache. Offen, s. 7.5 |

### 7.5 Offen

- **integration_r41_birkin_pistole rot** (7.4): mit der Nachbesserung ueberlebt der stehende Spieler den Kampf nicht mehr bis zum Tod
  des G5. Kandidaten (nicht getrennt gemessen): gestreckte Kind-Kollision (Arme fahren ihre Segmente beim Strecken durch den Spieler),
  gehaltene Einroll-Posen/Blenden, G5-Lage 800 westlicher (M3), [T14]-Wechsel 4 Bilder frueher. Naechster Schritt: je Aenderung einzeln
  gegen den Lauf pruefen und den Kampf gegen einen RE2-Kampf-Savestate messen.
- **G5-Richtung:** RE2 schreibt sie nur im Ctor, `sh zero,118(s0)` @0x8010044c (RAM 0). Der Port richtet den G5 bei [T0] auf den
  Spieler (im Arena-Fall 0..11, ~1 Grad). Mit 0 lag die Armform bei max. 12 statt 116 Abweichung, aber unit_r35_granate verlor ihre
  RE2-Granatentreffer (Trefferzentrum o94/o96 wechselt an Bit 0x400 der Richtung, re15_damage.c re15_re2_gl_kandidat). Zurueckgenommen,
  Folgespur (Granatentest gegen RE2 pruefen).
- **Speer-Phase (Schnitt 17):** RE2 s13 (Bild ~472) Arm 0 \|p3-p0\| 8170, Part-Winkel p0 rz 266, p3 rz -21; Port 6080 - die Port-Arme
  rollen sich in sub9 staerker ein, einer liegt nah vor der Kamera 17 und erscheint breit. Einroll-Rechnung (sub9 ph5, part_rz) gegen
  RE2 nachrechnen.
- **Uebergang Intro -> Kampf:** ein Wurzel-y-Schritt von 125 bei Bild 1123 (G5-Clipwort-Blende 0x1F im Port nicht gefuehrt).
- **Vorlauf-Ton/Beben:** RE2 sub04 (@0x0169E: Se_on Bank 2 Satz 12, 9x Sce_shake_on) und sub09/11/12 (vorbeiziehende Tunnellichter) nicht
  uebertragen. Vorlauf Port 55 Bilder, RE2 ~70 (RE2-Lauf -3800 -> 1400 mit 216,8/Bild, Port 200 + Ankunftsradius 300).
- **Musik:** RE2s Sce_bgm_control wirkt auf MAIN11; im Port laeuft RE1.5 SUB_17 weiter (PORT-WAHL, s. 7.2).
- **PSX-Bau:** Szenen-Schnitte 16/17, XA-Strecken und Zeichenbild-Merker nur auf PC (bg_pc.c, audio_pc.c, main.c).
- **Laenge:** Port 55 + 1162 Bilder; RE2 zusaetzlich 1,10 s + 0,74 s + 0,28 s CD-Standbilder (nicht nachgebildet).

## 8. Nachbesserung 2 (Auftrag nach NB1: roter Pistolenlauf, G5-Richtung, Speer-Arme, Uebergang Kampf, Licht)

Belege: `G5_belege/nb2/` (Werkzeuge `nb2/werkzeug/`). RE2-Code: `info/re2leon/PSX.EXE` und `build/extracted/re2_ems/CDEMD0_EM36_ai0.BIN` /
`CDEMD0_EM37_ai0.BIN` (Link-Adressen @0x8010xxxx wie oben), Disassembler `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py`.
RE2-RAM: Savestates s01..s26 der Abnahme 1 (Lesewerkzeuge `nb1/werkzeug/`).

### 8.1 Punkt 1: integration_r41_birkin_pistole rot - Halbierung, Ursache, Behebung

**Halbierung** (Schalter `nb2/werkzeug/halbierung_schalter.py` auf 58d3213a6, je NB1-Aenderung ein Bit in `RE15_NB2_AUS`, Leon-Lauf wie der
Test, Kampfprotokoll je Bild; Ergebnis `nb2/halbierung_ergebnis.txt`, Werkzeug `halbierung_auswert.py`):

| aus | Aenderung | Ausgang (Leon, 45 HP bei Kampfbeginn) |
|---|---|---|
| - | NB1 komplett | Peitsche 7 (n1745), Biss 20 (n1781, Abstand 4910, Bogen 0), Biss 18 -> 0 (n1917), gefressen n1940; G5 105 HP |
| 0x01 | gestreckte Kind-Kollision/Spitze (4833) | zwei Bisse (n1509/n1629), Spieler 5 HP, G5 15 HP bei Exit - G5 faellt nicht |
| 0x02 | gehaltene Einroll-Pose (+0x178) in der Kette | tot (wie NB1) |
| 0x04 | kein Schritt in sub8 ph4/ph6, ph3-Durchfall | tot (Bisse n1521/n1645/n1761) |
| 0x08 | Clip-Wort-Blendstufen der Arme | tot (wie NB1) |
| 0x10 | -800-Anker der G5-Wurzelspur | tot (HP 0, G5 15) |
| 0x20 | [T14]-Wechsel / Durchfaelle [T3]..[T13] | tot |
| 0x40 | Skriptlage (Nachklemme) | tot (wie NB1) |
| 0x80 | w1-Kopplung aus der Wurzel | tot (wie NB1) |
| 0x100 | Vorlauf (Szenendauer +55) | tot (wie NB1) |
| 0x200 | Spieler laeuft/geht rueckwaerts (M1, Lagen 20066/15646) | tot (wie NB1) |
| 0x400 | 0x37 nur Eigenschritt | tot |
| 0x1000 | G5-Richtung 0 (Punkt 2) | tot |
| 0x7FF | alle NB1-Aenderungen aus (~NB0) | Spieler 45 HP, kein Treffer; Spieler steht bei z -22151 |

Keine einzelne Aenderung kippt den Ausgang. Gemeinsamer Mechanismus aller roten Laeufe: der stehende Spieler steht im Bissbogen des G5 -
Abstand < 6000 und `arc_test == 0` - und wird gebissen, bis HP 0, dann gefressen. Der einzige gruene Lauf (alle aus) gewann, weil der Spieler
bei z -22151 stand: Bogen 1249 neben der Achse, `arc_test(128) != 0`, kein Biss im ganzen Kampf.

**Woher -22151:** NB0 liess den Spieler am Szenenende nicht auf RE2s Linie. RE2 setzt ihn mit Pos_set @0x0196C `32 00 02 3a 00 00 ee a3`
auf z -23570; im NB0-Lauf (Abnahme 1) sprang er nach der Freigabe auf (13564,-22855) - der G5 hatte die gesperrte Figur in der Szene
verschoben und die Nachklemme nahm die Pos_set-Lage zurueck (NB1 M1/re15_player_skriptlage hat beides behoben). Die Arme des ersten Zugs
schoben ihn dann auf -22151. In NB1 steht er RE2-gemaess auf -23570, der Zug schiebt ihn nur noch in x.

**Was RE2 tut (selbst disassembliert, CDEMD0_EM36_ai0.BIN):**
- Biss-Ausloeser jeden Takt im Puls (Blob 0): `lw v0,496(s0)` @0x80103e00 (Abstand), `sltiu v0,v0,0x1770` @0x80103e08 (Bytes `70 17 42 2c`,
  < 6000), Sperre +0x229 @0x80103e14-1c, `jal 0x80015614` @0x80103e34 mit a3 = 128 @0x80103e38, `bne v0,zero` @0x80103e3c (weg, wenn der
  Spieler AUSSERHALB des Bogens steht). Schaden 40 (a0 @0x80104118, a1 = 1: toetet nie, klemmt auf 0), Fressen bei HP <= 0 @0x80103e78-ec4.
- Der G5 kommt nicht weiter als X 12000: Zug `slti a0,a0,12000` @0x80100fd0 / `beq a0,zero` @0x80100fd4 vor `jal 0x800152c8` @0x80100fe0;
  Biss-Vorstoss nur solange <= 12000 (@0x80104060-94).
Ein Spieler, der auf RE2s Linie vor dem G5 stehen bleibt, wird also gebissen und gefressen - in RE2 wie im Port. Der Test schrieb mit "stehen
und feuern" eine Annahme fest, die RE2 widerspricht und nur durch den NB0-Lagefehler gruen war.

**Behebung (Test, nicht Code):** `tests/integration/test_r41_birkin_pistole.cmake` misst weiter "Pistole besiegt den G5", mit dem Abstand,
den RE2 dafuer laesst: bis F2548 Zielen + Feuer, dann 12 s Taste unten (wirkt erst mit der Freigabe am Szenenende F2775, Spieler geht
rueckwaerts 13873 -> 23113, also hinter X 18000 = 12000 + 6000), dann wieder Zielen + Feuer. Neu geprueft: Spieler-HP im Bild, in dem der
G5 faellt, muss > 0 sein (vorher nur "G5 tot"). EXIT_AT 5400 (G5 faellt bei F3775), Lauf-Zeitlimit 420 s, Test 900 s.
Gemessen (ctest): Leon und Elza "Mutationen 1, Form-HP 150 bis 0, Rueckstoss 1, G5 1, G5 tot 1, Spieler-HP dabei 45"; G5 600 -> -1 in 40
Schuessen a 15, kein Treffer am Spieler. Gegenprobe Endstand stehend (`kampf_lauf.sh`, RUECK 0): Biss 20 bei Abstand 5442, Tod n1933 - wie
oben, also kein Codefehler, sondern RE2s Biss.

### 8.2 Punkt 2: G5-Blickrichtung

RE2: Ctor `sh zero,118(s0)` @0x8010044c (Bytes `76 00 00 a6`). Zensus aller `sb/sh/sw rt,0x74|0x76|0x78(base)` in CDEMD0_EM36_ai0.BIN: nur
dieser und @0x80103a24 `sh v1,118(s1)` (s1 = Spieler beim Fressen). [T0] @0x80101164-0x801011e4 schreibt +0x76 nicht. RAM s03..s26: 0.
Port vorher: [T0] `rot_y = atan2(Spieler) - 0x400` (Arena 0..11; liegt der Spieler bei groesserem z, 0xFF5.. mit Bit 0x400).
Jetzt: Ctor setzt 0, [T0] setzt 0 (enemy_ai_boss_g5.c).

**unit_r35_granate** (fiel in NB1 mit Richtung 0): der Test rief `re15_granate_r35_explosion(p, 0, 2)` - Gier 0 = Wurf VOM Boss weg. RE2s
Explosionskasten liegt in Wurfrichtung: FUN_80041EF8 dreht (Box0, -Box1 - 4*Box3) mit RotMatrix(0,Gier,0) (`sh a1,162(sp)` @0x80041f44,
`jal 0x8008e1f4` @0x80041f74), Box {-2000,0,1000,500} + r1ee/4 -> bei Gier 0 reicht der Kasten von P nur 2000 nach -X. Das G5-Trefferzentrum
liegt bei X + (+0x94 = -2000, @0x80100534/@0x80100538) - mit Richtung 0 also 300 bzw. 1500 + 2000 hinter P, ausserhalb. Getroffen hatte nur
die Port-Richtung 0xC00 (Spieler im Test bei +Z), die mit Bit 0x400 das Zentrum auf (0,-2000) vertauscht. Behebung: der Test wirft wie ein
Werfer vor dem Boss auf ihn zu (Gier 0x800 = Westen, RE2-RAM s12 "Richtung 2048"), fuer alle fuenf Bosse. Gemessen: 184/185 RE15+RE2
"Treffer 1, hp 600 -> 520", ganzer Test gruen; mit Gier 0 und Richtung 0 vorher 4 Fehler (184/185).
Gewinn fuer M2 (unit_r41_g5auftritt, Gelenke gegen RE2 s04): max. Abweichung 116 -> 12.

### 8.3 Punkt 3: Speer-Phase Schnitt 17 (Arme zu stark eingerollt, Reichweite 6080 statt 8170)

Drei Ursachen, alle gemessen gegen RE2-RAM s12 (sub9 ph6) / s13 (sub9 ph7):

1. **Mischen nur im Anim-Schritt (FUN_80029614, RE2 PSX.EXE).** RE2 posiert die Teil-Winkel (+0x68/+0x6A/+0x6C je Teil, Stride 172) und die
   Wurzel (+0x2C..) nur im Schritt und haelt sie bis zum naechsten. Mit Stufen +0x14E (`lbu t3,334(s2)` @0x800296a8): Gewicht
   w = (a3 & 0xffff) * Stufen (@0x800296b8-bc), Stufen - 1 (@0x800299c8-cc), je Teil alt &= 0xfff (@0x800299e0-ec), neu +-4096 auf den kurzen
   Weg (`sltiu 0x1001` / `andi 0x8000` @0x80029a40-a9c), GPF IR0 = w auf alt (@0x800299f0-a3c), GPL IR0 = 4096 - w auf neu (@0x80029aac-ad8),
   Ablage @0x80029ae0-af4; Wurzel ebenso mit der alten Wurzel +0x38 (@0x800296b8-754). Der Port mischte im Zeichner JEDES Bild gegen das
   rohe Zielbild - in sub9 ph3..ph7 (kein Schritt) lief die Pose so gegen das volle Clip-2-Bild (rz 0/483/597/410), RE2 bleibt bei 4 Schritten
   mit 31..28 Stufen und a3 128 (@0x801021D0) auf (0,133,164,112).
   Neu: `re15_g5_pose_schritt` (engine/src/re15_g5_skin.c) haelt die Pose je Slot, `g_anim_pose_override` (skeleton_common.c) setzt sie im
   Pose-Builder ein (Zeichner UND Ketten-/Kollisionsrechnung); `tent_anim` faltet die Einroll-Zuschlaege vor dem Schritt in die Pose
   (`sh` +108/+280/+452/+624 @0x801023e4-24 schreiben in die Teil-Winkel).
2. **Abbruch des Einrollens auf der Winkelsumme der TEIL-WINKEL**, nicht der Zuschlaege: `lh v0,120(s1)`, `lh a1,108(s2)`, `lh v1,280(s2)`,
   `lh a0,624(s2)`, `lh v1,452(s2)` @0x8010245c-74, `andi 0xfff` @0x80102484, Fenster `addiu -2049` / `sltiu 0x7ff` @0x80102488-8c.
   Nachgerechnet in Python aus den RAM-Keyframes (EMR @0x801512f0, kf 0 / kf 2): 7 Einrollbilder, Ergebnis (356,-223,-192,-66) = RAM s12.
3. **Phase 7 fehlte:** [T5] -> [T6] setzt RE2 alle vier Arme auf +0x06 = 7 (Ausrollen/Zittern): `addiu v0,zero,7` @0x80101500,
   `sb v0,6(a2)` @0x80101504/@0x80101510/@0x8010151c/@0x8010152c (Bytes ab @0x80101500 `07 00 02 24 06 00 c2 a0 3c 3c 66 8e ...`), RAM s13
   alle Arme Wort 0x00070901. Im Port blieben die Arme in ph6 stehen (Szenenbild 440..520 gemessen).

Gemessen (unit_r41_g5auftritt, neu M5, 5090 und 5091):

| | RE2 | Port vorher | Port jetzt |
|---|---|---|---|
| Arm 0 sub9 ph6 rz (p0..p3) | s12 (356,-223,-192,-66) | - | (356,-223,-192,-66) |
| Arm 1 sub9 ph6 | s12 (302,-169,-138,-39) | - | (302,-169,-138,-39) |
| Arm 0 sub9 ph7 (+20 Bilder) | s13 (266,-133,-102,-21) | Arm blieb in ph6 | (266,-133,-102,-21) |
| Arm 1 sub9 ph7 | s13 (212,-79,-48,6) | - | (212,-79,-48,6) |
| Arm 0 \|p3 - p0\| | s13 8170 | 6080 | 8143 |

Bild: `nb2/vergleich_nb2_schnitt17_speer.jpg` (Bild 450/472/520/600): die Arme stehen als lange Speere wie in RE2, kein breiter Arm mehr vor
der Kamera.

### 8.4 Punkt 4: Uebergang Intro -> Kampf (Wurzelschritt 125 bei Bild 1123)

RE2 schreibt das Clip-Wort an jeder Stelle ganz, auch bei gleichem Clip, mit Stufen 0x1F (Zensus aller `sw rt,332(base)` in
CDEMD0_EM36_ai0.BIN: [T0] @0x8010116c, [T3] @0x8010138c, [T6] @0x801014f8, [T7] @0x801015d0, [T9] @0x80101618, [T11] @0x80101710,
[T13] @0x80101770, sub0 @0x801009a8, sub1 @0x80100dd8 / @0x80101000, sub3 @0x8010186c, Zucken @0x80102b28, Tod @0x8010306c / @0x801031e4 /
@0x80103430 / @0x80103444; nur das Fressen 7: @0x80101aec+@0x80101b14 und @0x80101ca0-b0). Die Schritte mischen mit a3 128 (sub0 @0x80100a98,
sub1 @0x80100fac/@0x80101020, Intro @0x80101310..@0x801017a8, sub3 @0x8010195c, Zucken @0x80102b78) bzw. 512 (Fressen @0x80101c8c/@0x80101cd8).
Port vorher: `g5_clip` nur bei anderem Clip und Stufen 0 statt 0x1F -> keine Ueberblendung, [T14] Clip 0 -> sub1 Clip 5 sprang die Wurzel um
125. Jetzt: `g5_clip` schreibt immer (Stufen 0x1F, Fressen 7), `g5_anim_a3` posiert ueber `re15_g5_pose_schritt` (a3 je Stelle), Ctor-Pose
Clip 0 Bild 0 (@0x80100590/@0x801005a8). Mitgezogen, weil die Kopplung die gemischte Wurzel liest: w1 aus Teil0+0x30 NACH dem Schritt
([T2] Schritt @0x8010130c vor Kopplung @0x80101338, [T6] @0x80101550 vor @0x8010157c), Kopf-Tracking aus den gehaltenen Teil-Winkeln und
der Zuschlag direkt in Teil1+0x6A (@0x80100354). Zwei Durchfaelle wie M4b: Zucken ph0 @0x80102b18-34 -> ph1-Koerper @0x80102b38 und sub1 ph2
@0x80100ff0-100c -> ph3-Koerper @0x80101010 (Schritt im Eintrittstakt).
Gemessen (unit_r41_g5auftritt): groesster Wurzel-y-Schritt im Kampfbeginn 125 (Bild 1123) -> 14 (Bild 1127); im Intro weiter 56 (Bild 129).
Riegel neu: <= 60. Bild: `nb2/vergleich_nb2_uebergang_kampf.jpg` (Bild 1100..1150, ohne Sprung).
Gegenprobe probe_g5_boss "Zug-Kappe": X 10997 nach 2500 Takten - der Wuerfel (Rueckzug 50 % ab X >= 9001, `andi v0,v0,0x1` @0x80100c8c-b4)
zog zweimal zurueck, weil das Ausrollen (Punkt 3) je Bild drei Wuerfe nimmt. Lauf auf 8000 Takte verlaengert (Begruendung im Test): X 12092.

### 8.5 Punkt 5: Licht der Masse in Schnitt 16

1. **Lichtsatz der Kamera:** RE2 ROOM7040.RDT Kopf [9] = LIT @0x500, 40 Byte je Schnitt im RE1.5-Format. Schnitt 9 @0x668
   `01 00 00 00 a8 a8 a8 be 8c 32 ff ff ff 49 49 49 ac 0d 6c ee 98 a4 68 f7 00 00 34 a4 e4 f3 00 00 fc bd f8 2a 10 27 40 1f`: grau (168,168,168)
   bei (3500,-4500,-23400) Hell 11000, ORANGE (190,140,50) am Boden vor der Tuer (-2200,0,-23500) Hell 10000, weiss bei (-3100,0,-16900) Hell
   8000; Umgebung (73,73,73). Schnitt 10 @0x690: die ersten beiden, drittes aus. Die PORT-WAHL "Lichtsatz von Cut 11" nahm RE1.5 ROOM5090
   LIT @0x55c + 11*40: Skala 2, alle Farben 0, nur Umgebung (83,78,78) - flach. Jetzt: `re15_g5a_licht_einsetzen` legt RE2-Satz 9/10 auf die
   Indizes 16/17 (5090/5091, nach jedem Licht-Parse in main.c / room_common.c; RE15_LIGHT_MAX_CUTS 18), `re15_g5a_licht_schnitt` gibt 16/17.
2. **Auswertepunkt:** RE2 wertet die Punktlichter an der Welttranslation von TEIL 0 aus: FUN_80027160 `jal 0x80076f88` @0x80027330 mit
   `addiu a0,s2,92` @0x80027334 (s2 = Teile); FUN_80076f88 = (Teil0 - Licht) >> 4 inkl. Y, XZ-Daempfung wie RE1.5; FUN_80077200 dieselben Zeilen
   (-Skala * VectorNormal, Farbe << 4). Fuer den G5 liegt Teil 0 auf der Wurzel-y (-3205..-4534) - das Bodenlicht faellt von unten. Port
   (main.c, nur 0x36/0x37 in 5090/5091): Auswertepunkt = Entity + Matrix * Teil-0-Translation statt Entity-Ursprung.
Gemessen (`nb2/werkzeug/licht_mass.py`, mittlere Farbe unten links in Schnitt 16, `nb2/licht_messung.txt`):

| Bild | RE2 echt | Port NB1 | Port NB2 |
|---|---|---|---|
| 180 | R 40 G 20 B 11, orange-hell 1,2 % | R 21 G 11 B 7, 0,0 % | R 46 G 22 B 12, 1,4 % |
| 240 | R 39 G 20 B 11, 1,1 % | R 19 G 10 B 7, 0,0 % | R 48 G 23 B 13, 1,6 % |

(Nur mit Schritt 1, Auswertung am Ursprung: R 61-65, orange-hell 4-5 % - zu hell, das Licht kam waagerecht.)
Bild: `nb2/vergleich_nb2_schnitt16_licht.jpg` (Bild 100/180/240/271).

### 8.6 Punkt 6 (optional): nicht gebaut

Vorlauf-Ton (sub04 @0x0169E Se_on Bank 2 Satz 12), Beben (9x Sce_shake_on) und Tunnellichter (sub09/11/12): nicht uebertragen, offen wie 7.5.

### 8.7 Tests (build/, ctest)

| Lauf | Ergebnis |
|---|---|
| unit_r41_g5auftritt (neu: M4b ueber den Kampfbeginn, M5 Speer gegen s12/s13, Reichweite) | gruen 5090/5091 |
| G5/Birkin/Tentakel/Granate/PRI-Gruppe (27 Tests) | 27/27 gruen |
| integration_r41_birkin_pistole (neu gestaltet, 8.1) | gruen, 399 s (Endstand; Leon und Elza "G5 tot 1, Spieler-HP dabei 45") |
| integration_r41_g5_tod / _g5_westen / r35_zgirl_5090 / pri_masken / r36_pri_schatten | gruen (Endstand, -j 3; westen 411 s, zgirl 148 s, pri_schatten 36 s) |
| Alle Nicht-Integrationstests (unit/prop/probe/sonstige, -j 8) | 1068/1074; rot nur unit_r38_releaseskript_{trocken,ablauf,fehler_bau,fehler_umfeld,cmd} und unit_r39_titelversion_ablauf - je Zeitlimit 1200 s, alle haengen in `df -Pk <pfad>` (release_alles.sh); `df -Pk C:/workspace` haengt auf der Maschine auch allein (> 10 s, gemessen), release/ ist in diesem Zweig unberuehrt. NB1 hatte sie gruen. |

### 8.8 Sichtbeleg (echtes Fenster, gdigrab, kein AUTOSHOT/SOFTWARE_RENDER)

`nb2/werkzeug/run_port.sh` (eigene exe-Kopie, hwnd der eigenen PID, 20 B/s), Zeitachse `zeitachse2.py` (Schnittkanten), Leisten
`leisten_nb2.py`: `nb2/vergleich_nb2_schnitt16_licht.jpg`, `nb2/vergleich_nb2_schnitt17_speer.jpg`, `nb2/vergleich_nb2_uebergang_kampf.jpg`
(je Zeile RE2 echt | Port 5090 Leon | Port 5091 Elza). Protokoll beider Laeufe wie NB1: Armierung F1613, Szenenende F2775 Bild 1162.

### 8.9 Offen

- **Masse in Schnitt 16/17 anders gedreht/hoeher:** Bild 180..271 zeigt die Stacheln im Port frontal, in RE2 schraeg nach rechts; in Schnitt 17
  (Bild 450..520) steht der Kopf im Port hoeher und die Masse groesser. Lage (M3) und Wurzel stimmen; Kandidaten Morph-Gewichte w0/w3 in [T3]..[T5]
  bzw. Teil-Winkel der Masse - nicht gemessen.
- **sub3 (Rueckzug):** RE2 ph0 @0x8010185c-a4 wuerfelt +0x16B (`jal 0x80015fe8`, `sb s0,363(s2)` @0x801018a4) und faellt in ph1 @0x801018a8;
  der Port-Zweig ist vereinfacht (Dreiecksrampe). Nicht angefasst.
- **Spieltest-Strategie:** der Pistolen-Test zieht sich hinter X 18000 zurueck; ob die Arme (Reichweite bis ~X 24700) einen Spieler dort im
  RE2-Zufall treffen koennen, ist nicht gemessen - in beiden Laeufen kein Treffer.
- Punkt 6 (Vorlauf-Ton/Beben/Tunnellichter), Musik MAIN11, PSX-Bau (Szenen-Schnitte, RE2-Licht nur PC-Plattform main.c), CD-Standbilder - wie 7.5.

## 9. Nachbesserung 3 (Abnahme 2: NICHT BESTANDEN, M1 Masse ohne Aufbaeumen, M2 Atmen, M4/M5)

Abnahme: `G5_abnahme_2.md`. Belege: `G5_belege/nb3/` (Werkzeuge `nb3/werkzeug/`, Pfade auf den Scratch-Ordner dieser Sitzung). RE2-Code:
`build/extracted/re2_ems/CDEMD0_EM36_ai0.BIN`, Datei-Offset = Adresse - 0x80100000 (im RAM bei 0x80100000, Datenzugriffe dort mit `lui 0x8010`,
in der Datei mit `lui 0x8011` relokiert). RE2-RAM: Savestates s01..s26 der Abnahme 1 (Lesewerkzeug `nb1/werkzeug/re2ss.py`).

### 9.1 M1 Ursache (gemessen, nicht vermutet)

Sonde `RE15_G5_MORPH_SONDE=<datei>` (main.c, im Zeichner direkt nach `re15_g5_morph_verts`, je Teil: Morphpuffer da, Gewichte, Bbox der
uebergebenen Vertices) und zwei Zeilen im G5-Modul (Ctor und [T0], dieselbe Variable, stderr). Fensterlauf Leon 5090 auf dem Kopf 77479f1f2
(`nb3/messung_sonde_zeichner_vorher_nachher.txt`):

- Der Morphpuffer kommt beim Zeichner an (Teil 2 = nbi 2 -> nmi 2, `mv 1`), aber **Gewicht 1 ist in jedem Bild 0** (`w1ok 0`), die
  gezeichnete Masse hat daher die Bbox der Grundform (y bis 23 statt RE2 -1313).
- `[g5sonde] F1558 Ctor slot 2 typ 36 bank 0 ok -1 pose 0`, `[g5sonde] F1613 T0 w1_ok 0 basis 0`.

Ablauf: `re15_bf_g5_auftritt_start` -> `re15_bf_morph_zu_g5` macht den Typwechsel und `re15_g5_boss_spawn`, der Ctor laeuft im selben KI-Takt
(F1558). Die RE2-EM36-Bank laedt main.c aber erst im Render-Durchgang danach (`pc_enemy_load_ex`, Log `[enemy] RE2 EM036 loaded` hinter der
F1558-Zeile). `g5_posieren` findet im Ctor keine Bank, die Ctor-Pose fehlt; [T0] liest daraus die Basis der Intro-Kopplung, findet nichts und
laesst `s_g5_w1_ok` auf 0 - die Kopplung in [T2]/[T6] schreibt Gewicht 1 nie. `unit_r41_g5auftritt` sah das nicht, weil es die Banken vor dem
Start lud (`g5_banken()`).

RE2 (selbst disassembliert):
- Ctor: Clip-Wort 0 `sw zero,332(s0)` @0x80100590 (`4c 01 00 ae`), Anim-Schritt `jal 0x8002959c` @0x801005a8 (`67 a5 00 0c`) = Pose Clip 0
  Bild 0; die Bank ist in RE2 immer resident.
- [T0] @0x80101164: Clip-Wort 0x1F0001 @0x8010116c, alle vier Gewichte 0 (`sh zero,12/16/20/24(v0)` @0x80101188/94/a0/ac), Basis
  `lw v0,48(a3)` @0x801011f4 (`30 00 e2 8c`, Teil0+0x30 OHNE eigenen Schritt davor), `sh v0,0x5bb0(at)` @0x80101200 (RAM
  `10 80 01 3c b0 5b 22 a4`), Blob 3 `sb v0,536(a1)` @0x80101208, Phase 0 `sb zero,537(a1)` @0x80101210.
- [T2] @0x80101334-68: `lh v1,0x5bb0(v1)`, `lw v0,48(a3)` @0x80101338, `subu` @0x80101340, *4415 @0x80101344-58, `srl a0,v0,11` @0x80101360,
  `sh a0,16(v1)` @0x80101368. [T6] @0x8010157c-b4 (*8830 >> 12).
- RE2-RAM (`nb3/messung_re2_ram_ctor_basis_und_gewichte.txt`, Werkzeug `re2_w1_basis.py`): @0x80105bb0 = 0 in s01/s02 (vor [T0]), -4534 ab s03;
  Teil0.y in s01/s02 = -4534 (Ctor-Pose, Clip-Wort 0x100).

### 9.2 M1 Behebung

`enemy_ai_boss_g5.c` (Haupttick, hinter dem Ctor-Block): solange der Boss seit dem Ctor keinen Schritt getan hat (Sub 2 Phase 0 = vor [T0])
und die gehaltene Pose fehlt, wird der Ctor-Schritt (Clip-Wort 0 @0x80100590, Schritt @0x801005a8 ohne Stufen) nachgeholt, sobald die Bank da
ist; Clip/Bild/Stufen des Aktors bleiben unberuehrt. Damit liegt beim [T0] dieselbe Pose vor wie in RE2. Keine Konstante neu; die Schnittstelle
zur Form-Spur (`re15_bf_g5_auftritt_start`, `re15_bf_morph_zu_g5`) ist unveraendert. Spiel danach: `F1558 Ctor ... bank 0 pose 0`,
`F1613 T0 w1_ok 1 basis -4534`.

Riegel (`unit_r41_g5auftritt`, neu):
- Bankreihenfolge wie main.c: EM36/EM37 fehlen im Takt des Typwechsels (`ok = 0` -> `re15_enemy_find` NULL), ab dem naechsten Bild da.
- Gewichte gegen RE2-RAM an 8 Bildern (s03 31, s04 80, s05 129, s06 165, s08 264, s09 313, s13 479, s17 677): w0, Blob-Phase und w1 bitgleich.
- **Zeichner-Vertices:** die Vertices, die main.c fuer Teil 2 uebergibt (`re15_g5_morph_verts(0x36, 2)`), an Bild 165/313/479/677 gegen den
  RE2-RAM-Vertexpuffer 0x80132be0 (s06/s09/s13/s17): Bbox aller 635 und jeder 10. Vertex, Grenze 4 Einheiten. Tabelle aus den RAM-Abzuegen
  der Abnahme 2 (`nb3/werkzeug/re2_morph_tabelle.py`).
- Gegenproben (`nb3/messung_riegel_gegenproben.txt`): (a) ohne die Nachholung: rot, 8 Fehler - w1 0 statt 566/2895/2865, Zeichner-Vertices
  weichen um 1334-1348 ab (Bbox 1336-1350), Bild 677 (w1 0) passt; (b) Engine-Stand 77479f1f2: rot, 12 Fehler - zusaetzlich w0 774 statt 5796 bei
  Bild 129 usw. (dieselben Werte wie im Fensterlauf der Abnahme, `gw0` 990 bei Bild 165) und Bild 677 um 86 daneben. Mit Behebung gruen:
  Zeichner-Vertices max. 2 Einheiten, Bbox 1.

### 9.3 M2 Vorkampf-Puls (Blob 3 @0x801044FC)

Selbst disassembliert (Werte in `enemy_ai_boss_g5.c` `case 3` je Zeile mit Adresse): Phase +0x219 `lbu v1,537(s0)` @0x80104510, `sltiu v0,v1,0x8`
@0x80104518, Sprungtabelle (Datei-Offset 0x84) = 0x80104540 / 0x80104564 / 0x80104634 / 0x80104720 / 0x80104740 / 0x8010477c / 0x801047cc /
0x801047d8.
- Ph 0 @0x80104540: Ph 1 (Verzoegerungsplatz @0x80104548), `jal 0x80015fe8` @0x80104544, Zeitgeber +0x21C = 30 + rnd&0x1F @0x8010454c-54,
  Tempo +0x21E = 6 @0x80104558-5c, +0x21B = 0 @0x80104560, faellt in Ph 1.
- Ph 1 @0x80104564: w3 halbieren wenn != 0 (@0x8010456c-80), w0 += Tempo (@0x8010458c-98), Tempo += 6 bis 162 solange +0x21B = 0 (@0x801045a8-e4,
  bei 162 +0x21B = 1); ab w0 >= 8501 (`slti v0,v0,8501` @0x801045f8) Tempo -= 6, unter 0 Ph 2 mit Tempo 8 (@0x80104604-30).
- Ph 2 @0x80104634: w0 -= Tempo, Tempo += 8 bis 192 (@0x80104658-94); unter -1048 (@0x801046a8) Tempo -= 8, unter 0 zurueck in Ph 1 (@0x801046b4-dc);
  w0 in [1, 2047] (`lhu`/`addiu -1`/`sltiu 0x7ff` @0x801046e8-f4) -> Ph 6 (@0x80104700); Zeitgeber -1 (@0x80104704-1c).
- Ph 3/4/5 @0x80104720/@0x80104740/@0x8010477c: +0x21B = 7 + rnd&3, w0 -= 2136 bzw. += 712 im Wechsel; in CDEMD0_EM36_ai0.BIN setzt niemand Ph 3
  (Zensus aller `sb ..,537`), gebaut der Vollstaendigkeit halber.
- Ph 6 @0x801047cc: +0x220 = 0, Ph 7. Ph 7 @0x801047d8: w0 = 8000 - 24 * Tabelle[+0x220] (Tabelle @0x80105ad4, 110 x s16, Datei-Offset 0x5ad4,
  `lh v0,0x2ad4(at)` @0x801047ec, *24 @0x801047f4-fc, 8000 @0x80104800), +0x220 + 1, bei 110 wieder Ph 6 (@0x80104810/@0x80104824).
Vorher im Port ("vereinfachte Puls-Variante"): Tempo fest 6, nur Ph 1/2 (w0 774 -> 2820). Nachgerechnet (`nb3/werkzeug/puls_sim.py`,
`nb3/messung_puls_sim_gegen_re2_ram.txt`) und im Riegel gemessen: bitgleich mit RE2-RAM an allen 8 Bildern; die 19 RAM-Werte s07..s25 sind
saemtlich Tabellenwerte 8000 - 24 * t. Spanne der Szene -4288 .. 10692.

### 9.4 Messwerte vorher / nachher (Fensterlauf, Sonde am Zeichner, Leon 5090 = Elza 5091)

| Bild | RE2-RAM w0 / w1 / Bbox-y-max | vorher (77479f1f2) w0 / w1 / y-max | nachher w0 / w1 / y-max |
|---|---|---|---|
| 31 (s03) | 3078 / 0 / - | 186 / 0 / 23 | 3078 / 0 / 23 |
| 80 (s04) | 10296 / 0 / - | 480 / 0 / 23 | 10296 / 0 / 23 |
| 129 (s05) | 5796 / 566 / - | 774 / 0 / 23 | 5796 / 566 / -241 |
| 165 (s06) | -184 / 2895 / -1327 | 990 / 0 / 23 | -184 / 2895 / -1326 |
| 264 (s08) | 9440 / 2865 / - | 1584 / 0 / 23 | 9440 / 2865 / -1312 |
| 313 (s09) | -4168 / 2865 / -1313 | 1878 / 0 / 23 | -4168 / 2865 / -1312 |
| 479 (s13, Schnitt 17) | 8000 / 2865 / -1313 | 2874 / 0 / 23 | 8000 / 2865 / -1312 |
| 677 (s17) | 3152 / 0 / 23 | - | 3152 / 0 / 23 |

(Bbox aus der Sonde ist +-1 gegen die RAM-Bbox: die Sonde rechnet ueber alle 635 Vertices des Mesh, die RAM-Bbox ebenso; Rundung der
GTE-Interpolation, Abnahme 2 3.1 "max 3 Einheiten".)

### 9.5 Sichtbeleg (echtes Fenster, gdigrab, kein AUTOSHOT/SOFTWARE_RENDER)

Laeufe `nb3/werkzeug/sonde_lauf.sh` (eigene exe-Kopie, hwnd der eigenen PID, 20 B/s, Eingabe wie integration_r41_birkin_pistole bis zur Szene),
Zeitachse mit `abnahme2/werkzeug/port_zeit.py` (Kante 16 -> 17 vorhergesagt 85,67 / gemessen 85,65 s Leon, 85,02 / 85,00 s Elza; 29,42 B/s),
Bilder `port_bilder.py`, RE2-Bilder der Abnahme 2.
- Umriss-Probe (`nb3/werkzeug/silhouette_n.py` = Abnahme-Werkzeug plus Beschriftung): `nb3/umriss_leon_bild165/313/479/677.jpg`,
  `nb3/umriss_elza_bild*.jpg`. Gruen = Masse gemorpht (RE2-RAM), magenta = Grundform. Bild 165/313: die Stachelreihe liegt im Port auf der
  gruenen Diagonale wie in RE2 (vorher auf dem magenta Rand); Bild 479: Masse klein und flach im gruenen Umriss; Gegenprobe 677 (Gewicht 1 = 0):
  beide Umrisse passen, RE2 und Port gleich.
- Leisten RE2 echt | Leon | Elza: `nb3/vergleich_nb3_1_bild165_313_479_677.jpg`, `nb3/vergleich_nb3_2_schnitt16_bild180_240_360.jpg` (Stachelreihe
  schraeg nach rechts unten wie RE2), `nb3/vergleich_nb3_3_schnitt17_bild450_472_520_600.jpg` (Masse in 17 klein und flach wie RE2).
- Farbe der Masse (Flaeche wie Abnahme 2, `nb3/messung_farbe_masse.txt`): Bild 180 RE2 35/18/11, Port 31/16/10 (vorher 44/20/12); Bild 240 RE2
  32/16/10, Port 23/12/8 (vorher 45/21/12); Bild 313 RE2 14/7/5, Port 13/6/5. Leon = Elza. Die Messflaeche enthaelt jetzt die gemorphte
  Masse; bei 240 liegt im Port weniger Masse in der Flaeche (dunkler Hintergrund), ein Lichtunterschied ist daraus nicht abzulesen. Licht
  unveraendert (NB2 8.5).

### 9.6 M4 / M5: nicht gebaut, Mechanismus belegt

- **Beben + Geraeusch (RE2 sub04 @0x0169E, gestartet von sub06 @0x0189A `04 06 18 04`):** `5c 00 fd` / `5c 00 fb` (@0x016A4/@0x016A8), Se_on
  `36 02 0c 00 01 ..` @0x016AC, 6 x `5c 00 f9` (For @0x016B8), dann fa/fb/fc/fd/fe/ff (@0x016C4..@0x016D8), je 1 Bild; dazu sub05 @0x016DE
  (8 x Sce_espr_on Effekt 0x19 an der Decke) und sub14 (op8C/op8B). Im Port ist Sce_shake_on ein Leerbefehl (`scd_vm.c` `op_sce_shake_on`,
  `t->pc += 4`), es gibt keinen Beben-Zeichner; RE2-Raumton und RE2-Effekte dieses Raums laedt der Port nicht. Offen.
- **Tunnellichter (sub06 @0x01894 `04 07 18 09`):** sub09 @0x01B20 startet sub11 (0x0B), nach 10 Bildern sub12 (0x0C); beide setzen Raum-Objekt
  3 bzw. 4 auf (0,-4400,-16800) (`32 00 00 00 d0 ee 60 be` @0x01B50/@0x01B9C), ziehen es je Bild um 1000 (`26 00 01 10 e8 03` @0x01B84/@0x01BD0)
  und setzen es an der Marke -20000 (`3e 00 0b 04 e0 b1`) zurueck; sub10 @0x01B2E beendet beide und parkt sie auf (-32000,0,-32000). Im Port
  fehlen die RE2-Raumobjekte von ROOM7040. Offen.
- **M5 Musik:** RE2s Sce_bgm_control @0x018F4..@0x01900 wirkt auf MAIN11 (7.1 M5); der Port spielt hier RE1.5 SUB_17. Eine Uebertragung der
  Lautstaerke auf ein anderes Stueck ist nicht belegt. Bleibt PORT-WAHL (7.2), offen.
- M3 (Kampfstart 13873, PORT-WAHL) und M6 (PSX) unveraendert.

### 9.7 Tests (re15_port/build, ctest)

| Lauf | Ergebnis |
|---|---|
| unit_r41_g5auftritt (neu: Bankreihenfolge wie main.c, Gewichte an 8 RE2-RAM-Bildern, Zeichner-Vertices Teil 2) | gruen 5090/5091; Gegenproben rot (9.2) |
| G5/Birkin/Tentakel/Granate/PRI-Gruppe (28 Tests, -j 6) | 27/28; rot nur unit_r41_birkin_wand_g5 (9.8) |
| integration_r41_g5_tod / _g5_westen / r35_zgirl_5090 / pri_masken / r36_pri_schatten (-j 3) | gruen (282 / 410 / 148 / 0,3 / 29 s) |
| integration_r41_birkin_pistole | Lauf 1 ROT (Leon und Elza: G5 tot 0 bis F5400): ein Arm peitscht den Spieler bei x 23113 (F3576 45 -> 38, F3618 -> 31), Blick 2055 -> 640, mit durchgehend gehaltenem R1 kein neues Heben, also keine Zielhilfe; G5 blieb bei 150 HP. Die Zufallsfolge ist um einen Wurf verschoben (9.8); NB2 8.9 hatte "kein Treffer bei 23113" als ungemessen offen gelassen. Teststrategie angepasst: nach jedem 4-s-Abschnitt `W0.2`, das naechste Heben zielt neu (Latch FUN_8003703c rundum, Radius 0x7530 @0x80032fac, Drehung FUN_8001a8f8 0xC8 @0x80032fe0-ec). Lauf 2 GRUEN (419 s): je ein Peitschentreffer F3807 (45 -> 38), G5 tot F3973, Spieler-HP 38 (`nb3/messung_pistolentest_vorher_nachher.txt`). Skript vom Koordinator abgesegnet (9.10) |
| Alle Nicht-Integrationstests ohne die 6 Release-Skript-Tests (-j 6) | 1065/1066, 945 s; rot nur unit_r41_birkin_wand_g5 (9.8, behoben in 9.10). Release-/Titelversion-Tests nicht gelaufen: df.exe haengt maschinenweit (Abnahme 2, 8.) |

### 9.8 Befund ausserhalb der Spur: unit_r41_birkin_wand_g5 haengt an der Zufallsfolge

RE2 wuerfelt im Puls-Eintritt (`jal 0x80015fe8` @0x80104544); der Port zieht dort jetzt ebenfalls einen Wert aus `re15_engine_rand8`. Alle
spaeteren Wuerfe des G5 verschieben sich damit um einen. Im Fuzz-Riegel `unit_r41_birkin_wand_g5` (Spur R, 424 Startlagen) faellt danach eine
Startlage auf: (2100,-25900), Lage i 14 (vom Test erzwungener Spiess-Griff, Variante 3): der gegriffene Spieler springt je Bild um rund 500 in z
und endet in Bild 30 und 32 in Zelle 30 (`2 Bildenden in fester Zelle`). Gegenprobe (`nb3/messung_wand_g5_alt_plus_ein_zufallswurf.txt`):
Engine-Stand 77479f1f2 mit NUR einem zusaetzlichen Wurf im alten Puls-Eintritt liefert genau dieselben zwei Bilder (Start, Bild, Lage, Zelle,
Boss-x gleich). Ohne den Wurf (Zeitgeber fest) kippt statt dessen die 5091-Lage derselben Startlage (241 Bilder in Zelle 30). Die Luecke liegt
also in der Spieler-Klemme waehrend des Griffs (Spur R, `re15_collision_constrain_re2`, Diagonale #30), nicht im Puls; sie haengt nur an der
Zufallsfolge. Saat des Tests nicht veraendert. Behoben in 9.10 (Auftrag Koordinator).

### 9.9 Commits

- `a73316c4f` fix(r41-g5auftritt): Nachbesserung 3 - Masse mit Gewicht 1 (Ctor-Pose trotz spaeter Bank), Vorkampf-Puls nach RE2, Zeichner-Riegel
  (enemy_ai_boss_g5.c, re15_enemy.h, main.c Sonde, test_r41_g5auftritt.c).
- `352a9a12f` test(r41-g5auftritt): Pistolentest hebt die Waffe nach jedem 4-s-Abschnitt neu (test_r41_birkin_pistole.cmake).
- `28390dd47` docs(r41-g5auftritt): Nachbesserung 3 im Dossier, Umriss-Probe und Vergleichsleisten, Messungen (`G5_belege/nb3/`).
- Folge-Commits: Klemme im Griff-Zweig + Riegel g5saat (game_step_common.c, test_r41_birkin_wand.c, tests/unit/CMakeLists.txt), danach Nachtrag 9.10.

### 9.10 Nachtrag: Klemmen-Luecke im Griff-Zweig behoben (Auftrag Koordinator nach 9.8)

Entscheidungen des Koordinators: (1) Pistolentest-Skript `MA4,W0.2` abgesegnet ("so spielt auch ein Mensch"). (2) unit_r41_birkin_wand_g5 darf
nicht rot in die Integration; Luecke in diesem Zweig beheben, fuer jede Zufallsfolge, Saat nicht aendern, Fuzz-Riegel ueber viele Saaten.

**Ursache an den zwei Bildern (gemessen, `nb3/messung_klemme_griffzweig.txt`).** Zelle 30 ist SCA #30 von ROOM5090/5091: Typ 6, x -425 z -27200
w 5725 d 2000, also die Sued-Diagonale (-425,-27200)-(5300,-25200), fest unter der Linie. Stationen des Spielschritts (Messschiene
`re15_schritt_station_*`) und eine Sonde im Griff-Zweig, Start (2100,-25900), erzwungener Spiessgriff (Opfer-Variante 3):
- Bild 30: Spieler-Klemme S3 (3190,-25588) frei. Der Gegner-Pass setzt den Gegriffenen (Zug 600 @0x80104780) auf (3009,-26098), 98 hinter die
  Linie (S5). Der Griff-Zweig (`grabbed_branch`, Koerper-Schub waehrend des Griffs) schiebt auf (3022,-26104) und klemmt mit der **Platzierung
  (3009,-26098) als Vorlage**. re2_diag6 erkennt "ungeschobene Lage jenseits der Linie" (@0x8004d8dc) und stellt auf die Vorlage zurueck - die
  liegt selbst in der Wand. spieler_klemme schreibt sie als `s_klemme_ende`. Die Nachklemme sieht `pl == s_klemme_ende` und kehrt sofort zurueck:
  Bildende in #30. Bild 32 ebenso (Platzierung (2985,-26127)).
- Bild 27-29: dieselbe Platzierung hinter der Linie, aber kein Koerper-Schub (Abstand zum G5), der Griff-Zweig klemmt nicht, die Nachklemme
  stellt auf (3046,-25646) usw. zurueck. Darum haengt der Fehler an der Zufallsfolge: er braucht Schub UND Platzierung hinter der Linie im selben
  Bild, also eine bestimmte Boss-Lage (Zug-Wuerfel).
- RE2: die Aktor-Klemme FUN_8003567c liest als Vorlage immer +0x118/+0x11a und schreibt sie nach jedem Aufruf aus der aktuellen Lage:
  `lw v0,132(s0)` / `lw v1,140(s0)` / `sh v0,280(s0)` / `sh v1,282(s0)` @0x800356ec-f8 (auch wenn der Klemmaufruf selbst uebersprungen
  wird, `bne` @0x80035698/@0x800356a8). Eine ungeklemmte Griff-Platzierung ist dort nie Vorlage. Der Port nahm sie aus dem RE1.5-Zombiegriff
  ("the anchored placement = the valid pos").

**Behebung (game_step_common.c).**
- Griff-Zweig: im Birkin-Kampf (`re15_birkin_klemme_aktiv`) ist die Vorlage `s_klemme_ende` = Port-+0x118 (letzte geklemmte Lage), nicht die
  Platzierung. Ausserhalb des Birkin-Kampfs unveraendert (RE1.5 FUN_8003b0a4).
- Nachklemme: schreibt danach `s_klemme_ende` = Ergebnis (+0x118 nach jeder Klemme @0x800356f4-f8); vorher blieb die Vorlage des Bildanfangs
  stehen.
Keine neue Konstante. Bild 30/32 enden danach auf (3190,-25588) / (3254,-25563), beide frei.

**Riegel `unit_r41_birkin_wand_g5saat` (neu, test_r41_birkin_wand.c Modus g5saat, CMake TIMEOUT 1200, RE15_ENDKAMPF_NUR_G5=1).** Je Raum 5090 und
5091: 200 Saaten der Engine-Ziehung (`re15_damage_seed_rng`) und der Testwuerfel, je Saat 5 wandnahe Startlagen (Zufall aus 424), Zufallsboss-x
(-1500 .. 12000), Zufallseingaben in jeder zweiten Lage, erzwungener Griff in Bild 10..39 mit Zufallsvariante 0..4, 150 Bilder. Mass: kein
Bildende mit dem Mittelpunkt in fester Zelle. Die Saat des bestehenden g5-Fuzz bleibt 0x0badf00d.
- Mit Behebung: je Raum 1000 Faelle, rund 38 500 Griff-Bilder, **0 Bildenden in fester Zelle**, Laufzeit 22 s.
- Gegenprobe (game_step_common.c auf dem Stand a73316c4f): je Raum **166 Bildenden in fester Zelle in 3 Saaten** (erste Saat 67, Start
  (900,-26200), Peitschen-Griff, Zelle #22) - rot.
- unit_r41_birkin_wand_g5 (Saat 0x0badf00d) mit Behebung: 0 Wandbilder in beiden Raeumen (vorher 2).

**Tests nach der Behebung.**

| Lauf | Ergebnis |
|---|---|
| unit_r41_birkin_wand_rein / _g5 / _griff / _g5saat (neu), unit_r41_g5auftritt und die G5/Birkin/Tentakel/Granate-Gruppe (23 Tests, -j 6) | 23/23 gruen |
| integration_r41_birkin_pistole, _g5_tod, _g5_westen, r35_zgirl_5090, r35_werfer_form, r37_elzaems_leon5080, r37_elzaems_elza1181griff, r34_plattform_takt, r34_plattform_esp_eintritt, pri_masken, r36_pri_schatten (-j 3) | 11/11 gruen; Pistole wie in 9.7 (je ein Peitschentreffer F3807, G5 tot F3973, Spieler-HP 38) |
| Alle Nicht-Integrationstests ohne die 6 Release-Skript-Tests (-j 6) | 1066/1067, 1039 s; rot nur unit_r35_android_bash_mutanten (Bash-Mutanten des Android-Skripts, ohne Bezug); einzeln wiederholt gruen (704 s) - Last |
