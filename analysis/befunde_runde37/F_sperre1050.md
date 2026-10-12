# Runde 37 — Spur F "sperre1050" (Dossier, fortlaufend)

Baum `.claude/worktrees/r37_sperre1050`, Zweig `r37/sperre1050`, Basis master f1e019ca.

## Punkte (Wortlaut, AUFTRAG.md #10 und #5)

- **#10** "bei Leon, bevor dem Ende wo er noch zu Irons muss habe ich eine Lücke gefunden - man kann über die Luke bis
  nach F1 und kann den "Ich muss zuerst irons holen" Blocker bei der Tür überspringen. Ich schlage vor - du machst den
  Text zusätzlich noch in ROOM 1050 bei der Tür, wo wir auch die Ada Cutscene triggern, wenn wir Ada noch nicht gerettet
  haben."
- **#5** "Ja, neuen Texte sind super. Bezug zu ada bei elza kann raus."

## Stand (fortlaufend)

- [x] Schritt 0: Pflichtlektuere (AUFTRAG/VERTRAG Runde 37; Runde 35 L_cut1150 §2.1 (tuer1060), Runde 34 Nacht
      D_adaruf (re15_adaruf.h), Runde 36 P_story (Text-Weg), A_klettern (Tritt 10F0), A_rolltor (1090-Ebenen))
- [x] Schritt 1: Luecke reproduziert (echte exe, ein durchgehender Lauf) + Wege-Zensus zum Ende von Stage 1
- [ ] Schritt 2: Sperre an ROOM1050 Slot 4 (nach der Ada-Rettung, solange der Chief fehlt)
- [ ] Schritt 3: ROOM50B1 msg 0 (Elza) ohne Ada-Bezug + Sweep
- [ ] Tests + gezielte ctest-Laeufe

Werkzeuge (Kopien in `F_belege/`): `tuer_graph.py` (alle Door_aot_set der Leon-Raeume STAGE1 aus den SCD-Dumps
`re15_port/tools/scd_dump_room.py`), `wege_zensus.py` (Erreichbarkeit), `sca_zellen.py` (SCA-Zellen + Typwort).
Messlaeufe: eigene exe-Kopie `re15_port/build/platform/pc/re15_pc_r37f_mess.exe`, Arbeitsordner im Scratchpad.

---

## Punkt #10 — "Chief first"-Sperre zusaetzlich an der Tuer ROOM1050 -> ROOM10A0

### Messung vorher (Schritt 1): welche "Luke"? — REPRODUZIERT an der exe

**Die Luke ist das Lueftungsgitter in ROOM10F0** (Tuer-Zensus Runde 31 T029 "Lueftungsgitter unter der Decke
(ROOM10F0 c7), Band 1, Ankunft y -9000"): Tritt schieben, hochklettern, Gitter = Tuer nach ROOM1090 — und von dort
fuehrt ein **Absprung** vom Laufsteg in den Hof hinunter zur Tuer nach ROOM1050 (1F, Suedteil), an der Sperre
1060 -> 1040 vorbei. Die Annahme der Runde 34 Nacht (A_rolltor.md: "auch ueber 10F0 -> 1090 kommt man NICHT in den
Suedteil (Laufsteg Band 5 abgeschlossen)") galt nur fuer das GEHEN — den Absprung (Kletter-Sonde, Zweig ABWAERTS)
hatte sie nicht geprueft.

Ein durchgehender Lauf der echten exe (Stand master f1e019ca = Code von v0.8.24), Endspiel-Zustand per Messhaken
`RE15_SET_FLAG=9:71,3:94,3:187,3:121,3:129` ((9,71) 10F0-Szene gesehen, (3,94) erste Irons-Szene, (3,0xBB) Ada
gerettet, (3,121) Rolltor offen, (3,129) Feuer geloescht), (9,73)=0 (Irons-Todesszene NICHT gesehen),
`RE15_DEBUG_JUMP=10f0@250`, Leon westlich des Tritts (-7125,4950) Blick +X, EIN Eingabeskript auf der durchgehenden
Bild-Zeitachse (Skript und Auszug: `F_belege/r7_e2e_vorher.txt`):

| Bild (T) | Raum | Ereignis (RE15_CLIMB_TRACE / debug.log) |
|---|---|---|
| T1119 | 10F0 | Tritt verschoben, Aktionstaste: `sub=9 kind=0x81` (Klettern hinauf, FUN_8002d474 @0x8002d614/18) |
| T1150 | 10F0 | oben: `pos=(-4901,-1800,-1256) band=1 stand=3` (auf dem Tritt) |
| T1225 | 1090 | `DOOR FIRE slot=1 ... spawn=(-13600,-9000,1300)` = Lueftungsgitter @0x00F52 -> Laufsteg, Band 5 |
| T1488 | 1090 | am Nordrand des Stegs (-9900,3358) Blick +Z, Aktionstaste: `sub=9 kind=0x0E` = **ABSPRUNG** |
| T1503 | 1090 | gelandet auf der Kiste obj 0: `pos=(-9900,-8410 .. -3600,4268) band=2 stand=0` |
| T1619 | 1090 | von der Kiste nach +X: `sub=9 kind=0x02` (Absprung vom Objekt) |
| T1658 | 1090 | Hof: `pos=(-8434,-1800,4262) band=1` |
| T1703 | 1050 | `DOOR FIRE slot=0 rect=(-10080,3060,...) spawn=(19850,0,-22300)` = 1090 main00 @0x0211A -> 1050-Sued |
| T2075 | 10A0 | `DOOR FIRE slot=4 rect=(17200,-13700,...) spawn=(26200,-14400,24800)` = **die Tuer zum Treppenhaus B1 ist offen** |

Kein "I have to get the Chief first..." auf dem ganzen Weg; aus dem Treppenhaus 10A0 geht es weiter nach B1
(1180 -> 11B0 -> 11C0). Das ist die Luecke des Nutzers, Ende-zu-Ende.

Gegenproben (je eigener Lauf): r2 — auf dem Laufsteg (Band 5) INNERHALB des Tuerrechtecks der 1050-Tuer
(x -11500 .. -10375, z 2700) Aktionstaste: KEIN `DOOR FIRE` (Band-Gate @0x80042cac greift, Tuer-Band 1 != 5);
r3/r4 — der Absprung allein (Sprung nach 1090 auf den Steg) zeigt dieselbe Kette; r6 — ROOM1050-Sued vom
1090-Eintritt (19850,-22300) bis zur Tuer Slot 4: Westflur bis x 16568, Nordflur bis z -13705, Blick +X,
`DOOR FIRE slot=4` -> ROOM10A0.

### RE-Belege (Original, selbst gelesen/disassembliert)

* **Lueftungsgitter** ROOM10F0 main00 @0x00F52 (Datei-Offset):
  `3b 01 02 31 01 00 1a eb 36 f7 b0 04 e8 03 e0 ca d8 dc 14 05 00 0c 00 09 00 05 ...`
  = Door_aot_set Slot 1, sce 2, Band **1**, Rechteck (-5350,-2250,1200,1000), Ziel Stage-Byte 0x00 / Raum 0x09 =
  ROOM1090, Spawn (-13600,**-9000**,1300) Gierung 3072, Ziel-Band 5. Der Tritt (obj 0 Typ 4, sub00 @0x010F4/@0x0111A)
  bringt Leon auf Band 1 (A_klettern.md).
* **Absprung-Zelle** ROOM1090.RDT SCA @0x01E3C (Kopf RDT+0x20 -> SCA @0x1DD0, Eintrag 12 B):
  `6e 0e cf 10 18 d2 f2 0e 01 01 0e 53` = Breite 3694, Tiefe 4303, Ecke (-11752,3826), Typ 1, u0 0x01, u1 0x0E,
  floor 0x53 -> Typwort w = u1 | floor<<8 = **0x530E**: Band 5 (w>>12), **Bit 1 = Absprung**, Klasse w & 0x0E = 0x0E.
  Dieselbe Klasse in ROOM1090 noch @0x01E18/@0x01ED8/@0x01F74/@0x02064 (x -4100..3100, z -3600..7896).
* **Kletter-Sonde, Zweig ABWAERTS** PSX.EXE FUN_8002d474 (`re15_disasm.py dis 0x8002d734 28`):

  ```
  8002d74c: andi v0,s6,0x2          ; Typwort-Bit 1 (Absprung)?
  8002d750: beq  v0,zero,0x8002d7a8 ; nein -> kein Absprung
  8002d760: lbu  a2,-13610(a2)      ; DAT_800acad6 = Spieler-Band
  8002d764: jal  0x8001c6e8         ; Bodenhoehe unter dem Sondenpunkt (a1=1, a3=0x100)
  8002d778: slt  v1,v1,v0           ; Spieler-y < Boden?
  8002d780: andi v0,s6,0xe          ; Klasse
  8002d788: sb   v0,-13734(at)      ; 0x800aca5a = kind
  8002d78c: ori  v1,zero,0x9
  8002d794: sb   v1,-13735(at)      ; 0x800aca59 = Substate 9
  ```

  Der Port bildet das nach (climb_common.c re15_climb_try_start). Der Absprung ist Original-Verhalten mit
  Original-Daten: die Luecke entsteht dadurch, dass die Port-Sperre 1060 -> 1040 nur EINE von mehreren Bruecken nach
  unten sperrt.
* **Tuer 1090 -> 1050** ROOM1090 main00 @0x0211A `3b 00 02 31 01 00 72 cf d4 08 5c 12 40 06 8a 4d 00 00 e4 a8 00 0c
  00 05 06 00 ...` = Slot 0, Band 1, Rechteck (-12430,2260,4700,1600), Ziel Raum 0x05 = ROOM1050, Spawn
  (19850,0,-22300), Cut 6 (unbedingt in main00).
* **Tuer 1050 -> 10A0** ROOM1050 main00 @0x00B5A `3b 04 02 31 00 00 3c 41 94 c6 e8 03 d0 07 58 66 c0 c7 e0 60 00 08
  00 0a 00 08 ...` = Slot 4, Band 0, Rechteck (16700,-14700,1000,2000), Ziel Raum 0x0A = ROOM10A0 (unbedingt in
  main00; der Port widmet den Slot per re15_adaruf.h nur bis (3,0xBB)=1 um).
* **Tuer 1060 -> 1040** ROOM1060 main00 @0x00D52 `3b 02 02 31 00 00 e8 67 ec 5e e8 03 98 08 f0 ad 00 00 b2 cc 40 08
  00 04 05 00 ...` (die bestehende Port-Sperre, re15_tuer1060.h).
* **Text-Platz sce 1** = Typtabelle @0x8007469c[1] = 0x80043084 (`dis 0x80043084 12`):

  ```
  80043098: lhu a3,2(v0)    ; Pausemaske
  8004309c: lhu a2,0(v0)    ; Nachricht
  800430a0: jal 0x80027e68  ; Nachricht oeffnen — kein Raumwechsel, keine Tuersequenz
  ```

* **Aot_reset** LAB_80040738 (`dis 0x80040738 32`): `sb v0,0(a1)` @0x80040764 rec[0] = pc[2] (sce);
  `andi v1,v1,0x80` / `addu v0,v1,v0` / `sb v0,1(a1)` @0x8004076c-78 rec[1] = (rec[1]&0x80) + pc[3]; Nutzlast u16
  pc[4..9] nach rec+12 (bzw. rec+20 bei Viereck) @0x80040788-a8; rec[2] (Band) wird NICHT geschrieben. Genau diese
  Form setzt der Port mit re15_aot_retype (wie tuer1060_1040.c).

### Wege-Zensus: alle Wege vom Endspiel-Ort zum Ende von Stage 1 (11B0/11C0)

`F_belege/wege_zensus.py` ueber ALLE Door_aot_set-Saetze (main + subs, sce != 0) der Leon-Raeume ROOM1xx0
(`F_belege/tuer_zensus_stage1_leon.txt`, 134 Saetze). Schlimmster Fall: JEDE flag-gesperrte Tuer gilt als offen
(Fahrstuhl 1080 mit Strom, alle Kartenleser, Rolltor), jeder Raum gilt innen als ganz begehbar (Absprung, Leitern) —
die Rechnung ueberschaetzt also die Erreichbarkeit. Inerte Saetze (sce 0, nie scharf: 1170 Slot 1 -> 10B0 @0x01226,
1250 -> 1000) zaehlen nicht. Ergebnis (`F_belege/wege_zensus.txt`):

| Sperren | 11B0 / 11C0 von ROOM10F0 aus |
|---|---|
| keine (Original) | erreichbar: 10F0 -> 1090 -> 1050 -> 10A0 -> 1180 -> 11B0 -> 11C0 |
| nur 1060 -> 1040 (v0.8.24) | **erreichbar: dieselbe Kette** (= die Luecke) |
| 1060 -> 1040 + **1050 -> 10A0** | **NICHT erreichbar** |

Mit beiden Sperren erreichbar sind genau 1000 1010 1020 1030 1040 1050 1060 1070 1080 1090 10C0 10D0 10E0 10F0
1100 1110 1120 1130 1140 1150 1170 1240 (1F/2F/3F, Dach, Fahrstuhl). Der EINZIGE Schnitt in den Rest (10A0 10B0 1160
1180 1190 11A0 11B0 11C0 11D0 11E0 11F0 1200 1210 1220 1230 1260 2000) ist **1050 -> 10A0 (@0x00B5A Slot 4 Band 0)**.
Damit schliesst die neue Sperre jeden Weg zum Ende — die Luke (Absprung 1090), den Fahrstuhl 1080 -> 1040 (Strom
(4,0xF3) aus 11F0) und den Weg ueber 1060, falls er je frei waere. Raumwechsel ausserhalb der Tueren:
re15_player_continue_reload (laedt den AKTUELLEN Raum neu, re15_damage.c:359) und die Port-Montage nach der
Irons-Todesszene (laeuft erst NACH (9,73)=1). Das Debug-Menue (SELECT, Original-Werkzeug @0x80014444) ist kein Spielweg.

### Bedingung (PORT-WAHL, begruendet)

`gesperrt <=> Raum 0x1050 UND (3,0xBB)=1 UND re15_tuer1060_sperre_aktiv()`,
`re15_tuer1060_sperre_aktiv() = (9,73)=0 UND ((9,71)=1 ODER (3,94)=0)` — dieselbe Funktion wie an der 1060-Tuer.

* "zusaetzlich" + "denselben Text": EINE Bedingung fuer beide Tueren; die beiden Sperren koennen nie
  auseinanderlaufen (die 1060-Sperre hat der Nutzer in Runde 35 abgenommen).
* **(3,0xBB)=0 (Fruehspiel): nichts** — dort gilt allein die Ada-Logik (re15_adaruf.h: Szene Ereignis 13 bzw.
  Sperrtext msg 25); der Installer laeuft NACH re15_adaruf_install und fasst Slot 4 nur an, wenn adaruf ihn als
  Original-Tuer gelassen hat ((3,0xBB)=1 -> adaruf kehrt sofort zurueck, adaruf_1050.c:217).
* Mittelspiel (Ada gerettet, 10F0-Szene noch nicht): (9,71)=0, (3,94)=1 -> frei (Keller-Wege fuer Zange,
  Minidisc, Generator bleiben offen).
* Endspiel ((9,71)=1, (9,73)=0) -> Text "I have to get the Chief first..."; nach der Irons-Todesszene ((9,73)=1,
  erstes Opcode der Szene) -> frei.
* Der Teil "(3,94)=0" kann mit (3,0xBB)=1 im normalen Spiel nicht vorkommen: der Suedteil von 1050 (Ada, Feuer,
  10A0) ist vor der Rettung nur durch das Rolltor erreichbar (A_rolltor.md), die Sicherung liegt in ROOM1150 (Irons'
  Buero, erste Szene (3,94) beim Betreten), der Fahrstuhl braucht Strom aus 11F0 und das Gitter 10F0 die Blue
  Keycard aus 1110 — beides nur ueber den Keller. Er bleibt drin, damit 1050 nie offen ist, wenn 1060 zu ist.
* Elza (ROOM1051) bleibt offen: eigene Raum-Id; Elzas Spiel kennt weder die 10F0-Szene noch die Irons-Szenen
  (re15_tuer1060.h).
