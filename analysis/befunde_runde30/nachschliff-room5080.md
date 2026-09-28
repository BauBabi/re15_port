# Nachschliff Runde 30 — ROOM5080: „Leon bewegt sich nach der Generator-Folge nicht mehr"

Spur `room5080`, Zweig `r30/n-room5080`, Basis `cac33993`. Ermittlung, **kein Engine-Eingriff**.

## Kurzfassung

| Behauptung des Gegenprüfers | Ergebnis |
|---|---|
| „sub02 hängt in der Warteschleife hinter Plc_dest" | **widerlegt.** Die Schleife endet nach 21 Bildern. Die Folge gibt den Spieler frei: Pad-Sperre weg, Kommando 1. Gemessen im Debug-Sprung **und** auf dem regulären Türweg aus ROOM6010. |
| „Leon bewegt sich danach nicht mehr" | Das ist **der Birkin-Kampf**, den sub03 freischaltet. Birkin G1 (Typ 0x30) versperrt den Gang mit seinem Körperzylinder (r 1000 + 450). Trifft er, **pinnt** der Port Leon 263–308 Bilder am Stück (Nebenbefund 2). |
| „Tür ohne Strom (Art S) nur über RE15_FIRE_AOT erreichbar, nicht zu Fuß" | **widerlegt.** Echtlauf über den regulären Türweg: Leon lockt den Stoß aus 3200 Einheiten an und läuft unten um Birkin herum. Mit hp 100 erreicht er die Tür. msg 2 geht auf, dazu **1 × ZU_P** (tuerse.log `F1090 raum=5080 nachricht=2 art=S weg=AOT satz=3(ZU_P) nr=1`). Danach schließt der Text, und Leon läuft wieder. |

Es gibt keine belegte Ursache für einen Hänger in sub02, deshalb ist nichts gebaut.

Zwei echte Nebenbefunde gehen als nächster Weg an die Birkin-Kampagne (siehe unten):
1. die ungeportete **Opfer-FSM des Spielers** (Leon von Birkin getroffen);
2. eine falsch zitierte Adresse im Port-Kommentar.

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
- Schienen: RE15_STATE_LOG (je Bild pad / Spieler / pauseflags / jeder Aktor), RE15_DISCARD_LOG, debug.log.
- Eingaben: `skript_bau.py`.
- Logs unter `build/r30n5080_*` (nicht committet).

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

`RE15_DEBUG_JUMP=6010@120`, dann `RE15_FIRE_AOT=2@60#6010`: der Tür-Platz 2 von ROOM6010, derselbe Pfad wie das Hineinlaufen (aot_fire_door).

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
- Körperzylinder r 1000 + Spieler 450 = 1450, FUN_8002aec4/FUN_8002b544. Selbst nachgelesen: Hurt-Box `{0,-1440,0,1000,1440,1000}` in STAGE3 @0x8011ee64 und STAGE5 @0x8011fe28 (Bytesuche), Spieler `{0,-1530,0,450,1530,450}` in PSX.EXE Datei 0x64694.
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

- **Für den gemeldeten Hänger in sub02: keine.** Die Schleife wartet auf die Ankunft einer reinen Drehung (@0x800313d0/@0x800313f8). Der Port erreicht sie in 21 Bildern.
- **Für „Leon kommt nicht zur Tür":** Das ist der Birkin-Kampf, den sub03 byte-getreu freischaltet (Member_set @Datei 0x0083E).
  - Der Port verstärkt ihn durch den Nebenbefund 2 (Pin statt Opfer-FSM).
  - Die Tür ist trotzdem zu Fuß erreichbar (4.5).

## 6 Änderung

Keine an der Engine. Committet sind nur die Werkzeuge unter `nachschliff-room5080_tools/` und dieses Dossier.

## 7 Nebenbefunde — nächster Weg (Birkin-Kampagne)

### N1 (erheblich, Port-Lücke): Leon-seitige Opfer-FSM für Typ 0x30 fehlt

**Original:** Der Treffer im Stoß schreibt
- 801177d0 `sw v0,0x800aca58` mit v0 = (facing<<8)|5 → Spieler-Kommando 5;
- dazu 801177a8 die Blickrichtung, 801177b0 den Angreifer 0x800acbfc und 801177c0/d8 die Opfer-Bank 0x800acbcc/d0 (STAGE3; STAGE5 +0x814).

**Kommando 5** 0x80036834 und **6** 0x800368c0 verzweigen nach dem Angreifer-Typ. Sie lesen `lw v0,-770(a0+type*4)` bzw. `-514` und springen in die Tabellen @0x800ac758/@0x800ac858 [0x30]:
- STAGE3: 0x800ac818 = **0x8011a7c4** (Installer STAGE3_overlay.c:12748)
- STAGE5: 0x800ac818 = 0x800ac918 = **0x8011afd8** (STAGE5_overlay.c:13155/13168)

Das ist eine Spieler-FSM:
- vier Varianten über +5, Tabelle @0x8011f4a4 (STAGE3);
- je 14 Phasen über +6, Tabelle @0x8010048c.

Variante 0 (STAGE3):
- **[0] @0x8011a840:**
  - Leons eigener Clip 0xd (8011a860), Bild 6, frac 3;
  - Rückstoß-Tempo +0x8c = 0x4b0 = 1200 (8011a884);
  - +0x93 |= 1;
  - SE 0x04010001.
- **[1] @0x8011a8c0:** Tempo −150 je Bild, min 400 (8011a8f0/904). Landung (+0x0 & 0x10) → [5].
- **[2]/[3] @0x8011a940/968:** Clip 0xe, Tempo −50.
- **[5] @0x8011aa00:** Clip 0xf, SE 0x04070001 + 0x04020001.
- **[7] @0x8011aa60:** Opfer-Clip 0 aus der Birkin-Bank. Zähler 0x800acaf0 = 75 (0x113 = 275 bei hp < 30, +380 bei hp < 10).
- **[8] @0x8011aac0:** Zähler −= 4 + 9 × FUN_80037024 (Tasten-Flanken = Befreien durch Hämmern).
- **[9]–[12]:** Aufstehen, Clips 0x10 / 0xb.
- **[13] @0x8011ab9c = Freigabe:**
  - 8011abac `sw 1,0x800aca58` (Kommando 1);
  - 8011abbc +0x1b8 := 0;
  - 8011abc8 +0x93 &= 0xfe;
  - 8011abd8 aca3c &= ~0x40.
- **Schwanz 8011abdc:** `jal 0x800245d8` (Rückstoß-Schritt).

**Modulgleichheit** (`birkin_modul_vergleich.py`):
- 0x80116230..0x8011b4d0 gegen STAGE5 +0x814: 5288 Worte, 5176 gleich, 112 reine Verschiebungen, **0 echte Abweichungen**.
- Die Opfer-FSM allein (835 Worte): ebenfalls 0 echte Abweichungen.

**Port:**
- Keine Stelle kennt 0x8011a7c4 oder 0x8011afd8.
- Stattdessen `s_player_grabbed = 1` bei jedem Treffer, dazu `birkin_grab`. Das wird **je Bild neu gesetzt** (enemy_ai_common.c `if (e->birkin_grab) s_player_grabbed = 1;` im Wurzel-Vorlauf).
- Gelöst wird es erst am Ende von Birkins GRAB-THROW (Sub 5 Phase 2, `e->birkin_grab = 0`).
- Leon steht dabei im Greif-Zweig (game_step_common.c `re15_player_is_grabbed()`): ohne Pad, ohne Rückstoß, ohne Clip, ohne Hämmern.

**Gemessen** (`ausweich1b`, Lücken im Normalzweig-Protokoll `[push]`):
- Stoß-Treffer F1059: gepinnt **F1060–F1367 (308 Bilder)**.
- Danach Zyklen von **263 Bildern gepinnt / 14 Bildern frei**, je −10 hp.
- Ohne Eingabe stirbt Leon so nach ~10 Zyklen, ohne jede Reaktions-Animation.

**Nächster Weg:**
- 0x8011afd8 (alle vier Varianten) portieren, wie `re15_player_victim_*` bei den Zombies.
- Die Freigabe an [13] hängen statt an Birkins Sub 5.
- Die Wurfkommando-6-Seite (Birkin Sub 5 Bild 0x2c) läuft über **dieselbe** Adresse.

### N2 (gering, Zitat): falsche STAGE5-Adresse im Port-Kommentar

`enemy_ai_common.c:11876` zitiert für `re15_game_flag_set(5, 0x1c, 1)` „@0x8011a6b8 / @0x8011ae10". Die STAGE3-Adresse stimmt:
- 8011a690 `addiu a0,…0x800b1028`, 8011a698 `ori a1,zero,0x1c`, 8011a6b8 `jal 0x8004ef90`.

In **STAGE5** steht an 0x8011ae10 ein Sprungtabellen-Zugriff (`lui at,0x8010` / `addiu at,at,1204`). Der Setzer liegt dort bei **0x8011aecc** (= 0x8011a6b8 + 0x814, `jal 0x8004ef90`, gleiche Argumente). Die Memory-Notiz `reai-v2-endkampf-birkin` trägt dieselbe falsche Zahl. Das ist nur ein Kommentar, das Verhalten ist nicht betroffen. Nicht geändert, weil die Spur ohne belegte Ursache nicht baut.

## 8 Nicht gemessen / offen

- **Birkin vor dem Absprung.** Grid 0x33 → INIT → Sub 9 EMERGENCE schon bei Raumladung:
  - Sub 9 läuft ab Bild 2 nach der Raumladung auf (−18100,200): Phase 0 = Se(3), Phase 2 um F65 = Se(10) (Lauf `tuerweg`);
  - danach Sub 1 außerhalb der Raumwände bei (−19107,−3529).
  - Ob das Original ihn dort ebenso laufen lässt, ist nicht gemessen. Der Code ist derselbe (Modulgleichheit oben), ein Savestate fehlt.
- **Grid 0x13 trägt Bit 0x10.** Der Port liest das als Form-2 (Stoß/Biss starten bei Bild 0x14, @0x80117600/@0x80117ae8). Damit kommt das Trefferfenster 16 Bilder nach Stoßbeginn. Das folgt den Skript-Bytes. Einen Laufzeitvergleich mit dem Original gibt es nicht.
- **Bildgenaue Parität der Drehung** (Ankunft F856 oder F857) gegen das Original: nicht gemessen, für die Frage ohne Belang.
- **Nur Zustandsprotokolle.** Kein Bildbeweis (Framedump/gdigrab) — die Befunde sind Zustands-, keine Bildbefunde.
