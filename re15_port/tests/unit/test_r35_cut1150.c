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
#include "re15_tuer1060.h"
#include "re15_irons_tod.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void re15_actor_step_all_walkers(void);

static int g_fail = 0;
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

/* Bis zu `max` Bilder fahren, Nachrichten-Ids und Tuer-Anforderung mitschreiben. */
typedef struct {
    int bilder; int raumwechsel; unsigned ziel; int ziel_cut; int32_t ziel_x, ziel_z;
    int msgs[16]; int n_msgs;
    int kniet_gesehen; int irons_clip5; int irons_fall_bild; int irons_clip2;
    int32_t pl_min_dist_couch;
} lauf_t;

static void lauf(lauf_t *L, int max, int bis_tuer)
{
    memset(L, 0, sizeof *L); L->pl_min_dist_couch = 1 << 30; L->irons_fall_bild = -1;
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
        {
            long dx = pl->x - RE15_IT_COUCH_X, dz = pl->z - RE15_IT_COUCH_Z;
            long d = labs(dx) + labs(dz);
            if (d < L->pl_min_dist_couch) L->pl_min_dist_couch = (int32_t)d;
        }
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
    case 0x47: return 2;
    default: return 0;
    }
}
static int frei_zone7(int bit)
{
    static const int frei[] = { 40,41,42,43,46,47,48,49,75,76,77,78,79 };
    for (unsigned i = 0; i < sizeof frei / sizeof frei[0]; i++) if (frei[i] == bit) return 1;
    return 0;
}
static void teil_programme(void)
{
    printf("== programme ==\n");
    for (int w = 0; w <= 13; w++) {
        int n = 0; const uint8_t *p = re15_irons_tod_programm(w, &n);
        if (!p) { PRUEF(0, "Programm %d fehlt", w); continue; }
        int o = 0, ok = 1, n_em = 0, n_door = 0, letzte = -1, bad_bit = -1, tiefe = 0;
        while (o < n) {
            int l = op_len(p[o]);
            if (l <= 0) { ok = 0; printf("  Programm %d: unbekanntes Opcode 0x%02x @+%d\n", w, p[o], o); break; }
            if (p[o] == 0x44) { n_em++; if (p[o+7] != 0xff && !frei_zone7(p[o+7])) bad_bit = p[o+7]; }
            if (p[o] == 0x3b) { n_door++; }
            if (p[o] == 0x0d || p[o] == 0x11) tiefe++;
            if (p[o] == 0x0e || p[o] == 0x12) tiefe--;
            letzte = p[o];
            o += l;
        }
        PRUEF(ok && o == n, "Programm %d: Opcode-Walk schliesst exakt (%d/%d B)", w, o, n);
        PRUEF(tiefe == 0, "Programm %d: For/Do-Bloecke ausgeglichen", w);
        if (w == 0 || w == 1 || w == 4 || w == 7 || w == 8 || w == 9 || w == 10 || w == 13)
            PRUEF(letzte == 0x01, "Programm %d endet mit Evt_end", w);
        PRUEF(bad_bit < 0, "Programm %d: Tot-Bits aus dem freien Bereich (%d)", w, bad_bit);
        if (w == 0) {
            PRUEF(p[0] == 0x22 && p[1] == 9 && p[2] == 73 && p[3] == 1, "Programm 0: erstes Opcode Set(9,73)=1");
            const uint8_t *d = p + n - 36;
            PRUEF(d[0] == 0x3b && d[23] == 0x13 && d[24] == 0 && p[n-4] == 0x47 && p[n-3] == RE15_IT_TUER_SLOT,
                  "Programm 0: Door_aot_set -> Raum 0x13 Cut 0 + Aot_on Slot %d", RE15_IT_TUER_SLOT);
        }
        if (w == 1) { PRUEF(n_em == 5 && n_door == 1 && p[n-36+23] == 0x04 && p[n-36+24] == 1, "Programm 1: 5 Records, Tuer -> 1040 Cut 1"); }
        if (w == 4) { PRUEF(p[n-36+23] == 0x03 && p[n-36+24] == 7, "Programm 4: Tuer -> 1030 Cut 7"); }
        if (w == 6) { PRUEF(n_em == 5, "Programm 6: 5 Kopien aus 1070"); }
        if (w == 7) { PRUEF(n_em == 3 && p[n-36+23] == 0x1c && p[n-36+24] == 13, "Programm 7: 3 Kriecher, Tuer -> 11C0 (Raum 0x1c) Cut 13"); }
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
    lauf_t L; lauf(&L, 4000, 1);
    PRUEF(re15_game_flag_get(9, 73) == 1, "(9,73)=1 nach dem ersten Opcode");
    PRUEF(L.pl_min_dist_couch < 400, "Leon rennt zur Liege (Minimalabstand %d)", L.pl_min_dist_couch);
    PRUEF(L.kniet_gesehen, "Leon kniet (PL00 Clip 11)");
    printf("  Nachrichten:"); for (int i = 0; i < L.n_msgs; i++) printf(" %d", L.msgs[i]); printf("\n");
    PRUEF(L.n_msgs >= 8 && L.msgs[0] == 22 && L.msgs[L.n_msgs-1] == 29, "Nachrichten 22 .. 29 der Reihe nach");
    PRUEF(L.irons_clip5, "Irons Arm ausgestreckt (Clip 5)");
    PRUEF(L.irons_clip2 && L.irons_fall_bild >= 72, "Irons' Arm faellt: Clip 2 ab Bild %d", L.irons_fall_bild);
    PRUEF(ir->motion == 2 && ir->anim_frame == 89 && ir->sub_state_2 == 2, "Irons tot: Clip 2 Bild 89 gehalten (mo=%d bild=%d ph=%d)", ir->motion, ir->anim_frame, ir->sub_state_2);
    PRUEF(L.raumwechsel && L.ziel == 0x1130 && L.ziel_cut == 0 && L.ziel_x == RE15_IT_PARK_1130_X,
          "Schnitt nach ROOM1130 Cut 0 (Parkplatz) nach %d Bildern", L.bilder);
    PRUEF(g_scd.player_mode == 2 && re15_game_flag_get(2, 7) && re15_game_flag_get(1, 27), "Spieler bleibt skriptgefuehrt");
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
    lauf_t L; lauf(&L, 600, 1);
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

/* ---- Teil: montage_1040 ------------------------------------------------------------------------ */
static void teil_montage_1040(void)
{
    printf("== montage_1040 ==\n");
    szene_flags(); re15_game_flag_set(9, 73, 1); re15_game_flag_set(9, 76, 1);
    re15_irons_tod_zustand_setzen(RE15_IT_S1130);
    if (room_boot(0x1040, RE15_IT_PARK_1040_X, RE15_IT_PARK_1040_Z, 1024, 1) != 0) return;
    PRUEF(re15_irons_tod_zustand() == RE15_IT_S1040 && faeden_im_programm() == 1, "1040: Montage-Programm laeuft");
    int32_t y0 = g_scd.props[0].y;
    int n0 = aktive_gegner(-1);
    lauf_t L; lauf(&L, 900, 1);
    PRUEF(y0 == 0 && g_scd.props[0].y == -5400, "Rolltor von y=%d auf y=%d (sub08 135 x -40)", y0, (int)g_scd.props[0].y);
    PRUEF(re15_game_flag_get(4, 5) == 1 && re15_game_flag_get(4, 4) == 1, "(4,5)=1 (4,4)=1 Tor offen");
    PRUEF(n0 == 5 && aktive_gegner(0x16) == 5, "5 Zombies (Gleichzeitig-Limit 5): %d", aktive_gegner(0x16));
    PRUEF(L.raumwechsel && L.ziel == 0x1030 && L.ziel_cut == 7, "Schnitt nach ROOM1030 Cut 7 ((9,76)=1) nach %d Bildern", L.bilder);
    /* Tor schon offen, 1070 leer: keine Fahrt, Cut 6 */
    szene_flags(); re15_game_flag_set(9, 73, 1); re15_game_flag_set(4, 5, 1);
    re15_irons_tod_zustand_setzen(RE15_IT_S1130);
    if (room_boot(0x1040, RE15_IT_PARK_1040_X, RE15_IT_PARK_1040_Z, 1024, 1) == 0) {
        lauf(&L, 900, 1);
        PRUEF(g_scd.props[0].y == -5400, "Tor war offen: bleibt bei y=-5400 (%d)", (int)g_scd.props[0].y);
        PRUEF(L.raumwechsel && L.ziel == 0x1030 && L.ziel_cut == 6 && L.bilder < 400, "Schnitt nach ROOM1030 Cut 6 nach %d Bildern", L.bilder);
    }
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
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
    lauf_t L; lauf(&L, 900, 1);
    int n1 = aktive_gegner(-1);
    PRUEF(n1 == n0 + 7, "4 Kopien + 3 Kriecher dazu: %d -> %d", n0, n1);
    PRUEF(g_scd.work_vars[0x12] == 20, "Gleichzeitig-Limit 20 (Save 0x12)");
    PRUEF(L.raumwechsel && L.ziel == 0x11C0 && L.ziel_cut == 13 && L.ziel_x == RE15_IT_PARK_11C0_X, "Schnitt nach ROOM11C0 Cut 13 nach %d Bildern", L.bilder);
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
    lauf_t L; lauf(&L, 1200, 1);
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
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
