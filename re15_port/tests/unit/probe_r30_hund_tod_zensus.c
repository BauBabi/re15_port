/* probe_r30_hund_tod_zensus.c — Runde 30, Thema I, Abschnitt ZENSUS
 * (Dossier analysis/befunde_runde30/hund-tod.md).
 *
 * Frage: gilt "der Spieler steht nach dem toedlichen Angriff wieder auf" auch fuer andere
 * Gegner mit Griff/Biss? Reiner MESSSTAND, keine Engine-Aenderung.
 *
 * Aufruf:
 *   probe_r30_hund_tod_zensus raum  <STAGEn/ROOMxxxx.RDT> <raum-hex> [bank bit]
 *       -> faehrt den Raum hoch und listet die gespawnten Aktoren (Typ, Lage, grid)
 *   probe_r30_hund_tod_zensus fall  <name> <druck> [ausfuehrlich]
 *   probe_r30_hund_tod_zensus alle
 *       druck 0 = keine Taste, 1 = Kreuz jedes 2. Bild
 *
 * ⛔ MASSGEBLICH ist "fall" = EIN Lauf je Prozess. "alle" faehrt die Faelle nacheinander im
 *    selben Prozess und traegt Zustand weiter (gemessen: 2 von 18 Zeilen weichen ab —
 *    Gorilla Druck 1 stirbt 10 Bilder frueher, Birkin Druck 1 wird gar nicht getroffen).
 *    Fuer das Dossier gilt build/r30_hund-tod/zensus_einzeln.log.
 *
 * Je Lauf: Spieler mit wenig hp neben den Gegner, echter re15_game_step + Live-KI
 * (Flavor RE2 = Auslieferungs-Default), bis Game Over oder Bildgrenze. */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_esp.h"
#include "re2_ems.h"
#include "re15_math.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

typedef struct {
    const char *name;
    const char *rdt_rel;          /* relativ zu shared_assets/PSX */
    unsigned    room;
    int         flag_bank, flag_bit;   /* -1 = kein Flag; sonst := 1 */
    unsigned    typ;              /* der Gegner, um den es geht */
    int         direkt;           /* 1 = Gegner direkt setzen statt ueber das Skript */
    int32_t     ex, ey, ez;       /* Lage beim Direkt-Setzen */
    int         fest;             /* 1 = Spieler-Lage fest vorgegeben */
    int32_t     px, py, pz;
    int         hp0;
    int         bilder;
    int         szenario;         /* Eintritts-Szenario fuer scd_room_reenter */
    int         geh;              /* 1 = der Spieler geht mit 75/Bild zum Ziel (gx,gz) */
    int32_t     gx, gz;
    int         stop_cut;         /* >= 0: Gehen endet, sobald dieser Cut gezeigt wird */
} fall_t;

static const fall_t k_faelle[] = {
    /* name        rdt                     raum   flag      typ direkt e-lage            fest p-lage             hp bilder szen geh ziel          stop */
    { "hund",     "/STAGE1/ROOM11D0.RDT", 0x11D0,  3, 152, 0x20, 0, 0,0,0,            1, -7878, 0, -17384,    5, 1500,  0, 0, 0, 0,           -1 },
    { "zombie",   "/STAGE1/ROOM1010.RDT", 0x1010, -1,  -1, 0x10, 0, 0,0,0,            0, 0,0,0,               5, 1800,  0, 0, 0, 0,           -1 },
    { "zombie1140", "/STAGE1/ROOM1140.RDT", 0x1140, -1, -1, 0x10, 0, 0,0,0,           0, 0,0,0,               5, 1800,  0, 0, 0, 0,           -1 },
    { "kraehe",   "/STAGE1/ROOM10C0.RDT", 0x10C0, -1,  -1, 0x21, 0, 0,0,0,            0, 0,0,0,               4, 2400,  0, 0, 0, 0,           -1 },
    { "gorilla",  "/STAGE1/ROOM11C0.RDT", 0x11C0,  4, 0x40, 0x27, 0, 0,0,0,           1, -14000, 0, 6000,     5, 2400,  0, 0, 0, 0,           -1 },
    { "arme",     "/STAGE1/ROOM1210.RDT", 0x1210, -1,  -1, 0x1A, 0, 0,0,0,            1, -19500, 0, -3500,    5, 1800,  0, 1, -23000, -15747, -1 },
    { "spinne",   "/STAGE2/ROOM2090.RDT", 0x2090, -1,  -1, 0x25, 0, 0,0,0,            0, 0,0,0,               5, 2400,  0, 0, 0, 0,           -1 },
    { "gator",    "/STAGE2/ROOM2090.RDT", 0x2090, -1,  -1, 0x23, 1, -6000, 0, -22000, 1, -4000, 0, -22000,   30, 2400,  0, 0, 0, 0,           -1 },
    { "birkin3070", "/STAGE3/ROOM3070.RDT", 0x3070, -1, -1, 0x30, 2, 0,0,0,           0, 0,0,0,               5, 2400,  0, 0, 0, 0,           -1 },
    { "birkin",   "/STAGE5/ROOM5090.RDT", 0x5090, -1,  -1, 0x36, 0, 0,0,0,            1, 25600, 0, -23350,    5, 4000, 14, 1, -30000, -23350, 12 },
};
#define N_FAELLE ((int)(sizeof k_faelle / sizeof k_faelle[0]))

static re15_rdt_t           s_rdt;
static re15_camera_view_t   s_cam;
static re15_game_ctx_t      s_ctx;
static re15_emd_animation_t s_pl00_anim;
static re15_emd_skeleton_t  s_pl00_skel;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int load_pl00(void)
{
    size_t a = 0, b = 0;
    uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &a);
    uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &b);
    return edd && emr &&
           re15_emd_parse_animation(edd, a, &s_pl00_anim) == 0 &&
           re15_emd_parse_skeleton (emr, b, &s_pl00_skel) == 0;
}

/* Bankwahl wie platform/pc/main.c pc_enemy_load_ex (ohne Hybrid und ohne Ton):
 *   0x36/0x37 in ROOM5090/5091 -> RE2 ; 0x1A unter RE2 -> RE2-Art 0x2D ;
 *   RE2-eigene Typen -> RE2 gleicher Art ; sonst RE1.5 aus CDEMD0.EMS ; 0x23 -> RE2. */
static uint8_t *s_ems2 = NULL;  static size_t s_ems2_n = 0;
static uint8_t *s_ems15 = NULL; static size_t s_ems15_n = 0;
static const char *s_bank_herkunft = "-";
static int bank_re2(unsigned type, int kind, re15_enemy_bank_t *eb)
{
    if (!s_ems2) s_ems2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems2_n);
    if (s_ems2 && re2_ems_load_bank(s_ems2, s_ems2_n, kind, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; (void)type; return 1;
    }
    return 0;
}
static int bank_laden(unsigned type, unsigned room)
{
    re15_enemy_bank_t *eb = re15_enemy_find((uint8_t)type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc((uint8_t)type);
    if (!eb) return 0;
    if ((type == 0x36u || type == 0x37u) && (room == 0x5090u || room == 0x5091u)) {
        if (bank_re2(type, (int)type, eb)) { s_bank_herkunft = "RE2"; return 1; }
    }
    if (type == 0x1Au && re15_ai_re2_for_type(0x1Au)) {
        if (bank_re2(type, 0x2D, eb)) { s_bank_herkunft = "RE2 Art 0x2D"; return 1; }
    }
    if (re15_ai_re2_for_type(type) && re15_re2_owns_type(type)) {
        if (bank_re2(type, (int)type, eb)) { s_bank_herkunft = "RE2"; return 1; }
    }
    {
        if (!s_ems15) s_ems15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_ems15_n);
        int idx = s_ems15 ? re15_ems_index_for_type((uint8_t)type) : -1;
        size_t off = 0, len = 0;
        if (idx >= 0 && re15_ems_get_entry(s_ems15, s_ems15_n, idx, &off, &len) == 0) {
            uint8_t *blob = (uint8_t *)malloc(len);
            memcpy(blob, s_ems15 + off, len);
            if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, NULL) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->victim_ok = (re15_emd_parse_victim_bank(blob, len, &eb->skel_victim,
                                                            &eb->anim_victim) == 0);
                s_bank_herkunft = "RE1.5";
                return 1;
            }
        }
    }
    if (type == 0x23u && bank_re2(type, 0x23, eb)) { s_bank_herkunft = "RE2"; return 1; }
    eb->type = 0;
    s_bank_herkunft = "FEHLT";
    return 0;
}

static uint16_t s_pad_now = 0, s_pad_edge = 0;
static int s_shown = 0;
static void frame(void)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = s_pad_now; s_ctx.pad_pressed = s_pad_edge;
    { extern int re15_cam_present_tick(void);
      if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id; }
    s_ctx.active_cut = s_shown;
    re15_game_step(&s_ctx);
}

static unsigned s_typen[16]; static int s_ntypen = 0;

static int s_direkt2_ok = 0;
static int begehbar_vor(int32_t x, int32_t z)
{
    int32_t nx = x, nz = z;
    re15_collision_ensure_band(0);
    if (re15_collision_on_floor(&s_rdt, x, z)) return 0;
    re15_collision_constrain(&s_rdt, x, z, &nx, &nz);
    return (nx == x && nz == z);
}
static void bringup(const fall_t *fl, int mit_baenken)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_victim_reset();
    re15_esp_fx_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    g_current_room_id = (uint16_t)fl->room; g_room_change.pending = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    if (fl->flag_bank >= 0) re15_game_flag_set(fl->flag_bank, fl->flag_bit, 1);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    if (fl->fest) { pl->x = fl->px; pl->y = fl->py; pl->z = fl->pz; }
    /* der ECHTE Raum-Eintritt der Engine (Tuer/Boot/Load laufen alle hier durch) */
    scd_room_reenter(&s_rdt, pl->x, pl->z, (uint8_t)fl->szenario);
    g_scd.cut_auto_enabled = 1;
    s_shown = fl->szenario; s_ctx.active_cut = s_shown;
    for (int i = 0; i < 8; i++) scd_vm_tick();
    if (mit_baenken) {
        /* Baenke NACH re15_enemy_reset und VOR dem ersten KI-Tick (der INIT zieht
         * Cliplaengen aus der Bank) */
        for (int i = 0; i < s_ntypen; i++) bank_laden(s_typen[i], fl->room);
        if (fl->typ == 0x36u) bank_laden(0x37u, fl->room);
        bank_laden(fl->typ, fl->room);
    }
    if (fl->direkt == 2) {
        /* der Skript-Gegner steht geparkt ausserhalb (ROOM3070: 30000/30000): ihn auf einen
         * begehbaren Platz holen, 2000 neben einen ebenfalls begehbaren Spieler-Platz */
        int es = -1;
        for (int s2 = 1; s2 < RE15_ACTOR_MAX; s2++)
            if (g_actors[s2].active && g_actors[s2].type == fl->typ) { es = s2; break; }
        int gefunden = 0;
        for (int32_t z = -28000; z <= 28000 && !gefunden; z += 1000)
            for (int32_t x = -28000; x <= 28000 && !gefunden; x += 1000)
                if (begehbar_vor(x, z) && begehbar_vor(x + 2000, z) && begehbar_vor(x + 1000, z)) {
                    pl->x = x; pl->z = z; pl->y = 0;
                    if (es >= 0) { g_actors[es].x = x + 2000; g_actors[es].z = z; g_actors[es].y = 0; }
                    gefunden = 1;
                }
        s_direkt2_ok = gefunden && es >= 0;
    }
    if (fl->direkt == 1) {
        int slot = RE15_ACTOR_MAX - 1;
        re15_actor_t *e = &g_actors[slot];
        memset(e, 0, sizeof *e);
        e->active = 1; e->type = (uint8_t)fl->typ;
        e->x = fl->ex; e->y = fl->ey; e->z = fl->ez;
        e->grid_id = 0; e->state = 0; e->em_flag_id = 0xFF;
        re15_enemy_apply_hitbox(e, (uint8_t)fl->typ);
    }
}

static int begehbar(int32_t x, int32_t z, int32_t y)
{
    int32_t nx = x, nz = z;
    re15_collision_ensure_band(y);
    if (re15_collision_on_floor(&s_rdt, x, z)) return 0;
    re15_collision_constrain(&s_rdt, x, z, &nx, &nz);
    return (nx == x && nz == z);
}

static int gegner(unsigned typ)
{
    /* Zombie-Familie: jeder Typ 0x10..0x18 zaehlt */
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!g_actors[s].active) continue;
        if (g_actors[s].type == typ) return s;
        if (typ == 0x10u && g_actors[s].type >= 0x10u && g_actors[s].type <= 0x18u) return s;
        if (typ == 0x36u && (g_actors[s].type == 0x30u || g_actors[s].type == 0x36u)) return s;
    }
    return -1;
}

typedef struct {
    int gemessen;                 /* 1 = ein Angriff hat hp gesenkt */
    int bisse, hp_min, hp_ende;
    int griff_bild, tot_bild, praes_bild, cmd3_bild, gameover_bild;
    int auferstanden_bild, steht_wieder_bild;
    int hp_hoch;                  /* wie oft stieg hp im Lauf (jeder Anstieg) */
    int vs_max;
} erg_t;

static int lauf(const fall_t *fl, int druck, int ausfuehrlich, erg_t *er)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(er, 0, sizeof *er);
    er->griff_bild = er->tot_bild = er->praes_bild = er->cmd3_bild = er->gameover_bild = -1;
    er->auferstanden_bild = er->steht_wieder_bild = -1;
    int es = gegner(fl->typ);
    if (es < 0) { printf("  FEHLLAUF %s: kein Gegner Typ 0x%02X im Raum\n", fl->name, fl->typ); return 0; }
    re15_actor_t *e = &g_actors[es];
    if (fl->fest) { pl->x = fl->px; pl->y = fl->py; pl->z = fl->pz; }
    else if (fl->direkt == 2) { if (!s_direkt2_ok) printf("  WARNUNG: kein begehbarer Platz gefunden\n"); }
    else {
        static const int dx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
        static const int dz[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
        int ok = 0;
        pl->y = 0;
        for (int r = 1200; r <= 3000 && !ok; r += 600)
            for (int k = 0; k < 8 && !ok; k++) {
                int32_t x = e->x + dx[k] * r, z = e->z + dz[k] * r;
                if (begehbar(x, z, pl->y)) { pl->x = x; pl->z = z; ok = 1; }
            }
        if (!ok) { pl->x = e->x + 1500; pl->z = e->z; }
    }
    pl->floor = e->floor;
    re15_collision_ensure_band(pl->y);
    pl->rot_y = 1024; pl->hp = (int16_t)fl->hp0; pl->state = 0; pl->motion = 0;
    er->hp_min = fl->hp0;
    /* Zwinger-/Fensterhunde freigeben wie im Hunde-Lauf */
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x20 && g_actors[s].grid_id == 0x41)
            g_actors[s].grid_id = 0x42;

    int war_negativ = 0, geht = fl->geh;
    int v_hp = pl->hp, v_st = -1, v_vs = -1, v_gr = -1, v_tot = -1, v_go = -1, v_es1 = -1;
    if (ausfuehrlich)
        printf("# %s druck=%d hp0=%d Spieler (%d,%d,%d) Gegner slot %d typ 0x%02X (%d,%d,%d)"
               " grid=0x%02X Bank=%s\n", fl->name, druck, fl->hp0, (int)pl->x, (int)pl->y,
               (int)pl->z, es, e->type, (int)e->x, (int)e->y, (int)e->z, e->grid_id,
               s_bank_herkunft);
    for (int f = 0; f < fl->bilder; f++) {
        int gr_vor = re15_player_is_grabbed();
        s_pad_now = 0; s_pad_edge = 0;
        if (druck == 1 && (f & 1)) { s_pad_now = 0x4000; s_pad_edge = 0x4000; }
        if (geht && !re15_player_is_grabbed() && re15_player_victim_state() == 0 &&
            pl->hp >= 0 && pl->hp == fl->hp0) {
            int32_t nx = pl->x, nz = pl->z;
            nx = (pl->x < fl->gx) ? pl->x + 75 : (pl->x > fl->gx) ? pl->x - 75 : pl->x;
            if ((pl->x < fl->gx && nx > fl->gx) || (pl->x > fl->gx && nx < fl->gx)) nx = fl->gx;
            nz = (pl->z < fl->gz) ? pl->z + 75 : (pl->z > fl->gz) ? pl->z - 75 : pl->z;
            if ((pl->z < fl->gz && nz > fl->gz) || (pl->z > fl->gz && nz < fl->gz)) nz = fl->gz;
            re15_collision_ensure_band(pl->y);
            re15_collision_constrain(&s_rdt, pl->x, pl->z, &nx, &nz);
            pl->x = nx; pl->z = nz;
            if (fl->stop_cut >= 0 && s_shown == fl->stop_cut) geht = 0;
        }
        int hp_vor = pl->hp;
        frame();
        int gr  = re15_player_is_grabbed();
        int vs  = re15_player_victim_state();
        int tot = re15_player_is_dead();
        int pr  = re15_death_presentation_active();
        int c3  = re15_player_death_cmd3_active();
        if (pl->hp < hp_vor) { er->bisse++; er->gemessen = 1; }
        if (pl->hp > hp_vor) er->hp_hoch++;
        if (pl->hp < er->hp_min) er->hp_min = pl->hp;
        if (vs > er->vs_max) er->vs_max = vs;
        if ((gr || vs) && !gr_vor && er->griff_bild < 0) er->griff_bild = f;
        if (tot && er->tot_bild < 0) er->tot_bild = f;
        if (pr  && er->praes_bild < 0) er->praes_bild = f;
        if (c3  && er->cmd3_bild < 0) er->cmd3_bild = f;
        if (g_gameover_active && er->gameover_bild < 0) er->gameover_bild = f;
        if (pl->hp < 0) war_negativ = 1;
        if (war_negativ && pl->hp >= 0 && er->auferstanden_bild < 0) er->auferstanden_bild = f;
        if (war_negativ && pl->hp >= 0 && vs == 0 && !gr && !tot && er->steht_wieder_bild < 0)
            er->steht_wieder_bild = f;
        int aend = (pl->hp != v_hp) || (pl->state != v_st) || (vs != v_vs) || (gr != v_gr) ||
                   (tot != v_tot) || (g_gameover_active != v_go) ||
                   ((int)e->sub_state_1 != v_es1);
        if (ausfuehrlich && (aend || (f % 60) == 0))
            printf("%-5d hp=%-4d st=%d mot=%-3d afr=%-3d | vs=%d gr=%d tot=%d praes=%d cmd3=%d go=%d |"
                   " gegner st=%d s1=%d s2=%d s3=%d clip=%d afr=%d hp=%d d=(%d,%d)%s\n",
                   f, pl->hp, pl->state, (int)pl->motion, (int)pl->anim_frame, vs, gr, tot, pr, c3,
                   g_gameover_active, e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3,
                   (int)e->motion, (int)e->anim_frame, e->hp, (int)(e->x - pl->x),
                   (int)(e->z - pl->z), aend ? "  <--" : "");
        v_hp = pl->hp; v_st = pl->state; v_vs = vs; v_gr = gr; v_tot = tot;
        v_go = g_gameover_active; v_es1 = e->sub_state_1;
        if (g_gameover_active && f > er->gameover_bild + 30) break;
    }
    er->hp_ende = pl->hp;
    return 1;
}

static void zeige(const fall_t *fl, int druck, const erg_t *er)
{
    const char *urteil;
    if (!er->gemessen)                 urteil = "NICHT GEMESSEN (kein Treffer)";
    else if (er->hp_min >= 0)          urteil = "NICHT TOEDLICH (hp blieb >= 0)";
    else if (er->auferstanden_bild >= 0) urteil = "AUFERSTANDEN";
    else if (er->gameover_bild >= 0)   urteil = "stirbt, Game Over";
    else                               urteil = "hp < 0, aber KEIN Game Over";
    printf("%-8s typ=0x%02X bank=%-12s druck=%d | treffer=%d hp_min=%d hp_ende=%d hp_hoch=%d"
           " griff@%d vs_max=%d tot@%d praes@%d cmd3@%d go@%d auf@%d | %s\n",
           fl->name, fl->typ, s_bank_herkunft, druck, er->bisse, er->hp_min, er->hp_ende,
           er->hp_hoch, er->griff_bild, er->vs_max, er->tot_bild, er->praes_bild, er->cmd3_bild,
           er->gameover_bild, er->auferstanden_bild, urteil);
}

static int raum_laden(const fall_t *fl)
{
    char path[600]; size_t rsz = 0;
    snprintf(path, sizeof path, "%s%s", RE15_ASSET_PSX_DIR, fl->rdt_rel);
    uint8_t *raw = slurp(path, &rsz);           /* bleibt resident (RDT aliast hinein) */
    if (!raw) { printf("FEHLT: %s\n", path); return 0; }
    if (re15_rdt_parse(raw, rsz, &s_rdt) != 0) { printf("FEHLT: RDT-Parse %s\n", path); return 0; }
    /* Durchgang 1: welche Typen spawnt das Skript? (Baenke muessen VOR dem Spawn stehen) */
    s_ntypen = 0;
    bringup(fl, 0);
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!g_actors[s].active || g_actors[s].type == 0) continue;
        int schon = 0;
        for (int i = 0; i < s_ntypen; i++) if (s_typen[i] == g_actors[s].type) schon = 1;
        if (!schon && s_ntypen < 16) s_typen[s_ntypen++] = g_actors[s].type;
    }
    return 1;
}

static int fall_fahren(const fall_t *fl, int druck, int ausfuehrlich)
{
    erg_t er;
    if (!raum_laden(fl)) return 0;
    bringup(fl, 1);
    bank_laden(fl->typ, fl->room);           /* setzt s_bank_herkunft fuer die Ausgabe */
    if (!lauf(fl, druck, ausfuehrlich, &er)) return 0;
    zeige(fl, druck, &er);
    return 1;
}

int main(int argc, char **argv)
{
    const char *mode = (argc > 1) ? argv[1] : "alle";
    if (!load_pl00()) { printf("FEHLT: PL00-Rig\n"); return 77; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    s_ctx.pl00_skel = &s_pl00_skel; s_ctx.pl00_anim = &s_pl00_anim;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    {   /* Gegenprobe: R30_FLAVOR=re15 faehrt dieselben Faelle mit der RE1.5-KI */
        const char *fv = getenv("R30_FLAVOR");
        if (fv && !strcmp(fv, "re15")) re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    }
#ifdef _WIN32
    _putenv("RE15_GB_TEST=1");               /* Alligator: sofort angriffsbereit (wie probe_gator_fress) */
#endif

    if (!strcmp(mode, "raum")) {
        fall_t fl; memset(&fl, 0, sizeof fl);
        fl.name = "raum"; fl.rdt_rel = (argc > 2) ? argv[2] : "/STAGE1/ROOM11D0.RDT";
        fl.room = (argc > 3) ? (unsigned)strtoul(argv[3], NULL, 16) : 0x11D0u;
        fl.flag_bank = (argc > 5) ? atoi(argv[4]) : -1;
        fl.flag_bit  = (argc > 5) ? atoi(argv[5]) : -1;
        if (!raum_laden(&fl)) return 1;
        printf("Raum %04X: %d Typen\n", fl.room, s_ntypen);
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active)
                printf("   slot %-2d typ=0x%02X pos=(%d,%d,%d) st=%d s1=%d hp=%d grid=0x%02X floor=%d\n",
                       s, g_actors[s].type, (int)g_actors[s].x, (int)g_actors[s].y,
                       (int)g_actors[s].z, g_actors[s].state, g_actors[s].sub_state_1,
                       g_actors[s].hp, g_actors[s].grid_id, g_actors[s].floor);
        return 0;
    }
    if (!strcmp(mode, "fall")) {
        const char *nm = (argc > 2) ? argv[2] : "hund";
        int druck = (argc > 3) ? atoi(argv[3]) : 0;
        int ausf  = (argc > 4) ? atoi(argv[4]) : 0;
        for (int i = 0; i < N_FAELLE; i++)
            if (!strcmp(k_faelle[i].name, nm)) return fall_fahren(&k_faelle[i], druck, ausf) ? 0 : 1;
        printf("unbekannter Fall\n");
        return 1;
    }
    if (!strcmp(mode, "alle")) {
        for (int i = 0; i < N_FAELLE; i++)
            for (int druck = 0; druck <= 1; druck++)
                fall_fahren(&k_faelle[i], druck, 0);
        return 0;
    }
    printf("unbekannter Modus\n");
    return 1;
}
