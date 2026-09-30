# Spur A (Rolltor ROOM1050) — ABNAHME 1 (unabhaengig)

Stand: FERTIG (2026-09-30, ~08:50). Rolle: unabhaengige Abnahme nach dem Bau, Code nicht selbst
geschrieben. Ziel war, zu WIDERLEGEN, dass Spur A fertig ist. Arbeitsbaum `.claude/worktrees/r34n_rolltor`
(Zweig `r34n/rolltor`), geprueft auf `38ad77a2`. Alle Zahlen unten selbst gemessen bzw. selbst an den
Bytes gelesen (eigene Skripte, nicht `belege.py`), ausser wo "vom Bau uebernommen" steht.

## 0. Urteil

**ABGENOMMEN — mit vier kosmetischen Maengeln (§6), kein Blocker, nichts Wesentliches.**

Widerlegen liess sich die Fertigmeldung nicht. Alle drei Nutzersaetze laufen an der echten exe,
auch unter Eingaben, die der Bau nicht gefahren hat: Leon ueber den echten Tuerweg statt Debug-Sprung,
Viereck-Hammern mit und ohne Sicherung, START waehrend der Nahansicht, erst "No", dann "Yes",
Wiederbetreten nach dem Einsetzen und der Kartenlauf Speichern -> Laden (nachgefahren). Suite 429/429.
Jede tragende Konstante ist im Code belegt, die Belege stimmen mit meinen eigenen Messungen ueberein.
Die Maengel betreffen Doku und Prozess (fehlender Abschluss-Commit, veralteter Dossier-Kopf) sowie eine
bekannte, dokumentierte Kunstgrenze.

## 1. Gepruefter Stand (Commit, Suite-Zeile)

* Zweig `r34n/rolltor` @ `38ad77a2` (wip-Sicherung beim Limit-Abbruch 05:40) + Bau-Fix `c1d08469` (cherry-pick).
  Port-Code der Spur: `engine/src/rolltor_1050.c`, `include/re15_rolltor.h`, zwei Haken in `scd_vm.c`
  (`scd_event_fire` +3 Zeilen, `register_opcodes` +4 Zeilen), Riegel `tests/unit/test_r34n_a_rolltor.c`,
  angepasst `tests/unit/test_room1050_sicherung.c`.
* Selbst gebaut: `RE15_MIN_TESTS=429 bash re15_port/tools/local_build.sh` (configure+build+test) ->
  **`=== LOCAL-BUILD-OK (all) — Tests 429/429`**, `100% tests passed, 0 tests failed out of 429`,
  816 s. Waehrenddessen fuhren mindestens zwei fremde Baeume (r34n_generator, r34n_leichen) ihre Suiten.
  Ninja baute nur die Sonde neu, weil die wip-Aenderung nur deren Kopfkommentar betraf. Die geprueften
  exe entsprechen also dem Code-Stand `38ad77a2`.
* `test_r34n_a_rolltor.exe` direkt gefahren: Faelle A/A-N/B/B-N/C fuer 1050 und 1051, W, L und
  PC-Schranke, `RESULT: OK`. Die Bildzahlen stimmen mit dem Dossier §9.4 ueberein.
* `re15_port/tools/r34n_a/belege.py` nachgefahren: `SUMME: 88 OK, 0 FEHLT`. Das ist nur ein Gegenlauf; die
  tragenden Bytes habe ich zusaetzlich mit eigenem Skript gelesen (§5).

## 2. Nutzerpunkte aus AUFTRAG.md — Fahrt an der echten exe

Gefahren wurde die echte `re15_pc.exe` dieses Baums als Kopie `re15_pc_abn1.exe` (eigenes Skript
`A_belege/abnahme1_lauf.sh`), mit beschleunigtem Renderer, `RE15_WINDOW_SCALE=3`, `RE15_FRAMEDUMP` sowie
`state.log`/`debug.log`. Ich habe alle genannten Bilder angesehen. Die Auszuege stehen in
`A_belege/abn1_laeufe.txt`.

| Nutzersatz | Lauf | Ergebnis (Bild / Log) |
|---|---|---|
| "nicht mehr einfach so geoeffnet … die CUT und der Text mit der Sicherung" | `ohne_mash` (keine Sicherung, 25 s Viereck-Hammern) | 8 volle Runden zu je 90 Bildern: Frage msg 0 -> Ja -> `Cut_chg(7)` + msg 2 "I need a fuse to run the shutter." -> `Cut_chg(3)` -> Steuerung frei (pm 0) -> naechste Frage. Kein Se_on, (3,121) wird nie gesetzt, das Tor bleibt zu. Kein Haenger, keine Doppelausloesung: je Druck genau eine `[rolltor]`-Zeile. `A_belege/abn1_ohne_mash.jpg` |
| "erst geoeffnet …, wenn die Sicherung eingesetzt" | `tuer_mash_mit` (Sicherung per RE15_GIVE, Leon ueber den ECHTEN Tuerweg 1030 Slot 0 -> 1050, 30 s Hammern) | F874 Frage, F912 Cut 7 + msg 2, F947 msg 20 "Will you use the **Fuse**?", F970 Ja -> `Sce_item_lost(0x40): Platz 3 geleert`, F972 Cut 8 + msg 21 "You've used the **Fuse**.", F995 Cut 3, F1009 frei, F1012 AUSGELIEFERTE Frage, F1050 Fahrt mit `Se_on 2/0x0C, 0x0A, 0x0B`, F1284 frei, Tor offen. `A_belege/abn1_tuerweg_mash_mit_*.jpg` |
| "Fuer die Sicherung und Nachricht soll es eine Nahansicht geben" | dieselben Laeufe | Cut 7 (Sockel rechts leer, rote Leuchte) steht genau so lange, wie msg 2 bzw. msg 20 steht; Cut 8 (zwei Sicherungen, beide gruen) genau so lange, wie msg 21 steht. In der Nahansicht ist der Spieler unsichtbar (Regions-Cull gegen das Blindrechteck, wie im Dossier). Nach der Rueckkehr steht Leon in Cut 3 wieder sichtbar am Kasten (Ausschnitt `A_belege/abn1_leon_am_schalter_cut3.jpg`). |

## 3. Gegenproben (vorher/nachher, falsche Eingaben, Laden/Speichern, Wiederbetreten, ROOM1051, Nachbarverhalten)

* **Falsche Eingabe "No" auf die Schalterfrage mit Sicherung** (`mit_erst_nein`): F777 schliesst die Frage,
  ohne Cut-Wechsel, die Sicherung bleibt. Naechster Druck F810 -> Ja -> F965 Cut 7 … F1211 Cut 8 … F1333 Cut 3.
  Dritter Druck F1485 -> ausgelieferte Fahrt ab F1640 mit Se_on 0x0C/0x0A/0x0B.
* **START (Statusschirm) waehrend der Nahansicht** (`mit_status`, Viereck und START im Wechsel alle 9
  Bilder): Acht START-Druecke zwischen F686 und F812 (Cut 7, Cut 8 und das Rueckweg-Bild) oeffnen den
  Statusschirm NICHT und stoeren den Ablauf nicht. Erst F830 (Steuerung frei) oeffnet ihn ganz normal:
  Knife/Browning/Bullets, KEINE Sicherung, kein Loch im Raster
  (`A_belege/abn1_mit_start_waehrend_nahansicht.jpg`, F842).
* **Tuerweg statt Debug-Sprung** (Leon): Der Bau hat Leon nur per Debug-JUMP gefahren. Mein Lauf
  `tuer_mash_mit` kommt ueber die echte Tuer ROOM1030 Slot 0 (@0x01C6A, Ziel (20600,0,12350) Blick 2048)
  in den Raum. Das Ergebnis ist identisch.
* **Wiederbetreten nach dem Einsetzen** (`wieder`): Im ersten Raumaufbau wird eingesetzt (Ja/Ja, F1007 Cut 8,
  F1129 Cut 3). Danach folgt ein Debug-Menue-JUMP von Hand (SELECT, Viereck), also derselbe Aufbauweg wie
  bei einer Tuer (`re15_room_request_change` -> `re15_room_apply_pending`). Im zweiten Aufbau erscheint
  KEINE `[rolltor]`-Zeile; es kommt die ausgelieferte Frage, bei F761 `Cut_chg(3)` mit Se_on 12/10/11, und
  das Tor ist offen (`A_belege/abn1_wiederbetreten_tor_offen_F1000.jpg`). Code-Gegenprobe: Der
  Raumaufbau loescht nur (2,7) und das Bank-5-Wort 0 (`scd_room_setup.c:273/280`); Bank 9 ueberlebt.
* **Laden/Speichern**: Den Kartenlauf des Baus `re15_port/tools/r34n_a/lauf_laden.sh` habe ich selbst
  nachgefahren. Gespeichert wurde in 1150 an der Schreibmaschine mit (9,63), ergebnis
  `[save] saved (room 1150)`. CONTINUE aus dem neuen Platz -> 1050 -> Schalter: keine `[rolltor]`-Zeile,
  ausgelieferte Fahrt, Se_on 12/10/11. Gegenprobe mit dem Platz ohne Bit: Port-Programm OHNE,
  Cut 7 -> Cut 3. Mechanismus im Code: `re15_savedata.c:210/274` kopiert ganz `g_game.flags` (memcpy).
* **ROOM1051 (Elza)**: Nicht neu gefahren, weil ich keinen Zweifel habe. Die Bytes habe ich selbst
  verglichen (§5). Die Bau-Bilder `bau_1051_mit.jpg` (Cut 7 -> msg 20 -> Cut 8 -> Cut 3 -> Tor offen)
  habe ich angesehen, und der Riegel faehrt 1051 mit.
* **Nachbarverhalten**: Der Haken greift nur bei Raum 0x1050/0x1051 UND Ereignis 2 UND (9,63)=0 UND
  (3,121)=0 (`rolltor_1050.c:161-169`); jedes andere `scd_event_fire` nimmt unveraendert
  `sub_scd[event_id]`. Opcode 0x62 ausserhalb des Port-Programms laeuft exakt wie `op_unknown`
  (`scd_vm.c:4260`: pc += s_opcode_sizes[0x62] = 1, return 1). Suite 429/429.
* **Zensus Bit 63/64** (eigener Lauf ueber ALLE 240 RDTs): 0 x Ck/Set Bank 9 Bit 63/64, und unter den
  164 Item_aot_set-Records hat keiner das Nimm-Bit 63/64. Im Port-Code nutzt nur `rolltor_1050.c` das Bit.
  Ein `git grep` nach Bit-Definitionen bzw. `flag_set(9, 63|64)` in `include/` und `engine/src` der
  anderen r34n-Zweige (Stand ihrer letzten wip-Commits) ergab keinen Treffer.
* **Nebenspuren**: B (hebetisch) und F (leichen) aendern `scd_vm.c` an anderen Funktionen (`op_for`,
  `op_message_on`). Bei der Zusammenfuehrung beruehren sich nur die `#include`-Zeilen.

## 4. RE-Gate-Pruefung des Codes (Konstanten, Belege, Stubs)

* `rolltor_1050.c` / `re15_rolltor.h`: Jede Bytecode-Zeile traegt ihr Vorbild mit Datei-Offset
  (sub02 @0x0CAC/@0x0CB6, ROOM1051 sub03 @0x0DA4/@0x0DC0/@0x0DC2/@0x0DD2, ROOM2060 sub12 @0x015D0,
  sub18 @0x0167C/@0x0167E/@0x01682/@0x01688, sub19 @0x0169E/@0x016A2/@0x016C8/@0x016CC, Kamera
  @0x140/@0x160). Die Texte stehen mit @0x1855/@0x1875, 0x62 mit den RE2-Adressen @0x800585fc..@0x80058644.
  (9,63) und msg 20/21 sind als PORT-WAHL (VERTRAG §1.1/§1.3) gekennzeichnet, Cut 7 fuer die Nachricht
  als NUTZER-VORGABE.
* Keine Rate-Woerter ("plausibel", "interim", "tunable", "TODO" …) in den neuen Dateien (grep).
  Die einzige Zahl ohne @0x ist `d < 65535` in `text_einsetzen`. Das ist dieselbe Wertebereichsschranke
  wie im Raum-Lader `msg_common.c:343` und in `tuer1120_1130.c:70`, keine Verhaltenskonstante.
* Blocklaengen selbst nachgerechnet: k_ohne `06 00 1a 00` @+06 -> +0x24 (hinter Endif +0x22);
  k_mit aussen `06 00 38 00` -> +0x42 (hinter Endif +0x40), innen `06 00 14 00` @+0x20 -> +0x38 (hinter Endif
  +0x36). Das entspricht der Regel sub02 @0x0CB2 `06 00 d0 00` -> 0x0D86.
* Kein Stub und kein Platzhalter: Der Opcode-Handler bildet die RE2-Folge komplett ab, auch den Zweig
  "nicht gefunden -> nur nachruecken" (@0x80058608 `bltz` -> @0x80058634).
* Commit-Messages: siehe Mangel M1.

## 5. Nachpruefung der Belege (Memory reai-v2-sicherung-raetsel-1050.md + Dossier)

Eigenes Skript (`chk_bytes.py`, Scratchpad) und eigene Disassembly:

* Kameratabelle ROOM1050 @0x60 (RDT+0x24): Cut 7 und Cut 8 haben flag 0, fov 26684, pos (16171,-2733,-8185)
  und tgt (18240,-1974,-8706), pri 0x518 / 0x51C; die ersten 28 Byte sind gleich. **Bestaetigt**;
  ROOM1051 ist identisch. Cut 3: pos (14605,-3721,550), tgt (17259,3056,-19612).
* Kunst ROOM105.BSS, Scheibe 7/8 (Engine-Dekoder `probe_bg_dump`, eigener Lauf): Kanalabweichung >4:
  **2761 px, bbox x123..268 y16..181**; >8: 1544; >24: 742. Cut 7 hat die rote Leuchte bei x195..203 y66..73
  und die gruene bei x141..149; Cut 8 hat 0 rote und 120 gruene Pixel. **Die Memory-Zahl "2317 Pixel, bbox
  x136..268" passt zu keiner Schwelle.** Das hatte der Bau schon angemerkt; ich bestaetige es (Memory
  berichtigen, M4). Der Befund "nur der Kasten unterscheidet sich" haelt. In Cut 3 sitzt der kleine Kasten
  bei x~128/130 y~76 mit gruener UND roter Leuchte (`A_belege/abn1_kunst_cut3kasten_cut7_cut8.jpg`).
* msg 2 @0x0ED2 "I need a fuse to run the shutter." (rdt_msgdump). Im SCD gibt es kein `2b 02`; Message_on
  kommt nur @0x0CAC mit id 0 und @0x0DD6 mit id 8 vor. **Bestaetigt.**
* Schalter: Aot_set Slot 7 sce 3 @0x0C22 mit Nutzlast `ff 00 18 02`, sub02 @0x0CB6 `21 0c 1f 00`, (3,121)
  @0x0C1E/@0x0CBA. **Bestaetigt.** ROOM1051: sub02 @0x0CC8..0x0DA4 ist bytegleich zu ROOM1050 sub02
  @0x0CAC..0x0D88, Slot-7-Zone @0x0C4C == @0x0C22, msg 2 @0x0E68 == @0x0ED2. **Bestaetigt.**
* Vorbild ROOM2060 (scd_dump selbst): sub12 @0x015BA Frage, @0x015C4 Ja, @0x015CC Ck(3,144,0) -> @0x015D0
  msg 1 (ohne Cut-Wechsel); sub18 @0x01678..@0x0169C; sub19 @0x0169E Set(3,144,1), @0x016A2 Cut_chg 9,
  @0x016B8 Aot_reset Slot 9 -> Textplatz msg 6, @0x016C8 msg 5, @0x016CE Cut_chg 10, @0x016D0 Cut_auto 1.
  Kein Se_on in sub18/sub19. **Bestaetigt.**
* msg 20/21 im Code (`k_msg20`/`k_msg21`) sind **bytegleich** zu ROOM2060.RDT @0x1855 (32 B) / @0x1875 (29 B)
  (eigener Vergleich). Die Laengen enden genau am naechsten Eintrag (0x1875 / 0x1892).
* RE2 `Sce_item_lost`: `re2_disasm.py table 0x800a7650` -> 0x800585e4; `dis 0x800585e4` zeigt @0x800585fc
  `lbu a0,1(v0)`, @0x80058600 `jal 0x800696cc`, @0x80058608 `bltz v0,0x80058634`, @0x80058618/24/30
  `sb zero` 0x800d4a3c/3d/3e, @0x80058634 `jal 0x80069714`, @0x80058640 `addiu v0,zero,1` und @0x80058644
  `addiu v1,v1,2`. **Bestaetigt**; der Port-Handler bildet genau das ab. Nachruecken:
  `re15_inv_remove_slot` -> `re15_inv_compact`, das den Ausruestungsplatz nachzieht
  (`inventory_common.c:91-92`).
* Feuerloescher: Im Original verlaesst er das Inventar nie. Der Port nimmt ihn ueber die Wegwerf-Stelle
  `gen/discard_sites.inc:44` `{ 0x1090, 9, 0x31 }` heraus. **Bestaetigt.**
* Fundstelle: `sicherung_1150.c:201` `re15_item_modal_start(RE15_SICHERUNG_ITEM, 1, …)`, gesperrt ueber
  Nimm-Bit 53. Das ist dasselbe Inventarbild wie `RE15_GIVE=0x40:1` (Id 0x40, Anzahl 1); die Weiche liest
  nur die Id.

## 6. Maengelliste (Schwere + Beleg)

| # | Schwere | Mangel | Beleg |
|---|---|---|---|
| M1 | kosmetisch | **Der Abschluss-Commit fehlt.** Dossier §9.3 kuendigt `feat(r34n-a): Rolltor ROOM1050/1051 …` mit voller Message an: alle Konstanten mit @0x und die PORT-WAHL-Begruendung zum Verzicht auf das ROOM2060-Zeigerpanel, die Gegenpruefungs-Auflage 1 ausdruecklich "in der Commit-Message" verlangt. Diesen Commit gibt es nicht; der Zweig endet mit wip-Commits. `1b74cb01` zitiert die meisten Konstanten, aber weder die PORT-WAHL-Begruendung noch Kamera Cut 7/8 @0x140/@0x160 noch die Sperre (2,7) @0x0DA4/@0x0DD2. -> Bei der Integration in der Commit-Message nachliefern. | `git log --format=%B cf0e68ba..38ad77a2` (kein "PORT-WAHL … Zeiger" in einer Bau-Message); Dossier Z. 591 |
| M2 | kosmetisch | **Der Dossier-Stand ist veraltet bzw. unvollstaendig**: Kopf Z. 3 "Stufe: ERMITTLUNG + BAUPLAN (kein Port-Code)", §9 Z. 540 "Stand: in Arbeit", §9.3 nennt den nicht existierenden Abschluss-Commit, §9.5 Zeile "3 \| (siehe unten)" ist leer. | `A_rolltor.md` Z. 3, 540, 591, 651 |
| M3 | kosmetisch (bekannt, dokumentiert) | **Nach dem Einsetzen zeigt der Normalblick Cut 3 den kleinen Kasten weiter mit roter Leuchte.** ROOM105.BSS hat fuer Cut 3 keine Kunstvariante (Dossier §8.1/§9.8.1). Die Leuchte ist ~2 px gross, im Ausschnitt aber sichtbar. Im Liefertext an den Nutzer nennen. | `A_belege/abn1_leon_am_schalter_cut3.jpg` (tuer_mash_mit F998/F1400 nach dem Einsetzen: rot) |
| M4 | kosmetisch (Nebenbefund, nicht Spur-Code) | **Die Memory `reai-v2-sicherung-raetsel-1050.md` nennt "2317 Pixel, bbox x136..268 y16..181"**; nachgemessen sind es 2761 px, bbox x123..268 y16..181 (>4). Ausserdem nennt die Memory noch "Shutter-Bedingung, Cut_chg 8, Message_on 2" als offen. -> Die Hauptsitzung soll die Memory nach der Integration berichtigen. | §5, eigener `probe_bg_dump`-Lauf |

**Abweichungen, die KEINE Maengel sind** (begruendet, von der Gegenpruefung als Auflage 1 so verlangt):
* Arbeitsauftrag Punkt 4 "Nahansicht zeigt beim Ansehen Cut 8": Das ist nicht gebaut. Nach dem Einsetzen
  faehrt der Schalterplatz den ausgelieferten sub02, Cut 8 erscheint genau einmal waehrend msg 21. Die
  Begruendung ist ROOM2060: Dort wird der Kasten nach dem Einsetzen zum reinen Textplatz msg 6 ohne
  Cut_chg (sub19 @0x016B8), und der Nutzerwortlaut verlangt keine zweite Nahansicht. Soll es doch eine
  geben, ist das eine neue Nutzerentscheidung.
* Kein Ton beim Einsetzen (ROOM2060 sub18/sub19 ohne Se_on); die Ja/Nein-Toene der Dialog-FSM bleiben.
* Das Tor oeffnet nicht automatisch nach dem Einsetzen; der Spieler drueckt erneut (wie in ROOM2060,
  wo Einsetzen und Bedienen getrennt sind).

**Nicht selbst nachgeprueft (fuer Spur A nicht tragend):** "Suedteil nur durch das Rolltor erreichbar"
(Dossier §7, Laufsteg ROOM1090). Das entscheidet nur, ob die Sicherung Pflicht ist, und die
Cut-4-Folge fuer Spur D. Die Softlock-Freiheit haengt nicht daran, denn die Fundstelle ROOM1150 liegt vor
dem Tor. Hinweis fuer die Integration: Die neue Quelldatei `rolltor_1050.c` braucht im Android-Bau ein
frisches Configure (memory reai-v2-android-glob-cache).

## 7. Belegdateien (A_belege/)

* `abnahme1_lauf.sh` — fuenf Laeufe (Modi `tuer_mash_mit`, `ohne_mash`, `mit_status`, `mit_erst_nein`, `wieder`)
* `abn1_laeufe.txt` — gefilterte debug.log-Zeilen und state.log-Wechsel aller Laeufe, dazu der nachgefahrene Kartenlauf
* `abn1_tuerweg_mash_mit_nahansicht.jpg`, `abn1_tuerweg_mash_mit_torfahrt.jpg` — mit Sicherung, echter Tuerweg
* `abn1_ohne_mash.jpg` — ohne Sicherung, Hammern
* `abn1_mit_start_waehrend_nahansicht.jpg` — START waehrend der Nahansicht
* `abn1_wiederbetreten_tor_offen_F1000.jpg` — Wiederbetreten nach dem Einsetzen, zweiter Druck, Tor offen
* `abn1_leon_am_schalter_cut3.jpg` — Ausschnitt Cut 3: Leon sichtbar am Kasten, rote Leuchte vor und nach dem Einsetzen
* `abn1_kunst_cut3kasten_cut7_cut8.jpg` — Kunst Cut-3-Kasten (vergroessert), Cut 7, Cut 8
