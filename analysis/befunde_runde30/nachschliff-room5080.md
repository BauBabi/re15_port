# Nachschliff Runde 30 — ROOM5080: „Leon bewegt sich nach der Generator-Folge nicht mehr"

Spur `room5080`, Zweig `r30/n-room5080`, Basis `cac33993`. Ermittlung. Nach der Nachbesserung gibt es einen Engine-Eingriff (N2, Abschnitt 6).

## Kurzfassung

| Behauptung | Ergebnis |
|---|---|
| „sub02 hängt in der Warteschleife hinter Plc_dest" | **widerlegt.** Die Schleife endet nach 21 Bildern. Die Folge gibt den Spieler frei: Pad-Sperre weg, Kommando 1. Gemessen im Debug-Sprung **und** auf dem regulären Türweg aus ROOM6010 (Abschnitt 4.1/4.2). |
| „Leon bewegt sich danach nicht mehr" | **echt, Ursache belegt: N1.** Birkin G1 (Typ 0x30) greift an, sobald sub03 ihn freischaltet. Trifft er, hält der Port Leon über `s_player_grabbed` fest, statt die Opfer-FSM des Originals zu fahren. Der Port hält ihn **308 Bilder** am Stück, danach in Zyklen von 263 Bildern. Das Original gibt ihn nach der Halte-Phase frei, die ohne Tasteneingabe **19 Bilder** dauert (Zähler 75, −4 je Bild), dazu kommen die Clip-Phasen davor und danach. Die Lücke steht schon als HONEST-OPEN im Port (enemy_ai_common.c:11435-11436). **Nicht gebaut, und das ist eine Umfangsentscheidung** (Abschnitt 7, N1). |
| „Tür ohne Strom (Art S) nur über RE15_FIRE_AOT erreichbar, nicht zu Fuß" | **widerlegt.** Echtlauf über den regulären Türweg: Leon lockt den Stoß aus 3200 Einheiten an und läuft unten um Birkin herum. Mit hp 100 erreicht er die Tür. msg 2 geht auf, dazu **1 × ZU_P** (tuerse.log `F1090 raum=5080 nachricht=2 art=S weg=AOT satz=3(ZU_P) nr=1`). Danach schließt der Text, und Leon läuft wieder (Abschnitt 4.5). |
| (Gegenprüfer, Nachbesserung) Birkin geht **vor** der Folge durch die Außenwand und beißt Leon | **bestätigt, Ursache belegt, gebaut: N2.** Die Birkin-Wandklemme des Ports nahm das Band aus der Höhe: `band_from_y(-5600)` = 3, und in Band 3 liegt keine Zelle. Das Original nimmt das Zustands-Byte +0x82, das der Spawn auf 0 setzt, und klemmt ihn an der Nordwand. Vor dem Bau stand er im Echtlauf ab Bild 471 im Rauten-Block und biss Leon von hp 100 auf 10. Die Sonde fand ihn in 2024 von 2400 Bildern in einer Wandzelle, hp fiel bis 0. Nach dem Bau steht er in 0 Bildern in einer Zelle und bleibt bei z −5636 vor der Nordwand. Leon behält hp 100 (Abschnitt 8.1). |

Befunde, einheitlich nummeriert (Abschnitt 7):
- **N1** Pin statt Opfer-FSM des Spielers. Belegte Ursache des Nutzer-Symptoms, nicht gebaut (Umfangsentscheidung). Die Lücke ist schon als HONEST-OPEN benannt.
- **N2** Birkin-Wandklemme mit falschem Band. Belegt und **gebaut**, mit Riegel `unit_r30_n_room5080_birkin_wand`.
- **N3** Falsche STAGE5-Adresse im Port-Kommentar zum Birkin-Tod-Flag. Nur der Kommentar war falsch, er ist **korrigiert**.

---

## 1 Symptom (Übergabe)

Der Nebenbefund stammt aus dem Echtlauf der Tür-Töne (`tuer-verschlossen.md`, „Nicht gemessen"):
- Die Generator-Folge läuft: zwei Texte, F247–F499 und F501–F835.
- Danach kam Leon in 4 Anläufen nicht zurück zur Tür. Er hing bei x ≈ −25440.
- „Nach einer Wandberührung bewegte ihn der folgende Laufbefehl nicht mehr."
- „In einem Lauf wanderte er ohne Eingabe langsam nach −x."

Der Gegenprüfer hat daraus die Ursache „sub02 hängt in der Warteschleife" gemacht. Untersucht war das nicht (Dossier: „nicht Teil dieser Spur und nicht untersucht").

## 2 Die Folge im Skript (ROOM5080.RDT, `re15_port/tools/scd_dump_room.py`)

| Datei-Offset | Bytes | Bedeutung |
|---|---|---|
| main00 0x006FA | `3b 00 02 31 … b2 da 96 b5 84 03 d0 07 12 99 00 00 d4 9a 00 00 05 01 07 …` | Door_aot_set Platz 0: Rechteck (−9550,−19050,900,2000), Ziel Stage-Byte 5 / Raum 1 = ROOM6010 (Formel aot_common.c:597-619) |
| sub00 0x0072A | `21 03 30 00` | nur wenn (3,48) = 0: … |
| sub00 0x0072E | `2c 01 03 31 … 5c 95 c8 b5 20 03 6c 07 ff 00 18 02` | Aot_set Platz 1 = Generator-Ereignis → sub02 |
| sub00 0x00746 | `44 00 30 33 00 00 00 ff 4c b9 20 ea c8 00 … 00 08` | Sce_em_set Slot 0, **Typ 0x30 (G-Birkin)**, grid 0x33, Lage (−18100,−5600,200) = geparkt außerhalb |
| sub01 0x0078A | `21 05 1c 01` → `22 05 1c 00`, `22 03 30 01`, `22 03 31 01`, `04 ff 18 04` | Birkin tot (5,28) → Strom da: (3,48) (3,49) setzen, sub04 |
| sub02 0x007AE | `2b 00 ff ff` | msg 0 „Will you turn on the switch?" (Ja/Nein) |
| sub02 0x007B8 | `21 0c 1f 00` | Antwort Ja ((12,31) = 0, dieselbe Abfrage wie in ~100 anderen Subs) |
| sub02 0x007BC | `2b 01 ff ff` | msg 1 „The emergency mode has been activated…" |
| sub02 0x007C2 | `46 00 01 31 02 00 ff ff 00 00` | Platz 0 (Tür) → Text-Platz msg 2 = **Art S** |
| sub02 0x007DC | `22 02 07 01` | Pad-Sperre an (Zone 2 = DAT_800aca40, Bit 0x01000000, game_state.c) |
| sub02 0x007EC | `04 ff 18 03` | sub03: Birkin fällt |
| sub02 0x007F4 | `40 00 09 20 4c b9 78 ba` | **Plc_dest** Spieler, Mode 9 (Drehen auf der Stelle), Flag-Bit 0x20, Ziel (−18100,−17800) = der Birkin-Absprungpunkt |
| sub02 0x007FC/0x00802/0x00804 | `11 00 08 00` … `12 04` `21 05 20 00` | Do / Evt_next / Edwhile **Ck(5,32) == 0** = die Warteschleife |
| sub02 0x00808 | `09 0a 14 00` | Sleep 20 |
| sub02 0x00810 | `22 02 07 00` | Pad-Sperre aus |
| sub02 0x00814/0x00816/0x00818 | `29 04` `3c 01` `42` | Cut_chg 4, Cut_auto 1, **Plc_ret** |
| sub03 0x00822 | `32 00 4c b9 20 ea 78 ba` | Pos_set Birkin (−18100,−5600,−17800) |
| sub03 0x0082E/0x00832 | `2f 01 90 01`, `0d 00 06 00 0e 00` … `30` | Speed_set 400, 6 × Add_speed = der Fall |
| sub03 0x0083E | `34 0c 13 00` | **Member_set(0x0C, 0x13)** → grid 0x13 = Kampf frei |
| sub04 0x00852 | `46 00 02 31 12 99 00 00 d4 9a` | nach dem Birkin-Tod: Platz 0 wieder Tür (die Art S ist damit weg) |

Texte (`rdt_msgdump.py`):
- msg 0 @0x0880: „…Will you turn on the switch?"
- msg 1 @0x08F8: „The emergency mode has been activated…"
- msg 2 @0x0996: „The door won't open until the power is restored!"
- msg 3 @0x09CA: „The power already has been supplied to the train!"

## 3 Original-Mechanismus (selbst disassembliert, `re15_disasm.py`)

**SCD-Opcode-Tabelle** @0x800744a8 (Basis = Eintrag für Set 0x22 @0x80074530 − 0x88):
- [0x21] Ck = 0x8003fcf4
- [0x40] Plc_dest = 0x80041be4
- [0x42] Plc_ret = 0x80041f88

**Plc_dest** 0x80041be4 (Work-Entity = Thread+0x154, `lw a1,340(a0)`):
- 80041c14 `sb v0(=4),4(a1)` → Kommando 4 (Plc-Executor)
- 80041c18 `sb a2,5(a1)` → Sub = Mode 9
- 80041c1c/20 `sb zero,6/7(a1)`
- 80041c24 `sb v1,451(a1)` → +0x1c3 = Flag-Bit 0x20
- 80041c38 `sh v0,444(a1)` / 80041c58 `sh v1,446(a1)` → Ziel x/z
- 80041c4c `sh zero,452(a1)`

**Spieler-Kommandotabelle** @0x80073f90: [4] = 0x80030660 (Executor). Die Mode-Tabelle @0x80073e30 hat [9] = **0x80031360**. Der Handler dreht nur, er läuft nicht:
- Phase 0:
  - 80031398 Phase := 1
  - 800313a4 `sb 5,0x800acae8` (+0x94 Clip 5)
  - 800313b0 +0x95 := 0
  - 800313b8 +0x8f := 7
- Phase 1:
  - 800313d0 `jal 0x8001ab9c` (arc_test gegen Ziel +0x1bc/+0x1be) mit 800313d4 `ori a2,zero,0x60`. Ergebnis ≠ 0 → nur weiterdrehen (800313d8 `bne v0,zero,0x8003142c`).
  - Ergebnis 0 = **Ankunft**:
    - 800313ec `sb 6,0x800aca59` (Sub 6 = Event-Reach)
    - 800313f4 Phase := 0
    - **800313f8 `jal 0x8004ef90`** mit a0 = 0x800acc10+0x4418 = **0x800b1028 (Bank 5)** @0x800313fc und a1 = `lbu 0x800acc17` (+0x1c3) @0x800313e4.
  - 0x8004ef90 = `bank[bit>>5] |= 0x80000000 >> (bit&0x1f)` (8004ef90–8004efb4). Damit ist (5,32) gesetzt, und die Edwhile-Schleife endet.
- Drehen: 8003143c `jal 0x8001aac4` mit `ori a2,zero,0x60` = 96 Einheiten je Bild.

**Plc_ret** 0x80041f88:
- 80041f90 `sb 1,4(v0)` → Kommando 1 (frei)
- 80041f94–9c +5/+6/+7 := 0

**Die einzige Abbruchbedingung der Schleife ist also die Ankunft der Drehung.** Mode 9 bewegt die Figur nicht. Kollision, der Walk-Solver oder x ≈ −25440 können die Bedingung gar nicht berühren. Die Drehung dauert höchstens 2048/96 ≈ 22 Bilder. Birkin landet gleichzeitig 8400 Einheiten entfernt.

## 4 Messung (echte `re15_pc.exe`, Bau dieses Baums)

Treiber: `nachschliff-room5080_tools/r30_5080_echtlauf.sh`
- Schienen: RE15_STATE_LOG (je Bild pad / Spieler / pauseflags / jeder Aktor), RE15_DISCARD_LOG, debug.log; für Birkins Höhe zusätzlich `RE15_BIRKIN_DBG=1` (birkin_dbg.log, je 15 Birkin-Ticks).
- Eingaben: `skript_bau.py`; die Skripte der Läufe liegen als `tuerweg_skript.txt`, `lauf1_skript.txt`, `ausweich_skript.txt` bei.
- Logs unter `build/r30n5080_*` (nicht committet).

### 4.0 Zeitbasis der Eingabeskripte (Nachbesserung, Punkt 4)

Die Läufe sind nur mit der richtigen Zeitbasis nachfahrbar. Der Treiber setzt sie jetzt selbst: Standard ist `RE15_INPUT_SCRIPT_BASIS=spiel`, `BASIS=roh` schaltet sie ab (input_pc.c:70-74/110-111).

| Lauf | Basis | debug.log | Aufruf |
|---|---|---|---|
| `lauf1` (Debug-Sprung) | roh | `Tick 0 -> F77` | `BASIS=roh r30_5080_echtlauf.sh <ziel> "$(cat lauf1_skript.txt)" 200 80` |
| `tuerweg`, `ausweich1`–`6` (Türweg) | **spiel** | `Tick 0 -> F160` | `RAUM=6010 r30_5080_echtlauf.sh <ziel> "$(cat tuerweg_skript.txt)" 160 80 RE15_FIRE_AOT=2@60#6010` |
| Gegenprüfer `tuerweg` (Birkin vor der Folge, 8.1) | roh | `Tick 0 -> F37` | wie Türweg, mit `BASIS=roh` |

Ohne `spiel` zählt der Parser die gerenderten Bilder ab Programmstart. Beim Türweg verschiebt sich das Skript dadurch um 123 Bilder, es läuft schon in ROOM6010 an, und die Generator-AOT wird nie ausgelöst. Genau so entstand der Gegenprüfer-Lauf, den 8.1 auswertet.

### 4.1 Die Warteschleife endet — Debug-Sprung (Lauf `lauf1`, `RE15_DEBUG_JUMP=5080@120`)

| Bild | pad | Spieler | pst | pauseflags | pm | Ereignis |
|---|---|---|---|---|---|---|
| F835 | 8000 | (−26483,−18200) rot −6144 | 1 | 00000007 | 0 | msg 1 zu |
| F836 | 8000 | rot 1952 | **4** | **01000007** | 2 | `[scd] Plc_dest(slot=0 mode=0x09 dest=(-18100,-17800) flag=32)` |
| F837–F856 | 0 | rot −96 je Bild | 4 | 01000007 | 2 | Drehung (0x60 = @0x8003143c) |
| F857 | 0 | rot **4065** | 4 | 01000007 | 2 | Ankunft, mo 1 (Sub 6 Clip 1) |
| F878 | 8000 | — | **1** | **00000007** | 1 | `[scd F878] Cut_chg(4)` = nach Sleep 20, Plc_ret |
| F892 | 0 | — | 1 | 00000007 | **0** | `letterbox closed -> gameplay` |

Die Schleife läuft **21 Bilder** (F836–F857).

### 4.2 Regulärer Weg (Lauf `tuerweg`)

`RE15_DEBUG_JUMP=6010@120`, dann `RE15_FIRE_AOT=2@60#6010`: der Tür-Platz 2 von ROOM6010, derselbe Pfad wie das Hineinlaufen (aot_fire_door). Zeitbasis `spiel` (4.0).

- Eintritt ROOM5080 bei (−9950,−18200) **rot 2048** (Tür-Datensatz).
- Plc_dest F861, Cut_chg(4) F903, Spiel frei F917. **Dieselben 42 Bilder** wie im Debug-Sprung.

### 4.3 Debug-Sprung gegen regulären Weg: keine fehlenden Vorbedingungen

**Einzige Tür nach ROOM5080** (`tueren_nach_5080.py`, alle RDTs): ROOM6010/6011 main00 @0x00FCE, Platz 2
- Bytes `3b 02 02 31 00 00 c6 94 b4 97 66 03 b1 08 22 d9 00 00 e8 b8 00 08 04 08 …`
- Ziel (−9950,0,−18200), Richtung 2048, Cut 0.

**Flag-Zensus** (`flag_zensus_5080.py`, 202 Treffer):
- (3,48): nur ROOM5080/5081 (sub00-Schranke, sub01-Setzer). (3,49): wird nur in ROOM6000/6001 main00 @0x00DFE gelesen (der Zug).
- (12,31): die allgemeine Ja/Nein-Antwort.
- (5,28): der Birkin-Tod.
- **Kein anderer Raum setzt ein Flag, das die Folge in ROOM5080 schaltet.**

**Unterschiede Debug-Sprung ↔ Tür:**
- Spawn-Richtung 0 statt 2048;
- das Inventar. Der Debug-Lauf hat keine geladene Waffe (mg=0). Deshalb gibt es dort keinen Kampf, nur Ausweichen.

**Die Folge ist kein Artefakt des Debug-Sprungs und hängt in beiden Wegen nicht.**

### 4.4 Was Leon danach tatsächlich festhält

**Der Raum** (`sca_5080.py`):
- ein Kreuz aus vier Armen um vier Rauten-Blöcke (SCA-Typ 2 = Raute, push_diag2 @0x8003d00c);
- der West-Arm (Generator) ist ein gerader Gang z ∈ [−19725,−16525] (Typ-1-Zellen #56/#59).

**Birkin G1** (Typ 0x30, grid 0x13 nach Member_set) läuft Leon mit 30 je Bild entgegen:
- Körperzylinder r 1000 + Spieler 450 = 1450, FUN_8002aec4/FUN_8002b544. Die Boxen sind selbst nachgelesen, jetzt mit dem Nachweis, wer sie benutzt (Nachbesserung, Punkt 5; die erste Fassung hatte die Birkin-Box nur per Bytesuche gefunden):
  - **Birkin, STAGE5:** INIT @0x80116f74 `lui v0,0x8012` / **@0x80116f78 `lw v0,-448(v0)`** liest die Zeigertabelle 0x8011fe40 = [0x8011fe28, 0x8011fe34]. **@0x80116f80 `sw v0,120(v1)`** schreibt den ersten Zeiger nach entity+0x78. Box @0x8011fe28 = `{0,-1440,0,1000,1440,1000}`, dahinter @0x8011fe34 `{0,-180,0,1000,180,1000}`.
  - **Birkin, STAGE3:** @0x80116760 `lui v0,0x8012` / **@0x80116764 `lw v0,-4484(v0)`** = 0x8011ee7c → [0x8011ee64, 0x8011ee70], @0x8011676c `sw v0,120(v1)`. Box @0x8011ee64 ist dieselbe.
  - Den Radius liest die Wurzel selbst: STAGE5 @0x80116ccc `lw v0,120(v0)` / @0x80116cd4 `lhu a1,6(v0)` = 1000 für FUN_8003b0a4 (8.1).
  - **Spieler:** PSX.EXE @0x80073e94 (Datei 0x64694) = `{0,-1530,0,450,1530,450}`. @0x80031640 `lui v1,0x8007` / @0x80031644 `lw v1,16032(v1)` liest den Zeiger 0x80073ea0 = 0x80073e94, und @0x80031664 `sw v1,-13620(at)` schreibt ihn nach 0x800acacc = Spieler+0x78 (Spielerblock 0x800aca54, hp +0x9a = 0x800acaee).
- Mittig im 3200 breiten Gang lässt er Leon nicht vorbei.

Gemessen `lauf1`:
- **F1154:** Leon rennt (pad 4010) auf (−25440,−18148), Birkin steht bei (−23984,−18122). Abstand **1456 ≈ 1450**: Körperkontakt. Das ist das gemeldete „hängt bei x ≈ −25440".
- **F1169:** Biss (Sub 4), hp 100 → 90, Leon gepinnt.
- **F1214–F1244:** Birkin REPOSITION (Sub 8, Schritt 30) schiebt den gepinnten Leon von −25435 nach −26483, ohne Eingabe. Das ist das gemeldete „wanderte ohne Eingabe nach −x".

### 4.5 Tür ohne Strom zu Fuß erreicht (Lauf `ausweich6`, Skript `ausweich_skript.txt`)

Regulärer Türweg, danach nur Pad-Eingaben, **kein Messhaken in ROOM5080**:
- F929: stehen bei (−24095,−18092).
- F958: Birkin beginnt den Stoß (Sub 3) bei Abstand 3196 < 3200. Er steht während Stoß und Erholung (bis F1066).
- F969–F1035: Leon läuft unten über die Rauten-Kerbe (z bis −20737) um ihn herum, Mindestabstand > 2800. Dann nordöstlich in den Ost-Arm.
- F1078: an der Ostwand (−9709,−17767), **hp 100**.
- F1090 QUADRAT: **msg 2 offen, pauseflags FFFF0007, tuerse.log `F1090 raum=5080 nachricht=2 art=S weg=AOT satz=3(ZU_P) nr=1`.**
- F1175: Text geschlossen (pauseflags 00000007).
- F1207–F1228: Leon läuft rückwärts (pad 0040), x −9709 → −11227.

## 5 Ursache

- **Hänger in sub02: keiner.** Die Schleife wartet auf die Ankunft einer reinen Drehung (@0x800313d0/@0x800313f8). Der Port erreicht sie in 21 Bildern.
- **„Leon bewegt sich nicht mehr" / „kommt nicht zur Tür": belegte Ursache N1.** Den Kampf schaltet sub03 byte-getreu frei (Member_set @Datei 0x0083E). Trifft Birkin, pinnt der Port Leon, statt ihn durch die Opfer-FSM zu führen:
  - Port: 308 Bilder am Stück gepinnt (F1060–F1367, Lauf `ausweich1b`), danach Zyklen von 263 Bildern gepinnt zu 14 Bildern frei. Leon steht dabei trotz Eingabe fest, zum Beispiel F1062–F1146 bei (−23326,−19254) mit pad 4010.
  - Original: Die Halte-Phase endet ohne Tasteneingabe nach 19 Bildern (Zähler 75 @0x8011b288, −4 je Bild @0x8011b308, STAGE5). Danach folgen Aufstehen und Freigabe (Kommando 1 @0x8011b3c0). Die Belege stehen in Abschnitt 7, N1.
  - Das x ≈ −25440 aus der Übergabe ist der Körperkontakt (Abschnitt 4.4). Das „wanderte ohne Eingabe nach −x" ist Birkins REPOSITION, die den gepinnten Leon mitschiebt.
- **Vor der Folge: belegte Ursache N2.** Die Wandklemme hatte das falsche Band (Abschnitt 8.1). Birkin kam dadurch schon vor dem Generator durch die Außenwand. Blieb der Spieler im Westgang stehen, stand Birkin ab F471 (≈ 16 s nach Raumeintritt) im Rauten-Block. Ab F819 (≈ 27 s) biss er, und N1 hielt Leon fest. Dieser Teil des Symptoms ist mit dem Bau weg.
- Die Tür ohne Strom ist zu Fuß erreichbar (4.5), vor und nach dem Bau.

## 6 Änderung

**N2 gebaut** (Commit `fix(r30/room5080): Birkin-Wandklemme nimmt das Band aus +0x82 …`):
- `re15_port/engine/src/enemy_ai_common.c`, 0x30/0x36-Zweig von `re15_enemy_ai_run_all`: `re15_collision_constrain_enemy(…, e->y, 4u)` wird zu `re15_collision_constrain_contact_band(…, (int)e->floor, 4u, NULL, NULL)`. Das ist dieselbe Klemme mit dem Band aus +0x82, wie sie der Gorilla-Zweig schon benutzt. Es gibt keine neue Konstante. Die Belege stehen im Code-Kommentar und in Abschnitt 8.1.
- Der Endkampf-G5 (0x36 in ROOM5090/5091) läuft über sein eigenes Modul und ist nicht berührt.

**Riegel:** `re15_port/tests/unit/probe_r30_n_room5080.c`, registriert in `probes/r30_n_room5080.cmake`, ctest `unit_r30_n_room5080_birkin_wand`.
- Aufbau: ROOM5080 laden, SCD hochfahren (sub00 spawnt Birkin), der Spieler steht bei (−23350,−18200). Das ist die Lage aus dem Gegenprüfer-Lauf. Dann 2400 Bilder ohne Eingabe.
- A (Abdeckung): Spawn wie der Record, EMERGENCE gelaufen (grid 0x33 → 0x30), Birkin entfernt sich ≥ 5000 von der Spawnlage und kommt bis z ≤ −5554 an die Nordwand.
- B: in keinem Bild in der Rohfläche einer Band-0-Zelle, nie im Raum (z < −6654).
- C: Spieler-hp bleibt 100, kein Griff.
- D: Die Folge lief nicht, (5,32) und (3,48) bleiben 0.
- **Vor dem Bau ROT:** B1 2024 Bilder in einer Zelle (erstes Bild 376, Zelle #1 = @0x00320), B2 2024 Bilder im Raum, C hp min 0 bei 1447 Griff-Bildern.
- **Nach dem Bau GRÜN:** max. Abstand 7848, kleinstes z −5651, 0 Bilder in Zellen, hp 100, 0 Griff-Bilder.

**Suite** (Bau dieses Baums mit Fix, `ctest --test-dir re15_port/build --timeout 240`): **395/395 grün**, 407,6 s. Das sind die 394 Tests des Integrationsstands plus der neue Riegel. Nach der N3-Korrektur (nur ein Kommentar) wurde neu gebaut, und die 7 Birkin-Tests (`-R "unit_r30_n_room5080|birkin"`) sind 7/7 grün.

**N3 korrigiert:** Nur der Kommentar in `enemy_ai_common.c` (Birkin-Tod-Flag) nennt jetzt STAGE5 @0x8011aecc statt @0x8011ae10. Das Verhalten ist unverändert.

**Nicht gebaut:** N1. Die Begründung (Umfang) steht in Abschnitt 7.

**Werkzeuge** (`nachschliff-room5080_tools/`):
- `r30_5080_echtlauf.sh` (Zeitbasis jetzt im Kopf, siehe 4.0)
- `tuerweg_skript.txt`, `lauf1_skript.txt`, `ausweich_skript.txt` (die Eingaben der Läufe)
- `birkin_band_zensus.py` (Band je Birkin-Spawn gegen das Port-Band)
- `birkin_vor_folge.py` (Birkin-Weg, Wandkontakte und Spieler-hp aus einer state.log)

## 7 Befunde N1–N3 — nächster Weg

### N1 (erheblich, Port-Lücke, NICHT gebaut): Leon-seitige Opfer-FSM für Typ 0x30 fehlt

**Kein neuer Befund.** Der Port benennt die Lücke selbst als HONEST-OPEN (enemy_ai_common.c:11435-11436): „aca58 player-command FSM (grab cmd 5 / throw cmd 6 / knockdown cmd 2) is unported port-wide; the port uses s_player_grabbed (via the birkin_grab latch) for cmd 5/6 …". Neu sind hier die Messung, der Größenvergleich und die Zuordnung zum Nutzer-Symptom.

**Original:** Der Treffer im Stoß schreibt
- 801177d0 `sw v0,0x800aca58` mit v0 = (facing<<8)|5 → Spieler-Kommando 5;
- dazu 801177a8 die Blickrichtung, 801177b0 den Angreifer 0x800acbfc und 801177c0/d8 die Opfer-Bank 0x800acbcc/d0 (STAGE3; STAGE5 +0x814).

**Kommando 5** 0x80036834 und **6** 0x800368c0 verzweigen nach dem Angreifer-Typ. Sie lesen `lw v0,-770(a0+type*4)` bzw. `-514` und springen in die Tabellen @0x800ac758/@0x800ac858 [0x30]:
- STAGE3: 0x800ac818 = **0x8011a7c4** (Installer STAGE3_overlay.c:12748)
- STAGE5: 0x800ac818 = 0x800ac918 = **0x8011afd8** (STAGE5_overlay.c:13155/13168)

Das ist eine Spieler-FSM:
- vier Varianten über +5, Tabelle @0x8011f4a4 (STAGE3);
- je 14 Phasen über +6, Tabelle @0x8010048c.

Variante 0 (STAGE3, in Klammern STAGE5 = +0x814, selbst nachgelesen):
- **[0] @0x8011a840:**
  - Leons eigener Clip 0xd (8011a860), Bild 6, frac 3;
  - Rückstoß-Tempo +0x8c = 0x4b0 = 1200 (8011a884);
  - +0x93 |= 1;
  - SE 0x04010001.
- **[1] @0x8011a8c0:** Tempo −150 je Bild, min 400 (8011a8f0/904). Landung (+0x0 & 0x10) → [5].
- **[2]/[3] @0x8011a940/968:** Clip 0xe, Tempo −50.
- **[5] @0x8011aa00:** Clip 0xf, SE 0x04070001 + 0x04020001.
- **[7] @0x8011aa60 (STAGE5 @0x8011b274):**
  - +6 := 8 (@0x8011b284);
  - Zähler 0x800acaf0 = 0x4b = **75** (@0x8011b288 `ori v0,zero,0x4b`, @0x8011b298 `sh v0,-13584(at)`);
  - 0x113 = 275 bei hp < 30 (@0x8011b29c-2ac), +380 bei hp < 10 (@0x8011b2b0-2d0);
  - Opfer-Clip aus der Birkin-Bank 0x800acbcc/d0 (@0x8011b2d8-2e8).
- **[8] Halte-Phase (STAGE3 @0x8011aaf4, STAGE5 @0x8011b2f0-b328):**
  - `jal 0x80037024` (Tastenflanken);
  - Zähler −= 4 + 9 × Flanken (`sll v1,v0,3` / `addu v1,v1,v0` / **`addiu a0,a0,-4`** @0x8011b308 / `subu a0,a0,v1`);
  - Zähler < 0 → +6 := 9 (@0x8011b31c-328).
  - **Ohne Eingabe ist der Zähler nach 19 Bildern negativ** (75 − 19 × 4 = −1). Bei hp < 30 sind es 69 Bilder, bei hp < 10 164 Bilder.
- **[9]–[12]:** Aufstehen, Clips 0x10 / 0xb (STAGE5 @0x8011b334-3a8).
- **[13] Freigabe (STAGE3 @0x8011ab9c, STAGE5 @0x8011b3b0):**
  - @0x8011b3c0 `sw v0(=1),-13736(at)` = 0x800aca58 := 1 (Kommando 1, frei);
  - +0x93 &= 0xfe (@0x8011b3d4/3dc);
  - aca3c &= ~0x40 (@0x8011b3e0-3ec).
- **Schwanz @0x8011b3f0:** `jal 0x800245d8` mit a0 = 0x800 (Rückstoß-Schritt).

**Größenvergleich:**
- Original: Die Halte-Phase dauert 19 Bilder ohne Eingabe. Rückstoß [0]–[5] und Aufstehen [9]–[12] sind Clip-Längen der Opfer-Bank. Ihre Summe ist **nicht gemessen**, weil der Port diese Phasen nicht hat.
- Port: 308 Bilder gepinnt (F1060–F1367, `ausweich1b`), danach 263 zu 14. Kein Rückstoß, keine Opfer-Animation, kein Befreien durch Hämmern.
- Ohne Eingabe stirbt Leon im Port so nach etwa 10 Zyklen, ohne jede Reaktions-Animation.

**Modulgleichheit** (`birkin_modul_vergleich.py`):
- 0x80116230..0x8011b4d0 gegen STAGE5 +0x814: 5288 Worte, 5176 gleich, 112 reine Verschiebungen, **0 echte Abweichungen**.
- Die Opfer-FSM allein (835 Worte): ebenfalls 0 echte Abweichungen.

**Port:**
- Keine Stelle kennt 0x8011a7c4 oder 0x8011afd8.
- Stattdessen `s_player_grabbed = 1` bei jedem Treffer, dazu `birkin_grab`. Das wird **je Bild neu gesetzt** (enemy_ai_common.c `if (e->birkin_grab) s_player_grabbed = 1;` im Wurzel-Vorlauf).
- Gelöst wird es erst am Ende von Birkins GRAB-THROW (Sub 5 Phase 2, `e->birkin_grab = 0`).
- Leon steht dabei im Greif-Zweig (game_step_common.c `re15_player_is_grabbed()`): ohne Pad, ohne Rückstoß, ohne Clip, ohne Hämmern.

**Warum nicht in dieser Spur gebaut (Umfangsentscheidung, keine fehlende Ursache):**
- Die Ursache ist belegt. Der Port bräuchte aber eine neue Spieler-FSM mit vier Varianten zu je 14 Phasen. Dazu gehören der Rückstoß über 0x800245d8, die Opfer-Clips aus der Birkin-Bank 0x800acbcc/d0, das Hämmern über 0x80037024 und die Freigabe an [13] statt an Birkins Sub 5.
- Die Wurf-Seite (Kommando 6, Birkin Sub 5 Bild 0x2c) läuft über **dieselbe** Adresse 0x8011afd8 und müsste mit.
- Der Tausch berührt jeden Birkin-Raum (3070/3071, 3080, 5080/5081, 50E0/50F1) und den Endkampf-Pfad. Das ist ein eigener Auftrag der Birkin-Kampagne, kein Nachschliff einer Ermittlungsspur.
- **Nächster Weg:** 0x8011afd8 (alle vier Varianten) portieren wie `re15_player_victim_*` bei den Zombies. Den Pin durch Kommando 5/6 ersetzen, die Freigabe an [13] hängen. Riegel: Leon wird ohne Eingabe nach Halte-Phase + Clip-Längen frei, nicht erst nach 308 Bildern.

### N2 (erheblich, Port-Defekt, GEBAUT): Birkin-Wandklemme mit dem Band aus der Höhe

Siehe Abschnitt 8.1 (Messung, Original-Mechanismus, Ursache, Messung nachher) und Abschnitt 6 (Änderung, Riegel).

### N3 (gering, Zitat, GEBAUT): falsche STAGE5-Adresse im Port-Kommentar

`enemy_ai_common.c` (Birkin-Tod, `re15_game_flag_set(5, 0x1c, 1)`) zitierte „@0x8011a6b8 / @0x8011ae10". Die STAGE3-Adresse stimmt:
- 8011a690 `lui a0,0x800b` / 8011a694 `addiu a0,a0,4136` (= 0x800b1028), 8011a698 `ori a1,zero,0x1c`, 8011a6b8 `jal 0x8004ef90`.

In **STAGE5** steht an 0x8011ae10 ein Sprungtabellen-Zugriff (`lui at,0x8010` / 8011ae14 `addiu at,at,1204`). Der Setzer liegt dort bei **0x8011aecc** (= 0x8011a6b8 + 0x814): 8011aea4 `lui a0,0x800b` / 8011aea8 `addiu a0,a0,4136`, 8011aeac `ori a1,zero,0x1c`, 8011aecc `jal 0x8004ef90`. Korrigiert ist nur der Kommentar, das Verhalten bleibt unverändert. Die Memory-Notiz `reai-v2-endkampf-birkin` trägt dieselbe falsche Zahl. Sie liegt außerhalb des Baums und ist hier nicht geändert.

## 8 Birkin vor der Folge; nicht gemessen / offen

### 8.1 Birkin vor der Generator-Folge (N2) — KORREKTUR der ersten Fassung

**Die erste Fassung war falsch.** Dort stand, Birkin bleibe „außerhalb der Raumwände bei (−19107,−3529)". Das war nur eine Momentaufnahme, der weitere Weg war nicht gemessen. Der Gegenprüfer hat es mit dem Lauf `build/r30_pruef_n_room5080/tuerweg` gezeigt: regulärer Türweg, aber mit roher Zeitbasis (siehe 4.0). Das Skript läuft dabei schon in ROOM6010 an, Leon bleibt in ROOM5080 bei (−23350,−18200) stehen, und die Generator-AOT fällt nie.

**Messung vorher** (eigener Nachlauf `build/r30n5080_vor_birkin_dbg`, Bau vor dem Fix, `BASIS=roh`, zusätzlich `RE15_BIRKIN_DBG=1`). Über 2317 gemeinsame Zeilen ist `state.log` bitgleich zum Lauf des Gegenprüfers. Auswertung mit `birkin_vor_folge.py`, Bildzählung ab Raumeintritt:
- Birkin steht F1 bei (−18100,200), Höhe **y = −5600** über den ganzen Lauf (`birkin_dbg.log`: `pos=(-18100,-5600,200)` … `pos=(-20834,-5600,-8377)`).
- Bis etwa F140 EMERGENCE (Sub 9, Clip 16/13), grid 0x33 → 0x30. Danach Sub 1 WALK auf den Spieler zu.
- F382 steht er zum ersten Mal in der Rohfläche der Nordwand SCA @0x00320 (x −28961..−6841, z −8914..−6654) bei (−20362,−6666). Nichts klemmt ihn.
- Ab F471 steht er im Rauten-Block SCA @0x003C8 (x −26936..−19656, z −16509..−9229), insgesamt 1694 Bilder. Ab F804 steht er fest bei (−22788,−15772).
- Spieler: hp 90 ab F819, 10 am Laufende (F2164). Birkin wechselt zwischen Sub 4 (Biss) und Sub 5 (Wurf). Der Abstand beträgt etwa 2490 (Bissweite 0x9c4 = 2500, DECIDE @0x80117038 STAGE3).
- Die Folge lief nicht (kein Plc_dest in debug.log).

**Warum `re15_collision_constrain_enemy` ihn nicht klemmte** (Frage des Gegenprüfers):
- Der Port leitet das Band aus der Höhe ab: `re15_collision_band_from_y(y) = -(y / 0x708)` (re15_collision.c:186). Bei y = −5600 ergibt das **3**.
- Alle 80 SCA-Einträge von ROOM5080 tragen floor = 3, also Band `floor>>4` = **0** (`sca_5080.py`: 5 Gruppen × 16, alle „floor=3").
- `collision_constrain_impl` prüft `band != (e->floor >> 4)` streng (re15_collision.c:726). In Band 3 gibt es keine Zelle, also klemmt nichts.

**Original-Mechanismus (selbst disassembliert):**
- Birkin-Wurzel STAGE5 @0x80116cc0-cdc, jedes Bild nach dem Zustands-Dispatch:
  - 80116cc8 `ori a2,zero,0x4`
  - 80116ccc `lw v0,120(v0)` (+0x78 Box)
  - 80116cd4 `lhu a1,6(v0)` (Radius)
  - 80116cd8 `jal 0x8003b0a4` mit 80116cdc `addiu a0,a0,52` (+0x34)
- FUN_8003b0a4 nimmt das Band **aus dem Aktor**:
  - 8003b224 `lhu a0,0(s2)` (Zelle+10 = u1 | floor<<8), 8003b230 `sll v0,a0,16` / 8003b238 `srl v0,v0,28` (= floor>>4);
  - **8003b234 `lbu v1,130(a3)`** (a3 = aktueller Aktor, +0x82);
  - **8003b23c `bne v1,v0,…`** (Band ungleich: Zelle überspringen).
- +0x82 schreibt der Spawn: Sce_em_set 0x800420a0 (SCD-Tabelle @0x800744a8 [0x44] = @0x800745b8):
  - 800420f4 `addiu s2,a1,2`;
  - **800421c8 `lbu v0,2(s2)`** = pc[4], **800421d0 `sb v0,130(s0)`**;
  - dazu 80042164 `sb v0,9(s0)` = grid aus pc[3], 8004217c/88/94 x/y/z aus pc[8..13].
  - ROOM5080 sub00 @0x00746: `44 00 30 33 00 00 00 ff 4c b9 20 ea c8 00`. pc[4] bei **Datei 0x0074A = 0x00** → +0x82 = 0. Im Port übernimmt scd_vm.c das schon byte-getreu (`a->floor = t->pc[4]`).
- Niemand ändert +0x82 bis zur Folge:
  - Das Birkin-Modul hat keinen Store mit Offset 130 und keinen auf +0x38 (Scan über STAGE5 0x80116a44..0x8011bce4 und STAGE3 0x80116230..0x8011b4d0: nur `sw …,56(sp)`-Stapelzugriffe).
  - Von den 7 EXE-Stellen, die +0x82 schreiben (0x8001be54, 0x8002c6c4, 0x80040974, 0x80041228, 0x800421d0, 0x8005276c, 0x80052dd0), liegt nur eine in einem Callee des Moduls: FUN_8001bd60 @0x8001be54. Sie läuft nur bei y == −(+0x82 × 1800) (8001bde4 `lw v0,56(a0)` / 8001bdec `bne v0,v1`). Bei y = −5600 und +0x82 = 0 springt sie vorbei.
  - Member_set Index 18 (+0x82, Sprungtabelle @0x80010c8c [18] = 0x80041224) kommt in ROOM5080 nicht vor. Das einzige Member_set ist @0x0083E Index 0x0C = +0x9 (0x800411f4 `sb a2,9(a0)`).
- Keine Kollisionssperre: FUN_8003b0a4 bricht bei +0x0 & 8 ab (8003b12c `lw v0,0(v1)` / 8003b134 `andi v0,v0,0x8` / 8003b138-140 → return 0). Birkins INIT setzt Bit 8 nur für grid&0xf == 1 (STAGE5 801170b4-801170e8: `andi v0,v0,0xf` / `bne v0,v1(=1)` / **801170e4 `ori v0,v0,0x8`**). grid 0x33 hat Nibble 3 (→ +0x95 = 0x10, Sub 9 @0x80117094-a4).
- **Folge im Original:** Birkin läuft auch dort ab der Raumladung los (derselbe INIT → EMERGENCE → WALK). FUN_8003b0a4 klemmt ihn aber in Band 0 an der Nordwand. Die Parkregel aus ROOM5090 ist dafür **nicht** das Vorbild: Sie ist eine Port-Wahl nach RE2 (G5 bei (−32000,−32000), RE2 @0x801011d0-dc, enemy_ai_common.c:11500-11530) und gilt nur für Typ 0x36. RE1.5 parkt den 5080-Birkin nicht, es klemmt ihn.

**Zensus** (`birkin_band_zensus.py`, alle 13 Birkin-Spawns): Das Port-Band weicht nur bei den vier Spawns mit y = −5600 ab, alle grid 0x33 und pc[4] = 0:
- ROOM5080 @0x00746
- 5081 @0x00742
- 50E0 @0x00ACE
- 50F1 @0x009AC

Die übrigen 9 Spawns (3070/3071/3080/5090/5091 und die 0x10-Spawns in 50E0/50F1) haben y = 0, also band_from_y == pc[4] == 0. Für sie ändert der Fix nichts.

50E0 und 50F1 haben dieselbe Skriptform wie 5080 (`scd_dump_room.py`): Spawn außerhalb bei y = −5600, später Work_set, Pos_set bei y = −5600 im Raum, Speed_set/Add_speed (der Fall) und Member_set(0x0C,0x13). Die Stellen: 50E0 @0x00BD2 / @0x00BEE, 50F1 @0x00A98 / @0x00AB4. Dort lief derselbe Wanddurchgang, und der Fix schließt ihn gleich mit. Gemessen ist das nur für 5080.

**Messung nachher** (Bau mit Fix, derselbe Lauf, `build/r30n5080_nach_birkin_dbg`):
- Birkin steht in keinem Bild in der Nordwand oder im Rauten-Block. Das kleinste z ist −5651 (F347). Er gleitet an der Wand entlang nach Westen bis (−23350,−5636), genau nördlich über Leon, 2D-Abstand 12540.
- Spieler hp 100 über den ganzen Lauf, kein Biss.
- Der kleinste mögliche Abstand zwischen Birkin vor der Nordwand (z ≥ −6654 + 1000) und Leon im Nordarm (z ≤ −8914 − 450) ist 3710. Das liegt über jeder Trefferweite des Ports (Biss/Stoß-Treffer 0x9c4 = 2500). Durch die Wand trifft er also nicht. Ein Tackle-**Start** (dist < 0xed8 = 3800, rennender Spieler, @0x8011706c STAGE3) ist dort möglich, trifft aber nicht.
- Die Folge ist unverändert. `lauf1` (Debug-Sprung, `BASIS=roh`) ist nach dem Bau über 2279 Zeilen `state.log` bitgleich zum Lauf vor dem Bau. `ausweich6` (Türweg, `BASIS=spiel`, Tür zu Fuß) ist über 2355 Zeilen bitgleich, einschließlich F1090 msg 2 / Art S.
- Sonde `unit_r30_n_room5080_birkin_wand`: GRÜN (Abschnitt 6).

### 8.2 Nicht gemessen / offen

- **Das Original nicht dynamisch.** Das Klemmen an der Nordwand ist statisch belegt (Wurzel-Aufruf, Bandvergleich, Spawn-Byte, keine Schreiber, keine Sperre). Ein DuckStation-Savestate aus ROOM5080 nach etwa 25 s ohne Folge fehlt, es gibt keinen in `stage_saves/`. Der nächste Weg: `re15-room-capture` (Debug-Menü Stage 5 / Raum 08), 30 s warten, Savestate. Dann Birkin +0x34/+0x3c/+0x82 lesen, erwartet sind z ≈ −5650 und +0x82 = 0.
- **Nav-Steer 0x80039e7c** (Wurzel @0x80116bb8) ist im Port nicht portiert (HONEST-OPEN). Wie der Original-Birkin an der Wand entlanggleitet, ist deshalb nicht bildgenau.
- **Grid 0x13 trägt Bit 0x10.** Der Port liest das als Form-2 (Stoß/Biss starten bei Bild 0x14, @0x80117600/@0x80117ae8). Damit kommt das Trefferfenster 16 Bilder nach Stoßbeginn. Das folgt den Skript-Bytes. Einen Laufzeitvergleich mit dem Original gibt es nicht.
- **Bildgenaue Parität der Drehung** (Ankunft F856 oder F857) gegen das Original: nicht gemessen, für die Frage ohne Belang.
- **Die Summe der Opfer-FSM-Clip-Längen** (N1, Phasen [0]–[5] und [9]–[12]) ist nicht gemessen.
- **Nur Zustandsprotokolle.** Kein Bildbeweis (Framedump/gdigrab) — die Befunde sind Zustands-, keine Bildbefunde.
