/*
 * Runde 41 Spur E "kleinkram_b", Punkt 1: Kurbel ROOM2020 — der Spieler-Clip im Kommandowort 4 laeuft als SCHLEIFE,
 * solange +0x1c4 & 4 (Plc_flg(0,4)) steht, sonst haelt er Bild fc-1 (0x80050cb8 Phase 1/2, erreicht ueber
 * FUN_80030660 @0x800306a4). Modul engine/src/plcloop_r41.c (re15_plcloop_r41.h); Dossier E_kleinkram_b.md.
 * Muster: test_r41_kleinkram_b_11b1.c (Raumstart + Spielbild).
 *
 *   laenge    Cliplaenge aus der Raum-RBJ: ROOM2020 rec0 (Marker 1 = Spieler) Clip 0 = 40 Bilder (RBJ @0xBB0);
 *             ohne RBJ / anim_use_pl00 -> PL00 (0 ohne gespiegelte Bank); ausser Bereich 0.
 *   schleife  ROOM2020 (3,0x72)=1: Kurbel-AOT Slot 3 (sce 3 @0x009AE) gefeuert, Frage bestaetigt -> sub02 @0x00A9C
 *             Plc_motion(0,0,0) + @0x00AA0 Plc_flg(0,4): anim_frame laeuft 0..39 und wrappt auf 0 (Umlaeufe >= 2
 *             bis Plc_ret), nie > 39; Elza ROOM2021 ebenso. Gegenprobe: ohne das Flag (Plc_flg vor dem Takt
 *             zurueckgesetzt) haelt der Clip bei 39.
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
#include "re15_plcloop_r41.h"
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
static int                s_shown = 0, s_bild = 0, s_cine_was = 0;
static uint16_t           s_rdt_raum = 0;
static uint64_t           s_uhr = 1000000;

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

static void bild(uint16_t pressed, uint16_t held)
{
    s_uhr += 16667; re15_host_clock_set_us(s_uhr);
    s_ctx.pad_current = held; s_ctx.pad_pressed = pressed;
    if (re15_menu_gameplay_frozen()) { re15_game_step(&s_ctx); re15_fade_tick(); s_bild++; return; }
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
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut = s_shown;
    re15_game_step(&s_ctx);
    re15_fade_tick();
    s_bild++;
}

static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, int band)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
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
    s_shown = 0; s_bild = 0; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    /* Raum-RBJ binden wie der Raumlader (platform main.c re15_rbj_bind_room: RDT @0x5C animation) */
    re15_rbj_bind_room(s_rdt.animation, (size_t)s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

/* ============================================================================================ laenge */
static void teil_laenge(void)
{
    printf("== laenge ==\n");
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (room_boot(0x2020, 2600, -2800, 2048, 1) != 0) return;
    pl->motion = 0; pl->anim_use_pl00 = 0;
    PRUEF(re15_plcloop_r41_clip_len(pl) == 40, "ROOM2020 RBJ rec0 (Marker 1) Clip 0: %d Bilder (Soll 40, RBJ @0xBB0)", re15_plcloop_r41_clip_len(pl));
    pl->motion = 1;
    PRUEF(re15_plcloop_r41_clip_len(pl) == 0, "Clip 1 ausser Bereich (1 Clip in der RBJ): %d (Soll 0)", re15_plcloop_r41_clip_len(pl));
    pl->motion = 0; pl->anim_use_pl00 = 1;
    PRUEF(re15_plcloop_r41_clip_len(pl) == re15_player_pl00_clip_frames(0), "anim_use_pl00: PL00-Bank (%d)", re15_plcloop_r41_clip_len(pl));
    pl->anim_use_pl00 = 0;
    if (room_boot(0x2010, 0, 0, 0, 0) == 0) {
        pl->motion = 0;
        PRUEF(re15_plcloop_r41_clip_len(pl) == re15_player_pl00_clip_frames(0), "ROOM2010 ohne RBJ: PL00-Bank (%d)", re15_plcloop_r41_clip_len(pl));
    }
    if (room_boot(0x2021, 2600, -2800, 2048, 1) == 0) {
        pl->motion = 0;
        PRUEF(re15_plcloop_r41_clip_len(pl) == 40, "ROOM2021 (Elza) RBJ Clip 0: %d Bilder", re15_plcloop_r41_clip_len(pl));
    }
}

/* ============================================================================================ schleife */
typedef struct { int max_frame, umlaeufe, plc_frames, frames_nach_plc, halt_bild; } lauf_t;

/* Kurbel-Szene fahren: AOT Slot 3 feuern (= Aktionstaste an der Kurbel, Handler [3] @0x800430F0), Fragen mit
 * Quadrat bestaetigen, dann bis Plc_ret (player_mode faellt) oder 400 Bilder ticken. ohne_flag: Plc_flg(0,4) jeden
 * Takt wieder loeschen (Gegenprobe Halten). */
static lauf_t kurbel(uint16_t room, int ohne_flag)
{
    lauf_t r = { -1, 0, 0, 0, -1 };
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    scd_vm_init();
    re15_game_flag_set(3, 0x72, 1);                     /* Kurbel aufgenommen (ROOM2010 sub02 @0x008CA) */
    if (room_boot(room, 2600, -2800, 2048, 1) != 0) return r;
    for (int f = 0; f < 5; f++) bild(0, 0);
    re15_aot_fire_slot(3);                              /* Slot 3 sce 3 (sub00 @0x009AE) -> sub02 */
    unsigned u0 = re15_plcloop_r41_umlaeufe();
    int plc_seen = 0, vorher = -1;
    for (int f = 0; f < 420; f++) {
        uint16_t pr = (!plc_seen && (f % 20) == 10) ? RE15_PAD_BIT_SQUARE : 0;   /* Fragen bestaetigen */
        bild(pr, pr);
        if (ohne_flag) pl->anim_flags &= (uint8_t)~0x04u;
        if (!plc_seen && pl->state == 4 && pl->motion == 0 && !pl->anim_use_pl00) { plc_seen = 1; r.plc_frames = f; }
        if (plc_seen) {
            if (pl->state != 4) break;                                      /* Plc_ret @0x00B16: Steuerung zurueck */
            r.frames_nach_plc++;
            if ((int)pl->anim_frame > r.max_frame) r.max_frame = pl->anim_frame;
            if (vorher >= 30 && pl->anim_frame == 0) r.umlaeufe++;
            if (ohne_flag && pl->anim_frame == 39 && r.halt_bild < 0) r.halt_bild = f;
            vorher = pl->anim_frame;
        }
    }
    (void)u0;
    return r;
}

static void teil_schleife(void)
{
    printf("== schleife ==\n");
    lauf_t a = kurbel(0x2020, 0);
    printf("  Leon: Plc_motion nach %d Bildern, %d Bilder im Kommandowort 4, max anim_frame %d, Umlaeufe %d\n",
           a.plc_frames, a.frames_nach_plc, a.max_frame, a.umlaeufe);
    PRUEF(a.frames_nach_plc >= 100, "Kurbel-Szene lief (>= 100 Bilder im Kommandowort 4: %d)", a.frames_nach_plc);
    PRUEF(a.max_frame <= 39, "anim_frame nie ueber 39 (max %d) - Clip-Ende bekannt", a.max_frame);
    PRUEF(a.umlaeufe >= 2, "Schleife: >= 2 Umlaeufe 39 -> 0 (Plc_flg(0,4) @0x00AA0, Phase 1 @0x80050dbc-c8): %d", a.umlaeufe);
    lauf_t b = kurbel(0x2020, 1);
    printf("  Leon ohne Flag: max %d, Umlaeufe %d, Halt ab Bild %d\n", b.max_frame, b.umlaeufe, b.halt_bild);
    PRUEF(b.frames_nach_plc >= 100 && b.umlaeufe == 0 && b.max_frame == 39, "ohne +0x1c4&4: Halten bei 39 (@0x80050da4), kein Umlauf (%d)", b.umlaeufe);
    lauf_t c = kurbel(0x2021, 0);
    printf("  Elza: %d Bilder, max %d, Umlaeufe %d\n", c.frames_nach_plc, c.max_frame, c.umlaeufe);
    PRUEF(c.frames_nach_plc >= 100 && c.max_frame <= 39 && c.umlaeufe >= 2, "Elza ROOM2021: Schleife (%d Umlaeufe)", c.umlaeufe);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "laenge")   || !strcmp(teil, "alle")) teil_laenge();
    if (!strcmp(teil, "schleife") || !strcmp(teil, "alle")) teil_schleife();
    printf("%s: %d/%d bestanden\n", g_fail ? "FAIL" : "OK", g_n - g_fail, g_n);
    return g_fail ? 1 : 0;
}
