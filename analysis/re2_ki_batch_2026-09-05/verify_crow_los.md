# VERIFIKATION Paket S1 — "Kraehen haengen" / LOS-Shim (Skeptiker-Runde)

Datum 2026-09-05. Alle Adressen/Bytes dieser Datei sind in DIESER Sitzung selbst hergeleitet
(re2_disasm.py auf EMOVL21_S0.BIN @0x80100000 bzw. RE2 PSX.EXE; eigene RDT-Parses; eigenes
Code-Lesen im Port). Nichts aus dem Diagnose-Dossier ungeprueft uebernommen.

## Kurzfazit

Die BAUSTEINE des Dossiers halten der Nachpruefung stand (dec11/dec13-Exits, exec13-Hover-Stopp,
Post-Pass-Mutex, dec0 ohne LOS, Pacify nur bei totem Spieler, beide Damage-Writer ueberschreiben
+0x5). **Die ZENTRALE DEUTUNG von Ursache B faellt aber fuer genau die Raeume, in denen Kraehen
vorkommen:** in ROOM10C0 sind Klemmen-Menge und Shim-Menge bereits heute IDENTISCH (eigener
RDT-Parse: dieselben 15 Rechtecke in allen 3 Baendern und allen 5 Regionen), und in ROOM1170
klemmt die SCA die fliegende Kraehe NIE (Flugband 5/6, keine Zellen > Band 4). Der geplante
Fix (0x80050858-Ray gegen die Klemmen-Zellen) aendert in 10C0 die Kandidatenmenge also NICHT
und kann einen dort reproduzierten Haenger nicht beheben; uebrig bleibt als B-Mechanismus nur
der Radius-Keil (Klemme = 200-aufgeblasenes Rechteck, Ray = nulldick) — den hat das ORIGINAL
genauso (Ray nulldick vs. +0x90-Radius) und den beseitigt der Fix ebenfalls nicht.

---

## (a) FUN_80050858 — komplett selbst gelesen (Decompilat + Roh-Disasm)

### Record-Quelle und -Layout — CONFIRMED, mit KORREKTUR der Benennung
Eigenes Roh-Disasm @0x80050858-8f0 (RE2 PSX.EXE):
```
80050890: lw t1,32(v0)        ; v0=[0x800ce324] -> base = [room+0x20] = SCA-Kollisionsdaten
80050898: lw v0,4(t1)         ; count (u32 @header+4, INKL. Header-Block)
800508a0: sll v0,v0,4         ; * 16 Byte
800508b0: addiu t1,t1,16      ; Stride 16, erster Record @base+16 (nach 16-B-Header)
800508bc: lhu v1,8(t1)        ; ATTRIBUT/SHAPE-Feld = RECORD+0x08 (u16)
800508c4: and v0,v1,fp        ; & param_3 (0x8400)  -> 0 = Record ueberspringen
800508d0: andi v0,v1,0xf      ; SHAPE-Nibble desselben Feldes
800508dc: lbu v0,0x800a73b4[v0]; Skip-Tabelle, !=0 -> ueberspringen
```
- Die "Kollisionslinien" sind KEINE eigene Tabelle: `[DAT_800ce324+0x20]` ist dieselbe
  SCA-Kollisionstabelle, die auch der Bewegungs-/Wand-Pfad iteriert (FUN_8004c1bc liest
  identisch `*(short**)(DAT_800ce324+0x20)`; eigene greps: FUN_8004c1bc/8004c658/8004fba0).
- Record-Layout (16 B): {s16 X, s16 Z, u16 W, u16 D, u16 ATTR|SHAPE @+0x08,
  u16 QUADRANT-BITS|FLOOR-PACK @+0x0A, u32 FLOOR-BITMASKE @+0x0C}. Beleg +0x0A/+0x0C:
  FUN_80050858 param_4!=0 dekodiert +0x0A als `(>>11)*-100 + ((>>6)&0x1f)*-1800` (Ober-Y) und
  +0x0C als niedrigstes gesetztes Floor-Bit * -1800 (Zeilen 68-77 des Decompilats);
  FUN_8004c1bc filtert `(rec[+0x0A] & quadrantmask)` und `(rec[+0x0C] & (1<<entity[0x106]))`.

### 0x8400 — MASKE, nicht Mode — CONFIRMED + Semantik hergeleitet
- `and v1,fp` @0x800508c4: param_3 ist eine BITMASKE auf ATTR (+0x08). Der "Mode" ist param_4
  (0 = rein 2D, 1 = zusaetzliche Hoehenphase mit +0x0A/+0x0C). Kraehen-Root ruft mit
  a2=0x8400, a3=0 (eigenes Disasm @0x801001ac/b4) -> der Kraehen-LOS ist HOEHEN-BLIND.
- Bit-Bedeutung ueber den Bewegungs-Pfad hergeleitet: Kraehen-Root-Tail ruft
  `FUN_8003567c(self, 1024)` (eigenes Disasm @0x80100250-254) -> FUN_8004c1bc(pos,
  radius=+0x90, floorbit=1<<[+0x106], mask=0x400|0x1000000). Include-Bedingung dort:
  `(ATTR & 0x400) != 0` UND `(ATTR & 0x100) == 0` UND Floor-Bit UND Quadrant-Bits.
  => **0x0400 = "blockt die Kraehen-Bewegung", 0x8000 = zusaetzliche reine Sichtblocker.**
  Der Sicht-Ray (0x8400, ohne Floor-/Quadrant-Filter) ist damit im ATTRIBUT- und
  HOEHEN-Raum eine OBermenge der Bewegungs-Blocker — bis auf die Shape-Skip-Tabelle:
- `DAT_800a73b4 = 00 01 01 01 01 01 00 00 00 00 00 00 00 01 00 00` (selbst gelesen):
  Shapes 1..5 und 13 blocken NIE die Sicht (wohl Dreiecke/Rampen; deren
  Rechteck-Naeherung wuerde ueberblocken). Shape-0-Rechtecke + 6 (Kreis) nehmen teil —
  der Ray behandelt JEDEN teilnehmenden Record als achsparalleles Rechteck.
- Empirie RE2-Kraehenraum: ROOM1090.RDT (PL0), SCA @0x1eb0, 22 Records — ALLE tragen
  0x8400-Bits (12+2 x ATTR 0xfe80 = 0x8000|0x400|..., 8 x 0x9800 = 0x8000|0x1000|0x800).
  Die 8 0x9800er blocken NUR Sicht, nicht den Kraehenflug (kein 0x400) — das Original ist
  in seinem eigenen Kraehenraum bewusst sicht-blockiger als flug-blockig (Anti-Stall-Richtung).

### Rueckgabe-Semantik — CONFIRMED mit Praezisierung
- return 0 = KEIN maskierter Record getroffen = **Sicht FREI** (Schleifenende
  @0x800508b4 mit v0=0 im Delay-Slot; Root wertet `beq v0,zero -> ori 0x2` @0x801001c8-e8).
- return 1 = blockiert; zwei Wege: (i) ein ENDPUNKT liegt im Record-Rechteck
  (DAT_800c3b80=2), (ii) 2D-Segment kreuzt das Rechteck (DAT_800c3b80=3).
- DAT_800c3b80 ist ein Seitenkanal (2=Endpunkt-drin, 3=Kreuzung; bei param_4=1 durch die
  Vertikalebenen-Tests per XOR verfeinert). Der Kraehen-Aufruf (param_4=0) liest ihn NIE.

## (b) dec11 / dec13 / exec13 — CONFIRMED (instruktionsgenau selbst disassembliert)

- **dec11 @0x80101d30-90**: `jal 0x80015614 a3=96` (Kegel 96 auf [0x800cfc30/38] = Spieler);
  ret!=0 ODER `(+0x22A & 0x2)==0` -> `jal 0x80104088 a1=4` (Sub 4); danach IMMER
  `jal 0x80104400` (Abort-Listener). KEIN weiterer Exit, kein Wand-/Dist-Check.
- **dec13 @0x801021ec-25c**: identische Struktur mit Kegel 192; der Fail-Zweig macht
  zusaetzlich `jal 0x8010472c a1=350` (Vol) vor Sub 4. KEIN Fern-Exit: nirgendwo ein
  Dist-Vergleich (das 7195/7199-Fenster ist NUR das Arbiter-Eintritts-Fenster
  `dist-901 < 0x189B` @0x80104254-60 und wird nach dem Claim nie erneut geprueft).
- **exec13 @0x80102328-474**: claimed(+0x22A&4) -> steer(Spieler,96)+speed+4, sonst
  steer(384)+speed+12; `jal 0x8002959c` (adv); Hoehenklemme +-60 auf +0x224
  (@0x801023bc-f0); dist<0x384 -> vol(100); **dist<0x28A -> NUR Hoehenfenster +-99 ->
  sub 14, KEIN 0x80015350** (beq @0x8010241c springt ueber den Move); dist>=0x28A ->
  `jal 0x80015350` (@0x80102458). Dossier-Behauptung exakt bestaetigt.
- **exec11-Schwanz @0x80101ee8-f74**: kein Kontakt -> `jal 0x80015350`; Kontakt ->
  Hoehenfenster +-359, speed>=231 -> Sub 12, sonst Nav-Reseed (+0x220=rand&0x7f,
  +0x21F=0x8004aa50()) + Sub 8. Wand-Prober 0x80104094 laeuft NUR im Flug-DEC-Schwanz
  (eigenes Disasm @0x80101150 `jal 0x80104094` im dec4-Tail; in 11/13 nicht vorhanden).
- **Abort-Listener 0x80104400**: nur +0x22A&0x10 (Broadcast 16) -> Vol 350,
  +0x220=rand&0x9d, +0x21F=aa50(), Sub 4, +0x21C=30, `andi 0xffa7`.
- **LOS-Bit-2-Konsumenten, vollstaendiger Overlay-Scan** (alle lhu/sh auf Offset 554 in
  0x80100000-0x80104b00 disassembliert): GENAU DREI Leser von Bit 2 — dec11 @0x80101d60,
  dec13 @0x8010221c, Arbiter @0x80104284. Alle anderen 554-Zugriffe testen 0x1/0x4/0x10/
  0x20/0x80. **dec0/Wake liest Bit 2 NICHT** (@0x80100754 testet 0x1; Wake-Kette
  @0x801006c0-790: hp>=0-Kette, dist<0x709, 0x800cfbf6&4 + dist<0x1519, 0x800cfbf6&1,
  Timer +0x158 unter +0x22A&1).

## (c) Port-SCA-Klemme & Band-Logik — Dossier-KERNPRAEMISSE REFUTED (fuer 10C0/1170)

### Die Klemme ist NICHT hoehenunabhaengig
`re15_collision_constrain_enemy(&g_room_rdt, ox,oz,&nx,&nz, 200, e->y, 4)` (crow.c:1881)
-> collision_constrain_impl mit `band = re15_collision_band_from_y(e->y) = -(y/0x708)`
(re15_collision.c:186/791-798) und STRIKTEM Filter `band == (cell.floor>>4)` (:726) plus
`(mask & u0)` (:727). Eine Kraehe in >1800 Einheiten Hoehe ist Band>=1 und wird nur von
Zellen geblockt, die in diesem Band existieren.

### Der Shim laeuft auf einem ANDEREN, EINGEFRORENEN Band
`re15_los_ray_blocked` filtert `(cell.floor>>4) == e->floor` und `(cell.floor&0xf)==3`
(enemy_ai_common.c:2607-2608). Fuer die RE2-Kraehe wird `e->floor` NIE aktualisiert:
der RE2-Hook kehrt VOR dem Band-Refresh zurueck (enemy_ai_common.c:6332-6338, Refresh
`e->floor = -(y/1800)` erst :6365; kein floor-Writer in enemy_ai_re2_crow.c — eigener grep).
e->floor bleibt der Spawn-Wert `pc[4]` aus Sce_em_set (scd_vm.c ~:3370 `a->floor = t->pc[4]`).

### Aber: in den realen Kraehen-Raeumen traegt die Divergenz NICHT (eigene RDT-Parses)
- **ROOM10C0** (Spawns @0xF8C/0xFA0/0xFB4: pc4=0, y=0 — selbst gedumpt): SCA @0x360,
  225 Zellen = **DIESELBEN 15 Rechtecke** (13 typ-1 + 2 typ-3, u0=0xff, cls=3) repliziert
  in 3 Baendern x 5 Regionen (Mengen-Gleichheit selbst geprueft: band0==band1==band2,
  group0..4 identisch). Folgen:
  - Die Klemme blockt die fliegende Kraehe (Flugband 1/2: Anlauf pl_floor-2700,
    Hover -2880, Kreisen zwischen floor-1800 und floor-4800 aus dec5/dec6-Grenzen
    @0x80101300/@0x801014a0) — Ursache-B-Praemisse "Klemme greift" stimmt.
  - ABER der heutige Shim (Band 0, cls==3) testet EXAKT dieselben 15 Rechtecke.
    **"Shim meldet frei, wo die Klemme blockt" ist in 10C0 mengen-seitig FALSCH** —
    moeglich bleibt nur der Radius-Keil: Klemme = Broadphase um r=200 aufgeblasen
    (:729-730), Ray = nulldicke Segment/Diagonalen-Kreuzung. Diesen Keil hat auch das
    Original (nulldicker 0x8400-Ray vs. +0x90-Radius; im Hover Original-Radius 100 —
    der Port-Fixwert 200 verdoppelt das Fenster).
- **ROOM1170** (7 Spawns @0x1886-18fe: pc4=4, y=-7200 — selbst gedumpt): Baender 0/2/3/4,
  KEINE Zellen in Band 5/6. Flug ueber dem Pad (y ca. -9000..-12000) -> Klemme findet
  NIE eine Zelle -> move3d wird nie neutralisiert -> **B-Stall dort strukturell unmoeglich**;
  der Shim (Band 4) kann dagegen Sicht brechen — Richtung "exit nach Sub 4", kein Haenger.

### Spezifikation fuer den neuen Ray ("blockt Flug => blockt Sicht")
Damit die Implikation im PORT konstruktiv gilt, muss der Ray dieselbe Praedikat-Familie
nutzen wie die Klemme, minus Broadphase:
- Zellmenge: `(u0 & e->sca_mask)` (Kraehe: 4) — NICHT `cls==3`;
- Band: `band_from_y(e->y)` im SELBEN Tick ausgewertet wie die Klemme — NICHT `e->floor`;
- Regionen 0..3 (Ray quert Quadranten; Obermenge der Klemmen-Region quadrant_of()=0..3,
  Region 4 sieht auch die Klemme nie — quadrant_of liefert nur 0..3, re15_collision.c:42-47);
- Shape-Handhabung: Rechteck-Test fuer alle Typen oder RE1.5-Analogon der Skip-Tabelle
  (RE2 skippt Dreiecke/Rampen 1-5,13; RE1.5-Diagonaltypen 2/4/5/6/7 entsprechend behandeln).
**ACHTUNG:** Das ORIGINAL ist hoehen-blind (param_4=0, kein Floor-Filter). Ein "byte-treuer"
Alle-Baender-Ray waere in RE1.5s gestapelten Raeumen FALSCH (1170: Band-0/2/3-Zellen der
unteren Ebenen laegen in 2D unter dem Flugpfad und braechen die Sicht grundlos). Die
Live-Band-Variante ist ein MAPPING (so deklarieren), aber die einzige, die mit der
RE1.5-Band-Klemme koppelt. Rest-Divergenz danach: NUR der Radius-Keil (s.o.), den auch
das Original hat — nicht wegzuschummeln, dokumentieren.

## (d) Alternativ-Hypothesen

### 4.1 Mutex-Leak — Entlastung BESTAETIGT, aber mit ZWEI Korrekturen
- Post-Pass selbst disassembliert @0x801044b0-500: `andi 0x4; lbu +0x5; addiu -11;
  sltiu 4; bne -> skip` — Release, sobald +0x5 (aktuell) nicht in [11..14]; Port
  :1866-1870 identisch. Kein State-Gate. Raum-Reload cleart das Wort. Kein permanenter Halt.
- BEIDE Port-Writer ueberschreiben +0x5: Gun/Hitscan-Resolver `e->sub_state_1 = weapon_id`
  (re15_damage.c:1453; Messer w1/w2 laeuft durch DENSELBEN Resolver, nur anderer
  Kegel-Zweig :1428-1432), Trefferbox-Pfad `react_table[type] < 11` (:1703). Eine
  HURT-Zeile, die +0x5 "in [11..14] laesst", gibt es nicht als Restzustand: der
  Zeilen-Dispatch uebersetzt +0x5 per s_re2c_row_from_weapon[22] (crow.c:1433-1452),
  JEDE der 4 Lanes (Flug/GIB/Launch/Boden) exitet nach (1,9)/(1,0) oder in CORPSE-Subzeilen
  0..3 — im selben Root-Tick greift dann der Post-Pass.
- **Korrektur 1:** Der Datei-Kommentar "RE2-Waffen 11..14 existieren im Port nicht"
  (crow.c:1864) ist FALSCH — die RE1.5-Waffen-Ids gehen bis 21 (Tester-Tabelle
  @0x8006E548, re15_damage.c:1416-1418; 12=Ingram, 13=SPAS-12, 19=MC51, 9/10/11/14ff
  weitere). Ein Treffer mit Id 11..14 haelt den Mutex fuer die HURT-/Todes-Animationsdauer
  (mehrere Sekunden) — transient und mechanisch analog zum Original (dessen +0x5 = RE2-Item-
  Id, 11..14 dort ebenfalls haltend), aber die WAFFEN-MENGE, die haelt, ist eine andere
  (unuebersetzte Id-Raeume). Kein Dauer-Leak, aber eine reale, undeklarierte Divergenz.
- **Korrektur 2 (Randfall, kein Leak):** Waehrend der Halte-Phase sind alle anderen
  Kraehen arbiter-blockiert — ein Treffer mit Ingram/SPAS "friert" den Schwarm also fuer
  die Dauer der Reaktion. Das kann als kurzer "alle haengen"-Eindruck sichtbar werden,
  loest sich aber selbst.

### 4.2 Pacify — Entlastung BESTAETIGT (eigenes Disasm)
dec0-Kopf @0x801006c4-f4: `lh 0x800cfd4e; bgez -> Wake-Kette` (lebender Spieler geht NIE
in den Nie-wecken-Ast); `lb +0x221; bgtz -> Wake-Kette`; nur tot && pac<=0 && dist<0x708
-> return. Wake-Kette liest +0x22A&1 (nicht LOS). Pacify-Produzent: +0x22A&0x80 ->
+0x221=120 (@0x80104504-20, eigener Scan). Bei lebendem Spieler unerreichbar.

### Fern-Fall (>7195) — KEIN Exit, aber auch KEIN Stall
dec13/dec11 haben keinerlei Dist-Pruefung (eigenes Disasm, s. (b)). Die geclaimte Kraehe
verfolgt per move3d (speed bis 300) — dist faellt wieder; Kegel 192 + steer 96 halten sie
ausgerichtet. Byte-true identisch im Original. Kein eigenstaendiger Haenger-Mechanismus.

## (e) Fix-Risiko — ehrliche Bewertung

1. **Der Fix trifft den gemeldeten Fall womoeglich NICHT.** In 10C0 aendert der neue Ray
   die Kandidatenmenge nicht (identische 15 Rechtecke heute wie danach); der einzig
   verbleibende B-Mechanismus (Radius-Keil) bleibt bestehen, weil auch der neue Ray
   nulldick ist. VOR der Portierung die Repro aus Dossier-Abschnitt 6 fahren und pruefen,
   ob der Haenger ueberhaupt LOS-getrieben ist — sonst ist der wahrscheinlichere Treiber
   Ursache A (fehlender Navigator; MAPPING im Port bestaetigt: exec4/5/6 steern hart auf
   den Spieler, crow.c:617/657/684; Original @0x801005b4-c0 steert Routen-Knoten via
   0x8004a808) bzw. der Radius-200-vs-100-Keil im Hover (Ursache C).
2. **Sub-0-Wake unveraendert** — dec0 liest Bit 2 nicht (Overlay-Scan, s. (b)). Konsumenten
   des Rays sind exakt dec11/dec13/Arbiter.
3. **Performance unkritisch:** der heutige Shim scannt bereits Regionen 0..3 pro Kraehe
   pro Tick (10C0: 180 Eintraege, 15 einzigartig); der neue Ray hat dasselbe Volumen.
   Original-Referenz: RE2 1090 iteriert 22 Records x bis zu 28 Kraehen jeden Tick.
   PSX-Budget: ~500-600 Rechteck-Tests/Tick ~ <1% Frame. Dedupe (Zellen sind 12-fach
   repliziert) waere ein optionaler Generator-Schritt (PSX-Laufzeitbudget-Memory).
4. **"Zu passiv"-Risiko begrenzt, ABER richtungsabhaengig von der Band-Wahl:**
   - Live-Band (empfohlen): 10C0 identisch zu heute; 1170 wird AGGRESSIVER als heute
     (Band 5/6 leer -> LOS nie geblockt; heute blockt der Shim mit Band-4-Waenden).
     Das entspricht dem Original-Charakter (RE2-1090-Kraehen ueber offener Strasse).
   - Alle-Baender ("byte-treu" zu param_4=0): 1170 wird DEUTLICH passiver (untere
     Stockwerks-Zellen brechen Sicht in 2D) — NICHT nehmen.
   - Der Vergleich "RE2 1090 = offene Strasse, kaum Sichtblocker" stimmt so nicht:
     ALLE 22 Records dort tragen 0x8400 — das Original lebt damit, dass jede Kulisse
     die Sicht bricht und die Kraehe dann regulaer in Sub 4 zurueckfaellt (dort faengt
     der Wand-Prober). Moeblierte RE1.5-Raeume machen die Kraehe also nicht kaputt-passiv,
     solange der Sub-4-Kreislauf funktioniert; Camping hinter Kisten unterdrueckt Angriffe
     wie im Original.
5. **Spaeterer Zombie-Konsument (+0x154&0x800):** der RE1.5-Zombie-Sensor ist ein
   amortisierter 16-Tick-Probe MIT FOV-Kegel (re15_enemy_los_probe); ihn durch einen
   Jeden-Tick-Kollisionsray zu ersetzen aendert Kadenz UND Semantik (kein Kegel im
   0x80050858) — getrennt bewerten, nicht im selben Schritt mitziehen.

## Offene Punkte (nicht erfunden)
- Welcher RE1.5-Zelltyp dem RE2-0x8000 ("nur Sicht") entspraeche, ist weiter offen —
  praktisch irrelevant fuer 10C0/1170 (alle Zellen dort sind ohnehin cls=3/u0-solid).
- Der konkrete 10C0-Haenger ist weiterhin UNREPRODUZIERT; die Messung (RE15_RE2_TRACE +
  Positions-Dump) bleibt Voraussetzung vor jedem Fix.
