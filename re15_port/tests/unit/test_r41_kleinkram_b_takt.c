/*
 * Runde 41 Spur E "kleinkram_b", Nachbesserung 2 (Abnahme 1 Maengel 1-3, master-AUFTRAG E-N1/E-N2). Module
 * engine/src/msgtakt_r41.c, kamerahalt_r41.c; Dossier E_kleinkram_b.md "Nachbesserung 2".
 *
 *   msgtakt     Restore beim Schliessen IM Nachrichten-Takt wirkt erst hinter dem Spiel-Takt (Hauptschleife
 *               @0x80020de4 Spiel-Task < @0x80020f3c/@0x80010044 Nachrichten-Takt); ein Oeffnen im selben Bild holt
 *               ihn vorher nach (Schnappschuss @0x80027ec8 bleibt richtig); ohne takt_beginn alles wie vorher;
 *               Auswahl-Message_on laeuft durch nur mit Skript-Bit 0x02000000 (@0x8003f040-@0x8003f04c).
 *   kamerahalt  Apply-Bild = pending ODER work_vars[0x0A] != cam_id (Present @0x800214f4-@0x80021538 ->
 *               @0x80021558/@0x80021560); erstes Bild eines Raums und ROOM1240-Montage nicht.
 *   frage       ROOM2020 ohne Fenster, Leon GEHT in die Kurbel-Zone (Slot 3 @0x009AE), Aktion, beide Fragen per
 *               Quadrat: mit der Original-Reihenfolge kein Luecken-Bild zwischen Message_on 0 @0x0A5C und 1 @0x0A62
 *               (Geh-Bild bleibt, Original n9..n94 Clip 5 Bild 9) und Pos_set @0x0A8C im ERSTEN Bild nach dem
 *               Schliessen der Frage (Original n94 -> n95); Gegenprobe alte Reihenfolge: Luecke + ein Bild spaeter.
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
#include "re15_gameflow.h"
#include "re15_menu.h"
#include "re15_map_hint.h"     /* re15_host_clock_set_us */
#include "re15_cut10f0.h"      /* re15_cine_active */
#include "re15_montage_fx.h"
#include "re15_msgtakt_r41.h"
#include "re15_kamerahalt_r41.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif
void re15_actor_step_all_walkers(void);

static int g_fail = 0, g_n = 0;
#define PRUEF(bed, ...) do { g_n++; int _ok = (bed) ? 1 : 0; if (!_ok) g_fail++; \
    printf("  %s: ", _ok ? "PASS" : "FAIL"); printf(__VA_ARGS__); printf("\n"); } while (0)

/* ============================================================================================ msgtakt */
static void schliessen(void)   /* wie msg_common.c fsm 6 */
{
    if (!re15_msgtakt_r41_schliessen_aufschieben()) re15_pauseflags_close();
}
static void oeffnen(uint32_t maske)   /* wie re15_dialog_open_mask */
{
    re15_msgtakt_r41_vor_oeffnen();
    re15_pauseflags_open(maske);
}

static void teil_msgtakt(void)
{
    printf("== msgtakt ==\n");
    re15_msgtakt_r41_zuruecksetzen();
    re15_pauseflags_clear(); g_re15_pauseflags = 0x7;
    oeffnen(0xffff0000u);
    PRUEF(g_re15_pauseflags == 0xffff0007u, "Oeffnen: g_pauseflags |= Maske (@0x80027ed0): %08x", (unsigned)g_re15_pauseflags);
    schliessen();
    PRUEF(g_re15_pauseflags == 0x7u && !re15_msgtakt_r41_aktiv(), "ohne takt_beginn (PSX, Sonden): Restore sofort: %08x", (unsigned)g_re15_pauseflags);

    re15_msgtakt_r41_takt_beginn();
    oeffnen(0xffff0000u);
    schliessen();                                   /* im Nachrichten-Takt */
    re15_msgtakt_r41_takt_ende();
    PRUEF(g_re15_pauseflags == 0xffff0007u && re15_msgtakt_r41_restore_offen(),
          "Schliessen im Nachrichten-Takt: Spiel-Takt dieses Bilds sieht die Pause noch (%08x)", (unsigned)g_re15_pauseflags);
    re15_msgtakt_r41_nach_spieltakt();
    PRUEF(g_re15_pauseflags == 0x7u && !re15_msgtakt_r41_restore_offen(), "hinter dem Spiel-Takt: Restore (%08x)", (unsigned)g_re15_pauseflags);

    re15_msgtakt_r41_takt_beginn();
    oeffnen(0xffff0000u);
    schliessen();
    re15_msgtakt_r41_takt_ende();
    oeffnen(0x00800000u);                           /* neue Nachricht im selben Bild (Spiel-Takt) */
    PRUEF(g_re15_pauseflags == 0x00800007u, "Oeffnen bei offenem Restore: erst Restore, dann neue Maske (%08x)", (unsigned)g_re15_pauseflags);
    re15_msgtakt_r41_nach_spieltakt();
    schliessen();                                   /* ausserhalb des Takts: sofort */
    PRUEF(g_re15_pauseflags == 0x7u, "deren Schliessen stellt den Zustand VOR der ersten wieder her (%08x)", (unsigned)g_re15_pauseflags);

    re15_msgtakt_r41_zuruecksetzen();
    PRUEF(!re15_msgtakt_r41_frage_durchlaufen(0xffff0000u), "inaktiv: Auswahl parkt wie vorher");
    re15_msgtakt_r41_takt_beginn(); re15_msgtakt_r41_takt_ende();
    PRUEF(!re15_msgtakt_r41_frage_durchlaufen(0x00800000u), "Maske ohne Skript-Bit 0x02000000: parkt");
    PRUEF(re15_msgtakt_r41_frage_durchlaufen(0xff800000u), "Maske 0xff800000 (33 der 102 Auswahl-Message_on): laeuft durch");
    g_scd.message_query = 1; g_scd.message_active = 1;
    re15_msgtakt_r41_nach_spieltakt();
    PRUEF(g_scd.message_query == 1, "Frage noch offen: message_query bleibt (msg_block)");
    g_scd.message_active = 0;
    re15_msgtakt_r41_nach_spieltakt();
    PRUEF(g_scd.message_query == 0, "Frage zu: message_query faellt mit dem Restore");
    re15_msgtakt_r41_zuruecksetzen(); re15_pauseflags_clear();
}

/* ============================================================================================ kamerahalt */
static void teil_kamerahalt(void)
{
    printf("== kamerahalt ==\n");
    re15_montage_fx_set_active(0);
    g_scd.cam_change_pending = 0; g_scd.work_vars[0x0A] = 0; g_scd.cam_id = 0;
    PRUEF(re15_kamerahalt_r41_haelt(0x2020) == 0, "erstes Bild des Raums: zeigen");
    PRUEF(re15_kamerahalt_r41_haelt(0x2020) == 0, "angezeigt == angefordert, nicht pending: zeigen");
    g_scd.cam_id = 2;
    PRUEF(re15_kamerahalt_r41_haelt(0x2020) == 1, "angefordert 2 != angezeigt 0 (@0x80021508/@0x80021514): halten");
    g_scd.cam_id = 0; g_scd.cam_change_pending = 1;
    PRUEF(re15_kamerahalt_r41_haelt(0x2020) == 1, "Cut_chg auf denselben Cut (@0x800402f4 ohne Vergleich): halten");
    PRUEF(re15_kamerahalt_r41_haelt(0x2021) == 0, "Raumwechsel: zeigen");
    re15_montage_fx_set_active(1);
    PRUEF(re15_kamerahalt_r41_haelt(0x2021) == 0, "ROOM1240-Montage aktiv: zeigen");
    re15_montage_fx_set_active(0);
    g_scd.cam_change_pending = 0;
}

/* ============================================================================================ frage */
static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return NULL; }
    fclose(f); *n = (size_t)sz; return b;
}

static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0, s_cine_was = 0, s_r41 = 1;
static uint64_t           s_uhr = 1000000;

static void bild(uint16_t pressed, uint16_t held)
{
    s_uhr += 16667; re15_host_clock_set_us(s_uhr);
    s_ctx.pad_current = held; s_ctx.pad_pressed = pressed;
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_letterbox_tick(re15_game_flag_get(1, 27));
    {
        int cine = re15_cine_active();
        if (cine) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was) g_scd.letterbox_countdown = 15;
        s_cine_was = cine;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) g_scd.player_mode = 0;
    }
    if (re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7)) g_scd.player_mode = 2;   /* main.c Skript-Modus */
    if (s_r41) re15_msgtakt_r41_takt_beginn();
    re15_msg_tick(&raw, &len, &id);
    if (s_r41) re15_msgtakt_r41_takt_ende();
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut = s_shown;
    re15_game_step(&s_ctx);
    if (s_r41) re15_msgtakt_r41_nach_spieltakt();
    re15_fade_tick();
}

static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, int band)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100 || re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) { printf("  FEHLER: ROOM%04X\n", room); g_fail++; return -1; }
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(1);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    g_gameflow.character = (room & 0xFu) ? 4 : 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->motion = 0; pl->floor = 0; pl->y = -band * 0x708;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(band);
    s_shown = 0; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    re15_rbj_bind_room(s_rdt.animation, (size_t)s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

typedef struct { int ok, f_msg0, f_msg1, f_zu, f_pos, bild_msg0, luecke, pos_x, pos_z, motion_pos; } frage_t;

static frage_t frage_lauf(int r41)
{
    frage_t r; memset(&r, 0, sizeof r); r.f_msg0 = r.f_msg1 = r.f_zu = r.f_pos = -1;
    s_r41 = r41;
    {   static int s_gemerkt = 0; static re15_game_state_t s_ausgang;   /* jeder Lauf vom selben Spielstand */
        if (!s_gemerkt) { s_ausgang = g_game; s_gemerkt = 1; } else g_game = s_ausgang; }
    re15_msgtakt_r41_zuruecksetzen();
    re15_game_flag_set(3, 0x72, 1);                     /* Kurbel aufgenommen (ROOM2010 sub02 @0x008CA) */
    if (room_boot(0x2020, 2600, -1500, 1024, 1) != 0) return r;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int f = 0; f < 3; f++) bild(0, 0);
    int offen_vorher = 0, id_vorher = -1, letzte_bestaet = -100, frame_bei_msg0 = -1;
    for (int f = 0; f < 600; f++) {
        uint16_t held = 0, pr = 0;
        if (r.f_msg0 < 0) {                              /* gehen, ab der Kante Aktion jedes zweite Bild */
            held = RE15_PAD_BIT_UP;
            if (pl->z <= -3300 && (f & 1)) { held |= RE15_PAD_BIT_SQUARE; pr = RE15_PAD_BIT_SQUARE; }
        } else if (g_scd.message_active && f - letzte_bestaet >= 12) {
            held = pr = RE15_PAD_BIT_SQUARE; letzte_bestaet = f;
        }
        const int af_vorher = pl->anim_frame;
        bild(pr, held);
        const int offen = g_scd.message_active ? 1 : 0, id = g_scd.message_id;
        if (offen && !offen_vorher && id == 0 && r.f_msg0 < 0) { r.f_msg0 = f; frame_bei_msg0 = pl->anim_frame; }
        if (offen && id == 1 && r.f_msg1 < 0) r.f_msg1 = f;
        if (r.f_msg0 >= 0 && r.f_msg1 < 0 && f > r.f_msg0 && pl->anim_frame != af_vorher) r.luecke++;
        if (!offen && offen_vorher && id_vorher == 1 && r.f_zu < 0) r.f_zu = f;
        if (r.f_zu >= 0 && r.f_pos < 0 && pl->x == 3650) {
            r.f_pos = f; r.pos_x = pl->x; r.pos_z = pl->z; r.motion_pos = pl->motion;
        }
        offen_vorher = offen; id_vorher = id;
        if (r.f_pos >= 0 && f > r.f_pos + 2) break;
    }
    r.bild_msg0 = frame_bei_msg0;
    r.ok = r.f_msg0 >= 0 && r.f_msg1 > r.f_msg0 && r.f_zu > r.f_msg1 && r.f_pos > r.f_zu;
    s_r41 = 1;
    re15_msgtakt_r41_zuruecksetzen(); re15_pauseflags_clear();
    return r;
}

static void teil_frage(void)
{
    printf("== frage ==\n");
    frage_t a = frage_lauf(1);
    printf("  Original-Reihenfolge: msg0 Bild %d (Geh-Bild %d), msg1 Bild %d, Frage zu %d, Pos_set %d (%d,%d) mo %d, Luecken %d\n",
           a.f_msg0, a.bild_msg0, a.f_msg1, a.f_zu, a.f_pos, a.pos_x, a.pos_z, a.motion_pos, a.luecke);
    PRUEF(a.ok, "Szene lief (Aktion im Gehen -> msg 0 -> Frage -> Ja -> Pos_set)");
    PRUEF(a.ok && a.luecke == 0, "kein Spieler-Bild zwischen Message_on 0 @0x0A5C und 1 @0x0A62 (Original n9..n47 Bild 9): %d", a.luecke);
    PRUEF(a.ok && a.f_pos == a.f_zu + 1, "Pos_set @0x0A8C im ersten Bild nach dem Schliessen (Original n94 -> n95): zu %d, Pos_set %d", a.f_zu, a.f_pos);
    PRUEF(a.ok && a.pos_z == -3018 && a.motion_pos == 200, "im Pos_set-Bild Geh-Stopp an der Kurbel (3650,-3018), Clip 3 = mo 200 (@0x80032328/@0x80032480, @0x80032088): z %d mo %d",
          a.pos_z, a.motion_pos);
    frage_t b = frage_lauf(0);
    printf("  alte Reihenfolge (Gegenprobe): msg0 %d, msg1 %d, zu %d, Pos_set %d, Luecken %d\n", b.f_msg0, b.f_msg1, b.f_zu, b.f_pos, b.luecke);
    PRUEF(b.ok && b.luecke >= 1 && b.f_pos == b.f_zu + 2, "Gegenprobe ohne Original-Reihenfolge: Luecken-Bild (%d) und Pos_set 2 Bilder nach dem Schliessen (%d)",
          b.luecke, b.f_pos - b.f_zu);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "msgtakt")    || !strcmp(teil, "alle")) teil_msgtakt();
    if (!strcmp(teil, "kamerahalt") || !strcmp(teil, "alle")) teil_kamerahalt();
    if (!strcmp(teil, "frage")      || !strcmp(teil, "alle")) teil_frage();
    printf("%s: %d/%d bestanden\n", g_fail ? "FAIL" : "OK", g_n - g_fail, g_n);
    return g_fail ? 1 : 0;
}
