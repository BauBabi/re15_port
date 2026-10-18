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
#include "re15_damage.h"   /* re15_player_equipped_weapon (Nachbesserung 1 N4.2) */

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
        "Elza: Oh, no, this is John! He is in trouble, I have to help him!",
        "John: Puuuh, that was close! I thought we end up as gorilla meal....",
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
    PRUEF(kh > 0 && kb > 0, "HAEMMERN39 %d B / ENTRIEGELN39 %d B opcode-genau zerlegbar", nh, nb);
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
    /* ENTRIEGELN39 (Nachbesserung 1: die Szene am Riegel der Dachtuer, frueher BEFREIUNG39) */
    {   /* Punkt 2: Cut 7 -> Tuer auf (0x0E,0) -> Pos_set John innen vor die Tuer + Dir_set -> Tuer zu (0x0E,1) -> Cut 6 */
        const int16_t tx = RE15_R39J_JOHN_TUER_X, tz = RE15_R39J_JOHN_TUER_Z;
        const uint8_t c7[2] = { 0x29, RE15_R39J_CUT_ELZA }, auf[3] = { 0x36, 0x0E, 0x00 }, zu[3] = { 0x36, 0x0E, 0x01 };
        const uint8_t pos[8] = { 0x32, 0x00, (uint8_t)tx, (uint8_t)((uint16_t)tx >> 8), 0, 0, (uint8_t)tz, (uint8_t)((uint16_t)tz >> 8) };
        const uint8_t dir[8] = { 0x33, 0x00, 0x00, 0x00, (uint8_t)RE15_R39J_JOHN_TUER_YAW, (uint8_t)(RE15_R39J_JOHN_TUER_YAW >> 8), 0, 0 };
        const uint8_t c6[2] = { 0x29, RE15_R39J_CUT_TUER };
        const int i7 = finde(b, nb, c7, 2, 0), ia = finde(b, nb, auf, 3, i7 > 0 ? i7 : 0);
        const int ip = finde(b, nb, pos, 8, ia > 0 ? ia : 0), id = finde(b, nb, dir, 8, ip > 0 ? ip : 0);
        const int iz = finde(b, nb, zu, 3, id > 0 ? id : 0), i6 = finde(b, nb, c6, 2, iz > 0 ? iz : 0);
        PRUEF(i7 > 0 && ia > i7 && ip > ia && id > ip && iz > id && i6 > iz,
              "ENTRIEGELN39 Punkt 2: Cut 7 @+%04X, Tuer auf @+%04X, John Pos_set (%d,%d) @+%04X + Dir_set %d, Tuer zu @+%04X, Cut 6 @+%04X",
              i7, ia, tx, tz, ip, RE15_R39J_JOHN_TUER_YAW, iz, i6);
        const int16_t ex = RE15_R39J_ELZA_VORNE_X, ez = RE15_R39J_ELZA_VORNE_Z;
        const uint8_t walk[8] = { 0x40, 0x00, 0x05, 0x00, (uint8_t)ex, (uint8_t)((uint16_t)ex >> 8), (uint8_t)ez, (uint8_t)((uint16_t)ez >> 8) };
        const int iw = finde(b, nb, walk, 8, 0);
        PRUEF(iw > 0 && iw < i7, "ENTRIEGELN39 N4.2: Elza RUN (%d,%d) vor Cut 7 (rennt zur anderen Kamera)", ex, ez);
        /* N4.2: Programm beginnt mit sub05 @0x00FC8..@0x00FDB bytegleich (Riegel-Slot aus, (4,0xC3)=1, msg 12, Evt_next) */
        static const uint8_t sub05[20] = { 0x46, 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0x22, 0x04, 0xC3, 0x01, 0x2B, 0x0C, 0xFF, 0xFF, 0x02, 0x00 };
        PRUEF(nb > 20 && !memcmp(b, sub05, 20), "ENTRIEGELN39 N4.2: Anfang = ROOM1141 sub05 @0x00FC8..@0x00FDB (msg 12 \"You've opened the lock.\")");
        /* N4.2: nach Cut 7 TURN zur Tuer, dann Spieler-Plc_motion(2,6) (Form ROOM3060 @0x010BA/@0x010BE + Sleep 40 @0x010C2), vor Tuer auf */
        const uint8_t heben[12] = { 0x2E, 0x01, 0x00, 0x00, 0x3F, RE15_R39J_W_SUB, RE15_R39J_W_CLIP_HEBEN, 0x00, 0x09, 0x0A, 0x28, 0x00 };
        const uint8_t turn[4] = { 0x40, 0x00, 0x09, 0x00 };
        const int it = finde(b, nb, turn, 4, i7 > 0 ? i7 : 0), ih = finde(b, nb, heben + 4, 8, it > 0 ? it : 0);
        PRUEF(it > i7 && ih > it && ih < ia, "ENTRIEGELN39 N4.2: Cut 7 -> TURN zur Tuer @+%04X -> Plc_motion(2,6) + Sleep 40 @+%04X -> Tuer auf @+%04X",
              it, ih, ia);
        const uint8_t senken[8] = { 0x3F, RE15_R39J_W_SUB, RE15_R39J_W_CLIP_HEBEN, 0x00, 0x43, 0x00, 0x80, 0x00 };
        const int is = finde(b, nb, senken, 8, i6 > 0 ? i6 : 0);
        PRUEF(is > i6, "ENTRIEGELN39 N4.2: Waffe senken = Plc_motion(2,6) + Plc_flg 0x80 (ROOM1021 @0x020EA/@0x020EE) nach Cut 6 (@+%04X)", is);
    }
    {   /* N4.1: ROLLTOR39 = Gemaelde-Frage, Rolladen mit Ton, Steuerung zurueck (sub06 @0x010C8..@0x010D8), Merker aus */
        int nr = 0;
        const uint8_t *r = re15_dining_r39_programm(2, &nr);
        static const uint8_t frage[4] = { 0x2B, 0x0D, 0x80, 0xFF }, ende[12] = { 0x22, 0x02, 0x07, 0x00, 0x22, 0x01, 0x1B, 0x00, 0x3C, 0x01, 0x2E, 0x01 };
        const uint8_t merker_aus[6] = { 0x22, RE15_R39J_MERKER_BANK, RE15_R39J_MERKER_BIT, 0x00, 0x01, 0x00 };
        static const uint8_t ton[3] = { 0x36, 0x3B, 0x0C }, john[2] = { 0x2E, 0x02 }, c7r[2] = { 0x29, 0x07 };
        PRUEF(r && nr > 40 && !memcmp(r, frage, 4) && finde(r, nr, ton, 3, 0) > 0 && finde(r, nr, ende, 12, 0) > 0 &&
              !memcmp(r + nr - 6, merker_aus, 6) && finde(r, nr, john, 2, 0) < 0 && finde(r, nr, c7r, 2, 0) < 0,
              "ROLLTOR39 N4.1: Frage msg 13, Rolladen-Ton, Steuerung zurueck, Merker (5,0x10) aus, kein John/kein Cut 7 (%d B)", nr);
        /* N4.1/M1: HAEMMERN39 laeuft bis zum Entriegeln; die Reaktion wartet auf Merker und Pad-Sperre */
        static const uint8_t schleife[4] = { 0x21, 0x04, 0xC3, 0x00 };
        const uint8_t warte_m[8] = { 0x0F, 0x04, 0x08, 0x00, 0x21, RE15_R39J_MERKER_BANK, RE15_R39J_MERKER_BIT, 0x01 };
        static const uint8_t warte_p[8] = { 0x0F, 0x04, 0x08, 0x00, 0x21, 0x02, 0x07, 0x01 };
        const uint8_t m50[4] = { 0x2B, RE15_R39J_MSG_REAKTION, 0x00, 0x00 };
        const int iwm = finde(h, nh, warte_m, 8, 0), iwp = finde(h, nh, warte_p, 8, 0), i50 = finde(h, nh, m50, 4, 0);
        PRUEF(nh > 14 && !memcmp(h + 14, schleife, 4), "HAEMMERN39 N4.1: While Ck(4,0xC3)==0 (main00 @0x00A76) statt bis zum Rolladen");
        PRUEF(iwm > 0 && iwp > iwm && i50 > iwp && finde(h, nh, schleife, 4, iwp) < i50,
              "HAEMMERN39 M1: Reaktion wartet auf Merker (5,0x10) und Pad-Sperre (2,7), dann Ck(4,0xC3)==0, dann msg 50");
    }
    {   /* Punkt 6: zwei Se_on(0x3E,0x21) mit Sleep 5 dazwischen unmittelbar vor `09 0a 14 00` + Message_on 6 */
        const uint8_t g[24] = { 0x36, RE15_R39J_SE_BANK, RE15_R39J_SATZ_GLAS, 0,0,0,0,0,0,0,0,0, 0x09, 0x0A, 0x05, 0x00,
                                0x36, RE15_R39J_SE_BANK, RE15_R39J_SATZ_GLAS, 0,0,0,0,0 };
        const uint8_t m6[8] = { 0x09, 0x0A, 0x14, 0x00, 0x2B, 0x06, 0x00, 0x00 };
        const int ig = finde(b, nb, g, 24, 0), i6 = finde(b, nb, m6, 8, 0);
        PRUEF(ig > 0 && i6 == ig + 28, "ENTRIEGELN39 Punkt 6: Glasbruch x2 (RE2 ROOM1090 @0x02BB0..@0x02BC0) direkt vor Sleep 20 + "
                                       "msg 6 \"What was that?!\" (@+%04X, msg 6 @+%04X)", ig, i6 + 4);
        PRUEF(ig >= 4 && b[ig - 4] == 0x2B && b[ig - 3] == RE15_R39J_MSG_STIMME_WARTEN && b[ig - 2] == 0 && b[ig - 1] == 0,
              "ENTRIEGELN39 Punkt 6: davor Message_on 58 = Warten auf das Ende von Johns Sprachzeile (msg 5)");
        static const uint8_t pc58[4] = { 0x2B, RE15_R39J_MSG_STIMME_WARTEN, 0x00, 0x00 }, pc6[4] = { 0x2B, 0x06, 0x00, 0x00 };
        const unsigned alt_raum = g_current_room_id;
        g_current_room_id = 0x1141u;
        const int v58 = re15_dining_r39_message_on(pc58), v6 = re15_dining_r39_message_on(pc6);
        g_current_room_id = 0x1140u;
        const int v58l = re15_dining_r39_message_on(pc58);
        g_current_room_id = alt_raum;
        PRUEF(v58 == 1 && v6 == 0 && v58l == 0, "Warte-Zeile: (1141, 58) verbraucht, msg 6 und (1140, 58) nicht");
    }
    {   /* Punkt 7: Johns Plc_dest Modus 9 zu Elzas Platz (Bit 1), unmittelbar danach Gosub sub04, vor msg 9 */
        const int16_t ex = RE15_R38F_ELZA_PLATZ_X, ez = RE15_R38F_ELZA_PLATZ_Z;
        const uint8_t t[10] = { 0x40, 0x00, 0x09, 0x01, (uint8_t)ex, (uint8_t)((uint16_t)ex >> 8), (uint8_t)ez,
                                (uint8_t)((uint16_t)ez >> 8), 0x18, 0x04 };
        const uint8_t m9[4] = { 0x2B, 0x09, 0x00, 0x00 };
        int it = -1, k = 0;
        for (int i = finde(b, nb, t, 10, 0); i >= 0; i = finde(b, nb, t, 10, i + 1)) { it = i; k++; }
        const int i9 = finde(b, nb, m9, 4, 0);
        PRUEF(k == 2 && it > 0 && i9 > it, "ENTRIEGELN39 Punkt 7: John TURN -> Elza + Gosub sub04 auch vor msg 9 (%d Stellen, "
                                           "letzte @+%04X, msg 9 @+%04X)", k, it, i9);
    }
    {   /* Punkt 8: nach der Warteschleife (4,0xF2): Cut 8, msg 51 + Gesten (0,15)/(0,23), Plc_motion(2,6), msg 52 */
        const uint8_t w[4] = { 0x21, 0x04, 0xF2, 0x01 }, c8[2] = { 0x29, RE15_R39J_CUT_GESPRAECH };
        const uint8_t m51[4] = { 0x2B, RE15_R39J_MSG_PUH, 0, 0 }, m52[4] = { 0x2B, RE15_R39J_MSG_FOLGEN, 0, 0 };
        const uint8_t g15[4] = { 0x3F, 0x00, 0x0F, 0x00 }, g23[4] = { 0x3F, 0x00, 0x17, 0x00 }, wink[4] = { 0x3F, 0x00, 0x11, 0x00 };
        const uint8_t ziel[4] = { 0x3F, 0x02, 0x06, 0x00 };
        const int iw = finde(b, nb, w, 4, 0), i8 = finde(b, nb, c8, 2, iw > 0 ? iw : 0);
        const int i51 = finde(b, nb, m51, 4, i8 > 0 ? i8 : 0), i15 = finde(b, nb, g15, 4, i51 > 0 ? i51 : 0);
        const int i23 = finde(b, nb, g23, 4, i15 > 0 ? i15 : 0), i52 = finde(b, nb, m52, 4, i23 > 0 ? i23 : 0);
        const int iwk = finde(b, nb, wink, 4, i52 > 0 ? i52 : 0);
        PRUEF(iw > 0 && i8 > iw && i51 > i8 && i15 > i51 && i23 > i15 && i52 > i23 && iwk > i52,
              "ENTRIEGELN39 Punkt 8: Gorillas tot -> Cut 8 -> msg 51 + Gesten 15/23 -> msg 52 + Folge-Geste Clip 17 (M2)");
        const uint8_t run[4] = { 0x40, 0x00, 0x05, 0x01 };
        const int ir = finde(b, nb, run, 4, i52 > 0 ? i52 : 0);
        PRUEF(ir > iwk, "ENTRIEGELN39 Punkt 8: Johns Abgang (RUN) erst NACH msg 52 und der Geste");
        const int iz = finde(b, nb, ziel, 4, i8 > 0 ? i8 : 0);
        PRUEF(iz < 0 || iz > ir, "ENTRIEGELN39 M2/N4.4: kein Plc_motion(2,6) (= Zielen, ROOM1021 @0x020C6) zwischen Cut 8 und Johns Abgang");
        /* M3: John Pos_set an seinen Platz im selben Takt wie der Schnitt (vor dem ersten Sleep nach Cut_chg 8) */
        const int16_t jx = RE15_R39J_ECKE_X, jz = RE15_R39J_ECKE_Z;
        const uint8_t jpos[8] = { 0x32, 0x00, (uint8_t)jx, (uint8_t)((uint16_t)jx >> 8), 0, 0, (uint8_t)jz, (uint8_t)((uint16_t)jz >> 8) };
        static const uint8_t sl[2] = { 0x09, 0x0A };
        const int ijp = finde(b, nb, jpos, 8, i8 > 0 ? i8 : 0), isl = finde(b, nb, sl, 2, i8 > 0 ? i8 : 0);
        PRUEF(ijp > i8 && ijp < isl, "ENTRIEGELN39 M3: John Pos_set (%d,%d) @+%04X im Schnittbild von Cut 8 (vor dem ersten Sleep @+%04X)",
              jx, jz, ijp, isl);
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
    {   /* Der Zeichenweg (platform/pc/main.c npc_region_culled, PSX FUN_80014324) zeichnet einen NPC nur innerhalb des
         * Viereck-Ankers des angezeigten Cuts: der geparkte John darf in keinem Cut-Viereck liegen. */
        int drin = 0, mit = 0;
        for (int c = 0; c < (int)s_rdt.cut_count; c++) {
            int16_t xs[4], zs[4];
            if (!re15_rdt_get_region_quad(&s_rdt, c, xs, zs)) continue;
            mit++;
            if (re15_aot_point_in_quad(RE15_R39J_PARK_X, RE15_R39J_PARK_Z, xs, zs)) { drin++; printf("    Park-Lage im Viereck von Cut %d\n", c); }
        }
        PRUEF(mit == (int)s_rdt.cut_count && drin == 0, "geparkter John in keinem der %d Cut-Vierecke (%d mit Viereck) = nie gezeichnet",
              (int)s_rdt.cut_count, mit);
    }
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
    /* Nachbesserung 1 N4.1: nach dem Rolladen gibt ROLLTOR39 die Steuerung zurueck, John bleibt draussen, es klopft weiter */
    long frei_nach_rolladen = -1;
    int merker_an = 0;
    for (int f = 0; f < 600 && frei_nach_rolladen < 0; f++) {
        frame(0, 0);
        if (re15_game_flag_get(RE15_R39J_MERKER_BANK, RE15_R39J_MERKER_BIT)) merker_an = 1;
        if (g_scd.player_mode == 0 && !re15_game_flag_get(2, 7) && !re15_game_flag_get(1, 0x1B) &&
            !re15_game_flag_get(RE15_R39J_MERKER_BANK, RE15_R39J_MERKER_BIT) && s_bild > ja_bild + 30) frei_nach_rolladen = s_bild;
    }
    PRUEF(merker_an && frei_nach_rolladen > 0, "N4.1: nach dem Rolladen Steuerung frei (Bild %ld), Merker (5,0x10) war an und ist "
          "wieder aus", frei_nach_rolladen);
    PRUEF(re15_dining_r38_phase_h() == 1 && re15_game_flag_get(4, 0xC3) == 0 && john->x == RE15_R39J_PARK_X,
          "N4.1: Phase H laeuft weiter (Dachtuer verriegelt), John draussen (geparkt)");
    {   /* frei laufen: 200 Bilder vorwaerts gehen, Elza bewegt sich; das Klopfen laeuft weiter */
        const int32_t x0 = pl->x, z0 = pl->z;
        const int se0 = s_n_se;
        pl->rot_y = 3072;
        for (int f = 0; f < 260; f++) frame(RE15_PAD_BIT_UP, f == 0 ? RE15_PAD_BIT_UP : 0);
        const long weg = (long)labs((long)(pl->x - x0)) + (long)labs((long)(pl->z - z0));
        int klopf = 0;
        for (int i = se0; i < s_n_se; i++) if (s_se[i].bank == RE15_R39J_SE_BANK && s_se[i].satz == RE15_R39J_SATZ_KLOPFEN) klopf++;
        for (int f = 0; f < 400 && klopf == 0; f++) {
            frame(0, 0);
            for (int i = se0; i < s_n_se; i++) if (s_se[i].bank == RE15_R39J_SE_BANK && s_se[i].satz == RE15_R39J_SATZ_KLOPFEN) klopf++;
        }
        PRUEF(weg > 300, "N4.1: Elza laeuft nach dem Rolladen frei (Weg %ld)", weg);
        PRUEF(klopf > 0, "N4.1: John klopft weiter, solange die Dachtuer verriegelt ist (%d Schlaege)", klopf);
    }
    /* Nachbesserung 1 N4.2: Riegel der Dachtuer (Ereignis 5) - msg 12, dann die Szene */
    /* Elzas Inventar ist hier leer (Elza-Starttabelle @0x80074bc4 ohne Messer, re15_messer.h): eine Pistole hineinlegen,
     * nicht ausgeruestet - die Regel des Nutzers verlangt, dass sie diese hebt (Fall Messer: Teil fenster). */
    (void)re15_inv_grant(0x03, 15);
    int waffe_vorher = re15_player_equipped_weapon();
    printf("    Inventar beim Entriegeln:"); for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) printf(" %02x/%u", g_inv.slots[i].id, g_inv.slots[i].qty); printf("\n");
    druck_t riegel = druecke(-7250, -250 - 620, 3072, 1);
    PRUEF(!riegel.raumwechsel, "Riegel: kein Raumwechsel beim Entriegeln");
    int msg12 = 0;
    int msgs[64], n_msg = 0, pm2 = 0;
    long frei_bild = -1, bild_auf = -1, bild_zu = -1, bild_cut6 = -1, bild_cut7 = -1, bild_john_da = -1, bild_msg6 = -1;
    long bild_glas2 = -1, bild_heben = -1, bild_senken = -1, bild_steht = -1;
    int cut_beim_auftauchen = -1, john_sichtbar_vor_cut6 = 0, elza_in_cut7 = 0, john_in_cut6 = 0, cut_beim_heben = -1;
    int blick9 = -1, blick_fehl = 0, blick_n = 0, gerannt = 0, cuts_lauf[16], n_cuts_lauf = 0, w_vorwaerts_beim_heben = 0;
    last = -1;
    for (int f = 0; f < 6000; f++) {
        uint16_t e = (g_scd.message_active && g_scd.message_id == 12 && (f % 15) == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
        if (g_scd.message_active && (int)g_scd.message_id != last) {
            last = (int)g_scd.message_id;
            if (n_msg < 64) msgs[n_msg++] = last;
            if (last == 12) msg12 = 1;
            if (last == 6) bild_msg6 = s_bild;
            if (last == 9) blick9 = ausgerichtet(john, pl, 128);
        }
        if (!g_scd.message_active) last = -1;
        if (g_scd.player_mode == 2 && bild_steht < 0 && labs((long)pl->z + 870L) > 1000L) gerannt = 1;
        if (bild_cut7 < 0 && n_cuts_lauf < 16 && (n_cuts_lauf == 0 || cuts_lauf[n_cuts_lauf - 1] != s_shown) && bild_heben < 0)
            cuts_lauf[n_cuts_lauf++] = s_shown;
        if (bild_steht < 0 && abs(pl->x - RE15_R39J_ELZA_VORNE_X) < 150 && abs(pl->z - RE15_R39J_ELZA_VORNE_Z) < 150) bild_steht = s_bild;
        if (bild_heben < 0 && re15_dining_r39_w_clip() == RE15_R39J_W_CLIP_HEBEN) {
            bild_heben = s_bild; cut_beim_heben = s_shown; w_vorwaerts_beim_heben = !(pl->anim_flags & 0x80);
        }
        if (bild_heben >= 0 && bild_senken < 0 && re15_dining_r39_w_clip() == RE15_R39J_W_CLIP_HEBEN && (pl->anim_flags & 0x80))
            bild_senken = s_bild;
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
    printf("    Kameras beim Lauf:"); for (int i = 0; i < n_cuts_lauf; i++) printf(" %d", cuts_lauf[i]); printf("\n");
    printf("    Elza steht %ld, Cut 7 ab Bild %ld, Waffe hoch %ld (Cut %d), Tuer auf %ld, John da %ld (angezeigter Cut %d), "
           "Tuer zu %ld, Cut 6 ab %ld, Waffe runter %ld\n", bild_steht, bild_cut7, bild_heben, cut_beim_heben, bild_auf,
           bild_john_da, cut_beim_auftauchen, bild_zu, bild_cut6, bild_senken);
    PRUEF(msg12, "N4.2: Riegel -> msg 12 \"You've opened the lock.\" (sub05 @0x00FD6)");
    PRUEF(re15_game_flag_get(4, 0xC3) == 1, "N4.2: (4,0xC3) = 1 (sub05 @0x00FD2)");
    PRUEF(gerannt && bild_steht > 0, "N4.2: Elza rennt vom Riegel in den Raum (Bild %ld am Ziel)", bild_steht);
    {
        const int w = re15_player_equipped_weapon();
        PRUEF(w >= 3 && w != 9 && w != 10 && w != 11 && re15_inv_find_item((uint8_t)w) >= 0,
              "N4.2: Elza hebt eine Schusswaffe aus dem Inventar (Waffe %d, vorher %d)", w, waffe_vorher);
    }
    PRUEF(bild_heben > bild_steht && cut_beim_heben == RE15_R39J_CUT_ELZA && w_vorwaerts_beim_heben,
          "N4.2: Waffe heben = Plc_motion(2,6) W-Bank-Clip 6 vorwaerts, nachdem sie steht, bei Kamera Cut 7 (Bild %ld)", bild_heben);
    PRUEF(bild_auf > bild_heben, "N4.2: erst Waffe heben (Bild %ld), dann Tuer auf (Bild %ld)", bild_heben, bild_auf);
    PRUEF(bild_senken > bild_cut6, "N4.2: Waffe runter (Bild %ld) erst nach dem Blick auf John (Cut 6 ab %ld)", bild_senken, bild_cut6);
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
    {   /* Runde 38 weiter gueltig: Rolladen-Ton in der Folge von ROOM1131 sub02; nach dem Entriegeln kein Klopfen/Hilferuf */
        int r0 = -1, r1 = -1, r2 = -1, danach = 0;
        for (int i = se_vorher; i < s_n_se; i++) {
            if (s_se[i].bank == 0x3B && s_se[i].satz == 0x0C && r0 < 0) r0 = i;
            if (s_se[i].bank == 0x3B && s_se[i].satz == 0x0A && r1 < 0) r1 = i;
            if (s_se[i].bank == 0x3B && s_se[i].satz == 0x0B && r2 < 0) r2 = i;
            if (s_se[i].bank == RE15_R39J_SE_BANK && s_se[i].satz != RE15_R39J_SATZ_GLAS && bild_steht > 0 && s_se[i].bild > bild_steht) danach++;
        }
        PRUEF(r0 >= 0 && r1 > r0 && r2 > r1, "Rolladen-Ton 0x3B 0x0C, 0x0A, 0x0B in der Folge von ROOM1131 sub02 (Runde 38)");
        PRUEF(danach == 0, "nach dem Entriegeln kein Klopfen/Hilferuf mehr (%d)", danach);
    }
    /* Kampf: Gorillas einfrieren, John in die Ecke, Sperre */
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == RE15_R38F_GORILLA_TYP) g_actors[s].grid_id |= 0x20;
    for (int f = 0; f < 900 && !(abs(john->x - RE15_R39J_ECKE_X) < 400 && abs(john->z - RE15_R39J_ECKE_Z) < 400); f++) frame(0, 0);
    PRUEF(abs(john->x - RE15_R39J_ECKE_X) < 400 && abs(john->z - RE15_R39J_ECKE_Z) < 400, "John in der Ecke (%d,%d)", john->x, john->z);
    PRUEF(g_aot.slots[0].active && g_aot.slots[0].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[0].event_id == RE15_R38F_MSG_1141_SPERRE,
          "Sperre: Doppeltuer zeigt msg 41 solange die Gorillas leben (Runde 38)");
    {
        druck_t r = druecke(-7250, -250 - 620, 3072, 200);   /* Dachtuer: Vorwaerts-Punkt (-7250,-250), Blick +Z */
        PRUEF(r.msg_id == RE15_R38F_MSG_1141_SPERRE && !r.raumwechsel, "Dachtuer waehrend des Kampfs: msg %d, kein Raumwechsel", r.msg_id);
        for (int f = 0; f < 200; f++) { uint16_t e = (g_scd.message_active && (f % 15) == 0) ? RE15_PAD_BIT_SQUARE : 0; frame(e, e); }
    }
    {   /* Nachbesserung 1 N4.3: John schiesst ab und an (Faden SCHUETZE39, Form ROOM1021 @0x020C6..@0x020F2) */
        const unsigned s0 = g_re15_r39j_schuss_zaehler, t0 = g_re15_r39j_treffer_zaehler;
        int hp0[RE15_ACTOR_MAX], gesehen_zielen = 0, gesehen_feuer = 0, auf = 0, ziel_john = 0;
        long b_feuer = -1, b_zielen = -1;
        for (int s = 0; s < RE15_ACTOR_MAX; s++) hp0[s] = g_actors[s].hp;
        for (int f = 0; f < 1200 && g_re15_r39j_schuss_zaehler < s0 + 2; f++) {
            frame(0, 0);
            if (john->state == 4 && john->sub_state_1 == 2 && john->motion == 6 && !(john->anim_flags & 0x80) && b_zielen < 0) {
                gesehen_zielen = 1; b_zielen = s_bild; }
            if (john->state == 4 && john->sub_state_1 == 2 && john->motion == 7 && b_feuer < 0) { gesehen_feuer = 1; b_feuer = s_bild; }
            for (int s = 1; s < RE15_ACTOR_MAX; s++) {
                if (re15_dining_r39_aufmerksam(s) > 0) {
                    auf = 1;
                    int32_t zx = pl->x, zz = pl->z;
                    re15_dining_r39_affe_ziel(s, &zx, &zz);
                    if (zx == john->x && zz == john->z) ziel_john = 1;
                }
            }
        }
        int getroffen = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == RE15_R38F_GORILLA_TYP && g_actors[s].hp < hp0[s]) getroffen++;
        printf("    John: Zielen Bild %ld, Feuer-Clip Bild %ld, Schuesse %u, Treffer %u\n", b_zielen, b_feuer,
               g_re15_r39j_schuss_zaehler - s0, g_re15_r39j_treffer_zaehler - t0);
        PRUEF(gesehen_zielen && gesehen_feuer && b_feuer > b_zielen,
              "N4.3: John zielt (eigene Bank Clip 6, ROOM1021 @0x020C6) und feuert (Clip 7, @0x020CE)");
        PRUEF(g_re15_r39j_schuss_zaehler >= s0 + 2, "N4.3: zwei Schuesse je Salve (@0x020CE/@0x020D6): %u",
              g_re15_r39j_schuss_zaehler - s0);
        PRUEF(g_re15_r39j_treffer_zaehler > t0 && getroffen > 0,
              "N4.3: Treffer am Gorilla (Energie gesunken bei %d Gorilla(s)), Schaden der Handfeuerwaffe", getroffen);
        PRUEF(auf && ziel_john, "N4.3: der getroffene Gorilla beachtet John (Lenkziel = Johns Lage)");
    }
    /* Gorillas tot (Kill-Flags pc[7] 0xCB/0xCC, Bank 7 — wie RE15_R38F_GORILLAS_TOT, KEINE Schusszahl) */
    pl->x = -3000; pl->z = -9000; pl->rot_y = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == RE15_R38F_GORILLA_TYP) re15_actor_free(s);
    re15_game_flag_set(7, 0xCB, 1);
    re15_game_flag_set(7, 0xCC, 1);
    int m51 = -1, m52 = -1, geste15 = 0, geste23 = 0, wink = 0, cut8 = 0, blick51 = -1, park = -1, k = 0, elza_verschoben = 0;
    int zielt_auf_elza = 0, sprung_max = 0, sprung_bild = -1, vor_cut = s_shown;
    int32_t vor_jx = john->x, vor_jz = john->z;
    long min_d2 = -1;
    int vor_sicht = 0, park_cut_vorher = -1, park_cut = -1, park_sicht = 1, sicht_cut7 = 0;
    long b51 = -1, b52 = -1, b_run = -1;
    last = -1;
    for (int f = 0; f < 4000; f++) {
        frame(0, 0);
        if (s_shown == RE15_R39J_CUT_GESPRAECH) cut8 = 1;
        {   /* Nachbesserung 1 M3 (Runde-38-Pruefung "John springt nie"): Schritt je Bild ohne Kameraschnitt, im Bild */
            const int dx = (int)(john->x - vor_jx), dz = (int)(john->z - vor_jz);
            const int schritt = abs(dx) > abs(dz) ? abs(dx) : abs(dz);
            const int geht = (john->state == 4 && (john->sub_state_1 == 4 || john->sub_state_1 == 5 ||
                                                   john->sub_state_1 == 7 || john->sub_state_1 == 8));   /* Walk-Subs 0x80076ca0 */
            if (!geht && s_shown == vor_cut && john->x > -29000 && vor_jx > -29000 && sichtbar(s_shown, john) && schritt > sprung_max) {
                sprung_max = schritt; sprung_bild = (int)s_bild; }
            vor_jx = john->x; vor_jz = john->z; vor_cut = s_shown;
        }
        if (g_scd.message_active && (int)g_scd.message_id != last) {
            last = (int)g_scd.message_id;
            if (last == RE15_R39J_MSG_PUH) { m51 = k++; b51 = s_bild; blick51 = ausgerichtet(john, pl, 128); }
            if (last == RE15_R39J_MSG_FOLGEN) { m52 = k++; b52 = s_bild; }
        }
        if (!g_scd.message_active) last = -1;
        if (b51 >= 0 && b52 < 0 && john->sub_state_1 == 0 && john->motion == 15) geste15 = 1;
        if (b51 >= 0 && b52 < 0 && john->sub_state_1 == 0 && john->motion == 23) geste23 = 1;
        if (b52 >= 0 && john->sub_state_1 == 0 && john->motion == 17) wink = 1;   /* Nachbesserung 1 M2: Clip 17 */
        if (b51 >= 0 && b_run < 0 && john->sub_state_1 == 2 && john->motion == 6) zielt_auf_elza++;
        if (b52 >= 0 && b_run < 0 && john->state == 4 && john->sub_state_1 == 5) b_run = s_bild;
        if (b_run >= 0 && john->x > -29000) {   /* Runde 38 Nachbesserung 1 M3 weiter gueltig: kein Schub, Abstand im Bild */
            if (abs(pl->x - RE15_R39J_ELZA_NACH_X) > 50 || abs(pl->z - RE15_R39J_ELZA_NACH_Z) > 50) elza_verschoben++;
            const long ddx = john->x - pl->x, ddz = john->z - pl->z;
            const long d2 = ddx * ddx + ddz * ddz;
            if (sichtbar(s_shown, john) && (min_d2 < 0 || d2 < min_d2)) min_d2 = d2;
        }
        if (b52 >= 0 && park < 0 && john->x > -29000) { vor_sicht = sichtbar(s_shown, john); park_cut_vorher = s_shown;
                                                         if (s_shown == 7 && sichtbar(7, john)) sicht_cut7 = 1; }
        if (john->x <= -29000 && park < 0 && b52 >= 0) { park = (int)s_bild; park_cut = park_cut_vorher; park_sicht = vor_sicht; break; }
    }
    PRUEF(cut8, "Punkt 8: Kamera Cut 8 (Suedwest-Ecke) nach dem Tod der Gorillas");
    PRUEF(sprung_max <= 40, "M3: John springt nie im Bild (groesster Schritt je Bild ohne Schnitt und ohne Lauf-Sub %d, Bild %d; Abnahme: 138)",
          sprung_max, sprung_bild);
    PRUEF(abs(pl->x - RE15_R39J_ELZA_NACH_X) < 200 && abs(pl->z - RE15_R39J_ELZA_NACH_Z) < 200, "Punkt 8: Elza bei (%d,%d)", pl->x, pl->z);
    PRUEF(m51 == 0 && m52 == 1 && b52 > b51, "Punkt 8: msg 51 (Bild %ld) vor msg 52 (Bild %ld)", b51, b52);
    PRUEF(blick51 == 1, "Punkt 8: John blickt bei msg 51 zu Elza");
    PRUEF(geste15 && geste23, "Punkt 8: zu msg 51 Johns Gesten Clip 15 und 23 (Form sub02 @0x00CB8/@0x00CCC)");
    PRUEF(wink, "Punkt 8 / M2: zu msg 52 die Folge-Geste Clip 17 (John 1141 @0x00DBA, ROOM3070 @0x03724)");
    PRUEF(zielt_auf_elza == 0, "M2/N4.4: John richtet im Gespraech nie die Waffe auf Elza (Sub 2 Clip 6: %d Bilder)", zielt_auf_elza);
    PRUEF(b_run > b52, "Punkt 8: John laeuft erst nach msg 52 los (Bild %ld)", b_run);
    PRUEF(elza_verschoben == 0, "Abgang: Elza wird nicht verschoben (Runde 38 M3, %d Bilder)", elza_verschoben);
    PRUEF(min_d2 < 0 || min_d2 > 900L * 900L, "Abgang: John im Bild nie naeher als 900 an Elza (Runde 38 M3; min %ld)",
          min_d2 < 0 ? -1L : (long)sqrt((double)min_d2));
    PRUEF(park > 0 && re15_game_flag_get(9, RE15_R38F_BIT_JOHN_WEG) == 1 && re15_game_flag_get(4, 0xC3) == 1,
          "Abgang: John fort (Bild %d), (9,67) = 1, Riegel (4,0xC3) = 1", park);
    PRUEF(park_cut == 7 && !park_sicht && sicht_cut7, "Abgang wie Runde 38: geparkt waehrend Cut 7 (Cut %d), im Bild davor nicht "
          "sichtbar, vorher sichtbar durch Cut 7 gelaufen", park_cut);
    for (int f = 0; f < 60 && g_scd.player_mode != 0; f++) frame(0, 0);
    for (int f = 0; f < 5; f++) frame(0, 0);
    PRUEF(g_scd.player_mode == 0 && g_scd.cut_auto_enabled && s_shown != 7, "Steuerung frei, Kamera-Automatik an, Cut 7 verlassen (Cut %d)",
          s_shown);
    PRUEF(g_aot.slots[1].active && g_aot.slots[1].type == RE15_AOT_TYPE_DOOR && g_aot.door_params[1].dest_room == 0x17 &&
          g_aot.slots[0].active && g_aot.slots[0].type == RE15_AOT_TYPE_DOOR && g_aot.door_params[0].dest_room == 0x13,
          "Dachtuer Slot 1 -> 0x17 (ROOM1171), Doppeltuer Slot 0 -> 0x13 (ROOM1131)");
    {
        druck_t r = druecke(-7250, -250 - 620, 3072, 200);
        PRUEF(r.raumwechsel && r.ziel == 0x1171, "Dachtuer -> ROOM%04X (soll 1171)", (unsigned)r.ziel);
    }
    re15_game_state_t g = g_game;
    if (room_boot(RE15_R38F_RAUM_1141, -7600, -17600, 0, 0) != 0) return;
    g_game = g;
    for (int f = 0; f < 30; f++) frame(0, 0);
    int john_da = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == RE15_R38F_JOHN_TYP) john_da = 1;
    PRUEF(!john_da && re15_dining_r38_phase_h() == 0, "Wiederbetreten: kein John, keine Phase H");
}

/* ---- Teil: fenster (Nachbesserung 1, M1 + N4.1) ---------------------------------------------------------------------
 * Abnahme 0 Lauf F: Gemaelde-Frage zwischen erstem Hilferuf (F75) und Reaktion (F165) geoeffnet, "ja" -> msg 50 erschien
 * mitten in der Rolladen-Szene. Jetzt: die Reaktion wartet, bis ROLLTOR39 fertig ist (Merker (5,0x10), Pad-Sperre (2,7)),
 * und kommt dann genau einmal, solange die Dachtuer verriegelt ist. Danach Wiederbetreten mit offenem Rolladen (Phase H
 * weiter: Klopfen, kein Gemaelde, Riegel = Ereignis 5, keine zweite Reaktion) und Entriegeln von dort. */
static void teil_fenster(void)
{
    printf("== fenster ==\n");
    re15_game_state_init();
    re15_inv_load_briefing();
    re15_game_flag_set(RE15_R38F_F0_BANK, RE15_R38F_F0_BIT, 1);
    if (room_boot(RE15_R38F_RAUM_1141, -8300, -10250, 2048, 0) != 0) return;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *john = &g_actors[1];
    long ruf = -1;
    for (int f = 0; f < 400 && ruf < 0; f++) { frame(0, 0); if (g_re15_r39j_se_zaehler[2] > 0) ruf = s_bild; }
    for (int f = 0; f < 5; f++) frame(0, 0);
    PRUEF(ruf > 0 && !g_scd.message_active && re15_game_flag_get(9, RE15_R39J_BIT_REAKTION) == 0,
          "erster Hilferuf Bild %ld, Reaktion noch nicht da (Fenster der Abnahme F75..F165)", ruf);
    /* Gemaelde im Fenster: Quadrat, Frage, "ja" */
    pl->x = -8300; pl->z = -10250; pl->rot_y = 2048;
    long ja = -1, b50 = -1, frei = -1, merker_bei_50 = -1, roll_bei_50 = -1;
    int n50 = 0, last = -1, merker_gesehen = 0;
    for (int f = 0; f < 1500; f++) {
        uint16_t e = (f == 0 || (g_scd.message_active && g_scd.message_id == 13 && (f % 20) == 0)) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
        if (ja < 0 && re15_game_flag_get(RE15_R38F_6A_BANK, RE15_R38F_6A_BIT)) ja = s_bild;
        if (re15_game_flag_get(RE15_R39J_MERKER_BANK, RE15_R39J_MERKER_BIT)) merker_gesehen = 1;
        /* Ende des Gemaelde-Programms ROLLTOR39 = Merker wieder aus (letzte Zeile vor Evt_end) */
        if (ja > 0 && frei < 0 && merker_gesehen && !re15_game_flag_get(RE15_R39J_MERKER_BANK, RE15_R39J_MERKER_BIT)) frei = s_bild;
        if (g_scd.message_active && (int)g_scd.message_id != last) {
            last = (int)g_scd.message_id;
            if (last == RE15_R39J_MSG_REAKTION) {
                n50++; b50 = s_bild;
                merker_bei_50 = re15_game_flag_get(RE15_R39J_MERKER_BANK, RE15_R39J_MERKER_BIT);
                roll_bei_50 = re15_game_flag_get(RE15_R38F_6A_BANK, RE15_R38F_6A_BIT) && !re15_game_flag_get(1, 0x1B);
            }
        }
        if (!g_scd.message_active) last = -1;
    }
    printf("    Ruf %ld, ja %ld, Rolladen-Programm zu Ende %ld, msg 50 ab %ld (%d mal)\n", ruf, ja, frei, b50, n50);
    PRUEF(ja > ruf && merker_gesehen, "Gemaelde im Fenster: \"ja\" Bild %ld nach dem ersten Ruf, Merker (5,0x10) gesetzt", ja);
    PRUEF(n50 == 1, "M1: Reaktion genau einmal (%d)", n50);
    PRUEF(b50 > frei && frei > ja && merker_bei_50 == 0 && roll_bei_50 == 1,
          "M1: Reaktion erst nach der Rolladen-Szene (ROLLTOR39 zu Ende Bild %ld, msg 50 Bild %ld, Merker dabei %ld, Balken aus)",
          frei, b50, merker_bei_50);
    PRUEF(re15_game_flag_get(9, RE15_R39J_BIT_REAKTION) == 1 && john->x == RE15_R39J_PARK_X, "Reaktion gesehen, John weiter draussen");
    /* Wiederbetreten mit offenem Rolladen (Phase H laeuft weiter, N4.1) */
    re15_game_state_t g = g_game;
    if (room_boot(RE15_R38F_RAUM_1141, -7600, -17600, 0, 0) != 0) return;
    g_game = g;
    john = &g_actors[1];
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const unsigned klopf0 = g_re15_r39j_se_zaehler[0];
    int n50b = 0;
    for (int f = 0; f < 400; f++) {
        frame(0, 0);
        if (g_scd.message_active && g_scd.message_id == RE15_R39J_MSG_REAKTION) n50b++;
    }
    PRUEF(re15_dining_r38_phase_h() == 1 && john->active && john->x == RE15_R39J_PARK_X,
          "Wiederbetreten mit offenem Rolladen: Phase H, John geparkt");
    PRUEF(g_re15_r39j_se_zaehler[0] > klopf0 && n50b == 0, "Wiederbetreten: Klopfen laeuft (%u Schlaege), keine zweite Reaktion",
          g_re15_r39j_se_zaehler[0] - klopf0);
    PRUEF(!g_aot.slots[3].active,
          "Wiederbetreten: kein Gemaelde-Ereignis mehr (sub00 setzt Slot 3 nur bei (3,0x6A)=0, @0x00B84/@0x00B88)");
    {   /* Entriegeln aus dem Wiederbetreten heraus */
        druck_t r = druecke(-7250, -250 - 620, 3072, 1);
        int msg12 = 0, john_da = 0;
        for (int f = 0; f < 1500 && !john_da; f++) {
            uint16_t e = (g_scd.message_active && g_scd.message_id == 12 && (f % 15) == 0) ? RE15_PAD_BIT_SQUARE : 0;
            frame(e, e);
            if (g_scd.message_active && g_scd.message_id == 12) msg12 = 1;
            if (john->x > -29000) john_da = 1;
        }
        PRUEF(re15_player_equipped_weapon() == 1, "N4.2: ohne Schusswaffe im Inventar hebt Elza das Messer (Waffe %d)",
              re15_player_equipped_weapon());
        PRUEF(!r.raumwechsel && msg12 && john_da && re15_game_flag_get(4, 0xC3) == 1,
              "Entriegeln nach dem Wiederbetreten: msg 12, (4,0xC3) = 1, John kommt herein");
    }
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
    else if (!strcmp(argv[1], "fenster")) teil_fenster();
    else if (!strcmp(argv[1], "pegel")) teil_pegel(argc, argv);
    else { printf("unbekannter Teil %s\n", argv[1]); return 2; }
    printf("%s: %d Fehler\n", argv[1], g_fail);
    return g_fail ? 1 : 0;
}
