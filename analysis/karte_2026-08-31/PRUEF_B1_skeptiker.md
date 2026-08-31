# GEGENPRUEFUNG B1 (Rolle: Skeptiker) — "Seite 5 r0 IST der 2. Bereich von ROOM1170"

Datum 2026-08-31. Alles unten ist SELBST gemessen (EXE-Bytes, PIX-Pixel, RDT-Bytes,
eigener MIPS-Dekoder). Keine Zahl aus B_kartenkacheln.md uebernommen.
Sonden: `scratchpad/chk/{rects,tile,sca,sca2,sca3,dis,proj,shape,bands,aot,aot2,search}.py`

## VERDIKT

**refuted = true** — aber gezielt: **die BEGRUENDUNG faellt, die SCHLUSSFOLGERUNG haelt.**
Dass r0 den 2. Bereich zeigt, konnte ich NICHT widerlegen (und ich habe aktiv gesucht).
Widerlegt sind die Saetze, mit denen B1 es *belegt*:
"43,9 x 25,7", "Delta 5,9 px", "bester Treffer aller 119 Rechtecke" — und die
Ueberschrift "geometrisch bewiesen".

---

## 1. WAS ICH UNABHAENGIG BESTAETIGT HABE (Adresse/Offset je Zeile)

| Behauptung | mein Befund | Beleg |
|---|---|---|
| EXE-Anker `off(a)=a-0x80010000+0x800` | t_addr `0x80010000` @0x18, t_size `0xaf000` @0x1C = Auslieferungsstand (nicht das gepatchte Image) | `xxd -l 64 info/Re1.5/PSX.EXE` |
| Skalenzeile 23 = ox100 oy206 sx2280 sy2268 | **exakt** `64 00 ce 00 e8 08 dc 08` | Datei-Offset **0x67168** (=0x670b0+23*8) |
| Paartabelle, 119 Rechtecke gesamt | 14 Eintraege, Summe **119** | 0x67040; Seite 4 ptr 0x66c68 count 7, Seite 5 ptr 0x66cbc count 2 |
| Seite 5 = r0 (140,80) 48x24 uv(192,16), r1 (148,101) 48x56 uv(0,32) | **exakt** | 0x66cbc |
| Kachel uv(192,16): linker Block x0..17 y0..21, Arm bis x41 y14..21 | **exakt**; Spalten 42..47 und Zeilen 22..23 leer, 588/1152 nonzero | MAP06.PIX (32768 B = 256x256 4bpp, kein Header) |
| Kollision @0x0a80, ceil (55888,51982), 5x30 | **exakt** | ROOM1170.RDT+0x20 |
| comp1 = x[-28900,-8784] z[-28841,-17070], Ausdehnung 20116 x 11771 | **exakt**, stabil fuer GAP 0..5000 | eigene Union-Find ueber die 30 Zellen |
| 4 Treppen-AOTs, chain {0,2,2,4} | **exakt** gefunden: mainScd @0x1337a/0x1338e/0x133a2/0x133b6, Zentren (-25670,-20645) (-25640,-23425) (-23295,-26155) (-20690,-25595) | Aot_set 0x2C, Zentrum = rect_x+w/2 |
| Selbst-Tuer (2550,15250) -> (-11710,-26500) Raum 0x17 | **exakt** @0x1206; ferner -> 0x13 (ROOM1130) @0x12f8 und -> 0x14 (ROOM1140) @0x1338 | Door_aot_set 0x3B |
| Seite4-r3 vs Seite5-r0: 22 Pixel, alle 4->1, Silhouetten identisch | **exakt** 22; Silhouetten-Differenz **0**; x10..15, y{1..5} und {16..20} | eigener Pixelvergleich |
| **Nur Raum 23 zeigt Seite 5** | **bestaetigt**: Fall-Through-Switch, case23 @0x8004b75c `j 0x8004b888` + `ori v0,zero,5`; case22 springt vorher weg, also faellt nichts in case23 hinein | Sprungtabelle @0x8001103c, eigener Dekoder |
| Markerformel | selbst dekodiert; **B1s Auszug ist unvollstaendig**: `addiu v1,v1,32000` @0x80047480 (Z-Zweig) fehlt dort | 0x800473f8-0x80047528 |

Die Formel selbst reproduziert die beiden "bewiesenen" Paare exzellent — mit meinem Code, nicht ihrem:
* ROOM1150 (Zeile 21) -> x[118,150] y[81,121] gegen Seite4-r2 x[120,152] y[80,120] — **<=2 px**
* 1170-Hof Band 3 (Zeile 23) -> x[149,194] y[103,157] gegen Seite5-r1 x[148,196] y[101,157] — **<=3 px**

Damit ist die Formel als Messinstrument geeicht. Das macht Punkt 3 unten schwer.

---

## 2. WAS FALSCH IST

### 2a. Die Groessenzahl ist mit den FALSCHEN Konstanten gerechnet
comp1 am **echten** Massstab von Raum 23 (sx=2280, sy=2268):
`20116*2280/2^20 = 43,74` x `11771*2268/2^20 = 25,46`.
B1s "43,9 x 25,7" entsteht nur mit **sx=sy=2290** (dem gerundeten Mittelwert aus §7) —
obwohl §6e woertlich "px/Einheit = 2280/2^20 bzw. 2268/2^20" behauptet.
Delta gegen 48x24 ist damit **5,72**, nicht 5,9.

### 2b. Verglichen wird gegen die falsche Groesse
Von r0 sind **6 der 48 Spalten und 2 der 24 Zeilen komplett transparent** (selbst gemessen).
Gezeichnet ist **42 x 22**, nicht 48x24. Gegen die gezeichnete Ausdehnung ist das Delta
**5,20** — und das Vorzeichen kippt (Zeichnung schmaler als comp1 statt breiter).

### 2c. "bester Treffer aller 119 Rechtecke" — WIDERLEGT
Ich habe alle 119 Rechtecke aller 13 Seiten gerastert (`search.py`):

* Am **nominellen** Mass ist es ein **DREIER-GLEICHSTAND** bei d=5,72:
  `Seite4 r3`, `Seite5 r0`, `Seite12 r4` — kein "bester", ein Patt.
  Schlimmer: Seite4-r3 ist **dieselbe Zeichnung** (22 Pixel Unterschied, Silhouette identisch).
  Das Groessenmass kann r0 und r3 also **prinzipiell nicht** unterscheiden — es waehlt
  eine GROESSE, kein RECHTECK. Als Identifikationsbeleg hat es null Trennschaerfe.
* Am **gezeichneten** Mass ist r0 **nicht einmal vorne**:
  `Seite3 r5` (48x32, gezeichnet 44x22) d=**3,72** und `Seite8 r0` (48x32, gezeichnet 44x30)
  d=**4,80** schlagen r0 (d=5,20). Die Aussage "bester Groessentreffer aller 119 Rechtecke
  des Spiels fuer diese Flaeche" ist damit **falsch**.

### 2d. Die AOT-Tabelle in §6e ist ZIRKULAER
B1s Kachelpixel (41,19) (19,17) (13,18) (7,12) (7,7) habe ich exakt reproduziert —
**aber nur** mit der **Port**-Abbildung `(X-x0)/(x1-x0)*48`, die comp1s Bbox per
Konstruktion auf das 48x24-Rechteck streckt. Unter dieser Abbildung landen die Treppen
in JEDEM Rechteck proportional "an der richtigen Stelle"; sie kann die Identifikation
nicht pruefen, sie setzt sie voraus. (B1 nennt die Quelle in Klammern, verkauft das
Ergebnis aber als Beleg.)
Am **festen Original-Massstab** liegen dieselben AOTs bei (37,20) (18,18) (12,20) (7,14) (7,8)
— immer noch in der L-Form, die Schlussfolgerung ueberlebt also; der *Beleg* taugt nicht.

### 2e. §6e-Bandtabelle widerspricht ihren eigenen Etiketten
B1 schreibt "Band 0 -> x 0,6..24,0 ... -> **linker Block**". Der linke Block endet aber bei
**x=17** (selbst gemessen; x18..41 ist oberhalb y=14 transparent). Band 0 ragt ~7 px darueber
hinaus: Zelle 17 (x[-20000,-18200], Kachel x19,4..23,3, volle Hoehe) und Zelle 16 (Kachel
x0,6..24,0 bei y0,1..2,6) liegen bei y<14 auf **transparenten** Pixeln.
Ebenso beginnt Band 4 bei Kachel-x **15,1** (Zelle 29), nicht an der Armkante x=18.
Erklaerbar (SCA-Zellen sind Wandquader, die Zeichnung zeigt das Innere) — aber B1 fuehrt
Zahlen, die das Etikett nicht tragen, als Bestaetigung.

### 2f. Zwei Sachfehler in §6a
* "die 5 Gruppen sind die Quadranten-Partition" — **nein**: alle 5 Gruppen sind
  **byte-identische Kopien** derselben 30 Zellen (Union = 30, jede Gruppe == Gruppe 0).
  Die Zellzahlen "80"/"70" sind 5-fach vervielfacht; real sind es **16 / 14**.
* Band 4 von comp1: B1 sagt x[-21946,**-8924**] z[-28734,**-23154**]. Die Union ueber
  Zellen 26-29 ist x[-21946,**-8784**] z[-28734,**-22758**] — x-max aus Zelle 26 und
  z-max aus Zelle 28 gemischt statt vereinigt.

---

## 3. DER PUNKT, DEN B1 SELBST EINRAEUMT UND DANN WEGARGUMENTIERT

Die einzige **original-belegte** Welt->Bildschirm-Abbildung sagt das Gegenteil:
comp1 -> **x[107,150] y[174,199]** (selbst gerechnet), r0 liegt bei x[140,188] y[80,104].
**Kein Ueberlapp in y**, 94 px Abstand — bei einem Instrument, das die beiden geeichten
Faelle auf <=3 px trifft (siehe §1).
Die **Umkehrung** von r0 unter Zeile 23 ergibt Welt X[-13604,8471] Z[15158,26254] — das ist
das Gebiet direkt **noerdlich des Hofes**, nicht comp1.
Auch die Richtung stimmt nicht: comp1 liegt in Weltkoordinaten **suedwestlich** des Hofes
(kleineres x, kleineres z -> wegen der y-Spiegelung @0x800474b0 **unterhalb** auf dem Schirm),
r0 ist aber **oberhalb** von r1 gezeichnet.
B1s Ausrede ("das Original ist an dieser Stelle unfertig") ist moeglich, aber eine
**unbelegte Hilfsannahme** — und sie entwertet genau das Instrument, das sonst als Beweis dient.

## 4. WARUM DIE SCHLUSSFOLGERUNG TROTZDEM HAELT (und was der bessere Beleg waere)

Nicht die Groesse traegt, sondern eine **Ausschluss-Kette**, die B1 nur beilaeufig nennt:
1. Seite 5 wird **ausschliesslich** fuer Raumindex 23 gesetzt (@0x8004b75c, selbst disassembliert).
2. Seite 5 hat **genau 2** Rechtecke (0x66cbc).
3. r1 ist der Hof — **original-belegt** ueber die Markerformel auf <=3 px.
4. ROOM1170 zerfaellt in **genau 2** Komponenten (selbst gerechnet, stabil GAP 0..5000).

=> r0 kann nur comp1 sein, es sei denn, der Kuenstler haette auf ein Ein-Raum-Blatt einen
fremden Raum gezeichnet.
Stuetzend, nicht beweisend: comp1s Rasterung IST topologisch eine L-Form (linker Block +
Arm rechts unten), und der Tuergraph (-> ROOM1130/ROOM1140, Raumindex 19/20 = Seite 4)
erklaert die Doppelzeichnung auf dem 3F-Blatt.

WARNUNG: Silhouetten-IoU taugt hier NICHT als Mass — das bestaetige ich: die beiden
**bewiesenen** Paare streuen von IoU **0,206** (ROOM1150 -> Seite4 r2) bis **0,881**
(Hof -> Seite5 r1). comp1 -> r0 liegt mit **0,459** dazwischen, also weder Beleg noch Gegenbeleg.

## 5. WAS ZU AENDERN WAERE
* "Delta 5,9 / 43,9 x 25,7" -> **5,72 / 43,74 x 25,46** (sx=2280, sy=2268).
* "bester Treffer aller 119 Rechtecke" **streichen** — Dreier-Patt nominell, Platz 3 real.
* §6e "geometrisch bewiesen" -> "durch Ausschluss belegt (Seite5 nur Raum 23 + 2 Rects +
  r1 original-belegt + genau 2 Komponenten)"; die AOT-Tabelle als **Illustration** kennzeichnen.
* §6a: "5 identische Kopien" statt "Quadranten-Partition"; Zellzahlen 16/14 statt 80/70;
  Band-4-Bbox auf x[-21946,-8784] z[-28734,-22758] korrigieren.
