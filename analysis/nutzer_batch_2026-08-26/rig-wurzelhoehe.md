# FINDING 1 (RE2-KI): „der Zombie in 10D0 schwebt nach dem Aufstehen leicht in der Luft"

**Dossier 2026-08-26. ⛔ KEINE Code-Aenderung an der Engine in dieser Phase.** Neu im Baum ist
ausschliesslich die Mess-Sonde `re15_port/tests/unit/probe_rig_sohle.c` (+ CMake-Eintrag, **kein
`add_test`**). `bash re15_port/tools/local_build.sh` meldet weiterhin **230/230**.

---

## 0. URTEIL VORWEG

1. **Der Fehler ist reproduziert und global** — nicht 10D0-spezifisch, nicht Sitz-Import-spezifisch.
   In **ROOM1030** (sechs stehende Zombies 0x16, die den Sitz-Import nie beruehren) misst die Sohle
   im RE1.5-Flavor **+6 … +127** (auf dem Boden) und im RE2-Flavor **−133 … −219** (schwebt).
2. **Das Original macht das nicht — und es MUSS es auch nicht.** Es gibt in RE1.5 **wie** in RE2
   **keinerlei Boden-Verankerung fuer Y**: die Wurzel-Translation wird 1:1 aus dem Keyframe
   uebernommen (Zuweisung, keine Addition, `FUN_8001f3bc` @0x8001f434-0x8001f440), und die einzige
   Wurzel-Nachfuehr-Maschinerie des Spiels (`FUN_8001ac38` / `FUN_8001ad68`) arbeitet
   **ausschliesslich in X und Z**. Das Original braucht keine Verankerung, weil Wurzel-Translation
   und Bind-Kette dort **immer aus derselben EMR-Sektion** stammen. Der Beleg fuer die *Abwesenheit*
   steht in §3 — das ist ein vollwertiges Ergebnis, kein Fehlschlag.
3. **Der Hybrid-Rig allein ist NICHT das Richtige. Die Wurzel muss mit-konvertiert werden** —
   aber **nicht** mit einer Konstante. Der Vorschlag „+168" aus dem Vor-Dossier
   (`analysis/nutzer_batch_2026-08-25/schweben-10d0.md` §6 A) ist **durch Messung widerlegt**:
   der Fehler haengt an der Pose und laeuft ueber alle Clips von **−4 bis −165** (Bone) bzw.
   **−7 bis −219** (Sohle). Eine additive Konstante wuerde jeden liegenden, kriechenden, fressenden
   und zuschlagenden Zombie um bis zu **160 Einheiten IN den Boden** druecken. Gemessen in §2.4.
4. **Der richtige Wert ist PRO KEYFRAME berechenbar** — vollstaendig aus zwei gelesenen
   Byte-Tabellen, ohne eine einzige geratene Zahl: `K(kf) = tiefster Punkt mit RE2-Bindlaengen −
   tiefster Punkt mit RE1.5-Bindlaengen`, **bei identischen Keyframe-Rotationen**. Gegengemessen
   (§5.3): damit landet die Sohle exakt im Band, das das jeweilige Original haelt.
5. **Betroffen ist ausschliesslich die Zombie-Familie** (0x10/0x11/0x16, per Tabelle auch
   0x12/0x18/0x13). **Hund 0x20 und Kraehe 0x21 sind messbar NICHT betroffen** (Korrektur ±2 bzw.
   0) — beides selbst gemessen, nicht uebernommen.

---

## 1. METHODE — was gemessen wurde und warum so

### 1.1 Die Messgroesse: die ECHTE SOHLE, nicht ein Bone

Das Vor-Dossier misst den tiefsten **Bone**. Was der Nutzer sieht, ist der tiefste **Vertex**.
Die neue Sonde rechnet deshalb die vollstaendige Renderer-Kette aus `platform/pc/main.c:6957-7050`
nach:

```
v_world = (nyaw * pose[b].rot) * v_local + (nyaw * pose[b].trans) + (e->x, e->y, e->z)
Mesh-Index = mesh_remap[b]   (main.c:7043-7049;  −1 = Slot wird NICHT gezeichnet)
Bone-Klammer nb = bone_count, ohne Remap auf mesh_count geklemmt (main.c:6916)
```

mit `yaw = 0`, Aktor im Ursprung. PSX-Y zeigt nach UNTEN:

| Sohle | Bedeutung |
|---|---|
| `= 0` | steht exakt auf der Bodenebene |
| `< 0` | **schwebt** um \|Sohle\| |
| `> 0` | steckt um Sohle im Boden |

Drei Konfigurationen, jeweils komplett:

| Kuerzel | Rig (Bind) | Keyframes | Mesh | im Spiel? |
|---|---|---|---|---|
| **RE15** | RE1.5-EMR | RE1.5 | RE1.5 | ja — RE1.5-Flavor |
| **RE2** | RE2-EMR | RE2 | RE2 | **nein** — analytische Referenz |
| **HYB** | RE2-Hierarchie + **RE1.5-Bindlaengen** | RE2 | RE1.5 | **ja** — RE2-Flavor, bedingungslos |

Der Hybrid laeuft im RE2-Flavor **ohne Gate** (`main.c:668-676`, Kommentar dort: „die Stufe ist auf
Nutzer-Entscheidung entfernt, also laeuft der Hybrid BEDINGUNGSLOS"). Wer ihn beim Messen weglaesst,
misst ein Skelett, das es im Spiel nicht gibt.

### 1.2 Ein Mess-Artefakt, das im ersten Anlauf falsche Zahlen erzeugt hat (offengelegt)

Die erste Fassung der Sonde lud alle RE1.5-Modelle eines Raums in **einen** Puffer. `re15_md1_t`
haelt **Zeiger** in diesen Puffer; das Laden des zweiten Typs bog die Zeiger des ersten auf fremde
Bytes um und erzeugte Sohlen von 3 000 … 45 000. Behoben durch einen eigenen Puffer je Typ
(`probe_rig_sohle.c`, `m15_t s_m15[6]`). **Alle Zahlen unten stammen aus der korrigierten Fassung.**
Einziger unauffaelliger Einzel-Typ-Raum und damit die sauberste Messung: **ROOM1030**.

### 1.3 Was NICHT gemessen ist

Kein gdigrab-Lauf am echten Fenster (Skill `re15-port-visual-verify`). Die Kette ist rechnerisch
identisch mit dem Renderer, aber die **Abnahme eines Fixes** gehoert gefilmt. Siehe §7.

---

## 2. DIE MESSREIHE

### 2.1 ROOM10D0 — derselbe Zombie, beide Flavors, ganze Sequenz (`e->y = 0`, `floor = 0` durchgehend)

| Tick | Phase | RE1.5-Flavor: Clip / **SOHLE** | RE2-Flavor: Clip / **SOHLE** |
|---|---|---|---|
| t0 | Sitzen | 42 / **+179** | 42 / **+179** |
| t40 | Wecken | 42 / **+255** | 42 / **+255** |
| t60 | Aufstehen | 41 / **+188** | 41 / **+188** |
| t100 | Aufstehen Ende | 41 / **+64** | 41 / **+64** |
| **t120** | **erster Geh-Schritt** | 4 / **+42** | **0 / −202** |
| t140 | Gehen | 4 / +101 | 0 / **−151** |
| t160 | Gehen | 4 / +99 | 0 / **−180** |
| t180 | Gehen | 4 / +99 | 0 / **−181** |
| t200 | Nachfassen | 4 / +100 | 12 / **−163** |
| t220 | Nachfassen | 4 / +98 | 12 / **−207** |
| t260 | Angriff | 10 / +186 | 12 / **−229** |
| t300 | Angriff/Griff | 10 / +189 | 2 / **−2** |
| t340 | Griff | 10 / +188 | 22 / **+106** |
| t400 | Griff | 10 / +188 | 9 / **+55** |

**Bis t100 sind beide Flavors bitgleich** — Sitzen/Wecken/Aufstehen laufen in BEIDEN aus der
RE1.5-Sitz-Pose-Bank (`re2z_re15_pose`, `main.c:6817-6830`). Der Sprung kommt **exakt mit dem
ersten RE2-Loco-Clip**. Damit ist der Nutzer-Satz „**nach dem Aufstehen**" praezise erklaert: nicht
das Aufstehen ist kaputt, sondern der Clip, der danach uebernimmt.

**Und ebenso wichtig:** ab t300 (Angriff/Griff, Clips 2/22/9) steht derselbe Zombie im RE2-Flavor
wieder auf dem Boden (−2 … +106). Eine additive Konstante von +168 wuerde ihn dort 170–270
Einheiten versenken.

### 2.2 ROOM1030 — die Gegenprobe ohne Sitz-Import (6 × Typ 0x16, reines Stehen/Gehen)

| Tick | RE1.5-Flavor Clip / **SOHLE** | RE2-Flavor Clip / **SOHLE** |
|---|---|---|
| t0 | 39 / +127 | 39 / +119 |
| t20 | 4 / **+25** | 0 / **−196** |
| t60 | 1 / **+35** | 0 / **−152** |
| t100 | 1 / **+34** | 0 / **−165** |
| t140 | 1 / **+43** | 0 / **−200** |
| t200 | 1 / **+34** | 0 / **−170** |
| t260 | 1 / **+30** | 0 / **−160** |
| t320 | 1 / **+26** | 12 / **−163** |
| t380 | 4 / **+21** | 12 / **−219** |
| t400 | 4 / **+32** | 13 / **−162** |

→ **Der Fehler ist nicht 10D0-spezifisch und nicht Uebergabe-spezifisch.** Gleicher Raum, gleiche
Aktoren, gleiche `e->y = 0`, gleicher Boden — nur der Flavor unterscheidet sich.

### 2.3 ROOM1140 — drei Zombie-Typen, RE2 startet LIEGEND

| Tick | Zustand | RE2-Flavor Clip / **SOHLE** |
|---|---|---|
| t0 | liegt | 18 / **+92** |
| t20 | liegt | 20 / **+28** |
| t40 | richtet sich auf | 21 / **+103** |
| t60 | steht auf | 21 / **−145** |
| t80…t180 | **geht** | 0 / **−141 … −203** |
| t200 | Angriff | 2 / −50 |
| t220 | Angriff | 2 / **+264** |
| t260…t320 | Griff | 22 / +99, 9 / +27…+77 |
| t360…t400 | **geht wieder** | 0 / **−161 … −201** |

Zum Vergleich RE1.5-Flavor im selben Raum: **+4 … +200 durchgehend**, nie negativ.

**Das ist der Kern-Befund:** derselbe Aktor wechselt innerhalb einer Sekunde zwischen
„+264 im Boden" und „−203 in der Luft", je nach Clip. Ein pose-unabhaengiger Versatz kann das
per Konstruktion nicht treffen.

### 2.4 Hund 0x20 (ROOM1230) und Kraehe 0x21 (ROOM10C0) — NICHT betroffen

| Raum/Typ | RE1.5-Flavor SOHLE | RE2-Flavor SOHLE |
|---|---|---|
| ROOM1230 / 0x20 (Hund) | +9 … +110 (Sprung-Frames y=−130/−190 → −111/−292) | +11 … +242 (Sprung y=−1080 → −1176) |
| ROOM10C0 / 0x21 (Kraehe) | folgt `e->y` (Flug), rootY ≈ 0…−48 | folgt `e->y` (Flug), rootY ≈ 0…−274 |

Beim Hund ist die einzige abweichende Bind-Kante Bone 4 (Δ −44, eine **Vorderpfote**), und die
ist im Stand nie der tiefste Punkt; gemessene Pro-Keyframe-Korrektur ueber alle 28 Clips:
**−3 … +5**. Bei der Kraehe sind die Bind-Tabellen beider Spiele **identisch**
(`k_perm_ident`), Korrektur **±2**. Beide Typen brauchen **nichts**.

### 2.5 Wer ist ueberhaupt betroffen? (STAGE1-Zensus, alle Raeume mit statischem Spawn)

`1010, 1020, 1040, 1070, 10B0, 10D0, 10E0, 1030, 1100, 1110, 1140, 1180, 11E0, 11F0, 1200, 1220`
→ Typen **0x10 / 0x11 / 0x16** (alle Zombie-Familie, alle betroffen).
`11D0` → 0x20 (Hund, nicht betroffen). `10C0, 1120` → 0x21 (Kraehe, nicht betroffen).
`11C0` → 0x27 + 0x42, `1090` → 0x26, `1150` → 0x45, `11A0` → 0x40, `1210` → 0x1A
→ diese Typen haben **keine** Perm-Tabelle (`re2_hybrid_perm` liefert −1 → `rc = −2`), der Hybrid
laeuft dort gar nicht. **Zombie Girl 0x13 kommt in STAGE1 nicht vor** (siehe §7.2).

---

## 3. WAS DAS ORIGINAL TUT — und was es nachweislich NICHT tut

### 3.1 Die Wurzel-Translation kommt aus dem Keyframe, per ZUWEISUNG

`FUN_8001f3bc` (RE1.5 PSX.EXE, selbst disassembliert):

```
8001f3f8: lhu v0,2(a0)        ; EMR+2 = Keyframe-Pool-Offset
8001f404: addu a2,a0,v0       ; a2 = Pool-Basis
8001f41c/20: bne a3,zero,… / addu a2,a2,v0   ; a2 = DIESER Keyframe
8001f424: lh  v0,0(a2)        ; Wurzel X
8001f42c: sw  v0,44(s1)       ;   -> Modell +0x2C          (ZUWEISUNG)
8001f430: sh  v0,56(s1)       ;   -> Vor-Pose (Crossfade)
8001f434: lh  v0,2(a2)        ; Wurzel Y        <<<<<<
8001f43c: sw  v0,48(s1)       ;   -> Modell +0x30          (ZUWEISUNG)
8001f440: sh  v0,58(s1)
8001f448: lh  v0,4(a2)        ; Wurzel Z
8001f450: sw  v0,52(s1)       ;   -> Modell +0x34          (ZUWEISUNG)
```

Kein `addu` mit dem Bind-Wert, kein Boden-Term. Der Port bildet das 1:1 ab
(`skeleton_common.c:680-682`, Kommentar „Root translation = keyframe px/py/pz ONLY … does NOT add
the EMR bind").

### 3.2 Die EINZIGE Wurzel-Nachfuehrung des Spiels ist X/Z — Y kommt darin nicht vor

**Der Delta-Extraktor `FUN_8001ae38`** schreibt in seinen Ausgabepuffer `a3` **zwei** Halbworte:

```
8001aeb0: sh v0,0(a3)     ; X
8001af10: sh v0,4(a3)     ; Z
```

Es gibt **keinen** `sh` nach `4(a3)`+2 bzw. `2(a3)`. Die Y-Komponente verlaesst die Funktion nie.

**Der Anker `FUN_8001ac38`** (setzt den Bezugspunkt beim Clip-Wechsel):

```
8001acec: lhu v1,48(sp)    ; rotiertes Wurzel-X
8001acf0: lhu v0,52(a0)    ; entity+0x34 = X
8001acf8: subu v0,v0,v1
8001acfc: sh  v0,160(a0)   ; entity+0xA0
8001ad08: lhu v1,52(sp)    ; rotiertes Wurzel-Z
8001ad0c: lhu v0,60(a0)    ; entity+0x3C = Z
8001ad14: subu v0,v0,v1
8001ad18: sh  v0,162(a0)   ; entity+0xA2
```

**Der Anwender `FUN_8001ad68`** (schiebt die Entity pro Frame):

```
8001adf4: lh   v0,160(s1)  ; +0xA0
8001adf8: lh   v1,48(sp)
8001ae00: addu v0,v0,v1
8001ae04: sw   v0,52(s1)   ; entity+0x34 = X
8001ae08: lh   v0,162(s1)  ; +0xA2
8001ae0c: lh   v1,52(sp)
8001ae14: addu v0,v0,v1
8001ae18: sw   v0,60(s1)   ; entity+0x3C = Z
```

**`entity+0x38` (Y) wird in keiner der drei Funktionen angefasst.**

### 3.3 Der einzige Y-Wurzel-Delta im Spiel ist SPIELER-only

`FUN_800390e0` hat im gesamten Dump **zwei** Aufrufer: `jal` @**0x80038aa8** und @**0x80038d50**.
Beide entscheiden unmittelbar davor anhand von `DAT_800acae9` (Spieler-Zustandsbyte):

```
80038a84: lbu v0,-13591(v0)   ; DAT_800acae9
80038a8c: addiu v0,v0,-12
80038a90: sltiu v0,v0,0xf     ; Zustaende 12..26  (Leiter/Treppe)
80038aa8: jal 0x800390e0
```
```
80038d34: addiu v0,v0,-11
80038d38: sltiu v0,v0,0xf
80038d50: jal 0x800390e0
```

**Null Gegner-Aufrufer.** Es gibt im Original also keine Boden-Verankerung fuer Gegner —
weder statisch noch dynamisch.

### 3.4 Warum das Original ohne Verankerung auskommt: Bind und Keyframe sind DIESELBE Sektion

EMR-Kopf (`emd_common.c re15_emd_parse_skeleton`, RE15_KNOWLEDGE §1.4):
`+0 bones_table_off, +2 keyframe_off, +4 bone_count, +6 keyframe_size`, danach ab **EMR+8** die
Bind-Tabelle (3×s16 je Bone), und bei **EMR+keyframe_off** der Keyframe-Pool.

Selbst gelesene Bytes:

```
RE1.5 EM10  struct-EMR @0x000698 : 64 00 b0 00 0f 00 50 00
   -> bones_tbl=0x64  kf_ofs=0xB0  bones=15  kf_size=80
   Bind @0x0006A0 : (0,-1751,0) (0,228,-160) (0,664,0) (0,783,0) (0,228,160) (0,664,0) (0,783,0) (0,0,0)
   kf-Pool @0x000748, erster Keyframe root(x,y,z) = (0, -1811, 0)

RE2   EM010 struct-EMR @0x02AE9C : 64 00 b0 00 0f 00 50 00
   -> bones_tbl=0x64  kf_ofs=0xB0  bones=15  kf_size=80
   Bind @0x02AEA4 : (0,-2067,0) (0,0,0) (0,252,-175) (0,730,1) (0,861,1) (0,251,176) (0,730,1) (0,861,1)
   kf-Pool @0x02AF4C, erster Keyframe root(x,y,z) = (0, -1985, 0)
```
(Dateien: `re15_port/shared_assets/PSX/EMD/CDEMD0.EMS` bzw. `.../RE2/CDEMD0.EMS`;
RE2-Blob EM010 @0x02A800 = TOC kind 0x10 / rec 3, `gen/re2_ems_toc.inc`.)

**Bein-Kette Wurzel→Fuss (Y-Summe):**

```
RE2   : 0 + 252 + 730 + 861 = 1843     (Bones 1→2→3→4)
RE1.5 : 0 + 228 + 664 + 783 = 1675     (Bones 0→1→2→3, perm-gemappt)
                       DELTA =  168
```

**Und die Wurzel-Y der ersten Keyframes: −1985 gegen −1811, DELTA = −174.**

Das ist der direkte Byte-Beweis der Invariante: **die RE2-Autoren haben die Wurzel genau um den
Betrag hoeher gesetzt, um den die Beine laenger sind.** Wurzel-Translation und Bind-Kette sind ein
Paar. Sie liegen physisch in derselben EMR-Sektion und werden nie getrennt.

### 3.5 Es gibt in keiner der beiden Engines einen Pfad, der zwei Rigs mischt

RE2-Binder `FUN_8001aaa8` verteilt die **acht** dir-Eintraege **eines** Blobs (`s2`) auf die Entity
(`ghidra_re2_Leon.txt`, selbst nachgeschlagen):

```
8001ab88: lw v1,0x0(s2)          ; dir-Tabellen-Offset
8001ab90: addu s0,s2,v1
8001aba0: sw v1,0x1bc(s1)        ; dir[0]
8001abb0: sw v1,0x17c(s1)        ; dir[1]  EDD Paar 1
8001abc0: sw v1,0x108(s1)        ; dir[2]  <<< DER EINZIGE BIND-EMR-ZEIGER
8001abd0: sw v1,0x184(s1)        ; dir[3]  EDD Paar 2
8001abe0: sw v1,0x180(s1)        ; dir[4]  kf-Pool Paar 2
8001abf0: sw v1,0x18c(s1)        ; dir[5]  EDD Paar 3
8001ac00: sw v1,0x188(s1)        ; dir[6]  kf-Pool Paar 3
8001ac14: sw v1,0x14(s1)         ; dir[7]  MD1
```

**Eine** Entity, **ein** `+0x108`. Alle drei Animationskanaele benutzen dieselbe Bind-Tabelle.
Der Port bildet das ab (`re2_ems.c re2_emd_parse_bank`: `D[2]` geht als `struct_off` in **alle
drei** `re2_parse_pair`-Aufrufe).

> **FAZIT §3: „Das Original macht das auch nicht" — belegt.** Es gibt keine Boden-Korrektur, weil es
> keinen Rig-Mix gibt. Der WELLE-G-Hybrid ist ein **reines PORT-KONSTRUKT**; die Invariante, die er
> bricht, ist §3.4. Byte-true heisst hier deshalb nicht „welche Zahl steht im Original", sondern
> „**welche Invariante garantiert das Original, und wie stellt der Port sie wieder her**".

---

## 4. WIDERLEGUNG DES KONSTANTEN VERSATZES (+168)

Pose-Sweep ueber **alle Clips aller Baenke**, Keyframe fuer Keyframe, Typ 0x10.
`D` = Fehler des Hybrids gegen das RE2-Original am tiefsten Bone (negativ = schwebt),
`Rest ADD` = was nach dem konstanten +168 uebrig bliebe:

| Bank / Clip | rootY-Bereich | **D (IST-Fehler)** | **Rest nach ADD(+168)** |
|---|---|---|---|
| LOCO 0/1 (Gehen) | −2048 … −1977 | −165 … −158 | +3 … +10 ✔ |
| LOCO 2…7 (Gehen) | −2048 … −1881 | −165 … −148 | +3 … +20 ✔ |
| MAIN 5 (liegend) | **−175 … −175** | **−40 … −10** | **+128 … +158 ✘** |
| MAIN 6 (liegend) | −461 … −175 | −64 … −25 | **+104 … +143 ✘** |
| MAIN 7 (liegend) | −461 … −175 | −56 … −20 | **+112 … +148 ✘** |
| MAIN 14 | −329 … −161 | −48 … −18 | **+120 … +150 ✘** |
| MAIN 16 | −387 … −137 | −44 … −8 | **+124 … +160 ✘** |
| MAIN 22 | −229 … −227 | −16 … −11 | **+152 … +157 ✘** |
| MAIN 23 | −226 … −221 | −19 … −18 | **+149 … +150 ✘** |
| MAIN 18/19/20 | −947 … −851 | −83 … −66 | **+85 … +102 ✘** |
| MAIN 2 (gemischt) | −1985 … −99 | **−164 … −4** | +4 … +164 ✘ |

**Der IST-Fehler laeuft von −4 bis −165.** Er ist proportional zur *aufgerichteten* Beinstellung:
steht der Zombie, traegt die volle Kette 168; liegt er, traegt sie fast nichts. Ein konstanter
Versatz waere im Gehen richtig und in **jeder** Liege-, Kriech-, Fress- und Angriffs-Pose um bis zu
160 Einheiten falsch — sichtbar bestaetigt durch §2.3 (Clip 2 bei +264, Clip 22 bei +99).

Auf Sohlen-Ebene (das, was man sieht) ist die Spanne noch groesser: **−7 … −219** bei den
betroffenen Clips, und **positive** Werte bis +461 bei den Liege-Clips.

**Der Vorschlag (A) des Vor-Dossiers ist damit zurueckgewiesen.**

Ebenfalls geprueft und verworfen — **multiplikativ** (Wurzel-Y × chain15/chain2, Faktor
1675/1843 = 0.9088): Restfehler **−32 … +30** ueber alle Clips. Deutlich besser als ADD, aber immer
noch ein Modell statt einer Messung: es unterstellt „Wurzel-Y ist proportional zur Beinlaenge", was
fuer eine liegende Pose (rootY = −175) schlicht nicht gilt.

---

## 5. DER RICHTIGE WERT — berechnet, nicht gewaehlt

### 5.1 Ableitung

Die Kette lautet in beiden Spielen und im Port identisch:

```
Punkt_Welt.y  =  e->y  +  Wurzel_Y(Keyframe)  +  SUM ueber die Kette ( R_Eltern * Bind_Offset ).y
                          ^ aus Rig A                                  ^ aus Rig A
```

Der Hybrid ersetzt **nur** die `Bind_Offset` durch die RE1.5-Werte und laesst `Wurzel_Y` aus RE2
stehen. Da die **Rotationen** in beiden Faellen dieselben sind (die kommen aus dem RE2-Keyframe),
ist die Differenz exakt:

```
K(kf) = SUM ueber die Kette ( R_Eltern(kf) * (Bind_RE2 − Bind_RE15) ).y
```

Das ist keine Modellannahme, sondern eine algebraische Identitaet — und sie ist **pro Keyframe
messbar**, indem man dieselbe Pose zweimal aufbaut (einmal mit den RE2-Bindwerten, einmal mit den
RE1.5-Bindwerten) und die Differenz des tiefsten Punktes nimmt. Genau das tut die Sonde.

### 5.2 Zwei Ausbaustufen — beide byte-abgeleitet

| Variante | Bezugspunkt | Datenquelle | Restfehler |
|---|---|---|---|
| **K_bone** | tiefster **Bone** | nur die zwei Bind-Tabellen (0x0006A0 / 0x02AEA4) | die Mesh-Tiefendifferenz (RE1.5-Fuss ist ~20 flacher) |
| **K_mesh** | tiefster **Vertex** | zusaetzlich die beiden MD1-Sektionen | **0** — reproduziert die RE2-Bodenlage exakt |

`K_mesh` setzt voraus, dass beim RE2-Zombie `Mesh-Index == Bone-Index` gilt. Das ist belegt: die
RE2-EM010-MD1 hat **17** Meshes bei **15** Bones, und die beiden ueberzaehligen sind die
GORE-Reserven, die der Zerleger als `part_mesh` stempelt (`main.c` Kommentar „GORE-GRENZE: der
Zerleger stempelt part_mesh = RESERVE-Mesh 15 … 16. Mesh"). Slots 0…14 sind 1:1.

### 5.3 GEGENMESSUNG (STOP-GATE Schritt 4) — die Sohle NACH der Korrektur

Typ 0x10, LOCO-Bank (die Gehen-Clips = genau das, was der Nutzer sieht):

| Clip | RE15 nativ | RE2 nativ | **HYB heute** | K_bone | **HYB + K_bone** | **HYB + K_mesh** |
|---|---|---|---|---|---|---|
| 0 | +3 … +11 | −21 … +47 | **−206 … −129** | +158…+165 | −41 … +29 | **−21 … +47** |
| 1 | +6 … +74 | −20 … +47 | **−205 … −129** | +158…+165 | −40 … +29 | **−20 … +47** |
| 2 | 0 … +35 | −36 … +60 | **−211 … −116** | +151…+160 | −58 … +40 | **−36 … +60** |
| 3 | 0 … +35 | −36 … +60 | **−211 … −116** | +151…+160 | −58 … +40 | **−36 … +60** |
| 4 | −6 … +65 | −33 … +32 | **−212 … −138** | +152…+165 | −53 … +17 | **−33 … +32** |
| 5 | −6 … +65 | −30 … +32 | **−210 … −138** | +152…+165 | −50 … +17 | **−30 … +32** |
| 6 | — | −34 … +87 | **−216 … −87** | +148…+165 | −54 … +68 | **−34 … +87** |
| 7 | — | −34 … +87 | **−216 … −87** | +148…+165 | −54 … +68 | **−34 … +87** |

Und in den Liege-/Angriffs-Clips der MAIN-Bank, wo ADD versagt:

| Clip | HYB heute | **HYB + K_bone** | **HYB + K_mesh** | (ADD +168 haette ergeben) |
|---|---|---|---|---|
| 5 (liegend) | +111 … +415 | +140 … +454 | +124 … +383 | +279 … +583 ✘ |
| 6 (liegend) | +182 … +461 | +207 … +518 | +221 … +442 | +350 … +629 ✘ |
| 15 | +209 … +249 | +262 … +290 | +270 … +299 | +377 … +417 ✘ |
| 0 (stehend) | −184 … −168 | −23 … −7 | −3 … +13 | −16 … 0 |

**`HYB + K_mesh` reproduziert Spalte „RE2 nativ" Zeile fuer Zeile exakt** — per Konstruktion, und
das ist genau das gewuenschte Ergebnis: der Hybrid-Zombie haelt dieselbe Bodenlage, die das
RE2-Original haelt, nur mit RE1.5-Geometrie. `HYB + K_bone` bleibt um die genuine Mesh-Differenz
(~20 Einheiten = **0,7 %** der Koerperhoehe) darunter und liegt damit immer noch innerhalb des
Bandes, das der RE1.5-Flavor mit seinen eigenen Clips selbst einnimmt (ROOM1030: +6 … +127;
ROOM1140: +4 … +200).

**Antwort auf „WELCHER Wert, mit Beleg":** kein Skalar. `K` ist eine **Tabelle mit einem int16 pro
Keyframe**, erzeugt zur Ladezeit aus
`shared_assets/RE2/CDEMD0.EMS @0x02AEA4 ff.` (RE2-Bind),
`shared_assets/PSX/EMD/CDEMD0.EMS @0x0006A0 ff.` (RE1.5-Bind) und dem Keyframe-Pool
`@0x02AF4C ff.`. Fuer den Steh-Frame faellt daraus 168 heraus, fuer den liegenden 10, fuer den
Hund 0 — **automatisch, ohne dass irgendwo eine Zahl eingetippt wird.**

---

## 6. DIE STELLE IM PORT — und wie ein Fix aussehen muesste

### 6.1 Wo der Fehler entsteht

| Datei / Zeile | Rolle |
|---|---|
| `re15_port/platform/pc/main.c:668-676` | ruft `pc_enemy_hybrid_re15_models` im RE2-Flavor **bedingungslos** |
| `re15_port/platform/pc/main.c:505-540` | laedt das RE1.5-EMD und ruft `re2_hybrid_apply` |
| `re15_port/engine/src/re2_ems.c:185-211` `re2_hybrid_rig_skel` | **ersetzt `bone_relative_pos[i]` durch die RE1.5-Werte** — und laesst die Wurzel-Translation der Keyframes unangetastet |
| `re15_port/engine/src/re2_ems.c:231-233` | wendet das auf `skel`, `skel_loco`, `skel_own` an |
| `re15_port/engine/src/skeleton_common.c:242-278` | liest die Keyframe-Wurzel und setzt sie als `poses[0].trans` (Zeilen 680-682) |

Der Kommentar in `re2_hybrid_rig_skel` benennt die Falle sogar korrekt („der Root-Trans kommt aus
dem Keyframe") und zieht die falsche Konsequenz: **weil** er aus dem Keyframe kommt, muss er
mit-konvertiert werden, wenn die Kette darunter getauscht wird.

### 6.2 Wie der Fix aussehen muesste

1. **Tabelle bei `re2_hybrid_apply` erzeugen — VOR dem Ueberschreiben.**
   `re2_hybrid_apply` haelt an dieser Stelle beides gleichzeitig: die RE2-Bind-Tabelle (noch nicht
   ueberschrieben) und die RE1.5-Tabelle (`skel15`), dazu das RE2-MD1 (`eb->md1`, noch nicht durch
   `eb->md1 = *md15` ersetzt, `re2_ems.c:241`). Fuer jede Skelett-Kopie und jeden Keyframe ihres
   Pools zweimal posieren und die Differenz des tiefsten Punktes als `int16` ablegen.
   Aufwand EM10: MAIN und OWN teilen sich einen Pool (1513 kf), LOCO hat 1812 → **~3325 int16 =
   6,6 KB je Zombie-Typ**. Der ganze Block sitzt in `re2_ems.c`, das bereits `#ifdef
   RE15_PLATFORM_PC` gekapselt ist → **null Kosten fuer das RAM-kritische PSX-Target**.

2. **Anwenden ausschliesslich im RENDER-Pfad**, in `skeleton_common.c` unmittelbar nach
   `re15_emd_get_keyframe_position(skel, keyframe_index, …)` (Zeile 242) **und** nach dem
   Tween-Zweig-Lesen `kf_from` (Zeile 246) — also **vor** dem Crossfade-Lerp (Zeilen 272-278), damit
   `prev_root` konsistent bleibt und keine Uebergangs-Rampe springt. Nur `kf_py`, niemals `kf_px` /
   `kf_pz` (X/Z sind byte-true die Root-Motion, §3.2).

3. **Verschluesselt ueber den Skelett-ZEIGER, nicht ueber den Typ.** Die Nachschlage-Funktion darf
   nur fuer die drei umgebauten Hybrid-Kopien einen Wert ≠ 0 liefern. Damit gilt automatisch:
   RE1.5-Flavor unberuehrt, Spieler unberuehrt, Nicht-Hybrid-Typen (0x1A, 0x26, 0x27, 0x40-0x4B)
   unberuehrt, und die **Victim-Bank bleibt aussen vor** — sie posiert LEON und wird vom Hybrid
   bewusst nicht umgebaut (`re2_ems.c:234-236`).

### 6.3 Was der Fix NICHT anfassen darf (die Stellen, die kippen wuerden)

* ⛔ **NICHT** in `re15_emd_get_keyframe_position` selbst korrigieren. Dieser Leser hat einen
  Gameplay-Konsumenten: `re2z_root_py` (`enemy_ai_re2_zombie.c:5360-5379`) liefert die
  Keyframe-Wurzel-Y an die byte-true RE2-Leichen-Bounce-Physik
  (`(e->re2z_gy232 − 200 − rooty) < e->y` @0x80106EB4-C0, und `+0x15A` @0x80107270,
  `enemy_ai_re2_zombie.c:5536/5629`). Die erwartet den **RE2**-Wurzelwert. Wird dort korrigiert,
  aendert sich die Absprunghoehe der Leiche — eine echte Regression im byte-true Teil.
* ✔ `enemy_ai_common.c:7870-7873` liest denselben Aufruf, benutzt aber nur `rx`/`rz` (die
  XZ-Root-Motion, `e->x/e->z` @0x8011bfe4/@0x8011c004). Y-Korrektur beruehrt ihn nicht.
* ✔ `enemy_ai_common.c:3290` (Kriecher-Matrix `@0x801094a0`) nimmt die Wurzel als
  **Translationsvektor der Bone-0-Matrix**. Da liegt der Wert im selben Raum wie die Renderpose —
  wird die Renderpose korrigiert, muss diese Matrix mitziehen oder bewusst ausgenommen werden.
  **Das ist der einzige Punkt, an dem der Fix nicht trivial lokal ist; er gehoert vor der
  Implementierung gemessen** (Kriecher in ROOM1010, Typ 0x10).
* ⛔ **Nicht in die Asset-Bytes schreiben.** Die Bank aliast das residente EMS (`eb->buf = NULL`,
  `main.c:468`; im Hybrid-Pfad zeigt `md1` in einen eigenen Puffer, der **Keyframe-Pool** aber
  weiterhin ins gemeinsame RE2-EMS). Deshalb Seiten-Tabelle, keine In-Place-Korrektur.
* ⛔ Kein Env-Schalter mit Default-AUS. Im Original steht der Zombie auf dem Boden, im Port nicht →
  das ist falsch und wird gefixt (Memory `reai-v2-original-oder-nicht`).

### 6.4 Abnahme-Kriterium fuer den Fix (vorab festgelegt, damit nichts „aussieht wie")

1. `probe_rig_sohle sweep 10|11|16|20|21` — die Spalte **HYB** muss Zeile fuer Zeile in der Spalte
   **RE2** liegen (K_mesh) bzw. maximal 20-40 Einheiten darueber (K_bone).
2. `probe_rig_sohle room 1030 1` — Sohle im Gehen **≥ −40** statt heute −133 … −219.
3. `probe_rig_sohle room 1030 0`, `room 1140 0`, `room 10D0 0` — **unveraendert** (RE1.5-Flavor
   darf sich um kein Bit bewegen).
4. `probe_rig_sohle room 1230 1` (Hund) und `room 10C0 1` (Kraehe) — **unveraendert**.
5. `local_build.sh` 230/230.
6. gdigrab-Lauf am echten Fenster (Skill `re15-port-visual-verify`), ROOM1030 im RE2-Modus.

---

## 7. OFFEN — ehrlich

1. **Kein gdigrab-Beweis in diesem Dossier.** Die Kette ist rechnerisch identisch mit dem Renderer
   (gleiche `re15_skel_compute_pose`, gleiche Vertex-Transformation, gleiches `mesh_remap`), aber
   nicht gefilmt. Fuer die Abnahme eines Fixes ist der Lauf Pflicht.
2. **Zombie Girl 0x13 hat ZUSAETZLICH ein anderes Problem.** Gemessen (Sweep): das **RE2-Original**
   EM013 steht in seinen Steh-Clips selbst bei Sohle **−243 … −210**, also 230 ueber der Ebene
   `y=0`; der Hybrid landet bei −368 … −352, und die Ketten-Korrektur deckt davon nur ~113 ab.
   Entweder ist RE2-kind 0x13 ein anderes Wesen als der RE1.5-Typ 0x13, oder EM013 laeuft im
   RE2-Original mit einem `e->y ≠ 0`. **0x13 kommt in STAGE1 nirgends vor** (§2.5), der Punkt ist
   also nicht dringend — aber er ist offen und darf nicht als „mit erledigt" verbucht werden.
3. **Die Kriecher-Matrix `enemy_ai_common.c:3290`** (§6.3) ist als Wechselwirkung identifiziert,
   aber nicht durchgemessen.
4. **`K_mesh` haengt an der Annahme Mesh-Index == Bone-Index fuer die RE2-Zombie-MD1.** Begruendet
   (Slots 15/16 = GORE-Reserve), aber nicht durch RE2-Disasm des Zeichners `FUN_80027160`
   verifiziert. `K_bone` braucht die Annahme nicht — deshalb ist `K_bone` die risikoaermere Stufe.
5. **Nur der Zombie-Sweep ist vollstaendig.** Fuer Hund und Kraehe ist gemessen, dass die Korrektur
   ~0 ist; die Rest-Clips wurden nicht einzeln bewertet.
6. Unveraendert offen aus den Vor-Dossiers: `+0x1B8` (Liege-Latch, @0x80105540/@0x801055EC) hat im
   Port weiterhin keinen Konsumenten.

---

## 8. ARTEFAKTE

* **Neue Sonde:** `re15_port/tests/unit/probe_rig_sohle.c` + Eintrag in
  `re15_port/tests/unit/CMakeLists.txt` (kein `add_test`).
  `sweep <typehex>` (alle Clips aller Baenke, RE15/RE2/HYB + beide Korrektur-Kandidaten +
  korrigierte Sohle) · `room <roomhex> <0|1>` (echter Raumlauf, 420 Ticks) ·
  `chain <typehex>` (Bind-Ketten + Wurzel-Bind beider Rigs).
* **Vorhandene Schwester-Sonde:** `re15_port/tests/unit/probe_rig_root_y.c` (Modus 3 liefert die
  ADD/MUL-Restfehler-Tabelle aus §4).
* **Disasm, selbst gezogen**
  (`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis <addr> <n>`):
  `0x8001f3bc` (Keyframe-Wurzel → Modell), `0x8001ac38` (Anker, X/Z), `0x8001ad68` (Anwender, X/Z),
  `0x8001ae38` (Delta-Extraktor, schreibt nur `0(a3)`/`4(a3)`), Aufrufer-Gate von `FUN_800390e0`
  @0x80038a80 / @0x80038d30. RE2-Binder `FUN_8001aaa8` @0x8001ab88-0x8001ac14 aus
  `ghidra_re2_Leon.txt` (Zeilen 91905-91945).
* **Asset-Bytes:** `shared_assets/PSX/EMD/CDEMD0.EMS` EMR @0x000698 (Kopf), Bind @0x0006A0,
  kf-Pool @0x000748 · `shared_assets/RE2/CDEMD0.EMS` EMR @0x02AE9C, Bind @0x02AEA4,
  kf-Pool @0x02AF4C (Blob EM010 @0x02A800).
* **Build:** `bash re15_port/tools/local_build.sh` → **230/230**.
