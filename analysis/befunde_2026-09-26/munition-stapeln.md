# Munition gleicher Sorte beim Aufnehmen stapeln — das Original stapelt beim Aufnehmen NICHT; der Port bildet das korrekt ab, der Nutzerwunsch ist eine Abweichung vom Original

## 1. Was der Nutzer meldet / was ich gemessen habe

Nutzer: "Munition gleicher Sorte kannst du bitte gleich Stapeln, wenn du sie aufnimmst."

Beobachtung im Port: nimmt man 2x H.GUN BULLETS auf, liegen zwei getrennte
Inventar-Plaetze mit je eigener Menge im Inventar statt eines Platzes mit der Summe.

Selbst gemessen (Laufzeit, Original): alle 50 sauberen Savestates in
`stage_saves/` an `0x800b10ac` gelesen — Platzanzahl `0x800b0fbc` = 10,
Schrittweite 4, ueberall die Startausruestung `01:0 03:15 15:50`. Kein Stand
enthaelt eine zweite aufgenommene Munitionspackung, die Savestates koennen die
Frage also nicht entscheiden (Details §6).

Das ist **kein Port-Defekt** — es ist byte-true nachgebaut. Beleg siehe §2.
(Vorgeschichte: Der Port hat frueher gestapelt; das wurde im Audit `wf_6eea7fa1`
als NICHT byte-true entfernt, Memory `reai-v2-inventory-pickup`.)

## 2. Das Original — mit Adressen und Bytes

### 2.1 Es gibt genau EINEN Einfuege-Pfad fuer Welt-Items: `FUN_8004dc4c`

Roh-Byte-Zensus ueber `info/Re1.5/PSX.EXE` (t_addr 0x80010000, t_size 0xaf000,
Codebereich ab Datei-Offset 0x800) nach dem `jal`-Wort
`0x0C000000 | (ziel>>2)`:

| gesuchtes Ziel | Treffer |
|---|---|
| `jal 0x8004dc4c` (Insert) | **genau 1**: `0x8001e0c4` |
| `jal 0x8004df2c` (erster freier Platz) | 3: `0x8001df14`, `0x8004daec`, `0x8004debc` |
| `jal 0x8004df90` (Zaehler freier Plaetze) | 1: `0x8004dc68` |
| `jal 0x8004dfec` (**Platz mit gleicher Id suchen**) | 2: `0x8004ebbc`, `0x8004ec34` |

`0x8001e0c4` liegt in `FUN_8001db28` (Item-Get-FSM), `case 7`.
Die beiden Aufrufer von `FUN_8004dfec` (dem Gleiche-Id-Sucher) liegen
**beide im NACHLADE-Pfad** — siehe §2.4. Kein einziger Aufruf im Aufnahme-Pfad.

### 2.2 `FUN_8004dfec` @0x8004dfec = "finde Platz mit Id == a0" (der Stapel-Sucher)

```
8004dfec: lui   v0,0x800b
8004dff0: lbu   v0,4028(v0)        ; v0 = Platzanzahl DAT_800b0fbc
8004dff8: beq   v0,zero,0x8004e048
8004dffc: addu  v1,zero,zero       ; v1 = i = 0
8004e000: andi  a0,a0,0xff         ; a0 = gesuchte Item-Id
8004e00c: sll   v0,v1,2            ; i*4  (Platz = 4 Byte)
8004e014: addiu at,at,4268         ; 0x800b10ac = Inventar-Array
8004e01c: lbu   v0,0(at)           ; slot[i].id
8004e024: bne   v0,a0,0x8004e034
8004e02c: j     0x8004e04c         ; Treffer -> return i
8004e048: addiu v0,zero,-1         ; kein Treffer -> return -1
```
Die Funktion EXISTIERT also, wird beim Aufnehmen aber nie gerufen.

### 2.3 `FUN_8004dc4c` @0x8004dc4c — der Insert, Zweig "normales Item"

```
8004dc4c: addiu sp,sp,-32
8004dc54: addu  s0,a0,zero         ; s0 = Item-Id
8004dc5c: addu  s2,a2,zero
8004dc68: jal   0x8004df90         ; Zaehler freier Plaetze -> Ergebnis wird VERWORFEN
8004dc6c: addu  s1,a1,zero         ; s1 = Menge
8004dc70: addiu v0,s0,-14
8004dc74: andi  v0,v0,0xff
8004dc78: sltiu v0,v0,0x6          ; Id-0x0e < 6  -> 2-Zell-Waffe (0x0e..0x13)
8004dc7c: beq   v0,zero,0x8004debc ; sonst: normaler Einzelplatz-Zweig
...
8004debc: jal   0x8004df2c         ; <<< ERSTER FREIER Platz (id==0), NICHT "gleiche Id"
8004dec4: andi  a0,v0,0xff
8004ded4: sll   v0,v0,2
8004dedc: addiu at,at,4268         ; 0x800b10ac
8004dee4: sb    s0,0(at)           ; slot[frei].id  = Item-Id
8004deec: addiu at,at,4269         ; 0x800b10ad
8004def4: sb    s1,0(at)           ; slot[frei].qty = Menge  (ROHE Zuweisung, kein +=)
8004defc: addiu at,at,4270         ; 0x800b10ae
8004df04: sb    zero,0(at)         ; slot[frei].flag = 0
8004df08: jal   0x800492b8         ; Icon hochladen
8004df24: jr    ra
```
**`sb s1` = Zuweisung, nicht Addition.** Es gibt im ganzen Insert kein `lbu`
des alten qty, keine Addition, keinen Vergleich gegen eine Obergrenze.

### 2.4 `FUN_8004df2c` @0x8004df2c = "erster FREIER Platz" (Beleg, dass §2.3 nicht stapelt)

```
8004df2c: lui   v0,0x800b
8004df30: lbu   v0,4028(v0)        ; Platzanzahl DAT_800b0fbc
8004df58: lbu   v0,0(at)           ; slot[i].id
8004df60: bne   v0,zero,0x8004df70 ; id != 0 -> weiter
8004df68: j     0x8004df88         ; id == 0 -> return i
8004df84: addiu v0,zero,-1         ; voll -> return -1
```
Das Argument wird **nicht** ausgewertet (kein `andi a0,a0,0xff`, kein Vergleich
gegen a0) — anders als in `FUN_8004dfec` @0x8004e000/0x8004e024.

### 2.5 Die Obergrenzen-Tabelle @0x80074da8 wird beim Aufnehmen nicht gelesen

Tabelle `0x80074da8`, **Schrittweite 12 Byte**, Index = Item-Id:
`+0x00 u32 = Obergrenze je Platz`, `+0x04 = Zeiger auf den Item-Deskriptor`,
`+0x08 u8`, `+0x09 u8`.

Roh-Bytes (`re15_disasm.py bytes 0x80074da8 0x180`):
```
80074da8: 00 00 00 00 88 4c 07 80 00 00 00 00 | 00 00 00 00   <- Eintrag 0
80074db8: 88 4c 07 80 00 00 00 00 ...
80074dc8: ... 0f 00 00 00 8c 4c 07 80 03 01 00 00             <- Eintrag 3 @0x80074dcc, max 0x0f
80074dfc:     06 00 00 00 9c 4c 07 80 03 01 00 00             <- Eintrag 7 @0x80074dfc, max 0x06
80074e08:     07 00 00 00 a0 4c 07 80 03 01 00 00             <- Eintrag 8 @0x80074e08, max 0x07
80074ea4:     fa 00 00 00 cc 4c 07 80 00 07 00 00             <- Eintrag 0x15 @0x80074ea4, max 0xfa
```
Zensus der Zugriffe auf `0x80074da8` (Obergrenzen-Feld) im ganzen PSX.EXE —
7 Stellen: `0x80049798`, `0x8004a11c`, `0x8004e330`, `0x8004e400`, `0x8004e43c`,
`0x8004e50c`, `0x8004ec74`. **Keine davon liegt im Insert-Bereich
0x8004dc4c..0x8004df28.**

### 2.6 Der Aufnahme-Pfad davor stapelt auch nicht

`FUN_8001db28` (Item-Get-FSM, Zustand in `DAT_80072d3b`):
* `case 5` @0x8001df14: `DAT_8008f62c = FUN_8004df2c()` — nur die Voll-Pruefung
  (`== 0xff` -> Aufnahme verweigert), nicht "gibt es schon so ein Item".
* `case 7` @0x8001e0c4: `FUN_8004dc4c(DAT_800afbb6, puVar11[1], DAT_800ac77c+0x21a0)`.

Wer die FSM startet, ist ebenfalls eindeutig: Zensus der Zugriffe auf den
FSM-Zustand `0x80072d3b` findet nur einen Schreiber ausserhalb der FSM selbst:
`0x8004334c` in `FUN_80043328` (SCD-Opcode-Handler):
```
80043328: lui   v0,0x8007
8004332c: lbu   v0,11579(v0)       ; DAT_80072d3b (FSM-Zustand)
80043334: bne   v0,zero,0x80043368 ; laeuft schon -> nichts tun
80043338: ori   v1,zero,0x1
8004333c: lbu   v0,0(a0)           ; Item-Id aus dem SCD-Operanden
8004334c: sb    v1,11579(at)       ; DAT_80072d3b = 1  (FSM starten)
80043354: sb    v0,-31453(at)      ; DAT_800b8523 = Item-Id
8004335c: sb    v0,-1098(at)       ; DAT_800afbb6 = Item-Id
80043364: sw    a0,-13776(at)      ; DAT_800aca30 = SCD-Datenzeiger
```
Auch hier: kein Inventar-Scan, kein Mengen-Zusammenlegen.

Und der Zensus ueber das Inventar-Array selbst (`0x800b10ac`) findet **55**
Zugriffsstellen, **alle** im Inventar-Modul `0x8004a...0x8004e...` — es gibt keinen
zweiten, versteckten Einfuege-Pfad ausserhalb.

### 2.7 Kein Overlay ruft den Insert — Zensus ueber alle sechs Stage-Overlays

Roh-Byte-Scan (`jal`-Wort, Basis 0x80100000, ohne Header) ueber
`info/Re1.5/PSX/BIN/STAGE{1..6}.BIN`:

| Ziel | STAGE1 | STAGE2 | STAGE3 | STAGE4 | STAGE5 | STAGE6 |
|---|---|---|---|---|---|---|
| `jal 0x8004dc4c` | 0 | 0 | 0 | 0 | 0 | 0 |
| `jal 0x8004dfec` | 0 | 0 | 0 | 0 | 0 | 0 |
| `jal 0x8004df2c` | 0 | 0 | 0 | 0 | 0 | 0 |
| `jal 0x80043328` | 0 | 0 | 0 | 0 | 0 | 0 |

Gegenprobe, damit die Null nicht am Scanner liegt: derselbe Scan zaehlt in
STAGE1 **1079** und in STAGE2 **736** `jal` in den EXE-Bereich
0x80010000..0x800bf000 (z.B. `jal 0x80065f60 @0x801004ac`). Der Scanner
funktioniert; es gibt schlicht keinen Overlay-Aufruf ins Inventar-Modul.

### 2.8 Zensus der Munitionssorten und ihrer Obergrenzen (Frage b)

Byte-true aus der Tabelle @0x80074da8 (Feld +0, Schrittweite 12). Munition ist
byte-true der Id-Bereich **0x15..0x21** — die Schranken sind nachgelesen:
`sltiu v0,v0,0x15` @0x80047d54 (Waffen-Ende) und `sltiu v0,v0,0x22` @0x80049124
(Schluessel-/Heil-Anfang).

| Id | Name (Port-Katalog) | Obergrenze @0x80074da8+12*Id |
|---|---|---|
| 0x15 | H. GUN BULLETS | **250** |
| 0x16 | SHOTGUN SHELLS | **250** |
| 0x17 | MAGNUM BULLETS | **250** |
| 0x18 | FLAME FUEL | **100** |
| 0x19 | EXPLOSIVE RND | **250** |
| 0x1a | ACID ROUNDS | **250** |
| 0x1b | INCEND. ROUNDS | **250** |
| 0x1c | REMOTE DETON. | **250** |
| 0x1d | EMPTY SHELLS | **250** |
| 0x1e | NITRO CAPSULE | **250** |
| 0x1f | ACID CAPSULE | **250** |
| 0x20 | INCEND. CAPSULE | **250** |
| 0x21 | MEMORY CARD | **250** |

**Ja, die Grenze haengt von der Sorte ab** — FLAME FUEL (0x18) hat 100, alle
anderen Munitionssorten 250. Zum Vergleich: Waffen 0x03..0x06 = 15, 0x07 = 6,
0x08 = 7, 0x0c = 100, 0x0d = 12, 0x0e = 100; ab Id 0x22 (Schluessel/Heilmittel)
steht ueberall **1**.

### 2.9 Wie das Original stapelt, WENN es stapelt: die Menue-Zusammenlegung `FUN_8004e054`

Das ist der einzige Ort mit echter Stapel-Arithmetik — er zeigt die byte-true
Semantik, die eine Aufnahme-Stapelung uebernehmen muesste.

Summe (@0x8004e13c-160, `addu s0`): `s0 = qty[A] + qty[B]`.
Dann zweimal gespiegelt (Aktion 2 @0x8004e320, Aktion 3 @0x8004e42c):
```
8004e320: sll   v0,s2,1            ; Id * 12
8004e324: addu  v0,v0,s2
8004e328: sll   a0,v0,2
8004e330: addiu at,at,19880        ; 0x80074da8
8004e338: lbu   v1,0(at)           ; v1 = Obergrenze(Id)
8004e340: sltu  v0,v1,s0           ; Obergrenze < Summe ?
8004e344: bne   v0,zero,0x8004e3e4 ;   ja  -> TEILUEBERTRAG
  8004e3f4: sb  v1,0(at)           ;      Ziel.qty   = Obergrenze
  8004e410: subu v0,s0,v0          ;      Quelle.qty = Summe - Obergrenze
8004e34c: ...                      ;   nein -> VOLL
  8004e370: sb  DAT_800b25d5       ;      Ziel.id  = Ergebnis-Id
  8004e398: addu v0,v0,a2 / sb     ;      Ziel.qty = Ziel.qty + Quelle.qty
  8004e3b8/c8/d8: sb zero          ;      Quelle.id/qty/flag = 0
  8004e310: jal 0x8004947c         ;      Quell-Zelle leeren
```
Regel byte-true: **Ziel = min(Summe, Obergrenze(Id)); Rest = Summe - Obergrenze
bleibt in der Quelle; passt alles, wird die Quelle geloescht.**
(Dekompilat-Gegenprobe `RE_15_Quellcode_V2/FUN_8004e054.c:26-27` = Summe,
`:61-63` = der Teiluebertrag — die Zeilen decken sich mit den Bytes.)

### 2.10 RE2-Retail-Vergleich (Frage c): identisch, auch RE2 stapelt beim Aufnehmen nicht

RE2 Leon `info/re2leon/PSX.EXE` (t_addr 0x80010000). Inventar-Array
`0x800d4a3c`, Schrittweite 4 (Quelle fuer die Adresse: eigenes Dossier
`analysis/itembox_re2/re2-box-speicher.md` §2 — hier per Zensus nachgeprueft:
19 direkte Zugriffe, alle im Inventar-/Box-Modul).

Die drei Helfer entsprechen 1:1 denen von RE1.5:
* `FUN_80069668` = erster freier Platz / Zaehler (`RE2_Quellcode_V2/FUN_80069668.c`)
* `FUN_800696cc` = Platz mit gleicher Id (`RE2_Quellcode_V2/FUN_800696cc.c`)
* `FUN_8006947c` = Platz schreiben {id,qty,flag} (`RE2_Quellcode_V2/FUN_8006947c.c`)

Der Insert `FUN_80069adc` (nachgelesen, nicht nur zitiert):
```
80069adc: addiu sp,sp,-40
80069ae4: addu  s1,a0,zero         ; Id
80069aec: addu  s2,a1,zero         ; Menge
80069af4: addu  s3,a2,zero         ; Icon-Quelle
80069af8: addu  a0,zero,zero
80069b00: jal   0x80069668         ; <<< ERSTER FREIER Platz (a0=0), NICHT "gleiche Id"
80069b08: addu  s0,v0,zero
80069b0c: addiu v0,zero,19         ; Id == 0x13 -> Einzelplatz
80069b10: beq   s1,v0,0x80069b6c
80069b14: addiu v0,s1,-14
80069b18: sltiu v0,v0,0x6          ; 0x0e..0x13 -> 2-Zell-Waffe
...
80069b78: jal   0x8006947c         ; slot[s0] = {Id, Menge, 0}
80069b7c: addu  a3,zero,zero
```
Kein `jal 0x800696cc`, keine Addition, kein Obergrenzen-Vergleich — genau wie
RE1.5. Xref-Zensus: `jal 0x80069adc` hat **2** Aufrufer, `0x80058864`
(SCD-Item-Get; Menge aus dem Operanden `lbu a1,2(v0)` @0x80058860) und
`0x80072320` (Item-Box-Entnahme). Der Item-Get-Aufrufer prueft davor nichts
Inventar-Seitiges.

Die RE2-Stellen, die `FUN_800696cc` (gleiche Id) rufen, sind SCD-Opcodes zum
**Entfernen/Pruefen**, nicht zum Hinzufuegen: `0x800584f0` (Item vorhanden?),
`0x800585e4` (Item loeschen: `sb zero` @0x80058618/24/30 + Kompaktierung
`jal 0x80069714` @0x80058634), `0x80058660` (Menge abziehen: `lbu` @0x80058698,
`subu v0,v0,v1` @0x800586a4, `sb` @0x800586b0).

**Fazit (a)+(c): In beiden Engines gibt es beim Aufnehmen KEIN Stapeln.**
Die Gleiche-Id-Suche existiert in beiden, wird aber nur vom Nachladen (RE1.5
@0x8004ebbc/@0x8004ec34) bzw. von Script-Opcodes (RE2) benutzt.

## 3. Was der Port tut — mit datei.c:zeile

**Der Port macht es exakt wie das Original — er stapelt bewusst nicht.**

* `re15_port/engine/src/inventory_common.c:23-63` `re15_inv_grant()` — der Kopf-
  Kommentar `:27-33` zitiert genau die Belege aus §2.3/§2.4. `:31-34` sucht den
  ersten FREIEN Platz, `:52-58` schreibt `{id, qty, flags=0}` roh hinein. Es gibt
  keinen Zweig, der einen vorhandenen Platz gleicher Id sucht.
* `re15_port/engine/src/item_modal_common.c:226` `s_grant = inv_free_slot();`
  (= `FUN_8004df2c` @0x8004df2c) und `:293` `re15_inv_grant(s_type, s_amount)`
  (= `FUN_8004dc4c` @0x8004dc4c in Zustand 7). Das ist der EINZIGE Produktiv-Pfad
  einer Welt-Aufnahme; beide Aufrufer von `re15_item_modal_start` sind die
  Item-AOT (`aot_common.c:648` Sofortzuendung, `aot_common.c:1303` Scan-Zuendung).
* `re15_port/engine/src/item_modal_common.c:143-154` — hier sitzt bereits eine
  **markierte Nutzer-Abweichung**: `re15_pickup_menge_nutzer()` halbiert
  aufgesammelte Munition (Nutzer-Entscheidung 2026-09-20). Genau dieses Muster
  ("byte-true Kern + ausdruecklich markierte Nutzer-Regel an EINER Stelle") ist
  die Vorlage fuer das Stapeln.
* `re15_port/engine/src/inventory_common.c:238-240` — die byte-true
  Id-Klassifikation: `re15_item_is_ammo(id) = 0x15..0x21`
  (`sltiu ...,0x15` @0x80047d54 / `sltiu ...,0x22` @0x80049124, beide nachgelesen).
* `re15_port/engine/src/menu_common.c:820` `#define X_PROP_TBL 0x80074DA8u` und
  `:921-950` `x_reload()` — die byte-true Stapel-Arithmetik aus §2.9 ist im Port
  **schon vorhanden**, aber nur im Menue-Pfad: `cap = xu8(X_PROP_TBL + id*12)`
  (`:925`), `if (cap < total)` -> `dst.qty = cap; src.qty = total - cap` (`:927-932`),
  sonst Summe + Quelle leeren (`:939-948`).
* Pins, die die heutige Nicht-Stapelung festnageln:
  `re15_port/tests/unit/test_inv_grant.c:22-28` ("no-stack: slot1 = {0x15, 20}
  (separate slot, NOT merged to 50)") — dieser Test muss mitgeaendert werden.
  `re15_port/tests/unit/probe_r17_munition.c` misst nur die SUMME im Beutel
  (`menge_im_beutel`) und bleibt gruen. `test_item_stack_press.c` prueft die
  Druck-pro-Item-Ordnung, nicht die Platzbelegung, und bleibt gruen.

## 4. Der Unterschied, in einem Satz

Es gibt keinen Unterschied zum Original — RE1.5 (und RE2) legen beim Aufnehmen
IMMER einen neuen Platz an; der Nutzerwunsch "gleich stapeln" ist eine bewusste
Abweichung vom Auslieferungsstand und muss als solche markiert werden.

## 5. Umsetzungsplan (konkrete Dateien/Zeilen, jede Konstante mit @0x)

Grundsatz: **Der byte-true Insert bleibt unveraendert.** Die Abweichung kommt an
EINE Stelle, mit dem gleichen STOP-Markierungsmuster wie
`re15_pickup_menge_nutzer` — und sie borgt sich die Arithmetik byte-true aus der
Menue-Zusammenlegung `FUN_8004e054`, statt eine neue zu erfinden.

### 5.1 Neue Nutzer-Regel neben `re15_pickup_menge_nutzer`

`re15_port/engine/src/inventory_common.c`, direkt unter `re15_pickup_menge_nutzer`
(heute `:252-259`), neue Funktion `re15_pickup_stapeln_nutzer(uint8_t id, uint8_t menge)`;
Deklaration in `re15_port/include/re15_inventory.h` neben `:112`.

Verhalten (jede Konstante belegt):
1. Nur fuer `re15_item_is_ammo(id)` — Id-Fenster 0x15..0x21, byte-true
   (`sltiu ...,0x15` @0x80047d54, `sltiu ...,0x22` @0x80049124).
   Alles andere: unveraendert durchreichen, also weiterhin neuer Platz.
2. Zielplatz = erster Platz mit gleicher Id. Die Suchfunktion existiert byte-true
   als `FUN_8004dfec` @0x8004dfec (Port: `re15_inv_find_item`); sie liefert -1,
   wenn es keinen gibt (@0x8004e048 `addiu v0,zero,-1`).
   ACHTUNG, **nicht** den Nachlade-Sonderfall uebernehmen: `FUN_8004eb70`
   @0x8004ebc4-c8 (`slt zero,slot`) ignoriert Platz 0 — das ist eine Eigenheit
   des Nachladens, nicht der Suche. Fuer das Stapeln gilt Platz 0 mit.
3. Obergrenze = `Tabelle[0x80074da8 + Id*12] + 0`, als u8 gelesen wie @0x8004e338
   (`lbu v1,0(at)`): 250 fuer alle Munitionssorten ausser FLAME FUEL 0x18 = 100
   (§2.8). Im Port steht die Tabelle schon als `X_PROP_TBL`
   (`menu_common.c:820`) — fuer die Nutzung aus `inventory_common.c` entweder
   dorthin verschieben oder einen kleinen Zugriff `re15_item_cap(id)` anbieten;
   **nicht** die Zahlen 250/100 hart hinschreiben, sie stehen im Blob.
4. Arithmetik **wortgleich zur Menue-Zusammenlegung** `FUN_8004e054`:
   `summe = ziel.qty + menge` (@0x8004e13c-160);
   `if (obergrenze < summe)` (@0x8004e340 `sltu v0,v1,s0`) ->
   `ziel.qty = obergrenze` (@0x8004e3f4), Rest `summe - obergrenze`
   (@0x8004e410 `subu v0,s0,v0`) geht als NEUER Platz den normalen
   `re15_inv_grant`-Weg; `else` -> `ziel.qty = summe` (@0x8004e398-3a8),
   kein neuer Platz.
   (Praktisch tritt der Ueberlauf im Auslieferungsstand fast nie ein: die
   groesste Munitionspackung im Spiel ist 30 Stueck — Zensus
   `tools/aot_sce_census.py` ueber 240 RDTs, zitiert in
   `probe_r17_munition.c:65-69` — und sie wird vorher noch halbiert.)

### 5.2 Einhaengen an genau einer Stelle

`re15_port/engine/src/item_modal_common.c:293`, Zustand 7:
vor `re15_inv_grant(s_type, s_amount)` erst `re15_pickup_stapeln_nutzer()`
versuchen; liefert sie "vollstaendig gestapelt", ist die Aufnahme fertig
(Rueckgabe wie ein erfolgreicher Grant, damit `s_taken`,
`scd_prop_hide_by_obj_id(s_taken_prop)` und `g_aot.slots[].active = 0` in
`:300-317` unveraendert weiterlaufen); liefert sie einen Rest, geht der Rest in
`re15_inv_grant`.

Zusaetzlich `item_modal_common.c:226` (`s_grant = inv_free_slot()`): heute ist
"kein freier Platz" gleich "kann ich nicht tragen" (`:234` `s_prompt = 2`,
byte-true @0x8001df14 + @0x8001e054). Mit dem Stapeln muss dieser Zweig
zusaetzlich zulassen, dass ein vorhandener gleicher Platz noch Luft hat —
sonst bleibt bei vollem Inventar die Munition liegen, obwohl sie draufpasst.
Auch das ist Nutzer-Regel, nicht byte-true, und gehoert in denselben markierten
Kommentar. Der Waffen-Sonderfall `:232` (Ids 0x0e..0x13, 2-Zell-Schub) bleibt
unangetastet — Waffen stapeln nie (byte-true-Zweig @0x8004dc78 `sltiu v0,v0,0x6`).

### 5.3 Tests

* `re15_port/tests/unit/test_inv_grant.c:22-28` — Block (1) pinnt heute das
  Gegenteil. Der byte-true Insert `re15_inv_grant` soll weiterhin NICHT stapeln
  (Test bleibt inhaltlich bestehen, Kommentar praezisieren); die neue Regel wird
  eine Ebene hoeher getestet (`re15_pickup_stapeln_nutzer` / Modal-Zustand 7).
* Neuer Test analog `probe_r17_munition.c` Abschnitt B: zweimal
  `modal_durchlaufen(0x15, 30)` -> EIN Platz mit 30 (2x 30, jeweils halbiert),
  plus ein Ueberlauf-Fall gegen die Tabellen-Obergrenze (0x18 = 100
  @0x80074da8+12*0x18, 0x15 = 250 @0x80074ea4) mit Rest-Platz.
* `test_item_stack_press.c` und `probe_r17_munition.c` (Summe im Beutel) muessen
  unveraendert gruen bleiben.

## 6. Offen / nicht belegt

* **Nicht belegbar, weil es das Original nicht tut:** eine Aufnahme-Stapelung mit
  eigener `@0x…`-Adresse. Es gibt sie in RE1.5 nicht (§2.1-§2.7) und in RE2 nicht
  (§2.10). Jede Zahl, die ueber die hier zitierten Tabellen- und
  Arithmetik-Belege hinausginge, waere geraten — deshalb borgt §5 alles aus
  `FUN_8004e054`.
* **Dynamisch VERSUCHT, kein Gegenbeweis moeglich:** alle **50** lesbaren sauberen
  Savestates unter `stage_saves/` (die sechs `PATCHED-EXE_*`-Staende sind laut
  CLAUDE.md ein Mod und wurden ausgelassen) wurden an `0x800b10ac` ausgelesen.
  Bestaetigt wird dabei das Layout: Platzanzahl `0x800b0fbc` = **10**,
  Schrittweite 4, und in jedem einzelnen Stand steht die Startausruestung
  `01:0  03:15  15:50` (KNIFE / BROWNING HP x15 / H.GUN BULLETS x50).
  **Kein einziger Stand hat je eine zweite Munitionspackung aufgenommen**, also
  gibt es dort weder ein Doppel noch ein Zusammenlegen zu sehen — die Savestates
  koennen die Frage nicht entscheiden. Der statische Beweis bleibt vollstaendig
  (ein einziger Insert, ein einziger Aufrufer, kein Overlay-Aufruf).
  Naechster Weg, falls jemand die Laufzeit-Bestaetigung will:
  `re15-room-capture` in ROOM1020 (dort liegen laut
  `test_item_stack_press.c:3` zwei H.GUN-BULLETS-AOTs auf demselben Rechteck,
  main00 @0x1d6a und @0x1d80) beide aufnehmen und `0x800b10ac` erneut lesen.
* **Nicht geprueft:** die Item-Box-Entnahme im Port (`re15_itembox.c`) — sie hat
  ihr eigenes byte-true Transfer-Modell
  (`analysis/itembox_re2/re2-box-transfer.md`) und ist vom Nutzerwunsch
  ("wenn du sie aufnimmst") nicht beruehrt. Falls das Stapeln auch dort gelten
  soll, ist das eine zweite Entscheidung.
* **Nicht entschieden (bewusst):** ob die Regel gebaut werden soll. Sie weicht
  vom Auslieferungsstand ab; nach Memory `reai-v2-original-oder-nicht` darf das
  kein Env-Schalter und keine Wahlfrage im Code werden, sondern muss eine
  markierte Nutzer-Entscheidung an genau einer Stelle sein — so wie die
  Munitions-Halbierung vom 2026-09-20. Diese Runde baut nichts (Regel 1).
