# AUFGABE D — Grundwahrheit ROOM1140 (Briefing Room)

Datum: 2026-09-03 · Datei: `re15_port/shared_assets/PSX/STAGE1/ROOM1140.RDT`
(147176 B = 0x23EE8; byte-identisch mit `info/Re1.5/PSX/STAGE1/ROOM1140.RDT`, Auslieferungsstand)

**KURZANTWORT (Frage 4): NEIN.** Die Original-Engine kann in ROOM1140 nichts vor eine Figur
zeichnen. Alle 10 Kamera-Cuts tragen eine NULL-Maskensektion (`FF FF FF FF`), `hdr[7] = 0`,
kein SLD-Atlas im BSS, kein einziges Sprite-/Effekt-Opcode im SCD, und das einzige Raum-Modell
(`nOmodel = 1`) ist **das Rolltor am Flurausgang**, nicht der Tisch. Das ist eine
**Autoren-Luecke des Prototyps** (50 von 78 STAGE1-RDTs haben null Masken) — der Port darf sie
**nicht** erfinden. Einen dritten Weg gibt es nicht (Beleg §5).

---

## 1. RDT vollstaendig zerlegt

### 1.1 Header Byte[0..7] — Datei-Offset 0x00

```
00000000: 00 0a 01 00 00 00 00 00
```

| Byte | Feld | Wert |
|---|---|---|
| [0] | nSprite (zur Laufzeit: **AKTIVE Maskenzahl**, s. §3) | 0 |
| [1] | nCut | **10** |
| [2] | nOmodel | **1** |
| [3] | nItem | 0 |
| [4] | nDoor | 0 |
| [5] | nRoom_at | 0 |
| [6] | reverb_lv | 0 |
| [7] | **Masken-Kapazitaet pro Raum** | **0** |

### 1.2 Verzeichnis 0x08..0x5C (22 u32, RDT-relativ)

Die Semantik ist nicht geraten: `FUN_800396fc` relokiert genau dieses Fenster beim Raumladen

```
piVar1 = (int *)(DAT_800ac778 + 8);
do { if (*piVar1 != 0) *piVar1 = *piVar1 + DAT_800ac778; piVar1 = piVar1 + 1; }
while (piVar1 < (int *)(DAT_800ac778 + 0x60));
```
(`RE_15_Quellcode_V2/FUN_800396fc.c`) — also 0x08 .. 0x5C einschliesslich.

| Off | Sektion | Start | Groesse (bis naechste Grenze) | erste 16 Bytes |
|---|---|---|---|---|
| +0x24 | camera (RID) | 0x000060 | 320 | `00 00 3c 68 7d 03 00 00 a7 f0 ff ff 4f d2 ff ff` |
| +0x30 | model_ptr_tbl | 0x0001A0 | 8 | `c8 da 01 00 84 0e 00 00` |
| +0x28 | zone (RVD) | 0x0001A8 | 484 | 24 Zonen (Terminator `FF FF FF FF`) |
| +0x2c | light (LIT) | 0x00038C | 484 | 10x40 B = 400 B Nutzlast, danach die pri-Tabelle |
| +0x20 | collision (SCA) | 0x000570 | 1168 | `50 da 0e cb 13 00 00 00 ...` |
| +0x38 | block (BLK) | 0x000A00 | 64 | `05 00 00 00 3c dd a4 a2 ...` |
| +0x34 | floor (FLR) | 0x000A40 | 16 | `01 00 08 d5 0c 9a c4 6d ...` |
| +0x40 | scd_main | 0x000A50 | 176 | 1 Sub |
| +0x44 | scd_sub | 0x000B00 | 640 | 6 Subs |
| +0x48 | scd_extra | 0x000D80 | 8 | `04 00 06 00 01 00 01 00` |
| +0x3c | message (MSG) | 0x000D88 | 6 Offsets / 5 Texte, endet 0xE84 | s. §4.3 |
| +0x4c | effect (ESP-IDs) | 0x0011E0 | 1180 | `05 07 ff ff ff ff ff ff ...` |
| +0x50 | esp-Offsettabelle (letzter Eintrag) | 0x00167C | 4 | `08 00 00 00` |
| +0x08/0x0c/0x10 | snd0 edt/vh/vb | 0x1680 / 0x1700 / 0x2FA0 | 128 / 3104 / 34256 | `BAVp` = VAB |
| +0x14/0x18/0x1c | snd1 edt/vh/vb | 0x2320 / 0x2380 / 0xB570 | 96 / 3104 / 61624 | |
| +0x54 | esp_tim | 0x01A628 | 13472 | `10 00 00 00 08 00 00 00` (4 bpp) |
| +0x58 | model_tim | 0x01DAC8 | 25632 | `10 00 00 00 09 00 00 00` (8 bpp + CLUT) |
| +0x5c | animation (RBJ) | **0x00000000** | — | keine Cinematic-Clips |

Werkzeug: `analysis/esp_masken_2026-09-03/d_rdt_dump.py`.

### 1.3 Kamera-Cut-Tabelle (RID) @0x60 — 10 x 32 B

Layout gemaess `FUN_800392d4` (s. §3): `+0x1C` ist der pri-Zeiger, Schrittweite 0x20.

| Cut | flag | fov | pos (x,y,z) | target (x,y,z) | pri_offset | **Inhalt @pri_offset** |
|---|---|---|---|---|---|---|
| 0 | 0 | 26684 | (893, -3929, -11697) | (-17202, 3679, -26058) | 0x0000051C | **FF FF FF FF** |
| 1 | 0 | 26684 | (-4782, -3801, -18372) | (18182, 4393, -22530) | 0x00000520 | **FF FF FF FF** |
| 2 | 0 | 26684 | (894, -3925, -11688) | (21240, 1316, -16449) | 0x00000524 | **FF FF FF FF** |
| 3 | 0 | 26684 | (5981, -5504, -13046) | (27614, 7224, -16468) | 0x00000528 | **FF FF FF FF** |
| 4 | 0 | 26684 | (-3829, -3845, -11638) | (19882, 4752, -4922) | 0x0000052C | **FF FF FF FF** |
| 5 | 0 | 26684 | (-7266, -3806, -17278) | (-3224, 2605, 2675) | 0x00000530 | **FF FF FF FF** |
| 6 | 0 | 26684 | (-8292, -838, -6606) | (-6375, -3295, 8211) | 0x00000534 | **FF FF FF FF** |
| 7 | 0 | 26684 | (-7465, -3024, -9613) | (-4265, 2209, -27792) | 0x00000538 | **FF FF FF FF** |
| 8 | 0 | 26684 | (-2824, -1295, -18232) | (-17127, -4112, -31804) | 0x0000053C | **FF FF FF FF** |
| 9 | 1 | 26684 | (2126, -4096, -19522) | (14282, 7149, -32459) | 0x00000540 | **FF FF FF FF** |

Roher Byte-Dump des Zielbereichs (die 10 NULL-Sektionen liegen luekenlos hinter dem
LIGHT-Block, 0x38C + 10*40 = 0x51C):

```
0x510: 00 00 00 00 00 00 00 00 00 00 00 00 ff ff ff ff
0x520: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
0x530: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
0x540: ff ff ff ff | 24 00 00 00 20 00 00 00 1c 00 00 00
```
0x51C..0x543 = **40 Bytes = 10 x 0xFFFFFFFF**. Ab 0x544 folgt eine absteigende
u32-Ruecktabelle (0x24, 0x20, ... 0x00), deren Eintraege alle auf denselben Nullwert
@0x568 zeigen.

**==> Die vorgegebene Messung ist bestaetigt: `hdr7=0`, 10/10 pri_offsets -> `FF FF FF FF`.**

Gegenprobe mit dem **Parser des Ports selbst** (`analysis/esp_masken_2026-09-03/rdtprobe.c`,
gelinkt gegen `rdt_common.c` + `pri_common.c` + `md1_common.c` + `tim_common.c`):

```
parse rc=0  nCut=10 nOmodel=1 prop_count=1 zones=24 sca=95 subs=6
  prop[0]: md1 file-offset=0xE84 size=860   tim file-offset=0x1DAC8 size=25632
           md1_parse=0 meshes=1
             mesh0 tris=0 quads=15 qverts=24
           tim_parse=0  128x192
  cut0  pri_offset=0x0000051C  first_u32=0xFFFFFFFF  parse=0 masks=0
  ...  (cut1..cut9 identisch)
  hdr[7] (per-room mask capacity) = 0
```

### 1.4 SLD-Gegenprobe im BSS (`STAGE1/ROOM114.BSS`, 655360 B = 10 Chunks a 0x10000)

Mit `find_sld()` aus `analysis/esp_masken_2026-09-03/sld.py`:

```
ROOM1140:  cut0..cut9  used_bytes = 21222..30530   SLD hits = []   (0 Treffer, alle 10 Cuts)
ROOM1090:  cut0 [(25704, 66080)]  cut1 [(25196, 66080)]  cut3..cut9, cut11..cut14 ebenso
```
66080 = 0x10220 = exakt ein 256x256-8bpp-TIM (8 + (12+512) + (12+65536)).
ROOM1140 nutzt nur ~21-30 KB je 64-KB-Chunk, der Rest ist **Null** — dort liegt nichts.

### 1.5 Reichweite: STAGE1-Sweep (eigene Messung, `d_stage1_pri.py`)

28 von 78 STAGE1-RDTs haben mindestens eine echte pri-Sektion; **50 haben null**.
`hdr[7]` == max(Summe der Gruppenzahlen ueber alle Cuts) in allen 28 Faellen.

Mit Masken: 1020/1021 (50), 1030 (51), 1031 (51), 1040/1041 (66), 1070/1071 (56),
1090/1091 (71), 10B0/10B1 (63), 1150/1151 (54), 1170 (56), 1171 (56), 1190/1191 (79),
11B0/11B1 (46), 11C0 (62), 11C1 (62), 11E0/11E1 (32), 1200/1201 (32), 1210/1211 (77).

**ROOM1140 UND ROOM1141** (Elza-Variante): beide `nCut=10, hdr7=0, 10 NULL, 0 Sektionen`.
Der Raum wurde in **keiner** der beiden Fassungen je mit Masken bestueckt.

---

## 2. Was ist `nOmodel = 1`? — **Das Rolltor am Flurausgang.** (kein Tisch, kein Pult)

### 2.1 Modelltabelle

`RDT+0x30 = 0x1A0`, 2*nOmodel u32:
```
0x1A0: c8 da 01 00   84 0e 00 00      ->  TIM = 0x0001DAC8 ,  MD1 = 0x00000E84
```
Reihenfolge {TIM, MD1} ist belegt durch die Relokation in `FUN_800396fc`
(`piVar1 = iVar2 + *(RDT+0x30); *piVar1 += base; ... *(int *)(iVar2 + 4) += base;`,
Schleife ueber `*(byte *)(DAT_800ac778 + 2)` = nOmodel).

### 2.2 MD1 @0xE84

```
0xE84: 6c 02 00 00  00 00 00 00  02 00 00 00     length=0x26C(620)  unk=0  objectCount=2
0xE90: 38 00 00 00  18 00 00 00  f8 00 00 00  0f 00 00 00
0xEA4: 70 01 00 00  00 00 00 00  60 02 00 00  38 00 00 00
0xEB4: 18 00 00 00  f8 00 00 00  0f 00 00 00  70 01 00 00
0xEC4: 0f 00 00 00  60 02 00 00
```
-> **1 Mesh**, tri_count = 0, **quad_count = 15**, 24 Vertices, 15 Normalen,
Quad-Faces @+0x170, Quad-UVs @+0x260 (Parser-Referenz `re15_port/engine/src/md1_common.c`,
`HEADER_SIZE=12`, `MESH_HEADER_SIZE=56`).

Geometrie (Modellraum, PSX-Y nach unten positiv):
```
X-Teilung  : -1494 | -498 | +497 | +1493              (3 Spalten a ~996)
Y-Teilung  :    -1 | -1195 | -2391 | -3586 | -4781    (4 Reihen a ~1195)
Z          :  -112  (12 Quads = die Vorhangflaeche)
             -112..+111 bei Y=-1 (3 Quads q0/q3/q4 = die waagerechte Bodenschiene)
BBOX       :  2987 (X) x 4780 (Y, nach OBEN) x 223 (Z)
```
Also eine **3x4-Quadkachelung einer duennen, hohen Platte** plus Bodenschiene —
das Standard-Bauteil eines Rollgitters.

### 2.3 TIM @0x1DAC8

```
0x1DAC8: 10 00 00 00  09 00 00 00      magic=0x10, flags=0x09 -> 8 bpp + CLUT
         CLUT len=1036 org=(0,480) 256x2 · IMG len=24588 org=(0,0) 64x192 words -> 128x192 px
```
Dekodiert: `analysis/esp_masken_2026-09-03/room1140/prop0_model_tim.png` —
**waagerechte Metall-Lamellen** ueber die oberen ~2/3, unten glattes Grau. Rolltor-Textur.

### 2.4 Platzierung — SCD `Obj_model_set` @0xB28 (sub-SCD sub00 +0x28)

```
2d 00 00 00 01 00 0a 00 00 00 2c e4 00 00 c0 e4 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
```
Feldbelegung **aus dem Original**, `LAB_80040914` (ghidra1_V2.txt Zeilen 150719 ff.):
```
8004093c  lbu  a3,0x1(a2)      ; pc[1] = obj_id   -> Pool-Index (Schrittweite 148 @0x800b3f98)
80040940  lbu  a0,0x2(a2)      ; pc[2] -> slot+0x08   (Render-Typ)
80040960  lbu  v0,0x3(a2)      ; pc[3] -> slot+0x09
8004096c  lbu  v0,0x4(a2)      ; pc[4] -> slot+0x82   (Band)
80040990  lhu  v0,0x6(a2)      ; pc[6..7] -> slot+0x00 |1  (Flags)
800409a8  lh   v0,0x8(a2)      ; pc[8..9]   -> slot+0x0c
800409b4  lh   v0,0xa(a2)      ; pc[10..11] -> slot+0x34   = X
800409c0  lh   v0,0xc(a2)      ; pc[12..13] -> slot+0x38   = Y
800409cc  lh   v0,0xe(a2)      ; pc[14..15] -> slot+0x3c   = Z
800409d8  lhu  v0,0x10(a2)     ; pc[16..17] -> slot+0x68   rot_x
800409e4  lhu  v0,0x12(a2)     ; pc[18..19] -> slot+0x6a   rot_y
800409f0  lhu  v1,0x14(a2)     ; pc[20..21] -> slot+0x6c   rot_z
800409f4  addiu v0,a2,0x16     ; pc+0x16    -> slot+0x78   (Rest-Nutzlast)
```
==> obj_id = 0, Typ = 0 (Mesh-Pfad), **pos = (-7124, 0, -6976)**, rot = (0,0,0),
Kollisionsbox pc[22..33] = **alles 0** (der Prop selbst ist nicht kollidierbar).

### 2.5 Drei unabhaengige Bestaetigungen der Identitaet

**(a) Kollision.** SCA-Eintrag [13] (`analysis/.../d_sca.py`):
`x = -9000, z = -7200, w = 3700, d = 500, type = 1` -> Flaeche x -9000..-5300, z -7200..-6700.
Der Prop-Fussabdruck (-8618..-5631 / -7088..-6865) liegt exakt darin. Der Blocker IST das Tor.

**(b) Projektion.** `d_project2.py` baut die Sicht nach
`re15_camera_build_view` (`camera_common.c`, Original `FUN_80053ca4`,
`H = fov >> 7 = 208`) und legt das MD1 ueber die dekodierten Hintergruende.
Kontrollgroesse im selben Bild: die SCA-Rechtecke bei y=0 und y=-4000 — sie decken sich in
allen 10 Cuts mit Wand- und Tischkanten (`room1140/ctl_cut0.png` ... `ctl_cut9.png`).
In **Cut 5** liegen alle 24 Vertices vor der Kamera und fuellen bei Screen-x 82..152,
y 30..134 **exakt die Flurdurchfahrt** (`room1140/overlay_cut5.png`, `ctl_cut5.png`).

**(c) Skript.** sub-SCD sub03 ist die Oeffnungsszene:
```
+0x01A6  Message_on 01      ; "A BEAUTIFUL PAINTING. THERE'S A SWITCH BELOW. WILL YOU PUSH IT?"
+0x01B0  Ck 0c 1f 00
+0x01B4  Set 03 6a 01       ; <- genau das Flag, das sub00 abfragt
+0x01B8  Aot_reset 05
+0x01CA  Cut_chg 05         ; Kamera auf den FLUR-Cut, in dem das Tor steht
+0x01CC  Work_set 03 00     ; Arbeits-Entity = PROP obj_id 0  (scd_work_bind kind==3)
+0x01D0  Speed_set 01 0000 / Speed_set 07 fffe
+0x01D8  For ... { Add_aspeed ; Add_speed ; Evt_next }   (4 geschachtelte Schleifen)
+0x0216  Sca_id_set 00..04 = 0x0d ; Sca_floor_set 00..04 = 0x0d   ; Blocker-Partition wechselt
+0x0246  Cut_auto 01 ; Work_set 01 00 ; Plc_ret
```
und sub00 installiert Tor + Schalter-AOT nur, **solange** das Flag 0 ist:
```
+0x000C  Ifel_ck 0x3e
+0x0010  Ck 03 6a 00
+0x0014  Aot_set 05 (x -9000..-8000, z -11000..-9500 -> Event 3 = das Gemaelde)
+0x0028  Obj_model_set 00 ...   (das Tor)
+0x004A  Else_ck 0x2e
+0x004E  Sca_id_set/Sca_floor_set 00..04 = 0x0d   (Tor weg -> Durchgang offen)
```
Dazu passen die Raumtexte msg0 "YOU'VE OPENED THE LOCK." und msg5 "THE SWITCH IS NOT
RESPONDING...".

**Ergebnis:** Der einzige Raum-Prop ist das **Rollgitter/Rolltor am Flurausgang**,
Teil des Gemaelde-Schalter-Raetsels. Er hat mit Tisch oder Rednerpult nichts zu tun.

### 2.6 Zeichnet der Port ihn?

Der Pfad ist vollstaendig vorhanden:
* `rdt_common.c` schneidet MD1 (0xE84, 860 B) + TIM (0x1DAC8, 25632 B) aus dem RDT,
  `prop_count = 1` (gemessen, s. rdtprobe-Ausgabe oben);
* `pc_load_room_prop_set` (`re15_port/platform/pc/main.c:956`) parst beide und bindet die
  TIM in `RE15_TIM_SLOT_PROP(0)`;
* `op_obj_model_set` (`re15_port/engine/src/scd_vm.c:3593`) legt `g_scd.props[0]` an,
  sobald sub00 laeuft;
* die Zeichenschleife `re15_port/platform/pc/main.c:7708` rendert jeden aktiven Prop bis
  `RE15_SCD_MAX_PROPS` (kein 6er-Cap), mit `re15_prop_culled` als einzigem Gate.

**Nicht gemessen** (kein Live-Lauf in diesem Auftrag): ob `re15_prop_culled` das Tor in
Cut 5 durchlaesst und ob `Work_set(3,0)` + `Add_speed` es im Port wirklich hochfahren.
-> offen, s. §7.

---

## 3. Der Original-Beweis: warum in ROOM1140 nichts ueberdeckt wird

### 3.1 Puffer-Reservierung — `FUN_80039270` (aus `FUN_800396fc` gerufen)

```
8003928c  lbu  v0,0x7(a0)      ; a0 = DAT_800ac778 = residenter RDT-Zeiger ; hdr[7]
80039294  sll  v0,v0,0x2       ; Flag-Feld  = hdr7 * 4
800392a4  lbu  v1,0x7(a0)
800392ac  sll  v1,v1,0x5       ; Prim-Puffer A = hdr7 * 0x20
800392bc  lbu  v0,0x7(a0)
800392c4  sll  v0,v0,0x5       ; Prim-Puffer B = hdr7 * 0x20
800392d0  sw   v0,0x0(a1)      ; Arena-Kopf DAT_800ac77c weitergeschoben
```
`hdr[7] = 0` ==> alle drei Puffer haben Laenge 0.

### 3.2 Aufbau pro Cut — `FUN_800392d4`

```
80039310  lw   v1,0x24(a0)          ; RDT+0x24 = Kamera-Tabelle (absolut nach Relokation)
80039314  sll  v0,v0,0x5            ; Cut-Index * 0x20
80039318  addu v0,v0,v1
8003931c  lw   t5,0x1c(v0)          ; <<< cut[i] + 0x1C = pri-Zeiger
80039324  lw   v1,0x0(t5)           ; erstes Wort der Sektion
80039328  li   v0,-0x1              ; 0xFFFFFFFF
8003932c  bne  v1,v0,LAB_8003933c   ; ungleich -> Masken bauen
80039330  _srl t2,v1,0x10
80039334  j    LAB_8003955c         ; gleich  -> sofort raus (Funktionsepilog)
80039338  _sb  zero,0x0(a0)         ; <<< hdr[0] = 0  = AKTIVE Maskenzahl
```
(`LAB_8003955c` @0x8003955c ist das `lw ra,... / addiu sp,... / jr ra`-Epilog.)

### 3.3 Zeichnen — `FUN_80039590`

```
80039594  lw   v0,-0x3888(v0)  ; DAT_800ac778
800395c0  lbu  s4,0x0(v0)      ; hdr[0] = die in 3.2 gesetzte Zahl
800395cc  beq  s4,zero,LAB_80039678   ; == 0 -> gesamte AddPrim-Schleife uebersprungen
...
8003960c  lw   v0,0x0(v0)      ; DAT_800bb4d4 (Prim-Puffer)
8003961c  jal  SetSprt
80039634  jal  SetDrawMode
```

**==> Fuer jeden der 10 Cuts von ROOM1140 gilt: `*t5 == 0xFFFFFFFF` ==> `hdr[0] = 0` ==>
`FUN_80039590` verlaesst die Funktion vor dem ersten `AddPrim`. Es wird nachweislich
KEIN Vordergrund-Sprite in die Ordering-Table gehaengt.**

Der Port verhaelt sich identisch: `re15_port/platform/pc/main.c:4432-4472` ruft
`re15_pri_parse_section(...)`, bekommt `pri_n = 0` (NULL-Sektion,
`re15_port/engine/src/pri_common.c:44`) und ruft dann
`re15_render_pc_set_pri_rects(..., 0)` — Overdraw aus.
`re15_pri_load_cut_atlas` (`re15_port/platform/pc/src/bg_pc.c:63`) findet ohnehin kein
`BSS/ROOM1140/PRI##.TIM` (das Verzeichnis enthaelt nur BG00..BG09) und liefert 0.
**Hier weicht der Port NICHT vom Original ab.**

---

## 4. ESP-Sektion und SCD-Skripte

### 4.1 ESP-Init — `FUN_80019354`

```
piVar4 = *(int **)(DAT_800ac778 + 0x4c);
if ((piVar4 != (int *)0x0) && (*piVar4 != -1)) {
  FUN_8001945c(piVar4, *(undefined4 *)(DAT_800ac778 + 0x50), piVar4, 8);
  FUN_800194f8(*(undefined4 *)(DAT_800ac778 + 0x58), *(undefined4 *)(DAT_800ac778 + 0x54));
}
```
`FUN_8001945c` liest bis zu **8** Effekt-IDs ab RDT+0x4C und dazu eine **rueckwaerts**
gelesene u32-Offsettabelle, deren LETZTER Eintrag bei RDT+0x50 steht
(`iVar2 = *param_2; param_2 = param_2 + -1;`).

ROOM1140:
```
0x11E0: 05 07 ff ff ff ff ff ff      -> Effekt-IDs 0x05, 0x07, dann Abbruch (0xFF)
0x167C: 08 00 00 00                  -> esp[0] @ 0x11E0+0x008 = 0x11E8
0x1678: d8 01 00 00                  -> esp[1] @ 0x11E0+0x1D8 = 0x13B8
0x1674/0x1670: ff ff ff ff           (unbenutzt)
```
ESP-TIM (RDT+0x54 = 0x1A628): 4 bpp, VRAM-Org **(768,0)**, 256x72 px — dekodiert in
`room1140/esp_tim.png`: **rote Blut-/Gore-Spritzer**. Das ist das Partikel-Sheet der
Zombie-Treffer, kein Vordergrund-Atlas. (IDs 0x05/0x07 sind Raum-Bank-IDs; die
GLOBAL-Bank CORE00.ESP fuehrt nur 0x00/0x02/0x03/0x04/0x08 —
`re15_port/engine/src/re15_esp.c:291 s_global_sheet_id`.)

### 4.2 SCD-Opcode-Zensus (main + sub, vollstaendig disassembliert)

```
0x00 x20  0x01 x7  0x02 x8  0x06 x5  0x07 x3  0x08 x2  0x09 x1  0x0D x5  0x0E x5
0x21 x5   0x22 x7  0x29 x1  0x2B x4  0x2C x5  0x2D x1  0x2E x2  0x2F x4  0x30 x4
0x31 x3   0x37 x10 0x39 x10 0x3B x3  0x3C x1  0x42 x1  0x44 x10 0x46 x5  0x50 x1
```
Gesucht und **NICHT vorhanden**:
`0x3A Sce_espr_on`, `0x4C Sce_espr_kill`, `0x48 Super_set`, `0x49 Super_reset`,
`0x52 Sce_espr_control`, `0x2A Cut_old`, `0x4B Cut_replace`,
`0x60 Kage_set`, `0x64/0x65 Sce_espr_on2/kill2`, `0x72 Super_on`, `0x73 Mirror_set`,
`0x75 Sce_espr3d_on2`.

Das Skript enthaelt also **keine einzige Sprite-, Effekt- oder Vordergrund-Operation**.
Vollstaendige Disassembly: Anhang §9.

### 4.3 Raumtexte (RDT+0x3C = 0xD88, Codierung `byte + 4 = ASCII`, `0x00 = Space`)

```
msg0  YOU'VE OPENED THE LOCK.
msg1  A BEAUTIFUL PAINTING. THERE'S A SWITCH BELOW. WILL YOU PUSH IT?
msg2  IT'S A BIG TABLE.
msg3  THE STREETS ARE INFESTED WITH ZOMBIES!
msg4  THE STREETS ARE INFESTED WITH ZOMBIES!
msg5  THE SWITCH IS NOT RESPONDING...
```
`Aot_set 03` (main sub00 +0x0084) = `x -4900 ... +7800 / z -17800 ... -11100`, Nutzlast-Byte
pc[14] = **02** ==> zeigt msg2 "IT'S A BIG TABLE.".
Der grosse Konferenztisch ist also **rein gemalt** — Hintergrund + SCA-Blocker +
Untersuchungs-AOT. **Er hat kein Modell und keine Maske.** Sein Blocker ist SCA[6]
(`x -4650, z -17450, w 12100, d 6100` -> x -4650..7450, z -17450..-11350).

---

## 5. Frage 4 — Entscheidung, belegt

**Kann die Original-Engine Leon in ROOM1140 hinter dem Tisch verdecken? NEIN.**

Die RE1.5-Engine kennt genau drei Wege, Geometrie VOR eine Figur zu bekommen:

| Weg | Mechanismus | ROOM1140 |
|---|---|---|
| 1 | `sprite.pri`-Masken, OT-Index = `depth` x1 (`FUN_800392d4` / `FUN_80039590`, Shift @0x80039658) | **10/10 Cuts NULL, hdr7=0, kein SLD im BSS** -> tot |
| 2 | Raum-Props aus `Obj_model_set` (Mesh in dieselbe OT, `FUN_8002c18c`) | genau **1** Prop = das Rolltor am Flur; der Tisch ist keiner |
| 3 | ESP-Sprites (`Sce_espr_on` 0x3A -> Row-VM) | **kein 0x3A im ganzen Raum-SCD**; die 2 ESP-Banken sind Blutspritzer |

Einen **vierten** Weg gibt es in RE1.5 nicht: die Opcode-Sprungtabelle
`PTR_LAB_800744a8` reicht von 0x800744a8 bis **0x80074620** — 95 Eintraege = Opcodes
0x00..0x5E; ab 0x80074624 folgen Rohdaten (`10 00 11 00 12 00 13 01 ...`), keine Zeiger.
RE2-Retails `Kage_set` (0x60) und `Mirror_set` (0x73) **existieren in RE1.5 nicht**.

**Einordnung: Luecke des Prototyps, nicht der Engine.** Die Engine und die Werkzeugkette
funktionieren — 28 von 78 STAGE1-RDTs tragen echte Maskensaetze (bis 79 Masken in
ROOM1190). ROOM1140 **und** seine Zweitfassung ROOM1141 wurden schlicht nie bestueckt;
im BSS liegt nicht einmal der Vordergrund-Atlas.

**==> Der Port darf hier nichts erfinden.** Keine synthetischen Masken, keine aus dem
Hintergrund geratenen Rechtecke, kein "Tisch-Modell" nachbauen. Der Port zeichnet
heute schon genau das, was das Original zeichnet: nichts.

---

## 6. Gegenprobe: Hintergruende dekodiert

`analysis/esp_masken_2026-09-03/room1140/bg00.png ... bg09.png` (320x240),
erzeugt mit `bgdump.c` gegen die Port-Dekoder `bss_common.c` + `bss_vlc.c` + `bss_mdec.c`
(Quelle: `re15_port/shared_assets/PSX/BSS/ROOM1140/BG0N.BSS`, alle `id=0x3800`, `ver=2`,
quant=2 ausser BG06 quant=1).

| Cut / BG | Motiv | Tisch/Pult im Vordergrund? |
|---|---|---|
| 0 | Konferenztisch von der NW-Ecke, Doppeltuer hinten | **JA** — Tischkante ueber die untere Bildhaelfte |
| 1 | Tischende links, Computertisch rechts | JA (links) |
| 2 | Rednerpult + Flaggen vor halb heruntergelassenem Rollgitter, Tisch im Vordergrund | **JA** |
| 3 | Rednerpult + zwei US-Flaggen, grosse Fensterfront | Pult frei stehend |
| 4 | Langer Beistelltisch links (Papiere), Konferenztisch rechts, Stativ | **JA** (beide) |
| 5 | Flur nach links, Gemaelde an der Wand, Karte rechts, Tisch rechts unten | JA (rechts) |
| 6 | Metalltuer frontal mit EXIT-Schild | nein |
| 7 | Tischecke links, Tuer rechts | JA (links) |
| 8 | Ecke mit Palme und Beistelltisch | Beistelltisch |
| 9 | Computertisch mit Monitor, Gemaelde | Computertisch |

**Kamera-Position vs. Tischblocker SCA[6] (x -4650..7450, z -17450..-11350):**
die Kameras von **Cut 0 (893, -11697)**, **Cut 2 (894, -11688)** und
**Cut 4 (-3829, -11638)** liegen alle DREI *innerhalb* dieses Rechtecks, d.h. sie haengen
direkt ueber dem Konferenztisch und blicken hinaus. Genau dort — und nur dort — braeuchte
man eine Maske, damit eine Figur hinter der Tischkante verschwindet. Genau dort hat das
Original keine.

Zusatzmaterial im selben Verzeichnis:
`room1140/ctl_cut0..9.png` (SCA-Blocker tuerkis y=0 / blau y=-4000, Prop rot),
`room1140/overlay_cut0..9.png` (nur der Prop),
`room1140/plan_room1140.png` (Draufsicht: SCA, RVD-Zonen magenta, Kameras gelb, Prop rot),
`room1140/prop0_model_tim.png`, `room1140/esp_tim.png`,
`room1140/crop_bg05_door.png`, `room1140/crop_bg02_back.png`.

---

## 7. Was daraus fuer den Nutzer-Report folgt (und was OFFEN bleibt)

Der Befund schliesst die naheliegende Erklaerung aus: "Leon steht auf dem Tisch statt
dahinter" ist in ROOM1140 **keine fehlende Maske**, denn es gab dort nie eine.
Damit bleiben genau drei pruefbare Hypothesen — **keine davon ist in diesem Auftrag
gemessen worden**, alle sind hier NICHT BELEGT:

1. **Kollision.** Steht Leon im Port an einer Stelle, die das Original per SCA[6]
   (x -4650..7450, z -17450..-11350, type 1) sperrt? Konkreter Test: Leon in Cut 0/2/4
   an die Nordkante des Tisches fahren und `g_actors[PLAYER].x/z` gegen dieses Rechteck
   pruefen. Liegt er drin, ist die Kollision der Defekt, nicht die Maske.
2. **Projektion/Hoehe.** Zeichnet der Port ihn an derselben Bildschirmposition und -groesse
   wie die PSX? (Parity-Lauf nach `.claude/skills/re15-parity-verify`; die Kamerakonstante
   ist `H = fov>>7 = 208`.)
3. **Vergleichsmassstab.** Falls gegen RE2-Retail oder ein HD-Projekt verglichen wird:
   dort ist der Raum ein anderer und maskiert.

Ebenfalls offen (aus §2.6): ob das **Rolltor** im Port tatsaechlich gezeichnet und von
sub03 hochgefahren wird. Fehlt es, wirkt der Flurausgang von Anfang an offen — ein
sichtbarer, aber anderer Defekt als der gemeldete.

---

## 8. Werkzeuge / Reproduktion

| Datei | Zweck |
|---|---|
| `analysis/esp_masken_2026-09-03/d_rdt_dump.py` | Header + Verzeichnis + Sektionsgroessen |
| `analysis/esp_masken_2026-09-03/d_cuts.py` | RID-Tabelle + pri_offset-Inhalt |
| `analysis/esp_masken_2026-09-03/d_md1_dump.py` / `d_md1_geom.py` | MD1-Kopf, Vertices, Quads, UVs |
| `analysis/esp_masken_2026-09-03/d_sca.py` | SCA-Kollision (95 Eintraege, 5x19) |
| `analysis/esp_masken_2026-09-03/d_scd.py` | vollstaendige SCD-Disassembly (main + 6 subs) |
| `analysis/esp_masken_2026-09-03/d_tim.py` | Prop-TIM + ESP-TIM -> PNG |
| `analysis/esp_masken_2026-09-03/d_project.py` / `d_project2.py` | Prop + SCA in die 10 Cuts projiziert |
| `analysis/esp_masken_2026-09-03/d_plan.py` | Draufsicht-Grundriss |
| `analysis/esp_masken_2026-09-03/d_stage1_pri.py` | STAGE1-weiter pri-Sweep (78 RDTs) |
| `analysis/esp_masken_2026-09-03/d_crop.py` | PNG-Ausschnitt/Zoom |
| `analysis/esp_masken_2026-09-03/bgdump.c` | BSS -> PPM ueber die Port-Dekoder |
| `analysis/esp_masken_2026-09-03/rdtprobe.c` | RDT/PRI/MD1/TIM durch den **Port-Parser** |

Bauen (Git-Bash, msys64 zuerst im PATH):
```
gcc -O2 -I re15_port/include -o bgdump.exe analysis/esp_masken_2026-09-03/bgdump.c \
    re15_port/engine/src/bss_common.c re15_port/engine/src/bss_vlc.c re15_port/engine/src/bss_mdec.c
gcc -O1 -I re15_port/include -o rdtprobe.exe analysis/esp_masken_2026-09-03/rdtprobe.c \
    re15_port/engine/src/rdt_common.c re15_port/engine/src/pri_common.c \
    re15_port/engine/src/md1_common.c re15_port/engine/src/tim_common.c
```

---

## 9. Anhang — vollstaendige SCD-Disassembly ROOM1140

```
=== main SCD (RDT+0x40) @0xA50..0xB00 (size 176)  table=1 entries: 0x2
  --- sub00 @+0x2 (file 0xA52..0xB00, 174 B)
    +0x0002  3b Door_aot_set       00 02 31 00 00 74 dc da b2 e8 03 a0 0f ec fa 00 00 82 c9 00 08 00 13 00 00 00 00 00 00 00 00
    +0x0022  06 Ifel_ck            00 3c 00
    +0x0026  21 Ck                 04 c3 00
    +0x002A  3b Door_aot_set       01 00 31 00 00 c6 df 12 fd d0 07 e8 03 4f aa 00 00 8f 9b 00 0c 00 17 09 00 00 00 00 00 00 00
    +0x004A  2c Aot_set            02 03 31 00 00 c6 df 12 fd d0 07 e8 03 ff 00 18 02 00 00
    +0x005E  07 Else_ck            00 26 00
    +0x0062  3b Door_aot_set       01 02 31 00 00 c6 df 12 fd d0 07 e8 03 4f aa 00 00 8f 9b 00 0c 00 17 09 00 00 00 00 00 00 00
    +0x0082  00 nop                
    +0x0083  00 nop                
    +0x0084  2c Aot_set            03 01 31 00 00 dc ec 78 ba 9c 31 2c 1a 02 00 ff ff 00 00
    +0x0098  2c Aot_set            04 01 31 00 00 46 ec 0e a2 ee 1b e8 03 04 00 ff ff 00 00
    +0x00AC  01 Evt_end            00
    +0x00AE  00 nop                
    +0x00AF  00 nop                

=== sub SCD (RDT+0x44) @0xB00..0xD80 (size 640)  table=6 entries: 0xC, 0x17A, 0x186, 0x1A6, 0x252, 0x266
  --- sub00 @+0xC (file 0xB0C..0xC7A, 366 B)
    +0x000C  06 Ifel_ck            00 3e 00
    +0x0010  21 Ck                 03 6a 00
    +0x0014  2c Aot_set            05 03 31 00 00 d8 dc 08 d5 e8 03 dc 05 ff 00 18 03 00 00
    +0x0028  2d Obj_model_set      00 00 00 01 00 0a 00 00 00 2c e4 00 00 c0 e4 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    +0x004A  07 Else_ck            00 2e 00
    +0x004E  37 Sca_id_set         00 0d 00
    +0x0052  37 Sca_id_set         01 0d 00
    +0x0056  37 Sca_id_set         02 0d 00
    +0x005A  37 Sca_id_set         03 0d 00
    +0x005E  37 Sca_id_set         04 0d 00
    +0x0062  39 Sca_floor_set      00 0d 00
    +0x0066  39 Sca_floor_set      01 0d 00
    +0x006A  39 Sca_floor_set      02 0d 00
    +0x006E  39 Sca_floor_set      03 0d 00
    +0x0072  39 Sca_floor_set      04 0d 00
    +0x0076  00 nop                
    +0x0077  00 nop                
    +0x0078  06 Ifel_ck            00 96 00
    +0x007C  21 Ck                 03 d2 00
    +0x0080  2c Aot_set            06 03 41 00 00 80 f3 02 a4 88 13 d4 17 ff 00 18 04 00 00
    +0x0094  50 Item_aot_set       07 09 31 00 00 1e fb c6 ad e8 03 e8 03 15 00 1e 00 a8 00 ff 00
    +0x00AA  44 Sce_em_set         00 16 88 00 00 00 d3 e0 fc 00 00 88 af 00 00 00 04 00 00
    +0x00BE  44 Sce_em_set         01 10 86 00 00 01 d4 f8 f8 00 00 70 b3 00 00 00 02 00 00
    +0x00D2  44 Sce_em_set         02 10 86 00 00 01 d5 f8 f8 00 00 a0 ab 00 00 00 0e 00 00
    +0x00E6  44 Sce_em_set         03 11 86 00 00 01 d6 c8 00 00 00 a0 ab 00 00 00 0a 00 00
    +0x00FA  44 Sce_em_set         04 11 86 00 00 01 d7 c8 00 00 00 70 b3 00 00 00 06 00 00
    +0x010E  07 Else_ck            00 6a 00
    +0x0112  44 Sce_em_set         00 16 02 00 00 00 d3 22 0b 00 00 c4 d7 00 00 00 00 00 00
    +0x0126  44 Sce_em_set         01 10 02 00 00 01 d4 06 e6 00 00 c4 d7 00 00 00 04 00 00
    +0x013A  44 Sce_em_set         02 10 02 00 00 01 d5 78 ec 00 00 da b2 00 00 00 0a 00 00
    +0x014E  44 Sce_em_set         03 11 02 00 00 01 d6 f0 23 00 00 8e c7 00 00 00 08 00 00
    +0x0162  44 Sce_em_set         04 11 02 00 00 01 d7 a8 16 00 00 32 b5 00 00 00 04 00 00
    +0x0176  00 nop                
    +0x0177  00 nop                
    +0x0178  01 Evt_end            00
  --- sub01 @+0x17A (file 0xC7A..0xC86, 12 B)
    +0x017A  46 Aot_reset          05 03 31 ff 00 18 05 00 00
    +0x0184  01 Evt_end            00
  --- sub02 @+0x186 (file 0xC86..0xCA6, 32 B)
    +0x0186  46 Aot_reset          02 00 00 00 00 00 00 00 00
    +0x0190  22 Set                04 c3 01
    +0x0194  2b Message_on         00 ff ff
    +0x0198  02 Evt_next           
    +0x0199  00 nop                
    +0x019A  46 Aot_reset          01 02 31 4f aa 00 00 8f 9b
    +0x01A4  01 Evt_end            00
  --- sub03 @+0x1A6 (file 0xCA6..0xD52, 172 B)
    +0x01A6  2b Message_on         01 80 ff
    +0x01AA  02 Evt_next           
    +0x01AB  00 nop                
    +0x01AC  06 Ifel_ck            00 a0 00
    +0x01B0  21 Ck                 0c 1f 00
    +0x01B4  22 Set                03 6a 01
    +0x01B8  46 Aot_reset          05 00 00 00 00 00 00 00 00
    +0x01C2  22 Set                02 07 01
    +0x01C6  22 Set                01 1b 01
    +0x01CA  29 Cut_chg            05
    +0x01CC  2e Work_set           03 00
    +0x01CF  00 nop                
    +0x01D0  2f Speed_set          01 00 00
    +0x01D4  2f Speed_set          07 fe ff
    +0x01D8  0d For                00 06 00 14 00
    +0x01DE  31 Add_aspeed         
    +0x01DF  30 Add_speed          
    +0x01E0  02 Evt_next           
    +0x01E1  00 nop                
    +0x01E2  0e Next               00
    +0x01E4  0d For                00 2c 00 0a 00
    +0x01EA  0d For                00 04 00 0a 00
    +0x01F0  30 Add_speed          
    +0x01F1  02 Evt_next           
    +0x01F2  0e Next               00
    +0x01F4  2f Speed_set          07 03 00
    +0x01F8  0d For                00 06 00 03 00
    +0x01FE  31 Add_aspeed         
    +0x01FF  30 Add_speed          
    +0x0200  02 Evt_next           
    +0x0201  00 nop                
    +0x0202  0e Next               00
    +0x0204  2f Speed_set          07 fd ff
    +0x0208  0d For                00 06 00 03 00
    +0x020E  31 Add_aspeed         
    +0x020F  30 Add_speed          
    +0x0210  02 Evt_next           
    +0x0211  00 nop                
    +0x0212  0e Next               00
    +0x0214  0e Next               00
    +0x0216  37 Sca_id_set         00 0d 00
    +0x021A  37 Sca_id_set         01 0d 00
    +0x021E  37 Sca_id_set         02 0d 00
    +0x0222  37 Sca_id_set         03 0d 00
    +0x0226  37 Sca_id_set         04 0d 00
    +0x022A  39 Sca_floor_set      00 0d 00
    +0x022E  39 Sca_floor_set      01 0d 00
    +0x0232  39 Sca_floor_set      02 0d 00
    +0x0236  39 Sca_floor_set      03 0d 00
    +0x023A  39 Sca_floor_set      04 0d 00
    +0x023E  22 Set                02 07 00
    +0x0242  22 Set                01 1b 00
    +0x0246  3c Cut_auto           01
    +0x0248  2e Work_set           01 00
    +0x024B  00 nop                
    +0x024C  42 Plc_ret            
    +0x024D  00 nop                
    +0x024E  08 Endif              00
    +0x0250  01 Evt_end            00
  --- sub04 @+0x252 (file 0xD52..0xD66, 20 B)
    +0x0252  46 Aot_reset          06 00 00 00 00 00 00 00 00
    +0x025C  09 Sleep              0a 01 00
    +0x0260  22 Set                03 d2 01
    +0x0264  01 Evt_end            00
  --- sub05 @+0x266 (file 0xD66..0xD80, 26 B)
    +0x0266  2b Message_on         01 80 ff
    +0x026A  02 Evt_next           
    +0x026B  00 nop                
    +0x026C  06 Ifel_ck            00 0c 00
    +0x0270  21 Ck                 0c 1f 00
    +0x0274  2b Message_on         05 ff ff
    +0x0278  02 Evt_next           
    +0x0279  00 nop                
    +0x027A  08 Endif              00
    +0x027C  01 Evt_end            00
    +0x027E  00 nop                
    +0x027F  00 nop                

=== extra SCD (RDT+0x48) @0xD80..0xD88: 04 00 06 00 01 00 01 00
```

---

## Gegenpruefung

Unabhaengige Nachpruefung (eigene Python-Leser gegen die Roh-Bytes, eigene `grep -n` in
`ghidra1_V2.txt`, eigene Projektion). Es wurde KEINE Zahl aus dem Bericht uebernommen.

### Ergebnis: 14/14 Kern-Behauptungen halten. Vier Beleg-Details sind falsch formuliert bzw. falsch gezaehlt.

---

### NICHT haltbar

**(N1) Behauptung 5, Positivkontrolle: "12 von 16 Cuts" ist falsch — es sind 13 von 16.**

Eigener Lauf, `find_sld()` aus `analysis/esp_masken_2026-09-03/sld.py`, Default-Parameter, gegen
`re15_port/shared_assets/PSX/STAGE1/ROOM109.BSS` (1048576 B = 16 x 0x10000):

```
cut0  [(25704, 66080)]     cut8  [(26528, 66080)]
cut1  [(25196, 66080)]     cut9  [(26648, 66080)]
cut2  []                   cut10 []
cut3  [(29588, 66080)]     cut11 [(32188, 66080)]
cut4  [(31632, 66080)]     cut12 [(22100, 66080)]
cut5  [(23320, 66080)]     cut13 [(27300, 66080)]
cut6  [(20280, 66080)]     cut14 [(25276, 66080)]
cut7  [(30128, 66080)]     cut15 []
-> 13 Cuts mit SLD, leer sind NUR cut2, cut10, cut15.
```

`ROOM1090.RDT` Byte[1] = 0x10 = 16 Cuts (bestaetigt), Byte[7] = 71, Byte[2] = 4.
Der Bericht selbst (Zeile 120) listet korrekt `cut0 cut1 cut3..cut9 cut11..cut14` = 13; nur die
zusammengefasste Zaehlung "12" ist falsch. Die Aussagerichtung (Positivkontrolle greift) ist
davon unberuehrt.

**(N2) Behauptung 1, Ortsangabe falsch formuliert: "Cut-Tabelle @0x60, +0x1C je 32-B-Record =
0x51C,0x520,...,0x540".**

Die `+0x1C`-FELDER liegen bei 0x7C, 0x9C, 0xBC, 0xDC, 0xFC, 0x11C, 0x13C, 0x15C, 0x17C, 0x19C
(= 0x60 + i*32 + 0x1C). 0x51C..0x540 sind die WERTE, die dort stehen. Eigener Dump:

```
cut0 ... 1c 05 00 00  pri=0x51c  *(0x51c)=ffffffff
cut1 ... 20 05 00 00  pri=0x520  *(0x520)=ffffffff
...
cut9 ... 40 05 00 00  pri=0x540  *(0x540)=ffffffff
0x51C..0x543 = 40x 0xFF   (all(b==0xFF) -> True)
```

Der Sachverhalt stimmt, der Satz beschreibt ihn falsch.

**(N3) Behauptung 8c, Beleg-Detail: "4x For{Add_aspeed,Add_speed}" ist falsch — es sind 5
For-Bloecke, davon 3 mit diesem Rumpf.**

Eigene strukturierte Disassemblierung von sub03 (0xCA6..0xD52, Groessen aus
`re15_port/engine/src/scd_vm.c:s_opcode_sizes`; die Verschachtelung schliesst exakt auf 0xD52 auf,
der Lauf ist also selbst-konsistent):

```
0x0cd8 0d 00 06 00 14 00     For count=20  { 31 30 02 00 }
0x0ce4 0d 00 2c 00 0a 00     For count=10  (Huelle)
0x0cea   0d 00 04 00 0a 00     For count=10  { 30 02 }        <- NUR Add_speed
0x0cf8   0d 00 06 00 03 00     For count=3   { 31 30 02 00 }
0x0d08   0d 00 06 00 03 00     For count=3   { 31 30 02 00 }
```

= 5x Opcode 0x0D, davon 3 mit {0x31,0x30}. Die Schlussfolgerung ("faehrt ihn hoch") bleibt und ist
unten sogar staerker belegt.

**(N4) Behauptung 9, Beleg-Detail: die Codierungsangabe "(byte+4=ASCII)" trifft nicht zu.**

Wendet man byte+4 stur an, liest msg2 `)T>S<00>A<00>BIG<00>TABLE[` — 'I' wird zu ')', der
Apostroph zu '>', der Punkt zu '['. Es ist eine Font-Tabelle, kein ASCII-Offset (Satzanfangs-
Buchstaben liegen um 0x20 versetzt: ')'=0x29 vs 'I'=0x49, '4'=0x34 vs 'T'=0x54, '!'=0x21 vs
'A'=0x41, '7'=0x37 vs 'W'=0x57, '9'=0x39 vs 'Y'=0x59). Der INHALT der Behauptung — msg2 ist die
Tisch-Untersuchungs-Meldung — stimmt.

---

### Unabhaengig BESTAETIGT

**B1 (Beh. 1)** `ROOM1140.RDT` (147176 B) Byte 0x00..0x07 = `00 0a 01 00 00 00 00 00`
-> nSprite=0, nCut=10, nOmodel=1, hdr7=0. Alle 10 pri-Zeiger 0x51C..0x540, dort 40x 0xFF.

**B2 (Beh. 2)** `@0x80039310 lw v1,0x24(a0)` / `@0x80039314 sll v0,v0,0x5` / `@0x80039318 addu v0,v0,v1`
/ `@0x8003931c lw t5,0x1c(v0)` — woertlich in `ghidra1_V2.txt:137258-137261`. Funktionskopf
`FUN_800392d4 @0x800392d4: lui a0,0x800b / lw a0,-0x3888(a0)=>DAT_800ac778` und
`lh v0,DAT_800b0fe4` (Cut-Index) — a0 IST die RDT-Basis, +0x24 IST die Cut-Tabelle (Datei: 0x60).
Relokation in `RE_15_Quellcode_V2/FUN_800396fc.c` woertlich wie zitiert, inkl. Header-Verzeichnis
`base+8 .. base+0x60` und der nOmodel-Zeigerpaare bei `+0x30` (beide Worte je Paar).

**B3 (Beh. 3)** `@0x80039324 lw v1,0x0(t5)` / `@0x80039328 li v0,-0x1` / `@0x8003932c bne v1,v0,LAB_8003933c`
/ `@0x80039334 j LAB_8003955c` / `@0x80039338 _sb zero,0x0(a0)` (137263-137268).
`LAB_8003955c @0x8003955c = lw ra,0x64(sp) ... 80039588 jr ra` = Epilog (137413-137418).
Zeichenfunktion `FUN_80039590 @0x80039590` liest `@0x800395c0 lbu s4,0x0(v0)` (v0 = DAT_800ac778)
und `@0x800395cc beq s4,zero,LAB_80039678`. Zusaetzlich habe ich den uebersprungenen Rumpf
verifiziert: `@0x8003961c jal SetSprt`, `@0x80039634 jal SetDrawMode`, `@0x80039640 jal MargePrim`,
`@0x80039660 jal AddPrim`, Schleifenkopf `LAB_800395e8`, Ruecksprung `@0x80039670 bne v0,zero,LAB_800395e8`,
`LAB_80039678 @0x80039678 = lw ra,0x40(sp)` = Epilog. **Zusatzbefund:** in ROOM1140 steht Byte[0]
schon AUF DER DISC auf 0x00 — die Verdeckung ist damit unabhaengig von der Aufrufreihenfolge
FUN_800392d4/FUN_80039590 tot.

**B4 (Beh. 4)** `@0x8003928c lbu v0,0x7(a0)` / `@0x80039294 sll v0,v0,0x2` / `@0x800392a0 sw v0,DAT_800bb4d4`;
`@0x800392a4 lbu v1,0x7(a0)` / `@0x800392ac sll v1,v1,0x5` / `@0x800392b8 sw v1,DAT_800bb4d8`;
`@0x800392bc lbu v0,0x7(a0)` / `@0x800392c4 sll v0,v0,0x5` / `@0x800392d0 sw v0,0x0(a1)=>DAT_800ac77c`
(137173-137190). hdr7=0 -> alle drei Groessen 0.

**B5 (Beh. 5)** `ROOM114.BSS` = 655360 B = 10 x 0x10000; `find_sld()` liefert `[]` fuer cut0..cut9.
Genutzte Bytes je Chunk (letztes Nicht-Null-Byte + 1): 23160/26862/28026/23294/30530/27744/26704/
24782/21790/21222 -> Spanne **21222..30530**, exakt wie behauptet. Chunk-Header alle
`.. .. 00 38 .. 00 02 00` = id 0x3800, ver 2.

**B6 (Beh. 6)** Modelltabelle @0x1A0 = `c8 da 01 00 84 0e 00 00` -> TIM 0x1DAC8, MD1 0xE84.
MD1 @0xE84 = `6c 02 00 00 00 00 00 00 02 00 00 00` (len 0x26C, objectCount 2 = 1 Mesh).
Eigener MD1-Parse: tri_count=0, **quad_count=15**, quad_vtx_count=**24**,
X = {-1494,-498,497,1493} (3 Spalten), Y = {-1,-1195,-2391,-3586,-4781} (4 Reihen),
Z = {-112, 111} -> 3x4 = 12 Platten-Quads + 3 Schienen-Quads = 15. Ausdehnung **2987 x 4780 x 223**.
Die 4 Vertices mit Z=+111 (idx 0,3,7,8) liegen alle auf Y=-1, also am BODEN -> Bodenschiene.
TIM @0x1DAC8 = `10 00 00 00 09 00 00 00` -> 8bpp+CLUT, CLUT(0,480) 256x2, Bild 128x192, gesamt 25632 B.
**Objektive Gegenprobe zur "waagerechten Lamellen"-Aussage** (statt Bildbeschau): Zeilenmittel-
Standardabweichung der Luminanz = **4.02**, Spaltenmittel = **0.08**, Zeilenmittel periodisch mit
Periode 5 (`7 13 10 11 6 | 7 13 10 11 6 | ...`) -> rein waagerechte Streifen. Palette: grau
(128,136,144)/(48,48,48)/(80,88,96) = Metall.
Obj_model_set @0xB28 = `2d 00 00 00 01 00 0a 00 00 00 2c e4 00 00 c0 e4 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00` -> obj_id 0, Typ 0, pos (-7124, 0, -6976), pc[22..33] alle 0.

**B7 (Beh. 7)** `@0x8004093c lbu a3,0x1(a2)`; `@0x80040940 lbu a0,0x2(a2)` -> `@0x8004095c sb a0,0x8(a1)`;
`@0x8004096c lbu v0,0x4(a2)` -> `@0x80040974 sb v0,0x82(a1)`; `@0x800409a8 lh v0,0x8(a2)` -> `sw 0xc(a1)`;
`@0x800409b4 lh v0,0xa(a2)` -> `@0x800409bc sw v0,0x34(a1)`; `@0x800409c0 lh v0,0xc(a2)` -> `sw 0x38(a1)`;
`@0x800409cc lh v0,0xe(a2)` -> `sw 0x3c(a1)`; `@0x800409d8/e4/f0 lhu 0x10/0x12/0x14` -> `sh 0x68/0x6a/0x6c`;
`@0x800409f4 addiu v0,a2,0x16`. Index-Rechnung `@0x80040944-58`: a3*8+a3=a3*9, *4=a3*36, +a3=a3*37,
*4 = **a3*148** — bestaetigt. Anwendung auf 0xB28: pc[10..11]=`2c e4`=-7124, pc[12..13]=0,
pc[14..15]=`c0 e4`=-6976 — deckt sich mit B6.

**B8 (Beh. 8)** (a) SCA-Header @0x570 = `50 da 0e cb | 13 00 00 00` x5 -> 5 Partitionen a 19 = 95
Eintraege. SCA[13] = `74 0e f4 01 d8 dc e0 e3 01 ff 00 03` -> w=3700 d=500 x=-9000 z=-7200 type=1,
Flaeche -9000..-5300 / -7200..-6700. Prop-Fussabdruck aus B6: X -8618..-5631, Z -7088..-6865 —
vollstaendig innerhalb.
(b) **Eigene, unabhaengig implementierte Projektion** (LookAt aus pos/target, H = fov>>7):
Cut5 fov=26684 -> **H=208**, pos (-7266,-3806,-17278), target (-3224,2605,2675). Alle 24 Vertices
mit z_cam > 0 (8977..11187), Bildschirm-Box **x 82.4..152.0 / y 30.8..134.2** — deckt sich mit den
berichteten "82..152 / 30..134". `bg05.png` zeigt an genau dieser Stelle die Flur-Durchfahrt mit
EXIT-Schild.
(c) sub-Tabelle @0xB00 = `0c 00 7a 01 86 01 a6 01 52 02 66 02` -> 6 Subs, sub00 @+0x0C, sub03 @+0x1A6.
sub00 @0xB0C beginnt `06 00 3e 00 | 21 03 6a 00` (If / Ck bank3 bit0x6A ==0), darin Aot_set 05 und
das Obj_model_set. sub03 @0xCA6: `2b 01 80 ff` (Message_on, 0x2B = SCD_OP_MESSAGE_ON,
`re15_port/include/re15_scd.h:620`), `21 0c 1f 00` (Ck), `22 03 6a 01` (Set), `29 05` (Cut_chg 5),
**`2e 03 00`** (Work_set kind=3; `scd_work_bind` in `scd_vm.c` sucht bei kind==3 den Prop mit
obj_id == idx), `2f 01 00 00`, `2f 07 fe ff`, dann die For-Bloecke, dann
`37 00 0d / 37 01 0d / 37 02 0d / 37 03 0d / 37 04 0d` und `39 00 0d / 39 01 0d / 39 02 0d /
39 03 0d / 39 04 0d` — **fuenf mal Element 0x0D = SCA-Eintrag 13 in JEDER der fuenf Partitionen**,
also genau der Rolltor-Blocker aus (a). Das ist der staerkste Beleg im ganzen Bericht.
**Zusatz-Beleg fuer "faehrt ihn hoch" (im Bericht nicht ausgefuehrt):** Opcode 0x2F schreibt
u16 nach `+0x158 + id*2` (`RE15_SCD_OPCODES_REFERENCE.md` Zeile 104, `@0x80040f14`); id=7 ->
+0x166. Opcode 0x31 (`@0x80040fd4`) addiert +0x164..+0x16E in +0x158..+0x162, also +0x166 -> +0x15A.
Opcode 0x30 (`@0x80040f40`) addiert +0x158/15A/15C als Positions-Delta, +0x15A = Y.
`2f 07 fe ff` = Y-Geschwindigkeit **-2**; PSX-Y ist nach unten positiv -> negativ = AUFWAERTS.
Summe ueber alle Schleifen ca. -6550 Einheiten > Torhoehe 4781 -> das Tor faehrt vollstaendig
aus dem Bild. Byte-true bestaetigt.

**B9 (Beh. 9)** SCA[6] = `44 2f d4 17 d6 ed d6 bb 01 ff 00 03` -> w=12100 d=6100 x=-4650 z=-17450
type=1. Aot_set @0xAD4 (= 0xA50+0x84) = `2c 03 01 31 00 00 dc ec 78 ba 9c 31 2c 1a 02 00 ff ff 00 00`
-> x -4900..7800, z -17800..-11100, sce=pc[2]=1, Nutzlast pc[14]=0x02. MSG-Verzeichnis @0xD88 hat
6 Eintraege; Eintrag 2 @0xDF5 ist die Tisch-Meldung. nOmodel=1 und dieses eine Modell ist das
Rolltor (B6) -> Tisch hat kein Modell und keine Maske.

**B10 (Beh. 10)** Eigener Opcode-Zensus (eigener Walker, Groessen aus `s_opcode_sizes`, Start an der
jeweiligen Sub-Tabelle; **jeder Sub endet exakt auf der naechsten Sub-Grenze bzw. auf dem
Sektionsende — keine einzige Fehl-Ausrichtung**, was den Walk selbst validiert):

```
main-SCD 0xA50..0xB00 (1 Sub) + sub-SCD 0xB00..0xD80 (6 Subs)
0x00 0x01 0x02 0x06 0x07 0x08 0x09 0x0D 0x0E 0x21 0x22 0x29 0x2B 0x2C 0x2D 0x2E
0x2F 0x30 0x31 0x37 0x39 0x3B 0x3C 0x42 0x44 0x46 0x50
```

Zeichen-fuer-Zeichen identisch mit der Liste im Bericht. 0x2D genau **1x** (0xB28).
Abwesend geprueft und bestaetigt: 0x3A, 0x4C, 0x48, 0x49, 0x52, 0x2A, 0x4B, 0x60, 0x64, 0x65,
0x72, 0x73, 0x75. 0x3A ist laut `RE15_SCD_OPCODES_REFERENCE.md:113` die EINZIGE `draw-sprite`-Zeile
(`@0x80041864` -> `0x80019700`) und fehlt.
Dispatch-Tabelle: `800744a8 d8 f1 03 80  addr LAB_8003f1d8` ... `8007461c addr LAB_8004203c` ...
`80074620 04 2b 04 80  addr LAB_80042b04`; ab `80074624` Rohbytes `10 00 11 00 12 00 13 01 16 00
18 00 1a 02 20 03 21 04 ...` (von `FUN_8003e9d4:8003e9d8` als BYTE gelesen, nicht als Zeiger).
`grep -c "addr "` ueber 800744a8..80074620 = **95** Eintraege = 0x00..0x5E.
**Praezisierung (kein Widerspruch):** 0x44 ist im Skript 10x vorhanden und im Katalog als
`spawn-alloc` aus dem "effect/entity pool" gefuehrt — es sind hier aber Gegner-Spawns
(pc[2] = 0x16/0x10/0x10/0x11/0x11, zweimal je 5 Stueck), keine Effekt-Sprites.

**B11 (Beh. 11)** `RE_15_Quellcode_V2/FUN_80019354.c` woertlich wie zitiert (`RDT+0x4c`,
`*piVar4 != -1`, `FUN_8001945c(piVar4, RDT+0x50, piVar4, 8)`, `FUN_800194f8(RDT+0x58, RDT+0x54)`).
`FUN_8001945c.c` liest IDs vorwaerts (`param_1 = param_1 + 1`) und Offsets rueckwaerts
(`param_2 = param_2 + -1`) — bestaetigt.
Bytes: 0x11E0 = `05 07 ff ff ff ff ff ff`; 0x167C = `08 00 00 00` -> esp[0] @0x11E8;
0x1678 = `d8 01 00 00` -> esp[1] @0x13B8. ESP-TIM @0x1A628 = `10 00 00 00 08 00 00 00`,
CLUT(0,480) 16x1, Bild org **(768,0)**, **256x72**, 4bpp — alles bestaetigt.
**Objektive Gegenprobe zu "Blut" (statt Bildbeschau):** die 5 haeufigsten Nicht-Index-0-Farben sind
rgb(80,8,0), (56,0,8), (48,16,16), (104,16,0), (136,24,0) — reine Rottoene.
`re15_esp.c:291`: `s_global_sheet_id[] = { 0x00, 0x02, 0x03, 0x04, 0x08 }` — 0x05/0x07 koennen
nicht aus der Global-Bank kommen.

**B12 (Beh. 12)** Eigener STAGE1-Sweep (80 RDT-Dateien, davon 2 mit 4 Byte = ROOM1270/1271, die die
`len<0x60`-Schranke aussortiert -> **78** vergleichbare RDTs): **28** RDTs haben mindestens einen
Cut mit `*(pri) != 0xFFFFFFFF`. ROOM1190 hdr7=**79**, nCut=15, 10 echte Cuts; ROOM1210 hdr7=**77**,
nCut=9, 8 echte Cuts; ROOM1140 UND ROOM1141 hdr7=0, nCut=10, **0** echte Cuts.
Port: `re15_port/platform/pc/main.c:4430-4472` — `pri_n = re15_pri_parse_section(...)`,
danach `else { re15_render_pc_set_pri_rects(NULL,...,0); }`.
`re15_port/platform/pc/src/bg_pc.c:63 re15_pri_load_cut_atlas()` — `snprintf(rel,...,
"BSS/ROOM%04X/PRI%02d.TIM", ...)`, bei Fehlschlag `re15_render_pc_set_pri_atlas(NULL,0,0); return 0;`.
`ls re15_port/shared_assets/PSX/BSS/ROOM1140/` = **nur** BG00.BSS..BG09.BSS, keine PRI##.TIM.
Der Port weicht in ROOM1140 nicht vom Original ab.

**B13 (Beh. 13)** Cut-Record-Layout aus `re15_port/include/re15_camera.h:28-38` (flag u16, fov u16,
pos xyz s32, target xyz s32, pri u32). Cut0 pos (**893, -3929, -11697**), Cut2 (**894, -3925,
-11688**), Cut4 (**-3829, -3845, -11638**). SCA[6]-Flaeche x -4650..7450, z -17450..-11350 —
alle drei (x,z) liegen darin. `bg00.png` und `bg04.png` (eigene Sichtpruefung) zeigen den grossen
Konferenztisch in der unteren Bildhaelfte im Vordergrund.

**B14 (Beh. 14)** Die Datenpunkte sind unabhaengig reproduziert: md1 @0xE84, 1 Mesh / 0 Tris /
15 Quads / 24 Quad-Vertices; tim @0x1DAC8, 128x192, 25632 B. Die angegebene `size=860` ist die
Port-Spanne 0x11E0 - 0xE84 = 0x35C = 860 (naechste Sektion = ESP-IDs) — konsistent.
Code-Stellen existieren: `main.c:955 pc_load_room_prop_set`, `scd_vm.c:3593 op_obj_model_set`,
`main.c:7708` Zeichenschleife mit einzigem Gate `re15_prop_culled(...)`.
Die Selbst-Einschraenkung "kein Live-Lauf" ist korrekt und bleibt bestehen.

---

### Fazit der Gegenpruefung

Die inhaltliche Antwort auf Frage 4 — **ROOM1140 hat im Original keinen Vordergrund-Verdecker,
und der PC-Port bildet das korrekt ab** — haelt jeder Nachpruefung stand. Die drei tragenden
Wege (sprite.pri / Obj_model_set / SCD-Sprite-Opcode) sind einzeln byte-belegt, und der
SCA-13-Beleg in sub03 (`37 0x00..0x04 0x0D` + `39 0x00..0x04 0x0D`) bindet das eine vorhandene
Modell zwingend an das Rolltor. Zu korrigieren sind nur N1 (13 statt 12), N2 (Ortsangabe),
N3 (5 statt 4 For-Bloecke) und N4 (Codierungs-Notiz).
