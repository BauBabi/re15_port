# Spur AN: unabhaengige Abnahme 3 (Runde 41)

Gepruefter Stand: Arbeitsbaum `.claude/worktrees/r41_annette`, Zweig `r41/annette`, Kopf 682b1ad96 (Code b646c72a6). Der Baum war
sauber. Bau: `bash re15_port/tools/local_build.sh build` ergab `LOCAL-BUILD-OK` und "ninja: no work to do". Die exe stammt von
07:30, die letzte Quellaenderung (annette_r41.c) von 07:23. Ich bin nicht der Autor. Alle Werte unten stammen aus eigenen Laeufen:
Fensterlaeufe mit eigener exe-Kopie `re15_pc_abn3_*` und dem Renderer des Nutzers, ohne AUTOSHOT und ohne SOFTWARE_RENDER. Die
Bilder kommen aus `RE15_FRAMEDUMP`, der Ton aus `RE15_AUDIO_CAP_SYNC` (1470 Stereo-Samples je Spielbild). Dazu kommen eigene
Disassemblies, eigene RDT- und RAM-Lesungen und zwei eigene Sonden gegen `libre15_engine.a`. Werkzeug: `AN_belege/abnahme3/werkzeug/`.

N1 der Abnahme 2 (Riesenform kommt im Wagenweg mit HP 0) ist in r41/form behoben (954f73d1c), in diesem Zweig aber noch nicht
enthalten. Er steht hier als bekannt und zaehlt nicht als Mangel dieser Spur.

## 0. Urteil

**BESTANDEN**, mit zwei leichten Nachtraegen (A1, A2) und einer Entscheidung fuer den Nutzer (E1).

N2 bis N4 der Abnahme 2 sind behoben, und das haelt im eigenen Fensterlauf. Die Begruendung zu N5 traegt. Die neue Typpruefung im
Waffen-Resolver entspricht dem Original Byte fuer Byte. Sie aendert aber fuer alle Spuren sichtbar das Verhalten bei Begleitern:
ein Begleiter im Schusskeil schluckt jetzt jeden Schuss, vorher nur den ersten. Kein Test deckt das ab, und RE2 macht es anders
(E1, Abschnitt 3). Ablauf und Reihenfolge sind in Ordnung, alle Tests sind gruen.

## 1. Maengel und Hinweise

| # | Schwere | Was | Beleg |
|---|---|---|---|
| A1 | leicht (Beleg) | **Die Adresse fuer "Plc_motion setzt Bild 0" ist falsch.** Zitiert ist "RE2 0x80052ef0 `a060014d sb zero,0x14d`" (annette_r41.c:129, Dossier N4.2). In beiden RE2-Binaries im Repo (`info/re2leon/PSX.EXE`, `SLUS_007.48`) steht an 0x80052ef0 das Wort `0x00431023` = `subu v0,v0,v1`; das zeigt auch `ghidra_re2_Leon.txt:171650`. Der Plc_motion-Handler beginnt bei 0x80056b8c (`lbu a1,2(v0)` = Clip-Byte), und `sb zero,333(v1)` steht @0x80056ba0. Ein Claire-Binary liegt nicht im Repo. Der Mechanismus stimmt also, die Adresse nicht. | `re2_disasm.py dis 0x80056b60 24`; Wortsuche `a060014d` -> nur 0x80056ba0 |
| A2 | leicht (Testluecke) | **Kein Integrations- oder Spieltest prueft "Begleiter im Schusskeil".** Nur `unit_r41_annette` test_schuss (b) prueft das, mit der 0x4D in 5090. Die NPC-Integrationstests (Roy 1021, 11C0, 11B0, Irons 1150) schiessen nicht. | Abschnitt 3.4 |
| E1 | Entscheidung Nutzer, kein Mangel der Spur | **Ein Begleiter vor dem Gegner schluckt jetzt jeden Schuss** (RE1.5 @0x80012178-84, wie im Original). Vorher traf der erste Schuss den Begleiter: hp -1 -> -6, Zustand 3 = Tod. Danach gingen die Schuesse durch. RE2 laesst die Schuesse durch (hp < 0 = kein Kandidat). Das betrifft 29 Raeume, deren Skript NPCs und Gegner setzt, darunter 2000 (Ada, Marvin, zwei 0x16 unter derselben Bedingung) und 3080 (Ada, Marvin, Birkin 0x36). Nach der Projektregel gilt bei einem vollstaendigen RE1.5-System RE1.5. Weil RE2 hier vollstaendig anders arbeitet, gehoert der Punkt in die Release Notes unter "Zu entscheiden". | Abschnitt 3 |
| H1 | Hinweis (Beleg) | Die RE2-Regel "hp < 0 ist kein Kandidat" ist mit @0x80047148/50 zitiert. Das ist FUN_800470C0, nach dem eigenen Kommentar in re15_damage.c der Messer-/Bolzen-Applier. Fuer die Pistole gilt FUN_800410CC mit derselben Abfrage: `lh v0,342(s2)` @0x80041280, `bltz v0,0x80041300` @0x80041288. Die Regel stimmt, die passendere Adresse fehlt. | `re2_disasm.py dis 0x80041230 60` |
| H2 | Hinweis | Die Leiche schiebt Leon wie ein NPC (Kasten 450 an der Wurzel). Im Lauf `leiche_west` laeuft Leon bei z -23550 vorbei und wird bei x ~4800 um 100 nach Sueden versetzt (-23550 -> -23650). Das faellt kaum auf. Gegen RE2 6141 habe ich das nicht geprueft. | state.log `leiche_west` F384..F399 |
| H3 | Hinweis | N5 untersucht nur den Satz (msg 40). Bei "William." (msg 43, Cut 12) steht sie weiter klein und weit hinten. Der Nutzer hat "Annette rechts" gedeckt, deshalb nur ein Hinweis. | `szene5090_abn3.jpg` F1270 |
| H4 | Hinweis | Das Dossier nennt "ab F1504 Annette rechts an der Tuer, Leon frei". Im Bild F1504 verdeckt sie noch seinen linken Unterschenkel, frei ist er ab F1508. Sie geht vor ihm durch das Bild, kommt ihm aber nie naeher als 1229 (state.log). | `zugstart6030_abgang_abn3.jpg` |
| N1 | bekannt (Form-Spur, 954f73d1c) | Wagen C: Form HP 0 (F946), Rueckstoss beim ersten Treffer F1915 | state.log `kampf` |
| M3 | wie Abnahme 1/2 | Stimmen fehlen: `[voice] ROOM6030 msg 27..30: keine Aufnahme` | debug.log `z6030` |

## 2. N2 bis N5, eigene Messung

### 2.1 Laeufe (alle aus dem echten Fenster, `werkzeug/abn3_alle.sh`)

| Lauf | Weg | Zweck |
|---|---|---|
| `z6030` | 6000-Zugtuer mit Torstand wie Abnahme 2 ((3,48)(3,49)(3,100)(3,239)(4,23)(3,89)(3,22)), Eingabe `eingabe_w8_nb4` des Autors; Bilder F1340..F1700 einzeln, Ton | N4 |
| `s5090` | dieselbe Kette bis 5090; Bilder F1150..F1960 einzeln, Ton | Szene, Leiche ab dem Schnitt |
| `kampf` | dieselbe Kette mit Pistole und Feuer (`W21,MA4` x 85); Bilder F1700..F5400 jedes 10., Ton | N2/N3 im Kampf, Ablauf bis G5 |
| `tor` | Debug-Sprung an den Torschalter, Aktion, "Yes", `RE15_FLAG_CENSUS` | Tor |
| `leiche`, `leiche_west` | 5090 mit (3,89),(9,133),(3,42), Wagen B -> C; `leiche_west` laeuft bis ans Westende | Wiedereintritt, Lage, Durchgang |

### 2.2 N2: Leiche liegt flach (RE2-Clip 0x18)

- **RE2-Bytes selbst gelesen** (`info/re2clair/PL1/RDT/ROOM6141.RDT`):
  - Pos_set @0x1162 `32 00 f4 cf 84 03 3c c4` = (-12300, 900, -15300)
  - Dir_set @0x116A `33 00 00 00 e4 0c` = 3300
  - Plc_motion @0x1172 `3f 00 18 04` = Clip 0x18
  - Sce_em_set @0xD16 `44 00 00 49 00 09 00 00 00 ff f4 cf 84 03 3c c4 e4 0c 18 01 40 00`
  - RDT+0x60 zeigt auf @0x2E70, dort `b8 79 00 00 01 00 00 00`
  - Alle Bytes stimmen mit dem Dossier.
- **Fenster (`kampf`):** Ab F1674 (Schnitt weg) steht Slot 1 t=4d bis F5400 unveraendert:
  - st 0, Clip 24 Bild 0, @(4856,-22748) r1170, hp -1
  - Das sind 3726 Bilder ohne Zustands- oder Lagewechsel (Werkzeug `zustand.py` der Abnahme 2).
- **Flach und ohne Wanddurchdringung (Sonde `leiche_wand.c`):**
  - Gerippe aus `ANNETTE_4D.EMD`, Clip 24 Bild 0, an der Fensterlage.
  - Alle 15 Gelenke liegen auf y -24..-425, also flach am Boden.
  - Der kleinste Abstand eines Gelenks zu einer SCA-Zelle des Bandes 0 betraegt 410: Gelenk 9 bei z -22110 gegen das Regal #28
    bei z -21700. Das Suedregal #29 ist mindestens 849 entfernt.
  - Das passt zum Netzpunkt-Abstand 43 des Autors: Gelenke liegen innen, das Netz reicht um den Kopfradius weiter.
- **Bild (`leiche_abn3.jpg`, `kampf_abn3.jpg`):**
  - Im offenen Cut 11 (F2060) liegt sie als flacher Streifen am unteren Rand, Gesicht oben.
  - In Cut 12 (Wiedereintritt F600) liegt sie flach neben dem Gang.
  - Waehrend Kampfstart, Rueckstoss, Sturz und G5 (F1779..F2300 Cut 11, F2380..F3050 Cut 12) liegt sie nicht im Band zwischen
    den Kinobalken und stoert nicht.
  - Das sitzende Bild aus Abnahme 2 ist weg.
- **N2 behoben.** Offen bleibt der Beleg A1.

### 2.3 N3: Leiche nimmt keinen Schaden

- **Fenster (`kampf`):** Der Kampf lief mit 144 Pistolenschuessen (mg-Abnahme im state.log, F1915..F5061).
  - Riesenform: N1, HP 0 -> Rueckstoss F1915 (hp 1), Sturz F2005.
  - G5: F2106 mit HP 600, dann 40 Treffer im 22er-Takt von F2377 bis F3235 (HP -1). Leon hat durchgehend hp 100.
  - Zwischen F2377 und F3235 gibt es keinen Schuss ohne HP-Wechsel. Die Leiche lag dabei seitlich 802 neben der Schusslinie und
    naeher als die G5. In Abnahme 2 traf genau diese Lage die Leiche (F2751); jetzt nimmt bei F2751 die G5 den Treffer (345 -> 330).
  - Die 103 Schuesse ohne HP-Wechsel liegen alle vor dem ersten G5-Treffer (F1937..F2355: Rueckstoss, Sturz, G5-Auftritt) oder
    nach dem G5-Tod (ab F3257).
- **Code:** `re15_annette_r41_kein_ziel` greift nur fuer Annettes Slot und nur bei (Treffer oder Phase RUHE) und hp < 0.
  `re15_annette_r41_install` setzt den Zustand bei jedem Raumwechsel zurueck (frau = -1), ein Slot aus 5090 wirkt also nicht in
  anderen Raeumen nach.
- **N3 behoben.** Hinweis H1 zur zitierten RE2-Stelle.

### 2.4 N4: Abgang in 6030 mit Tuerton, keine Tuerbewegung

- **Ablauf (`z6030`, jedes Bild):**

  | Bild | Was |
  |---|---|
  | F1460 | Cut 0 |
  | F1460 | Plc_dest Leon Modus 4 nach (-3000,-23700) |
  | F1464..F1480 | Leon tritt zur Seite, Endlage (-3080,-23674) |
  | F1482..F1510 | Annette geht von (-1790,-22272) zum Tuerziel (-3820,-22540) |
  | F1511 | `Se_on: bank=14 id=0`, Cut_chg(1), sie wird versteckt |
  | F1541 | `Se_on: bank=14 id=1` |
  | F1555 | Steuerung frei |

  - Das Log meldet `Tuerbank DOOR2A geladen`.
  - Kein Bild zeigt ein Verschwinden innerhalb eines Schnitts: F1511 ist noch Cut 0 mit ihr an der Tuer, F1512 ist Cut 1 ohne
    sie. Zu H4 siehe Tabelle 1.
- **Ton, gemessen gegen den Abnahme-2-Lauf ohne Tuerton** (`werkzeug/ton_diff.py`, `ton_diff_6030.txt`):
  - Bis F1465 sind beide Mitschnitte bitgleich. Die erste Abweichung liegt bei F1466, dem ersten Schritt von Leons Seitschritt.
  - Bei F1512 steigt der Pegel auf 1717, der Vergleichslauf liegt bei 1053 (Differenz 1478). Die Differenz bleibt bis F1540 bei
    ~600 und steigt bei F1542 erneut auf 1926 gegen 1116 (Differenz 1666).
  - Die beiden Tuertoene setzen also jeweils ein Bild nach ihrem Se_on ein und liegen 30 Bilder auseinander. Leon steht ab F1500
    still, Schritte kommen als Ursache nicht in Frage.
  - Ausrichtung: Tick = state.log-Zeile + 349, bestimmt am Einsatz von Ton 3 "Zug an" (F800).
  - Video: `zugstart6030_abgang_abn3_mit_ton.mp4` (F1340..F1640).
- **RE2-Muster selbst gelesen** (`info/re2clair/PL1/RDT/ROOM4081.RDT`):
  - @0x3634 `36 00 00 01 03 ..` (oeffnen)
  - @0x3640 `09 0a 1e 00` (Sleep 30)
  - @0x3644 `36 00 01 01 03 ..` (zufallen)
  - @0x364C `32 00 20 4e 20 4e 20 4e` (Pos_set weg)
  - Die Tuertabelle @0x8009A718 in `info/re2leon/PSX.EXE` lautet `28 40 54 87 09 00 00 00 62 3f 00 00`.
  - DOOR2A.DO2 hat 53076 B = 9 x 0x800 + 0x8754.
- **Begruendung "keine Tuerbewegung" geprueft:**
  - Der Kopf von ROOM6030.RDT lautet `00 0e 02 ..`, also nOmodel = 2.
  - Obj_model_set @0x0CEE: x `78 ba` = -17800, z `d4 9a` = -25900.
  - Obj_model_set @0x10A2: -26230 / -20148.
  - Beide Objekte liegen weit weg von der Fuehrerstandstuer (Tuer-AOT im Lauf: Rechteck (-4650,-23350) hw 450 hh 850). Die Tuer
    ist Hintergrundbild.
  - Eine Tuerbewegung gibt es nur in der DO2-Sequenz. Leon geht danach selbst durch diese Tuer: `[tuer] Sequenz Archiv 2 DOOR2A`
    spielt dieselbe Bank.
  - Die Begruendung haelt. Eine Tuerbewegung waere erfunden.
- **N4 behoben.**

### 2.5 N5: Begruendung

- Kameras von ROOM5090 selbst aus dem RID gelesen (RDT+0x24 = 0x60, 0x20 je Eintrag):
  - Cut 11 Auge (7626,-3256,-25360) Ziel (-1504,-986,-21970)
  - Cut 12 (18526,-2656,-24660)
  - Cut 13 (11050,-3110,-24812)
  - Cut 14 (23870,-7450,-29130)
  - Cut 15 (46,-1236,-24302)
  - Das sind genau die Werte im Dossier N4.5.
- Bei den Lagen der Szene (Annette S, Form L) zeigt nur Cut 15 Annette von vorn, dort verdeckt aber der Leib der Form das Bild.
  Cut 11 zeigt ihren Ruecken. Das passt zum ID-Puffer-Bild `n5_satz_kameras.png`.
- Die Suche (suche5) habe ich nicht neu gerechnet. Ihre Folgerung ist aber geometrisch schluessig: Die Treffer liegen am anderen
  Ende des Wagens und wuerden die Szene umdrehen.
- **N5: Begruendung traegt** (Hinweis H3 fuer "William.").

## 3. Nebenwirkung: Typpruefung im Waffen-Resolver fuer alle NPCs

### 3.1 Original, selbst disassembliert (`info/Re1.5/PSX.EXE`, FUN_80011f50)

```
80011fec..80012038  Liste aller Entities mit Wort 0 Bit 0 (0x800acc2c, Schritt 0x1f4, bis 20)
800120d0: lw v0,0(s0) / lw v1,player / and / lui v1,0xe000 / and / beq -> naechster   ; Band
800120f4: lw v0,144(s0) / and s4(0x3000000) / beq v0,s4 -> naechster                  ; Ein-Treffer-Sperre
80012108: lw v0,0(s5) / jalr v0                     ; Tester @0x8006e548[Waffe]; Pistole = 0x80012574
80012120: addu s1,s0,zero                           ; naechster Kandidat
80012168: jal 0x8001b9b4 / bne v0,zero,0x80012540   ; Sichtlinie
80012178: lbu v0,8(s1)
80012180: sltiu v0,v0,0x40
80012184: beq v0,zero,0x80012540                    ; NPC (Typ >= 0x40) -> Rueckgabe 0, kein Treffer
```

Der Pistolen-Tester 0x80012574 liest den Kasten ohne Typabfrage: `lw v0,120(s6)` / `lhu s2,6(v0)` @0x800125b8-c0. Die Schleife hat
also keinen NPC-Filter, und erst die Typpruefung nach der Wahl laesst den Schuss ins Leere gehen.

RAM (eigene Lesung, `stage_saves/mzd_stage1_npc.sav`, sauberer Stand: @0x80026e4c = `03e00008`):

| Slot | Typ | Wort 0 | Kasten (+0x78) | Radius | hp |
|---|---|---|---|---|---|
| 0 | 0x40 | 0x40000001 (LEVEL) | 0x80121588 | 450 | -1 |
| 1 | 0x42 | 0x40000001 | 0x80121658 | 450 | -1 |
| 2 | 0x4B | 0x40000001 | 0x801218c8 | 300 | -1 |

**Im Original schluckt ein NPC, das im Keil naeher steht als der Gegner, jeden Schuss.** Seine Sperre +0x93 wird nie gesetzt, weil
die Typpruefung vor `ori 0x1` steht, also bleibt es Kandidat. Der Port nach NB4 tut dasselbe.

RE2 (`info/re2leon/PSX.EXE`) arbeitet anders. FUN_800410CC (Schusswaffen) verwirft hp < 0 schon in der Schleife: `lh v0,342(s2)`
@0x80041280 / `bltz` @0x80041288. RE2-NPCs tragen hp -1, also gehen Schuesse an ihnen vorbei.

### 3.2 Port vorher/nachher (Sonde `npc_schuss.c`)

Aufbau:
- Leon bei (0,0), Blick -x.
- NPC (hp -1, Port-Kasten) bei x -3000, Gegner bei x -6000.
- 5 Pistolenschuesse; nur beim Gegner wird Bit 0 von +0x93 zwischen den Schuessen geloescht.
- Gelinkt gegen `libre15_engine.a`, fuer "vor NB4" mit re15_damage.c aus b646c72a6^.

| Fall | nach NB4 (Kopf) | vor NB4 |
|---|---|---|
| NPC 0x40/0x42/0x45/0x47/0x49/0x4B/0x4D davor, Gegner G5 | 0 Gegner-Treffer, 5 ins Leere, NPC unveraendert (hp -1 st 0) | Schuss 1 trifft das NPC: hp -1 -> -6, **st 3 (Tod)**, +0x93 01; Schuss 2..5 treffen den Gegner |
| NPC 0x40 davor, Zombie 0x10 (hp 100) | Zombie bleibt 100 | Zombie 100 -> 36 (4 Treffer), NPC st 3 |
| NPC 0x40 seitlich 900 | geschluckt | 1 NPC + 4 Gegner |
| NPC 0x40 seitlich 2000 / hinter dem Gegner | 5 Gegner-Treffer | 5 Gegner-Treffer |
| NPC-Typen ohne Port-Kasten (0x41, 0x43, 0x44, 0x46, 0x48, 0x4A, 0x4C) | 5 Gegner-Treffer | 5 Gegner-Treffer |
| ohne NPC (Kontrolle) | 5 Gegner-Treffer | 5 Gegner-Treffer |

Ergebnisdateien: `werkzeug/npc_schuss_nach_nb4.txt`, `npc_schuss_vor_nb4.txt`.

### 3.3 Wo das im Spiel vorkommt (`werkzeug/npc_gegner_zensus.py`, Walker nach scd_vm.c)

29 RDTs setzen per Sce_em_set sowohl NPCs (>= 0x40) als auch Gegner (0x10..0x3F):

- 1011, 1021, 1090, 10B1, 10D0, 1141, 1170, 1171, 11C0, 1211
- 2000, 2001
- 3000, 3001, 3010, 3011, 3060, 3061, 3070, 3071, 3080, 30E0, 30E1
- 4000, 4001, 4031
- 5031, 5090, 50B0

Ob beide gleichzeitig im Raum sind, haengt an den Flags. Unter derselben Bedingung stehen zum Beispiel:
- ROOM2000 sub00 @0x017A0..0x017E0: Ada 0x42, Marvin 0x40 und zwei 0x16, alle unter Ck(4,244)=1.
- ROOM3080 sub00 @0x007AC..0x007D4: Ada, Marvin und Birkin 0x36, alle unter Ck(4,96)=0.

Alle NPC-Typen, die ein Skript setzt (0x40, 0x42, 0x47, 0x49, 0x4B, 0x4D), haben einen Port-Kasten. Die kastenlosen Typen aus
3.2 kommen in keinem Sce_em_set vor.

### 3.4 Bewertung

- **Dem Original entsprechend:** ja. FUN_80011f50 @0x80012178-84 mit den RAM-Werten aus 3.1.
- **Ungewollt verschluckt?** Ja, im Sinn einer sichtbaren Aenderung:
  - Steht ein Begleiter naeher im Keil (seitlich bis ~900) und zielt Leon eben, trifft kein Schuss mehr den Gegner dahinter.
  - Vorher "toetete" der erste Schuss den Begleiter in Zustand 3, und danach ging alles durch. Das war ein eigener, schwerer
    Fehler, den NB4 nebenbei behebt.
  - Zielt Leon nach oben oder unten, ist der Begleiter kein Kandidat (NPC-Band LEVEL), und der Schuss geht durch.
- **Regel:** Die Projektregel (Beta -> Retail) sagt: Hat RE1.5 ein vollstaendiges System, gilt RE1.5. Das ist hier der Fall.
  RE2 arbeitet aber vollstaendig anders. Das ist genau der Fall, in dem die Regel eine Nutzerfrage vorsieht, deshalb E1.
- **Tests:** Die Sonde und `unit_r41_annette` (b) zeigen das Verhalten. Kein Integrationstest prueft es (A2). Die gelaufenen
  NPC-Integrationstests sind gruen, schiessen aber nicht.

## 4. Gesamtablauf (eigene Laeufe)

| Schritt | Bild | Beleg |
|---|---|---|
| Tor: Frage, "Yes", das Skript setzt (3,89) und (3,22) selbst (`[flagcensus] room=6000 WRITE Set zone=3 idx=89/22`), Tor oeffnet sich (Cut 12), Cut 8 | 6000 F60..F410 | `tor_abn3.jpg` |
| Weg Torschalter -> Zugtuer | wie Abnahme 2 H2 per Sprung mit genau diesem Flagstand, nicht neu geprueft | |
| 6030 Zugstart | F704 Szene, F800 Ton 3, F1010 Ton 0, msg 27..30 (stumm, M3), F1460 Abgang, F1511/F1541 Tuertoene, F1555 Steuerung | `z6030` |
| durch die Fuehrerstandstuer (DOOR2A-Sequenz), Wagen, Tuer nach 5090 | | `s5090` |
| 5090 Szene | Krach F1168, "William." F1264..F1299, Cut 15 F1330, Sturz F1350 (Ton 1), gelandet F1365, Bruellen F1407 (Ton 2), Satz msg 40, Hieb F1598, Treffer F1640 (Blut 30 Aufrufe, Pool 96/96), Schnitt weg/Leiche F1674, "Leon: Annette!" msg 42, Kampfstart F1776 | `szene5090_abn3.jpg`; Bildnummern gleich wie Abnahme 2 |
| Kampf, Rueckstoss, G5 | Rueckstoss F1915 (N1), Sturz F2005, G5 F2106, G5 tot F3235, Leon hp 100 | `kampf_abn3.jpg` |
| Wiedereintritt nach der Szene | `Wiedereintritt nach der Szene: Leiche liegt`, Clip 24/0 bei (4856,900,-22748), keine Form, keine Szene | `leiche`, `leiche_west` |

Untertitel im Bild: "Annette: William" (F1270), "W-William! You're alive!" (F1500), "Leon: Annette!" (F1720). Alle Toene liegen
auf den protokollierten Bildern.

Die Faelle ohnezug, Elza 5091 und zug_wieder aus Abnahme 2 habe ich nicht wiederholt. NB4 aendert dort nichts (Liegepose,
kein_ziel, Abgang 6030, Resolver), und `integration_r41_annette` (5 Laeufe) ist gruen.

## 5. Tests (eigene Laeufe in diesem Arbeitsbaum)

| Test | Ergebnis |
|---|---|
| Suite `^unit_\|^prop_\|^probe_` ohne `releaseskript\|android\|r37_masken36_ausschluss\|paket\|titelversion`, `-j 4` | **1040/1040 gruen** (190 s), darin unit_r41_annette (+ `_se_bank/_toene/_lagen`); test_schuss: (a) Rueckgabe 3, G5 330, Leiche hp -1 st 0; (b) Rueckgabe 0, NPC hp -1 st 0, G5 345 |
| integration_r35_zgirl_5090 | gruen (147 s) |
| integration_r41_annette | gruen (248 s) |
| integration_r41_birkin_pistole | gruen (256 s) |
| Stichprobe NPC, nacheinander: integration_r36_szene1150 (Irons), integration_r37_roy_szene1021 (Roy, Zombies), integration_r37_szene11c0 (Ada/Marvin), integration_r38_werkstatt11b0 | **4/4 gruen** (116 s / 58 s / 122 s / 159 s); keiner davon schiesst auf einen Gegner hinter einem NPC (A2) |
| Sonden `npc_schuss` (vor/nach NB4), `leiche_wand` | Ergebnisse in Abschnitt 2.2 und 3.2 |

Die df-haengenden Skripttests sind laut Auftrag ausgeschlossen (Umgebungsproblem). Ein GUI-Test lief nie parallel zu einem
Fensterlauf.

## 6. Belege

`analysis/befunde_runde41/AN_belege/abnahme3/`:

| Datei | Inhalt |
|---|---|
| `zugstart6030_abgang_abn3.jpg`, `zugstart6030_abgang_abn3_mit_ton.mp4` | Abgang in 6030 F1460..F1556; Video F1340..F1640 mit bildgenauem Ton |
| `szene5090_abn3.jpg` | 20 Felder 5090, Krach bis Kampfstart |
| `kampf_abn3.jpg` | Kampf: Form, Rueckstoss, Sturz, G5, Ende |
| `leiche_abn3.jpg` | Leiche im Kampf (Cut 11 offen), beim Wiedereintritt (Cut 12, Cut 11) und im Sturzbild vor dem Schnitt |
| `tor_abn3.jpg` | Torschalter, Frage, Tor, Cut 8 |
| `werkzeug/` | `abn3_lauf.sh`, `abn3_alle.sh` (alle Fensterlaeufe), `eingabe_kette*.txt`, `ton_diff.py` (+ `ton_diff_6030.txt`), `npc_schuss.c` (+ Ergebnisse vor/nach NB4), `leiche_wand.c` (+ `leiche_wand.txt`), `npc_gegner_zensus.py` (+ `.txt`) |

Die Rohdaten liegen unter `re15_port/build/abn3/<lauf>/` und sind nicht versioniert. Ich habe keine Datei unter `pri/`, `synchro/`,
`Version_80%/` oder `info/` veraendert und keinen Code geaendert. Alle exe-Kopien `re15_pc_abn3_*` liefen unter `timeout` auf die
eigene PID und sind geloescht.
