# Runde 37 — Spur I feuerloescher (Dossier, fortlaufend)

Baum `.claude/worktrees/r37_feuerloescher`, Zweig `r37/feuerloescher`, Basis master 64ba31e0.
Wortlaut (AUFTRAG.md Nachtrag 4, massgeblich):

> Bitte erstelle mir zusätzlich auch ein Modell des Feuerlöschers. Ich finde deine Idee, bei elza einen Leeren
> Feuerlöscher am Boden zu haben gut. Der müsste dann bei Leon nach der Nutzung erstmalig auftauchen.
>
> Außerdem finde ich den FEuerlöscher im Spind bei Leon noch unglücklich und würde ihn gerne woanders platzieren....

Punkte:
1. Modell des Feuerloeschers bauen (es gibt keines, weder RE1.5 noch RE2 — Vorarbeit
   `analysis/befunde_2026-09-21/re2-feuerloescher-modell.md`).
2. Elza ROOM1091: leerer Loescher liegt von Anfang an am Boden.
3. Leon ROOM1090: derselbe leere Loescher liegt erst NACH dem Loeschen, danach dauerhaft.
4. Leons Loescher aus dem Spind ROOM1000 an einen anderen Ort verlegen (sichtbar, mit dem neuen Modell).

Nachtraege waehrend der Arbeit (vom Koordinator in AUFTRAG.md dieses Baums committet, 11cadf09 / 99dbe19e):
* 4b "ah, it can only lay there - if leon when he is asked to discard it select "yes"." -> der leere Loescher liegt
  in ROOM1090 NUR nach JA an der Wegwerf-Abfrage (so schon gebaut, Abschnitt 3.4).
* 4c "If he doesn't he has the text empty fire extinguisher in the player inventory." -> nach NEIN heisst der behaltene
  Loescher "Empty Fire Extinguisher" (Abschnitt 3.5).

Hinweis zur Zuteilung: Der Auftrag, der diesen Workflow angestossen hat, ist Nachtrag 3b ("Es muss schon in der
richtigen Farbe leuchten das Panel ...") — der gehoert laut AUFTRAG.md/VERTRAG.md zur Spur E codepanel und wurde dem
Bau-Agenten E uebermittelt. Spur I fasst die Code-Panels NICHT an.

## Stand
Alle vier Punkte gebaut und an der exe abgenommen (Abschnitte 3-5). Offene Entscheidungen fuer den Nutzer: Abschnitt 7.
Nachbesserung 1 (Abnahme 0): M1 (Loescher lag nach NEIN + Item-Box doppelt in ROOM1090) und M2 (Abschnittsverweis)
behoben — Abschnitt "Nachbesserung 1" am Ende.

## 1. Messung vorher (Stand master 64ba31e0, Baum gebaut 2026-10-12)

Raumstart mit allen Port-Installern (`probe_r34n_e_dokumente aots <Raum>`, echte RDT + scd_room_reenter):

* ROOM1000 (Leon): `AOT slot= 3 ITEM x[20500..21300] z[-800..0] item=0x31 x1 bit=134 prop=255` — die
  Aufnahme ist eine reine Zone ohne Modell (prop 255 = 0xFF), in Cut 0-2 (Umkleide, Spindreihen; Bild
  `I_belege/vorher_1000_umkleide_cut0.png`, offener Spind Mitte rechts). Props: obj 0/1 Medizinkoffer, obj 2 Dokument (Runde 34).
* ROOM1090 (Leon, Feuer brennt): Slots 0-4 (Tueren, Text msg 7 Viereck, AUTO, Schrot r35), Props obj 0,1
  (Typ 4), obj 3 (Wagen-Teil), obj 4 (Schrot). Nach dem Loeschen legt der Raum KEINEN Loescher hin.
* ROOM1091 (Elza): Slots 0,1,2,4,5 (Schuppen-Text Runde 36 Spur P), Props obj 0,1,2,4 — kein Loescher.
* Modell: es gibt keins (Vorarbeit `analysis/befunde_2026-09-21/re2-feuerloescher-modell.md` §4.2/4.3:
  Item 0x31 kommt genau einmal vor, ROOM1000 @0x0C24 tk_prop 0xFF; 570/570 RE1.5-Props und 392 RE2-Netze
  angesehen).

## 2. RE-Belege (selbst disassembliert / aus den RDT-Bytes)

### 2.1 Die Aufnahme im Original — ROOM1000.RDT sub00 @0x00C24 (22 B, Kurzform)
```
0C24: 50 03 09 31 00 00 14 50 e0 fc 20 03 20 03 31 00 01 00 86 00 ff 00
      Item_aot_set Slot 3, sce 9, sat 0x31, Band 0, Rechteck (20500,-800) 800x800, Item 0x31 x1, Bit 0x86, tk_prop 0xFF
```
Handler Item_aot_set @0x80040644 (PSX.EXE, `re15_disasm.py dis 0x80040644`):
```
80040664  andi v0,v0,0x80        ; sat & 0x80 -> Lang/Kurzform
80040680  lhu  a1,18(a2)         ; +0x12 = Taken-Bit (Kurzform)
80040684  lbu  s1,20(a2)         ; +0x14 = tk_prop
80040688  addiu v0,a2,22         ; Kurzform 22 B
800406d8  lw   a0,18056(a0)      ; 0x80074688 = Flag-Tabelle Bank 9 (0x800b1078)
800406dc  jal  0x8004efe4        ; Bit lesen
800406e4  beq  v0,zero,0x8004071c ; nicht genommen -> fertig
800406f4  sb   zero,0(v0)        ; genommen -> Satz still (sce 0)
800406f8..80040714               ; Pool 0x800b3f98 + tk_prop*148
80040718  sw   v1,0(at)          ; = 0x80000000 -> Modell versteckt
```
Aufnahme bestaetigt (FUN_8001db28 Zustand 7 @0x8001e048): `sb zero,0(v1)` @0x8001e090 (Zone tot), Einfuegen
`jal 0x8004dc4c` @0x8001e0c4, Taken-Bit `jal 0x8004ef90` @0x8001e0d0 mit a0 = 0x800b0fd6+162 = 0x800b1078 (Bank 9),
a1 = Nutzlast +4. Prompt-Rueckruf LAB_80021f6c: Modell weg `sw zero,0(at)` @0x80021fc8 (Pool[Nutzlast+6 = tk_prop]),
Bit `jal 0x8004ef90` @0x80021fd8. ⇒ KEIN Pfad setzt dort (3,0x85).

### 2.2 Story-Bit (3,0x85) — ROOM1000.RDT sub01 @0x00CFC
```
0CFC: 06 00 14 00   Ifel_ck
0D00: 21 09 86 01   Ck(9,0x86)==1     (Loescher genommen)
0D04: 06 00 0a 00   Ifel_ck
0D08: 21 03 85 00   Ck(3,0x85)==0
0D0C: 22 03 85 01   Set(3,0x85)=1
```
sub01 wird in JEDEM Spielbild neu gestartet: FUN_8003f038 `ori a0,zero,0x1` @0x8003f064, `lw v0,68(v0)` @0x8003f070
(RDT+0x44), `jal 0x8003ee3c` @0x8003f080 mit a1=1. Liegt der Fundort NICHT mehr in ROOM1000, muss dieselbe Regel
dort greifen, wo genommen wird. Leser: ROOM1090 sub00 @0x02306 `06 00 24 00 21 03 85 00`: (3,0x85)=0 -> Slot 2
sce 1 msg 7 @0x0230E ("I must hurry up and get something to put out this fire to save that woman!"); sonst
@0x02336 Slot 2 sce 3 -> sub06 + Selbst-Tuer Slot 3 @0x02352. Zensus aller 258 RDTs (`scd_dump_room.py`):
(3,0x85) nur in ROOM1000 sub01 und ROOM1090 sub00.

### 2.3 Loeschen und das DAUERHAFTE Bit — ROOM1090.RDT
```
sub06 @0x02702: 2b 07 ff ff  Message_on 7 / 2b 08 ff ff Message_on 8 ("Will you use the Fire Extinguisher?")
      @0x0270E: 06 00 18 00  Ifel_ck ; @0x02712 21 0c 1f 00 Ck(12,0x1F)==0 (= Ja)
      @0x02716: 22 02 07 01  Set(2,7)=1 ; @0x0271A 09 0a 0a 00 Sleep 10
      @0x0271E: 22 03 81 01  Set(3,0x81)=1   <- Feuer aus (dauerhaft)
      @0x02722: 22 03 84 01  Set(3,0x84)=1   <- Einmal-Zeichen fuer die Rettungsszene
      @0x02726: 47 03        Aot_on 3 (Selbst-Tuer @0x02352 -> ROOM1090 neu laden)
sub00 @0x022A2: 06 00 40 00 / 21 03 84 01  Ck(3,0x84)==1 -> obj 2, Sce_em_set 0x42 (Ada), Evt_exec sub03 @0x022E0
sub03 @0x024CE: 22 03 84 00  Set(3,0x84)=0  <- ERSTER Befehl der Rettungsszene
```
⛔ Der Auftrag nennt (3,0x84) als Bedingung — das Bit ist nach der Rettungsszene wieder 0 (sub03 @0x024CE); ein daran
gehaengter Loescher laege nur waehrend des einen Neuladens da und waere beim Wiederbetreten weg. DAUERHAFT ist (3,0x81):
sub00 legt Feuer/brennende Kamerabilder nur bei Ck(3,0x81)==0 an (@0x021C8 Cut-Wahl 8/11, @0x021EE obj 3 + 7x Sce_em_set
Typ 0x26, @0x022EA Musik, @0x02332 Loesch-Platz). Zensus Set(3,0x81) in 258 RDTs: ROOM1090 sub06 (Leon) und ROOM10B1
@0x01832/@0x01954 (Elza-Variante, eigene Bedeutung — Leons Spiel betritt 10B1 nie). ⇒ Tor fuer Leon = (3,0x81)==1.

### 2.4 Elza — ROOM1091 / ROOM1001
ROOM1091 main00 @0x02202 `2d 02 00 00 02 00 08 00 10 00 6a 09 a3 f5 2e ff 40 00 40 07 ...` = obj 2 an Leons
Nach-Rettungs-Lage (wie ROOM1090 sub00 @0x0237E), kein Typ 0x26 — bei Elza ist der Platz schon geloescht (Leon war
zuerst da). ROOM1001 sub00 @0x00C24 `50 03 09 31 00 00 14 50 e0 fc 20 03 20 03 15 00 1e 00 86 00 ff 00` = derselbe
Spind traegt bei Elza Item 0x15 x30 mit demselben Bit 0x86 — bleibt unberuehrt (der Spind-Umbau gilt nur 0x1000).

### 2.5 Wie das Original Gegenstands-Modelle zeichnet (Licht)
Obj_model_set LAB_80040914: Flags `lhu v0,6(a2)` @0x80040990, `ori v0,v0,0x1` @0x80040998; Attribut `lh v0,8(a2)`
@0x800409a8 -> `sw v0,12(a1)` @0x800409b0 (Pool+0x0c). Zeichner FUN_8002c18c: `lw v0,12(s1)` @0x8002c2ec,
`andi v0,v0,0x10` @0x8002c2f4, `beq v0,zero,0x8002c360` @0x8002c2f8 -> ohne Bit NCCT (Raumlicht), mit Bit flach
`jal 0x80022f0c` @0x8002c320. Item-Props tragen Attribut 0: ROOM1010 @0x00930 `2d 00 00 00 01 00 0a 00 00 00`
(First Aid Spray), ROOM4050 @0x01E8A/@0x01F32/@0x01FC6 (Antidote Spray, Kraeuter), ROOM1000 @0x00C66/@0x00C88
(Medizinkoffer). ⇒ Loescher = Typ 0, Band 1, Flags 0x000A|1, Attribut 0 (NCCT) — wie jedes Gegenstands-Modell.
(Die Lageplaene der Runde 36 tragen 0x0010, weil RE2 seine WANDPLAENE so zeichnet — ein Zylinder ist kein Plan.)

### 2.6 Aktionspruefpunkt
FUN_80042bac `ori v0,zero,0x26c` @0x80042bd0 = 620 Einheiten vor dem Spieler; je Druck gewinnt der KLEINSTE treffende
Slot (aot_common.c, `j 0x80043028` @0x80042f94). Daraus: Aufhebe-Rechteck muss den 620-Punkt vor der Kollisionskante
enthalten (Abschnitt 3.2), Untersuchen-Platz in ROOM1091 braucht einen Slot < 5 (Schuppen-Text, Abschnitt 3.4).

### 2.7 Wegwerf-Abfrage, Namen und Beschreibungen (fuer 4b/4c)
* Abfrage: Port-Konstruktion der Runde 25 in RE2-Form (include/re15_item_discard.h): Zeile `{ 0x1090, 9, 0x31, 0, 0 }`
  in gen/discard_sites.inc = nach ROOM1090 msg 9 "You've used the Fire Extinguisher." (sub03 @0x02502); JA leert den
  Platz (RE2 `sb zero,id` @0x80051774 / `jal FUN_80069714` @0x80051798 -> re15_inv_remove_slot), NEIN laesst ihn mit
  Anzahl 1 (@0x800517C4). Kein RE1.5-Opcode entfernt Gegenstaende (Dispatch 0x00..0x5E, kein Sce_item_lost) — ohne
  Abfrage bliebe der Loescher fuer immer im Inventar. Das Ergebnis (Gegenstand da/weg) speichert der Spielstand ohnehin
  mit (Inventar in re15_savedata) — ein eigenes Bit ist nicht noetig (VERTRAG 1.1 teilt Spur I auch keins zu).
* Namen: die Port-Namensbank gen/inv_name_bank.inc ist das DEBUG.BIN-Bild (102 Eintraege, 0x07-Ende; Name 0x31
  "Fire Extinguisher" @0x4CDC = `22 45 4e 41 00 21 54 50 45 4a 43 51 45 4f 44 41 4e`); Beschreibungen gen/inv_desc_bank.inc
  (`01 00`-Ende, 0x08 Zeilenumbruch; laengste Original-Zeile 191 px). Vorbild fuer einen LEEREN Gegenstand ist RE1.5
  selbst: Name 0x1C "Empty Grenade Shells" @0x4BA7 (`21 49 4c 50 55 00 ...`). RE2 hat keins: 0 "Empty"-Eintraege unter
  139 Namen und 0 in den Beschreibungen (analysis/befunde_2026-09-21/belege/re2_item_names.txt / _descriptions.txt).
  RE3 liegt nicht im Repo (info/ enthaelt nur Re1.5 und re2leon) — dort nicht pruefbar. Eigener Port-Gegenstand
  (freie Id) verworfen: Icon, Item-Bild, Name, Beschreibung und Wegwerf-Stelle muessten fuer eine neue Id gebaut
  werden; derselbe Gegenstand 0x31 mit abgeleitetem Zustand braucht nur Name + CHECK-Text und bleibt mit allen
  alten Spielstaenden vertraeglich.
* Haken-Stellen wie Runde 36 Spur Q (Schluessel-Namen): re15_inv_screen.c (Name im Statusschirm emit_name/emit_name_at,
  CHECK-Text re15_inv_desc_entry) und item_prompt_common.c (Name in Abfragen).

### 2.8 Nachtrag 3b (der Auftrag, der diesen Lauf ausgeloest hat)
"Es muss schon in der richtigen Farbe leuchten das Panel..." gehoert laut AUFTRAG.md (Nachtrag 3b) / VERTRAG.md zur
Spur E codepanel und wurde dem Bau-Agenten E uebermittelt. Spur I hat keine Panel-Datei angefasst.

## 3. Umsetzung

### 3.1 Modell (Punkt 1) — `re15_port/tools/r37_feuerloescher/feuerloescher_modell.py` -> `engine/src/gen/feuerloescher_r37.inc`
PORT-WAHL (es gibt kein Original-Modell). Einzige Original-Vorlage = Item-Bild ITPS.ITP Block 0x31 @file 0x093000
(0x31 * ITPS_STRIDE 0x3000, engine/src/itps_common.c), 112x72 8bpp; daraus GEMESSEN (Ausgabe des Werkzeugs):
* Achse der Koerperpixel (PCA): 29,4 Grad; Koerper t -25,8..25,4 px. Breite QUER zur Achse (0,05-px-Abtastung je 2 px):
  Median 17,05 px; Zylinder (>= 90 % Breite) t -20..20 = 40 px -> Laenge/Durchmesser 2,346; Schulter 5,8 px, Boden 5,4 px;
  Hebelpixel s -13,8..11,3 px (Hebel ~1,55 R ab der Achse), Chrom = hellere Haelfte der grauen Hebelpixel.
* Massstab: Leon PL00.PLD Nullpose 3022 Einheiten (`tor/tor_massstab.py` spielermodell; Trefferkasten @0x80073e94 = 3060)
  = 1,78 m -> 1697,8 Einheiten/m; Durchmesser 13 cm (10-lb-Klasse) -> R = 110; Mantel 586 (Fase 35), Kuppel 75, Kopf 127
  -> Gesamthoehe 792 = 0,47 m (Riegel: 0,40..0,60 m). Zehnkant wie RE2 ROOM60D0 Prop 2 (Laborflasche).
* Netz: 235 Punkte, 10 Dreiecke (Boden), 138 Vierecke; Mantel (3 Baender, radiale Normalen), Fase, Kuppel (4 Ringe),
  Hals, Ventil-Kasten (Chrom), Griff + zweiteiliger Hebel (Chrom; Seite aus dem Bild: von der Etikettseite gesehen
  rechts-oberhalb der Achse), schwarzer Schlauch mit Duese zur Gegenseite. Zwei Lagen DESSELBEN Netzes:
  `stehend` (Ursprung Bodenmitte, Achse -Y; Wand) und `liegend` (Ursprung Mitte auf der Auflage y=0, Achse X,
  Etikett oben; Boden) — keine 90-Grad-Prop-Drehung noetig (Lehre der Sicherung, sicherung_engine_export.py).
* Textur 128x128 8bpp + CLUT: Mantel = das ITPS-Bild entlang der Achse abgewickelt (Etikett, Flecken, Baender), quer
  ent-schattiert (Helligkeitsprofil je Querlage der roten Pixel normiert); Helligkeit x0,899 = Weiss der Original-
  Spraydose ROOM4050 TIM @0x2DDA8 (99,5-Perzentil R+G+B = 656; ITPS-Weiss nach dem Ent-Schatten 730); Farbfelder
  Rot/Dunkelrot (Mantelmittel), Chrom, Metall, Schwarz. CLUT-Index 0 frei, kein weiteres 0x0000.
* Format wie gen/sicherung_prop.inc: MD1-Kopf 65/0/2, Normalen Q12 nach aussen, Vierecke in Z-Ordnung, TIM @VRAM(0,0),
  CLUT @VRAM(0,480), clut 0x7800 / page 0x80. Licht: Attribut 0 = NCCT wie jedes Gegenstands-Modell (2.5).
* Bilder: `I_belege/modell_stehend.png`, `modell_liegend.png`, `modell_textur.png`, `modell_vergleich_itps.png`
  (Original-Bild neben dem Modell in derselben Achse und Groesse).

### 3.2 Fundort Leon (Punkt 4) — ROOM1050 "EAST CORRIDOR", Wand rechts der Notausgangs-Tuer, Cut 6
PORT-WAHL (der Nutzer kann ihn verschieben; Alternativen in Abschnitt 7). Begruendung mit Bild
(`I_belege/nachher_1050_wand_cut6.png`): die Doppeltuer mit EXIT-Schild in Cut 6 IST die Tuer nach ROOM1090
(Door_aot_set Slot 5 @0x00B7A, Rechteck x 18100..21600 z -23900..-22900, Ziel Raum 0x09 Cut 3). Leon kommt nach dem
Hilferuf an der 10A0-Tuer (Ada-Ruf, Runde 34 Nacht) genau hier vorbei; ein Loescher neben dem Notausgang ist der
uebliche Platz. Rechts der Tuer ist die gemalte Wand leer (Muenztelefon links, Zeitungsstaender rechts unten).
* Lage: x 17200 (Bild x ~222, zwischen Tuerrahmen x ~205 und Zeitungsstaender x ~230), Wandebene z ~ -23900 (die
  Tuerunterkante im Hintergrund deckt sich mit der Projektion bei z -23900; die Kollisionszelle x 12700..26100
  z -24350..-23377 liegt VOR der gemalten Wand) -> Prop z -23775 (R + 15 vor der Wand), Unterkante y -1150 (0,68 m),
  Oberkante -1942 (1,14 m; NFPA 10: Oberkante <= 1,53 m), rot_y 0 = Etikett zum Raum. Keine Maske ueber dem Punkt in
  Cut 6 (105 Masken), Cut 3, Cut 4 (`probe_r34n_e_dokumente sicht 1050 17200 -1600 -23250`).
* Aufnahme: Slot 13 (frei; belegt 0..10 Original, 15 Dokument, 52.. Kamerazonen), Item 0x31 x1, Taken-Bit (9,0x86) —
  DERSELBE Satz wie ROOM1000 @0x00C24, nur mit tk_prop 5 (das Modell verschwindet bei der Aufnahme, @0x80021fc8).
  Rechteck 1000x1000 x 16700..17700 z -23900..-22900 (z-Spanne wie die Tuer daneben: der Spieler steht vor der
  Kollisionskante z -23377, der Pruefpunkt 620 voraus @0x80042bd0 faellt dann in z -23520..-23000). Kein Ueberlapp mit
  Slot 5 (Tuer, ab x 18100) und Slot 6 (30 Patronen, bis x 16650).
* (3,0x85): Haken in item_modal_common.c (Aufnahme bestaetigt, Taken-Bit gesetzt) -> re15_feuerloescher_r37_genommen:
  Item 0x31 + Bit 0x86 -> Set(3,0x85)=1 = ROOM1000 sub01 @0x00D0C. Spielstaende bleiben gueltig (dieselben Bits).

### 3.3 Spind ROOM1000 (Punkt 4)
Leon (0x1000): Slot 3 (Original-Aufnahme @0x00C24) wird zum Raumtext msg 0 "Nothing unusual." mit dem Rechteck der
Aufnahme (20500,-800) 800x800, sat 0x31, Band 0, Pause 0xFFFF — dieselbe Satzform wie die Plaetze 6..9 @0x00CAA.. Gilt
auch mit gesetztem Taken-Bit (der Port fuehrt den Satz dann als NONE, Gegenstueck zu `sb zero,0(v0)` @0x800406f4).
Elza (0x1001): unberuehrt (30 Patronen, Item 0x15).

### 3.4 Der leere Loescher (Punkte 2 und 3) — ROOM1091 Elza / ROOM1090 Leon
* Lage (2500,-1800,-1300), liegend, rot_y 3250: vor der Seitentuer des Wagens, aus der die Frau in der Rettungsszene
  steigt (sub03 Ada-Start (2602,-2233) @0x02554), zwischen den Feuern (2052,-1334) @0x02214 und (2375,-2333) @0x02228 —
  dort stand Leon beim Loeschen (Viereck Slot 2 @0x02336). Im Bild in Cut 4/5/6/7 und unter keiner Maske (`sicht`);
  verworfen: (700,-1300) Cut 4 Maske 21, (2300,0) Cut 5 Maske 15 (Stossstange), (2100,600) in Cut 7 unter dem
  Bildrand (Cut 7 = Hauptbild der Rettungsszene, gemessen Cut_chg-Folge 5 -> 7 -> 6 -> 7), (2700,-1000)/(2900,-1200)
  Cut 5 Maske 14. rot_y 3250 = diagonal zu den Blickachsen von Cut 5 (Lot 3760) und Cut 6 (Lot 2733).
* Untersuchen (PORT-WAHL zur Freigabe): msg 29 "It's an empty fire extinguisher." (36 B, Form ROOM1000 msg 0 @0x0D22),
  Platz 1000x1000 um das Prop. ROOM1091 Slot 3 (< Slot 5 = Schuppen-Text der Runde 36 -> Vorrang am Loescher; der
  Messpunkt der Runde 36 (0,250) -> (620,250) liegt nicht darin), ROOM1090 Slot 6 (Slot 5 bleibt frei, Riegel
  unit_r36_story_schuppen).
* ELZA ROOM1091: immer (der Platz ist bei ihr schon geloescht, 2.4).
* LEON ROOM1090: Tor = (3,0x81)==1 (DAUERHAFT, 2.3 — NICHT (3,0x84), das sub03 @0x024CE loescht) UND Item 0x31 nicht im
  Inventar UND nicht in der ITEM BOX (Nachbesserung 1: die Kiste lagert nur um, RE2 FUN_800703b8 @0x80070710). Grund fuer die zweite Bedingung (PORT-WAHL, an der exe gemessen): nach dem Loeschen fragt die Port-Wegwerf-
  Abfrage der Runde 25 bei msg 9 "You've used the Fire Extinguisher." (gen/discard_sites.inc Zeile ROOM1090/9/0x31)
  "You don't need this item any more. Discard it?". JA = Leon legt ihn hin -> Haken in item_discard_common.c (Ja-Zweig
  nach re15_inv_remove_slot) -> re15_feuerloescher_r37_weggeworfen -> der Loescher liegt SOFORT da (Installer mit
  offenem Tor; das Modell ist beim Raumstart schon geladen). NEIN = Leon behaelt ihn (RE1.5-Verhalten: kein Opcode
  entfernt Gegenstaende), dann liegt keiner da. Der erste Lauf (Tor nur (3,0x81)) zeigte genau dieses Doppel: Loescher
  am Boden UND im Inventar, waehrend die Wegwerf-Frage offen stand.

### 3.5 "Empty Fire Extinguisher" (Nachtrag 4c)
NUTZER-VORGABE. Zustand "leer" = Leon (re15_char_variant() == 0) UND (3,0x81) — das Bit setzt nur sub06 @0x0271E, also
genau dann, wenn Leon den Loescher benutzt hat; vor dem Loeschen bleibt "Fire Extinguisher". Name = "Empty " aus
@0x4BA7 + Name 0x31 aus @0x4CDC + 0x07 (162 px; laengster Original-Name "Minidisc Player w/ Disc" 158 px, passt im
Statusschirm, Bild unten). CHECK-Text (PORT-WAHL, Vorschlag des Koordinators): "An empty fire extinguisher." (186 px,
eine Zeile; Original-Zeilen bis 191 px). Elza: kein Ersatz ((3,0x81) hat dort eine andere Bedeutung, ROOM10B1).
Spielstand: Flags + Inventar reisen mit re15_savedata -> nach Laden derselbe Name. Bild/Icon bleiben das Original
(ITPS 0x31, ITEMALL-Kachel 0x31) — derselbe Gegenstand, nur leer.

### 3.6 Dateien
Neu: `engine/src/feuerloescher_r37.c`, `include/re15_feuerloescher_r37.h`, `engine/src/gen/feuerloescher_r37.inc`,
`tools/r37_feuerloescher/feuerloescher_modell.py`, `tools/r37_feuerloescher/kamera.py`,
`tests/unit/test_r37_feuerloescher.c`, `tests/unit/probe_r37_feuerloescher_karte.c`,
`tests/unit/probes/r37_feuerloescher.cmake`, `tests/integration/test_r37_feuerloescher.cmake`.
Haken (je 1-2 Zeilen, Kommentar "Runde 37 Spur I"): `engine/src/scd_room_setup.c` (Include + Installer direkt nach
re15_inhalt_r35_install, VOR re15_dokumente_install wie Runde 35 — am Blockende waere das Dokument-Prop nicht mehr das
letzte, Riegel unit_r34n_e_dokumente "hinten angehaengt" rot), `platform/pc/main.c` (Include, Prop-Lader obj 5 nach
dem Lageplan-Block, Boot-/CONTINUE-Installer an derselben Stelle wie in scd_room_setup.c),
`engine/src/item_modal_common.c` (Include + re15_feuerloescher_r37_genommen), `engine/src/item_discard_common.c`
(Include + re15_feuerloescher_r37_weggeworfen), `engine/src/re15_inv_screen.c` (Include + CHECK-Text + 2x Name),
`engine/src/item_prompt_common.c` (Include + Name in Abfragen). Keine Bank-9-Bits neu belegt (nur die Original-Bits (9,0x86), (3,0x85),
(3,0x81)), Nachrichten-Id 29 (VERTRAG 1.2), Prop obj 5 (TIM-Slot 9), kein Patch an shared_assets.

## 4. Messung nachher
* `probe_r34n_e_dokumente aots` (echter Raumstart mit allen Installern): 1050 `AOT slot=13 ITEM x[16700..17700]
  z[-23900..-22900] item=0x31 x1 bit=134 prop=5`, `PROP obj_id=5 pos=(17200,-1150,-23775)`; 1091 `slot= 3 MESSAGE
  band=0x01 evt=29 x[2000..3000] z[-1800..-800]`, Slot 5 bleibt msg 20; 1000 `slot= 3 MESSAGE evt=0 x[20500..21300]
  z[-800..0]`; 1090 vor dem Loeschen `[feuerloescher] ROOM1090: kein Loescher (Feuer brennt noch, (3,0x81)=0)`.
* exe Ende-zu-Ende (integration_r37_feuerloescher Lauf A, CONTINUE ROOM1050, Eingabeskript): Aktion an der Wand ->
  "Will you take the Fire Extinguisher? Yes No" mit dem ITPS-Bild (`I_belege/nachher_1050_aufnahme_modal.png`) -> Ja ->
  `[feuerloescher] Aufnahme Item 0x31 (9,0x86) -> Set(3,0x85)=1`; Sprung ROOM1090 (Feuer brennt, kein Loescher) ->
  Aktion -> `id=7 "I must hurry up ..."`, `id=8 "Will you use the Fire Extinguisher?"` -> Ja -> Selbst-Tuer ->
  `ROOM1090: kein Loescher (Leon traegt ihn noch (Item 0x31 im Inventar))` -> `id=9 "You've used the Fire Extinguisher."`
  -> Wegwerf-Ja -> `Loescher weggeworfen -> liegt in ROOM1090` -> `Loescher liegt leer am Boden obj 5 bei
  (2500,-1800,-1300) rot_y 3250, Zone Slot 6` -> `[prop-render] oid=0x05 pos=(2500,-1800,-1300) rot=(0,3250,0)`; danach
  laeuft die Rettungsszene (msg 3..6). Bild Cut 7 direkt nach dem Ja: `I_belege/nachher_1090_cut7_weggeworfen.png` und
  `nachher_1090_cut7_zoom.png`.
* exe Tuer-Weg (Lauf B): ROOM1050 mit (9,0x86)/(3,0x85)/(3,0x81), Loescher abgelegt, Aktion an der Notausgangs-Tuer ->
  ROOM1090 -> `Loescher liegt leer am Boden` (Wiederbetreten).
* exe Elza (Lauf C, Zombies 0xF8..0xFB als erledigt — sonst packt Zombie 3 sie vor dem Druck): `ROOM1091: Loescher liegt
  leer am Boden`, Aktion -> `id=29 "It's an empty fire extinguisher."`; Bild `I_belege/nachher_1091_elza_cut6.png`
  (Elza vor der offenen Wagentuer, der Loescher liegt vor ihr). Cut 5 beider Varianten:
  `I_belege/nachher_1090_1091_cut5.png` (klein an der rechten Stossstange; Cut 5 ist die Fernkamera).
* exe Spind (Lauf D): `room=1000 id=0 "Nothing unusual."`, keine Aufnahme.
* exe Wand (Lauf E): `oid=0x05 pos=(17200,-1150,-23775)` gezeichnet; Bild `I_belege/nachher_1050_wand_cut6.png`.
* exe Spielstand (Lauf F, CONTINUE in ROOM1090 mit (9,0x86)/(3,0x85)/(3,0x81)/(3,0xBB)/(3,0x6E), Loescher abgelegt):
  `ROOM1090: Loescher liegt leer am Boden ... Zone Slot 6`, `[prop-render] pi=4 oid=0x05 pos=(2500,-1800,-1300)`;
  Lauf G (dasselbe, Loescher im Inventar): `ROOM1090: kein Loescher (Leon traegt ihn noch (Item 0x31 im Inventar))`,
  kein prop-render obj 5.
* exe NEIN (Lauf I, Nachtrag 4b): Spielstand ROOM1090 am Feuer mit Loescher -> msg 7/8 -> Ja -> Neueintritt
  `kein Loescher (Leon traegt ihn noch ...)` -> msg 9 -> Wegwerf-Abfrage -> NEIN (X auf der fertig getippten Abfrage)
  -> KEIN `weggeworfen`, KEIN `liegt leer am Boden`, die Rettungsszene laeuft weiter (`id=3 "Leon: There! Are you okay?"`).
* exe Nachtrag 4c (Laeufe J/K, Spielstand + CONTINUE mit Loescher nach dem Loeschen): Statusschirm
  `[feuerloescher] Name Item 0x31: Empty Fire Extinguisher`, CHECK `[feuerloescher] CHECK-Text Item 0x31: An empty fire
  extinguisher.`; Lauf L (vor dem Loeschen): kein Ersatz-Name. Bild `I_belege/nachher_4c_empty_fire_extinguisher.png`
  (links Statusschirm mit "Empty Fire Extinguisher", rechts CHECK mit Foto und Text).
* exe Elza Tuer-Weg (Lauf H, CONTINUE ROOM1051 vor der Notausgangs-Tuer, Aktion): `PC loaded room1091.rdt` ->
  `ROOM1091: Loescher liegt leer am Boden`; in ROOM1051 haengt keiner (Elzas Variante bekommt keinen Wand-Loescher —
  Leon war zuerst da und hat ihn genommen).

## 5. Tests
* NEU `unit_r37_feuerloescher_{modell,raum,aufnahme,loeschen,elza,spind,spielstand,leer}` (+ `kiste` aus der
  Nachbesserung 1, s.u.) (tests/unit/
  test_r37_feuerloescher.c, Mechanik am echten Port-Code; `leer` = Nachtrag 4c: Name aus @0x4BA7+@0x4CDC, Abfrage-Text
  ueber re15_item_prompt_walk, CHECK-Text ueber re15_inv_desc_entry, Elza ohne Ersatz, nach Speichern/Laden) und
  `integration_r37_feuerloescher` (echte exe, Laeufe A-L).
* exe-Gruppen (ctest -R, Ausdruck woertlich):
  `^(integration_r37_feuerloescher|integration_r36_story_leon|integration_r36_story_elza|integration_r35_inventar1050|integration_r34n_e_dokumente_bild|integration_r36_schluessel_befreiung)$`
  -> `100% tests passed, 0 tests failed out of 6` (1083 s); danach `integration_r37_feuerloescher` um die Laeufe F/G/H
  erweitert: `^integration_r37_feuerloescher$` -> `100% tests passed, 0 tests failed out of 1`.
* Beruehrte Gruppen (ctest -R, Ausdruck woertlich):
  `^(unit_r37_feuerloescher|unit_aot_edge|unit_scd_aot_opcodes|unit_savedata|unit_item_modal|unit_item_stack_press|unit_aot_sce_census|unit_sicherung_1150|unit_re2_room1090_ab|unit_1090_|probe_1090_|unit_plc_back_yaw_1090|unit_map_1090|unit_ada_eintritt_1050|unit_r19_marke3_1050_c1_tiefe|unit_r21_discard_wegwerfen|unit_r27_ada_cutscene_1050|unit_r30_|unit_r34n_a_rolltor|unit_r34n_d_adaruf|unit_r34n_e_dokumente|unit_r35_granate|unit_r35_inhalt|unit_r35_inventar1050|unit_room1050_sicherung|unit_r36_granate|unit_r36_kartenbits|unit_r36_kartenitems|unit_r36_schluessel|unit_r36_story)`
  -> `100% tests passed, 0 tests failed out of 101`.
  Zwischenstand: mit dem Installer am Blockende und dem 1090-Platz in Slot 5 waren unit_r34n_e_dokumente ("Dokument
  hinten angehaengt") und unit_r36_story_schuppen ("Leon 1090 nach dem Loeschen: kein Text-Platz in Slot 2/5") rot ->
  Code angepasst (Installer vor den Dokumenten wie r35, Slot 6); beide Alt-Riegel unveraendert gruen.

* ENDSTAND (letzter Code-Stand dieses Zweigs inkl. 4b/4c, Bau + ein Lauf, -j 3): `^(unit_r37_feuerloescher|integration_r37_feuerloescher|unit_aot_edge|unit_scd_aot_opcodes|unit_savedata|unit_item_modal|unit_item_stack_press|unit_aot_sce_census|unit_sicherung_1150|unit_re2_room1090_ab|unit_1090_|probe_1090_|unit_plc_back_yaw_1090|unit_map_1090|unit_ada_eintritt_1050|unit_r19_marke3_1050_c1_tiefe|unit_r21_discard_wegwerfen|unit_r27_ada_cutscene_1050|unit_r30_|unit_r34n_a_rolltor|unit_r34n_d_adaruf|unit_r34n_e_dokumente|unit_r35_granate|unit_r35_inhalt|unit_r35_inventar1050|unit_room1050_sicherung|unit_r36_granate|unit_r36_kartenbits|unit_r36_kartenitems|unit_r36_schluessel|unit_r36_story|unit_inv_fsm|unit_item_icon|unit_inv_grant|unit_room1150_itembox|unit_itembox_transfer|unit_itembox_screen|unit_debug_menu|unit_pri_inventar_riegel|unit_r26_inventar|unit_r30_android_n1_inventar_cmd|unit_r36_inventar|unit_msg_dialog|unit_action_msg_gate|unit_text_freeze|integration_item_name_census|integration_r36_inventar|integration_r36_schluessel|integration_r35_inventar1050)`
  -> `100% tests passed, 0 tests failed out of 121` (278 s; darin integration_r37_feuerloescher Laeufe A-L,
  integration_r36_inventar / _schluessel / _schluessel_befreiung / item_name_census / r35_inventar1050 wegen der
  Namens-/CHECK-Haken).

## 6. OFFEN
* Nichts Funktionales. Ehrliche Grenzen der Messung:
  - Das Modell ist ein Neubau (PORT-WAHL); seine Masse stammen aus dem Item-Bild (2D-Verhaeltnis; ist der Koerper im
    Bild geneigt, waere der echte Loescher etwas laenger) und aus der Leon-Groesse 1,78 m. Kein Original zum Vergleich.
  - In Cut 5 (Fernkamera der Gasse) ist der liegende Loescher nur ~10 x 6 Bildpunkte gross; deutlich zu sehen ist er
    in Cut 6 (Wagentuer) und Cut 7 (Rettungsszene).
  - Die Wandhalterung selbst ist nicht modelliert (bei ~14 Bildpunkten Hoehe nicht erkennbar); nach der Aufnahme ist
    die Wand leer wie gemalt.
* Volle Suite NICHT gefahren (Spurbaum-Regel); die gezielten Laeufe stehen in Abschnitt 5.
* Fuer die Integration/Release: neue Quelldatei `engine/src/feuerloescher_r37.c` (GLOB) — der Android-Bau friert die
  GLOB-Liste im app/.cxx-Cache ein (Memory reai-v2-android-glob-cache), also dort neu konfigurieren. Merge-Stellen:
  `scd_room_setup.c` / `platform/pc/main.c` je eine Zeile direkt NACH re15_inhalt_r35_install (nicht am Blockende,
  Grund 3.6), `item_modal_common.c` / `item_discard_common.c` / `re15_inv_screen.c` / `item_prompt_common.c` je
  Include + eine Zeile (re15_inv_screen.c: drei) direkt neben den Runde-36-Spur-Q-Haken.

## 7. Fuer den Nutzer

### Neue Assets / Paket-Gate
Keine neue Datei: Netz, Textur und Text sind in die exe gebacken (`engine/src/gen/feuerloescher_r37.inc`, erzeugt von
`tools/r37_feuerloescher/feuerloescher_modell.py` aus `shared_assets/PSX/ITEM/ITPS.ITP`). Kein Paket-Gate-Eintrag noetig.

### Sprachdateien
Keine. msg 29 ist ein Untersuchungstext (Untersuchungstexte haben in RE1.5 keine Stimme).

### Bedienhinweise
* Leon: der Feuerloescher haengt im Ostflur (ROOM1050, Kamera mit der Notausgangs-Tuer) rechts neben der Tuer zum
  Hinterhof. Davor stellen, Aktionstaste -> "Will you take the Fire Extinguisher?" -> Ja.
* Am Feuer (ROOM1090) wie bisher benutzen. Danach fragt das Spiel "You don't need this item any more. Discard it?":
  Ja = Leon legt den leeren Loescher vor der Wagentuer ab (er bleibt dort liegen, auch beim Wiederkommen und nach dem
  Laden); Nein = Leon behaelt ihn, im Inventar heisst er dann "Empty Fire Extinguisher" (CHECK: "An empty fire
  extinguisher."), am Boden liegt keiner — auch nicht, wenn Leon ihn spaeter in eine Item-Box legt (dort steht er als
  "Empty Fire Extinguisher").
* Elza: der leere Loescher liegt in ROOM1091 vor der Wagentuer; Aktionstaste davor -> "It's an empty fire extinguisher."
* Der Spind in der Umkleide (ROOM1000) antwortet jetzt mit "Nothing unusual." — dem Raumtext msg 0, den das Original an
  seinen anderen Untersuchen-Plaetzen (Slots 6..9, Waschraum) zeigt.

### Zu entscheiden
1. FUNDORT: Ostflur 1050, Wand rechts der Notausgangs-Tuer (Bild `I_belege/nachher_1050_wand_cut6.png`). Andere
   moegliche Plaetze: an der Wand der Umkleide 1000 (sichtbar statt im Spind), in der Lobby 1030, auf dem Boden neben
   der Tuer. Sag, wohin er soll.
2. (entschieden durch 4b/4c: Ja -> liegt, Nein -> "Empty Fire Extinguisher" im Inventar.)
3. TEXTE: "It's an empty fire extinguisher." beim Untersuchen am Boden (Leon nach dem Ablegen, Elza) und der CHECK-Text
   des behaltenen Loeschers "An empty fire extinguisher." — Wortlaut ok?
4. GROESSE / HOEHE: 0,47 m hoch, 13 cm Durchmesser (10-lb-Klasse), an der Wand 0,68-1,14 m ueber dem Boden.
5. SPIND: "Nothing unusual." (Raumtext msg 0 der uebrigen Untersuchen-Plaetze in ROOM1000) oder ein eigener Satz wie
   "The locker is empty."?

## Nachbesserung 1 (nach Abnahme 0, `I_abnahme_0.md`)

Auftrag: M1 (blockierend, Nachtrag 4b + Punkt 3) und M2 (Doku) beheben; die als erfuellt bewerteten Punkte (Modell,
Elza 1091, Fundort 1050/Spind 1000, Nachtrag 4c) nicht verschlechtern. Der Auftrag, der den Workflow ausgeloest hat
(Nachtrag 3b, Leuchtfarbe der Code-Panels), gehoert weiter zur Spur E codepanel (AUFTRAG.md Nachtrag 3b, VERTRAG.md
Abschnitt 0) — Spur I fasst keine Panel-Datei an.

### N1.M1 — leerer Loescher liegt in ROOM1090 auch ohne JA (Item-Box-Weg)

**Ursache (Code):** `feuerloescher_r37.c` `tor_offen()` Fall `RE15_FL37_FEUER_AUS` = `(3,0x81) &&
re15_inv_find_item(0x31) < 0`. `re15_inv_find_item` ist das Gegenstueck zu RE1.5 FUN_8004dfec und sucht NUR im
Inventar; die ITEM BOX (`g_itembox.slots[64]`, re15_itembox.h) ist ein eigenes Feld. "Nicht im Inventar" ist damit
kein Beleg fuer "an der Wegwerf-Abfrage Ja gesagt".

**Messung vorher (gebauter Stand = Code c0d3033d, exe 13:14:43 juenger als die letzte Code-Aenderung 13:13:32; Messkopie
`re15_pc_r37i_nb1_mess.exe`, Lauf-Skript wie die Abnahme, Rohdaten Scratchpad `inb1/`):**
* Lauf v8 = L8 der Abnahme: Karte ROOM1050 (19850,-22300) rot 1024 Cut 6, `flag:9:0x86 flag:3:0x85 flag:3:0x80
  flag:3:0x81 leer give:0x31:1`, `RE15_BOX_TEST=1`, Skript `W1,A0.1,W0.7,A0.1,W1,X0.1,W0.7,X0.1,W2,A0.5,W6`,
  Ende `200#1090`:
  ```
  [save] CONTINUE: resumed in room 1050 (hp=100)
  [feuerloescher] ROOM1050: kein Loescher (schon genommen, (9,0x86)=1)
  [feuerloescher] Name Item 0x31: Empty Fire Extinguisher
  [room] PC loaded room1090.rdt (336440 bytes)
  [feuerloescher] ROOM1090: Loescher liegt leer am Boden obj 5 bei (2500,-1800,-1300) rot_y 3250, Zone Slot 6 Untersuchen msg 29
  ```
* Lauf v8v (dieselbe Eingabe, Ende `135#1050`, Bild F130): ITEM BOX zeigt "Empty Fire Extinguisher", die Item-Liste
  (Inventar) ist leer (`I_belege/nb1_vorher_box_F130.png`). => Der Loescher liegt in ROOM1090 UND in der Box: M1
  reproduziert.

**RE-Belege (selbst disassembliert):**
* RE1.5 FUN_8004dfec (PSX.EXE, `re15_disasm.py dis 0x8004dfec 22`) — die Inventar-Suche kennt nur das Inventar:
  ```
  8004dff0  lbu  v0,4028(v0)       ; Zaehler 0x800b0fbc
  8004e014  addiu at,at,4268       ; Basis 0x800b10ac (Inventar-Id, Schritt 4)
  8004e01c  lbu  v0,0(at)          ; Id lesen
  8004e024  bne  v0,a0,0x8004e034  ; Id == gesucht? sonst naechster Platz
  ```
* RE2 FUN_800703b8 (Leon PSX.EXE, `re2_disasm.py dis 0x800703b8 420`) — Einlagern ist ein TAUSCH zwischen zwei
  Feldern (Basis s4 = 0x800cc1e8 @0x800703d8; +0x10000-30636 = 0x800d4a3c Inventar, +0x10000-30592 = 0x800d4a68 Box):
  ```
  80070400  lbu  v0,-30636(at)     ; Inventar-Id[Cursor] -> sp+16 (@0x80070408)
  800706b8  lbu  v0,-30592(at)     ; Box-Id[Platz]
  800706c8  sb   v0,-30636(at)     ; -> Inventar[Cursor]
  80070700  lbu  v0,16(sp)         ; alte Inventar-Id
  80070710  sb   v0,-30592(at)     ; -> Box[Platz]
  ```
  Der Gegenstand verlaesst das Inventar und liegt danach im Box-Feld — er ist NICHT weg.
* RE2 Wegwerf-"Ja" (dieselbe Datei, `dis 0x80051750 26`): `80051774 sb zero,-30636(at)` (Inventar-Id := 0),
  `80051794 sb zero,-30634(at)` (Groesse), `80051798 jal 0x80069714` (Verdichten) — das Box-Feld (-30592) wird nicht
  beschrieben. Nur dieser Weg nimmt den Gegenstand aus BEIDEN Feldern.
* Zensus der Port-Wege, die Item 0x31 aus `g_inv` nehmen (grep re15_inv_remove_slot / `.id = 0` in engine/ + platform/):
  Wegwerf-Ja `item_discard_common.c:353`; Kiste `re15_itembox_transfer` (Fall 0, `bs->id = old_inv.id`); Heilen
  `menu_common.c:712` nur ueber s_c3 = 3 fuer Ids 0x22..0x2F ohne 0x25 (`sltiu 0xe` @0x8004ab48, 0x31 -> "cant-use"
  s_c3 = 6 @0x8004ab6c); Kombinieren nie (Item-Tabelle @0x80074DA8 Zeile 0x31 @0x80074FF4 = `01 00 00 00 88 4c 07 80
  00 00 00 00`, Paarzahl @+9 = 0; kein Paar der Tabelle nennt 0x31 als Partner oder Ergebnis — Zensus-Skript
  `inb1/kombi_zensus.py`); `inventar_r36.c` nur Karte 0x21/Granaten mit Menge 0; `rolltor_1050.c` nur im eigenen
  Programm `k_mit`; `messer_rueckfall.c` nur Messer 0x01; Neues Spiel/Laden setzen Flags mit zurueck. => Ist (3,0x81)
  gesetzt und 0x31 weder im Inventar noch in der Box, dann hat Leon an der Wegwerf-Abfrage Ja gesagt.

**Wahl:** Abhilfe (a) der Abnahme — das Tor fragt zusaetzlich die Box. Kein neues Bit (VERTRAG 1.1 teilt Spur I keins
zu; Inventar UND Box reisen mit dem Spielstand: `re15_savedata.c:211` `re15_itembox_export(out->box)`, `:294` import).

**Aenderung:**
* `engine/src/feuerloescher_r37.c`: neu `in_der_kiste(id)` (Suche in `g_itembox.slots[0..RE15_BOX_SLOTS)`, Kommentar mit
  @0x80070400/@0x800706c8/@0x80070710 und @0x8004e014/@0x8004dff0); Tor `RE15_FL37_FEUER_AUS` = `(3,0x81) &&
  re15_inv_find_item(0x31) < 0 && in_der_kiste(0x31) < 0` (Kommentar mit @0x80051774 und dem Wortlaut 4b). Neue
  Messzeile bei geschlossenem Tor: `[feuerloescher] ROOM1090: kein Loescher (Leon hat ihn in die Item-Box gelegt (Item
  0x31 in der Kiste, Platz N))`; die drei alten Zeilen bleiben woertlich (Riegel A/G/I/J pruefen sie).
* `include/re15_feuerloescher_r37.h`: Kopf-Kommentar der Regel nachgezogen.
* `tests/unit/probe_r37_feuerloescher_karte.c`: Option `kiste:<id>:<n>` (naechster freier Box-Platz ab 2 = (Scroll 0 + 2)
  & 0x3f, die Auswahlformel von RE2 FUN_800703b8; die Kiste reist mit re15_savedata v4).
* Werkzeug `tools/r37_feuerloescher/kombi_zensus.py` (Zensus Kombinieren, t_addr aus dem PS-X-EXE-Kopf @0x18).
* Keine gemeinsame Datei beruehrt, kein Bit, keine Nachricht, kein Asset.

**Messung nachher (exe 14:35:46, Messkopie neu gezogen):**
* Lauf n8 = v8/L8 (gleiche Karte, gleiche Eingabe): 
  ```
  [feuerloescher] Name Item 0x31: Empty Fire Extinguisher
  [room] PC loaded room1090.rdt (336440 bytes)
  [feuerloescher] ROOM1090: kein Loescher (Leon hat ihn in die Item-Box gelegt (Item 0x31 in der Kiste, Platz 2))
  [flow] EXIT_AT: Bild 200 in Raum 1090 erreicht -> exit
  ```
  keine Zeile `liegt leer am Boden`, kein `prop-render ... oid=0x05`.
* Lauf n8r (ECHTE Box ROOM1150, Karte (-23000,-12100) rot 2048 wie L8r, Skript
  `W1,A0.1,W3,A0.1,W0.7,A0.1,W1,X0.1,W0.7,X0.1,W3`, `RE15_DEBUG_JUMP=1090@330`, `RE15_PLAYER_POS=2500,-2400,3072,1`):
  `[msg] room=1150 id=3 ITEMBOX (box screen, message suppressed)`, `[feuerloescher] Name Item 0x31: Empty Fire
  Extinguisher`, `[debug-menu] AUTO-JUMP -> ROOM1090 (Frame 330)`, `[room] PC loaded room1090.rdt`,
  `[feuerloescher] ROOM1090: kein Loescher (Leon hat ihn in die Item-Box gelegt (Item 0x31 in der Kiste, Platz 2))`.
  Bild `I_belege/nb1_nachher_1150_echte_box_F210.png` (ITEM BOX mit "Empty Fire Extinguisher", Item-Liste leer).
* Laeufe n9k / n9j (CONTINUE ROOM1090 (2500,-2400) rot 3072 Cut 6, Zustand nach der Rettung, einmal `kiste:0x31:1`,
  einmal Loescher weg = Wegwerf-Ja-Zustand): n9k `kein Loescher (Leon hat ihn in die Item-Box gelegt ... Platz 2)`, kein
  prop-render; n9j `Loescher liegt leer am Boden obj 5 bei (2500,-1800,-1300)` + `[prop-render] pi=4 oid=0x05
  pos=(2500,-1800,-1300) rot=(0,3250,0)`. Bild `I_belege/nb1_nachher_1090_cut6_kiste_vs_ja.png` (links Kiste: Boden vor
  der Wagentuer leer; rechts Ja: der Loescher liegt).

**Riegel:**
* NEU `unit_r37_feuerloescher_kiste` (16 Pruefungen, Mechanik am echten Port-Code): echter NEIN-Weg am Bild-Ablauf
  (loeschen_fahren: Feuer, msg 7/8, Ja, Selbst-Tuer, msg 9, Wegwerf-Abfrage NEIN) -> Einlagern ueber den ECHTEN Tausch
  `re15_itembox_transfer(Platz, 2)` -> Tor zu; Wiederbetreten (Spawn @0x00B7A) -> kein Prop obj 5, kein Platz msg 29;
  Name bleibt "Empty Fire Extinguisher"; Spielstand (capture/restore) traegt Kiste + (3,0x81) -> nach dem Laden liegt
  keiner; Herausnehmen (`re15_itembox_transfer(0, 2)`) -> weiter keiner; Gegenprobe: Loescher weg, Kiste mit Patronen
  0x15 -> liegt (2500,-1800,-1300) + msg 29. Der Raumstart `start(neu)` leert jetzt auch die Kiste (`re15_itembox_init`,
  Hygiene; die alten Teile fassten die Kiste nie an).
  **Gegenprobe ohne die Kisten-Pruefung** (Tor-Zeile voruebergehend ohne `in_der_kiste`, nur das Test-Ziel neu
  gebaut): `16 Pruefungen, 3 Fehler` —
  `FAIL: K Tor zu, solange der Loescher in der Kiste liegt (nicht weggeworfen)`,
  `FAIL: K Wiederbetreten mit dem Loescher in der Kiste: kein Prop obj 5, kein Platz msg 29 (kein Doppel)`,
  `FAIL: K geladen + Raumstart: keiner liegt`. Mit der Pruefung: `16 Pruefungen, 0 Fehler`; alle Teile in einem
  Prozess (`test_r37_feuerloescher` ohne Argument): `92 Pruefungen, 0 Fehler`.
* `integration_r37_feuerloescher` um die exe-Laeufe M/N/O erweitert: M = L8 (Box-Schirm `RE15_BOX_TEST`, Tuer 1050 ->
  1090), N = echte Box ROOM1150 + Sprung, O = CONTINUE in 1090 mit `kiste:0x31:1`; je `kein Loescher (Leon hat ihn in
  die Item-Box gelegt (Item 0x31 in der Kiste, Platz 2))` und KEIN `Loescher liegt leer am Boden` / `oid=0x05`. Der alte
  Stand liefert fuer dieselbe Eingabe genau die verbotene Zeile (Lauf v8 oben). Ergebnis einzeln:
  `^integration_r37_feuerloescher$` -> `100% tests passed, 0 tests failed out of 1` (362 s; im Log `ok   M/N/O: ...`,
  `r37_feuerloescher: alle Laeufe gruen`).

### N1.M2 — Abschnittsverweis im Kommentar feuerloescher_r37.c:82
Ursache: Der Kommentar der Tor-Regel nannte "Dossier Abschnitt 3.3" (dort steht der Spind); die Regel steht in 3.4.
Aenderung: Der Kommentar der Regel ist mit M1 neu geschrieben und verweist auf "Dossier Abschnitt 3.4 + Nachbesserung 1"
(`feuerloescher_r37.c`, Fall `RE15_FL37_FEUER_AUS`). Messung nachher: `grep -n "Abschnitt 3.3" feuerloescher_r37.c` ->
kein Treffer.
