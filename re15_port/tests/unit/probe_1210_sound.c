/* probe_1210_sound.c — MESSUNG: welche SE gibt der Port an den ROOM1210-Armen ab?
 *
 * Nutzer-Befund 2026-08-29: "Ausserdem fehlt das laute Geraeusch wie bei RE 2."
 * Diese Sonde behauptet nichts. Sie protokolliert JEDEN re15_audio_room_se-Aufruf
 * (FUN_800453d0-Spion in tests/test_support.c) waehrend
 *   (A) eines Flur-Durchlaufs im Gehtempo 75/Bild und
 *   (B) der gesetzten Griff-Stellung (wie test_1210_gitterhaende (4)),
 * jeweils mit Bild, Arm-Phase (+0x5) und Unter-Phase (+0x6).
 * Zusaetzlich dumpt sie die snd0/snd1-EDT-Records des Raums, damit sichtbar ist,
 * welche SE-Id ueberhaupt hinterlegt ist.
 *
 * AUSSAGEKRAFT: die Plattform-Audio ist im Test ein No-op (test_support.c). Die Sonde
 * misst also, WELCHE ID WANN gerufen wird — NICHT, ob und wie laut sie zu hoeren ist.
 * Ueber Lautstaerke/Prio-Gate/Panning sagt sie NICHTS.
 *
 * KEIN add_test — reine Messsonde. */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_skeleton.h"
#include "re15_math.h"
#include "re15_vab.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

/* SNDL1-Spion aus tests/test_support.c */
extern int g_test_room_se_log[2048];
extern int g_test_room_se_tickof[2048];
extern int g_test_room_se_n;
extern int g_test_room_se_tick;
extern int g_test_snd0_se_last;
extern int g_test_snd0_se_count;

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t            s_blob[0x80000];
static uint8_t            s_blob_donor[0x80000];

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int load_bank(uint8_t type, uint8_t *blob, size_t blobcap)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0; int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= blobcap) {
        memcpy(blob, ems + off, len);
        re15_enemy_bank_t *eb = re15_enemy_find(type);
        if (!eb) eb = re15_enemy_alloc(type);
        if (eb) {
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok   = (re15_emd_parse_loco_bank(blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
                eb->victim_ok = (re15_emd_parse_victim_bank(blob, len, &eb->skel_victim, &eb->anim_victim) == 0);
                ok = 1;
            }
        }
    }
    free(ems);
    return ok;
}

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

/* ---- EDT-Dump (byte-true Feldlage aus FUN_800453d0: b1&0x7f = Programm @0x80045490-98,
 *      b2>>4 = Tone @0x80045474, b2&0xf = Prio @0x800454b0, (b3&0x1f)-16 = Stimme
 *      @0x80045478-7c, b3>>5 = Extra-Layer @0x8004548c) ------------------------------- */
static void dump_edt(const uint8_t *rdt, const char *label, int eo, int ho, int vo)
{
    uint32_t e = (uint32_t)rdt[eo] | ((uint32_t)rdt[eo+1]<<8) | ((uint32_t)rdt[eo+2]<<16) | ((uint32_t)rdt[eo+3]<<24);
    uint32_t h = (uint32_t)rdt[ho] | ((uint32_t)rdt[ho+1]<<8) | ((uint32_t)rdt[ho+2]<<16) | ((uint32_t)rdt[ho+3]<<24);
    uint32_t v = (uint32_t)rdt[vo] | ((uint32_t)rdt[vo+1]<<8) | ((uint32_t)rdt[vo+2]<<16) | ((uint32_t)rdt[vo+3]<<24);
    printf("  %s: EDT@0x%x VH@0x%x VB@0x%x  (%u Records bis zur VH)\n",
           label, e, h, v, (unsigned)((h - e) / 4));
    re15_vab_t vab; memset(&vab, 0, sizeof vab);
    int have = (re15_vab_parse(rdt + h, (size_t)(v - h), &vab) == 0);
    printf("    id | b0 b1 b2 b3 | prog tone prio stimme lay | tvol tpan vag  VAG-Bytes\n");
    for (uint32_t i = 0; i < (h - e) / 4 && i < 0x19; i++) {
        const uint8_t *r = rdt + e + i * 4;
        if (!r[0] && !r[1] && !r[2] && !r[3]) { printf("    %2u | -- leer --\n", i); continue; }
        int prog = r[1] & 0x7f, tone = r[2] >> 4, prio = r[2] & 0xf;
        int voice = (r[3] & 0x1f) - 0x10, lay = r[3] >> 5;
        int tv = -1, tp = -1, vag = -1; long bytes = -1;
        if (have && prog < RE15_VAB_PROGRAM_COUNT) {
            const re15_vab_tone_t *t = &vab.tones[prog * RE15_VAB_TONES_PER_PROGRAM + tone];
            tv = t->vol; tp = t->pan; vag = t->vag_index;
            if (vag >= 1 && vag <= vab.vag_count) bytes = (long)vab.samples[vag-1].size;
        }
        printf("    %2u | %02x %02x %02x %02x | %4d %4d %4d %6d %3d | %4d %4d %3d  %8ld\n",
               i, r[0], r[1], r[2], r[3], prog, tone, prio, voice, lay, tv, tp, vag, bytes);
    }
}

int main(void)
{
    char path[600];
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT");
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    printf("=== ROOM1210: welche SE gibt der Port ab? ===\n");
    printf("\n-- MESSUNG 0: die SE-Baenke des Raums (RDT-Bytes, Feldlage aus FUN_800453d0) --\n");
    dump_edt(buf, "snd0 (RDT+0x08/0x0c/0x10)", 0x08, 0x0c, 0x10);
    dump_edt(buf, "snd1 (RDT+0x14/0x18/0x1c)", 0x14, 0x18, 0x1c);

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1210;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    re15_collision_set_band(0);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, 0, 0, 0);
    printf("\n  EM01A-Bank: %s\n", load_bank(0x1A, s_blob, sizeof s_blob) ? "ok" : "FEHLT");
    int donor = load_bank(0x10, s_blob_donor, sizeof s_blob_donor);
    re15_enemy_bank_t *db = re15_enemy_find(0x10);
    printf("  EM010-Spenderbank: %s victim_ok=%d\n", donor ? "ok" : "FEHLT", db ? db->victim_ok : -1);
    if (db && db->victim_ok) re15_victim_donor_set(0x1Au, 0x10u);
    for (int f = 0; f < 8; f++) frame_step();

    int slots[RE15_ACTOR_MAX], n = 0;
    int32_t zmin = 0x7fffffff, zmax = -0x7fffffff;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type != 0x1A) continue;
        slots[n++] = s;
        if (e->z < zmin) zmin = e->z;
        if (e->z > zmax) zmax = e->z;
    }
    printf("  %d Arme, z-Spanne %ld..%ld\n", n, (long)zmin, (long)zmax);
    if (n == 0) { free(buf); return 1; }

    /* ---- MESSUNG A: Flur-Durchlauf, Gehtempo 75/Bild -------------------------------- */
    printf("\n-- MESSUNG A: Flur-Durchlauf (Gehtempo 75/Bild, Flurmitte x=-19500) --\n");
    g_test_room_se_n = 0; g_test_room_se_tick = 0;
    g_test_snd0_se_count = 0;
    int phase_hist[8]; memset(phase_hist, 0, sizeof phase_hist);
    int tick = 0, last_n = 0;
    for (int32_t z = zmax + 3000; z > zmin - 3000; z -= 75) {
        pl->x = -19500; pl->z = z; pl->hp = 100;
        g_test_room_se_tick = tick;
        frame_step();
        if (g_test_room_se_n != last_n) {
            for (int k = last_n; k < g_test_room_se_n; k++) {
                printf("     Bild %4d: SE(%d)  Arm-Phasen:", tick, g_test_room_se_log[k]);
                for (int i = 0; i < n; i++) {
                    re15_actor_t *e = &g_actors[slots[i]];
                    if (e->sub_state_1) printf(" [slot%d +0x5=%u +0x6=%u]", slots[i],
                                               (unsigned)e->sub_state_1, (unsigned)e->sub_state_2);
                }
                printf("\n");
            }
            last_n = g_test_room_se_n;
        }
        for (int i = 0; i < n; i++) {
            re15_actor_t *e = &g_actors[slots[i]];
            if (e->sub_state_1 < 8) phase_hist[e->sub_state_1]++;
        }
        tick++;
    }
    printf("     %d Bilder gelaufen. SE-Aufrufe gesamt: %d (snd0: %d)\n",
           tick, g_test_room_se_n, g_test_snd0_se_count);
    printf("     Arm-Bilder je Phase +0x5:");
    for (int p = 0; p < 8; p++) if (phase_hist[p]) printf(" %d:%d", p, phase_hist[p]);
    printf("\n");
    {   int hist[32]; memset(hist, 0, sizeof hist);
        for (int k = 0; k < g_test_room_se_n; k++)
            if (g_test_room_se_log[k] >= 0 && g_test_room_se_log[k] < 32) hist[g_test_room_se_log[k]]++;
        printf("     SE-Haeufigkeit:");
        for (int i = 0; i < 32; i++) if (hist[i]) printf(" SE%d x%d", i, hist[i]);
        printf("\n");
    }

    /* ---- MESSUNG B: gesetzte Griff-Stellung (wie test_1210_gitterhaende (4)) ---------
     * ERZWUNGENER ZUSTAND: der Arm wird von Hand in Sub 2 gesetzt und der Spieler 900
     * Einheiten davor. Das ist KEIN Spielzustand — es misst nur die SE-Kadenz der
     * Greif-/Halte-Maschine. Ob ein laufender Spieler diese Stellung erreicht, misst
     * test_1210_gitterhaende getrennt. */
    printf("\n-- MESSUNG B: gesetzte Griff-Stellung (ERZWUNGEN, kein Spielzustand) --\n");
    re15_player_victim_reset();
    re15_collision_set_band(0);
    re15_actor_t *ga = &g_actors[slots[0]];
    for (int i = 0; i < n; i++) { g_actors[slots[i]].sub_state_1 = 0; g_actors[slots[i]].sub_state_2 = 0; }
    ga->sub_state_1 = 2; ga->sub_state_2 = 0;
    int32_t asn = re15_sin_q12(((int)ga->rot_y + 0x400) & 0xfff);
    int32_t acs = re15_cos_q12(((int)ga->rot_y + 0x400) & 0xfff);
    int32_t sx0 = ga->x + (int32_t)((asn * 900) >> 12);
    int32_t sz0 = ga->z + (int32_t)((acs * 900) >> 12);
    printf("     Arm slot %d (%ld,%ld) yaw=%d, Spieler auf (%ld,%ld)\n",
           slots[0], (long)ga->x, (long)ga->z, (int)ga->rot_y, (long)sx0, (long)sz0);
    g_test_room_se_n = 0; last_n = 0;
    for (int f = 0; f < 400; f++) {
        if (!re15_player_is_grabbed()) { pl->x = sx0; pl->z = sz0; }
        g_test_room_se_tick = f;
        frame_step();
        if (g_test_room_se_n != last_n) {
            for (int k = last_n; k < g_test_room_se_n; k++)
                printf("     Bild %3d: SE(%d)  [+0x5=%u +0x6=%u Clip=%u Frame=%u Opfer=%d]\n",
                       f, g_test_room_se_log[k], (unsigned)ga->sub_state_1, (unsigned)ga->sub_state_2,
                       (unsigned)ga->motion, (unsigned)ga->anim_frame,
                       re15_player_victim_state());
            last_n = g_test_room_se_n;
        }
        pl->hp = 100;
    }
    printf("     SE-Aufrufe in 400 Bildern: %d\n", g_test_room_se_n);
    {   int hist[32]; memset(hist, 0, sizeof hist);
        for (int k = 0; k < g_test_room_se_n; k++)
            if (g_test_room_se_log[k] >= 0 && g_test_room_se_log[k] < 32) hist[g_test_room_se_log[k]]++;
        printf("     SE-Haeufigkeit:");
        for (int i = 0; i < 32; i++) if (hist[i]) printf(" SE%d x%d", i, hist[i]);
        printf("\n");
    }
    free(buf);
    return 0;
}
