# Runde 39 Spur K — heliport1171 (Dossier, fortlaufend)

Baum: `.claude/worktrees/r39_heliport1171` (Zweig `r39/heliport1171`, Basis 1038dc15).

## Nutzer-Punkt (Wortlaut, AUFTRAG.md)

> Wenn John in Room 1171 sagt - "Elza hurry up" macht er so eine komische Kletter Animation statt der ran wink
> Animation, die er im Original nicht macht. Auch sonst scheinen die Animationen hier noch nicht ganz zu passen.

Abgrenzung (VERTRAG §0/§1.4): Spur A hat die Stimme "Alright, let's get down from here" (1171 msg 3 <- main02)
bearbeitet — nicht anfassen. Spur J aendert nur den Uebergang 1141 -> 1171 (Musik).

## Stand (Schritte)

- 2026-10-18: Dossier angelegt, Pflichtlektuere AUFTRAG/VERTRAG/UEBERGABE Runde 39, Dossier I_heliport1171 (Runde 38)
  gelesen.
- 2026-10-18 14:25: Messung vorher (eigener Bau 1038dc15, Lade-Weg) — Johns Clips je Szenenschritt, Ursache gefunden
  (RBJ-Kanal-Cache des Ports von den Kraehen belegt), Original-Mechanismus disassembliert (Abschnitte 1/2).

- 2026-10-18 17:06 (FORTSETZUNG nach Sitzungslimit, neuer Agent): Probe-Merge war abgebrochen, statt dessen master gemergt
  (b35c66bd = Welle 1+2 inkl. Spur H Walker-Sub-Init und Spur A Stimme 1171 msg 3). Baum sauber, kein MERGE_HEAD.
  `local_build.sh configure` + `build` auf b35c66bd: `LOCAL-BUILD-OK (configure)` / `LOCAL-BUILD-OK (build)` (exe 17:06).
  Kern-Riegel auf dem gemergten Stand:
  `ctest --test-dir re15_port/build -R "r39_heliport1171|r38_heliport1171|r39_stimmen_(tabelle|1171)|crow|kraehe|rbj" --timeout 900 -j 3 --output-on-failure`
  -> `100% tests passed, 0 tests failed out of 32` (170 s).
  Naechster Schritt: "Auch sonst" — jede Szenen-Animation in 1171 (John UND Elza) gegen das Original (Abschnitt 6).

- 2026-10-18 17:15-19:05: "Auch sonst" abgearbeitet (Abschnitt 6): K2 INIT-Pose, K3 Plc_motion-Saatbild, K4 Schub
  Phase 5 -> 6 gemessen, im Original belegt, umgesetzt (6d027394), nachgemessen, Gegenprobe Leon 1170, Riegel
  (d2d97a88, 696a282f), Gegenprobe der Riegel am alten Motor, Bilder, gezielte Tests gruen (Abschnitt 11).
- 2026-10-18 19:40-21:00: NACHBESSERUNG 1 (Abnahme 0, Maengel M1-M3; Abschnitt 12): Elzas Sub-6-Saattakt + Start nach
  dem Dispatcher-Bild (M1/M3a), Spieler-Schub Phase 5 -> 6 -> 7 (M2), Kraehen zeichnen das zuletzt posierte Bild (M3b);
  Riegel elza/kraehen + Integrationsblock, Gegenprobe am alten Motor rot, gezielte Tests gruen (12.10).
## 1. Messung vorher

Gebauter Stand 1038dc15 (eigener Baum, `local_build.sh configure` + `build`), Messkopie `re15_pc_k39_v1.exe`.
Lade-Weg wie `integration_r38_heliport1171_laden`: Karte `probe_r36_elza_karte re15_card.mcr 1171 2300 14365 1024 3:194`
(Elza, (3,0xC2)=1), `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1`, dazu `RE15_ANIM_TRACE` (Pose-Strom aus
anim_select_common.c: Bild, Aktor, Typ, motion, Clip, Bildzahl des GERENDERTEN Clips, Zaehler, Bild-Slot, ...),
`RE15_SCD_TRACE`, `RE15_STATE_LOG`, `RE15_EXIT_AT=60#10b1`. Lauf exit 0, Szene bis ROOM10B1.

ROOM1171 sub02, John = Aktor 1 (Sce_em_set Slot 0 @0x016C8, Typ 0x49). Skript-Schritte Johns (Work_set(2,0)) gegen den
Pose-Strom (`fc` = Bildzahl des Clips, den der Renderer tatsaechlich posiert):

| Bild | Skript | Port posiert (Clip / fc) | Original (Kanal) |
|---|---|---|---|
| F0-F220 | INIT, Eskorte (Kraehen < 15000 -> nur DECIDE) | eigene Bank Clip 2 / 52, Bild 1 stehend | INIT-Pose Loco-Paar Clip 2 Bild 0 (s. 2.4) |
| F221-F260 | Plc_motion(0,0,0) @0x01416 | **Bank 0 Clip 0 / 34** | RBJ rec1 Clip 0 / 20 |
| F261-F280 | Plc_motion(0,0,0) @0x0141E + Plc_flg 0x80 @0x01422 | **Bank 0 Clip 0 / 34 rueckwaerts** | RBJ rec1 Clip 0 / 20 rueckwaerts |
| F281-F443 | Plc_motion(2,6,0) @0x0142A | eigene Bank Clip 6 / 10 | eigene Bank Clip 6 / 10 |
| F444-F463 | msg 0 "Elza, hurry up!" @0x014BA, Plc_motion(2,6,0) @0x014C2 + Plc_flg 0x80 @0x014C6 | eigene Bank Clip 6 / 10 rueckwaerts | dito |
| F464-F591 | Plc_motion(0,2,0) @0x014CE, Sleep 60 | **Bank 0 Clip 2 / 50 = Kletter-Clip "Kante hoch"** | RBJ rec1 Clip 2 / 96 |
| F686-F745 | msg 1, Plc_motion(0,0,0) @0x01526 | **Bank 0 Clip 0 / 34** | RBJ rec1 Clip 0 / 20 |
| F746-F775 | Plc_motion(0,0,0) @0x0152E + Plc_flg 0x80 @0x01532 | **Bank 0 Clip 0 / 34 rueckwaerts** | RBJ rec1 Clip 0 / 20 rueckwaerts |

(Walks Plc_dest 9/4/5, Sub 6 und Schub 0x13 posieren die richtigen Kanaele: eigene Bank Clip 5 / 30, Clip 1 / 16,
Clip 2 / 52, Loco Clip 17 / 10, 18 / 25.)

Bank 0 Clip 2 ist der Kletter-Clip: `RE15_NPK_KANTE_CLIP_HOCH 2` (+0x94 = 2 @0x800526a0/a4, Kanal +0x84/+0x16c,
anim_set @0x800527f0-f8; include/re15_npckante_r38.h:38) = genau der "komische Kletter"-Clip des Nutzers.

Ursache im Port (gemessen): der Marker-Binder ist lazy mit einem Cache von ZWEI Eintraegen
(`RE15_RBJ_CACHE_MAX 2`, engine/src/enemy_common.c), und jeder abgefragte Aktor-Slot belegt einen Eintrag — auch einer
OHNE Record ("negativ cachen"). Der Renderer fragt den RBJ-Kanal fuer JEDEN Aktor in state 4 / Sub 0 ab (main.c
Executor-Kanal-Render, ohne Typ-Tor). Die beiden Ereignis-Kraehen (Aktoren 2/3, Typ 0x21) stehen ab F0 in state 4 /
Sub 0 (FLIGHT-2 ASCEND, state.log F0 `[2 t=21 st=4 ss1=0 ...] [3 t=21 st=4 ss1=0 ...]`) und belegen beide Eintraege mit
"kein Record" (ihre Marker-Bits 0x4/0x8 stehen in keinem Record). John kommt erst F221 in state 4 / Sub 0 —
`rbj_resolve_slot(1)` findet keinen freien Eintrag, gibt -1 zurueck, Renderer und Uhr fallen auf die Container-Bank
(dir[1] = Bank 0, 24 Clips) zurueck.

Regression? Nein: die Szene lief vor Runde 38 Spur I nie bis zu Johns Plc_motion (sub02 wartete ewig @0x013F4 auf
(5,0x1C), gemessen in analysis/befunde_runde38/I_heliport1171.md Abschnitt 1 fuer v0.8.23-v0.8.25). Seit v0.8.26 laeuft
die Szene durch und zeigt den Fehler zum ersten Mal.

## 2. RE-Belege

### 2.1 Johns Baenke (CDEMD1.EMS, TOC @0x80073178[0x49] = Sektor 0x6AD, 0x217F0 B; CDEMD0 Sektor 0x6AB bytegleich)
- Verzeichnis @0x217CC, 9 Eintraege. dir[1] Bank 0 (Loco +0x16c/+0x84): 24 Clips {34,32,50,26,20,20,50,1,1,1,1,25,1,1,
  1,1,1,10,25,1,1,1,30,30}; dir[3] Bank 1 (eigene +0x174/+0x170): 14 Clips {22,16,52,1,50,30,10,16,1,17,1,17,1,1}; dir[5]
  leer. Kanal-Lader FUN_80022300: +0x16c=dir[1] @0x80022498, +0x84=dir[2] @0x800224a8, +0x174=dir[3] @0x800224b8,
  +0x170=dir[4] @0x800224c8 (memory cutscene-npc-system).
- ROOM1171.RDT RBJ (RDT+0x5C -> @0x188C, 0x409C B): 2 Records. rec0 Marker 0x1 (Spieler), EMR 15 Bones / 80 B / 50
  Keyframes, EDD 2 Clips {20,30}; **rec1 Marker 0x2 (Gegner 0 = John), 15 Bones / 80 B / 146 Keyframes, EDD @0x3E34
  3 Clips {20,30,96}**.

### 2.2 Plc_motion @0x80041b90 (PSX.EXE)
`lbu a2,1(v0)` (pc[1] = Sub) @0x80041b98, `lhu a1,2(v0)` @0x80041b9c, `sb a1,148(v0)` (+0x94 = pc[2]) @0x80041ba8,
`sb v1(=4),4(v0)` @0x80041bb0, `sb zero,6(v0)` @0x80041bb4, `sb a2,5(v0)` (+0x5 = pc[1]) @0x80041bc4,
`sh a1(>>8),452(v0)` (+0x1C4 = pc[3], loescht das Rueckwaerts-Bit) @0x80041bc8.

### 2.3 Welcher Kanal je Sub (NPC-Tabelle @0x80076ca0: [0]=0x80050cb8 [1]=0x80050ddc [2]=0x80050f00 ...)
- Sub 0 0x80050cb8: `lw a0,384(v0)` (+0x180) @0x80050d40, `lbu a2,452(v0)` / `srl a2,a2,7` (Rueckwaerts = +0x1C4 Bit 7)
  @0x80050d44/50, `lw a1,388(v0)` (+0x184) @0x80050d48, `jal 0x8001f314` @0x80050d4c; Ende -> +0x6 = 2 @0x80050da4
  (Halten), Schleife nur mit +0x1C4 & 4 @0x80050dbc-c8.
- Sub 2 0x80050f00: `lw a0,368(v0)` (+0x170) @0x80050f88, `lw a1,372(v0)` (+0x174) @0x80050f90, `jal 0x8001f314`
  @0x80050f94.

### 2.4 Wer +0x180/+0x184 belegt: der Marker-Binder FUN_8001b3f8, EINMAL beim Raumaufbau
- Raumaufbau: `jal 0x8003ef6c` (SCD-Init, Spawns) @0x80039a00, danach `jal 0x8001b3f8` @0x80039a08.
- FUN_8001b3f8: RBJ = Raumkopf+0x5C (`lw a2,92(v0)` @0x8001b404, 0 -> Ende @0x8001b40c); Recordzahl `lbu t2,4(a2)`
  @0x8001b420; je Record Marker `lw a0,0(v1)` @0x8001b438; Bit 0 -> Spieler-Kanal 0x800acbd4/0x800acbd8 @0x8001b450/60;
  Gegner-Bits: Entity-Zaehler a3 (einmal 0 @0x8001b410, `addiu a3,a3,1` je Bit @0x8001b4c4), Stride 500
  (@0x8001b480-90), `sw t1,0(at)` -> 0x800acdac+500*a3 (= Gegner a3 +0x180) @0x8001b4a0, EDD -> 0x800acdb0+500*a3
  (+0x184) @0x8001b4bc. KEINE Kapazitaetsgrenze, kein Typ-Tor: jeder markierte Gegner wird gebunden.
- ROOM1171: rec1 Marker 0x2 -> a3 = 0 -> Gegner 0 = John (Port-Aktor-Slot 1): +0x180/+0x184 = rec1. Damit spielt das
  Original Plc_motion(0,0,0) = RBJ rec1 Clip 0 (20 Bilder) und Plc_motion(0,2,0) = RBJ rec1 Clip 2 (96 Bilder).

### 2.5 Johns Wurzel 0x8011dc68 (STAGE1.BIN; Eintrag @0x80072cd0 gesetzt @0x8011e954-5c)
- Freeze-Tore `lui v1,0x2000` / `and` auf 0x800aca40 @0x8011dc80-88 und +0x9 & 0x20 @0x8011dca4-a8; Zustandstabelle
  @0x80121808: [0]=0x8011dd98 INIT, [1]=0x8011df40 Eskorte, [4]=0x80050be8 Executor.
- INIT 0x8011dd98: +0x4 = 1 @0x8011ddac, +0x94 = 2 @0x8011de7c, +0x95 = 0 @0x8011de8c, +0x8f = 0 @0x8011de9c,
  `lw a0,132(v0)` (+0x84) / `lw a1,364(v0)` (+0x16c) / `jal 0x8001f314` @0x8011debc-c4 = die INIT-Pose ist Clip 2 Bild 0
  des LOCO-Paars (Bank 0); +0x9 & 0x40 -> +0x4 = 4 / +0x5 = 6 @0x8011df1c/2c.
- Eskorte 0x8011df40: naechster lebender Gegner < 15000 (`ori a0,zero,0x3a98` @0x8011df48, `jal 0x800509e4` @0x8011df54)
  -> +0x9 = 1 @0x8011df94; Dispatch +0x9 & 0xF ueber @0x8012181c: [1] = 0x8004f100 (nur DECIDE, kein anim_set).
- 2026-10-18 14:41: Fix 18940e95 (RBJ-Cache) gebaut + gemessen (Abschnitt 4). Vorher-Bau fuer die Gegenproben laeuft in
  re15_port/build_bisect: dafuer steht enemy_common.c VORUEBERGEHEND auf 1038dc15 in der Arbeitskopie. Steht beim Lesen
  ein ` M re15_port/engine/src/enemy_common.c` ohne Rueckmeldung hier: `git checkout -- re15_port/engine/src/enemy_common.c`
  (der Fix ist in 18940e95 committet), NICHT committen.
- 2026-10-18 14:43-14:55: Vorher-Bau re15_port/build_bisect (enemy_common.c von 1038dc15, danach zurueckgesetzt —
  Arbeitskopie wieder sauber), Gegenproben gefahren (Abschnitte 4/5), Riegel 0b468a87.

## 3. Umsetzung

`re15_port/engine/src/enemy_common.c` `rbj_resolve_slot` (3 Zeilen, Kommentar "Runde 39 Spur K"): ein Aktor-Slot OHNE
Record (`rec < 0` nach Marker-Scan und dem 10F0-Alias) gibt -1 zurueck, BEVOR er einen der zwei Cache-Plaetze nimmt.
Vorher wurde er "negativ gecacht" und belegte einen Platz. Original: FUN_8001b3f8 bindet nur Marker-Bits und hat keine
Kapazitaetsgrenze (@0x8001b470-c4) — ein Slot ohne Bit bekommt nichts und verdraengt nichts. Kosten: der Marker-Scan
(<= 8 u32-Lesungen) laeuft fuer Slots ohne Record bei jeder Abfrage neu.
Census der ausgelieferten RDTs (scratchpad rbjzensus.py, alle 6 Stages): hoechstens 2 Gegner-Bindungen je Raum ausser
ROOM1040/1041 (Marker 0x7E = Gegner 0..5 an rec0) — dort aendert sich nichts, weil nur Slots OHNE Record anders
behandelt werden.

KEIN neuer Quelltext ausser den Riegeln; kein Haken in den Wave-3-Dateien anderer Spuren.

## 4. Messung nachher

Gleicher Lade-Weg, Stand 18940e95 (Messkopie, exit 0, Szene bis ROOM10B1, Bildzahlen unveraendert):

| Bild | Skript | VORHER (1038dc15) | NACHHER (18940e95) |
|---|---|---|---|
| F221-F260 | Plc_motion(0,0,0) @0x01416 | Bank 0 Clip 0 / 34 | **RBJ rec1 Clip 0 / 20** (Bild 1..19, haelt 19) |
| F261-F280 | + Plc_flg 0x80 | Bank 0 Clip 0 / 34 rueckw. | **RBJ rec1 Clip 0 / 20 rueckw.** (18..0) |
| F281-F463 | Plc_motion(2,6,0) (+0x80) | eigene Clip 6 / 10 | eigene Clip 6 / 10 (unveraendert) |
| F464-F591 | Plc_motion(0,2,0) @0x014CE | **Bank 0 Clip 2 / 50 = Klettern** | **RBJ rec1 Clip 2 / 96 = Heran-Winken** |
| F686-F775 | Plc_motion(0,0,0) x2 (+0x80) | Bank 0 Clip 0 / 34 | **RBJ rec1 Clip 0 / 20** |

Bilder (RE15_FRAMEDUMP, RE15_WINDOW_SCALE=1, beschleunigter Renderer, F440-F528 je 8 Bilder):
`K_belege/vorher_hurry_up_F440-528.png` — F480-F504 hebt John ein Bein, geht in die Hocke und richtet sich wieder auf (der
Kletter-Clip "Kante hoch"); `K_belege/nachher_hurry_up_F440-528.png` — John senkt die Waffe (eigene Clip 6 rueckwaerts),
dreht sich zur Kamera und winkt Elza mit erhobenem Arm heran (F504-F520).

Gegenprobe Leon ROOM1170 (echter Weg wie integration_r37_roy_leon1170: Neuspiel -> Montage ROOM1240 -> Tuer -> ROOM1170,
RE15_FPS=240, EXIT_AT 1200#1170; Elliot geladen): Pose-Strom (4360 Zeilen, Leon 4043 / Elliot 317) und state.log
VORHER (build_bisect) und NACHHER BYTEGLEICH (md5 3848a27f... / e6e98cb7...).

## 5. Tests

Registriert nur in `re15_port/tests/unit/probes/r39_heliport1171.cmake`:
- `test_r39_heliport1171.c` (Unit, ausgelieferte RDT/Archive, Bilder in main.c-Reihenfolge PLUS die RBJ-Abfrage des
  Renderers = main.c Executor-Kanal-Render, Slot-Reihenfolge, ohne Typ-Tor): Teile `banken`, `binder` (1171 John nach den
  Kraehen und nach 19 Slots ohne Record; Gegenprobe ROOM10D0 Marvin rec1; ROOM1170 Elliot bleibt ausgenommen), `szene`
  (sieben Plc_motion-Schritte Johns: Sub/Clip/Kanal-Laenge/Richtung = Original; kein Bild an der Kletter-Laenge 50; Szene
  bis ROOM10B1), `leon1170`.
- `tests/integration/test_r39_heliport1171.cmake` (echte exe, Elza): `weg` (ROOM1131 -> Flurtuer Slot 3 -> ROOM1171 ->
  Selbst-Tuer Slot 5 -> Szene -> ROOM10B1) und `laden`; misst den Pose-Strom RE15_ANIM_TRACE im Szenen-Abschnitt.

Ergebnisse (eigener Baum, Stand 0b468a87):
- `ctest --test-dir re15_port/build -R "unit_r39_heliport1171" --timeout 600 --output-on-failure` ->
  `100% tests passed, 0 tests failed out of 4`
- `ctest --test-dir re15_port/build -R "integration_r39_heliport1171" --timeout 900 --output-on-failure -V` ->
  `100% tests passed, 0 tests failed out of 2` (weg 65,1 s / laden 53,6 s; je Heran-Winken 128 Bilder, Kletter 0, RBJ-Clip 0
  150, Bank-0-Clip-0 0, eigener Clip 6 183)
- GEGENPROBE vorher (1038dc15, build_bisect): Unit `FEHLER (10 Fehler)` — binder 4 (John/Marvin ohne Record-Kanal),
  szene 6 ("Laenge 34", "Laenge 50", 128 Kletter-Bilder); Integration laden ROT: "rendert nicht RBJ rec1 Clip 2 (96 Bilder:
  0) bzw. noch den Bank-0-Kletter-Clip (50 Bilder: 128)".
- Nachbarn (Unit/Probe der Module/Raeume mit NPC-RBJ-Kanal):
  `ctest --test-dir re15_port/build -R "^(unit|probe)_.*(marvin|ada|1050|1150|r36_elza|roy|elzaems|szene11c0|dining1141|elzatoene|kleinkram|montage|r39_toene|werkstatt11b0|lane_b1|eintritt|gegnerzahl|schluessel|cut1150|szene1150|npc|10d0|1141|1171|10b1|rbj|cine)|^eintritt_pose_einmal$|^r19_marke1_10d0_c1$" --timeout 600 -j 4`
  -> `100% tests passed, 0 tests failed out of 157`
- 2026-10-18 14:58: Probe-Merge gegen r39/integration (Welle 2 inkl. Spur H Walker-Fix 564810c0) NUR in der Arbeitskopie
  (`git merge --no-commit --no-ff r39/integration`), Bau in re15_port/build_bisect, danach `git merge --abort`. Steht beim
  Lesen ein MERGE_HEAD im Baum, hat das Sitzungslimit die Probe unterbrochen: `git merge --abort`, nichts davon committen.

## 6. "Auch sonst" — jede Szenen-Animation Johns in 1171 gegen das Original (Fortsetzung 2026-10-18)

### 6.1 Messung (gemergter Stand b35c66bd, Lade-Weg wie Abschnitt 1, RE15_ANIM_TRACE + RE15_STATE_LOG)

Johns Pose-Strom (Aktor 1, Typ 0x49) Zeile fuer Zeile gegen die Original-Instruktionen. "Port posiert" = GERENDERTE
Slots (anim_select_common.c), "Original" = was FUN_8001f314 im selben Takt posiert (Herleitung 6.2).

| Bild | Skript (sub02) | Port posiert | Original | Befund |
|---|---|---|---|---|
| F0-F220 | INIT, Eskorte +0x9=1 (Kraehen < 15000, nur DECIDE 0x8004f100) | eigene Bank Clip 2 (52) Bild **1** gehalten | **Loco-Paar** Clip 2 (50) Bild **0** gehalten | **K2** |
| F221-F260 | Plc_motion(0,0,0) @0x01416 | RBJ Clip 0 (20): 1,2..19, haelt ab F239 | 0,1..19, haelt ab F240 | **K3** |
| F261-F280 | Plc_motion(0,0,0) + Plc_flg 0x80 | 18..0 | 19..0 | **K3** |
| F281-F443 | Plc_motion(2,6,0) @0x0142A | eigene Clip 6 (10): 1..9 | 0..9 | **K3** |
| F444-F463 | Plc_motion(2,6,0) + 0x80 @0x014C2 | 8..0 | 9..0 | **K3** |
| F464-F591 | Plc_motion(0,2,0) @0x014CE (Heran-Winken) | RBJ Clip 2 (96): 1..95 | 0..95 | **K3** |
| F592-F609 | Plc_dest(0,9,1) Sub 9 | eigene Clip 5 (30) 0..17 | dito (Sub 9 +0x94=5 @0x80051d38-3c, +0x95=0 @0x80051d4c, f314(+0x170,+0x174) @0x80051e9c-ea4) | ok |
| F610-F663 | Plc_dest(0,4,1) Sub 4 | eigene Clip 5 0..23, Ankunft | dito | ok |
| F664-F685 | Plc_dest(0,9,1) Sub 9 | eigene Clip 5 0..21 | dito | ok |
| F686-F745 | Plc_motion(0,0,0) @0x01526 | RBJ Clip 0: 1..19 | 0..19 | **K3** |
| F746-F775 | + 0x80 @0x0152E | 18..0 | 19..0 | **K3** |
| F776-F791 | Plc_dest(0,9,1) Sub 9 | eigene Clip 5 0..15 | dito | ok |
| F792-F846 | Sub 6 (Ankunft) | eigene Clip 1 (16) 0..15, dann Clip 2 (52) | dito (+0x94=1 @0x80051854, f314 @0x80051884-8c; +0x94=2 @0x800518c4-c8, f314 @0x800518e8-f0) | ok |
| F847-F932 | Plc_dest(0,0x13,0x14) Schub Ph 0..5 | eigene Clip 5 Bild 0; Loco Clip 17 (10) 0..9; Loco Clip 18 (25) x3 | dito | ok |
| F933 | Ph 5, Bit (5,0x14) gesetzt | **Loco Clip 17 rueckw. Bild 9** | **Loco Clip 18** (Pose aus anim_set @0x80052af4 VOR der Bit-Abfrage @0x80052b44) | **K4** |
| F934-F943 | Ph 6 | Clip 17 rueckw. 9..0 | 9..0 | ok |
| F944 | Ph 7 (kein anim_set) | Clip 17 Bild 0 gehalten | dito | ok |
| F945- | Sub 6 | Clip 1 0..15, Clip 2 | dito | ok |

### 6.2 RE-Belege

- **f314 posiert ZUERST, dann Vorschub** (selbst nachdisassembliert): Bildzeiger = Clipbasis + 4*(+0x95)
  (lbu v0,149(t0) @0x8001f35c, rueckwaerts fc-(+0x95)-1 @0x8001f344-54), sw a2,360(t0) (+0x168) @0x8001f36c,
  Pose in FUN_8001f3bc (jal @0x8001f38c), ERST an dessen Ende addiu v0,v0,1 / sb v0,149(v1) @0x8001f618-1c;
  sltu v0,v0,s4 @0x8001f624, sonst sb zero,149(v1) @0x8001f63c + Rueckgabe 1.
- **K2 INIT-Pose** (alle NPC-INITs, nicht nur John): ori v0,zero,0x2 / sb v0,148(v1) (+0x94=2) @0x8011de78-7c,
  sb zero,149(v0) (+0x95=0) @0x8011de8c, sb zero,143(v0) (+0x8f=0) @0x8011de9c, addu a2,zero,zero @0x8011deb8,
  lw a0,132(v0) (+0x84) / lw a1,364(v0) (+0x16c) / jal 0x8001f314 / ori a3,zero,0x200 @0x8011debc-c8. Dasselbe
  Muster (+0x95=0, +0x8f=0, a2=0, a3=0x200, Loco-Paar) in ALLEN NPC-INITs (eigener Scan, scratchpad init_check.py):
  STAGE1 @0x8011c808/@0x8011cdd8/@0x8011d3e4/@0x8011d978/@0x8011dec4/@0x8011e49c, STAGE3 @0x8011c520/@0x8011caf8,
  STAGE5 @0x8011c1a0/@0x8011c770/@0x8011cd34/@0x8011d30c/@0x8011d978. Danach Eskorte 0x8011df40: Gegner < 15000
  (ori a0,zero,0x3a98 @0x8011df48, jal 0x800509e4 @0x8011df54) -> +0x9 = 1 @0x8011df94 -> Tabelle @0x8012181c[1] =
  0x8004f100 = DECIDE ohne jal 0x8001f314 (scan bis jr ra @0x8004f1fc). Die Wurzel 0x8011dc68 ruft ausser dem
  Zustands-Dispatch (jalr @0x8011dd08) kein anim_set (Calls 0x8001bd60/0x80039e7c/0x8002b498/0x8002aec4/0x8002b544/
  0x8003b0a4/0x80037358/0x8001b38c/0x8001b064). Die Pose steht also bis F220 auf Loco Clip 2 Bild 0. Die Eskorte-EXEC
  (+0x9=0) posiert dagegen aus der eigenen Bank (Stehen lw a0,368(v0) / lw a1,372(v0) @0x8004f384-88) — die
  Port-Regel "state 1 = eigene Bank" stimmt fuer die EXEC, nicht fuer die INIT-Pose davor.
  Bild (scratchpad k_tools/john_init_posen.png, Posenkette von emd_ansichtsblatt.py): Loco Clip 2 Bild 0 = aufrecht,
  symmetrisch, Arme haengend; eigene Clip 2 Bild 1 = Leerlauf mit geneigtem Kopf und angewinkelten Armen.
- **K3 Plc_motion-Saatbild** (Sub 0 0x80050cb8; Subs 1/2/3 0x80050ddc/0x80050f00/0x80051024 gleich gebaut):
  Phase 0 sb v0(=1),6(a0) @0x80050cec, sb v0(=7),143(v1) @0x80050cfc, sb zero,149(v0) @0x80050d0c, No-Blend
  +0x1C4&0x40 -> sb zero,143(v1) @0x80050d30, faellt OHNE Sprung in den Phase-1-Rumpf @0x80050d34:
  jal 0x8001f314 @0x80050d4c (a0=+0x180 @0x80050d40, a1=+0x184 @0x80050d48, a2=+0x1C4>>7 @0x80050d44/50) ->
  im Saat-Takt wird Bild 0 (rueckwaerts fc-1) POSIERT. Doppelschritt +0x1C4&8 @0x80050d70 -> zweiter f314 @0x80050d88;
  Ende -> +0x6 = 2 @0x80050da4, LOOP +0x1C4&4 -> +0x6 = 1 @0x80050dc8. Der Port-Handler (enemy_ai_common.c
  re15_npc_sub_motion) schob im Saat-Takt sofort vor (Bild 1) und ignorierte den Vorhalte-Takt, den op_plc_motion
  seit b06f2a8f (Runde 26: "die Pose-FSM faehrt die Phase-0-Kette ... und posiert im selben Bild Bild 0") setzt
  (motion_init_delay = 1, scd_vm.c) — beim Spieler wird er verbraucht (player_common.c), beim NPC nie.
- **K4 Schub Phase 5 -> 6** (0x80052864): Phase 5 @0x80052ab4: +0x95==1 -> SE @0x80052ad8, jal 0x8001f314 (Loco,
  a2=0) @0x80052af4 = die Pose dieses Takts, Bildwort&0x2000 -> 0x800369f8 @0x80052b24, DANACH Bit-Abfrage
  jal 0x8004efe4 (0x800b1028, +0x1C3) @0x80052b44 -> sb v0(=6),6(v1) @0x80052b60, +0x94 = 0x11 @0x80052b6c-70,
  +0x95 = 0 @0x80052b80 (Delay-Slot von j 0x80052bf4 = Ausgang, kein weiteres anim_set). Erst Phase 6 @0x80052b84
  posiert Clip 0x11 rueckwaerts (ori a2,zero,0x1 @0x80052b8c, jal 0x8001f314 @0x80052b98). Der Port setzte den
  Clip 0x11 schon im Bit-Takt (szenenleon_r36.c re15_szl_schub_npc Fall 5) -> Clip 17 ein Bild zu frueh, Bild 9 doppelt.

### 6.3 Nicht in Johns 1171-Szene, nur festgehalten (OFFEN, kein Eingriff)
- Eskorte-EXEC-Saaten (Stehen/Gehen/Drehen/Nah/Laufen, re15_npc_escort_exec) setzen +0x95=0 ohne Vorhalte-Takt ->
  dieselbe Ein-Bild-Vorausklasse wie K3 (Original: Saat z.B. @0x8004f354/64 und f314 @0x8004f38c im selben Takt). In
  1171 laeuft die EXEC nie (John bleibt bis zum ersten Plc_motion in +0x9=1).
- Elza (Spieler) in derselben Szene: Einzelbilder Clip 1 Bild 1 zwischen TURN und WALK (F1, F840, F846) und der
  Schub-Ruecklauf ein Bild frueh (F933) — Spieler-Walker (Spur H Welle 2, dort als eigener RE-Punkt vermerkt) bzw.
  Spieler-Schub (Runde 36 J); nicht Johns Animation.

### 6.4 Umsetzung K2/K3/K4 (6d027394)

- NEU `re15_port/engine/src/heliport1171_r39.c` + `re15_port/include/re15_heliport1171_r39.h` (K2): Merker je Aktor-Slot
  "zeigt noch die INIT-Pose" — gesetzt in der NPC-INIT, geloest beim ersten Eskorte-EXEC-Takt und beim ersten
  Executor-Takt; gilt nur fuer Typ 0x40..0x4d, state 0/1/4, ohne Plc_dest-Walker und solange motion = 2 /
  anim_frame = 0 (RE15_R39K_INIT_CLIP @0x8011de7c, RE15_R39K_INIT_BILD @0x8011de8c). Raumaufbau loescht alle Merker
  (re15_r39k_install, letzte Installer-Zeile in scd_room_setup.c; die INIT laeuft erst im Spieltakt).
- `enemy_ai_common.c` (Haken, je "Runde 39 Spur K"): Include; re15_actor_uses_own_bank -> 0 und
  re15_actor_uses_loco_bank -> 1, solange der Merker gilt (die EINE Bank-Regel fuer Renderer main.c:9974/10017,
  Treffer-Skelett schuss_r36.c und Uhr re15_actor_clip_len); NPC-INIT anim_frame 1 -> 0 + Merker setzen;
  Merker loesen vor re15_npc_escort_decide/exec und vor re15_npc_sub_dispatch.
- `enemy_ai_common.c` re15_npc_sub_motion (K3): Phase 0 setzt den Vorhalte-Takt (motion_init_delay = 1), Phase 1
  ruft je f314 ein re15_npc_anim (posieren -> vorschieben -> +0x8f-1, Rueckgabe 1 im Takt, der fc-1 posiert);
  Doppelschritt +0x1C4&8 (@0x80050d70/88), HOLD +0x6 = 2 (@0x80050da4), LOOP +0x1C4&4 (@0x80050dbc-c8).
- `szenenleon_r36.c` re15_szl_schub_npc (K4): im Bit-Takt der Phase 5 nur +0x6 = 6 und der Port-Merker
  RE15_R39K_SCHUB_SAAT (0xFE in sub_state_3); Phase 6 setzt Clip 0x11 rueckwaerts in ihrem ersten Takt (npc_clip mit
  Vorhalte-Takt -> posiert Bild fc-1 = 9) und spielt dann wie bisher.

### 6.5 Messung nachher (Stand 6d027394, gleicher Lade-Weg, exit 0, Szene bis ROOM10B1, Bildzahlen unveraendert)

| Bild | Skript | VORHER (b35c66bd) | NACHHER (6d027394) | Original |
|---|---|---|---|---|
| F0-F220 | INIT/Eskorte | eigene Clip 2 (52) Bild 1 | **Loco Clip 2 (50) Bild 0** (loco=1) | Loco Clip 2 Bild 0 |
| F221-F260 | Plc_motion(0,0,0) | RBJ C0: 1..19 | **0..19**, haelt ab F240 | 0..19, haelt ab F240 |
| F261-F280 | + 0x80 | 18..0 | **19..0** | 19..0 |
| F281-F443 | Plc_motion(2,6,0) | eigene C6: 1..9 | **0..9** | 0..9 |
| F444-F463 | + 0x80 | 8..0 | **9..0** | 9..0 |
| F464-F591 | Plc_motion(0,2,0) Heran-Winken | RBJ C2: 1..95 | **0..95** | 0..95 |
| F686-F775 | Plc_motion(0,0,0) x2 | 1..19 / 18..0 | **0..19 / 19..0** | dito |
| F933 | Schub Ph 5 Bit-Takt | Clip 17 rueckw. 9 | **Clip 18 (25) Bild 0** | Clip 0x12 (anim_set @0x80052af4) |
| F934-F943 | Ph 6 | 9,8..1 (9 doppelt ab F933) | **9..0** | 9..0 |
| alle uebrigen Zeilen Johns | | | unveraendert | |

Nebenwirkungen im selben Lauf: Pose-Strom ALLER anderen Aktoren (Elza, Kraehen) vorher/nachher bytegleich
(md5 65b9f910b1e43bd3b5f6fef2582ce04a beide), Zustandslog ohne Johns Klammer 1138/1138 Zeilen gleich.
- 17:29: Vorher-Bau fuer die Gegenproben K2-K4 in re15_port/build_bisect: enemy_ai_common.c/szenenleon_r36.c/scd_room_setup.c stehen VORUEBERGEHEND auf b35c66bd in der Arbeitskopie. Steht beim Lesen ein ' M' auf diesen Dateien ohne Rueckmeldung hier: `git checkout HEAD -- re15_port/engine/src/enemy_ai_common.c re15_port/engine/src/szenenleon_r36.c re15_port/engine/src/scd_room_setup.c` (K2-K4 sind in 6d027394 committet), NICHT committen.
- Vorher-Bau erledigt (17:29): re15_port/build_bisect = b35c66bd-Stand der drei Dateien, Arbeitskopie danach mit
  `git checkout HEAD -- ...` zurueckgesetzt, `git status --porcelain` leer.

### 6.6 Gegenprobe Leon ROOM1170 (vorher build_bisect = b35c66bd, nachher build = 6d027394)

Weg wie integration_r37_roy_leon1170 (Neuspiel -> Montage ROOM1240 -> Tuer -> ROOM1170, RE15_FPS=240,
RE15_EXIT_AT=1200#1170), beide exit 0, ROOM1170 geladen. Pose-Strom 4360 Zeilen (Leon 4043, Elliot 317):
- **Leon bytegleich** (md5 der Leon-Zeilen c23ba5ea vorher = nachher); Zustandslog ohne Elliots Klammer 4043/4043
  Zeilen gleich. VORHER-md5 des ganzen Pose-Stroms 3848a27f / state.log e6e98cb7 = dieselben Werte wie in Abschnitt 4
  (1038dc15 + K1) — Welle 1+2 hat 1170 nicht veraendert.
- **Elliot 24 von 317 Zeilen anders, alle durch K2/K3 erklaert:**
  - F884-F889: INIT-Pose Loco Clip 2 (fc 50) Bild 0 statt eigene Clip 2 (fc 52). Elliot (Typ 0x47) hat die Wurzel
    0x8011d6d4 (Tabelle @0x80072cc8, gesetzt `sw v0,11464(at)` @0x8011e94c) und deren INIT posiert das Loco-Paar
    (+0x94 = 2 @0x8011d930, `jal 0x8001f314` @0x8011d978). Er steht dort mit g=40 schon in state 4 / Sub 6 / Phase 0,
    der Executor laeuft in diesen 6 Bildern aber nicht (Zustandslog: ss2 bleibt 0) — also bleibt im Original die
    INIT-Pose stehen, der Merker loest erst mit dem ersten Executor-Takt.
  - F920-F1200: Plc_motion(0,15,0) "Hey!" ab Bild 0 statt 1, rueckwaerts ab 19 statt 18 (K3). Bei RE15_FPS=240 posiert
    der Renderer anim_frame>>1, darum [0,0,1,1,..] statt [0,1,1,2,..].
  Das Zustandslog weicht nur in Elliots af ab (44 Zeilen, z.B. `st=4 ss1=6 ss2=0 g=40 mo=2 af=1` -> `af=0`).
- 18:12: Gegenprobe der neuen Riegel: enemy_ai_common.c/szenenleon_r36.c/scd_room_setup.c stehen VORUEBERGEHEND auf b35c66bd (Bau re15_port/build_bisect mit den NEUEN Testquellen). Steht beim Lesen ein ' M' auf diesen Dateien ohne Rueckmeldung hier: `git checkout HEAD -- re15_port/engine/src/enemy_ai_common.c re15_port/engine/src/szenenleon_r36.c re15_port/engine/src/scd_room_setup.c`, NICHT committen.
- 18:14: Gegenprobe-Bau fertig, Arbeitskopie mit `git checkout HEAD -- ...` zurueckgesetzt, `git status --porcelain` leer.

### 6.7 Gegenprobe der neuen Riegel und Bilder

Neue Testquellen gegen den ALTEN Motor (build_bisect = enemy_ai_common.c/szenenleon_r36.c/scd_room_setup.c von b35c66bd):
- `initpose`: FEHLER "221 Bilder ... (221 abweichend)"; `eskorte`: FEHLER "INIT-Bild: Loco Clip 2 Bild 0 (st 1, loco 0,
  Bild 1)"; `saatbild`: 7 FEHLER ("Saattakt Bild 1 (soll 0)" bzw. "Bild 18 (soll 19)" / "Bild 8 (soll 9)" in allen sieben
  Fenstern); `schub`: FEHLER "Bit-Takt (Bild 933): ... (Clip 17, rueckw. 1)"; `szene` (K1) OK.
- `integration_r39_heliport1171_laden` gegen build_bisect: ROT — "INIT-Pose Loco 2/50 Bild 0 0, eigene 2/52 vorher 221",
  "Saattakte 7 (falsch: F221:0/0/20/0:Bild1;F261:0/0/20/1:Bild18;F281:6/6/10/0:Bild1;F444:6/6/10/1:Bild8;
  F464:2/2/96/0:Bild1;F686:0/0/20/0:Bild1;F746:...)".
- Derselbe Stand (6d027394) im build/: alle gruen (Abschnitt 7).

Bilder (RE15_FRAMEDUMP, RE15_WINDOW_SCALE=1, beschleunigter Renderer, Lade-Weg):
- `K_belege/k2_initpose_F208-224.png`: oben VORHER (b35c66bd), unten NACHHER (6d027394), F208..F224 je 4 Bilder. Kamera 4
  schaltet bei F218 auf John: F220 VORHER Leerlauf-Haltung (Kopf geneigt, Arme angewinkelt), NACHHER die INIT-Pose (aufrecht,
  symmetrisch, Arme haengend); F224 = erste Geste (RBJ Clip 0). In Kamera 0/1 davor ist John nicht im Bild.
- `K_belege/k2_john_posen_loco2b0_vs_eigen2b1.png`: die beiden Posen direkt aus CDEMD0 Typ 0x49 (Posenkette
  emd_ansichtsblatt.py; Loco Clip 2 Keyframe 66 gegen eigene Bank Clip 2 Bild 1 Keyframe 38).
K3/K4 sind Ein-Bild-Verschiebungen; ihr Beleg ist der Pose-Strom (6.5), nicht ein Bild.

## 7. Tests (Stand nach K2-K4)

Registriert nur in `re15_port/tests/unit/probes/r39_heliport1171.cmake` (keine CMakeLists.txt angefasst):
- Unit `test_r39_heliport1171.c`, Teile `banken binder szene leon1170` (K1, Abschnitt 5) und NEU (Abschnitt 6):
  - `initpose` (K2): ganze Szene; bis zum ersten Plc_motion jede Zeile state 1, re15_actor_uses_loco_bank = 1,
    uses_own_bank = 0, Clip 2 an Loco-Laenge 50, gerendertes Bild 0, Merker aktiv (221 Bilder); im Plc_motion-Bild (221)
    Merker geloest, Sub 0 an RBJ rec1 Clip 0 (20).
  - `eskorte` (K2-Gegenprobe): Kraehen inaktiv -> INIT-Bild Loco Clip 2 Bild 0, danach 29/29 Takte eigene Bank
    (Eskorte-EXEC Stehen @0x8004f384-8c), nie mehr Loco.
  - `saatbild` (K3): je Plc_motion-Fenster (PC auf der Sleep-Zeile) Fensterbild k -> gerendert min(k,fc-1) bzw.
    gespiegelt; Saattakt Bild 0 / fc-1; +0x6 = 2 genau ab k = fc-1 (@0x80050da4); sieben Fenster.
  - `schub` (K4): Bit-Takt der Phase 5 (Bild 933) posiert Clip 0x12 vorwaerts, danach 10 Takte Clip 0x11 rueckwaerts
    9..0, +0x6 = 7 im 10. Takt.
- Integration `test_r39_heliport1171.cmake` (`weg`, `laden`, echte exe, Elza): zusaetzlich zu Abschnitt 5 jetzt
  INIT-Pose Loco 2/50 Bild 0 vor dem ersten Plc_motion (>= 150 Bilder, eigene 2/52 = 0), Kletter-Clip 2/50 nur NACH dem
  ersten Plc_motion gezaehlt (= 0), alle sieben Plc_motion-Laeufe beginnen mit Bild 0 / fc-1, Schub: Clip 17 rueckwaerts
  folgt direkt auf Clip 18 und laeuft 9,8,..,0. Gemessen weg: INIT 226, laden: INIT 221; je Saattakte 7 (falsch: keine),
  Schub [9,8,7,6,5,4,3,2,1,0], Heran-Winken 128, Kletter 0, RBJ-Clip 0 150, Bank-0-Clip 0 0, eigener Clip 6 183.
- Fremder Riegel angepasst (eine Bedingung, mit Beleg): `tests/unit/test_r37_roy.c` `spur 1170` (= integration_r37_roy_
  leon1170, Runde 37 K) verlangte fuer ALLE Clip-2-Zeilen Elliots die eigene Bank (52). Die 6 Zeilen vor dem ersten
  Executor-Takt sind seit K2 Elliots INIT-Pose = Loco Clip 2 (50) Bild 0 (Wurzel 0x8011d6d4, Tabelle @0x80072cc8, INIT
  +0x94 = 2 @0x8011d930, f314(+0x84,+0x16c) @0x8011d978). Die Bedingung laesst jetzt genau diese Form zu (fc 50, loco 1,
  Slot 0) und meldet sie ("davon INIT-Pose 6"); jede andere Clip-2-Zeile muss weiter Bank 1 (52) sein.

## 8. OFFEN (kein Teil von Johns 1171-Szene; mit Adressen fuer eine spaetere Runde)

- **Eskorte-EXEC-Saattakt** (re15_npc_escort_exec, enemy_ai_common.c): die fuenf Exec-Subs setzen Clip/+0x95 = 0 und
  rufen re15_npc_anim OHNE Vorhalte-Takt -> im Saattakt Bild 1 statt 0 (dieselbe Klasse wie K3). Original: Saat und f314
  im selben Takt, z.B. Stehen +0x94 = 2 @0x8004f354 / +0x95 = 0 @0x8004f364 / `jal 0x8001f314` @0x8004f38c, Gehen
  +0x94 = 5 @0x8004f524 / +0x95 = 0 @0x8004f534 / f314 @0x8004f5c8. In ROOM1171 laeuft die EXEC nie (John bleibt bis zum
  ersten Plc_motion in +0x9 = 1); betrifft folgende NPCs (Ada 1090 u.a.). Nicht angefasst, um den Eingriff auf Johns
  Szene zu begrenzen.
- **Elza (Spieler) in derselben Szene** — nicht Johns Animation, eigener Code (Spieler-Walker Spur H Welle 2, Spieler-
  Schub Runde 36 J): Einzelbilder Clip 1 Bild 1 zwischen TURN und WALK (F1, F840, F846; Spur H hat genau das als
  "eigener RE-Punkt" vermerkt), Clip 1 nach jeder Ankunft ab Bild 1 statt 0 (F37, F591, F669, F943 = Event-Reach-Saattakt
  0x800517f0 @0x80051844-8c), Schub-Ruecklauf ab dem Bit-Takt (F933, wie K4). Die Spieler-Tabelle @0x80073e30 teilt sich
  [0..3] = 0x80050cb8.., [6] = 0x800517f0 und [19] = 0x80052864 mit der NPC-Tabelle — K3/K4 gelten dort ebenso, der Port
  fuehrt den Spieler aber ueber eigene Pfade (game_step_common.c, szenenleon_r36.c Spieler-Teil).

## 9. Fuer den Nutzer

- Neue Assets: keine (kein Paket-Gate-Eintrag). Sprachdateien: keine. Bedienung: unveraendert. Entscheidungsfragen: keine.
- Sichtbar in ROOM1171 (Elza, Dach):
  1. "Elza, hurry up!": John senkt die Waffe und winkt Elza heran (RBJ rec1 Clip 2, 96 Bilder) — kein Klettern mehr (K1).
  2. Die Gesten davor und bei "We need to get this thing off of here!" sind die Original-Gesten (RBJ rec1 Clip 0, 20 Bilder,
     vor und zurueck) statt eines fremden Bank-0-Clips (K1).
  3. Vor seiner ersten Geste steht John in der Original-Grundhaltung (aufrecht, Arme haengend) statt in der Leerlauf-
     Haltung (K2; sichtbar, sobald Kamera 4 auf ihn schaltet, Bild K_belege/k2_initpose_F208-224.png).
  4. Jede Geste beginnt mit ihrem ersten Bild — vorher sprang der Port ueber das erste Bild (K3).
  5. Beim gemeinsamen Schieben beginnt Johns Ruecklauf im Takt des Originals (K4).
- Dieselben Original-Regeln gelten fuer alle NPCs, weil das Original sie fuer alle gleich ausfuehrt (K2 in allen NPC-INITs,
  K3 im gemeinsamen Executor). Beispiel Leon ROOM1170: Leon bytegleich; Elliot zeigt beim Eintritt 6 Bilder die
  Grundhaltung (INIT @0x8011d978) und sein "Hey!" beginnt mit Bild 0.

## 10. Hinweise fuer die Integration (Orchestrator)

- Dateien gegen master: NEU engine/src/heliport1171_r39.c + include/re15_heliport1171_r39.h (GLOB -> `configure`!);
  gemeinsame Dateien nur mit "Runde 39 Spur K"-Haken: enemy_ai_common.c (Include, zwei Bank-Regel-Zeilen, INIT-Zeile +
  Merker, Merker loesen vor Eskorte/Executor, Rumpf von re15_npc_sub_motion), enemy_common.c (K1, 3 Zeilen),
  szenenleon_r36.c (K4, Phase 5/6 + Include), scd_room_setup.c (Include + Installer-Zeile am ENDE des Blocks — andere
  Welle-3-Spuren haengen dort ebenfalls an: beide Zeilen behalten, Reihenfolge egal, re15_r39k_install loescht nur die
  eigenen Merker); Riegel tests/unit/test_r37_roy.c (eine Bedingung, Abschnitt 7).
- Spur J (dining1141, gleiche Welle) fuehrt John (0x49) in ROOM1141: K2/K3 gelten dort automatisch (gemeinsamer NPC-Weg).
  Gemessen auf diesem Stand: integration_r38_dining1141_{w,z,d,l,s,r,a} gruen. Wer in 1141 eigene Gesten-Bildfolgen
  pinnt, sieht jede Plc_motion-Geste ab Bild 0 (vorher Bild 1).
- Spur A (Stimme 1171 msg 3) unberuehrt: integration_r39_stimmen_1171 gruen.

## 11. Abschluss — gezielte Tests auf dem Endstand (build/, Motor 6d027394, Riegel d2d97a88 + 696a282f (r37_roy) + Kopfkommentar der Integration im Schlusscommit)

Keine volle Suite (VERTRAG). Woertlich:
- Kern (eigene + 1171/Kraehen/RBJ/r38_heliport1171/r39_stimmen_1171):
  `ctest --test-dir re15_port/build -R "r39_heliport1171|r38_heliport1171|r39_stimmen_(tabelle|1171)|crow|kraehe|rbj" --timeout 900 -j 3 --output-on-failure`
  -> `100% tests passed, 0 tests failed out of 36`
- NPC-Familie Unit/Probe (jedes Modul/jeder Raum mit NPC-INIT, Plc_motion-Executor, Schub, Eskorte):
  `ctest --test-dir re15_port/build -R "npc|marvin|ada|elliot|irons|sherry|ben|annette|roy|john|1170|1171|10d0|11b0|1150|1050|1090|1141|szenenleon|werkstatt|schub|cine|cutscene|eintritt|plc|gest|rbj|neck|kopf|escort|eskorte|10f0|cut10f0|kendo|npckante|montage|elza|crow|kraehe" -E "^integration_" --timeout 600 -j 4 --output-on-failure`
  -> `100% tests passed, 0 tests failed out of 206`
- NPC-Familie Integration (echte exe):
  `ctest --test-dir re15_port/build -R "^integration_.*(npc|marvin|ada|elliot|irons|sherry|ben|annette|roy|john|1170|1171|10d0|11b0|1150|1050|1090|1141|szenenleon|werkstatt|schub|cine|cutscene|eintritt|plc|gest|rbj|neck|kopf|escort|eskorte|10f0|cut10f0|kendo|npckante|montage|elza|crow|kraehe)" --timeout 1200 -j 3 --output-on-failure`
  -> `100% tests passed, 0 tests failed out of 65`
- Verlauf: ein erster Lauf derselben Integrationsgruppe (63 ohne die eigenen) hatte 4 rote Eintraege — schrift1150 und
  irons_tisch_licht mit exit=-1 (die exe wurde in diesem Moment von meinem eigenen Test-Bau neu gelinkt, 17:36:10),
  elza_vollstart (Zeitablauf-Lauf unter Last, Szenenende=0; einzeln Szenenende=1) und r37_roy_leon1170 (fremder Riegel,
  Abschnitt 7 angepasst). Einzeln nachgefahren: `100% tests passed, 0 tests failed out of 6` (inkl. weg/laden).

Stand: alle drei "Auch sonst"-Abweichungen Johns (K2 INIT-Pose, K3 Saatbild, K4 Schub-Uebergang) und K1 (Heran-Winken
statt Klettern) sind gegen die Original-Instruktionen belegt, umgesetzt, nachgemessen (Abschnitte 4, 6.5, 6.6) und mit
Riegeln gesichert, die am alten Motor rot werden (Abschnitte 5, 6.7). Jede Zeile von Johns Pose-Strom in 1171 entspricht
jetzt der Original-Herleitung (Tabelle 6.5).

## 12. Nachbesserung 1 (nach Abnahme 0, 2026-10-18 ab ~19:40)

Maengel aus `K_abnahme_0.md` §6: M1 Elzas Event-Reach-Saatbild (Spieler-Pfad, K3-Klasse), M2 Elzas Schub-Ruecklauf im
Bit-Takt (Spieler-Pfad, K4-Klasse), M3 (a) Einzelbilder Clip 1 Bild 1 zwischen TURN-Ankunft und naechstem Gang,
(b) Kraehen-Animationen der Szene gegen das Original. Baum sauber, HEAD a2d1e1dc, exe 18:59 = Code-Stand 6d027394.
Je Mangel: Ursache / Messung vorher / Beleg / Aenderung / Messung nachher (fortlaufend unten).

### 12.1 M1 + M3(a) — Elzas Sub 6 (Event-Reach) nach Ankunft/Wurf/Schub, Einzelbilder zwischen Gang und Drehen

**Ursache (Port).** `re15_player_event_reach_begin` (game_step_common.c) setzte Clip 1 / Bild 0, und der Event-Reach-Takt
im SELBEN Bild schob sofort auf Bild 1 vor -> Bild 0 wurde nie gezeigt (15 statt 16 Bilder). Die Walker-Ankunft
(actor_locomotion.c) und der Schub-Ausgang (szenenleon_r36.c) riefen begin im Ankunfts-/Phase-7-Bild -> statt der
Gang-/Drehpose dieses Bildes stand dort schon Clip 1 Bild 1; folgte im naechsten Bild ein neuer Plc_dest, blieb genau
EIN Bild Clip 1 Bild 1 uebrig (M3a).

**Messung vorher** (eigener Lade-Weg `K39nb1/vorher_laden`, Stand 6d027394 = Abnahme-Stand; echter Weg = Abnahme
`K39abn0/runA_neu`, +66/67 Bilder): Elza (Aktor 0) Clip 1 je Bild 1..15 bei F37-F51, F186-F200, F591-F605, F669-F683,
F943-F957 (Lade-Weg); Einzelbilder `mo=1 Bild 1` bei F1, F840, F846 jeweils zwischen mo 105 (Gang/Drehen) und dem
naechsten Plc_dest; SCD-Spur (RE15_SCD_TRACE) zeigt: F1 Ankunft, F2 `@0x16A0 While` falsch -> `@0x16AC Evt_end` ->
`@0x13D4 Plc_dest` im selben Takt; F841 bzw. F847 ebenso (`@0x1558` / `@0x1566`+`@0x1572`).

**Beleg (selbst disassembliert, info/Re1.5/PSX.EXE md5 b55fdaa5):**
- Sub 6 0x800517f0 (`table 0x80073e30`: [6] = 0x800517f0): Phase 0 `80051844 sb v1(=1),6(a0)`, `80051854 sb v1,148(v0)`
  (+0x94 = 1), `80051864 sb zero,149(v0)` (+0x95 = 0), `80051874 sb v0(=7),143(v1)`, faellt OHNE Sprung in
  `80051878` (Phase-1-Rumpf): `80051884 lw a0,368` / `80051888 lw a1,372` / `8005188c jal 0x8001f314` -> posiert Bild 0;
  `800518a8 addu v1,v1,v0` / `800518b0 sb v1,6(a0)` (+0x6 += Rueckgabe = 1 im Takt, der Bild 15 posiert); Phase 2
  `800518b8 sb v0(=3),6(a0)`, `800518c8 sb v0(=2),148(v1)`, `800518d8 sb v0(=7),143(v1)` faellt in `800518f0 jal
  0x8001f314` (Clip 2 Bild 0 im selben Takt). => Clip 1 = 16 Takte (Bild 0..15), Clip 2 ab dem 17.
- Wer +0x5 = 6 setzt, und wann Sub 6 laeuft: Kommando 4 = 0x80030660 dispatcht +0x5 EINMAL je Bild
  (`8003068c lbu v0,0x800aca59` / `8003069c addiu at,at,15920` (0x80073e30) / `800306ac jalr v0`). Gang [4] 0x80030af0:
  f314 `80030c78` VOR der Ankunft (`80030cc4 slti v0,v0,100`, Bit-SET `80030cd8`, `80030cf0 sb v0(=6),0x800aca59`,
  `80030cfc sb zero` +0x6); Drehen [9] 0x80031360: `800313ec sb v0(=6),0x800aca59` / `800313f4 sb zero` +0x6, danach
  f314 `80031494`; Lauf [5] 0x80030d28 f314 `80030e4c`; rueckwaerts [8] 0x800311f0: f314 `800312b0` vor `80031318`.
  Im Ankunftsbild steht also die Gang-/Drehpose; Sub 6 Phase 0 laeuft fruehestens im naechsten Bild.
- Bildreihenfolge FUN_8001c6e8: `8001cdec jal 0x8003f038` (SCD; `8003f088 jal 0x8003f0a0` scd_execute_scripts) VOR
  `8001ce04 jal 0x8001a50c` (Gegner) VOR `8001ce0c jal 0x80031c44` (Spieler-Dispatcher). SCD-Opcodes (Tabelle
  @0x800744a8, gefunden ueber das Paar 0x3F->0x80041b90 / 0x40->0x80041be4 @0x800745a4): While 0x0F 0x8003f6f4 kehrt
  in BEIDEN Zweigen mit 1 zurueck (`8003f81c` wahr / `8003f840` falsch, PC = Schleifenende `8003f830-38`), Evt_end
  0x01 im Gosub-Rahmen mit 1 (`8003f22c`), Evt_next 0x02 mit 2 (`8003f26c`). Plc_dest 0x40 0x80041be4:
  `80041c14 sb v0(=4),4(a1)` / `80041c18 sb a2,5(a1)` (+0x5 = Modus) / `80041c1c sb zero,6(a1)`. => ist die
  Warteschleife `@0x169C` (Set / While Ck(5,bit)==0 {Evt_next}) im Folgebild erfuellt, ersetzt der naechste Plc_dest
  +0x5 noch vor dem Dispatcher: Sub 6 laeuft GAR NICHT, es gibt kein Clip-1-Bild (M3a: die drei Einzelbilder sind falsch,
  nicht "Bild 0 statt 1").
- Kraehen-Wurf: Rueck-FSM P4 `80115cf0 jal 0x8001f314`, bei Rueckgabe 1 `80115d04 sb v0(=4),0x800aca58`,
  `80115d10 sb v0(=6),0x800aca59`, `80115d18 sh zero,0x800aca5a` (STAGE1.BIN) — im Takt, der Wurf-Bild 29 posiert;
  Sub 6 Phase 0 im Folgebild. Der Port beendet P4 erst im Folgebild (`++s_bild >= 30`, heliport1171_r38.c) und ruft dort
  begin -> dort ist die Saat im SELBEN Bild richtig (Clip 1 Bild 0 im Bild nach Wurf-Bild 29).
- Schub Phase 7 0x80052bc0: `80052bcc jal 0x80045024` (SE), `80052be0` +0x5 = 6, `80052bf0` +0x6 = 0, kein anim_set ->
  Pose von Phase 6 (Clip 0x11 rueckwaerts Bild 0) bleibt stehen; Sub 6 im Folgebild.

**Aenderung** (alle Haken "Runde 39 Spur K NB1"):
- `heliport1171_r39.c` / `re15_heliport1171_r39.h`: Saat-Zustand `re15_r39k_reach_*` — 1 = Phase 0 im naechsten
  Event-Reach-Takt (posiert Bild 0 OHNE Vorschub, +0x8f = 7), 2 = Dispatcher-Bild (Sub 6 erst im naechsten Bild; bis
  dahin rendert die Gang-/Dreh-/Schub-Pose, `re15_player_event_reach_clip` = -1).
- `game_step_common.c`: begin (SCD-Modus 6 scd_vm.c:2651, Kraehen-Wurf) -> Saat 1; NEU
  `re15_player_event_reach_begin_naechstes_bild` -> Saat 2; end loescht die Saat; Takt ruft `re15_r39k_reach_takt` vor
  dem Vorschub.
- `actor_locomotion.c`: Spieler-Ankunft ruft `..._begin_naechstes_bild`; im Walker-Takt des Folgebildes (hinter dem
  Spieler-Pause-Gate @0x80031c78) `re15_r39k_reach_bildanfang` (2 -> 1). Die Saat selbst bleibt im Event-Reach-Takt =
  nach den Gegnern wie der Dispatcher.
- `player_common.c`: die pm-2-Leerlauf-Ruecksetzung (105/100 ohne Walker -> 200) laeuft im Dispatcher-Bild nicht
  (sonst stand dort Leerlauf statt Gang-Pose; gemessen nach dem ersten Umbau: F1 `mo=200`).
- `elza_r39.c` (Spur H, Walker-Init): zusaetzlich `motion_init_delay = 1` — Phase 0 von Gang/Lauf/Drehen faellt in f314
  (`80030bf8` / `80030e4c` / `80031494`) und posiert Bild 0 auch bei GLEICHEM Clip. Vorher verdeckt, weil zwischen zwei
  Plc_dest immer das (falsche) Sub-6-Bild den Clip wechselte; ohne es begann das Drehen F841 mit Bild 1 (gemessen).

**Messung nachher** (Lade-Weg `K39nb1/nach3_laden`, Stand cb17bb39; echter Weg `K39nb1/echt1`, gleicher Stand, Abschnitt
ROOM1171): Elza Clip 1 nach jeder Ankunft / nach dem Wurf **Bild 0..15 (16 Bilder)** ab dem Folgebild: Lade-Weg F38-F53,
F186-F201, F592-F607, F670-F685, F945-F960 (echter Weg F105, F252, F658, F736, F1011); Ankunftsbilder F37 / F591 / F669 /
F840 / F846 zeigen die Gang- bzw. Drehpose (mo 100/105, z.B. F840 Bild 4, F846 Bild 5); **keine** Einzelbilder mehr (F1,
F840, F846 / echter Weg F68, F906, F912 = Gang/Drehen). Drehen F841 beginnt mit Bild 0. Alle Nicht-Elza-Zeilen bytegleich
(Lade-Weg md5 acbd458d vorher = nachher; echter Weg Abschnitt 1171 md5 7f8fc006 = Abnahme-Lauf `runA_neu`); Elza aendert
sich nur in den erwarteten Bereichen (echter Weg F68, F104-F160, F252-F599, F657-F729, F735-F841, F906, F912, F999-F1142).
Hinweis: die Laenge von ROOM1141 im echten Weg schwankt auch bei GLEICHER exe (Abnahme `runA_neu` 4258 / `runA_neu_1` 4292
Bilder, v0.8.26 4243 / 4291) — Tonlaufzeit; nicht durch diese Aenderung.

### 12.2 M2 — Elzas Schub-Ruecklauf (Spieler-Sub 0x13, Phase 5 -> 6 -> 7)

**Ursache (Port).** `szenenleon_r36.c re15_szl_schub_spieler_tick` Fall 5 setzte im Bit-Takt sofort Clip 0x11 rueckwaerts
(`sp_clip(p, RE15_SZL_CLIP_ANSATZ, 1, 0)`), Phase 7 (`default`) loeschte das Rueckwaerts-Bit und rief begin (Sub 6 sofort).
**Messung vorher** (Lade-Weg): Bit-Takt F933 Elza Clip 17 rueckwaerts Bild 9 (John seit K4: Clip 18), Ruecklauf F933-F942,
F943 schon Clip 1 Bild 1; echter Weg F999-F1008 / F1009 (Abnahme 4.3).
**Beleg** (dieselbe Sub fuer Spieler und NPC: `table 0x80073e30` [19] = `table 0x80076ca0` [19] = 0x80052864): Phase 5
`80052af4 jal 0x8001f314` (Clip 0x12 posiert) VOR `80052b44 jal 0x8004efe4` (Bit-Abfrage), dann `80052b60` +0x6 = 6,
`80052b6c-70` +0x94 = 0x11, `80052b80` +0x95 = 0, Ausgang `j 0x80052bf4`; Phase 6 `80052b8c ori a2,zero,0x1` /
`80052b98 jal 0x8001f314` (rueckwaerts), `80052bb4` +0x6 += Rueckgabe; Phase 7 `80052bcc jal 0x80045024` (SE),
`80052be0` +0x5 = 6, `80052bf0` +0x6 = 0, kein anim_set.
**Aenderung** (`szenenleon_r36.c`, "Runde 39 Spur K NB1"): Bit-Takt setzt nur +0x6 = 6 und den Merker `s_sp_saat`; Phase 6
setzt Clip 0x11 rueckwaerts in ihrem ersten Takt; Phase 7 haelt die Pose (Bild fc-1 rueckwaerts = Bild 0, PL00-Bank
bleibt ueber den Port-Merker `s_sp = 8`), ruft `re15_player_event_reach_begin_naechstes_bild`; der Sub-6-Saattakt im
Folgebild beendet den Schub (`re15_szl_schub_spieler_ende`, Rueckwaerts-Bit weg).
**Messung nachher** (Lade-Weg): F933 Elza Clip 18 Bild 0 (= John), F934-F943 Clip 17 rueckwaerts 9..0 (= John), F944 Clip
17 Bild 0 gehalten (Phase 7, = John), F945 Clip 1 Bild 0 (= John); echter Weg F999 / F1000-F1009 / F1010 / F1011.

### 12.3 M3(b) — die Ereignis-Kraehen (Griff, Wurf, Tod) gegen das Original

**Messung vorher** (Lade-Weg `vorher_laden`, Aktoren 2/3, Typ 0x21 = Ereignis-Kraehe mit RE1.5-Hirn): jede Kraehen-Pose
ein Bild voraus: F0 Clip 0 Bild 1; HOVER Clip 4 1,1,2,3..; ORIENT (Griff) ab F96 Clip 8 **1..7,0** (die Doppelzeile F96
"Bild 0" ist die Effekt-Abfrage); Tod: Landebild F376 schon Clip 0x0a Bild 0, FINISH Bild 1..12, **Leiche Bild 12**.
**Beleg** (STAGE1.BIN / PSX.EXE): Clip-Setzer 0x80115d94 `80115da0 sb a0,148(v0)` (+0x94), `80115db0 sb zero,149(v0)`
(+0x95 = 0), `80115dc4 sb v0(=7),143(v1)` (+0x8f, Delay-Slot von `jr ra`) — posiert NICHT. f314 posiert +0x95
(`8001f35c lbu v0,149(t0)`, `8001f36c sw a2,360(t0)`) und schiebt erst danach vor (`8001f618-1c`, +0x8f-- `8001f5b0-b4`,
Umlauf `8001f63c`). Die Kraehen-Handler lesen +0x95 vor ihrem f314: ASCEND `80115078 lbu v1,149(a0)` vor `801150f0`, Flug
Sub 4 `80112b00` vor `80112b84`, Sub 5 `80112cac` vor `80112d1c`. ORIENT 0x801152e0: Clip 8 `@0x80115310`, Effekt
`@0x80115334`, Schritt 1 f314 `@0x8011536c`, Effekt am Umlauf `@0x80115394`. Tod Normal-Lane: Landung `801148e4 sw v1,56(a0)`,
Clip 0x0a `801148f4 jal 0x80115d94`, Se(5) `8011490c`, +0x1d5 = 11 `80114910`, +0x7++ `80114928-30`, KEIN f314; FINISH
`8011493c jal 0x8001f314` (+0x84/+0x16c), Timer `80114950-60`, Zustand 7 `80114978` + `80114974 jal 0x80115d74(0)`;
Zustand 7 0x801157e8 (scan bis `80115828 jr ra`) ohne f314. Effekt-Matrix: `a2 = [+0x188]+408` (Teil 2, Matrix +64) wird
von der Zeichenschleife gesetzt: `8001d108 jal 0x8001e8c8` je Entity NACH der Logik (`8001ce04` Gegner, `8001ce0c`
Spieler) — der Effekt sieht die Pose des VORIGEN Bildes. Der alte Port-Kommentar "Freeze-Frame = 12" (crow_death_pool.md
§1.2) zaehlte +0x95 nach dem 12. f314 (nie gezeichnet).
**Aenderung**: `heliport1171_r39.c` haelt +0x94/+0x95/+0x8f je Aktor-Slot (`re15_r39k_kraehe_clip/_anim/_bild`), die
Aktorfelder tragen das zuletzt POSIERTE Bild (= Renderer); fremde Schreiber (RE2-Kraehenhirn, Zwang ROOM1120) werden
erkannt (Aktorfelder != zuletzt posiert -> alte Form). `enemy_ai_common.c` (Haken "Runde 39 Spur K NB1"):
`re15_crow_clip`/`re15_crow_anim` gehen darueber, die drei Bild-8-Abfragen lesen +0x95, der ORIENT-Umlauf-Effekt nimmt
die Pose vor dem f314 dieses Takts. Installer loescht die Tabelle beim Raumaufbau.
**Messung nachher** (Lade-Weg `K39nb1/kraehe1_laden`, build_bisect = Endstand): F0 Clip 0 Bild 0; Start HOVER 0,0,1,2..;
ORIENT F96 (Kraehe 2) / F107 (Kraehe 3) Clip 8 **0..7,0..**; Effekt-Abfrage am Eintritt mit der HOVER-Pose (Clip 4 Bild 3);
Tod: Landebild F376 / F425 haelt Clip 4 (Fall-Pose), FINISH Clip 0x0a **0..11**, **Leiche Bild 11**. Hirn-Zeitachse
unveraendert: state.log ohne mo/af 1138/1138 Zeilen gleich; Elza/John bytegleich (md5 787b922b vorher = nachher).
Fremde Riegel angepasst (mit Beleg): `probe_crow_death.c` (unit_crow_death_corpse) Frame 12 -> 11;
`test_r38_heliport1171.c` flagwort: Effekt-Pose = Kraehen-Aktorfelder VOR dem ORIENT-Bild (vorher Clip 8 Bild 0).
- (NB1, laufend) Gegenprobe der neuen Riegel: engine/src/{game_step_common,actor_locomotion,player_common,elza_r39,
  szenenleon_r36,enemy_ai_common,heliport1171_r39}.c und include/re15_heliport1171_r39.h stehen VORUEBERGEHEND auf a2d1e1dc
  (Abnahme-Stand) in der Arbeitskopie, Bau nur des Ziels test_r39_heliport1171 in re15_port/build_bisect. Steht beim Lesen
  ein ' M' auf diesen Dateien ohne Rueckmeldung hier: `git checkout HEAD -- <diese Dateien>`, NICHT committen.
- (NB1) Gegenprobe erledigt, Arbeitskopie mit `git checkout HEAD -- ...` zurueckgesetzt, `git status --porcelain` leer.
  Neue Riegel gegen den ALTEN Motor (a2d1e1dc-Engine, neue Testquellen, nur Ziel test_r39_heliport1171 in build_bisect):
  `elza` FEHLER (4): "8 Laeufe: Start 0, 16 Bilder 0" (Laeufe ab Bild 1 mit 15 Bildern, Einzelbilder F1/F840/F846),
  "kein einzelnes Sub-6-Bild (3)", "Ankunftsbilder 0 von 6", "M2 ... Folge FALSCH"; `kraehen` FEHLER (4): Start Clip 0
  Bild 1, ORIENT-Folge falsch, Landebild schon Clip 10, Leiche Bild 12. Integration (Trockenlauf des neuen Blocks gegen
  die Spuren): Abnahme-Stand ROT, Stand ohne Kraehen-Fix ROT (M3b), Endstand GRUEN.

### 12.4 Tests der Nachbesserung (Riegel)

Registriert nur in `re15_port/tests/unit/probes/r39_heliport1171.cmake` (keine CMakeLists.txt angefasst):
- NEU `unit_r39_heliport1171_elza` (M1/M2/M3a): ganze Szene mit Elzas echten Baenken (W-Bank PL04W01-EDD wie die
  Start-Bank in main.c, COMMON-Paar aus PL04.PLD per re15_pld_part): je Sub-6-Lauf Clip 1 Bild 0 / Blende 7 am Anfang,
  16 Bilder 0..15, dann Clip 2 Bild 0 (5 Laeufe: Bild 38/186/592/670/945); kein einzelnes Sub-6-Bild; Ankunftsbilder
  (6) zeigen Gang/Drehen; Schub: Bit-Takt Clip 0x12, Phase 6 Clip 0x11 rueckwaerts 9..0, Phase 7 haelt Bild 0, danach
  Sub 6 Bild 0 — Bit-Takt im selben Bild wie John (933).
- NEU `unit_r39_heliport1171_kraehen` (M3b): INIT Clip 0 Bild 0, ORIENT Clip 8 ab Bild 0 (0..7,0,1), Landebild = Fall-Pose
  (Clip 4), FINISH Clip 0x0a Bild 0..11, Leiche haelt Bild 11 (beide Kraehen).
- `integration_r39_heliport1171_weg/_laden` zusaetzlich (Block am Skriptende): Elzas Clip-1-Laeufe je 0..15 (16 Bilder,
  >= 5 Laeufe, keine kurzen), Elzas Clip 17 rueckwaerts im Bild Johns, je Kraehe ORIENT ab Bild 0 und Leiche Clip 0x0a
  Bild 11 (gerendert = letzte Zeile je Bild/Aktor). Kletterzaehler zaehlt jetzt auch Loco 2/50 mit Bild > 0 vor dem ersten
  RBJ-Clip-0-Bild (Hinweis H4).
- GEGENPROBE (alter Motor a2d1e1dc, neue Testquellen): `elza` 4 Fehler, `kraehen` 4 Fehler (Abschnitt-Notiz oben);
  Integrationsblock trocken gegen die aufgezeichneten Spuren: Abnahme-Stand ROT, ohne Kraehen-Fix ROT, Endstand GRUEN.
- Fremde Riegel angepasst (je mit Beleg, Abschnitte 12.1/12.3): `test_plc_back_yaw_1090.c` (Ankunftsbild posiert noch
  Modus 8, f314 @0x800312b0 vor +0x5 = 6 @0x80031318; der Riegel faehrt den Sub-6-Takt re15_r39k_reach_takt mit),
  `probe_crow_death.c` (Leiche Frame 12 -> 11), `test_r38_heliport1171.c` flagwort (ORIENT-Effekt mit der Pose VOR dem
  ORIENT-Bild), `test_r37_roy.c` spur (Hinweis H3: INIT-Form nur als Lauf ab der ersten Elliot-Zeile).

### 12.5 Wirkung ausserhalb von ROOM1171 (gleiche Original-Funktionen)

- M1/M3a gelten fuer JEDE Plc_dest-Ankunft des SPIELERS (Leon und Elza, alle Szenen): Ankunftsbild = Gang-/Dreh-Pose,
  Sub 6 Clip 1 ab dem Folgebild Bild 0..15; folgt ein Plc_dest direkt, kein Sub-6-Bild. Die Spieler-Tabelle @0x80073e30
  ist fuer beide Figuren dieselbe. Beispiel ROOM1141 (echter Weg): F1305-Einzelbild weg, Drehen beginnt mit Bild 0.
- M2 gilt fuer jeden Spieler-Schub (Sub 0x13): ROOM11B0 (Leon/Marvin, Runde 36 J) und ROOM1171.
- M3b gilt fuer jede Kraehe mit RE1.5-Hirn: die Ereignis-Kraehen ROOM1171 in jedem KI-Modus, im RE1.5-KI-Modus alle
  Kraehen (10C0/1120/1170). Im Standard-KI-Modus laufen die uebrigen Kraehen ueber das RE2-Hirn (eigener Code, unberuehrt).

### 12.6 Messung nachher am Endstand, echter Weg (1131 -> 1141 -> Dachtuer -> 1171 -> Tor Slot 5 -> Szene -> 10B1)

Lauf `K39nb1/echt2` (Befehl wie Abnahme `K39abn0/lauf.sh` mit `RE15_FIRE_AOT=5@60#1171`, exe = build_bisect am Stand
0b133e27/f6a0b815-Motor, EXIT=0, ROOM10B1 erreicht), Abschnitt ROOM1171:
- Elza: Drehen F67-F68 Bild 0,1 -> Gang F69 ab Bild 0 (kein Sub-6-Bild); Clip 1 Bild 0..15 bei F105, F252 (nach Wurf-Bild
  F251), F658, F736, F1011; Ankunftsbilder F104/F657/F735 Gang, F906 / F912 Drehen; Schub F999 Clip 18 / F1000-F1009 Clip
  17 rueckwaerts 9..0 / F1010 Bild 0 gehalten / F1011 Clip 1 Bild 0 — John F1000 / F1010 / F1011 gleich.
- Kraehen: F61-F66 Clip 0 Bild 0 (vorher 1); ORIENT F162 (Kraehe 2) / F173 (Kraehe 3) Clip 8 ab Bild 0; Landebild F442 /
  F491 haelt Clip 4 (vorher Clip 0x0a Bild 0); FINISH Clip 0x0a 0..11; Leiche Bild 11 (vorher 12). Gerenderter Clip je Bild
  sonst identisch mit dem Abnahme-Lauf (1081 von 1082 Bildern je Kraehe, die Ausnahme ist genau das Landebild).
- John: alle 1082 Zeilen bytegleich mit dem Abnahme-Lauf `runA_neu`.

### 12.7 OFFEN nach Nachbesserung 1 (mit Adressen, nicht Teil der Maengel M1-M3)

- **Ton der Opfer-Phasen P1/P2 einen Takt spaet** (heliport1171_r38.c, Runde 38 I): Original spielt Se_on 0x03020001 im
  Takt, dessen f314 das letzte Bild posiert (P1 f314 @0x80115c20, `beq v0,zero` @0x80115c28, SE @0x80115c3c; P2 f314
  @0x80115c88, SE @0x80115ca4); der Port erkennt den Umlauf erst im Folgetakt (`++s_bild >= Laenge`) — die Animation ist
  davon unberuehrt (Clip 1 Bild 0 im selben Bild wie das Original), nur der Ton liegt 1 Bild (33 ms) spaet. Der Riegel
  test_r38_heliport1171 `szene_*` pinnt die Port-Lage (SE im ersten Clip-1-Bild).
- **Hund 0x20** (re15_dog_clip/re15_dog_anim, enemy_ai_common.c) zeichnet wie vorher die Kraehe das Bild NACH dem Vorschub
  (gleiche Klasse wie M3b); nicht in ROOM1171, nicht angefasst.
- **Eskorte-EXEC-Saattakt** (Abschnitt 8, unveraendert offen; NPC, nicht in ROOM1171).
- **H5** (RBJ-Cache-Grenze fuer Slots MIT Record, ROOM1040/1041) und **H6** (build_bisect + liegengebliebene Messkopie
  re15_pc_r35e_haken_173bda57.exe im build/, ignorierte Dateien) bei der Integration.

### 12.8 Fuer den Nutzer (Nachbesserung 1)

- Neue Assets: keine. Sprachdateien: keine. Bedienung: unveraendert. Entscheidungsfragen: keine.
- Sichtbar in ROOM1171 (und ueberall, wo dieselben Original-Ablaeufe laufen):
  1. Elza: nach jedem Ankommen (Gang, Drehen) und nach dem Kraehen-Wurf beginnt ihre Halte-Geste mit dem ersten Bild und
     im Bild NACH der Ankunft; im Ankunftsbild selbst steht noch der letzte Schritt. Folgt sofort der naechste Gang, gibt es
     kein eingeschobenes Halte-Bild mehr (vorher je ein Bild "Arm-Ansatz" zwischen Drehen und Gehen).
  2. Elza und John lassen den Container beim Loslassen im selben Bild los (vorher Elza ein Bild frueher).
  3. Die Kraehen zeigen jedes Bild ihrer Animationen ab dem ersten (vorher immer eins voraus); beim Aufschlag bleibt im
     Landebild die Fallhaltung, die tote Kraehe liegt im Original-Endbild (11 statt 12 des Lande-Clips).
- Leon ist mitbetroffen, wo er per Skript laeuft (gleiche Spieler-Tabelle im Original): Ankunft/Halte-Geste wie Punkt 1
  (z.B. ROOM1090 Rueckwaerts-Gang, Riegel unit_plc_back_yaw_1090); Leon ROOM1170 (Riegel-Weg) bytegleich.

### 12.9 Hinweise fuer die Integration (Nachbesserung 1)

- Neue Haken (je "Runde 39 Spur K NB1", 1-6 Zeilen): game_step_common.c (Include, begin/_naechstes_bild/end/clip/Takt),
  actor_locomotion.c (Ankunft -> _naechstes_bild, Bildanfang im Walker-Takt), player_common.c (pm-2-Leerlauf nicht im
  Dispatcher-Bild), elza_r39.c (Spur H: Walker-Init motion_init_delay = 1), szenenleon_r36.c (Spieler-Schub Ph 5/6/7),
  enemy_ai_common.c (Kraehen-Clip/Anim/Bild-8/ORIENT-Effekt). Logik in heliport1171_r39.c (+ Header). Keine neue .c-Datei.
- Fremde Riegel geaendert: test_plc_back_yaw_1090.c, probe_crow_death.c, test_r38_heliport1171.c, test_r37_roy.c (12.4).
- Spur J (ROOM1141) sieht Elzas/Johns Ankunfts-Bilder geaendert (Einzelbild F1305 weg, Drehen ab Bild 0); wer dort
  Spieler-Posen pinnt, rechnet mit Sub 6 ab dem Folgebild der Ankunft.
