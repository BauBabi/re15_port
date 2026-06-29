# RE1.5 Port — Byte-true Audit (2026-06-28)

Multi-Agent-Audit: 20 Subsysteme, 123 WRONG/MISSING-Verdachtsfälle, **104 adversariell bestätigt**, dedupliziert auf 44 Fixes.

## Executive Summary

63 adversariell bestÃ¤tigte Funde, dedupliziert auf ~45 eigenstÃ¤ndige Fixes in 11 Subsystemen. Zwei dominante Fehler-Cluster: (A) der SCD-VM ist NICHT byte-true zur RE1.5-Dispatch-Tabelle @0x800744a8 â€” Opcode-GrÃ¶ÃŸen aus RE2 Ã¼bernommen (0x4C/0x4D/0x4E/0x4F/0x38/0x3B/0x50 falsch -> 8-22 Byte PC-Desync je Vorkommen), Steuerfluss (While/Ewhile/Break) und Var-Ops (Save/Copy/Calc) fehlen ganz, Cmp/Member_cmp lesen falsches Byte/falschen Operator/falsche Endianness. Diese Desyncs korrumpieren die Skript-Dekodierung kaskadierend und sind die Wurzel der meisten Verhaltensdivergenzen. (B) Drei unabhÃ¤ngige 1-2-LSB-Quellen in der Render-Mathematik (RotMatrix Single-Shift-Q36, negate-after-shift, sinf-Truncation, Kamera-Q12-Rundung +0.5) propagieren durch die Bone-Kette und produzieren die vom Nutzer wiederholt gemeldete Pixel-Verschiebung â€” das ist der konkrete AufhÃ¤nger fÃ¼r die Nutzer-Notiz und sollte mit hÃ¶chster PrioritÃ¤t gefixt werden. Daneben fehlt der KOMPLETTE Player-Damage-Pfad (FUN_80012d60, Damage-Tabelle, Death/Hit-State, I-Frames) â€” vier kritische PORT-LÃ¼cken. Reihenfolge unten: zuerst Byte-true-Mathematik (Pixel-Shift) + die kaskadierenden SCD-Opcode-GrÃ¶ÃŸen (blockieren jede tiefere Skript-Verifikation), dann fehlende Steuerfluss-/Var-Opcodes, dann Damage-System, AOT-Semantik, Audio, dann Doku/Kleinkram. Risiko-Hinweis: Cmp-pc[2]-Fix und Thread-Pool-Umbau brauchen Laufzeit-Verifikation (frÃ¼here PSX-Boot-Crashes); RotMatrix-Fix auf PSX braucht eigenes RotMatrix statt PSn00bSDK.

## Fix-Reihenfolge (priorisiert)

### #1 · HIGH · REPAIR · skeleton
**RotMatrix Off-Diagonal-Trunkierung byte-true (Single-Shift-Q36 -> per-Produkt >>12) + negate-before-shift**

- **Ort:** `re15_port/engine/src/skeleton_common.c:136 (m[1]), :138-160 (m[3]/m[4]/m[6]/m[7]), :149 (m[5])`
- **RE-Beleg:** RE_15_Quellcode_V2/RotMatrix.c:35-39 (per-Produkt >>0xc + short-Cast); ghidra1_V2.txt RotMatrix Triple-Trunk @0x80068238/0x80068250/0x80068268, subu @0x8006826c, sh @0x80068274; negate-before-shift @0x80068170-0x80068180 (m[1][2]) und @0x8006821c-0x8006822c (m[0][1]). Numerisch: 46.18% Matrix-Elemente weichen ab (SK-01), zusÃ¤tzlich 199958/200000 1-LSB bei m[1]/m[5] (negate-after-shift).
- **Fix:** Pro Off-Diagonal-Term innere Produkt-Trunkierung vorziehen: iv=(int16_t)((cz*sy)>>12); m[3]=(int16_t)((sz*cx)>>12)-(int16_t)((iv*sx)>>12) usw. fÃ¼r m[4]/m[6]/m[7]. FÃ¼r m[1]/m[5]: erst voll negieren, dann >>12: m[1]=(int32_t)((int64_t)(-(sz*cy))>>12). Auf PSX eigenes RotMatrix (Rx*Ry*Rz, DAT_800794c4), NICHT PSn00bSDK RotMatrix.
- **Risiko:** Kern der Pixel-Shift-Wurzel; betrifft JEDE Bone-Matrix. PSX braucht eigene RotMatrix-Impl. Gegen 50k-Winkel-Testvektor verifizieren (Audit lieferte Referenz).

### #2 · MEDIUM · REPAIR · skeleton
**PC-Trig sinf/cosf-Truncation -> gerundete LUT DAT_800794c4 (oder lroundf)**

- **Ort:** `re15_port/platform/pc/src/skeleton_trig_pc.c:21 ((int)(s*4096.0f)), :28 ((int)(c*4096.0f))`
- **RE-Beleg:** LUT DAT_800794c4 @0x800794c4 (ghidra1_V2.txt:263165ff), 4-Byte [sin16le][cos16le]; idx2 LUT=13 vs Port-Trunc 12, idx3 LUT=19 vs 18. 2008/4096 (49%) round!=trunc. SK-02.
- **Fix:** Byte-true: 4096-Wort-LUT DAT_800794c4 aus EXE extrahieren (low16=sin, high16=cos Q12), re15_sin_q12/re15_cos_q12 auf BEIDEN Ports als Tabellen-Lesung. Minimal: lroundf() statt (int)-Cast.
- **Risiko:** Speist jede sin/cos in mat3_from_euler â€” addiert sich zur RotMatrix-Drift. Muss mit Rank 1 zusammen getestet werden (gleiche Trig-Quelle). PSX-Polynom (skeleton_trig_psx.c) ebenfalls auf LUT umstellen.

### #3 · HIGH · REPAIR · camera-projection
**View-Matrix + Translationsvektor: float +0.5-Rundung -> Integer-GTE-Truncation**

- **Ort:** `re15_port/engine/src/camera_common.c:172 (Q12_ROUND), :173-181 (rot), :185-187 (trans)`
- **RE-Beleg:** FUN_80053ca4 @0x80053ca4: sll/div/mflo @0x80053d78-da4 (sp), @0x80053de4 (cp), @0x80053e94/ed4 (sy/cy); SquareRoot0=floor-isqrt (PSn00bSDK squareroot.s srl 12). GTE SAR ohne Round (psx-spx geometrytransformationenginegte.md:414-416). ROOM1100: 11/28 Trig-Elemente 1-LSB falsch mit +0.5.
- **Fix:** build_view als Integer-Pfad: len=isqrt(dxÂ²+dyÂ²+dzÂ²), sp=(int)(((int64)(pos_y-tgt_y)<<12)/len) [trunc] usw.; Matrix Integer-Q12 ohne +0.5; Newton-sqrt -> floor-isqrt (cam_isqrt Z.415). Translation t[r]=-((m[r][0]*px+m[r][1]*py+m[r][2]*pz)>>12) int64, >>12-trunc.
- **Risiko:** Dritte Pixel-Shift-Quelle, propagiert durch compose_view_bone. Q12-Rundung-Entfernung muss konsistent mit allen Matrix-Konsumenten sein.

### #4 · HIGH · REPAIR · scd-vm-core
**Opcode-GrÃ¶ÃŸen 0x38/0x4C/0x4D/0x4E/0x4F/0x3B/0x50 byte-true setzen (RE2-Importe entfernen)**

- **Ort:** `re15_port/engine/src/scd_vm.c:138-153 (s_opcode_sizes), :233 (0x4E-Alias)`
- **RE-Beleg:** Dispatch-Basis 0x800744a8. 0x38->LAB_800417ac addiu+0xc=12 (@0x800417f0); 0x4C->LAB_80040858 +0x12=18 (@0x80040890); 0x4D->LAB_800408a8 +0xa=10 (@0x80040900); 0x4E->LAB_80041980 +5 (@0x80041a6c); 0x4F->LAB_80016f20 +0x16=22 (@0x80016ffc); 0x3B->LAB_800405bc (pc[3]&0x80)?40:32; 0x50->LAB_80040644 (pc[3]&0x80)?30:22.
- **Fix:** s_opcode_sizes: [0x38]=12,[0x4C]=18,[0x4D]=10,[0x4E]=5,[0x4F]=22. Alias s_op_table[0x4E]=op_item_aot_set ENTFERNEN (Item_aot_set nur an 0x50). 0x3B/0x50 konditional Ã¼ber pc[3]&0x80 (siehe Rank 5). op_flr_set auf 12-Byte-Layout, op_sce_espr_kill (0x4C-Bindung) auf 18-Byte-Semantik. Java-Disassembler (0x4F=4, 0x4E=22 etc.) parallel korrigieren.
- **Risiko:** BLOCKIERT jede tiefere SCD-Verifikation â€” jeder Desync verschiebt alle Folge-Opcodes. Muss VOR Steuerfluss-Fixes erfolgen. Echte Handler-Semantik (Slot-Tabellen 0x800b2360/2368) ist teils noch zu portieren, aber die GrÃ¶ÃŸe zuerst.

### #5 · CRITICAL · REPAIR · scd-aot-door-cut
**Door_aot_set (0x3B) + Aot_set (0x2C) + Item_aot_set (0x50) konditionaler PC-Advance pc[3]&0x80**

- **Ort:** `re15_port/engine/src/scd_vm.c:2300-2347 (op_door_aot_set +=32), :1706-1772 (op_aot_set +=20), :2366-2384/2382 (op_item_aot_set +=22)`
- **RE-Beleg:** 0x3B LAB_800405bc: beq @0x80040624 -> +0x28(40)/+0x20(32) @0x80040630/0x80040634. 0x2C LAB_80040534: andi 0x80 @0x80040598 -> +0x1c(28)/+0x14(20) @0x800405a8/0x800405ac. 0x50 LAB_80040644: andi 0x80 @0x80040664 -> +0x1e(30)/+0x16(22). Belegt: ROOM4030/4031.RDT main00 @0x47E/@0x4A6 pc[3]=0xB1 (4P-TÃ¼r, 40B).
- **Fix:** op_door_aot_set: adv=(pc[3]&0x80)?40:32; op_aot_set: adv=(pc[3]&0x80)?28:20; op_item_aot_set: adv=(pc[3]&0x80)?30:22. 4P-Layout (zusÃ¤tzl. Rect-Punkte ab +0x20) parsen. s_opcode_sizes[0x3B] auf dyn (0xFF) o.Ã¤. markieren.
- **Risiko:** Kritisch fÃ¼r STAGE4-TÃ¼ren (8-Byte-Desync). GehÃ¶rt eng zu Rank 4. RE1.5 hat KEINEN separaten 0x68/0x67-Opcode â€” 4P-Variante ausschlieÃŸlich Ã¼ber Bit 0x80.

### #6 · HIGH · REPAIR · scd-vars-flags
**Cmp (0x23): var-Index pc[1]->pc[2] und Operatoren case3/case4 (>=/<=) tauschen**

- **Ort:** `re15_port/engine/src/scd_vm.c:2085 (var_idx=pc[1]), :2093-2095 (case2/3/4)`
- **RE-Beleg:** LAB_8003ff68: andi 0xff @0x8003ff80 -> var=pc[2]; srl>>8 @0x8003ff88 -> op=pc[3]; lh @0x8003ff74 LE-imm. switchD_8003ffc0: op2 (slt v0,a0,a1;xori) = v>=imm, op3 (slt v0,a0,a1) = v<imm, op4 (slt a1,a0;xori) = v<=imm. Java: SCDScriptDisassembler.java:998 var=data[offset+2].
- **Fix:** var_idx=t->pc[2]. Operatoren: case2:v>=imm; case3:v<imm; case4:v<=imm (Engine {0:==,1:>,2:>=,3:<,4:<=,5:!=,6:&}). pc[3]/pc[4..5] bleiben.
- **Risiko:** pc[2]-Umstellung verursachte frÃ¼her PSX-Boot-Crash (laut Port-Kommentar) â€” laut Audit weil Operator/Array-Semantik fehlte. Nach Fix gegen verifizierten PSX-Boot prÃ¼fen; pc[1] (Array-Selektor) ggf. modellieren.

### #7 · HIGH · PORT · scd-vars-flags
**Save (0x24), Copy (0x25), Calc (0x26), Calc2 (0x27) â€” Work-Var-Ops portieren**

- **Ort:** `re15_port/engine/src/scd_vm.c:173-302 (0x24/0x25/0x26/0x27 nicht in register_opcodes) -> op_unknown`
- **RE-Beleg:** DAT_800b0fd0=work_vars. Save LAB_80040018 @0x80040020-40 (dispatch @0x80074538). Copy LAB_8004004c @0x8004004c: work_vars[pc[1]]=work_vars[pc[2]], PC+=3. Calc LAB_8004008c: dst=pc[3],op=pc[2],imm LE pc[4..5],PC+=6,jal FUN_80040140. Calc2 LAB_800400dc: op=pc[1],dst=pc[2],src=pc[3],PC+=4. FUN_80040140: 12 Ops (+,-,*,/,%,|,&,^,~,<<,>>,>>).
- **Fix:** op_save: work_vars[pc[1]]=scd_read_le_s16(&pc[2]); pc+=4. op_copy: work_vars[pc[1]]=work_vars[pc[2]]; pc+=3. FUN_80040140-Tabelle als C-switch; op_calc/op_calc2 registrieren. Division-durch-0-Sonderfall (kein Schreiben) beachten.
- **Risiko:** Save ist Grundlage jeder Cmp/Switch-Logik â€” ohne sie bleiben alle Vars 0. Reihenfolge nach Rank 4/6 (GrÃ¶ÃŸen+Cmp mÃ¼ssen erst stimmen).

### #8 · HIGH · PORT · scd-control-flow
**While (0x0F), Ewhile (0x10), Break (0x1A) â€” Schleifen-/Switch-Ausstieg portieren**

- **Ort:** `re15_port/engine/src/scd_vm.c:173-302 (0x0F/0x10/0x1A nicht registriert) -> op_unknown`
- **RE-Beleg:** While LAB_8003f6f4 (@0x800744e4): block_length lh @0x8003f74c, loop-back thread+0x20 @0x8003f754, exit thread+0x60 @0x8003f76c, PrÃ¤dikat-Akku s2 @0x8003f7cc-810. Ewhile LAB_8003f878 (@0x800744e8): lw +0x20, sw +0x1c. Break LAB_8003fca8 (@0x80074510): lw v1,0x60(v1) @0x8003fca8, sw 0x1c @0x8003fcb0. Assets: room1090/sub03 @0x04 '0f 04 08 00', room1030/sub00 Switch+Breaks.
- **Fix:** op_while: block_length=lh@pc[2]; PrÃ¤dikatkette ab pc auswerten (AND default, OR per Flag); Loop-Back+Exit in per-Frame Loop-Slot; FALSE->Exit. op_ewhile: pc=Loop-Back. op_break: pc=Exit-PC der innersten Loop/Switch, Counter--. Erfordert per-Frame Loop-Stack (thread+0x8/+0x20/+0x60).
- **Risiko:** Pre-getestete Schleifen laufen sonst nur 1x; Break fÃ¤llt durch alle Cases. Braucht das per-Frame Loop-Slot-Modell, das auch Do (Rank 12) und Edwhile (Rank 11) teilen.

### #9 · HIGH · REPAIR · scd-control-flow ✅ ANGEWANDT 2026-06-28 (byte-true, build+ctest grün)
**Switch/Case/Default/Eswitch komplett byte-true ins unified Loop-Modell (Audit-„off-by-2" war zu klein gegriffen)**

- **Ort:** `re15_port/engine/src/scd_vm.c` op_switch/op_case/op_default/op_end_switch; `re15_scd.h` (switch_val-Feld entfernt); Test `test_switch_case_break` in `tests/unit/test_scd_opcodes.c`.
- **RE-Beleg (instruktionsgenau verifiziert, beide Wege + Agent):** Der echte Mechanismus ist NICHT „Case korrigiert sich selbst", sondern **op_switch (LAB_8003fa5c, dispatch 0x800744f4) scannt die GANZE Case/Default-Tabelle selbst** und springt direkt:
  - Header 4B: var_index lbu@0x8003fa78, block_length lhu@0x8003fa74, table=pc+4 @0x8003fa7c. PUSH wie Do: loop_count++ @0x8003fa90/94, loop_exit[idx]=(pc+4)+block_length @0x8003faa8/ac. cmp_val=(s16)work_vars[var] lh@0x8003fad8.
  - Scan LAB_8003fadc: **0x15 Default → pc=default+2 @0x8003faf0, loop_count BLEIBT gepusht**; **0x16 Eswitch (kein Match) → pc=eswitch+2 @0x8003fb20, loop_count-- @0x8003fb24/28**; **0x14 Case** (block_len lhu@+2, value lh@+4): Match → pc=case+6 @0x8003fb0c (loop_count bleibt); kein Match → a3+=6+block_len @0x8003fb0c/fb14.
  - Standalone Case (LAB_8003fb38)=pc+6 (reiner Fall-through, KEIN Vergleich). Default (LAB_8003fb50)=pc+2. Eswitch (LAB_8003fb68)=loop_count-- + pc+2.
- **Fix (angewandt):** op_switch macht den Tabellen-Scan inline und pusht/poppt das unified Loop-Modell — Break (0x1A) verlässt den Switch über loop_exit, Eswitch balanciert loop_count. op_case=+6, op_default=+2, op_end_switch=loop_count--/+2. `switch_val`-Feld entfernt (kein Konsument mehr). Test deckt Match-mid-table+Break, erstes-Case-Match und No-Match→Default ab (loop_count balanciert je auf 0).
- **Index-Konsistenz belegt:** op_do (LAB_8003f8bc) und op_break (LAB_8003fca8) nutzen dieselbe Push-am-neuen-Index/Pop-Konvention (PSX 1-indexiert → Port konsistent 0-indexiert).

### #10 · HIGH · REPAIR · scd-message-event ✅ ANGEWANDT 2026-06-28 (byte-true, build+ctest grün)
**Evt_chain (0x03) No-Op -> In-place-Thread-Reinit; Evt_exec cond<10-Direktslot**

- **Ort:** `re15_port/engine/src/scd_vm.c` op_evt_chain + op_evt_exec; Tests `test_evt_chain_reinit` + `test_evt_exec_direct_slot`.
- **RE-Beleg (instruktionsgenau, eigener Trace + Agent: 4/4 Claims CONFIRMED):** Evt_chain LAB_8003f270 (@0x800744b4): sub_id=(u8)pc[3] @0x8003f280, jal FUN_8003edec — **FUN_8003edec reinitialisiert den Thread IN-PLACE** (KEIN memset): active=1 @0x8003edf0, loop_count=-1 @0x8003ee04 (Port-empty=0), block_sp-Reset thread+0x140 @0x8003ee14, PC=sub_table_base+base[sub_id*2] @0x8003ee38; **call_depth/work/locals/sleep BLEIBEN** (Audit-„Reset call_depth/sleep" war FALSCH). Evt_exec LAB_8003f2a0: cond=(u8)pc[1], sub_id=(u8)pc[3], pc+=4, FUN_8003ee3c(cond,sub_id). FUN_8003ee3c (Mode DAT_800b3f7a, Normal=0 agent-belegt 5 Writer): cond<10 → Slot=cond DIREKT (unbedingter Reinit); cond>=10 → Scan [2..8]+Fallback 9 (Mode!=0: <14 direkt / Scan [10..12]+13). Asset room1021/sub02 'Evt_exec(9,0)' → Slot 9 (vorher GEDROPPT).
- **Fix (angewandt):** op_evt_chain = in-place Reinit (pc=target, loop_count=0, block_sp=0, KEIN pc+=4, call_depth bewahrt). op_evt_exec = cond<10 → Direkt-Slot=cond (force-overwrite, byte-true); cond>=10 → Auto-Scan der Port-Event-Range [10..23]. Force-overwrite + cond<10-Direktslot fixen den room1021-Drop.
- **#24-Kopplung (deferred-Rest):** Die exakte PSX-Auto-Scan-Range [2..9]/[10..13] + Mode-Flag DAT_800b3f7a hängt am Thread-Pool-Umbau (24→14, Rank 24). Der Port nutzt Slot 2 dediziert + Event-Range [10..23] → Auto-Scan bleibt [10..23] bis #24. ROOM1170 Evt_exec(255,11) (Intro) unverändert in [10..23] → keine Regression.
- **Risiko:** Evt_chain-No-Op war Kontrollfluss-Korruption (chained Sub lief nie). Laufzeit: Headless-Boot 12s ohne Crash; 26/26 ctest grün.

### #11 · CRITICAL · REPAIR · scd-actor-model
**Member_set/cmp id-Tabelle: 1:1 RE1.5-FUN_8004116c statt RE2-Heuristik (id12->+0x09 nicht Y!)**

> ✅ **ERLEDIGT 2026-06-28 (byte-true)** — `re15_actor_get_member`/`set_member` (actor_common.c) nutzen jetzt die
> RE1.5-DIREKT-Tabelle aus `FUN_8004116c` (set, ghidra:151322-151380) / `FUN_80041358` (get, :151486-151575);
> `re15_to_re2_member_id()` GELÖSCHT (war Fiktion). 3 Call-Sites (op_member_set/cmp/set2) übergeben die rohe RE1.5-id.
> 4 Orphan-Felder ergänzt (member_0c@+0x0c, grid_id@+0x09, member_0a@+0x0a, member_0b@+0x0b) → alle 20 ids round-trippen.
> **Bug behoben:** id12 (häufigster Member_set, +0x09 grid-id) schrieb via RE2-0x0C → Leon.y; jetzt grid_id. Auch
> id6/8/9/10/11/13/16/17/18/19 waren falsch/gedroppt (z.B. id8 RE1.5=state, alt=motion). **ids 0–5 (Koords/Rot) sind
> byte-identisch zu vorher → ROOM1170 unverändert, keine Regression.** GET-Vorzeichen via Port-Feldtypen exakt (lbu
> zero-, lh sign-extend). Test `tests/unit/test_member.c`. 28/28 ctest grün, Headless ~9s ohne Crash.

- **Ort:** `re15_port/engine/src/re15_to_re2.c:55-66, actor_common.c:94,131-159 (case 0x0C->a->y)`
- **RE-Beleg:** FUN_8004116c (Member_set core, LAB_800410b8 @0x80074578): id12 sb +0x09 @0x800411f4; id1=Y sw +0x38; id8 sb +0x04 @0x800411d8; id6 sw +0x00; id9 sb +0x05; id10 sb +0x06; id18 sb +0x82; id19 sh +0x1ba. id0x0C = 117 Vorkommen (hÃ¤ufigster). Asset room1070/sub02 'Member_set(12,138)'.
- **Fix:** re15_to_re2_member_id durch 1:1-Tabelle aus FUN_8004116c ersetzen: 0->X,1->Y,2->Z,3->rotx,4->roty,5->rotz,6->+0x00,7->+0x0c,8->+0x04,9->+0x05,10->+0x06,11->+0x07,12->+0x09,13->+0x08,14->+0x0a,15->+0x0b,16->+0x1c4,17->+0x98,18->+0x82,19->+0x1ba. Alle 20 ids explizit, keine RE2-Passthrough-Heuristik.
- **Risiko:** id12 schreibt aktuell 138 nach Leon.y (zieht Spieler unter den Boden). Behebt id6/8/9/10/18/19 mit. Member_set Y-Wert wird LE gelesen (korrekt).

### #12 · HIGH · REPAIR · scd-actor-model
**Member_cmp: Operator-Tabelle korrigieren + Vergleichswert BE->LE**

> ✅ **ERLEDIGT 2026-06-28 (byte-true)** — Operator-Tabelle + LE-Wert waren bereits in einer früheren Welle angewandt
> (op_member_cmp: Wert `scd_read_le_s16(&pc[4])`, switch `{0:== 1:> 2:>= 3:< 4:<= 5:!= 6:&}`, op≥7→false — verifiziert
> gegen `LAB_80041290`/`switchD_800412f0` ghidra:151416-151468). Der EINZIGE Restfehler war die geteilte kaputte
> id-Übersetzung (Wert kam vom falschen Feld) → mit **#11** behoben (op_member_cmp ruft jetzt `re15_actor_get_member`
> mit der rohen RE1.5-id). VM-Tests in `test_member.c` (Feld-Quelle id19=hp/id8=state, LE-Wert hp=300, Operatoren 0..6+op7).

- **Ort:** `re15_port/engine/src/scd_vm.c:2112 (scd_read_be_s16), :2119-2125 (switch cmp_op)`
- **RE-Beleg:** LAB_80041290 (@0x800745a0, Opcode 0x3E): lh s1,0x4(v0) @0x800412ac = LE-VALUE (gleicher lh wie Member_set, dort liest Port LE). switchD_800412f0: {0:==,1:>,2:>=,3:<,4:<=,5:!=,6:&}. Port: BE + {0:==,1:!=,2:<,3:>}.
- **Fix:** arg=scd_read_le_s16(&pc[4]). switch: 0:cur==arg;1:cur>arg;2:cur>=arg;3:cur<arg;4:cur<=arg;5:cur!=arg;6:(cur&arg)!=0;default:false.
- **Risiko:** VerfÃ¤lscht jede Gegner-/Cinematic-If mit Member_cmp op>=1. Eng mit Rank 11 (gemeinsame id-Tabelle / re15_actor_get_member).

### #13 · CRITICAL · PORT · player-damage
**Kompletter Player-Damage-Pfad FUN_80012d60 portieren (Handler + Damage-Tabelle + I-Frames + Death/Hit-State)**

> ✅ **ERLEDIGT 2026-06-28 (Player-Branch, byte-true)** — `re15_port/engine/src/re15_damage.c` + `include/re15_damage.h`,
> actor-Felder `hit_react`(+0x93)/`status_flags`(+0x98), Test `tests/unit/test_damage.c` (6 Tests, 27/27 ctest grün).
> Verifiziert instruktionsgenau gegen `ghidra1_V2.txt:77607-77814` (Disasm == Decompile). `re15_player_take_damage()`:
> Hit-Once-Gate (+0x93 bit0 @80012e30) → HP-=`re15_damage_table[type]` (@80012e44-64) → type<2 Bleed-Roll
> (+0x98 bit0x2 via 2×RNG&1 @80012ea4) → state=2/sub_state_1=front-back/sub_state_2=0/+0x93|=1 → signed HP<0 → state=3.
> **Audit-Korrektur:** Damage-Tabelle hat **11** Einträge, `DAT_8006f418[10]=0` (ghidra:223455-223478) — der Audit
> listete nur 10 (`[10]=0` fehlte). Reaktions-Tabelle `DAT_8006f430` ebenfalls 11 (`[10]=0x14`).
> **Caller verifiziert:** `FUN_80017fa4` (Gegner-Lunge, Dispatch-Tab @0x80071da4) ruft `FUN_80012d60(0x1f4, hitbox, 0)`
> → `clear a2` @80018004 = **attack_type 0 = 10 dmg** (nicht "a2=2"; der `a2=2`-Audit-Wert gehört zum 2. Call-Site
> @800185b8 = eine ANDERE Attack-Action, ungeprüft, deferred).
> **DEFERRED (In-Game-Trigger):** Gegner-Attack-FSM (Dispatch @0x80071da4) + Actor-vs-Actor-Hitbox `FUN_8002b5d0`
> sind NICHT portiert (Port hat keine Gegner-AI/Hitbox) → nichts ruft den Resolver in-game; er ist der getestete
> byte-true Kern, den die Gegner-AI-Phase verdrahtet. Hit-Once-Clear-Trigger (`re15_player_clear_hit_guard`) gehört
> zur selben deferred FSM. Front/Back-Hurt-Anim (sub_state_1) ist faithful aus der Disasm abgeleitet, aber
> unbeobachtbar bis distinkte Hurt-Clips existieren. Bleed-RNG cycle-exakt unmöglich (leftover-Register), 1/4-Wkt
> erhalten (Test: 507/2000). Verwandt: **#22** (Idle-FSM auf state==Idle gaten — Damage setzt state jetzt), **#41**
> (Poison-Drain HP-=2; Bleed-Status `status_flags` wird jetzt gesetzt, der Drain selbst bleibt deferred).

- **Ort:** `re15_port/engine/src/* (keine Implementierung, kein Aufrufer); player_common.c (nur Kommentare :69-74)`
- **RE-Beleg:** FUN_80012d60 @0x80012d60: I-Frame-Gate DAT_800acae7&0x1 @0x80012e2c; HP-=dmg @0x80012e44-64 (HP=DAT_800acaee=player+0x9a); dmg-Tab DAT_8006f418 @0x8006f418 = {10,20,1000,1000,1000,50,100,200,300,1000}; Hit-State DAT_800aca58=2 @0x80012ebc; Death signed bgez @0x80012ee8 ->=3 @0x80012ef4; NPC-Clip-Tab DAT_8006f430={03,03,09,0A,0B,0E,0F,10,11,12,14}. Caller @0x80018008 (a2=0), @0x800185b8 (a2=2 Instakill).
- **Fix:** re15_player_damage(type): I-Frame-Gate (player+0x93 bit0) prÃ¼fen; HP-=damage_table[type&0xff]; static const int16_t damage_table[10]={10,20,1000,1000,1000,50,100,200,300,1000}; type<2 Sonderfall (FUN_800453d0(10)+25%-Chance DAT_800acaec|=2); (int16_t)hp<0 -> Death-State=3 sonst Hit-State=2. DAT_800aca58-Ã„quivalent als Player-FSM-State. NPC-Damage generisch Ã¼ber actor+0x9a, DAT_8006f430 als Clip-LUT.
- **Risiko:** 5 zusammenhÃ¤ngende kritische LÃ¼cken (Handler/Tabelle/Death/I-Frame/NPC). Braucht Enemy-Attack-Hitbox-Test FUN_8002b5d0 (auch nicht portiert) als Aufrufer. Player-State-Var DAT_800aca58 verzahnt mit Idle-FSM (Rank 22).

### #14 · HIGH · REPAIR · aot-runtime
**AOT-Scan: Edge-Trigger entfernen (jeder Frame inside), ALLE Entities (mask 1/2/4), generischer Band-Gate mit 0x80-Ignore**

- **Ort:** `re15_port/engine/src/aot_common.c:213-417 (was_inside Edge), :213 (nur player_x/z), :369-381 (Band nur DOOR), game_step_common.c:53,72`
- **RE-Beleg:** FUN_80042bac @0x80042bac: kein Vorframe-Inside-Feld, AUTO param_3==0 feuert + goto LAB_80043018 (kein Edge). FUN_800436a8: 3x jal @0x800436cc(Player=1)/@0x80043724(Item=2)/@0x8004378c(NPC=4). Band-Gate @0x80042cac: (pbVar9[2]&0x80)||entity+0x82==pbVar9[2].
- **Fix:** Edge-Gate (was_inside) entfernen; AOT feuert jeden Frame inside; Re-Trigger nur Ã¼ber handler-seitiges Aot_reset/Self-Disable. Entity-Maske ins Flagbyte; re15_aot_scan Ã¼ber Player/Item/NPC-Pools. Band-Byte+0x80-Bit pro Slot, generisch vor Geometrietest: (flag2&0x80)||(entity_band==aot_band).
- **Risiko:** Fundamentale Semantik-Ã„nderung â€” AOTs ohne Self-Disable feuern dann wiederholt (byte-true). Erfordert dass op_aot_reset korrekt arbeitet. Hoch riskant fÃ¼r bestehendes ROOM1170-Verhalten; gegen Savestate verifizieren.

### #15 · HIGH · REPAIR · aot-runtime
**TÃ¼r-Interaktion als Entity mit 9-Frame-Halte-ZÃ¤hler (AI-Schleife) statt Edge-Action-AOT**

- **Ort:** `re15_port/engine/src/aot_common.c:362-382, :412-413 (door_inside && g_aot_action_pressed Edge)`
- **RE-Beleg:** FUN_8002bd44/Halte-ZÃ¤hler obj+0x8c (DAT_800b3efc): Inkr @0x8002bf60-70, Reset @0x8002bf74; Held-Pad DAT_800ac768&1 @0x8002bf24; Band obj+0x82 @0x8002bf38; Reach FUN_8002d1e8 @0x8002bf50; Ã–ffnen bei ==9 @0x8002bf8c. Hauptschleife @0x8001ce14.
- **Fix:** TÃ¼r als Entity mit Frame-ZÃ¤hler: gehaltenes Action+Band+Reach -> ZÃ¤hler++; Abbruch->0; bei ==9 (und TÃ¼r-Flag 0x80 clear) Ã–ffnungs-Handler. In AI-Schleife-Phase (nach Player-State, vor Auto-AOT), nicht im AOT-Scan.
- **Risiko:** GroÃŸer Umbau (TÃ¼r raus aus AOT-Scan). HÃ¤ngt an Rank 14 (AOT-Scan-Refactor). Reach-Funktion FUN_8002d1e8 muss portiert sein.

### #16 · HIGH · REPAIR · scd-aot-door-cut
**Cut_chg/Cut_old: Cut-Freeze-Flag (DAT_800aca3c|0x100) setzen/lÃ¶schen**

- **Ort:** `re15_port/engine/src/scd_vm.c:941-955 (op_cut_chg), :2630-2636 (op_cut_old)`
- **RE-Beleg:** Cut_chg LAB_800402a0 (@0x8007454c): ori 0x100 @0x800402d4. Cut_old LAB_8004032c (@0x80074550): and ~0x101 @0x8004037c. Gate Auto-Scan @0x8001cce0 andi 0x100 -> bne Ã¼berspringt jal FUN_80014230 @0x8001ccec.
- **Fix:** op_cut_chg: g_scd.cut_auto_enabled=0 (Bit 0x100 SET, Auto-Scan aus). op_cut_old: g_scd.cut_auto_enabled=1 (Bit CLEAR). Bilden Freeze/Resume-Paar; unterdrÃ¼ckt RVD-Scan in aot_common.c:341.
- **Risiko:** Niedriges Risiko, klar lokalisiert. Verzahnt mit cut_auto_enabled-Gate (aot_common.c:341).

### #17 · MEDIUM · REPAIR · scd-control-flow
**Goto (0x17), Gosub/Return Block-/Loop-Stack-Unwind + per-Frame-Isolation**

> ✅ **ERLEDIGT 2026-06-29 (byte-true, empirisch belegt)** — `block_stack`/`block_sp` jetzt **pro Gosub-Frame**
> (indexiert nach call_depth, re15_scd.h). op_gosub initialisiert den neuen Frame mit leerem Block-Stack
> (byte-true LAB_8003fbe8); Return/Evt_end (call_depth--) restaurieren den Caller implizit. **op_goto** unwindt
> `block_sp[cd] = (s8)pc[1]+1` + `loop_count = (s8)pc[2]+1` (LAB_8003fb9c: PSX `base+(s8)pc[1]*4+4`, leer = -1 → 0).
> **Empirischer SCD-weiter Scan (alle 344 Subs):** **15/22 Gotos** nutzen pc[1]≠0xFF (echtes Out-of-Block-Unwind),
> **124/278 Gosubs** feuern aus einem offenen If-Block — der alte FLACHE Single-Stack + ignore-Goto-args
> korrumpierte die FALSE-Pop-Ziele. pc[2]≠0xFF: **0×** (Goto resettet Loops immer auf leer). Verifizierter
> Rotor-Goto (`17 FF FF 00 FE FF`, pc1=pc2=0xFF, kein umgebender Block) → Unwind auf 0 = No-Op (keine Regression).
> Tests `test_goto_block_unwind` + `test_gosub_block_isolation` (test_scd_opcodes.c). 28/28 ctest grün;
> ROOM1240→1170-Intro (Gosub/If/Goto-intensiv) ~10s clean.

- **Ort:** `re15_port/engine/src/scd_vm.c:807-827 (op_goto), :839-865 (op_gosub/op_return), re15_scd.h:78-89 (geteilte Stacks)`
- **RE-Beleg:** Goto LAB_8003fb9c: block_sp(thread+0x140)=(frame*32+0xc0)+pc[1]*4+4 @0x8003fbd4, pc[1]=Unwind-Level, pc[2]=Loop-Counter. Gosub LAB_8003fc18 Block-SP-Rebase @0x8003fc18-24, 0xFF-Init @0x8003fc10; Return-Restore @0x8003fc7c-9c. If-Push +0x4 @LAB_8003f328.
- **Fix:** op_goto: block_sp + do/for-Depth auf pc[1]-Niveau zurÃ¼cksetzen, Loop-Counter (pc[2]) restaurieren. op_gosub/return: Block-/Loop-Stacks pro Gosub-Frame (indexiert nach call_depth, oder save+null/restore).
- **Risiko:** Korrumpiert sonst FALSE-Pop-Ziele bei Goto-aus-offenem-Block. Setzt korrektes If/Else/Endif-Block-Modell voraus. Mittlere KomplexitÃ¤t.

### #18 · MEDIUM · REPAIR · scd-control-flow
**Edwhile (0x12) volle PrÃ¤dikat-Kette + Do (0x11) Exit-PC speichern + Return Root-Terminierung**

> ✅ **ERLEDIGT durch den #8-Loop-Umbau (verifiziert 2026-06-29)** — op_edwhile nutzt bereits die volle
> `scd_eval_pred_chain` (AND/OR-Prädikatkette via echten Dispatcher, nicht nur Ck); op_do pusht bereits
> `loop_exit[idx] = body+block_len` (Exit-PC). Beide #18-Hauptpunkte sind im aktuellen Code byte-true. Der
> dritte (op_return Root → SCD_R_FRAME_RET) ist äquivalent gelöst: **Evt_end (0x00) gibt 0 = SCD_R_FRAME_RET**
> zurück (op_evt_end:584), der Dispatcher poppt den Frame (scd_vm.c:519-531) bzw. terminiert bei call_depth==0 —
> die byte-true Root-Terminierung. Kein zusätzlicher Code nötig.

- **Ort:** `re15_port/engine/src/scd_vm.c:605-630 (op_edwhile), :584-592 (op_do), :856-865 (op_return Root)`
- **RE-Beleg:** Edwhile LAB_8003f930 (@0x800744f0): Kette dispatcht jeden cond-Opcode, AND/OR-Akku s2 @0x8003f9a8. Do LAB_8003f8bc (@0x800744ec): block_length lh @0x8003f8f0, Exit-PC thread+0x60 @0x8003f918. Return root: korrekt via Evt_end (return 2 YIELD); op_return root sollte SCD_R_FRAME_RET.
- **Fix:** op_edwhile: PrÃ¤dikatkette ab pc+2 generisch via Opcode-Dispatch (AND/OR-Akku) statt nur Ck. op_do: Exit-PC=pc+4+block_length in per-Frame Loop-Slot. op_return root: return SCD_R_FRAME_RET + active=0+break.
- **Risiko:** Edwhile bricht sonst bei Cmp/Member_cmp-Bedingungen sofort ab. Do-Exit-PC wird erst mit Break (Rank 8) relevant. Gemeinsames Loop-Slot-Modell mit Rank 8.

### #19 · MEDIUM · REPAIR · scd-vars-flags
**Ck (0x21) value>=2 Bitset-Semantik + Opcode 0x58/0x59 indizierte Flag-Ops (statt Plc_rot/Xa_on)**

> ✅ **ERLEDIGT/DEFERRED 2026-06-29 (byte-true RE + empirischer Scan):** **Ck (0x21) ist byte-true** (op_ck @scd_vm.c:1384:
> `(got!=0)^(value==0)`, voller value, re15_game_flag_get löst idx>31 auf — verifiziert vs LAB_8003fcf4; schon Wave-1).
> **0x58/0x59 RE't** (LAB_8003fd54 @disp 0x80074608 = indizierter Flag-CHECK; LAB_8003fe90 @0x8007460c = indizierter
> Flag-SET OR/AND/XOR; ghidra1:149868-149954): beide indexieren das Flag-Wort über **`DAT_800b0fd0[pc[2]]>>5`** =
> das **AOT-Zonen-Scratch-Array** (geschrieben von FUN_80042bac) → **an das deferred #14 AOT-System gekoppelt**.
> **Empirischer SCD-weiter Scan (344 Subs):** **0x58 = 0 Verwendungen** (Port-`op_plc_rot@0x58` ist totes RE2-Mapping,
> RE1.5 0x58 ist KEIN Plc_rot); **0x59 = 1 Verwendung** (room1030/sub09 `59 05 04 01` = Flag-Set op=1). Der Port-Handler
> (op_xa_on/op_plc_rot) advanced PC korrekt (size 4) → kein Desync, nur Action-Divergenz in 1 unverifiziertem Raum.
> → **0x58/0x59-Flag-Op-Port DEFERRED bis #14** (AOT-Zonen-Index DAT_800b0fd0); byte-true entkoppeln wäre ohne den
> AOT-Index ein Rateschritt (= exakt die Audit-Risiko-Notiz). Kein Blind-Fix.

- **Ort:** `re15_port/engine/src/scd_vm.c:1221 (op_ck got==value), :289 (0x58->op_plc_rot), :205 (0x59->op_xa_on)`
- **RE-Beleg:** Ck LAB_8003fcf4: value=pc[3] voll @0x8003fd1c, a1=(value==0) @0x8003fd30, XOR @0x8003fd50. 0x58 LAB_8003fd54 (@0x80074608), 0x59 LAB_8003fe90 (@0x8007460c): indizierte Flag-Ops DAT_800b0fd0, PTR_DAT_80074664, OR/AND/XOR.
- **Fix:** op_ck: cond=(got!=0)!=(value==0). 0x58/0x59: prÃ¼fen ob STAGE1-6-Skripte sie nutzen; falls ja op_idxflag_check/set portieren (zone=pc[1],var=pc[2],bit=pc[2]&0x1f,word=work_vars[var]>>5) und von Plc_rot/Xa_on entkoppeln.
- **Risiko:** Ck value>=2 real selten (Java maskiert &0x01). 0x58/0x59-Entkopplung erst nach Nutzungs-Scan â€” sonst Regress bei Cinematic-Plc_rot.

### #20 · HIGH · REPAIR · audio-vab
**ADPCM-Predictor-Reset bei Loop-Flag (0x04) entfernen (Hardware-DiskontinuitÃ¤t)**

- **Ort:** `re15_port/engine/src/vab_common.c:291-294 (if(flags&0x04){state1=0;state2=0;})`
- **RE-Beleg:** psx-spx soundprocessingunitspu.md:128-141 (Flag-Bit2=nur Copy-address-to-repeat-reg, KEIN Reset). vag2wav.c:150-177 (s_1/s_2 nie per Flag resettet). RE1.5 legt Loop-Marker auf Block 1 -> Port reiÃŸt Predictor-Kette an Block0->1.
- **Fix:** if(flags&0x04){...}-Block ersatzlos entfernen. Predictor-State nur einmal pro VAG am Decode-Start (f=0) auf 0 (geschieht schon L248-249). Loop-Start-Adresse separat erfassen ohne Predictor-Einfluss.
- **Risiko:** Niedrig, klar lokalisiert. HÃ¶rbarer Knack/DiskontinuitÃ¤t verschwindet. Java-Referenz PsxAdpcmDecoder.java:42-45 macht denselben Fehler (separat fixen).

### #21 · HIGH · REPAIR · render-pc-parity
**Letterbox-Balken halbtransparent counter-gerampt + Subtitle-Layer-Reihenfolge zuletzt**

- **Ort:** `re15_port/platform/pc/src/render_pc.c:553-559 (opake Balken), :536-540/553-559/566-571 (Layer-Reihenfolge), psx/render.c:230-234`
- **RE-Beleg:** FUN_80021a0c: Counter DAT_800b5568 +0x10..0xF0 @0x80021a40; RGB Top DAT_80072ed0 @0x80021ab4; SetSemiTrans STP=1 @0x80021284; Geometrie (0,0)-(320,24)/(0,216)-(320,240). main.c:68 Text-OT VOR Z.71 Fade/Z.73 Letterbox -> Subtitle oben.
- **Fix:** Balken als halbtransparentes Grau (RGB=lvl,lvl,lvl, lvl=Fade-Counter, STP=1), gekoppelt an Fade-Counter; PC via SDL-Blend statt opakem FillRect. Subtitle-Overlay als LETZTE Schicht (nach Letterbox+Fade) blitten.
- **Risiko:** Sichtbarer Cinematic-Ãœbergang. Braucht Fade-Counter-FSM-Modell + DR_MODE/ABR-Blend-Nachbildung. Mittlere KomplexitÃ¤t.

### #22 · MEDIUM · REPAIR · player-damage
**Player-State-Maschine DAT_800aca58 (11-case FSM, Hit/Death) statt 5 Idle-Phasen**

> 🔶 **RE'T + DEFERRED (an deferred Infrastruktur gekoppelt, 2026-06-29):** State-Dispatcher **FUN_80031c44**
> (ghidra1:127059-127096) liest JEDEN Frame `DAT_800aca58` (= player+0x4 = port `g_actors[0].state`, RE1.5 Member
> id 8) << 2 → `PTR_LAB_80073f90[state]`: state0/1=Idle/Gameplay, **state2=Hit FUN_80035af0**, state3=Death FUN_800366bc.
> Der State-**SET ist erledigt (#13** setzt state=2 hurt / 3 death). Der Hit-Handler **FUN_80035af0** (ghidra1:131869+)
> ist ein Sub-FSM (sub-states `DAT_800aca59`/`DAT_800aca5a` = port sub_state_1/2) das die **Hurt-Animation clip 0xa via
> FUN_8001f314** spielt, runterzählt und zu Idle zurückkehrt. **GEKOPPELT an deferred:** (1) Hurt/Death-Clips sind im
> Port NICHT verdrahtet (#13-Notiz „unbeobachtbar bis Hurt-Clips"); (2) der Damage-TRIGGER (Gegner-AI/Hitbox) ist
> deferred (#13) → state wird in-game NIE 2/3. ⚠️ Ein Idle-Gate OHNE Hit-Handler würde den Spieler nach einem Treffer
> in „hit" STRANDEN (kein Return-to-Idle) → KEIN sicherer self-contained Teil. → DEFER bis Gegner-AI-Phase + Hurt-Clips.
> Die HP→injured-Idle-Eskalation (clip 22/23 bei HP<50/30) ist dagegen schon erledigt (#13-HP-Set + bestehendes Idle-FSM).

- **Ort:** `re15_port/engine/src/player_common.c:160,192-226 (s_idle_phase)`
- **RE-Beleg:** FUN_80031c44 @0x80031c8c liest DAT_800aca58, Tabelle PTR_LAB_80073f90 (state2=Hit 0x80035af0, state3=Death 0x800366bc). Idle-FSM switchD_8003206c (11 cases, Jump-Tab 0x80010b38). Banks healthy @0x800320e8/injured @0x800322c0.
- **Fix:** Player-State-Var analog DAT_800aca58 einfÃ¼hren, die der Damage-Pfad (Rank 13) auf 2/3 setzt; Idle-FSM nur laufen wenn state==Idle. Setup/play-Paarung + expliziter Bank-Wechsel (WEAPON/COMMON).
- **Risiko:** Verzahnt eng mit Rank 13 (Damage setzt diese Var). Ohne sie kann Treffer Idle nicht unterbrechen. Mittlere KomplexitÃ¤t.

### #23 · MEDIUM · REPAIR · anim-locomotion
**BACK-WALK/Plc_dest 7/8 Clip 0 (180Â°) statt Clip 5+reverse**

- **Ort:** `re15_port/engine/src/player_common.c:56,173-174,241; re15_to_re2.c:99-101`
- **RE-Beleg:** BACK LAB_80031080 (@0x80073e4c idx4): clip=0 sb DAT_800acae8 @0x800310bc; 180Â°-Yaw FUN_800245d8(0x800) @0x800310ec. Plc_dest 7/8 LAB_80031080/800311f0: clip=0 @0x800310bc/@0x8003122c. Forward schreibt 5.
- **Fix:** Player-Back auf W01 Clip 0 OHNE REVERSE-Flag, Translation per 180Â°-Offset. re15_to_re2.c case 0x07/0x08 -> RE15_MOTION_RUN (Clip 0). Falsche Kommentare korrigieren.
- **Risiko:** Niedrig, klar belegt. Rein visuell (falsche Walk-Clip rÃ¼ckwÃ¤rts).

### #24 · MEDIUM · REPAIR · scd-message-event
**Thread-Pool-Modell: 24 uniform -> 14 in 2 Kontexten (0..9 Gameplay + 10..13 Event) + Slot-1-Repointing**

- **Ort:** `re15_port/include/re15_scd.h:41 (SCD_THREAD_COUNT=24), scd_vm.c:466-512, scd_room_setup.c:145-165`
- **RE-Beleg:** Haupt-Dispatcher Schranke sltiu 0xa @0x8003f180; Event-Loop ori 0xa @0x80016520, sltiu 0xe @0x80016634; Stride a2*0x170 @0x8003eef4. Slot-1-Repointing FUN_8003f038 jeden Frame auf sub_scd[1] (a1=1 @0x8003f084).
- **Fix:** Thread-Pool auf 14, zwei Slot-Klassen (Gameplay 0..9, Event 10..13). Slot 1 jeden Gameplay-Frame auf sub_scd[1] reinit. Erfordert Active-Message-Gate DAT_800aca40&0x200.
- **Risiko:** GroÃŸer Scheduler-Umbau. Beeinflusst Evt_exec-Slot-Wahl (Rank 10). Hoch riskant fÃ¼r bestehende Threads; sorgfÃ¤ltig gegen Savestate verifizieren.

### #25 · MEDIUM · REPAIR · scd-aot-door-cut
**Aot_on (0x47) Typ-Handler Ã¼ber Deskriptor (DAT_800ac9b0) re-dispatchen statt nur active-Bit**

- **Ort:** `re15_port/engine/src/scd_vm.c:2510-2518 (op_aot_on)`
- **RE-Beleg:** Aot_on LAB_800407bc (@0x800745c4): Deskriptor DAT_800ac9b0[slot] @0x800407ec, Typ @0x8004080c, Handler PTR_LAB_8007469c[type] @0x80040824, jalr @0x8004082c. Deskriptor geschrieben von Aot_set @0x80040584/Door_aot_set @0x8004060c.
- **Fix:** DAT_800ac9b0-Ã¤quivalente Deskriptor-Tabelle (Pointer pro Slot) fÃ¼hren; op_aot_on fÃ¼hrt Typ-Install-Pfad aus gespeichertem Deskriptor neu aus statt active-Bit zu setzen.
- **Risiko:** Niedrig sofern Slot vorher installiert. Divergiert nur bei Aot_reset-umgetypten AOTs. Braucht Deskriptor-Struktur (verzahnt mit Rank 14).

### #26 · MEDIUM · PORT · scd-actor-model
**Work_set kind=5 Zeiger-Tabellen-AuflÃ¶sung (DAT_800b23f4) portieren**

- **Ort:** `re15_port/engine/src/scd_vm.c:1880-1883 (else: work_slot=-1)`
- **RE-Beleg:** LAB_80040df4 @0x80040df4-0e0c: kind==5 @0x80040d90; Tabelle DAT_800b23f4, Laufzeit-BefÃ¼llung FUN_800161e0 @0x80016240. Dispatch @0x80074560 (0x2E). 5x in DOOR0003-0007.
- **Fix:** kind=5 als Routing-Pfad; DAT_800b23f4-Array-Semantik aus EXE bestimmen (welche Slots), auf Port-Pool (actor/prop) mappen.
- **Risiko:** Verwirft sonst Member_set/Plc_motion/Speed_set auf das work entity. Erfordert EXE-AuflÃ¶sung von DAT_800b23f4 (offene RE-Arbeit).

### #27 · MEDIUM · REPAIR · scd-message-event
**Message_on non-blocking (PC+=4, return 1) + eigener Wait-Opcode (0x0C) + Active-Guard + YES/NO 0xf9-Dispatch**

- **Ort:** `re15_port/engine/src/scd_vm.c:1039-1051 (op_message_on Park), msg_common.c:413-416 (Flag 12,31)`
- **RE-Beleg:** Message_on LAB_800404f4 (@0x80074554): pc+=4 @0x80040510, return 1 @0x80040520, non-blocking. Wait-Opcode 0x0C LAB_8003f4c4 (@0x800744d8): poll DAT_800b8520&0x80 @0x8003f4f8, yield. Active-Guard FUN_80027e68 @0x80027e7c (DAT_800b8520&0x80). YES/NO: FUN_8002877c 0xf9-Dispatch @0x80028820, KEIN (12,31)-Flag.
- **Fix:** Message_on immer pc+=4/return 1. Wait-Logik in Opcode 0x0C (pollt DAT_800b8520&0x80). Active-Guard vor re15_dialog_open. YES/NO-Ergebnis Ã¼ber DAT_800b8520&1 + 0xf9-Inline-Branch statt erfundenem Flag (12,31).
- **Risiko:** 0xf9-Dispatch unverifiziert (confidence medium im Audit). Message-Active-Gate DAT_800aca40&0x200 muss modelliert werden. Mechanismus-Umbau.

### #28 · MEDIUM · REPAIR · anim-locomotion
**Rotor-Audio: LautstÃ¤rke &0x7f-Mask (kein Clamp) + Y-Distanz-Term cam.y-|cam.y-heli.y|**

- **Ort:** `re15_port/engine/src/rotor_common.c:130-131, :101-104`
- **RE-Beleg:** FUN_80045a64: panL andi 0x7f @0x80045d28, panR @0x80045d3c (kein Clamp). Y-Term cam.y-|cam.y-heli.y| @0x80045b04-b20, r=SquareRoot0(v1Â²+dxÂ²+dzÂ²) @0x80045b38.
- **Fix:** panL=(panL-SE_ATTEN(di))&0x7f; panR analog (ohne if-Clamps). dy=cam_eye[1]-llabs(cam_eye[1]-heli_pos[1]); r=isqrt(dxÂ²+dzÂ²+dyÂ²).
- **Risiko:** Niedrig, klar belegt. Nur Rotor-SE-LautstÃ¤rke/Index in Unterlauf-/Y-FÃ¤llen.

### #29 · HIGH · PORT · collision-stair
**Slope/Diagonal-Kollisionstypen 2/4/5/6/7 portieren**

- **Ort:** `re15_port/engine/src/re15_collision.c:304-307 (DEFERRED)`
- **RE-Beleg:** Dispatch 0x800b2858 @0x8003b4a0; t2=LAB_8003d00c, t4=LAB_8003beb0, t5=LAB_8003c734, t6=LAB_8003cb9c, t7=LAB_8003c2cc. RDT: ROOM1010 t2, ROOM1050 t4/t5, ROOM10D0 t4-7.
- **Fix:** 5 Handler @LAB_8003d00c/8003beb0/8003c734/8003cb9c/8003c2cc transliterieren (Fixpunkt-intensiv), an Origin=0-Konvention anpassen; in Slope-Raum (ROOM1050) gegen Savestate verifizieren.
- **Risiko:** Spieler lÃ¤uft sonst durch SchrÃ¤gen (viele RÃ¤ume). ROOM1170-Fokus nur type1/3 -> kein Regress dort. AufwÃ¤ndige Fixpunkt-Transliteration.

### #30 · MEDIUM · REPAIR · collision-stair
**Treppen-Finalize ohne Forward-Eject (byte-true) + Ascend-Intro-Phase (clip5/fwd75)**

- **Ort:** `re15_port/engine/src/stair_common.c:140-169 (Eject-Schleife), :186 (clip fix), :180-194`
- **RE-Beleg:** Finalize LAB_80038e50 (ghidra1_V2.txt:136857-136893): 8 Stores, KEIN player+0x34/0x38/0x3c. Ascend LAB_80038850 caseD_0 @8003888c: fwd 0x4b(75), clip 0x5; Stepping caseD_2 @80038a00: clip 0x14(20), fwd 0xa(10).
- **Fix:** Finalize: nur Band+Clip+Control-Unlock, kein X/Z-Eject (setzt byte-genauen FK-Travel FUN_800390e0 voraus). Ascend zweiphasig: Phase0 clip5/fwd75 -> Phase1 clip20/fwd10-Stepping.
- **Risiko:** Eject-Entfernung erst nach byte-genauem FK-Travel mÃ¶glich (sonst unterschieÃŸt Spieler). Intro-Phase additiv, geringeres Risiko.

### #31 · MEDIUM · PORT · camera-projection
**Camera-Orbit (rcos/rsin Pos) aktivieren + PC-Projektion an GTE-RTPS angleichen**

- **Ort:** `re15_port/engine/src/camera_common.c:359-372 (Orbit SKIPPED), platform/pc/main.c:1060-1061 u.a. (Projektion)`
- **RE-Beleg:** FUN_80015850: cam.x=(rcos(yaw)*dist>>12)+ptr[+0x14] @0x800158d4, cam.z=(rsin*dist>>12)+ptr[+0x1c] @0x80015948. RTPS: psx-spx geometrytransformationenginegte.md:418-419 (div-vor-mul, gerundetes Reziprok), UNR-Tabelle :605-657.
- **Fix:** Orbit: cam_pos[0]=(re15_cos_q12(yaw)*dist>>12)+orbit_center_x usw.; rcos/rsin gegen re15_cos_q12 verifizieren; Orbit-Center-Offset (+0x14/+0x1c) belegen. PC-Projektion: q=((H*0x20000/vz)+1)/2, sx=OFX+(q*ir1)>>16, ir1/ir2 auf s16 saturiert.
- **Risiko:** Orbit-Center-Bytes ungeprÃ¼ft (DAT_800b5250-Struktur). Sichtbare Differenz gering wenn yaw_step/dist_step=0. PC-RTPS-Angleichung optional (PSX schon korrekt via gte_rtpt).

### #32 · LOW · REPAIR · rdt-parse
**RVD-Zonen-Cap 32 entfernen + FLR width/depth sign-extend + FLR-Terminator nach Advance**

- **Ort:** `re15_port/engine/src/rdt_common.c:117 (Cap), :359-362 (uint16_t ew/ed), :355-365 (Terminator), re15_rdt.h:49`
- **RE-Beleg:** FUN_80014230 @0x8001426c-d4: kein Loop-Counter/0x20-Vergleich. FUN_800437d4: lh sign-extend @0x800437fc/@0x80043814; Terminator-Test NACH addiu @0x80043834-40 (entry[0] immer getestet).
- **Fix:** RE15_RDT_MAX_ZONES dynamisch bis Sentinel (oder >=64). ew/ed als int16_t sign-extend. FLR-Terminator-Check ans Schleifenende.
- **Risiko:** Alles latent (geprÃ¼fte STAGE1-RÃ¤ume <32 Zonen, width/depth positiv). Niedriges Risiko.

### #33 · MEDIUM · REPAIR · emd-md1
**EDD-Clip-Offset aus high16 lesen + bones_table_offset &0xfffc + keyframe_speed-Citation korrigieren**

- **Ort:** `re15_port/engine/src/emd_common.c:69-96 (kontiguierlich), :144 (kein Mask), :344-362 (speed)`
- **RE-Beleg:** FUN_8001f314.c:8 (Eintrag=*(EDD+clip*4)), :15 (ptr=EDD+(high16)+frame*4). FUN_8001e5b0.c:76 (&0xfffc). FUN_8001f3bc/f8b4 lesen bytes 6..11 NICHT (keyframe_speed-Citation FUN_8001F3BC falsch).
- **Fix:** Pro Clip clip_off=read_u16_le(edd+i*4+2); first_frame aus high16 statt next_offset-Summe. bones_table_offset &= ~3. keyframe_speed-Citation korrigieren bzw. echte Konsumstelle der bytes 6..11 per Xref/Savestate ermitteln.
- **Risiko:** Alle geprÃ¼ften EDDs kontiguierlich/aligned -> latent. keyframe_speed-Funktion hat 0 Aufrufer (tot). Niedriges Laufzeit-Risiko, aber nicht byte-true.

### #34 · MEDIUM · REPAIR · scd-vm-core
**Opcodes 0x5F..0x8E (RE2-Importe) auf RE1.5-Default (pc+=1, return 1) abbilden**

- **Ort:** `re15_port/engine/src/scd_vm.c:153-167 (GrÃ¶ÃŸen), :294-301 (Handler 0x60/0x61/0x62/0x63/0x80)`
- **RE-Beleg:** Dispatch-Tabelle PTR_LAB_800744a8 = 95 EintrÃ¤ge (0x00..0x5E), letzter @0x80074620. Dispatcher FUN_8003f0a0 OHNE Bounds-Check -> Opcode>=0x5F = OOB. Default LAB_8003f1d8 (pc+=1, return 1).
- **Fix:** Opcodes >=0x5F auf Default (pc+=1, return 1) abbilden statt RE2-Handler (Kage_set/Se_vol etc.). Mindestens dokumentieren dass 0x5F..0x8E NICHT RE1.5 sind.
- **Risiko:** Praktisch harmlos sofern echte RE1.5-RDTs diese Bytes nie als Opcode fÃ¼hren. Aber nicht byte-true. Erst nach GrÃ¶ÃŸen-Fix (Rank 4).

### #35 · MEDIUM · REPAIR · bg-vlc-mdec
**MDEC AC-Dequant +4-Rundung + Koeffizienten-Saturierung minmax(-1024,1023)**

- **Ort:** `re15_port/engine/src/bss_mdec.c:183,192`
- **RE-Beleg:** psx-spx macroblockdecodermdec.md:154 ((...*q_scale+4)/8), :148 (minmax -400h..+3FFh vor IDCT). Port: reine >>3-Trunkierung, kein Clamp. Nur PC-Software-MDEC (PSX nutzt HW-Chip).
- **Fix:** block_buf[ptr+idx]=(iq*qscale*level+4)>>3 (DC ohne +4); danach v auf [-1024,1023] clampen.
- **Risiko:** psx-spx markiert +4 selbst als unsicher ('(?)'). VOR Ã„nderung DuckStation-MDEC-DMA1-Dump eines RE1.5-Frames gegen beide Varianten diffen. Nur PC-PSX-Pixeldifferenz.

### #36 · MEDIUM · REPAIR · msg-text-fsm
**Dialog-Dismiss Cross|Square (0xc000), PSX-Glyph-Cap >0x5F, Typewriter-FF 2 Glyphs, 0x04-Lookahead, Page-Resume-Timer**

- **Ort:** `re15_port/engine/src/msg_common.c:400-420, platform/psx/src/render.c:348, msg_common.c:369-408`
- **RE-Beleg:** PAGE_WAIT/END_WAIT andi 0xc000 @0x80028458/@0x80028698 (Cross|Square). Glyph default-case @0x800289d4 (kein Cap), Control sltiu 0xb @0x80028934. FF iVar8=2 @FUN_80028134.c:31. 0x04-Lookahead @FUN_80028134.c:72-85. Page-Resume DAT_800b8524 @0x80028538.
- **Fix:** Dismiss-Edge (pad_pressed&0xc000). PSX-Atlas auf 256 Glyphs / 0x5F-Cap entfernen, 0x61/0x19 in re15_msg_glyph. FF bis 2 Reveals/Frame. 0x04-Arg==0 Forward-Scan. Page-Resume message_timer=message_scroll. v-Offset +0x20.
- **Risiko:** Mehrere kleine unabhÃ¤ngige Fixes, niedriges Risiko. Glyph-Cap betrifft PSX vs PC-Inkonsistenz (AnfÃ¼hrungszeichen).

### #37 · MEDIUM · REPAIR · audio-vab
**VAB-Parser pBAV-Signatur-Scan (Capcom-Prefix) + Footstep-EDT bank!=0 + Tone-Pitch**

- **Ort:** `re15_port/engine/src/vab_common.c:59-60 (form@+0), :169-177 (bank!=0->-1)`
- **RE-Beleg:** FUN_80045630 @0x800457d0: prog=e[1]&0x7f, tone-tab=prog*0x200+0x820, DAT_800b25a0. ARMS00.EDH 'pBAV'@0x10, CORE00.EDH @0x40 (Capcom-Prefix). EDT prog=1 in ROOM11A0/2040 etc.
- **Fix:** Signatur-Scan vor Parse (erstes 'pBAV' suchen). Mehrere SE-BÃ¤nke (snd0/snd1) laden, bank-Index zur VAB-Wahl. Tone-Pitch via re15_vab_note2pitch (center_note/shift) anwenden.
- **Risiko:** ROOM1170 nur prog=0/snd0 -> dort Ã¤quivalent. Se_on-Pfad nutzt aktuell Platzhalter test.vh/vb. Mittlerer Aufwand (Mehr-Bank-Laden).

### #38 · LOW · REPAIR · collision-stair
**Collision Status-only-Branch (Flag 0x20) + Quadranten-Partition in Band-Helfern**

- **Ort:** `re15_port/engine/src/re15_collision.c:296 (continue), :187-275 (Full-Scan-Helfer)`
- **RE-Beleg:** Status-Gate @0x8003b244-260, Flag-Set 0x20 @0x8003b2cc-dc (DAT_800ac784). Quadranten FUN_8003b7f0.c:26-40 (base+4+q*4). Helfer aktuell TOTER CODE (einzige Ref Kommentar stair_common.c:307).
- **Fix:** Status-Branch (u0-bit0==0 -> Player-Flag 0x20). Band-Helfer auf Quadranten einschrÃ¤nken ODER da tot: entfernen/dokumentieren.
- **Risiko:** Status-Flag-Konsument im Port nicht aktiv; Band-Helfer ungenutzt. Beide latent.

### #39 · LOW · REPAIR · rdt-parse
**parse_main_scd: sub>main-Guard entfernen (positionsunabhÃ¤ngig)**

- **Ort:** `re15_port/engine/src/rdt_common.c:175`
- **RE-Beleg:** FUN_8003ef6c:8/10 setzt main/sub unabhÃ¤ngig (RDT+0x40/+0x44). Keine sub>main-Bedingung.
- **Fix:** main_scd unabhÃ¤ngig berechnen (data+main_scd_start+first_off); GrÃ¶ÃŸe via kleinste Header-Boundary > main, nicht zwingend subScdStart.
- **Risiko:** GeprÃ¼fte STAGE1-RÃ¤ume sub>main -> latent. Niedrig.

### #40 · HIGH · REPAIR · room-intro-flow
**Boot-Scaffolding raumabhÃ¤ngig kapseln (1170-only Cheats raus aus 1240-Default) + Cross-Room-Szenario aus TÃ¼r**

- **Ort:** `re15_port/platform/pc/main.c:296,596-722,603,610,693,714-722,753-764; room_common.c:98; scd_room_setup.c:147-154`
- **RE-Beleg:** Default 1240 (main.c:296), aber Scaffolding auf 1170 verdrahtet. ROOM1240 main00 nur Door_aot_set+Evt_end, 0x Ck-Opcodes. Cross-Room-Szenario: FUN_8001d600:42-44 DAT_800b0fe4(=work_var[10])=*(door+5). 1170 sub00 case 11 Evt_exec(0x180E).
- **Fix:** Forcierte Flags (3,193)/(4,195)/sub11-Detektion/door-3-Reentry hinter if(boot_room==0x1170). Default-Kommentare+RE15_BOOT_ROOM auf 0x1240. 1240-Pfad: scd_thread_start(0,main00)+(1,sub00). Cross-Room: Eintrittsszenario aus TÃ¼r-Feld (door+5) an scd_room_reenter durchreichen statt 0.
- **Risiko:** Cheats funktional inert fÃ¼r 1240 (kein Crash) aber nicht byte-true und verschleiernd. Szenario-Durchreichung braucht re15_room_change_t-Erweiterung. Mehrere verzahnte Stellen.

### #41 · LOW · PORT · player-damage
**Poison-Drain FUN @0x80031e98 (HP-=2, clamp 1) portieren wenn Status-Trigger existiert**

- **Ort:** `re15_port/engine/src/player_common.c:72-74 (nur Kommentar)`
- **RE-Beleg:** @0x80031e9c HP-=2 @0x80031ea4, clamp=1 @0x80031ec8; gated DAT_800acaec&0x4, Timer DAT_800acc27. Status gesetzt im Damage-Handler @0x80012eb4 (|=0x2).
- **Fix:** Optional: gated HP-=2 mit min(HP,1)-Clamp, sobald Status-Effekt-Trigger (Gift) im Port existiert.
- **Risiko:** Gameplay-only, bewusst weggelassen. Erst nach Damage-System (Rank 13).

### #42 · LOW · REPAIR · render-pc-parity
**CLEAR_RGB-Wert korrigieren (0x081030FF, opake Alpha)**

- **Ort:** `re15_port/platform/pc/src/render_pc.c:287`
- **RE-Beleg:** PSX render.c:157-158 setRGB0(8,16,48). Port 0x00081830 dekodiert zu R=0,G=8,B=24,A=0x30. Sichtbar nur Boot-/Fehlframes (BG-memcpy Ã¼berschreibt sonst).
- **Fix:** CLEAR_RGB = 0x081030FFu (R=8,G=16,B=48,A=0xFF).
- **Risiko:** Trivial, niedrig. Sichtbar nur vor erstem BG-Blit.

### #43 · LOW · REPAIR · emd-md1
**bone_mesh_index[] als toten/falschen Metadaten dokumentieren oder entfernen**

- **Ort:** `re15_port/engine/src/emd_common.c:185-200`
- **RE-Beleg:** Renderer (pc/main.c:1717, psx/mesh_psx.c:838) nutzen mesh_idx==bone_idx (byte-true 1:1). bone_mesh_index befÃ¼llt fÃ¤lschlich Child-Liste, ist tot.
- **Fix:** Feld entfernen oder als 'flat child list (NOT a mesh map)' dokumentieren.
- **Risiko:** Kein Render-Bug. Reine AufrÃ¤umung.

### #44 · LOW · REPAIR · doc-consistency
**Stale-Doku-Korrekturen (light_apply_cut, bss version, msg 0x05 3->2 Byte, sub_scd[11] Kommentar, bg-vlc YUV-Konstanten)**

- **Ort:** `re15_light.h:94-96, re15_bss.h:12, re15_msg.h:8, scd_vm.c:699-703, bss_mdec.c:50-52`
- **RE-Beleg:** light: Impl tint=128 (FUN_80039b2c.c:27 0x808080). bss: v2 in 43% (bss_vlc.c:344). msg 0x05: 2-Byte (FUN_80028868.c:67-70). sub_scd[11]=sub11. YUV MUL_R/MUL_G_CB gerundet vs Java/revengi trunkiert (5742/-1407).
- **Fix:** Header-/Code-Kommentare an Implementierung/RE angleichen; falls Byte-Treue zur Java-Referenz Ziel: MUL_R=5742, MUL_G_CB=-1407.
- **Risiko:** Reine Doku/Konstanten, kein Laufzeit-Bug auÃŸer YUV (1 Stufe bei groÃŸem |Cr|/|Cb|).

## Offene Verifikations-Lücken (zweite Runde nötig)

- SCD-Handler-SEMANTIK (nicht nur GrÃ¶ÃŸe): FÃ¼r 0x4C (Slot-Tabelle 0x800b2360), 0x4D, 0x4E (Tabelle 0x800b2368), 0x4F (FUN_80017048 Modellblock), 0x38 (4 HalbwÃ¶rter Floor/SCA) wurde nur der PC-Vorschub byte-genau belegt, NICHT die vollstÃ¤ndige Feld-/Effekt-Semantik der Handler. Zweite Runde: Handler-Bodies transliterieren und Zielstrukturen auflÃ¶sen.
- Cmp pc[1]->pc[2]-Fix (Rank 6): Der frÃ¼here PSX-Boot-Crash beim Umstellen ist NICHT reproduziert/diagnostiziert. pc[1] (Array-/targetType-Selektor) ist unmodelliert â€” ob es eine Tabellenwahl kodiert, ist offen. Laufzeit-Verifikation auf echtem PSX-Boot nÃ¶tig.
- Thread-Pool-Umbau 24->14 (Rank 24) und Slot-1-Repointing: GroÃŸer Scheduler-Eingriff, nur statisch belegt. Active-Message-Gate DAT_800aca40&0x200 und die Zwei-Kontext-Dispatch-Reihenfolge sind nicht gegen Savestate-Laufzeit verifiziert.
- AOT-Scan-Refactor (Rank 14/15): Edge->per-Frame-Fire + Multi-Entity + 9-Frame-TÃ¼r-Halte ist die riskanteste VerhaltensÃ¤nderung. Welche AOTs im Original auf Dauerfeuern angewiesen sind (vs. Self-Disable via Aot_reset), wurde NICHT katalogisiert. Item-/NPC-Maske-Vorkommen (mask 2/4) nicht gezÃ¤hlt. Reach-Funktion FUN_8002d1e8 nicht portiert/verifiziert.
- Work_set kind=5 (Rank 26): DAT_800b23f4-Array-Semantik (welche logische EntitÃ¤t die Slots indizieren) ist in der EXE NICHT aufgelÃ¶st â€” offener RE-Punkt (FUN_800161e0 BefÃ¼llung).
- keyframe_speed bytes 6..11 (Rank 33): Die ECHTE Konsumstelle ist unbestimmt (in FUN_8001f3bc/f8b4 Ã¼bersprungen). Per Xref/Savestate-RAM-Dump prÃ¼fen, ob Ã¼berhaupt eine Speed-Integration in actor.pos existiert.
- Camera-Orbit-Center (Rank 31): Offsets cut[+0x14]/[+0x1c] (DAT_800b5250-Struktur) vs View-Builder-Target +0x10/+0x14/+0x18 â€” welche Bytes RE1.5 dem Orbit-Center tatsÃ¤chlich fÃ¼ttert, ist ungeprÃ¼ft. rcos/rsin vs re15_cos_q12 Skalierungs-/Tabellengleichheit nicht verifiziert.
- YES/NO 0xf9-Inline-Dispatch (Rank 27, confidence medium): Der Branch-Mechanismus (Dispatch-Tabelle 0x80072f34, Stride-Bytes) ist nur skizziert, nicht byte-genau gegen eine reale .msg-Choice durchverfolgt. Flag-(12,31)-Hack-Ersatz unverifiziert.
- MDEC +4/Saturierung (Rank 35, bg-vlc): psx-spx selbst unsicher ('(?)'). KEIN DuckStation-MDEC-DMA1-Dump eines echten RE1.5-Frames gegen die Varianten gediff't â€” vor Ã„nderung zwingend nÃ¶tig.
- STAGE2-6 generell: Fast alle Asset-Belege stammen aus STAGE1 (ROOM10xx/11xx/12xx) plus vereinzelt STAGE3/4. Opcode-Vorkommen (0x58/0x59, 0x4C-0x4F, Door 4P-Variante, Slope-Typen) wurden NICHT systematisch Ã¼ber alle 6 Stages mit einem SCD-Walker gezÃ¤hlt â€” viele 'kommt real vor?'-Fragen sind nur stichprobenartig beantwortet.
- PSX-Build-Pfad: Die meisten Render-/Mathematik-Fixes (RotMatrix, Trig-LUT, Kamera-Integer) wurden gegen die EXE-Disassembly belegt, aber der PSX-Port (PSn00bSDK gte_rtpt/RotMatrix) wurde nur als 'teils korrekt' eingestuft â€” eigene RotMatrix/LUT auf PSX ist ungetestet und kÃ¶nnte die PSX-ParitÃ¤t brechen.
- Enemy-Subsysteme (AI, sce_em_set Spawn, Zombie-Logik in STAGE-Overlays @0x8010xxxx): Der gesamte Audit fokussiert Engine-Infrastruktur (EXE-FUN_80xxxxxx). Die Overlay-Decompilate (RE_15_Quellcode_Overlays/STAGE{1..6}) als PRIMÃ„RE Quelle fÃ¼r Raum-/Gegner-Logik wurden NICHT auditiert â€” Gegnerverhalten/Dispatch @0x80072bac ist eine komplett ungeprÃ¼fte Subsystem-Klasse.