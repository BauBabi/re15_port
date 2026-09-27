# Runde 32 / Welle 2: DIE SENKRECHTE ZIELPOSE — GEBAUT

Nutzer-Auftrag: *„Zielpose bauen, damit wir diesen Aspekt sauber abgeschlossen haben."*
Projektziel: *„Das soll alles sauber Resident Evil 2 entsprechen."*

`bash re15_port/tools/local_build.sh all` → **`=== LOCAL-BUILD-OK (all) — Tests 356/356`**
Messprotokoll: `analysis/befunde_2026-09-27/zielpose-bau.log` (`probe_r30b_muendung riegel`,
Abschluss `PROBE-OK`).

---

## 0. Kurzurteil

| | |
|---|---|
| **Gebaut** | Die Zielpose. Die Mündung kommt jetzt aus der **Pose** der aktiven Waffen-Bank, nicht aus einer Konstante: `re15_player_aim_muzzle_world()`, engine-seitig, headless **und** PSX. Gemessen **HOCH 2751 · EBEN 2500 · TIEF 1988** über den Füßen, als Riegel festgenagelt. |
| **Widerlegt** | Welle 1s Satz „RE2s Aim-Keyframes liegen nicht in den Bänken, die wir haben". Sie liegen dort. RE2s eigene Bank liefert **2805 / 2504 / 1921** — dieselbe Pose wie RE1.5. |
| **Nicht gebaut** | Das fünfte Tor füttert die Zielpose **nicht**. Mit ihr wird der **stehende Hund in 200 Bildern Dauerbeschuss 0 mal getroffen** (gemessen). Das ist die Runde-13/14-Falle. |
| **Nicht gebaut** | Krähe 0x21 und Baby 0x26 bleiben **ungegatet**. Die Zielpose öffnet sie **nicht** — sie macht es schlechter (Zahlen in §4). |
| **Nicht gebaut** | Die Waffen-Absenkung um 200 (@0x80042F60-6C). Begründung mit Adresse in §5. |

---

## 1. Was das Original tut — selbst gelesen

### 1.1 Die Kette, und dass sie die LOKALEN Teilmatrizen liest

`info/re2leon/PSX.EXE`, Funktion @0x80042C64 (Rahmen −104):

```
80042e18: lw    s2,408(s1)      ; s2 = Teile-Pool (+0x198)
80042e1c: jal 0x8008e1f4        ; RotMatrix(SVECTOR *r = Spieler+0x74, MATRIX *m = Spieler+0x24)
80042e50: jal 0x8008dba4        ; ApplyMatrixLV(m, v, out) — v = (0,0,300-15*((+0x14D)-5)) -> sp+64
80042e60: addiu s0,sp,32        ; ZIEL-MATRIX auf dem Stack
80042e64: jal 0x8002ce94        ; a0 = Spieler-MATRIX, a1 = s2+24,   a2 = sp+32
80042e74: jal 0x8002ce94        ; a0 = sp+32,          a1 = s2+1572, a2 = sp+32
80042e84: jal 0x8002ce94        ; a0 = sp+32,          a1 = s2+1744, a2 = sp+32
80042e94: jal 0x8002ce94        ; a0 = sp+32,          a1 = s2+1916, a2 = sp+32
80042f8c: addiu a0,sp,52        ; &MATRIX.t[0]
80042f94: jal   0x800470c0      ; der Kandidatenfilter
```

Neu in dieser Welle, weil es die Kette *erklärt*: der **allgemeine** Skelett-Komponierer
@0x80019100-64 verkettet dieselben Teile (24, 196, 368, 884, 1572, 2088) und legt die
**Welt**-Matrix je Teil nach `+72 + 172·k` ab. Der Schuss-Pfad benutzt die **nicht** — er liest
die **lokalen** Matrizen `+24 + 172·k` und verkettet selbst. Deshalb *ist* es eine Kette
0 → 9 → 10 → 11 und kein fertiger Wert.

Und: `s4 = a0` ist bewiesen, nicht angenommen — @0x800470D0 `addu s4,a0,zero`. `lw a0,4(s4)`
@0x8004718C liest damit `sp+56` = `MATRIX.t[1]`.

### 1.2 Welcher Clip die Pose füllt

```
80042cf8: lhu v0,340(s1)     ; +0x154 (das Band, Bit 15/14/13)
80042d08: srl v0,v0,13
80042d0c: sll v0,v0,1
80042d10: addu v0,v0,a0      ; a0 = sp+16 = Kopie der Tabelle @0x80011010
80042d1c: lh  v0,0(v0)
80042d28: sw  v0,332(s1)     ; +0x14C = 0x00070000 | Clip
```

Tabelle @0x80011010, selbst gelesen (`re2_disasm.py read … --w 2 --signed`):
`[0, 14, 10, 0, 12, …]` ⇒ **TIEF = Clip 14 · EBEN = Clip 10 · HOCH = Clip 12** der jeweiligen
Bank. (Die 10 Bytes werden @0x80042C98-CBC per `lwl/lwr`+`lh` auf `sp+16` kopiert; die zweite
Kopie @0x80042CC8-E4 nach `sp+72` ist die **horizontale** Keil-Geometrie `{−200, 0, 250, 125}`,
die FUN_800470C0 @0x800471BC-E8 um `(+0x1EE >> 2)` aufweitet — **keine** Höhe.)

---

## 2. ⛔ Welle 1s Kernaussage ist widerlegt: RE2s Bänke sind da

Welle 1 (`zielpose-ermittlung.md` §6.1) schrieb, RE2s Aim-Keyframes fehlten, und stützte das auf
`info/re2leon/PL0/PLD/PL00W03.PLW` = **3436 B**. Das ist in RE2 ein **Stummel** — genauso wie
W09/W0A/W0B/W0C/W0E, alle exakt 3436 B. Die **vollen** Bänke liegen daneben:

| Bank | PLW | EMR | Clips |
|---|---|---|---|
| PL00W00 | 32928 | 27848 | 17 |
| PL00W01 | 34524 | 29288 | 17 |
| **PL00W02** | 33928 | 27848 | **17** |
| PL00W04…W08, W0D, W0F…W12 | 34–47 kB | 27–40 kB | 17 |

`zielpose_fk.py` (unverändert) + das neue `zielpose_frames.py` (gleiche FK, Bild-für-Bild-Ausgabe)
auf RE2s eigenem `PL00.emr` + `PL00W02`:

```
Clip 10  n=23  Ruecklauf EBEN  2431..2565      Clip 11  n=1  HALTEN EBEN 2504
Clip 12  n=24  Ruecklauf HOCH  2740..2805      Clip 13  n=1  HALTEN HOCH 2805
Clip 14  n=24  Ruecklauf TIEF  1893..2069      Clip 15  n=1  HALTEN TIEF 1921
```

Gegenüber RE1.5 `PL00W03` (Clips 7/8 · 9/10 · 11/12): **2500 · 2751 · 1988**.

> **Abstand EBEN 4 · HOCH 54 · TIEF 67 Einheiten** (0,2 % / 1,9 % / 3,4 %).
> Welle 1s Sorge „RE2s Fenster gegen RE1.5s Posenhöhen — zwei Engines" ist damit
> **gegenstandslos**. Es ist dieselbe Pose.

Die Clip-Rollen decken sich strukturell: in beiden Bänken ist der HALTE-Clip ein
**Ein-Bild-Clip**, davor steht der Rückstoß-Clip mit 22–25 Bildern, der auf **demselben Wert
beginnt und endet**. Genau die Rückstoß-Clips (RE2: 14/10/12) stehen in der Tabelle @0x80011010.

---

## 3. Was im Port jetzt steht

| Baustein | Ort | Zustand |
|---|---|---|
| W-Bank im Engine-Teil | `re15_game_ctx_t.w_skel/.w_anim` → `re15_player_set_w_banks()` (`player_common.c`) → `re15_player_w_skel()/w_anim()` | **neu** |
| PC füllt sie | `platform/pc/main.c` (gctx-Block), aus derselben Quelle wie der Aim-Render-Override L7423-7431 | **neu** |
| Die Kette | `muzzle_bone_world(aim, out, rot)` in `re15_damage.c` — @0x80042E60-94, Bone 11, t[1] | **neu (gemeinsamer Kern)** |
| **Die Zielpose** | `re15_player_aim_muzzle_world()` | **neu** |
| Basis-Pose (Tor-Eingang) | `re15_player_muzzle_world()` | unverändert im Wert, Kommentar berichtigt |
| Waffen-Bone headless/PSX | `re15_player_gunbone_world()` fällt auf die Zielpose zurück, wenn der PC-Renderer nichts geliefert hat | **neu** |

Der letzte Punkt ist der eigentliche Verbraucher: `re15_player_gunbone_world` war **gemessen
0 von 1200 Bildern gültig**, sobald kein PC-Renderer lief, und der PSX-Zweig füttert ihn nie.
Daran hängen die Mündungsfeuer-, Rauch- und Hülsen-Anker
(`game_step_common.c:1771 / 1851-1861 / 1895`). Die Rotation dazu ist `Ry(rot_y) · R_bone` —
dieselbe Größe, die der Renderer als `yawed_rot` spiegelt.

**Gemessen im Port** (`probe_r30b_muendung` Teil 0, echter Weg, `re15_game_step` + Pad,
Waffe 3 = `PL00W03`, RE2-Bank `shared_assets/RE2/CDEMD0.EMS` geladen):

```
[HOCH] Clip 10 | Muendung ueber den Fuessen 2751..2751 (haltend 2751) | 50 Bilder
[EBEN] Clip  8 | Muendung ueber den Fuessen 2500..2500 (haltend 2500) | 50 Bilder
[TIEF] Clip 12 | Muendung ueber den Fuessen 1988..1988 (haltend 1988) | 50 Bilder
```

Riegel: die drei Sollwerte **und** die Monotonie HOCH > EBEN > TIEF. Fällt einer, ist die
Bank- oder die Clip-Wahl kaputt. (Die Zahlen kommen unabhängig auch aus `zielpose_fk.py`,
also von außerhalb des Ports.)

---

## 4. ⛔ Warum das fünfte Tor die Zielpose NICHT bekommt — mit Zahl

Das Tor @0x8004717C-A4 verlangt, dass die Mündung senkrecht **in** der Trefferzone liegt:
`DURCH ⟺ −(b+h+100) ≤ Hgun < (h−b+100)`. Neue Sonden-Ausgabe (Teil 0b), reine Arithmetik
gegen die drei gemessenen Halte-Höhen:

```
Box                      Fenster Hgun         DURCH bei HOCH/EBEN/TIEF
ZOMBIE 0x10 stehend      [ -100, 3100)        HOCH=JA   EBEN=JA   TIEF=JA
ZOMBIE 0x10 Kriecher     [ -100,  800)        HOCH=nein EBEN=nein TIEF=nein
HUND   0x20 stehend      [ -100, 2100)        HOCH=nein EBEN=nein TIEF=JA
HUND   0x20 liegend      [ -100, 1100)        HOCH=nein EBEN=nein TIEF=nein
KRAEHE 0x21              [ -280,  980)        HOCH=nein EBEN=nein TIEF=nein
SPINNE 0x25              [-1500, 1500)        HOCH=nein EBEN=nein TIEF=nein
BABY   0x26              [ -100,  120)        HOCH=nein EBEN=nein TIEF=nein
```

Und der Lauf mit angeschlossener Zielpose (verworfener Zwischenstand, gemessen, Protokoll in
diesem Verzeichnis nicht abgelegt, weil der Stand nicht committet ist):

```
[HUND 0x20   ] Treffer   0 in 200 Bildern | re2_hit_box_set=1 -> ⛔ NIE GETROFFEN
  HUND 0x20    Tor DURCH   0/240
  Hgun beim Hund: 2150..2950  (Fenster stehend [-100,2100))
```

> Der **stehende Hund** fällt aus seinem eigenen Fenster: EBEN 2500 liegt **400 Einheiten**
> über der Oberkante 2100. Damit wäre er **dauerhaft untreffbar**, solange der Spieler nicht
> RUNTER drückt — genau das ⛔, das diese Runde verbietet, und ein direkter Widerspruch zum
> Nutzer-Befund aus Runde 26 („im Original sind die Hunde wieder verwundbar, sobald sie
> stehen"), gegen den Runde 30/31 gemessen wurden.

**Zwei byte-true Bausteine ergeben zusammen ein falsches Ergebnis ⇒ ein dritter fehlt.**
Was ausgeschlossen ist (jeweils selbst gelesen, nicht vermutet):

* **Kein Auto-Band für Leon.** Das höhengetriebene Band @0x80060F4C-FD0 hängt an der
  Handler-Tabelle @0x800A8930 eines NPC-Schützen; Leons Aim-FSM (0x8004326C) steht sechsmal in
  einer anderen Tabelle (@0x800A702C/7048/7068/7080/7098/70B0). Sein Band kommt aus den
  Pad-Bits 0x10/0x20 (@0x8004371C-B4, @0x80047AB0-B4C).
* **Kein zusätzlicher Höhen-Schlupf im Filter.** `sp+72` = `{−200, 0, 250, 125}` wird
  @0x800471BC-E8 nur in den **lateralen** Komponenten (+4/+6) um `(+0x1EE >> 2)` aufgeweitet und
  geht als `a3` in den **horizontalen** Keiltest `jal 0x80041ef8` @0x800471E8.
* **`eY` ist die Fußhöhe, nicht die Körpermitte.** Beleg aus RE2s eigener Spieler-Box
  −1530/1530 (@0x8005742C/34/38/3C): Mitte 1530 über `eY`, Halbhöhe 1530 ⇒ die Box reicht von
  `eY` genau 3060 nach oben. `eY` = Boden.
* **Der weggelassene −200** (§5) dreht keinen einzigen Fall um: 2500 − 200 = 2300 liegt immer
  noch 200 über der Hundeoberkante 2100.

**Der nächste Weg** (nicht mehr in dieser Welle): das Original hat für den Spieler **zwei**
Aufrufstellen von FUN_800470C0 — @0x80042F94 (nur für `(+0x14D)−7 ∈ [0,4]`) und **@0x800467C0**,
deren erstes Argument `s5` noch nicht aufgelöst ist. Ist `s5` dort **nicht** die Mündung, dann
hat die Waffenklasse, die Hunde und Kriecher trifft, gar kein senkrechtes Mündungsfenster — und
der Widerspruch löst sich ohne jede geratene Zahl auf. Das ist die erste Frage der nächsten
Welle.

### 4.1 Krähe 0x21 und Baby 0x26 — mit Zahl nicht gebaut

Der Auftrag sagt: nachziehen **nur**, wenn die Messung zeigt, dass sie wieder Kandidaten werden.
Sie werden es nicht — die Zielpose macht es **schlechter**:

| Typ | Fenster | Basis-Pose (heute) | HOCH 2751 | EBEN 2500 | TIEF 1988 | Tor DURCH |
|---|---|---|---|---|---|---|
| Krähe 0x21 | [−280, 980) | 1420…1671 | nein | nein | nein | **0 / 240** |
| Baby 0x26 | [−100, 120) | 1665…1671 | nein | nein | nein | **0 / 240** |

Beide bleiben ungegatet und damit treffbar (Teil 4: Krähe 10, Baby 10 Treffer in 200 Bildern).
Kein Env-Schalter, keine Schwelle angefasst.

---

## 5. ⛔ Nicht portiert: die Waffen-Absenkung um 200

```
80042f48: lbu   v0,333(s1)      ; +0x14D
80042f50: addiu v0,v0,-7
80042f54: sltiu v0,v0,0x5       ; nur (+0x14D)-7 in [0,4] — sonst wird der GANZE Block
80042f58: beq   v0,zero,0x80042fbc ;   inklusive `jal 0x800470c0` uebersprungen
80042f60: lw    v0,56(sp)
80042f68: addiu v0,v0,200       ; PSX-Y zeigt nach unten -> 200 Einheiten TIEFER
80042f6c: sw    v0,56(sp)
80042fac-b8:  dieselbe Stelle wieder -200
```

Die Absenkung selbst ist eindeutig. **Was fehlt, ist die Zuordnung von `+0x14D`.** Vollscan
(`scan_off.py info/re2leon/PSX.EXE 0x80010000 0x800 333`): **20 Schreiber, 40+ Leser**, und sie
sind **nicht** deckungsgleich mit der Waffen-Id des Ports — ein Schreiber legt dort `7·x` ab
(@0x8003D790-B4 `sll v0,v1,3` / `subu s1,v0,v1` / `sb s1,333(s0)`), ein anderer die Konstante 4
(@0x8003EFC8). Eine geratene [7,11]-Klammer wäre ein Rate-Defekt. **Wirkung auf die heutigen
Urteile: keine** — 200 Einheiten drehen keinen der sieben Fälle um (kleinster Abstand zur
Fenstergrenze: 400 beim stehenden Hund).

---

## 6. Sichtprüfung — ehrlich

`RE15_NOAUDIO=1 RE15_TITLE_SHOT=1 RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP="1140@320"
RE15_INPUT_SCRIPT="W1,M3,MU3,M1,MD3,M2" RE15_INPUT_SCRIPT_START=320
RE15_FRAMEDUMP="380-620/10:…"` → 114 Bilder, kein `AUTOSHOT`, kein Softwarerenderer.

`analysis/befunde_2026-09-27/zielpose_bilder/`:
* `raum1140_f420.png` — der volle Frame (Briefing-Raum, Zombie links unten am Boden).
* `c_eben.png` / `c_hoch.png` / `c_tief.png` — derselbe Ausschnitt um Leon, aus den Bildern
  f420 (kein Band), f470 (UP gehalten), f500 (DOWN gehalten).

**Was zu sehen ist:** Leons Oberkörper- und Armhaltung **ändert sich mit dem Band** — bei UP
liegt der rechte Arm höher quer vor der Brust, bei DOWN sitzt er tiefer und die Schulter kippt
nach vorn. Das ist die Zielpose bei der Arbeit.

**Was NICHT zu sehen ist, und das sage ich klar:** Leon trägt in diesen Bildern das **Messer**,
weil ein per `RE15_DEBUG_JUMP` betretener Raum mit dem Start-Inventar beginnt. Die
Handfeuerwaffen-Pose (die 2751/2500/1988 misst) ist deshalb **nicht** im Bild belegt, sondern
nur als Zahl — dreifach: durch den Port (Teil 0), durch die FK außerhalb des Ports
(`zielpose_fk.py`) und durch RE2s eigene Bank. **Es wird kein Bild als Beleg für die
Gun-Höhen behauptet.**

Ein Bild „Leon zielt auf einen niedrigen Gegner und trifft ihn" gibt es in dieser Welle
ebenfalls nicht — weil Krähe und Baby aus §4.1 bewusst **nicht** gegatet wurden und sich für sie
folglich nichts geändert hat.

---

## 7. Riegel-Stand

* `local_build.sh all` → **356/356**, Abschlusszeile
  `=== LOCAL-BUILD-OK (all) — Tests 356/356`.
* `probe_r30b_muendung riegel` → **PROBE-OK** (Protokoll `zielpose-bau.log`), jetzt zusätzlich
  mit den drei festgenagelten Posen-Höhen und der Monotonie.
* Der Hunde-Riegel aus Runde 30 (liegend untreffbar, stehend treffbar, nach dem Aufstehen wieder
  treffbar) und der Boxen-Riegel `probe_r31_boxen` bleiben grün.
* Kein Typ dauerhaft untreffbar: Zombie 19, Hund 3, Krähe 10, Spinne 19, Baby 10 Treffer in je
  200 Bildern.

## 8. Offen

1. **`s5` an der zweiten Spieler-Aufrufstelle @0x800467C0** — die entscheidende Frage aus §4.
2. **`+0x14D`** ist nicht aufgelöst (§5); solange bleibt die −200 draußen.
3. **`re15_player_aim_clip()` kann 15 liefern** (`player_common.c:933/1038`, Auto-Maschine 9+6),
   `PL00W03` hat aber nur 14 Clips (0…13). Der neue Pfad fängt das ab (`wc < clip_count`), aber
   die Ursache steht weiter offen — Welle 1 hat sie schon notiert.
4. **Kein Bildbeleg für die Gun-Pose** (§6).
