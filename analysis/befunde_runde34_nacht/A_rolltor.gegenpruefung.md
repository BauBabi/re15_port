# Spur A — Gegenpruefung des Bauplans (A_rolltor.md, Stand 24a3e97b)

Stufe: GEGENPRUEFUNG vor dem Bau. Rolle: Skeptiker, Dossier nicht selbst geschrieben. Kein Port-Code geaendert.
Alle Zahlen unten SELBST gelesen: SCD per `re15_port/tools/scd_dump_room.py`, Nachrichten per `rdt_msgdump.py`,
RDT-Tabellen (Kamera @RDT+0x24, RVD @RDT+0x28, Props @RDT+0x30) per Python aus `re15_port/shared_assets/PSX/…`,
EXE per `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (`info/Re1.5/PSX.EXE`) bzw. `re2_disasm.py`
(`info/re2leon/PSX.EXE`), Hintergruende per `probe_bg_dump.exe` (Engine-Dekoder), Port-Code dieses Baums.
Die Sonde `probe_r34n_a_rolltor.exe` habe ich unveraendert nachgefahren: Ausgabe bytegleich zu
`A_belege/sonde_r34n_a_rolltor.txt` (6 Faelle sauber, Gegenprobe 0x62 "still verschluckt").
Neues Belegbild: `A_belege/gegen_2060_zeigerpanel_vs_1050_weltkamera.jpg`.

## Urteil

**haltbar_mit_auflagen.**

Der Kern haelt: jede tragende Adresse/Byte-Folge stimmt (Tabelle (a)), die Lesart passt zum Wortlaut, der Bauweg
(Weiche in `scd_event_fire` + Port-Bytecode durch die vorhandene VM + RE2 `Sce_item_lost` 0x62 mit PC-Schranke)
ist tragfaehig, Ressourcen bleiben im Vertrag, ein Softlock ist ausgeschlossen.

Widerlegt bzw. falsch begruendet sind aber zwei Aussagen, und eine dritte wiegt schwerer als das Dossier meint:

1. **§3.3 "Feinbedienung (Objekt 4) nicht uebernommen: ROOM1050 hat fuer den Kasten kein bewegliches Modell …
   ohne Modell gaebe es nichts zu fuehren" — sachlich falsch.** Objekt 4 in ROOM2060 ist kein Sicherungsmodell,
   sondern der ZEIGER (Cursor): obj 3 (Generator-Panel) und obj 4 (Sicherungs-Panel) tragen dasselbe Modell
   (TIM @0x382B0, MD1 @0x33E4, md5 gleich). Das vollstaendige RE1.5-Einsetzen ist ein Zeiger-Panel. Der Verzicht
   darauf bleibt richtig, aber aus einem ANDEREN Grund (Kunstart von ROOM1050 Cut 7/8), siehe (f).
2. **§7 "Spur D: keine Ueberschneidung (Slot 4 liegt suedlich des Rolltors)" — widerlegt.** Solange das Tor zu ist,
   fuellt das Tor-Prop Cut 4 (D §2.3, Bild `r34n_adaruf/…/D_belege/ist_cut4_rolltor_zu_offen_cut5.png`); Ds Szene an
   der 10A0-Tuer liegt in Cut 4. Die Sperre verlaengert genau diesen Zustand.
3. **§8.3 (Suedteil ohne Rolltor erreichbar?) ist nicht nur eine Softlock-Frage** — er entscheidet, ob die Folge aus 2.
   im normalen Spiel auftritt. Er muss vor dem Bau geschlossen werden.

## (a) Tragende Konstanten/Adressen selbst nachgeprueft

| Behauptung (Dossier) | Selbst gelesen | Ergebnis |
|---|---|---|
| sub00 @0x0C1E `21 03 79 00`; @0x0C22 Aot_set Slot 7 sce 3 sat 0x31 Rechteck (16800,-8950,800,800), Nutzlast `ff 00 18 02` | Dump ROOM1050 sub00 | OK |
| sub02 @0x0CAC `2b 00 80 ff`, @0x0CB2 `06 00 d0 00` (Blockende 0x0CB2+4+0xD0 = 0x0D86 = hinter Endif @0x0D84), @0x0CB6 `21 0c 1f 00`, @0x0CBA `22 03 79 01`, @0x0CBE Aot_reset 7, @0x0CD0 `29 03`, Se_on @0x0CD2/0x0CE2/0x0D3C, @0x0D74..@0x0D82 Rueckgabe | Dump sub02 | OK — Blocklaengen-Regel des Plans (§5.3) damit belegt |
| msg 2 @0x0ED2 "I need a fuse to run the shutter.", nie aufgerufen | msg_sec 0xE44, 9 Eintraege; Message_on im SCD nur id 0 @0x0CAC und id 8 @0x0DD6 | OK |
| ROOM1051 = ROOM1050 (sub02, Slot 7, msg 0/2, Kameras) | sub02 @0x0CC8..0x0DA4 bytegleich; Kameras 0x60..0x19F, RVD 0x1B0..0x367 und SCA ab 0x36C (0x400 B) bytegleich; Slot 11 @0x0C0E / Slot 12 @0x0C22 original belegt | OK (auch RVD/SCA, die das Dossier nicht nannte) |
| Kamera Cut 7/8 @0x140/@0x160 gleich, pri 0x518/0x51C | pos (16171,-2733,-8185) tgt (18240,-1974,-8706), flag 0 | OK |
| RVD @0x1B0: Cut 3 Heim @0x250, 3→4 @0x278, 4→3 @0x2A0, Cut 7/8/9 nur Anker @0x32C/@0x340/@0x354 (19600..22100, -6900..-4900) | eigener RVD-Walk (20-B-Saetze, Ende `ff`) | OK |
| ROOM2060 sub12 @0x015BA..0x015D6, sub18 @0x01678..0x0169C, sub19 @0x0169E..0x016D2, msg 4 @0x1855 / msg 5 @0x1875 | Dump + msgdump ("Will you use the Fuse?" / "You've used the Fuse.") | OK |
| RE1.5 Opcodes (Tabelle 0x800744a8) | [0x29]=0x800402a0: @0x800402d4 `ori 0x100`, @0x800402e4 `sb`→0x800b3f7b, @0x800402fc `sh`→0x800b0fe4, @0x80040310 pc+2; [0x2A]=0x8004032c; [0x2B]=0x800404f4: a1=0x300 @0x80040500, id @0x80040504, Maske @0x80040508 `<<16` @0x8004051c, pc+4 @0x8004050c; [0x3C]=0x800403ac: ==1 → `and ~0x100` @0x800403d8 | OK |
| §3.5 Cut_replace = RVD-Umetikettierung | [0x4B]=0x80040414: `lw a3,40(v0)` @0x80040434 = RDT+0x28, Tausch +2/+3, Schritt 20 @0x80040498, Ende `0xff` | OK — Korrektur am Vor-Dossier bestaetigt |
| sce-3-Handler | Tabelle 0x8007469c[3]=0x800430f0: @0x800430fc `lhu a0,0(v0)`, @0x80043100 `lbu a1,3(v0)`, @0x80043104 `jal 0x8003ee3c` | OK |
| Bank 9 / Bank 12 | Flag-Tabelle 0x80074664: [9]=0x800b1078 (32 B = 256 Bit), [12]=0x800b8520, [2]=0x800aca40 | OK |
| RE2 Sce_item_lost | 0x800a74c8[0x62] @0x800a7650 = 0x800585e4; @0x800585fc `lbu a0,1(v0)`, @0x80058600 `jal 0x800696cc`, @0x80058608 `bltz`, @0x80058618/24/30 `sb zero` 0x800d4a3c/3d/3e, @0x80058634 `jal 0x80069714`, @0x80058640 v0=1, @0x80058644 pc+2 | OK |
| … SPRUNGZIEL (memory zitierte-adresse) | 0x800696cc selbst disassembliert: Schleife ueber 0x800d4a3c+4i, Anzahl @0x800d46ac, Rueckgabe Index/-1 = Inventarsuche | OK |
| RE2 Keep_Item_ck | [0x5E]=0x800584f0: `jal 0x800696cc`, `nor`/`srl 31` | OK |
| RE2-Einsetzstellen | room1110 sub04 (Cut_chg 7, Se_on(2,15), Sce_Item_lost(74), Cut_chg 6/Cut_auto 1), room10B0 sub10 (Cut_chg 9, Sce_Item_lost(51), Se_on(2,11), Cut_chg 3/Cut_auto 1), room60D0 sub06 (Sce_Item_lost(77), Aot_on 1, Cut_old) — Decompilate `info/re2leon/PL0/RDT/room*/scd/*.c` | OK |
| Port-Gegenstueck Entfernen | FUN_8004dfec: Anzahl @0x8004dff0 = 0x800b0fbc, -1 @0x8004e048; `sb zero` @0x8004aef0/@0x8004af0c; `re15_inv_compact` zieht den Ausruestungsplatz nach (inventory_common.c:91-92) | OK |
| Item 0x40 nicht kombinierbar | 0x80074da8 + 0x40·12 = 0x800750a8: `01 00 00 00 88 4c 07 80 00 00 00 00`, Paarzahl (Byte 9) 0. ZUSAETZLICH: alle Zeilen 0x00..0x5F gescannt — KEIN Paar mit 0x40 als Partner oder Ergebnis | OK (auch von der Gegenseite) |
| Bank-9-Bit 63 frei | eigener Zensus mit `scd_walk_lib` (206 RDTs, 40 674 Opcodes, Ck/Set Bank 9 + Item_aot_set-Nimm-Bit +18): Bits 53..84 unbenutzt; kein r34*-Zweig aendert scd_vm.c/aot_common.c/msg_common.c (`git diff merge-base..Zweig`) | OK |
| Port-Einhaengestelle | `scd_event_fire` scd_vm.c:588; dort liegt bereits eine Umleitung derselben Art (`s_power_gates`, scd_vm.c:566-585); `s_opcode_sizes[0x62]=1` (scd_vm.c:227); 0xFE Dbg_text (scd_vm.c:240/1374); `re15_msg_install_text` msg_common.c:268, Teardown-Clear msg_common.c:256 | OK |

Zwei Negativaussagen des Dossiers habe ich zusaetzlich an den Bytes gegengeprueft (Lehre "airtight"-Verdikte):
* RE1.5 Opcode 0x5E (0x80074620 → 0x80042b04) ist KEIN Besitztest: `ori 0x20` auf 0x800aca3c (@0x80042b20),
  `jal 0x80013278` mit pc[1]/pc[2..3] (Tabellenlauf), pc+4 — kein Praedikat. Bestaetigt.
* Die RE1.5-Inventarauswahl FUN_8004aa24 schickt Nicht-Waffe/Nicht-Heilmittel in Zustand 6 (Tabelle 0x80074c28[6]
  = 0x8004b250): nur `jal 0x80027e68` (Meldung) und Warten auf 0x800b8520&0x80 — kein Verbrauch. Schluessel-
  gegenstaende verlassen das RE1.5-Inventar nie. Bestaetigt.

## (b) Plan vs. Nutzerwortlaut (AUFTRAG.md) und Bilder

* Fuer Spur A gibt es kein Nutzerbild. Die Lesart ("die CUT" = Cut 7, "der Text mit der Sicherung" = msg 2,
  "Nahansicht fuer Sicherung und Nachricht" = Cut 7 zur Nachricht, Cut 8 beim Einsetzen) deckt den Wortlaut
  vollstaendig; keine bequemere Lesart gewaehlt.
* Reihenfolge Schalterfrage → Sperre ist mit ROOM2060 sub12 belegt (@0x015BA Frage, @0x015C4 Ja, @0x015CC Ck(3,144,0),
  @0x015D0 msg 1) — haelt.
* Zweischritt (Einsetzen, dann erneut druecken) haelt: auch in ROOM2060 sind Einsetzen (sub18/19) und Bedienen
  (sub12) getrennte Handlungen.
* Nachtrag zum Auftragspunkt 4 ("Nahansicht zeigt beim Ansehen Cut 8"): der Plan zeigt Cut 8 nur im Einsetzablauf.
  Das ist RE1.5-konform — ROOM2060 macht den Kasten nach dem Einsetzen zum reinen Textplatz msg 6 "The fuse is in
  place." (sub00 @0x010DE, sub19 @0x016B8), ohne Nahansicht. Das Dossier muss das so begruenden (Auflage 1).
* §8.1 "Rote Leuchte in Cut 3 … dem Nutzer vorlegen" widerspricht VERTRAG §3.2 (nicht fragen). Der Kasten ist in
  Cut 3 nur ~17×28 px (x124..141 y68..96) mit einem roten Punkt; als bekannte Grenze melden, nicht fragen (Auflage 5).

## (c) Softlocks, Regressionen, Laden/Speichern, Raumvarianten

* **Softlock: keiner** (selbst geprueft): Sicherung vor dem Rolltor erreichbar (Leon: der Port sperrt 1130→1120 bis
  (3,94), und (3,94) faellt in ROOM1150 = Fundraum); nicht wegwerfbar (`gen/discard_sites.inc` fuehrt 0x40 nicht,
  ROOM2060 msg 5 dort ausdruecklich VERWORFEN); nicht kombinierbar (beidseitig, (a)); Kiste nur ROOM1150/1151
  (`re15_itembox.c` s_boxpoints) = Ablage; das Inventar verbraucht Schluesselgegenstaende nicht (FUN_8004aa24 Zustand 6).
  Die Weiche liest das Inventar zum Ausloesezeitpunkt; in ROOM1050 kann sich der Besitz waehrend des Fadens nicht
  aendern (keine Kiste, keine Fundstelle; Frage/Text frieren mit 0xFF80/0xFFFF die VM, (2,7) sperrt die Eingabe bis
  auf 0xf000 — game_state.c:212-255).
* **Leon in der Nahansicht:** nicht Standort-Zufall, sondern Mechanismus — der Spieler wird gegen das Regionsviereck des
  aktiven Cuts gecullt (main.c:8104-8106, byte-true FUN_80039ca0→FUN_80014368), und Cut 7/8 haben nur das
  Blindrechteck @0x32C/@0x340. Gilt also fuer JEDEN Ausloese-Standort; das Dossier sollte den Mechanismus nennen.
* **Regression im Originalablauf — Cut 4 (Folge der Sperre, Dossier sieht sie nicht):** Tor-Prop obj 0 (sub00
  @0x0C36, (15432,0,-10424)) liegt im Regionsviereck von Cut 4 (@0x28C: (12800,-25000),(12800,-8500),(17800,-8500),
  (20800,-25000)) und wird dort gezeichnet (Prop-Cull main.c:9128ff); Kamera 4 steht NOERDLICH des Tors
  ((14999,-3549,-8140) @0xE0) und blickt nach Sueden. Wer bei (3,121)=0 von Sueden kommt, sieht in Cut 4 nur die
  Torplatte (D §2.3 gemessen). Ob das im normalen Spiel vorkommt, haengt an §8.3:
  * NICHT ueber ROOM1000 (Ds Praemisse ist falsch): ROOM1000-RVD @0x190..@0x348 hat drei GETRENNTE Kameragruppen
    {0,1,2} (x 14500..25000, Umkleide), {3,4,5} und {6,7,8} (x -6900..3600, die WCs), ohne Uebergang. Die Umkleide
    haengt nur am Suedteil (ROOM1050 Slot 3 ↔ ROOM1000 Slot 0 @0x00BBE → (14700,0,-13500) Cut 4), die WCs nur am
    Nordteil (Slot 1/2). `raumgraph.py` modelliert das richtig.
  * MOEGLICHERWEISE ueber 1110 → 10D0 → 10F0 → 1090: Blue Keycard 0x38 liegt in ROOM1110 (main00 @0x0B2E, Nimm-Bit
    52), 1110 ist ueber 1120→1060→10C0→10D0→1100→1110 ohne Rolltor erreichbar (Tuergraph
    `r34n_adaruf/…/D_belege/tuergraph_stage1_leon.txt`; die 10D0-Tueren Slot 14/15/16 @0x01196/@0x011B6/@0x011D6 stehen
    HINTER dem If/Else-Block @0x01022..@0x0119A, also unbedingt — Ds Etikett "[Else]" ist ein Werkzeug-Artefakt), die
    Code-Schranke setzt (3,50) @0x0152A, 10D0 Slot 0 (@0x01174, Else-Zweig) → 10F0, 10F0 Slot 1 → 1090 (Ankunft
    (-13600,-9000,1300), obere Ebene), 1090 Slot 0 → ROOM1050 (19850,0,-22300) Cut 6. Die 1090-Kameragruppen sind in
    der RVD verbunden (@0x2A8 Cut 0→3, @0x2F8 Cut 1→5). OFFEN und entscheidend: ob der Abstieg zur Tuer Slot 0 vor dem
    Loeschen begehbar ist — der Feuerloescher (ROOM1000 Item_aot_set @0x0C24, (20500,-800) = Umkleide) liegt nur vom
    Suedteil aus erreichbar.
  → Ist der Abstieg frei, wird "Suedteil bei geschlossenem Tor" mit der Sperre zum Normalfall fuer jeden, der den
  10D0-Weg vor der Sicherung geht, und Ds Szene an der 10A0-Tuer (Cut 4) waere dann unsichtbar. Sperrt das Feuer, ist
  das Rolltor fortschrittsnoetig (dann keine Cut-4-Folge, aber die Sicherung wird Pflicht — Fund bleibt erreichbar).
* **Laden/Speichern:** Bank 9 liegt in `g_game.flags`, komplett gespeichert/geladen (re15_savedata.c memcpy), Inventar
  im selben Satz — konsistent. Die Weiche entscheidet beim Ausloesen, also richtig nach Laden/Wiederbetreten. Die
  Abnahme #6 ("CONTINUE-Lauf mit Karte (9,63)=1") nennt aber keine Herkunft der Karte (Auflage 4d).
* **Raumvarianten:** 1051 vollstaendig gleich (inkl. RVD/SCA, (a)); Fund in 1151 abgedeckt (sicherung_1150.c:84).
  Nur der Riegel faehrt 1051, die echte exe nicht (Auflage 4a).
* **Bestehende Tests:** nur `test_room1050_sicherung.c` (6) feuert Ereignis 2 in ROOM1050 (grep ueber tests/) — im Plan
  vorgesehen.
* **Alte Spielstaende:** (3,121)=1 → kein Slot 7, Sicherung bleibt nutzlos im Inventar; (3,121)=0 → normaler Ablauf. OK.

## (d) Ressourcen-Kollisionen (VERTRAG.md, andere Spuren)

* Bank-9-Bit 63 (64 Reserve), Nachrichten 20/21: im Vertrag, frei (Zensus (a)). Keine AOT-Slots, keine Props — richtig,
  denn die Vertragsslots 11/12 sind in ROOM1051 original belegt (@0x0C0E/@0x0C22); das Dossier hat den Vertragsfehler
  selbst gefunden.
* Opcode 0x62 wird global registriert, die PC-Schranke isoliert ihn gegen jede andere Spur/RDT — gut.
* `scd_event_fire`: A setzt HAKEN 1 in dieselbe Funktion, in der `s_power_gates` schon umleitet; falls D ebenfalls eine
  Ereignis-Umleitung braucht, besser EINE Tabelle als zwei Nachbarzeilen (Auflage 7, Soll).
* `local_build.sh` RE15_MIN_TESTS: B (B_hebetisch.md:455/552) und C (C_generator.md:489/570) heben dieselbe Zahl →
  sicherer Zusammenfuehrungskonflikt (Auflage 6).
* Spur E (Leiche, Slot 15, Zone (15250,-7250,1000,1000)): raeumlich getrennt vom Schalter-Rechteck; E misst 0
  Abfangungen in 1050 — keine Kollision mit A.
* Spur D: siehe (c) — Wechselwirkung ueber Cut 4, nicht ueber Slots/IDs.

## (e) Abnahmeplan: misst er das Nutzer-Symptom am Artefakt?

Im Kern ja (echte exe unter eigenem Namen, echter Eingabepfad, Framedumps, state.log, Ton-Log). Luecken:
1. Kein Lauf fuer die Folge in Cut 4 (Suedteil bei geschlossenem Tor) — genau das, was der Nutzer als naechstes sieht.
2. ROOM1051 nur im Riegel, nicht an der echten exe.
3. "Statusschirm ohne Sicherung" steht in der Tabelle, `lauf.sh` hat dafuer keinen Modus/Framedump.
4. Kein Beleg gegen Doppelausloesen (Viereck schliesst msg 2/21 UND ist die Aktionstaste des Schalters).
5. Laden/Speichern ohne reproduzierbares Rezept.
6. `RE15_GIVE` statt der echten Aufnahme — vertretbar, weil die Weiche nur das Inventar liest; einmal die echte Kette
   (Hebetisch 1150 → Tueren → 1050) waere die staerkere Abnahme (Soll).

## (f) Uebersehene bessere Vorbilder (Original / RE2)

1. **ROOM2060 ist ein Zeiger-Panel, kein "Fuehren der Sicherung"** (Bild `A_belege/gegen_2060_zeigerpanel_vs_1050_weltkamera.jpg`):
   * Prop-Tabelle @0x200: obj 3 und obj 4 = TIM @0x382B0 (33 312 B) + MD1 @0x33E4 (5 556 B), bytegleich (md5 26ac39af /
     91418c7c) — dasselbe Zeigermodell (obj 5..8 = die vier Hebel, TIM @0x404D0/MD1 @0x4998).
   * Cut 7 (Generator) und Cut 8/9 (Sicherung) sind SENKRECHTE Panelkameras: pos (-19628,-17442,22616) → tgt
     (-19628,15928,22617) bzw. (19572,-17442,-22584) → (19572,15928,-22583), genau ueber den geparkten Zeigern
     obj 3 (-19554,0,22684) @0x00EF0 / obj 4 (19571,0,-22542) @0x00F12; die Hintergruende sind gerahmte Panels.
   * Sockelzone = Aot_set Slot 7 sce 5 flags 0x44 (Objekt-Pool) @0x0100C, Rechteck (19700,-24300,2050,2050); sub01
     @0x012B4..@0x0130C bewegt obj 4 per Sce_key_ck 0x01/0x04/0x02/0x08 (sub14..17: Speed_set(2|0,±200)+Add_speed),
     @0x01314..@0x01332 Ck(5,11,1) + Member_cmp(0x0F==7) + Sce_key_ck 0x40 → sub19.
   * Warum der Verzicht trotzdem richtig ist: ROOM1050 Cut 7/8 sind WELTKAMERA-Nahansichten ohne Panelrahmen (y=-2733,
     Blick fast waagerecht nach Osten), ROOM1050.RDT hat kein Zeigermodell (nOmodel=2) und keine sce-5-Zonen. Die
     RE1.5-Form fuer Weltkamera-Nahansichten ist die DIREKTE Frage: ROOM1051 sub03 (Cut_chg 9 @0x0DB4, Message_on 5
     @0x0DB6, im selben Raum) und ROOM1090 sub06 (@0x02702 msg 7, @0x02708 msg 8 "Will you use …", @0x02712 Ja →
     Wirkung). Ein Zeiger ueber Cut 7 muesste Ebene, Zonen und Zeigerlage erfinden = raten.
2. **RE1.5s Sicherung ist ein reines Flag, nie Inventar:** ROOM2030 sub06 @0x01FE4 `2b 02 ff ff` "Will you take the
   Fuse?" → @0x01FEE Ck(12,31,0) → @0x01FF2 `22 03 6c 01` Set(3,108,1) → @0x02000 `2b 03 ff ff` "You've taken the
   Fuse." — kein Item_aot_set/Aot_on (Zensus: 0 Ausgaben fuer 0x40). ROOM2060 prueft (3,108) (sub00 @0x010A2). Das
   erklaert, warum RE1.5 kein Entfernen braucht, und macht die RE2-Wahl (`Sce_item_lost`) fuer das Port-INVENTAR-Item
   wasserdicht — gehoert in §3.4.
3. **RE1.5-Werkzeug fuer zustandsabhaengige Kameras am Rolltor:** ROOM1130 sub00 @0x009D2 Ck(3,107,1) → Switch auf
   den aktiven Cut → Cut_replace 9,8 @0x009E0/@0x009EC/@0x009F8; ROOM2060 sub00 @0x010F2/@0x010F5. ROOM1050 hat fuer
   "Sued, Tor zu" keinen solchen Ersatz — relevant fuer die Cut-4-Folge ((c)); nicht raten, erst pruefen, ob ROOM105.BSS
   ueberhaupt einen passenden Cut hat.
4. RE2 setzt beim Einsetzen immer einen Raum-SE (room1110 sub04 Se_on(2,15,…), room10B0 sub10 Se_on(2,11,…)); der Plan
   bleibt stumm nach ROOM2060 (kein Se_on in sub14..19) — vertretbar, weil RE1.5 dort fertig ist.

## Widerlegte Aussagen des Dossiers

1. §3.3: "Die Feinbedienung (Objekt 4 per Richtungstaste) wird NICHT uebernommen: ROOM1050 hat fuer den Kasten kein
   bewegliches Modell … ohne Modell gaebe es nichts zu fuehren." — Objekt 4 ist der Panel-Zeiger (Modell identisch mit
   obj 3, TIM @0x382B0 / MD1 @0x33E4), kein Sicherungsmodell; die Begruendung traegt nicht.
2. §7: "Spur D (Tuer Slot 4 nach 10A0): keine Ueberschneidung (Slot 4 liegt suedlich des Rolltors)." — Bei (3,121)=0
   fuellt das Tor-Prop Cut 4 (obj 0 @0x0C36 im Regionsviereck @0x28C), und Ds Szene liegt in Cut 4.
3. Nebenbefund fuer D (nicht As Dossier): D §2.3 "ueber ROOM1000 erreicht (der Normalfall …)" — ROOM1000 hat drei
   getrennte Kameragruppen (RVD @0x190..@0x348); ohne Rolltor ist der Suedteil, wenn ueberhaupt, nur ueber
   1110→10D0→10F0→1090 erreichbar (Abstieg in 1090 bei brennendem Feuer ungemessen).

## Auflagen (nummeriert, umsetzbar)

1. **§3.3 berichtigen und neu begruenden.** ROOM2060 als Zeiger-Panel beschreiben (Belege (f)1). Den Verzicht als
   "PORT-WAHL — Grund: ROOM1050 Cut 7/8 sind Weltkamera-Nahansichten (Kamera @0x140/@0x160, y=-2733, ohne Panelrahmen),
   kein Zeigermodell/keine sce-5-Zonen in ROOM1050.RDT; RE1.5-Form fuer Weltkamera-Nahansichten = direkte Frage
   (ROOM1051 sub03 @0x0DB4/@0x0DB6, ROOM1090 sub06 @0x02702..@0x0272A)" kennzeichnen — im Dossier, im Kopfkommentar
   von `rolltor_1050.c` und in der Commit-Message. Den Satz "ohne Modell gaebe es nichts zu fuehren" streichen. Dazu
   Auftragspunkt 4 beantworten: nach dem Einsetzen keine Nahansicht, wie ROOM2060 (Textplatz msg 6, sub00 @0x010DE).
2. **§3.4 ergaenzen:** RE1.5-Sicherung = Flag (ROOM2030 sub06 @0x01FE4/@0x01FF2/@0x02000, ROOM2060 sub00 @0x010A2), dazu
   die zwei gegengeprueften Negativbelege (0x5E @0x80042b04 ist kein Besitztest; FUN_8004aa24 Zustand 6 @0x8004b250
   verbraucht nichts).
3. **§7/§8.3 schliessen, Cut-4-Folge aufnehmen — vor dem Bau.** (a) Am Artefakt messen, ob 1090 von der Ankunft aus
   10F0 ((-13600,-9000,1300)) zur Tuer Slot 0 (→ 1050 Cut 6) begehbar ist, und zwar MIT brennendem Feuer (der Loescher
   ist nur vom Suedteil aus erreichbar) — Lauf mit RE15_DEBUG_JUMP 10F0 + Eingabeskript, oder SCA-/Feuer-Beleg;
   (b) wenn ja: Lauf 1090 → 1050 bei (3,121)=0 nach Norden bis in Cut 4 (Zone 5→4 @0x2DC), Framedumps nach
   `A_belege/`, und die Folge im Dossier (§7) und im Liefertext als bekannte Folge der Sperre nennen, mit Mechanismus
   (obj 0 @0x0C36 im Regionsviereck @0x28C, Kamera 4 @0xE0 noerdlich des Tors); (c) an D uebergeben (Ds Szene an Slot 4
   liegt in Cut 4; Ds ROOM1000-Praemisse ist falsch, ROOM1000-RVD @0x190..@0x348). Keine Abhilfe erfinden; falls die
   Integration eine will, ist das RE1.5-Werkzeug Cut_replace in sub00 (ROOM1130 @0x009E0) — erst pruefen, ob
   ROOM105.BSS dafuer einen Cut hat.
4. **Abnahme §6 ergaenzen:** (a) ROOM1051 an der echten exe (ohne/mit Sicherung); (b) `lauf.sh`-Modus mit Framedump des
   Statusschirms nach dem Einsetzen (Sicherung weg, kein leeres Loch im Raster); (c) Doppelausloesen ausschliessen:
   Zahl der `[msg] room=1050 id=0`-Zeilen == Zahl der Schalterdruecke, nach dem Schliessen von msg 2 bzw. msg 21 kein
   neues Oeffnen ohne neuen Druck (state.log/debug.log); (d) Laden/Speichern reproduzierbar: Riegel-Fall "nach dem
   Einsetzen re15_savedata exportieren → Zustand loeschen → importieren → scd_event_fire(2) startet den AUSGELIEFERTEN
   sub02 (PC im RDT-Puffer auf 0x0CAC), Inventar ohne 0x40", plus ein ausgeschriebenes Rezept fuer den echten
   CONTINUE-Lauf (Karte im Lauf erzeugt, Speichern in 1150/1120); (e) Wiederbetreten als Riegel-Fall (Raumaufbau mit
   (9,63)=1 → sub02); (f) der Cut-4-Lauf aus Auflage 3. Der Riegel prueft wie geplant das ERGEBNIS ((9,63)=1,
   Sicherung weg, msg 21 gezeigt), nicht nur das Fadenende.
5. **§8.1 umformulieren:** keine Frage an den Nutzer (VERTRAG §3.2), sondern bekannte Grenze im Liefertext: Cut 3 zeigt
   den Kasten (x124..141 y68..96, ~17×28 px) weiter rot; ROOM1050 hat keine Kunstvariante (RE1.5 loest das in ROOM2060
   mit Varianten-Cuts 10/11 per Cut_replace @0x016C2/@0x016C5).
6. **`local_build.sh` RE15_MIN_TESTS** im Spurzweig nicht anheben (eigener Lauf mit `RE15_MIN_TESTS=429` als Umgebung)
   ODER im Dossier ausdruecklich als Zusammenfuehrungskonflikt mit B und C markieren.
7. **(Soll)** HAKEN 1 mit D abstimmen: braucht D eine Ereignis-Umleitung, eine gemeinsame Tabelle nach dem Muster
   `s_power_gates` (scd_vm.c:566-585) statt zwei Nachbarzeilen.
8. **(Soll)** §2 den Sichtbarkeits-Mechanismus nennen (Regions-Cull main.c:8104-8106 gegen das Blindrechteck
   @0x32C/@0x340) statt "Spieler ist nicht im Bild" an einem Standort; einmal die echte Kette (Aufnahme Hebetisch 1150,
   Bit 53 → Weg nach 1050) fahren oder begruenden, warum RE15_GIVE gleichwertig ist.
