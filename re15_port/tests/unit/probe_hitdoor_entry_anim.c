/* probe_hitdoor_entry_anim.c — MESSSONDE / REPRO (A1, kein Fix): Nutzer-Report 2026-08-08:
 *   "Nach Zombie-Kampf (getroffen/gegrabbt) spielt Leon nach dem naechsten Tuerwechsel
 *    am Raumanfang eine komische Animation."
 *
 * Faehrt den ECHTEN Tuer-Pfad (re15_aot_scan DOOR-Fire -> re15_room_request_change ->
 * re15_room_apply_pending, room_common.c) mit Datei-Ladern als ARCH-Callbacks und dem
 * VOLLEN re15_game_step als Frame-Maschine (wie probe_crow_flinch + probe_cam_1030_reentry):
 *
 *   ROOM1170 (Tuer slot5, flag(4,0xc3)=1) -> ROOM1140 (Briefing, Zombies)
 *     Variante A: KEIN Kampf (Kontrolle)
 *     Variante B: ein Melee-Hit (hp -= 10, wie die Live-Angriffe direkt schreiben —
 *                 game_step_common.c Flinch-Detector-Kommentar), Flinch ausklingen lassen
 *     Variante C: ECHTER Grab (Zombie-AI weckt/grabt), Mash-Escape, Release ausklingen lassen
 *   dann Tuer aus ROOM1140 raus (erste DOOR-AOT mit dest != current) und die ersten
 *   120 Ticks im Zielraum pro Tick loggen:
 *     motion / anim_frame / anim_frac / anim_flags / state / sub1-3 / hit_react / hp /
 *     pos / rot / victim_state / aim_active / grabbed.
 *
 * ERWARTETER DIFF (Hypothese, zu MESSEN): re15_player_cmd_reset (game_step_common.c:118-124,
 * gerufen vom Raumwechsel room_common.c:148) setzt s_prev_hp = 100. Der HP-Drop-Detector
 * (game_step_common.c:418 `pl->hp < s_prev_hp`) sieht im ERSTEN Tick des Zielraums
 * hp(<100) < 100 -> spielt einen Phantom-Flinch (Clip 0x0a, 20 Ticks, Knockback 200).
 * Das Original hat diesen Detector nicht (Raum-Load schreibt cmd=0 @0x80031518). */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_camera.h"
#include "re15_collision.h"
#include "re15_player.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_game_step.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern int re15_player_victim_state(void);
extern int re15_player_aim_active(void);

/* --- Port-seitige Sicht auf den Raum (wie platform/pc/main.c) --- */
static re15_rdt_t                s_rdt;
static int                       s_rdt_ok = 0;
static const re15_camera_cut_t  *s_active_cuts = NULL;
static int                       s_active_cut_count = 0;
static int                       s_active_cut_idx = 0;
static re15_camera_view_t        s_cam_view;

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

/* ARCH-Callback (Gegenstueck zu room_pc.c re15_room_load) */
static uint8_t *s_room_buf = NULL;
static int probe_load_rdt(unsigned room_id)
{
    char path[600];
    size_t sz = 0;
    snprintf(path, sizeof path, "%s/STAGE%u/ROOM%04X.RDT",
             RE15_XSTR(RE15_ASSETS_PATH), (room_id >> 12) & 0xF, room_id);
    uint8_t *buf = read_file(path, &sz);
    if (!buf) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return -1; }
    if (re15_rdt_parse(buf, sz, &g_room_rdt) != 0) { free(buf); return -1; }
    if (s_room_buf) free(s_room_buf);
    s_room_buf        = buf;
    g_current_room_id = room_id;
    g_room_rdt_ok     = 1;
    return 0;
}
static void probe_reset_render(void) { }
static int  probe_load_bg(int cut) { (void)cut; return 0; }

static void make_ctx_room(re15_room_apply_ctx_t *rc)
{
    memset(rc, 0, sizeof *rc);
    rc->rdt              = &s_rdt;
    rc->rdt_ok           = &s_rdt_ok;
    rc->active_cuts      = &s_active_cuts;
    rc->active_cut_count = &s_active_cut_count;
    rc->cam_active_cut   = &s_active_cut_idx;
    rc->cam_view         = &s_cam_view;
    rc->load_rdt         = probe_load_rdt;
    rc->reset_render     = probe_reset_render;
    rc->load_bg_cut      = probe_load_bg;
}

/* EM-Bank eines Typs inkl. VICTIM-Bank laden (nach jedem re15_enemy_reset noetig) */
static uint8_t s_em_blob[3][0x80000];
static size_t  s_em_len[3] = {0, 0, 0};
static const uint8_t s_em_types[3] = {0x10, 0x11, 0x16};
static void load_zombie_banks(const char *base)
{
    for (int t = 0; t < 3; t++) {
        if (s_em_len[t] == 0) {
            char emsp[600]; size_t ems_size = 0;
            snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
            uint8_t *ems = read_file(emsp, &ems_size);
            if (!ems) { fprintf(stderr, "WARN: CDEMD0.EMS nicht lesbar\n"); return; }
            int idx = re15_ems_index_for_type(s_em_types[t]);
            size_t off = 0, len = 0;
            if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0 &&
                len <= sizeof s_em_blob[t]) {
                memcpy(s_em_blob[t], ems + off, len);
                s_em_len[t] = len;
            }
            free(ems);
        }
        if (s_em_len[t] == 0) continue;
        re15_enemy_bank_t *eb = re15_enemy_find(s_em_types[t]);
        if (!eb) eb = re15_enemy_alloc(s_em_types[t]);
        if (!eb) continue;
        re15_tim_t tim = (re15_tim_t){0};
        if (re15_emd_parse_container(s_em_blob[t], s_em_len[t], &eb->md1, &eb->skel,
                                     &eb->anim, &tim) == 0) {
            eb->ok = 1; eb->buf = NULL;
            eb->loco_ok = (re15_emd_parse_loco_bank(s_em_blob[t], s_em_len[t],
                              &eb->skel_loco, &eb->anim_loco) == 0);
            eb->own_ok  = (re15_emd_parse_own_bank(s_em_blob[t], s_em_len[t],
                              &eb->skel_own, &eb->anim_own) == 0);
            eb->victim_ok = (re15_emd_parse_victim_bank(s_em_blob[t], s_em_len[t],
                              &eb->skel_victim, &eb->anim_victim) == 0);
        }
    }
}

/* ---- Log ---- */
typedef struct {
    int16_t mo; uint16_t af; uint8_t frac; uint16_t afl;
    uint8_t st, s1, s2, s3, hr;
    int16_t hp; int32_t x, z; int16_t rot;
    int vs, aim, grab;
} psnap_t;

static void ptake(psnap_t *s)
{
    const re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    s->mo = p->motion; s->af = p->anim_frame; s->frac = p->anim_frac;
    s->afl = p->anim_flags;
    s->st = p->state; s->s1 = p->sub_state_1; s->s2 = p->sub_state_2; s->s3 = p->sub_state_3;
    s->hr = p->hit_react; s->hp = p->hp; s->x = p->x; s->z = p->z; s->rot = p->rot_y;
    s->vs = re15_player_victim_state(); s->aim = re15_player_aim_active();
    s->grab = re15_player_is_grabbed();
}
static int pdiff(const psnap_t *a, const psnap_t *b)
{
    return a->mo != b->mo || a->st != b->st || a->s1 != b->s1 || a->s2 != b->s2 ||
           a->s3 != b->s3 || a->hr != b->hr || a->hp != b->hp || a->vs != b->vs ||
           a->aim != b->aim || a->grab != b->grab || a->afl != b->afl;
}
static void pdump(const char *tag, int t, const psnap_t *s, const psnap_t *prev)
{
    long dx = prev ? (long)(s->x - prev->x) : 0, dz = prev ? (long)(s->z - prev->z) : 0;
    printf("%-6s t=%4d mo=%4d af=%3u frac=%u afl=0x%02x st=%u s=%u/%u/%u hr=%u hp=%3d "
           "pos=(%ld,%ld) d=(%ld,%ld) rot=%d vs=%d aim=%d grab=%d\n",
           tag, t, (int)s->mo, s->af, s->frac, s->afl, s->st, s->s1, s->s2, s->s3,
           s->hr, (int)s->hp, (long)s->x, (long)s->z, dx, dz, (int)s->rot,
           s->vs, s->aim, s->grab);
}

static re15_game_ctx_t s_ctx;
static void tick_game(uint16_t pressed, uint16_t held)
{
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = s_rdt_ok;
    s_ctx.cam_view = &s_cam_view; s_ctx.active_cut = s_active_cut_idx;
    s_ctx.pad_pressed = pressed; s_ctx.pad_current = held;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

/* Tuer aus dem AKTUELLEN Raum feuern: DOOR-Slot mit dest != current suchen, Spieler in die
 * Rect stellen, Band angleichen, SQUARE druecken (voller game_step = echter Scan-Pfad),
 * Rotationen durchprobieren (Irons-Memory-Methode). Rueckgabe: Slot oder -1. */
static int fire_first_cross_door(unsigned wanted_room /* 0 = beliebig */)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* Probe-Neutralisierung: Intro-AUTO-Tueren (Null-Rect) feuern im Scan positionslos,
     * sobald ihre Cinematic-/Idle-Bedingung steht, und wuerden die Queue kapern (Lauf 1:
     * slot 2 -> ROOM1240 statt slot 5 -> ROOM1140). Hier interessiert NUR die Rect-Tuer. */
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        re15_aot_t *a = &g_aot.slots[i];
        if (a->active && a->type == RE15_AOT_TYPE_DOOR &&
            a->half_w == 0 && a->half_h == 0)
            a->active = 0;
    }
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active || a->type != RE15_AOT_TYPE_DOOR) continue;
        if (a->half_w == 0 && a->half_h == 0) continue;   /* Auto-/Intro-Tuer (Null-Rect) meiden */
        const re15_aot_door_params_t *d = &g_aot.door_params[i];
        unsigned dest_id = (((unsigned)d->dest_stage + 1u) << 12)
                         | ((unsigned)d->dest_room << 4)
                         | (g_current_room_id & 0x000Fu);
        if (dest_id == g_current_room_id) continue;
        if (wanted_room && dest_id != wanted_room) continue;
        printf("[door] slot=%d rect c=(%ld,%ld) h=(%ld,%ld) band=%d dest=%04X cut=%d "
               "spawn=(%ld,%ld,%ld) yaw=%d\n",
               i, (long)a->x, (long)a->z, (long)a->half_w, (long)a->half_h,
               (int)d->band, dest_id, d->target_cut,
               (long)d->spawn_x, (long)d->spawn_y, (long)d->spawn_z,
               (int)d->spawn_yaw_4096);
        for (int r = 0; r < 4 && !g_room_change.pending; r++) {
            pl->x = a->x; pl->z = a->z; pl->rot_y = (int16_t)(r * 1024);
            re15_collision_set_band((int)d->band);
            tick_game(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
        }
        if (g_room_change.pending) return i;
        printf("[door] slot=%d NICHT gefeuert (4 Rotationen)\n", i);
    }
    return -1;
}

static re15_actor_t *find_type(uint8_t ty)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == ty) return &g_actors[s];
    return NULL;
}
static void park_enemies_except(const re15_actor_t *keep)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *z = &g_actors[s];
        if (!z->active || z == keep) continue;
        z->grid_id = 0x86; z->sub_state_1 = 0; z->sub_state_2 = 0; z->ai_flags = 0;
        z->x = 30000; z->z = 30000;   /* feeding-Sub + dist >> 4000 -> kein Wake/Grab */
    }
}

static void run_phase_log(const char *tag, int nticks, int period)
{
    psnap_t prev, now; memset(&prev, 0xFF, sizeof prev);
    psnap_t last; int have_last = 0;
    for (int t = 0; t < nticks; t++) {
        tick_game(0, 0);
        ptake(&now);
        if (pdiff(&now, &prev)) { pdump(tag, t, &now, have_last ? &last : NULL); prev = now; }
        else if (t % period == 0) pdump("  .", t, &now, have_last ? &last : NULL);
        last = now; have_last = 1;
    }
}

int main(int argc, char **argv)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char variant = (argc > 1) ? argv[1][0] : 'A';
    printf("##### VARIANTE %c (A=Kontrolle, B=Hit, C=Grab) #####\n", variant);

    re15_room_apply_ctx_t rc; make_ctx_room(&rc);

    /* ===== PHASE 0: ROOM1170 booten, Tuer slot5 (flag(4,0xc3)=1) nach ROOM1140 feuern ===== */
    if (probe_load_rdt(0x1170) != 0) return 1;
    s_rdt = g_room_rdt; s_rdt_ok = 1;
    re15_actor_init();
    scd_vm_init();
    re15_game_flag_set(4, 0xc3, 1);           /* Tuer slot5 = DOOR nach ROOM1140 (Else-Zweig) */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 2664; pl->y = -7200; pl->z = -7336; pl->rot_y = 0;
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 8; f++) scd_vm_tick();
    g_scd.player_mode = 0;

    /* Tuer slot5 (dest 1140) NICHT per Scan feuern — der 1170-INTRO-SCD feuert waehrend
     * der Versuchs-Ticks selbst per Aot_on eine Tuer (slot 2 -> ROOM1240) und kapert die
     * Queue (Lauf 1+2 gemessen). Stattdessen die byte-true Door-Params aus der RDT
     * (Door_aot_set slot5: spawn/yaw/cut) direkt in re15_room_request_change geben —
     * exakt die Werte, die aot_fire_door uebergeben wuerde; der Apply-Pfad bleibt echt. */
    int slot = -1;
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active || a->type != RE15_AOT_TYPE_DOOR) continue;
        const re15_aot_door_params_t *d = &g_aot.door_params[i];
        unsigned dest_id = (((unsigned)d->dest_stage + 1u) << 12)
                         | ((unsigned)d->dest_room << 4)
                         | (g_current_room_id & 0x000Fu);
        if (dest_id != 0x1140u) continue;
        printf("[door] 1170 slot=%d -> 1140 spawn=(%ld,%ld,%ld) yaw=%d cut=%d\n",
               i, (long)d->spawn_x, (long)d->spawn_y, (long)d->spawn_z,
               (int)d->spawn_yaw_4096, d->target_cut);
        re15_room_request_change(0x1140u, d->spawn_x, d->spawn_y, d->spawn_z,
                                 d->spawn_yaw_4096, (int)d->target_cut);
        slot = i;
        break;
    }
    if (slot < 0 || !g_room_change.pending) {
        printf("FAIL: keine 1170-Tuer nach 1140 gefunden — Abbruch\n"); return 1;
    }
    printf("[queue] room=%04X spawn=(%ld,%ld,%ld) yaw=%d cut=%d\n",
           g_room_change.room_id, (long)g_room_change.x, (long)g_room_change.y,
           (long)g_room_change.z, (int)g_room_change.yaw_4096, g_room_change.target_cut);
    if (!re15_room_apply_pending(&rc)) { printf("FAIL: apply 1140\n"); return 1; }
    load_zombie_banks(base);                  /* enemy_reset hat die Banks gedroppt */

    /* ===== PHASE 1: in ROOM1140 settlen ===== */
    printf("== ROOM1140 betreten (room=%04X) — 180 Ticks settle ==\n", g_current_room_id);
    run_phase_log("R1140", 180, 60);

    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (variant == 'B') {
        /* Melee-Hit nachstellen: die Live-Angriffe schreiben hp DIREKT (game_step_common.c
         * Flinch-Detector-Kommentar) — der Detector sieht den Drop im naechsten Tick.
         * ALLE Zombies parken (Isolation, room-probe-Gotcha): Lauf 1 zeigte, dass sonst ein
         * echter Grab den plain-Hit-Fall kontaminiert (vs=2/devour vor der Tuer). */
        park_enemies_except(NULL);
        pl->hp = (int16_t)(pl->hp - 10);
        printf("== VARIANTE B: hp -= 10 (hp=%d), Flinch ausklingen lassen ==\n", (int)pl->hp);
        run_phase_log("HIT", 200, 50);
    } else if (variant == 'C') {
        re15_actor_t *z = find_type(0x10);
        if (!z) z = find_type(0x11);
        if (!z) { printf("FAIL: kein Zombie in 1140\n"); return 1; }
        park_enemies_except(z);
        z->x = pl->x + 900; z->z = pl->z;             /* Wake-Distanz < 0xBB8 */
        printf("== VARIANTE C: Zombie slot=%d type=0x%02x an (%ld,%ld) — Grab + Mash ==\n",
               (int)(z - g_actors), z->type, (long)z->x, (long)z->z);
        int grabbed_seen = 0, released_tick = -1;
        psnap_t prev, now; memset(&prev, 0xFF, sizeof prev);
        for (int t = 0; t < 1200; t++) {
            uint16_t mash = (uint16_t)((grabbed_seen || re15_player_is_grabbed())
                                       ? RE15_PAD_BIT_UP : 0);   /* Mash = Press-Edge je Tick */
            tick_game(mash, 0);
            if (re15_player_is_grabbed()) grabbed_seen = 1;
            ptake(&now);
            if (pdiff(&now, &prev)) { pdump("GRAB", t, &now, NULL); prev = now; }
            if (grabbed_seen && !re15_player_is_grabbed() &&
                re15_player_victim_state() == 0) { released_tick = t; break; }
            if (pl->hp <= 0) { printf("FAIL: Spieler im Grab gestorben (Mash zu langsam)\n"); return 1; }
        }
        if (!grabbed_seen) { printf("FAIL: kein Grab in 1200 Ticks\n"); return 1; }
        printf("== Grab beendet (Tick %d, hp=%d) — Zombie parken, ausklingen ==\n",
               released_tick, (int)pl->hp);
        park_enemies_except(NULL);
        run_phase_log("POST", 200, 50);
    } else {
        printf("== VARIANTE A: kein Kampf ==\n");
        run_phase_log("CTRL", 200, 50);
    }

    /* ===== PHASE 2: Tuer aus ROOM1140 raus ===== */
    psnap_t predoor; ptake(&predoor);
    printf("== VOR TUER: "); pdump("PRE", 0, &predoor, NULL);
    g_scd.player_mode = 0;                    /* falls ein Skript den Mode gesetzt hat */
    int slot2 = fire_first_cross_door(0);
    if (slot2 < 0 || !g_room_change.pending) {
        printf("FAIL: keine 1140-Tuer gefeuert — Abbruch\n"); return 1;
    }
    printf("[queue] room=%04X spawn=(%ld,%ld,%ld) yaw=%d cut=%d\n",
           g_room_change.room_id, (long)g_room_change.x, (long)g_room_change.y,
           (long)g_room_change.z, (int)g_room_change.yaw_4096, g_room_change.target_cut);
    if (!re15_room_apply_pending(&rc)) { printf("FAIL: apply Zielraum\n"); return 1; }

    /* ===== PHASE 3: die ersten 120 Ticks im Zielraum ===== */
    printf("== ZIELRAUM %04X — die ersten 120 Ticks ==\n", g_current_room_id);
    {
        psnap_t entry; ptake(&entry);
        printf("   ENTRY (vor Tick 0): "); pdump("E", -1, &entry, NULL);
    }
    run_phase_log("DEST", 120, 10);

    printf("\nBEFUND (%c): hp beim Eintritt=%d — Felder oben vergleichen (A vs B/C).\n",
           variant, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].hp);
    return 0;
}
