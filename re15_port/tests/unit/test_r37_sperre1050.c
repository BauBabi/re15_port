/*
 * test_r37_sperre1050.c — Runde 37 Spur F "sperre1050": Riegel.
 *
 * Dossier analysis/befunde_runde37/F_sperre1050.md, Code include/re15_sperre1050_r37.h + engine/src/sperre1050_r37.c,
 * Text-Weg tools/r36_p/texte_bauen.py (UNTERSUCHEN) -> engine/src/gen/story_r36_texte.inc.
 *
 * Teile (Aufruf: test_r37_sperre1050 <teil>):
 *   bedingung  alle 16 Stellungen (3,0xBB) (9,71) (3,94) (9,73): Sperre genau bei (3,0xBB)=1, (9,71)=1, (9,73)=0;
 *              in jeder dieser Stellungen ist auch die 1060-Sperre zu (re15_tuer1060_sperre_aktiv)
 *   raum       ROOM1050 echter Aufbau (main00 + Installer) je Spielabschnitt: Ada-Szene / Ada-Sperrtext / Tuer /
 *              Text-Platz msg 26 (Form, Rechteck, Band, Bytes = 1060-Sperre) / Tuer nach Irons; ROOM1051 (Elza) frei
 *   druck      Aktionstaste vor Slot 4: Endspiel -> "I have to get the Chief first...", Freeze, KEIN Raumwechsel,
 *              keine Tuersequenz, zweiter Druck schliesst, Steuerung frei; Mittelspiel -> ROOM10A0 (Tuer bleibt)
 *   luke       die Luecke selbst (Original-Daten): Gitter ROOM10F0 Slot 1 -> ROOM1090 Band 5, Absprung-Zelle
 *              ROOM1090 @0x01E3C Typwort 0x530E unter dem Steg, Hof-Tuer -> ROOM1050
 *   elza50b1   ROOM50B1 msg 0 nach dem Laden = neuer Text (Kopf/Trenner/Schwanz/Dauer = Original, Zeilen wie das
 *              Original); Sweep aller Elza-RDTs: "Ada" nur noch in ROOM30C1 msg 4 (Marvin) und msg 5 ("Elza: Ada?")
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
#include "re15_door_seq.h"
#include "re15_adaruf.h"
#include "re15_tuer1060.h"
#include "re15_sperre1050_r37.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

void re15_actor_step_all_walkers(void);

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

/* Standplatz vor Slot 4 (test_r34n_d_adaruf.c STAND_X/Z, Blick +X) und Eintritts-Cut aus ROOM1000 (Cut 4). */
#define STAND_X  16580
#define STAND_Z  (-13700)
#define EINTRITT_CUT_1050  4

static const char *const k_text_chief = "I have to get the Chief first...";

/* ---- Harness (Muster test_r34n_d_adaruf.c) --------------------------------------------------------------- */
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;

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
    return re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0 ? -1 : 0;
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

/* Raumaufbau wie der Tuer-Weg (room_common.c: Schritt 7 scd_room_reenter, Schritt 11 msg-Block); die Flags bleiben
 * (ein Raumwechsel ruft scd_vm_init nicht). */
static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, uint8_t cut, int bilder)
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
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = cut; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    for (int f = 0; f < bilder; f++) frame(0, 0);
    int warm = 0;
    while (warm < 3000 && (g_scd.player_mode == 2 || g_scd.letterbox_countdown != 0 || g_scd.message_active ||
                           g_scd.message_display_frames > 0 || g_scd.message_query)) {
        uint16_t e = ((warm % 10) == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
        warm++;
    }
    return 0;
}

static int text_ist(int id, const char *soll)
{
    const char *t = re15_msg_get_text(id);
    printf("    msg %d Text \"%s\"\n", id, t ? t : "(keiner)");
    return t && strcmp(t, soll) == 0;
}

/* Tuersequenz-Laeufer: faengt jede Anfrage (im Spiel spielt ihn die Plattform). */
static int s_n_seq = 0;
static void fang(const re15_door_seq_anfrage_t *a) { (void)a; s_n_seq++; }

/* Die Flags eines Spielabschnitts. Rolltor (3,121) ist im Endspiel offen (ROOM1050 sub02 @0x0CBA `22 03 79 01`). */
typedef struct { int bb, k, t, tod, b65; } stand_t;
static void flags_setzen(const stand_t *s)
{
    re15_game_state_init();
    re15_game_flag_set(3, 121, 1);
    if (s->bb)  re15_game_flag_set(RE15_S1050_ADA_BANK, RE15_S1050_ADA_BIT, 1);
    if (s->k)   re15_game_flag_set(RE15_S1050_PLAN_BANK, RE15_S1050_PLAN_BIT, 1);
    if (s->t)   re15_game_flag_set(3, 94, 1);
    if (s->tod) re15_game_flag_set(RE15_S1050_TOD_BANK, RE15_S1050_TOD_BIT, 1);
    if (s->b65) re15_game_flag_set(RE15_ADARUF_GESEHEN_BANK, RE15_ADARUF_GESEHEN_BIT, 1);
}

typedef struct { int stand, raumwechsel, ziel, msg_id, freeze, seq, zu, pm_frei; } druck_t;

/* Vor Slot 4 stellen (Vorwaerts-620-Punkt im Rechteck, Blick +X), einmal Quadrat; bis zu 300 Bilder beobachten.
 * Steht eine Nachricht, ab Bild 40 alle 10 Bilder Quadrat (blaettern/schliessen wie der Spieler). */
static druck_t drueck_slot4(void)
{
    druck_t r; memset(&r, 0, sizeof r); r.msg_id = -1; r.ziel = -1;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = STAND_X; pl->z = STAND_Z; pl->rot_y = 0;
    frame(0, 0);
    g_room_change.pending = 0;
    r.stand = (pl->x == STAND_X && pl->z == STAND_Z && pl->rot_y == 0);
    s_n_seq = 0;
    int zweiter = -1;
    for (int f = 0; f < 300; f++) {
        uint16_t e = 0;
        if (f == 0) e = RE15_PAD_BIT_SQUARE;
        if (f >= 40 && (f % 10) == 0 && g_scd.message_active) { e = RE15_PAD_BIT_SQUARE; if (zweiter < 0) zweiter = f; }
        frame(e, e);
        if (g_scd.message_active && r.msg_id < 0) {
            r.msg_id = (int)g_scd.message_id;
            r.freeze = ((g_re15_pauseflags & 0xffff0000u) == 0xffff0000u);
        }
        if (zweiter >= 0 && !g_scd.message_active && !r.zu) r.zu = 1;
        if (g_room_change.pending) { r.raumwechsel = 1; r.ziel = (int)g_room_change.room_id; break; }
    }
    r.seq = s_n_seq;
    r.pm_frei = (g_scd.player_mode == 0 && !g_scd.message_active);
    return r;
}

/* ---- Teil: bedingung -------------------------------------------------------------------------------------- */
static void teil_bedingung(void)
{
    printf("== bedingung ==\n");
    int n_zu = 0;
    for (int m = 0; m < 16; m++) {
        stand_t s = { (m >> 0) & 1, (m >> 1) & 1, (m >> 2) & 1, (m >> 3) & 1, 0 };
        flags_setzen(&s);
        const int soll = s.bb && s.k && !s.tod;
        const int ist = re15_sperre1050_r37_aktiv();
        PRUEF(ist == soll, "(3,0xBB)=%d (9,71)=%d (3,94)=%d (9,73)=%d -> gesperrt %d", s.bb, s.k, s.t, s.tod, ist);
        if (ist) {
            n_zu++;
            PRUEF(re15_tuer1060_sperre_aktiv() == 1, "   ... und die 1060-Sperre ist in derselben Stellung zu");
        }
    }
    PRUEF(n_zu == 2, "genau 2 der 16 Stellungen gesperrt ((3,94) frei waehlbar)");
}

/* ---- Teil: raum ------------------------------------------------------------------------------------------- */
static void raum_fall(const char *name, const stand_t *s, uint16_t raum, int soll_typ, int soll_id)
{
    printf("[raum] %s ROOM%04X: (3,0xBB)=%d (9,71)=%d (3,94)=%d (9,73)=%d (9,65)=%d\n", name, raum, s->bb, s->k, s->t,
           s->tod, s->b65);
    flags_setzen(s);
    if (room_boot(raum, STAND_X, STAND_Z, 0, EINTRITT_CUT_1050, 30) != 0) return;
    const re15_aot_t *a = &g_aot.slots[RE15_S1050_SLOT];
    const re15_aot_door_params_t *dp = &g_aot.door_params[RE15_S1050_SLOT];
    if (soll_typ == RE15_AOT_TYPE_DOOR) {
        PRUEF(a->active && a->type == RE15_AOT_TYPE_DOOR && dp->dest_room == 0x0A && re15_sperre1050_r37_gesperrt() == 0,
              "Slot 4 = Original-Tuer -> Raum 0x%02X (@0x00B71), Sperre %d", dp->dest_room, re15_sperre1050_r37_gesperrt());
    } else {
        PRUEF(a->active && (int)a->type == soll_typ && (int)a->event_id == soll_id,
              "Slot 4 Typ %d Id %d (soll Typ %d Id %d), Sperre %d", a->type, a->event_id, soll_typ, soll_id,
              re15_sperre1050_r37_gesperrt());
    }
}

static void teil_raum(void)
{
    printf("== raum ==\n");
    const stand_t frueh0  = { 0, 0, 1, 0, 0 };
    const stand_t frueh1  = { 0, 0, 1, 0, 1 };
    const stand_t mittel  = { 1, 0, 1, 0, 1 };
    const stand_t mittel0 = { 1, 0, 0, 0, 0 };   /* = Stellung des Riegels unit_r34n_d_adaruf_frei */
    const stand_t ende    = { 1, 1, 1, 0, 1 };
    const stand_t danach  = { 1, 1, 1, 1, 1 };
    raum_fall("Fruehspiel, Szene noch nicht gesehen", &frueh0, 0x1050, RE15_AOT_TYPE_GENERIC, RE15_ADARUF_EREIGNIS);
    PRUEF(re15_adaruf_zustand() == RE15_ADARUF_SZENE && re15_sperre1050_r37_gesperrt() == 0,
          "Ada-Logik: Szene scharf, keine Chief-Sperre");
    raum_fall("Fruehspiel, Szene gesehen", &frueh1, 0x1050, RE15_AOT_TYPE_MESSAGE, RE15_ADARUF_MSG_SPERRE);
    PRUEF(re15_adaruf_zustand() == RE15_ADARUF_SPERRE && re15_sperre1050_r37_gesperrt() == 0,
          "Ada-Logik: Sperrtext msg 25, keine Chief-Sperre");
    raum_fall("Mittelspiel (Ada gerettet, 10F0-Szene nicht)", &mittel, 0x1050, RE15_AOT_TYPE_DOOR, 0);
    raum_fall("Mittelspiel ohne (3,94) (Stellung unit_r34n_d_adaruf_frei)", &mittel0, 0x1050, RE15_AOT_TYPE_DOOR, 0);
    raum_fall("ENDSPIEL (Plan gefasst, Chief nicht geholt)", &ende, 0x1050, RE15_AOT_TYPE_MESSAGE, RE15_S1050_MSG_ID);
    {
        const re15_aot_t *a = &g_aot.slots[RE15_S1050_SLOT];
        PRUEF(re15_sperre1050_r37_gesperrt() == 1 && re15_adaruf_zustand() == RE15_ADARUF_AUS,
              "Chief-Sperre gesetzt, Ada-Logik aus");
        /* Rechteck der Tuer @0x00B5A (16700,-14700,1000,2000) -> Mitte (17200,-13700), halb (500,1000), Band 0. */
        PRUEF(!a->has_quad && a->x == 17200 && a->z == -13700 && a->half_w == 500 && a->half_h == 1000 && a->band == 0,
              "Rechteck/Band der Tuer (%d,%d) halb (%d,%d) Band %d", (int)a->x, (int)a->z, (int)a->half_w,
              (int)a->half_h, a->band);
        PRUEF(a->pause_mask16 == RE15_S1050_MASKE && a->sce_flags == RE15_S1050_FLAGS,
              "Form wie ROOM1130 sub01 @0x00A1C: Maske 0x%04X, flags 0x%02X", a->pause_mask16, a->sce_flags);
        int n = 0, rn = 0;
        const uint8_t *m = re15_tuer1060_meldung(&n);
        const unsigned char *raw = re15_msg_get_raw(RE15_S1050_MSG_ID, &rn);
        PRUEF(m && raw && rn == n && memcmp(raw, m, (size_t)n) == 0, "msg 26 = Bytes der 1060-Sperre (%d B)", rn);
        PRUEF(text_ist(RE15_S1050_MSG_ID, k_text_chief), "msg 26 woertlich");
        PRUEF(re15_msg_is_choice(RE15_S1050_MSG_ID) == 0, "keine Ja/Nein-Abfrage");
        /* Die anderen Tueren des Raums bleiben (main00 @0x00ADA/@0x00AFA/@0x00B1A/@0x00B3A/@0x00B7A). */
        static const struct { int slot; uint8_t ziel; } tueren[] = { {0, 0x03}, {1, 0x00}, {2, 0x00}, {3, 0x00}, {5, 0x09} };
        int ok = 1;
        for (unsigned i = 0; i < sizeof tueren / sizeof tueren[0]; i++) {
            const re15_aot_t *t = &g_aot.slots[tueren[i].slot];
            if (!t->active || t->type != RE15_AOT_TYPE_DOOR || g_aot.door_params[tueren[i].slot].dest_room != tueren[i].ziel)
                ok = 0;
        }
        PRUEF(ok, "Slots 0/1/2/3/5 bleiben Tueren (1030/1000/1000/1000/1090)");
        const char *t2 = re15_msg_get_text(2);
        PRUEF(t2 && strstr(t2, "I need a fuse to run the shutter.") != NULL, "Raum-Nachricht 2 unveraendert");
    }
    raum_fall("nach der Irons-Todesszene", &danach, 0x1050, RE15_AOT_TYPE_DOOR, 0);
    raum_fall("ELZA, Endspiel-Bits", &ende, 0x1051, RE15_AOT_TYPE_DOOR, 0);
}

/* ---- Teil: druck ------------------------------------------------------------------------------------------ */
static void teil_druck(void)
{
    printf("== druck ==\n");
    const stand_t ende   = { 1, 1, 1, 0, 1 };
    const stand_t mittel = { 1, 0, 1, 0, 1 };
    const stand_t danach = { 1, 1, 1, 1, 1 };
    flags_setzen(&ende);
    if (room_boot(0x1050, STAND_X, STAND_Z, 0, EINTRITT_CUT_1050, 30) == 0) {
        druck_t d = drueck_slot4();
        printf("  Endspiel: Stand %d, Nachricht %d, Freeze %d, zu %d, Raumwechsel %d, Sequenzen %d, frei %d\n",
               d.stand, d.msg_id, d.freeze, d.zu, d.raumwechsel, d.seq, d.pm_frei);
        PRUEF(d.stand && d.msg_id == RE15_S1050_MSG_ID, "Quadrat oeffnet msg 26 \"%s\"", k_text_chief);
        PRUEF(d.freeze, "Text-Freeze 0xffff0000 wie jeder sce-1-Text (@0x80043098)");
        PRUEF(!d.raumwechsel && d.seq == 0 && g_current_room_id == 0x1050,
              "kein Raumwechsel in 300 Bildern, keine Tuersequenz (Handler 0x80043084 ruft nur 0x80027e68)");
        PRUEF(d.zu && d.pm_frei, "weitere Quadrate schliessen den Text, Steuerung frei");
        druck_t d2 = drueck_slot4();
        PRUEF(d2.msg_id == RE15_S1050_MSG_ID && !d2.raumwechsel && d2.seq == 0, "zweiter Druck ebenso");
    }
    flags_setzen(&mittel);
    if (room_boot(0x1050, STAND_X, STAND_Z, 0, EINTRITT_CUT_1050, 30) == 0) {
        druck_t d = drueck_slot4();
        printf("  Mittelspiel: Nachricht %d, Raumwechsel %d -> ROOM%04X, Sequenzen %d\n", d.msg_id, d.raumwechsel,
               (unsigned)d.ziel, d.seq);
        PRUEF(d.stand && d.raumwechsel && d.ziel == 0x10A0 && d.msg_id < 0 && d.seq == 1,
              "Mittelspiel: Quadrat -> Tuersequenz + ROOM10A0 (Original-Tuer)");
        g_room_change.pending = 0;
    }
    flags_setzen(&danach);
    if (room_boot(0x1050, STAND_X, STAND_Z, 0, EINTRITT_CUT_1050, 30) == 0) {
        druck_t d = drueck_slot4();
        PRUEF(d.stand && d.raumwechsel && d.ziel == 0x10A0 && d.msg_id < 0, "nach Irons: Quadrat -> ROOM10A0");
        g_room_change.pending = 0;
    }
}

/* ---- Teil: luke (die Luecke in den Original-Daten) -------------------------------------------------------- */
static void teil_luke(void)
{
    printf("== luke ==\n");
    size_t n = 0;
    char p[600];
    snprintf(p, sizeof p, "%s/STAGE1/ROOM10F0.RDT", RE15_ASSET_PSX_DIR);
    uint8_t *f0 = slurp(p, &n);
    static const uint8_t gitter[26] = { 0x3b, 0x01, 0x02, 0x31, 0x01, 0x00, 0x1a, 0xeb, 0x36, 0xf7, 0xb0, 0x04, 0xe8,
                                        0x03, 0xe0, 0xca, 0xd8, 0xdc, 0x14, 0x05, 0x00, 0x0c, 0x00, 0x09, 0x00, 0x05 };
    PRUEF(f0 && n > 0xF52 + 26 && memcmp(f0 + 0xF52, gitter, sizeof gitter) == 0,
          "ROOM10F0 @0x00F52: Gitter = Door_aot_set Slot 1 Band 1 -> ROOM1090 (-13600,-9000,1300) Band 5");
    free(f0);
    if (rdt_laden(0x1090) != 0) { PRUEF(0, "ROOM1090 ladbar"); return; }
    static const uint8_t zelle[12] = { 0x6e, 0x0e, 0xcf, 0x10, 0x18, 0xd2, 0xf2, 0x0e, 0x01, 0x01, 0x0e, 0x53 };
    PRUEF(s_rawsz > 0x1E3C + 12 && memcmp(s_raw + 0x1E3C, zelle, sizeof zelle) == 0,
          "ROOM1090 SCA @0x01E3C = Absprung-Zelle (-11752,3826) 3694x4303, Typwort 0x530E");
    /* Sondenpunkt des Absprungs vom Steg (-9900,3358) Blick +Z: 800 voraus (FUN_8002d474 @0x8002d518-28). */
    const uint16_t w = re15_collision_floor_typeword(&s_rdt, -9900, 3358 + 800, 5, 0);
    PRUEF(w == 0x530E && (w & 2) && (w & 0x0e) == 0x0e,
          "Typwort am Sondenpunkt (-9900,4158) Band 5 = 0x%04X: Bit 1 (Absprung @0x8002d74c), Klasse 0x0E (@0x8002d780)", w);
    static const uint8_t hof[26] = { 0x3b, 0x00, 0x02, 0x31, 0x01, 0x00, 0x72, 0xcf, 0xd4, 0x08, 0x5c, 0x12, 0x40, 0x06,
                                     0x8a, 0x4d, 0x00, 0x00, 0xe4, 0xa8, 0x00, 0x0c, 0x00, 0x05, 0x06, 0x00 };
    PRUEF(s_rawsz > 0x211A + 26 && memcmp(s_raw + 0x211A, hof, sizeof hof) == 0,
          "ROOM1090 @0x0211A: Hof-Tuer Slot 0 Band 1 -> ROOM1050 (19850,0,-22300)");
}

/* ---- Teil: elza50b1 --------------------------------------------------------------------------------------- */
typedef struct { uint8_t *buf; size_t n; re15_rdt_t rdt; } raum_t;
static int raum_laden(raum_t *r, uint16_t id)
{
    char p[600];
    snprintf(p, sizeof p, "%s/STAGE%X/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(id >> 12), (unsigned)id);
    r->buf = slurp(p, &r->n);
    if (!r->buf) return -1;
    return re15_rdt_parse(r->buf, r->n, &r->rdt) == 0 ? 0 : -1;
}

static const char *const k_neu_50b1 = "It's a woman. She's dead.It looks like one of those gorillas killed her.";
static const char *const k_alt_50b1 = "It's Ada. She's dead.It looks like one of those gorillas killed her.";

static void teil_elza50b1(void)
{
    printf("== elza50b1 ==\n");
    raum_t r;
    if (raum_laden(&r, 0x50B1) != 0) { PRUEF(0, "ROOM50B1 ladbar"); return; }
    const uint8_t *blk = r.rdt.messages;
    const int bs = (int)r.rdt.messages_size;
    const int o0 = blk[0] | (blk[1] << 8);
    const uint8_t *orig = blk + o0;
    char alt[256];
    re15_msg_decode_text(orig, (size_t)(bs - o0), alt, sizeof alt);
    PRUEF((uint32_t)(orig - r.buf) == 0x0055A && strcmp(alt, k_alt_50b1) == 0,
          "Original ROOM50B1 msg 0 @0x%05X = \"%s\"", (unsigned)(orig - r.buf), alt);
    g_current_room_id = 0x50B1;
    re15_msg_clear_room_block();
    re15_msg_load_room_block(blk, bs);
    PRUEF(text_ist(0, k_neu_50b1), "nach dem Laden: neuer Text ohne Ada-Bezug (NUTZER-VORGABE)");
    int rl = 0;
    const unsigned char *neu = re15_msg_get_raw(0, &rl);
    /* Form = Original: Kopf 04 02, Seitentrenner 02 00 (eine Stelle), Zeilenumbruch 08 (eine Stelle, Seite 2),
     * Schwanz 01 00 (@0x0055A: `04 02 ... 57 02 00 25 50 ... 41 08 43 ... 57 01 00`). */
    int trenner = 0, umbruch = 0, umbruch_nach_trenner = 0;
    for (int i = 2; neu && i + 1 < rl - 2; i++) {
        if (neu[i] == 0x02 && neu[i + 1] == 0x00) { trenner++; i++; continue; }
        if (neu[i] == 0x08) { umbruch++; if (trenner == 1) umbruch_nach_trenner = 1; }
    }
    PRUEF(neu && rl >= 6 && neu[0] == 0x04 && neu[1] == 0x02 && neu[rl - 2] == 0x01 && neu[rl - 1] == 0x00 &&
          trenner == 1 && umbruch == 1 && umbruch_nach_trenner,
          "Form wie das Original: Kopf 04 02, 1 Seitenwechsel, 1 Zeilenumbruch auf Seite 2, Schwanz 01 00 (%d B)", rl);
    int olen = 0;
    while (o0 + olen < bs && orig[olen] != 0x01) {
        uint8_t b = orig[olen];
        olen += (b == 0x02 || b == 0x04 || b == 0x05 || b == 0x06 || b == 0x09 || b == 0x0A || b == 0x0B) ? 2 : 1;
    }
    olen += 2;
    PRUEF(neu && re15_msg_compute_duration(neu, (size_t)rl, 0) == re15_msg_compute_duration(orig, (size_t)olen, 0),
          "Dauer %d Bilder = Original", neu ? re15_msg_compute_duration(neu, (size_t)rl, 0) : -1);
    free(r.buf);

    /* Leon bleibt unberuehrt: ROOM50B0 msg 18 "She's dead." */
    if (raum_laden(&r, 0x50B0) == 0) {
        g_current_room_id = 0x50B0;
        re15_msg_clear_room_block();
        re15_msg_load_room_block(r.rdt.messages, (int)r.rdt.messages_size);
        PRUEF(text_ist(18, "She's dead."), "Leon ROOM50B0 msg 18 unveraendert");
        free(r.buf);
    }

    /* Sweep aller Elza-RDTs (Raumlader Elza = Leon + 1, PSX.EXE @0x800397e4/@0x800397ec): "Ada" nur noch, wo Elza
     * den Namen von Marvin HOERT (30C1 msg 4) und nachfragt (30C1 msg 5 "Elza: Ada?"). */
    int raeume = 0, treffer = 0, unerlaubt = 0, alt_text = 0;
    for (int st = 1; st <= 6; st++)
        for (int rm = 0; rm < 0x40; rm++) {
            const uint16_t id = (uint16_t)((st << 12) | (rm << 4) | 1);
            if (raum_laden(&r, id) != 0) { free(r.buf); continue; }
            raeume++;
            g_current_room_id = id;
            re15_msg_clear_room_block();
            if (r.rdt.messages && r.rdt.messages_size >= 4) {
                re15_msg_load_room_block(r.rdt.messages, (int)r.rdt.messages_size);
                for (int k = 0; k < 256; k++) {
                    const char *t = re15_msg_get_text(k);
                    if (!t) continue;
                    if (strstr(t, "It's Ada")) alt_text++;
                    const char *h = strstr(t, "Ada");
                    int wort = 0;
                    while (h) {
                        const char nach = h[3];
                        const int vorn_ok = (h == t) || !((h[-1] >= 'a' && h[-1] <= 'z') || (h[-1] >= 'A' && h[-1] <= 'Z'));
                        if (vorn_ok && !((nach >= 'a' && nach <= 'z') || (nach >= 'A' && nach <= 'Z'))) wort = 1;
                        h = strstr(h + 1, "Ada");
                    }
                    if (!wort) continue;
                    treffer++;
                    const int ok = (id == 0x30C1 && (k == 4 || k == 5));
                    if (!ok) unerlaubt++;
                    printf("    %s ROOM%04X msg %d: %s\n", ok ? "erlaubt  " : "UNERLAUBT", id, k, t);
                }
            }
            free(r.buf);
        }
    PRUEF(raeume >= 90, "Sweep: %d Elza-RDTs geladen (Port-Lader + Overrides)", raeume);
    PRUEF(treffer == 2 && unerlaubt == 0, "Sweep: \"Ada\" %d mal, nur ROOM30C1 msg 4 (Marvin) und msg 5 (Elza fragt nach)",
          treffer);
    PRUEF(alt_text == 0, "Sweep: \"It's Ada\" in keiner Elza-Nachricht mehr");
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    const int alle = !strcmp(teil, "alle");
    re15_door_seq_setze_laeufer(fang);
    if (alle || !strcmp(teil, "bedingung")) teil_bedingung();
    if (alle || !strcmp(teil, "raum"))      teil_raum();
    if (alle || !strcmp(teil, "druck"))     teil_druck();
    if (alle || !strcmp(teil, "luke"))      teil_luke();
    if (alle || !strcmp(teil, "elza50b1"))  teil_elza50b1();
    re15_door_seq_setze_laeufer(NULL);
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
