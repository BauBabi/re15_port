# G5-Auftritt nach dem Rausschiessen (Runde 41, Zweig r41/g5auftritt)

Auftrag (Wortlaut Nutzer, 2026-11-02): "Die finale Mutationsstelle beim Endboss trifft es noch nicht ganz. nach dem raus schiessen
von Birkin aus dem Zug, sollst du die letzte Stufe GENAUSO machen wie bei Resident Evil 2. Bei Resident Evil 2 ist es so - man hat
eine Grossaufnahme an seitlich der offenen Stelle des Zuges, und dann kommen da zunaechst die Tentakel vom finalen Birkin G5, und
der zieht ihn dann rein. Schau es dir an!"

Stand: laufend (Dossier wird fortlaufend geschrieben). Belege: `G5_belege/`.

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
| Lichtsatz 16/17 | der von Cut 11 | RE2s Lichtsaetze sind fuer RE2-Figuren; Cut 11 beleuchtet dasselbe Wagenende. |
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
