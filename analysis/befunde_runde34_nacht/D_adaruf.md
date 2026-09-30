# Spur D — Ada-Ruf an der Tuer ROOM1050 -> ROOM10A0 (Runde 34 Nacht)

Stufe: ERMITTLUNG + BAUPLAN (kein Port-Code). Arbeitsbaum `.claude/worktrees/r34n_adaruf`, Zweig `r34n/adaruf`.
Werkzeuge dieser Spur: `re15_port/tools/r34n_d/` (rbj_zensus.py, plc_motion_zensus.py, gesten_streifen.py,
gesten_arme.py, tuer_graph.py, messlauf.sh). Belege: `analysis/befunde_runde34_nacht/D_belege/`.

Stand: Abschnitte 1-3 gefuellt (Zwischenstand), 4-8 folgen.

## 0 Kurzfassung

(folgt am Ende)

## 1 Nutzerwortlaut + Lesart

Wortlaut (AUFTRAG.md Z. 14-19):

> Ich möchte, bevor Leon von ROOM 1050 zu ROOM 10A0 wechseln kann, das eine Cutscene stattfindet: Dort soll
> folgender Dialog passieren:
> - Ada: Hello? Anyone? Please, get me out of here!
> - Leon: Another civilian survivor. I have to help her!
> - Dabei soll Leon nach Adas Dialog einen Schritt zurück machen von der Tür, so wie in ROOM 1090 in der
>   Cutscene, nachdem er zum Feuer läuft. Dann bei "another Civilian Survivor" soll er diese Animation machen wo
>   er den rechten Arm um 180° grad dreht und nach rechts bewegt, die es auch schon in anderen Cutscenes gibt.
>   Dann macht er den Arm den gleichen Weg wieder zurück. Und bei "I have to help her" macht er seine Arm
>   Schwung Animation - ebenfalls aus anderen Cutscenes benutzt.
> - Drückt man nach dieser Cutscene erneut die Tür soll der Text kommen "I have to help the Survivor first!".
> - Erst wenn man Ada gerettet hat in ROOM 1090, dann kann man durch die Tür laufen.

Lesart, Punkt fuer Punkt:

| # | Mehrdeutigkeit | Festlegung | Grund |
|---|---|---|---|
| L1 | "bevor Leon ... wechseln kann" — wann feuert die Szene? | Beim ERSTEN Druck (Quadrat) auf die Tuer Slot 4, solange Ada nicht gerettet ist. | Tueren oeffnen im Original nur per Quadrat-Flanke (FUN_80042bac(…,0x10), memory reai-v2-door-transition); "Drückt man nach dieser Cutscene erneut die Tür" setzt einen ersten Druck voraus. |
| L2 | Sprecherkennung "Ada:" | Bildschirmtext **"Woman: Hello? Anyone? Please, get me out of here!"** | RE1.5 nennt Ada vor und waehrend ihrer Rettung durchgehend "Woman:" (ROOM1090 msg 0 @0x275C, msg 2 @0x27BD, msg 4 @0x281F); "Ada:" steht erstmals NACH der Rettung (ROOM1050 msg 6 @0x1007, ROOM11C0 msg 1..8). Leons eigene Zeile "Another civilian survivor ... help her" zeigt, dass er sie nicht kennt. "Ada:" im Auftrag lese ich als Sprecherangabe des Drehbuchs (wie "Leon:"), nicht als Bildschirmtext. Umstellung = eine Glyphenfolge (die fuenf Bytes "Ada: " liegen in ROOM1050 msg 6 @0x1007). |
| L3 | "einen Schritt zurück ... so wie in ROOM 1090 ..., nachdem er zum Feuer läuft" | Der RUECKWAERTSSCHRITT = `Plc_dest` Modus **8** (ROOM1090 sub02 @0x0247C `40 00 08 20 7e ff 3c f8`), NACH den drei Lauf-`Plc_dest` Modus 5 zum Feuer (@0x0244A/0x02454/0x0245E). | Einziger Rueckwaerts-Modus in sub02; Handler @0x800311f0 laeuft rueckwaerts (§3.3). Das ist genau "nachdem er zum Feuer läuft". |
| L4 | "rechten Arm um 180° dreht und nach rechts bewegt" | (folgt, §3.5) | Alle Leon-Gesten 15..23 bewegen den LINKEN Arm (Knochen 12-14); der rechte (9-11) traegt die Waffe. "rechts" ist Bildschirm-/Zuschauersicht. |
| L5 | "Arm Schwung Animation" | (folgt, §3.5) | |
| L6 | "erneut die Tür" -> Text | Jeder weitere Druck vor der Rettung zeigt NUR den Text, keine Szene. | Wortlaut. |
| L7 | "Erst wenn man Ada gerettet hat in ROOM 1090" | Freigabe-Flag **(3,0xBB)** = Bank 3 Bit 187, NICHT (3,0x6E). | (3,0x6E) ist ein Ein-Mal-Handshake: gesetzt von ROOM1090 sub03 @0x24D6, GELOESCHT von ROOM1050 sub03 @0x0D88 (`22 03 6e 00`, erstes Opcode der "Hey, wait!"-Szene) — die Tuer waere nach Adas Weglaufen wieder zu (Softlock). (3,0xBB) setzt dieselbe Rettungsszene @0x24D2 (`22 03 bb 01`), in keinem der 240 RDTs wird es geloescht (§3.2). |
| L8 | Elza (ROOM1051) | Keine Sperre in ROOM1051. | ROOM1091 hat weder Feuer noch Ada noch Nachrichten (sub00/sub01 leer, sub02 nur Aot_reset); (3,0xBB) wird in Elzas Spiel nie gesetzt -> eine Sperre waere dauerhaft. Gleiche Regel wie tuer1120 (nur ROOM1130). |

## 2 Ist-Zustand im Port (gemessen)

Messlaeufe der echten exe (`re15_port/tools/r34n_d/messlauf.sh`, Kopie `re15_pc_r34n_d.exe`, Baum
`r34n/adaruf` = master cf0e68ba):

**2.1 Tuer Slot 4 oeffnet sofort.** Weg: Titel -> `RE15_DEBUG_JUMP=1000@120` -> Standplatz vor ROOM1000
Slot 0 (`RE15_PLAYER_POS=22230,-13400,0,0`, aus `probe_r33_tueren standplatz 1000 1050`) -> Quadrat ->
Tuersequenz -> ROOM1050 (Cut 4, Spawn (14700,-13500)) -> 0,8 s vorwaerts -> Quadrat an der 10A0-Tuer:

    [aot] DOOR FIRE slot=4 rect=(17200,-13700,hw=500,hh=1000) target_cut=0 spawn=(26200,-14400,24800)
    [tuer] Sequenz Archiv 2 DOOR07 Port-Archiv P07G Variante 0 Bit7 0 Tuer 7 Seite S021 T013 Spender FF (9 Skripte)
    [tuer] Sequenz fertig: 301 Bilder + 1 Warten, 1 Se_on, Schliesston 1, Ton geladen, Notizen 0x0
    [room] PC loaded room10a0.rdt (101164 bytes)

Also: keine Szene, kein Text, Tuersequenz P07G (re15_tuer_eigen.inc Zeile ROOM1050 @0x00B5A, Archiv 21 Var 13)
und Raumwechsel — Flag-unabhaengig.

**2.2 Kamera an der Tuer = Cut 4.** RVD (RDT+0x28 = 0x1B0, 20-B-Saetze, rdt_common.c parse_zones): Standplatz
(16580,-13700) liegt in den Eigenbereichen von Cut 3/4/5 (Satz 8/11/14); Umschaltlinien 4->5 bei z -16700..-15700
(Satz 13 @0x2B4) und 5->4 bei z -15700..-14700 (Satz 15 @0x2DC). Von Norden UND von Sueden (ROOM1090) kommend
steht an der Tuer also Cut 4; die Tuer ROOM1000 Slot 0 setzt beim Eintritt ebenfalls Cut 4
(`... 00 05 04 ...`, Door_aot_set @0x00BBE Byte 24). Kamera Cut 4 (RDT @0x60+4*0x20): von (14999,-3549,-8140)
nach (15618,-2173,-12830).

**2.3 ⛔ Befund ausserhalb dieser Spur: bei geschlossenem Rolltor verdeckt das Tor-Prop den ganzen Cut 4.**
Bild `D_belege/ist_cut4_rolltor_zu_offen_cut5.png`: links Cut 4 mit (3,121)=0 (Tor zu) — der Bildschirm ist bis
auf ein Dreieck die Rueckseite des Rolltors, Leon unsichtbar; Mitte derselbe Lauf mit `RE15_SET_FLAG=3:121`
(Tor offen) — Leon an der 10A0-Tuer (linke Tuer im Bild); rechts `RE15_FORCE_CUT=5` — Leon an der Tuer hinter
der Wandkante, nur ein Streifen sichtbar. Geometrie: Prop-Modell 0 der RDT (@0xE004) ist eine Platte 3203 x 5127
(x -1602..1601, y -5127..-1, z +-120); `Obj_model_set` obj 0 (sub00 @0x00C36, nur bei (3,121)==0) stellt sie
bei (15432,0,-10424) auf — 2284 Einheiten VOR der Kamera 4, zwischen Kamera und Tuer. Mit der Projektionsweite
der Raumkameras fuellt sie das Bild. Das ist die RDT-Geometrie selbst, kein Zeichenfehler des Ports (ob die
PSX sie genauso zeichnet, ist nicht gemessen). Folge fuer Spur D: wer die 10A0-Tuer vor dem Toroeffnen ueber
ROOM1000 erreicht (der Normalfall, sobald Spur A das Tor an die Sicherung bindet), sieht die Szene NICHT. §7 R1.

## 3 Original-/RE2-Mechanismus (Adressen, Bytes, Instruktionen)

### 3.1 Die Tuer und ihre Nachbarn (ROOM1050 main00)

`scd_dump_room.py ROOM1050.RDT` (main 0xAD8, sub 0xC10, msg 0xE44):

    0x00B5A Door_aot_set 3b 04 02 31 00 00 3c 41 94 c6 e8 03 d0 07 58 66 c0 c7 e0 60 00 08 00 0a 00 08 ...
            Slot 4, sce 2, flags 0x31, Band 0, Rechteck (16700,-14700,1000,2000)
            -> Raum 0x0A = ROOM10A0, Spawn (26200,-14400,24800), dir 0x0800, Cut 0
    0x00B7A Door_aot_set 3b 05 ... -> ROOM1090 (Slot 5, Rechteck (18100,-23900,3500,1000), Cut 3)
    0x00ADA Slot 0 -> ROOM1030; 0x00AFA/0x00B1A/0x00B3A Slot 1..3 -> ROOM1000
    0x00B9A Item_aot_set Slot 6; 0x00BD2/0x00BE6/0x00BFA Aot_set Slot 8..10 (sce 1, msg 1/3/4)
    sub00 0x00C22 Aot_set Slot 7 sce 3 (Rolltor-Schalter, nur bei (3,121)==0)

Belegte AOT-Slots in ROOM1050: 0..10 (VERTRAG §1.2). Nachrichten im RDT: 0..8 (Tabelle @0xE44, off[0] =
`12 00` -> 9 Eintraege). Fuer Spur D: Slots 13/14, Nachrichten 22..25.

### 3.2 Die Ada-Kette — wer setzt/liest welches Flag (Zensus aller 240 RDTs, Bank 3)

    ROOM1000 sub01 0x0D00 Ck(9,134)==1 [Feuerloescher-Item 0x31 genommen, Item_aot_set Slot 3 @0x0C24]
             sub01 0x0D08 Ck(3,133)==0 -> 0x0D0C SET (3,133)=1            "Loescher im Besitz"
    ROOM1090 sub01 0x23F8 Ck(3,128)==0 -> Evt_exec sub02, 0x240A SET (3,128)=1   Feuer-Szene (einmal)
             sub00 0x230A Ck(3,133)==0 -> Slot 2 Text msg 7 "I must hurry up and get something to put out this fire"
                   sonst Ck(3,129)==0 -> Slot 2 sce 3 -> sub06 (Ja/Nein "Will you use the Fire Extinguisher?")
             sub06 0x271E SET (3,129)=1, 0x2722 SET (3,132)=1, Aot_on 3 (Selbst-Tuer)
             sub00 0x22A6 Ck(3,132)==1 -> Sce_em_set Ada (Typ 0x42) + Evt_exec sub03 (Rettung)
             sub03 0x24CE SET (3,132)=0
             sub03 0x24D2 SET (3,187)=1      <- persistent, nie geloescht
             sub03 0x24D6 SET (3,110)=1      <- Handshake an ROOM1050
    ROOM1050 sub00 0x0C8A Ck(3,110)==1 -> Sce_em_set Ada + Evt_exec sub03
             sub03 0x0D88 SET (3,110)=0      <- loescht den Handshake ("Hey, wait!"-Szene, einmal)

Ergebnis: das dauerhafte Merkmal "Ada gerettet" ist **(3,0xBB)**. (3,0x6E) ist nach der ersten Rueckkehr in
ROOM1050 wieder 0. Bank 3 ist spielweit (tuer1120.h: Zonen-Tabelle @0x80074664[3] -> 0x800b0ff8; der Raum-Init
FUN_8003ecec loescht nur Bank 5 Wort 0, @0x8003ed74).

Wohin laeuft Ada in ROOM1050 sub03/sub04? `Plc_dest` Modus 5 nach (16250,-16650) und (16250,-14200) — das ist
das Rechteck der 10A0-Tuer (Slot 4) —, danach `Member_set` x=850/z=-14200 (@0x0E2C/0x0E30) = geparkt. Ada
verschwindet also durch GENAU diese Tuer. Die Nutzer-Sperre bis zur Rettung passt dazu.

### 3.3 Der Rueckwaertsschritt (ROOM1090 sub02)

    0x02446 Work_set(1,0)                             Spieler
    0x0244A Plc_dest 40 00 05 20 3b f9 ad 06          Modus 5 (Rennen) -> (-1733,1709)   } zum Feuer
    0x02454 Plc_dest 40 00 05 20 5c ff da 00          Modus 5 -> (-164,218)              }
    0x0245E Plc_dest 40 00 05 20 92 02 2a f7          Modus 5 -> (658,-2262)             }
    0x02468 Cut_chg 15 / 0x0246A Cut_auto 1
    0x0246C Member_set 0 = 613 / 0x02470 Member_set 2 = -2123 / 0x02474 Member_set 4 = 145   (Stellen)
    0x02478 Sleep 10
    0x0247C Plc_dest 40 00 08 20 7e ff 3c f8          Modus 8 -> (-130,-1988), Ankunftsbit 0x20
    0x02484 Gosub sub04  (Do Evt_next Edwhile Ck(5,32)==0)   wartet auf die Ankunft
    0x02486 Sleep 20
    0x0248A Message_on 1 "Leon: Hey! Who's that?!"  + Gesten 15 vor/zurueck, 20 vor/zurueck (§3.4)

Weg des Schritts: (613,-2123) -> (-130,-1988) = 755,2 Einheiten.

Spieler-Plc_dest-Tabelle @0x80073e30 (`re15_disasm.py table`): [4]=0x80030af0 GEHEN, [5]=0x80030d28 RENNEN,
[6]=0x800517f0, [7]=0x80031080, **[8]=0x800311f0**, [9]=0x80031360 DREHEN. Handler Modus 8 (`dis 0x800311f0`):

    800311fc addiu s0,s0,-13734      ; s0 = 0x800aca5a = Spieler+0x6 (Phase), Spieler-Basis 0x800aca54
    80031204 lbu v0,0(s0) / 8003120c bne v0,zero,…      ; Phase 0 = Init
    80031210 ori v0,zero,0x46 / 80031218 sh v0,-13600(at)  ; +0x8c Tempo = 70
    8003122c sb zero,-13592(at)       ; +0x94 Clip = 0      80031234 sb zero,-13591(at) ; +0x95 Bild = 0
    80031224 ori v0,zero,0x7 / 8003123c sb v0,-13597(at)   ; +0x8f Ueberblendung = 7
    80031244 lh a0,-13296(a0) / 8003124c lh a1,-13294(a1)  ; Ziel +0x1bc/+0x1be
    80031250 jal 0x8001aac4 / 80031254 addiu a2,zero,-48   ; drehen, Rate -48 = RUECKEN zum Ziel
    80031258 jal 0x800245d8 / 8003125c ori a0,zero,0x800   ; Vortrieb in Richtung Gierung+0x800 = rueckwaerts
    800312a4 lw a0,-13608(a0) ; 0x800acad8 = PL00.EMR  800312ac lw a1,-13376(a1) ; 0x800acbc0 = PL00.EDD
    800312b0 jal 0x8001f314 (anim_set)                     ; COMMON-Bank Clip 0
    800312f4 jal SquareRoot0 / 800312fc slti v0,v0,100     ; Ankunft < 100
    80031318 sb v0(=6),-13735(at)  ; +0x5 = 6      8003131c jal 0x8004ef90 ; Ankunftsbit (+0x1c3) setzen

`FUN_8001aac4` mit negativer Rate (`bgez s0` @0x8001ab08, sonst `addiu v0,a0,2048` @0x8001ab14): Sollrichtung
= atan2 + 0x800, d.h. Leon dreht sich mit 48/Bild so, dass sein RUECKEN zum Ziel zeigt, und wird nach hinten
geschoben. Clip = PL00.EDD Clip 0 (34 Bilder, In-Place-Rueckwaertsgang, Port: re15_to_re2.c
RE15_PLAYER_MOTION_BACK_PL00, bereits portiert). Im Port: `op_plc_dest` -> Walker `walk_mode` 8.

### 3.4 Leons Gestenbibliothek im Raum-RBJ

Leons Szenen-`Plc_motion` spielen aus dem Raum-RBJ (RDT+0x5C; Record mit Marker-Bit 0 = Spieler,
Binder FUN_8001b3f8, Port `re15_apply_room_cinematic` -> `re15_emd_parse_rbj`). Zensus aller RDTs
(`rbj_zensus.py liste`): ROOM1050 hat RBJ @0x108C (53112 B), Record 0 Marker 0x00000001, 26 Clips, davon 15..25
Gesten (Bildzahlen 20 30 30 20 30 25 35 50 24 12 29). INHALTSVERGLEICH (Folge der Keyframe-Bytes je Bild,
`rbj_zensus.py suche`): ROOM1050 rec0 Clip 15..23 sind **bytegleich** mit ROOM1090 rec0 15..23 und ROOM1170
rec0/rec1 15..23 (Hashes fe313d415aee, fec1ec970163, ce75d881cd9d, 37bbc50056bc, fde929aa6e6b, 28617fb5751d,
12f8c236fb59, 082e218fc8dd, b385706e1ff0). Raeume mit 10-Clip-RBJ (ROOM10D0, 3000, 3020, 1150 …) tragen dieselbe
Bibliothek ab Clip 0 (10D0 Clip 0 == 1050 Clip 15). **Jede Leon-Geste aus einer anderen Szene liegt damit in
ROOM1050 selbst vor — kein fremdes RBJ noetig.** (Clip 24 ist in ROOM1050 ein anderer, 12 Bilder.)

Zensus aller Spieler-`Plc_motion` (661 Aufrufe, `D_belege/plc_motion_zensus.tsv`): Leon benutzt aus der
Bibliothek 15 (83x, 40x rueckwaerts), 16 (23x), 17 (10x, NIE rueckwaerts), 18 (17x), 19 (26x), 20 (21x),
21 (1x), 23 (45x, fast immer als Abschluss einer Zeile), 22 nie. Muster "vor + zurueck":
`Plc_motion(0,N,0)`, `Sleep`, `Plc_motion(0,N,0)` + `Plc_flg(0,0x80,0)`, `Sleep`.

### 3.5 Welche Geste ist welche (folgt)

## 4 Soll-Verhalten (Zeitlinie)

(folgt)

## 5 Bauplan

(folgt)

### 5.x Konstanten-Tabelle

| Konstante | Wert | Beleg @0x… / Datei-Offset bzw. NUTZER-VORGABE / PORT-WAHL + Grund |
|---|---|---|

## 6 Abnahmeplan

(folgt)

## 7 Risiken, Softlocks, Wechselwirkungen

(folgt)

## 8 Offene Punkte

(folgt)
