/*
 * test_r42_klein.c — Runde 42 "story" (v0.8.31), Stufe Kleinkram: T-08 (Ada im Schutzraum 6020 nur lebend) und T-09 (Beschreibung
 * der Blue Master Keycard). Belege include/re15_story_klein_r42.h. PORT-STORY nach dem Konzept (5 T-08 [B22], T-09 [B39]).
 * Harness (Raumaufbau ueber scd_room_reenter mit allen Installern, SCD-VM) = test_r42_story3b.c.
 *
 * Teile (Aufruf: test_r42_klein <teil>):
 *   belege   Original-Bytes: ROOM6020 sub00 @0x00B92/@0x00B96/@0x00BAA, ROOM50B0 sub02 @0x005A6, ROOM4010 Slot 9 @0x00592,
 *            Beschreibung 0x47 @0x800c5e53 (DEBUG.BIN) "Goro"
 *   ada      ROOM6020 als Leon mit (3,76): (3,0x62)=0 -> Marvin (0x40) und Ada (0x42) da; (3,0x62)=1 -> nur Marvin
 *   elza     ROOM6021 als Elza mit (3,76) und (3,0x62)=1: beide Saetze (0x49/0x4B) wie im Original
 *   karte    re15_inv_desc_entry(0x47) = Neutext ("Michael", 3 Zeilen, 1 Seite, Ende 01 00); 0x46/0x45 unveraendert
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
#include "re15_fade.h"
#include "re15_item_discard.h"
#include "re15_gameflow.h"
#include "re15_light.h"
#include "re15_story_klein_r42.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

const uint8_t *re15_inv_desc_entry(int id);

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

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
static int bytes_gleich(const char *rel, uint32_t off, const uint8_t *soll, size_t n)
{
    char p[600]; size_t sz = 0;
    snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, rel);
    uint8_t *d = slurp(p, &sz);
    int ok = d && off + n <= sz && memcmp(d + off, soll, n) == 0;
    free(d);
    return ok;
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
    g_current_room_id = room; g_room_change.pending = 0;
    g_room_rdt_ok = 1;
    g_gameflow.character = (room & 0xFu) ? 4 : 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    return 0;
}
static int typ_da(uint8_t typ)
{
    for (int i = 1; i < RE15_ACTOR_MAX; i++) if (g_actors[i].active && g_actors[i].type == typ) return 1;
    return 0;
}

/* ---- belege ------------------------------------------------------------------------------------------------------------------ */
static void teil_belege(void)
{
    printf("== belege ==\n");
    static const uint8_t ck76[4]   = { 0x21, 0x03, 0x4C, 0x01 };
    static const uint8_t marvin[4] = { 0x44, 0x00, 0x40, 0x40 };
    static const uint8_t ada[4]    = { 0x44, 0x01, 0x42, 0x40 };
    static const uint8_t tod[4]    = { 0x22, 0x03, 0x62, 0x01 };
    static const uint8_t ck62[4]   = { 0x21, 0x03, 0x62, 0x00 };
    static const uint8_t blue[16]  = { 0x50, 0x09, 0x09, 0x31, 0x00, 0x00, 0x50, 0x97, 0x20, 0xB8, 0x40, 0x06, 0xD0, 0x07, 0x47, 0x00 };
    PRUEF(bytes_gleich("STAGE6/ROOM6020.RDT", 0x00B92, ck76, 4), "ROOM6020 sub00 @0x00B92 Ck(3,76)=1");
    PRUEF(bytes_gleich("STAGE6/ROOM6020.RDT", 0x00B96, marvin, 4), "ROOM6020 sub00 @0x00B96 Sce_em_set Slot 0 Typ 0x40 (Marvin)");
    PRUEF(bytes_gleich("STAGE6/ROOM6020.RDT", 0x00BAA, ada, 4), "ROOM6020 sub00 @0x00BAA Sce_em_set Slot 1 Typ 0x42 (Ada), ohne Ck(3,0x62)");
    PRUEF(bytes_gleich("STAGE5/ROOM50B0.RDT", 0x005A6, tod, 4), "ROOM50B0 sub02 @0x005A6 Set(3,0x62) (Adas Tod)");
    PRUEF(bytes_gleich("STAGE5/ROOM50B0.RDT", 0x004E6, ck62, 4), "ROOM50B0 sub00 @0x004E6 Ck(3,0x62)=0 (Ada-Szene noch nicht gelaufen)");
    PRUEF(bytes_gleich("STAGE4/ROOM4010.RDT", 0x00592, blue, 16), "ROOM4010 Item_aot_set Slot 9 @0x00592 Item 0x47 (Blue Master Keycard)");
    /* Original-Beschreibung 0x47 @0x800c5e53 = DEBUG.BIN @0x5e53: "It seems ..." ... "Goro" (0x23 0x4b 0x4e 0x4b) */
    static const uint8_t goro[5] = { 0x61, 0x23, 0x4B, 0x4E, 0x4B };
    PRUEF(bytes_gleich("BIN/DEBUG.BIN", 0x5E53 + 41, goro, 5), "Original-Beschreibung 0x47 @0x800c5e53 nennt \"Goro\"");
}

/* ---- ada ----------------------------------------------------------------------------------------------------------------------- */
static void schutzraum(int tot, int *marvin, int *ada)
{
    re15_game_state_init();
    g_gameflow.character = 0;
    re15_game_flag_set(3, 76, 1);                                   /* (3,76): ROOM4010 @0x00DAC / ROOM5011 @0x0094C */
    re15_game_flag_set(RE15_SKL_TOD_BANK, RE15_SKL_TOD_BIT, tot);   /* ROOM50B0 sub02 @0x005A6 */
    memset(g_re15_skl_zaehler, 0, sizeof g_re15_skl_zaehler);
    *marvin = *ada = -1;
    if (room_boot(0x6020, -4500, 2300, 3072, 0) != 0) return;     /* Ankunft aus 5130 (re15_room_spawns.h ROOM6020) */
    for (int f = 0; f < 30; f++) scd_vm_tick();
    *marvin = typ_da(0x40);
    *ada = typ_da(0x42);
}
static void teil_ada(void)
{
    printf("== ada ==\n");
    int m, a;
    schutzraum(0, &m, &a);
    PRUEF(m == 1 && a == 1 && g_re15_skl_zaehler[1] == 1 && g_re15_skl_zaehler[0] == 0,
          "6020, Ada lebt ((3,0x62)=0): Marvin %d, Ada %d (zugelassen %u, verworfen %u)", m, a, g_re15_skl_zaehler[1], g_re15_skl_zaehler[0]);
    schutzraum(1, &m, &a);
    PRUEF(m == 1 && a == 0 && g_re15_skl_zaehler[0] == 1,
          "6020, Ada tot in 50B0 ((3,0x62)=1): Marvin %d, Ada %d (verworfen %u)", m, a, g_re15_skl_zaehler[0]);
    PRUEF(re15_story_klein_r42_spawn_weg(0x6020, 0x40) == 0 && re15_story_klein_r42_spawn_weg(0x50B0, 0x42) == 0,
          "nur 6020 und nur Typ 0x42 (Marvin bleibt, Ada in 50B0 unberuehrt)");
}
static void teil_elza(void)
{
    printf("== elza ==\n");
    re15_game_state_init();
    g_gameflow.character = 4;
    re15_game_flag_set(3, 76, 1);
    re15_game_flag_set(RE15_SKL_TOD_BANK, RE15_SKL_TOD_BIT, 1);
    if (room_boot(0x6021, -4500, 2300, 3072, 0) != 0) return;
    for (int f = 0; f < 30; f++) scd_vm_tick();
    PRUEF(typ_da(0x49) && typ_da(0x4B), "6021 (Elza): beide Original-Saetze 0x49/0x4B (sub00 @0x00B96/@0x00BAA) unveraendert");
}

/* ---- karte ---------------------------------------------------------------------------------------------------------------------- */
static int dekodieren(const uint8_t *b, char *out, size_t n, int *zeilen, int *seiten)
{
    size_t k = 0; int i = 0;
    *zeilen = 1; *seiten = 1;
    for (; i < 400; i++) {
        const uint8_t c = b[i];
        char ch = 0;
        if (c == 0x01) { out[k] = 0; return b[i + 1] == 0x00; }
        if (c == 0x02) { (*seiten)++; *zeilen = 1; i++; ch = '|'; }
        else if (c == 0x08) { (*zeilen)++; ch = ' '; }
        else if (c == 0x00) ch = ' ';
        else if (c >= 0x1D && c <= 0x36) ch = (char)('A' + c - 0x1D);
        else if (c >= 0x3D && c <= 0x56) ch = (char)('a' + c - 0x3D);
        else if (c == 0x57) ch = '.';
        else if (c == 0x61 || c == 0x19) ch = '"';
        else ch = '?';
        if (k + 1 < n) out[k++] = ch;
    }
    out[k] = 0;
    return 0;
}
static void teil_karte(void)
{
    printf("== karte ==\n");
    char t[256]; int z = 0, s = 0;
    const uint8_t *b = re15_inv_desc_entry(0x47);
    const int ende = b && dekodieren(b, t, sizeof t, &z, &s);
    PRUEF(ende && !strcmp(t, "An ID card for the research center. It belongs to someone named \"Michael.\"") && z == 3 && s == 1,
          "0x47 Blue Master Keycard: \"%s\" (%d Zeilen, %d Seite, Ende 01 00 wie Original @0x800c5e53)", ende ? t : "?", z, s);
    const uint8_t *r = re15_inv_desc_entry(0x46);
    PRUEF(r && dekodieren(r, t, sizeof t, &z, &s) && !strcmp(t, "It looks like a card key for lab access."),
          "0x46 Red Master Keycard unveraendert: \"%s\"", t);
    PRUEF(re15_story_klein_r42_desc(0x47) != NULL && re15_story_klein_r42_desc(0x46) == NULL && re15_story_klein_r42_desc(0x3F) == NULL,
          "Haken nur fuer 0x47 (Figur egal: Leon und Elza)");
}

int main(int argc, char **argv)
{
    const char *t = argc > 1 ? argv[1] : "";
    if (!strcmp(t, "belege")) teil_belege();
    else if (!strcmp(t, "ada")) teil_ada();
    else if (!strcmp(t, "elza")) teil_elza();
    else if (!strcmp(t, "karte")) teil_karte();
    else { printf("unbekannter Teil '%s'\n", t); return 2; }
    printf("%s: %d Fehler\n", t, g_fail);
    return g_fail ? 1 : 0;
}
