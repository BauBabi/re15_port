# C — Kreuz-Zeitleiste Leon <-> Elza: Bedingungen, Graph, Widersprueche, Vorschlaege

Stand 2026-10-10 (Runde 36, Spur P). NUR ANALYSE, kein Port-Code geaendert. Eingaben: `A_zensus.md`, `B_figuren.md`,
`ablauf_leon.md`, `ablauf_elza.md`, `zensus.md`, `texte_leon.md`/`texte_elza.md`, `flag_xref.txt`, `scd_annot/`;
Port-Ergaenzungen aus `analysis/befunde_runde34_nacht/AUFTRAG.md`, `befunde_runde35/K_cut10f0.md`, `L_cut1150.md`,
`befunde_runde36/AUFTRAG.md` + `VERTRAG.md`. Werkzeug: `zeitleiste.py` (dieser Ordner) -> `zeitleiste.md`
(Graph, Zyklen, topologische Sortierung) und `zeitleiste_ohne_vorschlag.md` (nur Original-/Port-/Nutzer-Kanten).
Belegform: `ROOMxxxx region@0xOFFSET` = Datei-Byte-Offset in `re15_port/shared_assets/PSX/STAGEn/ROOMxxxx.RDT`,
`msg N` = Nachrichten-Index (Text in `texte_*.md`), Flags `(Bank,Bit)`. Unsicheres ist als **VERMUTUNG** markiert.
Ereignis-Ids (L.., E.., P.., EPW/EPK) = Knoten in `zeitleiste.py`/`zeitleiste.md` §1.

---

## 0. Empfehlung (Kurzfassung)

1. **Modell (c) "parallel, raumweise versetzt" ist das einzige widerspruchsfreie.** (a) "Elza ganz nach Leon" und
   (b) "Elza ganz vor Leon" erzeugen Zyklen aus Original-Bytes (§2). Das Muster ist RE2-Zapping: beide sind
   gleichzeitig im Revier, nehmen verschiedene Wege und treffen sich erst am Zug (ROOM6030/6031).
2. **Die Reihenfolge je Gebaeudeteil ist eindeutig** (§3): Elza ist im Keller (Generator, Garage/Sherry, Draht,
   "Prisons"-Tuer, Zelle, Roy) VOR Leon; Leon ist im Hinterhof (Ada), im Funkraum, bei Irons (Tod) und bei den
   Knall-Folgen (Fenster 1120) VOR Elza; am Schacht in der Garage ist Leon (Container schieben) vor Elzas Rueckkehr.
   Global: `E01..E13 (Elza Keller-Akt)` < `L17..P08 (Leon Strom..Montage)` < `E14..E20 (Elza 3F/Dach/Kanal)`,
   dazu `L19/L20` < `E21`, `L26` < `E31` < `L27`, `L33` < `E40`, `L36 = E43`. (Hart belegt ist `E09..E11 < L11E/L18`;
   `E12/E13 < L17` folgt aus der Vorschlags-Kante `E13 < L17` bzw. ueber die optionalen Raeume 1020/1210.)
3. **Nutzer-Vorgaben passen in dieses Modell** (Irons tot bei Elza: `P07 < E15`; Fenster 1121 immer kaputt:
   `P08 < E14`) — mit zwei weichen Stellen, weil Elza 1121/1151 auch VOR Roys Tod erreichen kann (§4.3).
4. **Fuenf echte Widersprueche/Luecken** (§4): (1) ROOM30C1 msg 3 "We got separated back at the RPD" (einziger
   Satz, der eine Begegnung behauptet) -> Satz aendern; (2) Roys Leiche in Leons 1020 kann VOR Elzas Szene gesehen
   werden -> an (4,0xF3) gaten; (3) Elzas 1091 kann VOR Leons Rettung besucht werden -> Tuer 1051->1091 an (4,0xE9)
   gaten oder hinnehmen + Spur; (4) Elzas Parkplatz-Tuer wird durch Spur Q (Parking-Lot-Schluessel nur fuer Leon)
   fuer immer zu -> ROOM11C1 (5 Hunde) unerreichbar, Text "The door is locked." setzen; (5) der Generator wird in
   beiden Szenarien gestartet -> Elzas eigener Satz "The building's electric power is starting to fail..."
   (ROOM1021 msg 5) erklaert es, wenn Elza VOR Leon startet (Vorschlags-Kante `E13 < L17`).
5. **Kendo** (§4.1): Zelle = ROOM1211 (nicht 1221); wer ihn einsperrte, sagt KEIN Text — kleinster Eingriff: EIN
   neuer Satz Johns in 1211 ("Roy locked me in here to sleep it off...") + bei Leon in 1210 EIN Text-AOT an der
   Zelle ("The cell is open. Whoever was in here is gone."). Leon sieht sonst nichts (Original: kein Text).
6. **Werkstatt-Tuer 11B0** (§4.5): sie ist in KEINER Variante eine Tuer. Bei Elza (11B1) ist sie ein Text-Platz
   "The door is locked." (Slot 8, nur solange (3,0x83)==0), ohne Strom "It's too dark..."; bei Leon (11B0) gibt es
   dort KEINEN AOT. "Ohne Strom zu, mit Strom offen" beschreibt die PARKPLATZ-Tuer (Slot 1 -> 11C0/11C1), die der
   Strom-Poll ohne Strom durch "It's too dark..." ersetzt. Vorschlag: Leon 11B0 von Anfang an "The door is locked."
   (Elzas msg-12-Bytes), Marvins Abschliessen (Spur J) = "sichergehen/verriegeln"; Elza unveraendert.
7. **Spuren** (§5): bestehend 8 (Elza->Leon) + 7 (Leon->Elza); vorgeschlagen 9 kleine Ergaenzungen (nur Texte,
   ein Prop, zwei Tuer-Gates), alle additiv, Original-Inhalte beider Szenarien bleiben.

---

## 1. Was fuer die Zeitleiste zaehlt (Methode)

Ein Raumpaar xxx0/xxx1 liefert eine Bedingung "X war vor Y in R", wenn der Zustand des einen Szenarios genau der
Zustand ist, den eine Szene des anderen Szenarios HERSTELLT (gleiche Objekt-Position, gleiche Spawn-Records,
gleiches Flag). Blosse Unterschiede (andere Gegner, andere Items) liefern keine Reihenfolge. Die Kanten stehen in
`zeitleiste.py` (CROSS/VORSCHLAG) mit Art:
- **hart** = Original-Bytes beider RDTs,
- **port** = Port-Mechanik (Runden 34-36),
- **nutzer** = Nutzer-Vorgabe Runde 36,
- **vorschlag** = Empfehlung dieses Dossiers (laesst sich mit `--ohne-vorschlag` weglassen; Ergebnis je Modell
  aendert sich dadurch NICHT, s. `zeitleiste_ohne_vorschlag.md` §0).

Korrektur gegenueber `ablauf_leon.md`/`ablauf_elza.md`: **Die Pliers brauchen in BEIDEN Varianten den Strom.**
ROOM11B0 sub01@0x011F6 `21 04 f3 00` (Ck(4,0xF3)==0) -> @0x01220 `46 04 01 31 0f 00` (Aot_reset Slot 4 = das
Pliers-Item main00@0x00FF2 -> Text msg 15 "It's too dark to see anything..."); ROOM11B1 sub01@0x011A0 -> @0x011CA
(Slot 4 -> msg 14). Damit gilt: Leon L17 (Strom) < L18 (Sherry rennt, erster Garagen-Eintritt mit Strom:
main00@0x00F56 `22 04 06 01` nur bei (4,0xF3)==1, sub01@0x011B6) < L12a (Pliers); Elza EPW (Strom) < E09 (Pliers,
dann Sherry). Der Kesselraum ist ohne Pliers erreichbar: ROOM11E0 Kessel-Tuer Slot 0 @0x01512 (-27100,-10300) liegt
ueber die SCA-Zellen 3-5-4-2-7-10 (SCA @0xF7C, 5x22 Eintraege) erreichbar; das Drahtgitter (Zelle 0, x -14100..-6800,
z -12600..-10600, sub00@0x017B0ff `37 0r 00 ff` = solid) sperrt nur die Leichenhalle (Slot 1 @0x01532 ueber Zellen
0-1-11) und die Abkuerzung (Zelle 9).

---

## 2. Modell-Pruefung (zeitleiste.py, Zyklen = Widersprueche)

| Modell | Ergebnis | Beteiligte Original-Kanten (Auszug aus `zeitleiste_ohne_vorschlag.md`) |
|---|---|---|
| (c) parallel / raumweise versetzt | **widerspruchsfrei** (keine Zyklen; topologische Sortierung in `zeitleiste.md` §3 Modell c) | — |
| (a) Elza ganz nach Leon (`L37 < E01`) | **Widerspruch** | `E09 < L18` (Sherry steht bei Elza an (-14886,-24048), ROOM11B1 sub02@0x011FC, und rennt bei Leon von genau dort weg, ROOM11B0 sub04@0x01324/@0x013AC); `E11 < L11E` ((3,0x8B) aus ROOM1240 sub02@0x0055A = Zustand nach Elzas Kartenleser ROOM11E1 sub01@0x01A8A); `E31 < L27` (Marvin in Elzas 30C1 ungebissen, in Leons 30E0 gebissen zurueck); `E42 < L36` (Zug) |
| (a1) Elza beginnt nach Leons STAGE1 (`P09 < E01`) | **Widerspruch** | `E09 < L18`; `E11 < L11E` |
| (b) Elza ganz vor Leon (`E44 < L01`) | **Widerspruch** | `L03 < E17` (Hubschrauber nur in ROOM1170 sub15 obj 2-5); `L18/L20 < E21` (Sherry weg, Container bei z=-19368: ROOM11B1 sub00@0x01122 == ROOM11B0 main00@0x010EA); `L18 < E22`; `L33 < E40` (Ada-Leiche); `L35 < E43` (Zug) |
| (b1) Elzas STAGE1 vor Leons STAGE1 (`E22 < L01`) | **Widerspruch** | `L20 < E21`, `L18 < E21/E22`, `L03 < E17`; mit Nutzer-Vorgaben zusaetzlich `P07 < E15`, `P08 < E14` |

Fazit: Die Nutzer-Idee "Elza spielt nach Leon" ist als Ganzes nicht haltbar (Sherry, "Prisons"-Tuer, Marvin); sie
gilt aber fuer Elzas 3F-Akt und alles danach (E14..E44) gegenueber Leons STAGE1-Ende — genau das, was die beiden
Nutzer-Vorgaben (Irons tot, Fenster kaputt) ausdruecken. Umgekehrt ist Elza im Keller (E09..E13) VOR Leons
Keller-Akt (L17..L12b). Das ist "parallel mit raumweisem Zeitversatz".

---

## 3. Die Bedingungen "X war vor Y in Raum R" (je mit Beleg)

### 3.1 Original (hart)

| # | Bedingung | Raum | Beleg |
|---|---|---|---|
| H1 | Elza (E13 Roy erschossen) VOR Leon (L20a) | 1020/1021 | Leons Leiche obj 6 (-4488,0,-10684) unbedingt (ROOM1020 main00@0x01DEE `2d 06 ... 78 ee`) = Elzas Leiche (ROOM1021 main00@0x01E40, nur bei (3,0x5F)==1, gesetzt sub02@0x01FE4); Leon msg 0 "died recently" |
| H2 | Elza (E12 John befreit) VOR Leon (L21a) | 1210/1211 | 10 Gefangenen-Zombies bei Leon ohne Flag (ROOM1210 main00@0x01D86..@0x01E3A), bei Elza erst nach (3,0x2C) (ROOM1211 sub00, gesetzt sub02@0x01F8A); Leons 1210 hat keinen Zellen-Text (Elza: Slot 8 msg 10 @0x01DEE "someone inside") |
| H3 | Elza (E11 Kartenleser) VOR Leon (L11E erster B2-Besuch) | 11E0/11E1 | (3,0x8B) wird nur von den Kartenlesern 11E0/11E1 sub01@0x01A8A UND von Leons Vorspann ROOM1240 sub02@0x0055A gesetzt (flag_xref Z.1501-1509); Leser 11E0 main00@0x01576 -> Slot 3 scharf @0x016C4 |
| H4 | Leon (L10 Rettung) VOR Elza (E91) | 1090/1091 | ROOM1091 main00@0x0216E..@0x021AA (4 Zombies, Kill-Bits 0xf8-0xfb) bytegleich ROOM1090 sub00@0x023A0..@0x023DC (`Ck(3,0xBB)==1`); Feuer Typ 0x26 (@0x02214ff) und Schuppen-Tuer Slot 3 @0x02352 nur in 1090; 1091 hat 0 Texte |
| H5 | Elza (E09 Sherry wartet im Auto) VOR Leon (L18 Sherry rennt) | 11B0/11B1 | gleiche Startposition (-14886,0,-24048): ROOM11B1 sub02@0x011FC vs ROOM11B0 sub04@0x01324; Leon: Lauf zu (-1828,-21753) @0x01404, Pos_set -30000 @0x013AC |
| H6 | Leon (L18) VOR Elza (E21 "Where has she gone!?") | 11B1 | ROOM11B1 sub01@0x01192 (3,0x46) -> sub03 msg 10/11 |
| H7 | Leon (L20 Container geschoben) VOR Elza (E21 kommt durch den Schacht) | 11B0/11B1 | ROOM11B1 sub00@0x010FA `21 03 83 01` -> @0x01122 Member_set obj 0 z=-19368 = ROOM11B0 main00@0x010EA obj 0 (-2168,0,-19368) bei (4,0xCD)==1; **dasselbe Flag (3,0x83)** setzt Leon in 11B0 sub06@0x01478 und Elza in 10B1 sub04@0x0195E (flag_xref Z.1458-1463) = "Schacht offen" |
| H8 | Leon (L18) VOR Elza (E22 Sherry im Kanal gefunden) | 2000/2001 | ROOM2001 sub00@0x0180A Spawn 0x4B; Leon ROOM2000 msg 2-6 "I saw her coming in this way" |
| H9 | Leon (L26 Lift) VOR Elza (E31 Marvin allein) VOR Leon (L27 Marvin gebissen) | 3080/30C1/30E0 | ROOM3080 @0x007AC/@0x007C0/@0x007D4 (Birkin, Ada, Marvin); ROOM30C1 sub03 msg 2/4; ROOM30E0 sub11@0x0108C/@0x010A0, msg 10/11 |
| H10 | Leon (L33 Ada stirbt) VOR Elza (E40) | 50B0/50B1 | ROOM50B1 sub00@0x004F2 obj 0 (-11800,0,-11600) = ROOM50B0 Leiche @0x00566 |
| H11 | Leon (L03 Hubschrauber weg) VOR Elza (E17) | 1170/1171 | Hubschrauber = obj 2-5 nur in ROOM1170 sub15@0x017E6..@0x0184C |
| H12 | Leon L36 = Elza E43 (gleichzeitig) | 6030/6031 | "Where's Elza?"/"She's out there fighting the monsters!" (6030 msg 1/2); "Where's Leon?"/"He's fighting off monsters." (6031 msg 0/1) |

### 3.2 Port-Ergaenzungen und Nutzer-Vorgaben (was Elza dann sehen MUSS)

| # | Bedingung | Folge fuer Elza | Beleg |
|---|---|---|---|
| U1 | Leon P07 (Irons stirbt) VOR Elza E15 | Irons tot in 1151, "He has already passed..." (Nutzer); Sicherung im Hebetisch bleibt entnehmbar (Port gilt fuer 1150 UND 1151) | AUFTRAG r36 Z.36; irons_tisch_1150.c; L_cut1150.md §8.2 |
| U2 | Leon P08 (Montage) VOR Elza E14 | Fenster 1121 kaputt + 1 Kraehe (Nutzer). Konsequent waeren ausserdem (Vorschlag V6): 1041 Rolltor OFFEN + 5 Zombies, 1031 Tor unterkrochen + Zombies vor dem Tor, 1131 Rest-Zombies aus 1140 — aber erst NACH Roys Tod ((3,0x5F)), s. §4.3 | AUFTRAG r36 Z.34; L_cut1150.md §7 ("Die in der Montage erschienenen Zombies bleiben") |
| P1 | Leon P06 (Parking-Lot-Schluessel) + P09 (Marvin schliesst ab) | Elzas Parkplatz-Tuer 11B1 Slot 1 ist zu JEDEM Zeitpunkt verschlossen (vor L19: Schluessel-Sperre; nach L20: Marvins Schloss) -> ROOM11C1 (5 Hunde, sub00@0x016F8ff, einzige Tuer zurueck Slot 0) ist fuer Elza unerreichbar | Spur Q/J; ROOM11B1 main00@0x00F9C Slot 1 -> 11C1 |
| P2 | Leon P09 (Werkstatt-Tuer) | Elza sieht die Werkstatt-Tuer ohnehin "locked" (Slot 8 msg 12) — keine Aenderung | ROOM11B1 sub00@0x0112A |
| P3 | Elza EPK (Prison Key in 10F1, Spur Q) VOR Leon L16/P05 (10F0) | Elza ist VOR Leon im Funkraum (EPK < E12; mit `E13 < L17` oder EPK < E11 < L11E: EPK < L17 < ... < L16): Leon findet den Prison-Key-Platz leer (Spur K3) | Kette §3.3; EPK/E11 untereinander frei |
| P4 | Port-Dokumente (beide Varianten) | "Elliot's Diary" (1000), "Marvin's Notes" 4312 (1020), "Armory Notice" 5632 (1010), "Police Officer's Final Diary Entry" (1050) liest auch Elza — keine Zeitaussage, konsistent | dokumente_r34.c:57 (room_id & 0xFFFE) |

### 3.3 Daraus folgende Gesamt-Reihenfolge (eine gueltige Linearisierung aus `zeitleiste.md` Modell c)

```
Phase 1 (unabhaengig, beliebig verschraenkt):
  Leon  L01 Vorspann, L02/L03 Heliport (Elliot fliegt ab), L04, L05 Irons LEBT, L06 Abstieg, L07 1050,
        L08 Ada ruft, P03, L09 Feuerloescher, L10 Ada gerettet, L11 Ada geht
  Elza  E01 Vorspann, E02 Lobby, E03 SIG P228, E04 Schuss, E05 1021, E06 Roy verwundet, E07 Zettel 0513,
        E08 Lobby-Tor haelt
Phase 2 (Elza im Keller, VOR Leons Keller-Akt):
  Elza  EPW Strom, E09 Pliers + Sherry, E10 Draht, EPK Prison Key (10F1), E11 "Prisons"-Tuer, E12 John frei,
        E13 Roy erschossen ("electric power is starting to fail")
Phase 3 (Leon):
  Leon  L11E B2 (Tuer schon offen), L17 Strom, L18 Sherry rennt, L12a Pliers, L12b Draht/Minidisc, L13, L14,
        L15 Marvin, L16/P05 Funkraum-Szene, P06 Schluessel, P04, P07 Irons stirbt, P08 Montage
        (optional dazwischen: L20a Roy-Leiche, L21a Zelle leer)
Phase 4 (Elza 3F/Dach, NACH Leons Montage):
  Elza  E14 1121 Fenster kaputt, E15 Irons tot, E16 John/Gorillas, E17 Dach (kein Hubschrauber), E18 Vorhof ->
        Kanal, E19, E20 (optional E91 Hinterhof)
Phase 5 (Leon verlaesst das Revier):
  Leon  L19 Ada + Gorillas, L20 Schacht (Container), P09 Marvin schliesst ab, L21 Kanal
Phase 6 (Elza zurueck in die Garage):
  Elza  E21 Container verschoben/Sherry weg, E22 Sherry gefunden, E23.. Kanal
STAGE2-6:  L22..L26 ; E24..E30 ; E31 (Marvin allein) ; L27..L33 ; E40 (Ada-Leiche) ; L35/E42 ; L36 = E43
```
Phase 2 vor Phase 3 folgt aus H3 (E11 < L11E) und H5 (E09 < L18) und mit Spur Q aus EPK < E12; Phase 3 vor Phase
4 aus U1/U2; Phase 5 vor Phase 6 aus H7. Die Vorschlags-Kante `E13 < L17` (Generator) macht Phase 2 vollstaendig
vor Phase 3 — ohne sie bleibt nur E12 < L11E (wegen E11 < L11E, E12 danach) unbestimmt zu L17.

---

## 4. Widersprueche und offene Fragen -> Vorschlaege (kleinster Eingriff, nur ERGAENZEN)

### 4.1 Kendo/John — wer sperrte ihn ein, warum, was sieht Leon

Befund (Original): Zelle in **ROOM1211** (Spawn sub00@0x01E36 (-26225,0,-25100); ROOM1221 ist bytegleich und hat
keinen NPC). Kein Text nennt den Einsperrenden; Hinweise msg 1 "I'm trying to sleep here!", msg 2 "my head is
killing me", Roy nennt ihn "my friend John" (ROOM1011 msg 15). Befreiung nur ueber (4,0xEC) (Original: Roys Hand,
ROOM1011 sub06@0x00E2C; Port Q: Prison Key in 10F1). John flieht mit Elza (1021, 1141, 1171, 10B1 -> Kanal; stirbt
nie, Biss in 30E1). Leon: 1210 hat nur Tueren + den Auto-AOT Slot 6 @0x01EAE (gleiches Rechteck wie Elzas Slot 7
@0x01E72) — **kein Text, keine Spur** an der Zelle; die 10 Zombies sind frei (H2).

| Vorschlag | Eingriff | Port |
|---|---|---|
| K1 (empfohlen) | EIN Satz Johns in 1211 nach msg 2, z.B. "John: Roy locked me in here to sleep it off... said it was the safest place in the building." (VERMUTUNG Ausnuechterungszelle, durch msg 1/2 gedeckt; erklaert wer + warum + Roys "my friend") | ROOM1211 neue msg 11 (Spur L: "1211 ab RDT-Zahl, max +3"), Message_on im Port-Programm der Szene sub02 zwischen @0x02028 (msg 2) und @0x02062 (msg 3) |
| K2 (empfohlen) | Leon 1210: Text-AOT an Elzas Zellen-Rechteck (ROOM1211 sub00@0x01DEE `2c 08 01 31 00 00 5c aa 00 a8 0e 03 ...` = x -21924, z -22528, w 782) mit "The cell door is open. Whoever was locked in here is gone." | ROOM1210: neuer Text-Platz (Muster re15_tuer1120.h: sce-1-Platz), neue msg 0 (1210 hat 0 Nachrichten) |
| K3 (optional) | Leon 10F0 nach dem Parking-Lot-Schluessel: "The hook next to it is empty. The tag reads 'Prison'." (Spur, dass jemand den Prison Key vorher nahm = Elza, P3) | Spur Q: Examine-Text am Schluessel-Platz nach (9,82)=1 |

### 4.2 Roy

Befund: Elza E06/E07 (lebend, 1011), E13 (als Zombie erschossen, 1021); Leon L20a (Leiche an EXAKT Elzas Position,
"died recently", unbedingt). Weicher Punkt: Leons 1020 ist ab 1040 frei (ROOM1040 Slot 3 @0x010F8 / ROOM1020
Slot 2 @0x01CC2) und enthaelt nur Munition + Port-Dokument — Leon KANN vor Elzas Szene dort sein.

| Vorschlag | Eingriff | Port |
|---|---|---|
| R1 (empfohlen) | Leons Roy-Leiche (obj 6 + Slot 5 @0x01DDA) erst zeigen, wenn (4,0xF3)=1 (Strom, L17). Mit `E13 < L17` (Generator-Lesart §4.6) liegt sie dann sicher nach Elzas Szene; vorher ist der Platz leer | ROOM1020: obj 6 + AOT Slot 5 im Port-Installer an (4,0xF3) gaten (Muster: bedingte Obj_model_set wie 11B0 main00@0x010BC) |
| R2 (optional) | Leon 1010 (Roys Verwundungsort (-2000,0,7450), ROOM1011 sub00@0x009F6): Text-Platz "A pool of blood on the floor. Someone wounded was lying here." (ohne "fresh": E06 und Leons 1010-Besuch sind unbestimmt) | ROOM1010: neuer Text-Platz, neue msg 3 (1010 hat 3 Nachrichten) |
| R3 (keine Aenderung) | Schuss in 1041 (E04, msg 3 "gunshot", Schuetze unbenannt): als Roys Schuss (VERMUTUNG, er ist verwundet und bewaffnet) ODER als Leons Schuss im selben Korridor (L06 Abstieg durch 1040) lesbar — beides ohne Text moeglich | — |

### 4.3 Irons tot bei Elza / Fenster 1121 / Montage-Folgen

Befund: Original beide lebend (1151 sub03 msg 2, Flag (3,0x9D) ohne Leser ausserhalb 1150/1151, flag_xref Z.1629).
Nutzer: tot. Mechanisch haengt nichts daran; die Zeitleiste verlangt P07 < E15 und (Fenster) P08 < E14. Weich: Elza
erreicht 1121/1131/1151 schon in Phase 1 (1061 Slot 0 -> 1121 @0x00D12 ohne Bedingung) — dann saehe sie Leons
Folgen, BEVOR sie passiert sind (im globalen Modell).

| Vorschlag | Eingriff | Port |
|---|---|---|
| I1 (Nutzer-Entscheidung, bleibt) | Irons immer tot, Fenster immer kaputt (statisch) — Preis: weicher Punkt oben | Spur L |
| I2 (Alternative, konsistenter) | Beides erst ab (3,0x5F)=1 (Roy tot, E13 = Elzas letztes Ereignis vor Phase 3): Elzas Pflichtweg nach 3F (E14, Gate (4,0xF0) aus 1021 sub02@0x01FE0) liegt ohnehin danach; nur ein frueher optionaler 3F-Besuch zeigt dann noch den Vorzustand (Irons lebend = Original msg 2) | fenster_1120.c Elza-Zweig + 1151-Installer an (3,0x5F) |
| V6 (empfohlen, Ergaenzung) | Dieselbe Bedingung (3,0x5F) fuer die uebrigen Montage-Folgen bei Elza: 1041 Rolltor offen ((4,0x05)=1 vorbelegt; Leser ROOM1041 sub00@0x01568 `21 04 05 01`, Schalter-Poll sub01@0x015A4) + 5 Zombies im Korridor; 1031 Tor-Zustand "unterkrochen" ((4,0x0F)-Zweig: ROOM1031 sub00@0x020E0 `21 04 0f 01` -> Cut_replace 6->12 @0x020ED) + Zombies vor dem Tor; 1131 Rest-Zombies (5 Records von 1140 sub00@0x00BAA..@0x00C62 an den Montage-Plaetzen). Erzaehlung: Elza sagt in E13 selbst "The building's electric power is starting to fail..." (msg 5) — danach ist das Revier ueberrannt | neue Datei engine/src/montage_elza_r36.c nach dem Muster von L_cut1150 §8.4 (Auffuellung) |

### 4.4 Hinterhof 1090/1091

Befund H4: Elzas 1091 ist Leons Nach-Rettungs-Zustand; die Tuer 1051 Slot 5 @0x00B7A ist ab Spielstart frei;
1091 hat 0 Texte und ist Nebenweg (Slot 1 @0x0213A -> ROOM1101 Cut 5, auch ueber 10D1 erreichbar).

| Vorschlag | Eingriff | Port |
|---|---|---|
| G1 (empfohlen) | Hinnehmen + Spur: Text-Platz am Schuppen (Leons Slot 3 Rechteck @0x02352) "The shed is burnt out. An empty fire extinguisher lies in front of the door." | ROOM1091 neue msg 0 + Text-Platz |
| G2 (streng) | Tuer 1051 -> 1091 bis (4,0xE9) (E17 Dach-Abstieg, ROOM1171 sub02@0x013A0) als Text "It's locked." — ab E17 ist Leons L10 im globalen Modell sicher vorbei (L10 < P03 < ... < P08 < E14 < E17) | Tuer-Sperre nach Muster tuer1120_1130.c; Entsperren ohne Ereignis (Flag) |

### 4.5 Werkstatt-Tuer ROOM11B0 und der Strom (Wahrheitstabelle aus den SCDs)

| Platz in der Garage | Leon ROOM11B0 | Elza ROOM11B1 |
|---|---|---|
| Slot 0 (Tuer B1-Korridor) | ohne Strom -> 1180, mit Strom -> 1230 (main00@0x00F64/@0x00F88) | gleich (@0x00F56/@0x00F7A) |
| Slot 1 (Tuer **Parkplatz** 11C0/11C1, Rechteck (-26730,-29940), Breite 2500) | ohne Strom: Text msg 15 "It's too dark to see anything... I must find way to activate the energy." (sub01@0x011F6 -> @0x01216); mit Strom: Tuer (main00@0x00FAA) [Port Q: zusaetzlich Schluessel (9,82)] | ohne Strom: msg 14 (sub01@0x011C0); mit Strom: Tuer (@0x00F9C) |
| Slot 4 (Pliers) | ohne Strom: msg 15 (@0x01220); mit Strom: Item | ohne Strom: msg 14 (@0x011CA); mit Strom: Item |
| Slot 5 (**Schacht** unter dem Container, Rechteck (-895,-21121)) | inert bis (4,0xCD) (main00@0x01010 sce 0 / @0x01034 sce 2) -> ROOM1260 Cut 7 | inert bis (3,0x83) (sub00@0x0113E Aot_reset sce 0), dann Tuer -> ROOM1261 Cut 7 (main00@0x00FFA) |
| Slot 7 (Rolltor-Text "The shutter is tightly sealed in place.", (-22400,-24300)) | ohne Strom: msg 15 (@0x0122A); mit Strom: msg 14 | ohne Strom: msg 14 (@0x011D4); mit Strom: msg 13 |
| Slot 8 (**"Werkstatt"-Tuer**, (-22400,-17200), Breite 1200) | **KEIN AOT** (nie etwas) | solange (3,0x83)==0: Text msg 12 "The door is locked." (sub00@0x0112A); ohne Strom: msg 14 (sub01@0x011DE); ab (3,0x83)==1: kein AOT |

Lesart: Die "Werkstatt"-Tuer ist in keiner Variante eine Tuer (kein Door_aot_set, kein Zielraum; auch
`tuer_zuordnung.inc`/`re15_tuer_eigen.inc` kennen nur Slots 0/1/5). "Ohne Strom zu, mit Strom offen" ist der
Original-Mechanismus der PARKPLATZ-Tuer (und der Pliers): sub01 ersetzt sie bei (4,0xF3)==0 durch den
"too dark"-Text, weil die Garage ohne Strom dunkel ist (Cuts 13/14 statt 0/1, sub01@0x011FA Switch var0A).

| Vorschlag | Eingriff | Port |
|---|---|---|
| W1 (empfohlen) | Leon 11B0: an Elzas Slot-8-Rechteck von Anfang an den Text msg 12 "The door is locked." (Bytes aus ROOM11B1) — vor UND nach Marvins Szene; Marvins Abschliessen (Spur J) wird damit "sichergehen/verriegeln" ("I just want to make sure..."), kein Widerspruch zu Elzas Zustand (H5: Elza ist VOR Leon in der Garage und sieht die Tuer schon zu) | Spur J: Text-Platz vor der Szene installieren (jetzt erst danach geplant) |
| W2 (empfohlen) | Elza 11B1 Slot 1 (Parkplatz): immer "The door is locked." (msg 12) statt der Tuer — Folge von Spur Q (kein Parking-Lot-Schluessel fuer Elza) und P09; ROOM11C1 (5 Hunde) entfaellt fuer Elza (keine Items, keine weitere Tuer: zensus.md ROOM11C Tueren 1/1) | Spur Q (Elza-Zweig): Aot_reset Slot 1 -> sce 1 msg 12 nach dem Strom-Poll |
| W3 (Nutzer-Wahl) | Wenn die Hunde bleiben sollen: Elza findet den Parking-Lot-Schluessel NICHT; stattdessen Marvins Schloss erst nach E21 wirksam = Elza kann in Phase 2 (vor L19) auf den Parkplatz — dann muss Spur Q die Schluessel-Sperre nur fuer Leon setzen (Tuer bei Elza mit Strom offen wie Original) und P09 fuer Elza ab (3,0x83) sperren. Konsistent, aber zwei Zustaende statt einem | — |

### 4.6 Doppelt geloeste Mechanik (geduldet, RE2-Zapping-Praezedenz)

Beide Szenarien loesen: Generator 11F0/11F1 (sub18@0x016F6 identisch), Drahtgitter 11E0/11E1 (sub21@0x01FC6),
Schloss Briefing->Heliport (4,0xC3) (1140 sub02@0x00C90 / 1141 sub05@0x00FD2), Kartenleser 10D0/10D1 (Leon L14,
Elza ueber Spur Q), Rolltor 1050/1051 + Sicherung aus Irons' Hebetisch (Port, beide), Alligator 2090/2091, Birkin
3070/3071, 3080/3081, 50E0/50F1, Endkampf. Kein Eingriff; EIN erzaehlerischer Anker reicht: Elzas Satz "The building's
electric power is starting to fail..." (ROOM1021 msg 5, E13) = der Generator faellt nach ihrem Start wieder aus,
Leon startet ihn danach neu (Vorschlags-Kante `E13 < L17`; ohne Textaenderung).

### 4.7 Text-Widerspruch ROOM30C1 msg 3

"Elza: Leon?! How is he? We got separated back at the RPD." ist der EINZIGE Satz beider Szenarien, der eine
Begegnung behauptet (kein Elza-Spawn bei Leon, kein Leon-Spawn bei Elza in 120 Raumpaaren, zensus.md Spalte EM).

| Vorschlag | Eingriff | Port |
|---|---|---|
| T1 (empfohlen) | msg 3 ersetzen: "Elza: Leon?! Is he all right? I never ran into him back at the RPD..." (gleiche Laenge/Breite-Klasse, Sprecherfarbe wie msg 1) | ROOM30C1 msg 3 ueber re15_msg_install_text (Elza-Variante) |
| T2 (Alternative) | "Elza: Leon? ...The rookie whose patrol car is still parked out front?" (bindet die 11C0/11C1-Streifenwagen an) | dito |

### 4.8 Briefing Room 1140/1141

Leon: 5 Zombies durch die Fenster (sub00@0x00BAA..@0x00C62, Ausloeser Slot 6 (3,0xD2)); Elza: 2+2 Gorillas durch die
Fenster waehrend Johns Szene (msg 9 "that would explain how they managed to enter through the windows"). Mit U2 liegt
E16 nach P08 (Leons Zombies sind in 1130 weitergezogen): Elzas 1141 ist dann leer bis auf John + Gorillas — konsistent
("durch die Fenster" = die bereits zerbrochenen Fenster). Kein Eingriff. Der Schalter (3,0x6A) "beautiful painting"
und das Schloss (4,0xC3) sind in beiden Varianten bedienbar (Doppelung §4.6).

---

## 5. Spuren-Liste (was jeder fuer den anderen hinterlaesst)

### 5.1 Bestehend im Original

| Richtung | Spur | Raum | Beleg |
|---|---|---|---|
| Elza -> Leon | Roys Leiche an Elzas Szenen-Position, "died recently" | 1020 | H1 |
| Elza -> Leon | "Prisons"-Tuer bereits aufgeschlossen (Leon braucht keine Yellow Keycard; Item 0x39 nur in 1011) | 11E0 | H3 |
| Elza -> Leon | Zelle leer, 10 Gefangenen-Zombies frei | 1210 | H2 |
| Elza -> Leon | Sherry versteckt sich im Auto und rennt vor Leon in den Schacht; Leon sucht sie im Kanal | 11B0, 2000 | H5, H8 |
| Elza -> Leon | Die SIG P228 fehlt an der Leiche vor dem Rolltor (Leon hat dort keinen Untersuch-AOT) | 1050 | ROOM1051 main00@0x00C22 + Slot 11; ROOM1050 ohne (VERMUTUNG als Spur) |
| Elza -> Leon | John + Sherry im Zug: "She told me to stay and keep an eye on Marvin here." | 6030 | sub06@0x0101C, msg 3/5 |
| Leon -> Elza | Hinterhof ohne Feuer, Schuppen-Tuer weg, 4 Zombies | 1091 | H4 |
| Leon -> Elza | Kein Hubschrauber auf dem Dach | 1171 | H11 |
| Leon -> Elza | Container in der Garage beiseite, Schacht offen (gleiches Flag (3,0x83)) | 11B1 | H7 |
| Leon -> Elza | Marvin allein in der Fabrik-Lobby: "I'm here with Leon Kennedy... helping me and Ada escape" | 30C1 | H9 |
| Leon -> Elza | Adas Leiche "one of those gorillas killed her" | 50B1 | H10 |
| Leon -> Elza | Marvin liegt im Zug; "He's fighting off monsters. He wants you to start the train." | 6031 | H12 |
| beide | Tote Polizisten mit Zetteln 4312/5632, RPD-Truppe in 3000/3001, Port-Dokumente | 1110, 1230, 3000/3001, 1000/1010/1020/1050 | zensus.md; dokumente_r34.c |

### 5.2 Port-Ergaenzungen (beschlossen) und ihre Spur

| Richtung | Spur | Raum | Stand |
|---|---|---|---|
| Leon -> Elza | Irons tot auf der Couch, "He has already passed..." | 1151 | Spur L (U1) |
| Leon -> Elza | Fenster zerbrochen + 1 Kraehe | 1121 | Spur L (U2) |
| Leon -> Elza | Werkstatt-Tuer "The door is locked." (Original-Text bleibt; Marvin verriegelt sie bei Leon) | 11B1 | Spur J (W1) |
| Leon -> Elza | Parkplatz-Tuer verschlossen (Schluessel-Sperre / Marvins Schloss) | 11B1 | Spur Q/J -> W2 noetig |
| Elza -> Leon | Prison Key fehlt am Schluesselbrett (Elza war vor Leon im Funkraum) | 10F0 | K3 (Vorschlag) |

### 5.3 Vorgeschlagene Ergaenzungen (nur additiv; Reihenfolge = Empfehlung)

| # | Richtung | Spur | Raum | Eingriff |
|---|---|---|---|---|
| K1 | — | Johns Satz "Roy locked me in here to sleep it off..." | 1211 | 1 Nachricht |
| K2 | Elza -> Leon | "The cell door is open. Whoever was locked in here is gone." | 1210 | 1 Text-Platz + 1 Nachricht |
| K3 | Elza -> Leon | "The hook next to it is empty. The tag reads 'Prison'." | 10F0 | 1 Examine-Text (Spur Q) |
| R1 | Elza -> Leon | Roys Leiche erst ab (4,0xF3) | 1020 | Gate auf obj 6 + Slot 5 |
| R2 | Elza -> Leon | Blutlache an Roys Platz | 1010 | 1 Text-Platz + 1 Nachricht |
| G1 | Leon -> Elza | "The shed is burnt out. An empty fire extinguisher lies in front of the door." | 1091 | 1 Text-Platz + 1 Nachricht |
| V6 | Leon -> Elza | Rolltor 1041 offen + 5 Zombies, Tor 1031 unterkrochen + Zombies, Rest-Zombies 1131 — ab (3,0x5F) | 1041/1031/1131 | neue Datei montage_elza_r36.c |
| W1/W2 | Leon -> Elza | Werkstatt-Tuer bei Leon von Anfang an "locked"; Parkplatz-Tuer bei Elza immer "locked" | 11B0/11B1 | 2 Text-Plaetze |
| T1 | — | 30C1 msg 3 ohne Begegnungs-Behauptung | 30C1 | 1 Nachricht ersetzen |

---

## 6. Offene Punkte / VERMUTUNGEN

1. E04 "gunshot" (1041): Schuetze unbenannt (Roy oder Leon) — kein Eingriff noetig.
2. E17 "this thing" (1171): obj 0/1 @0x01224/@0x01246 (4990,-8300/-10150,-7350) werden nach (4,0xE9) nicht mehr
   gesetzt = das weggeschobene Hindernis; Bedeutung (Leiter/Kiste) nicht aus Text belegbar.
3. ROOM3091 NPC-Slot 1 Typ 0x42 (Ada-Modell) als Sherry-Platzhalter (B_figuren §0) — unveraendert VERMUTUNG.
4. Die Schicht-Tabelle in `zeitleiste.md` §3 (Modell c) zeigt FRUEHESTE Positionen (laengster Weg von den Quellen);
   gleiche Schicht bedeutet NICHT Gleichzeitigkeit. Verbindlich sind nur die Kanten (§3.1/3.2).
5. Was der Nutzer bei "Werkstatt-Tuer ohne Strom zu, mit Strom offen" im gebauten Stand gesehen hat, ist nicht
   gemessen (Auftrag: nur Analyse); die Wahrheitstabelle §4.5 gilt fuer die RDTs. Messweg: RE15_DEBUG_JUMP=11b0 /
   11b1 mit und ohne (4,0xF3), AOT-Zensus am Rechteck (-22400,-17200).
