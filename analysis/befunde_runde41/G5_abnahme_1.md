# Abnahme 1 der Spur G5A (G5-Auftritt im Zug, ROOM5090/5091) - unabhaengig

Zweig `r41/g5auftritt` (6d8e65214, 78ebadefd auf master 789f6e681), eigener Baum `re15_port/build_abn`. Pruefer ist nicht der Autor.
Massstab laut Nutzer: "die letzte Stufe GENAUSO machen wie bei Resident Evil 2". Belege: `G5_belege/abnahme1/`.

## Urteil: NICHT BESTANDEN

Die Reihenfolge stimmt (erst die Arme, dann zieht sich der Koerper herein), Kamera und Hintergrund der zwei RE2-Einstellungen sind
die echten. Die Szene sieht aber nicht aus wie RE2: In RE2 laeuft Leon im Moment der Armierung aus dem Bild, die Grossaufnahme zeigt
nur die offene Tuer, die Arme und die Masse. Im Port stehen Leon/Elza (und in 5090 die Frau im Kittel) mitten in der Grossaufnahme,
die Arme fahren als gerade Stangen in Brusthoehe durch sie hindurch, der Koerper deckt sie zu. Dazu fehlt der Ton der Szene, der G5
steht 800 Einheiten zu weit vorn, und der linke Tuerpfosten verdeckt die Masse nicht. Alles unten mit Beleg.

**Echte RE2-Aufnahme: gelungen** (DuckStation, `re2leon.cue`, Abschnitt 1). Die Szene ist jetzt Bild fuer Bild, mit Ton und mit
RAM-Werten bekannt; die Nachbildung des Autors (`G5_belege/re2/re2_kontakt.jpg`, Marken statt Figuren) ist damit ueberholt.

## 1. Echte RE2-Referenz

### 1.1 Weg

- Image `C:/Users/mjoedicke/Downloads/ePSXe2018/re2leon.cue` = SLUS-007.48 (RE2 Dual Shock, Leon; `info/re2leon/SYSTEM.CNF`
  `BOOT = cdrom:\SLUS_007.48`, md5 von `info/re2leon/SLUS_007.48` = `PSX.EXE`). DuckStation legt dafuer die Karte
  `%LOCALAPPDATA%/DuckStation/memcards/Resident Evil 2 - Dual Shock Ver. (USA)_1.mcd` an: 5 Spielstaende, alle Leon A Arrange
  ("1.LeonA /02/Hall" .. "5.LeonA /05/Waiting Room"). Kein Spielstand in Stage 7.
- Spielstand 1 per virtuellem Pad geladen (Titel LOAD GAME -> Arrange -> 1), im Raum per Fenster-Schliessen gesichert
  (`SLUS-00748_resume.sav`). Geladener Raum laut RAM = `PL0/RDT/ROOM2000.RDT` (INIT-Block im RAM ab 0x8011cb6e).
- Sprung nach Stage 7 Raum 04 ueber die echte Tuermechanik, nur RAM-Bytes geaendert (`werkzeug/mk_7040b.py`):
  - AOT-Tabelle 0x800ce558.. zeigt auf Befehl+2 der Door_aot_set (0x800ce55c -> 0x8011cbb6 = Tuer 1, ROOM2000 INIT @0x02558).
    Diese Tuer: Rechteck um Leon gelegt (Spieler-Zeiger 0x800cfe14 aus Work_set @0x8005595c-68, Lage +0x38/+0x40 aus
    Member_set-Tabelle @0x80011228[11]/[13] = `sw a2,56(a0)` / `sw a2,64(a0)`; Leon stand bei (-12095, y -1800, -2319), daher
    Stockwerk-Byte der Tuer 1),
    Ziel Stage 6 Raum 4 Schnitt 4, Lage (-5000, 0, -23570), Richtung 0x800, Tuerbild 0x2A/2 wie die echte Tuer 7030 -> 7040
    (ROOM7030 @0x01C78 `3b 02 01 31 .. 62 4e 00 00 b0 a4 48 08 06 04 0c 00 2a 02 ..`).
  - Flag 4:0x89 (ROOM7040 MAIN sub00 @0x01194 `21 04 89 01`): Ck-Zeigertabelle @0x800a78c8, [4] = 0x800d4854; Bitpruefung
    @0x800519c0-0x80051a24 (`srl v0,a1,3 / andi 0x1c` Wort, `lui 0x8000 / srlv` Maske) -> Wort 0x800d4864 |= 0x00400000.
  - Danach im Emulator echt: Kreuz (Tuer), 2 s nach Westen laufen -> sub02 @0x013A2 `3e 00 0b 02 90 e8` -> sub06.
- Fehlversuch (dokumentiert): 0x800df348 = 1 / 0x800df38c = Zeiger setzen oeffnet das Statusmenue - das ist der Statusbildschirm-Pfad
  (@0x800264a0-0x80026520 haengt an DAT_800ce310 & 0x4000), nicht die Tuer.
- Aufnahme: DuckStation Media-Capture (FFmpeg/mp4, 640x480, 59,8 Bilder/s, Ton 44,1 kHz). Lauf 1 ungestoert (Video), Lauf 2 mit 26
  Savestates im Abstand ~1,5 s (Hotkey SaveSelectedSaveState dafuer kurz auf SDL-0/RightShoulder, danach wieder Keyboard/F2;
  settings.ini mit der Sicherung `settings.ini.vor_g5abnahme_2026-11-02` verglichen: gleich).
- Dieselbe EXE: die Bytes `info/re2leon/SLUS_007.48` [0x800:0x830] stehen im Savestate-RAM bei 0x80010000 (re2ss.py sucht danach),
  die zitierten Adressen gelten also fuer die Aufnahme.
- Grenze: kein natuerlicher Leon-B-Spielstand (Arrange-Spielstand, nur Flag 4:0x89 gesetzt, Eintritt bei x -5000 statt durch die
  Tuer 7030). sub06 und das G5-Overlay sind aber RE2s eigener Code; ab sub02 laeuft nichts Geaendertes mehr.

### 1.2 Zeitachse (Lauf 1, Schnittkanten per Bilddifferenz)

| Video | Schnitt | Inhalt |
|---|---|---|
| 56,75 s | 3 | Hinterwagen, Deckenplatte faellt, Dach-Arm haengt herab |
| 60,47 s | 4 | |
| 61,47 s | **9** | leere offene Tuer links (dunkel, orange Bruestung darin); Leon laeuft von links herein, bleibt in der Mitte stehen, schaut zur Tuer zurueck |
| 63,80 s | 9 | **Bild 0 (Armierung @0x018DE)**: Leon laeuft nach rechts los (@0x018EA `3f 00 0f 80`) |
| 64,13 s | 9 | erster Arm aus der Tuer (= Bild 10, [T1] t=10); Leon verlaesst bei 64,37 s rechts das Bild |
| 64-79 s | 9 | nur Tuer, Arme, Masse: Arme biegen sich am Rahmen, an Decke und Boden; Masse ab ~69 s unten links herein, hinter dem linken Pfosten |
| 79,64 s | **10** (Bild 437) | Leon rechts vorn von hinten, geht rueckwaerts aus dem Bild (1-2 s); G5 kriecht heran, Maul fuellt am Ende das Bild |
| 92,76 s | **5** (Bild 807) | tief hinter Leon, G5 weit hinten; Leon geht rueckwaerts auf die Kamera zu und verlaesst das Bild; Wedeln |
| 104,87 s | **6** (Bild 1162) | Kampf |

Echtzeit-Dauern: Schnitt 9 ab Armierung 15,84 s, Schnitt 10 13,12 s, Schnitt 5 12,10 s, zusammen 41,07 s.
Belege: `re2_echt_vorlauf_schnitt3_4.jpg`, `re2_echt_armierung_15bps.jpg`, `re2_echt_lauf_und_erster_arm_5bps.jpg`,
`re2_echt_schnitt9_2bps.jpg`, `re2_echt_schnitt10_und_5.jpg`, Clip `re2_echt_szene_320.mp4` (mit Ton), `re2_echt_ton_spektrogramm.jpg`.

### 1.3 RAM (Lauf 2, Savestates; Zeiger 0x800cfe14 Spieler, 0x800cfe1c[i] Gegner-Slot i; Lage +0x38/+0x3C/+0x40, Richtung +0x76, Schnitt u16 @0x800d4820 aus FUN_80054884 `sh a0,0(0x800d4820)`)

| Stand | Schnitt | Leon x (Richtung) | G5 x (Slot 0) | Arme 1..4 (x, y, z) |
|---|---|---|---|---|
| s01 | 9 | -3800 | (Parkplatz -32000) | |
| s02 | 9 | 0 | Parkplatz | |
| s03 | 9 | 5736 (4095 = Osten) | -9000 | (-6235,-2277,-25333) (-5585,-1887,-24620) (-5791,-717,-20905) (-6243,-2460,-21442) |
| s04 | 9 | 16308 | -9000 | |
| s05 | 9 | 20066 (Ostwand) | **-9525** | |
| s06 | 9 | 20066 | -5126 | |
| s08..s11 | 9 | 20066 | **-2786** | |
| s12 | 10 | 6565 (2048 = Westen) | -2786 | |
| s13..s19 | 10 | 7715 .. 16038 (rueckwaerts) | -2071 .. **1160** | |
| s20..s26 | 5 | 11394 .. 19468 (rueckwaerts) | 1160 | |

Leon steht in RE2 also nie in der Grossaufnahme: Er laeuft bei Bild 0 nach Osten bis an die Wand (20066) und geht in Schnitt 10 und 5
rueckwaerts (Blick nach Westen) aus dem Bild.

## 2. Port im echten Fenster

Laeufe `werkzeug/run_port.sh`: eigene exe-Kopie, gdigrab `hwnd=` der eigenen PID, 20 Bilder/s, kein AUTOSHOT, kein SOFTWARE_RENDER;
Eingabe wie `integration_r41_birkin_pistole` (Debug-Sprung, Pistole, Zielen und Feuer gehalten), EXIT_AT 3300. Protokoll beider
Laeufe: `[g5auftritt] F1558 ... Armierung, Schnitt 16`, Hintergruende RE2 rv=0, Atlas 33312 Byte, `F2720 Szenenende Bild 1162:
G5 Sub 1 Ph 1, Spieler x 13873, Cut 12`. Schnittkanten im Video: Rueckstoss Cut 11 66,85 s, Schnitt 16 68,65 s, 17 83,50 s, Cut 12
96,15 s (Elza 69,90 / 83,80 / 96,40). Dauern 14,85 / 12,65 / ~12,1 s (zusammen ~39,6 s).

Vergleichsbilder (gleiche Szenenbilder, RE2 echt | Leon | Elza): `vergleich_re2_leon_elza_1.jpg` (Bild 0..360),
`vergleich_re2_leon_elza_2.jpg` (Bild 440..1165); Nahvergleiche `arme_re2_port_bild45_100.jpg`, `koerper_re2_port_bild180_271.jpg`,
`pfosten_re2_bild240_port_bild180.jpg`; Uebergang `port_uebergang_rueckstoss_schnitt16.jpg`, Schnittkanten `port_schnittkanten_16_17_12.jpg`;
Clip `port_leon_szene_320.mp4`.

In Ordnung:
- Reihenfolge: Arme bei Bild 11/31/41 (RE2 10/30/40), Masse ab ~Bild 100 von unten links (RE2 ab ~Bild 90-150), danach Schnitt 10/5.
- Kamera und Hintergrund von Schnitt 16/17 = RE2 9/10 (Bildaufbau deckungsgleich mit dem echten RE2-Bild).
- Uebergang Rueckstoss -> Schnitt 16: harter Schnitt, kein Zwischenbild, Szenenbalken ab Schnitt 16; Schnittkanten 16 -> 17 -> 12
  ohne Flackern oder falsches Zwischenbild.
- Kampf danach: G5 nimmt Schaden (600 -> 210 bis F3300 im Fensterlauf), Spieler lebt; Kamera wechselt nach Cut 13, als der G5 ihn
  bei F3055 nach Osten (14635) schiebt.

## 3. Maengel

**M1 (hoch) - Die Spielfigur steht in der Grossaufnahme, statt aus dem Bild zu laufen.**
RE2: direkt nach der Armierung `3f 00 0f 80` Plc_motion @0x018EA (Bild 1) laeuft Leon nach Osten los (RAM s02 -> s03 -> s05:
0 -> 5736 -> 20066, Richtung 4095; Video 63,83-64,37 s); in Schnitt 10 `3f 0b 01 54` @0x01940 und in Schnitt 5 geht er rueckwaerts
(Richtung 2048, x 6565 -> 16038 bzw. 11394 -> 19468) und ist nach 1-2 s aus dem Bild. Die Grossaufnahme zeigt ab Bild ~15 nur Tuer,
Arme und Masse.
Port: `spieler_setzen(G5A_X_ARMIERUNG)` stellt Leon/Elza auf 1400, Pose 200, fuer 437 Bilder mitten ins Bild (g5auftritt_r41.c,
Kommentar "Leon steht"); ebenso 6500 in Schnitt 17 und 13873 in Cut 12. Die Arme fahren durch Brust und Kopf, der Koerper schiebt
sich ueber die Figur (Vergleich Bild 45..360). Der G5 schiebt die gesperrte Figur dabei herum (state.log: 1400 -> (822,-24493) Bild 50
-> (2345,-24701) Bild 271 -> (2712,-25500) Bild 360; 6500 -> (6673,-23215) Bild 437; 13873 -> 15149 Bild 1161; nach der Freigabe sofort
(13564,-22855)). Die Dossier-Aussage "der G5 schiebt die Figur zur Seite (1400 -> 834 -> 2712), wie in RE2" ist durch den RE2-RAM
widerlegt: in RE2 steht dort niemand. Der Kommentar `G5A_X_ARMIERUNG 1400 ... auf dem Leon bei der Armierung steht` beschreibt nur
Bild 0; @0x018EA folgt im naechsten Bild. Die "Gesten" sind keine Gesten, sondern Laufen bzw. Rueckwaertsgehen - dafuer hat RE1.5
eigene Spieler-Clips, RE2-PL0-Clips sind nicht noetig. Der Riegel `unit_r41_g5auftritt` ("Leon im Bild von Schnitt 17") haelt den
Fehler fest.

**M2 (hoch) - Die Arme sehen nicht aus wie in RE2.**
RE2 Bild 45/100 (`arme_re2_port_bild45_100.jpg`): die Arme biegen sich aus der Tuer, einer im Bogen an die Decke, zwei flach ueber den
Boden bzw. die Bruestung bis etwa Bildmitte. Port: gerade, steife Stangen waagerecht in Brust-/Kopfhoehe, bis ~3/4 der Bildbreite.
Die Ankerlagen stimmen (RE2 s03 Arme (x,z) (-6235,-25333) (-5585,-24620) (-5791,-20905) (-6243,-21442); Port Bild 45
(-6269,-25380) (-5607,-24678) (-5750,-20960) (-6211,-21489)); RE2-Ankerhoehen y -2277 / -1887 / -717 / -2460. Die Abweichung liegt
also in Form/Biegung/Hoehe der gezeichneten Arme (Tentakel-Modul, nicht im Diff dieser Spur), ist aber genau das Bild, um das es
dem Nutzer geht ("zunaechst die Tentakel"). Nachzumessen: Bone-Kette/Clips EM37 sub8 (Dossier: Clip 14/16/15) gegen RE2-Savestate.

**M3 (mittel) - Der G5 steht im ganzen Auftritt 800 Einheiten zu weit oestlich.**
RE2 (RAM): [T0] -9000 (s03/s04, = @0x801011d0 `addiu v0,zero,-9000`), waehrend [T1] -9525 (s05), Ende Schnitt 9 -2786 (s08..s11),
Ende Schnitt 10 1160 (s17..s26). Port (state.log, beide Figuren gleich): [T1] Bild 0..90 konstant -9000, Bild 271 -1986, Bild 700
1960. Die Spurdeltas stimmen ([T2] +7014, [T6] +3946); es fehlt die Bewegung um -800 waehrend [T1]. Der [T1]-Code
@0x80101214-0x801012e8 schreibt selbst keine Lage (nur Zeitgeber +0x158 und Arm-Phasen), [T0] setzt `sw v0,332(s0)` = 0x1F0001 und
den Clip ueber `j 0x8010e7e0` - die Verschiebung kommt also aus dem Clip-1-Lauf ausserhalb des Zustands; Mechanismus nachzuweisen.
Folge: Kampfbeginn G5 1960 statt 1160, mit der Spielerlage 13873 statt 14850 Abstand 11913 statt 13690.

**M4 (mittel) - Linker Tuerpfosten verdeckt die Masse nicht.**
RE2 Bild 180-271 (auch Bild 240, gleiche G5-Lage wie Port Bild 180): der Pfosten links liegt vor der Masse, sie erscheint erst rechts
davon. Port Bild 180/271: die Masse liegt ueber dem Pfosten (`pfosten_re2_bild240_port_bild180.jpg`, `koerper_re2_port_bild180_271.jpg`).
Ursache offen (Tiefenregel der RE2-Masken im Port oder M3); per Masken-Tint/Tiefenprotokoll in Schnitt 16 zu klaeren.

**M4b (mittel) - Ein-Bild-Sprung der Masse beim Aufrichten.**
Fensterlauf Leon, Video 77,60 s (Szenenbild ~265, Uebergang [T2] Clip 1 -> [T3] Clip 3 bei Bild 271): fuer genau ein Bild haengt die
Masse in Kopfhoehe ueber den Figuren, davor und danach liegt sie am Boden (Bilddifferenz 17,5 gegen Median 4,5;
`port_leon_sprung_bild265.jpg`). RE2 an derselben Stelle (72-76 s): keine Bilddifferenz ueber 3,9, kein Sprung. Im Elza-Lauf fiel das
Bild zwischen zwei Aufnahmebilder (20/s Aufnahme gegen ~29/s Spiel); die Logik beider Laeufe ist gleich (state.log), der Sprung ist
also dort zu erwarten. Ursache (Clip-Wechsel/Interpolation) nicht untersucht.

**M5 (mittel) - Ton der Szene fehlt.**
RE2: Musikwechsel bei Bild 12 (`51 00 00 01 50 40`, `51 00 00 02 50 40`, `51 01 01 00 80 40` @0x018F4/@0x018FA/@0x01900),
Birkin-Laute Xa 0x0B (@0x01920, STAGE7.BIN @0x186 `04 00 00 00 54 00`) und 0x0C (@0x01946, @0x18C `01 00 00 00 6b 00`); im Clip und
Spektrogramm durchgehend hoerbar/sichtbar. Port: g5auftritt_r41.c hat keinen Ton; es laufen nur die SE des G5-Overlays
(enemy_ai_boss_g5.c `g5_se` 10/9/11/0). Der Autor fuehrt den XA-Ton als offen; den Musikwechsel listet das Dossier in 1.2
("Kampfmusik"), setzt ihn aber nicht um und fuehrt ihn weder unter PORT-WAHL noch unter Offen.

**M6 (mittel) - RE1.5-Dinge in der RE2-Grossaufnahme.**
Frau im Kittel (5090, Typ 0x4D bei (600,-25300)) steht mitten im Bild von Schnitt 16 und im Hintergrund von Schnitt 17, die Arme
fahren durch sie. Das RE1.5-Feuer (Boden-ESP) deckt das untere Drittel samt Bruestung in der Tuer zu; in RE2 Bild 0/45 ist die
offene Stelle eine leere dunkle Tuer mit orange Bruestung, im Port ist sie hinter Frau und Flammen nur halb zu erkennen. Als PORT-WAHL
begruendet ("der Raum bringt seine Dinge mit"), fuer "genauso wie RE2" in dieser einen Einstellung aber sichtbar falsch.

**M7 (gering-mittel) - Vorlauf von Schnitt 9 fehlt.**
RE2 schneidet 2,3 s vor der Armierung auf Schnitt 9 (61,47 s): erst die leere offene Tuer, dann laeuft Leon herein (Pos_set -3800
@0x0187C, Plc_dest 1400 @0x018B0), schaut zur Tuer (@0x018C7 `3f 00 0f 08`, @0x018CC Plc_neck), dann Armierung. Der Port beginnt
Schnitt 16 mit Bild 0, die Figur steht schon da. Dass die 45-Bilder-Pause wegfiel, ist richtig (RE2 hat keine).

**M8 (gering) - Zeitachse.**
RE2 echt 15,84 / 13,12 / 12,10 s (41,07 s), Port 14,85 / 12,65 / ~12,1 s (~39,6 s). Reihenfolge und Inhalt der Abschnitte stimmen;
der Unterschied kommt aus dem Echtzeit-XA-Warten und Ruckelbildern in RE2. Gering.

**M9 (gering) - Paket-Gate fehlt, obwohl der Commit es nennt.**
`release/make_package.sh` ist im Zweig unveraendert; R704_CUT09.BSS/.PRI/_PRI.TIM und R704_CUT10.BSS/.PRI kommen nur ueber
`cp -r "$RE2"` (Zeile 699) ins Paket, ohne die Pflicht-/cmp-Pruefung, die alle anderen Runde-41-RE2-Dateien haben (Zeilen 286-297).
Fehlt eine, laufen 16/17 still ohne Hintergrund (bg_pc.c "fehlt - Schnitt ohne Hintergrund").

**M10 (gering) - Lesen hinter der RID-Tabelle in Protokollzeilen.**
main.c `[pri] cut=%d pri_offset=0x%X` (Zeile 6260) und die Ausgaben bei F1700/1750/1800 (Zeile 9397 ff.) lesen `active_cuts[16/17]`
jenseits `rdt.cut_count` (16) - gemessen im Fensterlauf: `[pri] cut=16 pri_offset=0x26DE0E74`, `cut=17 pri_offset=0x1FF01`,
`[F1700-leon-root] ... cam_pos=(-81857914,753726086,743864200)`. Kein Absturz beobachtet, aber das debug.log luegt in der Szene.
(Die Masken selbst kommen richtig an: `cut=16 ... masks=32 fg_atlas=1`, `cut=17 masks=0`; M4 liegt also nicht am Laden.)

**M11 (gering) - `integration_r41_g5_westen` zweckentleert** (vom Autor gemeldet, bestaetigt: Leon und Elza "Weg ab F1600 4955" =
Szenenversatz, nicht Eingabe).

**M12 (bekannt) - PSX-Bau** laedt Schnitte 16/17 nicht.

## 4. PORT-WAHLEN des Autors gegen "genauso wie RE2"

| Punkt | Bewertung |
|---|---|
| Ton (XA 0x0B/0x0C) | Mangel M5 (dazu Musikwechsel) |
| RE2-Spielergesten weggelassen ("Leon steht") | Mangel M1, hoch: es sind Lauf/Rueckwaertsgehen aus dem Bild, mit RE1.5-Clips machbar |
| 45-Bilder-Pause entfallen | vertretbar, RE2 hat keine; aber M7 (Vorlauf fehlt) |
| Spielerlage 13873 statt 14850 | vertretbar (RVD-Begruendung @0x4F4/@0x4CC nachvollziehbar), Abstand zum G5 wegen M3 aber 1777 kuerzer |
| Frau im Kittel / RE1.5-Feuer im RE2-Wagen | Mangel M6 in der Grossaufnahme |
| grauer RE2-Hintergrund zwischen orangen RE1.5-Bildern | vertretbar: RE1.5 hat keine Kamera dieser Art; Stilbruch sichtbar, aber Folge der Vorgabe |
| Lichtsatz Cut 11 fuer 16/17 | vertretbar, Figuren sehen normal aus |
| RE2 5/6 -> Cut 12 | vertretbar (RE1.5-Spielkamera desselben Blicks) |

## 5. Stichproben der zitierten Adressen (selbst gelesen)

- sub06 @0x018DE `34 17 01 00`, @0x0192A/@0x01932/@0x0193A, @0x01950/@0x01956, @0x01962..@0x01987: im SCD-Dump von ROOM7040 bestaetigt.
- Member_set-Tabelle @0x80011228[23] -> 0x80055d8c `j / sh a2,468(a0)` @0x80055d90: bestaetigt.
- Wsleeping @0x80053ac0-d0 `lw v0,-1064(0x800d)` = 0x800cfbd8, `andi v0,v0,0x20`: bestaetigt.
- STAGE7.BIN @0x186 `04 00 00 00 54 00`, @0x18C `01 00 00 00 6b 00`, @0xE6 `40 cf`: bestaetigt.
- RID ROOM7040 Kopf[7] = 0x64, @0x184 und @0x1A4 = die im Code zitierten 32 Byte: bestaetigt.
- RE2 FUN_80049b1c @0x80049c10 `lhu v1,-2(a2)`, @0x80049c18 `bne v1,zero,0x80049c3c`, @0x80049c20-28 Rechteckzweig: bestaetigt.
- G5 [T0] @0x801011d0/@0x801011d8 (-9000/-23400), [T1] Zeitgeber @0x80101214-2c, Arm-Phasen @0x80101240/68/90 (CDEMD0_EM36_ai0.BIN): bestaetigt.
- Konstanten in g5auftritt_r41.c tragen Adresse oder PORT-WAHL. Inhaltlich falsch gedeutet: G5A_X_ARMIERUNG (s. M1).

## 6. Tests (build_abn, ctest)

Bau `local_build.sh configure/build` mit `RE15_BUILD_DIR=re15_port/build_abn`: LOCAL-BUILD-OK, 2261/2261 Schritte.

| Lauf | Ergebnis |
|---|---|
| Gezielt (-j 4): unit_r41_g5auftritt, unit_birkin_szenen, unit_birkin_formen, unit_birkin_bahnsteig, unit_birkin_ai, unit_birkin_opfer, unit_r41_birkin_wand_rein/_g5/_griff, unit_sld_atlas, unit_pri_* (5), unit_g5_tentakel/_morph/_boss, unit_r17/r18/r20 Birkin-Pins, unit_r30_birkin_frost, unit_r30_n_room5080_birkin_wand, unit_r36_pri_absperrung/_schatten | 25/25 gruen |
| Integration (-j 6): integration_r41_birkin_pistole (338 s; Leon und Elza: Mutationen 1, Form-HP 150 bis 0, Rueckstoss 1, G5 1, G5 tot 1), integration_r41_g5_tod (461 s; HP 0, Tod bei HP 0, exit 0, beide), integration_r41_g5_westen (305 s; 0 Bildenden in fester Zelle, "Weg ab F1600 4955" in allen drei Laeufen = M11), integration_r35_zgirl_5090, integration_pri_masken, integration_r36_pri_schatten | 6/6 gruen |
| Alle Nicht-Integrationstests (unit/prop/probe + 13 sonstige, -j 10) | 1073/1074; unit_r41_birkin_wand_griff Zeitlimit 360 s unter Last (allein im gezielten Lauf 128 s gruen) - Einzelwiederholung gruen (211 s) -> 1074/1074 |

Paket-Gate: s. M9 (R704_* kopiert, aber nicht geprueft).

Die Tests pruefen den Ablauf so, wie der Autor ihn gebaut hat; M1 (Figur im Bild) haelt `unit_r41_g5auftritt` sogar fest, M2-M6
prueft keiner.

## 7. Fuer die Nachbesserung (nur Befund, keine Loesung vorgegeben)

- Spielfigur in Schnitt 9/10/5 wie RE2 aus dem Bild bewegen (RAM-Spur 1.3 als Massstab), Riegel "Leon im Bild von Schnitt 17" umdrehen.
- Arme gegen RE2-Savestate (Bild 45/100) vermessen; G5-[T1]-Versatz -800 per Savestate-Reihe in [T1] belegen und den Mechanismus finden.
- Masken-Tiefe in Schnitt 16 pruefen (linker Pfosten).
- Ton: Musikwechsel Bild 12 und die zwei XA-Spuren.
- R704_* ins Paket-Gate.

Werkzeuge: `G5_belege/abnahme1/werkzeug/` (re2_drv.py Pad-Fernsteuerung, re2ss.py Savestate lesen/schreiben, mk_7040b.py Sprung nach
ROOM7040, run_port.sh Fensterlauf).
