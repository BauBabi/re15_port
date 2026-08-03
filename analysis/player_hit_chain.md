# Die Spieler-Hit-Kommandokette — wer schreibt aca58/aca59, wann läuft der Wund-Dispatcher @0x8010a580

**Datum:** 2026-08-03 · **Status:** Kette **byte-belegt** (Disasm + Savestate-RAM), Port-IST code-verifiziert
**Quellen:** `info/Re1.5/PSX.EXE` (t_addr 0x80010000, Datei-Off = addr−0x80010000+0x800), `info/Re1.5/PSX/BIN/STAGE{1..6}.BIN` (@0x80100000 roh, Off = addr−0x80100000), `RE_15_Quellcode_V2/`, `RE_15_Quellcode_Overlays/STAGE*_full/`, `stage_saves/mzd_blood_decals_hp30.sav` + `mzd_death_cmd5_struggle.sav` (Live-RAM), Schwester-Dossiers `analysis/blood_decals.md` (Wund-DECAL-Maschine) und `analysis/bite_blood_fx.md` (Blut-FX-Spawns; dort §1/2 = dieselbe Dispatch-Kette aus FX-Sicht).
**Methode Zensus:** vollständiger Binär-Scan aller `sb/sh/sw` mit absolutem `lui 0x800b`-Target 0x800aca58/59 über EXE + alle 6 Overlays; Werte an den Schlüssel-Sites roh-disasm-verifiziert; Decompile-Quercheck. Grenze: register-indirekte Stores (`sb v0,4(s1)` mit s1=Spieler) erfasst der Scan nicht — der einzige gefundene indirekte Writer ist FUN_80012d60 (§2.1, roh-verifiziert).

---

## 0. Executive Answer

`0x800aca58/59/5a` = Spieler-Entity `+0x4/+0x5/+0x6` = **Kommando / Richtungs-Parameter / Phase**. Der Spieler-Task `FUN_80031c44` dispatcht **jeden Frame** `PTR_LAB_80073f90[aca58]` (`@0x80031c88`, nur wenn `DAT_800aca40 ≥ 0`).

**Der „Wund-Dispatcher @0x8010a580" ist KEIN Hit-Entry — er ist der Tail von Phase 4 der GRAB-Opfer-Phasenmaschine (cmd 5).** Die Kette: Gegner-Grab schreibt `aca58=5, aca59=Grab-Richtung (0..3), aca5a=0` → EXE-cmd-5-Handler `0x80036834` ruft pro Frame `jalr [0x800ac758 + Grabber-Typ·4]` (`@0x80036884-a8`; Basis versteckt als `lw v0,-770(a0)` mit a0=0x800aca5a!) → Overlay-Hook `0x8010a28c` (von `FUN_8011e864` beim Overlay-Load registriert) → Tabelle `0x801201a8[aca59]` → Phasenmaschine `0x8010a2cc` (Phasen via `aca5a`, Sprungtabelle `@0x8010017c`). **Der GEGNER schiebt beim Mash-Release `aca5a := 4`** (Zombie-Girl `@0x8010faf4`/`@0x8010ff48`, Shared-Grab FUN_80102548 case 3) → Phase-4-Handler `0x8010a4e8`: Blut-Spawn + **`@0x8010a580 lbu aca59; jalr 0x801201b8[aca59]`** = Wund-Helper → `FUN_80037edc`. **Genau EINMAL pro überlebtem Grab** (Phase 4 setzt sofort `aca5a=5`).

**Substate 0/1 sind keine „Schwer-Treffer" — sie sind die Grab-Richtung vorne/hinten.** Die 3× Substate-0-Wunden des Nutzer-Saves = **drei per Button-Mash überlebte Front-Grabs**. Der 15s-Grab-Test stempelte nichts, weil ohne Mash der Release nie eintritt: ohne Tastendruck läuft der Biss-Zähler (+0x9e=50) ab → Devour (cmd 6, tödlich, HP:=−1 `@0x8010a814`) — der Devour-Pfad durchläuft Phase 4 NIE.

**Cmd-2-Hits stempeln über den Dispatcher NIE**: der cmd-2-Handler `0x80035af0` dispatcht in eine **reine EXE-Tabelle** `0x800741a8[aca59]` (kein Overlay-Hook, kein `FUN_80037edc`-Call in 0x80035b70–0x80036700). Einzelne Angriffe stempeln stattdessen **direkt an der Attack-Site** (Zombie-Steh-Biss `@0x8010f360`: zufälliges Torso-Panel +50).

---

## 1. Die Cmd-Maschine (EXE)

### 1.1 Dispatch

```
FUN_80031c44 (Spieler-Task, pro Frame):
  DAT_800ac784 = &DAT_800aca54           ; g_entity := Spieler
  if (DAT_800aca40 >= 0)                  ; nicht pausiert/gefreezt
    (*PTR_LAB_80073f90[DAT_800aca58])()   ; Cmd-Dispatch
```

Tabelle `@0x80073f90` (Datei-Off 0x64790):

| cmd | Handler | Bedeutung (belegt) |
|---|---|---|
| 0 | 0x800318f8 | Init/Spawn → schreibt cmd 1 (`sw 1 @0x8003192c`) |
| 1 | 0x80031de8 | Normal-Steuerung (aca59 = Motion-Substate; Writer §2.2) |
| 2 | 0x80035af0 | **HIT** → `jalr 0x800741a8[aca59]` `@0x80035b40-58` (reine EXE-Tabelle) |
| 3 | 0x800366bc | **DEATH** (Schuss/Hit-Tod) → jal FUN_80036718 → `sh aca58=7 @0x80036814` |
| 4 | 0x80030660 | Fall/Knockdown-Bewegung (aca59 = Phase 4..9, Writer `@0x80030cf0-0x8003141c`; Krähe wirft mit `4/6` um, §2.3) |
| 5 | 0x80036834 | **GRABBED** → `jalr [0x800ac758 + Typ·4]` `@0x800368a0` (Hook-Tabelle A) |
| 6 | 0x800368c0 | **DEVOURED** → `jalr [0x800ac858 + Typ·4]` `@0x8003692c` (Hook-Tabelle B) |
| 7 | 0x8003694c | Leiche/Death-Sequenz (120-Frame-Fade; liest 0x800acb18) |
| 8 | 0x80031f38 | Tür/Climb-Sequenzen (schreibt Folge-cmds 0x101/0x301/0x401/0x701, §2.1) |
| 9 | 0x800322e8 | dito (zweite Familie) |

`FUN_8001500c` behandelt `aca58 ∈ {6,3,7}` als Todes-Klasse (`@FUN_8001500c.c:4`).

### 1.2 cmd 2 (HIT): EXE-Substate-Tabelle `@0x800741a8` — OHNE Wund-Stempel

```
80035b34 lbu v0, 0x800aca59
80035b40 sll v0,v0,2 ; lui at,0x8007 ; addiu at,0x41a8
80035b50 lw v0,0(at)                  ; 0x800741a8[aca59]
80035b58 jalr v0
```

| aca59 | Handler | Klasse | Writer existiert? |
|---|---|---|---|
| 0 | 0x80035b70 | „schwer vorne" (Clip 0xa `@0x80035bd0-d8`) | **NEIN — toter Code** (Zensus §2: kein cmd-2-Writer mit aca59<2 in EXE + allen 6 Overlays) |
| 1 | 0x80035ca8 | „schwer hinten" | **NEIN — toter Code** |
| 2 | 0x80035de0 | Standard vorne | ja (überall: `facing+2`) |
| 3 | 0x80035f64 | Standard hinten | ja |
| 4 | 0x800360e8 | Knockdown vorne | ja (Dog-Schwer-Biss FUN_8011854c `@0x801187e8/f0`: `facing+4`; STAGE3-5-Pendants) |
| 5 | 0x8003644c | Knockdown hinten | ja |

**Alle sechs Handler sind selbständige EXE-Phasen-FSMs** (Setup → Anim → Exit `aca58:=1`), Byte-Scan 0x80035b70–0x80036700: **kein `jal 0x80037edc`, kein `jal 0x80019700`, kein `jalr`** (deckt sich mit `bite_blood_fx.md` §3.2). → **Der plain Hit stempelt keine Wunde und ruft kein Overlay.** Die Wund-Helper [2]/[3] der Tabelle `0x801201b8` werden von cmd 2 nie erreicht — sie gehören zu Grab-Richtung 2/3 (§3.2).

### 1.3 cmd 5/6: die versteckten Hook-Tabellen (warum statische Scans sie verfehlen)

```
80036884 lw v0, 0x800acbfc     ; GRABBER-Entity (vom Gegner beim Grab-Init gesetzt)
80036890 lbu v0, 8(v0)         ; +0x8 = Typ
80036898 sll v0,v0,2
8003689c addu v0,a0,v0         ; a0 = 0x800aca5a (!)
800368a0 lw v0,-770(v0)        ; 0x800aca5a - 0x302 + Typ*4 = 0x800AC758 + Typ*4
800368a8 jalr v0
```

cmd 6 analog `@0x8003692c`: `lw v0,-514(v0)` → Basis **0x800AC858**. Die Tabellen liegen im RAM (BSS), werden **vom Stage-Overlay beim Load befüllt** und tauchen deshalb in keinem statischen Xref auf (die EXE bildet die Basis über das aca5a-Register, die Overlays schreiben sie per `at`-relativem `sw`).

**Registrierung STAGE1: `FUN_8011e864` (Overlay-Init), roh `@0x8011e9c0-0x8011eac8`:**

| Typ | Hook A (cmd 5, Grab) @0x800ac758+Typ·4 | Hook B (cmd 6, Devour) @0x800ac858+Typ·4 |
|---|---|---|
| 0x10,0x11,0x12,0x16,0x18,0x1c–0x1f | LAB_8010a28c (generisch) | LAB_8010a6b8 (generisch) |
| 0x13 Zombie-Girl | LAB_8010c16c (Trampolin via `0x801208fc[aca59]` → gleiche FSM 0x8010a2cc) | LAB_8010c1ac (via `0x8012090c[aca59]` → 0x8010a6f8) |
| 0x20 Dog | LAB_80111944 (**stempelt selbst**: jal 0x80037edc `@0x80111b64` +10) | LAB_80111cb0 (**stempelt**: `@0x80111e78/84/90` +10/+50/+50) |
| 0x21 Krähe | LAB_8011597c (kein Stempel) | LAB_80115d2c (**stempelt**: `@0x8011620c/2c/38/44/60/70` +10er) |
| 0x27 Gorilla | LAB_8011c118 (kein Stempel) | LAB_8011c3d4 (**stempelt**: `@0x8011c55c/68/74` +10/+50/+50) |

(Savestate-verifiziert: beide Saves zeigen exakt diese Pointer live @0x800ac798ff/0x800ac898ff. `bite_blood_fx.md` §1 nennt für 0x8011c118 „Typ 0x26" — mit der disasm-verankerten Basis 0x800ac758 ist Slot 0x800ac7f4 = **Typ 0x27 Gorilla**; 0x26 Spider (Slot 0x800ac7f0) bleibt 0 = kein Grab.)

STAGE2 registriert dieselbe Architektur (`STAGE2_overlay.c:9560-9588`): generisch A=0x8010a120/B=0x8010a54c, Zombie-Typen 0x10/0x11/0x13 → 0x8010c000/0x8010c040, dazu 0x22→0x8010c3a8, 0x23→0x8010eda0, 0x25 Adult-Spider→0x801156b8 (+B-Pendants).

---

## 2. (A) Schreib-Zensus 0x800aca58/59 (vollständig, absolut adressierte Stores)

### 2.1 EXE (47 Sites)

**Kampf-relevant:**

| Site | Write | Kontext |
|---|---|---|
| `@0x80012ebc/ed4/ee0` | `sb 2,4(s1)` / `sb facing+2,5(s1)` / `sb 0,6(s1)` (s1=Spieler; **register-indirekt**) | **FUN_80012d60** Waffen-/Standard-Damage-Entry: cmd 2, Substate 2/3 (facing via jal 0x8001a7a8 `@0x80012ec8`) |
| `@0x80012ef4/ef8/efc` | `sb 3,4(s1)` / 0 / 0 | dito, HP<0 → cmd 3 DEATH (bgez `@0x80012ee8`) |
| `@0x80031518`, `@0x80031788` | `sw aca58=0` | Spieler-Load (FUN_800314b0-Pfad) — Reset |
| `@0x8003192c` | `sw =1` | cmd-0-Handler → Steuerung |
| `@0x80035c80/db8/f00/0x80036084/362f4/3640c` | `sw =1` | Exits der cmd-2-Substate-Handler [0]–[5] |
| `@0x8003667c` | `sw =1` | Exit Handler [5]-Region |
| `@0x80036814` | `sh aca58=7` | FUN_80036718 (aus cmd 3): Tod → cmd 7 Leiche |
| `@0x8004672c/34` | `sb =1 / sb aca59=0` | Steuerungs-Restore (SCD/Event) |

**Bewegungs-Substates (aca59 bei cmd 1/4/8/9 — KEIN Kampf):** `@0x8002d604/794` (=9, Kollisions-Stolpern, FUN_8002d474), `@0x80030cf0-0x8003141c` (=4..9, cmd-4-Fall-FSM), `@0x80031f88-0x80032d08` (Wörter 0x101/0x301/0x401/0x701 = cmd 1 mit Motion-Substate, Tür/Climb), `@0x80033d4c/34d38/35500` (=0, cmd-1-Motion-Resets), `@0x8003811c-0x800390bc` (=0/0xa/0xc, Aim/Turn-FSM), `@0x8001cbdc` (=0).

### 2.2 STAGE1 (58 Sites) — nach Besitzer-Funktion

**Shared-Angriffs-Bibliothek (Overlay-Anfang; von Action-Tabellen `0x8011f89c`/`0x801202b4` referenziert):**

| Funktion | Site | Write | Bedeutung |
|---|---|---|---|
| FUN_80102548 Grab (Briefing-Zombies) | `@0x80102640` | `sw (state−3)<<8 \| 5` | **cmd 5, dir = Attacker-State−3 ∈ {0,1}** (3=vorne/4=hinten); Biss −10/−5; Mash-Release: `aca5a:=4` (case 3, `_ALL:753`) → **Wund-Stempel** |
| FUN_80102bd8 Devour | `@0x80102c80` | `sw (state−5)<<8 \| 6` | cmd 6, dir 0/1 |
| FUN_80103b94 Liegend-Grab | `@0x80103cc0` | `sw (state+1)<<8 \| 5` | cmd 5, dir 0x26/0x27 (Liegend-Fressen am Lebenden; `aca5a:=4`-Push `_ALL:1373`) |
| FUN_80104548 Liegend-Devour | `@0x801045dc` | `sw (state−1)<<8 \| 6` | cmd 6 |
| FUN_80107634 | `@0x80107b40` | `sw (+0x9f)<<8 \| 5` | cmd 5, dir aus Attacker-Feld |
| FUN_80107634 | `@0x80107c08` | `sw =0x101` | cmd 1 Restore |

**Opfer-FSMs (Spieler-Seite, laufen via Hook):** `@0x8010a664` `sw =1` (Grab-Phase-5-Exit → Steuerung), `@0x8010a8ac/b4` `sb 7 / sb 0` (Devour-Reaktion → cmd 7, FUN_8010a6f8-Tail).

**Zombie-Girl 0x13 (Action-Tabelle `0x80120fd4[+0x5]`, Dispatch `@0x8010dce0-f0`):**

| +0x5 | Funktion | Sites | Write |
|---|---|---|---|
| 8 | FUN_8010f15c Steh-Biss | `@0x8010f30c/324/32c` (case 1), `@0x8010f4d8/4f0` (case 3) | cmd 2, `facing+2`, aca5a=0; HP−10; **case 1 stempelt direkt: `@0x8010f360 jal FUN_80037edc(byte[0x80121010+aca59·2+(rng&1)], 0x32)`** — Panel-Bytes `@0x80121014: 05 07 04 06` (vorne 5\|7, hinten 4\|6, je +50); Gates: HP≥0, +0x1e4==0 |
| 9 | FUN_8010f80c Front-Grab | `@0x8010f910/928/930` | **cmd 5, dir 0**, Phase 0; Biss-Budget +0x9c=100 (case 2), Mash-Fenster +0x9e=50; **Release-Push `@0x8010faf4 sb 4→aca5a`** (Budget −1−100·Mash `@0x8010faa4-facc`); Zähler-Exhaust/HP<0 → State 0xb OHNE Push (`@0x8010fa8c`) |
| 0xa | FUN_8010fc60 Rück-Grab | `@0x8010fd64/7c/84` | cmd 5, **dir 1**; Push `@0x8010ff48` |
| 0xb | FUN_801100b4 Devour-Start | `@0x80110138/140` | **cmd 6, dir 0** (Clip 0x1b; acbfc=self) |

**Weitere STAGE1-Gegner:**

| Funktion (Besitzer) | Sites | Write |
|---|---|---|
| Dog-Grab-Reaktion LAB_80111944 (Hook A) | `@0x80111c60/68` | cmd 1, 0 — Release |
| Dog-Devour-Reaktion LAB_80111cb0 (Hook B) | `@0x80111ea0` | `sw =7` — gefressen → cmd 7 |
| Krähe Dive FUN_801137fc | `@0x80113b00/28` | cmd 2, `facing+2` (HP−4); `@0x80113b48/6c` HP<0 → cmd 3, 0 |
| Krähe Peck-Grab FUN_80113c7c | `@0x80113e30/48` | **aca59=0, cmd 5** (HP−8) — zweite Substate-0-Quelle; `@0x80113f20/44` Tod → 3/0 |
| Krähe Strike FUN_80114484 | `@0x80114518/3c` | Tod → cmd 3, 0 |
| Krähe Rück-Peck FUN_80115130 | `@0x801152a0/ac` | **cmd 5, dir 1** |
| Krähen-Grab-Reaktion LAB_8011597c (Hook A) | `@0x80115b1c/30/40` | cmd 1, 0 — Release |
| Krähen-Wurf (Tail Hook A) | `@0x80115d04/10` | **cmd 4, dir 6** — Umwerfen (Fall-FSM §1.1) |
| Spider-Spit FUN_80116288 | `@0x80116390/3a4` | cmd 2, `facing+2` (HP−2) |
| Dog Leicht-Biss FUN_80118270 | `@0x80118484/9c`, Tod `@0x801184b4/bc` | cmd 2, `facing+2` (HP−6); 3/0 |
| Dog Schwer-Biss FUN_8011854c | `@0x801187d8`, **`@0x801187e8 addiu v0,v0,4; @0x801187f0 sb`**, Tod `@0x80118808/10` | cmd 2, **`facing+4` = Knockdown-Substates 4/5** (HP−12); 3/0 |
| Dog Fress-Attacke FUN_80118ddc | `@0x801191cc` | `sw =6` (cmd 6, dir 0; HP−600 `_ALL:9858`) |
| Gorilla Strike FUN_8011a44c | `@0x8011a7d4/ec`, Tod `@0x8011a804/0c` | cmd 2, `facing+2` (HP−12); 3/0 |
| Gorilla Rear-Up-Grab FUN_8011a960 | `@0x8011ac48/68` | cmd 5, dir = `func_0x8001a780(&player)` ∈ {0,1} |
| Gorilla-Grab-Reaktion LAB_8011c118 (Hook A) | `@0x8011c38c` | `sw =1` Release |
| Gorilla-Devour-Reaktion LAB_8011c3d4 (Hook B) | `@0x8011c584` | `sw =7` gefressen |

### 2.3 STAGE2 (Stichprobe, 21 Sites) + STAGE3-5-Muster, STAGE6

STAGE2 = identische Architektur: Shared-Vierergruppe `@0x801024d4/80102b14/80103b54/80104470` (computed \|5/\|6), `@0x80107a9c` 0x101, Opfer-FSM-Exits `@0x8010a4f8` (=1) / `@0x8010a740/48` (7/0), Gegner-Hits cmd 2 `facing+2` mit Tod 3/0 (`@0x8010d27c-0x8010e15c`, `@0x8011255c/70`, `@0x801129d4/e8`). **Kein einziger cmd-2-Write mit aca59<2.**

STAGE3/5 zusätzlich: **`sw aca58=0x205`** (`@0x80117c1c` STAGE3, `@0x80118430` STAGE5) = **cmd 5 mit dir 2** — die Krabbler-Grabs (Bein-Biss; beantwortet `bite_blood_fx.md` Offen-Punkt 3: der aca59≥2-Pfad der Grab-FSM inkl. Wund-Helper [2]/[3] und Knie-Blut), plus `0x202`-Wörter (cmd 2 dir 2 als Wort) und computed-dir-Grabs (`@0x801177d0`/`@0x80117fe4`). STAGE4: nur Standard-Muster. **STAGE6: 0 Stores** (deckt sich mit „STAGE6: NULL Wund-Sites", blood_decals §3.3).

**Negativ-Befund (Kernaussage):** In **keinem** Binary existiert ein Writer `cmd 2 + aca59 ∈ {0,1}`. Die EXE-„Schwer-Hit"-Handler `0x80035b70`/`0x80035ca8` sind **toter Code** (RE2-Erbe). aca59=0/1 entsteht ausschließlich mit cmd 5 (Grab vorne/hinten), cmd 6 (Devour), cmd 3/7 (Tod, dir geklärt 0) oder als Motion-Substate bei cmd 1/4.

---

## 3. (B) Der Wund-Dispatcher: Ort, Caller, Bedingungen

### 3.1 Er ist Phase 4 der Grab-Opfer-FSM

`0x8010a28c` (Hook-A-Entry): `lbu aca59; jalr 0x801201a8[aca59]` — alle 4 Einträge (`0x801201a8-b4`) → **`0x8010a2cc`**. Diese FSM dispatcht `aca5a` (<6, `@0x8010a2dc`) über `@0x8010017c`:

| aca5a | Handler | Tut |
|---|---|---|
| 0 | 0x8010a308 | Setup: Seiten-Flag `0x800acaf3 := (aca59∈{1,3})` `@0x8010a328-33c`; Clip `acae8 = Seite·3 (+8 falls aca59≥2)` `@0x8010a35c-380`; Yaw-Flip +0x800 bei vorne `@0x8010a3c4-d4`; `aca5a:=1` |
| 1 | 0x8010a404 | Struggle-Anim; `aca5a += anim_done` `@0x8010a440-454` |
| 2 | 0x8010a460 | Clip+1, `aca5a:=3` |
| 3 | 0x8010a4a4 | **Halte-Schleife** (kein Selbst-Advance — beide Savestates stehen hier: cmd-Wort 5/1/3) |
| **4** | **0x8010a4e8** | `aca5a:=5`; Clip+2 (Release-Stagger); Blut `0x80019700(0x1500, …)` `@0x8010a574` (aca59<2: Spieler-Bone 8; ≥2: Bone 3 Knie); dann **`@0x8010a580 lbu aca59 → @0x8010a5b0 jalr 0x801201b8[aca59]`** = Wund-Helper (`0x8010a1cc/a208/a244/a268` → FUN_80037edc; blood_decals §3.2) |
| 5 | 0x8010a5b8 | Ausklang-Anim; Yaw-Restore; **`sw 1 → aca58` `@0x8010a664`** (cmd/dir/Phase genullt), `+0x93 &= ~1` |

### 3.2 Bedingungen — wann er WIRKLICH läuft

1. Nur bei `aca58 == 5` (der cmd-5-Handler ist der einzige Caller der Hook-A-Einträge; Hook-Slots werden sonst nirgends gelesen — EXE+Overlay-weiter lw/addiu/ori/Pointer-Scan leer).
2. Nur wenn der **Gegner** `aca5a := 4` schreibt = **Mash-Release**. Devour (Zähler-Ablauf), Tod im Grab und Abbruch erreichen Phase 4 nicht.
3. **Genau einmal pro Grab** (Phase 4 setzt selbst sofort `aca5a := 5` `@0x8010a4f0-f8`).
4. `aca59` ist dabei die **Grab-Richtung** (0 vorne / 1 hinten / 2..3 Krabbler-Varianten) — daher Helper 0/1 = Torso-Panels vorne/hinten (0+10, 5+50, 7+50 bzw. 0+10, 4+50, 7+50).
5. Per-Typ-Ausnahmen: Dog/Krähe/Gorilla haben eigene Hook-Reaktionen; Dog-Grab und alle drei Devour-Reaktionen stempeln an eigenen Sites (§1.3-Tabelle), die generische Maschine läuft dort nicht.

### 3.3 Der cmd-6-Pfad (kein Stempel, der Kill)

Hook B generisch `0x8010a6b8`: Death-Bite-Clip 6/7, Blut-Paar `@0x8010a7a0`(0x1500)/`@0x8010a84c`(0x2000, Frame 0x37), **`HP := −1` `@0x8010a80c-814` (Frame 0x23)**, dann `aca58:=7, aca59:=0` `@0x8010a8ac/b4`. Kein `jal 0x80037edc` in 0x8010a6b8–0x8010a8c8 (Scan).

---

## 4. (C) Reconciliation: Nutzer-Save (3× Substate 0) vs. 15s-Grab-Test (0 Stempel)

**Die belegte Kette des Nutzer-Saves** (ROOM-Zombie grabt frontal, Spieler masht sich frei — dreimal):

```
Zombie-Girl +0x5=9 → FUN_8010f80c [case 0]:
  @0x8010f890  acbfc := self (Grabber-Registrierung)
  @0x8010f910  sb 5 → 0x800aca58        ; cmd 5
  @0x8010f928  sb 0 → 0x800aca59        ; dir 0 = FRONT  ← „Substate 0"
  @0x8010f930  sb 0 → 0x800aca5a
[case 2] +0x9c := 100 (Biss-Budget), +0x9e := 0x32 (Devour-Zähler)
[case 3] pro Frame: Budget −= 1 + 100·FUN_80037024()
         (Mash = Press-Edge Maske 0xF0F0 @0x80037030: lhu 0x800ac762; andi 0xf0f0)
  Budget<0 → @0x8010faec sb 4 → zombie+0x6  UND  @0x8010faf4 sb 4 → 0x800aca5a
→ nächster Spieler-Frame: cmd5-Handler → Hook A (0x13→LAB_8010c16c) → 0x8010a2cc Phase 4:
  @0x8010a574 Blut-Spawn; @0x8010a580-5b0 jalr 0x801201b8[0] = FUN_8010a1cc:
  FUN_80037edc(0,+10); (5,+50); (7,+50)
→ Phase 5 → aca58:=1.
```

3 Durchläufe ⇒ Panel 0: 30 (<120, unsichtbar), Panels 5/7: 150 ≥120 → Level 1 — **exakt der Save-Befund** (blood_decals §3.4). Für den Briefing-Raum läuft dieselbe Kette über den Shared-Grab FUN_80102548 (dir = State−3 ∈ {0,1}, Push `_ALL:753`, Biss zusätzlich −10/−5 HP) — gleiche Phase-4-Mündung.

**Warum der Test leer blieb:** Ohne Mash schreibt niemand `aca5a=4`. Nach 50 Frames ohne Tastendruck läuft der Devour-Zähler ab (`@0x8010fa64-74`) → State 0xb → cmd 6 → HP=−1: kein Phase-4-Durchlauf, kein Stempel. Beide Savestates (`mzd_blood_decals_hp30`, `mzd_death_cmd5_struggle`) zeigen live `aca58/59/5a = 5/1/3` = in der Halte-Phase 3 hängend — der Zustand, in dem der Test gemessen hat. Und selbst EIN überlebter Grab (+50) bleibt unter der 120er-Schwelle — sichtbar wird Blut erst ab dem dritten Release aufs selbe Panel (bzw. zweitem nach Vorschaden).

**Korrektur an blood_decals.md §3.1/3.2:** Der „Spieler-Hurt-Entry @0x8010a580" ist der **Grab-Release-Pfad**; die Labels „Substate 0/1 = schwer front/back" heißen richtig „**Grab-Richtung vorne/hinten**", [2]/[3] = Krabbler-Grab-Richtungen (nicht „Standard-Hit front/back" — die erreicht cmd 2 nie).

---

## 5. (D) PORT-IST und Einhäng-Plan

### 5.1 IST (code-verifiziert)

| Original-Mechanismus | Port-Ort | IST |
|---|---|---|
| cmd-Dispatch @0x80073f90 | — | keine Cmd-FSM (bekannt OPEN); funktional ersetzt durch `player->state` + Victim-FSM |
| FUN_80012d60 (cmd 2, Substate `facing+2`) | `re15_damage.c re15_player_take_damage` (:160) | Writes byte-true (state 2, sub 2/3, Tod 3) — **ABER stempelt zusätzlich `s_wound_helper[substate]` bei JEDEM Hurt-Entry (:203-210)**. Falsch doppelt: (a) das Original stempelt bei cmd 2 NIE über die Helper-Tabelle (§1.2), (b) Helper [2]/[3] sind Grab-Richtungen, nicht Hit-Substates. Der Code-Kommentar :204 („Dispatcher feuert beim Hurt-EINTRITT") ist widerlegt |
| Zombie-Steh-Biss Direkt-Stempel `@0x8010f360` | — | **FEHLT** (der Port feuert den Steh-Biss der Girl-FSM derzeit nicht; falls er kommt: Panel `byte[0x80121014+(aca59−2)·2+(rng&1)]` ∈ {5,7}/{4,6}, +0x32) |
| Grab-FSM FUN_80102548 | `enemy_ai_common.c re15_enemy_ai_live_grab` (:1066ff) | Byte-true (Impact −10, Biss −5/Zyklus, Budget 0x6e, Mash 1+5·mash, Devour-Handoff, Mercy 0x5a) ✓ |
| **Release-Wund-Stempel (Phase 4 `@0x8010a580`)** | Grab case 4 (:1227ff) / `re15_player_victim_throwoff` | **FEHLT** — der Port ruft beim Throw-off keinen Wund-Helper (und keinen Blut-Spawn, bite_blood_fx §4) |
| Devour HP:=−1 `@0x8010a814` @Frame 0x23 | `re15_player_victim_tick` | vorhanden (HP=−1 @0x23 lt. bite_blood_fx §4/D4) ✓ |
| Dog-Schwer-Biss Substates 4/5 (`@0x801187f0`) + EXE-Knockdown-Handler [4]/[5] | — | **FEHLT** (Knockdown-Klasse ungeportet; Port-Dog müsste `facing+4` schreiben) |
| Krähen/Gorilla/Dog-Hook-Spezialreaktionen (+ deren eigene Stempel-Sites §1.3) | Krähen-AI nutzt HP/cmd-Proxy | **FEHLT** (Wund-Stempel der Devour-Reaktionen) |

### 5.2 Fixes (jede Konstante mit Beleg)

- **F1 — Falsch-Stempel entfernen:** den `s_wound_helper`-Block aus `re15_player_take_damage` (:203-210) streichen. Byte-true stempelt der plain Hit **nichts** (Beweis §1.2: Byte-Scan 0x80035b70–0x80036700 ohne 0x80037edc/jalr; FUN_80012d60 ruft ihn ebenfalls nicht).
- **F2 — Release-Stempel einhängen:** im Victim-FSM-Übergang STRUGGLE→RELEASE (`re15_player_victim_throwoff`, dem Pendant von `aca5a:=4` `@0x8010faf4`/`_ALL:753`) `re15_wound_add` mit Helper[dir] rufen: dir 0 → (0,+10),(5,+50),(7,+50) `@0x8010a1cc`; dir 1 → (0,+10),(4,+50),(7,+50) `@0x8010a208`; dir 2/3 → (1,+50)/(2,+50) `@0x8010a244/a268`; Dispatch-Beleg `@0x8010a580-5b0` (`jalr 0x801201b8[aca59]`). NUR beim Release — Devour/Death stempeln nicht (§3.2/3.3). Zusammen mit dem Release-Blut-Spawn (bite_blood_fx D3, gleiche Site `@0x8010a574`).
- **F3 — Steh-Biss-Stempel (wenn der Port die Attack aktiviert):** an der Hit-Site, Panel aus der Byte-Tabelle `@0x80121014 = 05 07 04 06` (Index `(aca59−2)·2 + rng&1`), Amount 0x32 `@0x8010f360`; Gates HP≥0 und Script-Immunität +0x1e4==0 (FUN_8010f15c case 1; case 3 stempelt NICHT).
- **F4 — Knockdown-Klasse:** Dog-Schwer-Biss schreibt `sub_state = facing+4` (`@0x801187e8/f0`); die EXE-Handler [4]/[5] (`0x800360e8`/`0x8003644c`) sind eigene Umfall-FSMs — separates Port-Ticket (bis dahin darf der Port-Dog nicht still auf +2 degradieren, ohne es als Divergenz zu führen).
- **F5 — Nicht erfinden:** KEINE Writer für cmd-2-Substates 0/1 bauen — toter Code im Original (§2-Negativ-Befund).

**Verifikation:** Headless ROOM1140: (1) Grab + Mash → beim Throw-off exakt ein `re15_wound_add`-Triple (Panels 0/5/7 bzw. 0/4/7); 3 Wiederholungen → Panels 5/7 Level 1, Akku-Tabelle == mzd_blood_decals_hp30 (§3.4 blood_decals). (2) Grab ohne Mash → Devour, HP=−1, **kein** Stempel. (3) Plain Hit (Crow-Dive) → kein Stempel.

---

## 6. Offene Punkte (ehrlich OFFEN)

1. **Register-indirekte Store-Zensus:** Der Binär-Scan deckt nur absolute `lui 0x800b`-Stores ab. Bekannt indirekt: FUN_80012d60 (verifiziert §2.1). Generische Entity-Leaves (z.B. 0x8004fxxx) schreiben `+0x4/+0x5` über Entity-Pointer und träfen den Spieler nur, wenn `DAT_800ac784` auf ihn zeigt — nicht ausgeschlossen, nicht erschöpfend verfolgt. Nächster Weg: PCSX-Redux-Write-Watchpoint auf 0x800aca58/59 (Skill `re15-pcsx-watchpoint`).
2. **cmd-4-Familie** (Fall/Knockdown-Bewegung, aca59 4..9 `@0x80030cf0-0x8003141c`; Krähen-Wurf 4/6 `@0x80115d04/10`): Semantik der Phasen nicht disassembliert.
3. **Hook-Einträge für Typen 0x16/0x18/0x1c–0x1f** (registriert auf die generische FSM): welche STAGE1-Typen das konkret sind (0x1c–0x1f gelten laut Roster als unbenutzt) — Registrierung defensiv oder erreichbar?
4. **STAGE3/5-`0x205`-Writer** (Krabbler-Grab dir 2): Besitzer-Typ pro Site nicht namentlich zugeordnet (nur Region).
5. **Liegend-Devour-Einstieg** (FUN_80103b94, aca59 0x26/0x27 → `0x801201a8[38/39]`-Äquivalent): weiter offen (= bite_blood_fx Offen-Punkt 1).
6. **FUN_80107634-Familie** (cmd 5 mit dir aus Attacker+0x9f): Typ-Zuordnung offen.
