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

**Boxen (RE2 `bytes 0x80010900 112`, je {s16 x0, 0, x1, Halbbreite}):** @0x80010900 {-1400,0,350,250}
(Op 15 Flug), @0x80010908 {-800,0,400,200} (Op 24 Rakete), @0x80010910 {-600,0,300,150} (Op 40),
@0x80010918 {-2000,0,1000,500} (Op 47 Explosion), @0x80010920 {-1000,0,500,250} (Op 47 zweite Box,
nur fuer Sub 13 adressiert, ungenutzt), @0x8001093c {-1200,0,600,300} / @0x80010944 {-600,0,300,150}
(Op 48/49), @0x80010964 {-1400,0,400,200} (Op 70 Flamme).

**Op 15 @0x8001ed9c (GL-Flug, 228 Instr.)**: Wasser (`jal 0x800527b4` @0x8001ede8; unter dem Spiegel:
y += 500 @0x8001ee34-3c, Spritzer 0x1A051C00 @0x8001ee1c-38, Op[step[2]+Art]); Lebensdauer-- (@0x8001ee64-70);
Boden `FUN_8004fba0(&P,2,8192,0)` @0x8001eea0 -> +0x14 := Boden-10 (@0x8001eeac-bc); Treffer
`FUN_800470c0(&P, Gier +0x22, Box, 0x30009 + Art)` bei y+1000 (@0x8001eec8-d8) und y-1000 (@0x8001eef8-f08);
Treffer oder Lebensdauer 0 -> Status |= 0x80 (@0x8001ef44-50), Lage -> DAT_800CFB88/8C/90 (@0x8001ef58-70),
Op[step[2]+Art] (@0x8001ef74 / @0x8001f0e4-104); sonst Wand (DAT_800DCBC8 @0x8001ef84-8c): Rueckprall
vel -= acc, lokal -= vel, vel -= acc, lokal -= vel/3 (0x55555556 @0x8001ef90-0x8001f0c8), Weltlage
(`jal 0x8001d894` @0x8001f0cc), Op[step[3]+Art] (@0x8001f0e0).
**Op 17 @0x8001f198** (Start): Art := Waffen-Id(0x800CFD06) - 9 (@0x8001f1a8-b8), Op A := 22, Op B := 15
(@0x8001f1c4-d8), Status 0xB403 (@0x8001f1e4-e8), Anim 18 (@0x8001f1ec-f0), TPage |= 0x20 (@0x8001f1f4-204),
Lebensdauer: Id 9 -> 10 + rng%3 (@0x8001f224-74), sonst 15 (@0x8001f284); dann `jal 0x8001ed9c` (@0x8001f288).
**Op 22 @0x8001f634** (Rauchspur): 0x030B0000 | Skala*150/100 (@0x8001f648-90), a1 = 0, Einheit 0x8009DB44.
**Op 7 @0x8001e154** (Bank 1 Skr 0 Muendungsblitz): Sub += step[3] (@0x8001e164-74), SE (+0x16 << 16) + 1
= 0x01000001 (`lhu a0,22 / sll 16 / addiu 1` @0x8001e188-b8) bzw. 0x031F0001 bei Art != 0 (@0x8001e1bc-c4),
Latch 0x800DF349 := 1 (@0x8001e1dc-e4), dann Op 1 (@0x8001e1e8). **Der Schussknall der RE2-Werfer kommt
also aus dem Muendungs-Effekt** (wie RE1.5 Routine 9), nicht aus dem Handler.
**Op 59 @0x800223f8** (Bank 5 Skr 6): Op 2, +0x1B := 2, +0x42 := rng%4 + 10 (@0x80022400-48).
**RE2-Pumpe FUN_8001d300**: s0 = 0x800D8CF0 + 11904 = Pool-Ende (@0x8001d314); FUN_8001d68c sichert s0
nicht -> Op 70 liest in seinem Nachbrenn-Zweig `bne s0,2` @0x800232bc den Pool-Ende-Zeiger (uninitialisierte
Variable) -> der Zufalls-Puff 0x0506xxxx (@0x800232c8-32c) ist in RE2 toter Code.

**Fuel FUN_8006a0cc Id 16 (@0x8006a184-0x8006a21c, Delay-Slot-genau):** Zaehler DAT_800d5c1c++
(@0x8006a194-a0), `slti v0,v0,8 / bne` (@0x8006a1ac-b0) -> < 8 nichts; `sh zero` (@0x8006a1bc) Zaehler := 0;
`beq s0,zero` (@0x8006a1b8) Menge 0 -> 0; `addiu s0,s0,-1 / jal 0x800694b8` (@0x8006a1c0-cc) Menge-1;
`bne s0,zero,0x8006a20c` (@0x8006a1fc) -> Menge jetzt 0 -> return 0 (@0x8006a204); sonst `addiu a1,s0,-1 /
jal` (@0x8006a20c-10) zweite Einheit, return 1. Ids 15/18 (SMG/Gatling) laufen denselben Doppelabzug.
Der SMG-Aufrufer @0x80047e70-88 geht bei Rueckgabe 0 mit v0 = 3 nach 0x80048064 — ohne SE.

**RE2-Feuerzustand, Spieler +0x14D** = In-Clip-Bild (RE1.5 acae9): GL-/Raketen-Handler spawnen bei 1,
Flamme bei %3 == 1 (SE 1/11), Magnum bei 1 und 4, Colt S.A.A. bei 2.

### 2.4 Audio

RE1.5-Weg: Bank 1 = ARMS der gefuehrten Waffe (FUN_80043d8c), Knall = Routine 9 des Muendungs-Effekts
(0x01000001 = Satz 0). RE2-Weg: Bank 1 = ARMS der Waffe (FUN_80059c74), Knall = Op 7 des Muendungs-
Effekts 0x0100xxxx (0x01000001 = Satz 0), Aufschlag-/Explosions-Toene aus den Ops (0x01110001 Satz 17
Explosiv, 0x01120001/0x01130001 Satz 18/19 Brand/Saeure, 0x01140001 Satz 20 Rakete), Flamme Satz 0/11.
Sample-Identitaet RE1.5 <-> RE2 (VAG-Groessen): GL-Explosiv RE1.5 ARMS0F {4672, 12800, 1824, 6896, 4176} =
RE2 ARMS09 (Aufschlag-VAG 12800 B liegt in RE1.5 bei Satz 10 `00003320`, in RE2 bei Satz 17); Acid/Brand
analog (10992/11664 B); Rakete: Schuss 18256 B gleich, Explosion RE1.5 17104 B vs RE2 12672 B (anderes
Sample); Flamme: RE2 ARMS10 {2624, 2352, 3520, 14368, 8928} — RE1.5 ARMS0E hat 8928 (Satz 11, Strahl) nicht.
RE2 EDH-Layout = RE1.5 EDH-Layout (Satz-Deskriptoren 4 B @0, `pBAV` @ u32[size-8]; ARMS10/11: 0x80).

## 3. Umsetzung (Commit-Kette r35/werfer; jede Konstante mit @0x im Code)

Einordnung je Waffe (Beta -> Retail): 15..18 und 20 sind nachweislich unfertig (§2.1) -> RE2 Retail ist
das Ziel fuer Entladung/Projektil/Treffer/Effekt/Ton; RE1.5 bleibt fuer Baenke, Magazine, Schadensspalten,
Reichweite, eigene Toene massgeblich. 14 ist in RE1.5 ein Debug-Patch mit Puff -> RE2-Handler.

### 3.1 Neue Dateien
* `re15_port/include/re15_werfer.h`, `re15_port/engine/src/werfer_r35.c` — Werfer-Modul:
  - `re15_werfer_tick()` (je Spielbild hinter dem Feuerpfad): bei Id 15..18 im Rueckstossbild 1
    (`lbu v1,333 / addiu v0,zero,1 / bne` @0x80044b98-a0 / @0x8004559c-a4) die RE2-Spawns mit der
    Waffenknochen-Matrix (`re15_player_gunbone_matrix`, Pose+0x7AC-Analogon) und Gier = Spieler-Gier
    (+0x76 RE2 = +0x6a RE1.5): GL Explosiv 0x01002000 + 5 x 0x020C0A00 (Ueberschreibungen acc.x/vel
    @0x80044c18-e74, Tabelle @0x80011030, HOCH = Zielbit 15), Brand/Saeure 0x01002000 + 0x020C1000 +
    0x03081200 (@0x80044f68-c8 / @0x800450b4-114), Rakete 0x01002000/0x020D1000/0x030A1A00/0x030A1500
    (@0x800455a8-62c). Versatz GL {120,1200,0}, Rakete {0,1100,0} + {300,-900,0}.
  - `re15_werfer_flamme_bild(f)`: RE2 [16] @0x800454a0 — 0x031D1200 mit Gier bei f%3==1, Versatz
    {150,1200,0}, SE RE2-ARMS10 Satz 0 bei f==1, Satz 11 bei f==11.
  - `re15_werfer_fuel_bild()`: FUN_8006a0cc Id 16 (Zaehler, 2 je 8 Bilder, leer -> 0).
  - `re15_werfer_clip_remap`, `re15_werfer_bank_id`, `re15_werfer_nachladbar`, `re15_werfer_recoil_break`.
  - Bezugsebene des RE2-Bodentests := Standhoehe des Spielers (`re2fx_boden_basis_setzen(pl->y)`),
    PORT-ZUORDNUNG wie granate_boden (Runde 34) fuer RE1.5-Raeume mit Boden != 0.
* `re15_port/shared_assets/RE2/SOUND/ARMS10.EDH/.VB`, `ARMS11.EDH/.VB` (Kopien aus
  info/re2leon/COMMON/SOUND; md5 74d9982e… / a074d427… / 1b9abbaf… / 6c5fa622…) — NEUE ASSETS fuer das
  Paket-/Android-Gate.
* `re15_port/tests/unit/test_r35_werfer.c`, `tests/unit/probes/r35_werfer.cmake`,
  `tests/integration/test_r35_werfer.cmake`.

### 3.2 re2_fx.c — Block "RUNDE 35 SPUR B" (am Dateiende) + 9 Faelle in op_rufen
Ops 7 (@0x8001e154), 15 (@0x8001ed9c), 17 (@0x8001f198), 22 (@0x8001f634), 23 (@0x8001f6a4),
24 (@0x8001f6e0), 47 (@0x80020c3c), 59 (@0x800223f8), 70 (@0x80023204) wie in §2.3 belegt;
Wand-Rueckprall `wand_rueckprall()` (@0x8001ef90-0x8001f0d0); `kind0_an_lage` = FUN_8001cbe8 mit a1 = 0;
Treffer-Lage DAT_800CFB88.. und Latch 0x800DF349 als Buchfuehrung (`re2fx_r35_treffer_lage/latch`).
Abweichung dokumentiert: Op 24 beendet bei |x|/|z| > 32000 (Original faellt nach dem Freigeben durch).

### 3.3 re15_damage.c
* `re15_player_gunbone_matrix(rot, t)` neben `re15_player_gunbone_world` (gleiche Quelle, Rueckfall Pose).
* Applier `re15_re2_gl_apply`: Zeilen 16/17 fuer RE2-Typen mit den Records (§3.3a); fuer RE1.5-KI-
  Kandidaten die RE1.5-Angriffsart je Hitcode-Zeile aus DAT_8006f430/DAT_8006f418 (Zeile 9 -> Art 6 = w15
  100, 11 -> Art 7 = w16 200, 10 -> Art 8 = w17 300, 17 -> Art 9 = w18 1000, 16 -> Art 5 = w14 50;
  Bodenfeuer 0x2002000A bleibt Art 5), Sperre der Zeilen 16/17 aus w1 des Records.
* `re15_enemy_take_damage_at`: E4-Modellauswahl (RE2-Record statt RE1.5-Flachzahl) auch fuer die Arten
  6..9; `gl`-Zeilen 16/17.
* Hitscan: Waffe 20 nimmt die Schadensspalte 7 (Redhawk) und den Schuss-Streifen 0x80012574 (PORT-WAHL §3.5).
* 3.3a Records Zeilen 16/17 (`read` 2026-10-03, Zeiger *(0x800A6A88+Typ*4), Zeile r @ Basis+(r-1)*20):
  Zombie 0x800A4258/5C 0x00F03C0F/0x02850A0A (15/15/15, Sperre 5), 0x800A426C/70 0x384E1384/0x078F1E0A
  (900, Sperre 15); Zombie 0x16 0x800A43D4.. 0x00C0300C (12); Hund 0x800A4550.. 0x00C0300C / 0x800A4564
  0x12C4B12C (300); Kraehe 0x800A46CC 0x00F03C0F/0x078F1E0A (15, Sperre 15) / 0x12C4B12C; Spinne
  0x800A4CBC 0x00A0280A (10) / 0x0C8320C8 (200); Arm 0x800A52AC 0x0280A028/0x03870E0A (40, Sperre 7);
  G5 0x800A6008 0x00501405/0x0102040A (5, Sperre 2) / 0x0C8320C8 (200).

### 3.4 Haken in gemeinsamen Dateien (klein, benannt "Runde 35 Spur B")
* `game_step_common.c`: Include; Nachlade-Gate `eq_item < 9 || re15_werfer_nachladbar(eq_item)`;
  ENT[15..18] = {1,0,1,0} (nur Munition; FX/Treffer ueber RE2), ENT[20] = Redhawk-Zeile 0x800339A4
  (0x02000E00 {0x8c,0x25d,0} @0x800339a8-e4, 0x03001000 {0x91,0x1f4,-25} @0x800339ec-1c, keine Huelse,
  Resolve + 1 Patrone @0x80033a34-3c); `re15_werfer_tick()` hinter dem Granaten-Block; Dauerfeuer-Zweig
  14: `re15_werfer_flamme_bild(anim_frame)` statt (3,0x1D,3000) und `re15_werfer_fuel_bild()` statt
  (acae9&4)==0 x Hitscan.
* `player_common.c`: `s_aim_clip_n`; `aim_clip_wirksam()` (Renderer + Bildzahl) ueber
  `re15_player_werfer_clip`; Rueckstoss-Schwelle 10 fuer 15..18/20.
* `inventory_common.c`: `s_wpn_props[21]` (Zeilen 15..20, Belege im Kommentar), Grenzen 16 -> 21.
* `enemy_ai_re2_zombie.c`: `re2z_row_from_weapon[20] = 5`; `re2z_row_from_atktype[7]=11, [8]=10, [10]=5`
  (DAT_8006f430-Semantik: Art 7 = w16 Saeure, Art 8 = w17 Brand — vorher vertauscht).
* `main.c`: Bank/Mesh/dir[3]-TIM fuer 16/17 -> W0F (3 Einzeiler).
* `audio_pc.c`: `re15_audio_re2_arms_se()` (RE2-ARMS-Bank aus shared_assets/RE2/SOUND), ARMS_ZUSATZ_N 3.
* `fx_plattform_pc.c`: `re15_pc_re2fx_se`: 0x01000001 -> ARMS Satz 0 der Waffe, 0x01110001 -> ARMS0F
  Satz 10, 0x01140001 -> RE2 ARMS11 Satz 20.
* `tests/test_support.c`: Stub `re15_audio_re2_arms_se` (zaehlt).

### 3.5 PORT-WAHLEN (gekennzeichnet) und ihre Belege
1. **Colt Python = zweiter Magnum-Revolver** (statt RE2 Colt S.A.A.): ARMS14 traegt das Record-Layout des
   Redhawk ARMS07 (`00001330 00003310 00004210 … 00006311 00007311 00009314`, Pistolen: `…36/…00/…01`),
   Bank W14 = Revolver-Clips (10/26/27/50 ~ W07 8/26/25/40), Magazin 6 wie w7 (@0x80074e98 / @0x80074df8).
   Folgen: Entladung = Redhawk-Handler 0x800339A4, Schaden = Spalte 7, RE2-Zeile 5, Munition MAGNUM
   BULLETS 0x17 (Record 0x80074c9c `17 07 02 00` des Redhawk), Tester = Schuss-Streifen.
2. **GL 16/17 fuehren Bank/Mesh/TIM der W0F** (Leon): PL00W10 = PL00W11 ist Ingram-foermig (Platzhalter);
   Elza PL04W0F = W10 = W11, RE2 PL01W09 = 0A = 0B (eine Bank je Werfer).
3. **PL00W0F-Clipumsetzung** (11 Clips, Basis -2; Nachladen -> Hold-Clip, Nachladen sofort): Leon hat
   keinen Nachlade-Clip, RE2 Claire auch nicht (16 Clips ohne 13), Elza hat ihn (PL04W0F Clip 13 = 32).
4. **Nachladen fuer 15/16/17/20** (RE1.5-Gate `sltiu id,9` @0x80033368 laesst sie nicht): die Munitions-
   Records existieren unverwiesen (0x80074cb4/b8/bc), Magazine 6 — die Verdrahtung ist der fehlende Rest.
5. **Rueckstoss-Schwelle 10** fuer 15..18/20 (Saetze @0x800740d6-ef sind 0 = unfertig; 5..13 = 10).
6. **Bezugsebene des RE2-Bodentests = Standhoehe** (wie granate_boden, Runde 34).
7. **Schaden an RE1.5-KI-Gegnern** ueber die RE1.5-Resolver-Arten 6..9/5 (DAT_8006f418/430, byte-true
   Daten ohne Original-Aufrufer); RE2-Modell-Typen bekommen die RE2-Records (E4-Regel der Runde 34).
8. Flamme ohne Leer-Ton (RE2 `j 0x80048064` ohne SE); die Flamme brennt nach dem letzten Fuel bis zum
   naechsten Takt-Bild weiter (RE2 prueft nur im Takt-Bild).

## 4. Messung NACHHER (Commit 9e6852e2 + Tests; gleiche Harness wie §1, exe-Kopie re15_r35b.exe, Laeufe `re15_port/build/platform/pc/mess_r35b/nachher/w<ID>/` und `.../re15ki/w<ID>/`)

RE2-KI (RE15_AI_FLAVOR=re2), Leon (-1676,-18070) Blick 1076, Zombie slot2 0x10 HP 50 in 1535:

| Id | wf.log (RE2SPAWN / SE) | state.log (Gegner) |
|---|---|---|
| 14 | je 3. Bild `RE2SPAWN a0=031d1200 a1=1079 ofs=(150,1200,0)` (11 Strahlen), `SE re2arms ARMS10 satz=0` (Bild 1) und `satz=11` (Bild 11); Fuel 6 -> 0 nach 24 Schleifenbildern (mag=0 ab F44), Schleife endet im naechsten Taktbild | slot2 HP 50 -> 35 -> 20 -> 5 -> -10 (15 je Treffer, alle 5 Bilder = Sperre 5 der Zeile 16), ss1=16, Tod -> Leiche st=7; danach slot3 80 -> ... -> -10 |
| 15 | Rueckstossbild 1 (F21): `01002000` + 5 x `020c0a00 a1=1079`, Explosionen `SE re2fx code=0x01110001 -> ARMS0F Satz 10` (5x, je Teilgeschoss), Knall `0x01000001 -> ARMS Satz 0` (Op 7) | F21 slot2 50 -> -150 (200 = Zeile 9 Klammer 0), slot3 80 -> 30, slot5 250 -> 200, slot4 (F25) 250 -> 200 (50 = Klammer 1), ss1=9; Tote -> st=7 |
| 16 | `01002000`, `020c1000 a1=1079`, `03081200`; `SE 0x01130001 -> ARMS10 Satz 10` | slot2 50 -> -150, ss1=11 (Saeure) |
| 17 | wie 16, `SE 0x01120001 -> ARMS11 Satz 10` | slot2 50 -> -150, ss1=10 (Brand) |
| 18 | F26: `01002000`, `020d1000 a1=1079`, `030a1a00`, `030a1500 ofs=(300,-900,0)`; `SE 0x01140001 -> RE2 ARMS11 Satz 20`, Knall Satz 0 | slot2 50 -> -850 (900 = Zeile 17), ss1=17 (re2z_death_rip), st=3 -> st=7; zweite Rakete F60 slot3 -> -820 |
| 20 | `SPAWN id=2 sub=0 scale=0xe00 streams=2`, `id=3 sub=0 scale=0x1000`, `SE arms_rec=0 bank=20`; KEINE Huelse | F19 slot2 50 -> -850 (900 = Zeile 5 Magnum), ss1=5, Tod |

Bilder (RE15_FRAMEDUMP, `mess_r35b/nachher/w15/f_000030.png` usw.): GL-Explosionswolke (Bank 3/4
RE2-Billboards) an den Zombies, Granatwerfer-Mesh W0F in Leons Haenden; Flammenstoss vor der Muendung
(w14 f_000030); Raketen-Explosion und Raketenwerfer-Mesh W12 (w18 f_000030); Python W14 mit
Muendungsblitz (w20 f_000020). Vorher-Bilder (vorher/w15, w14) zeigen keine Effekte bzw. den Puff.
(gdigrab lieferte in dieser Sitzung weisse Bilder; deshalb RE15_FRAMEDUMP.)

RE1.5-KI (RE15_AI_FLAVOR=re15, Leon (-4311,-19289) Blick 0): alle sechs Laeufe exit 0 und `[flow]
EXIT_AT` erreicht (kein Haenger auf den RE1.5-Reaktionszeilen 14/15/16/17/18): 14: slot2 75 -> 60 -> 45
-> 30 (15 je 15 Bilder, Zeile 14 -> Zombie steht wieder auf); 15: slot2/3 -> -125/-135 (RE2-Record
200 unter dem Import-Modell), ss1=15; 16/17: ss1=16/17, Leichen st=7 ab F76; 18: ss1=18, -825; 20 (in
2530 ausserhalb der Streifen-Reichweite: kein Treffer) und mit der nahen Aufstellung (w20nah) ebenfalls
kein Treffer VOR der Streifen-Korrektur — der Hitscan lief fuer 20 durch den begrenzten Kegel 0x800128A0
(Reichweite 1000 + Radius < 1535). Nach der Korrektur (Streifen wie w7) trifft die Python auch unter
RE1.5-KI (unit_r35_werfer Teil F, Treffer in 800).

## 5. Tests

* `unit_r35_werfer` (tests/unit/test_r35_werfer.c, 60 Pruefungen, OK): A Daten (Magazine/Munitions-
  Records, Bank-/Clip-Umsetzung, Schwelle), B GL-Runde (Op 17 Art/Lebensdauer, Op 22 + Op 15 je Bild,
  zwei Applier 0x30009 je Bild mit Box {-1400,0,350,250} und y+-1000, Wand -> Op 47: SE 0x01110001,
  2 x 0x10020009 Box {-2000,0,1000,500} y/y+900, drei Kinder, Phasen 1..4 -> Platz frei; Treffer ->
  sofort; Saeure -> Op 49 + 0x01130001 mit Hitcode 0x3000B, Brand -> Op 48 + 0x01120001 mit 0x3000A;
  Lebensdauer 10..12), C Rakete (Op 23 + Op 24, 0x30011/0x20011 Box {-800,0,400,200}, Treffer -> Op 47
  Sub 13 = 0x01140001 ohne Flaechenschaden, Wand -> Op 47), D Flammenstrahl (Op 70 je Bild, 0x20010 Box
  {-1400,0,400,200}, Vortrieb, Zaehler 10 -> acc.y -10, Treffer -> Nachbrennen, Puff bei step[2]==3),
  E Fuel-Takt (7 Bilder nichts, 8. Bild 2 Einheiten, leer -> 0), F Python/Rakete/GL/Flamme gegen den
  RE1.5-KI-Zombie (Treffer, Tod, +0x5 = 20/18/15/14, RE2-Zeile 5, DAT_8006f430[6..10]).
* `integration_r35_werfer` (tests/integration/test_r35_werfer.cmake, echte exe, 6 Laeufe ROOM1140,
  94 s, Passed): je Waffe die RE2-Spawns im Rueckstossbild 1, die Toene, HP-Verlust und Reaktionszeile
  (15: ss1 9; 16: 11; 17: 10; 18: 900/ss1 17; 14: ss1 16 + Fuel leer; 20: Redhawk-FX ohne Huelse, 900/ss1 5),
  EXIT_AT erreicht (kein Haenger). Exit -1 (fremder Kill) wird einmal wiederholt.
* Suite: s. Abschluss (local_build.sh all).

## 6. OFFEN (mit Adresse und naechstem Messweg)

1. **Flammen-Latch 0x800DF349** (Op 7 / Flammen-Handler @0x80045518-20): Leser @0x80026784 / @0x8002693c
   in der RE2-Gegnerschleife nicht portiert (vermutlich "Laerm/Licht fuer Gegner"). Naechster Weg:
   `re2_disasm.py dis 0x80026760 40` und `0x80026920 24`, Konsument bestimmen.
2. **FUN_80043d30(player)** im Flammen-/Werfer-Handler Bild 1 (Tabelle 0x800A6ED0 (id-2)*6, FUN_8003947c /
   FUN_80039514): nicht portiert (vermutlich Muendungs-Licht/Kamerazittern). Weg: beide Funktionen
   disassemblieren; sie werden auch von Magnum/Schrot gerufen (@0x80043a94, @0x80045720, @0x800489b8).
3. **Flamme: Elevation**: der RE2-Strahl fliegt im Knochenrahmen (Status 0x400) — hoch/tief folgt
   dem Knochen; nicht einzeln gemessen (Clips W0E 10/13 vorhanden). Weg: wf.log mit MU/MD-Skript + Lage
   der Strahl-Plaetze (re2fx_platz +0x34..38).
4. **Explosiv-GL: Trefferzonen (Klammer k)** im Applier fuer RE1.5-KI-Typen = Zone + 3k mit k aus dem
   Hitcode (0x10020009 -> k = 1): die Gore-Spalten der RE2-Zombies sind nur im RE2-Modell erreichbar.
5. **Wasser-Spritzer 0x1A051C00** (Op 15 @0x8001ee1c-38): Bank 0x1A ist eine Raum-ESP-Bank (nicht in
   CORE00) — spawn liefert -1, kein Spritzer in Raeumen mit Wasser. Weg: RE2-Raum-ESP-Banken registrieren.
6. **Rakete: Explosions-Sample**: RE2 ARMS11 Satz 20 (12672 B) statt RE1.5 ARMS12 Satz 10 (17104 B) —
   Vertrag "Sound ist RE2"; wer das RE1.5-Sample will, aendert nur fx_plattform_pc.c (0x01140001).
7. **Elza (PL04)**: Baenke W0F/W10/W11/W12/W14 sind Standard-14-Clip-Baenke (Nachlade-Clip 13 = 32 Bilder
   vorhanden) — die Umsetzung greift nur bei 11 Clips; nicht mit Elza gemessen.
8. **Pose+0x7AC vs. 0x7A4**: RE2 nimmt die Knochenmatrix bei +0x7AC, RE1.5 bei +0x7A4 — der Port
   nimmt in beiden Faellen den gerenderten Knochen 11 (R_gunbone/T_gunbone); die RE2-Versatzrichtung
   (+y Lauf, +x oben) stimmt mit der RE1.5-Muendung ueberein (gemessen: Runden fliegen in Blickrichtung,
   Aufschlaege an den Zombies vor Leon).

## 7. Fuer den Nutzer

* Keine neuen Sprachzeilen (synchro/): nichts aufzunehmen.
* NEUE ASSETS fuer das Paket-/Android-Gate: `re15_port/shared_assets/RE2/SOUND/ARMS10.EDH`,
  `ARMS10.VB`, `ARMS11.EDH`, `ARMS11.VB` (RE2-Flammenwerfer- und Raketen-Toene; ohne sie bleiben
  Flammen-Ignition/-Strahl und die Raketen-Explosion stumm, alles andere laeuft).
* Bedienung: Granatwerfer (3 Munitionsarten) und Raketenwerfer feuern im Rueckstossbild 1 ein RE2-
  Geschoss (fliegt in Blickrichtung, explodiert an Wand/Boden/Gegner bzw. nach 10..15 Bildern; die
  Rakete nur bei Treffer/Wand). Nachladen: GL mit EXPLOSIVE/ACID/INCEND. ROUNDS (6 je Magazin), Python mit
  MAGNUM BULLETS (6); Rakete 4 Schuss ohne Nachladen. Flammenwerfer: 100 Fuel = 400 Bilder Dauerfeuer
  (2 Einheiten je 8 Bilder), Zombies brennen nach 10 Treffern (RE2-Zeile 16).
* Item-Debug: SELECT + R1/L1 am Statusschirm (Runde 34) oder RE15_GIVE="15:6,25:12" / RE15_EQUIP=15
  (Munitions-Ids 0x19 = 25, 0x1a = 26, 0x1b = 27, 0x17 = 23).
