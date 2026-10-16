# Runde 38 — Spur J "rekorder": UNABHAENGIGE ABNAHME 0

Pruefer: Abnahme-Agent 0 (glaubt dem Bau-Agenten nichts, was nicht selbst gemessen ist).
Der erste Abnahme-Lauf brach am Sitzungslimit ab (Teilbericht aab44f11, nur Abschnitt "Bau"). Dieser Bericht ist der
vollstaendige Neu-Lauf (2026-10-16, ab ~21:10); alles hier ist in diesem Lauf selbst gemessen.
Baum `.claude/worktrees/r38_rekorder`, Zweig `r38/rekorder`, HEAD `aab44f11` (Code-Stand `4c2a9a64`), Basis `1bcbd0ff`.
Wortlaut des Nutzers (AUFTRAG.md, massgeblich):

> Nutze beim Rekorder Rätsel beim reinsprechen den Sound Irons_Rekorder_Sound.wav aus dem synchro Ordner, und nachdem er
> fertig abgespielt ist, machst du den Piep Sound für Erfolgreiche Eingabe und den Tür öffnen Sound. Das zusammen mit der
> erfolgreich message auf den Panel.

Status: ABGESCHLOSSEN — **bestanden** (alle drei Teile des Wortlauts erfuellt, RE-Gate und Vertrag halten, Tests gruen).
Nicht blockierende Hinweise H1..H5 in §8 (H1: eine Gleichheits-Pruefung im Integrationstest prueft ein Mess-Artefakt).

## 0. Bau und Diff

* `git diff master...HEAD --name-only` (nur die Zweig-Aenderungen; `git diff master` zeigt zusaetzlich Loeschungen von
  `r38_w2.js`/`r38_w2b.js`/Release-Notes-Entwurf/AUFTRAG-Nachtrag — das sind spaetere master-Commits, nicht die Spur):
  Dossier + 4 Belegbilder + dieser Bericht; Code: `engine/src/rekorder_r38.c` (neu, 183 Z.), `include/re15_rekorder_r38.h`
  (neu, 100 Z.), Haken `scd_vm.c` (+8/-1: include, Scan, Dispatcher-Zeile, msg-2-Weiche), `game_step_common.c` (+2),
  `platform/pc/src/audio_pc.c` (+2), `tests/unit/test_synchro_stimme.c` (+7/-2); Tests `probe_r38_rekorder.c`,
  `probes/r38_rekorder.cmake`, `integration/test_r38_rekorder.cmake`; Werkzeug `tools/r38_j/rekorder_cap.py`.
* `git log 1bcbd0ff..master -- re15_port` ist leer: master hat seit der Basis keinen Code geaendert; `v0.8.25` (2077bb8e)
  ist Vorfahre der Basis, `git log v0.8.25..1bcbd0ff -- re15_port` ebenfalls leer -> v0.8.25-exe = Stand "vorher".
* `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `build` -> `ninja: no work to do.`,
  `=== LOCAL-BUILD-OK (build)`; exe `re15_pc.exe` 2026-10-16 17:42:08 (nach dem letzten Code-Commit 17:38:52), Arbeitsbaum
  sauber (`git status` leer) -> die exe ist der Code-Stand von HEAD.

## 1. Gezielte Tests (keine volle Suite)

| Lauf | Ergebnis | Protokoll |
|---|---|---|
| `ctest -R unit_r38_rekorder` (anker/datei/ablauf) | 3/3 bestanden, 0,30 s | `re15_port/build/r38j_abnahme0/ctest_unit.log` |
| `ctest -R integration_r38_rekorder` (echte exe L/S/E/T) | 1/1 bestanden, 267 s | `re15_port/build/r38j_abnahme0/ctest_integration.log` |
| Testgruppen der beruehrten Module/Raeume (Regex unten, ohne die 4 r38-Tests) | **93/93 bestanden**, 582 s, `-j 2` | `re15_port/build/r38j_abnahme0/ctest_gezielt.log` |

Regex (Bau-Agent-Auswahl, erweitert um Elza-1101-EMS, r36-Karten/Schluessel, r33-Tueren, alle r30-Tuer-Tests):
`r38_rekorder|r36_tueren|r37_codepanel|unit_synchro_stimme|unit_voice_ausreden|unit_se_voice_machine|unit_audio_sperren|
unit_msg_dialog|unit_action_msg_gate|unit_text_freeze|unit_r21_discard|r37_kistenregel|r37_pri1100|
r37_elzaems_(leon1100|elza1101|toc|weiche|raeume)|unit_lane_b1_t45_room1100|unit_scd_|unit_aot_|unit_cut_old_displayed|
unit_r34n_b_cut_old|unit_r30_tuer|r36_elza_unlock|r34n_f_leiche|integration_headless_run|integration_door_lock|
integration_keydoor|integration_keypad|unit_r36_karte|r36_schluessel|unit_r33_tuer` (darin u. a. integration_r36_tueren,
integration_r37_codepanel, integration_r36_elza_unlock, integration_r37_kistenregel, integration_r37_pri1100_*,
integration_r37_elzaems_leon1100/elza1101, integration_r36_schluessel* — alle gruen, kein Nachfahren noetig).

Ausgaben integration (Auszug): `[l] Aufnahme F499, Piep F676 = erstes ACCEPTED, Aufschliessen F691`, `Mitschnitt Aufnahme
Tick 623..702 (Korrelation 0.931), Piep ab Tick 800, Versatz 124`, `[l_piep_takt] ANZEIGE Cut 12`, `[l_piep_bild] ANZEIGE
Cut 11`; `[s] 32 Wartetakte an @0x00C96, Aufnahme zu Ende F579, frei F580, Piep F585`; `[e] Aufnahme F629, Piep F806`,
`Mitschnitt Aufnahme Tick 1526..1605 (Korrelation 0.902), Piep ab Tick 1703`; T bestanden (ROOM1110 geladen).

## 2. RE-Gate (selbst nachgeprueft)

### 2.1 ROOM1100 sub02 (Datei-Offsets, RE1.5)
`shared_assets/PSX/STAGE1/ROOM1100.RDT` sha1 32dbb558... == `info/Re1.5/PSX/STAGE1/ROOM1100.RDT`; `cmp` ROOM1100 ==
ROOM1101 (bytegleich). `scd_dump_room.py --sub sub02`: sub02 = Datei 0xC68..0xD20, Opcode-Walk:
```
0x00C8C Set 22 02 07 01 | 0x00C90 Message_on 2b 04 ff ff | 0x00C94 Evt_next 02 | 0x00C95 Nop 00
0x00C96 Cut_chg 29 0c | 0x00C98 Sleep 09 0a 05 00 | 0x00C9C Cut_chg 29 0b | 0x00C9E Sleep 09 0a 05 00
0x00CA2 29 0c / 0x00CA8 29 0b / 0x00CAE 29 0c / 0x00CB4 29 0b (je Sleep 5)
0x00CBA Message_on 2b 02 ff ff | 0x00CBE Evt_next | 0x00CC0 Set 22 02 07 00 | 0x00CC4 Cut_chg 29 06 | 0x00CC6 Cut_auto 3c 01
0x00CC8 Aot_reset 46 01 02 31 a0 f6 00 00 78 ec (slot 1 sce 2) | 0x00CD2 Aot_reset 46 02 01 31 .. (slot 2 sce 1)
```
Alle im Code zitierten Anker (@0x00C90/C94/C96/C9C/CBA/CD2) und die 56-Byte-Signatur `k_sig` stimmen Byte fuer Byte.
Kein `36` (Se_on) in irgendeinem Sub von ROOM1100 (Walk aller 10 Regionen) -> "RE1.5 spielt hier keinen Ton" stimmt.
sub08 @0x00D86 Cut 7 / @0x00D88 Sleep 30 / @0x00D90 Cut 8, sub07 @0x00D64 Cut 12 nach "Ja" — wie im Dossier.

### 2.2 RE2 ROOM2110 sub08 (Datei-Offsets, `info/re2leon/PL0/RDT/ROOM2110.RDT`)
```
0x1B4C 36 02 0c 00 ..  Se_on(2,0x0C)      0x1B58 09 0a 0f 00  Sleep 15
0x1B5C 36 02 0d 00 ..  Se_on(2,0x0D) Piep 0x1B68 65 01 65 02  (RE2 0x65 = Sce_espr_kill2: Lampen-Sprites weg)
0x1B6C 64 01 16 08 .. / 0x1B7C 64 02 16 08 ..  (RE2 0x64 = Sce_espr_on2: neue Lampen-Sprites)
0x1B8C 09 0a 0f 00  Sleep 15              0x1B90 36 02 0b 00 ..  Se_on(2,0x0B) Aufschliessen   0x1B9C 29 09 3c 01
```
Piep und Wechsel der Anzeige-Sprites im selben Skript-Takt (kein Sleep dazwischen), Aufschliessen 15 Bilder spaeter: stimmt.

### 2.3 MIPS-Stichprobe: RE2 Sleep/Sleeping (Grundlage des Abstands +15)
`re2_disasm.py dis 0x800539dc` (Sleep): `lw a2,28(a0)` pc / `addiu v0,a2,1` / `sw v0,28(a0)` pc+1 / `lhu a0,2(a2)` Zaehler /
`sh a0,0(v0)` / `jr ra; addiu v0,zero,1`. `dis 0x80053a24` (Sleeping): Zaehler `addiu v0,v0,-1` @0x80053a50, bei 0 pc+3
@0x80053a6c, `80053a84: jr ra / 80053a88: addiu v0,zero,2` IMMER Bildende. -> Se_on Bild T, naechster Se_on T+15. Stimmt
mit `aufschliessen_r36.c` (RE2_KARTE_SLEEP 15) ueberein, die Spur benutzt diese Runde-36-Folge unveraendert.

### 2.4 Mini-Bank
`gen/re2_karten_bank.inc`: Satz 1 PIEP = ROOM2110 Satz 0x0D, Satz 2 AUF_E = Satz 0x0B (Runde 36, unveraendert);
`gen/r36_aufschliessen.inc` Z. 19/20: `{ 0x1100, 2, RE15_AUF_ART_KARTE_AUF, 2 }`, `{ 0x1101, 2, ... }`.

## 3. Messung am Spiel (eigene Laeufe, ECHTER Weg)

Messkopie `re15_port/build/platform/pc/re15_pc_r38j_abn0.exe` (sha1 1bba94ea... == re15_pc.exe, nach den Laeufen
geloescht), Laeufer `re15_port/build/r38j_abnahme0/werkzeug/lauf.sh`, Eingaben `.../werkzeug/weg.py` (Bildnummern je
Raum; der Bildzaehler beginnt bei jedem Raumladen neu). Gemeinsame Umgebung: `RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
RE15_STATE_LOG RE15_MSG_LOG=1 RE15_SCD_TRACE=1 RE15_AUFSCHLIESS_LOG=auf.log RE15_REKORDER_LOG=rk.log RE15_CUT_SYNC_LOG=cut.log
RE15_STIMME_LOG=1 RE15_AUDIO_CAP_SYNC=cap.raw RE15_SET_FLAG=3:0x67 RE15_GIVE=0x44:1` (Minidisc Player w/ Disc im Inventar).

**Weg (anders als der Integrationstest, der das Panel per `RE15_FIRE_AOT=2` feuert):** `RE15_DEBUG_JUMP=10D0@120` mit
`RE15_PLAYER_POS=-18400,24150,2048` (vor der Tuer ROOM10D0 Platz 16 -> ROOM1100), Viereck -> `[aot] DOOR FIRE slot=16 ...
spawn=(-20975,0,-25000)` -> `[room] PC loaded room1100.rdt`; in ROOM1100 zu Fuss (Rennen Ost 46 Bilder, links 10, Nord 79,
links 11, West 62, rechts 10; Lage gemessen per STATE_LOG: Spieler (-25208,-10168) rot -1056 vor dem Panel), Viereck F445
-> Panel-AOT Platz 2 (sce 3) feuert ueber die Aktions-Pruefung (msg 1 offen ab F446); dann msg 1, msg 3 "Ja", Cursor
ON/OFF (SYSTEM READY F703, SAY HELLO F733), Cursor Mikrofon, Viereck F793. Nach dem Oeffnen zu Fuss an die Westtuer und
Viereck -> `[aot] DOOR FIRE slot=1 ...` -> `[room] PC loaded room1110.rdt` (Elza: room1111).

### 3.1 Leon langsam (Lauf m2_leon_langsam, Nachrichten werden gelesen)
```
rk.log  F794 stimme an: sub02 @0x00C94 (Message_on 4 im selben Takt), Id 40 -> synchro/STAGE1/room1100/Irons_Rekorder_Sound.wav
        F795 stimme laeuft: Plattform meldet 79 Restbilder / F874 stimme zu Ende: 81 Bilder nach der Anforderung
        F966 frei: sub02 @0x00C96 Cut_chg 12 nach 0 Wartetakten / F971 ACCEPTED: sub02 @0x00C9C Cut_chg 11 -> Piep + Aufschliessen
        F996 Message_on 2 @0x00CBA: ... Runde-36-Folge hier aus (1) / F1037 Tuer frei: Platz 1 aktiv 1 typ 1
auf.log F971 nachricht=2 art=KARTE_AUF satz=1(PIEP) bild=+0 / F986 satz=2(AUF_E) bild=+15   (genau 2 Zeilen)
debug   [voice] clip loaded: Irons_Rekorder_Sound.wav (32000 Hz x1 -> 116736 @44100 Hz); [scd F966] Cut_chg(12),
        [scd F971] Cut_chg(11), F976 12, F981 11, F986 12, F991 11, [scd F1037] Cut_chg(6)
cut.log angezeigt: F733 #8 SAY HELLO .. F966, F967 #12, F972 #11 ACCEPTED, F977 #12, F982 #11, F987 #12, F992 #11 (46 Bilder), F1038 #6
Bilder  F794 msg 4 "Yo..", F800..F860 msg 4 tippt (bis ~F864), F875..F960 Wegwerf-Abfrage "Discard it?", F966 SAY HELLO,
        F967/F971 leer, F972 ACCEPTED, F987 leer (Blinkphase), F997 ACCEPTED + msg 2
        -> re15_port/build/r38j_abnahme0/abn0_leon_echtweg_ablauf.png
Tuer    F1090 Viereck an der Westtuer -> [aot] DOOR FIRE slot=1 -> [room] PC loaded room1110.rdt -> EXIT_AT Bild 60 in 1110
```

### 3.2 Ton: Differenz gegen v0.8.25 (gleicher Weg, gleiche Eingaben)
Gegenprobe "vorher" mit der ausgelieferten v0.8.25-exe (Kopie von `release/pkg-win/re15_port_v0.8.25`, sha1 f1addbd9...;
Code-Stand = Basis, s. §0), Lauf b2_v0825_langsam: kein `[voice] clip loaded`, `stimme.log` "nachricht=4 laeuft=0",
auf.log **F996** PIEP / **F1011** AUF_E (an msg 2), Tuer nach ROOM1110 ebenso. Beide Mitschnitte 10454640 B (1778 Ticks).
Auswertung `re15_port/build/r38j_abnahme0/werkzeug/diffana.py` (neu - alt, Mono):
```
identisch bis Tick 1250 (Musik, Schritte, Tuer 10D0, Menuetoene: bitgleich)
Differenz Tick 1250..1329 (80 Ticks) = die WAV: Korrelation 1.0000, Verstaerkung 1.000, Anfang Tick 1250 + 0 Samples
Differenz Tick 1426..1502 = Tonpaar: neu ab Tick 1426, alt ab Tick 1451 (= +25)
WAV im Mitschnitt neu (mit Musik) Korrelation 0.9287; im Mitschnitt v0.8.25 0.0368 (nicht vorhanden)
```
Identitaet des Tonpaars (Kamm-Inversion P(t) = r(t) + P(t - 25 Ticks) ab Tick 1426): P ist endlich, RMS 1385/1274/1211 in
den Ticks +0..+14 (Piep), danach Aufschliessen bis +49, **ab +55 exakt 0**; Gegenprobe mit 24 bzw. 26 Ticks Versatz:
Rest RMS 2385 bzw. 2263 (verschwindet nicht). => Der neue Stand spielt **sample-genau dasselbe Tonpaar** wie v0.8.25,
nur 25 Bilder frueher (am ersten ACCEPTED statt an "You've opened the lock."). Spektrum: Piep Spitze 1466 Hz (94 % der
Energie in 1350..1550 Hz), Aufschliessen tieffrequent (Spitze 29 Hz, 70 % unter 250 Hz).
Takt-Zuordnung: Stimme ueber die Warteschlange hoerbar ab Tick = Anforderungsbild + 456 (F794 -> 1250), Piep direkt
gespielt ab Tick = Bild + 455 (F971 -> 1426).

### 3.3 Leon schnell (Lauf m3_leon_schnell, Viereck dicht: msg 4 + Wegwerf-Abfrage sofort)
```
rk.log  F794 stimme an / F795 laeuft (79) / F874 stimme zu Ende / F875 frei: ... nach 32 Wartetakten / F880 ACCEPTED -> Piep
auf.log F880 PIEP +0 / F895 AUF_E +15; msg 2 F905 ohne Folge
cut.log F733 #8 SAY HELLO .. F875, F876 #12, F881 #11 ACCEPTED, ...
lage.py WAV Tick 1250..1329 (Korrelation 0.9050), Tonpaar ab Tick 1335 (= F880 + 455, Korrelation 0.8693),
        Abstand letztes WAV-Sample -> Piep-Anfang 8215 Samples = 5,6 Ticks
Bilder  F842..F875 kein Text, Anzeige SAY HELLO, Irons spricht weiter; F876 leer, F881 ACCEPTED
        -> re15_port/build/r38j_abnahme0/abn0_leon_schnell_warten.png
```
sub02 kam an @0x00C96 an, als die Aufnahme noch 32 Bilder zu spielen hatte (F843); ohne das Warten waere ACCEPTED +
Piep 5 Bilder spaeter (F848, Cut 12 + Sleep 5) mitten in die Aufnahme gefallen (abgeleitet aus den 32 Wartetakten). Mit
dem Warten geht sub02 genau 1 Bild nach dem letzten Aufnahme-Tick weiter.

### 3.4 Elza ROOM1101 (Lauf m4_elza_langsam, `RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`, `[pl] Spieler-Familie PL04`)
Echter Weg ROOM10D1 -> Tuer Platz 16 -> `PC loaded room1101.rdt` -> zu Fuss ans Panel -> ... -> Westtuer -> `room1111.rdt`.
```
rk.log  F794 raum=1101 stimme an ... -> synchro/STAGE1/room1100/Irons_Rekorder_Sound.wav / F874 zu Ende / F966 frei (0)
        F971 ACCEPTED -> Piep / F996 Message_on 2 ... aus / F1037 Tuer frei
auf.log F971 raum=1101 PIEP +0 / F986 AUF_E +15
lage.py WAV Tick 1994..2073 (Korrelation 0.9287), Tonpaar ab Tick 2170 (= F971 + 1199), Abstand 96,6 Ticks
cut.log F967 #12, F972 #11 ACCEPTED, F977 #12, F982 #11, F987 #12, F992 #11, F1038 #6
```

### 3.5 Echtzeit (Lauf m5_echtzeit_schnell, OHNE `RE15_AUDIO_CAP_SYNC`, echtes Audiogeraet, `RE15_AUDIO_CAP=cap_rt.raw`)
Gleiche Eingaben wie 3.3. In Echtzeit liefen die Spielbilder langsamer als die Tonausgabe (die Aufnahme war nach 57
Spielbildern zu Ende statt nach 81) — genau dafuer wartet der Haken auf den echten Kanal statt auf eine Bildzahl:
```
rk.log  F794 stimme an / F795 laeuft (78) / F850 stimme zu Ende: 57 Bilder / F851 frei: ... nach 8 Wartetakten
        F856 ACCEPTED -> Piep / F881 Message_on 2 ... aus / F957 Tuer frei
auf.log F856 PIEP +0 / F871 AUF_E +15
cap_rt  WAV gefunden (Korrelation 0.8901), letztes WAV-Sample 2137087; Piep-Band 1350..1550 Hz ab ~2150317
        (Anteil 0.59, davor 0.00) = ~13000 Samples (~0,3 s) NACH dem letzten WAV-Sample; Tonpaar-Korrelation 0.52
```
Auch in Echtzeit: kein Ueberlappen, Piep erst nach dem Ende der Aufnahme.

### 3.6 Weitere Pruefungen am Code/Bestand
* WAV selbst gelesen: RIFF/WAVE, `fmt ` PCM (1), 1 Kanal, 32000 Hz, 16 bit, data 169414 B = 84707 Samples = 79,41
  Bilder, Spitze 30146; sha1 5963ace6... im Zweig == master (Datei des Nutzers unveraendert, nicht umbenannt).
* `test_synchro_stimme.exe` direkt: "= .../synchro/STAGE1/room1100/Irons_Rekorder_Sound.wav: Sonderbindung
  re15_rekorder_r38_stimme", "Zensus: 195 WAVs, 0 Namensfehler, 0 Formatfehler, 0 Stage-Fehler, 0 noch nicht zugeordnet";
  die Datei laeuft jetzt durch die Formatpruefung P4 (vorher als "noch nicht zugeordnet" uebersprungen).
* Paket: `release/make_package.sh` kopiert `synchro/STAGE*` ganz (Z. 681-684) und vergleicht die WAV-Zahl Repo/Paket
  (Z. 324-326); Android: `apk_asset_gate.py` Z. 159/1435 `include "STAGE*/**"` -> die Datei kommt in alle Pakete.
* Raumladen: Scan in `scd_register_current_rdt`, gerufen ueber `scd_register_room_events` vom Boot-/CONTINUE-Weg
  (main.c:4510) und vom Raumwechsel (scd_room_setup.c:368) -> Anker wird bei jedem Laden neu gesetzt/geloescht
  (kein haengender `s_basis` in anderen Raeumen).
* `re15_game_step`: vor `re15_rekorder_r38_tick()` (game_step_common.c:1072) gibt es kein `return`; der Zaehler laeuft je Bild.

## 4. Konstanten-Gate (Diff `re15_port`, selbst gelesen)

| Konstante | Wert | Beleg / Kennzeichnung | geprueft |
|---|---|---|---|
| RE15_REK_MSG4/START/WARTE/ACCEPTED/MSG2/TUER | @0x00C90/C94/C96/C9C/CBA/CD2 | ROOM1100.RDT Datei-Offsets | Bytes + Opcode-Walk §2.1 |
| k_sig / RE15_REK_SIG_LAENGE | 56 B @0x00C90..0x00CC7 | dieselben Bytes | 0xCC8-0xC90 = 0x38 = 56, Byte fuer Byte |
| RE15_REK_ACCEPTED_CUT | 11 | `29 0b` @0x00C9C; Hintergrund Cut 11 = "ACCEPTED" | Bytes + eigene Framedumps (F972 #11) |
| RE15_REKORDER_STIMME_ID | 40 | VERTRAG §1.2 Spur J (1100/1101 Ids 40-44) = PORT-WAHL im Kontingent | Vertrag |
| RE15_REKORDER_ORDNER/STAMM | synchro/STAGE1/room1100/ Irons_Rekorder_Sound | NUTZER-VORGABE (Datei des Nutzers) | Datei |
| Piep +0 / Aufschliessen +15, Satz 1/2 | Runde-36-Folge KARTE_AUF | RE2 ROOM2110 @0x01B5C/@0x01B8C/@0x01B90, Sleep @0x800539dc/@0x80053a24 | §2.2/§2.3, Mitschnitt §3.2 |
| REK_NOTBREMSE | 300 Bilder | "dieselbe Grenze wie der Stimmen-Riegel ... Kein Original-Gegenstueck" (= RE15_VOICE_NOTBREMSE) | als Port-Wahl gekennzeichnet; greift in keinem Lauf |
| REK_STEMPEL_BILDER | 2 | "Port-Struktur, kein Original-Gegenstueck" | rk.log "laeuft ... 2 Bilder nach der Anforderung" |

Suche im Diff nach "deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|vorerst|ungefaehr|geschaetzt": kein
Treffer im neuen Code (einziger Treffer "SIEHT richtig aus" ist ein Kontext-Kommentar von 2026-08 in
test_synchro_stimme.c). getenv im Diff: nur `RE15_REKORDER_LOG` (Messschiene, kein Verhalten) und `RE38J_DEBUG`/
`RE38J_CURSOR` in der Sonde. Kein Env-Schalter als Abschluss.
Erklaert der Fix den Befund? Vorher (v0.8.25 gemessen): keine Aufnahme, Tonpaar an msg 2 (F996). Nachher: Aufnahme im
Takt von msg 4, Tonpaar am ersten ACCEPTED, sample-gleich (§3.2). Ja.

## 5. Vertrag

* Pfade (`git diff master...HEAD --name-only`): keine `release/`, `platform/android/`, `shared_assets/PSX/`, keine
  CMakeLists; Tests in eigenen Dateien (`probes/r38_rekorder.cmake`, `integration/test_r38_rekorder.cmake`).
* Gemeinsame Dateien nur Haken mit Kommentar "Runde 38 Spur J": scd_vm.c (include + 1 Zeile Scan + 1 Zeile Dispatcher +
  2 Zeilen msg-2-Weiche), game_step_common.c (include + 1), audio_pc.c (include + 1), test_synchro_stimme.c (5 Zeilen,
  Zensus). Hinweis Integration: `r38/elzaintro` haengt in audio_pc.c `re15_voice_load_clip` wenige Zeilen weiter unten und
  ein include daneben ein (Textkonflikt moeglich, kein Wirkkonflikt: dort nur ROOM1241); scd_vm.c/game_step_common.c
  werden auch von dining1141/elzatoene/kleinkram/spinnen/tagebuch/heliport1171 beruehrt (nur Haken).
* Ressourcen: keine Bank-9-Bits, keine Nachrichten-Texte, keine Item-Ids; Stimmen-Id 40 im Kontingent 1100/1101 40-44.
* Tests je Punkt: `unit_r38_rekorder_anker/datei/ablauf` + `integration_r38_rekorder` (echte exe, Mitschnitt mit
  Korrelation, Framedumps) — vorhanden und messend, in diesem Lauf gruen (§1).

## 6. Abgleich der Behauptungen des Bau-Agenten

| Behauptung | eigenes Ergebnis |
|---|---|
| vorher: beim Rekorder kein Ton, erstes ACCEPTED stumm, Piep/Aufschliessen erst an Message_on 2 | bestaetigt mit der v0.8.25-exe (b2: kein Clip, PIEP F996 / AUF_E F1011 an msg 2) |
| ROOM1100 == ROOM1101, sub02/sub01/sub08-Offsets, RE2 ROOM2110-Offsets | bestaetigt (§2) |
| Cuts 7..12 = Anzeigen des Panels, Cut 11 = ACCEPTED | bestaetigt (eigene Framedumps F703 #7, F733 #8, F967 #12, F972 #11) |
| WAV PCM16 mono 32000 Hz, 169414 B, 79,4 Bilder | bestaetigt |
| Aufnahme im Takt von Message_on 4 (Id 40), Warten an @0x00C96, Piep +0 / Aufschliessen +15 am ersten ACCEPTED, msg 2 ohne zweite Folge | bestaetigt (Leon langsam/schnell/Echtzeit, Elza) |
| Aufnahme Tick 623..702, Korrelation 0.931 | gleichwertig bestaetigt (eigener Lauf: Tick 1250..1329, 0.9287; in der Differenz 1.0000) |
| "Piep ab Tick 800 im Bild mit ACCEPTED", "Versatz gleich fuer Aufnahme und Piep" | **ungenau**: der Piep beginnt im Ton-Takt des Skript-Bilds von Cut_chg 11 (eigener Lauf Tick 1426 = F971), die Anzeige zeigt ACCEPTED ab dem naechsten Bild (F972); die Aufnahme laeuft ueber die Warteschlange einen Takt spaeter an (F794 -> Tick 1250 = +456, Piep F971 -> 1426 = +455). Hinweis H1 |
| schnell: 32 Wartetakte, Piep 7 Ticks nach dem Ende | 32 bestaetigt; genau 5,6 Ticks (8215 Samples) vom letzten WAV-Sample bis zum Piep-Anfang |
| Elza ROOM1101 dieselbe Folge | bestaetigt (echter Weg 10D1 -> 1101 -> 1111) |
| gezielte Laeufe 55/55 | eigene Auswahl 93/93 + 4/4 (r38) |

## 7. Urteil je Punkt (gegen den Wortlaut)

| Teil des Wortlauts | Urteil | Beleg |
|---|---|---|
| "beim reinsprechen den Sound Irons_Rekorder_Sound.wav aus dem synchro Ordner" | **erfuellt** | Datei des Nutzers (unveraendert) spielt ab dem Mikrofon-Druck im Takt von "You've used the Minidisc Player w/ Disc." (rk.log F794, `[voice] clip loaded: Irons_Rekorder_Sound.wav`), im Mitschnitt sample-genau (Korrelation 1.0000, Verstaerkung 1.000 in neu - v0.8.25), Leon und Elza, echter Weg |
| "nachdem er fertig abgespielt ist, ... Piep Sound für Erfolgreiche Eingabe und den Tür öffnen Sound" | **erfuellt** | Piep beginnt erst nach dem letzten WAV-Sample: schnell +5,6 Ticks (sub02 wartet 32 Takte), langsam +96,6 Ticks (Spieler schliesst msg 4 + Wegwerf-Abfrage), Echtzeit ~0,3 s; Tonpaar = Piep (1466 Hz) + Aufschliessen der Kartenleser/Code-Panels, sample-gleich mit dem Paar von v0.8.25 (Kamm-Inversion, Rest 0 ab +55 Ticks) |
| "Das zusammen mit der erfolgreich message auf den Panel" | **erfuellt** | Piep im Skript-Takt des ersten Cut_chg 11 "ACCEPTED" (@0x00C9C), Anzeige ACCEPTED ab dem naechsten Bild (F972; RE2-Form ROOM2110 @0x01B5C/@0x01B6C: Ton und Anzeige im selben Skript-Takt); Aufschliessen +15 waehrend ACCEPTED blinkt; "You've opened the lock." +25 ohne zweites Paar; Tuer danach benutzbar (ROOM1110/1111 geladen) |

Gates: RE-Gate haelt (§2, §4), Vertrag haelt (§5), Tests vorhanden/messend/gruen (§1). **Bestanden.**

## 8. Hinweise (nicht blockierend, nachpruefbar)

* **H1 (Messung/Test, Dossier §4.1 + `integration/test_r38_rekorder.cmake` Z. 164-168):** Die Pruefung "Takt-Versatz
  Aufnahme == Piep" stimmt nur, weil der Piep-Detektor (`probe_r38_rekorder cap`, Anteil 1350..1550 Hz > 0,3) den
  Anfangs-Tick des Pieps verfehlt: in meinem Lauf m2 meldet er "PIEP erster Tick 1427", der Piep beginnt aber in Tick 1426
  (Differenz neu - alt ab Sample 78 von Tick 1426, RMS 1809 = lautester Piep-Tick; Bandanteil dort nur 0,16). Wirklich
  gilt: Stimme (Warteschlange) +456, Piep (direkt gespielt) +455. Funktional ohne Folgen (1 Bild, entspricht der RE2-Form
  "Se_on und Anzeige im selben Skript-Takt"), aber der Riegel prueft ein Mess-Artefakt und der Satz im Dossier ("ab dem
  naechsten Audio-Tick gemischt ... gleich fuer Aufnahme und Piep") ist falsch. Empfehlung: Erwartung auf
  `Versatz Piep = Versatz Aufnahme - 1` mit einem Einsatz-Detektor (z. B. Differenz/Vorlage) umstellen oder die
  Gleichheits-Pruefung streichen und den Dossier-Satz korrigieren.
* **H2 (Dossier §3.1/§6):** "die Zeile 4 bleibt im Untertitel-Nachhall stehen, solange sie spricht" ist am Spiel nicht zu
  sehen: im schnellen Lauf zeigt das Bild F843..F874 keinen Text, waehrend Irons weiterspricht (die Wegwerf-Abfrage hat
  msg 4 abgeloest). Nur Doku.
* **H3 (Integration):** Textnachbarschaft mit `r38/elzaintro` in audio_pc.c (`re15_voice_load_clip`, include-Block);
  RE15_MIN_TESTS +4; Android-GLOB-Cache neu konfigurieren (neue `rekorder_r38.c`); optionale ausdrueckliche Paket-Pruefung
  der WAV — wie vom Bau-Agenten vermerkt.
* **H4 (was der Nutzer erlebt):** Wer die Meldung "You've used the Minidisc Player w/ Disc." liest und danach die
  Wegwerf-Abfrage beantwortet, hoert den Piep erst danach (gemessen ~3,2 s nach dem Ende der Aufnahme); wer schnell
  klickt, wartet auf SAY HELLO, bis Irons fertig ist, dann leer -> ACCEPTED + Piep. Die Reihenfolge msg 4 -> ACCEPTED ist
  die Original-Reihenfolge von sub02 (@0x00C90/@0x00C94 vor @0x00C96), die Spur hat sie nicht veraendert.
* **H5 (Pegel, Information):** Die Datei des Nutzers ist mit Spitze 0,92 die lauteste der 188 lesbaren synchro-WAVs
  (Median 0,40, naechste 0,64). Der Lader hebt nur leise Dateien an und laesst laute unveraendert (dokumentierte Regel,
  synchro/README.md) -> im Mitschnitt Spitze 32677 waehrend der Aufnahme (kein Uebersteuern: 0 Proben an der Grenze),
  Musik/Toene im Raum davor hoechstens 8153. Die Spur hat die Datei bewusst unveraendert gelassen (Wortlaut "nutze ...
  den Sound"); wirkt sie zu laut, ist der Hebel die Datei selbst, kein Code.

## 9. Belege / Dateien
* Logs + Mitschnitte (rk/auf/cut/stimme/debug/state.log, cap.raw bzw. cap_rt.raw): `re15_port/build/r38j_abnahme0/runs/
  {m2_leon_langsam,b2_v0825_langsam,m3_leon_schnell,m4_elza_langsam,m5_echtzeit_schnell}`; Auswerter und Eingaben
  `re15_port/build/r38j_abnahme0/werkzeug/` (lauf.sh, weg.py, diffana.py, lage.py, sheet.py, sca_dump.py, P.npy =
  rekonstruiertes Tonpaar). Nicht versioniert.
* Bilder: `re15_port/build/r38j_abnahme0/abn0_leon_echtweg_ablauf.png` (F793..F997), `abn0_leon_schnell_warten.png`
  (F800..F895), `abn0_leon_schnell_nachhall.png` (F816..F844, H2) — nicht versioniert.
* ctest-Protokolle: `re15_port/build/r38j_abnahme0/ctest_{unit,integration,gezielt}.log`.
