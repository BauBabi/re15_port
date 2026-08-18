/* probe_10d0_knockse.c — MESSUNG (Nutzer-Report 2026-08-17, ZWEITMELDUNG):
 * "2F-Korridor (ROOM10D0): Zombie-Sound beim Hinfallen nach mehreren Schuessen und
 *  beim Aufstehen nicht korrekt."
 *
 * Gemessen wird der RE1.5-PFAD (Default-KI, NICHT RE2). Erfasst werden ALLE
 * snd1-SEs (FUN_800453d0 -> re15_audio_room_se) mit Tick, Zustand, Clip und Frame.
 *
 * ORIGINAL-SOLL (selbst disassembliert, STAGE1.BIN roh @0x80100000):
 *   HURT-Router FUN_80105a8c -> Steh-Stagger FUN_80105b7c Phase 0: SE 6 UNBEDINGT
 *     (@0x80105cf4 jal 0x800453d0, a0=6)
 *   Poise +0x1dc unter 0 -> +0x4=1 / +0x5=0x11 (@0x80105b48-58) = KNOCKDOWN
 *   KNOCKDOWN FUN_8010512c, Phasentabelle @0x801000c4:
 *     [0] @0x8010516c  Clip 0x0b, +0x8f=0xf, SE-Wurf 1/4 -> (rand&1)?5:8
 *                      (@0x801051c4 rng / andi 3 / bne @0x801051d0 / rng /
 *                       andi 1 @0x801051e0 / @0x801051e8 ori a0,8 (Delay) /
 *                       @0x801051ec ori a0,5 / @0x801051f0 jal 0x800453d0)
 *     [1] @0x80105234  f314 rate 0x100 -> +0x6 += done
 *     [2] @0x80105278  Liege-Timer +0x9c = tbl@0x8011fb10[rand&0xf]*30
 *     [3] @0x801052f8  Countdown — KEIN f314, KEIN SE
 *     [4] @0x8010532c  Clip 0x12, SE 8 DETERMINISTISCH (@0x801053bc): a0 == 0x20000000
 *                      (FUN_80012974 @0x800129d4, im DOWNED-Zustand immer gerufen
 *                       @0x80101638) -> FUN_8001af20 gibt 0 -> beide Wuerfe 0
 *     [5] @0x801053c4  f314 -> +0x6 += done
 *     [6] @0x80105400  Wort 0x201 (ENGAGE), Poise neu
 *   FRAME-WORT-SE-DEKODER FUN_8001b38c: `srl s0,v0,22` (@0x8001b3b4) -> Bit N = SE N,
 *     gerufen UNBEDINGT im Wurzel-Tail @0x80100634 (FUN_80100424) in JEDEM Zustand.
 *     => Die Fall-/Aufsteh-Geraeusche kommen NICHT aus dem Handler, sondern aus den
 *        Frame-Woertern des jeweiligen Clips.
 *
 * Diese Probe misst: (a) die Frame-Wort-Tabellen der Clips 0x0b/0x12 wie der PORT sie
 * sieht, (b) welche SEs der Port beim Knockdown/Getup in welchem Tick/Frame feuert.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_damage.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern int g_test_room_se_log[2048];
extern int g_test_room_se_tickof[2048];
extern int g_test_room_se_n;
extern int g_test_room_se_tick;

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

static uint8_t s_b10[0x80000];
static void load_em10(const char *base)
{
    char emsp[600]; size_t ems_size = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(emsp, &ems_size);
    if (!ems) { fprintf(stderr, "FAIL: CDEMD0.EMS\n"); exit(1); }
    int idx = re15_ems_index_for_type(0x10);
    size_t off = 0, len = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0 && len <= sizeof s_b10) {
        re15_enemy_bank_t *eb = re15_enemy_alloc(0x10);
        if (eb) {
            memcpy(s_b10, ems + off, len);
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(s_b10, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok = (re15_emd_parse_loco_bank(s_b10, len, &eb->skel_loco, &eb->anim_loco) == 0);
                eb->own_ok  = (re15_emd_parse_own_bank(s_b10, len, &eb->skel_own, &eb->anim_own) == 0);
            }
        }
    }
    free(ems);
}

/* Beide Baenke nebeneinander: das Original posiert Idle/Wander/Stagger/Charge aus BANK0
 * (`lw a0,132(v0)` / `lw a1,364(v0)` @0x80101da8 / @0x80102000 / @0x80105d44 / @0x80103128),
 * Knockdown/Getup/Grab/Downed aus BANK1 (`lw a0,368` / `lw a1,372` @0x80105248 / @0x801053d8). */
static void dump_clip_bank(int clip, int loco)
{
    re15_enemy_bank_t *b = re15_enemy_find(0x10);
    if (!b || !b->ok) { printf("  (keine Bank)\n"); return; }
    if (loco && !b->loco_ok) { printf("  (keine LOCO-Bank)\n"); return; }
    const re15_emd_animation_t *A = loco ? &b->anim_loco : &b->anim;
    if (clip >= A->clip_count) {
        printf("[%s] Clip 0x%02x  -> existiert nicht (clip_count %d)\n",
               loco ? "LOCO" : "ACT ", clip, A->clip_count);
        return;
    }
    const re15_emd_clip_t *c = &A->clips[clip];
    printf("[%s] Clip 0x%02x fc=%-3d ->", loco ? "LOCO" : "ACT ", clip, c->frame_count);
    int any = 0;
    for (int f = 0; f < c->frame_count; f++) {
        uint32_t m = A->frames[c->first_frame + f] >> 22;
        if (m) { printf("  f%d:SE", f);
                 for (int bit = 0; bit < 10; bit++) if (m & (1u << bit)) printf("%d", bit);
                 any = 1; }
    }
    if (!any) printf("  (keine SE-Bits)");
    printf("\n");
}

static void dump_clip_framewords(int clip)
{
    re15_enemy_bank_t *b = re15_enemy_find(0x10);
    if (!b || !b->ok) { printf("  (keine Bank)\n"); return; }
    const re15_emd_animation_t *A = &b->anim;
    if (clip >= A->clip_count) { printf("  Clip 0x%02x > clip_count %d\n", clip, A->clip_count); return; }
    const re15_emd_clip_t *c = &A->clips[clip];
    printf("Clip 0x%02x  fc=%d  first=%d   SE-Bits (Wort>>22):\n", clip, c->frame_count, c->first_frame);
    int any = 0;
    for (int f = 0; f < c->frame_count; f++) {
        uint32_t fw = A->frames[c->first_frame + f];
        uint32_t m  = fw >> 22;
        if (m) { printf("   f%-3d word=0x%08x mask=0x%03x -> SE", f, fw, m);
                 for (int bit = 0; bit < 10; bit++) if (m & (1u<<bit)) printf(" %d", bit);
                 printf("\n"); any = 1; }
    }
    if (!any) printf("   (kein einziges SE-Bit)\n");
}

static void tick_once(void)
{
    re15_aot_scan(g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z, 0);
    if (g_aot.fired_event_id_this_frame != 0)
        (void)scd_event_fire(g_aot.fired_event_id_this_frame);
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_actors_anim_advance();
    re15_enemy_ai_run_all(0);
}

static re15_actor_t *find_z(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x10) return &g_actors[s];
    return NULL;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600]; size_t sz = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM10D0.RDT", base);
    uint8_t *d = read_file(path, &sz);
    if (!d) { fprintf(stderr, "FAIL: RDT\n"); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(d, sz, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }

    re15_actor_init(); scd_vm_init();
    g_current_room_id = 0x10D0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 7650; pl->y = 0; pl->z = 11400; pl->rot_y = 2048;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    load_em10(base);
    for (int f = 0; f < 8; f++) tick_once();

    printf("===== FRAME-WORT-TABELLEN (Port-Sicht, Bank 0 = Aktions-Bank) =====\n");
    dump_clip_framewords(0x0b);   /* FALL   */
    dump_clip_framewords(0x12);   /* GET-UP */
    dump_clip_framewords(0x0d);   /* Steh-Tod (Kontrolle) */
    printf("\n===== BANK-VERGLEICH Clips 0..5 =====\n");
    for (int cl = 0; cl <= 5; cl++) { dump_clip_bank(cl, 1); dump_clip_bank(cl, 0); }
    { re15_enemy_bank_t *b = re15_enemy_find(0x10);
      printf("Bank: anim.clip_count=%d loco_ok=%d loco.clip_count=%d own_ok=%d\n",
             b->anim.clip_count, b->loco_ok, b->loco_ok ? b->anim_loco.clip_count : -1, b->own_ok); }

    re15_actor_t *z = find_z();
    if (!z) { fprintf(stderr, "FAIL: kein Zombie\n"); return 1; }
    printf("\n===== ZOMBIE @Start: slot type=0x%02x grid=0x%02x st=%u s1=0x%02x hp=%d =====\n",
           z->type, z->grid_id, z->state, z->sub_state_1, (int)z->hp);

    /* Zombie aufwecken: Spieler nah heran (der Liege-Schlaefer wacht bei dist<0xBB8). */
    { int32_t tx = z->x, tz = z->z + 1500;
      for (int i = 0; i < 500; i++) {
        int32_t dx = tx - pl->x, dz = tz - pl->z;
        if (dx > 40) dx = 40; if (dx < -40) dx = -40;
        if (dz > 40) dz = 40; if (dz < -40) dz = -40;
        pl->x += dx; pl->z += dz; tick_once(); } }
    printf("nach Wecken: st=%u s1=0x%02x s2=%u mo=%d hp=%d poise=%d grid=0x%02x\n",
           z->state, z->sub_state_1, z->sub_state_2, (int)z->motion, (int)z->hp,
           (int)z->hit_stun, z->grid_id);

    /* ===== SCHUSS-SERIE: Handfeuerwaffe, bis der Poise bricht ===== */
    g_test_room_se_n = 0;
    for (int shot = 0; shot < 6; shot++) {
        int before_n = g_test_room_se_n;
        g_test_room_se_tick = -(shot + 1);           /* negativer Stempel = Schuss-Tick */
        z->hit_react &= (uint8_t)~2u;                /* neues Angriffsfenster */
        (void)re15_enemy_take_damage(z, 0);          /* attack_type 0 = Handfeuerwaffe */
        printf("[Schuss %d] hp=%d poise=%d st=%u s1=0x%02x s2=%u mo=%d fr=%u  SE:",
               shot, (int)z->hp, (int)z->hit_stun, z->state, z->sub_state_1,
               z->sub_state_2, (int)z->motion, z->anim_frame);
        for (int i = before_n; i < g_test_room_se_n; i++) printf(" %d", g_test_room_se_log[i]);
        printf("\n");
        /* ein paar Ticks laufen lassen, damit der Stagger anlaeuft */
        for (int t = 0; t < 40; t++) {
            g_test_room_se_tick = shot * 100 + t;
            int b4 = g_test_room_se_n;
            tick_once();
            if (g_test_room_se_n != b4) {
                printf("   t=%3d st=%u s1=0x%02x s2=%u mo=%-3d fr=%-3u  SE:",
                       t, z->state, z->sub_state_1, z->sub_state_2, (int)z->motion, z->anim_frame);
                for (int i = b4; i < g_test_room_se_n; i++) printf(" %d", g_test_room_se_log[i]);
                printf("\n");
            }
        }
        if (z->state == 1 && z->sub_state_1 == 0x11) { printf("  -> KNOCKDOWN erreicht\n"); break; }
        if (z->hp < 0) { printf("  -> tot\n"); break; }
    }

    /* ===== KNOCKDOWN + GETUP durchlaufen lassen ===== */
    printf("\n===== KNOCKDOWN/GETUP-VERLAUF =====\n");
    g_test_room_se_n = 0;
    uint8_t p_s1 = 0xff, p_s2 = 0xff, p_mo = 0xff;
    for (int t = 0; t < 1400; t++) {
        g_test_room_se_tick = t;
        int b4 = g_test_room_se_n;
        tick_once();
        int se_here = (g_test_room_se_n != b4);
        if (z->sub_state_1 != p_s1 || z->sub_state_2 != p_s2 || (uint8_t)z->motion != p_mo || se_here) {
            printf("t=%4d st=%u s1=0x%02x s2=%u mo=%-3d fr=%-3u frac=%-3u af=%04x tmr=%-5d",
                   t, z->state, z->sub_state_1, z->sub_state_2, (int)z->motion,
                   z->anim_frame, z->anim_frac, z->anim_flags, (int)z->ai_timer);
            if (se_here) { printf("   SE:");
                for (int i = b4; i < g_test_room_se_n; i++) printf(" %d", g_test_room_se_log[i]); }
            printf("\n");
            p_s1 = z->sub_state_1; p_s2 = z->sub_state_2; p_mo = (uint8_t)z->motion;
        }
        if (z->state == 1 && z->sub_state_1 == 2 && t > 50) { printf("(zurueck in ENGAGE @t=%d)\n", t); break; }
    }
    return 0;
}
