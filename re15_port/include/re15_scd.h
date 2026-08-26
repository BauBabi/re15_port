/*
 * RE1.5 Rebuilt — SCD VM (Phase 4.4, 2026-05-18).
 *
 * Reimplementation of RE1.5/RE2's SCD script interpreter in clean C.
 *
 * Architecture:
 *   - 10 thread slots running in parallel
 *   - Each thread has: PC pointer, call stack, sleep counters, locals
 *   - Per-frame `scd_tick()` iterates threads, dispatches opcodes
 *   - Opcodes update PC + thread state; return:
 *       0 = pop call frame
 *       1 = continue same frame (re-dispatch next opcode immediately)
 *       2 = yield this thread until next frame
 *
 * Tick rate: 30 Hz (every other vsync). Sleep durations in 30Hz ticks.
 *
 * RE2-faithful invariants:
 *   - VM never blocks on libsnd / SPU during save
 *   - No SsVoKeyOff / SsVabClose during room transitions
 *   - Audio teardown is the room manager's concern, not VM's
 *
 * Phase 4.4 supports a subset of opcodes (Nop, Evt_end, Sleep/Sleeping,
 * Goto, simple If/Else, debug-text). Phase 4.4+N adds more.
 */
#ifndef RE15_SCD_H
#define RE15_SCD_H

/* Prop-Pool. Das Original hat hier KEINE feste Grenze — sein Pool @0x800b3f98
 * (Schrittweite 148) wird ueber obj_id indiziert und bis nOmodel durchlaufen
 * (@0x8004093c-58 / @0x8002be5c / @0x80043758). Game-weit gemessenes Maximum:
 * nOmodel = 17 und groesste obj_id = 16, beides nur in ROOM1190/1191.
 * ⛔ GEMESSEN am laufenden Port (probe_1190_props, ROOM1190): 7 Installationen im
 * Boot-Szenario, Reihenfolge == obj_id. Der Pool war also NICHT der Engpass fuer die
 * fehlende Weste — das war die RDT-Kappung (s. RE15_RDT_MAX_PROPS in re15_rdt.h) —
 * aber er zieht mit, damit beide Grenzen dieselbe gemessene Zahl tragen. */
#define RE15_SCD_MAX_PROPS 17

#include <stdint.h>

/* Phase 4.5.11: RE2 has 2 parallel VMs: Player/AOT (slots 0..9, 10 threads)
 * + Event (slots 10..13, 4 threads = Evt_exec dispatch pool). We flatten
 * into one pool of 24 slots: 0 = main thread, 1..9 = player/aot subs,
 * 10..23 = event slots (Evt_exec target).
 *
 * Expanded 14→24 (event 10..13 → 10..23) on 2026-05-25: ROOM1170 fires
 * 8+ concurrent event subs during the helipad cinematic (sub04/05 rotor
 * spinners run forever, sub06/07/08 takeoff ascent runs ~30s, sub11
 * narrator runs ~60s, sub12/13 fly-away). With only 4 event slots,
 * sub07/sub08/sub13 silently dropped → rotor and tail-rotor stayed put
 * after takeoff. */
#define SCD_THREAD_COUNT       24
#define SCD_EVENT_SLOT_FIRST   10
#define SCD_EVENT_SLOT_LAST    23
#define SCD_CALL_DEPTH_MAX     4
#define SCD_SLEEP_SLOT_COUNT   16

/* Per-thread state. Layout intentionally similar to RE1.5's native struct
 * at 0x800B2B4C+slot*0x170 to ease debugging / direct comparison. */
typedef struct {
    uint8_t  active;        /* 0 = slot free, 1 = running */
    uint8_t  kill_pending;  /* 1 = thread will die at next yield */
    int8_t   call_depth;    /* current frame index 0..3 */
    uint8_t  _pad1;

    int8_t   frame_tag[SCD_CALL_DEPTH_MAX];      /* negative = blocked */
    uint8_t  sleep_count[SCD_CALL_DEPTH_MAX];    /* sleep slots per depth */

    const uint8_t *pc;        /* current bytecode PC */

    uint8_t  locals[128];     /* scratch for opcode args / variables */

    /* Sleep slot bank: per-depth × 16 slots × u16 counter */
    uint16_t sleep_slots[SCD_CALL_DEPTH_MAX][SCD_SLEEP_SLOT_COUNT];

    const uint8_t *call_stack[12];   /* Gosub return PCs */
    /* [#8] Byte-true UNIFIED SCD loop model — While(0x0F)/Ewhile(0x10)/Do(0x11)/
     * Edwhile(0x12)/For(0x0D)/Next(0x0E)/Break(0x1A) all share it. Mirrors the PSX
     * thread fields: counter @+0x08, loop-back @+0x20+idx*4, exit @+0x60+idx*4,
     * For-count @+0xA0+idx*2 (LAB_8003f6f4 ff). idx = loop_count-1. N=4: the PSX
     * +0x20 region holds 4 u32 slots before +0x30. Flat [4] (not per-call-depth)
     * is a documented simplification — no known RE1.5 script nests loops across a
     * Gosub boundary. (If/Else/Endif keep their OWN block_stack — separate model.) */
    const uint8_t *loop_back[4];    /* +0x20 — loop-back PC per nesting     */
    const uint8_t *loop_exit[4];    /* +0x60 — exit PC (While/Do/Break)     */
    uint16_t       loop_for_cnt[4]; /* +0xA0 — For/Next iteration count     */
    int8_t         loop_count;      /* +0x08 — shared nesting counter 0..4  */

    /* 2026-06-09 byte-true If/Else/End_if BLOCK STACK (= PSX thread+0x140 sp).
     * If(0x06 LAB_8003f328) pushes block_end=(pc+4)+block_length and ALWAYS
     * enters the body; the predicate opcode (Ck/Cmp/Member_cmp/Sce_key_ck) then
     * runs as a real opcode and RETURNS its boolean — FALSE makes the dispatcher
     * (FUN_8003f0a0) pop this stack and jump PC to block_end. Else(0x07)/End_if
     * (0x08) pop. Replaces the old op_if pre-evaluation look-ahead.
     * [#17] PER GOSUB-FRAME (indexed by call_depth): the PSX gives each Gosub
     * frame its OWN block region (Gosub LAB_8003fbe8 rebases block_sp to a fresh
     * empty stack, Return LAB_8003fc50 restores the caller's). 124/278 real Gosubs
     * fire from INSIDE an open If-block + 15/22 Gotos unwind a block (pc[1]!=0xFF)
     * → the old single shared stack corrupted the caller's FALSE-pop targets. */
    const uint8_t *block_stack[SCD_CALL_DEPTH_MAX][8];
    int8_t         block_sp[SCD_CALL_DEPTH_MAX];

    /* For/Next loop state now lives in the unified loop model above (loop_back /
     * loop_for_cnt / loop_count — see [#8]). ROOM1170 sub06/07/08 (heli takeoff
     * body + main/tail rotor) drive it via for(n<count){ Add_speed; }. */

    /* AO5-round 2026-05-26: per-thread velocity vector. PSX disasm
     * (LAB_80040f14 / LAB_80040f40) stores Speed_set values at thread
     * offset 0x158 and Add_speed reads them from the SAME thread that
     * executes Add_speed — NOT from the work entity. Critical for
     * ROOM1170 takeoff: sub04 (rotor spin) and sub07 (rotor takeoff)
     * share work entity (main rotor prop) but have independent vel
     * vectors. Without per-thread vel, sub04's infinite Add_speed loop
     * was integrating sub07's vel_y → rotor flew up 2× faster than body.
     *
     * Axes: 0=vx 1=vy 2=vz 3=vrx 4=vry 5=vrz (6 × s16 LE, total 12 B). */
    int16_t        vel[6];
    /* Acceleration slots (byte-true 2026-06-13): the original's velocity array at
     * thread+0x158 has 12 s16 slots — 0..5 = velocity (vel[] above), 6..11 = the
     * matching ACCELERATION. Speed_set(axis, v) writes slot[axis] (axis 0..11), so
     * axis 6..11 land here. Add_aspeed (0x31, LAB_80040fd4) does vel[i] += accel[i]
     * for i=0..5 (apply acceleration); Add_speed then integrates vel into position.
     * The ROOM1130 roller door rises this way (Speed_set(7,-2)=accel for vel_Y,
     * Add_aspeed+Add_speed loops). Axes 6=ax 7=ay 8=az 9=arx 10=ary 11=arz. */
    int16_t        accel[6];

    /* I-round (2026-05-24): per-thread work_slot. Per F11 PSX.EXE
     * disasm, RE1.5 stores the "current work entity" at thread+0x154.
     * Was global on g_scd, causing parallel threads (sub04/05 speed
     * mods, sub02 dialog) to stomp each other → sub02's Work_set(2,0)
     * intended for Elliot got overridden to player by sub04's
     * Work_set(3,3) before sub02's Plc_motion fired. */
    int8_t   work_slot;
    int8_t   work_prop_idx;

    /* BC-round 2026-05-28: per-thread Message_on hold counter. When > 0,
     * op_message_on yields (decrement, return 2). When reaches 0, op
     * advances PC by 4. Implements PSX-canonical cinematic timing.
     * See [[bugfix_BC_message_blocking_2026_05_28]]. */
    uint16_t message_wait;

    /* [#9] Switch (0x13) no longer snapshots a value into the thread: the
     * byte-true handler (LAB_8003fa5c) scans the whole Case/Default table inline
     * and jumps to the matching body, comparing work_vars[var_index] on the spot.
     * The former `switch_val` field was removed (no consumer remains). Switch
     * pushes/pops the unified loop model (loop_count/loop_exit) so Break (0x1A)
     * and Eswitch (0x16) exit it like a loop. */
} scd_thread_t;

/* Phase 4.4.3: Audio events emitted by SCD opcodes.
 * Audio subsystem (Phase 4.6) consumes these from the queue each frame. */
typedef enum {
    SCD_AUDIO_NONE          = 0,
    SCD_AUDIO_SE_ON         = 1,    /* 0x36 — SFX */
    SCD_AUDIO_BGMTBL_SET    = 2,    /* 0x57 — configure BGM table */
    SCD_AUDIO_XA_ON         = 3,    /* 0x59 — XA stream start */
    SCD_AUDIO_SE_VOL        = 4,    /* 0x80 — volume scale */
    SCD_AUDIO_VOICE_ON      = 5,    /* Message_on (0x2B) -> dialogue voice clip.
                                     * sample_id = message id; PC backend plays
                                     * the room's synchro mainNN.wav (CD-XA
                                     * voice keyed by msg id; RE 2026-05-29). */
    SCD_AUDIO_SEQ_CTL       = 6,    /* 0x54 Sce_bgm_control (corrected 2026-06-08: the
                                     * "Sce_espr_control2" label was a Java-extractor
                                     * mislabel) == SsSeq slot control
                                     * (PSX FUN_80044da4). bank = seq slot, volume =
                                     * op (1=SsSeqPlay/loop, 2=SsSeqStop, 3=replay,
                                     * 4=pause, 5=decrescendo). ROOM1170 sub02 toggles
                                     * slot 1 (the rotor SUB layer) at cut boundaries
                                     * = the canonical heli-rotor on/off. RE 2026-05-31. */
} scd_audio_kind_t;

typedef struct {
    uint8_t  kind;                  /* scd_audio_kind_t value */
    uint8_t  bank;                  /* SE bank or BGM table index */
    uint8_t  sample_id;             /* sample number within bank */
    uint8_t  volume;
    uint8_t  pan;
    int16_t  pos_x, pos_y, pos_z;   /* 3D positional cues (Se_on) */
    uint16_t raw_w0, raw_w1;        /* opcode-specific extra args */
} scd_audio_event_t;

#define SCD_AUDIO_QUEUE_SIZE 16

/* VM state */
typedef struct {
    scd_thread_t threads[SCD_THREAD_COUNT];
    uint32_t     tick_count;

    /* Phase 4.4.2: shared script-driven state, consumed by render layer */

    /* Camera change request — set by Cut_chg opcode (0x29).
     * cam_change_pending is sticky: render layer clears after applying. */
    uint8_t  cam_change_pending;
    uint8_t  cam_id;             /* target camera index */
    uint8_t  cam_arg2;
    uint8_t  cam_arg3;
    uint8_t  cam_id_prev;          /* for Cut_old (0x2A)                  */
    uint8_t  cut_auto_enabled;     /* set by Cut_auto (0x3C)              */
    /* (The screen-fade channels live in the dedicated engine module re15_fade.h — ENGINE
     * globals @DAT_800b5458 that persist across room loads, not per-room SCD state. SCD 0x56 =
     * config -> re15_fade_config, 0x57 = kick -> re15_fade_kick. Trace wf_2c73ab52.) */
    /* Phase 8.6 — the port repr of DAT_800aca3c & 1 ("combat active"), the gate the LIVE
     * STAGE1 zombie attack-ARM (overlay FUN_8010ab2c @0x8010a4f0 `andi v0,v0,0x1`) checks
     * before committing a lunge. Byte-true semantics (RE'd 2026-06-29, two agents + raw
     * STAGE1.BIN/EXE traces): DAT_800aca3c is flag-bank 1 (PTR_DAT_80074664[1]); bit 0x1 is
     * a HELD script latch set by the SCD `Set` (0x22) opcode at bank 1 / bit 31 (mask
     * 0x80000000>>31), routed through FUN_80042bac/LAB_80043120 — that runtime-COMPUTED mask is
     * why no literal `ori …,0x1` exists anywhere in the EXE or the 6 overlays (3-agent byte-true
     * sweep). It is NOT engine/per-frame driven, and is CLEARED on room (re)load (FUN_800396fc
     * `DAT_800aca3c &= 0xffff0000` — the ONLY thing that ever touches bit 0). OPEN (savestate, skill
     * re15-room-capture): whether any RE1.5 room script actually issues that Set(1,31) — if it never
     * does, the arm gate is DORMANT in this prototype build and the briefing zombies are a scripted
     * feeding set-piece (they never arm). Default 0 is byte-true either way. Modeled as a dedicated boolean exactly like
     * the sibling bit 0x100 (cut_auto_enabled) above; the g_scd memset on room re-enter / init
     * gives the byte-true room-load clear for free. game_step forwards it to re15_enemy_ai_run_all.
     * DEFERRED (cited): wiring the SCD `Set(bank1,bit31)` event to write this + the feeding->combat
     * WAKE-UP sub-mode handlers (@0x8011f80c[6]/[8]); until both land it stays 0 = the briefing
     * zombies keep feeding (the faithful current behaviour — they never reach the arm). */
    uint8_t  combat_active;
    /* BL-round 2026-05-29: CANONICAL player-mode FSM (PSX DAT_800aca58, read each
     * frame by FUN_80031c44 @0x80031c44, dispatch table 0x80073f90). We reduce
     * the 4 PSX modes to: 0/1 = GAMEPLAY (free, PAD honored — LAB_800318f8 /
     * LAB_80031de8), 2 = SCRIPTED (SCD owns the actor, PAD ignored — PSX modes
     * 2/3 LAB_80035af0/800366bc, which we collapse since our SCD drives Leon's
     * clips directly). Plc_ret writes mode=1 with ZERO delay (PSX 0x80041f90) —
     * gameplay is live the very next frame; there is NO player-side countdown.  */
    uint8_t  player_mode;
    /* Set by the AOT scan when an EXAMINE→work-var AOT fires (ev==0 Aot_set, e.g. ROOM1150
     * slot-2 over Irons). scd_vm_tick consumes it ONCE to re-run the room's sub01 (the
     * work_vars poll) — the re-examine re-polls only on the actual examine, never per-frame. */
    uint8_t  examine_poll_pending;
    /* The LETTERBOX is an ORTHOGONAL render FSM on PSX (FUN_8003014c, not the
     * player mode). It persists through the final cut and closes ~23 frames after
     * Plc_ret (ablauf-measured: cut3 @219773 → bars off @219796). Held on while
     * scripted (sentinel -1); op_plc_ret sets it to the 23-frame close window;
     * the main loop counts it down at 30Hz and at 0 finalizes the handoff. */
    int      letterbox_countdown;
    uint8_t  plc_gun_state;        /* set by Plc_gun (0x4A)               */

    /* On-screen message — set by Message_on opcode (0x2B).
     * `message_active` is the persistent state flag (RE semantics: stays
     * set until the player dismisses or a Message_off opcode clears
     * it). `message_display_frames` is the HUD countdown — set to a
     * fixed duration on each Message_on fire so the demo HUD shows a
     * transient indicator instead of sticking at act=1 forever. */
    uint8_t  message_active;
    uint8_t  message_id;
    uint8_t  message_arg2;
    uint8_t  message_arg3;
    int      message_display_frames;
    /* YES/NO query (Message_on arg2 bit 0x80 = a selection prompt, e.g. the ROOM1130
     * switch "operate it? YES/NO"). The opcode BLOCKS the SCD thread until the player
     * confirms; 0=not a query, 1=waiting for the action button to RELEASE (debounce the
     * press that opened the prompt), 2=waiting for a fresh action PRESS = the answer.
     * On confirm the answer is written to a flag the script Ck's (ROOM1130: (12,31)=0). */
    uint8_t  message_query;
    /* YES/NO cursor (byte-true RE of the FUN_80028134 selection state, DAT_800b8520):
     * `message_select` = 1 while the YES/NO prompt is on screen (render gate for the two
     * options + cursor). `message_choice` is the live cursor selection — 0 = YES, 1 = NO
     * (the original's DAT_800b8520 bit 0, toggled by UP/DOWN, default YES). On confirm it
     * is written to the script-read flag (ROOM1130: (12,31) — Ck(12,31,0) true = YES). */
    uint8_t  message_select;
    uint8_t  message_choice;
    /* Cursor blink counter (byte-true: FUN_80028134 state 4 decrements DAT_800b8525
     * every frame and shows the cursor only when (counter & 0x18) != 0; reset to 0 on
     * each toggle so the cursor is solid right after you move it, then blinks). */
    uint8_t  message_blink;

    /* BYTE-TRUE dialog typewriter FSM (RE'd 2026-06-14 from FUN_80028134 / FUN_80028868).
     * The original reveals the .msg body one glyph at a time (DAT_800b852c = the parse
     * cursor; FUN_80028868 draws only [DAT_800b8528 page-start, DAT_800b852c)). At a page
     * break (0x02) it WAITS for the action button (down-arrow shown), at end (0x01) it waits
     * or auto-holds, at 0x03 it enters YES/NO. Holding the action button during typing makes
     * the timer fall 4× faster = the fast-forward. We mirror that with byte offsets:
     *   message_fsm     — sub-state: 0=TYPING 1=PAGE_WAIT(button) 2=PAGE_TIMED 3=SELECT(yes/no)
     *                     4=END_WAIT(button) 5=END_TIMED 6=DONE
     *   message_parse   — typewriter cursor (byte offset into the raw .msg body)
     *   message_page    — current page's first byte (render draws [page, parse))
     *   message_scroll  — frames per revealed glyph (DAT_800b8524; the 0x04 code sets it)
     *   message_timer   — per-glyph / wait countdown (DAT_800b8525)
     *   message_color   — live colour attr (the 0x05 code) */
    uint8_t  message_fsm;
    uint16_t message_parse;
    uint16_t message_page;
    uint8_t  message_scroll;
    uint16_t message_timer;
    uint8_t  message_color;
    /* 1 = drive the byte-true typewriter FSM (re15_dialog_step in re15_msg_tick) for this
     * message — set for every examine/choice line. 0 = the legacy all-at-once timed display
     * (kept for the verified VOICED cinematic subtitles so the intro timing is untouched). */
    uint8_t  message_fsm_active;

    /* Phase 4.4.3: audio events queue (ring buffer). */
    scd_audio_event_t audio_queue[SCD_AUDIO_QUEUE_SIZE];
    uint8_t           audio_head;     /* next free slot */
    uint8_t           audio_count;    /* events waiting consumption */
    uint32_t          audio_total;    /* lifetime event count (never decreases) */

    /* Phase 4.5.9-F: "work entity" — RE2's implicit target for Member_set /
     * Member_cmp / Pos_set/register / etc. Set by Work_set (0x2E):
     *   kind=1 idx=0 → player (g_actors[RE15_ACTOR_SLOT_PLAYER])
     *   kind=3 idx=N → enemy slot N (maps to actor N+1, +1 offset)
     *   kind=2/4     → room/object entity (NOT in actor pool → -1 sentinel)
     *
     * -1 means "no valid actor target" → Member_set/cmp skip their write.
     * This prevents sub02's `Work_set(2,..); Member_set(12,-30208)` from
     * dragging the player below the floor. */
    int8_t  work_slot;

    /* Obj_model_set props (helicopter parts, lights, etc.). Each call to
     * 0x2D registers one prop with its world position + rotation. Velocity
     * fields enable helicopter takeoff via sub06/07/08 (Speed_set + Add_speed
     * loop). The renderer projects each to screen as a colored marker.   */
    struct {
        uint8_t  active;
        uint8_t  obj_id;
        uint8_t  obj_type;     /* Obj_model_set RENDER type = opcode pc[2] (the byte
                                * LAB_80040914 stores to pool+0x08; FUN_8002c18c
                                * branches: ==3 → FUN_8003e64c sprite-grid, !=3 →
                                * mesh + quad cull). ALL room1170 props = type 0.
                                * (NOT pc[4] — that is the +0x82 BAND field.) */
        uint8_t  band;         /* the prop's FLOOR band = Obj_model_set pc[4] → pool+0x82
                                * (byte-true LAB_80040914 @0x8004096c-74 `lbu v0,4(a2);
                                * sb v0,130(a1)`). Read by the AOT scan's per-entity band
                                * gate (@0x80042cac) in the OBJECT-pool pass (the sce-5
                                * box-on-zone markers, aot_sce_census fix 4). */
        int32_t  x, y, z;
        /* Full Euler rotation. ROOM1170 sub15 Obj_model_set 0x04 (tail
         * rotor) mounts the rotor with rot_x=1024 (Q12 90°) so its local
         * Y-axis points horizontally — sub05's Add_speed then spins it
         * around that tilted axis. Previously only rot_y was stored,
         * making the tail rotor render flat/horizontal like the main
         * rotor instead of vertical/on-the-side. */
        int16_t  rot_x, rot_y, rot_z;
        int16_t  vel_x, vel_y, vel_z;
        /* Angular Y velocity — integrated by Add_speed into rot_y each tick.
         * Written by Speed_set axis 4 (RE2-canonical rot_y-speed slot). Used
         * by ROOM1170 sub04/sub05 helicopter rotor spin (Work_set(3, prop_idx);
         * Speed_set(4, 2); Add_speed; in goto-loop). */
        int16_t  vel_ry;
        /* Object collision box (authored in the Obj_model_set operand stream at
         * pc[22..33] = 6 LE shorts: centre X/Y/Z then half-extent X/Y/Z). Drives
         * the faithful object push-out (FUN_8002cabc) in re15_collision.c. A box
         * with all-zero half-extents = non-collidable (heli/rotors), so non-zero
         * half-extents = SOLID. Verified: obj00 (helipad box) = {0,-900,0,900,900,900}
         * (matches the obj00.md1 mesh AABB ~{936,948,929}); obj01 (heli) = all-zero. */
        int16_t  box_cx, box_cy, box_cz;
        int16_t  box_hx, box_hy, box_hz;
        /* member_0b (object +0xb): the combination-lock NOTCH. The AOT scan sets it to the
         * sce=5 grid-cell slot this object is currently over (byte-true FUN_80042bac @0x80042f5c
         * `sb v0=slot-1, 0xb(entity)`), and the keypad confirm reads it via Member_cmp(15==notch)
         * on the Work_set(3,0) dial object. Read/written by op_member_cmp/op_member_set's prop
         * path. [S1-4 PROG-3 keypad dial input] */
        uint8_t  member_0b;
        /* Objekt-FLAGS = pool+0x00. Byte-true LAB_80040914 @0x80040990-a4:
         * `lhu v0,6(a2); ori v0,v0,0x1; sll/sra 16; sw v0,0(a1)` — also das
         * u16 aus dem Opcode-Strom pc[6..7] ODER 1. (Wird das Objekt ein
         * zweites Mal installiert, schreibt @0x8004098c stattdessen 0 = AUS.)
         * ROOM1090 obj0/obj1 tragen pc[6..7] = 00 01 -> flags = 0x0101.
         * Gelesen von: FUN_8002d2c0 @0x8002d358 (`& 0x101` = kletterbar),
         * FUN_8002d474 @0x8002d59c/@0x8002d680 und FUN_8002d100 @0x8002d170
         * (`& 1` = aktiv). */
        uint16_t flags;
    } props[RE15_SCD_MAX_PROPS];
    uint8_t  prop_count;
    /* When Work_set kind=3 selects a script slot that has NO active
     * actor, route Speed_set/Add_speed to the prop at this index. */
    int8_t   work_prop_idx;

    /* 2026-06-09 byte-true: the GLOBAL 16-bit work-variable array = DAT_800b0fd0.
     * Switch (0x13, LAB_8003fa5c) and Cmp (0x23, LAB_8003ff68) index it by
     * var<<1 (`lh a0,0x0(DAT_800b0fd0 + var*2)`). It is GLOBAL (shared across
     * threads), NOT per-thread locals[]. ROOM1170 sub00 `switch(10)` reads
     * work_vars[10] = the entry scenario, stamped at room (re)entry from the
     * door descriptor. 256 entries (var index is a u8). */
    int16_t  work_vars[256];
} scd_vm_t;

/* Phase 4.5.9-F: SCD game-flag tables.
 *
 * RE2's bit storage is per-zone u32 arrays addressed via:
 *   *(tbl + (idx >> 5)) & (0x80000000 >> (idx & 0x1f))
 *
 * (FUN_80077360 / FUN_8007730c). Scripts use idx up to 255 within a
 * single zone (verified vs ROOM1021/sub00 Ck(3,133,1), ROOM11C0 Ck(4,64,0)
 * etc). Our previous `zone*32+idx` packing collided once idx > 31. */
#define RE15_FLAG_ZONES         16
#define RE15_FLAG_WORDS_ZONE    8        /* 8 u32 words = 256 bits per zone */

typedef struct {
    /* Byte-faithful to the original: per-zone u32 array, MSB-first WITHIN each
     * 32-bit word — word = idx>>5, mask = 0x80000000 >> (idx&0x1f)
     * (FUN_8004ef90 set / FUN_8004efe4 check; = RE2 FUN_80077360/FUN_8007730c).
     * A byte-array layout selects DIFFERENT bits for the same idx, which would
     * mis-read any future savegame / flag-blob. */
    uint32_t flags[RE15_FLAG_ZONES][RE15_FLAG_WORDS_ZONE];
} re15_game_state_t;

extern re15_game_state_t g_game;

void re15_game_state_init(void);
int  re15_game_flag_get(uint8_t zone, uint8_t idx);     /* 0 or 1 */
void re15_game_flag_set(uint8_t zone, uint8_t idx, int value);

/*=========================================================================
 * GLOBALE PAUSE-FLAGS = DAT_800aca40 (byte-true, RE 2026-08-17).
 *
 * DAS ist der Mechanismus hinter "wenn man im Original einen Text liest,
 * friert alles andere ein". Er ist DATEN-getrieben, nicht pauschal:
 *
 *   FUN_80027e68 (Message-Open) bekommt die Maske als param_4 und macht
 *     @0x80027e74  lbu v0,DAT_800b8520 / @0x80027e7c andi 0x80 / @0x80027e80 beq
 *                  -> schon offen: `j 0x800280ac; addiu v0,zero,-1` (KEIN Re-Save)
 *     @0x80027eb4  lw  v0,0(a0)          a0 = 0x800aca40 (@0x80027e98 addiu a0,a0,-13760)
 *     @0x80027ec8  sw  v0,DAT_800b853c   <- SNAPSHOT des Vorzustands
 *     @0x80027ecc  or  v0,a3,v0
 *     @0x80027ed0  sw  v0,0(a0)          <- g_pauseflags |= Maske
 *
 * Die Maske kommt aus den ROOM-DATEN:
 *   SCD 0x2B Message_on @0x800404f4: @0x80040508 `lhu a3,2(v0)` + @0x8004051c `sll a3,16`
 *   sce-1 Examine-AOT   @0x80043084: @0x80043098 `lhu a3,2(v0)` + @0x800430a4 `sll a3,16`
 * Eigener Census ueber alle 240 ausgelieferten RDTs (2026-08-17):
 *   Message_on 698x -> 420x 0x0000 (Untertitel: KEIN Freeze), 34x 0xff80, 244x 0xffff
 *   sce-1 AOT  511x -> ALLE 0xffff  (=> JEDER Examine-Text friert die Welt ein)
 *   ROOM1240 (6) + ROOM1170 (16) Kino-Captions: ALLE 0x0000 -> Cutscenes laufen weiter.
 *
 * Close = Snapshot-Restore (FUN_80028134): @0x800285a4 (Select-Confirm),
 * @0x800286cc (End-Wait), @0x8002871c (Hold-N-Timeout) — jeweils
 * `lw a0,DAT_800b853c` -> `sw a0,0x800aca40`.
 * Raumwechsel loescht das Wort komplett: @0x8001ca44 / @0x8001caec `sw zero,0x800aca40`.
 *
 * Jeder Leser gated sich SELBST auf sein eigenes Bit (Haupt-Loop ruft alles
 * IMMER, @0x8001cdec..@0x8001ce34).
 *=========================================================================*/
#define RE15_PAUSE_PLAYER  0x80000000u  /* Spieler-Dispatcher FUN_80031c44: @0x80031c54 `lw a0,g_pauseflags`
                                         * + @0x80031c78 `bltz a0,0x80031da8` (Vorzeichen-Bit) */
#define RE15_PAUSE_AI      0x20000000u  /* alle AI-Roots, exemplarisch Live-Zombie FUN_80100424 (STAGE1.BIN):
                                         * @0x8010042c lw / @0x80100430 `lui v1,0x2000` / @0x80100438 and /
                                         * @0x8010043c `bne -> 0x80100658` */
#define RE15_PAUSE_ACTION  0x10000000u  /* Action-Driver / Model-Instance-Animator FUN_80019e20:
                                         * @0x80019e28 lw / @0x80019e2c `lui v1,0x1000` / @0x80019e3c and /
                                         * @0x80019e40 `bne -> 0x8001a4a4` */
#define RE15_PAUSE_SCD     0x02000000u  /* SCD-Frame-Runner FUN_8003f038: @0x8003f040 lw /
                                         * @0x8003f044 `lui v1,0x200` / @0x8003f04c `bne -> 0x8003f090` */
#define RE15_PAUSE_PAD     0x01000000u  /* Pad-Remap FUN_80030444: @0x800304f4 lw / @0x800304f8 `lui v1,0x100` /
                                         * @0x80030500 beq / @0x80030514 `andi v0,v0,0xf000` /
                                         * @0x8003051c `sw 0x800ac768` (Held-Wort; das Edge-Wort
                                         * 0x800ac76c wird DANACH daraus gebildet, Decompile Z.26) */

extern uint32_t g_re15_pauseflags;        /* DAT_800aca40 */
extern uint32_t g_re15_pauseflags_saved;  /* DAT_800b853c — Snapshot fuer den Restore */

void re15_pauseflags_open(uint32_t mask);  /* @0x80027eb4-ed0 (mit Open-Guard @0x80027e74) */
void re15_pauseflags_close(void);          /* @0x800285a4 / @0x800286cc / @0x8002871c */
void re15_pauseflags_clear(void);          /* @0x8001ca44 / @0x8001caec (Raumwechsel) */

/* Object (prop) work-entity member access — Work_set kind 3. member 15 = member_0b = the
 * combination-lock NOTCH (S1-4 PROG-3 keypad dial). */
int32_t re15_prop_get_member(int prop_idx, uint8_t member_id);
void    re15_prop_set_member(int prop_idx, uint8_t member_id, int32_t value);

extern scd_vm_t g_scd;

/* VM control */
void scd_vm_init(void);
void scd_vm_tick(void);    /* call once per 30Hz tick (every 2nd vsync) */

/* ===========================================================================================
 *  re15_cam_present_tick — der SELBSTHEILENDE Kamera-Apply des Originals
 * ===========================================================================================
 * Einmal PRO BILD aufrufen, NACH scd_vm_tick + re15_aot_scan (die Stelle der Present-Routine
 * FUN_8002137c im Original-Hauptloop). Rueckgabe 1 = dieses Bild muss der Aufrufer den Cut
 * anwenden (Ansicht bauen, BG/Licht/sprite.pri des Cuts laden); 0 = nichts zu tun.
 *
 * BYTE-TRUE (PSX.EXE, alle Adressen selbst disassembliert aus ghidra1_V2.txt):
 *
 *   DAT_800afbb5 (u8)  = der ANGEFORDERTE Cut.  Genau 3 Schreiber:
 *        FUN_800142f4 @0x80014300 `sb a0,-0x44b(at)`  (der Kamera-Setzer; Aufrufer u.a. der
 *                     RVD-Zonen-Scan FUN_80014230 @0x800142ac, Cut_chg @0x80040300,
 *                     Cut_old @0x80040368, Cut_replace @0x800404c8)
 *        FUN_8001d600 @0x8001d818 `sb zero` (Boot) / @0x8001d940 `sb v1` (Tuer-Payload Byte 10)
 *      -> Port: g_scd.cam_id
 *
 *   DAT_800b0fe4 (s16) = work_vars[0x0A] = der zuletzt ANGEWANDTE (angezeigte) Cut.
 *      work_vars-Basis 0x800b0fd0 (work_var[0]@0x800b0fd0, [1]@0x800b0fd2) -> 0x800b0fe4 = Index 0x0A.
 *      Genau 5 Schreiber: FUN_80021bbc @0x80021bfc (DIESER Apply), FUN_8001d600 @0x8001d820 /
 *      @0x8001d948 (Raum-Warp: zusammen mit DAT_800afbb5 gestempelt), Cut_chg @0x800402fc,
 *      Cut_old @0x80040364.  Der RVD-Zonen-Scan schreibt ihn NICHT.
 *      -> Port: g_scd.work_vars[0x0A]
 *
 *   DAT_800b5457 (u8)  = Dirty-Flag -> Port: g_scd.cam_change_pending
 *
 *   Der Kern, Present-Routine FUN_8002137c:
 *        @0x800214e0 `lw v0,DAT_800aca3c` @0x800214e8 `andi v0,v0,0x80`
 *        @0x800214ec `bne v0,zero,LAB_80021518`      Bit 0x80 gesetzt -> Selbstheilung AUS
 *        @0x800214f8 `lh  v1,DAT_800b0fe4`           angewandter Cut
 *        @0x80021500 `lbu v0,DAT_800afbb5`           angeforderter Cut
 *        @0x80021508 `beq v1,v0,LAB_80021518`        gleich -> nichts
 *        @0x80021514 `sb  1,DAT_800b5457`            UNGLEICH -> JEDES BILD neu dirty
 *        @0x80021538 `beq v0,zero,LAB_80021568`      nicht dirty -> kein Apply
 *        @0x80021558 `jal FUN_80021bbc`              Apply
 *        @0x80021618 `sb  zero,DAT_800b5457`         Dirty am Bildende geloescht
 *   und der Apply-Kopf FUN_80021bbc:
 *        @0x80021be0 `lbu v1,DAT_800afbb5`
 *        @0x80021be8 `lhu v0,DAT_800b0fe4`
 *        @0x80021bf4 `sh  v0,DAT_800b0fe8`           work_vars[0x0C] = alter angewandter Cut
 *        @0x80021bfc `sh  v1,DAT_800b0fe4`           work_vars[0x0A] = angeforderter Cut
 *
 * WARUM DAS NOETIG IST: der RVD-Zonen-Scan setzt NUR DAT_800afbb5. Nichts an diesem Pfad
 * bewaffnet das Dirty-Flag — der Kamerawechsel beim Durchlaufen eines Raums entsteht
 * AUSSCHLIESSLICH aus diesem Per-Bild-Vergleich. Der Port hatte stattdessen ein EINMALIGES
 * Flag: ging es verloren oder blieb der Apply in dem Bild aus, stand das Bild DAUERHAFT
 * (Nutzer-Befund "nach dem Generator-Raetsel wechselt die Kamera nicht mehr"). */
int  re15_cam_present_tick(void);

/* Start a thread executing bytecode at `pc` */
int  scd_thread_start(int slot, const uint8_t *pc);

/* Kill a specific thread (sets kill_pending; takes effect at next yield) */
void scd_thread_kill(int slot);
/* Unbedingter Reseed (FUN_8003ee3c -> FUN_8003edec): ueberschreibt den Slot auch wenn er laeuft —
 * das braucht der Per-Frame-sub01-Reseed. scd_thread_start weigert sich bei aktivem Slot. */
void scd_thread_reseed(int slot, const uint8_t *pc);
/* Raum-Init-Lauf markieren: waehrend er laeuft unterbleibt der sub01-Reseed (das Original hat dafuer
 * zwei getrennte Call-Sites, FUN_8003ef6c @0x8003f018 vs FUN_8003f038 @0x8003f088). */
void scd_vm_set_room_init(int on);

/* Phase 4.4.6.1: event-handler registry. Bytecode authors register a
 * handler (entry pc) for each event_id; AOT triggers (and later other
 * event sources) call `scd_event_fire` to spawn a thread executing
 * that handler. Slot 0 is reserved for the main demo thread; events
 * use slots 1..SCD_THREAD_COUNT-1. */
void                  scd_event_register(uint8_t event_id, const uint8_t *pc);
const uint8_t        *scd_event_get_handler(uint8_t event_id);
int                   scd_event_fire(uint8_t event_id);  /* returns thread slot, -1 if none free */

/* scd room-setup backend — provides the boot bytecode + event handler registration.
 * Phase 4.5.9-B: `rdt` may be NULL (RDT load failed) — demo then only sets
 * up the hand-built Door + Item AOTs. */
#include "re15_rdt.h"
const uint8_t *scd_fallback_bytecode(void);
void           scd_register_room_events(const re15_rdt_t *rdt);

/* Faithful same-room door self-reentry (room1170 narrator→helipad handoff):
 * re-inits the SCD VM + re-runs init(main00)+main(sub00) with the GAME FLAGS and
 * PLAYER actor PRESERVED — mirrors the original FUN_8001d600→FUN_8003ef6c room
 * reload (whose flag banks survive). Same room → no asset reload. */
/* ─────────────────────────────────────────────────────────────────────────────────────
 * SPIELERMODELL-RUECKRUF — und warum er GENAU HIER haengt.
 *
 * Das Original haengt den Modellwechsel an den RAUMLADER, nicht an die Tuer und nicht
 * ans Skript: FUN_800396fc @0x80039760-8c vergleicht die untere Nibble von DAT_800aca5c
 * mit work_vars[0x10] (DAT_800b0ff0) und laesst bei Abweichung FUN_800314b0 laden.
 *
 * ⛔ UND DER RAUMLADER LAEUFT AUCH BEI EINER SELBST-TUER. Byte-belegt im Tuer-Warp
 * FUN_8001d600:
 *     8001d960  lbu  v0,8(a0)             ; Ziel-STAGE
 *     8001d968  beq  v1,v0,0x8001d988     ; gleiche Stage -> Stage-Lader ueberspringen
 *     8001d980  jal  0x80039a30           ; (nur bei Stage-Wechsel)
 *     8001d988  jal  0x800396fc           ; RAUMLADER — UNBEDINGT
 * Verglichen wird NUR die Stage. Der Raum selbst wird nie gegen den aktuellen geprueft.
 *
 * Der Port tat genau das aber: aot_common.c gatet den Ladeweg mit
 * `if (dest_id != g_current_room_id)`, und die Ruestungs-Tuer in ROOM1190 (Record
 * @Datei 0x2d70) zielt auf ROOM1190 SELBST. Ergebnis: der Anlege-Vorgang lief, setzte
 * work_vars[0x10] = 1 — und kein Ladeweg zog das Modell nach. Der Rueckruf haengt darum
 * an scd_room_reenter, durch das ALLE drei Ladewege des Ports laufen (Selbst-Raum
 * game_step_common.c, Raumwechsel room_common.c, Boot main.c).
 *
 * Plattform-Sache, weil das Laden der PLD plattformseitig ist; ohne Registrierung
 * (z.B. PSX-Target, Unit-Tests) bleibt das Verhalten unveraendert. */
typedef void (*re15_player_model_sync_fn)(void);
void           re15_scd_set_player_model_sync(re15_player_model_sync_fn fn);

void           scd_room_reenter(const re15_rdt_t *rdt, int32_t player_x, int32_t player_z,
                                uint8_t entry_scenario);
/* Signal from the door AOT scan: a same-room door whose entry scenario triggers a
 * sub00 cutscene case has fired. -1 = none; else the scenario (e.g. 11 = outdoor →
 * sub14). The main loop performs the scd_room_reenter and clears it. */
extern int     g_scd_pending_scenario;
/* Latched when the game-step self-room scenario reenter has fired (the byte-true intro
 * door-3 handoff). The PC hand-deferred sub00-spawn fallback checks it so it can't fire a
 * redundant second reenter (which would spawn the intro crows). Reset on room entry. */
extern int     g_scd_self_reenter_fired;
/* 1 if actor `slot` is currently the Work_set-bound work-entity of an active SCD thread (the SCD
 * owns its animation via Plc_dest/Plc_motion — the enemy brain must yield). */
int            re15_scd_slot_event_controlled(int slot);
/* 1 if `room_id` uses the all-at-once full-text pre-intro NARRATOR (the "we barricaded ourselves"
 * captions) — the rooms {0x1170,0x1240} whose narration plays on the held cut-to-black fade. */
int            re15_room_full_text(unsigned room_id);

/* SCD opcode constants (subset for Phase 4.4) */
#define SCD_OP_NOP        0x00
#define SCD_OP_EVT_END    0x01
#define SCD_OP_EVT_EXEC   0x04
#define SCD_OP_EVT_KILL   0x05
#define SCD_OP_IF         0x06
#define SCD_OP_ELSE       0x07
#define SCD_OP_END_IF     0x08
#define SCD_OP_SLEEP      0x09
#define SCD_OP_SLEEPING   0x0A
#define SCD_OP_SWITCH     0x13   /* Phase 4.4.2 */
#define SCD_OP_CASE       0x14
#define SCD_OP_DEFAULT    0x15
#define SCD_OP_END_SWITCH 0x16
#define SCD_OP_GOTO       0x17
#define SCD_OP_GOSUB      0x18
#define SCD_OP_RETURN     0x19

/* RE1.5-extended opcodes (subset) */
#define SCD_OP_CK            0x21   /* 4B  — flag bit check  (Phase 4.4.4) */
#define SCD_OP_SET           0x22   /* 4B  — flag bit write  (Phase 4.4.4) */
#define SCD_OP_CUT_CHG       0x29   /* camera/cut change — Phase 4.4.2 */
#define SCD_OP_MESSAGE_ON    0x2B   /* on-screen message — Phase 4.4.2 */
/* Phase 4.5.13-E (2026-05-20): Sleep opcode constants CORRECTED via
 * sub02.scd byte analysis + PSX.EXE @0x8003F3E0/0x8003F428 disasm.
 * RE1.5 uses 0x09=Sleep INIT (reads LE u16 dur @PC+2, stores in slot)
 * and 0x0A=Sleeping LOOP (decrement). Previous off-by-one constants
 * (0x08/0x09) caused all sub02 Sleeps to no-op — the 0x09 in bytecode
 * hit our op_sleeping which found empty slot and skipped 3 bytes. */
#define SCD_OP_SLEEP_LEGACY  0x08   /* RE2 mapping (kept for now, may be unused) */
#define SCD_OP_POS_SET       0x32   /* 8B  — player pos set (Phase 4.4.4) */
#define SCD_OP_DIR_SET       0x33   /* 8B  — player rotation set (Phase 4.4.4) */
#define SCD_OP_PLC_MOTION    0x3F   /* 4B  — set animation clip (Phase 4.4.4) */
#define SCD_OP_PLC_DEST      0x40   /* 8B  — walk-to waypoint (Phase 4.5.13-B) */
#define SCD_OP_PLC_NECK      0x41   /* 10B — head/neck orientation (Phase 4.5.13-C) */
#define SCD_OP_PLC_RET       0x42   /* 1B  — return cinematic control (Phase 4.5.13-D) */
#define SCD_OP_PLC_FLG       0x43   /* 4B  — set player anim flag (Phase 4.5.13-C) */
#define SCD_OP_AOT_SET       0x2C   /* 20B — trigger zone (Phase 4.4.6) */
#define SCD_OP_AOT_RESET     0x46   /* 10B — disable trigger zone (Phase 4.4.6) */
#define SCD_OP_WORK_SET      0x2E   /* 3B  — select work entity (Phase 4.5.9-F) */
#define SCD_OP_SPEED_SET     0x2F   /* 4B  — set velocity component (RE2 F6)  */
#define SCD_OP_ADD_SPEED     0x30   /* 1B  — apply velocity to position       */
#define SCD_OP_MEMBER_SET    0x34   /* 4B  — actor property write (Phase 4.4.8) */
#define SCD_OP_MEMBER_CMP    0x3E   /* 6B  — actor property compare (Phase 4.4.8) */
#define SCD_OP_SCE_EM_SET    0x44   /* 22B — spawn enemy/NPC (Phase 4.4.8) */

/* Phase 4.4.3 audio opcodes (per SCDScriptDisassembler) */
#define SCD_OP_SE_ON         0x36   /* 12B — SFX play */
#define SCD_OP_SCE_BGMTBL_SET 0x57  /* 8B  — configure BGM table entry */
#define SCD_OP_XA_ON         0x59   /* 4B  — XA stream start */
#define SCD_OP_SE_VOL        0x80   /* 2B  — SFX volume scale */

/* Phase 4.4.3 consumer API for audio subsystem (Phase 4.6 will use these) */
int scd_audio_queue_pop(scd_audio_event_t *out);  /* returns 1 if event, 0 if empty */

/* Phase 4.4.7: producer-side enqueue, exposed so non-SCD subsystems
 * (AOT C dispatchers, future native gameplay code) can trigger SFX
 * without going through bytecode. */
void scd_audio_queue_push(const scd_audio_event_t *evt);

/* Phase 4.4 debug helper: opcode 0xFE writes text via re15_debug_text.
 * Bytecode encoding: [0xFE x y len text...]
 * Custom opcode (not in RE1.5 ISA) — for VM bring-up testing only. */
#define SCD_OP_DBG_TEXT   0xFE

#endif
