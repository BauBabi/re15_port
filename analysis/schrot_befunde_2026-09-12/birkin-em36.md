# Endkampf-Birkin (RE2 EM36) — „fliegt in der Luft" + „unvollständig" (2026-09-12)

Nutzer-Befund zum ROOM5090-Endkampf mit dem RE2-Modell EM036 (Typ 0x36, Port-Option
`pc_enemy_load_re2`, main.c:718-724): (1) „er fliegt irgendwie komisch in der Luft",
(2) „er ist unvollständig".

Alle Messungen an `build/extracted/re2_ems/CDEMD0_EM36_emd.EMD` (112340 B, md5 3a49b091f8ad,
TOC-authoritativ geschnitten via `re15_port/tools/re2_ems_cut.py`, EXE-TOC @0x8009ADF4),
dem KI-Overlay `CDEMD0_EM36_ai1.BIN` (23476 B, gelinkt @0x80100000, TOC k=1) und der
RE2-Leon-EXE (`ghidra_re2_Leon.txt` + `RE2_Quellcode_V2`).

---

## 1. Das RE2-EM36-EMD — vollständig zerlegt

### 1.1 Directory (dir_off 0x1B6B4, dir_cnt 8)

| dir | Offset  | Inhalt |
|-----|---------|--------|
| 0   | 0x8     | 4 Zero-Bytes (Reserve) |
| 1   | 0x4AB0  | EDD Paar 1 — **11 Clips** (Haupt-Bank des Ports) |
| 2   | 0x6020  | EMR — Struktur **2 Bones**, kf_size 0x18, Keyframes @+0x20 |
| 3   | 0xDFC0  | EDD Paar 2 — 4 B, `count0=0/offset0=0` → **leer** |
| 4   | 0xDFC4  | (Pool Paar 2, leer) |
| 5   | 0xDFC8  | EDD Paar 3 — **1 Clip, 150 Frames** (Victim-Kanal) |
| 6   | 0xE228  | Pool Paar 3 — Header {0,8,**15 Bones**,0x50}: eine **PL00-/Leon-Rig-Animation** (Devour) |
| 7   | 0x11110 | MD1 — length 0x6F0C, **14 Objekte = 7 Meshes** |

Port-Splitter `re2_emd_parse_bank` (re2_ems.c) wählt Paar 1 (meiste Clips) → Log
„RE2 EM036: 7 Meshes, 2 Bones, 11 Clips, Slot 12" stimmt mit dem Parse überein.

### 1.2 EMR @0x6020 — die Antwort auf „Wurzel-Bind-Y ??"

Header `{arm_off=0x14, kf_off=0x20, bones=2, kf_size=0x18}` (Konvention wie
`re15_emd_parse_skeleton`, emd_common.c:144-147). Positionstabelle fest @+8:

```
Bone 0 (Wurzel):  (0, -4534, 0)      @Datei 0x6028  ← Wurzel-Bind-Y = -4534
Bone 1 (Kind):    (2146, 807, 0)     @Datei 0x602E
Armature @+0x14:  Bone0 {1 Kind, off 8} -> [1];  Bone1 {0, off 9}
Keyframes @+0x20 = Datei 0x6040, 24 B/kf, exakt 1360 Stück (Pool endet @0xDFC0 = dir[3])
```

(Vergleich: RE2-EM30 Wurzel-Bind-Y = -1994; **RE1.5-EM36** dir[2]: 16 Bones, kf 84,
Wurzel-Bind (27, **-2029**, 0) — CDEMD0.EMS Blob 17 @EMS+0x2AE000, dir @0x1F634.)

### 1.3 ⛔ Der frühere Befund „Bind-Offsets laufen im 6-B-Takt hinter der Positionstabelle weiter" — HALB wahr

**Der LESE-Mechanismus ist real, die DATEN sind es bei EM36 nicht.**

RE2-Renderer-Setup `FUN_80028368` (Werte roh verifiziert, ghidra_re2_Leon.txt):
* Schleifenzähler = **MESH-Zahl**, nicht Bone-Zahl: `lbu s4,0x107(a0)` **@0x800283ac**;
  entity+0x107 wird in `FUN_80028324` aus dem MD1 gesetzt: `nObj>>1` (RE2_Quellcode_V2/FUN_80028324.c:6).
* Positions-Cursor = EMR+8: `addiu t2,t0,0x8` **@0x80028374**; pro Mesh drei `lh`
  (**@0x80028500/0x80028510/0x80028524**) mit 6-Byte-Schritt (**@0x80028514** `addiu t2,t2,6`,
  **@0x80028540** `addiu s3,s3,0x6`) → **jedes der 7 Meshes konsumiert einen 6-B-Eintrag ab EMR+8**.
* Die Armature wird nur verbraucht, solange Bones übrig sind (`lhu t2,0x4(t0)` @0x800283bc,
  Abbruch **@0x8002853c** `beq t2,zero`); danach bekommt jedes weitere Mesh
  **Parent = Identitätsmatrix `&DAT_8009db44`** und Parent-Work = 0 (**@0x800285dc-e8**).
  Mesh 0 hängt an der Entity-Weltmatrix entity+0x24 (**@0x800283c8/0x800283d0**).

Für EM36 (2 Bones, 7 Meshes) überläuft der 6-B-Takt aber die reale Tabelle — die
„Bind-Offsets" der Meshes 2..6 sind **re-interpretierte Armature-/Keyframe-Bytes**:

| Mesh | gelesener Offset | Datei | Herkunft der Bytes |
|------|------------------|-------|--------------------|
| 0 | (0, -4534, 0)  | 0x6028 | echte Position Bone 0 |
| 1 | (2146, 807, 0) | 0x602E | echte Position Bone 1 |
| 2 | (1, 8, 0)      | 0x6034 | **= Armature-Tabelle** |
| 3 | (9, 1, 0)      | 0x603A | **= Armature-Tabelle** |
| 4 | (0, -4534, 0)  | 0x6040 | **= Keyframe 0** |
| 5 | (0, 0, 0)      | 0x6046 | **= Keyframe 0 (Speed)** |
| 6 | (0, 0, 0)      | 0x604C | **= Keyframe 0 (Winkel)** |

⇒ **Es gibt im EM36-EMR keine sinnvollen Bind-Offsets für die bonelosen Meshes.**
Deren Platzierung/Sichtbarkeit macht in RE2 der G5-KI-Overlay-Code selbst (§3).

### 1.4 MD1: was die 7 Meshes SIND (Vertices/BBox, PSX-Einheiten)

| Mesh | v | tri | quad | BBox | Deutung |
|---|---|---|---|---|---|
| 0 | 398 | 84 | 134 | x −1800..3682, y −802..1687, z −1817..1827 | **Kriech-Körper** (Cutscene-Form, an Bone 0) |
| 1 | 8 | 0 | 1 | 10×10 flach | Effekt-Quad (an Bone 1) |
| 2 | 1270 | 248 | 467 | x −3600..2694, **y −4224..+24**, z ±3648 | **der 4,2-m-Riesenblob** — Sohle bei y≈+24 = steht AUF dem Entity-Ursprung |
| 3 | 56 | 26 | 0 | x 883..2628, y 570..1297 | herausragender Arm/Torso-Fetzen |
| 4-6 | je 8 | 0 | je 1 | 10×10 flach | Effekt-Quads |

**EM36 enthält also ZWEI Gestalten** (Kriecher + Riesenblob) plus Zubehör — RE2 zeigt
sie nie gleichzeitig; das Overlay schaltet/positioniert die Teile. Der Port zeichnet
seit heute ALLE 7 auf der Wurzelpose (main.c:8139-8156, `npc_poses[nbi<bones?nbi:0]`
@main.c:8207) → Kriecher UND Blob UND Quads übereinander, alle am Keyframe-Root
(−4534) = „unvollständig/kaputt" in jeder Variante. (Vor der Änderung klemmte die
Schleife auf bone_count=2 → 5 von 7 Teilen unsichtbar, u.a. der komplette Blob.)

Randnotiz: die vier Fenstertentakel des RE2-Finales sind NICHT in EM36 — RE2 spawnt
in ROOM7040 **vier eigene kind-0x37-Entities** (Sce_em_set-Kette @RDT 0x11AC..0x1204:
id 0 = 0x36, ids 1-4 = 0x37, alle geparkt bei (−32000, 0, −32000)); 0x37 hat eigenes
EMD/TIM/KI (TOC: EM037 59376 B). RE1.5-ROOM5090 spawnt kein 0x37.

---

## 2. Die 11 Clips des RE2-EM36 (EDD @0x4AB0) + Anim-Pfad des Ports

### 2.1 Clip-Katalog (Frames; Keyframe-Root-Translation, direkt aus dem Pool @0x6040)

| Clip | Frames | kf | rootY min..max | RE2-Nutzung (KI-Overlay, §3) |
|---|---|---|---|---|
| 0 | 100 | 0..99 | −4534..−4392 | Idle-Wippen (Wort 0x1F0000 @0x80101618/@0x80101770; 0x70000 @0x80103978) |
| 1 | 180 | 100..279 | −4536..−3191 | **INIT-Grundpose** (@0x8010116c) — großer Y-Hub (Aufbäumen/Rise) |
| 2 | 150 | 280..429 | −4534..−4392 | **Haupt-/Vorrück-Clip** (@0x80101000/@0x801015d0/@0x80101710/@0x8010186c → X-Advance @0x8010194c; 0x70002 @0x80101cb0) |
| 3 | 100 | 430..529 | −3259..−3200 | State[3] @0x8010138c (hohe Lage — aufgerichtet) |
| 4 | 180 | 530..709 | −4534..−3105 | State[5] @0x801014f8 (zweiter großer Hub) |
| 5 | 150 | 710..859 | −4534..−4159 | @0x80100dd8 |
| 6 | 100 | 860..959 | −4517..−4385 | Angriffs-Cluster @0x8010306c/@0x80103430 |
| 7 | 50 | 960..1009 | −4507..−4391 | Angriffs-Cluster @0x801031e4 |
| 8 | 50 | 1010..1059 | −4520..−4391 | Angriffs-Cluster @0x80103444/@0x80102b28 |
| 9 | 150 | 1060..1209 | −4504..−4313 | Sonderfall interp 7 (Wort 0x70009 @0x80101b30) |
| 10 | 150 | 1210..1359 | −4518..−4391 | kein direkter Setzer gefunden |

Kein einziger Frame-Eintrag trägt Flag-Bits (alle >>12 == 0) — keine SE-Marker im EDD.
Die Damage-Fenster liegen im Overlay: `jal 0x800401d4` (player_damage) @0x80100904,
**@0x80103ea0** (Funktion ab 0x8010380c, Tabelleneintrag [21]) und **@0x8010416c**
(Funktion ab 0x80103fe0, Eintrag [24]) — im selben Cluster wie die Clips 6/7/8.

**Alle 1360 Keyframes tragen rootY ∈ [−4536, −3105]** — der Kriecher-Körper hängt in
JEDER Pose 3,1–4,5 m ÜBER dem Entity-Ursprung (PSX-Y zeigt nach unten).

### 2.2 Port-Anim-Pfad: welche Clips die RE1.5-0x36/0x30-KI treibt

Der Endkampf fährt die geteilte Birkin-Wurzel 0x80116230 (STAGE3 registriert 0x30 UND
0x36 auf dieselbe Wurzel, sw @0x8011cf48/@0x8011cf50; enemy_ai_common.c:10990-10992).
`re15_birkin_clip`-Aufrufe (enemy_ai_common.c) mit Clip-Indizes:

* **in range (0..10):** 0 (INIT), 1 (Walk @0x80117220), 3 (Claw @0x801175b0),
  4 (@0x80117…), 5, 7 (@0x8011a2e0), 9 (Hurt @0x8011a634), 0x0a (Recovery @0x801178c0)
* **OUT OF RANGE für die 11-Clip-Bank:** 0x0b (:11158), 0x0c (:11369, :12749),
  0x0d (:11251, :12769), 0x0e (:11276), 0x10 (:11245, :12825), 0x14 (:11182)

Was der Port daraus macht — zwei unabhängige Defekte:
1. **Render:** `re15_compute_actor_kf` wickelt `clip_idx % clip_count`
   (anim_select_common.c:75) → motion 0x14 → RE2-Clip 9, 0x0e → 3, 0x0c → 1 … =
   **willkürliche RE2-Clips** mit teils riesigem Root-Y-Hub (Clip 1/4: 1,4 m) —
   der zweite Anteil von „fliegt komisch in der Luft" (rhythmisches Auf-und-Ab).
2. **KI-Uhr:** `re15_birkin_anim` fällt bei `motion >= clip_count` auf **fc = 40**
   (enemy_ai_common.c:10997-11000) zurück, während der Renderer den gewickelten Clip
   mit 50..180 Frames spielt → Pose springt alle 40 Ticks mitten im Clip zurück.

### 2.3 Der Y-Anker (Hauptursache „fliegt in der Luft")

Port-Wurzelpose = NUR Keyframe-Root-Translation (`poses[0].trans[1] = rt_y`,
skeleton_common.c:705; Bind[0] wird belegt nie addiert — Kommentar ebd. zu
FUN_8001f3bc.c:28-37). Actor-Y steht in ROOM5090 auf dem Boden ⇒ mit rootY ≈ −4534:
* Kriecher (Mesh 0, y −802..+1687 um Bone 0): schwebt **2,8..5,3 m** über dem Boden;
* Blob (Mesh 2, auf Wurzelpose gezeichnet): Sohle bei −4534+24 = **4,5 m** über dem Boden
  — obwohl er im Modellraum FÜR den Entity-Ursprung gebaut ist (Sohle +24).

Der frühere RE1.5-EM36-Stand schwebte nicht, weil dessen Keyframes rootY ≈ −790..−985
tragen (Becken über Fuß, Füße am Ursprung — Bank 0, 5 Clips {30,63,69,23,10}).

---

## 3. Referenz: wie RE2s eigener G5-Handler posiert (KI-Overlay EM36, ROOM7040)

Das Overlay (23476 B, TOC k=1 → gelinkt @0x80100000; Header: Wort 0x1A + 16 Byte
Permutationstabelle + Funktionstabelle ab +0x14 mit Einträgen 0x80101164, 0x80101214,
0x801012ec, 0x8010136c, …) ist der Beweis, dass **die Inszenierung Handler-Sache** ist:

* **INIT (Tabelleneintrag [0] @0x80101164):** Clip-Wort **0x1F0001** → +0x14C
  (@0x8010116c — Clip 1 = Grundpose), löscht Bit 1 in +0x1C0, **nullt vier Halbwörter
  in einer Part-Substruktur** (`lw v1,0x198(s0)`, dann `lw v0,480(v1)` / `sh zero,12/16/20/24(v0)`
  @0x80101180-ac), und setzt bei (+0x1D4 & 1) **hart X=−9000 / Z=−23400**
  (@0x801011d0-dc, `sw …,0x38/0x40(s0)` — RE2-Entity-Pos X/Y/Z = +0x38/+0x3C/+0x40,
  Yaw +0x76, vgl. FUN_80015558-Beleg in enemy_ai_re2_zombie.c:293-296).
  Die RDT-Spawns liefern KEINEN Anker (alle 5 Entities geparkt bei (−32000,0,−32000)).
* **Y wird vom Handler GERAMPT, nie vom Keyframe verortet:** die einzige Entity-Y-Schreibstelle
  im Overlay ist **@0x80103808** `sw v1,0x3C(s3)` mit
  `y = (t · 2950) >> 12` und Gegenstück `sw v0,0x30(s2)` @0x80103804 =
  **work.bind_y = 4500 − y** (@0x801037f8 `addiu v1,zero,4500`).
  work+0x2C/+0x30/+0x34 ist exakt das Bind-Offset-Tripel, das FUN_80028368 aus dem EMR
  vorbefüllt (`sw v1,-0x68(s1)`/`sh v1,-0x5c(s1)` @0x80028508/0c, s1 = work+0x94) —
  **der Handler ÜBERSCHREIBT die Part-Bind-Offsets zur Laufzeit** (Auftauch-Rampe über
  4500 Einheiten, aufgeteilt zwischen Entity-Y und Part-Offset).
* **Part-Array-Steuerung ist massiv:** 44 `lw …,0x198(…)`-Stellen im Overlay
  (0x801002e8 … 0x80105500) — Sichtbarkeit und Lage der bonelosen Meshes (Blob, Arm,
  Quads) sind reine Handler-Arbeit, genau wie beim RE1.5-Typ-0x26-Feuer-Emitter
  (Part-Bit-0-Maske).
* **Clip-Repertoire des Handlers:** alle Setzer schreiben `0x1F0000|{0..9}` bzw.
  `0x70000|{0,2,9}` (vollständige Liste in §2.1) — **RE2 adressiert nie einen Clip > 9**;
  die RE1.5-Brain-Indizes 0x0b..0x14 haben im RE2-Clipsatz schlicht keine Entsprechung.

---

## 4. Wurzelursachen — Kurzfassung

**(2) „fliegt in der Luft":**
* **U1:** Alle RE2-EM36-Keyframes tragen rootY ≈ −4534 (Modellraum-Anker des Kriechers,
  §1.2/§2.1); der Port setzt die Wurzel = Keyframe-Translation über dem Boden-Actor-Y
  (skeleton_common.c:705) → ganzes Modell 3–4,5 m hoch. RE2 verortet Y ausschließlich
  per Handler (§3), nie über diesen Keyframe-Anker.
* **U2:** Clip-Wickeln mod 11 (anim_select_common.c:75) + KI-Uhr-Fallback fc=40
  (enemy_ai_common.c:10997-11000) für die sechs OOB-Indizes → falsche Clips mit 1,4-m-Hub,
  Pose-Sprünge = „komisch".

**(1) „unvollständig":**
* **U3:** EM36 = Kriecher (Mesh 0/1, animiert) + Riesenblob (Mesh 2, bonelos, Sohle am
  Ursprung) + Arm/Quads. Bone-Klammer zeigte nur Mesh 0+1 (Blob = der eigentliche Boss
  fehlte); der heutige Alle-auf-Wurzelpose-Fix stapelt beide Gestalten am −4534-Anker.
  Die EMR-„Bind-Offsets" der Meshes 2..6 sind Garbage-Überläufe (§1.3); im Original
  platziert der Handler die Teile über work+0x2C..0x34 und schaltet sie (§3).

---

## 5. Plan für den Port (jeder Schritt mit Beleg)

Der Endkampf ist eine PORT-OPTION (RE2-Modell unter RE1.5-Brain) — es gibt kein
byte-true „Original" dieser Kombination. Referenzrahmen = RE2s G5-Handler (§3).

1. **Zeichenschleife (main.c:8139-8215): boneloser Part = ENTITY-Ursprung, nicht Wurzelpose.**
   Für `nbi >= bone_count` statt `npc_poses[0]` eine Identitäts-Pose mit trans (0,0,0)
   verwenden (Weltlage = nyaw · 0 + npc->x/y/z). Beleg: RE2 hängt diese Parts an
   `&DAT_8009db44` (Identität, @0x800285dc-e8) — NICHT an Bone 0; der Keyframe-Anker
   −4534 gehört nur dem Kriecher. Effekt: Blob-Sohle (+24) steht auf dem Boden.
   (Ob die Entity-Yaw mitgedreht wird, ist eine dokumentierte Abweichung: RE2 dreht
   die Identitäts-Parts nicht mit; für einen Boss, der sich zum Spieler dreht, ist
   Mitdrehen die sinnvolle Port-Wahl.)
2. **Part-Maske für 0x36 setzen (Entscheidung: welche Gestalt kämpft).**
   Mechanik existiert schon als per-Part-Skip (gore_draw-Pfad, main.c:8203-8206 bzw.
   `npc->no_draw`-Vorbild des 0x26-Emitters @0x801165d0-e4). Empfehlung:
   **Kriecher (Mesh 0+1) als animierten Kampfkörper zeigen, Blob (2), Arm (3) und
   Effekt-Quads (4-6) verbergen** — die 11 Clips animieren ausschließlich Bone 0/1;
   der Blob ist starre Kulisse (7,3 m breit — gebaut für den Zug-Korridor, nicht für
   die ROOM5090-Arena). Alternative (Blob als Boss) wäre unanimiert und sprengt die
   Arena — Nutzer-Entscheid, Zahlen in §1.4.
3. **Y-Anker für den Kriecher: per-Keyframe-`root_y_fix`.** Infrastruktur existiert
   (re15_emd.h:113, angewandt @skeleton_common.c:256/264; Pool-Bauer
   `re2_build_rootfix` in re2_ems.c). Beim EM36-Laden eine Tabelle bauen, die pro
   Keyframe den tiefsten Kriecher-Punkt auf 0 hebt (kf 0: −4534+1687 = −2847 →
   fix ≈ +2847; NICHT pauschal +4534, sonst stecken die Beine 1,7 m im Boden).
   Messgrundlage: rootY-Bänder §2.1 + Mesh-0-BBox §1.4.
4. **Clip-Mapping-Tabelle RE1.5-Index → RE2-EM36-Index** statt mod-11-Wickeln, an
   EINER Stelle konsumiert von Renderer UND KI-Uhr (re15_birkin_anim MUSS die Länge
   des GEMAPPTEN Clips takten — sonst bleibt der 40-Frame-Sprung, §2.2).
   Startvorschlag entlang der RE2-Semantik (§2.1):
   `{0→0, 1→2(Advance), 3→6, 4→7, 5→8, 7→9, 9→9, 0xa→0, 0xb→1, 0xc→9, 0xd→0,
   0xe→7, 0x10→0, 0x14→1(Rise)}` — jede Zielnummer existiert (0..9), Feintuning
   visuell per re15-port-visual-verify.
5. **Optional (Vollausbau):** eigener Mini-Treiber nach dem Overlay-FSM —
   INIT Clip 1 (@0x8010116c), Advance Clip 2 + X-Vortrieb (@0x8010186c/@0x8010194c),
   Beiß-Fenster an den player_damage-Stellen (@0x80103ea0/@0x8010416c),
   Auftauch-Rampe 4500 (@0x801037f8-808). Erst sinnvoll, wenn Schritt 1-4 stehen.
6. **Nicht tun:** die Garbage-Offsets aus §1.3 als „Bind" portieren (Armature-Bytes
   (1,8,0)/(9,1,0) sind keine Geometrie), und keine weiteren Konstanten ohne
   Overlay-Beleg — die Part-Feinlage (Arm am Blob etc.) ist Handler-RE, nicht EMR-RE.

## 6. Offen / nicht kritisch

* `sh v0,0x14C(v1)` mit 212 @0x8010045c — Basis v1 ist nicht die Entity (vermutlich
  Substruktur-Init); ohne Einfluss auf die vier Wurzelursachen.
* Vollständige Zuordnung der 27 Tabelleneinträge des Overlays zu FSM-Zuständen
  (nur INIT/Advance/Angriffs-Cluster/Y-Rampe sind hier kartiert).
* Clip 10 (150 F) hat keinen direkten Setzer im k=1-Overlay — evtl. nur vom
  k=0-Twin (@0x8010D000) oder EXE-seitig adressiert.
