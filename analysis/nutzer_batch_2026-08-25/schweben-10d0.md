# FINDING 2 (RE2-KI, ROOM10D0): „der Zombie schwebt nach dem Aufstehen leicht in der Luft"

**Dossier 2026-08-25. ⛔ Keine Code-Aenderung in dieser Phase** — neu im Baum ist ausschliesslich
die Mess-Sonde `re15_port/tests/unit/probe_10d0_float_y.c` (+ CMake-Eintrag, **kein `add_test`**).

---

## 0. ERGEBNIS IN DREI SAETZEN

1. **Der Zombie schwebt gemessen 170–205 Einheiten ueber dem Boden** (Sohle des Fuss-Meshes,
   Welt-Y; der RE1.5-Zombie steht bei −30…+10, also praktisch auf 0).
2. **Es ist KEIN Uebergabe-Fehler des 10D0-Sitzers.** `e->y` ist ueber die ganze Sequenz
   konstant **0** und `e->floor` konstant **0** — in beiden Flavors, in jedem einzelnen Tick.
   Die Gegenprobe in **ROOM1140** (stehende Zombies, die den Sitz-Import NIE beruehren) schwebt
   **identisch**. Der Fehler ist nicht 10D0-spezifisch und nicht v0.3.21-neu; er steckt seit
   **23c20b0f (Welle G, 2026-08-19)** in **jedem** RE2-Flavor-Zombie.
3. **Ursache: der WELLE-G-HYBRID tauscht die Bein-Kette gegen die RE1.5-Kette, laesst aber die
   Wurzel-Translation der RE2-Keyframes stehen.** Die RE2-Bein-Kette ist **1843** Einheiten lang,
   die RE1.5-Kette **1675** — Differenz **168**. Weil die Wurzel (Brust) vom RE2-Keyframe gehalten
   wird und die Beine daran haengen, kommen die Fuesse genau um diese Differenz nach OBEN.
   Erzeugt in `re15_port/engine/src/re2_ems.c:185-211` (`re2_hybrid_rig_skel`), gerufen aus
   `re2_hybrid_apply` (:231-233), verdrahtet in `platform/pc/main.c:675`.

---

## 1. METHODE (STOP-GATE Schritt 1 — MESSEN, nicht modellieren)

Sonde: `re15_port/tests/unit/probe_10d0_float_y.c`. Echter Weg ueber `re15_game_step`, echte
`ROOM10D0.RDT`/`ROOM1140.RDT`-Spawns, echte RE2-Bank aus `shared_assets/RE2/CDEMD0.EMS`,
echte RE1.5-Bank aus `shared_assets/PSX/EMD/CDEMD0.EMS`.

**Was die aeltere Sonde `probe_10d0_situp_re2` NICHT tut und was hier den Unterschied macht:**
sie wendet den **WELLE-G-HYBRID nicht an**. Im Spiel laeuft er im RE2-Flavor **bedingungslos**
(`platform/pc/main.c:668-676`, der frueher vorhandene „RE2 MODELS"-Gate ist entfernt). Ohne ihn
misst man ein Skelett, das im Spiel gar nicht existiert — und genau in ihm sitzt der Fehler.

Gerechnet wird die Hoehe exakt wie im Renderer:

```
bone_world = rotate(pose[bone].trans, yaw) + (e->x, e->y, e->z)     platform/pc/main.c:6968-6979
pose[0].trans          = die Wurzel-Translation AUS DEM KEYFRAME    skeleton_common.c:676-683
pose[b].trans (b != 0) = parent_rot * bone_relative_pos + parent_trans
```

Bankwahl exakt wie in `main.c` (Sitz-Import-Zweig `main.c:6817-6830`, Loco-Zweig
`main.c:6752-6759`).

**Modi:** `0` 10D0/RE2 · `1` 10D0/RE1.5 · `2` 1140/RE2 · `3` 1140/RE1.5 · `4` 10D0/RE2 **ohne**
Hybrid · `5` Bind-Offsets vorher/nachher · `6` Ketten-Delta ueber alle Hybrid-Typen ·
`7` Mesh-Tiefe RE2 vs. RE1.5.

**Messehrlichkeit:** `SOHLE` ist der Bone mit dem groessten Welt-Y (PSX-Y zeigt nach unten) und
wird ohne FRAC-Crossfade gerechnet, `b0y`/`b8y` mit. In den zitierten Ruhelagen ist `frac = 0`,
dort sind beide identisch. Die Mesh-Tiefe (§4) ist im **Bone-lokalen** Frame gemessen, also fuer
einen stark verdrehten Fuss eine Naeherung.

---

## 2. MESSREIHE — ROOM10D0, RE2-Flavor (Modus 0), die ganze Sequenz

`e->y = 0` und `e->floor = 0` in **jeder** Zeile. Bank 1 = RE1.5-Sitz-Pose-Bank, Bank 2 = Loco.

| Tick | Phase | st/+0x5/+0x6 | Clip | Bank | Wurzel-Bone `b0y` | Fuss-Bone (Welt-Y) |
|---|---|---|---|---|---|---|
| t0…t19 | Sitzen | 1/0x12/1 | 0x2A | 1 | −327 | **−14** (b13) |
| t0 | Wecken (dist 1693) | 1/0x12/3 | 0x2A | 1 | −327 | −14 |
| t16 | Wecken | 1/0x12/3 | 0x2A | 1 | −711 | −127 (b6) |
| t27 | Wecken Ende | 1/0x12/4 | 0x2A | 1 | −763 | −(Hold) |
| t28 | Uebergabe Schlaf→Aufsteher | 1/**0x0d**/0 | 0x2A | 1 | −763 | — |
| t29 | Aufstehen Start | 1/0x0d/1 | **0x29** | 1 | −763 | — |
| t87 | Aufstehen Ende | 1/0x0d/2 | 0x29 f58 | 1 | −1727 | **−203** (b3) |
| **t88** | **Uebergabe an RE2** | 1/**0x01**/1 | **0** | **2** | −1744 | **−399** (b4) |
| t92 | RE2-WALK | 1/0x01/1 | 0 f4 | 2 | −1907 | −400 |
| t104 | RE2-WALK (frac=0) | 1/0x01/1 | 0 f16 | 2 | −2048 | **−407** |
| t113 | RE2-WALK | 1/0x01/1 | 0 f25 | 2 | −1977 | −391 (b7) |
| t119…t133 | RE2-BUMP | 1/0x03/3 | 12 | 0 | −1929…−1989 | −381…−388 |

**Der Sprung sitzt in t87 → t88: der Fuss-Bone geht von −203 auf −399 und bleibt dort.**
Das ist **kein** Anim-Ruckler und **kein** Crossfade-Artefakt (bei t104 ist `frac = 0`), sondern
die neue, dauerhafte Steh-Hoehe des RE2-Rigs.

**Gegenprobe RE1.5-Flavor, derselbe Raum, derselbe Zombie (Modus 1):**

```
t87  clip 41 f58  b0y=-1727  Fuss-Bone b3 = -203
t88  clip 41 f58  b0y=-1727  Fuss-Bone b3 = -203    (RE1.5-Engage-Uebergang)
t89  clip 4  f29  b0y=-1734  Fuss-Bone b3 = -219
t104 clip 4  f44  b0y=-1751  Fuss-Bone b6 = -194
```

→ Der RE1.5-Zombie bleibt bei −193…−232. **Die Differenz zum RE2-Flavor ist ~200 Einheiten,
dauerhaft.**

---

## 3. DIE ENTSCHEIDENDE GEGENPROBE: ROOM1140 (nie durch den Sitz-Import)

`probe_10d0_float_y 2` (RE2) vs. `3` (RE1.5), erster Zombie 0x10, `e->y = 0`, `floor = 0`:

```
RE2-Flavor  t44 clip21 f41  b0y=-1877  Fuss-Bone b4 = -359
            t48 clip21 f45  b0y=-1922  Fuss-Bone b4 = -362
RE15-Flavor t48 clip41 f47  b0y=-1614  Fuss-Bone b3 = -201
            t56 clip41 f55  b0y=-1717  Fuss-Bone b3 = -202
```

→ **Auch der ROOM1140-Zombie, der die 10D0-Sitz-Maschine nie sieht, schwebt.**
Damit ist der Uebergabe-Tick als Ursache **widerlegt**. Ebenso widerlegt:

* **Boden-/Band-Wert** (`-(pc[4]*1800)`, Schreiber `sh v0,442(s0)` = `+0x1BA` @**0x80042210**;
  Rechnung @0x800421F8-0x8004220C: `sll 3 / subu / sll 5 / addu / sll 3 / subu zero` = `-(v1*1800)`).
  Gemessen: `e->floor = 0` → Referenz 0, in **beiden** Flavors gleich. Die Aktor-Position selbst
  kommt aus dem Sce_em_set-Record (`lh 6(s2) → sw 52(s0)` @0x8004217C, `lh 8(s2) → sw 56(s0)`
  @0x80042188, `lh 10(s2) → sw 60(s0)` @0x80042194) — im Port `pos=(5878,0,25694)`, Y = 0.
* **Ein Sitz-spezifischer Rest-Offset:** es gibt keinen. Der Sitz-Import setzt kein Y-Feld
  (`enemy_ai_common.c:4887-4932`), und `re15_re2z_exec_only` → `re2z_exec_dispatch`
  (`enemy_ai_re2_zombie.c:3077-3106`) → `re2z_exec_walk` schreibt **kein** `e->y`.
* **Ein v0.3.21-Regress:** der Hybrid stammt aus **23c20b0f (2026-08-19, Welle G)**, der
  Uebergabe-Fix aus 508e71d0 (2026-08-24). Der Schwebe-Fehler ist aelter; v0.3.21 hat ihn nur
  **sichtbar gemacht**, weil der Zombie vorher am Ende des Aufstehens erst noch in die
  Liege-Pose zusammenbrach (das war FINDING 3a von 2026-08-24) und der Blick des Nutzers dort
  haengen blieb.

---

## 4. URSACHE — mit Zahlen und Byte-Offsets

### 4.1 Wie das ORIGINAL die Hoehe zusammensetzt

Die Wurzel-Translation kommt aus dem **Keyframe**, nicht aus dem Bind-Skelett. Selbst
disassembliert, `FUN_8001f3bc` (PSX.EXE):

```
8001f3f8: lhu v0,2(a0)          ; EMR: Keyframe-Basis
8001f414: mflo v0 / sll v0,2    ; Keyframe-Index * kf_size
8001f420: addu a2,a2,v0         ; a2 = Zeiger auf DEN Keyframe
8001f424: lh  v0,0(a2)          ; Wurzel X   aus dem Keyframe
8001f42c: sw  v0,44(s1)         ;   -> Modell +0x2C
8001f430: sh  v0,56(s1)         ;   -> Vor-Pose (Crossfade)
8001f434: lh  v0,2(a2)          ; Wurzel Y   aus dem Keyframe   <<<<
8001f43c: sw  v0,48(s1)         ;   -> Modell +0x30
8001f440: sh  v0,58(s1)
8001f448: lh  v0,4(a2)          ; Wurzel Z
8001f450: sw  v0,52(s1)         ;   -> Modell +0x34
```

Der Port bildet das 1:1 ab (`skeleton_common.c:676-683`: „Root translation = keyframe px/py/pz
ONLY … does NOT add the EMR bind"). Alle **anderen** Bones haengen ueber ihr
`bone_relative_pos` (Bind-Laenge aus dem Struktur-EMR) an ihrem Eltern-Bone.

**Damit gilt im Original wie im Port:**

```
Fuss-Welt-Y = e->y  +  Keyframe-Wurzel-Y  +  Sum(rotierte Bind-Offsets Wurzel->Fuss)
                        ^^^ aus der EDD/EMR-Bank      ^^^ aus DERSELBEN Bank
```

Beide Summanden stammen im Original **immer aus demselben Rig**. Der Hybrid bricht genau das.

### 4.2 Die zwei Ketten, byte-belegt

Bind-Offsets (`x,y,z`, s16, Stride 6) im Struktur-EMR:

**RE1.5 EM10** — `re15_port/shared_assets/PSX/EMD/CDEMD0.EMS`, EM10-Blob @0x000000
(EMS-Index 0, `re15_ems.c:63-67`):

| Bone | Datei-Offset | (x, y, z) | Rolle |
|---|---|---|---|
| 0 | 0x0006A0 | (0, −1751, 0) | Wurzel — **wird nie gelesen** |
| 1 | **0x0006A6** | (0, **228**, −160) | Oberschenkel R |
| 2 | **0x0006AC** | (0, **664**, 0) | Unterschenkel R |
| 3 | **0x0006B2** | (0, **783**, 0) | Fuss R |
| 4 | 0x0006B8 | (0, 228, 160) | Oberschenkel L |
| 5 | 0x0006BE | (0, 664, 0) | Unterschenkel L |
| 6 | 0x0006C4 | (0, 783, 0) | Fuss L |
| 7 | 0x0006CA | (0, 0, 0) | Hueft/Brust-Umwurzelung |

**RE2 EM010** — `re15_port/shared_assets/RE2/CDEMD0.EMS`, EM010-EMD-Blob @**0x02A800**
(TOC-Eintrag kind 0x10 / rec 3 = Sektor 0x55 × 0x800, Groesse 0x23DCC,
`gen/re2_ems_toc.inc:10-11`; Binder `FUN_8001aaa8` @0x8001ab7c-80):

| Bone | Datei-Offset | (x, y, z) |
|---|---|---|
| 0 | 0x02AEA4 | (0, −2067, 0) — Wurzel, nie gelesen |
| 1 | 0x02AEAA | (0, **0**, 0) |
| 2 | **0x02AEB0** | (0, **252**, −175) |
| 3 | **0x02AEB6** | (0, **730**, 1) |
| 4 | **0x02AEBC** | (0, **861**, 1) |
| 5 | 0x02AEC2 | (0, 251, 176) |
| 6 | 0x02AEC8 | (0, 730, 1) |
| 7 | 0x02AECE | (0, 861, 1) |

```
RE2-Kette   Wurzel -> Fuss : 0 + 252 + 730 + 861 = 1843
RE1.5-Kette (permutiert)   : 0 + 228 + 664 + 783 = 1675
                                          DELTA  =  168      <<< der Betrag der Schwebe
```

Die Permutation `k_perm_zombie` (`re2_ems.c:160`) mappt RE2-Bone → RE1.5-Bone:
`{7,0,1,2,3,4,5,6,14,8,9,10,11,12,13}` — RE2-Bone 4 ist also **derselbe physische Fuss-Bone**
wie RE1.5-Bone 3. Die Messreihen vergleichen Aepfel mit Aepfeln.

### 4.3 Der Port-Code, der es erzeugt

`re15_port/engine/src/re2_ems.c:185-211`:

```c
static int re2_hybrid_rig_skel(re15_emd_skeleton_t *dst, const int8_t *perm, int n,
                               const re15_emd_skeleton_t *s15)
{
    for (int i = 0; i < n; i++) {
        int p = (int)dst->bone_parent[i];
        if (p < 0) continue;   /* Wurzel: bone_relative_pos[0] wird nie gelesen —
                                * der Root-Trans kommt aus dem Keyframe. */
        ...
        dst->bone_relative_pos[i][*] = s15->bone_relative_pos[mi][*];   /* <<< Kette getauscht */
    }
}
```

Der Kommentar benennt die Falle sogar korrekt („der Root-Trans kommt aus dem Keyframe") und
zieht dann die falsche Konsequenz: **weil** die Wurzel aus dem RE2-Keyframe kommt und die Kette
darunter auf RE1.5 umgestellt wird, ist die Gesamt-Hoehe inkonsistent. Angewandt auf **alle
drei** Skelett-Kopien (`:231-233`), verdrahtet in `platform/pc/main.c:675`
(`pc_enemy_hybrid_re15_models`), das im RE2-Flavor **immer** laeuft (`main.c:668-676`).

### 4.4 Gegenprobe im Experiment: Hybrid AUS

`probe_10d0_float_y 4` (identischer Lauf, nur `re2_hybrid_apply` nicht gerufen):

| Tick | Wurzel `b0y` | Fuss-Bone, Hybrid **AUS** | Fuss-Bone, Hybrid **AN** | Δ |
|---|---|---|---|---|
| t88 | −1744 | −239 | −399 | 160 |
| t92 | −1907 | −239 | −400 | 161 |
| t96 | −2014 | −239 | −402 | 163 |
| t100 | −2040 | −242 | −406 | 164 |
| t104 | −2048 | −243 | −407 | **164** |
| t107 | −2038 | −243 | −407 | 164 |

Die Wurzel `b0y` ist in beiden Laeufen **bit-identisch** (−2048 bei t104) — der Keyframe wird
nicht angefasst. Der ganze Versatz kommt aus der Kette. Gemessen 158–165 statt der 168 aus der
geraden Kette, weil das Knie im Walk-Clip leicht gebeugt ist (`cos`-Anteil).

### 4.5 Der Rest: die Mesh-Tiefe (`probe_10d0_float_y 7`)

Tiefster Vertex des Fuss-Meshes unter seinem Bone (Bone-lokal, MD1):

```
RE2-bone4 (RE15-mesh 3): RE2 = 223   RE1.5 = 203   D = -20
RE2-bone7 (RE15-mesh 6): RE2 = 223   RE1.5 = 203   D = -20
```

Damit schliesst sich die Rechnung vollstaendig — **Sohle = Fuss-Bone-Welt-Y + Mesh-Tiefe**,
Steh-Pose (`frac = 0`):

| Konfiguration | Fuss-Bone | Mesh-Tiefe | **Sohle ueber Boden** |
|---|---|---|---|
| RE1.5-Flavor (RE1.5-Rig + RE1.5-Mesh) | −203 | +203 | **0** — steht |
| RE2 **ohne** Hybrid (RE2-Rig + RE2-Mesh) | −243 | +223 | **−20** — steht |
| **RE2 mit Hybrid** (RE2-Wurzel + RE1.5-Kette + RE1.5-Mesh) | −407 | +203 | **−204 — schwebt** |

Dass die beiden konsistenten Konfigurationen bei 0 bzw. −20 landen und nur die gemischte bei
−204, ist die Bestaetigung, dass die Messkette selbst stimmt.

Ueber die Bewegung schwankt der Wert mit der Kniebeugung: gemessen **−170 bis −206**
(10D0-Walk t88…t113). Bei einer Koerperhoehe von ~2800 Einheiten (Brust-Bone) sind das ~7 % —
genau „leicht in der Luft".

---

## 5. SKOPUS — wen trifft es noch? (`probe_10d0_float_y 6`)

Aufsummierte Y-Kette Wurzel→Bone, vor/nach dem Hybrid:

| Typ | Fuss-Kette vor → nach | Δ Fuss | Δ Kopf | Δ Hand |
|---|---|---|---|---|
| **EM10 / EM11 / EM16** (Zombies) | 1843 → 1675 | **−168** | +116 | −37/−38 |
| **EM13** (Zombie Girl) | 1621 → 1508 | **−113** | −11 | −25 |
| **EM20** (Hund) | (nur Bone 4) | −44 | — | — |
| **EM21** (Kraehe) | ±2 | ~0 | ~0 | ~0 |

→ Der Fehler ist ein **Zombie-Familien-Fehler**. Alle erwachsenen RE2-Zombies (0x10/0x11/0x16,
und nach derselben Tabelle 0x12/0x18) schweben 168, das Zombie-Girl 113, der Hund 44, die
Kraehe praktisch nicht.

Beachte die Asymmetrie: der **Kopf** wandert +116 nach UNTEN. Das RE1.5-Modell ist schlicht
~11 % kleiner als das RE2-Modell (Spannweite Fuss→Kopf 2626 vs. 2342); weil die Wurzel (Brust)
vom RE2-Keyframe festgehalten wird, verteilt sich die Groessendifferenz auf „Fuesse hoch" und
„Kopf runter". Ein Fix muss also am **Boden** referenzieren, nicht an der Wurzel.

---

## 6. FIX-RICHTUNGEN (nicht Teil dieser Phase)

Es gibt **kein Original** fuer „RE1.5-Geometrie unter RE2-Keyframes" — der Hybrid ist ein
PORT-KONSTRUKT (Welle G). Byte-true ist hier also nicht „welche Zahl steht im Original", sondern
**„welche Invariante garantiert das Original und wo bricht der Port sie"**. Die Invariante ist
§4.1: Wurzel-Translation und Bind-Kette stammen aus demselben Rig.

**(A) Wurzel-Y der RE2-Keyframes um die Ketten-Differenz nachziehen** (empfohlen).
Beim Hybrid-Umbau die Kette Wurzel→Fuss in beiden Rigs summieren und die Differenz auf die
Y-Komponente jeder Keyframe-Wurzel des betroffenen Pools addieren. Der Wert ist **nicht
geraten**, er ist aus zwei zitierten Byte-Bloecken abgeleitet:
`168 = (252+730+861 @RE2/CDEMD0.EMS 0x02AEB0/B6/BC) − (228+664+783 @PSX/EMD/CDEMD0.EMS 0x6A6/AC/B2)`,
und er faellt fuer jeden Typ automatisch aus den Tabellen (EM13 → 113, EM20 → 44, EM21 → 0).
Gegenprobe an der Messreihe: `−407 + 168 = −239` → Sohle `−239 + 203 = −36`, mitten im
RE1.5-Band (−30…+10). Der Keyframe-Pool aliast heute das residente EMS (`eb->buf = NULL`,
`main.c:470`) — eine Korrektur braucht also eine eigene Kopie, sonst wird das gemeinsame
Asset veraendert.

**(B) Die Bind-Kette gar nicht tauschen.** Dann sitzt der Fuss richtig, aber die RE1.5-Meshes
haengen an RE2-Gelenkabstaenden — sichtbare Luecken an Knie/Knoechel (10 % zu lang). Genau
deshalb existiert der Hybrid ueberhaupt; kein Weg zurueck.

**(C) Uniforme Skalierung** der RE1.5-Kette auf die RE2-Laenge (Faktor 1843/1675 = 1.100).
Macht den RE1.5-Zombie 10 % groesser als im RE1.5-Modus — verletzt das Praesentations-Mandat
(„Modelle = RE1.5") und passt nicht zu den unveraenderten Mesh-Groessen.

**Zusatz, unabhaengig:** die verbleibenden 20 Einheiten (RE2-Fuss-Mesh 223 vs. RE1.5 203,
§4.5) sind eine echte Modell-Eigenschaft und muessen **nicht** kompensiert werden — mit (A)
landet der RE1.5-Fuss dort, wo ihn das RE1.5-Rig auch im RE1.5-Modus hinstellt.

---

## 7. OFFEN (ehrlich)

1. **Kein gdigrab-Beweis.** Die Schwebe ist ueber die exakte Renderer-Bank-/Keyframe-/Bind-Kette
   gemessen, aber nicht am echten Fenster gefilmt (Skill `re15-port-visual-verify`). Fuer die
   Abnahme eines Fixes gehoert der Lauf dazu — auch, weil ~200 Einheiten je nach Kamera-Cut
   unterschiedlich stark auffallen.
2. **Die Mesh-Tiefe ist Bone-lokal gemessen** (§4.5), nicht rotiert. Fuer die Steh-Pose ist der
   Fuss nahezu achsparallel, fuer stark verdrehte Posen (Sturz, Kriechen) waere die Zahl neu zu
   messen.
3. **Die Sonde rendert pro Tick zweimal** (einmal mit, einmal ohne Crossfade), was die
   Vor-Pose-Kette waehrend einer laufenden `frac`-Rampe leicht stoert. Alle zitierten
   Ruhelage-Zahlen stehen bei `frac = 0` und sind davon nicht betroffen.
4. **Nur die Loco-Bank ist durchgemessen.** `skel`/`skel_own` bekommen denselben Umbau
   (`re2_ems.c:231-233`) und dieselben Bind-Werte, also gilt dasselbe Delta — aber die
   Keyframe-Wurzeln ihrer Clips sind nicht Tick-fuer-Tick vermessen.
5. **Der Hund (−44) und das Zombie-Girl (−113)** sind gemessen, aber nicht als Sohlen-Hoehe
   verifiziert (andere Perm-Tabelle, andere Mesh-Tiefen).
6. `+0x1B8` (Liege-Latch, @0x80105540/@0x801055EC) hat im Port weiterhin keinen Konsumenten —
   unveraendert offen aus `zombie_lyer_10d0.md` §5.2 und `10d0-aufstehen.md` §4.4.

---

## 8. ARTEFAKTE

* Sonde: `re15_port/tests/unit/probe_10d0_float_y.c` + Eintrag in
  `re15_port/tests/unit/CMakeLists.txt` (kein `add_test`).
  Modi: `0` 10D0/RE2 · `1` 10D0/RE1.5 · `2` 1140/RE2 · `3` 1140/RE1.5 · `4` 10D0/RE2 ohne
  Hybrid · `5` Bind-Offsets · `6` Ketten-Delta je Typ · `7` Mesh-Tiefe.
* Disasm (selbst gezogen, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`):
  `dis 0x8001f3bc 60` (Keyframe-Wurzel → Modell), `dis 0x80042170 10` /
  `dis 0x800421e0 20` (Spawn-Position + `+0x1BA = -(floor*1800)` @0x80042210).
* Asset-Byte-Offsets: `PSX/EMD/CDEMD0.EMS` 0x0006A0-0x0006CA (RE1.5 EM10 Bind-Kette),
  `RE2/CDEMD0.EMS` 0x02AEA4-0x02AECE (RE2 EM010 Bind-Kette, Blob @0x02A800).
* Build: `bash re15_port/tools/local_build.sh build` (GCC 16.2, gruen).
