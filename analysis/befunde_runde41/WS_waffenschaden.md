# Spur WS — Waffenschaden (Rakete + Handgranate), Runde 41, Bauer Runde 1

Stand 2026-11-07. Zweig `r41/waffenschaden`, Worktree `C:/workspace/git/reAi_v2/.claude/worktrees/r41_waffen`,
Basis `r41/integration` 9e0066c26 (Welle 1: S, W, ZA, PF, GR, ST80, G5A).
Spezifikation: `WS_ENTSCHEIDUNG.md` (Prüfer), `WS_LESER_A.md`, `WS_LESER_B.md` (im selben Ordner, Commit 57d416401).
Laufausgaben: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/ws/`.

## 1. Nutzerentscheid

> "mach wie resident evil 2. Auch bei der Granate. mach sie so stark, daß sie gerade so die Standard Gegner mit einer
> Granate töten kann."

Lesung (dem Nutzer so gesagt):
* **(R) Rakete:** Schaden je Gegnertyp aus der RE2-Tabelle, Zeile 17, Klammer 0.
* **(G) Handgranate:** EIN Wert D_G = max HP der Standard-Gegner + 1 (Todesregel hp < 0). Die Boss-Regel der Spur GR
  (Rest R, danach N Pistolenschüsse) entfällt, Bosse nehmen D_G.
* **Zusatz Orchestrator (H3):** Gator-Boss-HP byte-true 25240 statt 25300 ("Original oder nicht").

## 2. Werte mit Adressen

### 2.1 Handgranate D_G = 251 (`RE15_GRANATE_DG`, `include/re15_damage.h`)

| Teil | Beleg |
|---|---|
| 250 = Maximum der Standard-Gegner, Typ 0x11 | `addiu v1,zero,250` @0x801008C8 / `sh v1,342(s2)` @0x801008CC (`info/re2leon/COMMON/BIN/EMOVL10_S0.BIN`) |
| +1 = Todesregel hp < 0 | RE1.5 `bgez v1,0x80013024` @0x80013014; RE2 `bgez v1,0x80047294` @0x80047284 (mit 0 HP lebt ein Gegner) |
| kein Abfall | Applier FUN_800470C0 @0x80047208-0x8004727c ohne Abstand/Zufall/Schwierigkeit; Kasten {-2000,0,1000,500} @0x80010918 an P und P+900 |
| G5 Art 3/4 bleiben | 70 = Z11 @0x800A5FA4 / Z10 @0x800A5F90 (K0); die Art-2-Ausnahme 80 (Z9 @0x800A5F7C) ist entfernt |

### 2.2 Rakete (`re15_rakete_schaden`, `engine/src/re15_damage.c`)

Hitcode Gegnerkontakt 0x00030011 (`lui a3,0x3` @0x8001f7b0 / `ori a3,a3,0x11` @0x8001f7c0, `jal 0x800470c0` @0x8001f7bc),
Satz *(0x800A6A88 + Typ*4) + 16*20 (`lw a1,27272(at)` @0x8004722c, `addiu v0,v0,-20` @0x8004723c), Schaden (w0 >> 0) & 0x3FF
(`srlv` @0x80047254 / `andi v1,v1,0x3ff` @0x8004725c). Vorher DAT_8006f418[9] = 1000 (`e8 03` @0x8006f42a, im Original ohne Aufrufer).

| Port-Typ | RE2-Vorbild | Rakete | w0 @Adresse |
|---|---|---|---|
| 0x10-0x14, 0x18, 0x19 Zombies | 0x10.. | 900 | `384e1384` @0x800A426C |
| 0x15-0x17 Zombies | 0x15.. | 900 | `384e1384` @0x800A43E8 |
| 0x1A Gitterhände | 0x2D Zombie-Arme | 40 | `0280a028` @0x800A52C0 |
| 0x20 Hund | 0x20 | 300 | `12c4b12c` @0x800A4564 |
| 0x21 Krähe | 0x21 | 300 | `12c4b12c` @0x800A46E0 |
| 0x23 Alligator / Gator-Boss | 0x23 | 110 | `06e1b86e` @0x800A4B54 |
| 0x25/0x26 Spinne, Baby, Feuer-Emitter | 0x25/0x26 | 200 | `0c8320c8` @0x800A4CD0 |
| 0x27 Gorilla | 0x22 Licker (RE2 hat keinen Affen) | 200 | `0c8320c8` @0x800A485C |
| 0x29 kleiner Affe | Kakerlake (alle kleinen Tiere 300) | 300 | `12c4b12c` @0x800A5144 |
| 0x2B Tyrant | 0x2B | 500 | `1f47d1f4` @0x800A55B8 |
| 0x2D Ivy | 0x2E Ivy | 300 | `12c4b12c` @0x800A5734 |
| 0x30 Birkin (Mensch, Form 1, Mutation, danach) | 0x30 G1 | 200 | `0c8320c8` @0x800A58B0 |
| 0x30 Riesenform (`re15_bf_granate_form` hp0 == `RE15_BB_FORM_HP`) | 0x34 G4 | 301 | `12d4b52d` @0x800A5D24 |
| 0x36 G5 (und 0x36 in 3080) | 0x36 | 200 | `0c8320c8` @0x800A601C |
| alle übrigen | Vorgabezeiger (z. B. @0x800A6AD8) | 900 | `384e1384` @0x800A426C |

### 2.3 H3 — Gator-Boss 25240 (`RE15_GR2_HP_INIT`, `include/re15_gator_re2_r41.h`)

Selbst disassembliert (re2_disasm.py), bevor eine Zeile geändert wurde:

```
EM23_OVL_0000.BIN
  8010051c lw   v1,-1164(v1)        ; 0x800cfb74
  80100524 andi v0,v1,0x40
  80100528 beq  v0,zero,0x80100558  ; ohne Bit 0x40 -> 300-Zweig
  8010052c andi v0,v1,0x20
  80100530 beq  v0,zero,0x80100548
  8010053c lhu  v0,17900(v0)        ; [0x801045ec] = 240
  80100544 addiu v0,v0,25000
  8010054c lhu  v0,17902(v0)        ; [0x801045ee] = 240
  80100554 addiu v0,v0,25000
  80100564 lhu  v0,17852(v0)        ; [0x801045bc] = 300 (nur ohne Bit 0x40)
  80100574 lhu  v0,17854(v0)        ; [0x801045be] = 300 (nur ohne Bit 0x40)
  80100580 sh   v0,342(s0)
read 0x801045b0 u16: [48,10173,8,992,0,0,300,300,...,240,240]  (Index 6/7 = 300, Index 30/31 = 240)
RE2 PSX.EXE (FUN_8002b48c, gerufen jal 0x8002b48c @0x8002af00 am Kopf von FUN_8002aee0)
  8002b4a4 lw   v0,-1164(v0)
  8002b4ac ori  v0,v0,0x40
  8002b4b4 sw   v0,-1164(at)
```

Schreiber-Scan auf 0x800cfb74 (lui 0x800d + sw/sh/sb -1164) in RE2 PSX.EXE und allen COMMON/BIN-Overlays: 34 Stores.
Keiner löscht Bit 0x40. Masken: 0x7fffffff @0x80039400, 0x010008d9 @0x8002c690, 0xfeffffff (TITLE @0x80100ce8, ENDING
@0x80101b50), 0xfffbffff (ENDING @0x8010118c/@0x8010139c/@0x80101b00), 0xffdfffff (RESULT @0x80100514), -2/-9 (TITLE
@0x801038c8-d0, ENDING @0x801013d8-e0, MEM_CARD @0x80101410). Alle übrigen ODER-n Bits dazu; MEM_CARD @0x80103af4 setzt 0x40.

Die Lesung hält. Den Setter gibt es allerdings nur in der RE2-EXE. In der RE1.5-EXE steht @0x8002b4ac `addiu s3,sp,40`
(Wortlaut des Auftrags "RE1.5 setzt" berichtigt). Das ändert nichts, denn das Gator-Modul ist ein RE2-Overlay. Folge: eine
Granate schwächt ihn (25240 − 251 = 24989 < 25000, `slti v0,v0,25000` @0x80101ec4), bis zur Schwäche braucht es weiter 3 Raketen.

## 3. Treffer bis zum Tod (für die Release Notes)

Gemessen in `test_r41_granate boss` (echter Hitscan, echte Explosion, echter Applier). Die HP-Folgen stehen in
`scratchpad/ws/granate_alle.log`.

### Standard-Gegner (aus voller Gesundheit, im Kasten bzw. in der Flugbox)

| Gegner | max HP | Rakete | Raketen | Granaten (251) |
|---|---|---|---|---|
| Zombies 0x10/0x12/0x13/0x16/0x18 | 143 | 900 | 1 | 1 |
| dicker Zombie 0x11 | 250 | 900 | 1 | 1 (Rest −1) |
| Hund 0x20 | 132 / 111 | 300 | 1 | 1 |
| Krähe 0x21 | 10 | 300 | 1 | 1 |
| Spinne 0x25 / RE2-Baby 0x26 | 136 / 1 | 200 | 1 | 1 |
| kleiner Affe 0x29 | 121 | 300 | 1 | 1 |
| Tyrant 0x2B | 126 | 500 | 1 | 1 |
| Ivy 0x2D | 100 | 300 | 1 | 1 |
| Gitterhände 0x1A (nie tot) | 250, nach DEATH 250 | 40 | 7 bis zum DEATH-Zweig | 1 bis zum DEATH-Zweig |

### Bosse (gemessen, HP nach jedem Treffer)

| Boss / Form | HP | Granaten (Folge) | Raketen (Folge) |
|---|---|---|---|
| Gorilla 0x27 | 360 | 2 (109, −142) | 2 (160, −40) |
| Alligator 0x23 RE1.5-KI | 300 | 2 (49, −202) | 3 (190, 80, −30) |
| Birkin 600-Form (3070, 5080, 50A0, 50E0) | 600 | 3 (349, 98, −153) | 4 (400, 200, 0, −200) |
| Birkin Form 1 bis zur Mutation (3071, 5090) | 600, Ende < 200 | 2 (349, 98) | 3 (400, 200, 0) |
| Birkin während der Mutation | 300 | nie (Revive 100) | nie |
| Birkin nach der Mutation | 300 | 2 (49, −202) | 2 (100, −100) |
| Riesenform 5090 | 1000 | 4 (749, 498, 247, −4) | 4 (699, 398, 97, −204) |
| G5 0x36 5090 | 600 | 3 (349, 98, −153) | 4 (400, 200, 0, −200) |
| Gator-Boss 2090 bis zur Schwäche | 25240 | 1 (24989) | 3 (25130, 25020, 24910) |

Im Spiel kommt die Birkin-Fern-Rückgabe dazu: +5 je Treffer ab Abstand 0xfa1 (@0x8011a134). Gemessen im Endkampf 5090:
Form 1 400 → 405 → 205 → 210 → 10. Die Zahl der Treffer ändert sich dadurch nicht.

### Endkampf 5090 im echten Spiel (Lauf leon_ma13, state.log)

Form 1: Raketen bei F413 / F444 / F475 (600 → 10), Mutation ab F477 (300). Die Raketen F507-F635 fallen in den Revive (100).
Riesenform ab F654 mit 1000 HP: F666 699, F698 398, F730 97, F761 −204 (tot), G5 ab F866.

| Kampf | Raketen | Granaten | vorher Raketen / Granaten |
|---|---|---|---|
| Leon 3070 | 4 | 3 | 1 / 1 Granate + 8 Schuss |
| Elza 3071 (Form 1 + nach der Mutation) | 3 + 2 = 5 | 2 + 2 = 4 | 2 / 2 Granaten + 16 Schuss |
| Endkampf 5090 (Form 1 + Riesenform + G5) | 3 + 4 + 4 = 11 | 2 + 4 + 3 = 9 | 1 + 2 + 3 |
| Gator-Boss bis zur Kanister-Szene | 3 | 1 (H3) | 1 / 1 Granate + 8 Schuss |
| Gorilla | 2 | 2 | 1 / 1 Granate + 4 Schuss |

## 4. Hinweise H1-H13 (Stand)

| Nr | Inhalt | Stand |
|---|---|---|
| H1 | 0x11 bestimmt D_G; ohne ihn wäre D_G 144 | übernommen (D_G 251) |
| H2 | Gorilla zählt als Boss (sonst D_G 361) | übernommen |
| H3 | Gator-Boss 25240 statt 25300 | **gebaut** (Commit e0cfc8c5d), Disassembly selbst geprüft, Lesung hält |
| H4 | Umgebungs-Modi `RE15_RE15_RE2Z_IMPORT=0` / `RE15_RE2_DMG_MODEL=0`: 0x18 mit 1058 HP überlebt eine Granate | unverändert, kein Menüpunkt |
| H5 | kein Abfall, kein Direkttreffer der Handgranate | unverändert (granate_r35.c) |
| H6 | Birkin nach der Mutation G1 200 statt G2 300 | so gebaut (Typ 0x30 = G1); Rakete 2 wie mit 300 |
| H7 | mit 0 HP lebt ein Gegner (G5/Birkin-600 nach 3 Raketen) | gemessen: G5 0 nach 3, −200 nach 4 (PRUEF 131) |
| H8 | Gitterhände: D_G löst DEATH-Zweig in einem Wurf aus | nicht neu gemessen |
| H9 | Feuer-Emitter 0x26 in 1090: Granate 251, Rakete 200 | Granate gemessen (probe_r34_reaktion 212: 100 → −151) |
| H10 | Gator-Boss nimmt in FLINCH/RE2_TOD keinen Schaden | unverändert |
| H11 | Endkampf braucht 11 Raketen; im Raum liegen keine | für die Release Notes |
| H12 | Säure/Brand, Werferrunden, Spielerseite (Art 2 = 1000 am Spieler), HP | unverändert (unit_damage grün) |
| H13 | (Prüfblatt nennt H1-H12; H13 nicht belegt) | — |

## 5. Riegel Soll/Ist

| Riegel | Soll (neue Rechnung) | Ist |
|---|---|---|
| unit_r41_granate PRUEF 1/2/4 | Schaden 251; 107 − 251 = −144 | grün |
| unit_r41_granate [6] PRUEF 70-82, 110-122, 130-132 | Tabelle Abschnitt 3 | grün |
| unit_r41_granate PRUEF 102 | 0x36 in 3080: Granate 251 | grün |
| unit_r41_granate_aus PRUEF 61 | 250 − 251 = −1 | grün |
| unit_r41_granate_ws PRUEF 140-146 (neu) | 39/39 Standard tot; 251-Dummy lebt mit 0; Rakete 60/60; Gegenprobe 8/8 | grün |
| unit_r34_schaden 13/13b/99/31/34/35/37/38 | −71 / 251 / −171 / −1 / 49 | grün |
| unit_r34_wurf 24 | −71 | grün |
| unit_r35_granate 112, nr, 234 | −71, −151 | grün |
| unit_r34_reaktion(+_ohne80) 190/194/212 | 349, 98, −151 | grün |
| unit_birkin_bahnsteig M4 | Rakete 200: Form 1 vorher auf 199, tödlich | grün |
| unit_r35_werfer 84 | 900 (nur Text) | grün |
| unit_r41_re2gator_sockel | HP-Start = RE15_GR2_HP_INIT 25240; geschwächt ab Treffer 31 | grün |
| integration_r35_granate gator / birkin | HP fällt um 251 | grün: "[gator]: HP 25240 -> 24989, Zustand 2", "[birkin]: HP 600 -> 349" |
| integration_r41_g5_tod / g5_westen | MA9 → MA13 (4 Riesenform-Raketen, Feuer endet vor dem G5) | grün: tod leon/elza "Tod bei HP 0"; westen 3 Läufe Szenenende F2083 (vorher F1954), Weg danach 15145 |
| übrige Integrationsriegel aus (e) und git grep (Granate/Werfer/Inventar/Alligator2/Gatorszene/Kanister/re2gator/Restpunkte/Birkin-Pistole) | keine Erwartungsänderung | grün (42/42) |

## 6. Tests

Summenzeilen wörtlich:

* unit_/prop_/probe_ ohne die 7 df-hängenden Skripttests, -j 4, VOR dem Nachziehen der Riegel:
  `99% tests passed, 6 tests failed out of 1066`. Rot waren genau die erwarteten: unit_r34_schaden, unit_r34_reaktion,
  unit_r34_reaktion_ohne80, unit_r34_wurf, unit_r35_granate, unit_birkin_bahnsteig.
* Dasselbe NACH dem Nachziehen (Endstand): `100% tests passed, 0 tests failed out of 1066`
* 42 Integrationsriegel, -j 2 --timeout 3000: `100% tests passed, 0 tests failed out of 42`

Vorher-Lauf (Gegenprobe): Abschnitt `[8] ws` und `[6] boss` wurden gegen die Engine VOR WS gebaut (git stash auf
`re15_port/engine` + `re15_port/include`). Rot waren 140, 141 (HP −750), 142 (−749), 144 (29/60), 70-80 sowie 81
(G5-Granate 80, 8 Granaten), 82 (Gator 25300, 250), 110-120, 122, 130 (Riesenform 0, −1000) und 132. Grün blieben 121 und 131,
denn die G5-Rakete war schon vorher 200. Beleg: `scratchpad/ws/vorher_ws_boss_beleg.log`.

## 7. Offen

* Endkampf mit Rakete: Die Raketen während der Mutation (F507-F635) wirken nicht (Revive 100). Das war vorher genauso.
* `include/re15_birkin_formen.h` behauptet "Bit 0x20 nie gesetzt". Der Scan zeigt aber zwei Schreiber, die 0x20 setzen:
  TITLE.BIN `ori v0,a0,0x20` @0x80102994 / `sw` @0x8010299c (bei s0[14] == 1) und MEM_CARD.BIN `ori v0,v0,0x20` @0x80103aa8.
  Für den Gator ist das ohne Folge (beide Zweige 240). Für die G4-HP (900-Zweig @0x80100690) muss man es prüfen.
  WS ändert daran nichts.
* H8 (Gitterhände nach D_G im Spiel) und H9 (ob der Feuer-Emitter im Spiel Kandidat ist) sind nicht im Spiel gemessen.
* Der Gator-Boss ist jetzt mit EINER Granate geschwächt (H3 + D_G). Das folgt aus RE2 und aus dem Nutzerentscheid, widerspricht
  aber dem früheren GR-Wunsch F4 ("nicht einfach mit einer Granate beenden"). Töten kann ihn weiter nur der Kanister. Das muss
  in die Release Notes.

## 8. Bildbeleg

RE15_FRAMEDUMP-Serie, Endkampf Leon 5090, beschleunigter Renderer, Messkopie `re15_pc_wsmess_leon_fd.exe`, Bilder 650-770 in
4er-Schritten: `scratchpad/ws/g5mess/leon_fd/fd_000650.ppm` bis `fd_000770.ppm` (31 Bilder, PNG-Auszüge 666/730/762).
state.log desselben Laufs: Riesenform F654 1000, F666 699, F698 398, F730 97, F761 −204 (Zustand 3), G5 F866.
