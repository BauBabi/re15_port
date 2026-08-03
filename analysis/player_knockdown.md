# Die Spieler-KNOCKDOWN-Klasse — cmd-2-Substates 4/5 (EXE 0x800360e8 / 0x8003644c) + die cmd-4-Plc-Familie

**Datum:** 2026-08-03 · **Status:** Original-Seite **byte-belegt** (Roh-Disasm + Live-Savestate), Port-IST code-verifiziert
**Quellen:** `info/Re1.5/PSX.EXE` (t_addr 0x80010000, off = 0x800+addr−0x80010000), `info/Re1.5/PSX/BIN/STAGE{1,3,4,5}.BIN` (@0x80100000 roh, off = addr−0x80100000), `RE_15_Quellcode_V2/`, `stage_saves/mzd_stage1_maggot.sav` + `mzd_stage1_maggot_heavy.sav` + `mzd_stage1_dog.sav` (Live-RAM), Schwester-Dossiers `analysis/player_hit_chain.md` (Cmd-Maschine), `analysis/bite_blood_fx.md` (§7.1 Typ-Frage), `analysis/blood_decals.md`.
**Werkzeug:** `re15_disasm.py` (dis/table/bytes), `re15_ss.py`/`re15_enemy_state.py` (Savestate-RAM).

---

## 0. Executive Answer

Die Knockdown-Klasse ist ein **eigenes Paar EXE-Phasen-FSMs** in der cmd-2-Hit-Tabelle `0x800741a8`: **[4] = 0x800360e8 (Treffer von VORN → Rückwärts-Sturz)** mit 12 Phasen über Sprungtabelle `@0x80010b88`, **[5] = 0x8003644c (Treffer von HINTEN → Vorwärts-Sturz)** mit 7 Phasen über `@0x80010bb8`. Beide: eigener Wurf-Impuls über das Walker-Feld `+0x8c` (1000 bzw. 500), Root-Motion per `FUN_800245d8(yaw_offset)`, Wand-Probe per `FUN_8001c2dc`, Clips **0xd/0xe/0xf/0x10/0xb-rückwärts** bzw. **0xc/0x10/0xb-rückwärts** aus der **PLD-Basis-Bank** (Paar A `+0x84/+0x16c`), Ende immer `aca58 := 1` (Steuerung zurück). Der Spieler ist die gesamte Zeit **unverwundbar** (`+0x93 |= 1` bis zum Exit) und liest **nie** das Pad.

**Typ-Korrektur (löst bite_blood_fx §7.1):** Der „Dog-Schwer-Biss" `FUN_8011854c` gehört **NICHT dem Hund**, sondern dem **0x27-Parking-Garage-BOSS**: Er ist Eintrag **[6] der Boss-ANIMATE-Tabelle `@0x80121428`** (Zweitlevel-Dispatch des Active-Handlers `0x80117254` auf `+0x5`), Tick-Root `0x80072bac[0x27] = 0x80116db8` (savestate-verifiziert). `mzd_stage1_maggot_heavy.sav` zeigt die Klasse LIVE: Spieler `aca58/59/5a = 2/4/6` (Knockdown-vorn, Wand-Slam-Phase, Clip 0xF, speed 1000), Angreifer = zwei Typ-0x27-Entities in `+0x5=6`. **Der Hund (0x20, Root 0x8010d7f8) löst die Klasse NIE aus.**

**Die cmd-4-Familie ist KEINE „Fall-FSM", sondern die Plc-Klasse** (SCD-geskriptete Spieler-Bewegung): Eintritt = SCD-Opcode **`Plc_dest` (0x40) @0x80041be4** (schreibt register-indirekt `+0x4=4, +0x5=mode, +0x6/7=0` — deshalb im absoluten Store-Zensus unsichtbar), Dispatch `@0x80073e30[aca59]`. Modes: 0–3 = Plc_motion-Clip-Executor (generisch `0x80050cb8ff`), **4 = Gehen zum Punkt, 5 = Rennen, 6 = Hinlegen/Halten (Clip 1→Loop 2), 7/8 = Rückwärtsgehen, 9 = Auf-der-Stelle-Drehen**, 10/11 = Clip+Footlock. Die **Krähe wirft den Spieler um, indem sie Mode 6 direkt als Attacke schreibt** (`@0x80115d04/10/18`: cmd 4, dir 6, Phase 0). Die Ankunfts-Pfade der Modes 4/5 enthalten einen **Original-Compiler-Bug** (falsches Basisregister → Flag-Set auf die unaligned Adresse 0x800B11DE + Phase-Reset in die Ziel-Koordinate statt `aca5a`).

---

## 1. Kontext: wie die Klasse erreicht wird (Beleg-Kette)

### 1.1 Dispatch (aus player_hit_chain.md, hier re-verifiziert)

Spieler-Task `FUN_80031c44` → `PTR_LAB_80073f90[aca58]` → cmd-2-Handler `0x80035af0` → `jalr 0x800741a8[aca59]` (`@0x80035b34-58`). Tabelle `@0x800741a8`: [4] = **0x800360e8**, [5] = **0x8003644c**.

### 1.2 Der einzige STAGE1-Auslöser: 0x27-Boss-Heavy-Biss `FUN_8011854c`

Connect-Block roh (STAGE1.BIN):

```
801187b4: lhu v0,0(s0)            ; s0 = 0x800acaee = player.hp
801187bc: addiu v0,v0,-12         ; HP -= 12
801187c0: sh v0,0(s0)
801187c4: ori v0,zero,0x2d
801187c8: jal 0x800453d0          ; Room-SE 5 (a0=5 @0x80118798)
801187cc: sh v0,476(v1)           ; boss+0x1dc := 0x2d Attack-Lockout
801187d0/d8: sb 2 -> 0x800aca58   ; cmd 2 (HIT)
801187dc: jal 0x8001a780          ; facing-Test (a0 = 0x800aca54 Spieler-Block)
801187e8: addiu v0,v0,4
801187f0: sb v0 -> 0x800aca59     ; aca59 = facing+4  ∈ {4,5}
801187f8: sb zero -> 0x800aca5a   ; Phase 0
801187fc: bgez v1(HP),0x80118814  ; HP<0 (signed)?
80118808/10: sb 3 / sb 0          ;   -> cmd 3 DEATH statt Knockdown
80118828: sb (|1) -> 0x800acae7   ; player+0x93 hit_react |= 1
```

`FUN_8001a780` liefert 0 = Angriff von vorn / 1 = von hinten (dieselbe Konvention wie die Grab-Richtung, hit_chain §2.2 Gorilla-Grab) → Substate **4 = vorn** (fällt rückwärts), **5 = hinten** (fällt vorwärts).

### 1.3 Typ-Zuordnung 0x27 (BOSS), nicht 0x20 (Dog) — vierfache Kette

1. **Pointer-Zensus STAGE1.BIN:** `0x8011854c` wird genau EINMAL referenziert — als Wort `@0x80121440` (file-off 0x21440). Nachbarn: `@0x8012143c → 0x80118270`, `@0x80121444 → 0x80118908`, `@0x80121448 → 0x80118ddc`.
2. **Boss-Root-Dispatch:** `0x80072bac[0x27]`-Tick-Root `0x80116db8` dispatcht `jalr @0x801213c8[ent+0x4]` (`@0x80116e00-20`); State-1-„active"-Handler `0x80117254` macht ZWEI `+0x5`-Dispatches: DECIDE `jalr @0x801213e8[+0x5]` (`@0x80117324-44`) und ANIMATE `jalr @0x80121428[+0x5]` (`@0x80117358-78`). **`0x80121428[6] = 0x8011854c`** (Heavy-Biss), `[5] = 0x80118270` (Leicht-Biss), `[7] = 0x80118908` (Leap), `[8] = 0x80118ddc` (Devour, cmd 6, HP−600). Die DECIDE-Einträge [5]/[6]/[7]/[8] (`0x80118268/0x80118544/0x80118900/0x80118dd4`) sind **leere Stubs** (`jr ra`, roh verifiziert @0x80118544).
3. **Tick-Tabelle live** (alle drei Savestates identisch): `0x80072bac[0x27] = 0x80116db8`, `[0x20] = 0x8010d7f8` (Dog), `[0x21] = 0x80112020` (Krähe). Die On-Disc-Tabelle ist ab Typ 9 genullt → **RAM-gepatcht vom Overlay** (deshalb greift nur der Savestate-Beweis).
4. **Live-Zustand `mzd_stage1_maggot_heavy.sav`:** Spieler `aca58/59/5a = 0x2/0x4/0x6`, Clip `+0x94 = 0xF` (Wand-Slam!), Frame 10, `+0x8c = 1000`, HP 82, `aca3c = 0x1d0` (Bits **0x40 UND 0x80** gesetzt — exakt die Handler-Writes §2); Angreifer: zwei Entities **Typ 0x27** in `+0x4=1, +0x5=6` (= ANIMATE[6] = Heavy-Biss läuft). Kein Hund im Raum.

**Korrektur an player_hit_chain.md §2.2:** die Zeilen „Dog Leicht-Biss FUN_80118270 / Dog Schwer-Biss FUN_8011854c / Dog Fress-Attacke FUN_80118ddc" sind **0x27-Boss-Substates 5/6/8**. Der Hund selbst hat in seiner Familie (0x8010d7f8–0x80112020) **keinen** cmd-2-Writer mit `facing+4` (Zensus §2.2 dort: einziger `facing+4`-Writer STAGE1 = `@0x801187e8`).

### 1.4 Analoge Auslöser in anderen Stages (roh verifiziert)

| Stage | Site | Bytes |
|---|---|---|
| STAGE3 | `@0x801122d0` cmd 2, `@0x801122e0/e8` `addiu v0,v0,4; sb → aca59`, `@0x801122f0` Phase 0, HP<0 → 3 | ✓ dis |
| STAGE4 | `@0x8010d970/84/88/90` identisches Muster | ✓ dis |
| STAGE5 | `@0x8010daf0/db00/db08/db10` identisches Muster | ✓ dis |

(Besitzer-Typen dieser Sites: nicht zugeordnet — OFFEN, §7.)

### 1.5 Toter Code [0]/[1]

Die „Schwer-Hit"-Handler `0x80035b70`/`0x80035ca8` (Clip 0xa) haben game-weit **keinen Writer** (hit_chain §2-Negativ-Befund) — die Knockdown-Klasse [4]/[5] ist der einzige lebende „Schwer-Treffer".

---

## 2. (A) Handler [4] `0x800360e8` — Knockdown von VORN (fällt rückwärts)

Entry: `lbu aca5a; sltiu 0xc` (`@0x800360f8`, Phase < 12) → `jr @0x80010b88[aca5a]` (`@0x80036104-11c`). Sprungtabelle `@0x80010b88` (file-off 0x1388):

| Phase | Code | Tut (jeder Store mit Adresse) |
|---|---|---|
| 0 | 0x80036124 | Setup Sturz: `aca5a=1` @0x80036134-38; **Clip `+0x94=0xd`** @0x80036144-4c; `+0x8f=7` (Blend) @0x80036150-58; Frame `+0x95=0` @0x80036164; **Speed `+0x8c=0x3e8` (1000)** @0x8003615c/6c; `+0x9e(acaf2)=0` @0x80036174; **`+0x93 \|= 1`** (unverwundbar) @0x80036178-80; **SE `0x80045024(0x04010001, player+0x34)`** @0x80036184-88; **`aca3c \|= 0x40`** @0x8003618c-a0 → fällt in Phase 1 |
| 1 | 0x800361a4 | Sturz-Flug: **`speed −= 15·anim_frame`** (`sll t0,4; subu; subu` @0x800361c8-e0, KEIN Clamp, u16-Store/s16-Konsum); anim `FUN_8001f314([0x800acad8],[0x800acbc0],a2=0,0x200)` @0x800361b4-e0 (= **PLD-Basis-Bank**, §4.1); `aca5a += ret` @0x800361e4-f8 (Clip-Ende → Phase 2); **Root-Motion `FUN_800245d8(0x800)`** @0x800361fc-200 (= rückwärts, facing+180°); **Wand-Probe `FUN_8001c2dc(player+0x34, hitbox[+6], &acaf2)`** @0x8003620c-18 (a1 = `lhu [0x800acacc]+6` = Kollisionsradius); `acaf2 != 0` → **`aca5a = 5`** (Wand-Slam-Zweig) @0x8003621c-30 |
| 2 | 0x8003623c | Setup Aufprall/Aufstehen: `aca5a=3` @0x8003624c-50; **Clip `0xe`** @0x80036254-5c; Frame 0 @0x80036264; `+0x8f=7` @0x80036268-70; SE `0x04010001` @0x80036274 → fällt in Phase 3 |
| 3 | 0x8003627c | anim (a2=0) @0x80036290; `aca5a += ret` @0x80036298-ac; Root-Motion rückwärts @0x800362b0-b4; **`speed −= 15`, Clamp ≥0** @0x800362bc-e0 |
| 4 | 0x800362ec | **EXIT (Normalweg): `aca58 = 1`** (sw) @0x800362f4; `+0x93 = 0` @0x800362f8-fc. ⚠ **`aca3c`-Bit 0x40 wird HIER NICHT gelöscht** (nur Exit [11] löscht 0xC0) — Original-Asymmetrie, byte-Fakt |
| 5 | 0x80036308 | **Wand-Slam**: `aca5a=6` @0x80036318-1c; **Clip `0xf`** @0x80036320-28; Frame 0/Blend 7 @0x80036334-3c; SE @0x80036340-44; **`aca3c \|= 0x80`** @0x80036348-5c → tail |
| 6 | 0x80036360 | anim weiter (a2=0), Ende → 7 |
| 7 | 0x80036368 | `aca5a=8`; **Clip `0x10`** (bäuchlings aufkommen) @0x80036374-7c; Frame 0/Blend 7 @0x80036380-90 |
| 8 | 0x80036394 | anim weiter, Ende → 9 |
| 9 | 0x8003639c | `aca5a=0xa`; **Clip `0xb` RÜCKWÄRTS** @0x800363a8-b0 (fällt in 0x800363c8 `ori a2,1`) — Aufstehen = Kollaps-Clip revers |
| 10 | 0x800363c8 | anim revers weiter (a2=1), Ende → 11 |
| 11 | 0x80036404 | **EXIT (Wandweg): `aca58 = 1`** @0x8003640c; `+0x93 = 0` @0x80036420; **`aca3c &= ~0xC0`** @0x80036410-34 (`andi`-Paar −129/−65) |

Gemeinsamer anim-tail @0x800363cc-f8: `f314([acad8],[acbc0],a2,0x200)`; `aca5a += ret`.
**Kein Pad-Read, kein HP-Check, kein Blut-Spawn, kein Wund-Stempel** in 0x800360e8–0x80036448 (deckt sich mit bite_blood_fx §3.2-Zensus).

**Physik zusammengefasst:** Impuls 1000 rückwärts; Verzögerung quadratisch (pro Tick −15·Frame — Formel läuft bei langen Clips ins Negative, u16/s16-Roh-Verhalten, kein Clamp in Phase 1); am Boden (Phase 3) linear −15/Tick mit Clamp 0, rutscht weiter rückwärts. Wand-Kontakt bricht in den Slam-Zweig (Bewegung stoppt dort komplett — Phasen 5-10 haben KEIN `pos_advance`). Savestate-Kreuzcheck: heavy-Save hängt in Phase 6 mit speed noch 1000 → die Wand wurde am Anfang des Flugs getroffen (kein Decel gelaufen), konsistent.

---

## 3. (A) Handler [5] `0x8003644c` — Knockdown von HINTEN (fällt vorwärts)

Entry: `sltiu 0x7` (`@0x8003645c`) → `jr @0x80010bb8[aca5a]` (`@0x80036468-80`). Tabelle `@0x80010bb8` (file-off 0x13b8):

| Phase | Code | Tut |
|---|---|---|
| 0 | 0x80036488 | Setup: `aca5a=1` @0x80036498-9c; **Clip `0xc`** @0x800364a8-b0; `+0x8f=7` @0x800364b4-bc; **Speed `0x1f4` (500)** @0x800364c0-d0; Frame 0 @0x800364c8; `acaf2=0` @0x800364d4-d8; **`acaf3(+0x9f)=0`** @0x800364dc-e0; `+0x93 \|= 1` @0x800364e4-ec; SE `0x04010001` @0x800364f0-f4; `aca3c \|= 0x40` @0x800364f8-50c → Phase 1 |
| 1 | 0x80036510 | anim (a2=0) @0x80036524; `aca5a += ret` @0x80036534-4c; **Gate `anim_frame < 0xf`** @0x8003653c-50 (ab Frame 15: keine Bewegung mehr, direkt Epilog); Verzögerung **`speed −= 5·t; t++`** (t = `+0x9f`; `sll t,2; addu` = 5·t @0x8003656c-8c); Wand-Probe @0x80036594 (Args wie [4], a2=&acaf2 @0x80036598); Treffer → **`speed = 0`** @0x8003659c-b0 (KEIN Slam-Zweig!); **Root-Motion `FUN_800245d8(0)`** = VORWÄRTS @0x800365b4-b8 |
| 2 | 0x800365c4 | `aca5a=3` @0x800365c4-cc; **Clip `0x10`** @0x800365d0-d8; Frame 0/Blend 7 @0x800365e8-f4; **`aca3c \|= 0x80`** @0x800365dc-600 (kein SE) |
| 3 | 0x80036604 | anim weiter (a2=0), Ende → 4 |
| 4 | 0x8003660c | `aca5a=5`; **Clip `0xb` RÜCKWÄRTS** @0x80036618-20 (fällt in `ori a2,1` @0x80036638) |
| 5 | 0x80036638 | anim revers weiter, Ende → 6 |
| 6 | 0x80036674 | **EXIT: `aca58 = 1`** @0x80036678; `+0x93 = 0` @0x80036690; `aca3c &= ~0xC0` @0x80036680-a4 |

Asymmetrien zu [4]: halber Impuls (500), dreieckige Verzögerung (−5·t kumulativ), Bewegung nur die ersten 15 Clip-Frames, Wand = Stopp statt Slam, EIN fester Clip-Weg (0xc → 0x10 → 0xb-rev), nur EIN SE.

---

## 4. Gemeinsame Infrastruktur (byte-belegt)

### 4.1 Die drei Anim-Quellen-Paare des Entity-Structs

`FUN_80022300` (EMS-Loader, generisch) belegt pro Entity DREI EMR/EDD-Paare: `+0x84/+0x16c` (A), `+0x170/+0x174` (B), `+0x178/+0x17c` (C) (Decompile Z.32-37). Für den **Spieler**:

| Paar | Adressen | Quelle | Wer nutzt es |
|---|---|---|---|
| **A** | `0x800acad8/0x800acbc0` | **PLD-Basis** `dir[1]/dir[0]` (EMR/EDD) — `FUN_800314b0` Z.20-23 (`DAT_800acbc0 = base+dir[0]`, `DAT_800acad8 = base+dir[1]`) | **Knockdown [4]/[5]** (alle f314-Calls), Standard-Hit [2]/[3] (Clip 8/9), cmd-4-Modes 7/8 |
| **B** | `0x800acbc4/0x800acbc8` | **Waffen-PLW** `dir[1]/dir[0]` — `FUN_80036b68` Z.12-13 (Tabelle `@0x800741e8[char]+weapon`) | Normal-Steuerung/Health-Idle (`0x80033440-44`), cmd-4-Modes 4/5/9, generischer Mode 6 (`ent+0x170/+0x174` @0x80051884-88) |
| **C** | `0x800acbcc/0x800acbd0` | Victim-Bank des Grabbers (EMS) | Krähen-Opfer-FSM (`@0x80115a5c-68`), Grab/Devour (Port kennt sie als skel_victim/anim_victim) |

Die Knockdown-Clips sind also **PLD-EDD-Indizes** (waffenunabhängig): 0x8=Hit hinten, 0x9=Hit vorn (Port-Mapping existiert), 0xa=toter Schwer-Hit, **0xb=Kollaps (revers = Aufstehen), 0xc=Sturz vorwärts, 0xd=Sturz rückwärts, 0xe=Aufprall+Aufstehen (Rücken), 0xf=Wand-Slam, 0x10=bäuchlings Aufkommen** (Struktur byte-belegt; die visuellen Namen sind Interpretation der Sequenz-Reihenfolge, nicht einzeln gerendert-verifiziert).

### 4.2 `FUN_8001f314` (anim_set) — der Rückwärts-Flag

Decompile Z.9-14: `param_3 == 0` → Frame = `+0x95` (vorwärts); **`param_3 != 0` → Frame = `len − 1 − (+0x95)`** = **RÜCKWÄRTS-Playback**. Genau so spielen [4]-Phase 9/10 und [5]-Phase 4/5 den Clip 0xb als Aufstehen. Rückgabe ≠ 0 = Clip-Wrap (bekannt aus Grab-FSM) → Phasen-Advance.

### 4.3 `FUN_800245d8(yaw_offset)` — Root-Motion

Rotiert `(speed = *(s16*)(ent+0x8c), 0, 0)` mit `RotMatrixY(ent+0x6a + yaw_offset)` und addiert IR1/IR3 auf `+0x34/+0x3c` (Decompile Z.33-46). `0x800 = 180°` = rückwärts. `+0x8c` des Spielers = `0x800acae0`.

### 4.4 `FUN_8001c2dc(pos, radius, out)` — Wand-/Boden-Probe

Bandweise `FUN_8003b7f0(pos, radius, band)`-Schleife ab `−pos.y/0x708`; `*out = 1` bei Kollisionsbit 1 (Decompile Z.27-39); Rückgabewert (Bodenhöhe −band·1800) wird von den Knockdown-Handlern ignoriert — nur das Flag `+0x9e` zählt. Radius-Quelle: `lhu +6` des Hitbox-Records `[player+0x78]` (= radius_min, Layout aus re15_enemy_state.py).

### 4.5 `+0x8f = 7` = Anim-Blend-Zähler (Port: `anim_frac`)

Jeder Clip-Wechsel schreibt 7; live steht das Feld auf 0 (beide Savestates) → wird vom Anim-System heruntergezählt. Der Port modelliert genau das bereits (`anim_frac = 7`, Dekrement enemy_ai_common.c:754).

### 4.6 `aca3c`-Bits 0x40/0x80

Gesetzt: 0x40 bei Knockdown-Entry (beide Handler), 0x80 bei Wand-Slam ([4] Ph.5) bzw. Bauchlage ([5] Ph.2); gelöscht `&= ~0xC0` nur an den End-Exits [4]-Ph.11/[5]-Ph.6 ([4]-Ph.4 löscht NICHT). Live bestätigt (heavy-Save: `aca3c = 0x1d0`). Auch der Krähen-Opfer-Hook setzt `\|= 0xC0` (`@0x80115a3c-44`), und der Eaten-Death ebenso (Port-Kommentar enemy_ai_common.c:4487). **Konsument der Bits = OFFEN** (§7) — nicht raten.

### 4.7 SE

`FUN_80045024(0x04010001, player+0x34)` = positionsbehaftetes SE Bank 4 / Rec 1 (dieselbe Site-Familie wie Grab-Release, bite_blood_fx §2.3) an: [4] Ph.0/2/5, [5] Ph.0. Der Attack-Site-SE des Boss-Bisses ist getrennt: Room-SE 5 `@0x801187c8`.

---

## 5. (B) Die cmd-4-Familie = die Plc-Klasse (SCD-geskriptete Spieler-Bewegung)

### 5.1 Handler-Kopf `0x80030660`

- Gate `0x800acc18` (= player+0x1c4): Bit 0x10 gesetzt UND Parity-Bit 0x20 gesetzt → Dispatch übersprungen (Half-Rate: `acc18 ^= 0x20` pro Frame @0x800306bc-c8).
- Dispatch **`jalr @0x80073e30[aca59]`** (`@0x80030688-ac`) — **OHNE Bounds-Check**.
- Footlock-Tail: `acc18 & 1` → `FUN_800369f8(0,0)`, `& 2` → `FUN_800369f8(0,1)` (@0x800306cc-f8) = Locator-Bone-Root-Motion (Matrix-Kette in einen Bone, Position += Bone-Delta; Decompile Z.11-24; param_1≠0-Variante schreibt stattdessen `+0x8c = |delta|`).

### 5.2 Eintritt: SCD-Opcode `Plc_dest` (0x40) @0x80041be4 — register-indirekt

```
80041c14: sb 4,4(a1)      ; entity+0x4 := 4   (a1 = Work-Entity; Spieler: 0x800aca58!)
80041c18: sb a2,5(a1)     ; +0x5 := mode (pc[2])
80041c1c/20: +0x6 := 0, +0x7 := 0
80041c24: sb v1,451(a1)   ; +0x1c3 := Flag-Index (pc[3])   (Spieler: 0x800acc17)
80041c38: sh X,444(a1)    ; +0x1bc := Ziel-X               (Spieler: 0x800acc10)
80041c4c: sh 0,452(a1)    ; +0x1c4 := 0 (Flags)
80041c58: sh Z,446(a1)    ; +0x1be := Ziel-Z               (Spieler: 0x800acc12)
80041c5c-84: Typ<0x10: mode-4 < 6 -> jr @0x80010dcc[mode-4]  ; Speed-Setup +0x1c8
              (Byte-Tabellen @0x80073ea5 / @0x80073f25; Mode 6 -> 0x80041e38 = KEIN Setup)
```

Re-Init-Guard @0x80041bf8-c0c: läuft `+0x1c4 & 4` und `+0x5 == mode` schon → nur Ziel-Update. **Das erklärt, warum der aca58-Zensus keinen cmd-4-Writer fand: der Store ist entity-relativ.** Live-Beweis: `mzd_stage1_maggot.sav` = Spieler in `aca58/59/5a = 4/5/2` (Boss-Intro-Lauf), Ziel `acc10/12 = (−15893, −6270)`, Spieler (−15897, −5707) auf dem Weg, speed 200, Clip 0.

### 5.3 Substate-Tabelle `@0x80073e30` (Modes)

| Mode | Handler | Semantik (belegt) |
|---|---|---|
| 0–3 | 0x80050cb8 / 0x80050ddc / 0x80050f00 / 0x80051024 | **Plc_motion-Clip-Executor** (generisch, `ent+0x6`-Phasen 0→1→2, anim aus B; scan-verifiziert; = die NPC-State-4-Executor-Familie um 0x80050be8) |
| **4** | 0x80030af0 | **GEHEN zum Punkt**: Phase 0: Clip 5 (Bank B), speed 75 (0x4b @0x80030b20/44); Phase 1: `arc_test(acc10,acc12,0x15e)`-Ausrichtung → Phase 2, Turn `0x8001aac4(rate 0x60)`, Schritt-SE `0x80045630` (Gate `[0x800acbbc]&0x4000`, Fuß aus Bit 0x1000); Phase 2: Turn rate 0x30, `245d8(0)`, **Ankunft dist<100** (SquareRoot0 @0x80030cbc-c4) → Flag-Set + **`aca59 := 6`** @0x80030cf0; `acc18&4` → re-arm Mode 4 @0x80030d00-10 |
| **5** | 0x80030d28 | **RENNEN zum Punkt**: speed 200 (0xc8 @0x80030d58/7c), Clip 5 → bei Ausrichtung **Clip 0** + Blend 7 (@0x80030dd0-e8), Turn 0x60/0x48, **Ankunft dist<300** @0x80030f18 → wie [4] mit re-arm Mode 5 |
| **6** | 0x800517f0 | **HINLEGEN/HALTEN (generisch, `ent+0x6`-Phasen)**: Ph.0: **Clip 1** (aus `ent+0x170/+0x174` = Paar B) @0x80051844-74; Ph.1: anim → Ph.2; Ph.2: **Clip 2** @0x800518b4-d8; Ph.3: anim OHNE Advance = **Endlos-Loop/Halten** @0x800518dc-f4. = Ziel des Krähen-Wurfs UND Ankunfts-Folgezustand aller Geh-Modes |
| **7** | 0x80031080 | **RÜCKWÄRTS-Gehen zum Punkt**: Clip 0, speed 70 (0x46 @0x800310a0-a8), Turn rate **−48**, `245d8(0x800)`; Ankunft dist<100 → Flag-Set Basis **0x800B0028** (s0=aca5a! @0x80031194) + `aca59:=6`, **`aca5a:=0`** @0x800311b0 (KORREKTE Codierung); `acc18&4` → re-arm 7 |
| **8** | 0x800311f0 | Variante von 7 (identischer Body, eigenes re-arm 8 @0x8003133c) |
| **9** | 0x80031360 | **DREHEN auf der Stelle** zum Punkt: Clip 5, `arc_test(cone 0x60)`; ausgerichtet → Flag-Set Basis **0x800B1028** (@0x800313fc; = das Krähen-/Script-Bitfeld, Port `s_crow_gflags`!) + `aca59:=6`, `aca5a:=0` @0x800313f4; anim aus **B** @0x80031484-94 |
| 10/11 | 0x80051ebc / 0x80051fa8 | Clip-Play + Footlock (`jal 0x800369f8` im Body; scan) |
| 12+ | 0x80052038 … | weitere generische Executor (nicht weiter verfolgt) |

Modes 4/5 animieren aus **B** (`[0x800acbc4]/[0x800acbc8]` @0x80030be8-f8), Modes 7/8 aus **A** (`[acad8]/[acbc0]` @0x80031130-40) — byte-Fakt, Grund unklar.

Der Ankunfts-Flag-Setter `FUN_8004ef90(base, idx)` = MSB-first-Bit-Set `base[idx>>5] |= 0x80000000>>(idx&0x1f)` (roh @0x8004ef90-efb4).

### 5.4 ⚠ Original-BUG in den Ankunfts-Pfaden der Modes 4/5

In [4]/[5] wurde s0 auf `0x800acc10` (Ziel-Block) umgeladen (`@0x80030b70-74` / `@0x80030da8-ac`, Bytes `3c10800b 2610cc10` verifiziert), der Ankunfts-Code aber gegen s0=`0x800aca5a` geschrieben (wie in [7]/[8], dort korrekt):

```
80030f2c: jal 0x8004ef90
80030f30: addiu a0,s0,17870   ; a0 = 0x800acc10+0x45CE = 0x800B11DE  (UNALIGNED lw!)
80030f50: sb zero,0(s0)       ; byte[0x800acc10] := 0  (gemeint: aca5a := 0)
80030f64: sb 2,0(s0)          ; byte[0x800acc10] := 2  (gemeint: aca5a := 2, re-arm)
```

Folgen (statisch): (a) `aca5a` wird bei Ankunft NICHT genullt → Mode 6 startet in Phase 2 (überspringt Clip 1, hält sofort Clip 2); (b) das Ziel-X-Low-Byte wird zerschrieben; (c) der Ankunfts-Flag-Set trifft die unaligned Adresse `0x800B11DE` — auf realer Hardware ein AdEL-Trap, im DuckStation-Recompiler eine maskierte Schreibstelle **mitten im Wund-Decal-Block** (`0x800b10ec`-Familie, blood_decals). Der maggot.sav stand 3 Frames vor genau dieser Ankunft und das Spiel lief weiter (heavy-Save existiert danach) → in der Emu-Praxis kein Crash. **Laufzeit-Wirkung im Detail = OFFEN** (§7). Für den Port gilt: das Verhalten der KORREKTEN Codierung ([7]-Muster) ist die Intention, der Bug ist zu dokumentieren, nicht zu emulieren — es sei denn, Parität verlangt es (Entscheidung beim Port-Ticket).

### 5.5 Der Krähen-Wurf (der einzige ATTACK-Eintritt in cmd 4)

Krähen-Opfer-Hook `LAB_8011597c` (Hook A Typ 0x21) dispatcht `@0x8012123c[aca59]`; die Opfer-FSM phasiert über `@0x801002ec[aca5a]` und animiert Leon aus **Paar C** (`[0x800acbcc]/[0x800acbd0]` @0x80115a5c-68); Phase 0 setzt `aca3c |= 0xC0` @0x80115a3c-44. Der Wurf-Tail:

```
80115cf0: jal 0x8001f314          ; Wurf-Clip zu Ende?
80115cf8: beq v0,zero,0x80115d1c
80115d04: sb 4  -> 0x800aca58     ; cmd 4
80115d10: sb 6  -> 0x800aca59     ; Mode 6 (Hinlegen/Halten)
80115d18: sh 0  -> 0x800aca5a     ; Phase 0 (Halfword nullt aca5a+aca5b)
```

→ Leon spielt PLW-Clip 1 und hält dann PLW-Clip 2 im Loop. **Wer danach `aca58 := 1` zurückschreibt (Aufstehen nach Krähen-Wurf), ist statisch NICHT gefunden** — Mode 6 Phase 3 hat keinen Selbst-Exit; die cmd-1-Restores (`@0x8004672c` SCD/Event, Krähen-Hook `@0x80115b1c/30/40` — letztere laufen nur bei cmd 5) erklären es nicht. **OFFEN** (§7, Watchpoint-Plan).

### 5.6 Abgrenzung: der `aca52`-„Knockdown-Latch" des Ports ist falsch verankert

Port-Kommentar (enemy_ai_common.c:3062-3066, Audit wf_827f186d crow #A) nennt als Setter des Krähen-Sturzflug-Gates `DAT_800aca52|=1` „die EXE-Knockdown-Handler @0x800334e8-0x80033504 / @0x800345c8". Byte-Fakt: **die Knockdown-Handler 0x800360e8/0x8003644c schreiben aca52 NIE** (voller Disasm §2/§3). `@0x800334e8-0x80033504` liegt in `0x80033460` = **Phase 2 (via `@0x800740f4[aca5a]`, Dispatch @0x80032eb4) des cmd-1-Substate-Executors `0x80032e9c`** (cmd-1-Substates ≥3 via `@0x80074030[aca59]`; die Door/Climb-Folge-Cmds 0x301/0x401/0x701 landen genau dort). Diese Phase setzt zusätzlich `0x800acc24 := 0x5a` und wählt einen Health-Tier-Halte-Clip `7 + 2·bit15(acaec) + 4·bit13(acaec)` ∈ {7,9,11,13} aus Bank **B** mit per-Waffen-Dispatch `@0x80074100[weapon]`. Der Latch bedeutet also „Spieler steht/verharrt nach geskripteter Bewegung", nicht „Spieler liegt am Boden". **Das Krähen-Dive-Gate im Port braucht ein Re-Audit** (§6 F5, §7).

---

## 6. (D) PORT-IST und Einhäng-Plan

### 6.1 IST (code-verifiziert)

| Original | Port-Ort | IST |
|---|---|---|
| cmd-2 [2]/[3] Standard-Flinch (Clip 8/9, kb 200, −50/Frame) | game_step_common.c HP-Drop-Detector (:214-245) + Flinch-Branch (:300-318) | ✓ portiert (byte-true zitiert) |
| **cmd-2 [4]/[5] Knockdown-Klasse** | — | **FEHLT komplett** (kein Zustand, keine Clips 0xb-0x10, kein Rückwärts-Playback, kein Wand-Slam) |
| 0x27-Heavy-Biss-Connect (HP−12, `facing+4`) | enemy_ai_common.c maggot case 6 (:5592-5613) | **TEILWEISE**: Clip 0x13, Fenster {0x15-0x18,0x21,0x22}, Dual-Sphären, HP−12, SE 5, Lockout 0x2d ✓ — aber Connect schreibt nur `hit_react\|=1` („port convention"); der HP-Drop-Detector degradiert die Reaktion zum **Standard**-Flinch (Clip 8/9, kb 200) statt Knockdown (Clip 0xd/0xc, kb 1000/500, Boden, Aufstehen, ~2-3× längere Wehrlosigkeit + i-Frames) |
| Krähen-Wurf (Opfer-FSM Paar C → cmd 4/6/0) | enemy_ai_common.c crow case 13 → sub 14 (:3589-3634) | **FEHLT**: Release lässt den Spieler einfach weiterlaufen (nur Krähe dreht ab) |
| cmd-4-Plc-Klasse | scd_vm.c op_plc_dest (:1861ff) modelliert Stash+walk_active; keine Spieler-FSM | teilweise (eigene Baustelle, nicht dieses Ticket) |
| `FUN_8001a780` (facing 0/1) | `re15_maggot_a780` (enemy_ai_common.c:5332) | ✓ vorhanden — direkt als Richtungs-Selektor nutzbar |
| `+0x8f`-Blend | `anim_frac` (Setzen 7 + Dekrement :754) | ✓ vorhanden |

### 6.2 Einhäng-Plan (jede Konstante trägt ihre Adresse aus §2/§3)

- **F1 — Knockdown-Zustand einbauen** (game_step_common.c, parallel zu `s_hit_flinch`): Eigener Player-Reaktionszustand `knockdown(dir, phase)` mit exakt den Phasen/Stores aus §2/§3. Trigger im Boss-Case 6 (:5605-5613): statt nur `hit_react|=1` → `dir = re15_maggot_a780(e, pl)` (Original `@0x801187dc`) und Knockdown-Klasse starten (`facing+4` `@0x801187e8/f0`); HP<0 → Death-Pfad (`@0x801187fc-8810`) hat Vorrang. Kern-Konstanten: Impuls 1000 `@0x8003616c` / 500 `@0x800364d0`; Decel `−15·frame` `@0x800361c8-e0` (ohne Clamp), `−15` Clamp 0 `@0x800362bc-e0`, `−5·t` `@0x8003656c-8c` mit Move-Gate frame<15 `@0x80036544`; Richtung rückwärts 0x800 `@0x80036200` / vorwärts 0 `@0x800365b8`; Wand-Probe = vorhandene Port-Kollision (Radius `[+0x78]+6` `@0x80036210`), vorn: Treffer → Slam-Zweig `@0x8003622c`, hinten: speed=0 `@0x800365ac`; Clips/Sequenzen exakt wie §2/§3 (0xd→0xe bzw. 0xd→0xf→0x10→0xb-rev bzw. 0xc→0x10→0xb-rev); Clip-Längen aus PL00.EDD lesen (wie der Port es für 0x8/0x9/0xa schon tut, game_step_common.c:236); SE Bank4/Rec1 an den §4.7-Sites; i-Frames = `hit_react` gesetzt lassen bis Exit (`@0x80036178`/`@0x800362f8`/`@0x80036420`/`@0x800364e4`/`@0x80036690`).
- **F2 — Rückwärts-Playback**: Der Port-Anim-Pfad braucht die f314-a2=1-Semantik `frame = len−1−n` (Decompile FUN_8001f314 Z.13) für Clip 0xb.
- **F3 — Krähen-Wurf**: Beim Release (crow case 13 → 14) die Opfer-Kette portieren: Wurf-Clip aus der Victim-Bank (Paar C — Infrastruktur existiert), danach der cmd-4/6-Liegend-Halt (`@0x80115d04-18`; Mode-6-Clips 1→2 aus Paar B `@0x80051854/0x800518c8`). **ERST den Exit RE'en** (§7 Punkt 1) — den Halt ohne belegten Ausgang zu portieren erzeugt einen Softlock; bis dahin bleibt der Wurf als dokumentierte Divergenz offen, NICHT still auf Flinch degradieren ohne Vermerk.
- **F4 — Dossier-/Label-Korrekturen**: player_hit_chain.md §2.2 „Dog Leicht-/Schwer-Biss/Fress-Attacke" → 0x27-Boss ANIMATE[5]/[6]/[8] (Beleg §1.3); bite_blood_fx §2.5/§7.1 entsprechend schließen.
- **F5 — Krähen-Dive-Gate re-auditieren**: `g_aca52_flags`-Producer ist NICHT die Knockdown-Klasse (§5.6) — bevor F1 landet, prüfen, was das Original-Gate `aca52&1` semantisch wirklich schaltet (sonst divergiert das Dive-Verhalten, sobald der Port echte Knockdowns hat).

**Verifikation:** (1) ROOM11C0-Headless/Parity: Boss-Heavy-Connect vorn → Spieler-Sequenz Clip 0xd→0xe, kb-Positions-Trace == Formel; mit Wand im Rücken → 0xd→0xf→0x10→0xb-rev (Savestate-Vergleich gegen mzd_stage1_maggot_heavy: Phase 6, Clip 0xF, speed 1000). (2) Während des gesamten Knockdowns keine weitere HP-Änderung (i-Frames). (3) Ende: Steuerung zurück (aca58=1-Äquivalent), hit_react 0.

---

## 7. Offene Punkte (ehrlich OFFEN)

1. **Exit des Krähen-Wurf-/Mode-6-Halts** (wer schreibt danach `aca58 := 1`): statisch nicht gefunden; Mode 6 Phase 3 hält endlos. Nächster Weg: `re15-pcsx-watchpoint` auf 0x800aca58 während eines live gemashten Krähen-Grabs (ROOM10C0/1120), alternativ SCD-Threads der Krähen-Räume auf Plc-/Restore-Opcodes walken.
2. **Laufzeit-Wirkung des Ankunfts-Bugs der Modes 4/5** (unaligned `0x800B11DE`, DuckStation-vs-Hardware, was genau im Wund-Block landet): nur statisch belegt; Watchpoint/RAM-Diff um den Ankunftsframe des Boss-Intro-Laufs.
3. **Konsument der `aca3c`-Bits 0x40/0x80**: Setter/Clear vollständig kartiert, Leser nicht identifiziert (Kandidaten: Kamera/Schatten/Render-Gates; FUN_800396fc maskiert Low-16 beim Room-Load). Nicht raten.
4. **Semantik des `aca52`-Bit-0-Latches** (Producer = cmd-1-Executor-Phase `0x80033460`, mit `acc24=0x5a`-Timer): was das Krähen-Gate wirklich abfragt (steht-still? nach-Tür? verwundbar?) — Voraussetzung für F5.
5. **Visuelle Clip-Identitäten** 0xb-0x10 (PLD) und 1/2/5/0 (PLW): Sequenz-Logik belegt, Sichtprüfung (Render-Katalog oder gdigrab am Original) steht aus.
6. **Besitzer-Typen der STAGE3/4/5-`facing+4`-Sites** (§1.4): Muster verifiziert, Typ-Zuordnung je Overlay offen (gleicher Weg wie §1.3: Pointer-Zensus + Tick-Tabelle im Savestate).
7. **Flag-Array `0x800B0028`** (Ankunfts-Bits der Modes 7/8) — Identität unklar (0x800B1028 ist das bekannte Script-/Krähen-Bitfeld; 0x800B0028 hat noch keinen benannten Leser).
8. **Boss-DECIDE-Tabelle `@0x801213e8`**: nur die für die Typ-Frage nötigen Einträge geprüft ([5]-[8]-Stubs); Rest der 12+ Einträge unkartiert.
