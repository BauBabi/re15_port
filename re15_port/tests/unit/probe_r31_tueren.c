/* probe_r31_tueren.c — Riegel der Runde 31 "Tueren" (analysis/befunde_runde31/tueren_04_bau.md).
 *
 * Teile (Aufruf mit dem Teilnamen, je ein ctest):
 *   viereck   Die 40-Byte-Tuersaetze ROOM4030/4031 @0x47E / @0x4A6 (sat 0xB1): Punkte pc+6..21,
 *             Nutzlast pc+22 (@0x80042f90 addiu a0,s0,20), Trefftest FUN_80014368. Vorher las der
 *             Port sie mit dem 32-B-Schema und stellte Tueren mit Ziel "BF0A0"/"C5F00" auf
 *             (re15_port/tools/engine_tueren.txt Zeilen 184/185). Geprueft: Installation (Punkte,
 *             Ziel, Lage, Cut), der Trefftest Befehl fuer Befehl, und ein echter Durchgang im
 *             Spielschritt (Standplatz vor der Tuer, QUADRAT) -> Raumwechsel nach ROOM4040 bzw.
 *             ROOM4080 mit der Lage aus der Nutzlast.
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
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 */
#include "re15_door_seq.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_fehler = 0;
#define PRUEF(c, ...) do { if (!(c)) { g_fehler++; printf("FEHLER: " __VA_ARGS__); printf("\n"); } } while (0)

/* ---------------------------------------------------------------------------------------------
 * Spielschritt-Geruest (wie probe_r30_tuer_verschlossen.c): Raum booten, Einschwingen, Bilder.
 * ------------------------------------------------------------------------------------------ */
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static int                s_shown = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    scd_audio_event_t e;
    scd_vm_tick();
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    while (scd_audio_queue_pop(&e)) { }
}

static int room_boot(uint16_t room)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR,
             (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100) { printf("FEHLER: %s fehlt\n", rp); g_fehler++; return -1; }
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) { printf("FEHLER: RDT-Parse %s\n", rp); g_fehler++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = 0; pl->z = 0; pl->rot_y = 0;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    s_shown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 30; f++) frame(0, 0);
    int warm = 0;
    while (warm < 3000 &&
           (g_scd.player_mode == 2 || g_scd.letterbox_countdown != 0 || g_scd.message_active ||
            g_scd.message_display_frames > 0 || g_scd.message_query)) {
        uint16_t e = ((warm % 10) == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
        warm++;
    }
    g_room_change.pending = 0;
    return 0;
}

/* Vorwaertspunkt 620 vor dem Spieler (FUN_80042bac @0x80042bd0 ori 0x26c) in der Tuerflaeche? */
static int vorwaerts_trifft(const re15_aot_t *a, int32_t px, int32_t pz, int yaw)
{
    int32_t c = re15_cos_q12(yaw), s = re15_sin_q12(yaw);
    int32_t fx = px + (int32_t)((620 * c) >> 12);
    int32_t fz = pz - (int32_t)((620 * s) >> 12);
    if (a->has_quad) return re15_aot_point_in_quad_fun80014368(fx, fz, a->xs, a->zs);
    long dx = (long)fx - (long)a->x, dz = (long)fz - (long)a->z;
    if (dx < 0) dx = -dx;
    if (dz < 0) dz = -dz;
    return dx <= a->half_w && dz <= a->half_h;
}

/* Standplatz vor der Tuer suchen (acht Blickrichtungen x fuenf Zielpunkte), den die
 * Kollision nicht verschiebt, und dort QUADRAT druecken. Rueckgabe 1 = Raumwechsel angefragt. */
static int durchgehen(int slot, int *gefunden_out)
{
    re15_aot_t *a = &g_aot.slots[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int band = g_aot.door_params[slot].band;
    long ax = a->x, az = a->z;
    if (a->has_quad) {
        ax = ((long)a->xs[0] + a->xs[1] + a->xs[2] + a->xs[3]) / 4;
        az = ((long)a->zs[0] + a->zs[1] + a->zs[2] + a->zs[3]) / 4;
    }
    static const int dxf[5] = { 0, 1, -1, 0, 0 }, dzf[5] = { 0, 0, 0, 1, -1 };
    int gefunden = 0;
    for (int k = 0; k < 5 && !gefunden; k++) {
        long zx = ax + dxf[k] * (long)(a->has_quad ? 150 : a->half_w / 2);
        long zz = az + dzf[k] * (long)(a->has_quad ? 150 : a->half_h / 2);
        for (int d = 0; d < 16 && !gefunden; d++) {
            int yaw = d * 256;
            int32_t c = re15_cos_q12(yaw), s = re15_sin_q12(yaw);
            int32_t px = (int32_t)zx - (int32_t)((620 * c) >> 12);
            int32_t pz = (int32_t)zz + (int32_t)((620 * s) >> 12);
            pl->rot_y = (int16_t)yaw; pl->x = px; pl->z = pz;
            if (!(band & 0x80)) { re15_collision_set_band(band); pl->floor = (uint8_t)band; }
            frame(0, 0);
            g_room_change.pending = 0;
            if (pl->x == px && pl->z == pz && (int)pl->rot_y == yaw && vorwaerts_trifft(a, px, pz, yaw))
                gefunden = 1;
        }
    }
    if (gefunden_out) *gefunden_out = gefunden;
    if (!gefunden) return 0;
    for (int f = 0; f < 30; f++) {
        uint16_t e = (f == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
        if (g_room_change.pending) return 1;
    }
    return 0;
}

/* ---------------------------------------------------------------------------------------------
 * Teil "viereck"
 * ------------------------------------------------------------------------------------------ */
typedef struct {
    int slot;
    int16_t xs[4], zs[4];
    uint8_t stage, raum, cut;
    int16_t x, y, z, dir;
} viereck_soll_t;

/* Satzbytes selbst gelesen (ROOM4030.RDT == ROOM4031.RDT an beiden Stellen):
 * @0x47E 3b 01 02 b1 00 00 | 82 a1 6e 9c ae 9d 40 98 fa 97 7c 9d 5e 9d 86 a2 |
 *        be 0a 00 00 86 24 00 06 | 03 04 0c 00 | 00 00 ...
 * @0x4A6 3b 02 02 b1 00 00 | f6 af f0 a1 38 b4 12 9e fc ae a4 98 b0 aa 68 9d |
 *        c4 f0 00 00 34 08 00 02 | 03 08 08 00 | 00 00 ... */
static const viereck_soll_t s_viereck[2] = {
    { 1, { -24190, -25170, -26630, -25250 }, { -25490, -26560, -25220, -23930 },
      3, 0x04, 12, 2750, 0, 9350, 1536 },
    { 2, { -20490, -19400, -20740, -21840 }, { -24080, -25070, -26460, -25240 },
      3, 0x08, 8, -3900, 0, 2100, 512 },
};

static void teil_viereck(void)
{
    /* 1. Trefftest Befehl fuer Befehl (FUN_80014368) */
    {
        const viereck_soll_t *v = &s_viereck[0];
        PRUEF(re15_aot_point_in_quad_fun80014368(-25310, -25300, v->xs, v->zs) == 1, "Viereck 1: Mitte muss treffen");
        PRUEF(re15_aot_point_in_quad_fun80014368(-23000, -25300, v->xs, v->zs) == 0, "Viereck 1: ausserhalb darf nicht");
        /* feste Umlaufrichtung: dasselbe Viereck andersherum umlaufen trifft im Original nie */
        int16_t rx[4] = { v->xs[0], v->xs[3], v->xs[2], v->xs[1] }, rz[4] = { v->zs[0], v->zs[3], v->zs[2], v->zs[1] };
        PRUEF(re15_aot_point_in_quad_fun80014368(-25310, -25300, rx, rz) == 0,
              "Viereck 1 umgekehrt: FUN_80014368 prueft feste Umlaufrichtung");
        /* 32-Bit-Produkte (mult/mflo): ein weit entfernter Punkt ueberlaeuft nicht ins Treffen */
        PRUEF(re15_aot_point_in_quad_fun80014368(30000, 30000, v->xs, v->zs) == 0, "Viereck 1: ferner Punkt");
        const viereck_soll_t *w = &s_viereck[1];
        PRUEF(re15_aot_point_in_quad_fun80014368(-20617, -25212, w->xs, w->zs) == 1, "Viereck 2: Mitte muss treffen");
    }

    /* 2. Installation + 3. Durchgang, beide Varianten */
    const uint16_t raeume[2] = { 0x4030, 0x4031 };
    for (int r = 0; r < 2; r++) {
        for (int i = 0; i < 2; i++) {
            if (room_boot(raeume[r]) != 0) return;
            const viereck_soll_t *v = &s_viereck[i];
            const re15_aot_t *a = &g_aot.slots[v->slot];
            const re15_aot_door_params_t *d = &g_aot.door_params[v->slot];
            int pts_ok = 1;
            for (int k = 0; k < 4; k++) if (a->xs[k] != v->xs[k] || a->zs[k] != v->zs[k]) pts_ok = 0;
            unsigned ziel = (((unsigned)d->dest_stage + 1u) << 12) | ((unsigned)d->dest_room << 4)
                          | (raeume[r] & 0xFu);
            unsigned ziel_soll = (((unsigned)v->stage + 1u) << 12) | ((unsigned)v->raum << 4) | (raeume[r] & 0xFu);
            printf("ROOM%04X Slot %d: Typ %d Viereck %d Punkte %s Ziel ROOM%04X Lage (%d,%d,%d) Richtung %d Cut %u\n",
                   raeume[r], v->slot, a->type, a->has_quad, pts_ok ? "gleich" : "ANDERS", ziel,
                   (int)d->spawn_x, (int)d->spawn_y, (int)d->spawn_z, d->spawn_yaw_4096, d->target_cut);
            PRUEF(a->active && a->type == RE15_AOT_TYPE_DOOR, "ROOM%04X Slot %d keine Tuer", raeume[r], v->slot);
            PRUEF(a->has_quad == 1 && pts_ok, "ROOM%04X Slot %d: Viereck-Punkte", raeume[r], v->slot);
            PRUEF(ziel == ziel_soll, "ROOM%04X Slot %d: Ziel ROOM%04X, soll ROOM%04X", raeume[r], v->slot, ziel, ziel_soll);
            PRUEF(d->spawn_x == v->x && d->spawn_y == v->y && d->spawn_z == v->z && d->spawn_yaw_4096 == v->dir
                  && d->target_cut == v->cut, "ROOM%04X Slot %d: Nutzlast (Lage/Richtung/Cut)", raeume[r], v->slot);
            PRUEF(d->band == 0, "ROOM%04X Slot %d: Band %d", raeume[r], v->slot, d->band);

            int gefunden = 0;
            int wechsel = durchgehen(v->slot, &gefunden);
            printf("  Durchgang: Standplatz %s, Raumwechsel %d -> ROOM%04X Lage (%d,%d,%d) Cut %d\n",
                   gefunden ? "gefunden" : "KEINER", wechsel, g_room_change.room_id,
                   (int)g_room_change.x, (int)g_room_change.y, (int)g_room_change.z, g_room_change.target_cut);
            PRUEF(gefunden, "ROOM%04X Slot %d: kein Standplatz vor der Tuer", raeume[r], v->slot);
            PRUEF(wechsel && g_room_change.room_id == ziel_soll && g_room_change.x == v->x
                  && g_room_change.z == v->z && g_room_change.target_cut == v->cut,
                  "ROOM%04X Slot %d: Durchgang fuehrt nicht nach ROOM%04X", raeume[r], v->slot, ziel_soll);
            g_room_change.pending = 0;
        }
    }
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "viereck";
    if (!strcmp(teil, "viereck")) teil_viereck();
    else { printf("unbekannter Teil %s\n", teil); return 2; }
    printf(g_fehler ? "probe_r31_tueren %s: %d FEHLER\n" : "probe_r31_tueren %s: OK\n", teil, g_fehler);
    return g_fehler ? 1 : 0;
}
