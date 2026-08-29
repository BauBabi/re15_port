# Schlafender Content — game-weiter Sweep: Texte und Szenen, die da sind, aber nie aktiviert werden

Stand 2026-08-29. ⛔ Reiner Recon — kein Code geändert, kein Build. Nutzer-Frage: „Gibt es noch
andere Texte oder Cutscenes, die da sind, aber nicht aktiviert? Und wenn ja, wo? Kann man die
aktivieren?"

Alle Zitate sind Datei-Byte-Offsets in `re15_port/shared_assets/PSX/STAGE{1..6}/ROOM####.RDT`
(mit Roh-Bytes bei den Kernfunden). Dekodierung = Port-Zeichensatz
`re15_port/engine/src/msg_common.c:177-201`. Werkzeug: `scdtool.py` (Scratchpad; erweiterte Kopie
des 100%-Coverage-Walkers `re15_port/tools/aot_sce_census.py`) + neue Sweeps `dormant2.py` /
`gate_analysis.py` (Scratchpad). Sweep-Umfang: **alle 240 RDTs** (Messages, Message-Referenzen,
sub-Trigger, 2999 Flag-Schreib-Ops inkl. 26 AOT-sce4-Flag-Records), 0 Walker-Desyncs.

**Bereits beantwortete Fälle werden hier nur referenziert, nicht wiederholt:**
11C0/11B0/1050-Cutscenes (inkl. der Waisen msg06/07 „Give me a moment, I need a break" und der
1090-Aktivierungskette) → `cutscenes-11c0-11b0-1050.md`; 10F0 „kleine Tür" → `10f0-kleine-tuer.md`;
Timer-Bomb (Message-Pickup statt Item) → `timerbomb-verwendung.md`.

---

## 0. Kurzfassung

**JA — es gibt deutlich mehr schlafenden Content.** Drei Klassen:

1. **Waisen-Texte** (in der Message-Tabelle, aber von KEINEM Opcode angezeigt — weder
   `Message_on` 0x2B noch AOT-sce1-Install 0x2C noch AOT-sce1-Retype 0x46):
   **STAGE1: 61 Vorkommen in 18 RDTs = 35 einzigartige Zeilen** (davon 4 schon in der
   1050-Analyse dokumentiert). **STAGE2: NULL** — alle STAGE2-Texte sind verdrahtet.
   STAGE3-6: weitere ~40 (Appendix §6, inkl. einer kompletten unverdrahteten Sherry-Szene
   in 3060 und der T-Virus-Synthese-Maschine in 4090).
2. **Ungetriggerte sub-Prozeduren** (kein Evt_exec/Evt_chain/Gosub/AOT-sce3 auf sie; main00,
   sub00 und sub01 laufen implizit — scd_vm.c:564-596): STAGE1: 7, davon **eine echte
   verlorene Szene (ROOM1150 sub04)**; STAGE2: 1 echte (20A0/20A1 sub02, Ambient-Effektschleife);
   STAGE6: **6020/6021 sub02 = „Itembox is not available in this preview"** — eine komplette
   Mini-Szene samt exklusiver Message, die nirgends gezündet wird.
3. **Flag-gegatete Szenen ohne Setzer**: game-weit nur EIN Fall in STAGE1/2 — der zweite
   Auslöseort der 1190-Rolltor-Szene ist in Leons Szenario toter Code (Setzer nur in Elzas
   Intro 1241). Die Szene selbst läuft bei Leon über den ersten Zweig — kein Content-Verlust.

---

## 1. Methode (Belege)

- **Message-Referenz-Kanäle** (erschöpfend, belegt): `Message_on` (0x2B, id=pc[1]);
  `Aot_set` sce=1 → msg = u16@payload+0 (Original-Handler `LAB_80043084`,
  `aot_common.c:198` mit `@0x8004309c lhu a2,0(v0)`); `Aot_reset` sce=1 → msg = p0 = u16@pc+4
  (`aot_common.c:224-229`, Payload-Writes `@0x80040788-a8`). Türen (0x3B) tragen KEIN
  Message-/Key-Feld (Beleg: `10f0-kleine-tuer.md` §1, `re15_aot_door_params_t`).
- **sub-Trigger-Kanäle**: Evt_exec 0x04 / Evt_chain 0x03 (sub=pc[3]), Gosub 0x18 (pc[1]),
  `Aot_set` sce=3 → sub = u8@payload+3 (`@0x800430F0`/`@0x80043100`), `Aot_reset` sce=3 →
  sub = p1>>8. Implizit laufen main00 + sub00 (Init, `scd_vm.c:564-570`, Original
  `@0x8003efd8`) und sub01 (Per-Frame-Reseed, `scd_vm.c:583-596`, Original FUN_8003f038).
- **Flag-Setzer-Index**: alle `Set` (0x22) + alle AOT-sce4-FLAG-Records (group/bit/on,
  Handler `@0x80043120`) über 240 RDTs = 2999 Schreib-Ops. Engine-geschriebene Bänke von der
  „nie gesetzt"-Analyse ausgenommen: 5 (Zonen-Handshake), 7/8 (em-Kill-Bank Stage<3/≥3,
  `enemy_ai_common.c:12796-12800`, `@0x800420ec`), 9 (Item-taken), 12 (System/Yes-No).
- Szenario-Zuordnung: ROOMxxx**0** = Leon, ROOMxxx**1** = Elza (Konvention der Vorberichte).

Grenzen → §8.

---

## 2. STAGE1 — die Waisen-Texte im Detail

### 2.1 ⭐ ROOM1150/1151 (Briefing-Raum, Irons) — gestrichene Keycard-Übergabe + verlorene Szene sub04

**(a) msg11 @Datei 0x1657** (`ROOM1150.RDT`, Roh-Anfang `04 00 05 02 25 4e 4b 4a 4f 16 05 00 00
30 3d 47 41 …`):

> „Irons: Take this card... it will allow you to enter the communications room... now go..."

Der Irons-Dialog (sub08) zeigt msg04-10, **überspringt msg11** und fährt mit msg12 fort — Beleg
(`ROOM1150.RDT` 0x1236-0x1256): `2b 0a 00 00` (Message_on id=**10**) … `2b 0c 00 00`
(Message_on id=**12**); dazwischen nur Spieler-Motion + Sleep (`3f 00 0b 00 / 09 0a 3c 00 / 3f 00
0b 00 / 43 00 80 00 / 09 0a 3d 00`). **Die Karten-Übergabe durch Irons wurde aus der Szene
geschnitten** — im Auslieferungsstand findet man die Blue Keycard stattdessen in ROOM1110
(sce-9-Item Typ 0x38, `timerbomb-verwendung.md` §1b). Passt zur Memory-Notiz „z9 Keycard"
(STAGE1-Flag-Map).

**(b) sub04 @0x0F96-0x10B6 — komplette Szenen-Choreographie ohne jeden Trigger.**
1150 sub_refs (erschöpfend): sub2←sub00@0x0EB2, sub3←sub01@0x0ECE, sub5←sub00@0x0EB6,
sub6/7/8←main00-AOT-sce3 @0x0DA6/0x0DCE/0x0DEA. **Niemand ruft sub04** (in 1151 identisch
verwaist). Inhalt (Voll-Disasm `dis2_1150.txt`):

- Cutscene-Klammern `22 02 00 01 / 22 02 02 01` @0x0F96/0x0F9A, Kamera `Cut_chg 4` @0x0FB2;
- Objekt 0 (in main00 @0x0E00 bei y=-20324 UNTER der Welt geparkt installiert:
  Bytes `24 af 9c b0 cc bb` = x=-20700, y=-20324, z=-17460) wird auf **y=-305 an dieselbe
  XZ-Position gehoben** (`Pos_set r=0 x=-20700 y=-305 z=-17460` @0x0FB4, unter `Work_set kind=3
  idx=0`), dann mit Motor-Sounds (Se_on Bank2 ids 0x0a/0x0c/0x0d @0x0FA2/0x0FE2/0x1000/0x1022/
  0x1032/0x104C) über For-Schleifen 91/10/90/2/2 Frames auf Achse 1 gefahren, während Objekte 1+2
  (For 15×) mit ±10 vibrieren;
- am Ende wird Objekt 0 wieder unter die Welt geparkt (`Pos_set r=0 … y=-20224` @0x109E) und die
  Klammern gelöst (@0x10A6-0x10B2, `Cut_old`).

**Ein verstecktes, voll modelliertes Requisit fährt herunter, arbeitet hörbar und verschwindet
wieder** — im Briefing-Raum plausibel eine herabfahrende Leinwand/Anzeigetafel
(Deutung UNSICHER; die Mechanik ist byte-belegt). **Aktivierung: `RE15_DEBUG_JUMP=1150@…` +
`RE15_FORCE_EVENT=4`** — die Szene ist in sich geschlossen (setzt/löst ihre Klammern selbst,
Objekte existieren aus main00).

### 2.2 ROOM1190/1191 (Zwinger/Rolltor) — „driver"-Pickup ersetzt durch Red Keycard; toter Zweit-Trigger

**msg01 @0x2E39** (Roh: `04 02 30 44 41 4e 41 3a 4f 00 3d 00 40 4e 45 52 41 4e 00 44 41 4e 41 …`):

> „There's a driver here. Take it? \<YES/NO\>"

Von keinem Opcode gezeigt (msg00/02/03/04/05 sind alle verdrahtet — Referenzliste im JSON).
Ein Message-Pickup wie die Timer-Bomb — aber im Auslieferungsstand gibt die Rolltor-Szene sub10
stattdessen ein **echtes Item**: `Aot_reset slot=5 → sce=9` @0x27AA mit Payload
`37 00 01 00 88 00` = Item-Typ **0x37, Menge 1, taken-Bit 0x88**. DEBUG.BIN-Namensbank
(off-Tabelle @0x495c, Blob-Basis 0x4a28): id 0x37 @0x4D2E = **„Red Keycard"**. Die Namensbank
enthält keinerlei „Driver"-Item → der Schraubenzieher-Pickup ist Alt-Design, ersetzt durch die
Red Keycard. Aktivierung des Textes: nur Content-Patch.

**Toter Trigger-Zweig (einziger Fund des Flag-Gate-Sweeps §4):** 1190 sub01 hat ZWEI
Evt_exec-sub10-Zweige (Rolltor-/Hunde-Szene, beide hinter `Ck(4,0xF3)==1` = Strom an):

```
Zweig A @0x2468: Ck(3,0x6F)==0 && Zone 5:04==1 && 5:05==1 && 5:06==0 && 5:07==0
Zweig B @0x24A8: Ck(3,0x70)==1 && Zone 5:04==0 && 5:05==0 && 5:06==1 && 5:07==1
  (Bytes @0x2480: 06 00 32 00 21 03 70 01 06 00 28 00 21 05 04 00 06 00 1e 00 21 05 05 00
   06 00 14 00 21 05 06 01 06 00 0a 00 21 05 07 01 04 ff 18 0a)
```

Flag(3,0x70) wird game-weit **nur von ROOM1241 sub02 @0x055E** gesetzt (Elzas Start-Intro:
`22 03 6f 01 22 03 70 01` @0x055A — setzt auch 3:0x6F=1 und blockiert damit für Elza Zweig A).
Leons Intro 1240 sub02 setzt stattdessen 3:0x8B (`22 03 8b 01` @0x055A). **Ergebnis: bei Leon ist
Auslöseort B (andere Zonen-Kombination = anderer Standort im Raum) toter Code, bei Elza Ort A.**
Die Szene selbst ist in beiden Szenarien erreichbar — nur je einer der beiden Auslöseorte.
sub10 löscht 3:0x70 im Elza-Pfad selbst wieder (Else-Zweig @0x277E `Set(3,0x70)=0`).

### 2.3 ROOM1030/1031 (Vorplatz/Front Gate) — gestrichener Mülleimer-Fund „Green Medicine" u.a.

6 Waisen pro Variante; main00 installiert nur AOT-Slots für msg00-04/06/08/12-16 (Liste im JSON).
Ohne jeden Anzeige-Kanal:

| # | Datei (1030) | Text |
|---|---|---|
| 09 | 0x2ACE | "It's too dark to see if there's anything inside. Would you like to reach inside?" |
| 10 | 0x2B25 | "Found Green Medicine. Take it?" (Roh: `04 00 22 4b 51 4a 40 00 05 01 23 4e 41 41 4a 00 29 41 40 45 3f 45 4a 41 05 00 57 08 30 3d 47 41 00 45 50 1b 01 00`) |
| 11 | 0x2B4B | "Better not dig through other's trash..." |
| 05 | 0x29C5 | 'This emergency severity" chart gives more confusion than it prevents.' |
| 07 | 0x2A37 | "Seems like the crash broke the axle. Poor thing..." (1031 msg08 @0x2C94: "Seems like whoever rode this crashed it through the front gate. Wonder if they're ok...") |
| 17 | 0x2CBB | "There are some words carved here... I CAN'T ESCAPE!!! / The door is locked, I can't open it." |

msg09→10→11 ist ein kompletter gestrichener Such-Pickup (Mülleimer/dunkle Öffnung: hineingreifen →
Green Medicine finden → Moral-Kommentar) — dieselbe Message-Pickup-Klasse wie die Timer-Bomb, aber
hier ohne jede Skript-Logik. msg17 (die eingeritzte Verzweiflungs-Botschaft am Tor) und die
Motorrad-Texte (msg07/08 — Elzas verunfallte Maschine!) sind reine Kulissen-Examines ohne AOT.
**Aktivierung: nur Content-Patch** (AOT-Slot + für msg09/10 zusätzlich Skript-Logik/Item-Grant).

### 2.4 ROOM1070/1071 (Großraumbüro) — 10 von 21 Texten ohne Examine-Punkt

main00 installiert 12 sce-1-AOTs (msg 0/2/3/4/7/8/9/11/12/18/18/20). Verwaist (beide Varianten,
Offsets aus 1070): msg01 @0x1769 (Stühle-Scherz), **msg05 @0x18A1 / msg06 @0x18E7** (Dienstplan-
Paar: „Guess "L.S.K." lucked out this week." / „This really wasn't my week." — vermutlich als
szenario-abhängige Varianten desselben Objekts geschrieben, KEINE von beiden verdrahtet),
msg10 @0x1A18 („I almost miss filling out paperwork…"), msg13-17 @0x1AE0-0x1C01 (fünf
Schreibtisch-Beschreibungen), **msg19 @0x1C7E „A police officer. He's dead..."** (eine
Polizisten-Leiche, die es als Examine nie ins Spiel geschafft hat). Nur Content-Patch.

### 2.5 ROOM1120/1121 (Büro mit Telefon-Save) — 7 von 9 Texten verwaist

main00 hat exakt 2 Examine-AOTs: slot1→msg00 (Aufzug ohne Strom) @0x0C9E, slot3→msg06
(Telefon: „You can save your progress with this. Save is not available in this preview")
@0x0CF8 — plus 3 Türen. Ohne Kanal: msg01 @0x0E24 („A painting, titled: The Music Lesson""),
msg02 @0x0E4E, msg03 @0x0E6A (Fenster), msg04 @0x0E9B, msg05 @0x0ED5 („A strange Christmas toy."),
msg07 @0x0F51 („..."), **msg08 @0x0F58 (das Armdrück-Poster „…named Chief for a Day of the Raccoon
Police Department!")**. ⚠️ Korrektur zur 11C0-Analyse §4: das „Chief for a Day"-Poster steht zwar
in der Message-Tabelle, wird aber **nie angezeigt** (der dortige Sweep prüfte nur Text-Existenz).
Nur Content-Patch.

### 2.6 Kleinere Einzel-Waisen

| Raum | msg @Datei | Text | Befund |
|---|---|---|---|
| 1020 / 1021 | msg04 @0x20FB / msg13 @0x259C | "There is something saved here." | Kein Kanal. Gehört zur Preview-Save-Klasse (Branaghs Schreibtisch msg03 ist verdrahtet, der Savegame-Hinweis nicht — Deutung als Save-Praesenz-Text UNSICHER). |
| 1131 (nur Elza) | msg01 @0x0B42 | "It's not necessary to go back." | In **1130** (Leon) per Frame scharf (sub01-`Aot_reset(3,sce1,msg1)` — `aot_common.c:150-152`), in 1131 existiert der Retype nicht → Elza darf zurückgehen, der Text ist dort tot. Szenario-Divergenz, kein verlorener Inhalt. |
| 1140 | msg03 @0x0E0A | "The streets are infested with zombies!" | Byte-gleiches Duplikat von msg04 @0x0E36 (verdrahtet via main00-AOT slot4 @0x0AE8). Redundante Kopie, kein Verlust. |
| 1171 (nur Elza) | msg02 @0x1810 | "John: C'mon, give me a hand!" | sub02 zeigt msg01 @0x151E, dann `04 ff 18 03 04 ff 18 04 09 0a 96 00 2b 03 00 00` @0x157A (Evt_exec sub03+sub04, Sleep 150, Message_on **03**) — msg02 wird übersprungen; die Zeile wurde durch die stumme gemeinsame Schiebe-Choreographie (beide `Plc_dest mode=19` @0x1566/0x1572) ersetzt. Gleiche Klasse wie 1150-msg11. |
| 11F0 / 11F1 | msg03 @0x1909 | "Power supply incorrect." | Generator-Puzzle: sub16 zeigt msg00/01, sub18 msg02 — der alternative Fehlertext msg03 ist unverdrahtet (Alt-Logik der Strom-Sequenz). |
| 1050 | msg02/05/06/07 | (Fuse / Leiche / Ada-Break-Dialog) | **Schon dokumentiert** → `cutscenes-11c0-11b0-1050.md` §5. |

---

## 3. Ungetriggerte subs STAGE1/2 (ohne 1150 sub04, §2.1b)

| Raum | sub | Inhalt (Disasm) | Bewertung |
|---|---|---|---|
| 1021 | sub05 @0x21DE | `Do{Evt_next}Edwhile Ck(5,0x21)` — Warte-auf-Zonen-Bit-Helfer | Tote Kopie der Gosub-Helfer-Klasse (sub04 daneben IST referenziert). Kein Inhalt. |
| 10B1 | sub05 @0x1A92 | dito mit Ck(5,0x20) | dito (sub06 ist 19× referenziert). |
| 11B0 | sub02 @0x1238 | dito mit Ck(12,0x18) | dito. |
| 1030/1031 | sub10 @0x283A/0x28xx | For 20× `Sce_bgm_control(2,4)` + Member-Arithmetik = BGM-/Lautstärke-Rampe | Ungenutzter Audio-Helfer. |
| **20A0/20A1** | **sub02 @0x1BF0-0x1CB0** | Endlosschleife: 11× `Sce_espr_on` Effekt-ID 6, Scale 0x1388, Emitter bei x≈20800-21800/z≈-23200 bis -27000, Sleep 5, `Goto ->0x1BF0` | **Echte deaktivierte Ambient-Effektschleife** im Adult-Spider-Kanalraum (em Typ 0x25; Türen → 2090/11A0). Visuelle Identität (Tropfen/Partikel) UNSICHER — nicht gerendert geprüft. Aktivierung: `RE15_FORCE_EVENT=2` in 20A0. |

STAGE2 hat **keine** Waisen-Texte — alle Messages aller 32 STAGE2-RDTs sind referenziert.

---

## 4. Flag-Gate-Sweep (Szenen, deren Zünder nie gesetzt wird)

`gate_analysis.py`: alle If-`Ck(bank,flag)==1`-Bedingungen direkt vor Evt_exec/Evt_chain/Gosub in
STAGE1+2 gegen den game-weiten Setzer-Index (2999 Schreib-Ops; Engine-Bänke 5/7/8/9/12
ausgenommen). **Ergebnis: exakt EIN Fund** — der 1190-Zweig B (§2.2), Klasse
„Setzer nur im anderen Szenario". Keine einzige STAGE1/2-Szene hängt an einem Flag, das
nirgends gesetzt wird. (Die 1050-Kette aus der Vorarbeit bestätigt das Muster: ihr Gate 3:0x6E
hat einen realen Setzer in 1090.)

---

## 5. Aktivierbarkeit — Zusammenfassung je Klasse

| Klasse | Aktivierung |
|---|---|
| Verlorene Szene **1150/1151 sub04** | **Sofort im Port**: `RE15_DEBUG_JUMP=1150@…` + `RE15_FORCE_EVENT=4` (`platform/pc/main.c:5321-5346`, feuert `sub_scd[4]` nach Raum-Setup). Selbstgekapselt. |
| Preview-Stub-Szene **6020/6021 sub02** („Itembox") | **Sofort im Port**: Raum 6020 + `RE15_FORCE_EVENT=2` (Cut 8 → Message → Cut zurück, @0x0BC4-0x0BD0). |
| Effektschleife **20A0 sub02** | **Sofort im Port**: `RE15_FORCE_EVENT=2` (läuft endlos — nur beobachten, nicht als Dauerzustand). |
| 1190 Zweig B (Leon) | Flag(3,0x70) setzen (kein Port-Env-Setter vorhanden → Savestate-/Debug-Patch); ohne Verlust, Szene läuft ohnehin über Zweig A. |
| **Alle 35 Waisen-Texte** | **Nur per Content-Patch** — es existiert kein Opcode, der sie zeigt (kein `Message_on`, kein sce-1-AOT). Minimal-invasiv wäre je ein `Aot_set sce=1` (20 B) im main00 bzw. bei Dialog-Zeilen (1150 msg11, 1171 msg02) ein `2b 0b 00 00`/`2b 02 00 00` an der Sprungstelle — Design-Entscheidung des Nutzers, kein byte-true-Fix. |

---

## 6. Appendix — Waisen ausserhalb STAGE1/2 (nur gelistet, nicht vertieft)

Bemerkenswerteste Funde:

- **3060 msg01-08 @0x1117-0x126A: die komplette Sherry-Fluchtszene als Textgeister** („Elza:
  Sherry, come back!" … „Sherry: I'll unlock the back door from the other side!" … „John: H-hey,
  you brat! That's dangerous, get back here!"). In **3061** läuft dieselbe Szene aktiv (sub02
  zeigt msg00-08 per Message_on @0x1040-0x1184; 3061 hat subs 0-10, 3060 nur 0-3). Die
  3060-Variante trägt nur die Message-Tabelle — inkl. der **John-Zeile msg08, die es in der
  aktiven 3061-Fassung so nicht gibt**. Szenario-Divergenz + eine echte Waisen-Zeile.
- **4090/4091 msg01/02/03/04/06 @0x0BF9-0x0D43: die T-Virus-Synthese-Maschine** („The enzyme is
  already in place. I need to operate the machine to synthesize a sample of the T-Virus." /
  Roboterarm-Steuerbox \<YES/NO\> / Keycard-Slot / Zahlen-Keypanel). Beide Varianten haben nur
  subs 0-1 — die gesamte Maschinen-Interaktion ist unverdrahtet. Großer gestrichener
  Puzzle-Komplex.
- **4001 msg00/01/02 @0x1CA2-0x1D4F: „Blue Master Keycard"-Notschleusen-Tür** (Lockdown-Panel,
  \<YES/NO\>-Use, „You've used the Blue Master Keycard.") — nur in der x1-Variante vorhanden und
  dort verwaist.
- **6020/6021 sub02 @0x0BC4 + msg00 @0x0BDE: „Itembox is not available in this preview."** —
  das Itembox-Gegenstück zur bekannten Save-Preview-Meldung; Message UND Szene existieren, kein
  Trigger (sub_refs leer). RE1.5 hat designbedingt keine Item-Box — hier lag der gestrichene
  Interaktionspunkt.
- Kleinere: 3020 msg03/3021 msg00 („locked from the other side"), 3050/3051 msg01/02
  (Panel/Aufzug „not responding"), 3091 msg07, 30A0/30A1 msg04, 30B0/30B1 msg01
  (Computer-Save-Preview-Text!), 30C0/30C1 msg00, 30E0 msg06 („Manhole...?"), 4050/4051 msg03
  (David/Eric-ID-Memo), 40A0/40A1 msg04+09, 5010 msg02/5011 msg40, 5050/5051 msg04 („Will you
  register?"), 5130/5131 msg00-02 (Shelter-01/02-Türen), 5140/5141 msg00 + sub02
  (Objekt-Fahr-Choreographie ohne Trigger), 30C1 sub02 (leerer Stub).

---

## 7. Grenzen der Methode (UNSICHER-Ränder)

1. Der Walker läuft linear mit Vorwärts-Sprung-Tracking (identisch zum Census-Walker mit
   100.00%-Coverage-Claim); Code hinter `Evt_end`, der nur über Rückwärts-`Goto` erreichbar wäre,
   würde nicht gescannt (kein solcher Fall in den 240 RDTs aufgefallen — 0 Desyncs).
2. Gate-Analyse: nur `Ck`-Bedingungen direkt nach `If` vor dem Trigger; `Cmp`(work_vars)-,
   `Member_cmp`- und Else-Zweig-Logik nicht modelliert. Work-var-Dispatches (sce-5-AOTs) sind
   über ihre Evt_exec-Ziele trotzdem erfasst.
3. Engine-seitige Message-Anzeigen ausserhalb der RDT-Tabellen (sysmes/DEBUG.BIN-Item-Modal)
   referenzieren Raum-Messages nicht — einzige bekannte Ausnahme-Klasse wären hartkodierte
   EXE-Zugriffe auf Raum-msg-Indizes; dafür gibt es in den geprüften Fällen keinen Hinweis
   (die 21 „Save …preview"-Texte laufen alle über sce-1-AOTs, z.B. 1120 slot3 @0x0CF8).
4. Deutungen von Requisiten (1150-Leinwand, 20A0-Tropfen) sind als UNSICHER markiert — die
   Byte-Mechanik ist belegt, die visuelle Identität nicht gerendert verifiziert.

Reproduktion: Scratchpad `dormant2.py` (Sweep, JSON `dormant2_stage1_2_3_4_5_6.json`),
`gate_analysis.py` (Setzer-Index), `scdtool.py disasm <ROOM>` (Voll-Disassembly), Disasm-Dumps
`dis2_*.txt`.
