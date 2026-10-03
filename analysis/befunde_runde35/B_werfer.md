# Runde 35 — Spur B "werfer": Granatwerfer, Raketenwerfer, Flammenwerfer, Colt Python

Baum: `.claude/worktrees/r35_werfer`, Zweig `r35/werfer`, Basis master 154a73c1. Beginn 2026-10-03.

Auftrag (woertlich, AUFTRAG.md):
> - Einige Waffen, wie die Granatwerfer oder der Raketenwerfer gehen noch nicht
> - Andere Waffen wie der Flammenwerfer oder die Colt Python gehen noch nicht richtig.

Waffen-Ids (Item-Katalog DAT_800c4a28, inventory_common.c s_item_names): 14 = FLAMETHROWER,
15/16/17 = GRENADE LAUNCHER (Explosiv/Saeure/Brand — Subtyp-Byte +8 der Munitions-Records
@0x80074cb4/b8/bc = 03/01/02, dieselbe Kennung wie die Handgranaten 9/10/11), 18 = ROCKET LAUNCHER,
20 = COLT PYTHON. Munitions-Items 0x19 EXPLOSIVE RND, 0x1a ACID ROUNDS, 0x1b INCEND. ROUNDS,
0x18 FLAME FUEL, 0x17 MAGNUM BULLETS.

Dossier wird FORTLAUFEND geschrieben (Sitzungsabbrueche moeglich).

## 0. Arbeitsprotokoll (chronologisch)

- 2026-10-03 09:55 Baum geprueft (`git status --porcelain` leer, HEAD 154a73c1). AUFTRAG/VERTRAG gelesen.
  Spur B hat KEINE Bank-9-Bits, Nachrichten-IDs, AOT-Slots (raumunabhaengige Waffen-Mechanik).
- 10:10 Pflichtlektuere: SPEC analysis/waffen_fsm_2026-09-12/SPEC.md, Memories dauerfeuer-fsm,
  waffen-banken, runde34-granaten, beta-zu-retail; r34-Dossier re_saeure_brand.md §2 (RE2-GL-Runden).
- 10:20 configure+build im eigenen Baum (`local_build.sh configure` + `build`): `=== LOCAL-BUILD-OK (build)`.
- 10:30 Messung VORHER (Abschnitt 1), exe-Kopie `re15_port/build/platform/pc/re15_r35b.exe`,
  Laeufe unter `re15_port/build/platform/pc/mess_r35b/vorher/w<ID>/` (nicht versioniert).
- 10:45-11:30 RE-Belege (Abschnitt 2): RE1.5-Tabellen, RE2-Effekt-Handler, RE2-FX-Ops, Baenke, SE-Banken.

## 1. Messung VORHER (Stand 154a73c1)

Harness (Muster tests/integration/test_r34_granaten.cmake): `RE15_NO_INTRO=1 RE15_NOAUDIO=1
RE15_DEBUG_JUMP=1140@250 RE15_PLAYER_POS=-1676,-18070,1076 RE15_AI_FLAVOR=re2 RE15_GIVE=<id>:6
RE15_EQUIP=<id> RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1
RE15_INPUT_SCRIPT="M0.6,MA1.5,M1.0,W4" RE15_STATE_LOG RE15_WAFFEN_LOG RE15_FX_LOG RE15_WPN_DBG=1
RE15_EXIT_AT=200#1140`. Alle sechs Laeufe exit 0 (kein Absturz, kein Haenger). Gegner nach dem
Sprung: slot2 Zombie 0x10 HP 50 bei (-1800,-19600) = 1535 Einheiten vor Leon, slot3 0x10 HP 80,
slot4/5 0x11 HP 250.

| Id | Bank (wpnbank.log) | gemessen (wf.log / state.log / fx.log) |
|---|---|---|
| 14 Flammenwerfer | W0E 16 Clips | Dauerfeuer-FSM laeuft (Hold 9, Feuer 7 fc=21, Abklingen 8). Je 3. Bild `SPAWN id=3 sub=29 scale=0x1200 streams=1` = EIN Platz an der Muendung mit `drift=(0,-70,0)`, `xlat=(0,-75,0)`, naechstes Bild `(0,-145,0)`: der "Strahl" steigt SENKRECHT nach oben, keine Vorwaertsbewegung (fx.log F20-F27). 6 Fuel in 10 Bildern weg (Schuss an Bild 0-3, 8-9). Kein Gegner verliert HP (Hitscan-Reichweite 1000 < 1535; Flamme trifft nichts). Leer: `SE arms_rec=0` (0x01000001). |
| 15 GL Explosiv | W0F **11 Clips** | Heben Clip 6 fc=**1** (sofort), Hold 8 fc=1, Feuer Clip 7 fc=36 (F20-F54), Nachfeuern F55. `fx=0` IMMER: kein Projektil, keine Muendung, kein Knall, keine Explosion. Kein HP-Verlust (Hitscan 1000). Munition 6 -> 4. |
| 16 GL Saeure | W10 14 Clips | Heben Clip 6 fc=19, ab F24 "Hold" Clip 8 fc=**9**, nie Clip 7 (rec=0 bleibt): das Feuer-Clip 7 hat fc<=1, der Rueckstoss endet im selben Tick -> jedes Bild ein Schuss: **6 Granaten in 6 Bildern** (F24-F29), fx=0, kein Projektil. |
| 17 GL Brand | W11 (= W10 byte-gleich, md5 a480735f…) | identisch zu 16. |
| 18 Rakete | W12 14 Clips | Heben 6 fc=20, Hold 8, Feuer Clip 7 fc=33 (F26-F57), Nachfeuern F58. fx=0: keine Rakete, keine Explosion, kein Ton. Munition 6 -> 4 (RE1.5 gibt 4 Schuss vor, s. 2.1). |
| 20 Colt Python | W14 14 Clips | Heben 6 fc=10, Hold 8, Feuer Clip 7 fc=26. TRIFFT (Hitscan): slot2 HP 50 -> 34 -> 18 (F19/F44), Reaktion ss1=13 (RE2-Zeile 13 Colt S.A.A., Schaden 16 aus dem RE2-Record). fx=25 nur Blut. KEINE Muendung, KEIN Knall, KEINE Huelse; RE1.5-Schadensspalte 20 = 0 fuer alle Nicht-RE2-Gegner; kein Nachladen moeglich (s_wpn_props endet bei 15). |

Ursachen im Code (Stand vorher): game_step_common.c ENT[15..18]/[20] = `{1,1,1,0}` = Hitscan-Resolve
ohne Effekte (PORT-BRUECKE seit 2026-09-12, Kommentar "Original-Handler NULL"); kein Projektil, kein
RE2-FX; player_common.c Clip-Formel 6/7+2u+4d/8+2u+4d/13 passt nicht auf die Bank W0F (11 Clips) und
W10/W11 (Dauerfeuer-Form); inventory_common.c s_wpn_props[16] kennt 16..20 nicht; Flammenstrahl =
RE1.5-CORE00 Effekt 3 sub 5 (ein aufsteigender Puff) mit param 3000.

## 2. RE-Belege

### 2.1 RE1.5 — was der Auslieferungsstand hat (alles PSX.EXE, re15_disasm.py)

**Dispatch @0x80074030** (`table 0x80074030 21`): [15..18] -> 0x80032e9c (Standard-FSM), [20] ->
0x00000000 (NULL: R1 mit Waffe 20 waere ein jalr-0-Absturz). **Entlade-Tabelle @0x80074100**: [15..18]
= NULL, [20] = 0x80034060 (Tabellen-Ueberlappung, Dauerfeuer-Sub 0). [14] = 0x800c45a8 (DEBUG.BIN).

**Tester-Dispatch @0x8006e548** (`table`): 14/15/16/17/18/20 -> 0x800128a0 (Sprengstoff-Tester, wie
9/10/11), 19 -> 0x80012574 (Schusswaffe). **Reichweite @0x8006e5a0** (`read --w 4`): alle 1000 bis
auf 1=1100, 6=1200, 8=1500, 12=1300, 13=1800, 19=1100.

**Spieler-Schaden @0x8006e0d0** (`read 22 --w 2 --stride 4 --rows 0x48 --rowstride 0x58`, Zeile =
Gegnertyp, u16 je Waffe): Typ 0x10..0x1a (Zombies) `[0, 6, 24, 5, 5, 15, 15, 200, 40, 100, 200, 100,
10, 100, 10, 100, 200, 100, 400, 20, 0, 100]` -> w14 = 10, w15 = 100, w16 = 200, w17 = 100,
w18 = 400, w20 = **0**. Typ 0x20 (Hund) `[.., 7:200, .., 15:100, 16:100, 17:200, 18:400, 20:0]`,
Typ 0x22 `[.., 7:200, 9:200, 10:50, 11:50, 15:200, 16:50, 17:50, 18:400, 20:0]`, Typ 0x23
`[.., 7:50, 15:70, 16:30, 17:30, 18:400, 20:0]` usw. **Spalte 20 ist in JEDER Zeile 0** (Colt Python
unfertig), Spalte 18 ueberall 400.

**Waffen-Records @0x80074da8** (Stride 0xC: u32 Magazin, ptr Munitions-Record, u8 Subtyp, u8 nachladbar):
```
[14] 64 00 00 00  a8 4c 07 80  02 01   Flamme: 100, Record 0x80074ca8 = 18 0e (FLAME FUEL), nachladbar
[15] 06 00 00 00  88 4c 07 80  03 00   GL Explosiv: 6, Record 0x80074c88 = NULL-Record (00 00 00 00)
[16] 06 00 00 00  88 4c 07 80  01 00   GL Saeure:   6, NULL-Record
[17] 06 00 00 00  88 4c 07 80  02 00   GL Brand:    6, NULL-Record
[18] 04 00 00 00  88 4c 07 80  03 00   Rakete:      4, NULL-Record
[19] 64 00 00 00  88 4c 07 80  03 00   MC51:      100, NULL-Record
[20] 06 00 00 00  88 4c 07 80  03 00   Colt Python: 6, NULL-Record
```
Die Munitions-Records der GL-Runden EXISTIEREN aber (@0x80074c88 `bytes`): 0x80074cb4 `19 0f 02 00`
(EXPLOSIVE RND fuer Waffe 0x0f), 0x80074cb8 `1a 10 04 0c` (ACID ROUNDS fuer 0x10), 0x80074cbc
`1b 11 04 0c` (INCEND. ROUNDS fuer 0x11), 0x80074cb0 `15 13 02 00` (H.GUN fuer MC51) — alle OHNE
Verweis aus der Record-Tabelle (unverdrahtet). Nachlade-Gate der FSM `sltiu id,9` @0x80033368 (nur
Ids < 9 laden nach).

**Animationsbaenke** (PLW dir[0] = EDD, eigener Parser plw_clips.py; Clip-Laengen):
```
PL00W03 (Pistole, Referenz-Layout) 14: [22,16,52,1,50,30, 6:10 Heben, 7:23 Feuer, 8:1 Hold, 9:24 Feuer-hoch, 10:1, 11:24 Feuer-tief, 12:1, 13:32 Nachladen]
PL00W07 (Super Redhawk)            14: [22,16,52,1,50,30, 8, 26, 1, 25, 1, 25, 1, 40]
PL00W08 (M870)                     14: [20,16,52,1,50,30, 15, 34, 1, 36, 1, 36, 1, 65]
PL00W0E (Flamme)                   16: [20,31,39,1,50,30, 15, 21,5,1, 21,5,1, 21,5,1]  (Dauerfeuer-Form)
PL00W0F (GL Explosiv, Leon)        11: [22,31,39,50, 4:15 Heben, 5:34 Feuer, 6:1 Hold, 7:36, 8:1, 9:36, 10:1]  = M870-Feuerclips (34/36), OHNE Clip 3 (1), 5 (30) und 13 (Nachladen) -> Basis -2
PL00W10 = PL00W11 (GL Saeure/Brand, Leon) 14: [22,31,39,50, 16, 9,19,1, 9,18,1, 9,18,1] = Ingram-Form (W0C) ohne Clips 3/5 -> KEIN Granatwerfer-Clip (Platzhalterbank, 223 kf)
PL00W12 (Rakete)                   14: [20,16,52,1,50,30, 20, 33, 1, 33, 1, 33, 1, 80]  Standard-Layout
PL00W14 (Colt Python)              14: [22,16,52,1,50,30, 10, 26, 1, 27, 1, 27, 1, 50]  Standard-Layout
PL04W0F = PL04W10 = PL04W11 (Elza, md5 26acffcc) 14: [20,16,52,1,105,30, 15, 39, 1, 39, 1, 39, 1, 32]  Standard, Feuer 39 = RE2 Claire-GL
PL04W12 (Elza Rakete)              14: [20,16,52,1,105,30, 20, 33, 1, 33, 1, 33, 1, 80]
PL04W14 (Elza Python)              14: [22,16,52,1,105,30, 10, 26, 1, 27, 1, 27, 1, 50]
RE2 PL01W09 (Claire GL)            16: [30,20,50,34,20,30,45,22,30, 9:15 Heben, 10:39 Feuer, 11:1 Hold, 12:39, 13:1, 14:39, 15:1] (9 gemeinsame Clips, dann dieselbe Folge; kein Nachlade-Clip)
RE2 PL00W11 (Leon Rakete)          12: [.. 9 gemeinsame .., 20, 33, 1]  (KEINE hoch/tief-Clips)
RE2 PL00W05 (Magnum)               17: [.. 9 .., 10, 36, 1, 36, 1, 36, 1, 32]
RE2 PL01W0D (Colt S.A.A.)          17: [.. 9 .., 8, 8, 1, 8, 1, 8, 1, 45]
RE2 PL00W10 (Flamme)               20: [.. 9 .., 15, 21,5,1, 21,5,1, 21,5,1, 35]
```
PLW-md5-Gruppen Leon: W00=W01=W02, W03=W04, W05=W06, W09=W0A=W0B, W10=W11; W0F, W12, W14 je eigen.
Elza: W0F=W10=W11 (EINE Bank fuer alle drei GL-Varianten, wie RE2 PL01W09=0A=0B).

**SE-Baenke** (EDH-Deskriptoren = erste pBAV_off Bytes, 4 Byte je Record; VAG-Groessen aus dem VH):
```
RE1.5 ARMS0F (GL Expl): 16 Rec: 0 00001336 (Schuss) 1 00005216 3 00007217 10 00003320 (Aufschlag) 14 00006216; VAG [48,4672,12800,1824,6896,4176]
RE1.5 ARMS10 (GL Acid): wie 0F;  VAG [48,4672,10992,1824,6896,4176]
RE1.5 ARMS11 (GL Brand): wie 0F; VAG [48,4672,11664,1824,6896,4176]
RE2   ARMS09/0A/0B:  0 00001436 1 00005416 3 00007417 14 00006416 17 00003320; Aufschlag-VAG 12800/10992/11664 (r34-Dossier §2.6)
  -> RE1.5 ARMS0F/10/11 tragen DIESELBEN Samples wie RE2 ARMS09/0B/0A (Schuss 4672 B, Aufschlag 12800/10992/11664 B), nur der Aufschlag-Record liegt bei 10 statt 17.
RE1.5 ARMS12 (Rakete): 0 00001320 (Schuss), 10 00003320 (Explosion); VAG [48,18256,17104]
RE2   ARMS11 (Rakete): 0 00001436, 1 00005416, 20 00003320 (= Op-47-Ton 0x01140001)
RE1.5 ARMS0E (Flamme): 0 00001300 1 00005300 3 00007217 12 00003300 14 00006216; VAG [48,12928,2624,2352,3520,14368]
RE2   ARMS10 (Flamme): 0 00001416 1 00005417 3 00007400 11 00002417 12 00003400 13 00004401 14 00006400
RE1.5 ARMS07 (Redhawk): 8 Rec 0 00001330 1 00003310 2 00004210 3 00000310 4 00005310 5 00006311 6 00007311 7 00009314; VAG [48,11616,2752,2976]
RE1.5 ARMS14 (Python): 10 Rec 0 00001330 1 00003310 2 00004210 3 00005310 4 00006310 5 00006311 6 00007311 7 00009314 8/9 00004210; VAG [48,15808,1472,1296,7200,9824]
RE1.5 ARMS03 (Pistole): 0 00001336 1 00003300 2 00004201 3 00005300 4 00007300 14 00006301
  -> ARMS14 hat das Record-Layout des REVOLVERS ARMS07 (00001330/00003310/00004210/00006311/00007311/00009314), nicht das der Pistolen (…36/…00/…01): die Python ist in RE1.5 als zweiter Magnum-Revolver angelegt.
```

**Flammenwerfer-Handler 0x800C45A8 (DEBUG.BIN)**: Effekt `0x031D1200` je 3. Bild, **a1 = 3000 statt
Gier** (@0x800c466c), Rauch-jal entfernt (SPEC §2). RE1.5 CORE00.ESP Effekt 3 sub 5 (core00_subs.txt
@0x0574): 1 Strom, Z00 `A=10 acc=(0,5,0) vel=(0,-80,0)`, Z01 `vel=(0,-75,0)` — ein aufsteigender Puff
ohne Routine, die +0x2e (Gier/param) liest; genau das misst fx.log (drift (0,-70,0)). Der RE1.5-
Flammenwerfer ist damit ein Debug-Platzhalter (Patch-Konstante 3000, Puff statt Strahl, Hitscan).

**Einordnung (Beta -> Retail, Memory reai-v2-beta-zu-retail):** 15..18 = Entlade-Handler NULL +
Munitions-Zeiger NULL + Schaden vorhanden; 20 = Dispatch NULL + Schaden 0 ueberall; 14 = Debug-Patch
mit Puff. Alle vier sind NACHWEISLICH UNFERTIG -> RE2 Retail ist das Ziel fuer Entladung, Projektil,
Treffer, Effekt und Ton. Was RE1.5 fertig hat und bleibt: Animationsbaenke (W0F/W12/W14 mit
hoch/tief), Magazingroessen (6/6/6/4/6 @0x80074da8), Schadensspalten 15..18 (100/200/100/400) und
die eigenen SE-Samples (ARMS0F/10/11/12/14), Reichweiten.

### 2.2 RE2 Retail — Entlade-Effekt-Handler je Waffe (info/re2leon/PSX.EXE, re2_disasm.py)

Per-Waffe-Effekt-Tabelle @0x800A6FDC (Index = RE2-Waffen-Id; r34-Dossier §2.2: gelesen
`lhu v0,0x10e(s1)` / `jalr` @0x800431ac-cc im Feuer-Zustand, jedes Bild mit In-Clip-Bild +0x14D):
[5] 0x800444e0 Magnum, [9] 0x80044b44 GL Expl, [10] 0x80044f44 GL Brand, [11] 0x80045090 GL Saeure,
[13] 0x8004522c Colt S.A.A., [16] 0x800454a0 Flammenwerfer, [17] 0x80045588 Rakete.
Entlade-Tabelle @0x800A6F90: [5]/[13]/[17] = `lhu a0,270(a0)` / `jal 0x8006a0cc` (eine Patrone),
[16] = `jr ra` (Fuel laeuft ueber den 8-Bild-Zaehler in FUN_8006a0cc @0x8006a130-… DAT_800d5c1c).
Spawner FUN_8001bf10(a0 = Bank<<24|Sub<<16|Skala, a1 -> Platz+0x22 (Gier), a2 = Matrix (32 B),
a3 = Versatz-Tripel); Sub&7 = Skript, Sub>>3 = CLUT-Zeile (re2_fx.c spawn_kern).

**Rakete [17] @0x80045588** (nur Bild `+0x14D == 1`, `lbu v1,333(s1)` / `addiu v0,zero,1` @0x8004559c-a4;
Matrix `lw s0,408(s1)` + 1964 = Pose+0x7AC = Waffenknochen):
```
800455ac ori a0,a0,0x2000 / lui 0x100 -> 0x01002000  a1=0, ofs {0,1100,0}  @0x800455a8-d0  Muendung (Bank 1 Skr 0)
800455d8 lui a0,0x20d / ori 0x1000    -> 0x020D1000  a1 = lh 118(s1) = Spieler+0x76 GIER, ofs {0,1100,0} @0x800455d8-ec  RAKETE (Bank 2 Skr 5, CLUT 1)
800455f0 lui 0x30a / ori 0x1a00       -> 0x030A1A00  a1=0, ofs {0,1100,0}  @0x800455f0-604  Rauch (Bank 3 Skr 2)
80045608 lui 0x30a / ori 0x1500       -> 0x030A1500  a1=0, ofs {300,-900,0} @0x80045608-2c  Rauch hinten (Rueckstrahl)
```
**Flammenwerfer [16] @0x800454a0**: jedes Bild mit `+0x14D % 3 == 1` (Magic 0xAAAAAAAB @0x800454b8-e0):
`0x031D1200`, **a1 = lh 0x800cfc6e = Spieler+0x76 (GIER)** (@0x80045500), ofs {150,1200,0}
(@0x800454f0-508), danach `0x800df349 := 1` (@0x80045518-20, Flammen-Latch; Leser @0x80026784/8002693c).
Bild 1: SE `0x01000001` (@0x80045534-44 jal 0x8005ba28) + FUN_80043d30(player) (@0x80045548);
Bild 11: SE `0x010B0001` (@0x80045550-6c). Vergleich RE1.5 DEBUG.BIN: gleicher Effektcode, aber
3000 statt Gier und Takt %3==0 statt %3==1.
**Magnum [5] @0x800444e0**: Bild 1: `0x030A1400` ofs {110,670,-10} (@0x80044504-38) + `0x01001D00`
gleiche ofs (@0x8004453c-50); Bild 4: `0x03080900` ofs {330,50,0} zweimal (@0x80044560-a8) +
`0x02000900` a1 = Gier+1684 (Huelse, @0x800445ac-d0).
**Colt S.A.A. [13] @0x8004522c**: Bild 2: `0x01001000` {130,730,0}, `0x01041000` {130,280,0},
`0x01061000` a1 = Gier+2048, `0x030A0C00` {130,280,0}, `0x030C1000` {130,830,0}, `0x01073000`
{130,910,0} ueber FUN_8001c224 (@0x80045240-32c).
**GL [9]/[10]/[11]**: r34-Dossier re_saeure_brand.md §2.2 (Bild 1: `0x01002000` + 5 x `0x020C0A00`
mit acc/vel-Ueberschreibungen @0x80044c18-e74 bzw. 1 x `0x020C1000` a1 = Gier + `0x03081200`).

### 2.3 RE2 Retail — FX-Skripte und Ops (CORE00.ESP RE2, re2_core00_scripts.txt; Optab @0x8009D868)

```
Bank 2 Skr 4 (GL-Runde 0x020C): S00 OpA 0 OpB 17, step[2]=47 step[3]=47, acc (-10,0,0), step[0xB]=15, vel (0,640,0)   (r34 §2.2)
Bank 2 Skr 5 (Rakete 0x020D):  S00 OpA 1 anim 18 vel (-70,768,0) st 0xB403; S01 OpA 23 OpB 24 anim 47 b3 47 vel (-70,768,0)
Bank 3 Skr 5 (Flamme 0x031D):  S00 OpA 1 anim 0 st 0xB403 tp 0x20; S01 OpA 0 OpB 70 anim 10 acc (0,20,0) vel (0,20,0)
Bank 3 Skr 2 (Rauch 0x030A):   S00 OpA 1 anim 24 st 0xB003 (nur Op 1)
Bank 1 Skr 0 (Muendung 0x0100): S00 OpA 0 OpB 7 st 0xB803 tp 0x20 rr 256
```
Ops (re2_disasm.py dis): **Op 23 @0x8001f6a4** (Rakete Op A): jedes Bild `FUN_8001cbe8(0x030A1800, 0,
Identitaet 0x8009db44, &Lage+0x34)` = Rauchspur. **Op 24 @0x8001f6e0** (Rakete Op B): Box @0x80010908
(lwl/lwr nach sp+32); Lage ausserhalb +-32000 -> tot (`slti 32001` @0x8001f724/738, `sb zero,0 / sh zero,24`
@0x8001f744-48); Treffer `FUN_800470c0(&Lage, Gier +0x22, Box, 0x30011)` @0x8001f7b0-c0 -> Lage nach
DAT_800cfb88/8c/90, Dispatch Op[step[2]] = 47 (@0x8001f7fc-804 / 9dc-ec); sonst Boden
`FUN_8004fba0(&P,2,8192,0)` @0x8001f810, P.y := Boden, +0x14 := Boden-10 (@0x8001f838-40), zweiter Treffer
0x20011 @0x8001f83c -> Op 47; sonst Wand (DAT_800dcbc8 != 0 @0x8001f868-70): vel -= acc, xlat -= vel,
vel -= vel/3 (0x55555556 @0x8001f874-9c0), `jal 0x8001d894` (Weltlage) -> Op[step[3]] = 47. KEIN
Flaechenschaden: **Op 47 mit Sub 13** (`lbu v1,30 / addiu v0,zero,12 / bne` @0x80020d30-38) spielt nur
SE 0x01140001 (@0x80020d3c/dc0-c4) und die Kinder 0x031F1700/0x040C1F00/0x041D1700 (Skalen +2560,
@0x80020d1c-2c s1 = (Sub-12)*5<<9) — der Raketen-Schaden kommt allein aus dem Flugtreffer 0x30011/0x20011.
**Op 47 Sub 12** (GL Explosiv): SE 0x01110001 + Schaden 0x10020009 Box @0x80010918 {-2000,0,1000,500}
bei y und y+900 (@0x80020d40-b4), Phasen 1-3 Pause, Phase 4 Kind 0x04152700, Platz frei (r34 §2.4).
**Op 70 @0x80023204** (Flammenstrahl Op B): Box @0x80010964; step[2]-- (@0x80023244-50), bei 0 ->
acc.y := -10 (@0x80023268-70); step[3]==0 (Flug): Wasser (FUN_800527b4 @0x80023364, unter Spiegel:
Skala*65/100 @0x80023398-d4, Kind 0x030F2000, s0=3), **Treffer `FUN_800470c0(&Lage, Gier +0x22, Box,
0x20010)`** (@0x800233dc-f4; Treffer -> Lage nach DAT_800cfb88.., s0=2), Boden `FUN_8004fba0(&P,2,8192,0)`
unter der Lage -> s0=1 (@0x80023434-58), Wand DAT_800dcbc8 -> Skala*70/100, s0=1 (@0x8002345c-b0);
s0 != 0: acc.x/vel.x/acc.y := 0, Anim 12, vel.y := 20, step[2] := 6, step[3] := 1 (@0x800234b4-538);
step[3]==1 (Nachbrennen): bei step[2]==3 Kind 0x040C2000 (@0x8002329c-b8), dann 0x0506xxxx mit
rng (@0x800232c8-32c).
Trefferzustellung FUN_800470C0 = Port `re15_re2_gl_apply` (re15_damage.c, re2fx_applier).

(Fortsetzung folgt: Boxen @0x80010900.., Op 15/17/22/7, Audio-Pfad, Umsetzung.)

## 3. Umsetzung

(folgt)

## 4. Messung nachher

(folgt)

## 5. Tests

(folgt)

## 6. OFFEN

(folgt)

## 7. Fuer den Nutzer

(folgt)
