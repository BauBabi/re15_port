/* test_r37_roy.c — RIEGEL Spur K (Runde 37) "roy": Elzas Roy (Typ 0x47) mit Modell + Animationen des Originals.
 *
 * Dossier: analysis/befunde_runde37/K_roy.md. Belege: include/re15_roy_r37.h, engine/src/roy_r37.c,
 * platform/pc/src/roy_pc.c. Verfahren wie test_r36_elza.c: ausgelieferte RDT laden, Raum ueber scd_room_reenter
 * aufbauen, Bilder in der Reihenfolge von platform/pc/main.c fahren und die MECHANIK messen.
 *
 * Teile (je ein ctest-Eintrag):
 *   toc       FUN_80022300: TOC @0x80072f38 (Leon) / @0x80073178 (Elza), Typ 0x47 -> CDEMD0 1644/127640 (= ELLIOT.*,
 *             bytegleich) bzw. CDEMD1 1639/142272 (Roy: Loco-Clip 7 = 113 Bilder, Clip 23 = 30, eigene Bank Clip 5 = 61)
 *   weiche    Elza-Bit (bltz @0x80022334): Leons 0x47 = Elliot-Sonderweg, Elzas 0x47 = Elza-Archiv, andere Typen nie
 *   szene1011 ROOM1011 (Elza): Roy bindet RBJ-Record 1 (Marker 0x2, @0x8001b498), Ruhe-Clip 5 = 28 Bilder (nicht
 *             Elzas 20), Ohnmacht @0x00D54 Clip 9 = 100 Bilder, haelt danach Bild 99 (keine Schleife)
 *   szene1021 ROOM1021 (Elza): Roy (Gegner 1) ohne Record; Gang Plc_dest @0x020B2 eigene Bank Clip 5 = 61 Bilder;
 *             Sturz @0x020DE Loco-Clip 7 = 113 Bilder, haelt danach Bild 112
 *   leon1170  Gegenprobe ROOM1170 (Leon): Elliot behaelt die Ausnahme im Marker-Binder (kein Record-Kanal) */
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
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_tim.h"
#include "re15_gameflow.h"
#include "re15_roy_r37.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void re15_actor_step_all_walkers(void);
const re15_emd_animation_t *re15_actor_rbj_anim(int slot);

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
static int                s_bild = 0;

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

/* Blob `type` aus dem Archiv `archiv` nach TOC (re15_ems_toc — dieselbe Funktion wie der PC-Lader roy_pc.c). */
static uint8_t s_blob[2][0x40000];
static size_t blob_lesen(int archiv, uint8_t type, int buf)
{
    uint32_t sek = 0, len = 0;
    if (!re15_ems_toc(archiv, type, &sek, &len) || len > sizeof s_blob[buf]) return 0;
    char p[600]; snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, re15_ems_archiv_datei(archiv));
    FILE *f = fopen(p, "rb");
    if (!f) return 0;
    size_t n = 0;
    if (fseek(f, (long)sek * 2048L, SEEK_SET) == 0) n = fread(s_blob[buf], 1, len, f);
    fclose(f);
    return n == len ? len : 0;
}

/* Registry-Bank wie pc_enemy_load: Roy (Elzas 0x47) aus CDEMD1, sonst der Port-Standard CDEMD0 (TOC). */
static uint8_t s_bank_buf[3][0x40000];
static int bank_laden(uint8_t type, int buf)
{
    int archiv = re15_roy_aus_elza_archiv(type) ? RE15_EMS_ARCHIV_ELZA : RE15_EMS_ARCHIV_LEON;
    size_t len = blob_lesen(archiv, type, 0);
    if (!len) return -1;
    memcpy(s_bank_buf[buf], s_blob[0], len);
    re15_enemy_bank_t *eb = re15_enemy_alloc(type);
    if (!eb) return -1;
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_bank_buf[buf], len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return -1;
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok   = (re15_emd_parse_loco_bank(s_bank_buf[buf], len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(s_bank_buf[buf], len, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->own_ok    = (re15_emd_parse_own_bank(s_bank_buf[buf], len, &eb->skel_own, &eb->anim_own) == 0);
    return archiv;
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
    s_bild++;
}

static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, uint8_t cut, const uint8_t *typen, int n)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    for (int i = 0; i < n; i++) bank_laden(typen[i], i);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = cut; s_cine_was = 0; s_bild = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

/* Bild, in dem der Opcode an der RDT-Datei-Stelle `off` lief (danach parkt ein Faden zwischen off+1 und off+16). */
static int bild_nach_stelle(unsigned off, int max)
{
    for (int f = 0; f < max; f++) {
        int b = s_bild;
        frame();
        for (int i = 2; i < SCD_THREAD_COUNT; i++) {
            const scd_thread_t *t = &g_scd.threads[i];
            if (!t->active || !t->pc) continue;
            unsigned o = (unsigned)(t->pc - s_rdt.raw);
            if (o > off && o <= off + 16u) return b;
        }
    }
    return -1;
}

static int slot_von_typ(uint8_t type)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == type) return s;
    return -1;
}

/* Clip `c` eines Clip-Satzes oder -1. */
static int fc(const re15_emd_animation_t *a, int c) { return (a && c < a->clip_count) ? a->clips[c].frame_count : -1; }

/* ---- toc ----------------------------------------------------------------------------------------------- */
static void teil_toc(void)
{
    printf("== toc ==\n");
    uint32_t s = 0, l = 0;
    PRUEF(re15_ems_toc(RE15_EMS_ARCHIV_LEON, 0x47, &s, &l) && s == 1644 && l == 127640,
          "Leon-TOC @0x80072f38[0x37]: Typ 0x47 Sektor %u, %u B (1644/127640)", (unsigned)s, (unsigned)l);
    PRUEF(re15_ems_toc(RE15_EMS_ARCHIV_ELZA, 0x47, &s, &l) && s == 1639 && l == 142272,
          "Elza-TOC @0x80073178[0x37]: Typ 0x47 Sektor %u, %u B (1639/142272)", (unsigned)s, (unsigned)l);
    PRUEF(!re15_ems_toc(RE15_EMS_ARCHIV_ELZA, 0x14, NULL, NULL) && !re15_ems_toc(RE15_EMS_ARCHIV_LEON, 0x50, NULL, NULL),
          "kein Eintrag fuer Typ 0x14 (Laenge 0) und ausserhalb 0x10..0x4F");
    PRUEF(!strcmp(re15_ems_archiv_datei(RE15_EMS_ARCHIV_LEON), "EMD/CDEMD0.EMS") &&
          !strcmp(re15_ems_archiv_datei(RE15_EMS_ARCHIV_ELZA), "EMD/CDEMD1.EMS"),
          "Datei 0x26 = EMD/CDEMD0.EMS, Datei 0x27 = EMD/CDEMD1.EMS");

    /* Roy-Blob: Verzeichnis + Clip-Laengen der drei Baenke, die er im Original spielt */
    size_t n = blob_lesen(RE15_EMS_ARCHIV_ELZA, 0x47, 0);
    PRUEF(n == 142272, "CDEMD1-Blob Typ 0x47 gelesen (%u B)", (unsigned)n);
    if (n) {
        static re15_md1_t md1; static re15_emd_skeleton_t sk, skl, sko; static re15_emd_animation_t an, anl, ano;
        re15_tim_t tim = (re15_tim_t){0};
        int ok = re15_emd_parse_container(s_blob[0], n, &md1, &sk, &an, &tim) == 0 &&
                 re15_emd_parse_loco_bank(s_blob[0], n, &skl, &anl) == 0 &&
                 re15_emd_parse_own_bank(s_blob[0], n, &sko, &ano) == 0;
        PRUEF(ok && md1.mesh_count == 15 && skl.bone_count == 15, "Roy: 15 Meshes, 15 Knochen");
        PRUEF(fc(&anl, 7) == 113 && fc(&anl, 8) == 2 && fc(&anl, 23) == 30,
              "Roy Loco-Bank dir[1]: Clip 7 = %d (Sturz, 113), Clip 8 = %d, Clip 23 = %d", fc(&anl, 7), fc(&anl, 8), fc(&anl, 23));
        PRUEF(fc(&ano, 5) == 61, "Roy eigene Bank dir[3]: Clip 5 = %d (Gang, 61)", fc(&ano, 5));
    }
    /* Elliot unveraendert: der Leon-Blob ist bytegleich PLD/ELLIOT.* (dir[7] MD1, dir[1] EDD, dir[2] EMR, dir[8] TIM) */
    n = blob_lesen(RE15_EMS_ARCHIV_LEON, 0x47, 1);
    PRUEF(n == 127640, "CDEMD0-Blob Typ 0x47 gelesen (%u B)", (unsigned)n);
    if (n) {
        const uint8_t *b = s_blob[1];
        uint32_t d0 = (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
        uint32_t dir[9];
        for (int k = 0; k < 9; k++) {
            const uint8_t *q = b + d0 + (size_t)k * 4;
            dir[k] = (uint32_t)q[0] | ((uint32_t)q[1] << 8) | ((uint32_t)q[2] << 16) | ((uint32_t)q[3] << 24);
        }
        static const struct { const char *datei; int von, bis; } teile[4] = {
            { "MD1", 7, 8 }, { "EDD", 1, 2 }, { "EMR", 2, 3 }, { "TIM", 8, -1 } };
        int gleich = 0;
        for (int k = 0; k < 4; k++) {
            char p[600]; snprintf(p, sizeof p, "%s/PLD/ELLIOT.%s", RE15_ASSET_PSX_DIR, teile[k].datei);
            size_t fn = 0; uint8_t *f = slurp(p, &fn);
            size_t lo = dir[teile[k].von], hi = teile[k].bis < 0 ? d0 : dir[teile[k].bis];   /* TIM endet am Verzeichnis */
            if (f && hi - lo == fn && !memcmp(b + lo, f, fn)) gleich++;
            else printf("  (ELLIOT.%s: %u B, Blob %u B)\n", teile[k].datei, (unsigned)fn, (unsigned)(hi - lo));
            free(f);
        }
        PRUEF(gleich == 4, "PLD/ELLIOT.MD1/.EDD/.EMR/.TIM == CDEMD0 Typ 0x47 dir[7]/dir[1]/dir[2]/dir[8] (%d/4)", gleich);
    }
}

/* ---- weiche -------------------------------------------------------------------------------------------- */
static void teil_weiche(void)
{
    printf("== weiche ==\n");
    g_gameflow.character = 0;                           /* Leon */
    PRUEF(re15_ems_archiv_aktiv() == RE15_EMS_ARCHIV_LEON && re15_typ47_ist_elliot(0x47) && !re15_roy_aus_elza_archiv(0x47),
          "Leon: Archiv CDEMD0, Typ 0x47 = Elliot-Sonderweg, kein Elza-Archiv");
    g_gameflow.character = 4;                           /* Elza (Bit 2 des Charakter-Bytes -> DAT_800aca3c Bit 31) */
    PRUEF(re15_ems_archiv_aktiv() == RE15_EMS_ARCHIV_ELZA && !re15_typ47_ist_elliot(0x47) && re15_roy_aus_elza_archiv(0x47),
          "Elza: Archiv CDEMD1, Typ 0x47 = Roy (allgemeiner NPC-Weg) aus dem Elza-Archiv");
    int fremd = 0;
    for (int t = 0x10; t < 0x50; t++) if (t != 0x47 && (re15_typ47_ist_elliot((uint8_t)t) || re15_roy_aus_elza_archiv((uint8_t)t))) fremd++;
    g_gameflow.character = 0;
    for (int t = 0x10; t < 0x50; t++) if (t != 0x47 && (re15_typ47_ist_elliot((uint8_t)t) || re15_roy_aus_elza_archiv((uint8_t)t))) fremd++;
    PRUEF(fremd == 0, "kein anderer Typ wird umgelenkt (%d)", fremd);
}

/* ---- szene1011 ------------------------------------------------------------------------------------------ */
static void teil_szene1011(void)
{
    printf("== szene1011 ==\n");
    re15_game_state_init();
    g_gameflow.character = 4;
    static const uint8_t typen[] = { 0x47 };
    if (room_boot(0x1011, 0, 5000, 0, 0, typen, 1) != 0) return;
    frame();
    int roy = slot_von_typ(0x47);
    PRUEF(roy == 1, "Roy = Aktor 1 (Sce_em_set Slot 0 @0x009F6)");
    if (roy < 0) return;
    const re15_emd_animation_t *rb = re15_actor_rbj_anim(roy);
    PRUEF(rb && rb->clip_count == 14, "RBJ-Kanal +0x180/+0x184 = Record 1 (Marker 0x2, 14 Clips; Elzas Record 0 hat 10): %d",
          rb ? rb->clip_count : -1);
    PRUEF(fc(rb, 5) == 28 && fc(rb, 6) == 78 && fc(rb, 7) == 30 && fc(rb, 9) == 100 && fc(rb, 10) == 60 && fc(rb, 13) == 2,
          "Record 1: Clip 5/6/7/9/10/13 = %d/%d/%d/%d/%d/%d (28/78/30/100/60/2)",
          fc(rb, 5), fc(rb, 6), fc(rb, 7), fc(rb, 9), fc(rb, 10), fc(rb, 13));
    for (int f = 0; f < 4; f++) frame();
    re15_actor_t *r = &g_actors[roy];
    PRUEF(r->motion == 5 && (r->anim_flags & 0x04) && re15_actor_clip_len(r) == 28,
          "Ruhe-Clip 5 (sub03 @0x00DE0 + Plc_flg 4) taktet mit %d Bildern (Original-RAM: Clip 5, 28 Bilder, +0x1c4 0x24)",
          re15_actor_clip_len(r));
    re15_enemy_bank_t *eb = re15_enemy_find(0x47);
    PRUEF(eb && eb->loco_ok && fc(&eb->anim_loco, 7) == 113, "Registry-Bank 0x47 = CDEMD1 (Loco-Clip 7 = %d)",
          eb ? fc(&eb->anim_loco, 7) : -1);

    int sackt = bild_nach_stelle(0x00D54u, 4000);           /* Plc_motion(0,9) @0x00D54 */
    PRUEF(sackt > 0 && r->motion == 9 && re15_actor_clip_len(r) == 100,
          "Ohnmacht @0x00D54 in Bild %d: Clip 9, %d Bilder (Record 1)", sackt, re15_actor_clip_len(r));
    int max = 0, halt = 0, wrap = 0, prev = -1;
    for (int f = 0; f < 260; f++) {
        frame();
        int af = (int)r->anim_frame;
        if (r->motion != 9) break;
        if (af > max) max = af;
        if (prev >= 0 && af < prev) wrap++;
        if (af == 99) halt++;
        prev = af;
    }
    PRUEF(max == 99 && wrap == 0 && halt >= 150,
          "Clip 9 laeuft bis Bild %d und haelt (%d Bilder auf 99, %d Ruecksprunge) — Original Phase 2, +0x168 -> Bild 99",
          max, halt, wrap);
}

/* ---- szene1021 ------------------------------------------------------------------------------------------ */
static void teil_szene1021(void)
{
    printf("== szene1021 ==\n");
    re15_game_state_init();
    g_gameflow.character = 4;
    re15_game_flag_set(4, 0x60, 1); re15_game_flag_set(3, 0xC9, 1);   /* ROOM1211 sub02 @0x01F82 / @0x01F86 */
    static const uint8_t typen[] = { 0x47, 0x49 };
    if (room_boot(0x1021, -13100, -9100, 0, 9, typen, 2) != 0) return;
    frame();
    int roy = slot_von_typ(0x47);
    PRUEF(roy == 2, "Roy = Aktor 2 (Sce_em_set Slot 1 @0x01E0A)");
    if (roy < 0) return;
    re15_actor_t *r = &g_actors[roy];
    PRUEF(re15_actor_rbj_anim(roy) == NULL, "kein RBJ-Record fuer Gegner 1 (Marker 0x1/0x2) — wie Original-RAM (+0x180 = 0)");
    PRUEF(r->motion == 23 && r->sub_state_1 == 1 && re15_actor_clip_len(r) == 30,
          "sub03 @0x021C2 Plc_motion(1,23): Loco-Bank Clip 23, %d Bilder", re15_actor_clip_len(r));
    int geht = bild_nach_stelle(0x020B2u, 3000);             /* Plc_dest Mode 4 -> John */
    int gang = -1;
    for (int f = 0; f < 20; f++) { frame(); if (r->motion == 5) { gang = re15_actor_clip_len(r); break; } }
    PRUEF(geht > 0 && gang == 61, "Plc_dest @0x020B2 (Bild %d): Gang-Clip 5 der eigenen Bank, %d Bilder (CDEMD1 dir[3])", geht, gang);
    int sturz = bild_nach_stelle(0x020DEu, 2000);            /* Plc_motion(1,7) @0x020DE */
    PRUEF(sturz > 0 && r->motion == 7 && re15_actor_clip_len(r) == 113,
          "Sturz @0x020DE in Bild %d: Loco-Clip 7, %d Bilder", sturz, re15_actor_clip_len(r));
    int max = 0, halt = 0, wrap = 0, prev = -1;
    for (int f = 0; f < 200; f++) {
        frame();
        int af = (int)r->anim_frame;
        if (r->motion != 7) break;
        if (af > max) max = af;
        if (prev >= 0 && af < prev) wrap++;
        if (af == 112) halt++;
        prev = af;
    }
    PRUEF(max == 112 && wrap == 0 && halt >= 60,
          "Clip 7 laeuft bis Bild %d und haelt (%d Bilder auf 112, %d Ruecksprunge; +0x1c4 = pc[3] = 0 @0x80041bc8)",
          max, halt, wrap);
}

/* ---- leon1170 (Gegenprobe) ----------------------------------------------------------------------------- */
static void teil_leon1170(void)
{
    printf("== leon1170 ==\n");
    re15_game_state_init();
    g_gameflow.character = 0;
    re15_game_flag_set(3, 125, 1); re15_game_flag_set(4, 0xF2, 1);   /* Erstbesuch vorbei, Elliot-Gate sub15 */
    static const uint8_t typen[] = { 0x47 };
    if (room_boot(0x1170, 1000, 9000, 0, 0, typen, 1) != 0) return;
    for (int f = 0; f < 5; f++) frame();
    int el = slot_von_typ(0x47);
    PRUEF(el > 0, "Elliot (Typ 0x47) gespawnt (sub15 @0x017D2), Aktor %d", el);
    if (el < 0) return;
    PRUEF(re15_typ47_ist_elliot(0x47) && re15_actor_rbj_anim(el) == NULL,
          "Leon: Elliot behaelt die Marker-Binder-Ausnahme (O3) — kein Record-Kanal, Gesten aus seiner eigenen Bank");
    re15_enemy_bank_t *eb = re15_enemy_find(0x47);
    PRUEF(eb && eb->loco_ok && fc(&eb->anim_loco, 7) == 1, "Leon-Registry-Bank 0x47 = CDEMD0 (Loco-Clip 7 = %d)",
          eb ? fc(&eb->anim_loco, 7) : -1);
}


/* ---- spur: Auswertung eines RE15_ANIM_TRACE der echten exe (Integration test_r37_roy.cmake) ------------------
 * Zeile: bild aktor typ motion clip fc cur slot fc_uhr loco rev (11 Felder, anim_select_common.c). Geprueft fuer Typ 0x47:
 *   - clip == motion (kein Modulo-Klemmen auf einen fremden Clip-Satz) und fc == fc_uhr (Bild und Uhr dieselbe Bank)
 *   - Clip -> Bildzahl = die Original-Banken des Raums (1011: RBJ-Record 1; 1021: CDEMD1 Loco/eigene Bank)
 *   - Einmal-Clips (ohne Skript-Schleife): vorwaerts nie zurueck, rueckwaerts nie vor = HALTEN statt Schleife
 *   - Schluss-Clip (1011 Clip 9 / 1021 Clip 7) endet auf dem letzten Bild und bleibt dort */
static int spur(const char *pfad, const char *raum)
{
    printf("== spur %s (%s) ==\n", raum, pfad);
    int is1011 = !strcmp(raum, "1011");
    if (!strcmp(raum, "1170")) {
        FILE *g = fopen(pfad, "r");
        PRUEF(g != NULL, "Spur lesbar");
        if (!g) return 1;
        char zz[256]; int n = 0, c15 = 0, c15ok = 0, c2 = 0, c2ok = 0, uhr = 0;
        while (fgets(zz, sizeof zz, g)) {
            unsigned bild, cur; int akt, typ, mo, clip, fcc, slot, fcu, loco, rev;
            if (sscanf(zz, "%u %d %d %d %d %d %u %d %d %d %d", &bild, &akt, &typ, &mo, &clip, &fcc, &cur, &slot,
                       &fcu, &loco, &rev) != 11 || typ != 0x47) continue;
            n++;
            if (fcc != fcu) uhr++;
            if (mo == 15) { c15++; if (fcc == 20) c15ok++; }
            if (mo == 2)  { c2++;  if (fcc == 52) c2ok++; }
        }
        fclose(g);
        /* Uhr != Bild bei Elliots Gesten ist der VORBESTEHENDE Leon-Stand (O3-Ausnahme: der Takt liest Clip 15 der
         * eigenen Bank, das Bild das REC0-Overlay; Kommentar re15_actor_clip_len) — hier nur gemeldet, nicht veraendert. */
        PRUEF(n > 100, "Elliot: %d Spurzeilen (Uhr != Bild in %d Zeilen = Leon-Stand vor Runde 37, unveraendert)", n, uhr);
        PRUEF(c15 > 0 && c15ok == c15, "Elliot-Geste Clip 15 = 20 Bilder (RBJ-REC0 auf ELLIOT, %d/%d Zeilen)", c15ok, c15);
        PRUEF(c2 > 0 && c2ok == c2, "Elliot Clip 2 = 52 Bilder (EM047-Bank 1 CDEMD0, %d/%d Zeilen)", c2ok, c2);
        return 0;
    }
    /* erwartete Bildzahl je Clip (-1 = darf nicht vorkommen); schleife = Clip mit Plc_flg 4 im Skript */
    int soll[32], schleife[32];
    for (int i = 0; i < 32; i++) { soll[i] = -1; schleife[i] = 0; }
    int schluss, schluss_fc;
    if (is1011) { soll[5] = 28; soll[6] = 78; soll[7] = 30; soll[9] = 100; soll[10] = 60; soll[13] = 2;
                  schleife[5] = 1; schleife[13] = 1; schluss = 9; schluss_fc = 100; }
    else        { soll[23] = 30; soll[5] = 61; soll[7] = 113; schleife[23] = 1; schleife[5] = 1; schluss = 7; schluss_fc = 113; }
    FILE *f = fopen(pfad, "r");
    PRUEF(f != NULL, "Spur lesbar");
    if (!f) return 1;
    char z[256];
    int n = 0, fremd = 0, uhr = 0, falsch = 0, rueck = 0, gesehen[32] = {0};
    int pm = -1, pr = -1, ps = -1, letzter_clip = -1, letzter_slot = -1, schluss_ende = 0, schluss_max = -1;
    while (fgets(z, sizeof z, f)) {
        unsigned bild; int akt, typ, mo, clip, fcc, slot, fcu, loco, rev; unsigned cur;
        /* 11 Felder: der Formatstring in anim_select_common.c hat 11 Konversionen (legacy wird nicht gedruckt) */
        if (sscanf(z, "%u %d %d %d %d %d %u %d %d %d %d", &bild, &akt, &typ, &mo, &clip, &fcc, &cur, &slot,
                   &fcu, &loco, &rev) != 11 || typ != 0x47) continue;
        n++;
        if (clip != mo) fremd++;
        if (fcc != fcu) uhr++;
        if (mo < 0 || mo >= 32 || soll[mo] != fcc) { if (falsch < 5) printf("  (Clip %d mit %d Bildern in Bild %u)\n", mo, fcc, bild); falsch++; }
        else gesehen[mo] = 1;
        if (mo == pm && rev == pr && mo >= 0 && mo < 32 && !schleife[mo]) {
            if ((!rev && slot < ps) || (rev && slot > ps)) { if (rueck < 5) printf("  (Clip %d springt in Bild %u von %d auf %d)\n", mo, bild, ps, slot); rueck++; }
        }
        if (mo == schluss) { if (slot > schluss_max) schluss_max = slot; }
        pm = mo; pr = rev; ps = slot; letzter_clip = mo; letzter_slot = slot;
        if (mo == schluss && slot == schluss_fc - 1) schluss_ende++;
    }
    fclose(f);
    int alle = 1;
    for (int i = 0; i < 32; i++) if (soll[i] > 0 && !gesehen[i] && !(is1011 && i == 13)) alle = 0;
    PRUEF(n > 200, "%d Spurzeilen fuer Typ 0x47", n);
    PRUEF(fremd == 0, "Clip == Motion in jeder Zeile (%d Klemmungen)", fremd);
    PRUEF(uhr == 0, "Bild und Uhr aus derselben Bank (%d Abweichungen)", uhr);
    PRUEF(falsch == 0 && alle, "jeder Clip mit der Original-Bildzahl (%d falsch), alle Szenen-Clips gesehen", falsch);
    PRUEF(rueck == 0, "kein Einmal-Clip springt zurueck (keine Schleife ueber das Ende, %d)", rueck);
    PRUEF(letzter_clip == schluss && letzter_slot == schluss_fc - 1 && schluss_max == schluss_fc - 1 && schluss_ende >= 30,
          "Schluss-Clip %d endet auf Bild %d und haelt (%d Bilder)", schluss, letzter_slot, schluss_ende);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc > 3 && !strcmp(argv[1], "spur")) {
        spur(argv[2], argv[3]);
        printf("%s: %s (%d Fehler)\n", "test_r37_roy spur", g_fail ? "FEHLER" : "OK", g_fail);
        return g_fail ? 1 : 0;
    }
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "toc")       || !strcmp(teil, "alle")) teil_toc();
    if (!strcmp(teil, "weiche")    || !strcmp(teil, "alle")) teil_weiche();
    if (!strcmp(teil, "szene1011") || !strcmp(teil, "alle")) teil_szene1011();
    if (!strcmp(teil, "szene1021") || !strcmp(teil, "alle")) teil_szene1021();
    if (!strcmp(teil, "leon1170")  || !strcmp(teil, "alle")) teil_leon1170();
    printf("%s: %s (%d Fehler)\n", "test_r37_roy", g_fail ? "FEHLER" : "OK", g_fail);
    return g_fail ? 1 : 0;
}
