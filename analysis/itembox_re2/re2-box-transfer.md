# RE2s Item-Box: Transfer-Engine FUN_800703b8 — vollständige Ableitung

Quelle: `RE2_Quellcode_V2/FUN_800703b8.c` (Ghidra-Dekompilat der RE2-Leon-EXE,
SLUS-00748, t_addr 0x80010000), selbst durchgearbeitet 2026-08-30.

## 1. Das Speicher-Modell (direkt aus dem Code ablesbar)

| Was | Adresse | Form |
|---|---|---|
| **Inventar-Array** | `DAT_800d4a3c` + slot·4 | `{id @+0, qty @+1, size @+2}` (4. Byte ungenutzt) |
| **BOX-Array** | `DAT_800d4a68` + slot·4 | gleiches 4-Byte-Format |
| Inventar-Cursor | `DAT_800d5bfc` | Slot-Index |
| Inventar-Kapazität | `DAT_800d46ac` | Schleifen-Obergrenze der Munitions-Suche |
| **Box-Scroll-Stand** | `DAT_800d5c14` | siehe Auswahl-Formel |
| Ausgerüsteter Slot | `DAT_800d5bf8` | `0x80` = keiner (+ `DAT_800d5bfa` Nebenregister) |
| Bildschirm-Zustand | `DAT_800d5bf2` | wird bei Abweisung auf **5** gesetzt |
| Munitions-Kapazität je Id | `DAT_800a9e1c[id·8]` | Tabelle mit Schrittweite 8 |

**Die Auswahl im Ring:** `uVar15 = (DAT_800d5c14 + 2) & 0x3f`
⇒ der ausgewählte Box-Platz ist **Scroll + 2**, maskiert auf **64 Plätze**. Der Cursor
steht also FEST (zwei Zeilen unter dem Fensteranfang), gescrollt wird der Inhalt.
Box-Array-Bereich damit: `0x800d4a68 .. 0x800d4b68` (64 · 4 = 256 Byte).
Inventar davor: `0x800d4a3c`, Abstand zur Box = 0x2C = **11 Slots** (0..10, inkl.
RE2s Personal-Slot 10).

**Munitions-Ids:** Test `uVar10 - 0x14 < 0xc` ⇒ Ids **0x14..0x1F**.

### Kapazität 64 — doppelt belegt

1. **Auswahl-Maske:** `(DAT_800d5c14 + 2) & 0x3f` im Kopf von FUN_800703b8 ⇒ Ring über 64.
2. **Speicherbereich:** das nächste eigenständige Symbol hinter dem Box-Array ist
   `DAT_800d4b68` (im Ghidra-Dump als eigene Variable gelesen/geschrieben, u. a.
   `lbu v0,0x0(v1)=>DAT_800d4b68` @0x800692ec und `sb a0,0x0(v1)` @0x80069300).
   `0x800d4b68 − 0x800d4a68 = 0x100` = 256 Byte = **64 × 4 Byte**. Das Array endet
   also genau dort, wo das nächste Feld beginnt — die 64 ist keine Annahme.

## 2. Der Fall-Code (die zentrale Verzweigung)

```
cVar12 = (inv[cursor].size != 0) * 2  +  (box[pick].size != 0 ? 1 : 0)
```

| cVar12 | Bedeutung | Zweig |
|---|---|---|
| 0 | Inventar 1-zellig, Box 1-zellig/leer | einfacher Tausch |
| 1 | Inventar 1-zellig, **Box-Gegenstand 2-zellig** | Entnehmen einer breiten Waffe |
| 2 | **Inventar 2-zellig**, Box 1-zellig/leer | Ablegen einer breiten Waffe |
| 3 | beide 2-zellig | Tausch zweier breiter Waffen |

`size`: 0 = einzellig, 1 = breiter KOPF, 2 = breiter SCHWANZ, **3 = „breit, in der Box
liegend"** (die Box speichert eine 2-Zellen-Waffe auf EINEM Platz mit size 3).

## 3. Munitions-Sonderweg (läuft VOR der Fallunterscheidung)

Nur wenn der Box-Gegenstand Munition ist (Id 0x14..0x1F):

1. **Gleiche Id wie im Inventar-Slot:** Kapazität `k = DAT_800a9e1c[id·8]`.
   - Inventar-Menge **== k** (schon voll) ⇒ **MENGEN-TAUSCH**: die beiden Mengen
     werden vertauscht, fertig.
   - Summe **> k** ⇒ Inventar wird auf `k` aufgefüllt, in der Box bleibt der Rest
     (`summe − k`).
   - Sonst ⇒ **ganzer Stapel wandert ins Inventar**, der Box-Platz wird komplett
     geleert (id/qty/size = 0) und sein Symbol-Zwischenspeicher aktualisiert.
2. **Inventar-Slot ist LEER:** das Spiel durchsucht das GANZE Inventar (bis
   `DAT_800d46ac`) nach einem Platz mit derselben Id, auf den die Menge noch passt
   (`qty <= k − box_qty`) — und legt sie dort dazu, statt auf dem leeren Platz.

## 4. Die vier Zweige

**Fall 0 — einfacher Tausch:**
Auto-Unequip (falls der Cursor-Slot die ausgerüstete Waffe ist ⇒ `DAT_800d5bf8 = 0x80`),
dann `uVar10 = FUN_80069668(0)`; ist der erste freie Platz **kleiner** als der Cursor,
landet der Gegenstand **dort** statt am Cursor (`min(cursor, erster-freier)`).
Anschließend werden die drei Bytes schlicht getauscht.

**Fall 1 — breite Waffe aus der Box holen:**
`FUN_80069668(1)` prüft, ob zwei Zellen frei sind.
**ABWEISUNG**, wenn `(ret == 1 && Cursor-Slot leer) || ret == 0` ⇒ Meldung
`FUN_8002fe38(0xaf0010, 0xe400, 8, 0)` und `DAT_800d5bf2 = 5` (Melde-Zustand).
Sonst: Cursor-Slot leeren, **verdichten** (`FUN_80069714`), `FUN_800698b4(1)`, und die
Waffe landet auf **Slot 0 + 1** (`DAT_800d4a3c`/`DAT_800d4a40`, size 1 und 2) — also
vorn, nicht am Cursor. Der Inventar-Gegenstand wandert in den Box-Platz.

**Fall 2 — breite Waffe ablegen:**
Beide belegten Zellen werden geleert und **zweimal** verdichtet (`FUN_80069714` doppelt).
Dann `FUN_80069668(0)` = erster freier Platz, dorthin der Box-Gegenstand; die Waffe geht
mit **size = 3** in die Box.

**Fall 3 — breit gegen breit:**
Je nach `size` des Cursor-Slots (1 = Kopf, 2 = Schwanz) wird der Partner-Slot bestimmt;
beide Zellen bekommen den Box-Gegenstand als Kopf/Schwanz, die Box bekommt die Waffe mit
**size = 3**.

## 5. Hilfsfunktionen (Rollen aus dem Aufrufmuster)

| Adresse | Rolle |
|---|---|
| `FUN_80069668(mode)` | erster freier Platz bzw. Prüfung auf zwei zusammenhängende Zellen (mode 1) |
| `FUN_80069714()` | **Verdichtung** des Inventars (Lücken schließen) |
| `FUN_800698b4(1)` | Vorschub/Nachziehen nach dem Freimachen (Fall 1) |
| `FUN_80069c40(0x10, id, ptr)` | Symbol-Zwischenspeicher der **Box**-Seite auffrischen |
| `FUN_80069bb4(slot, ptr)` | Symbol-Zwischenspeicher der **Inventar**-Seite auffrischen |
| `FUN_8002fe38(0xaf0010,0xe400,8,0)` | Abweisungs-Meldung („kein Platz") |

Symbol-Zwischenspeicher: `DAT_8019c000 + id · 0x4b0` (0x4b0 = 1200 Byte je Symbol);
der Zuschlag `+ 0x56` adressiert die zweite Hälfte eines breiten Symbols.

## 6. Abgleich mit unserer bisherigen Portierung

Unsere Hybrid-Fassung (`analysis/itembox_re2/backup_hybrid_v1/`) bildet **diese Engine
bereits korrekt ab** (Mengen-Tausch, Stapel-Umleitung, min(cursor,frei)-Landung,
2-Zellen-Abweisung, Doppel-Verdichtung, stilles Auto-Ablegen, size-3-Ablage).

**Was fehlt, ist NICHT die Engine, sondern das Umfeld:**

| Punkt | RE2 | unsere Hybrid-Fassung |
|---|---|---|
| Kapazität | **64 Plätze** (Maske 0x3f) | 32 (4 Seiten × 8) |
| Auswahl | fester Cursor, **Inhalt scrollt** (`scroll+2`) | frei bewegter 2×4-Gitter-Cursor |
| Blättern | Scroll ±1 / Schulter ±5 | L1/R1 blättert ganze Seiten |
| Darstellung | Liste mit Namen | Symbol-Gitter |
| Inventar-Kapazität | 11 Plätze inkl. Personal-Slot | 10, kein Personal-Slot (RE1.5) |

Der Umbau betrifft also Speicher-Struktur, Bedien-Modell und Zeichner — die
Transfer-Regeln können weitgehend übernommen werden.
