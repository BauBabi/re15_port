/*
 * test_r41_birkin_aufrichten.c — Runde 41 "birkin_aufrichten": nach dem Sprung von oben / dem Aufrichten zeigt der Birkin im
 * Wechselbild NICHT mehr das erste Bild des fertigen Clips (Hocke) und blendet den naechsten Clip mit dem Original-Gewicht.
 * Nutzerbefund 16.11.2026 (Kraftraum 50E0/50F1, "wie bei anderen Birkin in Stage3 auch").
 *
 * Original-Regel (Version_40%/PSX.EXE, anim_set FUN_8001f314): `lbu v0,149(t0)` @0x8001f35c -> `sw a2,360(t0)` @0x8001f36c, Pose
 * FUN_8001f3bc, ERST DANACH +0x95 += 1 (@0x8001f610-61c); am Clip-Ende `sb zero,149(v1)` @0x8001f63c + Rueckgabe 1 (@0x8001f638).
 * Im Wechselbild steht also das LETZTE Bild; der naechste Clip blendet mit +0x8f * a3 (`mult v0,a3` @0x8001f474) aus dieser Pose.
 * Port-Gegenstueck enemy_ai_common.c re15_birkin_anim_szene (Runde 42 T-03/T-04, 9fe525fb2, woertlich uebernommen).
 *
 * Teile (Aufruf: test_r41_birkin_aufrichten <teil>):
 *   kraftraum_leon  ROOM50E0 Sce_em_set grid 0x33 (@0x00ACE, y -5600) -> Ausloeser AOT Slot 1 -> Sub 9 (Emergence STAGE5 0x80119b8c,
 *                   code-gleich STAGE3 0x80119378): Wechselbild Clip 0x10 = letztes Bild, Clip 0xd mit a3 0x100 (@0x80119cb8),
 *                   Ende Clip 0xd = letztes Bild bis zum Ansprung
 *   kraftraum_elza  dasselbe in ROOM50F1 (@0x009AC)
 *   s3070           ROOM3070 (Leon), Emergence STAGE3 0x80119378 (a3 0x100 @0x80119430/@0x801194a4)
 *   s3071           ROOM3071 (Elza), Auftritt Sub 10 STAGE3 0x80119524: Clipende 0xf (Phase 3 -> 4) und 0xd (Phase 7 -> 8)
 *                   im Wechselbild = letztes Bild, Gewicht a3 0x100 (@0x801196fc/@0x80119918 u. a.)
 *   s3080           ROOM3080 Hand im Aufzug (Szenen-Modus 0x80118d00): Wechselbild nach Stoss 1 = letztes Bild, a3 0x400
 */
#if !defined(_WIN32) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE 1
#endif
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
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_fade.h"
#include "re15_inventory.h"
#include "re15_item_discard.h"
#include "re15_gameflow.h"
#include "re15_light.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

void re15_actor_step_all_walkers(void);
int  re15_cine_active(void);
int  re15_cam_present_tick(void);

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

/* ---- Harness (Muster test_r41_gatorszene.c / test_r42_story3b.c) ------------------------------------------------- */
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
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    g_room_rdt = s_rdt;
    g_re15_room_lights_ok = (s_rdt.lights && re15_light_parse(s_rdt.lights, (size_t)s_rdt.lights_size, &g_re15_room_lights) == 0);
    return 0;
}
static void frame(void)
{
    const unsigned char *raw; int len, id;
    if (re15_discard_frozen()) return;
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
    s_ctx.pad_current = 0;
    s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e)) { }
    re15_fade_tick();
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
    s_shown = cut; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}
static int typ_slot(uint8_t typ)
{
    for (int i = 1; i < RE15_ACTOR_MAX; i++) if (g_actors[i].active && g_actors[i].type == typ) return i;
    return -1;
}
/* Bank eines Typs aus CDEMD0.EMS wie der Raumlader. Die Kraftraum-Form EM30_HANGAR.EMD traegt die unveraenderte EDD des EM30
 * (birkin_formen.c Kopf BF_HALT_N) -> dieselben Clip-Laengen. */
static uint8_t s_ems_buf[0x80000];
static int bank_laden(uint8_t type)
{
    size_t esz = 0, off = 0, len = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &esz);
    if (!ems || re15_ems_get_entry(ems, esz, re15_ems_index_for_type(type), &off, &len) != 0 || len > sizeof s_ems_buf) {
        free(ems); return 0;
    }
    memcpy(s_ems_buf, ems + off, len); free(ems);
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re15_emd_parse_container(s_ems_buf, len, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) return 0;
    eb->ok = 1; eb->buf = NULL;
    return 1;
}
static int clip_bilder(uint8_t type, int clip)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (!eb || !eb->ok || clip >= eb->anim.clip_count) return -1;
    return eb->anim.clips[clip].frame_count;
}

/* ---- Emergence (Sub 9): Kraftraum 50E0/50F1 und 3070 --------------------------------------------------------------- */
static void emergence(const char *name, uint16_t room, int32_t px, int32_t pz, int16_t rot, int aot_slot)
{
    printf("== %s (ROOM%04X) ==\n", name, (unsigned)room);
    re15_game_state_init(); g_gameflow.character = (room & 0xFu) ? 4 : 0;
    if (room_boot(room, px, pz, rot, 0) != 0) return;
    PRUEF(bank_laden(0x30), "EM30 aus CDEMD0.EMS");
    const int fc10 = clip_bilder(0x30, 0x10), fc0d = clip_bilder(0x30, 0x0d);
    PRUEF(fc10 > 2 && fc0d > 2, "Clip 0x10 %d Bilder, Clip 0xd %d Bilder", fc10, fc0d);
    int b = -1, halt10 = -1, rate0d = -1, frac0d = -1, halt0d = -1, wechsel = -1, phase_alt = -1, sub9 = 0, rate_kampf = -1;
    for (int f = 0; f < 3000; f++) {
        /* Ausloesefeld (im Durchlauf laeuft der Spieler hinein). 50E0 beginnt mit einer Szene (Plc_dest), in der der Ausloeser
         * nicht greift -> alle 50 Bilder erneut, bis das Member_set(0x0C,0x13) den Parkzustand grid 0x33 aufhebt. */
        if (f % 50 == 5 && (b < 0 || g_actors[b].grid_id == 0x33)) re15_aot_fire_slot(aot_slot);
        frame();
        if (b < 0) b = typ_slot(0x30);
        if (b < 0) continue;
        re15_actor_t *e = &g_actors[b];
        if (e->sub_state_1 == 9) sub9 = 1;
        if (getenv("BAUF_SPUR") && (f % 10) == 0) printf("    F%d st %d s1 %d s2 %d mo %d af %d gr %02x y %d\n", f, e->state,
                                                     e->sub_state_1, e->sub_state_2, (int)e->motion, (int)e->anim_frame, e->grid_id,
                                                     (int)e->y);
        if (!sub9) continue;
        const int ph = e->sub_state_1 == 9 ? e->sub_state_2 : 5;
        if (ph != phase_alt) {
            if (phase_alt == 1 && ph == 2) halt10 = (int)e->anim_frame;                     /* Wechselbild Clip 0x10 */
            if (phase_alt == 2 && ph == 3) { rate0d = e->anim_blend_rate; frac0d = e->anim_frac; wechsel = e->motion; }
            if (phase_alt == 3 && ph == 4) halt0d = (int)e->anim_frame;
            if (ph == 5) rate_kampf = e->anim_blend_rate;
            phase_alt = ph;
        }
        if (ph == 5) break;
    }
    PRUEF(sub9, "Auftritt Sub 9 (grid-Nibble 3, INIT `05 00 50 a0` @0x801170a4 STAGE5 / @0x80116890 STAGE3)");
    PRUEF(halt10 == fc10 - 1, "Wechselbild Clip 0x10: Bild %d = letztes (%d) statt Umlauf 0 (@0x8001f35c/@0x8001f63c)", halt10, fc10 - 1);
    /* Runde 2 (Pruefer R1): gezeichnet wird +0x8f VOR dem Abzug (`lbu s0,143(v1)` @0x8001f408, `mult v0,a3` @0x8001f474, Abzug erst
     * @0x8001f5b0/@0x8001f5b4) -> das erste Bild von Clip 0xd zeigt 0xf * 0x100 = 3840 (Zeichenpfad wp = anim_frac * rate). */
    PRUEF(wechsel == 0x0d && rate0d == 0x100 && frac0d == 0x0f,
          "Clip 0xd blendet mit a3 0x100, erstes Bild gezeichnet mit frac 0xf = wp %d (soll 3840) (Clip %#x Rate %#x frac %d)",
          frac0d * rate0d, wechsel, rate0d, frac0d);
    PRUEF(halt0d == fc0d - 1, "Ende Clip 0xd: letztes Bild %d (soll %d) bleibt bis zum Ansprung (Phase 4 ohne anim_set)", halt0d, fc0d - 1);
    PRUEF(rate_kampf == 0, "Kampf (Sub 3) ohne die Auftritts-Staerke: Rate %d (r42/story NB1 B1)", rate_kampf);
}

/* ---- Auftritt Sub 10 (3071 Elza) ----------------------------------------------------------------------------------- */
static void auftritt3071(void)
{
    printf("== s3071 (ROOM3071) ==\n");
    re15_game_state_init(); g_gameflow.character = 4;
    if (room_boot(0x3071, -26155, -8635, 2048, 0) != 0) return;
    PRUEF(bank_laden(0x30), "EM30 aus CDEMD0.EMS");
    const int fc0f = clip_bilder(0x30, 0x0f), fc0d = clip_bilder(0x30, 0x0d);
    int b = -1, sub10 = 0, phase_alt = -1, halt0f = -1, halt0d = -1, rate4 = -1, rate8 = -1, af4 = -1, af8 = -1, mo8 = -1, rate_kampf = -1;
    int wp6_min = 99999, wp6_max = -1, n6 = 0, wp7 = -1, n7 = 0, frac4 = -1;
    for (int f = 0; f < 4000; f++) {
        if (f == 5) re15_aot_fire_slot(2);   /* Aot_set Slot 2 @0x0344C -> sub02 */
        frame();
        if (b < 0) b = typ_slot(0x30);
        if (b < 0) continue;
        re15_actor_t *e = &g_actors[b];
        if (e->state == 1 && e->sub_state_1 == 10) sub10 = 1;
        if (getenv("BAUF_SPUR") && (f % 10) == 0) printf("    F%d st %d s1 %d s2 %d mo %d af %d gr %02x\n", f, e->state,
                                                     e->sub_state_1, e->sub_state_2, (int)e->motion, (int)e->anim_frame, e->grid_id);
        if (!sub10) continue;
        const int ph = e->sub_state_1 == 10 ? e->sub_state_2 : 14;
        const int ph_vor = phase_alt;                    /* Phase des vorigen Bildes (das erste ph 6 zeichnet noch Phase 5) */
        if (ph != phase_alt) {
            if (phase_alt == 3 && ph == 4) halt0f = (int)e->anim_frame;                   /* Wechselbild Clip 0xf */
            if (phase_alt == 4 && ph == 5) { rate4 = e->anim_blend_rate; af4 = e->anim_frame; frac4 = e->anim_frac; }
            if (phase_alt == 7 && ph == 8) halt0d = (int)e->anim_frame;                   /* Wechselbild Clip 0xd */
            if (phase_alt == 8 && ph == 9) { rate8 = e->anim_blend_rate; af8 = e->anim_frame; mo8 = e->motion; }
            if (ph == 14) rate_kampf = e->anim_blend_rate;
            phase_alt = ph;
        }
        if (ph == 6 && ph_vor == 6) {                    /* gezeichnetes Gewicht der vorigen Pose (skeleton_common.c wp = frac * rate) */
            const int wp = e->anim_frac * e->anim_blend_rate;
            if (wp < wp6_min) wp6_min = wp;
            if (wp > wp6_max) wp6_max = wp;
            n6++;
        }
        if (ph == 7) {                                   /* n7 0 = das Bild, in dem Phase 6 fertig wurde; 1 = erstes Phase-7-Bild */
            if (n7 == 1) wp7 = e->anim_frac * e->anim_blend_rate;
            n7++;
        }
        if (ph == 14) break;
    }
    PRUEF(sub10, "Auftritt Sub 10 (grid-Nibble 1, ROOM3071 sub02 Member_set(0x0c,1) @0x03645)");
    PRUEF(fc0f > 2 && halt0f == fc0f - 1, "Wechselbild Clip 0xf (Phase 3 -> 4): Bild %d = letztes (%d) statt Umlauf 0", halt0f, fc0f - 1);
    PRUEF(rate4 == 0x100 && af4 == 1, "Phase 4: Clip 1 ab Bild 0 (`sb zero,149` @0x801197f0, gezeichnet 1), a3 0x100 @0x8011982c (Bild %d Rate %#x)",
          af4, rate4);
    PRUEF(frac4 == 0x0f, "Phase 4: erstes Bild von Clip 1 gezeichnet mit +0x8f 0xf vor dem Abzug (@0x801197cc-800, @0x8001f408): frac %d",
          frac4);
    PRUEF(n6 > 0 && wp6_min == 3840 && wp6_max == 3840,
          "Phase 6: jedes Bild +0x8f = 0xf (`sb v0,143(v1)` @0x801198d8) * a3 0x100 (@0x80119918) = 3840 vor dem Abzug (@0x8001f408/@0x8001f474;"
          " %d Bilder, wp %d..%d)", n6, wp6_min, wp6_max);
    PRUEF(wp7 == 0x0e * 0x100, "Phase 7: erstes Bild mit +0x8f 0xe (Abzug des letzten Phase-6-Bildes @0x8001f5b4): wp %d (soll 3584)", wp7);
    PRUEF(fc0d > 2 && halt0d == fc0d - 1, "Wechselbild Clip 0xd (Phase 7 -> 8): Bild %d = letztes (%d) statt Umlauf 0", halt0d, fc0d - 1);
    PRUEF(mo8 == 0x13 && af8 == 1 && rate8 == 0x100,
          "Phase 8: Clip 0x13 ohne +0x95-Schreiben ab Umlaufwert 0 (@0x80119944-970, gezeichnet 1), a3 0x100 @0x8011998c (Clip %#x Bild %d Rate %#x)",
          mo8, af8, rate8);
    PRUEF(rate_kampf == 0, "Kampf (Sub 3, Ende @0x80119ae0) ohne die Auftritts-Staerke: Rate %d", rate_kampf);
}

/* ---- Szenen-Modus 3080 (T-04) -------------------------------------------------------------------------------------- */
static void szene3080(void)
{
    printf("== s3080 (ROOM3080) ==\n");
    re15_game_state_init(); g_gameflow.character = 0;
    if (room_boot(0x3080, -21000, -21000, 0, 0) != 0) return;
    PRUEF(bank_laden(0x36), "EM36 aus CDEMD0.EMS");
    const int fc4 = clip_bilder(0x36, 4);
    int b = -1, phase_alt = -1, halt9 = -1, bild10 = -1, rate10 = -1, frac10 = -1, rate9 = -1, stoesse = 0;
    for (int f = 0; f < 3000; f++) {
        frame();
        if (b < 0) b = typ_slot(0x36);
        if (b < 0) continue;
        re15_actor_t *e = &g_actors[b];
        const int ph = e->sub_state_2;
        if (e->motion == 4 && ph == 9 && rate9 < 0) rate9 = e->anim_blend_rate;
        if (ph != phase_alt) {
            if (ph == 9 || ph == 11) stoesse++;
            if (phase_alt == 9 && ph == 10) halt9 = (int)e->anim_frame;
            if (phase_alt == 9 && ph == 11) halt9 = -2;
            if (ph == 11 && bild10 < 0) { bild10 = (int)e->anim_frame; rate10 = e->anim_blend_rate; frac10 = e->anim_frac; }
            phase_alt = ph;
        }
        if (ph >= 12) break;
    }
    PRUEF(fc4 > 2, "Clip 4 hat %d Bilder", fc4);
    PRUEF(stoesse == 2, "zwei Stoesse (Original @0x801191e4/@0x8011922c): %d", stoesse);
    PRUEF(rate9 == 0x400, "Stoss 1: a3 0x400 (@0x80119274): %#x", rate9);
    PRUEF(halt9 == fc4 - 1, "Wechselbild nach Stoss 1: letztes Bild %d (soll %d)", halt9, fc4 - 1);
    PRUEF(bild10 == 1 && rate10 == 0x400 && frac10 == 3,
          "Stoss 2: Clip 4 ab Bild 0 (gezeichnet 1), gezeichnet frac 3 * a3 0x400 = 3072 vor dem Abzug (@0x80119238-250, @0x8001f408)"
          " (Bild %d Rate %#x frac %d)", bild10, rate10, frac10);
}

int main(int argc, char **argv)
{
    const char *t = argc > 1 ? argv[1] : "";
    /* 50E0: Spieler im Ausloesefeld an der Stelle, an der ihn der Durchlauf (Route test_r41_labor_birkin_angriff) hineintraegt -
     * von der Tuer-Ankunft (-25450,-14700) aus erreicht der Szenen-Gang (Plc_dest (-15300,-16900)) sein Ziel nicht. */
    if (!strcmp(t, "kraftraum_leon"))      emergence(t, 0x50E0, -11605, -15512, 960, 1);
    else if (!strcmp(t, "kraftraum_elza")) emergence(t, 0x50F1, -24550, -23650, 0, 1);
    else if (!strcmp(t, "s3070"))          emergence(t, 0x3070, -26155, -8635, 2048, 2);
    else if (!strcmp(t, "s3071"))          auftritt3071();
    else if (!strcmp(t, "s3080"))          szene3080();
    else { printf("unbekannter Teil '%s'\n", t); return 2; }
    printf("%s: %d Fehler\n", t, g_fail);
    return g_fail ? 1 : 0;
}
