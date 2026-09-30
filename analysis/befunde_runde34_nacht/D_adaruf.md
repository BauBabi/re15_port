# Spur D — Ada-Ruf an der Tuer ROOM1050 -> ROOM10A0 (Runde 34 Nacht)

Stufe: ERMITTLUNG + BAUPLAN (kein Port-Code). Arbeitsbaum `.claude/worktrees/r34n_adaruf`, Zweig `r34n/adaruf`.
Werkzeuge: `re15_port/tools/r34n_d/` (rbj_zensus.py, plc_motion_zensus.py, gesten_streifen.py, gesten_arme.py,
tuer_graph.py, texte_bauen.py, messlauf.sh). Sonde: `re15_port/tests/unit/probe_r34n_d_adaruf.c`
(+ `probes/r34n_d_adaruf.cmake`, kein add_test). Belege: `analysis/befunde_runde34_nacht/D_belege/`.

## 0 Kurzfassung

* **Ausloeser:** erster Quadrat-Druck an der Tuer ROOM1050 Slot 4 (Door_aot_set @0x00B5A -> ROOM10A0), solange
  Ada nicht gerettet ist. Die Tuer wird nach dem Init-Lauf von main00 umgewidmet (Muster `tuer1120_1130.c`):
  Slot 4 -> sce 3 / Ereignis 13 (Szene) bzw. nach der Szene -> sce 1 / msg 25 (Text). Kein neuer AOT-Slot.
* **Freigabe-Flag = (3,0xBB), NICHT (3,0x6E).** (3,0x6E) setzt ROOM1090 sub03 @0x24D6, aber ROOM1050 sub03 loescht
  es @0x0D88 (`22 03 6e 00`) beim ersten Wiederbetreten — die Tuer waere danach wieder zu (Softlock). (3,0xBB)
  setzt dieselbe Rettungsszene @0x24D2 (`22 03 bb 01`); in keinem der 240 RDTs wird es geloescht.
* **Szene = portseitiger SCD-Bytecode (142 B) aus Original-Opcodes**, ausgefuehrt von der vorhandenen VM, Form
  und Zeiten aus ROOM1090 sub02/sub03, ROOM1050 sub03 und ROOM1170 sub02. An der echten VM + Spielschritt
  gefahren (Sonde): 305 Bilder (10,2 s) vom Standplatz, Letterbox (1,27) und Pad-Sperre (2,7) wie jede
  Original-Szene, aktive Kamera Cut 4, Ruf "Woman: ...", Drehung zur Tuerwand (`Plc_dest` Modus 9) und
  Rueckschritt = `Plc_dest` Modus 8 (Handler @0x800311f0) exakt wie ROOM1090 nach dem Lauf zum Feuer
  (700 Einheiten in 10 Bildern, von allen sieben geprueften Druckstellen), "Leon: Another civilian survivor."
  mit **Clip 19 vor + zurueck**,
  "Leon: I have to help her!" mit **Clip 17 (Arm-Schwung)**, danach Text-Platz msg 25
  "I have to help the Survivor first!". Beide Clips liegen bytegleich im ROOM1050-RBJ (Record 0).
* **Kein Softlock:** Feuerloescher (ROOM1000-Umkleide) und Ada (ROOM1090-Hof) haengen am Suedteil von ROOM1050
  und sind ohne ROOM10A0 erreichbar; ROOM10A0 und alles dahinter ist NUR ueber Slot 4 erreichbar. Elza
  (ROOM1051/1091) hat keine Rettung -> keine Sperre fuer Elza.
* **Zwei Haken-Zeilen in gemeinsamen Dateien:** `scd_room_setup.c` (Installation, neben tuer1120) und
  `scd_vm.c scd_event_fire` (Ereignis 13 -> Port-Programm, eine Zeile neben Spur As HAKEN 1).
* **Sprachdateien:** `synchro/STAGE1/room1050/main22.wav` (Frau, <= 3,3 s), `main23.wav`, `main24.wav` (Leon).
  msg 25 ist ein Untersuchungstext (Text-Platz) — der spielt im Port keine Stimme (wie tuer1120).
* **Lesarten (§1):** Bildschirmname "Woman:" statt "Ada:" (RE1.5-Konvention vor der Rettung); "rechter Arm ...
  nach rechts" = Zuschauersicht (alle Leon-Gesten sind LINKSARMIG, die rechte Hand haelt die Waffe).

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

| # | Mehrdeutigkeit | Festlegung | Grund |
|---|---|---|---|
| L1 | Wann feuert die Szene? | Beim ERSTEN Quadrat-Druck an Slot 4, solange (3,0xBB)=0 und (9,65)=0. | Tueren oeffnen nur per Quadrat-Flanke (memory reai-v2-door-transition, FUN_80042bac kind 0x10); "Drückt man nach dieser Cutscene erneut die Tür" setzt einen ersten Druck voraus. |
| L2 | "Ada:" | Bildschirmtext **"Woman:"** | RE1.5 nennt Ada vor und waehrend der Rettung "Woman:" (ROOM1090 msg 0 @0x275C, msg 2 @0x27BD, msg 4 @0x281F); "Ada:" erstmals NACH der Rettung (ROOM1050 msg 6 @0x1007, ROOM11C0 msg 1..8). Leons eigene Zeile ("Another civilian survivor ... help her") zeigt, dass er sie nicht kennt. "Ada:" im Auftrag = Drehbuch-Sprecherangabe (wie "Leon:"). Umstellung = die 6 Namensbytes `33 4b 49 3d 4a 16` gegen die 4 von "Ada:" `1d 40 3d 16` tauschen (ROOM1050 msg 6 @0x100B, gleiche Farbe 02). |
| L3 | "einen Schritt zurück ... wie in ROOM 1090 ..., nachdem er zum Feuer läuft" | `Plc_dest` Modus **8** (ROOM1090 sub02 @0x0247C), gleiche Weglaenge. | Der einzige Rueckwaertsbefehl in sub02, direkt nach den drei Lauf-`Plc_dest` Modus 5 zum Feuer (@0x0244A/0x02454/0x0245E). Der Nutzer hat genau diesen Schritt schon einmal beanstandet ("im Original macht er noch EINEN Schritt zurueck, bei uns ZWEI", actor_locomotion.c) — er ist ihm vertraut. |
| L4 | "den rechten Arm um 180° dreht und nach rechts bewegt ... den gleichen Weg wieder zurück" | **Clip 19**, vorwaerts + rueckwaerts (`Plc_flg 0x80`). | §3.5: alle Leon-Gesten bewegen den LINKEN Arm (Knochen 12-14); der rechte (9-11, Waffenhand) steht. "rechts" ist also Zuschauersicht (Leon frontal: sein linker Arm ist rechts im Bild). Clip 19 hebt den Arm seitlich nach aussen (Hand +300 nach aussen, Unterarm von haengend -79 Grad auf waagerecht +11 Grad) und dreht die Hand dabei um (Handflaeche vom Oberschenkel nach oben, ~100 Grad) — "dreht und bewegt nach rechts". "den gleichen Weg wieder zurück" = das Muster, in dem Clip 19 im Original IMMER laeuft (vor + `Plc_flg 0x80`, 11 von 26 Aufrufen rueckwaerts; z.B. ROOM1090 sub03 @0x0265C/@0x02664). |
| L5 | "seine Arm Schwung Animation" | **Clip 17**, einmal vorwaerts (kehrt selbst in die Ruhe zurueck). | Der einzige Schwung der Bibliothek: Hand von der Brustmitte bis ganz seitlich hinaus (+780 Einheiten), Unterarm schwenkt 107 Grad. Leons "Now what am I gonna do?" im Intro (ROOM1170 sub02 @0x015F0, im Port bei Bild 1686 gemessen), "Ada, you hide inside that patrol car.", "Hurry up! They're coming!". Clip 17 wird im Original NIE rueckwaerts gespielt — passt dazu, dass der Nutzer hier keinen Rueckweg nennt. |
| L6 | "erneut die Tür" | Jeder weitere Druck vor der Rettung zeigt NUR den Text msg 25; auch nach Raumwechsel (Bit (9,65) gespeichert). | Wortlaut. |
| L7 | "Erst wenn man Ada gerettet hat" | Freigabe (3,0xBB). | §3.2. |
| L8 | Elza ROOM1051 | Keine Sperre. | ROOM1091 hat weder Feuer noch Ada noch Nachrichten (sub00 @0x0222E und sub01 @0x02230 = `01 00`), (3,0xBB) wird in Elzas Spiel nie gesetzt; Sperre waere dauerhaft. Gleiche Regel wie tuer1120 (nur ROOM1130). |
| L9 | Kamera | Aktive Kamera (Cut 4), kein Cut_chg. | §2.2: Cut 4 ist die einzige Kamera, die die Tuer zeigt (Cut 5: Leon hinter der Wandkante). Der Nutzer nennt keinen Schnitt. |

## 2 Ist-Zustand im Port (gemessen)

Messlaeufe der echten exe (`tools/r34n_d/messlauf.sh`, Kopie `re15_pc_r34n_d.exe`, Stand master cf0e68ba).

**2.1 Tuer Slot 4 oeffnet sofort.** Weg: Titel -> `RE15_DEBUG_JUMP=1000@120` -> Standplatz vor ROOM1000 Slot 0
(`RE15_PLAYER_POS=22230,-13400,0,0`, aus `probe_r33_tueren standplatz 1000 1050`) -> Quadrat -> Tuersequenz ->
ROOM1050 (Cut 4, Spawn (14700,-13500)) -> 0,8 s vorwaerts -> Quadrat an der 10A0-Tuer:

    [aot] DOOR FIRE slot=4 rect=(17200,-13700,hw=500,hh=1000) target_cut=0 spawn=(26200,-14400,24800)
    [tuer] Sequenz Archiv 2 DOOR07 Port-Archiv P07G Variante 0 Bit7 0 Tuer 7 Seite S021 T013 Spender FF (9 Skripte)
    [tuer] Sequenz fertig: 301 Bilder + 1 Warten, 1 Se_on, Schliesston 1, Ton geladen, Notizen 0x0
    [room] PC loaded room10a0.rdt (101164 bytes)

Keine Szene, kein Text, Tuersequenz P07G (gen/re15_tuer_eigen.inc Zeile ROOM1050 @0x00B5A) und Raumwechsel —
unabhaengig von jedem Flag.

**2.2 Kamera an der Tuer = Cut 4.** RVD (RDT+0x28 = 0x1B0, 20-B-Saetze, rdt_common.c parse_zones): der
Standplatz (16580,-13700) liegt in den Eigenbereichen von Cut 3/4/5; Umschaltlinien 4->5 bei z -16700..-15700
(Satz 13 @0x2B4), 5->4 bei z -15700..-14700 (Satz 15 @0x2DC). Aus jeder Richtung steht an der Tuer Cut 4; die
ROOM1000-Tuer Slot 0 setzt beim Eintritt ebenfalls Cut 4 (Door_aot_set @0x00BBE Byte 24 = 4). Kamera Cut 4
(RDT @0x60+4*0x20): (14999,-3549,-8140) -> (15618,-2173,-12830). Leon an der Tuer blickt nach +X (Gierung 0),
die Kamera sieht ihn schraeg von hinten links — sein linker Gestenarm ist der Kamera zugewandt
(`D_belege/geste19_17_kamera4.png`, obere Zeile je Clip).

**2.3 Rolltor-Prop in Cut 4 — BERICHTIGT.** Mein erstes Messbild (`D_belege/ist_cut4_rolltor_zu_offen_cut5.png`,
links) zeigt Cut 4 bei (3,121)=0 bis auf ein Dreieck voll Rolltor-Rueckseite (RDT-Prop-Modell 0 @0xE004 =
Platte 3203 x 5127, sub00 @0x00C36 bei (15432,0,-10424), 2284 vor der Kamera). **Im Spiel tritt das nicht auf:**
der Suedteil (Tuer 10A0, Umkleide ROOM1000, Hof ROOM1090) ist NUR durch das Rolltor erreichbar. Belege:
(a) Spur A, A_rolltor.md §7: der Laufsteg ROOM1090 (Band 5, aus 10F0) hat keinen Abstieg in den Hof (Leon laeuft
bei (-9900,3358) fest); (b) selbst nachgeprueft: ROOM1000-RVD @0x190 zerfaellt in DREI getrennte Kameragruppen
{0,1,2} (x 14500..25000, Umkleide mit Feuerloescher Slot 3 @(20500,-800) und Tuer Slot 0 -> 1050-Sued),
{3,4,5} und {6,7,8} (x -6900..3600, die zwei Tueren zum 1050-Nordteil) — keine Umschaltkante zwischen den
Gruppen. Mein Messbild war ein Debug-Sprung-Zustand. An der Tuer 10A0 gilt im Spiel also immer (3,121)=1, kein
Tor-Prop, Cut 4 frei (Mitte des Bildes: Leon an der linken Tuer).

## 3 Original-/RE2-Mechanismus (Adressen, Bytes, Instruktionen)

### 3.1 Die Tuer (ROOM1050 main00) und die Belegung

    0x00B5A Door_aot_set 3b 04 02 31 00 00 3c 41 94 c6 e8 03 d0 07 58 66 c0 c7 e0 60 00 08 00 0a 00 08 ...
            Slot 4, sce 2, flags 0x31, Band 0, Rechteck (16700,-14700,1000,2000)
            -> Raum 0x0A = ROOM10A0, Spawn (26200,-14400,24800), dir 0x0800, Cut 0
    0x00B7A Slot 5 -> ROOM1090; 0x00ADA Slot 0 -> ROOM1030; 0x00AFA/0x00B1A/0x00B3A Slot 1..3 -> ROOM1000

AOT-Slots 0..10 belegt (VERTRAG §1.2). Nachrichten im RDT 0..8 (Tabelle @0xE44, off[0] = `12 00` = 9). sub-Tabelle
@0xC10: 5 Eintraege (sub00..sub04). Standplatz vor Slot 4: (16580,-13700) Gierung 0 (probe_r33_tueren).

### 3.2 Die Ada-Kette — Zensus aller 240 RDTs, Bank 3

    ROOM1000 sub01 0x0D00 Ck(9,134)==1 [Feuerloescher Item 0x31, Item_aot_set Slot 3 @0x0C24]
             sub01 0x0D08 Ck(3,133)==0 -> 0x0D0C SET (3,133)=1          "Loescher im Besitz"
    ROOM1090 sub01 0x23F8 Ck(3,128)==0 -> Evt_exec sub02, 0x240A SET (3,128)=1  Feuer-Szene
             sub00 0x230A Ck(3,133)==0 -> Slot 2 Text msg 7; sonst Ck(3,129)==0 -> Slot 2 sce 3 -> sub06
             sub06 0x271E SET (3,129)=1, 0x2722 SET (3,132)=1, Aot_on 3 (Selbst-Tuer)
             sub00 0x22A6 Ck(3,132)==1 -> Sce_em_set Ada + Evt_exec sub03 (Rettung)
             sub03 0x24CE SET (3,132)=0 / 0x24D2 SET (3,187)=1 / 0x24D6 SET (3,110)=1
    ROOM1050 sub00 0x0C8A Ck(3,110)==1 -> Sce_em_set Ada + Evt_exec sub03
             sub03 0x0D88 SET (3,110)=0      ("Hey, wait!", Ada laeuft durch die 10A0-Tuer davon)

(3,0xBB)=(3,187): gesetzt nur @0x24D2, gelesen nur ROOM1090 sub00 @0x237A (Zombies nach der Rettung), nie
geloescht. Bank 3 ist spielweit (tuer1120.h: @0x80074664[3] -> 0x800b0ff8; Raum-Init FUN_8003ecec loescht nur
Bank 5 Wort 0, @0x8003ed74). Adas Weg in ROOM1050 sub04: `Plc_dest` nach (16250,-16650) und (16250,-14200) =
vor das Rechteck von Slot 4, dann `Member_set` x=850 (@0x0E2C) = geparkt: sie verschwindet durch GENAU diese Tuer.

### 3.3 Der Rueckwaertsschritt (ROOM1090 sub02)

    0x02446 Work_set(1,0)                         0x0244A/0x02454/0x0245E Plc_dest Modus 5 (Rennen zum Feuer)
    0x02468 Cut_chg 15 / 0x0246A Cut_auto 1
    0x0246C Member_set 0 = 613 / 0x02470 Member_set 2 = -2123 / 0x02474 Member_set 4 = 145
    0x02478 Sleep 10
    0x0247C Plc_dest 40 00 08 20 7e ff 3c f8      Modus 8 -> (-130,-1988), Ankunftsbit 0x20
    0x02484 Gosub sub04  = Do / Evt_next / Edwhile Ck(5,32)==0 (@0x026E6..0x026F1)
    0x02486 Sleep 20 / 0x0248A Message_on 1 "Leon: Hey! Who's that?!"

Spieler-Plc_dest-Tabelle @0x80073e30 (`re15_disasm.py table`): [4]=0x80030af0 [5]=0x80030d28 [6]=0x800517f0
[7]=0x80031080 **[8]=0x800311f0** [9]=0x80031360. Handler Modus 8 (`dis 0x800311f0`), Spieler-Basis 0x800aca54:

    80031204 lbu v0,0(s0) / 8003120c bne v0,zero,…     ; s0 = +0x6 (Phase), Phase 0 = Init
    80031210 ori v0,zero,0x46 / 80031218 sh v0,-13600(at)  ; +0x8c Tempo = 70
    8003122c sb zero,-13592(at) / 80031234 sb zero,-13591(at)  ; +0x94 Clip 0, +0x95 Bild 0
    80031224 ori v0,zero,0x7 / 8003123c sb v0,-13597(at)   ; +0x8f = 7
    80031250 jal 0x8001aac4 / 80031254 addiu a2,zero,-48   ; Ruecken zum Ziel drehen, 48/Bild
    80031258 jal 0x800245d8 / 8003125c ori a0,zero,0x800   ; Vortrieb Gierung+0x800 = rueckwaerts
    800312a4 lw a0,0x800acad8 / 800312ac lw a1,0x800acbc0 / 800312b0 jal 0x8001f314  ; PL00.EMR/EDD Clip 0
    800312f4 jal SquareRoot0 / 800312fc slti v0,v0,100     ; Ankunft < 100
    80031318 sb 6,+0x5 / 8003131c jal 0x8004ef90           ; Sub 6, Ankunftsbit (+0x1c3)

`FUN_8001aac4` bei negativer Rate: `bgez s0` @0x8001ab08, sonst `addiu v0,a0,2048` @0x8001ab14 (Sollrichtung =
atan2 + 0x800). Weg in 1090: (613,-2123) -> (-130,-1988) = 755,2; die Ankunft < 100 beendet ihn nach 10 Bildern.
**Echte exe, ROOM1090 sub02** (`RE15_DEBUG_SUB=2`, `RE15_SCD_TRACE`): @0x247C bei Bild 301, @0x2486 bei 311,
Endstand (-77,-1997) = 701 Einheiten — `D_belege/r1090_sub02_rueckschritt_echt.png`.

### 3.4 Leons Gestenbibliothek im Raum-RBJ

`Plc_motion(0,N,0)` des Spielers spielt aus dem Raum-RBJ (RDT+0x5C; Record mit Marker-Bit 0 = Spieler,
FUN_8001b3f8; Port `re15_apply_room_cinematic` -> `re15_emd_parse_rbj`). Handler `Plc_motion` @0x80041b90
(Dispatch 0x800744a8[0x3F]): `lhu a1,2(v0)` @0x80041b9c, `sb a1,148(v0)` @0x80041ba8 (+0x94 = Clip = pc[2]),
`srl a1,a1,8` + `sh a1,452(v0)` @0x80041bac/@0x80041bc8 (+0x1c4 = pc[3]), `sb v1(=4),4(v0)` @0x80041bb0,
PC += 4 @0x80041bd4. `Plc_flg` @0x80041fb8 ([0x43]): Unterbefehl 0 -> `or v0,v0,a2` @0x80041ffc auf +0x1c4
(0x80 = rueckwaerts). `Plc_ret` @0x80041f88 ([0x42]): +0x4 = 1, +0x5..+0x7 = 0, PC += 1.

`rbj_zensus.py`: ROOM1050 RBJ @0x108C (53112 B), Record 0 Marker 0x00000001, 26 Clips, 15..25 = 20 30 30 20 30 25
35 50 24 12 29 Bilder. INHALTSVERGLEICH (Keyframe-Bytes je Bild, `rbj_zensus.py suche`): Clip 15..23 von
ROOM1050 rec0 sind bytegleich mit ROOM1090 rec0 und ROOM1170 rec0/rec1 (Clip 17 Hash ce75d881cd9d, Clip 19
fde929aa6e6b). Die Sonde laedt dasselbe RBJ ueber den Port-Leser: 26 Clips, Clip 17 = 30, Clip 19 = 30 Bilder.

Zensus aller Spieler-`Plc_motion` (661, `D_belege/plc_motion_zensus.tsv`): 15 (83x, 40x rueckw.), 16 (23x),
17 (10x, nie rueckw.), 18 (17x), 19 (26x, 11x rueckw.), 20 (21x), 21 (1x), 23 (45x, meist Abschluss), 22 nie.

### 3.5 Welche Geste ist welche (Bildbeleg)

`D_belege/gesten_katalog_vorn.png`: alle neun Bibliotheks-Gesten, Leon frontal (PL00-Mesh/-Textur, Posen aus dem
ROOM1050-RBJ, `gesten_streifen.py`). Messung `gesten_arme.py` (Rumpfsystem, x vorn / y unten / z links):

| Clip | bewegter Arm | Hand (vorn, unten, links) max | Unterarm | Handflaeche | Bild |
|---|---|---|---|---|---|
| 15 | links | (544,-707,120) | haengend -> 26 Grad hoch, nach vorn | nach unten/vorn | "Hey!"-Greifen nach vorn |
| 16 | links | (333,-403,81) | -> waagerecht vorn | nach OBEN | kleine Handflaechen-Geste |
| **17** | links | (163,-677,**668**) | schwenkt 107 Grad von quer vor der Brust nach aussen | nach vorn | **Arm-Schwung** bis ganz seitlich |
| 18 | links | (256,-707,-309) | -> quer vor die Brust | zum Koerper | Hand zur Brust |
| **19** | links | (170,-513,151), Weg nach aussen +300 | haengend -79 Grad -> +11 Grad, seitlich-vorn | Oberschenkel -> OBEN (~100 Grad) | **Arm seitlich hinaus, Hand dreht auf** |
| 20 | links | (362,-515,219) | -> waagerecht vorn | zur Koerpermitte | Unterarm nach vorn |
| 21 | links | (397,-884,-6) | -> hoch (+61 Grad) | seitlich | Hand hoch |
| 23 | links | (2,-177,-112) | leicht angewinkelt | — | Hand an die Huefte |

Der RECHTE Arm (Knochen 9-11, die Waffenhand — Skill re15-weapon-render: "the weapon REPLACES the hand mesh at
bone 11", Schulter rel z = -369) bewegt sich in keiner Geste nennenswert (max 342 in Clip 17). Seitenbeleg:
PL00.EMR rel-Offsets Knochen 9 z=-369 / 12 z=+369; die rechte Flanke zeigt das Holster.

Wo der Nutzer sie gesehen hat (echte exe): Clip 17 im Intro bei Bild 1686 (`D_belege/r1170_sub02_clip17_echt.png`,
Kamera hinter Leon), Clip 15/20 in ROOM1090 sub02 nach dem Rueckschritt (`r1090_sub02_gesten15_20_echt.png`).
So sieht es an der 10A0-Tuer aus: `D_belege/geste19_17_kamera4.png` (Zeile "kamera" = Blickrichtung Cut 4 auf
Leon an der Tuer, Zeile "vorn" = frontal; Clip 19 mit Rueckweg, Clip 17).

### 3.6 Ausloeser: Text-/Ereignis-Platz statt Tuer

AOT-Typtabelle @0x8007469c (`table`): [1]=0x80043084 Text, [2]=0x800430bc Tuer, [3]=0x800430f0 Ereignis. Der
Ereignis-Handler (`dis 0x800430f0`): `lhu a0,0(v0)` @0x800430fc (Nutzlast +0), `lbu a1,3(v0)` @0x80043100 (Sub),
`jal 0x8003ee3c` @0x80043104 — er startet das Sub des Raums. Form des Satzes wie der Rolltor-Schalter
ROOM1050 sub00 @0x00C22 `... ff 00 18 02 00 00` (Nutzlast `ff 00`, `18`, Sub). Umwidmen per Aot_reset-Semantik
(LAB_80040738 = Dispatch [0x46], @0x8004076c-78 fasst nur rec[0]/rec[1] an; Port `re15_aot_retype`, sce 3 ->
GENERIC bei flags&0x10, event = p1>>8). Text-Platz wie ROOM1130 sub01 @0x00A1C `46 03 01 31 01 00 ff ff 00 00`
und ROOM1170 main00 @0x01320 (Maske 0xffff; alle 524 ausgelieferten sce-1-Saetze tragen 0xffff).
Der Port leitet Ereignisse ueber `scd_event_fire` (scd_vm.c) auf `sub_scd[event]`; Spur A haengt dort bereits
eine Weiche fuer Port-Programme ein (A_rolltor.md §5.2 HAKEN 1) — Spur D braucht dieselbe Stelle fuer Ereignis 13.

### 3.7 Szenen-Rahmen (in jeder Original-Szene gleich)

`Set(2,7)=1` (Pad-Sperre; Zone 2 Wort 0 = g_pauseflags, game_state.c) + `Set(1,27)=1` (Letterbox: DAT_800aca3c
&0x10, FUN_80021a0c +0x10/Bild @0x80021a40-54, 15 Bilder) am Anfang, `Set(2,7)=0` / `Set(1,27)=0` / `Work_set(1,0)`
/ `Plc_ret` am Ende — ROOM1090 sub02 @0x02414/@0x02418 bzw. @0x024BE/@0x024C2/@0x024C6/@0x024CA, ebenso sub03,
ROOM1050 sub02/sub03. Stehen bleiben: `Plc_dest 40 00 06 3f 00 00 00 00` (Modus 6 = 0x800517f0, Clip 1 einmal ->
Clip-2-Idle) — ROOM1090 sub02 @0x0242C.

### 3.8 Nachrichtenform und Sprachausgabe

Dialogzeile: `04 00 05 cc <Name> 16 05 00 00 <Text> 04 01 01 63` (ROOM1090 msg 0 @0x275C: Farbe 02 "Woman:",
msg 1 @0x279C: Farbe 01 "Leon:"; `01 63` = 99 Bilder Standzeit, re15_msg_compute_duration). Tuer-/Untersuchungs-
text: `04 02 <Text> 01 00` (ROOM1130 msg 1 @0x0B46; tuer1120_1130.c). Sprachausgabe: `op_message_on` reiht die
Stimme fuer (Raum, msg) ein (scd_vm.c scd_queue_voice), `voice_wait` parkt das NAECHSTE Message_on bis zum Ende
der Aufnahme; Text-Plaetze (sce 1) laufen ueber `re15_scd_show_message` OHNE Stimme ("No voiceover").

## 4 Soll-Verhalten (Zeitlinie)

Gemessen mit der Sonde `probe_r34n_d_adaruf` (echte VM + `re15_actor_step_all_walkers` + `re15_msg_tick` +
`re15_game_step`, Reihenfolge wie platform/pc/main.c :5378/:5419/:5503/:7358), Start Standplatz (16580,-13700)
Gierung 0, Rolltor offen. B = Bilder nach dem Quadrat-Druck (30 Hz). Ausgabe: `D_belege/sonde_r34n_d_adaruf.txt`.

| B | Opcode (Plan-Versatz) | sichtbar |
|---|---|---|
| 0 | Quadrat an Slot 4 -> Aktions-Scan meldet Ereignis 13 -> Faden startet | — |
| 1 | +00 Set(9,65), +04 Set(2,7), +08 Set(1,27), +0C Work_set, +10 Plc_dest 6, +18 Sleep 20 | Balken fahren ein (voll ab B16), Leon bleibt stehen |
| 21 | +1C Message_on 22, +20 Sleep 100 | "Woman: Hello? Anyone? Please, / get me out of here!" (B22..B121, Standzeit 99) |
| 121 | +24 Plc_dest 9 -> (x+755, z), +2C Do/Evt_next/Edwhile Ck(5,32) | Leon dreht sich auf der Stelle zur Tuerwand (+X); vom Standplatz 1 Bild, schraeg bis 12 Bilder |
| 123 | +38 Plc_dest 8 -> (x-755, z), +40 Do/Evt_next/Edwhile Ck(5,32) | Rueckschritt: 70 je Bild, Blick bleibt zur Tuer |
| 133 | Ankunft (Rest 55 < 100) — Weg 700, Endstand (15880,-13700) Gierung 4095 | Leon steht |
| 134 | +4C Sleep 20 | — |
| 154 | +50 Message_on 23, +54 Plc_motion 19, +58 Sleep 25 | "Leon: Another civilian survivor." + Arm seitlich hinaus |
| 179 | +5C Plc_motion 19, +60 Plc_flg 0x80, +64 Sleep 26 | Arm denselben Weg zurueck |
| 205 | +68 Message_on 24, +6C Plc_motion 17, +70 Sleep 100 | "Leon: I have to help her!" + Arm-Schwung |
| 305 | +74/+78 Set(2,7)=0 Set(1,27)=0, +7C Work_set, +80 Plc_ret, +82 Aot_reset Slot 4 -> msg 25, +8C Evt_end | Balken fahren aus (15 Bilder), dann Steuerung frei |
| danach | Quadrat an Slot 4 | "I have to help the Survivor first!" (Text-Platz, Maske 0xffff, bis Tastendruck) |

Druckstellen (Sonde Fall E, Punkt 620 voraus im Rechteck): Standplatz, Nord-/Suedrand mit Blick +X, schraeg von
Nordwest/Suedwest, im Rechteck mit Blick +Z/-Z — ueberall 10 Bilder Rueckschritt (einmal 6: Rest am Ziel), Weg
658..703, Endgierung 4095, Faden-Ende B305..B314, z unveraendert (keine Kamerazone gekreuzt). **Ohne die Drehung**
(erste Planfassung, Rueckschritt direkt aus schraegem Stand) kreiste der Modus-8-Laeufer um das Ziel und die Szene
endete nie (3 von 7 Stellen) — Modus 8 dreht nur 48/Bild bei 70 Vortrieb (Wendekreis ~950 > 755). Das Original
vermeidet das, indem es Leon vorher per `Member_set` auf Ort UND Richtung stellt (ROOM1090 sub02 @0x0246C-74,
hinter einem Cut_chg); hier ohne Schnitt uebernimmt die sichtbare Drehung (Modus 9) diese Aufgabe.

Mit Sprachdateien: `voice_wait` haelt Message_on 23 bis main22.wav zu Ende ist und Message_on 24 bis main23.wav
zu Ende ist; die Gesten haengen jeweils direkt hinter ihrem Message_on und bleiben dadurch am Satz.
Weitere Faelle (Sonde): (9,65)=1 -> Slot 4 = Text msg 25, Druck zeigt msg 25, kein Raumwechsel;
(3,0xBB)=1 (mit oder ohne (9,65)) -> Slot 4 bleibt Tuer, Druck fordert Raumwechsel 0x10A0 an (Tuersequenz P07G).

## 5 Bauplan

### 5.1 Dateien

| Datei | Art | Inhalt |
|---|---|---|
| `re15_port/include/re15_adaruf.h` | NEU | Konstanten (5.4, je mit Beleg), `re15_adaruf_install(uint16_t room)`, `const uint8_t *re15_adaruf_ereignis(uint16_t room, uint8_t event_id)`, Pruefhaken `re15_adaruf_programm(int *len)`, `re15_adaruf_meldung(int id, int *len)` |
| `re15_port/engine/src/adaruf_1050.c` | NEU | Bytecode `k_ruf` (5.3), Nachrichten 22..25 (5.5), `s_prog[142]` (RAM-Kopie fuer Blickpunkt und Rueckschritt-Ziel), Installation, Weiche, Protokollzeile `[adaruf]` (nur PC). PSX-tauglich (keine Heap-/stdio-Abhaengigkeit ausserhalb `#ifdef RE15_PLATFORM_PC`). |
| `re15_port/engine/src/scd_room_setup.c` | HAKEN 1 (1 Zeile + include) | direkt nach `re15_tuer1120_install((uint16_t)g_current_room_id);` (Zeile ~428): `re15_adaruf_install((uint16_t)g_current_room_id);` + `#include "re15_adaruf.h"` |
| `re15_port/engine/src/scd_vm.c` | HAKEN 2 (1 Zeile + include) | in `scd_event_fire` zwischen Spur As Weiche und dem Rueckfall auf `sub_scd[event_id]`: `if (!pc) pc = re15_adaruf_ereignis((uint16_t)g_current_room_id, event_id); /* Spur D */`. Ohne Spur A: `const uint8_t *pc = re15_adaruf_ereignis(...); if (!pc) pc = s_current_rdt->sub_scd[event_id];` (die Schranke `event_id >= RE15_RDT_MAX_SUB_SCD` davor bleibt; 13 < 32). |
| `re15_port/tests/unit/test_r34n_d_adaruf.c` + `probes/r34n_d_adaruf.cmake` (add_test `unit_r34n_d_adaruf`) | NEU (Riegel) | aus der Sonde, aber ueber die ECHTEN Haken: Faelle A (Szene), B (gesehen -> Text), C/D (gerettet -> Tuer), E (ROOM1051 unveraendert); Ergebnis pruefen (Nachrichtenfolge 22/23/24, Ankunft <= 12 Bilder, Weg 650..760, Clips 19/19+0x80/17, Flags danach, Slot 4 = Text 25), Herkunft der Bytes (Glyphen-Fundstellen 5.5, Muster-Offsets 5.3 gegen ROOM1090/1170/1130.RDT). Gegenprobe: HAKEN 2 aus -> Szene startet nicht (Riegel ROT). |
| `re15_port/tools/local_build.sh` | Zahl | `RE15_MIN_TESTS` +1 |

Keine Aenderung an: aot_common.c, msg_common.c, game_step_common.c, main.c, door_seq_*.c, RDT-Dateien.

### 5.2 Installation (`re15_adaruf_install`, Muster re15_tuer1120_install)

1. Nur ROOM1050 (`room != 0x1050` -> nichts). ROOM1051 bewusst nicht (L8).
2. (3,0xBB)=1 -> nichts (Originaltuer inkl. Tuersequenz).
3. Slot 4 muss aktive Tuer sein (`type == RE15_AOT_TYPE_DOOR`), sonst nichts (Schutz gegen kuenftige Skripte).
4. Nachrichten 22..25 per `re15_msg_install_text` + `re15_msg_install_durations(re15_msg_compute_duration)`.
5. `g_aot.slots[4].band = g_aot.door_params[4].band;` (wie tuer1120: Aot_reset schreibt rec[2] nicht).
6. (9,65)=0 -> `re15_aot_retype(4, 3, 0x31, 0x00FF, (13<<8)|0x18, 0)` (Ereignis 13);
   (9,65)=1 -> `re15_aot_retype(4, 1, 0x31, 25, 0xFFFF, 0)` (Text).

Weiche (`re15_adaruf_ereignis`): nur `room == 0x1050 && event_id == 13`; kopiert `k_ruf` nach `s_prog`, setzt aus
der Spielerposition im Druckbild den Blickpunkt `(x+755, z)` in `s_prog[0x28..0x2B]` und das Rueckschritt-Ziel
`(x-755, z)` in `s_prog[0x3C..0x3F]` (LE s16), gibt `s_prog` zurueck. `scd_event_fire` startet den Faden im ersten
freien Ereignis-Slot (10..23) wie jedes Raum-Sub.

### 5.3 Der Bytecode `k_ruf` (142 Bytes; Sonde = Plan)

| Vers. | Bytes | Opcode | Vorbild |
|---|---|---|---|
| +00 | `22 09 41 01` | Set(9,65)=1 | Einmal-Riegel am Szenenanfang wie ROOM11B0 sub06 @0x1478 `22 03 83 01` |
| +04 | `22 02 07 01` | Set(2,7)=1 | ROOM1090 sub02 @0x02414 |
| +08 | `22 01 1b 01` | Set(1,27)=1 | @0x02418 |
| +0C | `2e 01 00 00` | Work_set(1,0)+Nop | @0x0241C |
| +10 | `40 00 06 3f 00 00 00 00` | Plc_dest Modus 6 | @0x0242C |
| +18 | `09 0a 14 00` | Sleep 20 | @0x02434 |
| +1C | `2b 16 00 00` | Message_on 22, Maske 0 | Form @0x02438 `2b 00 00 00` |
| +20 | `09 0a 64 00` | Sleep 100 | @0x0243C |
| +24 | `40 00 09 20 xx xx zz zz` | Plc_dest Modus 9 (drehen), Bit 0x20, Blickpunkt zur Laufzeit | ROOM1050 sub03 @0x0DC2 `40 00 09 20 ...`; Handler @0x80031360 (Clip 5 @0x800313a4, Kegel `ori a2,0x60` @0x800313d4, Drehrate 0x60 @0x80031440, kein Vortrieb) |
| +2C | `11 00 08 00` `02 00` `12 04` `21 05 20 00` | Do / Evt_next / Edwhile Ck(5,32)==0 | ROOM1050 sub03 @0x0DCA..0x0DD5 |
| +38 | `40 00 08 20 xx xx zz zz` | Plc_dest Modus 8, Bit 0x20, Ziel zur Laufzeit | Form ROOM1090 sub02 @0x0247C |
| +40 | `11 00 08 00` `02 00` `12 04` `21 05 20 00` | Do / Evt_next / Edwhile Ck(5,32)==0 | ROOM1090 sub04 @0x026E6..0x026F1 (inline: ROOM1050 sub04 ist Adas Weg, kein Gosub moeglich) |
| +4C | `09 0a 14 00` | Sleep 20 | ROOM1090 sub02 @0x02486 |
| +50 | `2b 17 00 00` | Message_on 23 | Form @0x0248A |
| +54 | `3f 00 13 00` | Plc_motion(0,19,0) | ROOM1090 sub03 @0x0265C |
| +58 | `09 0a 19 00` | Sleep 25 | @0x02660 |
| +5C | `3f 00 13 00` `43 00 80 00` | Plc_motion 19 + Plc_flg(0,0x80,0) | @0x02664/@0x02668 |
| +64 | `09 0a 1a 00` | Sleep 26 | @0x0266C |
| +68 | `2b 18 00 00` | Message_on 24 | Form @0x02670 |
| +6C | `3f 00 11 00` | Plc_motion(0,17,0) | ROOM1170 sub02 @0x015F0 |
| +70 | `09 0a 64 00` | Sleep 100 | ROOM1170 sub02 @0x015F4 |
| +74 | `22 02 07 00` `22 01 1b 00` `2e 01 00 00` `42 00` | Szenen-Ende | ROOM1090 sub02 @0x024BE..@0x024CA |
| +82 | `46 04 01 31 19 00 ff ff 00 00` | Aot_reset(4, sce 1, 0x31, msg 25, 0xffff, 0) | ROOM1130 sub01 @0x00A1C |
| +8C | `01 00` | Evt_end | — |

### 5.4 Konstanten-Tabelle

| Konstante | Wert | Beleg @0x… / Datei-Offset bzw. NUTZER-VORGABE / PORT-WAHL + Grund |
|---|---|---|
| RE15_ADARUF_RAUM | 0x1050 | NUTZER-VORGABE ("von ROOM 1050 zu ROOM 10A0"); nur Leon: ROOM1091 sub00 @0x0222E / sub01 @0x02230 = `01 00`, (3,0xBB) nur ROOM1090 sub03 @0x24D2 |
| RE15_ADARUF_SLOT | 4 | ROOM1050 main00 @0x00B5A `3b 04 02 31 ...`, Zielbyte `0a` @0x00B71 |
| RE15_ADARUF_FREI_BANK/BIT | 3 / 0xBB (187) | ROOM1090 sub03 @0x24D2 `22 03 bb 01`; Zensus 240 RDTs: kein Loeschen. NICHT (3,0x6E): geloescht ROOM1050 sub03 @0x0D88 `22 03 6e 00` |
| RE15_ADARUF_GESEHEN_BANK/BIT | 9 / 65 | VERTRAG §1.1 (Spur D 65/66); Zensus: kein Ck/Set/Item-Bit (9,65/66) in 240 RDTs, kein Port-Code |
| RE15_ADARUF_EREIGNIS | 13 | PORT-WAHL — Grund: < RE15_RDT_MAX_SUB_SCD 32 (Schranke in scd_event_fire), kein ROOM1050-Sub (Tabelle @0xC10, 5 Eintraege), nicht Spur As Ereignis 2; = Vertrags-Slotnummer 13 zur Wiedererkennung |
| Ereignis-Nutzlast | p0 0x00FF, p1 0x0D18 | Form ROOM1050 sub00 @0x00C22 Nutzlast `ff 00 18 02`; Handler @0x800430f0 `lhu a0,0` @0x800430fc, `lbu a1,3` @0x80043100 |
| Platz-Flags | 0x31 | Door_aot_set @0x00B5A pc[3]; Schalter @0x00C22 pc[3]; ROOM1130 @0x00A1C |
| Text-Platz | sce 1, msg 25, Maske 0xFFFF | ROOM1130 sub01 @0x00A1C `46 03 01 31 01 00 ff ff`; 524/524 sce-1-Saetze 0xffff (scd_vm.c) |
| Nachrichten-Ids | 22, 23, 24, 25 | VERTRAG §1.3; ROOM1050 Nachrichtentabelle @0xE44 off[0] `12 00` = 9 Eintraege (0..8); PSX MSG_TABLE_N 32 |
| Rueckschritt-Laenge | 755 | NUTZER-VORGABE "wie in ROOM 1090": \|(613,-2123)-(-130,-1988)\| = 755,2 aus ROOM1090 sub02 Member_set @0x0246C/@0x02470 und Plc_dest @0x0247C; tatsaechlich 700 wegen Ankunft `slti 100` @0x800312fc (Sonde B123..B133 = echte exe ROOM1090 Bild 301..311) |
| Rueckschritt-Richtung | -X (x-755, z gleich) | PORT-WAHL — Grund: die Tuer liegt auf der Ostseite (Rechteck x 16700..17700 @0x00B5A, Standplatz Gierung 0 = Blick +X); Modus 8 dreht den Ruecken zum Ziel (`addiu a2,zero,-48` @0x80031254), Leon bleibt also zur Tuer gewandt; z bleibt, also keine Kamerazone (Satz 12/13 @0x2A0/@0x2B4). Das Original stellt Leon vorher per Member_set auf Ort UND Richtung (@0x0246C-74) — hinter einem Cut_chg versteckt; hier ohne Schnitt waere der Sprung sichtbar, darum Ziel relativ zum Standort (C setzt die Operanden). |
| Drehung vor dem Schritt | Plc_dest Modus 9 -> (x+755, z) | PORT-WAHL — Grund: ohne Drehung kreist Modus 8 aus schraegem Stand um das Ziel (Sonde, 3 von 7 Druckstellen endlos; Wendekreis 70/(48·2π/4096) ≈ 950 > 755). Modus 9 ersetzt die Richtungs-Stellung des Originals (Member_set 4 = 145 @0x02474) sichtbar und weich; Blickpunkt-Entfernung beliebig (arc_test @0x800313d0 prueft nur den Winkel), 755 = dieselbe Laenge wie der Schritt. Form ROOM1050 sub03 @0x0DC2 (Leon, Modus 9, Bit 0x20). |
| Plc_dest-Form | `40 00 08 20` / `40 00 09 20` | ROOM1090 sub02 @0x0247C / ROOM1050 sub03 @0x0DC2; Ankunftsbit 0x20 = Ck(5,32) (ROOM1090 sub04 @0x026EE, ROOM1050 sub03 @0x0DD2) |
| Szenen-Rahmen | (2,7), (1,27), Plc_dest 6, Plc_ret | ROOM1090 sub02 @0x02414/@0x02418/@0x0242C/@0x024BE/@0x024C2/@0x024CA; Plc_ret-Handler @0x80041f88 |
| Sleep 20 / 100 / 20 | 0x14 / 0x64 / 0x14 | ROOM1090 sub02 @0x02434 / @0x0243C / @0x02486 |
| Geste b | Clip 19, Sleep 25 + (19, Plc_flg 0x80) Sleep 26 | NUTZER-VORGABE (Lesart L4, Bildbeleg §3.5); Bytes ROOM1090 sub03 @0x0265C..@0x0266C; Clip im ROOM1050-RBJ rec0 = Hash fde929aa6e6b |
| Geste c | Clip 17, Sleep 100 | NUTZER-VORGABE (Lesart L5); ROOM1170 sub02 @0x015F0/@0x015F4; Hash ce75d881cd9d |
| Plc_flg 0x80 | rueckwaerts | Handler @0x80041fb8, `or` @0x80041ffc auf +0x1c4 |
| Sprecher | "Woman:" Farbe 02 / "Leon:" Farbe 01 | ROOM1090 msg 0 @0x275C `04 00 05 02 33 4b 49 3d 4a 16 05 00 00`; msg 1 @0x279C `04 00 05 01 28 41 4b 4a 16 05 00 00` |
| Dialog-Ende | `04 01 01 63` | ROOM1090 msg 0/1 (Standzeit 99, re15_msg_compute_duration) |
| Text-Ende | `04 02 ... 01 00` | ROOM1130 msg 1 @0x0B46 |
| Umbruch msg 22 | nach "Please," | Vorbild ROOM1090 msg 0 bricht vor "get me out of here!?" (@0x2784); Breiten 204 px (mit "Woman: ") / 129 px (font_width.h) |

### 5.5 Nachrichten (Rohbytes, `tools/r34n_d/texte_bauen.py`)

    22 "Woman: Hello? Anyone? Please, / get me out of here!"  (59 B)
       04 00 05 02 33 4b 49 3d 4a 16 05 00 00 24 41 48 48 4b 1b 00 1d 4a 55 4b 4a 41 1b 00 2c 48 41 3d 4f 41 18
       08 43 41 50 00 49 41 00 4b 51 50 00 4b 42 00 44 41 4e 41 1a 04 01 01 63
    23 "Leon: Another civilian survivor."  (42 B, 218 px)
       04 00 05 01 28 41 4b 4a 16 05 00 00 1d 4a 4b 50 44 41 4e 00 3f 45 52 45 48 45 3d 4a 00 4f 51 4e 52 45 52
       4b 4e 57 04 01 01 63
    24 "Leon: I have to help her!"  (35 B, 165 px)
       04 00 05 01 28 41 4b 4a 16 05 00 00 25 00 44 3d 52 41 00 50 4b 00 44 41 48 4c 00 44 41 4e 1a 04 01 01 63
    25 "I have to help the Survivor first!"  (Nutzer-Schreibung, eine Zeile 226 px)
       04 02 25 00 44 3d 52 41 00 50 4b 00 44 41 48 4c 00 50 44 41 00 2f 51 4e 52 45 52 4b 4e 00 42 45 4e 4f 50
       1a 01 00

Wortstuecke mit Fundstelle im Auslieferungsstand (erste gefundene, laengstes Stueck):
"Woman:" ROOM1090 msg 0 @0x2760 · "Hel" ROOM1021 msg 1 @0x2319 · "lo" ROOM1011 msg 15 @0x11CD · "? A" ROOM30E1
msg 6 @0x1099 · "nyone" ROOM1031 msg 19 @0x2FBC · "? " ROOM1011 msg 4 @0x0F8F · "Please," ROOM1011 msg 16 @0x11E1 ·
"get me out of here!" ROOM1090 msg 0 @0x2784 · "Leon:" ROOM1050 msg 7 @0x103B · "An" ROOM1010 msg 1 @0x0A90 ·
"other " ROOM1011 msg 13 @0x1164 · "ci" ROOM1011 msg 6 @0x0FFE · "vi" ROOM1011 msg 13 @0x1156 · "li" ROOM1011
msg 18 @0x126A · "an su" ROOM1240 msg 0 @0x0673 · "rvivor" ROOM1011 msg 13 @0x1155 · "." (0x57) ROOM1000 msg 0
@0x0D33 · "I have to " ROOM1170 msg 16 @0x1C4B · "help her" ROOM11B0 msg 11 @0x1BBA · "!" ROOM1000 msg 0 @0x0E10 ·
"help" ROOM1011 msg 16 @0x11F5 · "the " ROOM1011 msg 1 @0x0F13 · "S" ROOM1010 msg 0 @0x0A54 · "urvivor" ROOM1011
msg 13 @0x1154 · " first" ROOM11B0 msg 11 @0x1BAE. Glyphentabelle = Umkehrung msg_common.c re15_msg_glyph.

### 5.6 Sprachdateien (der Nutzer nimmt auf, MiniMax)

| Datei | Wortlaut | Hinweis |
|---|---|---|
| `synchro/STAGE1/room1050/main22.wav` | Woman: "Hello? Anyone? Please, get me out of here!" | <= 100 Bilder (3,3 s): der Rueckschritt folgt nach Sleep 100 und wartet nicht auf die Stimme |
| `synchro/STAGE1/room1050/main23.wav` | Leon: "Another civilian survivor." | Message_on 24 wartet ihr Ende ab |
| `synchro/STAGE1/room1050/main24.wav` | Leon: "I have to help her!" | laeuft ueber das Szenenende hinaus weiter, wenn > 3,3 s |
| (`main25.wav`) | "I have to help the Survivor first!" | wird NICHT abgespielt: Text-Platz (sce 1) ohne Sprachausgabe, wie tuer1120 und alle Untersuchungstexte (scd_vm.c re15_scd_show_message) |

Nichts aus `synchro/unused` zuordnen. Ohne Dateien laufen alle Zeilen stumm mit Untertitel (Zeitlinie §4).

## 6 Abnahmeplan

Echte exe (`tools/r34n_d/messlauf.sh`, eigene Kopie), Weg in den Suedteil wie im Spiel ueber die ROOM1000-Tuer
(`RE15_SET_FLAG=3:121 RE15_DEBUG_JUMP=1000@120 RE15_PLAYER_POS=22230,-13400,0,0 RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=100 RE15_INPUT_SCRIPT=U0.8,W14`), dann in ROOM1050 Quadrat per `RE15_PRESS=square@150`
(feuert zuerst die 1000-Tuer, dann in ROOM1050 die 10A0-Tuer). Bilder `RE15_FRAMEDUMP` (RE15_WINDOW_SCALE=3),
Protokoll `RE15_MSG_LOG`, `RE15_SCD_TRACE`, `RE15_FLAG_TRACE`.

| Nutzerpunkt | Messung | Soll |
|---|---|---|
| Szene statt Raumwechsel | debug.log: `[adaruf]`-Zeile, KEIN `DOOR FIRE slot=4`, kein `room10a0` | Szene startet im Druckbild |
| Balken wie Original-Szenen | FRAMEDUMP B+20: Balken oben/unten; B+322: weg | Letterbox voll ab B+16, zu nach B+305+15 |
| Ruf "Woman: ..." | FRAMEDUMP B+30 | Text 2-zeilig, "Woman:" in Farbe 02 (wie ROOM1090 msg 0) |
| Schritt zurueck wie 1090 | FRAMEDUMP B+119..B+137 alle 2 Bilder, `[walk]`-Zeilen; zweiter Lauf mit schraegem Stand (`RE15_PLAYER_POS` 16300,-12300 Gierung 512 vor dem Druck) | Drehung zur Tuerwand, dann 10 Bilder / ~700 Einheiten rueckwaerts, Blick bleibt zur Tuer; Bild neben `D_belege/r1090_sub02_rueckschritt_echt.png` |
| Geste bei "Another civilian survivor" | FRAMEDUMP B+154..B+205 alle 5 | Clip 19 hinaus + denselben Weg zurueck (vgl. `geste19_17_kamera4.png`) |
| Arm-Schwung bei "I have to help her" | FRAMEDUMP B+205..B+235 alle 3 | Clip 17 (vgl. Intro `r1170_sub02_clip17_echt.png`) |
| Folgetext | zweiter Druck (`RE15_PRESS=square@150,square@520`) | "I have to help the Survivor first!", Spiel steht bis Tastendruck, kein Raumwechsel |
| auch nach Raumwechsel | Lauf mit `RE15_SET_FLAG=3:121,9:65` | erster Druck = Text, keine Szene |
| erst nach Rettung durch die Tuer | Lauf mit `RE15_SET_FLAG=3:121,3:187` | `DOOR FIRE slot=4`, Tuersequenz P07G, room10a0 |
| Ablauf ueber die echte Rettung | Lauf ROOM1090 mit `RE15_SET_FLAG=3:132,3:128,3:129,3:133` (Sub03 wie in §3.3-Messung), dann Tuer Slot 0 -> 1050 | (3,0xBB)=1 im FLAG_TRACE, "Hey, wait!"-Szene laeuft, danach Slot 4 = Tuer |
| Elza | `RE15_DEBUG_JUMP=1051@120` + Standplatz + Quadrat | Tuer nach 10A1 wie bisher |

Gegenproben: (a) Riegel `unit_r34n_d_adaruf` mit HAKEN 2 auskommentiert -> ROT (Szene startet nicht, Druck
bleibt folgenlos — Port meldet `event 13 DROPPED`); (b) Freigabe probeweise auf (3,0x6E) -> Riegel-Fall "Rueckkehr
nach Rettung" ROT (0x6E ist nach ROOM1050 sub03 wieder 0). Suite im eigenen Baum; GUI-Haken unter Last einzeln
nachfahren (memory reai-v2-gui-tests-flattern).

## 7 Risiken, Softlocks, Wechselwirkungen

* **Softlock-Pruefung (Pflicht).** Tuergraph ab ROOM1170 (`tuer_graph.py`, `D_belege/tuergraph_stage1_leon.txt`):
  ohne ROOM10A0 sind 22 Leon-Raeume erreichbar, darunter 1000, 1050, 1090; 10A0 und alles dahinter (1180, 1160,
  1190, 11B0-11F0, 1230, …) NUR ueber 1050 Slot 4 — die Sperre kann also nicht von der anderen Seite umgangen
  werden. Innerhalb: Suedteil nur durch das Rolltor (Spur A, §2.3); Feuerloescher ROOM1000 Slot 3 (Umkleide,
  Kameragruppe {0,1,2}) haengt an 1050-Sued Slot 3 <-> 1000 Slot 0; Ada ROOM1090 Hof an 1050 Slot 5. Die
  1090-Kette braucht (3,133) aus ROOM1000 — nichts hinter 10A0. Kein Softlock.
* **Freigabe-Flag.** Wer (3,0x6E) nimmt, sperrt die Tuer nach Adas Weglaufen wieder (Softlock). (3,0xBB) ist
  Pflicht. Alte Spielstaende nach der Rettung tragen (3,0xBB)=1 -> Tuer offen, keine Szene (richtig).
* **Rueckschritt haengt?** Erste Planfassung (Schritt ohne Drehung) haengte aus schraegem Stand endlos (Modus 8
  kreist, Do-Schleife wartet ewig = Szene friert, Spieler gesperrt). Die Drehung (Modus 9) davor behebt das:
  Sonde Fall E, sieben Druckstellen (Standplatz, Nord-/Suedrand, schraeg NW/SW, im Rechteck mit Blick +Z/-Z) —
  alle enden (B305..B314), echte Kollision. Der Riegel prueft dieselben sieben Stellen.
* **Spur A (Rolltor, gleiche Datei scd_vm.c):** beide Weichen in `scd_event_fire`, disjunkt (A: Ereignis 2 +
  Flags, D: Ereignis 13). Reihenfolge egal; bei der Zusammenfuehrung EINE zusaetzliche Zeile (A_rolltor.md §7
  bietet genau das an). Cut 4 ist bei der Szene frei, weil A das Tor zur Pflicht macht (§2.3).
* **Spur E (Dokument an der Leiche, ROOM1050 Slot 15, msg 26/27):** kein gemeinsamer Slot, keine gemeinsame
  Nachricht; Leiche noerdlich des Rolltors, Tuer 10A0 suedlich.
* **Tuersequenz (door_seq_zuordnung.c):** unberuehrt; sie haengt an `aot_fire_door`, das bei umgewidmetem Slot 4
  nicht laeuft und nach der Rettung wieder laeuft (Sonde Fall C: Raumwechsel 0x10A0 angefordert).
* **Stimme laenger als die Sleeps:** main22 > 3,3 s ueberlappt den Rueckschritt (kein Opcode wartet auf eine
  Stimme; der Port-Riegel `voice_wait` gilt nur fuer Message_on). Aufnahme-Hinweis 5.6.
* **Mehrfach-Ausloesung:** ausgeschlossen — (9,65) wird im ersten Szenenbild gesetzt, die Pad-Sperre haelt die
  Taste, und am Ende wird Slot 4 zum Text-Platz.

## 8 Offene Punkte

* **Gesten-Zuordnung ist eine Lesart (L4/L5)**, am Bild begruendet, aber die Worte "180°" und "rechts" passen
  auch auf Clip 17 (Unterarm schwenkt von quer vor der Brust nach aussen). Alternative, falls der Nutzer anders
  meint: b = Clip 17 vor + zurueck, c = Clip 15 (Arm-Schwung nach vorn, "Hey!"). Umstellung = die Clip-Bytes an
  +0x56/+0x5E bzw. +0x6E. Katalogbild `D_belege/gesten_katalog_vorn.png` fuer die Rueckfrage bei der Abnahme.
* **Sprechername "Woman:" vs. "Ada:"** (L2) — 6 gegen 4 Namensbytes.
* **Echte-exe-Abnahme** erst mit dem Bau (§6); in dieser Stufe nur die Sonde an der echten VM/Spielschritt.
