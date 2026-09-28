# Runde 30 / Thema H — Sicherung: unsichtbar im Hebetisch + „andere Sicherung" als Item

Stand: master 437905cb (v0.8.15). Phase ERMITTLUNG — es wurde KEIN Spielcode geändert.
Dieses Dossier ist eine FORTSETZUNG: ein Vorgänger-Agent wurde am Sitzungslimit abgebrochen.
Seine Messungen sind übernommen, wo sie der Stichprobe (§0) standhielten; wo nicht, steht
die Korrektur dabei.

## 1. Symptom / Auftrag

Nutzer wörtlich (AUFTRAG.md Abschnitt H):

> Achso, und vergessen unter C:\workspace\git\reAi_v2\build\sicherung hast du ein
> Sicherungsmodell erstellt, aber in ROOM 1170 im Modell das hochgeht ist es nicht sichtbar.
> Außerdem bekommt man nicht genau diese SIcherung als Item im Anschluss, sondern eine
> anderen Sicherung. Ergänze mir das.

Vom Nutzer bestätigt: gemeint ist **ROOM1150** (Irons' Office, Hebetisch), nicht ROOM1170.

Zwei Befunde:

* **H1 — unsichtbar.** Das Welt-Modell der Sicherung (Prop obj_id 4, `engine/src/sicherung_1150.c`)
  ist während der Hebetisch-Szene (sub04) nicht zu sehen.
* **H2 — andere Sicherung.** Das Aufnahme-Modal und das Inventar zeigen einen anderen
  Gegenstand als das Welt-Modell. Maßstab des Nutzers ist das **Modell** (`build/sicherung/`).

Dazu als Bestandsaufnahme: **H3 — das ROOM1050-Ende der Kette** (Shutter-Bedingung,
`Cut_chg 8`, `Message_on 2`).

## 0. Übernahme vom Vorgänger — Stichprobe und Korrekturen

Übernommen wurden die Ergebnisdateien unter `build/r30_sicherung/` und die Werkzeuge unter
`analysis/befunde_runde30/sicherung_werkzeug/`. Selbst nachgeschlagen:

| # | Aussage des Vorgängers | Nachgeschlagen | Ergebnis |
|---|---|---|---|
| S1 | sub04 beginnt @Datei 0x0F96, `Cut_chg 4` @0x0FB2, `Pos_set` @0x0FB4 | `xxd -s 0xF96 ROOM1150.RDT` = `22 02 00 01 …`; @0xFB2 = `29 04`; @0xFB4 = `32 00 24 af cf fe cc bb` (x=-20700, y=-305, z=-17460) | stimmt |
| S2 | Auslöser-Record slot 1 @0x0D7E, sce=0 | `2c 01 00 31 00 00 d8 aa e0 b1 dc 05 e4 0c ff 00 18 04 00 00` | stimmt |
| S3 | Item 0x40 heißt „Fuse" | DEBUG.BIN Tabelle @0x800C49DC = 0x0366 → Name @0x800C4D8E `22 51 4f 41 07`; Leser FUN_80028840 @0x80028840 (`lhu v1,0x800c495c(a0)` / `addu v0,v1,0x800c4a28`) | stimmt |
| S4 | Item-Bild 0x40 @ITPS.ITP 0xC0000, TIM 8bpp | Kopf `10 00 00 00 09 00 00 00 0c 02 00 00 00 00 e0 01 00 01 01 00` | stimmt |
| S5 | Icon/Bild 0x40 = RE2 „Fuse Case" | `re2_gleichheit.py`: Tile 0x40 @0x12C00 = RE2-Tile 77 @0x168F0, **1200 von 1200** Bytes; `re2_itps_gleichheit.py`: Block 0x40 @0xC0000 = RE2-Block 77 @0xE7000, **ganzer Block bytegleich** | stimmt |
| S6 | 40 von 40 Vierecken des Sicherungs-MD1 stehen im Umlauf | `viereck_stichprobe.py`: Viereck 0 (Face-Record @MD1+0x4E4) = (-203,11,26) (-203,26,11) (-107,26,11) (-107,11,26) = Umlauf | stimmt |
| S7 | Arbeits-RDT = Auslieferung | `cmp shared_assets/…/ROOM1150.RDT info/Re1.5/PSX/STAGE1/ROOM1150.RDT` = gleich | stimmt |

**Korrekturen am Vorgänger (alle drei ändern Zahlen, keine ändert die Richtung):**

* **K1 — Deckelweg 150, nicht 240.** `sitz_analyse.py`/`sitz_kandidaten.py` rechnen mit
  `DECKEL_WEG = 240  # For 24 @0x0FC0`. Der Record @Datei 0x0FC0 ist `0d 00 18 00 0f 00`:
  0x18 ist die **Blocklänge**, der **Zähler ist 0x0F = 15**. Beleg im Original, For-Handler
  @0x8003f540:

  ```
  8003f564: lh   t1,2(t0)      ; Blocklaenge
  8003f568: lhu  a1,4(t0)      ; Zaehler
  8003f56c: addiu t0,t0,6
  8003f594: sh   a1,0(v0)      ; Zaehler -> For-Feld (+160)
  8003f5a4: addu t1,t0,t1      ; Ausgang = Rumpf + Blocklaenge
  ```

  Gemessen in der Engine (`probe_r30_sicherung_fahrt`, `build/r30_sicherung/probe_fahrt.txt`):
  `MESS Bild 25: Plattform y= -305 Deckel1 z= 150 Deckel2 z= -150`. Derselbe Fehler steht im
  Kommentar `include/re15_sicherung.h` („24 Schritte a +-10"). Alle Verdeckungszahlen für
  „Deckel offen" sind deshalb neu gerechnet (`sitz_zeitplan.py`, §5.3).
* **K2 — Maske des alten Gegenstands war unvollständig.** `vergleich.py` und die erste
  Fassung von `sicherung_itembild.py` zählten „fast schwarze" Punkte zum Hintergrund. Das
  Item-Bild 0x40 hat aber eine **flache** Innenfläche 0x1C00 (5244 von 7344 Innenpunkten,
  `itps_hintergrund_40.py`); der Gegenstand misst 1972 px (nicht 1447), Achse 147,4° (nicht
  150°, `achse_altes_bild.py`). Folge im ersten Prototyp: die dunklen Flächen des alten
  Gegenstands blieben als Striche neben dem neuen stehen (`weg2_vorher_nachher.png`).
  Berichtigt in `weg2n_*` (§5.4).
* **K3 — der erste Weg-2-Prototyp wurde mit dem Umlauf-Modell gerendert.** Neu gerendert
  aus `build/r30_sicherung/sicherung_zordnung.md1`.

## 2. MESSUNG im Port

Alle Läufe: echte exe (`re15_port/build_r30_sicherung/platform/pc/re15_pc.exe` bzw. die
Mess-Variante `…/tests/unit/re15_pc_r30_sicherung.exe`), beschleunigter Renderer, **kein**
`RE15_AUTOSHOT`, **kein** `RE15_SOFTWARE_RENDER`. Bilder aus `RE15_FRAMEDUMP` (Readback vor
`SDL_RenderPresent`), 960x720. Laufskripte: `sicherung_werkzeug/lauf.sh`, `lauf_laden.sh`,
`lauf_laden2.sh`.

### 2.1 Auslöser von sub04

* Record @Datei 0x0D7E (S2): `Aot_set slot=1 sce=0 flags=0x31`, Rechteck Ecke (-21800,-20000)
  Größe 1500x3300, Nutzlast `ff 00 18 04 00 00` → sub 4. `sce=0` ist im Original **inert**
  (Handler[0] @0x8004305C, der ACTION-Scan überspringt sce-0-Records @0x80042f48-50). Der
  Port armiert genau diesen Record seit Runde 18 (`scd_vm.c:2869-2905`,
  `re15_aot_retype(slot,3,…)`).
* Im Spiel: Westseite des Mitteltisches, Aktionstaste. Lauf `lauf_taste` (Spieler
  (-21000,-18500), Eingabeskript): `[scd] thread start slot=10 first_op=0x22` →
  `[scd F103] Cut_chg(4)`. Die übrigen Läufe lösen denselben Weg über
  `RE15_FIRE_AOT=1@90#1150` aus (`[fire-aot] slot=1 at F90 (Raum 1150)`).

### 2.2 Zeitplan der Szene (Engine, `probe_fahrt.txt`; ROOM1150 und ROOM1151 identisch)

Bild = Engine-Tick nach dem Auslösen; im Spiellauf ist F = Bild + 91.

| Bild | F | Zustand | Beleg im Skript (Datei-Offset ROOM1150.RDT) |
|---|---|---|---|
| 5 | 96 | Plattform y=-305, Cut 4 | `Cut_chg 4` @0x0FB2, `Pos_set` @0x0FB4 |
| 10–24 | 101–115 | Deckelhälften ±10 je Bild → **±150** | `For` @0x0FC0 `0d 00 18 00 0f 00`, `Speed_set` @0x0FCA `2f 02 0a 00` / @0x0FD4 `2f 02 f6 ff` |
| 25–54 | 116–145 | steht | `Sleep 30` @0x0FDE |
| 55–145 | 146–236 | Plattform −10 je Bild → -1215 | `Speed_set` @0x0FF2 `2f 01 f6 ff`, `For` @0x0FF6 `0d 00 04 00 5b 00` (91) |
| **134** | **225** | **y=-1105 ≤ -1100 → Item-Modal** | Port: `sicherung_1150.c:28` `OBEN_BIS` |
| 146–155 | 237–246 | +1 je Bild → -1205 | `For` @0x1010 `0d 00 04 00 0a 00` (10) |
| 156–195 | 247–286 | steht oben | `Sleep 30` @0x101A, `Sleep 10` @0x102E |
| 196–289 | 287–380 | fährt herunter auf -305 | `For` @0x1042 (90), @0x105C (2), @0x106A (2) |
| 320–334 | 411–425 | Deckel schließen | `For` @0x1078 (15) |
| 395 | 486 | geparkt auf y=-20224, Cut zurück | `Pos_set` @0x109E `32 00 24 af 00 b1 cc bb`, `Cut_old` @0x10B2 |

### 2.3 Hypothesen, einzeln abgehakt

| # | Hypothese | Messzeile | Ergebnis |
|---|---|---|---|
| A | Prop nicht im Pool | `MESS pool[4] obj_id=4 type=0 parent=0 active=1 flags=0x0001 pos=(-628,-927,784) rot=(0,0,0)` (`probe_fahrt.txt`, beide Räume) | im Pool — **am Tür-/Sprungweg** |
| B | nicht in der Zeichenliste | `[prop-render] pi=4 oid=0x04 pos=(-20072,-1232,-18244) rot=(0,0,0) meshes=1` (`lauf_aot/debug.log`) | wird gezeichnet — **am Tür-/Sprungweg** |
| C | TIM-Slot `RE15_TIM_SLOT_PROP(4)=8` nicht hochgeladen | Differenzbild MIT/OHNE, F100: 1114 Punkte, Mittel RGB (78,81,76), Helligkeit 1..181, **706 verschiedene Farben** (`farbe_im_spiel.py`) | texturiert |
| D | Zeichen-Bit fehlt | Pool-Zeile A: `active=1 flags=0x0001`; der Cull (`re15_prop_culled`) greift nicht, sonst gäbe es Zeile B nicht | widerlegt |
| E | von der PRI-Maske verdeckt | `sichtbar_im_spiel.txt` gegen `sichtbar_im_spiel_nopri.txt` (`RE15_NO_PRI=1`): alle 13 Bilder F100–F220 **zahlengleich** | 0 Punkte durch PRI verdeckt |
| F | von Deckel/Plattform verdeckt | §2.5: 189 → 77 → 26 px (wahre Verdeckung, `sitz_zeitplan.txt`) bzw. 124 → 45 → 12 px (Spiel) | **ja** — die sichtbare Fläche fällt auf ein Zehntel |
| G | Lage/Größe | bbox im Spiel x135..144 y154..177 von 320x240 = 9x23 px | **winzig, Stirnansicht** |
| H | nur in EINER Raumvariante | `probe_fahrt.txt`: ROOM1150 und ROOM1151 Zeile für Zeile gleich | widerlegt |
| I | **am LADE-Weg gar nicht angelegt** | `laden_var_bestand/debug.log`: `[save] CONTINUE: resumed in room 1150 (hp=100)`, danach `[prop-render] pi=0`, `pi=1`, `pi=2` — **keine** Zeile `pi=4`, **kein** Modal | **BESTÄTIGT** |
| J | Vierecke falsch geordnet | `md1_zordnung.txt`: `SICHERUNG gen/sicherung_prop.inc 40 Vierecke: Z-Ordnung 0 | Umlauf 40 | Deckung Mittel 0.748` | **BESTÄTIGT** |

Kill-Switch zum Lokalisieren war durchgehend das Genommen-Flag: `RE15_SET_FLAG=9:53` vor dem
Raumeintritt → das Prop wird nicht angelegt (Läufe `lauf_aot_ohne*`). Jeder Bildpunkt, der
sich zwischen MIT und OHNE unterscheidet, gehört zur Sicherung.

### 2.4 Befund I im Einzelnen — der Lade-Weg

`re15_sicherung_install()` hat genau **eine** Aufrufstelle: `engine/src/scd_room_setup.c:410`,
in `scd_room_reenter()`. Der CONTINUE-Boot läuft nicht durch diese Funktion, er startet die
Threads direkt (`platform/pc/main.c:3989-4119`; dieselbe Lücke ist dort für die Polizeiweste
schon einmal beschrieben, `main.c:4020-4052`).

Messung (`sichtbar_ladeweg_bestand_gegen_ohne.txt`): der Lade-Lauf ist über **alle 25 Bilder
F90–F330 pixelgleich** mit dem Lauf OHNE Sicherung — 0 abweichende Punkte, also weder Prop
noch Modal. Wer in ROOM1150 einen Spielstand lädt und den Tisch auslöst, bekommt die
Sicherung nicht zu sehen und nicht angeboten. (Der Hund-Befund derselben Runde nennt als
Ausgangslage des Nutzers „geladener Stand in ROOM1150".)

Gegenprobe mit der Mess-Variante (`RE15_R30_NACHINSTALL=1`: die Variante ruft
`re15_sicherung_install` im ersten Spielbild nach — ohne main.c zu berühren), Lauf
`laden_var_nach`:

```
[save] CONTINUE: resumed in room 1150 (hp=100)
[r30-sicherung] Prop slot=4 Sitz=(-628,-927,784) rot=(0,0,0)
[r30-sicherung] NACHINSTALL im Spielbild: raum_aktiv=1
[prop-render] pi=4 oid=0x04 pos=(-20072,-1232,-18244) rot=(0,0,0) meshes=1
[r30-sicherung] Modal bei Plattform y=-1105
```

Danach trägt die Sicherung dieselben Punktzahlen bei wie am Türweg
(`sichtbar_ladeweg.txt`, F100 = 1114 Punkte). Modell und Textur stehen am Lade-Weg also
bereit (`pc_load_room_prop_set`, `main.c:3363`, läuft nach `g_current_room_id = boot_room`
@3192); es fehlt **nur** der Install-Aufruf.

### 2.5 Befund F/G im Einzelnen — wie viel von der Sicherung zu sehen ist

Im echten Spiel, Differenz MIT/OHNE je Bild (`sichtbar_im_spiel.txt`, auf 320x240 umgerechnet):

| F | 100 | 110 | 120–140 | 150 | 160 | 170 | 180 | 190 | 200 | 210 | 220 |
|---|---|---|---|---|---|---|---|---|---|---|---|
| Punkte | 124 | 89 | 45 | 29 | 15 | **12** | 27 | 51 | 76 | 101 | 87 |
| Anteil am Bild | 0,161 % | 0,116 % | 0,059 % | 0,037 % | 0,020 % | **0,016 %** | 0,035 % | 0,067 % | 0,099 % | 0,131 % | 0,114 % |

Ab F225 liegt das Item-Modal über der Bildmitte. Die Sicherung trägt zu keinem Zeitpunkt mehr
als **124 von 76 800** Bildpunkten bei, über die Hälfte der Szene weniger als 50.
Bildbelege: `ab_mit_ohne_zoom.png`, `lauf_aot_uebersicht.png`.

Gründe (Geometrie in Plattform-Koordinaten, MD1 von Prop 0 @Datei 0x11E40, `sitz_analyse.txt`):

* Sitz (-628,-927,784) liegt **außerhalb** des Kuppelfachs (Deckel-Grundriss x[-485..-74]
  z[875..1645], Naht z=1260) auf dem Deck **hinter** der Kuppel, von der Kamera aus gesehen.
* Die Längsachse zeigt auf die Kamera: Winkel zwischen Blickrichtung Cut 4 (Kamera @Datei
  0xE0, pos (-21942,-2160,-18378) → tgt (-19980,-1566,-18396)) und Längsachse 16,9° — die
  406 lange Sicherung erscheint auf 0,29 ihrer Länge; man sieht im Wesentlichen die Stirnkappe.
* Die aufgehende Deckelhälfte (Prop 2, −150 in z) und die Kuppel schieben sich davor.

### 2.6 Befund J — Viereck-Ordnung des Modells

`md1_zordnung.txt`: von 484 Vierecken der geprüften Original-Props (ROOM1150 Prop 0–3,
ROOM1170 Prop 0/1, ROOM11F0 Prop 0/1, RE2 ROOM60D0 Prop 1) stehen 482 in Z-Ordnung, 2 sind
entartet, **0 im Umlauf**. Die **40 Vierecke der Sicherung stehen alle im Umlauf**; gezeichnete
Deckung im Mittel **0,748** — ein Viertel jeder Mantelfläche bleibt offen, die Textur läuft
über Kreuz. Bild: `viereck_ordnung_im_spiel.png` (oben Bestand, unten berichtigt, beides
Framedump aus dem Spiel).

### 2.7 H2 — was Modal und Inventar zeigen (Bestand)

Vergleichsblatt `nebeneinander.png` (Vorgänger) + Messung `vergleich2.txt` (vollständige Masken):

| Darstellung | Quelle | Fläche | Länge | Dicke | Schlankheit | Achse | Mittel RGB | dunkel (<60) |
|---|---|---|---|---|---|---|---|---|
| (a) Welt-Modell | `gen/sicherung_prop.inc`, MD1 bbox x[-203..203] y/z[-26..26] | — | 406 | 52 | **7,8 : 1** | — | Textur (120,149,146) | 12,0 % |
| (b) Item-Bild | `ITEM/ITPS.ITP` @0xC0000, 112x72 | 1972 px | 83,9 px | 26,8 px | **3,1 : 1** | 147,4° | (62,58,61) | 57,1 % |
| (c) Inventar-Icon | `DATA/ITEMALL.PIX` @0x12C00, 40x30 | 368 px | 36,4 px | 12,1 px | **3,0 : 1** | 147,4° | (62,59,59) | 60,9 % |
| (d) ROOM1050 Cut 8 − Cut 7 | `STAGE1/ROOM105.BSS`, 2761 Punkte > 4 | 756 px | 72,0 px | 12,6 px | **5,7 : 1** | 94° | (48,59,58) | 59,3 % |

(a) und (d) sind derselbe Gegenstand (helles Rohr mit zwei Metallkappen, das Modell ist aus (d)
gebaut). (b) und (c) sind ein **anderer** Gegenstand: dunkler Sechskantkörper mit Bohrung und
Messingstift, gut doppelt so gedrungen.

Im Spiel gemessen (Mess-Variante, Item 0x40 in Inventarplatz 0, `inventar_bestand_gegen_weg2.png`):

* **Aufnahme-Modal:** zeigt (b) — `lauf_aot` F240–F330.
* **Inventar-Raster:** zeigt (c), Name „Fuse" — Lauf `inv_bestand_grid` F50.
* **CHECK-Foto: LEER.** Das Fotofeld bleibt dunkelblau, darunter nur „Fuse" — Lauf
  `inv_bestand_check` F140. Grund in §3.5. Gegenprobe (`check_foto_gegenprobe.png`), Punkte im
  Fotofeld, die nicht Hintergrundblau sind:

  | Item in Platz 0 | Blockformat | Punkte im Fotofeld (von 143 964) |
  |---|---|---|
  | 0x40 Fuse | RE2 | **0** |
  | 0x24 Green Medicine | RE2 | **0** |
  | 0x3F Pocket Watch | RE1.5 | 29 097 |
  | 0x40 Fuse, Weg-2-Prototyp | RE1.5 | 14 166 |

### 2.8 H3 — Bestand des ROOM1050-Endes der Kette

Vorarbeit: `analysis/befunde_2026-09-27/sicherung-verdrahtung.md` und der Riegel
`unit_room1050_sicherung` (`tests/unit/test_room1050_sicherung.c`,
`probes/r35_sicherung-1050.cmake`). Lauf des Riegels im eigenen Bauverzeichnis:
`build/r30_sicherung/test_room1050_sicherung.txt`, `RESULT: OK`. Selbst nachgeschlagen
(`room1050_scd.txt`, Byteanker-Suche über den SCD-Bereich 0x0AD8..0x0E3C):

| Was | Datei-Offset ROOM1050.RDT | Bytes | Stand |
|---|---|---|---|
| Shutter schon offen? | sub00 @0x0C1E | `21 03 79 00` Ck(3,121,0) | einzige Bedingung des Schalters |
| Schalter | sub00 @0x0C22 | `2c 07 03 31 00 00 a0 41 0a dd 20 03 20 03 ff 00 18 02 00 00` slot 7 sce=3 → sub02 | installiert, solange (3,121)=0 |
| Frage | sub02 @0x0CAC | `2b 00 80 ff` Message_on 0 | „It's a shutter switch. / Will you push it?" |
| Antwort JA | sub02 @0x0CB6 | `21 0c 1f 00` Ck(12,31,0) | — |
| Shutter offen | sub02 @0x0CBA | `22 03 79 01` Set(3,121,1) | **ohne** Sicherungs-Bedingung |
| Blick der Fahrt | sub02 @0x0CD0 | `29 03` Cut_chg 3 | — |
| Kollision frei | sub02 @0x0D4C–@0x0D70 | 5x `37 xx 13 00`, 5x `39 xx 13 00` | SCA-Zelle 19 aller fünf Partitionen |
| `Message_on` im ganzen Raum | @0x0CAC (msg 0), @0x0DD6 (msg 8) | — | **msg 2 „I need a fuse to run the shutter." wird nie gerufen** |
| `Cut_chg 7/8` (`29 07`/`29 08`) | — | 0 Treffer | **Cut 7 und 8 sind unerreichbar** |
| `Cut_replace` auf 7/8 (`4b 07/08`) | — | 0 Treffer | — |
| Cut 7 / Cut 8 | Kameratabelle @0x140 / @0x160 | erste 28 Byte gleich, pri 0x518 / 0x51C | zwei Zustände eines Blicks |

(Der Kommentar in `test_room1050_sicherung.c` nennt für das Cut-Paar `@0x1E0`/`@0x200`; die
Tabelle beginnt @0x60, Cut 7 liegt bei 0x60 + 7·32 = **0x140**, Cut 8 bei **0x160**.)

Im Port: **nichts davon ist verdrahtet.** `RE15_SICHERUNG_ITEM` wird genau einmal benutzt —
beim Öffnen des Modals (`sicherung_1150.c:114`). Keine Stelle fragt das Inventar nach Item
0x40, keine setzt einen Zustand „Sicherung eingesetzt", keine schaltet auf Cut 7 oder 8. Der
Gegenstand aus dem Hebetisch hat im Spiel zurzeit **keine Verwendung**; der Shutter öffnet
wie im Auslieferungsstand ohne ihn.

Das Vorbild für das Einsetzen steht vollständig im Schwesterrätsel ROOM2060 (übernommen aus
dem Vor-Dossier §1.2, dort mit Bytes): Nahaufnahme vorher `Cut_chg 8` @0x01688, Zustand
`Set(3,144,1)` @0x0169E, Nahaufnahme nachher `Cut_chg 9` @0x016A2, dauerhafter Tausch
`Cut_replace 5,11` @0x016C2 / `6,10` @0x016C5 und derselbe Tausch beim Wiedereintritt in sub00
@0x010F2/@0x010F5. Dort ist die Sicherung aber ein **Flag** (bank 3 bit 108), kein
Inventar-Gegenstand.

## 3. ORIGINAL-MECHANISMUS

Vorab die Einordnung (Beta → Retail): Die Sicherung **im Hebetisch** ist Port-Inhalt auf
Nutzer-Vorgabe vom 2026-09-27. Das Original hat dort keinen Gegenstand; für Sitz, Drehung
und Modal-Zeitpunkt gibt es deshalb **keine** Original-Adresse. Belegbar sind die Mechanismen,
auf denen der Einbau steht, und die Geometrie, in die er gesetzt wird.

### 3.1 Der Raumlader läuft am Tür- UND am Lade-Weg

```
8001d5ac: jal 0x800396fc        ; Session-Start / LOAD  (FUN_8001d22c)
8001d988: jal 0x800396fc        ; Tuer                  (FUN_8001d600)
```

FUN_800396fc hat im ganzen Image (t_addr 0x80010000, t_size 0xaf000) **genau diese zwei
Aufrufer** (Wortsuche nach `jal 0x800396fc`); die beiden umschließenden Funktionen beginnen
@0x8001d22c (Prolog `addiu sp,sp,-24`, gerufen @0x8001c96c) und @0x8001d600 (gerufen
@0x8001ca54). FUN_800396fc ruft ihrerseits @0x80039a00 `jal 0x8003ef6c` die SCD-Raum-Init.
Jeder Raumstart — neu, geladen oder durch eine Tür — läuft also durch **dieselbe**
Initialisierung; im Original gibt es keinen Raumzustand, der nur an einem Weg entsteht.
(Dass @0x8001d22c der Session-/LOAD-Zweig ist, steht so in `main.c:4075-4078`; selbst
nachgeschlagen habe ich die beiden Aufrufstellen, nicht die Rolle der Funktion.) Der Port hat zwei getrennte Wege
(`scd_room_reenter` / Boot in `main.c`), und die Sicherung hängt nur am ersten.

### 3.2 Anhänge-Form `Obj_model_set pc[5] = 0xC0` (Elternmatrix)

```
80040a04: andi v1,a0,0xc0
80040a34: ori  v0,zero,0xc0
80040a38: beq  v1,v0,0x80040a84
80040a84: addu v0,v0,a0          ; (a0*8 + a0)
80040a88: sll  v0,v0,2
80040a8c: addu v0,v0,a0          ; *4 + a0
80040a90: sll  v0,v0,2           ; = a0 * 148
80040a98: addiu v1,v1,-12064     ; 0x800ad0e0
80040a9c: addu v0,v0,v1
80040aa0: sw   v0,116(a1)        ; pool+116 = Zeiger auf die Elternmatrix
```

In ROOM1150.RDT tragen genau die Deckelhälften diese Form: @Datei 0x0E22 `2d 01 00 00 01 c0 …`
und @0x0E44 `2d 02 00 00 01 c0 …`. Die Sicherung benutzt dieselbe Form port-seitig
(`parent_obj = 0`), gemessen §2.3 A.

### 3.3 Eckenfolge der Vierecke (Z-Ordnung)

FUN_800256b0 (Viereck-Zeichner der Raum-Objekte, `RE_15_Quellcode_V2/FUN_800256b0.c`):

```
gte_ldv3(v0,v1,v2); gte_rtpt(); gte_nclip();      ; Face-Record {n0,v0,n1,v1,n2,v2,n3,v3}
gte_stsxy3_gt3(prim);                             ; -> POLY_GT4 x0y0, x1y1, x2y2
gte_ldv0(v3); gte_rtps(); gte_stsxy(prim+0x2c);   ; -> POLY_GT4 x3y3
param_3->cd = param_4 << 1 | 0x3c;                ; Primitiv 0x3C = POLY_GT4
```

Die vier MD1-Ecken gehen unverändert in die vier Ecken des GPU-Primitivs. Die GPU teilt
ein Viereck in (1,2,3) + (2,3,4) (psx-spx `graphicsprocessingunitgpu.md:210`: „first
consisting of vertices 1,2,3, and the second of vertices 2,3,4"). Ecke 0–1 ist also eine
Kante, Ecke 2–3 die gegenüberliegende; 0→1→2→3 ist ein **Z**, kein Umlauf. Der Port teilt
passend dazu (0,1,3) + (0,3,2) (`platform/pc/main.c:9735-9746`).

### 3.4 Normalen zeigen nach außen

Gemessen an konvexen ausgelieferten Modellen (`md1_normalen_wenden.py`): ROOM1150 Prop 1 und
Prop 2 (Deckelhälften, MD1 @Datei 0x138D4 / 0x13B88) 24 von 24 Eck-Normalen außen; RE2
ROOM60D0 Prop 1 (Fuse Case) 80 von 80; Prop 3 (Main Fuse) 120 von 120.
Sicherung (`normalen_pruefen.py`): **160 von 160 nach innen** — z.B. Punkt 0 (-203,11,26) mit
Normale (3009,-1064,-2568), Kappenmitte (-203,0,0) mit Normale (+4096,0,0).

### 3.5 Item 0x40: Name, Bild, Icon — und woher sie stammen

* Name: FUN_80028840 @0x80028840, Tabelle @0x800C495C, Eintrag 0x40 @0x800C49DC = 0x0366 →
  @0x800C4D8E `22 51 4f 41 07` = „Fuse" (§0 S3).
* Bild und Icon sind **bytegleich RE2-Retail-Item 0x4D „Fuse Case"**: ITPS-Block 0x40
  @0xC0000 = RE2 `COMMON/DATA/ITPS.ITP` Block 77 @0xE7000 (0x3000 von 0x3000 Byte); Icon-Tile
  0x40 @0x12C00 = RE2 `COMMON/DATA/ITEMALL.PIX` Tile 77 @0x168F0 (1200 von 1200). Ebenso
  0x41 „Spark Plug" = RE2 0x4C „Main Fuse" (Block 76 @0xE4000, Tile 76 @0x16440).
* RE2 Retail hat zu diesem Bild ein **Weltmodell**: `PL0/RDT/ROOM60D0.RDT` Prop-Slot 1, MD1
  @0x0ED8 (1356 B), TIM @0x1F384 (26144 B), platziert mit `Item_aot_set_4p` @0x08FC
  (`extracted_re2_sicherung/item_077_fuse_case/beleg.json`).
* **Der Block 0x40 ist im RE2-Format, nicht im RE1.5-Format** (`itps_koepfe.py`): 53 der 72
  RE1.5-Blöcke tragen `crect (0,489) 256x1 | prect (832,256) 56x72`; 19 Blöcke — darunter
  0x40 und 0x41 — tragen `crect (0,480) 256x1 | prect (0,0) 56x72`, und **alle 100**
  RE2-Blöcke tragen genau diese zweite Form. Bildkopf von Block 0x40 @Datei 0xC0214:
  `8c 1f 00 00 00 00 00 00 38 00 48 00`.
* Der Foto-Lader des RE1.5-Statusschirms lädt an die **eingebetteten** Rechtecke
  (DEBUG.BIN @0x800c0258):

  ```
  800c0260: jal 0x8006bbbc       ; OpenTIM
  800c0268: jal 0x8006bbcc       ; ReadTIM -> TIM_IMAGE @sp+4
  800c0280: jal 0x80068c88       ; LoadImage(crect, caddr)   a0 = 8(sp), a1 = 12(sp)
  800c02a0: jal 0x80068c88       ; LoadImage(prect, paddr)   a0 = 16(sp), a1 = 20(sp)
  ```

  Das Fotofenster des RE1.5-Schirms liegt bei (832,256) mit CLUT-Zeile (0,489). Ein Block
  mit prect (0,0) landet dort nicht. Deshalb ist das CHECK-Foto der Sicherung leer — im Port
  (`inv_render_pc.c:365-400` übernimmt nur Blöcke mit den RE1.5-Rechtecken) und nach derselben
  Logik im Original.
* Jeder Block führt das 40x30-Icon ein zweites Mal bei **+0x21A0**: 66 von 72 Blöcken sind
  dort bytegleich mit ihrem ITEMALL-Tile (die 6 übrigen sind die Breit-Waffen 0x0E–0x13 mit
  80x30).

**Einordnung:** Die Darstellung von Item 0x40 ist in RE1.5 **unfertig** — kein Weltmodell,
0 Platzierungen (Zensus über alle 240 RDT), Bild im Format eines anderen Statusschirms. Die
Hintergrundkunst von ROOM1050 (Cut 7/8) zeigt dagegen fertig eine **Rohr**-Sicherung in genau
dem Sockel, in den der Gegenstand gehört.

### 3.6 Geometrie des Hebetischs (ausgelieferte Bytes)

Prop 0, MD1 @Datei 0x11E40 (Punktliste @0x11E84, 163 Punkte; Vierecke @0x12950, 120 Stück).
Der Boden des Kuppelfachs ist ein Achteck auf y = -1036 aus den Vierecken 79/80/81
(Face-Records @0x12E40 / 0x12E50 / 0x12E60):

| Punkt | @Datei | Koordinate |
|---|---|---|
| 101 | 0x121AC | (-74, -1036, 1260) |
| 103 | 0x121BC | (-128, -1036, 1562) |
| 105 | 0x121CC | (-280, -1036, 1645) |
| 107 | 0x121DC | (-432, -1036, 1562) |
| 109 | 0x121EC | (-485, -1036, 1260) |
| 111 | 0x121FC | (-432, -1036, 958) |
| 113 | 0x1220C | (-280, -1036, 875) |
| 115 | 0x1221C | (-128, -1036, 958) |

Mitte des Achtecks: x = -280 (Punkte 105/113), z = 1260 (Punkte 101/109). Ausdehnung 411 in x,
770 in z. Deckelhälften: Prop 1 MD1 @0x138D4 x[-485..-74] y[-1185..-1036] z[1260..1645],
Prop 2 MD1 @0x13B88 z[875..1260]; Naht z = 1260. Offen fahren sie je 150 (§0 K1), die
Öffnung ist dann z[1110..1410] = 300 breit.

Die Textur dieses Bodens (UV u[0..61] v[215..248]) zeigt einen Kasten mit Schlüsselloch
(`draufsicht.png`, `proto_weg2_normal_zoom.png`).

## 4. URSACHE

**H1 „nicht sichtbar" hat vier Ursachen, die sich überlagern:**

| # | Ursache | Wirkung | Beleg |
|---|---|---|---|
| U1 | `re15_sicherung_install()` fehlt am Lade-Weg | nach LOAD in ROOM1150: **kein Prop, kein Modal**, 0 Bildpunkte | §2.4 |
| U2 | Sitz (-628,-927,784), Drehung 0: außerhalb des Fachs, hinter der Kuppel, Längsachse zur Kamera | 12–124 von 76 800 Bildpunkten | §2.5 |
| U3 | Vierecke im Umlauf statt in Z-Ordnung | ein Viertel jeder Fläche offen, Textur über Kreuz | §2.6, §3.3 |
| U4 | Normalen nach innen | Beleuchtung verkehrt: Mittel RGB (65,65,60) statt (100,107,101) | §3.4, §5.2 |

Keine Ursache sind: Pool, Zeichenliste, TIM-Slot, Zeichen-Bit, PRI-Maske, Raumvariante
(§2.3 A–E, H).

**H2 „andere Sicherung":** Das Welt-Modell ist aus der ROOM1050-Hintergrundkunst gebaut
(Rohr-Sicherung). Item 0x40 trägt aber Bild und Icon eines anderen Gegenstands — bytegleich
RE2s „Fuse Case". Beides ist ausgeliefert, beides heißt „Fuse", aber es sind zwei
verschiedene Dinge. Dazu kommt, dass das CHECK-Foto der Sicherung im Inventar leer bleibt,
weil der Block die Rechtecke eines anderen Statusschirms trägt (§3.5).

## 5. UMSETZUNGSPLAN für den Bau-Agenten

Reihenfolge ist Abhängigkeit: erst das Modell (Schritt 2), dann die Bilder daraus (Schritt 4).
Soll-Dateien zum Bytevergleich liegen versionierbar unter
`analysis/befunde_runde30/sicherung_werkzeug/soll/`.

### 5.1 Schritt 1 — Lade-Weg (U1)

* **Datei:** `platform/pc/main.c`, Boot-/CONTINUE-Weg. Hinter dem Block, der sub00 als
  Init-Lauf fährt (`scd_vm_set_room_init(0);` Zeile 4119, Blockende), und **vor** dem
  Kamera-Restore (Kommentar „FE-4 CONTINUE: restore the SAVE-TIME camera cut LAST", Zeile 4129):
  `re15_sicherung_install((uint16_t)g_current_room_id);` — dieselbe Zeile, die
  `scd_room_setup.c:410` am Türweg hat. `re15_sicherung.h` ist in main.c schon eingebunden
  (Zeile 68). Die Funktion tut in jedem anderen Raum nichts und legt nichts doppelt an
  (`sicherung_1150.c:62-69`).
* **Konstanten:** keine.
* **Beleg:** §3.1 (ein Raumlader, zwei Aufrufer @0x8001d5ac / @0x8001d988).
* **Riegel:** Integrations-Pin nach dem Vorbild `integration_weste_load_pin`
  (`tests/integration/test_weste_load_pin.cmake`, Karte über `probe_r17_weste_karte
  re15_card.mcr 1150`): CONTINUE in ROOM1150, `RE15_FIRE_AOT=1@90#1150`, Lauf bis F≥110.
  Er muss im `debug.log` die Zeile `[prop-render] pi=4 oid=0x04` finden. Vorher-Stand
  gemessen: die Zeile fehlt (`laden_var_bestand/debug.log`).
* **Abnahme-Messung:** `lauf_laden.sh` (echte exe) und danach
  `sichtbar_im_spiel.py lauf_laden lauf_aot_ohne 225`: mit dem neuen Sitz ≥ 200 Punkte in
  jedem Stichbild F120–F220. Vorher: 0 in allen 25 Bildern.

### 5.2 Schritt 2 — Modell berichtigen (U3, U4)

* **Datei:** `re15_port/tools/sicherung_engine_export.py`, Funktion `md1_bauen()`; danach das
  Werkzeug laufen lassen → `engine/src/gen/sicherung_prop.inc` neu.
  * Zeile 151-156, Vierecke in **Z-Ordnung** schreiben: Face-Record `a,a, b,b, e,e, c,c`
    (statt `a,a, b,b, c,c, e,e`) und die UV-Sätze in derselben Folge
    `uv(f[0]), uv(f[1]), uv(f[3]), uv(f[2])`. Beleg §3.3.
  * Zeile 104-115, Normalen **nach außen**: das Vorzeichen jeder Normale wenden (die
    Achsentausch samt Y-Spiegelung in Zeile 97 hat die Händigkeit gedreht, das Kreuzprodukt aus der
    OBJ-Umlaufrichtung zeigt danach nach innen). Beleg §3.4.
* **Konstanten:** keine neuen.
* **Abnahme-Messung (bytegenau):** die 2532 MD1-Bytes der neuen `.inc` müssen gleich
  `soll/sicherung_zord_normal.md1` sein (sha256 `39badaa3…194efb1`). Dazu
  `md1_zordnung.py` → `Z-Ordnung 40 | Umlauf 0`, Deckung ≥ 0,99 (gemessen 0,999);
  `normalen_pruefen.py` → `nach AUSSEN 160 | nach INNEN 0`.
* **Abnahme im Spiel:** `farbe_im_spiel.py <lauf> lauf_aot_ohne 140` — Mittel RGB der
  Sicherung gemessen am Prototyp **(100,107,101)**, Bestand (65,65,60).
* **Riegel:** Unit-Test auf `re15_sicherung_md1_bytes()`: je Viereck schneiden sich die
  Diagonalen 0–3 und 1–2 (Z-Ordnung); je Mantelpunkt ist Normale·(0,y,z) > 0.

### 5.3 Schritt 3 — Sitz und Drehung (U2)

* **Dateien:** `include/re15_sicherung.h` (Konstanten + Kommentar), `engine/src/sicherung_1150.c`
  Zeile 85 (`rot_y`).

| Konstante | Wert | Herkunft (ROOM1150.RDT, identisch in ROOM1151) |
|---|---|---|
| `RE15_SICHERUNG_POS_X` | **-280** | Mitte des Fachbodens: Punkte 105 @0x121CC und 113 @0x1220C tragen x = -280; Rand -485 @0x121EC / -74 @0x121AC |
| `RE15_SICHERUNG_POS_Y` | **-1062** | Fachboden y = -1036 (alle acht Punkte, §3.6) minus Rohrradius 26 (MD1 der Sicherung, y[-26..26]) |
| `RE15_SICHERUNG_POS_Z` | **1260** | Naht der Deckelhälften = Mitte des Fachbodens: Punkte 101 @0x121AC und 109 @0x121EC tragen z = 1260 |
| `RE15_SICHERUNG_ROT_Y` (neu) | **1024** | Viertelkreis (4096 = 360°): Längsachse X des Modells → Z der Plattform = lange Seite des Fachs (770 gegen 411) und quer zur Blickrichtung von Cut 4 |

  ⛔ Das sind **keine Original-Werte** — das Original hat dort keinen Gegenstand. Es ist eine
  Port-Wahl, und jede Zahl ist aus den ausgelieferten Modellbytes abgeleitet, nicht geschätzt.
  So muss es auch im Code-Kommentar stehen.
* **Warum das Kuppelfach und nicht das Regalfach** (gemessen im echten Spiel,
  `sichtbar_varianten.txt`, `sichtbar_proto_weg2_normal.txt`; Punkte bei 320x240):

| Sitz | sichtbar in Stichbildern F100–F220 | kleinste / größte Fläche | erste Sichtbarkeit |
|---|---|---|---|
| Bestand (-628,-927,784) rot 0 | 13 von 13 | 12 / 124 | F100 |
| **Kuppelfach (-280,-1062,1260) rot_y 1024** | **12 von 13** | **211 / 439** | F110 (Deckel gehen auf) |
| Regalfach A (-150,-116,478) rot_y 1024 | 2 von 13 | 87 / 548 | F210 (15 Bilder vor dem Modal) |
| Regalfach B (-150,-117,1332) rot_y 1024 | 2 von 13 | 72 / 549 | F210 |

  Das Kuppelfach ist das Fach, das die Szene selbst öffnet (`For` @0x0FC0), die Sicherung
  liegt dort 115 Bilder lang sichtbar, bevor das Modal kommt. Wahre Verdeckung über den
  ganzen Zeitplan: `sitz_zeitplan.txt` (Kuppelfach quer 41 von 43 Stichbildern, Mittel 358 px;
  Bestand Mittel 84 px).
* **Kommentar berichtigen** (`re15_sicherung.h`): „24 Schritte a +-10" → 15 Schritte
  (`For` @0x0FC0 `0d 00 18 00 0f 00`, Zähler = drittes Feld, @0x8003f568), Deckelweg ±150;
  Hochpunkt -1215, danach -1205; Parklage nach der Szene -20224 (`Pos_set` @0x109E).
* **Riegel:** `probe_sicherung_1150.c` Prüfung 4 folgt den Konstanten (Text „y=-927"
  nachziehen). Neu: Unit-Test, der aus ROOM1150.RDT **und** ROOM1151.RDT den Fachboden liest
  (Prop-0-Vierecke 79–81) und prüft, dass (POS_X, POS_Z) dessen Mitte ±1 und POS_Y = Boden − 26
  ist — so fällt der Test, wenn jemand den Sitz verschiebt, ohne die Geometrie anzusehen.
* **Abnahme-Messung:** `lauf.sh <marke> aot` mit der echten exe, dann
  `sichtbar_im_spiel.py <marke> lauf_aot_ohne 225`: ≥ 200 Punkte in jedem Stichbild
  F120–F220, 0 bei F100 (Deckel zu). Bildvergleich gegen `proto_weg2_normal_spiel.png`.

### 5.4 Schritt 4 — Item-Bild und Icon aus dem Modell (H2, Weg 2 = Empfehlung)

* **Erzeugen:** `python re15_port/tools/sicherung_itembild.py --inc
  re15_port/engine/src/gen/sicherung_itembild.inc` (liest das berichtigte Modell aus
  `gen/sicherung_prop.inc`, deshalb nach Schritt 2). Ergibt
  `re15_sicherung_itps_block[0x3000]` und `re15_sicherung_icon_tile[1200]`.
  Bytevergleich: `soll/weg2n_itps_block_40.bin` (sha256 `33d01cb1…`),
  `soll/weg2n_icon_tile_40.bin` (sha256 `d6a3248f…`).
* **Einsetzen — im Speicher, nicht auf der Platte.** Neue Funktionen in
  `include/re15_sicherung.h` / `engine/src/sicherung_1150.c`:
  `void re15_sicherung_bild_einsetzen(uint8_t *itps, int size);`
  `void re15_sicherung_icon_einsetzen(uint8_t *itemall, int size);`
  Sie überschreiben im **geladenen Puffer** Block bzw. Tile 0x40. Aufrufstellen — alle sechs,
  sonst zeigen zwei Leser zwei Gegenstände:

| Datei | Zeile | Puffer |
|---|---|---|
| `platform/pc/main.c` | 3789-3790 | ITEMALL.PIX vor `re15_itemall_set_pix` |
| `platform/pc/main.c` | 3796-3797 | ITPS.ITP vor `re15_itps_set_data` |
| `platform/pc/src/inv_render_pc.c` | 293 | `s_itemall` |
| `platform/pc/src/inv_render_pc.c` | 306 | `s_itps` |
| `engine/src/itps_common.c` | 45-48 | fauler Lader `re15_itps_load` |
| `engine/src/item_icon_common.c` | 74-77 | fauler Lader `re15_itemall_load` |

| Konstante | Wert | Herkunft |
|---|---|---|
| Blockgröße | 0x3000 | `itps_common.c:16`; Lader LAB_8001e404: `sll v0,a0,1` / `addu v0,v0,a0` / `sll v0,v0,1` @0x8001e450-58 = id·6 Sektoren = id·0x3000 |
| Block-Offset | 0x40 · 0x3000 = 0xC0000 | Item-Id 0x40, Namenstabelle @0x800C49DC |
| Tile-Offset | 0x40 · 1200 = 0x12C00 | `item_icon_common.c:14`, Tile = Id |
| Icon-Kopie im Block | +0x21A0 | 66 von 72 Blöcken dort bytegleich mit ihrem Tile (§3.5) |
| crect | (0,489) 256x1 | Kopf der 53 RE1.5-Blöcke; Fenster des Foto-Laders @0x800c0258 |
| prect | (832,256) 56x72 | dito |
| Hintergrundwort | 0x1C00 | 5244 von 7344 Innenpunkten des Blocks 0x40 |
| Rahmen | oben/links: außen 0x739C, innen 0x4210; unten/rechts: innen 0x739C, außen 0x4210 | Block 0x40 Zeile/Spalte 0 und 1 bzw. Spalte 110/111 und Zeile 70/71; wird punktgenau aus dem ausgelieferten Block übernommen |
| Icon-Hintergrund | Indizes 0xE3/0xE4/0xE6 | einzige blau-dominante Indizes in den Ecken aller 72 Tiles |

* **Riegel:** Unit-Test: nach dem Einsetzen in eine Kopie der ausgelieferten Dateien sind in
  ITPS.ITP nur Bytes in 0xC0000..0xC2FFF verändert (gemessen 8904) und in ITEMALL.PIX nur
  0x12C00..0x130AF; `re15_itps_pixel(0x40, 0, 0)` = (224,224,224) (Rahmen 0x739C),
  `re15_itps_pixel(0x40, 4, 4)` = (0,0,56) (0x1C00).
* **Abnahme-Messung** (Prototyp über eine Mess-Wurzel gefahren, `RE15_CD_ROOT =
  build/r30_sicherung/cd_weg2`, shared_assets unberührt):
  Modal `proto_weg2_normal_spiel.png` F240/F300; Inventar-Raster und CHECK
  `inventar_bestand_gegen_weg2.png`; CHECK-Fotofeld 14 166 Punkte (Bestand 0).

### 5.5 Weg 1 und Weg 2 im Vergleich

| | Weg 1 — Welt-Prop an das Item-Bild anpassen | Weg 2 — Bild + Icon aus dem Modell |
|---|---|---|
| Was sich ändert | `gen/sicherung_prop.inc` trägt RE2s Weltmodell „Fuse Case" (ROOM60D0.RDT MD1 @0x0ED8 1356 B, TIM @0x1F384 26144 B) | zwei eingebackene Bilder + sechs Einsetz-Aufrufe |
| Belegt | Item-Bild = RE2 0x4D bytegleich; RE2 führt genau dieses Weltmodell dazu | Modell = ROOM1050-Kunst (Abnahme `sicherung_abnahme.py`); Bildformate, Rahmen, Hintergrund, Lage/Länge im Bild |
| Erfunden | Sitz und Drehung im Fach | Sitz und Drehung im Fach; Blickneigung 25°, Lichtrichtung (-0,45,-0,70,0,55), Umgebung 0,45 / Streuung 0,75 des Item-Bilds |
| Widerspruch | zur Vorgabe des Nutzers („genau DIESE Sicherung"); zur ROOM1050-Kunst: der Sockel in Cut 7/8 hält ein Rohr 5,7 : 1, das Fuse Case ist 3,1 : 1 | Item 0x40 zeigt nicht mehr das ausgelieferte Bild |
| CHECK-Foto | bleibt **leer** (Block im RE2-Format), braucht zusätzlich einen Eingriff | **erscheint** (Block im RE1.5-Format) |
| Im Spiel gemessen | `proto_weg1_spiel.png`: 279–937 Punkte, Mittel RGB (33,30,29), ragt bei geschlossenem Deckel heraus (F100: 279) | `proto_weg2_normal_spiel.png`: 211–439 Punkte, Mittel RGB (100,107,101), F100: 0 |
| Aufwand | klein (Modellbytes tauschen), plus CHECK-Eingriff | mittel (Generator steht, Prototyp steht) |

**Empfehlung: Weg 2.** Der Nutzer hat den Maßstab genannt — das Modell. Die
Hintergrundkunst des Raums, in den die Sicherung gehört, zeigt denselben Gegenstand. Und das
ausgelieferte Bild von Item 0x40 ist nachweislich unfertig eingebunden (§3.5).

## 6. Risiken / offene Fragen

1. **Zeichenreihenfolge.** Der Port sortiert Dreiecke nach mittlerer Tiefe, nicht je Pixel.
   Für den Sitz im Kuppelfach gemessen: 211–439 Punkte im Spiel gegen 373–461 nach wahrer
   Verdeckung — kein Stichbild, in dem die Sicherung wegsortiert wird. Das gilt für **Cut 4**;
   ein anderer Blick auf den ausgefahrenen Tisch existiert im Skript nicht.
2. **Das Modal kommt in der Fahrt, nicht im Stillstand.** Schranke -1100 → Bild 134, die
   Plattform steht erst ab Bild 156 (`Sleep 30` @0x101A). -1100 und -5000 sind Port-Schranken
   ohne Adresse. Wer das Modal in den Stillstand legen will: Bedingung „y == -1205 und im Bild
   davor -1206" (= -305 @0x0FB4 + 91·(-10) @0x0FF2/@0x0FF6 + 10·(+1) @0x100C/@0x1010) trifft
   genau Bild 155. Nicht Teil des Auftrags, nicht gebaut, nicht als Prototyp gefahren.
3. **Antwort „No".** `s_modal_ausgeloest` wird nur beim Raumeintritt zurückgesetzt. Ob ein
   zweites Auslösen des Tischs im selben Aufenthalt die Sicherung noch einmal anbietet, ist
   **nicht gemessen**.
4. **CHECK-Foto leer bei 19 Items** — Nebenbefund außerhalb des Themas: alle Blöcke im
   RE2-Format (0x1C Remote Detonator, 0x24–0x2E alle „Medicine", 0x34 Minidisc Player, 0x35
   Timer Bomb, 0x3C Enzyme, 0x3E G-Vaccine, 0x40 Fuse, 0x41 Spark Plug, 0x44 Minidisc Player
   w/ Disc). Gemessen für 0x40 und 0x24 (0 Punkte im Fotofeld). „Leer" gilt für den ersten
   CHECK einer Sitzung; nach dem CHECK eines anderen Items bleibt dessen Foto stehen
   (`inv_render_pc.c:361-363` „leave the windows STALE") — das ist **nicht** gemessen.
5. **PSX-Ziel.** `platform/psx` lädt weder ITPS.ITP noch ITEMALL.PIX über diese Stellen; die
   sechs Einsetz-Aufrufe decken PC, Linux und Android (dieselben Quellen), nicht die PSX.
6. **GUI-Tests unter Last.** Die Integrations-Pins mit echter exe flattern, wenn parallel
   Agenten bauen (Memory `gui-tests-flattern`). Einzeln fahren.
7. **Messfalle dieser Runde bestätigt:** `gdigrab` über den Fenstertitel hat ab `g_11.png`
   das Fenster eines anderen Agenten aufgenommen (`gdigrab_uebersicht.png`, Elza-Intro).
   Alle Zahlen dieses Dossiers stammen deshalb aus `RE15_FRAMEDUMP`.
8. **ROOM1050-Ende.** Steht vollständig aus (§2.8). Mit der Fundstelle im Hebetisch ist die
   Voraussetzung des Vor-Dossiers („Gate und Fundstelle gehören zusammen") jetzt erfüllt; der
   Bau braucht noch: Bedingung am Schalter, `Message_on 2`, Einsetz-Handlung mit Verbrauch
   des Gegenstands, ein Zustands-Bit, `Cut_replace 7,8` samt Wiedereintritt. Der Riegel
   `unit_room1050_sicherung` muss dann mitgezogen werden.

## 7. Was NICHT belegt ist

* **Sitz, Drehung und Modal-Zeitpunkt der Sicherung im Hebetisch** — das Original hat dort
  keinen Gegenstand. Die Werte sind aus der ausgelieferten Geometrie abgeleitete Port-Wahl.
* **Blickneigung und Licht des Ersatz-Item-Bilds** — ein Item-Bild der Rohr-Sicherung hat es
  nie gegeben.
* **Was der Kasten mit Schlüsselloch auf dem Fachboden bedeutet.** Er ist gemalte Textur; ob
  das Original dort einen Schlüssel oder einen Gegenstand vorsah, ist NICHT BELEGT. Kein
  Skript des Raums greift darauf zu.
* **Wozu sub04 im Original diente.** Der Auslöser ist abgeschaltet (`sce=0` @0x0D7E), die
  Szene endet ohne Wirkung.
* **Dass FUN_8001d22c der LOAD-Zweig ist** — übernommen aus `main.c`, nicht selbst
  nachdisassembliert; belegt ist nur, dass FUN_800396fc genau zwei Aufrufer hat.
* **Ob der Nutzer über Tür oder über LOAD in den Raum kam.** Beide Wege sind gemessen; am
  Türweg ist die Sicherung 12–124 Punkte groß, am Lade-Weg fehlt sie ganz.
* **Verhalten des Original-Statusschirms bei Blöcken im RE2-Format** — aus dem Lader
  @0x800c0258 abgeleitet, nicht auf der Original-Disc beobachtet.

## Anhang — Artefakte

Werkzeuge (`analysis/befunde_runde30/sicherung_werkzeug/`): `lauf.sh`, `lauf_laden.sh`,
`lauf_laden2.sh`, `lauf_inventar.sh`, `sichtbar_im_spiel.py`, `farbe_im_spiel.py`,
`sitz_zeitplan.py`, `md1_zordnung.py`, `md1_normalen_wenden.py`, `normalen_pruefen.py`,
`viereck_stichprobe.py`, `re2_gleichheit.py`, `re2_itps_gleichheit.py`, `itps_koepfe.py`,
`itps_hintergrund_40.py`, `itps_platte.py`, `icon_platte.py`, `achse_altes_bild.py`,
`vergleich2.py`, `cd_weg2_bauen.py`, `kontaktbogen.py`, `dis_debugbin.py`, `item_namen.py`,
`item_bilder.py`; vom Vorgänger mit falschem Deckelweg: `sitz_analyse.py`, `sitz_kandidaten.py`.
Generator: `re15_port/tools/sicherung_itembild.py`.
Sonden: `re15_port/tests/unit/probe_r30_sicherung_variante.c` (Mess-Variante der exe),
`probe_r30_sicherung_fahrt.c` (Zeitplan), `probes/r30_sicherung.cmake` (beide nur mit
`-DRE15_R30_SICHERUNG_VARIANTE=ON`, kein `add_test`).
Bilder (`build/r30_sicherung/`): `uebersicht_bestand_weg2_weg1.png`,
`proto_weg2_normal_spiel.png`, `proto_weg2_normal_zoom.png`, `proto_weg1_spiel.png`,
`inventar_bestand_gegen_weg2.png`, `check_foto_gegenprobe.png`, `weg2n_itps_40_4x.png`,
`weg2n_icon_40_8x.png`, `weg2z_vorher_nachher.png`, `nebeneinander.png`,
`vergleich_tuerweg_ladeweg.png`, `viereck_ordnung_im_spiel.png`, `sitz_zeitplan.png`,
`ab_mit_ohne_zoom.png`.
