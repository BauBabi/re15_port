# BEFUND B — Gruppe „fress-sync": Leon steckt beim Fress-Finisher nicht im Maul

Nutzer 2026-09-12 (ROOM2090-Alligator): *„leon steckt bei der Todesanimation immer
noch nicht dann im Aligator Maul und wird rumgeschleudert, das ist noch sehr
asynchron."*

Der Finisher ist CUSTOM-Design (enemy_ai_boss_gator.c `GBP_FRESSEN`, Nutzer-Design
2026-09-11); es gibt hier also KEINE @0x-Wahrheit fuer die Bahn — die Wahrheit ist
die GERENDERTE Kieferposition des Ports selbst. Genau die wurde gemessen (nicht
gedeutet): eine neue Mess-Schiene loggt pro FRESSEN-Frame den analytischen
Bahn-Anker, Leons Wurzel und die gerenderten Kiefer-Bones. Messlauf archiviert:
`analysis/befunde_runde4_2026-09-12/fress-sync_messlauf.log` (199 Zeilen, davon
185 FSYNC).

## Kurzfassung (Wurzelursache)

**Falscher Anker in der HOEHE, um ~2300–2450 Einheiten.** Die Wirbelbahn haengt an
einem ANALYTISCHEN Maulpunkt (`kx/kz` = Wurzel + 2600 voraus, y hart −400→−700 =
„Maulhoehe beim gesenkten Kopf", enemy_ai_boss_gator.c:1826-1827/1851/1864-1869).
Waehrend des Wurfs ist das GERENDERTE Maul aber gar nicht unten: `pitch_vz`
schnellt in 15 F auf −260 (Kopf hochgerissen, :1859-1862), und der Schnapp-Clip 4
(45 F, HOLD-LAST ab af≥45, anim_select_common.c:104-116) friert den Kopf in der
aufgebaeumten Endpose ein. Gemessen steht die Maulmitte damit von t=110 bis t=173
UNVERAENDERT auf y=−3143 — der Anker der Leon-Bahn zielt auf −400…−700. Leon
fliegt also eine Parabel zur WASSEROBERFLAECHE, waehrend das offene Maul ~2400
Einheiten darueber haengt: exakt „steckt nicht im Maul und wird rumgeschleudert".

Horizontal ist der Anker fast richtig (Restfehler ~297: anker (−3398,−23857) vs.
Maulmitte (−3307,−24140)) — das Problem ist die ignorierte Render-Kette
(pitch_vz + Clip-Pose + Scale 2/3) in der Y-Achse.

---

## 1. Die Mess-Schiene (neu, bleibt im Baum)

### 1.1 Warum `re15_enemy_bone_world_pos` allein hier LUEGT

Zwei gemessene Eigenschaften des Query-Pfads (re15_damage.c:1969-1987):

1. Er NULLT `g_anim_pose_actor` (re15_damage.c:1979 „QUERY: don't mutate the
   crossfade blend") — dadurch ist in `re15_skel_compute_pose` `bact == NULL`
   (skeleton_common.c:212), und der Boss-Zusatzwinkel-Hook
   `if (bact && bact->type == 0x23u) az += re15_gator_spine_arc_vz(bact, b)`
   (skeleton_common.c:602-603) posiert NICHT. pitch_vz/jaw_vz/arc_vz fehlen der
   Query-Pose komplett.
2. Er skaliert nicht: der Renderer legt den Entity-Render-Scale +0x166 (Gator
   GB_SCALE_Q12=2731, gesetzt enemy_ai_boss_gator.c:857) auf die 3×3-Rotations-
   matrix (main.c:8155-8159, byte-true ScaleMatrix-Pfad FUN_8001e8c8
   @0x8001e904-40) — Bone-Offsets skalieren in x, y UND z um die Wurzel. Die
   Query gibt Full-Size-Offsets (1,5× zu weit draussen).

### 1.2 `gb_bone_welt` — gerenderte Bone-Weltposition (enemy_ai_boss_gator.c:384-404)

Dieselbe Kette wie der NPC-Renderer: `re15_compute_actor_kf` →
`g_anim_pose_actor = e` (Render-Konvention main.c:8119) →
`re15_skel_compute_pose` (vz-Hook aktiv) → `re15_skel_bone_to_world` →
Uniform-Scale um die Wurzel in allen drei Achsen (main.c:8155-Semantik).

Nebenwirkungsfrei im FRESSEN-Kontext: der Pose-Lauf mit gesetztem `bact`
schreibt zwar `prev_root/prev_angles` (skeleton_common.c:296/325) und wuerde bei
aktivem Crossfade dessen Abklingrate verdoppeln — der Boss setzt `e->motion`
aber DIREKT (nie ueber einen Crossfade-armenden Setzer), `anim_frac` bleibt 0,
`blend` (skeleton_common.c:213) ist nie aktiv. Im Sweep-Betrieb
(`RE15_GB_STUMM=1`) laeuft die Schiene gar nicht (s_fs bleibt NULL).

### 1.3 FSYNC-Logzeile (enemy_ai_boss_gator.c:1901-1930)

Am Ende des FRESSEN-Case, NACH `e->anim_frame++` (= der Stand, den der Renderer
dieses Frame posiert), pro Frame nach `gator_boss.log`:

```
FSYNC t=<timer> anker=(kx,kz) pl=(x,y,z) b6=(…) b7=(…) maul=(…) dpl=<3D-Dist> pitch=… jaw=… mo=… af=…
```

`b6`/`b7` = gerenderte Bones 6 (Kopf, Kette 1→5→6) und 7 (Unterkiefer, der
jaw_vz-Bone) — Hierarchie selbst geparst, Datei-Kopf enemy_ai_boss_gator.c:19.
`maul` = Mitte(b6,b7), `dpl` = |pl − maul| in 3D.

### 1.4 Treiber: `probe_gator_fress.c` (tests/unit, ctest `unit_gator_fress`)

Echter Boss-Tick + Dispatcher-Klemme (Muster probe_gator_sweep.c:165-172), mit
GELADENER RE2-EM23-Bank aus CDEMD0.EMS (22 Bones verifiziert im Lauf; ohne Bank
faellt bone_world_pos auf die Wurzel zurueck und taeuscht — Lehre
reai-v2-re2-trace-datei). `RE15_GB_TEST=1` (aggro sofort; die Werte 2/3/5 des
Hebels schalten Cross/Todespfad/Rampen-Repro, KEINER triggert FRESSEN direkt —
enemy_ai_boss_gator.c:884-905). FRESSEN wird deshalb ueber den echten Weg
erreicht: Leon mit hp=30 im Wasser, erster Biss = 50 Schaden (dmg_table[5]
@DAT_8006f418) → hp=−20 <0 → `gb_biss_abschluss` → GBP_FRESSEN
(enemy_ai_boss_gator.c:346-352). Bestehen = Sequenz laeuft bis `pl->no_draw==1`.
Lauf 2026-09-12: `OK`, Suite `gator|alligator` 3/3 gruen.

---

## 2. Messreihe: die Asynchronitaet beziffert

Wirbel-Fenster t=96..185 (90 Frames, Leon fliegt / wird verschlungen):

| Metrik | Wert |
|---|---|
| dpl > 300 | **90/90 Frames** (kein einziger Frame „im Maul") |
| dpl > 500 | 83/90 |
| dpl > 1000 | 40/90 |
| dpl > 2000 | 15/90 |
| Minimum | 368 (t=96, Wurfstart) bzw. 496 (t=135-138, Parabel-Scheitel) |
| **Schnapp 2 (t=174, der Fang-Moment)** | **dpl = 2352** |
| **Verschlingen (t=185, no_draw)** | **dpl = 3184** |

Schluessel-Frames (aus fress-sync_messlauf.log):

```
t=95  pl=(-4000,    0,-22000)  maul=(-3669, -490,-23952)  dpl=2048   (Maul UNTEN, Kopf gesenkt)
t=96  pl=(-3398, -511,-23857)  maul=(-3717, -577,-24022)  dpl= 368   (Wurfstart: Leon TELEPORTIERT 731 horizontal an den Anker)
t=110 pl=(-3398,-1796,-23857)  maul=(-3243,-3142,-24118)  dpl=1392   (pitch-Flip fertig: Maul in 15F von y=-577 auf y=-3142 hochgerissen)
t=136 pl=(-3398,-2751,-23857)  maul=(-3307,-3143,-24140)  dpl= 496   (Leons Parabel-SCHEITEL bleibt 392 UNTER dem Maul)
t=174 pl=(-3398, -804,-23857)  maul=(-3270,-3133,-24137)  dpl=2352   (Schnapp 2 klappt 2352 UEBER dem gelandeten Leon zu)
t=185 pl=(-3398, -804,-23857)  maul=(-3508,-3972,-24058)  dpl=3184   (Leon verschwindet an der Wasseroberflaeche, nicht im Maul)
```

Dazu: von t=110 bis t=173 steht die gerenderte Maulmitte EINGEFROREN auf exakt
(−3307,−3143,−24140) — Clip 4 haelt HOLD-LAST (af 46..99 > 45 Frames,
anim_select_common.c:104-116), pitch konstant −260. 63 Frames lang wirbelt Leon
gegen einen stehenden, aufgebaeumten Kopf.

### 2.1 Ursachen, den Messwerten zugeordnet

1. **Falscher Anker (dominant, ~2300-2450 in Y):** Bahn-Endpunkt y ist hart
   −400→−700 („Maulhoehe beim gesenkten Kopf", :1844-1851/1864-1869) — gueltig
   fuer die FLACHE Pose (gemessen t≤95: Maul-y −309…−490 ✓), aber ab t=96 ist
   der Kopf per pitch −260 + Schnapp-Endpose aufgebaeumt: Maul-y = −3143. Der
   Anker modelliert einen Zustand, den der Renderer zu diesem Zeitpunkt nicht
   mehr zeigt.
2. **Phasenversatz am Wurfstart (t=96..110):** das Maul schnellt in 15 F von
   −577 auf −3142 hoch (pitch 350→−260, :1859-1862), Leons Parabel steigt mit
   ~100/F — dpl waechst 368→1392. Das Maul eilt Leon VORAUS (nach oben weg).
3. **Amplituden-Mismatch:** Parabel-Hub 2200 + Slope −300 (:1864-1869) toppt
   Leon bei −2751 aus — selbst der Scheitel bleibt 392 unter der Maulmitte;
   die Landung (−700-Linie) liegt 2440 darunter.
4. **Horizontal nahezu richtig:** konstanter Restfehler ~297 (2600er-Vorhalt
   vs. gerenderte ~2500 + seitlicher Clip-Versatz ~283). Kein eigenstaendiger
   Fix noetig, faellt mit der Bone-Kopplung automatisch weg.

### 2.2 Sekundaerbefunde (gemessen, gleiche Reihe)

* **Leichen-Teleport bei t=96:** Leon springt in einem Frame von seinem Todesort
  (−4000,0,−22000) an den Anker (−3398,−511,−23857) = 731 Einheiten horizontal +
  511 vertikal. `wirbel_x/z` wird bei t=95 auf kx/kz gesetzt (:1851) statt auf
  Leons Leiche — der Lerp startet damit AM ZIEL statt an der Leiche.
* **Kopf-runter-Phase konvergiert nicht (t=1..75):** der 48/F-Vorwaertsschub
  (:1836-1841) schiebt entlang der AKTUELLEN Blickrichtung, waehrend steer nur
  mit Slew 0x60 nachdreht (:1822) — der Gator UMKREISTE Leon in diesem Lauf 60+
  Frames (Maul-Distanz horizontal nie < ~2000; Schnapp 1 bei t=75 biss 2096
  entfernt ins Leere). Der Wurfstart uebernimmt den Versatz als Teleport (s.o.).
* **Doppelter Schnapp-1:** `gb_biss_abschluss` startet Clip 4 schon beim
  FRESSEN-Eintritt (:350-351), t=75 startet ihn erneut (:1842-1843) — der
  Kommentar-Timeline nach ist t=75 „Schnapp 1", faktisch ist es der zweite.

---

## 3. PLAN — Leon-Wirbel DIREKT an den gerenderten Kiefer koppeln

Kernaenderung (enemy_ai_boss_gator.c, Case GBP_FRESSEN :1812-1899): die
analytische Ziel-Konstante durch die LIVE-Maulmitte ersetzen. `gb_bone_welt`
existiert seit heute (:384-404) und ist exakt die Render-Wahrheit.

1. **Wurfstart t=95 (:1844-1851):**
   `g->wirbel_x/y/z = pl->x / pl->y / pl->z` (die LEICHE, nicht kx/kz) — beendet
   den 731er-Teleport; der Lerp traegt Leon von der Leiche ins Maul.
2. **Flug t=96..174 (:1852-1874):** pro Frame
   ```c
   int32_t b6[3], b7[3], mzx, mzy, mzz;
   gb_bone_welt(e, 6, b6); gb_bone_welt(e, 7, b7);
   mzx=(b6[0]+b7[0])/2; mzy=(b6[1]+b7[1])/2; mzz=(b6[2]+b7[2])/2;  /* LIVE-Maul */
   pl->x = g->wirbel_x + (int64_t)(mzx - g->wirbel_x) * t / T;
   pl->z = g->wirbel_z + (int64_t)(mzz - g->wirbel_z) * t / T;
   pl->y = g->wirbel_y + (int64_t)(mzy - g->wirbel_y) * t / T
         - (int32_t)((int64_t)4 * HUB * t * (T - t) / ((int64_t)T * T));
   ```
   Das Ziel folgt damit AUTOMATISCH dem pitch-Flip (t=96..110), der
   HOLD-Pose (t=110..173) und Schnapp 2 (t=174) — jeder kuenftige Anim-Umbau
   bleibt synchron, weil die Bahn an der gerenderten Pose haengt.
   `HUB` neu bemessen: das Ziel liegt schon ~2400 hoeher als der alte Anker;
   Ueberhoehung uebers Maul nur noch ~500-700 (Design-Wert, im Sichtlauf
   abnehmen — 2200 wuerde Leon ~2200 UEBER den Kopf schleudern).
   Die Raum-Klemme (:1831-1832) bleibt als Sicherung auf pl->x/z.
3. **Fang t=174..184:** Leon bleibt AN der Maulmitte gepinnt (kein Parabel-Term
   mehr, `t/T`-Lerp ist bei 174/80 laengst gesaettigt → einfach `pl = maul`),
   sodass Schnapp 2 sichtbar UM ihn zuklappt; t=185 `no_draw` unveraendert
   (:1875-1888).
4. **Kopf-runter-Phase (Sekundaerbefund, kleiner Zusatz):** den 48/F-Schub
   (:1836-1841) nur ausfuehren, wenn die Blickrichtung grob zur Leiche zeigt
   (re15_ai_arc-Fenster) ODER den Schub-Vektor auf `(pl - maul)` statt
   „vorwaerts" legen — beendet das gemessene Umkreisen; Schnapp 1 (t=75) und
   Wurfstart finden dann AN der Leiche statt.
5. **Abnahme = dieselbe Schiene:** `unit_gator_fress` laufen lassen; Kriterium
   aus der FSYNC-Reihe: dpl ≤ ~300 bei t=174 und t=184 (Fang-Beats), kein
   Frame > ~HUB+300 im Fenster 96..174, kein Positionssprung > ~150/Frame bei
   t=96 (Teleport-Waechter). Danach Sichtlauf nach Skill
   re15-port-visual-verify (gdigrab), denn „Zahl gut" ersetzt das Nutzer-Bild
   nicht (Lehre reai-v2-tabelle-vs-bild).

### 3.1 Byte-true-Alternative (groesser, spaeter): das RE2-OPFER-RIG

EM23.EMD traegt ein eigenes Verschling-Paar: dir[5]/dir[6] = 2 Clips
[30, 120 F] auf EMR3 mit 15 Bones = Spieler-Skelett
(analysis/re2_alligator_2026-09-09/DOSSIER.md „Anim-Paar 3", dort als
„Verschling-Sequenz" identifiziert, Muster des 0x36-Endbosses). RE2
synchronisiert Opfer und Alligator also DATENSEITIG — auffaellig: Gator-Clip 5
hat ebenfalls 120 F (Clip-Tabelle ebd.); ob beide frame-locked als Paar laufen,
ist NICHT disassembliert (Victim-FSM im EM23-Overlay = offener Punkt 4 des
Dossiers) — Hypothese, vor einer Umsetzung am Overlay belegen
(`re2_disasm.py dis … --bin EM23_OVL_0000.BIN`). Das waere der Weg, den
Custom-Wirbel komplett durch die Original-Choreografie zu ersetzen; Punkt 3
oben repariert das heutige Design unabhaengig davon.

---

## 4. Geaenderte/neue Dateien (Messrunde, kein Verhaltens-Fix)

* `re15_port/engine/src/enemy_ai_boss_gator.c` — +`#include re15_anim_select.h`
  (:55), +`gb_bone_welt` (:384-404), +FSYNC-Zeile im FRESSEN-Case (:1901-1930).
  Verhalten des Kampfs UNVERAENDERT (nur Telemetrie; unter RE15_GB_STUMM aus).
* `re15_port/tests/unit/probe_gator_fress.c` — neuer Treiber (ctest
  `unit_gator_fress`, TIMEOUT 120).
* `re15_port/tests/unit/CMakeLists.txt` — Registrierung.
* `analysis/befunde_runde4_2026-09-12/fress-sync_messlauf.log` — archivierter
  Messlauf (Referenz fuer die Vorher/Nachher-Abnahme).

Suite-Stand nach Umbau: `ctest -R "gator|alligator"` 3/3 gruen (inkl. Sweep,
der die Schiene via RE15_GB_STUMM nicht sieht).
