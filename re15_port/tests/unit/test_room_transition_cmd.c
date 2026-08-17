/*
 * test_room_transition_cmd.c — Fix-Runde Cluster 4 (2026-08-17).
 *
 * Zwei zusammenhaengende Pins, beide Sollwerte aus selbst nachdisassemblierten
 * Instruktionen der ausgelieferten PSX.EXE (Adresse je Assertion zitiert):
 *
 *  (A) RAUM-TRANSITIONS-PRAESENTATION (Nutzer-Report "Elliots Laufanimation am
 *      Anfang im Intro ist noch falsch", Teil a: der HARTE SCHNITT).
 *      Transitions-FSM FUN_8001c958, Zustands-Byte DAT_800b5359, Tabelle
 *      @0x8001069c = {0x8001c9c8, 0x8001ca98, 0x8001cbb8, 0x8001cc34, 0x8001cc70}:
 *        - State-3-Rumpf @0x8001cbb8-cc28: FUN_800217b0(0x200,-6144,7,0)
 *          @0x8001cc00 + FUN_800216ec @0x8001cc18 = Einblendung, level 0x7fff;
 *          `sb zero,0x800aca58` @0x8001cbdc = Spieler-Kommandowort;
 *          `sw 7,g_pauseflags` @0x8001cbe4.
 *        - State 4 @0x8001cc34-6c: `g_pauseflags |= 0xff000000` (@0x8001cc5c
 *          `lui v0,0xff00` + @0x8001cc6c `sw`) -> die LOGIK STEHT.
 *        - State 5 @0x8001cc70-94: Freigabe erst wenn FUN_8002178c(0) meldet,
 *          dass die Blende durch ist; dann `sb zero,DAT_800b5359` @0x8001cc8c +
 *          `sw DAT_8008f628,g_pauseflags` @0x8001cc94.
 *      Dauer AUS DEN BYTES: level 0x7fff, Schritt -0x1800 -> 6 gezeichnete Frames.
 *
 *  (B) DAS ECHTE SPIELER-KOMMANDOWORT (offener Punkt der Freeze-Runde).
 *      Der Dispatcher FUN_80031c44 indiziert die Tabelle @0x80073f90 mit
 *      DAT_800aca58 = Spieler+0x4 (@0x80031c8c lbu / @0x80031c94 sll /
 *      @0x80031ca4 addiu at,0x80073f90 / @0x80031cac lw / @0x80031cb4 jalr).
 *      [1] = 0x80031de8 = der Auto-Look-Prolog, [4] = 0x80030660 = Plc-Executor.
 *      Plc_motion @0x80041bb0 und Plc_dest @0x80041c14 schreiben +0x4 = 4,
 *      Plc_ret @0x80041f90 schreibt +0x4 = 1 zurueck (und raeumt +0x1b8 NICHT
 *      auf — @0x80041f88-9c enthaelt keinen Store auf 440). Deshalb muss der
 *      Prolog nach Plc_ret wieder jeden Frame `ori 0x12` auf +0x1b8 fahren.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_fade.h"
#include "re15_game_step.h"

static int g_fail = 0;
#define CHECK(cond, ...) do { if (cond) { printf("  PASS: "); }                 \
                              else { printf("  FAIL: "); g_fail = 1; }          \
                              printf(__VA_ARGS__); printf("\n"); } while (0)

/* ======================================================================== */
/* (A) Transitions-Praesentation                                            */
/* ======================================================================== */

static void reset_world(void)
{
    scd_vm_init();          /* ACHTUNG: raeumt auch Actors/Flags/Fade — nur EINMAL pro Szenario */
    re15_actor_init();
    re15_fade_init();
    re15_pauseflags_clear();
    re15_room_transition_reset();
}

static void test_transition(void)
{
    printf("== (A) Raum-Transition: Blende + Logik-Freeze (FUN_8001c958 State 3/4/5)\n");
    reset_world();

    /* Vorbedingung: ein noch offener Text-Freeze. Der Raumwechsel loescht das Wort
     * KOMPLETT (@0x8001ca44 / @0x8001caec `sw zero,0x800aca40`) — genau der Fall
     * "kein Bit bleibt haengen". */
    re15_pauseflags_open(0xffff0000u);
    CHECK(g_re15_pauseflags == 0xffff0000u, "Vorbedingung: Text-Freeze offen (0x%08lx)",
          (unsigned long)g_re15_pauseflags);

    g_actors[RE15_ACTOR_SLOT_PLAYER].state = 4;     /* so, als endete die Szene skriptgetrieben */

    re15_room_transition_present();

    CHECK(g_re15_pauseflags == 7u,
          "State-3-Rumpf setzt g_pauseflags = 7 (@0x8001cbc4 ori 0x7 + @0x8001cbe4 sw), "
          "der Text-Freeze ist weg (0x%08lx)", (unsigned long)g_re15_pauseflags);
    CHECK(g_actors[RE15_ACTOR_SLOT_PLAYER].state == 1,
          "Spieler-Kommandowort zurueck auf 1 (@0x8001cbdc sb zero -> cmd-0-INIT "
          "@0x8003192c sw 1), ist %d", (int)g_actors[RE15_ACTOR_SLOT_PLAYER].state);
    CHECK(g_fade_ch[0].level == 0x7FFFu && g_fade_ch[0].step == (int16_t)-0x1800
          && g_fade_ch[0].abr == 2,
          "Einblendung armiert: level 0x%04x step %d abr %d "
          "(FUN_800217b0(0x200,-6144,7,0) @0x8001cc00 + FUN_800216ec @0x8001cc18)",
          (unsigned)g_fade_ch[0].level, (int)g_fade_ch[0].step, (int)g_fade_ch[0].abr);
    CHECK(re15_room_transition_active(),
          "DAT_800b5359 = 4 (@0x8001cc20-28) — die Transition laeuft");

    /* Jetzt der Frame-Takt: erst die FSM (@0x8001c994, VOR den Subsystemen
     * @0x8001cdec), dann — im echten Loop — die Subsysteme, dann die Blende
     * (FUN_80021880). Hier: FSM-Tick + re15_fade_tick pro Frame. */
    const uint32_t all_gates = RE15_PAUSE_PLAYER | RE15_PAUSE_AI | RE15_PAUSE_ACTION
                             | RE15_PAUSE_SCD    | RE15_PAUSE_PAD;
    int frozen_frames = 0;
    int gates_ok      = 1;
    for (int f = 0; f < 8; f++) {
        int running = re15_room_transition_tick();
        if (running) {
            frozen_frames++;
            if ((g_re15_pauseflags & all_gates) != all_gates) gates_ok = 0;
        }
        re15_fade_tick();
    }
    /* level 0x7fff, Schritt -0x1800: 0x7fff,0x67ff,0x4fff,0x37ff,0x1fff,0x07ff,
     * dann Unterlauf -> 0xefff (Bit15 = fertig). 6 gezeichnete Frames, die
     * Freigabe faellt in den 7. Poll. State 4 + 5 = 6 eingefrorene Ticks. */
    CHECK(frozen_frames == 6,
          "Freeze-Dauer = 6 Ticks (aus den Bytes gerechnet: level 0x7fff / Schritt "
          "-0x1800 @0x8001cbbc), gemessen %d", frozen_frames);
    CHECK(gates_ok,
          "waehrend des Freezes stehen ALLE fuenf Gates: Spieler 0x80000000 "
          "(@0x80031c78), KI 0x20000000 (@0x8010043c), Anim 0x10000000 (@0x80019e40), "
          "SCD 0x02000000 (@0x8003f04c), Pad 0x01000000 (@0x80030514)");
    CHECK(!re15_room_transition_active() && g_re15_pauseflags == 7u,
          "Freigabe stellt DAT_8008f628 wieder her (@0x8001cc84/@0x8001cc94): "
          "g_pauseflags = 0x%08lx, kein Bit bleibt haengen",
          (unsigned long)g_re15_pauseflags);

    /* Regression: ein zweiter Tick nach der Freigabe darf NICHTS mehr aendern
     * (DAT_800b5359 == 0 faellt im Original in den gemeinsamen Rumpf @0x8001cc98). */
    re15_room_transition_tick();
    CHECK(g_re15_pauseflags == 7u, "Tick ohne laufende Transition ist ein No-Op");
}

/* ======================================================================== */
/* (B) Kommandowort + Auto-Look-Prolog                                       */
/* ======================================================================== */

/* Kleinste SCD-Programme fuer die drei Opcodes. Groessen aus s_opcode_sizes:
 *   0x3F Plc_motion 4 B, 0x40 Plc_dest 8 B, 0x42 Plc_ret 1 B, 0x01 Evt_end 2 B. */
static const uint8_t bc_plc_motion[] = { 0x3F, 0x00, 0x0F, 0x00,  0x01, 0x00 };
static const uint8_t bc_plc_dest_run[] = { 0x40, 0x00, 0x05, 0x21,
                                           0x2b, 0xf7, 0x9f, 0x16,  0x01, 0x00 };
static const uint8_t bc_plc_ret[]    = { 0x42,  0x01, 0x00 };

/* Einen Opcode auf Slot 0 ausfuehren, OHNE die VM neu zu initialisieren (scd_vm_init
 * raeumt Actors + Flags mit — das wuerde die zu messende Neck-/Kommando-Kette wischen). */
static void run_op(const uint8_t *bc)
{
    scd_thread_start(0, bc);
    g_scd.threads[0].work_slot = RE15_ACTOR_SLOT_PLAYER;   /* = Work_set(1,0) */
    g_scd.work_slot            = RE15_ACTOR_SLOT_PLAYER;
    scd_vm_tick();
}

static void step_once(void)
{
    re15_game_ctx_t ctx;
    memset(&ctx, 0, sizeof ctx);
    ctx.rdt_ok = 0;          /* kein Raum: alle rdt_ok-Zweige fallen aus, der
                              * cmd-0/1-Zweig (mit dem Prolog) bleibt uebrig */
    re15_game_step(&ctx);
}

static void test_cmd_word(void)
{
    printf("== (B) Spieler-Kommandowort +0x4 (DAT_800aca58) und der Prolog @0x80031de8\n");

    /* --- Plc_motion setzt cmd 4 (@0x80041ba4 ori 0x4 + @0x80041bb0 sb v1,4(v0)) --- */
    reset_world();
    g_actors[RE15_ACTOR_SLOT_PLAYER].state = 1;
    run_op(bc_plc_motion);
    CHECK(g_actors[RE15_ACTOR_SLOT_PLAYER].state == 4,
          "Plc_motion(Work=Spieler) -> +0x4 = 4 (@0x80041bb0), ist %d",
          (int)g_actors[RE15_ACTOR_SLOT_PLAYER].state);

    /* --- cmd 4: der Prolog laeuft NICHT, ein Skript-Neck-Modus bleibt unangetastet.
     * Sweep-Fall aus ROOM1170 sub02 (Plc_neck(4,…) -> +0x1b8 = 0x80|0x58 = 0xd8,
     * @0x80041ea8 + @0x80041f10). Wuerde der Prolog laufen, kaeme mit `ori 0x12`
     * (@0x80031e04) das Bit 0x02 dazu und ueberschriebe das Sweep-Ziel
     * @0x80037534-48. */
    g_actors[RE15_ACTOR_SLOT_PLAYER].neck_flags = 0xd8;
    step_once();
    CHECK(g_actors[RE15_ACTOR_SLOT_PLAYER].neck_flags == 0xd8,
          "cmd 4 -> Tabelle [4] = 0x80030660, KEIN Prolog: neck_flags bleibt 0x%02x "
          "(Sweep-Phase geschuetzt)", (unsigned)g_actors[RE15_ACTOR_SLOT_PLAYER].neck_flags);

    /* --- Plc_ret fuehrt auf cmd 1 zurueck (@0x80041f8c ori 0x1 + @0x80041f90 sb) --- */
    run_op(bc_plc_ret);
    CHECK(g_actors[RE15_ACTOR_SLOT_PLAYER].state == 1,
          "Plc_ret -> +0x4 = 1 (@0x80041f90), ist %d",
          (int)g_actors[RE15_ACTOR_SLOT_PLAYER].state);
    CHECK(g_actors[RE15_ACTOR_SLOT_PLAYER].neck_flags == 0xd8,
          "Plc_ret raeumt +0x1b8 NICHT auf (@0x80041f88-9c hat keinen Store auf 440): "
          "neck_flags noch 0x%02x", (unsigned)g_actors[RE15_ACTOR_SLOT_PLAYER].neck_flags);

    /* --- ...und GENAU DESHALB muss der Prolog ab jetzt jeden Frame das Release-Bit
     * nachfuehren: `ori v0,v0,0x12` @0x80031e04 + `sb v0,0(s0)` @0x80031e08.
     * DAS ist die Restdivergenz, die den Kopf bisher dauerhaft gepinnt liess. */
    step_once();
    CHECK((g_actors[RE15_ACTOR_SLOT_PLAYER].neck_flags & 0x12) == 0x12,
          "cmd 1 -> Tabelle [1] = 0x80031de8: Prolog fuehrt `ori 0x12` nach "
          "(@0x80031e04-08), neck_flags = 0x%02x",
          (unsigned)g_actors[RE15_ACTOR_SLOT_PLAYER].neck_flags);

    /* --- Plc_dest (Walk-Mode 5) setzt ebenfalls cmd 4 (@0x80041c14 sb v0(=4),4(a1)) --- */
    reset_world();
    g_actors[RE15_ACTOR_SLOT_PLAYER].state = 1;
    run_op(bc_plc_dest_run);
    CHECK(g_actors[RE15_ACTOR_SLOT_PLAYER].state == 4,
          "Plc_dest(mode 5, Work=Spieler) -> +0x4 = 4 (@0x80041c14), ist %d",
          (int)g_actors[RE15_ACTOR_SLOT_PLAYER].state);
    g_actors[RE15_ACTOR_SLOT_PLAYER].neck_flags = 0x00;
    step_once();
    CHECK(g_actors[RE15_ACTOR_SLOT_PLAYER].neck_flags == 0x00,
          "script-gelaufener Spieler (cmd 4) bekommt keinen Prolog: neck_flags 0x%02x",
          (unsigned)g_actors[RE15_ACTOR_SLOT_PLAYER].neck_flags);

    /* --- Gegenprobe cmd 0 (Port-Boot-Wert, im Original der INIT-Handler
     * @0x800318f8, der sofort auf 1 schaltet, @0x8003192c): Prolog laeuft. --- */
    reset_world();
    g_actors[RE15_ACTOR_SLOT_PLAYER].state      = 0;
    g_actors[RE15_ACTOR_SLOT_PLAYER].neck_flags = 0x00;
    step_once();
    CHECK((g_actors[RE15_ACTOR_SLOT_PLAYER].neck_flags & 0x12) == 0x12,
          "cmd 0/1 (Gameplay + Montage) -> Prolog laeuft, neck_flags = 0x%02x",
          (unsigned)g_actors[RE15_ACTOR_SLOT_PLAYER].neck_flags);
}

int main(void)
{
    printf("== Fix-Runde Cluster 4 — Tuer-Transition + Spieler-Kommandowort ==\n");
    test_transition();
    test_cmd_word();
    if (g_fail) { printf("\n%s\n", "Pin(s) FEHLGESCHLAGEN"); return 1; }
    printf("\nAlle Pins OK\n");
    return 0;
}
