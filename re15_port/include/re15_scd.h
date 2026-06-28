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
     * (0x08) pop. Replaces the old op_if pre-evaluation look-ahead. */
    const uint8_t *block_stack[8];
    int8_t         block_sp;

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
    } props[16];
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

extern scd_vm_t g_scd;

/* VM control */
void scd_vm_init(void);
void scd_vm_tick(void);    /* call once per 30Hz tick (every 2nd vsync) */

/* Start a thread executing bytecode at `pc` */
int  scd_thread_start(int slot, const uint8_t *pc);

/* Kill a specific thread (sets kill_pending; takes effect at next yield) */
void scd_thread_kill(int slot);

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
void           scd_room_reenter(const re15_rdt_t *rdt, int32_t player_x, int32_t player_z,
                                uint8_t entry_scenario);
/* Signal from the door AOT scan: a same-room door whose entry scenario triggers a
 * sub00 cutscene case has fired. -1 = none; else the scenario (e.g. 11 = outdoor →
 * sub14). The main loop performs the scd_room_reenter and clears it. */
extern int     g_scd_pending_scenario;

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
