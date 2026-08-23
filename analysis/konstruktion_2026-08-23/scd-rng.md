# SCD-RNG 0x8003ea3c + Opcode 0x28

Alle RE-Fragen sind beantwortet. Hier die Rohdaten für das Dossier.

---

# SCD-RNG 0x8003ea3c — RE-Dossier (Rohdaten)

## 1) MECHANISMUS (vollständig belegt)

### 1.1 Der RNG-Step `FUN_8003ea3c` — 32-bit-Rekurrenz, publiziert nach work_vars[13]

Disasm (PSX.EXE, vollständig):
```
8003ea3c: lui   a1,0x800b
8003ea40: addiu a1,a1,16236        ; a1 = &DAT_800b3f6c (state)
8003ea44: lw    v0,0(a1)           ; v0 = state
8003ea4c: sll   a0,v0,1            ; a0 = state << 1
8003ea50: srl   v1,a0,16           ; v1 = (state<<1) >> 16
8003ea54: addu  v0,v0,v1           ; v0 = state + carry-half
8003ea58: andi  v0,v0,0xffff       ; lo = (state + ((state<<1)>>16)) & 0xffff
8003ea5c: lui   v1,0xffff
8003ea60: and   a0,a0,v1           ; hi = (state<<1) & 0xffff0000
8003ea64: addu  v0,v0,a0           ; state' = hi | lo   (lo < 0x10000 → addu == or)
8003ea68: sw    v0,0(a1)           ; DAT_800b3f6c = state'
8003ea70: sh    v0,4074(at)        ; DAT_800b0fea = (s16)state'   ← work_vars[13]
8003ea74: jr    ra
```
Formel: `t = state<<1; state = (t & 0xffff0000) | ((state + (t>>16)) & 0xffff); work_vars[13] = (u16)state`.
- `DAT_800b0fea` = `0x800b0fd0 + 13*2` — 0x800b0fd0 ist die game-weite work_vars-Basis (belegt u.a. `lh 0x800b0fd0+idx*2` @0x80041524, @0x8003f620, @0x80041138; Port scd_vm.c dokumentiert dieselbe Basis).
- **state==0 ist Fixpunkt** (bleibt 0). Initialwert im EXE-Image: **0x00000000** (Datei-Offset 0xA476C in `info/Re1.5/PSX.EXE`, t_addr=0x80010000+0x800-Header) → der RNG ist tot, BIS der Seed geschrieben wird.

### 1.2 Der SEED — Konstante 0x138201C3, geschrieben im New-Game/Session-Init (Ghidra-blinder Fleck)

Ghidra listet für `DAT_800b3f6c` nur XREF[2] (R/W in FUN_8003ea3c selbst) — **falsch**: Byte-Scan über die EXE fand einen dritten, nicht disassemblierten Store. Die ganze Region ist im Dump roh (`??`-Bytes ab Zeile 147772), deshalb fehlen Funktion + Xref komplett:
```
8003e4f4: addiu sp,sp,-24          ; Funktion "FUN_8003e4f4" (Session-/New-Game-Reset)
8003e4f8: lui   v1,0x1382
8003e4fc: ori   v1,v1,0x1c3        ; v1 = 0x138201C3
...
8003e520: lui   at,0x800b
8003e524: sw    v1,16236(at)       ; DAT_800b3f6c = 0x138201C3   ← DER SEED
8003e528: sb    zero,0(v0)         ; DAT_800afbb4 = 0
8003e52c-..: Schleifen: Flag-Bänke 0x800b1444/1484/14a4/14c4 (8 Wörter), 0x800b1018+0x800b0080 (4), 0x800b1030+0x800aba14 (2), 0x800b1028=0
8003e5c0: jal 0x8003ebf4           ; work_vars[0..3] = -1 + Bank5-Wort1 = 0
8003e5c8: jal 0x8003edbc           ; AOT-Slots (0x800ac9b0, 0x20 Wörter) + 0x800afbb4 = 0
```
(FUN_8003e4f4 ist dem Port schon als New-Game-Init bekannt: `re15_itembox.c:21` zitiert seine Zero-Loops.)

Aufrufkette (beide Caller ebenfalls in Ghidra-rohen Regionen, per JAL-Encoding-Scan gefunden):
- `@0x8001c96c jal 0x8001d22c` — FUN_8001c958 = Ingame-Hauptfunktion, ruft den Session-Init EINMAL beim Spielstart, dann `j 0x8001cbbc` in die Frame-Schleife.
- `0x8001d22c` (Session-Init, endet @0x8001d5f8, direkt vor dem Raumlader FUN_8001d600): `@0x8001d570 jal 0x8003e4f4` (Seed) → `@0x8001d5ac jal 0x800396fc` (Raum-Setup → `@0x80039a00 jal FUN_8003ef6c`).

**Konsequenz: NULL Entropie.** Der Seed ist eine Compile-Zeit-Konstante; der RNG-Strom ist für jede Session identisch und hängt nur von der Schrittzahl ab. Deckt sich mit Memory `reai-v2-rng-determinism` (der andere RNG @0x800AC774 ist ein separates System).

### 1.3 Die drei Aufrufstellen + Frame-Reihenfolge (Ghidra-Xrefs, vollständig)

`FUN_8003ea3c` XREF[3] (ghidra1_V2.txt Z.148452):

**(a) Raum-Init `FUN_8003ef6c`** — Step als ALLERERSTE Aktion, 1× pro Raumladen:
```
8003ef6c: addiu sp,sp,-24
8003ef74: jal 0x8003ea3c    ; ← RNG-Step (VOR allem anderen)
8003ef7c: jal 0x8003ebf4    ; wv[0..3]=-1-Wisch
8003ef84: jal 0x8003ecec    ; Raum-Scratch (wv[0x12]=0xFF, wv[0x11]=0, ...)
...       main00 (+0x40)/sub00 (+0x44) laden, Dispatcher FUN_8003f0a0 DIREKT @0x8003f018
```

**(b) Gameplay-Frame `FUN_8003f038`** (gerufen `@0x8001cdec jal 0x8003f038`, VOR Gegner-AI @0x8001ce04 und AOT-Scan @0x8001ce1c):
```
8003f04c: bne v0,zero,0x8003f090   ; Pause-Gate 0x02000000: gesetzt → KEIN Step, KEIN Dispatch
8003f054: jal 0x8003ea3c           ; ← RNG-Step (1. Aktion nach dem Gate)
8003f064-84: sub01-Reseed (jal 0x8003ee3c @0x8003f080)
8003f088: jal 0x8003f0a0           ; VM-Dispatcher
```
→ **work_vars[13] trägt in jedem Gameplay-Frame einen frischen Wert, BEVOR irgendein Skript läuft.** Der Ende-Wisch FUN_8003ebf4 @0x8003f18c betrifft nur wv[0..3], NICHT wv[13].

**(c) Opcode 0x28** — Dispatch-Tabelle `0x800744a8[0x28]` = Eintrag @0x80074548 → `LAB_80040270` (Nachbar-Beleg: @0x8007454c → LAB_800402a0 = bekannter Cut-Freeze 0x29):
```
80040270: addiu sp,sp,-0x18
80040278: lw    v0,0x1c(a0)     ; VM-PC
80040280: addiu v0,v0,1         ; PC += 1 (1-Byte-Opcode)
80040284: jal   FUN_8003ea3c    ; ← Extra-Step auf Skript-Wunsch
80040288: _sw   v0,0x1c(a0)
8004028c: ori   v0,zero,0x1     ; return 1 (continue)
```

### 1.4 End-to-End-Verifikation gegen Hardware-RAM (78 saubere Savestates)

- Alle Boot-Savestates: state=0, wv13=0 (Seed noch nicht geschrieben). Alle Gameplay-Savestates: state≠0, wv13 = Low-Halfword des states (exakt das `sh` @0x8003ea70).
- **Formel+Seed bewiesen:** Vorwärts-Iteration ab 0x138201C3 erreicht die realen Savestate-Werte in plausiblen Schrittzahlen — `mzd_debugmenu`=Schritt 277, `orig_intro_late`=479, `room1140_entry`=1263, `mzd_stage1_briefing_live`=1475, `orig_lamp_start`=4019 (Schritte = Frames + Raumladungen seit Spielstart). 7/7 getestete Werte erreichbar. Periode > 3M Schritte.

### 1.5 GAME-WEITER NUTZUNGS-ZENSUS: **NIEMAND liest work_vars[13], NIEMAND ruft 0x28**

Walker (Klon des kampferprobten `re15_port/tools/aot_sce_census.py` inkl. der Sektionsende-Fixes vom 2026-08-19) über **alle 240 RDTs** (206 echte, 188 mit extra-Sektion), Sektionen main @0x40 + sub @0x44 + extra @0x48, **0 Desyncs**:
- **Opcode 0x28: 0 Vorkommen game-weit.**
- **work_var-Index 13 in ALLEN index-nehmenden Opcodes: 0 Vorkommen** — geprüft wurden 0x13 Switch(pc[1]), 0x1B For2(pc[5]), 0x23 Cmp(pc[2]), 0x24 Save(pc[1]), 0x25 Copy(pc[1]/pc[2]), 0x26 Calc(pc[3]), 0x27 Calc2(pc[2]/pc[3]), 0x35 Member_set2(pc[2]), 0x3D Member_get(pc[1]), 0x53 fade_set(pc[2]), 0x58/0x59 flag_ck2/set2(pc[2]), 0x5B Plc_cnt(pc[3]).
- Konservativ gegengeprüft per Roh-Byte-Scan (Upper Bound): alle Roh-Kandidaten liegen NACHWEISLICH außerhalb der gelaufenen Skript-Bereiche (Daten-Residuen hinter Evt_end / Sektionsende=EOF-Überschuss) und auf keinem Instruktions-Start.
- Zusätzlich EXE+alle Overlays byte-gescannt: **kein einziger direkter Lese-Zugriff auf 0x800b0fea** (einziger Treffer = das `sh` des RNG selbst @0x8003ea70).
- Var-Index-Histogramm der echten Skripte zur Einordnung: var4=342, var10=177 (Kamera-Cut), var0=39, var5=32, var16=18, var6=12, var7=10, var1=10, var18=4, var11=2. **Kein Raum braucht den RNG.**

RE2-Vergleich (Architektur-Evolution): RE2-Zwilling `FUN_80052ca0` (state DAT_800d8cb8, Output DAT_800d4826) = identische Formel, aber mit Gate `(DAT_800cfb74 & 0x1000000)` → dann stattdessen der echte RE2-PRNG FUN_80015fe8. RE1.5 hat dieses Gate noch nicht.

## 2) PORT-STAND

- `re15_port/engine/src/scd_vm.c:178` — Größentabelle `[0x28] = 1` (korrekt, LAB_80040270 PC+=1).
- **0x28 ist NICHT registriert** (register_opcodes hat im 0x2x-Block nur 0x23–0x27, 0x2A, 0x2D) → fällt auf `op_unknown` = 1-Byte-Skip ohne RNG-Step. Verhaltensneutral (0 Nutzungen game-weit), aber kein byte-true Handler.
- **Der LFSR existiert im Port nirgends**: kein Treffer für 800b3f6c/800b0fea/work_vars[13] in engine/src. `scd_vm_tick()` (scd_vm.c:558) spiegelt FUN_8003f038 (Pause-Gate @0x8003f040-4c, sub01-Reseed @0x8003f064-84, Ende-Wisch @0x8003f18c) — **ohne den Step @0x8003f054**. `re15_scd_room_setup` (scd_room_setup.c:232-238) spiegelt FUN_8003ef6c — **ohne den Step @0x8003ef74**.
- Session-Init-Spiegel existiert: `re15_gameflow_new_game` (engine/src/re15_gameflow.c:36); FUN_8003e4f4 ist dort als Vorbild bereits zitiert (re15_itembox.c:21, re15_itembox.h:62).
- Der deterministische Engine-RNG `re15_engine_rand8` (AI-Strom) ist ein SEPARATES System — nicht vermischen.
- VM-Tick-Aufrufstellen (für die Step-Zählung relevant): platform/pc/main.c:3307 (Boot-Init-Tick, room_init-Flag AUS), :3331 (sub00-Init-Lauf, Flag AN), :3722 (Frame); engine/src/scd_room_setup.c:237 (Tür-Pfad, Flag AN); platform/psx/main.c:213/293.

## 3) IMPLEMENTIERUNGS-PLAN

1. **State + Step** in `scd_vm.c` (State ins `g_scd`-Struct, `re15_scd.h`):
   `void scd_rng_step(void)`: `t=state<<1; state=(t&0xffff0000)|((state+(t>>16))&0xffff); g_scd.work_vars[13]=(int16_t)state;` — Kommentar `@0x8003ea4c-70`.
2. **Seed**: in `re15_gameflow_new_game` (re15_gameflow.c:36, dem FUN_8003e4f4-Spiegel): `g_scd.rng_state = 0x138201C3;` — Kommentar `@0x8003e4f8/fc (lui/ori) + @0x8003e524 (sw)`. CONTINUE/LOAD läuft im Original durch dieselbe Kette (0x8001d22c) → auch dort seeden.
3. **Verdrahtung, Zählparität beachten**:
   - `scd_vm_tick()`: Step direkt nach dem Pause-Gate, aber NUR wenn `!s_vm_room_init` (= FUN_8003f038 @0x8003f054; der Original-INIT-Dispatcher FUN_8003f0a0 steppt NICHT selbst). Der Boot-Tick pc/main.c:3307 läuft mit Flag AUS → sein 1 Step steht für den FUN_8003ef6c-Step des Boot-Raums; der sub00-Lauf :3331 (Flag AN) steppt nicht. Zählung == Original.
   - `re15_scd_room_setup` (Tür-Pfad, scd_room_setup.c ~Z.232, VOR dem Init-Tick): expliziter `scd_rng_step()` = FUN_8003ef6c @0x8003ef74 (1× pro Raumladen). PSX-main.c-Pfade analog prüfen (213/293).
4. **Opcode 0x28 registrieren**: `op_rnd`: `scd_rng_step(); t->pc += 1; return SCD_R_CONTINUE;` — byte-true LAB_80040270 (@0x80040280 pc+1, @0x80040284 jal, @0x8004028c return 1). Größentabelle stimmt schon.
5. **Determinismus-Politik**: kein Konflikt — das ORIGINAL ist hier selbst 100% deterministisch (konstanter Seed, Schrittzahl = Frames + Raumladungen + 0×Opcode). Kein Env-Schalter nötig; der Port wird durch die Übernahme determinismus-treuer, nicht weniger.
6. **Verifikation**:
   - Unit-Test: Seed 0x138201C3 → Schritte 277/479/1263/1475/4019 müssen exakt 0x3c205286/0x11d1fd86/0xc1e55c39/0xbf0f562e/0x7ba2ca47 liefern (= reale Hardware-RAM-Werte aus mzd_debugmenu/orig_intro_late/room1140_entry/mzd_stage1_briefing_live/orig_lamp_start — Hardware-Gegenprobe im Test eingebaut).
   - Parity: gleicher Raum + gleiche Frames → `g_scd.rng_state`/wv13 == Savestate-RAM 0x800b3f6c/0x800b0fea (re15-parity-verify-Harness).
   - Da kein Skript den Wert liest: Verhaltens-Regressionsrisiko null; das Ziel ist RAM-Parität.

## 4) OFFEN (ehrlich)

- **wv13==0 trotz state≠0** in 3 Savestates (lamp_1170_stage1, mzd_stage1_walked, mzd_title_after_death): irgendein Title-/Menü-Pfad wischt das work_vars-Array nach dem letzten Step (FUN_8003e4f4/ebf4/edbc tun es NICHT — belegt oben). Nicht weiter verfolgt, da folgenlos: wv13 wird in jedem Gameplay-Frame vor dem VM-Lauf neu publiziert und game-weit nie gelesen. Nächster RE-Schritt, falls je nötig: PCSX-Redux-Write-Watchpoint auf 0x800b0fea im Title-Flow (Skill re15-pcsx-watchpoint).
- Schrittzahl-Herleitung der Savestates (277/479/…) wurde als *plausibel* eingeordnet, nicht als exakte Frame-Rechnung nachgezählt — für die Formel-/Seed-Beweisführung irrelevant (Erreichbarkeit + Boot=0 + sh-Kopplung genügen), für einen Frame-genauen Parity-Test wäre die Zählung (1/Frame + 1/Raumladen, Pause-Gate zählt nicht) live zu bestätigen.
- Die exakte Semantik des Session-Init 0x8001d22c für CONTINUE/LOAD (Seed-Neuschreibung bei LOAD) ist aus der Kette belegt (Port-Kommentar pc/main.c:3290 nennt LOAD über dieselbe Kette), aber nicht separat im LOAD-Flow disassembliert worden.
- FUN_8001c958/0x8001d22c/FUN_8003e4f4 fehlen in RE15_FUN_CATALOG.md (Ghidra hat die Regionen nie disassembliert — Zeilen 95489/94050/147772 sind `??`-Bytes); Katalog-Nachtrag lohnt: 0x8001c958=Ingame-Main, 0x8001d22c=Session-Init, 0x8003e4f4=New-Game-State-Reset (Seed @0x8003e524).
