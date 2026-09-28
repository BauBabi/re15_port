/* probe_r30_birkin_frost.c — Runde 30, Spur birkin-frost: die FROST-SCHRANKE der Birkin-Wurzel,
 * gemessen an ALLEN 13 Birkin-Spawns der ausgelieferten RDTs.
 * Dossier: analysis/befunde_runde30/nachschliff-room5080.md, Abschnitt 9.
 *
 * ORIGINAL (selbst disassembliert, Dossier 9.1):
 *   Wurzel STAGE5 0x80116a44 / STAGE3 0x80116230:
 *     @0x80116a68 / @0x80116254  bne (g_pauseflags & 0x20000000)      -> Schatten-Schwanz
 *     @0x80116a7c / @0x80116268  lbu v0,9(a0)
 *     @0x80116a84 / @0x80116270  andi v0,v0,0x20
 *     @0x80116a88 / @0x80116274  bne v0,zero,0x80116eb8 / 0x801166a4 -> NUR jal 0x8001b064 (Schatten)
 *   Sce_em_set: @0x8004256c/@0x80042570 Bit 0x20 loeschen, @0x8004259c jalr Wurzel EINMAL
 *   (INIT: Nibble 3 -> Sub 9 @0x80116890, +0x95=0x10 @0x80116880; Nibble 1 -> Sub 10 @0x801168b8),
 *   @0x80042604/@0x80042608 Bit zuruecksetzen.
 *   Freigabe = Member_set (0x34) Index 0x0C schreibt +0x9 (@0x800411f4 `sb a2,9(a0)`).
 *
 * JE SPAWN (Tabelle unten, alle Bytes beim Start gegen die RDT geprueft):
 *   (a) VOR DER FREIGABE: in JEDEM Bild steht Birkin auf der Record-Lage, grid mit 0x20,
 *       Zustand 1 / Sub wie der INIT beim Spawn (9 / 10 / 0), Pose unveraendert, kein Treffer,
 *       kein Griff. Der INIT lief schon im SPAWN-Bild (Zustand 1 direkt nach dem SCD-Tick, der
 *       Sce_em_set ausfuehrt, VOR dem ersten KI-Tick).
 *   (b) DIE FREIGABE WIRD ERREICHT: grid verliert 0x20 durch GENAU den Member_set am
 *       Datei-Offset der Tabelle (Thread-PC-Klammer vor/nach dem SCD-Tick), neuer Wert = Record.
 *   (c) DANACH: Nibble 3 -> Sub 9 (EMERGENCE, Clip 0x10) laeuft, dann WALK (Sub 1), dann
 *       Angriff (Spieler-hp sinkt oder Griff). Nibble 1 -> Sub 10 -> Angriff. Nibble 4 -> Sub 0/1
 *       -> Angriff. G5 (ROOM5090/5091, eigenes Modul): scharf (x = -9000 @0x801011d0), Angriff.
 *   Spawns ohne Bit 0x20 (grid 0x10): kein Frost, INIT im Spawn-Bild, dann Kampf.
 * Ein Raum, in dem (b) nie eintritt, ist ein SOFTLOCK und laesst die Sonde rot.
 *
 * Aufruf: probe_r30_birkin_frost [raum=5080] [n=<bilder nach der Freigabe>]
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
#include "re15_md1.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_skeleton.h"
#include "re15_tim.h"
#include "re15_fade.h"
#include "re15_math.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

typedef struct { uint8_t bank, bit, val; } flag_t;

typedef struct {
    const char *pfad;        /* unter shared_assets/PSX */
    uint16_t    raum;
    uint32_t    spawn_off;   /* Sce_em_set 0x44 */
    uint8_t     typ, grid;   /* Record-Bytes pc[2]/pc[3] */
    uint32_t    frei_off;    /* Member_set 34 0c <v> 00, 0 = kein Frost */
    uint8_t     frei_wert;
    int         aot;         /* Event-AOT-Slot, -1 = Folge startet selbst, -2 = G5: nach Westen */
    int32_t     px, pz;      /* Spieler-Lage (Tuerziel des Raums) */
    int16_t     prot;
    int         cut;
    int32_t     ax, az;      /* Spieler-Lage beim Ausloesen (Mitte des AOT-Rechtecks) */
    flag_t      fl[2];
    int         nfl;
    uint8_t     sub_frost;   /* Sub des INIT beim Spawn */
    int         angriff;     /* 1 = (c) verlangt einen Treffer; 0 = nicht verlangt (Grund in `ohne`) */
    const char *ohne;
} fall_t;

/* Tabelle: Record-Offsets aus birkin_band_zensus.py (13 Spawns), Freigabe-Offsets aus
 * scd_dump_room.py, Tuerziele aus den Door_aot_set-Records, die in den Raum fuehren
 * (ROOM4000 @0x01104 -> 3070, ROOM3070 @0x03302 -> 3080, ROOM6010 @0x00FCE -> 5080,
 * ROOM50D0 @0x0121A -> 50E0, ROOM50D0 @0x011FA -> 50F0, ROOM5090 @0x0108E -> 5090). */
static const fall_t FAELLE[] = {
    { "STAGE3/ROOM3070.RDT", 0x3070, 0x033CE, 0x30, 0x33, 0x0358E, 0x13,  2,
      -25773, -12208, 105, 4,  -26155,  -8635, {{0,0,0},{0,0,0}}, 0, 9, 1, NULL },
    { "STAGE3/ROOM3070.RDT", 0x3070, 0x0340C, 0x30, 0x10, 0,       0,    -1,
      -25773, -12208, 105, 4,  0, 0,           {{4,215,1},{0,0,0}}, 1, 0, 1, NULL },
    { "STAGE3/ROOM3071.RDT", 0x3071, 0x03434, 0x30, 0x21, 0x03645, 0x01,  2,
      -25773, -12208, 105, 4,  -26155,  -8635, {{0,0,0},{0,0,0}}, 0, 10, 0, "Sub 10 CHARGE-COMBO 0x80119524 ist im Port kompakt (OFFEN); die Folge wartet auf (5,31)/(5,30), die nur dessen Phasen setzen (@0x80119658/@0x801197b4)" },
    { "STAGE3/ROOM3071.RDT", 0x3071, 0x0347A, 0x30, 0x10, 0,       0,    -1,
      -25773, -12208, 105, 4,  0, 0,           {{4,75,1},{0,0,0}}, 1, 0, 1, NULL },
    { "STAGE3/ROOM3080.RDT", 0x3080, 0x007D4, 0x36, 0x24, 0x00934, 0x04, -1,
      -25850, -25900, 930, 0,  0, 0,           {{0,0,0},{0,0,0}}, 0, 0, 0, "grid 4 = Szenenmodus: BRAIN @0x80116d9c-db4 waehlt ueber @0x8011eea4[4] = 0x80116e48 die Tabellen @0x8011ef28/@0x8011ef2c (DECIDE 0x80118cf8 = jr ra, ACT 0x80118d00 = 15-Phasen-Choreografie) — kein Angriff im Original, im Port nicht portiert (OFFEN)" },
    { "STAGE5/ROOM5080.RDT", 0x5080, 0x00746, 0x30, 0x33, 0x0083E, 0x13,  1,
       -9950, -18200, 2048, 0, -26900, -18050, {{0,0,0},{0,0,0}}, 0, 9, 1, NULL },
    { "STAGE5/ROOM5081.RDT", 0x5081, 0x00742, 0x30, 0x33, 0x00836, 0x13,  1,
       -9950, -18200, 2048, 0, -26900, -18050, {{0,0,0},{0,0,0}}, 0, 9, 1, NULL },
    { "STAGE5/ROOM5090.RDT", 0x5090, 0x0124A, 0x30, 0x33, 0x0130A, 0x13, -2,
       25600, -23350, 1024, 14, 0, 0,          {{0,0,0},{0,0,0}}, 0, 0, 1, NULL },
    { "STAGE5/ROOM5091.RDT", 0x5091, 0x01232, 0x30, 0x33, 0x012EE, 0x13, -2,
       25600, -23350, 1024, 14, 0, 0,          {{0,0,0},{0,0,0}}, 0, 0, 1, NULL },
    { "STAGE5/ROOM50E0.RDT", 0x50E0, 0x00ACE, 0x30, 0x33, 0x00BEE, 0x13,  1,
      -25450, -14700, 0, 0,    -13050, -16450, {{0,0,0},{0,0,0}}, 0, 9, 1, NULL },
    { "STAGE5/ROOM50E0.RDT", 0x50E0, 0x00AEE, 0x30, 0x10, 0,       0,    -1,
      -25450, -14700, 0, 0,    0, 0,           {{3,61,1},{0,0,0}}, 1, 0, 0, "grid 0x10, Spawn weit vom Tuerziel; ohne Nav-Steer 0x80039e7c (OFFEN) bleibt er an der Kulisse haengen — kein Frost-Thema" },
    { "STAGE5/ROOM50F1.RDT", 0x50F1, 0x009AC, 0x30, 0x33, 0x00AB4, 0x13,  1,
      -24550, -23650, 0, 1,    -11850, -14000, {{0,0,0},{0,0,0}}, 0, 9, 1, NULL },
    { "STAGE5/ROOM50F1.RDT", 0x50F1, 0x009D4, 0x30, 0x10, 0,       0,    -1,
      -24550, -23650, 0, 1,    0, 0,           {{3,61,1},{0,0,0}}, 1, 0, 0, "grid 0x10, Spawn weit vom Tuerziel; ohne Nav-Steer 0x80039e7c (OFFEN) bleibt er an der Kulisse haengen — kein Frost-Thema" },
};
#define NFAELLE ((int)(sizeof FAELLE / sizeof FAELLE[0]))

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_fail = 0;
static uint8_t           *s_raw = NULL;
static size_t             s_rsz = 0;

#define PIN(cond, ...) do { if (!(cond)) { s_fail++; printf("    RIEGEL FEHLT: " __VA_ARGS__); printf("\n"); } \
                            else { printf("    RIEGEL ok: " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* Die Baenke wie zur Laufzeit: EM030/EM036 aus dem RE1.5-CDEMD0.EMS (Cliplaengen der
 * RE1.5-Wurzel), fuer den G5 (0x36 in ROOM5090/5091) die RE2-Bank wie probe_5090_birkin. */
static int load_re15_bank(uint8_t typ)
{
    size_t n = 0, off = 0, len = 0;
    static uint8_t *ems = NULL; static size_t ems_n = 0;
    if (!ems) ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &ems_n);
    if (!ems) return 0;
    n = ems_n;
    int idx = re15_ems_index_for_type(typ);
    if (idx < 0 || re15_ems_get_entry(ems, n, idx, &off, &len) != 0) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(typ);
    if (!eb) eb = re15_enemy_alloc(typ);
    if (!eb) return 0;
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (re15_emd_parse_container(ems + off, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return 0;
    eb->buf = NULL; eb->ok = 1;
    return 1;
}
static int load_g5_bank(void)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
    if (!ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(0x36);
    if (!eb) eb = re15_enemy_alloc(0x36);
    if (!eb) { free(ems); return 0; }
    if (re2_ems_load_bank(ems, n, 0x36, eb, NULL) != 0) { free(ems); return 0; }
    eb->buf = NULL; eb->ok = 1;
    return 1;
}

static int s_cine_was_active = 0;
static uint16_t s_pad = 0;
/* Ein Bild wie der echte Hauptlauf (derselbe Rahmen wie probe_r30_n_room5080/probe_5090_birkin).
 * nach_scd != NULL: Rueckruf direkt nach dem SCD-Tick, VOR re15_game_step (= vor dem KI-Tick). */
static void frame(void (*nach_scd)(void))
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    if (nach_scd) nach_scd();
    re15_actor_step_all_walkers();
    {
        int cine_active = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
        re15_letterbox_tick(re15_game_flag_get(1, 27));
        if (cine_active) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was_active) { g_scd.letterbox_countdown = 15; }
        s_cine_was_active = cine_active;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
            g_scd.player_mode = 0;
            re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                               g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        }
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = s_pad;
    s_ctx.pad_pressed = s_pad;
    re15_game_step(&s_ctx);
}

/* ---- Beobachtung je Fall ---- */
static const fall_t *s_f = NULL;
static int     s_bslot = -1;
static int32_t s_rx, s_ry, s_rz;           /* Record-Lage */
static int     s_spawn_state = -1, s_spawn_sub = -1, s_spawn_frame95 = -1, s_spawn_grid = -1;

static void finde_birkin(void)
{
    if (s_bslot >= 0) return;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || (e->type != 0x30 && e->type != 0x36)) continue;
        if (e->x != s_rx || e->z != s_rz) continue;
        s_bslot = s;
        s_spawn_state = e->state; s_spawn_sub = e->sub_state_1;
        s_spawn_frame95 = e->anim_frame; s_spawn_grid = e->grid_id;
        return;
    }
}

static int32_t s16le(const uint8_t *p) { return (int16_t)(p[0] | (p[1] << 8)); }

static void bereite_raum(const fall_t *f)
{
    memset(&s_cam, 0, sizeof s_cam);
    memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = f->cut;
    re15_game_state_init();
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    g_current_room_id = f->raum; g_room_change.pending = 0;
    for (int i = 0; i < f->nfl; i++) re15_game_flag_set(f->fl[i].bank, f->fl[i].bit, f->fl[i].val);
    if ((f->raum & 0xFFFEu) == 0x5090u) load_g5_bank(); else load_re15_bank(f->typ);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = f->px; pl->y = 0; pl->z = f->pz; pl->rot_y = f->prot;
    re15_collision_set_band(0);
    s_cine_was_active = 0; s_pad = 0;
}

typedef struct {
    int spawn_ok, init_im_spawnbild;
    int vor_bilder, vor_verletzt, vor_erstes;      /* (a) */
    char vor_grund[160];
    int frei_bild, frei_off, frei_grid;            /* (b) */
    int emerg_bild, walk_bild, angriff_bild, hp_min, griff_bild, armiert_bild;
    int sub_frost_nach;                            /* der eingefrorene Sub lief nach der Freigabe */
    int folge_ende;                                /* erstes Bild nach der Freigabe mit (1,27)=(2,7)=0 */
    int32_t y_frei, y_spaeter;                     /* Hoehe beim Freigabe-Bild / 200 Bilder spaeter */
    char subs[200];
} erg_t;

static int s_frei_off_gesehen = -1;
static int s_spur = 0;             /* spur=<n>: je n Bilder nach dem Ausloesen eine Zeile */
static const uint8_t *s_pc_vor[SCD_THREAD_COUNT];
static uint8_t s_akt_vor[SCD_THREAD_COUNT];

static void pc_merken(void)
{
    for (int i = 0; i < SCD_THREAD_COUNT; i++) { s_pc_vor[i] = g_scd.threads[i].pc; s_akt_vor[i] = g_scd.threads[i].active; }
}
/* Welcher Thread hat im letzten SCD-Tick den Freigabe-Opcode ueberschritten? */
static int pc_klammer(uint32_t off)
{
    const uint8_t *ziel = s_raw + off;
    for (int i = 0; i < SCD_THREAD_COUNT; i++) {
        if (!s_akt_vor[i] || !s_pc_vor[i]) continue;
        const uint8_t *nach = g_scd.threads[i].pc;
        if (s_pc_vor[i] <= ziel && ziel < nach && (nach - s_pc_vor[i]) < 0x400) return (int)off;
        if (s_pc_vor[i] <= ziel && ziel < s_pc_vor[i] + 0x400 && !g_scd.threads[i].active) return (int)off;
    }
    return -1;
}

static void nach_scd_spawn(void) { finde_birkin(); }

static void lauf(const fall_t *f, int nach_n, erg_t *r)
{
    memset(r, 0, sizeof *r);
    r->frei_bild = r->emerg_bild = r->walk_bild = r->angriff_bild = r->griff_bild = r->armiert_bild = -1;
    r->vor_erstes = -1; r->frei_off = -1; r->hp_min = 100; r->sub_frost_nach = 0; r->folge_ende = -1;
    s_f = f; s_bslot = -1; s_frei_off_gesehen = -1;
    s_spawn_state = s_spawn_sub = s_spawn_frame95 = s_spawn_grid = -1;
    s_rx = s16le(s_raw + f->spawn_off + 8); s_ry = s16le(s_raw + f->spawn_off + 10);
    s_rz = s16le(s_raw + f->spawn_off + 12);

    bereite_raum(f);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, f->px, f->pz, f->cut);
    g_scd.cut_auto_enabled = 1;
    s_shown = f->cut;
    finde_birkin();              /* falls sub00 schon im Reenter gespawnt hat */

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int g5 = (f->raum & 0xFFFEu) == 0x5090u;
    int frost = (f->grid & 0x20) != 0;
    int vor_n = frost ? 300 : 0;
    int bild = 0;
    char *sp = r->subs; int last_sub = -1, last_state = -1;

    /* Erstes Bild mit Rueckruf nach dem SCD-Tick: der Spawn-Zustand VOR dem ersten KI-Tick. */
    frame(nach_scd_spawn); bild++;
    finde_birkin();
    r->spawn_ok = (s_bslot >= 0);
    if (s_bslot >= 0) {
        r->init_im_spawnbild = g5 ? 1 : (s_spawn_state == 1 && s_spawn_sub == f->sub_frost);
    }

    /* (a) VOR DER FREIGABE */
    for (int i = 0; i < vor_n && s_bslot >= 0; i++) {
        re15_actor_t *b = &g_actors[s_bslot];
        int ok_lage = (b->x == s_rx && b->y == s_ry && b->z == s_rz);
        int ok_grid = (b->grid_id == f->grid);
        int ok_zust = g5 ? (b->motion == 0) :
                      (b->state == 1 && b->sub_state_1 == f->sub_frost && b->motion == 0 &&
                       b->anim_frame == (uint16_t)(f->sub_frost == 9 ? 0x10 : 0));
        int ok_spl  = (pl->hp == 100 && !re15_player_is_grabbed());
        r->vor_bilder++;
        if (!(ok_lage && ok_grid && ok_zust && ok_spl)) {
            if (r->vor_verletzt == 0) {
                r->vor_erstes = bild;
                snprintf(r->vor_grund, sizeof r->vor_grund,
                         "Bild %d: pos=(%d,%d,%d) grid=0x%02x st=%d sub=%d clip=%d bild95=%d hp=%d",
                         bild, (int)b->x, (int)b->y, (int)b->z, (unsigned)b->grid_id, b->state,
                         b->sub_state_1, (int)b->motion, (int)b->anim_frame, (int)pl->hp);
            }
            r->vor_verletzt++;
        }
        frame(NULL); bild++;
    }

    /* AUSLOESEN */
    if (f->aot >= 0) {
        pl->x = f->ax; pl->z = f->az;
        re15_aot_fire_slot(f->aot);
    }
    int ausloes_bild = bild;

    /* (b)+(c) */
    int walk_west = (f->aot == -2);
    int frei = !frost;                    /* ohne Frost gilt die Freigabe ab Spawn */
    if (!frost) r->frei_bild = 1;
    uint8_t grid_vorher = (s_bslot >= 0) ? g_actors[s_bslot].grid_id : 0;
    int32_t spieler_x0 = pl->x;
    for (int i = 0; i < nach_n; i++) {
        /* Nachrichten bestaetigen wie ein Spieler (Quadrat = virt. 0x4000, msg_common.c) */
        s_pad = (g_scd.message_active && ((i & 7) == 0)) ? 0x8000 : 0;
        if (walk_west && s_shown != 12 && !frei) {
            int32_t nx = pl->x - 75, nz = pl->z;
            re15_collision_set_band(0);
            re15_collision_constrain(&s_rdt, pl->x, pl->z, &nx, &nz);
            pl->x = nx; pl->z = nz;
        }
        pc_merken();
        frame(NULL); bild++;
        if (s_bslot < 0) { finde_birkin(); continue; }
        re15_actor_t *b = &g_actors[s_bslot];
        if (!frei && !(b->grid_id & 0x20) && (grid_vorher & 0x20)) {
            frei = 1; r->frei_bild = bild; r->frei_grid = b->grid_id;
            r->frei_off = pc_klammer(f->frei_off);
            r->y_frei = b->y;
        }
        if (frei && r->frei_bild >= 0 && bild == r->frei_bild + 200) r->y_spaeter = b->y;
        if (frei && r->folge_ende < 0 &&
            ((!re15_game_flag_get(1, 27) && !re15_game_flag_get(2, 7)) || g_room_change.pending))
            r->folge_ende = bild;          /* Folge vorbei: Sperren weg ODER Tuer ausgeloest (3080 Aot_on) */
        if (frei && b->state == 1 && b->sub_state_1 == f->sub_frost && (f->sub_frost != 9 || b->motion == 0x10))
            r->sub_frost_nach = 1;
        grid_vorher = b->grid_id;
        if (!frei) continue;
        if (g5) {
            if (r->armiert_bild < 0 && b->x == -9000) r->armiert_bild = bild;
        } else {
            if (b->state != last_state || b->sub_state_1 != last_sub) {
                if (sp - r->subs < (int)sizeof r->subs - 12)
                    sp += snprintf(sp, sizeof r->subs - (size_t)(sp - r->subs), "%s%d/%d",
                                   (last_sub < 0) ? "" : ">", b->state, b->sub_state_1);
                last_state = b->state; last_sub = b->sub_state_1;
            }
            if (r->emerg_bild < 0 && b->state == 1 && b->sub_state_1 == 9 && b->motion == 0x10) r->emerg_bild = bild;
            if (r->walk_bild < 0 && b->state == 1 && b->sub_state_1 == 1) r->walk_bild = bild;
        }
        if (s_spur > 0 && (i % s_spur) == 0)
            printf("      spur Bild %5d: Birkin (%d,%d,%d) st=%d sub=%d/%d clip=%d grid=0x%02x | Spieler (%d,%d) hp %d "
                   "mode=%d f1:27=%d f2:07=%d f5:31=%d f5:30=%d msg=%d cam=%d\n",
                   bild, (int)b->x, (int)b->y, (int)b->z, b->state, b->sub_state_1, b->sub_state_2,
                   (int)b->motion, (unsigned)b->grid_id, (int)pl->x, (int)pl->z, (int)pl->hp,
                   (int)g_scd.player_mode, re15_game_flag_get(1, 27), re15_game_flag_get(2, 7),
                   re15_game_flag_get(5, 31), re15_game_flag_get(5, 30), (int)g_scd.message_active, s_shown);
        if (pl->hp < r->hp_min) r->hp_min = pl->hp;
        if (r->angriff_bild < 0 && pl->hp < 100) r->angriff_bild = bild;
        if (r->griff_bild < 0 && re15_player_is_grabbed()) r->griff_bild = bild;
        if (r->angriff_bild >= 0 && r->folge_ende >= 0 && bild > r->angriff_bild + 30) break;
    }
    (void)ausloes_bild; (void)spieler_x0;
}

int main(int argc, char **argv)
{
    int nur = -1, nach_n = 5000;
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "raum=", 5) == 0) nur = (int)strtol(argv[i] + 5, NULL, 16);
        else if (strncmp(argv[i], "n=", 2) == 0) nach_n = atoi(argv[i] + 2);
        else if (strncmp(argv[i], "spur=", 5) == 0) s_spur = atoi(argv[i] + 5);
    }
    printf("=== Birkin-Frost-Schranke: alle 13 Birkin-Spawns (Dossier nachschliff-room5080 Abschnitt 9) ===\n");
    int gelaufen = 0;
    for (int k = 0; k < NFAELLE; k++) {
        const fall_t *f = &FAELLE[k];
        if (nur >= 0 && f->raum != (uint16_t)nur) continue;
        char pfad[256];
        snprintf(pfad, sizeof pfad, "%s/%s", RE15_ASSET_PSX_DIR, f->pfad);
        free(s_raw); s_raw = slurp(pfad, &s_rsz);
        if (!s_raw) { printf("FEHLT: %s\n", pfad); return 77; }
        memset(&s_rdt, 0, sizeof s_rdt);
        if (re15_rdt_parse(s_raw, s_rsz, &s_rdt) != 0) { printf("FEHLT: RDT-Parse %s\n", pfad); return 77; }
        const uint8_t *rec = s_raw + f->spawn_off;
        printf("\n-- ROOM%04X Spawn @0x%05X:", (unsigned)f->raum, (unsigned)f->spawn_off);
        for (int i = 0; i < 20; i++) printf(" %02x", rec[i]);
        printf("\n");
        if (rec[0] != 0x44 || rec[2] != f->typ || rec[3] != f->grid || rec[18] != 0 || rec[19] != 0) {
            printf("FEHLT: Spawn-Record weicht von der Tabelle ab\n"); return 77;
        }
        if (f->frei_off) {
            const uint8_t *m = s_raw + f->frei_off;
            printf("   Freigabe @0x%05X: %02x %02x %02x %02x\n", (unsigned)f->frei_off, m[0], m[1], m[2], m[3]);
            if (m[0] != 0x34 || m[1] != 0x0c || m[2] != f->frei_wert || m[3] != 0) {
                printf("FEHLT: Freigabe-Record weicht von der Tabelle ab\n"); return 77;
            }
        }
        erg_t r; lauf(f, nach_n, &r);
        int g5 = (f->raum & 0xFFFEu) == 0x5090u;
        int frost = (f->grid & 0x20) != 0;
        printf("   Spawn: Slot %d Zustand nach dem SCD-Tick des Spawns st=%d sub=%d +0x95=0x%02x grid=0x%02x\n",
               s_bslot, s_spawn_state, s_spawn_sub, (unsigned)s_spawn_frame95, (unsigned)s_spawn_grid);
        if (frost)
            printf("   (a) %d Bilder vor der Freigabe, %d verletzt%s%s\n", r.vor_bilder, r.vor_verletzt,
                   r.vor_verletzt ? " — erstes: " : "", r.vor_verletzt ? r.vor_grund : "");
        printf("   (b) Freigabe Bild %d, Opcode-Klammer @0x%05X, grid danach 0x%02x\n",
               r.frei_bild, r.frei_off < 0 ? 0 : (unsigned)r.frei_off, (unsigned)r.frei_grid);
        if (g5) printf("   (c) scharf (x=-9000) Bild %d, erster Treffer Bild %d, hp min %d, Griff Bild %d\n",
                       r.armiert_bild, r.angriff_bild, r.hp_min, r.griff_bild);
        else    printf("   (c) Zustaende %s | EMERGENCE Bild %d, WALK Bild %d, erster Treffer Bild %d, hp min %d, Griff Bild %d\n",
                       r.subs, r.emerg_bild, r.walk_bild, r.angriff_bild, r.hp_min, r.griff_bild);

        char tag[64]; snprintf(tag, sizeof tag, "ROOM%04X@0x%05X", (unsigned)f->raum, (unsigned)f->spawn_off);
        PIN(r.spawn_ok, "%s gespawnt", tag);
        PIN(r.init_im_spawnbild, "%s INIT im Spawn-Bild (st=%d sub=%d, erwartet 1/%d) — Sce_em_set @0x8004259c",
            tag, s_spawn_state, s_spawn_sub, f->sub_frost);
        if (frost) {
            PIN(r.vor_bilder > 0 && r.vor_verletzt == 0, "%s (a) steht eingefroren (%d/%d Bilder verletzt)",
                tag, r.vor_verletzt, r.vor_bilder);
            PIN(r.frei_bild >= 0 && r.frei_off == (int)f->frei_off && r.frei_grid == f->frei_wert,
                "%s (b) Freigabe durch Member_set @0x%05X erreicht (Bild %d, grid 0x%02x)", tag,
                (unsigned)f->frei_off, r.frei_bild, (unsigned)r.frei_grid);
        }
        if (g5) {
            PIN(r.armiert_bild >= 0, "%s (c) G5 scharf (x=-9000 @0x801011d0)", tag);
        } else if (f->sub_frost == 9 && f->angriff) {
            PIN(r.emerg_bild >= 0 && r.walk_bild > r.emerg_bild,
                "%s (c) EMERGENCE (Bild %d) vor WALK (Bild %d)", tag, r.emerg_bild, r.walk_bild);
        }
        if (frost && !g5 && f->sub_frost != 0)   /* Sub 0 = grid-4-Szenenmodus, im Port nicht portiert */
            PIN(r.sub_frost_nach, "%s (c) der eingefrorene Sub %d laeuft nach der Freigabe", tag, f->sub_frost);
        if (f->angriff)
            PIN(r.angriff_bild >= 0 || r.griff_bild >= 0, "%s (c) Angriff (hp-Bild %d, Griff-Bild %d)",
                tag, r.angriff_bild, r.griff_bild);
        else
            printf("    (c) Angriff NICHT verlangt: %s — gemessen hp-Bild %d\n", f->ohne, r.angriff_bild);
        if (frost)
            printf("    Folge-Ende nach der Freigabe: %s (Bild %d)%s\n", r.folge_ende >= 0 ? "ja" : "NEIN",
                   r.folge_ende, r.folge_ende >= 0 ? "" : "  <- (1,27)/(2,7) bleiben gesetzt = Spieler gesperrt");
        if (frost && !g5)
            printf("    Hoehe: Freigabe-Bild y=%d, 200 Bilder spaeter y=%d\n", (int)r.y_frei, (int)r.y_spaeter);
        gelaufen++;
    }
    printf("\n  ERGEBNIS: %s (%d Faelle, %d Riegel verletzt)\n", s_fail ? "ROT" : "GRUEN", gelaufen, s_fail);
    return s_fail ? 1 : 0;
}
