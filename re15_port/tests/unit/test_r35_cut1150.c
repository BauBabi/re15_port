/* test_r35_cut1150.c — RIEGEL Spur L (Runde 35): Tuer 1060->1040, Irons-Todesszene ROOM1150,
 * Knall-Montage 1130/1040/1030, Schluss-Schnitt 11C0, Rueckkehr, Nachspawns, Totenpose.
 *
 * Dossier: analysis/befunde_runde35/L_cut1150.md. Konstanten/Belege: include/re15_tuer1060.h,
 * include/re15_irons_tod.h, engine/src/tuer1060_1040.c, engine/src/irons_tod_1150.c.
 *
 * Verfahren wie test_r34n_d_adaruf.c: ausgelieferte RDT laden, Raum ueber scd_room_reenter aufbauen
 * (derselbe Einhaengepunkt wie im Spiel -> die Installer laufen), Bilder in der Reihenfolge von
 * platform/pc/main.c fahren und MESSEN (Flags, Faeden, Aktoren, Nachrichten, Tuer-Anforderung).
 * Jeder Montage-Schritt wird einzeln aufgebaut (Zustand der Kette vorgegeben, wie ihn der vorige
 * Raum hinterlaesst) und bis zur Tuer-Anforderung (g_room_change) gefahren.
 *
 * Teile (je ein ctest-Eintrag): tuer1060 programme zaehlung szene montage_1130 montage_1040
 * montage_1030 montage_11c0 rueckkehr totenpose */
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
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_tim.h"
#include "re15_vab.h"
#include "re15_tuer1060.h"
#include "re15_irons_tod.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void re15_actor_step_all_walkers(void);

static int g_fail = 0;
/* Spione der Tuer-/Knall-Tonbank (tests/test_support.c): der Port-Takt spielt die Knall-Saetze auf die Signale
 * (5,28)/(5,29) hin (re15_irons_tod.h RE15_IT_SIG_KNALL_*). */
extern int g_test_tuer_laden_count, g_test_tuer_se_last, g_test_tuer_se_count;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;
static uint16_t           s_rdt_raum = 0;

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

static void bank_laden(uint8_t type, int buf);

static int rdt_laden(uint16_t room)
{
    if (s_raw && s_rdt_raum == room) return 0;
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz); s_rdt_raum = 0;
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    s_rdt_raum = room;
    return 0;
}

/* Ein Spielbild in der Reihenfolge von platform/pc/main.c (wie test_r34n_d_adaruf.c). */
static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_letterbox_tick(re15_game_flag_get(1, 27));
    {
        int cine = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
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

static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, uint8_t cut)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    bank_laden(0x45, 0); bank_laden(0x42, 1); bank_laden(0x40, 2);   /* Irons, Ada, Marvin */
    bank_laden(0x16, 3);   /* Zombie Typ 0x16 (Raum-Records 1040/1030, Kriecher): ohne Bank keine Wurzelbewegung */
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = cut; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    /* Raum-RBJ binden wie main.c:4355 (die Szenen-Clips von Irons/Ada/Marvin liegen im Record) —
     * VOR scd_room_reenter, weil der Installer dort Irons' Pose setzt. */
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

static void flags_leeren(void) { re15_game_state_init(); }

/* Gegner-Bank aus CDEMD0.EMS laden wie die Live-Plattform (main.c pc_enemy_load) und probe_marvin_10d0.c:
 * ohne geladene Bank loest der Marker-Binder den RBJ-Record nicht auf und die Clip-Laengen fallen auf
 * die eingebetteten Tabellen zurueck (Irons' Clip 2 waere 52 statt 90 Bilder). */
static uint8_t *s_ems = NULL; static size_t s_ems_sz = 0;
static uint8_t  s_bank_buf[4][0x60000];
static void bank_laden(uint8_t type, int buf)
{
    if (!s_ems) {
        char p2[600]; snprintf(p2, sizeof p2, "%s/EMD/CDEMD0.EMS", RE15_ASSET_PSX_DIR);
        s_ems = slurp(p2, &s_ems_sz);
    }
    if (!s_ems) return;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_ems, s_ems_sz, idx, &off, &len) != 0) return;
    re15_enemy_bank_t *eb = re15_enemy_alloc(type);
    if (!eb || len > sizeof s_bank_buf[buf]) return;
    memcpy(s_bank_buf[buf], s_ems + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_bank_buf[buf], len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return;
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok   = (re15_emd_parse_loco_bank(s_bank_buf[buf], len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(s_bank_buf[buf], len, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->own_ok    = (re15_emd_parse_own_bank(s_bank_buf[buf], len, &eb->skel_own, &eb->anim_own) == 0);
}

static int faeden_im_programm(void)
{
    int n = 0; const uint8_t *p = re15_irons_tod_laufprogramm(NULL);
    for (int s = 0; s < SCD_THREAD_COUNT; s++) {
        const scd_thread_t *t = &g_scd.threads[s];
        if (t->active && t->pc >= p && t->pc < p + 640) n++;
    }
    return n;
}

static int aktive_gegner(int typ /* -1 = alle */)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type != 0 && (typ < 0 || (int)g_actors[s].type == typ)) n++;
    return n;
}

/* Messhilfe (RE15_R35L_DUMP=1): alle aktiven Gegner mit Ort, Stempel und Kriech-Zustand. */
static void gegner_dump(const char *marke, int bild)
{
    if (!getenv("RE15_R35L_DUMP")) return;
    printf("  [%s F%d]", marke, bild);
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *a = &g_actors[s];
        if (!a->active || !a->type) continue;
        if (getenv("RE15_R35L_DUMP_AB") && s < atoi(getenv("RE15_R35L_DUMP_AB"))) continue;
        printf(" %d:t%02x(%d,%d,y%d)g%02x s%d/%d st%d fl%04x mo%d", s, a->type, (int)a->x, (int)a->z, (int)a->y, a->grid_id,
               a->state, a->sub_state_1, a->member_0b, a->anim_flags, a->motion);
    }
    printf("\n");
}

/* Bis zu `max` Bilder fahren, Nachrichten-Ids und Tuer-Anforderung mitschreiben. */
typedef struct {
    int bilder; int raumwechsel; unsigned ziel; int ziel_cut; int32_t ziel_x, ziel_z;
    int msgs[16]; int n_msgs;
    int kniet_gesehen; int irons_clip5; int irons_fall_bild; int irons_clip2;
    int32_t pl_min_dist_couch;
    /* ROOM1030: je Aktor das Kriech-Bit gesehen (anim_flags 0x1000 bzw. Kriech-Wurzel grid&0xf == 1) und die
     * z-Spanne; ROOM1040: kleinstes z je Aktor (durchs Tor = z < Tor-z). */
    uint8_t kriech[RE15_ACTOR_MAX]; int32_t z_min[RE15_ACTOR_MAX], z_max[RE15_ACTOR_MAX], z_erst[RE15_ACTOR_MAX];
    uint8_t gesehen[RE15_ACTOR_MAX];
    /* Leons Kopfschuetteln (Plc_neck Modus 4 = Flag-Bit 0x40): erstes/letztes Bild, Gier-Spanne, Neigung. */
    int neck4_von, neck4_bis; int neck_yaw_min, neck_yaw_max, neck_pitch_min, neck_pitch_max; int steht_auf_von, steht_auf_bis;
    /* Aufstehen (Nachbesserung 1 M3): Bild, in dem der Rueckwaerts-Zaehler das Clipende (Bild 24) erreicht, und die
     * Zahl der Bilder, in denen er sich bewegt hat (Halbtakt @0x80030670-80: jedes zweite Bild). */
    int steht_auf_ende, steht_auf_schritte, steht_auf_letzt;
} lauf_t;

static void lauf(lauf_t *L, int max, int bis_tuer)
{
    memset(L, 0, sizeof *L); L->pl_min_dist_couch = 1 << 30; L->irons_fall_bild = -1;
    L->neck4_von = L->neck4_bis = L->steht_auf_von = L->steht_auf_bis = -1;
    L->steht_auf_ende = -1; L->steht_auf_letzt = -1;
    int letzte_msg = -1;
    for (int f = 0; f < max; f++) {
        frame(0, 0);
        L->bilder = f + 1;
        if (g_scd.message_active && (int)g_scd.message_id != letzte_msg) {
            letzte_msg = (int)g_scd.message_id;
            if (L->n_msgs < 16) L->msgs[L->n_msgs++] = letzte_msg;
        }
        if (!g_scd.message_active) letzte_msg = -1;
        const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        if (pl->motion == 11 && pl->anim_use_pl00) L->kniet_gesehen = 1;
        if (pl->neck_flags & 0x40) {
            if (L->neck4_von < 0) { L->neck4_von = f; L->neck_yaw_min = L->neck_yaw_max = pl->neck_yaw; L->neck_pitch_min = L->neck_pitch_max = pl->neck_pitch; }
            L->neck4_bis = f;
            if (pl->neck_yaw < L->neck_yaw_min) L->neck_yaw_min = pl->neck_yaw;
            if (pl->neck_yaw > L->neck_yaw_max) L->neck_yaw_max = pl->neck_yaw;
            if (pl->neck_pitch < L->neck_pitch_min) L->neck_pitch_min = pl->neck_pitch;
            if (pl->neck_pitch > L->neck_pitch_max) L->neck_pitch_max = pl->neck_pitch;
        }
        if (L->neck4_bis >= 0 && pl->motion == 11 && pl->anim_use_pl00 && (pl->anim_flags & 0x80)) {
            if (L->steht_auf_von < 0) L->steht_auf_von = f;
            if (pl->sub_state_2 != 2) L->steht_auf_bis = f;
            if (L->steht_auf_ende < 0) {
                if (L->steht_auf_letzt >= 0 && (int)pl->anim_frame != L->steht_auf_letzt) L->steht_auf_schritte++;
                L->steht_auf_letzt = (int)pl->anim_frame;
                if (pl->anim_frame >= 24) L->steht_auf_ende = f;      /* PL00.EDD Clip 11 = 25 Bilder */
            }
        }
        {
            long dx = pl->x - RE15_IT_COUCH_X, dz = pl->z - RE15_IT_COUCH_Z;
            long d = labs(dx) + labs(dz);
            if (d < L->pl_min_dist_couch) L->pl_min_dist_couch = (int32_t)d;
        }
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            const re15_actor_t *a = &g_actors[s];
            if (!a->active || !a->type) continue;
            if (!L->gesehen[s]) { L->gesehen[s] = 1; L->z_erst[s] = L->z_min[s] = L->z_max[s] = a->z; }
            if (a->z < L->z_min[s]) L->z_min[s] = a->z;
            if (a->z > L->z_max[s]) L->z_max[s] = a->z;
            if ((a->anim_flags & 0x1000) || (a->grid_id & 0x0f) == 1) L->kriech[s] = 1;
        }
        if ((f % 30) == 0) gegner_dump("lauf", f);
        const re15_actor_t *ir = &g_actors[RE15_IT_IRONS_SLOT];
        if (ir->active && ir->type == RE15_IT_IRONS_TYP) {
            if (ir->motion == 5) L->irons_clip5 = 1;
            if (ir->motion == 2 && L->irons_fall_bild < 0) { L->irons_fall_bild = ir->anim_frame; L->irons_clip2 = 1; }
        }
        if (g_room_change.pending) {
            L->raumwechsel = 1; L->ziel = g_room_change.room_id; L->ziel_cut = g_room_change.target_cut;
            L->ziel_x = g_room_change.x; L->ziel_z = g_room_change.z;
            if (bis_tuer) break;
        }
    }
}

/* ---- Teil: tuer1060 ---------------------------------------------------------------------------- */
static void teil_tuer1060(void)
{
    printf("== tuer1060 ==\n");
    struct { int k, t, i, soll; } tab[] = {
        /* (9,71) (3,94) (9,73) -> gesperrt */
        { 0, 0, 0, 1 }, { 0, 1, 0, 0 }, { 1, 0, 0, 1 }, { 1, 1, 0, 1 },
        { 0, 0, 1, 0 }, { 0, 1, 1, 0 }, { 1, 0, 1, 0 }, { 1, 1, 1, 0 },
    };
    for (unsigned i = 0; i < sizeof tab / sizeof tab[0]; i++) {
        flags_leeren();
        if (tab[i].k) re15_game_flag_set(9, 71, 1);
        if (tab[i].t) re15_game_flag_set(3, 94, 1);
        if (tab[i].i) re15_game_flag_set(9, 73, 1);
        PRUEF(re15_tuer1060_sperre_aktiv() == tab[i].soll, "(9,71)=%d (3,94)=%d (9,73)=%d -> gesperrt %d",
              tab[i].k, tab[i].t, tab[i].i, tab[i].soll);
    }
    /* Raum: gesperrt -> Slot 2 Text-Platz msg 1; frei -> Tuer; Elza 1061 nie */
    flags_leeren(); re15_game_flag_set(9, 71, 1); re15_game_flag_set(3, 94, 1);
    if (room_boot(0x1060, 27100, 25400, 2112, 5) == 0) {
        const re15_aot_t *a = &g_aot.slots[RE15_TUER1060_SLOT];
        PRUEF(re15_tuer1060_gesperrt() == 1, "ROOM1060 (9,71)=1,(9,73)=0: Sperre installiert");
        PRUEF(a->active && a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == RE15_TUER1060_MSG_ID,
              "Slot 2 = Text-Platz msg %d (type %d id %d)", RE15_TUER1060_MSG_ID, a->type, a->event_id);
        PRUEF(g_aot.slots[0].type == RE15_AOT_TYPE_DOOR && g_aot.slots[1].type == RE15_AOT_TYPE_DOOR,
              "Slots 0/1 (Etage 8 / 4) bleiben Tueren");
        int n = 0; const uint8_t *m = re15_tuer1060_meldung(&n);
        PRUEF(n == 36 && m[0] == 0x04 && m[1] == 0x02 && m[n-2] == 0x01 && m[n-1] == 0x00,
              "Nachricht: Kopf 04 02, Ende 01 00, %d B", n);
        PRUEF(m[2] == 0x25 && m[3] == 0x00 && m[4] == 0x44 && m[20] == 0x1f && m[31] == 0x57 && m[33] == 0x57,
              "Nachricht: 'I have ... Chief ...' Glyphen (I=0x25, h=0x44, C=0x1f, .=0x57)");
    }
    re15_game_flag_set(9, 73, 1);
    if (room_boot(0x1060, 27100, 25400, 2112, 5) == 0) {
        PRUEF(re15_tuer1060_gesperrt() == 0 && g_aot.slots[2].type == RE15_AOT_TYPE_DOOR,
              "(9,73)=1: Slot 2 bleibt die Original-Tuer");
    }
    flags_leeren();
    if (room_boot(0x1061, 27100, 25400, 2112, 5) == 0)
        PRUEF(re15_tuer1060_gesperrt() == 0, "ROOM1061 (Elza): keine Sperre");
}

/* ---- Teil: programme (Opcode-Walk aller Vorlagen) ------------------------------------------- */
static int op_len(uint8_t op)
{
    switch (op) {
    case 0x00: return 1; case 0x01: return 2; case 0x02: return 1; case 0x09: return 4;
    case 0x0d: return 6; case 0x0e: return 2; case 0x11: return 4; case 0x12: return 2;
    case 0x21: return 4; case 0x22: return 4; case 0x24: return 4; case 0x29: return 2;
    case 0x2b: return 4; case 0x2e: return 3; case 0x2f: return 4; case 0x30: return 1;
    case 0x34: return 4; case 0x36: return 12; case 0x37: return 4; case 0x39: return 4;
    case 0x3b: return 32; case 0x3c: return 2; case 0x3f: return 4; case 0x40: return 8;
    case 0x41: return 10; case 0x42: return 1; case 0x43: return 4; case 0x44: return 20;
    case 0x47: return 2; case 0x46: return 10; case 0x4b: return 3; case 0x18: return 2;
    default: return 0;
    }
}
static int frei_zone7(int bit)
{
    static const int frei[] = { 40,41,42,43,46,47,48,49,75,76,77,78,79, 83,84,90,91,95 };
    for (unsigned i = 0; i < sizeof frei / sizeof frei[0]; i++) if (frei[i] == bit) return 1;
    return 0;
}
static void teil_programme(void)
{
    printf("== programme ==\n");
    for (int w = 0; w <= 16; w++) {
        int n = 0; const uint8_t *p = re15_irons_tod_programm(w, &n);
        if (!p) { PRUEF(0, "Programm %d fehlt", w); continue; }
        int o = 0, ok = 1, n_em = 0, n_door = 0, letzte = -1, bad_bit = -1, tiefe = 0, n_gosub9 = 0, bad_se = -1;
        while (o < n) {
            int l = op_len(p[o]);
            if (l <= 0) { ok = 0; printf("  Programm %d: unbekanntes Opcode 0x%02x @+%d\n", w, p[o], o); break; }
            if (p[o] == 0x44) { n_em++; if (p[o+7] != 0xff && !frei_zone7(p[o+7])) bad_bit = p[o+7]; }
            if (p[o] == 0x3b) { n_door++; }
            if (p[o] == 0x36 && p[o+1] != 0x02) bad_se = p[o+1];
            if (p[o] == 0x18 && p[o+1] == 0x09) n_gosub9++;
            if (p[o] == 0x0d || p[o] == 0x11) tiefe++;
            if (p[o] == 0x0e || p[o] == 0x12) tiefe--;
            letzte = p[o];
            o += l;
        }
        PRUEF(ok && o == n, "Programm %d: Opcode-Walk schliesst exakt (%d/%d B)", w, o, n);
        PRUEF(tiefe == 0, "Programm %d: For/Do-Bloecke ausgeglichen", w);
        if (w == 0 || w == 1 || w == 4 || w == 8 || w == 9 || w == 10 || w == 13 || w == 14 || w == 16)
            PRUEF(letzte == 0x01, "Programm %d endet mit Evt_end", w);
        PRUEF(bad_bit < 0, "Programm %d: Tot-Bits aus dem freien Bereich (%d)", w, bad_bit);
        PRUEF(bad_se < 0, "Programm %d: jedes Se_on traegt Bank 2 wie alle 284 ausgelieferten (kein Port-Bankbyte): %d", w, bad_se);
        if (w == 0) {
            PRUEF(p[0] == 0x22 && p[1] == 9 && p[2] == 73 && p[3] == 1, "Programm 0: erstes Opcode Set(9,73)=1");
            const uint8_t *d = p + n - 36;
            PRUEF(d[0] == 0x3b && d[23] == 0x13 && d[24] == 0 && p[n-4] == 0x47 && p[n-3] == RE15_IT_TUER_SLOT,
                  "Programm 0: Door_aot_set -> Raum 0x13 Cut 0 + Aot_on Slot %d", RE15_IT_TUER_SLOT);
        }
        if (w == 1) { PRUEF(n_em == 5 && n_door == 1 && p[n-36+23] == 0x04 && p[n-36+24] == 1, "Programm 1: 5 Records, Tuer -> 1040 Cut 1"); }
        if (w == 4) { PRUEF(p[n-36+23] == 0x03 && p[n-36+24] == 7, "Programm 4: Tuer -> 1030 Cut 7"); }
        if (w == 6) { PRUEF(n_em == 5, "Programm 6: 5 Kopien aus 1070"); }
        if (w == 2 || w == 14) { PRUEF(n_em == 5, "Programm %d: 5 Auffuell-Records ROOM1040", w); }
        if (w == 7 || w == 15 || w == 16) {
            if (w == 16) {
                PRUEF(n_em == 3 && p[n-36+23] == 0x1c && p[n-36+24] == 13, "Programm 16: 3 Kriecher, Tuer -> 11C0 (Raum 0x1c) Cut 13");
                PRUEF(n_gosub9 == 4, "Programm 16: viermal Gosub sub09 wie ROOM1030 sub08 (@0x0279E/@0x027A4/@0x027B4/@0x027BA): %d", n_gosub9);
            }
            /* die Zeilen der Original-Szene liegen woertlich im Programm: gegen die RDT-Bytes pruefen */
            if (rdt_laden(0x1030) == 0) {
                static const struct { int w; unsigned off; int n; } z[] = {
                    { 7, 0x02776, 12 }, { 15, 0x0276C, 10 }, { 15, 0x02782, 12 }, { 15, 0x0278E, 8 }, { 16, 0x02796, 8 } };
                for (unsigned k = 0; k < sizeof z / sizeof z[0]; k++) {
                    if (z[k].w != w) continue;
                    int gefunden = 0;
                    for (int a = 0; a + z[k].n <= n; a++) if (!memcmp(p + a, s_raw + z[k].off, (size_t)z[k].n)) { gefunden = 1; break; }
                    PRUEF(gefunden, "Programm %d traegt ROOM1030 sub08 @0x%05X (%d B) woertlich", w, z[k].off, z[k].n);
                }
                PRUEF(s_raw[0x027E0 + 0x24] == 0x3e && s_raw[0x027E0 + 0x26] == 0x0f && s_raw[0x027E0 + 0x28] == 0x05,
                      "ROOM1030 sub09 @0x02804 Member_cmp member 0x0f == 5 (Warte-Stempel)");
            }
        }
        if (w == 8) { PRUEF(n_em == 1 && p[n-36+23] == 0x15 && p[n-36+24] == 7, "Programm 8: Marvin, Tuer -> 1150 Cut 7"); }
    }
    /* Nachrichten vorhanden, Kopf/Ende der Dialogform */
    int ids1150[] = { 22,23,24,25,26,27,28,29 }, ids11c0[] = { 10,11,12,13,14 };
    for (int i = 0; i < 8; i++) { int n; const uint8_t *m = re15_irons_tod_meldung(0x1150, ids1150[i], &n);
        PRUEF(m && n > 16 && m[0] == 0x04 && m[2] == 0x05 && m[n-4] == 0x04 && m[n-3] == 0x01, "ROOM1150 msg %d Dialogform (%d B)", ids1150[i], n); }
    for (int i = 0; i < 5; i++) { int n; const uint8_t *m = re15_irons_tod_meldung(0x11C0, ids11c0[i], &n);
        PRUEF(m && n > 16 && m[0] == 0x04 && m[2] == 0x05 && m[n-4] == 0x04 && m[n-3] == 0x01, "ROOM11C0 msg %d Dialogform (%d B)", ids11c0[i], n); }
}

/* ---- Teil: zaehlung ---------------------------------------------------------------------------- */
static void szene_flags(void) { flags_leeren(); re15_game_flag_set(9, 71, 1); re15_game_flag_set(3, 94, 1); }
static void teil_zaehlung(void)
{
    printf("== zaehlung ==\n");
    szene_flags();
    PRUEF(re15_irons_tod_lebend(0) == 5 && re15_irons_tod_lebend(1) == 5, "alle Tot-Bits 0: 5 + 5 lebend");
    re15_game_flag_set(7, 0xd3, 1); re15_game_flag_set(7, 0xd5, 1);          /* 2 von 1140 tot */
    for (int i = 0; i < 5; i++) re15_game_flag_set(7, 0xc6 + i, 1);         /* 1070 komplett tot */
    PRUEF(re15_irons_tod_lebend(0) == 3 && re15_irons_tod_lebend(1) == 0, "2 tot in 1140 -> 3 lebend; 1070 -> 0");
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
    if (room_boot(0x1150, -17150, -11960, 1600, 0) == 0) {
        PRUEF(re15_irons_tod_zustand() == RE15_IT_SZENE, "Szene gestartet (Zustand %d)", re15_irons_tod_zustand());
        PRUEF(re15_game_flag_get(9, 74) == 1 && re15_game_flag_get(9, 76) == 0 && re15_game_flag_get(9, 75) == 1,
              "(9,74)=1 (1130 bekommt Spawns), (9,76)=0 (1070 leer), (9,75)=1");
        int quelle_tot = 1; for (int i = 0; i < 5; i++) if (!re15_game_flag_get(7, 0xd3 + i)) quelle_tot = 0;
        PRUEF(quelle_tot, "alle 1140-Records stillgelegt (umgezogen oder schon tot)");
        PRUEF(re15_game_flag_get(7, 40) == 1 && re15_game_flag_get(7, 41) == 0 && re15_game_flag_get(7, 42) == 1 &&
              re15_game_flag_get(7, 43) == 0 && re15_game_flag_get(7, 46) == 0,
              "1130-Kopien: 40/42 still (Quelle war tot), 41/43/46 scharf");
        int kopie1030_still = 1; static const int b[5] = { 47,48,49,75,76 };
        for (int i = 0; i < 5; i++) if (!re15_game_flag_get(7, b[i])) kopie1030_still = 0;
        PRUEF(kopie1030_still, "1030-Kopien alle still (1070 war leer)");
        /* 1040: alle 20 Raum-Records leben -> keine Auffuellung (alle fuenf Auffuell-Records still) */
        static const int a[5] = { 83,84,90,91,95 };
        int still = 0; for (int i = 0; i < 5; i++) still += re15_game_flag_get(7, a[i]);
        PRUEF(re15_irons_tod_lebend_1040() == 20 && still == 5, "1040: 20 Raum-Records leben, Auffuellung 0 (still %d/5)", still);
    }
    /* 1040: nur 2 der 20 Raum-Records leben -> drei Auffuell-Records scharf (83,84,90), zwei still (91,95) */
    szene_flags();
    for (int i = 2; i < 20; i++) re15_game_flag_set(7, 0x14 + i, 1);
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
    if (room_boot(0x1150, -17150, -11960, 1600, 0) == 0) {
        PRUEF(re15_irons_tod_lebend_1040() == 2, "1040: 2 Raum-Records leben");
        PRUEF(!re15_game_flag_get(7, 83) && !re15_game_flag_get(7, 84) && !re15_game_flag_get(7, 90) &&
              re15_game_flag_get(7, 91) && re15_game_flag_get(7, 95), "1040: Auffuellung 3 (83,84,90 scharf; 91,95 still)");
    }
    /* 1040: alle 20 tot -> fuenf Auffuell-Records scharf */
    szene_flags();
    for (int i = 0; i < 20; i++) re15_game_flag_set(7, 0x14 + i, 1);
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
    if (room_boot(0x1150, -17150, -11960, 1600, 0) == 0) {
        int scharf = 0; static const int a[5] = { 83,84,90,91,95 };
        for (int i = 0; i < 5; i++) scharf += !re15_game_flag_get(7, a[i]);
        PRUEF(re15_irons_tod_lebend_1040() == 0 && scharf == 5, "1040: 0 Raum-Records leben -> Auffuellung 5 (%d)", scharf);
    }
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
}

/* ---- Teil: szene (ROOM1150) ------------------------------------------------------------------ */
static void teil_szene(void)
{
    printf("== szene ==\n");
    szene_flags();
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
    if (room_boot(0x1150, -17150, -11960, 1600, 0) != 0) return;
    PRUEF(re15_irons_tod_zustand() == RE15_IT_SZENE && faeden_im_programm() == 1, "Szene: genau ein Faden");
    const re15_actor_t *ir = &g_actors[RE15_IT_IRONS_SLOT];
    PRUEF(ir->active && ir->type == 0x45, "Irons liegt im Raum (Slot 1, Typ 0x45)");
    int knall0 = g_test_tuer_se_count;
    lauf_t L; lauf(&L, 4000, 1);
    PRUEF(re15_game_flag_get(9, 73) == 1, "(9,73)=1 nach dem ersten Opcode");
    PRUEF(L.pl_min_dist_couch < 400, "Leon rennt zur Liege (Minimalabstand %d)", L.pl_min_dist_couch);
    PRUEF(L.kniet_gesehen, "Leon kniet (PL00 Clip 11)");
    printf("  Nachrichten:"); for (int i = 0; i < L.n_msgs; i++) printf(" %d", L.msgs[i]); printf("\n");
    PRUEF(L.n_msgs >= 8 && L.msgs[0] == 22 && L.msgs[L.n_msgs-1] == 29, "Nachrichten 22 .. 29 der Reihe nach");
    PRUEF(L.irons_clip5, "Irons Arm ausgestreckt (Clip 5)");
    printf("  Kopfschuetteln: Bilder %d..%d (%d), Gier %d..%d, Neigung %d..%d; Aufstehen Bilder %d..%d\n", L.neck4_von, L.neck4_bis,
           L.neck4_bis - L.neck4_von + 1, L.neck_yaw_min, L.neck_yaw_max, L.neck_pitch_min, L.neck_pitch_max, L.steht_auf_von, L.steht_auf_bis);
    /* M3 "steht dann langsam wieder auf": Clip 11 rueckwaerts im HALBTAKT (Plc_flg 0x90; Tor @0x80030670-80, Kippbit
     * @0x800306c4) = 24 Schritte in ~48 Bildern statt 24 (vorher gemessen: 25 Bilder, Abnahme s1150 F1163..F1188). */
    printf("  Aufstehen: Beginn Bild %d, Clipende Bild %d (%d Bilder), %d Schritte\n", L.steht_auf_von, L.steht_auf_ende,
           L.steht_auf_ende - L.steht_auf_von, L.steht_auf_schritte);
    PRUEF(L.steht_auf_von >= 0 && L.steht_auf_ende > L.steht_auf_von && L.steht_auf_schritte == 24 &&
          L.steht_auf_ende - L.steht_auf_von >= 46 && L.steht_auf_ende - L.steht_auf_von <= 50,
          "Leon steht LANGSAM auf: 24 Clip-Schritte in %d Bildern (Halbtakt)", L.steht_auf_ende - L.steht_auf_von);
    PRUEF(L.irons_clip2 && L.irons_fall_bild >= 72, "Irons' Arm faellt: Clip 2 ab Bild %d", L.irons_fall_bild);
    PRUEF(ir->motion == 2 && ir->anim_frame == 89 && ir->sub_state_2 == 2, "Irons tot: Clip 2 Bild 89 gehalten (mo=%d bild=%d ph=%d)", ir->motion, ir->anim_frame, ir->sub_state_2);
    PRUEF(L.raumwechsel && L.ziel == 0x1130 && L.ziel_cut == 0 && L.ziel_x == RE15_IT_PARK_1130_X,
          "Schnitt nach ROOM1130 Cut 0 (Parkplatz) nach %d Bildern", L.bilder);
    PRUEF(g_scd.player_mode == 2 && re15_game_flag_get(2, 7) && re15_game_flag_get(1, 27), "Spieler bleibt skriptgefuehrt");
    PRUEF(g_test_tuer_laden_count >= 1 && g_test_tuer_se_count == knall0 + 1 && g_test_tuer_se_last == 0 &&
          !re15_game_flag_get(5, RE15_IT_SIG_KNALL_TUER),
          "der Knall am Szenenende: Knall-Bank geladen, genau ein Satz 0 gespielt (Signal (5,28) verbraucht): %d", g_test_tuer_se_count - knall0);
    /* Variante: 1140 leer -> direkt 1040 */
    szene_flags(); for (int i = 0; i < 5; i++) re15_game_flag_set(7, 0xd3 + i, 1);
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
    if (room_boot(0x1150, -17150, -11960, 1600, 0) == 0) {
        lauf(&L, 4000, 1);
        PRUEF(L.raumwechsel && L.ziel == 0x1040 && L.ziel_cut == 1, "1140 leer: Schnitt direkt nach ROOM1040 Cut 1");
    }
    /* Wiederbetreten: (9,73)=1 -> keine Szene, Irons tot */
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
    if (room_boot(0x1150, -17150, -11960, 1600, 0) == 0) {
        PRUEF(re15_irons_tod_zustand() == RE15_IT_AUS && faeden_im_programm() == 0, "(9,73)=1: keine zweite Szene");
        for (int f = 0; f < 10; f++) frame(0, 0);
        PRUEF(ir->motion == 2 && ir->anim_frame == 89 && ir->state == 4 && ir->sub_state_2 == 2, "Irons bleibt tot (Clip 2 Bild 89)");
    }
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
}

/* ---- Teil: montage_1130 ------------------------------------------------------------------------ */
static void teil_montage_1130(void)
{
    printf("== montage_1130 ==\n");
    szene_flags(); re15_game_flag_set(9, 73, 1); re15_game_flag_set(9, 74, 1);
    re15_game_flag_set(7, 40, 1);                  /* eine Kopie still -> 4 erscheinen */
    re15_irons_tod_zustand_setzen(RE15_IT_SZENE);
    if (room_boot(0x1130, RE15_IT_PARK_1130_X, RE15_IT_PARK_1130_Z, 0, 0) != 0) return;
    PRUEF(re15_irons_tod_zustand() == RE15_IT_S1130 && faeden_im_programm() == 1, "1130: Montage-Programm laeuft");
    PRUEF(re15_irons_tod_sub01_gesperrt() == 1, "sub01-Reseed gesperrt");
    int knall0 = g_test_tuer_se_count;
    lauf_t L; lauf(&L, 600, 1);
    PRUEF(g_test_tuer_se_count == knall0 + 1 && g_test_tuer_se_last == 0, "1130: lautes Tuerknallen (Knall-Bank Satz 0) genau einmal: %d", g_test_tuer_se_count - knall0);
    PRUEF(aktive_gegner(-1) == 4, "4 Zombies erschienen (Kopie 40 still): %d", aktive_gegner(-1));
    PRUEF(aktive_gegner(0x10) == 2 && aktive_gegner(0x11) == 2, "Typen 0x10 x2, 0x11 x2");
    PRUEF(s_shown == 0 || g_scd.cam_id == 0, "Kamera Cut 0 (angezeigt %d, angefordert %d)", s_shown, (int)g_scd.cam_id);
    PRUEF(L.raumwechsel && L.ziel == 0x1040 && L.ziel_cut == 1 && L.ziel_x == RE15_IT_PARK_1040_X, "Schnitt nach ROOM1040 Cut 1 nach %d Bildern", L.bilder);
    /* Nachspawn beim spaeteren Betreten */
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
    re15_game_flag_set(7, 41, 1);                  /* eine im Montage-Lauf getoetet */
    if (room_boot(0x1130, -800, -5000, 0, 0) == 0) {
        for (int f = 0; f < 5; f++) frame(0, 0);
        PRUEF(aktive_gegner(-1) == 3, "Nachspawn: 3 Zombies (40, 41 still): %d", aktive_gegner(-1));
        PRUEF(re15_irons_tod_sub01_gesperrt() == 0 && g_room_change.pending == 0, "normaler Raum: sub01 frei, keine Tuer");
    }
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
}

/* Die fuenf Auffuell-Bits so setzen, wie umzug_vorbereiten es bei `lebend` lebenden Raum-Records tut. */
static void auf_1040(int lebend)
{
    static const int a[5] = { 83,84,90,91,95 };
    for (int i = 0; i < 5; i++) re15_game_flag_set(7, (uint8_t)a[i], (lebend + i < 5) ? 0 : 1);
    re15_game_flag_set(9, 75, 1);
}

/* Nachbesserung 1 M1/M2/M4: nach dem ersten Takt stehen ALLE Zombies hinter dem Tor in Navigations-Zone 0 (block.blk
 * ROOM1040 @0x0FF4: x -27100..-23300; z >= 1200 = hinter dem Tor) auf den Plaetzen der Aufstellung (Form ROOM1030
 * sub00 @0x020C6..@0x0216E) und blicken zum Tor (Gierung 1024). */
static void aufstellung_pruefen(const char *fall)
{
    static const int32_t px[5] = { -24700, -25700, -25100, -24500, -25900 }, pz[5] = { 1600, 2300, 3200, 4100, 4800 };
    int n = 0, ok = 0;
    for (int sl = 1; sl < RE15_ACTOR_MAX; sl++) {
        const re15_actor_t *a = &g_actors[sl];
        if (!a->active || a->type != 0x16) continue;
        n++;
        for (int k = 0; k < 5; k++)
            if (labs((long)a->x - px[k]) < 300 && labs((long)a->z - pz[k]) < 300) { ok++; break; }
    }
    PRUEF(n == 5 && ok == 5, "%s: alle fuenf hinter dem Tor aufgestellt (Zone 0): %d von %d", fall, ok, n);
}
/* Durchgang je Zombie (z < Tor-z -360, NACHDEM er hinter dem Tor stand) ueber den Bilanz-Haken, und die Spieler-HP
 * ueber die ganze Montage (Abnahme: hp 100 -> 60 bei offenem Tor). Messung an der exe: Tor zu letzter Durchgang
 * Bild 273 von 451, Tor offen Bild 239 von 276 (Dossier §9.1) -> der letzte muss vor dem Schnitt durch sein. */
static void durchgang_pruefen(const char *fall, const lauf_t *L)
{
    int n = 0, letzt = -1, durch = re15_irons_tod_bilanz_1040(&n, &letzt);
    int hp0 = 0, hpmin = 0; re15_irons_tod_hp(&hp0, &hpmin);
    PRUEF(n == 5 && durch == 5 && letzt >= 0 && letzt < L->bilder && !re15_irons_tod_1040_kappe(),
          "%s: alle fuenf kommen durchs Tor - %d von %d, letzter bei Bild %d, Schnitt nach %d Bildern (Kappe %d)", fall, durch, n, letzt,
          L->bilder, re15_irons_tod_1040_kappe());
    PRUEF(hp0 == 100 && hpmin == 100, "%s: Spieler-HP waehrend der Montage unveraendert (%d -> min %d; hinter der Kamera angehalten: %d)",
          fall, hp0, hpmin, re15_irons_tod_1040_angehalten());
}

/* ---- Teil: montage_1040 ------------------------------------------------------------------------ */
static void teil_montage_1040(void)
{
    printf("== montage_1040 ==\n");
    szene_flags(); re15_game_flag_set(9, 73, 1); re15_game_flag_set(9, 76, 1); auf_1040(20);
    re15_irons_tod_zustand_setzen(RE15_IT_S1130);
    re15_irons_tod_bilanz_reset();
    if (room_boot(0x1040, RE15_IT_PARK_1040_X, RE15_IT_PARK_1040_Z, 1024, 1) != 0) return;
    PRUEF(re15_irons_tod_zustand() == RE15_IT_S1040 && faeden_im_programm() == 1, "1040: Montage-Programm laeuft");
    int32_t y0 = g_scd.props[0].y;
    int n0 = aktive_gegner(-1);
    int knall0 = g_test_tuer_se_count;
    frame(0, 0);
    aufstellung_pruefen("Tor zu");
    lauf_t L; lauf(&L, RE15_IT_1040_KAPPE + 200, 1);   /* Kappe 1000 + Tor-Vorlauf */
    durchgang_pruefen("Tor zu", &L);
    PRUEF(g_test_tuer_se_count == knall0 + 1 && g_test_tuer_se_last == 1, "1040: der Knall wie ROOM1030 (Knall-Bank Satz 1) genau einmal: %d", g_test_tuer_se_count - knall0);
    PRUEF(y0 == 0 && g_scd.props[0].y == -5400, "Rolltor von y=%d auf y=%d (sub08 135 x -40)", y0, (int)g_scd.props[0].y);
    PRUEF(re15_game_flag_get(4, 5) == 1 && re15_game_flag_get(4, 4) == 1, "(4,5)=1 (4,4)=1 Tor offen");
    PRUEF(n0 == 5 && aktive_gegner(0x16) == 5, "5 Zombies (Gleichzeitig-Limit 5): %d", aktive_gegner(0x16));
    PRUEF(L.raumwechsel && L.ziel == 0x1030 && L.ziel_cut == 7, "Schnitt nach ROOM1030 Cut 7 ((9,76)=1) nach %d Bildern", L.bilder);
    /* Tor schon offen, 1070 leer: keine Fahrt, Cut 6 */
    szene_flags(); re15_game_flag_set(9, 73, 1); re15_game_flag_set(4, 5, 1); auf_1040(20);
    re15_irons_tod_zustand_setzen(RE15_IT_S1130);
    re15_irons_tod_bilanz_reset();
    if (room_boot(0x1040, RE15_IT_PARK_1040_X, RE15_IT_PARK_1040_Z, 1024, 1) == 0) {
        frame(0, 0);
        aufstellung_pruefen("Tor offen");
        lauf(&L, RE15_IT_1040_KAPPE + 200, 1);
        durchgang_pruefen("Tor offen", &L);
        PRUEF(g_scd.props[0].y == -5400, "Tor war offen: bleibt bei y=-5400 (%d)", (int)g_scd.props[0].y);
        PRUEF(aktive_gegner(0x16) == 5, "Tor war offen: 5 Zombies (%d)", aktive_gegner(0x16));
        PRUEF(L.raumwechsel && L.ziel == 0x1030 && L.ziel_cut == 6 && L.bilder < RE15_IT_1040_KAPPE, "Schnitt nach ROOM1030 Cut 6 nach %d Bildern", L.bilder);
    }
    /* Tor offen und der Spieler hat vorher 18 der 20 Raum-Zombies getoetet: 2 Raum-Records + 3 Auffuell-Records
     * = wieder fuenf (NUTZER-VORGABE "Wenn es bereits offen ist, kommen nur 5 Zombies"). */
    szene_flags(); re15_game_flag_set(9, 73, 1); re15_game_flag_set(4, 5, 1);
    for (int i = 2; i < 20; i++) re15_game_flag_set(7, 0x14 + i, 1);
    auf_1040(2);
    re15_irons_tod_zustand_setzen(RE15_IT_S1130);
    re15_irons_tod_bilanz_reset();
    if (room_boot(0x1040, RE15_IT_PARK_1040_X, RE15_IT_PARK_1040_Z, 1024, 1) == 0) {
        int raum = aktive_gegner(0x16);
        frame(0, 0);
        aufstellung_pruefen("18 tot");
        lauf(&L, RE15_IT_1040_KAPPE + 200, 1);
        PRUEF(raum == 2 && aktive_gegner(0x16) == 5, "18 tot: %d Raum-Zombies + Auffuellung = %d Zombies", raum, aktive_gegner(0x16));
        durchgang_pruefen("18 tot (2 Raum + 3 Auffuellung)", &L);
        PRUEF(L.raumwechsel && L.ziel == 0x1030, "Schnitt nach ROOM1030 nach %d Bildern", L.bilder);
        /* spaeteres Betreten: die Auffuell-Zombies stehen wieder da (einer inzwischen getoetet) */
        re15_irons_tod_zustand_setzen(RE15_IT_AUS);
        re15_game_flag_set(7, 84, 1);
        if (room_boot(0x1040, -21008, -13134, 2112, 5) == 0) {
            for (int f = 0; f < 5; f++) frame(0, 0);
            PRUEF(aktive_gegner(0x16) == 4, "Nachspawn 1040: 2 Raum-Zombies + 2 Auffuell-Zombies (84 tot): %d", aktive_gegner(0x16));
            PRUEF(g_room_change.pending == 0 && re15_irons_tod_sub01_gesperrt() == 0, "normaler Raum: keine Tuer, sub01 frei");
        }
    }
    /* ohne Szene ((9,75)=0): kein Nachspawn, auch wenn die Auffuell-Bits 0 sind */
    flags_leeren();
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
    if (room_boot(0x1040, -21008, -13134, 2112, 5) == 0) {
        for (int f = 0; f < 5; f++) frame(0, 0);
        PRUEF(aktive_gegner(0x16) == 5 && faeden_im_programm() == 0, "ohne Szene: nur die Raum-Zombies (%d), kein Port-Programm", aktive_gegner(0x16));
    }
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
}

/* Die drei Zusatz-Zombies (erschienen bei z -24800 im Warte-Rechteck Slot 5 @0x01CF2) muessen das Kriech-Bit
 * bekommen haben (sub09 @0x0280A..@0x02814: member 0x10 |= 0x1000 -> Kriech-Wurzel grid&0xf == 1) und bis
 * zum Schnitt durchs Tor sein: z > -22500 = im Rechteck VOR dem Tor (Slot 4 @0x01CDE z -22500..-20300). */
static void kriecher_pruefen(const lauf_t *L, const char *fall)
{
    int n = 0, kriecht = 0, durch = 0; long zsum = 0;
    for (int sl = 1; sl < RE15_ACTOR_MAX; sl++) {
        if (!L->gesehen[sl] || L->z_erst[sl] != -24800) continue;
        n++; kriecht += L->kriech[sl]; if (L->z_max[sl] > -22500) durch++;
        zsum += L->z_max[sl];
    }
    PRUEF(n == 3 && kriecht == 3, "%s: drei Zusatz-Zombies, alle drei kriechen (%d/%d)", fall, kriecht, n);
    PRUEF(durch == 3, "%s: alle drei sind beim Schnitt durchs Tor (z > -22500): %d von %d, mittleres z %ld", fall, durch, n, n ? zsum / n : 0);
}

/* ---- Teil: montage_1030 ------------------------------------------------------------------------ */
static void teil_montage_1030(void)
{
    printf("== montage_1030 ==\n");
    szene_flags(); re15_game_flag_set(9, 73, 1); re15_game_flag_set(9, 76, 1);
    re15_game_flag_set(7, 47, 1);                  /* eine 1070-Kopie still -> 4 + 3 */
    re15_irons_tod_zustand_setzen(RE15_IT_S1040);
    if (room_boot(0x1030, RE15_IT_PARK_1030_X, RE15_IT_PARK_1030_Z, 0, 7) != 0) return;
    int n0 = aktive_gegner(-1);
    PRUEF(re15_irons_tod_zustand() == RE15_IT_S1030 && faeden_im_programm() == 1, "1030: Montage-Programm laeuft (%d Raum-Zombies)", n0);
    int knall0 = g_test_tuer_se_count;
    lauf_t L; lauf(&L, 900, 1);
    int n1 = aktive_gegner(-1);
    PRUEF(g_test_tuer_se_count == knall0 + 1 && g_test_tuer_se_last == 0, "1030 Cut 7: Tuerknall (Knall-Bank Satz 0) genau einmal: %d", g_test_tuer_se_count - knall0);
    PRUEF(n1 == n0 + 7, "4 Kopien + 3 Kriecher dazu: %d -> %d", n0, n1);
    PRUEF(g_scd.work_vars[0x12] == 20, "Gleichzeitig-Limit 20 (Save 0x12)");
    PRUEF(L.raumwechsel && L.ziel == 0x11C0 && L.ziel_cut == 13 && L.ziel_x == RE15_IT_PARK_11C0_X, "Schnitt nach ROOM11C0 Cut 13 nach %d Bildern", L.bilder);
    kriecher_pruefen(&L, "(4,15)=0");
    PRUEF(re15_game_flag_get(4, 15) == 1, "(4,15)=1: die Original-Szene des Raums ist damit gelaufen (Tor offen)");
    PRUEF(g_scd.cam_id == 12, "Kamera = Cut 12 (Cut 6 mit aufgebrochenem Tor, Cut_replace @0x0278B): %d", (int)g_scd.cam_id);
    /* Variante: die Original-Szene war schon gelaufen ((4,15)=1) — "noch einmal" */
    szene_flags(); re15_game_flag_set(9, 73, 1); re15_game_flag_set(4, 15, 1);
    re15_irons_tod_zustand_setzen(RE15_IT_S1040);
    if (room_boot(0x1030, RE15_IT_PARK_1030_X, RE15_IT_PARK_1030_Z, 0, 6) == 0) {
        for (int f = 0; f < 3; f++) frame(0, 0);
        PRUEF(g_scd.cam_id == 12, "(4,15)=1: die Tor-Ansicht ist von Anfang an Cut 12 (sub00 hat 6<->12 getauscht @0x01FFD): %d", (int)g_scd.cam_id);
        {   /* kein zweiter Tausch: die Zonen, die sub00 auf 12 umetikettiert hat, tragen am Ende noch 12 */
            int z6 = 0, z12 = 0;
            for (int i = 0; i < s_rdt.zone_count; i++) { if (s_rdt.zones[i].cam_from == 6) z6++; if (s_rdt.zones[i].cam_from == 12) z12++; }
            lauf(&L, 900, 1);
            int n6 = 0, n12 = 0;
            for (int i = 0; i < s_rdt.zone_count; i++) { if (s_rdt.zones[i].cam_from == 6) n6++; if (s_rdt.zones[i].cam_from == 12) n12++; }
            PRUEF(z12 > 0 && n12 == z12 && n6 == z6, "(4,15)=1: Sicht-Zonen bleiben auf Cut 12 (vorher %d/%d, nachher %d/%d Zonen mit cam_from 12/6)", z12, z6, n12, n6);
        }
        kriecher_pruefen(&L, "(4,15)=1");
        PRUEF(L.raumwechsel && L.ziel == 0x11C0 && L.ziel_cut == 13, "(4,15)=1: Schnitt nach ROOM11C0 Cut 13 nach %d Bildern", L.bilder);
    }
    /* Variante: Ada steht nicht mehr an Cut 13 ((4,64)=1) -> aus 1030 direkt zurueck nach 1150 */
    szene_flags(); re15_game_flag_set(9, 73, 1); re15_game_flag_set(4, 64, 1);
    re15_irons_tod_zustand_setzen(RE15_IT_S1040);
    if (room_boot(0x1030, RE15_IT_PARK_1030_X, RE15_IT_PARK_1030_Z, 0, 6) == 0) {
        lauf(&L, 900, 1);
        PRUEF(L.raumwechsel && L.ziel == 0x1150 && L.ziel_cut == 7 && L.ziel_x == RE15_IT_COUCH_X && L.ziel_z == RE15_IT_COUCH_Z,
              "(4,64)=1: Schnitt aus 1030 direkt nach ROOM1150 Cut 7 (Ziel %04X Cut %d)", L.ziel, L.ziel_cut);
        if (room_boot(0x1150, RE15_IT_COUCH_X, RE15_IT_COUCH_Z, RE15_IT_COUCH_YAW, 7) == 0) {
            PRUEF(re15_irons_tod_zustand() == RE15_IT_RUECKKEHR, "(4,64)=1: Rueckkehr-Programm in 1150 (Zustand %d)", re15_irons_tod_zustand());
            for (int f = 0; f < 200; f++) frame(0, 0);
            PRUEF(re15_irons_tod_zustand() == RE15_IT_AUS && g_scd.player_mode == 0, "(4,64)=1: Kette beendet, Steuerung frei");
        }
    }
    szene_flags(); re15_game_flag_set(9, 73, 1); re15_game_flag_set(9, 76, 1); re15_game_flag_set(7, 47, 1);
    /* Nachspawn */
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
    if (room_boot(0x1030, 160, 6145, 0, 0) == 0) {
        int m0 = 0; for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type) m0++;
        for (int f = 0; f < 5; f++) frame(0, 0);
        int m1 = aktive_gegner(-1);
        PRUEF(m1 == m0 + 7 || m1 == 15, "Nachspawn 1030: +7 (%d -> %d)", m0, m1);
        PRUEF(g_room_change.pending == 0 && re15_irons_tod_sub01_gesperrt() == 0, "normaler Raum: keine Tuer, sub01 frei");
    }
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
}

/* ---- Teil: montage_11c0 ------------------------------------------------------------------------ */
static void teil_montage_11c0(void)
{
    printf("== montage_11c0 ==\n");
    szene_flags(); re15_game_flag_set(9, 73, 1);
    re15_irons_tod_zustand_setzen(RE15_IT_S1030);
    if (room_boot(0x11C0, RE15_IT_PARK_11C0_X, RE15_IT_PARK_11C0_Z, RE15_IT_PARK_11C0_YAW, 13) != 0) return;
    PRUEF(re15_irons_tod_zustand() == RE15_IT_S11C0 && faeden_im_programm() == 1, "11C0: Montage-Programm laeuft");
    PRUEF(g_actors[1].active && g_actors[1].type == 0x42, "Ada steht da (sub00, Typ 0x42)");
    /* REIHENFOLGE DER ECHTEN EXE (main.c Tuer-Weg :8168, debug.log: "[irons-tod] ... Montage 11C0" VOR "[rbj] room
     * 11C0 cinematic overlay"): das Raum-RBJ wird NACH dem Installer gebunden und setzt dabei die Marker-Aliase
     * zurueck. Marvins Alias (er spielt Adas Gesten-Record) muss das ueberleben, sonst hat er keine Arm-Geste. */
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    lauf_t L; lauf(&L, 1200, 1);
    {
        const re15_emd_animation_t *ma = re15_actor_rbj_anim(4);
        PRUEF(ma != NULL && ma->clip_count > 15 && ma->clips[15].frame_count > 2,
              "Marvin (Aktor 4) hat den Gesten-Record gebunden: Clip 15 = %d Bilder (Arm streck)",
              ma ? (int)ma->clips[15].frame_count : -1);
    }
    PRUEF(g_actors[4].active && g_actors[4].type == 0x40, "Marvin erschienen (Aktor 4, Typ 0x40)");
    PRUEF(re15_game_flag_get(4, 64) == 0, "(4,64) unberuehrt: die spaetere Ada-Szene bleibt (sub01 gesperrt)");
    printf("  Nachrichten:"); for (int i = 0; i < L.n_msgs; i++) printf(" %d", L.msgs[i]); printf("\n");
    PRUEF(L.n_msgs == 5 && L.msgs[0] == 10 && L.msgs[4] == 14, "Nachrichten 10..14");
    {
        long dx = labs((long)g_actors[4].x - (-10500)), dz = labs((long)g_actors[4].z - (-12300));
        PRUEF(dx + dz > 3000, "Marvin ist aus dem Bild gelaufen (Weg %ld)", dx + dz);
    }
    PRUEF(L.raumwechsel && L.ziel == 0x1150 && L.ziel_cut == 7 && L.ziel_x == RE15_IT_COUCH_X && L.ziel_z == RE15_IT_COUCH_Z,
          "Schnitt zurueck nach ROOM1150 Cut 7 (Couchplatz) nach %d Bildern", L.bilder);
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
}

/* ---- Teil: rueckkehr ---------------------------------------------------------------------------- */
static void teil_rueckkehr(void)
{
    printf("== rueckkehr ==\n");
    szene_flags(); re15_game_flag_set(9, 73, 1);
    re15_irons_tod_zustand_setzen(RE15_IT_S11C0);
    if (room_boot(0x1150, RE15_IT_COUCH_X, RE15_IT_COUCH_Z, RE15_IT_COUCH_YAW, 7) != 0) return;
    PRUEF(re15_irons_tod_zustand() == RE15_IT_RUECKKEHR && faeden_im_programm() == 1, "Rueckkehr-Programm laeuft");
    const re15_actor_t *ir = &g_actors[RE15_IT_IRONS_SLOT];
    PRUEF(ir->active && ir->motion == 2 && ir->anim_frame == 89 && ir->sub_state_2 == 2, "Irons tot bei der Rueckkehr");
    for (int f = 0; f < 200; f++) frame(0, 0);
    PRUEF(re15_irons_tod_zustand() == RE15_IT_AUS, "Kette beendet (Zustand %d)", re15_irons_tod_zustand());
    PRUEF(re15_game_flag_get(2, 7) == 0 && re15_game_flag_get(1, 27) == 0 && g_scd.player_mode == 0,
          "Steuerung frei (pm=%d)", g_scd.player_mode);
    PRUEF(g_room_change.pending == 0, "keine weitere Tuer");
    PRUEF(ir->motion == 2 && ir->anim_frame == 89, "Irons bleibt tot");
}

/* ---- Teil: totenpose + Riegel gegen Fremdstart -------------------------------------------------- */
static void teil_totenpose(void)
{
    printf("== totenpose ==\n");
    /* ohne (9,71): keine Szene */
    flags_leeren(); re15_game_flag_set(3, 94, 1);
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
    if (room_boot(0x1150, -17150, -11960, 1600, 0) == 0) {
        PRUEF(re15_irons_tod_zustand() == RE15_IT_AUS && faeden_im_programm() == 0, "(9,71)=0: keine Szene");
        for (int f = 0; f < 5; f++) frame(0, 0);
        PRUEF(g_actors[1].motion == 3, "Irons lebt (Clip 3 Liege-Loop, mo=%d)", g_actors[1].motion);
    }
    /* ohne (3,94): keine Szene */
    flags_leeren(); re15_game_flag_set(9, 71, 1);
    if (room_boot(0x1150, -17150, -11960, 1600, 0) == 0)
        PRUEF(re15_irons_tod_zustand() == RE15_IT_AUS && faeden_im_programm() == 0, "(3,94)=0: keine Szene");
    /* Elza-Raum 1151: nie */
    szene_flags();
    if (room_boot(0x1151, -17150, -11960, 1600, 0) == 0)
        PRUEF(re15_irons_tod_zustand() == RE15_IT_AUS, "ROOM1151: keine Szene");
    /* Kette unterbrochen (fremder Raum) -> AUS */
    re15_irons_tod_zustand_setzen(RE15_IT_S1130);
    if (room_boot(0x1140, -1300, -13950, 0, 0) == 0)
        PRUEF(re15_irons_tod_zustand() == RE15_IT_AUS && re15_irons_tod_sub01_gesperrt() == 0, "fremder Raum: Kette zurueckgesetzt");
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
}

/* ---- Teil: tot_bleibt_tot — "sich garnicht mehr bewegen - tot" (AUFTRAG Z. 75) ------------------
 * ROOM1150 Slot 2 (@0x00D92 sce 5, Rechteck an der Liege) stempelt beim Ansprechen work_vars[0] = 2
 * (@0x80042f3c); sub01 @0x00EC0 `23 00 00 00 02 00` + @0x00EC6 Ck(3,157)==0 startet dann sub03
 * (@0x00EE4: msg 2 "Irons: I'll be fine. Just worry about yourself for now.", Irons Clip 6 @0x00F24,
 * Liege-Loop @0x00F34). Nach der Todesszene darf das nicht mehr laufen. */
static void teil_tot_bleibt_tot(void)
{
    printf("== tot_bleibt_tot ==\n");
    const re15_actor_t *ir = &g_actors[RE15_IT_IRONS_SLOT];
    /* Gegenprobe: Irons lebt ((9,73)=0, (9,71)=0) -> das Ansprechen startet sub03 */
    flags_leeren(); re15_game_flag_set(3, 94, 1);
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
    if (room_boot(0x1150, RE15_IT_COUCH_X, RE15_IT_COUCH_Z, RE15_IT_COUCH_YAW, 5) == 0) {
        for (int f = 0; f < 5; f++) frame(0, 0);
        g_scd.work_vars[0] = 2;
        int msg2 = 0, clip6 = 0;
        for (int f = 0; f < 150; f++) { frame(0, 0); if (g_scd.message_active && g_scd.message_id == 2) msg2 = 1; if (ir->motion == 6) clip6 = 1; }
        PRUEF(re15_game_flag_get(3, 157) == 1 && msg2 && clip6, "Gegenprobe lebend: Ansprechen -> sub03 ((3,157)=%d msg2=%d Clip6=%d)",
              re15_game_flag_get(3, 157), msg2, clip6);
        PRUEF(re15_irons_tod_sub01_gesperrt() == 0, "lebend: sub01 frei");
    }
    /* tot: (9,73)=1, (3,157)=0 -> nichts */
    szene_flags(); re15_game_flag_set(9, 73, 1);
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
    if (room_boot(0x1150, RE15_IT_COUCH_X, RE15_IT_COUCH_Z, RE15_IT_COUCH_YAW, 5) == 0) {
        for (int f = 0; f < 5; f++) frame(0, 0);
        g_scd.work_vars[0] = 2;
        int msg = 0, bewegt = 0;
        for (int f = 0; f < 150; f++) {
            frame(0, 0);
            if (g_scd.message_active) msg = 1;
            if (ir->motion != 2 || ir->anim_frame != 89 || ir->sub_state_2 != 2) bewegt = 1;
        }
        PRUEF(re15_game_flag_get(3, 157) == 0 && !msg, "tot: Ansprechen startet sub03 NICHT ((3,157)=%d msg=%d)", re15_game_flag_get(3, 157), msg);
        PRUEF(!bewegt, "tot: Irons bleibt 150 Bilder in Clip 2 Bild 89 Phase 2 (mo=%d bild=%d ph=%d)", ir->motion, ir->anim_frame, ir->sub_state_2);
        PRUEF(g_scd.player_mode == 0 && !re15_game_flag_get(2, 7), "tot: Steuerung bleibt frei (pm=%d)", g_scd.player_mode);
    }
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
}

/* ---- Teil: knallbank (die eingebackene Tonbank mit den Lesern des Tuerbank-Laders pruefen) ---- */
#include "../../engine/src/gen/knall_bank.inc"
static void teil_knallbank(void)
{
    printf("== knallbank ==\n");
    const uint8_t *b = k_knall_bank; const uint32_t n = KNALL_BANK_SIZE;
    uint32_t vh_off = (uint32_t)b[0xC30] | ((uint32_t)b[0xC31] << 8) | ((uint32_t)b[0xC32] << 16) | ((uint32_t)b[0xC33] << 24);
    PRUEF(n > 0xC38u && vh_off == 0x10, "Tonteil-Form: %u B, VH @0x%X (Nachspann @0xC30)", (unsigned)n, (unsigned)vh_off);
    re15_vab_t vab; int rc = re15_vab_parse(b + vh_off, 0xC38u - vh_off, &vab);
    PRUEF(rc == 0 && vab.vag_count == 2, "re15_vab_parse rc=%d, %d VAGs", rc, vab.vag_count);
    uint32_t vbd = n - 0xC38u;
    PRUEF(vab.samples[0].size == 9216 && vab.samples[1].size == 8080 && vab.samples[1].offset + vab.samples[1].size == vbd,
          "VAG 1 = 9216 B (DOOR04 Door_exit), VAG 2 = 8080 B (ROOM1030 0x0c), VB %u B", (unsigned)vbd);
    re15_edt_rec_t e0, e1;
    PRUEF(re15_edt_decode(b, 0, &e0) == 0 && re15_edt_decode(b, 1, &e1) == 0 &&
          e0.prog == 0 && e0.tone == 0 && e1.prog == 0 && e1.tone == 1 && !e0.empty && !e1.empty,
          "EDT-Saetze 0/1 -> Programm 0 Tone 0 / Tone 1 (prio %d/%d, Stimme %d/%d)", e0.prio, e1.prio, e0.voice, e1.voice);
    /* Tone-Saetze: VAG-Index 1 bzw. 2 (Tone-Segment Programm 0 @VH+0x20+0x800, Satz 0x20 B, VAG @+0x16) */
    const uint8_t *t0 = b + vh_off + 0x20 + 0x800, *t1 = t0 + 0x20;
    PRUEF(t0[0x16] == 1 && t1[0x16] == 2 && t0[2] == 127 && t1[2] == 127, "Tone 0 -> VAG 1 (vol %d), Tone 1 -> VAG 2 (vol %d)", t0[2], t1[2]);
    for (int i = 0; i < 2; i++) {
        size_t cap = (vab.samples[i].size / 16) * 28;
        int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
        int got = pcm ? re15_vag_adpcm_decode(b + 0xC38u + vab.samples[i].offset, vab.samples[i].size, pcm, cap) : 0;
        int peak = 0; for (int k = 0; k < got; k++) { int v = pcm[k] < 0 ? -pcm[k] : pcm[k]; if (v > peak) peak = v; }
        /* Abtastwerte bis zum ADPCM-Endflag: 16100 (DOOR04 VAG 3) / 14112 (ROOM1030 VAG 6) — dieselben Zahlen wie
         * die Python-Dekodierung im Dossier §2.5. */
        PRUEF(got == (i == 0 ? 16100 : 14112) && peak > 30000, "VAG %d dekodiert: %d Abtastwerte, Spitze %d", i + 1, got, peak);
        free(pcm);
    }
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    re15_game_state_init();
    if (!strcmp(teil, "tuer1060") || !strcmp(teil, "alle")) teil_tuer1060();
    if (!strcmp(teil, "programme") || !strcmp(teil, "alle")) teil_programme();
    if (!strcmp(teil, "zaehlung") || !strcmp(teil, "alle")) teil_zaehlung();
    if (!strcmp(teil, "szene") || !strcmp(teil, "alle")) teil_szene();
    if (!strcmp(teil, "montage_1130") || !strcmp(teil, "alle")) teil_montage_1130();
    if (!strcmp(teil, "montage_1040") || !strcmp(teil, "alle")) teil_montage_1040();
    if (!strcmp(teil, "montage_1030") || !strcmp(teil, "alle")) teil_montage_1030();
    if (!strcmp(teil, "montage_11c0") || !strcmp(teil, "alle")) teil_montage_11c0();
    if (!strcmp(teil, "rueckkehr") || !strcmp(teil, "alle")) teil_rueckkehr();
    if (!strcmp(teil, "totenpose") || !strcmp(teil, "alle")) teil_totenpose();
    if (!strcmp(teil, "tot_bleibt_tot") || !strcmp(teil, "alle")) teil_tot_bleibt_tot();
    if (!strcmp(teil, "knallbank") || !strcmp(teil, "alle")) teil_knallbank();
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
