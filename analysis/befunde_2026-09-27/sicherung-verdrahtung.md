# Sicherungs-Raetsel ROOM1050 — was belegt ist, was ich gebaut habe, was ich NICHT gebaut habe

Arbeitsbaum: `.claude/worktrees/wf_3a416b11-82e-1` · 2026-09-27
Bilder: `analysis/befunde_2026-09-27/sicherung/`
Riegel: `re15_port/tests/unit/test_room1050_sicherung.c` (+ `probes/r35_sicherung-1050.cmake`)

---

## 0. Kurzfassung

**Das Raetsel ist NICHT gebaut — und das ist das Ergebnis, nicht ein Abbruch.**
Der Auftrag sagt: Gate und Fundstelle gehoeren zusammen, sonst keines von beidem.
Ich habe die Fundstelle mit sechs verschiedenen Wegen gesucht und **keinen Beleg**
gefunden, der ROOM1050s Shutter an eine auffindbare Sicherung bindet. Also baue ich
die Bedingung nicht — sie wuerde den Raum unloesbar machen.

Zwei Korrekturen an der Auftragslage sind dabei herausgekommen, beide gemessen:

1. **Die Sicherung IST im Spiel — nur nicht in ROOM1050.** ROOM2030 gibt sie her,
   ROOM2060 verbraucht sie. Sie ist ein **Flag**, kein Inventar-Gegenstand; darum hat
   Item 0x40 null Platzierungen.
2. **Der Mechanismus fuer das Cut-Paar ist `Cut_replace` (0x4B), nicht `Cut_chg`.**
   Das Schwesterraetsel fuehrt es vollstaendig vor.

Gebaut habe ich stattdessen den **Durchspielbarkeits-Riegel plus Herkunftsmarke**:
ein Haken, der live faehrt, dass der Shutter erreichbar und oeffenbar bleibt, und der
alle belegten Bytes festnagelt, auf denen eine spaetere Wiederherstellung aufsetzen muss.

---

## 1. Was RE1.5 mit der Sicherung wirklich macht

### 1.1 Fundstelle: ROOM2030 (STAGE2)

```
ROOM2030.RDT main00 @0x01BA6  21 03 6c 00   Ck(3,108,0)          ; noch nicht genommen?
ROOM2030.RDT main00 @0x01BAA  2c 09 03 31 01 00 f4 e8 fa 19 20 03 e8 03 ff 00 18 06 00 00
                              Aot_set slot=9 sce=3 flags=0x31 floor=1
                              Ecke(-5900,6650) Groesse(800,1000), Nutzlast -> sub06
ROOM2030.RDT sub06  @0x01FE4  2b 02 ff ff   Message_on 2  "Will you take the Fuse? "  (@0x208B)
ROOM2030.RDT sub06  @0x01FEE  21 0c 1f 00   Ck(12,31,0)          ; Antwort JA
ROOM2030.RDT sub06  @0x01FF2  22 03 6c 01   Set(3,108,1)         ; = "Sicherung genommen"
ROOM2030.RDT sub06  @0x01FF6  46 09 00 …    Aot_reset slot=9 sce=0
ROOM2030.RDT sub06  @0x02000  2b 03 ff ff   Message_on 3  "You've taken the Fuse."     (@0x20AC)
```

Es gibt **kein** `Item_aot_set` dafuer: die Sicherung ist in RE1.5 ein reines Flag
(bank 3, bit 108). Deshalb ist der Befund "Item 0x40 hat 0 Platzierungen" richtig und
trotzdem irrefuehrend — der Gegenstand ist im Spiel, nur nicht im Inventar.

### 1.2 Benutzungsstelle: ROOM2060 (STAGE2) — das VORBILD fuer ROOM1050

```
ROOM2060.RDT sub00 @0x010A2  21 03 6c 00  Ck(3,108,0)  -> slot9 sce=1 msg 3
                             "One of the fuses is missing..."               (@0x1833)
ROOM2060.RDT sub00 @0x010C2  21 03 90 00  Ck(3,144,0)  -> slot9 sce=3 -> sub18
ROOM2060.RDT sub00 @0x010DE  (sonst)      -> slot9 sce=1 msg 6
                             "The fuse is in place."                        (@0x1892)
ROOM2060.RDT sub00 @0x010F2  4b 05 0b     Cut_replace 5,11   <- Zustand beim WIEDEREINTRITT
ROOM2060.RDT sub00 @0x010F5  4b 06 0a     Cut_replace 6,10
ROOM2060.RDT sub18 @0x01678  2b 03 ff ff  Message_on 3
ROOM2060.RDT sub18 @0x0167E  2b 04 ff ff  Message_on 4  "Will you use the Fuse? "  (@0x1855)
ROOM2060.RDT sub18 @0x01688  21 0c 1f 00  Ck(12,31,0) -> Cut_chg 8 (Nahaufnahme VORHER)
ROOM2060.RDT sub19 @0x0169E  22 03 90 01  Set(3,144,1)      ; "Sicherung eingesetzt"
ROOM2060.RDT sub19 @0x016A2  29 09        Cut_chg 9         ; Nahaufnahme NACHHER
ROOM2060.RDT sub19 @0x016B8  46 09 01 31 06 00 ff ff 00 00  Aot_reset slot9 -> msg 6
ROOM2060.RDT sub19 @0x016C2  4b 05 0b     Cut_replace 5,11
ROOM2060.RDT sub19 @0x016C5  4b 06 0a     Cut_replace 6,10
ROOM2060.RDT sub19 @0x016C8  2b 05 ff ff  Message_on 5  "You've used the Fuse."  (@0x1875)
```

### 1.3 ⛔ KORREKTUR: `Cut_replace`, nicht `Cut_chg 8`

Kameratabelle `@0x60`, 32 B je Cut, `+0x1C` = pri/bg-Offset. Gemessen: die Paare
unterscheiden sich in **genau diesem einen Dword**, die ersten 28 Byte sind bitgleich.

| Raum | Paar | erste 28 Byte | pri-Offset |
|---|---|---|---|
| ROOM2060 | 5 / 11 | identisch | 0x65C / 0x674 |
| ROOM2060 | 6 / 10 | identisch | 0x660 / 0x670 |
| ROOM2060 | 8 / 9  | identisch | 0x668 / 0x66C |
| **ROOM1050** | **7 / 8** | **identisch** | **0x518 / 0x51C** |

`Cut_chg 8` waere ein einmaliger Blick; `Cut_replace 7,8` macht Kamera-Slot 7 dauerhaft
zum "Sicherung drin"-Bild — und genau das braucht ein Raetselzustand, der beim
Wiedereintritt bestehen bleiben muss. ROOM2060 tut beides: beim Einsetzen (sub19) und
beim Raumladen (sub00).

Zensus `Cut_replace` ueber alle 240 RDT: **80 Vorkommen in 14 Raeumen**
(1030/1031, 1130/1131, 11B0/11B1, 2040/2041, 2060/2061, 3090/3091, 30E1, 40A0/40A1).

### 1.4 Was Cut 7 und Cut 8 zeigen (selbst gemessen)

Hintergruende ueber `probe_bg_dump` aus `STAGE1/ROOM105.BSS` dekodiert
(`analysis/befunde_2026-09-27/sicherung/`):

* **Cut 7** — geoeffneter Wandschrank, **linke** Sicherung steckt (Leuchte gruen),
  **rechter** Sockel LEER, Leuchte **rot**.
* **Cut 8** — beide Sicherungen stecken, **beide Leuchten gruen**.

Abweichung je Schwelle der groessten Kanaldifferenz (MDEC-Rauschen faellt so heraus):

| Schwelle | Pixel | bbox |
|---|---|---|
| >0 | 18409 | x32..319 y0..215 (DCT-Rauschen) |
| **>4** | **2761** | **x123..268 y16..181** |
| >8 | 1544 | x137..221 y64..178 |
| >24 | 742 | x188..206 y64..162 |

Die frueher notierte Zahl 2317 gehoert zu einer anderen Schwelle; die bbox deckt sich.
Der Unterschied liegt **ausschliesslich** im Sicherungskasten.

---

## 2. Die Fundstelle fuer ROOM1050 — sechs Wege, kein Beleg

### (a) Ungenutzte AOT-Slots
ROOM1050 belegt **0..10 lueckenlos**: 0-5 Tueren, 6 Item (`H. Gun Bullets` x30 @0x00B9A),
7 Shutter-Schalter (sub00 @0x00C22), 8/9/10 Untersuchungstexte. Keine Luecke.
ROOM1051 hat zusaetzlich 11 und 12 — die gehoeren zum Leichen-Fund (s. 2c).

### (b) Flag-Spuren — Vollscan ueber alle 240 RDT
Ck/Set/0x58 opcode-exakt gelaufen. **Bank 3 benutzt 110 Bits; KEIN einziges ist
verwaist** — kein Bit wird nur gesetzt und nie geprueft, keines nur geprueft und nie
gesetzt. Die einzigen Sicherungs-Bits sind 108 (genommen) und 144 (eingesetzt), beide
STAGE2. ROOM1050 kennt nur bank 3 bit 121 = "Shutter offen"
(`sub00 @0x00C1E` Ck / `sub02 @0x00CBA` Set).

### (c) Requisiten/Subs ohne Besitzer
ROOM1050 hat `nOmodel=2`; beide werden gesetzt (main00 @0x00BB0 obj 1, sub00 @0x00C36
obj 0 = das geschlossene Rolltor). Unreferenzierte Subs: nur `sub01` — und das ist der
Per-Frame-Reseed-Slot, den die Engine selbst faehrt, kein Rest.
Rohbyte-Suche im gesamten RDT nach `2b 02` (Message_on 2), `29 07`/`29 08` (Cut_chg 7/8)
und `4b 07 08` (Cut_replace 7,8): **im SCD-Bereich 0xAD8..0xE3C null Treffer**; die
Treffer weiter hinten liegen in Textur-/Modelldaten. Es liegt also **kein herausgetrennter
Bytecode** mehr im Container.

**Was ich dabei gefunden habe (ein echter Nebenbefund):** ROOM1050 hat **drei** verwaiste
Cuts, nicht einen. Die RVD-Tabelle `@0x1B0` fuehrt fuer Cut 7, 8 **und 9** je einen
Anker-Eintrag (alle drei mit demselben Rechteck x 19600..22100, z -6900..-4900, to=0) —
also drei per Skript angesprungene Cuts. Cut 9 ist die Nahaufnahme des toten Polizisten.
**ROOM1051 (John) hat diesen Teil noch:**
```
ROOM1051.RDT main00 @0x00C0E  2c 0b 03 31 … ff 00 18 03   Aot_set slot=11 sce=3 -> sub03
ROOM1051.RDT main00 @0x00C22  50 0c 09 31 00 00 00 00 00 00 00 00 00 00 04 00 0f 00 a5 00 ff 00
                              Item_aot_set slot=12, Rechteck (0,0,0,0) = "Parkplatz",
                              item=4 SIG P228, n=15, flag=165
ROOM1051.RDT sub01  @0x00CB6  21 09 a5 01   Ck(9,165,1) -> Aot_reset slot=11
ROOM1051.RDT sub03  @0x00DB4  29 09         Cut_chg 9
ROOM1051.RDT sub03  @0x00DB6  2b 05 ff ff   Message_on 5  "He's got a big bite on his neck…"
ROOM1051.RDT sub03  @0x00DC0  29 03         Cut_chg 3
```
In ROOM1050 fehlen Slot 11 und 12 komplett, msg 5 ist dort ebenfalls verwaist.
Das ist **die Bauform**, die eine Fundstelle in diesem Raum haette — Nahaufnahme-Cut +
Nachricht + geparkter `Item_aot_set` + Flag. Nur fuer die Sicherung gibt es sie nirgends.

### (d) Die Item-Nachbarn
`0x3F Pocket Watch` = ROOM2090 main00 @0x00A12. `0x41 Spark Plug` = ROOM4040 main00
@0x00F66. Zwei verschiedene Stages, keine raeumliche Nachbarschaft. Das Indiz faellt aus.

### (e) RE2 als Vorbild
RE2 hat **zwei** Sicherungen: `Main Fuse` (0x4C) und `Fuse Case` (0x4D), beide in
**ROOM60D0** (`PL0/RDT/ROOM60D0.RDT @0x0008E6` / `@0x0008FC`), benutzt im selben
Stage-6-Bereich (Raumtexte in ROOM6080/60D0/6110). RE2s Muster ist also
**Fund und Benutzung im selben Abschnitt** — was gegen die Annahme spricht, ROOM1050
(STAGE1) haenge an der STAGE2-Sicherung.
Beleg: `analysis/befunde_2026-09-21/re2-sicherung-item.md`, Extrakt `extracted_re2_sicherung/`.

### (f) Textsuche
Alle 240 RDT-Nachrichtenbloecke nach `fuse` durchsucht: 20 Treffer in 1050/1051 (msg 2,
verwaist), 2030/2031, 2060/2061, 4030/4031 (`Main Fuse` — ein eigenes, spaeteres Raetsel
mit mehreren Sicherungen). **Keine Nachricht im ganzen Spiel beschreibt, wo eine
Sicherung liegt.** Ergaenzend nach `shutter|power|generator|breaker|circuit|socket`
gesucht: STAGE1s Strom-Thema laeuft ueber ROOM11F0s "Reserve Power Control Panel",
nicht ueber eine Sicherung.

### Byte-Anker-Gegenprobe
Zusaetzlich zum opcode-exakten Walker eine Suche an **jeder Byteposition** aller RDT und
BIN nach einem `Item_aot_set`-Record (0x50, sce=9) mit Item 0x40 — die kann strukturell
nichts uebersehen. **0 Treffer.** Dieselbe Suche findet die geparkte SIG P228 in
ROOM1051 @0x00C22, also funktioniert sie.

### Ergebnis
**Kein Beleg fuer eine Fundstelle.** Die einzige erreichbare Sicherung des Spiels ist die
in ROOM2030, und sie gehoert nachweislich zum Generator in ROOM2060.

---

## 3. Der Verdrahtungsweg — geklaert, aber nicht benutzt

Der Port **patcht keine Original-Assets**. Eingriffe laufen port-seitig, und es gibt
dafuer zwei etablierte Formen:

1. **Port-seitige Tabelle**, aus den ausgelieferten Daten abgeleitet:
   `re15_port/tools/gen_*.py` -> `engine/src/gen/*.inc` (13 Stueck, z.B.
   `discard_sites.inc`). Richtig, wenn die Regel aus den Daten MESSBAR ist.
2. **Schicht in der SCD-VM**, an der Aot_set-Stelle, mit dem autorisierten Record im
   Kommentar: `engine/src/scd_vm.c` op_aot_set, Fall ROOM1150/1151 Slot 1
   (Zeilen um 2890) — dort wird ein sce=0-Record port-seitig armiert, so wie es ein
   `Aot_reset(1,3,…)` taete (LAB_80040738).

Fuer das Sicherungs-Raetsel waere **Form 2** der Weg: ROOM1050 sub00 port-seitig um die
Bedingung erweitern und `Cut_replace 7,8` nachziehen. **Ich habe ihn nicht beschritten**,
weil Punkt 2 keinen Beleg fuer die Fundstelle liefert.

---

## 4. Was ich gebaut habe

`re15_port/tests/unit/test_room1050_sicherung.c`, registriert ueber
`re15_port/tests/unit/probes/r35_sicherung-1050.cmake` als `unit_room1050_sicherung`.
Kein Verhaltens-Code, keine Konstante im Spielpfad.

**Herkunftsmarke** auf den ausgelieferten Bytes — Cut-Paar 7/8 (pri 0x518/0x51C), die
drei ROOM2060-Paare, die fuenf `Cut_replace`-Stellen, die vier Flag-Stellen der
Sicherungskette, die verwaiste ROOM1050-msg-2 und die 0 Ausgabe-Records fuer Item 0x40.

**Durchspielbarkeit, live gefahren** (nicht argumentiert):

```
  Gegenprobe: mit flag(3,121)=1 bleibt Slot 7 WEG (act=0)
  Vorzustand: SCA-Zelle 19 in allen 5 Partitionen SOLIDE (u0=0xFF, floor=3)
  Schalter:   AOT 7 (sub00 @0x00C22) ev=2 Mitte(17200,-8550) halb(400,400)
  Frage:      msg 0 "It's a shutter switch. / Will you push it?"
  flag(3,121)=1 (sub02 @0x00CBA) — der Shutter ist offen
  SCA-Zelle 19 in allen 5 Partitionen frei (u0=0, floor=0)
```

Drei Dinge machen das zu einer Messung statt zu einer Selbstbestaetigung:
* der **Vorzustand** wird mitgeprueft (solide vorher, frei nachher),
* die **Gegenprobe** zeigt, dass der Haken einen fehlenden Schalter wirklich sieht,
* sie laeuft auf einer **Kopie** der RDT-Bytes, weil `Sca_id_set` den SCA-Block im
  Puffer bleibend umschreibt.

Wer spaeter die Sicherungs-Bedingung einbaut, muss diesen Haken mitziehen — ohne
auffindbare Sicherung faellt er, und genau dann waere der Raum unloesbar.

---

## 5. Was ich BEWUSST nicht gebaut habe

* **Die Shutter-Bedingung** (`Ck(3,108,0)` -> msg 2). Kein Beleg, dass ROOM1050 an
  bank 3 bit 108 haengt; STAGE1 liegt vor STAGE2, und RE2s Muster ist Fund und
  Benutzung im selben Abschnitt.
* **`Cut_replace 7,8` als reine Anzeige.** Geprueft, nicht reflexhaft verworfen: Cut 8
  zeigt eine **eingesetzte** Sicherung. Ohne Einsetz-Handlung waere das eine Anzeige,
  die luegt. Und Cut 7 ist selbst unerreichbar — es gibt nichts, worauf der Tausch
  wirken koennte.
* **Eine Fundstelle "im Wachraum".** Das waere Design-Geschmack, kein Beleg.
* **Ein Prop-Modell.** Das Modell aus Commit 79e1b6e0 bleibt liegen, bis eine Fundstelle
  belegt ist; fuer den Kasten selbst braucht es ohnehin keines (die Kunst ist gemalt).

---

## 6. Offen — der naechste Weg

Der aussichtsreichste noch nicht begangene Weg ist **dynamisch**: ein DuckStation-Lauf
in ROOM1050 auf der Original-Disc mit RAM-Beobachtung, ob das ausgelieferte Spiel
vielleicht doch auf Cut 7 schaltet (die RVD-Anker fuer 7/8/9 existieren). Statisch ist
der Raum ausgeschoepft.

Zweitens: **ROOM1050s zweite verwaiste Stelle** — die Leichen-Nahaufnahme (Cut 9 +
msg 5), die ROOM1051 vollstaendig hat und ROOM1050 fehlt. Das ist ein eigener, sauber
belegter Wiederherstellungs-Kandidat mit Vorlage in der Schwesterdatei — aber ein
anderes Arbeitspaket, und es gehoert dem Nutzer vorgelegt, bevor es jemand baut.
