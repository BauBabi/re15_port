# Leons Opfer-Animationen bei Krähen (Typ 0x21) — Hook A/B komplett, Dive-Flinch, Port-Einhängung

**Datum:** 2026-08-03 · **Status:** Original-Seite **byte-belegt** (Roh-Disasm STAGE1.BIN + PSX.EXE + RDT-Byte-Parse), Port-Seite **dynamisch gemessen** (probe_crow_flinch, voller `re15_game_step`)
**Quellen:** `info/Re1.5/PSX/BIN/STAGE1.BIN` (@0x80100000 roh, off = addr−0x80100000), `info/Re1.5/PSX.EXE` (off = addr−0x80010000+0x800), `re15_port/shared_assets/PSX/STAGE1/ROOM1171.RDT`, `re15_port/shared_assets/PSX/EMD/CDEMD0.EMS`, `RE_15_Quellcode_Overlays/STAGE1_full/`, Port `re15_port/engine/src/`.
**Schwester-Dossiers:** `analysis/player_hit_chain.md` (Cmd-Maschine; §1.3 wird hier korrigiert), `analysis/player_knockdown.md` (cmd-4-Plc-Klasse; §7.1 wird hier für die 1171-Szene GESCHLOSSEN), `analysis/crow_1170.md`, `analysis/crow_shot_attack.md`.
**Werkzeug:** `re15_disasm.py` (dis/table + jal-Zensus), SCD-Parse (Opcode-Tabelle Skill `scd-disassembly`), `re15_port/tests/unit/probe_crow_flinch.c` (neu, Diagnose, kein ctest).
⚠ **Build-Flake auf dieser Maschine (2026-08-03):** `cc.exe` wird UNTER NINJA reproduzierbar mit Exit 1 ohne Diagnose abgeschossen (betrifft auch das vorbestehende `probe_crow_1170` — Kontrolltest; identischer Befehl läuft interaktiv fehlerfrei; vgl. `grep.exe.stackdump`/AV-Verdacht). Workaround, mit dem alle Messungen liefen: identische cc-Kommandozeile direkt ausführen und gegen `engine/libre15_engine.a tests/libre15_test_support.a` linken.

---

## 0. Executive Answer

1. **Der Krähen-GRAB (cmd 5) hat eine eigene Spezial-Opfer-FSM** (Hook A [0x21] = `LAB_8011597c`, Registrierung `sw @0x8011ea20-28` → `0x800ac7dc`), die Leon **aus Paar C** (Victim-Bank der Krähe, `0x800acbcc/0x800acbd0` = crow`+0x178/+0x17c`) die Clips **0 (Intro, 14 Frames) → 1 (Peck-Halte-Loop, 36 Frames/Zyklus) → 2 (Release/Wurf, 20 Frames)** spielen lässt. Vorne endet sie mit `aca58:=1` (Steuerung zurück), hinten mit **cmd 4 Mode 6** (Wurf → Liegend-Halt aus Paar B/PLW).
2. **Hook B [0x21] = `0x80115d2c` ist TOTER CODE**: er dispatcht `0x80121244[aca59]` mit [0] = `jr ra`-Stub (@0x80115d6c) und [1..] = 0 — und **kein Krähen-Code schreibt je cmd 6** (vollständiger Store-Zensus §2.2). Die player_hit_chain-§1.3-Zeile „Hook B stempelt @0x8011620c…" ist **falsch**: diese Stempel-Sites gehören dem **Attack-Site-Wund-Helper `0x801161e8`** (vert-gebändert), gerufen von Dive/Grab/Strike (@0x80113b7c/0x80113ef4/0x801144bc).
3. **Der DIVE fährt byte-true NUR den Standard-Flinch** (cmd 2, `aca59 = facing+2` @0x80113b00/28 → EXE-Handler [2]=Clip 8/[3]=Clip 9 aus Paar A/PLD) **plus den Attack-Site-Stempel**. Dynamisch gemessen: **der Port-Flinch FEUERT beim Dive** (sauberer Zustand: t+1 nach dem HP-Drop → motion=0x08, 22 Ticks). Was der Nutzer als „fehlende Reaktion" sieht, ist der **GRAB**: in ROOM1170 committen die armed Krähen (grid 0x00) per steer[9] **immer in sub 12 (Grapple)**, nie in den Dive — und der Port pinnt den gegriffenen Leon nur (`s_player_grabbed`), **ohne jede Victim-Animation** (gemessen: motion bleibt auf dem Idle/Walk-Sentinel 0xd2-0xd5, 3 Grabs à ~100 Ticks, 0 Flinch-Starts).
4. **Der Wurf-Exit (player_knockdown §7.1) ist für die ROOM1171-Szene GELÖST**: der Rück-Grab existiert nur im Event-Zweig (FLIGHT-2-Krähen, behav 0x40); die Szene ROOM1171 sub02 wartet auf z5/28 (Grab-Connect @0x801152c4), setzt nach 60 Frames **z5/29** (Wurf-Push des Roots @0x801120a0-e8), schießt die Krähe per z5/31 ab und **beendet den Mode-6-Halt per nächstem `Plc_dest`** (mode 5 @RDT 0x14de usw.) — der Plc_dest-Re-Init (@0x80041bf8-c0c skippt nur bei GLEICHEM Mode) überschreibt cmd4/mode6. Es gibt keinen Engine-Timeout; der Exit ist Script-getrieben.

---

## 1. (A) Hook A `LAB_8011597c` — die Krähen-Opfer-FSM, vollständig

### 1.1 Entry + Richtungs-Dispatch

```
8011597c: lbu 0x800aca59                 ; Grab-Richtung
8011598c-9c: sll 2; lw 0x8012123c[aca59]
801159a4: jalr
```
Tabelle `@0x8012123c` (on-disc, dumped):
| aca59 | Handler | Bedeutung |
|---|---|---|
| 0 | `0x801159bc` | **FRONT-Opfer-FSM** (Writer: Front-Grab `sb 0→aca59` @0x80113e30) |
| 1 | `0x80115b80` | **REAR-Opfer-FSM** (Writer: Rück-Peck `sb 1→aca59` @0x801152ac, FUN_80115130) |
| 2 | `0x80115d6c` = `jr ra; nop` | defensiver Stub — kein Krähen-Writer mit dir 2 existiert (Zensus §2.2) |

### 1.2 FRONT-FSM `0x801159bc` — Phasen via `0x801002ec[aca5a]` (aca5a<5, sonst Exit @0x80115b70)

Phasentabelle `@0x801002ec` (dumped): [0]=0x801159f4 [1]=0x80115a58 [2]=0x80115aa8 [3]=0x80115acc [4]=0x80115af4.

| aca5a | Handler | Stores (jede Adresse roh-disasm-verifiziert) |
|---|---|---|
| 0 | 0x801159f4 | `0x800acc0c:=1` @0x801159f8-fc (Anim-Basis-Flag, FUN_80024c30-Konsument); **Clip `+0x94:=0`** @0x80115a18 (0x800acae8), Frame `+0x95:=0` @0x80115a20, **Blend `+0x8f:=7`** @0x80115a28 (0x800acae3); **`aca3c \|= 0xC0`** @0x80115a3c-44; **`hit_react +0x93 \|= 1`** @0x80115a40-4c; `aca5a:=1` @0x80115a54 → fällt in Phase 1 |
| 1 | 0x80115a58 | **`f314([0x800acbcc],[0x800acbd0], a2=0, 0x200)`** @0x80115a5c-70 = **Paar C**; Wrap → **Clip 1** @0x80115a88, Frame 0, `aca5a:=2` |
| 2 | 0x80115aa8 | f314 Paar C @0x80115aac-c0, **KEIN Selbst-Advance = Peck-Halte-Loop** (Clip 1 loopt) |
| 3 | 0x80115acc | **Clip 2** @0x80115ad4-dc, Frame 0, `aca5a:=4` → fällt in Phase 4 |
| 4 | 0x80115af4 | f314 Paar C @0x80115af8-b0c; Wrap → **`sb 1→aca58` @0x80115b1c**, `aca59:=0` @0x80115b30, `sh 0→aca5a` @0x80115b38, **`sw 1→aca58` (Wort: cmd=1, 59/5a/5b=0)** @0x80115b40; **`hit_react &= ~1`** @0x80115b44-4c; **`aca3c &= ~0xC0`** @0x80115b54-6c |

**Kein Wund-Stempel und kein Blut-Spawn in 0x801159bc-0x80115b7c** (kein `jal 0x80037edc`, kein `jal 0x80019700`) — die Krähe stempelt am ATTACK-Entry (§2.3), nicht beim Release (anders als die generische Grab-FSM `0x8010a2cc`).

### 1.3 REAR-FSM `0x80115b80` — Phasen via `0x80100304[aca5a]` (aca5a<5, sonst Exit @0x80115d1c)

Phasentabelle `@0x80100304` (dumped): [0]=0x80115bb8 [1]=0x80115c0c [2]=0x80115c74 [3]=0x80115cb4 [4]=0x80115cdc.

| aca5a | Handler | Stores |
|---|---|---|
| 0 | 0x80115bb8 | `acc0c:=1` @0x80115bc0-c4; Blend 7 @0x80115bd0; Clip 0 @0x80115bec, Frame 0 @0x80115bf4; `aca5a:=1` @0x80115c00; **SE `0x80045024(0x03020001, player+0x34)`** @0x80115c04 (a1 = `[0x800ac784]+0x34`; im Spieler-Task zeigt ac784 auf den Spieler, gesetzt @FUN_80031c44). ⚠ **KEIN `hit_react\|=1`, KEIN `aca3c\|=0xC0`** (Asymmetrie zur Front-FSM — byte-Fakt) → fällt in Phase 1 |
| 1 | 0x80115c0c | f314 **Paar C** @0x80115c10-24; Wrap → SE 0x03020001 @0x80115c3c, Clip 1 @0x80115c54, Frame 0, `aca5a:=2` |
| 2 | 0x80115c74 | f314 Paar C @0x80115c78-8c, kein Advance = **Halte-Loop**; **SE 0x03020001 bei jedem Clip-Wrap** @0x80115ca4 |
| 3 | 0x80115cb4 | Clip 2 @0x80115cc4, Frame 0, `aca5a:=4` → fällt in Phase 4 |
| 4 | 0x80115cdc | f314 Paar C @0x80115ce0-f4; Wrap → **`sb 4→aca58` @0x80115d04, `sb 6→aca59` @0x80115d10, `sh 0→aca5a` @0x80115d18** = **cmd 4 Mode 6** (Hinlegen/Halten) — der WURF. Kein hit_react-/aca3c-Clear |

### 1.4 Bank-Paar-Auflösung — JEDER f314-Call der Kette

| f314-Call-Site | a0/a1-Register | Paar | Quelle des Paars (Writer) |
|---|---|---|---|
| Front-FSM Ph.1/2/4: @0x80115a6c, @0x80115abc, @0x80115b08 | `[0x800acbcc]/[0x800acbd0]` | **C (Victim-Bank der Krähe)** | Front-Grab-Link @0x80113e08-14/e20-e50: `acbcc := crow+0x178` (lw 376), `acbd0 := crow+0x17c` (lw 380); crow+0x178/17c gefüllt vom EMS-Loader FUN_80022300 (Paar-C-Slots, player_knockdown §4.1) |
| Rear-FSM Ph.1/2/4: @0x80115c20, @0x80115c88, @0x80115cf0 | dito | **C** | Rear-Grab-Link FUN_80115130 (Decompile Z.31-34): `acbfc=crow; acbcc=crow+0x178; acbd0=crow+0x17c` (+ `aca58=5, aca59=1`) |
| Mode-6-Handler (nach dem Wurf) `0x800517f0`: @0x8005188c, @0x800518f0 | `lw a0,368(v0)/a1,372(v0)` = `ent+0x170/+0x174` @0x80051884-88/0x800518e8-ec | **B (PLW)** für den Spieler: player+0x170 = `0x800acbc4`, +0x174 = `0x800acbc8` | FUN_80036b68 (PLW dir[1]/dir[0], Tabelle @0x800741e8) — **Clip 1** @0x80051854, **Clip 2 (Loop)** @0x800518c8, Blend 7 @0x80051874/0x800518d8 |
| Standard-Flinch [2]/[3] (Dive) und Knockdown [4]/[5] | `[0x800acad8]/[0x800acbc0]` | **A (PLD-Basis)** | FUN_800314b0 Z.20-23 (bereits belegt, player_knockdown §4.1) |

### 1.5 Die Victim-Clips selbst (EM021, Datei-Beleg)

`CDEMD0.EMS`, Blob-Index 8 (= Typ 0x21 per `s_ems_order`), **Blob @EMS-Offset 0x17b800**, dir-Count 9:
- **dir[5] = Victim-EDD @blob+0x49d4**: **3 Clips — Clip 0 = 14 Frames (Intro/Gepackt), Clip 1 = 36 Frames (Halte-Loop), Clip 2 = 20 Frames (Release/Wurf)**.
- **dir[6] = Victim-Keyframe-Pool @blob+0x4afc**: Header `bones_tbl=0, kf_off=8, bone_count=15, kf_size=80` = **PL00-kompatibel** (animiert LEON).
- ⚠ **dir[2] @blob+0x5b4 = das KRÄHEN-eigene EMR: bone_count=13, kf_size=72.** (Vergleich: EM10/EM20 haben dir[2]=15/80 — nur bei der Krähe divergiert dir[2] von der Victim-Geometrie; Port-Falle §5.3.)

### 1.6 Dauer/Struggle/Mash (Krähen-Seite, Front-Grab)

- Connect (move[12], Kontakt +0x1d0): `aca50=(aca50&0xfff)|0x8000` @0x80113df0-fc; Grab-Link (§1.4); `aca5a:=0` @0x80113e28; `aca59:=0` @0x80113e30; **`hp −= 8`** @0x80113e34-3c (UNGATED); `aca58:=5` @0x80113e48; Krähe → sub 13 @0x80113e54-58.
- Halte-Phase (move[13]): Budget `+0x9c=100` (@0x80113ef0/f8); Drain `+0x9c −= 1 + 3·FUN_80037024()` (Mash-Maske 0xf0f0) — **ohne Mash 100 Ticks, mit Dauer-Mash ~25**; Budget<0 → **`aca5a++` @0x801140b4-c0** (= Opfer-Phase 2→3), `+0x1d8=1` @0x801140c8, `aca50|=0x4000`. **Der Release kommt IMMER** (Drain −1 läuft unbedingt); die Krähe kennt keinen Devour.
- Tod im Grab: `hp<0` → `aca58:=3` @0x80113f20 (cmd 3 DEATH, kein cmd 6, kein Hook B).

### 1.7 Der zweite Push-Kanal: Root + z5/29 (Script)

Root `0x80112020` @0x801120a0-e8 (roh):
```
efe4(0x800b1028, 0x1d)  &&  aca58==5  &&  aca5a<3   →   sb 3 → aca5a
```
= **z5-Flag 29 zwingt die Opfer-FSM in Phase 3** (Release/Wurf). Setter von z5:29/30/31:
- **ROOM1171 subScd sub02** (Datei-Offsets): `Set z5:30` @0x13b2, **`Set z5:29` @0x1404**, `Set z5:31` @0x1464/0x1498, Clears @0x14a0-ac. (SCD-Disasm §3.)
- **Spider-Baby-Flinch** FUN_80116a04: `ef90(0x800b1028, (grid&0x7f)+29)` @0x80116ac0-cc, gated `phase<8 && (grid&0x7f)<3` — Cross-Typ-Setter, in Krähen-Räumen ohne Spinnen irrelevant.

---

## 2. Hook B `0x80115d2c` — die Kill-Seite ist toter Code

### 2.1 Registrierung + Dispatch

- Registrierung: `sw 0x80115d2c → 0x800ac8dc` (= 0x800ac858 + 0x21·4) @0x8011eaa8-b0 (FUN_8011e864; Hook A analog @0x8011ea20-28 → 0x800ac7dc).
- Entry @0x80115d2c: `lbu aca59; jalr 0x80121244[aca59]`. On-disc-Tabelle @0x80121244: **[0] = 0x80115d6c (`jr ra`-Stub)**, [1..5] = 0x00000000 (danach Datenwörter).

### 2.2 Zensus: KEIN Krähen-Writer für cmd 6

Vollständiger Binär-Scan aller `sb/sh/sw` mit imm-Offset 0xca58 (0x800aca58) über 0x80111a4c-0x80116400: 12 Sites — Dive cmd 2 (@0x80113b00), Grab cmd 5 (@0x80113e48), Rear-Peck cmd 5 (@0x801152a0), Tode cmd 3 (@0x80113b48/0x80113f20/0x80114518), Opfer-FSM-Exits cmd 1 (@0x80115b1c/40), Wurf cmd 4 (@0x80115d04), dazu Dog-Hook-Releases (@0x80111c60/ea0) und Spider-Spit cmd 2 (@0x80116390). **Wert 6 kommt nicht vor** → der cmd-6-Handler `0x800368c0` erreicht den Krähen-Slot nie; die Null-Einträge [1..] sind unerreichbar.

### 2.3 Korrektur an player_hit_chain.md §1.3 + crow_1170.md

Die dort Hook B zugeschriebenen Stempel-Sites `@0x8011620c/2c/38/44/60/70` liegen im Helper **`0x801161e8`** — einem **vert_err-gebänderten Wund-Stempler** (liest `+0x1ec` der Krähe):

| vert_err (+0x1ec) | FUN_80037edc-Calls (Panel, Betrag) |
|---|---|
| < 1500 | (1, +0xa) @0x8011620c, (2, +0xa) @0x80116270 (a0 @0x80116214, a1 @0x8011621c) |
| 1500..2999 | (3, +0xa) @0x8011622c, (4, +0x28) @0x80116238, (6, +0x28) @0x80116244, (5, +0x28) @0x80116270 (a0 @0x80116250, a1 @0x8011626c) |
| 3000..3599 | (0, +0x8) @0x80116260, (7, +0x28) @0x80116270 (a0 @0x80116268) |
| ≥ 3600 | nichts (beq @0x80116258 → Exit) |

Caller (jal-Zensus STAGE1.BIN): **@0x80113b7c (Dive-Connect), @0x80113ef4 (Grab move[13] step0), @0x801144bc (Strike move[16])**. `crow_1170.md` nennt ihn „Screech" — **Fehletikett**; es sind Wund-Panels, keine SEs (Port-Folge §4.4).

---

## 3. Die ROOM1171-Wurf-Szene — SCD-Choreographie (löst player_knockdown §7.1 für diesen Pfad)

ROOM1171 subScd (base 0x1364, 8 subs), **sub02 @0x1398-0x1592** (gestartet per `Evt_exec ff 18 02` aus sub07-Event-Zweig, z3/194):

```
0x13b2  Set z5:30            ; FLIGHT-2-Gate (FUN_8011553c wartet auf Bit 30)
0x13ca  Plc_dest mode 9 →(1145,7305), flag 0   ; Spieler dreht  (Gosub 05 = Wait z5:0)
0x13d4  Plc_dest mode 5 →(1145,7305)           ; Spieler RENNT
0x13f4  While Ck z5:28==0 → Evt_next           ; WARTET auf den Grab-Connect
                                                ; (z5:28 setzt FUN_80115130 @0x801152c4 beim Rück-Peck)
0x1400  Sleep 60                                ; 60 Frames Peck-Halten (Opfer-Phase 2 loopt Clip 1)
0x1404  Set z5:29                               ; → Root @0x801120e4-e8: aca5a:=3 → Clip 2 → WURF
                                                ;   (cmd 4 Mode 6 @0x80115d04-18; Leon liegt, PLW-Clip 1→2-Loop)
0x140c  Cut_chg 4 … Plc_motion(NPC) …           ; Elliot-Regie
0x1438-0x148c  6× Se_on Bank2/Rec10 + Set z5:31 (@0x1464/0x1498)   ; Krähe wird abgeschossen
                                                ;   (z5:31 = Death-Promotion-Gate des Roots @0x80112048)
0x14a0-0x14ac  Set z5:31/30/29/28 := 0          ; Aufräumen
0x14de  Work_set(1,0); Plc_dest mode 5 →(140,−3890)   ; ← DER MODE-6-EXIT: Plc_dest-Re-Init
                                                ;   (@0x80041c14: +0x4=4,+0x5=mode,+0x6/7=0; Guard
                                                ;   @0x80041bf8-c0c skippt NUR bei +0x5==mode)
0x14f2-0x1560  weitere Plc_dest 9/4-Ketten (Spieler+NPC), Gosub 05/06 (= Wait z5:0/z5:1,
                Arrival-Flags der Plc-Modes)
0x1566/0x1572  Plc_dest mode 0x13, flag 0x14 (Spieler+NPC)   ; Semantik OFFEN (§7)
0x157a/0x157e  Evt_exec sub03/sub04 (Prop-Regie: Work_set kind 3 = Obj_model_set-Props 0/1,
                Speed_set/Add_speed-Rampen; scd_work_bind kind-Mapping: 1=Spieler, 2=EM-Slot, 3=Prop)
```

**Konsequenzen:**
- Der Rück-Grab/Wurf ist eine **Script-Szene** (Event-Krähen behav 0x40 → State 4 FLIGHT-2; sub 0 tail @0x801150f8-11c: `vert_err ≥ 2001 → sub 1` = Angriff; sub 1 = FUN_80115130 Connect; sub 2 = FUN_801152e0 Krähen-Halte mit Blut-FX `0x80019700(0x1000, …)` je Clip-Wrap).
- **Kein Engine-Exit für Mode 6 existiert** — der Halt endet, wenn das Script den nächsten `Plc_dest` schreibt. Für die Normal-Räume (1170/10C0/1120) ist das irrelevant: dort gibt es **nur den Front-Grab** (State-1-Krähen schreiben ausschließlich dir 0 @0x80113e30), der mit `aca58:=1` endet.
- OFFEN bleibt die Laufzeit-Interaktion mit dem Modes-4/5-Ankunfts-Bug (player_knockdown §5.4): sub05 wartet auf z5:0, das der (buggy codierte) Mode-5-Ankunftspfad setzen soll — Live-Capture nötig (§7).

---

## 4. (B) Der DIVE — Original vs. Port (dynamisch gemessen)

### 4.1 Original (roh @0x80113aa8-b80)

- Gates: `(vert_err − 1) < 0xe0f` (unsigned → vert ∈ [1,3599]) @0x80113ab0-b4 **und `hit_react +0x93 == 0`** @0x80113ac0-cc (NUR der Dive hat dieses Gate).
- Connect: `+0x1d7:=6` @0x80113ad4-d8 (Blutfahnen-Latch), `+0x1e6:=−20` @0x80113ae4-ec, Room-SE 4 (jal 0x800453d0, a0=4 @0x80113ad0), **`aca58:=2`** @0x80113b00, **`hp −= 4`** @0x80113b04-0c, **`aca59 := FUN_8001a780(player)+2`** @0x80113b10-28, `aca5a:=0` @0x80113b30; hp<0 → cmd 3 @0x80113b48 + Flock 0x2000; **danach IMMER `jal 0x801161e8`** @0x80113b7c (Wund-Stempel §2.3).
- **`FUN_8001a780` ist ein reiner YAW-Vergleich** (roh @0x8001a788-a4): `ret = ((player.rot_y − attacker.rot_y + 0x400) & 0xfff) < 0x800` — ret 0 = Angreifer schaut dem Spieler ins Gesicht (frontal), ret 1 = gleiche Blickrichtung (von hinten). KEINE Positionsrechnung.
- EXE-Substate-Handler: **[2] (vorne) spielt Clip 0x8** (@0x80035e38-40) + Knockback 0xc8 (@0x80035e44-4c); **[3] (hinten) spielt Clip 0x9** (@0x80035fbc-c4) — beide aus **Paar A** (PLD).

### 4.2 Port-Messung (probe_crow_flinch.c, voller `re15_game_step`, ROOM1170 Re-Entry, Band 4)

| Lauf | Ergebnis |
|---|---|
| A: 3600 Ticks neutral neben der armed Krähe | **3 HP-Drops, alle −8 (GRAB), alle mit `grabbed=1`, motion=0xd3-Sentinel — 0 Flinch-Starts.** Kein einziger Dive-Connect (armed 1170-Krähen sind grid 0x00 → steer[9] @0x8011335c-68 wählt IMMER sub 12/Grapple; Dive-Lane 10/11 nur für grid≥0x80, in 1170 erst nach Schuss-Re-Arm-Broadcast erreichbar) |
| B: dito mit gehaltenem R1 (AIM) | identisch; motion=0xd5 (AIM-Sentinel) |
| C: Dive erzwungen (sub 11 Ph.2, vert 1500, dist<600), sauberer Prozesszustand | Connect (hp −4, hit_react=1) → **nächster Tick FLINCH: motion=0x08, Frames 0..21, dann Idle — der HP-Drop-Detector FUNKTIONIERT für die fliegende Krähe** (re15_nearest_hostile hat kein Distanz-/Höhen-Gate und findet die hp-0-Krähe; hp<0-Skip greift nur beim +0x9a=−1-Latch ≥5200 — beim Dive ist die Krähe tief) |
| C nach vorherigem Aim-Lauf | `aim=1` blieb gelatcht → `!re15_player_aim_active()`-Gate (game_step_common.c:355) **unterdrückt den Flinch komplett** |

### 4.3 Befund zu Report „Reaktions-Animation fehlt"

1. **Hauptursache = der GRAB**: die häufigste (in 1170 praktisch einzige) Krähen-Attacke pinnt Leon ohne jede Anim — die Spezial-Opfer-FSM (§1) ist nicht angebunden (§5). Der Dive-Flinch selbst ist funktional.
2. **Aim-Gate-Divergenz**: das Original kennt kein Aim-Gate — der Dive schreibt cmd 2 unconditional (@0x80113b00; der cmd-Dispatch @0x80031c88 ERSETZT den kompletten cmd-1-Zustand inkl. Aim). Der Port unterdrückt den Flinch bei `re15_player_aim_active()` — wer beim Krähen-Kampf zielt (der Normalfall!), sieht NIE einen Flinch. → Fix: Flinch muss den Aim abbrechen, nicht umgekehrt.
3. **Richtungs-Mapping wahrscheinlich invertiert**: Original vorne(a780=0)→[2]→Clip **8**, hinten→[3]→Clip **9**; der Port-Detector mappt `hit_from_front ? 0x09 : 0x08` und leitet die Richtung aus POSITIONEN ab statt aus dem Yaw-Vergleich @0x8001a794-a4. (Polarität von `re15_player_hit_from_front` gegen a780 beim Fix verifizieren.)

### 4.4 Wund-Stempel-Divergenzen (Attack-Site)

- `re15_crow_screech` (enemy_ai_common.c:3188) spielt Room-SEs 1/2, 3/4/6/5, 0/7 — **Fehldeutung von 0x801161e8**: die Zahlen sind `a0`-PANELS von FUN_80037edc, die Beträge a1 = 0xa/0x28/0x8 (§2.3). Es gibt dort keinen einzigen SE-Call.
- Der Dive-Case stempelt zusätzlich inline, aber unvollständig: Band 2 nur (3,+10),(4,+40) statt (3,+10),(4,+40),(6,+40),(5,+40); Band 3 ((0,+8),(7,+40)) fehlt ganz; Grab/Strike stempeln gar nicht (nur Fehl-SEs).

---

## 5. (C) PORT-IST + Einhäng-Plan

### 5.1 IST (code-verifiziert + gemessen)

| Original | Port | IST |
|---|---|---|
| Hook A [0x21] Opfer-FSM (Paar C, Clips 0/1/2) | — | **FEHLT**: Grab pinnt nur (`s_player_grabbed=1` bei crow sub 13, enemy_ai_common.c:4091); `re15_player_victim_latch` wird für 0x21 nie gerufen; `re15_victim_clip_map` kennt nur 0x20/Zombie-Default |
| Victim-Bank EM021 (dir[5]/[6]) | `re15_emd_parse_victim_bank` (emd_common.c:556) generisch, main.c:449 lädt sie für JEDEN Typ | Daten vorhanden, **aber Geometrie-Falle**: der Parser nimmt bone_count/kf_size aus **dir[2]** — bei EM021 ist das das Krähen-EMR (13/72), der Pool ist 15/80 (§1.5) → `victim_ok=1` mit inkonsistenter Geometrie |
| Release-Ende vorne: Clip 2 → `aca58:=1` | crow case 13 → sub 14 ohne Spieler-Seite | **FEHLT** (Leon läuft einfach weiter) |
| Rück-Grab + Wurf (1171-Szene) | — | FEHLT (Szene-Ticket; Exit jetzt RE'd §3) |
| Dive-Flinch | HP-Drop-Detector | ✓ funktioniert (gemessen §4.2) — aber Aim-Gate + Richtungs-Mapping (§4.3) |
| Attack-Site-Stempel 0x801161e8 | `re15_crow_screech` + Teil-Inline | **falsch** (§4.4) |

### 5.2 Einhäng-Plan Front-Grab (byte-true, jede Konstante zitiert)

1. **Latch**: im Port-Grab-Connect (case 12 Kontakt-Zweig, dem Pendant von @0x80113dd4-e58) `re15_player_victim_latch`-Äquivalent mit `type=0x21`, `variant=0` (aca59=0 @0x80113e30); Rück-Variante (`variant=1`) NUR aus dem 1171-Szenen-Pfad (@0x801152ac).
2. **Clip-Map 0x21** in `re15_victim_clip_map`: intro=0 (@0x80115a18), hold=1 (@0x80115a88), release=2 (@0x80115ad4); KEIN Collapse-Clip (Krähe devourt nie, §2.2). Frame-Längen 14/36/20 (EM021-EDD @blob+0x49d4, §1.5).
3. **Phasen**: Intro spielt Clip 0 bis Wrap (Blend 7 @0x80115a28), Hold loopt Clip 1 (kein Selbst-Exit @0x80115aa8), Push→Release beim Mash-Out des Krähen-Budgets (`+0x9c<0` → `aca5a++` @0x801140b4-c0 — Port: `crow_struggle<0`-Zweig, dort statt nur `re15_crow_sub(e,14)` auch die Victim-Phase schieben) sowie beim Event-Flag z5:29 (Root @0x801120a0-e8, Port `s_crow_gflags & 0x4`); Release spielt Clip 2 bis Wrap, dann Steuerung zurück + `hit_react&=~1` + aca3c-Bits clear (@0x80115b40-6c). Beim Latch: `hit_react|=1` + aca3c|=0xC0 (@0x80115a3c-4c, nur Front).
4. **Geometrie-Fix**: für EM021 `skel_victim` mit bone_count/kf_size aus dem **dir[6]-Pool-Header (15/80 @blob+0x4afc)** und der **PL00-Hierarchie** aufbauen (das Original animiert das SPIELER-Modell; Paar C liefert nur EMR-Keyframes + EDD) — nicht aus dir[2] (13/72).
5. **SEs**: Front-FSM hat keine eigenen SEs; die Peck-SEs kommen von der Krähe (Se(2) alle 30, Se(0) alle 10, move[13]). Rück-FSM: SE `0x03020001` an den zitierten Sites (nur Szene).
6. **Stempel-Fix (§4.4)**: `re15_crow_screech` durch `re15_wound_add`-Bänder ersetzen (Tabelle §2.3), Inline-Duplikate am Dive entfernen; Grab/Strike stempeln über denselben Helper (@0x80113ef4/@0x801144bc).
7. **Flinch-Fixes**: Aim-Gate entfernen/invertieren (Aim wird vom Flinch abgebrochen — Original-Beleg: cmd-2-Write ohne jedes Gate @0x80113b00 + Dispatch-Ersetzung @0x80031c88); Richtungs-Quelle auf den Yaw-Vergleich (@0x8001a794-a4) umstellen und Mapping vorne→Clip 8 / hinten→Clip 9 (@0x80035e38/@0x80035fbc) herstellen.
8. **Wurf (Mode 6) — dokumentierte Rest-Lücke, NICHT still degradieren**: Rück-Grab + Wurf gehören zur 1171-Event-Szene (behav-0x40-Krähen). Minimal korrekt bis zum Szenen-Ticket: die Szene ist im Port ohnehin nur über den z3/194-Zweig erreichbar; solange sie nicht gebaut ist, existiert der Wurf-Pfad nicht (kein stiller Ersatz nötig). WENN sie gebaut wird: Wurf-Clip 2 aus Paar C, dann PLW-Clips 1→2-Loop (@0x80051854/@0x800518c8, Bank B @0x80051884-88), Exit = nächster `Plc_dest` des Scripts (§3) — kein erfundener Timeout.

**Verifikation:** (1) probe_crow_flinch Lauf A nach dem Latch-Fix: während der 3 Grabs muss `pl->motion` die Victim-Clips 0→1(Loop)→2 zeigen (Frames 14/36/20), danach Idle; hit_react währenddessen 1. (2) Lauf C unverändert: Dive-Flinch Clip 8/9 nach Richtungs-Fix, auch mit gehaltenem R1. (3) gdigrab-Sichtprüfung (Skill `re15-port-visual-verify`): Peck-Halte-Pose sichtbar, Release-Stagger sichtbar.

---

## 6. Findings (kompakt)

| # | Conf | Kurz |
|---|---|---|
| F1 | belegt | Hook A [0x21] = zwei 5-Phasen-Opfer-FSMs (Front 0x801159bc / Rear 0x80115b80), alle f314 aus Paar C; Clips 0/1/2 = 14/36/20 Frames (EM021 dir[5]) |
| F2 | belegt | Hook B [0x21] = toter Code ([0]=jr-ra-Stub, kein cmd-6-Writer); player_hit_chain §1.3-Korrektur |
| F3 | belegt | Stempel-Sites @0x8011620c-70 = Attack-Site-Helper 0x801161e8 (vert-Bänder), Caller Dive/Grab/Strike |
| F4 | belegt | Wurf-Push = z5:29 (ROOM1171-SCD @0x1404; Root @0x801120a0-e8); Mode-6-Exit = Script-`Plc_dest` (Re-Init @0x80041bf8-c0c) — knockdown §7.1 für die Szene geschlossen |
| F5 | belegt (gemessen) | Port: Grab ohne jede Victim-Anim (Sentinel-Pose) = der Nutzer-Report; Dive-Flinch funktioniert im sauberen Zustand |
| F6 | belegt | Port-Aim-Gate unterdrückt den Dive-Flinch — Original cmd 2 unconditional |
| F7 | wahrscheinlich | Port-Flinch-Richtung invertiert + positions- statt yaw-basiert (a780 @0x8001a794-a4; [2]→Clip 8 @0x80035e38, [3]→Clip 9 @0x80035fbc) |
| F8 | belegt | `re15_crow_screech` = Fehldeutung des Wund-Stemplers (Panels als SE-Nummern); Stempel unvollständig |
| F9 | belegt | EM021-Victim-Bank-Geometrie-Falle: Parser nimmt dir[2] (13/72) statt Pool-Header (15/80) |

## 7. Offene Punkte (ehrlich OFFEN)

1. **`Plc_dest` mode 0x13 / flag 0x14** (ROOM1171 @0x1566/0x1572): Handler `@0x80073e30[19]` nicht disassembliert.
2. **Läuft die 1171-Szene über den Modes-4/5-Ankunfts-Bug hinweg?** (sub05 wartet auf z5:0; der Mode-5-Ankunftspfad hat den Flag-Set auf 0x800B11DE, knockdown §5.4) — Live-Capture der Szene (re15-room-capture, z3/194) steht aus.
3. **FLIGHT-2-Handler sub 3/4/5/6** (0x801153ac Wegflug→re-attack sub 1 @0x801154e4; 0x8011553c z5:30-gegated; 0x80115830/0x80115910) nur teil-disassembliert.
4. **Visuelle Clip-Identitäten**: EM021-Victim-Clips 0/1/2 und PLW-Clips 1/2 (Liegen) sind sequenz-, nicht sicht-verifiziert (Render-Katalog/gdigrab aussstehend).
5. **Aim-Latch-Beobachtung** (Probe: nach langem R1-Lauf + `re15_actor_init` blieb `re15_player_aim_active()==1` dauerhaft): vermutlich Probe-Artefakt (Actor-Reset mitten im Aim), aber ein echter Latch-ohne-Release-Pfad ist nicht ausgeschlossen — separater Kurz-Audit.
6. **`0x800acc0c`-Semantik** (Opfer-FSM Phase 0 setzt 1; Konsument FUN_80024c30) — weiterhin offen (= crow_1170 D7).
7. **SE-Code `0x03020001`** (Rück-FSM): Bank/Rec-Zuordnung im Port nicht gemappt (nur Szene relevant).
