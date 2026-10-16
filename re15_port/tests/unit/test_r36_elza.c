/* test_r36_elza.c — RIEGEL Spur L (Runde 36) "elza": misst die MECHANIK jedes Nutzer-Punkts am echten VM.
 *
 * Dossier: analysis/befunde_runde36/L_elza.md. Konstanten/Belege: include/re15_elza_r36.h,
 * include/re15_fenster1120.h (Abschnitt ELZA-ZWEIG), engine/src/elza_r36.c, engine/src/fenster_1120.c.
 * Verfahren wie test_r35_cut1150.c: ausgelieferte RDT laden, Raum ueber scd_room_reenter aufbauen (dieselben
 * Haken wie im Spiel: Installer, Anker-Scan in scd_register_current_rdt, Opcode-/Takt-Haken), Bilder in der
 * Reihenfolge von platform/pc/main.c fahren und MESSEN.
 *
 * Teile (je ein ctest-Eintrag):
 *   fenster    P1 ROOM1121: Schaden Cut 1 immer sichtbar, vierte Kraehe (Slot 3, Kill-Flag 0xAC), Leon 1120 unberuehrt
 *   irons      P2 ROOM1151: Totenpose Clip 2 Bild 89 gehalten, Kopf fest, Anklicken -> Text 20, kein sub03/msg 2
 *   unlock     P3 ROOM1211: Unlock-Ton genau 20 Takte nach der Aufschliess-Geste @0x020F8
 *   schuss     P4 ROOM1021: zwei Schuss-Toene je 1 Takt nach @0x020CE / @0x020D6
 *   roy        P5 ROOM1021: Roys Sterbe-Ton = Raum-SE 5 im Takt von Schuss 2, hinter dem Schuss
 *   stoehnen   P6 ROOM1011: Nachricht 24 = ROOM1011 msg 10 woertlich + Stimme 24 -> main16.5 im Takt von @0x00D54
 *   bank       ELZASE.VBS: zwei Saetze, VAG-Groessen 5264/4736, EDT/Tone wie die RE2-Quelle
 *   signatur   Anker armieren nur bei passender Signatur; Leon-Raeume ohne Anker */
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
#include "re15_fenster1120.h"
#include "re15_inventory.h"   /* Runde 37 Spur Q: re15_inv_grant (Prison Key im Inventar, s. room_boot) */
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

/* Spione: Plattform-Ton (re15_elza_se_hook) und Stimmen-Ereignisse (scd_audio_queue). */
static int s_ton_n = 0, s_ton[16], s_ton_bild[16];
static void ton_spion(int ton) { if (s_ton_n < 16) { s_ton[s_ton_n] = ton; s_ton_bild[s_ton_n] = s_bild; s_ton_n++; } }
static int s_stimme_n = 0, s_stimme[64], s_stimme_bild[64];

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
static uint8_t  s_bank_buf[4][0x60000];
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
    s_bild++;
}

/* Runde 37 Spur Q: Gegenstand, den room_boot nach scd_vm_init (leert das Inventar) ins INVENTAR legt (0 = keiner).
 * "Prison key genommen" heisst seit der Kistenregel Bit (4,0xEC) UND Item 0x42 im Inventar (kistenregel_r37.c K11,
 * ROOM1211 sub00 @0x01DEA; RE2 Keep_Item_ck @0x800584F0 -> FUN_800696CC sucht nur das Inventar). */
static uint8_t s_boot_gabe = 0;
static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, uint8_t cut)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    if (s_boot_gabe) re15_inv_grant(s_boot_gabe, 1);   /* Runde 37 Spur Q (s.o.) */
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    bank_laden(0x47, 0); bank_laden(0x49, 1); bank_laden(0x45, 2); bank_laden(0x21, 3);
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
    s_ton_n = 0; s_stimme_n = 0; g_test_room_se_n = 0; s_bild = 0;
    re15_elza_se_hook = ton_spion;
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

static int kraehen(void)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == 0x21) n++;
    return n;
}

/* Bild, in dem der Opcode an der RDT-Datei-Stelle `off` ausgefuehrt wurde: nach diesem Bild steht ein Faden
 * zwischen off+1 und off+16 (der Anker-Opcode ist ein Plc_motion, danach parkt der Faden im folgenden Sleep). */
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

/* ---- P1 Fenster + Kraehe ROOM1121 -------------------------------------------------------------------------- */
static void teil_fenster(void)
{
    printf("== fenster ==\n");
    re15_game_state_init();
    if (room_boot(0x1121, 5000, 5000, 2048, 1) != 0) return;
    for (int f = 0; f < 10; f++) frame();
    const re15_actor_t *k = &g_actors[RE15_FENSTER_ELZA_EM + 1];
    PRUEF(kraehen() == 4, "ROOM1121: vier Kraehen (main00 drei + Elza-Kraehe), gezaehlt %d", kraehen());
    PRUEF(k->active && k->type == 0x21 && k->x == RE15_FENSTER_ELZA_X && k->z == RE15_FENSTER_ELZA_Z,
          "Elza-Kraehe Aktor %d Typ 0x%02X bei (%d,%d)", RE15_FENSTER_ELZA_EM + 1, k->type, (int)k->x, (int)k->z);
    PRUEF(k->em_flag_id == RE15_FENSTER_ELZA_KILL, "Kill-Flag 0x%02X gelatcht (+0x1C6)", k->em_flag_id);
    PRUEF(re15_fenster1120_schaden_sichtbar(1) == 1 && re15_fenster1120_schaden_sichtbar(0) == 0 &&
          re15_fenster1120_schaden_sichtbar(2) == 0, "Schaden nur auf Cut 1, ohne Ereignis");
    PRUEF(re15_fenster1120_phase() == RE15_FENSTER_AUS && re15_game_flag_get(9, 79) == 0,
          "kein Fenster-Ereignis, (9,79) bleibt 0 (Phase %d)", re15_fenster1120_phase());
    /* erlegt -> bleibt tot (Gate @0x80042128-38) */
    re15_game_flag_set(7, RE15_FENSTER_ELZA_KILL, 1);
    if (room_boot(0x1121, 5000, 5000, 2048, 1) == 0) {
        for (int f = 0; f < 5; f++) frame();
        PRUEF(kraehen() == 3 && re15_fenster1120_schaden_sichtbar(1) == 1, "Kill-Flag gesetzt: wieder drei Kraehen, Fenster bleibt kaputt");
    }
    /* Leon ROOM1120 unberuehrt: ohne (9,73) drei Kraehen, heiles Fenster */
    re15_game_state_init();
    if (room_boot(0x1120, 5000, 5000, 2048, 1) == 0) {
        for (int f = 0; f < 10; f++) frame();
        PRUEF(kraehen() == 3 && re15_fenster1120_schaden_sichtbar(1) == 0 && re15_fenster1120_elza_gespawnt() == 0,
              "Leon ROOM1120: drei Kraehen, Fenster heil");
    }
}

/* ---- P2 Irons ROOM1151 ------------------------------------------------------------------------------------- */
static void teil_irons(void)
{
    printf("== irons ==\n");
    re15_game_state_init();
    if (room_boot(0x1151, -20538, -25147, 1500, 5) != 0) return;
    const re15_actor_t *ir = &g_actors[1];
    int bewegt = 0;
    for (int f = 0; f < 150; f++) {
        frame();
        if (ir->motion != 2 || ir->anim_frame != 89 || ir->sub_state_2 != 2) bewegt = 1;
    }
    PRUEF(ir->active && ir->type == 0x45 && !bewegt, "Irons 150 Bilder Clip 2 Bild 89 Phase 2 (mo=%d bild=%d ph=%d)",
          ir->motion, ir->anim_frame, ir->sub_state_2);
    PRUEF(ir->neck_flags == (0x80 | 0x12) && ir->neck_speed == 0 && ir->neck_yaw == 0 && ir->neck_pitch == 0 && ir->neck_bone == 8,
          "Kopf fest: Flags 0x%02X Geschwindigkeit %d Akku (%d,%d), Kopf-Part %d", ir->neck_flags, ir->neck_speed,
          ir->neck_yaw, ir->neck_pitch, ir->neck_bone);
    const re15_aot_t *a = &g_aot.slots[RE15_ELZA_IRONS_SLOT];
    PRUEF(a->active && a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == RE15_ELZA_IRONS_MSG_ID && a->pause_mask16 == 0xFFFF,
          "Slot 2 = Text-Platz msg %d Maske 0x%04X (Typ %d)", a->event_id, a->pause_mask16, a->type);
    /* Anklicken = derselbe Pfad wie der AOT-Scan (re15_aot_fire_slot) */
    re15_aot_fire_slot(RE15_ELZA_IRONS_SLOT);
    int msg20 = 0, msg2 = 0;
    for (int f = 0; f < 200; f++) {
        frame();
        if (g_scd.message_active && g_scd.message_id == RE15_ELZA_IRONS_MSG_ID) msg20 = 1;
        if (g_scd.message_active && g_scd.message_id == 2) msg2 = 1;
    }
    PRUEF(msg20 && !msg2 && re15_game_flag_get(3, 0x9D) == 0, "Anklicken: Text 20, kein msg 2, (3,0x9D)=%d", re15_game_flag_get(3, 0x9D));
    /* auch der Original-Ansprech-Weg (work_vars[0] = 2) startet sub03 nicht mehr? Nein — er WUERDE (sub01 ist
     * unveraendert); der Slot stempelt ihn nur nicht mehr. Gegenprobe: Slot ist kein sce 5 mehr. */
    PRUEF(a->type != RE15_AOT_TYPE_EXAMINE_WORKVAR, "kein sce-5-Platz mehr in Slot 2 -> work_vars[0] wird nie 2");
    int tl = 0; const uint8_t *tt = re15_elza_r36_meldung(0, &tl);
    char txt[96]; int n = re15_msg_decode_text(tt, (size_t)tl, txt, sizeof txt);
    PRUEF(n > 0 && strstr(txt, "He has already passed...") != NULL, "Text dekodiert: \"%s\"", txt);
    /* Wort-Belege: die Glyphen stehen so im Auslieferungsstand */
    {
        uint8_t *r1011 = NULL, *r10d0 = NULL; size_t n1 = 0, n2 = 0; char p[600];
        snprintf(p, sizeof p, "%s/STAGE1/ROOM1011.RDT", RE15_ASSET_PSX_DIR); r1011 = slurp(p, &n1);
        snprintf(p, sizeof p, "%s/STAGE1/ROOM10D0.RDT", RE15_ASSET_PSX_DIR); r10d0 = slurp(p, &n2);
        const uint8_t *t = re15_elza_r36_meldung(0, NULL);
        int ok = r1011 && r10d0 && n1 > 0x12C0 && n2 > 0x2230 &&
                 !memcmp(t + 2, r1011 + 0x01233, 6) && !memcmp(t + 8, r10d0 + 0x02226, 8) &&
                 !memcmp(t + 16, r1011 + 0x01239, 7) && !memcmp(t + 23, r1011 + 0x012BA, 3) &&
                 t[0] == 0x04 && t[1] == 0x02 && t[26] == 0x01 && t[27] == 0x00;
        PRUEF(ok, "Glyphen = ROOM1011 @0x01233/@0x01239/@0x012BA + ROOM10D0 @0x02226, Rahmen 04 02 .. 01 00");
        free(r1011); free(r10d0);
    }
    /* Leon ROOM1150: unberuehrt (Irons lebt, Slot 2 bleibt sce 5) */
    re15_game_state_init();
    if (room_boot(0x1150, -20538, -25147, 1500, 5) == 0) {
        for (int f = 0; f < 5; f++) frame();
        PRUEF(g_actors[1].motion == 3 && g_aot.slots[2].type == RE15_AOT_TYPE_EXAMINE_WORKVAR,
              "Leon ROOM1150: Irons Clip 3 (lebt), Slot 2 sce 5");
    }
}

/* ---- P3 Unlock ROOM1211 ------------------------------------------------------------------------------------ */
static void teil_unlock(void)
{
    printf("== unlock ==\n");
    re15_game_state_init();
    re15_game_flag_set(4, 0xEC, 1);                        /* Prison key (ROOM1011 sub06 @0x00E2C) */
    s_boot_gabe = 0x42;                                    /* Runde 37 Spur Q: + Prison Key im Inventar */
    int rb = room_boot(0x1211, -19500, -19100, 2048, 7);
    s_boot_gabe = 0;
    if (rb != 0) return;
    PRUEF(g_re15_elza_anker_n == 1 && re15_elza_r36_anker_off(0) == 0x020F8u, "ein Anker @0x%05X", re15_elza_r36_anker_off(0));
    int geste = bild_nach_stelle(0x020F8u, 2000);           /* Bild, in dem die Geste @0x020F8 startet */
    PRUEF(geste > 0, "Szene erreicht die Aufschliess-Geste @0x020F8 in Bild %d", geste);
    int tuer = bild_nach_stelle(0x0210Cu, 200);
    for (int f = 0; f < 5; f++) frame();
    PRUEF(s_ton_n == 1 && s_ton[0] == RE15_ELZA_TON_AUF, "genau ein Ton: Satz %d (AUF)", s_ton_n ? s_ton[0] : -1);
    PRUEF(s_ton_n == 1 && s_ton_bild[0] - geste == 20, "Ton %d Bilder nach der Geste (RE2 ROOM1010 @0x01C1C Sleep 20)",
          s_ton_n ? s_ton_bild[0] - geste : -1);
    PRUEF(tuer > 0 && s_ton_n == 1 && s_ton_bild[0] < tuer, "Ton vor dem Aufschwingen der Zellentuer @0x0210C (Bild %d)", tuer);
}

/* ---- P4/P5 Schuesse + Roy ROOM1021 ------------------------------------------------------------------------- */
static int s_1021_schuss1 = -1, s_1021_schuss2 = -1;
static void szene_1021(void)
{
    re15_game_state_init();
    re15_game_flag_set(4, 0x60, 1); re15_game_flag_set(3, 0xC9, 1);   /* ROOM1211 sub02 @0x01F82 / @0x01F86 */
    if (room_boot(0x1021, -13100, -9100, 0, 9) != 0) return;
    s_1021_schuss1 = bild_nach_stelle(0x020CEu, 2000);
    s_1021_schuss2 = bild_nach_stelle(0x020D6u, 200);
    for (int f = 0; f < 10; f++) frame();
}
static void teil_schuss(void)
{
    printf("== schuss ==\n");
    szene_1021();
    PRUEF(g_re15_elza_anker_n == 3, "drei Anker in ROOM1021 (%d)", g_re15_elza_anker_n);
    PRUEF(s_1021_schuss1 > 0 && s_1021_schuss2 - s_1021_schuss1 == 30, "Feuer-Clips in Bild %d und %d (Sleep 30 @0x020D2)",
          s_1021_schuss1, s_1021_schuss2);
    int n = 0, b1 = -1, b2 = -1;
    for (int i = 0; i < s_ton_n; i++) if (s_ton[i] == RE15_ELZA_TON_SCHUSS) { if (n == 0) b1 = s_ton_bild[i]; else b2 = s_ton_bild[i]; n++; }
    PRUEF(n == 2, "zwei Schuss-Toene (%d)", n);
    PRUEF(b1 - s_1021_schuss1 == 1 && b2 - s_1021_schuss2 == 1, "je 1 Bild nach dem Feuer-Clip (RE2 sub31 @0x02680 Evt_next): +%d / +%d",
          b1 - s_1021_schuss1, b2 - s_1021_schuss2);
    const re15_actor_t *john = &g_actors[1];
    PRUEF(john->type == 0x49, "John (Typ 0x49) schiesst");
}
static void teil_roy(void)
{
    printf("== roy ==\n");
    szene_1021();
    int n5 = 0, t5 = -1;
    for (int i = 0; i < g_test_room_se_n; i++) if (g_test_room_se_log[i] == RE15_ELZA_ROY_SE) { n5++; t5 = g_test_room_se_tickof[i]; }
    PRUEF(n5 == 1, "genau ein Raum-SE 5 (FUN_80106c18 @0x80106da8/ac) in der Szene (%d)", n5);
    PRUEF(t5 == s_1021_schuss2 + 1, "im Bild des zweiten Schuss-Tons (Bild %d, Schuss 2 = %d + 1)", t5, s_1021_schuss2);
    const re15_actor_t *roy = &g_actors[2];
    PRUEF(roy->type == 0x47 && roy->motion == 7, "Roy (Typ 0x47) im Todes-Clip 7");
    PRUEF(re15_elza_r36_zaehler(RE15_ELZA_AKT_ROY_TOD) == 1 && re15_elza_r36_zaehler(RE15_ELZA_AKT_SCHUSS) == 2 &&
          re15_elza_r36_letzter_takt(RE15_ELZA_AKT_ROY_TOD) == re15_elza_r36_letzter_takt(RE15_ELZA_AKT_SCHUSS),
          "Sterbe-Ton und zweiter Schuss im selben VM-Takt (Schuss zuerst eingereiht)");
}

/* ---- P6 ROOM1011 Stoehnen ---------------------------------------------------------------------------------- */
static void teil_stoehnen(void)
{
    printf("== stoehnen ==\n");
    re15_game_state_init();
    if (room_boot(0x1011, 0, 5000, 0, 0) != 0) return;
    PRUEF(g_re15_elza_anker_n == 1 && re15_elza_r36_anker_off(0) == 0x00D54u, "ein Anker @0x%05X", re15_elza_r36_anker_off(0));
    int sackt = bild_nach_stelle(0x00D54u, 3000);              /* das Bild, in dem @0x00D54 laeuft */
    PRUEF(sackt > 0 && g_actors[1].type == 0x47 && g_actors[1].motion == 9, "Roy sackt zusammen (Clip 9) in Bild %d", sackt);
    PRUEF(g_scd.message_active && g_scd.message_id == RE15_ELZA_STOEHNEN_MSG_ID, "Untertitel = Nachricht %d", (int)g_scd.message_id);
    int st = -1;
    for (int i = 0; i < s_stimme_n; i++) if (s_stimme[i] == RE15_ELZA_STOEHNEN_MSG_ID) st = s_stimme_bild[i];
    PRUEF(st == sackt, "Stimme 24 im selben Bild eingereiht (Bild %d)", st);
    const char *d = re15_elza_stimme_datei(0x1011, RE15_ELZA_STOEHNEN_MSG_ID);
    PRUEF(d && !strcmp(d, "main16.5") && !re15_elza_stimme_datei(0x1011, 16) && !re15_elza_stimme_datei(0x1010, 24) &&
          !re15_elza_stimme_datei(0x1101, 24), "Datei (1011,24) -> main16.5, sonst keine Umlenkung");
    /* Text = ROOM1011 msg 10 @0x010C9 woertlich (23 Byte bis msg 11 @0x010E0) */
    int len = 0; const uint8_t *t = re15_elza_r36_meldung(1, &len);
    PRUEF(len == 23 && !memcmp(t, s_rdt.raw + 0x010C9, 23), "Text = ROOM1011 msg 10 @0x010C9 (23 Byte) woertlich");
    char txt[64]; re15_msg_decode_text(t, (size_t)len, txt, sizeof txt);
    printf("  (Untertitel dekodiert: \"%s\")\n", txt);
    /* danach laeuft das Original weiter: msg 17 */
    int m17 = 0;
    for (int f = 0; f < 200; f++) { frame(); if (g_scd.message_active && g_scd.message_id == 17) m17 = 1; }
    PRUEF(m17, "danach msg 17 \"Elza: Roy, wake up!\" wie im Original");
}

/* ---- Bank ELZASE.VBS -------------------------------------------------------------------------------------- */
static void teil_bank(void)
{
    printf("== bank ==\n");
    char p[600]; snprintf(p, sizeof p, "%s/../RE2/ELZASE.VBS", RE15_ASSET_PSX_DIR);
    size_t n = 0; uint8_t *b = slurp(p, &n);
    PRUEF(b && n == RE2_ELZA_VBD_OFF + RE2_ELZA_VBD_SIZE, "ELZASE.VBS %u B", (unsigned)n);
    if (!b) return;
    uint32_t vh = (uint32_t)b[RE2_ELZA_EDT_SIZE - 8] | ((uint32_t)b[RE2_ELZA_EDT_SIZE - 7] << 8);
    re15_vab_t vab; int rc = re15_vab_parse(b + vh, RE2_ELZA_EDT_SIZE - vh, &vab);
    PRUEF(rc == 0 && vab.vag_count == 2 && vab.samples[0].size == 5264 && vab.samples[1].size == 4736,
          "VH @0x%X: %d VAGs, %u / %u B (ROOM1140 Satz 0x25 / ROOM40A0 Satz 0x08)", (unsigned)vh, vab.vag_count,
          (unsigned)vab.samples[0].size, (unsigned)vab.samples[1].size);
    re15_edt_rec_t e0, e1;
    PRUEF(re15_edt_decode(b, RE2_ELZA_SE_AUF, &e0) == 0 && re15_edt_decode(b, RE2_ELZA_SE_SCHUSS, &e1) == 0 &&
          !e0.empty && !e1.empty && e0.tone == 0 && e1.tone == 1,
          "EDT: Satz 0 -> Ton 0 (Stimme %d), Satz 1 -> Ton 1 (Stimme %d, < 0 = Direkt-Zweig)", e0.voice, e1.voice);
    /* RE2-Quelle bytegleich: die VAG-Daten */
    {
        char q[600]; size_t n1 = 0, n2 = 0;
        snprintf(q, sizeof q, "%s/../../../info/re2leon/PL0/RDT/ROOM1140.RDT", RE15_ASSET_PSX_DIR); uint8_t *a = slurp(q, &n1);
        snprintf(q, sizeof q, "%s/../../../info/re2leon/PL0/RDT/ROOM40A0.RDT", RE15_ASSET_PSX_DIR); uint8_t *c = slurp(q, &n2);
        if (a && c) {
            PRUEF(!memcmp(b + RE2_ELZA_VBD_OFF + vab.samples[0].offset, a + 0x04480, 5264) &&
                  !memcmp(b + RE2_ELZA_VBD_OFF + vab.samples[1].offset, c + 0x17B80, 4736),
                  "VAG-Daten bytegleich ROOM1140 @0x04480 / ROOM40A0 @0x17B80");
            PRUEF(a[0x01484 + 3] == b[3] && c[0x09E00 + 3] == b[7] && (a[0x01484 + 2] & 0x0F) == (b[2] & 0x0F) &&
                  (c[0x09E00 + 2] & 0x0F) == (b[6] & 0x0F),
                  "EDT-Stimmen-/Prioritaetsbyte unveraendert (0x%02X / 0x%02X)", b[3], b[7]);
        } else printf("  (info/re2leon fehlt — Quellvergleich uebersprungen)\n");
        free(a); free(c);
    }
    free(b);
}

/* ---- Signatur ---------------------------------------------------------------------------------------------- */
static void teil_signatur(void)
{
    printf("== signatur ==\n");
    if (rdt_laden(0x1021) != 0) { g_fail++; return; }
    uint8_t *kopie = (uint8_t *)malloc(s_rawsz);
    memcpy(kopie, s_raw, s_rawsz);
    re15_elza_r36_raum_scan(kopie, (int)s_rawsz, 0x1021);
    PRUEF(g_re15_elza_anker_n == 3, "ROOM1021 Original: 3 Anker");
    kopie[0x020D0] ^= 0xFF;                                  /* Sleep-Operand hinter @0x020CE verfaelscht */
    re15_elza_r36_raum_scan(kopie, (int)s_rawsz, 0x1021);
    PRUEF(g_re15_elza_anker_n == 2, "eine Signatur verfaelscht -> nur 2 Anker (%d)", g_re15_elza_anker_n);
    re15_elza_r36_raum_scan(s_raw, (int)s_rawsz, 0x1020);
    PRUEF(g_re15_elza_anker_n == 0, "Leon-Raumnummer: kein Anker");
    free(kopie);
    if (rdt_laden(0x1020) == 0) {
        re15_elza_r36_raum_scan(s_raw, (int)s_rawsz, 0x1020);
        PRUEF(g_re15_elza_anker_n == 0, "ROOM1020 (Leon): kein Anker");
    }
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    re15_game_state_init();
    if (!strcmp(teil, "fenster")  || !strcmp(teil, "alle")) teil_fenster();
    if (!strcmp(teil, "irons")    || !strcmp(teil, "alle")) teil_irons();
    if (!strcmp(teil, "unlock")   || !strcmp(teil, "alle")) teil_unlock();
    if (!strcmp(teil, "schuss")   || !strcmp(teil, "alle")) teil_schuss();
    if (!strcmp(teil, "roy")      || !strcmp(teil, "alle")) teil_roy();
    if (!strcmp(teil, "stoehnen") || !strcmp(teil, "alle")) teil_stoehnen();
    if (!strcmp(teil, "bank")     || !strcmp(teil, "alle")) teil_bank();
    if (!strcmp(teil, "signatur") || !strcmp(teil, "alle")) teil_signatur();
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
