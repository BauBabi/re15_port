# T2 — RE2-Tuersemantik + Port-Anschluss-Plan (Runde 31)

Auftrag: analysis/befunde_runde31/AUFTRAG.md (Nutzer woertlich). Teil T2: zu einer RE1.5-Tuer mit
zugeordnetem RE2-Archiv die RICHTIGE Variante, den RICHTIGEN Griff, den RICHTIGEN Ton — auch fuer
Tueren zwischen zwei Raeumen. Kein Spielcode; Werkzeuge unter re15_port/tools/tueren/re2_*.py.

Stand: IN ARBEIT (laufend nachgefuehrt, nach jedem Abschnitt committet).

## 0. Quellen und Werkzeuge

Herkunftsmarken: `@0x8...` = selbst disassemblierte Instruktion (RE2 = `info/re2leon/PSX.EXE`, t_addr 0x80010000,
`.claude/skills/re15-psx-disasm/scripts/re2_disasm.py`); `Datei 0x...` = Byte-Offset; `[SIM]` = Ergebnis des
Skript-Simulators `re15_port/tools/tor/tuerkatalog.py` (VM, Handler je Opcode mit PC-Vorschub-Adresse, siehe
analysis/tor_1170/03/04); `[BILD]` = selbst angesehenes Bild.

| Werkzeug (neu, nur lesend) | Zweck | Ausgabe |
|---|---|---|
| `re15_port/tools/tueren/re2_tuer_zensus.py` | alle 572 RE2-`Door_aot_set`/`_4p` lesen, Seiten bilden, zu physischen Tueren paaren | `build/r31_tueren/t2/re2_tueren.json` |
| `re15_port/tools/tueren/re2_tuer_varianten.py` | je Archiv und Variante: Rollen der Objekte, Angel/Griff im Bild, Richtung, Bilder Anfang/Mitte | `build/r31_tueren/t2/varianten.json`, `bilder/DOORxx_vN.png`, `bogen_varianten_NN.png` |

Satzformat (selbst gelesen):
- `0x3B Door_aot_set`: Handler `0x80054be4` (Tabelle `0x800a74c8[59]`), Satzzeiger = pc+2 (`@0x80054c30 addiu v0,v0,2`),
  Laenge 32 (`@0x80054c40 addiu v0,v0,32`). Rechteck pc+6..13, Nutzlast pc+14.
- `0x68 Door_aot_set_4p`: Handler `0x80054c50` (Tabelle `[104]`), Satzzeiger pc+2 (`@0x80054c9c`), sat |= 0x80
  (`@0x80054cb4 ori v0,v0,0x80`), Laenge 40 (`@0x80054cc4 addiu v0,v0,40`). Vier Punkte pc+6..21, Nutzlast pc+22.
- Nutzlast +12 = Archiv (`@0x80015088 lbu v1,12(v0)`), +13 = Variante & 0x7f / Bit 7 (`@0x80013e5c`, `@0x80013e6c andi 0xff7f`,
  `@0x80013e84 andi 0x80`), +15/+16 Schloss (`@0x800515a8`, `@0x800515d0`); Rest 03_tuersequenz 4.
- Tuerkamera: Auge (10000,0,0), Ziel 0, H 290 -> Bild-x = 160 + 290*z/(10000-x) (03 3.2; Bildmitte (160,120) aus den
  Spiel-Aufrufern `@0x80049cac/b0`, `@0x80068e80/88`).


## 1. Variante <-> Griff je Archiv (Angel/Griff links/rechts, weg/hin, Meshes)

**Verfahren.** `re2_tuer_varianten.py` faehrt jede Variante, die Skript 0 verteilt (Case-Werte des `Switch var 12`),
durch den Simulator `[SIM]` und nimmt Bild t0 (erstes Bild mit allen Objekten) und das Bild, in dem die Bewegung des
Blatts endet (Kamerafahrt herausgerechnet). Angel = Ursprung des bewegten Blatt-Meshes (03 9.2: Ursprung = Drehachse),
freie Kante = Blatt-Ecke mit dem groessten |z|, Griff = Kind-Objekt des Blatts (Flag 0x10, Eltern = Blatt) mit
Mesh-Hoehe < 3000. Bild-x nach der Tuerkamera (Abschnitt 0). "links" < 150, "rechts" > 170, dazwischen "mitte".
"weg"/"hin" = die freie Kante geht in der Offen-Stellung um mehr als 50 Einheiten nach -x / +x (Kamera steht bei +x).
Sichtbar ist ein Griff, wenn sein Weltpunkt in t0 vor der Blattebene liegt (x groesser als die Angel).
Die Tuerkamera zeigt die Tuer so, wie der Spieler VOR ihr steht: "Griff links" heisst also links, wenn man die
Tuer von dieser Raumseite ansieht.

Bilder: je Archiv/Variante `build/r31_tueren/t2/bilder/DOORxx_vN.png` (Bild t0 mit Marken Angel/Griff | Bild Mitte der
Bewegung); Uebersicht aller Archive (je zwei Seiten) im Repo: `analysis/befunde_runde31/tueren_belege/t2_varianten_anfang_mitte.jpg`
`[BILD]`. Vollstaendige Tabelle aller 152 Varianten (inkl. Bit-7-Laeufe) in Anhang A1.

### 1.1 Ergebnis nach Gruppen

| Gruppe | Archive | V0 | V1 | weitere Varianten |
|---|---|---|---|---|
| Einfluegelige Drehtuer, Standardaufstellung | 00 01 02 03 04 05 06 07 08 09 0A 0B 0D 13 15 17 18 1A 1C 1D 22 23 24 29 2E 2F (26) | Angel **rechts** (Bild-x 226..234), Griff **links**, Blatt geht **weg** (aufdruecken) | Angel **links** (86..107), Griff **rechts**, Blatt kommt **hin** (aufziehen) | s. u. |
| Doppeltuer, ein Fluegel geht | 01 04 06 15 1D: V2 / V3; 11: V0=V2, V1=V3 | V2: rechter Fluegel (Angel x 289/290) geht weg, linker steht | V3: linker Fluegel (Angel x 30) kommt hin, rechter steht | 15 V2/V3 wie 01 |
| Doppeltuer, beide Fluegel gehen | 0C (V0 hin, V1 weg), 1B (V0=V2 hin, V1=V3 weg), 0D (V2 hin, V3 weg), 30 (V0 weg, V1 hin) | | | Angeln aussen (x 26/289) |
| Schott mit Handrad | 26, 31 | V0: zweiteilig, linker Teil (Angel x 51) geht weg, Rad links; V1: Angel rechts (269), Rad rechts, hin | | V2: einteilig, Angel links (106), Rad rechts, weg; V3: Angel rechts (214), Rad links, hin |
| Schiebetuer | 10 (Scherengitter), 14 (Maschendraht), 19, 27, 2A, 2C | 10: Pfosten links, schiebt nach links; 14: nach links, Griff rechts; 19: nach rechts; 2A: nach rechts | 10: Pfosten rechts; 14: nach rechts, Griff links; 19: nach links; 2A: nach links | 27 V0..V3 gleich (zweiteilig); 2A V2/V3, 2C V0..V3 zweiteilig |
| Hub-/Klapptuer | 1F (zwei Platten), 25 (Aufzugtuer, hebt), 2B (Klappe, hebt) | 1F: Griffplatte links; 25/2B nur V0 | 1F: Griffplatte rechts | - |
| Klappe / Lueftung | 1E, 33, 35 | V0/V1 (Skript 1, gleich), V6 (Skript 2, naeher), V7 (nur Rahmen), V8 (Kette, Kamerafahrt) | | Klappe dreht um z (-1600) |
| Treppe | 0E, 0F, 12 | - | - | **V4 = hinauf** (Stufen mit Setzstufen, Modell sinkt und kommt naeher), **V5 = hinab** (Trittflaechen unter dem Horizont) |
| Leiter | 16 | - | - | **V4 = hinauf** (Leiter faehrt nach unten, y +300), **V5 = hinab** (y -300) |
| Bodenluke | 28 | V0/V2/V4 (Skript 2, 281 Bilder) | V1/V3/V5 (Skript 1, 301 Bilder) | Kamerafahrt, keine Blattbewegung |
| Hubbuehne | 2D | V0/V2/V4 (Skript 1, 301) | V1/V3/V5 (Skript 2, 451) | Kamerafahrt |
| Ohne Objekt | 20 21 32 34 36 | nur Blende + Ton (271..321 Bilder) | | Uebergaenge per Skript (Rechteck 0/20000/25000) |

Beleg der Standardgruppe am Skript (Datei-Offsets `[SIM]`, Bytes selbst gelesen): DOOR00 Skript 1 @Datei 0x5036
`4d 00 00 00 01 00 a0 0a 10 00 d0 07 ce 0e 00 08 ...` = Blatt Obj 0 bei (2000,3790,2048) Drehung 0; Knauf Obj 1 als Kind
bei (130,-3224,-3372). Bild-x Angel = 160 + 290*2048/8000 = 234,2; Knauf 160 + 290*(2048-3372)/(10000-2000-130) = 111.
Skript 2 stellt das Blatt bei z = -1600 mit Drehung y = 2048 auf -> Angel 160 - 290*1600/8000 = 102.

### 1.2 Meshes: Blatt / Griff / Anbauteil

- **Blatt:** 36 Archive benutzen als Mesh 0 dieselbe Platte (8 Ecken, 12 Dreiecke, 288 x 6602 x 3599; 34 davon auch mit
  denselben UV): 00 01 02 03 04 05 06 07 08 09 0B 0C 0D 11 13 15 17 1A 1B 1C 1D 20 21 22 23 24 25 29 2A 2C 2F 32 34 36
  (+ 18/19 geometriegleich, andere UV). Unterschied zwischen diesen Tueren = nur Textur (Blatt v 0..218) und Beschlag.
- **Griff/Beschlag:** Mesh 1 (seltener 2/3), immer Kind des Blatts (Flags 0x00d0/0x04d0 = Eltern Obj 0), Anhaengepunkt
  x = +130 (Vorderseite) bzw. -130 (Rueckseite, Drehung x 2048 oder z 2048), y -2464..-4234, z -2660..-3594
  (= 5..940 Einheiten von der freien Kante z = -3599). Tabelle Abschnitt 3.
- **Anbauteile:** fester zweiter Fluegel (Wurzelobjekt ohne Bewegung: 01/04/06/11/15/1D V2/V3), Pfosten (10 Mesh 1),
  Schild/Tafel (1F Mesh 2/3, 29 Mesh 3), zweite Platte (1F Mesh 1), Rahmen (1E/33/35 Mesh 1), Fahrkorb/Gelaender (2D).
- **Vorder- und Rueckseitengriff:** V1 setzt bei 00 01 02 03 04 05 06 08 09 0A 0B 0D 11 13 15 17 18 1A 1C 1D 23 24 2E 2F
  (24 Archive) zwei Griff-Objekte (vorn + hinten; das Blatt zeigt beim Aufziehen seine Rueckseite), V0 nur den vorderen.
  Nur einen Griff in V1: 07 0C 14 19 1B 26 2C 31. (29 V1: Tafel vorn + hinten, Schild; 1F: drei Kinder.)

## 2. Paar-Regel in RE2 (572 Door_aot_set -> physische Tueren, Variante je Seite, Griff im Hintergrund)
(folgt)

## 3. Griff-Formen je Archiv (Knauf/Druecker/Ring/Stange/keiner, Mesh, Anhaengepunkt, Austauschbarkeit)
(folgt)

## 4. Ton je Archiv (Tonteil, Se_on-Bilder, Door_exit-Ton, Tonfamilien, Treppen/Leitern)
(folgt)

## 5. Treppen/Leitern/Aufzug/Schott (Varianten 4/5, Archive 0E/0F/12/16, 1E/33/35, 25/29, 2B, 2D)
(folgt)

## 6. Port-Anschluss-Plan (Datei:Zeile, RE2-Adressen, Schrittfolge fuer den Bau-Agenten)
(folgt)

## 7. Offen / nicht belegt
(folgt)
