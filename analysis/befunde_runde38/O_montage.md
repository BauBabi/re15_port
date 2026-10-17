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
- [ ] Schritt 2b: Bauen
- [ ] Schritt 3: Messen nachher + Riegel

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
