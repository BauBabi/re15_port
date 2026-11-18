/* test_r42_arme_re15_modell.c — RIEGEL (Runde 42, Strang "original", Stufe 2, 18.11.2026; Runde 2):
 * Die ROOM1210/1211-Gitterhaende zeichnen die Arme des RE1.5-Modells EM1A, das Verhalten bleibt das RE2-Zellenarm-
 * Gehirn — UNVERAENDERT, bis auf das Bild.
 *
 * NUTZERWUNSCH 18.11.2026, woertlich: "Und unten in den Gefängnis in STAGE1 hast du die Arme aus Resident Evil 2
 * übernommen. Aber du solltest EIGENTLICH NUR die funktionalität aus Resident Evil 2 übernehmen, nicht das Modell. Das
 * Modell darf schon noch das aus Resident Evil 1.5 sein...."
 *
 * Belege im Modul engine/src/enemy_ai_re2_zellenarm.c, Block MODELL (dort auch die Messung im echten RE1.5: im Bild
 * sind nur die zwei Arme zu sehen, Rumpf und Kopf nie; vor dem Ausloeser ist jedes Fenster leer).
 *
 * Aufbau: derselbe Ablauf (Schlaf, Weckruf, Ausfahren, Zugriff, Halten, Loslassen, Rueckzug, Treffer) laeuft ZWEIMAL:
 *   Lauf A = Stand bis Runde 41: EM2D (RE2 CDEMD0.EMS kind 0x2D) ist die Bank des Typs 0x1A, keine Anmeldung;
 *   Lauf B = neuer Stand: EM1A ueber den EXE-TOC (@0x80072f38 Leon, Eintrag 0x1A) ist die Bank, EM2D nur Logik-Bank
 *            (re15_re2arm_modell_einrichten), wie der Lader in platform/pc/main.c.
 * Verlangt: jedes Bild gleich (Zustandswort, Lage, Hand, Leon, Griff, Toene) — "Verhalten bleibt wie heute", auch die
 * Reichweite. Dazu das Bild: Rumpf/Kopf aus, beide Arme an, Handgelenk des gezeichneten EM1A-Arms auf der RE2-Hand.
 *
 * GEGENPROBE: auf aaea5f8c2 fehlen re15_re2arm_modell_einrichten / _modell_re15 / _em1a_clip / _takt /
 * _zeichen_versatz; sie sind hier SCHWACH deklariert, der Test baut dort und faellt in den Pruefungen (Modell, Maske,
 * Zeichen-Lage). Auf 4e7c8dfcd (Runde 1) fallen Maske, Hoehe, Hand-Knochen und der Bild-fuer-Bild-Vergleich.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_math.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_ai_flavor.h"
#include "re15_roy_r37.h"
#include "re2_ems.h"
#include "re15_enemy_ai_re2_zellenarm.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

/* schwach: fehlen auf dem Stand vor Stufe 2 (Gegenprobe) */
extern void re15_re2arm_modell_einrichten(re15_enemy_bank_t *anzeige, const re15_enemy_bank_t *em2d_logik) __attribute__((weak));
extern int  re15_re2arm_modell_re15(const re15_actor_t *e) __attribute__((weak));
extern int  re15_re2arm_em1a_clip(int re2_clip) __attribute__((weak));
extern int  re15_re2arm_takt(int slot, int *re2_clip, int *re2_bild) __attribute__((weak));
extern int  re15_re2arm_zeichen_versatz(const re15_actor_t *e, const re15_skel_pose_t *poses, int n, int32_t d[3]) __attribute__((weak));

#define WEST_FLAECHE (-21090)          /* SCA-Zelle [6] ROOM1210, Flurseite der Westwand (Modul RE2ARM_1210_WEST_FLAECHE) */

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t *s_ems15 = NULL; static size_t s_ems15_n = 0;
static uint8_t *s_ems15e = NULL; static size_t s_ems15e_n = 0;
static uint8_t *s_ems2 = NULL;  static size_t s_ems2_n = 0;
static uint8_t *s_em1a = NULL;  static size_t s_em1a_n = 0;
static re15_enemy_bank_t s_em2d;
static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static const uint8_t *toc_blob(const uint8_t *ems, size_t n, int archiv, size_t *len)
{
    uint32_t sek = 0, lg = 0;
    if (!ems || !re15_ems_toc(archiv, 0x1A, &sek, &lg)) return NULL;     /* Index type-0x10 @0x80022338 */
    if ((size_t)sek * 2048u + lg > n) return NULL;
    *len = lg; return ems + (size_t)sek * 2048u;
}
/* Lauf A: EM2D als Bank des Typs (Stand bis Runde 41). Lauf B: EM1A + EM2D-Logik (platform/pc/main.c). */
static int banken_laden(int em1a)
{
    if (!s_ems15) s_ems15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_ems15_n);
    if (!s_ems2)  s_ems2  = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &s_ems2_n);
    if (!s_ems15 || !s_ems2) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(0x1A);
    if (!eb) eb = re15_enemy_alloc(0x1A);
    if (!eb) return 0;
    re15_tim_t tim = {0};
    if (!em1a) {
        if (re2_ems_load_bank(s_ems2, s_ems2_n, 0x2D, eb, &tim) != 0) return 0;
        eb->ok = 1; eb->buf = NULL;
        return 1;
    }
    if (!s_em1a) {
        size_t lg = 0; const uint8_t *b = toc_blob(s_ems15, s_ems15_n, RE15_EMS_ARCHIV_LEON, &lg);
        if (!b) return 0;
        s_em1a = (uint8_t *)malloc(lg); memcpy(s_em1a, b, lg); s_em1a_n = lg;
    }
    if (re15_emd_parse_container(s_em1a, s_em1a_n, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return 0;
    eb->loco_ok   = (re15_emd_parse_loco_bank(s_em1a, s_em1a_n, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(s_em1a, s_em1a_n, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->own_ok    = (re15_emd_parse_own_bank(s_em1a, s_em1a_n, &eb->skel_own, &eb->anim_own) == 0);
    eb->ok = 1; eb->buf = NULL;
    memset(&s_em2d, 0, sizeof s_em2d);
    re15_tim_t t2 = {0};
    if (re2_ems_load_bank(s_ems2, s_ems2_n, 0x2D, &s_em2d, &t2) != 0) return 0;
    if (re15_re2arm_modell_einrichten) re15_re2arm_modell_einrichten(eb, &s_em2d);
    return 1;
}
static int s_se[256], s_se_n = 0;
static void se_cap(int id, int f) { if (s_se_n < 256) s_se[s_se_n++] = id | (f << 8); }
static void bank_cap(int b) { (void)b; }

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}
static uint32_t word(const re15_actor_t *e)
{ return (uint32_t)e->state | ((uint32_t)e->sub_state_1 << 8) | ((uint32_t)e->sub_state_2 << 16); }

static int aufsetzen(int em1a, int *slots, int *bank_ok)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    re15_re2z_rng_reset();
    g_current_room_id = 0x1210;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    re15_collision_set_band(0);
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, 0, 0, 0);
    *bank_ok = banken_laden(em1a);                       /* NACH dem Reset */
    re15_re2arm_audio_hook(se_cap, bank_cap);
    s_se_n = 0;
    pl->x = -19500; pl->z = 5000;
    for (int f = 0; f < 8; f++) frame_step();
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x1A) slots[n++] = s;
    return n;
}

/* ---- Bild-Protokoll eines Laufs ---- */
#define MAXB 700
typedef struct { uint32_t w5, w9; int32_t x5, z5, h5[3], plx, plz; int8_t griff; int16_t se; } bild_t;
typedef struct {
    int n; bild_t b[MAXB];
    int ereignis_griff, ereignis_loesen, rueckzug_bilder, rz_clip, t_mo_reach, t_mo_zugriff, t_mo_halten, mo9_treffer;
    int hidden_vor, hidden_nach, sicht_reach, x_nach_rz, x_vor_rz, hp_nach, cd_nach, pin_ok;
    double pin_abstand;
    /* Zeichen-Lage (nur Lauf B) */
    int versatz_n, versatz_fehl, versatz_max_diff; int32_t rumpf_x_max, hand_x_min_halten;
} lauf_t;
static lauf_t L[2];
static int s_slot5, s_slot9;

static void protokoll(lauf_t *r)
{
    if (r->n >= MAXB) return;
    re15_actor_t *a5 = &g_actors[s_slot5], *a9 = &g_actors[s_slot9], *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    bild_t *b = &r->b[r->n++];
    b->w5 = word(a5); b->w9 = word(a9); b->x5 = a5->x; b->z5 = a5->z;
    re15_re2arm_hand_parts(s_slot5, b->h5, NULL);
    b->plx = pl->x; b->plz = pl->z; b->griff = (int8_t)re15_player_is_grabbed();
    b->se = (int16_t)s_se_n;
}
/* Zeichen-Lage wie der Zeichner (platform/pc/main.c): Pose des gezeichneten Clips, Versatz, Handgelenk = RE2-Hand. */
static void zeichen_pruefen(lauf_t *r, int slot)
{
    re15_actor_t *e = &g_actors[slot];
    if (e->no_draw) return;
    re15_enemy_bank_t *b = re15_enemy_find(0x1A);
    if (!b || b->skel.bone_count != 4) return;
    if (!re15_re2arm_zeichen_versatz) { r->versatz_fehl++; return; }
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    int kf = re15_compute_actor_kf(&b->anim, &b->skel, e, -1, e->anim_frame);
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int rv = re15_skel_compute_pose(&b->skel, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return;
    int32_t d[3];
    if (!re15_re2arm_zeichen_versatz(e, poses, b->skel.bone_count, d)) { r->versatz_fehl++; return; }
    /* Handgelenk-Ring des gezeichneten Arms (Quad-Vertices 4..7 von Mesh 2/3, Modul em1a_ring) */
    const int k = (e->re2z_f10e & 1u) ? 3 : 2;
    const re15_md1_mesh_t *mm = &b->md1.meshes[b->skel.bone_mesh_index[k]];
    int32_t c[3] = {0, 0, 0};
    for (int v = 4; v < 8; v++) { c[0] += mm->quad_vertices[v].x; c[1] += mm->quad_vertices[v].y; c[2] += mm->quad_vertices[v].z; }
    for (int i = 0; i < 3; i++) c[i] /= 4;
    int32_t mdl[3], w[3], h[3], rumpf[3];
    for (int i = 0; i < 3; i++)
        mdl[i] = (int32_t)(((int64_t)poses[k].rot[i*3+0] * c[0] + (int64_t)poses[k].rot[i*3+1] * c[1] +
                            (int64_t)poses[k].rot[i*3+2] * c[2]) >> 12) + poses[k].trans[i];
    re15_skel_bone_to_world(mdl, e->rot_y, e->x, e->y, e->z, w);
    re15_skel_bone_to_world(poses[0].trans, e->rot_y, e->x, e->y, e->z, rumpf);
    re15_re2arm_hand_parts(slot, h, NULL);
    int diff = 0;
    for (int i = 0; i < 3; i++) { int dd = abs(w[i] + d[i] - h[i]); if (dd > diff) diff = dd; }
    if (diff > r->versatz_max_diff) r->versatz_max_diff = diff;
    if (slot == s_slot5) {
        if (rumpf[0] + d[0] > r->rumpf_x_max) r->rumpf_x_max = rumpf[0] + d[0];
        if (e->sub_state_1 == 4 && w[0] + d[0] < r->hand_x_min_halten) r->hand_x_min_halten = w[0] + d[0];
    }
    r->versatz_n++;
}

static int lauf(int em1a, lauf_t *r)
{
    memset(r, 0, sizeof *r);
    r->rumpf_x_max = -999999; r->hand_x_min_halten = 999999;
    r->ereignis_griff = r->ereignis_loesen = r->rueckzug_bilder = r->rz_clip = -1;
    int slots[RE15_ACTOR_MAX], bank_ok = 0;
    int n = aufsetzen(em1a, slots, &bank_ok);
    if (!bank_ok || n < 10) return n;
    s_slot5 = slots[4]; s_slot9 = slots[8];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER], *arm = &g_actors[s_slot5];
    int32_t hx, hz; re15_re2arm_home(s_slot5, NULL, &hx, &hz);
    /* Schlaf */
    pl->x = hx + 1500; pl->z = hz;
    for (int f = 0; f < 60; f++) { frame_step(); protokoll(r); }
    r->hidden_vor = (word(arm) == 0x000001u && arm->no_draw == 1);
    /* Weckruf (Member_set(12,1) @0x1EDA) */
    pl->x = hx + 3500; pl->z = hz + 3500;
    for (int i = 0; i < n; i++) g_actors[slots[i]].grid_id = 1;
    { int f = 0; do { frame_step(); protokoll(r); f++; } while (f < 21 && arm->sub_state_2 != 2); }
    pl->x = hx + 2000; pl->z = hz;
    frame_step(); protokoll(r); zeichen_pruefen(r, s_slot5);
    r->sicht_reach = (arm->sub_state_1 == 1 && arm->no_draw == 0);
    r->t_mo_reach = (int)arm->motion;
    int32_t h[3]; re15_re2arm_hand_parts(s_slot5, h, NULL);
    pl->x = h[0] + 300; pl->z = h[2];
    { int f = 0; do { frame_step(); protokoll(r); zeichen_pruefen(r, s_slot5); f++; } while (f < 30 && arm->sub_state_1 == 1); }
    r->t_mo_zugriff = (arm->sub_state_1 == 3) ? (int)arm->motion : -1;
    for (int f = 0; f < 40 && r->ereignis_griff < 0; f++) {
        frame_step(); protokoll(r); zeichen_pruefen(r, s_slot5);
        if (arm->sub_state_1 == 4) r->ereignis_griff = r->n;
    }
    if (r->ereignis_griff >= 0) {
        double lx = pl->x - arm->x, lz = pl->z - arm->z;
        r->pin_abstand = sqrt(lx * lx + lz * lz);
        r->pin_ok = re15_player_is_grabbed() && re15_player_victim_state() == 4 && re15_re2arm_holder_slot() == s_slot5;
        r->t_mo_halten = (int)arm->motion;
    }
    for (int f = 0; f < 200 && r->ereignis_loesen < 0; f++) {
        frame_step(); protokoll(r); zeichen_pruefen(r, s_slot5);
        if (arm->sub_state_1 == 5) r->ereignis_loesen = r->n;
    }
    r->cd_nach = (int)g_re2_room_gflags;
    {   int f = 0; r->x_vor_rz = arm->x;
        while (f < 60 && (re15_player_is_grabbed() || r->rueckzug_bilder < 0)) {
            if (arm->sub_state_1 == 5 && r->rz_clip < 0 && arm->sub_state_2 == 1) r->rz_clip = arm->motion;
            frame_step(); protokoll(r); zeichen_pruefen(r, s_slot5); f++;
            if (r->rueckzug_bilder < 0 && arm->sub_state_1 == 7) r->rueckzug_bilder = f;
        }
        r->x_nach_rz = arm->x; r->hp_nach = arm->hp; r->hidden_nach = arm->no_draw; }
    /* Treffer an Arm 9 */
    {   re15_actor_t *arm9 = &g_actors[s_slot9];
        int32_t h9x, h9z; re15_re2arm_home(s_slot9, NULL, &h9x, &h9z);
        pl->x = h9x - 2000; pl->z = h9z;
        for (int f = 0; f < 3; f++) { frame_step(); protokoll(r); zeichen_pruefen(r, s_slot9); }
        arm9->state = 2;
        frame_step(); protokoll(r);
        frame_step(); protokoll(r);
        r->mo9_treffer = (int)arm9->motion; }
    return n;
}

int main(void)
{
    size_t sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT", &sz);
    if (!buf) { printf("FAIL: ROOM1210.RDT fehlt\n"); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    printf("=== ROOM1210 Gitterhaende: RE1.5-Arme (EM1A), RE2-Verhalten unveraendert (Runde 42 Stufe 2, Runde 2) ===\n");

    /* Elza: dasselbe Modell aus CDEMD1 (TOC @0x80073178) — Datenvoraussetzung, kein Port-Verhalten. */
    {   s_ems15e = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD1.EMS", &s_ems15e_n);
        if (!s_ems15) s_ems15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_ems15_n);
        size_t l0 = 0, l1 = 0;
        const uint8_t *b0 = toc_blob(s_ems15, s_ems15_n, RE15_EMS_ARCHIV_LEON, &l0);
        const uint8_t *b1 = toc_blob(s_ems15e, s_ems15e_n, RE15_EMS_ARCHIV_ELZA, &l1);
        CHECK("(D) EM1A Leon (CDEMD0) == Elza (CDEMD1), 0x12EC0 Byte", b0 && b1 && l0 == 0x12EC0u && l1 == l0 &&
              memcmp(b0, b1, l0) == 0); }

    /* ---- Lauf A: Stand bis Runde 41 (EM2D gezeichnet) ---- */
    int nA = lauf(0, &L[0]);
    CHECK("Lauf A (EM2D): zehn Arme gespawnt", nA == 10);
    int sA5 = s_slot5;
    /* Modul auf einen anderen Raum stellen, damit der Raumwechsel-Reset (s_room_of_state) fuer Lauf B greift. */
    g_current_room_id = 0x1211; (void)re15_re2arm_tick(sA5);

    /* ---- Lauf B: neuer Stand (EM1A gezeichnet, EM2D Logik) ---- */
    int nB = lauf(1, &L[1]);
    CHECK("Lauf B (EM1A): zehn Arme gespawnt", nB == 10);
    if (nA < 10 || nB < 10) return 1;
    re15_actor_t *arm = &g_actors[s_slot5];
    re15_enemy_bank_t *eb = re15_enemy_find(0x1A);

    CHECK("(M0) Zeichen-Bank 0x1A = RE1.5-EM1A ueber den EXE-TOC: 4 Knochen, 4 Meshes, 4 Clips 30/39/30/54",
          eb && eb->skel.bone_count == 4 && eb->md1.mesh_count == 4 && eb->anim.clip_count == 4 &&
          eb->anim.clips[0].frame_count == 30 && eb->anim.clips[1].frame_count == 39 &&
          eb->anim.clips[2].frame_count == 30 && eb->anim.clips[3].frame_count == 54);
    CHECK("(M1) das Gehirn erkennt das RE1.5-Modell (re15_re2arm_modell_re15)",
          re15_re2arm_modell_re15 && re15_re2arm_modell_re15(arm) == 1);
    printf("  Part-Maske 0x%04X\n", (unsigned)re15_re2arm_part_hide_mask(arm));
    CHECK("(M2) gezeichnet werden beide EM1A-Arme (Teile 2/3), Rumpf und Kopf (Teile 0/1) nicht: Maske 0x0003 "
          "(Original: nur die Arme sichtbar)", re15_re2arm_part_hide_mask(arm) == 0x0003u);
    CHECK("(M3) Leons Opfer-Clips sind die RE2-Funktion: Paar 3 aus EM2D (2 Clips, gleiche Laengen) auf der Zeichen-Bank",
          eb->victim_ok && s_em2d.victim_ok && eb->anim_victim.clip_count == 2 &&
          eb->anim_victim.clips[0].frame_count == s_em2d.anim_victim.clips[0].frame_count &&
          eb->anim_victim.clips[1].frame_count == s_em2d.anim_victim.clips[1].frame_count);
    CHECK("(M4) keine Wurzel-Hebung auf der EM1A-Bank, Anker bleibt RE2 (y -2500)",
          eb->skel.root_y_fix == NULL && arm->y == -2500);
    CHECK("(M5) Clip-Zuordnung RE2 -> EM1A = 0,1,0,2,3,1",
          re15_re2arm_em1a_clip && re15_re2arm_em1a_clip(0) == 0 && re15_re2arm_em1a_clip(1) == 1 &&
          re15_re2arm_em1a_clip(2) == 0 && re15_re2arm_em1a_clip(3) == 2 && re15_re2arm_em1a_clip(4) == 3 &&
          re15_re2arm_em1a_clip(5) == 1);
    CHECK("(L1) die Hand des Gehirns bleibt der EM2D-Handknochen (Arm A: 3, Tabelle @0x80101414 [var*7+2])",
          re15_re2arm_hand_bone(arm) == 3);

    /* ---- (L2) Bild fuer Bild gleich ---- */
    {   int n = L[0].n < L[1].n ? L[0].n : L[1].n, erst = -1;
        for (int i = 0; i < n && erst < 0; i++) {
            const bild_t *a = &L[0].b[i], *b = &L[1].b[i];
            if (a->w5 != b->w5 || a->w9 != b->w9 || a->x5 != b->x5 || a->z5 != b->z5 || a->plx != b->plx ||
                a->plz != b->plz || a->griff != b->griff || a->se != b->se ||
                a->h5[0] != b->h5[0] || a->h5[1] != b->h5[1] || a->h5[2] != b->h5[2]) erst = i;
        }
        printf("  Bilder A %d / B %d, erste Abweichung %d, Toene B %d\n", L[0].n, L[1].n, erst, s_se_n);
        if (erst >= 0) {
            const bild_t *a = &L[0].b[erst], *b = &L[1].b[erst];
            printf("    A: w5 %06X w9 %06X x %d z %d Hand (%d,%d,%d) Leon (%d,%d) griff %d se %d\n", (unsigned)a->w5,
                   (unsigned)a->w9, a->x5, a->z5, a->h5[0], a->h5[1], a->h5[2], a->plx, a->plz, a->griff, a->se);
            printf("    B: w5 %06X w9 %06X x %d z %d Hand (%d,%d,%d) Leon (%d,%d) griff %d se %d\n", (unsigned)b->w5,
                   (unsigned)b->w9, b->x5, b->z5, b->h5[0], b->h5[1], b->h5[2], b->plx, b->plz, b->griff, b->se);
        }
        CHECK("(L2) Verhalten Bild fuer Bild wie bis Runde 41: Zustand, Lage, RE2-Hand (Reichweite), Leon, Griff, Toene",
              L[0].n == L[1].n && L[0].n > 200 && erst < 0 && s_se_n > 0); }

    /* ---- Ablauf des neuen Stands (Werte wie unit_1210_arme_re2, plus gezeichneter EM1A-Clip) ---- */
    lauf_t *r = &L[1];
    printf("  Griff Bild %d, Loesen Bild %d (nach %d), Rueckzug %d Bilder Clip %d, Pin %.0f vom Ursprung\n",
           r->ereignis_griff, r->ereignis_loesen, r->ereignis_loesen - r->ereignis_griff, r->rueckzug_bilder, r->rz_clip,
           r->pin_abstand);
    CHECK("(1) ohne Weckruf tickt der Arm nicht (@0x8002659C): 0x000001, nicht gezeichnet (Original: Fenster leer)",
          r->hidden_vor);
    CHECK("(3) A0 2500/Kegel 1024 -> 0x101 REACH, sichtbar; gezeichnet EM1A-Clip 0/1 (RE2 rng&1 @0x80100878)",
          r->sicht_reach && (r->t_mo_reach == 0 || r->t_mo_reach == 1));
    CHECK("(4b) A1 Hand < 900 -> 0x301 ZUGRIFF, gezeichnet EM1A-Clip 2 (Ausfahren)", r->t_mo_zugriff == 2);
    CHECK("(4d) Griff: Pin an die RE2-Hand + RE2-Push -> Leon 1251 vom Ursprung (FUN_80034D0C r 800+450), Victim-Modus 4",
          r->ereignis_griff >= 0 && r->pin_ok && fabs(r->pin_abstand - 1251.0) <= 1.5);
    CHECK("(4e) HALTEN zeichnet EM1A-Clip 1 (naechste RE1.5-Pose, kein Halte-Clip im Original)", r->t_mo_halten == 1);
    CHECK("(5) Halten ohne Mash 152 Bilder, dann 0x501 + Cooldown 120 (@0x80100D28-5C)",
          r->ereignis_loesen - r->ereignis_griff == 152 && r->cd_nach == 119);
    CHECK("(6) Rueckzug: EM1A-Clip 3, 10 Bilder (RE2-Clip 4), 300 zurueck, hp -1, danach nicht gezeichnet (@0x80100E38-64)",
          r->rz_clip == 3 && r->rueckzug_bilder == 10 && r->x_nach_rz == r->x_vor_rz - 300 && r->hp_nach == -1 &&
          r->hidden_nach == 1);
    CHECK("(7) Treffer -> Rueckzug, gezeichnet EM1A-Clip 3 (Zuruecksinken)", r->mo9_treffer == 3);

    /* ---- (V) Zeichen-Lage ---- */
    printf("  Zeichen-Lage: %d Bilder geprueft, %d ohne Versatz, max |Handgelenk + Versatz - RE2-Hand| = %d, "
           "Rumpf-Wurzel x max %d (Wandflaeche %d), Handgelenk x min im Halten %d\n", r->versatz_n, r->versatz_fehl,
           r->versatz_max_diff, (int)r->rumpf_x_max, WEST_FLAECHE, (int)r->hand_x_min_halten);
    CHECK("(V1) das Handgelenk des gezeichneten EM1A-Arms liegt in jedem sichtbaren Bild auf der RE2-Hand (<= 1)",
          r->versatz_n > 150 && r->versatz_fehl == 0 && r->versatz_max_diff <= 1);
    CHECK("(V2) die (nicht gezeichnete) Rumpf-Wurzel bleibt hinter der Wandflaeche, die Hand greift im Halten in den Flur",
          r->versatz_n > 0 && r->rumpf_x_max < WEST_FLAECHE && r->hand_x_min_halten > WEST_FLAECHE);

    printf(g_fail ? "=== FEHLGESCHLAGEN ===\n" : "=== OK ===\n");
    return g_fail;
}
