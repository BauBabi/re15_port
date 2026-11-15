/* test_r41_monitor.c — Runde 41 Spur Monitor (include/re15_monitor_r41.h): die Hangar-Szene als Monitorbild im Monitor
 * Room ROOM5030 (Leon) / ROOM5031 (Elza), Raumprobe ohne main.c (echte RDTs, echtes SCD, echte Szene birkin_formen.c).
 *
 *   effekt     Monitor-Bild-Effekt Zone 1 Bit 0x1E = FUN_80020794(0,0): Zeilenversatz Sinus(@0x80072d70)/64, Phase +2 je Bild,
 *              Kopier-Reihenfolge der OT; aus ohne 0x1E und bei 0x1C/0x1D (FUN_80020674-Zweige).
 *   ablauf     (c)+(e) je Raum 5030/5031: Ausloeser beim ersten freien Bild -> Cut 7 (Pad-Sperre, Zonen aus) 20 Takte -> Wechsel in
 *              den Hangar (Schnitt 12, Bit 38) -> Szene genau einmal -> Wechsel zurueck an die gespeicherte Stelle (Schnitt 9)
 *              -> 50 Takte -> Schnitt des Spielers, Zonen an, Pad frei; danach und beim Wiederbetreten kein zweites Mal.
 *              5031 (d): nach der Rueckkehr steht das Sherry-Ereignis bereit (Sherry 0x4B gesetzt, AOT Slot 6, (3,0x2F) == 0).
 *   hangar     (b) 50A0/50A1 nach dem Monitorbild, nach Wiederbetreten und nach Speichern/Laden: kein 0x30/0x2B, keine Szene.
 *   speichern  (b)+(c) Bit 38 ueberlebt re15_savedata_capture/restore; nach dem Laden loest 5030 nicht erneut aus.
 *
 * Gegenproben (Aufruf mit einem Abschnittsnamen + "gegen": Erfolg nur, wenn der Abschnitt ROT ist):
 *   ablauf gegen    mit RE15_KEIN_MONITORBILD=1 (kein Monitorbild)          -> (c) rot
 *   hangar gegen    mit RE15_HANGAR_IM_RAUM=1 (alter Ausloeser im Hangar)   -> (b) rot (Birkin im Hangar)
 *   stelle gegen    Rueckweg 300 Einheiten neben die gespeicherte Stelle      -> (e) rot
 *   sherry gegen    (3,0x2F) = 1 vor dem Monitor Room                         -> (d) rot (kein Sherry-Ereignis bereit)
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_room.h"
#include "re15_birkin_formen.h"
#include "re15_monitor_r41.h"
#include "re15_savedata.h"

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); fprintf(stderr, "\n"); fails++; } \
    else if (s_laut) { fprintf(stderr, "ok:   " __VA_ARGS__); fprintf(stderr, "\n"); } } while (0)
static int s_laut = 0;

static uint8_t *slurp(const char *path, size_t *out)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)n);
    if (b && fread(b, 1, (size_t)n, f) != (size_t)n) { free(b); b = NULL; }
    fclose(f);
    if (b) *out = (size_t)n;
    return b;
}

static uint8_t s_ems_buf[2][0x80000];
static int bank_laden(uint8_t type, int idx, const char *ersatz)
{
    size_t len = 0;
    uint8_t *src = NULL;
    if (ersatz) {
        char p[512]; snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/%s", ersatz);
        src = slurp(p, &len);
    } else {
        size_t esz = 0, off = 0;
        uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &esz);
        if (ems && re15_ems_get_entry(ems, esz, re15_ems_index_for_type(type), &off, &len) == 0) {
            src = (uint8_t *)malloc(len); memcpy(src, ems + off, len);
        }
        free(ems);
    }
    if (!src || len > sizeof s_ems_buf[idx]) { free(src); return 0; }
    memcpy(s_ems_buf[idx], src, len); free(src);
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re15_emd_parse_container(s_ems_buf[idx], len, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) return 0;
    eb->ok = 1; eb->buf = NULL;
    return 1;
}

/* Raum laden wie test_birkin_szenen.c raum_laden (main00 + sub00, 60 SCD-Takte), Spieler an (x,z) mit Gier, Eintritts-Schnitt cut,
 * danach der Haken des Raumwechsels (room_common.c re15_room_apply_pending -> re15_monitor_r41_raum_geladen). */
static re15_rdt_t s_rdt;
static uint8_t *s_buf = NULL;
static int raum(unsigned id, int32_t x, int32_t z, int16_t gier, int cut)
{
    char p[512]; size_t sz = 0;
    snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/STAGE%u/ROOM%04X.RDT", (id >> 12) & 0xF, id);
    free(s_buf); s_buf = slurp(p, &sz);
    if (!s_buf || re15_rdt_parse(s_buf, sz, &s_rdt) != 0) return 0;
    extern void scd_register_current_rdt(const re15_rdt_t *);
    scd_register_current_rdt(NULL);
    g_current_room_id = id;
    re15_enemy_reset();
    re15_actor_init();
    re15_aot_init();
    {   /* scd_vm_init ruft re15_game_state_init (Flags weg) - ein Raumwechsel behaelt die Story-Flags (room_common.c ruft
         * scd_room_reenter, nicht scd_vm_init): Flags ueber den Init retten */
        static uint32_t keep[RE15_FLAG_ZONES][RE15_FLAG_WORDS_ZONE];
        memcpy(keep, g_game.flags, sizeof keep);
        scd_vm_init();
        memcpy(g_game.flags, keep, sizeof keep);
    }
    /* wie der Raumwechsel: Pause-Wort leer (room_common.c @0x8001ca44), Zone 1 untere 16 Bit weg (scd_room_setup.c @0x80039710-30),
     * Pad-Sperren-Proxy (2,7) weg */
    re15_pauseflags_clear();
    for (int fi = 16; fi < 32; fi++) re15_game_flag_set(1, (uint8_t)fi, 0);
    re15_game_flag_set(2, 7, 0);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->state = 1; pl->hp = 100; pl->x = x; pl->y = 0; pl->z = z; pl->rot_y = gier;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[0x0A] = (int16_t)cut; g_scd.cam_id = (uint8_t)cut; g_scd.cut_auto_enabled = 1;
    for (int i = 0; i < 60; i++) scd_vm_tick();
    scd_register_current_rdt(&s_rdt);
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    g_scd.work_vars[0x0A] = (int16_t)cut; g_scd.cam_id = (uint8_t)cut; g_scd.cut_auto_enabled = 1;
    g_room_change.pending = 0;
    re15_monitor_r41_raum_geladen();
    return 1;
}

/* Wechsel ausfuehren, den der Monitor angefordert hat (g_room_change), wie room_common.c (Zielraum, Lage, Gier, Schnitt). */
static int wechsel(void)
{
    if (!g_room_change.pending) return 0;
    const re15_room_change_t c = g_room_change;
    if (!raum(c.room_id, c.x, c.z, c.yaw_4096, c.target_cut)) return 0;
    if ((c.room_id & 0xFFFEu) == 0x50A0u) {
        CHECK(bank_laden(0x30u, 0, re15_bf_emd_datei(0x30u)), "EM30-Bank im Monitorbild");
        CHECK(bank_laden(0x2Bu, 1, NULL), "EM2B-Bank im Monitorbild");
    }
    return 1;
}

static int zahl(uint8_t typ)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == typ) n++;
    return n;
}

/* ---- effekt ------------------------------------------------------------------------------------------------------- */
static void t_effekt(void)
{
    static const int8_t sinus_auszug[4][2] = { { 2, 9 }, { 64, 127 }, { 130, -9 }, { 202, -122 } };   /* Tabelle @0x80072d70 */
    uint8_t q[240];
    (void)sinus_auszug;
    re15_monitor_r41_zeilen_phase(0, q);
    /* Phase 0: Zeile y nimmt Sinus[(2*(y+1)) & 0xff]/64. y 0 (Sinus[2] = 9 -> 0), y 31 (Sinus[64] = 127 -> +1: Zeile 32),
     * y 100 (Sinus[202] = -122 -> -1: Zeile 99), y 64 (Sinus[130] = -9 -> 0). */
    CHECK(q[0] == 0 && q[31] == 32 && q[100] == 99 && q[64] == 64, "Zeilen Phase 0: q[0]=%d q[31]=%d q[100]=%d q[64]=%d (Soll 0/32/99/64)",
          q[0], q[31], q[100], q[64]);
    int verschoben = 0, weit = 0;
    for (int y = 0; y < 240; y++) { if (q[y] != y) verschoben++; if (abs((int)q[y] - y) > 1) weit++; }
    CHECK(verschoben > 100 && weit == 0, "Zeilen Phase 0: %d Zeilen verschoben, %d weiter als 1 (Soll > 100, 0)", verschoben, weit);
    uint8_t q2[240];
    re15_monitor_r41_zeilen_phase(2, q2);
    CHECK(memcmp(q, q2, 240) != 0, "Phase 2 = Phase 0 (die Welle rollt nicht)");
    re15_game_flag_set(1, 0x1E, 0);
    CHECK(re15_monitor_r41_zeilen(q) == 0, "Effekt an ohne Zone-1-Bit 0x1E");
    re15_game_flag_set(1, 0x1E, 1);
    CHECK(re15_monitor_r41_zeilen(q) == 1, "Effekt aus trotz Zone-1-Bit 0x1E");
    re15_game_flag_set(1, 0x1C, 1);
    CHECK(re15_monitor_r41_zeilen(q) == 0, "Effekt an trotz Bit 0x1C (FUN_80020674-Zweig, ungebaut)");
    re15_game_flag_set(1, 0x1C, 0); re15_game_flag_set(1, 0x1E, 0);
}

/* ---- ablauf (c)(d)(e) ------------------------------------------------------------------------------------------------ */
static int s_stelle_gegen = 0, s_sherry_gegen = 0;
static void t_ablauf(unsigned mon)
{
    const unsigned hangar = 0x50A0u | (mon & 1u);
    const int32_t x0 = -13846, z0 = -5474;              /* Eintritt aus 5000 (Door ROOM5000 slot 0 @0x007FE) */
    const int16_t g0 = 798;
    const int cut0 = 2;
    re15_game_state_init();
    re15_monitor_r41_reset();
    re15_bf_reset();
    if (s_sherry_gegen) re15_game_flag_set(3, 0x2F, 1);
    if (!raum(mon, x0, z0, g0, cut0)) { CHECK(0, "ROOM%04X nicht ladbar", mon); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* Gegenprobe Ausloeser: Spieler nicht frei -> wartet */
    pl->state = 2; re15_enemy_ai_run_all(0);
    CHECK(re15_monitor_r41_phase() == RE15_MON41_AUS, "%04X: Monitor startet, obwohl der Spieler nicht frei ist", mon);
    pl->state = 1;
    re15_enemy_ai_run_all(0);
    CHECK(re15_monitor_r41_phase() == RE15_MON41_VOR && g_scd.work_vars[0x0A] == 7 && g_scd.cam_id == 7 && !g_scd.cut_auto_enabled &&
          re15_game_flag_get(2, 7) && !re15_game_flag_get(1, 27),
          "%04X: Start: Phase %d Schnitt %d/%d Zonen %d Pad-Sperre %d Letterbox %d (Soll VOR, Cut 7 @0x01194, Zonen aus, (2,7) @0x01142, keine (1,0x1B))",
          mon, re15_monitor_r41_phase(), g_scd.work_vars[0x0A], g_scd.cam_id, g_scd.cut_auto_enabled, re15_game_flag_get(2, 7),
          re15_game_flag_get(1, 27));
    int takte = 1;
    while (!g_room_change.pending && takte < 100) { re15_enemy_ai_run_all(0); takte++; }
    CHECK(takte == 20 + 1, "%04X: Cut 7 gehalten %d Takte (Soll 20 = Sleep @0x01196, + Starttakt)", mon, takte);
    CHECK(g_room_change.pending && g_room_change.room_id == hangar && g_room_change.target_cut == 12 && g_room_change.x == -12117 &&
          g_room_change.z == 4472 && g_room_change.yaw_4096 == 1024 && re15_game_flag_get(RE15_MON41_BANK, RE15_MON41_BIT),
          "%04X: Wechsel in den Hangar: Raum %04X Schnitt %d (%d,%d) Gier %d, Bit 38 %d (Soll %04X, 12, (-12117,4472) @0x00EC6, 1024, 1)",
          mon, g_room_change.room_id, g_room_change.target_cut, (int)g_room_change.x, (int)g_room_change.z,
          (int)g_room_change.yaw_4096, re15_game_flag_get(RE15_MON41_BANK, RE15_MON41_BIT), hangar);
    /* Hangar: Monitorbild */
    int starts = 0, ph_alt = 0, maxph = 0, bild_takte = 0, zeilen_an = 0;
    if (wechsel()) {
        CHECK(re15_monitor_r41_bild() && re15_game_flag_get(1, 0x1E) && g_scd.work_vars[0x0A] == 12 && !g_scd.cut_auto_enabled,
              "%04X: Monitorbild: bild %d Effekt %d Schnitt %d Zonen %d (Soll 1, 1 @0x011A8, 12, 0)", hangar, re15_monitor_r41_bild(),
              re15_game_flag_get(1, 0x1E), g_scd.work_vars[0x0A], g_scd.cut_auto_enabled);
        while (!g_room_change.pending && bild_takte < 400) {
            re15_enemy_ai_run_all(0);
            bild_takte++;
            const int ph = re15_bf_hangar_phase();
            if (ph == 1 && ph_alt == 0) starts++;
            if (ph > maxph) maxph = ph;
            ph_alt = ph;
            uint8_t q[240];
            if (re15_game_flag_get(1, 0x1E) && re15_monitor_r41_zeilen(q)) zeilen_an++;
            int fremd = 0;
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (g_actors[s].active && g_actors[s].type != 0x30u && g_actors[s].type != 0x2Bu) fremd++;
            if (bild_takte >= 1 && fremd) { CHECK(0, "%04X: %d Raumfiguren (Affen) im Monitorbild", hangar, fremd); break; }
        }
    }
    CHECK(starts == 1 && maxph == 7, "%04X: Szene %d-mal gestartet, hoechste Phase %d (Soll 1, 7 = frei)", hangar, starts, maxph);
    CHECK(zeilen_an > 130, "%04X: Monitor-Bild-Effekt nur %d Takte an (Soll die ganze Szene)", hangar, zeilen_an);
    CHECK(g_room_change.pending && g_room_change.room_id == mon && g_room_change.target_cut == 9 && !re15_game_flag_get(1, 0x1E),
          "%04X: Rueckweg: pending %d Raum %04X Schnitt %d Effekt %d (Soll %04X, 9 @0x011EA, 0 @0x011E6)", hangar, g_room_change.pending,
          g_room_change.room_id, g_room_change.target_cut, re15_game_flag_get(1, 0x1E), mon);
    if (s_stelle_gegen) g_room_change.x += 300;          /* Gegenprobe (e): falsche Rueckkehrstelle */
    /* zurueck im Monitor Room */
    if (wechsel()) {
        CHECK(re15_monitor_r41_phase() == RE15_MON41_NACH && g_scd.work_vars[0x0A] == 9 && !g_scd.cut_auto_enabled,
              "%04X: Rueckkehr: Phase %d Schnitt %d Zonen %d (Soll NACH, 9, aus)", mon, re15_monitor_r41_phase(), g_scd.work_vars[0x0A],
              g_scd.cut_auto_enabled);
        int n = 0;
        while (re15_monitor_r41_phase() != RE15_MON41_AUS && n < 200) { re15_enemy_ai_run_all(0); n++; }
        CHECK(n == 50, "%04X: Monitorwand %d Takte (Soll 50 = Sleep @0x011F8)", mon, n);
    }
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    CHECK(pl->x == x0 && pl->z == z0 && pl->y == 0 && pl->rot_y == g0 && pl->state == 1 && pl->hp == 100,
          "%04X (e): Spieler danach (%d,%d,%d) Gier %d Zustand %d hp %d (Soll (%d,0,%d) Gier %d, Zustand 1, hp 100)", mon,
          (int)pl->x, (int)pl->y, (int)pl->z, (int)pl->rot_y, (int)pl->state, (int)pl->hp, (int)x0, (int)z0, (int)g0);
    CHECK(g_scd.work_vars[0x0A] == cut0 && g_scd.cam_id == cut0 && g_scd.cut_auto_enabled && !re15_game_flag_get(2, 7) &&
          !re15_game_flag_get(1, 27) && !re15_game_flag_get(1, 0x1E) && !re15_cine_active(),
          "%04X (e): Ende: Schnitt %d/%d Zonen %d Pad-Sperre %d Letterbox %d Effekt %d (Soll Schnitt %d, Zonen an @0x0120A, alles frei @0x0120C)",
          mon, g_scd.work_vars[0x0A], g_scd.cam_id, g_scd.cut_auto_enabled, re15_game_flag_get(2, 7), re15_game_flag_get(1, 27),
          re15_game_flag_get(1, 0x1E), cut0);
    /* (c) genau einmal: weiter im Raum, und Wiederbetreten */
    for (int i = 0; i < 120; i++) re15_enemy_ai_run_all(0);
    CHECK(re15_monitor_r41_phase() == RE15_MON41_AUS && !g_room_change.pending, "%04X (c): Monitor loest im selben Besuch erneut aus", mon);
    raum(mon, x0, z0, g0, cut0);
    for (int i = 0; i < 60; i++) re15_enemy_ai_run_all(0);
    CHECK(re15_monitor_r41_phase() == RE15_MON41_AUS && !g_room_change.pending, "%04X (c): Monitor loest beim Wiederbetreten erneut aus", mon);
    /* (d) Elza: das Sherry-Ereignis steht nach der Rueckkehr bereit (ROOM5031 sub00 @0x010B6/@0x010CA/@0x010E8, Ck(3,0x2F)==0) */
    if (mon & 1u) {
        const int sherry = zahl(0x4Bu);
        CHECK(sherry == 1 && zahl(0x29u) == 1 && g_aot.slots[6].active && !re15_game_flag_get(3, 0x2F),
              "5031 (d): Sherry-Ereignis nicht bereit: Sherry 0x4B %d, Affe 0x29 %d, AOT Slot 6 %d, (3,0x2F) %d (Soll 1, 1, 1, 0)",
              sherry, zahl(0x29u), g_aot.slots[6].active, re15_game_flag_get(3, 0x2F));
    }
    fprintf(stderr, "Ablauf %04X: Cut 7 %d Takte, Monitorbild %d Takte (Effekt %d), Szene %d-mal, zurueck an (%d,%d) Gier %d Schnitt %d\n",
            mon, takte, bild_takte, zeilen_an, starts, (int)pl->x, (int)pl->z, (int)pl->rot_y, g_scd.work_vars[0x0A]);
}

/* ---- hangar (b) -------------------------------------------------------------------------------------------------------- */
static void hangar_besuch(unsigned id, const char *wann)
{
    raum(id, -12406, 4381, 1024, 0);                     /* Eintritt aus 5030 (debug-Sprung-Lage) */
    CHECK(bank_laden(0x30u, 0, "EMD/EM30_HANGAR.EMD"), "EM30-Bank (%s)", wann);
    CHECK(bank_laden(0x2Bu, 1, NULL), "EM2B-Bank (%s)", wann);
    for (int c = 0; c < 12; c++) {                       /* alle Spielschnitte, auch die frueheren Ausloeser 4/5/6 */
        g_scd.cam_id = (uint8_t)c; g_scd.work_vars[0x0A] = (int16_t)c;
        for (int f = 0; f < 10; f++) re15_enemy_ai_run_all(0);
    }
    CHECK(zahl(0x30u) == 0 && zahl(0x2Bu) == 0 && re15_bf_hangar_phase() == 0 && !re15_bf_hangar_riegel(),
          "%04X (b) %s: %d Birkin, %d Spider-Man, Szenenphase %d (Soll 0, 0, 0)", id, wann, zahl(0x30u), zahl(0x2Bu),
          re15_bf_hangar_phase());
}

static void t_hangar(void)
{
    for (int v = 0; v < 2; v++) {
        const unsigned mon = 0x5030u | (unsigned)v, hg = 0x50A0u | (unsigned)v;
        re15_game_state_init(); re15_monitor_r41_reset(); re15_bf_reset();
        hangar_besuch(hg, "vor dem Monitor Room (Weg ueber 4070)");
        /* Monitorbild abspielen (wie t_ablauf, ohne Pruefungen) */
        raum(mon, -13846, -5474, 798, 2);
        for (int i = 0; i < 40 && !g_room_change.pending; i++) re15_enemy_ai_run_all(0);
        if (wechsel()) for (int i = 0; i < 400 && !g_room_change.pending; i++) re15_enemy_ai_run_all(0);
        if (wechsel()) for (int i = 0; i < 80; i++) re15_enemy_ai_run_all(0);
        hangar_besuch(hg, "nach dem Monitorbild");
        hangar_besuch(hg, "Wiederbetreten");
        re15_savedata_t sd; uint16_t r = 0;
        g_current_room_id = mon;
        re15_savedata_capture(&sd, 0, 1);
        re15_game_state_init(); re15_monitor_r41_reset(); re15_bf_reset();
        CHECK(re15_savedata_restore(&sd, &r) == 0, "Laden %04X", mon);
        hangar_besuch(hg, "nach Speichern/Laden");
    }
}

/* ---- speichern (b)(c) ---------------------------------------------------------------------------------------------------- */
static void t_speichern(void)
{
    re15_game_state_init(); re15_monitor_r41_reset(); re15_bf_reset();
    re15_game_flag_set(RE15_MON41_BANK, RE15_MON41_BIT, 1);
    g_current_room_id = 0x5000u;
    re15_savedata_t sd; uint16_t r = 0;
    re15_savedata_capture(&sd, 0, 1);
    re15_game_state_init();
    CHECK(!re15_game_flag_get(RE15_MON41_BANK, RE15_MON41_BIT), "neues Spiel: Bit 38 steht noch");
    CHECK(re15_savedata_restore(&sd, &r) == 0 && re15_game_flag_get(RE15_MON41_BANK, RE15_MON41_BIT),
          "Bit 38 ueberlebt Speichern/Laden nicht");
    raum(0x5030u, -13846, -5474, 798, 2);
    for (int i = 0; i < 60; i++) re15_enemy_ai_run_all(0);
    CHECK(re15_monitor_r41_phase() == RE15_MON41_AUS && !g_room_change.pending, "(c) nach dem Laden loest 5030 erneut aus");
    /* Gegenstueck: neues Spiel ohne Bit -> loest aus (das Bit ist der einzige Riegel) */
    re15_game_state_init(); re15_monitor_r41_reset();
    raum(0x5030u, -13846, -5474, 798, 2);
    re15_enemy_ai_run_all(0);
    CHECK(re15_monitor_r41_phase() == RE15_MON41_VOR, "neues Spiel: 5030 loest nicht aus");
    re15_monitor_r41_reset(); re15_game_flag_set(2, 7, 0);
    /* Story-Grenze (3,0x4C): ab dort ist 5030 unerreichbar (ROOM5000 @0x007FE); loest auch per Sprung nicht aus */
    re15_game_state_init(); re15_monitor_r41_reset();
    re15_game_flag_set(3, 0x4C, 1);
    raum(0x5030u, -13846, -5474, 798, 2);
    re15_enemy_ai_run_all(0);
    CHECK(re15_monitor_r41_phase() == RE15_MON41_AUS, "(3,0x4C) = 1: 5030 loest trotzdem aus");
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alles";
    const int gegen = argc > 2 && strcmp(argv[2], "gegen") == 0;
    s_laut = getenv("RE15_TEST_LAUT") != NULL;
    re15_game_state_init();
    if (!strcmp(teil, "stelle")) s_stelle_gegen = gegen;
    if (!strcmp(teil, "sherry")) s_sherry_gegen = gegen;
    if (!strcmp(teil, "alles") || !strcmp(teil, "effekt")) t_effekt();
    if (!strcmp(teil, "alles") || !strcmp(teil, "ablauf") || !strcmp(teil, "stelle")) { t_ablauf(0x5030u); t_ablauf(0x5031u); }
    if (!strcmp(teil, "sherry")) t_ablauf(0x5031u);
    if (!strcmp(teil, "alles") || !strcmp(teil, "hangar")) t_hangar();
    if (!strcmp(teil, "alles") || !strcmp(teil, "speichern")) t_speichern();
    free(s_buf);
    if (gegen) {
        fprintf(stderr, "GEGENPROBE %s: %d Pruefung(en) rot -> %s\n", teil, fails, fails ? "Riegel sieht den Befund (OK)" : "BLIND");
        return fails ? 0 : 1;
    }
    fprintf(stderr, fails ? "test_r41_monitor %s: %d FAIL\n" : "test_r41_monitor %s: OK\n", teil, fails);
    return fails ? 1 : 0;
}
