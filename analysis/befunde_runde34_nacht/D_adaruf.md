# Spur D — Ada-Ruf an der Tuer ROOM1050 -> ROOM10A0 (Runde 34 Nacht)

Stufe: ERMITTLUNG + BAUPLAN (kein Port-Code). Arbeitsbaum `.claude/worktrees/r34n_adaruf`, Zweig `r34n/adaruf`.
**BAU-Stand (§9):** gebaut wie geplant plus EINE Abweichung (Gegenpruefung Auflage 7): nach dem Rueckschritt
dreht sich Leon zur Kamera Cut 4, erst dann die zwei Leon-Zeilen mit den Gesten — nur so geht der Gestenarm im
Bild "nach rechts" (Programm 162 statt 142 Bytes). Riegel `unit_r34n_d_adaruf_*` (8 Teile), Suite §9.4, Abnahme
an der echten exe §9.5.
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
* **Lesarten (§1):** Bildschirmname "Woman:" statt "Ada:" (RE1.5-Konvention vor der Rettung, vom Nutzer
  bestaetigt: AUFTRAG.md Nachtrag); "rechter Arm ... nach rechts" = Zuschauersicht (alle Leon-Gesten sind
  LINKSARMIG, die rechte Hand haelt die Waffe); Leons Zeile als zwei Nachrichten (Gesten- und Stimmen-Takt).

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
| L2 | "Ada:" | Bildschirmtext **"Woman:"** — **vom Nutzer bestaetigt** (AUFTRAG.md, Nachtrag 2026-09-30: "Du hast recht! Da muss woman: stehen statt Ada:"). | RE1.5 nennt Ada vor und waehrend der Rettung "Woman:" (ROOM1090 msg 0 @0x275C, msg 2 @0x27BD, msg 4 @0x281F); "Ada:" erstmals NACH der Rettung (ROOM1050 msg 6 @0x1007, ROOM11C0 msg 1..8). Satz bleibt woertlich "Hello? Anyone? Please, get me out of here!". |
| L2b | "Leon: Another civilian survivor. I have to help her!" = eine oder zwei Nachrichten? | ZWEI: msg 23 "Leon: Another civilian survivor." und msg 24 "Leon: I have to help her!" — **PORT-WAHL (BAU, Auflage 4)** | Grund = Sprach-Takt: jede Geste haengt direkt hinter ihrem Message_on, und `voice_wait` haelt msg 24 bis main23.wav fertig ist — nur so bleibt die zweite Geste am zweiten Satz, egal wie lang die (noch nicht aufgenommene) Stimme wird. **Gegenbeleg (Gegenpruefung, angenommen):** RE1.5 setzt zwei Saetze eines Sprechers auch in EINEN Kasten, mit zwei Gesten waehrend EINER Nachricht (ROOM1090 msg 5 @0x2861 "We can talk later... It's not safe / here.", sub03 @0x02640 Message_on 5, Clip 18 @0x02648/@0x02650, Clip 19 @0x0265C/@0x02664); das frueher hier zitierte Vorbild msg 5/6 belegt die Aufteilung NICHT. **Lesbarkeit msg 23 ohne Stimme (51 Bilder, Sleep 25+26) am Zensus gemessen** (`dialog_budget`, 301 Dialogzeilen aller RDTs mit fester Zeit bis zum naechsten Message_on): das Original laesst eine Dialogzeile selbst nur 51 Bilder stehen (ROOM4001 sub10 @0x01A24 msg 9 "John: Sherry, hold on!", @0x01A3C msg 10) und kommt bis 0,99 Bilder je Zeichen herunter (ROOM1141 sub02 @0x00EBC, 101 Bilder / 102 Zeichen); msg 23 hat 26 Zeichen = 1,96 Bilder je Zeichen — innerhalb der Original-Praxis, keine Verlaengerung der Sleep-Kette. Framedump F315..F366 (§9.5): der Text steht ganz. |
| L3 | "einen Schritt zurück ... wie in ROOM 1090 ..., nachdem er zum Feuer läuft" | `Plc_dest` Modus **8** (ROOM1090 sub02 @0x0247C), gleiche Weglaenge. | Der einzige Rueckwaertsbefehl in sub02, direkt nach den drei Lauf-`Plc_dest` Modus 5 zum Feuer (@0x0244A/0x02454/0x0245E). Der Nutzer hat genau diesen Schritt schon einmal beanstandet ("im Original macht er noch EINEN Schritt zurueck, bei uns ZWEI", actor_locomotion.c) — er ist ihm vertraut. |
| L4 | "den rechten Arm um 180° dreht und nach rechts bewegt ... den gleichen Weg wieder zurück" | **Clip 19**, vorwaerts + rueckwaerts (`Plc_flg 0x80`). | §3.5: alle Leon-Gesten bewegen den LINKEN Arm (Knochen 12-14); der rechte (9-11, Waffenhand) steht. "rechts" ist also Zuschauersicht (Leon frontal: sein linker Arm ist rechts im Bild). Clip 19 hebt den Arm seitlich nach aussen (Hand +300 nach aussen, Unterarm von haengend -79 Grad auf waagerecht +11 Grad) und dreht die Hand dabei um (Handflaeche vom Oberschenkel nach oben, ~100 Grad) — "dreht und bewegt nach rechts". "den gleichen Weg wieder zurück" = das Muster, in dem Clip 19 im Original IMMER laeuft (vor + `Plc_flg 0x80`, 11 von 26 Aufrufen rueckwaerts; z.B. ROOM1090 sub03 @0x0265C/@0x02664). |
| L5 | "seine Arm Schwung Animation" | **Clip 17**, einmal vorwaerts (kehrt selbst in die Ruhe zurueck). | Der einzige Schwung der Bibliothek: Hand von der Brustmitte bis ganz seitlich hinaus (+780 Einheiten), Unterarm schwenkt 107 Grad. Leons "Now what am I gonna do?" im Intro (ROOM1170 sub02 @0x015F0, im Port bei Bild 1686 gemessen), "Ada, you hide inside that patrol car.", "Hurry up! They're coming!". Clip 17 wird im Original NIE rueckwaerts gespielt — passt dazu, dass der Nutzer hier keinen Rueckweg nennt. |
| L6 | "erneut die Tür" | Jeder weitere Druck vor der Rettung zeigt NUR den Text msg 25; auch nach Raumwechsel (Bit (9,65) gespeichert). | Wortlaut. |
| L7 | "Erst wenn man Ada gerettet hat" | Freigabe (3,0xBB). | §3.2. |
| L8 | Elza ROOM1051 | Keine Sperre. | ROOM1091 hat weder Feuer noch Ada noch Nachrichten (sub00 @0x0222E und sub01 @0x02230 = `01 00`), (3,0xBB) wird in Elzas Spiel nie gesetzt; Sperre waere dauerhaft. Gleiche Regel wie tuer1120 (nur ROOM1130). |
| L9 | Kamera | Aktive Kamera (Cut 4), kein Cut_chg. **BAU (Auflage 7): nach dem Rueckschritt dreht sich Leon zur Kamera Cut 4 (Plc_dest Modus 9 auf ihren Standort), erst dann die Gesten — PORT-WAHL.** | §2.2: an der Tuer ist aus jeder Richtung Cut 4 aktiv, und er zeigt Leon an der Tuer (Bild Mitte in `ist_cut4_rolltor_zu_offen_cut5.png`); die Nachbarkamera Cut 5 zeigt ihn nur als Streifen hinter der Wandkante (rechts). Der Nutzer nennt keinen Schnitt. **Am echten Bild gemessen (§9.5):** zur Tuer gewandt sieht Cut 4 Leon schraeg von hinten links, Clip 19 geht im Bild nach LINKS und Clip 17 verschwindet halb hinter dem Koerper — das ist NICHT "der rechte Arm ... nach rechts". Eine Kamera von vorn gibt es an der Tuer nicht (Kameratabelle RDT @0x00060: Cut 0..4/6..9 westlich/noerdlich x 14332..16171, Cut 5 @0x00100 suedoestlich = Waffenseite). Mit Blick zur Kamera liegt der Gestenarm (sein linker) rechts im Bild und geht nach rechts hinaus — genau die Beschreibung des Nutzers (Lesart L4 = Zuschauersicht, frontal). |

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

**Nachtrag BAU (Gegenpruefung Auflage 2) — GEPRUEFT UND VERWORFEN** (Bild
`D_belege/gegenpruefung_weitere_leon_clips.png`, vom Gegenpruefer mit `gesten_streifen.py` gerendert, von mir
angesehen): Clip 24 des ROOM1050-RBJ (= ROOM3070 Clip 24, dort vor+rueck @0x0350C/@0x03514) = Kopf-/Oberkoerper-
Wendung OHNE Arm; Clip 25 = Ganzkoerper-Drehung um 90 Grad (nie gerufen); die raumeigenen Spieler-Clips ROOM1170
rec0 Clip 25 (@0x015E4, "Oh, that's just freaking great...") = Hand ans Gesicht; ROOM1150 rec0 Clip 10/11/12 (Irons-
Szene sub03/sub08) = KNIEND. Selbst nachgerendert (`D_belege/bau_weitere_leon_clips_50b0_2020.png`): ROOM50B0 rec0
Clip 13 (84 B, sub04 @0x00A76, Adas Tod) = Haende an den Kopf, Zusammensacken auf die Knie; Clip 14 (3 B, @0x00A88)
= kniend; ROOM2020 rec0 Clip 0 (40 B, sub02 @0x00A9C "Will you use the Crank?") = beide Arme nach vorn, Kurbel.
(ROOM50B0/ROOM10D0 rufen ausserdem Clip 4 = INHALT von Clip 19, Hash fde929aa6e6b.) Keiner ist eine
stehende Arm-Geste "180 Grad nach rechts" — die Wahl 19/17 bleibt. **Zensus berichtigt:** `plc_motion_zensus.py`
nahm den ERSTEN Record mit Bit 0; der Binder FUN_8001b3f8 laeuft aber ueber alle Records ohne Abbruch (Schleife
@0x8001b42c..@0x8001b4d4) und schreibt die Spieler-Bindung bei jedem Bit 0 neu (`sw v0,0(t3)` @0x8001b450,
`sw v0,4(t3)` @0x8001b460, t3 = 0x800acbd4) — der LETZTE Record gewinnt (ROOM6030 rec2, Marker 3). Neu erhoben
(`D_belege/plc_motion_zensus.tsv`, 661 Aufrufe, ohne Hash 91 -> 33): Inhalt Clip 17 (ce75d881cd9d) 11x, nie
rueckwaerts; Inhalt Clip 19 (fde929aa6e6b) 32x, davon 14x rueckwaerts. Schluss unveraendert.
Nebenbefund (NICHT Spur D, ungeprueft): der Gegner-Index a3 im selben Binder wird nur einmal genullt
(`addu a3,zero,zero` @0x8001b410, vor der Record-Schleife) und je Marker-Bit hochgezaehlt (`addiu a3,a3,1`
@0x8001b4c4) — bei mehreren Gegner-Records haengt die Zuordnung Bit -> Gegner also von den Vorgaenger-Records ab;
der Port (enemy_common.c rbj_resolve_slot: erster Record mit Bit == Slot) rechnet anders. Fuer die Integration
notiert.

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

`Set(2,7)=1` (Pad-Sperre) + `Set(1,27)=1` (Letterbox) am Anfang. Selbst nachgeprueft: Bank-Tabelle @0x80074664
(`read`): [1] = 0x800aca3c, [2] = 0x800aca40 (Pausemaske), [3] = 0x800b0ff8; Bit-Zaehlung MSB-first -> (1,27) =
Maske 0x10, (2,7) = 0x01000000. Letterbox FUN_80021a0c: `lw v0,0x800aca3c` @0x80021a10, `andi v0,v0,0x10`
@0x80021a24, Stand @0x800b5568 `sltiu 0xf0` @0x80021a40 -> `addiu +16` @0x80021a54, sonst `sltiu 0x10` @0x80021a68 ->
`addiu +240` (= -16) @0x80021a7c: 15 Bilder auf/zu. Am Ende `Set(2,7)=0` / `Set(1,27)=0` / `Work_set(1,0)`
/ `Plc_ret` am Ende — ROOM1090 sub02 @0x02414/@0x02418 bzw. @0x024BE/@0x024C2/@0x024C6/@0x024CA, ebenso sub03,
ROOM1050 sub02/sub03. Stehen bleiben: `Plc_dest 40 00 06 3f 00 00 00 00` (Modus 6 = 0x800517f0, Clip 1 einmal ->
Clip-2-Idle) — ROOM1090 sub02 @0x0242C. Selbst nachgeprueft (`dis 0x80051840`): Phase `sb v1,6(a0)` @0x80051844,
`sb v1,148(v0)` @0x80051854 (+0x94), `sb zero,149(v0)` @0x80051864, +0x8f = 7 @0x80051874, `jal anim_set` @0x8005188c;
Folgeclip `ori v0,zero,0x2` / `sb v0,148(v1)` @0x800518c4/@0x800518c8. Message_on (`dis 0x800404f4`): `lbu a2,1(v0)`
@0x80040504 (Id), `lhu a3,2(v0)` @0x80040508 (Maske), `sll a3,a3,16` @0x8004051c, `jal 0x80027e68` @0x80040518, PC += 4
@0x8004050c — Maske 0 = Untertitel ohne Einfrieren (wie alle Leon-Dialogzeilen `2b nn 00 00`).

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

**BAU (gebaut, gemessen mit dem Riegel `unit_r34n_d_adaruf_szene`, echte VM + Spielschritt):** mit der Drehung zur
Kamera (+0x4C) verschiebt sich alles ab dem Rueckschritt um 12 Bilder: Ruf B21 (bis B120), Drehung zur Tuer ab
B121, Rueckschritt B123..B133 (Weg 700, dz 0), Drehung zur Kamera bis B145 (cos 1,000), "Leon: Another civilian
survivor." + Clip 19 B165, Clip 19 rueckwaerts B190, "Leon: I have to help her!" + Clip 17 B216, Faden-Ende B316,
Balken voll B15 / weg B330. An der echten exe (Druck bei Bild 150 in ROOM1050, also F = B + 150): Ruf ab F171,
Schritt F273..F283, zur Kamera bis F295, Leon-Zeilen ab F315/F366, Balken weg F480 — im Bild bestaetigt
(`D_belege/bau_abnahme_ueberblick.png`; die Nachrichten-Bilder im Log `[msg] room=1050 id=22/23/24`).

Mit Sprachdateien: `voice_wait` haelt Message_on 23 bis main22.wav zu Ende ist und Message_on 24 bis main23.wav
zu Ende ist; die Gesten haengen jeweils direkt hinter ihrem Message_on und bleiben dadurch am Satz.
**BAU gemessen (Auflage 9, Platzhalter-WAVs, §9.5):** Message_on 24 wartete 61 Bilder auf main23 (3,76 s), Clip 17
kam mit dem zweiten Satz; main22 mit 4,38 s reichte 29 Bilder in Rueckschritt und Drehung hinein.
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
freien Ereignis-Slot (10..23) wie jedes Raum-Sub. **BAU (Auflage 5):** die Weiche liefert das Programm nur, wenn zusaetzlich (9,65)=0, (3,0xBB)=0 und kein Faden das Programm schon ausfuehrt; sonst NULL.

### 5.3 Der Bytecode `k_ruf` (142 Bytes; Sonde = Plan — gebaut 162 Bytes, siehe +4C/+54 und §9.7)

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
| **+4C** | `40 00 09 20 97 3a 34 e0` | **BAU:** Plc_dest Modus 9 zur Kamera Cut 4 (14999,-8140) | Standort ROOM1050.RDT @0x000E4/@0x000EC; Form wie +24. PORT-WAHL (Auflage 7, L9) |
| **+54** | `11 00 08 00` `02 00` `12 04` `21 05 20 00` | **BAU:** Do / Evt_next / Edwhile Ck(5,32)==0 | ROOM1050 sub03 @0x0DCA..0x0DD5 |
| +4C -> **+60** | `09 0a 14 00` | Sleep 20 | ROOM1090 sub02 @0x02486 (ab hier alle Versaetze +0x14: Message_on 23 +64, Clip 19 +68/+70, Plc_flg +74, Message_on 24 +7C, Clip 17 +80, Ende +88..+94, Aot_reset +96, Evt_end +A0; zusammen 162 Bytes) |
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
| Text-Platz | sce 1, msg 25, Maske 0xFFFF | ROOM1130 sub01 @0x00A1C `46 03 01 31 01 00 ff ff`; 524/524 sce-1-Saetze 0xffff (scd_vm.c). **Vorbild im selben Handlungsstrang (BAU, Auflage 11):** ROOM1090 sub00 @0x0230A `21 03 85 00` Ck(3,133)==0 -> @0x0230E `2c 02 01 b1 ...` Aot_set Slot 2, sce 1, Langform (flags 0xB1), Nutzlast `07 00 ff ff` = Text-Platz msg 7 "I must hurry up and get something to / put out this fire to save that woman!" — derselbe Mechanismus (Hinweistext, solange die Bedingung fehlt). Ohne Ton: der sce-1-Handler @0x80043084 ruft nur `jal 0x80027e68` @0x800430a0 (Nachricht oeffnen), kein Se_on; RE2 sperrt Story-Tueren ebenso stumm (re15_tuer1120.h). |
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
@0x0D33 · "I have to " ROOM1170 msg 16 @0x1C4B · "help her" ROOM11B0 msg 11 @0x1BBA · "!" ROOM1011 msg 1 @0x0EF0 ·
"help" ROOM1011 msg 16 @0x11F5 · "the " ROOM1011 msg 1 @0x0F13 · "S" ROOM1010 msg 0 @0x0A54 · "urvivor" ROOM1011
msg 13 @0x1154 · " first" ROOM11B0 msg 11 @0x1BAE. Glyphentabelle = Umkehrung msg_common.c re15_msg_glyph; Suche nur innerhalb der Nachrichten (bis Ende-Code `01 xx`).

### 5.6 Sprachdateien (der Nutzer nimmt auf, MiniMax)

| Datei | Wortlaut | Hinweis |
|---|---|---|
| `synchro/STAGE1/room1050/main22.wav` | Woman: "Hello? Anyone? Please, get me out of here!" | <= 100 Bilder (3,3 s): der Rueckschritt folgt nach Sleep 100 und wartet nicht auf die Stimme (BAU gemessen: eine 4,38-s-Aufnahme reicht 29 Bilder in Rueckschritt und Drehung) |
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
| Elza | Riegel-Fall ROOM1051 (Installation kehrt bei room != 0x1050 sofort zurueck, Slot 4 bleibt Tuer); exe-Lauf mit Elza-Start (Spielerauswahl-Schalter `RE15_PSELECT_*` — getenv-Stelle vor dem Lauf lesen) bis ROOM1051 Slot 4 | Tuer nach 10A1 wie bisher, keine Szene |

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
  **BAU, Auflage 1 — Tuergraph berichtigt:** `tuer_graph.py` las das Stage-Byte nicht (Stage aus dem Quellraum);
  jetzt Byte 22 (bei den 4 Viereck-Saetzen ROOM4030/4031, Satzbreite 40, Byte 30). Falsch waren u.a. ROOM11A0
  @0x01006 Slot 4 (Byte 22 = 0x01 -> **ROOM20A0**, nicht 10A0), 11A0 Slot 0 -> ROOM2070, Slot 1 -> ROOM3000, ROOM1260
  Slot 1 -> ROOM2000. Berichtigte Eingaenge von ROOM10A0 (alle selbst gelesen): 1050 S4 @0x00B5A, 1180 S1 @0x00A34,
  11E0 S2 @0x01552, 1230 S1 @0x00B34 — alle ausser 1050 liegen HINTER 10A0. ROOM1170 main00 @0x01226 Slot 1 -> 10B0
  ist sce 0 und wird nie scharf (die BFS folgt nur sce 2). Die Zahl bleibt: 22 Raeume ohne 10A0, 37 mit.
  **BAU, Auflage 3 — die ganze Kette MIT Bedingungen** (Bytes selbst gelesen, `bytes_check`):
  1170 -S4 @0x012F8 `3b 04 02 31 ... 00 13 07`-> 1130 -S2-> 1150 (erste Irons-Szene setzt (3,94) sub08 @0x01110
  `22 03 5e 01`; die Sicherung liegt im Hebetisch, sub04 vom Port ueber Slot 1 @0x0D7E ohne Vorbedingung scharf)
  -> 1130 -S1 @0x008AE (tuer1120: erst nach (3,94))-> 1120 -> 1060/1080 -> 1040 -> 1030 -S0 @0x01C6A-> 1050 Nord
  -> Rolltor (Spur A: nur mit Sicherung, Bank 9 Bit 63) -> 1050 Sued -S3 @0x00B3A-> 1000 Umkleide (Loescher Item
  0x31 Item_aot_set Slot 3 @0x00C24, sub01 @0x00D00 Ck(9,134) / @0x00D08 Ck(3,133)==0 / @0x00D0C Set(3,133)) -> 1000
  -S0 @0x00BBE (Cut 4)-> 1050 -S5 @0x00B7A-> 1090 (sub00 @0x0230A Hinweistext solange (3,133)=0; sonst Slot 2 ->
  sub06: @0x0271E Set(3,129), @0x02722 Set(3,132), @0x02726 Aot_on 3 Selbsttuer -> sub00 @0x022A6 Ck(3,132)==1 ->
  sub03 @0x024D2 Set(3,187)) -> 1050 -S4-> 10A0. Keine Bedingung dieser Kette liegt hinter 10A0 -> **kein Softlock**.
  Der Riegel `unit_r34n_d_adaruf_rettung` faehrt den Schluss der Kette im Port-VM (1090 sub03 setzt (3,187) selbst,
  1050 sub03 loescht (3,110), Tuer bleibt frei, auch beim Wiederbetreten).
  **Alt-Spielstaende** (heutiger Port, Speicherstand schon HINTER 10A0 ohne Rettung): Rueckweg 10A0 S0 @0x00DB2 ->
  1050; der naechste Druck an Slot 4 zeigt dann die Szene bzw. den Sperrtext, die Rettung (1000/1090) bleibt
  erreichbar -> kein Softlock. Ein Speicherstand IN ROOM1050 ist nicht moeglich (keine Speicherstelle: STAGE1 nur
  1070/1120/1150, re15_savepoint.c:45-47); der CONTINUE-Weg (main.c, ohne scd_room_reenter) kommt also nie direkt
  in ROOM1050 an — jede Ankunft dort laeuft durch die Installation in scd_room_reenter.
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
* **Echte-exe-Abnahme** erst mit dem Bau (§6); in dieser Stufe nur die Sonde an der echten VM/Spielschritt.


## 9 Umsetzung (Stufe BAU, Runde 34 Nacht)

Stand: gebaut, Riegel gruen, an der echten exe abgenommen. Die Suite-Zeile steht in 9.4. Alle Konstanten
stehen mit Beleg in `include/re15_adaruf.h` bzw. im Programm `k_ruf` (`engine/src/adaruf_1050.c`, eine Zeile je
Opcode mit Vorbild-Offset). Fuer den Bau selbst nachgeprueft (Bytes/Instruktionen):
ROOM1050 @0x00B5A (Tuer Slot 4, Stage 0x00 @0x00B70, Raum 0x0A @0x00B71), ROOM1090 @0x024CE..@0x024D9
(`22 03 84 00 22 03 bb 01 22 03 6e 01`), ROOM1050 @0x00D88 (`22 03 6e 00`), ROOM1090 sub02 @0x02414..@0x024CD,
ROOM1050 sub03 @0x00DC2 (`40 00 09 20 7a 3f f6 be` + Warteschleife), ROOM1130 @0x00A1C, ROOM1170 @0x015F0,
ROOM1090 sub03 @0x02640..@0x02673, ROOM1050 @0x00C22, ROOM1050.RDT Kamera Cut 4 @0x000E0; PSX.EXE (`re15_disasm.py`):
Typtabelle @0x8007469c, sce-3-Handler @0x800430f0..@0x80043114, sce-1-Handler @0x80043084..@0x800430b8,
Plc_dest-Tabelle @0x80073e30, Modus 8 @0x800311f0..@0x8003132c, Modus 9 @0x80031360..@0x8003145c, Plc_motion
@0x80041b90..@0x80041bdc, Plc_flg @0x80041fb8..@0x80042014, Flag-Banktabelle @0x80074664, Marker-Binder
@0x8001b3f8..@0x8001b4e0. Eigener Walker-Zensus (206 RDTs, 40674 Opcodes): (3,187) gesetzt nur ROOM1090 sub03
@0x024D2, gelesen nur ROOM1090 sub00 @0x0237A; (3,110) gesetzt @0x024D6, geloescht ROOM1050 sub03 @0x00D88; kein
Ck/Set (9,65)/(9,66).

### 9.1 Auflagen der Gegenpruefung (abgehakt / begruendet abgelehnt)

| # | Auflage | Stand | Wo / Beleg |
|---|---|---|---|
| 1 | Tuergraph mit Stage-Byte | ERLEDIGT | `tools/r34n_d/tuer_graph.py` liest Byte 22 (Viereck-Saetze Byte 30); `D_belege/tuergraph_stage1_leon.txt` neu; §7 mit den Eingaengen 1050 S4 @0x00B5A, 1180 S1 @0x00A34, 11E0 S2 @0x01552, 1230 S1 @0x00B34 und dem toten 1170 S1 @0x01226 (sce 0) |
| 2 | Gestenkatalog vervollstaendigen, ROOM6030 ueber rec2 | ERLEDIGT | §3.5 Nachtrag: Clip 24/25 (1050), 1170 Clip 25, 1150 Clip 10-12 (Bild des Gegenpruefers), 50B0 Clip 13/14 und 2020 Clip 0 selbst gerendert (`bau_weitere_leon_clips_50b0_2020.png`) — alle verworfen. Spieler-Record = LETZTER mit Bit 0 (Binder @0x8001b42c..@0x8001b4d4, Bindung @0x8001b450/@0x8001b460); `plc_motion_zensus.tsv` neu |
| 3 | Softlock-Kette mit Bedingungen, Alt-Spielstaende | ERLEDIGT | §7 (Offsets selbst gelesen); dazu: ROOM1050 ist keine Speicherstelle (re15_savepoint.c:45-47), jede Ankunft laeuft durch scd_room_reenter |
| 4 | L2b neu entscheiden | ERLEDIGT: zwei Nachrichten als PORT-WAHL | §1 L2b mit Gegenbeleg ROOM1090 msg 5 @0x2861; Lesbarkeit am Zensus `tools/r34n_d/dialog_budget.py` -> `D_belege/dialog_budget.txt` (Original-Minimum 51 Bilder ROOM4001 sub10 @0x01A24/@0x01A3C; msg 23 = 1,96 Bilder/Zeichen gegen Original-Minimum 0,99 ROOM1141 @0x00EBC) und am Framedump (msg 23 steht ab F315 ganz). Keine Verlaengerung der Sleep-Kette |
| 5 | Weiche absichern | ERLEDIGT | `re15_adaruf_ereignis`: Raum 0x1050, Ereignis 13, (9,65)=0, (3,0xBB)=0 UND kein Faden fuehrt das Programm schon aus (`programm_laeuft`). Riegel `unit_r34n_d_adaruf_doppel` (Quadrat alle 10 Bilder + zweiter `scd_event_fire(13)` nach dem ersten VM-Takt UND im Druckbild selbst): genau 1 Faden, Ziele unveraendert. Mutationsproben c/c2 (9.6) |
| 6 | Raster statt 7 Punkte | ERLEDIGT | `unit_r34n_d_adaruf_raster`: 100 x 100, Gierung je 256; die Begehbarkeit misst der Teil selbst (Lauf mit Pad UP je Zeile: Ostgrenze 16482 bei z -15700, 16732 sonst — wie vom Gegenpruefer gemessen); 735/735 Druckstellen: Ereignis 13, Faden endet (max B326), Schritt <= 10 Bilder, Weg 632..700, dz 0, Blick zur Kamera cos 1,000; 15 s |
| 7 | Gesten am echten Bild, Suedwest-Druckstelle | ERLEDIGT, mit Folge | Zur Tuer gewandt: Clip 19 geht im Bild nach LINKS, Clip 17 halb verdeckt (`bau_gesten_zur_tuer_verworfen.png`) -> **Drehung zur Kamera** (L9 neu begruendet, 9.7 Nr. 1); danach liegt der Arm rechts im Bild, geht nach rechts hinaus, die Hand dreht auf, derselbe Weg zurueck; der Schwung ist voll sichtbar (`bau_gesten_zur_kamera.png`). Suedwest (16300,-15100), Gierung 3584 an der exe: Drehung zur Wand, Schritt, Drehung zur Kamera, Geste (`bau_druckstelle_suedwest.png`) |
| 8a | Kartenlauf Speichern/CONTINUE | ERLEDIGT | Riegel `unit_r34n_d_adaruf_speicher` (Karte schreiben -> Init -> laden -> Sperrtext; Rettung -> speichern -> laden -> Tuer S021). Echte exe `tools/r34n_d/kartenlauf.sh` mit `probe_r34n_d_karte`: CONTINUE aus ROOM1150 -> 1000 -> 1050: Karte "gesehen" -> Sperrtext (`bau_karte_continue_sperrtext.png`), "gerettet" -> DOOR FIRE Slot 4, Sequenz P07G S021 T013, room10a0 (`bau_karte_continue.txt`) |
| 8b | echter Raumwechsel statt RE15_SET_FLAG | ERLEDIGT | echte exe: Szene -> links drehen, nach Westen -> ROOM1050 Slot 3 -> ROOM1000 -> umdrehen -> Slot 0 -> ROOM1050: `[adaruf] ... Text-Platz sce 1 / msg 25 (Sperre)`, Druck -> "I have to help the Survivor first!" (`bau_sperrtext_nach_raumwechsel.png`); Riegel `unit_r34n_d_adaruf_sperre` ebenso |
| 8c | Quadrat waehrend der Szene | ERLEDIGT | echte exe, 19 Druecke von F150 bis F475: eine Szene, ein Faden, kein DOOR FIRE, kein DROPPED (`bau_quadrat_waehrend_szene.txt`) |
| 8d | A+D-Lauf nach der Zusammenfuehrung | OFFEN (Integration) | Spur A liegt nicht in diesem Zweig; 9.8 |
| 9 | Stimme gegen Zeitlinie | ERLEDIGT | Platzhalter-WAVs (1090 main02 4,38 s / 1090 main06 3,76 s / 1150 main04 4,25 s), danach geloescht, nicht committet: stimme.log `nachricht=24 laeuft=1 ... 61 gewartet` — Clip 17 kommt mit dem zweiten Satz (F430); main22 reicht 29 Bilder in Rueckschritt + Drehung -> Laengenhinweis <= 3,3 s in 9.9 (`bau_stimme_platzhalter.png/.txt`) |
| 10 | local_build.sh nicht anfassen | ERLEDIGT | unveraendert; RE15_MIN_TESTS bleibt 428 (Untergrenze) |
| 11 | Folgetext-Vorbild zitieren | ERLEDIGT | §5.4 Text-Platz: ROOM1090 sub00 @0x0230A/@0x0230E msg 7 |

### 9.2 Dateien

| Datei | Art | Inhalt |
|---|---|---|
| `re15_port/include/re15_adaruf.h` | NEU | Konstanten mit Beleg, API (`re15_adaruf_install`, `re15_adaruf_ereignis`, Pruefhaken) |
| `re15_port/engine/src/adaruf_1050.c` | NEU | Programm `k_ruf` (162 B), Nachrichten 22..25, Installation, Weiche mit Waechtern, `[adaruf]`-Protokoll (nur PC) |
| `re15_port/engine/src/scd_room_setup.c` | HAKEN 1 | +1 include, +1 Aufruf `re15_adaruf_install` direkt nach `re15_tuer1120_install` |
| `re15_port/engine/src/scd_vm.c` | HAKEN 2 | +1 include; `scd_event_fire`: `pc = re15_adaruf_ereignis(...)`, sonst `sub_scd[event_id]` |
| `re15_port/tests/unit/test_r34n_d_adaruf.c` | NEU (Riegel) | 8 Teile: szene, doppel, sperre, frei, rettung, speicher, elza, raster |
| `re15_port/tests/unit/probes/r34n_d_adaruf.cmake` | GEAENDERT | 8 add_test + Kartenwerkzeug (die Sonde der Ermittlung bleibt, ohne add_test) |
| `re15_port/tests/unit/probe_r34n_d_karte.c` | NEU (Werkzeug) | Speicherkarte mit Stand ROOM1150 fuer den CONTINUE-Lauf |
| `re15_port/tools/r34n_d/` | GEAENDERT/NEU | `tuer_graph.py` (Stage-Byte), `plc_motion_zensus.py` (letzter Record), `dialog_budget.py`, `kameras.py`, `kartenlauf.sh` |
| `analysis/befunde_runde34_nacht/D_belege/` | NEU | `bau_*.png/.txt`, `dialog_budget.txt`, `kameras_1050.txt`, neu erzeugt `tuergraph_stage1_leon.txt`, `plc_motion_zensus.tsv` |

**Fuer die Zusammenfuehrung mit Spur A** (beide Weichen in `scd_event_fire`, dieselbe Zeile) so zusammensetzen:

    const uint8_t *pc = re15_rolltor_ereignis((uint16_t)g_current_room_id, event_id);   /* Spur A */
    if (!pc) pc = re15_adaruf_ereignis((uint16_t)g_current_room_id, event_id);          /* Spur D */
    if (!pc) pc = s_current_rdt->sub_scd[event_id];

Die Ereignisse sind disjunkt (A: 2, D: 13). In `scd_room_setup.c` stehen E (`re15_dokumente_install`) und B
(`re15_hebetisch_cursor_install`) VOR, D NACH `re15_tuer1120_install` — nur die include-Zeilen liegen nebeneinander.

### 9.3 Commits

e6f46443 (Modul + zwei Haken + Riegel), 5e9d089a (Drehung zur Kamera), e77d5b2e (Kommentar), efdb343f (Riegel
doppel im Druckbild), 7fa67079 (Auflagen 1-4/11: Doku + Werkzeuge), 76e43499 (Belegbilder), f35f2744 (Auflage 8:
speicher, Kartenwerkzeug, CONTINUE-/Quadrat-Laeufe), 7d2b735c/fd35e68a/26e6bf49 (Doku), dazu der Abschluss-Commit mit der Suite-Zeile.

### 9.4 Suite-Zeile

`bash re15_port/tools/local_build.sh` (configure + build + test) im eigenen Baum, Code-Stand f35f2744 (alle
spaeteren Commits aendern nur Doku/Belege):

    100% tests passed, 0 tests failed out of 436
    === LOCAL-BUILD-OK (all) — Tests 436/436

436 = Basis 428 + 8 neue (`unit_r34n_d_adaruf_szene/_doppel/_sperre/_frei/_rettung/_speicher/_elza/_raster`).
Ein erster Volllauf (Stand efdb343f, 7 Teile) lief ebenfalls durch: `LOCAL-BUILD-OK (all) — Tests 435/435`. In keinem
der beiden Laeufe fiel ein GUI-Haken aus (andere Agenten bauten parallel), also kein Nachfahren noetig.

### 9.5 Eigene Abnahme an der echten exe (Bilder)

Alle Laeufe mit der exe dieses Baums (eigene Kopie `re15_pc_r34n_d.exe`), RE15_WINDOW_SCALE=3, RE15_FRAMEDUMP
(komponiert vor dem Present, mit Balken und Text). Weg wie im Spiel: Debug-Sprung in die Umkleide ROOM1000,
Quadrat an deren Tuer Slot 0 (Tuersequenz P07G), ROOM1050 im Eintritts-Cut 4, 0,8 s vorwaerts, Quadrat bei Bild 150.
Alle Bilder angesehen.

* `bau_abnahme_ueberblick.png` — die ganze Szene: F140 ohne Balken an der Tuer; ab F152 Balken; F172..F268
  "Woman: Hello? Anyone? Please, / get me out of here!" (Sprecher rot = Farbe 02); F272..F284 Drehung zur Wand und
  Schritt zurueck (Beine im Schritt, Blick zur Tuer); F288..F308 Drehung zur Kamera; F316..F352 "Leon: Another
  civilian survivor." mit Clip 19; F368..F460 "Leon: I have to help her!" mit Clip 17; F472 Balken gehen; F484 Spiel.
* `bau_gesten_zur_kamera.png` — Ausschnitt Leon: Clip 19 F318..F348 (Arm rechts im Bild geht nach rechts hinaus,
  Hand dreht auf, derselbe Weg zurueck), Clip 17 F369..F399 (Arm quer vor die Brust, dann ganz nach rechts
  hinaus, zurueck in die Ruhe).
* `bau_gesten_zur_tuer_verworfen.png` — dieselben Gesten mit Blick zur Tuer (erste Bauform): der Arm geht nach
  LINKS, der Schwung verschwindet hinter dem Koerper -> Grund fuer die Drehung (9.7 Nr. 1).
* `bau_druckstelle_suedwest.png` — Druck schraeg von Suedwest (16300,-15100), Gierung 3584 (Debug-Sprung direkt nach
  ROOM1050 + RE15_FORCE_CUT=4, weil der Sprung Cut 0 setzt; im Spiel ist dort Cut 4 aktiv, §2.2): Drehung zur
  Ostwand, Schritt, Drehung zur Kamera, Geste — endet normal.
* `bau_sperrtext_nach_raumwechsel.png` — nach echtem Wechsel 1050 -> 1000 -> 1050 (8b): Schreibmaschinentext
  "I have to help the Survivor first!", keine Szene; ebenso im selben Raum nach der Szene (erster Lauf, F510..F560).
* `bau_karte_continue_sperrtext.png` + `bau_karte_continue.txt` — CONTINUE von Karte (8a): Fall A Sperrtext,
  Fall B Tuer mit Sequenz S021 nach ROOM10A0.
* `bau_quadrat_waehrend_szene.txt` — 19 Quadrat-Druecke waehrend der Szene: eine Szene, kein Raumwechsel.
* `bau_stimme_platzhalter.png` + `.txt` — Platzhalter-Stimmen (9): msg 23 steht, bis main23 zu Ende ist
  (F315..F425), msg 24 + Clip 17 ab F430.
* Elza: Riegel `unit_r34n_d_adaruf_elza` (ROOM1051 Slot 4 bleibt Tuer, Druck -> ROOM10A1); kein exe-Lauf mit
  Elza-Start — ein Versuch mit RE15_PSELECT_AUTO/_AUTO_SWITCH ueber `messlauf.sh` waehlte Leon (Log `Spieler-Familie
  PL00`, die Titel-Automatik des Messlaufs kollidiert mit der Auswahl-Automatik; `integration_elza_vollstart` faehrt
  den Titel anders). Die Installation kehrt fuer jeden Raum ausser 0x1050 in ihrer ersten Zeile zurueck.

### 9.6 Mutationsproben (Fix raus -> Riegel rot, danach zurueck, Riegel wieder gruen)

| Probe | Aenderung | Ergebnis |
|---|---|---|
| a | HAKEN 2 raus (`pc = NULL` statt Weiche) | `szene` ROT, 14 Fehler; Log `event 13 DROPPED` |
| b | Freigabe auf (3,0x6E) | `rettung` ROT, 3 Fehler — schon das ERSTE Betreten nach der Rettung ist gesperrt (ROOM1050 sub03 loescht (3,0x6E) @0x00D88 noch im Init-Lauf) |
| c | nur Waechter `programm_laeuft` raus | `doppel` ROT (2 Faeden bei zweitem Ausloeser im Druckbild) |
| c2 | alle drei Waechter raus | `doppel` ROT (2 Faeden auch nach dem ersten VM-Takt) |
| d | Drehung vor dem Schritt raus (+0x24..+0x37 Nop) | `raster` ROT: 253 von 735 Druckstellen HAENGEN (Szene endet nie) |
| e | HAKEN 1 raus | `szene` ROT, 18 Fehler (Slot 4 bleibt Tuer, Druck wechselt den Raum) |
| f | Blickpunkt (30000,-13700) statt Kamera | `szene` ROT, 2 Fehler (Drehung/Gesten nicht zur Kamera) |

### 9.7 Abweichungen vom Plan (mit Grund)

1. **Drehung zur Kamera nach dem Rueckschritt** (+20 Bytes: +0x4C Plc_dest Modus 9 auf den Cut-4-Standort, +0x54
   Warteschleife; Programm 162 statt 142 Bytes). Grund: Auflage 7 am echten Bild — zur Tuer gewandt geht der
   Gestenarm im Bild nach links; der Nutzer beschreibt "den rechten Arm ... nach rechts", das ist die Sicht von
   vorn; eine Kamera von vorn gibt es an der Tuer nicht (`kameras_1050.txt`). PORT-WAHL, belegt mit dem Kamerasatz
   @0x000E0 und dem Modus-9-Handler @0x80031360.
2. Riegel-Schranke Szenendauer 360 statt 330 (Sleep-Summe 291 + zwei Drehungen je <= 0x800/0x60 = 22 Bilder +
   Schritt <= 12 + Uebergaenge); gemessen max 326.
3. Riegel mit 8 Teilen statt der geplanten Faelle A..E: dazu `doppel` (Auflage 5), `raster` (6), `speicher` (8a).
4. `local_build.sh` NICHT geaendert (Auflage 10 statt Plan 5.1).
5. Nebenbei berichtigt: RDT-Zahl 206 (nicht 240) in den neuen Belegen; `tuer_graph.py`, `plc_motion_zensus.py`.

### 9.8 Offene Punkte

* **Zusammenfuehrung mit Spur A** (Auflage 8d): `scd_event_fire` wie in 9.2 zusammensetzen; danach ein Lauf
  Sicherung -> Tor -> Szene an der 10A0-Tuer (erst dann ist der Suedteil im Spiel ohne RE15_SET_FLAG=3:121
  erreichbar).
* **Sprachdateien** fehlen (Nutzer, 9.9); bis dahin laufen die Zeilen stumm mit Untertitel.
* **Lesart der Gesten** (L4/L5) bleibt eine Lesart; nach der Drehung stimmen Arm-Seite und Richtung mit der
  Beschreibung. Umstellung falls gewuenscht: Clip-Bytes jetzt an +0x6A/+0x72 (Clip 19) und +0x82 (Clip 17).
* **PSX-Ziel** nicht gebaut (kein PSn00bSDK-Lauf in dieser Runde); `adaruf_1050.c` nutzt ausserhalb von
  `#ifdef RE15_PLATFORM_PC` nur memcpy und Engine-Aufrufe.
* **Nebenbefund (nicht Spur D, ungeprueft):** der Marker-Binder FUN_8001b3f8 zaehlt den Gegner-Index a3 ueber alle
  Records hinweg (`addu a3,zero,zero` einmal @0x8001b410, `addiu a3,a3,1` @0x8001b4c4); der Port
  (enemy_common.c rbj_resolve_slot) nimmt "erster Record mit Bit == Slot". Bei Raeumen mit mehreren
  Gegner-Records koennten Clips beim falschen Gegner landen — nachmessen.

### 9.9 Sprachdateien fuer den Nutzer

| Datei | Sprecher / Wortlaut | Laenge |
|---|---|---|
| `synchro/STAGE1/room1050/main22.wav` | Woman: "Hello? Anyone? Please, get me out of here!" | **hoechstens 3,3 s** (100 Bilder) — sonst spricht sie noch, waehrend Leon zurueckweicht (gemessen: 4,38 s -> 29 Bilder Ueberlappung) |
| `synchro/STAGE1/room1050/main23.wav` | Leon: "Another civilian survivor." | frei — "I have to help her!" wartet auf ihr Ende (voice_wait) |
| `synchro/STAGE1/room1050/main24.wav` | Leon: "I have to help her!" | bis 3,3 s endet sie mit der Szene; laenger laeuft sie ueber die Balken hinaus weiter |
| (kein main25) | "I have to help the Survivor first!" | Text-Platz — spielt keine Stimme (wie alle Untersuchungstexte) |

Aus `synchro/unused` wurde nichts zugeordnet; die Platzhalter des Messlaufs sind geloescht.
