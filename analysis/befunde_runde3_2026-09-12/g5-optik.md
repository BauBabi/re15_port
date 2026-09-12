# G5-Optik: Wie RE2s room7040-Endkampf WIRKLICH komponiert ist (2026-09-12)

Nutzer 2026-09-12: „Auch der Endboss sieht immer noch nicht so aus wie im originalen
Resident Evil 2." — Der Port zeigt vom RE2-EM36 nur die Meshes 0/1 („Kriecher") und
VERBIRGT den 4,2-m-Blob (Mesh 2), den Arm (3) und die Effekt-Quads (4-6)
(`eb->md1.mesh_count = 2`, re15_port/platform/pc/main.c:746 — Empfehlung des
birkin-em36-Dossiers §5.2). Der Nutzer vergleicht mit RE2s Zugkampf: dort sieht man
die RIESIGE Fleischmasse. **Befund: der Nutzer hat recht — RE2 versteckt im Kampf
KEIN einziges Part. Alle 7 Teile sind gleichzeitig sichtbar; der Blob ist als Kind
des Kriecher-Wurzelbones aufgehängt und bildet mit ihm EINE zusammenhängende Masse.**

Quellen (nur Code/Daten, kein Screenshot-Wissen):
* `build/extracted/re2_ems/CDEMD0_EM36_ai1.BIN` (23476 B) — der EM36-KI-Handler,
  gelinkt @0x80100000 (alle 0x8010xxxx-Adressen unten). `..._ai0.BIN` ist **derselbe
  Code @0x8010D000 gelinkt** (Funktionstabellen-Diff konstant −0xD000, 316
  Relokations-Bytes — der „0x8010D000-Twin"; RE2 lädt zwei Gegner-Overlays parallel,
  room7040 braucht EM36+EM37). Header: Wort0=0x1A, 16-B-Permutationstabelle,
  Funktionstabelle @+0x14 mit 27 Einträgen [0..26] (`[15]`=0), [0]=0x80101164 …
  [26]=0x80104228.
* RE2-Leon-EXE-Decompiles `RE2_Quellcode_V2/FUN_80028324.c`, `FUN_80028368.c`,
  `FUN_8004b3b8.c`, `FUN_8004baf0.c`, `FUN_800152c8.c`, `FUN_800197f4.c`.
* `build/extracted/re2_ems/CDEMD0_EM36_emd.EMD` (112340 B), `info/re2leon/PL0/RDT/
  ROOM7040.RDT` / `ROOMF040.RDT`, `re15_port/shared_assets/PSX/STAGE5/ROOM5090.RDT`.
* Disasm-Werkzeug: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis <addr>
  --bin <ai1.BIN>` (load() akzeptiert absolute Pfade; Overlay-Mapping addr−0x80100000).
* Vorarbeit: `analysis/schrot_befunde_2026-09-12/birkin-em36.md` (EMD-Zerlegung §1,
  Clips §2, erste Handler-Kartierung §3).
* `RE2_Quellcode_Overlays/STAGE7/` enthält NUR `FUN_80100000.c` (12 Zeilen) — kein
  Decompile des EM36-Overlays; alles Folgende ist Direkt-Disasm aus den Bytes.

---

## 1. Die Part-Maschine des RE2-Renderers (EXE)

`FUN_80028324` (RE2_Quellcode_V2/FUN_80028324.c): `entity+0x107 = nObj>>1`
(Mesh-Zahl aus MD1), `entity+0x198 = part_array`; **ein Part-Record ist 0xAC Bytes**
(Rückgabe `param_2 + mesh_count*0xAC`).

`FUN_80028368` (Init aller Records; Feld-Layout aus den puVar8-Offsets, puVar8 =
rec+0x94):

| Record-Offset | Feld | Init |
|---|---|---|
| +0x00 | **Status-/Flagwort** | **1** für JEDES Part (`*puVar10 = 1`) |
| +0x08 / +0x10 | MD1-Objekt-Ptr (tri/quad) | dir[7]-Objekte, Schritt 0x38 |
| +0x18..0x2A | **LOKALE Matrix** (MATRIX, rot 9×s16) | Identität 0x1000 |
| +0x2C/+0x30/+0x34 | **Bind-Translation (Words)** = Trans der lokalen Matrix | EMR+8-Tabelle im 6-B-Takt |
| +0x38/+0x3A/+0x3C | Bind-Kopie (Shorts) | dito |
| +0x48..0x5A | **WELT-Matrix** (rot) | Identität |
| +0x5C/+0x60/+0x64 | Welt-Translation | 0 |
| +0x70 | **Farbwort (RGB)** | 0x808080 |
| +0x74 | Parent-WELT-Matrix-Ptr | Armature-Kind: Parent-rec+0x48; sonst Identität `&DAT_8009db44`; Part 0: entity+0x24 |
| +0x78 | Part-Index | 0..6 |
| +0x88 | **Morph-Controller-Ptr** | via FUN_8004b3b8 (§3.4) |
| +0x8C/+0x8E/+0x90 | Skalenvektor (s16, 4096=1.0) | — |
| +0x94 | Parent-RECORD-Ptr | Armature; Part 0: entity |
| +0x98..0xA2 | 6 Halbwörter lokale Winkel/Params | 0 |

Für EM36 (2 Bones, 7 Meshes): Parts 0/1 nach Armature (1 Kind von 0), Parts 2..6
kriegen von der EXE Identitäts-Parent + Garbage-Binds (birkin-em36 §1.3) — **und der
Handler verdrahtet sie dann selbst um (§2)**.

## 2. ⛔ DER KERNBEFUND: der Handler baut die Masse als Kind des Kriechers

Konstruktor/Setup = EINE Funktion @0x801003cc..0x80100780 (Prolog `addiu sp,-80`
@0x801003cc, Epilog @0x80100770). Der Part-Kompositionsblock @0x80100670-0x80100740:

| Part | sichtbar (`+0x00=1`) | Parent-Matrix `+0x74` | Parent-Rec `+0x94` | Bind `+0x2C/+0x30/+0x34` |
|---|---|---|---|---|
| 2 **Blob** | @0x8010068c | part0+0x48 (@0x80100678/0x8010067c) | part0 (@0x80100690) | **(1800, 4500, 0)** @0x80100680/0x80100688/0x80100698 |
| 3 **Arm** | @0x801006b8 | part0+0x48 (@0x801006a4/0x801006a8) | part0 (@0x801006bc) | **(800, 500, 0)** @0x801006ac/0x801006b4/0x801006c4 |
| 4 Quad | @0x801006e0 | **part2**+0x48 (@0x801006d0/0x801006e4) | part2 (@0x801006d4/0x801006e8) | **(1698, −1398, 0)** @0x801006cc/0x801006dc/0x801006f0 |
| 5 Quad | @0x8010070c | part2+0x48 (@0x80100710) | part2 (@0x80100714) | **(1432, −2691, 1023)** @0x801006f8/0x80100700/0x80100708-18 |
| 6 Quad | @0x80100724 | part2+0x48 (@0x80100728) | part2 (@0x8010072c) | **(1430, −2284, −2078)** @0x80100720/0x80100734/0x8010073c |

Dazu: part0-Flag |= 0x4000 (@0x80100628), Farbwort part2+0x70 = 0 initial
(@0x801003f4, `sh zero,0x1C8(s0)` — Basis hier die ENTITY? nein: s0=entity, 0x1C8 =
entity-Feld; Farb-Init des Blobs bleibt 0x808080 aus FUN_80028368), Morph-Anwendung
`jal 0x8004baf0(entity, &part2)` @0x80100760, Haupt-Routine=2 (`sb 2,5(s0)`
@0x8010076c). Pro-Frame-Callback: `entity+0x208 = 0x801003ac` (@0x80100664-6c) →
ruft 0x80103af4.

**Die kinematische Kette (Pro-Frame-Callback 0x80103af4):**
```
0x80103b18  jal 0x8008e1f4(entity+0x74, entity+0x24)    ; Entity-Weltmatrix aus Winkeln
0x80103b2c  jal 0x8002ce94(entity+0x24, part0+0x18, part0+0x48)  ; part0_w = entity × lokal0
0x80103b3c  jal 0x8002ce94(part0+0x48, part1+0x18, part1+0x48)   ; part1_w = part0 × lokal1
0x80103b4c  jal 0x8002ce94(part0+0x48, part2+0x18, part2+0x48)   ; BLOB_w  = part0 × lokal2
0x80103b5c  jal 0x8002ce94(part0+0x48, part3+0x18, part3+0x48)   ; ARM_w   = part0 × lokal3
0x80103b70  jal 0x800197f4(entity, parts, 0x801059d8)   ; Vertex-Naht Bone1↔Bone0 (186 V.)
0x80103b84  jal 0x800197f4(entity, parts, 0x80105a44)   ; Vertex-Naht Bone1↔Arm  (13 V.)
0x80103b90  jal 0x80019cd0(entity, parts)
```
⇒ **Der Blob hängt am Wurzelbone des Kriechers** (Offset 1800 in Bone-0-X, 4500 nach
unten) und **schwingt/hebt sich mit jeder Wurzel-Animation mit** — Kriecher und Masse
sind EIN verbundener Körper. Parts 4/5/6 hängen am Blob.

Die Naht-Tabellen (statisch im Overlay): @0x801059d8 = {part_a=1, part_b=0,
count=186, SVECTOR (−2146,−807) = −Bone1-Bind}, @0x80105a44 = {1, 3, 13,
(−2146,−807) und (−800,−500) = −Arm-Bind} — `FUN_800197f4` transformiert die
Vertex-Läufe mit BEIDEN Welt-Matrizen und blendet per Gewichtstabelle (GTE
rtv0/Interp): **2-Bone-Skinning der Übergänge** Kopf↔Rumpf und Kopf↔Arm.
(Init der Läufe: `jal 0x80019628(entity, parts, tab)` @0x80103ac0/0x80103ad8.)

### 2.1 Sichtbarkeit: ALLE 7 Parts, den ganzen Kampf

Alle 44 `lw …,0x198(…)`-Stellen (0x801002e8 … 0x80105500) wurden kartiert. Es gibt
**keinen einzigen Schreiber, der ein Part-Statuswort auf 0 setzt**. Die einzigen
Flag-Operationen: Setup setzt Parts 2..6 = 1 (Tabelle oben), part0 |= 0x4000
(@0x80100628), und ein Spezial-State setzt part2 |= 0x400 + Skalenvektor
part2+0x8C/8E/90 = 4096 (@0x801034b0-c0). Scheinbare Flag-Writes wie
`sw …,0x0(s2)`/`sh …,0x158(s0)` in anderen Funktionen adressieren die ENTITY
(+0x00-Flags, +0x158-Zähler) — Basisregister-Provenienz geprüft.

## 3. Was der Handler im Kampf mit den Parts TUT

### 3.1 INIT (Tabelleneintrag [0] @0x80101164)

Clip-Wort 0x1F0001 → entity+0x14C (@0x8010116c), Morph-Gewichte [0..3] = 0
(`lw v0,0x1E0(v1)` = *(part2+0x88); `sh zero,12/16/20/24(v0)` @0x80101180-ac),
und **`lw v0,0x30(a3)` @0x801011f4 → Overlay-Globale 0x80105bb0** = Original-Bind-Y
von Part 0 (−4534) als Baseline gesichert. Bei (entity+0x1D4 & 1): Park-Position
X=−9000/Z=−23400 (@0x801011d0-dc — die ROOMF040-Variante), sub=1, +0x218=3/+0x219=0.

### 3.2 Y-Anker: die Setz-Rampe (Tabelleneintrag [20] @0x80103668)

Pro Tick (Rampenteil ab @0x80103730):
```
t   = *(morph_ctrl+0x14)                      ; Gewicht von Morph-Segment 2
t  += 16 (t<1025) | 12 (<2049) | 8 (<3073) | 4  @0x8010373c-78
t >= 4097 -> entity+0x06 = 5                   @0x801037a4-b0
y   = (t * 2950) >> 12                         @0x801037c4-f4 (Shift-Add-Kette)
part2+0x30 = 4500 - y                          @0x801037f8/0x80103804 (s2 = parts+0x158 @0x80103750)
entity+0x3C = y                                @0x80103808
```
⇒ Bei t=0: Entity am Boden, Kriecher-Wurzel −4534 (4,5 m hoch), Blob-Bind 4500 →
Blob-Ursprung Welt-Y −34 (Sohle +24 ⇒ −10 ≈ Boden). Bei t=4096: Entity-Y = +2950,
Bind = 1550 → **Blob-Welt-Y konstant −34; der Kriecher sinkt von 4,5 m auf 1,6 m
über dem Boden.** Der Blob bleibt während der GANZEN Rampe exakt am Boden verankert;
Dauer ≈ 64+86+128+256 ≈ 534 Ticks. Parallel (jeden 2. Tick, @0x80103668-72c) faden
die Farbworte part0+0x70 UND part2+0x70 kanalweise von 0x808080 auf **0x403030**
(Ziele 0x30/0x30/0x40 @0x80103680/0x801036a8/0x801036c4).

### 3.3 Bewegt sich der Blob? JA — mit der Entity (Vortrieb)

Advance-State @0x80101810: Clip-Wort 0x1F0002 (Clip 2) @0x8010186c, Zähler
entity+0x158 rampt 0→50 (Sound 11 @0x8010187c) und zurück 50→0 (Sound 10
@0x80101920), Zähler → Speed-Vektor entity+0x144 (@0x80101930), und
**`if (entity_x > 4000) entity_x -= zaehler`** (@0x80101934-4c, `sw v0,0x38(s2)`
@0x8010194c) — die gesamte Entity (Blob+Kriecher+Anhang) kriecht ruckweise
(max. 50 Einheiten/Frame) den Korridor entlang auf X=4000 zu. Treffer-Check
`jal 0x8002959c(…,128)` @0x80101958.

### 3.4 Der Blob PULSIERT: Vertex-Morph (EMD dir[0] ist KEINE Reserve)

`FUN_8004b3b8(entity, dir0_blob)`: Wort 0 = **Mesh-Bitmaske**, dann
Controller-Records; `part[i]+0x88 = Record` für jedes gesetzte Bit.
EM36-EMD @0x8: Maske **0x4 → nur Mesh 2 (Blob)**; Controller @EMD+0x14 =
{data_off=0x34, 0xEE4, seg_count=4}, Gewichte u16 @ctrl+0xC/+0x10/+0x14/+0x18
(0..0xFFF); Morph-Zieldaten EMD+0x34..0x4AB0 (~19 KB — das ist der „leere" dir[0]).
`FUN_8004baf0(entity, &part2)` blendet die Blob-Vertices per GTE
(gte_gpf12/gpl12) segmentweise zwischen Basis und Ziel. Treiber im Overlay:
* Rampe inkrementiert Gewicht[2] (§3.2) — die Masse verformt sich beim Setzen.
* **Gewicht[1] = (part0_bind_y − Baseline) × 4415/2048** (@0x8010132c-68,
  Multiplikationskette `<<4+…<<2+…<<6−…`, `sh a0,16(v1)` @0x80101368; Baseline aus
  0x80105bb0, §3.1) — part0+0x30 trägt zur Laufzeit die ANIMIERTE
  Keyframe-Root-Translation (sonst wäre diese Differenz konstant 0 und der Code
  sinnlos), **d.h. die Masse bläht sich proportional zur Hebung des Kriechers**.
* Gewicht[0] wird als Gate gelesen (`lh v0,12(v0)` < 6000 @0x80101d04-14).

### 3.5 Was tun die Effekt-Quads? Sie sind EMITTER-ANKER (und Naht-Hilfen)

Der Partikel-Spawner `FUN_8001bf10(id, yaw, part_matrix, params)` wird mit den
WELT-Matrizen der Parts gefüttert:
* alle 8 Frames ((entity+0x14D & 7)==0): Parts **0** (+0x48 @0x80105434),
  **4** (+0x2F8 @0x80105470), **5** (+0x3A4 @0x801054ac); bei Maske==4 zusätzlich
  **6** (+0x450 @0x801054fc) mit vergrößerten Params (+1200/+400 @0x8010550c-1c).
* Angriffs-/Hurt-Cluster spawnen an **Part 1** (+0xF4, `addiu s2,s2,244`
  @0x80102508) und an Zufalls-Offsets (0x801022f4: 4241/2356; 0x801042e8: 1200;
  0x80104a14/0x80104d44: 256/128-Bänder).
* Part 1 (Bone-1-Quad) bekommt im Konstruktor die Parameter +0x9C/+0x9E = 8,
  +0xA0/+0xA2 = 212, +0x98/+0x9A = 0 (@0x80100450-6c) — und ist Endpunkt der
  beiden Vertex-Nähte (§2). Die 10×10-Quads selbst sind visuell vernachlässigbar
  (10 Einheiten = 1 cm), sie dienen als Koordinatenrahmen.
* Angriffs-Ziellogik misst die Spielerdistanz zur **Blob-Weltposition**
  (part2+0x5C/+0x60, @0x80101db8-dc/0x80102008-38/0x80102234-64), nicht zur Entity.

### 3.6 Intro-Orchestrierung (Tabelleneinträge [1]-[7])

[1] @0x80101214: Tick 10/30/40 weckt die vier Typ-0x37-Tentakel-Entities
(Globals @s3+15416/15420/15424/15428; sub=1, deren part0+0x8C=0), Tick 90 → alle
sub=6. [3] @0x8010136c: Clip 3. [4] @0x80101390: Tick 30 Tentakel sub=8, Tick 90 →
sub 5. [5] @0x8010141c: Tick 20/40/45/56 Tentakel-Routine 2305, Tick 110: Clip 4,
sub=6, Tentakel sub=7. Die Damage-Fenster liegen in [21] @0x8010380c (player_damage
@0x80103ea0) und [24] @0x80103fe0 (@0x8010416c) (birkin-em36 §2.1). Konstruktor:
HP = 600 (easy 400, Flag 0x20 @0x800cfb74) @0x801003fc-418; Hitbox-Halbwerte
6000/6000, Y −2000/−1500, 2200 @0x80100534-58.

## 4. Antwort auf Frage 2: Liegt der „Kriecher" IM Blob?

**JA — er ist der aus der Masse wachsende Vorderleib.** Nach der Setz-Rampe
(Kampf-Normalzustand), Weltkoordinaten bei Entity-Yaw 0 (PSX-Y nach unten):

| Teil | Welt-Y | Ausdehnung |
|---|---|---|
| Blob (Mesh 2, lokal y −4224..+24) | Ursprung −34 | Sohle −10 ≈ Boden, **Kuppe 4,26 m hoch** |
| Kriecher-Wurzel (Bone 0) | −1584 | Körper (lokal −802..+1687) = 2,39 m hoch bis 0,10 m UNTER Boden |
| Arm (Mesh 3, Bind 800,500,0) | −1084 | ragt bei ~1 m Höhe seitlich aus der Masse |

Der Kriecher (x lokal −1800..3682) steckt in der vorderen Flanke des Blobs
(x −1800..4494 im selben Bone-0-Rahmen) — Kopf/Maul auf 0..2,4 m Spielerhöhe,
die Fleischkuppe türmt sich dahinter 4,26 m auf. Attack-Clips (Root −3105..−3200)
stoßen das Maul bis auf Bodenhöhe herab, und weil der Blob am selben Bone hängt,
**wogt die gesamte Masse mit jedem Stoß mit** (plus Morph-Blähung §3.4). Vor der
Rampe (t=0, Kampfbeginn) thront der Kriecher 4,5 m hoch auf der Masse.

## 5. Maßstab/Arena: ROOM5090 IST der room7040-Korridor

RE2 room7040 (SCA v2.0 @RDT+0xF80, Offset-Slot 6): begehbarer Korridorstreifen
z −26884..−19870 = **7014 Einheiten quer**, >45 m lang in X (Wände: z=−29750
d=2866 [3], z=−19870 d=3006 [1]).

RE1.5 ROOM5090 (SCA @0x860, 32 Zellen ×5 Quadranten): DREI parallele Streifen,
und **Streifen C (z −29765..−16864) ist zellengenau der room7040-Korridor**:

| ROOM5090-Zelle | room7040-Zelle | Werte |
|---|---|---|
| [23] x=−5673 z=−19870 w=33730 **d=3006** | [1] x=−30673 **z=−19870** w=57000 **d=3006** | Südwand identisch (x-verschoben) |
| [22] x=−5673 **z=−29750 d=2866** | [3] x=−30673 **z=−29750 d=2866** | Nordwand identisch |
| [21] **x=−5683 z=−21990 w=5180** d=5166 | [2] **x=−5683 z=−21990 w=5180** d=4916 | fast Byte-gleich |
| [25] x=−5683 z=−29740 w=5220 | [4] x=−5683 z=−29990 w=5220 | dito |

Begehbare Quertiefe ebenfalls **7014**; der Blob (z ±3648 = 7296) füllt den
Querschnitt wandfüllend mit ~141 Einheiten Überlapp je Seite — GENAU wie in RE2.
Der RE1.5-Spawn `44 01 30 33` (ROOM5090 sub00 @0x0124A; Umtypung 0x30→0x36 in
scd_vm.c:3313-3325) steht auf (−14700, 0, **−23350**) = Mittellinie von Streifen C
(−23377). Längsachse = X wie in room7040 (Vortrieb §3.3). **Keine Skalierung nötig;
die RE2-Komposition ist für exakt diesen Korridorquerschnitt gebaut.**
(room7040 ceil_y = −5166 > Blob-Kuppe 4258 — Höhe passt ebenfalls.)

## 6. PLAN für den Port (RE2-Komposition nachbauen)

Port-IST: `pc_enemy_load_ex` main.c:719-771 (Klammer mesh_count=2 @746,
per-Keyframe-`root_y_fix` „tiefster Punkt auf Boden" @747-760, BKMAP @761-771);
Zeichenschleife main.c:8139-8215 (`npc_zeichen_n`, bonelose Meshes auf
`npc_poses[0]`).

1. **Klammer `eb->md1.mesh_count = 2` ENTFERNEN** (main.c:746) — RE2 zeigt alle 7
   Parts (§2.1).
2. **root_y_fix ersetzen: uniform +2950 statt per-Keyframe-Bodenhebung.**
   Beleg: Rampenendwert entity_y = (4096·2950)>>12 = 2950 (@0x801037c4-f4/0x80103808);
   der Port hält den Actor am Boden und verlegt die RE2-Entity-Absenkung in die
   Wurzel. Kriecher-Wurzel dann −1584 = RE2-Kampfhöhe (§4). Die heutige Tabelle
   (tiefster Punkt auf 0) widerspricht RE2 — dort ragt der Körper bis 0,10 m
   UNTER den Boden in die Masse.
3. **Kompositions-Tabelle für die bonelosen Meshes** (statt `npc_poses[0]` pur):
   `pose(mesh) = pose(bone0) ⊕ R_bone0·offset` mit den Setup-Offsets (§2):
   * Mesh 2: (1800, **1550**, 0) — 1550 = 4500 − 2950, der Rampen-Endbind
     (@0x80100688 minus Schritt 2); Blob-Sohle landet bei Welt −10.
   * Mesh 3: (800, 500, 0) (@0x801006ac-c4).
   * Mesh 4/5/6 (Kinder des Blobs; dessen lokale Rotation ist Identität, Offsets
     addieren sich): (1800+1698, 1550−1398, 0) = (3498, 152, 0);
     (3232, −1141, 1023); (3230, −734, −2078).
   Implementation: Bank-Feld (z.B. `part_anchor[7] = {bone, off[3]}`) am
   EM36-Ladepfad setzen; die Zeichenschleife (main.c:8203ff) nutzt für
   `nbi >= bone_count` die Anker-Pose. Die Quads sind 10×10 Einheiten (1 cm) —
   mitzeichnen ist RE2-treu und visuell neutral.
4. **Tint 0x403030 auf Meshes 0 und 2** (Rampen-Fade-Ziel @0x80103680-72c) —
   statisch setzen oder als Fade; Modulationspfad existiert (RE2_GORE_TINT,
   main.c:8226ff, `prim*tint/0x80`). Ohne diesen Schritt wirkt die Masse zu hell.
5. **BKMAP + KI-Uhr: unverändert lassen** (Bank-Umsortierung main.c:761-771 speist
   schon Renderer UND `re15_birkin_anim` gemeinsam).
6. **Optionaler Vollausbau** (nach Sichtprüfung, jeweils mit Beleg oben):
   Setz-Rampe als Einlauf-Effekt (534 Ticks, §3.2), Blob-Vertex-Morph
   (dir[0]-Controller, 4 Segmente, §3.4 — Gewicht[1] an die Root-Hebung koppeln),
   Partikel-Anker an Parts 0/4/5/6 alle 8 Frames (§3.5), Vortriebs-Rampe
   0→50→0 mit Sounds 11/10 (§3.3), 2-Bone-Naht (§2 — im Port entbehrlich,
   die Nahtstellen liegen im Blob-Inneren).
7. **Nicht tun:** Parts versteckt lassen (widerspricht §2.1); die
   Garbage-EMR-Binds der Meshes 2..6 verwenden (birkin-em36 §1.3); Blob an die
   Actor-Identität statt an Bone 0 hängen (er muss mitwogen, §2).

## 7. Offen / nicht kritisch

* Semantik der Part-Flag-Bits 0x4000 (part0, @0x80100628) und 0x400 (part2,
  @0x801034c0, mit Skala 4096=1.0) — Skala bleibt 1.0, visuell neutral.
* Part-1-Params 8/8/212/212 (+0x9C..0xA2, @0x80100450-60) — vermutlich
  Sprite-/Naht-Parameter; für den Port ohne Folgen (Quad 1 cm groß).
* Der EXE-seitige Pro-Frame-Schreiber, der part0+0x2C/0x30/0x34 mit der
  Keyframe-Root-Translation füllt, wurde nicht einzeln disassembliert; belegt ist
  der Mechanismus indirekt über die Gewicht[1]-Formel (§3.4), die ohne animiertes
  part0+0x30 konstant 0 wäre.
* Zuordnung der Tabelleneinträge [8]-[19]/[22..26] zu FSM-Zuständen nur teilweise
  kartiert (INIT/Cue/Rise/Settle/Advance/Angriffs-Cluster reichen für den Plan).
