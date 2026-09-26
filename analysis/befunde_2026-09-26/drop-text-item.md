# Wegwerf-Text: "item" statt "key" fuer Nicht-Schluessel — es gibt im Original KEINE zweite Wortlaut-Variante; die Aenderung ist eine reine Nutzer-Entscheidung, und der byte-naehere Weg ist eine Laufzeit-Ersetzung von genau 3 Glyphen im Skript-Lauf, ohne ein einziges Byte der ausgelieferten Tabelle anzufassen

Stand 2026-09-26. Alles unten selbst nachgelesen aus
`re15_port/shared_assets/PSX/BIN/DEBUG.BIN` (== `info/Re1.5/PSX/BIN/DEBUG.BIN`,
0x40000 Bytes, laedt RAW nach 0x800C0000 → **Datei-Offset == RAM − 0x800C0000**) und
`info/re2leon/PSX.EXE` (PS-X EXE, `t_addr = 0x80010000` @0x18, `t_size = 0x000F0800`
@0x1C, Dateigroesse 0xF1000 → **RAM = 0x80010000 + Datei-Offset − 0x800**).
Beide Baeume NUR gelesen. Kein Build, keine Engine-Aenderung in diesem Lauf.

---

## 1. Was der Nutzer meldet / was ich gemessen habe

Nutzer: *"aendere fuer items den text zu item statt key in der drop message."*

Gemessen: die Abfrage zeigt fuer **alle** acht Gegenstaende denselben Satz, weil sie
immer dasselbe Prompt-Skript [6] abspielt (`item_discard_common.c:19`
`#define RE15_DISCARD_PROMPT_KEY 8`, aufgeloest in `item_prompt_common.c:60`
`case 8: return 6;`). Der Satz enthaelt das Wort **key** fest verdrahtet in den
ausgelieferten Daten.

Von den 14 ausgelieferten Wegwerf-Stellen (`gen/discard_sites.inc`) betreffen acht
Gegenstaende. **Drei davon sind nachweislich keine Schluessel** — gemessen am
ausgelieferten Namen, nicht behauptet (Namenstabelle 102 u16 @0x800C495C, Blob
@0x800C4A28, 0x07-terminiert; Zensus s. §2.4):

| Id | Name (aus dem Blob dekodiert) | Namens-Adresse | traegt "Key"? |
|----|-------------------------------|----------------|----------------|
| 0x30 | Pliers                      | @0x800C4CD5 | **nein** |
| 0x31 | Fire Extinguisher           | @0x800C4CDC | **nein** |
| 0x36 | Green Keycard               | @0x800C4D20 | ja |
| 0x37 | Red Keycard                 | @0x800C4D2E | ja |
| 0x38 | Blue Keycard                | @0x800C4D3A | ja |
| 0x44 | Minidisc Player w/ Disc     | @0x800C4DBB | **nein** |
| 0x46 | Red Master Keycard          | @0x800C4DDD | ja |
| 0x47 | Blue Master Keycard         | @0x800C4DF0 | ja |

---

## 2. Das Original — mit Adressen und Bytes

### 2.1 Wo der Text liegt (Frage a)

Prompt-Skript-Tabelle **@0x800C4FC6** (Datei-Offset 0x4FC6 in DEBUG.BIN), 8 u16, Offsets
**relativ zur Tabelle selbst** (Selektor `FUN_80027e68`: @0x80027f20 `sll v0,a2,1`,
@0x80027f30 `addiu at,at,20422` = 0x800C4FC6, @0x80027f44 `addu v0,v0,at`). Selbst
ausgelesen:

```
@0x800C4FC6  10 00 2f 00 50 00 6b 00 8b 00 a9 00 c5 00 f8 00
   [0] +0x0010 -> 0x800C4FD6   [4] +0x008B -> 0x800C5051
   [1] +0x002F -> 0x800C4FF5   [5] +0x00A9 -> 0x800C506F
   [2] +0x0050 -> 0x800C5016   [6] +0x00C5 -> 0x800C508B   <== die Wegwerf-Abfrage
   [3] +0x006B -> 0x800C5031   [7] +0x00F8 -> 0x800C50BE
```
Der kleinste Offset (0x10) ist zugleich das physische Ende der Tabelle → genau 8 Skripte.

Skript [6], **@0x800C508B** (Datei-Offset 0x508B), 51 Bytes bis zum naechsten Skript
(0xC5 + 51 = 0xF8), roh gelesen:

```
@0x800C508B  35 4b 51 00 40 4b 4a 3a 50 00 4a 41 41 40 00 50   "You don't need t"
@0x800C509B  44 45 4f 00 47 41 55 00 3d 4a 55                  "his key any"
@0x800C50A6  08                                                Zeilenumbruch
@0x800C50A7  49 4b 4e 41 57 00 20 45 4f 3f 3d 4e 40 00 45 50   "more. Discard it"
@0x800C50B7  1b                                                '?'
@0x800C50B8  03                                                Ja/Nein-Seite
@0x800C50B9  02 f9 02                                          Branch, Ja-Aktion 0x02 = WEGWERFEN
@0x800C50BC  01                                                END
```

Glyphen-Kodierung bestaetigt (Dekoder `re15_msg_glyph`, `msg_common.c:175-199`):
0x00 = Leerzeichen, 0x1D-0x36 = A-Z, 0x3D-0x56 = a-z, 0x3A = `'`, 0x1B = `?`,
0x57 = `.`, 0x08 = Zeilenumbruch, 0x03 = Auswahlseite.

**Das Wort "key" steht damit an genau drei Byte-Stellen:**

```
@0x800C509F  47   'k'     (Datei-Offset 0x509F, Blob-Index 0xD9, Skript-Index +20)
@0x800C50A0  41   'e'     (Datei-Offset 0x50A0, Blob-Index 0xDA, Skript-Index +21)
@0x800C50A1  55   'y'     (Datei-Offset 0x50A1, Blob-Index 0xDB, Skript-Index +22)
```
Gegenprobe per Roh-Byte-Suche nach `50 44 45 4f 00 47 41 55` ("this key") ueber den
ganzen ausgelieferten RE1.5-Baum (3193 Dateien): **genau 1 Treffer**, DEBUG.BIN @0x509A.
Die Suche nach `20 45 4f 3f 3d 4e 40` ("Discard"): ebenfalls **genau 1 Treffer**,
DEBUG.BIN @0x50AD. Es gibt also im Auslieferungsstand nur diese eine Wegwerf-Zeile.

Zusammensetzung zur Laufzeit: `re15_item_prompt_walk` (`item_prompt_common.c:66-105`)
laeuft das Skript Byte fuer Byte ab und reicht jede Glyphe an einen Callback —
0x01/0x03 = Abbruch, 0x02 = Branch (2 Operanden ueberspringen), 0x05 = Farbe,
0x06 = Namen einsetzen, 0x08 = Zeilenumbruch, sonst literale Glyphe. Skript [6] traegt
**kein** 0x06, der Gegenstandsname kommt in diesem Satz also gar nicht vor.

### 2.2 Wo "item" bereits ausgeliefert ist — die Ersatzglyphen sind nicht erfunden

Roh-Byte-Suche nach `45 50 41 49` ("item") in DEBUG.BIN: 2 Treffer,
**@0x800C500E** und @0x800C5C06. Der erste liegt im Prompt-Skript [1]:

```
@0x800C4FF5  35 4b 51 00 3f 3d 4a 3a 50 00 3f 3d 4e 4e 55 00 3d 4a 55   "You can't carry any"
@0x800C5008  08                                                          Zeilenumbruch
@0x800C5009  49 4b 4e 41 00 45 50 41 49 4f 57 01                         "more items."
                        ^^^^^^^^^^^
                        @0x800C500E = 45 50 41 49 = "item"
```
Die vier Ersatzbytes `45 50 41 49` sind damit **aus dem Auslieferungsstand entnommen**
(@0x800C500E), nicht zusammengesetzt.

### 2.3 Hat RE2 eine zweite Variante mit "item"? (Frage b) — NEIN

Roh-Byte-Zensus (glyph-kodiert, nicht ASCII) ueber **`info/re2leon/**` = 21597 Dateien**
(EXE, SLUS_007.48, 250 Leon-RDTs, alle ausgepackten `msg/*.msg`, COMMON/BIN, PL0):

| Glyphenlauf | Bytes | Treffer |
|-------------|-------|---------|
| `Discard` | `20 45 4f 3f 3d 4e 40` | **2** (PSX.EXE @0x8F979 **und** SLUS_007.48 @0x8F979 — dasselbe Binaerbild, doppelt im Baum) |
| `discard` | `40 45 4f 3f 3d 4e 40` | 2 (ROOM2040.RDT @0x26EC / room2040/msg/sub03.msg — *"There appear to be discarded files inside."*, kein Prompt) |
| `This key` | `30 44 45 4f 00 47 41 55` | 2 (= dieselbe EXE zweimal, @0x8F960) |
| `This item` | `30 44 45 4f 00 45 50 41 49` | **0** |
| `this item` | `50 44 45 4f 00 45 50 41 49` | 4 — **keiner davon ist ein Wegwerf-Text**: @0x91002 *"You need two item spaces to obtain this item."*, @0x910A0 *"You need more inventory space to hold this item."* (je zweimal, EXE + SLUS) |
| `useless` | `51 4f 41 48 41 4f 4f` | 8 — davon nur @0x8F96C die Wegwerf-Zeile; @0x91FF4 *"It's useless by itself"* (C4-Zuender), Rest RDT-Untersuchen-Texte |

Die eine RE2-Wegwerf-Zeile, selbst nachgelesen (Datei-Offset 0x8F960 = RAM 0x8009F160):

```
@0x8009F160  30 44 45 4f 00 47 41 55 00 45 4f 00 51 4f 41 48 41 4f 4f 00 4a 4b 53   "This key is useless now"
@0x8009F177  01                                                                      '.'
@0x8009F178  fc                                                                      Zeilenwechsel
@0x8009F179  20 45 4f 3f 3d 4e 40                                                    "Discard"
@0x8009F180  1b                                                                      '?'
@0x8009F181  fb 00                                                                   Auswahlbox
@0x8009F183  fe 00                                                                   Seitenende
```

> **RE2 hat GENAU EINE Wegwerf-Abfrage, und die sagt "key". Eine Variante mit "item"
> existiert im RE2-Bestand nicht — 0 Treffer.** Das deckt sich mit
> `analysis/befunde_2026-09-21/discard-re2-mechanismus.md` §0 und ergaenzt es um den
> Vollzensus ueber den gesamten Baum (dort war nur die EXE durchsucht).

### 2.4 Das datengetriebene Schluessel-Kriterium — gemessen, keine Liste

Kriterium: **der ausgelieferte Name des Gegenstands enthaelt den Glyphenlauf
`27 41 55` ("Key") oder `47 41 55` ("key")**. Quelle ist die Original-Namenstabelle
(Offsettabelle 102 u16 @0x800C495C, Blob @0x800C4A28, Reader `FUN_80028840` @0x80028840
ohne Bereichspruefung; 102 = (0x800C4A28 − 0x800C495C)/2).

Vollzensus ueber **alle 102 Namen**: **8 Treffer**, und zwar ausschliesslich echte
Schluessel:

```
0x36 @0x800C4D20  Green Keycard          0x42 @0x800C4D9E  Key Disc
0x37 @0x800C4D2E  Red Keycard            0x45 @0x800C4DD3  Water Key
0x38 @0x800C4D3A  Blue Keycard           0x46 @0x800C4DDD  Red Master Keycard
0x39 @0x800C4D47  Yellow Keycard         0x47 @0x800C4DF0  Blue Master Keycard
```

Geschnitten mit den acht Gegenstaenden der Wegwerf-Stellen ergibt das **genau** die
Aufteilung, die der Nutzer nennt:
* **Schluessel (Original-Zeile bleibt):** 0x36, 0x37, 0x38, 0x46, 0x47
* **Nicht-Schluessel (neue Zeile):** 0x30 Pliers, 0x31 Fire Extinguisher,
  0x44 Minidisc Player w/ Disc

Das Kriterium liest also die ausgelieferten Daten und braucht keine gepflegte
Id-Liste und kein `if (room == …)`.

### 2.5 Zeilenbreite — gemessen, nicht geschaetzt

Die Vorschubtabelle der Spielschrift liegt ausgeliefert **@0x800C4416** (indiziert mit
dem Glyphen-Code; der Port liest sie in `render_pc.c:2525`
`s_msgfont_w[code] = dbg[0x4416 + code]`). Selbst ausgelesen:

```
@0x800C4416+0x47 'k' = 8    @0x800C4416+0x45 'i' = 5
@0x800C4416+0x41 'e' = 8    @0x800C4416+0x50 't' = 6
@0x800C4416+0x55 'y' = 8    @0x800C4416+0x41 'e' = 8
                            @0x800C4416+0x49 'm' = 9
```

Damit exakt gerechnet (Skript [6], Zeile 1):
* mit `key`:  27 Glyphen = **185 px**
* mit `item`: 28 Glyphen = **189 px**  → **Delta +4 px**
* Zeile 2 unveraendert: 112 px

Bezugsgroesse aus denselben Daten: die breiteste Zeile, die die 8 ausgelieferten
Skripte mit den 102 ausgelieferten Namen ueberhaupt erzeugen, ist **289 px**
(Skript [3] "Will you place the …" mit Item 0x44 "Minidisc Player w/ Disc"), Skript [2]
288 px, Skript [4] 275 px. Gezeichnet wird ab **x = 34** (`main.c:9746`
`re15_render_item_prompt(34, 180, …)`, dieselbe Box wie das Original 0x22/0xb4
@0x80027eec/@0x80027f14). 34 + 189 = 223 px — **66 px innerhalb** dessen, was der
Auslieferungsstand schon zeichnet. Ein Umbruch-/Ueberlauf-Problem entsteht durch die
Aenderung also nachweislich nicht.

---

## 3. Was der Port tut — mit datei.c:zeile

* `re15_port/engine/src/item_discard_common.c:19`
  `#define RE15_DISCARD_PROMPT_KEY 8` — der eine Prompt-Schluessel der Abfrage.
* `re15_port/engine/src/item_discard_common.c:365-371` `re15_discard_prompt()` gibt
  diesen Schluessel an den Zeichner zurueck; `*out_item` traegt den Gegenstand.
* `re15_port/engine/src/item_discard_common.c:278-279`
  `s_reveal_total = re15_item_prompt_walk(RE15_DISCARD_PROMPT_KEY, s_item, 0, 0, 0)`
  — die Schreibmaschinen-Gesamtzahl kommt aus **demselben** Lauf.
* `re15_port/engine/src/item_prompt_common.c:50-63` `prompt_key_to_script()`:
  `case 8: return 6;` — Port-Schluessel 8 → Original-Skript [6].
* `re15_port/engine/src/item_prompt_common.c:66-105` `re15_item_prompt_walk()` —
  der einzige Ort, an dem die Glyphen des Skripts entstehen. Er bekommt `item_id`
  bereits als Parameter (Zeile 66).
* Zeichner, beide Plattformen, beide ueber denselben Lauf:
  `re15_port/platform/pc/src/render_pc.c:2680-2688` und
  `re15_port/platform/psx/src/render.c:406-423`.
* Aufrufstellen der Abfrage: `re15_port/platform/pc/main.c:9743-9747`,
  `re15_port/platform/psx/main.c:802-804`.
* Daten: `re15_port/engine/src/gen/item_prompt_data.inc` —
  `re15_item_prompt_script_off[8] = {0x10,0x2f,0x50,0x6b,0x8b,0xa9,0xc5,0xf8}`,
  `re15_item_prompt_script_blob[290]`; nachgerechnet: `blob[0xD9..0xDB] = 47 41 55`
  ("key"), `blob[0x48..0x4B] = 45 50 41 49` ("item"). Die Datei wird von
  `tools/gen_item_prompt_data.py` aus DEBUG.BIN erzeugt und ist byte-gleich mit
  @0x800C4FC6.

**Der Port spielt also fuer alle acht Gegenstaende bitgleich Skript [6] ab. Es gibt
im Port keine zweite Wortlaut-Variante und keine Stelle, an der der Gegenstand den
Wortlaut beeinflusst.**

---

## 4. Der Unterschied, in einem Satz

Original und Port sagen **immer** "key" (Skript [6] @0x800C508B, "key" @0x800C509F),
weil weder RE1.5 noch RE2 eine zweite Wortlaut-Variante besitzen (RE1.5: 1 Treffer
"Discard" im ganzen Baum; RE2: 1 Treffer, 0 Treffer fuer eine Wegwerf-Zeile mit
"item") — der Wunsch des Nutzers ist damit **keine Original-Abweichung, die man
repariert, sondern eine ausdrueckliche Nutzer-Entscheidung**, die nur so eingebaut
werden darf, dass die Original-Zeile fuer die fuenf echten Schluessel bit-gleich
bleibt.

---

## 5. Umsetzungsplan (konkrete Dateien/Zeilen, jede Konstante mit @0x)

### 5.1 Wegwahl: warum Laufzeit-Ersetzung und **nicht** ein zweiter Skript-Slot

Gegen einen **neunten Skript-Slot** spricht ein Beleg, keine Vorliebe: die Tabelle
@0x800C4FC6 ist **ihre eigene Basis** und ihr kleinster Offset (0x10, @0x800C4FC6) ist
zugleich ihr physisches Ende — daraus leitet der Port die Zahl 8 ab
(`item_prompt_common.c` Kopf, Selektor @0x80027f30/@0x80027f44). Ein 9. Eintrag wuerde
diese Invariante zerstoeren, `re15_item_prompt_script_count()` (== 8) verfaelschen und
`gen/item_prompt_data.inc` vom Auslieferungsstand entkoppeln — die Datei traegt
ausdruecklich "GENERATED … DO NOT EDIT" und ist heute byte-gleich mit DEBUG.BIN.

Ebenfalls dagegen: ein **Patch der Bytes** @0x800C509F..A1 auf "item". Das wuerde die
Original-Zeile fuer die fuenf echten Schluessel zerstoeren und ist ausserdem
laengen-inkompatibel (3 Glyphen vs. 4).

**Byte-naeher ist die Laufzeit-Ersetzung genau im Skript-Lauf**: die ausgelieferte
Tabelle bleibt Byte fuer Byte unangetastet, das Original-Skript [6] wird fuer jeden
Schluessel **unveraendert** abgespielt, und nur fuer Nicht-Schluessel werden an den
drei bekannten Stellen (Skript-Index +20/+21/+22, @0x800C509F..@0x800C50A1) vier
Glyphen aus dem ebenfalls ausgelieferten Lauf @0x800C500E eingesetzt.

### 5.2 Die Aenderung — eine Datei, ein Ort

**Datei: `re15_port/engine/src/item_prompt_common.c`** (die einzige Stelle, die Glyphen
erzeugt; PC- und PSX-Zeichner sowie die Schreibmaschinen-Zaehlung haengen alle daran).

1. **Schluessel-Test, datengetrieben** — neue statische Funktion direkt vor
   `re15_item_prompt_walk` (Zeile 66):

   ```c
   /* Traegt der AUSGELIEFERTE Name des Gegenstands den Glyphenlauf "Key"/"key"?
    * Namenstabelle 102 u16 @0x800C495C, Blob @0x800C4A28, 0x07-terminiert
    * (Reader FUN_80028840 @0x80028840). Glyphen: 'K' = 0x27, 'k' = 0x47,
    * 'e' = 0x41, 'y' = 0x55 (msg_common.c:175-199).
    * GEMESSEN: von 102 Namen tragen ihn genau 8 - 0x36/0x37/0x38/0x39/0x42/
    * 0x45/0x46/0x47 - und das sind ausschliesslich Schluessel. */
   static int re15_prompt_name_ist_schluessel(uint8_t item_id)
   {
       if ((int)item_id >= RE15_ITEM_PROMPT_NIDS) return 0;
       const unsigned char *nm =
           re15_item_prompt_name_blob + re15_item_prompt_name_off[item_id];
       for (; nm[0] != 0x07 && nm[1] != 0x07 && nm[2] != 0x07; nm++)
           if ((nm[0] == 0x27 || nm[0] == 0x47) && nm[1] == 0x41 && nm[2] == 0x55)
               return 1;
       return 0;
   }
   ```
   (Die Abbruchbedingung prueft alle drei Bytes, damit der 0x07-Terminator nie
   ueberlaufen wird — die Namen sind im Blob dicht gepackt.)

2. **Die Ersetzung im Lauf** — in `re15_item_prompt_walk`, innerhalb der
   `for`-Schleife (heute Zeile 74-104), als **erste** Fallunterscheidung nach dem
   Laden von `b`:

   ```c
   /* NUTZER-ENTSCHEIDUNG 2026-09-26 (KEIN Original-Abgleich: weder RE1.5 noch RE2
    * haben eine zweite Wortlaut-Variante - RE1.5 1 Treffer "Discard" @0x800C50AD im
    * ganzen Baum, RE2 1 Treffer @0x8009F179, 0 Treffer fuer eine Wegwerf-Zeile mit
    * "item"). Nur fuer Skript [6] und nur fuer Gegenstaende, deren AUSGELIEFERTER
    * Name kein "Key" traegt, wird der Glyphenlauf "key" @0x800C509F..@0x800C50A1
    * (47 41 55, Skript-Index +20..+22) durch "item" ersetzt. Die vier Ersatzglyphen
    * sind ebenfalls ausgeliefert: 45 50 41 49 @0x800C500E (Skript [1]
    * "...more items."). Fuer JEDEN Schluessel laeuft Skript [6] bit-gleich.  */
   if (idx == 6 && i == 20 && !re15_prompt_name_ist_schluessel(item_id)) {
       static const unsigned char ersatz[4] = { 0x45, 0x50, 0x41, 0x49 }; /* @0x800C500E */
       for (int k = 0; k < 4; k++) {
           if (cb && total < max_glyphs) cb(ctx, ersatz[k], attr, 0);
           total++;
       }
       i += 2;            /* 'k','e','y' = 3 Bytes; das for-i++ nimmt das dritte */
       continue;
   }
   ```
   `i` ist hier der Index **relativ zum Skriptanfang** (`s = blob + off[idx]`), der
   Vergleich `i == 20` trifft also exakt Blob-Index 0xC5 + 20 = 0xD9 = @0x800C509F.
   Ein Riegel soll diese 20 gegen die Daten pruefen (s. 5.4), nicht gegen den Text.

### 5.3 Was sich dadurch automatisch mitzieht (nichts weiter zu aendern)

* **Schreibmaschinen-Zaehlung**: `item_discard_common.c:278` ruft denselben Lauf mit
  demselben `s_item` → `s_reveal_total` wird fuer Nicht-Schluessel automatisch um 1
  groesser (28 statt 27 Glyphen in Zeile 1). Kein Sonderfall noetig.
* **Beide Zeichner**: `render_pc.c:2687` und `platform/psx/src/render.c:423` rufen
  denselben Lauf → kein Plattform-Zweig.
* **Riegel `tests/unit/r21_discard_wegwerfen.c:254`** vergleicht
  `re15_discard_reveal_total()` gegen `re15_item_prompt_walk(8, item, 0,0,0)` — beide
  Seiten sehen dieselbe Ersetzung, die Pruefung bleibt gueltig.
* **`gen/item_prompt_data.inc` wird NICHT angefasst** — kein Byte der ausgelieferten
  Tabelle aendert sich, `tools/gen_item_prompt_data.py` bleibt unveraendert.
* **Zeilenbreite**: +4 px (185 → 189 px, §2.5), 66 px innerhalb der breitesten
  ausgelieferten Zeile (289 px). Kein Layout-Eingriff noetig.

### 5.4 Der Riegel, der das belegt (neue Pruefungen in
`re15_port/tests/unit/r21_discard_wegwerfen.c`)

1. **Bit-Gleichheit fuer Schluessel**: fuer 0x36/0x37/0x38/0x46/0x47 muss die von
   `re15_item_prompt_walk(8, id, 200, sammeln, …)` erzeugte Glyphenfolge **Byte fuer
   Byte** gleich dem direkten Lauf ueber `re15_item_prompt_script_blob + 0xC5` sein
   (also gleich @0x800C508B). Keine Text-Zeichenkette im Test — der Vergleich laeuft
   gegen die Daten.
2. **Ersetzung fuer Nicht-Schluessel**: fuer 0x30/0x31/0x44 muss die Folge sich von (1)
   an **genau** den Indizes 20..22 unterscheiden, um **genau ein** Glyph laenger sein,
   und an 20..23 `45 50 41 49` tragen.
3. **Die Ankerpruefung gegen die Daten**: `blob[0xD9..0xDB] == {0x47,0x41,0x55}` und
   `blob[0x48..0x4B] == {0x45,0x50,0x41,0x49}` — bricht der Riegel, hat sich die
   ausgelieferte Tabelle verschoben und die 20 aus 5.2 stimmt nicht mehr.
4. **Kriterium-Zensus**: ueber alle 102 Namen liefert
   `re15_prompt_name_ist_schluessel` genau die 8 Ids 0x36,0x37,0x38,0x39,0x42,0x45,
   0x46,0x47 — und keine andere.
5. **Kein Kollateral**: fuer alle Prompt-Schluessel != 8 (Skripte [0]..[5],[7]) ist die
   Glyphenfolge fuer jeden der 102 Namen unveraendert gegenueber dem heutigen Stand.

---

## 6. Offen / nicht belegt

* **Nichts am Mechanismus ist offen** — Fundstelle, Kodierung, Zusammensetzung,
  RE2-Gegenprobe und das datengetriebene Kriterium sind alle roh nachgelesen.
* **Ausdruecklich KEIN Original-Beleg existiert fuer den neuen Wortlaut selbst.** Die
  Zeile *"You don't need this item any more. Discard it?"* steht in KEINER der beiden
  Binaerdateien. Sie ist eine Nutzer-Entscheidung und muss im Code und in der
  Commit-Message genau so gekennzeichnet werden — nicht als byte-true.
* **Nicht gemessen (weil in diesem Lauf nicht gebaut/gelaufen werden darf):** das
  gerenderte Bild mit der neuen Zeile. Die Breite ist aus der ausgelieferten
  Vorschubtabelle @0x800C4416 exakt gerechnet (189 px ab x=34), aber ein
  Bild-Nachweis (`RE15_FBDUMP` bzw. gdigrab nach dem Skill
  `re15-port-visual-verify`) steht aus und gehoert in den Umsetzungs-Lauf.
* **Bewusst NICHT geaendert:** die Gegenstaende 0x39 (Yellow Keycard), 0x42 (Key Disc)
  und 0x45 (Water Key) tragen "Key" im Namen, haben aber heute keine Wegwerf-Stelle
  (0x39 ist per Kollisions-Riegel E5 gestrichen, `gen/discard_sites.inc`). Sollte je
  eine Stelle dazukommen, waehlt das Kriterium fuer sie automatisch die
  Original-Zeile — was richtig ist.
* **Alternative, die ich verworfen habe und warum**: ein zweites Prompt-Skript als
  9. Tabelleneintrag. Verworfen mit Beleg (§5.1): der kleinste Tabellen-Offset 0x10
  @0x800C4FC6 IST das Tabellenende, aus dem der Port die 8 ableitet
  (@0x80027f30/@0x80027f44) — ein 9. Eintrag entkoppelt `item_prompt_data.inc` vom
  Auslieferungsstand.
