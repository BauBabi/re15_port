# Runde 37 Spur H "zombieaktiv" — Abnahme 1 (unabhaengig, nach Nachbesserung 1)

Datum 2026-10-12. Baum `.claude/worktrees/r37_zombieaktiv`, Zweig `r37/zombieaktiv`, HEAD 3261690c, Merge-Basis 44881ac2.
Gepruefter Diff: `git diff master...HEAD` (drei Punkte). Seit Abnahme 0 (9c2a825d) geaendert: nur Kommentare in
`re15_zombieaktiv_r37.h` / `irons_tod_1150.c`, die Schreibweise von `EM_1130_NACH` (Zeilenfortsetzungen) und Dossier/Belege
(`git diff 9c2a825d HEAD -- re15_port`).

## Ergebnis

| Nutzer-Punkt (Wortlaut) | Urteil | Kern-Beleg (eigene Messung) |
|---|---|---|
| 1 "das soll nicht nur da gelten, das darf generell für die Zombies gelten — schaue einfach nach wie Resident Evil 2 das macht" | **erfuellt** | RE2-Mechanismus selbst nachgelesen (Deskriptor 6 -> 0x0101 Gang @0x80100ab8-abc, Grundwort 0x0001 Stehen @0x801006ac-b8); die RE1.5-Aktivform grid 0x00 (FUN_80101b64, ENGAGE ohne Abstandsgrenze, STAGE1.BIN; Girl-Kette STAGE4.BIN Zeile fuer Zeile nachgelesen) wird ueber den Sce_em_set-Weg in JEDEM Raum Deskriptor 6: an der exe 1250/1180/1181(Elza)/11E0/1010/4050(Girl, echte Tuer)/1030 nach der Montage — HEAD Gang ab Spawn, NUTZER-exe Stehen; 0x02/0x0d/Liegende/Fresser Zeile fuer Zeile wie v0.8.24 (E5-E7) |
| 2 "die Zombies sind immer noch zu passiv ... nach Irons tod - die Zombies bleiben einfach nur Stumpf alle im Gang stehen. So sind sie keine Bedrohung .... In Resident Evil 2 bewegen sie sich doch auch teilweise aktiv im Gang." | **erfuellt** | GANZE Kette in einem Lauf (Irons' Tod -> Montage -> Rueckkehr -> Tuer -> 1130, E1): HEAD alle fuenf Gang ab F6, Griff F896; NUTZER-exe alle fuenf stehen 900 Bilder. Leon geht den Gang (E2): HEAD Griff F459, Leon tot; NUTZER Pulk steht am Gangende (Bild F700/F750) bis Leon 5000..7000 heran ist |

**Gates:** RE-Gate haelt (Abschnitt 2), Vertrag haelt (3), Tests gruen (4: 4/4 Spur, 181/181 Unit, 44/44 Integration).
**Maengel der Abnahme 0 (M1/M2): behoben** — M1 selbst nachgelesen (2.1), M2a Zensus selbst gefahren (E5), M2b an der exe
nachgemessen (E4).

**bestanden: JA** — keine Maengel. Hinweise H1-H6 (Abschnitt 6) sind keine Maengel dieser Spur.

## 0. Bau

`bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.`,
`LOCAL-BUILD-OK (build)`. Arbeitsbaum sauber. exe `re15_port/build/platform/pc/re15_pc.exe` 14:31:49; letzte Quell-Aenderung
`re15_zombieaktiv_r37.h` 14:24:43 (Commit d3d92ecd), HEAD 3261690c aendert nur `analysis/` -> die exe entspricht HEAD.

## 1. Messungen an der exe (eigene Laeufe)

Sitzungs-Ablage `scratchpad/` = `C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/` (nicht eingecheckt; Auftrag: nur diese Datei committen).
Werkzeuge (`scratchpad/h_abn1/tools/`): `fahr.sh` (Kopie der exe unter eigenem Namen
`re15_pc_r37h_abn1_<name>.exe` neben die Baum-exe, Lauf im eigenen Ordner, Kopie danach geloescht; KEIN Software-Renderer),
`ausw.py` (EIGENER Auswerter: state.log je Besuch (Bildzaehler faellt) und Zombie (Typ 0x10-0x13/0x16/0x18): Zustandsfolge
st/ss1, erstes Bild im RE2-Gang 1/1, Abstand aus den LAGEN Leon<->Zombie, Weglaenge, Leon-HP).
exe HEAD = Spurbaum `re15_port/build/platform/pc/re15_pc.exe` (14:31:49). exe NUTZER = Hauptbaum
`re15_port/build/platform/pc/re15_pc.exe` (09:02, Stand v0.8.24 ohne Spur H; `git diff 44881ac2 master -- re15_port` leer),
nur gelesen/kopiert.

### E1 — Nutzer-Beispiel, GANZE Kette in einem Lauf (Irons' Tod -> Montage -> Rueckkehr -> Tuer 1150 -> 1130), Leon steht
Karte `probe_r35_cut1150_karte re15_card.mcr 1130 nach10f0 ersteszene` (ROOM1130 vor Irons' Tuer, Irons lebt, alle
1140-/1070-Zombies leben) + `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1`, Skript `W1,A0.2,W200` ab Spielbild 60 (Aktion an der
Tuer), Tuer 1150 -> 1130 nach der Rueckkehr ueber `RE15_FIRE_AOT=0@1450#1150` (Slot 0 = Tuer nach 1130), Ende
`RE15_EXIT_AT=900#1130`. debug.log (HEAD und NUTZER gleich): `CONTINUE: resumed in room 1130` -> `loaded room1150` ->
`loaded room1130` -> `room1040` -> `room1030` -> `room11c0` -> `room1150` -> `Rueckkehr beendet, Steuerung frei` ->
`Bilanz Kette: Spieler-HP 100 -> min 100` -> `[fire-aot] slot=0 at F1450 (Raum 1150)` -> `loaded room1130` ->
`Nachspawn 1130 (Programm 10)` -> `EXIT_AT: Bild 900 in Raum 1130`.
```
HEAD  Besuch 7 (1130 nach der Montage), F1..F900, Leon (-5900,16350), HP 100 -> 100
  slot 1 t=16 g0=0d Gang(1/1) ab F6 | Abstand F1:30880 F300:25957 F600:21739 F900:17288 | Weg 15898
  slot 2 t=10 g0=0d Gang(1/1) ab F6 | Abstand F1:29674 F300:23180 F600:17245 F900:13017 | Weg 21990
  slot 3 t=10 g0=0d Gang(1/1) ab F6 | Abstand F1:28617 F300:23873 F600:19608 F900:15184 | Weg 15795
  slot 4 t=11 g0=0d Gang(1/1) ab F6 | Abstand F1:27537 F300:20359 F600:13834 F900:7491  | Weg 23671
  slot 5 t=11 g0=0d Gang(1/1) ab F6 | Abstand F1:28430 F300:19583 F600:11117 F900:982   | Weg 32961 (Griff 1/3 @F896)
NUTZER (v0.8.24), gleicher Lauf
  slot 1..5 g0=02 Folge 0/0@1 1/0@6 (Stehen) 900 Bilder | Abstand F1 27537..30880 -> F900 27515..29761 | Weg 29..3327 (Wander)
```
=> Der Nutzer-Befund ist mit seiner exe am echten Weg reproduziert (alle fuenf stehen), mit HEAD gehen alle fuenf ab dem
ersten KI-Takt nach der Blende (F6) auf Leon zu, der erste greift in F896. Der einzige Unterschied der beiden Laeufe
bis zum Nachspawn ist die Spawnform (NUTZER g0=02 = RE2-Steher, Wecker nur < 0x1388/0x1D4C mit Sicht; HEAD g0=0d =
re2z_init sel 0x0d -> 0x0101): Montage-Besuche, Bilanzen und Leon-Lagen sind in beiden gleich -> der Fix erklaert den Befund.

### E2 — Nutzer-Beispiel, Spielstand nach der Montage + echte Tuer 1150 -> 1130, Leon GEHT den Gang hinunter
Karte `probe_r36_gegnerzahl_karte re15_card.mcr 1150 -17150 -11960 0 9:71 3:94 9:73-76 4:4-5 4:15 7:0xd3-0xd7 7:0xc6-0xca`
+ CONTINUE, Skript `W1,A0.2,W1,R0.33,U25` (Aktion an der Tuer, in 1130 rechts drehen + vorwaerts), Ende `900#1130`.
```
HEAD    Leon (-5900,16350) -> (-768,-185), HP 100 -> -30 (erstes Bild HP<100: F480)
  alle fuenf Gang(1/1) ab F6; Abstand F1 27537..30880 -> F300 7685..13743 -> F500 898..8348;
  Griffe 1/3 @F459 (slot 5), @F683 (slot 2); Biss/Abstoss 1/9, 1/5
NUTZER  Leon (-5900,16350) -> (-1488,-11846), HP 100 -> 40 (erstes Bild HP<100: F758)
  alle fuenf Stehen 1/0 bis F672..702 (Leon dann 5000..7000 vor ihnen), erst dann Gang
```
Bilder (FRAMEDUMP 150..750/50, accelerated Renderer): `scratchpad/h_abn1/e4_gang_user.png` — NUTZER: in Cut 1 (F700/F750)
steht der ganze Pulk am Gangende vor dem Briefing Room = genau "bleiben einfach nur stumpf alle im Gang stehen";
`e4_gang_head.png` — HEAD: ab F500 kommt der erste Zombie in Cut 3 an Leon heran, ab F600 stehen/greifen sie an der
1120-Tuer (Cut 2), Leon liegt.

### E3 — Montage-Einstellung 1130 (Cut 0, Besuch 2 der Kette), HEAD
Gleiche Kette wie E1, `RE15_FRAMEDUMP=20-170/30:m1130_`, Ende `170#1130` (erst Besuch 2 erreicht Bild 170 in 1130).
state.log Besuch 2 (F1..F181, Leon geparkt (-3050,-2150)): alle fuenf g0=02 Folge `0/0@1 1/0@6`, Weg 0..18.
Bild `scratchpad/h_abn1/e3_montage_bild/montage1130_F20_50_80_110_140_170.png`: die fuenf stehen als Pulk vor der
Briefing-Room-Tuer, 6 Sekunden lang ohne Ortswechsel. -> unveraendert gegenueber v0.8.24 (Absicht der Spur, Hinweis H1).

### E4 — RE1.5-KI-Aussagen des Dossiers (M2b der Abnahme 0) nachgemessen
* ROOM1180 `RE15_AI_FLAVOR=re15`, Sprung `1180@250`, 600 Bilder: state.log HEAD gegen NUTZER **851/851 Zeilen, 0
  abweichend** -> "die neue Spawn-Regel wirkt dort nicht" stimmt.
* Nachspawn 1130 `RE15_AI_FLAVOR=re15` (Karte 1150 + Tuer, Leon steht, 400 Bilder): HEAD alle fuenf `0/0@1 1/2@6`
  (RE1.5-Angriff), Abstand F1 -> F400 30880->23389, 29674->27163, 28617->20007, 27537->21469, 28430->22590 (2511..8610
  naeher); NUTZER alle fuenf `1/0` (Stehen), Weg 0..18 -> "der Nachspawn ... gilt auch dort" stimmt Zahl fuer Zahl.

### E5 — "generell fuer die Zombies": Raeume mit RE1.5 grid 0x00 und Gegenproben, HEAD gegen NUTZER
Datenzensus selbst gefahren (`H_belege/re15_emset_zensus.py sum`/`all`, `re2_emset_zensus.py sum`): RE1.5 419 Zombie-Saetze
(0x00 72, 0x02 91, 0x0d 142, Rest liegend/fressend/kriechend/sitzend), RE2 433 (Deskriptor 0 214, 6 140) — wie im Dossier.
Je Raum (Leon-Varianten): 0x00 in 1010(2) 1020(4) 1070(5) 1100(5) 1180(3) 11E0(4) 11F0(4) 1250(7) 4050(2 Girls);
0x02 in 1090 10B0 10D0 10E0 1140 1220 2020 30D0 30E0 40B0 5120 5140.
Sprung = `RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=<raum>@250` (Neues Spiel), Leon steht, 600 Bilder.

| Raum / Weg | Zombies | HEAD | NUTZER (v0.8.24) |
|---|---|---|---|
| 1250 Sprung | 7x 0x00 | alle Gang ab F6, Abstand 18180..25719 -> F600 8767..12520 | alle Stehen 1/0, Weg 0 (einer Wander 6894) |
| 1180 Sprung | 3x 0x00 | alle Gang ab F6; naher greift F339; ferner #2 34102 -> 22953 | ferne zwei Stehen, naher wacht F197 |
| 1181 Elza-Karte (`probe_r36_elza_karte 1181 -4729 -16018 0`) + CONTINUE | 3x 0x00 | alle Gang ab F0; naher greift F333; ferner #2 34003 -> 22846 | ferne zwei Stehen, naher wacht F191 |
| 11E0 Sprung, Leon an der 1200-Tuer (`RE15_PLAYER_POS=2062,-24428,0`), 900 Bilder | 4x 0x00 | alle Gang ab F6, F900 1285..12056 (vorher 13910..16714) | alle Stehen/Wander, keiner naeher |
| 11E0 Sprung, Leon an der 11F0-Tuer (`RE15_PLAYER_POS=-24707,-9442,0`), 900 Bilder | 4x 0x00 | alle Gang ab F6, Weg 7363..11078, aber Abstand nur 728..1555 naeher: sie laufen bis z=-13018 und bleiben dort an einer Kante stehen (Hinweis H3) | Stehen/Wander |
| 1010 Sprung | 2x 0x00 (+2x 0x81) | Gang ab F6 | Stehen F6, Gang ab F7 (Leon 3400 nah -> sofort wach); ab F7 identische Lagen |
| 1090 / 1220 / 10E0 Sprung | (im Neuspiel keine 0x02-Steher; 1220 2x 0x81, 10E0 Fresser 0x06 + Liegender 0x88) | state.log HEAD gegen NUTZER je **851/851 Zeilen, 0 abweichend** | |
| 1040 Sprung | 5x 0x0d | state.log HEAD gegen NUTZER **851/851, 0 abweichend** (0x0d war schon Gang) | |
| 1130 nach der Montage (E1/E2) | 5x 0x0d (Nachspawn) | Gang ab F6 | Stehen |

=> In jedem gemessenen Raum mit der RE1.5-Aktivform gehen die Zombies jetzt ab dem Spawn (RE2-Deskriptor 6) — bei Leon UND
Elza, ueber Sprung, Karte und echte Tuer; die anderen Formen laufen Zeile fuer Zeile wie v0.8.24.

### E6 — ROOM4050 Zombie-Girl (Typ 0x13, grid 0x00) an der exe (in Abnahme 0 nicht gemessen, H5)
ROOM4050 main00 waehlt die Gegner ueber `Switch 13 0a` @0x01E56 = work_vars[0x0A] = der angezeigte Cut
(aot_common.c:1615-1628). Girl-Faelle: Case 9 @0x01EAE -> `44 00 13 00 ...` @0x01EB4 (-9900,1150), Case 0xE @0x01F56 ->
@0x01F5C. Cut 9 entsteht ueber die Tuer Slot 6 @0x01C16 (`3b 06 ... 7a db 00 00 d8 f5 00 0c 03 05 09`: Ziel (-9350,-2600)
Gierung 3072, Stage 3 / Raum 5 / Cut 9). Weg: Karte `probe_r36_gegnerzahl_karte re15_card.mcr 4050 -9300 -23800 3072` +
CONTINUE, Skript `W1,A0.2` (Aktion vor der Tuer Slot 6), Ende `600#4050`. (Gegenversuche ohne echte Tuer — Sprung mit
`RE15_FORCE_CUT=9`, Karte direkt auf dem Zielpunkt — spawnen KEIN Girl: der Switch laeuft dann mit Cut 0.)
```
HEAD    slot 1 t=13 g0=00 @(-9900,1150) ab F90: Folge 1/1@90 1/3@250 (Griff) 1/5@353 1/1@512 | Abstand F100:3735 F300:761
NUTZER  slot 1 t=13 g0=00 @(-9900,1150) ab F90: Folge 1/0@90 1/1@96 1/3@250 ...           | ab F96 identisch
```
=> Die Abbildung greift auch fuer das Girl (HEAD 1/1 im Spawn-Bild statt 1/0); weil Leon dort nur 3735 entfernt eintritt,
weckt der RE2-Steher den NUTZER-Stand 6 Bilder spaeter ohnehin (DECISION[0] < 0x1388) — praktisch gleich (wie 1010).

### E7 — weiterer Weg nach der Montage: echte Tuer 1040 -> 1030, Leon steht (450 Bilder)
Karte `probe_r36_gegnerzahl_karte re15_card.mcr 1040 -20083 -3559 0` + Montage-Bits wie E2, Skript `W1,A0.2`.
debug.log (beide): `Nachspawn 1040 (Programm 14)` -> `loaded room1030` -> `Nachspawn 1030 (Programm 11-13)`.
```
HEAD    14 Zombies: 6 Raum-Records + 3 Kriecher (0x0d) Gang ab F6; 5 Kopien aus 1070 (g0=00) Gang ab F6,
        Griffe/Bisse ab F86; Leon HP 100 -> 0 (erstes Bild HP<100: F107)
NUTZER  0x0d gleich (Gang ab F6); die 5 Kopien g0=00 Stehen ab F6, wach erst F7/F7/F177/F200/F334 (je nach Abstand
        3817..7623); Leon HP 100 -> 20
```
=> 1030 war schon bedrohlich; die 1070-Kopien gehen jetzt ebenfalls ab dem Betreten los.

## 2. RE-Gate — Stichproben selbst disassembliert (re15_disasm.py / re2_disasm.py, richtige Binaerdatei)

### 2.1 M1 der Abnahme 0 (Zombie-Girl-Kette jetzt STAGE4.BIN) — jede Zeile des neuen Kopf-Absatzes nachgelesen
| Zitat (Kopf `re15_zombieaktiv_r37.h`) | selbst gelesen | Urteil |
|---|---|---|
| Raumlader @0x800397a4-ec, Stage 3 / Raum 5 -> @0x8007438c[3] -> Datei 912 | PSX.EXE `800397a8: lh v0,4064(v0)` (0x800b0fe0), `800397b0: lh v1,4066(v1)`, `800397cc: addiu at,at,17292` (0x8007438c), `800397e0: lhu v0,0(v1)`, `800397e4: srl a0,a0,31`, `800397e8: jal 0x80013b60`; @0x8007438c[3] = 0x8007432c, dort u16[5] = **912** | stimmt |
| Datei 912 = ROOM4050.RDT (221128 Byte) | Dateitabelle 8 Byte je Eintrag ab 0x8006f43c, Eintrag 912 @0x800710bc = 0x35fc8 = 221128 = Groesse `info/Re1.5/PSX/STAGE4/ROOM4050.RDT` (913 ebenso = ROOM4051) | stimmt |
| Overlay-Wechsel @0x80039a30 -> @0x800299a4(2, 3+1) | `80039a38: lh a1,4064(a1)`, `80039a3c: ori a0,zero,0x2`, `80039a44: jal 0x800299a4`, Delay `80039a48: addiu a1,a1,1` | stimmt |
| Ladesatz @0x80073c0c = Datei 3, 0x1A620 Byte = STAGE4.BIN (@0x8006f454), Installation 0x80118380 | 0x800299a4: s0 = 12*a1 (`sll/addu/sll` @0x800299b4-bc), Datei `lw a0` @0x800299e0 aus 0x80073bdc+s0, `jal 0x80013b60` @0x800299e8, Installation `jal 0x80029a98` @0x80029a04 mit a1 aus 0x80073be0+s0. Satz 4 @0x80073c0c = {3, 0x80118380, 0x80011370 -> 0x80100000}; Dateitabelle Eintrag 3 @0x8006f454 = 0x1a620 = 108064 = Groesse STAGE4.BIN (Eintraege 0..2 = 137648/103216/129036 = STAGE1..3.BIN) | stimmt |
| STAGE4.BIN 0x80072bac[0x13] = 0x8010a87c (@0x80118390 / @0x80118478) | `8011838c: lui v1,0x8011`, `80118390: addiu v1,v1,-22404` (0x8010a87c), v1 bis zum Store unveraendert, `80118478: sw v1,11256(at)` = 0x80072bf8 = 0x80072bac + 4*0x13; Typ 0x10/11/12/16/18/1c-1f -> 0x801003d8 (`80118388`, Stores @0x80118398-d8) | stimmt |
| Zustand 1 = 0x8010b228 (@0x80119734), Modus ueber @0x80119758, Modus 0 = 0x8010b688, DECIDE @0x8011978c[+0x5], [0] = 0x80101b18 | `8010a9c0: lbu v0,4(v0)`, `8010a9d0: addiu at,at,-26832` (0x80119730), `8010a9e0: jalr v0`; @0x80119730 = [0x8010aae0, **0x8010b228**, ...]; `8010b648: lbu v0,9(v0)`, `8010b650: andi v0,v0,0xf`, `8010b65c: addiu at,at,-26792` (0x80119758), `8010b66c: jalr v0`; @0x80119758[0] = **0x8010b688**; `8010b6cc: lbu v0,5(v0)`, `8010b6dc: addiu at,at,-26740` (0x8011978c), `8010b6ec: jalr v0`; @0x8011978c[0] = **0x80101b18** | stimmt |
| 0x80101b18 = STAGE1 FUN_80101b64 Wort fuer Wort (70/70 bis `jr ra` @0x80101c28) | eigener Byte-Vergleich STAGE4 @0x80101b18 gegen STAGE1 @0x80101b64: 80 von 80 Worten gleich, `jr ra` in Wort 68 (@0x80101c74 bzw. @0x80101c28) + Delay = 70 Worte | stimmt |
| Stage-4-Zombies (0x801003d8): @0x80118d34[0] = 0x80101640 -> DECIDE @0x80118d68 (`addiu at,at,-29336` @0x80101660) | `80101600: lbu v0,9(v0)`, `80101608: andi 0xf`, `80101614: addiu at,at,-29388` (0x80118d34); @0x80118d34[0] = 0x80101640; `80101650: lbu v0,5(v0)`, `80101660: addiu at,at,-29336` (0x80118d68); @0x80118d68[0] = 0x80101b18 | stimmt |
| Girls ROOM4050/4051 main00 @0x01EB4 / @0x01F5C grid 0x00 | RDT-Bytes `44 00 13 00 00 01 00 a0 54 d9 ...` bzw. `44 00 13 00 00 01 00 7d 40 06 ...` (shared_assets == info) | stimmt |

### 2.2 Kern der Abbildung (unabhaengig von Abnahme 0 nachgelesen)
* **RE2 Deskriptor 6 -> 0x0101** (EMZ0.BIN): `80100aa8: lhu v0,270(s2)` (+0x10E), `80100aac: addiu v1,zero,6`, `80100ab0: andi
  v0,v0,0x3f`, `80100ab4: bne v0,v1,0x80100ac0`, Delay `80100ab8: addiu v0,zero,257`, `80100abc: sw v0,4(s2)`; Grundwort
  `801006ac: addiu v0,zero,1` ... `801006b8: sw v0,4(s2)` -> RE15_R37_RE2_WORT_GANG/STEHEN stimmen.
* **RE1.5 grid 0x00 = FUN_80101b64** (STAGE1.BIN): Sce_em_set-Handler PSX.EXE `800420f4: addiu s2,a1,2`, `8004215c: lbu
  v0,1(s2)` / `80042164: sb v0,9(s0)` -> pc[3] nach +0x9; Modus-Dispatch `8010164c: lbu v0,9(v0)` / `80101654: andi 0xf` /
  `80101660: addiu at,at,-2036` (0x8011f80c); @0x8011f80c = [**0x8010168c**, 0x80101708, 0x80101784]; 0x8010168c: `lbu
  v0,5(v0)` @0x8010169c, `addiu at,at,-1984` @0x801016ac (0x8011f840), `jalr` @0x801016bc; @0x8011f840[0] = **0x80101b64**.
  Ganze Funktion gelesen (@0x80101b64-c78): arc_test 0x2c8 / 0x5f4 (@0x80101b7c/8c), Zaehler +0x9C -> 0x101 (@0x80101ba0-c4),
  0x701 bei `sltiu 0x7d0` + Kegel 0x2c8 (@0x80101bd4-f0), `80101bf4: sll v0,a0,16 / bne -> Ende`, Sicht `80101c0c: lhu
  v0,472(v1) / andi 0x10 / beq -> Ende`, `80101c1c: ori v0,zero,0x201` / `80101c20: sw v0,4(v1)`, dann `80101c38: sltiu
  0x2711` / `jal rng` @0x80101c44 / `andi 0x7` / `ori 0x801` / `sw` @0x80101c64. Kein Abstandstest vor dem ENGAGE — wie
  im Kopf beschrieben (der Kopf-Satz zum 1/8-Wurf ist seit NB1 korrekt). Sichtsensor FUN_8001b84c: atan2 + Kegel
  (@0x8001b87c-8b0), dann Strahl `jal 0x8003dcc4` @0x8001b8e8 mit dem vollen Vektor zum Spieler (dx,0,dz) — in
  FUN_8001b84c selbst kein Abstandsvergleich (0x8003dcc4 nicht weiter verfolgt).
* Code `re15_r37_em_set`: `behavior = t->pc[3]` (scd_vm.c op_sce_em_set), Marke nur bei `behavior == 0x00`, Zombie-Familie
  (`re15_re2z_owns_type`: 0x10/11/12/13/16/18) und ausserhalb eines Montage-Schritts.

### 2.3 Gate-Suche im Code-Diff
`git diff master...HEAD -- re15_port | grep -i "deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|getenv|approx"`
-> kein Treffer. Kein `getenv` in den neuen Dateien; `re15_r37_zombieaktiv_enable(0)` nur aus Riegeln/Sonden
(test_r37_zombieaktiv.c, test_r36_zombieki_sicht.c). Jede Konstante traegt ihr Zitat: `RE15_R37_RE2_WORT_GANG 0x0101u`
@0x80100ab8-abc, `RE15_R37_RE2_WORT_STEHEN 0x0001u` @0x801006ac-b8, `RE15_R37_RE15_AKTIV 0x00u` @0x8011f80c[0] /
@0x8011f840[0], `EM_1130_NACH` grid 0x0d NUTZER-VORGABE + @0x80100f40-44 + Form ROOM1040 @0x011FC / ROOM1030 @0x01DE6.
Riegel-Schwellen (Bild <= 2, >= 15 je Bild) aus der RE2-Originalmessung `H_belege/re2_original_room20a0_takte.txt`:
nachgerechnet — alle sechs Zombies Takt 0/1 `w4=00000101`, ab **Takt 2 `00010101`**; Annaeherung Takt 1 -> 100 1572..3268
(kleinste 1572 = 15,9 je Takt) -> Schwelle 15 haelt.
**RE-Gate: haelt.**

## 3. Vertrag
* Pfade (`git diff master...HEAD --name-only`): `analysis/befunde_runde37/H_*` (eigene), NEU `engine/src/zombieaktiv_r37.c` +
  `include/re15_zombieaktiv_r37.h` (Namensregel §1.4), Haken `scd_vm.c` (+2: Include + 1 Zeile am Ende des Spawn-Zweigs von
  op_sce_em_set, Kommentar "Runde 37 Spur H"), `enemy_ai_re2_zombie.c` (+4: Include, 1 Zeile in re15_re2z_rng_reset, 2 Zeilen
  re2z_init), `irons_tod_1150.c` (H-Datei laut §1.4: Liste `EM_1130_NACH` + 2-Zeilen-Weiche, Rest Kommentar). Tests nur
  `tests/unit/probes/r37_zombieaktiv.cmake`, `tests/unit/test_r37_zombieaktiv.c`, `tests/integration/test_r37_zombieaktiv.cmake`
  (`cmake_minimum_required(VERSION 3.16)` Z. 1), dazu `tests/unit/test_r36_zombieki_sicht.c` (+6, begruendet: der Runde-36-
  Sichtzensus misst STEHENDE Zombies und laeuft deshalb mit der v0.8.24-Abbildung). Kein release/, platform/android/,
  shared_assets/, keine Test-CMakeLists. Test-Quelle ohne POSIX-Aufrufe (nur stdio/stdlib), `m` nur unter `NOT WIN32`.
* Keine Bank-9-Bits, keine Text-Ids, keine Ereignisse/AOT-Slots (Tot-Bits 40-43/46 des Nachspawns = die der alten Liste).
* Seit Abnahme 0 nur Kommentare/Schreibweise (`git diff 9c2a825d HEAD -- re15_port`: re15_zombieaktiv_r37.h Kopf,
  irons_tod_1150.c Kommentar + `EM_1130_NACH` mit Zeilenfortsetzungen, Inhalt Byte fuer Byte gleich). Meine E1/E2-Zahlen
  decken sich mit N1/N2 der Abnahme 0 bis auf Rundung (z.B. slot 5 F900 982 gegen 981) -> Verhalten unveraendert.
**Vertrag: haelt.**

## 4. Tests (gezielt, KEINE volle Suite im Spurbaum)
* Spur-Riegel `ctest -R "r37_zombieaktiv" -V`: **4/4** — abbildung 6/6 (14 RE1.5-Formen: nur 0x00 1/0 -> 1/1, Gang ab Bild 1;
  ohne Sce_em_set und mit liegengebliebener Marke Steher; Montage-Schritt Steher), nachspawn1130 4/4 (kleinste Annaeherung
  4873; Gegenprobe 5/5 nie im Gang), raum1180 5/5, integration_r37_zombieaktiv (Bild 7 5/5 im Gang, 4892..8948 naeher).
* Unit/Sonden aller beruehrten Module, Regex `re2|zombie|r35_cut1150|r36_szene1150|r36_gegnerzahl|r36_zombieki|irons|
  montage|1010|1020|1070|1100|1130|1030|1040|1180|11e0|11f0|1250|4050|enemy|ai_|scd|spawn|kriecher|girl|em_set|emset|sce|
  gore|grab|griff|biss|lane`, `-E "^integration_" -j 6` -> **100% tests passed, 0 tests failed out of 181** (29 s).
* Integration (echte exe), Regex `^integration_(r35_entladen|r34n_e_dokumente|r35_cut10f0|r35_zgirl|r36_elza|r36_inventar|
  r36_pri_schatten|r36_story|r35_cut1150|r36_szene1150|r36_gegnerzahl|r30_cut_blitz|r37_zombieaktiv|pri_masken|
  r30_irons_tisch|r34n_g_schrift1150|r36_dachkiste|r36_szenenleon|r36_schluessel|room_probe_sweep|cross_stage|headless_run|
  room_transition|door_traversal)` `-j 3` -> **100% tests passed, 0 tests failed out of 44** (1882 s, parallel zu anderen
  Agenten; kein Wiederholungslauf noetig). Darin die Montage-Kette integration_r35_cut1150, integration_r36_szene1150,
  Story Leon/Elza, alle Entladen-Laeufe, Zombie-Girl 4050/5090, Szenen Leon.

## 5. Maengel

Keine.

## 6. Hinweise (keine Maengel dieser Spur; fuer Orchestrator/Nutzer)

* **H1 — Montage-Einstellung 1130 zeigt die fuenf weiter stehend (E3, Bild `montage1130_F20_50_80_110_140_170.png`).**
  Absicht der Spur und im Abschnitt "Fuer den Nutzer" gesagt ("Die Montage selbst ist unveraendert"). Der Wortlaut
  "bleiben ... stehen. So sind sie keine Bedrohung" zielt auf das Spiel danach (E1/E2 erfuellt); wer die Einstellung
  selbst meint, sieht dort weiter einen stehenden Pulk. Zahlen fuer eine Entscheidung: geparkter Leon (-3050,-2150)
  8744..12162 von den fuenf entfernt, Einstellung 181 Bilder, gehend >= 257 Bilder bis zum Parkplatz.
* **H2 — zwei vorbestehende RE2-Abweichungen im Steh-Zweig (Dossier OFFEN), selbst bestaetigt:** DECISION[0] Block 3 —
  Original `801013c4: andi v0,v0,0x1` / `801013c8: beq v0,zero,0x801013d4` = Commit 0xC01 bei (rand&1)==1, Port
  `enemy_ai_re2_zombie.c` Z. 1373 `(re2z_rand() & 1u) == 0u` (Block 2 dagegen richtig: `8010136c: bne v0,zero` = Commit bei
  0, Port Z. 1370 `== 0u`). EXEC[0] P1 — Original laeuft nach dem Wander-Wurf (`80101540: sb v0,6(s0)`) ohne Ruecksprung in
  den Selbstwecker (`80101544: lh v0,344(s0)`), Port sperrt ihn im selben Takt (`if (e->sub_state_2 == 1)` Z. 1446).
  Beide aendern die Wahrscheinlichkeit/den Takt kaum (keine Passivitaets-Ursache), verschieben beim Beheben aber den
  gemeinsamen RE2-Zufallsstrom -> eigener Schritt mit Strom-Messung (NACHLAUF).
* **H3 — 11E0 von der 11F0-Tuer aus (E5, auch ueber die echte Tuer per `RE15_FIRE_AOT=0@90#11F0` gemessen, identische
  Zahlen):** die vier jetzt gehenden Zombies laufen bis z = -13018 und haengen dort an einer Kante (Weg 7363..11078, Abstand
  nur 728..1555 naeher, Zustand 1/1/1 bleibt). Im Eintritts-Cut 12 nicht im Bild; von der 1200-Tuer aus erreichen sie
  Leon (F900 1285). Ursache ist die vorbestehende Steuerung des Gangs (RE1.5-Navigator FUN_80039e7c = re15_nav_update_steer;
  der RE2-Navigator FUN_8004A808 ist laut `enemy_ai_re2_zombie.c` Z. 288-291 "DEFERRED ... not ported"), nicht der Code
  dieser Spur — durch die Abbildung laufen aber mehr Zombies ueber diesen Weg. Vorschlag NACHLAUF: Navigator gegen
  RE2 FUN_8004A808 pruefen.
* **H4 — Port aggressiver als RE2** (Zensus selbst gefahren: 214/419 = 51 % bzw. 214/305 = 70 % der aufrechten gegen RE2
  140/433 = 32 % bzw. 140/354 = 40 %); dem Nutzer im Dossier so gesagt. Spuerbar z.B. in 1030 nach der Montage (E7:
  stehender Leon tot nach 450 Bildern, v0.8.24 HP 20).
* **H5 — ROOM1250** (7x grid 0x00, im Dossier unter "Betroffen") hat nur Tuer-AOTs mit sce 0 ohne Ziel und taucht in
  `tools/engine_tueren.txt` nicht als Ziel auf — nur ueber das Debug-Menue erreichbar; fuer das normale Spiel ohne Belang.
* **H6 — Kommentar-Altlast (vom Bau-Agenten gemeldet, nicht im Diff):** der Girl-Block in `enemy_ai_common.c` (~Z.
  10650-10690) nennt Adressen der STAGE1.BIN-Kopie ohne Binaerdatei; fuer ROOM4050 gilt STAGE4.BIN (Modus 0: 42/44 Worte
  gleich, Abweichung genau die Tabellenbasen 0x80120264/0x801202a8 gegen 0x8011978c/0x801197d0 — selbst verglichen).
* Release (Bau-Agent, unveraendert): neue Quelldatei `re15_port/engine/src/zombieaktiv_r37.c` (Android: GLOB-Cache neu
  konfigurieren), +4 Tests (3 unit, 1 integration; RE15_MIN_TESTS anheben).
