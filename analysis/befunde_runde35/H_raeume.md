# Runde 35 Spur H "raeume" — Dossier (fortlaufend)

Baum `.claude/worktrees/r35_raeume`, Zweig `r35/raeume`, Basis master 154a73c1.
Zuteilung (VERTRAG.md): Nachrichten-IDs ROOM1190/1191 = 6..11, Ereignis 27 (1190). Keine Bank-9-Bits.

## Punkte (Wortlaut AUFTRAG.md)
1. ROOM 1190: Hunde haben einen Schatten waehrend sie durch die Luke springen (in der Luft) -> muss raus.
2. ROOM 1190: Zielscheibe ganz links + 3. von links: "This target has a surprisingly large number of bullet holes."; die anderen beiden: "This target does not have many bullet holes".
3. ROOM 1200: nach Minidisc-Player steht der Trage-Zombie auf und laeuft durch die Luft, statt auf die Spieler-Ebene herunterzukommen.
4. ROOM 1210: Zombie-Arme beim Greifen nicht synchron zu Leon beim Schuetteln (Clipping).

Messwerkzeug: Kopie der exe unter eigenem Namen (`re15_pc_r35h_mess.exe` neben der exe), Lauf aus einem
Arbeitsordner im Scratchpad. Bilder per `RE15_FRAMEDUMP` (gdigrab liefert in dieser Sitzung weisse Bilder,
s. Auftrag). Bau: `local_build.sh configure` + `build` am Basisstand 154a73c1 = OK.

---

## Punkt 1 — Hunde-Schatten beim Sprung durch die Luke (ROOM1190)

### 1.1 Messung vorher (Basisstand, gebaute exe)
Lauf: `RE15_DEBUG_JUMP=1190@250 RE15_SUBSTART=10@60#1190 RE15_STATE_LOG RE15_FRAMEDUMP=100-520/4`
(sub10 = das Hunde-Ereignis des Raums, s. 1.2). State-Log Hund Slot 1:
F181 st4 ss2=1 Clip 0x14 (Absprung) -> F197 ss2=2 z=-27480 -> F205 ss2=3 Clip 0x15 (Flug) z=-25368 ->
F221 st1 (gelandet) z=-22401. Hund 2 springt F213..F253, Hund 3 F245..F285. Kamera = Cut 11.
Bilder `H_raeume/p1_vorher_F184-216.png` (8 Bilder) und `p1_vorher_F200_F204.png` (vergroessert):
**unter dem Hund haengt ein dunkler Schatten-Quad mitten an der Wand, in Hoehe der Pfoten** — genau der
Nutzerbefund. Ursache im Port: `main.c` NPC-Schatten `int32_t nsh_y = npc->y;` — fuer Typ 0x20 wird der
Schatten auf die KOERPER-Hoehe gelegt (nur die Kraehe 0x21 hat einen Boden-Zweig).

### 1.2 RE-Belege
ROOM1190 SCD (scd_dump_room.py): sub13 `Sce_em_set 44 00 20 40 02 ... b2 02 f0 f1 ac 90` = Hund Typ 0x20,
grid 0x40, floor 2, (690,-3600,-28500); sub10 @0x027E6 `Member_set 0x0C=0x43` (grid 0x43 = Absprung-Freigabe)
je Hund, Hund 1/2 zusaetzlich `Member_set 0x01=-3600` (y). sub11 dreht die Luke (obj 4).

RE1.5 — Sprung-Maschine FUN_80111398 (Zustand 4 Sub 0, STAGE1_full/FUN_80111398.c):
`case 1: if (0xc < +0x95) { +0x8c += 6; 245d8(0); +0x38 -= 0x14; +0x1ba = 0; }` — ab Bild 0xD steigt der
Koerper (+0x38 = y), die BODEN-Referenz +0x1ba wird 0 (Raumboden). `case 3: 0x8001c1a4(+0x8c,0,-0x1e,+0x1ba)`
ballistisch bis +0x1ba.
RE1.5 — Hunde-Root FUN_8010d7f8 (Tabelle @0x80120f74), letzte Zeile, IMMER (auch im Pausen-Zweig):
`func_0x8001b064(_DAT_800ac784 + 0xb0, (int)*(short *)(_DAT_800ac784 + 0x1ba));`
FUN_8001b064 (RE_15_Quellcode_V2): `local_88.t[1] = (long)param_2;` — die Quad-Hoehe IST der 2. Parameter =
**+0x1ba (Boden), nicht +0x38 (Koerper)**. X/Z = +0x34/+0x3c + gedrehter Versatz (+0xb8/+0xba). Gezeichnet nur,
wenn `FUN_80014368(+0x34, DAT_800ac790)` (Punkt im Cut-Viereck) — der Port hat das als `npc_region_culled`.

RE2 (Ziel, wo RE1.5 unfertig ist — hier nur Gegenprobe, RE1.5 ist vollstaendig):
* Allokator 0x80016480 (PSX.EXE RE2): `sw a1,20(t0)` @0x800164DC = rec+0x14 Zeiger auf die Position,
  `sb 5,14(t0)` @0x800164D4 (belegt), `sw a2,4(t0)` @0x80016530 (Halbmasse). Hund ruft ihn mit &+0x38
  @0x80100238-64 (EMD0G_MOD0.BIN).
* Entity-Schleife: @0x80026868 `lw a0,20(s1)` / @0x8002686c `jal 0x8004fba0` (Boden-Sonde an der Position) /
  @0x80026874 `sh v0,450(s0)` = **+0x1C2 = Boden-Y**, dann @0x800268c8 `lh a1,450(s0)` / @0x800268d0
  `lh a2,118(s0)` / @0x800268d4 `andi a3,a3,0x400` / @0x800268d8 `jal 0x800168b4`.
* 0x800168b4 (RE2_Quellcode_V2/FUN_800168b4.c): `param_1[5] = param_2` = rec+0x0A = **Quad-Y = +0x1C2**;
  rec+8/rec+0xC = Position X/Z aus dem Zeiger rec+0x14; Bit 4 von rec+0x0E an, bei word0 & 0x400 aus.
* Zeichner 0x8001699c: `lbu v1,8(s1)` @0x80016A04 / `andi v0,v1,0x4` @0x80016A0C / `andi v0,v1,0x8`
  @0x80016A14, Hoehe `lh v0,4(s1)` @0x80016A4C = rec+0x0A.
* Fenster-Sprung des RE2-Hundes 0x80102C78: setzt nur +0x1C0 |= 2 (@0x80102CE8) und +0x1D3 |= 0x80, KEIN
  Schatten-Ausblenden (Suche `ori ...,0x400` im ganzen EMD0G_MOD0.BIN: kein Treffer) — der Schatten bleibt
  auch in RE2 beim Sprung AM BODEN (+0x1C2).

**Befund:** RE1.5 und RE2 legen den Hunde-Schatten auf die Boden-Referenz (+0x1ba bzw. +0x1C2). Der Port legt
ihn auf den Koerper — deshalb "fliegt" der Schatten mit. Kein Original blendet ihn beim Sprung aus; er liegt
dort, wo der Hund landen wird (Raumboden y=0, +0x1ba := 0 @FUN_80111398 case 1), d.h. unter Cut 11
(Kamera blickt zur Luke hoch) ausserhalb des Bildes bzw. am Boden — NICHT in der Luft.

### 1.3 Umsetzung
(folgt)

## OFFEN
- (laufend)
