# Runde 38 — Spur H "elzatoene": Abnahme 0 (unabhaengig)

Datum: 2026-10-16. Baum `.claude/worktrees/r38_elzatoene`, Zweig `r38/elzatoene`, Stand 8328ae1f (+ bccfcb4f = Teilbericht
dieser Abnahme), Basis e2cc7673. Pruefer: Abnahme 0 (kein Code geaendert; nur diese Datei committet).
Messordner (ausserhalb des Baums, Scratchpad `ab0h/`): Laeufer `lauf.sh`, eigene Auswertung `py/` (eigener PSX-ADPCM-
Decoder `kreuz.py`/`kreuz_np.py`, `differenz.py`, `klang.py`/`klang2.py`, `re_gate.py`, `vag_suche.py`, `tim2png.py`),
v0.8.25-Paket-Kopie `v0825/` (exe md5 7aa892fc.. = release/pkg-win/re15_port_v0.8.25/re15_pc.exe), Laeufe `runs/<name>/`.

**Ergebnis: NICHT bestanden.** P1 und P2 erfuellt. P3 teilweise: der neue Ton spielt und ist ein RE2-Skript-Schuss, aber
die RE-Begruendung der Spur fuer den Befund ist durch die RE2-Bytes widerlegt — der alte Ton (ROOM40A0 Satz 0x08) ist mit
hoher Wahrscheinlichkeit selbst ein RE2-Schuss (Partnerin feuert, Muendungsfeuer-ESP im selben Schleifenkoerper), und er
klang in v0.8.25 gemessen schon in Sprachpegel. Der Fix erklaert den Befund des Nutzers damit nicht (Maengel 1-4).

## 0. Stand / Bau
- `git log master..HEAD`: 11 Commits (1468730f .. 8328ae1f + Teilbericht bccfcb4f). master ist seit e2cc7673 nur um
  Doku-/Orchestrator-Commits gewachsen; Spur-Aenderungen = `git diff master...HEAD` (20 Dateien inkl. dieses Berichts).
- Bau: `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` (17:5x und erneut
  21:0x) -> `ninja: no work to do`, `LOCAL-BUILD-OK (build)`. exe 17:18, md5 b7bab4ef..; Messkopie
  `re15_pc_ab0h.exe` (bytegleich). Letzter Code-Commit c08a9a47; spaetere Commits nur Doku/Tests.
- ELZASE.VBS neu: 14232 B, md5 ba96a662.. (= Dossier). Gegen master (13144 B, md5 8e996ef2..): EDT/VH-Teil unterscheidet
  sich nur in 8 Bytes (0x2C-0x2D Kopf, 0x862-0x867 Ton von Satz 1, 0xA44 VAG-Groesse); Satz 0 (AUF) EDT `00 00 04 16` und
  VAG 0 (5264 B) bytegleich; VAG 1 bytegleich RE2 ROOM4040 @0x18CC8 (5824 B). v0.8.25-Pakete win/linux/apk tragen
  ELZASE.VBS (13144 B) — ein Paketfehler erklaert den Befund nicht.
- ctest -N: 796 Tests (v0.8.25 = 788, +8 = 6 Unit + 2 Integration).

## 1. Code-Diff (gelesen)
- `engine/src/elzatoene_r38.c` + `include/re15_elzatoene_r38.h` (neu): Anker-Tabelle (Raum, Datei-Offset, 8-Byte-Signatur,
  Aktion); Scan beim Raumladen, Opcode-Haken (nur Aktion 2, Roys Plc_dest), Message_on-Haken (nur Aktion 1: Ton statt Text,
  Aufrufer `pc += 4`). Einziges getenv = `RE15_ELZA_LOG` (Messschiene, kein Verhaltensschalter).
- `scd_vm.c`: Include + 3 Haken-Zeilen + Kommentare. Message_on-Haken liegt HINTER Speicher-/Box-/Auswahl-Zweigen und dem
  parkenden Stimmen-Riegel; davor wird kein Zustand gesetzt, der aufgeraeumt werden muesste. `op_plc_dest` (scd_vm.c:2514)
  parkt nie (`t->pc += 8; return 1`) -> der Opcode-Haken feuert je Ausfuehrung genau einmal.
- `tools/r36_l/re2_elza_se_cut.py`, `gen/re2_elza_bank.inc`, `shared_assets/RE2/ELZASE.VBS`: Satz 1 ROOM40A0 0x08 ->
  ROOM4040 0x0A. `elza_se_pc.c`, `re15_elza_r36.h`: nur Kommentare. `RE15_ELZA_TON_SCHUSS` wird nur von den 1021-Ankern
  (Runde 36) und jetzt vom 1041-Anker benutzt -> keine weiteren Raeume betroffen.
- Tests: `tests/unit/test_r38_elzatoene.c` (6 Teile), `tests/integration/test_r38_elzatoene.cmake` (2 Teile),
  `tests/unit/probe_r38_elzatoene_kreuz.c`, registriert nur in `tests/unit/probes/r38_elzatoene.cmake` (GLOB
  tests/unit/CMakeLists.txt:4060). Nachgezogen: `test_r36_elza.c` (Teil bank: Quelle Satz 1), `test_r36_elza.cmake`
  (Teil schuss: Bezug = Spitze der 28 Bilder vor Schuss 1 statt Raum-Median; Untergrenze 3000/1500 — ein fehlender Schuss
  faellt weiter auf).
- Rate-Woerter im Code-Diff (`deferred|tunable|interim|for now|faithful|plausib|TODO|vermutlich|gedeutet`): keine.

## 2. Gezielte Regression (keine volle Suite)
`ctest --test-dir re15_port/build -R "scd|msg_dialog|text_freeze|synchro_stimme|voice_ausreden|se_voice_machine|leiche|r36_elza|r38_elzatoene|r37_roy|gegnerzahl_raum1041|story_elza|elza_vollstart|elza_zweig|codepanel|lift2030|savepoint|itembox" --timeout 1200 --output-on-failure -j2`
(21:09-21:20) -> **`100% tests passed, 0 tests failed out of 69`** (649 s), darunter integration_r38_elzatoene_gunshot1041
(89,6 s), integration_r38_elzatoene_szene1021 (76,0 s), integration_r36_elza_schuss (62,9 s), alle 6 unit_r38_elzatoene_*.

## 3. Messung je Nutzer-Punkt (echte exe, echter Weg)

Laeufer `ab0h/lauf.sh <exe> <name> <fall>`: Spielstand mit `probe_r36_elza_karte` (Elza), CONTINUE
(`RE15_NO_INTRO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0`), `RE15_WINDOW_SCALE=1`, `RE15_STATE_LOG`,
`RE15_AUDIO_CAP_SYNC` (1470 Stereo-Frames je Bild), `RE15_ELZA_LOG`, `RE15_SE_DEBUG=1`, `RE15_FRAMEDUMP`, Eingabe ab Bild 60.
- `g1041`: Spielstand ROOM1031 an Tuer 2, `RE15_FIRE_AOT=2@40#1031` (Tuer 1031 -> 1041), `RE15_INPUT_SCRIPT=U2.5` (Elza geht
  ins Auto-Feld AOT 8), `RE15_EXIT_AT=500#1041`.
- `s1021`: Spielstand ROOM1041 (Flags 4:0x60 3:0xC9 4:0xF1 3:0xAD), `RE15_FIRE_AOT=3@40#1041` (Tuer 1041 -> 1021),
  `RE15_INPUT_SCRIPT=L0.25,U0.8,R0.25,U6` (ins Auto-Feld AOT 5), `RE15_EXIT_AT=1300#1021`.
- "neu" = Spur-exe (Messkopie re15_pc_ab0h.exe, md5 b7bab4ef..), "alt" = v0.8.25-Paket (re15_pc_ab0h_v0825.exe mit den
  Paket-Assets, md5 7aa892fc..). Beide Laeufe je Fall mit identischer Eingabe; die Mitschnitte sind bis zum ersten
  Spur-Ereignis BITGLEICH (Differenz 0, `py/differenz.py`) -> die Differenz neu-alt ist genau das, was die Spur hinzufuegt.

### P1 — ROOM1041 "*gunshot*" ohne Schuss-Ton -> **erfuellt**
| Lauf | Beleg |
|---|---|
| alt_g1041 | state.log: msg 3 offen F83..F131 (`msg(a=1 fsm=1 id=3)`), msg 2 F144..F242; debug.log: KEIN Schuss-[se] — v0.8.25 zeigt nur den Text |
| neu_g1041 | elza.log `F40 [elzatoene] ROOM1041: 1 Anker armiert`, `F83 [elzatoene] ROOM1041 Anker @0x02200 -> Aktion 1`; debug.log `[se] Stimme: se=1 layer=0 vag=1 note=66 fine=57 center=84 vol=110 pitch=0x5f3 (16397 Hz) -> SE-Stimme -16 (SPU 0)`; state.log: msg 3 NIE offen, msg 2 F144..F242 (Zeitform unveraendert) |
| Kreuzkorrelation (eigener Decoder, `kreuz_np.py runs/neu_g1041/cap.raw 500 re2:ROOM4040.RDT:0x0A 83`) | **0,900** bei Versatz 0,00 Bilder, Gegenprobe 60 Bilder frueher 0,063; Spitze 13113 gegen Raum ~2000 |
| Bild | `ab0h/g1041_streifen.png`: alt F84/F100/F130 "*gunshot*", F150 "Elza: What was that!?"; neu F84/F100/F130 ohne Text, F150 "What was that!?" |

Der Platzhalter ist zum Ton geworden (VERTRAG §0 Spur H "Schuss-Ton ROOM1041 (statt Platzhalter "gunshot")"), Elzas
Reaktion bleibt im Original-Takt. Leon ROOM1040: kein Anker (unit signatur/gunshot1041).

### P2 — Roy stoehnt beim Zugehen auf John (ROOM1021) -> **erfuellt**
| Lauf | Beleg |
|---|---|
| neu_s1021 | elza.log `F788 [elzatoene] ROOM1021 Anker @0x020B2 -> Aktion 2`; debug.log direkt davor `[se] Stimme: se=4 layer=0 vag=4 note=64 ... (11025 Hz) -> SE-Stimme 5 (SPU 21)`, danach `[scd] Plc_dest(slot=2 mode=0x04 dest=(-4274,-6590))` |
| Differenz neu-alt (`differenz.py ... 1300 760 850`) | bis F788 0; ab F789 nur das Stoehnen: F789-F797 5..23 dB unter Johns Zeile main03 (Ende ~F808), F798-F837 dominierend, F809-F837 ALLEIN (RMS bis 2218, Spitze bis 7797) |
| Kreuzkorrelation (`r15:ROOM1021.RDT:1:4 788 600 600`) | **1,000** bei Versatz 1,00 Bild, Gegenprobe 0,068; im alt-Lauf 0,098 (kein Stoehnen) |
| Bild | `ab0h/s1021_streifen.png`: F780-F800 Roy im Vordergrund (John: "Roy, it's me..."), ab F820 geht Roy den Gang hinunter auf John zu |

Welle: ROOM1021 snd1 SE 4 `00 00 5b 15` = sha1 a5234f0ba034 (10304 B), bytegleich SE 4 in ROOM1000/1010/1020/1060/10D0
(selbst gelesen) = das Zombie-Stoehnen der Raumbaenke. Form/Takt belegt (FUN_801021f8, §4). 1/4-Wurf entfaellt
(NUTZER-VORGABE "soll stoehnen"), SE 4 statt Wurf 4/5 (PORT-WAHL, SE 5 = Roys Sterbe-Ton) — beide gekennzeichnet.

### P3 — Johns Schuesse in ROOM1021 -> **teilweise**
| Lauf | Beleg |
|---|---|
| alt_s1021 (v0.8.25) | elza.log `F939 [elza] ROOM1021 Anker @0x020CE -> Aktion 2`, `F969 ... @0x020D6 -> Aktion 2`, `F969 ... @0x020DE -> Aktion 3`; debug.log `[se] ... vag=1 note=69 fine=109 center=91 vol=100 pitch=0x4f3 (13641 Hz) -> SE-Stimme -16` zweimal; Kreuzkorrelation ROOM40A0 0x08 bei F939 **1,000**; Pegel F939 RMS 2568 / Spitze 6045 in STILLEM Raum (F932-F938 = 0), Abklingen bis F954 |
| neu_s1021 | dieselben Anker/Bilder; `[se] ... note=66 fine=57 center=84 vol=110 pitch=0x5f3 (16397 Hz)` zweimal; Kreuzkorrelation ROOM4040 0x0A bei F939 **1,000** / F969 **0,982**, Gegenprobe 0,000; gegen die alte Welle 0,305; Pegel F939 RMS 2576 / Spitze 7062, Abklingen bis F957 |

Was erfuellt ist: im Bild jedes Feuer-Clips (+1) klingt jetzt der RE2-Skript-Pistolenschuss (ROOM4040 Satz 0x0A — §4: in
4040/4090/6020 jeweils Se_on + ESP 0x18 "GUN"; dieselbe Welle nur in Schuss-Raeumen 4040/4090/6020/6110/D040/F110,
`py/vag_suche.py`). Was NICHT belegt ist: dass damit der Befund des Nutzers behoben ist.
- v0.8.25 spielte an genau diesen Bildern bereits einen Ton in Sprachpegel in einem stillen Raum (oben). Der Nutzer meldete
  trotzdem "muss auch ein Schusssound kommen" (zum zweiten Mal: Runde 36 AUFTRAG Zeile 47).
- Die Spur begruendet das mit "der Runde-36-Ton ist KEIN Schuss (Alligator-Schleife)". Das ist durch die RE2-Bytes
  widerlegt (Mangel 1): der alte Ton ist mit hoher Wahrscheinlichkeit der Schuss der Partnerin in RE2s Alligator-Szene.
- Pegel und Klangklasse alt/neu liegen nah beieinander (Spur selbst, OFFEN 4; eigene Messung: RMS 2568/2576; Baender
  tief/mittel/hoch alt 0,35/0,46/0,19, neu 0,50/0,45/0,05; spektrale Flachheit alt 0,698, neu 0,495, Elzas Browning 0,417).
  Der neue Ton ist etwas "dumpfer/weniger rauschig" — ob der Nutzer ihn als Schuss hoert, wo er den alten nicht hoerte,
  ist damit nicht gezeigt.

## 4. RE-Gate (selbst disassembliert / Bytes selbst gelesen)
1. **STAGE1.BIN FUN_801021f8** (`re15_disasm.py dis 0x801021f8 45 --bin STAGE1.BIN`; Tabelle @0x8011f890[2] = 0x801021f8):
   `80102210 lbu v0,6(v1)` / `80102218 bne v0,zero,0x801023b0` (nur beim Eintritt) / `80102270 jal 0x8001af20` /
   `80102278 andi v0,v0,0x3` / `8010227c bne v0,zero,0x801022a4` / `80102284 jal 0x8001af20` / `8010228c andi v0,v0,0x1` /
   `80102290 beq v0,zero,0x8010229c` (Delay `ori a0,zero,0x5`) / `80102298 ori a0,zero,0x4` / `8010229c jal 0x800453d0`.
   **Stimmt** (Stoehnen 4 oder 5 beim Gang-Eintritt, 1/4).
2. **ROOM1041.RDT** (= shared_assets, md5 39e286e8..): @0x021EA `22 03 ad 01` Set(3,0xAD) .. @0x02200 `2b 03 00 00 2e 01 00 00`
   (Message_on 3 + Work_set(1,0)), @0x02224 `2b 02 00 00`; msg 3 @0x022FB = `<04><00>*gunshot*<04><01><01>`; main00
   @0x0116C `21 04 f1 00`, @0x01174 `21 03 ad 00`, @0x01178 `2c 08 03 41 ..` (AOT 8). **Stimmt** (Kleinigkeit: das Dossier
   zitiert als 15. Byte `00`, die Datei hat dort `31` = Folgebyte).
   **ROOM1021.RDT** (md5 480d807b..): @0x020AE `2e 02 01 00`, @0x020B2 `40 00 04 22 4e ef 42 e6` (Plc_dest Modus 4 nach
   (-4274,-6590) = Johns Platz, Sce_em_set @0x01DF6 `44 00 49 40 .. 4e ef 00 00 42 e6`), @0x020BE Sleep 40, @0x020CA Sleep 110,
   @0x020CE/@0x020D6 `3f 02 07 00`, @0x020DE `3f 01 07 00`; snd1 EDT @0x145A8 Satz 4 `00 00 5b 15`. **Stimmt.**
3. **RE2 ROOM4040.RDT**: @0x014BE `2e 01 00`, @0x014D6 `3f 00 11 00`, @0x014E4 `36 02 0a 00 03 01 ..` (Se_on Satz 0x0A,
   Art 3 Index 1), @0x014F0 `3a 00 18 00 00 00 00 10 ..` (ESP 0x18), @0x01528/@0x01534 dasselbe; EDT @0x0BF60 `00 00 73 00`;
   Ton @0x0C8F8 `00 04 6e 40 54 39 42 42`; VAG @0x18CC8 5824 B sha1 994b137d4007..; ROOM4090 @0x02A4E `36 02 08 01 ..` +
   @0x02A68 `3a 00 18`; ROOM6020 @0x00B94 `36 02 0a 01 ..` + @0x00BA4 `3a 00 18`. **Stimmt.**
4. **RE2 ROOM40A0.RDT** (Begruendung der Korrektur): @0x025A0 `44 00 00 23` stimmt; Schleife @0x0287E `2e 02 00` /
   @0x02881 `3f 02 0e 04` / @0x0288A `0f 04 82 00` / @0x0288E `21 05 0b 00` / @0x02896 `36 02 08 01 02 00 ..` stimmt.
   **Die Schlussfolgerung stimmt NICHT** — siehe Mangel 1.

Konstanten im Diff: alle mit @0x/Datei-Offset oder als NUTZER-VORGABE/PORT-WAHL gekennzeichnet (Offsets/Signaturen
0x02200/0x020B2, SE 4, Satz ROOM4040 0x0A mit EDT/Ton/VAG-Offsets). Gate "Fix erklaert den Befund": **nicht erfuellt**
(Maengel 1, 2).

## 5. Vertrag
- Pfade (`git diff master...HEAD --name-only`): keine release/, platform/android/, shared_assets/PSX/, keine
  tests/**/CMakeLists.txt (Registrierung nur probes/r38_elzatoene.cmake). shared_assets/RE2/ELZASE.VBS ist die Mini-Bank der
  Runde-36-Elza-Toene (nicht PSX/*), erzeugt vom Werkzeug der Spur.
- Gemeinsame Dateien: scd_vm.c 1 Include + 3 Haken-Zeilen mit Kommentar "Runde 38 Spur H" (VERTRAG §1.4). Keine
  Bank-9-Bits, keine neuen Nachrichten-Ids, keine Item-Ids.
- Riegel je Punkt vorhanden und messend (Unit am VM mit Gegenproben ohne Anker; Integration an der echten exe mit
  Kreuzkorrelation >= 0,7 / Gegenprobe <= 0,2). Ausnahme: unit_r38_elzatoene_re2beleg prueft eine falsche Behauptung
  selbstbestaetigend (Mangel 3).
- Nebenwirkungen: Leon-Raeume ohne Anker (Raum-Id-Schalter, Signatur); ELZASE Satz 1 nur in 1021/1041; Zeitform 1041
  unveraendert (msg 2 F144 alt = neu); Mitschnitte alt/neu bis zum ersten Spur-Ereignis bitgleich; 69/69 gruen.

## 6. Maengel

1. **RE-Grundlage von P3 widerlegt (ROOM40A0 Satz 0x08 "KEIN Schuss, Alligator-Schleife").** Die Spur stuetzt die
   Korrektur auf (a) "einziger Gegner = Alligator, sonst kein Sce_em_set in der Datei" und (b) "Work_set(2,0) @0x0287E
   waehlt also den Alligator; Se_on Ursprung Modus 2 Index 0 = am Alligator". Selbst gelesen in info/re2leon/PL0/RDT:
   - ROOM40A0.RDT hat FUENF Sce_em_set: @0x02564 `44 00 ff 41` (Ada), @0x02586 `44 00 ff 45`, @0x025A0 `44 00 00 23`
     (Alligator), @0x02612 `44 00 ff 41`, @0x0262C `44 00 ff 41`.
   - Der Alligator wird im selben Raum mit Art **3** angesprochen: sub11 @0x02CDC `2e 03 00` (Work_set(3,0)), @0x02CFE
     `22 05 02 01` Set(5,2), @0x02D02 `36 02 0a 01 03 00 ..` Se_on Art 3 Index 0. Art 3 = Gegner auch in ROOM4040: Annette
     = Sce_em_set Slot 1 @0x00E74 `44 00 01 47`, `2e 03 01` @0x012FE/@0x01378/@0x0144C/@0x01490, ihr Skript-Schuss
     @0x014E4 `36 02 0a 00 03 01` = Art 3 Index 1.
   - Die Schleife @0x0287E benutzt Art **2** (das einzige `2e 02 00` des Raums; Se_on Art 2 @0x02896 und @0x0291E), wartet
     danach @0x029A0 `21 05 02 00` auf das (5,2) des Alligator-Skripts, und setzt direkt nach dem Se_on @0x028A2
     `3a 00 18 ..` ESP 0x18 — esp18.tim in room40A0 und room4040 bytegleich (md5 fd8fe615..), Blatt beschriftet
     "GUN 5.00" (Muendungsblitz/Leuchtspur) = dasselbe ESP, das in 4040/4090/6020 auf den Skript-Schuss folgt — und nach
     Sleep 2 @0x028C6 `3a 00 1a 05 ..` ESP 0x1A (esp1A.tim "MIZ 7.00" = Wasserspritzer).
   => Satz 0x08 ist mit hoher Wahrscheinlichkeit der Schuss der PARTNERIN (Art 2 = die Sce_em_set mit Slot 0xFF; Name
   "Splayer" nach RE2-Konvention, nicht in der EXE nachgeprueft) auf den Alligator im Wasser — also ein Schuss. Die
   RE1.5-Zuordnung "Art 2 = Gegner" (Port: Work_set(2,1) = Roy) gilt in RE2 nicht. Runde 36 ("Pistolenschuss eines NPC +
   Muendungsfeuer @0x028A2") lag bei der Art des Tons richtig.
   Nachpruefen: `py/re_gate.py`, Bytes oben; `py/tim2png.py room4040/esp18.tim` -> `ab0h/esp18_4040.png`.
2. **Befund P3 nicht erklaert, Wirksamkeit nicht belegt.** v0.8.25 spielte an F939/F969 bereits einen RE2-NPC-Schuss
   (Korrelation 1,000) in Sprachpegel (RMS 2568) in stillem Raum; der neue Ton hat denselben Pegel (RMS 2576) und dieselbe
   Klangklasse (Spur OFFEN 4). Warum der Nutzer den alten Ton nicht als Schuss gehoert hat, ist offen; dass der Tausch
   gegen einen anderen RE2-NPC-Schuss das behebt, ist nicht gezeigt. Gefordert: die tatsaechliche Ursache belegen (z.B.
   Klangvergleich mit dem Schuss, den der Nutzer kennt — Elzas/Leons Waffe, RE2 ARMS02 SE0 = RE1.5 ARMS03 SE0 —, Lage/Pegel,
   Einbettung in RE2: Se_on-Byte 3 = 1 bei 40A0 gegen 0 bei 4040) und zeigen, dass der neue Stand genau diese Ursache
   beseitigt — oder die Wahl des Tons neu begruenden.
3. **Riegel unit_r38_elzatoene_re2beleg selbstbestaetigend.** test_r38_elzatoene.c teil_re2beleg zaehlt Sce_em_set nur mit
   `a0[i + 2] < 16` und meldet "einziger Gegner (Alligator)" bei n_em == 1 — der Filter schliesst genau die vier
   Slot-0xFF-Saetze (@0x02564/@0x02586/@0x02612/@0x0262C) aus; die Meldung "Se_on(2,8) @0x02896 (Ursprung 2/0)" wird als
   "am Alligator" gelesen. Der Riegel muss die tatsaechliche Lage pruefen (Art 3 = Alligator in sub11, Art 2 + ESP 0x18 in
   sub04) bzw. die Behauptung fallen lassen.
4. **Falsche Behauptung als Tatsache in Code/Werkzeug/Dossier.** "kein Schuss"/"Alligator-Schleife"/"am Alligator" steht in
   platform/pc/src/elza_se_pc.c (Kopfkommentar Satz 1), include/re15_elzatoene_r38.h (Kopf, P3), tests/unit/test_r36_elza.c
   (teil_bank), tools/r36_l/re2_elza_se_cut.py (Docstring Satz 1), analysis/befunde_runde38/H_elzatoene.md §2.3/§2.4/§6
   (OFFEN 4) — mit Mangel 1 berichtigen.

## 7. Hinweise (nicht blockierend)
- P1: Ton nicht positional (Spur OFFEN 3) — der ferne Schuss klingt wie ein naher (-21 dB(A)). Wortlaut verlangt nur den Ton.
- P2: Das Stoehnen beginnt 10 Bilder vor dem Ende von Johns Zeile main03 (F789-F798 darunter), ist danach 0,93 s allein
  hoerbar (F809-F837).
- Die RE2-Folge-Welle (Spur OFFEN 2) und das Muendungsfeuer zu Johns Schuessen (OFFEN 1) sind nicht beauftragt.
- Dossier-Zitat msg 3: 15. Byte `00` statt `31` (s. §4.2).

## 8. Urteil
| Punkt | Urteil |
|---|---|
| P1 ROOM1041 Schuss-Ton bei "*gunshot*" | erfuellt |
| P2 ROOM1021 Roys Zombie-Stoehnen beim Zugehen | erfuellt |
| P3 ROOM1021 Schuss-Ton bei Johns Schuessen | teilweise (Ton spielt, Befund nicht erklaert, RE-Begruendung widerlegt) |
| Gates | Bau ok, gezielte Regression 69/69, Vertrag ok; RE-Gate "Fix erklaert Befund" NICHT erfuellt; Riegel re2beleg selbstbestaetigend |

**bestanden = false** (Maengel 1-4).
