/* test_r41_spinnenanim.c — Runde 41 Spur L "spinnenanim" (Nutzer: "Die Spinnen Animation beim herausklettern vom
 * Gitter und runter klettern an der Wand stimmen noch nicht ganz."). Belege: include/re15_spinnenanim_r41.h,
 * engine/src/spinnenanim_r41.c, Dossier analysis/befunde_runde41/L_spinnenanim.md.
 *
 * Misst die MECHANIK des Zeichnens im echten Spielschritt (scd_vm_tick + re15_game_step, echte ROOM2060-RDT und
 * EM025-Baenke) gegen die ORIGINAL-Messung je Takt (test_r41_spinnenanim_orig.inc, erzeugt aus
 * L_belege/orig_2060_gdb.log: DuckStation-GDB, Haltepunkt 0x8001a50c, Figur-Matrix +0x20, Teil-0-Weltmatrix +0x40):
 *
 *  A  RE1.5-KI und RE2-KI, Original-Ablauf nachgestellt (Spieler Bild 60 -> (6000,0,3000) Blick 1024, Cut 1):
 *     je Takt 61..134 (Halten, Herausklettern, Wand, Landung)
 *       A1 Figur-Matrix des Zeichners == Original +0x20 (RotMatrix(+0x68) @0x8001e8f4 x ScaleMatrix(0xccc) @0x8001e940)
 *       A2 gezeichnetes Bild / Ueberblendung == Original (+0x95/+0x8f VOR dem Vorschub, FUN_8001f3bc @0x8001f610-1c)
 *       A3 Weltlage + Weltmatrix des Wurzelteils (Pose wie der Zeichner) == Original Teil 0 (+0x54/+0x40), Toleranz
 *          belegt (Dossier 4/5)
 *       A4 Gegenprobe: der Zeichenweg VOR Runde 41 (nur Gier, ohne Massstab, Bild nach dem Vorschub) verfehlt das
 *          Original in den Wand-Takten 93..132 und im Umbruchtakt 93 (Beweis, dass A1-A3 die Mechanik messen)
 *       A5 Schatten +-1300 x +-1400 (FUN_8001af5c @0x80110d90) auf der Boden-Referenz +0x1ba (@0x80110b4c): an der Wand
 *          auf dem Boden (vorher 500 x 600 auf der Figur-Hoehe, also in der Luft)
 *  B  Spucken nach der Landung (RE1.5-KI): Spawnlage des Giftklumpens == Original (5741,-320,4049)
 *     (R38 orig_2060_schacht_gift.txt Bild 136: Teil-0-Weltmatrix *(+0x188)+0x40 @0x80112770 + (500,0,0) @0x80118ee8)
 *  pruef_log <datei>: Auswerter fuer die Messzeilen der echten exe (RE15_SPINNENANIM_LOG, integration_r41_spinnenanim).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_rdt.h"
#include "re15_room.h"
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_esp.h"
#include "re15_engine.h"
#include "re2_ems.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_spinnen_r38.h"
#include "re15_spinnenanim_r41.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

/* Toleranzen A3/B (Dossier 5). Ueberblendung im Original: FUN_80020510 -> LoadAverageShort12 @0x800664e0 = GTE gpf12
 * (MAC = IR0*prev >> 12, ABGESCHNITTEN) und danach gpl12 (MAC = (MAC<<12 + IR0*neu) >> 12); der Port mischt
 * (prev*wp + neu*wc) >> 12 in einem Schritt (skeleton_common.c re15_blend_angle12) -> in Ueberblend-Takten bis zu EINE
 * Winkeleinheit Unterschied (gemessen nur in Takt 62-68 und 102-107 = Ueberblendung 7..1 nach den Clipwechseln).
 *  W: eine Winkeleinheit = 2*pi*4096/4096 = 6,28 je Q12-Eintrag, mal Massstab 0.8 = 5,03 -> 6, + 1 Rundung des
 *     Matrixprodukts = 7 (gemessen max 6).
 *  T: Wurzel-Versatz ebenfalls gpf12/gpl12 (@0x8001f4d8 / @0x8001f500): 1 Einheit, mal 0.8, + 1 Produkt-Rundung = 2
 *     (gemessen max 1).
 *  GIFT: Anker = M x (R0 x (500,0,0) + t0) + Lage, drei >>12-Produkte -> 2 (gemessen 0). */
#ifndef RE15_R41_TOL_W
#define RE15_R41_TOL_W 7
#endif
#ifndef RE15_R41_TOL_T
#define RE15_R41_TOL_T 2
#endif
#ifndef RE15_R41_TOL_GIFT
#define RE15_R41_TOL_GIFT 2
#endif

static int s_fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: "); printf(__VA_ARGS__); printf("\n"); s_fails++; } \
                           else { printf("ok    "); printf(__VA_ARGS__); printf("\n"); } } while (0)

/* ---- Original-Tabelle ------------------------------------------------------------------------------------- */
typedef struct {
    int     takt, zustand, phase;
    int32_t pos[3];
    int16_t rot[3];
    int     clip, bild_nach, f8f_nach;
    int     boden;                 /* +0x1ba (Boden-Referenz, Schatten-Hoehe @0x80110b4c) */
    int16_t M[9], P0W[9];
    int32_t P0T[3];
    int16_t root[3];
} orig_t;
static const orig_t s_orig[] = {
#include "test_r41_spinnenanim_orig.inc"
};
static const int s_orig_n = (int)(sizeof s_orig / sizeof s_orig[0]);
static const orig_t *orig_takt(int K)
{
    for (int i = 0; i < s_orig_n; i++) if (s_orig[i].takt == K) return &s_orig[i];
    return NULL;
}
/* EM025-Cliplaengen (spinnen_r38.c, CDEMD0.EMS Blob 10): Clip 16 = 32, 13 = 32, 2 = 16, 0 = 16. */
static int clip_laenge(int c) { return (c >= 0 && c < 17) ? re15_spinnen_r38_em025_clip_len[c] : 1; }
/* Original: gezeichnetes Bild = +0x95 VOR dem Takt = (Bild nach - 1) mod Laenge (FUN_8001f3bc @0x8001f610-3c). */
static int orig_bild(const orig_t *o)
{
    int fc = clip_laenge(o->clip);
    return (o->bild_nach + fc - 1) % fc;
}
/* Original: gezeichnete Ueberblendung = +0x8f VOR dem Takt (@0x8001f408); +0x8f zaehlt nur bei > 0 herunter
 * (@0x8001f5b0-b4) -> nach > 0: vorher = nach + 1; nach == 0: vorher 1, wenn der Vortakt 1 hinterliess, sonst 0. */
static int orig_frac(int K)
{
    const orig_t *o = orig_takt(K), *v = orig_takt(K - 1);
    if (!o) return -1;
    if (o->f8f_nach > 0) return o->f8f_nach + 1;
    return (v && v->f8f_nach == 1 && v->clip == o->clip) ? 1 : 0;
}

/* ---- Raum / Takt (Muster test_r38_spinnen.c) -------------------------------------------------------------- */
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static re15_esp_t         s_esp;
static uint8_t           *s_rdt_buf = NULL;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static uint32_t u32(const uint8_t *b, size_t o) { return (uint32_t)b[o] | ((uint32_t)b[o+1] << 8) | ((uint32_t)b[o+2] << 16) | ((uint32_t)b[o+3] << 24); }

static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static int bank_re2(uint8_t type)
{
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &s_re2_n);
    if (!s_re2_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_re2_ems, s_re2_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return 1; }
    eb->type = 0; return 0;
}
static uint8_t *s_re15_ems = NULL; static size_t s_re15_n = 0;
static uint8_t  s_blob[0x80000];
static int bank_re15(uint8_t type)
{
    if (!s_re15_ems) s_re15_ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_re15_n);
    if (!s_re15_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_re15_ems, s_re15_n, idx, &off, &len) != 0) return 0;
    if (len > sizeof s_blob) return 0;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    memcpy(s_blob, s_re15_ems + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) { eb->type = 0; return 0; }
    eb->ok = 1; eb->buf = NULL;
    return 1;
}

static int raum2060(re15_ai_flavor_t fl)
{
    size_t n = 0;
    free(s_rdt_buf);
    s_rdt_buf = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2060.RDT", &n);
    if (!s_rdt_buf) { printf("RDT fehlt\n"); return 0; }
    memset(&g_room_rdt, 0, sizeof g_room_rdt);
    if (re15_rdt_parse(s_rdt_buf, n, &g_room_rdt) != 0) return 0;
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;
    memset(&s_esp, 0, sizeof s_esp);
    if (re15_esp_parse(s_rdt_buf, n, u32(s_rdt_buf, 0x4C), u32(s_rdt_buf, 0x50), u32(s_rdt_buf, 0x54),
                       u32(s_rdt_buf, 0x58), &s_esp) == 0)
        re15_esp_set_room_bank(&s_esp);
    else
        re15_esp_set_room_bank(NULL);
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(fl);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    re15_esp_fx_reset();
    re15_spinnen_r38_reset();
    re15_spinnenanim_r41_reset();
    g_engine.frame_count = 0;
    g_current_room_id = 0x2060;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0; pl->status_flags = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = 7600; pl->z = 3950; pl->rot_y = 2048;
    re15_player_apply_hitbox(pl);
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    scd_room_reenter(&s_rdt, 7600, 3950, 0);
    pl->x = 7600; pl->y = 0; pl->z = 3950; pl->rot_y = 2048;
    if (fl == RE15_AI_FLAVOR_RE2) { (void)bank_re2(0x25); (void)bank_re2(0x26); }
    else                          { (void)bank_re15(0x25); }
    return 1;
}

/* Gezeichneter Wurzelteil der Spinne wie platform/pc/main.c (Pose-Klammer + Figur-Matrix): Weltmatrix W = M x R0,
 * Weltlage T = M x t0 + Lage. Rueckgabe 0 = keine Bank. */
typedef struct { int bild, frac, kf; int32_t M[9], W[9], T[3]; int16_t root[3]; } zug_t;
static int zeichnen(re15_actor_t *e, zug_t *z)
{
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || !b->ok) return 0;
    uint32_t ncur = (uint32_t)e->anim_frame;
    re15_spinnenanim_r41_zeichnen_beginn(e, &ncur, 30);
    z->bild = (int)ncur; z->frac = e->anim_frac;
    z->kf = re15_compute_actor_kf(&b->anim, &b->skel, e, -1, ncur);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    g_anim_pose_actor = e;
    int rc = re15_skel_compute_pose(&b->skel, z->kf, poses);
    re15_spinnenanim_r41_zeichnen_ende(e);
    g_anim_pose_actor = NULL;
    if (rc != 0) return 0;
    int32_t M[9];
    if (re15_spinnenanim_r41_volle_drehung(e)) re15_spinnenanim_r41_figur_matrix(e, M);
    else { re15_actor_t t = *e; t.rot_x = 0; t.rot_z = 0; re15_spinnenanim_r41_figur_matrix(&t, M); }
    memcpy(z->M, M, sizeof M);
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            int64_t s = 0;
            for (int k = 0; k < 3; k++) s += (int64_t)M[r*3+k] * poses[0].rot[k*3+c];
            z->W[r*3+c] = (int32_t)(s >> 12);
        }
        int64_t t = 0;
        for (int k = 0; k < 3; k++) t += (int64_t)M[r*3+k] * poses[0].trans[k];
        z->T[r] = (int32_t)(t >> 12) + (r == 0 ? e->x : r == 1 ? e->y : e->z);
        z->root[r] = (int16_t)poses[0].trans[r];
    }
    return 1;
}
/* Zeichenweg VOR Runde 41: nur Gier, kein Massstab, Bild/Ueberblendung nach dem Vorschub (Gegenprobe A4). */
static void matrix_alt(const re15_actor_t *e, int32_t M[9])
{
    int32_t c = re15_cos_q12((int)e->rot_y), s = re15_sin_q12((int)e->rot_y);
    int32_t m[9] = { c, 0, s, 0, 0x1000, 0, -s, 0, c };
    memcpy(M, m, sizeof m);
}

static void bild(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
    re15_esp_fx_tick(re15_esp_room_bank());
}

static re15_actor_t *spinne_bei(int32_t x, int32_t z)
{
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (e->active && e->type == 0x25 && e->x == x && e->z == z) return e;
    }
    return NULL;
}

static int iabs(int v) { return v < 0 ? -v : v; }

/* ============================== A ============================== */
static void teil_a(re15_ai_flavor_t fl, const char *name)
{
    const int re2 = (fl == RE15_AI_FLAVOR_RE2);
    printf("\n== A %s: ROOM2060, Original-Ablauf (Spieler Bild 60 -> (6000,0,3000), Cut 1), Takt 61..134\n", name);
    if (!raum2060(fl)) { s_fails++; return; }
    re15_actor_t *e = spinne_bei(5800, 6400);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    CHECK(e != NULL, "%s: Schachtspinne (5800,-3600,6400) gespawnt", name);
    if (!e) return;
    zug_t z;
    for (int f = 0; f < 60; f++) { bild(); (void)zeichnen(e, &z); g_engine.frame_count++; }
    pl->x = 6000; pl->z = 3000; pl->rot_y = 1024;              /* wie gdb_shots.py --pl 6000,3000,1024 --tp 60 */
    for (int f = 0; f < 2; f++) { bild(); (void)zeichnen(e, &z); g_engine.frame_count++; }
    g_scd.work_vars[0x0A] = 1;                                  /* angezeigter Cut 1 -> sub01 -> sub02 (grid 0x42) */

    /* Takte sammeln: T = erster Takt mit Phase 1 = Original-Takt 62 (Zeile n63, z 6200). */
    enum { NMAX = 140 };
    static zug_t zs[NMAX]; static re15_actor_t es[NMAX]; static int K_von[NMAX];
    int T = -1, n = 0;
    zug_t halt; int halt_ok = 0; re15_actor_t halt_e;
    {   /* der Takt VOR dem Ausloeser = Original-Takt 61 (Halten) */
        halt_ok = zeichnen(e, &halt); halt_e = *e;
    }
    for (int f = 0; f < 120 && n < NMAX; f++) {
        bild();
        zug_t zz; int ok = zeichnen(e, &zz);
        g_engine.frame_count++;
        if (T < 0 && e->sub_state_3 == 1) T = f;
        if (T >= 0 && ok) { zs[n] = zz; es[n] = *e; K_von[n] = 62 + (f - T); n++; }
        if (T >= 0 && 62 + (f - T) >= 134) break;
    }
    CHECK(T >= 0 && n >= 73, "%s: Ablauf erfasst (%d Takte ab dem Ausloeser)", name, n);

    /* A0 Halten (Original-Takt 61): Bild 0 der Haltepose, Massstab, Gier-Matrix */
    {   const orig_t *o = orig_takt(61);
        int gl = halt_ok && o;
        for (int k = 0; k < 9 && gl; k++) gl = (halt.M[k] == o->M[k]);
        CHECK(gl && halt.bild == orig_bild(o) && halt.frac == orig_frac(61),
              "%s: Halten im Schacht (Takt 61): Figur-Matrix == Original %s, gezeichnetes Bild %d (Original %d = INIT-anim_set "
              "@0x80110d60 vor dem Hochzaehlen), Ueberblendung %d (Original %d)", name, gl ? "ja" : "NEIN",
              halt.bild, o ? orig_bild(o) : -1, halt.frac, orig_frac(61));
    }

    int m_gleich = 0, b_gleich = 0, n_cmp = 0, wand_m = 0, wand_n = 0;
    int w_max = 0, t_max = 0, r_max = 0, w_K = -1, t_K = -1;
    int alt_wand_falsch = 0, alt_bild_falsch_93 = 0, alt_frac_falsch = 0, lage_gleich = 0;
    int schatten_gleich = 0, schatten_boden_wand = 0, alt_schatten_luft = 0;
    for (int i = 0; i < n; i++) {
        const int K = K_von[i];
        const orig_t *o = orig_takt(K);
        if (!o) continue;
        n_cmp++;
        /* Logik (Runde 38) unveraendert: Lage/Drehung == Original */
        /* y: im Abstieg (Phase 4/6) zieht der Port andere Zufallszahlen (+100 + rng&0xf @0x801150dc-e8 / @0x80115198-bc)
         * -> Abweichung hoechstens 15 je Abstiegstakt; x, z und die Drehwerte exakt. */
        if (es[i].x == o->pos[0] && es[i].z == o->pos[2] && iabs(es[i].y - o->pos[1]) <= 15 * 20
            && es[i].rot_x == o->rot[0] && es[i].rot_y == o->rot[1] && es[i].rot_z == o->rot[2]) lage_gleich++;
        else printf("   Takt %d: Lage (%d,%d,%d) rot (%d,%d,%d) / Original (%d,%d,%d) (%d,%d,%d)\n", K, es[i].x, es[i].y, es[i].z,
                    es[i].rot_x, es[i].rot_y, es[i].rot_z, o->pos[0], o->pos[1], o->pos[2], o->rot[0], o->rot[1], o->rot[2]);
        /* A1 Figur-Matrix */
        int mg = 1;
        for (int k = 0; k < 9; k++) if (zs[i].M[k] != o->M[k]) mg = 0;
        if (mg) m_gleich++;
        if (o->rot[2] == 1024) { wand_n++; if (mg) wand_m++; }
        /* A2 Bild + Ueberblendung */
        int ob = orig_bild(o), of = orig_frac(K);
        if (zs[i].bild == ob && zs[i].frac == of) b_gleich++;
        else printf("   Takt %d: Bild %d/%d Ueberblendung %d/%d (Port/Original)\n", K, zs[i].bild, ob, zs[i].frac, of);
        /* A3 Wurzelteil */
        int dw = 0, dt = 0, dr = 0;
        for (int k = 0; k < 9; k++) if (iabs(zs[i].W[k] - o->P0W[k]) > dw) dw = iabs(zs[i].W[k] - o->P0W[k]);
        /* Weltlage RELATIV zur Figur (T - Lage): der Abstieg y += 100 + (rng & 0xf) (@0x801150dc-e8) zieht im Port
         * andere Zufallszahlen als die Messung -> absolute y weichen ab, der Versatz des Wurzelteils nicht. */
        const int32_t pp[3] = { es[i].x, es[i].y, es[i].z };
        for (int k = 0; k < 3; k++) {
            int d = iabs((zs[i].T[k] - pp[k]) - (o->P0T[k] - o->pos[k]));
            if (d > dt) dt = d;
        }
        for (int k = 0; k < 3; k++) if (iabs(zs[i].root[k] - o->root[k]) > dr) dr = iabs(zs[i].root[k] - o->root[k]);
        if (dw > 2) printf("   Takt %d: Wurzelteil |dW| %d (Bild %d Ueberblendung %d)\n", K, dw, zs[i].bild, zs[i].frac);
        if (dw > w_max) { w_max = dw; w_K = K; }
        if (dt > t_max) { t_max = dt; t_K = K; }
        if (dr > r_max) r_max = dr;
        /* A5 Schatten: Viereck +-1300 x +-1400 (FUN_8001af5c @0x80110d90, Record+0xc/+0xe; Messung
         * L_belege/orig_2060_gdb_schatten.log: Takt 1..140 unveraendert 1300/1400) auf +0x1ba (@0x80110b4c). */
        {   int32_t hx = 500, hz = 600, sy = es[i].y;               /* main.c-Standard vorher: 500 x 600 auf Figur-y */
            re15_spinnenanim_r41_schatten_groesse(&es[i], &hx, &hz);
            re15_spinnenanim_r41_schatten_y(&es[i], &sy);
            if (hx == 1300 && hz == 1400 && sy == o->boden) schatten_gleich++;
            else printf("   Takt %d: Schatten %dx%d y %d / Original 1300x1400 y %d\n", K, hx, hz, sy, o->boden);
            if (o->rot[2] == 1024 && sy == 0) schatten_boden_wand++;
            if (o->rot[2] == 1024 && es[i].y != o->boden) alt_schatten_luft++;   /* vorher: Figur-y = in der Luft */
        }
        /* A4 Gegenprobe alter Zeichenweg */
        int32_t Ma[9]; matrix_alt(&es[i], Ma);
        int ag = 1; for (int k = 0; k < 9; k++) if (Ma[k] != o->M[k]) ag = 0;
        if (o->rot[2] == 1024 && !ag) alt_wand_falsch++;
        if (K == 93 && es[i].anim_frame != ob) alt_bild_falsch_93 = 1;   /* alter Weg: Bild nach dem Vorschub */
        if (of > 0 && es[i].anim_frac != of) alt_frac_falsch++;
    }
    printf("   %s: Takte verglichen %d; Wurzelteil max |dW| %d (Takt %d), max |dT relativ| %d (Takt %d), max |dWurzel| %d\n",
           name, n_cmp, w_max, w_K, t_max, t_K, r_max);
    CHECK(lage_gleich == n_cmp, "%s: Logik unveraendert: Lage + Drehung in %d/%d Takten == Original (Runde 38)", name,
          lage_gleich, n_cmp);
    CHECK(m_gleich == n_cmp && wand_n == 40 && wand_m == 40,
          "%s: A1 Figur-Matrix in %d/%d Takten == Original +0x20 (Wand rot_z 1024: %d/%d Takte, Original 93..132 = 40)",
          name, m_gleich, n_cmp, wand_m, wand_n);
    CHECK(b_gleich == n_cmp, "%s: A2 gezeichnetes Bild + Ueberblendung in %d/%d Takten == Original (+0x95/+0x8f vor dem "
          "Vorschub)", name, b_gleich, n_cmp);
    /* Toleranz A3: Matrixeintraege und Weltlage Teil 0. Original rechnet Teil 0 per GTE (FUN_80022da0, Ueberblendung
     * GPF/GPL 12 Bit + FUN_80020510), der Port in C mit >>12 je Produkt -> Rundung. Wert belegt im Dossier 5. */
    CHECK(w_max <= RE15_R41_TOL_W && t_max <= RE15_R41_TOL_T,
          "%s: A3 Wurzelteil wie gezeichnet == Original Teil 0 (Weltmatrix max %d <= %d, Weltlage max %d <= %d)",
          name, w_max, RE15_R41_TOL_W, t_max, RE15_R41_TOL_T);
    CHECK(schatten_gleich == n_cmp && schatten_boden_wand == 40 && alt_schatten_luft == 40,
          "%s: A5 Schatten in %d/%d Takten == Original (1300 x 1400 auf +0x1ba); an der Wand %d/40 auf dem Boden y 0 "
          "(vorher %d/40 auf Figur-y in der Luft)", name, schatten_gleich, n_cmp, schatten_boden_wand, alt_schatten_luft);
    CHECK(alt_wand_falsch == 40 && alt_bild_falsch_93 && alt_frac_falsch > 0,
          "%s: A4 Gegenprobe: alter Zeichenweg verfehlt das Original (Wand-Takte %d/40 falsche Matrix, Takt 93 Bild falsch %d, "
          "Ueberblendung falsch in %d Takten)", name, alt_wand_falsch, alt_bild_falsch_93, alt_frac_falsch);
    (void)re2; (void)halt_e;
}

/* ============================== B ============================== */
static void teil_b(void)
{
    printf("\n== B RE1.5-KI: Spucken nach der Landung (Original Bild 136: Klumpen bei (5741,-320,4049))\n");
    if (!raum2060(RE15_AI_FLAVOR_RE15)) { s_fails++; return; }
    re15_actor_t *e = spinne_bei(5800, 6400);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!e) { CHECK(0, "Schachtspinne fehlt"); return; }
    zug_t z;
    for (int f = 0; f < 60; f++) { bild(); (void)zeichnen(e, &z); g_engine.frame_count++; }
    pl->x = 6000; pl->z = 3000; pl->rot_y = 1024;
    for (int f = 0; f < 2; f++) { bild(); (void)zeichnen(e, &z); g_engine.frame_count++; }
    g_scd.work_vars[0x0A] = 1;
    int32_t p[3] = {0, 0, 0}; int gef = 0;
    for (int f = 0; f < 140 && !gef; f++) {
        bild(); (void)zeichnen(e, &z); g_engine.frame_count++;
        for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
            const re15_esp_fx_t *fx = re15_esp_fx_get(i);
            if (fx && fx->rows_base && fx->effect_id == 0x0d) { p[0] = fx->wpos[0]; p[1] = fx->wpos[1]; p[2] = fx->wpos[2]; gef = 1; break; }
        }
    }
    printf("   erster Giftklumpen Port (%d,%d,%d), Original (5741,-320,4049)\n", p[0], p[1], p[2]);
    CHECK(gef && iabs(p[0] - 5741) <= RE15_R41_TOL_GIFT && iabs(p[1] + 320) <= RE15_R41_TOL_GIFT
          && iabs(p[2] - 4049) <= RE15_R41_TOL_GIFT,
          "B Gift-Anker mit Figur-Matrix (Massstab 0xccc): Abweichung <= %d je Achse", RE15_R41_TOL_GIFT);
}

/* ============================== pruef_log (echte exe) ============================== */
/* Zeilen "L41 F<bild> s<platz> st<z>/<u1>/<ph> c<clip> b<bild> f<frac> M<9 Werte>" (spinnenanim_r41.c Messschiene,
 * RE15_SPINNENANIM_LOG). Ausrichtung: erstes Bild mit Phase 1 = Original-Takt 62. */
static int pruef_log(const char *pfad)
{
    FILE *fp = fopen(pfad, "r");
    if (!fp) { printf("FAIL: Log fehlt: %s\n", pfad); return 1; }
    char z[512]; int T = -1, n = 0, m_gl = 0, b_gl = 0, wand = 0;
    while (fgets(z, sizeof z, fp)) {
        unsigned fb; int pl, st, u1, ph, cl, bi, fr; int M[9];
        if (sscanf(z, "L41 F%u s%d st%d/%d/%d c%d b%d f%d M%d,%d,%d,%d,%d,%d,%d,%d,%d", &fb, &pl, &st, &u1, &ph, &cl, &bi, &fr,
                   &M[0], &M[1], &M[2], &M[3], &M[4], &M[5], &M[6], &M[7], &M[8]) != 17) continue;
        if (pl != 1) continue;                                  /* Platz 1 = Spinne e0 (5800,-3600,6400) */
        if (T < 0 && st == 4 && u1 == 1 && ph == 1) T = (int)fb;
        if (T < 0) continue;
        int K = 62 + ((int)fb - T);
        const orig_t *o = orig_takt(K);
        if (!o || K > 133) continue;
        n++;
        int mg = 1; for (int k = 0; k < 9; k++) if (M[k] != o->M[k]) mg = 0;
        if (mg) m_gl++;
        if (o->rot[2] == 1024 && mg) wand++;
        if (bi == orig_bild(o) && fr == orig_frac(K)) b_gl++;
        else printf("   exe Takt %d: Bild %d/%d Ueberblendung %d/%d\n", K, bi, orig_bild(o), fr, orig_frac(K));
    }
    fclose(fp);
    CHECK(T >= 0 && n == 72, "exe: Ablauf der Spinne im Log (%d Takte 62..133)", n);
    CHECK(m_gl == n && wand == 40, "exe: Figur-Matrix des Zeichners in %d/%d Takten == Original +0x20 (Wand %d/40)", m_gl, n, wand);
    CHECK(b_gl == n, "exe: gezeichnetes Bild + Ueberblendung in %d/%d Takten == Original", b_gl, n);
    return s_fails ? 1 : 0;
}

int main(int argc, char **argv)
{
    if (argc >= 3 && strcmp(argv[1], "pruef_log") == 0) {
        int rc = pruef_log(argv[2]);
        printf("\n%s (%d FAIL)\n", rc ? "FEHLER" : "OK", s_fails);
        return rc;
    }
    printf("test_r41_spinnenanim — Original-Tabelle %d Takte\n", s_orig_n);
    teil_a(RE15_AI_FLAVOR_RE15, "RE1.5-KI");
    teil_a(RE15_AI_FLAVOR_RE2, "RE2-KI");
    teil_b();
    printf("\n%s (%d FAIL)\n", s_fails ? "FEHLER" : "OK", s_fails);
    return s_fails ? 1 : 0;
}
