# Endkampf-Birkin (EM36): „bewegt sich nicht" + „Modell nicht komplett" — MESSLAUF (2026-09-12)

Nutzer-Befund (v0.7.86, ROOM5090-Endkampf, RE2-EM36 unter RE1.5-0x36-KI):
„Bei Birkin glaube ich ist das Modell immernoch nicht komplett. Außerdem bewegt er
sich nicht."

Mess-Werkzeug dieser Runde (beide env-gegated, bleiben drin):
* `RE15_BIRKIN_DBG=1` → `birkin_dbg.log`:
  * KI-Zeile alle 15 Ticks am Ende von `re15_birkin_ai_tick`
    (enemy_ai_common.c, hinter dem +0x1d8-Snapshot @0x80116698-a0):
    `tick/st/sub/ph2/ph3/clip/af/pos/rot/dist/hp/grid/flags/atk_cd/stun/timer`.
  * DRAW-Zeile alle 60 Frames je gezeichnetem Part des 0x36 in der NPC-Schleife
    (platform/pc/main.c, nach `nbone_world_trans`): Part-Index, Weltposition.
* Läufe: `RE15_DEBUG_JUMP=5090@gp` (+ für den Kampf-Nachbau `RE15_FORCE_EVENT=4@1200`
  = ROOM5090 sub04, der echte Kampfstart-Event), Build via `local_build.sh`.
* Sichtläufe per gdigrab (Skill re15-port-visual-verify): `shots/birkin_{a,b,c}.png`.

---

## 0. Der echte Kampf-Ablauf (SCD, selbst dekodiert)

ROOM5090.RDT, mit der verifizierten Opcode-Tabelle (SCDScriptDisassembler.java):

* **sub00 @0x0124A:** `44 01 30 33` — Spawn Slot 1, Typ 0x30 (Port: →0x36,
  scd_vm.c), **grid 0x33**, Pos **(−14700, 0, −23350)** = Mittellinie Wagen C.
* **sub01 @0x1290:** `23 00 0a 00 0c 00` = `Cmp(work_vars[0x0A] == 12)` →
  `Evt_exec(ff,18,04)` + `Set(3,0x2a,1)`. work_vars[0x0A] IST der ANGEZEIGTE
  Kamera-Cut (Port-Spiegel: room_common.c:51-62, Original @0x80021bfc
  `sh v1,DAT_800b0fe4`). **Der Kampf startet, wenn der Spieler Kamera-Cut 12
  betritt.**
* **sub04 @0x12F2 (der Kampfstart):**
  * `Work_set(2,1)` → Aktor-Slot 2 = der Birkin (em-Slot 1 → Aktor idx+1).
  * **`Pos_set(1200, 0, −23350)`** @0x12FE — Boss-Startpose.
  * `Member_set(0x0c, 0x13)` @0x130A — **Member 0x0c = grid_id** (gemessen:
    grid 0x30 → 0x13 im Log genau am Teleport-Tick): Form-2-Bit + Nibble 3.
  * `Cut_chg(0x0f)` (Cutscene-Kamera), BGM-Steuerung,
  * `Work_set(1,0)` + **`Plc_dest(mode 9, x=300, z=−23400)`** @0x1320 — der
    SPIELER wird zur Kampfmarke gelaufen (Do/Edwhile-Warteschleife),
  * `Aot_reset(4, …)` @0x1334 (Fluchtsperre), `Sleep 30`, **`Cut_chg(0x0C)`**
    @0x1342 = die Kampfkamera, Flags lösen, `Plc_ret`.

⇒ Kampf-Design: Boss (1200, −23350) vs. Spieler (300, −23400) — **Start bei
Distanz ≈ 900**, beide im Kampfkorridor (Streifen C, begehbar z −26884..−19870).

---

## 1. Messlauf A — Sprung ohne Kampfstart (der Zustand VOR Cut 12)

Lauf: `RE15_BIRKIN_DBG=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
RE15_DEBUG_JUMP=5090@gp ./re15_pc.exe` (Debug-Spawn des Spielers ≈ (280, 5000),
rückgerechnet aus dist+rot).

birkin_dbg.log (435 Zeilen, Auszug):

```
tick=0    st=1 sub=9 ... clip=0  pos=(-14700,0,-23350) grid=0x33   ; INIT→EMERGENCE (grid&0xf==3)
tick=15.. sub=9 clip=16→13 pos konstant                             ; Emergence spielt ~200 Ticks
tick=210  sub=1 clip=1 pos=(-14515,0,-23257) dist=31924             ; WALK, 30 Einheiten/Tick
tick=915  pos=(-4611,0,-4262) dist=10492                            ; er LÄUFT (quer durch den Zug)
tick=1005 pos=(-3490,0,-2381) dist=8318                             ; ⛔ WAND
tick=1005..6510  pos oszilliert (-3490|-3503, -2381), dist=8318     ; >5500 Ticks = FÜR IMMER
```

**Befund A1 — die KI läuft und BEWEGT den Boss** (kein Freeze, kein Gate, kein
Clip-Mapping-Stillstand): WALK = `re15_dog_advance(e, 0x14+10)` (Form-2-Bit aus
grid 0x33) = 30 Einheiten/Tick, byte-true zur 0x30-Wurzel (+0x8c += 0x14
@0x801172ac, +10 @0x801172dc).

**Befund A2 — der Boss VERKEILT SICH DAUERHAFT an Innengeometrie.** Der WALK
steuert stur auf den Spieler (`re15_enemy_steer_point` 0x20 @0x80117268) — null
Pfadsuche, byte-true zur RE1.5-Wurzel (deren Original-Arena ROOM3070 ist ein
leerer Raum). ROOM5090 ist ein Drei-Wagen-Zug mit Zwischenwänden; die
SCA-Klemme (`re15_collision_constrain_enemy`, Radius 1000 aus der byte-true
0x30-Box {0,-1440,0,1000,1440,1000} @0x8011ee64) wirft ihn jeden Tick zurück →
±13-Einheiten-Oszillation auf der Stelle, gemessen ununterbrochen von Tick 1005
bis Prozessende (>3 Minuten). **Ein Spieler, der dem Boss VOR dem
Kampfstart-Event begegnet (oder dessen Kamera ihn zeigt), sieht einen Boss, der
sich nicht bewegt.**

Referenzrahmen RE2 (g5-optik.md §3.1): RE2s G5 wandert NIE vor dem Kampf — alle
fünf room7040-Entities sind bei (−32000, 0, −32000) GEPARKT, bis der
Handler-INIT sie platziert. Das Pendant „Boss läuft ab Raumladung frei durch den
Zug" existiert in RE2 nicht.

## 2. Messlauf B — der echte Kampf (sub04 erzwungen)

Lauf wie A plus `RE15_FORCE_EVENT=4@1200`. Log:

```
tick=735  pos=(-3465,0,-14244)                       ; wandert noch (Richtung Debug-Spieler)
tick=750  pos=(1368,0,-23403) grid=0x13              ; ⛔ sub04-Pos_set + Member_set greifen
tick=765..1545  sub=1 clip=1 pos: x 1815→10103, z≈-23410, 30/Tick, dist 11808→3519
                                                     ; er LÄUFT 8,3 km den Korridor entlang
tick=1560 sub=3 (CLAW) dist=3176 pos=(10422,0,-23439); Angriff in Reichweite 0xc80
tick=1560..1740+ sub=3 ph2 1→4, clip 3→10, pos KONSTANT, dist KONSTANT 3176
                                                     ; ⛔ Klauen-Schleife auf der Stelle
```

Und die Part-Sicht (alle 60 Frames), z. B. Frame 1500:

```
DRAW part=0/4 welt=(8624,-1520,-23377)   ; Kriecher-Wurzel ≈ -4534+2950 = -1584 ✓
DRAW part=1/4 welt=(10767,-720,-23508)   ; Bone-1 (+2146 lokal) ✓
DRAW part=2/4 welt=(10424,   24,-23492)  ; BLOB — Ursprung am Boden (Soll ≈ -34) ✓
DRAW part=3/4 welt=(9423,-1023,-23428)   ; Arm (~1 m Höhe) ✓
```

**Befund B1 — der Kampfstart funktioniert mechanisch:** Pos_set/Member_set/
Plc_dest laufen (Work_set(2,1)→Slot 2, scd_vm.c scd_work_bind), der Boss läuft
danach kilometerweit und greift an. Auch der Raum-eigene Trigger feuerte im
Lauf (zweiter Teleport, als Cut 12 anlag — sub01→sub04, einmalig per Set(3,0x2a)).

**Befund B2 — alle 4 Parts werden gezeichnet, an den geplanten Orten** (Klammer
mesh_count=4; Kriecher-Wurzel −1584, Blob-Sohle am Boden, Arm auf 1 m). „Nicht
komplett" ist also KEIN Zeichenschleifen-/Lade-Defekt → §3 (Sichtlauf).

**Befund B3 — die Nahkampf-Schleife steht auf der Stelle und sieht aus wie
Stillstand.** Bei dist 3176 (zwischen Klauen-Trefferfenster 2500 und
Klauen-Trigger 0xc80=3200) kettet der Boss Klaue→Recovery→Klaue, Position
eingefroren, gemessen >180 Ticks ohne Ende:
* Klaue clip 3 → BKMAP[3]=6 → RE2-Clip 6 = **100 Frames**;
* Recovery clip 0x0a → BKMAP[0xa]=0 → **RE2-Clip 0 = 100-Frame-IDLE-Wippen**.
⇒ Ein Angriffszyklus = ~200 Ticks (6,6 s), davon 3,3 s gemapptes Idle = der
Boss „steht nur da". (RE1.5-EM030-Clips sind deutlich kürzer; die BKMAP hat für
Recovery/viele Nicht-Angriffs-Clips nur das Idle als Ziel.)

## 3. Sichtlauf (gdigrab, echtes Fenster) — was der Nutzer sieht

`shots/birkin_a.png` (Lauf-Phase), `birkin_b.png` / `birkin_c.png` (Nahkampf,
je 3 s Abstand):

* **Sichtbar ist ausschließlich die MASSE (Mesh 2)** — eine dunkle Fleischwand
  mit Zahn-Spikes und radialem Muster, texturiert (CLUT-Stapel funktioniert;
  3 CLUT-Zeilen via clut_y − clut_base_y, render_pc.c:2157-2160).
* **Der Kriecher (der erkennbare G5-Körper: Kopf/Maul/Arme) ist in KEINEM Shot
  zu identifizieren.** Geometrie-Grund (BBoxen, birkin-em36.md §1.4): entlang
  der Blickachse reicht die Masse (lokal −1800..+4494 um Bone 0) **2350
  Einheiten ÜBER den Kriecher-Kopf (Bone 1 @ lokal +2146) hinaus** und ist mit
  4,26 m höher als der Kriecher (Körper bis 3,3 m) — von vorn ist der Kriecher
  IN der Masse begraben.
* **In b/c steckt der SPIELER mitten in der Masse-Silhouette** (Leon vor dem
  radialen Innenmuster): die RE1.5-Distanzen (Angriff <0x9c4/0xc80) messen zum
  ENTITY-URSPRUNG, die Masse-Front läuft aber ~4494 Einheiten voraus — der Boss
  schiebt die Masse ÜBER den Spieler. RE2 tut das nie: der Advance stoppt hart
  an der Linie x=4000 (`if (entity_x > 4000) entity_x -= zaehler`
  @0x80101934-4c) und die Angriffs-Ziellogik misst zur BLOB-Weltposition
  (part2+0x5C/+0x60, @0x80101db8-dc), nicht zur Entity.
* ⇒ Nutzer-Erlebnis im Nahkampf: bildfüllende, kaum bewegte Fleischwand ohne
  erkennbaren Boss = **„Modell nicht komplett" + „bewegt sich nicht"** in einem.

**Präsentationsseite (A/B-Test RE15_G5_FLIP):** RE2 rückt in −X vor und der
Spieler steht bei kleinerem X — RE2 zeigt der Kamera also die lokale −X-Seite
des Rigs; der Port dreht per steer die lokale +X-Seite zum Spieler. Der Schalter
`RE15_G5_FLIP=1` (main.c, Typ-0x36-Render-Yaw +0x800) stellt die RE2-Seite ein.
ERGEBNIS: A/B im Kampf-Nahbild NICHT abschließbar — im Flip-Lauf verkeilte sich
der Boss (U-A) an der Wagen-Zwischenwand bei z=−20858 (dist 25824) und stand
außerhalb der Spielerkamera (shots/fd_flip*.png zeigen den leeren Korridor).
Die Geometrie-Aussage steht dennoch: der 180°-Flip vertauscht nur, welche
lokale X-Seite der Masse führt — die Masse (±3648 in Z, −1800..+4494 in X um
Bone 0) bleibt auf JEDER Seite höher (4,26 m) und weiter vorn als der Kriecher
und verdeckt ihn. Der Flip allein löst „Modell nicht komplett" nicht; nötig ist
die Masse-Stopplinie (Schritt 2), die den Kriecher aus der Masse-Silhouette
heraushebt. Der Schalter bleibt env-gegatet für einen späteren Nahbild-A/B.

---

## 4. Wurzelursachen (2)

**U-A „bewegt sich nicht":** Der Boss ist ab RAUMLADUNG frei (INIT→EMERGENCE→
WALK), Stunden bevor der Kampf startet — und verkeilt sich mangels Pfadsuche
dauerhaft an den Zug-Innenwänden (gemessen §1: >5500 Ticks Oszillation an
(−3490, −2381)). Zusätzlich friert im Kampf selbst die Position in der
Klauen-Schleife bei 2500<dist<3200 ein, deren Recovery aufs 100-Frame-Idle
gemappt ist (§2 B3). RE2 parkt den Boss bis zum Kampfstart (@0x801011d0-dc,
RDT-Spawns (−32000,0,−32000)).

**U-B „Modell nicht komplett":** Die Komposition zeigt dem Spieler die Masse
IMMER als führende, höhere Wand; der Kriecher liegt entlang der Blickachse
2350 Einheiten HINTER der Masse-Front und ist nie sichtbar (§3). Verstärker:
die Entity-zentrierte Distanz lässt die Masse den Spieler überrollen (RE2:
Stopp-Linie + Blob-referenzierte Distanz, §3).

## 5. Plan (jeder Schritt mit Beleg)

1. **Boss bis zum Kampfstart HALTEN** (U-A, Hauptfix): Typ 0x36 in
   ROOM5090/5091 bleibt in einer Warte-Pose (clip 0, keine Bewegung), bis der
   Kampfstart ihn freigibt. Freigabe-Signal = dasselbe Prädikat, das der Raum
   selbst benutzt: `work_vars[0x0A] == 12` (sub01 @0x1290) — oder robuster: das
   sub04-`Member_set(0x0c,0x13)` (grid==0x13 ist NUR nach sub04 wahr; gemessen
   §2). RE2-Beleg fürs Parken: §1 Schluss. Umsetzung im 0x36-Zweig von
   `re15_birkin_ai_tick` (raumgebunden, dokumentierte Port-Wahl — die
   RE1.5-0x30-Wurzel bleibt für 3070 unangetastet).
2. **Nahkampf-Distanzmodell an die Masse anpassen** (U-B/B3): WALK-Advance für
   0x36-in-5090 stoppt, sobald die MASSE-Front den Spieler erreicht
   (Entity-Dist ≈ 4494 + Klauenreichweite), und die Angriffs-/Fenster-Distanzen
   der 0x36-Brücke werden um den Masse-Vorlauf versetzt — RE2-Beleg:
   Advance-Stopplinie @0x80101934-4c, Ziel-Distanz zur Blob-Weltposition
   @0x80101db8-dc/0x80102008-38. (Damit endet auch die Klauen-Schleife: der
   Spieler steht dann im Trefferfenster STATT in der Grauzone 2500..3200.)
3. **BKMAP-Recovery-Ziele schärfen** (B3): 0x0a (Recovery) und die übrigen
   aufs Idle gemappten Indizes auf kürzere RE2-Clips legen (Kandidaten: 7/8 =
   50-Frame-Angriffscluster als Recovery-Ersatz; Katalog birkin-em36.md §2.1) —
   Feinabnahme visuell (re15-port-visual-verify), es gibt kein byte-true
   Original dieser Kombination.
4. **Präsentationsseite** nach dem RE15_G5_FLIP-A/B (§5-Nachtrag) festnageln;
   wenn der Kriecher auf keiner Seite sichtbar wird, ist die RE2-Sichtbarkeit
   Morph-abhängig (dir[0]-Vertex-Morph, g5-optik.md §3.4) → dann Kriecher-
   Sichtbarkeit über die Masse-Stopplinie (Schritt 2) herstellen: bei RE2-
   Distanz ragt der Kriecher über die Masse-Silhouette (Kuppe liegt HINTER der
   Front, §4-Tabelle in g5-optik.md).
5. **Messschienen bleiben** env-gegated drin: `RE15_BIRKIN_DBG` (KI-Zeile +
   DRAW-Zeile), `RE15_G5_FLIP` (A/B). Kein Verhalten ohne env.

## 6. Nebenbefunde

* Der Debug-Spawn von ROOM5090 liegt bei ≈(280, 5000) — weit außerhalb des
  Kampfkorridors; Messläufe für diesen Kampf brauchen deshalb den
  sub04-Weg (RE15_FORCE_EVENT=4) oder einen Spieler im Streifen C.
* Beide Messläufe beendeten sich nach einigen Minuten selbst (Exit 0,
  RE15_CONTINUE_TEST-Harness) — für Langzeitmessungen einplanen.
* Parallele Sitzung im Baum während dieser Runde (HEAD 2d39fb21→4cdf63f1,
  enemy_ai_re2_spider.c zeitweise unkompilierbar) — eigene Dateien nur
  explizit adden.
