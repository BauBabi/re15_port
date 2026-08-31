# GEGENPRUEFUNG B6 (Rolle: Skeptiker) — "ROOM1170 auf Band 0 nach Seite 4 Rect 3 umschalten"

Datum 2026-08-31. Alles unten ist selbst gemessen: PSX.EXE-Bytes, MAP0x.PIX-Pixel,
ROOM*.RDT-Kollisionszellen und Laeufe gegen den ECHTEN Port-Code
(`re15_port/build/engine/libre15_engine.a`). Keine Zahl aus B uebernommen.
Sonden im Scratchpad: `s.c` (RDT-Baender + Zonen + Marker), `sim.c` (1:1-Nachbau von
`zone_index_at`), `s3.c` (Marker-Projektionen), `s4.c` (Kollisionszellen je Band),
`s5.c` (Op-Liste des Karten-Schirms), plus zwei Python-Renderer.

**ERGEBNIS: WIDERLEGT. Empfehlung = SEIN LASSEN.**
Der Vorschlag schaltet nachweislich am falschen Ort um, kippt 6 heute gruene
Test-Zusicherungen, verliert alle 7 Tuer-/Treppen-Marken des Bereichs und beschriftet die
UNTERSTE Ebene mit "3F".

---

## 0. Was ich unabhaengig BESTAETIGT habe (damit klar ist, was NICHT das Problem ist)

| Behauptung aus B | selbst geprueft | Ergebnis |
|---|---|---|
| Paartabelle @0x80076840: Seite 4 = 7 Rects @0x80076468, Seite 5 = 2 Rects @0x800764bc | PSX.EXE-Bytes, off(a)=a-0x80010000+0x800 | **stimmt**, alle 9 Rect-Tupel identisch (S4r3=(152,89) 48x24 uv(192,16); S5r0=(140,80) 48x24 uv(192,16)) |
| S4r3 und S5r0 sind dieselbe Zeichnung | MAP05.PIX/MAP06.PIX, 4bpp, 128 B/Zeile, Fenster uv(192,16) 48x24 | **stimmt**: 22/1152 Pixel abweichend, **alle** Paar (4->1); Silhouetten-Differenz **0** |
| L-Form: linker Block x0..17/y0..21, unterer Arm x0..41/y14..21, y22/23 + x42..47 leer | Pixel-Belegung Zeile/Spalte aus MAP06.PIX | **stimmt exakt** |
| Baender von ROOM1170 (0/2/3/4) und ihre Bboxen | `re15_rdt_parse` + SCA-Gruppe 0 | **stimmt exakt**: B0 x[-28622,-17876] z[-28841,-17070]; B2 x[-28900,-22300] z[-28160,-22885]; B3 x[-9632,11410] z[-9128,15640]; B4 x[-21946,15658] z[-28734,17746] |
| Marker-Kachelbahn ueber die Treppe (41,19)->(19,17)->(13,18)->(7,12)->(7,7) | echter Port-Code `re15_map_zone_marker` | **stimmt exakt**, und **alle 5 Punkte liegen AUF dem gezeichneten Grundriss** |

Der IST-Zustand ist also gut. Genau das macht die Aenderung teuer.

---

## (a) Wird der Marker beim Seitenwechsel mitgezogen?

**Der fruehere Pruefer hat halb recht — aber seine Schlussfolgerung ist falsch.**

* An der Zeichenstelle `re15_inv_screen.c:1509-1511` steht **keine** Seiten-Abfrage: der
  Marker-SPRT wird bedingungslos aus `st->map_marker_x/y` emittiert.
* Die Wache sitzt vorher, in `re15_inv_map_marker`:
  `re15_inv_screen.c:334` `if (zn && zn->page == re15_inv_map_page_shown())` und
  `:356` `if (zn && zn->page != ...) { *mx = -64; *my = -64; return; }`.
* **Diese Wache ist heute eine Tautologie.** `re15_inv_map_page_shown()` (`:229-233`)
  liefert `re15_map_zone_current()->page` — also genau den Wert, gegen den verglichen
  wird. Gegenprobe ueber die ganze Tabelle: **0 von 200** Zonen-Zeilen haben eine Seite,
  die von `page_of(room)` abweicht (Skript ueber `re15_map_zones.h` + der Seiten-Switch
  aus `gen_map_zones.py:50`). Die Wache kann heute gar nicht feuern.

Folge fuer B6: der Marker bleibt **nicht** auf dem alten Rechteck stehen — er folgt
korrekt, **solange** `re15_map_zone_current()` steht. Fuer den einen Pfad, in dem sie
nicht steht (Rueckfall auf `re15_map_zone_at`, `:330`), wuerde B6 die Wache zum
allerersten Mal scharf machen, und ihr einziger Effekt ist `*mx=*my=-64`, also
**Marker weg**. Der Datei-Kommentar `re15_map_zones.c:150-160` protokolliert genau
dieses Symptom ("Der Spielermarker steht im nirgendwo auf der Karte") als schon einmal
passiert. B6 baut den einzigen Weg dorthin neu auf.

`g_inv_screen.map_page` wird bei `re15_inv_screen_open()` (`:166`) und bei `map_entry()`
(`menu_common.c:156`) eingefroren, der Marker rechnet live — beide aus derselben Zone,
deshalb heute deckungsgleich. Ein Seitenwechsel *waehrend* des offenen Schirms ist
unmoeglich (Spiel steht). Insoweit: kein Problem.

## (b) Besucht-Bit und Marken, wenn ein Ort auf zwei Seiten erscheint

1. **Marken.** Gezaehlt aus `s_map_marks[]`: **Seite 5 Rect 0 traegt 7 Marken**
   (3 Tueren + 4 Treppen, alle `zid 27`), **Seite 4 Rect 3 traegt 0**. Wandert Band 0
   auf S4r3, zeigt genau der Schirm, den die Aenderung verbessern soll, **keine einzige
   Treppe und keine Tuer** — also die Rueckkehr des Nutzer-Reports "Treppen und Tueren
   werden garnicht angezeigt". Die Marken lassen sich nicht von Hand nachziehen: sie
   werden in `gen_map_zones.py:392-422` aus der Zonen-Bbox projiziert, und der Generator
   kann eine Zone gar nicht auf eine **fremde** Seite legen (`by_page[page_of(b)]`,
   Zeile 228-233).
2. **Besucht-Bit.** `zid` wird in `gen_map_zones.py:328-335` **fortlaufend** vergeben
   (heute 100 Zonen, zid 0..99). Eine Zone einfuegen und/oder ROOM1140-Zone-1 entfernen
   **verschiebt jede spaetere zid**. `re15_savedata.c:157/220` schreibt und liest
   `visited[32]` **roh**; `RE15_SAVE_VERSION` ist 7 und wuerde durch eine
   Header-Neugenerierung nicht steigen. Alte v7-Staende faerben danach **fremde Raeume**
   gruen. Stiller Datenfehler.
3. **Doppelter Zustand fuer einen Ort.** Der Tuersatz nach ROOM1130 liegt bei
   lokal(-21230,-17925) — das ist **innerhalb der Band-0-Bbox** (Zelle
   x[-28622,-17876] z[-18272,-17120]). Betritt der Spieler 1170 von 1130 aus, wird nur
   die neue Band-0-Zone besucht; S5r0 bleibt schwarz, obwohl er koerperlich in dem
   Bereich stand, den S5r0 zeichnet. Umgekehrt zeigt S5r0 heute schon den linken Block
   (= die Unterebene) als "besucht", sobald man den Korridor betritt. Ein Ort, zwei
   Bits, die nie zusammenpassen.

## (c) Faellt ein bestehender Test?

`re15_port/tests/unit/test_map_re2_system.c` laeuft heute **komplett gruen**
(30 CHECKs, Exit 0 — selbst gebaut und ausgefuehrt). B6 kippt davon:

| Zeile | CHECK | warum es faellt |
|---|---|---|
| 188-189 | "der kleine Bereich liegt auf Seite 5 Rect 0 (NICHT auf Seite 4)" | Testpunkt `re15_map_zone_update(0x1170,-18000,-22000)` liegt **in** der Band-0-Bbox -> neue Zone -> S5r0 nicht mehr CURRENT |
| 190 | "Seite 4 Rect 3 gehoert weiter ROOM1140, nicht 1170" | S4r3 wird CURRENT fuer 1170 — **wortwoertlich** das, wogegen der Pin geschrieben wurde |
| 192-193 | "nach dem Betreten sind Marken sichtbar" | zaehlt nur Marken mit `pg==5`; zid 27 bleibt unbesucht -> 0 |
| 106-107 | "nach dem Bereichswechsel ist Zone 1 aktuell (rot)" | nur wenn ROOM1140-Zone-1 entfernt wird (Teil des Vorschlags) |
| 108-109 | "und Zone 0 faellt auf besucht (gruen) zurueck" | dito |
| 194-197 | "TUEREN/TREPPEN sind eingezeichnet" | dito, gleicher Zaehler |

**Der Pin in Abschnitt (7) ist ausdruecklich die Anti-Regression zu v0.3.69** und nennt
die beiden Fehler beim Namen (Kommentar Zeile 168-177). B6 ist dieselbe Aenderung mit
kleinerem Radius.

*(Nebenbefund, unabhaengig von B6: der Reihenfolge-Pin in Abschnitt (8) ist wirkungslos.
Er sucht `RE15_INV_OP_LINE` als "erste Marke", die Marken werden aber als
`RE15_INV_OP_FILL` emittiert (`re15_inv_screen.c:1457`). Gemessen mit `s5.c`: op[26] ist
`kind=1` bei (26,129) rgb(32,255,32) = die EKG-Linie, keine Marke. Der Pin gegen den
v0.3.69-Zeichenreihenfolge-Fehler haelt also nichts.)*

## (d) Zweiter Raum, in dem dieselbe Regel etwas Falsches tut — JA, mindestens zwei

Kollisionszellen selbst geparst (`s4.c`):

* **ROOM1120** (Seite 4, direkter Nachbar): Baender **0, 1 und 2** haben **byte-identische
  Zellenlisten** — dieselben 13 Zellen, dieselbe Bbox x[-10550,8650] z[-7800,13200].
  Eine Regel "Band wechselt -> Karte wechselt" haette dort **keinerlei** raeumliche
  Bedeutung.
* **ROOM10A0** (das Treppen-Raum-Beispiel): **5 Baender** (1,3,4,6,8), alle auf demselben
  Grundriss x[17850,29100] z[15550,28300].
* **ROOM10B0**: 3 Baender mit Ueberlappungen bis 24450x6930.

**Und in ROOM1170 selbst traegt das Band die Etage nicht:** Band 4 umfasst **beides**,
die Hof-Waende (x bis +15658) **und** den oberen Korridor; der Hof-BODEN ist Band **3**,
liegt also numerisch **zwischen** Absatz (2) und Korridor (4). "Band 0 = untere Etage"
ist ein Zufall der x/z-Trennung, keine Semantik des Bytes.

## (e) Der Kern: die Umschaltung feuert am FALSCHEN Ort

`zone_index_at` (`re15_map_zones.c:68-96`) waehlt rein ueber **Bbox + kleinste Flaeche**
(`ZONE_SLACK 1500`), es gibt **kein** Band-Feld im Zonen-Datensatz
(`re15_room.h:44-54`). Ich habe die Funktion 1:1 nachgebaut, gegen den echten Port-Code
validiert (IST-Spalte identisch) und mit der vorgeschlagenen Tabelle gefahren:

```
                             IST            B6 (Zone 1 unveraendert   B6-Variante
                                             + Band-0-Zone)          (Zone 1 gekuerzt)
Selbsttuer-Ankunft (B4)   S5 r0          S5 r0                    S5 r0
Treppe Band 4             S5 r0          S4 r3   <-- ZU FRUEH      S5 r0
Treppe Band 2 (a)         S5 r0          S4 r3   <-- ZU FRUEH      S5 r0
Treppe Band 2 (b)         S5 r0          S4 r3   <-- ZU FRUEH      S5 r0
Treppe Band 0             S5 r0          S4 r3                    S4 r3
Testpunkt Pin(7)          S5 r0          S4 r3   <-- Test kippt    S5 r0
```

Grund: die Band-0-Bbox **ueberlappt** die Band-4-Korridor-Bbox um 4070x11664
Welteinheiten, und sie ist die kleinere (skalierte Flaeche 30561 gegen 57462), gewinnt
also. Der Kartenwechsel passiert **oben an der Treppe**, nicht "beim Runterlaufen" —
genau der Fehler, den `gen_map_zones.py:345-356` am 2026-08-31 schriftlich
zurueckgenommen hat ("Die Umlegung schaltete die Karte schon beim Betreten des kleinen
Raums auf die untere Etage"). Dort steht auch der richtige Weg: *"die Umschaltung muss
am BAND haengen, nicht an der Zone"* — und das Band gibt es im Zonen-Modell nicht.

Auch die naheliegende Reparatur "Zone 1 auf Band 2+4 kuerzen" ist gemessen schlechter:
sie schiebt die **verbleibenden** Marker-Positionen (Bbox-Streckung von d=11771 auf
5849) und legt die Treppe Band 4 auf Kachel (19,**11**) — dort ist der Grundriss
**leer** (Zeile 11 belegt nur x0..17). Der Marker saesse auf Schwarz. Heute liegen alle
5 Punkte auf dem Grundriss.

## (f) Und selbst wenn die Umschaltung praezise waere: das Ziel-Rechteck passt nicht

* **Beschriftung.** Titelkachel uv(0,0) 88x32 selbst gerendert:
  MAP05.PIX (Seite 4) = "POLICE STATION **3F**", MAP06.PIX (Seite 5) = "POLICE STATION"
  **ohne Etage**. Der Vorschlag beschriftet damit die **unterste** Ebene mit **3F** —
  beim **Hinunter**gehen. (Und wenn "3F" fuer Band 0 richtig waere, weil die Tuer dort
  nach ROOM1130/3F fuehrt, dann ist es fuer Band 4 genauso richtig, weil dessen Tuer
  nach ROOM1140/3F fuehrt — dann gehoert der **ganze** Bereich auf Seite 4, also exakt
  die zurueckgenommene v0.3.69-Aenderung. Die Halbierung ist in sich unschluessig.)
* **Groesse.** Am festen Kartenmassstab (sx~2280/2^20): Band 0 allein ist
  10746 x 11771 Welteinheiten = **23,4 x 25,5 Kartenpixel**, S4r3 ist **48x24**.
  Das ist eine **x-Streckung um 2,05** und ein Groessen-Delta von **26 px** — schlechter
  als jeder Kandidat, den B selbst durchgerechnet hat: derselbe Rect passt zum GANZEN
  2. Bereich mit Delta **6** und zu ROOM1160 mit Delta **16** (ROOM1160-Bbox selbst
  nachgemessen: 16750x13100 = 36,6x28,6 px). **B6 vergibt r3 an den schlechtesten der
  drei Bewerber.**
* **Folge der Streckung:** die Spalten 42..47 und die Zeilen 0..13 rechts von x=17 sind
  in der Kachel **leer**. Mit der Band-0-Bbox erreicht der Marker rechnerisch Kachel-x
  bis 44 (Klemmung `rx+rw-4`) — er wandert also in den **unteren Arm**, der laut B
  selbst der **obere Korridor** ist, und in schwarze Flaeche.

## (g) Restpunkte des Vorschlags

* "Rect 3 wird durch Entfernen des Artefakts ROOM1140-Zone-1 frei": Die Bbox-Enthaltung
  stimmt (Zone 1 x[-5250,7450] z[-17450,-11350] liegt komplett in Zone 0
  x[-10600,16700] z[-25600,1850]) und ROOM1140 hat gemessen **nur ein** Band (0x03,
  1 Band, 0 Ueberlappungen). Das ist ein eigenstaendiger, plausibler Fix — **aber er
  gehoert nicht in diesen Vorschlag** und kippt fuer sich schon 2 Test-CHECKs.
* "ROOM1130 bekaeme Rect 4": ROOM1130 fehlt tatsaechlich in der Tabelle und r4/r0 sind
  auf Seite 4 unbelegt (Rect-Nutzung gemessen: Seite 4 belegt nur {1,2,3,5,6}). Aber
  "bekaeme" ist keine Handlung: die Zuordnung entsteht aus einem **randomisierten
  Hill-Climbing** (`gen_map_zones.py:256-320`, `random.Random(9000+pg)`, 80 Neustarts).
  Von Hand ins generierte Header schreiben heisst, dass der naechste Generatorlauf es
  still zurueckdreht — die Hand-Tabelle `FLOOR_FIX` wurde am 2026-08-31 aus genau
  diesem Grund geleert.

---

## EMPFEHLUNG (genau eine): **SEIN LASSEN**

Der IST-Zustand ist gemessen richtig: der Marker laeuft ueber die gezeichnete
Wendeltreppe vom rechten Ende des unteren Arms (41,19) in den linken Block (7,7), alle
fuenf Treppen-Punkte liegen auf dem Grundriss, die 7 Tuer-/Treppen-Marken sitzen dort,
und das Original zeigt fuer Raum 23 grundsaetzlich Seite 5 (@0x8004b75c-68). B6 tauscht
das gegen einen Wechsel am falschen Ort, eine falsche Etagenbeschriftung, 0 Marken,
6 kippende Test-Zusicherungen und eine stille Save-Inkompatibilitaet — und wiederholt
eine bereits schriftlich zurueckgenommene Aenderung.

Falls der Nutzer beim Abstieg trotzdem eine sichtbare Rueckmeldung will, muss **zuerst**
gemessen werden, was das Original mit `entity+0x82` in der Karte tut (Antwort aus B §4b/4c,
von mir nicht gegengeprueft: **nichts** — weder Band noch Y werden im Kartencode gelesen).
Danach waere die einzige Aenderung, die keine der obigen sechs Nebenwirkungen hat, eine
**innerhalb** von S5r0: Band 0 bekommt keine eigene Seite, sondern einen eigenen
Marker-Teilbereich bzw. eine eigene Hervorhebung. Das ist ein anderer Vorschlag und
braucht seine eigene Pruefung.
