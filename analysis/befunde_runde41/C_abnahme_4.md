# Runde 41 — Spur C "ventilkette" — Abnahme 4 (unabhaengig)

Baum `.claude/worktrees/r41_ventilkette`, Zweig `r41/ventilkette`, gepruefter Stand HEAD b08bb0c4 (Basis = merge-base master
116e8b0f; master selbst steht auf 200184bc und enthaelt fremde Spuren — Gegenstand ist nur der Spur-Diff `git diff master...HEAD`).
Massstab: die sechs Punkte des Auftragstexts und AUFTRAG.md (massgeblich) mit den Nachtraegen 6-13 und den Entscheiden C-a..e,
C-E1..C-E3, C-E2b (C-E2c widerrufen durch Nachtrag 6). Vorlage: Abnahme 3 (`C_abnahme_3.md`, Maengel 1-4), Nachbesserung 4
(Dossier §0e, Commits a5830413..b08bb0c4).
Begonnen 2026-10-24 ~18:4x. Pruefer: Abnahme 4 (Opus). Der Bericht wird FORTLAUFEND geschrieben. Messordner scratchpad
`C41abn4/` (unten `<a4>` =
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/C41abn4`).

**Ergebnis: (in Arbeit)**

## 0. Stand / Protokoll (neueste zuerst)
* 18:5x Bau: `local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)` (Zeile 180 `integration_r41_ventilkette_kette
  uebersprungen: Spur A fehlt`), `local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)` (`<a4>/configure.txt`,
  `<a4>/build.txt`); Unit-Gruppe 241/241 (§3); Integrationsgruppen im Hintergrund (`<a4>/integ.sh`); K1 gemessen (§1).
* 18:4x Start: `git status` leer, HEAD b08bb0c4; Spur-Diff (`git diff master...HEAD --name-only`, ohne analysis/) = 32 Pfade:
  engine/src/ventilkette_r41.c (+839), include/re15_ventilkette_r41.h (+454), gen/ventilkette_r41_{original,plakat,programme,
  texte,wasser}.inc, tools/r41_ventilkette/*.py (5), Tests (unit 1 + probes 1 + integration 5 eigene), Haken in
  game_step_common.c (+1), re15_files.c (+10), scd_room_setup.c (+2), scd_vm.c (+4), platform/pc/main.c (+16),
  test_r36_kartenbits.c (+7), test_r36_dachkiste.cmake (+2), test_r37_itemband.cmake (+4), neue Assets
  shared_assets/RE2/FILES/FILE31_{title_page,p00_page,p01_page,p02_page,title_paper}.TIM. Last auf der Maschine: fremde
  Abnahme-/Bau-exe laufen (h41abn1, r41b, E41n4b, r37d) -> Fenster-Riegel ggf. einzeln nachfahren.

## 1. Messungen an der exe (eigene Laeufe)
Werkzeug: `<a4>/lauf.sh <name> <timeout> KEY=VAL...` (eigene exe-Kopie `re15_pc_abn4c_<name>.exe` neben der exe, erbt keine
RE15_-Variablen, Laufordner `<a4>/mess/<name>`, Kopie danach geloescht), Wegplaene `<a4>/plaene/*.txt` (Treiber RE15_WEGPLAN =
echte Pad-Bits, dachkiste_r36.c). Beschleunigter Renderer (kein RE15_SOFTWARE_RENDER), RE15_WINDOW_SCALE=1, RE15_FRAMEDUMP.
Leon = Titel-Haken, Elza = RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 (`[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`).
Spinnen 2030 als getoetet (RE15_SET_FLAG 7:155..158, wie die Riegel). exe = `re15_pc.exe` 16:59 (7889599 B) = Code HEAD
(nach 56b0eaa8 nur Doku; `local_build.sh build` -> `ninja: no work to do.`).

### K1 — Punkt 2 + Nachtraege 9/10/13 Teil 1: echte Tuer 2070 -> 2030, Bruecke, Ventil 13, Steg, Ventil ohne Sicherung, Sicherung (Leon `k1`, Elza `k1_elza`)
Start ROOM2070/2071 (4400,-6400) vor Tuer Slot 1, Wegplan `<a4>/plaene/k1.txt`. Hinweis: der Wegplan-Treiber nimmt hoechstens 96 Befehle
(`#define WP_MAX 96`, dachkiste_r36.c:165); mein Plan hatte 192 -> nach der Sicherung (Befehl 95) blieb der Treiber stehen, die zwei
Laeufe habe ich per PID beendet (eigene Kopien). Die zweite Haelfte faehrt K2 ab genau diesem Zustand.
Gemessen (debug.log / wegplan.txt, Leon; Elza Zeile fuer Zeile gleich mit `room=2031`):
* `[aot] DOOR FIRE slot=1 rect=(5500,-6400,hw=500,hh=1500) target_cut=0 spawn=(6550,-5400,-100)` -> `loaded room2030.rdt` (Elza
  `room2031.rdt`) -> `[ventilkette41] ROOM2030: Bruecke obj 0 oben y -5400, Zelle 12 gesperrt` + `Ventile 01/02/03/07/08/09 (Slots
  14-19), Plakat Slot 21, Dokument Slot 20 (Item 0x4E)`; Startbits `FLAG (9,101) = 0`, `(9,102) = 0`, `(3,108) = 0`, `(9,103) = 0`,
  `(9,107) = 0`; `ZELLE r0[12] ... u0 0xff` (Steg gesperrt, F69 und am Treppenfuss F1269).
* Pult (Ring Nordwand, Cut 2): `Ereignis 24 -> Port-Programm 2030_pult_ohne` -> `[msgtext] room=2030 id=40 "A control panel for the
  bridge. The pressure gauge reads zero.Intake valve 13 is closed."`.
* Treppe hinab, Nordventil 01 VOR der Bruecke: `Ereignis 17 -> Port-Programm 2030_ventile_vorher` -> `id=49 "I should look around here
  first."` (Nachtrag 13 Folge 1: neutral).
* Ventil 13 (Tank): `Ereignis 25 -> 2030_v13_frage` -> `id=41 "Intake valve 13. Will you open it?"` -> Ja -> `id=42 "You've opened
  valve 13. The pressure is rising."` -> `FLAG (9,101) = 1` (F1974).
* Treppe hinauf, Pult: `2030_pult_senken` -> `id=43 "The pressure gauge reads 80. Lower the bridge?"` -> Ja -> `[scd F2713] Cut_chg(4)`
  -> `[prop-member F2773] prop[0] obj=0x00 m=1 -> val=-1800` -> `FLAG (9,102) = 1` (F3047); erneute Aktion `2030_pult_unten` -> `id=44
  "The bridge is down."`.
* Treppe hinab: `ZELLE r0[12] ... u0 0x00` (F3745), ueber den Steg (Cut 4, `pl pos=(-1061,-1800,-3711)` .. `(-1001,-1800,729)`) zum
  Sued-Landeplatz; Ventil 09 OHNE Sicherung: `Ereignis 22 -> 2030_ventile_vorher` -> `id=49`; Sicherung (Original Slot 9): `id=2 "Will
  you take the Fuse?"` -> Ja -> `id=3 "You've taken the Fuse."`.
* Bildbogen `<a4>/bilder/k1_teil1.png` (F280..F4450): Pult an der Nordwand mit Text, Nordventile 01/02 mit Satz 49, Frage "Lower the
  bridge? Yes/No", Cut 4 Bruecke in der Fahrt (F2740/F2770), Spieler auf dem Steg (F3820), Ventil 09 Satz 49 (F4090), Sicherungsfrage.


## 2. Gates

### G1 RE-Gate (eigene Disassembly / eigene Byte-Lesung)
Stichproben der in Nachbesserung 4 NEU zitierten Stellen (alle selbst gelesen, richtige Binaerdatei):
1. **@0x8002d370** (Kletter-Sonde FUN_8002d2c0, Begruendung von RE15_R41C_BLATT_BAND 2) — `re15_disasm.py dis 0x8002d2c0 64`
   (info/Re1.5/PSX.EXE): Schleife ueber die Objekte `0x800b3f98 + i*148` (`addiu s2,s2,-148` @0x8002d360), aktiv `andi v0,v0,0x101`
   @0x8002d358, `lbu v1,0(s3)` @0x8002d364 mit s3 = 0x800aca3c + 154 = **0x800acad6**, `lbu v0,130(s0)` @0x8002d368 = Objekt+0x82,
   **`bne v1,v0,0x8002d3a4` @0x8002d370**, sonst `jal 0x8002d1e8` / `jal 0x8002da4c`. Gegenprobe **@0x80031cfc** `lbu v0,130(a0)`,
   @0x80031d04 `lbu v1,-13610(v1)` = 0x800acad6 (vom Disassembler als `player.floor(+0x82)` benannt), @0x80031d08 `addiu v0,v0,1`,
   @0x80031d0c `bne v0,v1` -> "Objekt-Band == Spielerband" (Sonde) und "Objekt-Band + 1 == Spielerband" (Standtest) stimmen; Band 2
   liegt am Landeplatz (Spielerband 1) in keinem der beiden Faelle. **Bestaetigt.**
2. **@0x80028698** (Schliessen der Plakat-Nahaufnahme) — `dis 0x80028680 12`: `lw v0,-14484(v0)` = 0x800ac76c, **`andi v0,v0,0xc000`
   @0x80028698**, `beq v0,zero,0x80028748`. **Bestaetigt.**
3. **RE2 ROOM2130** (info/re2leon/PL0/RDT/ROOM2130.RDT, Datei-Offsets, selbst gelesen): @0x01752 `23 00 05 00 50 00` (cmp var5 == 80),
   @0x017D6 `23 00 05 00 00 00 2b 00`, @0x017E8 `23 00 05 01 50 00 2b 00`, **@0x01800 `36 02 0b 01`** (Fehlerton Satz 0x0B), @0x01138
   `24 05 00 00`, @0x01192 `36 02 0a 01` (Klick), @0x01762 `36 02 0c 01`, @0x0171C `09 0a 1e 00`. **Bestaetigt.** RE2 **@0x80071d04**
   `addiu a0,a3,-104` (`re2_disasm.py dis 0x80071cf0`, info/re2leon/PSX.EXE). **Bestaetigt.**
4. **RE1.5 ROOM11F0 sub17** (STAGE1/ROOM11F0.RDT): @0x0168C..@0x016BC `22 05 00 00`..`22 05 0c 00`, **@0x016C0..@0x016E4 `22 05 0d 00`..
   `22 05 16 00`**, @0x016E8 `22 02 00 00`, @0x016EC `22 02 02 00`, @0x016F0 `2a 00 3c 01`; @0x01602 `32 00 9e b3 00 00 9c 58`, @0x0160E
   `33 00 00 00 00 00 00 02`. **Bestaetigt.** ROOM1051 @0x00DA4 `22 02 07 01` / @0x00DD2 `22 02 07 00` **bestaetigt**; ROOM2080
   @0x00CC4 `2e 03 00 00` **@0x00CC8 `2f 01 32 00`** @0x00CCC `0d 00 06 00 94 00` **bestaetigt**.
5. **ROOM2030 SCA** (STAGE2/ROOM2030.RDT, 12 B je Zelle w,d,x,z,typ,u0,u1,fl) — Absperrung: 19 @0x0097C `f4 01 f2 44 18 15 d8 dc 01 ff 00 33`
   (x 5400..5900 z -9000..8650), 20 @0x00988 (x -5900..5900 z -9000..-8500), 21 @0x00994 (x -6350..5900 z 8150..8650), 22 @0x009A0
   (x -6350..-5850 z 2450..8650), 31 @0x00A0C (x -8150..-5400 z -6900..-6400), 32 @0x00A18 (x -5900..-5400 z -9000..-6400), alle typ 1
   fl 0x33; Ringboden 36..41 @0x00A48..@0x00A84 u1 01 fl 0x23 (z. B. 36 `08 07 7a 0d ec e1 92 d7 01 ff 01 23`); Wandzellen 6 @0x008E0
   (z -9000..-8500) / 7 @0x008EC (z 7900..8400) fl 0x11 = Wandflaechen der Ventile. Die drei Wasser-Rechtecke (x -5400..5400 z -8500..
   -6400; x -7700..5400 z -6400..2450; x -5850..5400 z 2450..8150) liegen innerhalb der Absperrung und ueberdecken keine Ringboden-
   Zelle (selbst nachgerechnet). **Bestaetigt.** Sicherung Slot 9 @0x01BAA `2c 09 03 31 01 00 f4 e8 fa 19 20 03 e8 03 ...`
   = x -5900..-5100 z 6650..7650; @0x01FF2 `22 03 6c 01`; Bruecke @0x01B6C `2d 00 00 00 00 00 0a 00 ...`; Treppe @0x01B8E `2c 08 0c
   31 80 00 ...` **bestaetigt**.
6. **"2030 benutzt Bank 5 nicht"** (Begruendung der Raum-Scratch-Bits 0x10..0x13/0x1B/0x1C): `scd_dump_room.py ROOM2030.RDT` (main
   0x1AC8..0x1CBC, sub 0x1CBC..0x3DD8, 7 Regionen) -> **0 Zeilen `bank=5`** (`<a4>/scd_2030.txt`). **Bestaetigt.**
7. Dokument-Basis 0x48 (u8 @0x800c7370): liegt hinter dem PSX.EXE-Bild (t_addr 0x80010000 + t_size 0xaf000 = 0x800bf000; laut
   re15_files.h DEBUG.BIN) — kein neuer Beleg dieser Spur (vorhandene Konstante RE15_FILES_FIRST_ITEM_ID), nicht nachgeprueft.
* Konstanten-Durchsicht (Kopf re15_ventilkette_r41.h, alle NB4-Zeilen): jede traegt @0x.../Datei-Offset oder ist als PORT-WAHL bzw.
  NUTZER-VORGABE benannt (Ventil-x = Rueckprojektion BSS + "auf 50 gerundet", Zuordnung Rad -> Nummer PORT-WAHL, Loesung PORT-WAHL zur
  Freigabe, Pegel-Abstand 50 PORT-WAHL mit Mass @0x00CC8, Blatt-Band PORT-WAHL mit @0x8002d370/@0x80031cfc, Plakat-/Dokument-Lage
  PORT-WAHL mit Bezug Slot 9 @0x01BAA bzw. Wand z 7900). Ids/Slots/Ereignisse = Vertrags-Zuteilungen.
* Rate-Woerter: `git diff master...HEAD -- re15_port` (ohne die zwei Daten-inc) nach deferred|tunable|interim|for now|faithful|plausib|
  TODO|FIXME -> 0 Treffer (2 Fehltreffer "ROOMxxx1"). Kein Env-Schalter als Abschluss (die neuen getenv sind keine; Plakat/Dokument
  laufen ohne Haken).


## 3. Tests (gezielt, im Spurbaum; KEINE volle Suite)
(folgt)

## 4. Urteil je Nutzer-Punkt
(folgt)

## 5. Maengel
(folgt)
