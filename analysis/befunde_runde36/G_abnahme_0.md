# Runde 36 — Spur G "tueren": Unabhaengige Abnahme 0

Pruefer: Abnahme-Agent 0 (2026-10-11). Baum `.claude/worktrees/r36_tueren`, Zweig `r36/tueren`, HEAD 10754312,
Basis master 78a4a748 (merge-base). Massstab: Wortlaut AUFTRAG.md (die drei Tuer-Punkte), VERTRAG.md.
Geglaubt wird nur, was hier selbst am gebauten Stand gemessen ist.

Stand dieses Berichts: IN ARBEIT (wird fortlaufend geschrieben).

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
