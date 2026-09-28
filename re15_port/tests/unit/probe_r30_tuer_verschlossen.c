/* probe_r30_tuer_verschlossen.c — MESSSONDE Runde 30, Thema "tuer-verschlossen".
 *
 * Frage: Was tut der PORT HEUTE, wenn der Spieler an einer verschlossenen Tuer / einem
 * Kartenleser / einem Code-Feld QUADRAT drueckt — welcher Text geht auf, und erklingt
 * dabei IRGENDEIN Ton?
 *
 * Verfahren (keine Annahme ueber einzelne Tueren): je Raum wird JEDER nach dem Aufbau
 * aktive AOT-Platz der Arten MESSAGE / GENERIC / EXAMINE_WORKVAR / DOOR einzeln
 * angefahren — frischer Raumaufbau, Spieler so gestellt, dass der 620er-Vorwaertspunkt
 * (FUN_80042bac @0x80042bd0 `ori 0x26c`) genau auf der Platzmitte liegt, Band des Platzes
 * eingestellt, EIN QUADRAT-Druck. Danach 150 Bilder Protokoll:
 *   - jede aufgehende Nachricht (Id + Text),
 *   - jeder SE-Aufruf an den fuenf Bank-Spionen aus tests/test_support.c
 *     (snd1 re15_audio_room_se, snd0 re15_audio_room_se_snd0, CORE re15_audio_core_se,
 *      RE2-Panel re15_audio_re2_panel_se, RE2-Fahrstuhl re15_audio_re2_elevator_se),
 *   - jedes SCD-Se_on, das die VM in ihre Audio-Warteschlange legt (Bank/Id).
 * Frische Flags = frisches Spiel = die Schloesser sind zu.
 *
 * Aufruf: probe_r30_tuer_verschlossen <raum-hex> [<raum-hex> ...]
 * Ausgabe: eine Zeile je Platz; Summenzeile am Ende. Exit 0 (reine Messung).
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t         g_scd;
extern re15_aot_state_t g_aot;
extern uint32_t         g_re15_pauseflags;

/* Spione aus tests/test_support.c */
extern int g_test_room_se_n;
extern int g_test_snd0_se_last, g_test_snd0_se_count;
extern int g_test_core_se_last, g_test_core_se_count;
extern int g_test_elev_se_last, g_test_elev_se_count;
extern int g_test_panel_se_last, g_test_panel_se_count;

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static int                s_shown = 0;
static int                s_warm  = 0;   /* Bilder Einschwingen bis Spielzustand */

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* Audio-Warteschlange der VM leeren und die Se_on zaehlen. */
static int s_q_se = 0;
static char s_q_txt[256];
static void queue_leeren(int zaehlen)
{
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e)) {
        if (!zaehlen) continue;
        if (e.kind == SCD_AUDIO_SE_ON) {
            char b[32];
            snprintf(b, sizeof b, "%s(%u,0x%02x)", s_q_se ? " " : "",
                     (unsigned)e.bank, (unsigned)e.sample_id);
            if (strlen(s_q_txt) + strlen(b) + 1 < sizeof s_q_txt) strcat(s_q_txt, b);
            s_q_se++;
        }
    }
}

static void frame(uint16_t held, uint16_t edge, int zaehlen)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    queue_leeren(zaehlen);
}

static int room_boot(uint16_t room)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR,
             (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100) { printf("SKIP: %s fehlt/Platzhalter\n", rp); return -1; }
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) { printf("FAIL: RDT-Parse %s\n", rp); return -1; }

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
    for (int f = 0; f < 30; f++) frame(0, 0, 0);
    /* EINSCHWINGEN: eine Eintritts-Zwischensequenz (player_mode == 2 / Letterbox) sperrt in
     * aot_common.c ALLE Aktions-Plaetze (`in_cinematic`). Gemessen werden soll der Druck im
     * SPIEL, also laeuft die Sequenz hier erst zu Ende; offene Texte werden weitergeblaettert. */
    s_warm = 0;
    while (s_warm < 3000 &&
           (g_scd.player_mode == 2 || g_scd.letterbox_countdown != 0 || g_scd.message_active ||
            g_scd.message_display_frames > 0 || g_scd.message_query)) {
        uint16_t e = ((s_warm % 10) == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e, 0);
        s_warm++;
    }
    return 0;
}

static const char *type_name(uint8_t t)
{
    switch (t) {
    case RE15_AOT_TYPE_GENERIC:         return "EVENT";
    case RE15_AOT_TYPE_DOOR:            return "TUER";
    case RE15_AOT_TYPE_MESSAGE:         return "TEXT";
    case RE15_AOT_TYPE_EXAMINE_WORKVAR: return "MARKE5";
    default:                            return "?";
    }
}

static int s_sum_plaetze = 0, s_sum_text = 0, s_sum_ton = 0, s_sum_tuer = 0, s_sum_ohne_stand = 0;

/* Liegt der 620er-Vorwaertspunkt des Spielers im Platz? (dieselbe Rechnung wie
 * aot_common.c: fx = x + 620*cos(yaw), fz = z - 620*sin(yaw), FUN_80042bac @0x80042bd0) */
static int vorwaerts_im_platz(const re15_aot_t *a, int32_t px, int32_t pz, int yaw)
{
    int32_t c = re15_cos_q12(yaw), s = re15_sin_q12(yaw);
    int32_t fx = px + (int32_t)((620 * c) >> 12);
    int32_t fz = pz - (int32_t)((620 * s) >> 12);
    if (a->has_quad) return re15_aot_point_in_quad(fx, fz, a->xs, a->zs);
    long dx = (long)fx - (long)a->x, dz = (long)fz - (long)a->z;
    if (dx < 0) dx = -dx;
    if (dz < 0) dz = -dz;
    return dx <= a->half_w && dz <= a->half_h;
}

static int s_ktl_se1 = 0, s_ktl_rest = 0;   /* Kontrolllauf OHNE Druck: dieselben 150 Bilder */

static void messen(uint16_t room, int slot, int druck)
{
    if (room_boot(room) != 0) return;
    re15_aot_t *a = &g_aot.slots[slot];
    if (!a->active) return;
    uint8_t typ = a->type, ev = a->event_id, band = a->band, fl = a->sce_flags;
    long ax = (long)a->x, az = (long)a->z;
    if (a->has_quad) {
        ax = ((long)a->xs[0] + a->xs[1] + a->xs[2] + a->xs[3]) / 4;
        az = ((long)a->zs[0] + a->zs[1] + a->zs[2] + a->zs[3]) / 4;
    }
    if (typ == RE15_AOT_TYPE_DOOR) band = g_aot.door_params[slot].band;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* STANDPLATZ SUCHEN. Die Platzmitte einer Tuer liegt meist IN der Wand; game_step
     * faehrt vor dem Scan Spieler-Tick + Wandklemme, und die schiebt einen Spieler aus
     * der Wand. Deshalb: acht Blickrichtungen x fuenf Zielpunkte im Platz durchprobieren,
     * je EIN Bild ohne Eingabe laufen lassen und den ersten Stand nehmen, den die
     * Kollision NICHT verschiebt und dessen Vorwaertspunkt im Platz liegt. */
    static const int dxf[5] = { 0, 1, -1, 0, 0 }, dzf[5] = { 0, 0, 0, 1, -1 };
    int gefunden = 0, g_yaw = 0; int32_t g_x = 0, g_z = 0; int versuche = 0;
    for (int k = 0; k < 5 && !gefunden; k++) {
        long zx = ax + dxf[k] * (long)(a->has_quad ? 0 : a->half_w / 2);
        long zz = az + dzf[k] * (long)(a->has_quad ? 0 : a->half_h / 2);
        for (int d = 0; d < 8 && !gefunden; d++) {
            int yaw = d * 512;
            int32_t c = re15_cos_q12(yaw), s = re15_sin_q12(yaw);
            int32_t px = (int32_t)zx - (int32_t)((620 * c) >> 12);
            int32_t pz = (int32_t)zz + (int32_t)((620 * s) >> 12);
            pl->rot_y = (int16_t)yaw; pl->x = px; pl->z = pz;
            if (!(band & 0x80)) { re15_collision_set_band((int)band); pl->floor = band; }
            frame(0, 0, 0);
            versuche++;
            if (g_room_change.pending || g_scd.message_active) { g_room_change.pending = 0; }
            if (pl->x == px && pl->z == pz && (int)pl->rot_y == yaw &&
                vorwaerts_im_platz(a, pl->x, pl->z, yaw)) {
                gefunden = 1; g_yaw = yaw; g_x = px; g_z = pz;
            }
        }
    }

    int se1_0 = g_test_room_se_n, sn0_0 = g_test_snd0_se_count, core_0 = g_test_core_se_count;
    int pan_0 = g_test_panel_se_count, elv_0 = g_test_elev_se_count;
    s_q_se = 0; s_q_txt[0] = 0;
    g_room_change.pending = 0;

    int z_mode = (int)g_scd.player_mode, z_lb = (int)g_scd.letterbox_countdown;
    int z_msg = (int)g_scd.message_active, z_warm = s_warm;
    uint32_t z_pause = g_re15_pauseflags;
    int pb = re15_collision_debug_band();
    int msg_n = 0, msg_ids[8]; int prev_act = 0, prev_id = -1; int tuer = 0;
    if (gefunden && !z_msg) {
        for (int f = 0; f < 150; f++) {
            uint16_t e = 0;
            if (f == 0 && druck) e = RE15_PAD_BIT_SQUARE;
            else if (g_scd.message_active && (f % 10) == 0) e = RE15_PAD_BIT_SQUARE;  /* blaettern */
            frame(e, e, 1);
            int act = (int)g_scd.message_active, id = (int)g_scd.message_id;
            if (act && (!prev_act || id != prev_id)) {
                if (msg_n < 8) msg_ids[msg_n++] = id;
            }
            prev_act = act; prev_id = id;
            if (g_room_change.pending) { tuer = 1; break; }
        }
    }
    int d_se1 = g_test_room_se_n - se1_0, d_sn0 = g_test_snd0_se_count - sn0_0;
    int d_core = g_test_core_se_count - core_0, d_pan = g_test_panel_se_count - pan_0;
    int d_elv = g_test_elev_se_count - elv_0;
    int ton = d_se1 + d_sn0 + d_core + d_pan + d_elv + s_q_se;

    if (!druck) {            /* Kontrolllauf: nur die Zaehler merken, keine Zeile */
        s_ktl_se1  = d_se1;
        s_ktl_rest = d_sn0 + d_core + d_pan + d_elv + s_q_se;
        return;
    }
    s_sum_plaetze++;
    if (!gefunden) s_sum_ohne_stand++;
    if (msg_n) s_sum_text++;
    if (ton) s_sum_ton++;
    if (tuer) s_sum_tuer++;

    printf("ROOM%04X platz %2d %-6s ev=%-3u flags=0x%02x band=0x%02x mitte(%ld,%ld) | ",
           (unsigned)room, slot, type_name(typ), (unsigned)ev, (unsigned)fl, (unsigned)band, ax, az);
    if (!gefunden) printf("KEIN STANDPLATZ (%d Versuche) | ", versuche);
    else if (z_msg) printf("TEXT STAND SCHON OFFEN | ");
    else if (tuer) printf("RAUMWECHSEL | ");
    else if (!msg_n) printf("kein Text | ");
    for (int i = 0; i < msg_n; i++) {
        const char *t = re15_msg_get_text(msg_ids[i]);
        printf("msg %d \"%.70s\"%s", msg_ids[i], t ? t : "?", (i + 1 < msg_n) ? " ; " : " | ");
    }
    printf("TON: snd1=%d snd0=%d core=%d re2panel=%d re2elev=%d scd_se_on=%d%s%s",
           d_se1, d_sn0, d_core, d_pan, d_elv, s_q_se, s_q_se ? " " : "", s_q_txt);
    printf(" | KONTROLLE ohne Druck: snd1=%d uebrige=%d", s_ktl_se1, s_ktl_rest);
    printf(" | stand(%ld,%ld) yaw=%d band_ist=%d einschwingen=%d mode=%d letterbox=%d pause=0x%08x\n",
           (long)g_x, (long)g_z, g_yaw, pb, z_warm, z_mode, z_lb, (unsigned)z_pause);
}

int main(int argc, char **argv)
{
    if (argc < 2) { printf("Aufruf: %s <raum-hex> ...\n", argv[0]); return 2; }
    for (int k = 1; k < argc; k++) {
        uint16_t room = (uint16_t)strtoul(argv[k], NULL, 16);
        if (room_boot(room) != 0) continue;
        /* Plaetze des frisch aufgebauten Raums einsammeln */
        int slots[RE15_AOT_MAX], n = 0;
        for (int i = 0; i < RE15_AOT_MAX; i++) {
            const re15_aot_t *a = &g_aot.slots[i];
            if (!a->active) continue;
            if (a->type == RE15_AOT_TYPE_MESSAGE || a->type == RE15_AOT_TYPE_GENERIC ||
                a->type == RE15_AOT_TYPE_EXAMINE_WORKVAR || a->type == RE15_AOT_TYPE_DOOR)
                slots[n++] = i;
        }
        for (int j = 0; j < n; j++) { messen(room, slots[j], 0); messen(room, slots[j], 1); }
    }
    printf("SUMME: %d Plaetze, %d ohne Standplatz (nicht gemessen), %d mit Text, %d mit Raumwechsel, "
           "%d mit IRGENDEINEM Ton\n",
           s_sum_plaetze, s_sum_ohne_stand, s_sum_text, s_sum_tuer, s_sum_ton);
    return 0;
}
