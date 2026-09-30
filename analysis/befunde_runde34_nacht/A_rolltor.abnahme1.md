# Spur A (Rolltor ROOM1050) — ABNAHME 1 (unabhaengig)

Stand: IN ARBEIT (Zwischenstand nach Bau, Suite und vier eigenen exe-Laeufen)

Rolle: unabhaengige Abnahme nach dem Bau, Code nicht selbst geschrieben. Ziel: WIDERLEGEN, dass Spur A
fertig ist. Arbeitsbaum `.claude/worktrees/r34n_rolltor` (Zweig `r34n/rolltor`), geprueft auf `38ad77a2`.
Alle Zahlen unten selbst gemessen bzw. selbst an den Bytes gelesen (eigene Skripte, nicht `belege.py`),
ausser wo ausdruecklich "vom Bau uebernommen" steht.

## 0. Urteil

(vorlaeufig, wird am Ende gesetzt)

## 1. Gepruefter Stand (Commit, Suite-Zeile)

* Zweig `r34n/rolltor` @ `38ad77a2` (wip-Sicherung beim Limit-Abbruch 05:40) + Bau-Fix `c1d08469` (cherry-pick).
  Port-Code der Spur: `engine/src/rolltor_1050.c`, `include/re15_rolltor.h`, zwei Haken in `scd_vm.c`
  (`scd_event_fire` +3 Zeilen, `register_opcodes` +4 Zeilen), Riegel `tests/unit/test_r34n_a_rolltor.c`,
  angepasst `tests/unit/test_room1050_sicherung.c`.
* Selbst gebaut: `RE15_MIN_TESTS=429 bash re15_port/tools/local_build.sh` (configure+build+test) ->
  **`=== LOCAL-BUILD-OK (all) — Tests 429/429`**, `100% tests passed, 0 tests failed out of 429`,
  816 s, waehrend mindestens zwei fremde Baeume (r34n_generator, r34n_leichen) ihre Suiten fuhren.
  Ninja baute nur die Sonde neu (die wip-Aenderung betraf nur deren Kopfkommentar) -> die geprueften exe
  entsprechen dem Code-Stand `38ad77a2`.
* `re15_port/tools/r34n_a/belege.py` nachgefahren: `SUMME: 88 OK, 0 FEHLT` (nur Gegenlauf; die tragenden
  Bytes habe ich zusaetzlich mit eigenem Skript gelesen, §5).

## 2. Nutzerpunkte aus AUFTRAG.md — Fahrt an der echten exe

Echte `re15_pc.exe` dieses Baums als Kopie `re15_pc_abn1.exe` (eigenes Skript
`A_belege/abnahme1_lauf.sh`), beschleunigter Renderer, `RE15_WINDOW_SCALE=3`, `RE15_FRAMEDUMP`,
`state.log`/`debug.log`. Alle genannten Bilder angesehen. Auszuege: `A_belege/abn1_laeufe.txt`.

| Nutzersatz | Lauf | Ergebnis (Bild / Log) |
|---|---|---|
| "nicht mehr einfach so geoeffnet … die CUT und der Text mit der Sicherung" | `ohne_mash` (keine Sicherung, 25 s Viereck-Hammern) | 8 volle Runden je 90 Bilder: Frage msg 0 -> Ja -> `Cut_chg(7)` + msg 2 "I need a fuse to run the shutter." -> `Cut_chg(3)` -> Steuerung frei (pm 0) -> naechste Frage. KEIN Se_on, (3,121) nie gesetzt, Tor bleibt zu. Kein Haenger, keine Doppelausloesung (je Druck genau eine `[rolltor]`-Zeile). |
| "erst geoeffnet …, wenn die Sicherung eingesetzt" | `tuer_mash_mit` (Sicherung per RE15_GIVE, Leon ueber den ECHTEN Tuerweg 1030 Slot 0 -> 1050, 30 s Hammern) | F874 Frage, F912 Cut 7 + msg 2, F947 msg 20 "Will you use the **Fuse**?", F970 Ja -> `Sce_item_lost(0x40): Platz 3 geleert`, F972 Cut 8 + msg 21 "You've used the **Fuse**.", F995 Cut 3, F1009 frei, F1012 AUSGELIEFERTE Frage, F1050 Fahrt, `Se_on 2/0x0C, 0x0A, 0x0B`, F1284 frei, Tor offen. `A_belege/abn1_tuerweg_mash_mit_*.jpg` |
| "Fuer die Sicherung und Nachricht soll es eine Nahansicht geben" | dieselben Laeufe | Cut 7 (Sockel rechts leer, rote Leuchte) genau solange msg 2/msg 20 stehen; Cut 8 (zwei Sicherungen, beide gruen) genau solange msg 21 steht. Spieler in der Nahansicht unsichtbar, nach der Rueckkehr in Cut 3 wieder im Bild. |

## 3. Gegenproben (vorher/nachher, falsche Eingaben, Laden/Speichern, Wiederbetreten, ROOM1051, Nachbarverhalten)

* **Falsche Eingabe "No" auf die Schalterfrage mit Sicherung** (`mit_erst_nein`): F777 Frage zu, KEIN
  Cut-Wechsel, Sicherung bleibt; naechster Druck F810 -> Ja -> F965 Cut 7 ... F1211 Cut 8 ... F1333 Cut 3;
  dritter Druck F1485 -> ausgelieferte Fahrt ab F1640 mit Se_on 0x0C/0x0A/0x0B.
* **START (Statusschirm) waehrend der Nahansicht** (`mit_status`, Viereck und START im Wechsel alle 9
  Bilder): acht START-Druecke zwischen F686 und F812 (Cut 7/8 und das Rueckweg-Bild) oeffnen den
  Statusschirm NICHT und stoeren den Ablauf nicht; erst F830 (Steuerung frei) oeffnet er ganz normal —
  Inventar Knife/Browning/Bullets, KEINE Sicherung, kein Loch (`A_belege/abn1_mit_start_waehrend_nahansicht.jpg`, F842).
* **Tuerweg statt Debug-Sprung** (Leon): der Bau hat Leon nur per Debug-JUMP gefahren. Mein Lauf
  `tuer_mash_mit` kommt ueber die echte Tuer ROOM1030 Slot 0 (@0x01C6A, Ziel (20600,0,12350) Blick 2048)
  in den Raum — Ergebnis identisch.
* (weitere Punkte folgen)

## 4. RE-Gate-Pruefung des Codes (Konstanten, Belege, Stubs)

(in Arbeit)

## 5. Nachpruefung der Belege (Memory reai-v2-sicherung-raetsel-1050.md + Dossier)

Eigenes Skript (`chk_bytes.py`, Scratchpad), eigene Disassembly:

* Kameratabelle ROOM1050 @0x60 (RDT+0x24): Cut 7 und Cut 8 flag 0, fov 26684, pos (16171,-2733,-8185),
  tgt (18240,-1974,-8706), pri 0x518 / 0x51C, erste 28 Byte gleich — **bestaetigt**; ROOM1051 identisch.
* Kunst ROOM105.BSS Scheibe 7/8 (Engine-Dekoder `probe_bg_dump`, eigener Lauf): Kanalabweichung >4:
  **2761 px, bbox x123..268 y16..181**; >8: 1544; >24: 742. Cut 7: rote Leuchte x195..203 y66..73, gruene
  x141..149; Cut 8: 0 rote, 120 gruene Pixel. **Die Memory-Zahl "2317 Pixel, bbox x136..268" stimmt mit
  keiner Schwelle** (Bau hatte das schon angemerkt, bestaetigt) — der Befund "nur der Kasten unterscheidet
  sich" haelt. Cut 3: kleiner Kasten bei x~128/130 y~76 mit gruener UND roter Leuchte
  (`A_belege/abn1_kunst_cut3kasten_cut7_cut8.jpg`).
* msg 2 @0x0ED2 "I need a fuse to run the shutter." (rdt_msgdump), im SCD kein `2b 02` — bestaetigt
  (scd_dump: Message_on nur @0x0CAC id 0 und @0x0DD6 id 8).
* Schalter Aot_set Slot 7 sce 3 @0x0C22, Nutzlast `ff 00 18 02`; sub02 @0x0CB6 `21 0c 1f 00`; (3,121)
  @0x0C1E/@0x0CBA — bestaetigt. ROOM1051: sub02 @0x0CC8..0x0DA4 == ROOM1050 sub02 @0x0CAC..0x0D88 (bytegleich),
  Slot-7-Zone @0x0C4C == @0x0C22, msg 2 @0x0E68 == @0x0ED2 — bestaetigt.
* msg 20/21 im Code (`k_msg20`/`k_msg21`) == ROOM2060.RDT @0x1855 (32 B) / @0x1875 (29 B) **bytegleich**
  (eigener Vergleich), Laengen passen genau an den naechsten Eintrag (0x1875 / 0x1892).
* RE2 `Sce_item_lost`: `re2_disasm.py table 0x800a7650` -> 0x800585e4; `dis 0x800585e4`: @0x800585fc
  `lbu a0,1(v0)`, @0x80058600 `jal 0x800696cc`, @0x80058608 `bltz v0,0x80058634`, @0x80058618/24/30
  `sb zero` 0x800d4a3c/3d/3e, @0x80058634 `jal 0x80069714`, @0x80058640 `addiu v0,zero,1`, @0x80058644
  `addiu v1,v1,2` — **bestaetigt**, Port-Handler bildet genau das ab.

## 6. Maengelliste (Schwere + Beleg)

(in Arbeit)

## 7. Belegdateien (A_belege/)

* `abnahme1_lauf.sh` — die vier Laeufe (Modi `tuer_mash_mit`, `ohne_mash`, `mit_status`, `mit_erst_nein`)
* `abn1_laeufe.txt` — gefilterte debug.log-Zeilen + state.log-Wechsel aller vier Laeufe
* `abn1_tuerweg_mash_mit_nahansicht.jpg`, `abn1_tuerweg_mash_mit_torfahrt.jpg`
* `abn1_mit_start_waehrend_nahansicht.jpg`
* `abn1_kunst_cut3kasten_cut7_cut8.jpg`
