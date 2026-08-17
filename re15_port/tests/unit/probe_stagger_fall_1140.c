/* probe_stagger_fall_1140.c — MESSUNG (Nutzer-Report 2026-08-17, nach v0.2.4):
 * "Beim Schiessen und Zurueckfallen auf einen Zombie wurde die Animation drin noch
 *  einmal wiederholt und erst dann ging die Animation weiter und er ist gefallen."
 *
 * Diese Probe BEHAUPTET NICHTS — sie protokolliert pro Tick den kompletten
 * Animations-Zustand des getroffenen Zombies UND den TATSAECHLICH GERENDERTEN
 * Frame-Slot / Keyframe (derselbe Pfad, den main.c faehrt:
 * re15_actor_anim_select -> Loco-Bank-Override -> re15_compute_actor_kf).
 * Ein doppelt durchlaufener Frame-Bereich wird dadurch als wiederholte Slot-Rampe
 * sichtbar; zusaetzlich meldet der Ramp-Detektor am Ende jede Wiederholung.
 *
 * Bewusst NUR Symbole, die es AUCH in v0.2.2 (3a488891) gibt, damit dieselbe
 * Quelldatei im A/B-Worktree gebaut werden kann (re15_actor_playback_slot gibt es
 * erst ab v0.2.4 und wird hier NICHT benutzt).
 *
 * Aufruf:  probe_stagger_fall_1140 <szenario>
 *   idle    — Zombie im SEARCH-STAND (+0x5=0), dann Schuesse bis Knockdown
 *   wander  — Zombie im WANDER      (+0x5=1)
 *   engage  — Zombie im ENGAGE-WALK (+0x5=2)
 *   charge  — Zombie im CHARGE      (+0x5=8)
 *   death   — Zombie mit 1 HP: der Todes-Sturz
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
#include "re15_collision.h"
#include "re15_damage.h"
#include "re15_anim_select.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern int g_test_room_se_log[2048];
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

static void load_bank(const char *base, uint8_t type, uint8_t *scratch, size_t scratch_sz)
{
    char emsp[600]; size_t ems_size = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(emsp, &ems_size);
    if (!ems) return;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0 && len <= scratch_sz) {
        re15_enemy_bank_t *eb = re15_enemy_alloc(type);
        if (eb) {
            memcpy(scratch, ems + off, len);
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(scratch, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok = (re15_emd_parse_loco_bank(scratch, len, &eb->skel_loco, &eb->anim_loco) == 0);
                eb->own_ok  = (re15_emd_parse_own_bank(scratch, len, &eb->skel_own, &eb->anim_own) == 0);
            }
        }
    }
    free(ems);
}

static uint8_t s_b16[0x80000], s_b10[0x80000], s_b11[0x80000];

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

/* ==== RENDER-SICHT — 1:1 der Pfad aus platform/pc/main.c (Anim-Select + Loco-Override) ==== */
static void render_view(const re15_actor_t *a,
                        const re15_emd_animation_t **out_anim,
                        const re15_emd_skeleton_t  **out_skel,
                        int *out_clip_override, int *out_loco)
{
    re15_anim_banks_t banks; memset(&banks, 0, sizeof banks);
    re15_anim_view_t av;
    re15_actor_anim_select(a, 0, &banks, &av);
    const re15_emd_animation_t *an = av.anim;
    const re15_emd_skeleton_t  *sk = av.skel;
    int co = av.clip_override, loco = 0;
    if (re15_actor_uses_loco_bank(a)) {
        re15_enemy_bank_t *lb = re15_enemy_find(a->type);
        if (lb && lb->loco_ok && (int)a->motion < lb->anim_loco.clip_count) {
            sk = &lb->skel_loco; an = &lb->anim_loco; co = (int)a->motion; loco = 1;
        }
    }
    *out_anim = an; *out_skel = sk; *out_clip_override = co; *out_loco = loco;
}

/* Der GERENDERTE Frame-Slot: re15_compute_actor_kf liefert den Keyframe; den Slot
 * rekonstruieren wir per eindeutigem Rueckmatch in der Frame-Tabelle des Clips
 * (bei Mehrdeutigkeit -1 — dann zaehlt der kf-Verlauf). */
static int rendered_slot(const re15_emd_animation_t *an, const re15_emd_skeleton_t *sk,
                         const re15_actor_t *a, int clip_override, int *out_kf, int *out_fc)
{
    *out_kf = -1; *out_fc = 0;
    if (!an || !sk || an->clip_count <= 0) return -1;
    int ci = (clip_override >= 0) ? clip_override : (int)a->motion;
    if (ci < 0) ci = 0;
    if (ci >= an->clip_count) ci = ci % an->clip_count;
    const re15_emd_clip_t *c = &an->clips[ci];
    *out_fc = c->frame_count;
    if (c->frame_count <= 0) return -1;
    int kf = re15_compute_actor_kf(an, sk, a, clip_override, (uint32_t)a->anim_frame);
    *out_kf = kf;
    /* Slot 1:1 nach der Zweig-Kette von re15_compute_actor_kf (in v0.2.2 UND v0.2.4
     * textgleich — der v0.2.4-Refactor auf re15_actor_playback_slot ist ein No-Op). */
    uint32_t cur = (uint32_t)a->anim_frame;
    int fc = c->frame_count;
    int rev = ((a->anim_flags & 0x80) || re15_actor_toggle_reverse(a)) ? 1 : 0;
    int slot;
    if (a->anim_freeze) slot = 0;
    else if (a->walk_active || clip_override >= 0) {
        uint32_t m = cur % (uint32_t)fc;
        slot = rev ? (int)((uint32_t)(fc - 1) - m) : (int)m;
    } else if (rev) {
        uint32_t idx = (cur >= (uint32_t)fc) ? (uint32_t)(fc - 1) : cur;
        slot = (int)((uint32_t)(fc - 1) - idx);
    } else if (cur >= (uint32_t)fc) {
        int last = fc - 1;
        while (last > 0 && (an->frames[c->first_frame + last] & 0x8000u)) last--;
        slot = last;
    } else slot = (int)cur;
    return slot;
}

/* ==== RAMP-DETEKTOR ==========================================================================
 * Zwei getrennte Befunde:
 *  (A) WRAP      — Slot faellt von fc-1 auf 0. Das ist der byte-true Loop (@0x8001f63c) und
 *                  KEIN Defekt; nur gezaehlt.
 *  (B) RESTART   — Slot faellt MITTEN im Clip zurueck (Quelle != fc-1 oder Ziel != 0), waehrend
 *                  State+Clip+Bank unveraendert bleiben. GENAU das sieht ein Spieler als
 *                  "die Animation wurde noch einmal wiederholt". */
#define MAXT 2000
static int     s_kf[MAXT], s_slot[MAXT], s_fc[MAXT], s_tick[MAXT];
static uint8_t s_mo[MAXT], s_st[MAXT], s_bank[MAXT];
static int     s_n = 0;

static void ramp_report(void)
{
    printf("\n===== RAMP-DETEKTOR =====\n");
    int wraps = 0, restarts = 0;
    for (int k = 1; k < s_n; k++) {
        if (s_mo[k] != s_mo[k-1] || s_st[k] != s_st[k-1] || s_bank[k] != s_bank[k-1]) continue;
        if (s_slot[k] < 0 || s_slot[k-1] < 0) continue;
        if (s_slot[k] >= s_slot[k-1]) continue;                 /* vorwaerts */
        if (s_slot[k-1] == s_fc[k]-1 && s_slot[k] == 0) { wraps++; continue; }   /* (A) */
        printf("  RESTART t=%d: mo=%d st=%d fc=%d  slot %d -> %d (kf %d -> %d)\n",
               s_tick[k], s_mo[k], s_st[k], s_fc[k],
               s_slot[k-1], s_slot[k], s_kf[k-1], s_kf[k]);
        restarts++;
    }
    printf("  Summe: %d byte-true Wraps (fc-1 -> 0), %d RESTARTS mitten im Clip\n",
           wraps, restarts);
}

static void dump_line(int t, const re15_actor_t *z, const char *tag, int se_from)
{
    const re15_emd_animation_t *an; const re15_emd_skeleton_t *sk;
    int co, loco, kf, fc;
    render_view(z, &an, &sk, &co, &loco);
    int slot = rendered_slot(an, sk, z, co, &kf, &fc);
    printf("t=%4d %-7s st=%u s1=0x%02x s2=%u s3=%u mo=%3d af=%3u frac=%2u flg=%04x grid=%02x "
           "stun=%4d hp=%5d | %s co=%3d fc=%3d slot=%3d kf=%4d fcclk=%3d",
           t, tag, z->state, z->sub_state_1, z->sub_state_2, z->sub_state_3,
           (int)z->motion, z->anim_frame, z->anim_frac, z->anim_flags, z->grid_id,
           (int)z->hit_stun, (int)z->hp,
           loco ? "LOCO" : "ACT ", co, fc, slot, kf, re15_actor_clip_len(z));
    if (g_test_room_se_n > se_from) {
        printf("  SE:");
        for (int i = se_from; i < g_test_room_se_n; i++) printf(" %d", g_test_room_se_log[i]);
    }
    printf("\n");
    if (s_n < MAXT) { s_kf[s_n] = kf; s_slot[s_n] = slot; s_fc[s_n] = fc; s_tick[s_n] = t;
                      s_mo[s_n] = (uint8_t)z->motion; s_st[s_n] = z->state;
                      s_bank[s_n] = (uint8_t)loco; s_n++; }
}

int main(int argc, char **argv)
{
    const char *sc = (argc > 1) ? argv[1] : "idle";
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600]; size_t sz = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", base);
    uint8_t *d = read_file(path, &sz);
    if (!d) { fprintf(stderr, "FAIL: RDT\n"); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(d, sz, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }

    re15_damage_seed_rng(0x12345678u);
    re15_actor_init(); scd_vm_init(); re15_enemy_reset();
    g_current_room_id = 0x1140;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 0; pl->y = 0; pl->z = 0; pl->rot_y = 0;
    /* "feed*" = ERSTBESUCH (liegende/fressende Zombies, grid 0x86/0x88);
     * alles andere = Wiedereintritts-Zweig (STEHENDE Schlaefer, grid 0x02). */
    if (strncmp(sc, "feed", 4) != 0)
        re15_game_flag_set(3, 0xD2, 1);
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    load_bank(base, 0x16, s_b16, sizeof s_b16);
    load_bank(base, 0x10, s_b10, sizeof s_b10);
    load_bank(base, 0x11, s_b11, sizeof s_b11);
    for (int f = 0; f < 8; f++) tick_once();

    re15_actor_t *z = NULL;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *c = &g_actors[s];
        if (c->active && (c->type == 0x10 || c->type == 0x11 || c->type == 0x16)) { z = c; break; }
    }
    if (!z) { fprintf(stderr, "FAIL: kein Zombie\n"); return 1; }
    /* alle anderen Gegner stilllegen — nur EIN Aktor, damit die Sequenz eindeutig ist */
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (&g_actors[s] != z && g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type <= 0x18)
            g_actors[s].active = 0;

    { re15_enemy_bank_t *b = re15_enemy_find(z->type);
      printf("== Bank Typ 0x%02x: ACT clips=%d  LOCO ok=%d clips=%d\n",
             z->type, b ? b->anim.clip_count : -1, b ? b->loco_ok : -1,
             (b && b->loco_ok) ? b->anim_loco.clip_count : -1);
      if (b) {
          printf("   ACT  fc:"); for (int c = 0; c < 6 && c < b->anim.clip_count; c++) printf(" [%d]=%d", c, b->anim.clips[c].frame_count);
          printf("  |  0x0b=%d 0x0d=%d 0x11=%d 0x12=%d 0x1f=%d\n",
                 b->anim.clips[0x0b].frame_count, b->anim.clips[0x0d].frame_count,
                 b->anim.clips[0x11].frame_count, b->anim.clips[0x12].frame_count,
                 b->anim.clips[0x1f].frame_count);
          if (b->loco_ok) { printf("   LOCO fc:");
              for (int c = 0; c < b->anim_loco.clip_count; c++) printf(" [%d]=%d", c, b->anim_loco.clips[c].frame_count);
              printf("\n"); }
      } }

    /* "natural": KEIN Zustands-Eingriff — der Spieler laeuft heran, die echte KI entscheidet
     * (Grab/Engage/Turn/Approach/Stagger/Knockdown), es wird im Takt geschossen. */
    if (!strcmp(sc, "natural")) {
        int32_t tx = z->x, tz = z->z + 1400;
        for (int i = 0; i < 300; i++) {
            int32_t dx = tx - pl->x, dz = tz - pl->z;
            if (dx >  40) dx =  40; if (dx < -40) dx = -40;
            if (dz >  40) dz =  40; if (dz < -40) dz = -40;
            pl->x += dx; pl->z += dz; tick_once();
        }
    }
    /* Spieler in feste SCHUSSDISTANZ (weit genug: KEIN Grab, kein Kontakt) */
    if (strcmp(sc, "natural") != 0) {
    pl->x = z->x; pl->z = z->z + (!strcmp(sc, "feedwake") ? 1200 : 3000); pl->rot_y = 0;
    }
    for (int f = 0; f < 4; f++) tick_once();

    /* ==== MODE "sweep": VOLLSTAENDIGER REGEL-DELTA-DUMP (Bank + Clip-Uhr) ====
     * Ein Zeile je (Typ, State, +0x5, Grid-Nibble, Clip). Der Diff alt/neu listet damit
     * LUECKENLOS jede Zustands-Kombination, deren Pose-Bank oder Clip-Uhr sich geaendert hat. */
    if (!strcmp(sc, "sweep")) {
        static const uint8_t types[] = { 0x10, 0x11, 0x12, 0x13, 0x16, 0x18 };
        re15_actor_t a;
        for (unsigned ti = 0; ti < sizeof types; ti++) {
            if (!re15_enemy_find(types[ti])) continue;
            for (int st = 0; st <= 3; st++)
            for (int s1 = 0; s1 <= 0x14; s1++)
            for (int gn = 0; gn <= 10; gn++)
            for (int mo = 0; mo <= 6; mo++) {
                memset(&a, 0, sizeof a);
                a.active = 1; a.type = types[ti]; a.state = (uint8_t)st;
                a.sub_state_1 = (uint8_t)s1; a.grid_id = (uint8_t)gn; a.motion = (uint8_t)mo;
                printf("T%02x S%d s%02x g%02x m%d -> loco=%d fc=%d\n",
                       types[ti], st, s1, gn, mo,
                       re15_actor_uses_loco_bank(&a), re15_actor_clip_len(&a));
            }
        }
        return 0;
    }

    /* Szenario-Zustand setzen (PROBEN-HARNISCH — setzt genau die Felder, die der
     * jeweilige Original-Entry auch setzt; +0x6=0 laesst den Entry-Block laufen). */
    int hp = 4000;
    if      (!strcmp(sc, "idle"))   { z->state = 1; z->sub_state_1 = 0x00; z->sub_state_2 = 0; }
    else if (!strcmp(sc, "wander")) { z->state = 1; z->sub_state_1 = 0x01; z->sub_state_2 = 0; }
    else if (!strcmp(sc, "engage")) { z->state = 1; z->sub_state_1 = 0x02; z->sub_state_2 = 0; }
    else if (!strcmp(sc, "charge")) { z->state = 1; z->sub_state_1 = 0x08; z->sub_state_2 = 0; }
    else if (!strcmp(sc, "death"))  { z->state = 1; z->sub_state_1 = 0x02; z->sub_state_2 = 0; hp = 1; }
    /* GRAB-RECOVERY [6]/[7]: der "Zurueckfaller" — Clip 0x11 (60f) RUECKWAERTS
     * (f314 a2=1, a3=0x100 @0x80102a64ff). Direkt in Phase 6 eingesetzt. */
    else if (!strcmp(sc, "recover")) { z->state = 1; z->sub_state_1 = 3; z->sub_state_2 = 6; }
    /* NUTZER-FALL: WAEHREND des Zurueckfallens (Grab-Recovery [7], Clip 0x11 rueckwaerts)
     * wird geschossen. Der Grab-Handler kommt dann nie zu [8] und laesst das Rueckwaerts-Bit
     * stehen — Stagger/Sturz/Aufstehen laufen danach RUECKWAERTS. */
    else if (!strcmp(sc, "recovershot")) { z->state = 1; z->sub_state_1 = 3; z->sub_state_2 = 6;
                                           z->anim_flags &= (uint16_t)~0x04u; }
    /* PUSH-OFF (+0x5=0xb, FUN_8010385c): Einmal-Clip 0x10 + Rueckwaertsrutsch. */
    else if (!strcmp(sc, "pushoff")) { z->state = 1; z->sub_state_1 = 0x0b; z->sub_state_2 = 0; }
    if (strncmp(sc, "feed", 4) != 0) z->sub_state_3 = 0;   /* feed*: Spawn-Zustand unangetastet */
    z->hp = (int16_t)hp;
    printf("== Szenario '%s': st=%u s1=0x%02x grid=%02x poise=%d hp=%d\n",
           sc, z->state, z->sub_state_1, z->grid_id, (int)z->hit_stun, (int)z->hp);

    /* 20 Ticks im Zustand laufen lassen, DANN schiessen */
    printf("\n===== VORLAUF (20 Ticks) + SCHUSS -> TAUMEL -> STURZ =====\n");
    g_test_room_se_n = 0;
    int t = 0, shots = 0, knock = 0, dead = 0, post = 0;
    int no_shot = (!strcmp(sc, "recover") || !strcmp(sc, "pushoff"));
    int tmax = no_shot ? 200 : 900;
    int natural = !strcmp(sc, "natural");
    for (; t < tmax; t++) {
        int se_from = g_test_room_se_n;
        const char *tag = "";
        if (!no_shot && t >= (!strcmp(sc, "recovershot") ? 10 : 20) && !knock && !dead &&
            ((t - (!strcmp(sc, "recovershot") ? 10 : 20)) % 12) == 0 && shots < 20) {
            z->hit_react &= (uint8_t)~2u;
            (void)re15_enemy_take_damage(z, 0);
            if (strcmp(sc, "death") != 0) z->hp = (int16_t)hp;
            shots++;
            tag = "SCHUSS";
        }
        g_test_room_se_tick = t;
        tick_once();
        if (z->state == 1 && z->sub_state_1 == 0x11) knock = 1;
        if (z->state == 3 || z->state == 7) dead = 1;
        dump_line(t, z, tag, se_from);
        if (dead) { if (++post > 90) break; }
        if (!no_shot && !natural && knock && z->sub_state_1 == 0x11 && z->sub_state_2 >= 2) break;
    }
    printf("== Ende t=%d shots=%d st=%u s1=0x%02x s2=%u mo=%d\n",
           t, shots, z->state, z->sub_state_1, z->sub_state_2, (int)z->motion);
    ramp_report();
    return 0;
}
