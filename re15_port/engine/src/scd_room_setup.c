/*
 * RE1.5 Rebuilt — per-room SCD setup: RVD camera-zone install + event/RDT registration
 * + same-room door self-reentry. (Renamed 2026-06-14 from scd_demo.c — this is no longer a
 * demo; it sets up every room's SCD-driven AOTs/cameras.)
 *
 * s_main_bytecode is kept ONLY as the fallback when the real RDT main SCD is missing
 * (scd_fallback_bytecode()); with a valid room RDT it is unused. The former synthetic
 * 0x10/0x11 event handlers were removed (2026-06-03 audit fix) — real room events flow
 * through main00/sub15 Evt_exec → sub_scd[].
 */

#include <stdint.h>
#include <string.h>
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_rdt.h"
#include "re15_actor.h"

extern scd_vm_t g_scd;

#define U16_LO(v) ((uint8_t)((v) & 0xFF))
#define U16_HI(v) ((uint8_t)(((v) >> 8) & 0xFF))

/* Main thread bytecode — runs once at boot, then sleeps forever. */
static const uint8_t s_main_bytecode[] = {
    /* Pos_set (-20538, 0, -25722) — match ROOM1100 cut 0 target so the
     * initial spawn matches the demo's starting cut. (Cut auto-teleport
     * in main.c will keep Leon at each cut's target on cycle.) */
    SCD_OP_POS_SET, 0x00,
        U16_LO(-20538), U16_HI(-20538),
        U16_LO(0),      U16_HI(0),
        U16_LO(-25722), U16_HI(-25722),
    /* Dir_set yaw=0 */
    SCD_OP_DIR_SET, 0x00,  0, 0,  0, 0,  0, 0,
    /* Plc_motion 0 (idle) */
    SCD_OP_PLC_MOTION, 0x00, 0x00, 0x00,

    /* AOTs are set up in scd_register_room_events (C-side) so this
     * bytecode stays small. Real RE1.5 Aot_set / Door_aot_set / Item_aot_
     * set encodings will be ported alongside the Phase 4.5.9 RDT loader. */

    /* Sleep 60 ticks (= 2 s at 30 Hz). Then Goto back so we keep
     * sleeping forever. The thread must stay active so re15_scd_fire_event
     * can spawn handler threads (event slots are 1..N-1; slot 0 is us). */
    SCD_OP_SLEEP, SCD_OP_SLEEPING, 0x3C, 0x00,
    SCD_OP_GOTO, 0xFC, 0xFF,    /* offset = -4 → loop the Sleep */
};

/* (2026-06-03 audit fix) The synthetic 0x10/0x11 event handlers were removed.
 * They were demo scaffolding (Cut_chg(2)/Cut_chg(5) with filler 0xAA/0xBB operands
 * + an eyeballed Se_on) registered unconditionally — not derived from any RDT/SCD.
 * Worse, a real AOT/RVD that happens to carry event id 0x10/0x11 would have been
 * HIJACKED by the demo handler. Real room events flow through main00/sub15's
 * Evt_exec(0x18NN) → sub_scd[] (op_evt_exec), not this C-side table. */

#undef U16_LO
#undef U16_HI

const uint8_t *scd_fallback_bytecode(void) { return s_main_bytecode; }

/* Register event handlers — call from main.c after scd_vm_init.
 * Separated from the main bytecode because the registry is a C-side
 * table (no SCD opcode for it yet); a future Phase 4.4.7 could replace
 * this with an in-bytecode Evt_chain-style registration. */
/* Phase 4.5.9-C: the hand-built demo scaffold is gone. The only thing
 * this function still does is register event handlers for events 0x10 /
 * 0x11 (in case any AOT or sub-SCD references those ids), and install
 * RVD trigger zones from the RDT — RE2-faithful because the RVD data
 * comes straight from real ROOM1100.RDT, not from our demo C code. */
/* Phase 4.5.11: also exposes the RDT pointer to scd_vm via a registered
 * accessor so op_evt_exec can look up sub_scd[]. */
void scd_register_current_rdt(const re15_rdt_t *rdt);

void scd_register_room_events(const re15_rdt_t *rdt)
{
    /* (synthetic 0x10/0x11 handler registration removed — see note above) */

    /* AR-round 2026-05-26: install RVD at slot 16+ instead of 0. SCD's
     * main00 calls Door_aot_set(0..6, ...) + Aot_set(7..10, ...) at room
     * boot, which OVERWROTE the RVD slots we installed first at slots 0-10.
     * Only slots 11-13 survived as CAM_SWITCH; slots 0-1 (the critical
     * cam_from=0 → cam_to=1 transit zones) were stomped to DOOR type, so
     * RVD never fired during Leon's helipad walk.
     *
     * RE15_AOT_MAX=32 → 16..(16+13)=29 → 14 RVD slots, leaving 30-31 free.
     * No conflict with SCD-allocated slots 0-10. */
    if (rdt) {
        re15_rdt_apply_zones_as_aots(rdt, 16);
    }

    /* Phase 4.5.11: Evt_exec needs RDT.sub_scd[] table at dispatch time. */
    scd_register_current_rdt(rdt);
}

/* Faithful same-room door self-reentry — see re15_scd.h. Mirrors the original
 * FUN_8001d600→FUN_800396fc→FUN_8003ef6c room-reload chain that sub11's Aot_on(3)
 * (door 3, dest stage0/room0x17 == THIS room) triggers: re-init the SCD VM and
 * re-register init(main00, RDT+0x40) + main(sub00, RDT+0x44), then run them. The
 * original's flag banks (DAT_800b0ff8 etc.) are NOT cleared by a room load, so the
 * flags sub11 set — (3,125),(4,242),(2,7) — PERSIST and drive the re-run: main00 now
 * takes the Ck(3,125,1)→BGM path (no narrator re-spawn), and sub00→sub15 sees
 * Ck(4,242,1) → Evt_exec(0x1802) = the helipad cinematic. This is what our prior
 * hand-deferred sub00 spawn faked; now it is the real mechanism.
 *
 * = scd_vm_init MINUS re15_game_state_init (keep flags) MINUS re15_actor_init (keep
 * the player) MINUS register_opcodes (table persists). Same room → RDT/scripts/
 * models stay resident, so NO asset reload (a cross-room door would also need
 * re15_room_load + a VRAM reset — the separate multi-room work). */
void scd_room_reenter(const re15_rdt_t *rdt, int32_t player_x, int32_t player_z,
                      uint8_t entry_scenario)
{
    memset(&g_scd, 0, sizeof(g_scd));   /* reset threads/props/cam/modes/audio-queue */
    g_scd.work_slot = -1;               /* (matches scd_vm_init) */
    g_scd.cut_auto_enabled = 1;         /* RVD auto-camera ON at room entry (byte-true: the room-
                                         * (re)load chain FUN_800396fc masks DAT_800aca3c's low 16
                                         * bits off @0x80039710-30 = bit 0x100 CLEAR = scan runs;
                                         * Cut_chg re-disables. wf_559c230f AUTOSCAN-DEFAULT) */
    /* The SAME masking clears flag bank 1's LOW-16-value bits (aca3c &= 0xffff0000) — in the
     * MSB-first model that is idx 16..31, incl. the LETTERBOX direction bit (idx 27, value 0x10)
     * and the combat latch (idx 31): a cutscene room that left them set cannot leak bars/combat
     * into the next room. (Byte-true @0x80039730; trace wf_bba41002 #21.) */
    for (int fi = 16; fi < 32; fi++) re15_game_flag_set(1, (uint8_t)fi, 0);
    /* ...und die ZWEITE Haelfte des Cutscene-Latches, aus dem main.c player_mode jeden Frame NEU
     * ableitet (`cine_active = flag(1,27) || flag(2,7)`). Ohne das war der player_mode-Reset des
     * Raumwechsels (room_common.c) wirkungslos: flag(1,27) loescht die Bank-1-Maske oben byte-true
     * mit, flag(2,7) bisher NICHT — wer einen Raum verliess, waehrend eine Cutscene lief, behielt
     * dauerhaft einen Spieler, der keine Eingabe mehr annahm (gemessen: 600 Frames pmode=2,
     * unrettbar; Szenario C in probe_cut_1030).
     *
     * Warum das byte-true ist: flag(2,7) hat im Port GENAU EINEN Leser — eben diesen Proxy
     * (repo-weiter grep: nur platform/{pc,psx}/main.c). Es ist damit die Port-Speicherstelle fuer
     * "der Spieler ist skriptgefuehrt" — im Original das Routinen-Register DAT_800aca58, das die
     * Transitions-FSM @0x8001CBDC `sb zero,0x800aca58` beim Raumwechsel UNBEDINGT nullt. Beide
     * Haelften des Proxys folgen damit derselben Regel wie das Original.
     * (Die Flag SELBST hat im Original zwei weitere Wirkungen — Pad-Maske DAT_800ac768 &= 0xf000
     * @0x800304f4-51c und Inventar-Sperre @0x8001cd0c-20 —, die der Port noch nicht nachbildet;
     * dokumentierte, davon unabhaengige Luecke.) */
    re15_game_flag_set(2, 7, 0);
    /* FLAG-BANK 5, WORT 0 — der pro-Raum-Scratch. Byte-true FUN_8003ecec @0x8003ed74
     * `sw zero,0x800b1028`, gerufen aus der SCD-Raum-Init FUN_8003ef6c @0x8003ef84, die selbst an
     * der Raum-Ladekette FUN_800396fc @0x80039a00 haengt. (Wort 1, die Bits 0x20..0x3F, ist KEIN
     * Raum-Scratch sondern ein Ein-Frame-Handshake und wird am Ende jedes VM-Laufs gewischt —
     * siehe scd_vm_tick. Bits >= 0x40 liegen im Original in Bank 6/7 und bleiben unangetastet.)
     * Bank 5 ist im Original genau 8 Byte gross: Tabelle 0x80074664[5]=0x800b1028, [6]=0x800b1030. */
    g_game.flags[5][0] = 0;
    /* ...und aus DERSELBEN Funktion die beiden Gegner-Spawn-Zaehler (dieselbe Instruktionsfolge,
     * nur ein paar Zeilen davor bzw. danach):
     *   @0x8003ed58/60  `ori v0,zero,0xff` + `sh v0,4084(at)` -> 0x800b0ff4 = MAXIMUM := 0xFF
     *   @0x8003ed7c     `sh zero,4082(at)`                    -> 0x800b0ff2 = ZAEHLER := 0
     * Bei Basis 0x800b0fd0 sind das work_vars[0x12] (Maximum) und work_vars[0x11] (Zaehler).
     * Das Maximum MUSS hier auf 0xFF stehen: der memset oben nullt work_vars, und ein Maximum von
     * 0 wuerde in JEDEM Raum jeden Spawn verwerfen. Nur ROOM1030/1031 und ROOM1040/1041 setzen es
     * per Save(0x12,n) (Opcode 0x24) auf 6 bzw. 5 herunter — siehe das Gate in op_sce_em_set. */
    g_scd.work_vars[0x12] = 0xFF;
    g_scd.work_vars[0x11] = 0;
    /* Room re-entry re-inits the ENTITY pool. g_actors[] is a SEPARATE array from
     * g_scd (the memset above does NOT touch it), so boot-spawned NPCs would survive
     * the transition. The original clears all entities on room load; clear NPC slots
     * 1..N here (slot 0 = player is KEPT — already repositioned at the door spawn by
     * the door fire). This is what makes the despawn correct: door 6 → scenario 0 →
     * sub15 ELSE spawns the 7 crows on a pad now EMPTY of Elliot (type 0x47) + pilot,
     * instead of stacking them on top of the persistent cinematic cast. */
    for (int ai = 1; ai < RE15_ACTOR_MAX; ai++)
        if (g_actors[ai].active) memset(&g_actors[ai], 0, sizeof(g_actors[ai]));
    g_actor_count = 1;
    /* A same-room re-entry mirrors the original's room-reload PLAYER re-init.
     * Our run-to-door is a PERSISTENT walker (actor.walk_active + motion
     * 100/105 — an engine construct with NO original counterpart; the original
     * locomotion is pad/script-driven per frame). It must NOT survive the door
     * transition, else Leon keeps cycling the run/walk clip into the cutscene
     * (sub14) until its first Plc_motion (gated behind Sleep(20)+Message_on).
     * Clear the transient locomotion here; the new scenario's Pos_set + Plc_*
     * re-drive the player. (2026-06-08, byte-true: undoes OUR construct at the
     * re-init point, not a motion-reset bolted onto an opcode that lacks it.) */
    {
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->walk_active = 0;
        pl->walk_fsm    = 0;
        /* Reset to the IDLE sentinel (200 → W01 clip 3, arms-down), NOT 0: motion 0 is
         * NOT a locomotion sentinel and renders the DEFAULT bank clip 0 = the per-room
         * cinematic RBJ overlay's clip 0 (a gesture in e.g. ROOM1150 → the "arm-out before
         * kneel" bug). 200 is bank-independent. (Same fix as actor_locomotion.c arrival.) */
        if (pl->motion == 100 || pl->motion == 105) re15_actor_set_motion(pl, 200);
    }
    re15_aot_init();                    /* clear AOT slots … */
    scd_register_room_events(rdt);      /* … then reinstall RVD@16 + register rdt */
    if (rdt && rdt->main_scd)   scd_thread_start(0, rdt->main_scd);    /* init  = main00 */
    if (rdt && rdt->sub_scd[0]) scd_thread_start(1, rdt->sub_scd[0]);  /* main  = sub00  */
    /* sub00 dispatches on the GLOBAL work-var DAT_800b0fd0[10] = the ENTRY SCENARIO
     * (which sub00 `switch(10)` case to run): 0 = boot/helipad (→ sub15 → sub02
     * cinematic), 11 = outdoor entrance via door 0 (→ sub14, the courtyard dialog
     * cutscene). The original sets this from the door's entry event at room load.
     * 2026-06-09 byte-true: Switch reads g_scd.work_vars[] (global DAT_800b0fd0),
     * not per-thread locals[] — so stamp the scenario into the global work-var
     * here (the memset above zeroed it = default scenario 0 at boot). */
    g_scd.work_vars[10] = (int16_t)entry_scenario;
    /* Der INIT-Lauf: das Original ruft hier den Dispatcher FUN_8003f0a0 DIREKT (@0x8003f018),
     * OHNE den sub01-Reseed — sonst wuerde das gerade in Slot 1 geladene sub00 verdraengt, bevor es
     * sein erstes Opcode ausfuehrt (ROOM10D0 haelt seine Sce_em_set-Spawns dort). Der Reseed gehoert
     * ausschliesslich zum Gameplay-Einstieg FUN_8003f038 (@0x8003f088). */
    scd_vm_set_room_init(1);
    scd_vm_tick();                      /* run init+main once (main00 regs; sub00→scenario sub) */
    scd_vm_set_room_init(0);
    /* Der frueher hier stehende EINMAL-Start von sub01 (Slot 2) entfaellt: sub01 wird jetzt byte-true
     * in JEDEM Gameplay-Frame in Slot 1 neu geseedet (scd_vm_tick, FUN_8003f038 @0x8003f064-84).
     * Der Einmal-Start war die Ursache dafuer, dass ROOM1040s Schalter beim Druecken nichts tat und
     * ROOM1030s Cutscene erst im naechsten Tuer-Frame feuerte. */
    re15_aot_settle_at(player_x, player_z);
}
