# Messer-Dauerschlag — die Zombies kommen nicht heran (Runde 28)

Nutzer 2026-09-26: "wenn ich mit dem Messer die ganze Zeit nach unten schlage, kommen die
zombies nicht nah genug an mich ran, um mich zu beissen. bzw. sie beissen mich dann nicht."

Sonde: `re15_port/tests/unit/probe_r28_messer_dauerschlag.c` (echter Weg: `re15_game_step` +
Pad, ROOM1140, RE2-Bank EM010, RE2-KI = Auslieferungs-Default).

## 1. Reproduktion (Schritt 1)

Vier sonst identische Laeufe, 1400 Bilder, Startabstand 3400, gemeinsame Aufwaermphase
(R1 halten bis zielbereit) — danach unterscheiden sie sich NUR im Schlag:

<!-- ZAHLEN-PLATZHALTER: wird nach der Nachmessung ersetzt -->

Reichweiten-Sweep (`RE15_R28_SWEEP=1`, KI angehalten, Zombie auf festen Abstand):

| | bis d |
|---|---|
| RE1.5-KI (RE1.5-Kegel) | **1500** |
| RE2-KI (RE2-Applier-Sub-Box) | **3000 TIEF / 3400 EBEN** |

## 2. Die Kandidaten, einzeln

### (a) Klassen-Latch `s_aim_melee` — AUSGESCHLOSSEN
Alle elf Lesestellen durchgesehen (player_common.c:323, 346, 410, 476, 936, 971, 973, 974,
1012, 1024, 1046, 1088): Schlagfenster, Schrot-Fenster, Schwung-SE, Granaten-Frame,
Recoil-Break, Auto-Track-Radius/Slew, Zieh-Clip, In-Hand-Latch, Elevations-Frueh-Exit,
Nachlade-Zweig. **Keine** traegt ein Abstands-, Kollisions- oder Annaeherungs-Gatter.
Empirisch: Lauf B (Latch = melee) und Lauf C (Latch nie gesetzt) liefern bitgleich
6 Bisse / 3 Griffe / 285 Bilder unter 1200.

### (b) Fehlende Selbst-Ausschiebung (aec4-Haelfte) — NICHT DER FALL, der Zombie hat sie
`enemy_ai_common.c:13920` fuehrt `re15_body_push(pl, 450, e, e->hit_radius_min)` im
Zombie-Tick (die aec4-Haelfte a0 = Spieler / a1 = Zombie), gefolgt vom b544-Lauf gegen die
anderen Gegner. Empirisch: Lauf D (kein Griff, also keine Freeze-Ausnahme) erreicht nach
dem Fix exakt d_min = 850 = 400 (Zombie-Box STAGE1.BIN @file 0x1f778) + 450 (Spieler-Box
PSX.EXE @file 0x64694 = 0x1c2) — beide Haelften laufen.

### (c) Rueckstoss / Schlagtakt — NICHT die Ursache
Lauf D trifft jeden Tick und der Zombie erreicht trotzdem 850 — ein Treffer SCHIEBT ihn
also nicht weg; was ihn anhaelt, ist der ZUSTAND (Trefferreaktion). Takt: PL00W01.EDD
(selbst geparst, Tabelle ab Datei-Offset 0, 14 x u16 frame_count/offset) =
22,16,52,1,50,30,10,**25**,1,**20**,1,**20**,1,15. Schlag-Clips 7/9/11 = EBEN 25 /
HOCH 20 / TIEF 20 Bilder; der Dauerschlag TIEF laeuft also auf 21 Bilder je Schlag = die
Clip-Laenge. Der offene Runde-26-Nebenbefund "der Port feuert viermal so schnell wie RE2"
betrifft die Dauerfeuer-Waffen, NICHT das Messer.

### (d) Biss-Gatter des Zombies — korrekt, aber gegen einen Stehenden fast leer
DECISION[1] @0x80101714: Block D `sltiu 0xdac` @0x8010185c und Block E `sltiu 0x9c4`
@0x801018a4 verlangen zusaetzlich `andi 0x15` @0x8010187c bzw. `andi 0x17` @0x801018c4 auf
0x800CFBF6 = der Spieler muss GEHEN (Bit 0x2) oder LAUFEN (Bit 0x4). Beim Zielen ist er
festgenagelt, also bleibt gegen einen stehenden Spieler NUR Block G, der Seiten-Griff:
`sltiu 0x4b0` @0x801018f4 = **dist < 1200**, plus die beiden Sektor-Tests @0x80101948 /
@0x8010198c. Der Port hat dort KEINE Zusatzbedingung (kein Lesen des Spieler-Zielzustands
im Zombie-Gehirn; die beiden `re15_player_aim_elevation()`-Aufrufe waehlen nur die
HOCH/TIEF-Variante der Reaktion).

### (e) Trefferfilter +0x1D3 — ECHTER Defekt, aber nicht die Ursache des Befundes
Gemessen: nach einem Messertreffer 15 -> 13 -> 11 -> ... = **zwei** Abzuege je Bild. Das
Original hat GENAU EINEN, im Root-Prolog (EMZ0.BIN, laedt roh @0x80100000):
```
80100484: lbu   v1,467(s0)
80100488: nop
8010048c: andi  v0,v1,0x7f
80100490: beq   v0,zero,0x8010049c
80100494: addiu v0,v1,-1
80100498: sb    v0,467(s0)
```
Port: `enemy_ai_common.c` (Runde-14-Zeile) UND der RE2-Root `re15_re2z_tick`. Trefferpause
damit 8 statt 15 Bildern (Stun 15 fuer Messer/Zombie: Zeile 0x800A412C, Stun =
(Wort1 >> 9) & 0x7F, Stempel @0x80047338-4C; Filter `lbu v0,467(s0)` / `bne v0,zero`
@0x80047138-40). Behoben — aber der Schlagtakt ist 21 Bilder, also groesser als BEIDE
Pausen: der Befund aendert sich dadurch nicht.

## 3. Die Ursache

Das Messer laeuft seit Runde 19 ("re-restposten") fuer einen RE2-eigenen Zombie durch die
RE2-Geometrie-Records (@0x800A63A8 EBEN / @0x800A657C TIEF, Stride 0x1C, Muster
`ff/6 00/1 01/1 02/1 03/1 04/1 00/255` @0x800A6434). Das war fuer die TEILE-MASKE gedacht,
hat aber die Reichweite von 1500 auf 3000/3400 mehr als verdoppelt.

Sollseite ist RE1.5 — dort ist das Nahkampf-System VOLLSTAENDIG: die Tester-Dispatch-Tabelle
@0x8006E548 fuehrt GENAU die Ids 1 und 2 auf FUN_800127FC (den Nahkampf-KEGEL), und dessen
Treffer-Bedingung ist `dist < Reichweite(@0x8006E5A0[1] = 1100) + Gegner-Radius (hbdata+6 =
400)` = 1500, gemessen ab dem Klingen-Punkt.

Mit 3000/3400 schlaegt der Port den Zombie 1800-2200 Einheiten VOR seinem einzigen
erreichbaren Angriffstor (dist < 1200) in die Trefferreaktion — er kommt nie an.

## 4. Fix und Riegel

Der Applier entscheidet weiterhin, WELCHES Koerperteil/welche Klammer getroffen ist; der
RE1.5-Kegel entscheidet wieder, OB die Waffe hinreicht (nur Ids 1/2 — die Kegel-Spalte der
Tabelle @0x8006E548). Schusswaffen unveraendert.

RIEGEL `r28_messer_dauerschlag`: exit 1, sobald die Reichweite ueber 1500 liegt. Am alten
Stand nachgefahren: `RIEGEL-ROT (1): Messer reicht 3000/3400 statt 1500`, EXIT=1.
GEGEN-RIEGEL: Reichweite >= 1400 (das Messer trifft weiterhin); Bisse A <= Bisse B (der
geschlagene Zombie beisst nicht OEFTER als der unbehelligte); d_min im Dauertreffer-Lauf
>= 850 (Koerper-Standabstand, FUN_8002aec4 radSum @0x8002b164) — die Zombies laufen nicht
durch den Spieler.

## 5. Offen

* Der Klassen-Latch `s_aim_melee` bleibt bestehen (Runde 26 ausdruecklich vertagt); er ist
  fuer diesen Befund nachweislich folgenlos, aber weiterhin eine Port-Erfindung — das
  Original liest die angelegte Waffe jedes Bild frisch (`lbu 0x800aca5d` @0x80032e60,
  Tabelle @0x80074030, `jalr` @0x80032e84).
* Das Doppel-Dekrement von +0x1D3 ist fuer den ZOMBIE behoben. Der HUND hat dieselbe
  Doppelung auf einem EIGENEN Weg (nur CODE-GELESEN, nicht gemessen):
  `re15_dog_ai_tick` zieht +0x1D3 in enemy_ai_common.c:7602 ab und ruft danach
  `re15_re2dog_tick(slot)` (enemy_ai_common.c:7650), dessen Root-Prolog es in
  enemy_ai_re2_dog.c:2230 noch einmal abzieht (Original: EMD0G_MOD0.BIN @0x80100028-3C, EIN
  Dekrement). Kraehe (enemy_ai_re2_crow.c:1786) und Spinne (enemy_ai_re2_spider.c:2838)
  haben ebenfalls ein eigenes Root-Dekrement; ob ihr Weg ueber die gemeinsame Zeile laeuft,
  ist nicht geprueft. Alles drei bleibt OFFEN — eine eigene Runde mit eigenen Messungen,
  weil dort Hunde-Pins aus Runde 13/14/27 haengen.
