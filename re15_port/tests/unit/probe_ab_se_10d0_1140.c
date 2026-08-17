/* probe_ab_se_10d0_1140.c — A/B-MESSUNG (Nutzer-Report 2026-08-17, DRITTMELDUNG):
 *
 *   "Der Zombie im 2F-Korridor (ROOM10D0), der am Boden liegt — nach der Cutscene mit
 *    Marvin — macht ANDERE Geraeusche, wenn er getroffen wird, zu Boden geht und wieder
 *    aufsteht, als die Zombies im Dinner-Raum (ROOM1140)."
 *
 * Der Nutzer liefert damit ein ORAKEL: 1140 = richtig, 10D0 = falsch, GLEICHE Handlung.
 * Diese Probe faehrt BEIDE Faelle im SELBEN Binary mit IDENTISCHEM Ablauf und
 * IDENTISCHEM RNG-Seed und protokolliert jede einzelne FUN_800453d0-SE mit
 *   Tick | SE-ID | state/+0x5/+0x6/+0x7 | posierte Bank | Clip | Frame | Quelle
 * wobei "Quelle" post-hoc bestimmt wird: steht das SE-Bit im Frame-Wort der GERADE
 * posierten Bank/Clip/Frame (FUN_8001b38c `srl s0,v0,22` @0x8001b3b4), ist es ein
 * FRAME-WORT-SE; sonst ein direkter Handler-Call.
 *
 * Ablauf pro Fall (identisch):
 *   1. RDT laden, scd_room_reenter, Banks 0x10/0x11/0x16 laden, 8 Ticks settlen
 *   2. Ziel-Zombie waehlen (10D0: der Liegende; 1140: der per --slot gewaehlte)
 *   3. Spieler in 40er-Schritten heranfahren bis Wake/Engage (max 900 Ticks)
 *   4. RNG neu seeden (identisch), dann Schuss-Serie (attack_type 0) bis KNOCKDOWN
 *   5. Knockdown -> Liegen -> Aufstehen -> ENGAGE komplett durchticken
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
#include "re15_vab.h"

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

static uint8_t s_b10[0x80000], s_b11[0x80000], s_b16[0x80000];
static void load_bank(const char *base, uint8_t type, uint8_t *scratch, size_t cap)
{
    char emsp[600]; size_t n = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(emsp, &n);
    if (!ems) return;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= cap) {
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
static void load_banks(const char *base)
{
    load_bank(base, 0x10, s_b10, sizeof s_b10);
    load_bank(base, 0x11, s_b11, sizeof s_b11);
    load_bank(base, 0x16, s_b16, sizeof s_b16);
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

/* Frame-Wort-Maske der GERADE posierten Bank (dieselbe Regel wie Render/Uhr/SFX). */
static uint32_t posed_mask(const re15_actor_t *e, int *out_loco, int *out_fc)
{
    *out_loco = 0; *out_fc = 0;
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || !b->ok) return 0;
    const re15_emd_animation_t *A = &b->anim;
    if (re15_actor_uses_loco_bank(e) && b->loco_ok && (int)e->motion < b->anim_loco.clip_count) {
        A = &b->anim_loco; *out_loco = 1;
    }
    if ((int)e->motion >= A->clip_count) return 0;
    const re15_emd_clip_t *c = &A->clips[e->motion];
    if (c->frame_count <= 0) return 0;
    *out_fc = c->frame_count;
    return A->frames[c->first_frame + (e->anim_frame % (uint32_t)c->frame_count)] >> 22;
}

static void report(const char *tag, int t, const re15_actor_t *z, int from, int to)
{
    int loco = 0, fc = 0;
    uint32_t m = posed_mask(z, &loco, &fc);
    for (int i = from; i < to; i++) {
        int id = g_test_room_se_log[i];
        int fw = (id >= 0 && id < 10 && (m & (1u << id))) ? 1 : 0;
        printf("  %-5s t=%-5d SE=%-2d  st=%u s1=0x%02x s2=%-2u s3=%-2u grid=0x%02x "
               "bank=%s clip=%-3d fr=%-3u/%-3d  %s\n",
               tag, t, id, z->state, z->sub_state_1, z->sub_state_2, z->sub_state_3,
               z->grid_id, loco ? "LOCO" : "ACT ", (int)z->motion, z->anim_frame, fc,
               fw ? "<- Frame-Wort" : "<- Handler-Call");
    }
}

typedef struct { const char *room; unsigned id; int32_t px, pz; int16_t pyaw; int want_slot;
                 int marvin_seen; } caseinfo_t;

static void run_case(const char *base, const caseinfo_t *ci, const char *tag)
{
    char path[600]; size_t sz = 0;
    snprintf(path, sizeof path, "%s/STAGE1/%s.RDT", base, ci->room);
    uint8_t *d = read_file(path, &sz);
    if (!d) { printf("FAIL: %s nicht ladbar\n", ci->room); return; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(d, sz, &rdt) != 0) { printf("FAIL: %s parse\n", ci->room); return; }

    re15_actor_init(); scd_vm_init(); re15_enemy_reset();
    /* Marvin-Cutscene gesehen = Flag (4,247), gesetzt von sub21 +0x07e0 (Datei 0x19ec) ->
     * der ELSE-Zweig von sub00 spawnt DENSELBEN Zombie als STEHENDEN behavior-0x02-Schlaefer
     * (Datei 0x12a4: `44 00 10 02 00 00 00 cd`, Grid-Nibble 2). */
    if (ci->marvin_seen) re15_game_flag_set(4, 247, 1);
    g_current_room_id = ci->id;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = ci->px; pl->y = 0; pl->z = ci->pz; pl->rot_y = ci->pyaw;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    load_banks(base);
    re15_damage_seed_rng(0x13572468u);
    for (int f = 0; f < 8; f++) tick_once();

    printf("\n########## %s (%s) ##########\n", tag, ci->room);
    /* Der ECHTE Raum-SE-Pfad des Ports (audio_pc.c load_room_se_vab_pc + se_play_layers):
     * snd1-Scheibe aus der RDT -> re15_vab_parse -> re15_edt_resolve_layers_ex.
     * Zeigt, welches VAG/Pitch der Zombie-SE in DIESEM Raum wirklich trifft. */
    {
        static re15_vab_t vab;
        const uint8_t *vh = rdt.snd_vh[1], *vb = rdt.snd_vb[1], *edt = rdt.snd_edt[1];
        int vhsz = rdt.snd_vh_size[1], vbsz = rdt.snd_vb_size[1];
        int pr = (vh && edt) ? re15_vab_parse(vh, (size_t)vhsz, &vab) : -99;
        printf("snd1: vh=%p(%d) vb=%p(%d) edt=%p  parse=%d progs=%d vags=%d vb_total=%d\n",
               (const void *)vh, vhsz, (const void *)vb, vbsz, (const void *)edt,
               pr, vab.program_count, vab.vag_count, vab.vb_total_bytes);
        if (pr == 0) {
            for (int se = 0; se <= 8; se++) {
                int vags[8], tones[8];
                int n = re15_edt_resolve_layers_ex(edt, &vab, se, vags, tones, 8);
                printf("   SE%-2d -> layers=%d", se, n);
                for (int k = 0; k < n; k++) {
                    const re15_vab_tone_t *t = &vab.tones[tones[k]];
                    printf("  vag=%d sz=%u pitch=0x%03x vol=%u",
                           vags[k], vags[k] >= 0 ? vab.samples[vags[k]].size : 0,
                           re15_vab_note2pitch2(t->min_note, t->pitch_shift,
                                                t->center_note, t->pitch_shift), t->vol);
                }
                printf("\n");
            }
        }
    }
    printf("Roster:\n");
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *a = &g_actors[s];
        if (!a->active) continue;
        printf("  slot=%d type=0x%02x grid=0x%02x st=%u s1=0x%02x mo=%d pos=(%ld,%ld,%ld)\n",
               s, a->type, a->grid_id, a->state, a->sub_state_1, (int)a->motion,
               (long)a->x, (long)a->y, (long)a->z);
    }
    re15_actor_t *z = NULL;
    if (ci->want_slot >= 0 && g_actors[ci->want_slot].active) z = &g_actors[ci->want_slot];
    if (!z) for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type <= 0x18)
            { z = &g_actors[s]; break; }
    if (!z) { printf("FAIL: kein Zombie\n"); return; }
    int zslot = (int)(z - g_actors);
    printf("ZIEL: slot=%d type=0x%02x grid=0x%02x hurt_clip=%d poise=%d hp=%d\n",
           zslot, z->type, z->grid_id, (int)z->hurt_clip, (int)z->hit_stun, (int)z->hp);
    /* ISOLATION: der SE-Spion ist global — jeder andere Gegner im Raum (1140 hat vier
     * fressende Zombies, die per Chomp-SE 3 dazwischenfunken) wuerde die Kette
     * verfaelschen. Fuer den A/B bleibt NUR das Ziel aktiv. */
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (s != zslot && g_actors[s].active) g_actors[s].active = 0;

    /* --- heranfahren bis der Zombie steht/engaged (WACH-PHASE mitprotokolliert) --- */
    g_test_room_se_n = 0;
    printf("--- WACH-PHASE (Liegen -> Aufwachen -> Aufstehen -> Engage) ---\n");
    { int32_t tx = z->x + 2600, tz = z->z + 2600;
      uint8_t w1 = 0xff, w2 = 0xff, wm = 0xff;
      for (int i = 0; i < 900; i++) {
        int32_t dx = tx - pl->x, dz = tz - pl->z;
        if (dx > 40) dx = 40; if (dx < -40) dx = -40;
        if (dz > 40) dz = 40; if (dz < -40) dz = -40;
        pl->x += dx; pl->z += dz;
        int n0 = g_test_room_se_n;
        g_test_room_se_tick = i;
        tick_once();
        if (z->sub_state_1 != w1 || z->sub_state_2 != w2 || (uint8_t)z->motion != wm) {
            int loco = 0, fc = 0; (void)posed_mask(z, &loco, &fc);
            printf("  w    i=%-4d st=%u s1=0x%02x s2=%-2u mo=%-3d fr=%-3u/%-3d bank=%s\n",
                   i, z->state, z->sub_state_1, z->sub_state_2, (int)z->motion,
                   z->anim_frame, fc, loco ? "LOCO" : "ACT ");
            w1 = z->sub_state_1; w2 = z->sub_state_2; wm = (uint8_t)z->motion;
        }
        if (g_test_room_se_n != n0) report(tag, i, z, n0, g_test_room_se_n);
        if (z->state == 1 && (z->sub_state_1 == 0x02 || z->sub_state_1 == 0x13) && i > 200) break;
      } }
    printf("--- WACH-SE-KETTE %s:", tag);
    for (int i = 0; i < g_test_room_se_n; i++) printf(" %d", g_test_room_se_log[i]);
    printf("\n");
    printf("nach Anfahrt: st=%u s1=0x%02x s2=%u grid=0x%02x mo=%d fr=%u poise=%d hp=%d\n",
           z->state, z->sub_state_1, z->sub_state_2, z->grid_id, (int)z->motion,
           z->anim_frame, (int)z->hit_stun, (int)z->hp);

    /* --- IDENTISCHE Behandlung: gleiche Start-Kennwerte, dann Schuss-Serie --- */
    z->hurt_clip = 3; z->hit_stun = 4; z->hp = 200;         /* nur die Raum-/Pfad-Differenz messen */
    z->x = pl->x + 2600; z->z = pl->z + 2600;               /* feste Distanz: kein Grab dazwischen */
    re15_damage_seed_rng(0x0BADC0DEu);
    g_test_room_se_n = 0;
    printf("--- SE-SEQUENZ (Treffer -> Sturz -> Liegen -> Aufstehen) ---\n");
    int knocked = 0, gt = 0;
    for (int shot = 0; shot < 8 && !knocked; shot++) {
        int b4 = g_test_room_se_n;
        g_test_room_se_tick = gt;
        z->hit_react &= (uint8_t)~1u;
        (void)re15_enemy_take_damage(z, 0);
        printf("  [SCHUSS %d] hp=%d poise=%d -> st=%u s1=0x%02x\n",
               shot, (int)z->hp, (int)z->hit_stun, z->state, z->sub_state_1);
        report(tag, gt, z, b4, g_test_room_se_n);
        for (int t = 0; t < 60; t++, gt++) {
            g_test_room_se_tick = gt;
            int n0 = g_test_room_se_n;
            tick_once();
            if (g_test_room_se_n != n0) report(tag, gt, z, n0, g_test_room_se_n);
            if (z->state == 1 && z->sub_state_1 == 0x11) { knocked = 1; break; }
            if (z->hp < 0) break;
        }
        if (z->hp < 0) { printf("  (tot vor dem Knockdown)\n"); break; }
    }
    if (!knocked) printf("  !! KEIN KNOCKDOWN erreicht\n");
    else {
        printf("  [KNOCKDOWN] st=%u s1=0x%02x s2=%u mo=%d\n",
               z->state, z->sub_state_1, z->sub_state_2, (int)z->motion);
        uint8_t p1 = 0xff, p2 = 0xff, pm = 0xff;
        for (int t = 0; t < 2500; t++, gt++) {
            g_test_room_se_tick = gt;
            int n0 = g_test_room_se_n;
            tick_once();
            if (z->sub_state_1 != p1 || z->sub_state_2 != p2 || (uint8_t)z->motion != pm) {
                int loco = 0, fc = 0; (void)posed_mask(z, &loco, &fc);
                printf("  .    t=%-5d PHASE st=%u s1=0x%02x s2=%-2u s3=%-2u mo=%-3d fr=%-3u/%-3d bank=%s\n",
                       gt, z->state, z->sub_state_1, z->sub_state_2, z->sub_state_3,
                       (int)z->motion, z->anim_frame, fc, loco ? "LOCO" : "ACT ");
                p1 = z->sub_state_1; p2 = z->sub_state_2; pm = (uint8_t)z->motion;
            }
            if (g_test_room_se_n != n0) report(tag, gt, z, n0, g_test_room_se_n);
            if (z->state == 1 && z->sub_state_1 == 0x02 && z->sub_state_2 != 0 && t > 60) {
                printf("  (zurueck in ENGAGE @t=%d)\n", gt); break;
            }
        }
    }
    printf("--- SE-KETTE %s:", tag);
    for (int i = 0; i < g_test_room_se_n; i++) printf(" %d", g_test_room_se_log[i]);
    printf("\n");
    free(d);
}

int main(int argc, char **argv)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    int slot1140 = (argc > 1) ? atoi(argv[1]) : -1;
    /* ROOM10D0: Tuer-15-Spawn (7650,0,11400) yaw 2048 — wie probe_10d0_knockse. */
    caseinfo_t a = { "ROOM10D0", 0x10D0, 7650, 11400, 2048, -1, 0 };
    /* ROOM1140: Tuer-0-Spawn aus main00 next=(-1300,0,-13950) yaw 2048. */
    caseinfo_t b = { "ROOM1140", 0x1140, -1300, -13950, 2048, slot1140, 0 };
    /* ROOM10D0 NACH der Marvin-Cutscene (Ck(4,247)==1) — der Wortlaut des Nutzer-Reports. */
    caseinfo_t c = { "ROOM10D0", 0x10D0, 7650, 11400, 2048, -1, 1 };
    run_case(base, &a, "10D0");
    run_case(base, &c, "10D0c");
    run_case(base, &b, "1140");
    return 0;
}
