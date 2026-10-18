/* test_r39_toene.c — RIEGEL Spur B (Runde 39) "toene": misst die MECHANIK jedes Nutzer-Punkts am echten VM.
 *
 * Dossier: analysis/befunde_runde39/B_toene.md. Konstanten/Belege: include/re15_toene_r39.h, engine/src/toene_r39.c,
 * tools/r39_b/toene39_se_cut.py (TOENE39.VBS, gen/re2_toene39_bank.inc).
 * Verfahren wie test_r38_elzatoene.c: ausgelieferte RDT laden, Raum ueber scd_room_reenter aufbauen (dieselben Haken wie
 * im Spiel: Anker-Scan in scd_register_current_rdt, Opcode-Haken in scd_vm_tick), Bilder in main.c-Reihenfolge fahren.
 * Der Plattform-Haken re15_toene39_se_hook wird durch einen Spion ersetzt, der Satz + Bild protokolliert.
 *
 * Teile (je ein ctest-Eintrag):
 *   schublade  P1 ROOM11F0 sub19 (Schreibtisch-AOT 13 -> Evt_exec sub19): genau ein Schubladen-Ton (Satz 0) im Bild
 *              des Schnitts auf Cut 11 (@0x017B6 Cut_chg 11), beim ersten UND beim zweiten Oeffnen (RE1.5 zeigt die
 *              Schublade erneut), Gegenprobe ohne Anker stumm; ROOM11F1 (Elza) traegt denselben Anker
 *   gitter     P2 ROOM1190 sub10 ueber den ECHTEN Ausloeser (sub01 @0x02468: (4,0xF3)=1, (3,0x6F)=0, Bank-5-Bits
 *              4,5 = 1, 6,7 = 0 -> Evt_exec sub10): je Hund (3x Gosub sub11) ein Gitter-auf-Ton (Satz 1) im Bild
 *              des ersten Aufschwung-Schritts der Luke obj 4 (@0x0286A For) und ein Gitter-zu-Ton (Satz 2) im Bild
 *              der Ruhelage (@0x0288C Member_set(5,0)), 20 Bilder spaeter (10 + 10 Evt_next)
 *   schrank    P3 ROOM1190 sub10: genau ein Regal-Ton (Satz 3) im Bild des ersten Fahrschritts des Schranks obj 5
 *              (@0x02796 Work_set(3,5) -> @0x0279E For 20 {Add_speed, 2x Evt_next}), 20 Bilder nach Cut 8, Fahrt
 *              3920 -> 1920, Cut 11 danach
 *   bank       TOENE39.VBS: vier Saetze, Wellen bytegleich zu den Quellen (RE1.5 ROOM2060 snd0 0x0E/0x0D; RE2 ROOM1130
 *              0x28, ROOM1120 0x0C), EDT-Prioritaet/-Stimme und Ton-Parameter wie die Quelle
 *   signatur   Anker nur bei passender Signatur und nur in 11F0/11F1/1190/1191; 10F0/10F1/1090/1091 ohne Anker */
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
#include "re15_toene_r39.h"
#include "../../engine/src/gen/re2_toene39_bank.inc"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void re15_actor_step_all_walkers(void);

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

/* Spione: Plattform-Ton (re15_toene39_se_hook), erstes Bild je Kamera, Zustand der Requisiten je Bild. */
#define MAXB 2000
static int s_ton_n = 0, s_ton[32], s_ton_bild[32];
static void ton_spion(int ton) { if (s_ton_n < 32) { s_ton[s_ton_n] = ton; s_ton_bild[s_ton_n] = s_bild; s_ton_n++; } }
static int     s_cut_erst[32];                  /* erstes Bild, in dem g_scd.cam_id == cut war (-1 = nie) */
static int32_t s_schrank_x[MAXB];               /* props[5].x nach jedem Bild (ROOM1190: Schrank obj 5)   */
static int16_t s_luke_rz[MAXB];                 /* props[4].rot_z nach jedem Bild (ROOM1190: Luke obj 4)  */
static int     s_ea_erst = -1;                  /* erstes Bild mit (4,0xEA) == 1 (sub10 @0x0276A Set)     */

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
static uint8_t  s_bank_buf[2][0x60000];
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
    if (g_scd.cam_id < 32 && s_cut_erst[g_scd.cam_id] < 0) s_cut_erst[g_scd.cam_id] = s_bild;
    if (s_bild < MAXB) {
        s_schrank_x[s_bild] = (g_scd.prop_count > 5) ? g_scd.props[5].x : 0;
        s_luke_rz[s_bild]   = (g_scd.prop_count > 4) ? g_scd.props[4].rot_z : 0;
    }
    if (s_ea_erst < 0 && re15_game_flag_get(4, 0xEA)) s_ea_erst = s_bild;
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
    bank_laden(0x20, 0);                          /* ROOM1190 sub13: drei Hunde (Sce_em_set 0x20) im Schacht */
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
    re15_toene_r39_reset();
    s_ton_n = 0; s_bild = 0; s_ea_erst = -1;
    for (int i = 0; i < 32; i++) s_cut_erst[i] = -1;
    re15_toene39_se_hook = ton_spion;
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

static int zaehle_ton(int ton, int *erst, int *letzt)
{
    int n = 0; if (erst) *erst = -1; if (letzt) *letzt = -1;
    for (int i = 0; i < s_ton_n; i++) if (s_ton[i] == ton) { if (n == 0 && erst) *erst = s_ton_bild[i]; if (letzt) *letzt = s_ton_bild[i]; n++; }
    return n;
}

/* ---- P1 ROOM11F0 Schublade ------------------------------------------------------------------------------------- */
static int s_ohne_anker = 0;
static int schublade_oeffnen(uint16_t room, int zweites_mal)
{
    /* Spieler am Schreibtisch (AOT 13 main00 @0x00CE6: Rechteck (10200,-15950) 1500 x 1000). Der Schreibtisch feuert ueber
     * dieselbe Stelle wie der Aktionsdruck: re15_aot_fire_slot -> handler[3] @0x800430F0 -> Evt_exec sub19. */
    if (room_boot(room, 10950, -14600, 1024, 0) != 0) return -1;
    if (s_ohne_anker) g_re15_toene39_anker_n = 0;
    if (zweites_mal) re15_game_flag_set(9, 99, 1);     /* Patronen schon genommen (Item_aot_set @0x00CFA Bit (9,99)) */
    for (int f = 0; f < 3; f++) frame();
    int cut_vorher = (int)g_scd.cam_id;
    re15_aot_fire_slot(13);
    for (int f = 0; f < 120 && (s_cut_erst[11] < 0 || s_bild < s_cut_erst[11] + 60); f++) frame();
    return cut_vorher;
}

static void teil_schublade(void)
{
    printf("== schublade ==\n");
    re15_game_state_init();                       /* erster Besuch: (9,99) == 0 */
    int cv = schublade_oeffnen(0x11F0, 0);
    if (cv < 0) return;
    PRUEF(g_re15_toene39_anker_n == 1 && re15_toene_r39_anker_off(0) == RE15_TOENE39_SCHUBLADE_OFF,
          "ROOM11F0: ein Anker @0x%05X (Cut_chg 11)", re15_toene_r39_anker_off(0));
    PRUEF(s_cut_erst[11] >= 0 && cv != 11, "sub19 lief: Schnitt auf Cut 11 (die offene Schublade) in Bild %d (vorher Cut %d)",
          s_cut_erst[11], cv);
    int erst = -1;
    int n0 = zaehle_ton(RE15_TOENE39_TON_SCHUBLADE, &erst, NULL);
    PRUEF(n0 == 1 && s_ton_n == 1, "genau ein Schubladen-Ton (Satz %d), keine anderen (%d/%d)", RE15_TOENE39_TON_SCHUBLADE, n0, s_ton_n);
    PRUEF(erst == s_cut_erst[11], "Ton im Bild des Schnitts (Ton-Bild %d, Cut-11-Bild %d) wie RE2 @0x8005218c/@0x800521d8",
          erst, s_cut_erst[11]);
    PRUEF(re15_toene_r39_zaehler(RE15_TOENE39_TON_SCHUBLADE) == 1, "Anker einmal ausgefuehrt");
    PRUEF(re15_game_flag_get(9, 99) == 0, "Patronen (9,99) noch nicht genommen (Aot_on 14 nur armiert)");
    /* Zweites Oeffnen (Patronen genommen): RE1.5 zeigt die Schublade erneut (sub19 ohne Aot_on 14) -> Ton erneut. */
    cv = schublade_oeffnen(0x11F0, 1);
    if (cv < 0) return;
    n0 = zaehle_ton(RE15_TOENE39_TON_SCHUBLADE, &erst, NULL);
    PRUEF(s_cut_erst[11] >= 0 && n0 == 1 && erst == s_cut_erst[11],
          "zweites Oeffnen ((9,99)=1): Cut 11 in Bild %d, Ton %dx in Bild %d (benannte Abweichung von RE2, Dossier 2.1)",
          s_cut_erst[11], n0, erst);
    /* GEGENPROBE (Stand vor Runde 39 = Original): ohne Anker derselbe Schnitt, kein Ton. */
    re15_game_state_init();
    s_ohne_anker = 1; cv = schublade_oeffnen(0x11F0, 0); s_ohne_anker = 0;
    if (cv < 0) return;
    PRUEF(s_cut_erst[11] >= 0 && s_ton_n == 0, "Gegenprobe ohne Anker: Cut 11 in Bild %d, %d Toene", s_cut_erst[11], s_ton_n);
    /* Elza: ROOM11F1 ist bytegleich (cmp) -> derselbe Anker, dieselbe Mechanik. */
    re15_game_state_init();
    cv = schublade_oeffnen(0x11F1, 0);
    if (cv < 0) return;
    n0 = zaehle_ton(RE15_TOENE39_TON_SCHUBLADE, &erst, NULL);
    PRUEF(g_re15_toene39_anker_n == 1 && n0 == 1 && s_ton_n == 1 && erst == s_cut_erst[11] && s_cut_erst[11] >= 0,
          "ROOM11F1 (Elza): Anker %d, Ton %dx im Cut-11-Bild %d", g_re15_toene39_anker_n, n0, s_cut_erst[11]);
}

/* ---- P2/P3 ROOM1190 Raetsel-Szene ------------------------------------------------------------------------------ */
static void szene_1190(uint16_t room)
{
    re15_game_state_init();
    re15_game_flag_set(4, 0xF3, 1);               /* Strom (sub01 @0x0243C Ck(4,0xF3)=1); (3,0x6F) = 0 */
    /* Ankunft wie ueber die Tuer aus ROOM1180 (Door_aot_set slot 2 @0x00A54: (3174,-11182) Blick 0x400, Cut 0 -> sub13). */
    if (room_boot(room, 3174, -11182, 1024, 0) != 0) return;
    if (s_ohne_anker) g_re15_toene39_anker_n = 0;
    for (int f = 0; f < 3; f++) frame();
    /* Die vier Zielscheiben-Schalter: Bank 5 ist RAUMLOKAL (beim Raumaufbau geloescht), darum erst jetzt.
     * sub01 @0x0244C..@0x02464: (5,4)=1, (5,5)=1, (5,6)=0, (5,7)=0 -> @0x02468 Evt_exec sub10. */
    re15_game_flag_set(5, 4, 1); re15_game_flag_set(5, 5, 1);
    for (int f = 0; f < 700 && !(re15_toene_r39_zaehler(RE15_TOENE39_TON_GITTER_ZU) >= 3 && s_bild > s_ton_bild[s_ton_n - 1] + 40); f++) frame();
}

/* Bild, ab dem props[4].rot_z (Luke) im Bild b ungleich 0 ist, beginnend bei `ab` */
static int luke_bewegt_ab(int ab)   { for (int b = ab; b < s_bild && b < MAXB; b++) if (s_luke_rz[b] != 0) return b; return -1; }
static int luke_ruht_ab(int ab)     { for (int b = ab; b < s_bild && b < MAXB; b++) if (s_luke_rz[b] == 0) return b; return -1; }

static void teil_gitter(void)
{
    printf("== gitter ==\n");
    szene_1190(0x1190);
    PRUEF(g_re15_toene39_anker_n == 3, "ROOM1190: drei Anker (Schrank, Luke auf, Luke zu): %d", g_re15_toene39_anker_n);
    PRUEF(s_ea_erst >= 3, "sub10 ueber sub01 @0x02468 gestartet (Bits nach Bild 2 gesetzt): (4,0xEA) ab Bild %d", s_ea_erst);
    PRUEF(s_cut_erst[11] > s_cut_erst[8] && s_cut_erst[8] > 0, "Cut 8 (Bild %d), dann Cut 11 (Bild %d)", s_cut_erst[8], s_cut_erst[11]);
    int auf[3] = { -1, -1, -1 }, zu[3] = { -1, -1, -1 }, na = 0, nz = 0;
    for (int i = 0; i < s_ton_n; i++) {
        if (s_ton[i] == RE15_TOENE39_TON_GITTER_AUF && na < 3) auf[na++] = s_ton_bild[i];
        if (s_ton[i] == RE15_TOENE39_TON_GITTER_ZU  && nz < 3) zu[nz++]  = s_ton_bild[i];
    }
    PRUEF(zaehle_ton(RE15_TOENE39_TON_GITTER_AUF, NULL, NULL) == 3 && zaehle_ton(RE15_TOENE39_TON_GITTER_ZU, NULL, NULL) == 3,
          "je Hund ein Auf- und ein Zu-Ton: auf %dx, zu %dx", zaehle_ton(RE15_TOENE39_TON_GITTER_AUF, NULL, NULL),
          zaehle_ton(RE15_TOENE39_TON_GITTER_ZU, NULL, NULL));
    PRUEF(na == 3 && auf[0] > s_cut_erst[11], "erster Aufschwung (Bild %d) nach Cut 11 (Bild %d)", auf[0], s_cut_erst[11]);
    int such = 0;
    for (int k = 0; k < 3 && na == 3 && nz == 3; k++) {
        int b0 = luke_bewegt_ab(such);               /* erster Schritt des Aufschwungs (Add_speed im For-Bild) */
        int b1 = (b0 >= 0) ? luke_ruht_ab(b0) : -1;  /* zurueck in der Ruhelage (Summe +1230 / -1230)         */
        PRUEF(b0 == auf[k], "Hund %d: Auf-Ton im Bild des ersten Luken-Schritts (Ton %d, rot_z != 0 ab %d)", k, auf[k], b0);
        /* 10 Aufschwung-Bilder (For @0x0286A, Evt_next @0x02872) + Evt_next @0x0287E zwischen den Schleifen + 10 Rueckschwung-
         * Bilder (For @0x02880, Evt_next @0x02888) -> Member_set(5,0) @0x0288C im 21. Bild nach dem For (gemessen 155-134). */
        PRUEF(zu[k] == auf[k] + 21, "Hund %d: Zu-Ton 21 Bilder spaeter (10 + Evt_next @0x0287E + 10): %d", k, zu[k] - auf[k]);
        PRUEF(b1 >= 0 && b1 <= zu[k] && (zu[k] < MAXB && s_luke_rz[zu[k]] == 0) && s_luke_rz[zu[k] - 2] != 0,
              "Hund %d: Luke ruht im Zu-Bild (rot_z 0 ab %d, zwei Bilder davor %d)", k, b1, (int)s_luke_rz[zu[k] - 2]);
        if (k < 2) PRUEF(zu[k] < auf[k + 1], "Hund %d: Zu-Ton (%d) vor dem naechsten Aufschwung (%d)", k, zu[k], auf[k + 1]);
        such = (b1 >= 0) ? b1 + 1 : s_bild;
    }
    PRUEF(re15_toene_r39_zaehler(RE15_TOENE39_TON_GITTER_AUF) == 3 && re15_toene_r39_zaehler(RE15_TOENE39_TON_GITTER_ZU) == 3,
          "Anker-Zaehler auf/zu 3/3");
    /* GEGENPROBE (Original/Stand vorher): ohne Anker dieselbe Szene, kein Ton. */
    s_ohne_anker = 1; szene_1190(0x1190); s_ohne_anker = 0;
    PRUEF(s_cut_erst[11] > 0 && s_ton_n == 0 && luke_bewegt_ab(0) > s_cut_erst[11],
          "Gegenprobe ohne Anker: Cut 11 Bild %d, Luke bewegt ab %d, %d Toene", s_cut_erst[11], luke_bewegt_ab(0), s_ton_n);
    /* Elza: ROOM1191 traegt sub10/sub11 bytegleich -> dieselben Anker und Takte. */
    szene_1190(0x1191);
    PRUEF(g_re15_toene39_anker_n == 3 && zaehle_ton(RE15_TOENE39_TON_GITTER_AUF, NULL, NULL) == 3 &&
          zaehle_ton(RE15_TOENE39_TON_GITTER_ZU, NULL, NULL) == 3 && luke_bewegt_ab(0) > 0,
          "ROOM1191 (Elza): Anker %d, auf/zu %d/%d, Luke bewegt ab %d", g_re15_toene39_anker_n,
          zaehle_ton(RE15_TOENE39_TON_GITTER_AUF, NULL, NULL), zaehle_ton(RE15_TOENE39_TON_GITTER_ZU, NULL, NULL), luke_bewegt_ab(0));
}

static void teil_schrank(void)
{
    printf("== schrank ==\n");
    szene_1190(0x1190);
    int erst = -1;
    int n3 = zaehle_ton(RE15_TOENE39_TON_REGAL, &erst, NULL);
    PRUEF(n3 == 1, "genau ein Regal-Ton (Satz %d) in der Szene: %d (RE2 ROOM1120 sub04: EIN Se_on je Fahrt)", RE15_TOENE39_TON_REGAL, n3);
    int x0 = -1, xend = -1;
    for (int b = 1; b < s_bild && b < MAXB; b++) {
        if (x0 < 0 && s_schrank_x[b] != s_schrank_x[b - 1]) x0 = b;
        if (x0 >= 0 && s_schrank_x[b] != s_schrank_x[b - 1]) xend = b;
    }
    PRUEF(x0 > 0 && s_schrank_x[x0 - 1] == 3920 && s_schrank_x[x0] == 3820,
          "Schrank obj 5 faehrt ab Bild %d: %d -> %d (Speed_set(0,-100) @0x0279A)", x0, x0 > 0 ? s_schrank_x[x0 - 1] : 0, x0 > 0 ? s_schrank_x[x0] : 0);
    PRUEF(erst == x0, "Ton im Bild des ersten Fahrschritts (Ton %d, Fahrt ab %d) wie RE2 @0x02F2E vor Gosub sub08", erst, x0);
    PRUEF(xend > 0 && s_schrank_x[xend] == 1920 && xend - x0 == 38,
          "Endlage 1920 nach 20 Schritten je 2 Evt_next (letzter Schritt Bild %d = +%d)", xend, xend - x0);
    PRUEF(s_cut_erst[8] > 0 && erst - s_cut_erst[8] == 20, "Ton 20 Bilder nach Cut 8 (Sleep 20 @0x02792): %d", erst - s_cut_erst[8]);
    PRUEF(s_cut_erst[11] > erst && s_cut_erst[11] - erst == 60, "Cut 11 60 Bilder nach dem Ton (40 Fahrt + Sleep 20 @0x027DC): %d",
          s_cut_erst[11] - erst);
    PRUEF(re15_toene_r39_zaehler(RE15_TOENE39_TON_REGAL) == 1, "Anker-Zaehler Regal 1");
    /* Elza ROOM1191: derselbe Takt. */
    szene_1190(0x1191);
    n3 = zaehle_ton(RE15_TOENE39_TON_REGAL, &erst, NULL);
    x0 = -1; for (int b = 1; b < s_bild && b < MAXB && x0 < 0; b++) if (s_schrank_x[b] != s_schrank_x[b - 1]) x0 = b;
    PRUEF(n3 == 1 && erst == x0 && x0 > 0, "ROOM1191 (Elza): Regal-Ton %dx im Bild des ersten Fahrschritts (%d/%d)", n3, erst, x0);
}

/* ---- Bank ------------------------------------------------------------------------------------------------------- */
static uint8_t *re2_rdt(const char *raum, size_t *n)
{
    char q[600]; snprintf(q, sizeof q, "%s/../../../info/re2leon/PL0/RDT/ROOM%s.RDT", RE15_ASSET_PSX_DIR, raum);
    return slurp(q, n);
}
static uint8_t *re15_rdt_datei(const char *stage_raum, size_t *n)
{
    char q[600]; snprintf(q, sizeof q, "%s/%s.RDT", RE15_ASSET_PSX_DIR, stage_raum);
    return slurp(q, n);
}

static void bank_satz(const uint8_t *b, const re15_vab_t *vab, int satz, const char *name, const uint8_t *quelle, size_t qn,
                      unsigned vag_off, unsigned vag_len, unsigned ton_off, unsigned edt_off)
{
    int vags[8], tones[8];
    int nl = re15_edt_resolve_layers_ex(b + RE2_TOENE39_EDT_OFF, vab, satz, vags, tones, 8);
    PRUEF(nl == 1 && vags[0] >= 0 && vags[0] < vab->vag_count, "Satz %d %s: eine Lage, VAG %d", satz, name, nl == 1 ? vags[0] : -1);
    if (nl != 1) return;
    const re15_vab_sample_t *s = &vab->samples[vags[0]];
    PRUEF(s->size == vag_len, "Satz %d: VAG %u B (Quelle %u B)", satz, (unsigned)s->size, vag_len);
    re15_edt_rec_t e;
    PRUEF(re15_edt_decode(b + RE2_TOENE39_EDT_OFF, satz, &e) == 0 && !e.empty, "Satz %d: EDT-Record vorhanden (Ton %d, Prio %d, Stimme %d)",
          satz, e.tone, e.prio, e.voice);
    if (!quelle) { printf("  (Quelle %s nicht im Baum - Byte-Vergleich uebersprungen)\n", name); return; }
    PRUEF(qn >= vag_off + vag_len && !memcmp(b + RE2_TOENE39_VBD_OFF + s->offset, quelle + vag_off, vag_len),
          "Satz %d: Welle bytegleich Quelle @0x%05X (%u B)", satz, vag_off, vag_len);
    const re15_vab_tone_t *t = &vab->tones[tones[0]];
    PRUEF(qn >= ton_off + 8 && t->vol == quelle[ton_off + 2] && t->pan == quelle[ton_off + 3] && t->center_note == quelle[ton_off + 4] &&
          t->pitch_shift == quelle[ton_off + 5] && t->min_note == quelle[ton_off + 6] && t->max_note == quelle[ton_off + 7],
          "Satz %d: Ton wie Quelle @0x%05X: vol %u pan %u center %u shift %u min %u max %u", satz, ton_off, t->vol, t->pan,
          t->center_note, t->pitch_shift, t->min_note, t->max_note);
    /* EDT: Byte 2 unteres Nibble (Prio) und Byte 3 (Stimme/Zweig) wie die Quelle; nur Programm (Byte 1) und Ton-Index
     * (Byte 2 oberes Nibble) sind auf die Mini-Bank umgesetzt (toene39_se_cut.py). */
    const uint8_t *qe = quelle + edt_off;
    const uint8_t *me = b + RE2_TOENE39_EDT_OFF + satz * 4;
    PRUEF(qn >= edt_off + 4 && (qe[2] & 0x0f) == (me[2] & 0x0f) && qe[3] == me[3] && (me[2] >> 4) == satz && me[1] == 0,
          "Satz %d: EDT %02x %02x %02x %02x (Quelle @0x%05X %02x %02x %02x %02x)", satz, me[0], me[1], me[2], me[3], edt_off,
          qe[0], qe[1], qe[2], qe[3]);
}

static void teil_bank(void)
{
    printf("== bank ==\n");
    char p[600]; snprintf(p, sizeof p, "%s/../RE2/TOENE39.VBS", RE15_ASSET_PSX_DIR);
    size_t n = 0; uint8_t *b = slurp(p, &n);
    PRUEF(b && n == RE2_TOENE39_VBD_OFF + RE2_TOENE39_VBD_SIZE && n == RE2_TOENE39_GROESSE, "TOENE39.VBS %u B", (unsigned)n);
    if (!b) return;
    uint32_t vh = (uint32_t)b[RE2_TOENE39_EDT_SIZE - 8] | ((uint32_t)b[RE2_TOENE39_EDT_SIZE - 7] << 8);
    re15_vab_t vab; int rc = re15_vab_parse(b + vh, RE2_TOENE39_EDT_SIZE - vh, &vab);
    PRUEF(rc == 0 && vab.vag_count == RE2_TOENE39_SE_COUNT, "VH @%u: %d VAGs (Saetze %d)", vh, rc == 0 ? vab.vag_count : -1, RE2_TOENE39_SE_COUNT);
    if (rc != 0) { free(b); return; }
    size_t n2060 = 0, n1130 = 0, n1120 = 0;
    uint8_t *r2060 = re15_rdt_datei("STAGE2/ROOM2060", &n2060);
    uint8_t *r1130 = re2_rdt("1130", &n1130), *r1120 = re2_rdt("1120", &n1120);
    PRUEF(r2060 != NULL, "RE1.5 STAGE2/ROOM2060.RDT geladen (%u B)", (unsigned)n2060);
    bank_satz(b, &vab, RE2_TOENE39_SE_SCHUBLADE,  "SCHUBLADE (RE2 ROOM1130 0x28)",  r1130, n1130, RE2_TOENE39_Q_SCHUBLADE_VAG_OFF,
              RE2_TOENE39_Q_SCHUBLADE_VAG_LEN, RE2_TOENE39_Q_SCHUBLADE_TON_OFF, RE2_TOENE39_Q_SCHUBLADE_EDT_OFF);
    bank_satz(b, &vab, RE2_TOENE39_SE_GITTER_AUF, "GITTER_AUF (RE1.5 ROOM2060 0x0E)", r2060, n2060, RE2_TOENE39_Q_GITTER_AUF_VAG_OFF,
              RE2_TOENE39_Q_GITTER_AUF_VAG_LEN, RE2_TOENE39_Q_GITTER_AUF_TON_OFF, RE2_TOENE39_Q_GITTER_AUF_EDT_OFF);
    bank_satz(b, &vab, RE2_TOENE39_SE_GITTER_ZU,  "GITTER_ZU (RE1.5 ROOM2060 0x0D)",  r2060, n2060, RE2_TOENE39_Q_GITTER_ZU_VAG_OFF,
              RE2_TOENE39_Q_GITTER_ZU_VAG_LEN, RE2_TOENE39_Q_GITTER_ZU_TON_OFF, RE2_TOENE39_Q_GITTER_ZU_EDT_OFF);
    bank_satz(b, &vab, RE2_TOENE39_SE_REGAL,      "REGAL (RE2 ROOM1120 0x0C)",      r1120, n1120, RE2_TOENE39_Q_REGAL_VAG_OFF,
              RE2_TOENE39_Q_REGAL_VAG_LEN, RE2_TOENE39_Q_REGAL_TON_OFF, RE2_TOENE39_Q_REGAL_EDT_OFF);
    /* Die Gitter-Quelle ist die Se_on-Stelle in ROOM2060 sub02: @0x0134A Se_on(2,0x0E) vor dem Aufklappen, @0x01376 Se_on(2,0x0D). */
    if (r2060 && n2060 > 0x01380) {
        static const uint8_t se_auf[4] = { 0x36, 0x02, 0x0e, 0x00 }, se_zu[4] = { 0x36, 0x02, 0x0d, 0x00 };
        PRUEF(!memcmp(r2060 + 0x0134A, se_auf, 4) && !memcmp(r2060 + 0x01376, se_zu, 4),
              "ROOM2060 sub02: @0x0134A Se_on(2,0x0E) (Aufklappen), @0x01376 Se_on(2,0x0D) (Ruhelage) - die Quelle der Form");
    }
    /* Die Schubladen-Quelle: RE2 ROOM1130 Aot_set sce 13 (Schublade) @0x00D50; die Regal-Quelle: RE2 ROOM1120 sub04 @0x02F2E Se_on(2,0x0C). */
    if (r1130 && n1130 > 0x00D60) PRUEF(r1130[0x00D50] == 0x2c && r1130[0x00D52] == 0x0d, "RE2 ROOM1130 @0x00D50: Aot_set sce 13 = Schublade");
    if (r1120 && n1120 > 0x02F40) PRUEF(r1120[0x02F2E] == 0x36 && r1120[0x02F2F] == 0x02 && r1120[0x02F30] == 0x0c,
                                       "RE2 ROOM1120 @0x02F2E: Se_on(2,0x0C) = Regalfahrt");
    free(r2060); free(r1130); free(r1120); free(b);
}

/* ---- Signatur ------------------------------------------------------------------------------------------------- */
static void teil_signatur(void)
{
    printf("== signatur ==\n");
    static const struct { uint16_t raum; int n; } erw[] = {
        { 0x10F0, 0 }, { 0x10F1, 0 }, { 0x1090, 0 }, { 0x1091, 0 }, { 0x11F0, 1 }, { 0x11F1, 1 }, { 0x1190, 3 }, { 0x1191, 3 },
    };
    for (unsigned i = 0; i < sizeof erw / sizeof erw[0]; i++) {
        if (rdt_laden(erw[i].raum) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", erw[i].raum); g_fail++; continue; }
        re15_toene_r39_raum_scan(s_raw, (int)s_rawsz, erw[i].raum);
        PRUEF(g_re15_toene39_anker_n == erw[i].n, "ROOM%04X: %d Anker (soll %d)", erw[i].raum, g_re15_toene39_anker_n, erw[i].n);
    }
    if (rdt_laden(0x1190) == 0) {
        static const uint8_t sig_schrank[8] = { 0x2e,0x03,0x05,0x00, 0x2f,0x00,0x9c,0xff };
        static const uint8_t sig_auf[8]     = { 0x0d,0x00,0x06,0x00,0x0a,0x00, 0x30,0x31 };
        static const uint8_t sig_zu[8]      = { 0x34,0x05,0x00,0x00, 0x01,0x00, 0x2b,0x00 };
        PRUEF(!memcmp(s_raw + RE15_TOENE39_SCHRANK_OFF, sig_schrank, 8) && !memcmp(s_raw + RE15_TOENE39_LUKE_AUF_OFF, sig_auf, 8) &&
              !memcmp(s_raw + RE15_TOENE39_LUKE_ZU_OFF, sig_zu, 8), "ROOM1190: Signaturen @0x%05X/@0x%05X/@0x%05X wie ausgeliefert",
              RE15_TOENE39_SCHRANK_OFF, RE15_TOENE39_LUKE_AUF_OFF, RE15_TOENE39_LUKE_ZU_OFF);
        uint8_t *kopie = (uint8_t *)malloc(s_rawsz);
        if (kopie) {
            memcpy(kopie, s_raw, s_rawsz);
            kopie[RE15_TOENE39_SCHRANK_OFF + 2] ^= 0xff;             /* Work_set(3,5) -> anderer Index: Schrank-Anker nicht armiert */
            re15_toene_r39_raum_scan(kopie, (int)s_rawsz, 0x1190);
            PRUEF(g_re15_toene39_anker_n == 2 && re15_toene_r39_anker_off(0) == RE15_TOENE39_LUKE_AUF_OFF,
                  "abweichende Signatur @0x02796: nur 2 Anker (Luke auf/zu), kein Ton am falschen Opcode");
            free(kopie);
        }
        re15_toene_r39_raum_scan(NULL, 0, 0x1190);
        PRUEF(g_re15_toene39_anker_n == 0, "ohne RDT: 0 Anker");
    }
    if (rdt_laden(0x11F0) == 0) {
        static const uint8_t sig[8] = { 0x29,0x0b, 0x22,0x02,0x07,0x01, 0x2e,0x01 };
        PRUEF(!memcmp(s_raw + RE15_TOENE39_SCHUBLADE_OFF, sig, 8), "ROOM11F0 @0x017B6: Cut_chg 11 / Set(2,7,1) / Work_set(1,..) wie ausgeliefert");
        /* AOT 13 (main00 @0x00CE6) zeigt auf sub19 = die Stelle des Ankers (Evt_exec-Ziel aus den AOT-Daten `ff 00 18 13`). */
        PRUEF(s_raw[0x00CE6] == 0x2c && s_raw[0x00CE7] == 13 && s_raw[0x00CE8] == 3 && s_raw[0x00CF7] == 0x13,
              "ROOM11F0 main00 @0x00CE6: Aot_set 13 sce 3 -> sub 0x13 = sub19 (@0x017B6 = erster Opcode von sub19)");
    }
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    int alle = !strcmp(teil, "alle");
    if (alle || !strcmp(teil, "schublade")) teil_schublade();
    if (alle || !strcmp(teil, "gitter"))    teil_gitter();
    if (alle || !strcmp(teil, "schrank"))   teil_schrank();
    if (alle || !strcmp(teil, "bank"))      teil_bank();
    if (alle || !strcmp(teil, "signatur"))  teil_signatur();
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
