# Spur C — Generator ROOM11F0/11F1: freier Cursor, Endsperre bis 80, zwei gruene Lampen

Stufe: ERMITTLUNG + BAUPLAN (kein Port-Code). Arbeitsbaum `.claude/worktrees/r34n_generator`, Zweig `r34n/generator`.

Stand: alle Abschnitte gefuellt (Ermittlung + Bauplan), 2026-09-30.

## 0 Kurzfassung

* **Ist (gemessen, echte exe):** nach dem ersten Schalter steht der Auswahl-Cursor 49 Bilder
  (F532..F580) still, obwohl UNTEN gehalten wird — Zeigerfahrt 0 -> 20 plus 30 Ruhebilder
  (`C_belege/ist1_cursor_spur.txt`, `ist1_streifen.png`). Ursache: die Runde-31-Sperre
  `re15_panel_zeiger_sperrt()` greift nach JEDER Schalteraenderung.
* **Mechanismus:** RE1.5 sperrt die Eingabe beim Raetsel NIE (`22 02 07 01` nur in sub18 @0x01736 und
  sub19 @0x017B8); RE2 sperrt das ganze sub04 (@0x01110..@0x01818), hat aber keinen freien Cursor.
  Die Endabnahme bleibt RE2: Zeiger auf 80 (@0x01708), 30 Bilder (@0x0171C), dann "OK".
  Nur die Maske 0x155 bringt den Zeiger auf 80 (alle 1024 nachgezaehlt) = RE1.5-Loesung.
* **Bauplan 1 (freie Bewegung, Endsperre):** `sperrt()` = "Raetsel aktiv, nicht geloest, Maske ==
  0x155" (live). Keine Aenderung an game_step_common.c/scd_vm.c.
* **Spalten (aus den Bytes):** Schalter 1..5 = linke Spalte (Loesung 1,3,5 = "die 3"), 6..10 = rechte
  (7,9 = "die 2"). Lampe oben = `(m & 0x01F) == 0x015`, unten = `(m & 0x3E0) == 0x140`, je Bild, plus
  "dauerhaft an nach 4:238" (RE2 sub03 @0x00F3C).
* **Kunst gefunden:** RE1.5 hat keine (15 BG-Cuts, Props, ESP geprueft). RE2 ROOM2130 = dasselbe Panel:
  ESP 0x16 (`esp16.tim` = RDT @0x0E398), Zelle 3/4 = quadratische Gitterlampe, **CLUT-Zeile 2 = gruen**
  (RE2 benutzt sie @0x017A8/@0x017B8 fuer "Strom da"), additiv (0xBA03 -> Prim 0x2E, TPAGE|0x20),
  Wechsel 3/4 je Bild. Groesse in RE2 nachgerechnet (25,8 px), auf RE1.5 22 px uebertragen.
  Vorschau am Original-Hintergrund: `C_belege/vorschau_k22_z3.png`.
* **Neu:** `platform/pc/src/panel_lampen_pc.c`, `shared_assets/RE2/LAMPE2130.TIM`, 1 Haken in main.c,
  Riegel `unit_r34n_c_generator`; `unit_r31_generator` Teil D umschreiben. Vertragsressourcen
  (Bits 69/70, AOT 40/41, Msg 28/29) nicht gebraucht.

## 1 Nutzerwortlaut + Lesart

### 1.1 Wortlaut (AUFTRAG.md, dritter und vierter Punkt)

> Bei den Generator in ROOM 11F0 möchte ich nicht, das nach den Klick eines Schalters, der Cursor
> eingefroren bleibt, bis die Anzeige dort steht wo sie hin soll, sondern man soll sich frei bewegen
> können Ausser ganz am Ende - ganz am Ende, bevor das "OK" kommt und die Lichter angehen, dann soll
> der Cursor zunächst auf den finalen Wert - also die 80 gehen.

> Dann möchte ich, das in ROOM 11F0 beim Generator Rätsel die Lichter grün aufleuchten - siehe
> lights.bmp - Das obere Licht soll angehen, wenn links die 3 Schalter korrekt betätigt sind. Das
> untere Licht soll angehen, wenn die 2 Schalter rechts korrekt betätigt sind. Sobald eines der
> jeweiligen Schalter der jeweiligen Seite nicht mehr korrekt ist, dann soll das jeweilige Licht -
> (falscher Schalter links - das obere, falscher Schalter rechts - das untere) wieder aus gehen.

Referenzbild `lights.bmp` (Hauptbaum, 320x240, = Hintergrund Cut 10 mit zwei roten Pfeilen): oberer
Pfeil auf die Lampe x 213..231 / y 67..85, unterer Pfeil auf die Lampe x 213..231 / y 126..143
(Rahmen selbst am Original-Hintergrund `ROOM11F10.bmp` vermessen, Abschnitt 3.6). Gegenprobe
selbst gerechnet: lights.bmp = ROOM11F10.bmp + genau 590 rote Pfeilpixel, sonst KEIN Pixel mit
Abweichung > 60 — der Nutzer hat also den Original-Hintergrund markiert. Pfeil oben: x 198..218 /
y 42..70 (endet am oberen Lampenrahmen), Pfeil unten: x 196..219 / y 141..172 (endet am unteren).

### 1.2 Mehrdeutigkeiten und die gewaehlte Lesart

| Wort | Lesart | Begruendung |
|---|---|---|
| "der Cursor eingefroren bleibt" | der gruen/gelbe AUSWAHL-Cursor (obj 0x00, Obj_model_set @0x00E54) | Das ist das Ding, das heute nach jedem Schalter stehen bleibt: die Panel-Sperre maskiert die SCD-Padwoerter, aus denen sub01 die Cursor-Tasten liest (Abschnitt 2). Der Zeiger dagegen FAEHRT in dieser Zeit. |
| "dann soll der Cursor zunaechst auf den finalen Wert - also die 80 gehen" | der rote LEISTUNGS-ZEIGER auf der Skala | Nur der Zeiger kann "auf die 80 gehen" (die 80 steht auf der Skala, nicht im Schalterfeld). Der Nutzer nennt den Zeiger schon in Runde 26 "Cursor": "ich moechte in ROOM 11F0 so einen roten Cursor ... Nach der Eingabe der korrekten Position muss der Cursor auf 80 stehen" (`analysis/befunde_2026-09-26/raum11f0-raetsel-cursor.md` §1). |
| "Ausser ganz am Ende" | Sperre NUR, sobald die Schalterstellung die Loesung ist (Maske 0x155 = Schalter 1,3,5,7,9) und das Raetsel noch nicht abgenommen ist | "Ende" = der Schalterzustand, der zum "OK" fuehrt. Das ist genau die RE1.5-Loesungspruefung @0x012BE..0x012E2 und zugleich der einzige Zustand, in dem der Zeiger auf 80 zielt (Nutzer-Gewichte, Abschnitt 3.3). |
| "bevor das OK kommt und die Lichter angehen" | "OK" = Meldung 2 "Power supply OK." (sub18 @0x01742); "die Lichter" = das RAUMLICHT nach der Abnahme (sub18 Cut 0x0D-Flackern @0x0174A..0x0176C, Cut 0x0E) | Der Satz beschreibt den HEUTIGEN Ablauf ("bevor ... die Lichter angehen"); die gruenen Lampen gibt es heute noch nicht, sie werden erst im naechsten Punkt ("Dann moechte ich ...") neu gewuenscht. Runde 31 benutzte dieselbe Formulierung fuer denselben Ablauf: "bevor du mit ok das abnimmst, das Licht anschaltest etc." |
| "die Lichter gruen aufleuchten ... Das obere Licht ... Das untere Licht" | die ZWEI Lampen rechts neben dem Schalterfeld (Pfeile in lights.bmp), NICHT die zehn kleinen Lampen links neben jedem Hebel | Die Pfeile zeigen genau auf diese zwei Rahmen. |
| "links die 3 Schalter" / "die 2 Schalter rechts" | linke Spalte = Schalter 1..5 (davon 1,3,5 EIN), rechte Spalte = Schalter 6..10 (davon 7,9 EIN) | Aus den Bytes belegt (Abschnitt 3.4): obj 0x02..0x06 liegen auf Welt-x -25975 (Bild x 64..84), obj 0x07..0x0B auf -18775 (Bild x 155..169); die Loesung @0x012BE..0x012E2 verlangt links genau 3, rechts genau 2 EIN-Schalter. Die Zaehlung des Nutzers (3/2) bestaetigt die Spaltenzuordnung. |
| "wenn links die 3 Schalter korrekt betaetigt sind" | obere Lampe AN genau dann, wenn ALLE FUENF Schalter der linken Spalte in Loesungsstellung sind (1,3,5 EIN UND 2,4 AUS) | Der Folgesatz "Sobald EINES der jeweiligen Schalter der jeweiligen Seite nicht mehr korrekt ist ... wieder aus" macht jeden Schalter der Seite zum Pruefling — ein zusaetzlich eingeschalteter Schalter 2 ist "nicht korrekt". Nur so gilt auch: beide Lampen gruen <=> Maske 0x155 <=> RE1.5-Loesung <=> Zeigerziel 80. Die laxere Lesart (nur 1,3,5 EIN) wuerde zwei gruene Lampen bei falscher Stellung zeigen. |
| "angehen, wenn ... betaetigt sind" / "sobald ... nicht mehr korrekt" | Lampenzustand = Funktion der Schalterbits Bank 5 / 13..22 in JEDEM Bild; Wechsel im Bild, in dem das Schalterbit wechselt (Ende der 16-Bild-Kippung, sub06 @0x01340) | "sobald" verlangt einen lebenden Zustand, kein Einrasten. RE2 zuendet seine Schalterlampen zwar erst NACH der Zeigerfahrt (@0x01294 hinter ewhile @0x01290), dort bedeutet die Lampe aber "Schalter N bearbeitet" und geht nie wieder aus (nur Sammel-Kill @0x01180..0x01188 beim Start von sub04) — fuer "Seite korrekt / wieder aus" gibt es in RE2 kein Vorbild; die Zeitlinie folgt deshalb dem Schalterbit. |
| "gruen" | gruene Palette der RE2-Lampenkunst desselben Bedienfelds (RE2 ROOM2130 ESP 0x16, CLUT-Zeile 2) | Abschnitt 3.7: RE2 schaltet mit genau dieser Palette die zwei "Strom da"-Lampen von rot auf gruen (@0x017A8/@0x017B8, Unterindex 0x10). |

### 1.3 Einordnung Beta -> Retail

* **Freie Cursor-Bewegung waehrend der Zeigerfahrt ist RE1.5-Original:** sub01..sub15 von ROOM11F0
  setzen die Pad-Sperre (Set(2,7,1)) NIE; nur sub18 (Abnahme) tut es (@0x01736), Abschnitt 3.2.
  Die heutige Sperre nach jedem Schalter ist die RE2-Angleichung aus Runde 31 (RE2 haelt Bank 2
  Bit 7 ueber das ganze sub04 @0x01110..@0x01818). Der Nutzer nimmt sie fuer die Zwischenschalter
  ausdruecklich zurueck -> NUTZER-VORGABE, die mit dem RE1.5-Stand zusammenfaellt.
* **Die Endsperre bis 80 + Stillstand bleibt RE2** (RE1.5 hat keinen Zeiger = unfertig):
  ewhile @0x01708 -> sleep 30 @0x0171C/0x0171D -> cmp(var5==80) @0x01752, Sperre ueber die ganze
  Strecke. Neu ist nur, dass sie erst mit dem Loesungszustand beginnt.
* **Die gruenen Lampen haben in RE1.5 kein Gegenstueck** (Abschnitt 3.6: keine Lampenkunst in
  ROOM11F0, weder Prop noch ESP noch Hintergrund) -> NUTZER-VORGABE fuer das Ob/Wann, RE2-Kunst
  und RE2-Zeichenart fuer das Wie (Abschnitt 3.7), PORT-WAHL nur fuer Lage/Groesse (gemessen).

## 2 Ist-Zustand im Port (gemessen)

Stand: Zweig `r34n/generator` = master cf0e68ba, gebaut im Baum mit `local_build.sh configure` +
`build` (`=== LOCAL-BUILD-OK (build)`, Log `build/r34n_c/build.log`). Gemessen an einer Kopie der exe
(`re15_port/build/platform/pc/re15_r34nc.exe`, damit kein fremder Bau sie beendet).

### 2.1 Code-Stand (gelesen)

| Stelle | Was sie heute tut |
|---|---|
| `engine/src/panel_zeiger_common.c:107-191` `re15_panel_zeiger_tick` | Ziel = max(0, Summe Nutzergewichte) der Maske Bank 5 / 13..22 (ab 4:238 fest 80); Wert folgt 1 Punkt/Bild; `s_ruhe` = Bilder seit letzter Bewegung/Maskenaenderung (gesaettigt 30); Bestaetigungston an der Flanke von 4:238 |
| `panel_zeiger_common.c:222-243` `re15_panel_zeiger_sperrt` | **sperrt nach JEDER Maskenaenderung**, solange der Zeiger faehrt (`s_wert != ziel`), bis 30 Ruhebilder um sind, und am Ende, solange der Zeiger auf 80 steht und die Abnahme noch nicht lief |
| `game_step_common.c:1139-1150` (Haken "RE2-ANGLEICHUNG PANEL-SPERRE") | `if ((g_re15_pauseflags & RE15_PAUSE_PAD) \|\| panel_sperre)` -> `g_scd_pad_held/edge &= 0xf000` — genau die Woerter, aus denen `Sce_key_ck` (op_sce_key_ck, scd_vm.c) die Cursor- und Schalttasten liest |
| `scd_vm.c:1109-1112` (op_evt_exec) | haelt @0x012E6 `04 ff 18 12` (Evt_exec sub18) + @0x012EA zurueck (IF_FALSE), bis `re15_panel_zeiger_abnahme_frei()` = Zeiger 80, Ziel 80, 30 Ruhebilder, Maske unveraendert |
| `platform/pc/main.c:5274-5296` | roter Zeiger als 5 `re15_render_tile`-Zeilen direkt nach `re15_bg_blit`, nur ROOM11F0/11F1 Cut 10 |
| Lampen | **gibt es nicht** — der Port zeichnet an x 213..231 nur den Hintergrund (s. 2.3) |

### 2.2 Messung: der Cursor friert nach dem ersten Schalter 49 Bilder ein

Lauf `re15_port/tools/r34n_c/lauf.sh build/r34n_c/ist1 640 "U0.3,W0.2,A0.07,D3,W2" "505-640/3:fd_"`
(DEBUG_JUMP 11F0, FIRE_AOT Slot 1 = Panel, Meldungen blaettern, "Ja" -> F500; Cursor von der
Startzelle (Schalter 8) nach oben auf Schalter 7, Quadrat, dann **3 s UNTEN GEHALTEN** ab F517).
Auswertung `re15_port/tools/r34n_c/cursor_spur.py` (Cursorlage aus den hellgruenen Eckwinkeln im
gerenderten Bild, Zeigerlage aus dem roten Pfeil, dazu RE15_PANEL_LOG), voll in
`C_belege/ist1_cursor_spur.txt`, Auszug:

```
    F   cur_x   cur_y   zg_y sperr maske wert  ruhe
  517   160.2    95.7  177.3     0   000    0    30   <- UNTEN gehalten ab hier
  529   160.2   128.0  177.3     0   000    0    30   <- Cursor wandert (2,67 px/Bild), Hebel kippt noch
  532   160.2   136.0  177.3     1   040    1     0   <- Schalterbit 19 steht (sub12 @0x014C0) -> SPERRE
  550   160.2   136.0  155.3     1   040   19     0      Zeiger faehrt, Cursor steht trotz UNTEN
  553   160.2   136.0  152.3     1   040   20     2      Zeiger am Ziel (F551), 30 Ruhebilder laufen
  580   160.2   136.0  152.3     1   040   20    29      Cursor steht noch immer
  583   160.2   137.4  152.3     0   040   20    30   <- Sperre weg (F581), Cursor faehrt weiter
  595   160.2   169.4  152.3     0   040   20    30
```

Panel-Log (Bild fuer Bild): `panelsperre=1` von **F532 bis F580 = 49 Bilder** (20 Bilder Fahrt 0->20,
dann 30 Ruhebilder bis `ruhe=30`), obwohl UNTEN die ganze Zeit anliegt. Streifen
`C_belege/ist1_streifen.png` (F529/532/556/580/583/595, Ausschnitt x 140..290 / y 40..210): Cursor
steht in F532..F580 an derselben Stelle, der rote Zeiger steigt dabei von der 0 auf die 20.
Das ist genau das Nutzer-Symptom "nach den Klick eines Schalters bleibt der Cursor eingefroren, bis
die Anzeige dort steht wo sie hin soll". Beim Zeigerweg 50 -> 80 (letzter Schalter) waeren es 30 + 30.

### 2.3 Lampenbereich im Port-Bild

Gruene Glas-Bbox im Port-Framedump F517 (auf 320x240 verkleinert): x 216..229 / y 72..79 und
x 216..229 / y 131..140 — **identisch** mit dem Original-Hintergrund (3.6); mittlere Kanalabweichung
Port-BG gegen ROOM11F10.bmp im Lampenbereich x 205..239 / y 60..149: 0,66. Die am Original
gemessenen Lampenmitten gelten also 1:1 fuer das Port-Bild. Heute bleiben beide Lampen dunkel.

### 2.4 Variante ROOM11F1

`RE15_PANEL_IST_RAUM(id)` = 0x11F0 oder 0x11F1 (re15_panel_zeiger.h) — Zeiger, Sperre und Abnahme-Halt
gelten in beiden Raeumen (byte-identische RDTs). Live nur 11F0 gefahren: RE15_DEBUG_JUMP nimmt das
Varianten-Nibble nicht mit (Runde 31, debug_menu_common.c:318).

### 2.5 Zusatzmessung: zwei ueberlappende Kippungen, Zielwechsel mitten in der Fahrt

Lauf `lauf.sh build/r34n_c/parallel 600 "U0.3,W0.2,A0.07,D0.35,A0.07,W3"`: Quadrat auf Schalter 7
(F515), sofort 11 Bilder UNTEN in die Zelle von Schalter 8, Quadrat (F528) — die zweite Kippung
beginnt, BEVOR das Bit der ersten steht. Log (`C_belege/parallel_kippung_F526-548.txt`):
`F532 maske=040 ziel=20 wert=1 panelsperre=1` ... `F544 wert=13` / `F545 maske=0C0 ziel=0 wert=12`
/ `F546 wert=11`. Belegt: (a) eine vor der Sperre begonnene Kippung laeuft trotz Sperre zu Ende
(Ereignis-Thread, haengt nicht am Pad) und aendert die Maske 13 Bilder nach der Sperre; (b) wechselt
das Ziel mitten in der Fahrt, dreht der Zeiger vom AKTUELLEN Wert aus (13 -> 12 -> 11 ...), ohne Sprung.
Beides traegt die Sonderfaelle in 4.3.

## 3 Original-/RE2-Mechanismus (Adressen, Bytes, Instruktionen)

Alle RE1.5-Datei-Offsets: `re15_port/shared_assets/PSX/STAGE1/ROOM11F0.RDT` (152588 B; ROOM11F1.RDT
byte-identisch, `cmp` in dieser Runde erneut: keine Abweichung). Eigener Voll-Dump: `scd_dump_room.py` -> `build/r34n_c/room11f0_scd.txt`
(694 Zeilen, main00 + 20 Subs). Alle RE2-Datei-Offsets: `info/re2leon/PL0/RDT/ROOM2130.RDT` (139448 B);
die tragenden RE2-Bytes habe ich in dieser Runde selbst nachgelesen (12 Stellen, alle OK:
@0x00F40 `21 04 3c 01`, @0x01110 `22 02 07 01`, @0x011E0 `0f 06 36 00`, @0x01216 `02`, @0x01294
`64 01 16 02`, @0x012A4 `09 0a 1e 00`, @0x01708 `10 00`, @0x0171C `09 0a 1e 00`, @0x01752
`23 00 05 00 50 00`, @0x01758 `2b 00 07 00 ff ff`, @0x0175E `22 04 3c 01`, @0x01818 `22 02 07 00`).

### 3.1 RE1.5-Ablauf des Raetsels (selbst gedumpt)

| Stelle | Bytes | Wirkung |
|---|---|---|
| sub16 @0x015C0 | `29 0a` | Cut_chg 0x0A = Raetselbuehne (Cut 10) |
| sub16 @0x015C2..0x015F2 | 13x `22 05 xx 01` | Bank 5 Bits 0..12 = Raetsel aktiv + 11 Zellen scharf |
| sub01 @0x0108C..0x010EA | `06 00 14 00` / `21 05 00 01` / `51 01 {01,04,02,08} 00` / `04 ff 18 {02..05}` | je Bild: Cursor-Tasten -> sub02..05 |
| sub02..05 @0x012F6/0x01302/0x0130E/0x0131A | `2f 02 c8 00` / `2f 02 38 ff` / `2f 00 c8 00` / `2f 00 38 ff` | Cursor-Prop +-200 je Bild auf z bzw. x, OHNE Raster |
| sub01 @0x010EC ff. (10x) | `21 05 nn 01` / `2e 03 00` / `3e 00 0f 00 kk 00` / `51 01 40 00` / `04 ff 18 ss` | Zelle scharf + Cursor ueber AOT-Slot kk + Aktionstaste -> Schalter-Sub ss |
| sub06 @0x01322 | `22 05 01 00` | NUR die eigene Zelle sperren (Bit 1) |
| sub06 @0x01332..0x0133E | `2f 05 40 00` / `0d 00 04 00 10 00` / `30` / `02` / `0e 00` | Hebel 16 Bilder je +64 um Achse 5 kippen |
| sub06 @0x01340 | `22 05 0d 01` | ERST DANN das Schalterbit (Bank 5 Bit 13) |
| sub06 @0x0135C | `22 05 01 01` | Zelle wieder frei |
| sub01 @0x012A4..0x012B0 | `21 05 0c 01` / `22 02 00 01` / `22 02 02 01` | Spieler- und KI-Pause (nicht Pad!) |
| sub01 @0x012B6..0x012EA | `06 00 36 00` / `21 04 ee 00` / 10x `21 05 0d..16 01/00` / `04 ff 18 12` / `22 04 ee 01` | Loesungskette je Bild: nicht geloest + 1,0,1,0,1,0,1,0,1,0 -> Evt_exec(sub18), Set(4,238,1) |
| sub18 @0x016F6 / @0x016FA..0x0172A | `22 04 f3 01` / 13x `22 05 xx 00` | Reservestrom global; Bank 5 Bits 0..12 aus — **Bits 13..22 (Schalter) BLEIBEN stehen** |
| sub18 @0x01736 / @0x0173A / @0x01742 | `22 02 07 01` / `29 08` / `2b 02 ff ff` | Pad-Sperre, Cut 8, "Power supply OK." |
| sub18 @0x01748..0x01772 | `29 0d` / `56 00 02 07 00 00` / 4x `57 ..` + Sleep / `29 0e` / `09 0a 28 00` | Raumlicht-Flackern (die "Lichter" aus Punkt 3) |
| sub18 @0x01776 / @0x01784 | `46 01 01 31 02 00 ff ff 00 00` / `22 02 07 00` | Panel-AOT nur noch Text; Pad-Sperre aus |

### 3.2 Pad-Sperre: RE1.5 sperrt beim Spielen NIE, RE2 sperrt das ganze Schalten

* **RE1.5, Zensus im eigenen Dump:** `Set(2,7,*)` = `22 02 07 xx` steht in ROOM11F0 genau viermal:
  @0x01736 (=1) und @0x01784 (=0) in sub18 (Abnahme), @0x017B8 (=1) und @0x0180A (=0) in sub19
  (Schublade, Cut 0x0B). In sub01..sub17 — also waehrend der ganzen Raetselbedienung — **nie**.
  Wirkung des Bits (in dieser Runde selbst nachdisassembliert, re15_disasm.py): FUN_80030444
  @0x800304f4 `lw v0,-13760(v0)` (g_pauseflags) / @0x800304f8 `lui v1,0x100` / @0x800304fc `and` /
  @0x80030500 `beq v0,zero,..` / @0x8003050c `lw v0,-14488(v0)` (0x800ac768) / @0x80030514
  `andi v0,v0,0xf000` / @0x8003051c `sw v0,-14488(at)` = virtuelles Held-Wort; die Cursor-/Schalt-
  tasten 0x01/0x02/0x04/0x08/0x40 fallen damit weg.
* **RE2 ROOM2130 sub04:** `22 02 07 01` @0x01110 ist der ERSTE Befehl, `22 02 07 00` @0x01818 der
  vorletzte. Leser (selbst nachdisassembliert, re2_disasm.py) @0x800391F8 `lw v1,-1060(v1)`
  (0x800cfbdc) / @0x800391FC `lui a0,0x100` / @0x80039200 `and` / @0x80039204 `beq` / @0x80039210
  `lw v0,-7412(v0)` (0x800ce30c) / @0x8003921C `andi v0,v0,0x3c00` / @0x80039224 `sw`. RE2 hat aber auch KEINEN freien Cursor:
  es fragt die fuenf Schalter nacheinander per Ja/Nein ab (@0x0118A `2b 00 01 00 ff ef` ...).
* **Folgerung:** Die freie Bewegung waehrend der Zeigerfahrt ist das, was RE1.5 selbst tut. Die
  heutige Port-Sperre nach JEDEM Schalter (Runde 31) uebertraegt RE2s Sperre auf ein Bedienmodell,
  das RE2 gar nicht hat. Die Nachfuehrfahrt des Zeigers (1 Punkt/Bild) braucht keine Sperre, weil
  der Port-Zeiger zustandsbasiert ist (Ziel = Funktion der Schalterbits, jedes Bild neu).

### 3.3 Zeiger und Endabnahme (RE2, unveraendert gueltig)

| RE2 ROOM2130 | Bytes | Wirkung |
|---|---|---|
| @0x011E0 .. @0x01218 | `0f 06 36 00` .. `02` .. `10 00` | Nachfuehrschleife, 1 Wertpunkt je Bild (evt_next @0x01216) |
| @0x01708 | `10 00` | ewhile nach dem letzten Schalter: Zeiger steht |
| @0x0171C/@0x0171D | `09` / `0a 1e 00` | sleep 0x1E = 30 Bilder Stillstand (selbst nachgeprueft: Tabelle @0x800a74c8[9] = 0x800539DC, [10] = 0x80053A24; sleeping zaehlt @0x80053a50/0x80053a54 herunter, rueckt bei 0 um 3 vor @0x80053a6c und gibt IMMER das Bild ab: @0x80053a84 `jr ra` / @0x80053a88 `addiu v0,zero,2` -> Pruefung im Bild k+31) |
| @0x01752 | `23 00 05 00 50 00` | cmp(var5 == 80) |
| @0x01758/@0x0175E/@0x01762 | `2b 00 07 00 ff ff` / `22 04 3c 01` / `36 02 0c 01 ..` | "Power supply OK." + Geloest-Flag + Ton im selben Skriptbild (message_on = Tabelle @0x800a7574 -> 0x80054A8C, Rueckgabe @0x80054ad4 `addiu v0,zero,1` = weiter; selbst nachgeprueft) |

Port-Wertbildung = NUTZER-VORGABE 2026-09-26 (Gewichte {+20,-20,-10,-30,+20,-40,+20,-50,+30,-60},
Anzeige max(0, Summe)). **Selbst nachgezaehlt ueber alle 1024 Masken:** genau EINE Maske ergibt die
Anzeige 80, naemlich 0x155 (= Schalter 1,3,5,7,9 = die RE1.5-Loesung @0x012BE..0x012E2);
erreichbare Anzeigewerte {0,10,...,90}, Maximum 90. Daraus folgt:
* "Endzustand" ist eindeutig ueber die Maske definierbar: `maske == 0x155` <=> Zeigerziel 80
  <=> RE1.5-Kette zieht (solange 4:238 = 0).
* Ein Zeiger, der die 80 nur DURCHFAEHRT, tut das nur zwischen 90 und einem Wert < 80 — die Maske
  ist dann nie 0x155, die RE1.5-Kette scheitert am ersten falschen Ck, und die Abnahme-Freigabe
  des Ports verlangt zusaetzlich `ziel == 80` und 30 Stillstandsbilder (panel_zeiger_common.c:202-208).

### 3.4 Welcher Schalter liegt wo (Bytes -> Bild)

Selbst aus dem Dump gelesen (sub00 Aot_set/Obj_model_set, sub01 Member_cmp/Evt_exec, sub06..15 Set);
reproduzierbar aus den Rohbytes mit `re15_port/tools/r34n_c/zuordnung.py <ROOM11F0|ROOM11F1>`
(Ausgabe `C_belege/zuordnung_11F0.txt` / `_11F1.txt`, beide "Ergebnis: OK", Zustandsbit-Set je Sub
@0x01340/0x01380/.../0x01580):

| Schalter | Bank-5-Bit | Loesung | Zelle (AOT-Slot, Rechteck x/z) | Member_cmp @ | Sub | obj | Prop-Lage (x, z) | Spalte / Zeile |
|---|---|---|---|---|---|---|---|---|
| 1 | 13 | EIN | 2 (-27300, 25650) | 0x010FC `== 2` | sub06 | 0x02 | (-25975, 26000) | links / 1 |
| 2 | 14 | aus | 3 (-27300, 23450) | 0x01124 `== 3` | sub07 | 0x03 | (-25975, 24000) | links / 2 |
| 3 | 15 | EIN | 4 (-27300, 21250) | 0x0114C `== 4` | sub08 | 0x04 | (-25975, 22000) | links / 3 |
| 4 | 16 | aus | 5 (-27300, 19000) | 0x01174 `== 5` | sub09 | 0x05 | (-25975, 19800) | links / 4 |
| 5 | 17 | EIN | 6 (-27300, 16800) | 0x0119C `== 6` | sub10 | 0x06 | (-25975, 17800) | links / 5 |
| 6 | 18 | aus | 7 (-19700, 25650) | 0x011C4 `== 7` | sub11 | 0x07 | (-18775, 26000) | rechts / 1 |
| 7 | 19 | EIN | 8 (-19700, 23450) | 0x011EC `== 8` | sub12 | 0x08 | (-18775, 24000) | rechts / 2 |
| 8 | 20 | aus | 9 (-19700, 21250) | 0x01214 `== 9` | sub13 | 0x09 | (-18775, 22000) | rechts / 3 |
| 9 | 21 | EIN | 10 (-19700, 19000) | 0x0123C `== 10` | sub14 | 0x0A | (-18775, 19800) | rechts / 4 |
| 10 | 22 | aus | 11 (-19700, 16800) | 0x01264 `== 11` | sub15 | 0x0B | (-18775, 17800) | rechts / 5 |

Zellen-Offsets: Aot_set @0x00D78..0x00E2C (`2c 02 05 44` .. `2c 0b 05 44`), Props @0x00E76..0x00FA8.
Bildlage (Cut 10 ist eine Nadir-Kamera, RDT @0x01A0): Welt-x -25975 -> Bild x 64..84 (linke
Hebelspalte), -18775 -> x 155..169 (rechte); z 26000 -> y 70..78 (oben) ... z 17800 -> y 171..180
(Runde 26 am Port-Framedump vermessen). **Selbst nachgemessen** am eigenen Framedump F505 (Lauf 2.2,
Differenz gegen ROOM11F10.bmp > 40): 3D-Hebel links x 64..84, rechts x 155..172, Zeilen y 71..78 /
96..102 / 121..126 / 148..155 / 172..180. Artefakt-Gegenprobe der Zuordnung: im Lauf 2.2 stand der
Cursor beim Quadrat (F515) bei Bild (160, 96) = rechte Spalte, Zeile 2, und das Log zeigt danach
`maske=040` = Bit 6 = Schalter 7 = Bank 5 Bit 19 (sub12 @0x014C0 `22 05 13 01`).
**Ergebnis:** linke Spalte = Schalter 1..5 (Loesung 1,3,5 EIN = "die 3 Schalter"), rechte Spalte =
Schalter 6..10 (Loesung 7,9 EIN = "die 2 Schalter"). Die Erwartung des Auftrags stimmt.

Lampen-Wahrheit (Bitmaske m, Bit 0 = Schalter 1): oben = `(m & 0x01F) == 0x015`, unten =
`(m & 0x3E0) == 0x140`; beide zusammen <=> `m == 0x155`.

### 3.5 Die gruene Lampenkunst — Suche in RE1.5 (Ergebnis: nichts Verwendbares)

| Quelle | Befund |
|---|---|
| ROOM11F.BSS, alle 15 Cuts (`extracted/PSX/STAGE1/ROOM11F/ROOM11F00..14.bmp`, Kontaktbogen angesehen) | Die zwei Lampen existieren nur in Cut 10 (Raetselbuehne) und Cut 12 (Schraegansicht des Wandpanels) — in BEIDEN unbeleuchtet (dunkelgruenes Glas). Kein Cut zeigt sie leuchtend. |
| ROOM11F0 Prop-TIMs (Tabelle @0x0240): obj0 TIM @0x18DAC (Cursor), obj1 @0x20FCC, obj2..11 @0x231EC (Hebel, 128x64 8bpp) | Hebel-TIM = nur graues Metall (angesehen). Keine Lampenflaeche. |
| ROOM11F0 ESP (RDT+0x4C = 0x032F4) | Ids `05 07` = die allgemeinen Blut-/Brocken-Effekte (wie ROOM1140), keine Lampe. |
| RDT-Kopf | nSprite = 0. |
| ROOM1050 Cut 7/8 (Sicherungskasten, `ROOM10507/08.bmp`, angesehen) | Gruene/orange Leuchten sind RUND, ~6 px und fest ins MDEC-Bild gemalt — nicht als Sprite loesbar, andere Form als die quadratischen Generator-Lampen. |

RE1.5 ist hier also unfertig (die Lampenrahmen sind gemalt, eine Leuchtkunst fehlt) -> RE2.

### 3.6 Die zwei Lampen im RE1.5-Bild (Original-Hintergrund ROOM11F10.bmp, Pixel selbst gelesen)

| | Rahmen aussen | Oeffnung innen | gruenliches Glas | Mitte (Glas) |
|---|---|---|---|---|
| oben | x 213..231, y 67..85 | x 215..228, y 71..83 (14 x 13) | x 218..228, y 72..80 (z.B. @(223,75) = 0x2e3629) | (222.5, 75.5) |
| unten | x 213..231, y 126..143 | x 215..228, y 130..142 (14 x 13) | x 218..229, y 131..140 (z.B. @(223,134) = 0x2f3926) | (222.5, 135.5) |

(Glas-Bbox automatisch: "g - r >= 5 und g - b >= 5 und g >= 0x1c" -> x 216..229 / y 72..79 bzw. 131..140.)
Die Pfeile in lights.bmp zeigen genau auf diese Rahmen.

### 3.7 RE2 ROOM2130: dieselben Lampen, als ESP-Sprite, mit gruener Palette

**Kunst.** ROOM2130 hat genau ein ESP, Id 0x16 (`effect.esp` = RDT @0x03188, Kopf `16 ff ff ff ff ff ff ff`);
die TIM `esp16.tim` = RDT @0x0E398, 4256 B (= RDT-Kopfwort [20]), 4 bpp, 256x32, **vier CLUT-Zeilen**:
0 rot, 1 blau, **2 gruen**, 3 gelb (Paletten selbst dekodiert; gruen: Index 1..15 = 0xffff 0xeffb
0xdff7 0xcbb2 0xbb6e 0xa729 0x96e5 0x9284 0x8e03 0x8982 0x8521 0x84e1 0x80a0 0x8060 0x8020, Index 0 =
0x0000 = durchsichtig; **alle Nicht-Null-Eintraege tragen Bit 15** = halbtransparent). Acht Zellen
(Koordinatensaetze @effect.esp+0x70: `00 00 f0 f0`, ..., `60 00 f0 f0`, `80 00 f0 f0`, ...): Zelle 0..2 =
rechteckige Lampen, **Zelle 3/4 = die quadratische Gitterlampe** (u 96 / u 128, 32x32, Versatz -16/-16),
Zelle 5..7 = runde Leuchten.

**Wer zuendet was (Werkzeug `re15_port/tools/r34n_c/re2_2130_espr.py`, Ausgabe `C_belege/re2_2130_espr.txt`):**

| RE2 Stelle | Bytes | Platz | Unterindex | Bedeutung |
|---|---|---|---|---|
| sub04 @0x01180..0x01188 | `65 01` .. `65 05` | 1..5 | — | beim Betreten des Panels alle fuenf Schalterlampen AUS |
| sub04 @0x01294 (.. @0x0170C) | `64 01 16 02 00 00 ba 02 c2 a0 ce f5 c2 d4 00 00` | 1..5 | 0x02 | Schalterlampe N an, NACH der Zeigerfahrt; rot (CLUT+0), Anim-Strom 2 |
| sub04 @0x01774 / @0x01784 | `64 0b 16 00 .. b4 0b ..` / `64 0c ..` | 0x0B/0x0C | 0x00 | nach "OK": zwei Lampen am Waffenkammer-Schloss (Cut 8), ROT |
| sub04 @0x017A4/@0x017A6 | `65 0b` / `65 0c` | | | rote aus |
| **sub04 @0x017A8 / @0x017B8** | `64 0d 16 10 ..` / `64 0e 16 10 ..` | 0x0D/0x0E | **0x10** | **dieselben zwei Lampen GRUEN** (CLUT+2) = "Strom da" |
| **sub03 @0x00F3C..0x00F94** (Raumaufbau, `18 03` gosub aus sub00 @0x00F1C) | `06 00 56 00` / `21 04 3c 01` / 5x `64 0n 16 02 ..` | 1..5 | 0x02 | **ist das Raetsel geloest (4:0x3c), stehen alle fuenf Schalterlampen beim Betreten dauerhaft an** |

**Handler sce_espr_on2** (Tabelle @0x800a74c8 Eintrag 0x64 = 0x80056644, selbst disassembliert):
@0x80056664 `lhu a1,4(s1)` / @0x80056668 `lb a0,4(s1)` / @0x8005666c `lhu s0,2(s1)` / @0x80056674
`jal 0x80056a38` (Arbeitsobjekt) / @0x8005667c `sll a0,s0,24` / @0x80056680 `srl s0,s0,8` / @0x80056684
`sll s0,s0,16` / @0x80056688 `or a0,a0,s0` / @0x8005668c `lhu a1,6(s1)` / @0x800566ac `or a0,a0,a1` ->
a0 = (Esp-Id << 24) | (Unterindex << 16) | scale16; Lage = pc[8..13] -> @0x800566c0 `jal 0x8001c8c4`.

**Spawner 0x8001c8c4:** @0x8001c8c8 `srl t6,a0,24` (Id), @0x8001c8d0/d4 `srl v0,a0,16`/`andi t5,v0,0xff`
(Unterindex), @0x8001c8d8 `andi v0,t5,0x7` (Strom = Sub-Offset-Tabelle), @0x8001c95c/60 `ori v0,zero,0xa003`
/ `sh v0,24(t0)` (Flags), **@0x8001c9cc `lhu v1,4(t1)` / @0x8001c9e0 `srl v0,t5,3` / @0x8001c9e4
`sll v0,v0,6` / @0x8001c9f8 `addu v1,v1,v0` / @0x8001c9fc `sh v1,50(t0)` -> CLUT = Kopf-CLUT +
(Unterindex >> 3) * 64** (= eine VRAM-Zeile je 8), @0x8001c9d0/d4 `sll v0,a0,16`/`sw v0,56(t0)` ->
scale16 in Platz+58. Kopf-CLUT: der ESP-TIM-Lader FUN_8001bd38 legt die CLUT der ersten ESP-TIM auf
(0x120, 0x1E0) (@0x8001bd8c `li s4,0x1e0`, @0x8001be2c `addiu v0,v0,0x120`) und **patcht den Kopf**
(@0x8001be68 `jal GetClut` / @0x8001be70 `sh v0,4(s0)`) -> Unterindex 0x02 = Zeile 480 = ROT,
0x10 = Zeile 482 = **GRUEN**.

**Zeilen/Anim** (effect.esp, Zeilenblock @0x90, Sub-Offsets `04 00 12 00 20 00 2e 00 3c 00 4a 00`,
RE2-Zeile = 24 B, kopiert @0x8001ca60..0x8001caa8): Strom 2 (@0x110) Zeile 0 = `01 00 04 00 00 10 00 10
00 00 00 01 00 00 00 00 00 00 03 ba 20 00 00 00`. Routine 1 = Tabelle @0x8009d868[1] = 0x8001dc30:
@0x8001dc3c `lhu v1,18(v0)` / @0x8001dc44 `sh v1,24(v0)` -> **Flags := 0xBA03**; @0x8001dc40 `lbu a0,2(v0)`
/ @0x8001dc4c `sb a0,33(v0)` -> **Anim-Satz := 4**; @0x8001dc48 `lhu v1,42(v0)` / @0x8001dc50 `lhu a0,20(v0)`
/ @0x8001dc5c `or` / @0x8001dc60 `sh v1,42(v0)` -> **TPAGE |= 0x0020 = ABR 1 (B+F, additiv)**.
Anim-Saetze @effect.esp+0x10 (je 8 B, {Zelle, Anzahl, Dauer, S}): Satz 4 `03 01 01 20`, Satz 5
`04 01 01 20`, Satz 6 `04 01 ff 20`. Fortschalten (FUN_8001d68c @0x8001d7b8..0x8001d880, selbst
disassembliert): Zaehler (Platz+32) == 0 -> Satz+1; Dauer 0xFF -> Satz := Byte0 (@0x8001d824..0x8001d834);
Zaehler := Dauer; Zaehler-1 je Bild. **=> Zelle 3, Zelle 4, Zelle 3, Zelle 4 ... im Wechsel JEDES Bild.**
Erstes Bild: die RE2-Hauptschleife ruft den SCD-Laeufer VOR dem ESP-Tick (@0x800263b0 `jal 0x80053644`
-> FUN_800536c4 @0x800536ac; ESP-Tick @0x80026980 `jal 0x8001d300`), also laufen Routine 1 (Satz 4,
Zaehler 1) und das Fortschalten (Zaehler 1 -> 0, kein Satzwechsel) im Einschaltbild selbst -> **Zelle 3
ist schon im Einschaltbild zu sehen**, Zelle 4 im naechsten.

**Zeichnen** (Schleife um 0x800779ec, Sprite-Bauer FUN_80077ed0, selbst disassembliert):
@0x80077a0c `lhu s2,24(s0)` / @0x80077a18 `andi v1,s2,0xa000` (sichtbar nur mit 0xA000) /
**@0x80077a44 `andi v0,s2,0x1000` / @0x80077a4c `addiu s5,zero,44` / @0x80077a50 `addiu s5,zero,46` ->
Prim-Code 0x2E (POLY_FT4 halbtransparent)**, weil 0xBA03 & 0x1000; @0x8007808c `sb s2,7(t2)` schreibt ihn.
Groesse: @0x80077f14 `mult a2,t0` (S * scale16) / @0x80077f24 `mult t0,a0` (* camf = cam.fov>>7,
@0x800779ac `lhu v0,102(v1)` / @0x800779bc `srl v0,v0,7`) / @0x80077fd4 `sll v0,v0,4` / @0x80077fd8
`div t1,v0` (/ SZ<<4) / @0x80078004 `lhu v1,4(a1)` / @0x8007800c `mult v0,v1` (* defW 0x1000)
=> **Kantenlaenge = S * scale16 * camf / (SZ * 256)**; Schalterlampe: S = 32, scale16 = 0x02BA ->
87,25 Welteinheiten.

**Nachgerechnet am RE2-Bild** (`re15_port/tools/r34n_c/re2_2130_projektion.py`, Kamera camera.rid Cut 6:
fov 42280 -> H 330, pos (-24210,-2610,-12186) -> (-24210,-2610,-9990)): Kamera-Gegenprobe Zeiger Wert
0/80/100 -> x 26.9 / 239.8 / 293.1 (Skala-Teilstriche 0/80/100 bei x ~25 / 239 / 293). Lampen -> Mitten
x 109.1/133.4/160.0/186.6/213.2, y 120, **Sprite 25,8 px**; die Glasfenster im Hintergrund
ROOM21306.bmp: x 98..114, 125..140, 152..167, 178..194, 205..221 / y 113..127 (je ~17 x 15). Die Leuchte
deckt das Glas plus einen Hof. Bilder: `C_belege/re2_2130_lampen_rot.png` (= so sieht RE2 aus, 3-fach,
Ausschnitt x 80..240 / y 95..145), `C_belege/re2_2130_lampen_gruen.png` (gleiche Kunst, Palette 2).

## 4 Soll-Verhalten (Zeitlinie)

Bezeichnungen: Bild = Spiel-Tick (30/s, dieselbe Uhr wie VM und Zeiger). `m` = Schaltermaske
(Bit 0 = Schalter 1 = Bank 5 Bit 13). `L = 0x155` = Loesungsmaske. "gesperrt" = SCD-Padwoerter
`& 0xf000` (bestehender Haken game_step_common.c:1146-1150) + kein START-Poll (:1230).

### 4.1 Zwischenschalter (m wird NICHT L) — frei

| Bild | Was passiert | Quelle |
|---|---|---|
| t0 | Quadrat ueber Zelle n: sub01 startet Schalter-Sub, Klickton (Flanke) | sub01 @0x01106.. / scd_vm.c op_sce_key_ck |
| t0 .. t0+15 | Hebel kippt 16 Bilder; nur Zelle n gesperrt (Bit n) | sub06 @0x01322 / @0x01336 |
| t0+16 = b | Schalterbit wechselt -> Zeigerziel neu, **Lampen neu bewertet** | sub06 @0x01340; Tick panel_zeiger_common.c |
| b .. | Zeiger faehrt 1 Punkt/Bild aufs neue Ziel; **Cursor, Quadrat, EXIT, START frei** | RE2 @0x01216 (Fahrt); RE1.5 sub01..17 ohne `22 02 07 01` (Freiheit) |
| jederzeit | weiterer Schalter waehrend der Fahrt: Ziel wechselt in dessen Bild b', der Zeiger faehrt vom AKTUELLEN Wert aus weiter (kein Neustart, keine Sperre) | zustandsbasiertes Ziel (NUTZER-VORGABE 2026-09-26) |

Ist-Gegenstueck: in 2.2 steht der Cursor in b..b+49 still (panelsperre=1) — das faellt weg.

### 4.2 Letzter Schalter (m wird L) — Endsperre bis 80

| Bild | Was passiert | Quelle |
|---|---|---|
| b* | Schalterbit, das m = L macht; **beide Lampen gruen**; ab den Padwoertern, die in b* fuer die VM von b*+1 entstehen: **gesperrt** (Live-Lesung der Maske wie heute) | 3.3 (L <=> Ziel 80); Runde-31-Live-Lesung panel_zeiger_common.c:228-233 |
| b* .. k | Zeiger faehrt auf 80, k = b* + abs(80 - w) - 1 mit w = Zeigerwert im Bild VOR b* (1 Punkt/Bild, erstes Fahrtbild b*; nachgemessen 2.2: Bit F532, 0 -> 20, k = F551) | RE2 @0x011E0..0x01218 |
| k+1 .. k+30 | Stillstand auf 80, gesperrt, keine Meldung | RE2 sleep 30 @0x0171C/@0x0171D |
| k+31 | VM laesst @0x012E6 durch: Evt_exec(sub18) + Set(4,238,1); Bestaetigungston (Flanke 4:238); sub18 setzt eigene Pad-Sperre @0x01736; "Power supply OK." @0x01742; Lampen bleiben gruen (Bits 13..22 bleiben, 4:238 = 1) | RE2 @0x01752..0x01762; RE1.5 sub18 |
| k+32 .. | Cut 8, dann Raumlicht-Flackern Cut 0x0D / 0x0E ("die Lichter"), Cut 8 | sub18 @0x0173A..0x01782 |

Beispiel (Reihenfolge 7, 9, 3, 1 ohne Warten, dann 5; Werte nach Nutzer-Gewichten):
0 -> 20 -> 50 -> 40 -> 60, letzter Schalter 5 macht m = L im Bild b*: Zeiger faehrt 20 Bilder
(bzw. von seinem gerade erreichten Zwischenwert aus), steht 30 Bilder, Abnahme bei k+31.

### 4.3 Sonderfaelle (festgelegt)

| Fall | Soll | Begruendung |
|---|---|---|
| Waehrend der Endsperre laeuft noch eine VORHER begonnene Kippung eines anderen Schalters (gemessen 2.5) | endet sie (<= 16 Bilder), wird m != L: Sperre faellt im selben Bild, Lampe(n) aus, Ziel neu; die 30 Ruhebilder beginnen bei der naechsten 80 neu | Sperre = Funktion der Maske; `s_ruhe` wird bei jeder Maskenaenderung 0 (panel_zeiger_common.c:152). Eine Abnahme vor dem Ende dieser Kippung ist unmoeglich: sie braucht >= 31 Bilder nach b*, die Kippung dauert 16 (sub06 @0x01336 `0d 00 04 00 10 00`) |
| Zeiger faehrt nur DURCH die 80 (z.B. 90 -> 60) | keine Sperre, keine Abnahme | m != L (80 nur bei L, 3.3); RE1.5-Kette scheitert am ersten falschen Ck; Abnahme-Freigabe verlangt ziel == 80 + 30 Ruhebilder (panel_zeiger_common.c:202-208) |
| Doppel-Ausloesung | ausgeschlossen | Set(4,238,1) @0x012EA direkt hinter Evt_exec; Kette beginnt mit Ck(4,238)==0 @0x012BA; Ton nur an der Flanke (panel_zeiger_common.c:168-173) |
| Quadrat gehalten | Hebel kippt alle ~17 Bilder erneut (Zelle wieder frei @0x0135C, Sce_key_ck 0x51 liest das GEHALTENE Wort) | RE1.5-Original; heute durch die Sperre verdeckt. Endzustand: ab m = L gesperrt |
| EXIT (Zelle 12, sub17) | in 4.1 frei (RE1.5); in der Endsperre gesperrt (wie heute / RE2) | sub17 @0x015FA; RE2 @0x01110..@0x01818 |
| Raetsel verlassen (sub17) | Bits 0..22 aus (@0x0168C..0x016E4) -> Lampen aus, Ziel 0 | Lampen = f(Bits) |

### 4.4 Lampen: Zustand und Bild fuer Bild

* `oben  = geloest || (m & 0x01F) == 0x015` (Schalter 1..5 = 1,0,1,0,1; @0x012BE..0x012CE)
* `unten = geloest || (m & 0x3E0) == 0x140` (Schalter 6..10 = 0,1,0,1,0; @0x012D2..0x012E2)
* `geloest` = 4:238 (@0x012EA). Nach der Loesung dauerhaft an — Vorbild RE2 ROOM2130 sub03
  @0x00F3C..0x00F94: `if Ck(4,0x3c)` -> alle fuenf Panel-Lampen beim Betreten an. (In RE1.5 ist die
  Buehne nach der Loesung gar nicht mehr erreichbar: sub00 @0x0101A Else-Zweig setzt Slot 1 nur als
  Text @0x0101E und baut die Hebel-Props nicht auf — die Regel wirkt also nur im Abnahmebild und in
  Riegeln, schadet aber nirgends.)
* Sichtbar nur, wenn Raum 11F0/11F1 und aktiver Cut 10 (dieselbe Regel wie der Zeiger,
  panel_zeiger_common.c:255-256).
* Leuchten = RE2-Zelle 3 im Einschaltbild, dann Zelle 4, 3, 4 ... im Wechsel je Bild
  (je Lampe ab ihrem Einschalten; RE2-Anim-Saetze 4..6 + FUN_8001d68c, 3.7).
* Kein Einrasten: jede Maskenaenderung wird im Bild ihres Bits wirksam (an UND aus).

## 5 Bauplan

Grundsatz: alle Logik in den Dateien, die Spur C besitzt (`engine/src/panel_zeiger_common.c`,
`include/re15_panel_zeiger.h`) plus EINE neue Plattformdatei; in gemeinsamen Dateien nur eine
Haken-Zeile (main.c) und optional ein Paket-Gate. **game_step_common.c und scd_vm.c bleiben
unveraendert** — der Sperr-Haken (:1146-1150, :1230) und der Abnahme-Halt (scd_vm.c:1109) rufen
schon heute Funktionen aus panel_zeiger_common.c; geaendert wird nur deren Inneres.
Ressourcen des Vertrags: Bank-9-Bits 69/70, AOT 40/41, Nachrichten 28/29 werden **nicht gebraucht**
(Zustand vollstaendig aus Bank 5 Bits 13..22 + 4:238 ableitbar, kein neuer Text, keine neue Zone).

### 5.1 Schritte

1. **`include/re15_panel_zeiger.h`** — Kopf um einen Block "RUNDE 34 NACHT" ergaenzen (Nutzerwortlaut,
   Lesart aus Abschnitt 1, Belege aus 3.2/3.7), neue Konstanten (Tabelle 5.2) und drei Prototypen:
   ```c
   int re15_panel_lampen(void);            /* Bit0 = oben an, Bit1 = unten an (Stand letzter Tick) */
   int re15_panel_lampe_sicht(int nr, int *x0, int *y0, int *kante, int *zelle);
                                           /* nr 0 = oben, 1 = unten; 1 = zeichnen */
   int re15_panel_lampe_an_aus_maske(int nr, unsigned maske, int geloest);   /* reine Regel, Riegel */
   ```
   Kommentar an `re15_panel_zeiger_sperrt` neu fassen ("nur Endsperre").
2. **`engine/src/panel_zeiger_common.c`**
   * `re15_panel_zeiger_sperrt()` (heute :222-243) ersetzen durch:
     ```c
     if (!RE15_PANEL_IST_RAUM(g_current_room_id) || !s_eingeschwungen) return 0;
     if (!re15_game_flag_get(5, 0))   return 0;   /* Raetsel nicht aktiv (sub16 @0x015C2)          */
     if (re15_game_flag_get(4, 238))  return 0;   /* abgenommen: sub18 sperrt selbst @0x01736      */
     return panel_maske() == RE15_PANEL_LOESUNGSMASKE;   /* LIVE: Endsperre ab dem Bild des Bits */
     ```
     (die Zweige "Maske geaendert", "Zeiger faehrt", "Ruhe < 30", "Zeiger steht auf 80" fallen weg;
     in der Endphase sind sie alle in `maske == L` enthalten, weil L das Ziel 80 erzwingt und die
     Abnahme 4:238 setzt.) `s_ruhe`, `re15_panel_zeiger_abnahme_frei`, `..._abnahme_haelt`,
     Wertbildung, Fahrt, Ton: **unveraendert**.
   * Im Tick (nach der Stillstandszaehlung, vor der Messschiene): Lampen bewerten
     `an[0] = geloest || (m & 0x01F) == 0x015`, `an[1] = geloest || (m & 0x3E0) == 0x140`;
     je Lampe `s_lampe_takt[i] = an && !s_lampe_an[i] ? 0 : s_lampe_takt[i] + 1`, `s_lampe_an[i] = an`.
     `re15_panel_zeiger_reset` setzt beides auf 0.
   * `re15_panel_lampe_sicht(nr, ...)`: 0, wenn nicht Raum 11F0/11F1, nicht Cut 10 oder Lampe aus;
     sonst `x0 = RE15_PANEL_LAMPE_X0`, `y0 = nr ? RE15_PANEL_LAMPE_Y0_UNTEN : RE15_PANEL_LAMPE_Y0_OBEN`,
     `kante = RE15_PANEL_LAMPE_KANTE`, `zelle = RE15_PANEL_LAMPE_ZELLE_A + (s_lampe_takt[nr] & 1)`.
   * Messschiene RE15_PANEL_LOG: `lampe_o=%d lampe_u=%d lzelle_o=%d lzelle_u=%d` anhaengen.
3. **NEU `platform/pc/src/panel_lampen_pc.c`** (Spur-C-Datei):
   * `laden()` einmalig: `re15_pc_read_re2("LAMPE2130.TIM", &n)` (asset_root_pc.c:297, wie PANEL2130.*);
     pruefen: Magic 0x10, Flags 0x08, CLUT-Block 16x4 bei (0,480), Pixelblock 64x32 Halbworte;
     Zellen 3 und 4 (u 96/128, v 0, 32x32) mit CLUT-Zeile 2 in zwei `uint16_t[32*32]` (PSX-15-Bit mit
     Bit 15) dekodieren. Fehlt die Datei: einmal melden, Lampen still aus (wie PANEL2130).
   * `void re15_panel_lampen_pc_zeichnen(void)`: fuer nr 0/1 mit `re15_panel_lampe_sicht`:
     Zielpixel (x0+i, y0+j), i,j in [0,kante): Texel `u = i*32/kante`, `v = j*32/kante` (naechstes
     Texel, POLY_FT4 ohne Kantenabzug, weil Schritt < 0x1ffff @0x80078070-88); Texel 0x0000 ->
     auslassen; Bit 15 gesetzt -> **B+F je Kanal, bei 255 gesaettigt** auf `re15_pc_framebuffer()`
     (0xRRGGBBAA, render_pc.c:2629), Kanal = (Texel-5-Bit << 3); Modulation neutral (0x80).
     Bit 15 frei -> deckend (kommt in Palette 2 nicht vor, der Zweig bleibt fuer Treue).
4. **`platform/pc/main.c`** — EINE Haken-Zeile direkt hinter dem Zeiger-Block (heute :5282-5296, noch
   innerhalb `if (re15_bg_is_loaded())`, vor `} else {`):
   `{ extern void re15_panel_lampen_pc_zeichnen(void); re15_panel_lampen_pc_zeichnen(); }`
   Ebene 1 (Framebuffer, Skill re15-pc-render-order): nach BG und Zeiger, unter 3D-Props (die liegen
   bei x 64..172, die Lampen bei x 212..233 — keine Ueberdeckung), unter Text-Overlay (Meldung
   "Power supply OK." liegt richtig darueber).
5. **NEU `shared_assets/RE2/LAMPE2130.TIM`** (4256 B) = byte-gleicher Schnitt `ROOM2130.RDT[0x0E398 ..
   0x0E398+4256)` (= info/re2leon/PL0/RDT/room2130/esp16.tim). Werkzeug
   `tools/r34n_c/lampe2130_schnitt.py` liegt bereit (Standard = nur pruefen, `--schreiben` legt die
   Datei an); Pruefung in dieser Runde: 9/9 OK (`C_belege/lampe2130_pruefung.txt`). Paket: `release/make_package.sh:409` kopiert `shared_assets/RE2` ganz; optional eine
   Gate-Zeile in `check_tree` wie TORSE.VBS (:193).
6. **Riegel** `tests/unit/probe_r34n_c_generator.c` + `tests/unit/probes/r34n_c_generator.cmake`
   (ctest `unit_r34n_c_generator`, Harness wie r31_generator.c: echte RDT, scd_vm_tick + re15_game_step):
   * A Freie Fahrt: Schalter 7 per echter Cursorfahrt + Quadrat, dann UNTEN halten: Cursor-dz != 0 in
     JEDEM Fahrt- und Ruhebild, `sperrt() == 0`; Zeiger erreicht trotzdem 20.
   * B Endsperre: 7,9,3,1 frei, dann 5 -> ab dem Bild von m = L `sperrt() == 1`, Cursor-dz == 0 und
     0 Schalterwechsel bei gehaltenem HOCH+QUADRAT, Abnahme genau k+31 (Soll als Zahl 30 mit Beleg
     @0x0171D, nicht als Makro — Lehre Runde 31).
   * C Ziel wechselt waehrend der Fahrt (zweiter Schalter mitten in der Fahrt): keine Sperre, Zeiger
     dreht vom aktuellen Wert, keine Abnahme.
   * D Durchfahrt 90 -> 60 ueber 80: nie gesperrt, keine Abnahme.
   * E Lampen-Wahrheitstafel ueber alle 1024 Masken (+ 4:238): gegen die ZEHN Ck-Bytes
     @0x012BE..0x012E2 aus der RDT gelesen, nicht gegen die Port-Konstante (sonst selbstbestaetigend).
   * F Lampen-Zeitlinie: Einschaltbild = Bit-Bild, Zelle 3 dann 4 im Wechsel; aus im Bild des
     falschen Bits; kein Einrasten.
   * G ROOM11F1 wie 11F0; H Wiedereintritt nach Loesung: Lampen an, keine Sperre.
7. **Bestehende Riegel anpassen:** `tests/unit/r31_generator.c` Teil D prueft heute die Sperre nach
   einem ZWISCHEN-Schalter (Schalter 7) — genau das nimmt der Nutzer zurueck. D umschreiben auf
   "Zwischenschalter frei, Endsperre ab m = L" (oder in den neuen Riegel verschieben). A/B/C/E/F
   bleiben gueltig (C setzt Masken direkt). `r27_panel_schalterwerte` Teil G (90 Bilder Quadrat
   GEHALTEN) laeuft danach ohne Sperre: Hebel kippt alle ~17 Bilder neu — die Checks ("ein Ton je
   Betaetigung", "panel*4 < 90", "nie zwei Toene hintereinander") bleiben erfuellt, NACHFAHREN.
   `local_build.sh` RE15_MIN_TESTS +1.
8. **Nicht Teil dieses Plans:** PSX-Plattform (s. 8), Android-Konfiguration (neue Datei unter
   platform/pc/src -> GLOB neu einlesen, Memory reai-v2-android-glob-cache).

### 5.2 Konstanten-Tabelle

| Konstante | Wert | Beleg / Art |
|---|---|---|
| Endsperre nur bei Maske | `RE15_PANEL_LOESUNGSMASKE` 0x155 (besteht) | RE1.5 Ck-Kette @0x012BE..0x012E2 (1,0,1,0,1,0,1,0,1,0); Endsperre selbst = NUTZER-VORGABE Runde 34 ("Ausser ganz am Ende"), Sperrwirkung = RE2 Bank 2 Bit 7 @0x01110..@0x01818 / RE1.5 @0x80030514 `andi 0xf000` |
| Keine Sperre bei Zwischenschaltern | — | NUTZER-VORGABE Runde 34 ("man soll sich frei bewegen koennen"); deckt sich mit RE1.5 (kein `22 02 07 01` in sub01..sub17, 3.2) |
| Ruhe vor der Abnahme | 30 Bilder (`RE15_PANEL_RUHE_BILDER`, besteht) | RE2 @0x0171C `09` / @0x0171D `0a 1e 00` |
| Zeigerfahrt | 1 Punkt/Bild (besteht) | RE2 @0x01216 `02` in der Schleife @0x011E0..0x01218 |
| Lampe oben: Maske / Soll | 0x01F / 0x015 | Ck @0x012BE `21 05 0d 01`, @0x012C2 `.. 0e 00`, @0x012C6 `.. 0f 01`, @0x012CA `.. 10 00`, @0x012CE `.. 11 01`; Zuordnung oben <-> linke Spalte = NUTZER-VORGABE |
| Lampe unten: Maske / Soll | 0x3E0 / 0x140 | Ck @0x012D2 `21 05 12 00`, @0x012D6 `.. 13 01`, @0x012DA `.. 14 00`, @0x012DE `.. 15 01`, @0x012E2 `.. 16 00`; unten <-> rechte Spalte = NUTZER-VORGABE |
| Spaltenzuordnung | links = Schalter 1..5, rechts = 6..10 | Props @0x00E76..0x00EFE (x -25975) / @0x00F20..0x00FA8 (x -18775), Zellen @0x00D78..0x00E2C, Bild 3.4 |
| Lampe an nach der Loesung | `|| 4:238` | RE2 sub03 @0x00F3C `06 00 56 00` / @0x00F40 `21 04 3c 01` / @0x00F44.. 5x sce_espr_on2 |
| Kunst | LAMPE2130.TIM = ROOM2130.RDT @0x0E398, 4256 B | RE2 ESP 0x16 (effect.esp @0x03188, Kopf `16 ff ..`) |
| Zelle A / B | 3 (u 96) / 4 (u 128), v 0, S 32, Versatz -16/-16 | Koordinatensaetze effect.esp @0x7C `60 00 f0 f0` / @0x80 `80 00 f0 f0`; Anim-Saetze 4..6 @effect.esp+0x30 `03 01 01 20`, `04 01 01 20`, `04 01 ff 20`; Strom 2 Zeile 0 Byte 2 = 4 -> Routine 1 @0x8001dc40/0x8001dc4c |
| Wechseltakt | jedes Bild, Beginn mit Zelle 3 | Dauer-Byte 1 der Saetze 4/5, Schleife 0xFF -> Satz 4; FUN_8001d68c @0x8001d7b8..0x8001d880 |
| Palette | CLUT-Zeile 2 (gruen) | Unterindex 0x10 @0x017A8/@0x017B8 (`64 0d 16 10`, `64 0e 16 10`), (0x10 >> 3) * 64 @0x8001c9e0..0x8001c9fc, Kopf-CLUT gepatcht @0x8001be68/0x8001be70 |
| Mischung | B+F (ABR 1), nur Texel mit Bit 15, Saettigung | Flags 0xBA03 (Zeile+18) @0x8001dc3c/44 -> 0x1000 -> Prim 0x2E @0x80077a44..0x80077a50; TPAGE \|= 0x20 (Zeile+20) @0x8001dc48..0x8001dc60; psx-spx graphicsprocessingunitgpu.md:1416-1437 |
| Modulation | 0x80,0x80,0x80 (neutral) | Primitiv-Vorbelegung FUN_800783b4: @0x800783c4 `addiu v1,zero,768`, @0x800783cc/d0 `lui a0,0x2c80` / `ori a0,a0,0x8080`, @0x800783d8 `sw a0,4(v0)`; der Sprite-Bauer schreibt nur das Code-Byte (@0x8007808c `sb s2,7(t2)`) |
| Kantenlaenge | **22 px** | PORT-WAHL, abgeleitet: RE2-Bildgroesse 25,8 px (Formel @0x80077f14..0x8007800c mit S 32, scale16 0x02BA @0x01294+6, camf 330 aus camera.rid Cut 6, SZ 1116) x Verhaeltnis der Lampenoeffnungen RE1.5/RE2 ((14/17 + 13/15)/2 = 0,845; RE1.5 3.6, RE2 3.7) = 21,8 -> 22. Grund: RE1.5 hat keine ESP-Weltlage fuer die Lampe, gezeichnet wird 2D wie der Zeiger. Vorschau `C_belege/vorschau_k22_z3.png`; Vergleich 26/17 px `vorschau_k26_k17_vergleich.png` |
| Lage oben / unten | Ecke (212, 65) / (212, 125), Mitte (222.5, 75.5) / (222.5, 135.5) | gemessen: Mitte der gruenlichen Glasflaeche im Original-Hintergrund ROOM11F10.bmp (3.6) und identisch im Port-Framedump (2.3); Ecke = Mitte - 11 |
| Sichtbarkeit | Raum 11F0/11F1 und Cut 10 | wie Zeiger: Cut_chg 0x0A sub16 @0x015C0 (`29 0a`) |
| Ton beim Aufleuchten | keiner | RE2 zuendet Lampen ohne se_on (@0x01294 folgt `09 0a 1e 00`, kein `36`); RE1.5 0 Se_on im SCD |

## 6 Abnahmeplan (echte exe, Bilder ansehen)

Werkzeuge liegen bereit: `re15_port/tools/r34n_c/lauf.sh` (Messlauf, eigene exe-Kopie),
`cursor_spur.py` (Cursorlage + Zeigerlage aus den Framedumps neben RE15_PANEL_LOG),
`re15_lampen_vorschau.py` (Soll-Bild der Lampen auf dem Original-Hintergrund, exakt nach der Regel
aus 5.1 Schritt 3). Alle Laeufe mit `RE15_WINDOW_SCALE=3`, Framedumps = Readback vor Present.
⚠ Das gerenderte Bild haengt dem RE15_PANEL_LOG um 1 Bild nach (Hintergrund und Zeiger werden am
Anfang der Hauptschleife gezeichnet, VM und Spielschritt laufen danach; Runde-31-Pruefer §5, hier
nachgemessen: Framedump F535 zeigt Zeigerwert 3 = Log F534). Beim Vergleich Bild F mit Log F-1.

| # | Nutzerpunkt | Messung | Bestanden, wenn | Gegenprobe |
|---|---|---|---|---|
| 1 | "nach dem Klick ... frei bewegen" | `lauf.sh <d> 640 "U0.3,W0.2,A0.07,D3,W2" "505-640/3:fd_"` + `cursor_spur.py` | `panelsperre=0` in JEDER Zeile F500..F640; `cur_y` steigt von F517 bis zum Ende der Tastzeit ohne Plateau (>= 2 gleiche Werte bei gehaltenem UNTEN innerhalb des Felds); Zeiger steigt im selben Zeitraum 0 -> 20 | Ist-Lauf 2.2 (derselbe Befehl vor dem Umbau): Plateau `cur_y = 136.0` F532..F580, `panelsperre=1` 49 Bilder |
| 2 | "ganz am Ende ... zunaechst auf die 80" | Reihenfolge 7, 9, 3, 1 OHNE Warten (je Schalter W0.7 statt W4), dann 5; danach bis zur Abnahme alle 4 Bilder HOCH+QUADRAT (wie Pruefer-Lauf P2 Runde 31); Serie ab b*-3 bis Abnahme+5 | `panelsperre=1` genau ab dem Bild `maske=155` bis zur Abnahme; `maske` bleibt 155; `cur_x/cur_y` konstant in diesem Fenster; `wert` erreicht 80 im Bild k; `geloest=1`, `msg=1`, `bestaet=1` genau im Bild k+31; kein `panelsperre=1` in irgendeinem Bild VOR b* | Runde-31-Log `nachher_panel_F1180-1236.log` (Abnahme k+31 unveraendert) |
| 3 | Ziel wechselt waehrend der Fahrt | Schalter 9 (+30), nach 10 Bildern Schalter 3 (-10) | `ziel` 30 -> 20 im Bit-Bild von Schalter 3, `wert` dreht vom aktuellen Wert, `panelsperre=0` durchgehend | — |
| 4 | "kein Ausloesen bei Durchfahrt" | Riegel `unit_r34n_c_generator` Teil D (90 -> 60) | nie `sperrt()`, 4:238 bleibt 0 | Riegel Teil B loest bei L aus |
| 5 | Lampen oben/unten an/aus | Lauf mit Framedumps nach: Start; 1,3,5; +2; -2; +7; +9 (= L); Abnahmebild | Glasflaeche (x 218..228, y 72..80 bzw. 131..140): mittleres G gegen den Hintergrund (Glas-G im Original 0x30..0x3a, z.B. @(223,75) 0x2e3629) — AN: G > 150, AUS: G unveraendert (+-2); Tabelle: Start aus/aus, 1,3,5 an/aus, +2 aus/aus, -2 an/aus, +7 an/aus, +9 an/an, Abnahmebild an/an | RE15_PANEL_LOG `lampe_o/lampe_u` Bild fuer Bild gleich dem Bild |
| 6 | Lage/Groesse/Kunst | Framedump mit beiden Lampen an, auf 320x240 (naechster Nachbar) gegen `C_belege/vorschau_k22_z3.png` bzw. `_z4.png` (je nach `lzelle`) | Abweichung im Rechteck x 212..233 / y 65..86 und y 125..146 hoechstens MDEC-Rauschen (mittlere Kanaldifferenz <= 1, wie 2.3: 0,66); Bild angesehen | Bild einer AUS-Lampe = Hintergrund |
| 7 | Wechselrhythmus | zwei aufeinanderfolgende Framedumps mit Lampe an | Glasmuster wechselt zwischen Zelle 3 und 4 (`lzelle` 3/4 im Log), Beginn mit 3 im Einschaltbild | — |
| 8 | 11F1, Wiedereintritt, Laden | Riegel Teile G/H (DEBUG_JUMP nimmt das Varianten-Nibble nicht, 2.4); Speichern in 11F0 nicht moeglich (kein Speicherpunkt); Bank 5 Wort 0 (Bits 0..31, also alle Schalterbits) wird beim Raumaufbau geloescht (FUN_8003ecec @0x8003ed74 `sw zero,0x800b1028`, Port scd_room_setup.c:280) -> nach Laden/Betreten Lampen aus, Zeiger 0, ausser 4:238 (global, gespeichert) -> Lampen an, Panel ohnehin nur noch Text (sub00 @0x0101A) | Riegel gruen | — |
| 9 | nichts kaputt | `local_build.sh` all; Integrations-Haken mit echter exe bei Rot einzeln 2x nachfahren (Memory reai-v2-gui-tests-flattern) | 428 + neue Tests gruen | Rueckbau-Nachweis im Riegel: Endsperre aus -> B rot; Zwischensperre wieder an -> A rot; Lampenregel lax (nur EIN-Schalter) -> E rot |

Sichtpruefung wie Memory reai-v2-visual-verify-gdigrab: einmal gdigrab des echten Fensters beim
Loesen (beide Lampen gruen, Zeiger auf 80, dann "Power supply OK.").

## 7 Risiken, Softlocks, Wechselwirkungen

### 7.1 Softlock-Pruefung der neuen Sperrregel

| Moeglicher Haenger | Warum er nicht eintritt |
|---|---|
| Endsperre steht, Abnahme kommt nie | Sperre nur bei m = L; bei m = L zielt der Zeiger auf 80 (einzige Maske, 3.3) und kommt in <= 90 Bildern an; danach 30 Ruhebilder; die RE1.5-Kette ist bei m = L und 4:238 = 0 erfuellt -> Abnahme. Runde 31 hat denselben Endabschnitt bereits live gemessen (k+31). |
| Endsperre steht, aber eine vorher gestartete Kippung aendert m | Sperre faellt im Bild des Bits von selbst (Maske live), Spieler ist frei. |
| Spieler kann das Raetsel in der Endsperre nicht verlassen | gewollt (RE2 sperrt bis nach der Pruefung, @0x01818); dauert hoechstens 90 + 31 Bilder. |
| Sperre nach dem Verlassen (sub17) | sub17 loescht Bank 5 Bit 0 (@0x0168C) -> `sperrt()` kehrt sofort mit 0 zurueck (erste Bedingung). |
| Gehaltenes Quadrat kippt den Hebel wiederholt | RE1.5-Verhalten (0x51 liest das gehaltene Wort); in der Endsperre ausgeschlossen. Kein Haenger. |

### 7.2 Bestehende Riegel, die sich aendern (NACHFAHREN, nicht blind anpassen)

* `unit_r31_generator` Teil D — prueft die Zwischensperre nach Schalter 7 -> wird rot, MUSS auf die
  neue Regel umgeschrieben werden (5.1 Schritt 7). A/B/C/E/F bleiben.
* `unit_r27_panel_schalterwerte` Teil G (90 Bilder Quadrat gehalten) — laeuft ohne Zwischensperre
  anders (mehr Betaetigungen), die Checks sind relativ formuliert; nach dem Bau einzeln fahren.
* `unit_r26_panel_11f0` Teil A (90 Bilder Quadrat) — `umgelegt > 0`, bleibt.
* `unit_gen_11f0_switches` M6 (Abnahme-Zeitpunkt) — unberuehrt (Endabschnitt gleich).

### 7.3 Wechselwirkungen mit anderen Spuren

* **Spur B (Hebetisch 1150/1151)** benutzt panel_zeiger nur lesend (Klickton, Cursor-Vorbild). Die
  geaenderte `re15_panel_zeiger_sperrt()` ist raumgebunden (RE15_PANEL_IST_RAUM = 11F0/11F1) und
  wirkt in 1150 nicht. Falls B die alte Zwischensperre als Vorbild kopiert: sie gilt nicht mehr.
* **Spuren A, D, E (ROOM1050), F (1110/1230), G (1170):** keine gemeinsamen Raeume, Flags, Slots.
* **Gemeinsame Dateien:** main.c (1 Zeile hinter dem Zeiger-Block :5296 — Spuren, die ebenfalls
  hinter `re15_bg_blit` einhaengen, koennten dort kollidieren -> Zusammenfuehrung beachten),
  `local_build.sh` RE15_MIN_TESTS (jede Spur hebt die Zahl; die Integration setzt die Summe),
  optional `release/make_package.sh` check_tree (1 Zeile).
* **Granaten-Sitzung (r34g):** plant eine RE2-FX-Maschine fuer CORE00.ESP in `pc_draw_effects`
  (BAUPLAN.md Spur C). Keine Ueberschneidung: die Lampe ist eine Raum-ESP-Kunst, gezeichnet in einer
  eigenen Datei, ohne die ESP-Pools. Spaeter koennte sie auf deren RE2-Zeichner umziehen.
* **Android:** neue Datei unter platform/pc/src -> Android-GLOB-Cache neu konfigurieren
  (Memory reai-v2-android-glob-cache), sonst fehlt das Symbol beim Linken.
* **PSX-Plattform:** der Engine-Teil (Sperre, Lampenzustand) ist gemeinsam und wirkt dort sofort;
  der Zeichner fehlt dort (s. 8). Kein Absturz: die PSX ruft `re15_panel_lampe_sicht` nicht.

### 7.4 Bild-Risiken

* Die Lampe liegt in Ebene 1 unter dem Text-Overlay: "Power supply OK." ueberlagert sie im
  Abnahmebild korrekt. Unter den 3D-Props: keine Ueberdeckung (x 64..172 vs 212..233).
* Additiv auf 8-Bit-Framebuffer statt 5-Bit-VRAM: Unterschied hoechstens die unteren 3 Bit je Kanal.
* Groesse 22 px ist abgeleitet, nicht Original (RE1.5 hat keine Lampe). Vergleich 26/17 px liegt bei.

## 8 Offene Punkte

1. **PSX-Zeichner der Lampe** nicht geplant im Detail: braucht LAMPE2130.TIM im VRAM (4bpp, CLUT-Zeile
   2) und ein POLY_FT4 mit Code 0x2E + TPAGE-ABR 1 an den Ecken aus 5.2. Der PSX-Bau laeuft in diesem
   Projekt derzeit ohnehin nicht (Memory reai-v2-psx-build-gap).
2. **Kantenlaenge 22 px** ist PORT-WAHL (abgeleitet, 5.2). RE2s eigene Bildgroesse waere 26 px
   (`C_belege/vorschau_k26_k17_vergleich.png`, links; Abtastung dort noch mit halbem Pixel Versatz).
   Faellt dem Nutzer die Groesse auf, ist es genau eine Konstante.
3. **Die zehn kleinen Lampen links neben jedem Hebel** (im Hintergrund gemalt, dunkel) sind nicht
   Teil des Auftrags und bleiben dunkel. RE2 zuendet seine fuenf Schalterlampen (rot) je Schalter —
   eine Uebertragung waere ein eigener Nutzerwunsch.
4. **Meldung 3 "Power supply incorrect."** (RDT @0x1909) bleibt unbenutzt (Beta-Luecke, Runde 31 §7).
5. Die Lesart "untere Lampe gehoert zur RECHTEN Spalte, obwohl sie neben Zeile 3/4 sitzt" folgt dem
   Wortlaut ("Das untere Licht soll angehen, wenn die 2 Schalter rechts ...") — keine Deutung noetig,
   aber im Bild nicht offensichtlich; in der Rueckmeldung an den Nutzer erwaehnen.
6. Live nicht fahrbar: ROOM11F1 (DEBUG_JUMP ohne Varianten-Nibble) — nur Riegel.

