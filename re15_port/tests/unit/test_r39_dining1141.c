/*
 * test_r39_dining1141.c — Runde 39 Spur J "dining1141": Riegel ueber die ECHTEN Engine-Wege (scd_room_reenter mit allen
 * Installern, SCD-VM, Spieler-/NPC-Walker, Nachrichten-FSM, re15_game_step); Harness wie test_r38_dining1141.c.
 *
 * Dossier analysis/befunde_runde39/J_dining1141.md, Code include/re15_dining_r39.h + engine/src/dining_r39.c, Programme
 * engine/src/gen/dining_r39_programme.inc (tools/r39_dining/programme_bauen.py).
 *
 * Teile (Aufruf: test_r39_dining1141 <teil>):
 *   texte      Port-Texte 50/51/52 dekodiert = Wortlaut des Nutzers (Punkte 5 und 8)
 *   programme  Programm-Bytes je Punkt: HAEMMERN39 ohne Zombie-Stoehnen 0x3C/0x0B und ohne "Help me out." 0x3D (1, 3),
 *              Klopfen dreifach + help01..05 je Runde im Mix (4), Reaktion msg 50 unter Ck(9,94) (5); BEFREIUNG39 mit
 *              Tuer-Auftritt (2), Glasbruch vor msg 6 (6), Gosub sub04 nach Johns Drehung vor msg 9 (7), msg 51/52 mit
 *              Gesten nach dem Kampf (8); Lagen/Saetze = Kopf-Konstanten
 *   stamm      Sprach-Stamm: ROOM1141 Id 53..57 -> help01..05, alles andere unveraendert
 *   phase_h    ROOM1141 mit Elza am VM: John ab Bild 0 nicht im Raum; Klopfen/Hilferufe als Ton (Takt), Reaktion msg 50
 *              genau einmal; Gemaelde -> Rolladen, Elza in Cut 7 von vorn, Tuer auf, John innen vor der Tuer, Tuer zu,
 *              Cut 6 mit John (erstes Erscheinen nur am Kameraschnitt); Szene mit Glasbruch vor msg 6 und Johns Blick zu
 *              Elza ab msg 9; Sperre msg 41; Gorillas tot -> Cut 8, msg 51 (Gesten 15/23) und msg 52 (Heranwinken
 *              Sub 2 Clip 6), Johns Abgang, Riegel offen
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
#include "re15_fade.h"
#include "re15_inventory.h"
#include "re15_item_modal.h"
#include "re15_item_discard.h"
#include "re15_gameflow.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_tim.h"
#include "re15_dining_r38.h"
#include "re15_dining_r39.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

void re15_actor_step_all_walkers(void);
int  re15_ai_arc_test(const re15_actor_t *e, int32_t px, int32_t pz, int cone);   /* re15_damage.c */

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

/* ---- Harness (wie test_r38_dining1141.c) ------------------------------------------------------------------ */
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;
static long               s_bild = 0;

typedef struct { long bild; uint8_t bank, satz; } se_t;
static se_t s_se[8192];
static int  s_n_se = 0;

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

static int rdt_laden(uint16_t room)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%X/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    g_room_rdt = s_rdt;
    return 0;
}

static void spielbild(uint16_t held, uint16_t edge);
static void frame(uint16_t held, uint16_t edge)
{
    if (!re15_discard_frozen()) spielbild(held, edge);
    else s_bild++;
    if (re15_discard_active()) re15_discard_tick((edge & RE15_PAD_BIT_SQUARE) ? (uint16_t)0x4000u : 0, 0);
}

static uint8_t *s_ems = NULL;
static int      s_ems_sz = 0;
static uint8_t  s_bank[0x60000];
static void bank_john(void)   /* John (Typ 0x49) aus CDEMD1.EMS (Elza-Raeume; Muster test_r38_dining1141.c) */
{
    if (!s_ems) {
        char p[600]; snprintf(p, sizeof p, "%s/EMD/CDEMD1.EMS", RE15_ASSET_PSX_DIR);
        size_t n = 0; s_ems = slurp(p, &n); s_ems_sz = (int)n;
    }
    if (!s_ems) { printf("  FEHLER: CDEMD1.EMS fehlt\n"); g_fail++; return; }
    int idx = re15_ems_index_for_type(0x49);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_ems, (size_t)s_ems_sz, idx, &off, &len) != 0 || len > sizeof s_bank) {
        printf("  FEHLER: Bank 0x49 nicht im Archiv\n"); g_fail++; return; }
    re15_enemy_bank_t *eb = re15_enemy_alloc(0x49);
    if (!eb) return;
    memcpy(s_bank, s_ems + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_bank, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return;
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok   = (re15_emd_parse_loco_bank(s_bank, len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(s_bank, len, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->own_ok    = (re15_emd_parse_own_bank(s_bank, len, &eb->skel_own, &eb->anim_own) == 0);
}

static void spielbild(uint16_t held, uint16_t edge)
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
    while (scd_audio_queue_pop(&e)) {
        if (e.kind == SCD_AUDIO_SE_ON && s_n_se < (int)(sizeof s_se / sizeof s_se[0])) {
            s_se[s_n_se].bild = s_bild; s_se[s_n_se].bank = e.bank; s_se[s_n_se].satz = e.sample_id; s_n_se++;
            (void)re15_dining_r38_se_on(e.bank, e.sample_id);   /* Zaehler wie der Se_on-Zweig von audio_pc.c */
        }
    }
    s_bild++;
}

static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, uint8_t cut)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    if (room == RE15_R38F_RAUM_1141) bank_john();
    re15_discard_reset();
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    g_room_rdt_ok = 1;
    g_gameflow.character = (room & 0xFu) ? 4 : 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = cut; s_cine_was = 0; s_bild = 0; s_n_se = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

typedef struct { int msg_id, raumwechsel, ziel; } druck_t;
static druck_t druecke(int32_t x, int32_t z, int16_t rot, int bilder)
{
    druck_t r = { -1, 0, -1 };
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = x; pl->z = z; pl->rot_y = rot;
    frame(0, 0);
    g_room_change.pending = 0;
    for (int f = 0; f < bilder; f++) {
        uint16_t e = 0, h = 0;
        if (f == 0) e = RE15_PAD_BIT_SQUARE;
        if (f < 12) h = RE15_PAD_BIT_SQUARE;
        if (f >= 40 && (f % 15) == 0 && g_scd.message_active) e = h = RE15_PAD_BIT_SQUARE;
        frame(h, e);
        if (g_scd.message_active && r.msg_id < 0) r.msg_id = (int)g_scd.message_id;
        if (g_room_change.pending) { r.raumwechsel = 1; r.ziel = (int)g_room_change.room_id; break; }
    }
    return r;
}

static void projiziere(const re15_camera_view_t *v, int vx, int vy, int vz, int *sx, int *sy, int *z)
{
    const int32_t *r = v->rot, *t = v->trans;
    int32_t x = (int32_t)(((int64_t)vx*r[0] + (int64_t)vy*r[1] + (int64_t)vz*r[2]) >> 12) + t[0];
    int32_t y = (int32_t)(((int64_t)vx*r[3] + (int64_t)vy*r[4] + (int64_t)vz*r[5]) >> 12) + t[1];
    int32_t zz = (int32_t)(((int64_t)vx*r[6] + (int64_t)vy*r[7] + (int64_t)vz*r[8]) >> 12) + t[2];
    *z = zz;
    if (zz <= 0) { *sx = -9999; *sy = -9999; return; }
    *sx = 160 + (int)((int64_t)x * v->fov_screen_dist / zz);
    *sy = 120 + (int)((int64_t)y * v->fov_screen_dist / zz);
}

/* Sichtbar im Bild des Cuts: Fuss, Huefte oder Kopf projiziert ins 320x240-Bild. */
static int sichtbar(int cut, const re15_actor_t *a)
{
    if (cut < 0 || cut >= (int)s_rdt.cut_count) return 0;
    re15_camera_view_t v;
    if (re15_camera_build_view(&s_rdt.cuts[cut], &v) != 0) return 0;
    static const int hoehe[3] = { 0, -900, -1750 };
    for (int i = 0; i < 3; i++) {
        int sx, sy, z;
        projiziere(&v, a->x, a->y + hoehe[i], a->z, &sx, &sy, &z);
        if (z > 0 && sx >= 0 && sx < 320 && sy >= 0 && sy < 240) return 1;
    }
    return 0;
}

/* ---- Teil: texte ------------------------------------------------------------------------------------------ */
static void teil_texte(void)
{
    printf("== texte ==\n");
    static const char *const soll[3] = {
        "Elza: Oh, no, this is John! He is in Trouble, I have to help him!",
        "John: Puuuh, that was close! I thought we end up as Gorilla meal....",
        "John: Elza, follow me, I think I might have found a way out of here!",
    };
    for (int i = 0; i < 3; i++) {
        int len = 0; char t[512];
        const uint8_t *b = re15_dining_r39_text(i, &len);
        t[0] = 0;
        if (b && len > 0) re15_msg_decode_text(b, (size_t)len, t, sizeof t);
        printf("    Text %d: \"%s\"\n", 50 + i, t);
        PRUEF(!strcmp(t, soll[i]), "msg %d = \"%s\"", 50 + i, soll[i]);
        PRUEF(b && len > 8 && b[0] == 0x04 && b[1] == 0x00 && b[2] == 0x05 && b[3] == (i == 0 ? 0x01 : 0x07) &&
              b[len - 4] == 0x04 && b[len - 3] == 0x01 && b[len - 2] == 0x01 && b[len - 1] == 0x63,
              "msg %d in Dialogform ROOM1141 msg 2 @0x0115F (Sprecherfarbe %d, Ende `04 01 01 63`)", 50 + i, i == 0 ? 1 : 7);
    }
}

/* ---- Teil: programme -------------------------------------------------------------------------------------- */
typedef struct { int pc; uint8_t op; } zeile_t;
static int zerlege(const uint8_t *p, int n, zeile_t *z, int max)
{
    int k = 0;
    for (int pc = 0; pc < n && k < max; ) {
        int sz = scd_opcode_size_at(p + pc);
        if (sz <= 0) { printf("    Opcode 0x%02X @+%04X ohne Laenge\n", p[pc], pc); return -1; }
        z[k].pc = pc; z[k].op = p[pc]; k++;
        pc += sz;
    }
    return k;
}

static int finde(const uint8_t *p, int n, const uint8_t *m, int mn, int ab)
{
    for (int i = ab; i + mn <= n; i++) if (!memcmp(p + i, m, (size_t)mn)) return i;
    return -1;
}

static void teil_programme(void)
{
    printf("== programme ==\n");
    int nh = 0, nb = 0;
    const uint8_t *h = re15_dining_r39_programm(0, &nh);
    const uint8_t *b = re15_dining_r39_programm(1, &nb);
    static zeile_t zh[1024], zb[2048];
    const int kh = h ? zerlege(h, nh, zh, 1024) : -1;
    const int kb = b ? zerlege(b, nb, zb, 2048) : -1;
    PRUEF(kh > 0 && kb > 0, "HAEMMERN39 %d B / BEFREIUNG39 %d B opcode-genau zerlegbar", nh, nb);
    if (kh <= 0 || kb <= 0) return;
    /* Punkte 1/3: kein Se_on der Runde-38-Baenke 0x3C (Krachen/Stoehnen RE2 1060) / 0x3D ("Help me out." main42) */
    int alt = 0, klopf = 0, hilfe[6] = { 0 }, reihe[64], nreihe = 0, takt_ok = 1, letzter_klopf = -1;
    for (int i = 0; i < kh; i++) {
        if (zh[i].op != 0x36) continue;
        const uint8_t bank = h[zh[i].pc + 1], satz = h[zh[i].pc + 2];
        if (bank == 0x3C || bank == 0x3D) alt++;
        if (bank == RE15_R39J_SE_BANK && satz == RE15_R39J_SATZ_KLOPFEN) {
            /* zwischen zwei Schlaegen einer Gruppe genau `09 0a 05 00` (RE2 ROOM1090 @0x02BBC) */
            if (letzter_klopf >= 0 && i - letzter_klopf <= 4) {
                int sl = -1;
                for (int j = letzter_klopf + 1; j < i; j++) if (zh[j].op == 0x09) sl = j;
                if (sl < 0 || memcmp(h + zh[sl].pc, "\x09\x0a\x05\x00", 4)) takt_ok = 0;
            }
            letzter_klopf = i; klopf++;
        }
        if (bank == RE15_R39J_SE_BANK && satz >= RE15_R39J_SATZ_HILFE0 && satz < RE15_R39J_SATZ_HILFE0 + 5) {
            hilfe[satz - RE15_R39J_SATZ_HILFE0 + 1]++;
            if (nreihe < 64) reihe[nreihe++] = satz - RE15_R39J_SATZ_HILFE0 + 1;
        }
    }
    PRUEF(alt == 0, "HAEMMERN39: kein Se_on der Baenke 0x3C (Zombie-Stoehnen/Krachen) / 0x3D (\"Help me out.\") (Punkte 1, 3)");
    PRUEF(klopf == 15 && takt_ok, "HAEMMERN39: Klopfen 5 Runden x 3 Schlaege, Abstand Sleep 5 (Punkt 4): %d", klopf);
    PRUEF(hilfe[1] == 1 && hilfe[2] == 1 && hilfe[3] == 1 && hilfe[4] == 1 && hilfe[5] == 1,
          "HAEMMERN39: help01..05 je einmal je Durchlauf (Punkt 4)");
    {
        int gemischt = 0;
        for (int i = 1; i < nreihe; i++) if (reihe[i] != reihe[i - 1] + 1) gemischt = 1;
        PRUEF(nreihe == 5 && gemischt, "HAEMMERN39: Reihenfolge im Mix %d %d %d %d %d (nicht 1..5)", reihe[0], reihe[1], reihe[2],
              reihe[3], reihe[4]);
    }
    {   /* Punkt 5: Ifel Ck(9,94)==0 { ... Message_on 50 ... Set(9,94)=1 } genau einmal */
        const uint8_t ck[4] = { 0x21, 0x09, RE15_R39J_BIT_REAKTION, 0x00 }, set[4] = { 0x22, 0x09, RE15_R39J_BIT_REAKTION, 0x01 };
        const uint8_t m50[4] = { 0x2B, RE15_R39J_MSG_REAKTION, 0x00, 0x00 };
        const int a = finde(h, nh, ck, 4, 0), c = finde(h, nh, m50, 4, 0), d = finde(h, nh, set, 4, 0);
        PRUEF(a > 0 && c > a && d > c && finde(h, nh, m50, 4, c + 1) < 0, "HAEMMERN39: Ck(9,94)==0 -> msg 50 -> Set(9,94)=1, einmal");
        /* die Reaktion folgt dem ersten Hilferuf */
        const uint8_t h1[3] = { 0x36, RE15_R39J_SE_BANK, (uint8_t)(RE15_R39J_SATZ_HILFE0 + reihe[0] - 1) };
        const int erster = finde(h, nh, h1, 3, 0);
        PRUEF(erster > 0 && erster < a, "HAEMMERN39: Reaktion nach dem ersten Hilferuf (help%02d)", reihe[0]);
    }
    /* BEFREIUNG39 */
    {   /* Punkt 2: Cut 7 -> Tuer auf (0x0E,0) -> Pos_set John innen vor die Tuer + Dir_set -> Tuer zu (0x0E,1) -> Cut 6 */
        const int16_t tx = RE15_R39J_JOHN_TUER_X, tz = RE15_R39J_JOHN_TUER_Z;
        const uint8_t c7[2] = { 0x29, RE15_R39J_CUT_ELZA }, auf[3] = { 0x36, 0x0E, 0x00 }, zu[3] = { 0x36, 0x0E, 0x01 };
        const uint8_t pos[8] = { 0x32, 0x00, (uint8_t)tx, (uint8_t)((uint16_t)tx >> 8), 0, 0, (uint8_t)tz, (uint8_t)((uint16_t)tz >> 8) };
        const uint8_t dir[8] = { 0x33, 0x00, 0x00, 0x00, (uint8_t)RE15_R39J_JOHN_TUER_YAW, (uint8_t)(RE15_R39J_JOHN_TUER_YAW >> 8), 0, 0 };
        const uint8_t c6[2] = { 0x29, RE15_R39J_CUT_TUER };
        const int i7 = finde(b, nb, c7, 2, 0x0C8), ia = finde(b, nb, auf, 3, i7 > 0 ? i7 : 0);
        const int ip = finde(b, nb, pos, 8, ia > 0 ? ia : 0), id = finde(b, nb, dir, 8, ip > 0 ? ip : 0);
        const int iz = finde(b, nb, zu, 3, id > 0 ? id : 0), i6 = finde(b, nb, c6, 2, iz > 0 ? iz : 0);
        PRUEF(i7 > 0 && ia > i7 && ip > ia && id > ip && iz > id && i6 > iz,
              "BEFREIUNG39 Punkt 2: Cut 7 @+%04X, Tuer auf @+%04X, John Pos_set (%d,%d) @+%04X + Dir_set %d, Tuer zu @+%04X, Cut 6 @+%04X",
              i7, ia, tx, tz, ip, RE15_R39J_JOHN_TUER_YAW, iz, i6);
        const int16_t ex = RE15_R39J_ELZA_VORNE_X, ez = RE15_R39J_ELZA_VORNE_Z;
        const uint8_t walk[8] = { 0x40, 0x00, 0x04, 0x00, (uint8_t)ex, (uint8_t)((uint16_t)ex >> 8), (uint8_t)ez, (uint8_t)((uint16_t)ez >> 8) };
        const int iw = finde(b, nb, walk, 8, 0);
        PRUEF(iw > 0 && iw < i7, "BEFREIUNG39 Punkt 2: Elza WALK (%d,%d) vor Cut 7 (Bild von vorn)", ex, ez);
    }
    {   /* Punkt 6: zwei Se_on(0x3E,0x21) mit Sleep 5 dazwischen unmittelbar vor `09 0a 14 00` + Message_on 6 */
        const uint8_t g[24] = { 0x36, RE15_R39J_SE_BANK, RE15_R39J_SATZ_GLAS, 0,0,0,0,0,0,0,0,0, 0x09, 0x0A, 0x05, 0x00,
                                0x36, RE15_R39J_SE_BANK, RE15_R39J_SATZ_GLAS, 0,0,0,0,0 };
        const uint8_t m6[8] = { 0x09, 0x0A, 0x14, 0x00, 0x2B, 0x06, 0x00, 0x00 };
        const int ig = finde(b, nb, g, 24, 0), i6 = finde(b, nb, m6, 8, 0);
        PRUEF(ig > 0 && i6 == ig + 28, "BEFREIUNG39 Punkt 6: Glasbruch x2 (RE2 ROOM1090 @0x02BB0..@0x02BC0) direkt vor Sleep 20 + "
                                       "msg 6 \"What was that?!\" (@+%04X, msg 6 @+%04X)", ig, i6 + 4);
    }
    {   /* Punkt 7: Johns Plc_dest Modus 9 zu Elzas Platz (Bit 1), unmittelbar danach Gosub sub04, vor msg 9 */
        const int16_t ex = RE15_R38F_ELZA_PLATZ_X, ez = RE15_R38F_ELZA_PLATZ_Z;
        const uint8_t t[10] = { 0x40, 0x00, 0x09, 0x01, (uint8_t)ex, (uint8_t)((uint16_t)ex >> 8), (uint8_t)ez,
                                (uint8_t)((uint16_t)ez >> 8), 0x18, 0x04 };
        const uint8_t m9[4] = { 0x2B, 0x09, 0x00, 0x00 };
        int it = -1, k = 0;
        for (int i = finde(b, nb, t, 10, 0); i >= 0; i = finde(b, nb, t, 10, i + 1)) { it = i; k++; }
        const int i9 = finde(b, nb, m9, 4, 0);
        PRUEF(k == 2 && it > 0 && i9 > it, "BEFREIUNG39 Punkt 7: John TURN -> Elza + Gosub sub04 auch vor msg 9 (%d Stellen, "
                                           "letzte @+%04X, msg 9 @+%04X)", k, it, i9);
    }
    {   /* Punkt 8: nach der Warteschleife (4,0xF2): Cut 8, msg 51 + Gesten (0,15)/(0,23), Plc_motion(2,6), msg 52 */
        const uint8_t w[4] = { 0x21, 0x04, 0xF2, 0x01 }, c8[2] = { 0x29, RE15_R39J_CUT_GESPRAECH };
        const uint8_t m51[4] = { 0x2B, RE15_R39J_MSG_PUH, 0, 0 }, m52[4] = { 0x2B, RE15_R39J_MSG_FOLGEN, 0, 0 };
        const uint8_t g15[4] = { 0x3F, 0x00, 0x0F, 0x00 }, g23[4] = { 0x3F, 0x00, 0x17, 0x00 }, wink[4] = { 0x3F, 0x02, 0x06, 0x00 };
        const int iw = finde(b, nb, w, 4, 0), i8 = finde(b, nb, c8, 2, iw > 0 ? iw : 0);
        const int i51 = finde(b, nb, m51, 4, i8 > 0 ? i8 : 0), i15 = finde(b, nb, g15, 4, i51 > 0 ? i51 : 0);
        const int i23 = finde(b, nb, g23, 4, i15 > 0 ? i15 : 0), iwk = finde(b, nb, wink, 4, i23 > 0 ? i23 : 0);
        const int i52 = finde(b, nb, m52, 4, iwk > 0 ? iwk : 0);
        PRUEF(iw > 0 && i8 > iw && i51 > i8 && i15 > i51 && i23 > i15 && iwk > i23 && i52 > iwk,
              "BEFREIUNG39 Punkt 8: Gorillas tot -> Cut 8 -> msg 51 + Gesten 15/23 -> Heranwinken (2,6) -> msg 52");
        const uint8_t run[4] = { 0x40, 0x00, 0x05, 0x01 };
        const int ir = finde(b, nb, run, 4, i52 > 0 ? i52 : 0);
        PRUEF(ir > i52, "BEFREIUNG39 Punkt 8: Johns Abgang (RUN) erst NACH msg 52");
    }
}

/* ---- Teil: stamm ------------------------------------------------------------------------------------------ */
static void teil_stamm(void)
{
    printf("== stamm ==\n");
    for (int i = 0; i < 5; i++) {
        char s[24]; snprintf(s, sizeof s, "main%02d", RE15_R39J_HILFE_ID0 + i);
        re15_dining_r39_stamm(0x1141u, RE15_R39J_HILFE_ID0 + i, s, sizeof s);
        char soll[16]; snprintf(soll, sizeof soll, "help%02d", i + 1);
        PRUEF(!strcmp(s, soll), "ROOM1141 Id %d -> %s.wav", RE15_R39J_HILFE_ID0 + i, s);
    }
    char s[24];
    snprintf(s, sizeof s, "main%02d", 50); re15_dining_r39_stamm(0x1141u, 50, s, sizeof s);
    PRUEF(!strcmp(s, "main50"), "ROOM1141 Id 50 bleibt main50 (Elzas Zeile, Aufnahme des Nutzers moeglich)");
    snprintf(s, sizeof s, "main%02d", 53); re15_dining_r39_stamm(0x1140u, 53, s, sizeof s);
    PRUEF(!strcmp(s, "main53"), "ROOM1140 Id 53 bleibt main53 (nur ROOM1141)");
}

/* ---- Teil: phase_h ---------------------------------------------------------------------------------------- */
static int ausgerichtet(const re15_actor_t *a, const re15_actor_t *ziel, int kegel)
{
    return re15_ai_arc_test(a, ziel->x, ziel->z, kegel) == 0;
}

static void teil_phase_h(void)
{
    printf("== phase_h ==\n");
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_game_flag_set(RE15_R38F_F0_BANK, RE15_R38F_F0_BIT, 1);     /* (4,0xF0): ROOM1021 sub02 @0x01FE0 */
    if (room_boot(RE15_R38F_RAUM_1141, -7600, -17600, 0, 0) != 0) return;
    re15_actor_t *john = &g_actors[1];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    PRUEF(re15_dining_r38_phase_h() == 1, "Phase H erkannt ((4,0xF0)=1, (3,0x6A)=0, Anker bytegleich)");
    PRUEF(john->active && john->type == RE15_R38F_JOHN_TYP && john->x == RE15_R39J_PARK_X && john->z == RE15_R39J_PARK_Z,
          "Bild 0: John (Typ 0x%02X) geparkt (%d,%d) = auf dem Dach, nicht im Dachgang (Punkt 2)", john->type, john->x, john->z);
    /* 900 Bilder Phase H: Ton-Takt, Reaktion einmal */
    int msg50 = 0, msg_sonst = 0, last = -1, pad_gesperrt_bei_50 = 0;
    long bild50 = -1;
    for (int f = 0; f < 900; f++) {
        frame(0, 0);
        if (g_scd.message_active && (int)g_scd.message_id != last) {
            last = (int)g_scd.message_id;
            if (last == RE15_R39J_MSG_REAKTION) { msg50++; bild50 = s_bild; pad_gesperrt_bei_50 = re15_game_flag_get(2, 7); }
            else msg_sonst++;
        }
        if (!g_scd.message_active) last = -1;
    }
    PRUEF(msg50 == 1 && msg_sonst == 0, "900 Bilder: Elzas Reaktion msg 50 genau einmal (Bild %ld), sonst keine Nachricht", bild50);
    PRUEF(pad_gesperrt_bei_50, "Reaktion mit Pad-Sperre (2,7) (Form sub02 @0x00C26)");
    PRUEF(re15_game_flag_get(9, RE15_R39J_BIT_REAKTION) == 1, "(9,94) = 1 nach der Reaktion");
    PRUEF(john->x == RE15_R39J_PARK_X && john->z == RE15_R39J_PARK_Z, "John bleibt auf dem Dach (geparkt)");
    {
        int alt = 0, n_klopf = 0, n_hilfe = 0, erster_hilfe = -1, gruppen_ok = 1, hilfe_reihe[16], nh = 0;
        long vorher = -1000;
        for (int i = 0; i < s_n_se; i++) {
            if (s_se[i].bank == 0x3C || s_se[i].bank == 0x3D) alt++;
            if (s_se[i].bank != RE15_R39J_SE_BANK) continue;
            if (s_se[i].satz == RE15_R39J_SATZ_KLOPFEN) {
                const long d = s_se[i].bild - vorher;
                if (d < 5 && vorher >= 0) gruppen_ok = 0;
                vorher = s_se[i].bild; n_klopf++;
            } else if (s_se[i].satz >= RE15_R39J_SATZ_HILFE0 && s_se[i].satz < RE15_R39J_SATZ_HILFE0 + 5) {
                if (erster_hilfe < 0) erster_hilfe = (int)s_se[i].bild;
                if (nh < 16) hilfe_reihe[nh++] = s_se[i].satz - RE15_R39J_SATZ_HILFE0 + 1;
                n_hilfe++;
            }
            printf("    Bild %4ld Se_on(0x%02X,0x%02X)\n", s_se[i].bild, s_se[i].bank, s_se[i].satz);
        }
        PRUEF(alt == 0, "kein Se_on 0x3C/0x3D (Zombie-Stoehnen, \"Help me out.\") in Phase H (Punkte 1, 3)");
        PRUEF(n_klopf >= 6 && gruppen_ok, "Klopfen (DOOR0F) in Gruppen, %d Schlaege", n_klopf);
        PRUEF(n_hilfe >= 2 && nh >= 2 && hilfe_reihe[0] != 1, "Hilferufe help%02d, help%02d ... (%d, im Mix)", hilfe_reihe[0],
              nh > 1 ? hilfe_reihe[1] : 0, n_hilfe);
        PRUEF(erster_hilfe > 0 && bild50 > erster_hilfe, "Reaktion (Bild %ld) nach dem ersten Hilferuf (Bild %d)", bild50, erster_hilfe);
    }
    /* Gemaelde: Vorwaerts-Punkt in Aot slot 3 (x -9000..-8000, z -11000..-9500), Blick -X; Quadrat, Antwort "ja" */
    pl->x = -8300; pl->z = -10250; pl->rot_y = 2048;
    frame(0, 0);
    const int se_vorher = s_n_se;
    long ja_bild = -1;
    for (int f = 0; f < 600 && ja_bild < 0; f++) {
        uint16_t e = (f == 0 || (g_scd.message_active && (f % 20) == 0)) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
        if (re15_game_flag_get(RE15_R38F_6A_BANK, RE15_R38F_6A_BIT)) ja_bild = s_bild;
    }
    PRUEF(ja_bild >= 0, "Gemaelde: \"ja\" -> (3,0x6A) = 1 in Bild %ld", ja_bild);
    /* Tuer-Auftritt und Szene bis zur Rueckgabe der Steuerung */
    int msgs[64], n_msg = 0, pm2 = 0;
    long frei_bild = -1, bild_auf = -1, bild_zu = -1, bild_cut6 = -1, bild_cut7 = -1, bild_john_da = -1, bild_msg6 = -1;
    long bild_glas2 = -1;
    int cut_beim_auftauchen = -1, john_sichtbar_vor_cut6 = 0, elza_in_cut7 = 0, john_in_cut6 = 0;
    int blick9 = -1, blick_fehl = 0, blick_n = 0;
    last = -1;
    for (int f = 0; f < 6000; f++) {
        frame(0, 0);
        if (g_scd.message_active && (int)g_scd.message_id != last) {
            last = (int)g_scd.message_id;
            if (n_msg < 64) msgs[n_msg++] = last;
            if (last == 6) bild_msg6 = s_bild;
            if (last == 9) blick9 = ausgerichtet(john, pl, 128);
        }
        if (!g_scd.message_active) last = -1;
        /* Blick Johns waehrend msg 9..11 (Punkt 7) */
        if (g_scd.message_active && g_scd.message_id >= 9 && g_scd.message_id <= 11) { blick_n++; if (!ausgerichtet(john, pl, 160)) blick_fehl++; }
        if (bild_john_da < 0 && john->x > -29000) { bild_john_da = s_bild; cut_beim_auftauchen = s_shown; }
        if (bild_cut7 < 0 && s_shown == RE15_R39J_CUT_ELZA) { bild_cut7 = s_bild; elza_in_cut7 = sichtbar(RE15_R39J_CUT_ELZA, pl); }
        if (bild_cut6 < 0 && s_shown == RE15_R39J_CUT_TUER && bild_john_da > 0) { bild_cut6 = s_bild; john_in_cut6 = sichtbar(RE15_R39J_CUT_TUER, john); }
        if (bild_cut6 < 0 && john->x > -29000 && sichtbar(s_shown, john)) john_sichtbar_vor_cut6++;
        if (g_scd.player_mode == 2) pm2 = 1;
        if (pm2 && g_scd.player_mode == 0 && !g_scd.message_active && frei_bild < 0 && f > 400) { frei_bild = s_bild; break; }
    }
    for (int i = se_vorher; i < s_n_se; i++) {
        if (s_se[i].bank == 0x0E && s_se[i].satz == 0 && bild_auf < 0) bild_auf = s_se[i].bild;
        if (s_se[i].bank == 0x0E && s_se[i].satz == 1 && bild_zu < 0) bild_zu = s_se[i].bild;
        if (s_se[i].bank == RE15_R39J_SE_BANK && s_se[i].satz == RE15_R39J_SATZ_GLAS) bild_glas2 = s_se[i].bild;
    }
    printf("    Nachrichten:"); for (int i = 0; i < n_msg; i++) printf(" %d", msgs[i]); printf("\n");
    printf("    Cut 7 ab Bild %ld, Tuer auf %ld, John da %ld (angezeigter Cut %d), Tuer zu %ld, Cut 6 ab %ld\n",
           bild_cut7, bild_auf, bild_john_da, cut_beim_auftauchen, bild_zu, bild_cut6);
    PRUEF(bild_cut7 > ja_bild && elza_in_cut7, "Punkt 2: Kamera vorne auf Elza (Cut 7, Elza im Bild) ab Bild %ld", bild_cut7);
    PRUEF(bild_auf >= bild_cut7 && bild_john_da >= bild_auf && cut_beim_auftauchen == RE15_R39J_CUT_ELZA,
          "Punkt 2: waehrend die Tuer aufgeht (Bild %ld) kommt John herein, Kamera zeigt Cut %d (Elza)", bild_auf, cut_beim_auftauchen);
    PRUEF(bild_zu > bild_auf && bild_cut6 > bild_zu, "Punkt 2: Tuer-zu-Ton (Bild %ld), DANACH Kamera auf die Tuer (Cut 6, Bild %ld)",
          bild_zu, bild_cut6);
    PRUEF(john_in_cut6, "Punkt 2: John steht im ersten Bild von Cut 6 schon im Bild (innen vor der Dachtuer)");
    PRUEF(john_sichtbar_vor_cut6 == 0, "Punkt 2: John vor Cut 6 in keinem angezeigten Bild (%d Bilder)", john_sichtbar_vor_cut6);
    PRUEF(bild_glas2 > 0 && bild_msg6 > bild_glas2, "Punkt 6: Glasbruch (letzter Satz Bild %ld) vor \"What was that?!\" (Bild %ld)",
          bild_glas2, bild_msg6);
    PRUEF(blick9 == 1, "Punkt 7: John blickt beim Oeffnen von msg 9 zu Elza (Kegel 128, re15_ai_arc_test)");
    PRUEF(blick_n > 0 && blick_fehl == 0, "Punkt 7: John blickt waehrend msg 9..11 zu Elza (%d von %d Bildern daneben, Kegel 160)",
          blick_fehl, blick_n);
    PRUEF(frei_bild > 0, "Steuerung nach der Szene zurueck (Bild %ld)", frei_bild);
    /* Kampf: Gorillas einfrieren, John in die Ecke, Sperre */
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == RE15_R38F_GORILLA_TYP) g_actors[s].grid_id |= 0x20;
    for (int f = 0; f < 900 && !(abs(john->x - RE15_R39J_ECKE_X) < 400 && abs(john->z - RE15_R39J_ECKE_Z) < 400); f++) frame(0, 0);
    PRUEF(abs(john->x - RE15_R39J_ECKE_X) < 400 && abs(john->z - RE15_R39J_ECKE_Z) < 400, "John in der Ecke (%d,%d)", john->x, john->z);
    PRUEF(g_aot.slots[0].active && g_aot.slots[0].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[0].event_id == RE15_R38F_MSG_1141_SPERRE,
          "Sperre: Doppeltuer zeigt msg 41 solange die Gorillas leben (Runde 38)");
    /* Gorillas tot (Kill-Flags pc[7] 0xCB/0xCC, Bank 7 — wie RE15_R38F_GORILLAS_TOT, KEINE Schusszahl) */
    pl->x = -3000; pl->z = -9000; pl->rot_y = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == RE15_R38F_GORILLA_TYP) re15_actor_free(s);
    re15_game_flag_set(7, 0xCB, 1);
    re15_game_flag_set(7, 0xCC, 1);
    int m51 = -1, m52 = -1, geste15 = 0, geste23 = 0, wink = 0, cut8 = 0, blick51 = -1, park = -1, k = 0;
    long b51 = -1, b52 = -1, b_run = -1;
    last = -1;
    for (int f = 0; f < 4000; f++) {
        frame(0, 0);
        if (s_shown == RE15_R39J_CUT_GESPRAECH) cut8 = 1;
        if (g_scd.message_active && (int)g_scd.message_id != last) {
            last = (int)g_scd.message_id;
            if (last == RE15_R39J_MSG_PUH) { m51 = k++; b51 = s_bild; blick51 = ausgerichtet(john, pl, 128); }
            if (last == RE15_R39J_MSG_FOLGEN) { m52 = k++; b52 = s_bild; }
        }
        if (!g_scd.message_active) last = -1;
        if (b51 >= 0 && b52 < 0 && john->sub_state_1 == 0 && john->motion == 15) geste15 = 1;
        if (b51 >= 0 && b52 < 0 && john->sub_state_1 == 0 && john->motion == 23) geste23 = 1;
        if (b51 >= 0 && john->sub_state_1 == 2 && john->motion == 6) wink = 1;
        if (b52 >= 0 && b_run < 0 && john->state == 4 && john->sub_state_1 == 5) b_run = s_bild;
        if (john->x <= -29000 && park < 0 && b52 >= 0) { park = (int)s_bild; break; }
    }
    PRUEF(cut8, "Punkt 8: Kamera Cut 8 (Suedwest-Ecke) nach dem Tod der Gorillas");
    PRUEF(abs(pl->x - RE15_R39J_ELZA_NACH_X) < 200 && abs(pl->z - RE15_R39J_ELZA_NACH_Z) < 200, "Punkt 8: Elza bei (%d,%d)", pl->x, pl->z);
    PRUEF(m51 == 0 && m52 == 1 && b52 > b51, "Punkt 8: msg 51 (Bild %ld) vor msg 52 (Bild %ld)", b51, b52);
    PRUEF(blick51 == 1, "Punkt 8: John blickt bei msg 51 zu Elza");
    PRUEF(geste15 && geste23, "Punkt 8: zu msg 51 Johns Gesten Clip 15 und 23 (Form sub02 @0x00CB8/@0x00CCC)");
    PRUEF(wink, "Punkt 8: zu msg 52 Heranwinken Sub 2 Clip 6 (Form sub02 @0x00C78 / ROOM1171 @0x014C2)");
    PRUEF(b_run > b52, "Punkt 8: John laeuft erst nach msg 52 los (Bild %ld)", b_run);
    PRUEF(park > 0 && re15_game_flag_get(9, RE15_R38F_BIT_JOHN_WEG) == 1 && re15_game_flag_get(4, 0xC3) == 1,
          "Abgang: John fort (Bild %d), (9,67) = 1, Riegel (4,0xC3) = 1", park);
}

/* ---- Teil: pegel (Werkzeug des Riegels integration_r39_dining1141_d, Punkt 9) --------------------------------------
 * Aufruf: test_r39_dining1141 pegel <laufordner>. Liest <lauf>/debug.log: nach "loaded room1171.rdt" das erste
 * "[bgm] Sce_bgm_control slot=0 op=1 ... capTick=N" (= ROOM1171 main00 @0x011D6). Aus <lauf>/cap.raw (RE15_AUDIO_CAP_SYNC,
 * 16 bit Stereo, 1470 Abtastwerte je Takt) der Pegel in den Takten N+30..N+55: nach dem Tuer-zu-Ton (DOOR1A Ton 1 = 0,89 s
 * = 27 Takte) und vor dem Ende der 60-Takte-Ausblendung. Gemessen vor dem Fix (Lauf v0, Dossier 1.5): 230..480 = MAIN16 aus
 * 1141; nach dem Fix 0. Und N+70..N+100: MAIN32 laeuft (Lauf n1: 277..524). */
static void teil_pegel(int argc, char **argv)
{
    printf("== pegel ==\n");
    if (argc < 3) { printf("  FEHLER: Aufruf test_r39_dining1141 pegel <lauf>\n"); g_fail++; return; }
    char p[1024]; snprintf(p, sizeof p, "%s/debug.log", argv[2]);
    FILE *f = fopen(p, "rb");
    if (!f) { printf("  FEHLER: %s fehlt\n", p); g_fail++; return; }
    char z[4096]; int in1171 = 0; long n = -1;
    while (fgets(z, sizeof z, f)) {
        if (strstr(z, "loaded room1171.rdt")) in1171 = 1;
        const char *c = in1171 ? strstr(z, "[bgm] Sce_bgm_control slot=0 op=1 ") : NULL;
        if (c && (c = strstr(c, "capTick=")) != NULL) { n = atol(c + 8); break; }
    }
    fclose(f);
    PRUEF(n > 0, "ROOM1171 main00 Sce_bgm_control op=1 bei Takt %ld", n);
    if (n <= 0) return;
    snprintf(p, sizeof p, "%s/cap.raw", argv[2]);
    size_t sz = 0; uint8_t *raw = slurp(p, &sz);
    if (!raw) { printf("  FEHLER: %s fehlt\n", p); g_fail++; return; }
    const int16_t *x = (const int16_t *)raw; const long paare = (long)(sz / 4);
    double e1 = 0, e2 = 0; long n1 = 0, n2 = 0;
    for (long t = n + 30; t <= n + 55; t++)
        for (long k = t * 1470; k < (t + 1) * 1470 && k < paare; k++) { double v = (x[2*k] + x[2*k+1]) * 0.5; e1 += v * v; n1++; }
    for (long t = n + 70; t <= n + 100; t++)
        for (long k = t * 1470; k < (t + 1) * 1470 && k < paare; k++) { double v = (x[2*k] + x[2*k+1]) * 0.5; e2 += v * v; n2++; }
    free(raw);
    const int r1 = n1 ? (int)(sqrt(e1 / (double)n1)) : -1, r2 = n2 ? (int)(sqrt(e2 / (double)n2)) : -1;
    printf("    Takte %ld..%ld Pegel %d, Takte %ld..%ld Pegel %d\n", n + 30, n + 55, r1, n + 70, n + 100, r2);
    PRUEF(n1 > 0 && r1 < 60, "Punkt 9: nach dem Tuer-zu-Ton keine Musik aus 1141 (Pegel %d < 60; vorher 230..480)", r1);
    PRUEF(n2 > 0 && r2 > 150, "Punkt 9: danach laeuft der Raum-Titel MAIN32 (Pegel %d > 150)", r2);
}

int main(int argc, char **argv)
{
    if (argc < 2) { printf("Aufruf: test_r39_dining1141 <texte|programme|stamm|phase_h>\n"); return 2; }
    if (!strcmp(argv[1], "texte")) teil_texte();
    else if (!strcmp(argv[1], "programme")) teil_programme();
    else if (!strcmp(argv[1], "stamm")) teil_stamm();
    else if (!strcmp(argv[1], "phase_h")) teil_phase_h();
    else if (!strcmp(argv[1], "pegel")) teil_pegel(argc, argv);
    else { printf("unbekannter Teil %s\n", argv[1]); return 2; }
    printf("%s: %d Fehler\n", argv[1], g_fail);
    return g_fail ? 1 : 0;
}
