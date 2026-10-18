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
