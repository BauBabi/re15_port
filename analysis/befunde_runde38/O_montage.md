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
- [ ] Schritt 1c: Framedumps 1040/1030 (Bild)
- [ ] Schritt 2: RE-Belege der Rolltor-Form + Bauen
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
