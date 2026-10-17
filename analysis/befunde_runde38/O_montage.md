# Runde 38 Spur O — montage (Leon, Szene nach Irons' Tod: Zombies gehen auf die Kamera zu)

Baum: `.claude/worktrees/r38_montage` (Zweig `r38/montage`, Basis d70c234e). Dossier wird fortlaufend geschrieben.

## Wortlaut (AUFTRAG.md, massgeblich)

> Bei Leon - bei der Cutscene nach Irons Tod, wo die Zombies durchbrechen, sollen sich die Zombies jeweils auch
> Richtung Kamera bewegen, also so, wie es bei den Zombies hinter dem Rolltor schon passiert. Die anderen sollen das
> auch machen. Das wirkt auch bedrohlicher.

Lesart (VERTRAG §0 Spur O): in JEDEM Montage-Schnitt, in dem Zombies durchbrechen, gehen ALLE Zombies auf die Kamera zu,
in der Form, die der Rolltor-Schnitt (ROOM1040 Cut 1) schon zeigt; der Rolltor-Schnitt selbst bleibt unveraendert.
RE1.5 hat diese Montage nicht (Port-Szene auf Nutzerwunsch, Runde 35 Spur L) -> NUTZER-VORGABE; die FORM ist die des
Rolltor-Schnitts im Port, deren Bausteine Original-Adressen tragen (unten).

## Stand

- [x] Schritt 0: Pflichtlektuere (AUFTRAG/VERTRAG, Runde 35 L `L_cut1150.md`, Runde 36 K `K_szene1150.md`, Runde 37 H
      `H_zombieaktiv.md`, Code `irons_tod_1150.c` / `re15_irons_tod.h` / `zombieaktiv_r37.c`)
- [x] Schritt 1a: Kameras der Montage-Schnitte aus dem RDT (Datei-Offsets, unten)
- [x] Schritt 1b: Zensus an der echten exe (Messzeilen RE15_IT_LOG=5, Stand d70c234e gebaut)
- [x] Schritt 1c: Framedumps 1040/1030 (Bild)
- [x] Schritt 2a: RE-Belege der Rolltor-Form (selbst disassembliert)
- [x] Schritt 2a': FORTSETZUNG 2026-10-17 (zweiter Lauf nach dem Sitzungslimit des ersten): r37/re2zombie gemergt
      (05e7af25, `git merge --no-ff r37/re2zombie`, OHNE Konflikte — die Spur-L-Dateien enemy_ai_re2_zombie.c /
      enemy_ai_common.c / re15_actor.h / re15_re2zombie_r37.h und Tests beruehren keine Spur-O-Datei); alle RE-Belege
      unten im zweiten Lauf selbst nachdisassembliert / nachgelesen (Abschnitt "Nachpruefung im zweiten Lauf")
- [x] Schritt 2b: Bauen + Zwischenmessung an der exe -> zwei Befunde am Stand des Vorgaengers (veraltetes
      Navigator-Ziel in Cut 7, aufrechte Zombies am Tor in Cut 12) + Riegel-Defekt (ohne g_room_rdt), behoben
- [x] Schritt 3: Messen nachher an der exe (echter Weg) + Riegel (unit + integration) gruen, Gegenprobe v0.8.25 rot
- [x] Schritt 4 (dritter Lauf, Abschluss): Reste aus acdbfe7c gebaut + gefahren -> 2 neue Teile rot (Riegel-Defekte);
      Pruefung gegen den Wortlaut -> die Klemme (OFFEN 1 alt) erfuellt "alle" nicht; Mechanismus gemessen; Baustein 5
      (Zombies hinter dem Tor warten auf Cut 12, Form ROOM1141 sub02) behebt sie; Riegel misst "ueber die Schnittdauer"
- [ ] Schritt 5: exe ueber den echten Weg nachher (Baustein 5), Framedump Cut 12, gezielte ctest-Laeufe, Abschluss-Commit

## Messung vorher

### Kameras der Montage-Schnitte (RDT-RID, selbst gelesen: `O_belege/kameras_rid.py`)

RID-Block = RDT+0x24 (`camera_start` = 0x60 in allen vier Raeumen), 32 Byte je Schnitt (re15_camera.h: +0x04 pos_x,
+0x08 pos_y, +0x0C pos_z, +0x10 target_x, +0x14 target_y, +0x18 target_z):

| Raum | Cut | Datei-Offset | Kamera (x,y,z) | Ziel (x,y,z) | Bytes +4..+27 |
|---|---|---|---|---|---|
| ROOM1130 | 0 | 0x00060 | (-2386,-3801,-7451) | (876,303,-18973) | `ae f6 ff ff 27 f1 ff ff e5 e2 ff ff 6c 03 00 00 2f 01 00 00 e3 b5 ff ff` |
| ROOM1040 | 1 (Rolltor) | 0x00080 | (-26262,-3114,-10692) | (-25020,-1746,-1818) | `6a 99 ff ff d6 f3 ff ff 3c d6 ff ff 44 9e ff ff 2e f9 ff ff e6 f8 ff ff` |
| ROOM1030 | 7 | 0x00140 | (-26820,-3114,-2574) | (-15858,-936,-5040) | `3c 97 ff ff d6 f3 ff ff f2 f5 ff ff 0e c2 ff ff 58 fc ff ff 50 ec ff ff` |
| ROOM1030 | 12 (= 6) | 0x001E0 | (-11790,-3870,-15282) | (-8640,-684,-26478) | `f2 d1 ff ff e2 f0 ff ff 4e c4 ff ff 40 de ff ff 54 fd ff ff 92 98 ff ff` |

### Zensus der Montage-Schnitte mit Zombies (echte exe, Spielstand + CONTINUE + Tuer 1130 -> 1150, Kette)

Weg: Karte `probe_r35_cut1150_karte re15_card.mcr 1130 nach10f0 ersteszene`, CONTINUE, Eingabe `W1,A0.2,W200` (Tuer
1130 -> 1150 bei Bild 90), Szene, Montage. Messkopie der exe, `RE15_IT_LOG=5` (Messzeile alle 5 Takte: je Gegner Lage,
Steuerziel, grid, Zustand, Clip; irons_tod_1150.c mess_zeile), `RE15_STATE_LOG`. Skript `O_belege/lauf.sh`, Auswerter
`O_belege/ausw.py` (Abstand zur Kamera-Bodenprojektion = |(x,z) - (Kamera x,z)|, "tiefe" = Skalarprodukt mit der
Blickrichtung xz). Ergebnis `O_belege/vorher_messung.txt`, Messzeilen 1130 `O_belege/vorher_messzeilen_1130.txt`.

| Raum / Cut | Dauer | Zombies (Aktor:Typ, grid) | heute | Abstand zur Kamera (Anfang -> Ende) |
|---|---|---|---|---|
| 1130 Cut 0 | T0..T170 (175 Bilder) | 1:16, 2:10, 3:10, 4:11, 5:11 (grid 0x02, EM_1130_LISTE) | **stehen** (Weg 0, einer 18) | 6821/5648/4565/3521/4478 -> unveraendert (-7 bei einem) |
| 1040 Cut 1 (Rolltor) | T180..T580 (405 Bilder) | 1..5: 16 (Raum-Records, grid 0x0d) | gehen durchs Tor auf die Kamera zu | 12366..15470 -> 5395..8286 (je -5230..-9430) |
| 1030 Cut 7 | T585..T785 (212 Bilder) | 7:10, 8:10, 9:10, 10:11, 11:11 (1070-Kopien, grid 0x00, EM_1070_LISTE) | **stehen** (Weg 0) | 8272/6235/4644/7222/5599 -> unveraendert |
| 1030 Cut 12 (Tor) | T790..T1120 (336 Bilder) | 1..6: 16 (Raum-Records hinter dem Tor), 12..14: 16 (Kriecher) | Kriecher 4/5/12 kriechen auf die Kamera zu (-4031/-4211/-1565); 2 kriecht auf der Stelle (+12); 1/3/6/13/14 gehen hinter dem Tor umher (-13..-709) | s. vorher_messung.txt |

Spieler-HP ueber die ganze Kette 100 -> min 100 (debug.log `Bilanz Kette`). Im Rolltor-Schnitt kam keiner hinter die
Kamera (`hinter der Kamera angehalten 0`). Bild 1130: `O_belege/vor_1130.png` (F0 schwarz = Einblendung, F25..F175 die
fuenf Zombies stehen als Gruppe vor der Briefing-Room-Tuer, nur der Steh-Loop).

Befund: in 1130 Cut 0 und 1030 Cut 7 stehen die durchgebrochenen Zombies still — das ist der Nutzer-Befund. Ursache im
Code (gelesen, nicht vermutet): die Montage-Listen tragen die STEHENDE Form (EM_1130_LISTE grid 0x02, EM_1070_LISTE grid
0x00), und die Runde-37-Abbildung grid 0x00 -> RE2-Gang ist waehrend eines Montage-Schritts abgeschaltet
(zombieaktiv_r37.c `re15_irons_tod_zustand() != RE15_IT_AUS`), damit der geparkte Spieler nicht angegriffen wird.

Bilder (RE15_FRAMEDUMP, RE15_WINDOW_SCALE=1, je Raum ein Lauf mit RE15_EXIT_AT im Raum):
* `O_belege/vor_1040.png` (Rolltor, F50..F400): Tor faehrt hoch (F100/F150), die fuenf kommen durch und gehen auf die
  Kamera zu (F250..F400 immer naeher) = das Vorbild.
* `O_belege/vor_1030.png`: Cut 7 (F30..F210) die fuenf Kopien stehen still im Gang vor der 1070-Tuer; Cut 12
  (F240..F540) hinter dem aufgebrochenen Gitter stehen aufrechte Zombies, unten kriechen einzelne unter dem Tor durch
  nach vorn (F360..F540).

## RE-Belege (Form des Rolltor-Schnitts; alles selbst gelesen)

Der Rolltor-Schnitt (irons_tod_1150.c, Runde 35 L Nachbesserung 1/2) besteht aus drei Bausteinen:

1. **Zombie-Form "durchbrechend" = grid 0x0d (ENGAGE ab INIT).** STAGE1.BIN (re15_disasm.py):
   `80100f20 lbu v0,9(a0)` / `80100f24 ori v1,zero,0xd` / `80100f28 andi v0,v0,0x1f` / `80100f2c bne v0,v1,0x80100f60` /
   `80100f30 ori v0,zero,0x27` / `80100f34 sb v0,148(a0)` / `80100f40 ori v0,zero,0x201` / `80100f44 sw v0,4(v1)` /
   `80100f54 sb zero,9(v0)` = Zustandswort 0x201 (Angriff) ab dem ersten Takt. Gilt fuer die Typen 0x10/0x11/0x16 (alle
   Montage-Zombies): Installer `8011e868 lui v0,0x8010` / `8011e86c addiu v0,v0,1060` (= 0x80100424, Zombie-Wurzel) /
   `8011e88c sw v0,11244(at)` (0x80072bec = Typ 0x10) / `8011e894 sw v0,11248(at)` (0x80072bf0 = 0x11) /
   `8011e8a4 sw v0,11268(at)` (0x80072c04 = 0x16). Unter der RE2-KI (Standard) bildet re2z_init grid 0x0d auf RE2
   Deskriptor 6 = GANG ab (enemy_ai_re2_zombie.c `sel == 0x0d` -> 0x101); RE2 EMZ0.BIN (re2_disasm.py --bin EMZ0.BIN):
   `80100aa8 lhu v0,270(s2)` / `80100aac addiu v1,zero,6` / `80100ab0 andi v0,v0,0x3f` / `80100ab4 bne v0,v1,0x80100ac0` /
   `80100ab8 addiu v0,zero,257` / `80100abc sw v0,4(s2)` (Gang 0x0101); Stehen = `801006ac addiu v0,zero,1` /
   `801006b8 sw v0,4(s2)`. Die Rolltor-Zombies sind genau diese Form (Raum-Records ROOM1040 main00 @0x011FC.. Typ 0x16
   grid 0x0d; Auffuell-Records EM_1040_LISTE). Die heutigen Montage-Listen tragen dagegen die STEHENDE Form
   (EM_1130_LISTE grid 0x02 = ROOM1140 sub00 @0x00C12..; EM_1070_LISTE grid 0x00 = ROOM1070 sub00 @0x01632..).
2. **Ziel = der geparkte Spieler HINTER der Kamera.** Ein gehender/angreifender Zombie steuert auf die Spielerlage
   (Port: re15_nav_update_steer(e, Spieler x/z) enemy_ai_common.c, Form EXE FUN_80039e7c; RE2-Navigator FUN_8004A808
   Wurzel @0x801002d0-358). Rolltor: Parkplatz (-24950,-14900), Kamera Cut 1 (-26262,-10692), Blickrichtung xz
   (0,1386, 0,9903) -> 3985 hinter der Kamera, 1882 seitlich.
3. **Einzelstopp hinter der Kamera.** STAGE1.BIN Zombie-Wurzel `80100450 lbu v0,9(a0)` / `80100458 andi v0,v0,0x20` /
   `8010045c bne v0,zero,0x80100658` = Takt uebersprungen, wenn entity+0x9 Bit 0x20 (Member_set(12,0x30) im Original,
   ROOM1141 sub02); Port RE15_AI_GRID_SKIP (enemy_ai_common.c `if (e->grid_id & RE15_AI_GRID_SKIP) return 0;` vor
   Navigator und Gehirn). Rolltor: bilanz_takt setzt es, sobald ein Zombie z < Kamera-z (-10692) erreicht.

Kein Original-Vorbild fuer "Ziel = Kamera" (RE1.5 hat die Montage nicht) -> NUTZER-VORGABE, Form = 1+2+3.

### Nachpruefung im zweiten Lauf (2026-10-17, selbst disassembliert, Ausgaben woertlich)

* STAGE1.BIN Zombie-Wurzel `re15_disasm.py dis 0x80100424 12` / `dis 0x80100448 8`: Prolog `80100424 addiu sp,sp,-24`,
  `8010042c lw v0,-13760(v0)` (g_pauseflags) / `80100438 and v0,v0,v1` (0x20000000) / `8010043c bne v0,zero,0x80100658`
  (Pause), dann `80100450 lbu v0,9(a0)` / `80100458 andi v0,v0,0x20` / `8010045c bne v0,zero,0x80100658`. Sprungziel
  `dis 0x80100650 14`: `80100658..80100670` nur `jal 0x8001b064` (a0 = entity+0xb0, a1 = lh 442(entity)) und `jr ra`
  @0x80100680 = derselbe Ausgang wie die globale Pause -> Bit 0x20 friert den Zombie ein (keine KI, kein Schritt).
* Durchbruch-Form `dis 0x80100f20 14`: `80100f20 lbu v0,9(a0)` / `80100f24 ori v1,zero,0xd` / `80100f28 andi v0,v0,0x1f`
  / `80100f2c bne v0,v1,0x80100f60` / `80100f30 ori v0,zero,0x27` / `80100f34 sb v0,148(a0)` / `80100f3c lw v1,-14460(v1)`
  (g_entity) / `80100f40 ori v0,zero,0x201` / `80100f44 sw v0,4(v1)` / `80100f54 sb zero,9(v0)` — bestaetigt.
* Installer `dis 0x8011e868 16`: `8011e86c addiu v0,v0,1060` (0x80100424) / `8011e88c sw v0,11244(at)` (0x80072bec = Typ
  0x10) / `8011e894 sw v0,11248(at)` (0x80072bf0 = 0x11) / `8011e8a4 sw v0,11268(at)` (0x80072c04 = 0x16) — bestaetigt.
* RE2 EMZ0.BIN `re2_disasm.py dis 0x80100aa0 10 --bin EMZ0.BIN`: `80100aa8 lhu v0,270(s2)` / `80100aac addiu v1,zero,6` /
  `80100ab0 andi v0,v0,0x3f` / `80100ab4 bne v0,v1,0x80100ac0` / `80100ab8 addiu v0,zero,257` / `80100abc sw v0,4(s2)`;
  Stehen `801006ac addiu v0,zero,1` / `801006b8 sw v0,4(s2)` — bestaetigt.
* Pos_set PSX.EXE `dis 0x80041048 16`: `8004104c lw a1,340(a0)` (Work-Entity) / `80041050 lh v0,2(v1)` / `80041058 sw
  v0,52(a1)` / `80041064 sw v0,56(a1)` / `80041070 sw v0,60(a1)` / `8004106c addiu v1,v1,8` (8 Byte) — bestaetigt.
  ROOM11B0.RDT `xxd -s 0x12a8`: @0x012B2 `2e 01 00 00 32 00 ae 9a 00 00 47 ba 34 04 e8 02` = Work_set(1,0), Nop,
  Pos_set(-25938,0,-17849), Member_set(4,744) — bestaetigt (Member_set Id 4 = rot_y, scd_vm.c op_member_set ->
  re15_actor_set_member).
* RID-Bytes `xxd -s <off+4> -l 24` in ROOM1130/1040/1030.RDT: alle vier Zeilen der Kamera-Tabelle oben bytegleich;
  camera_start @0x24 = `60 00 00 00` in allen drei RDTs.

## Zwischenmessung im zweiten Lauf (Stand des Vorgaengers + L-Merge, gebaut 2026-10-17 08:45, ECHTE exe)

Lauf `x_kette` (O_belege/lauf.sh: Karte 1130 nach10f0 ersteszene, CONTINUE, Tuer 1130 -> 1150, Szene, Montage,
`RE15_EXIT_AT=60#11C0`, RE15_IT_LOG=5). Auswertung `O_belege/zwischen_messung_x_kette.txt` (ausw.py) und die
`[montage-r38]`-Zeilen des Mess-Hakens `O_belege/zwischen_montage_zeilen_x_kette.txt`. Abstand zur Kamera-Bodenprojektion
(Anfang -> Ende des Schnitts):

| Schnitt | Zombies | Ergebnis |
|---|---|---|
| 1130 Cut 0 (176 Bilder) | 1..5 | 6821->3961, 5648->1969, 4565->1734, 3521->438 (Stopp Bild 142), 4478->1295 (Stopp Bild 161) — alle naeher |
| 1040 Cut 1 Rolltor (405) | 1..5 | Endlagen bitgleich mit "vorher" (7136/5663/8286/5395/7046), 5 durchs Tor (letzter Bild 270), HP 100 |
| 1030 Cut 7 (207) | Kopien 7..11 | 8225->6268, 6154->4800, 4553->3154, 7169->5523, 5570->3869 (je -1354..-1958) |
| 1030 Cut 12 (335) | 1..6, 12..14 | 1 -473, 2 -81, 3 -1067, 4 -2229, 5 -2558, 6 -317, 12 -2407, 13 -410, **14 -12** |

**Befund 1 (Cut 7, Defekt des Vorgaengers): Navigator-Ziel VERALTET.** In den Messzeilen steuern ab T590 ALLE Zombies in
ROOM1030 auf (-24950,-14060) = die Spielerlage des VORIGEN Raums ROOM1040 (Parkplatz 1040). Ursache (gelesen +
nachgerechnet): der neue Parkplatz (-30708,-1699) liegt AUSSERHALB jeder BLK-Zone von ROOM1030 (`O_belege/blk.py`: Zone 7
@0x01C10 x -29340..-19550, z -6000..-1190 ist die westlichste) -> re15_nav_zone_from_pos = 0xff -> FUN_8003a524 kehrt ohne
Ausgabe zurueck und FUN_80039e7c kopiert die geteilten Globals 0x800AFBAE/B0/B2 UNBEDINGT (nav_zone_common.c
"stale-on-unreachable is byte-true") = das Ergebnis aus ROOM1040. Die Kopien liefen deshalb zur Suedwand SCA 4
(z -5212 - Radius = -4794..-4810) und an ihr entlang, statt auf die Kamera zu. Der v0.8.25-Parkplatz (-28244,-3650) lag
IN Zone 7 (gleiche Zone wie die Kopien -> Ziel = Spieler direkt).

**Befund 2 (Cut 12): die aufrechten Zombies hinter dem Tor kommen nicht durch.** Sie steuern auf die Zonen-Uebergaenge
(-12195,-23860) (Zone 0 -> 9) / (-8695,-23860) (Zone 0 -> 1) und stehen an der Torzelle SCA 18/28 (@0x01AE8/@0x01B60,
z -24420..-22771) bei z = -24838 fest. Die Torzelle wird in der Szene mit `37 02 06 f7` / `37 03 06 f7` (sub08
@0x0278E/@0x02792, Sca_id_set u0 0xff -> 0xf7) nur fuer KRIECHER durchlaessig; durch kommt nur, wer das Kriech-Bit
(member 0x10 |= 0x1000) hat. Die Szene vergibt es viermal (Gosub sub09 x4); 1/13/14 stehen mit AOT-Stempel 5 im
Warte-Rechteck und bekommen es nie, 3/6 stehen dahinter. Kriecher 2 haengt an der Ecke SCA 30 (x -15518..-12566) fest
(war schon vorher so: +12).

**Befund 3 (Riegel des Vorgaengers misst nicht, was die exe tut).** test_r38_montage.c setzte g_room_rdt/g_room_rdt_ok
nie (nur eine lokale Kopie s_rdt): ohne g_room_rdt gibt es KEINEN Navigator-Graphen (nav_tbl() == 0 -> Rohziel) und
KEINE Gegner-Wandklemme (re15_collision_constrain_enemy(&g_room_rdt, ..), vgl. probe_1030_crawl_live.c Defekt 3) -> im
Riegel liefen die aufrechten Zombies durch das Tor (Cut 12: 9 von 9 naeher, auch in der Gegenprobe = rot) und die
Kopien auf Cut 7 direkt zum Spieler. Ausserdem mutiert Sca_id_set das RDT im Speicher, und der Riegel lud es pro Raum
nur einmal (zweiter Lauf im selben Prozess sah das offene Tor). Gezielte Laeufe: `unit_r38_montage_s1030_re2` und
`_re15` ROT (Gegenprobe), die anderen gruen — die Zahlen fuer 1030 sind aber nicht die der exe.

## Dritter Lauf (Abschluss, 2026-10-17 ab ~09:55): Reste geprueft, Klemme behoben (Baustein 5), Riegel ueber die Schnittdauer

### Pruefung der Reste aus acdbfe7c (gebaut, gezielt gefahren)

Gebaut (local_build.sh configure + build, Stand acdbfe7c), `ctest -R "^unit_r38_montage"`: **6 von 8 gruen, die zwei neuen
Teile ROT**:
* `unit_r38_montage_s1030_ohne7_re2`: die Gegenprobe v0.8.25 war NICHT rot — ohne Cut-7-Teil kamen auch in v0.8.25 alle 9
  mindestens 500 naeher (kleinste 1424), weil sie aus ihren Spawn-Lagen (z -26000..-30000) erst zum Tor GEHEN. Danach standen
  die aufrechten am Tor (z -24838) still. Das Mass "Anfang minus Ende" sieht dieses Stehen nicht -> Riegel-Defekt.
* `unit_r38_montage_s1030_ohne7_re15`: ein Zombie (Bit 0) kam nie ins Warte-Rechteck (RE1.5-KI, Unterzustand 0x13 wendet auf
  der Stelle, s.u.) -> "jeder aufrechte bekommt das Kriech-Bit" verfehlt (8 von 9).

### Pruefung gegen den Wortlaut: die Klemme an SCA 30 erfuellt "Die anderen sollen das auch machen" NICHT

OFFEN 1 des zweiten Laufs (ein Kriecher +120 von der Kamera WEG, KI RE2 = Standard) ist eine Abweichung vom Wortlaut ("alle")
und vom Auftrags-Riegel ("Abstand nimmt ab, alle Zombies"). Mechanismus selbst gemessen (Spur-Ausgabe, jetzt mit Gierung und
naechstem Nachbarn: `test_r38_montage s1030_re2 mess 2 1`):
* Im Cut-7-Teil (Bild 1..210, Kamera auf dem 1070-Gang) laufen die sechs Raum-Records hinter dem Tor zum Uebergang Zone 0 -> 9
  (-12195,-23860) und draengen sich 120 Bilder am GESCHLOSSENEN Tor (z -24838). Aktor 2 (Bit 1) wird dabei von Bild 126 (x
  -12195) bis Bild 166 (x -12338) nach Westen geschoben — WESTLICH der Klemmlinie der Zelle SCA 30 (x -12566 + 400 = -12166).
* Ab dem Kriech-Bit (Bild ~196, sub09 der Szene) steht er mit Ziel NNO (Gierung 3127..3263) unter der Suedflaeche von SCA 30;
  der naechste Nachbar haelt konstant 800 Abstand (= 2 Radien, Aktor 14 / 3 schieben). FUN_8003bca8 (re15_collision.c
  push_rect, code 0 = "Schritt in die Flaeche": `*lx = prevx; *lz = prevz`) nimmt jeden Vorwaertsschritt GANZ zurueck, die
  Rueckwaerts-Phase des Kriechens (Bild 220 -> 247: z -24539 -> -24652 bei gleicher Gierung) nicht -> Netto-Drift nach
  Westsuedwest = +120.
* Ohne Cut-7-Teil (Cut 12 ab dem ersten Bild) gibt es diesen Stau nicht: die Zombies kommen zu verschiedenen Zeiten ans Tor
  und kriechen sofort (Baustein 4) -> keine Klemme (`s1030_ohne7_re2`, alle 9 naeher).

Ursache also: die Zombies des Tor-Schnitts laufen schon los, waehrend die Montage einen ANDEREN Schnitt zeigt (Cut 7), und
stauen sich. Die anderen Montage-Schnitte haben das nicht: 1130 Cut 0, 1040 Cut 1 (Rolltor) und 1030 Cut 7 setzen ihre Zombies
im Raumaufbau und zeigen sie ab dem ersten Bild — sie laufen los, wenn IHR Schnitt beginnt.

### Baustein 5 (neu): die Zombies hinter dem Tor warten auf ihren Schnitt

Original-Form (selbst gelesen, `scd_dump_room.py ROOM1141.RDT` + `xxd -s 0xE6C -l 0x14`): ROOM1141 sub02
`2e 02 01 00 34 0c 30 00 2e 02 02 00 34 0c 30 00 09 0a 01 00` @0x00E6C = Work_set(2,1) + Member_set(12, 0x30), Work_set(2,2)
+ Member_set(12, 0x30), Sleep 1, danach `29 00` Cut_chg 0 @0x00E80: die Szene haelt die beiden Zombies an (entity+0x9 Bit 0x20,
Member 12 = +0x09 `sb`, actor_common.c re15_actor_set_member case 12), BEVOR sie den Schnitt von ihnen wegnimmt, und gibt sie
mit `34 0c 10 00` @0x00F64 / @0x00F74 (Bit 0x20 wieder aus) frei, wenn das Spiel weiterlaeuft. Wirkung des Bits: STAGE1.BIN
`lbu v0,9(a0)` @0x80100450 / `andi v0,v0,0x20` @0x80100458 / `bne v0,zero,0x80100658` @0x8010045c (Takt uebersprungen; Port
RE15_AI_GRID_SKIP, enemy_ai_common.c `if (e->grid_id & RE15_AI_GRID_SKIP) return 0;` fuer beide KI-Geschmaecker).

Umsetzung (montage_r38.c `tor_warten`, Konstanten re15_montage_r38.h): im Montage-Schritt ROOM1030, solange NICHT Cut 12
(Cut_chg 12 @0x027AA) gezeigt wird, bekommt jeder Zombie in BLK-Zone 0 (@0x01BBC, x -13470..-3660, z -32760..-23860) oder
Zone 10 (@0x01C34, x -21290..-13470, z -31360..-26960) = HINTER dem Tor Bit 0x20; mit dem ersten Bild von Cut 12 gibt die
Regel genau die frei, die sie angehalten hat. Ohne Cut-7-Teil zeigt die Tuer gleich Cut 12 -> die Regel greift nie (dasselbe
Bild wie bisher). NUTZER-VORGABE ("alle"), FORM = ROOM1141 sub02 + die Startregel der anderen Montage-Schnitte.

Gemessen im Riegel (KI RE2, Stand 28bb1d5d): Cut 7: 9 Zombies hinter dem Tor ab ihrem ersten Bild angehalten, kein Raum-Record
im Warte-Rechteck; Cut 12: alle 9 in BEIDEN Haelften naeher (kleinste Haelfte 1415, kleinste Gesamt-Annaeherung 4198 statt
vorher -120), 6 von 6 Aufrechten bekommen am Warte-Rechteck das Kriech-Bit, 5 von 9 am Schnittende unter dem Tor durch.
Gegenprobe-Experiment (Baustein 5 aus, sonst gleich; RE1.5-KI): 3 Zombies stehen in einer Haelfte statt 1.

### Riegel: "nimmt ueber die Schnittdauer ab"

* je Zombie d0 -> dm -> d1 (dm = Abstand in der Mitte der eigenen Sichtzeit, 16-Bilder-Raster, montage_r38.c s_probe); in
  BEIDEN Haelften mindestens R38_NAEHER_HALB = 400 naeher = ein Koerperradius (+0x78-Satz +6, FUN_8003a31c `lhu v0,6(v0)`
  @0x8003a408-410), ausgenommen nur, wer hinter die Kamera gelangt und dort angehalten wurde (Ziel erreicht). Das Vorbild
  Rolltor erfuellt es (5 von 5, kleinste Haelfte 1581). Die Gesamtschwellen 1000 (Gehen) / 500 (Tor) bleiben.
* Klemmen-Ausnahme entfernt (es gibt keine mehr).
* Kriech-Kriterium: jeder, der AUFRECHT ins Warte-Rechteck kommt (AOT-Stempel 5, @0x024E0), bekommt das Kriech-Bit.
* Baustein 5: alle Zombies hinter dem Tor ab ihrem ersten Bild in Cut 7 angehalten, kein Raum-Record im Warte-Rechteck.
* Gegenproben v0.8.25 rot ueber dieselben Masse (Cut 12 Hauptweg: in beiden Haelften naeher nur 3 von 9; ohne Cut-7-Teil:
  5 von 9 — "die aufrechten stehen nach der Haelfte am Tor").
* Die Ende-Zeile traegt die KI der Zombies (re15_ai_re2_for_type(0x10)); das Haelften-Kriterium ist unter der Standard-KI
  RE2 Pflicht, unter der RE1.5-KI (Menue-Option) wird es gemeldet (s. OFFEN).

## Umsetzung (Stand zweiter Lauf; Dateien, Konstanten mit Beleg)

Bausteine 1-3 (Vorgaenger, nachgeprueft) + zwei Korrekturen + Baustein 4 (neu):

| Datei | Aenderung | Beleg |
|---|---|---|
| engine/src/irons_tod_1150.c | EM_1130_DURCH / EM_1070_DURCH: Montage-Listen 1130 Cut 0 und 1030 Cut 7 in der Durchbruch-Form grid 0x0d (Typen, Tot-Bits, Lagen, Blick unveraendert) | STAGE1.BIN @0x80100f40-44 (0x201), RE2 EMZ0 @0x80100ab8-abc (0x101) |
| engine/src/irons_tod_1150.c | OP_PARK_1030_12 nach Cut_chg 12: Spieler auf die Kameraachse Cut 12, 3985,5 hinter die Kamera (Rolltor-Abstand), Gierung 845 | Form ROOM11B0 @0x012B2, Pos_set @0x80041048-70; RID ROOM1030 @0x1E0, ROOM1040 @0x80 |
| include/re15_irons_tod.h | **KORREKTUR** RE15_IT_PARK_1030 (-30708,-1699) -> **(-28558,-2183)**: Achse Cut 7, Mitte des Teils der BLK-Zone 7 hinter der Wand SCA 5 | BLK @0x01C10 (x1 -29340), SCA 5 @0x01A4C (x -27776), RID @0x140; FUN_80039e7c `bne a0,a1` @0x80039fbc / `sh s1,444(v0)` @0x80039fd0, Kopie @0x8003a040/@0x8003a058 |
| engine/src/montage_r38.c | Baustein 3: Einzelstopp hinter der Bildebene (Skalarprodukt mit der Blickrichtung < 0) in 1130/0, 1030/7, 1030/12 | STAGE1.BIN @0x80100450-5c, Ausgang @0x80100658 |
| engine/src/montage_r38.c | **Baustein 4 (neu)**: auf Cut 12 bekommt jeder Zombie im Warte-Rechteck (AOT-Stempel 5) einmal das Kriech-Bit member 0x10 \|= 0x1000, Set(5,i) | ROOM1030 sub06 @0x024C8.. (`21 05 i 00`, `3e 00 0f 00 05 00` @0x024E0, `22 05 i 01` @0x024E8), sub07 @0x02754 (`26 00 05 04 00 10` @0x02758), sub09 @0x02822 `59 05 04 01`; Torzelle SCA 18/28 nur fuer Kriecher frei: sub08 @0x0278E/@0x02792 `37 02 06 f7`/`37 03 06 f7` |
| engine/src/montage_r38.c | Messung (kein Spielverhalten): je Schnitt/Zombie Abstand zur Kamera-Bodenprojektion, Tiefe, Kriech-Bit, Endlage; je Schnitt Spielerlage, HP, "Navigator-Ziel fremd" (laenger als ein Pfadsuch-Takt +0x91 = 7 Bilder, @0x80039eb8/@0x80039ed4), Kamera ausserhalb der Montage-Schnitte | debug.log `[montage-r38]` |
| tests/unit/test_r38_montage.c | **KORREKTUR Riegel**: g_room_rdt/g_room_rdt_ok gesetzt (Navigator + Gegner-Wandklemme), RDT je Lauf frisch, RNG-Saat; Kriterien je Schnitt in EINER Bewertung fuer Prozess und exe (`auswerten <lauf>`) | — |
| tests/integration/test_r38_montage.cmake, probes/r38_montage.cmake | integration_r38_montage: echte exe ueber den echten Weg | — |

NUTZER-VORGABE / PORT-WAHL (keine Original-Szene): die Montage selbst; "Ziel = der geparkte Spieler hinter der Kamera"
(Form des Rolltor-Schnitts); die sub06-Regel ohne die Spiel-Drossel sub02 @0x021CC ("weniger als 4 im Hauptraum") und
ohne das Spiel-Bit (5,0x22) @0x024CC, weil der Nutzer ALLE Zombies auf der Kamera haben will.
Gemeinsame Dateien: nur irons_tod_1150.c / re15_irons_tod.h (VERTRAG §1.4: O). Kein shared_assets-Patch, keine neuen
Daten, keine Sprachdateien.

Experiment (verworfen, nicht im Code): Cut 12 OHNE das Umparken (Spieler bleibt in Zone 7) + Baustein 4 -> alle
Annaeherungen gleich oder kleiner (1: -2721 statt -2905, 6: -3083 statt -3650, 13: -2755 statt -3281, 14: -3709 statt
-4061, 4/5: -4120/-4281 statt -4269/-4386; 2/3/12 gleich) -> das Umparken bleibt.

## Messung nachher (ECHTE exe, gebaut 2026-10-17, Stand 0d9d544c + Messzeilen)

Weg wie vorher (Karte 1130 nach10f0 ersteszene, CONTINUE, Tuer 1130 -> 1150, Szene, Montage), Laeufe x_kette2
(RE15_IT_LOG=5) und integration_r38_montage (lauf_0ea873a3). Auswertung `O_belege/nachher_messung_x_kette2.txt`
(ausw.py) und `O_belege/nachher_auswertung_exe.txt` (test_r38_montage auswerten), Messzeilen
`O_belege/nachher_montage_zeilen_exe.txt`. Abstand zur Kamera-Bodenprojektion Anfang -> Ende des Schnitts:

| Schnitt | vorher (v0.8.25-Stand, exe d70c234e) | nachher (exe) |
|---|---|---|
| 1130 Cut 0, 176 Bilder | alle fuenf +0 (einer -7) | 6821->3961, 5648->1969, 4565->1734, 3521->438 (Stopp hinter der Kamera Bild 142), 4478->1295 (Stopp Bild 161); Navigator-Ziel = Spieler in 0 Bildern fremd; HP 100 |
| 1040 Cut 1 Rolltor, 405 Bilder | Endabstaende 7136/5663/8286/5395/7046, 5 durchs Tor, letzter Bild 270 | **bitgleich** (7136/5663/8286/5395/7046; Bilanz 5/5, letzter Bild 270, Kappe 0, HP 100) |
| 1030 Cut 7, 207 Bilder | Kopien alle +0 | 8272->4995 (-3277), 6235->1293 (-4942), 4644->517 (-4127), 7222->2015 (-5207), 5599->860 (-4739); Ziel fremd 0 Bilder; Kamera bleibt Cut 7 (keine fremde Kamera) |
| 1030 Cut 12, 335 Bilder | 1 -709, 2 +12, 3 -707, 4 -4031, 5 -4211, 6 -629, 12 -1565, 13 -350, 14 -13; 3 von 9 unter dem Tor durch | 1 -2905, **2 +120 (Klemme, s. OFFEN)**, 3 -1189, 4 -4269, 5 -4386, 6 -3650, 12 -779, 13 -3281, 14 -4061; die fuenf aufrechten (1,3,6,13,14) bekommen das Kriech-Bit (Bild 1/1/1/1/30); 6 von 9 unter dem Tor durch |

Die Zahlen des Riegels im Prozess (unit_r38_montage_s1030_re2 / _s1130_re2) sind seit der Riegel-Korrektur
**identisch** mit der exe (jede Zeile oben); die Gegenprobe im Riegel reproduziert die exe-Messung "vorher"
(Cut 12: 1 -709/-709, 3 -710/-707, 12 -1565/-1565, 13 -351/-350).

Bilder (RE15_FRAMEDUMP, RE15_WINDOW_SCALE=1, echte exe): `O_belege/nach_1130.png` (F25..F175: die fuenf gehen aus der
Gruppe vor der Briefing-Room-Tuer auf die Kamera zu, die vorderen verlassen das Bild unten), `O_belege/nach_1030.png`
(Cut 7 F30..F210: die Kopien kommen den Gang herauf bis direkt vor die Kamera, Koepfe fuellen das Bild; Cut 12
F240..F540: die Zombies hinter dem Gitter gehen zu Boden und kriechen unter dem Tor durch nach vorn). Vergleich vorher:
`O_belege/vor_1130.png`, `O_belege/vor_1030.png` (stehende Gruppen).

## Nebenwirkungen (geprueft)

* Elza: die Szene startet nur in Leons ROOM1150 (re15_irons_tod_install `case RE15_IT_RAUM_1150` = 0x1150; Elzas Raum
  ist 0x1151); montage_r38.c wirkt nur bei re15_irons_tod_zustand() != AUS in 1130/1040/1030 (Leon-Raeume). Riegel
  unit_r36_elza_irons / integration_r36_elza_irons im gezielten Lauf (s. Tests).
* Spaeteres Betreten von 1130/1030: die Nachspawn-Programme (k_p_1130_nach, k_p_1030_nach_tuer/_tor) sind unveraendert;
  RE15_IT_PARK_1030 wird nur in den Montage-Tueren benutzt (irons_tod_1150.c k_p_1040_ende OP_DOOR, tuer_setzen ohne
  Cut-7-Teil). Bank 5 (Kriech-Marken Set(5,i)) ist raumlokal.
* Ohne Cut-7-Teil (1070 leer, (9,76)=0): Tuer zeigt gleich Cut 12, Kriech-Regel + Umparken gelten dort ebenso
  (Riegel unit_r38_montage_s1030_ohne7_*, exe-Weg integration_r35_cut1150 Lauf B).
* Rolltor 1040: bitgleich (Riegel unit_r38_montage_rolltor Bild fuer Bild; exe Endlagen gleich).

## OFFEN

1. **Klemme an der Zellenecke SCA 30 (Cut 12, KI RE2) — vorbestehend, nicht durch Spur O.** Ein Kriecher (Raum-Record
   ROOM1030 main00 Bit 1 = Aktor 2) haengt am Zone-9-Uebergang fest: Abstand zur Kamera +12 (exe vorher, d70c234e),
   +1 (Gegenprobe im Riegel), +120 (jetzt; die Nachbarn kriechen zum selben Uebergang und schieben ihn nach Westen).
   Mechanismus (gelesen + gemessen, Spur-Ausgabe `test_r38_montage s1030_re2 mess 2`): der Navigator setzt das Ziel auf
   den Uebergang Zone 0 -> 9 (-12195,-23860) (Mitte der Ueberlappung x -13070..-11320, BLK @0x01C28; FUN_8003a31c
   selbst disassembliert: Mitte `addu v0,v1,a3` / `sra v0,v0,1` @0x8003a3f0-f4, die eigene x nur wenn lo + 2r < x <
   hi - 2r, r = `lhu v0,6(v0)` aus dem +0x78-Satz @0x8003a408-410, `sll v0,v0,1` @0x8003a418, Ersatz `sh v0,0(a2)`
   @0x8003a440); dieser Punkt liegt nur 371 von der Ostkante der Zelle SCA 30 (@0x01B78,
   x -15518..-12566, z -24134..-22694) — weniger als der Zombie-Radius 400 (+0x78-Satz +6). Wer von WESTEN kommt,
   drueckt in die Ecke, die Wandklemme setzt ihn auf z -24552 zurueck (Zyklus ~50 Bilder). Navigator (FUN_8003a524 /
   FUN_8003a31c) und Wandklemme sind byte-true Ports anderer Spuren (nav_zone_common.c, re15_collision.c,
   enemy_ai_common.c = B/T); eine Loesung waere dort ein Eingriff gegen das Original und gehoert nicht zu Spur O. Unter
   der RE1.5-KI tritt sie nicht auf (alle 9 naeher, unit_r38_montage_s1030_re15). Der Riegel laesst hoechstens EINE
   solche Klemme zu und verlangt, dass es dasselbe Bit ist wie in der Gegenprobe v0.8.25 (sonst rot).
2. Cut 12 endet (Original-Sleeps sub08 @0x027BC..@0x027C4 + Port 120, Runde 35) mit 6 von 9 Zombies unter dem Tor
   durch; 3 (startet erst in Bild 30 aus der zweiten Reihe) und 12 (wartet am Uebergang) sind noch im Durchgang —
   beide kommen der Kamera naeher (-1189 / -779). Die Schnittlaenge ist nicht Teil des Auftrags (unveraendert).

## Fuer den Nutzer

* Neue Assets fuer das Paket-Gate: keine. Sprachdateien: keine. Keine Bedienung.
* Text fuer die Release-Notes (Vorschlag): "Montage nach Irons' Tod (Leon): Die Zombies, die in den Gang vor dem
  Briefing Room und vor die Tuer zu Raum 1070 durchbrechen, gehen jetzt auf die Kamera zu — wie die hinter dem Rolltor.
  Am aufgebrochenen Gitter-Tor in der Lobby kriechen jetzt alle Zombies hinter dem Gitter unter dem Tor durch nach vorn
  (vorher standen die aufrechten nur hinter dem Gitter). Der Rolltor-Schnitt ist unveraendert."
* Bekannt (OFFEN 1): an der linken Torecke kann ein Kriecher haengen bleiben (war schon vorher so).
