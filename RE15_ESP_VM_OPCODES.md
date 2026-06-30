# RE1.5 Effect-Script VM — Opcode Map (@0x800744a8, 95 ops 0x00-0x5e)

Dispatch: `FUN_8003f0a0` runs `(*table[*pc])(instance)` per active 0x170-instance. Port: `re15_esp_vm.c`.
Status: **✅PORTED** = byte-true in re15_esp_vm.c (re15_espvm_install_ops) + tested (test_esp_vm). **39/95 ported.**
Classification of 0x0d-0x5e by the 77-agent workflow (2026-06-30); every PORTED row self-disassembled before porting.
`portable_now` = pure instance-local + bytecode operands (no external pool/work-struct/GTE/audio).
⚠️ The rows marked `·portable` were AGENT-flagged portable but self-verified to deref the work-struct @inst+0x154
or the effect pool DAT_800b2368 -> actually **defer** (see deferred groups).

| op | addr | status | category | calls | summary |
|----|------|--------|----------|-------|---------|
| 0x00 | 0x8003f1d8 | ✅PORTED | control-flow |  | nop-continue (pc++, CONT) [alias 0x1c-0x20] |
| 0x01 | 0x8003f1f0 | ✅PORTED | control-flow |  | end/return-level (lvl0 deactivate+STOP, else pop level) |
| 0x02 | 0x8003f258 | ✅PORTED | control-flow |  | wait-1-frame (pc++, STOP) |
| 0x03 | 0x8003f270 | ✅PORTED | control-flow |  | jump-script (re-seed FUN_8003edec, id@pc+3) |
| 0x04 | 0x8003f2a0 | ✅PORTED | spawn |  | spawn sibling (FUN_8003ee3c slot@pc+1 id@pc+3) |
| 0x05 | 0x8003f2dc | ✅PORTED | control-flow |  | kill instance[idx@pc+1] |
| 0x06 | 0x8003f328 | ✅PORTED | loop |  | loop-push (push pc+4+u16@pc+2, depth++) |
| 0x07 | 0x8003f368 | ✅PORTED | loop |  | loop-jump (pc+=u16@pc+2, depth--) |
| 0x08 | 0x8003f3a4 | ✅PORTED | loop |  | loop-pop (pc+=2, depth--) |
| 0x09 | 0x8003f3e0 | ✅PORTED | loop |  | counter-set (CARRAY[++ci]=u16@pc+2) |
| 0x0a | 0x8003f428 | ✅PORTED | loop |  | counter-wait [0x09+0x0a=wait N frames] |
| 0x0b | 0x8003f490 | ✅PORTED | loop |  | counter-push (ci++) |
| 0x0c | 0x8003f4c4 | ✅PORTED | loop |  | wait-condition (re15_espvm_set_wait_gate) |
| 0x0d | 0x8003f540 | ✅PORTED | loop-counter |  | FOR/loop-push opcode: increments the per-level loop counter-index (+0x08), pushes a new loop frame — u16 iteration count (operand@pc+4) into |
| 0x0e | 0x8003f674 | ✅PORTED | loop-counter |  | Opcode 0x0e = NEXT/loop-back: decrements per-level u16 loop counter at +0xa0; if nonzero, restores pc from the loop-return stack (mem[+0x20  |
| 0x0f | 0x8003f6f4 | ✅PORTED | control-flow | *(0x800744a8 + 4*subopcode) via jalr v0 @0x8003f7ac (predicate head),*(0x800744a8 + 4*subopcode) via jalr v0 @0x8003f7f0 (predicate term loo | Conditional/loop-begin opcode: pushes a loop frame (+0x20 start, +0x60 end, +0x0c depth, +0x08 counter-index), evaluates a chained AND/OR pr |
| 0x10 | 0x8003f878 | ✅PORTED | loop-counter |  | Loop-back opcode: restores pc from the per-level saved-PC stack slot (+0x20 + level*0x10 + idx*4) and decrements the per-level counter-index |
| 0x11 | 0x8003f8bc | ✅PORTED | loop-counter |  | FOR/loop-entry opcode: increments per-level counter-index, pushes loop-body-start (pc+4) and loop-end target (pc+4 + s16@[pc+2]) into per-le |
| 0x12 | 0x8003f930 | ✅PORTED | control-flow |  | Opcode 0x12 = a compound-conditional / loop-test: it re-dispatches a block of N sub-condition opcodes through the VM table @0x800744a8, AND/ |
| 0x13 | 0x8003fa5c | ✅PORTED | control-flow |  | Opcode 0x13: a loop/jump-setup opcode — pushes a per-level pointer (pc+u16@pc+2) and a level byte into the instance's +0x60/+0x0c per-level  |
| 0x14 | 0x8003fb38 | ✅PORTED | control-flow |  | Opcode 0x14: fixed 6-byte NOP/skip — advances the bytecode pc (+0x1c) by 6 and returns CONT(1); reads no operands and writes nothing else. |
| 0x15 | 0x8003fb50 | ✅PORTED | control-flow |  | Opcode 0x15 is a 2-byte no-op/skip: it advances the bytecode pc by 2 and returns CONT(1); touches only the instance pc field (+0x1c). |
| 0x16 | 0x8003fb68 | ✅PORTED | loop-counter |  | Decrements the per-level loop counter-index (instance[+0x08+level] -= 1), advances pc by 2, and returns CONT(1). |
| 0x17 | 0x8003fb9c | ✅PORTED | control-flow |  | Loop/block-setup opcode: stores loop-depth[level]=op@[pc+1] and counter-index[level]=op@[pc+2], sets stack-pointer (+0x140) into the level's |
| 0x18 | 0x8003fbe8 | ✅PORTED | control-flow |  | GOSUB/push-level opcode: saves return pc (pc+2) into per-level array at +0x144, increments level (+0x02), reinitializes the new level's loop |
| 0x19 | 0x8003fc50 | ✅PORTED | loop-counter |  | Loop/scope POP (end-of-loop-iteration return): decrements the per-instance level at +0x02, restores pc (+0x1c) from the saved-return-pc tabl |
| 0x1a | 0x8003fca8 | ✅PORTED | control-flow |  | Loop-pop / return: restores pc (+0x1c) from the per-level saved-pc array (+0x60 + level*0x10 + counter_index*4), decrements counter-index[le |
| 0x1b | 0x8003f5d0 | ✅PORTED | loop-counter |  | FOR-loop / loop-counter setup: bumps the per-level counter-index, loads an iteration count from the read-only table at 0x800b0fd0, and pushe |
| 0x21 | 0x8003fcf4 | —defer | control-flow |  | Conditional flag-test opcode: selects a global flag bank via pointer-table[0x80074664][byte[pc+1]], tests bit (hw[pc+2]&0x1f) in word ((hw[p |
| 0x22 | 0x8003fdd0 | —defer | field-setter |  | Opcode 0x22: set/clear/toggle a single bit in an external global flag array selected by a pointer table @0x80074664; advances pc by 4 and re |
| 0x23 | 0x8003ff68 | ✅PORTED | control-flow |  | Opcode 0x23 = compare/test: LHS = global register DAT_800b0fd0[pc[2]&0xff] (s16), RHS = s16 imm at pc[4], op selector = pc[2]>>8 (0..6: ==,  |
| 0x24 | 0x80040018 | ✅PORTED | field-setter |  | SET-IMMEDIATE: writes s16 immediate operand into VM variable/register table 0x800b0fd0[id], advances pc+=4, returns CONT(1). |
| 0x25 | 0x8004004c | ✅PORTED | field-setter |  | Copy VM-variable: DAT_800b0fd0[u8 dest] = DAT_800b0fd0[u8 src] over the global u16 register array; advances pc+=3, returns CONT(1). |
| 0x26 | 0x8004008c | ✅PORTED | field-setter | 0x80040140 | VM "apply arithmetic/bitwise op to global variable" opcode: pc+=6, reads var-index+op-selector (u16@pc+2) and value (s16@pc+4), calls helper |
| 0x27 | 0x800400dc | ✅PORTED | field-setter | 0x80040140 | Opcode 0x27: arithmetic/logic op on the global VM register table @0x800b0fd0 — reads 3 operand bytes (op-id, dst-index, src-index), dispatch |
| 0x28 | 0x80040270 | —defer | rng-misc | 0x8003ea3c | Opcode 0x28: advance pc by 1 (no operands), invoke RNG/misc subroutine 0x8003ea3c (updates globals 0x800b3f6c/0x800b0fea), return CONT(1). |
| 0x29 | 0x800402a0 | —defer | control-flow | 0x800142f4 | Opcode 0x29 = cut/montage display-switch: saves the current display-id globals (0x800b3f7b/0x800b0fe8), sets the cut-freeze bit 0x800aca3c\| |
| 0x2a | 0x8004032c | —defer | control-flow | 0x800142f4 | Opcode 0x2a: a 1-byte (no-operand) control op that snapshots/sets a global display-state pair (saves [0x800b0fe4]->[0x800b0fe8], stores byte |
| 0x2b | 0x800404f4 | —defer | external-global | 0x80027e68 | Opcode 0x2b: advances pc+=4 (1-byte + 2-byte operands), calls FUN_80027e68(a0=0, a1=0x300, a2=u8@pc+1, a3=(u16@pc+2)<<16) — an external engi |
| 0x2c | 0x80040534 | —defer | control-flow |  | Opcode 0x2c registers/latches old_pc+2 into the external u32 table 0x800ac9b0[ [pc+1] ] (incrementing the global byte counter 0x800afbb4 on  |
| 0x2d | 0x80040914 | —defer | draw-sprite | 0x8002b898 | Opcode 0x2d spawns/configures an effect-sprite pool entry (base 0x800b3f98, stride 0x94) from a 22- or 34-byte inline operand block, selects |
| 0x2e | 0x80040d2c | ✅PORTED | field-setter |  | Opcode 0x2e: "set target object" — reads kind+index operands (pc+=3), clears instance +0x158..+0x16f, then stores a pointer into instance +0 |
| 0x2f | 0x80040f14 | ✅PORTED | field-setter |  | Opcode 0x2f: u16-store — writes a u16 bytecode operand into a per-id instance field slot at +0x158 + id*2, advances pc by 4, returns CONT(1) |
| 0x30 | 0x80040f40 | —defer | field-setter |  | Per-frame transform integrator: adds preloaded position deltas (+0x158/15a/15c) and rotation deltas (+0x15e/160/162) into a target struct po |
| 0x31 | 0x80040fd4 | ✅PORTED | field-setter |  | Opcode 0x31: per-tick integrator — adds six u16 velocity/delta fields (+0x164..+0x16e) into six u16 accumulator fields (+0x158..+0x162) on t |
| 0x32 | 0x80041048 | ✅PORTED | field-setter |  | Opcode 0x32: reads three signed-16 operands from the bytecode and stores them as three 32-bit words at offsets +0x34/+0x38/+0x3c of the inst |
| 0x33 | 0x80041080 | ✅PORTED | field-setter |  | Opcode 0x33: copies three u16 bytecode operands (pc+2/+4/+6) into the render/output struct pointed to by instance+0x154 at +0x68/+0x6a/+0x6c |
| 0x34 | 0x800410b8 | —defer | field-setter | 0x8004116c | Opcode 0x34: writes the s16 operand at [pc+2] into a field (selected by byte [pc+1], table 0..0x13) of the effect object pointed to by insta |
| 0x35 | 0x80041108 | —defer | field-setter | 0x8004116c | Opcode 0x35: SET actor member — selector byte[pc+1] picks an actor field, byte[pc+2] indexes external global table DAT_800b0fd0 (lh) for the |
| 0x36 | 0x80041624 | —defer | draw-sprite | 0x80045024 | Opcode 0x36: spawn/draw a positioned effect-sprite — picks a world-origin base (0/player@0x800b..ca94 / enemy_array@0x800b..cea c / object-p |
| 0x37 | 0x8004175c | —defer | field-setter |  | Opcode 0x37: byte-setter — resolves a 12-byte struct in the external pool behind DAT_800ac778 (mem[+0x20] table indexed by op1, element op2) |
| 0x38 | 0x800417ac | —defer | field-setter |  | Opcode 0x38: writes a 4×16-bit record (two u16 + two s16 from operands) into a stride-0xC slot of an external effect/object pool (indexed vi |
| 0x39 | 0x80041814 | —defer | external-global |  | Opcode 0x39: reads 3 bytecode operand bytes [pc+1..3], dereferences the global ptr @0x800ac778, indexes table[(op1+1)] at [ptr+0x20], and st |
| 0x3a | 0x80041864 | —defer | draw-sprite | 0x80019700 | Opcode 0x3a reads a 16-byte bytecode record, selects an external base/struct address via a 6-entry inner dispatch (a1&0xff indexing globals  |
| 0x3b | 0x800405bc | —defer | external-global |  | Opcode 0x3b: looks up a global "trigger/event" table slot (@0x800ac9b0 + operand[pc+1]*4); if the slot is 0, bumps a global byte counter @0x |
| 0x3c | 0x800403ac | —defer | external-global |  | Opcode 0x3c: cut/freeze-flag toggle — reads 1 operand byte at [pc+1]; operand==1 clears bit 0x100 of external global DAT_800aca3c, else sets |
| 0x3d | 0x80041238 | —defer | external-global | 0x80041358 | "Get game-object field (selector=op2, source=instance+0x154 referent, via FUN_80041358's 0x14-entry field-dispatch) and store the s16 result |
| 0x3e | 0x80041290 | —defer | control-flow | 0x80041358 | Conditional/control-flow opcode: reads an external object pointer at instance+0x154, queries one of 20 of its fields (selector lo-byte of u1 |
| 0x3f | 0x80041b90 | ·portable | field-setter |  | Opcode 0x3f: set-animation/state on the instance's +0x154 work struct — writes anim-id (0x05=operand byte), mode=4 (0x04), low operand byte  |
| 0x40 | 0x80041be4 | —defer | draw-sprite |  | Opcode 0x40: configures a sprite/effect render record (pointed at by instance+0x140) — sets a flags-gated animation state, position params ( |
| 0x41 | 0x80041e98 | —defer | draw-sprite |  | Opcode 0x41: configures the instance's associated draw/sprite primitive (struct at instance+0x154) — sets a GPU blend/primitive code byte at |
| 0x42 | 0x80041f88 | ✅PORTED | field-setter |  | Opcode 0x42: dereferences instance pointer field +0x154 and sets sub-struct bytes [+0x4]=1, [+0x5..+0x7]=0 (init/reset a flag block), advanc |
| 0x43 | 0x80041fb8 | —defer | field-setter |  | Opcode 0x43: SET/OR/XOR (sub-mode at pc+1) a u16 operand (pc+2) into the +0x1c4 field of a struct pointed to by instance+0x154; advances pc+ |
| 0x44 | 0x800420a0 | —defer | spawn-alloc | 0x8004efe4,0x80039b2c | Opcode 0x44 = SPAWN-ENTITY: allocates a fresh ~0x170-byte entity from the effect/entity pool (DAT_800ac77c), initializes ~40 fields from an  |
| 0x45 | 0x800428d4 | —defer | external-global | 0x800396a8 | Opcode 0x45: reads 2 operand bytes [pc+1],[pc+2], calls helper 0x800396a8 with a0=(op1+1), a1=op2 to set a byte field on all matching entrie |
| 0x46 | 0x80040738 | —defer | field-setter |  | Opcode 0x46: 10-byte field-setter that pokes an object fetched from the EXTERNAL global table @0x800ac9b0 (indexed by operand [pc+1]) — sets |
| 0x47 | 0x800407bc | —defer | draw-sprite | indirect jalr v0 @0x8004082c -> *(0x8007469c + struct[+0]*4); target is one of the EXE dispatch entries (e.g. 0x8004305c,0x80043084,0x800430 | Opcode 0x47: look up effect descriptor table[0x800ac9b0 + operand_id*4], select sub-struct (struct+20 if (struct[+1]&0x80) else struct+12),  |
| 0x48 | 0x80040b20 | —defer | field-setter |  | Effect-VM "model/object set" handler (sibling of SCD opcode 0x2D @0x80040914): a fixed 16-byte opcode that populates a render-object slot po |
| 0x49 | 0x80040cd0 | —defer | field-setter |  | Opcode 0x49: copies three sub-struct fields (+0x5c/0x60/0x64 -> +0x34/0x38/0x3c), plants a hardcoded global pointer 0x80072d4c into sub-stru |
| 0x4a | 0x80041e54 | ·portable | field-setter |  | Opcode 0x4a: sets the per-instance sub-struct (pointer @instance+0x154) into mode 4 with sub-mode = operand byte[pc+1], clears its 6/7/0x1c8 |
| 0x4b | 0x80040414 | —defer | external-global | 0x800142f4 | Opcode 0x4b: room camera-pair remap — swaps two cut/camera ids (operands pc+1,pc+2) throughout the room cam-def list (DAT_800ac778+0x28, 0x1 |
| 0x4c | 0x80040858 | ·portable | field-setter |  | Opcode 0x4c registers an effect-script subroutine/slot: saves the bytecode pointer pc+2 into pool table DAT_800b2368[id*4] (id = u8 @pc+1),  |
| 0x4d | 0x800408a8 | ·portable | field-setter |  | Opcode 0x4d: copy a 10-byte operand block from bytecode into fields (+0x0/+0x1 bytes, +0xa/+0xc/+0xe halfwords) of the effect-pool record se |
| 0x4e | 0x80041980 | —defer | external-global | 0x80019ca8 | Opcode 0x4e: reads a 5-byte instruction (2 id bytes + selector + index), picks one of four external base tables (0x80072d4c/0x800aca74/0x800 |
| 0x4f | 0x80016f20 | —defer | draw-sprite | 0x80017048 | Opcode 0x4f: configures an effect/sprite pool object (slot = pc[1], table 0x800b23f4) from a 22-byte operand block, sets a draw-link flag fi |
| 0x50 | 0x80040644 | —defer | control-flow | 0x8004efe4 | Opcode 0x50: registers/advances a per-id bytecode pointer in table @0x800ac9b0 (id=[pc+1], slot:=pc+2, bumps counter @0x800afbb4 on first se |
| 0x51 | 0x80042920 | —defer | control-flow |  | Conditional flag/branch opcode: advances pc+=4, tests u16-mask-operand@pc+2 against external global 0x800ac768; returns the u8 operand@pc+1  |
| 0x52 | 0x8004295c | —defer | control-flow |  | Conditional return-code opcode: advances pc by 4, reads a u8 return code and u16 mask from bytecode, ANDs the mask with external global flag |
| 0x53 | 0x80040e18 | —defer | field-setter |  | Opcode 0x53: clears instance words +0x158..+0x16f, reads u8 selector[pc+1] and u8 id[pc+2], looks up id via s16 table @0x800b0fd0, then per- |
| 0x54 | 0x80042998 | —defer | external-global | 0x80044da4 | Opcode 0x54: reads 5 operand bytes [pc+1..pc+5], bit-packs them into one 32-bit arg, calls external FUN 0x80044da4 (which re-unpacks the nib |
| 0x55 | 0x80041a88 | —defer | draw-sprite | 0x80019d50 | 6-byte opcode: switch on [pc+4] picks an external target struct (0x80072d4c / 0x800aca74 / pool 0x800acc4c stride0x174 / pool 0x800b3fb8 str |
| 0x56 | 0x80042a58 | —defer | external-global | 0x800217b0 | Opcode 0x56: reads a 16-bit id (bytes pc+1,pc+2), a byte (pc+3) and a signed half (pc+4) and forwards them with constant arg3=7 to FUN_80021 |
| 0x57 | 0x80042ab4 | —defer | audio-se | 0x800216ec | Opcode 0x57: reads u8 id + u16 value operands and calls external helper 0x800216ec to set/queue an entry in the global table @0x800b5458 (ch |
| 0x58 | 0x8003fd54 | —defer | control-flow |  | Opcode 0x58 = conditional flag-bit test: advances pc by 4, reads a game-flag bank (via tables @0x800b0fd0 + @0x80074664) and returns v0 = (b |
| 0x59 | 0x8003fe90 | —defer | external-global |  | Opcode 0x59 = SET/CLEAR/TOGGLE one bit in an external game-flag bitfield: ptr table @0x80074664[operand1] gives flag bank, bit-map table @0x |
| 0x5a | 0x80041474 | —defer | field-setter | 0x80041554,0x80040140 | Opcode 0x5a: resolve a target halfword field (via jump-table helper 0x80041554, off instance field 0x154) and apply an arithmetic/bitwise op |
| 0x5b | 0x800414e0 | —defer | field-setter | 0x80041554,0x80040140 | Opcode 0x5b: apply an arithmetic op (selector [pc+1], range<0xc) using an s16 constant from global table DAT_800b0fd0[[pc+3]] to a resolved  |
| 0x5c | 0x80042a10 | —defer | external-global | 0x80044fec | Opcode 0x5c reads a u16 operand at pc+0 and passes it to helper 0x80044fec (which indexes external global tables 0x800748fc/0x80074828 and s |
| 0x5d | 0x8004203c | —defer | field-setter |  | Opcode 0x5d: writes the u16 operand [pc+2] to offset 0x1c8 (selector 0) or 0x1ca (selector 1) of the object pointed to by instance field +0x |
| 0x5e | 0x80042b04 | —defer | external-global | 0x80013278 | Opcode 0x5e: sets bit 0x20 in the cut/game-state global 0x800aca3c, then calls external helper FUN_80013278(id=byte@pc+1, count=halfword@pc+ |

## Deferred opcode groups (need external integration / C3), by dependency

1. ~~**VM registers** DAT_800b0fd0~~ ✅ DONE (0x1b/0x23/0x24/0x25/0x26/0x27).
2. **work-struct @inst+0x154** (0x32/0x3f/0x42/0x4a/0x3d/0x3e/0x33/0x34/0x35/0x43/0x5a/0x5b/0x5d) — derefs a pointer to an external anim/render struct; needs that struct allocated/bound first.
3. **effect pool DAT_800b2368** (0x4c spawn-slot, 0x4d slot-config, 0x2d/0x48 model-set, 0x37/0x38/0x4f) — ESP-B pool integration.
4. **draw/GTE** (0x36/0x3a/0x40/0x41/0x47/0x55 + the SPRT builder FUN_80046a1c) — needs the render path (C3).
5. **external game-flags / cuts** (0x21/0x22/0x29/0x2a/0x3b/0x3c/0x4b/0x50/0x51/0x52/0x58/0x59/0x5e) — DAT_800aca3c / @0x80074664 flag banks / display globals.
6. **misc external** audio 0x57 (0x800216ec) · RNG 0x28 (0x8003ea3c) · entity-spawn 0x44 (0x800420a0) · 0x2b/0x39/0x45/0x46/0x49/0x54/0x56/0x5c (engine helpers).

Full per-op disasm: workflow run wv6srppqn (regenerable via the esp-vm-opcode-classify workflow).
