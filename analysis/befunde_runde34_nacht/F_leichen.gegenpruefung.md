# Spur F — Gegenpruefung des Bauplans (Skeptiker, vor dem Bau)

Geprueft: `analysis/befunde_runde34_nacht/F_leichen.md` (Stand d797e8a0), gegen AUFTRAG.md, VERTRAG.md,
CLAUDE.md (RE-Gate) und die Memory-Regeln. Kein Port-Code geaendert. Alle Bytes/Instruktionen unten
SELBST gelesen (RDT-Bytes per Python aus `re15_port/shared_assets/PSX` = bytegleich mit
`info/Re1.5/PSX`, `cmp` fuer 1110/1111/1230/1231/1011; MIPS per `re15_disasm.py` / `re2_disasm.py`).

## Urteil: haltbar_mit_auflagen

Der RE-Kern haelt: jede tragende Adresse, jedes Datei-Byte und die Zensuszahlen liessen sich
unabhaengig nachvollziehen, die Sonde laeuft reproduzierbar mit 0 Fehlern, der Plan trifft den
Nutzerwortlaut. Er hat aber vier echte Maengel, die VOR dem Bau behoben werden muessen:

1. **Haken-Stelle falsch:** der Message_on-Haken sitzt VOR dem Stimmen-Riegel "DEN VORIGEN SATZ
   AUSREDEN LASSEN" (scd_vm.c:1749-1768). Damit aendert der Bau mehr als "nur die Id" (Behauptung 3.1
   des Dossiers ist so falsch) — die Leichen-Texte wuerden eine noch laufende Aufnahme nicht mehr
   abwarten, und schon ihr Stimm-Aufruf gibt im neuen Raum den Clip-Cache des alten frei und bricht
   eine noch spielende Zeile ab (audio_pc.c:1923/:1976), auch ohne neue WAV.
2. **Softlock-Kopplung mit Spur E fehlt im Dossier:** F entfernt die einzigen Quellen der Codes
   4312 (Communication Room) und 5632 (Weapon Storage). Ohne E im selben Stand sind beide Tueren ohne
   Code. Das Dossier nennt die Kopplung (R8) "technisch unabhaengig"; E selbst (E_dokumente.md 7.5)
   verlangt gemeinsame Auslieferung.
3. **Uebersehenes RE1.5-Vorbild:** ROOM1011 (Roy) sub06/sub07 ist exakt das Muster "Koerper haelt
   etwas: 'He is holding something...' -> nehmen -> danach anderer Text", im selben Ereignisrahmen wie
   die Leichen 1110/1230. Der Satz "He is holding something" steht dort WOERTLICH (@0x012A3). Das
   Dossier behauptet, die Reihenfolge sei nur Nutzer-Vorgabe bzw. nur durch die Zeitbombe belegt.
4. **Die Sonde misst den geplanten Id-Tausch nicht:** sie legt den neuen Text unter die ORIGINAL-Ids
   0/10 (`re15_msg_install_text((unsigned char)msg, ...)`), nicht unter 20..23. Der eigentliche
   Mechanismus (Port-Nachricht 20..23 statt 0/10) ist ungemessen; der Riegel muss ihn tragen.

Keiner der Maengel stellt das Vorgehen (Original-Ereignis behalten, Text tauschen, danach das normale
Aufnahme-Modal) in Frage — deshalb "mit Auflagen", nicht "nicht haltbar".

## (a) Tragende Konstanten/Adressen selbst nachgeprueft

| # | Behauptung im Dossier | Selbst gelesen | Ergebnis |
|---|---|---|---|
| a1 | ROOM1110 main00 @0x00AEE Aot_set Slot 5, sce 3 -> sub02 | `2c 05 03 31 00 00 cc 29 66 08 e8 03 e8 03 ff 00 18 02 00 00` | stimmt |
| a2 | sub02 @0x0CEE..0x0D23 (Aot_reset 5 / Work_set / Plc_motion(1,11,0) / Sleep 30 / Message_on 0 / Evt_next / Plc_motion / Plc_flg / Sleep / Plc_ret / Aot_reset 5 -> sub02) | `46 05 00.. 2e 01 00 00 3f 01 0b 00 09 0a 1e 00 2b 00 ff ff 02 00 3f 01 0b 00 43 00 80 00 09 0a 1e 00 42 00 46 05 03 31 ff 00 18 02 00 00 01 00` | stimmt |
| a3 | ROOM1110 msg 0 @0x0D68 (3 Seiten, Zettel "4312") | `04 02 25 50 3a 4f ... 40 57 02 00 24 41 00 45 4f 00 44 4b 48 40 45 4a 43 00 3d 00 4f 48 45 4c 57 02 00 30 44 41 ... 61 05 01 10 0f 0d 0e 05 00 19 ... 57 01 00` | stimmt |
| a4 | ROOM1230 main00 @0x00D52 Aot_set Slot 18 -> sub21 (unbedingt, ausserhalb der Ifel-Bloecke) | `2c 12 03 31 00 00 48 0d 38 63 e8 03 e8 03 ff 00 18 15 00 00` | stimmt |
| a5 | sub21 @0x14A6..0x14DB, Message_on @0x014BC `2b 0a ff ff`, Re-Arm @0x014D0 | Dump bytegleich zu sub02 bis auf Slot 0x12 / Nachricht 0x0a | stimmt |
| a6 | ROOM1230 msg 10 @0x16F4 "A miserable death..." / "He is holding a slip." / "5632" | rdt_msgdump + Rohbytes | stimmt |
| a7 | Nachrichten-Anzahl 1110/1111 = 0..9, 1230/1231 = 0..11 | rdt_msgdump | stimmt -> Port-Ids 20..23 frei |
| a8 | Varianten bytegleich | 1111: sub02, Aot_set, Nachrichtenblock == 1110 (gleiche Offsets); 1231: sub21, Aot_set, Block == 1230 | stimmt |
| a9 | Msg 0 (1110) / Msg 10 (1230) nur vom Ereignis geoeffnet | Text-AOTs (sce 1) in 1110: msg 1..7 (Slots 11..19), in 1230: msg 6 (Slot 4); ROOM1230 msg 0 = Tastenfeld sub17 @0x013AA | stimmt; Schluessel (Raum, msg) ist Pflicht |
| a10 | Message_on-Handler: Tabelle 0x800744a8 + 0x2B*4 = @0x80074554 -> 0x800404f4 | `table 0x80074554` -> 0x800404f4; @0x80040500 `ori a1,zero,0x300`, @0x80040504 `lbu a2,1(v0)`, @0x80040508 `lhu a3,2(v0)`, @0x8004050c `addiu v0,v0,4`, @0x80040518 `jal 0x80027e68`, @0x8004051c `sll a3,a3,16` | stimmt |
| a11 | Item-AOT Typ 9 = LAB_80043328 | `table 0x8007469c` Eintrag [9] -> 0x80043328; @0x80043334 `bne` (Latch 0x80072d3b), @0x80043344 `lw a0,-16996(a0)` = 0x800bbd9c, @0x80043364 `sw a0,-13776(at)` = 0x800aca30 | stimmt |
| a12 | Modal-Zustand 7: nur Ja nullt die Zone und setzt das Bit | Sprungtabelle @0x800106b4 [7] -> 0x8001e048; @0x8001e054 `bltz v0,0x8001e0ec` (voll), @0x8001e068 `andi v0,v0,0x1` + @0x8001e06c `bne ...,0x8001e0ec` (Nein), @0x8001e090 `sb zero,0(v1)` (Delay-Slot, nur im Ja-Weg erreicht), @0x8001e0c4 `jal 0x8004dc4c` (a1 = `lhu a1,2(s1)` Menge), @0x8001e0d0 `jal 0x8004ef90` mit @0x8001e0d4 `addiu a0,s0,162` = 0x800b0fd6+0xA2 = 0x800b1078 (Bank 9), a1 = `lhu a1,4(s1)` | stimmt. Nebenbefund: @0x8001e0d8 `sh zero,6(s1)` (Nutzlast+6 genullt) nennt das Dossier nicht — fuer aot_slot -1 folgenlos |
| a13 | Modal-Freeze @0x8001dbb8/@0x8001dbc8 | @0x8001db90/94 t1 = &g_pauseflags (0x800aca40), @0x8001db98 `lui t0,0xff00`, @0x8001dbb8 `or v0,v0,t0`, @0x8001dbc8 `sw v0,0(t1)` | stimmt |
| a14 | SCD-Laeufer-Gate @0x8003f044-4c | @0x8003f040 `lw v0,-13760(v0)` g_pauseflags, @0x8003f044 `lui v1,0x200`, @0x8003f048 `and`, @0x8003f04c `bne v0,zero,0x8003f090`; Port scd_vm.c:656 `if (g_re15_pauseflags & RE15_PAUSE_SCD) return;` | stimmt |
| a15 | Bild-Reihenfolge main.c: SCD-Takt (:5378, bei Modal uebersprungen :5343) -> re15_msg_tick (:5503) -> re15_game_step (:7358) -> Modal-Tick (:7380) | Zeilen gelesen | stimmt |
| a16 | Haken-Stelle game_step: nach `re15_granate_tick()` (:1050), vor `if (re15_item_modal_active()) return;` (:1052) | gelesen; davor nur Game-Over-FSM + Sicherung/Granate (beide raumgebunden 1150) | stimmt |
| a17 | Item 0x15 = "H. Gun Bullets" | frueher live belegt (memory reai-v2-item-get-modal: Modal zeigt "H. GUN BULLETS" fuer ROOM1050 @0xb9a Typ 0x15); ROOM1110 @0x00B18 `50 07 09 31 ... 15 00 1e 00 e7 00 01 00` | stimmt |
| a18 | Mengen-Zensus RE1.5: 38 Saetze Typ 0x15, 22x15, 16x30 | EIGENER Zaehler (scd_dump_room.py ueber alle RDTs, nicht das Spur-Werkzeug): 38 / {15: 22, 30: 16} | stimmt (ohne Varianten-Doppel: 19 Plaetze, 11x15, 8x30 — Modus bleibt 15) |
| a19 | Bank 9 Bit 61/62 frei | eigener Zensus: Item_aot_set +18, Ck/Set Bank 9, 0x59 Bank 9 -> Bit 61/62 unbelegt; ZUSAETZLICH die 26 Flag-AOTs (sce 4, Aot_set/Aot_reset) geprueft, die das Spur-Werkzeug nicht erfasst: alle Gruppe 5 (raumlokal), keiner Bank 9. Port-Code in ALLEN Baeumen (r34n_*, r34g_*): nur 53..56 + Item-Bits | stimmt |
| a20 | Speicherstand traegt Bank 9 | re15_savedata.h:131 `flags[RE15_FLAG_ZONES][RE15_FLAG_WORDS_ZONE]` (16 x 256 Bit), re15_savedata.c:210 `memcpy(out->flags, ...)`, :274 `memcpy(g_game.flags, in->flags, ...)` | stimmt |
| a21 | Port-Modal: aot_slot -1 und taken_prop 0xFF folgenlos, Bit nur bei Ja | item_modal_common.c:334-342 (Voll/Nein -> Zustand 8), :340 `if (s_taken) re15_game_flag_set(9, s_taken, 1)`, :341 `if (s_aot_slot >= 0 ...)`, :360 prop 0xFF folgenlos; Halbierung :155 -> `re15_pickup_menge_nutzer` 15/2 = 7 | stimmt |
| a22 | RE2 ROOM4050 @0x00F1A Item-AOT Wolf Medal md1 0xFF action 1, @0x00F82 Text-AOT msg 8 gleiches Rechteck | `4e 06 02 31 01 00 74 dc d4 95 7e 09 aa 05 49 00 01 00 be 00 ff 01` / `2c 06 04 31 01 00 74 dc d4 95 7e 09 aa 05 08 00 00 00 ff ff`; msg 8 = "He's holding something. \| I don't need this right now." | stimmt |
| a23 | RE2 Item-Handler Typ 2 -> 0x80051884, action Bit 0 | `table 0x800a73c4` [2] -> 0x80051884; @0x800518cc `lbu v0,7(a0)`, @0x800518d4 `andi v0,v0,0x1`, @0x800518d8 `bne ...,0x80051924`, @0x80051924/2c `sb 6 -> 0x800cfbfd` | stimmt |
| a24 | RE2 Sce_Item_get 0x76 ohne Frage | Tabelle 0x800A74C8+0x76*4 = @0x800a76a0 -> 0x800587b8; @0x800587ec `lbu s0,1(v0)`, @0x80058860 `lbu a1,2(v0)`, @0x80058864 `jal 0x80069adc`, @0x80058874 `addiu v1,v1,3` | stimmt |
| a25 | Sonde reproduzierbar | `probe_r34n_f_leiche.exe alle alle` selbst gefahren: rc 0, "0 FEHLER"; 1110 soll-ja Runde 1 Modal F151..F245, Bit 1, 50 -> 57, Runde 2 kurz ohne Modal | stimmt — ABER: die Sonde installiert den neuen Text unter msg 0/10 (probe_r34n_f_leiche.c `re15_msg_install_text((unsigned char)msg, ...)`), der geplante Tausch auf 20..23 ist NICHT simuliert (Mangel 4) |

Fazit (a): keine zitierte Adresse ist ein Fehlziel; alle Sprungziele (0x800404f4, 0x80043328,
0x8001e048, 0x8004ef90-Aufruf, 0x800587b8, 0x80051884) wurden disassembliert, nicht nur die Aufrufstellen.

## (b) Passt der Plan zum Nutzerwortlaut und zu den Bildern?

Ja — keine bequemere Lesart gefunden. Einzeln:

- **Text 1110/1230, Seitenumbruch, Nachtext:** deckt sich mit AUFTRAG.md Z. 49 / Z. 61-64. Der
  Zettel-Satz faellt weg ("nicht mehr den gleichen Text ... sondern stattdessen"), die Codes wandern
  in Spur E ("Passend dazu") — Lesart L2 richtig.
- **Kleinschreibung "police"/"holding" (L6):** belegt durch den Vorlauf Runde 33: Nutzer schrieb
  "I have to Report the situation to the chief first..." (analysis/befunde_runde33/AUFTRAG.md Z. 14-15),
  ausgeliefert wurde "report" klein (tuer1120_1130.c k_meldung) — ohne Einwand des Nutzers. Die
  Grossbuchstaben sind Tipp-Gewohnheit, keine Vorgabe. Diesen Vorlauf im Code-Kommentar zitieren
  (Auflage 6).
- **Satzende "." (L8) und "..." (L9):** 1230 schreibt der Nutzer ausdruecklich "He is Holding
  something." — der Punkt ist Nutzer-Schreibung und schlaegt die RE1.5-Form "He is holding
  something..." (ROOM1011 msg 19, s. (f)). Fuer 1110 (ohne Satzzeichen beim Nutzer) ist Gleichlauf mit
  1230 die naheliegende Lesart. "A miserable death..." = Original-Seite 1 bytegleich. Haltbar.
- **Ja/Nein-Modal (L4/L5):** "bis man die Munition annimmt" setzt eine Wahl voraus; "zum mitnehmen
  erscheinen" = das Item-Bild erscheint. Das RE1.5-Vorbild ROOM1011 nimmt OHNE Frage (s. (f)) — dort
  gaebe es kein "bis man annimmt"; das Modal ist also die Nutzer-Lesart, nicht die bequemere.
- **Menge 15 -> 7:** "einmal Munition" = eine Packung; Halbierung/Stapeln gelten fuer jede
  Welt-Munition (Nutzerentscheidungen 2026-09-20/26). Haltbar, als PORT-WAHL mit Zensus markiert.
- **Bilder:** fuer Spur F gibt es keine Nutzerbilder (lights/elliot/marvin/interrogation gehoeren zu
  C/E, "irons items.png" ist Runde 30, Irons-Tisch).

## (c) Softlocks, Regressionen, Laden/Speichern, Raumvarianten

1. **REGRESSION Stimmen-Riegel (Mangel 1).** op_message_on (scd_vm.c) laeuft: Savepoint (:1641) ->
   Item-Box (:1658) -> `[msg]`-Log (:1680) -> Ja/Nein-Zweig (:1699) -> **Stimmen-Riegel (:1749-1768,
   `if (g_re15_voice_laeuft && g_re15_voice_restbilder > 0 ...) return 2`)** -> Besitz-Gate (:1790) ->
   Tuerton (:1799) -> Schreibmaschine (:1812-1819). Der Kommentar :1716 legt fest: "GENAU HIER, hinter
   allen Abfangungen" — vor dem Riegel stehen nur Nicht-Dialog-Abfangungen (Telefon, Box). Der Plan
   setzt den Leichen-Haken an :1682, also VOR den Riegel. Folge: die Leichen-Nachricht wartet eine
   laufende Aufnahme nicht mehr ab (heute tut sie das). Das ist schon OHNE neue WAV hoerbar: der
   geplante `scd_queue_voice(22)` landet in `re15_voice_play` -> `re15_voice_load_clip`
   (audio_pc.c:1923 `if (s_voice_room != room)`, Aufruf :2131), das beim ersten Aufruf im neuen Raum den Clip-Cache
   des alten Raums freigibt und einen noch daraus spielenden Strom abbricht (:1952-1976, Abbruch :1976, "XA-Stream
   geloest") — egal, ob fuer (1230, 22) eine Datei existiert. Konkreter Fall: Garage ROOM11B0 hat 13
   Aufnahmen bis 6,1 s (synchro/STAGE1/room11B0), Tuer 11B0 -> 1230 (main00 @0x00F88). Heute wartet
   msg 10 am Riegel, bis die Garagen-Zeile aus ist; mit dem Plan bricht sie ab. Abhilfe: Auflage 1.
2. **SOFTLOCK nur im Verbund (Mangel 2).** ROOM1110 msg 0 und ROOM1230 msg 10 sind die einzigen
   Stellen mit 4312/5632 (E_dokumente.md 3.7, eigener Byte-Befund a3/a6). Nach F ohne E: Communication
   Room (ROOM10D0 -> 10F0, Story-Ziel nach dem Irons-Hinweis) und Weapon Storage ohne Code. Abhilfe:
   Auflage 2.
3. **Kein Softlock im Ablauf selbst:** Nein/voll -> Zustand 8 -> 0, Faden laeuft ab @0x0D0A/@0x14C2
   weiter und legt den Platz @0x0D18/@0x014D0 wieder scharf (Sonde + a2/a5). Das Modal kann nur nicht
   starten, wenn schon eines laeuft — der Tick wartet dann (`re15_item_modal_active()` vor dem Start),
   der Faden steht solange (Text-Freeze vorbei, aber SCD durch das laufende Modal angehalten).
4. **Kein Doppel-Ausloesen durch die Bestaetigungs-Flanke:** das Modal geht in re15_game_step VOR dem
   Freeze-Gate auf, der Rest des Schritts (AOT-Scan) laeuft in diesem Bild nicht mehr; Zustand 1 liest
   keine Taste; Slot 5/18 ist bis zum Ereignisende stillgelegt.
5. **Laden/Speichern:** Bank 9 komplett im Stand (a20), kein neues Format; alte v9-Staende -> Bit 0 ->
   Angebot da. In 1110/1230 gibt es weder Telefon noch Box, speichern waehrend des Ereignisses geht nicht.
6. **Raumvarianten:** 1111/1231 bytegleich (a8), der Schluessel `RE15_ROOM_BASE` (re15_gameflow.h:82,
   `id & 0xFFF0`) deckt beide ab. Bit 61/62 gilt fuer Leon und Elza gemeinsam — getrennte Spielstaende,
   unkritisch. Stimme: der Pfad nimmt die VOLLE Raum-Id (audio_pc.c:1988 `room%04X`), Elza braucht
   eigene Dateien unter room1111/room1231 (Dossier R5 nennt das).
7. **Nachrichtentabelle:** `re15_msg_clear_room_block` (msg_common.c:259) raeumt beim Teardown alles,
   Einbau zur Message_on-Zeit deckt Tuer- und Ladeweg ab; PSX MSG_TABLE_N 32 > 23, Texte <= 63 B <
   MSG_RAW_LEN 128.
8. **Bestehende Riegel:** test_keypad.c haengt nicht an msg 10; discard_sites.inc (1230 msg 5, Panel
   leser_sub 20 = Sub-Index, keine Nachricht), lock_se_sites.inc (1230 msg 6) — kein Eintrag fuer
   (1110,0)/(1230,10). Keine Regression erkennbar.
9. **Latch-Robustheit (klein):** `s_angebot` wird beim Oeffnen gesetzt und erst geloescht, wenn der Text
   zu ist oder der Raum-BASIS-Wert wechselt. Endet ein Text je ohne Schliessen im selben Raum (Abbruch,
   Laden eines Standes im selben Raum), oeffnete das naechste Bild ein Modal ohne Text. Im
   Auslieferungsstand nicht erreichbar gefunden; billig abzusichern (Auflage 5).

## (d) Ressourcen-Kollisionen (VERTRAG.md, andere Spuren)

- Bank 9 Bit 61/62 = VERTRAG 1.1 Spur F; in keinem Baum (r34n_*, r34g_*) und keiner RDT belegt (a19).
- Nachrichten 20..23 = VERTRAG 1.3 Spur F; RDTs haben 0..9 / 0..11 (a7). E nutzt 20..23 nur in
  1000/1010/1020, B 20/21 nur in 1150 — raumlokale Tabelle, keine Kollision.
- AOT-Slots/obj_ids: keine neuen (Plan nutzt das Original-Ereignis) — nichts zu kollidieren.
- Haken-Zeilen: game_step neben `re15_granate_tick()` teilt sich die Stelle mit B
  (`re15_hebetisch_cursor_tick`, B_hebetisch.md Bauplan) — reiner Textkonflikt; op_message_on:
  r34g_* aendern scd_vm.c nicht (diff --stat gegen master geprueft), r34g_a aendert game_step_common.c
  nur im Waffenteil (:1766 ff./:1939 ff.). Mit Auflage 1 wandert der F-Haken hinter den Stimmen-Riegel.
- **Inhaltliche Kopplung E+F:** siehe (c)2 — die einzige echte Ressourcen-Abhaengigkeit dieser Spur.

## (e) Misst der Abnahmeplan das Nutzer-Symptom am Artefakt?

Ueberwiegend ja: echte exe, Framedumps je Seite, modal.log, `[leiche]`-Zeile, zweites Untersuchen,
Inventarbild, Ladepfad ueber eine Karte, Gegenproben an anderen Texten, und die Pflicht-Gegenprobe
"Haken aus -> rot". Luecken:

1. **Durchlauf nur als Kann:** 8.3 erklaert die Tuer-Laeufe fuer entbehrlich ("die Sprung-Laeufe
   decken die Funktion schon ab"). Das widerspricht memory reai-v2-playthrough-not-jumpin — der Weg des
   Nutzers ist die Tuer. Pflicht machen (Auflage 3).
2. **Der Riegel prueft den Id-Tausch nur, wenn er es ausdruecklich tut:** die Sonde, aus der er
   entsteht, installiert unter 0/10 (a25). Der Riegel muss nachweisen: Log/`g_scd.message_id` = 20/22
   (bzw. 21/23), die RDT-Nachricht 0/10 bleibt bytegleich im Speicher (fuer RE15_MSG_LOG/andere Leser),
   und die Gegenprobe "Haken aus" faellt (Auflage 4).
3. **Stimmen-Riegel ungetestet:** kein Teil deckt "laufende Aufnahme beim Untersuchen" ab
   (Auflage 1, Riegel-Teil).
4. **Voll-Fall nur in der Engine:** vertretbar (Nebenweg), an der exe optional.
5. Die Gegenprobe "Framedump = Original-Glyphen" ist mit `textvorschau.png` bereits gegen die echten
   Framedumps (`ist_1110_leiche.png`) gelegt — gut; im Bau am SOLL-Bild wiederholen.

## (f) Besseres Vorbild uebersehen?

**Ja — RE1.5 ROOM1011 (Elza-Variante des Verhoerraums, Officer Roy), selbst gelesen:**

    sub00 @0x00A52  2c 05 03 31 00 00 a0 f6 26 1b e8 03 e8 03 ff 00 18 06 00 00   Slot 5, sce 3 -> sub06
    sub00 @0x00A70  06 00 10 00 | 21 03 78 01 | 46 05 03 31 ff 00 18 07 00 00       wenn (3,120)=1: Slot 5 -> sub07
    sub06 @0x00E0A  46 05 00.. | 2e 01 00 | 3f 01 0b 00 | 09 0a 1e 00               Aot_reset / Work_set / Plc_motion(1,11,0) / Sleep 30
          @0x00E20  2b 13 ff ff | 02                                                 Message_on 19 "He is holding something..."
          @0x00E26  2b 14 ff ff | 02                                                 Message_on 20 "You've taken the Prison key."
          @0x00E2C  22 04 ec 01 | 22 03 78 01                                        Set (4,236)=1, Set (3,120)=1
          @0x00E34  3f 01 0b 00 | 43 00 80 00 | 09 0a 1e 00 | 42 00                  Plc_motion / Plc_flg / Sleep 30 / Plc_ret
          @0x00E42  46 05 03 31 ff 00 18 07 00 00 | 01 00                            Slot 5 -> sub07 (danach nur noch der andere Text)
    sub07 @0x00E4E  ... @0x00E64 2b 12 ff ff ...                                     msg 18 "He has passed out... He is holding a slip. / 0513"
    msg 19 @0x012A1 04 02 24 41 00 45 4f 00 44 4b 48 40 45 4a 43 00 4f 4b 49 41 50 44 45 4a 43 57 57 57 01 00

Das ist RE1.5s eigene Loesung fuer "Koerper haelt etwas -> nehmen -> danach anderer Text", im
bytegleichen Ereignisrahmen wie 1110 sub02 / 1230 sub21 (gleiche Plc_motion(1,11,0)/Sleep-30/Evt_next/
Plc_ret/Aot_reset-Folge). Es BESTAETIGT den Plan: Text vor der Aufnahme, Aufnahme direkt nach dem
Schliessen des Textes (dort die zweite Message_on nach Evt_next = die Stelle, an der F das Modal oeffnet),
Umschalten auf den Nachtext per Merkflag. Es korrigiert zwei Dossier-Aussagen: die Reihenfolge ist
NICHT nur Nutzer-Vorgabe (3.4) bzw. nur durch die Zeitbombe belegt (3.5), und "He is holding something"
muss nicht aus zwei Fundstellen zusammengesetzt werden — die Bytes stehen am Stueck @0x012A3 (gleiche
Bytes wie der Plan, also nur Beleg-, keine Byte-Aenderung). Unterschied: Roy gibt OHNE Frage; die
Ja/Nein-Wahl kommt aus dem Nutzerwort "bis man annimmt" (b). Das Vorbild gehoert als Beleg in Code und
Commit (Auflage 6).

**RE2:** das Dossier hat ROOM4050 aot 6 richtig gelesen. Nicht erwaehnt: im selben Zweig liegt
direkt anschliessend aot 7 (sub00 +0x0090 / +0x00AA, `4e 07 02 31 01 00 ce dc 64 9c 52 08 46 05 ...`,
Rechteck z -25500..-24150 gegen aot 6 z -27180..-25730) mit Small Key x1 bzw. **Shotgun Shells x7**,
md1 0xFF, action 1. Ob aot 7 eine zweite Leiche ist, habe ich nicht am Bild geprueft; fuer die Menge
aendert es nichts (andere Munitionssorte), die Aussage "kein RE2-Vorbild Leiche haelt Munition" ist
aber nur fuer "gleiches Rechteck wie ein Leichen-Text" belegt, nicht allgemein.

## Widerlegte Behauptungen des Dossiers

1. 3.1 "Der Bau tauscht NUR a2 (die Id) ... bleiben wie im Klartext-Zweig" — falsch fuer die geplante
   Haken-Stelle scd_vm.c:1682: sie liegt vor dem Stimmen-Riegel :1749-1768, der Bau schaltet fuer die
   zwei Leichen-Texte also auch das Abwarten laufender Aufnahmen ab.
2. 3.4/3.5 "Die Reihenfolge 'erst Text, dann Angebot' ... ist NUTZER-VORGABE" / "die Zeitbombe belegt
   nur die REIHENFOLGE in RE1.5 selbst" — unvollstaendig: ROOM1011 sub06 @0x00E0A (Roy) ist das direkte
   RE1.5-Vorbild im selben Ereignisrahmen.
3. 3.2/5.3 Glyph-Beleg "He is holding" (@0x0D8D) + " something" (@0x0EC4) als Zusammensetzung — der
   ganze Satz steht woertlich in ROOM1011.RDT @0x012A3 (msg 19); die Bytes des Plans stimmen, die
   Belegstelle ist die schwaechere.
4. R8 "technisch unabhaengig" (Spur E) — als Risikoaussage falsch: F ohne E = beide Codes weg
   (Softlock-Gefahr, von E selbst in E_dokumente.md 7.5 so benannt).
5. 0/2 "Belegt per Simulation ... 0 Fehler" als Beleg fuer den BAUPLAN — die Simulation installiert
   unter den Original-Ids 0/10 und misst den geplanten Tausch auf 20..23 nicht.
6. 8.3 "die Sprung-Laeufe (6.2) decken die Funktion schon ab" — widerspricht der Durchlauf-Regel
   (memory reai-v2-playthrough-not-jumpin); kein Ersatz fuer den Tuer-Weg.

## AUFLAGEN (nummeriert, umsetzbar)

1. **Haken hinter den Stimmen-Riegel legen.** `re15_leiche_message_on(...)` in op_message_on NICHT an
   :1682, sondern direkt NACH dem Block "DEN VORIGEN SATZ AUSREDEN LASSEN" (heute :1749-1768, nach
   `t->voice_wait = 0`) und VOR `re15_discard_besitz_vor_nachricht` (:1790). Die `[msg]`-Logzeile mit der
   Original-Id bleibt davon unberuehrt. Riegel-Teil `stimme`: `g_re15_voice_laeuft=1,
   g_re15_voice_restbilder=40` beim Druck -> die Leichen-Nachricht oeffnet erst nach Ablauf (wie heute
   msg 0/10), Gegenprobe mit Haken an der alten Stelle -> oeffnet sofort (rot).
2. **E+F-Kopplung festschreiben.** Im Dossier (R8) und in der Commit-Message: "F nur zusammen mit E
   (Dok 3 Marvin's Notes 4312 in ROOM1020, Dok 4 Armory Notice 5632 in ROOM1010) integrieren/ausliefern;
   F allein = Communication Room und Weapon Storage ohne Code." Die Integration prueft das mit einem
   gemeinsamen Lauf (Dok 3/4 lesbar UND Leichen-Texte ohne Code).
3. **Durchlauf-Abnahme verpflichtend.** `durchlauf_1110` (ROOM1100 Tuer @0x009BA -> 1110) und
   `durchlauf_1230` (ROOM11B0 @0x00F88 -> 1230) mit echtem Tuerweg, RE15_INPUT_SCRIPT, OHNE
   RE15_FORCE_CUT und ohne RE15_PLAYER_POS nach dem Eintritt; Framedumps von Text-Seite 2, Modal-Frage und
   dem zweiten Untersuchen ansehen und nach F_belege/ legen. Punkt 8.3 des Dossiers entsprechend aendern.
4. **Riegel misst den Id-Tausch.** Der Riegel ruft die ECHTEN Haken (nicht die Sonden-Simulation) und
   prueft: geoeffnete Nachricht = 20/21 (1110) bzw. 22/23 (1230), `re15_msg_get_raw(0)`/`(10)` nach dem
   Untersuchen unveraendert = RDT-Bytes @0x0D68/@0x16F4, Rohbytes 20..23 = die vier Texte, Varianten
   1111/1231 gleich. Gegenprobe "Haken aus" muss an der Id (0/10 statt 20/22) rot werden.
5. **Angebots-Latch absichern.** `s_angebot` nur scharf lassen, solange die zuletzt geoeffnete Nachricht
   die eigene Port-Id ist (`g_scd.message_id == lang_id` beim Schliessen) und im Raum-Teardown bzw. bei
   `RE15_ROOM_BASE`-Wechsel loeschen; als PORT-WAHL (Robustheit, keine Original-Adresse) kennzeichnen.
6. **Belege nachziehen (Code-Kommentar + Commit):** (a) RE1.5-Vorbild ROOM1011 sub06 @0x00E0A..0x00E4C /
   sub07 @0x00E4E / sub00 @0x00A70 als Muster "Text -> nehmen -> danach anderer Text" zitieren;
   (b) "He is holding something" mit der Stueck-Fundstelle ROOM1011.RDT @0x012A3 belegen (RE1.5 endet
   dort auf `57 57 57`; der Punkt 0x57 ist Nutzer-Schreibung aus dem 1230-Satz); (c) Kleinschreibung mit
   dem Runde-33-Vorlauf ("Report" -> "report", tuer1120_1130.c) begruenden; (d) RE2 ROOM4050 aot 7
   (Shotgun Shells x7, action 1) im Mengen-Abschnitt als geprueften Nicht-Treffer fuer Handgun-Munition
   vermerken.
7. **Paket/Android:** neue Datei `leiche_1110_1230.c` -> Android-GLOB neu konfigurieren
   (memory reai-v2-android-glob-cache); Stimmdateien-Liste fuer den Nutzer vollstaendig: room1110/
   main20.wav, main21.wav, room1230/main22.wav, main23.wav UND die Elza-Pfade room1111/, room1231/
   (audio_pc.c:1988 nimmt die volle Raum-Id).
