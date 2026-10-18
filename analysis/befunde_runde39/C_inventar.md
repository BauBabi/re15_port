# Runde 39 Spur C "inventar" — Dossier (laufend)

Baum: `.claude/worktrees/r39_inventar` (Zweig `r39/inventar`, Basis 3e49ff87). Start 2026-10-18.
Punkte (AUFTRAG.md, woertlich):
1. "In Room 1000 möchte ich im Spind, wo vorher der Feuerlöscher drin war jetzt einmal Handfeuerwaffen Munition drin
   haben. Erst danach den text "Nothing useful", den du jetzt schon hast."
2. "Memory Cards sind noch nicht Stapelbar, das ist falsch, alles mit einer Zahl dran an amounts was gleich ist muss
   stapelbar sein."
3. "Elza hat im Player Inventory noch das Bild und die Karte von Leon oben drin."
4. "Ich habe die gelbe Karte bei Elza noch, obwohl ich gesagt habe "discard"."

Ressourcen (VERTRAG 1.1/1.2): Bank-9-Bit 69 (Munition im Spind ROOM1000), Nachrichten-Id 40 in ROOM1000 nur bei
eigenem Modal-Text.

## Stand
- (laufend)
- 02:34 Dossier angelegt, Pflichtlektuere AUFTRAG/VERTRAG/UEBERGABE, Runde 38 K (Itemorte, Loescher nach 1030),
  Runde 37 I (feuerloescher_r37.c Spind-Umwidmung), Runde 25 (discard, E5), Runde 30/35 (Memory Card, Stapelregel).
- 02:37 Baum gebaut (`local_build.sh configure` + `build`, LOCAL-BUILD-OK).
- 03:xx RE-Belege der vier Punkte gesammelt (Abschnitt 2), Zensus-Werkzeug `re15_port/tools/r39_inventar/zensus_mengen.py`.

## 2. RE-Belege (selbst gelesen, Bytes/Adressen)

### 2.1 Punkt 1 — Spind ROOM1000 / ROOM1001
* `ROOM1000.RDT @0x00C24` (Leon) `50 03 09 31 00 00 14 50 e0 fc 20 03 20 03 31 00 01 00 86 00 ff 00`
  = Item_aot_set Slot 3, sce 9, sat 0x31, Rechteck (20500,-800) 800x800, **Item 0x31 x1** (Feuerloescher), Taken-Bit
  (9,0x86), tk_prop 0xFF (kein Modell).
* `ROOM1001.RDT @0x00C24` (Elza, DERSELBE Raum, DERSELBE Spind) `50 03 09 31 00 00 14 50 e0 fc 20 03 20 03 15 00 1e 00 86 00 ff 00`
  = derselbe Satz mit **Item 0x15 (H. Gun Bullets) x 0x1E = 30**. Das Original legt in Elzas Variante also genau in
  diesen Spind Handgun-Munition — Form (Id, Menge, Satz, Rechteck, kein Modell) fuer Leons Spind = dieser Satz.
* Heutiger Port: `feuerloescher_r37.c spind_umwidmen()` macht Slot 3 zum Raumtext **msg 0** (`ROOM1000 msg 0 @0x0D22`
  = "Nothing unusual." — der einzige Text des Raums, `rdt_msgdump.py`); die Nutzer-Zeile "Nothing useful" ist dieser
  Text (ROOM1000 hat keine andere Nachricht).
* Taken-Bit: (9,0x86) ist seit Runde 37/38 das Bit des Loeschers in ROOM1030 (`RE15_FL37_TAKEN_BIT`) -> fuer die
  Munition das VERTRAG-Bit **(9,69)**.
* Vorrang des kleinsten Slots: Aktions-Scan PSX.EXE `@0x80042f50 beq v0,zero` (sce 0 -> naechster Slot),
  `@0x80042f94 j 0x80043028` (erster Treffer gewinnt) — Runde 38 K 2.1. Damit traegt Slot 3 die Munition und ein
  hoeherer freier Slot mit demselben Rechteck den Text: vor der Aufnahme gewinnt Slot 3, danach (Zone weg) der Text.
* Menge im Spiel: die Port-Halbierung der Munition (NUTZER-ENTSCHEIDUNG 2026-09-20, `item_modal_common.c:157`
  `re15_pickup_menge_nutzer`) gilt fuer jede Welt-Aufnahme -> 30 im Satz, 15 im Inventar (wie jede 30er-Packung).

### 2.2 Punkt 2 — Stapeln (Zensus `tools/r39_inventar/zensus_mengen.py`)
* Zahl im Inventar: RE1.5 `FUN_80048f28`, Gate `sltiu id,0x22` @0x80049124 -> Ids 0x01..0x21 zeigen eine Zahl.
* Eigenschaftstabelle RE1.5 @0x80074DA8 (Schritt 12; +0 Obergrenze `lbu` @0x8004e338, +4 Satz-Zeiger @0x8004e9e8,
  +9 Satz-Anzahl @0x8004e9d8; Satz {Partner, Ergebnis, Aktion, Bild}, Aktion 5 = SELF-STACK @0x8004e664):
  - Munition 0x15..0x20: je ein Selbst-Satz, z.B. 0x15 @0x80074CCC `15 15 05 00`, 0x16 @0x80074CE8 `16 16 05 00`.
  - **0x21 Memory Card @0x80074F34 `fa 00 00 00 88 4c 07 80 01 00 ..`: Obergrenze 250, Satz = Null-Satz @0x80074C88,
    Anzahl 0 -> KEIN Selbst-Satz** (Beta-Luecke).
  - 0x09..0x0B Wurfgranaten: Obergrenze 250, Null-Satz (Zahl = Anzahl Granaten, Wurf zieht 1 ab `jal 0x8004eae4`
    @0x80033b40). Feuerwaffen 0x03..0x08, 0x0C..0x14: Zahl = Magazin EINER Waffe (Nachladen `FUN_8004ebdc`).
* RE2 (Retail-Ziel, info/re2leon/PSX.EXE, Tabelle @0x800a9e1c Schritt 8 {max, -, -, Anzahl, Satz}, Satz
  {Partner, Aktion, Ergebnis, Bild}): **Ink Ribbon 0x1E @0x800A9F0C `fa 00 00 01 b8 9d 0a 80` -> Satz @0x800A9DB8
  `1e 05 1e ff` = Selbst-Stapeln (Aktion 5)**; Munition 0x14..0x1D ebenso (`14 05 14 ff` @0x800A9D4C ...).
  RE2-Kisten-Gate @0x80070490 `addiu v0,a1,-20` / @0x80070494 `sltiu v0,v0,0xc` = Ids 0x14..0x1F (Munition + Ink
  Ribbon) verschmelzen in der Kiste. Die Memory Card ist im Port das RE2-Ink-Ribbon (Runde 33, re15_savepoint.h).
* Ausgaben (Zensus aller Item_aot_set): Memory Card im Original 0x; im Port ROOM1010/1011 x3 (inhalt_r35.c) und
  ROOM1150/1151 x3 (irons_tisch_1150.c) -> zwei Packungen, heute zwei Plaetze. Hand Grenade nur Port ROOM1150 x1.
  Jede Feuerwaffe hoechstens EINE Ausgabe je Szenario, keine gleich der Startwaffe 0x03 -> "gleich mit Zahl" tritt
  bei Feuerwaffen nicht auf.
* Port heute: `re15_pickup_stapelt_nutzer` (inventory_common.c:349) = Fenster 0x15..0x21 GESCHNITTEN mit "eigener
  Satz" -> 0x21 faellt heraus (Aufnahme legt zweiten Platz an); Menue-Matcher (menu_common.c `exchange_match`,
  FUN_8004e900) findet fuer 0x21 keinen Satz -> kein Kombinieren; Kiste (`re15_itembox.c box_is_ammo` = 0x15..0x21)
  verschmilzt 0x21 schon.

### 2.3 Punkt 3 — Bild und Karte oben im Statusschirm
* Lader DEBUG.BIN `FUN_800c01c4`: `800c01d0 9063ca5c lbu v1,-13732(v1)` (DAT_800aca5c = Charakter/PLD-Slot),
  `800c01d8 24423ad8 addiu v0,v0,15064` (0x80073ad8), `800c01dc sll v1,v1,1`, `800c01e4 94440000 lhu a0,0(v0)`,
  `800c01f0 0c004ed8 jal 0x80013b60` (CD-Lader nach Datei-Id). Aufrufer `@0x800316d0 jal 0x800c01c4` in
  FUN_800314b0 (Spieler-PLD-Lader).
* Tabelle @0x80073ad8 = u16 [0x2c..0x3b] (`re15_disasm.py read`); Datei-Tabelle @0x8006f43c: Ids 0x2c..0x3b je 12492 B,
  LBA 6384, 6391, ... (aufsteigend in Schritten von 7 Sektoren = Reihenfolge STPIC_00..0F in info/Re1.5/re15.cat).
* -> Leon (0) STPIC_00 = Polizeiausweis "POLICE Leon S. Kennedy" mit Foto; **Elza (4) STPIC_04 = "RACCOON CITY
  DRIVER LICENSE Elza Walker" mit Foto**. md5: 00=01=02, 04=05=06 (Varianten je Figur gleich).
* Port heute: `platform/pc/src/inv_render_pc.c:288` laedt IMMER `ITEM/STPIC_00.TIM` (Kommentar "blocker Q2").

### 2.4 Punkt 4 — gelbe Karte (Yellow Keycard 0x39) bei Elza
* Benutzung: nur ROOM11E0/11E1 (Kartenleser B2-Korridor; msg 7/8/9, Erfolg msg 5 "You've opened the lock.").
  Aufnahme: ROOM1011 (Taken-Bit (9,0x8A), kistenregel_r37.c K4).
* `gen/discard_sites.inc` (Generator `tools/gen_discard_sites.py`, Runde 25): ROOM11E0 UND ROOM11E1 "VERWORFEN E5:
  flag(3,139) wird auch ausserhalb gesetzt (ROOM1240 @0x0055A)" -> fuer die Yellow Keycard gibt es KEINE
  Wegwerf-Stelle, also auch keine Abfrage.
* ROOM1240 sub02 @0x0055A `22 03 8b 01` = Set(3,0x8B) — das ist LEONS Startraum (ROOM1240.RDT). ROOM1241 (Elza)
  @0x0055A traegt `22 03 6f 01` (Set(3,0x6F)), Set(3,0x8B) steht in keiner xxx1-Datei ausser ROOM11E1 @0x01A8A.
  Leon-/Elza-RDT sind getrennte Dateien (Raumlader `addu a0,a0,v0` @0x800397ec, Elza = Leon-Datei + 1,
  re15_gameflow.h) -> der Leon-Setzer kann in Elzas Spiel nie laufen. Der E5-Riegel klappte die Variante weg
  (`basisraum(r) = r[:-1] + "x"`) und strich damit auch Elzas Panel.

## 1. Messung vorher (Stand 3e49ff87 + nur die Messschiene RE15_INV_LOG, exe-Messkopie `re15_pc_r39c_mess.exe`)

Lade-Weg: Spielstand mit dem Kartenwerkzeug `probe_r37_feuerloescher_karte` + CONTINUE
(`RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0`), Eingabe `RE15_PRESS`/`RE15_PAD_AT`/`RE15_INPUT_SCRIPT`,
Logs `RE15_MSG_LOG=1` + neue Messschiene `RE15_INV_LOG=1` (`[inv-r39]` Inventar/Kiste bei jeder Aenderung,
`[inv-r39-menu]` Menue-/Nachrichten-/Wegwerf-Zustand mit Bildnummer). Lauf-Skript `C_belege/lauf.sh`.

* **P1 Spind ROOM1000** (Leon, Karte `1000 20900 -1100 3072` = vor dem Spind, Pruefpunkt 620 voraus (20900,-480) im
  Rechteck x 20500..21300 z -800..0; Stehflaeche `probe_r34n_e_dokumente stand 1000`: z <= -900):
  `[feuerloescher] ROOM1000: Spind Slot 3 (Item 0x31 @0x00C24) -> Raumtext msg 0`, je Druck
  `[msgtext] room=1000 id=0 "Nothing unusual."`, Inventar unveraendert (03:15,15:50). -> heute nur der Text.
* **P2 Memory Card aufnehmen** (Leon ROOM1010, Karte `1010 2800 -1300 0 give:0x21:3`, Regal-Zone Slot 10):
  `F0 inv=03:15,15:50,21:3,...` -> nach Ja `F198 inv=03:15,15:50,21:3,21:3,...` = **zweiter Platz** (reproduziert).
* **P2 Kombinieren** (Karte `leer give:0x21:3 give:0x21:2`, START F120, ITEM F140, Platz 0 F150 (st 4 F158),
  COMBINE F159 (cmd 3), F161 st 7, zweiter Cursor F163 -> 1, Bestaetigen F165): `F165 st=6` = Matcher ohne Paar,
  Inventar bleibt `21:3,21:2` (Bild `C_belege/p2_kombinieren_vorher.png`).
* **P2 Kiste** (Karte `leer give:0x21:3 kiste:0x21:2`, `RE15_BOX_SHOT=1`): `F34 inv=21:5 box=-` — die ITEM BOX
  legt Memory Cards SCHON zusammen (Gate re15_itembox.c `box_is_ammo` = 0x15..0x21).
* **P3 Elza Statusschirm** (Karte `1001 20900 -1100 3072 elza`, START F120, Bild F145): oben links Leons
  Polizeiausweis "POLICE Leon S. Kennedy" mit Leons Foto (`C_belege/p3_elza_status_vorher.png`). (ROOM1011 ging
  nicht: dort laeuft beim Betreten Roys Szene.)
* **P4 gelbe Karte Elza ROOM11E1** (Karte `11E1 10400 4250 0 elza give:0x39:1 flag:9:0x8A`, Kartenleser Slot 4
  (10650,3850) 800x800): F101 msg 7 (Ja) -> F403 msg 9 "You've used the Yellow Keycard." -> F492 msg 0 "Enter the
  first number." (Tastenfeld) -> Code-Bits (5,13..16) per `RE15_SET_FLAG_AT=...@560` (die vier Ziffer-Riegel, sub01
  @0x01A76..@0x01A82) -> F561 msg 5 "You've opened the lock." -> F701 zu. **`wegwerf=0` in jedem Bild, Inventar
  behaelt `39:1`** (reproduziert: keine Abfrage, Karte bleibt). Ohne Code-Riegel (vier falsche Ziffern): msg 4
  "Wrong code, try again." und zurueck zu msg 7 — der Lesepfad ist intakt.

## 3. Umsetzung (Fortsetzung 07:08–, nach dem Sitzungslimit des Vorgaengers)

Die vom Orchestrator gesicherten Reste (dfde7745) wurden Zeile fuer Zeile gegen Abschnitt 2 geprueft und uebernommen:
* `inventory_common.c` (2 Zeilen): `re15_pickup_passt_nutzer` / `re15_inv_grant_stapeln_nutzer` fragen
  `re15_inventar_r39_stapelt` statt `re15_pickup_stapelt_nutzer` (das alte Fenster "0x15..0x21 GESCHNITTEN mit eigenem
  Satz" liess 0x21 heraus). `re15_pickup_stapelt_nutzer` selbst bleibt (byte-true Helfer, Riegel probe_r26 C7b).
* `menu_common.c exchange_match` (5 Zeilen): nach dem Werfer-Paar (Runde 35) das Port-Paar `re15_inventar_r39_paar`
  (gleiche Id, stapelbar, OHNE eigenen Selbst-Satz -> Aktion 5, Ergebnis = Id, Bild 0). Der Executor-Fall 5
  (@0x8004e664 -> x_reload @0x8004e324/@0x8004e430) ist unveraendert; er liest s_x_pic nicht.
* `re15_itembox.c box_is_ammo` (1 Zeile): dasselbe Praedikat (Fenster 0x15..0x21 unveraendert, + Granaten 0x09..0x0B).
* `scd_room_setup.c` + `platform/pc/main.c` (je 1 Installer-Zeile NACH dining_r38, Boot-/CONTINUE-Weg wie Runde 38).

Neu in dieser Sitzung:
* **(3) `platform/pc/src/inv_render_pc.c`**: `stpic_laden()` laedt `ITEM/STPIC_<k>.TIM` mit k =
  `re15_inventar_r39_stpic_index()` (work_vars[0x10] = untere Nibble von DAT_800aca5c nach dem Raumlader-Merge
  @0x80039784; Rueckfall g_gameflow.character) statt fest STPIC_00; `stpic_refresh()` in `re15_inv_render_pc_draw`
  laedt den Block neu, wenn sich die Figur im selben Prozess aendert (Titel -> neues Spiel). Log-Zeile
  `[inv] Status-Bild ITEM/STPIC_04.TIM (Figur 4, Runde 39 Spur C)`.
* **(4) `tools/gen_discard_sites.py`**: E5 je SZENARIO-VARIANTE (`variante(r) = r[-1]`): ein Setzer der anderen Variante
  kann nie laufen (Raumlader @0x800397ec `addu a0,a0,v0`, Elza = Leon-Datei + 1; alle 120 Raeume liegen paarweise vor,
  selbst gezaehlt). Neu erzeugt `engine/src/gen/discard_sites.inc`: **ROOM11E1 msg 9 -> msg 5, Gate flag(3,139)==1
  (`Set` @0x01A8A), Ausloeser sub19 @0x01F86** = 15. Stelle; ROOM11E0 bleibt gestrichen (ROOM1240 @0x0055A `22 03 8b
  01` = Leons Startraum). `tests/unit/r21_discard_wegwerfen.c` TEIL R: Sollwerte 5 Stellen mit Gate, 5 lebende + 1
  gestrichenes Panel (vorher 4/4+2), mit Kommentar.
* **(1) Bit 69**: `tests/unit/test_r36_kartenbits.c k_port[]` + `RE15_R39C_MUNITION_BIT` (VERTRAG 1.1).
* Messschiene `[inv-r39-menu]` um `fsm=<Dialog-Zustand> sel=<Ja/Nein sichtbar> bereit=<Abfrage getippt>` erweitert
  (env-gegatet, kein Spielverhalten).
* Sonde `probe_r34n_e_dokumente aots 1000` (gebauter Stand): Slot 3 `ITEM item=0x15 x30 bit=69 prop=255
  x[20500..21300] z[-800..0]`, Slot 11 `MESSAGE x[20500..21300] z[-800..0]`, erster freier Slot 12 — die PORT-WAHL
  Slot 11 ist belegt-frei wie im Kopf beschrieben.

Konstanten (alle in `include/re15_inventar_r39.h` mit Adresse/Offset): Spind-Satz ROOM1001 @0x00C24..@0x00C39, Bit 69
(VERTRAG), msg 0 @0x0D22, Slot 11 (PORT-WAHL), Stapel-Obergrenze 250 (`fa` @0x80074DA8+id*12, Zeilen 0x09/0x0A/0x0B/0x21),
Aktion 5 (@0x8004e664; RE2 Ink Ribbon `1e 05 1e ff` @0x800A9DB8), STPIC-Basis 0x2c (@0x80073ad8).

## 4. Messung nachher (gebauter Stand 0eca7720+, Messkopie `re15_pc_r39c_mess.exe`, Laeufe `C_belege/lauf.sh`;
Bestaetigen = VIERECK (byte-true virtuell 0x4000 @0x80028570; Kreuz = Abbruch/0x8000))

* **P1 Spind** `lauf.sh m1_nachher "1000 20900 -1100 3072" 560 … RE15_INPUT_SCRIPT=W1,A0.1,W3.5,A0.1,W4,A0.1,W4,A0.1,W4`:
  `[inventar-r39] ROOM1000 Spind: Slot 3 Aufnahme Item 0x15 x30 Bit (9,69) (Form ROOM1001 @0x00C24), Raumtext msg 0 ->
  Slot 11`; Druck 1 (F~90) Modal, Druck 2 (F~198) Ja -> `[inv-r39] F198 … inv=03:15,15:65` (15:50 + 15: 30 im Satz,
  Nutzer-Halbierung), Druck 3 -> `[msgtext] room=1000 id=0 "Nothing unusual."` (genau 1x; Druck 4 schliesst).
  Wiederbetreten mit (9,69)=1 (`m1b_nachher`, Karte `flag:9:69`): `Munition schon genommen (9,69)=1 -> Slot 3 Raumtext
  msg 0`, Inventar unveraendert, erster Druck -> msg 0. Taken-Bit nach der Aufnahme: Riegel unit_r39_inventar_spind B.
* **P2 Aufnahme** `m2_nachher` (1010, give:0x21:3): `F0 inv=…,21:3` -> `F198 inv=03:15,15:50,21:6,00:0` = EIN Platz
  (vorher zweiter Platz 21:3,21:3).
* **P2 Kombinieren** `m2b_nachher` (leer give:0x21:3 give:0x21:2, RE15_PRESS=start@120,square@139,square@149,right@158,
  square@160,right@162,square@164): F139 sub 3, F149 st 3, F158 cmd 3 (COMBINE = rechts, Cluster @0x800c64c0), F160 st 7,
  F162 cur2 1, `[inventar-r39] Kombinieren: Item 0x21 Platz 0 + 1 -> Aktion 5 …` -> `[inv-r39] F164 … inv=21:5,00:0`
  (vorher F165 st=6 ohne Paar).
* **P2 Kiste**: unveraendert (verschmolz schon, 1. Messung); Riegel unit_r39_inventar_stapeln K.
* **P3 Elza Statusschirm** `m3_nachher` (1001 elza, start@120, Framedump 145): `[inv] Status-Bild ITEM/STPIC_04.TIM
  (Figur 4, Runde 39 Spur C)`, Bild `C_belege/p3_elza_status_nachher.png` = "RACCOON CITY DRIVER LICENSE Elza Walker"
  mit Elzas Foto oben links (vorher Leons Polizeiausweis, `p3_elza_status_vorher.png`).
* **P4 gelbe Karte** `m4_nachher` (11E1 elza give:0x39:1 flag:9:0x8A; Leser-Prompt msg 7 F101, Ja per Viereck, Code-Bits
  per RE15_SET_FLAG_AT): nach dem Ja `[msgtext] room=11e1 id=5 "You've opened the lock."` und
  `[inv-r39-menu] F822 … msg=1/5 wegwerf=1` = die Abfrage ist an msg 5 ARMIERT (vorher `wegwerf=0` in jedem Bild).
  Ja-Antwort + Karte weg: Lauf mit erweiterter Schiene (fsm/sel/bereit) s.u. / Riegel unit_r39_inventar_discard W.
