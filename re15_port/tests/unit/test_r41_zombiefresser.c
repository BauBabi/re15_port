/*
 * Runde 41 Spur I "zombiefresser" — die 0x84/0x85-Zombies in ROOM2000/2001/3000/3010 LIEGEN und stehen auf, wenn
 * man ihnen naeher kommt (RE1.5 Wecker 0x80103814 + Liege-Phasenmaschine 0x801038ec, STAGE2). Beide KI-Varianten,
 * echte RDT/SCD/SCA, game_step. Dossier analysis/befunde_runde41/I_zombiefresser.md, Code engine/src/zombiefresser_r41.c.
 *
 *   A  ZENSUS je Raum (2000, 3000, 3010 und die Elza-Raeume 2001, 3001, 3011): jeder 0x84/0x85-Spawn liegt nach dem INIT —
 *      RE2-KI: Zustand 1 / Sub 7 (EXEC[7], RE2 0x701 @0x801009E8-0x80100A84), Latch +0x10E & 0x4000, Clip 23/22, KEIN
 *      Sub 8 (EXEC[8] Fressen); RE1.5-KI: grid bleibt 0x84/0x85, Sub 0, Phase 0, Clip 0x12/0x13 (@0x80100c7c/cc0).
 *   B  RUHE: Spieler weiter als 4000 von allen -> 300 Bilder keiner steht auf, alle unschiessbar (+0x93 Bit 0 —
 *      RE1.5 P0 @0x80103940-4c, RE2 +0x1D3 |= 0x80 @0x80103804-14 / Trefferfilter).
 *   C  WECKEN je Raum: Spieler 500..1500 vor EINEM Zombie (2000/2001 auf dem Oststeg ueber dem Kanal, 3000/3010 auf
 *      derselben Ebene): Wecken im ersten KI-Takt mit d < 4000, Aufstehen beginnt nach (rand&0xf)+1 <= 16 Takten
 *      (@0x80103858-74), der Aufsteh-Clip laeuft vollstaendig (RE2 Clip 8/9, RE1.5 Clip 0x12/0x13) und der Zombie
 *      steht binnen 17 + Cliplaenge + 4 Bildern (RE2 0x101 grid 0 @0x80103900; RE1.5 0x201 grid 0 @0x801039d0-e0),
 *      danach greift er an (Griff Sub 3/4 oder Spieler-HP sinkt) binnen 900 Bildern (Kanalraum 2000/2001: je KI
 *      in der Aufstellung, in der sie die Kante nimmt — RE1.5 von (-21300,-10100), RE2 vom Oststeg (-20300,-11600)).
 *   D  GRENZE 0xFA0 (@0x8010382c) am gemessenen +0x1d0 (SquareRoot0 trifft nicht jeden Wert): kleinster Versatz
 *      mit +0x1d0 >= 4000 -> 300 Bilder kein Wecken; groesster mit +0x1d0 < 4000 -> Wecken.
 *   E  WARTEZEIT ueber 48 Seeds: Abstand Wecken -> Latch frei in [1, 16], mindestens 10 verschiedene Werte.
 *   F  GEGENPROBE echter Fresser 0x86 (ROOM1140): RE2-KI weiter EXEC[8] (Sub 8), RE1.5-KI grid 0 / Sub 0xc.
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
#include "re15_ems.h"
#include "re15_esp.h"
#include "re15_gameflow.h"
#include "re2_ems.h"
#include "re15_wasserzombie_r38.h"
#include "re15_zombiefresser_r41.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int s_fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); s_fails++; } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* ---- Gegnerbaenke (Muster test_r38_wasserzombie.c) -------------------------------------------------------------- */
static uint8_t *s_ems15 = NULL; static size_t s_ems15_n = 0;
typedef struct { uint8_t type; uint8_t *buf; size_t len; re15_md1_t md; re15_emd_skeleton_t sk; re15_emd_animation_t an; } emd15_t;
static emd15_t s_emd15[6]; static int s_emd15_n = 0;
static emd15_t *emd15(uint8_t type)
{
    for (int i = 0; i < s_emd15_n; i++) if (s_emd15[i].type == type) return &s_emd15[i];
    if (s_emd15_n >= 6) return NULL;
    if (!s_ems15) s_ems15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_ems15_n);
    if (!s_ems15) return NULL;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_ems15, s_ems15_n, idx, &off, &len) != 0) return NULL;
    emd15_t *m = &s_emd15[s_emd15_n];
    m->buf = (uint8_t *)malloc(len); if (!m->buf) return NULL;
    memcpy(m->buf, s_ems15 + off, len); m->len = len; m->type = type;
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (re15_emd_parse_container(m->buf, len, &m->md, &m->sk, &m->an, &tim) != 0) { free(m->buf); return NULL; }
    s_emd15_n++;
    return m;
}
static int load_bank_re15(uint8_t type)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    emd15_t *m = emd15(type); if (!m) return 0;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (re15_emd_parse_container(m->buf, m->len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) { eb->type = 0; return 0; }
    eb->ok = 1; eb->buf = NULL;
    eb->own_ok  = (re15_emd_parse_own_bank(m->buf, m->len, &eb->skel_own, &eb->anim_own) == 0);
    eb->loco_ok = (re15_emd_parse_loco_bank(m->buf, m->len, &eb->skel_loco, &eb->anim_loco) == 0);
    return 1;
}
static uint8_t *s_ems2 = NULL; static size_t s_ems2_n = 0;
static int load_bank_re2(uint8_t type)
{
    if (!s_ems2) s_ems2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems2_n);
    if (!s_ems2) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_ems2, s_ems2_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return 1; }
    eb->type = 0; return 0;
}

/* ---- Raumlauf (game_step-Reihenfolge wie test_r38_wasserzombie.c) ----------------------------------------------- */
static re15_rdt_t         s_rdt;
static uint8_t           *s_rdt_buf = NULL;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

static int raum_laden(const char *rel, unsigned room)
{
    char p[512]; snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, rel);
    size_t n = 0;
    free(s_rdt_buf); s_rdt_buf = slurp(p, &n);
    if (!s_rdt_buf) { printf("FAIL: %s nicht lesbar\n", p); return 0; }
    if (re15_rdt_parse(s_rdt_buf, n, &g_room_rdt) != 0) { printf("FAIL: RDT-Parse %s\n", p); return 0; }
    g_room_rdt_ok = 1; s_rdt = g_room_rdt;
    g_current_room_id = room;
    return 1;
}

static void bild(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

typedef struct { const char *rdt; unsigned room; int elza; int band; uint8_t flags[3][2]; int nflags; } raum_t;

/* Spawn-Bedingungen aus den Raum-Skripten (tools/r41_zombiefresser/scd_dump.py):
 *   2000 sub00 @0x01794 `If Ck(4,0xF4,1)` -> Else-Zweig spawnt (Flag 0 = Standard)
 *   2001 sub00 @0x01826 `If Ck(3,0x45,1)` (und @0x01802 Ck(3,0x44,1) = 0) -> spawnt
 *   3000 main00 @0x014b6 `If Ck(4,9,0)` -> Else spawnt; sub00 @0x01564 Ck(4,0xC6,0) = Marvin-Szene (aus)
 *   3010 main00 @0x0210e `If Ck(4,0x0A,0)` -> Else spawnt */
static const raum_t R2000 = { "STAGE2/ROOM2000.RDT", 0x2000, 0, 1, {{0,0}}, 0 };
static const raum_t R2001 = { "STAGE2/ROOM2001.RDT", 0x2001, 1, 1, {{3,0x45}}, 1 };
static const raum_t R3000 = { "STAGE3/ROOM3000.RDT", 0x3000, 0, 0, {{4,0x09},{4,0xC6}}, 2 };
static const raum_t R3010 = { "STAGE3/ROOM3010.RDT", 0x3010, 0, 0, {{4,0x0A}}, 1 };
/* Elza-Varianten: dieselben Saetze und Bedingungen (3001 main00 @0x014b6 Ck(4,9,0); 3011 main00 @0x02112 Ck(4,0x0A,0)) */
static const raum_t R3001 = { "STAGE3/ROOM3001.RDT", 0x3001, 1, 0, {{4,0x09},{4,0xC6}}, 2 };
static const raum_t R3011 = { "STAGE3/ROOM3011.RDT", 0x3011, 1, 0, {{4,0x0A}}, 1 };

static void raum_start(const raum_t *r, re15_ai_flavor_t fl, int32_t px, int32_t pz, int16_t prot, uint32_t seed)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(fl);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_esp_fx_reset();
    re15_damage_seed_rng(seed);
    re15_wasserzombie_reset();
    re15_wasserzombie_bank_vergessen();
    g_gameflow.character = r->elza ? 4 : 0;
    re15_game_state_init();                          /* Flags je Lauf frisch (Teile unabhaengig) */
    for (int i = 0; i < r->nflags; i++) re15_game_flag_set(r->flags[i][0], r->flags[i][1], 1);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = (uint8_t)r->band; pl->y = -1800 * r->band;
    pl->x = px; pl->z = pz; pl->rot_y = prot;
    re15_collision_set_band(r->band);
    re15_inv_load_briefing();
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    pl->x = px; pl->z = pz; pl->y = -1800 * r->band; pl->floor = (uint8_t)r->band; pl->rot_y = prot;
    static const uint8_t typen[3] = { 0x10, 0x11, 0x16 };
    for (int i = 0; i < 3; i++) {
        if (fl == RE15_AI_FLAVOR_RE2) (void)load_bank_re2(typen[i]);
        else                          (void)load_bank_re15(typen[i]);
        /* RE1.5-Aktionsbank je Typ an das Hochzieh-Modul melden (Runde 38 U) — das tut in der exe der Hybrid-
         * Lader (platform/pc/main.c); ohne sie zieht sich ein RE2-KI-Kanalzombie nicht an der Kante hoch. */
        emd15_t *m = emd15(typen[i]);
        if (m) re15_wasserzombie_bank_merken(typen[i], &m->sk, &m->an);
    }
}

static int ist_liegewecker(const re15_actor_t *e)
{
    return e->active && e->type >= 0x10 && e->type <= 0x1f &&
           ((e->grid_id == 0x84u) || (e->grid_id == 0x85u));
}

static int slot_bei(int32_t x, int32_t z)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *e = &g_actors[s];
        if (e->active && e->type >= 0x10 && e->type <= 0x1f && e->x == x && e->z == z) return s;
    }
    return -1;
}

static int liegt(const re15_actor_t *e, int re2)
{
    if (re2) return e->state == 1 && e->sub_state_1 == 7 && e->sub_state_2 <= 1 && (e->re2z_f10e & 0x4000u);
    return e->state == 1 && e->sub_state_1 == 0 && e->sub_state_2 <= 1 && ist_liegewecker(e);
}

/* ---- A: Zensus ------------------------------------------------------------------------------------------------- */
static void teil_a(const raum_t *r, const char *name, int erwartet)
{
    for (int k = 0; k < 2; k++) {
        const int re2 = (k == 1);
        raum_start(r, re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15, 30000, 30000, 0, 0x2545f491u);
        g_actors[RE15_ACTOR_SLOT_PLAYER].x = 30000; g_actors[RE15_ACTOR_SLOT_PLAYER].z = 30000;
        for (int f = 0; f < 12; f++) { g_actors[0].x = 30000; g_actors[0].z = 30000; bild(); }
        int n = 0, ok = 0, fress = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            const re15_actor_t *e = &g_actors[s];
            if (!ist_liegewecker(e)) continue;
            n++;
            int clip_ok = re2 ? (e->motion == 23 || e->motion == 22)
                              : (e->motion == ((e->grid_id & 0x1f) == 4 ? 0x12 : 0x13));
            if (liegt(e, re2) && clip_ok) ok++;
            if (e->state == 1 && e->sub_state_1 == 8) fress++;
            printf("  [A %s %s] slot %2d typ %02x grid %02x Wort %d/%d/%d f10e %04x Clip %d Bild %d\n", name,
                   re2 ? "RE2" : "RE1.5", s, e->type, e->grid_id, e->state, e->sub_state_1, e->sub_state_2,
                   e->re2z_f10e, (int)e->motion, (int)e->anim_frame);
        }
        CHECK(n == erwartet, "A %s %s: %d Liegende mit Naehe-Wecker gespawnt, erwartet %d", name,
              re2 ? "RE2" : "RE1.5", n, erwartet);
        CHECK(ok == n, "A %s %s: nur %d von %d liegen (Wort/Latch/Clip)", name, re2 ? "RE2" : "RE1.5", ok, n);
        CHECK(fress == 0, "A %s %s: %d Zombies FRESSEN (Sub 8)", name, re2 ? "RE2" : "RE1.5", fress);
    }
}

/* ---- B: Ruhe --------------------------------------------------------------------------------------------------- */
static void teil_b(const raum_t *r, const char *name, int32_t px, int32_t pz)
{
    for (int k = 0; k < 2; k++) {
        const int re2 = (k == 1);
        raum_start(r, re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15, px, pz, 0, 0x1234567u);
        int auf = 0, schiessbar = 0, n = 0; unsigned dmin = 0xffffffffu;
        for (int f = 0; f < 300; f++) {
            g_actors[0].x = px; g_actors[0].z = pz; g_actors[0].hp = 100;
            bild();
            if (f < 3) continue;
            for (int s = 1; s < RE15_ACTOR_MAX; s++) {
                const re15_actor_t *e = &g_actors[s];
                if (!ist_liegewecker(e) && !(e->active && e->type >= 0x10 && e->type <= 0x1f)) continue;
                if (f == 3) n++;
                if (e->ai_dist < dmin) dmin = e->ai_dist;
                if (!liegt(e, re2)) auf++;
                if (!(e->hit_react & 1u)) schiessbar++;
            }
        }
        printf("  [B %s %s] %d Zombies, kleinster Abstand %u, 300 Bilder: %d Bild-Slots nicht liegend, %d schiessbar\n",
               name, re2 ? "RE2" : "RE1.5", n, dmin, auf, schiessbar);
        CHECK(n > 0 && dmin >= 4000u, "B %s: Aufbau falsch (n=%d dmin=%u)", name, n, dmin);
        CHECK(auf == 0, "B %s %s: %d Bild-Slots ausserhalb der Liege-Pose bei d >= 4000", name, re2 ? "RE2" : "RE1.5", auf);
        CHECK(schiessbar == 0, "B %s %s: %d Bild-Slots schiessbar (Riegel +0x93 Bit 0 fehlt)", name,
              re2 ? "RE2" : "RE1.5", schiessbar);
    }
}

/* ---- C: Wecken, Aufstehen, Angriff ------------------------------------------------------------------------------ */
typedef struct { int weck, frei, auf_beginn, steht, angriff, clip_len, clip_max, auf_clip; unsigned d0; } verlauf_t;

static void verfolgen(int slot, int re2, int bilder, int32_t px, int32_t pz, int16_t prot, verlauf_t *v)
{
    memset(v, 0, sizeof *v);
    v->weck = v->frei = v->auf_beginn = v->steht = v->angriff = -1;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e = &g_actors[slot];
    for (int f = 0; f < bilder; f++) {
        pl->x = px; pl->z = pz; pl->rot_y = prot;
        int hp0 = pl->hp;
        int16_t t9c_vor = e->ai_timer;
        int lag_vor = liegt(e, re2);
        bild();
        if (f == 0) v->d0 = e->ai_dist;
        if (re2) {
            if (v->weck < 0 && t9c_vor == (int16_t)RE15_R41_UNSCHARF &&
                (e->ai_timer != (int16_t)RE15_R41_UNSCHARF || !(e->re2z_f10e & 0x4000u)))
                v->weck = f;                                   /* Wartezeit 0: Latch faellt im Wecktakt */
            if (v->weck >= 0 && v->frei < 0 && !(e->re2z_f10e & 0x4000u)) v->frei = f;
            if (v->auf_beginn < 0 && e->state == 1 && e->sub_state_1 == 7 && e->sub_state_2 >= 3) {
                v->auf_beginn = f; v->auf_clip = e->motion; v->clip_len = re15_actor_clip_len(e);
            }
            if (v->auf_beginn >= 0 && v->steht < 0 && e->state == 1 && e->sub_state_1 == 7 && (int)e->anim_frame > v->clip_max)
                v->clip_max = e->anim_frame;
            if (v->steht < 0 && e->state == 1 && e->sub_state_1 == 1 && e->grid_id == 0) v->steht = f;
        } else {
            if (v->weck < 0 && lag_vor && e->sub_state_2 >= 1) v->weck = f;
            if (v->weck >= 0 && v->frei < 0 && e->sub_state_2 >= 2) v->frei = f;
            if (v->auf_beginn < 0 && e->state == 1 && e->sub_state_1 == 0 && e->sub_state_2 == 2) {
                v->auf_beginn = f; v->auf_clip = e->motion; v->clip_len = re15_actor_clip_len(e);
            }
            if (v->auf_beginn >= 0 && v->steht < 0 && (int)e->anim_frame > v->clip_max) v->clip_max = e->anim_frame;
            if (v->steht < 0 && e->state == 1 && e->sub_state_1 == 2 && e->grid_id == 0) v->steht = f;
        }
        if (v->steht >= 0 && v->angriff < 0 &&
            (re15_player_is_grabbed() || pl->hp < hp0 ||
             (e->state == 1 && (e->sub_state_1 == 3 || e->sub_state_1 == 4))))
            v->angriff = f;
        if (pl->hp < 60) pl->hp = 100;                     /* am Leben halten, nur den Angriff zaehlen */
        if (v->angriff >= 0 && f > v->angriff + 2) break;
    }
}

static void teil_c(const raum_t *r, const char *name, int32_t zx, int32_t zz, int32_t px, int32_t pz, int16_t prot,
                   unsigned dlo, unsigned dhi, int angriff_pflicht)
{
    for (int k = 0; k < 2; k++) {
        const int re2 = (k == 1);
        const char *ki = re2 ? "RE2" : "RE1.5";
        raum_start(r, re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15, 30000, 30000, prot, 0x0badf00du);
        for (int f = 0; f < 3; f++) { g_actors[0].x = 30000; g_actors[0].z = 30000; bild(); }
        int s = slot_bei(zx, zz);
        CHECK(s > 0, "C %s %s: Zombie (%d,%d) nicht gespawnt", name, ki, (int)zx, (int)zz);
        if (s <= 0) continue;
        CHECK(liegt(&g_actors[s], re2), "C %s %s: Zombie liegt vor dem Naeherkommen nicht", name, ki);
        /* die anderen weit weg parken, damit nur dieser Zombie gemessen wird */
        for (int o = 1; o < RE15_ACTOR_MAX; o++)
            if (o != s && g_actors[o].active && g_actors[o].type >= 0x10 && g_actors[o].type <= 0x1f) {
                g_actors[o].x = -30000; g_actors[o].z = -30000;
            }
        verlauf_t v;
        verfolgen(s, re2, 900, px, pz, prot, &v);
        const re15_actor_t *e = &g_actors[s];
        printf("  [C %s %s] slot %d d0=%u weckt F%d frei F%d (Wartezeit %d) Aufstehen F%d Clip %d Laenge %d (max Bild %d) "
               "steht F%d angriff F%d | Ende Wort %d/%d/%d grid %02x @(%d,%d,%d)\n",
               name, ki, s, v.d0, v.weck, v.frei, v.frei - v.weck, v.auf_beginn, v.auf_clip, v.clip_len, v.clip_max,
               v.steht, v.angriff, e->state, e->sub_state_1, e->sub_state_2, e->grid_id, (int)e->x, (int)e->y, (int)e->z);
        CHECK(v.d0 >= dlo && v.d0 <= dhi, "C %s %s: Aufbau d0=%u nicht in %u..%u", name, ki, v.d0, dlo, dhi);
        CHECK(v.weck == 0, "C %s %s: Wecken in Bild %d statt im ersten Takt mit d < 4000", name, ki, v.weck);
        CHECK(v.frei >= 0 && v.frei - v.weck >= 0 && v.frei - v.weck <= 16,
              "C %s %s: Wartezeit %d ausserhalb (rand&0xf)+1 (@0x80103858-74)", name, ki, v.frei - v.weck);
        if (re2) CHECK(v.auf_clip == 8 || v.auf_clip == 9, "C %s RE2: Aufsteh-Clip %d statt 8/9 (@0x80103838-80)", name, v.auf_clip);
        else     CHECK(v.auf_clip == 0x12 || v.auf_clip == 0x13, "C %s RE1.5: Aufsteh-Clip %d statt 0x12/0x13 (+0x94 @0x80100c7c)", name, v.auf_clip);
        CHECK(v.clip_len > 10 && v.clip_max >= v.clip_len - 2,
              "C %s %s: Aufsteh-Clip nicht ausgespielt (max Bild %d von %d)", name, ki, v.clip_max, v.clip_len);
        CHECK(v.steht >= 0 && v.steht <= 17 + v.clip_len + 4,
              "C %s %s: steht erst in Bild %d (Grenze %d)", name, ki, v.steht, 17 + v.clip_len + 4);
        if (angriff_pflicht & (re2 ? 2 : 1))           /* Bit 0 = RE1.5-KI, Bit 1 = RE2-KI */
            CHECK(v.angriff >= 0, "C %s %s: kein Angriff binnen 900 Bildern", name, ki);
    }
}

/* ---- D: Grenze 0xFA0 ------------------------------------------------------------------------------------------- */
/* +0x1d0 ist SquareRoot0 (BIOS-Tabellenwurzel, re15_enemy_player_dist / dmg_isqrt) aus den 16-Bit-Lagen — sie trifft
 * nicht jeden Wert (gemessen: dx 4001..4008 -> 3998, dx 4009 -> 4007). Der Riegel prueft deshalb den GEMESSENEN Wert:
 * der kleinste x-Versatz mit +0x1d0 >= 4000 weckt nie, der groesste mit +0x1d0 < 4000 weckt (`sltiu 0xfa0` @0x8010382c). */
static void teil_d(void)
{
    if (!raum_laden("STAGE3/ROOM3000.RDT", 0x3000)) return;
    re15_actor_t a0, a1; memset(&a0, 0, sizeof a0); memset(&a1, 0, sizeof a1);
    int32_t off_hi = -1, off_lo = -1; unsigned d_hi = 0, d_lo = 0;
    for (int32_t off = 3900; off <= 4200; off++) {
        a1.x = off; unsigned d = (unsigned)re15_enemy_player_dist(&a0, &a1);
        if (d < 4000u) { off_lo = off; d_lo = d; }
        else if (off_hi < 0) { off_hi = off; d_hi = d; }
    }
    printf("  [D] SquareRoot0: groesster Versatz mit d<4000: %d (d=%u), kleinster mit d>=4000: %d (d=%u)\n",
           (int)off_lo, d_lo, (int)off_hi, d_hi);
    CHECK(off_lo > 0 && off_hi > 0 && off_hi > off_lo, "D: SquareRoot0-Grenze nicht gefunden");
    for (int k = 0; k < 2; k++) {
        const int re2 = (k == 1);
        const char *ki = re2 ? "RE2" : "RE1.5";
        for (int dd = 0; dd < 2; dd++) {
            const int32_t off = dd ? off_lo : off_hi;
            const unsigned soll = dd ? d_lo : d_hi;
            raum_start(&R3000, re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15, 30000, 30000, 0, 0x7777u);
            for (int f = 0; f < 3; f++) { g_actors[0].x = 30000; g_actors[0].z = 30000; bild(); }
            int s = slot_bei(-16300, -16700);
            CHECK(s > 0, "D %s: Zombie (-16300,-16700) fehlt", ki);
            if (s <= 0) continue;
            for (int o = 1; o < RE15_ACTOR_MAX; o++)
                if (o != s && g_actors[o].active && g_actors[o].type >= 0x10 && g_actors[o].type <= 0x1f) {
                    g_actors[o].x = -30000; g_actors[o].z = -30000;
                }
            re15_actor_t *e = &g_actors[s];
            int geweckt = -1, abweich = 0;
            for (int f = 0; f < 300; f++) {
                g_actors[0].x = e->x + off; g_actors[0].z = e->z;
                bild();
                if (e->ai_dist != soll && geweckt < 0) abweich++;
                if (geweckt < 0 && (!liegt(e, re2) ||
                                    (re2 && e->ai_timer != (int16_t)RE15_R41_UNSCHARF))) geweckt = f;
            }
            printf("  [D %s] Versatz %d (+0x1d0 = %u): geweckt F%d, Abweichungen %d\n", ki, (int)off, soll, geweckt,
                   abweich);
            CHECK(abweich == 0, "D %s: +0x1d0 wich %d-mal von %u ab", ki, abweich, soll);
            if (dd) CHECK(geweckt >= 0 && geweckt <= 17, "D %s: +0x1d0 = %u < 4000 weckt nicht (F%d)", ki, soll, geweckt);
            else    CHECK(geweckt < 0, "D %s: +0x1d0 = %u >= 4000 weckt (F%d) — `sltiu 0xfa0` @0x8010382c", ki, soll, geweckt);
        }
    }
}

/* ---- E: Wartezeit ---------------------------------------------------------------------------------------------- */
static void teil_e(void)
{
    if (!raum_laden("STAGE3/ROOM3000.RDT", 0x3000)) return;
    int gesehen[17] = { 0 }; int n_werte = 0, raus = 0, n = 0;
    for (int seed = 0; seed < 48; seed++) {
        raum_start(&R3000, RE15_AI_FLAVOR_RE2, 30000, 30000, 0, 0x1000u + (uint32_t)seed * 7919u);
        for (int f = 0; f < 3; f++) { g_actors[0].x = 30000; g_actors[0].z = 30000; bild(); }
        int s = slot_bei(-9900, -14800);
        if (s <= 0) { raus++; continue; }
        int weck = -1, frei = -1;
        for (int f = 0; f < 40 && frei < 0; f++) {
            int16_t vor = g_actors[s].ai_timer;
            g_actors[0].x = -8700; g_actors[0].z = -14800;
            bild();
            if (weck < 0 && vor == (int16_t)RE15_R41_UNSCHARF &&
                (g_actors[s].ai_timer != (int16_t)RE15_R41_UNSCHARF || !(g_actors[s].re2z_f10e & 0x4000u))) weck = f;
            if (weck >= 0 && !(g_actors[s].re2z_f10e & 0x4000u)) frei = f;
        }
        int w = (weck >= 0 && frei >= 0) ? frei - weck + 1 : -1;   /* Takte inkl. Wecktakt */
        if (w < 1 || w > 16) { raus++; continue; }
        if (!gesehen[w]++) n_werte++;
        n++;
    }
    printf("  [E] 48 Seeds: %d gemessen, %d verschiedene Wartezeiten (1..16), %d ausserhalb\n", n, n_werte, raus);
    CHECK(raus == 0, "E: %d Seeds ausserhalb der Wartezeit 1..16 (rand&0xf @0x80103858-60, Post-Dekrement @0x80103974)", raus);
    CHECK(n_werte >= 10, "E: nur %d verschiedene Wartezeiten — kein Zufallswurf?", n_werte);
}

/* ---- F: Gegenprobe Fresser 0x86 (ROOM1140) -------------------------------------------------------------------- */
static void teil_f(void)
{
    if (!raum_laden("STAGE1/ROOM1140.RDT", 0x1140)) return;
    for (int k = 0; k < 2; k++) {
        const int re2 = (k == 1);
        raum_t r1140 = { "STAGE1/ROOM1140.RDT", 0x1140, 0, 0, {{0,0}}, 0 };
        raum_start(&r1140, re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15, 30000, 30000, 0, 0x2545f491u);
        for (int f = 0; f < 6; f++) { g_actors[0].x = 30000; g_actors[0].z = 30000; bild(); }
        int fress = 0, falsch = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            const re15_actor_t *e = &g_actors[s];
            if (!e->active || e->type < 0x10 || e->type > 0x1f) continue;
            /* die ROOM1140-Fresser sind 0x86; nach dem INIT steht im RE1.5-Flavor grid 0 / Sub 0xc */
            if (re2) { if (e->state == 1 && e->sub_state_1 == 8) fress++; }
            else     { if (e->state == 1 && e->sub_state_1 == 0x0c && e->grid_id == 0) fress++; }
            if (re15_r41_liegewecker_liegt(e)) falsch++;
        }
        printf("  [F 1140 %s] Fresser %d, faelschlich Liegewecker %d\n", re2 ? "RE2" : "RE1.5", fress, falsch);
        CHECK(fress == 4, "F 1140 %s: %d Fresser statt 4 (0x86 x4, Zensus)", re2 ? "RE2" : "RE1.5", fress);
        CHECK(falsch == 0, "F 1140 %s: %d Fresser als Liegewecker erkannt", re2 ? "RE2" : "RE1.5", falsch);
    }
}

int main(void)
{
    re15_r41_liegewecker_enable(1);
    if (raum_laden(R2000.rdt, R2000.room)) {
        teil_a(&R2000, "2000", 2);
        teil_b(&R2000, "2000", 4350, -20500);
        /* Kanalraum: der Zombie liegt unten (y 0), der Spieler steht auf dem Oststeg (Band 1). Angriff heisst hier:
         * an der Kante hochziehen (Runde 38 U) und greifen. Direkt UEBER ihm (-21300,-10100) schnappt die RE2-KI
         * (EXEC[14] @0x80104D74) nach oben statt zur Kante zu laufen, vom Oststeg (-20300,-11600) waehlt der RE1.5-
         * Wegfinder den Weststeg — beides Verhalten NACH dem Aufstehen (Dossier OFFEN); je KI gilt die Angriffspflicht
         * fuer die Aufstellung, in der sie greift (Bit 0 RE1.5, Bit 1 RE2). */
        teil_c(&R2000, "2000", -22550, -10100, -21300, -10100, 0x800, 500u, 1500u, 1);
        teil_c(&R2000, "2000-Steg", -22550, -10100, -20300, -11600, 0x800, 500u, 3999u, 2);
    }
    if (raum_laden(R2001.rdt, R2001.room)) {
        teil_a(&R2001, "2001", 2);
        teil_c(&R2001, "2001", -22550, -10100, -21300, -10100, 0x800, 500u, 1500u, 1);
        teil_c(&R2001, "2001-Steg", -22550, -10100, -20300, -11600, 0x800, 500u, 3999u, 2);
    }
    if (raum_laden(R3000.rdt, R3000.room)) {
        teil_a(&R3000, "3000", 7);
        teil_b(&R3000, "3000", -30000, 9000);
        teil_c(&R3000, "3000", -9900, -14800, -8700, -14800, 0x800, 500u, 1500u, 3);
    }
    if (raum_laden(R3010.rdt, R3010.room)) {
        teil_a(&R3010, "3010", 6);
        teil_c(&R3010, "3010", -9100, -19800, -9100, -18700, 0x400, 500u, 1500u, 3);
    }
    if (raum_laden(R3001.rdt, R3001.room)) {
        teil_a(&R3001, "3001", 7);
        teil_c(&R3001, "3001", -9900, -14800, -8700, -14800, 0x800, 500u, 1500u, 3);
    }
    if (raum_laden(R3011.rdt, R3011.room)) {
        teil_a(&R3011, "3011", 6);
        teil_c(&R3011, "3011", -9100, -19800, -9100, -18700, 0x400, 500u, 1500u, 3);
    }
    teil_d();
    teil_e();
    teil_f();
    re15_wasserzombie_bank_vergessen();
    if (s_fails) { printf("== %d FEHLER ==\n", s_fails); return 1; }
    printf("== OK ==\n");
    return 0;
}
