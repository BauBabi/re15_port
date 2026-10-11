# Runde 36 — Spur G "tueren": Unabhaengige Abnahme 0

Pruefer: Abnahme-Agent 0 (2026-10-11). Baum `.claude/worktrees/r36_tueren`, Zweig `r36/tueren`, HEAD 10754312,
Basis master 78a4a748 (merge-base). Massstab: Wortlaut AUFTRAG.md (die drei Tuer-Punkte), VERTRAG.md.
Geglaubt wird nur, was hier selbst am gebauten Stand gemessen ist.

Stand dieses Berichts: ABGESCHLOSSEN (zweite Sitzung 2026-10-11, ~12:30) - bestanden, alle drei Punkte erfuellt, Gates halten; Hinweise H1-H7 am Ende.

Messordner (ausserhalb des Baums, Scratchpad dieser Sitzung):
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/G_abn0/`
(im Folgenden `<M>`). Messkopie der exe: `re15_port/build/platform/pc/re15_pc_g36abn0.exe` (Kopie von re15_pc.exe,
HEAD-Stand), Laeufer `<M>/lauf.sh` (RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2).

## 0. Diff-Lage und Bau

* `git diff master --stat` zeigt 96 Dateien, davon 63 geloeschte `synchro/STAGE1/...wav` und 15 Zeilen AUFTRAG.md —
  das ist NICHT die Spur, sondern master, der nach der Basis weiterlief (565da62b Sprachdateien, 836023f5 Nachtrag J).
  Massgeblich ist `git diff master...HEAD` (33 Dateien, +2740/-48). Code-Diff vollstaendig gelesen
  (aufschliessen_r36.c, re15_aufschliessen_r36.h, gen/r36_aufschliessen.inc, gen/re2_karten_bank.inc,
  gen/re15_tuer_eigen.inc, scd_vm.c, game_step_common.c, main.c, audio_pc.c, tuer_r36_pc.c, Tests).
* Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`;
  `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`.
  Gegenprobe Aktualitaet: `aufschliessen_r36.c` mtime 03:36:49 < `aufschliessen_r36.c.obj` 03:37:07 < `re15_pc.exe`
  03:37:08; Arbeitsbaum = HEAD (`git status --short re15_port` leer). Die exe ist der HEAD-Stand.
* Gezielte Tests (keine volle Suite), Regex = Spur + Gruppen der beruehrten Module (Tueren, Fahrstuhl, Schloss-,
  Nachrichten-, AOT-, Audio-/SE-Wege, Tastenfeld, Raumwechsel, 1140/10C0-Raeume, Karten-Uebergang, Flag-Zensus),
  81 Tests (`<M>/ctest_regex.txt`, Protokoll `<M>/ctest_gezielt.log`):
  `100% tests passed, 0 tests failed out of 81` (Total 286 s, -j 3, unter Last durch Parallel-Sitzungen).
  Darin `unit_r36_tueren_{fahrstuhl,schott,aufschliessen,textur}`, `integration_r36_tueren` (53,9 s),
  `integration_keypad`, `integration_keydoor`, `integration_door_lock`, `integration_door_traversal`,
  `integration_r35_karte_fahrstuhl` (187,8 s), `integration_r35_cut10f0` (258,2 s), `unit_r33_tueren_*`.
* Sonde direkt gefahren (`probe_r36_tueren.exe <teil>`), Ausgaben echt (keine stillen Abbrueche): fahrstuhl
  "DOOR27 V0 (271 Bilder): Fluegel obj 0 z-Weg 2240 (Richtung -2240), obj 1 z-Weg 2240 (Richtung +2240)",
  "ROOM10C0 Slot 1 ... -> ROOM1080, Laeufer 1 x: DOOR27 V0 eigen 0 S040"; schott "DOOR26 V0: 2 Fluegel-Objekte,
  V3 (vorher): 1", "ROOM2090 Slot 1 ... -> ROOM20A0 ... DOOR26 V0 eigen 0 S164".
  Gegenprobe Textur-Riegel: `probe_r36_tueren textur <master-P0CD>` (git show 78a4a748:...P0CD.DO2,
  sha1 a0f8bb19) -> `textur: ROT (4 Fehler)` (fein 0.0814, form 0.1824, hell 114.5, gy/gx 0.44) — der Riegel trennt.

---

## Punkt 1 — "Einige Türen in den Türsequenzen sind noch nicht ganz korrekt. So zum Beispiel der Elevator ROOM 1080 und die große Tür in ROOM 2090. Ich glaube beide liegen auch korrekt in Resident Evil 2 vor."

### RE2-Belege selbst gelesen (info/re2leon/PL0/RDT, Door_aot_set 0x3B, Byte 26 = Archiv, Byte 27 = Variante)

```
ROOM6010.RDT @0x004C2: 3b 00 01 31 00 00 7a ae 5e 9d 08 07 3c 0f .. 05 02 04 00 27 00 ..  -> ROOM6020 DOOR27 V0 (w 1800 d 3900)
ROOM6010.RDT @0x004E2: 3b 01 ..                                    05 06 06 00 27 00 ..  -> ROOM6060 DOOR27 V0
ROOM6010.RDT @0x00502: 3b 02 ..                                    06 00 05 00 27 00 ..  -> ROOM7000 DOOR27 V0
ROOM6020.RDT @0x00872 -> ROOM6010 DOOR27 V0 ; ROOM6060 @0x00E7C -> ROOM6010 DOOR27 V0 ; ROOM6070 @0x00B5E (w 1 d 1) -> 6010 DOOR27 V0
ROOM6020.RDT @0x008AC: ... 05 03 00 03 26 00 ..  -> ROOM6030 DOOR26 V0 (w 5900) ; ROOM6030 @0x029CA -> 6020 DOOR26 V0 (w 3200)
```
Zensus aller RE2-Door_aot_set mit DOOR25/26/27/29/10/2A (eigenes Python-Scan ueber PL0/RDT): DOOR26 V0 in 6020/6030/
6040/60C0/60F0/6140/6150/6160/6170 (Breiten 1800..5900); DOOR26 **V2/V3 (ein Fluegel)** nur im Kanal 3030 @0x015B8 /
3040 @0x01004 (Breiten 3020/2560). DOOR27 nur A-2-Kabine (6010/6020/6060/6070). Die Belegzeilen im Dossier und in
`gen/re15_tuer_eigen.inc` stimmen mit den Bytes ueberein.

### Messung an der exe (echter Weg: Tuer betreten, Aktionstaste)

```
E1  lauf.sh e1_fahrstuhl RE15_SET_FLAG=4:243 RE15_DEBUG_JUMP=10C0@120 RE15_PLAYER_POS=1500,7000,1024,0
        "RE15_PAD_AT=150:A,160:A,170:A" "RE15_EXIT_AT=60#1080" RE15_TUER_BOGEN=<M>/e1_bogen RE15_NOAUDIO=1
    [aot] DOOR FIRE slot=1 rect=(1500,6400,hw=1000,hh=500) target_cut=0 spawn=(-13650,0,-900)
    [tuer] Sequenz Archiv 2 DOOR27 Variante 0 Bit7 0 Tuer 39 Seite S040 T017 Spender FF (10 Skripte)
    [tuer] Sequenz fertig: 271 Bilder + 1 Warten, 0 Se_on, Schliesston 1, Ton STUMM ...
    [room] PC loaded room1080.rdt
E1b dasselbe mit Ton (RE15_AUDIO_CAP_SYNC): "[tuer] Bild 70 Se_on Tonkopf-Eintrag 0", "1 Se_on, Schliesston 1, Ton geladen"
E2  lauf.sh e2_schott RE15_KEIN_GATOR=1 RE15_DEBUG_JUMP=2090@120 RE15_PLAYER_POS=6150,-16350,0,1 "RE15_PAD_AT=150:A,..."
        "RE15_EXIT_AT=60#20A0" RE15_TUER_BOGEN=<M>/e2_bogen RE15_NOAUDIO=1
    [aot] DOOR FIRE slot=1 rect=(7250,-16350,hw=500,hh=1750) target_cut=13 spawn=(4950,-3600,4100)
    [tuer] Sequenz Archiv 2 DOOR26 Variante 0 Bit7 0 Tuer 38 Seite S164 T084 Spender FF (18 Skripte)
    [room] PC loaded room20a0.rdt
E3  Pruefhaken RE15_TUER_SEITE=S013,S040,S057,S164,S165,S041,S044 RE15_TUER_BOGEN=<M>/seq/bogen: alle fuenf
    Fahrstuhl-/Schott-Seiten "DOOR27 V0" bzw. "DOOR26 V0", Bilder <M>/seq_montage.png, Serie S164 <M>/s164_serie.png,
    Echtweg-Bilder <M>/g1_echtweg.png.
```
Gesehen: Fahrstuhl = zwei Fluegel mit Verzahnung schieben aus der Mitte auseinander (RE1.5 ROOM10800 malt die
Kabinentuer zweifluegelig mit Mittelfuge — Mechanik passt; vorher hob sich EIN Blatt). Schott 2090 = Rad dreht
(Bild 60..130), der Fluegel mit Rad schwenkt auf, die Kamera faehrt durch; zweiter Fluegel "TYPE-P" steht.

### Urteil Punkt 1: **erfuellt** (fuer die zwei genannten Tueren)

Beide genannten Tueren spielen jetzt die RE2-Tuer unveraendert, die RE2 fuer diese Art Tuer benutzt; belegt an den
Bytes, gemessen im echten Durchgang der exe. Hinweise (keine Maengel, aber fuer den Nutzer):
* H1 Optik: DOOR27 ist die RE2-Labor-Fahrstuhltuer (oliv-grau, Verzahnung, Bedienfeld), DOOR26 das rostrote Labor-
  Schott "TYPE-P". Die gemalten RE1.5-Tueren sind glatt-grau (Fahrstuhl) bzw. dunkel mit Warnstreifen (2090). Das
  vorherige P26W hatte die gemalten Farben. Der Nutzer hat RE2 als Vorbild genannt; ob ihm die Labor-Optik im Revier
  recht ist, sieht nur er.
* H2 Variante 2090: RE2s eigene Kanal-Schott-Tuer (3030/3040, Breite 2560..3020) ist EINFLUEGELIG (V2/V3); die Spur
  waehlt V0 (zweifluegelig, RE2-Labor ab 3200) wegen der RE1.5-Flaeche 3500 — begruendet und belegt, aber eine Wahl.
* H3 "Einige Türen ... zum Beispiel": das Dossier prueft nur die zwei genannten Tueren; ein Durchgang ueber die
  uebrigen Port-Archive (gleiche Fehlerklasse "falsche Mechanik") fehlt. Der Fahrstuhl-AUSSTIEG (ROOM1080 -> Etage)
  hat weiter KEINE Sequenz, obwohl RE2 beim Verlassen der A-2-Kabine DOOR27 V0 spielt (ROOM6010 @0x004C2/@0x004E2/
  @0x00502, Rechteck w 1800 d 3900 = echte Tuer; selbst gelesen). Die Spur fuehrt das unter OFFEN mit Adressen.

---

## Fortsetzung der Abnahme (zweite Sitzung, 2026-10-11 ab ~11:20)

Die erste Sitzung brach nach Punkt 1 ab (Wochenlimit). Fortsetzung durch einen zweiten Pruefer am
selben Baum (HEAD 414977b6 = 10754312 + Teilbericht; Code unveraendert). Messordner dieser Sitzung:
`<scratchpad>/G_abn0b/` (im Folgenden `<M2>`), Laeufer `<M2>/lauf.sh` (gleiche Umgebung wie `<M>/lauf.sh`),
Messkopie `re15_pc_g36abn0.exe` (cmp-identisch mit `re15_pc.exe` vom 03:37:08).

### 0b. Bau und Tests (erneut)
* `local_build.sh configure` -> OK, `local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`
  (`<M2>/build.log`); `git status --short re15_port` leer -> die exe ist der HEAD-Code-Stand 10754312.
* `ctest -R r36_tueren --timeout 600` (`<M2>/ctest_r36.log`): `100% tests passed, 0 tests failed out of 5`
  (unit_r36_tueren_fahrstuhl/schott/aufschliessen/textur, integration_r36_tueren 60,3 s).
* Die 81 Tests der ersten Sitzung (Regex `<M>/ctest_regex.txt`, alle gruen) liefen auf derselben exe und werden
  uebernommen (nicht wiederholt).
* Code-Diff (aufschliessen_r36.c, re15_aufschliessen_r36.h, gen/r36_aufschliessen.inc, gen/re2_karten_bank.inc,
  gen/re15_tuer_eigen.inc, scd_vm.c +3, game_step_common.c +2, main.c +3, audio_pc.c +65 am Dateiende,
  tuer_r36_pc.c, probe_r36_tueren.c, probe_r33_tueren.c, test_r36_tueren.cmake, probes/r36_tueren.cmake,
  tools/*) in dieser Sitzung erneut vollstaendig gelesen.

### RE-Gate (selbst nachgelesen, Bytes/Woerter aus den Dateien im Baum)

Stichproben (mehr als drei), je mit dem, was an der zitierten Stelle tatsaechlich steht:

| Zitat im Code/Dossier | Datei @Offset | gelesen | Urteil |
|---|---|---|---|
| RE2 ROOM2110 sub08 `Se_on(2,0x0C)` @0x01B4C | info/re2leon/PL0/RDT/ROOM2110.RDT | `36 02 0c 00 00 00 44 a4 ...` | stimmt |
| `Sleep 15` @0x01B58 und @0x01B8C | dito | `09 0a 0f 00` (Sleep/Sleeping, Zaehler 15) | stimmt |
| `Se_on(2,0x0D)` @0x01B5C, `Se_on(2,0x0B)` @0x01B90 | dito | `36 02 0d 00 ...`, `36 02 0b 00 ...` | stimmt |
| sub06 @0x01AE6 Set(4,0x0E), @0x01AEA Sce_Item_lost 0x34, @0x01AEC Evt_exec sub08 | dito | `22 04 0e 01`, `62 34`, `04 ff 18 08` | stimmt |
| RE2 EXE @0x800515f4 `lui a0,0x226` / @0x800515f8 `jal 0x8005ba28` / @0x800515fc `addu a1,zero,zero` | info/re2leon/PSX.EXE (t_addr 0x80010000, Kopf 0x800) | `3c040226`, `0c016e8a` (0x016e8a<<2 = 0x5ba28), `00002821` | stimmt |
| @0x800515ec `jal 0x8002fe38` (Text), @0x800515e8 `li a2,10` | dito | `0c00bf8e` (0xbf8e<<2 = 0x2fe38), `2406000a` | stimmt |
| Opcode-Tabelle @0x800A74C8: 0x09 -> 0x800539dc, 0x0A -> 0x80053a24 | dito | `800539dc`, `80053a24` | stimmt |
| Sleep @0x800539f4 `sw v0,0x1c(a0)` (pc+1), @0x80053a10 `lhu a0,2(a2)`, @0x80053a18 `sh a0,(v0)`, @0x80053a20 `addiu v0,zero,1` | dito | `ac82001c`, `94c40002`, `a4440000`, `24020001` | stimmt (von Hand decodiert) |
| Sleeping @0x80053a50 `addiu v0,v0,-1`, @0x80053a5c `bnez v0,+9` (-> 0x80053a84), @0x80053a6c `addiu v0,v0,3`, @0x80053a84 `jr ra`, @0x80053a88 `addiu v0,zero,2` | dito | `2442ffff`, `14400009`, `24420003`, `03e00008`, `24020002` | stimmt: Rueckgabe IMMER 2 (Bildende), bei 0 pc+3 |
| RE1.5 Sleep/Sleeping @0x8003F3E0/@0x8003F428 instruktionsgleich (ori statt addiu) | info/Re1.5/PSX.EXE | 18+26 Woerter gleich bis auf `34020001`/`34020002` (ori) | stimmt |
| RE1.5 ROOM10D0 sub19 @0x01988 Aot_reset Platz 2 sce 2, @0x0199E Message_on 5 | shared_assets/PSX/STAGE1/ROOM10D0.RDT | `46 02 02 31 d0 20 00 00 a2 fe`, `2b 05 ff ff`; kein `36` (Se_on) im Block | stimmt |
| ROOM10D0 sub20 @0x019AE Message_on 7, @0x019C4 Message_on 9 (Ja = Ck(12,0x1F)==0, Karte = Ck(9,0x34)==1) | dito | `2b 07 ff ff`, `21 0c 1f 00`, `21 09 34 01`, `2b 09 ff ff` | stimmt |
| ROOM10C0 main00 @0x00E66 Ck(4,0xF3)==0, @0x00E82 Door_aot_set Platz 1 | STAGE1/ROOM10C0.RDT | `21 04 f3 00`, `3b 01 02 31 ...` | stimmt |
| ROOM2090 @0x9B6 Door_aot_set Platz 1, Flaeche 1000 x 3500 | STAGE2/ROOM2090.RDT | `3b 01 02 31 01 00 5e 1a 4c b9 e8 03 ac 0d` = x 6750 z -18100 w 1000 h 3500 | stimmt (= exe-Log hw 500 hh 1750) |
| alle 29 Stellen `gen/r36_aufschliessen.inc` (Raum, Nachricht, sub-Offset) | je ROOMxxxx.RDT | 29/29: Byte `2b <msg>` am genannten Offset (`<M2>/stellen_check.py`) | stimmt |
| KARTSE.VBS = vier RE2-Wellen bytegleich (sha1 je Welle im .inc), Datei-sha1 b4dcdc24... | shared_assets/RE2/KARTSE.VBS gegen ROOM2110/ROOM20A0 VAG-Bereiche | 4/4 bytegleich an VBD 3144/6120/10760/22024, Datei 29592 B, sha1 stimmt (`<M2>/kartse_check.py`) | stimmt |
| P0CD.DO2: Tonteil/SCD/MD1 bytegleich DOOR0C, nur TIM neu | shared_assets/RE15DOOR/P0CD.DO2 gegen RE2/DOOR/DOOR0C.DO2 | gleich bis 0x6DFC (TIM-Beginn), Unterschiede 0x6E84..0xF01B | stimmt |

Guess-Tells (`deferred|tunable|interim|for now|faithful|plausibel|TODO|FIXME`) im re15_port-Diff: KEINE Treffer
(nur "xxx0/xxx1" als Raumbezeichnung). Env-Schalter: nur Messhaken (RE15_AUFSCHLIESS_LOG, RE15_TUER_ARCHIV,
RE15_TUER_BOGEN), kein Schalter, der Spielverhalten umschaltet. Konstanten: RE2_KARTE_SLEEP 15 (@0x01B58/@0x01B8C +
Sleep-Handler), Saetze 0x0C/0x0D/0x0B/0x26 (RE2-Bytes oben), Folgen je Art = RE2; die Zuordnung Nachricht -> Art ist
als PORT-WAHL gekennzeichnet (Kopf von aufschliessen_r36.c, Generator). Commit c8f2b5f3 (P0CD-Textur) traegt kein
@0x — die Textur ist eine PORT-WAHL (Kunst), ihre Masse sind aus dem gemalten Hintergrund ROOM10C04 abgeleitet
(Zeilenprofil im Dossier und im Rezept-Kopf), das ist die passende Belegform.

### Vertrag
* Pfade (`git diff 78a4a748...HEAD --name-only`): keine release/, platform/android/, shared_assets/PSX/, keine
  tests/*/CMakeLists.txt. Neue Dateien: engine/src/aufschliessen_r36.c, include/re15_aufschliessen_r36.h,
  platform/pc/src/tuer_r36_pc.c, gen/*.inc, tests/unit/probe_r36_tueren.c, tests/unit/probes/r36_tueren.cmake,
  tests/integration/test_r36_tueren.cmake, tools/*. Gemeinsame Dateien nur mit Haken: scd_vm.c +3, game_step_common.c
  +2, main.c +3 (audio_pc.c +65 am Dateiende = neuer Bank-Slot, nach dem Muster der vorhandenen Slots; steht nicht
  in der Liste der 1-5-Zeilen-Dateien). probe_r33_tueren.c (Runde-33-Riegel) um die fuenf RE2-Zeilen erweitert
  (notwendig, klein).
* Bank-9-Bits, Nachrichten-IDs, Ereignis-Nummern: keine verwendet (grep auf re15_game_flag_set/re15_msg_install/
  scd_event_fire in den neuen Dateien: leer). Spur G hat laut VERTRAG auch keine Zuteilung.
* Tests je Punkt: Punkt 1 unit_r36_tueren_fahrstuhl/schott + integration C; Punkt 2 unit_r36_tueren_textur
  (misst, Gegenprobe auf die alte Datei ROT); Punkt 3 unit_r36_tueren_aufschliessen + integration A/B.

### Punkt 1 — Nachtrag der zweiten Sitzung (Fahrstuhl-AUSSTIEG gemessen)

```
X1  lauf.sh x1_1080_ausstieg RE15_DEBUG_JUMP=1080@120 "RE15_FIRE_AOT=1@200#1080" "RE15_EXIT_AT=90#10C0" RE15_NOAUDIO=1
    [fire-aot] slot=1 at F200 (Raum 1080)
    [aot] DOOR FIRE slot=1 rect=(0,0,hw=0,hh=0) target_cut=0 spawn=(1450,0,7300)
    [room] PC loaded room10c0.rdt      <- KEINE "[tuer] Sequenz"-Zeile dazwischen
```
Das Verlassen der Kabine (ROOM1080 main00 @0x004A2 Door_aot_set Platz 1, Null-Rechteck, vom Skript per `Aot_on 1`
sub08 @0x0080E gezuendet; hier ueber denselben Pfad re15_aot_fire_slot) laeuft OHNE Tuersequenz direkt in den
Etagenraum. Das ist der Stand vor der Runde (keine Zeile fuer ROOM1080 in gen/re15_tuer_eigen.inc und
gen/tuer_zuordnung.inc) und vom Bau-Agenten unter OFFEN gefuehrt. RE2 spielt beim Verlassen der A-2-Kabine DOOR27 V0
(ROOM6010 @0x004C2/@0x004E2/@0x00502, erste Sitzung selbst gelesen). Der Nutzer hat "die Tuersequenz des Elevators"
genannt — die Einstiegsseite ist jetzt RE2-korrekt, die Ausstiegsseite hat weiterhin gar keine Sequenz. Ich werte
das nicht als Nicht-Erfuellung des Wortlauts (der Ausstieg hatte nie eine Sequenz, beanstandet war die falsche
Mechanik), fuehre es aber als Hinweis H3 fuer die Entscheidung des Nutzers (siehe Maengel/Hinweise unten).

Eigene Bogen-Bilder dieser Sitzung (`<M2>/x2_bogen/`, Montage `<M2>/x2_bogen_montage.png`, RE15_TUER_SEITE=
S041,S044,S040,S164): S040 = DOOR27 V0 zwei Fluegel mit Verzahnung aus der Mitte auseinander, S164 = DOOR26 V0
zweifluegeliges rostrotes Schott "TYPE-P" mit Rad links. Urteil Punkt 1 bleibt: **erfuellt** (mit H1-H3).

---

## Punkt 2 — "Die Tür Textur zwischen ROOM 10C0 und 10D0 bei der Türanimation ist herausstechend billig im Vergleich zum Rest."

### Was gemessen wurde (selbst, am gebauten Stand)

* Zuordnung unveraendert: `[tuer-seite] S041 ROOM10C0 DOOR0C V1 Spender FF Port-Archiv P0CD`, `S044 ROOM10D0 DOOR0C V0
  ... P0CD` (`<M2>/x2_bogen/debug.log`), Sequenz 307 Bilder, Bogen-Bilder S041/S044 anfang+mitte geschrieben.
* Datei: `shared_assets/RE15DOOR/P0CD.DO2` sha1 37f00db8 (master 78a4a748: a0f8bb19), gleiche Groesse 61468 B; bytegleich
  mit RE2 DOOR0C bis zum TIM-Beginn 0x6DFC, Unterschiede nur 0x6E84..0xF01B (= Textur). FNV in gen/re15_tuer_eigen.inc
  0xB4937612 (vorher 0xBCFF74C5).
* Masse (`probe_r36_tueren textur`, Blatt v 0..217, L Rec.601; Gegenprobe dieselbe Rechnung auf die master-Datei):

| | fein | fleck | form | hell | Faser gy/gx | Glas/Holz | Riegel |
|---|---|---|---|---|---|---|---|
| master P0CD (Runde 33) | 0.0814 | 0.0915 | 0.1824 | 114.5 | 0.44 (senkrecht) | 0.48 | ROT (4 Fehler) |
| HEAD P0CD (Runde 36) | 0.1039 | 0.1069 | 0.3544 | 86.1 | 4.43 (waagerecht) | 0.64 | GRUEN |

  "Gezeigt = gemalt" (Texel x 73/128 gegen das ROOM10C04-Zeilenprofil des linken Fluegels, 5 Zeilen): groesste
  Abweichung 6 je Kanal (y70 (99,84,66) vs (99,84,68) ... y138 (51,33,14) vs (45,31,11)).
* "Im Vergleich zum Rest" — `tools/tueren/r36_textur_mass.py` ueber alle 55 RE2-Archive + 29 Port-Archive
  (`<M2>/textur_mass.json`): vorher war P0CD mit hell 114.5 HELLER als jede RE2-Tuer (RE2-Maximum DOOR31 102.9) bei
  fein 0.0814 knapp ueber dem RE2-Minimum (DOOR07 0.0788) und form 0.18 im untersten Fuenftel — die hellste Tuer mit der
  wenigsten Zeichnung, das ist "herausstechend". Jetzt: fein 0.108 (RE2 0.079..0.525), form 0.357 (RE2-Median
  0.3955), hell 86.1 (RE2 11.7..102.9) — innerhalb der RE2-Verteilung auf allen drei Massen. (Mehrere Port-Archive
  liegen weiter ausserhalb: P24B hell 199, P06U 178, P07T 140/fein 0.037, P07G fein 0.053 — nicht Gegenstand dieses
  Punkts, aber derselbe Massstab wuerde sie als naechste "billige" Kandidaten nennen.)
* Gesehen (`<M2>/x2_bogen_montage.png`, `G_belege/g2_p0cd_vorher_nachher.jpg`, `<M>/g2_vergleich.png`): vorher
  gelbliches, flaches Blatt mit senkrechten verschwommenen Flecken, Fenster/Stange als gefuellte Rechtecke; jetzt
  waagerechte Holzfaser, Deckenlicht mit Abdunklung nach unten (wie ROOM10C04 gemalt), Drahtglas mit Rahmen und
  Spiegelverlauf, Panikstange als Zylinder mit Haltern, gebuerstetes Trittblech. Fenster-/Stangenlage wie gemalt
  (Fenster an der Mittelfuge). Neben S000/S059/S045/S022/S023/S080 (RE2- und Port-Tueren) faellt S041 nicht mehr
  als die glatteste/hellste heraus; sie ist eher die am feinsten gezeichnete.

### Urteil Punkt 2: **erfuellt** (messbar: kein Ausreisser mehr gegen RE2 und gegen die gemalte Tuer; der Rest ist
Geschmack des Nutzers — ob ihm das neue Blatt gefaellt, entscheidet er am Bild)

---

## Punkt 3 — "Türen die erfolgreich geöffnet wurden mit Karte oder sowas machen in Resident Evil 2 erst ein "piep" Sound und dann ein unlock Geräusch. Bei uns nicht."

### Mechanismus (gelesen, nicht geglaubt)
* Einhaengung: `scd_vm.c` op_message_on (Skript-Message_on) und re15_scd_show_message (Text-AOT) rufen
  `re15_aufschliessen_r36_notice(raum, msg, weg)`; die 29 Stellen in gen/r36_aufschliessen.inc tragen alle
  `wege = 2` (nur Skript). Trifft (Raum, Nachricht), startet die Folge; `re15_aufschliessen_r36_tick()` (game_step_common.c,
  jedes Spielbild, VOR dem Item-Modal-Return) spielt die faelligen Saetze; das Nachrichtenbild zaehlt als Bild 0
  (`frisch`-Schalter). Spieler = audio_pc.c `re15_audio_re2_karte_se` (Slot KARTSE.VBS, Kopie des TUERSE-Slots),
  angemeldet in main.c ueber tuer_r36_pc.c. PSX/Tests: kein Spieler = stumm, nur Zaehler.
* Folgen: KARTE 0/15/30 (Klack/Piep/Aufschliessen) = RE2 ROOM2110 sub08; KARTE_AUF 0/15 (Piep/Aufschliessen);
  KARTE_EIN 0 (nur Klack); RIEGEL 0 (RE2 Satz 0x26 vom Tuer-Handler @0x800515f8). Alle Bytes dazu oben im RE-Gate
  nachgelesen. Die Zuordnung Nachrichtentext -> Art ist PORT-WAHL (so gekennzeichnet); RE1.5 kennt kein key_type.
* Jede der 29 Nachrichten kommt im jeweiligen Raumskript genau an der genannten Stelle vor (Byte-Check 29/29);
  in den gemessenen Raeumen (10D0 msg 5/9, 1140 msg 0, 3010 msg 1, 1100 msg 2) ist es zudem das EINZIGE Message_on
  dieser Nummer im Raum (scd_dump) -> keine Fehlausloesung ueber einen anderen Skriptweg.

### Messung an der exe — ECHTER Weg (hinlaufen, Aktionstaste), Mitschnitt RE15_AUDIO_CAP_SYNC, Log RE15_AUFSCHLIESS_LOG
Alle Laeufe: `<M2>/lauf.sh <name> ... RE15_STATE_LOG=state.log RE15_AUDIO_CAP_SYNC=cap.raw RE15_AUFSCHLIESS_LOG=auf.log
RE15_SE_DEBUG=1`; Tick im Mitschnitt = Raumbild F + 124 (1470 Stereo-Frames je Bild; Versatz gemessen am Klack).
Hoerbarkeit NICHT nur ueber Baender, sondern per normierter Kreuzkorrelation der dekodierten RE2-Welle (PSX-ADPCM aus
KARTSE.VBS, 16397 Hz = pitch 0x5f3, auf 44100 Hz umgesetzt) mit dem Mitschnitt (`<M2>/korrel.py`; r = Korrelation an
der besten Lage, daneben die beste Lage ausserhalb +-1 Tick als Rauschmass; Gegenprobe Piep in einem Fenster ohne Ton:
r = 0.041).

**R4 ROOM3010, Green Keycard (Art KARTE)** — `RE15_SET_FLAG=3:59 RE15_DEBUG_JUMP=3010@120 RE15_PLAYER_POS=-26000,-20880,2048,1
RE15_PAD_AT=200:A,...` (Spieler vor der Tuer Platz 6, Aktionstaste bei F200 -> sub02 @0x02306 Message_on 1):
```
auf.log:   F201 raum=3010 nachricht=1 art=KARTE satz=0(KARTE_EIN) bild=+0 nr=1
           F216 raum=3010 nachricht=1 art=KARTE satz=1(PIEP)      bild=+15 nr=2
           F231 raum=3010 nachricht=1 art=KARTE satz=2(AUF_E)     bild=+30 nr=3
debug.log: [se] Stimme: se=0 ... vag=0 ... pitch=0x5f3 (16397 Hz) / se=1 vag=1 / se=2 vag=2   (KARTSE geladen)
state.log: msg(a=1 id=1) F201..F281 ("You've used the Green Keycard.")
Mitschnitt: KARTE_EIN beste Lage Tick 325.00 (= F201+124) r 0.480 (Rest 0.103)
            PIEP      beste Lage Tick 340.00 (= F216+124) r 0.793 (Rest 0.504); 1450-Hz-Band 0.49..0.81 in Ticks 340..354
            AUF_E     beste Lage Tick 355.00 (= F231+124) r 0.830 (Rest 0.132)
```
= Karte-Klack, nach 15 Bildern der Piep, nach weiteren 15 das Aufschliessen — hoerbar, in dieser Reihenfolge.

**R2c ROOM10D0, Blue Keycard + Ziffernfeld (Arten KARTE_EIN + KARTE_AUF)** — `RE15_SET_FLAG=9:52 RE15_DEBUG_JUMP=10D0@120
RE15_PLAYER_POS=1300,-5750,2048,0 RE15_PAD_AT=200:A,230:A,...,600:A RE15_SET_FLAG_AT=5:13,5:14,5:15,5:16@800
RE15_FRAMEDUMP=400-950/25:fd_` (Spieler vor dem Kartenleser Platz 1; Aktionstaste; Texte "A card reader." / "You have to
use the Blue Keycard and insert..." / Abfrage "Will you operate the card reader? Yes No" (msg 7, F201..F421) mit Ja
bestaetigt; dann msg 9; sub17 schaltet auf das Ziffernfeld (cam 11, "Enter the first number." F482..F531); die vier
Ziffern-Bits (5,13..16) bei F800 gesetzt, waehrend das Ziffernfeld offen ist — das ist der Zustand, den sub07 nach der
vierten richtigen Ziffer herstellt; sub01 @0x01526 feuert dann sub19 @0x0199E Message_on 5; die Ziffern selbst ueber
das Cursor-Raster einzugeben war nicht Gegenstand der Spur und wurde nicht ueber das Pad gefahren):
```
auf.log:   F423 raum=10D0 nachricht=9 art=KARTE_EIN satz=0(KARTE_EIN) bild=+0 nr=1   ("You've used the Blue Keycard.")
           F801 raum=10D0 nachricht=5 art=KARTE_AUF satz=1(PIEP)      bild=+0 nr=2   ("You've opened the lock.", cam 11 -> 0)
           F816 raum=10D0 nachricht=5 art=KARTE_AUF satz=2(AUF_E)     bild=+15 nr=3
Mitschnitt: KARTE_EIN Tick 547.00 (= F423+124) r 0.367 (Rest 0.169); PIEP Tick 923.98 (= F800+124, Mischer-Tick vor dem
            Protokollbild) r 0.726 (Rest 0.463); AUF_E Tick 940.00 (= F816+124) r 0.785 (Rest 0.149)
Bilder:    <M2>/r2c_montage.png (Leser -> Text -> Ziffernfeld -> "You've opened the lock.")
```
= beim Einstecken der Karte das Klack, beim richtigen Code Piep und eine halbe Sekunde spaeter das Aufschliessen.

**R3c ROOM1140, Riegel von dieser Seite (Art RIEGEL)** — `RE15_DEBUG_JUMP=1140@120 RE15_PLAYER_POS=-7250,-1100,3072,0
RE15_PAD_AT=200:A,...` (Spieler vor Platz 2 Mitte(-7250,-250), per RE15_AOT_DUMP geprueft; Aktionstaste F200 ->
sub02 @0x00C94 Message_on 0):
```
auf.log:   F201 raum=1140 nachricht=0 art=RIEGEL satz=3(AUF_HIER) bild=+0 nr=1
debug.log: [se] Stimme: se=3 layer=0 vag=3 note=65 fine=57 center=83 vol=127 pitch=0x5f3 (16397 Hz)
state.log: msg(a=1 id=0) F201..F281 ("You've opened the lock.")
Mitschnitt: AUF_HIER beste Lage Tick 325.00 (= F201+124) r 0.431 (Rest 0.072); Welle 0,81 s = Ticks 325..348
Bilder:    <M2>/r3c_montage.png
```
(Erster Versuch R3 mit Spieler bei z 800 scheiterte, weil die Position in der Wand lag — Kollision schob ihn nach z 2218,
kein Treffer; kein Befund gegen die Spur.)

**Ohne Ausloeser:** R2a/R2b (Abfrage offen, nie bestaetigt) und der 10D0-Lauf vor dem Code: auf.log leer, kein
KARTSE-Stimmenstart — kein Ton ohne erfolgreiches Aufschliessen.

**Elza-Raeume (10D1/11E1/1231/3011/1141/2021/2071/20A1/3041/6011/1101):** auf DIESEM Zweig nicht am Spiel pruefbar —
`RE15_DEBUG_JUMP=10D1` landet in der JUMP-Tabelle auf "10d 2F CORRIDOR (ROOM10D0)" (Lauf `<M2>/r5_elza_10d1`,
`[debug-menu] JUMP -> 10d 2F CORRIDOR (ROOM10D0)`; die Elza-Sprungkorrektur ist Spur I). Die Tabelleneintraege der
xxx1-Raeume sind byteweise gegen die xxx1-RDTs geprueft (29/29), der Mechanismus ist raum-/nachrichtengleich.

### Urteil Punkt 3: **erfuellt** — Kartentueren piepen und schliessen danach auf (RE2 ROOM2110-Folge, Abstand 15 Bilder
= RE2 Sleep 15), im echten Spielweg gemessen und im Mitschnitt nachgewiesen; Riegel von innen bekommt den RE2-Riegelton.

---

## Gates (zusammengefasst)
* **RE-Gate:** haelt — jede verhaltensrelevante Konstante traegt @0x/Datei-Offset oder ist als PORT-WAHL/RE2-ERGAENZUNG
  gekennzeichnet; 17 Zitate selbst nachgelesen, alle stimmen; keine Guess-Tells, keine Env-Schalter als Abschluss.
  Der Fix erklaert den Befund: RE1.5 hat an den Aufschliess-Stellen kein Se_on (ROOM10D0 sub19, 1140 sub02, 3010 sub02
  selbst gelesen) -> der Port war dort stumm; jetzt spielt er die RE2-Folge an genau diesen Stellen.
* **Vertrag:** haelt (Pfade, keine Bits/IDs/Ereignisse, Haken 2-3 Zeilen, eigene Testdateien).
* **Tests:** 5/5 Spur-Tests gruen (diese Sitzung), 81/81 Umfeld gruen (erste Sitzung, gleiche exe); die Riegel messen
  (Textur-Riegel auf der alten Datei ROT, Aufschliess-Riegel echter Skriptweg, Fahrstuhl/Schott echter Durchgang).

## Hinweise fuer Nutzer/Orchestrator (KEINE Maengel, alle Punkte erfuellt)
* **H1 Optik RE2-Labortueren** (erste Sitzung): DOOR27 (oliv-grau, Bedienfeld) und DOOR26 (rostrot "TYPE-P") laufen
  unveraendert; die gemalten RE1.5-Tueren sind glatt-grau bzw. dunkel mit Warnstreifen. Nutzer hat RE2 als Vorbild
  genannt — ob ihm die Labor-Optik im Revier recht ist, sieht nur er.
* **H2 Variante 2090** (erste Sitzung): V0 zweifluegelig gewaehlt (RE1.5-Flaeche 3500), RE2s Kanal-Schott 3030/3040 ist
  einfluegelig — begruendete Wahl.
* **H3 Fahrstuhl-AUSSTIEG** (gemessen X1): ROOM1080 -> Etage weiterhin ohne jede Tuersequenz (Stand vor der Runde);
  RE2 spielt beim Verlassen der A-2-Kabine DOOR27 V0 (ROOM6010 @0x004C2/@0x004E2/@0x00502). Nachruestbar, nicht
  beauftragt — fuer die Release Notes.
* **H4 ROOM4000 (Blue Master Keycard, Art KARTE):** RE1.5 spielt dort selbst Se_on(2,0x0F) VOR der Kartenabfrage (sub02
  @0x0142E) und direkt nach "You've used the Blue Master Keycard." den Tor-Motor Se_on(2,0x10) (Gosub sub03 @0x01470,
  Objekt 0 faehrt). Die Port-Folge Klack/Piep/Aufschliessen (0/15/30) liegt ueber dem Motor-Ton. Nicht an der exe
  gemessen (Cursor-Raster-Raum); statisch aus den Bytes. Ob die Ueberlagerung stoert, entscheidet der Nutzer beim Spielen.
* **H5 Elza-Raeume** nur statisch geprueft (s. o.); im Integrationsbaum (mit Spur I) einmal `RE15_DEBUG_JUMP=10D1` mit
  Elza fahren.
* **H6 Paket-Gate:** NEU shared_assets/RE2/KARTSE.VBS (29592 B, sha1 b4dcdc24...), GEAENDERT shared_assets/RE15DOOR/P0CD.DO2
  — Gate-Eintrag release/ = Orchestrator (im Dossier genannt).
* **H7 Messweg Ziffernfeld:** die Ziffern wurden nicht ueber das Pad eingegeben, sondern die vier Ziffern-Bits bei offenem
  Ziffernfeld gesetzt (= Zustand nach der vierten richtigen Ziffer); der Rest des Weges (sub01 -> sub19 -> Message_on 5)
  ist der echte Skriptweg.

## Ergebnis
| Punkt | Urteil | Beleg |
|---|---|---|
| 1 Fahrstuhl 1080 / grosse Tuer 2090 | erfuellt | RE2-Bytes ROOM6010/6020/6030 + DOOR26 Skript 3; exe E1/E2/E3/X1, Bogen-Bilder; unit_r36_tueren_fahrstuhl/schott |
| 2 Textur 10C0<->10D0 | erfuellt | probe textur GRUEN / alt ROT; Masse im RE2-Bereich (vorher hellste Tuer mit wenigster Zeichnung); gezeigt = gemalt +-6; Bilder |
| 3 Piep + Aufschliessen | erfuellt | RE2 ROOM2110 sub08 + EXE @0x800515f8 selbst gelesen; exe R4/R2c/R3c echter Weg, auf.log +0/+15/+30, Kreuzkorrelation r 0.37..0.83 an den erwarteten Ticks |

**bestanden = true.** Keine Maengel; Hinweise H1-H7 oben.
