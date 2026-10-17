/* test_r38_montage.c — RIEGEL Runde 38 Spur O "montage": in der Montage nach Irons' Tod gehen die durchbrechenden
 * Zombies in JEDEM Schnitt auf die Kamera zu, so wie die Zombies hinter dem Rolltor (ROOM1040 Cut 1); der Rolltor-
 * Schnitt bleibt unveraendert. Dossier analysis/befunde_runde38/O_montage.md, Mechanik include/re15_montage_r38.h,
 * Programme engine/src/irons_tod_1150.c.
 *
 * NUTZER (AUFTRAG.md Runde 38): "Bei Leon - bei der Cutscene nach Irons Tod, wo die Zombies durchbrechen, sollen sich
 * die Zombies jeweils auch Richtung Kamera bewegen, also so, wie es bei den Zombies hinter dem Rolltor schon passiert.
 * Die anderen sollen das auch machen. Das wirkt auch bedrohlicher."
 *
 * Verfahren wie test_r35_cut1150.c (ausgelieferte RDT, Raumaufbau ueber scd_room_reenter = derselbe Einhaengepunkt wie
 * im Spiel, Bilder in der Reihenfolge von platform/pc/main.c), Gegner-Baenke wie die exe je KI-Wahl (RE2: RE2/CDEMD0.EMS
 * wie test_r37_zombieaktiv.c; RE1.5: PSX/EMD/CDEMD0.EMS wie test_r35_cut1150.c). Jeder Montage-Schritt wird mit dem
 * Zustand der Kette aufgebaut, den der vorige Raum hinterlaesst, und bis zur Tuer-Anforderung gefahren; gemessen wird
 * ueber den Mess-Haken re15_r38_spuren (Abstand zur Bodenprojektion der Kamera je Schnitt und Zombie).
 *
 * Teile (argv[1]):
 *   kameras      Kamera-Tabelle = RID-Bytes der ausgelieferten RDTs (Datei-Offset camera_start + Cut*32); Parkplatz
 *                Cut 12 aus den RID-Bytes nachgerechnet (Abstand der Rolltor-Form, Kameraachse Cut 12, Gierung).
 *   s1130_re2 / s1130_re15     ROOM1130 Cut 0: alle Zombies kommen der Kamera naeher (KI RE2 = Standard / RE1.5);
 *                Einzelstopp nur hinter der Kamera; Spieler-HP unveraendert. Gegenprobe Stand v0.8.25: keiner bewegt sich.
 *   s1030_re2 / s1030_re15     ROOM1030 Cut 7 (Kopien aus 1070) und Cut 12 (Tor): ebenso; nach dem Schnitt auf Cut 12
 *                steht der geparkte Spieler auf der Kameraachse (Parkplatz Cut 12). Gegenprobe Stand v0.8.25.
 *   rolltor      ROOM1040 Cut 1: mit und ohne die Aenderung Bild fuer Bild dieselben Lagen aller Zombies, 5 durchs Tor,
 *                Spieler-HP unveraendert (= Rolltor-Schnitt unveraendert). */
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
#include "re15_ai_flavor.h"
#include "re15_fade.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_tim.h"
#include "re2_ems.h"
#include "re15_irons_tod.h"
#include "re15_montage_r38.h"
#include "re15_damage.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

void re15_actor_step_all_walkers(void);

static int g_fail = 0, g_mess = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

/* Mindest-Annaeherung je Zombie und Schnitt: das Rolltor-Vorbild kommt in 400 Bildern 5230..9430 naeher (Dossier
 * "Messung vorher"); RE2-Original (Runde 37 H R4) geht mindestens 15 Einheiten je Takt. Gefordert: mindestens 1000
 * naeher (der Schnitt 1130 dauert 175 Bilder; 1000 / 175 = 5,7 je Bild — ein stehender Zombie bleibt bei 0..18). */
#define R38_NAEHER_MIN 1000

static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0, s_cine_was = 0;
static uint16_t           s_rdt_raum = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (n) *n = (size_t)sz;
    return b;
}

/* Immer FRISCH von der Platte: Sca_id_set (ROOM1030 sub08 @0x0278E/@0x02792, scd_vm.c op_sca_id_set) aendert die
 * SCA-Zellen im geladenen RDT (wie im Original bis zum naechsten Laden) — ein zweiter Lauf im selben Prozess saehe sonst
 * das offene Tor. Und in g_room_rdt: der Navigator (nav_zone_common.c, BLK-Graph) und die Gegner-Wandklemme
 * (re15_collision_constrain_enemy(&g_room_rdt, ..)) lesen NUR ihn (probe_1030_crawl_live.c Defekt 3) — ohne ihn
 * liefen die Zombies im Riegel durch Waende und Tor (Dossier O_montage.md "Zwischenmessung", Befund 3). */
static int rdt_laden(uint16_t room)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz); s_rdt_raum = 0; g_room_rdt_ok = 0;
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &g_room_rdt) < 0) return -1;
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;
    s_rdt_raum = room;
    return 0;
}
static int32_t s32(const uint8_t *p) { return (int32_t)((uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24)); }
static int16_t s16(const uint8_t *p) { return (int16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8)); }

/* ---- Gegner-Baenke wie die exe je KI-Wahl ------------------------------------------------------------------------ */
static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static void bank_re2(uint8_t type)
{
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_re2_n);
    if (!s_re2_ems) return;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return;
    if (re2_ems_load_bank(s_re2_ems, s_re2_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; }
    else eb->type = 0;
}
static uint8_t *s_ems = NULL; static size_t s_ems_sz = 0;
static uint8_t  s_bank_buf[3][0x60000];
static void bank_re15(uint8_t type, int buf)
{
    if (!s_ems) { char p2[600]; snprintf(p2, sizeof p2, "%s/EMD/CDEMD0.EMS", RE15_ASSET_PSX_DIR); s_ems = slurp(p2, &s_ems_sz); }
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
static void banken(void)
{
    if (re15_ai_flavor() == RE15_AI_FLAVOR_RE2) { bank_re2(0x10); bank_re2(0x11); bank_re2(0x16); }
    else { bank_re15(0x10, 0); bank_re15(0x11, 1); bank_re15(0x16, 2); }
}

/* Ein Spielbild in der Reihenfolge von platform/pc/main.c (wie test_r35_cut1150.c). */
static void frame(void)
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
    s_ctx.pad_current = 0;
    s_ctx.pad_pressed = 0;
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
    banken();
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    re15_damage_seed_rng(0x2545f491u);                     /* reproduzierbar (Muster test_r37_zombieaktiv.c) */
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    pl->floor = (uint8_t)re15_collision_band_from_y(0);   /* wie test_r37_zombieaktiv.c grundzustand */
    re15_collision_set_band(pl->floor);
    s_shown = cut; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

static void szene_flags(void) { re15_game_state_init(); re15_game_flag_set(9, 71, 1); re15_game_flag_set(3, 94, 1); re15_game_flag_set(9, 73, 1); }

/* Bis zur Tuer-Anforderung fahren; Spieler-HP-Minimum, Lage des Spielers im ersten Bild auf Cut 12. */
typedef struct { int bilder, raumwechsel, hp_min, cut12_bild; int32_t cut12_px, cut12_pz; unsigned ziel; } lauf_t;
static void lauf(lauf_t *L, int max)
{
    memset(L, 0, sizeof *L); L->hp_min = 1000; L->cut12_bild = -1;
    int cam_alt = -1;
    for (int f = 0; f < max; f++) {
        frame();
        L->bilder = f + 1;
        const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        if (g_mess && (int)g_scd.cam_id != cam_alt) {
            printf("    [mess] Bild %d: Kamera Cut %d (angezeigt %d), Spieler (%d,%d) pm=%d auto=%d\n", f + 1, (int)g_scd.cam_id, s_shown,
                   (int)pl->x, (int)pl->z, (int)g_scd.player_mode, (int)g_scd.cut_auto_enabled);
            cam_alt = (int)g_scd.cam_id;
        }
        if (pl->hp < L->hp_min) L->hp_min = pl->hp;
        if (L->cut12_bild < 0 && g_scd.cam_id == 12) { L->cut12_bild = f + 1; }
        if (L->cut12_bild >= 0 && f + 1 == L->cut12_bild + 1) { L->cut12_px = pl->x; L->cut12_pz = pl->z; }
        if (g_room_change.pending) { L->raumwechsel = 1; L->ziel = g_room_change.room_id; break; }
    }
}

static int bit_in(int bit, const int *liste, int n) { for (int i = 0; i < n; i++) if (liste[i] == bit) return 1; return 0; }

/* Je Zombie eines Schnitts (Bits in `bits`, oder bits == NULL: alle, die beim Schnittanfang VOR der Kamera standen):
 * Anfang/Ende ausgeben; zaehlt die, die mindestens R38_NAEHER_MIN naeher kamen, und die Einzelstopps vor der Kamera. */
typedef struct { int n, naeher, stopp_vorn, min_naeher; } schnitt_t;
static schnitt_t schnitt(uint16_t raum, uint8_t cut, const int *bits, int nbits, const char *was)
{
    schnitt_t r = { 0, 0, 0, 1 << 30 };
    const re15_r38_spur_t *sp = NULL; int n = re15_r38_spuren(&sp);
    printf("  [%s] ROOM%04X Cut %d, %d Bilder:\n", was, raum, cut, re15_r38_abschnitt_bilder(raum, cut));
    for (int i = 0; i < n; i++) {
        const re15_r38_spur_t *p = &sp[i];
        if (p->raum != raum || p->cut != cut) continue;
        if (bits ? !bit_in(p->bit, bits, nbits) : !(p->t0 > 0 && p->bild0 == 1)) continue;
        r.n++;
        int naeher = (int)(p->d0 - p->d1);
        if (naeher >= R38_NAEHER_MIN) r.naeher++;
        if (naeher < r.min_naeher) r.min_naeher = naeher;
        if (p->angehalten && p->t1 > 0) r.stopp_vorn++;
        printf("    Aktor %2d Typ %02x Bit %3d: Abstand zur Kamera %5d -> %5d (%+6d), Tiefe %6d -> %6d, Bild %d..%d, angehalten %d\n",
               p->aktor, p->typ, p->bit, (int)p->d0, (int)p->d1, -naeher, (int)p->t0, (int)p->t1, p->bild0, p->bild1, p->angehalten);
    }
    return r;
}

/* ---- Teil: kameras ------------------------------------------------------------------------------------------------ */
static void teil_kameras(void)
{
    printf("== kameras ==\n");
    const re15_r38_kamera_t *k = NULL; int n = re15_r38_kameras(&k);
    PRUEF(n == 4, "vier Montage-Schnitte in der Tabelle (1130/0, 1040/1, 1030/7, 1030/12): %d", n);
    for (int i = 0; i < n; i++) {
        if (rdt_laden(k[i].raum) != 0) { PRUEF(0, "ROOM%04X ladbar", k[i].raum); continue; }
        uint32_t cs = (uint32_t)s32(s_raw + 0x24);
        const uint8_t *r = s_raw + k[i].rid_off;
        PRUEF(cs + 32u * k[i].cut == k[i].rid_off && s32(r + 4) == k[i].kx && s32(r + 0x0C) == k[i].kz &&
              s32(r + 0x10) == k[i].zx && s32(r + 0x18) == k[i].zz,
              "ROOM%04X Cut %d = RID @0x%03X (camera_start 0x%X): Kamera (%d,%d) Ziel (%d,%d)", k[i].raum, k[i].cut,
              k[i].rid_off, cs, (int)s32(r + 4), (int)s32(r + 0x0C), (int)s32(r + 0x10), (int)s32(r + 0x18));
    }
    /* Parkplatz Cut 12 aus den RID-Bytes: Abstand der Rolltor-Form (Parkplatz 1040 hinter Kamera Cut 1) auf der Achse Cut 12 */
    if (rdt_laden(0x1040) == 0) {
        const uint8_t *r = s_raw + 0x80;
        double kx = s32(r + 4), kz = s32(r + 0x0C), fx = s32(r + 0x10) - kx, fz = s32(r + 0x18) - kz, fn = sqrt(fx * fx + fz * fz);
        double tiefe = ((RE15_IT_PARK_1040_X - kx) * fx + (RE15_IT_PARK_1040_Z - kz) * fz) / fn;
        if (rdt_laden(0x1030) == 0) {
            const uint8_t *q = s_raw + 0x1E0;
            double cx = s32(q + 4), cz = s32(q + 0x0C), gx = s32(q + 0x10) - cx, gz = s32(q + 0x18) - cz, gn = sqrt(gx * gx + gz * gz);
            double px = cx + tiefe * gx / gn, pz = cz + tiefe * gz / gn;
            int yaw = (int)lround(atan2(-gz / gn, gx / gn) * 4096.0 / (2.0 * 3.14159265358979));
            if (yaw < 0) yaw += 4096;
            printf("  Rolltor-Form: Parkplatz 1040 %.1f vor der Kamera Cut 1 -> Parkplatz Cut 12 (%.1f,%.1f), Gierung %d\n", tiefe, px, pz, yaw);
            PRUEF(tiefe < -3900 && tiefe > -4100, "Parkplatz ROOM1040 liegt %.1f hinter der Kamera Cut 1 (Rolltor-Form)", -tiefe);
            PRUEF(fabs(px - RE15_R38_PARK_1030_12_X) <= 1.0 && fabs(pz - RE15_R38_PARK_1030_12_Z) <= 1.0,
                  "RE15_R38_PARK_1030_12 = (%d,%d) = Kameraachse Cut 12 im Rolltor-Abstand (%.1f,%.1f)",
                  RE15_R38_PARK_1030_12_X, RE15_R38_PARK_1030_12_Z, px, pz);
            PRUEF(abs(yaw - RE15_R38_PARK_1030_12_YAW) <= 1, "Gierung %d = Blick entlang der Achse Cut 12 (%d)", RE15_R38_PARK_1030_12_YAW, yaw);
            /* Parkplatz ROOM1030 (Tuer aus 1040, Cut 7): auf der Achse von Cut 7 (RID @0x140), in der Mitte des Teils der
             * BLK-Zone 7 (@0x01C10), der hinter der Wand SCA 5 (@0x01A4C) liegt (re15_irons_tod.h). */
            const uint8_t *c7 = s_raw + 0x140;
            double ax = s32(c7 + 4), az = s32(c7 + 0x0C), hx = s32(c7 + 0x10) - ax, hz = s32(c7 + 0x18) - az, hn = sqrt(hx * hx + hz * hz);
            const uint32_t blk = (uint32_t)s32(s_raw + 0x38);
            const uint8_t *z7 = s_raw + blk + 4 + 7 * 12;
            const re15_sca_entry_t *w5 = (s_rdt.sca_count > 5) ? &s_rdt.sca[5] : NULL;
            PRUEF(blk + 4 + 7 * 12 == 0x01C10 && s16(z7) == -29340 && w5 && (const uint8_t *)w5 - s_raw == 0x01A4C && w5->x == -27776,
                  "Quellen: BLK-Knoten 7 @0x%05X x1 %d, SCA 5 @0x%05X Rueckseite x %d", (unsigned)(blk + 4 + 7 * 12), (int)s16(z7),
                  w5 ? (unsigned)((const uint8_t *)w5 - s_raw) : 0u, w5 ? (int)w5->x : 0);
            double qx = w5 ? (w5->x + s16(z7)) / 2.0 : 0.0, qz = az + (qx - ax) * hz / hx;
            double q_tiefe = ((qx - ax) * hx + (qz - az) * hz) / hn;
            printf("  Parkplatz Cut 7: Mitte Zone 7 hinter SCA 5 auf der Achse = (%.1f,%.1f), %.1f hinter der Kamera\n", qx, qz, -q_tiefe);
            PRUEF(fabs(qx - RE15_IT_PARK_1030_X) <= 1.0 && fabs(qz - RE15_IT_PARK_1030_Z) <= 1.0 && q_tiefe < 0,
                  "RE15_IT_PARK_1030 = (%d,%d) = (%.1f,%.1f) hinter der Kamera Cut 7", RE15_IT_PARK_1030_X, RE15_IT_PARK_1030_Z, qx, qz);
            {   /* Navigator: gleiche Zone wie die Kopien = Ziel ist die Spielerlage (FUN_80039e7c `bne a0,a1` @0x80039fbc) */
                const uint8_t zp = re15_nav_zone_from_pos((int16_t)RE15_IT_PARK_1030_X, (int16_t)RE15_IT_PARK_1030_Z);
                const uint8_t z_alt = re15_nav_zone_from_pos(-28244, -3650), z_r38a = re15_nav_zone_from_pos(-30708, -1699);
                static const int16_t kop[5][2] = { { -18600, -3500 }, { -20600, -3000 }, { -22400, -4000 }, { -19600, -2400 }, { -21600, -4600 } };
                int in7 = 0;
                for (int i = 0; i < 5; i++) in7 += (re15_nav_zone_from_pos(kop[i][0], kop[i][1]) == 7);
                printf("  BLK-Zonen: Parkplatz %d, v0.8.25-Platz (-28244,-3650) %d, erster Runde-38-Platz (-30708,-1699) %d (255 = keine),"
                       " Kopien in Zone 7: %d von 5\n", zp, z_alt, z_r38a, in7);
                PRUEF(zp == 7 && in7 >= 4 && z_r38a == 0xff, "Parkplatz in BLK-Zone 7 wie %d von 5 Kopien (der erste Runde-38-Platz lag in "
                      "keiner Zone = veraltetes Navigator-Ziel)", in7);
            }
            {   /* Cut 12: der umgeparkte Spieler liegt in BLK-Zone 2 = dort, wo die Kriecher nach dem Tor hinkommen */
                const uint8_t z12 = re15_nav_zone_from_pos((int16_t)RE15_R38_PARK_1030_12_X, (int16_t)RE15_R38_PARK_1030_12_Z);
                PRUEF(z12 == 2, "Parkplatz Cut 12 (%d,%d) in BLK-Zone 2 (@0x01BD4, Hauptraum vor dem Tor): %d", RE15_R38_PARK_1030_12_X,
                      RE15_R38_PARK_1030_12_Z, z12);
            }
            {   /* keine SCA-Zelle am Parkplatz (sonst schiebt die Kollision den Spieler aus, wie (-27300,-3650) aus SCA 5):
                 * jede Zelle um den Spielerradius 450 vergroessert (re15_collision.c PR = DAT_80073e94[6]) */
                int drin = 0;
                for (int i = 0; i < s_rdt.sca_count; i++) {
                    const re15_sca_entry_t *e = &s_rdt.sca[i];
                    if (RE15_IT_PARK_1030_X >= e->x - 450 && RE15_IT_PARK_1030_X <= e->x + (int)e->width + 450 &&
                        RE15_IT_PARK_1030_Z >= e->z - 450 && RE15_IT_PARK_1030_Z <= e->z + (int)e->density + 450) drin++;
                }
                PRUEF(drin == 0, "Parkplatz ROOM1030 ausserhalb jeder SCA-Zelle (+ Spielerradius 450): %d Zellen", drin);
            }
        }
    }
}

/* ---- Montage-Schritte --------------------------------------------------------------------------------------------- */
static const int k_bits_1130[5] = { 40, 41, 42, 43, 46 };
static const int k_bits_1070[5] = { 47, 48, 49, 75, 76 };

static void schritt_1130(int an, lauf_t *L)
{
    re15_r38_montage_enable(an);
    szene_flags(); re15_game_flag_set(9, 74, 1);
    re15_irons_tod_zustand_setzen(RE15_IT_SZENE);
    re15_r38_montage_reset();
    if (room_boot(0x1130, RE15_IT_PARK_1130_X, RE15_IT_PARK_1130_Z, 0, 0) != 0) return;
    PRUEF(re15_irons_tod_zustand() == RE15_IT_S1130, "1130: Montage-Schritt laeuft (Zustand %d)", re15_irons_tod_zustand());
    lauf(L, 600);
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
    re15_r38_montage_enable(1);
}

static void teil_s1130(re15_ai_flavor_t fl)
{
    printf("== s1130 (KI %s) ==\n", fl == RE15_AI_FLAVOR_RE2 ? "RE2" : "RE1.5");
    re15_ai_flavor_set(fl);
    lauf_t L;
    schritt_1130(1, &L);
    schnitt_t s = schnitt(0x1130, 0, k_bits_1130, 5, "jetzt");
    PRUEF(L.raumwechsel && L.ziel == 0x1040, "Schnitt nach ROOM1040 nach %d Bildern (Montage laeuft weiter)", L.bilder);
    PRUEF(s.n == 5 && s.naeher == 5, "ROOM1130 Cut 0: alle fuenf kommen der Kamera mindestens %d naeher (%d von %d, kleinste Annaeherung %d)",
          R38_NAEHER_MIN, s.naeher, s.n, s.min_naeher);
    PRUEF(s.stopp_vorn == 0, "Einzelstopp nur hinter der Kamera (vor der Kamera angehalten: %d)", s.stopp_vorn);
    PRUEF(L.hp_min == 100, "Spieler-HP waehrend des Schritts unveraendert (min %d)", L.hp_min);
    if (g_mess) return;
    schritt_1130(0, &L);
    schnitt_t g = schnitt(0x1130, 0, k_bits_1130, 5, "Gegenprobe v0.8.25");
    PRUEF(g.n == 5 && g.naeher == 0, "Gegenprobe Stand v0.8.25 (stehende Form grid 0x02): keiner kommt naeher (%d von %d, groesste Annaeherung < %d)",
          g.naeher, g.n, R38_NAEHER_MIN);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
}

static void schritt_1030(int an, lauf_t *L)
{
    re15_r38_montage_enable(an);
    szene_flags(); re15_game_flag_set(9, 76, 1);
    re15_irons_tod_zustand_setzen(RE15_IT_S1040);
    re15_r38_montage_reset();
    /* Gegenprobe Stand v0.8.25: auch der alte Tuer-Parkplatz (-27300,-3650) (re15_irons_tod.h, Runde 38 Spur O) */
    if (room_boot(0x1030, an ? RE15_IT_PARK_1030_X : -27300, an ? RE15_IT_PARK_1030_Z : -3650, 0, 7) != 0) return;
    PRUEF(re15_irons_tod_zustand() == RE15_IT_S1030, "1030: Montage-Schritt laeuft (Zustand %d)", re15_irons_tod_zustand());
    lauf(L, 900);
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
    re15_r38_montage_enable(1);
}

static void teil_s1030(re15_ai_flavor_t fl)
{
    printf("== s1030 (KI %s) ==\n", fl == RE15_AI_FLAVOR_RE2 ? "RE2" : "RE1.5");
    re15_ai_flavor_set(fl);
    lauf_t L;
    schritt_1030(1, &L);
    schnitt_t s7 = schnitt(0x1030, 7, k_bits_1070, 5, "jetzt");
    schnitt_t s12 = schnitt(0x1030, 12, NULL, 0, "jetzt");
    PRUEF(L.raumwechsel && L.ziel == 0x11C0, "Schnitt nach ROOM11C0 nach %d Bildern", L.bilder);
    PRUEF(s7.n == 5 && s7.naeher == 5, "ROOM1030 Cut 7: alle fuenf Kopien kommen der Kamera mindestens %d naeher (%d von %d, kleinste %d)",
          R38_NAEHER_MIN, s7.naeher, s7.n, s7.min_naeher);
    PRUEF(s12.n >= 3 && s12.naeher == s12.n, "ROOM1030 Cut 12: alle Zombies vor der Kamera (am Tor) kommen ihr mindestens %d naeher (%d von %d, kleinste %d)",
          R38_NAEHER_MIN, s12.naeher, s12.n, s12.min_naeher);
    PRUEF(s7.stopp_vorn == 0 && s12.stopp_vorn == 0, "Einzelstopp nur hinter der Kamera (vorn angehalten: Cut 7 %d, Cut 12 %d)", s7.stopp_vorn, s12.stopp_vorn);
    {   /* die Kopien stehen beim Schnitt auf Cut 12 hinter dessen Kamera und werden sofort angehalten (kein Weg zum Spieler) */
        const re15_r38_spur_t *sp = NULL; int n = re15_r38_spuren(&sp), kopien = 0, sofort = 0;
        for (int i = 0; i < n; i++) if (sp[i].raum == 0x1030 && sp[i].cut == 12 && bit_in(sp[i].bit, k_bits_1070, 5)) {
            kopien++; if ((sp[i].angehalten >= 1 && sp[i].angehalten <= 2) || sp[i].halt0) sofort++; }
        PRUEF(kopien == 5 && sofort == 5, "Cut 12: die fuenf Kopien (hinter der Kamera Cut 12) sind spaetestens im ersten Bild angehalten (%d von %d)", sofort, kopien);
    }
    PRUEF(L.cut12_bild > 0 && labs((long)L.cut12_px - RE15_R38_PARK_1030_12_X) <= 600 && labs((long)L.cut12_pz - RE15_R38_PARK_1030_12_Z) <= 600,
          "nach dem Schnitt auf Cut 12 (Bild %d) steht der geparkte Spieler auf der Kameraachse: (%d,%d), Parkplatz (%d,%d)",
          L.cut12_bild, (int)L.cut12_px, (int)L.cut12_pz, RE15_R38_PARK_1030_12_X, RE15_R38_PARK_1030_12_Z);
    PRUEF(L.hp_min == 100, "Spieler-HP waehrend des Schritts unveraendert (min %d)", L.hp_min);
    if (g_mess) return;
    schritt_1030(0, &L);
    schnitt_t g7 = schnitt(0x1030, 7, k_bits_1070, 5, "Gegenprobe v0.8.25");
    schnitt_t g12 = schnitt(0x1030, 12, NULL, 0, "Gegenprobe v0.8.25");
    PRUEF(g7.n == 5 && g7.naeher == 0, "Gegenprobe Stand v0.8.25 (grid 0x00): auf Cut 7 kommt keine Kopie naeher (%d von %d)", g7.naeher, g7.n);
    PRUEF(g12.n >= 3 && g12.naeher < g12.n, "Gegenprobe Stand v0.8.25: auf Cut 12 kommen nicht alle naeher (%d von %d)", g12.naeher, g12.n);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
}

/* ---- Teil: rolltor ------------------------------------------------------------------------------------------------ */
#define R38_ROLLTOR_BILDER 2000
static int32_t s_bahn[2][R38_ROLLTOR_BILDER][RE15_ACTOR_MAX][2];
static int     s_bahn_n[2];
static void schritt_1040(int an, int idx)
{
    re15_r38_montage_enable(an);
    szene_flags(); re15_game_flag_set(9, 76, 1); re15_game_flag_set(9, 75, 1);
    { static const int a[5] = { 83,84,90,91,95 }; for (int i = 0; i < 5; i++) re15_game_flag_set(7, (uint8_t)a[i], 1); }
    re15_irons_tod_zustand_setzen(RE15_IT_S1130);
    re15_irons_tod_bilanz_reset();
    re15_r38_montage_reset();
    s_bahn_n[idx] = 0;
    if (room_boot(0x1040, RE15_IT_PARK_1040_X, RE15_IT_PARK_1040_Z, 1024, 1) != 0) return;
    for (int f = 0; f < R38_ROLLTOR_BILDER; f++) {
        frame();
        for (int s = 0; s < RE15_ACTOR_MAX; s++) { s_bahn[idx][f][s][0] = g_actors[s].x; s_bahn[idx][f][s][1] = g_actors[s].z; }
        s_bahn_n[idx] = f + 1;
        if (g_room_change.pending) break;
    }
    re15_r38_montage_enable(1);
}
static void teil_rolltor(void)
{
    printf("== rolltor ==\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    schritt_1040(1, 0);
    int n = 0, letzt = -1, durch = re15_irons_tod_bilanz_1040(&n, &letzt);
    int hp0 = 0, hpmin = 0; re15_irons_tod_hp(&hp0, &hpmin);
    schnitt(0x1040, 1, NULL, 0, "Rolltor jetzt");
    schritt_1040(0, 1);
    int gleich = (s_bahn_n[0] == s_bahn_n[1]), erste_abw = -1;
    for (int f = 0; gleich && f < s_bahn_n[0]; f++)
        for (int s = 0; s < RE15_ACTOR_MAX; s++)
            if (s_bahn[0][f][s][0] != s_bahn[1][f][s][0] || s_bahn[0][f][s][1] != s_bahn[1][f][s][1]) { gleich = 0; erste_abw = f; break; }
    PRUEF(gleich, "ROOM1040 Cut 1 (Rolltor): mit und ohne Runde-38-Aenderung Bild fuer Bild dieselben Lagen aller Aktoren "
          "(%d / %d Bilder, erste Abweichung %d)", s_bahn_n[0], s_bahn_n[1], erste_abw);
    PRUEF(n == 5 && durch == 5 && !re15_irons_tod_1040_kappe(), "Rolltor: 5 Zombies, alle durchs Tor (%d von %d, letzter Bild %d), keine Kappe",
          durch, n, letzt);
    PRUEF(hpmin == 100 || hp0 < 0, "Rolltor: Spieler-HP unveraendert (%d -> min %d)", hp0, hpmin);
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (argc > 2 && !strcmp(argv[2], "mess")) g_mess = 1;
    int alle = !strcmp(teil, "alle");
    if (alle || !strcmp(teil, "kameras"))   teil_kameras();
    if (alle || !strcmp(teil, "s1130_re2"))  teil_s1130(RE15_AI_FLAVOR_RE2);
    if (alle || !strcmp(teil, "s1130_re15")) teil_s1130(RE15_AI_FLAVOR_RE15);
    if (alle || !strcmp(teil, "s1030_re2"))  teil_s1030(RE15_AI_FLAVOR_RE2);
    if (alle || !strcmp(teil, "s1030_re15")) teil_s1030(RE15_AI_FLAVOR_RE15);
    if (alle || !strcmp(teil, "rolltor"))    teil_rolltor();
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
