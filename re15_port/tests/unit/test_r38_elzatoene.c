/* test_r38_elzatoene.c — RIEGEL Spur H (Runde 38) "elzatoene": misst die MECHANIK jedes Nutzer-Punkts am echten VM.
 *
 * Dossier: analysis/befunde_runde38/H_elzatoene.md. Konstanten/Belege: include/re15_elzatoene_r38.h,
 * include/re15_elza_r36.h, engine/src/elzatoene_r38.c, tools/r36_l/re2_elza_se_cut.py (ELZASE.VBS).
 * Verfahren wie test_r36_elza.c: ausgelieferte RDT laden, Raum ueber scd_room_reenter aufbauen (dieselben Haken wie im
 * Spiel: Anker-Scan in scd_register_current_rdt, Opcode-Haken, Message_on-Haken), Bilder in main.c-Reihenfolge fahren.
 *
 * Teile (je ein ctest-Eintrag):
 *   gunshot1041  P1 ROOM1041 sub11: Schuss-Ton im Takt von @0x02200, Nachricht 3 "*gunshot*" erscheint NIE, danach
 *                msg 2 "Elza: What was that!?" wie im Original
 *   stoehnen1021 P2 ROOM1021 sub02: genau ein Raum-SE 4 im Takt von Roys Gang @0x020B2, 150 Bilder vor Schuss 1
 *   schuss1021   P3 ROOM1021 sub02: zwei Schuss-Toene je +1 nach @0x020CE/@0x020D6, Satz = RE2 ROOM4040 0x0A
 *   bank         ELZASE.VBS Satz 1 bytegleich RE2 ROOM4040 Satz 0x0A (EDT/Ton/VAG), NICHT die Alligator-Welle
 *   re2beleg     die RE2-Bytes hinter der Korrektur: ROOM40A0 = Alligator + Schleife; ROOM4040/4090 = Skript-Schuss
 *   signatur     Anker nur bei passender Signatur; Leon-Raeume 1020/1040 ohne Anker */
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
#include "re15_vab.h"
#include "re15_elza_r36.h"
#include "re15_elzatoene_r38.h"
#include "../../engine/src/gen/re2_elza_bank.inc"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void re15_actor_step_all_walkers(void);
extern int g_test_room_se_log[2048], g_test_room_se_tickof[2048], g_test_room_se_n, g_test_room_se_tick;

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

/* Spione: Plattform-Ton (re15_elza_se_hook), Stimmen-Ereignisse, gezeigte Nachrichten je Bild. */
static int s_ton_n = 0, s_ton[16], s_ton_bild[16];
static void ton_spion(int ton) { if (s_ton_n < 16) { s_ton[s_ton_n] = ton; s_ton_bild[s_ton_n] = s_bild; s_ton_n++; } }
static int s_stimme_n = 0, s_stimme[64], s_stimme_bild[64];
static int s_msg_erst[64];                      /* erstes Bild, in dem Nachricht i offen war (-1 = nie) */

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

static uint8_t *s_ems = NULL; static size_t s_ems_sz = 0;
static uint8_t  s_bank_buf[3][0x60000];
static void bank_laden(uint8_t type, int buf)
{
    if (!s_ems) {
        char p2[600]; snprintf(p2, sizeof p2, "%s/EMD/CDEMD0.EMS", RE15_ASSET_PSX_DIR);
        s_ems = slurp(p2, &s_ems_sz);
    }
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
    g_test_room_se_tick = s_bild;
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
    while (scd_audio_queue_pop(&e))
        if (e.kind == SCD_AUDIO_VOICE_ON && s_stimme_n < 64) { s_stimme[s_stimme_n] = e.sample_id; s_stimme_bild[s_stimme_n++] = s_bild; }
    if (g_scd.message_active && g_scd.message_id < 64 && s_msg_erst[g_scd.message_id] < 0)
        s_msg_erst[g_scd.message_id] = s_bild;
    s_bild++;
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
    bank_laden(0x47, 0); bank_laden(0x49, 1); bank_laden(0x16, 2);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = cut; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    re15_elza_r36_reset();
    re15_elzatoene_r38_reset();
    s_ton_n = 0; s_stimme_n = 0; g_test_room_se_n = 0; s_bild = 0;
    for (int i = 0; i < 64; i++) s_msg_erst[i] = -1;
    re15_elza_se_hook = ton_spion;
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

/* Bild, in dem der Opcode an der RDT-Datei-Stelle `off` ausgefuehrt wurde: nach diesem Bild steht ein Faden zwischen
 * off+1 und off+16 (der Faden parkt im folgenden Sleep). */
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

/* ---- P1 ROOM1041 "*gunshot*" -> Schuss-Ton ------------------------------------------------------------------- */
static void teil_gunshot1041(void)
{
    printf("== gunshot1041 ==\n");
    re15_game_state_init();                       /* erster Besuch: (4,0xF1) == 0 und (3,0xAD) == 0 */
    /* Elza betritt das Auto-Feld AOT 8 (main00 @0x01178: Rechteck (-23800,-15500) 2000 x 14450, sce 3 -> sub11) von
     * Osten wie ueber die Tuer aus ROOM1031 (Ankunft (-20083,-3559)): erst ausserhalb, dann hinein (das Feld feuert beim
     * Betreten, nicht am Startplatz). */
    if (room_boot(0x1041, -20500, -8000, 2048, 0) != 0) return;
    PRUEF(g_re15_elzatoene_anker_n == 1 && re15_elzatoene_r38_anker_off(0) == RE15_ELZATOENE_1041_OFF,
          "ein Anker @0x%05X (Message_on 3)", re15_elzatoene_r38_anker_off(0));
    for (int f = 0; f < 3; f++) frame();
    PRUEF(re15_game_flag_get(3, 0xAD) == 0, "vor dem Feld: sub11 noch nicht gelaufen");
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = -22800;
    int b = bild_nach_stelle(RE15_ELZATOENE_1041_OFF, 600);
    PRUEF(b >= 0, "sub11 erreicht @0x02200 in Bild %d", b);
    PRUEF(re15_game_flag_get(3, 0xAD) == 1, "sub11 lief: (3,0xAD) gesetzt (@0x021EA)");
    PRUEF(s_ton_n == 1 && s_ton[0] == RE15_ELZA_TON_SCHUSS && s_ton_bild[0] == b,
          "genau ein Schuss-Ton (Satz %d) im Bild der Stelle @0x02200 (Ton-Bild %d)", s_ton_n ? s_ton[0] : -1,
          s_ton_n ? s_ton_bild[0] : -1);
    PRUEF(re15_elzatoene_r38_zaehler(RE15_ELZATOENE_AKT_SCHUSS_1041) == 1, "Aktion einmal");
    for (int f = 0; f < 600 && s_msg_erst[2] < 0; f++) frame();
    PRUEF(s_msg_erst[RE15_ELZATOENE_1041_MSG] < 0, "Nachricht 3 (\"*gunshot*\") erschien nie (erstes Bild %d)",
          s_msg_erst[RE15_ELZATOENE_1041_MSG]);
    PRUEF(s_msg_erst[2] > b, "danach msg 2 \"Elza: What was that!?\" wie im Original (Bild %d)", s_msg_erst[2]);
    int st2 = -1, st3 = -1;
    for (int i = 0; i < s_stimme_n; i++) { if (s_stimme[i] == 2) st2 = s_stimme_bild[i]; if (s_stimme[i] == 3) st3 = s_stimme_bild[i]; }
    PRUEF(st2 == s_msg_erst[2] && st3 < 0, "Stimme 2 mit msg 2 eingereiht (Bild %d), keine Stimme 3", st2);
    PRUEF(s_ton_n == 1, "kein weiterer Ton bis msg 2 (%d)", s_ton_n);
    /* GEGENPROBE (Stand vor Runde 38 = Original): derselbe Weg ohne armierten Anker zeigt den Platzhaltertext 3 im
     * Bild der Stelle @0x02200 und spielt keinen Ton — der Riegel misst also die Ersetzung. */
    re15_game_state_init();
    if (room_boot(0x1041, -20500, -8000, 2048, 0) != 0) return;
    g_re15_elzatoene_anker_n = 0;
    for (int f = 0; f < 3; f++) frame();
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = -22800;
    int bg = bild_nach_stelle(RE15_ELZATOENE_1041_OFF, 600);
    PRUEF(bg >= 0 && s_msg_erst[RE15_ELZATOENE_1041_MSG] == bg && s_ton_n == 0,
          "Gegenprobe ohne Anker: Text 3 in Bild %d (Stelle %d), %d Toene", s_msg_erst[RE15_ELZATOENE_1041_MSG], bg, s_ton_n);
    /* Leon: ROOM1040 traegt den Platzhalter nicht (Nachrichten-Zensus) -> kein Anker */
    if (rdt_laden(0x1040) == 0) {
        re15_elzatoene_r38_raum_scan(s_raw, (int)s_rawsz, 0x1040);
        PRUEF(g_re15_elzatoene_anker_n == 0, "ROOM1040 (Leon): kein Anker");
    }
}

/* ---- P2/P3 ROOM1021 ------------------------------------------------------------------------------------------- */
static int s_gang = -1, s_schuss1 = -1, s_schuss2 = -1;
static int s_ohne_r38 = 0;                      /* Gegenprobe: Runde-38-Anker nicht armiert (= Stand vorher) */
static void szene_1021(void)
{
    re15_game_state_init();
    re15_game_flag_set(4, 0x60, 1); re15_game_flag_set(3, 0xC9, 1);   /* ROOM1211 sub02 @0x01F82 / @0x01F86 */
    s_gang = s_schuss1 = s_schuss2 = -1;
    if (room_boot(0x1021, -13100, -9100, 0, 9) != 0) return;
    if (s_ohne_r38) g_re15_elzatoene_anker_n = 0;
    s_gang    = bild_nach_stelle(RE15_ELZATOENE_1021_OFF, 2000);
    s_schuss1 = bild_nach_stelle(0x020CEu, 400);
    s_schuss2 = bild_nach_stelle(0x020D6u, 200);
    for (int f = 0; f < 10; f++) frame();
}

static void teil_stoehnen1021(void)
{
    printf("== stoehnen1021 ==\n");
    szene_1021();
    PRUEF(g_re15_elzatoene_anker_n == 1 && re15_elzatoene_r38_anker_off(0) == RE15_ELZATOENE_1021_OFF,
          "ein Runde-38-Anker @0x%05X (Roys Plc_dest), Runde-36-Anker weiter %d", re15_elzatoene_r38_anker_off(0),
          g_re15_elza_anker_n);
    PRUEF(g_re15_elza_anker_n == 3, "Runde-36-Anker unveraendert 3");
    PRUEF(s_gang > 0, "Roys Gang @0x020B2 in Bild %d", s_gang);
    int n4 = 0, t4 = -1, n5 = 0;
    for (int i = 0; i < g_test_room_se_n; i++) {
        if (g_test_room_se_log[i] == RE15_ELZATOENE_ROY_STOEHN_SE) { n4++; t4 = g_test_room_se_tickof[i]; }
        if (g_test_room_se_log[i] == RE15_ELZA_ROY_SE) n5++;
    }
    PRUEF(n4 == 1, "genau ein Raum-SE 4 (Gang-Stoehnen, FUN_801021f8 @0x80102298/@0x8010229c) in der Szene (%d)", n4);
    PRUEF(t4 == s_gang, "im Takt des Gangbeginns (SE-Bild %d, Gang %d)", t4, s_gang);
    PRUEF(s_schuss1 - s_gang == 150, "Schuss 1 genau 150 Bilder spaeter (Sleep 40 @0x020BE + 110 @0x020CA): %d",
          s_schuss1 - s_gang);
    PRUEF(n5 == 1, "Roys Sterbe-Ton SE 5 weiter genau einmal (Runde 36 P5): %d", n5);
    const re15_actor_t *roy = &g_actors[2];
    PRUEF(roy->type == 0x47, "Aktor 2 = Roy (Typ 0x47)");
    PRUEF(re15_elzatoene_r38_zaehler(RE15_ELZATOENE_AKT_ROY_STOEHNEN) == 1, "Aktion einmal");
    /* GEGENPROBE (Original/Stand vorher): ohne Anker stoehnt Roy beim Gang nicht — kein SE 4 in der ganzen Szene. */
    s_ohne_r38 = 1; szene_1021(); s_ohne_r38 = 0;
    int g4 = 0;
    for (int i = 0; i < g_test_room_se_n; i++) if (g_test_room_se_log[i] == RE15_ELZATOENE_ROY_STOEHN_SE) g4++;
    PRUEF(s_gang > 0 && g4 == 0, "Gegenprobe ohne Anker: Gang in Bild %d, %d x SE 4", s_gang, g4);
}

static void teil_schuss1021(void)
{
    printf("== schuss1021 ==\n");
    szene_1021();
    PRUEF(s_schuss1 > 0 && s_schuss2 - s_schuss1 == 30, "Feuer-Clips in Bild %d und %d (Sleep 30 @0x020D2)", s_schuss1, s_schuss2);
    int n = 0, b1 = -1, b2 = -1;
    for (int i = 0; i < s_ton_n; i++) if (s_ton[i] == RE15_ELZA_TON_SCHUSS) { if (n == 0) b1 = s_ton_bild[i]; else b2 = s_ton_bild[i]; n++; }
    PRUEF(n == 2 && s_ton_n == 2, "zwei Schuss-Toene, keine anderen (%d/%d)", n, s_ton_n);
    PRUEF(b1 - s_schuss1 == 1 && b2 - s_schuss2 == 1, "je 1 Bild nach dem Feuer-Clip: +%d / +%d", b1 - s_schuss1, b2 - s_schuss2);
    PRUEF(RE15_ELZA_TON_SCHUSS == RE2_ELZA_SE_SCHUSS, "Ton = Mini-Bank-Satz %d (gen/re2_elza_bank.inc)", RE2_ELZA_SE_SCHUSS);
}

/* ---- Bank ------------------------------------------------------------------------------------------------------- */
static uint8_t *re2_rdt(const char *raum, size_t *n)
{
    char q[600]; snprintf(q, sizeof q, "%s/../../../info/re2leon/PL0/RDT/ROOM%s.RDT", RE15_ASSET_PSX_DIR, raum);
    return slurp(q, n);
}

static void teil_bank(void)
{
    printf("== bank ==\n");
    char p[600]; snprintf(p, sizeof p, "%s/../RE2/ELZASE.VBS", RE15_ASSET_PSX_DIR);
    size_t n = 0; uint8_t *b = slurp(p, &n);
    PRUEF(b && n == RE2_ELZA_VBD_OFF + RE2_ELZA_VBD_SIZE, "ELZASE.VBS %u B", (unsigned)n);
    if (!b) return;
    uint32_t vh = (uint32_t)b[RE2_ELZA_EDT_SIZE - 8] | ((uint32_t)b[RE2_ELZA_EDT_SIZE - 7] << 8);
    re15_vab_t vab; int rc = re15_vab_parse(b + vh, RE2_ELZA_EDT_SIZE - vh, &vab);
    PRUEF(rc == 0 && vab.vag_count == 2 && vab.samples[1].size == 5824, "Satz 1: VAG %u B (ROOM4040 Satz 0x0A)",
          rc == 0 ? (unsigned)vab.samples[1].size : 0u);
    re15_edt_rec_t e1;
    PRUEF(re15_edt_decode(b, RE2_ELZA_SE_SCHUSS, &e1) == 0 && !e1.empty && e1.tone == 1 && e1.prio == 3 && e1.voice < 0,
          "EDT Satz 1: Ton 1, Prio %d (RE2 `00 00 73 00`: Nibble 3), Direkt-Zweig (Stimme %d)", e1.prio, e1.voice);
    size_t n40 = 0, na0 = 0;
    uint8_t *r40 = re2_rdt("4040", &n40), *ra0 = re2_rdt("40A0", &na0);
    if (r40 && ra0 && rc == 0) {
        const uint8_t *w = b + RE2_ELZA_VBD_OFF + vab.samples[1].offset;
        PRUEF(!memcmp(w, r40 + 0x18CC8, 5824), "VAG bytegleich RE2 ROOM4040 @0x18CC8 (5824 B)");
        PRUEF(memcmp(w, ra0 + 0x17B80, 64) != 0, "NICHT mehr die Alligator-Welle ROOM40A0 @0x17B80");
        /* Ton-Satz: bytegleich bis auf den VAG-Index (+0x16) — vol/pan/center/shift/min wie RE2 @0x0C8F8 */
        PRUEF(vab.tones[1].vol == r40[0x0C8F8 + 2] && vab.tones[1].pan == r40[0x0C8F8 + 3] &&
              vab.tones[1].center_note == r40[0x0C8F8 + 4] && vab.tones[1].pitch_shift == r40[0x0C8F8 + 5] &&
              vab.tones[1].min_note == r40[0x0C8F8 + 6],
              "Ton wie RE2 @0x0C8F8: vol %u pan %u center %u shift %u min %u", vab.tones[1].vol, vab.tones[1].pan,
              vab.tones[1].center_note, vab.tones[1].pitch_shift, vab.tones[1].min_note);
    } else printf("  (info/re2leon fehlt — Quellvergleich uebersprungen)\n");
    free(r40); free(ra0); free(b);
}

/* ---- RE2-Beleg der Korrektur (Bytes, die das Dossier zitiert) ------------------------------------------------- */
static void teil_re2beleg(void)
{
    printf("== re2beleg ==\n");
    size_t na0 = 0, n40 = 0, n90 = 0;
    uint8_t *a0 = re2_rdt("40A0", &na0), *r40 = re2_rdt("4040", &n40), *r90 = re2_rdt("4090", &n90);
    if (!a0 || !r40 || !r90) { printf("  (info/re2leon fehlt — uebersprungen)\n"); free(a0); free(r40); free(r90); return; }
    /* ROOM40A0: einziger Sce_em_set = Alligator Typ 0x23; Se_on(2,8) in der While-Schleife am Gegner 0 */
    static const uint8_t em[4] = { 0x44, 0x00, 0x00, 0x23 };
    int n_em = 0;
    for (size_t i = 0; i + 22 < na0 && i < 0x8000; i++)
        if (a0[i] == 0x44 && a0[i + 1] == 0x00 && a0[i + 2] < 16 && a0[i + 3] >= 0x10 && a0[i + 3] < 0x60 && a0[i + 4] < 0x20) n_em++;
    PRUEF(!memcmp(a0 + 0x025A0, em, 4) && n_em == 1, "ROOM40A0: Sce_em_set @0x025A0 `44 00 00 23` = einziger Gegner (Alligator), %d", n_em);
    static const uint8_t sch[] = { 0x2e,0x02,0x00, 0x3f,0x02,0x0e,0x04, 0x00, 0x09,0x0a,0x02,0x00, 0x0f,0x04,0x82,0x00,
                                   0x21,0x05,0x0b,0x00, 0x09,0x0a,0x08,0x00, 0x36,0x02,0x08,0x01,0x02,0x00 };
    PRUEF(!memcmp(a0 + 0x0287E, sch, sizeof sch),
          "ROOM40A0 @0x0287E: Work_set(2,0) Plc_motion(2,14,4) .. While @0x0288A Ck(5,0x0b) .. Se_on(2,8) @0x02896 (Ursprung 2/0)");
    /* ROOM4040: Skript-Schuss Se_on(2,0x0A) + Muendungsfeuer, Welle @0x18CC8 == ROOM4090 @0x3E724 */
    static const uint8_t s40[] = { 0x36,0x02,0x0a,0x00,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00, 0x3a,0x00,0x18 };
    PRUEF(!memcmp(r40 + 0x014E4, s40, sizeof s40), "ROOM4040 @0x014E4 Se_on(2,0x0A) + @0x014F0 `3a 00 18` (Muendungsfeuer)");
    static const uint8_t e40[4] = { 0x00, 0x00, 0x73, 0x00 };
    PRUEF(!memcmp(r40 + 0x0BF60, e40, 4), "ROOM4040 EDT @0x0BF60 Satz 0x0A = `00 00 73 00`");
    PRUEF(n90 > 0x3E724 + 5824 && !memcmp(r40 + 0x18CC8, r90 + 0x3E724, 5824),
          "dieselbe Welle als Skript-Schuss in ROOM4090 (@0x3E724, Se_on @0x02A4E)");
    static const uint8_t s90[] = { 0x36,0x02,0x08,0x01 };
    PRUEF(!memcmp(r90 + 0x02A4E, s90, 4), "ROOM4090 @0x02A4E Se_on(2,8)");
    free(a0); free(r40); free(r90);
}

/* ---- Signatur --------------------------------------------------------------------------------------------------- */
static void teil_signatur(void)
{
    printf("== signatur ==\n");
    if (rdt_laden(0x1041) != 0) { g_fail++; return; }
    uint8_t *k = (uint8_t *)malloc(s_rawsz); memcpy(k, s_raw, s_rawsz);
    re15_elzatoene_r38_raum_scan(k, (int)s_rawsz, 0x1041);
    PRUEF(g_re15_elzatoene_anker_n == 1, "ROOM1041 Original: 1 Anker");
    k[RE15_ELZATOENE_1041_OFF + 1] ^= 0xFF;                     /* Nachrichten-Nummer verfaelscht */
    re15_elzatoene_r38_raum_scan(k, (int)s_rawsz, 0x1041);
    PRUEF(g_re15_elzatoene_anker_n == 0, "Signatur verfaelscht -> kein Anker");
    PRUEF(re15_elzatoene_r38_message_on(s_raw + RE15_ELZATOENE_1041_OFF) == 0, "unarmiert: Message_on bleibt Text");
    free(k);
    re15_elzatoene_r38_raum_scan(s_raw, (int)s_rawsz, 0x1040);
    PRUEF(g_re15_elzatoene_anker_n == 0, "Leon-Raumnummer: kein Anker");
    if (rdt_laden(0x1021) == 0) {
        re15_elzatoene_r38_raum_scan(s_raw, (int)s_rawsz, 0x1021);
        PRUEF(g_re15_elzatoene_anker_n == 1 && re15_elzatoene_r38_anker_off(0) == RE15_ELZATOENE_1021_OFF, "ROOM1021: 1 Anker");
        PRUEF(re15_elzatoene_r38_message_on(s_raw + RE15_ELZATOENE_1021_OFF) == 0,
              "der Gang-Anker ersetzt keine Nachricht");
    }
    if (rdt_laden(0x1020) == 0) {
        re15_elzatoene_r38_raum_scan(s_raw, (int)s_rawsz, 0x1020);
        PRUEF(g_re15_elzatoene_anker_n == 0, "ROOM1020 (Leon): kein Anker");
    }
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    re15_game_state_init();
    if (!strcmp(teil, "gunshot1041")  || !strcmp(teil, "alle")) teil_gunshot1041();
    if (!strcmp(teil, "stoehnen1021") || !strcmp(teil, "alle")) teil_stoehnen1021();
    if (!strcmp(teil, "schuss1021")   || !strcmp(teil, "alle")) teil_schuss1021();
    if (!strcmp(teil, "bank")         || !strcmp(teil, "alle")) teil_bank();
    if (!strcmp(teil, "re2beleg")     || !strcmp(teil, "alle")) teil_re2beleg();
    if (!strcmp(teil, "signatur")     || !strcmp(teil, "alle")) teil_signatur();
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
