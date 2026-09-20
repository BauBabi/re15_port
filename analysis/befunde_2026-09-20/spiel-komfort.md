# Runde 17 — Spiel-Komfort: drei Nutzer-Entscheidungen

Stand: 2026-09-20 · Zweig `wf_2d0f494e-255-2` auf v0.8.6 · Suite **324/324 gruen**

---

## 0 Kurzfassung

Drei Wuensche aus dem Spieltest, die ausdruecklich **KEINE byte-true Fragen** sind. Das
Original macht es an allen drei Stellen anders; geaendert wird, weil der Nutzer es so will.
Belegt (mit `@0x`) ist jeweils nur der **Mechanismus und die Nummer**, die uebernommen werden —
nicht die Entscheidung selbst. Jede Zahl im Bericht ist gemessen.

| # | Wunsch | Umsetzung | Messung |
|---|--------|-----------|---------|
| 1 | „die Munition auf maximal die haelfte begrenzt die man findet" | Munition (Id 0x15..0x21) wird beim **Aufsammeln** halbiert, abgerundet, Mindestmenge 1 | Zensus 240 RDTs: **1108 → 532** Stueck; STAGE1 **950 → 458** |
| 2 | Klick-Laut beim Cursor-Raetsel | Bewegungs-Klick am Notch-Wechsel (CORE-Bank 4, Satz 4) + Bestaetigungs-Laut (Satz 6), flankengegatet | Zensus 12 Raster-Raeume: **0 Fehl-Klicks** ohne Eingabe; Bestaetigung rattert nicht (kleinster Abstand 17 Bilder statt 1) |
| 3 | Untertitel so lange wie die Sprachausgabe | **NACHHALL** hinter dem regulaeren Zeilen-Ende, ohne den Spieler laenger zu sperren | ROOM1150, 11 Zeilen: vorher **6 zu kurz**, nachher **0**; Sperr-Bilder **unveraendert** |

Was **nicht** geaendert wurde und warum, steht in §5.

---

## 1 Reproduktion / Messung

### 1.1 Munition — wieviel liegt ueberhaupt herum?

Erhoben mit `re15_port/tools/aot_sce_census.py` (opcode-exakter Walk ueber mainScd+subScd aller
240 ausgelieferten RDTs, Walker-Abdeckung 100,00 %, 0 Desyncs). Munitions-Records sind die
`Item_aot_set` (Opcode 0x50) mit Item-Id im Fenster 0x15..0x21.

```
  ALLE Stages                  Records= 75  vorher= 1108  nachher=  532
  STAGE1 (beide Szenarien)     Records= 54  vorher=  950  nachher=  458
  STAGE1 Leon (ROOMxxx0)       Records= 27  vorher=  475  nachher=  229
  STAGE1 Elza (ROOMxxx1)       Records= 27  vorher=  475  nachher=  229
  STAGE2                       Records=  6  vorher=   56  nachher=   26
  STAGE3                       Records=  4  vorher=   26  nachher=   12
  STAGE4                       Records=  6  vorher=   38  nachher=   18
  STAGE5                       Records=  5  vorher=   38  nachher=   18
  STAGE6                       Records=  0  vorher=    0  nachher=    0

  id 0x15 H.GUN BULLETS    Records= 38  vorher=  810  nachher=  394
  id 0x16 SHOTGUN SHELLS   Records= 28  vorher=  238  nachher=  108
  id 0x17 MAGNUM BULLETS   Records=  9  vorher=   60  nachher=   30

  Packungsgroessen im Auslieferungsstand: 6, 7, 12, 14, 15, 30
```

Die Summe faellt nicht exakt auf die Haelfte (1108/2 = 554 gegen 532), weil ungerade Packungen
abgerundet werden: 7 → 3, 15 → 7. Das ist beabsichtigt — „maximal die Haelfte".

Ein Durchgang spielt nur EIN Szenario, in STAGE1 also 27 Records / **475 → 229** Stueck.

### 1.2 Cursor-Raetsel — welche Raeume sind es, und wandert dort etwas von selbst?

Die Cursor-Raetsel sind im Port kein Listen-Menue, sondern ein **Welt-Cursor**: ein
`Obj_model_set`-Prop, das das Skript per `Speed_set`/`Add_speed` unter dem D-Pad bewegt, plus
ein Raster aus AOT-Zellen, ueber denen das Prop seinen „Index" (`member_0b`) bekommt.

Zensus ueber dieselben 240 RDTs (Objekt-Pool-Bit 0x04 gesetzt, ACTION-Bit 0x10 nicht,
CENTRE/FORWARD gesetzt) ergibt 45 Raeume mit solchen Zellen; davon sind 12 echte Raster
(≥2 Zellen, ohne die Elza-Zwillinge `ROOMxxx1`):

```
  ROOM1080 (4)  ROOM10D0 (11)  ROOM1100 (2)  ROOM11E0 (12)  ROOM11F0 (11)  ROOM1230 (11)
  ROOM2050 (3)  ROOM2060 (7)   ROOM20A0 (5)  ROOM3050 (2)   ROOM30E0 (2)   ROOM4020 (5)
```

Gemessen mit `probe_r17_cursor_klick` (echter Raum-Boot + echter `re15_game_step`, 600 Bilder
**ohne** Eingabe je Raum):

```
  ROOM1080  Zellen= 4  Props= 1  Fehl-Klicks ohne Eingabe: 0
  ROOM10D0  Zellen=11  Props= 2  Fehl-Klicks ohne Eingabe: 0
  ROOM1100  Zellen= 2  Props= 0  Fehl-Klicks ohne Eingabe: 0
  ROOM11E0  Zellen=12  Props= 2  Fehl-Klicks ohne Eingabe: 0
  ROOM11F0  Zellen=11  Props=12  Fehl-Klicks ohne Eingabe: 0
  ROOM1230  Zellen=11  Props= 2  Fehl-Klicks ohne Eingabe: 0
  ROOM2050  Zellen= 3  Props= 8  Fehl-Klicks ohne Eingabe: 0
  ROOM2060  Zellen= 7  Props= 9  Fehl-Klicks ohne Eingabe: 0
  ROOM20A0  Zellen= 5  Props= 8  Fehl-Klicks ohne Eingabe: 0
  ROOM3050  Zellen= 2  Props= 3  Fehl-Klicks ohne Eingabe: 0
  ROOM30E0  Zellen= 2  Props= 3  Fehl-Klicks ohne Eingabe: 0
  ROOM4020  Zellen= 5  Props= 1  Fehl-Klicks ohne Eingabe: 0
  SUMME Fehl-Klicks ohne Eingabe: 0
```

Das war die offene Frage: kein Prop wandert von selbst ueber ein Raster, der Laut kann also
ausserhalb einer echten Cursor-Bedienung nicht anspringen.

### 1.3 Untertitel — wie gross ist der Fehlbetrag?

Gemessen mit `probe_r17_untertitel`: echte `.msg`-Bytes aus `ROOM1150.RDT`, echte FSM
(`re15_msg_tick` → `re15_dialog_step`), echte Tonlaenge aus dem WAV-Kopf unter `synchro/`.
„vorher" ist derselbe Lauf mit `g_re15_voice_laeuft = 0` — dann ist der neue Zweig
unerreichbar und der Code verhaelt sich exakt wie vor der Aenderung.

```
  Zeile       Ton     vorher    nachher   Nachhall    Block
  2           124        100        124         24      100
  4           127        120        127          7      120
  5           102        120        120          0      120
  6            99        120        120          0      120
  7           170        120        170         50      120
  8            57        100        100          0      100
  9           243        140        243        103      140
  10          101        120        120          0      120
  12          188        120        188         68      120
  13          176        120        176         56      120
  14           27        100        100          0      100
  gemessen: 11 Zeilen; zu kurz VORHER 6, zu kurz NACHHER 0
```

Spalte „Block" = Bilder, in denen `message_display_frames > 0` oder `message_query != 0` gilt —
das sind die beiden Werte, an denen `msg_block` haengt (player_common.c:756, aot_common.c:845).
Sie ist **vorher wie nachher identisch**: der Spieler wird keine Sekunde laenger gesperrt.

Gegenprobe: ohne laufende Aufnahme ist die Anzeigedauer bitgleich wie vorher (Zeile 2: 100
Bilder, Nachhall 0).

---

## 2 Original-Mechanismus (@0x)

### 2.1 Das Id-Fenster der Munition (byte-true, uebernommen)

Der Port klassifiziert Gegenstaende ueber ein Id-FENSTER, nicht ueber ein Typ-Byte —
byte-true belegt:

```
  @0x80047d54  sltiu  id,0x15      ; WAFFE  = 0x00..0x14
  @0x80049124  sltiu  id,0x22      ; MUNITION endet vor 0x22
```
gestuetzt auf die ARMS-Kopftabelle `@0x8007492c`, die genau fuer 0x00..0x14 ungleich null ist.
`re15_item_is_ammo` (inventory_common.c) ist damit die Schranke, auf die sich die Halbierung
stuetzt. **Gehalbiert wird auf Nutzer-Wunsch — das tut das Original nicht.**

Die Aufnahme-Kette selbst (unveraendert):
```
  @0x80043328  Handler[9]         ; Item-AOT feuert -> Aufnahme-FSM armen
  @0x8004334c  sb 1,DAT_80072d3b  ; Zustand 1
  @0x8004335c  DAT_800afbb6 = Typ
  FUN_8001db28                    ; die 9-Zustands-Praesentation; Grant erst in Zustand 7
```
und die Nachlade-Kette, die **nicht** angefasst wird:
```
  FUN_8004ebdc @0x8004ebdc        ; chunk = props[wid].chunk; mag += chunk; box -= chunk
  @0x80074da8                     ; Waffen-Eigenschaftstabelle (chunk = Magazingroesse)
```

### 2.2 RE2s Cursor-Laute (byte-true, uebernommen)

RE2 `PSX.EXE`, `FUN_8006b358` — der Raster-Cursor (2D-Gitter, D-Pad ueber `DAT_800ce304`):

```
  8006b574: lbu  v0,13(s1)              ; NEUER Cursor-Index
  8006b578: nop
  8006b57c: beq  a2,v0,0x8006b58c       ; ALTER == NEUER -> KEIN Laut
  8006b580: lui  a0,0x404               ; Se_on(0x04040000)  = Bewegung
  8006b584: jal  0x8005ba28
  8006b588: addu a1,zero,zero
  ...
  8006b5a0: jal  0x800695b0             ; Auswahl gueltig?
  8006b5ac: andi v0,v0,0xff
  8006b5b0: beq  v0,zero,0x8006b5bc
  8006b5b4: lui  a0,0x407               ; ungueltig -> Se_on(0x04070000)
  8006b5b8: lui  a0,0x406               ; gueltig   -> Se_on(0x04060000)
  8006b5bc: jal  0x8005ba28
  ...
  8006b5d0: lui  a0,0x405               ; Abbruch   -> Se_on(0x04050000)
  8006b5d4: jal  0x8005ba28
```

Kernaussage: **der Laut haengt an der AENDERUNG des Index, nicht am Tastendruck.**

Dieselbe Bank/Nummer benutzt RE1.5 in seinem EIGENEN Inventar-Cursor — die Nummer ist also
beidseitig belegt und nicht erfunden:
```
  8004a478: lui  a0,0x404
  8004a47c: jal  0x80045024             ; Se_on(0x04040000) = Cursor
  8004a51c-20                           ; 0x0406 = Bestaetigen
  8004a660-64                           ; 0x0405 = Abbrechen
```

### 2.3 Wo das Raetsel im Port bestaetigt (selbst disassembliert)

`ROOM11F0.RDT` sub01, opcode-exakter Walk der Roh-Bytes (Datei-Offsets):

```
  @0x10F0  21 05 01 01        Ck(5,1,1)             ; Schalter 1 noch offen?
  @0x10F4  2e 03 00           Work_set(3,0)         ; Arbeits-Entitaet = CURSOR-Objekt
  @0x10FC  3e 00 0f 00 02 00  Member_cmp(15 == 2)   ; steht er ueber Zelle 2?
  @0x1106  51 01 40 00        Sce_key_ck(1,0x0040)  ; und liegt BESTAETIGEN an?
  @0x110A  04 ff 18 06        Evt_exec(sub06)       ; dann Schalter umlegen
```
Elfmal derselbe Block fuer die Zellen 2..0x0C (@0x10FC, 0x1124, 0x114C, 0x1174, 0x119C,
0x11C4, 0x11EC, 0x1214, 0x123C, 0x1264, 0x128C). Die Bewegung haengt an
`Sce_key_ck(1,0x01/0x04/0x02/0x08)` @0x1098/0x10B0/0x10C8/0x10E0 → sub02..05
(`Speed_set(2,±200)` / `Speed_set(0,±200)` + `Add_speed`, @0x12F6/0x1302/0x130E/0x131A).

Maske `0x0040` = virtuelles Bit 6 = SQUARE (`re15_pad_virtual_word` vtbl[6], Preset-Tabelle
`@0x80073dbc`).

Notch-Stempel und -Clear (byte-true, bereits im Port):
```
  @0x80042f5c  sb  slot,0xb(entity)   ; Stempel = Zelle, ueber der die Entitaet steht
  @0x80043788  sb  zero,0x0(at)       ; UNBEDINGTER Clear je Objekt vor dem Scan
```
Darum ist 0 im Port „ueber KEINER Zelle" und nicht „Zelle 0" — die Zellen liegen auf den
AOT-Slots 1..17.

### 2.4 Text-Maschine und Sprachausgabe

Die Text-Maschine `FUN_80028134` bleibt unangetastet; der Nachhall haengt sich **hinter** ihre
drei Dismiss-Stellen (@0x80028594/@0x800286bc/@0x80028708, im Port `message_fsm == 6`).
Dass RE1.5 gar keine englische Sprachausgabe hat, ist der Grund, warum es hier kein
Original-Vorbild geben KANN: `synchro/` ist eigene Produktion des Projekts.

---

## 3 Port-Ist (Datei:Zeile, vor der Runde)

| Stelle | Ist |
|---|---|
| `engine/src/scd_vm.c:3697` `op_item_aot_set` | liest Typ/Menge aus dem RDT-Record und reicht sie unveraendert an den AOT weiter |
| `engine/src/aot_common.c:648` / `:1303` | die **einzigen** zwei Produktiv-Aufrufer von `re15_item_modal_start` |
| `engine/src/item_modal_common.c:139` | `s_amount = amount` — ungefiltert; Zustand 7 grantet daraus |
| `engine/src/aot_common.c:412` `re15_object_notch_update` | stempelte `member_0b` **stumm** |
| `engine/src/scd_vm.c` `op_sce_key_ck` | reines Praedikat, ohne Laut |
| `engine/src/msg_common.c` `re15_dialog_step` | `message_fsm == 6` → sofortiger Abbau; die Aufnahme lief weiter, der Text war weg |
| `engine/src/msg_common.c` `re15_msg_tick` (Legacy-Zweig) | `message_display_frames--`, bei 0 aus |

---

## 4 Umsetzung

### 4.1 Munition halbieren

* `include/re15_inventory.h` — neue Erklaerung + `re15_pickup_menge_nutzer(item_id, menge)`.
* `engine/src/inventory_common.c` — die Funktion: **nur** `re15_item_is_ammo`, Abrundung,
  Mindestmenge 1 (eine 1er-Packung darf nicht auf 0 fallen, weil `re15_inv_grant` `amount==0`
  zurueckweist und der Gegenstand dann spurlos verschwaende).
* `engine/src/item_modal_common.c` — ein Aufruf beim Modal-Start. Das ist die einzige Stelle,
  weil beide Aufnahme-Wege dort zusammenlaufen. Nebeneffekt, der so gewollt ist: die Abfrage
  („WILL YOU TAKE THE …") zeigt schon die halbierte Menge, verspricht also nichts anderes als
  sie eintraegt.

Nicht betroffen, gemessen und gepinnt: Nachladen aus der Reserve (volle 15 pro Nachladen,
Reserve faellt 50 → 35), Startausruestung (Magazin 15, Reserve 50), Kraeuter, Schluessel,
Waffen mit geladenem Magazin.

### 4.2 Klick-Laut

* `engine/src/aot_common.c` `re15_object_notch_update` — Bewegungs-Klick: `neu != alt` **und**
  `neu != 0`, plus eine Einschwing-Sperre fuer den ersten Stempel nach einem Raumwechsel
  (der geht von 0 auf die Startzelle und ist keine Cursor-Bewegung). → `re15_audio_core_se(4)`.
* `engine/src/scd_vm.c` `op_sce_key_ck` — Bestaetigungs-Laut: Praedikat wahr **und**
  Arbeits-Entitaet ist ein OBJEKT **und** dieses Objekt steht ueber einer Zelle
  (`member_0b != 0`). → `re15_audio_core_se(6)`, ueber einen Flankenspeicher, weil
  `Sce_key_ck` den GEHALTENEN Pad-Zustand liest und sonst je Bild feuern wuerde.

Die RE2-Variante 0x0407 („ungueltig bestaetigt") wurde **nicht** uebernommen: RE1.5 prueft die
Zelle gar nicht erst zu Ende, es gibt dort kein „falsch bestaetigt". Eine Nummer dafuer waere
erfunden gewesen.

Messung des Flankenspeichers am echten Raetsel (60 Bilder gehaltenes Quadrat):
**3 Laute, kleinster Abstand 17 Bilder** — nicht 60. Die 3 sind richtig: das Skript legt dabei
wirklich mehrfach einen Schalter um (7 Zone-5-Zustandswechsel), jeder Laut gehoert zu einer
echten Bestaetigung.

### 4.3 Untertitel-Nachhall

* `include/re15_scd.h` — neues Feld `message_nachhall` (Restguthaben in Bildern).
* `engine/src/msg_common.c` — Zustand **7 = NACHHALL**. Beim Eintritt werden
  `re15_pauseflags_close()` gefahren und `message_display_frames`/`message_query` genullt; der
  Spieler ist ab diesem Bild frei, genau wie heute beim Dismiss. Stehen bleibt nur der Text.
  Ende, sobald die Aufnahme durch ist, der Spieler drueckt, oder der Deckel leer ist.
* Derselbe Nachhall im Legacy-Full-Text-Zweig (Kino-Captions ROOM1170/ROOM1240).
* Deckel = **300 Bilder**, derselbe wie der bestehende Riegel in `op_message_on`
  (`RE15_VOICE_NOTBREMSE`): 10 s, ueber der laengsten Aufnahme des Bestands (ROOM1150 main09,
  242,5 Bilder). Er ist Notbremse fuer einen haengenden Kanal — gewartet wird gegen die ECHTE
  Restlaenge `g_re15_voice_restbilder`, nicht gegen eine geschaetzte Dauer.

### 4.4 Tests

`re15_port/tests/unit/probes/r17_spiel-komfort.cmake` (GLOB-registriert):

| ctest | prueft |
|---|---|
| `unit_r17_munition_pin` | Id-Fenster, Abrundung, Mindestmenge; echter Aufnahme-Pfad 30→15 / 7→3; Kraut bleibt; Startausruestung bleibt; Nachladen gibt VOLLE 15 |
| `unit_r17_cursor_klick_pin` | Zensus 12 Raster-Raeume = 0 Fehl-Klicks; Skript-Anker auf die zitierten ROOM11F0-Bytes; Cursor-Fahren klickt; Bestaetigung nie in zwei Bildern hintereinander |
| `unit_r17_untertitel_pin` | 11 Zeilen gegen echte Tonlaengen, vorher/nachher; Sperr-Bilder duerfen nicht steigen; ohne Aufnahme keine Aenderung |

Angepasst: `tests/unit/test_item_stack_press.c` erwartete +15 pro Aufnahme und erwartet jetzt
+7. Der Test prueft die DRUCK-ORDNUNG (ein Druck = ein Gegenstand), die Zahl ist dort nur
Beleg, dass genau EINE Aufnahme passiert ist; der Grund der Aenderung steht im Test.

### 4.5 Sichtpruefung

`analysis/befunde_2026-09-20/spiel-komfort/` — mit dem Lese-Werkzeug selbst angesehen:

* `f359.png` — ROOM1170 Pre-Intro, Caption „To escape." steht.
* `f479.png` — Helipad, Elliot-Zeile mit Sprecher-Namen, Szene vollstaendig.
* `raum11f0.png` — Generator-Raum laeuft normal.

---

## 5 Offen / bewusst nicht gemacht

1. **Waffen mit geladenem Magazin bleiben voll.** 13 Records tragen 495 Stueck Munition
   *in* einer Waffe (INGRAM M10 x100, H&K MC51 x100, SPAS-12 x12 …). Das ist keine
   „Munitions-Packung, die man findet", sondern die Waffe selbst; `re15_item_is_ammo` fasst
   sie nicht. Wenn der Nutzer auch das halbiert haben will, ist es eine Zeile — aber es ist
   seine Entscheidung, nicht meine.
2. **Der Bewegungs-Klick ist nicht live im Spiel gehoert worden.** Die Sitzung hat kein
   Audio-Geraet: `RE15_STIMME_LOG` meldet im echten Lauf durchgehend `laeuft=0 rest=0`. Der
   Klick ist ueber den Zaehler im echten `re15_game_step`-Pfad gemessen, nicht ueber das Ohr.
   Wer Ton hat, sollte ROOM11F0 einmal gegenhoeren.
3. **Aus demselben Grund ist der Nachhall nicht live im Spiel beobachtet.** Gemessen ist er
   end-to-end gegen echte `.msg`-Bytes und echte WAV-Laengen — aber mit gestempelter statt
   abgespielter Restlaenge.
4. **Die RE2-Nummer 0x0405 (Abbrechen) haengt nirgends.** Die RE1.5-Cursor-Raetsel haben
   keinen eigenen Abbruch-Knopf im selben Block; ROOM11F0 bricht ueber sub17 ab. Ohne Beleg,
   wo genau, wurde nichts angehaengt.
5. **Der Deckel 300 ist eine Notbremse, keine Dauer.** Greift er, ist ein Kanal haengen
   geblieben. Bisher ist das nie gemessen worden; wenn es auftritt, gehoert es untersucht und
   nicht durch ein groesseres Deckel-Zahl zugedeckt.
