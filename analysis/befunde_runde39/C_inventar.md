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
