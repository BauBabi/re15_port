# Runde 38 — Spur J "rekorder" (Dossier, fortlaufend)

Baum `.claude/worktrees/r38_rekorder` (Zweig `r38/rekorder`, Basis 1bcbd0ff). Auftrag: `AUFTRAG.md` (Wortlaut Zeile 25),
Vertrag `VERTRAG.md` Spur J (Nachrichten-Ids ROOM1100/1101 40-44).

## Wortlaut (AUFTRAG.md, unveraendert)

> Nutze beim Rekorder Rätsel beim reinsprechen den Sound Irons_Rekorder_Sound.wav aus dem synchro Ordner, und nachdem er
> fertig abgespielt ist, machst du den Piep Sound für Erfolgreiche Eingabe und den Tür öffnen Sound. Das zusammen mit der
> erfolgreich message auf den Panel.

## 0. Stand

**Fertig.** Aufnahme des Nutzers beim Druck aufs Mikrofon (Takt von Message_on 4 @0x00C90), sub02 wartet an @0x00C96 bis zu
ihrem Ende, Piep im Takt des ersten ACCEPTED (Cut_chg 11 @0x00C9C), Aufschliessen +15, "You've opened the lock." ohne
zweite Folge, Tuer frei - Leon ROOM1100 und Elza ROOM1101, echte exe + headless gemessen (§4), Riegel §5, gezielte Laeufe
`100% tests passed, 0 tests failed out of 55`. Abschnitte: 1 RE-Belege / 2 Messung vorher / 3 Lesart + Umsetzung /
4 Messung nachher / 5 Tests / 6 OFFEN / 7 Fuer den Nutzer.

---

## 1. RE-Belege (selbst gelesen)

### 1.1 Das Raetsel im Original — ROOM1100.RDT == ROOM1101.RDT (`cmp`: bytegleich, Leon/Elza)

Werkzeuge: `re15_port/tools/scd_dump_room.py` (Opcode-Walk), `re15_port/tools/rdt_msgdump.py` (Texte), Rohbytes per Python.

Nachrichten (msg_sec @0x0D9C): 1 @0x0DC7 "It's a control panel. A type of voice device is required / to open the lock.",
2 @0x0E16 "You've opened the lock.", 3 @0x0E31 "Will you use the Minidisc Player w/ Disc?" (Ja/Nein, ctrl `03`),
4 @0x0E64 "You've used the Minidisc Player w/ Disc.", 5 @0x0E94 "I don't need to use this anymore."

Ablauf (Opcode-Walk + Rohbytes):
```
sub00 @0x00AC4 Ifel Ck(3,0x67)==0 -> Platz 2 sce 1 (msg 1)         [Minidisc noch nicht genommen]
      @0x00B02 elif Ck(4,0xE8)==0 -> @0x00B14 Aot_set Platz 2 sce 3 `.. ff 00 18 07` (Ereignis sub07),
                 @0x00B3C/@0x00B50 Platz 4/5 sce 5 flags 0x44 (Cursor-Raster), @0x00B64 Obj_model_set type 4 (Cursor)
sub07 @0x00D50 `2b 01 ff ff` msg 1 / @0x00D56 `2b 03 ff ff` msg 3 (Frage) / @0x00D5C Ifel Ck(12,31)==0 (Ja):
      @0x00D64 `29 0c` Cut_chg 12 (Anzeige leer), @0x00D66..0x00D7A Set(5,0..4,6)=1 (Cursor frei)
sub01 @0x00C04 Ck(5,4,1) Member_cmp(0,0x0F,4) Sce_key_ck(1,0x40) -> Evt_exec sub08
      @0x00C2C `06 00 24 00 21 05 05 01 2e 03 00 00 06 00 16 00 3e 00 0f 00 05 00 06 00 0a 00 51 01 40 00 04 ff 18 02`
             = Ck(5,5,1) Work_set(3,0) Member_cmp(0,0x0F,5) Sce_key_ck(1,0x40) -> Evt_exec sub02   ["reinsprechen"]
sub08 @0x00D82 `22 05 04 00 29 07 09 0a 1e 00 22 05 05 01 29 08 01 00` = Set(5,4,0) Cut_chg 7 Sleep 30 Set(5,5,1) Cut_chg 8
sub02 @0x00C68 Set(4,0xE8,1) ... @0x00C8C `22 02 07 01` Set(2,7,1) (Spieler gesperrt)
      @0x00C90 `2b 04 ff ff`  Message_on 4 "You've used the Minidisc Player w/ Disc." (Pausemaske 0xffff)
      @0x00C94 `02 00`        Evt_next + Nop (wartet, bis die Nachricht zu ist)
      @0x00C96 `29 0c` @0x00C98 `09 0a 05 00`   Cut_chg 12 (leer), Sleep 5
      @0x00C9C `29 0b` @0x00C9E `09 0a 05 00`   Cut_chg 11 (ACCEPTED), Sleep 5     <- erstes ACCEPTED
      @0x00CA2 `29 0c` / @0x00CA8 `29 0b` / @0x00CAE `29 0c` / @0x00CB4 `29 0b`, je Sleep 5 (ACCEPTED blinkt 3x)
      @0x00CBA `2b 02 ff ff`  Message_on 2 "You've opened the lock."
      @0x00CBE `02 00` Evt_next / @0x00CC0 `22 02 07 00` Set(2,7,0) / @0x00CC4 `29 06` Cut_chg 6 / @0x00CC6 `3c 01` Cut_auto 1
      @0x00CC8 Aot_reset Platz 1 sce 2 (Tuer nach ROOM1110 frei, main00 @0x009BA), Platz 2 sce 1 (msg 5), Platz 3 sce 0
```
Bedeutung der Cuts 7..12 (Anzeige des Panels): Runde 37 E §0 "SYSTEM READY / SAY HELLO / INVALID / TRY AGAIN / ACCEPTED /
leer" — am Hintergrund selbst nachgesehen in §2 (Messung).

RE1.5 spielt in dem ganzen Ablauf KEINEN Ton (kein `36` Se_on in sub02/sub07/sub08); seit Runde 36 Spur G haengt die
RE2-Folge Piep (+0) / Aufschliessen (+15) an Message_on 2 (`gen/r36_aufschliessen.inc`: `{ 0x1100, 2, KARTE_AUF, 2 }`,
`{ 0x1101, 2, KARTE_AUF, 2 }`, "sub02 @0x00CBA").

### 1.2 RE2-Form fuer "Piep + Erfolgsanzeige im selben Takt, Aufschliessen danach" (ROOM2110 sub08, selbst gelesen)

`info/re2leon/PL0/RDT/ROOM2110.RDT`:
```
@0x01B2C 29 0c                            Cut_chg 12 (Kamera auf den Leser)
@0x01B4C 36 02 0c 00 ..                   Se_on(2,0x0C) Karte ein
@0x01B58 09 0a 0f 00                      Sleep 15
@0x01B5C 36 02 0d 00 ..                   Se_on(2,0x0D) PIEP
@0x01B68 65 01 65 02                      Lampen aus                  } im SELBEN Skript-Takt wie der Piep
@0x01B6C 64 01 16 08 .. / @0x01B7C 64 02 16 08 ..  Lampen BLAU (offen)  } (kein Sleep dazwischen)
@0x01B8C 09 0a 0f 00                      Sleep 15
@0x01B90 36 02 0b 00 ..                   Se_on(2,0x0B) AUFSCHLIESSEN
@0x01B9C 29 09 3c 01                      Kamera zurueck
```
Die Saetze liegen seit Runde 36 als Mini-Bank `shared_assets/RE2/KARTSE.VBS` (`gen/re2_karten_bank.inc`):
Satz 1 PIEP = RE2 ROOM2110 Satz 0x0D (VAG 4640 B, sha1 b03b6dff...), Satz 2 AUF_E = RE2 ROOM2110 Satz 0x0B (VAG 11264 B,
sha1 da805fa3...). Dieselben Saetze und derselbe Abstand 15 spielen die Kartenleser und Code-Panels
(`aufschliessen_r36.c` k_folgen[KARTE_AUF] = { PIEP +0, AUF_E +15 }, Sleep-Semantik RE2 @0x800539dc/@0x80053a24).

### 1.3 Die Ton-Datei des Nutzers

`synchro/STAGE1/room1100/Irons_Rekorder_Sound.wav` (169458 B, committet e2cc7673): RIFF/WAVE, `fmt ` @12 Laenge 16:
Tag 1 (PCM), 1 Kanal, 32000 Hz, 16 bit; `data` @36, Laenge (u32 @40) 169414 B = 84707 Samples = **2,647 s =
79,4 Spielbilder** (30 Hz). Spitze 30146 (92 % Vollaussteuerung -> der Lader hebt nichts an, audio_pc.c Ziel 14745).
Sie liegt NICHT im Schema `main<NN>.wav` (synchro/README.md) -> eigene Bindung noetig; `unit_synchro_stimme` meldet sie
bisher als "noch nicht zugeordnet".

---

## 2. Messung vorher (Stand 1bcbd0ff, echte exe, ECHTER Weg)

Messkopie `re15_port/build/platform/pc/re15_pc_r38j_mess.exe`, Laeufer `re15_port/build/r38j/lauf.sh` (nicht versioniert),
Laeufe `re15_port/build/r38j/runs/<name>/`. Weg: `RE15_DEBUG_JUMP=1100@120`, Minidisc genommen + im Inventar
(`RE15_SET_FLAG=3:0x67`, `RE15_GIVE=0x44:1`; K6 der Runde-37-Kistenregel), Panel per `RE15_FIRE_AOT=2@150#1100`
(Platz 2 sce 3 -> sub07), Viereck alle 10 Bilder 170..360 (msg 1, msg 3 "Ja"), Cursor per D-Pad (LEFT 15 / DOWN 19 Bilder,
Viereck F406 = ON/OFF-Schalter; RIGHT 30 / DOWN 16, Viereck F498 = Mikrofon-Gitter), Viereck alle 10 Bilder 530..690.
Hintergruende der Cuts 6..12 (Port-MDEC-Dekoder `probe_bg_dump`): `J_belege/vus_1100_cuts6_12.png` — Cut 7 SYSTEM READY,
8 SAY HELLO, 9 INVALID, 10 TRY AGAIN, **11 ACCEPTED**, 12 leer (bestaetigt Runde 37 E §0).

Ablauf (`RE15_SCD_TRACE`, Lauf `v3_weg`; Lauf `v4_vorher` mit denselben Eingaben bildgleich):
```
F151 sub07 msg 1 / F282 msg 3 (Frage) / F363 Ja: @0x0D64 Cut_chg 12, Cursor frei (bg 1100#12 ab F364)
F407 sub08 (ON/OFF): @0x0D86 Cut_chg 7 SYSTEM READY, Sleep 30 / F437 @0x0D90 Cut_chg 8 SAY HELLO
F499 sub02 (Mikrofon): @0x0C68.. Set(4,0xE8,1) .. @0x0C90 Message_on 4 "You've used the Minidisc Player w/ Disc."
     (Pausemaske 0xffff: die VM steht, bis die Nachricht zu ist - kein Trace F500..F670)
     msg 4 tippt bis ~F575 (`04 02` = 2 Bilder je Zeichen, 40 Zeichen), danach die Runde-21/22-Wegwerf-Abfrage
     "You don't need this item any more. Discard it?" (Framedumps F580..F660, Bild msg4_verlauf), "Ja" mit Viereck F670
F671 @0x0C96 Cut_chg 12 (leer) / F676 @0x0C9C Cut_chg 11 ACCEPTED (angezeigt ab F677) / blinkt bis F700
F701 @0x0CBA Message_on 2 "You've opened the lock."    auf.log: F701 PIEP (+0) / F716 AUF_E (+15)
```
Ton (`RE15_AUDIO_CAP_SYNC`, `tools/tueren/r36_cap_ticks.py`, Versatz Tick = F + 123): von Tick 622 (F499) bis Tick 823
nur Musik (Anteil unter 250 Hz 0,3..0,95, kein 1450-Hz-Anteil), **beim Benutzen des Rekorders kein Ton**
(`stimme.log`: "raum=1100 nachricht=4 laeuft=0", kein Clip geladen). Piep erst Tick 824..838 (= F701, Anteil 1450 Hz
0,28..0,73), Aufschliessen ab Tick 839 (F716, unter 250 Hz 0,93).
Bilder `J_belege/vorher_1100_ablauf.png`: F498 SAY HELLO (Cursor auf dem Gitter), F500 msg 4 "Yo..", F672 leer,
**F677 ACCEPTED ohne Ton**, F702 msg 2 + Piep, F717 Aufschliessen.

Befund: (a) die Datei des Nutzers spielt nirgends; (b) Piep und Aufschliessen kommen 25 Bilder NACH dem ersten ACCEPTED
mit "You've opened the lock." (Runde-36-Haken an Message_on 2), das ACCEPTED selbst ist stumm.

---

## 3. Lesart und Umsetzung

### 3.1 Lesart des Wortlauts (NUTZER-VORGABE, Form belegt)

* **"beim reinsprechen"** = der Druck auf das Mikrofon-Gitter (sub01 @0x00C2C -> sub02), den RE1.5 mit Message_on 4
  "You've used the Minidisc Player w/ Disc." quittiert. Die Aufnahme startet im SELBEN Skript-Takt wie diese Nachricht
  (Anker @0x00C94 = Evt_next direkt hinter Message_on 4). Sie laeuft, waehrend die Nachricht tippt (40 Zeichen x 2 Bilder
  = 80 Bilder; die Aufnahme hat 79,4 Bilder) und waehrend der Wegwerf-Abfrage weiter (Schliessen stoppt den Stimmen-Kanal
  nicht, audio_pc.c).
* **"nachdem er fertig abgespielt ist"** = erst wenn die Plattform die Aufnahme als beendet meldet, geht sub02 vom
  Schliessen der Nachricht zur ACCEPTED-Anzeige weiter. Bei langsamem Spieler ist sie da laengst zu Ende (kein Warten),
  bei schnellem wartet sub02 an @0x00C96 (gemessen bis 48 Takte).
* **"die erfolgreich message auf den Panel"** = die Anzeige des Panels "ACCEPTED" (Cut 11; die Hintergruende der Cuts
  7..12 sind die Anzeige-Zustaende des VUS, `J_belege/vus_1100_cuts6_12.png`). **"Piep ... für Erfolgreiche Eingabe"**
  kommt im Takt des ERSTEN ACCEPTED (RE2-Form: ROOM2110 sub08 Piep @0x01B5C und Erfolgsanzeige @0x01B68/@0x01B6C im
  selben Skript-Takt), **"den Tür öffnen Sound"** 15 Bilder spaeter (RE2 Sleep 15 @0x01B8C / Se_on(2,0x0B) @0x01B90),
  waehrend ACCEPTED noch blinkt; die Textnachricht "You've opened the lock." folgt wie im Original 25 Bilder nach dem
  ersten ACCEPTED (@0x00CBA) - jetzt ohne zweiten Piep.
* Saetze = die der Kartenleser/Code-Panels: KARTSE.VBS Satz 1 PIEP (RE2 ROOM2110 Satz 0x0D) und Satz 2 AUF_E (Satz 0x0B),
  ueber die unveraenderte Runde-36-Folge KARTE_AUF der Stelle (Raum, 2) (`gen/r36_aufschliessen.inc`).
* Alle Original-Opcodes laufen unveraendert und in Original-Reihenfolge. Eingefuegt ist nur das Warten an @0x00C96;
  der Runde-36-Ton ist von Message_on 2 auf das erste ACCEPTED vorgezogen.

### 3.2 Dateien

| Datei | Inhalt |
|---|---|
| `re15_port/include/re15_rekorder_r38.h` (neu) | Belege (Kopf), Anker-Konstanten, Zustaende, API |
| `re15_port/engine/src/rekorder_r38.c` (neu) | 56-Byte-Signatur sub02 @0x00C90, Anker, Aufnahme anfordern, Warten, Piep am ACCEPTED, msg-2-Weiche, Bindung der Datei, Messschiene `RE15_REKORDER_LOG` |
| `re15_port/engine/src/scd_vm.c` (Haken) | `#include`; Scan in `scd_register_current_rdt` (1 Zeile); Dispatcher `if (g_re15_rekorder_anker_n && re15_rekorder_r38_pc(t) == 2) break;` (1 Zeile, Wachposten 0 ausser 1100/1101); op_message_on: Runde-36-Aufruf hinter `if (!re15_rekorder_r38_message_on(...))` (1 Zeile) |
| `re15_port/engine/src/game_step_common.c` (Haken) | `#include` + `re15_rekorder_r38_tick();` neben `re15_aufschliessen_r36_tick()` |
| `re15_port/platform/pc/src/audio_pc.c` (Haken) | `#include` + 1 Zeile in `re15_voice_load_clip`: `re15_rekorder_r38_stimme(room, voice_id, reldir, .., stamm, ..)` setzt fuer (1100/1101, Id 40) Ordner `synchro/STAGE1/room1100/` und Stamm `Irons_Rekorder_Sound` ein (Muster Runde 36 `re15_elza_stimme_datei`) |
| `re15_port/tests/unit/test_synchro_stimme.c` (Haken) | Zensus erkennt die Bindung (statt "noch nicht zugeordnet"), Format-Pruefung P4 wie jede Datei |
| `re15_port/tests/unit/probe_r38_rekorder.c`, `tests/unit/probes/r38_rekorder.cmake`, `tests/integration/test_r38_rekorder.cmake` | Riegel (Abschnitt 5) |
| `re15_port/tools/r38_j/rekorder_cap.py` | Belegwerkzeug: Aufnahme im Mitschnitt (Kreuzkorrelation), Piep-Ticks |

Keine neuen Daten unter `gen/`/`shared_assets/`; die Nutzer-Datei liegt seit e2cc7673 im Repo. Keine Bank-9-Bits, keine
Nachrichten-Texte (Id 40 ist nur Stimmen-Id, VERTRAG §1.2 Spur J 1100/1101 40-44), keine Item-Ids.

### 3.3 Konstanten

| Konstante | Wert | Beleg |
|---|---|---|
| Anker Message_on 4 / Start / Warten / ACCEPTED / Message_on 2 / Tuer | @0x00C90 / @0x00C94 / @0x00C96 / @0x00C9C / @0x00CBA / @0x00CD2 | ROOM1100.RDT == ROOM1101.RDT sub02-Bytes (§1.1); Riegel `unit_r38_rekorder_anker` |
| Signatur | 56 B @0x00C90..0x00CC7 | dieselben Bytes; abweichend -> kein Anker (Original + Runde 36 bleiben) |
| ACCEPTED-Cut | 11 | @0x00C9C `29 0b`; Hintergrund Cut 11 = "ACCEPTED" |
| Stimmen-Id | 40 | VERTRAG §1.2 (Spur J: ROOM1100/1101 Ids 40-44) |
| Piep / Aufschliessen | KARTSE.VBS Satz 1 / 2, +0 / +15 | RE2 ROOM2110 @0x01B5C (Satz 0x0D) / @0x01B8C Sleep 15 / @0x01B90 (Satz 0x0B); Runde-36-Folge KARTE_AUF |
| "laeuft" | `g_re15_voice_laeuft && g_re15_voice_restbilder > 0` | dieselbe Bedingung wie der Stimmen-Riegel scd_vm.c op_message_on |
| Stempel-Frist | 2 Bilder | audio_pc.c re15_audio_tick stempelt VOR der Warteschlange (Port-Struktur; gemessen: "stimme laeuft ... 2 Bilder nach der Anforderung") |
| Notbremse | 300 Takte | = RE15_VOICE_NOTBREMSE in scd_vm.c (10 s > laengste Aufnahme); greift in keinem Messlauf |

---

## 4. Messung nachher (echte exe, dieselben Eingaben wie vorher)

Messschiene `RE15_REKORDER_LOG=rk.log` (neu), dazu `auf.log`, `cut.log`, `debug.log`, Mitschnitt `RE15_AUDIO_CAP_SYNC`,
Auswertung `probe_r38_rekorder cap` (Kreuzkorrelation der WAV, auf 44100 Hz gerechnet wie der Lader; Piep-Anteil
1350..1550 Hz wie `tools/tueren/r36_cap_ticks.py`, beide Werkzeuge liefern dieselben Ticks) und `probe_r38_rekorder bild`
(Anzeigefeld des Framedumps gegen die dekodierten Hintergruende der Cuts 8/11/12).

### 4.1 Leon, Nachrichten langsam (Lauf `n1_spaet` / `n4_bilder`, Riegel L)
```
rk.log  F499 stimme an: sub02 @0x00C94 (Message_on 4 im selben Takt), Id 40 -> synchro/STAGE1/room1100/Irons_Rekorder_Sound.wav
        F500 stimme laeuft: Plattform meldet 79 Restbilder (2 Bilder nach der Anforderung)
        F579 stimme zu Ende: 81 Bilder nach der Anforderung
        F671 frei: sub02 @0x00C96 Cut_chg 12 nach 0 Wartetakten          (msg 4 + Wegwerf-Abfrage erst F670 zu)
        F676 ACCEPTED: sub02 @0x00C9C Cut_chg 11 -> Piep + Aufschliessen (Runde-36-Art 2 = KARTE_AUF)
        F701 Message_on 2 @0x00CBA: Piep/Aufschliessen schon am ACCEPTED gespielt - Runde-36-Folge hier aus
        F742 Tuer frei: Platz 1 aktiv 1 typ 1 (1 = Tuer) nach @0x00CC8
auf.log F676 PIEP bild=+0 / F691 AUF_E bild=+15   (genau 2 Toene)
debug   [voice] clip loaded: Irons_Rekorder_Sound.wav (32000 Hz x1 -> 116736 @44100 Hz); [scd F676] Cut_chg(11)
cut.log F677 bg=1100#11
cap     WAV Tick 623 (+0) .. 702, Korrelation 0.931, Verstaerkung 1.000 | Piep Tick 800..812 | Versatz Tick-Takt 124 fuer beide
bild    fd F676 -> Cut 12 (Abstand 0.0), fd F677 -> Cut 11 ACCEPTED (Abstand 0.0)
```
Reihenfolge: Aufnahme F500..F579 (Ticks 623..702) -> ACCEPTED + Piep (Takt F676, Bild/Ton ab F677 = Tick 800) ->
Aufschliessen F691 -> "You've opened the lock." F701 (stumm) -> Tuer frei. Vorher kam der Piep erst F701 (§2).
Bilder `J_belege/nachher_1100_ablauf.png` (F498 SAY HELLO, F500 msg 4 + Aufnahme, F672 leer, F676 Takt des Pieps noch leer,
F677 ACCEPTED + Piep, F692 Aufschliessen (Blinkphase leer), F702 msg 2), Mitschnitt-Verlauf vorher/nachher
`J_belege/mitschnitt_vorher_nachher.png`.

Bild und Ton im selben Bild: der Piep wird im Skript-Takt von Cut_chg 11 angefordert (auf.log F676 = `[scd F676]
Cut_chg(11)`); die Anzeige wechselt im naechsten gezeigten Bild (Hintergrund wird vor dem VM-Takt gezeichnet, F677), der
Piep wird ab dem naechsten Audio-Tick gemischt (Versatz Anforderung -> Mitschnitt 124 Ticks, gleich fuer Aufnahme und Piep).
Das ist dieselbe Port-Struktur wie Runde 37 E (Lampe/Kamera +1 Bild gegen den Skript-Takt).

### 4.2 Leon, Nachrichten so schnell wie moeglich (Lauf `n3_schnell`, Riegel S)
```
rk.log  F499 stimme an / F500 laeuft (79) / F579 stimme zu Ende / F580 frei: ... nach 32 Wartetakten / F585 ACCEPTED -> Piep
auf.log F585 PIEP / F600 AUF_E ; msg 2 F610 ohne Folge
cap     WAV Tick 623..702 (Korrelation 0.906) | Piep ab Tick 709 (7 Ticks nach dem letzten WAV-Tick)
```
Hier ist sub02 an @0x00C96 angekommen, waehrend die Aufnahme noch lief (F548) - es wartet 32 Takte, geht ein Bild nach dem
Ende weiter (F580), ACCEPTED + Piep 5 Bilder spaeter (Cut 12 + Sleep 5, @0x00C96/@0x00C98). Ohne den Haken kaeme der
Piep in diesem Fall MITTEN in die Aufnahme.

### 4.3 Elza ROOM1101 (Lauf `e1_elza`, Riegel E; `RE15_PSELECT_AUTO_SWITCH=1`, "[pl] Spieler-Familie PL04")
```
rk.log  F629 raum=1101 stimme an ... -> synchro/STAGE1/room1100/Irons_Rekorder_Sound.wav / F709 zu Ende / F801 frei (0)
        F806 ACCEPTED -> Piep / F831 Message_on 2 ... aus / F872 Tuer frei
auf.log F806 raum=1101 PIEP / F821 AUF_E
cap     WAV Tick 1554..1633 (Korrelation 0.902) | Piep ab Tick 1731 | Versatz 925 fuer beide
```

### 4.4 Tuer (Lauf `t2_tuer`, Riegel T; `RE15_NOAUDIO=1` = zugleich "ohne Ton")
Panel per `RE15_FORCE_EVENT=7@150` (= Ereignis des Panel-Platzes, sub00 @0x00B14 `.. ff 00 18 07`), sonst gleiche Eingaben:
F676 ACCEPTED/Piep, F742 "Tuer frei: Platz 1 aktiv 1 typ 1"; `RE15_FIRE_AOT=1@860` -> `[aot] DOOR FIRE slot=1` -> Tuer-
Sequenz -> `[room] PC loaded room1110.rdt`. Ohne Ton (kein Geraet) laeuft der Ablauf ohne Warten durch (0 Wartetakte).

---

## 5. Tests

Neu (`re15_port/tests/unit/probes/r38_rekorder.cmake`):

| Test | misst |
|---|---|
| `unit_r38_rekorder_anker` | ROOM1100 == ROOM1101; sub02 @0x00C90..0x00CC7, @0x00C68, @0x00CC8/@0x00CD2; sub01 @0x00C2C; sub08 @0x00D82; Port-Konstanten == Bytes; Anker nur 1100/1101 und nur mit Signatur; RE2 ROOM2110 @0x01B5C/@0x01B68/@0x01B6C/@0x01B7C/@0x01B8C/@0x01B90; Runde-36-Stellen 1100/2 + 1101/2 = KARTE_AUF, Folge Satz 1 +0 / Satz 2 +15 |
| `unit_r38_rekorder_datei` | WAV vorhanden, PCM16 mono 32000 Hz, data 169414 B = 84707 Samples = 79,4 Bilder; Bindung 1100/1101 Id 40 -> genau diese Datei; keine Bindung fuer msg 4 / andere Raeume / Id 41; Zensus-Erkennung |
| `unit_r38_rekorder_ablauf` | ECHTER Skriptweg headless (Panel-AOT 2, msg 1, Ja, Cursor ON/OFF, SAY HELLO, Cursor Mikrofon, Wegwerf-Abfrage) mit nachgebildetem Stimmen-Kanal (Reihenfolge audio_pc.c, Laenge aus der WAV): A Leon langsam, B Leon schnell (Warten 48 Takte, Piep >= Ende + 5), C Elza ROOM1101, D Gegenprobe ohne Haken (= vorher: keine Aufnahme, Piep an msg 2), E ohne Ton (kein Haenger); je: Aufnahme im Takt von msg 4, Piep im Takt des ersten ACCEPTED (Cut 11, vorher 12), nach dem Ende der Aufnahme, Aufschliessen +15, genau 2 Toene, Tuer-Platz 1 = Tuer, Feuer -> Raumwechsel ROOM1110/1111 |
| `integration_r38_rekorder` | echte exe L/S/E/T wie Abschnitt 4 (Logs, Mitschnitt mit Korrelation, Versatz, Ende vor Piep, Framedumps Takt/Takt+1 = Cut 12/11, Tuer -> room1110.rdt) |

Geaendert: `unit_synchro_stimme` (Zensus: "= ...room1100/Irons_Rekorder_Sound.wav: Sonderbindung re15_rekorder_r38_stimme",
"195 WAVs, 0 Namensfehler, 0 Formatfehler, 0 Stage-Fehler, 0 noch nicht zugeordnet").

### 5.1 Gezielte Laeufe (KEINE volle Suite)

Regex = eigene Tests + Testgruppen der beruehrten Module/Raeume (SCD-VM-Haken: SCD/AOT/Nachrichten/Kamera; Stimmen-Kanal/
synchro; Runde-36-Aufschliess-Toene; Runde-37-Code-Panels (gleicher Mechanismus); ROOM1100: Kistenregel K6, PRI 1100,
Elza-EMS 1100, Zombie-Spur t45; Wegwerf-Abfrage; Tueren/Schloesser; headless):
```
ctest --test-dir "<baum>/re15_port/build" -R "r38_rekorder|r36_tueren|r37_codepanel|unit_synchro_stimme|unit_voice_ausreden|unit_se_voice_machine|unit_audio_sperren|unit_msg_dialog|unit_action_msg_gate|unit_text_freeze|unit_r21_discard_wegwerfen|r37_kistenregel|r37_pri1100|r37_elzaems_leon1100|unit_lane_b1_t45_room1100|unit_scd_|unit_aot_|unit_cut_old_displayed|unit_r34n_b_cut_old|unit_r30_tuer_stimme|r36_elza_unlock|r34n_f_leiche_stimme|r34n_f_leiche_nachhall|integration_headless_run|integration_door_lock|integration_keydoor|integration_keypad" --timeout 600 --output-on-failure -j 2
```
Ergebnis (Stand c59ead3c, Protokoll `re15_port/build/r38j/ctest_gezielt_1.log`):
```
100% tests passed, 0 tests failed out of 55
```

---

## 6. OFFEN / Grenzen

* **Kein Panel-Licht am VUS:** die Spur-Beschreibung des Orchestrators nennt "dem gruenen Panel-Licht"; das Stimmschloss
  hat keine gemalte Lampe (Hintergruende Cuts 7..12, `J_belege/vus_1100_cuts6_12.png`; Runde 37 E §0 "ROOM1100 (VUS) hat
  keine Lampen, nur eine Anzeige"). Die Erfolgsmeldung des Panels ist die Anzeige "ACCEPTED" (Cut 11); der Wortlaut des
  Nutzers nennt kein Licht. Es wird keines erfunden.
* **Aufschliessen in der Blinkphase:** +15 (RE2 Sleep 15) faellt auf eine leere Phase des ACCEPTED-Blinkens (F692,
  Original-Blinktakt 5 Bilder @0x00C98..); die Anzeige steht ab F697 wieder auf ACCEPTED, "You've opened the lock." F701.
* **Abstand Ende der Aufnahme -> Piep haengt am Spieler:** zwischen beiden liegen die Nachricht 4 und die Runde-21/22-
  Wegwerf-Abfrage ("Discard it?"); sub02 laeuft erst weiter, wenn beide zu sind. Gewartet wird nur, wenn der Spieler
  schneller ist als die Aufnahme (gemessen 32 Takte). Die Aufnahme laeuft dabei weiter (Schliessen stoppt den Kanal nicht),
  die Zeile 4 bleibt im Untertitel-Nachhall stehen, solange sie spricht (msg_common.c Zustand 7).
* **Bildversatz +1:** Anzeige und Ton erscheinen ein Bild nach dem Skript-Takt (Port-Struktur wie Runde 37 E), beide im
  selben Bild.
* **PSX-Ziel:** spielt keine WAV (audio_psx.c sucht `\VOICE\VOICE40.VAG`, nicht vorhanden -> still); der Ablauf laeuft dort
  ohne Warten (Stempel bleibt 0), Piep/Aufschliessen am ACCEPTED wie auf dem PC.
* **Gegenprobe-Grenze:** ohne Audiogeraet (RE15_NOAUDIO) gibt es nichts abzuwarten; gemessen Lauf T (0 Wartetakte).

## 7. Fuer den Nutzer / den Orchestrator

* **Was man sieht/hoert (Leon ROOM1100, Elza ROOM1101):** Minidisc Player im Inventar, am Panel "Ja", ON/OFF -> SYSTEM
  READY -> SAY HELLO, Cursor aufs Mikrofon-Gitter und Viereck: "You've used the Minidisc Player w/ Disc." und gleichzeitig
  Irons' Aufnahme (2,6 s). Danach (ggf. nach der Wegwerf-Frage) wird die Anzeige kurz leer, dann **ACCEPTED mit dem Piep**,
  eine halbe Sekunde spaeter das **Aufschliessen**, waehrend ACCEPTED blinkt; dann "You've opened the lock." (ohne
  zweiten Piep). Ist man schneller als die Aufnahme, wartet die Anzeige, bis Irons fertig gesprochen hat.
* **Neue Assets / Paket-Gate:** `synchro/STAGE1/room1100/Irons_Rekorder_Sound.wav` (Nutzer-Datei, seit e2cc7673 im Repo,
  NICHT umbenannt). `release/make_package.sh` kopiert `synchro/STAGE*` ganz (`cp -r`, ~Z. 681-684) und vergleicht die
  WAV-Zahl Repo/Paket (~Z. 324-326) -> die Datei ist damit im Paket und gezaehlt. Empfehlung fuer Spur V/Orchestrator: eine
  ausdrueckliche Pruefung wie fuer `room1170/main00.wav` (~Z. 319-320): `[[ -s "$out/synchro/STAGE1/room1100/Irons_Rekorder_Sound.wav" ]]`.
  Keine Dateien unter `shared_assets/`/`gen/`.
* **Sprachdateien:** keine neue Message-Id belegt; Stimmen-Id 40 (VERTRAG §1.2 Spur J) bindet die Datei, `main40.wav`
  wird nicht gesucht. Fuer Elza wird dieselbe Datei aus `room1100` gespielt.
* **Bau/Paket:** neue Datei `engine/src/rekorder_r38.c` -> Android-GLOB-Cache neu konfigurieren
  (reai-v2-android-glob-cache); RE15_MIN_TESTS +4 (3 unit + 1 integration).
* **Messhaken:** `RE15_REKORDER_LOG=<datei>` (Anforderung, Laufen/Ende laut Plattform, Warten, ACCEPTED/Piep, msg-2-Weiche,
  Tuer frei).
* **Entscheidungsfragen:** keine.
