/* test_r35_cut10f0.c — RIEGEL: Neue Szene beim ERSTEN Betreten von ROOM10F0 (Ada / Leon / Marvin),
 * danach Kartenhinweis ROOM11C0 -> ROOM1150 und MAIN01 bis zum Parkplatz (Runde 35, Spur K).
 *
 * Dossier analysis/befunde_runde35/K_cut10f0.md, Konstanten und Belege include/re15_cut10f0.h,
 * engine/src/cut_10f0.c, gen/cut10f0_szene.inc (Generator tools/r35_k/szene_bauen.py).
 *
 * Verfahren wie test_r34n_d_adaruf.c: die ausgelieferte RDT wird geladen, der Raum ueber scd_room_reenter
 * aufgebaut (derselbe Einhaengepunkt wie im Spiel; dort feuert re15_cut10f0_install das Ereignis 20),
 * jedes Bild laeuft in der Reihenfolge von platform/pc/main.c (scd_vm_tick -> Walker -> Letterbox ->
 * Szenen-Uebergabe -> re15_msg_tick -> re15_game_step). Gemessen wird die MECHANIK (Werte, Zustaende,
 * Bildfolge), nicht "Funktion existiert".
 *
 * Teile (je ein ctest-Eintrag):
 *   programm  statischer Lauf ueber das Programm (Opcode-Laengen = scd_vm.c s_opcode_sizes)
 *   texte     18 Nachrichten dekodieren zum Nutzer-Wortlaut (Sprecher, Farbe, Dauer)
 *   tuerton   eingebackener Tonteil == shared_assets/RE2/DOOR/DOOR13.DO2[0 .. 0x3DA8)
 *   szene     Szene in der echten VM: Start beim Raumaufbau, Nachrichten 6..23, Ada/Marvin, Kamera,
 *             Tuerknall, Balken, Abgang, Flags, Hinweis, BGM-Weiche
 *   einmal    danach erneuter Raumaufbau: nichts; Elza-Raum ROOM10F1: nie
 *   karte     Hinweiskette K1 -> K2, beide Ziele bis zum Besuch, Runde-33-Eintrag unveraendert
 *   bgm       Tabellenweiche vor/nach der Szene, Raum 0x1C, andere Stage, besuchter Parkplatz
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
#include "re15_map_hint.h"
#include "re15_menu.h"       /* re15_menu_map_hint_active — das Menue nimmt die Anforderung im Spielschritt an */
#include "re15_cut10f0.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void re15_actor_step_all_walkers(void);
extern int g_test_tuer_laden_groesse, g_test_tuer_laden_count;   /* tests/test_support.c Spion */
extern int g_test_bgm_reset_count, g_test_bgm_start_count, g_test_bgm_start_room;   /* dito: Raummusik-Anstoss */

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
    if (s_raw && s_rdt_raum == room) return 0;
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz); s_rdt_raum = 0;
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) return -1;
    s_rdt_raum = room;
    return 0;
}

/* Audio-Ereignisse des Bildes (Se_on der Port-Bank zaehlen). */
static int s_knall = 0;
/* Hinweis-Nummer, die waehrend des Laufs anstand (vor der Annahme durch das Menue). */
static int s_hint_gesehen = -1;

/* Ein Spielbild in der Reihenfolge von platform/pc/main.c. */
static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
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
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_cut10f0_tick();                       /* dieselbe Stelle wie im Spielschritt — die Anforderung
                                                * ist hier noch sichtbar, bevor game_step sie annimmt */
    if (re15_map_hint_pending() >= 0) s_hint_gesehen = re15_map_hint_pending();
    re15_game_step(&s_ctx);
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e)) {
        if (e.kind == SCD_AUDIO_SE_ON && e.bank == RE15_CUT10F0_SE_BANK && e.sample_id == RE15_CUT10F0_SE_TUERKNALL)
            s_knall++;
    }
}

/* Raum aufbauen OHNE die Flags anzufassen (test_r34n_d_adaruf.c room_boot). */
static int room_boot(uint16_t room, int32_t px, int32_t pz, int16_t rot, uint8_t cut)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = cut; s_cine_was = 0; s_knall = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_map_hint_room_scan(s_raw, (int)s_rawsz, room);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

static int faeden_im_programm(void)
{
    int plen = 0; const uint8_t *p = re15_cut10f0_programm(&plen);
    int n = 0;
    for (int s = 0; s < SCD_THREAD_COUNT; s++) {
        const scd_thread_t *t = &g_scd.threads[s];
        if (t->active && t->pc >= p && t->pc < p + plen) n++;
    }
    return n;
}

/* ---- Opcode-Laengen (scd_vm.c s_opcode_sizes; tools/scd_dump_room.py SIZES) ----------------------- */
static int op_len(const uint8_t *pc)
{
    static const uint8_t k[0x5F] = {
        1,2,1,4,4,2,4,4, 2,4,3,1,1,6,2,4, 2,4,2,4,6,4,2,6, 2,2,2,6,1,1,1,1,
        1,4,4,6,4,3,6,4, 1,2,1,4,20,34,3,4, 1,1,8,8,4,3,12,4, 12,4,16,32,2,3,6,4,
        8,10,1,4,20,3,10,2, 16,8,2,3,18,10,5,22, 22,4,4,3,6,6,6,4, 4,4,6,4,4,4,4 };
    uint8_t op = pc[0];
    if (op >= 0x5F) return -1;
    if (op == 0x2C) return (pc[3] & 0x80) ? 28 : 20;     /* Aot_set 20/28 @0x80040590 */
    if (op == 0x3B) return (pc[3] & 0x80) ? 40 : 32;     /* Door_aot_set 32/40 @0x80040618 */
    if (op == 0x50) return (pc[3] & 0x80) ? 30 : 22;     /* Item_aot_set 22/30 @0x8004065c */
    return (int)k[op];
}

static int text_normal(const char *t, char *out, size_t n)
{
    size_t o = 0; int sp = 1;
    for (; *t && o + 1 < n; t++) {
        char c = *t;
        if (c == '\n' || c == '\r' || c == '\t') c = ' ';
        if (c == ' ') { if (sp) continue; sp = 1; } else sp = 0;
        out[o++] = c;
    }
    while (o > 0 && out[o - 1] == ' ') o--;
    out[o] = 0;
    return (int)o;
}

static int text_ist(int id, const char *soll)
{
    const char *t = re15_msg_get_text(id);
    char a[512], b[512];
    text_normal(t ? t : "", a, sizeof a);
    text_normal(soll, b, sizeof b);
    printf("    msg %2d \"%s\"\n", id, t ? t : "(keiner)");
    return t && strcmp(a, b) == 0;
}

/* ================================================================================================ */
static void teil_programm(void)
{
    int plen = 0; const uint8_t *p = re15_cut10f0_programm(&plen);
    printf("  Programm %d Bytes\n", plen);
    int off = 0, n_ops = 0, ok = 1, letzte_msg = -1, msg_n = 0, msgs_steigend = 1, se_ok = 1, ck_ok = 1;
    int ada = 0, marvin = 0, ende = -1, first_ok = (plen >= 4 && p[0] == 0x22 && p[1] == RE15_CUT10F0_GESEHEN_BANK &&
                                                     p[2] == RE15_CUT10F0_GESEHEN_BIT && p[3] == 1);
    int cut_ok = 1, erste_cuts[2] = { -1, -1 }, n_cut = 0, letzter_cut = -1, cut_auto_ende = 0;
    while (off < plen) {
        int l = op_len(p + off);
        if (l <= 0 || off + l > plen) { ok = 0; printf("  Opcode 0x%02X @+%04X unbekannt/ragt heraus\n", p[off], off); break; }
        uint8_t op = p[off];
        if (op == 0x2B) {
            int id = p[off + 1];
            if (id < RE15_CUT10F0_MSG_ERSTE || id > RE15_CUT10F0_MSG_LETZTE || id <= letzte_msg) msgs_steigend = 0;
            if (p[off + 2] != 0 || p[off + 3] != 0) msgs_steigend = 0;   /* Maske 0 = Untertitel */
            letzte_msg = id; msg_n++;
        }
        if (op == 0x36 && !(p[off + 1] == RE15_CUT10F0_SE_BANK && p[off + 2] == RE15_CUT10F0_SE_TUERKNALL)) se_ok = 0;
        if (op == 0x21 && !(p[off + 1] == 5 && p[off + 2] <= RE15_CUT10F0_BIT_MARVIN)) ck_ok = 0;
        if (op == 0x44) {
            if (p[off + 1] == 0 && p[off + 2] == RE15_CUT10F0_TYP_ADA && p[off + 3] == 0x40 && p[off + 7] == 0xFF) ada++;
            if (p[off + 1] == 1 && p[off + 2] == RE15_CUT10F0_TYP_MARVIN && p[off + 3] == 0x40 && p[off + 7] == 0xFF) marvin++;
        }
        if (op == 0x29) { if (n_cut < 2) erste_cuts[n_cut] = p[off + 1]; n_cut++; letzter_cut = p[off + 1]; }
        if (op == 0x3C && p[off + 1] == 1) cut_auto_ende = off;
        if (op == 0x01) { ende = off; off += l; break; }
        off += l; n_ops++;
    }
    PRUEF(ok && ende == plen - 2, "Opcode-Laengen schliessen genau auf das Evt_end am Ende (+%04X, %d Opcodes)", ende, n_ops);
    PRUEF(first_ok, "ERSTES Opcode = Set(9,71)=1 (Einmal-Riegel wie ROOM11B0 sub06 @0x01478)");
    PRUEF(msg_n == 18 && msgs_steigend && letzte_msg == RE15_CUT10F0_MSG_LETZTE,
          "18 Message_on, Ids 6..23 streng aufsteigend, Maske 0 (%d Stueck, letzte %d)", msg_n, letzte_msg);
    PRUEF(se_ok, "jedes Se_on = Port-Bank 0x0E Satz 1 (RE2 DOOR13 Door_exit)");
    PRUEF(ck_ok, "jedes Ck = Bank 5 Bit 0..2 (Ankunftsbits, raumlokal)");
    PRUEF(ada == 1 && marvin == 1, "genau ein Spawn Ada 0x42 (Slot 0) und Marvin 0x40 (Slot 1), grid 0x40, Kill-Flag 0xff");
    PRUEF(erste_cuts[0] == 2 && erste_cuts[1] == 0, "Kamera zuerst Cut 2 (Ada), dann Cut 0 (Leon)");
    PRUEF(letzter_cut == 2 && cut_auto_ende > 0, "letzter Cut_chg = 2 (Leon im Bild), danach Cut_auto 1");
    (void)cut_ok;
}

static void teil_texte(void)
{
    static const struct { int id; const char *soll; } k[] = {
        {  6, "Leon: Hey - how did you came in here?" },
        {  7, "Woman: Did you really think there was only one staff card for the Communication Room?" },
        {  8, "Woman: Anyway... the communication system is completely destroyed." },
        {  9, "Woman: We won't reach anyone with it anymore..." },
        { 10, "Marvin: Leon! You already made it!" },
        { 11, "Leon: Hey Marvin, glad you made it!" },
        { 12, "Leon: Allow me to introduce you. This is..." },
        { 13, "Ada: ... Ada, Ada Wong" },
        { 14, "Leon: Ada Wong." },
        { 15, "Marvin: Hello, glad to meet another Survivor! I'm Marvin." },
        { 16, "Leon: Anyway... looks like we can't contact anyone with this thing anymore." },
        { 17, "Marvin: Ohh... what do we do then?..." },
        { 18, "Leon: ..." },
        { 19, "Leon: I know! The patrol car! We can use it to get out of here!" },
        { 20, "Marvin: Yeah, you're right! That could be our way out!" },
        { 21, "Leon: Okay, Marvin, you go with Ada to the parking lot and wait there." },
        { 22, "Leon: I'm going to get Chief Irons, and I'll be right behind you!" },
        { 23, "Marvin: Alright! Sounds like a plan. Take care Leon!" },
    };
    /* die Nachrichten werden beim Raumaufbau eingesetzt */
    re15_game_state_init();
    if (room_boot(RE15_CUT10F0_RAUM, RE15_CUT10F0_SPAWN_X, RE15_CUT10F0_SPAWN_Z, RE15_CUT10F0_SPAWN_DIR, 0) != 0) return;
    for (unsigned i = 0; i < sizeof k / sizeof k[0]; i++)
        PRUEF(text_ist(k[i].id, k[i].soll), "msg %d woertlich (Nutzervorgabe)", k[i].id);
    /* Form: Kopf 04 00 05 cc <Name> 16 05 00 00, Ende 04 01 01 63; Farben Leon 01 / Woman+Ada 02 / Marvin 07 */
    int form_ok = 1;
    for (int id = RE15_CUT10F0_MSG_ERSTE; id <= RE15_CUT10F0_MSG_LETZTE; id++) {
        int n = 0; const uint8_t *b = re15_cut10f0_meldung(id, &n);
        if (!b || n < 16 || b[0] != 0x04 || b[1] != 0x00 || b[2] != 0x05 ||
            b[n - 4] != 0x04 || b[n - 3] != 0x01 || b[n - 2] != 0x01 || b[n - 1] != 0x63) { form_ok = 0; continue; }
        const char *t = re15_msg_get_text(id);
        int farbe = b[3];
        int soll = (t && !strncmp(t, "Leon", 4)) ? 1 : (t && !strncmp(t, "Marvin", 6)) ? 7 : 2;
        if (farbe != soll) { form_ok = 0; printf("  msg %d Farbe %d statt %d\n", id, farbe, soll); }
        if (re15_msg_compute_duration(b, (size_t)n, 0) <= 0) form_ok = 0;
    }
    PRUEF(form_ok, "Dialogform (Kopf/Ende/Sprecherfarbe 01/02/07) und Dauer > 0 fuer alle 18");
    PRUEF(re15_cut10f0_meldung(5, NULL) == NULL && re15_cut10f0_meldung(24, NULL) == NULL,
          "keine Port-Nachricht ausserhalb 6..23 (RDT-Nachrichten 0..5 bleiben die des Raums)");
}

static void teil_tuerton(void)
{
    char rp[600]; size_t n = 0;
    snprintf(rp, sizeof rp, "%s/DOOR/DOOR13.DO2", RE15_ASSET_RE2_DIR);
    uint8_t *d = slurp(rp, &n);
    int tl = 0; const uint8_t *t = re15_cut10f0_tuerton(&tl);
    PRUEF(d && n == 54532, "shared_assets/RE2/DOOR/DOOR13.DO2 vorhanden (54532 B, Tabelle @0x8009A604)");
    PRUEF(tl == 0x3DA8, "eingebackener Tonteil = 0x3DA8 Bytes (Halbwort 0 der Tabellenzeile DOOR13)");
    PRUEF(d && tl == 0x3DA8 && memcmp(d, t, (size_t)tl) == 0, "Tonteil bytegleich mit dem Dateikopf des RE2-Archivs");
    free(d);
}

/* ---- Die Szene fahren und vermessen -------------------------------------------------------------- */
typedef struct {
    int gestartet, faeden_max;
    int msg_bild[18]; int msg_folge_ok; int fremde_msg;
    int ada_da, ada_x, ada_z, ada_dir;            /* beim ersten Bild nach dem Start */
    int marvin_park;                              /* Marvin zuerst geparkt            */
    int marvin_tuer_bild, marvin_tuer_x, marvin_tuer_z;   /* nach dem ersten Knall an der Tuer */
    int marvin_bei_leon_bild, marvin_end_x, marvin_end_z;
    int leon_los_bild, leon_ankunft_bild, leon_x, leon_z, leon_rot;
    int cuts[16], n_cuts;
    int knall1_bild, knall2_bild;
    int lb_voll, lb_weg, ende, pm_frei, auto_ende;
    int marvin_weg, ada_weg;                      /* am Ende geparkt */
    int hint_nr, bgm_vorher, bgm_nachher, flag_vorher, flag_nachher, f27, f17;
    /* Fortsetzung (Dossier §8.2/§8.3): Leons Lage/Gierung beim Aufgehen jeder Zeile, Marvins Lage dabei */
    int leon_rot_msg[18], leon_x_msg[18], leon_z_msg[18], mv_x_msg[18], mv_z_msg[18];
} szene_t;

/* Gierung (0 = +X, 1024 = -Z, 2048 = -X, 3072 = +Z; actor_locomotion.c: x += cos, z -= sin) von (x,z) nach (tx,tz). */
static int gierung_nach(int x, int z, int tx, int tz)
{
    double a = atan2(-(double)(tz - z), (double)(tx - x)) * 2048.0 / 3.14159265358979323846;
    int y = (int)(a < 0 ? a - 0.5 : a + 0.5);
    return y & 0xfff;
}
static int gier_abstand(int a, int b)
{
    int d = (a - b) & 0xfff;
    return d > 2048 ? 4096 - d : d;
}

#define SZENE_MAX 3600

static szene_t szene_fahren(void)
{
    szene_t r; memset(&r, 0, sizeof r);
    for (int i = 0; i < 18; i++) r.msg_bild[i] = -1;
    r.msg_folge_ok = 1; r.marvin_tuer_bild = r.marvin_bei_leon_bild = r.leon_los_bild = r.leon_ankunft_bild = -1;
    r.knall1_bild = r.knall2_bild = r.lb_voll = r.lb_weg = r.ende = -1; r.hint_nr = -2;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *ada = &g_actors[RE15_CUT10F0_AKTOR_ADA];
    re15_actor_t *mv  = &g_actors[RE15_CUT10F0_AKTOR_MARVIN];
    r.bgm_vorher = re15_cut10f0_bgm_eintrag(0, 0x0F);
    r.flag_vorher = re15_game_flag_get(RE15_CUT10F0_GESEHEN_BANK, RE15_CUT10F0_GESEHEN_BIT);
    r.gestartet = faeden_im_programm();
    int zuletzt = -1, letzter_cut = -1, knall_gesehen = 0, sx = pl->x, sz = pl->z, lb_max = 0;
    for (int f = 1; f <= SZENE_MAX; f++) {
        frame(0, 0);
        int nf = faeden_im_programm();
        if (nf > r.faeden_max) r.faeden_max = nf;
        if (f == 1) {
            r.ada_da = ada->active && ada->type == RE15_CUT10F0_TYP_ADA;
            r.ada_x = ada->x; r.ada_z = ada->z; r.ada_dir = ada->rot_y & 0xfff;
            r.marvin_park = mv->active && mv->type == RE15_CUT10F0_TYP_MARVIN && mv->x <= -20000 && mv->z <= -20000;
        }
        if (g_scd.message_active) {
            int id = (int)g_scd.message_id;
            if (id != zuletzt) {
                int k = id - RE15_CUT10F0_MSG_ERSTE;
                if (k < 0 || k >= 18) r.fremde_msg++;
                else {
                    if (r.msg_bild[k] < 0) {
                        r.msg_bild[k] = f;
                        r.leon_rot_msg[k] = pl->rot_y & 0xfff; r.leon_x_msg[k] = pl->x; r.leon_z_msg[k] = pl->z;
                        r.mv_x_msg[k] = mv->x; r.mv_z_msg[k] = mv->z;
                    }
                    if (k > 0 && r.msg_bild[k - 1] < 0) r.msg_folge_ok = 0;
                }
                zuletzt = id;
            }
        }
        if (s_shown != letzter_cut) { if (r.n_cuts < 16) r.cuts[r.n_cuts] = s_shown; r.n_cuts++; letzter_cut = s_shown; }
        if (s_knall > knall_gesehen) {
            if (s_knall == 1) r.knall1_bild = f; else if (s_knall == 2) r.knall2_bild = f;
            knall_gesehen = s_knall;
        }
        if (r.knall1_bild > 0 && r.marvin_tuer_bild < 0 && mv->x > -20000) {
            r.marvin_tuer_bild = f; r.marvin_tuer_x = mv->x; r.marvin_tuer_z = mv->z;
        }
        if (r.marvin_tuer_bild > 0 && r.marvin_bei_leon_bild < 0 && r.msg_bild[5] > 0 && mv->z > 9000) {
            r.marvin_bei_leon_bild = f; r.marvin_end_x = mv->x; r.marvin_end_z = mv->z;
        }
        if (r.leon_los_bild < 0 && (pl->x != sx || pl->z != sz)) r.leon_los_bild = f;
        /* Ankunft = Leons Lage, wenn Adas erste Zeile (msg 7) aufgeht — der Walker ist dann durch
         * (Warteschleife Ck(5,0)) und die Drehung zu Ada fertig. */
        if (r.leon_los_bild > 0 && r.leon_ankunft_bild < 0 && r.msg_bild[1] > 0) {
            r.leon_ankunft_bild = f; r.leon_x = pl->x; r.leon_z = pl->z;
        }
        if (g_letterbox_level > lb_max) lb_max = g_letterbox_level;
        if (r.lb_voll < 0 && g_letterbox_level >= 0xF0) r.lb_voll = f;
        if (r.ende < 0 && nf == 0 && r.gestartet) {
            r.ende = f;
            r.leon_rot = pl->rot_y & 0xfff;
            r.marvin_weg = (mv->x <= -20000 && mv->z <= -20000);
            r.ada_weg    = (ada->x <= -20000 && ada->z <= -20000);
            r.auto_ende  = g_scd.cut_auto_enabled;
        }
        if (r.ende > 0 && r.lb_weg < 0 && g_letterbox_level == 0) r.lb_weg = f;
        if (r.ende > 0 && f > r.ende + 40) break;
    }
    r.pm_frei = (g_scd.player_mode == 0);
    /* Die Anforderung wird im selben Spielschritt vom Menue angenommen (game_step_common.c:
     * re15_map_hint_pending -> re15_menu_request_map_hint -> re15_map_hint_take); sichtbar bleibt
     * der Menue-Zustand "Hinweis angefordert" (s_hint_target, RE2 Phase 1 @0x800591E8). */
    r.hint_nr = re15_menu_map_hint_active() ? s_hint_gesehen : re15_map_hint_pending();
    r.bgm_nachher = re15_cut10f0_bgm_eintrag(0, 0x0F);
    r.flag_nachher = re15_game_flag_get(RE15_CUT10F0_GESEHEN_BANK, RE15_CUT10F0_GESEHEN_BIT);
    r.f27 = re15_game_flag_get(2, 7); r.f17 = re15_game_flag_get(1, 27);
    return r;
}

static void teil_szene(void)
{
    re15_game_state_init();
    g_test_tuer_laden_count = 0;
    if (room_boot(RE15_CUT10F0_RAUM, RE15_CUT10F0_SPAWN_X, RE15_CUT10F0_SPAWN_Z, RE15_CUT10F0_SPAWN_DIR, 0) != 0) return;
    PRUEF(re15_cut10f0_zustand() == RE15_CUT10F0_LAEUFT, "Raumaufbau feuert Ereignis 20 -> Zustand LAEUFT");
    PRUEF(g_test_tuer_laden_count == 1 && g_test_tuer_laden_groesse == 0x3DA8,
          "RE2-Tuerbank (DOOR13-Tonteil 0x3DA8 B) beim Raumaufbau geladen");
    szene_t r = szene_fahren();
    printf("  Faden %d (max %d) | msg 6..23 ab B%d..B%d | Ada (%d,%d) dir %d | Marvin geparkt %d, an der Tuer B%d "
           "(%d,%d), bei Leon B%d (%d,%d) | Leon los B%d an B%d (%d,%d) rot %d | Knall B%d/B%d | Cuts:",
           r.gestartet, r.faeden_max, r.msg_bild[0], r.msg_bild[17], r.ada_x, r.ada_z, r.ada_dir, r.marvin_park,
           r.marvin_tuer_bild, r.marvin_tuer_x, r.marvin_tuer_z, r.marvin_bei_leon_bild, r.marvin_end_x,
           r.marvin_end_z, r.leon_los_bild, r.leon_ankunft_bild, r.leon_x, r.leon_z, r.leon_rot, r.knall1_bild,
           r.knall2_bild);
    for (int i = 0; i < r.n_cuts && i < 16; i++) printf(" %d", r.cuts[i]);
    printf(" | Ende B%d Balken voll B%d weg B%d\n", r.ende, r.lb_voll, r.lb_weg);
    PRUEF(r.gestartet == 1 && r.faeden_max == 1, "genau EIN Faden mit dem Port-Programm");
    PRUEF(r.flag_vorher == 0 && r.flag_nachher == 1, "(9,71) vor der Szene 0, danach 1 (erstes Opcode)");
    PRUEF(r.ada_da && r.ada_x == RE15_CUT10F0_ADA_X && r.ada_z == RE15_CUT10F0_ADA_Z && r.ada_dir == RE15_CUT10F0_ADA_DIR,
          "Ada (0x42) steht ab Bild 1 an den Monitoren (%d,%d) Blick %d", r.ada_x, r.ada_z, r.ada_dir);
    PRUEF(r.marvin_park, "Marvin (0x40) zuerst geparkt (-30000,-30000) wie ROOM11B0 main00");
    int ok_folge = r.msg_folge_ok && r.fremde_msg == 0;
    for (int i = 0; i < 18; i++) if (r.msg_bild[i] < 0) ok_folge = 0;
    PRUEF(ok_folge, "Nachrichtenfolge 6 -> 7 -> ... -> 23 vollstaendig, keine andere Nachricht");
    PRUEF(r.n_cuts >= 2 && r.cuts[0] == 2 && r.cuts[1] == 0, "Kamera zuerst Cut 2 (Ada), dann Cut 0 (Leon)");
    PRUEF(r.msg_bild[0] > 0 && r.leon_los_bild > r.msg_bild[0], "Leon laeuft erst NACH 'Hey - how did you came in here?' los (B%d > B%d)",
          r.leon_los_bild, r.msg_bild[0]);
    PRUEF(r.leon_ankunft_bild > 0 && r.leon_ankunft_bild <= r.msg_bild[1] &&
          abs(r.leon_x - RE15_CUT10F0_LEON_X) < 200 && abs(r.leon_z - RE15_CUT10F0_LEON_Z) < 300 && r.leon_x < r.ada_x,
          "Leon steht bei 'Did you really think' LINKS von Ada ((%d,%d), Ada x %d; Ziel (%d,%d), Modus-4-Ankunft < 100 "
          "@0x80030af0 bzw. Kollision der Konsole)", r.leon_x, r.leon_z, r.ada_x, RE15_CUT10F0_LEON_X, RE15_CUT10F0_LEON_Z);
    PRUEF(r.knall1_bild > r.msg_bild[3] && r.knall1_bild < r.msg_bild[4],
          "Tuerknall (Se_on Bank 0x0E Satz 1) zwischen 'We won't reach anyone' und 'Leon! You already made it!'");
    PRUEF(r.marvin_tuer_bild > 0 && r.marvin_tuer_bild - r.knall1_bild <= 15 &&
          r.marvin_tuer_x == RE15_CUT10F0_SPAWN_X && r.marvin_tuer_z == RE15_CUT10F0_SPAWN_Z,
          "Marvin erscheint nach dem Knall an der Tuer (Spawnpunkt (%d,%d), B%d)", r.marvin_tuer_x, r.marvin_tuer_z,
          r.marvin_tuer_bild);
    PRUEF(r.marvin_bei_leon_bild > 0 && r.marvin_bei_leon_bild < r.msg_bild[5] + 400 &&
          r.marvin_end_x < r.leon_x && r.marvin_end_z < r.leon_z,
          "Marvin steht danach schraeg links vor Leon und Ada ((%d,%d), B%d)", r.marvin_end_x, r.marvin_end_z,
          r.marvin_bei_leon_bild);
    PRUEF(r.knall2_bild > r.msg_bild[17], "zweiter Tuerknall nach Marvins letzter Zeile (Tuer faellt zu)");
    PRUEF(r.ende > 0 && r.ende <= SZENE_MAX, "Faden endet (B%d <= %d)", r.ende, SZENE_MAX);
    PRUEF(r.marvin_weg && r.ada_weg, "am Ende sind Marvin und Ada geparkt (durch die Tuer verschwunden)");
    PRUEF(r.lb_voll > 0 && r.lb_voll < r.msg_bild[0] && r.lb_weg > r.ende,
          "Balken wie jede Original-Szene: voll ab B%d, nach dem Ende weg (B%d)", r.lb_voll, r.lb_weg);
    PRUEF(r.f27 == 0 && r.f17 == 0 && r.pm_frei && r.auto_ende,
          "danach (2,7)=0, (1,27)=0, Steuerung frei, Kamera-Automatik an");
    PRUEF(r.n_cuts >= 2 && r.cuts[r.n_cuts - 1 < 16 ? r.n_cuts - 1 : 15] == 2, "letzter Cut der Szene = 2 (Leon im Bild)");
    PRUEF(re15_cut10f0_zustand() == RE15_CUT10F0_FERTIG, "Zustand FERTIG");
    int k1 = re15_map_hint_eintrag_fuer(RE15_CUT10F0_RAUM, RE15_CUT10F0_ZIEL1_RAUM);
    PRUEF(k1 >= 0 && r.hint_nr == k1, "Kartenhinweis angefordert: Eintrag K1 (ROOM11C0) = %d, anstehend %d", k1, r.hint_nr);
    /* Nachbesserung 1, Mangel 2: MAIN01 beginnt NICHT am Ende der 10F0-Szene, sondern erst nach der
     * 1150-Montage ((9,73), Spur L) — AUFTRAG.md Z.92 steht hinter der Montage. */
    PRUEF(r.bgm_vorher == -1 && r.bgm_nachher == -1 && !re15_cut10f0_bgm_fenster(),
          "BGM-Weiche: vor UND nach der 10F0-Szene Tabelle (-1/%d), Fenster zu - MAIN01 erst nach der 1150-Montage",
          r.bgm_nachher);
    /* ---- Fortsetzung (Dossier §8.2): wem Leon zugewandt ist. Zeile 7 (k=1) und 12 (k=6) zu Ada, Zeile 11
     * (k=5), 16 (k=10), 21 (k=15) und am Ende zu Marvins Standort — "Hey Marvin ... wieder arm strecken" zeigt
     * auf MARVIN, "This is... -> arm strecken Richtung Ada" auf ADA. Toleranz 160/4096 = 14 Grad. */
    {
        static const struct { int k; int zu_ada; const char *zeile; } w[] = {
            /* Nachbesserung 1, Mangel 3: schon die ERSTE Zeile spricht Leon Ada zugewandt (vorher Gierung 2048
             * bei Soll 2942 = 78,6 Grad vorbei). Stand = Tuer-Spawn (8400,-350). */
            { 0, 1, "6 'Hey - how did you came in here?' (Leon an der Tuer)" },
            { 1, 1, "7 'Did you really think' (Leon bei Ada)" }, { 5, 0, "11 'Hey Marvin, glad you made it!'" },
            { 6, 1, "12 'Allow me to introduce you. This is...'" }, { 10, 0, "16 'Anyway... looks like'" },
            { 13, 0, "19 'I know! The patrol car!'" }, { 15, 0, "21 'Okay, Marvin, you go with Ada'" },
            { 16, 0, "22 'I'm going to get Chief Irons'" },
        };
        for (unsigned i = 0; i < sizeof w / sizeof w[0]; i++) {
            int k = w[i].k;
            int soll = w[i].zu_ada
                ? gierung_nach(r.leon_x_msg[k], r.leon_z_msg[k], RE15_CUT10F0_ADA_X, RE15_CUT10F0_ADA_Z)
                : gierung_nach(r.leon_x_msg[k], r.leon_z_msg[k], RE15_CUT10F0_MARVIN_X, RE15_CUT10F0_MARVIN_Z);
            PRUEF(r.msg_bild[k] > 0 && gier_abstand(r.leon_rot_msg[k], soll) <= 160,
                  "Zeile %s: Leon blickt zu %s (Gierung %d, Soll %d)", w[i].zeile, w[i].zu_ada ? "Ada" : "Marvin",
                  r.leon_rot_msg[k], soll);
        }
        PRUEF(r.leon_x_msg[0] == RE15_CUT10F0_SPAWN_X && r.leon_z_msg[0] == RE15_CUT10F0_SPAWN_Z,
              "Zeile 6: Leon steht dabei noch am Tuer-Spawn (%d,%d)", r.leon_x_msg[0], r.leon_z_msg[0]);
        /* Marvin steht dabei wirklich dort, wohin Leon sich dreht */
        PRUEF(abs(r.mv_x_msg[5] - RE15_CUT10F0_MARVIN_X) < 200 && abs(r.mv_z_msg[5] - RE15_CUT10F0_MARVIN_Z) < 300,
              "Marvin steht bei Zeile 11 an seinem Platz (%d,%d)", r.mv_x_msg[5], r.mv_z_msg[5]);
        int soll_ende = gierung_nach(r.leon_x, r.leon_z, RE15_CUT10F0_MARVIN_X, RE15_CUT10F0_MARVIN_Z);
        PRUEF(gier_abstand(r.leon_rot, soll_ende) <= 160, "Leon steht am Ende Marvins Platz zugewandt (Gierung %d, Soll %d)",
              r.leon_rot, soll_ende);
    }
    /* ---- Fortsetzung (Dossier §8.3): Zeilentakt. Keine Zeile wird vor 90 Bildern von der naechsten ersetzt
     * (kuerzeste Original-Zeile ROOM11B0 sub06 msg 4 @0x01574: Sleep 40 @0x01580 + Sleep 50 @0x0158C); der
     * Regeltakt ist 110 (40 + 50 + 20, @0x014FA/@0x01502/@0x0150A). */
    {
        int min_abst = 1 << 30, min_k = -1;
        printf("  Zeilenabstaende (Bilder bis zur naechsten Zeile):");
        for (int k = 0; k + 1 < 18; k++) {
            int d = r.msg_bild[k + 1] - r.msg_bild[k];
            printf(" %d:%d", k + RE15_CUT10F0_MSG_ERSTE, d);
            if (d < min_abst) { min_abst = d; min_k = k; }
        }
        printf("\n");
        PRUEF(min_abst >= 90, "jede Zeile steht mindestens 90 Bilder (kuerzeste: msg %d mit %d Bildern)",
              min_k + RE15_CUT10F0_MSG_ERSTE, min_abst);
    }
}

static void teil_einmal(void)
{
    re15_game_state_init();
    if (room_boot(RE15_CUT10F0_RAUM, RE15_CUT10F0_SPAWN_X, RE15_CUT10F0_SPAWN_Z, RE15_CUT10F0_SPAWN_DIR, 0) != 0) return;
    szene_t r = szene_fahren();
    PRUEF(r.ende > 0, "erster Aufbau: Szene laeuft (Ende B%d)", r.ende);
    /* zweiter Aufbau desselben Raums mit den Flags danach */
    if (room_boot(RE15_CUT10F0_RAUM, RE15_CUT10F0_SPAWN_X, RE15_CUT10F0_SPAWN_Z, RE15_CUT10F0_SPAWN_DIR, 0) != 0) return;
    for (int f = 0; f < 60; f++) frame(0, 0);
    PRUEF(re15_cut10f0_zustand() == RE15_CUT10F0_AUS && faeden_im_programm() == 0,
          "zweiter Aufbau: keine Szene (Zustand AUS, kein Faden)");
    PRUEF(!g_actors[RE15_CUT10F0_AKTOR_ADA].active && !g_actors[RE15_CUT10F0_AKTOR_MARVIN].active,
          "zweiter Aufbau: kein Ada-/Marvin-Spawn");
    PRUEF(re15_cut10f0_rbj_quelle(RE15_CUT10F0_RAUM) == 0, "Gestenblock-Leihe nur solange die Szene aussteht");
    PRUEF(re15_cut10f0_ereignis(RE15_CUT10F0_RAUM, RE15_CUT10F0_EREIGNIS) == NULL, "Ereignis 20 liefert kein Programm mehr");
    /* Elza */
    re15_game_state_init();
    if (room_boot(0x10F1, RE15_CUT10F0_SPAWN_X, RE15_CUT10F0_SPAWN_Z, RE15_CUT10F0_SPAWN_DIR, 0) != 0) return;
    for (int f = 0; f < 60; f++) frame(0, 0);
    PRUEF(re15_cut10f0_zustand() == RE15_CUT10F0_AUS && faeden_im_programm() == 0 &&
          !g_actors[RE15_CUT10F0_AKTOR_ADA].active,
          "ROOM10F1 (Elza): keine Szene, kein Spawn, Flag (9,71) bleibt %d",
          re15_game_flag_get(RE15_CUT10F0_GESEHEN_BANK, RE15_CUT10F0_GESEHEN_BIT));
    PRUEF(re15_cut10f0_rbj_quelle(0x10F1) == 0 && re15_cut10f0_rbj_quelle(RE15_CUT10F0_RAUM) == RE15_CUT10F0_RBJ_RAUM,
          "Leihe nur fuer ROOM10F0 (Quelle ROOM11B0)");
}

static void teil_karte(void)
{
    re15_game_state_init();
    int alt = re15_map_hint_eintrag_fuer(0x1150, 0x10F0);
    int k1 = re15_map_hint_eintrag_fuer(RE15_CUT10F0_RAUM, RE15_CUT10F0_ZIEL1_RAUM);
    int k2 = re15_map_hint_eintrag_fuer(RE15_CUT10F0_RAUM, RE15_CUT10F0_ZIEL2_RAUM);
    PRUEF(alt >= 0 && k1 >= 0 && k2 >= 0 && k1 != k2 && k1 != alt, "Eintraege: Runde 33 (%d), K1 (%d), K2 (%d)", alt, k1, k2);
    PRUEF(re15_map_hint_folge(alt) == -1 && !re15_map_hint_zeitgesteuert(alt),
          "Runde-33-Eintrag unveraendert: kein Folge-Hinweis, nicht zeitgesteuert");
    PRUEF(re15_map_hint_folge(k1) == k2 && re15_map_hint_folge(k2) == -1 &&
          re15_map_hint_zeitgesteuert(k1) && re15_map_hint_zeitgesteuert(k2),
          "Kette K1 -> K2 -> Ende, beide zeitgesteuert");
    int p1 = -1, r1 = -1, p2 = -1, r2 = -1;
    PRUEF(re15_map_hint_ziel(k1, &p1, &r1) && p1 == 0 && r1 == 4, "K1 Ziel ROOM11C0 = Blatt %d Rechteck %d (Hauptzeile)", p1, r1);
    PRUEF(re15_map_hint_ziel(k2, &p2, &r2) && p2 == 4 && r2 == 2, "K2 Ziel ROOM1150 = Blatt %d Rechteck %d (Hauptzeile)", p2, r2);
    PRUEF(RE15_CUT10F0_HINWEIS_PERIODEN * re15_map_hint_periode() == 234, "Hinweisdauer 3 x 78 = 234 VBlank-Schritte");
    /* vor der Szene: keine Ziele */
    int zp = 0, zr = 0;
    PRUEF(!re15_map_ziel_aktiv_n(0, &zp, &zr), "vor der Szene kein markiertes Ziel");
    /* Szene gesehen (Flag), ROOM10F0 besucht, 11C0/1150 nicht */
    re15_game_flag_set(RE15_CUT10F0_GESEHEN_BANK, RE15_CUT10F0_GESEHEN_BIT, 1);
    int a0 = re15_map_ziel_aktiv_n(0, &zp, &zr); int p0 = zp, q0 = zr;
    int a1 = re15_map_ziel_aktiv_n(1, &zp, &zr); int p_1 = zp, q_1 = zr;
    int a2 = re15_map_ziel_aktiv_n(2, &zp, &zr);
    PRUEF(a0 && p0 == 0 && q0 == 4 && a1 && p_1 == 4 && q_1 == 2 && !a2,
          "nach der Szene: Ziel 0 = ROOM11C0 (0/4), Ziel 1 = ROOM1150 (4/2), kein drittes");
    PRUEF(re15_map_ziel_blatt_frei(0) && re15_map_ziel_blatt_frei(4), "Blaetter 0 und 4 fuer die Karte frei");
    /* ROOM1150 NACH der Szene betreten (Raumaufbau -> Latch (9,72)) -> nur noch 11C0; ROOM11C0 besucht -> nichts mehr */
    re15_cut10f0_install(0x1151);                                  /* Elzas Variante: kein Latch */
    PRUEF(re15_map_ziel_aktiv_n(1, &zp, &zr), "Raumaufbau ROOM1151 (Elza) laesst ROOM1150 weiter blinken");
    re15_cut10f0_install(0x1150);
    a0 = re15_map_ziel_aktiv_n(0, &zp, &zr); a1 = re15_map_ziel_aktiv_n(1, &zp, &zr);
    PRUEF(a0 && zp == 0 && !a1, "ROOM1150 nach der Szene betreten -> nur noch ROOM11C0 blinkt (Blatt 0)");
    /* Nachbesserung 1: "Parkplatz erreicht" = ORIGINAL-Flag (4,64) der Ankunftsszene (ROOM11C0 sub02 @0x0184E),
     * NICHT das Besucht-Bit der Zone — das setzt jeder Raumaufbau, auch der Montage-Schnitt der Spur L. */
    re15_map_zone_update(0x11C0, -10000, 0);
    PRUEF(re15_map_ziel_aktiv_n(0, &zp, &zr) && zp == 0 && zr == 4,
          "Raumaufbau ROOM11C0 allein (Montage-Schnitt, Zone besucht): die Kachel blinkt weiter");
    re15_game_flag_set(RE15_CUT10F0_ZIEL1_ERREICHT_BANK, RE15_CUT10F0_ZIEL1_ERREICHT_BIT, 1);
    PRUEF(!re15_map_ziel_aktiv_n(0, &zp, &zr), "Ankunftsszene ROOM11C0 gestartet ((4,64)=1) -> kein Ziel mehr");

    /* ---- WIE IM ECHTEN SPIEL (Fortsetzung, Dossier §8.1): ROOM1150 ist VOR der 10F0-Szene laengst besucht
     * (erste Irons-Szene (3,94), deren Hinweis erst nach ROOM10F0 schickt). "So lange der Raum nicht besucht
     * ist, blinken beide weiter" meint den Besuch NACH der Szene -> eigener Latch (9,72), nicht das
     * Besucht-Bit der Zone. */
    re15_game_state_init();
    re15_map_visited_reset();                                     /* die Besucht-Bits sind nicht Teil von g_game */
    re15_map_zone_update(0x1150, -21000, -20000);                 /* Besuch davor */
    re15_game_flag_set(3, 94, 1);
    re15_map_zone_update(RE15_CUT10F0_RAUM, RE15_CUT10F0_SPAWN_X, RE15_CUT10F0_SPAWN_Z);
    PRUEF(!re15_map_ziel_aktiv_n(0, &zp, &zr), "echter Weg: vor der 10F0-Szene blinkt nichts (1150 besucht, 10F0 besucht)");
    re15_cut10f0_install(0x1150);                                  /* Raumaufbau 1150 VOR der Szene: kein Latch */
    PRUEF(re15_game_flag_get(RE15_CUT10F0_ZIEL2_BESUCHT_BANK, RE15_CUT10F0_ZIEL2_BESUCHT_BIT) == 0,
          "echter Weg: Raumaufbau ROOM1150 vor der Szene setzt (9,72) nicht");
    re15_game_flag_set(RE15_CUT10F0_GESEHEN_BANK, RE15_CUT10F0_GESEHEN_BIT, 1);
    a0 = re15_map_ziel_aktiv_n(0, &zp, &zr); p0 = zp; q0 = zr;
    a1 = re15_map_ziel_aktiv_n(1, &zp, &zr); p_1 = zp; q_1 = zr;
    PRUEF(a0 && p0 == 0 && q0 == 4 && a1 && p_1 == 4 && q_1 == 2,
          "echter Weg: nach der Szene blinken ROOM11C0 (%d/%d) UND das schon frueher besuchte ROOM1150 (%d/%d, aktiv %d)",
          p0, q0, p_1, q_1, a1);
    re15_cut10f0_install(0x10D0);                                  /* andere Raeume: kein Latch */
    PRUEF(re15_map_ziel_aktiv_n(1, &zp, &zr), "echter Weg: ROOM1150 blinkt in anderen Raeumen weiter");
    re15_cut10f0_install(0x1150);                                  /* Leon betritt ROOM1150 NACH der Szene */
    a0 = re15_map_ziel_aktiv_n(0, &zp, &zr); a1 = re15_map_ziel_aktiv_n(1, &zp, &zr);
    PRUEF(re15_game_flag_get(RE15_CUT10F0_ZIEL2_BESUCHT_BANK, RE15_CUT10F0_ZIEL2_BESUCHT_BIT) == 1 && a0 && zp == 0 && !a1,
          "echter Weg: Betreten von ROOM1150 nach der Szene setzt (9,72) -> nur noch ROOM11C0 blinkt");
    re15_game_flag_set(RE15_CUT10F0_ZIEL1_ERREICHT_BANK, RE15_CUT10F0_ZIEL1_ERREICHT_BIT, 1);
    PRUEF(!re15_map_ziel_aktiv_n(0, &zp, &zr), "echter Weg: Parkplatz erreicht ((4,64)=1) -> kein Ziel mehr");
}

/* MAIN01-Fenster (Nachbesserung 1, Dossier §9). Beginn: (9,71) UND (9,73) und erst, wenn keine Szene mehr laeuft
 * (Mangel 2); der Tick stoesst die Raummusik selbst an (Montage-Ende, Laden). Ende: (4,64), das Flag der
 * Ankunftsszene ROOM11C0 sub02 @0x0184E. Solange die Audio-Schicht MAIN01 bekommen hat, gelten Skript-Befehle an
 * den MAIN-Slot nicht (Mangel 1). Der Spion in tests/test_support.c zaehlt re15_audio_start_room_bgm. */
static void szenenrahmen(int an) { re15_game_flag_set(2, 7, (uint8_t)an); re15_game_flag_set(1, 27, (uint8_t)an); }
static void ticks(int n) { for (int i = 0; i < n; i++) re15_cut10f0_tick(); }
/* Raumaufbau in der Reihenfolge von scd_room_reenter: memset(g_scd) (tick_count 0), Init-Lauf scd_vm_tick
 * (scd_room_setup.c:421, tick_count 1), Rahmen-Flags geloescht (@0x80039710-30), DANN die Installer. */
static void raumaufbau(uint16_t raum)
{
    g_current_room_id = raum;
    g_scd.tick_count = 1;
    szenenrahmen(0);
    re15_cut10f0_install(raum);
}
static void vm_lauf(void) { g_scd.tick_count++; }          /* ein Spielbild-Lauf der VM (scd_vm.c:684) */
static int weg_main01(void)
{
    /* jeder STAGE1-Raum des Wegs, auch der Zwinger 0x1D (Tabelle 0xFF7B) und der Hinterhof 0x09 (0x0355) */
    static const int raeume[] = { 0x0F, 0x0D, 0x10, 0x11, 0x12, 0x13, 0x15, 0x04, 0x06, 0x03, 0x09, 0x16, 0x18, 0x1D,
                                  0x1B, 0x1F };
    for (unsigned i = 0; i < sizeof raeume / sizeof raeume[0]; i++)
        if (re15_cut10f0_bgm_eintrag(0, raeume[i]) != RE15_CUT10F0_BGM_EINTRAG) return 0;
    return 1;
}

static void teil_bgm(void)
{
    re15_game_state_init();
    re15_map_visited_reset();
    raumaufbau(0x1150);
    (void)re15_cut10f0_bgm_eintrag(0, 0x15);                 /* die Audio-Schicht fragt beim Raumaufbau */
    vm_lauf();
    ticks(2);
    PRUEF(re15_cut10f0_bgm_eintrag(0, 0x0F) == -1 && re15_cut10f0_bgm_eintrag(0, 0x15) == -1 && !re15_cut10f0_bgm_fenster(),
          "vor der Szene: Tabelle (-1) fuer 10F0 und 1150");

    /* ---- Mangel 2: die 10F0-Szene allein oeffnet das Fenster NICHT ---- */
    re15_game_flag_set(RE15_CUT10F0_GESEHEN_BANK, RE15_CUT10F0_GESEHEN_BIT, 1);
    g_test_bgm_start_count = 0; g_test_bgm_reset_count = 0;
    ticks(3);
    PRUEF(re15_cut10f0_bgm_eintrag(0, 0x15) == -1 && re15_cut10f0_bgm_eintrag(0, 0x0F) == -1 &&
          !re15_cut10f0_bgm_fenster() && g_test_bgm_start_count == 0,
          "nach der 10F0-Szene allein ((9,71)=1, (9,73)=0): weiter Tabelle - der Weg zu Irons laeuft ohne MAIN01");

    /* ---- die 1150-Montage laeuft: (9,73) steht schon, die Rahmen-Flags (2,7)/(1,27) auch ---- */
    szenenrahmen(1);
    re15_game_flag_set(RE15_CUT10F0_BGM_START_BANK, RE15_CUT10F0_BGM_START_BIT, 1);
    ticks(5);
    PRUEF(!re15_cut10f0_bgm_fenster() && re15_cut10f0_bgm_eintrag(0, 0x15) == -1 &&
          re15_cut10f0_bgm_eintrag(0, 0x13) == -1 && g_test_bgm_start_count == 0,
          "waehrend der Montage ((9,73)=1, Szene laeuft): kein MAIN01, auch nicht bei einem Raumaufbau, kein Anstoss");

    /* ---- Schnitt der Montage in den naechsten Raum (gemessen mit K+L zusammen, Dossier §9.7): der Raumaufbau
     * loescht die Rahmen-Flags ((1,27) @0x80039710-30, (2,7)-Schatten); der Montage-Schritt startet erst NACH
     * dem Init-Lauf (Installer) und setzt sie bei seinem ersten Spielbild-Lauf. Vorher ging hier das Fenster auf
     * (Lauf m1: "MAIN01-Fenster auf in ROOM1130" vor "[flag] z2/7 = 1"). ---- */
    raumaufbau(0x1130);
    ticks(1);
    PRUEF(!re15_cut10f0_bgm_fenster() && g_test_bgm_start_count == 0,
          "Raumaufbau mitten in der Montage (Rahmen-Flags geloescht, nur der Init-Lauf der VM): Fenster zu");
    vm_lauf();                                               /* erster Spielbild-Lauf: Set(2,7,1)/Set(1,27,1) */
    szenenrahmen(1);
    ticks(3);
    PRUEF(!re15_cut10f0_bgm_fenster() && g_test_bgm_start_count == 0,
          "... nach dem ersten VM-Lauf des Montage-Schritts (Rahmen-Flags wieder gesetzt): weiter zu, kein Anstoss");

    /* ---- Montage-Ende (Rueckkehr nach ROOM1150, Programm loescht den Rahmen): Fenster auf, EIN Anstoss ---- */
    raumaufbau(0x1150);
    ticks(1);
    PRUEF(!re15_cut10f0_bgm_fenster(), "Rueckkehr-Raumaufbau ROOM1150 (Init-Lauf): noch zu");
    vm_lauf();
    szenenrahmen(1);                                         /* Rueckkehr-Programm laeuft */
    ticks(2);
    vm_lauf();
    szenenrahmen(0);                                         /* ... und gibt frei */
    ticks(1);
    PRUEF(re15_cut10f0_bgm_fenster() && g_test_bgm_start_count == 1 && g_test_bgm_start_room == 0x15 &&
          g_test_bgm_reset_count == 1,
          "Montage-Ende: Fenster offen, Raummusik EINMAL angestossen (Raum 0x%02X, %dx), Skript-Latches geleert (%dx)",
          g_test_bgm_start_room, g_test_bgm_start_count, g_test_bgm_reset_count);
    ticks(5);
    PRUEF(g_test_bgm_start_count == 1, "danach kein weiterer Anstoss (%dx)", g_test_bgm_start_count);
    PRUEF(weg_main01(), "Fenster offen: 0xFF01 (MAIN01, kein SUB, kein Handstart-Flag) in jedem STAGE1-Raum des Wegs");
    PRUEF(re15_cut10f0_bgm_eintrag(1, 0x0F) == -1 && re15_cut10f0_bgm_eintrag(2, 0x00) == -1, "andere Stages: Tabelle");

    /* ---- Mangel 1: die Auskunft an die Audio-Schicht sperrt Skript-Befehle an den MAIN-Slot ---- */
    (void)re15_cut10f0_bgm_eintrag(0, 0x1D);                 /* Raumaufbau Zwinger ROOM11D0 */
    PRUEF(re15_cut10f0_bgm_haelt_main() == 1,
          "Zwinger ROOM11D0 im Fenster: MAIN gehalten - Sce_bgm_control Slot 0 (sub01 @0x01710 op 2) gilt nicht");
    PRUEF(re15_cut10f0_bgm_eintrag(0, 0x1C) == -1 && re15_cut10f0_bgm_haelt_main() == 0,
          "im Parkplatz ROOM11C0 (Raum-Byte 0x1C): eigene Musik, Skript-Befehle gelten wieder");
    (void)re15_cut10f0_bgm_eintrag(0, 0x15);                 /* zurueck: die Audio-Schicht hat wieder MAIN01 */

    /* ---- "durchweg": eine spaetere Szene im Fenster schliesst es nicht ---- */
    szenenrahmen(1);
    ticks(3);
    PRUEF(re15_cut10f0_bgm_fenster() && re15_cut10f0_bgm_eintrag(0, 0x1B) == RE15_CUT10F0_BGM_EINTRAG,
          "eine spaetere Szene im Fenster: MAIN01 bleibt");
    szenenrahmen(0);

    /* ---- der Montage-Schnitt nach ROOM11C0 setzt das Besucht-Bit der Zone: das beendet NICHTS ---- */
    re15_map_zone_update(0x11C0, -10000, 0);
    (void)re15_cut10f0_bgm_eintrag(0, 0x15);
    ticks(2);
    PRUEF(re15_cut10f0_bgm_fenster() && re15_cut10f0_bgm_eintrag(0, 0x1B) == RE15_CUT10F0_BGM_EINTRAG,
          "Besucht-Bit der Zone ROOM11C0 (Raumaufbau ohne Leon) beendet das Fenster nicht");

    /* ---- Leon erreicht den Parkplatz: die Ankunftsszene setzt (4,64) ---- */
    g_current_room_id = 0x11C0;
    (void)re15_cut10f0_bgm_eintrag(0, 0x1C);                 /* Tuer 11B0 -> 11C0: Tabelle (0xFF56) */
    re15_game_flag_set(RE15_CUT10F0_ZIEL1_ERREICHT_BANK, RE15_CUT10F0_ZIEL1_ERREICHT_BIT, 1);
    g_test_bgm_start_count = 0;
    ticks(3);
    PRUEF(!re15_cut10f0_bgm_fenster() && re15_cut10f0_bgm_eintrag(0, 0x1B) == -1 &&
          re15_cut10f0_bgm_eintrag(0, 0x0F) == -1,
          "Parkplatz erreicht ((4,64)=1): Fenster zu, Tabelle wieder normal");

    /* ---- LADEN im Fenster: der Boot-BGM-Aufruf laeuft VOR dem Restore (Tabelle), der Tick zieht nach ---- */
    re15_game_state_init();
    g_current_room_id = RE15_CUT10F0_RAUM;
    PRUEF(re15_cut10f0_bgm_eintrag(0, 0x0F) == -1, "Boot-BGM vor dem Restore: Tabelle");
    re15_game_flag_set(RE15_CUT10F0_GESEHEN_BANK, RE15_CUT10F0_GESEHEN_BIT, 1);       /* Restore */
    re15_game_flag_set(RE15_CUT10F0_BGM_START_BANK, RE15_CUT10F0_BGM_START_BIT, 1);
    g_test_bgm_start_count = 0;
    raumaufbau(RE15_CUT10F0_RAUM);                            /* Raumaufbau des Ladens (Szene gesehen: kein Start) */
    ticks(1);
    PRUEF(!re15_cut10f0_bgm_fenster() && g_test_bgm_start_count == 0,
          "nach dem Laden, nur der Init-Lauf der VM: Fenster noch zu");
    vm_lauf();                                                /* erster Spielbild-Lauf im geladenen Raum */
    ticks(1);
    PRUEF(re15_cut10f0_bgm_fenster() && g_test_bgm_start_count == 1 && g_test_bgm_start_room == 0x0F &&
          re15_cut10f0_bgm_eintrag(0, 0x0F) == RE15_CUT10F0_BGM_EINTRAG,
          "nach dem Laden im Fenster: das erste Spielbild nach dem ersten VM-Lauf stoesst die Raummusik an (Raum 0x%02X, %dx)",
          g_test_bgm_start_room, g_test_bgm_start_count);

    /* ---- LADEN eines Stands AUSSERHALB des Fensters, waehrend die Audio-Schicht noch MAIN01 hat ---- */
    re15_game_state_init();                                   /* Flags weg, fluechtiger Zustand steht noch */
    g_test_bgm_start_count = 0;
    ticks(1);
    PRUEF(!re15_cut10f0_bgm_fenster() && g_test_bgm_start_count == 1 && re15_cut10f0_bgm_haelt_main() == 0,
          "Stand ausserhalb des Fensters geladen: Fenster zu, Raummusik zurueck auf die Tabelle angestossen (%dx)",
          g_test_bgm_start_count);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "szene";
    printf("test_r35_cut10f0 %s\n", teil);
    if      (!strcmp(teil, "programm")) teil_programm();
    else if (!strcmp(teil, "texte"))    teil_texte();
    else if (!strcmp(teil, "tuerton"))  teil_tuerton();
    else if (!strcmp(teil, "szene"))    teil_szene();
    else if (!strcmp(teil, "einmal"))   teil_einmal();
    else if (!strcmp(teil, "karte"))    teil_karte();
    else if (!strcmp(teil, "bgm"))      teil_bgm();
    else { printf("unbekannter Teil %s\n", teil); return 2; }
    printf("%s: %s (%d Fehler)\n", teil, g_fail ? "FAIL" : "PASS", g_fail);
    return g_fail ? 1 : 0;
}
