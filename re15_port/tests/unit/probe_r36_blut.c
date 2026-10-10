/**
 * @file probe_r36_blut.c
 * @brief Runde 36 Spur C — MESSSONDE + PIN fuer die Lage des Bluts am getroffenen RE2-Zombie, JEDER Emitter
 *        bis zum Tod (Nachbesserung 1: Abnahme 0 M1/M2/M3).
 *
 * Nutzer: "Bei den Zombies habe ich manchmal das Gefuehl, dass das Blut nicht an der richtigen Stelle auftaucht,
 * wenn sie getroffen werden."
 *
 * ORIGINAL (RE2 EMZ0.BIN, selbst disassembliert, Dossier C_schuss.md §2.2 / §8): jeder Emitter ruft
 * FUN_8001BF10(id, Gier, a2 = Part-Weltmatrix +0x198 + part*172 + 72, a3 = Versatz im ANKER-Rahmen); FUN_8001BF10
 * liest a3 nur (@0x8001BFE8-FFC), der RE2-Lagen-Tick 0x8001D894 setzt Lage = Anker.R * a3 + Anker.T
 * (@0x8001DB74-DC1C). Die a3 je Emitter (P8 = lokale Translation von Part 8, `lw 1420/1424/1428`):
 *   HURT    Haupt-Handler P0 @0x80105650-708 : Zone!=0 Part 0 (P8.x-100*s, P8.y+300, P8.z) | Zone 0 Part 1 (0,800,0)
 *   HURT16A Stagger P0 @0x80105D14-DBC       : dito, s = +0x16A
 *   FLINCH  Wurzel-Flinch @0x801050B0-158    : Zone!=0 Part 0 (P8.x, P8.y+500, P8.z) | Zone 0 Part 1 (0,800,0)
 *   NACH    Stagger P1 @0x80105F5C-FF8       : gerades +0x15A, Part 0, a3 = +0x144-Vektor, y - 500 (+0x222 != 0)
 *   AUF     Aufsteh-Treffer @0x80107B14-30 / @0x80107BA0-B4 : Part 8 (0,300,0)
 *   TOD     @0x8010864C-6B4                   : Zone!=0 Part 0 (P8.x, P8.y+300, P8.z) | Zone 0 Part 1 (0,800,0)
 *   TROPF   Tod P1 @0x8010887C-D4             : Part 0 (P8.x, P8.y+500, P8.z)
 *   KRIECH  Kriecher-Tod @0x80108A84-D4       : Part 0 (0,300,0)
 *   (Tod des Liegenden FUN_801099E4 @0x80109B30-60: Part 0 (P8.x, P8.y+300, P8.z) = TOD-Sollpunkt ohne Zonen-Weiche;
 *    LIEGE = EXEC[5] P6 @0x801035B0-DC mit a3 = (0,0,0), nur in diesem Kontext ein Original-Sollpunkt)
 *   MG      0x80107EF0 @0x80107FC8-801080C4  : Zufallsvektor sp+16 (Variante "mg", eigene Pruefung, s. main)
 * Die Sonde liest JEDEN Spawn aus dem Mess-Ring (re15_schuss_r36_fx_*), rechnet alle Sollpunkte aus der
 * GEZEICHNETEN Pose (g_anim_pose_actor = Aktor, wie der Renderer) der Bilder T-1 und T und ordnet den Spawn dem
 * naechsten Sollpunkt zu. BECKEN/PART1/PART8 (Anker OHNE a3) sind die Defekt-Kandidaten des alten Stands.
 *
 * Pin (Modus "pin"):
 *  (1) jeder Spawn an Part 0/1/8 dieses Zombies liegt <= 40 an einem Original-Sollpunkt UND naeher an ihm als an
 *      jedem Defekt-Kandidaten;
 *  (2) Todesbild: ein Spawn an Part 0/1 liegt <= 40 am TOD-, ZONE0- oder KRIECH-Sollpunkt;
 *  (3) ein Part-8-Spawn in einem Trefferbild liegt <= 40 am AUF-Sollpunkt;
 *  (4) Zaehler: Tod erreicht, >= 1 HURT, >= 1 NACH, >= 1 AUF, eben >= 1 Tropfblut (mg: >= 5 MG-Stoesse);
 *  (5) Mechanik: y des Versatzes im ANKER-Rahmen (Anker.R^T * (Lage - Anker.T)) im Band der Familie
 *      (HURT/TOD/NACH < -400, FLINCH/TROPF < -200, ZONE0 > 600, AUF/KRIECH > 200) — stehend wie liegend.
 *
 * Aufruf: probe_r36_blut [mess|pin] [eben|tief|mg|rip|arm|burst]  (mg = Ingram 12 von +z, MG-Blutstoss 0x80107EF0)
 *
 * NACHBESSERUNG 2 (Abnahme 1 M1, Dossier C_schuss.md §9): "der Zeichner" dieser Sonde ist jetzt ein ZWILLING DES
 * PORT-RENDERERS (platform/pc/main.c: Bankwahl 9798-9925 — Loco-Bank in den Paar-1-Phasen, clip_override bei +0x1C4 & 4,
 * Sitz-Import —, Crossfade 9960 einmal je Bild, Unterhaelfte 10195-10220, Ragdoll-Bodenanker 10315-10330, freifliegende
 * Teile samt Physik ueber re15_re2z_gore_part_matrix 10339). Vorher rechnete er immer die Hauptbank: in den Paar-1-Phasen
 * zeichnet der Renderer aber die Loco-Bank (gemessen 56-121 Abstand), und ohne den Gore-Haken flog nie ein Teil.
 * Neue Pruefung (G) fuer JEDEN Spawn dieses Zombies, mit und ohne a3, jeder Part (Zensus-Gruppe B, §8.8):
 *   Lage <= Schranke an R*a3 + T der gezeichneten Matrix — Spawn im Spielschritt: Skelett-Pose dieses Bilds (Teil, das
 *   erst jetzt abfliegt: genau die Pose, aus der der Renderer es seedet) bzw. die eigene Matrix eines schon fliegenden
 *   Teils; Spawn WAEHREND des Zeichnens (Aufschlag-Blut FUN_80028AD8 @0x80028C34-4C): die eigene Matrix des Teils.
 *   Schranke 40, in MG-Trefferbildern 100 (der MG-P0 dreht/schiebt den Zombie NACH dem Spawn @0x8010810C-6C).
 *   Varianten: rip = SPAS 13 von +x (RIP-Kopf @0x80108E28, RIP-P1 @0x80109108/@0x8010913C, Aufschlag-Blut der Beine),
 *   arm = Remington 8 von +z tief (Arm-Abriss, 4. Spawn a3 = 0 @0x80107704), burst = Remington 8 von +x (Ragdoll-Emitter
 *   Part 1 @0x80106D7C im Bodenanker), mg zusaetzlich Gliedabriss @0x80105368/@0x801053D0 + Aufschlag-Blut.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_emd.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_engine.h"
#include "re2_ems.h"
#include "re15_schuss_r36.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern re15_actor_t g_actors[];
extern re15_enemy_bank_t *re15_enemy_alloc(uint8_t type);
extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_compute_actor_kf(const re15_emd_animation_t *anim, const re15_emd_skeleton_t *skel,
                                  const re15_actor_t *a, int clip_override, uint32_t cur);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_ems = NULL; static size_t s_ems_n = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static void load_bank2(uint8_t type)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems_n);
    if (!s_ems) return;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return;
    if (re2_ems_load_bank(s_ems, s_ems_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; }
    else eb->type = 0;
}
static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

/* "Der Zeichner" = Zwilling des Port-Renderers (Kopf, Nachbesserung 2). Je Knochen: R/T = Skelett-Pose (vor dem
 * Gore-Haken; fuer ein Teil, das in diesem Bild zum ersten Mal mit eigener Matrix gezeichnet wird, genau die Seed-Pose),
 * Rd/Td = das, was gezeichnet wird (eigene Matrix eines fliegenden Teils nach seinem Physikschritt). rot/t: Parts 0/1/8
 * (k = 0/1/2) fuer die Familien-Pruefungen der Varianten eben/tief/mg. */
#define MAXB RE15_EMD_MAX_BONES
typedef struct {
    int ok, n, bank;                                   /* bank: 0 Hauptbank, 1 Loco-Bank, 2 RE1.5-Sitz-Import */
    int32_t R[MAXB][9], T[MAXB][3], Rd[MAXB][9], Td[MAXB][3]; int flieg[MAXB];
    int32_t rot[3][9]; int32_t t[3][3]; int32_t p8loc[3];
} drawn_t;
static void zeichne(re15_actor_t *e, drawn_t *d)
{
    memset(d, 0, sizeof *d);
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || !b->ok || b->skel.bone_count < 9) return;
    /* Bankwahl wie main.c:9798-9925 (Gegner-Zweig von re15_actor_anim_select, dann Loco-Bank / Sitz-Import). */
    re15_anim_banks_t banks; memset(&banks, 0, sizeof banks);
    re15_anim_view_t av; re15_actor_anim_select(e, 0, &banks, &av);
    const re15_emd_skeleton_t *sk = av.skel; const re15_emd_animation_t *an = av.anim;
    if (re15_actor_uses_loco_bank(e)) {
        if (b->loco_ok && (int)e->motion < b->anim_loco.clip_count) {
            sk = &b->skel_loco; an = &b->anim_loco; av.clip_override = (int)e->motion; d->bank = 1;
        }
    }
    if (e->re2z_re15_pose && e->state != 4 && !re15_actor_uses_own_bank(e)) {   /* Kette main.c:9852-9925 */
        const re15_emd_skeleton_t *rs = re15_re2z_re15_pose_skel(); const re15_emd_animation_t *ra = re15_re2z_re15_pose_anim();
        if (rs && ra && (int)e->motion < ra->clip_count) { sk = rs; an = ra; av.clip_override = (int)e->motion; d->bank = 2; }
    }
    if (!sk || !an || an->clip_count <= 0) return;
    int kf = re15_compute_actor_kf(an, sk, e, av.clip_override, e->anim_frame);
    re15_skel_pose_t poses[MAXB], low[MAXB]; int low_ok = 0;
    void *save = g_anim_pose_actor;
    g_anim_pose_actor = e;                                 /* wie main.c:9960 — der Crossfade-Zustand laeuft mit */
    int rv = re15_skel_compute_pose(sk, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return;
    if (e->re2z_rag_anchor_on) {                           /* Unterhaelfte (main.c:10195-10220) */
        const re15_emd_skeleton_t *ls = NULL; int lkf = re15_re2z_lower_kf(e, &ls);
        g_anim_pose_actor = NULL;
        low_ok = (re15_skel_compute_pose(ls ? ls : sk, lkf, low) == 0);
        g_anim_pose_actor = save;
    }
    int32_t c = re15_cos_q12(e->rot_y), s = re15_sin_q12(e->rot_y);
    int32_t ny[9] = { c, 0, s, 0, 0x1000, 0, -s, 0, c };
    uint8_t gd[MAXB]; uint32_t gt[MAXB]; uint8_t gm[MAXB];
    for (int i = 0; i < MAXB; i++) { gd[i] = 1u; gt[i] = 0x00808080u; gm[i] = (uint8_t)i; }
    int gore_on = re15_re2z_gore_resolve(e, sk->bone_parent, sk->bone_count < MAXB ? sk->bone_count : MAXB, gd, gt, gm);
    d->n = sk->bone_count < MAXB ? sk->bone_count : MAXB;
    for (int k = 0; k < d->n; k++) {
        const re15_skel_pose_t *np = (e->re2z_rag_anchor_on && k >= 1 && k <= 6 && low_ok) ? &low[k] : &poses[k];
        for (int r = 0; r < 3; r++) {
            int64_t a = 0;
            for (int q = 0; q < 3; q++) {
                d->R[k][r*3+q] = (int32_t)(((int64_t)ny[r*3+0]*np->rot[0*3+q] + (int64_t)ny[r*3+1]*np->rot[1*3+q] +
                                            (int64_t)ny[r*3+2]*np->rot[2*3+q]) >> 12);
                a += (int64_t)ny[r*3+q] * np->trans[q];
            }
            d->T[k][r] = (int32_t)(a >> 12);
        }
        d->T[k][0] += e->x; d->T[k][1] += e->y; d->T[k][2] += e->z;
        if (e->re2z_rag_anchor_on) {                       /* Ragdoll-Bodenanker (main.c:10315-10330) */
            int32_t p0w[3];
            for (int r = 0; r < 3; r++)
                p0w[r] = (int32_t)(((int64_t)ny[r*3+0]*poses[0].trans[0] + (int64_t)ny[r*3+1]*poses[0].trans[1] +
                                    (int64_t)ny[r*3+2]*poses[0].trans[2]) >> 12);
            p0w[0] += e->x; p0w[1] += e->y; p0w[2] += e->z;
            re15_re2z_ragdoll_part_anchor(e, k, p0w, d->T[k]);
        }
        memcpy(d->Rd[k], d->R[k], sizeof d->Rd[k]); memcpy(d->Td[k], d->T[k], sizeof d->Td[k]);
        if (gore_on)                                       /* eigene Matrix + Physik (main.c:10339) */
            d->flieg[k] = re15_re2z_gore_part_matrix(e, k, g_engine.frame_count, d->Rd[k], d->Td[k]);
    }
    static const int P[3] = { 0, 1, 8 };
    for (int k = 0; k < 3; k++) { memcpy(d->rot[k], d->R[P[k]], sizeof d->rot[k]); memcpy(d->t[k], d->T[P[k]], sizeof d->t[k]); }
    for (int k = 0; k < 3; k++) d->p8loc[k] = b->skel.bone_relative_pos[8][k];
    d->ok = 1;
}
static void appl_k(const int32_t R[9], const int32_t T[3], const int32_t a[3], int32_t out[3])
{
    for (int r = 0; r < 3; r++)
        out[r] = T[r] + (int32_t)(((int64_t)R[r*3+0]*a[0] + (int64_t)R[r*3+1]*a[1] + (int64_t)R[r*3+2]*a[2]) >> 12);
}
static void appl(const drawn_t *d, int k, int32_t ax, int32_t ay, int32_t az, int32_t out[3])
{
    for (int r = 0; r < 3; r++)
        out[r] = d->t[k][r] + (int32_t)(((int64_t)d->rot[k][r*3+0]*ax + (int64_t)d->rot[k][r*3+1]*ay +
                                          (int64_t)d->rot[k][r*3+2]*az) >> 12);
}
static int32_t abst(const int32_t a[3], const int32_t b[3])
{
    int64_t dx = a[0]-b[0], dy = a[1]-b[1], dz = a[2]-b[2];
    int64_t q = dx*dx + dy*dy + dz*dz; int32_t r = 0;
    while ((int64_t)(r+1)*(r+1) <= q) r++;
    return r;
}

enum { K_HURT, K_HURT16A, K_FLINCH, K_TOD, K_ZONE0, K_NACH, K_AUF, K_TROPF, K_KRIECH, K_LIEGE,
       K_BECKEN, K_PART1, K_PART8, K_N };
#define K_LEGIT K_BECKEN                        /* < K_LEGIT = Original-Sollpunkt, sonst Defekt (Anker ohne a3) */
static const char *KN[K_N] = { "HURT", "HURT16A", "FLINCH/TROPF", "TOD", "ZONE0", "NACH", "AUF", "TROPF", "KRIECH",
                               "LIEGE(a3=0)", "BECKEN(ohne a3)", "PART1(ohne a3)", "PART8(ohne a3)" };
static void kandidaten(const drawn_t *d, int s, int s16a, int32_t o[K_N][3])
{
    const int32_t px = d->p8loc[0], py = d->p8loc[1], pz = d->p8loc[2];
    appl(d, 0, px - 100 * s,    py + 300, pz, o[K_HURT]);     /* @0x80105680-B4 */
    appl(d, 0, px - 100 * s16a, py + 300, pz, o[K_HURT16A]);  /* @0x80105D2C-70 */
    appl(d, 0, px,              py + 500, pz, o[K_FLINCH]);   /* @0x80105124-48 */
    appl(d, 0, px,              py + 300, pz, o[K_TOD]);      /* @0x8010865C-80 */
    appl(d, 1, 0, 800, 0,                     o[K_ZONE0]);    /* @0x801056DC-F4 / @0x80108694-A8 */
    appl(d, 0, 0, -500, 0,                    o[K_NACH]);     /* @0x80105FBC-C8 (+0x146 - 500) */
    appl(d, 2, 0, 300, 0,                     o[K_AUF]);      /* @0x80107B20-24 */
    appl(d, 0, px,              py + 500, pz, o[K_TROPF]);    /* @0x8010887C-A8 */
    appl(d, 0, 0, 300, 0,                     o[K_KRIECH]);   /* @0x80108ABC-C8 */
    appl(d, 0, 0, 0, 0,                       o[K_LIEGE]);    /* EXEC[5] P6 @0x801035B0-DC: a3 = (0,0,0)
                                                               * (`sh zero,24/26/28(sp)` @0x801035C8-D0), nur im
                                                               * Kontext EXEC[5] P6 gueltig (s. main) */
    appl(d, 0, 0, 0, 0, o[K_BECKEN]); appl(d, 1, 0, 0, 0, o[K_PART1]); appl(d, 2, 0, 0, 0, o[K_PART8]);
}

int main(int argc, char **argv)
{
    int pin  = (argc > 1 && strcmp(argv[1], "pin") == 0);
    int tief = (argc > 2 && strcmp(argv[2], "tief") == 0);
    int mg   = (argc > 2 && strcmp(argv[2], "mg") == 0);     /* Ingram 12 -> RE2-Zeile 15 = MG-Handler 0x80107EF0 */
    /* Nachbesserung 2 — Gruppe-B-Varianten (Kopf): Waffe/Seite/Hoehe wie die Laeufe der Abnahme 1 (abn1_blut). */
    int rip   = (argc > 2 && strcmp(argv[2], "rip") == 0);    /* SPAS 13 von +x: RIP 0x80108BEC */
    int arm   = (argc > 2 && strcmp(argv[2], "arm") == 0);    /* Remington 8 von +z, tief: Arm-Abriss @0x80107680-708 */
    int burst = (argc > 2 && strcmp(argv[2], "burst") == 0);  /* Remington 8 von +x: Ragdoll-Emitter @0x80106D7C */
    int glied = (argc > 2 && strcmp(argv[2], "glied") == 0);  /* Ingram 12 von +z, Dauerfeuer, Saat der Abnahme 1:
                                                               * Gliedabriss @0x80105368/@0x801053D0 + Aufschlag-Blut */
    int grp_b = rip || arm || burst || glied;                 /* nur Pruefung (G) + Zaehler, keine Pistolen-Familien */
    if (arm) tief = 1;
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(glied ? 0x1234567u : 0x0badf00du);  /* glied: Saat von abn1_blut (Abnahme 1 §3.1, F76 Abriss) */
    g_current_room_id = 0x1140;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    load_bank2(0x10); load_bank2(0x11); load_bank2(0x16);
    re15_enemy_bank_t *bk = re15_enemy_find(0x10);
    if (!bk || !bk->ok) { printf("FAIL: RE2-Bank 0x10 fehlt (RE2/CDEMD0.EMS)\n"); return 1; }
    printf("Bank 0x10: bones=%d P8.loc=(%d,%d,%d)\n", bk->skel.bone_count,
           bk->skel.bone_relative_pos[8][0], bk->skel.bone_relative_pos[8][1], bk->skel.bone_relative_pos[8][2]);

    /* Einen stehenden Zombie (Platz 2, (-1800,-19600)) wecken: Leon 1800 vor ihm in +x. */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();
    int waffe = (mg || glied) ? 12 : rip ? 13 : (arm || burst) ? 8 : 3;
    if (waffe != 3) re15_inv_grant(waffe, 250);
    re15_player_set_equipped_weapon(waffe);
    {   int es = re15_inv_equipped_slot(); if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
    int zs = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x10 && g_actors[s].x == -1800 && g_actors[s].z == -19600) zs = s;
    if (zs < 0) { printf("FAIL: Platz (-1800,-19600) nicht gefunden\n"); return 1; }
    re15_actor_t *z = &g_actors[zs];
    /* Nur dieser eine Zombie: alle anderen Plaetze aus (Auto-Zielen und Koerperschub sonst auf Platz 5). */
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != zs) g_actors[s].active = 0;
    pl->x = z->x + 1800; pl->z = z->z; pl->rot_y = 2048;          /* Blick -x auf den Zombie */
    if (mg || arm || glied) { pl->x = z->x; pl->z = z->z + 1800; pl->rot_y = 1024; }  /* MG: von +z (Blick -z), damit der Zombie
                                                                    * NICHT mit Gier 0 steht — Welt- und Anker-Versatz
                                                                    * fielen sonst fast zusammen (Ryaw = 1) */
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); g_engine.frame_count++; }

    static drawn_t prev, cur; memset(&prev, 0, sizeof prev); memset(&cur, 0, sizeof cur);
    unsigned ring0 = re15_schuss_r36_fx_zahl();
    int fails = 0, treffer = 0, tot_ab = -1, steht_seit = -1;
    int n_k[K_N]; memset(n_k, 0, sizeof n_k);
    int n_spawn = 0, n_tod_pflicht = 0, n_tropf = 0, n_mg = 0;
    int64_t mg_sum_anker = 0, mg_sum_welt = 0;
    /* (G)-Zaehler (Nachbesserung 2) */
    int g_n = 0, g_max = 0, g_null = 0, g_zeichnen = 0, g_loco = 0, g_kopf = 0, g_rip1 = 0, g_glied = 0, g_arm4 = 0, g_rag = 0;
    for (int f = 0; f < 2400; f++) {
        pl->hp = 100;
        uint16_t cr = RE15_PAD_BIT_R1 | (tief ? RE15_PAD_BIT_DOWN : 0), ed = 0;
        /* Schuss alle 12 Bilder, sobald der Zombie steht (nicht mehr Fresser EXEC[8]), bis zum Tod. */
        if (steht_seit < 0 && z->state == 1 && z->sub_state_1 != 8) steht_seit = f;
        if (glied) {                                         /* Dauerfeuer wie abn1_blut: Taste gehalten, Flanke je 30 */
            if (tot_ab < 0 && steht_seit >= 0 && f >= steht_seit + 20) {
                cr |= RE15_PAD_BIT_SQUARE; if (((f - steht_seit) % 30) == 0) ed = RE15_PAD_BIT_SQUARE;
            }
        } else if (tot_ab < 0 && steht_seit >= 0 && f >= steht_seit + 20 && ((f - steht_seit - 20) % 12) == 0) {
            cr |= RE15_PAD_BIT_SQUARE; ed = RE15_PAD_BIT_SQUARE;
        }
        int32_t hp_vor = z->hp; unsigned st_vor = z->state, s1_vor = z->sub_state_1, s2_vor = z->sub_state_2;
        unsigned n0 = re15_schuss_r36_fx_zahl();
        frame(cr, ed);
        unsigned n1 = re15_schuss_r36_fx_zahl();                 /* Spawns des Spielschritts: [n0, n1) */
        int32_t roh[MAXB][3];                                    /* alter Anker (rohe Hauptbank-Pose), nur Ausgabe */
        for (int k = 0; k < MAXB; k++) re15_enemy_bone_world_pos(z, k, roh[k]);
        zeichne(z, &cur);                                        /* "der Zeichner" dieses Bildes */
        unsigned n2 = re15_schuss_r36_fx_zahl();                 /* Spawns WAEHREND des Zeichnens: [n1, n2) */
        g_engine.frame_count++;                                  /* wie main.c:11483 — Physik-Takt der Teile */
        int hit  = z->hp < hp_vor;
        int died = (hp_vor >= 0 && z->hp < 0) ||
                   ((st_vor != 3u && st_vor != 7u) && (z->state == 3u || z->state == 7u));
        if (hit) treffer++;
        if (!pin && ((f % 40) == 0 || ed || hit || n2 != n0))
            printf("  f%-4d z st=%u/%u/%u -> %u/%u/%u hp %d->%d 1D2=%u 222=%u 15A=%d 16A=%d 10E=%04X%s\n", f, st_vor, s1_vor,
                   s2_vor, z->state, z->sub_state_1, z->sub_state_2, hp_vor, z->hp, z->re2z_hits1d2, z->re2z_flag222,
                   z->re2z_t15a, (int)(int8_t)z->re2z_dir16a, z->re2z_f10e, ed ? " SCHUSS" : "");
        /* (G) Nachbesserung 2: JEDER Spawn (jeder Part, mit und ohne a3) an der gezeichneten Matrix (Kopf). */
        for (unsigned nr = n0; nr < n2 && cur.ok; nr++) {
            int part, mit; int32_t fx[3], a3[3], ref[3];
            if (!re15_schuss_r36_fx_holen(nr, &part, fx, a3, &mit)) continue;
            if (part < 0 || part >= cur.n) { if (pin) { printf("FAIL: F%d Spawn an Part %d ohne Knochen\n", f, part); fails++; } continue; }
            int beim_zeichnen = (nr >= n1);
            const char *quelle;
            if (beim_zeichnen) { appl_k(cur.Rd[part], cur.Td[part], a3, ref); quelle = "eigene Matrix (Aufschlag)"; }
            else if (prev.ok && prev.flieg[part]) { appl_k(prev.Rd[part], prev.Td[part], a3, ref); quelle = "eigene Matrix (fliegt)"; }
            else { appl_k(cur.R[part], cur.T[part], a3, ref); quelle = cur.flieg[part] ? "Skelett = Seed (fliegt ab)" : "Skelett"; }
            int32_t dg = abst(fx, ref), droh = abst(fx, roh[part]);
            int schranke = ((mg || glied) && hit && !beim_zeichnen) ? 100 : 40;   /* MG-P0 dreht/schiebt NACH dem Spawn @0x8010810C-6C */
            printf("F%-4d (G) %s part=%2d fx=(%d,%d,%d) a3=%s(%d,%d,%d) bank %s rag %u -> %s d=%d (Schranke %d) | roh d=%d\n",
                   f, beim_zeichnen ? "Z" : " ", part, fx[0], fx[1], fx[2], mit ? "" : "ohne ", a3[0], a3[1], a3[2],
                   cur.bank == 1 ? "loco" : cur.bank == 2 ? "re15" : "haupt", (unsigned)z->re2z_rag_anchor_on, quelle, dg,
                   schranke, droh);
            g_n++; if (dg > g_max) g_max = dg;
            if (!mit || (a3[0] == 0 && a3[1] == 0 && a3[2] == 0)) g_null++;
            if (beim_zeichnen) g_zeichnen++;
            if (cur.bank == 1) g_loco++;
            if (z->re2z_rag_anchor_on && !mit) g_rag++;
            if (died && part == 8 && !mit) g_kopf++;                     /* RIP-Kopf @0x80108E28 */
            if (rip && died && part == 0 && !mit) g_rip1++;              /* RIP-P1 @0x80109108/@0x8010913C (Port a3 = 0) */
            if (!beim_zeichnen && !mit && (part == 9 || part == 12 || part == 10 || part == 13) && !died) g_glied++;
            if (arm && part == 3 && !mit) g_arm4++;                      /* Arm-Abriss 4. Spawn @0x80107704 */
            if (pin && dg > schranke) {
                printf("FAIL: F%d Spawn part=%d %s liegt %d von der gezeichneten Matrix (%s), Schranke %d (roh %d)\n", f, part,
                       mit ? "mit a3" : "ohne a3", dg, quelle, schranke, droh); fails++;
            }
        }
        int tod_treffer_ok = 0, tod_spawns = 0;
        for (unsigned nr = n0; nr < n1 && !grp_b; nr++) {
            int part, mit; int32_t fx[3], a3[3];
            if (!re15_schuss_r36_fx_holen(nr, &part, fx, a3, &mit)) continue;
            int32_t zw[3] = { z->x, z->y, z->z };
            if (abst(fx, zw) > 3000) continue;                    /* nur dieser Zombie */
            if (part != 0 && part != 1 && part != 8) {
                if (!pin) printf("F%-4d SPAWN part=%d (%d,%d,%d) — anderer Anker, nicht bewertet\n", f, part, fx[0], fx[1], fx[2]);
                continue;
            }
            if (!cur.ok || !prev.ok) continue;
            n_spawn++;
            int zone = (int)(z->re2z_hits1d2 % 3u);
            int s    = (z->re2z_hitdir1d0 & 0x20u) ? -1 : 1;
            int s16a = (int)(int8_t)z->re2z_dir16a;
            int32_t kp[K_N][3], kc[K_N][3]; int32_t d[K_N];
            kandidaten(&prev, s, s16a, kp); kandidaten(&cur, s, s16a, kc);
            int best = 0, bestdef = K_LEGIT;
            int liege_ctx = (st_vor == 1u && s1_vor == 5u && s2_vor == 6u);   /* EXEC[5] P6 */
            for (int k = 0; k < K_N; k++) {
                int32_t a = abst(fx, kp[k]), b = abst(fx, kc[k]);
                d[k] = a < b ? a : b;
                if (k == K_LIEGE && !liege_ctx) d[k] = 0x7fffffff;
                if (k < K_LEGIT && d[k] < d[best]) best = k;
                if (k >= K_LEGIT && d[k] < d[bestdef]) bestdef = k;
            }
            const char *art = died ? "TOD-Bild" : hit ? "Treffer" : "ohne Treffer";
            printf("F%-4d SPAWN part=%d fx=(%d,%d,%d) a3=%s(%d,%d,%d) [%s, zone %d] -> %s d=%d | Defekt %s d=%d | "
                   "Becken y=%d\n", f, part, fx[0], fx[1], fx[2], mit ? "" : "ohne ", a3[0], a3[1], a3[2], art, zone,
                   KN[best], d[best], KN[bestdef], d[bestdef], cur.t[0][1]);
            if (mg && hit && !died && (part == 0 || part == 1)) {
                /* MG-Blutstoss 0x80107EF0 @0x80107FC8-801080C4: a3 = sp+16 ZUFALLSVEKTOR — Zone != 0 Part 0
                 * (256-2r, 312-4r, 256-2r) @0x80108000-38, Zone 0 Part 1 (127-r, 927-r, 127-r) @0x8010806C-98.
                 * Gepinnt: (a) der Ring-a3 liegt in diesem Band (r = 0..255, @0x80016004), (b) die Lage ist
                 * Anker.R * a3 + Anker.T (gezeichnete Pose T-1/T) <= 40 — vorher Welt-Versatz. */
                int k = (part == 0) ? 0 : 1;
                int32_t ep[3], ec[3];
                appl(&prev, k, a3[0], a3[1], a3[2], ep); appl(&cur, k, a3[0], a3[1], a3[2], ec);
                int32_t dm = abst(fx, ep) < abst(fx, ec) ? abst(fx, ep) : abst(fx, ec);
                int32_t wv[3] = { (k ? cur.t[1][0] : cur.t[0][0]) + a3[0], (k ? cur.t[1][1] : cur.t[0][1]) + a3[1],
                                  (k ? cur.t[1][2] : cur.t[0][2]) + a3[2] };
                int band = (zone != 0) ? (part == 0 && a3[0] >= -254 && a3[0] <= 256 && a3[1] >= -708 && a3[1] <= 312 &&
                                          a3[2] >= -254 && a3[2] <= 256)
                                       : (part == 1 && a3[0] >= -128 && a3[0] <= 127 && a3[1] >= 672 && a3[1] <= 927 &&
                                          a3[2] >= -128 && a3[2] <= 127);
                printf("F%-4d   MG a3=(%d,%d,%d) -> Anker.R*a3+Anker.T d=%d | Welt-Versatz d=%d | Band %s\n", f, a3[0], a3[1],
                       a3[2], dm, abst(fx, wv), band ? "ok" : "VERLETZT");
                n_mg++; mg_sum_anker += dm; mg_sum_welt += abst(fx, wv);
                /* Schranke 100 statt 40: der MG-P0 dreht den Zombie NACH dem Spawn um +64-(r&0x7f) (@0x8010815C-6C)
                 * und schiebt ihn 20..83 zurueck (@0x80108110-48) — die Sonde sieht nur die Pose vor (T-1) und nach
                 * (T) dem ganzen Bild. Trennschaerfe ueber die Summe unten (Anker-Modell gegen Welt-Versatz). */
                if (pin && (!mit || !band || dm > 100)) {
                    printf("FAIL: F%d MG-Blut: a3 %s, Band %s, d(Anker.R*a3+Anker.T) = %d (Schranke 100)\n", f,
                           mit ? "da" : "FEHLT", band ? "ok" : "verletzt", dm); fails++;
                }
                continue;
            }
            n_k[best]++;
            if (tot_ab >= 0 && f > tot_ab && (best == K_FLINCH || best == K_TROPF)) n_tropf++;   /* Tod P1 @0x8010887C-D4 */
            /* (1) naechster Sollpunkt ist ein Original-Emitter, <= 40, naeher als jeder Defekt-Kandidat */
            if (pin && (d[best] > 40 || (d[bestdef] <= d[best] && best != K_LIEGE))) {   /* LIEGE = Becken mit a3 0 */
                printf("FAIL: F%d Spawn part=%d liegt %d vom naechsten Original-Sollpunkt %s, %d vom Defekt-Kandidaten %s "
                       "(Schranke 40)\n", f, part, d[best], KN[best], d[bestdef], KN[bestdef]); fails++;
            }
            /* (3) Part-8-Spawn eines Treffers = Aufsteh-Treffer @0x80107B14-30 */
            if (pin && hit && part == 8 && !(best == K_AUF && d[K_AUF] <= 40)) {
                printf("FAIL: F%d Part-8-Treffer-Blut nicht am AUF-Sollpunkt (d=%d)\n", f, d[K_AUF]); fails++;
            }
            if (died && (part == 0 || part == 1)) {
                /* Todes-Emitter: TOD @0x8010864C-6B4 (Zone!=0 TOD, Zone 0 ZONE0), Tod des Liegenden FUN_801099E4
                 * @0x80109B30-60 (immer TOD-Versatz an Part 0), Kriecher-Tod @0x80108A84-D4 (KRIECH) — welcher
                 * Handler laeuft, entscheidet der Port-Dispatcher; gepinnt wird: einer der drei Sollpunkte <= 40
                 * und naeher als jeder Defekt-Kandidat (Anker ohne a3). */
                tod_spawns++;
                int32_t dt = d[K_TOD];
                if (d[K_ZONE0] < dt) dt = d[K_ZONE0];
                if (d[K_KRIECH] < dt) dt = d[K_KRIECH];
                if (dt <= 40 && dt < d[bestdef]) tod_treffer_ok = 1;
            }
            /* (5) Mechanik: der Versatz liegt im ANKER-Rahmen. Die Sonde dreht (fx - Anker.T) mit Anker.R^T zurueck
             * (gezeichnete Pose T-1 bzw. T, die naehere) und prueft die y-Komponente gegen das a3 der Familie:
             * HURT/HURT16A/TOD (P8.y+300 = -483) und NACH (-500) < -400; FLINCH/TROPF (P8.y+500 = -283) < -200;
             * ZONE0 (+800 an Part 1) > 600; AUF (+300 an Part 8) und KRIECH (+300 an Part 0) > 200. Vorher (Anker
             * ohne a3) war diese Komponente ~0 — unabhaengig davon, ob der Koerper steht oder liegt. */
            if (pin) {
                int k = -1; int32_t lo = 0, hi = 0;
                if (best == K_HURT || best == K_HURT16A || best == K_TOD || best == K_NACH) { k = 0; lo = -100000; hi = -400; }
                if (best == K_FLINCH || best == K_TROPF)                                    { k = 0; lo = -100000; hi = -200; }
                if (best == K_KRIECH)                                                       { k = 0; lo = 200; hi = 100000; }
                if (best == K_ZONE0)                                                        { k = 1; lo = 600; hi = 100000; }
                if (best == K_AUF)                                                          { k = 2; lo = 200; hi = 100000; }
                if (k >= 0) {
                    const drawn_t *dd = (abst(fx, kp[best]) <= abst(fx, kc[best])) ? &prev : &cur;
                    int32_t v[3] = { fx[0] - dd->t[k][0], fx[1] - dd->t[k][1], fx[2] - dd->t[k][2] };
                    int32_t ly = (int32_t)(((int64_t)dd->rot[k][0*3+1]*v[0] + (int64_t)dd->rot[k][1*3+1]*v[1] +
                                            (int64_t)dd->rot[k][2*3+1]*v[2]) >> 12);
                    if (ly < lo || ly > hi) {
                        printf("FAIL: F%d %s-Blut: Versatz im Anker-Rahmen y=%d ausserhalb [%d,%d]\n", f, KN[best], ly, lo, hi);
                        fails++;
                    }
                }
            }
        }
        if (died) {
            printf("F%-4d TOD hp %d->%d st %u/%u/%u -> %u/%u/%u 10E=%04X\n", f, hp_vor, z->hp, st_vor, s1_vor, s2_vor,
                   z->state, z->sub_state_1, z->sub_state_2, z->re2z_f10e);
            /* (2) das Todesbild traegt den TOD-/ZONE0-/KRIECH-Spawn */
            if (tod_treffer_ok) n_tod_pflicht++;
            else if (pin && !grp_b) { printf("FAIL: F%d Todesbild ohne TOD-Spawn am Sollpunkt (%d Spawns an Part 0/1)\n", f, tod_spawns); fails++; }
        }
        prev = cur;
        if (tot_ab < 0 && died) tot_ab = f;
        if ((mg || glied) && (z->re2z_f10e & 1u)) {     /* Kriecher (+0x10E Bit 0): danach Griff/Biss, kein Treffer-Emitter */
            printf("F%-4d Kriecher (10E=%04X) — MG-Lauf endet hier\n", f, z->re2z_f10e);
            break;
        }
        if (tot_ab >= 0 && f > tot_ab + 80) break;
    }
    printf("Treffer %d, Spawns %d (Ring %u..%u), Tod bei Bild %d, Tropfblut nach dem Tod %d\n", treffer, n_spawn, ring0,
           re15_schuss_r36_fx_zahl(), tot_ab, n_tropf);
    printf("Zuordnung:"); for (int k = 0; k < K_N; k++) if (n_k[k]) printf(" %s=%d", KN[k], n_k[k]); printf("\n");
    printf("(G): %d Spawns, max d %d, davon ohne a3 %d, beim Zeichnen (Aufschlag) %d, Loco-Bank %d, Ragdoll ohne a3 %d, "
           "RIP-Kopf %d, RIP-P1 %d, Gliedabriss %d, Arm-Abriss 4. Spawn %d\n", g_n, g_max, g_null, g_zeichnen, g_loco, g_rag,
           g_kopf, g_rip1, g_glied, g_arm4);
    if (pin && grp_b) {
        /* Zaehler je Gruppe-B-Variante (gemessen, Dossier §9.6): der Lauf muss die Emitter wirklich sehen. */
        if (tot_ab < 0) { printf("FAIL: Zombie nicht gestorben\n"); fails++; }
        if (rip && g_kopf < 1)     { printf("FAIL: kein RIP-Kopf-Spawn (Part 8, a3 0 @0x80108E28)\n"); fails++; }
        if (rip && g_rip1 < 1)     { printf("FAIL: kein RIP-P1-Nachbluten (Part 0 @0x80109108/@0x8010913C)\n"); fails++; }
        if (rip && g_zeichnen < 1) { printf("FAIL: kein Aufschlag-Blut eines fliegenden Teils (@0x80028C4C)\n"); fails++; }
        if (arm && g_arm4 < 1)     { printf("FAIL: kein Arm-Abriss-Spawn ohne a3 (@0x80107704)\n"); fails++; }
        if (burst && g_rag < 1)    { printf("FAIL: kein Ragdoll-Spawn ohne a3 (@0x80106D7C)\n"); fails++; }
        if (glied && g_glied < 1)  { printf("FAIL: kein Gliedabriss-Spawn (@0x80105368/@0x801053D0)\n"); fails++; }
        if (glied && g_zeichnen < 1) { printf("FAIL: kein Aufschlag-Blut des abgerissenen Glieds (@0x80028C4C)\n"); fails++; }
        if (fails) { printf("probe_r36_blut: %d FAILURES\n", fails); return 1; }
        printf("probe_r36_blut: OK\n");
        return 0;
    }
    if (pin) {
        /* (4) Zaehler: der Lauf muss die Emitter wirklich treffen, die er pinnen soll */
        if (!mg && tot_ab < 0)                  { printf("FAIL: Zombie nicht gestorben\n"); fails++; }
        if (!mg && n_tod_pflicht < 1)           { printf("FAIL: kein TOD-Spawn am Sollpunkt\n"); fails++; }
        if (!mg && n_k[K_HURT] + n_k[K_HURT16A] + n_k[K_FLINCH] + n_k[K_ZONE0] < 1)
                                                { printf("FAIL: kein Treffer-Spawn (HURT/FLINCH/ZONE0)\n"); fails++; }
        /* je Variante gemessen (Dossier §8.3): beide Laeufe treffen Nachbluten (Stagger P1) und Aufsteh-Treffer, der
         * ebene Lauf zusaetzlich das Tropfblut nach dem Tod — der Pin muss diese Emitter wirklich sehen. */
        if (mg) printf("MG: %d Stoesse, Summe d(Anker-Modell) %lld, Summe d(Welt-Versatz) %lld\n", n_mg,
                       (long long)mg_sum_anker, (long long)mg_sum_welt);
        if (mg && 2 * mg_sum_anker >= mg_sum_welt) {
            printf("FAIL: MG-Blut folgt nicht dem Anker-Rahmen (Summe %lld gegen Welt-Versatz %lld)\n",
                   (long long)mg_sum_anker, (long long)mg_sum_welt); fails++;
        }
        if (mg && n_mg < 5) { printf("FAIL: weniger als 5 MG-Blutstoesse (0x80107EF0 @0x801080C4)\n"); fails++; }
        if (!mg && n_k[K_NACH] < 1) { printf("FAIL: kein Nachblut-Spawn (Stagger P1 @0x80105F5C-FF8)\n"); fails++; }
        if (!mg && n_k[K_AUF] < 1)  { printf("FAIL: kein Aufsteh-Treffer (Part 8 @0x80107B14-30)\n"); fails++; }
        if (!tief && !mg && n_tropf < 1) { printf("FAIL: kein Tropfblut nach dem Tod (@0x8010887C-D4)\n"); fails++; }
        if (fails) { printf("probe_r36_blut: %d FAILURES\n", fails); return 1; }
        printf("probe_r36_blut: OK\n");
    }
    return 0;
}
