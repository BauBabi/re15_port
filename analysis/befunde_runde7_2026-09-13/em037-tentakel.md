# EM037 — Birkins TENTAKEL (RE2 Typ 0x37): Vollzensus + Umsetzungs-Spezifikation
*(Runde 7, 2026-09-13. Nutzer: „Birkin hat immer noch keine Tentakel.")*

Vorbefunde: `analysis/befunde_runde6_2026-09-12/birkin-g5-ki.md` (G5-Vollzensus, Sender
0x80104E9C/0x80104E5C, Muster @0x80105674) und `analysis/befunde_runde3_2026-09-12/g5-optik.md`.
Dieses Dossier schliesst die dort offen gelassene Luecke „EM037-Tentakel-Overlay (eigener Zensus
noetig, wenn Tentakel portiert werden)" **vollstaendig**.

Alle `0x8010xxxx` unten = `build/extracted/re2_ems/CDEMD0_EM37_ai1.BIN` (23024 B, TOC-Sektor
0xE04 → EMS-Offset 0x00708000, gelinkt @0x80100000; Datei-Offset = Adresse − 0x80100000).
Alle `0x800xxxxx` = `info/re2leon/PSX.EXE` (t_addr 0x80010000, Datei 0x800). Modell:
`CDEMD0_EM37_emd.EMD` (59376 B, Sektor 0xE2D), Textur `CDEMD0_EM37_tim.TIM` (33312 B).
Werkzeug: `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` (+ lokal erweiterte
lwl/lwr/swl/swr-Dekodierung — der mitgelieferte Disassembler kennt op 0x22/0x26/0x2A/0x2E nicht
und zeigt dort `.word`; ALLE Anker-Tabellen des EM037 werden per lwl/lwr gelesen).

---

## 0. Kurzantwort auf die Nutzer-Meldung

Die vier Tentakel sind **eigene Entities vom Typ 0x37 mit eigenem 23-KB-Overlay, eigener
24-Clip-Bank und eigenem 4-Bone-Rig**. Sie haben **HP = −1** (@0x80100530-34, unverwundbar), sie
**bewegen sich nie selbst** (die ganze Bank traegt NULL Root-Motion, §2.1), und ihre Position
wird **jeden Frame aus G5s Part-2-Weltmatrix plus einem festen Anker-Offset** berechnet
(@0x80100230-388, Ankertabelle @0x80105668). Sie sind also nicht „Gegner im Raum", sondern
**vier angebaute Gliedmassen**, die G5 per Routine-Wort in deren `+0x04` fernsteuert.
Deshalb gibt es im RE1.5-ROOM5090 heute nichts zu sehen: der Port hat die Aktoren nie gespawnt
und `g5_tentakel_cmd()` ist ein No-Op (`re15_port/engine/src/enemy_ai_boss_g5.c:86`).

---

## 1. Overlay-Architektur — alle Tabellen (Vollzensus)

Kopf: Wort0 `0x0000001B` @0x80100000, danach Zeiger-/Datenbereich 0x80100004..0x80100177,
Code ab **0x80100178** (Per-Frame-Main).

| Tabelle | Adresse | Eintraege | Index | Inhalt |
|---|---|---|---|---|
| Clip-Byte je Tentakel (sub8) | 0x80100048 | 4 Byte | `+0x218` | `0E 0E 10 0F` = Clip 14/14/16/15 |
| sub3-Phasen [P0..P4] | 0x80100034 | 5 | `+0x06` | §3.3 |
| sub8-Phasen [P0..P9] | 0x8010004C | 10 | `+0x06` | §3.8 |
| sub9-Phasen [P0..P9] | 0x80100074 | 10 | `+0x06` | §3.9 |
| sub10-Phasen [P0..P5] | 0x8010009C | 6 | `+0x06` | §3.10 |
| sub11-Phasen [P0..P10] | 0x801000B4 | 11 | `+0x06` | §3.11 |
| sub13-Phasen [P0..P5] | 0x801000FC | 6 | `+0x06` | §3.13 |
| sub14-Phasen [P0..P7] | 0x80100114 | 8 | `+0x06` | §3.14 |
| **Anker-Offsets** | **0x80105668** | 4 × 8 B | `+0x218` | §4.1 |
| sub3-RotX je Tentakel | 0x80105738 | 4 × u16 | `+0x218` | 896 / 1088 / 2976 / 3200 |
| Rot-Paar-Tabelle (sub9/sub10) | 0x80105740 | 4 × 12 B | `+0x218` | §3.9 |
| sub3-Fensterbytes | 0x80105730 | 2 × 4 B | `+0x16B` | `30,55,60,70` / `38,50,52,60` |
| **Routine-Tabelle** | **0x80105688** | 8 | `+0x04` (Byte) | [0] Ctor 0x801004F0 · [1] AI 0x801008F4 · [2] 0x80104184 · [3] 0x801041E4 · [4] 0x8010423C · [5]/[6] NULL · [7] 0x80104280 |
| Varianten-Tabelle | 0x801056A8 | 2 | `+0x10E & 0x3F` | beide 0x80100934 |
| **Sub-Tabelle A (Stubs)** | **0x801056B0** | 16 | `+0x05` | alle 16 sind `jr ra; nop` — nachgeprueft |
| **Sub-Tabelle B (Koerper)** | **0x801056F0** | 16 | `+0x05` | B[i] = A[i] + 8 |
| sub11-Rot-Tabelle | 0x80105770 | 16 × 8 B | `+0x218 + (rng&3)*4` | §3.11 |
| sub13-Clip/Rot | 0x801057F0 | 4 × 6 B | `+0x218` | Clip 23/21/21/23, RotZ −128/−256/−256/−128 |
| Routine-4-Tabelle | 0x8010582C | 5 | `+0x05` | Script-Hook |

Dispatch-Kette je Frame (Main @0x80100178):
`+0x04` → Routine-Tabelle @0x80105688 (@0x8010038C-B0, Argumente `(e, e[+0x108] EMR-Pool,
e[+0x17C] EDD)`) → Routine 1 @0x801008F4 → Varianten-Tabelle @0x801056A8 mit `+0x10E & 0x3F`
(@0x801008FC-1C) → 0x80100934 → **zwei** Dispatches auf `+0x05`: Phase A @0x801056B0
(@0x80100950-6C) und Phase B @0x801056F0 (@0x80100978-94). **A ist bei allen 16 Eintraegen ein
reiner `jr ra`-Stub** (programmatisch nachgeprueft) — es zaehlt nur die B-Tabelle.

**Sub-Zustaende (`+0x05`), B-Tabelle @0x801056F0:**

| sub | Koerper | Laenge | Rolle | G5-Kommando |
|---|---|---|---|---|
| 0 | 0x80100A44 | 0x78 | IDLE (versteckt, wartet) | Wort `1` (Rueckkehr) |
| 1 | 0x80100ABC | 0x26C | (nie kommandiert) Clip 9 + GTE-Zielen | — |
| 2 | 0x80100D28 | 0x68C | (nie kommandiert) Clip 1/9-Stoss mit Raumgrenzen | — |
| **3** | **0x801013B4** | 0x48C | **PEITSCHE (Clip 7/8) + GRIFF, 15 Schaden** | **0x301** |
| 4 | 0x80101840 | 0x1CC | HALTEN des gegriffenen Spielers (Clips 9..12) | (aus sub5/sub7) |
| 5 | 0x80101A0C | 0x174 | (nie kommandiert) Clip 9 → Wort 0x401 | — |
| **6** | **0x80101B80** | 0x94 | **EINZIEHEN → IDLE** | **0x601** |
| 7 | 0x80101C14 | 0xB8 | Clip 7, zielt auf den Spieler → Wort 0x401 | — |
| **8** | **0x80101CCC** | 0x380 | **AUSTRITT / BEBEN / EINZUG (Intro)** | Ctor-Start, `+0x06` 1/6/7/8/9 |
| **9** | **0x8010204C** | 0x6D0 | **SPEER: aufrichten, suchen, Raumgrenzen** | **0x901** |
| **10** | **0x8010271C** | 0x31C | **WEDELN (Clips 9..11) + Schau-Peitsche** | **0xA01**, **0x40A01** |
| **11** | **0x80102A38** | 0x8F0 | **ZUG-TENTAKEL: Griff 15 Schaden + Laengen-Skalierung** | **0xB01 / 0x70B01 / 0x90B01** |
| 12 | 0x80103328 | 0x1C4 | (nie kommandiert) Clip 22 | — |
| **13** | **0x801034EC** | 0x43C | **SPIESS (Clip 21/23) + GRIFF, 15 / 10 Schaden** | **0xD01** |
| **14** | **0x80103928** | 0x72C | **TOD (Clips 17..20, Blut, Ausbleichen)** | **0xE01** |
| 15 | 0x80104054 | 0x138 | Vorkampf-Leerlauf (Clip 13), nur Variante 1 | Ctor (`+0x10E`=1) |

Die Wort-Semantik ist identisch zu EM036: `*(u32*)(e+0x04) = wort` schreibt in einem Rutsch
`+0x04` (Routine), `+0x05` (Sub), `+0x06` (Phase), `+0x07`. Also
`0xB01` = Routine 1 / sub 11 / ph 0, `0x70B01` = sub 11 / **ph 7**, `0x90B01` = sub 11 / **ph 9**,
`0x40A01` = sub 10 / **ph 4**, `0x301` = sub 3 / ph 0, `0xD01` = sub 13 / ph 0.

**Routinen 2 und 3 sind TOTER CODE:** ihr Dispatch `*(0x801057E4 + (+0x05)*36 + (+0x1D2)*4)`
bzw. `*(0x80105808 + …)` (@0x80104190-B4 / @0x801041F0-14) zeigt in die **Rotations-Datentabelle**
0x80105770..0x801057F0 — dort stehen keine Funktionszeiger. Erreichbar waeren sie nur ueber den
EXE-Trefferpfad, und der laeuft nie: der Konstruktor setzt **HP `+0x156` = −1** (@0x80100530-34)
und `+0x151/152/153 = −126` (@0x80100538-44). **Tentakel sind unverwundbar.**

### 1.1 Der Konstruktor (Routine 0, 0x801004F0) — Reihenfolge

| Adresse | Wirkung |
|---|---|
| @0x8010052C | `+0x04 = 1` (Routine 1) |
| @0x80100530-44 | **`+0x156 = −1` (HP)**, `+0x151/+0x152/+0x153 = −126` |
| @0x80100554-84 | `+0x1D0 = +0x1CC = +0x1C8 = 0`, `+0x1D3 = 0`, `+0x1C0 = 0`, **`+0x1C1 = 3`** (Kollisionssegmente), `+0x219 = 0`, `+0x220 = 0`, `+0x222 = 0`, `+0x223/+0x224 = 0`, `+0x1E8 = 0` |
| @0x80100584-6C4 | Hitbox-/Reichweitenfelder `+0x90..+0xFE` (Standard ±500/1200/600/700; bei `+0x10E == 8` Sonderwerte 800/600 @0x80100674-C4) |
| @0x80100628 | `FUN_80016480(e+0x16C, 0,0,0, parts+92)` — Zweck OFFEN |
| **@0x80100630-64** | **Index-Bestimmung:** vergleicht `e` gegen `*(0x800CFE20)`, dann in einer Schleife gegen `*(0x800CFE24/28/2C/30/…)`; **`+0x218` = Trefferposition** |
| @0x8010066C-C4 | `+0x10E == 8` (Kind, §6) → `+0x1E8 = 4`, andere Hitboxen, Sprung ans Ende |
| @0x801006D8-E0 | `+0x1EE = 800`, `+0x14C = 0`, erster `FUN_8002959C(e,pool,edd,256)` |
| @0x801006E4-718 | **`0x800CE300[e+0x08] = 0x80104288`**, **`0x800CE400[e+0x08] = 0x80104B68`** (EXE-Callbacks je Slot) |
| @0x8010071C-8C | `e->flags \|= 0xC`; Part-Skalen `(0, 4096, 4096)` fuer alle 4 Parts (`+0x8C/+0x8E/+0x90`, Stride 172) — **`scaleX` startet auf 0** |
| @0x80100790-B4 | falls `e[+0x3C] == 0`: `0x800CE32C = 0x80104C84(e, 0x800CE32C)` (Effekt-Registrierung) |
| @0x801007B8-CC | `e[+0x208] = 0x801004B4` (2. Per-Frame-Callback); **wenn `(+0x10E & 0xFFF) != 0` → Platzierung ueberspringen** |
| @0x801007D0-87C | **Platzierung** aus Anker + G5-Matrix (§4.2) |
| **@0x8010088C-AC** | **`+0x05 = 8`** (Start in sub8); **`0x801050BC(52, e)` spawnt das KIND**, `kind->flags = 0`, `kind[+0x10E] = 8`, Rekursion in `0x801004F0` |
| @0x801008B0-CC | wenn `(+0x10E & 0xFFF) == 1` → **`+0x05 = 15`, `+0x219 = 1`** |

---

## 2. Das Modell (EMD) — Verzeichnis, Rig, Clips

`CDEMD0_EM37_emd.EMD`, Verzeichnis @0xE7D0, `dir_count = 8` (RE2-Konvention, die
`re2_emd_parse_bank` verlangt — passt).

| dir | Offset | Bedeutung | Inhalt |
|---|---|---|---|
| 0 | 0x00000008 | — | Groessen-/Pad-Wort |
| 1 | 0x0000000C | **EDD Paar 1** | Kopf `{1, 96}` → **24 Clips** |
| 2 | 0x0000134C | **EMR Paar 1 (Struktur)** | `{bones_tab=32, kf_ofs=52, bones=4, kf_size=32}` |
| 3 | 0x0000AA60 | EDD Paar 2 | Kopf `{0,0}` → **0 Clips** (leer) |
| 4 | 0x0000AA64 | EMR Paar 2 | `{0, 0, 24, 24}` (Stub) |
| 5 | 0x0000AA68 | **EDD Paar 3** | Kopf `{24,24}` → **6 Clips** (Opfer-Anims) |
| 6 | 0x0000ACCC | EMR Paar 3 | `{0, 8, **15 Bones**, kf_size 80}` = PL0-Spieler-Rig |
| 7 | 0x0000D984 | **MD1** | `length=0xA20`, `nObj=8` → **4 Meshes** |

**Rig (Paar 1, das Tentakel selbst):** 4 Bones, Kette **0 → 1 → 2 → 3** (Kind-Bytes
@EMD 0x137C `01 02 03`, Bone-Header @EMD 0x136C `{1,0x10}{1,0x11}{1,0x12}{0,0x13}`),
Relativ-Positionen @EMD 0x1354 (3 × s16 je Bone):

| Bone | rel. Position | Bedeutung |
|---|---|---|
| 0 | (0, 0, 0) | Wurzel (an G5s Blob) |
| 1 | (**3750**, 0, 0) | 1. Glied |
| 2 | (**3003**, 0, 0) | 2. Glied |
| 3 | (**1508**, 0, 0) | Spitze |

Gesamtlaenge 8261 Einheiten entlang **+X**; die Reichweiten-Sonde legt noch **1500** drauf
(Vektor `(1500,0,0)` durch die Part-3-Matrix, @0x80102C7C / @0x8010226C) → **Reichweite 9761**.

**Keyframe-Pool** @EMD 0x1380 (EMR+52), 32 B je Keyframe, (0xAA60 − 0x1380)/32 = **1207
Keyframes** (hoechster benutzter Index 1206 — exakt aufgehend). Layout wie RE1.5: `+0/+2/+4`
Translation, `+6/+8/+10` Root-Motion-Spur, ab `+12` die 4 × 3 gepackten 12-Bit-Winkel
(12 + 18 = 30 → 32).

**MD1:** 4 Meshes (`nObj = 8`, Port-Regel `mesh = nObj/2`), Kopf @EMD 0xD984:
Mesh 0 = 66 Vertices / 71 Normalen / **5 Dreiecke + 60 Quads** (der ganze sichtbare Strang),
Meshes 1..3 = je 4 Vertices / 1 Quad (Stummel). Der Strang haengt also an **Bone 0** und wird
ueber `part0.scaleX` (Feld `+0x8C` = 140) gestreckt; die Bones 1..3 fuehren die
Kollisions-Stuetzpunkte und die Spitzen-Sonde. **Bindung fuer den Port: `mesh_idx == bone_idx`**
— genau das, was `re15_md1_parse` + beide Renderer ohnehin tun.

**Textur:** TIM `flag = 0x9` → 8 bpp + CLUT; Bild-Block 64 × 256 VRAM-Halbwoerter @(0,0) =
**128 × 256 Pixel**, CLUT 256 × 1 @(0,480).

### 2.1 Clip-Katalog (EDD Paar 1) mit Frame-Flag-SEs

Anders als EM036 (dort sind alle 1360 Frame-Woerter flaggenlos) **traegt die EM037-Bank
Frame-Flags**: Bit `0x08000000` im Frame-Wort → `FUN_80016028` spielt `SE(wort >> 28)`
(@0x80016040-54). Alle Werte selbst aus dem EMD geparst.

| Clip | Frames | Keyframes | Root-Motion | Frame-Flag-SEs (Frame → SE) | Verwendung |
|---|---|---|---|---|---|
| 0 | 1 | 0 | 0 | — | Ruhe-/Nullpose (IDLE; sub0/3/9/10/11/13/14 ph0) |
| 1..6 | je 1 | 1..6 | 0 | — | Standposen (nur sub2 nutzt Clip 1) |
| **7** | **100** | 7..106 | 0 | **32→SE4, 62→SE15** | **sub3-Peitsche Var. 0; sub7; sub10 ph4** |
| **8** | **100** | 107..206 | 0 | **38→SE4, 51→SE15** | **sub3-Peitsche Var. 1; sub10 ph4** |
| 9 | 100 | 207..306 | 0 | — | sub4/sub10/sub14-Zappeln |
| 10 | 140 | 307..446 | 0 | — | sub4/sub10-Zappeln |
| 11 | 120 | 447..566 | 0 | — | sub4/sub10-Zappeln |
| 12 | 60 | 567..626 | 0 | — | sub4-Zappeln |
| **13** | **120** | 627..746 | 0 | 3→SE15, 30→SE4, 43→SE4, 55→SE5, 72→SE5, 108→SE4 | **sub15 (Variante 1, Vorkampf)** |
| **14** | **30** | 747..776 | 0 | 23→SE4 | **Austritt Tentakel 0 und 1** |
| **15** | **30** | 777..806 | 0 | 23→SE4 | **Austritt Tentakel 3** |
| **16** | **30** | 807..836 | 0 | 21→SE4 | **Austritt Tentakel 2** |
| 17 | 50 | 837..886 | 0 | 11→SE4, 31→SE4, 37→SE5 | Tod A |
| 18 | 50 | 887..936 | 0 | 22→SE4, 35→SE5 | Tod B |
| 19 | 50 | 937..986 | **rootY −382..−229** | 3→SE5, 16→SE5 | Tod C (sackt ab) |
| 20 | 40 | 987..1026 | **rootY −266..−79** | 7→SE5, 17→SE5 | Tod D (sackt ab) |
| **21** | **75** | 1027..1101 | 0 | **27→SE15, 35→SE1** | **sub13-Spiess, Tentakel 1 und 2** |
| 22 | 30 | 1102..1131 | 0 | — | sub12 |
| **23** | **75** | 1132..1206 | 0 | **25→SE15, 31→SE1** | **sub13-Spiess, Tentakel 0 und 3** |

**Root-Motion ist ueber ALLE 24 Clips exakt 0** (Spur `+6/+8/+10` konstant, Translation
`+0/+2/+4` konstant; nur Clips 19/20 bewegen rootY). Der Tentakel bewegt sich **ausschliesslich**
ueber Anker + Rotation + `scaleX` — im Port darf also **kein** `FUN_80015E7C`-Pfad angehaengt
werden.

**Clip-Wort:** `*(u32*)(e+0x14C)`. `FUN_8002959C` liest `+0x14C` (Byte) = **Clip-Index**,
`+0x14D` (Byte) = **aktueller Frame** (@0x800295A8 / @0x800295D0), `+0x14E` = Ueberblend-Byte.
Ein Schreiben von z. B. `0x001F0007` setzt also Clip 7, Frame 0, Blend 0x1F. Benutzte
Blend-Werte im EM037: **0x00, 0x07, 0x0F, 0x1F, 0x3F, 0x7F**. Ein `a3` ≠ 0 im HIGH-Halbwort
bedeutet Rueckwaerts-Abspielen (@0x800295C0-C4) — EM037 nutzt das nie (a3 ∈ {16,32,64,128,256,512}).

### 2.2 Opfer-Anims (Paar 3) — die Griff-Sequenzen

6 Clips à 24/16/10/25/21/50 Frames auf **15 Bones, kf_size 80** = das PL0-Spieler-Rig.
Das sind die Leon-Animationen zu den sechs Griff-Modi, die EM037 in `0x800CFBFC` schreibt
(§3.3/§3.11/§3.13). Im Port nicht auf das RE1.5-Rig uebertragbar → **DEFERRED**, wie beim
G5-Devour.

---

## 3. Zustandsmaschine — Kommando → Zustand → Clip → Frames → Bewegung → Schaden → SE

Gemeinsame Felder: `+0x218` = **Tentakel-Index 0..3** (§1.1), `+0x219` = **Anker-Modus**,
`+0x158`/`+0x15A` = Timer, `+0x16A`/`+0x16B` = Variantenbyte, `+0x220` = Treffer-/Zustandsbits,
`+0x222` = Ruettel-Countdown, `+0x74/76/78` = Rotation X / Y(Yaw) / Z,
`parts+0x8C` = `scaleX`, `parts + i*172 + 104/106/108` = Lokalwinkel je Part,
`parts + i*172 + 24` = lokale Matrix, `parts + i*172 + 72` = Weltmatrix (Translation +92/96/100).

### 3.0 sub0 — IDLE (0x80100A44), Rueckkehrziel aller Zustaende (Wort `1`)

`+0x06 != 0` → nichts. Sonst (@0x80100A5C-9C): Clip-Wort **0x003F0000** (Clip 0, Blend 0x3F),
`+0x06 = 1`, `+0x1C0 &= ~2`, **Timer `+0x158` = 60 + (rng & 0x3F)**, `parts.flags &= ~1`
(Modell aus). Danach passiv — G5 kommandiert den naechsten Zustand.

### 3.3 sub3 — PEITSCHE (Kommando **0x301**), Phasen @0x80100034

| ph | Adresse | Aktion |
|---|---|---|
| 0 | 0x80101404 | Clip 0; `+0x06=1`; `+0x1C0 &= ~2`; `parts.flags \|= 1`; **RotZ `+0x78` = −576**, Yaw = 0, **RotX `+0x74` = u16[idx] @0x80105738 = 896/1088/2976/3200**; **`G5[+0x228] \|= 1<<(idx+4)`** @0x8010146C-7C; **SE 0** @0x80101478 |
| 1 | 0x80101480 | Anim (Blend 64); `scaleX += 64`; ab `scaleX ≥ 2025` `+0x78 += 32` (Deckel +32) @0x801014D8-514; **`scaleX ≥ 3073` → `+0x06 = 2`, Clip-Wort `0x001F0007 + (rng&1)` → Clip 7 oder 8, `+0x16B = rng&1`** @0x801014B8-D4 |
| 2 | 0x80101518 | Anim (128); `scaleX += 64`; **`≥ 4097` → `scaleX = 4096`, `+0x06 = 3`, `+0x158 = (rng&0xF)+10`** @0x80101544-68; dann Fensterlogik |
| 3 | 0x801015C4 | Anim (128); **Clip-Ende → `+0x06 = 4`, Clip-Wort `0x001F0000`** @0x801015D8-E8; dann Fensterlogik |
| 4 | 0x801017BC | Anim (128); **`scaleX -= 64`; < 0 → `scaleX = 0`, Routine-Wort = 1 (sub0), `G5[+0x228] &= ~(1<<(idx+4))`** @0x801017D0-814 |

**Fensterlogik (ph2/ph3, Tabelle `T = 0x80105730 + (+0x16B)*4`):**

* `T[0] < Frame < T[1]` → `+0x78 += 8` (Ausholen) @0x8010156C-C0 / @0x801015EC-3C
* `T[2] < Frame < T[3]` → `+0x78 += 4` **und TREFFER-FENSTER** @0x80101640-94
* **Bytes:** Variante 0 (Clip 7) `T = {30, 55, 60, 70}`; Variante 1 (Clip 8) `T = {38, 50, 52, 60}`

**Treffer (@0x80101684-94):** verlangt `+0x220 & 4` (Kollisionsbit vom Kind, §6) **UND**
`*(0x800CFDCB) == 0` (Spieler nicht gegriffen) **UND** `G5[+0x156] >= 0` (G5 lebt,
@0x801016B4-C8). Dann:

* **`FUN_800401D4(15, 0)` = 15 Schaden** @0x801016D0
* Rueckgabe 2 → `0x800CFBFC = 3` @0x801016E4-EC; sonst
  `0x800CFBFC = (!FACING(e, Spieler) << 8) + 5` (= 5 oder 0x105) @0x801016F8-718
* **falls Distanz `+0x1F0` ≥ 6001** → `0x800CFBFC = 0x205` **und Kamera-Yaw
  `0x800CFC6E = 3456`, bei `idx < 2` stattdessen `640`** @0x8010171C-54
* `0x800CFDAC = e`, `0x800CFDCB |= 0x80`, `0x800CFD80/84 = e[+0x188]/[+0x18C]` @0x80101758-8C
* **SE 3** @0x80101790, `FUN_8003947C(3,0)` @0x8010179C, `FUN_80039514(10,250,0)` @0x801017AC

### 3.6 sub6 — EINZIEHEN (Kommando **0x601**), 0x80101B80

ph0: Clip-Wort **0x001F0000**, `+0x06 = 1` (@0x80101BB4-C0). ph1: Anim (128);
**`scaleX -= 64`; < 0 → `scaleX = 0`, Routine-Wort = 1** (@0x80101BD0-F0). Keine SE, kein
Schaden. Das ist das „alle Tentakel weg"-Signal am Ende von G5s Intro
([T13] Broadcast 0x601, EM036 @0x80101790-94).

### 3.8 sub8 — AUSTRITT / BEBEN / EINZUG (Ctor-Start, G5 treibt `+0x06`), Phasen @0x8010004C

| ph | Adresse | Aktion |
|---|---|---|
| 0 | 0x80101D38 | **Clip = Byte[idx] @0x80100048 → 14 / 14 / 16 / 15**; `parts.flags \|= 1`; **falls idx == 1: `parts.flags \|= 0x4000`, `+0x219 = 2`** @0x80101D60-78; Rotation 0/0/0; **`scaleX = 0`**. *Keine* Selbst-Weiterschaltung — G5 schreibt `+0x06`. |
| 1 | 0x80101D90 | **SE 0**; `+0x06 = 2`; **`+0x219 = 1`** (frei, nicht mehr angeankert) |
| 2 | 0x80101DAC | Anim (256); `+0x06 += Clip-Ende`; **`scaleX += 256`, Deckel 4096** (16 Frames vom Boden hoch) |
| 3 | 0x80101DFC | Clip-Wort **0x00070000** (Clip 0, weicher Blend 7); `+0x158 = 0`; `+0x06 = 4`; **SE 2**; **`+0x222 = 7`** (Ruettelsalve, §6) |
| 4 | 0x80101E24 | `t < 5`: `X -= t*10`; `6 ≤ t ≤ 34`: `X += 10`; `t == 36` → `+0x06 = 5`; `t++` |
| 5 | 0x80101E8C | `+0x158 = 0` — Warten (G5 treibt weiter) |
| 6 | 0x80101E94 | `t < 20`: `X -= t`; **`16 ≤ t ≤ 179`: Zittern `X/Y/Z += (rng&0xF) * (1 − 2*(t&1))`** @0x80101ED0-F58; `t ≥ 161`: `X += 5`; `t == 180` → `+0x06 = 7`; `t++` |
| 7 | — | leer (warten) |
| 8 | 0x80101FA8 | Anim (512); **`scaleX -= 64`**; `idx < 2` → Yaw `−16`, sonst `+16` @0x80101FCC-F8; **`scaleX < 2048` → `scaleX = 0`, SE 0, `+0x06 = 9`** @0x80101FFC-020 |
| 9 | — | leer (verborgen) |

### 3.9 sub9 — SPEER / SUCHEN (Kommando **0x901**), Phasen @0x80100074

Rot-Paar-Tabelle `R = 0x80105740 + idx*12`; sub9 nutzt den **B-Satz** `R[+6]/R[+8]/R[+10]`,
sub10 den **A-Satz** `R[+0]/R[+2]/R[+4]`:

| idx | A-Satz RotX/Yaw/RotZ (sub10) | B-Satz RotX/Yaw/RotZ (sub9) |
|---|---|---|
| 0 | 768 / 0 / 32 | **2304 / 0 / 128** |
| 1 | 1536 / 0 / 32 | **2816 / 0 / 128** |
| 2 | 2560 / 0 / 32 | **64 / 0 / −64** |
| 3 | 3584 / 0 / 32 | **1920 / 0 / 128** |

| ph | Adresse | Aktion |
|---|---|---|
| 0 | 0x8010209C | Clip 0; Rotation = B-Satz @0x801020A4-120; `+0x15A = 64`; **`+0x219 = 2`** (halber Anker); `+0x16A = 0`; `+0x158 = 0`; `+0x06 = 1`; `scaleX = 0`; `parts.flags \|= 1`; **`+0x1E8 = 4`**; **SE 0** @0x8010213C |
| 1 | 0x80102144 | Anim (64); `scaleX += +0x158`, `+0x158 += 8`; **`scaleX ≥ 3073` → `+0x06 = 2`, Clip-Wort `0x001F0002`** @0x80102188-9C; `+0x78 -= +0x15A`; `+0x15A -= 4` |
| 2 | 0x801021C0 | Anim (128); `scaleX += +0x158`, `+0x158 += 16`; **`≥ 4097` → `scaleX = 4096`, SE 15, `+0x06 = 3`** @0x801021F8-220; faellt in ph3 |
| 3 | 0x80102224 | `+0x78 -= +0x15A` (bei ph3 zusaetzlich `+0x15A -= 4`); `FUN_80104BA4` (Kette neu); **Spitzensonde** `(1500,0,0)` durch Part-3-Matrix + Part-3-Weltposition @0x80102258-2A8 und **Raumgrenzen** (§3.G) → sonst `+0x06 = 4` |
| 4 | 0x80102344 | `+0x15A = 64`; `+0x158 = 60`; `+0x06 = 5`; `+0x16A = 1`; **SE 2**; `+0x219 = 1`; `+0x222 = 7` @0x80102344-78 |
| 5 | 0x8010237C | Countdown `+0x158`; bei 0 → `+0x06 = 6`, Clip-Wort `0x003F0000`, `+0x158 = 8`, **`+0x21A = Part3.WeltX − Part0.WeltX`, `+0x21C = scaleX`, `+0x21E = Part0.WeltX`** @0x801023A8-D0; jeden Frame **Einroll-Rotation**: `part0.ang += +0x15A`, `part1/2.ang -= +0x15A`, `part3.ang -= +0x15A/2`, `+0x15A -= 2*(+0x16A)` @0x801023D4-48; danach `FUN_80104C30`; **Gesamtwinkel-Klemme**: `(RotZ + Σ part.ang) & 0xFFF ∈ [2049, 4095]` → Rotation zuruecknehmen, `+0x15A = 0`, `+0x16A = 0` @0x80102458-FC |
| 6 | 0x80102500 | `+0x158 = 0` (warten) |
| 7 | 0x80102508 | Ausrollen ueber `+0x158` (part0 −2t, part1/2 +2t, part3 +t) |
| 8 | — | leer |
| 9 | 0x80102690 | Abschluss |

### 3.10 sub10 — WEDELN (Kommando **0xA01**; **0x40A01** = direkt ph4), Phasen @0x8010009C

| ph | Adresse | Aktion |
|---|---|---|
| 0 | 0x8010276C | Clip 0; `+0x06 = 1`; `parts.flags \|= 1`; **Rotation = A-Satz** @0x8010278C-F8; **`+0x219 = 0`** (voller Anker); **SE 0** @0x801027F8 |
| 1 | 0x80102800 | Anim (64); `scaleX += 64`; **`≥ 2049` → `+0x06 = 2`, Clip-Wort `0x001F0009 + (idx % 3)` → Clip 9/10/11** @0x80102838-74 |
| 2 | 0x80102878 | Anim (128); `scaleX += 64`; **`≥ 4097` → `scaleX = 4096`, `+0x06 = 3`** @0x801028A4-BC |
| 3 | 0x801028C0 | Anim (64); `+0x158--`; bei 0 → **neuer Zufallsclip `9 + (rng%3)`, Startframe `rng%90`, Blend 0x3F**, `+0x158 = (rng&0xF)+60` @0x801028E8-7C |
| **4** | **0x80102980** | **`+0x06 = 5`; Clip-Wort `0x001F0007 + (rng&1)` → Peitschenclip 7/8** — aber **ohne** Trefferfenster |
| 5 | 0x801029A0 | Anim (128); Clip-Ende → `+0x06 = 3` + neuer Zufallsclip @0x801029B4-DC |

**Kein Schaden in sub10** — es ist die Schau-Choreografie des Intros.

### 3.11 sub11 — ZUG-TENTAKEL (Kommandos **0xB01** / **0x70B01** = ph7 / **0x90B01** = ph9)

Das ist der Zustand, den der Nutzer als „Tentakel, die ihn nach vorne ziehen und schlagen"
beschreibt. G5 sendet `0xB01` als **Broadcast an alle vier** (EM036 @0x80100DD8/@0x80100DE0) und
danach gestaffelt `0x70B01`/`0x90B01` an Reihenpaare. Phasen @0x801000B4:

| ph | Adresse | Aktion |
|---|---|---|
| 0 | 0x80102A88 | Clip 0; **Rotation = `0x80105770[idx + (rng&3)*4]`** (16 Saetze) @0x80102A88-B0C; `+0x15A = 64`; `+0x16A = 0`; `+0x158 = 0`; **`+0x219 = 0`** (voller Anker); `+0x06 = 1`; `scaleX = 0`; `parts.flags \|= 1`; **SE 0** @0x80102B20; **`G5[+0x228] &= ~(1<<(idx+4))`** (bleibt „frei") @0x80102B28-4C |
| 1 | 0x80102B50 | Anim (64); `scaleX += +0x158`, `+0x158 += 8`; **`≥ 3073` → `+0x06 = 2`, Clip-Wort `0x001F0002`** @0x80102B88-A8 |
| 2 | 0x80102BCC | Anim (128); `scaleX += +0x158`, `+0x158 += 16`; **`≥ 4097` → SE 15, `+0x06 = 3`** @0x80102C0C-2C |
| **3** | **0x80102C30** | Einroll-Rotation wie sub9 ph5; **Spitzensonde + Raumgrenzen** (§3.G) → `+0x06 = 4` @0x80102C60-D4C; **GRIFF-FENSTER** (unten) |
| 4 | 0x80102DF4 | `+0x15A = 64`; `+0x158 = 60`; `+0x06 = 5`; **SE 2** @0x80102E18; `+0x16A = 1`; `+0x222 = 7`; `+0x223`-Bit |
| 5 | 0x80102E2C | Countdown; bei 0 → `+0x06 = 6`, Clip-Wort `0x003F0000`, `+0x158 = 8`, **`+0x21A = X + 8192`, `+0x21C = scaleX`** @0x80102E40-6C; Einroll-Rotation + Gesamtwinkel-Klemme wie sub9 |
| **6** | **0x80102F9C** | `+0x220 \|= 1`; `parts.flags = (flags & ~0x400) \| 0x2000`; `part1.flags &= ~0x800`; **`scaleX = +0x21C · (+0x21A − X) / 8192`** @0x80102FD0-3004 — *der Strang verkuerzt sich, waehrend G5 auf den Ankerpunkt zukriecht* |
| **7** | **0x80103010** | `+0x158 < 10`: part0 −2t / part1,2 +2t / part3 +t @0x80103024-74; **`6 ≤ +0x158 ≤ 79`: Zittern X/Y/Z ±(rng&0xF)** @0x8010307C-11C; `+0x158 ≥ 81`: part0 +4 / part1,2 −4 / part3 −2 @0x80103120-6C; **`+0x158 == 90` → `+0x06 = 8`** @0x80103170-80; jeden Frame die Laengenformel aus ph6; `+0x158++` |
| 8 | 0x801031CC | `+0x16A = 0`; Clip-Wort `0x003F0000`; Laengenformel — Halten |
| **9** | **0x80103210** | `+0x78 -= 1`; **`scaleX += 128` bis `+0x21C`** erreicht → `parts.flags = (flags\|0x400) & ~0x2000`, `part1.flags \|= 0x800`, `+0x06 = 10`, `+0x220 &= ~1`, **SE 0** @0x80103240-88 |
| 10 | 0x801032A8 | Anim (64); `scaleX -= +0x158`, `+0x158++`, `+0x78--`; **`scaleX < 0` → `scaleX = 0`, Routine-Wort = 1** @0x801032E8-FC |

**GRIFF-FENSTER (ph3, @0x80102D4C-DEC):** `+0x220 & 2` **UND** `0x800CFDCB == 0` →

* **`FUN_800401D4(15, 1)` = 15 Schaden, Typ 1** @0x80102D78
* `0x800CFBFC = (!FACING(e, Spieler) << 8) + 5` (5 oder 0x105) @0x80102D84-DC8
* `0x800CFDAC = e`, `0x800CFDCB |= 0x80`, `0x800CFD80/84 = e[+0x188]/[+0x18C]`
* `FUN_8003947C(3, 0)` @0x80102DD4, `FUN_80039514(5, 250, 0)` @0x80102DE4

### 3.13 sub13 — SPIESS (Kommando **0xD01**), Phasen @0x801000FC

| ph | Adresse | Aktion |
|---|---|---|
| 0 | 0x8010353C | Clip 0; `+0x06 = 1`; `parts.flags \|= 1`; **RotZ = u16 @0x801057F2+idx*6 = −128/−256/−256/−128**; **`ARC(e, SpielerX@0x800CFC30, SpielerZ@0x800CFC38, 16)`** @0x80103594; RotX = 0; **`G5[+0x228] \|= 1<<(idx+4)`** @0x801035BC-CC; **SE 0** @0x801035C8; `e->flags \|= 0x00100000` |
| 1 | 0x801035E0 | `ARC(…, 16)` @0x801035F4; Anim (64); `scaleX += 64`; **`≥ 3573` → `+0x06 = 2`, Clip-Wort `0x001F0000 + s16 @0x801057F0+idx*6` → Clip 23 / 21 / 21 / 23** @0x80103628-60; `+0x15A = 0`; jeden Frame `+0x78 -= 8` |
| 2 | 0x80103678 | `scaleX += 64`; `≥ 4097` → `scaleX = 4096`, `+0x06 = 3` @0x80103690-A4; faellt in ph3 |
| **3** | **0x801036AC** | `+0x16B = 0`; **Frame < 21:** `+0x78 += +0x15A`, `+0x15A += 8`, Deckel `+0x78 ≤ 0` @0x801036AC-EC; **TREFFER-FENSTER Frames 26..32** (`(Frame − 26) < 7` @0x801036F4-FC) |
| 4 | 0x80103880 | `+0x06 = 5`; Clip-Wort `0x003F0000` |
| 5 | 0x80103890 | Anim (64); **`scaleX -= 128`; < 0 → `scaleX = 0`, Routine-Wort = 1, `G5[+0x228] &= ~(1<<(idx+4))`** @0x801038A4-EC, `e->flags &= ~0x00100000` |

**Trefferfenster (Frames 26..32), zwei Zweige:**

1. `+0x220 & 4` UND `0x800CFDCB == 0` → **`FUN_800401D4(15, 1)`** @0x80103730,
   `0x800CFBFC = 773 (0x305)` @0x80103760-68, Griff-Latch @0x8010373C-70, **SE 3** @0x80103774,
   `FUN_8003947C(3,0)` @0x80103788, `FUN_80039514(10,250,0)` @0x80103798, `+0x16B = 1`
2. `+0x220 & 4` UND `0x800CFDCB != 0` (Spieler bereits gegriffen) UND `+0x16B == 0` UND
   `0x800CFD4E ≥ 0` → **`FUN_800401D4(10, 1)` = 10 Schaden** @0x801037E8,
   `0x800CFBFC = 1029 (0x405)` @0x80103818-20, Griff-Uebernahme, **SE 3** @0x8010382C,
   gleiche Ruettler. *Ein Tentakel kann dem anderen (oder G5) den Spieler abnehmen.*

### 3.14 sub14 — TOD (Kommando **0xE01**), Phasen @0x80100114

| ph | Adresse | Aktion |
|---|---|---|
| 0 | 0x80103970 | Clip 0; `+0x06 = 1`; `parts.flags \|= 1`; Rotation 0/0/0; **`G5[+0x228] \|= 1<<(idx+4)`** @0x801039B0-BC (bleibt fuer immer gesetzt = nie wieder Kandidat); **`e->flags \|= 2` und `Kind->flags \|= 2`** (Kollision aus) @0x801039C0-F0 |
| 1 | 0x801039F4 | Anim (64); `scaleX += 256`; **`≥ 2049` → `+0x06 = 2`, Clip-Wort `0x001F0011 + (rng&3)` → Clip 17..20** @0x80103A1C-40; **jeden ungeraden Frame Blutpartikel** (`FUN_8001BF10`, Offsets `128 − rng`, Typ 16/17, Param `10096 + (rng&0xFF)*8`) @0x80103A44-B0 |
| 2 | 0x80103AB4 | Anim (128); `+0x06 += Clip-Ende`; `scaleX += 256`; `≥ 4097` → `+0x06 = 3`, Clip 17..20 neu @0x80103AEC-34 |
| 3 | 0x80103B94 | Anim (128); Clip-Ende → Clip `0x001F0011 + (rng&1)`; Timer `+0x158` → 0: `+0x06 = 4`, Clip-Wort **`0x003F0013 + (rng&1)` → Clip 19/20** (die beiden, die absacken) @0x80103BC8-F0 |
| 4 | 0x80103C10 | Zuckungen + Blut |
| 5 | 0x80103E50 | Anim (64); Clip-Ende → Clip 19/20 neu; Timer → 0: `+0x06 = 6`, Clip-Wort **`0x007F0009 + ((rng+rng) % 3)` → Clip 9..11, Blend 0x7F** @0x80103E98-EC; **SE 1** @0x80103E24 |
| 6 | 0x80103F98 | Anim (32); **`scaleX −= 6`, `scaleY −= 32`, `scaleZ += 16`** @0x80103FA8-D0; **Farbe `part0[+0x70]` ausbleichen**: rot −1 bis 32, gruen −0x100 bis 0, blau −0x10000 bis 0x30 @0x80103FC8-018; **`+0x14E == 64` → `+0x06 = 7`** @0x8010401C-2C |
| 7 | — | leer (Kadaver) |

### 3.G Gemeinsame RAUM-GRENZEN der Spitzensonde (sub2 / sub9 ph3 / sub11 ph3)

Spitze = `Part3.Weltposition + M(Part3) · (1500,0,0)`. Abbruch (`+0x06 = 4`, nur wenn
`+0x16A == 0`):

| Bedingung | Bedeutung | Belege (sub11 / sub9) |
|---|---|---|
| `tipY > 0` | unter dem Boden | @0x80102CB8 / @0x801022AC |
| `tipY < −5000` | ueber der Decke | @0x80102CDC / @0x801022D0 |
| `tipZ < −27000` | hinter der Nordwand | @0x80102D04 / @0x801022F8 |
| `tipZ ≥ −19799` | hinter der Suedwand | @0x80102D2C / @0x80102320 |

Das ist exakt der **Korridor um Z = −23400 ± 3600** aus room7040 — derselbe Streifen, in dem
EM036 seinen Wund-Spray auf `z ∈ [−26000, −20500]` klemmt (birkin-g5-ki.md §2).

---

## 4. Anker-Geometrie — wo die vier Tentakel stehen (Frage 3)

### 4.1 Die Ankertabelle @0x80105668 (4 × 8 B: s16 x, y, z, pad)

| idx | x | y | z | pad | Lage relativ zu G5s Blob |
|---|---|---|---|---|---|
| **0** | **965** | **−2243** | **−1933** | 0 | vorn, hoch, **links** |
| **1** | **1615** | **−706** | **−2440** | 0 | weiter vorn, tief, **links aussen** |
| **2** | **1409** | **−683** | **+2495** | 2048 | weiter vorn, tief, **rechts aussen** |
| **3** | **957** | **−2426** | **+1958** | 2048 | vorn, hoch, **rechts** |

Zwei Tentakel je Seite, je einer hoch und einer tief. Das `pad`-Halbwort (0/0/2048/2048) wird von
den beiden gefundenen Lesern **nicht** ausgewertet (es faellt in das Pad-Feld des SVECTOR) —
die Deutung „Spiegel-Yaw 180° fuer die rechte Seite" ist **plausibel, aber OFFEN**.

### 4.2 Einmalige Platzierung im Konstruktor (@0x801007B8-0x8010087C)

```
if ((e[+0x10E] & 0xFFF) != 0) → keine Platzierung            @0x801007C8
v = Anker[(3 + idx) - 3]   (Basis 0x80105650 + (3+idx)*8)    @0x801007D0-810  (lwl/lwr!)
M = RotMatrix(G5 + 0x74)                                     @0x80100818-24   (FUN_8008E1F4)
v = M · v                                                    @0x80100828-34   (FUN_8008DBA4)
e.X = G5.X + v.x ; e.Y = G5.Y + v.y ; e.Z = G5.Z + v.z       @0x80100838-7C
```

(`G5` = `*(0x800CFE1C)`, geladen ueber `s2 = 0x800CC1E8`, Offset `+15412`.)

### 4.3 Nachfuehrung JEDEN FRAME im Main (@0x801001E4-0x80100388)

```
if (e[+0x04] == 0)  → keine Nachfuehrung        (Konstruktor-Frame)   @0x801001E4-F0
switch (e[+0x219]) {                                                   @0x801001F4-22C
  case 0:  v = Anker[idx];                                             @0x80100230-88
  case 1:  keine Nachfuehrung (frei)
  case 2:  v = (Anker.x, Anker.y/2 − 1300, Anker.z/2);
           bei idx ∈ {1,2}: v.y = Anker.y/2 − 1500                      @0x80100290-33C
}
M   = G5.parts + 416          // Part 2 (Blob): 2*172 + 72
v   = M · v                                                            @0x80100348 (FUN_8008DBA4)
e.X = M.trans.x + v.x ; e.Y = … ; e.Z = …                              @0x80100350-88
```

**Der Anker haengt an G5s Part 2 — der MASSE, nicht am Kriecher.** Damit wandern alle vier
Tentakel exakt mit dem Blob mit, auch waehrend der Zugbewegung.

### 4.4 Was im RE2-Raum wirklich in der RDT steht (room7040)

`info/re2leon/PL0/RDT/ROOM7040.RDT`, Init-SCD @0x10FC (Offset-Tabelleneintrag 16), sub00 @0x10FE.
**Fuenf `Sce_em_set`-Records à 22 Byte** (RE2-Laenge; RE1.5 hat 20):

| Datei-Offset | Rohbytes | Slot | Typ | `+0x10E` | `pc[6]` | SE-Bank | Kill-Flag | X | Y | Z | Yaw |
|---|---|---|---|---|---|---|---|---|---|---|---|
| **0x11AC** | `44 00 00 36 00 80 00 27 00 ff 00 83 00 00 00 83 00 00 00 00 00 00` | 0 | **0x36** | 0 | 0 | **0x27** | 0xFF | **−32000** | 0 | **−32000** | 0 |
| **0x11C2** | `44 00 01 37 01 00 00 27 00 ff 00 83 00 00 00 83 00 00 00 00 00 00` | 1 | **0x37** | **1** | 0 | 0x27 | 0xFF | −32000 | 0 | −32000 | 0 |
| **0x11D8** | `44 00 02 37 00 00 00 27 00 ff 00 83 00 00 00 83 …` | 2 | 0x37 | 0 | 0 | 0x27 | 0xFF | −32000 | 0 | −32000 | 0 |
| **0x11EE** | `44 00 03 37 00 00 00 27 00 ff 00 83 00 00 00 83 …` | 3 | 0x37 | 0 | 0 | 0x27 | 0xFF | −32000 | 0 | −32000 | 0 |
| **0x1204** | `44 00 04 37 00 00 00 27 00 ff 00 83 00 00 00 83 …` | 4 | 0x37 | 0 | 0 | 0x27 | 0xFF | −32000 | 0 | −32000 | 0 |

**Feldbelegung byte-true aus dem RE2-Handler.** Opcode-Sprungtabelle **Basis 0x800A74C8**
(Index 0 = Opcode 0; verifiziert an den Eintraegen 0x800A74C8 → 0x800537E4 …), Eintrag 0x44 @
**0x800A75D8 → Handler 0x8005714C**:

| Record | Ziel | Beleg |
|---|---|---|
| `pc[2]` Slot | `*(0x800CFE1C + Slot*4) = Entity`, Entity `+0x0C = Slot+2` | @0x80057204-38 |
| `pc[3]` Typ | Entity `+0x08`; Overlay-Lader `FUN_8001B710` | @0x800571EC-F0 / @0x80057334-3C |
| **`pc[4..5]` u16** | **Entity `+0x10E`** (die Varianten-Zahl) | **@0x8005734C-54** |
| `pc[6]` | Entity `+0x106`; `+0x1C2 = −pc[6]·1800` (Etagenhoehe) | @0x80057388-90 / @0x800573A0-C0 |
| `pc[7]` | Entity `+0x1FA` = **ENEMSE-Bank** (Vergleich gegen `*(0x800D8CD0)`) | @0x80057274-80 |
| `pc[8]` / `pc[9]` | `+0x1CF` / `+0x1CE` (+ Kill-Flag-Gate `FUN_80077360`) | @0x80057370-78 / @0x800571C0-D8 |
| **`pc[10..11]` X** | `+0x44` (s16) und `+0x38` (s32) | @0x800572C8-DC |
| **`pc[12..13]` Y** | `+0x46`, `+0x3C` | @0x800572E0-F4 |
| **`pc[14..15]` Z** | `+0x48`, `+0x40` | @0x800572F8-0C |
| **`pc[16..17]` Yaw** | `+0x76`; `+0x74 = +0x78 = 0` | @0x80057320-30 |
| — | `+0x04 = 0`, `+0x09 = (Typ < 0x40 ? 4 : 128)` | @0x800573CC / @0x80057410-28 |

Gegenprobe der Feldlage: ROOM1030 `44 00 01 1f 06 10 03 02 00 ff b7 a1 e8 ea …` → `pc[6] = 3`
→ `+0x1C2 = −5400` **und** `pc[12..13] = 0xEAE8 = −5400`: Y-Feld und Etagenformel decken sich.

**Fazit zur Spawn-Geometrie:** die RDT haelt alle fuenf auf einem **Parkplatz (−32000, 0, −32000)**.
Die Buehne stellt der CODE: G5 setzt sich in [T0] auf **X = −9000 (@0x801011D0), Z = −23400
(@0x801011D8)** und `+0x218 = 3` (@0x80101208); die Tentakel rechnen sich daraus ihre Position
(§4.2/§4.3). **Es gibt keine handgesetzten Tentakelkoordinaten zu portieren.**

### 4.5 Die Sonderrolle von Slot 1 (`+0x10E = 1`)

`(e[+0x10E] & 0xFFF) == 1` → Konstruktor setzt **`+0x05 = 15`** und **`+0x219 = 1`**
(@0x801008B0-CC). Tentakel **Index 0** startet also in sub15 (Vorkampf-Clip 13), bleibt am
RDT-Parkplatz und **ohne Kollision** (`e->flags |= 2`, `Kind->flags |= 2` @0x801040A8-6C),
bis G5s `+0x1D4`-Bit 0 gesetzt ist (@0x801040F0-08). Danach: ph1 spielt Clip 13 (120 Frames) ab;
ph2 setzt **Routine-Wort 1, `+0x219 = 0`, `scaleX = 0`, Flag 2 wieder weg** (@0x8010412C-6C).

G5s Intro-[T1] (@0x80101234 / @0x80101258 / @0x80101280) weckt nur die Indizes **0, 2, 3**
(`0x800CFE20` / `0x800CFE28` / `0x800CFE2C`) mit `+0x06 = 1`; Index 1 (`0x800CFE24`) bleibt in
sub8 ph0 stehen, bis [T1] bei t = 90 **allen vieren** `+0x06 = 6` gibt (@0x801012AC-E0).
Der 120-Frame-Clip 13 ist bei t = 90 noch nicht fertig → Index 0 steht dann formal mit Phase 6
in sub15 (dort gibt es keine Phase 6 → Ruecksprung ohne Wirkung); **erst G5s [T5] mit dem VOLLEN
Wort `0x901` an alle vier holt ihn wieder in die Reihe**. Das ist im Original so und harmlos —
fuer den Port ist es aber sichtbarer Leerlauf (§8.1, dokumentierte Abweichung empfohlen).

---

## 5. Wie G5 kommandiert (Frage 4) — byte-true

### 5.1 Die Sender

```
0x80104E9C(slot, wort):   *(u32*)(*(0x800CFE20 + slot*4) + 4) = wort     @0x80104E9C-B0
0x80104E5C(wort):         dasselbe fuer slot 0,1,2,3                      @0x80104E5C-98
```

Das Ziel ist die **Entity-Slot-Liste** `0x800CFE20..0x800CFE2C` (= Slot-Array-Eintraege 1..4;
Basis `0x800CFE1C`, gesetzt in `FUN_80049E48` @0x80049ED8-DC, 33 Eintraege mit dem Dummy
`0x800D424C` vorbelegt @0x80049EE8-F4; Endzeiger `DAT_800CE334`). Geschrieben wird ein **ganzes
Wort** auf `+0x04`, also gleichzeitig Routine, Sub-Zustand und Phase.

### 5.2 Die `+0x228`-Maske (G5-Feld, 1 Byte)

Kandidatenscan im G5 (@0x801009E8-A50 und @0x80100AE4-B34, zweimal identischer Code):

```
mask = G5[+0x228]
for i in 0..3:
    if ((mask >> i)     & 1) continue      // Bit i   = gesperrt  (wird NIE gesetzt, s.u.)
    if ((mask >> (i+4)) & 1) continue      // Bit i+4 = BESCHAEFTIGT
    kandidaten.append(i)
if (#kandidaten == 4) → sofort angreifen   @0x80100A40 → 0x80100CE0
else { G5[+0x16B] = 1; G5[+0x158] = 10 }   @0x80100A44-50
```

**Wer Bit `i+4` setzt/loescht — Vollzensus aller acht Zugriffe im EM037:**

| Adresse | Wirkung | Ort |
|---|---|---|
| 0x8010146C / 0x8010147C | **setzt** | sub3 ph0 (Peitsche beginnt) |
| 0x80101808 / 0x80101814 | loescht | sub3 ph4 (Peitsche fertig) |
| 0x80102B40 / 0x80102B4C | **loescht** | sub11 ph0 (Zug-Tentakel gilt weiter als frei) |
| 0x801033C8 / 0x801033D4 | setzt | sub12 ph0 |
| 0x801034AC / 0x801034B8 | loescht | sub12 Ende |
| 0x801035BC / 0x801035CC | **setzt** | sub13 ph0 (Spiess beginnt) |
| 0x801038E0 / 0x801038EC | loescht | sub13 ph5 (Spiess fertig) |
| 0x801039B0 / 0x801039BC | **setzt** | sub14 ph0 (Tod — bleibt fuer immer gesetzt) |

**Bits 0..3 werden nirgends gesetzt.** Einziger Schreiber der unteren Haelfte ist G5s
Konstruktor `G5[+0x228] = 0` (@0x80100494). Die Maske fuehrt im Auslieferungsstand also nur
die obere Nibble.

### 5.3 Die Muster-Auswahl (G5 sub0 ph1 / sub3, @0x80100B34-C8)

```
n = #kandidaten;  if (n == 0) → nichts                     @0x80100B38
if (G5[+0x1F0] /*Distanz*/ < 7000) G5[+0x16A] = 0          @0x80100B40-54
slot = kandidaten[rng() % n]                               @0x80100B58-80
b    = ((rng() >> (rng() & 3)) & 1)                        @0x80100B84-A8
wort = *(u32*)(0x80105674 + G5[+0x16A]*8 + b*4)            @0x80100BAC-C0
0x80104E9C(slot, wort)                                     @0x80100BC4
```

**Mustertabelle @0x80105674 (8 Zeilen × 2 Woerter), vollstaendig:**

| Muster `+0x16A` | Wort A | Wort B |
|---|---|---|
| 0 (immer bei Distanz < 7000) | 0x301 | 0x301 |
| 1 | 0xD01 | 0xD01 |
| 2 | 0x301 | 0x301 |
| 3 | 0xD01 | 0x301 |
| 4 | 0xD01 | 0x301 |
| 5 | 0xD01 | 0xD01 |
| 6 | 0xD01 | 0xD01 |
| 7 | 0x301 | 0xD01 |

Nur **0x301 (sub3)** und **0xD01 (sub13)** werden je „frei gewaehlt" gesendet. Alles andere
(0x601 / 0x901 / 0xA01 / 0x40A01 / 0xB01 / 0x70B01 / 0x90B01 / 0xE01) sind **feste
Choreografie-Schuesse** aus dem Intro, dem Zug-Zustand und der Todessequenz (§7).

---

## 6. Kollision: das KIND-Entity (und warum es gebraucht wird)

Der Konstruktor spawnt fuer **jeden** Tentakel ein zweites Entity (@0x80100880-AC):

```
e[+0x05] = 8                                            @0x80100890
kind = 0x801050BC(52, e)   // Klon-Allokator: 584 B je Entity, Anhang ans Slot-Array
kind->flags = 0 ; kind[+0x10E] = 8                      @0x801008A4-AC
0x801004F0(kind, pool, edd)   // Rekursion in den Konstruktor
```

`0x801050BC` sucht ab `0x800CFE1C` den ersten Slot, dessen Entity `flags & 1 == 0` ist
(@0x801050DC-124), haengt notfalls den Endzeiger `0x800CE334` weiter (@0x8010512C-48) und
reserviert 584 B aus dem Heap `0x800CE32C` (@0x8010514C-80). Da die Slots 0..4 von G5 + 4
Tentakeln belegt sind, landen die Kinder auf **`0x800CFE30 + idx*4`**; ihr eigener
Konstruktorlauf gibt ihnen `+0x218 = 4 + idx` (die Suchschleife @0x80100640-64 laeuft ueber das
Slot-Array hinaus weiter). Ihr Main-Zweig ist `+0x10E == 8` → **`0x80104F64`, dann
`e->flags &= ~2`, fertig** (@0x801001A0-C8).

`0x80104F64` (Kind, je Frame):

* Elter = `*(0x800CFE10 + kindIdx*4)` = `0x800CFE20 + idx*4` = der eigene Tentakel @0x80104F70-84
* Kollisionspunkte aus den **Part-Weltpositionen des ELTERN-Modells**:
  `+0x84 = Part3` (parts+608/612/616), `+0xA4 = Part2` (parts+436/440/444),
  `+0xC4 = Part1` (parts+264/268/272), `+0xE4 = Mitte(Part1, Part2)` @0x80104F8C-074
* `+0x0D = 255`; `Elter[+0x220] &= ~6`;
  **`Elter[+0x220] |= FUN_80034D0C(kind, 0x800CFBF8) * 6`** @0x80105078-A4

Der Tentakel selbst baut im Main `0x80104E08` die Punkte `+0x84 = Part1`, `+0xA4 = Part0`,
`+0xC4 = Part0 + (Part1 − Part0)/4` (@0x80104E18-EA4). **Zusammen decken Elter und Kind die ganze
4-Glied-Kette ab.** `+0x220`-Bit 1 wird von sub11 geprueft, Bit 2 von sub3/sub13 — beide sind
gesetzt, wenn `FUN_80034D0C` **1** zurueckgibt.

Ausserdem im Main je Frame:

* `0x801054CC(e)`: `G5[+0x223] &= ~(1<<idx)`; falls `+0x222 > 0`: `+0x222--`,
  `G5[+0x223] |= (1<<idx)`, **`FUN_80039514(3,130,0)` + `FUN_800395B8(25,130,0,3)`**
  (@0x801054DC-548) — das Beben beim Durchbrechen.
* `parts[+0x8C]` (scaleX von Part 0) wird auf Part 1/2/3 kopiert, solange `!(+0x220 & 1)`
  (@0x801003BC-EC) — **ein `scaleX` streckt die ganze Kette**.
* `FUN_80016028(e)` = Frame-Flag-SE (§2.1) @0x801003F0.
* Hilfsfunktionen: `0x80104C30(e, parts)` = `RotMatrix(part.ang@+104)` → `part.mat@+24` fuer alle
  4 Parts (@0x80104C3C-6C); `0x80104BA4(e, parts)` = Weltkette `RotMatrix(e+0x74)` → `e+36`,
  dann `CompMatrix` Part fuer Part (@0x80104BB8-…); `0x80104D64(e, px, pz, schritt)` = RotX
  `+0x74` schrittweise auf einen Zielpunkt drehen (@0x80104D64-DF0).

---

## 7. Gesamtfahrplan: G5-Phase → Tentakel-Kommando → Clip → Schaden

| G5-Zustand (EM036) | Zeitpunkt | Ziel | Wort / Schreibweise | EM037-Wirkung |
|---|---|---|---|---|
| [T1] Intro ph1 | t=10 / 30 / 40 | idx 0 / 2 / 3 | `+0x06 = 1` @0x80101240 / @0x80101268 / @0x80101290 | sub8 ph1: SE 0, Austritt beginnt, `+0x219 = 1` |
| [T1] | t=90 | alle 4 | `+0x06 = 6` @0x801012B8-E0 | sub8 ph6: Bodenbeben, 180 Ticks |
| [T4] Intro ph4 | t=30 | alle 4 | `+0x06 = 8` | sub8 ph8: einziehen (`scaleX` −64/F) |
| [T5] Intro ph5 | t=20/40/45/56 | idx 0/1/2/3 | **`0x901`** | **sub9 ph0** — Speer, Rotation B-Satz |
| [T5] | t=110 | alle 4 | `+0x06 = 7` | sub9 ph7: Ausrollen |
| [T7] Intro ph7 | — | alle 4 | `+0x06 = 9` | sub9 ph9: Abschluss |
| [T10] Intro ph10 | t=20/35/40/50 | idx 0/2/1/3 | **`0xA01`** | **sub10 ph0** — Wedeln, Clips 9/10/11 |
| [T11] Intro ph11 | — | idx 0 + 2 | **`0x40A01`** | **sub10 ph4** — Peitschenclip 7/8 (ohne Schaden) |
| [T13] Intro ph13 | — | Broadcast | **`0x601`** | **sub6** — einziehen → sub0 |
| G5 sub0 ph1 (Kampf) | alle 16 Ticks | 1 Kandidat | **`0x301` / `0xD01`** (§5.3) | **sub3** (Clip 7/8, 15 Schaden + Griff) bzw. **sub13** (Clip 21/23, 15 / 10 Schaden + Griff) |
| G5 sub3 (Rueckzug) | alle 32 Frames | 1 Kandidat | wie oben | dito |
| G5 sub1 ph0 (ZUG) | Frame 0 | Broadcast | **`0xB01`** | **sub11 ph0** — alle vier greifen an den Zugpunkt |
| G5 sub1 ph1 | Frames 0/7/10/15 | Reihe[0..3] | `0xB01` | gestaffelter Nachschlag |
| G5 sub1 ph1 | Frame 55 / 61 | Reihe[0]+[2] / [1]+[3] | **`0x70B01`** | **sub11 ph7** — Zittern + Zug-Skalierung |
| G5 sub1 ph3 | Frame 25 / 15 | Reihe[0]+[2] / [1]+[3] | **`0x90B01`** | **sub11 ph9** — Loesen, zurueck zu sub0 |
| G5 Tod [T17] | alle 4 Frames | Zufallsslot | **`0xE01`** | **sub14 ph0** — Tod, Clips 17..20 |

Reihenfolge-Tabelle fuer die Staffelung: G5-Overlaykopf `+0x04`, 16 Byte @0x80100004
`{0,2,1,3 / 1,2,3,0 / 3,0,2,1 / 3,1,2,0}`; Zeile = `(rng >> (rng & 3)) & 3` (EM036 @0x80100E0C-14).

**SE-Katalog des EM037** (`FUN_8005BD6C(id, e)`, ENEMSE-Bank `+0x1FA` = **0x27** → Paarzeile 25
`{0x27,0x00}` @PSX.EXE-Datei 0x97C32, exakt wie beim G5):

| id | Ausloeser |
|---|---|
| **0** | Austritt/Setzen: sub3 ph0 @0x80101478, sub8 ph1 @0x80101DA4, sub8 ph8 @0x8010201C, sub9 ph0 @0x8010213C, sub10 ph0 @0x801027F8, sub11 ph0 @0x80102B20, sub11 ph9 @0x80103284, sub13 ph0 @0x801035C8 |
| **1** | Tod sub14 ph5 @0x80103E24; **Frame-Flag** Clip 21 F35, Clip 23 F31 |
| **2** | Durchbruch/Ruettler: sub8 ph3 @0x80101E14, sub9 ph4 @0x8010236C, sub11 ph4 @0x80102E18 |
| **3** | **Griff/Treffer**: sub3 @0x80101790, sub13 @0x80103774 und @0x8010382C |
| **4** | nur **Frame-Flag** (Clips 7, 8, 13, 14, 15, 16, 17, 18) |
| **5** | nur **Frame-Flag** (Clips 13, 17, 18, 19, 20) |
| **15** | Volle Streckung: sub9 ph2 @0x8010221C, sub11 ph2 @0x80102C28; **Frame-Flag** Clip 7 F62, Clip 8 F51, Clip 13 F3, Clip 21 F27, Clip 23 F25 |

Zusaetzlich 3D-SE `FUN_8005BA28` (6 Aufrufe) ausschliesslich in den Script-Routinen 4/7 — fuer
ROOM5090 ohne Belang.

---

## 8. UMSETZUNGS-SPEZIFIKATION — `enemy_ai_tentakel_g5.c`

**Muster:** `re15_port/engine/src/enemy_ai_boss_g5.c` (Modul-Zustandsstruct + eigener Tick,
Registrierung nach dem Gator-Muster). Neues Modul, **kein** Eingriff in bestehende KI-Wurzeln.

### 8.1 Aktoren

* 4 Aktoren, Typ **0x37**, gespawnt vom G5-Modul beim **Kampfstart** (`grid == 0x13`,
  `Member_set(0x0c,0x13)` — dieselbe Freigabe, die heute `s_g5.gestartet` setzt).
  ROOM5090 belegt nur Skript-Slot 0 und 1 (`re15_port/shared_assets/PSX/STAGE5/ROOM5090.RDT`
  @0x122A Typ 0x4D, @0x124A Typ 0x30 → Port-Umtypung 0x36) → Aktor-Indizes 1 und 2 belegt,
  `RE15_ACTOR_MAX = 16` (`re15_port/include/re15_actor.h:22`),
  `SCRIPT_SLOT_TO_ACTOR(s) = s+1` (`scd_vm.c:2799`) → **Aktoren 3..6 sind frei**.
  **PORT-ENTSCHEIDUNG (dokumentieren):** RE1.5s ROOM5090-Skript kennt keine 0x37-Records; RE2
  spawnt sie aus der RDT. Wir spawnen sie aus dem Modul mit exakt den RE2-Record-Werten
  (SE-Bank 0x27, Kill-Flag 0xFF, Yaw 0, Boden 0).
* **`+0x10E = 0` fuer ALLE VIER** (Abweichung von RE2s Slot 1 = 1). Begruendung dokumentieren:
  Variante 1 parkt den Tentakel bis [T5] am RDT-Parkplatz und laesst ihn Clip 13 im Leerlauf
  spielen (§4.5) — im Port ohne diesen Parkplatz waere das nur ein fehlender Tentakel.
  Voll byte-true waere: sub15 nachbauen **plus** Parkplatz (−32000, 0, −32000) **plus**
  Flag-2-Kollisionsabschaltung.
* Das **Kind-Entity** wird NICHT als fuenfter Aktor gebaut. Das Modul rechnet die vier
  Kollisionspunkte (§6) direkt und testet sie selbst. Dokumentierte Vereinfachung — die
  Semantik (Bits 1/2 in `+0x220`) bleibt identisch.

### 8.2 Platzierung (jeden Frame, vor dem Zustands-Dispatch)

```c
static const int16_t k_anker[4][3] = {   /* @0x80105668 */
    {  965, -2243, -1933 },              /* idx 0 */
    { 1615,  -706, -2440 },              /* idx 1 */
    { 1409,  -683,  2495 },              /* idx 2 */
    {  957, -2426,  1958 },              /* idx 3 */
};
/* Modus +0x219: 0 = voll, 1 = frei, 2 = halbiert + angehoben  (@0x80100230-33c) */
v = k_anker[idx];
if (modus == 2) { v.z /= 2; v.y = v.y/2 - ((idx == 1 || idx == 2) ? 1500 : 1300); }
if (modus != 1) { v = rot_y(boss_yaw) * v;  pos = boss_part2_world + v; }   /* @0x80100348-88 */
```

`boss_part2_world`: der Port rendert EM036 mit Mesh 2 = Blob (`main.c` ab Zeile 783). Solange die
Part-Weltmatrizen nicht ausgelesen werden, ist der **Ersatzanker** = Bossposition + Blob-Bindeversatz
(Bind 1800/4500/0 laut `main.c`-Kommentar) — als **dokumentierte Naeherung** kennzeichnen, nicht
als byte-true.

**Vorwaerts** ist im Port die Spielerrichtung entlang X (`enemy_ai_boss_g5.c`-Kopf: „Yaw beim
Kampfstart einrasten, danach konstant"). Damit zeigt Anker-`x` nach vorn, Anker-`z` nach
links/rechts, Anker-`y` nach oben — genau wie in RE2 (Yaw dort konstant 0, @0x8010044C).

### 8.3 Raumgrenzen der Spitzensonde

RE2: `tipY ∈ (−5000, 0]`, `tipZ ∈ [−27000, −19800)`. Im Port ist der Korridor derselbe Streifen
um **z ≈ −23400** (`enemy_ai_boss_g5.c`-Kopf; SCA @0x860 spannt x −37469..23962)
→ **`tipZ` woertlich uebernehmen** (−27000 / −19800 liegen symmetrisch um −23400, ±3600),
`tipY` woertlich (Boden 0, Decke −5000). Eine X-Grenze gibt es nicht — RE2 begrenzt X nicht,
weil der Tentakel am Blob haengt.

### 8.4 Zustands-Tabelle fuers Modul (Ticks = Frames @30 fps)

| Port-State | RE2 | Clip | Dauer | Bewegung | Schaden | SE |
|---|---|---|---|---|---|---|
| IDLE | sub0 | 0 | 60 + rng&63 | `scaleX` 0, Modell aus | — | — |
| AUSTRITT | sub8 ph0..2 | 14 / 14 / 16 / 15 (je idx) | 30 F, `scaleX` 0→4096 in 16 F | X-Mikroversatz §3.8 | — | 0, Frame-Flag 4 |
| BEBEN | sub8 ph3..6 | 0 (Blend 7) | 36 + 180 T | Zittern ±15/Achse | — | 2 + `+0x222`-Ruettler |
| EINZUG | sub8 ph8 | — | `scaleX` −64/F | Yaw ∓16/F | — | 0 |
| SPEER | sub9 | 0 → 2 | `scaleX` 0→4096 (8/16 je F) | Rot B-Satz, Einrollrampe `+0x15A` 64→0 (−2/F) | — | 0, 15, 2 |
| WEDELN | sub10 | 9/10/11 (idx%3), Wechsel alle 60 + rng&15 T | — | Rot A-Satz | — | 0 |
| WEDEL-SCHLAG | sub10 ph4 | 7 oder 8 | 100 F | — | **keiner** | Frame-Flag 4/15 |
| **PEITSCHE** | **sub3** | **7 (rng&1 == 0) / 8** | **100 F** | RotX je idx 896/1088/2976/3200; RotZ −576 → +32, dann +8/+4 im Fenster | **15** (Typ 0), Fenster **F 61..69** (Clip 7) / **F 53..59** (Clip 8) | 0, 3, Frame-Flag 4/15 |
| **SPIESS** | **sub13** | **23 (idx 0,3) / 21 (idx 1,2)** | **75 F** | `ARC` auf den Spieler (Schritt 16); RotZ −128/−256 → 0 | **15** (Typ 1) frei / **10** bei bereits Gegriffenem, Fenster **F 26..32** | 0, 3, Frame-Flag 15/1 |
| **ZUG** | **sub11** | 0 → 2 | ph0..2 ≈ 30 F, ph5 60 T, ph7 90 T | Rot aus 16er-Tabelle; **`scaleX = scale0 · (X0 + 8192 − X) / 8192`** | **15** (Typ 1) im ph3-Fenster | 0, 15, 2 |
| TOD | sub14 | 17..20 → 19/20 → 9..11 | ≈ 500 T | `scaleX −6 / scaleY −32 / scaleZ +16`, Farbe ausbleichen | — | 1, Frame-Flag 4/5 |

*(Peitschenfenster: die Vergleiche sind `T[2] < Frame < T[3]`, also streng — Clip 7: 61..69,
Clip 8: 53..59; das „Ausholfenster" `T[0] < Frame < T[1]` ist 31..54 bzw. 39..49.)*

**Griff-Latch (sub3 / sub11 / sub13):** Schaden + Spieler-Grab setzen + Griffmodus
3 / 5 / 0x105 / 0x205 / 0x305 / 0x405 merken + Kamera-Yaw-Override (3456 bzw. 640 bei idx < 2,
nur sub3 und nur bei Distanz ≥ 6001). Die sechs **Opfer-Anims** liegen in EDD-Paar 3 auf dem
15-Bone-PL0-Rig → **DEFERRED** (gleiche Entscheidung wie beim G5-Devour); im Port genuegen
Schaden + Freeze + Yaw-Snap.

### 8.5 Kommando-Hook im G5-Modul

`enemy_ai_boss_g5.c:86` (`g5_tentakel_cmd` / `g5_tentakel_broadcast`) auf echte Zustellung
umstellen: das Wort in `tent[idx].wort` legen, das Tentakelmodul dekodiert
`{routine, sub, ph, _}` daraus (LE-Bytes). **Die `+0x228`-Maske muss echt werden:** der
Kandidatenscan in `enemy_ai_boss_g5.c:567` darf nicht mehr „alle 4 frei" annehmen, sondern muss
Bit `idx+4` lesen — sonst kommandiert G5 dauernd belegte Tentakel. Setzen/Loeschen exakt nach
§5.2; Bits 0..3 bleiben 0. Die Muster-Auswahl (§5.3) samt Tabelle @0x80105674 dazu.

### 8.6 Modell / Renderer

`pc_enemy_load_ex` laedt 0x37 in 5090/5091 bereits ueber `pc_enemy_load_re2`
(`re15_port/platform/pc/main.c:755`). Nachzupruefen ist nur:

* `re2_emd_parse_bank` waehlt das Paar mit den meisten Clips → **Paar 1 (24 Clips)** ✓
* `dir_count == 8` ✓, `md1.mesh_count = 4` ✓ (kein Kuerzen noetig, anders als beim EM036)
* **`scaleX` pro Part** braucht Renderer-Unterstuetzung. Ohne sie kann der Tentakel nicht
  „wachsen" — Minimalloesung: den Strang beim Austritt/Einzug ueber Bone-0-Skalierung oder
  ersatzweise ueber Sichtbarkeit schalten und die Abweichung dokumentieren.
* `bone_relative_pos` = (0,0,0)/(3750,0,0)/(3003,0,0)/(1508,0,0), Kette 0→1→2→3 (§2).
* **Kein** Root-Motion-Pfad anhaengen (§2.1).

### 8.7 Reihenfolge der Arbeit (Empfehlung)

1. Aktor-Spawn + Ankernachfuehrung + IDLE/AUSTRITT (sofort sichtbar: vier Tentakel).
2. Kommando-Hook + `+0x228`-Maske + sub3/sub13 samt Trefferfenstern (Schaden kommt an).
3. sub11 mit Laengenskalierung (der „Zug").
4. sub8/sub9/sub10-Introchoreografie an G5s [T1]..[T13].
5. sub14-Tod (nur wenn G5 stirbt — `0xE01` aus [T17]).

---

## 9. OFFEN

* `pad`-Halbwort der Ankertabelle (0/0/2048/2048 @0x8010566E/76/7E/86) — kein Leser gefunden.
* `FUN_80034D0C`-Rueckgabewerte (1 / 2 / 3) → Bitmuster `ret*6` in `+0x220`: welche Hitbox
  welcher Rueckgabe entspricht, ist nicht disassembliert; im Port genuegt „Treffer = Bits 1+2".
* `FUN_80016480(e+0x16C, 0,0,0, parts+92)` @0x80100628 — Zweck unklar.
* Routine 7 (0x80104280) und Routine 4 (Tabelle @0x8010582C, 5 Eintraege) = Script-/Event-Kanal
  mit 3D-SE `FUN_8005BA28` — fuer ROOM5090 ohne Belang, nicht zensiert.
* Subs 1, 2, 5, 12 werden von keinem G5-Kommando erreicht (Sende-Vollzensus §7); sie sind
  disassembliert, aber nicht Frame fuer Frame ausgeschrieben.
* Ob RE2s Slot-1-Tentakel (`+0x10E = 1`) im echten Spiel je sichtbar wird, ist statisch
  beantwortet (er wird erst von [T5]s `0x901` eingereiht), aber nicht dynamisch gegengeprueft.
* `0x800CE300[slot] = 0x80104288` und `0x800CE400[slot] = 0x80104B68` (Ctor @0x801006E4-718):
  EXE-Callbacks, deren Aufrufstellen nicht verfolgt sind.

---

## FAZIT

Das EM037-Overlay ist vollstaendig zensiert und bestaetigt die Kernthese des Nutzerbefunds:
Birkins Tentakel sind **kein Effekt am G5-Modell, sondern vier eigenstaendige Entities mit
eigener 23-KB-KI, eigener 24-Clip-Bank und eigenem 4-Bone-Rig** — und der Port hat sie schlicht
nie gespawnt. Die Mechanik ist ungewoehnlich leicht zu portieren, weil die Tentakel **null
Eigenbewegung** haben: alle 24 Clips tragen exakt 0 Root-Motion (selbst aus dem EMD geparst),
die Position kommt jeden Frame aus G5s Part-2-Weltmatrix plus einem festen Ankeroffset
(@0x80105668: zwei links, zwei rechts, je einer hoch und einer tief), und die „Laenge" ist nichts
als `part0.scaleX` 0→4096, das der Main auf alle vier Parts kopiert (@0x801003BC-EC). G5 steuert
sie ueber ein einziges Wort in deren `+0x04` (`0x80104E9C`, Broadcast `0x80104E5C`), das
gleichzeitig Routine, Subzustand und Phase setzt; frei gewaehlt werden nur zwei Angriffe —
**0x301 = sub3 Peitsche (Clip 7/8, Trefferfenster Frames 61..69 bzw. 53..59, 15 Schaden + Griff)**
und **0xD01 = sub13 Spiess (Clip 23 fuer Tentakel 0/3, Clip 21 fuer 1/2, Fenster Frames 26..32,
15 Schaden bzw. 10 beim Wegschnappen eines schon Gegriffenen)** — gewuerfelt aus der Mustertabelle
@0x80105674 ueber die Kandidatenmaske `+0x228`, deren obere Nibble genau vier
„beschaeftigt"-Bits fuehrt (setzende und loeschende Stellen vollstaendig gelistet, Bits 0..3
werden nie gesetzt). Der vom Nutzer beschriebene „Zug nach vorn" ist sub11: alle vier greifen auf
`0xB01` einen Punkt 8192 Einheiten vor G5 und verkuerzen sich proportional zur Restdistanz
(@0x80102FD0-3004), waehrend G5 per Clip-5-Root-Motion nachkriecht — `0x70B01` und `0x90B01` sind
nur die Phasen 7 und 9 desselben Zustands. Die Kollision laeuft ueber ein **zweites, unsichtbares
Kind-Entity je Tentakel** (`+0x10E = 8`, gespawnt im Konstruktor @0x8010088C), das die Weltpunkte
der Bones 1..3 traegt und das Ergebnis als Bits 1/2 in `+0x220` des Elters zurueckmeldet. Die
RE2-RDT liefert keine Geometrie: alle fuenf `Sce_em_set`-Records in room7040 parken auf
(−32000, 0, −32000) — die Buehne baut der Code (G5 [T0] auf X=−9000/Z=−23400). Fuer ROOM5090
heisst das: **vier Aktoren beim Kampfstart spawnen und permanent an den Boss ankern; handgesetzte
Positionen braucht es nicht.** Die einzigen bewusst zu dokumentierenden Abweichungen sind die
Variante `+0x10E = 1` des RE2-Slots 1 (Vorkampf-Leerlauf auf dem Parkplatz), das Kind-Entity
(im Port als direkte Punktberechnung) und die Opferanimationen des Griffs (15-Bone-PL0-Rig,
wie beim G5-Devour deferred).
