/*
 * Runde 38 Spur U "wasserzombie" — die Zombies im Wasser ziehen sich am Rand hoch (RE1.5), beide KI-Varianten.
 * Dossier analysis/befunde_runde38/U_wasserzombie.md, Code engine/src/wasserzombie_r38.c, include/re15_wasserzombie_r38.h.
 *
 *   A  CLIP-BELEG: RE1.5-Aktionsbank Typ 0x16 (CDEMD0.EMS): Clip 0x16 = 159 Bilder, Wurzel f0 (0,-1817,0) ->
 *      f150 (1444,-3278,111) -> f158 (0,-1698,0); Bild 130 traegt das SE-Bit 23 (SE 1); 0x10/0x11/0x12/0x18 gleich.
 *   B  AUSLOESER @0x80101f00-44: u1&3, Kegel [Kante-0x1ff .. Kante+0x200], u1 Bit 0.
 *   C  RE1.5-KI, echter Raum ROOM2020 (echte RDT/SCA/Spawns, game_step): Wasserzombie (-11300,0,300) laeuft an die
 *      Suedkante, Clipfolge 1 -> 0x16, bei Clip-Bild 0x46 +0x1ba = -1800 (y noch 0, Schatten auf +0x1ba), danach
 *      y = -1800 / Band 1 / +0x1ba = -1800, Vorschub 1140 entlang 0xC00 (z + 1140, x gleich), danach 120 Bilder
 *      kein Sturz zurueck und kein Band 2.
 *   D  RE2-KI (Standard), derselbe Raum: dieselbe Mechanik ueber re15_wasserzombie_re2_dispatch (RE1.5-Bank je Typ
 *      gemeldet), am Ende RE2-Zustand 1/0 mit RE2-Clip; Gegenprobe ohne gemeldete Bank: kein Hochziehen.
 *   E  ROOM2000 (Deskriptor 0x84, Modus 4 -> Engage) beide KI: beide Wasserzombies stehen am Ende auf einem Steg.
 *   F  RE2-Abbruch nach Bild 0x46 durch einen fremden Schreiber NACH dem eigenen Tick (Partner-Domino 0x901):
 *      Zwischenbild = Stand des eigenen Ticks (RE1.5-Bank, Clip 0x16, Uhr 159, Schatten +0x1ba, Advancer Bild + 1);
 *      erst der eigene Tick bricht ab (+0x1ba zurueck, RE2-Bank, EXEC[9]-Clip mit RE2-Laenge). Nachbesserung 1.
 *      F-Neuspawn: Zustand 0 im Slot -> keine RE1.5-Bank trotz stehender Marke.
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
#include "re2_ems.h"
#include "re15_wasserzombie_r38.h"

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

/* ---- RE1.5-EMD je Typ (eigener Puffer: keyframe_data zeigt hinein) -------------------------------------------- */
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
static void root_at(const emd15_t *m, int clip, int f, int16_t *x, int16_t *y, int16_t *z)
{
    uint32_t fe = re15_emd_get_frame_entry(&m->an, clip, f);
    re15_emd_get_keyframe_position(&m->sk, (int)(fe & 0xfffu), x, y, z);
}

/* ---- Gegner-Baenke wie probe_1010_kriecher (RE1.5 bzw. RE2 aus shared_assets/RE2) ----------------------------- */
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

/* ---- Raumlauf (game_step-Reihenfolge wie probe_1010_kriecher) -------------------------------------------------- */
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

/* Spieler auf einen Steg (Band 1), Raum aufbauen, Baenke laden. bank_melden: RE1.5-Bank an das Modul melden. */
static void raum_start(re15_ai_flavor_t fl, int32_t px, int32_t pz, int16_t prot, int bank_melden)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(fl);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_esp_fx_reset();
    re15_damage_seed_rng(0x2545f491u);
    re15_wasserzombie_reset();
    re15_wasserzombie_bank_vergessen();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 1; pl->y = -1800;
    pl->x = px; pl->z = pz; pl->rot_y = prot;
    re15_collision_set_band(1);
    re15_inv_load_briefing();
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    pl->x = px; pl->z = pz; pl->y = -1800; pl->floor = 1; pl->rot_y = prot;
    if (fl == RE15_AI_FLAVOR_RE2) (void)load_bank_re2(0x16);
    else                          (void)load_bank_re15(0x16);
    if (bank_melden) {
        emd15_t *m = emd15(0x16);
        if (m) re15_wasserzombie_bank_merken(0x16, &m->sk, &m->an);
    }
}

static int slot_bei(int32_t x, int32_t y, int32_t z)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *e = &g_actors[s];
        if (e->active && e->type == 0x16 && e->x == x && e->y == y && e->z == z) return s;
    }
    return -1;
}

/* Verlauf eines Wasserzombies: Eintritt in 0x0A, Clipfolge, Bild-0x46-Boden, Ende. */
typedef struct {
    int eintritt, klettern, boden46, oben;          /* Bildnummern, -1 = nie */
    int clip_vor_kletter;                           /* motion im Drehen (1) */
    int32_t boden46_y, boden46_f1ba, boden46_schatten;
    int32_t vor_x, vor_z;                           /* Lage im Bild vor "oben" */
    int32_t oben_x, oben_y, oben_z, oben_f1ba; int oben_band;   /* Lage nach dem ganzen Bild */
    int32_t land_x, land_y, land_z; int land_slot;            /* Lage im Hochzieh-Ende (vor Koerperstoss) */
    int16_t oben_rot;
    int oben_state, oben_sub; int oben_motion;
    int bank_re15_im_klettern;                      /* re15_wasserzombie_bank aktiv (RE2) */
    int clip_len_klettern;
    int max_y_nachher, max_band_nachher, min_band_nachher;   /* 120 Bilder nach dem Ende */
    int erste30_ok;                                          /* 30 Bilder danach: y = -1800, Band 1 */
} verlauf_t;

static void verlauf_init(verlauf_t *v)
{
    memset(v, 0, sizeof *v);
    v->eintritt = v->klettern = v->boden46 = v->oben = -1;
    v->max_y_nachher = -0x7fffffff; v->max_band_nachher = -1; v->min_band_nachher = 99;
    v->erste30_ok = 1;
}

static void verfolgen(int slot, int bilder, int bilder_nachher, verlauf_t *v)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e = &g_actors[slot];
    int32_t px = pl->x, pz = pl->z; int16_t prot = pl->rot_y;
    int nach = -1;
    for (int f = 0; f < bilder; f++) {
        /* Spieler steht still auf dem Steg und lebt (er soll nur den Zombie herlocken) */
        pl->hp = 100; pl->x = px; pl->z = pz; pl->y = -1800; pl->floor = 1; pl->rot_y = prot;
        int32_t vx = e->x, vz = e->z;
        int war_sub = (e->state == 1) ? e->sub_state_1 : -1;
        int war_phase = e->sub_state_2;
        bild();
        if (v->oben >= 0) {
            if (nach < 0) nach = f;
            if (f - nach <= bilder_nachher) {
                if (e->y > v->max_y_nachher) v->max_y_nachher = e->y;
                if ((int)e->floor > v->max_band_nachher) v->max_band_nachher = e->floor;
                if ((int)e->floor < v->min_band_nachher) v->min_band_nachher = e->floor;
            }
            if (f - nach < 30 && !(e->y == -RE15_WZ_BAND && e->floor == 1)) v->erste30_ok = 0;
            continue;
        }
        if (v->eintritt < 0 && e->state == 1 && e->sub_state_1 == RE15_WZ_SUB) {
            v->eintritt = f; v->clip_vor_kletter = e->motion;
        }
        if (v->eintritt >= 0 && v->klettern < 0 && e->state == 1 && e->sub_state_1 == RE15_WZ_SUB
            && e->motion == (int16_t)RE15_WZ_CLIP_KLETTER) {
            v->klettern = f;
            const re15_emd_skeleton_t *sk = NULL; const re15_emd_animation_t *an = NULL;
            v->bank_re15_im_klettern = re15_wasserzombie_bank(e, &sk, &an);
            v->clip_len_klettern = re15_actor_clip_len(e);
        }
        if (v->klettern >= 0 && v->boden46 < 0 && e->state == 1 && e->sub_state_1 == RE15_WZ_SUB
            && e->anim_frame == RE15_WZ_BODENBILD) {
            v->boden46 = f; v->boden46_y = e->y; v->boden46_f1ba = e->dog_floor_y;
            int32_t sy = e->y; v->boden46_schatten = re15_wasserzombie_schatten_y(e, &sy) ? sy : 0x7fffffff;
        }
        if (v->klettern >= 0 && war_sub == RE15_WZ_SUB && war_phase == 3
            && !(e->state == 1 && e->sub_state_1 == RE15_WZ_SUB)) {
            v->oben = f; v->vor_x = vx; v->vor_z = vz;
            v->land_slot = re15_wasserzombie_letzte_landung(&v->land_x, &v->land_y, &v->land_z);
            v->oben_x = e->x; v->oben_y = e->y; v->oben_z = e->z; v->oben_f1ba = e->dog_floor_y;
            v->oben_band = e->floor; v->oben_rot = e->rot_y;
            v->oben_state = e->state; v->oben_sub = e->sub_state_1; v->oben_motion = e->motion;
        }
    }
}

static void verlauf_drucken(const char *tag, const verlauf_t *v)
{
    printf("  [%s] eintritt=F%d (clip %d) klettern=F%d (re15bank=%d len=%d) bild46=F%d (y=%d f1ba=%d schatten=%d) "
           "oben=F%d landung(slot %d)=(%d,%d,%d) bildende @(%d,%d,%d) b%d f1ba=%d rot=%d st=%d/%d mo=%d | vorher (%d,%d) "
           "| danach max_y=%d band %d..%d\n",
           tag, v->eintritt, v->clip_vor_kletter, v->klettern, v->bank_re15_im_klettern, v->clip_len_klettern,
           v->boden46, (int)v->boden46_y, (int)v->boden46_f1ba, (int)v->boden46_schatten,
           v->oben, v->land_slot, (int)v->land_x, (int)v->land_y, (int)v->land_z,
           (int)v->oben_x, (int)v->oben_y, (int)v->oben_z, v->oben_band, (int)v->oben_f1ba, (int)v->oben_rot,
           v->oben_state, v->oben_sub, v->oben_motion, (int)v->vor_x, (int)v->vor_z,
           (int)v->max_y_nachher, v->min_band_nachher, v->max_band_nachher);
}

/* gemeinsame Mechanik-Pruefung eines vollstaendigen Hochziehens an der Kante mit Richtung `kante` */
static void pruefe_oben(const char *tag, const verlauf_t *v, int kante, int re2)
{
    CHECK(v->eintritt >= 0, "%s: Wasserzombie tritt nie in 0x0A (Hochziehen) ein", tag);
    CHECK(v->clip_vor_kletter == (int)RE15_WZ_CLIP_DREH, "%s: Drehphase Clip %d statt 1 (@0x801032c0)", tag, v->clip_vor_kletter);
    CHECK(v->klettern >= 0, "%s: Clip 0x16 nie gestartet", tag);
    CHECK(v->clip_len_klettern == 159, "%s: Clip-Uhr %d statt 159 Bilder (RE1.5-Aktionsbank Clip 0x16)", tag, v->clip_len_klettern);
    if (re2) CHECK(v->bank_re15_im_klettern == 1, "%s: RE2-KI posiert nicht die RE1.5-Bank im Hochziehen", tag);
    else     CHECK(v->bank_re15_im_klettern == 0, "%s: RE1.5-KI darf die Import-Bank nicht melden", tag);
    CHECK(v->boden46 >= 0, "%s: Bild 0x46 nie erreicht", tag);
    CHECK(v->boden46_y == 0 && v->boden46_f1ba == -RE15_WZ_BAND,
          "%s: Bild 0x46: y=%d f1ba=%d (erwartet 0 / -1800 @0x801033f8)", tag, (int)v->boden46_y, (int)v->boden46_f1ba);
    CHECK(v->boden46_schatten == -RE15_WZ_BAND, "%s: Schatten bei Bild 0x46 = %d (erwartet +0x1ba -1800 @0x801004fc)",
          tag, (int)v->boden46_schatten);
    CHECK(v->oben >= 0, "%s: Hochziehen nie beendet", tag);
    CHECK(v->oben_y == -RE15_WZ_BAND && v->oben_band == 1 && v->oben_f1ba == -RE15_WZ_BAND,
          "%s: oben y=%d band=%d f1ba=%d (erwartet -1800/1/-1800 @0x80103454/@0x8010343c)", tag,
          (int)v->oben_y, v->oben_band, (int)v->oben_f1ba);
    CHECK((int)v->oben_rot == kante, "%s: Blick %d statt Kante %d (@0x801033bc)", tag, (int)v->oben_rot, kante);
    /* Vorschub FUN_800245d8(0) mit +0x8c = 0x474 entlang der Kante (@0x80103474) — gemessen an der Lage im
     * Hochzieh-Ende (der Koerperstoss gegen den Spieler laeuft danach im selben Bild) */
    CHECK(v->land_y == -RE15_WZ_BAND, "%s: Landung y=%d", tag, (int)v->land_y);
    {
        int32_t dx = v->land_x - v->vor_x, dz = v->land_z - v->vor_z;
        int32_t ex = 0, ez = 0;
        if (kante == 0x000) ex = RE15_WZ_TEMPO;
        if (kante == 0x800) ex = -RE15_WZ_TEMPO;
        if (kante == 0xC00) ez = RE15_WZ_TEMPO;
        if (kante == 0x400) ez = -RE15_WZ_TEMPO;
        CHECK(dx == ex && dz == ez, "%s: Vorschub (%d,%d) statt (%d,%d)", tag, (int)dx, (int)dz, (int)ex, (int)ez);
    }
    CHECK(v->oben_state == 1 && v->oben_sub == 0, "%s: Ende-Wort %d/%d statt 1/0 (@0x80103464)", tag, v->oben_state, v->oben_sub);
    if (re2) CHECK(v->oben_motion != (int)RE15_WZ_CLIP_KLETTER,
                   "%s: RE2-Uebergabe: Clip 0x16 bleibt auf der RE2-Bank stehen (re15_re2z_exec_only fehlt)", tag);
    /* der Vorbefund: im Bild danach zurueck ins Wasser, bis y = +1800 unter den Wasserboden, dann Band 2 */
    CHECK(v->erste30_ok, "%s: in den ersten 30 Bildern nach dem Hochziehen nicht stabil auf dem Steg (y=-1800/Band 1)", tag);
    CHECK(v->max_y_nachher <= 0 && v->max_band_nachher <= 1,
          "%s: nach dem Hochziehen unter den Wasserboden/ueber Band 1: max_y=%d Band %d..%d", tag,
          (int)v->max_y_nachher, v->min_band_nachher, v->max_band_nachher);
}

int main(void)
{
    printf("== Runde 38 Spur U: Wasserzombies ziehen sich am Rand hoch ==\n");

    /* ---------------- A: Clip-Beleg ---------------- */
    {
        emd15_t *m = emd15(0x16);
        CHECK(m != NULL, "A: RE1.5-EMD Typ 0x16 nicht ladbar");
        if (m) {
            CHECK(m->an.clip_count == 43, "A: Aktionsbank %d Clips statt 43", m->an.clip_count);
            CHECK(m->an.clips[0x16].frame_count == 159, "A: Clip 0x16 %d Bilder statt 159", m->an.clips[0x16].frame_count);
            int16_t x0, y0, z0, x1, y1, z1, x2, y2, z2;
            root_at(m, 0x16, 0, &x0, &y0, &z0);
            root_at(m, 0x16, 150, &x1, &y1, &z1);
            root_at(m, 0x16, 158, &x2, &y2, &z2);
            printf("  [A] Clip 0x16 Wurzel f0 (%d,%d,%d) f150 (%d,%d,%d) f158 (%d,%d,%d)\n", x0, y0, z0, x1, y1, z1, x2, y2, z2);
            CHECK(x0 == 0 && y0 == -1817 && z0 == 0, "A: f0 Wurzel (%d,%d,%d)", x0, y0, z0);
            CHECK(x1 == 1444 && y1 == -3278 && z1 == 111, "A: f150 Wurzel (%d,%d,%d) — Huefte nicht ueber der Kante", x1, y1, z1);
            CHECK(x2 == 0 && y2 == -1698 && z2 == 0, "A: f158 Wurzel (%d,%d,%d)", x2, y2, z2);
            uint32_t fe130 = re15_emd_get_frame_entry(&m->an, 0x16, 130);
            CHECK(((fe130 >> 22) & 2u) != 0, "A: Bild 130 ohne SE-Bit (Frame-Wort 0x%08x)", (unsigned)fe130);
            static const uint8_t andere[] = { 0x10, 0x11, 0x12, 0x18 };
            for (unsigned k = 0; k < sizeof andere; k++) {
                emd15_t *o = emd15(andere[k]);
                CHECK(o != NULL, "A: RE1.5-EMD Typ 0x%02x nicht ladbar", andere[k]);
                if (!o) continue;
                int16_t ox, oy, oz;
                root_at(o, 0x16, 150, &ox, &oy, &oz);
                CHECK(o->an.clips[0x16].frame_count == 159 && ox == x1 && oy == y1 && oz == z1,
                      "A: Typ 0x%02x Clip 0x16 weicht ab (%d Bilder, f150 (%d,%d,%d))", andere[k],
                      o->an.clips[0x16].frame_count, ox, oy, oz);
            }
        }
    }

    /* ---------------- B: Ausloeser ---------------- */
    {
        re15_actor_t t; memset(&t, 0, sizeof t);
        t.ai_contact = 0xC9; t.rot_y = 0xC00;              /* Richtung 0xC, +8, u1 = 1 */
        CHECK(re15_wasserzombie_kante_vorn(&t) == 1, "B: u1=1 frontal nicht erkannt");
        t.ai_contact = 0xCA; CHECK(re15_wasserzombie_kante_vorn(&t) == 0, "B: u1=2 (Absturzkante) loest aus");
        t.ai_contact = 0xC8; CHECK(re15_wasserzombie_kante_vorn(&t) == 0, "B: u1=0 loest aus");
        t.ai_contact = 0xC9;
        t.rot_y = 0xC00 + 0x200; CHECK(re15_wasserzombie_kante_vorn(&t) == 1, "B: Kegelrand +0x200");
        t.rot_y = 0xC00 + 0x201; CHECK(re15_wasserzombie_kante_vorn(&t) == 0, "B: ausserhalb +0x201");
        t.rot_y = 0xC00 - 0x1ff; CHECK(re15_wasserzombie_kante_vorn(&t) == 1, "B: Kegelrand -0x1ff");
        t.rot_y = 0xC00 - 0x200; CHECK(re15_wasserzombie_kante_vorn(&t) == 0, "B: ausserhalb -0x200");
        t.rot_y = 0; CHECK(re15_wasserzombie_kante_vorn(&t) == 0, "B: Kante seitlich/hinten loest aus");
    }

    /* ---------------- C/D: ROOM2020, beide KI ---------------- */
    if (raum_laden("STAGE2/ROOM2020.RDT", 0x2020)) {
        for (int k = 0; k < 2; k++) {
            const int re2 = (k == 1);
            const char *tag = re2 ? "D RE2 2020" : "C RE1.5 2020";
            /* Leon steht schraeg vor dem Zombie auf dem Suedsteg, NICHT auf dessen Landeplatz (sonst schiebt der
             * Koerperstoss FUN_8002aec4 den Gelandeten zurueck ueber die Absturzkante) */
            raum_start(re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15, -9500, 2600, 0x400, 1);
            int s = slot_bei(-11300, 0, 300);
            CHECK(s > 0, "%s: Wasserzombie (-11300,0,300) nicht gespawnt", tag);
            if (s <= 0) continue;
            verlauf_t v; verlauf_init(&v);
            verfolgen(s, 520, 120, &v);
            verlauf_drucken(tag, &v);
            pruefe_oben(tag, &v, 0xC00, re2);
            CHECK(v.land_z >= 1500 + 400 && v.land_z <= 3300 - 400,
                  "%s: Landung z=%d nicht auf dem Suedsteg (Band-1-Freiflaeche z 1500..3300)", tag, (int)v.land_z);
        }
        /* D-Gegenprobe: RE2-KI ohne gemeldete RE1.5-Bank -> kein Hochziehen (RE2-Gang bleibt an der Wand) */
        {
            raum_start(RE15_AI_FLAVOR_RE2, -9500, 2600, 0x400, 0);
            int s = slot_bei(-11300, 0, 300);
            if (s > 0) {
                verlauf_t v; verlauf_init(&v);
                verfolgen(s, 300, 0, &v);
                printf("  [D-Gegenprobe ohne Bank] eintritt=F%d y=%d\n", v.eintritt, (int)g_actors[s].y);
                CHECK(v.eintritt < 0 && g_actors[s].y == 0, "D: ohne RE1.5-Bank darf der RE2-Zombie nicht hochziehen");
            }
        }
        /* F: RE2-Abbruch durch einen FREMDEN Schreiber NACH dem eigenen Tick (gemessener Fall, Abnahme 0 M5: ROOM2000
         *    F400 schreibt der greifende Slot 4 im Griff-Ende P8 das Wort 0x901 in den kletternden Slot 3, dessen Tick in
         *    diesem Bild schon gelaufen ist — RE2-Partner-Domino `addiu v0,zero,2305` / `sw v0,4(s0)` @0x80102E90/98).
         *    Zwischenbild (Zeichnen, Schuss-Anker, Advancer des Folgebilds): im Original zeigt der Aktor die Pose seines
         *    letzten eigenen anim_set (Teileblock +0x188 @0x8001f40c, RotMatrix @0x8001f574/@0x8001f5f0) -> RE1.5-Bank,
         *    Clip 0x16 gueltig, Laenge 159, Schatten +0x1ba; der Advancer des Folgebilds taktet 0x16 auf 159 (Bild + 1).
         *    Erst der eigene Tick bricht ab: +0x1ba zurueck, RE2-Bank, RE2-Clip aus EXEC[9] P0 (Tabelle {3,3,4,0x0D}
         *    @0x801000D8, `sw v0,332(s1)` @0x80103F30) mit RE2-Laenge. Vorher (a4809c35) hing die Bank am Zustandswort:
         *    im Zwischenbild Clip 22 auf der RE2-Bank (10 Bilder) -> Advancer pinnte af 49 -> 9, Bild F400 leer. */
        {
            raum_start(RE15_AI_FLAVOR_RE2, -9500, 2600, 0x400, 1);
            int s = slot_bei(-11300, 0, 300);
            emd15_t *m15 = emd15(0x16);
            if (s > 0 && m15) {
                re15_actor_t *e = &g_actors[s];
                int abgebrochen = 0;
                for (int f = 0; f < 400 && !abgebrochen; f++) {
                    g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
                    bild();
                    if (e->state == 1 && e->sub_state_1 == RE15_WZ_SUB && e->anim_frame > RE15_WZ_BODENBILD + 5) {
                        CHECK(e->dog_floor_y == -RE15_WZ_BAND, "F: vor dem Abbruch f1ba=%d", (int)e->dog_floor_y);
                        const int af0 = (int)e->anim_frame;
                        re15_ai_set_state_word(e, 0x0901u);       /* fremder Schreiber NACH dem eigenen Tick dieses Bilds */
                        /* --- Zwischenbild: Stand des letzten eigenen Ticks --- */
                        const re15_emd_skeleton_t *zsk = NULL; const re15_emd_animation_t *zan = NULL;
                        const int zbank = re15_wasserzombie_bank(e, &zsk, &zan);
                        const int zlen = re15_actor_clip_len(e);
                        int32_t zsy = 12345; const int zsch = re15_wasserzombie_schatten_y(e, &zsy);
                        re15_actor_t snap[RE15_ACTOR_MAX];
                        memcpy(snap, g_actors, sizeof snap);
                        re15_actors_anim_advance();                /* der Advancer des Folgebilds (game_step, vor der KI) */
                        const int zaf = (int)g_actors[s].anim_frame;
                        memcpy(g_actors, snap, sizeof snap);
                        printf("  [F] Zwischenbild: st=%d/%d mo=%d af=%d bank_re15=%d len=%d schatten=%d(%d) advancer af=%d\n",
                               e->state, e->sub_state_1, (int)e->motion, af0, zbank, zlen, zsch, (int)zsy, zaf);
                        CHECK(zbank == 1 && zsk != NULL && zan != NULL && zan->clips == m15->an.clips,
                              "F: Zwischenbild posiert nicht die RE1.5-Aktionsbank (bank=%d)", zbank);
                        CHECK(e->motion == (int16_t)RE15_WZ_CLIP_KLETTER && zan && (int)e->motion < zan->clip_count,
                              "F: Zwischenbild Clip %d (erwartet 0x16 auf der RE1.5-Bank)", (int)e->motion);
                        CHECK(zlen == 159, "F: Zwischenbild-Uhr %d statt 159 (RE1.5-Clip 0x16)", zlen);
                        CHECK(zsch == 1 && zsy == -RE15_WZ_BAND, "F: Zwischenbild-Schatten %d/%d statt +0x1ba -1800",
                              zsch, (int)zsy);
                        CHECK(af0 + 1 < 159 && zaf == af0 + 1, "F: Advancer im Folgebild af %d -> %d (erwartet %d)",
                              af0, zaf, af0 + 1);
                        /* --- Folgebild: der eigene Tick bricht ab --- */
                        bild();
                        const int nbank = re15_wasserzombie_bank(e, NULL, NULL);
                        const int nlen = re15_actor_clip_len(e), nlen2 = re15_actor_clip_len_legacy(e);
                        printf("  [F] Abbruch: st=%d/%d mo=%d af=%d y=%d f1ba=%d band=%d bank_re15=%d len=%d (RE2 %d)\n",
                               e->state, e->sub_state_1, (int)e->motion, (int)e->anim_frame, (int)e->y,
                               (int)e->dog_floor_y, (int)e->floor, nbank, nlen, nlen2);
                        CHECK(e->y == 0 && e->dog_floor_y == 0 && e->floor == 0 && e->sub_state_1 == 9,
                              "F: nach Abbruch y=%d f1ba=%d band=%d (erwartet 0/0/0)", (int)e->y, (int)e->dog_floor_y,
                              (int)e->floor);
                        CHECK(nbank == 0, "F: nach dem eigenen Abbruch-Tick noch RE1.5-Bank");
                        CHECK(e->motion == 3 || e->motion == 4 || e->motion == 0x0D,
                              "F: nach Abbruch Clip %d (erwartet EXEC[9] P0 {3,3,4,0x0D} @0x801000D8)", (int)e->motion);
                        CHECK(nlen == nlen2 && nlen > 0, "F: nach Abbruch Uhr %d statt RE2-Laenge %d", nlen, nlen2);
                        int32_t sy = 12345;
                        CHECK(re15_wasserzombie_schatten_y(e, &sy) == 0, "F: Schatten haengt nach Abbruch an +0x1ba");
                        abgebrochen = 1;
                    }
                }
                CHECK(abgebrochen, "F: Hochziehen nicht ueber Bild 0x46 hinaus erreicht");
            }
        }
        /* F-Neuspawn: steht die Marke noch, wenn im Slot neu gespawnt wird (Sce_em_set `sw zero,4(s0)` @0x800421e0 =
         * Zustand 0), gilt die RE1.5-Bank NICHT — der neue Aktor hat keinen eigenen Tick im Hochziehen. */
        {
            raum_start(RE15_AI_FLAVOR_RE2, -9500, 2600, 0x400, 1);
            int s = slot_bei(-11300, 0, 300);
            if (s > 0) {
                re15_actor_t *e = &g_actors[s];
                int geprueft = 0;
                for (int f = 0; f < 400 && !geprueft; f++) {
                    g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
                    bild();
                    if (e->state == 1 && e->sub_state_1 == RE15_WZ_SUB && e->motion == (int16_t)RE15_WZ_CLIP_KLETTER) {
                        CHECK(re15_wasserzombie_bank(e, NULL, NULL) == 1, "F-Neuspawn: Klettern ohne RE1.5-Bank");
                        re15_ai_set_state_word(e, 0x0000u);       /* Neuspawn im Slot */
                        CHECK(re15_wasserzombie_bank(e, NULL, NULL) == 0, "F-Neuspawn: Zustand 0 posiert die RE1.5-Bank");
                        int32_t sy = 12345;
                        CHECK(re15_wasserzombie_schatten_y(e, &sy) == 0, "F-Neuspawn: Zustand 0 nimmt den Kletter-Schatten");
                        geprueft = 1;
                    }
                }
                CHECK(geprueft, "F-Neuspawn: Klettern nie erreicht");
            }
        }
    }

    /* ---------------- E: ROOM2000 (0x84), beide KI ---------------- */
    if (raum_laden("STAGE2/ROOM2000.RDT", 0x2000)) {
        for (int k = 0; k < 2; k++) {
            const int re2 = (k == 1);
            const char *tag = re2 ? "E RE2 2000" : "E RE1.5 2000";
            raum_start(re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15, -20300, -11600, 0x800, 1);
            int s = slot_bei(-22550, 0, -10100);
            CHECK(s > 0, "%s: Wasserzombie (-22550,0,-10100) nicht gespawnt", tag);
            if (s <= 0) continue;
            verlauf_t v; verlauf_init(&v);
            verfolgen(s, 600, 120, &v);
            verlauf_drucken(tag, &v);
            pruefe_oben(tag, &v, 0x000, re2);
            CHECK(v.land_x >= -21650 + 400 && v.land_x <= -19850 - 400,
                  "%s: Landung x=%d nicht auf dem Oststeg (Band-1-Freiflaeche x -21650..-19850)", tag, (int)v.land_x);
        }
    }

    re15_wasserzombie_bank_vergessen();
    if (s_fails) { printf("== %d FEHLER ==\n", s_fails); return 1; }
    printf("== OK ==\n");
    return 0;
}
