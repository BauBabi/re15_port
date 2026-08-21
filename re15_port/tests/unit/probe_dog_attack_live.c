/* probe_dog_attack_live.c — REPRODUKTION (kein Fix) des Nutzer-Befunds 2026-08-21:
 *   "Wenn Leon von den Hunden angegriffen wird, verschwindet er teilweise. Ausserdem
 *    bekommt er keine Blutwunde/Effekt vom Angriff."
 *
 * ECHTER WEG (Skill re15-room-probe): ROOM1190.RDT (RE1.5-Hunde-Raum) laden, die Hunde ueber
 * das raum-eigene sub13 spawnen, EM020/EM_TYPE20 als ECHTE Bank laden (ohne Bank ist
 * clip_len == 0 und jede Messung wertlos), dann re15_game_step + Pad ticken und Leon in den
 * Hund laufen lassen. Pro Tick wird die GERENDERTE Pose gemessen — exakt die Bank-Komposition
 * von platform/pc/main.c:5487-5518 (anim_select, dann der Victim-Override PL00-Rig +
 * Keyframe-Pool der Greifer-Bank, clip_override = player->motion) — plus:
 *   - alle 15 PL00-Knochen-Weltpositionen (Spannweite = "verschwindet teilweise"-Detektor)
 *   - FX-Spawns (Zahl/Skala/Position) = "Effekt"
 *   - Wund-Akkumulatoren der 8 Panels = "Blutwunde"
 *
 * Aufruf: probe_dog_attack_live [re15|re2]   (Default: beide nacheinander)
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_esp.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_msg.h"
#include "re15_inventory.h"
#include "re15_collision.h"
#include "re15_room.h"
#include "re2_ems.h"

extern void re15_player_aim_reset(void);

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* ---- Leons eigenes Rig (was der Renderer als def/pl00 bindet) ---------------------------- */
static re15_emd_animation_t s_pl00_anim;
static re15_emd_skeleton_t  s_pl00_skel;
static re15_emd_animation_t s_w01_anim;
static re15_emd_skeleton_t  s_w01_skel;
static int s_w01_ok = 0;
static int load_pl00(void)
{
    size_t a = 0, b = 0, c = 0, d = 0;
    uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &a);
    uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &b);
    uint8_t *wedd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00W01.EDD", &c);
    uint8_t *wemr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00W01.EMR", &d);
    if (!(edd && emr &&
          re15_emd_parse_animation(edd, a, &s_pl00_anim) == 0 &&
          re15_emd_parse_skeleton (emr, b, &s_pl00_skel) == 0))
        return 0;
    /* W01 ist im Renderer ein KOMPOSIT (main.c:2850-2854): PL00-Hierarchie + W01-Keyframes.
     * Das rohe W01-EMR direkt zu posieren liefert absurde Knochen (gemessen: Kopf-dy -25874). */
    re15_emd_skeleton_t w01_raw = {0};
    s_w01_ok = (wedd && wemr &&
                re15_emd_parse_animation(wedd, c, &s_w01_anim) == 0 &&
                re15_emd_parse_skeleton (wemr, d, &w01_raw) == 0);
    if (s_w01_ok) {
        s_w01_skel = s_pl00_skel;
        s_w01_skel.keyframe_data       = w01_raw.keyframe_data;
        s_w01_skel.keyframe_data_size  = w01_raw.keyframe_data_size;
        s_w01_skel.keyframe_count      = w01_raw.keyframe_count;
        s_w01_skel.keyframe_size_bytes = w01_raw.keyframe_size_bytes;
    }
    return 1;
}

/* ---- Der RENDERPFAD aus platform/pc/main.c:5487-5562, 1:1 nachgebaut --------------------- */
typedef struct { int32_t b[RE15_EMD_MAX_BONES][3]; int ok; int clip; int kf; int nbones; } pose_t;

static void render_pose(const re15_actor_t *pl, pose_t *out)
{
    memset(out, 0, sizeof *out);
    re15_anim_banks_t banks;
    memset(&banks, 0, sizeof banks);
    banks.def_skel = &s_pl00_skel; banks.def_anim = &s_pl00_anim;
    banks.w01_skel = s_w01_ok ? &s_w01_skel : &s_pl00_skel;
    banks.w01_anim = s_w01_ok ? &s_w01_anim : &s_pl00_anim;
    banks.w01_ok   = s_w01_ok;
    banks.pl00_skel = &s_pl00_skel; banks.pl00_anim = &s_pl00_anim; banks.pl00_ok = 1;
    re15_anim_view_t av;
    re15_actor_anim_select(pl, 1, &banks, &av);
    const re15_emd_skeleton_t  *p_skel = av.skel;
    const re15_emd_animation_t *p_anim = av.anim;
    int clip_ovr = av.clip_override;

    static re15_emd_skeleton_t s_victim_skel;              /* == main.c:5503 */
    if (re15_player_victim_state() != 0) {
        re15_enemy_bank_t *vb = re15_enemy_find(re15_player_victim_type());
        if (vb && vb->victim_ok && vb->anim_victim.clip_count > 0) {
            s_victim_skel = s_pl00_skel;
            s_victim_skel.keyframe_data       = vb->skel_victim.keyframe_data;
            s_victim_skel.keyframe_data_size  = vb->skel_victim.keyframe_data_size;
            s_victim_skel.keyframe_count      = vb->skel_victim.keyframe_count;
            s_victim_skel.keyframe_size_bytes = vb->skel_victim.keyframe_size_bytes;
            p_skel   = &s_victim_skel;
            p_anim   = &vb->anim_victim;
            clip_ovr = (int)pl->motion;
        }
    }
    if (!p_anim || p_anim->clip_count <= 0) return;
    int kf = re15_compute_actor_kf(p_anim, p_skel, pl, clip_ovr, (uint32_t)pl->anim_frame);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor;
    g_anim_pose_actor = NULL;                              /* QUERY (kein Crossfade-Verbrauch) */
    int rv = re15_skel_compute_pose(p_skel, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return;
    out->ok = 1; out->clip = clip_ovr; out->kf = kf;
    out->nbones = p_skel->bone_count;
    for (int i = 0; i < p_skel->bone_count && i < RE15_EMD_MAX_BONES; i++)
        re15_skel_bone_to_world(poses[i].trans, pl->rot_y, pl->x, pl->y, pl->z, out->b[i]);
}

static void pose_stats(const pose_t *p, const re15_actor_t *pl,
                       int *head_dy, int *span, int *worst_bone)
{
    *head_dy = 0; *span = -1; *worst_bone = -1;
    if (!p->ok) return;
    *head_dy = p->b[8][1] - pl->y;
    for (int i = 0; i < p->nbones; i++) {
        int32_t dx = p->b[i][0] - pl->x, dy = p->b[i][1] - pl->y, dz = p->b[i][2] - pl->z;
        int32_t d = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy) + (dz < 0 ? -dz : dz);
        if (d > *span) { *span = d; *worst_bone = i; }
    }
}

/* ---- Baenke --------------------------------------------------------------------------- */
static uint8_t *s_ems15 = NULL; static size_t s_ems15_n = 0;
static uint8_t *s_ems2  = NULL; static size_t s_ems2_n  = 0;

static re15_enemy_bank_t *load_bank(uint8_t type, int re2)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return eb;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return NULL;
    if (re2) {
        if (!s_ems2) s_ems2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems2_n);
        if (s_ems2 && re2_ems_load_bank(s_ems2, s_ems2_n, (int)type, eb, NULL) == 0) {
            eb->buf = NULL; eb->ok = 1; return eb;
        }
    } else {
        if (!s_ems15) s_ems15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_ems15_n);
        int idx = s_ems15 ? re15_ems_index_for_type(type) : -1;
        size_t off = 0, len = 0;
        if (idx >= 0 && re15_ems_get_entry(s_ems15, s_ems15_n, idx, &off, &len) == 0) {
            uint8_t *blob = (uint8_t *)malloc(len);
            memcpy(blob, s_ems15 + off, len);
            if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, NULL) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->victim_ok = (re15_emd_parse_victim_bank(blob, len, &eb->skel_victim,
                                                            &eb->anim_victim) == 0);
                return eb;
            }
        }
    }
    eb->type = 0;
    return NULL;
}

/* ---- game_step-Harness ------------------------------------------------------------------ */
static re15_rdt_t   s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static void bringup(void)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_player_victim_reset();
    re15_esp_fx_reset(); re15_wound_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1190;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    /* ROOM1190s Hunde-Welle lebt in sub13 (test_re2_room1190_ab.c: Sce_em_set Typ 0x20 @RDT
     * 0x2900/0x2914/0x2928 im sub13-Fenster) — wie dort direkt zuenden. */
    if (s_rdt.sub_scd_count > 13 && s_rdt.sub_scd[13]) scd_thread_start(2, s_rdt.sub_scd[13]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
}

static int fx_dump(int prev, const re15_actor_t *pl)
{
    int now = re15_esp_fx_count();
    if (now <= prev) return now;
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (!f) continue;
        if (f->frame == 0 && f->timer == 0)
            printf("        FX id=%u sub=%u scale=0x%04X pos=(%d,%d,%d) dy_zu_Leon=%d bank=%s\n",
                   f->effect_id, f->sub_index, f->scale16, (int)f->x, (int)f->y, (int)f->z,
                   (int)(f->y - pl->y), f->bank ? "aufgeloest" : "KEINE");
    }
    return now;
}

static void run(int re2)
{
    re15_ai_flavor_set(re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15);
    re15_enemy_reset();
    bringup();
    re15_enemy_bank_t *db = load_bank(0x20, re2);
    printf("\n================ %s ================\n", re2 ? "RE2-FLAVOR" : "RE1.5-FLAVOR");
    printf("EM020-Bank: %s (%d Knochen, %d Clips, %d Meshes) | Victim ok=%d Clips=%d "
           "Struktur-Bones=%d Pool-kf=%d B\n",
           (db && db->ok) ? "geladen" : "FEHLT",
           db ? db->skel.bone_count : 0, db ? db->anim.clip_count : 0, db ? db->md1.mesh_count : 0,
           db ? db->victim_ok : 0,
           (db && db->victim_ok) ? db->anim_victim.clip_count : -1,
           (db && db->victim_ok) ? db->skel_victim.bone_count : -1,
           (db && db->victim_ok) ? db->skel_victim.keyframe_size_bytes : -1);

    int ds[8], nd = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x20 && nd < 8) ds[nd++] = s;
    printf("Hunde gespawnt: %d\n", nd);
    if (nd == 0) { printf("ABBRUCH: kein Hund\n"); return; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++)          /* Isolation: nur Hund 0 lebt */
        if (g_actors[s].active && s != ds[0]) g_actors[s].active = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *d  = &g_actors[ds[0]];
    for (int f = 0; f < 30; f++) frame(0, 0);
    /* ROOM1190 spawnt die Hunde im SKRIPT-SPRUNG (grid 0x40 -> state 4 sub 0), der auf die
     * SCD-Marke 0x43 wartet (@0x801113e4-ec). Genau die Marke zuenden = der echte Weg, wie
     * der Raum die Hunde losschickt (danach: Landung -> state 1 sub 2 CHASE). */
    if (!re2 && d->state == 4 && d->sub_state_1 == 0) d->grid_id = 0x43;

    uint8_t w0[8][2]; re15_wound_save(w0);
    int fx_prev = re15_esp_fx_count();
    int hp_last = pl->hp;
    int last_key = -1;
    int span_max = -1, span_frame = -1, span_bone = -1;
    int reported = 0;

    printf("%-6s %-3s %-3s %-3s %-4s %-3s %-4s | %-3s %-3s | %-7s %-6s %-4s %-7s | %s\n",
           "frame", "vs", "st", "s1", "mot", "af", "hp", "ds1", "ds2",
           "KopfdY", "Span", "Bone", "clip/kf", "Ereignis");
    for (int f = 0; f < 900; f++) {
        /* Leon direkt neben den Hund stellen + RUN-Sentinel (identisch zu
         * test_re2_room1190_ab.c — der bewiesene Weg, dass die Hunde-AI committet). */
        if (pl->hp >= 0 && re15_player_victim_state() == 0 && !re15_player_is_grabbed()) {
            pl->x = d->x + 1200; pl->z = d->z; pl->y = d->y; pl->floor = d->floor;
            /* NUR im freien Zustand den RUN-Sentinel setzen — waehrend des Treffer-Flinch
             * (state 2 / Clips 8/9/0xa) wuerde das die Flinch-Anim ueberschreiben und das
             * gemessene Bild verfaelschen. */
            if (pl->state == 0 && pl->motion != 8 && pl->motion != 9 && pl->motion != 0x0a)
                pl->motion = 100;
        }
        frame(0, 0);
        pose_t P; render_pose(pl, &P);
        int hdy, span, wb; pose_stats(&P, pl, &hdy, &span, &wb);
        if (span > span_max) { span_max = span; span_frame = f; span_bone = wb; }
        int key = (re15_player_victim_state() << 24) | (pl->state << 16) |
                  (pl->sub_state_1 << 8) | (int)pl->motion;
        key ^= (d->sub_state_1 << 12) ^ (d->sub_state_2 << 4) ^ (d->state << 20);
        if (f == 0 || f == 40)
            printf("   [diag f%d] dog st=%d s1=%d grid=0x%02X hp=%d pos=(%d,%d,%d) floor=%d | "
                   "pl pos=(%d,%d,%d) floor=%d | combat_active=%d\n",
                   f, d->state, d->sub_state_1, d->grid_id, d->hp, d->x, d->y, d->z, d->floor,
                   pl->x, pl->y, pl->z, pl->floor, (int)g_scd.combat_active);
        int hit = (pl->hp < hp_last);
        if (key != last_key || hit) {
            if (reported < 200) {
                printf("%-6d %-3d %-3d %-3d %-4d %-3d %-4d | %-3d %-3d | %-7d %-6d %-4d %d/%-5d | "
                       "pl=(%d,%d) ank=(%d,%d) hund=(%d,%d) d=%d | %s%s\n",
                       f, re15_player_victim_state(), pl->state, pl->sub_state_1,
                       (int)pl->motion, (int)pl->anim_frame, pl->hp,
                       d->sub_state_1, d->sub_state_2,
                       hdy, span, wb, P.clip, P.kf,
                       pl->x, pl->z, pl->anchor_x, pl->anchor_z, d->x, d->z,
                       (int)((pl->x - d->x) * (pl->x - d->x) + (pl->z - d->z) * (pl->z - d->z)) / 1000,
                       hit ? "HP-DROP " : "", P.ok ? "" : "POSE-FEHLER");
                reported++;
            }
            last_key = key;
        }
        if (hit) fx_prev = fx_dump(fx_prev, pl);
        else     fx_prev = fx_dump(fx_prev, pl);
        hp_last = pl->hp;
        if (pl->hp < 0 && re15_player_victim_state() == 0) { printf("  (Leon tot, Ende f=%d)\n", f); break; }
    }
    uint8_t w1[8][2]; re15_wound_save(w1);
    printf("  Wund-Akku-Delta:");
    int wsum = 0;
    for (int i = 0; i < 8; i++) { int dv = (int)w1[i][1] - (int)w0[i][1];
        wsum += dv; printf(" [%d]=%+d(lvl %u)", i, dv, w1[i][0]); }
    printf("  Summe=%d\n", wsum);
    printf("  FX gesamt jetzt aktiv=%d | max Knochen-Spannweite=%d (Bone %d @f%d); "
           "Referenz stehend ~1900\n", re15_esp_fx_count(), span_max, span_bone, span_frame);
}

int main(int argc, char **argv)
{
    size_t sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1190.RDT", &sz);
    if (!buf) { printf("FAIL: ROOM1190.RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    if (!load_pl00()) { printf("FAIL: PL00-Rig nicht ladbar — Messung waere wertlos\n"); return 1; }
    printf("PL00: %d Knochen, %d Keyframes a %d B, %d Clips | W01 ok=%d\n",
           s_pl00_skel.bone_count, s_pl00_skel.keyframe_count,
           s_pl00_skel.keyframe_size_bytes, s_pl00_anim.clip_count, s_w01_ok);
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    /* Wie die Plattformen (platform/pc/main.c:4344, platform/psx/main.c:499): ohne diese
     * Bindung spiegelt game_step NULL in die Engine und die Opfer-Bone-Abfrage laeuft auf
     * ihrem Rueckfallpfad — die Messung waere dann nicht die des Spiels. */
    s_ctx.pl00_skel = &s_pl00_skel; s_ctx.pl00_anim = &s_pl00_anim;

    const char *which = (argc > 1) ? argv[1] : NULL;
    if (!which || strcmp(which, "re15") == 0) run(0);
    if (!which || strcmp(which, "re2")  == 0) run(1);
    return 0;
}
