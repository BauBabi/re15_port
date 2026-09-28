# Nachschliff Runde 30 — ROOM5080: „Leon bewegt sich nach der Generator-Folge nicht mehr"

Spur `room5080`, Zweig `r30/n-room5080`, Basis `cac33993`. Ermittlung. Nach der Nachbesserung gibt es einen Engine-Eingriff (N2, Abschnitt 6), nach der zweiten Nachbesserung (Spur birkin-frost) einen weiteren (N4, Abschnitt 9).

## Kurzfassung

| Behauptung | Ergebnis |
|---|---|
| „sub02 hängt in der Warteschleife hinter Plc_dest" | **widerlegt.** Die Schleife endet nach 21 Bildern. Die Folge gibt den Spieler frei: Pad-Sperre weg, Kommando 1. Gemessen im Debug-Sprung **und** auf dem regulären Türweg aus ROOM6010 (Abschnitt 4.1/4.2). |
| „Leon bewegt sich danach nicht mehr" | **echt, Ursache belegt: N1.** Birkin G1 (Typ 0x30) greift an, sobald sub03 ihn freischaltet. Trifft er, hält der Port Leon über `s_player_grabbed` fest, statt die Opfer-FSM des Originals zu fahren. Der Port hält ihn **308 Bilder** am Stück, danach in Zyklen von 263 Bildern. Das Original gibt ihn nach der Halte-Phase frei, die ohne Tasteneingabe **19 Bilder** dauert (Zähler 75, −4 je Bild), dazu kommen die Clip-Phasen davor und danach. Die Lücke steht schon als HONEST-OPEN im Port (enemy_ai_common.c:11435-11436). **Nicht gebaut, und das ist eine Umfangsentscheidung** (Abschnitt 7, N1). |
| „Tür ohne Strom (Art S) nur über RE15_FIRE_AOT erreichbar, nicht zu Fuß" | **widerlegt.** Echtlauf über den regulären Türweg: Leon lockt den Stoß aus 3200 Einheiten an und läuft unten um Birkin herum. Mit hp 100 erreicht er die Tür. msg 2 geht auf, dazu **1 × ZU_P** (tuerse.log `F1090 raum=5080 nachricht=2 art=S weg=AOT satz=3(ZU_P) nr=1`). Danach schließt der Text, und Leon läuft wieder (Abschnitt 4.5). |
| (Gegenprüfer, Nachbesserung) Birkin geht **vor** der Folge durch die Außenwand und beißt Leon | **bestätigt. Ursache (zweite Nachbesserung): N4, die fehlende Frost-Schranke — gebaut (Abschnitt 9).** Im Original läuft Birkin vor der Folge gar nicht: bei grid & 0x20 steigt seine Wurzel vor Dispatch und Klemme aus (STAGE5 @0x80116a88). ⛔ Die folgende erste Erklärung (N2) war **falsch**: Die Birkin-Wandklemme des Ports nahm das Band aus der Höhe: `band_from_y(-5600)` = 3, und in Band 3 liegt keine Zelle. ~~Das Original nimmt das Zustands-Byte +0x82, das der Spawn auf 0 setzt, und klemmt ihn an der Nordwand.~~ (Das Band aus +0x82 ist richtig, aber das Original erreicht die Klemme vor der Freigabe nie.) Vor dem Bau stand er im Echtlauf ab Bild 471 im Rauten-Block und biss Leon von hp 100 auf 10. Die Sonde fand ihn in 2024 von 2400 Bildern in einer Wandzelle, hp fiel bis 0. Nach N2 stand er in 0 Bildern in einer Zelle und glitt bei z −5636 an der Nordwand entlang (kein Original-Verhalten). **Nach N4 steht er bis zur Freigabe auf (−18100,−5600,200) in Sub 9**, gemessen im Echtlauf über 2175 Bilder, Leon behält hp 100 (Abschnitt 9.5). |

Befunde, einheitlich nummeriert (Abschnitt 7):
- **N1** Pin statt Opfer-FSM des Spielers. Belegte Ursache des Nutzer-Symptoms, nicht gebaut (Umfangsentscheidung). Die Lücke ist schon als HONEST-OPEN benannt.
- **N2** Birkin-Wandklemme mit falschem Band. Belegt und **gebaut**, mit Riegel `unit_r30_n_room5080_birkin_wand`. ⛔ Als Ursache des Symptoms vor der Folge falsch, die richtige ist N4.
- **N3** Falsche STAGE5-Adresse im Port-Kommentar zum Birkin-Tod-Flag. Nur der Kommentar war falsch, er ist **korrigiert**.
- **N4** (zweite Nachbesserung) Fehlende Frost-Schranke der Birkin-Wurzel und fehlender Wurzelaufruf beim Spawn. Belegt und **gebaut**, mit Riegel `unit_r30_birkin_frost` (alle 13 Birkin-Spawns). N2 ist seitdem ohne messbare Wirkung (9.2); der Code bleibt in der Original-Form.

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
- **Vor der Folge: belegte Ursache N4 (die fehlende Frost-Schranke, Abschnitt 9).** ⛔ Die erste Nachbesserung nannte hier N2; das war falsch. Ohne Schranke lief Birkin los, und die Wandklemme hatte zusätzlich das falsche Band (Abschnitt 8.1). Birkin kam dadurch schon vor dem Generator durch die Außenwand. Blieb der Spieler im Westgang stehen, stand Birkin ab F471 (≈ 16 s nach Raumeintritt) im Rauten-Block. Ab F819 (≈ 27 s) biss er, und N1 hielt Leon fest. Dieser Teil des Symptoms ist mit N4 weg: Birkin steht bis zur Freigabe still (9.5).
- Die Tür ohne Strom ist zu Fuß erreichbar (4.5), vor und nach dem Bau.

## 6 Änderung

> Nachbesserung 2: Der eigentliche Eingriff gegen „Birkin vor der Folge" ist **N4** (Frost-Schranke + Wurzelaufruf beim Spawn, Abschnitt 9.2). N2 unten bleibt im Code, hat seit N4 aber keine messbare Wirkung. Die Riegel-Beschreibung unten ist die der ersten Fassung; den Umbau (A2/A3 umgekehrt, Teil E) beschreibt 9.8.

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

> N4 (gebaut), N5 (Softlock ROOM3071, schon vor N4) und N6 (grid-4-Szenenmodus ROOM3080) stehen in Abschnitt 9. N1 ist dort mit dem neuen Ablauf neu gemessen (9.6).

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

> ⛔ **Nachbesserung 2: Auch diese Korrektur lag im Kern falsch.** Die Messungen „vorher" unten stimmen (so verhielt sich der **Port**). Falsch war der Original-Mechanismus: Die Birkin-Wurzel erreicht die Klemme FUN_8003b0a4 vor der Freigabe gar nicht, weil sie bei grid & 0x20 vorher aussteigt (STAGE5 @0x80116a7c/@0x80116a84/@0x80116a88 → 0x80116eb8, Abschnitt 9.1). Birkin läuft im Original vor der Folge nicht los und wird nicht geklemmt, er steht eingefroren. Die durchgestrichenen Sätze unten sind die Falschaussagen.

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
- ~~**Folge im Original:** Birkin läuft auch dort ab der Raumladung los (derselbe INIT → EMERGENCE → WALK). FUN_8003b0a4 klemmt ihn aber in Band 0 an der Nordwand.~~ **Richtig (9.1):** Der INIT läuft einmal beim Spawn (Sce_em_set @0x8004259c mit gelöschtem Bit), danach friert die Schranke @0x80116a88 die Wurzel ein, bis Member_set(0x0C,0x13) @0x0083E das Bit löscht. Die Parkregel aus ROOM5090 ist dafür **nicht** das Vorbild: Sie ist eine Port-Wahl nach RE2 (G5 bei (−32000,−32000), RE2 @0x801011d0-dc, enemy_ai_common.c:11500-11530) und gilt nur für Typ 0x36. ~~RE1.5 parkt den 5080-Birkin nicht, es klemmt ihn.~~ **Richtig:** RE1.5 parkt ihn nicht und klemmt ihn nicht, es friert ihn ein.

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

- **Das Original nicht dynamisch.** ~~Das Klemmen an der Nordwand ist statisch belegt (Wurzel-Aufruf, Bandvergleich, Spawn-Byte, keine Schreiber, keine Sperre).~~ ⛔ Falsch, siehe 9.1: statisch belegt ist das **Einfrieren** (Schranke @0x80116a88). Ein DuckStation-Savestate aus ROOM5080 nach etwa 25 s ohne Folge fehlt, es gibt keinen in `stage_saves/`. Der nächste Weg: `re15-room-capture` (Debug-Menü Stage 5 / Raum 08), 30 s warten, Savestate. Dann Birkin +0x34/+0x3c/+0x82 lesen. ~~Erwartet sind z ≈ −5650 und +0x82 = 0.~~ Erwartet (9.1) sind x/z = (−18100, 200), +0x9 = 0x33, +0x4 = 1, +0x5 = 9, +0x95 = 0x10.
- **Nav-Steer 0x80039e7c** (Wurzel @0x80116bb8) ist im Port nicht portiert (HONEST-OPEN). Wie der Original-Birkin an der Wand entlanggleitet, ist deshalb nicht bildgenau.
- **Grid 0x13 trägt Bit 0x10.** Der Port liest das als Form-2 (Stoß/Biss starten bei Bild 0x14, @0x80117600/@0x80117ae8). Damit kommt das Trefferfenster 16 Bilder nach Stoßbeginn. Das folgt den Skript-Bytes. Einen Laufzeitvergleich mit dem Original gibt es nicht.
- **Bildgenaue Parität der Drehung** (Ankunft F856 oder F857) gegen das Original: nicht gemessen, für die Frage ohne Belang.
- **Die Summe der Opfer-FSM-Clip-Längen** (N1, Phasen [0]–[5] und [9]–[12]) ist nicht gemessen.
- **Nur Zustandsprotokolle.** Kein Bildbeweis (Framedump/gdigrab) — die Befunde sind Zustands-, keine Bildbefunde.

## 9 Nachbesserung 2: die Frost-Schranke (Befund des Gegenpruefers, selbst nachgeprueft)

**Der Gegenpruefer hat recht, und Abschnitt 8.1 lag in seiner Kernaussage falsch.** Der Birkin in ROOM5080 laeuft im Original vor der Generator-Folge gar nicht los. Er wird auch nicht an der Nordwand geklemmt, denn die Wurzel erreicht die Klemme gar nicht. Bis zur Freigabe steht er auf seiner Spawnlage, mit grid 0x33, Zustand 1 und Sub 9. Der Port hatte diese Schranke im 0x30/0x36-Zweig nicht. N2 (Band aus +0x82) ist die Original-Form der Klemme und bleibt im Code. Seit N4 hat es aber keine messbare Wirkung mehr, weil Birkin nach dem Fall am Boden steht (9.2).

### 9.1 Belegblock (selbst disassembliert, `re15_disasm.py`, STAGE*.BIN roh ab 0x80100000, ohne Header)

**Registrierung der Wurzel.**
- STAGE5 (`STAGE5_overlay.c:13134`): `_DAT_80072c6c = 0x80116a44`. Das ist 0x80072bac + 0x30*4, also Typ 0x30. STAGE5 registriert **kein** 0x36, die Adresse 0x80072c84 bleibt dort unbeschrieben.
- STAGE3: @0x8011cf40 `addiu v0,v0,25136` (= 0x80116230), dann @0x8011cf48 `sw v0,11372(at)` (0x80072c6c, Typ 0x30) und @0x8011cf50 `sw v0,11396(at)` (0x80072c84, Typ 0x36).

**Die Schranke am Wurzelanfang.**

| | STAGE5 (0x80116a44) | STAGE3 (0x80116230) |
|---|---|---|
| Pause | @0x80116a4c `lw v0,-13760(v0)` (g_pauseflags 0x800aca40), @0x80116a50 `lui v1,0x2000`, @0x80116a64 `and`, **@0x80116a68 `bne v0,zero,0x80116eb8`** (Bytes `13 01 40 14`) | @0x80116238 / @0x8011623c / @0x80116250, **@0x80116254 `bne v0,zero,0x801166a4`** (`13 01 40 14`) |
| Frost | @0x80116a74 `lw a0,-14460(a0)` (g_entity 0x800ac784), **@0x80116a7c `lbu v0,9(a0)`**, **@0x80116a84 `andi v0,v0,0x20`**, **@0x80116a88 `bne v0,zero,0x80116eb8`** (`0b 01 40 14`) | **@0x80116268 `lbu v0,9(a0)`**, **@0x80116270 `andi v0,v0,0x20`**, **@0x80116274 `bne v0,zero,0x801166a4`** (`0b 01 40 14`) |
| Sprungziel | @0x80116eb8-0x80116ef0: `lh a1,442(v0)` (+0x1ba) / **@0x80116ecc `jal 0x8001b064`** mit @0x80116ed0 `addiu a0,a0,176` (+0xb0), dann nur Register-Restore und `jr ra` @0x80116eec | @0x801166a4-0x801166dc: dasselbe, **@0x801166b8 `jal 0x8001b064`**, `jr ra` @0x801166d8 |

Das Sprungziel ist zugleich der gemeinsame Schwanz des normalen Pfads (STAGE5 @0x80116ea0-eb4 `sw v0,472(v1)`, danach faellt der Pfad nach 0x80116eb8 durch). Die Schranke ueberspringt also alles davor:
- den Abstand zum Spieler (@0x80116ad8 SquareRoot0) und FUN_8001bd60 (@0x80116af0);
- die Navigation 0x80039e7c (@0x80116bb8) und die Zaehler +0x1de/+0x1df (@0x80116bcc-c40);
- den **Zustands-Dispatch** ueber die Tabelle @0x8011fe48 (@0x80116c60 `addiu at,at,-440` / @0x80116c70 `jalr v0`);
- den Bild-Takt 0x8001b4e4 (@0x80116c78) und die Kontaktloeschung 0x8002b498 (@0x80116ca4);
- die Koerperstoesse 0x8002aec4 und 0x8002b544 (@0x80116cb0 / @0x80116cb8);
- die **SCA-Klemme `jal 0x8003b0a4` @0x80116cd8**, den Frame-SE 0x8001b38c (@0x80116ce0) und die Mutationspruefung (@0x80116cf4-e9c).

**Was FUN_8001b064 tut:** Es zeichnet nur den Bodenschatten.
- @0x8001b0e0 `lh a0,106(v0)` (+0x6a Blickrichtung) / @0x8001b0e4 `jal 0x800659d0` (RotMatrixY);
- danach `jal 0x80067a28` / `0x80022da0` / `0x80066960` / `0x80068328` / `0x80014368` / `0x80065d10` (Matrix, Projektion, Primitiv);
- @0x8001b328 `lbu a1,-13772(a1)` (Puffer-Index 0x800aca34) und @0x8001b350-360 `lw`/`and`/`or`/`sw v1,0(a0)` = das Einhaengen in die Ordnungstabelle.
- Einen Store auf das Entity gibt es nicht (Katalog `RE15_FUN_CATALOG.md:221`: „character floor SHADOW draw").

Im Port zeichnet `platform/pc/main.c` (Block „RE1.5 character shadow for this NPC — FUN_8001b064") den Schatten fuer jeden Aktor auf der Buehne, unabhaengig vom KI-Tick. Das Gegenstueck existiert also schon. Ein eingefrorener Birkin braucht im KI-Zweig **nichts** zu tun, damit er sichtbar in seiner Pose steht. Das Modell zeichnet der Renderer aus motion/anim_frame. Weil 0x30/0x36 in `re15_type_self_advances_anim` stehen (player_common.c), schaltet der globale Bild-Takt sie nicht weiter, genau wie im Original, wo 0x8001b4e4 nicht laeuft.

**Der einmalige Wurzelaufruf beim Spawn** (Sce_em_set 0x800420a0, SCD-Tabelle @0x800744a8 [0x44]):
- @0x80042564 `lbu v1,9(s0)`, **@0x8004256c `andi v0,v1,0xdf`**, **@0x80042570 `sb v0,9(s0)`** (Bytes `df 00 62 30 09 00 02 a2`): Bit 0x20 wird geloescht.
- @0x80042578 `sw s0,-14460(at)` (g_entity := das neue Entity).
- @0x8004257c-0x8004259c `lbu v0,8(s0)` / `sll v0,v0,2` / `addiu at,at,11180` (0x80072bac) / `lw v0,0(at)` / **`jalr v0`**: die Wurzel des Typs laeuft einmal.
- Im Delay-Slot @0x800425a0 `andi s4,v1,0x20` (`20 00 74 30`) wird das alte Bit gemerkt.
- @0x800425a4 `lh v0,16(s2)` = pc[18..19]. Nur wenn das ungleich 0 ist, folgt ein zweiter Aufruf mit +0x4 = 4 (@0x800425b8-f4). Alle 13 Birkin-Records tragen pc[18..19] = `00 00` (Zensus 9.3), der zweite Aufruf entfaellt also.
- **@0x80042604 `or v0,v0,s4`**, **@0x80042608 `sb v0,9(s0)`** (`25 10 54 00 09 00 02 a2`): Das Bit wird zurueckgesetzt.

Beim Spawn laeuft die Wurzel also genau einmal mit geloeschtem Bit durch den INIT (STAGE3 0x801166e0, STAGE5 +0x814):
- @0x801166f8 `sb v0(=1),4(v1)`: Zustand 1;
- @0x80116710 / @0x80116728: +0x1bc/+0x1be = Spielerlage;
- @0x8011683c / @0x8011684c / @0x8011685c: +0x94 / +0x95 / +0x8f = 0;
- @0x8011686c-78 `lbu v0,9(a0)` / `andi v0,v0,0xf` / `bne v0,v1(=3)`; bei Nibble 3 **@0x80116880 `sb v0(=0x10),149(a0)`** und **@0x80116890 `sb s0(=9),5(v0)`**: Sub 9 mit +0x95 = 0x10;
- bei Nibble 1 @0x801168b8 `sb v0(=0xa),5(a0)` (Sub 10) und @0x801168d0 `ori v0,v0,0x8`.

Danach steht der Birkin eingefroren in Zustand 1 / Sub 9 (bzw. Sub 10 bei grid 0x21 oder Sub 0 bei grid 0x24), bis das Bit geloescht wird.

**Wer das Bit loescht:** Member_set (SCD 0x34) mit Index 0x0C schreibt +0x9 (Sprungtabelle @0x80010c8c, @0x800411f4 `sb a2,9(a0)`). Die Datei-Offsets je Raum stehen in 9.3.

**Vorbild im Port:** Der Gorilla (0x27) hat die Schranke. Original STAGE1 @0x80116dec `lbu v0,9(v1)` / @0x80116df4 `andi v0,v0,0x20` / @0x80116df8 `bne v0,zero,0x80116e88`, im Port `re15_maggot_ai_tick` erste Zeile `if (e->grid_id & 0x20) return;`. Weitere Roots mit derselben Schranke im Port: der Zombie-Root `re15_enemy_ai_live_tick` (@0x80100450-5c) und die Zombie-Girl-Wurzel (@0x8010a8f4-900).

**Pausebit 0x20000000 im Port:** `game_step_common.c` ruft `re15_enemy_ai_run_all` nur bei `!(g_re15_pauseflags & RE15_PAUSE_AI)` auf (Beleg-Kommentar dort, Zombie-Root @0x8010042c-3c). Fuer alle Roots zusammen ist die Pause-Haelfte der Schranke damit schon verdrahtet. Der Birkin-Zweig prueft zusaetzlich `s_ai_paused`, wie die anderen Roots.

### 9.2 Bau (N4)

`re15_port/engine/src/enemy_ai_common.c`:
- **`re15_birkin_root`** ist der bisherige 0x30/0x36-Zweig von `re15_enemy_ai_run_all` (KI-Tick, b544-Stöße, Wandklemme). Davor steht jetzt die Schranke:
  - `s_ai_paused || (g_re15_pauseflags & RE15_PAUSE_AI)` für @0x80116a68 / @0x80116254;
  - `grid_id & 0x20` für @0x80116a7c-a88 / @0x80116268-274.
  - Steigt die Wurzel dort aus, tut der KI-Zweig nichts. Das Gegenstück von FUN_8001b064 ist der Schatten im Renderer, der für jeden Aktor gezeichnet wird (9.1). Die Pose bleibt stehen, weil 0x30/0x36 ihr Bild selbst takten.
- **`re15_enemy_spawn_root`** bildet Sce_em_set @0x8004256c-@0x80042608 nach: Bit 0x20 löschen, Wurzel einmal rufen, altes Bit zurücksetzen.
  - Aufgerufen wird es aus `op_sce_em_set` (scd_vm.c) nach dem Aufsetzen des Aktors.
  - Ausgenommen ist der G5 in ROOM5090/5091 (Typ 0x36, RE2-Modul `enemy_ai_boss_g5.c`), weil dieses Modul bei grid == 0x13 scharf wird.
  - Andere Gegnertypen bekommen den Spawn-Aufruf nicht (OFFEN).
- Die 5090-Parkregel ist **nicht entfernt**, nur als unerreichbar kommentiert (9.4).

**N2 nach N4, gemessen:** Ich habe N2 testweise zurückgedreht (Band aus `e->y`, nicht committet) und `probe_r30_birkin_frost` über alle 13 Spawns gefahren. Die Ausgabe ist **byte-gleich** zu der mit N2. Der Grund: Vor der Freigabe erreicht die Wurzel die Klemme nicht. Danach steht Birkin am Boden, denn der Fall ist Pos_set y −5600 (ROOM5080 @0x00822) + Speed_set(1,400) (@0x0082E `2f 01 90 01`) × For 14 (@0x00832 `0d 00 06 00 0e 00`) = y 0, gemessen y 0 im Freigabe-Bild. `band_from_y(0)` = 0 = +0x82. N2 bleibt als Original-Form im Code, der Kommentar sagt das jetzt.

### 9.3 Messung je Birkin-Spawn (alle 13, Zensus `birkin_band_zensus.py`)

Sonde `probe_r30_birkin_frost` (ctest `unit_r30_birkin_frost`, Rahmen wie `probe_r30_n_room5080`):
- Raum laden, SCD hochfahren, Spieler auf das Türziel des Raums setzen (Door_aot_set der Nachbarräume: ROOM4000 @0x01104 → 3070, ROOM3070 @0x03302 → 3080, ROOM6010 @0x00FCE → 5080, ROOM50D0 @0x0121A/@0x011FA → 50E0/50F0, ROOM5090 @0x0108E).
- 300 Bilder warten.
- Dann auslösen:
  - Event-AOT über `re15_aot_fire_slot`, mit dem Spieler in der Mitte des AOT-Rechtecks;
  - 3080 startet die Folge selbst (sub00 @0x007E8 Evt_exec sub02);
  - 5090/5091: nach Westen laufen bis Kamera 12 (sub01 @0x1290 Cmp work[0x0A]==12).
- Texte werden mit Quadrat bestätigt.
- (b) ist per Thread-PC-Klammer vor/nach dem SCD-Tick gemessen: Der Thread, der das grid kippt, hat im selben Tick den Datei-Offset der Tabelle überschritten.
- „Bild" zählt ab Raumladung.
- „vorher" ist der Engine-Stand `48531e99` (N2 ohne N4, per Stash zurückgesetzt) mit derselben Sonde. „nachher" ist mit N4 (`122d6580`).

| Raum / Spawn | Typ, grid | Freigabe (Opcode @Datei) | vorher (a) / (b) / (c) | nachher (a) / (b) / (c) |
|---|---|---|---|---|
| 3070 @0x033CE | 0x30, 0x33 | Member_set(0x0C,0x13) @0x0358E (sub02 ← AOT Platz 2 @0x033E2) | (a) **299/300 Bilder verletzt**: ab Bild 2 EMERGENCE auf (30000,30000), dann WALK / (b) Bild 613 @0x0358E / (c) kein EMERGENCE mehr, sofort WALK, Treffer Bild 1019; Folge endet Bild 776 | (a) 0/300 / (b) Bild 613 @0x0358E / (c) EMERGENCE 613 → WALK 739 → Treffer 1159; Folge endet 776 |
| 3070 @0x0340C | 0x30, 0x10 (Flag (4,215)=1) | — (kein Bit 0x20) | INIT erst im ersten KI-Tick (Spawn-Bild st 0); WALK 2, Treffer 59 | INIT im Spawn-Bild (st 1 / Sub 0); WALK 2, Treffer 58 |
| 3071 @0x03434 | 0x30, 0x21 | Member_set(0x0C,0x01) @0x03645 (sub02 ← AOT Platz 2 @0x0344C) | (a) **299/300**: ab Bild 2 Sub 10 (Ansturm) / (b) Bild 802 @0x03645 / (c) trifft den gesperrten Spieler ab Bild 802 (hp 80); **Folge endet nie** | (a) 0/300 / (b) Bild 802 @0x03645 / (c) Sub 10 → 3 → 1, WALK 997, bleibt bei (−28058,−11154) hängen, kein Treffer; **Folge endet nie** (9.7, N5) |
| 3071 @0x0347A | 0x30, 0x10 (Flag (4,75)=1) | — | INIT im KI-Tick; Treffer 59 | INIT im Spawn-Bild; Treffer 58 |
| 3080 @0x007D4 | 0x36, 0x24 | Member_set(0x0C,0x04) @0x00934 (sub02 ← sub00 @0x007E8) | (a) **299/300**: ab Bild 2 Sub 4/1 / (b) Bild 367 @0x00934 / (c) Sub 4/1 im Wechsel, kein Treffer; Folge endet (Tür) Bild 907 | (a) 0/300 / (b) Bild 367 @0x00934 / (c) wie vorher (Szenenmodus grid 4 nicht portiert, 9.7 N6); Folge endet 907 |
| 5080 @0x00746 | 0x30, 0x33 | Member_set(0x0C,0x13) @0x0083E (sub03 ← sub02 @0x007EC ← AOT Platz 1 @0x0072E) | (a) **299/300** / (b) Bild 775 @0x0083E / (c) kein EMERGENCE, WALK 775, Treffer 1096; Folge endet 802 | (a) 0/300 / (b) Bild 775 @0x0083E / (c) EMERGENCE 775 → WALK 901 → Treffer 1220; Folge endet 802 |
| 5081 @0x00742 | 0x30, 0x33 | Member_set(0x0C,0x13) @0x00836 (sub03 ← sub02 @0x007E4) | wie 5080 (Freigabe @0x00836) | wie 5080 (Freigabe @0x00836) |
| 5090 @0x0124A | 0x30 → Port 0x36 (G5), 0x33 | Member_set(0x0C,0x13) @0x0130A (sub04 ← sub01 @0x1296) | (a) 0/300 (G5 unscharf) / (b) Bild 454 @0x0130A / (c) scharf (x −9000) 454, Treffer 1962; Folge endet 495 | **byte-gleich** zu vorher |
| 5091 @0x01232 | 0x30 → Port 0x36 (G5), 0x33 | Member_set(0x0C,0x13) @0x012EE (sub04) | wie 5090 (@0x012EE) | **byte-gleich** zu vorher |
| 50E0 @0x00ACE | 0x30, 0x33 | Member_set(0x0C,0x13) @0x00BEE (sub04 ← sub02 @0x00B8E ← AOT Platz 1 @0x00ABA) | (a) **299/300** / (b) Bild 501 @0x00BEE / (c) kein EMERGENCE, WALK 547, Treffer 790; Folge endet 528 | (a) 0/300 / (b) Bild 501 @0x00BEE / (c) EMERGENCE 501 → WALK 627 → Treffer 870; Folge endet 528 |
| 50E0 @0x00AEE | 0x30, 0x10 (Flag (3,61)=1) | — | INIT im KI-Tick; WALK 2, kein Treffer in 5000 Bildern | INIT im Spawn-Bild; sonst gleich (Nav-Steer fehlt, 9.7) |
| 50F1 @0x009AC | 0x30, 0x33 | Member_set(0x0C,0x13) @0x00AB4 (sub04 ← sub02 @0x00A54 ← AOT Platz 1 @0x00994) | (a) **299/300** / (b) Bild 368 @0x00AB4 / (c) kein EMERGENCE, WALK 368, Treffer 662; Folge endet 383 | (a) 0/300 / (b) Bild 368 @0x00AB4 / (c) EMERGENCE 368 → WALK 494 → Treffer 804; Folge endet 383 |
| 50F1 @0x009D4 | 0x30, 0x10 (Flag (3,61)=1) | — | INIT im KI-Tick; WALK 2, kein Treffer | INIT im Spawn-Bild; sonst gleich |

Befunde aus der Tabelle:
- **Die Freigabe wird in allen 9 eingefrorenen Spawns im Port erreicht**, jeweils durch genau den Member_set am Datei-Offset der Tabelle. Keiner der Räume steht also für immer vor der Freigabe. Die Freigabe-Bilder sind vorher und nachher gleich: Die Schranke ändert das Skript-Timing nicht.
- Nach der Freigabe läuft in 3070/5080/5081/50E0/50F1 **zuerst EMERGENCE** (Clip 0x10), dann WALK, dann der Angriff. Vorher war EMERGENCE schon vor der Folge verbraucht.
- **Softlock gefunden, aber nicht durch N4: ROOM3071** (9.7, N5). Die Folge endet dort nie, vorher wie nachher.

### 9.4 Die 5090-Parkregel und der Endkampf

- Die Parkregel (`re15_birkin_ai_tick`, Typ 0x36, grid == 0x33, Lage Spawn oder (−32000,−32000)) war **schon vor N4 von keinem ausgelieferten Raum erreichbar**:
  - 0x36 in ROOM5090/5091 läuft über den G5-Zweig, der in `run_all` vor dem 0x30/0x36-Zweig steht.
  - Der einzige andere 0x36-Spawn (ROOM3080 @0x007D4) trägt grid 0x24.
- Mit N4 ist sie doppelt unerreichbar, denn 0x33 & 0x20 steigt vorher aus. Sie kollidiert nicht mit der Schranke. Sie ist **nicht entfernt**, nur kommentiert.
- `test_birkin_ai` (4) hatte sie synthetisch gepinnt (Raum-Id 0). Der Fall pinnt jetzt das Einfrieren: Zustand 0, Spawnlage unverändert, nach grid 0x13 INIT → Sub 9.
- **Der Endkampf ist unberührt:**
  - Der G5-Zweig und `enemy_ai_boss_g5.c` sind nicht geändert, und der Spawn-Aufruf steigt für 0x36 in 5090/5091 aus.
  - Gemessen: 13 G5-/Birkin-/5090-Sonden liefern vor und nach N4 **byte-gleiche Ausgaben**: probe_5090_birkin, probe_5090_birkin_verify, probe_5090_lang, probe_g5_boss, probe_g5_tentakel, probe_p2_birkin_g5, probe_p3_birkin_rest, probe_r16_birkin_g5, probe_r16_sk_birkin_g5, probe_r17_birkin_1zu1, probe_r18_birkin_rest, probe_r20_birkin_push, probe_r21_tentakel_schub.
  - `probe_g5_boss` deckt Auftritt, Zug, Biss und Tod ab: „Intro: u −9000 → 10612 … Tod: Clip10=1, Absink-y max=2952 (Soll GENAU 2952)".
  - Der Abspann-Pfad ist nicht eigens gefahren. Kein Code dorthin ist geändert.

### 9.5 Echtlauf (echte re15_pc.exe, Bau `5bc08f83`)

Treiber `r30_5080_echtlauf.sh`, Auswertung `birkin_frost_echtlauf.py`. Die State-Log trägt jetzt `gr=` (Griff-Kanal des Ports).

| Lauf | vorher (ohne N4) | nachher (N4) |
|---|---|---|
| Türweg mit Folge (`RAUM=6010`, `BASIS=spiel`, `tuerweg_skript.txt`, `RE15_FIRE_AOT=2@60#6010`) | `r30n5080_tuerweg` (Bau vor N2 und N4): 874/875 Bilder vor der Freigabe nicht eingefroren (Spawn-Bild st 0, danach läuft er); Freigabe F876; kein EMERGENCE danach, WALK F876; erster Treffer F1183, hp min 10 | `echt_tuerweg`: **875/875 Bilder eingefroren** (ab F861 auf der sub03-Lage (−18100,−17800), weiter grid 0x33 / Sub 9 / Clip 0 / +0x95 0x10); Freigabe F876 (grid 0x33 → 0x13); **EMERGENCE F876 → WALK F1002 → erster Treffer F1322**; hp min 40 |
| Gegenprüfer-Lauf, Folge nie (`BASIS=roh`) | vor N2: ab F471 im Rauten-Block, Biss ab F819, hp 10. Mit N2: gleitet an der Nordwand bis (−23350,−5636) | `echt_gegenpruefer`: **2175/2175 Bilder eingefroren auf (−18100,200)**, grid 0x33, Sub 9; Leon hp 100; nie in einer Wandzelle |

### 9.6 N1 mit dem neuen Ablauf neu gemessen (nicht gebaut)

| Messung | Erster Treffer | Griff-Läufe |
|---|---|---|
| Sonde `probe_r30_n_room5080` Teil 2, Spieler ohne Eingabe, **nach N4** | 918 Bilder nach dem Auslösen | **142 Bilder gegriffen, 14 frei**, im Zyklus (8 von 13 Läufen aufgelistet, alle gleich) |
| dieselbe Sonde **vor N4** | 784 | 142 : 14, gleich |
| Echtlauf `echt_tuerweg` nach N4 (keine Eingabe nach dem Skript) | F1322 | 5 volle Läufe **142 : 14** + einer läuft am Ende |
| alte Zahl (Abschnitt 5/7, `ausweich1b`, mit Pad-Eingabe) | F1060 | 308 am Stück, dann 263 : 14 |
| Original (statisch, STAGE5) | — | Halte-Phase **19 Bilder** ohne Eingabe (Zähler 75 @0x8011b288, −4 je Bild @0x8011b308), dazu die Clip-Phasen [0]–[5] und [9]–[12] (Summe nicht gemessen) |

- N4 ändert an N1 nichts außer dem Zeitpunkt des ersten Treffers. Birkin muss jetzt erst EMERGENCE spielen und aus der Raummitte (−18100,−17800) anlaufen.
- Der Pin selbst ist unverändert: Ohne Eingabe hält der Port Leon 142 Bilder, das Original 19 Bilder plus Clip-Phasen.
- N1 bleibt die belegte Ursache für „Leon bewegt sich nach der Folge nicht mehr" und ist weiter nicht gebaut (Abschnitt 7).

### 9.7 Nebenbefunde der Messung (nicht gebaut, Umfang)

- **N5 SOFTLOCK ROOM3071 (schon vor N4, durch N4 nicht verursacht).**
  - Nach der Freigabe @0x03645 wartet sub02 in `While Ck(5,31)==0` (@0x0364D-0x03657) und danach in `While Ck(5,30)==0` (@0x03665).
  - Setzer von (5,0x1f)/(5,0x1e) gibt es in STAGE3 nur im Birkin-CHARGE-COMBO 0x80119524 (Sub 10): @0x80119658 `jal 0x8004ef90` mit a0 = 0x800b1028 (Bank 5) / a1 = 0x1f, gesetzt bei Zähler +0x9c == 0x46 (@0x80119640-648), und @0x801197b4 mit a1 = 0x1e. In STAGE5 liegen sie bei @0x80119e6c / @0x80119fc8. Das ist ein Byte-Scan über beide Overlays nach `jal 0x8004ef90` mit `ori a1,zero,0x1e/0x1f`.
  - Der Port-Sub 10 ist kompakt: Er setzt die Flags nie und hat auch die Kollisionssperre +0x0 | 8 nicht (INIT @0x801168d0).
  - Folge: (1,27)/(2,7) bleiben gesetzt, und Leon ist für immer gesperrt, vorher wie nachher gemessen.
  - Vor N4 stürmte Birkin ab Bild 2 auf den gesperrten Spieler los. Nach N4 bleibt er nach Sub 10 → 3 → 1 bei (−28058,−11154) an der Kulisse hängen.
  - **Nächster Weg:** CHARGE-COMBO 0x80119524 mit allen 14 Phasen portieren (Sprungtabelle @0x80100434, STAGE3), mit Flag-Setzern und Sperre.
- **N6 ROOM3080: der grid-4-Szenenmodus fehlt.**
  - Die BRAIN wählt die Modus-Tabelle über das grid-Nibble: @0x80116d90 `lbu v0,9(v0)` / `andi 0xf` / @0x80116da4 `addiu at,at,-4444` (0x8011eea4) / @0x80116db4 `jalr`.
  - [0..3] = 0x80116dcc (die DECIDE/ACT-Tabellen @0x8011eeb8/@0x8011eef0). **[4] = 0x80116e48** mit den Tabellen @0x8011ef28 / @0x8011ef2c: DECIDE[0] = 0x80118cf8 (`jr ra`), ACT[0] = 0x80118d00, eine 15-Phasen-Choreografie (Sprungtabelle @0x801003dc, SE 3/6/8). Das ist kein Angriff.
  - Der Port fährt für grid 4 die normale HUB. Mit dem 0x36-Masse-Vorlauf beißt Birkin in der Schnittfolge ins Leere (Sub 4/1 im Wechsel, kein Treffer). Vorher wie nachher, die Folge endet über die Tür.
- **grid-0x10-Spawns in 50E0/50F1** (Wiederbetreten nach dem Kampf): Birkin läuft, erreicht den Spieler am Türziel aber in 5000 Bildern nicht, weil Nav-Steer 0x80039e7c fehlt (HONEST-OPEN). Das ist kein Frost-Thema, vorher wie nachher gleich.
- **Gorilla-Schranke im Port nur halb** (Beobachtung):
  - Das Original springt @0x80116df8 nach 0x80116e88, also hinter die Klemme @0x80116e70.
  - Der Port steigt nur aus `re15_maggot_ai_tick` aus. `run_all` fährt für einen eingefrorenen Gorilla trotzdem `re15_enemy_body_push_tail` und `re15_enemy_sca_clamp_band`. Nicht geändert.

### 9.8 Riegel, Suite

- `unit_r30_birkin_frost` (neu, `probes/r30_birkin_frost.cmake`): 13 Spawns, (a)/(b)/(c) wie 9.3. **Vor N4 ROT (29 Riegel verletzt), nach N4 GRÜN.** Den Angriff verlangt der Riegel nur, wo das Original ihn hat und der Port ihn leisten kann. Für 3071 (N5), 3080 (N6) und die grid-0x10-Spawns in 50E0/50F1 ist er mit Begründung ausgenommen und wird ausgegeben.
- `unit_r30_n_room5080_birkin_wand` (umgebaut):
  - A2/A3 sind umgekehrt: In jedem der 2400 Bilder ohne Folge ist Birkin eingefroren, er bewegt sich nicht.
  - Neuer Teil E: Freigabe durch @0x0083E, zuerst EMERGENCE (Bild 473 nach dem Auslösen), dann WALK (599), y 0 nach dem Fall, nie in einer Wandzelle. Dazu die N1-Messung.
  - **Vor N4 ROT** (A2 2399 Bilder verletzt, A3 Abstand 7848, E2 kein EMERGENCE), **nach N4 GRÜN**.
- `unit_birkin_ai`: INIT über `re15_enemy_spawn_root`, neuer Frost-Fall (1b). Fälle, die frei laufen sollen, haben grid 0x13 statt 0x33 bzw. 0x01 statt 0x21. (4) friert statt zu parken.
- **Suite 396/396 grün** (394 Integrationsstand + die 2 Riegel dieses Zweigs: `unit_r30_n_room5080_birkin_wand`, `unit_r30_birkin_frost`), 440 s, `ctest --test-dir re15_port/build --timeout 240`, Bau `122d6580`.

**Werkzeuge** (`nachschliff-room5080_tools/`): `birkin_frost_echtlauf.py` (Echtlauf-Auswertung), `birkin_sonden_lauf.sh` (baut und fährt die 15 Birkin-/G5-Sonden, eine Ausgabe je Datei, für den Vorher/Nachher-Vergleich).
