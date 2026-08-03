# Biss-Blut-Effekte (Zombie-Grab / Devour / Krähe / Lunge) — Original-Spawns, Port-IST, Fix-Plan

**Datum:** 2026-08-02 · **Status:** Original-Seite **byte-belegt** (Disasm + Savestate), Port-IST code-verifiziert
**Quellen:** `info/Re1.5/PSX/BIN/STAGE1.BIN` (@0x80100000, roh, off = addr−0x80100000), `info/Re1.5/PSX.EXE` (t_addr 0x80010000, off = 0x800+addr−0x80010000), `RE_15_Quellcode_Overlays/STAGE1_full/FUN_*.c` (jeweils roh-disasm-gegengeprüft), `stage_saves/mzd_death_cmd5_struggle.sav` (Spieler IM Grab, cmd-Wort 0x00030105), `stage_saves/mzd_stage1_briefing.sav`/`mzd_stage1_engage_live.sav` (Bone-Anatomie), `RE15_ESP_ROWMACHINE.md` (Spawner-ABI), `analysis/blood_decals.md` (Schwester-Dossier Wund-DECALS).
**Nutzer-Report:** „wenn Leon gebissen wird, fehlt der Blut-Effekt, den das Original zeigt."

---

## 0. Executive Answer

Der fehlende Biss-Blut-Effekt ist **kein einzelner Spawn, sondern vier**: (1) das Original spawnt **pro Biss-Zyklus** des stehenden Grabs einen Blut-Effekt `0x1500` am **Zombie-Bone 10** (Girl: 11) — `@0x80102818`; (2) beim **Abschütteln** (Grab-Release) einen `0x1500` am **Spieler-Bone 8 = Hals/Kopf** — `@0x8010a574`, unmittelbar gefolgt vom Wund-DECAL-Stempel-Dispatch (löst gleichzeitig Open-Punkt 1 aus `blood_decals.md`: Hurt-Substate 0/1 = Grab vorne/hinten); (3) beim **Devour-Kill** (cmd 6) ein Paar `0x1500`(Start) + `0x2000`(Frame 0x37) am Spieler-Bone 8 — `@0x8010a7a0`/`@0x8010a84c`; (4) die **Krähe** spawnt bei jedem Kontakt Krähen-Bone-2-Blut (Dive: 6-Frame-Burst `ctr<<11` = 0x3000…0x800 `@0x80113bb0`; Peck-Grab: `0x2000` pro Zyklus `@0x80113f6c/fe4`; Strike: `0x1000` `@0x801144e0`). Der Port hat **keinen** dieser Spawns (nur den Frame-0x37-Burst, aber an der Spieler-WURZEL mit halber Größe). Der **Zombie-LUNGE-Treffer (cmd 2) spawnt im Original NACHWEISLICH KEIN Blut** — die EXE-Hit-Handler-Region 0x80035b70–0x80036700 enthält null `jal 0x80019700` und null `jalr` (Byte-Scan) — der Port ist dort bereits paritätisch. Alle Spawns nutzen `FUN_80019700(a0 = cat<<24 | sub<<16 | scale16, a1 = yaw, a2 = Part-Matrix-Ptr, a3 = Offset-Vektor)`; **alle Offset-Vektoren sind (0,0,0)** (0x8011f7d4, 0x801201c8, 0x8012110c, 0x80100068 — alle genullt gelesen).

---

## 1. Spawner-ABI (Vorwissen, verifiziert)

`FUN_80019700` (EXE; `RE_15_Quellcode_V2/FUN_80019700.c` + RE15_ESP_ROWMACHINE.md §4):

- `a0` gepackt: **category = a0>>24** (ESP-Bank-Id; 0 = Blut/Universal-Hit aus CORE00), **sub = (a0>>16)&0xff**, **scale16 = a0&0xffff** → Slot `+0x70/+0x71/+0x72` (Decompile: `*(short *)(puVar6+0x72) = (short)param_1`). `0x1500`/`0x2000`/`0x1000` sind also **cat 0, sub 0** mit Größe 1.3125/2.0/1.0 (Q12).
- `a1` → Slot `+0x2e` (base-yaw-Seed). In allen hier belegten Sites = `lh ent+0x6a` (rot_y des Spawn-Ankers) bzw. `lh 0x800acabe` (Spieler-Yaw; identisch, da Spieler-Entity @0x800aca54 → +0x6a = 0x800acabe).
- `a2` = **MATRIX-Pointer** (32 Byte kopiert; Translation @+0x14) = `*(ent+0x188) + bone*0xAC + 0x40` — der Part-Pool-Eintrag ist 0xAC groß, die komponierte Part-Matrix liegt @+0x40 (bereits etabliert in re15_damage.c `re15_enemy_hurt_blood`).
- `a3` = lokaler Offset-Vektor (nach Slot +0x40/44/48). **Alle Vektoren dieses Dossiers = (0,0,0)**: `DAT_8011f7d4` (=Nullwort-Lücke der State-Tabelle 0x8011f7b4[8..11]), `DAT_801201c8` (=Nullwort-Lücke der Hook-Tabelle 0x80120190[14..17]), `DAT_8012110c`, `DAT_80100068` — alle als 8×u16 = 0 gedumpt.

**Dispatch-Infrastruktur (neu belegt):**
- Spieler-cmd-Dispatch EXE `@0x80073f90[cmd]`: [2]=0x80035af0 hit, [3]=0x800366bc death, [5]=0x80036834 grab, **[6]=0x800368c0 death-by-grabber**, [7]=0x8003694c corpse.
- cmd 5 `@0x800368a0`: `jalr *(0x800ac758 + grabber_type*4)` (RAM-Tabelle, vom Overlay befüllt; Savestate: Typ 0x10/11/12/16 u. 0x1c–0x1f → **0x8010a28c**, 0x13 → 0x8010c16c, 0x20 Dog → 0x80111944, 0x21 Krähe → 0x8011597c, 0x26 → 0x8011c118).
- cmd 6 `@0x8003692c`: `jalr *(0x800ac858 + grabber_type*4)` (`lw v0,-514(a0=0x800aca5a+type*4)`; Savestate: Zombie-Typen → **0x8010a6b8** = Dispatcher `jalr 0x801201d8[aca59]` → **FUN_8010a6f8**).
- `0x8010a28c` = Dispatcher `jalr 0x801201a8[aca59]`; Overlay-Tabelle `0x80120190`: [6..9]=0x8010a2cc (Grab-FSM Spieler-Seite), [10..13]=0x8010a1cc/208/244/268 (Wund-DECAL-Helper, = blood_decals.md §3.2), [18..21]=0x8010a6f8 (Devour-Kill).
- cmd 2 `@0x80035b40-58`: `jalr 0x800741a8[aca59]` — **reine EXE-Tabelle**: [0]=0x80035b70, [1]=0x80035ca8 (schwer v/h), [2]=0x80035de0, [3]=0x80035f64 (normal v/h), [4]=0x800360e8, [5]=0x8003644c (Knockdown; Dog schreibt aca59=facing+4 `@FUN_8011854c` Z.91).

---

## 2. (A) Die Grab-/Biss-Sequenz — jeder Effekt-Spawn

### 2.1 Stehender Grab: Biss-Zyklus-Blut — `@0x80102818` (FUN_80102548 case 3)

Roh-Disasm (STAGE1.BIN):
```
801027c4: beq v0,zero,0x80102820   ; nur wenn func_0x8001f314(...) != 0 = BISS-CLIP-WRAP
801027dc: addiu v0,v0,-5           ; Spieler-HP -= 5 (0x800acaee)
801027e8: lw a0,392(v1)            ; a0 = *(zombie+0x188)  Part-Pool
801027f4: addiu v0,v0,-19          ; type - 0x13
801027f8: sltiu v0,v0,0x2          ; type in {0x13,0x14} (Zombie-Girl)?
801027fc: beq v0,zero,0x80102808
80102800: addiu a2,a0,1720         ; a2 = pool + 0x6b8 = 172*10  -> BONE 10
80102804: addiu a2,a0,1892         ; (Girl) pool + 0x764 = 172*11 -> BONE 11
80102808: ori a0,zero,0x1500       ; cat 0, sub 0, scale16 = 0x1500
8010280c: lh a1,106(v1)            ; a1 = zombie rot_y (+0x6a)
80102810/14: a3 = 0x8011f7d4       ; Offset (0,0,0)
80102818: jal 0x80019700
8010281c: addiu a2,a2,64           ; +0x40 = Part-MATRIX
```
**Kadenz:** einmal **pro Biss-Clip-Zyklus** (~26 Frames), zusammen mit dem HP−5; NICHT beim −10-Impact (case 2 `@0x80102714`-Block hat keinen Spawn). **Anker:** Zombie-Part 10 (Girl 11), Matrix @+0x40, Offset 0. Savestate-Messung (mzd_death_cmd5_struggle, Grab-Hold): Zombie-Part 10 = (−580,−2428,−23699) — hoch + vorn (Hand-/Maul-Bereich am Hals des Opfers); Part 14 (= Shot-Gore-Bone, `u8[0x8011f784+type]`) liegt dem Spieler-Kopf am nächsten = Zombie-Kopf.

### 2.2 Grab-Release: Spieler-Hals-Blut + Wund-Stempel — `@0x8010a574` (FUN_8010a2cc sub-step 4)

Der Spieler-seitige cmd-5-Handler `0x8010a2cc` (6-Phasen-FSM auf `DAT_800aca5a`, Sprungtabelle `@0x8010017c`: 0=0x8010a308, 1=0x8010a404, 2=0x8010a460, 3=0x8010a4a4, **4=0x8010a4e8**, 5=0x8010a5b8). Der Zombie schaltet die Phase 4 selbst frei (`_DAT_800aca58 = CONCAT12(4,…)` = `sb aca5a=4` im Escape-Pfad von case 3). Phase 4 roh:
```
8010a4f4/f8: sb aca5a = 5
8010a510/14: acae8(Clip) = flag*3+2       ; Release-Stagger (flag = acaf3 = vorne0/hinten1)
8010a518: sltiu v0(aca59),0x2
8010a51c: bne v0,zero,0x8010a550          ; aca59 0/1 (Zombie-Grab) ->
  8010a550: ori a0,zero,0x1500
  8010a554/58: a3 = 0x801201c8            ; (0,0,0)
  8010a56c: lh a1,106(player)             ; Spieler-Yaw
  8010a568/70: a2 = DAT_800acbdc + 0x5a0  ; Spieler-Pool + 172*8 + 0x40 = BONE 8 = HALS/KOPF
  8010a574: jal 0x80019700
; (aca59 >= 2: Clip flag*3+10, a2 = DAT_800acbdc + 0x244 = BONE 3 = Knie — Nicht-Zombie-Grabber)
8010a57c-b0: lbu aca59 ; acae9=0 ; acae3=7 ; jalr 0x801201b8[aca59]   ; WUND-DECAL-HELPER
```
`DAT_800acbdc == player+0x188` (Savestate: beide 0x801d3498). **Kadenz: einmal pro Grab, nur beim Freikommen** (Devour-Pfad geht stattdessen nach cmd 6).

**Korrektur/Ergänzung zu `analysis/blood_decals.md`:** Der „Wund-Dispatcher @0x8010a580" ist keine eigene Funktion, sondern der Tail von Phase 4 **direkt nach diesem Blut-Spawn**. Damit ist Open-Punkt 1 dort gelöst: **Hurt-Substate 0/1 = Grab vorne/hinten** (der Zombie schreibt `aca59 = (+0x5)−3` beim Grab-Init `@0x80102640`), und die 3×Substate-0-Wunden des Nutzer-Saves = **drei überlebte Front-Grabs** (je Release: Panels 0+10/5+50/7+50 → dritter Release überschreitet die 120er-Schwelle). Der 15s-Grab-Test stempelte nichts, weil er den Release nie erreichte (ein Grab, kein Freikommen).

**Bone-Anatomie (savestate-gemessen, stehend, rel. zur Spieler-Position, Y=hoch ist negativ):** Part 8 = (−24,−2478,−125)/(−38,−2514,90) = höchster zentraler Part = **Kopf/Hals**; Parts 9-11/12-14 = Arme (11 = Hand ✓ deckt sich mit dem Weapon-Render-Befund „PLW ersetzt Bone 11"); Part 3 = rechtes Knie (−956) — der aca59≥2-Pfad (Bein-Biss der Krabbler-Typen; welcher Typ cmd 5 mit aca59≥2 schreibt = OFFEN).

### 2.3 Devour-Kill (cmd 6): das Blut-Paar — FUN_8010a6f8 `@0x8010a7a0` + `@0x8010a84c`

Zombie-Seite: HP<0 oder Kill-Counter abgelaufen → `ent[+0x4] = ((+0x5)+2)<<8 | 1` (FUN_80102548 case 3) → Kill-Bite-FSM **FUN_80102bd8** (+0x5=5/6): Clip `+0x94 = (+0x5)+4` (=9/10), `aca58 = ((+0x5)−5)<<8 | 6` = **cmd 6**, SE room 4, Frame 0x28 SE room 3. KEIN eigener Spawn.
Spieler-Seite FUN_8010a6f8 (via 0x800ac858[type] → 0x8010a6b8 → 0x801201d8[aca59]) roh:
```
8010a760/68: acae8(Clip) = acaf3 + 6      ; Death-Bite-Clip 6/7 (vorne/hinten)
8010a770: ori a0,zero,0x1500              ; SPAWN #1 (Start, einmal)
8010a788: a2 = DAT_800acbdc(+0x5a0 @a7a4) ; Spieler-BONE 8 (Hals)
8010a790: a3 = 0x801201c8                 ; (0,0,0)
8010a79c: lh a1,106(player)               ; Spieler-Yaw
8010a7a0: jal 0x80019700
8010a7b8: jal 0x80045024 (0x4010001, player+0x34)      ; SE Bank4-Rec1
8010a7e8: acae9 == 0x23 ->                              ; Anim-Frame 35:
8010a804:   jal 0x80045024 (0x2070001)                  ;   Chomp-SE (Bank 2)
8010a80c/14:  player.hp = -1 (sh 0xffff, 0x800acaee)    ;   der Kill
8010a824: acae9 == 0x37 ->                              ; Anim-Frame 55:
8010a82c:   ori a0,zero,0x2000                          ;   SPAWN #2 (groß)
8010a834:   a3 = 0x801201c8 ; a2 = acbdc+0x5a0          ;   wieder Spieler-BONE 8
8010a84c:   jal 0x80019700  (+ SE 0x4030001 @0x8010a85c-Bereich)
; sub-step 2: aca58 = 7, aca59 = 0 (cmd 7 = Leiche/Game-Over)
```
**Kadenz:** Spawn #1 einmal beim Collapse-Entry, Spawn #2 einmal @Frame 0x37. Beide am Spieler-Hals-Bone 8, Offset 0.

### 2.4 Liegend-Devour (Zombie +0x5 = 0x25/0x26): FUN_80103b94 — `@0x80103e80` + `@0x80104080`

Animate-Tabelle `@0x8011f890`: [37]/[38] = FUN_80103b94 (Zombie frisst den LIEGENDEN, lebenden Spieler; Clips 0x1b/0x1c/0x1d; Spieler-cmd `((+0x5)+1)<<8 | 5` → aca59 38/39 → `0x801201a8[38/39]` = 0x8010b8f8/0x8010b974 — beide OHNE eigene Spawns, Site-Zensus leer):
- case 2: HP −5 (Entry); case 3 pro Biss-Clip-Wrap: **HP −1 + identischer Spawn wie 2.1** (`0x1500`, Zombie-Bone 10/11 +0x40, a3=0x8011f7d4) `@0x80103e80`.
- case 5 (Kill, `DAT_800aca5c & 4`==0, Anim-Frame 0x19): `puVar5 = pool + u8[0x8011f784+type]*0xAC` (Gore-Bone 14/Girl 8); **`0x2000`-Spawn** `@0x80104080` (a2 = puVar5+0x40, a3 = Kopie von `DAT_80100068` = (0,0,0)), danach Part-Detach-Writes (+0x68=0x6f, +0x94/9a/9e=0, part[0]|=0x4a) + SE room 9.
- Einstiegspfad in 37/38 (wer committet den Liegend-Devour): OFFEN (nicht verfolgt; der Tote-Spieler-Pfad committet 0xc01 = Feeding).

### 2.5 Dog-Biss (Task-Referenz „FUN_801185dc") und Spieler-cmd-5-Handler 0x80036834

- „FUN_801185dc" liegt **mitten in FUN_8011854c** (Dog-Biss-Attack-FSM; die Adresse ist dessen `+0x8f=7`-Store). Duale Hit-Sphären `pool+0x448`(=172·6+0x40, Bone 6) / `pool+0x6f8`(=172·10+0x40, Bone 10) via func_0x8001bff8 (Z.72-74); Treffer: HP −12, cmd 2, `aca59 = facing+4` (Knockdown-Substates [4]/[5]). **KEIN Blut-Spawn im Dog-Biss** (kein jal 0x80019700 in der Funktion; Site-Zensus).
- **FUN_80036834 (EXE cmd-5) ist ein reiner Dispatcher** (Grabbed-Y-Referenz `0x800acc0e = −floor·1800` + `jalr 0x800ac758[grabber_type]`) — **kein Spawn**.

---

## 3. (B) Krähen-Kontakt und Zombie-Lunge

### 3.1 Krähe — drei Kontakt-Spawn-Familien, alle an KRÄHEN-Bone 2 (`pool+0x198` = 172·2+0x40), a3=`DAT_8012110c`=(0,0,0), a1 = Krähen-rot_y (+0x6a)

| Angriff | Site | a0 | Kadenz | Begleit-Effekte |
|---|---|---|---|---|
| **DIVE-Hit** (FUN_801137fc) | `@0x80113bb0` | `ctr<<11` mit ctr=6…1 → **0x3000,0x2800,0x2000,0x1800,0x1000,0x800** | 6 aufeinanderfolgende Frames (Counter +0x1d7=6 beim Kontakt gesetzt, pro Frame Spawn+Decrement) | Kontakt: HP−4, cmd 2 aca59=facing+2, SE room 4, Bounce +0x1e6=−20; HP<0→cmd 3 |
| **PECK-GRAB** (FUN_80113e94; Grab-cmd `sb 5` @0x80113e48 davor) | `@0x80113f6c` (Entry) + `@0x80113fe4` (pro Peck-Clip-Wrap) | **0x2000** | 1× Entry + 1× pro Zyklus (Counter +0x9c=100, Mash −1−3·mash) | SE room 2/0-Kadenz; HP−8 UNGATED @0x80113e34 |
| **STRIKE** (FUN_80114484) | `@0x801144e0` | **0x1000** | einmal (Entry) | HP−4 UNGATED, HP<0→cmd 3 |

Der Spieler-seitige Krähen-cmd-5-Handler (0x8011597c, RAM-Tabelle) hat keine eigenen Spawns (Site-Zensus 0x80115000-0x80116000: nur 0x80115334/94 = FUN_801152e0, die Krähen-eigene FX-Routine).

### 3.2 Zombie-LUNGE (und jeder plain cmd-2-Hit): **KEIN Blut im Original**

Beweiskette:
1. cmd-2-Dispatch `@0x80035b40-58` → reine EXE-Tabelle `0x800741a8[aca59]` (6 Handler 0x80035b70…0x8003644c).
2. Byte-Scan der gesamten Region 0x80035b70–0x80036700: **null `jal 0x80019700`** (EXE-weiter Site-Zensus: alle 40 Sites liegen bei 0x8002c7xx/0x80033-34xxx/0x80038xxx/0x80041954/0x80045710) und **null `jalr`** (kein Overlay-Hook, keine RAM-Tabelle).
3. `FUN_80012d60` (Spieler-Damage-Entry): kein FUN_80019700-Call (Decompile-grep leer).
4. Die Zombie-Angriffs-FSMs (Engage/Attack-Region 0x80102058–0x80102548): Site-Zensus leer (Lücke zwischen 0x80100c14 und 0x80102818).

**Konsequenz:** Der Lunge-Treffer zeigt im Original nur die Hit-React-Animation (+ Wund-DECAL-Akkumulation NICHT — auch die Wund-Helper [2]/[3] der Tabelle 0x801201b8 werden von cmd 2 nie erreicht, die EXE-Handler rufen sie nicht). Port-Parität „kein Spawn beim Lunge" ist **korrekt**; der vom Nutzer vermisste Effekt ist der Grab-/Release-/Devour-/Krähen-Komplex aus §2/§3.1.

---

## 4. (C) PORT-IST (`re15_port/engine/src/`)

| Original-Spawn | Port-Ort | IST |
|---|---|---|
| 2.1 Biss-Zyklus `0x1500` @Zombie-Bone 10/11 | enemy_ai_common.c Grab case 3 (`bite_now`, ~L1196-1201) | **FEHLT** (nur HP−5) |
| 2.2 Release `0x1500` @Spieler-Bone 8 (+ Wund-Stempel) | Grab case 4 (~L1227-1241) + `re15_player_victim_throwoff` (L677) / Victim-Release (L739-747, 804 ff.) | **FEHLT** (beides) |
| 2.3 Devour Start `0x1500` @Spieler-Bone 8 | `re15_player_victim_devour` (L694-712) | **FEHLT** (nur SE 1) |
| 2.3 Devour Frame 0x37 `0x2000` @Spieler-Bone 8 | `re15_player_victim_tick` L798-803 | **TEILWEISE**: Spawn existiert, aber an der Spieler-WURZEL statt Bone 8 und via `re15_esp_fx_spawn` = scale16-Default **0x1000 statt 0x2000** |
| 2.4 Liegend-Devour (Zombie-States 0x25/0x26) komplett | — | **FSM UNPORTIERT** (Dead-Feed committet nur 0xc01; die 37/38-Kette existiert nicht) |
| 3.1 Krähe Dive-Burst / Peck-Grab / Strike | `re15_crow_hit_player(_ungated)` L3149-3177; Call-Sites L3471 (Dive), L3509/3670 (Grab), L3608 (Strike) | **FEHLT** (alle drei Familien; nur HP/cmd-Proxy) |
| 3.2 Lunge-Hit: kein Spawn | game_step/re15_damage.c | **PARITÄTISCH KORREKT** (kein Fix nötig) |

Vorhandene, wiederverwendbare Port-Infrastruktur: `re15_esp_fx_spawn_ex(bank,id,sub,scale16,x,y,z,yaw)` (re15_esp.c:307; Default-Wrapper `re15_esp_fx_spawn` = scale16 0x1000, re15_esp.c:335-339), `re15_enemy_bone_world_pos(e,bone,out)` (re15_damage.c:796, posiert das Enemy-Skelett und rotiert Bone→Welt), `re15_enemy_blood_at_bone`, `re15_esp_fx_splatter`, Gore-Bone-Tabelle `s_gore_bone` (= `u8[0x8011f784]`, re15_damage.c:789). Der Renderer nutzt scale16 (platform/pc/main.c:248 `step16 = S*scale16*camf/(sz<<4)`) → der 0x1000-Default halbiert derzeit die Sprite-Größe aller Spawns, deren Original-a0 0x2000 war (Hurt/Death/Dog-Eaten/Devour-0x37).

---

## 5. (D) Implementierungsvorschlag (jede Konstante mit Beleg)

**D1 — Biss-Zyklus-Blut (case 3, L~1198):** im `bite_now`-Zweig zusätzlich:
```c
int bone = (e->type == 0x13 || e->type == 0x14) ? 11 : 10;  /* 0x764/0x6b8 = 172*11/172*10 @0x801027f4-0x80102804 */
int32_t g[3]; re15_enemy_bone_world_pos(e, bone, g);
re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x1500,    /* a0=0x1500 @0x80102808 (cat0,sub0,scale 0x1500) */
                     g[0], g[1], g[2], (int16_t)e->rot_y);  /* a1 = +0x6a @0x8010280c; a3 = (0,0,0) @0x8011f7d4 */
```
Kadenz exakt der bestehende `bite_now`-Gate (== f314-Wrap `@0x801027c4`).

**D2 — Spieler-Bone-8-Helper:** neuer Helper (Analogon zu `re15_enemy_bone_world_pos`), der den Spieler im Victim-Zustand aus `vb->skel_victim/anim_victim` des Grabber-Typs posiert (der Port animiert Leon während Grab/Devour aus genau dieser Bank, enemy_ai_common.c L530 ff.) und `re15_skel_bone_to_world(poses[8].trans, player->rot_y, …)` liefert. Bone-Index **8** = `0x5a0 = 172*8+0x40` (`@0x8010a570`, `@0x8010a7a4`); Bank2-Skelett ist PL00-förmig 15 Bones (Port-Kommentar L4394 + Savestate-Anatomie §2.2).

**D3 — Release-Blut:** in `re15_player_victim_throwoff` (L677; deckt beide Zombie-Release-Sites L1238/L8070/L8108 ab — Achtung: L8070/8108 sind der Krähen/anderer Pfad, dort NICHT doppeln, sondern nur im Zombie-Grab-Release feuern; sauberster Ort = der Victim-FSM-Übergang STRUGGLE→RELEASE L739-747):
```c
re15_esp_fx_spawn_ex(bank, 0, 0, 0x1500, bone8[0],bone8[1],bone8[2], player->rot_y);
/* a0=0x1500 @0x8010a550; a2=acbdc+0x5a0 @0x8010a568/70; a3=(0,0,0) @0x801201c8 */
```
Zusätzlich hier der Wund-DECAL-Hook (blood_decals.md D3): `re15_wound_add`-Äquivalent mit Helper[aca59=variant] = Panels 0(+10)/5(+50)/7(+50) vorne bzw. 0/4/7 hinten (`@0x8010a5a0` jalr 0x801201b8 + blood_decals §3.2).

**D4 — Devour-Paar:** (a) in `re15_player_victim_devour` (L694-712) den Entry-Spawn ergänzen: `spawn_ex(..., 0x1500, bone8, player->rot_y)` (`@0x8010a770/0x8010a7a0`); (b) den bestehenden Frame-0x37-Burst (L800) auf `spawn_ex(..., 0x2000, bone8, …)` umstellen (`@0x8010a82c/0x8010a84c`; Frame-Gate 0x37 `@0x8010a820-24` ist schon korrekt portiert, ebenso HP=−1@0x23).

**D5 — Krähe:** (a) Dive: beim Kontakt (L3471-Region) `e->crow_fx_ctr = 6` (`+0x1d7=6 @FUN_801137fc Z.29`), und im Krähen-Tick solange `crow_fx_ctr>0`: `spawn_ex(bank,0,0,(uint16_t)(crow_fx_ctr<<11), bone2, e->rot_y); crow_fx_ctr--;` (`@0x80113bb0` + Shift `<<0xb` Z.47); (b) Grab: `0x2000` @Bone 2 beim Grab-Entry (L3509/3670) + einmal pro Peck-Zyklus (`@0x80113f6c/0x80113fe4`); (c) Strike: `0x1000` @Bone 2 (L3608, `@0x801144e0`). Bone-2-Position via `re15_enemy_bone_world_pos(e, 2)` (`0x198 = 172*2+0x40`).

**D6 (systemisch, optional separat):** die bestehenden `re15_esp_fx_spawn`-Sites, deren Original-a0 `0x2000` war (hurt_fx re15_damage.c:851, hurt_blood :890, blood_at_bone :903, death_fx :924, gore_tick :764, Dog-Eaten enemy_ai_common.c:4408/4412), auf `_ex(..., 0x2000, ...)` heben — der Renderer skaliert mit scale16 (main.c:248), aktuell rendern sie halb so groß.

**Verifikation:** Headless-Room-Probe ROOM1140: Grab erzwingen → pro Biss-Zyklus genau 1 aktiver neuer FX-Slot (effect 0, scale 0x1500); Release → 1 Slot scale 0x1500; Devour → Slots bei Entry + Frame 0x37 (scale 0x1500/0x2000). Visuell zwingend per `re15-port-visual-verify` (gdigrab), nicht per Autoshot.

---

## 6. Offene Punkte (ehrlich OFFEN)

1. **Einstieg in die Liegend-Devour-States 0x25/0x26** (wer committet sie; Voraussetzung liegender lebender Spieler = Knockdown-Kette aca59 4/5): nicht verfolgt. Nächster Schritt: Schreib-Zensus `+0x4`-Word `0x2501/0x2601` als sw-Wert bzw. der `+0x5`-Increment-Pfad um FUN_801036dc (animate[36]).
2. **Anatomische Benennung Zombie-Bone 10/11** (Hand vs. Maul): Geometrie gemessen (hoch+vorn am Opferhals), Benennung schematisch; für den Port irrelevant (Index zählt).
3. **Wer nutzt den aca59≥2-Pfad des cmd-5-FSM** (Release-Blut @Spieler-Bone 3 = Knie; Kandidat Krabbler-Typen 0x1b-0x1e): unverifiziert.
4. **SE-Reste:** Chomp-SE `0x2070001` (Bank 2) im Port weiter deferred (Port-Kommentar L796); Krähen-SE-Kadenz (room 2/0) beim Peck nicht geprüft.
5. **FUN_80109e4c** (Zombie animate[15], Spawn `0x2800` @0x8010a038) und die Kategorie-8/9-Spawns (FUN_80100688/0x8010ab2c `0x803xxxx`, FUN_801164b0 `0x9031800`) — benachbarte, hier nicht klassifizierte FX-Familien (Gib/Dismember-Verdacht).
6. **`DAT_800aca5c & 4`-Gate** im Liegend-Devour-Kill (case 5): Bedeutung des Flags offen.

---

## 7. Addendum 2026-08-03 — unabhängige Re-Verifikation + Ergänzungen (zweiter RE-Durchlauf)

Ein vollständiger zweiter Durchlauf (jal-0x80019700-Zensus beider Binaries: **98 Sites STAGE1.BIN / 40 Sites PSX.EXE**, Raw-Disasm aller Kern-Sites) **bestätigt byte-für-byte**: §2.1 (@0x80102818, a0 @0x80102808, Bone-Select @0x801027f4-0x80102804, Wrap-Gate @0x801027c4, HP−5 @0x801027dc), §2.2 (@0x8010a574; Zweig-Gate `sltiu aca59,2` @0x8010a518; Bone 8 @0x8010a570 = +0x5A0, Bone 3 @0x8010a54c = +0x244; Wund-jalr `0x801201b8[aca59]` @0x8010a5a4-a8), §2.3 (@0x8010a770/0x8010a7a0 + @0x8010a82c/0x8010a84c; cmd-6-Trigger `((+0x5)-5)<<8|6` @0x80102c80 in FUN_80102bd8), §2.4 (@0x80103e70/0x80103e80), §3.1 (@0x80113bb0 `ctr<<11` @0x80113b98, Dekrement @0x80113bcc; @0x80113f0c/0x80113f6c; Zyklus-Gate @0x80113fc4/0x80113fe4; @0x801144c4/0x801144e0 `0x1000`), §3.2 (Negativ-Zensus cmd-2-Region + FUN_80012d60). Zusätzlich neu belegt:

1. **Decompile-Verschiebung (Task-Referenz „FUN_801185dc"):** Der Body von `RE_15_Quellcode_Overlays/STAGE1/FUN_801185dc.c` liegt real bei **0x80118ddc** (identische Unikate: Clip-Liste `0x80121478`, `aca58=6`, HP−600; STAGE1_full/FUN_80118ddc.c). Dessen dekompilierter `func_0x80019700(3, …)`-Aufruf ist ein **Decompiler-Artefakt** — roh steht dort nur der State-Write-Block (`+0x4=3/+0x5=9/+0x6=3` → LAB_80119248), **kein jal**. Echte Spawns dieses Funktionspaares (0x80118908 + 0x80118ddc, Tabellen-Einträge @0x80121444/48): je ein `0x2000` @**0x80118bd0** bzw. @**0x80118f60**, Anker Bone 0 (`pool+0x40`), Vektor `DAT_801213a8`. Das Paar schreibt cmd 6 @**0x801191cc** mit **HP−=600** — Alt-Text §2.5 ordnet die 0x80118xxx-Region dem Dog zu; die Region gehört nach Dispatch-Nachbarschaft eher zur 0x80116db8-Boss-Familie (0x27). **Zuordnung Dog↔Boss dort = OFFEN** (beide Lesarten im Dossier markieren, bevor 0x8011854c ff. portiert wird).
2. **Hook-Registrierung statisch belegt** (Ergänzung zu §1, dort savestate-belegt): STAGE1-Overlay-Init @**0x8011e9a0-0x8011eac8** schreibt Hook A `0x800ac798/79c/7a0/7b0/7b8/7c8/7cc/7d0/7d4 ← 0x8010a28c`, `0x7a4 ← 0x8010c16c` (0x13), `0x7d8 ← 0x80111944` (0x20), `0x7dc ← 0x8011597c` (0x21), `0x7f4 ← 0x8011c118` (= Basis 0x800ac758 + **0x27**·4 — Alt-Text §1 nennt „0x26"; statisch ist der Slot 0x27); Hook B @0x8011ea3c-: `0x800ac898…8d4 ← 0x8010a6b8`, `0x8a4 ← 0x8010c1ac`, `0x8d8 ← 0x80111cb0` (0x20), `0x8dc ← 0x80115d2c` (0x21), `0x8f4 ← 0x8011c3d4` (0x27). EXE-Leser: `lw v0,-770(0x800aca5a+type*4)` = **0x800AC758+type·4** @0x800368a0 (cmd 5) bzw. `-514` = **0x800AC858+type·4** @0x8003692c (cmd 6) — kein weiterer statischer Leser (imm-Zensus beider Binaries).
3. **Krähen-Wund-DECALS bestätigt** (löst blood_decals §3.3-„Krähe?"): der Dive-Hit ruft @**0x80113b7c** den Helper **0x801161e8**, der nach `ent+0x1ec` gestaffelt `FUN_80037edc` füttert: `<1500` → Panels (1,+10)+(2,+10) @0x80116200-0x80116230; `<3000` → (3,+10)+(4,+0x28) @0x80116220-0x8011623c; weitere Sites @0x8011620c/2c/38/44/60/70. Die Krähe stempelt also Wund-Decals BEIM KONTAKT (nicht nur im Grab-Release-Pfad).
4. **Neue Spur für Offen-Punkt 1** (Liegend-Devour-Einstieg/-Kill): @**0x801045dc** (FUN_80104540) schreibt `cmd = ((+0x5)-1)<<8 | 6` — der Liegend-Kill-Handoff mit aca59 > 3 (Dispatcher 0x8010a6b8 indexiert `0x801201d8[aca59]` ungeprüft → höhere Einträge, z. B. 0x8012090c-Region). Store-Zensus `0x800aca58`: cmd-6-Writer in STAGE1 NUR @0x80102c80 (Zombie-Grab-Kill), @0x801045dc (Liegend-Kill), @0x80110138 (Zgirl, FUN_801100b4), @0x801191cc (0x80118ddc); **EXE: keiner**.
5. **Grab case 2 (Impact −10) spawnt nichts** — Zensus-Lücke 0x80102640-0x80102818 erneut bestätigt (§2.1-Aussage steht).
