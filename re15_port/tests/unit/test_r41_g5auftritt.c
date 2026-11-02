/* test_r41_g5auftritt.c — Riegel G5-Auftritt nach RE2 room7040 sub06 (include/re15_g5auftritt_r41.h,
 * Dossier analysis/befunde_runde41/G5_auftritt.md).
 *
 * 1. DATEN: RE2/R704_CUT09.BSS und .PRI sind die RE2-Bytes (gegen info/re2leon, wenn vorhanden), der Chunk dekodiert mit dem
 *    Port-Decoder (Hintergrund gegen ROOM70409.bmp), der Masken-Atlas mit SLD-Laenge 0xCF40, die Masken 2 Gruppen / 32;
 *    die Szenenkamera ist RID @0x184.
 * 2. ABLAUF in ROOM5090 (Leon) und ROOM5091 (Elza) mit RE2-EM36/EM37 ueber die Schnittstelle re15_bf_g5_auftritt_start, im
 *    Bildtakt wie main.c (Szenenmodus, Walker, Spieler-Takt, Gegner-KI): VORLAUF (M7: Leon laeuft von -3800 nach 1400 ins Bild,
 *    G5 unarmiert geparkt), Armierung, Schnitt 16 bis Bild 436, 17 ab 437, Cut 12 ab 807, Ende 1162; M1 Leon laeuft ab Bild 12 aus
 *    der Grossaufnahme und geht in 17/12 rueckwaerts hinaus; M2 Armgelenke = RE2-RAM s04; M3 G5-Lage = RE2-RAM; M4b keine Spruenge
 *    der gezeichneten Wurzel; M4 Masse hinter der Pfostenmaske; M6 Frau in 16/17 unsichtbar; M5 Xa-Aufrufe; Sperre/Zonen; Kampf.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_room.h"
#include "re15_camera.h"
#include "re15_bss.h"
#include "re15_sld.h"
#include "re15_pri.h"
#include "re15_tim.h"
#include "re15_birkin_formen.h"
#include "re15_g5auftritt_r41.h"
#include "re2_ems.h"
#include "re15_xa.h"
#include "re15_emd.h"
#include "re15_player.h"
#include "re15_math.h"           /* re15_squareroot0 (Speer-Reichweite) */

extern int re15_g5_boss_zustand(int *sub, int *ph, int *timer);
extern int re15_g5_tentakel_zustand(int idx, int *sub, int *ph, int *ankermodus, int32_t *scale_x, uint16_t *kontakt);
extern int re15_g5_tentakel_spitze(int idx, int32_t out[3]);
extern void scd_register_current_rdt(const re15_rdt_t *);
extern int g_test_re2xa_count, g_test_re2xa_lese;   /* tests/test_support.c: Spion re15_audio_re2_xa (Abnahme 1 M5) */
extern const char *g_test_re2xa_datei;

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

#define RE2_DIR   RE15_ASSET_PSX_DIR "/../RE2"
#define INFO_RE2  RE15_ASSET_PSX_DIR "/../../../info/re2leon"

static uint8_t *slurp(const char *path, size_t *out)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)(n > 0 ? n : 1));
    if (b && fread(b, 1, (size_t)n, f) != (size_t)n) { free(b); b = NULL; }
    fclose(f);
    if (b) *out = (size_t)n;
    return b;
}

/* Projektion wie der Zeichner: re15_camera_build_view, sx = 160 + H*x/z, sy = 120 + H*y/z. 1 = im Bild (0..319, 0..239, z > 64). */
static int im_bild(const re15_camera_cut_t *c, int32_t x, int32_t y, int32_t z, int *sx, int *sy)
{
    re15_camera_view_t v;
    if (re15_camera_build_view(c, &v) != 0) return 0;
    int64_t cx = ((int64_t)v.rot[0] * x + (int64_t)v.rot[1] * y + (int64_t)v.rot[2] * z) / 4096 + v.trans[0];
    int64_t cy = ((int64_t)v.rot[3] * x + (int64_t)v.rot[4] * y + (int64_t)v.rot[5] * z) / 4096 + v.trans[1];
    int64_t cz = ((int64_t)v.rot[6] * x + (int64_t)v.rot[7] * y + (int64_t)v.rot[8] * z) / 4096 + v.trans[2];
    if (cz <= 64) return 0;
    int px = 160 + (int)(v.fov_screen_dist * cx / cz), py = 120 + (int)(v.fov_screen_dist * cy / cz);
    if (sx) *sx = px;
    if (sy) *sy = py;
    return px >= 0 && px < 320 && py >= 0 && py < 240;
}

/* ---- 1. Daten ---------------------------------------------------------------------------------------------------------------- */
/* Abnahme 1 M5: R704_XA0B/0C.XAS = die Sektoren des Kanals im Lesefenster von VOICE07.XAS (STAGE7.BIN @0x186/@0x18C). */
static void test_xa_daten(void)
{
    static const struct { const char *datei; int kanal, start, ende, n; } k[2] = {
        { RE15_G5A_XA0B_DATEI, 4, 4, 4 + RE15_G5A_XA0B_LESE, 83 },
        { RE15_G5A_XA0C_DATEI, 1, 1, 1 + RE15_G5A_XA0C_LESE, 106 },
    };
    size_t vn = 0, sn = 0;
    uint8_t *v = slurp(INFO_RE2 "/PL0/VOICE/VOICE07.XAS", &vn);
    uint8_t *st7 = slurp(INFO_RE2 "/COMMON/BIN/STAGE7.BIN", &sn);
    if (st7 && sn > 0x192) {
        CHECK(memcmp(st7 + 0x186, "\x04\x00\x00\x00\x54\x00", 6) == 0 && memcmp(st7 + 0x18C, "\x01\x00\x00\x00\x6b\x00", 6) == 0,
              "STAGE7.BIN @0x186/@0x18C nicht (4,0,84)/(1,0,107)");
        CHECK(84 * 16 - 0x1E == RE15_G5A_XA0B_LESE && 107 * 16 - 0x1E == RE15_G5A_XA0C_LESE, "Lesefenster");
    }
    for (int i = 0; i < 2; i++) {
        char p[256]; size_t n = 0;
        snprintf(p, sizeof p, "%s/%s", RE2_DIR, k[i].datei);
        uint8_t *x = slurp(p, &n);
        CHECK(x && n == (size_t)k[i].n * 2336u, "RE2/%s fehlt oder Laenge %zu statt %d Sektoren", k[i].datei, n, k[i].n);
        if (x && n == (size_t)k[i].n * 2336u) {
            int falsch = 0;
            for (int s = 0; s < k[i].n; s++) {
                const uint8_t *q = x + (size_t)s * 2336u;
                if (q[0] != 1 || q[1] != k[i].kanal || q[2] != 0x64 || q[3] != 0) falsch++;
                const int lage = k[i].start + 16 * s;
                if (lage >= k[i].ende) falsch++;
                if (v && vn == 1904u * 2336u && memcmp(q, v + (size_t)lage * 2336u, 2336) != 0) falsch++;
            }
            CHECK(falsch == 0, "RE2/%s: %d Sektoren nicht VOICE07.XAS Kanal %d im Lesefenster", k[i].datei, falsch, k[i].kanal);
            int hist[2] = { 0, 0 };
            int16_t pcm[4032];
            re15_xa_decode_sector_mono(x + 8, pcm, hist);       /* nur: dekodierbar, kein Absturz (Gegenprobe s. re15_xa.h) */
        }
        free(x);
    }
    free(v); free(st7);
}

static void test_daten(void)
{
    size_t bn = 0, pn = 0;
    uint8_t *bss = slurp(RE2_DIR "/R704_CUT09.BSS", &bn);
    uint8_t *pri = slurp(RE2_DIR "/R704_CUT09.PRI", &pn);
    CHECK(bss && bn == 0x10000, "RE2/R704_CUT09.BSS fehlt oder Laenge %zu", bn);
    CHECK(pri && pn == 340, "RE2/R704_CUT09.PRI fehlt oder Laenge %zu", pn);
    if (!bss || !pri) { free(bss); free(pri); return; }

    {   /* Quelle (nur im Repo, nicht im Paket): Bytes gleich */
        size_t qn = 0, rn = 0;
        uint8_t *q = slurp(INFO_RE2 "/COMMON/BSS/ROOM704.BSS", &qn);
        uint8_t *r = slurp(INFO_RE2 "/PL0/RDT/ROOM7040.RDT", &rn);
        if (q && qn == 15u * 0x10000u) CHECK(memcmp(q + 9 * 0x10000, bss, 0x10000) == 0, "R704_CUT09.BSS != ROOM704.BSS Schnitt 9");
        if (r && rn > 0xF2C) {
            CHECK(memcmp(r + 0xDD8, pri, 340) == 0, "R704_CUT09.PRI != ROOM7040.RDT [0xDD8, 0xF2C)");
            g_current_room_id = 0x5090u;
            static const struct { int schnitt, re2, rid; } kam[2] = { { RE15_G5A_SCHNITT9, 9, 0x184 }, { RE15_G5A_SCHNITT10, 10, 0x1A4 } };
            for (int i = 0; i < 2; i++) {
                const re15_camera_cut_t *k = re15_g5a_schnitt(kam[i].schnitt);
                re15_camera_cut_t rid;
                memcpy(&rid, r + kam[i].rid, sizeof rid);
                CHECK(k && re15_g5a_re2_schnitt(kam[i].schnitt) == kam[i].re2 && k->fov == rid.fov && k->pos_x == rid.pos_x &&
                      k->pos_y == rid.pos_y && k->pos_z == rid.pos_z && k->target_x == rid.target_x && k->target_y == rid.target_y &&
                      k->target_z == rid.target_z, "Szenenkamera %d != RE2 RID @0x%X", kam[i].schnitt, kam[i].rid);
            }
            {   size_t cn = 0;
                uint8_t *c10 = slurp(RE2_DIR "/R704_CUT10.BSS", &cn);
                CHECK(c10 && cn == 0x10000 && q && memcmp(q + 10 * 0x10000, c10, 0x10000) == 0, "R704_CUT10.BSS != ROOM704.BSS Schnitt 10");
                free(c10);
            }
        } else fprintf(stderr, "Hinweis: info/re2leon fehlt, Quellvergleich uebersprungen\n");
        free(q); free(r);
    }

    re15_bss_chunk_t ch;
    CHECK(re15_bss_parse_chunk(bss, bn, &ch) && re15_bss_chunk_has_video(&ch), "RE2-Chunk: kein MDEC-Kopf");
    {   /* Masken-Atlas: RE2 packt mit FUN_80010084, RE1.5s Entpacker (sld_common.c) liest den Strom nicht -> entpacktes TIM.
         * SLD-Laenge 0xCF40 = STAGE7.BIN @0xE6, im Chunk present = 1, Block @0x7A00, entpackt 0x8220. */
        static uint8_t atlas[0x10000];
        int len = 0;
        int rv = re15_sld_atlas_from_chunk(bss, (int)bn, (uint16_t)0xCF40u, atlas, (int)sizeof atlas, &len);
        CHECK(rv < 0, "RE1.5-Entpacker liest den RE2-Strom (rv %d) - dann braucht es R704_CUT09_PRI.TIM nicht", rv);
        size_t tn = 0, rn = 0;
        uint8_t *tim = slurp(RE2_DIR "/R704_CUT09_PRI.TIM", &tn);
        CHECK(tim && tn == 0x8220u, "RE2/R704_CUT09_PRI.TIM fehlt oder Laenge %zu statt 0x8220", tn);
        if (tim) {
            re15_tim_t t;
            CHECK(re15_tim_parse(tim, (int)tn, &t) == 0 && t.bpp == 8 && t.has_clut && t.width == 256 && t.height == 128,
                  "Atlas kein 8-bpp-TIM 256x128 mit CLUT");
            uint8_t *ref = slurp(INFO_RE2 "/COMMON/BSS/ROOM704/ROOM7040009.tim", &rn);
            if (ref) CHECK(rn >= tn && memcmp(ref, tim, tn) == 0, "Atlas != ROOM7040009.tim (fremdes Extraktionswerkzeug)");
            free(ref);
        }
        free(tim);
    }
    {   re15_pri_cut_t p;
        int n = re15_pri_parse_section_re2(pri, pn, 0u, &p);
        CHECK(n == 32 && p.mask_count == 32 && p.draw_count == 32, "RE2-Masken Schnitt 9: %d (gezeichnet %d) statt 32", n, p.draw_count);
        int n15 = re15_pri_parse_section(pri, pn, 0u, &p);
        CHECK(n15 != 32, "RE1.5-Regel liest die RE2-Masken auch richtig (%d) - Sonderweg unnoetig", n15);
        /* erste Maske: Rechteck 32x40 (Feld 0), fuenfte: Quadrat 32 (Feld 0x20) - RE2 FUN_80049b1c @0x80049c18 */
        (void)re15_pri_parse_section_re2(pri, pn, 0u, &p);
        CHECK(p.masks[0].width == 32 && p.masks[0].height == 40 && p.masks[4].width == 32 && p.masks[4].height == 32,
              "RE2-Masken: Groessen %dx%d / %dx%d", p.masks[0].width, p.masks[0].height, p.masks[4].width, p.masks[4].height);
    }
    free(bss); free(pri);
}

/* ---- 2. Ablauf --------------------------------------------------------------------------------------------------------------- */
static re15_rdt_t s_rdt;
static uint8_t *raum_laden(const char *rel, unsigned id)
{
    size_t sz = 0;
    char p[512]; snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/%s", rel);
    uint8_t *buf = slurp(p, &sz);
    if (!buf || re15_rdt_parse(buf, sz, &s_rdt) != 0) return NULL;
    scd_register_current_rdt(NULL);
    g_current_room_id = id;
    re15_enemy_reset();
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 60; i++) scd_vm_tick();
    scd_register_current_rdt(&s_rdt);
    return buf;
}

static uint8_t *s_ems = NULL;
static size_t   s_ems_n = 0;
static int g5_banken(void)
{
    if (!s_ems) s_ems = slurp(RE2_DIR "/CDEMD0.EMS", &s_ems_n);
    if (!s_ems) return 0;
    for (int k = 0x36; k <= 0x37; k++) {
        re15_enemy_bank_t *eb = re15_enemy_find((uint8_t)k);
        if (!eb) eb = re15_enemy_alloc((uint8_t)k);
        if (!eb || re2_ems_load_bank(s_ems, s_ems_n, k, eb, NULL) != 0) return 0;
        eb->buf = NULL; eb->ok = 1;
    }
    return 1;
}

/* ---- Bildtakt wie platform/pc/main.c: Szenen-Spielermodus (main.c cine_active), Walker, Spieler-Takt, Gegner-KI. ---------------- */
static re15_camera_view_t s_cam;
static re15_emd_animation_t s_pl00_anim;
static re15_emd_skeleton_t  s_pl00_skel;
static uint8_t *s_pl00_edd = NULL, *s_pl00_emr = NULL;
static int pl00_banken(void)
{
    size_t en = 0, rn = 0;
    if (s_pl00_edd && s_pl00_emr) return 1;
    s_pl00_edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &en);
    s_pl00_emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &rn);
    if (!s_pl00_edd || !s_pl00_emr) return 0;
    if (re15_emd_parse_animation(s_pl00_edd, en, &s_pl00_anim) != 0 || re15_emd_parse_skeleton(s_pl00_emr, rn, &s_pl00_skel) != 0)
        return 0;
    re15_player_set_pl00_banks(&s_pl00_skel, &s_pl00_anim);
    return 1;
}
static void takt(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->hp = 100; pl->hit_react = 0;
    pl->pos_s_x = (uint16_t)(int32_t)pl->x; pl->pos_s_y = (uint16_t)(int32_t)pl->y; pl->pos_s_z = (uint16_t)(int32_t)pl->z;   /* game_step @0x8001d0d4-e4 */
    if (re15_cine_active()) g_scd.player_mode = 2;          /* main.c: flag(1,27) || flag(2,7) -> Skript */
    re15_actor_step_all_walkers();                          /* main.c: Walker vor dem Spielschritt */
    re15_player_tick(&s_cam, 0);                            /* game_step: Spieler-Takt (Bildvorschub, Halbtakt) */
    re15_enemy_ai_run_all(0);                               /* game_step: Gegner-KI (bf_raumtick -> G5-Szene, G5, Arme) */
}

/* Gezeichnete Wurzel-y des G5 = Teil0+0x30 der gehaltenen Pose (Nachbesserung G5 2: re15_g5_pose_schritt = FUN_80029614 mit den
 * +0x14E-Stufen; vorher das rohe Keyframe-y des Zeichenbilds). 0 = keine Angabe. */
extern int re15_g5_zeichenbild(const re15_actor_t *e, int *clip, int *bild);
extern int re15_g5_tentakel_gelenke(int idx, int32_t out[4][3]);
extern int re15_g5_tentakel_teilwinkel(int idx, int part, int16_t w[3]);
extern int re15_g5_pose_wurzel(int slot, int32_t r[3]);
static int g5_wurzel_y(const re15_actor_t *e, int32_t *y)
{
    int32_t r[3];
    if (!re15_g5_pose_wurzel((int)(e - g_actors), r)) return 0;
    *y = r[1];
    return 1;
}

/* RE2-RAM Speer (Nachbesserung G5 2, Punkt 3), Teil-Winkel rz parts+i*172+108 (Lesewerkzeug G5_belege/nb1/werkzeug/tent_ss3.py):
 *   s12 = sub9 ph6 (Wort 0x00060901): Arm 0 (356,-223,-192,-66), Arm 1 (302,-169,-138,-39)
 *   s13 = sub9 ph7 nach dem Ausrollen tt<10 (Wort 0x00070901): Arm 0 (266,-133,-102,-21), Arm 1 (212,-79,-48,6); Arm 0 |p3-p0| 8170. */
static const int16_t k_re2_speer_s12[2][4] = { { 356, -223, -192, -66 }, { 302, -169, -138, -39 } };
static const int16_t k_re2_speer_s13[2][4] = { { 266, -133, -102, -21 }, { 212, -79, -48, 6 } };
static int16_t winkel12(int v) { v &= 0xfff; return (int16_t)(v >= 2048 ? v - 4096 : v); }

/* Kamera-z eines Weltpunkts in einem Schnitt (re15_camera_build_view, wie im_bild). */
static int64_t kamera_z(const re15_camera_cut_t *c, int32_t x, int32_t y, int32_t z)
{
    re15_camera_view_t v;
    if (re15_camera_build_view(c, &v) != 0) return 0;
    return ((int64_t)v.rot[6] * x + (int64_t)v.rot[7] * y + (int64_t)v.rot[8] * z) / 4096 + v.trans[2];
}

/* RE2-RAM der echten Szene, Savestate s04 ([T1] t=80, Arme in sub8 ph4/ph5, gezeichnet Clip 14/16/15 Bild 29 = kf 776/836/806):
 * Gelenk-Weltlagen parts+i*172+92/96/100 relativ zu Part 0 (Lesewerkzeug analysis/befunde_runde41/G5_belege/abnahme1/werkzeug/re2ss.py,
 * Arm-Zeiger *(0x800cfe20+4i), Parts *(e+0x198)). Arm 1 ist dort noch eingezogen (scaleX 0). */
static const struct { int arm, clip, bild; int32_t d[3][3]; } k_re2_s04[3] = {
    { 0, 14, 29, { { 3622,   530,   801 }, { 6569,    46,  1076 }, { 6288, -1161,   220 } } },
    { 2, 16, 29, { { 3508,  -310, -1281 }, { 6499,  -123, -1248 }, { 6510,  -207,   254 } } },
    { 3, 15, 29, { { 3565,   724,  -895 }, { 6397,  -251,  -998 }, { 6583, -1727,  -774 } } },
};

/* ---- Nachbesserung G5 3: was der ZEICHNER von der Masse bekommt -------------------------------------------------------------------
 * M1 (Abnahme G5 2): im Fenster zeichnete das Spiel die Masse in Schnitt 16/17 ohne Gewicht 1 ("Aufbaeumen"), dieser Test sah es nicht,
 * weil er die EM36-Bank VOR dem Ctor lud. main.c laedt sie erst im Render-Durchgang nach dem KI-Takt, in dem der Typwechsel und der Ctor
 * laufen (gemessen F1558 "Ctor ... bank 0 pose 0"). Der Ablauf unten bildet das nach (banken_wie_main_c) und prueft die Gewichte und
 * die Vertices, die main.c dem Zeichner fuer Teil 2 uebergibt (re15_g5_morph_verts(0x36, nmi 2), main.c "G5-BLOB"), gegen RE2-RAM.
 * RE2-RAM-Gewichte (Teil2+0x88 +12/+16, Blob +0x219; Lesewerkzeug G5_belege/abnahme2/werkzeug/re2_g5_masse.py), Szenenbild aus dem
 * [T1]-Zeitgeber +0x158 (s03 31, s04 80) bzw. der Abnahme-Zuordnung; w0 folgt dem Vorkampf-Puls @0x801044FC (M2), w1 der Intro-Kopplung
 * [T2] @0x80101334-68 / [T6] @0x8010157c-b4 gegen die Ctor-Basis @0x80101200. */
extern int re15_g5_blob_zustand(int *blob, int *ph, int32_t *gewicht0);
static const struct { const char *ss; int bild; int32_t w0, w1; int ph; } k_re2_gewichte[8] = {
    { "s03",  31,  3078,    0, 1 }, { "s04",  80, 10296,    0, 1 }, { "s05", 129,  5796,  566, 2 }, { "s06", 165,  -184, 2895, 7 },
    { "s08", 264,  9440, 2865, 7 }, { "s09", 313, -4168, 2865, 7 }, { "s13", 479,  8000, 2865, 6 }, { "s17", 677,  3152,    0, 7 },
};
/* RE2-RAM Vertexpuffer 0x80132be0 (Mesh 2 nach FUN_8004bf90, 635 Vertices): Bbox aller und jeder 10. Vertex. Erzeugt von
 * analysis/befunde_runde41/G5_belege/nb3/werkzeug/re2_morph_tabelle.py aus den RAM-Abzuegen der Abnahme G5 2. */
static const struct { const char *ss; int bild; int16_t w[4]; int16_t bb[6]; int16_t v[64][3]; } k_re2_masse[4] = {
    { "s06", 165, { -184, 2895, 0, 0 }, { -3600, 3687, -4224, -1327, -2359, 2357 }, {
        {3241,-2300,-903}, {243,-1391,1966}, {-680,-1519,2184}, {-1033,-1741,2315}, {-3600,-2433,2066}, {-2548,-2653,2267}, {-2913,-3227,2036}, {-3264,-3496,-1822},
        {-2169,-2938,-2225}, {-2548,-2310,-2314}, {-1401,-2009,-2356}, {-665,-1710,-2310}, {-286,-1507,-2188}, {-688,-1407,-1941}, {2490,-1980,-257}, {2074,-1332,-403},
        {3069,-1387,0}, {3029,-1598,185}, {2507,-1954,172}, {1847,-1329,1572}, {2986,-1428,1583}, {2983,-1629,1533}, {2478,-1679,1898}, {1216,-1878,2220},
        {-154,-2232,2282}, {3188,-1719,-947}, {1809,-2369,1408}, {623,-2793,2038}, {-2169,-3207,2108}, {1484,-2732,1310}, {-945,-3222,1889}, {1697,-2683,563},
        {-135,-3386,1610}, {1775,-2485,155}, {57,-3411,1294}, {-3600,-4224,1067}, {-72,-3366,887}, {-1103,-3835,769}, {1545,-2875,132}, {-761,-3681,-644},
        {1682,-2711,-87}, {-741,-3676,-941}, {1687,-2695,-169}, {-302,-3416,-1272}, {3361,-1392,-1236}, {611,-3332,-1390}, {-2171,-3445,-1940}, {1286,-2936,-1190},
        {-408,-2937,-1994}, {1820,-2522,-426}, {1414,-2655,-1618}, {-15,-2543,-2170}, {1919,-2368,-244}, {2228,-2236,-1170}, {1876,-1871,-2078}, {807,-1657,-2257},
        {925,-1476,-2159}, {991,-3140,1379}, {2056,-2879,-214}, {1973,-2413,-900}, {2966,-2253,-642}, {2373,-2042,191}, {2916,-2226,-124}, {3526,-1883,-342},
    } },
    { "s09", 313, { -4168, 2865, 0, 0 }, { -3601, 3677, -4224, -1313, -2372, 2369 }, {
        {3232,-2241,-1021}, {197,-1377,1977}, {-716,-1506,2196}, {-1068,-1730,2328}, {-3601,-2428,2077}, {-2554,-2649,2280}, {-2915,-3226,2046}, {-3266,-3497,-1830},
        {-2184,-2935,-2237}, {-2554,-2304,-2328}, {-1434,-2000,-2370}, {-702,-1699,-2323}, {-325,-1494,-2201}, {-724,-1394,-1952}, {2504,-1934,-272}, {2019,-1318,-404},
        {3038,-1366,0}, {3010,-1552,194}, {2520,-1909,182}, {1797,-1315,1581}, {2966,-1404,1616}, {2970,-1583,1611}, {2467,-1652,1949}, {1180,-1867,2235},
        {-195,-2225,2295}, {3176,-1653,-1016}, {1789,-2347,1496}, {596,-2788,2049}, {-2182,-3206,2120}, {1468,-2731,1391}, {-972,-3220,1898}, {1685,-2683,609},
        {-145,-3385,1617}, {1829,-2488,165}, {52,-3410,1298}, {-3601,-4224,1067}, {-80,-3365,889}, {-1103,-3835,770}, {1545,-2890,141}, {-762,-3681,-644},
        {1714,-2716,-93}, {-744,-3676,-943}, {1717,-2698,-181}, {-310,-3414,-1276}, {3342,-1369,-1259}, {611,-3331,-1397}, {-2181,-3445,-1949}, {1277,-2947,-1251},
        {-444,-2934,-2004}, {1838,-2511,-458}, {1394,-2650,-1707}, {-54,-2536,-2183}, {1967,-2360,-259}, {2216,-2195,-1263}, {1866,-1859,-2098}, {753,-1646,-2269},
        {872,-1463,-2171}, {989,-3154,1424}, {2094,-2884,-252}, {1955,-2387,-976}, {2957,-2187,-731}, {2386,-2000,203}, {2953,-2148,-144}, {3515,-1795,-381},
    } },
    { "s13", 479, { 8000, 2865, 0, 0 }, { -3601, 3677, -4224, -1313, -2372, 2370 }, {
        {3232,-2389,-678}, {332,-1377,1977}, {-606,-1506,2196}, {-963,-1730,2328}, {-3601,-2428,2077}, {-2539,-2649,2280}, {-2906,-3226,2046}, {-3262,-3497,-1830},
        {-2139,-2935,-2237}, {-2539,-2304,-2327}, {-1334,-2000,-2370}, {-591,-1699,-2323}, {-206,-1494,-2201}, {-614,-1394,-1952}, {2429,-2041,-227}, {2153,-1318,-404},
        {3098,-1389,0}, {3037,-1656,170}, {2446,-2015,153}, {1928,-1315,1581}, {3005,-1437,1542}, {2986,-1683,1400}, {2483,-1698,1825}, {1281,-1868,2231},
        {-72,-2225,2295}, {3181,-1818,-818}, {1832,-2393,1250}, {676,-2788,2049}, {-2143,-3206,2120}, {1509,-2715,1166}, {-891,-3220,1899}, {1707,-2668,480},
        {-116,-3385,1617}, {1642,-2459,139}, {70,-3409,1298}, {-3601,-4224,1067}, {-53,-3365,889}, {-1103,-3835,769}, {1534,-2832,115}, {-762,-3681,-644},
        {1598,-2686,-76}, {-732,-3676,-942}, {1609,-2672,-148}, {-283,-3414,-1276}, {3371,-1399,-1210}, {611,-3328,-1391}, {-2150,-3445,-1949}, {1298,-2899,-1081},
        {-334,-2934,-2004}, {1764,-2526,-370}, {1446,-2649,-1462}, {64,-2536,-2182}, {1796,-2360,-217}, {2231,-2298,-1001}, {1887,-1865,-2076}, {908,-1646,-2269},
        {1023,-1463,-2171}, {990,-3103,1301}, {1961,-2840,-141}, {1991,-2442,-760}, {2957,-2359,-471}, {2313,-2098,169}, {2810,-2353,-83}, {3515,-2029,-266},
    } },
    { "s17", 677, { 3152, 0, 0, 0 }, { -3600, 2693, -4224, 23, -3648, 3646 }, {
        {2424,-1422,-1092}, {200,-79,3042}, {-583,-299,3379}, {-941,-681,3583}, {-3600,-1943,3196}, {-2535,-2260,3505}, {-2905,-3144,3046}, {-3260,-3565,-2575},
        {-2130,-2679,-3420}, {-2536,-1697,-3579}, {-1314,-1161,-3645}, {-568,-617,-3571}, {-213,-273,-3385}, {-590,-111,-3003}, {1414,-1109,-355}, {1245,16,-621},
        {2110,-94,0}, {2054,-502,266}, {1418,-1067,241}, {1305,19,2432}, {2263,-167,2383}, {2234,-546,2185}, {1973,-577,2827}, {1047,-854,3433},
        {-74,-1448,3531}, {2219,-748,-1288}, {1357,-1689,1960}, {635,-2322,3134}, {-2135,-3077,3188}, {1200,-2227,1788}, {-875,-3042,2790}, {1218,-2156,749},
        {-109,-3261,2225}, {907,-1814,218}, {74,-3294,1680}, {-3600,-4224,1068}, {-48,-3257,1155}, {-1103,-3835,770}, {1134,-2427,170}, {-762,-3681,-644},
        {1014,-2188,-116}, {-729,-3659,-1176}, {1024,-2164,-228}, {-278,-3313,-1648}, {2401,-108,-1868}, {610,-3162,-1823}, {-2143,-3428,-2830}, {1087,-2520,-1559},
        {-313,-2582,-3033}, {1115,-1918,-582}, {1196,-2114,-2282}, {60,-1931,-3356}, {982,-1644,-340}, {1554,-1514,-1578}, {1521,-843,-3195}, {735,-504,-3489},
        {788,-214,-3339}, {916,-2826,1802}, {1401,-2072,-232}, {1395,-1766,-1201}, {2110,-1385,-763}, {1313,-1199,266}, {1875,-1379,-137}, {2543,-943,-425},
    } },
};
/* Bank-Reihenfolge wie main.c: die RE2-Banken fehlen im Takt des Typwechsels (re15_enemy_find == NULL, wie "bank 0" im Fensterlauf) und
 * sind ab dem naechsten Bild da (pc_enemy_load_ex im Render-Durchgang desselben Bilds). */
static void banken_wie_main_c(int da)
{
    for (int k = 0x36; k <= 0x37; k++)
        for (int i = 0; i < RE15_ENEMY_MAX; i++)
            if (g_enemy[i].type == (uint8_t)k) g_enemy[i].ok = (uint8_t)(da ? 1 : 0);
}
/* Vergleich der Zeichner-Vertices (Teil 2) mit RE2-RAM: max. Abweichung je Koordinate ueber die 64 Stichproben und die Bbox. */
static int masse_abweichung(int k, int *bb_abw)
{
    const re15_md1_vertex_t *v = re15_g5_morph_verts(0x36u, 2);   /* main.c: nbi 2 -> nmi 2 (gemessen, Sonde RE15_G5_MORPH_SONDE) */
    const re15_enemy_bank_t *eb = re15_enemy_find(0x36u);
    int abw = 0, n;
    *bb_abw = 99999;
    if (!v || !eb || !eb->morph_ok) return 99999;
    n = (int)eb->morph[2].nverts;
    if (n != 635) return 99999;
    for (int i = 0; i < 64; i++)
        for (int a = 0; a < 3; a++) {
            const int16_t p = a == 0 ? v[i * 10].x : (a == 1 ? v[i * 10].y : v[i * 10].z);
            int d = (int)p - (int)k_re2_masse[k].v[i][a];
            if (d < 0) d = -d;
            if (d > abw) abw = d;
        }
    {   int bb[6] = { 32767, -32768, 32767, -32768, 32767, -32768 };
        for (int i = 0; i < n; i++) {
            const int c[3] = { v[i].x, v[i].y, v[i].z };
            for (int a = 0; a < 3; a++) { if (c[a] < bb[2 * a]) bb[2 * a] = c[a]; if (c[a] > bb[2 * a + 1]) bb[2 * a + 1] = c[a]; }
        }
        *bb_abw = 0;
        for (int j = 0; j < 6; j++) { int d = bb[j] - (int)k_re2_masse[k].bb[j]; if (d < 0) d = -d; if (d > *bb_abw) *bb_abw = d; }
    }
    return abw;
}

static void test_ablauf(const char *rdt, unsigned raum)
{
    uint8_t *buf = raum_laden(rdt, raum);
    CHECK(buf != NULL, "%s nicht ladbar", rdt);
    if (!buf) return;
    re15_g5a_reset();
    int bs = -1, frau = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (g_actors[s].active && g_actors[s].type == 0x30u) bs = s;
        if (g_actors[s].active && g_actors[s].type == 0x4Du) frau = s;
    }
    CHECK(bs > 0, "%04X: kein 0x30-Spawn (sub00 @0x0124A)", raum);
    CHECK(g5_banken(), "RE2-Banken EM36/EM37");
    CHECK(pl00_banken(), "PL00.EDD/EMR (Rueckwaertsgang, Wurzelspur)");
    if (bs <= 0) { free(buf); return; }
    if (raum == 0x5090u) CHECK(frau > 0, "5090: keine Frau im Kittel (sub00 @0x0122A Typ 0x4D)");
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER], *e = &g_actors[bs];
    pl->active = 1; pl->hp = 100; pl->x = 9000; pl->z = -23400; pl->state = 1;   /* wo Leon in der Rueckstoss-Szene stehen kann */
    e->x = -6283; e->z = -23482; e->no_draw = 0;                                /* hinter der Tuer, wo die Form verschwindet */
    re15_game_flag_set(1, 27, 0); re15_game_flag_set(2, 7, 0);
    g_scd.cut_auto_enabled = 1; g_scd.player_mode = 0;
    memset(&s_cam, 0, sizeof s_cam);
    const re15_camera_cut_t *k16 = re15_g5a_schnitt(RE15_G5A_SCHNITT9), *k17 = re15_g5a_schnitt(RE15_G5A_SCHNITT10);
    const re15_camera_cut_t *k12 = (s_rdt.cuts && s_rdt.cut_count > 12) ? &s_rdt.cuts[12] : NULL;
    CHECK(k16 && k17 && k12, "%04X: Szenenkameras/Cut 12 fehlen", raum);
    if (!k16 || !k17 || !k12) { free(buf); return; }

    banken_wie_main_c(0);                                         /* NB3 M1: Bank fehlt im Takt des Typwechsels (main.c) */
    re15_bf_g5_auftritt_start(bs);                                /* die SCHNITTSTELLE */
    /* ---- VORLAUF (M7): RE2 sub06 @0x0187C..@0x018D6 ---- */
    CHECK(re15_g5a_laeuft() && re15_g5a_bild() == -1 && re15_g5a_vorlauf() == 0, "%04X: Szene startet nicht mit dem Vorlauf", raum);
    CHECK(g_scd.cam_id == RE15_G5A_SCHNITT && !g_scd.cut_auto_enabled, "%04X: Vorlauf nicht Schnitt 16 mit Zonen aus (cam %d auto %d)",
          raum, g_scd.cam_id, g_scd.cut_auto_enabled);
    CHECK(re15_game_flag_get(1, 27) && re15_game_flag_get(2, 7), "%04X: Sperre (1,27)/(2,7) fehlt (@0x01778/@0x0177C)", raum);
    CHECK(pl->x == -3800 && pl->z == -23570 && pl->rot_y == 0, "%04X: Spieler (%d,%d) Gier %d statt Pos_set(-3800) @0x0187C / Dir_set 0 @0x01884",
          raum, (int)pl->x, (int)pl->z, pl->rot_y);
    CHECK(e->type == 0x36u && e->grid_id != 0x13u && e->x == -32000, "%04X: G5 im Vorlauf nicht unarmiert geparkt (Typ 0x%02X grid 0x%02X x %d, @0x011AC)",
          raum, e->type, e->grid_id, (int)e->x);
    int vl = 0, vl_armiert = 0, vl_frau_sichtbar = 0, vl_spieler_im_bild = 0;
    for (vl = 1; vl <= 200 && re15_g5a_bild() < 0; vl++) {
        takt();
        if (vl == 1) banken_wie_main_c(1);                        /* ... und ist ab dem naechsten Bild geladen */
        if (re15_g5a_bild() < 0) {
            if (e->grid_id == 0x13u || e->x != -32000) vl_armiert++;
            if (g_scd.cam_id != RE15_G5A_SCHNITT) vl_armiert++;
        }
        if (frau > 0 && !g_actors[frau].no_draw) vl_frau_sichtbar++;
        if (im_bild(k16, pl->x, -1600, pl->z, NULL, NULL)) vl_spieler_im_bild++;
    }
    fprintf(stderr, "%04X: Vorlauf %d Bilder (RE2 61,47 -> 63,80 s = 70), Spieler bei der Armierung x %d, im Bild %d Vorlaufbilder\n",
            raum, vl - 1, (int)pl->x, vl_spieler_im_bild);
    CHECK(re15_g5a_bild() == 0 && e->grid_id == 0x13u, "%04X: keine Armierung nach dem Vorlauf (Bild %d grid 0x%02X)", raum, re15_g5a_bild(), e->grid_id);
    CHECK(vl - 1 >= 45 && vl - 1 <= 80, "%04X: Vorlauf %d Bilder statt Sleep 1+20 + Lauf -3800 -> 1400 + Sleep 11 (RE2 ~70)", raum, vl - 1);
    CHECK(vl_armiert == 0, "%04X: G5 im Vorlauf %d Bilder armiert/ausgeparkt oder falscher Schnitt", raum, vl_armiert);
    CHECK(pl->x >= 1100 && pl->x <= 1700 && vl_spieler_im_bild > 20, "%04X: Spieler kommt nicht ins Bild nach 1400 (x %d, %d Bilder im Bild)",
          raum, (int)pl->x, vl_spieler_im_bild);
    CHECK(vl_frau_sichtbar == 0, "%04X: Frau im Kittel %d Vorlaufbilder sichtbar (M6)", raum, vl_frau_sichtbar);

    /* ---- SZENE ab der Armierung (Bild 0 ist schon gelaufen) ---- */
    FILE *spur = NULL;
    {   const char *sp = getenv("RE15_G5A_SPUR");   /* Messschiene: <Pfad>_<Raum>.txt */
        if (sp && *sp) { char pf[600]; snprintf(pf, sizeof pf, "%s_%04X.txt", sp, raum); spur = fopen(pf, "w"); } }
    int bild_tent = -1, bild_zug = -1, bild_tuer = -1, tent_im_bild = 0, g5_im_bild = 0;
    int falsch_schnitt = 0, zonen_an = 0, frei_vorher = 0, ende = -1;
    int letzt16 = -1, im16_spaet = 0, im17_437 = 0, letzt17 = -1, im12_807 = 0, letzt12 = -1;
    int frau_sichtbar_re2 = 0, frau_ok_cut12 = 1, sprung_max = 0, sprung_bild = -1, pfosten_vor = 0, pfosten_n = 0;
    int32_t g5x_129 = 0, g5x_270 = 0, g5x_800 = 0, plx_437 = 0, plx_806 = 0, plx_807 = 0, plx_1161 = 0, y_alt = 0;
    int y_alt_ok = 0, arme_ok = 0, arme_n = 0;
    int sprung_kampf = 0, sprung_kampf_bild = -1;
    int speer_ph6[2] = { 0, 0 }, speer_ph7_n[2] = { 0, 0 }, speer_ok6[2] = { 0, 0 }, speer_ok7[2] = { 0, 0 };
    int16_t speer_w6[2][4] = { { 0 } }, speer_w7[2][4] = { { 0 } };
    int32_t speer_laenge = -1;
    int32_t arme_abw_max = 0;
    int32_t gew_ist[8][2] = { { 0 } };
    int gew_ph[8] = { 0 }, gew_ok0 = 0, gew_ok1 = 0;
    int masse_abw[4] = { 99999, 99999, 99999, 99999 }, masse_bb_abw[4] = { 99999, 99999, 99999, 99999 };
    int32_t x_alt = e->x;
    const int xa_n0 = g_test_re2xa_count;
    int xa_alt = xa_n0, xa_bild[2] = { -1, -1 }, xa_lese[2] = { 0, 0 };
    const char *xa_datei[2] = { "", "" };
    int16_t cam_bei[3] = { -1, -1, -1 };
    int32_t pl_x_bei[3] = { 0, 0, 0 };
    const float pfosten_schluessel = re15_pri_mask_camera_z_re2(100);   /* linker Pfosten: RE2-Masken depth 100 (M4) */
    for (int b = 0; b <= RE15_G5A_BILD_ENDE + 40; b++) {    /* b = Szenenbild dieses KI-Durchlaufs (Bild 0 = Armierung, [T0]) */
        if (b > 0) takt();
        CHECK(b >= RE15_G5A_BILD_ENDE || re15_g5a_bild() == b, "%04X: Szenenbild %d statt %d", raum, re15_g5a_bild(), b);
        if (g_test_re2xa_count != xa_alt) {
            const int k = g_test_re2xa_count - xa_n0 - 1;
            if (k >= 0 && k < 2) { xa_bild[k] = b; xa_lese[k] = g_test_re2xa_lese; xa_datei[k] = g_test_re2xa_datei; }
            xa_alt = g_test_re2xa_count;
        }
        if (spur) {   /* MESSSCHIENE (kein Verhalten): Lagen je Bild */
            int32_t sp[4][3] = { { 0 } };
            int32_t scx[4] = { 0 };
            for (int i = 0; i < 4; i++) {
                (void)re15_g5_tentakel_zustand(i, NULL, NULL, NULL, &scx[i], NULL);
                if (re15_g5_tentakel_spitze(i, sp[i]) != 0) sp[i][0] = sp[i][1] = sp[i][2] = 0;
            }
            int gs = -1, gp = -1, zc = -1, zb = -1; int32_t wy = 0;
            (void)re15_g5_boss_zustand(&gs, &gp, NULL);
            (void)re15_g5_zeichenbild(e, &zc, &zb); (void)g5_wurzel_y(e, &wy);
            fprintf(spur, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", b, (int)g_scd.cam_id, (int)pl->x, (int)pl->z, (int)e->x, (int)e->z,
                    (int)e->motion, (int)e->anim_frame, gs, gp, (int)e->rot_y, zc, zb, (int)wy, (int)pl->motion);
            for (int i = 0; i < 4; i++) fprintf(spur, " %d %d %d %d", (int)scx[i], (int)sp[i][0], (int)sp[i][1], (int)sp[i][2]);
            for (int i = 0; i < 4; i++) { int32_t g[4][3]; int su = -1, ph = -1;   /* Gelenke p0/p3 (RE2 parts+92.., s13 = Bild ~472) */
                (void)re15_g5_tentakel_zustand(i, &su, &ph, NULL, NULL, NULL);
                if (re15_g5_tentakel_gelenke(i, g) == 0) fprintf(spur, " | a%d s%d p%d p0 %d %d %d p3 %d %d %d", i, su, ph, (int)g[0][0], (int)g[0][1], (int)g[0][2], (int)g[3][0], (int)g[3][1], (int)g[3][2]); }
            fprintf(spur, "\n");
        }
        if (b < RE15_G5A_BILD_ENDE) {
            int soll = b < RE15_G5A_BILD_SCHNITT10 ? RE15_G5A_SCHNITT9 : (b < RE15_G5A_BILD_SCHNITT5 ? RE15_G5A_SCHNITT10 : 0x0C);
            if (g_scd.cam_id != soll) falsch_schnitt++;
            if (g_scd.cut_auto_enabled) zonen_an++;
            if (!re15_game_flag_get(1, 27) || !re15_game_flag_get(2, 7)) frei_vorher++;
        } else if (ende < 0 && !re15_g5a_laeuft()) ende = b;
        /* M1: Spieler im Bild der jeweiligen Einstellung (Kopfhoehe -1600) */
        if (b < RE15_G5A_BILD_SCHNITT10) {
            if (im_bild(k16, pl->x, -1600, pl->z, NULL, NULL)) { letzt16 = b; if (b >= 45) im16_spaet++; }
        } else if (b < RE15_G5A_BILD_SCHNITT5) {
            if (im_bild(k17, pl->x, -1600, pl->z, NULL, NULL)) { letzt17 = b; if (b == RE15_G5A_BILD_SCHNITT10) im17_437 = 1; }
        } else if (b < RE15_G5A_BILD_ENDE) {
            if (im_bild(k12, pl->x, -1600, pl->z, NULL, NULL)) { letzt12 = b; if (b == RE15_G5A_BILD_SCHNITT5) im12_807 = 1; }
        }
        if (b == RE15_G5A_BILD_SCHNITT10) plx_437 = pl->x;
        if (b == RE15_G5A_BILD_SCHNITT5 - 1) plx_806 = pl->x;
        if (b == RE15_G5A_BILD_SCHNITT5) plx_807 = pl->x;
        if (b == RE15_G5A_BILD_ENDE - 1) plx_1161 = pl->x;
        /* M6: Frau im Kittel in 16/17 unsichtbar, ab Cut 12 wie vorher (sichtbar) */
        if (frau > 0) {
            if (b < RE15_G5A_BILD_SCHNITT5 && !g_actors[frau].no_draw) frau_sichtbar_re2++;
            if (b >= RE15_G5A_BILD_SCHNITT5 && g_actors[frau].active && g_actors[frau].no_draw) frau_ok_cut12 = 0;
        }
        /* M3: G5-Lage gegen RE2-RAM (s05 +0x14D 39 -> -9525; Clipende [T2] -2786; [T6]-Ende 1160) */
        if (b == 129) g5x_129 = e->x;
        if (b == 270) g5x_270 = e->x;
        if (b == 800) g5x_800 = e->x;
        /* NB3 M1/M2: Gewichte gegen RE2-RAM, Zeichner-Vertices der Masse gegen den RE2-Vertexpuffer */
        for (int k = 0; k < 8; k++) {
            if (b != k_re2_gewichte[k].bild) continue;
            int32_t w[4] = { 0, 0, 0, 0 }; int gu = 0, gm = -1, w1ok = 0, blob = -1, bph = -1;
            (void)re15_g5_morph_zustand(w, &gu, &gm, &w1ok);
            (void)re15_g5_blob_zustand(&blob, &bph, NULL);
            gew_ist[k][0] = w[0]; gew_ist[k][1] = w[1]; gew_ph[k] = bph;
            if (w[0] == k_re2_gewichte[k].w0 && bph == k_re2_gewichte[k].ph) gew_ok0++;
            if (w[1] == k_re2_gewichte[k].w1) gew_ok1++;
        }
        for (int k = 0; k < 4; k++) {
            if (b != k_re2_masse[k].bild) continue;
            masse_abw[k] = masse_abweichung(k, &masse_bb_abw[k]);
        }
        /* M4b: gezeichnete Wurzel-y je Bild ohne Sprung - im Intro (Sub 2) UND ueber den Uebergang in den Kampf (Nachbesserung G5 2,
         * Punkt 4: [T14] Clip 0 -> sub1 Clip-Wort 0x1F0005 @0x80100dd8 mischt 31 Stufen mit a3 128 @0x80100fac; vorher 125 bei Bild 1123). */
        {   int32_t y = 0; int gsub = -1;
            (void)re15_g5_boss_zustand(&gsub, NULL, NULL);
            if (g5_wurzel_y(e, &y)) {
                if (y_alt_ok) {
                    int d = (int)(y - y_alt); if (d < 0) d = -d;
                    if (gsub == 2 && d > sprung_max) { sprung_max = d; sprung_bild = b; }
                    if (gsub != 2 && d > sprung_kampf) { sprung_kampf = d; sprung_kampf_bild = b; }
                }
                y_alt = y; y_alt_ok = 1;
            } else y_alt_ok = 0; }
        /* M5 Speer (Punkt 3): Teil-Winkel der Arme 0/1 beim Eintritt in sub9 ph6 (= RE2 s12) und nach 20 Bildern ph7 (= s13) */
        for (int k = 0; k < 2; k++) {
            int sub = -1, ph = -1; int16_t w[3];
            if (re15_g5_tentakel_zustand(k, &sub, &ph, NULL, NULL, NULL) != 0 || sub != 9) continue;
            if (ph == 6 && !speer_ph6[k]) {
                speer_ph6[k] = 1; speer_ok6[k] = 1;
                for (int pp = 0; pp < 4; pp++) {
                    if (re15_g5_tentakel_teilwinkel(k, pp, w) != 0) { speer_ok6[k] = 0; continue; }
                    speer_w6[k][pp] = winkel12(w[2]);
                    if (speer_w6[k][pp] != k_re2_speer_s12[k][pp]) speer_ok6[k] = 0;
                }
            }
            if (ph == 7 && ++speer_ph7_n[k] == 20) {
                speer_ok7[k] = 1;
                for (int pp = 0; pp < 4; pp++) {
                    if (re15_g5_tentakel_teilwinkel(k, pp, w) != 0) { speer_ok7[k] = 0; continue; }
                    speer_w7[k][pp] = winkel12(w[2]);
                    if (speer_w7[k][pp] != k_re2_speer_s13[k][pp]) speer_ok7[k] = 0;
                }
                if (k == 0) { int32_t g[4][3];
                    if (re15_g5_tentakel_gelenke(0, g) == 0) {
                        int64_t dx = g[3][0] - g[0][0], dy = g[3][1] - g[0][1], dz = g[3][2] - g[0][2];
                        speer_laenge = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dy * dy + dz * dz));
                    } }
            }
        }
        /* M2: Gelenklagen der Arme gegen RE2 s04 (Arme gehalten: sub8 ph4..ph7, Bild 80..120) */
        if (b >= 80 && b <= 120) {
            for (int k = 0; k < 3; k++) {
                int sub = -1, ph = -1; int32_t g[4][3];
                if (re15_g5_tentakel_zustand(k_re2_s04[k].arm, &sub, &ph, NULL, NULL, NULL) != 0 || sub != 8 || ph < 4 || ph > 7) continue;
                if (re15_g5_tentakel_gelenke(k_re2_s04[k].arm, g) != 0) continue;
                arme_n++;
                int32_t abw = 0;
                for (int p = 1; p < 4; p++)
                    for (int a = 0; a < 3; a++) {
                        int32_t d = (g[p][a] - g[0][a]) - k_re2_s04[k].d[p - 1][a];
                        if (d < 0) d = -d;
                        if (d > abw) abw = d;
                    }
                if (abw > arme_abw_max) arme_abw_max = abw;
                if (abw <= 250) arme_ok++;
            }
        }
        /* M4: die Masse liegt hinter dem linken Pfosten (RE2-Regel: Maske depth 100 verdeckt ab vz >= 100*32768/1023) */
        if (b >= 100 && b < RE15_G5A_BILD_SCHNITT10) {
            int sx = 0;
            if (im_bild(k16, e->x + 4494, -1500, e->z, &sx, NULL) && sx < 64) {
                pfosten_n++;
                if ((float)kamera_z(k16, e->x + 4494, -1500, e->z) >= pfosten_schluessel) pfosten_vor++;
            }
        }
        if (b == RE15_G5A_BILD_SCHNITT10) { cam_bei[0] = g_scd.cam_id; pl_x_bei[0] = pl->x; }
        if (b == RE15_G5A_BILD_SCHNITT5)  { cam_bei[1] = g_scd.cam_id; pl_x_bei[1] = pl->x; }
        if (b == RE15_G5A_BILD_ENDE)      { cam_bei[2] = g_scd.cam_id; pl_x_bei[2] = pl->x; }
        for (int i = 0; i < 4; i++) {
            int32_t sc = 0;
            if (re15_g5_tentakel_zustand(i, NULL, NULL, NULL, &sc, NULL) == 0 && sc > 0 && bild_tent < 0) bild_tent = b;
            int32_t sp[3];
            if (b < RE15_G5A_BILD_SCHNITT10 && sc > 2048 && re15_g5_tentakel_spitze(i, sp) == 0 &&
                im_bild(k16, sp[0], sp[1], sp[2], NULL, NULL)) tent_im_bild++;
        }
        if (e->x > x_alt + 4 && bild_zug < 0 && b > 1) bild_zug = b;
        if (e->x > -5683 && bild_tuer < 0) bild_tuer = b;
        if (b < RE15_G5A_BILD_SCHNITT10 && im_bild(k16, e->x + 4494, -2000, e->z, NULL, NULL)) g5_im_bild++;
        x_alt = e->x;
    }
    int sub = -1, ph = -1;
    (void)re15_g5_boss_zustand(&sub, &ph, NULL);
    fprintf(stderr, "%04X: Tentakel ab Bild %d, Hereinziehen ab %d, Ursprung durch die Tuer bei %d, Tentakel-Spitzen im Bild %d, "
                    "G5-Front im Bild %d, Ende %d, G5 Sub %d Ph %d x %d, Spieler x %d Cut %d\n",
            raum, bild_tent, bild_zug, bild_tuer, tent_im_bild, g5_im_bild, ende, sub, ph, (int)e->x, (int)pl->x, g_scd.cam_id);
    fprintf(stderr, "%04X: M1 Spieler zuletzt im Bild 16 bei Bild %d, 17 bis %d, 12 bis %d; x 437 %d / 806 %d / 807 %d / 1161 %d\n",
            raum, letzt16, letzt17, letzt12, (int)plx_437, (int)plx_806, (int)plx_807, (int)plx_1161);
    fprintf(stderr, "%04X: M2 Armvergleiche %d, davon <= 250 %d, max Abweichung %d; M3 G5 x Bild 129 %d / 270 %d / 800 %d; "
                    "M4b groesster Wurzel-y-Schritt %d bei Bild %d; M4 Pfosten %d von %d\n",
            raum, arme_n, arme_ok, (int)arme_abw_max, (int)g5x_129, (int)g5x_270, (int)g5x_800, sprung_max, sprung_bild, pfosten_vor, pfosten_n);
    CHECK(falsch_schnitt == 0, "%04X: %d Bilder im falschen Schnitt (16 / 17 ab 437 / 12 ab 807)", raum, falsch_schnitt);
    CHECK(g_test_re2xa_count - xa_n0 == 2, "%04X: %d RE2-Xa_on statt 2 (@0x01920, @0x01946)", raum, g_test_re2xa_count - xa_n0);
    CHECK(xa_bild[0] == RE15_G5A_BILD_XA0B && strcmp(xa_datei[0], RE15_G5A_XA0B_DATEI) == 0 && xa_lese[0] == RE15_G5A_XA0B_LESE,
          "%04X: Xa 0x0B bei Bild %d (%s, %d) statt 132 (@0x01920)", raum, xa_bild[0], xa_datei[0], xa_lese[0]);
    CHECK(xa_bild[1] == RE15_G5A_BILD_XA0C && strcmp(xa_datei[1], RE15_G5A_XA0C_DATEI) == 0 && xa_lese[1] == RE15_G5A_XA0C_LESE,
          "%04X: Xa 0x0C bei Bild %d (%s, %d) statt 438 (@0x01946)", raum, xa_bild[1], xa_datei[1], xa_lese[1]);
    CHECK(zonen_an == 0, "%04X: Zonen in %d Szenenbildern an (Cut_auto(0) @0x01780 bis @0x01981)", raum, zonen_an);
    CHECK(frei_vorher == 0, "%04X: Sperre in %d Bildern vor dem Ende frei", raum, frei_vorher);
    CHECK(bild_tent > 0 && bild_tent <= 12, "%04X: erste Tentakel bei Bild %d statt [T1] t=10 (@0x80101240)", raum, bild_tent);
    CHECK(bild_zug > bild_tent && bild_zug >= 91 && bild_zug <= 120, "%04X: Hereinziehen bei Bild %d (Tentakel %d), soll nach den Tentakeln mit [T2] ab t=91",
          raum, bild_zug, bild_tent);
    CHECK(bild_tuer > bild_zug && bild_tuer < RE15_G5A_BILD_SCHNITT10, "%04X: G5 kommt in Schnitt 16 nicht durch die Tuer (Bild %d)", raum, bild_tuer);
    CHECK(tent_im_bild > 0, "%04X: keine Tentakel-Spitze im Bild von Schnitt 16", raum);
    CHECK(g5_im_bild > 0, "%04X: G5-Front nie im Bild von Schnitt 16", raum);
    /* M1 (Abnahme 1, hoch): RE2 laeuft Leon ab Bild 12 nach Osten aus der Grossaufnahme (RE2-Aufnahme: aus dem Bild 0,57 s nach der
     * Armierung; RAM s03 x 5736) - danach zeigt Schnitt 9 nur Tuer, Arme und Masse. */
    CHECK(letzt16 >= 0 && letzt16 < 45 && im16_spaet == 0, "%04X: M1 Spieler in Schnitt 16 bis Bild %d im Bild (%d Bilder ab 45)", raum, letzt16, im16_spaet);
    /* Schnitt 10: RE2 Pos_set(6500)+Dir_set 0x800 @0x0192A/@0x01932, dann rueckwaerts aus dem Bild (RAM s12..s19 6565 -> 16038). */
    CHECK(im17_437 && letzt17 >= RE15_G5A_BILD_SCHNITT10 && letzt17 < RE15_G5A_BILD_SCHNITT10 + 90,
          "%04X: M1 Schnitt 17: bei 437 im Bild %d, zuletzt im Bild %d (RE2: rueckwaerts hinaus in 1-2 s)", raum, im17_437, letzt17);
    CHECK(plx_437 >= 6500 && plx_437 <= 6600 && plx_806 - plx_437 >= 8000 && plx_806 - plx_437 <= 12000,
          "%04X: M1 Rueckwaertsgang 437..806 von %d nach %d (RE2 Basisclip 1 im Halbtakt: ~28 je Bild, 6565 -> 16038)", raum, (int)plx_437, (int)plx_806);
    CHECK(im12_807 && letzt12 >= RE15_G5A_BILD_SCHNITT5 && letzt12 < RE15_G5A_BILD_SCHNITT5 + 150,
          "%04X: M1 Cut 12: bei 807 im Bild %d, zuletzt im Bild %d (RE2: rueckwaerts auf die Kamera zu hinaus)", raum, im12_807, letzt12);
    /* M2: gezeichnete Armform = RE2 s04 (eingerollter letzter Austritts-Keyframe, nicht Clip 0 = gerade Stange) */
    CHECK(arme_n > 0 && arme_ok == arme_n, "%04X: M2 Armgelenke %d von %d Vergleichen innerhalb 250 von RE2 s04 (max %d)", raum, arme_ok, arme_n, (int)arme_abw_max);
    /* M3: RE2-RAM s05 (+0x14D 39) x -9525, Ende [T2] -2786 (s09..s12), Ende [T6] 1160 (s17..s26) */
    CHECK(g5x_129 >= -9535 && g5x_129 <= -9515, "%04X: M3 G5 x %d bei Bild 129 statt -9525 (RE2-RAM s05)", raum, (int)g5x_129);
    CHECK(g5x_270 >= -2796 && g5x_270 <= -2776, "%04X: M3 G5 x %d bei Bild 270 statt -2786 (RE2-RAM s09..s12)", raum, (int)g5x_270);
    CHECK(g5x_800 >= 1150 && g5x_800 <= 1170, "%04X: M3 G5 x %d bei Bild 800 statt 1160 (RE2-RAM s17..s26)", raum, (int)g5x_800);
    /* M4b: kein Ein-Bild-Sprung der gezeichneten Wurzel (Port vorher: 1329 bei Bild 270, -1329 bei 651) */
    CHECK(sprung_max <= 120, "%04X: M4b Wurzel-y springt im Intro um %d bei Bild %d", raum, sprung_max, sprung_bild);
    fprintf(stderr, "%04X: M4b Uebergang Kampf: groesster Wurzel-y-Schritt %d bei Bild %d (vorher 125 bei 1123)\n", raum, sprung_kampf, sprung_kampf_bild);
    CHECK(sprung_kampf <= 60, "%04X: Punkt 4 Wurzel-y springt im Kampfbeginn um %d bei Bild %d (Clip-Wort 0x1F0005 @0x80100dd8 mischt)", raum,
          sprung_kampf, sprung_kampf_bild);
    for (int k = 0; k < 2; k++) {
        fprintf(stderr, "%04X: M5 Speer Arm %d ph6 (%d,%d,%d,%d) RE2 s12 (%d,%d,%d,%d); ph7+20 (%d,%d,%d,%d) RE2 s13 (%d,%d,%d,%d)\n", raum, k,
                speer_w6[k][0], speer_w6[k][1], speer_w6[k][2], speer_w6[k][3], k_re2_speer_s12[k][0], k_re2_speer_s12[k][1], k_re2_speer_s12[k][2],
                k_re2_speer_s12[k][3], speer_w7[k][0], speer_w7[k][1], speer_w7[k][2], speer_w7[k][3], k_re2_speer_s13[k][0], k_re2_speer_s13[k][1],
                k_re2_speer_s13[k][2], k_re2_speer_s13[k][3]);
        CHECK(speer_ph6[k] && speer_ok6[k], "%04X: Punkt 3 Arm %d Teil-Winkel in sub9 ph6 != RE2 s12", raum, k);
        CHECK(speer_ph7_n[k] >= 20 && speer_ok7[k], "%04X: Punkt 3 Arm %d Teil-Winkel in sub9 ph7 != RE2 s13", raum, k);
    }
    fprintf(stderr, "%04X: M5 Speer Arm 0 |p3-p0| %d (RE2 s13 8170, vorher 6080)\n", raum, (int)speer_laenge);
    CHECK(speer_laenge >= 8170 - 250 && speer_laenge <= 8170 + 250, "%04X: Punkt 3 Arm 0 Reichweite %d statt RE2 s13 8170", raum, (int)speer_laenge);
    /* NB3 M2: Vorkampf-Puls @0x801044FC - w0 und Phase bitgleich mit RE2-RAM. NB3 M1: Gewicht 1 = RE2-RAM (Ctor-Basis trotz spaeter Bank) */
    for (int k = 0; k < 8; k++) {
        fprintf(stderr, "%04X: NB3 Bild %3d %s w0 %6d (RE2 %6d) ph %d (RE2 %d) w1 %5d (RE2 %5d)\n", raum, k_re2_gewichte[k].bild,
                k_re2_gewichte[k].ss, (int)gew_ist[k][0], (int)k_re2_gewichte[k].w0, gew_ph[k], k_re2_gewichte[k].ph,
                (int)gew_ist[k][1], (int)k_re2_gewichte[k].w1);
    }
    CHECK(gew_ok0 == 8, "%04X: NB3 M2 Puls-Gewicht 0 / Phase nur %d von 8 Bildern gleich RE2-RAM (@0x801044FC)", raum, gew_ok0);
    CHECK(gew_ok1 == 8, "%04X: NB3 M1 Gewicht 1 nur %d von 8 Bildern gleich RE2-RAM (Ctor-Basis @0x80101200, Kopplung @0x80101334/@0x8010157c)",
          raum, gew_ok1);
    /* NB3 M1 ZEICHNER: die Vertices, die main.c fuer Teil 2 an den Zeichner gibt, liegen auf dem RE2-Vertexpuffer (RE2-Morph = Port-Morph
     * auf 3 Einheiten, Abnahme G5 2 3.1). Vorher (Gewicht 1 = 0) wich die Grundform um bis zu ~1300 ab (Bbox y max 23 statt -1313). */
    for (int k = 0; k < 4; k++) {
        fprintf(stderr, "%04X: NB3 Zeichner-Vertices Teil 2 Bild %d gegen RE2-RAM %s: max %d, Bbox %d\n", raum, k_re2_masse[k].bild,
                k_re2_masse[k].ss, masse_abw[k], masse_bb_abw[k]);
        CHECK(masse_abw[k] <= 4 && masse_bb_abw[k] <= 4, "%04X: NB3 M1 gezeichnete Masse Bild %d weicht von RE2-RAM %s um %d (Bbox %d) ab",
              raum, k_re2_masse[k].bild, k_re2_masse[k].ss, masse_abw[k], masse_bb_abw[k]);
    }
    /* M4: die Masse links im Bild liegt hinter der Pfostenmaske (depth 100, RE2-Regel) */
    CHECK(pfosten_n == 0 || pfosten_vor == pfosten_n, "%04X: M4 Masse in %d von %d Bildern vor dem Pfosten", raum, pfosten_n - pfosten_vor, pfosten_n);
    CHECK(frau_sichtbar_re2 == 0 && frau_ok_cut12, "%04X: M6 Frau in 16/17 %d Bilder sichtbar, Cut 12 wie vorher %d", raum, frau_sichtbar_re2, frau_ok_cut12);
    CHECK(cam_bei[0] == RE15_G5A_SCHNITT10, "%04X: Bild 437 Cut %d statt Schnitt 17 (@0x0193A)", raum, cam_bei[0]);
    CHECK(cam_bei[1] == 0x0C && pl_x_bei[1] >= 15646 && pl_x_bei[1] <= 15800, "%04X: Bild 807 Cut %d Spieler %d statt Cut 12 / 15646",
          raum, cam_bei[1], (int)pl_x_bei[1]);
    CHECK(ende == RE15_G5A_BILD_ENDE, "%04X: Szenenende bei Bild %d statt 1162 (@0x01962)", raum, ende);
    CHECK(cam_bei[2] == 0x0C && pl_x_bei[2] >= 13873 - 400 && pl_x_bei[2] <= 13873 + 400, "%04X: Szenenende Cut %d Spieler %d statt Cut 12 / 13873 "
          "(zweite Setzung @0x0196C)", raum, cam_bei[2], (int)pl_x_bei[2]);
    CHECK(!re15_game_flag_get(1, 27) && !re15_game_flag_get(2, 7) && g_scd.cut_auto_enabled, "%04X: nach dem Ende gesperrt", raum);
    CHECK(sub == 1 || sub == 0, "%04X: G5 kaempft nach dem Ende nicht (Sub %d)", raum, sub);
    if (spur) fclose(spur);
    free(buf);
}

int main(void)
{
    test_daten();
    test_xa_daten();
    test_ablauf("STAGE5/ROOM5090.RDT", 0x5090u);
    test_ablauf("STAGE5/ROOM5091.RDT", 0x5091u);
    free(s_ems);
    if (fails) { fprintf(stderr, "test_r41_g5auftritt: %d Fehler\n", fails); return 1; }
    fprintf(stderr, "test_r41_g5auftritt: OK\n");
    return 0;
}
