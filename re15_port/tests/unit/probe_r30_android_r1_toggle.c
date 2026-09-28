/* probe_r30_android_r1_toggle.c - MESSSONDE + RIEGEL (Runde 30, Thema C).
 *
 * BAU (Runde 30): die Umschalt-Logik liegt jetzt in platform/pc/src/touch_r1_toggle_pc.h
 * (dieselbe Datei, die touch_overlay_pc.c einbindet), der Phasen-Riegel ist die Engine-Funktion
 * re15_player_pad_live() (engine/src/pad_phase_common.c), die main.c vor re15_input_tick
 * abfragt. Die Sonde prueft damit GENAU den ausgelieferten Code; add_test
 * unit_r30_android_r1_toggle (tests/unit/probes/r30_android-r1-toggle.cmake).
 *
 * Auftrag (Nutzer): "Beim Android Port ... R1 nicht gedrueckt gehalten werden muss, um die
 * Waffe zu heben, sondern das man einmal kurz R1 andrueckt, dann bleibt die Kampfpose
 * vorbereitet, und drueckt man erneut R1 geht die Kampfpose wieder zurueck."
 *
 * Die Sonde aendert NICHTS an engine/ oder platform/. Sie faehrt den ECHTEN Spielschritt
 * (re15_game_step) mit drei Arten, das R1-Bit zu bilden:
 *   HALTEN  = R1-Bit folgt dem Finger            (heutiger Stand, Referenz)
 *   RASTE   = Umschalter MIT Phasen-Gate         (der Vorschlag, touch_r1_toggle.h)
 *   NAIV    = Umschalter OHNE Phasen-Gate        (Gegenprobe: was ohne Gate klemmt)
 * und misst je Fall Bildzahlen, Zielphase, Magazin, Menue-/Kistenzustand.
 *
 * Original-Belege, auf die sich die Faelle stuetzen (selbst disassembliert, info/Re1.5/PSX.EXE):
 *   Zieleintritt  lw v0,0(s0) @0x80031ff4 (s0 = 0x800ac768 @0x80031f40/44), andi 0x100 @0x80031ffc,
 *                 sw 0x701,0x800aca58 @0x80032020
 *   Senken        lw v1,0x800ac768 @0x800331e4, andi 0x100 @0x800331ec, sh 3,0x800aca5a @0x80033200
 *   Feuern        lw v0,0x800ac768 @0x80033300, andi 0x40 @0x80033308
 *   Menue-Ende    sb zero,0x800aca58 @0x8001cbdc (Transitions-FSM Zustand 3, erreicht ueber
 *                 j 0x8001cbac @0x8001cb70 nach dem Status-Task)
 *   Cutscene      sb v1(=4),4(v0) @0x80041bb0 (Plc_motion schreibt cmd 4)
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_inv_screen.h"
#include "re15_itembox.h"
#include "re15_item_modal.h"
#include "re15_item_discard.h"
#include "re15_msg.h"
#include "re15_menu.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "touch_r1_toggle_pc.h"   /* platform/pc/src/ - der ausgelieferte Umschalter */

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_lens(const uint16_t *fcs, int n);
extern int  re15_player_aim_phase_debug(void);
extern int  re15_player_aim_melee_dbg(void);
extern int  re15_player_aim_ready(void);
extern int  re15_player_aim_active(void);
extern int  re15_player_aim_clip(void);
extern void re15_player_room_entry_pose(void);
extern void re15_fade_tick(void);      /* fade_common.c (FUN_80021880) - im Spiel tickt ihn der
                                        * Renderer je Bild (render_pc.c); ohne ihn endet keine
                                        * Menue-Blende und das Inventar geht nie auf */
extern void re15_fade_kill(int ch);

enum { M_HALTEN = 0, M_RASTE = 1, M_NAIV = 2 };
static const char *MNAME[3] = { "HALTEN", "RASTE ", "NAIV  " };

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static re15_r1_toggle_t   s_tg;
static uint16_t           s_prev_pad;
static int                s_bilder;
static int                s_fehler;

/* PL00W03 (Browning) Cliplaengen wie probe_m93r_nachladen.c (PLW-EDD selbst geparst dort) */
static const uint16_t FC_W03[14] = {22,16,52,1,50,30,10,23,1,24,1,24,1,32};

#define SOLL(cond, ...) do { if (!(cond)) { s_fehler++; printf("  RIEGEL-ROT: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* DER PHASEN-RIEGEL = die Engine-Funktion re15_player_pad_live() (pad_phase_common.c), die
 * main.c vor re15_input_tick abfragt. Jede Zeile dort = eine Phase, in der der
 * Spieler-Dispatcher das Pad NICHT als Spielereingabe liest oder R1 eine andere Bedeutung hat. */
static int phase_live(void) { return re15_player_pad_live(); }

static int phase(void)  { return re15_player_aim_phase_debug() & 0x0f; }
static int recoil(void) { return (re15_player_aim_phase_debug() & 0x10) ? 1 : 0; }
static int mag_qty(void)
{
    int s = re15_inv_equipped_slot();
    if (s < 0 || s >= RE15_INV_MAX_SLOTS) return -1;
    return (int)g_inv.slots[s].qty;
}

/* EIN Bild. finger_r1 = liegt ein Finger auf dem R1-Knopf; rest = uebrige Pad-Pegel. */
static uint16_t bild(int modus, int finger_r1, uint16_t rest)
{
    int r1;
    if (modus == M_HALTEN)     r1 = finger_r1 ? 1 : 0;
    else if (modus == M_RASTE) r1 = re15_r1_toggle_step(&s_tg, finger_r1, phase_live());
    else                       r1 = re15_r1_toggle_step(&s_tg, finger_r1, 1);
    uint16_t cur  = (uint16_t)(rest | (r1 ? RE15_PAD_BIT_R1 : 0));
    uint16_t edge = (uint16_t)(cur & ~s_prev_pad);
    s_prev_pad = cur;
    {
        const unsigned char *raw; int len, id;
        re15_msg_tick(&raw, &len, &id);
    }
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    re15_fade_tick();
    s_bilder++;
    return cur;
}

static void bringup(int waffe, int mag)
{
    /* Menue/Kiste eines vorigen Falls sicher schliessen (sonst bleibt der Spielschritt
     * eingefroren und der naechste Fall misst NICHTS). */
    for (int i = 0; i < 300 && re15_menu_gameplay_frozen() && !re15_menu_is_open(); i++) {
        s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
        re15_game_step(&s_ctx); re15_fade_tick();
    }
    if (re15_menu_is_open()) re15_menu_toggle();
    re15_fade_kill(0);
    re15_itembox_reset();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(1);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1140;
    g_re15_pauseflags = 0;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    for (int i = 0; i < 60; i++) scd_vm_tick();
    for (int s = 1; s < RE15_ACTOR_MAX; s++) g_actors[s].active = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = 30000; pl->z = 30000;
    re15_collision_set_band(0);
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    g_scd.message_display_frames = 0; g_scd.message_query = 0;
    g_re15_pauseflags = 0;

    re15_inv_init();
    g_inv.slots[0].id = 0x01; g_inv.slots[0].qty = 0;             /* KNIFE   */
    g_inv.slots[1].id = 0x03; g_inv.slots[1].qty = (uint8_t)mag;  /* BROWNING */
    g_inv.slots[2].id = 0x15; g_inv.slots[2].qty = 50;            /* H.GUN BULLETS */
    re15_inv_set_prev_equip_slot(0x80);
    re15_player_set_aim_clip_lens(FC_W03, 14);
    re15_player_set_equipped_weapon(waffe);
    if (waffe == 3) re15_inv_set_equipped_slot(1);
    else            re15_inv_set_equipped_slot(0);

    re15_r1_toggle_reset(&s_tg);
    s_prev_pad = 0; s_bilder = 0;
    /* drei Leerbilder: Eintritts-Pose (cmd-0-Latch, zwei Bilder) abarbeiten */
    for (int i = 0; i < 3; i++) bild(M_HALTEN, 0, 0);
    s_bilder = 0;
}

/* R1 antippen (2 Bilder Finger), danach Finger weg; zaehlt bis zielbereit. */
static int tipp_bis_bereit(int modus, int max)
{
    int n = 0;
    for (; n < max && !re15_player_aim_ready(); n++)
        bild(modus, (modus == M_HALTEN) ? 1 : (n < 2), 0);
    return n;
}

/* ============================================================ TEIL I: reine Logik */
static void teil1_logik(void)
{
    printf("\n===== TEIL I  REINE UMSCHALT-LOGIK (touch_r1_toggle_pc.h) =====\n");
    static const struct { const char *name; int n; int finger[12]; int live[12]; int soll[12]; } T[] = {
        { "Tipp rastet, zweiter Tipp loest", 8,
          {1,0,0,0,1,0,0,0}, {1,1,1,1,1,1,1,1}, {1,1,1,1,0,0,0,0} },
        { "langes Halten rastet EINMAL (keine Wiederholung)", 8,
          {1,1,1,1,1,1,0,0}, {1,1,1,1,1,1,1,1}, {1,1,1,1,1,1,1,1} },
        { "Blitz-Tipp (1 Tick) zaehlt", 6,
          {1,0,1,0,1,0}, {1,1,1,1,1,1}, {1,1,0,0,1,1} },
        { "Phase aus: Halte-Taste, Raste faellt", 8,
          {1,0,0,0,1,1,0,0}, {1,1,0,0,0,0,0,0}, {1,1,0,0,1,1,0,0} },
        { "Phase kehrt zurueck: Raste bleibt UNTEN", 8,
          {1,0,0,0,0,0,0,0}, {1,1,0,0,1,1,1,1}, {1,1,0,0,0,0,0,0} },
        { "Finger liegt beim Phasenbeginn auf: KEINE Raste", 8,
          {1,1,1,1,0,0,1,0}, {0,0,1,1,1,1,1,1}, {1,1,0,0,0,0,1,1} },
        { "Raste an, Phase 1 Tick aus, Finger nie wieder: bleibt aus", 6,
          {1,0,0,0,0,0}, {1,1,1,0,1,1}, {1,1,1,0,0,0} },
    };
    int nT = (int)(sizeof T / sizeof T[0]);
    for (int t = 0; t < nT; t++) {
        re15_r1_toggle_t g; re15_r1_toggle_reset(&g);
        int ok = 1; char ist[32] = {0}, soll[32] = {0};
        for (int i = 0; i < T[t].n; i++) {
            int r = re15_r1_toggle_step(&g, T[t].finger[i], T[t].live[i]);
            ist[i] = (char)('0' + r); soll[i] = (char)('0' + T[t].soll[i]);
            if (r != T[t].soll[i]) ok = 0;
        }
        printf("  [%d] %-58s ist=%s soll=%s %s\n", t, T[t].name, ist, soll, ok ? "ok" : "FEHLER");
        SOLL(ok, "Logikfall %d (%s)", t, T[t].name);
    }
}

/* ============================================================ TEIL II: Engine */

/* Fall A: heben, stehen lassen, feuern, senken - HALTEN gegen RASTE. */
static void fall_A(int modus, int *o_heben, int *o_senken, int *o_schuss)
{
    bringup(3, 15);
    printf("\n--- Fall A [%s] Browning, Magazin 15 ---\n", MNAME[modus]);
    int heben = tipp_bis_bereit(modus, 90);
    printf("  zielbereit nach %d Bildern (Phase %d, Clip %d)\n", heben, phase(), re15_player_aim_clip());
    int steht = 0;
    for (int i = 0; i < 30; i++) {                     /* Finger ist WEG (RASTE) bzw. haelt (HALTEN) */
        bild(modus, modus == M_HALTEN, 0);
        if (re15_player_aim_ready()) steht++;
    }
    printf("  30 Bilder ohne neuen Tipp: %d davon zielbereit\n", steht);
    int m0 = mag_qty(), schuss = 0;
    for (int i = 0; i < 40; i++) {                     /* Viereck 3 Bilder, dann los */
        bild(modus, modus == M_HALTEN, (uint16_t)((i < 3) ? RE15_PAD_BIT_SQUARE : 0));
        if (recoil() && !schuss) schuss = s_bilder;
    }
    printf("  Viereck (3 Bilder): Rueckstoss ab Bild %d, Magazin %d -> %d\n", schuss, m0, mag_qty());
    int start = s_bilder, senken = -1;
    for (int i = 0; i < 60; i++) {                     /* zweiter Tipp bzw. Loslassen */
        bild(modus, (modus == M_HALTEN) ? 0 : (i < 2), 0);
        if (phase() == 0 && senken < 0) senken = s_bilder - start;
    }
    printf("  zweiter Tipp/Loslassen: Zielphase 0 nach %d Bildern\n", senken);
    *o_heben = heben; *o_senken = senken; *o_schuss = m0 - mag_qty();
    if (modus == M_RASTE) {
        SOLL(steht == 30, "A: die Raste haelt die Pose nicht (nur %d von 30 Bildern)", steht);
        SOLL(m0 - mag_qty() == 1, "A: Viereck feuert nicht genau einmal (Magazin %d -> %d)", m0, mag_qty());
        SOLL(senken > 0, "A: zweiter Tipp senkt nicht");
    }
}

/* Fall B: Inventar. */
static void fall_B(int modus)
{
    bringup(3, 15);
    printf("\n--- Fall B [%s] Inventar mit gehobener Waffe ---\n", MNAME[modus]);
    tipp_bis_bereit(modus, 90);
    bild(modus, modus == M_HALTEN, RE15_PAD_BIT_START);
    int auf = -1, r1_im_menue = 0, n_menue = 0;
    for (int i = 0; i < 40; i++) {
        uint16_t cur = bild(modus, modus == M_HALTEN, 0);
        if (re15_menu_is_open()) {
            if (auf < 0) auf = i + 1;
            n_menue++;
            if (cur & RE15_PAD_BIT_R1) r1_im_menue++;
        }
    }
    printf("  Menue offen nach %d Bildern; R1-Bit im Pad-Wort in %d von %d Menue-Bildern; substate=%d tab=%d\n",
           auf, r1_im_menue, n_menue, re15_menu_substate(), (int)g_inv_screen.tab);
    /* EIN Tipp auf R1 im Tab-Schirm = FILE-Sprung (raw 0x8 @0x80049834) */
    int sub0 = re15_menu_substate();
    bild(modus, (modus == M_HALTEN) ? 0 : 1, 0);      /* HALTEN: erst loslassen ...        */
    bild(modus, (modus == M_HALTEN) ? 1 : 0, 0);      /* ... dann druecken (eine Flanke)   */
    for (int i = 0; i < 3; i++) bild(modus, 0, 0);
    printf("  ein R1-Tipp im Tab-Schirm: substate %d -> %d (2 = FILE)\n", sub0, re15_menu_substate());
    if (modus == M_RASTE) {
        SOLL(auf > 0, "B: Menue oeffnet nicht");
        SOLL(r1_im_menue == 0, "B: gerastetes R1 leckt ins Menue (%d Bilder)", r1_im_menue);
        SOLL(re15_menu_substate() == 2, "B: R1-Tipp im Menue springt nicht nach FILE (substate %d)", re15_menu_substate());
    }
}

/* Fall C: Kiste (R1 = +5 blaettern, mit Wiederholung). */
static void fall_C(int modus)
{
    bringup(3, 15);
    printf("\n--- Fall C [%s] Item-Kiste ---\n", MNAME[modus]);
    tipp_bis_bereit(modus, 90);
    re15_menu_toggle_box();
    bild(modus, modus == M_HALTEN, 0);
    bild(modus, modus == M_HALTEN, RE15_PAD_BIT_SQUARE);        /* -> Kistenseite */
    for (int i = 0; i < 3; i++) bild(modus, modus == M_HALTEN, 0);
    int s0 = re15_itembox_screen_scroll();
    for (int i = 0; i < 60; i++) bild(modus, modus == M_HALTEN, 0);
    int s1 = re15_itembox_screen_scroll();
    printf("  60 Bilder Kistenseite ohne Tipp: scroll %d -> %d (state=%d)\n", s0, s1, re15_itembox_screen_state());
    /* zwei Tipps im Abstand von 30 Bildern */
    int nach[2];
    for (int t = 0; t < 2; t++) {
        if (modus == M_HALTEN) { bild(modus, 0, 0); bild(modus, 1, 0); bild(modus, 1, 0); }
        else                   { bild(modus, 1, 0); bild(modus, 1, 0); }
        for (int i = 0; i < 30; i++) bild(modus, 0, 0);
        nach[t] = re15_itembox_screen_scroll();
    }
    printf("  Tipp 1 (+30 Bilder): scroll=%d ; Tipp 2 (+30 Bilder): scroll=%d   (je Tipp erwartet +5)\n", nach[0], nach[1]);
    if (modus == M_RASTE) {
        SOLL(s1 == s0, "C: Kiste blaettert ohne Tipp");
        SOLL(nach[0] == ((s1 + 5) & 63) && nach[1] == ((s1 + 10) & 63),
             "C: R1-Tipp blaettert nicht genau +5 (%d, %d)", nach[0], nach[1]);
    }
}

/* Fall D: Waffenwechsel im Inventar bei gehobener Waffe. */
static void fall_D(int modus)
{
    bringup(1, 15);                                   /* MESSER angelegt */
    printf("\n--- Fall D [%s] Messer gehoben -> Inventar -> Browning anlegen -> zu ---\n", MNAME[modus]);
    int heben = tipp_bis_bereit(modus, 90);
    printf("  zielbereit (Messer) nach %d Bildern, melee_latch=%d\n", heben, re15_player_aim_melee_dbg());
    bild(modus, modus == M_HALTEN, RE15_PAD_BIT_START);
    for (int i = 0; i < 30 && !re15_menu_is_open(); i++) bild(modus, modus == M_HALTEN, 0);
    for (int i = 0; i < 10; i++) bild(modus, modus == M_HALTEN, 0);
    printf("  im Menue: offen=%d Zielphase=%d (eingefroren)\n", re15_menu_is_open(), phase());
    re15_inv_set_equipped_slot(1);                    /* Cursor: Browning (DAT_800b25c8 := 1) */
    bild(modus, modus == M_HALTEN, RE15_PAD_BIT_START);
    int zu = 0;
    for (; zu < 200 && re15_menu_gameplay_frozen(); zu++) bild(modus, modus == M_HALTEN, 0);
    printf("  Menue zu nach %d Bildern: Waffe=%d Zielphase=%d melee_latch=%d Clip=%d\n",
           zu, re15_player_equipped_weapon(), phase(), re15_player_aim_melee_dbg(), re15_player_aim_clip());
    int weg = -1, st = s_bilder;
    for (int i = 0; i < 40; i++) {
        bild(modus, modus == M_HALTEN, 0);
        if (phase() == 0 && weg < 0) weg = s_bilder - st;
    }
    printf("  40 Bilder danach: Zielphase=%d (Phase 0 erreicht nach %d Bildern), melee_latch=%d\n",
           phase(), weg, re15_player_aim_melee_dbg());
    if (modus != M_HALTEN) {
        /* neuer Tipp -> frisches Heben mit der neuen Waffe */
        int n = 0;
        if (phase() == 0) {
            for (; n < 90 && !re15_player_aim_ready(); n++) bild(modus, n < 2, 0);
            printf("  neuer Tipp: zielbereit nach %d Bildern, melee_latch=%d Clip=%d\n",
                   n, re15_player_aim_melee_dbg(), re15_player_aim_clip());
        }
    }
    int m0 = mag_qty();
    for (int i = 0; i < 40; i++)
        bild(modus, modus == M_HALTEN, (uint16_t)((i < 3) ? RE15_PAD_BIT_SQUARE : 0));
    printf("  Viereck: Magazin %d -> %d, melee_latch=%d\n", m0, mag_qty(), re15_player_aim_melee_dbg());
    if (modus == M_RASTE) {
        SOLL(weg > 0, "D: nach dem Menue bleibt die Zielphase stehen (Phase %d)", phase());
        SOLL(re15_player_aim_melee_dbg() == 0, "D: Messer-Klasse klebt nach dem Waffenwechsel");
        SOLL(m0 - mag_qty() == 1, "D: Browning feuert nach dem Wechsel nicht (Magazin %d -> %d)", m0, mag_qty());
    }
}

/* Faelle E/F/G/I: Phase wird dem Spieler entzogen. art: 'E' Raumblende+cmd-Reset,
 * 'F' Cutscene, 'G' Text-Freeze, 'I' Tod. */
static void fall_entzug(int modus, char art)
{
    bringup(3, 15);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const char *nm = art == 'E' ? "Raum-/Tuerblende (pauseflags 0xff000000 + cmd-Reset)"
                   : art == 'F' ? "Cutscene (player_mode 2)"
                   : art == 'G' ? "Text-Freeze (pauseflags PLAYER|PAD)"
                                : "Tod (hp < 0)";
    printf("\n--- Fall %c [%s] %s ---\n", art, MNAME[modus], nm);
    tipp_bis_bereit(modus, 90);
    for (int i = 0; i < 5; i++) bild(modus, modus == M_HALTEN, 0);
    printf("  vorher: Zielphase=%d Raste=%d\n", phase(), (int)s_tg.latched);

    int r1_bilder = 0, aktiv_bilder = 0, dauer = 40;
    for (int i = 0; i < dauer; i++) {
        if (art == 'E') {
            g_re15_pauseflags |= 0xff000000u;                 /* @0x8001cc54-6c */
            if (i == 3) { re15_player_room_entry_pose(); re15_player_cmd_reset(); }  /* room_common.c:288/308 */
        } else if (art == 'F') {
            g_scd.player_mode = 2;
        } else if (art == 'G') {
            g_re15_pauseflags |= (RE15_PAUSE_PLAYER | RE15_PAUSE_PAD);
        } else {
            pl->hp = -1;
        }
        uint16_t cur = bild(modus, modus == M_HALTEN, 0);
        if (cur & RE15_PAD_BIT_R1) r1_bilder++;
        if (re15_player_aim_active()) aktiv_bilder++;
    }
    printf("  waehrend (%d Bilder): R1-Bit gesetzt in %d, Zielphase aktiv in %d, Raste am Ende=%d\n",
           dauer, r1_bilder, aktiv_bilder, (int)s_tg.latched);
    if (art == 'I') {
        if (modus == M_RASTE) SOLL(s_tg.latched == 0, "I: Raste ueberlebt den Tod");
        return;
    }
    g_re15_pauseflags = 0; g_scd.player_mode = 0;
    int weg = -1, wieder = -1, st = s_bilder;
    for (int i = 0; i < 60; i++) {
        bild(modus, modus == M_HALTEN, 0);
        if (phase() == 0 && weg < 0) weg = s_bilder - st;
        if (re15_player_aim_ready() && wieder < 0 && (weg >= 0 || art == 'E')) wieder = s_bilder - st;
    }
    printf("  danach (60 Bilder): Zielphase=%d, Phase 0 nach %d Bildern, wieder zielbereit nach %d Bildern\n",
           phase(), weg, wieder);
    if (modus == M_RASTE) {
        SOLL(s_tg.latched == 0, "%c: Raste ueberlebt den Phasenentzug", art);
        SOLL(phase() == 0, "%c: Zielphase klemmt nach dem Phasenentzug (Phase %d)", art, phase());
    }
}

/* Fall H: Treffer (Flinch) - die Raste BLEIBT, die Waffe kommt von selbst wieder hoch. */
static void fall_H(int modus)
{
    bringup(3, 15);
    printf("\n--- Fall H [%s] Treffer waehrend des Zielens (cmd 2, Clip 8) ---\n", MNAME[modus]);
    tipp_bis_bereit(modus, 90);
    for (int i = 0; i < 5; i++) bild(modus, modus == M_HALTEN, 0);
    re15_player_stagger_cmd2(0x08);
    printf("  nach dem Treffer: Zielphase=%d Raste=%d\n", phase(), (int)s_tg.latched);
    int wieder = -1, st = s_bilder;
    for (int i = 0; i < 80; i++) {
        bild(modus, modus == M_HALTEN, 0);
        if (re15_player_aim_ready() && wieder < 0) wieder = s_bilder - st;
    }
    printf("  wieder zielbereit nach %d Bildern (Raste=%d)\n", wieder, (int)s_tg.latched);
    if (modus == M_RASTE) {
        SOLL(s_tg.latched == 1, "H: Raste faellt beim Treffer");
        SOLL(wieder > 0, "H: Waffe kommt nach dem Treffer nicht wieder hoch");
    }
}

int main(void)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("RDT fehlt: %s\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    teil1_logik();

    printf("\n===== TEIL II  ECHTER SPIELSCHRITT (ROOM1140, Gegner-KI pausiert) =====\n");
    int hA[3], sA[3], mA[3];
    for (int m = 0; m < 3; m++) fall_A(m, &hA[m], &sA[m], &mA[m]);
    SOLL(hA[M_RASTE] == hA[M_HALTEN], "A: Heben dauert mit Raste %d statt %d Bilder", hA[M_RASTE], hA[M_HALTEN]);
    SOLL(sA[M_RASTE] == sA[M_HALTEN], "A: Senken dauert mit Raste %d statt %d Bilder", sA[M_RASTE], sA[M_HALTEN]);
    for (int m = 0; m < 3; m++) fall_B(m);
    for (int m = 0; m < 3; m++) fall_C(m);
    for (int m = 0; m < 3; m++) fall_D(m);
    { const char arten[4] = { 'E', 'F', 'G', 'I' };
      for (int a = 0; a < 4; a++) for (int m = 0; m < 3; m++) fall_entzug(m, arten[a]); }
    for (int m = 0; m < 3; m++) fall_H(m);

    printf("\n================ ZUSAMMENFASSUNG ================\n");
    printf("Fall A  Heben/Senken/Schuss  HALTEN=%d/%d/%d  RASTE=%d/%d/%d  NAIV=%d/%d/%d\n",
           hA[0], sA[0], mA[0], hA[1], sA[1], mA[1], hA[2], sA[2], mA[2]);
    if (s_fehler) { printf("RIEGEL-ROT: %d Abweichung(en)\n", s_fehler); return 1; }
    printf("RIEGEL-GRUEN: Umschalt-Logik + Phasen-Riegel erfuellen alle Sollwerte\n");
    return 0;
}
