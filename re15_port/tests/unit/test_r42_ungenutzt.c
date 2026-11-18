/*
 * test_r42_ungenutzt.c — Runde 42 Strang "rest", Stufe 3: ungenutzte Capcom-/80-%-Inhalte STAGE2-6.
 * Herleitung: include/re15_ungenutzt_r42.h. U-07 (Sherry 2011) prueft test_r42_story2 loch_elza, U-03 (6031) test_r42_story6 wagen6031.
 *
 * ECHTE WEGE: RDT laden, Raum wie im Spiel hochfahren (scd_room_reenter = Installer-Block), Spieler vor die Stelle, Aktion ->
 * re15_aot_scan -> scd_event_fire bzw. Aktions-Merker V[0] -> sub01 -> Evt_exec -> Programm; Bilder in der Reihenfolge von main.c
 * (scd_vm_tick, Schauspieler, Nachrichten, Kamera, re15_game_step - Geruest test_r42_story6.c).
 *   rechner  5010/5011 (U-05): Untersuchen des Speicherrechners -> erst Disc #40 (5010 msg 2 / 5011 msg 40) im Nahbild Cut 2, dann
 *            der Speicher-Text (msg 6 / 45); zweimal hintereinander (Disc: kein Aot_reset)
 *   plan     4020/4021 (U-11): Wandplan -> msg 0, dann msg 2 "You've taken the laboratory map.", Bit (9,36), Blatt 8 im Besitz;
 *            zweites Untersuchen nur msg 0
 *   schalter 3091 (U-02): Schalter-Platz Slot 5 ohne Strom -> msg 35 "There's a power supply.", dann msg 3; mit Strom nur msg 2
 *   szene    3091 (U-01): Szene sub07 -> Folge 32, 9..24, 33, 25..28, 34, 29, 30; nach den Schlaegen blicken John und Elza zur
 *            Hoftuer (-24506,1500), nicht zur Lobby-Tuer (-28481,-155)
 *   bytes    Programme gegen die Original-Saetze (nur mit Modul; die Gegenprobe auf aaea5f8c2 laeuft ohne)
 * Aufruf: test_r42_ungenutzt [rechner|plan|schalter|szene|bytes|alle]
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_fade.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_tim.h"
#include "re15_inventory.h"
#include "re15_item_modal.h"
#include "re15_engine.h"
#include "re15_elza_r36.h"
#include "re15_kleinkram_r38.h"
#include "re15_map_owned.h"
#if defined(__has_include)
#  if __has_include("re15_ungenutzt_r42.h")
#    include "re15_ungenutzt_r42.h"
#    define U42_MODUL 1
#  endif
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

void re15_actor_step_all_walkers(void);
extern uint16_t g_scd_pad_edge;
extern uint16_t g_scd_pad_held;
extern uint8_t  g_aot_action_pressed;

static int g_fail = 0;
#define PRUEF(c, ...) do { if (c) { printf("  ok    "); } else { printf("  FEHLER "); g_fail++; } printf(__VA_ARGS__); printf("\n"); } while (0)

static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;
static uint16_t           s_rdt_raum = 0;

#define PROT_N 64
static int s_prot_id[PROT_N], s_prot_cut[PROT_N], s_prot_bild[PROT_N], s_prot_n = 0;
static char s_prot_text[PROT_N][192];
static int s_offen_vorher = 0, s_id_vorher = -1, s_bild = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (n) *n = (size_t)sz;
    return b;
}

static uint8_t *s_ems = NULL; static size_t s_ems_sz = 0;
static uint8_t  s_bank_buf[3][0x60000];
static void bank_laden(uint8_t type, int buf)   /* = test_r42_story6.c bank_laden */
{
    if (!s_ems) {
        char p2[600]; snprintf(p2, sizeof p2, "%s/EMD/CDEMD0.EMS", RE15_ASSET_PSX_DIR);
        s_ems = slurp(p2, &s_ems_sz);
    }
    if (!s_ems) return;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_ems, s_ems_sz, idx, &off, &len) != 0) return;
    re15_enemy_bank_t *eb = re15_enemy_alloc(type);
    if (!eb || len > sizeof s_bank_buf[buf]) return;
    memcpy(s_bank_buf[buf], s_ems + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_bank_buf[buf], len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return;
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok   = (re15_emd_parse_loco_bank(s_bank_buf[buf], len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(s_bank_buf[buf], len, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->own_ok    = (re15_emd_parse_own_bank(s_bank_buf[buf], len, &eb->skel_own, &eb->anim_own) == 0);
}

static int rdt_laden(uint16_t room)
{
    if (s_raw && s_rdt_raum == room) return 0;
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz); s_rdt_raum = 0;
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    s_rdt_raum = room;
    return 0;
}

static void protokoll(void)
{
    const int offen = g_scd.message_active ? 1 : 0;
    const int id = offen ? (int)g_scd.message_id : -1;
    if (offen && (!s_offen_vorher || id != s_id_vorher) && s_prot_n < PROT_N) {
        const char *t = re15_msg_get_text(id);
        s_prot_id[s_prot_n] = id; s_prot_cut[s_prot_n] = g_scd.work_vars[0x0A]; s_prot_bild[s_prot_n] = s_bild;
        snprintf(s_prot_text[s_prot_n], sizeof s_prot_text[0], "%s", t ? t : "");
        s_prot_n++;
    }
    s_offen_vorher = offen; s_id_vorher = id;
}

/* Ein Spielbild in der Reihenfolge von platform/pc/main.c (= test_r42_story6.c frame_pad); bestaetigen = Taste an wartenden Texten. */
static void frame(int bestaetigen)
{
    const unsigned char *raw; int len, id;
    if (bestaetigen && g_scd.message_active && (s_bild & 7) == 7 &&
        (g_scd.message_fsm == 1 || g_scd.message_fsm == 3 || g_scd.message_fsm == 4 || g_scd.message_fsm == 7))
        g_scd_pad_edge = 0x4000;
    if (!re15_item_modal_active()) scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_letterbox_tick(re15_game_flag_get(1, 27));
    {
        int cine = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
        if (cine) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was) g_scd.letterbox_countdown = 15;
        s_cine_was = cine;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
            g_scd.player_mode = 0;
            re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        }
    }
    re15_msg_tick(&raw, &len, &id);
    g_scd_pad_edge = 0;
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = 0;
    s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
    if (re15_item_modal_active()) re15_item_modal_tick(0x4000, 0);   /* Speicher-Frage: bestaetigen */
    protokoll();
    { scd_audio_event_t e; while (scd_audio_queue_pop(&e)) {} }
    s_bild++;
}

static void prot_leeren(void) { s_prot_n = 0; s_offen_vorher = 0; s_id_vorher = -1; }

static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, const uint8_t *banken, int nb)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;   /* wie der Raumlader (room_common.c); test_r42_story3b.c raum_laden */
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_game_state_t flags_vorher = g_game;
    re15_inventory_t inv_vorher = g_inv;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    g_inv = inv_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    for (int i = 0; i < nb && i < 3; i++) bank_laden(banken[i], i);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = 0; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    re15_elza_r36_reset();
    re15_kleinkram_r38_reset();
    s_bild = 0;
    prot_leeren();
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    for (int f = 0; f < 40; f++) frame(0);
    prot_leeren();
    return 0;
}

static int laeuft(void)
{
    if (g_scd.message_active || re15_item_modal_active()) return 1;
    for (int k = SCD_EVENT_SLOT_FIRST; k <= SCD_EVENT_SLOT_LAST; k++) if (g_scd.threads[k].active) return 1;
    return 0;
}

/* Aktion an (x,z,rot); dann Bilder, bis nichts mehr laeuft. */
static void untersuchen(int32_t x, int32_t z, int rot, int max_bilder)
{
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    p->x = x; p->z = z; p->rot_y = (int16_t)rot;
    for (int i = 0; i < 10; i++) frame(0);
    prot_leeren();
    g_aot.fired_event_id_this_frame = 0;
    g_aot_action_pressed = 1;
    re15_aot_scan(p->x, p->z, 0xFF);
    g_aot_action_pressed = 0;
    if (g_aot.fired_event_id_this_frame) (void)scd_event_fire(g_aot.fired_event_id_this_frame);
    for (int f = 0; f < max_bilder; f++) { frame(1); if (f > 40 && !laeuft()) break; }
}

static void zeige(const char *was)
{
    printf("   %s:", was);
    for (int i = 0; i < s_prot_n; i++) printf(" %d@Cut%d", s_prot_id[i], s_prot_cut[i]);
    printf("\n");
    for (int i = 0; i < s_prot_n; i++) printf("     msg %2d \"%s\"\n", s_prot_id[i], s_prot_text[i]);
}

/* ---- rechner (U-05) ----------------------------------------------------------------------------------------------------- */
static void teil_rechner(void)
{
    printf("== rechner (U-05, 501) ==\n");
    for (int v = 0; v < 2; v++) {
        const uint16_t r = (uint16_t)(0x5010 + v);
        const int satz = v ? 40 : 2, speicher = v ? 45 : 6;
        re15_game_state_init(); re15_inv_init();
        /* Rechteck Slot 9 (5010 @0x45E) / Slot 1 (5011 @0x3BA) = (-20327,-21250,1600,2700): Stand (-20900,-19900), Blick +x */
        if (room_boot(r, -20900, -19900, 0, NULL, 0) != 0) continue;
        for (int n = 0; n < 2; n++) {
            untersuchen(-20900, -19900, 0, 3000);
            zeige(n ? "zweites Mal" : "erstes Mal");
            PRUEF(s_prot_n >= 2 && s_prot_id[0] == satz && s_prot_cut[0] == 2 && strstr(s_prot_text[0], "used a little") != NULL,
                  "ROOM%04X %d. Untersuchen: zuerst msg %d \"It looks like this was used a little while ago.\" im Nahbild Cut 2", r,
                  n + 1, satz);
            /* der Speicher-Text msg 6/45 wird im Port durch den Memory-Card-Ablauf ersetzt (scd_vm.c op_message_on ->
             * re15_savepoint_examine, Port-Text Id 254; ohne Karte "If I had a Memory Card...") */
            PRUEF(s_prot_n == 2 && s_prot_id[1] == 254 && strstr(s_prot_text[1], "Memory Card") != NULL && s_prot_cut[1] == 2,
                  "ROOM%04X: danach der Speicher-Ablauf (msg %d -> Memory-Card-Text, Cut 2)", r, speicher);
        }
    }
}

/* ---- plan (U-11) -------------------------------------------------------------------------------------------------------- */
static void teil_plan(void)
{
    printf("== plan (U-11, 402) ==\n");
    for (int v = 0; v < 2; v++) {
        const uint16_t r = (uint16_t)(0x4020 + v);
        re15_game_state_init(); re15_inv_init();
        PRUEF(!re15_map_owned_page(8), "ROOM%04X vorher: Blatt 8 nicht im Besitz", r);
        /* Slot 8 @0x073C Rechteck (-26650,-24750,1000,2300): Stand (-25100,-23600), Blick -x (2048) */
        if (room_boot(r, -25100, -23600, 2048, NULL, 0) != 0) continue;
        untersuchen(-25100, -23600, 2048, 3000);
        zeige("erstes Mal");
        PRUEF(s_prot_n == 2 && s_prot_id[0] == 0 && s_prot_id[1] == 2 && s_prot_cut[0] == 3 &&
              strstr(s_prot_text[1], "taken the laboratory map") != NULL,
              "ROOM%04X: msg 0 (Cut 3), dann msg 2 \"You've taken the laboratory map.\"", r);
        PRUEF(re15_game_flag_get(9, 36) == 1 && re15_map_owned_page(8) && (re15_map_owned_bits() & (1u << 8)),
              "ROOM%04X: Bit (9,36) = %d, Blatt 8 im Besitz", r, re15_game_flag_get(9, 36));
        PRUEF(!re15_map_owned_page(9) && !re15_map_owned_page(12), "ROOM%04X: nur Blatt 8, nicht 9 (5030) oder 12", r);
        untersuchen(-25100, -23600, 2048, 3000);
        zeige("zweites Mal");
        PRUEF(s_prot_n == 1 && s_prot_id[0] == 0, "ROOM%04X: zweites Untersuchen nur msg 0", r);
    }
}

/* ---- schalter (U-02) ---------------------------------------------------------------------------------------------------- */
static void teil_schalter(void)
{
    printf("== schalter (U-02, 3091) ==\n");
    for (int strom = 0; strom < 2; strom++) {
        re15_game_state_init(); re15_inv_init();
        re15_game_flag_set(4, 8, (uint8_t)strom);
        re15_game_flag_set(4, 16, 1);   /* Szene vorbei (sub00 @0x01492): keine Begleiter-Saetze */
        /* Slot 5 @0x013EC Rechteck (-22900,-4750,500,1000), floor 0: Stand (-23400,-4250), Blick +x */
        if (room_boot(0x3091, -23400, -4250, 0, NULL, 0) != 0) continue;
        untersuchen(-23400, -4250, 0, 2000);
        zeige(strom ? "mit Strom" : "ohne Strom");
        if (!strom)
            PRUEF(s_prot_n == 2 && s_prot_id[0] == 35 && s_prot_id[1] == 3 && strstr(s_prot_text[0], "power supply") != NULL,
                  "ohne Strom (4,8)=0: msg 35 \"There's a power supply.\", dann msg 3 \"I can't reach the breaker switch...\"");
        else
            PRUEF(s_prot_n == 1 && s_prot_id[0] == 2, "mit Strom (4,8)=1: nur msg 2 wie bisher");
    }
}

/* ---- szene (U-01) ------------------------------------------------------------------------------------------------------- */
static const re15_actor_t *aktor_typ(uint8_t typ)
{
    for (int i = 1; i < RE15_ACTOR_MAX; i++) if (g_actors[i].active && g_actors[i].type == typ) return &g_actors[i];
    return NULL;
}
static int gier_abstand(const re15_actor_t *a, double zx, double zz)
{
    const double dx = zx - a->x, dz = zz - a->z;
    int soll = (int)lround(atan2(-dz, dx) / (2.0 * 3.14159265358979) * 4096.0) & 4095;   /* dachkiste_r36.c wp_yaw_zu */
    int d = ((int)(a->rot_y & 4095) - soll) & 4095;
    return d > 2048 ? 4096 - d : d;
}

static void teil_szene(void)
{
    printf("== szene (U-01, 3091 sub07) ==\n");
    re15_game_state_init(); re15_inv_init();
    re15_game_flag_set(9, 21, 1);   /* sub02 @0x01596 Ck(9,21)=0 -> sonst nur msg 6; 21 setzt sub06 @0x016C2 */
    static const uint8_t k_banken[] = { 0x49, 0x42 };   /* sub00 @0x014E0 / @0x014F4 */
    if (room_boot(0x3091, -23592, 109, 2048, k_banken, 2) != 0) return;
    PRUEF(aktor_typ(0x49) && aktor_typ(0x42), "John (0x49) und Elza (0x42) stehen bereit (sub00 @0x014E0/@0x014F4)");
    int slot = -1;
    for (int k = SCD_EVENT_SLOT_FIRST; k <= SCD_EVENT_SLOT_LAST && slot < 0; k++)
        if (!g_scd.threads[k].active && scd_thread_start(k, s_rdt.sub_scd[2]) == 0) slot = k;   /* sub02 -> Evt_exec 7 @0x015A4 */
    int bild22 = -1, j_hof = 9999, e_hof = 9999, j_lob = 9999, e_lob = 9999;
    for (int f = 0; f < 20000; f++) {
        frame(1);
        if (bild22 < 0 && s_prot_n > 0 && s_prot_id[s_prot_n - 1] == 22) {
            bild22 = s_bild;
            const re15_actor_t *j = aktor_typ(0x49), *e = aktor_typ(0x42);
            if (j && e) {
                j_hof = gier_abstand(j, -24506, 1500); e_hof = gier_abstand(e, -24506, 1500);
                j_lob = gier_abstand(j, -28481, -155); e_lob = gier_abstand(e, -28481, -155);
            }
        }
        if (f > 100 && !laeuft() && !re15_game_flag_get(2, 7) && !re15_game_flag_get(1, 27)) break;
    }
    zeige("Folge");
    static const int k_soll[] = { 32, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 33, 25, 26, 27, 28, 34, 29, 30 };
    const int ns = (int)(sizeof k_soll / sizeof k_soll[0]);
    int ok = s_prot_n == ns;
    for (int i = 0; i < ns && i < s_prot_n; i++) if (s_prot_id[i] != k_soll[i]) ok = 0;
    PRUEF(ok, "Folge 32, 9..24, 33 (D309 #21), 25..28, 34 (D309 #24), 29, 30 (%d Zeilen)", s_prot_n);
    int i33 = -1, i34 = -1;
    for (int i = 0; i < s_prot_n; i++) { if (s_prot_id[i] == 33) i33 = i; if (s_prot_id[i] == 34) i34 = i; }
    PRUEF(i33 >= 0 && strstr(s_prot_text[i33], "swarming back") != NULL, "msg 33 John: \"%s\"", i33 >= 0 ? s_prot_text[i33] : "-");
    PRUEF(i34 >= 0 && strstr(s_prot_text[i34], "all right, won't you") != NULL, "msg 34 Elza: \"%s\"", i34 >= 0 ? s_prot_text[i34] : "-");
    PRUEF(i33 >= 0 && i33 + 1 < s_prot_n && s_prot_bild[i33 + 1] - s_prot_bild[i33] >= 60,
          "msg 33 steht, bis sie zu Ende ist (%d Bilder bis msg 25)", i33 >= 0 && i33 + 1 < s_prot_n ? s_prot_bild[i33 + 1] - s_prot_bild[i33] : -1);
    printf("   bei msg 22 (Bild %d): John Hof %d / Lobby %d, Elza Hof %d / Lobby %d (Gier-Abstand, 4096 = 360 Grad)\n", bild22,
           j_hof, j_lob, e_hof, e_lob);
    PRUEF(bild22 >= 0 && j_hof <= 128 && e_hof <= 128 && j_lob > 256 && e_lob > 256,
          "nach den Schlaegen: John und Elza blicken zur Hoftuer (-24506,1500), nicht zur Lobby-Tuer (-28481,-155)");
    PRUEF(re15_game_flag_get(4, 16) == 1, "Szene zu Ende: (4,16) = 1 (sub07 @0x016D0)");
}

/* ---- bytes ------------------------------------------------------------------------------------------------------------- */
static void teil_bytes(void)
{
    printf("== bytes ==\n");
#ifdef U42_MODUL
    int n = 0;
    const uint8_t *p = re15_ungenutzt_r42_programm(0x5010, 0, &n);
    PRUEF(p && n == 20 && p[0] == 0x29 && p[1] == 0x02 && p[2] == 0x2b && p[3] == 2 && p[8] == 0x2b && p[9] == 6,
          "5010: Cut 2, msg 2, msg 6 (sub03 @0x4F6 + Disc #40)");
    p = re15_ungenutzt_r42_programm(0x5011, 0, &n);
    PRUEF(p && n == 20 && p[3] == 40 && p[9] == 0x2d, "5011: msg 40, msg 45 (sub08 @0xE34)");
    p = re15_ungenutzt_r42_programm(0x4020, 0, &n);
    static const uint8_t k_plan_kern[] = { 0x21, 9, 36, 0, 0x22, 9, 36, 1, 0x2b, 2, 0xff, 0xff };
    PRUEF(p && n > 24 && !memcmp(p + 12, k_plan_kern, sizeof k_plan_kern), "4020: Ck(9,36)=0, Set(9,36), msg 2 (Form ROOM5030 sub00/sub02)");
    int t = 0;
    const uint8_t *x = re15_ungenutzt_r42_text(0x4020, 2, &t);
    if (rdt_laden(0x5030) == 0) {
        const uint8_t *q = s_raw + 0x1132;
        PRUEF(x && t == 36 && !memcmp(x, q, 36), "4020 msg 2 = ROOM5030 msg 1 @0x1132 Byte fuer Byte");
    }
    if (rdt_laden(0x3091) == 0) {
        memset(&g_room_rdt, 0, sizeof g_room_rdt);
        g_room_rdt = s_rdt;
        p = re15_ungenutzt_r42_programm(0x3091, 0, &n);
        const int alt = 0x1DB2 - 0x16CA;
        PRUEF(p && n > alt && !memcmp(p, s_raw + 0x16CA, 0x1AA6 - 0x16CA), "3091 sub07: Kopf bis @0x1AA6 unveraendert");
        static const uint8_t k_hof[4] = { 0x46, 0xa0, 0xdc, 0x05 };
        PRUEF(p && p[0x1AA6 - 0x16CA + 2] == k_hof[0] && p[0x1AA6 - 0x16CA + 3] == k_hof[1] && p[0x1AA6 - 0x16CA + 6] == k_hof[2] &&
              p[0x1ACA - 0x16CA + 4] == k_hof[0] && p[0x1ACA - 0x16CA + 7] == k_hof[3],
              "3091 sub07: Blickziele @0x1AA6/@0x1ACA = Disc-Tonort `46 a0 .. dc 05` (@0x1778)");
        PRUEF(p && !memcmp(p + n - (0x1DB2 - 0x1C64), s_raw + 0x1C64, 0x1DB2 - 0x1C64), "3091 sub07: Schluss ab @0x1C64 unveraendert");
        memset(&g_room_rdt, 0, sizeof g_room_rdt);
    }
#else
    printf("  (Modul re15_ungenutzt_r42.h nicht im Baum - uebersprungen)\n");
#endif
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "rechner")  || !strcmp(teil, "alle")) teil_rechner();
    if (!strcmp(teil, "plan")     || !strcmp(teil, "alle")) teil_plan();
    if (!strcmp(teil, "schalter") || !strcmp(teil, "alle")) teil_schalter();
    if (!strcmp(teil, "szene")    || !strcmp(teil, "alle")) teil_szene();
    if (!strcmp(teil, "bytes")    || !strcmp(teil, "alle")) teil_bytes();
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
