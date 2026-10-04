# Runde 35 Spur H "raeume" — Abnahme 0 (unabhaengig, 2026-10-04)

Baum `.claude/worktrees/r35_raeume`, Zweig `r35/raeume`, HEAD 96d492dd (Code-Stand = ae4162c8;
`git diff ae4162c8 HEAD -- re15_port` ist leer, 96d492dd ist ein leerer Abschluss-Commit).
Basis 154a73c1 (merge-base mit master). Gemessen wurde an der gebauten exe
`re15_port/build/platform/pc/re15_pc.exe` (Kopie `re15_pc_abn0h.exe`, `local_build.sh configure` +
`build` = "ninja: no work to do", LOCAL-BUILD-OK (build)). Bilder per `RE15_FRAMEDUMP` (320x240),
Logs per `RE15_GEGNER_Y_LOG` / `RE15_HUNDESCHATTEN_LOG` / `RE15_STATE_LOG` / `RE15_RE2_TRACE`.
Alle Laeufe mit `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2`.
Arbeitsordner der Laeufe: Scratchpad `runs/<name>/` (debug.log, gy.log, st.log, fd_*.ppm).

## Ergebnis

| Punkt | Urteil |
|---|---|
| 1 ROOM1190 Hunde-Schatten im Luken-Sprung | **erfuellt** |
| 2 ROOM1190 Zielscheiben-Texte | **erfuellt** |
| 3 ROOM1200 Bahren-Zombie kommt herunter | **teilweise** |
| 4 ROOM1210 Gitterarme beim Griff (Clipping) | **teilweise** |

**bestanden = false** (Punkte 3 und 4 nicht voll erfuellt; Maengel M1-M4 unten).

---

## Punkt 1 — Hunde-Schatten waehrend des Luken-Sprungs (erfuellt)

**Echter Weg:** Tuer ROOM1180 -> ROOM1190 per Aktionstaste (Tuer-AOT slot 2 ROOM1180, rect
(-9266,8888,2297,934), Ziel (3174,0,-11182)), danach `RE15_SUBSTART=10@60#1190` = genau das sub10,
das sub01 @0x02468 (`Evt_exec 04 ff 18 0a`) nach der Raetselloesung startet. Die Kamera Cut 11 setzt
sub10 selbst (`Cut_chg 29 0b` @0x027E0), nicht die Spielerlage.

```
run.sh p1_re2  RE15_DEBUG_JUMP=1180@gp RE15_PLAYER_POS=-8118,9355,1024 RE15_PAD_AT=45:A,46:A
               RE15_SUBSTART=10@60#1190 RE15_HUNDESCHATTEN_LOG=hs.log RE15_GEGNER_Y_LOG=gy.log
               RE15_STATE_LOG=st.log RE15_FRAMEDUMP=100-420/2:fd_ RE15_EXIT_AT=430#1190
run.sh p1_re15 (dasselbe + RE15_AI_FLAVOR=re15)
```
* gy.log: `45 R1180` -> `430 R1190` (Tuer genommen), debug.log `[substart] sub_scd[10] at F60`.
* Kamera (st.log, ROOM1190): F1 cam 0, F101 cam 8, **F181 cam 11**, F279 cam 2.
* Hund 1 st.log: F181 `st=4 ss2=1 mo=20` (Absprung) / F205 `ss2=3 mo=21` (Flug) / F221 `st=1` gelandet;
  Hund 2 F213-253, Hund 3 F245-285.
* hs.log RE2-KI: 505 gezeichnete Hunde-Schatten, davon **117 mit Koerper ueber Boden ("luft=1"), alle
  117 `boden_y=0 schatten_y=0`** (Koerper bis `koerper_y=-3680` @F196). RE1.5-KI: 107 Luft-Bilder,
  alle `boden_y=0 schatten_y=0`.
* Bilder (Kontaktbogen, Scratchpad `p1_re2_jump1.png`, `p1_re2_jump23.png`, `p1_re15_jump.png`):
  F184-F284 (RE2) und F196-F272 (RE1.5), alle drei Hunde im Sprung — **kein Schatten-Quad an der Wand
  oder in der Luft**. Gegenbild Dossier `H_raeume/p1_vorher_F200_F204.png` (Basisstand): dunkler Quad
  unter dem Hund an der Wand. Der Befund des Nutzers ist damit am gebauten Stand weg.

**Erklaert der Fix den Befund?** Ja: vorher `nsh_y = npc->y` (Koerper, main.c NPC-Schatten); nachher
Quad-Y = +0x1ba, das die Sprungmaschine ab Bild 0xD auf 0 setzt (`sh zero,442(v0)` @0x801114f0, selbst
disassembliert). Der Schatten liegt waehrend des Sprungs am Raumboden, der unter Cut 11 ausserhalb des
Bilds liegt.

**RE-Beleg selbst disassembliert (STAGE1.BIN):**
```
8010d91c: lh   a1,442(v0)      ; +0x1ba
8010d920: jal  0x8001b064
8010d924: addiu a0,a0,176      ; +0xb0
801114a8: sltiu v0,v0,0xd ... 801114dc: addiu v0,v0,-20 / 801114e0: sw v0,56(v1) / 801114f0: sh zero,442(v0)
```
RE2-Gegenprobe (info/re2leon/PSX.EXE): `800268c8: lh a1,450(s0)` (+0x1C2) -> `800268d8: jal 0x800168b4` —
stimmt mit dem Dossier ueberein.

Hinweis (kein Mangel): Der Schatten wird nicht "entfernt", sondern liegt am Boden (beide Originale tun
das so). Unter Cut 11 ist das im Bild dasselbe wie "raus"; in keinem der 24 geprueften Sprungbilder ist
er sichtbar.

---

## Punkt 2 — Zielscheiben-Texte (erfuellt)

Raumsprung `RE15_DEBUG_JUMP=1190@gp` mit Spieler 620 vor dem jeweiligen Schalter (x=-3180, Blick -x),
Strom an `RE15_SET_FLAG=4:243`, Tasten erst ab Bild 1800 (`RE15_PRESS=square@1800..1803, @1960..1963,
@2120..2123`, liegt hinter allen Boot-Raum-Bildern), Bilder `RE15_FRAMEDUMP=1790-2500/10`.

| Lauf | Platz | debug.log | Bild (Seite 1 / Seite 2) |
|---|---|---|---|
| p2c_s0 (Cut 5 erzwungen) | Slot 0 (z -24500) | `Slot 0 -> Port-Nachricht 6 (viele Einschuesse) + Original msg 0` | "This target has a surprisingly / large number of bullet holes." / "There's a switch here. Push it? Yes No" |
| p2c_s1 (Cut 5) | Slot 1 (z -20800) | `Slot 1 -> Port-Nachricht 7 (wenige ...)` | "This target does not have / many bullet holes." / Frage |
| p2c_s2 (Cut 5) | Slot 2 (z -17300) | `Slot 2 -> Port-Nachricht 6 (viele ...)` | "surprisingly large..." / Frage |
| p2_s3dog (`4:243,4:234`) | Slot 3 (z -13700) | `Slot 3 -> Port-Nachricht 7 ... + Original msg 2` | "does not have many..." / "I have nothing else to do here." (Cut 6, Leon an Kabine "4") |
| p2_s1off (Strom aus) | Slot 1 | `Slot 1 -> Port-Nachricht 7 ...` | Satz / Frage / nach Ja "No response..." (msg 3) |

**Welche Scheibe "ganz links" ist — selbst am Bild belegt (nicht nur Projektion):** In den Laeufen mit
erzwungenem Cut 5 (Blick den Stand hinunter, alle vier Scheiben im Bild) wurde nach dem Satz mit "Ja"
bestaetigt. Danach faehrt in p2c_s0 die **linke** Scheibe nach vorn (F2160 am linken Bildrand, F2200 aus
dem Bild), in p2c_s1 die **zweite** von links, in p2c_s2 die **dritte** von links (F2160/F2200).
Scratchpad `p2c_s0.png`, `p2c_s1.png`, `p2c_s2.png`. Damit: Slot 0 = ganz links = "surprisingly large",
Slot 2 = 3. von links = "surprisingly large", Slot 1/3 = "does not have many" — Wortlaut erfuellt, und die
Original-Mechanik (Ja -> Scheibe faehrt; Strom aus -> msg 3; nach Hunden -> msg 2) bleibt intakt.

Hinweise (kein Mangel): (a) Satzende "." beim zweiten Satz ist eine gekennzeichnete PORT-WAHL (Nutzer
schrieb ihn ohne Punkt). (b) In allen `RE15_DEBUG_JUMP`-Laeufen mit Strom an blieb die Kamera auf Cut 0
(Leon nicht im Bild, st.log `cam=0` durchgehend) — auch im Dossier-Bild `p2_exe_slot0_seite1_seite2.png`;
das ist ein Harness-Effekt des Sprung-Spawns (cut 0 @0x8001d818-20), nicht dieser Spur zuzurechnen.

---

## Punkt 3 — ROOM1200 Bahren-Zombie (teilweise)

**Echter Weg (Teil):** Tuer ROOM11E0 -> ROOM1200 (Tuer-AOT slot 1, rect (1000,-25000,2000,1500), Spieler
(2000,-24250) Blick 3072, Aktion F45): gy.log `45 R11E0` -> `100 R1200`; F2 `[2 t=10 ... g=87 @(-24249,
-1800,-18579) b1 f1ba=-1800 f1c0=0000]` — der +0x1ba-Seed (Vorbedingung des Fixes) steht auch auf dem
Tuerweg. Das Aufnehmen selbst lief per Raumsprung mit Spieler vor dem Minidisc-Platz (wie im Dossier;
der Weg von der Tuer um die Tische ist per Skript nicht reproduzierbar gefahren worden):
```
COMMON = RE15_DEBUG_JUMP=1200@gp RE15_PLAYER_POS=-25880,-16450,2048 RE15_GEGNER_Y_LOG=gy.log
RE15_PRESS=square@1800..1803,@1900..1903,@2000..2003   (Aktion -> Modal "Will you take the Minidisc Player" -> Ja)
```

**RE1.5-KI (`RE15_AI_FLAVOR=re15`)** — erfuellt: Slot 2 (= id 1) F1909 grid 0x89, F2007 steht auf,
laeuft 75 Bilder auf Band 1, **F2083 `@(-24134,-1810,-16900) b0 f1ba=0 f1c0=8001`** (Zelle 13), Folge
-1810, -1800, -1770, -1720, -1650, -1560, -1450, -1320, -1170, -1000, -810, -600, -370, -120,
**F2097 `y=0 b0 f1c0=000f`** (= -10+20t), danach Angriff auf Leon. Bild `p3b_re15_c2.png` (Cut 2):
F2085 oben links, F2090/2095 Sturz, landet neben Leon.

**RE2-KI (Standard) mit Leon weit weg** (`RE15_PLAYER_POS=-20154,-25245`, sub03 per `RE15_SUBSTART=3`):
F1985 laeuft los, **F2059 Kante (Zelle 12) `@(-22833,-1810,-19536) b0`**, F2073 `y=0`, geht Leon auf der
Spieler-Ebene an — erfuellt.

**RE2-KI (Standard) mit Leon am Minidisc-Platz = der Fall des Nutzers** — NICHT verlaesslich:
Slot 2 laeuft F1988-2062 auf Band 1 und bleibt dann bei **`@(-24049,-1800,-17373) b1`** stehen (22 vor
der Zellengrenze z -17351 von Zelle 13), Sub 14 (Schnappbiss) F2062, danach Leerlauf `st=1/0/1 mo=0`.

| Lauf | Leon nach dem Aufnehmen | Slot 2 auf Spieler-Ebene |
|---|---|---|
| p3_re2 / p3_re2_c2 | bleibt stehen, wird von Slot 4 gepackt | erst **F2372**, und nur weil der Partner-Domino `0x901` setzt (F2367 `st=1/9/0`), Sub 9 schiebt ihn in 5 Bildern 1500 Einheiten in Zelle 12 |
| p3c_re2_LU / p3c_re2_RU (`L0.7,U4` / `R0.7,U4`) | geht nach Osten | F2531 / F2530 (~470 Bilder Stehen am Rand) |
| **p3c_re2_D** (`D3`) | geht rueckwaerts nach Osten, bleibt in ~1600 Abstand | **nie** — F2999 (Laufende) weiter `@(-24049,-1800,-17380) b1`, 1011 Bilder ≈ 33 s nach dem Aufstehen |

Der Stillstand kommt aus dem RE2-Entscheidungszweig `floor != pl.floor && dist < 0x7d0` -> 0x0E01
(enemy_ai_re2_zombie.c:3564-3572, @0x80102064-94). Der Bau-Agent kannte den Fall: der Riegel
`unit_r35_raeume_trage` stellt den Spieler bewusst **4500 hinter die Kante** und laesst ihn hin- und
herrennen, "steht der Spieler DIREKT unter der Kante, beisst der RE2-Zombie von oben (Sub 14, 2D-Abstand)
statt weiterzugehen" (test_r35_raeume_trage.c:196-200). Im Dossier steht dazu nur "RE2 ... faellt bei
F~680" (3.4), unter OFFEN nichts. Genau das ist aber die Lage des Nutzers direkt nach dem Aufnehmen.
-> Mangel M1.

**Erklaert der Fix den Befund?** Fuer das "Herumlaufen auf y=-1800 durch den ganzen Raum" ja (Engine-
Schwerkraft fehlte; vorher Dossier: 600+ Bilder auf -1800 bis (-25622,..)/(-22368,..)). Fuer den
Standard-Geschmack in der Nutzerlage nur teilweise (s. o.).

**RE-Beleg selbst disassembliert:** Zombie-Wurzel STAGE1.BIN `801004d8: beq v0,zero,0x80100514` /
`801004dc: addiu a0,zero,-10` (Delay-Slot, beide Wege) / `80100514: jal 0x8001bd60` /
`80100518: ori a1,zero,0x14`. FUN_8001bd60 (PSX.EXE) Zeile fuer Zeile gegen trage_1200.c gelesen:
`8001bd7c andi 0x4000 / bne -> be58`; `8001bd94 lw v1,120(v0)`, `8001bd9c lhu a1,6(v1)`,
`8001bda4 jal 0x8003b7f0` mit `8001bda8 subu a1,zero,a1`; `8001bdb0 andi a1,0x2`; `8001bdd0-e8` = Band*1800
(`sll 3/subu/sll 5/addu/sll 3`), `bne y` ; `8001bdf0 ori 0x8000 / sh 448`; `8001bdf8-be14` ((r&0xc)>>2)<<13;
`8001be24-54` +0x1ba += 1800 (`addiu v0,v0,1800` @0x8001be2c), +0x82 -= 1, `bne v1,zero` (n--);
`8001be6c andi 0x8000`, `8001be7c andi 0x1fff`, `mult`, `8001be84 addiu +1`, y += a0 + a1*t;
`8001beb4-c4` slt +0x1ba < y -> `8001becc sw a0,56`, `8001bee4 andi 0x7fff`. Port = Original.
Radius: `80100770 lw v0,-2160(v0)` liest den Zeiger @0x8011f790 = **0x8011f778**, Halbwort +6 = **400** ✓.
Seed: `800421d4 lbu v1,2(s2)`, `800421f4 sh zero,448(s0)`, `800421f8-8004220c` -(v*1800),
`80042210 sh v0,442(s0)` ✓. Geltung nur fuer die Wurzel-Typen 0x10/0x11/0x12/0x16/0x18
(enemy_ai_common.c:14451 ruft live_step nur fuer diese) ✓.

---

## Punkt 4 — ROOM1210 Gitterarme (teilweise)

**Echter Weg:** Tuer ROOM1220 -> ROOM1210 (Tuer-AOT slot 0, Spieler (-21750,-6400) Blick 0, Aktion F45),
dann `RE15_INPUT_SCRIPT_BASIS=spiel START=80 "R0.5,W0.3,U14,R0.7,U8"`, `RE15_RE2_TRACE=1`, Bilder F236-1000/2.
* st.log: Leon laeuft -z (rot 1440), **F~245 gegriffen bei (-20588,-15148) rot 1667**, gehalten bis ~F415,
  danach weiter. re2_ki.log: `[re2arm] PIN slot 5 yaw 3664: Parts-Pose Clip 3 Bild 4 -> (-20588,-15148);
  Clip 5 Bild 0 (bisher) -> (-20728,-15056); Leon vorher (-20622,-15020) yaw 1440` — der neue Pin greift
  im echten Spiel. Das ist der **Gesicht-Fall** (FUN_80015910: (3664-1440+0x400)&0xFFF = 3296 >= 0x800,
  kein Flip). Bild Scratchpad `p4_front_zoom.png` (F252-F288, 3x): die Hand liegt an Kopf/Schulter,
  keine Hand durch den Oberkoerper sichtbar.
* Ruecken-Fall im Spiel nicht ausgeloest (Rueckweg lief an der Ostwand; ein Lauf nordwaerts wurde von
  Arm slot 2 von vorn gegriffen: `PIN slot 2 yaw 565 ... Leon vorher (-20622,-11200) yaw 3072`, Endyaw
  2561 = Gesicht; Stillstand bei (-20622,-15300) loeste keinen Griff aus).
* Mess-Sonde selbst gefahren (`probe_r35_raeume_arme.exe`), byte-true Phase d=1:
  `Pin=Parts-Pose C3B4 Leon-Yaw 1665 (Gesicht): min 169 ... Bilder <120 0 ... nicht klar richtige Seite 5`
  `Pin=Parts-Pose C3B4 Leon-Yaw 3713 (Ruecken): min 4 mittel 97, Bilder <120 11, <250 19 von 19; ... nicht
  klar richtige Seite 14` — im Ruecken-Griff steckt die Zombiehand weiter in 11 von 19 Bildern < 120
  Einheiten an Leons Brustachse (Minimum 4), bei jedem Phasenversatz d=0..18 (6..16 von 19).
-> Der Nutzerbefund "dadurch clipped er" ist nur fuer den Griff von vorn behoben. Mangel M2. Der
Bau-Agent fuehrt das selbst als OFFEN 1.

**RE-Beleg selbst disassembliert (RE2 PSX.EXE):** `800295e8: lbu v0,333(a0)` -> `800295f8: sw a2,376(a0)`
(+0x178 aus dem Bild VOR dem Zaehlen) -> `800295fc: jal 0x80029614`; gezaehlt wird erst
`80029b28: lbu v0,333(s2) / 80029b30: addiu v0,v0,1 / 80029b34: sb v0,333(s2)` ✓ (Annahme: der Arm ruft
0x8002959C mit a0 == a1 == self — 0x80029614 bekommt `a0 = t0 = a1`; vom Bau-Agenten nicht ausdruecklich
belegt, aendert das Ergebnis nur, wenn a0 != a1). Pin @0x80100C18-38 nur gegen den vorhandenen
Overlay-Dump `analysis/befunde_2026-09-19/arme-1210-re2_em2d_ai1.dis` gelesen (`80100c18: lw v0,92(v1)`,
`80100c20: sw v0,-976(at)` = 0x800cfc30, `80100c24: lw v0,100(v1)`, `80100c38: sw v0,-968(at)` =
0x800cfc38, `80100cac: j 0x80100d6c`) — die Overlay-Datei selbst liegt nicht im Baum
(build/extracted/re2_ems), daher nicht am Binaerbild nachdisassembliert.

---

## Gates

### Suite
* Dossier traegt woertlich `=== LOCAL-BUILD-OK (all) — Tests 482/482` (Schranke 478), gemessen nach
  ae4162c8 = Code-Stand HEAD.
* Selbst gefahren: `ctest -R "r35_raeume|1210_arme_re2"` -> **5/5 gruen** (unit_r35_raeume_hundeschatten,
  _ziel, _trage, _arme, unit_1210_arme_re2). Volle Suite `local_build.sh test`: **482/482** (Nachtrag).

### @0x-Gate
* Jede neue Konstante traegt Adresse/Offset oder ist gekennzeichnet: -10 @0x801004dc, 0x14 @0x80100518,
  1800 @0x8001be2c, 0x8000 @0x8001bdf0, 0x1fff @0x8001be7c, 0x7fff @0x8001bee4, Seed @0x80042210, Typ 0x20
  (sub13 Sce_em_set), Scheiben-Kanten x -4400 / z -25700..-14900 (RDT 0x02226.. Bytes 8..9), msg 0/2/4
  (@0x2E14/@0x2E5E/@0x2EAF), Ids 6/7 (VERTRAG), Puffer 128 (MSG_RAW_LEN), Saetze = NUTZER-VORGABE, "." =
  PORT-WAHL. Ok.
* Stichproben selbst disassembliert (oben): 0x8010d91c, 0x801114a8-f0, 0x801004d8-18, 0x8001bd60-bee8
  komplett, 0x8011f790, 0x800421d4-42210, 0x80042f3c (`sh v0,4048(at)` = 0x800B0FD0, v0 = s2-1 ✓),
  RE2 0x800268c8-d8, RE2 0x8002959C-F8 / 0x80029B30. Alle enthalten das Behauptete.
* Wortsuche im Diff (deferred/tunable/interim/for now/faithful/plausibel/TODO): nur ein Zitat des alten
  Kommentars im Header. Aber: der alte Kommentar steht im Code weiter (M3).

### Pfad-/Vertrags-Gate
* `git diff 154a73c1 --name-only`: nur engine/src (2 gemeinsame Dateien mit kleinen Haken:
  enemy_ai_common.c +5, scd_vm.c +8, main.c +6, re15_actor.h +4 Feld), 3 neue Quelldateien + 3 Header,
  tests/unit/probes/r35_raeume.cmake + 4 neue Tests + 1 Sonde, 1 angepasster Bestandstest
  (test_p2_1210_arme_re2.c 4d, begruendet mit @0x800295E8-F8 — selbst nachgeprueft), analysis/.
  Keine release/, platform/android/, shared_assets/PSX/, tests/unit|integration/CMakeLists.txt. Ok.
* Nachrichten-Ids 6/7 in ROOM1190 (zugeteilt 6..11), keine Bank-9-Bits, Ereignis 27 nicht belegt. Ok.
* (Hinweis: `git diff master` zeigt 184 Dateien, weil master seit 154a73c1 weitergelaufen ist —
  massgeblich ist der Vergleich gegen die merge-base.)

### Tests
* Fuer jeden Punkt ein messender Riegel; fehlende Assets -> FAIL (kein Skip). Aber: P3-Riegel umgeht die
  Nutzerlage (Spieler 4500 hinter der Kante, M1); P4-Riegel prueft den Ruecken-Griff nicht (nur
  protokolliert, M2).

---

## Maengel

**M1 (Punkt 3, Standard-KI RE2): Der Bahren-Zombie bleibt in der Lage des Nutzers oben stehen; Riegel und
Dossier umgehen bzw. verschweigen das.** Leon nimmt den Minidisc-Player bei (-25880,-16450) und bleibt
in der Naehe (Band 0, Abstand < 2000): Slot 2 (Sce_em_set id 1 @RDT 0x0086A) laeuft 75 Bilder auf Band 1
und steht dann bei (-24049,-1800,-17373), 22 Einheiten vor Zelle 13, im Leerlauf `st=1/0/1 mo=0` nach
Sub 14 (RE2-Zweig `floor != pl.floor && dist < 0x7d0` -> 0x0E01, enemy_ai_re2_zombie.c:3564-3572 /
@0x80102064-94). Gemessen: Lauf p3c_re2_D (`D3` nach dem Aufnehmen) — Slot 2 bei F2999 noch
`@(-24049,-1800,-17380) b1`, 1011 Bilder ohne Abstieg; p3c_re2_LU/RU erst nach ~470 Bildern (F2530/2531);
p3_re2 nur durch den Partner-Domino 0x901 (F2367, Sub 9). Unter RE1.5-KI faellt er dagegen 75 Bilder nach
dem Aufstehen (F2083-2097). `unit_r35_raeume_trage` setzt den Spieler absichtlich 4500 hinter die Kante
(test_r35_raeume_trage.c:196-200) und deckt damit genau diesen Fall nicht ab; Dossier 3.4 nennt "faellt
bei F~680" ohne Bedingung, OFFEN fehlt. Noetig: entweder belegen (RE2-Retail-Verhalten an einer Kante bzw.
RE1.5-Original), dass "oben stehen und schnappen" in dieser Lage Original ist, und das dem Nutzer offen
sagen — oder den Abstieg auch in der Nutzerlage herstellen; Riegel um den Fall "Spieler direkt unter der
Kante" erweitern.

**M2 (Punkt 4): Griff von hinten clippt weiter.** `probe_r35_raeume_arme` (selbst gefahren), Phase d=1
(byte-true Leon = Arm+1), Leon-Yaw 3713: Hand < 120 Einheiten an der Brustachse in 11 von 19 Halte-
Bildern, Minimum 4, 14 von 19 "nicht klar richtige Seite"; bei d=0..18 jeweils 6..16 von 19. Ausloeser im
Spiel: Leon blickt beim Griff in Armrichtung (FUN_80015910 -> +2048 @0x80101304-18), z.B. nordwaerts an
Arm slot 5 (z -15747) vorbei. Der Riegel prueft den Fall nicht (nur Protokoll). Bau-Agent: OFFEN 1 mit
naechstem Messweg (RE2 ROOM2050 in pcsx-redux, PL+0x38/+0x40/+0x76) — der Weg ist noch nicht gegangen.

**M3 (Doku, klein): veralteter Kommentar widerspricht dem Code.** enemy_ai_common.c:214
"func_0x8001bd60(-10,20) setup helper — deferred" steht weiter in der Zombie-Wurzel, obwohl der Aufruf
jetzt bei :5701 laeuft; :10695 "unmodeled port-wide" gilt nur noch fuer die anderen Aufrufer (OFFEN 4).
Beide Stellen auf den neuen Stand bringen.

**M4 (Punkt 4, nicht geprueft): RE1.5-KI der Gitterarme** (EM01A + geliehene Opferbank) weder vom
Bau-Agenten (OFFEN 2) noch hier gemessen. Der Nutzer unterscheidet die Geschmaecker nicht; ob der Befund
dort besteht, ist offen.

---

## Nachtrag Suite
Selbst gefahren am HEAD-Stand: `bash re15_port/tools/local_build.sh test` ->
`100% tests passed, 0 tests failed out of 482` (1113 s, unter Last paralleler Spuren; alle fuenf
Fenster-Haken gruen im ersten Lauf) -> **`=== LOCAL-BUILD-OK (test) — Tests 482/482`**. Suite-Gate haelt.
