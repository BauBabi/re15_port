/* test_r38_affen3.c — RIEGEL Runde 38 Spur S "affen3": die kleinen Affen (Typ 0x29, EM029) in Stage 3 treffen.
 *
 * Nutzer (AUFTRAG.md Runde 38): "In Stage 3 die kleinen Affen koennen einen nicht treffen, es passiert nichts".
 * Dossier analysis/befunde_runde38/S_affen3.md, Code engine/src/affen3_r38.c + include/re15_affen3_r38.h.
 *
 * Teile (Aufruf: test_r38_affen3 <teil>):
 *   typ     Typ 0x29 ist der kleine Affe: der Original-Lader PSX.EXE FUN_80022300 nimmt Sektor/Groesse aus der
 *           Tabelle @0x80072f38 + (Typ-0x10)*8 (@0x80022338-78); Eintrag 0x29 @0x80073000 = Sektor 994 /
 *           0x364d8 Byte = genau der CDEMD0.EMS-Blob, den der Port fuer 0x29 laedt (re15_ems_index_for_type).
 *   biss    Port ab dem ORIGINAL-Zustand F137 der GDB-Spur o4 (ROOM3040, Leon auf der Plattform (-8800,-1800,
 *           -4531), Affe A ruht bei (-8800,-6082) Clip 0x16 Bild 56): Biss-Commit im 2. Tick (Original F139),
 *           Treffer -5 im Fenster {12..15} (Original F150 = Bild 12), Spieler-Reaktion cmd 2 (Flinch), Sperre
 *           0x2d, danach Treffer im Takt wie das Original (57 Bilder). Vorher (alter Port) prueft der Biss den
 *           Mitten-Abstand < 1000 — im Original-Treffermoment ist der 1572 -> nie ein Treffer.
 *   schwer  der schwere Angriff (Sub 6, Clip 0x13): Treffer -10 im Fenster {21,22,23,24,33,34}, Records 6/10 r 0x320,
 *           Niederschlag (aca59 = a780 + 4) statt Flinch.
 *   flavor  "biss" unter RE2-KI (Standard) UND RE1.5-KI: dieselben Treffer (0x29 hat in beiden Flavors das
 *           RE1.5-Gehirn; re15_ai_re2_for_type wird fuer 0x29 nirgends gefragt).
 *   quadrat FUN_8001bff8 als QUADRAT um den gezeichneten Knochenpunkt (@0x8001c080-c0c0): Treffer bei Mitten-
 *           Abstand 1572 (Original F150), kein Treffer, wenn Leon 1100 weiter weg steht.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_damage.h"
#include "re15_fade.h"
#include "re15_skeleton.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_affen.h"
#include "re15_affen3_r38.h"
#include "re15_ai_flavor.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void re15_actor_step_all_walkers(void);
void re15_player_cmd_zero(void);

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (n) *n = (size_t)sz;
    return b;
}

/* ---- Raum-Harness (Muster test_r35_affen.c: Bildfolge wie platform/pc/main.c) ---------------------------- */
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;

static int rdt_laden(uint16_t room)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    return 0;
}

static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_letterbox_tick(re15_game_flag_get(1, 27));
    {
        int cine = re15_cine_active();
        if (cine) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was) g_scd.letterbox_countdown = 15;
        s_cine_was = cine;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
            g_scd.player_mode = 0;
            re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        }
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e)) { }
}

static int room_boot(uint16_t room, int32_t px, int32_t py, int32_t pz, int16_t rot, int band, uint8_t cut, int bilder)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->y = py;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(band);
    s_shown = cut; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    for (int f = 0; f < bilder; f++) frame(0, 0);
    return 0;
}

static re15_actor_t *aktor_vom_typ(uint8_t typ, int nth)
{
    int k = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == typ) { if (k == nth) return &g_actors[s]; k++; }
    return NULL;
}

static double dist2d(int32_t x0, int32_t z0, int32_t x1, int32_t z1)
{
    double dx = (double)x0 - x1, dz = (double)z0 - z1; return sqrt(dx * dx + dz * dz);
}

/* EM029-Bank aus CDEMD0.EMS (wie main.c pc_enemy_load / test_r35_affen.c bank_laden_27). */
static uint8_t s_blob29[400000];
static int bank_laden_29(void)
{
    re15_enemy_bank_t *eb = re15_enemy_find(0x29);
    if (eb && eb->ok) return 1;
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/EMD/CDEMD0.EMS", RE15_ASSET_PSX_DIR);
    uint8_t *ems = slurp(p, &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(0x29);
    size_t off = 0, len = 0;
    int ok = (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= sizeof s_blob29);
    if (ok) { memcpy(s_blob29, ems + off, len); if (!eb) eb = re15_enemy_alloc(0x29); ok = (eb != NULL); }
    if (ok) {
        re15_tim_t tim = (re15_tim_t){0};
        ok = (re15_emd_parse_container(s_blob29, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0);
        if (ok) { eb->ok = 1; eb->buf = NULL;
                  /* Opfer-Bank (dir[5]/[6]) wie main.c pc_enemy_load: Leon im Griff / Todessprung */
                  eb->victim_ok = (re15_emd_parse_victim_bank(s_blob29, len, &eb->skel_victim, &eb->anim_victim) == 0); }
        else eb->type = 0;
    }
    free(ems);
    return ok;
}

/* ================================================================================================== */
static void teil_typ(void)
{
    char p[600]; size_t n = 0;
    uint8_t *exe = slurp(RE15_R38_PSX_EXE, &n);
    PRUEF(exe != NULL && n > 0x63808, "PSX.EXE gelesen (%u B)", (unsigned)n);
    if (!exe) return;
    /* PS-X-EXE: Text ab Datei 0x800, t_addr @Kopf 0x18 */
    uint32_t t_addr = (uint32_t)exe[0x18] | ((uint32_t)exe[0x19] << 8) | ((uint32_t)exe[0x1a] << 16) | ((uint32_t)exe[0x1b] << 24);
    uint32_t a = 0x80072f38u + (0x29u - 0x10u) * 8u;               /* FUN_80022300: (typ-0x10)*8 @0x80022338-44 */
    size_t   o = 0x800u + (a - t_addr);
    uint32_t sektor = (uint32_t)exe[o] | ((uint32_t)exe[o + 1] << 8) | ((uint32_t)exe[o + 2] << 16) | ((uint32_t)exe[o + 3] << 24);
    uint32_t groesse = (uint32_t)exe[o + 4] | ((uint32_t)exe[o + 5] << 8) | ((uint32_t)exe[o + 6] << 16) | ((uint32_t)exe[o + 7] << 24);
    PRUEF(a == 0x80073000u && sektor == 994u && groesse == 0x364d8u,
          "Lader-Tabelle @0x%08x: Sektor %u, Groesse 0x%x (Soll 994 / 0x364d8)", (unsigned)a, (unsigned)sektor, (unsigned)groesse);
    snprintf(p, sizeof p, "%s/EMD/CDEMD0.EMS", RE15_ASSET_PSX_DIR);
    uint8_t *ems = slurp(p, &n);
    size_t off = 0, len = 0;
    int idx = re15_ems_index_for_type(0x29);
    int ok = (ems && idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0);
    PRUEF(ok && off == (size_t)sektor * 2048u && len == groesse,
          "Port-Blob fuer 0x29 = Index %d @0x%x Laenge %u = Sektor %u * 2048 / 0x%x (der Blob, den das Original laedt)",
          idx, (unsigned)off, (unsigned)len, (unsigned)sektor, (unsigned)groesse);
    free(ems); free(exe);
}

/* ---- gemeinsamer Aufbau: Original-Zustand o4 F137 ---------------------------------------------------- */
typedef struct { int tick, hp_vorher, hp_nach, sub, clip, bild, pmo; int32_t ax, az, px; double abstand; } treffer_t;

static int aufbau_o4(re15_actor_t **pa)
{
    /* Leon auf der Plattform: Original F137 Spieler (-8800,-1800,-4531) Band 1, +0x6a = 0. Der Port fuehrt die
     * Spieler-Gier mit demselben Wert wie +0x6a (gemessen: Sprung-Spawn ROOM3040 Original r0 / Port rot=0). */
    if (room_boot(0x3040, -8800, -1800, -4531, 0, 1, 0, 2) != 0) return 0;
    PRUEF(bank_laden_29(), "EM029-Bank geladen (Pool, Fuss-Sperre, Trefferpunkt)");
    re15_actor_t *a = aktor_vom_typ(0x29, 0), *b = aktor_vom_typ(0x29, 1);
    PRUEF(a != NULL && b != NULL, "zwei Affen 0x29 aus Sce_em_set (ROOM3040.RDT @0x01310/@0x01324)");
    if (!a || !b) return 0;
    PRUEF(a->render_scale_q12 == RE15_AFFEN3_MASSSTAB, "INIT setzt den Massstab +0x166 = 0x%x (@0x80110f54-58)", (unsigned)a->render_scale_q12);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = -8800; pl->y = -1800; pl->z = -4531; pl->rot_y = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->sub_state_1 = 0; pl->motion = 0;
    re15_collision_set_band(1);
    re15_player_cmd_zero();
    /* Affe A (Entity Slot 0) wie o4 F137: ACTIVE Sub 0 (+0x6 = 1), Ruhe-Clip 0x16 Bild 56, +0x8f 0, (-8800,-1800,-6082),
     * +0x6a 3072, +0x1dc 0, HP 105, +0x1d0 0, +0x1d6 1. Affe B ruht weit weg (Original (-13618,-1800,-11800)). */
    a->state = 1; a->sub_state_1 = 0; a->sub_state_2 = 1; a->sub_state_3 = 0;
    a->x = -8800; a->y = -1800; a->z = -6082; a->rot_y = 3072; a->floor = 1;
    a->motion = 0x16; a->anim_frac = 0; a->dog_blocked_ctr = 0; a->hp = 105; a->hit_react = 0;
    a->roach_los = 0; a->sca_wall_hit = 1; a->ai_timer = 200;   /* Ruhe-Zeitgeber laeuft noch (Original wechselt erst durch Leons Naehe) */
    a->anim_frame = 55; re15_affen_pool_anim(a); a->anim_frame = 56;   /* Pool = das zuletzt gezeichnete Bild 55 */
    b->state = 1; b->sub_state_1 = 0; b->sub_state_2 = 1; b->sub_state_3 = 0;
    b->x = -13618; b->y = -1800; b->z = -11800; b->rot_y = 3072; b->floor = 1;
    b->motion = 0x16; b->anim_frac = 0; b->dog_blocked_ctr = 0; b->ai_timer = 2000;
    b->anim_frame = 55; re15_affen_pool_anim(b); b->anim_frame = 56;
    *pa = a;
    return 1;
}

static int lauf_o4(int ticks, treffer_t *t, int tmax, int *commit_tick, int32_t *biss_x, int nbx, int verbose)
{
    re15_actor_t *a = NULL;
    if (!aufbau_o4(&a)) return 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_affen3_log_reset();
    int nt = 0, hp_alt = pl->hp; *commit_tick = -1;
    for (int k = 1; k <= ticks; k++) {
        frame(0, 0);
        if (*commit_tick < 0 && a->sub_state_1 == 5) *commit_tick = k;
        if (*commit_tick > 0 && k - *commit_tick < nbx && biss_x) biss_x[k - *commit_tick] = a->x;
        if (verbose)
            printf("    T%3d hp%d pl %d/%d c%d (%d,%d) | A %d/%d/%d/%d c%d/%d cd%d (%d,%d) r%d d%.0f\n", k, (int)pl->hp,
                   pl->state, pl->sub_state_1, (int)pl->motion, (int)pl->x, (int)pl->z, a->state, a->sub_state_1,
                   a->sub_state_2, a->sub_state_3, (int)a->motion, (int)a->anim_frame, (int)a->dog_blocked_ctr, (int)a->x,
                   (int)a->z, (int)a->rot_y, dist2d(a->x, a->z, pl->x, pl->z));
        if (pl->hp != hp_alt && nt < tmax) {
            t[nt].tick = k; t[nt].hp_vorher = hp_alt; t[nt].hp_nach = pl->hp; t[nt].sub = a->sub_state_1;
            t[nt].clip = a->motion; t[nt].bild = a->anim_frame; t[nt].pmo = pl->motion; t[nt].ax = a->x; t[nt].az = a->z;
            t[nt].px = pl->x;
            t[nt].abstand = dist2d(a->x, a->z, pl->x, pl->z); nt++;
        }
        hp_alt = pl->hp;
        if (pl->hp < 0) break;
    }
    return nt;
}

/* Original o4 (GDB-Spur, Bild = (VSync - 6286)/2): Leon erscheint F137; Treffer in F150, 207, 264, 321, ... 1233
 * (-5 je Biss), Tod F1290. In Ticks ab F137: 13, 70, 127, 184, ... (Abstand 57). */
static const int s_o4_treffer[] = { 13, 70, 127, 184, 241, 298, 355, 412, 469, 526 };
/* Lage x des Affen A im Biss F139..F150 (Commit-Tick und die 11 folgenden), Original: */
static const int32_t s_o4_biss_x[12] = { -8698, -8675, -8666, -8683, -8683, -8683, -8683, -8715, -8713, -8652, -8532, -8385 };

static void teil_biss(re15_ai_flavor_t fl, const char *name)
{
    re15_ai_flavor_set(fl);
    treffer_t t[32]; int commit = -1; int32_t bx[12] = {0};
    const char *v = getenv("R38_AFFEN3_SPUR");
    int nt = lauf_o4(560, t, 32, &commit, bx, 12, v != NULL);
    printf("  [%s] Commit Tick %d, %d Treffer:", name, commit, nt);
    for (int i = 0; i < nt; i++) printf(" T%d(%d->%d Bild %d)", t[i].tick, t[i].hp_vorher, t[i].hp_nach, t[i].bild);
    printf("\n  [%s] Biss-x ab Commit:", name);
    for (int i = 0; i < 12; i++) printf(" %d", (int)bx[i]);
    printf("\n");
    PRUEF(commit == 2, "[%s] Biss-Commit im 2. Tick (Original F139 = F137 + 2): Tick %d", name, commit);
    int alle5 = (nt > 0);
    for (int i = 0; i < nt; i++) if (t[i].hp_vorher - t[i].hp_nach != RE15_AFFEN3_BISS_SCHADEN || t[i].sub != 5 || t[i].clip != 0x12) alle5 = 0;
    PRUEF(nt >= 10 && alle5, "[%s] %d Treffer, jeder genau -%d aus Sub 5 / Clip 0x12 (@0x80111f60)", name, nt, RE15_AFFEN3_BISS_SCHADEN);
    int fenster = (nt > 0);
    for (int i = 0; i < nt; i++) if (t[i].bild < 12 || t[i].bild > 15) fenster = 0;
    PRUEF(fenster, "[%s] alle Treffer im Fenster {12..15} (@0x8011ed48)", name);
    PRUEF(nt > 0 && t[0].tick == s_o4_treffer[0] && t[0].bild == 12,
          "[%s] erster Treffer in Tick %d Bild %d (Original F150 = Tick 13, Bild 12)", name, nt > 0 ? t[0].tick : -1, nt > 0 ? t[0].bild : -1);
    PRUEF(nt > 0 && t[0].abstand > 1000.0,
          "[%s] Mitten-Abstand im Treffer %.0f > 1000 (der alte Port-Ersatz `Abstand < 1000` traf hier nie)", name, nt > 0 ? t[0].abstand : 0.0);
    PRUEF(nt > 0 && t[0].pmo == 8,
          "[%s] Leon flincht von vorn: cmd 2 mit aca59 = a780 + 2 = 2 (@0x80111f7c-94; Original-Spur aca58..59 = 02 02) -> Clip 8: Clip %d",
          name, nt > 0 ? t[0].pmo : -1);
    int maxabw = 0, nsoll = (int)(sizeof s_o4_treffer / sizeof s_o4_treffer[0]);
    for (int i = 0; i < nsoll && i < nt; i++) { int d = t[i].tick - s_o4_treffer[i]; if (d < 0) d = -d; if (d > maxabw) maxabw = d; }
    PRUEF(nt >= nsoll && maxabw <= 2,
          "[%s] Treffer-Takt wie das Original (57 Bilder; Soll 13/70/127/...): groesste Abweichung %d Bilder (Schranke 2 = Fenster-Bild 12..14)",
          name, maxabw);
    int xabw = 0;
    for (int i = 0; i < 12; i++) { int d = (int)(bx[i] - s_o4_biss_x[i]); if (d < 0) d = -d; if (d > xabw) xabw = d; }
    PRUEF(xabw <= 40, "[%s] Lunge-Bahn x im Biss wie das Original F139..F150 (Fuss-Sperre FUN_80115b68 @0x80111e34): groesste Abweichung %d", name, xabw);
}

static void teil_flavor(void)
{
    teil_biss(RE15_AI_FLAVOR_RE2, "RE2-KI (Standard)");
    teil_biss(RE15_AI_FLAVOR_RE15, "RE1.5-KI");
}

static void teil_quadrat(void)
{
    re15_actor_t *a = NULL;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    if (!aufbau_o4(&a)) return;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* bis zum Treffer-Tick laufen, Zustand davor festhalten */
    int hp0 = pl->hp, hit = 0; int32_t px = 0, pz = 0;
    for (int k = 1; k <= 20 && !hit; k++) { px = pl->x; pz = pl->z; frame(0, 0); if (pl->hp != hp0) hit = k; }
    PRUEF(hit > 0, "Treffer im Tick %d", hit);
    if (!hit) return;
    int32_t g[3] = {0};
    int ok = re15_affen3_trefferpunkt(a, RE15_AFFEN3_BISS_RECORD, g);
    printf("    Trefferpunkt Record 9 + (0x64,0,0): (%d,%d,%d), Affe (%d,%d), Leon vor dem Rueckstoss (%d,%d)\n",
           (int)g[0], (int)g[1], (int)g[2], (int)a->x, (int)a->z, (int)px, (int)pz);
    PRUEF(ok, "Zeichenstand vorhanden (Record+0x40 des Vorbilds)");
    re15_actor_t probe = *pl;
    probe.x = px; probe.z = pz;
    PRUEF(abs((int)(probe.x - g[0])) <= RE15_AFFEN3_BISS_R && abs((int)(probe.z - g[2])) <= RE15_AFFEN3_BISS_R,
          "Leon lag im Quadrat +-%d um den Knochenpunkt (|dX| %d, |dZ| %d); Mitten-Abstand zum Affen %.0f",
          RE15_AFFEN3_BISS_R, abs((int)(probe.x - g[0])), abs((int)(probe.z - g[2])), dist2d(a->x, a->z, px, pz));
    probe.z = pz + 1100;
    PRUEF(!re15_affen3_quadrat(a, &probe, RE15_AFFEN3_BISS_RECORD, RE15_AFFEN3_BISS_R),
          "1100 weiter weg (z %d): kein Treffer (Quadrat, kein Bogen, kein Mittenradius @0x8001c080-c0c0)", (int)probe.z);
}

/* Schwerer Angriff gegen die GDB-Spur o6 des Originals: Affe B laeuft in Sub 4 (B[4], Clip 6) den Gang hinunter
 * und steht bei (-13618,-1800,-10549) r1005 an der Wand (+0x1d6 = 1, +0x8c = 196); F1007 setzt Leon nach
 * (-13500,-1800,-14200) r1024 (Abstand 3647 < 0xfa0) -> im SELBEN Tick A[4] @0x80111ad4-b18 -> Sub 6, Clip 0x13 Bild 1.
 * Original: Lage z waehrend des Angriffs F1007..F1029 (unten), Treffer F1029 = Bild 23 (Fenster @0x8011ed4c),
 * HP 100 -> 90 (@0x801122b4), +0x1dc 44, cmd 2 mit aca59 = a780 + 4 = 5 (Niederschlag, @0x801122d4-e8),
 * Leon rutscht z -14200 -> -14700 -> -15195 -> -15507; Ende F1046/F1047 -> Sub 3 (+0x1dc != 0 @0x80112388-3ac). */
static const int32_t s_o6_z[23] = { -10549, -10549, -10549, -10508, -10570, -10570, -10570, -10570, -10683, -10791, -10901,
                                    -11012, -11124, -11124, -11124, -11124, -11124, -11124, -11124, -11124, -11121, -11266, -11430 };

static void teil_schwer(void)
{
    re15_actor_t *a = NULL;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    if (!aufbau_o4(&a)) return;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *b = aktor_vom_typ(0x29, 1);
    if (!b) return;
    /* A ruht weit weg und bleibt aus dem Spiel (Original A: Sub 3 blind an seiner Wand, Abstand > 9000) */
    a->sub_state_1 = 0; a->sub_state_2 = 1; a->ai_timer = 2000; a->dog_blocked_ctr = 0;
    /* B wie o6 F1006 */
    b->state = 1; b->sub_state_1 = 4; b->sub_state_2 = 1; b->sub_state_3 = 0;
    b->x = -13618; b->y = -1800; b->z = -10549; b->rot_y = 1005; b->floor = 1;
    b->motion = 6; b->anim_frac = 0; b->dog_blocked_ctr = 0; b->hit_react = 0; b->crow_speed = 196;
    b->roach_los = 1; b->sca_wall_hit = 1;
    b->anim_frame = 11; re15_affen_pool_anim(b); b->anim_frame = 12;
    pl->x = -13500; pl->y = -1800; pl->z = -14200; pl->rot_y = 1024; pl->hp = 100; pl->hit_react = 0;
    re15_player_cmd_zero();
    re15_affen3_log_reset();
    int hp0 = pl->hp, commit = -1, treffer = -1, bild = -1, nach_sub = -1; int32_t bz[23] = {0}, pzt[8] = {0};
    for (int k = 1; k <= 45; k++) {
        frame(0, 0);
        if (commit < 0 && b->sub_state_1 == 6) commit = k;
        if (k <= 23) bz[k - 1] = b->z;
        if (treffer > 0 && k - treffer < 8) pzt[k - treffer] = pl->z;   /* Leon-z nach Tick treffer+i */
        if (treffer < 0 && pl->hp != hp0) { treffer = k; bild = b->anim_frame; pzt[0] = pl->z; }
        if (treffer > 0 && nach_sub < 0 && b->sub_state_1 != 6) nach_sub = b->sub_state_1;
        if (getenv("R38_AFFEN3_SPUR"))
            printf("    T%2d hp%d pl c%d (%d,%d) | B %d/%d/%d c%d/%d cd%d (%d,%d) r%d\n", k, (int)pl->hp, (int)pl->motion,
                   (int)pl->x, (int)pl->z, b->sub_state_1, b->sub_state_2, b->sub_state_3, (int)b->motion, (int)b->anim_frame,
                   (int)b->dog_blocked_ctr, (int)b->x, (int)b->z, (int)b->rot_y);
    }
    re15_affen3_treffer_t log[8]; int nl = re15_affen3_log(log, 8);
    printf("    schwer: Commit Tick %d, Treffer Tick %d Bild %d, %d HP, danach Sub %d; z:", commit, treffer, bild,
           nl > 0 ? log[0].schaden : 0, nach_sub);
    for (int i = 0; i < 23; i++) printf(" %d", (int)bz[i]);
    printf("\n    Leon z ab Treffer-Tick:"); for (int i = 0; i < 8; i++) printf(" %d", (int)pzt[i]); printf("\n");
    PRUEF(commit == 1, "A[4] -> Sub 6 im selben Tick (Original F1007, a804(0xfa0,0xc0) < 0 @0x80111ae8): Tick %d", commit);
    PRUEF(nl > 0 && log[0].sub == 6 && log[0].clip == 0x13 && log[0].schaden == RE15_AFFEN3_SCHWER_SCHADEN,
          "schwerer Treffer -%d aus Sub 6 / Clip 0x13 (@0x801122b4)", RE15_AFFEN3_SCHWER_SCHADEN);
    PRUEF(treffer == 23 && bild == 23, "Treffer in Tick %d Bild %d (Original F1029 = Tick 23, Bild 23, Fenster @0x8011ed4c)", treffer, bild);
    int zabw = 0;
    for (int i = 0; i < 23; i++) { int d = (int)(bz[i] - s_o6_z[i]); if (d < 0) d = -d; if (d > zabw) zabw = d; }
    PRUEF(zabw <= 40, "Lunge-Bahn z wie das Original F1007..F1029 (Fuss-Sperre c3c/b68 @0x80112124-15c): groesste Abweichung %d", zabw);
    /* Niederschlag nach vorn (Handler [5] 0x8003644c, aca59 = 5): Original-Schritte 500 / 495 / 312 (z -14200 -> -14700
     * -> -15195 -> -15507, Proben F1029..F1031). Verglichen werden die Schrittweiten ab dem ersten Schritt. Der Port
     * beginnt den Vorschub einen Tick spaeter als das Original (game_step_common.c Phase 0 bricht ab; das Original
     * faellt aus Ph0 @0x8003648c-50c in anim_set/Vorschub @0x80036524-5b4 durch) — geteiltes Spieler-Modul, im
     * Dossier unter OFFEN, hier nur ausgegeben. */
    int erster = -1;
    for (int i = 1; i < 8; i++) if (pzt[i] != pzt[0]) { erster = i; break; }
    int s1 = (erster > 0) ? (int)(pzt[erster - 1] - pzt[erster]) : 0;
    int s2 = (erster > 0 && erster + 1 < 8) ? (int)(pzt[erster] - pzt[erster + 1]) : 0;
    int s3 = (erster > 0 && erster + 2 < 8) ? (int)(pzt[erster + 1] - pzt[erster + 2]) : 0;
    printf("    Niederschlag: erster Schritt %d Tick(s) nach dem Treffer-Tick (Original: Spieler-Tick direkt danach = 1), "
           "Schritte %d / %d / %d (Original 500 / 495 / 312)\n", erster, s1, s2, s3);
    PRUEF(erster > 0 && s1 == 500 && s2 == 495 && abs(s3 - 312) <= 60,
          "Niederschlag nach vorn wie im Original (aca59 = a780 + 4 = 5): Schritte %d / %d / %d (Soll 500 / 495 / 312 +-60 Wand)", s1, s2, s3);
    PRUEF(nach_sub == 3, "Ende mit +0x1dc != 0 -> Sub 3 (@0x80112388-3ac): Sub %d", nach_sub);
}

/* ==== DECKE / GRIFF (Subs 9..15) gegen die GDB-Spuren o7/o8 ===========================================
 * Original (DuckStation-GDB, affen_spur2.py): Leon (-8800,-1800,-2400) Band 1, in Bild 150 Affe A per RAM in den HURT
 * gesetzt (+0x4..7 = 02 03 00 00, HP 73) = die Lage nach einem ueberlebten Treffer (Spur 0, Ausgang +0x1e3 -> Sub 9).
 * Start des Port-Laufs = Original-Zustand F167 (Affe A: HURT 2/3/0/0, Clip 5 Bild 24, (-8719,-1800,-6082) r3053,
 * HP 73, +0x93 0, Bodenkasten, +0x1ba -1800; Pool = Bild 23 von Clip 5). */
typedef struct { int tick; uint8_t st, s1, s2, clip, bild; int32_t x, y, z; int16_t r; } decke_t;

/* Leons COMMON-Bank PL00 (PLD/PL00.EDD + .EMR = DAT_800acbc0/acad8) fuer die Haken-Phasen aca5a 6..9 (Clip 0x10/0xb). */
static re15_emd_animation_t s_pl00_anim; static re15_emd_skeleton_t s_pl00_skel;
static uint8_t *s_pl00_edd = NULL, *s_pl00_emr = NULL;
static int pl00_laden(void)
{
    if (s_pl00_edd && s_pl00_emr) return 1;
    char p[600]; size_t esz = 0, rsz = 0;
    snprintf(p, sizeof p, "%s/PLD/PL00.EDD", RE15_ASSET_PSX_DIR); s_pl00_edd = slurp(p, &esz);
    snprintf(p, sizeof p, "%s/PLD/PL00.EMR", RE15_ASSET_PSX_DIR); s_pl00_emr = slurp(p, &rsz);
    if (!s_pl00_edd || !s_pl00_emr) return 0;
    if (re15_emd_parse_animation(s_pl00_edd, esz, &s_pl00_anim) != 0 || re15_emd_parse_skeleton(s_pl00_emr, rsz, &s_pl00_skel) != 0) return 0;
    re15_player_set_pl00_banks(&s_pl00_skel, &s_pl00_anim);
    return 1;
}
static void pl00_in_ctx(void) { s_ctx.pl00_skel = &s_pl00_skel; s_ctx.pl00_anim = &s_pl00_anim; }   /* game_step spiegelt c->pl00_* je Bild (@game_step_common.c) */

static int aufbau_decke(re15_actor_t **pa, int16_t leon_gier)
{
    re15_damage_seed_rng(0x2545f491u);   /* Ersatz-Zufallsquelle (FUN_8001af20-Hash, re15_damage.c) je Lauf gleich: Lauf-Reihenfolge egal */
    if (room_boot(0x3040, -8800, -1800, -2400, leon_gier, 1, 0, 2) != 0) return 0;
    if (!bank_laden_29()) { printf("  FEHLER: EM029-Bank\n"); g_fail++; return 0; }
    if (!pl00_laden()) { printf("  FEHLER: PL00.EDD/EMR"); printf("%c", 10); g_fail++; return 0; }
    pl00_in_ctx();
    re15_actor_t *a = aktor_vom_typ(0x29, 0), *b = aktor_vom_typ(0x29, 1);
    if (!a || !b) return 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = -8800; pl->y = -1800; pl->z = -2400; pl->rot_y = leon_gier; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->sub_state_1 = 0; pl->motion = 0;
    re15_collision_set_band(1);
    re15_player_cmd_zero();
    a->state = 2; a->sub_state_1 = 3; a->sub_state_2 = 0; a->sub_state_3 = 0;
    a->x = -8719; a->y = -1800; a->z = -6082; a->rot_y = 3053; a->floor = 1; a->dog_floor_y = -1800;
    a->hp = 73; a->hit_react = 0; a->dog_blocked_ctr = 0; a->roach_air = 0; a->roach_esc = 1; a->roach_beh2 = 0;
    a->roach_los = 1; a->ai_timer = 252; a->crow_speed = 0;
    a->motion = 5; a->anim_frac = 0; a->anim_frame = 23; re15_affen_pool_anim(a); a->anim_frame = 24;
    b->state = 1; b->sub_state_1 = 0; b->sub_state_2 = 1; b->sub_state_3 = 0;
    b->x = -13618; b->y = -1800; b->z = -11800; b->rot_y = 3072; b->floor = 1;
    b->motion = 0x16; b->anim_frac = 0; b->dog_blocked_ctr = 0; b->ai_timer = 2000;
    b->anim_frame = 55; re15_affen_pool_anim(b); b->anim_frame = 56;
    *pa = a;
    return 1;
}

typedef struct { uint8_t st, s1, s2, clip, bild, air, band; uint16_t rad; int32_t x, y, z; int16_t r;
                 int32_t px, pz; int16_t php; int vs, ph, pmo, pbild; } snap_t;
#define SNAP_N 1500
static snap_t s_snap[SNAP_N + 1];

static int lauf_decke(int16_t leon_gier, int ticks, decke_t *ev, int evmax, int *hp_ab, int *hp_tick, int nhp)
{
    re15_actor_t *a = NULL;
    if (!aufbau_decke(&a, leon_gier)) return 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_affen3_log_reset();
    int n = 0, nh = 0, hp_alt = pl->hp; uint8_t st0 = 0xff, s10 = 0xff, s20 = 0xff, cl0 = 0xff;
    const char *v = getenv("R38_AFFEN3_SPUR");
    for (int k = 1; k <= ticks; k++) {
        frame(0, 0);
        if ((a->state != st0 || a->sub_state_1 != s10 || a->sub_state_2 != s20 || a->motion != cl0) && n < evmax) {
            decke_t *d = &ev[n++];
            d->tick = k; d->st = a->state; d->s1 = a->sub_state_1; d->s2 = a->sub_state_2; d->clip = (uint8_t)a->motion;
            d->bild = (uint8_t)a->anim_frame; d->x = a->x; d->y = a->y; d->z = a->z; d->r = a->rot_y;
            st0 = a->state; s10 = a->sub_state_1; s20 = a->sub_state_2; cl0 = (uint8_t)a->motion;
        }
        if (pl->hp != hp_alt && nh < nhp) { hp_ab[nh] = hp_alt - pl->hp; hp_tick[nh] = k; nh++; }
        hp_alt = pl->hp;
        if (k <= SNAP_N) {
            snap_t *q = &s_snap[k];
            q->st = a->state; q->s1 = a->sub_state_1; q->s2 = a->sub_state_2; q->clip = (uint8_t)a->motion; q->bild = (uint8_t)a->anim_frame;
            q->air = a->roach_air; q->band = a->aim_band; q->rad = a->hit_radius_min; q->x = a->x; q->y = a->y; q->z = a->z; q->r = a->rot_y;
            q->px = pl->x; q->pz = pl->z; q->php = pl->hp; q->vs = re15_player_victim_state(); q->ph = re15_affen3_opfer_phase();
            q->pmo = (int)pl->motion; q->pbild = (int)pl->anim_frame;
        }
        if (v) printf("    T%4d A %d.%d.%d c%d f%d (%d,%d,%d) r%d hr%x air%d r%u | PL (%d,%d) r%d hp%d vs%d ph%d mo%d f%d\n", k,
                      a->state, a->sub_state_1, a->sub_state_2, (int)a->motion, (int)a->anim_frame, (int)a->x, (int)a->y,
                      (int)a->z, (int)a->rot_y, a->hit_react, a->roach_air, (unsigned)a->hit_radius_min, (int)pl->x, (int)pl->z,
                      (int)pl->rot_y, (int)pl->hp, re15_player_victim_state(), re15_affen3_opfer_phase(), (int)pl->motion,
                      (int)pl->anim_frame);
    }
    return n | (nh << 16);
}

/* ==== SPRUNG / TODESSPRUNG (Subs 7/8) gegen die GDB-Spur o9 ===========================================
 * Original (affen_spur2.py --write, Bild 150): Affe A auf den offenen Boden versetzt (+0x34/+0x3c = (-17000,-9000),
 * +0x38 = 0, +0x82 = 0, +0x1ba = 0), Sub 0 Phase 1 mit Zeitgeber +0x9c = 30 (ohne Fuss-Sperre, damit die Zeichen-
 * Records am neuen Ort neu entstehen); Leon (-8000,0,-3000) Band 0, Gier 1665 (blickt zum Affen: a780 = 0), HP 40.
 * Original-Ablauf (T = F - 167): T31 Sub 3 (Zeitgeber), T32 Sub 4 (Abstand >= 0x1771 + Sicht, A[3] @0x801118b4-8fc),
 * T41 SPRUNG Sub 7 (A[4] @0x80111b28-bf0), T50 Absprung Bild 0xa (+0x1e0 = 1), T72 Landung, T80 Sub 4, T81 SCHWER Sub 6,
 * T102 Treffer 40 -> 30; ... T284 Sprung Bild 0x13 bei HP 20 < 50 -> TODESSPRUNG Sub 8 (@0x8011267c-710), T293 Treffer im
 * Fenster {8,9,10} (@0x8011ed54): HP 20 -> -480 (-500 @0x80112b00), Spieler cmd 6 Clip 0 (Haken 0x801160a4), +0x1dc 0x12c,
 * T297 Landung, T305 Sub 2 (Spieler-HP < 0 @0x80112bbc-c30), T363 Spieler cmd 7 (tot). Die Ersatz-Zufallsquelle des Ports
 * (re15_damage.c, FUN_8001af20 haengt im Original am a0 des Aufrufers) laesst Bahn und Takt abweichen; geprueft wird die
 * Mechanik: Sprung -> Schwer/Treffer -> Todessprung bei HP < 50 -> -500 im Fenster -> cmd 6 -> Tod. */
static void teil_sprung_fl(re15_ai_flavor_t fl)
{
    re15_ai_flavor_set(fl);
    printf("  [%s]\n", fl == RE15_AI_FLAVOR_RE2 ? "RE2-KI (Standard)" : "RE1.5-KI");
    re15_damage_seed_rng(0x2545f491u);
    if (room_boot(0x3040, -8000, 0, -3000, 1665, 0, 0, 2) != 0) return;
    if (!bank_laden_29() || !pl00_laden()) { printf("  FEHLER: Banken\n"); g_fail++; return; }
    pl00_in_ctx();
    re15_actor_t *a = aktor_vom_typ(0x29, 0), *b = aktor_vom_typ(0x29, 1);
    if (!a || !b) { g_fail++; return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = -8000; pl->y = 0; pl->z = -3000; pl->rot_y = 1665; pl->hp = 40; pl->hit_react = 0;
    pl->state = 1; pl->sub_state_1 = 0; pl->motion = 0;
    re15_collision_set_band(0);
    re15_player_cmd_zero();
    a->state = 1; a->sub_state_1 = 0; a->sub_state_2 = 1; a->sub_state_3 = 0;
    a->x = -17000; a->y = 0; a->z = -9000; a->rot_y = 3087; a->floor = 0; a->dog_floor_y = 0;
    a->hp = 105; a->hit_react = 0; a->dog_blocked_ctr = 0; a->roach_air = 0; a->roach_esc = 1; a->roach_beh2 = 0;
    a->roach_los = 1; a->ai_timer = 30; a->crow_speed = 0;
    a->motion = 5; a->anim_frac = 0; a->anim_frame = 23; re15_affen_pool_anim(a); a->anim_frame = 24;
    b->state = 1; b->sub_state_1 = 0; b->sub_state_2 = 1; b->sub_state_3 = 0; b->ai_timer = 2000;
    re15_affen3_log_reset();
    int t_sprung = -1, t_schwer = -1, t_tod_sub8 = -1, t_tod = -1, tod_bild = -1, t_cmd6 = -1, t_sub2 = -1;
    int n_sprung = 0, n_schwer_treffer = 0, hp_alt = pl->hp, tod_ab = 0, s1_alt = a->sub_state_1, sprung_bild13_hp = -1;
    const char *v = getenv("R38_AFFEN3_SPUR");
    for (int k = 1; k <= 700; k++) {
        frame(0, 0);
        if (a->sub_state_1 == 7 && s1_alt != 7) { n_sprung++; if (t_sprung < 0) t_sprung = k; }
        if (a->sub_state_1 == 6 && s1_alt != 6 && t_schwer < 0) t_schwer = k;
        if (a->sub_state_1 == 8 && s1_alt != 8 && t_tod_sub8 < 0) { t_tod_sub8 = k; sprung_bild13_hp = hp_alt; }
        if (pl->hp != hp_alt) {
            int d = hp_alt - pl->hp;
            if (d == RE15_AFFEN3_SCHWER_SCHADEN) n_schwer_treffer++;
            if (d == RE15_AFFEN3_TOD_SCHADEN && t_tod < 0) { t_tod = k; tod_bild = a->anim_frame; tod_ab = d; }
        }
        if (t_tod > 0 && t_cmd6 < 0 && re15_player_victim_state() == 2) t_cmd6 = k;
        if (t_tod > 0 && t_sub2 < 0 && a->sub_state_1 == 2) t_sub2 = k;
        if (v) printf("    T%3d A %d.%d.%d c%d f%d (%d,%d,%d) r%d hr%x air%d | PL (%d,%d) hp%d vs%d st%d mo%d f%d\n", k, a->state,
                      a->sub_state_1, a->sub_state_2, (int)a->motion, (int)a->anim_frame, (int)a->x, (int)a->y, (int)a->z,
                      (int)a->rot_y, a->hit_react, a->roach_air, (int)pl->x, (int)pl->z, (int)pl->hp, re15_player_victim_state(),
                      pl->state, (int)pl->motion, (int)pl->anim_frame);
        hp_alt = pl->hp; s1_alt = a->sub_state_1;
    }
    printf("    Sprung T%d (%d Spruenge), Schwer T%d (%d Treffer -10), Todessprung T%d (HP davor %d), Treffer -%d T%d Bild %d, cmd 6 T%d, Sub 2 T%d\n",
           t_sprung, n_sprung, t_schwer, n_schwer_treffer, t_tod_sub8, sprung_bild13_hp, tod_ab, t_tod, tod_bild, t_cmd6, t_sub2);
    PRUEF(t_sprung > 0, "Sprung Sub 7 aus A[4] (Original T41)");
    PRUEF(n_schwer_treffer >= 1, "schwerer Angriff trifft -10 (Original T102 40 -> 30)");
    PRUEF(t_tod_sub8 > 0 && sprung_bild13_hp < RE15_AFFEN3_TOD_HP_GRENZE, "Todessprung Sub 8 erst bei Spieler-HP < 50 (@0x8011269c): HP davor %d", sprung_bild13_hp);
    PRUEF(t_tod > 0 && tod_ab == RE15_AFFEN3_TOD_SCHADEN && tod_bild >= 8 && tod_bild <= 10,
          "Todessprung trifft -500 im Fenster {8,9,10} (@0x80112b00 / @0x8011ed54): Bild %d", tod_bild);
    PRUEF(t_cmd6 > 0, "Spieler cmd 6 = Haken 0x801160a4 (Gorilla-Finisher-Weg) nach dem Treffer");
    PRUEF(t_sub2 > 0, "Affe danach Sub 2 (Spieler-HP < 0 @0x80112bbc-c30)");
    PRUEF(pl->state == 7, "Spieler am Ende tot (aca58 = 7 @0x80116214): Zustand %d", pl->state);
}
static void teil_sprung(void) { teil_sprung_fl(RE15_AI_FLAVOR_RE2); teil_sprung_fl(RE15_AI_FLAVOR_RE15); }

/* Pruefung eines Bildes gegen das Original (exakt). */
static int snap_gleich(int t, uint8_t st, uint8_t s1, uint8_t s2, int clip, int bild, int32_t x, int32_t y, int32_t z)
{
    const snap_t *q = &s_snap[t];
    return q->st == st && q->s1 == s1 && q->s2 == s2 && (clip < 0 || q->clip == clip) && (bild < 0 || q->bild == bild)
        && q->x == x && q->y == y && q->z == z;
}

/* o7: Leon blickt den Affen an (Gier 1024). Original-Bilder (T = F - 167, GDB-Spur o7_decke.txt):
 *   T13 Sub 9 (HURT-Ausgang +0x1e3 @0x80114a1c-2c), T38 Decke y -4500 (@0x80112f10-28), T110 Sub 12, T111 Clip 15
 *   (-8706,-4500,-6076), T160 Sub 13 (-8701,-4500,-6382), T161 Clip 24 (-8685,-4500,-6371), T177 vom Spieler
 *   zurueckgeschoben (-8818,-4500,-3646), T210 Sub 14 Hieb (-8987,-4500,-3763) r3075, T281 Hieb 2 (-8989,-4500,-3651)
 *   r3243; in 1600 Bildern KEIN HP-Abzug (Leon blickt den Affen an, die Hiebe gehen am geschobenen Leon vorbei). */
static void teil_decke_fl(re15_ai_flavor_t fl)
{
    decke_t ev[256]; int ab[64], tk[64];
    re15_ai_flavor_set(fl);
    printf("  [%s]\n", fl == RE15_AI_FLAVOR_RE2 ? "RE2-KI (Standard)" : "RE1.5-KI");
    int rv = lauf_decke(1024, 1450, ev, 256, ab, tk, 64);
    int nh = rv >> 16;
    PRUEF(snap_gleich(13, 1, 9, 0, 7, 0, -8719, -1800, -6082), "T13 HURT-Ausgang -> Sub 9 (Original F180)");
    PRUEF(snap_gleich(38, 1, 9, 3, 0x0c, 25, -8719, -4500, -6082) && s_snap[38].air == 1 && s_snap[38].rad == 800,
          "T38 an der Decke: y = +0x1ba - 2700 = -4500 (@0x80112f10-28), Luftkasten r 800 (@0x8011ec20), +0x1e0 = 1 (Original F205)");
    PRUEF(snap_gleich(110, 1, 12, 0, 0x0c, 0, -8719, -4500, -6082), "T110 Sub 12 (Original F277)");
    PRUEF(snap_gleich(111, 1, 12, 1, 0x0f, 1, -8706, -4500, -6076), "T111 Kriechen Clip 0xf, Fuss-Sperre c3c (-8706,-6076) (Original F278)");
    PRUEF(snap_gleich(160, 1, 13, 0, 0x0f, 50, -8701, -4500, -6382), "T160 Sub 13 bei Bild 0x32 (@0x801137bc) (-8701,-6382) (Original F327)");
    PRUEF(snap_gleich(161, 1, 13, 1, 0x18, 1, -8685, -4500, -6371), "T161 Jagd Clip 0x18 (-8685,-6371) (Original F328)");
    PRUEF(snap_gleich(177, 1, 13, 1, 0x18, 17, -8818, -4500, -3646),
          "T177 an der Decke durch den Gang, vom Spieler zurueckgeschoben (aec4 @0x80110b88): (%d,%d) (Original -8818/-3646)",
          (int)s_snap[177].x, (int)s_snap[177].z);
    PRUEF(snap_gleich(210, 1, 14, 1, 0x1a, 1, -8987, -4500, -3763) && s_snap[210].r == 3075,
          "T210 Hieb Sub 14 (A[13] Fenster {0,0x31} @0x8011ed59) bei (-8987,-3763) r3075 (Original F377)");
    PRUEF(snap_gleich(281, 1, 14, 1, 0x1a, 1, -8989, -4500, -3651) && s_snap[281].r == 3243,
          "T281 zweiter Hieb (-8989,-3651) r3243 (Original F448)");
    PRUEF(s_snap[300].band == 4 && s_snap[300].air == 1,
          "Zielband an der Decke = NUR UP (@0x80111184-1b0 / FUN_80012a0c(0x1388)): %u", (unsigned)s_snap[300].band);
    PRUEF(nh == 0, "kein HP-Abzug in 1450 Bildern (Original o7: 0 in 1600) - %d", nh);
}

/* o8: Leon kehrt den Ruecken zu (Gier 3072). Original-Bilder (o8_griff.txt): T174 GRIFF Sub 15 (A[13] @0x80113a1c-a94),
 *   Affe (-8818,-1400,-3283), Spieler cmd 5; Haken-Phasen (aca5a) 1 -> 2 (T212) -> 3 (T213, Clip 2) -> 5 (T313, Clip 3)
 *   -> 6 (T364) -> 7 (T365, eigener Clip 0x10) -> 8 (T380) -> 9 (T381, Clip 0xb) -> 10 (T405) -> frei (T406);
 *   Leon (-8777,-2711) T210, (-9132,-1993) T330; Affe auf dem Boden (-9118,-1800,-3547) T365, Sub 2 T385;
 *   im Griff KEIN HP-Abzug; danach Bisse T458/516/574/632/... je -5. */
static void teil_griff_fl(re15_ai_flavor_t fl)
{
    decke_t ev[256]; int ab[64], tk[64];
    re15_ai_flavor_set(fl);
    printf("  [%s]\n", fl == RE15_AI_FLAVOR_RE2 ? "RE2-KI (Standard)" : "RE1.5-KI");
    int rv = lauf_decke(3072, 1450, ev, 256, ab, tk, 64);
    int nh = rv >> 16;
    PRUEF(snap_gleich(174, 1, 15, 1, 0x1c, 1, -8818, -1400, -3283) && s_snap[174].vs == 1 && s_snap[174].ph == 1 && s_snap[174].pmo == 1,
          "T174 Griff von der Decke: Affe (-8818,-1400,-3283) Clip 0x1c, Spieler cmd 5 Haken-Phase 1 Clip 1 (Original F341)");
    PRUEF(s_snap[210].px == -8777 && s_snap[210].pz == -2711,
          "T210 Leon-Platzierung ad68 (-8777,-2711) (Original F377): (%d,%d)", (int)s_snap[210].px, (int)s_snap[210].pz);
    PRUEF(s_snap[213].ph == 3 && s_snap[213].pmo == 2 && s_snap[213].s2 == 3, "T213 Ringen: Haken 3 Clip 2, Affe Phase 3 (Original F380)");
    PRUEF(s_snap[312].ph == 3 && s_snap[313].ph == 5 && s_snap[313].pmo == 3,
          "T313 Ringen vorbei (+0x9c 100 -> < 0 @0x80114524) -> Haken 5 Clip 3 (Original F480)");
    PRUEF(s_snap[330].px == -9132 && s_snap[330].pz == -1993,
          "T330 Leon (-9132,-1993) (Platzierung, dann Wand @0x80031d70) (Original F497): (%d,%d)", (int)s_snap[330].px, (int)s_snap[330].pz);
    PRUEF(snap_gleich(365, 1, 15, 7, 3, 30, -9118, -1800, -3547) && s_snap[365].rad == 1100 && s_snap[365].air == 0,
          "T365 Affe faellt auf den Boden (-9118,-1800,-3547), Bodenkasten (@0x80114608-6cc) (Original F532)");
    PRUEF(s_snap[365].ph == 7 && s_snap[365].pmo == 0x10 && s_snap[365].pbild == 1, "T365 Leon eigene Bank Clip 0x10 Bild 1 (Original F532)");
    PRUEF(s_snap[381].ph == 9 && s_snap[381].pmo == 0x0b && s_snap[381].pbild == 1, "T381 Leon Clip 0xb (a2 = 1) Bild 1 (Original F548)");
    PRUEF(s_snap[405].ph == 10 && s_snap[406].vs == 0 && s_snap[406].ph == 0xff, "T405 Haken 10, T406 frei (aca58 = 1 @0x80116018) (Original F572/F573)");
    PRUEF(s_snap[385].s1 == 2 && s_snap[385].s2 == 1, "T385 Affe Sub 2 nach dem Griff (@0x80114710-77c) (Original F552)");
    int im_griff = 0; for (int i = 0; i < nh; i++) if (tk[i] >= 174 && tk[i] <= 406) im_griff++;
    PRUEF(im_griff == 0, "kein HP-Abzug im Griff (Original o8): %d", im_griff);
    static const int soll[8] = { 458, 516, 574, 632, 690, 748, 806, 864 };
    int ok = (nh >= 8);
    for (int i = 0; i < 8 && i < nh; i++) if (tk[i] != soll[i] || ab[i] != RE15_AFFEN3_BISS_SCHADEN) ok = 0;
    PRUEF(ok, "danach Bisse aus dem Gang in den Original-Bildern T458/516/574/632/690/748/806/864 je -5: %d Treffer, erster T%d",
          nh, nh > 0 ? tk[0] : -1);
}

static void teil_decke(void) { teil_decke_fl(RE15_AI_FLAVOR_RE2); teil_decke_fl(RE15_AI_FLAVOR_RE15); }
static void teil_griff(void) { teil_griff_fl(RE15_AI_FLAVOR_RE2); teil_griff_fl(RE15_AI_FLAVOR_RE15); }

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    printf("=== test_r38_affen3 [%s] ===\n", teil);
    int alle = (strcmp(teil, "alle") == 0);
    if (alle || !strcmp(teil, "typ"))     teil_typ();
    if (alle || !strcmp(teil, "biss"))    teil_biss(RE15_AI_FLAVOR_RE2, "RE2-KI (Standard)");
    if (alle || !strcmp(teil, "flavor"))  teil_flavor();
    if (alle || !strcmp(teil, "quadrat")) teil_quadrat();
    if (alle || !strcmp(teil, "schwer"))  teil_schwer();
    if (alle || !strcmp(teil, "decke"))   teil_decke();
    if (alle || !strcmp(teil, "griff"))   teil_griff();
    if (alle || !strcmp(teil, "sprung"))  teil_sprung();
    if (!strcmp(teil, "spur_o7") || !strcmp(teil, "spur_o8")) {   /* Messlauf ohne Pruefung (R38_AFFEN3_SPUR=1 fuer jedes Bild) */
        decke_t ev[256]; int ab[64], tk[64];
        int rv = lauf_decke(!strcmp(teil, "spur_o7") ? 1024 : 3072, 1450, ev, 256, ab, tk, 64);
        int n = rv & 0xffff, nh = rv >> 16;
        for (int i = 0; i < n; i++)
            printf("  E T%d st%d s%d.%d c%d f%d (%d,%d,%d) r%d\n", ev[i].tick, ev[i].st, ev[i].s1, ev[i].s2, ev[i].clip, ev[i].bild,
                   (int)ev[i].x, (int)ev[i].y, (int)ev[i].z, (int)ev[i].r);
        for (int i = 0; i < nh; i++) printf("  HP T%d -%d\n", tk[i], ab[i]);
    }
    printf("=== %s: %d Fehler ===\n", g_fail ? "FAIL" : "PASS", g_fail);
    return g_fail ? 1 : 0;
}
