/* test_r37_szene11c0.c — RIEGEL Runde 37 Spur O "szene11c0": Ada/Marvin-Szene ROOM11C0 nach Irons' Tod.
 *
 * Dossier: analysis/befunde_runde37/O_szene11c0.md. Code: Programm 4 k_p_11c0 in engine/src/irons_tod_1150.c,
 * Rechenhilfe + Mess-Haken engine/src/szene11c0_r37.c (re15_s11c0_gier, RE15_S11C0_LOG).
 *
 * NUTZER (Nachtrag 11, woertlich): "Erst schauen sie komplett in die falsche Richtung. Dann drehen sie sich einmal um
 * 180 grad um. Dann dreht sich Ada noch mal um 90 grad um. Zwischendurch steht marvin da, spricht, schaut aber nicht zu
 * ada.... da musst du noch mal ran".
 *
 * Teile (je ein ctest-Eintrag):
 *   aufstellung  Form + Zahlen: die Anfangs-Gieren im Programm sind die Peilungen auf den Partner (re15_s11c0_gier =
 *                FUN_8001a6d4-Form des Ports); das Original-Vorbild ROOM10D0 @0x01ABC..@0x01AF6 stellt seine
 *                Gespraechspartner genauso auf (Dir_set = Peilung +-17); Adas Record @0x01770 traegt 475 (die
 *                "falsche Richtung"); im Programm keine Koerperdrehung im Gespraech (kein Plc_dest Modus 9 vor dem
 *                Lauf), genau ein Lauf (Modus 5) erst nach Ablauf von Marvins letzter Zeile, danach genau eine Drehung
 *                Adas nach dem Signal "Ada folgt Marvin" (Nachtrag 12).
 *   ablauf       die Szene im Spieltakt (Raumaufbau wie das Spiel, scd_room_reenter): einander zugewandt ab dem ersten
 *                Bild, im Gespraech (msg 10..13) dreht sich niemand, Marvin nur beim Losrennen (nach seiner Zeile,
 *                < 120 Grad), je Zeile Sprecher UND Zuhoerer zum Partner (Koerper im Kegel 0x60 @0x80076c41, Kopf auf
 *                dem Partner bzw. Keyframe/Nicken), der Knall nur mit den Koepfen; Nachtrag 12: Ada dreht sich dem
 *                laufenden Marvin EINMAL nach (< 90 Grad, bei msg 14 im Kegel 0x60 neben ihm) und ihr Kopf-Ziel
 *                folgt ihm in jedem Bild; Schnitt zurueck nach 1150 Cut 7.
 *   auswerten <lauf>  Auswerter fuer integration_r37_szene11c0 (echte exe): s11c0.log (RE15_S11C0_LOG) + neck_trace.log
 *                (RE15_NECK_TRACE, der GEZEICHNETE Kopf): dieselben Bedingungen mit dem wirklichen Kopf-Blick. */
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
#include "re15_irons_tod.h"
#include "re15_szene11c0_r37.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void re15_actor_step_all_walkers(void);
int  re15_ai_arc_test(const re15_actor_t *e, int32_t px, int32_t pz, int cone);

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

/* Orte (Dossier B1): Ada = ROOM11C0 sub00 @0x01770, Marvin = r35 M5, Tuer = Mitte Rechteck Slot 0 @0x01712. */
#define ADA_X   (-8965)
#define ADA_Z   (-14347)
#define MAR_X   (-8163)
#define MAR_Z   (-13748)
#define TUER_X  (-25100)
#define TUER_Z  (17250)
#define KEGEL   0x60     /* Ausrichtungs-Kegel des Drehens Typ 0x40/0x42 (Tabelle @0x80076c41, gelesen 96) */
#define KLEMME  0x2c8    /* NPC-Kopf-Gier-Klemme (STAGE1 @0x8011c79c/@0x8011c7a0) */
#define KEIN_180 0x555   /* 120 Grad: jede Einzeldrehung bleibt deutlich unter einer Kehrtwende */

static int wabs(int a, int b) { int d = re15_s11c0_winkel_diff(a, b); return d < 0 ? -d : d; }

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
static int le16(const uint8_t *p) { return (int16_t)(p[0] | (p[1] << 8)); }

/* ================================================================================================================
 * Teil aufstellung
 * ============================================================================================================== */
static uint8_t *rdt_roh(uint16_t room, size_t *n)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    return slurp(rp, n);
}
static int op_len(uint8_t op)   /* Satzbreiten = s_opcode_sizes (scd_vm.c) der im Programm 4 benutzten Opcodes */
{
    switch (op) {
    case 0x00: return 1;  /* Nop hinter Work_set (`2e kk ii` + Nop, ROOM1150 sub08 @0x0111E) */
    case 0x01: return 2;  case 0x09: return 4;  case 0x22: return 4;  case 0x2b: return 4;  case 0x2e: return 3;
    case 0x33: return 8;  case 0x3b: return 32; case 0x3f: return 4;  case 0x40: return 8;  case 0x41: return 10;
    case 0x43: return 4;  case 0x44: return 20; case 0x47: return 2;  case 0x18: return 2;
    default: return 0;
    }
}
static void teil_aufstellung(void)
{
    printf("== aufstellung ==\n");
    const int g_am = re15_s11c0_gier(ADA_X, ADA_Z, MAR_X, MAR_Z), g_ma = re15_s11c0_gier(MAR_X, MAR_Z, ADA_X, ADA_Z);
    const int g_at = re15_s11c0_gier(ADA_X, ADA_Z, TUER_X, TUER_Z), g_mt = re15_s11c0_gier(MAR_X, MAR_Z, TUER_X, TUER_Z);
    printf("  Peilungen (re15_s11c0_gier): Ada->Marvin %d, Marvin->Ada %d, Ada->Tuer %d, Marvin->Tuer %d\n", g_am, g_ma, g_at, g_mt);
    PRUEF(g_am == 3679 && g_ma == 1631, "Peilungen Ada->Marvin 3679 / Marvin->Ada 1631 (FUN_8001a6d4-Form des Ports)");

    /* Original-Vorbild ROOM10D0: Marvin/Leon-Dialog, Aufstellung per Pos_set + Dir_set + Plc_neck auf den Partner */
    size_t n10 = 0; uint8_t *r10 = rdt_roh(0x10D0, &n10);
    PRUEF(r10 && n10 > 0x01B00, "ROOM10D0.RDT gelesen");
    if (r10 && n10 > 0x01B00) {
        static const uint8_t mpos[8] = { 0x32,0x00,0xfb,0x1d,0x00,0x00,0x91,0x50 }, mdir[8] = { 0x33,0x00,0x00,0x00,0xea,0x06,0x00,0x00 };
        static const uint8_t lpos[8] = { 0x32,0x00,0x57,0x17,0x00,0x00,0x71,0x4d }, ldir[8] = { 0x33,0x00,0x00,0x00,0xf2,0x0e,0x00,0x00 };
        static const uint8_t mws[3] = { 0x2e,0x02,0x00 }, lws[3] = { 0x2e,0x01,0x00 };
        PRUEF(!memcmp(r10 + 0x01A96, mws, 3) && !memcmp(r10 + 0x01ABC, mpos, 8) && !memcmp(r10 + 0x01AC4, mdir, 8) &&
              !memcmp(r10 + 0x01AE2, lws, 3) && !memcmp(r10 + 0x01AE6, lpos, 8) && !memcmp(r10 + 0x01AEE, ldir, 8) &&
              r10[0x01ACC] == 0x41 && r10[0x01ACD] == 0x01 && r10[0x01AF6] == 0x41 && r10[0x01AF7] == 0x01,
              "ROOM10D0 @0x01A96..@0x01AF6: Work_set(2,0) Pos_set Dir_set 0x06ea Plc_neck m1 / Work_set(1,0) Pos_set Dir_set 0x0ef2 Plc_neck m1");
        int pm = re15_s11c0_gier(7675, 20625, 5975, 19825), pl = re15_s11c0_gier(5975, 19825, 7675, 20625);
        PRUEF(wabs(pm, 0x06ea) <= 17 && wabs(pl, 0x0ef2) <= 17,
              "ROOM10D0: Dir_set = Peilung auf den Partner (Marvin %d vs 0x06ea, Leon %d vs 0x0ef2, je <= 17)", pm, pl);
        PRUEF(le16(r10 + 0x01ACC + 2) == 5975 && le16(r10 + 0x01ACC + 6) == 19825 && le16(r10 + 0x01AF6 + 2) == 7675 &&
              le16(r10 + 0x01AF6 + 6) == 20625, "ROOM10D0: die Koepfe zeigen auf den Ort des Partners");
    }
    free(r10);
    size_t n11 = 0; uint8_t *r11 = rdt_roh(0x11C0, &n11);
    if (r11 && n11 > 0x01784) {
        PRUEF(r11[0x01770] == 0x44 && r11[0x01772] == 0x42 && le16(r11 + 0x01770 + 8) == ADA_X && le16(r11 + 0x01770 + 12) == ADA_Z &&
              le16(r11 + 0x01770 + 16) == 475,
              "ROOM11C0 sub00 @0x01770: Ada (Typ 0x42) bei (%d,%d) mit Gier 475 = Ruecken zur Kamera Cut 13 (die 'falsche Richtung')",
              ADA_X, ADA_Z);
        PRUEF(wabs(475, g_am) > KEGEL * 8, "Gier 475 zeigt nicht auf Marvin (%d Einheiten daneben)", wabs(475, g_am));
    } else PRUEF(0, "ROOM11C0.RDT gelesen");
    free(r11);

    /* das Programm selbst */
    int n = 0; const uint8_t *p = re15_irons_tod_programm(8, &n);
    PRUEF(p && n > 40, "Programm 4 (k_p_11c0) vorhanden, %d B", n);
    if (!p) return;
    int o = 0, ok = 1, work_k = -1, work_i = -1, em_rot = -1, ada_dir = -1, n_dreh = 0, n_lauf = 0, msg13 = -1, sleep_nach13 = 0;
    int lauf_nach13 = 0, neck_tuer_m = 0, neck_tuer_a = 0, msg10 = -1, dir_vor_riegel = 0, riegel = -1, neck_vor_riegel = 0;
    int n_dreh_nach = 0, dreh_ada_f1 = 0, sig_folgen = 0, sig_vor_dreh = 0;
    while (o < n) {
        int l = op_len(p[o]);
        if (l <= 0) { ok = 0; printf("  unbekanntes Opcode 0x%02x @+%d\n", p[o], o); break; }
        const uint8_t *q = p + o;
        if (q[0] == 0x2e) { work_k = q[1]; work_i = q[2]; }
        if (q[0] == 0x44) em_rot = le16(q + 16);
        if (q[0] == 0x22 && q[1] == 2 && q[2] == 7 && q[3] == 1 && riegel < 0) riegel = o;
        if (q[0] == 0x33 && work_k == 2 && work_i == 0) { ada_dir = le16(q + 4); if (riegel < 0) dir_vor_riegel = 1; }
        if (q[0] == 0x41 && riegel < 0) neck_vor_riegel = 1;
        if (q[0] == 0x22 && q[1] == 5 && q[2] == RE15_S11C0_SIG_FOLGEN && q[3] == 1) sig_folgen = 1;
        if (q[0] == 0x40 && q[2] == 9) {
            if (!n_lauf) n_dreh++;
            else { n_dreh_nach++; dreh_ada_f1 = (work_k == 2 && work_i == 0 && q[3] == 0x01); sig_vor_dreh = sig_folgen; }
        }
        if (q[0] == 0x40 && q[2] == 5) { n_lauf++; if (msg13 >= 0) lauf_nach13 = 1; }
        if (q[0] == 0x2b && q[1] == 13) msg13 = o;
        if (q[0] == 0x2b && q[1] == 10) msg10 = o;
        if (q[0] == 0x09 && msg13 >= 0 && !n_lauf) sleep_nach13 += le16(q + 2);
        if (q[0] == 0x41 && q[1] == 1 && le16(q + 2) == TUER_X && le16(q + 6) == TUER_Z && msg10 < 0) {
            if (work_k == 2 && work_i == 3) neck_tuer_m = 1;
            if (work_k == 2 && work_i == 0) neck_tuer_a = 1;
        }
        o += l;
    }
    PRUEF(ok && o == n && p[n - 2] == 0x01, "Programm 4: Opcode-Walk schliesst exakt (%d/%d B), Evt_end", o, n);
    PRUEF(em_rot == g_ma, "Marvin erscheint Ada zugewandt: Sce_em_set-Gier %d == Peilung %d (Satz +16 -> +0x6a @0x800421bc/c4)", em_rot, g_ma);
    PRUEF(ada_dir == g_am && dir_vor_riegel, "Ada wird im Raumaufbau Marvin zugewandt: Dir_set %d == Peilung %d (LAB_80041080, Form ROOM10D0 @0x01AC4)",
          ada_dir, g_am);
    PRUEF(!neck_vor_riegel, "kein Plc_neck vor dem ersten Spieltakt (der NPC-INIT @0x8011c768 wuerde es loeschen)");
    PRUEF(n_dreh == 0, "keine Koerperdrehung im Gespraech: kein Plc_dest Modus 9 vor Marvins Lauf (%d)", n_dreh);
    PRUEF(n_dreh_nach == 1 && dreh_ada_f1 && sig_vor_dreh,
          "Nachtrag 12: genau eine Drehung nach dem Lauf - Ada (Work_set(2,0)), Ankunfts-Bit 1 wie @0x018CA, nach dem Signal "
          "(5,%d) 'Ada folgt Marvin'", RE15_S11C0_SIG_FOLGEN);
    PRUEF(n_lauf == 1 && lauf_nach13 && sleep_nach13 >= 99,
          "genau ein Lauf (Modus 5), erst nach Marvins letzter Zeile: %d Bilder nach msg 13 >= 99 (Haltezeit 01 63)", sleep_nach13);
    PRUEF(neck_tuer_m && neck_tuer_a, "Knall: beide KOEPFE zur Tuer vor Adas Frage (Plc_neck m1, kein Drehen)");
}

/* ================================================================================================================
 * Spiel-Harness (wie test_r36_szene1150.c)
 * ============================================================================================================== */
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;

static int rdt_laden(uint16_t room)
{
    free(s_raw); s_raw = rdt_roh(room, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100) return -1;
    return re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0 ? -1 : 0;
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
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) g_scd.player_mode = 0;
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
static int room_boot_11c0(void)
{
    if (rdt_laden(0x11C0) != 0) { PRUEF(0, "ROOM11C0 nicht ladbar"); return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 13;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    bank_laden(0x45, 0); bank_laden(0x42, 1); bank_laden(0x40, 2);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = 0x11C0; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = RE15_IT_PARK_11C0_X; pl->z = RE15_IT_PARK_11C0_Z; pl->rot_y = RE15_IT_PARK_11C0_YAW;
    re15_collision_set_band(0);
    s_shown = 13; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 13);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);   /* Reihenfolge der exe (RBJ nach dem Installer) */
    return 0;
}

/* ================================================================================================================
 * gemeinsame Auswertung: Bild-Datensaetze (Spieltakt oder exe-Protokoll)
 * ============================================================================================================== */
#define MAXB 2048
typedef struct {
    int da;                      /* Datensatz vorhanden */
    int x, z, r, mo, s, nf, ntx, ntz, hx, hz;
    int tr; int tgt, res, acc, kf;   /* gezeichneter Kopf (nur exe) */
} fig_t;
typedef struct { int f, ma, mid; fig_t a, m; } bild_t;
static bild_t s_b[MAXB];
static int    s_nb = 0;

static int partner_ziel(const fig_t *k, const fig_t *p) { return (k->nf & 0x04) && k->ntx == p->x && k->ntz == p->z; }
static int tuer_ziel(const fig_t *k) { return (k->nf & 0x04) && k->ntx == TUER_X && k->ntz == TUER_Z; }
/* Kopf frei (0x12) / Nicken (0x2a): der Kopf steht auf dem Keyframe = geradeaus = beim zugewandten Koerper auf dem Partner */
static int kopf_geradeaus(const fig_t *k) { return !(k->nf & 0x04) && (k->nf & 0x02); }

/* Drehabschnitte einer Figur: Summe der Gier-Schritte; ein Abschnitt endet nach > 3 Bildern ohne Aenderung. */
typedef struct { int f0, f1, g0, g1, summe; } dreh_t;
static int drehungen(int welche, dreh_t *D, int maxd)
{
    int nd = 0, prev_f = -1, prev_r = 0, offen = 0;
    for (int i = 0; i < s_nb; i++) {
        const fig_t *k = welche ? &s_b[i].m : &s_b[i].a;
        if (!k->da) continue;
        if (prev_f >= 0) {
            int d = re15_s11c0_winkel_diff(prev_r, k->r);
            if (d != 0) {
                if (!offen && nd < maxd) { D[nd].f0 = prev_f; D[nd].g0 = prev_r; D[nd].summe = 0; nd++; offen = 1; }
                if (offen) { D[nd - 1].f1 = s_b[i].f; D[nd - 1].g1 = k->r; D[nd - 1].summe += d; }
            } else if (offen && s_b[i].f - D[nd - 1].f1 > 3) offen = 0;
        }
        prev_f = s_b[i].f; prev_r = k->r;
    }
    return nd;
}
typedef struct { int id, f0, f1; } zeile_t;
static int zeilen(zeile_t *Z, int maxz)
{
    int nz = 0;
    for (int i = 0; i < s_nb; i++) {
        if (!s_b[i].ma) continue;
        if (nz && Z[nz - 1].id == s_b[i].mid && s_b[i].f - Z[nz - 1].f1 <= 1) Z[nz - 1].f1 = s_b[i].f;
        else if (nz < maxz) { Z[nz].id = s_b[i].mid; Z[nz].f0 = Z[nz].f1 = s_b[i].f; nz++; }
    }
    return nz;
}

/* Kopf-Blick (Welt) wie die Kopf-FSM ihn zeichnet: Gier + Wurzel-Keyframe + Kopf-Keyframe + Akku; der Wurzel-Keyframe
 * folgt aus jedem ungeklemmten Bild: wYaw(Kopf-Ort -> Zielpunkt) - Gier - tgt (skeleton_common.c tgt_yaw-Form). */
static void blicke_rechnen(int welche, int *blick)
{
    int wurzel = 0, wurzel_da = 0;
    for (int i = 0; i < s_nb; i++) {
        const fig_t *k = welche ? &s_b[i].m : &s_b[i].a;
        blick[i] = -1;
        if (!k->da || !k->tr) continue;
        if ((k->nf & 0x04) && (k->tgt < KLEMME && k->tgt > -KLEMME)) {
            int w = re15_s11c0_gier(k->hx, k->hz, k->ntx, k->ntz);
            wurzel = re15_s11c0_winkel_diff(k->r, w) - k->tgt; wurzel_da = 1;
        }
        blick[i] = (k->r + (wurzel_da ? wurzel : 0) + k->kf + k->acc) & 0x0fff;
    }
}

/* Die Bedingungen des Nutzer-Befunds (je ein PRUEF); mit_kopf = gezeichneter Kopf vorhanden (exe). */
static void bedingungen(int mit_kopf, const char *quelle)
{
    printf("  [%s] %d Bilder (F%d..F%d)\n", quelle, s_nb, s_nb ? s_b[0].f : -1, s_nb ? s_b[s_nb - 1].f : -1);
    if (s_nb < 100) { PRUEF(0, "[%s] zu wenige Bilder", quelle); return; }
    const int g_am = re15_s11c0_gier(ADA_X, ADA_Z, MAR_X, MAR_Z), g_ma = re15_s11c0_gier(MAR_X, MAR_Z, ADA_X, ADA_Z);
    /* (1) "komplett in die falsche Richtung" -> im ersten Bild einander zugewandt */
    const bild_t *b0 = &s_b[0];
    PRUEF(b0->a.da && b0->m.da && wabs(b0->a.r, g_am) <= KEGEL && wabs(b0->m.r, g_ma) <= KEGEL,
          "[%s] erstes Bild F%d: Ada Gier %d (Peilung auf Marvin %d), Marvin Gier %d (Peilung auf Ada %d) - einander zugewandt",
          quelle, b0->f, b0->a.r, g_am, b0->m.r, g_ma);
    /* Zeilen */
    zeile_t Z[16]; int nz = zeilen(Z, 16);
    printf("  [%s] Zeilen:", quelle); for (int i = 0; i < nz; i++) printf(" %d F%d..F%d", Z[i].id, Z[i].f0, Z[i].f1); printf("\n");
    int z13_ende = -1, z10_anf = -1;
    for (int i = 0; i < nz; i++) { if (Z[i].id == 13) z13_ende = Z[i].f1; if (Z[i].id == 10 && z10_anf < 0) z10_anf = Z[i].f0; }
    PRUEF(nz == 5 && Z[0].id == 10 && Z[1].id == 11 && Z[2].id == 12 && Z[3].id == 13 && Z[4].id == 14,
          "[%s] fuenf Zeilen 10..14 in der Reihenfolge (%d)", quelle, nz);
    /* (2)+(3) Drehungen: Ada keine, Marvin nur der Lauf nach seiner letzten Zeile, keine Kehrtwende */
    dreh_t DA[32], DM[32];
    int na = drehungen(0, DA, 32), nm = drehungen(1, DM, 32);
    int max_a = 0, max_m = 0, m_vor_lauf = 0, a_vor_lauf = 0;
    for (int i = 0; i < na; i++) { printf("  [%s] Ada dreht F%d..F%d %d -> %d (%+d)\n", quelle, DA[i].f0, DA[i].f1, DA[i].g0, DA[i].g1, DA[i].summe);
        if (abs(DA[i].summe) > max_a) max_a = abs(DA[i].summe);
        if (z13_ende < 0 || DA[i].f0 < z13_ende) a_vor_lauf++; }
    for (int i = 0; i < nm; i++) { printf("  [%s] Marvin dreht F%d..F%d %d -> %d (%+d)\n", quelle, DM[i].f0, DM[i].f1, DM[i].g0, DM[i].g1, DM[i].summe);
        if (abs(DM[i].summe) > max_m) max_m = abs(DM[i].summe);
        if (z13_ende >= 0 && DM[i].f0 < z13_ende) m_vor_lauf++; }
    PRUEF(a_vor_lauf == 0, "[%s] Ada dreht den Koerper im ganzen Gespraech nicht (bis Marvins letzte Zeile endet): %d Drehungen "
          "davor - kein 180, kein zweites 90", quelle, a_vor_lauf);
    PRUEF(na <= 1 && max_a < 0x400, "[%s] Nachtrag 12: danach hoechstens EINE Drehung Adas (dem laufenden Marvin nach), %d Einheiten "
          "= %.1f Grad < 90 Grad (%d Abschnitte)", quelle, max_a, max_a * 360.0 / 4096.0, na);
    PRUEF(m_vor_lauf == 0 && z13_ende >= 0,
          "[%s] Marvin dreht den Koerper erst nach dem Ende seiner letzten Zeile (F%d): %d Drehungen davor", quelle, z13_ende, m_vor_lauf);
    PRUEF(max_m < KEIN_180, "[%s] groesste Einzeldrehung Marvins %d Einheiten = %.1f Grad < 120 Grad (Losrennen zur Tuer hinter der Kamera)",
          quelle, max_m, max_m * 360.0 / 4096.0);
    /* (4) je Zeile: der Sprecher sieht den Partner an (Nachtrag 11) - und der Zuhoerer ihn (r36 K2 "Sie muessen sich
     *     schon miteinander unterhalten"); rolle 0 = Sprecher, 1 = Zuhoerer */
    static int blick_a[MAXB], blick_m[MAXB];
    if (mit_kopf) { blicke_rechnen(0, blick_a); blicke_rechnen(1, blick_m); }
    for (int zr = 0; zr < 2 * nz; zr++) {
        int zi = zr / 2, rolle = zr % 2;
        int id = Z[zi].id; if (id < 10 || id > 13) continue;
        int sp_ada = (id == 10) ^ rolle;
        int n = 0, koerper = 0, kopf = 0, nk = 0, kblick = 0, kmax = 0;
        for (int i = 0; i < s_nb; i++) {
            const bild_t *b = &s_b[i];
            if (b->f < Z[zi].f0 || b->f > Z[zi].f1 || !b->a.da || !b->m.da) continue;
            const fig_t *sp = sp_ada ? &b->a : &b->m, *pa = sp_ada ? &b->m : &b->a;
            int gp = re15_s11c0_gier(sp->x, sp->z, pa->x, pa->z);
            n++;
            koerper += (wabs(sp->r, gp) <= KEGEL);
            kopf += partner_ziel(sp, pa) || kopf_geradeaus(sp);
            if (mit_kopf) {
                int bl = sp_ada ? blick_a[i] : blick_m[i];
                if (bl >= 0) { int gh = re15_s11c0_gier(sp->hx, sp->hz, pa->x, pa->z); int d = wabs(bl, gh);
                               nk++; kblick += (d <= KEGEL); if (d > kmax) kmax = d; }
            }
        }
        PRUEF(n > 0 && koerper == n && kopf == n,
              "[%s] msg %d %s %s: Koerper zum Partner (Kegel 0x60) %d/%d, Kopf auf dem Partner/geradeaus %d/%d",
              quelle, id, rolle ? "Zuhoerer" : "Sprecher", sp_ada ? "Ada" : "Marvin", koerper, n, kopf, n);
        if (mit_kopf)
            PRUEF(nk > 0 && kblick == nk, "[%s] msg %d %s %s: gezeichneter Kopf-Blick zum Partner <= 8,4 Grad in %d/%d Bildern (max %.1f Grad)",
                  quelle, id, rolle ? "Zuhoerer" : "Sprecher", sp_ada ? "Ada" : "Marvin", kblick, nk, kmax * 360.0 / 4096.0);
    }
    /* (5) Knall: vor Adas Frage beide Koepfe zur Tuer, der Koerper bleibt */
    int knall_a = 0, knall_m = 0;
    for (int i = 0; i < s_nb; i++) {
        if (z10_anf >= 0 && s_b[i].f >= z10_anf) break;
        knall_a |= tuer_ziel(&s_b[i].a); knall_m |= tuer_ziel(&s_b[i].m);
    }
    PRUEF(knall_a && knall_m, "[%s] Knall: beide Koepfe zur Tuer vor Adas Frage (Ada %d, Marvin %d), Koerper unveraendert (s.o.)",
          quelle, knall_a, knall_m);
    /* (6) msg 14 (Nachtrag 12): Ada hat sich dem laufenden Marvin nachgedreht - im ersten Bild der Zeile ist ihr Koerper ihm
     *     zugewandt (Kegel 0x60), und ihr Kopf-Ziel folgt ihm in jedem Bild (Port-Takt, RE2 Plc_neck Modus 5): Abstand
     *     Zielpunkt - Marvins Ort < 500 (ein Bild Verzug bei ~190 Einheiten Laufschritt je Bild + Kopf-Versatz); mit dem
     *     gezeichneten Kopf: Blick zu Marvin <= 8,4 Grad */
    for (int zi = 0; zi < nz; zi++) {
        if (Z[zi].id != 14) continue;
        int n = 0, folgt = 0, nk = 0, kblick = 0, kmax = 0, erst = -1, koerper0 = 0;
        for (int i = 0; i < s_nb; i++) {
            const bild_t *b = &s_b[i];
            if (b->f < Z[zi].f0 || b->f > Z[zi].f1 || !b->a.da || !b->m.da) continue;
            n++;
            if (erst < 0) { erst = i; koerper0 = wabs(b->a.r, re15_s11c0_gier(b->a.x, b->a.z, b->m.x, b->m.z)); }
            long dx = labs((long)b->a.ntx - b->m.x), dz = labs((long)b->a.ntz - b->m.z);
            folgt += ((b->a.nf & 0x04) && dx < 500 && dz < 500);
            if (mit_kopf && blick_a[i] >= 0) {
                int gm = re15_s11c0_gier(b->a.hx, b->a.hz, b->m.x, b->m.z), d = wabs(blick_a[i], gm);
                nk++; kblick += (d <= KEGEL); if (d > kmax) kmax = d;
            }
        }
        PRUEF(n > 0 && koerper0 <= KEGEL, "[%s] msg 14 'Marvin!...': Ada hat sich ihm nachgedreht - Koerper im ersten Bild %d Einheiten "
              "neben ihm (Kegel 0x60)", quelle, koerper0);
        PRUEF(n > 0 && folgt == n, "[%s] msg 14: Adas Kopf-Ziel folgt dem laufenden Marvin in %d/%d Bildern", quelle, folgt, n);
        if (mit_kopf)
            PRUEF(nk > 0 && kblick == nk, "[%s] msg 14: Adas gezeichneter Blick auf Marvin <= 8,4 Grad in %d/%d Bildern (max %.1f Grad)",
                  quelle, kblick, nk, kmax * 360.0 / 4096.0);
    }
}

/* ================================================================================================================
 * Teil ablauf (Spieltakt)
 * ============================================================================================================== */
static void fig_aus_aktor(fig_t *k, const re15_actor_t *a)
{
    k->da = a->active; k->x = a->x; k->z = a->z; k->r = a->rot_y & 0x0fff; k->mo = a->motion; k->s = a->sub_state_1;
    k->nf = a->neck_flags; k->ntx = a->neck_tx; k->ntz = a->neck_tz; k->hx = a->x; k->hz = a->z; k->tr = 0;
}
static void teil_ablauf(void)
{
    printf("== ablauf ==\n");
    re15_game_state_init(); re15_game_flag_set(9, 71, 1); re15_game_flag_set(3, 94, 1); re15_game_flag_set(9, 73, 1);
    re15_irons_tod_zustand_setzen(RE15_IT_S1030);
    if (room_boot_11c0() != 0) return;
    PRUEF(re15_irons_tod_zustand() == RE15_IT_S11C0, "11C0: Programm 4 laeuft");
    const re15_actor_t *ada = &g_actors[1], *mar = &g_actors[4];
    PRUEF(ada->active && ada->type == 0x42 && mar->active && mar->type == 0x40, "Ada und Marvin nach dem Raumaufbau da");
    s_nb = 0;
    bild_t *b = &s_b[s_nb++]; b->f = 0; b->ma = 0; b->mid = 0;     /* Zustand vor dem ersten Bild */
    fig_aus_aktor(&b->a, ada); fig_aus_aktor(&b->m, mar);
    int f;
    for (f = 1; f < 1500 && s_nb < MAXB; f++) {
        frame();
        if (g_room_change.pending) break;
        b = &s_b[s_nb++]; b->f = f; b->ma = g_scd.message_active ? 1 : 0; b->mid = g_scd.message_id;
        fig_aus_aktor(&b->a, ada); fig_aus_aktor(&b->m, mar);
    }
    bedingungen(0, "Spieltakt");
    long dx = labs((long)mar->x - MAR_X), dz = labs((long)mar->z - MAR_Z);
    PRUEF(dx + dz > 3000, "Marvin ist losgerannt (Weg %ld)", dx + dz);
    PRUEF(g_room_change.pending && g_room_change.room_id == 0x1150 && g_room_change.target_cut == 7,
          "Schnitt zurueck nach ROOM1150 Cut 7 nach %d Bildern", f);
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
}

/* ================================================================================================================
 * Teil auswerten (exe-Protokolle der Integration)
 * ============================================================================================================== */
static int fig_lesen(const char *z, const char *kopf, fig_t *k)
{
    const char *p = strstr(z, kopf);
    memset(k, 0, sizeof *k);
    if (!p) return 0;
    int nf = 0;
    int n = sscanf(p + strlen(kopf), "%d,%d) r=%d mo=%d s=%d nf=%x nt=(%d,%d) acc=%*d gp=%*d h=(%d,%d)",
                   &k->x, &k->z, &k->r, &k->mo, &k->s, (unsigned *)&nf, &k->ntx, &k->ntz, &k->hx, &k->hz);
    k->nf = nf;
    if (n == 8) { k->hx = k->x; k->hz = k->z; }   /* aeltere Protokolle ohne Kopf-Ort (Lauf "vor"): Wurzel-Ort */
    k->da = (n == 10 || n == 8);
    return k->da;
}
typedef struct { int da, tgt, res, acc, kf; } spur_t;
static spur_t s_spur[2][MAXB];
static int s12(int v) { v &= 0x0fff; return v >= 0x800 ? v - 0x1000 : v; }
static void teil_auswerten(const char *dir)
{
    printf("== auswerten %s ==\n", dir);
    char p[1024];
    snprintf(p, sizeof p, "%s/neck_trace.log", dir);
    FILE *t = fopen(p, "r");
    PRUEF(t != NULL, "neck_trace.log vorhanden");
    if (t) {
        char z[512]; int letzt[2] = { -1, -1 };
        while (fgets(z, sizeof z, t)) {
            unsigned fr; int sl, fl, tg, tp, rs, rp, ay, ap, ky, kp;
            if (sscanf(z, "F%u slot=%d fl=%x tgt=(%d,%d) res=(%d,%d) acc=(%d,%d) kf=(%d,%d)",
                       &fr, &sl, (unsigned *)&fl, &tg, &tp, &rs, &rp, &ay, &ap, &ky, &kp) != 11) continue;
            int w = (sl == 1) ? 0 : (sl == 4) ? 1 : -1;
            if (w < 0 || fr >= MAXB) continue;
            if ((int)fr < letzt[w]) memset(s_spur[w], 0, sizeof s_spur[w]);   /* neuer Raumbesuch: nur der letzte zaehlt */
            letzt[w] = (int)fr;
            spur_t *s = &s_spur[w][fr];
            s->da = 1; s->tgt = s12(tg); s->res = s12(rs); s->acc = s12(ay); s->kf = s12(ky);
        }
        fclose(t);
    }
    snprintf(p, sizeof p, "%s/s11c0.log", dir);
    FILE *l = fopen(p, "r");
    PRUEF(l != NULL, "s11c0.log vorhanden");
    if (!l) return;
    char z[1024];
    s_nb = 0;
    while (fgets(z, sizeof z, l) && s_nb < MAXB) {
        unsigned fr; int ma, mid, cam;
        if (sscanf(z, "F%u msg=%d:%d cam=%d", &fr, &ma, &mid, &cam) != 4) continue;
        bild_t *b = &s_b[s_nb];
        b->f = (int)fr; b->ma = ma; b->mid = mid;
        if (!fig_lesen(z, " A(p=(", &b->a) || !fig_lesen(z, " M(p=(", &b->m)) continue;
        /* Mess-Haken (Irons-Takt) laeuft VOR den SCD-Faeden, die Kopf-FSM beim Zeichnen NACH VM + KI: die Zeile F gehoert
         * zum gezeichneten Kopf von Bild F-1 (Dossier "Messung nachher"). */
        for (int w = 0; w < 2; w++) {
            fig_t *k = w ? &b->m : &b->a;
            if (fr >= 1 && fr - 1 < MAXB && s_spur[w][fr - 1].da) {
                const spur_t *s = &s_spur[w][fr - 1];
                k->tr = 1; k->tgt = s->tgt; k->res = s->res; k->acc = s->acc; k->kf = s->kf;
            }
        }
        s_nb++;
    }
    fclose(l);
    bedingungen(1, "exe");
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    re15_game_state_init();
    if (!strcmp(teil, "aufstellung") || !strcmp(teil, "alle")) teil_aufstellung();
    if (!strcmp(teil, "ablauf") || !strcmp(teil, "alle")) teil_ablauf();
    if (!strcmp(teil, "auswerten")) {
        if (argc < 3) { printf("Aufruf: test_r37_szene11c0 auswerten <laufordner>\n"); return 2; }
        teil_auswerten(argv[2]);
    }
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
