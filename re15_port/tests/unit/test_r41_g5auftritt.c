/* test_r41_g5auftritt.c — Riegel G5-Auftritt nach RE2 room7040 sub06 (include/re15_g5auftritt_r41.h,
 * Dossier analysis/befunde_runde41/G5_auftritt.md).
 *
 * 1. DATEN: RE2/R704_CUT09.BSS und .PRI sind die RE2-Bytes (gegen info/re2leon, wenn vorhanden), der Chunk dekodiert mit dem
 *    Port-Decoder (Hintergrund gegen ROOM70409.bmp), der Masken-Atlas mit SLD-Laenge 0xCF40, die Masken 2 Gruppen / 32;
 *    die Szenenkamera ist RID @0x184.
 * 2. ABLAUF in ROOM5090 (Leon) und ROOM5091 (Elza) mit RE2-EM36/EM37 ueber die Schnittstelle re15_bf_g5_auftritt_start:
 *    Schnitt 16 bis Bild 436, Cut 11 ab 437, Cut 12 ab 807, Ende 1162; Spielerlagen; Sperre (1,27)/(2,7) und Zonen aus bis
 *    zum Ende, danach frei; ZUERST treten Tentakel aus, DANN zieht sich der G5 herein, beides im Schnitt 16, und beides im
 *    Bild der Szenenkamera; die offene Tuer liegt im Bild; nach dem Ende kaempft der G5 (Sub 1).
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

extern int re15_g5_boss_zustand(int *sub, int *ph, int *timer);
extern int re15_g5_tentakel_zustand(int idx, int *sub, int *ph, int *ankermodus, int32_t *scale_x, uint16_t *kontakt);
extern int re15_g5_tentakel_spitze(int idx, int32_t out[3]);
extern void scd_register_current_rdt(const re15_rdt_t *);

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

static void test_ablauf(const char *rdt, unsigned raum)
{
    uint8_t *buf = raum_laden(rdt, raum);
    CHECK(buf != NULL, "%s nicht ladbar", rdt);
    if (!buf) return;
    re15_g5a_reset();
    int bs = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x30u) bs = s;
    CHECK(bs > 0, "%04X: kein 0x30-Spawn (sub00 @0x0124A)", raum);
    CHECK(g5_banken(), "RE2-Banken EM36/EM37");
    if (bs <= 0) { free(buf); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER], *e = &g_actors[bs];
    pl->active = 1; pl->hp = 100; pl->x = 9000; pl->z = -23400;   /* wo Leon in der Rueckstoss-Szene stehen kann */
    e->x = -6283; e->z = -23482; e->no_draw = 0;                  /* hinter der Tuer, wo die Form verschwindet */
    re15_game_flag_set(1, 27, 0); re15_game_flag_set(2, 7, 0);
    g_scd.cut_auto_enabled = 1;

    re15_bf_g5_auftritt_start(bs);                                /* die SCHNITTSTELLE */
    CHECK(re15_g5a_laeuft() && re15_g5a_bild() == 0, "%04X: Szene startet nicht", raum);
    CHECK(e->type == 0x36u && e->grid_id == 0x13u, "%04X: kein G5 armiert (Typ 0x%02X grid 0x%02X)", raum, e->type, e->grid_id);
    CHECK(g_scd.cam_id == RE15_G5A_SCHNITT && !g_scd.cut_auto_enabled, "%04X: Bild 0 nicht Schnitt 16 mit Zonen aus (cam %d auto %d)",
          raum, g_scd.cam_id, g_scd.cut_auto_enabled);
    CHECK(re15_game_flag_get(1, 27) && re15_game_flag_get(2, 7), "%04X: Sperre (1,27)/(2,7) fehlt (@0x01778/@0x0177C)", raum);
    CHECK(pl->x == 1400 && pl->z == -23570 && pl->rot_y == 0x800, "%04X: Spieler Bild 0 (%d,%d) Gier %d statt (1400,-23570) 0x800",
          raum, (int)pl->x, (int)pl->z, pl->rot_y);

    const re15_camera_cut_t *k16 = re15_g5a_schnitt(RE15_G5A_SCHNITT);
    CHECK(k16 != NULL, "%04X: kein Szenen-Schnitt", raum);
    if (k16) {
        /* RE2-Schnitt 9 blickt schraeg von Suedost auf das Wagenende: der Nordpfosten der Tuer (x -5683, z -21990) und die Nordwest-
         * Ecke liegen am linken Bildrand, die Tuermitte (z -23482) schon links ausserhalb (Dossier 1.5). */
        int sx = -1, sy = -1, ex = -1, ey = -1;
        CHECK(im_bild(k16, -5683, -1500, -21990, &sx, &sy), "%04X: Tuer-Nordpfosten nicht im Bild von Schnitt 16 (%d,%d)", raum, sx, sy);
        CHECK(im_bild(k16, -5683, -2500, -19870, &ex, &ey), "%04X: Nordwest-Ecke nicht im Bild von Schnitt 16 (%d,%d)", raum, ex, ey);
        fprintf(stderr, "%04X: Schnitt 16: Tuer-Nordpfosten bei (%d,%d), Nordwest-Ecke bei (%d,%d)\n", raum, sx, sy, ex, ey);
    }

    FILE *spur = NULL;
    {   const char *sp = getenv("RE15_G5A_SPUR");   /* Messschiene: <Pfad>_<Raum>.txt */
        if (sp && *sp) { char pf[600]; snprintf(pf, sizeof pf, "%s_%04X.txt", sp, raum); spur = fopen(pf, "w"); } }
    int bild_tent = -1, bild_zug = -1, bild_tuer = -1, tent_im_bild = 0, g5_im_bild = 0;
    int falsch_schnitt = 0, zonen_an = 0, frei_vorher = 0, ende = -1;
    int32_t x_alt = e->x;
    int16_t cam_bei[3] = { -1, -1, -1 };
    int32_t pl_x_bei[3] = { 0, 0, 0 };
    for (int b = 0; b <= RE15_G5A_BILD_ENDE + 40; b++) {    /* b = Szenenbild dieses KI-Durchlaufs (Bild 0 = Armierung, [T0]) */
        pl->hp = 100; pl->hit_react = 0;
        re15_enemy_ai_run_all(0);
        CHECK(b >= RE15_G5A_BILD_ENDE || re15_g5a_bild() == b, "%04X: Szenenbild %d statt %d", raum, re15_g5a_bild(), b);
        if (spur) {   /* MESSSCHIENE (kein Verhalten): Lagen je Bild fuer das RE2-Kontaktblatt (tools/r41_g5auftritt/re2_kontakt.py) */
            int32_t sp[4][3] = { { 0 } };
            int32_t scx[4] = { 0 };
            for (int i = 0; i < 4; i++) {
                (void)re15_g5_tentakel_zustand(i, NULL, NULL, NULL, &scx[i], NULL);
                if (re15_g5_tentakel_spitze(i, sp[i]) != 0) sp[i][0] = sp[i][1] = sp[i][2] = 0;
            }
            int gs = -1, gp = -1;
            (void)re15_g5_boss_zustand(&gs, &gp, NULL);
            fprintf(spur, "%d %d %d %d %d %d %d %d %d %d %d", b, (int)g_scd.cam_id, (int)pl->x, (int)pl->z, (int)e->x, (int)e->z,
                    (int)e->motion, (int)e->anim_frame, gs, gp, (int)e->rot_y);
            for (int i = 0; i < 4; i++) fprintf(spur, " %d %d %d %d", (int)scx[i], (int)sp[i][0], (int)sp[i][1], (int)sp[i][2]);
            fprintf(spur, "\n");
        }
        if (b < RE15_G5A_BILD_ENDE) {
            int soll = b < RE15_G5A_BILD_SCHNITT10 ? RE15_G5A_SCHNITT9 : (b < RE15_G5A_BILD_SCHNITT5 ? RE15_G5A_SCHNITT10 : 0x0C);
            if (g_scd.cam_id != soll) falsch_schnitt++;
            if (g_scd.cut_auto_enabled) zonen_an++;
            if (!re15_game_flag_get(1, 27) || !re15_game_flag_get(2, 7)) frei_vorher++;
        } else if (ende < 0 && !re15_g5a_laeuft()) ende = b;
        if (b == RE15_G5A_BILD_SCHNITT10) {
            cam_bei[0] = g_scd.cam_id; pl_x_bei[0] = pl->x;
            /* RE2-Schnitt 10 zeigt Leon auf 6500 rechts im Bild (im RE1.5-Cut 11 lag er rechts ausserhalb, Dossier 3) */
            const re15_camera_cut_t *k17 = re15_g5a_schnitt(RE15_G5A_SCHNITT10);
            int lx = -1, ly = -1;
            CHECK(k17 && im_bild(k17, pl->x, -900, pl->z, &lx, &ly), "%04X: Leon (%d,%d) nicht im Bild von Schnitt 17 (%d,%d)",
                  raum, (int)pl->x, (int)pl->z, lx, ly);
        }
        if (b == RE15_G5A_BILD_SCHNITT5)  { cam_bei[1] = g_scd.cam_id; pl_x_bei[1] = pl->x; }
        if (b == RE15_G5A_BILD_ENDE)      { cam_bei[2] = g_scd.cam_id; pl_x_bei[2] = pl->x; }
        for (int i = 0; i < 4; i++) {
            int32_t sc = 0;
            if (re15_g5_tentakel_zustand(i, NULL, NULL, NULL, &sc, NULL) == 0 && sc > 0 && bild_tent < 0) bild_tent = b;
            int32_t sp[3];
            if (b < RE15_G5A_BILD_SCHNITT10 && k16 && sc > 2048 && re15_g5_tentakel_spitze(i, sp) == 0 &&
                im_bild(k16, sp[0], sp[1], sp[2], NULL, NULL)) tent_im_bild++;
        }
        if (e->x > x_alt + 4 && bild_zug < 0 && b > 1) bild_zug = b;
        if (e->x > -5683 && bild_tuer < 0) bild_tuer = b;
        if (b < RE15_G5A_BILD_SCHNITT10 && k16 && im_bild(k16, e->x + 4494, -2000, e->z, NULL, NULL)) g5_im_bild++;
        x_alt = e->x;
    }
    int sub = -1, ph = -1;
    (void)re15_g5_boss_zustand(&sub, &ph, NULL);
    fprintf(stderr, "%04X: Tentakel ab Bild %d, Hereinziehen ab %d, Ursprung durch die Tuer bei %d, Tentakel-Spitzen im Bild %d, "
                    "G5-Front im Bild %d, Ende %d, G5 Sub %d Ph %d x %d, Spieler x %d Cut %d\n",
            raum, bild_tent, bild_zug, bild_tuer, tent_im_bild, g5_im_bild, ende, sub, ph, (int)e->x, (int)pl->x, g_scd.cam_id);
    CHECK(falsch_schnitt == 0, "%04X: %d Bilder im falschen Schnitt (16 / 17 ab 437 / 12 ab 807)", raum, falsch_schnitt);
    CHECK(zonen_an == 0, "%04X: Zonen in %d Szenenbildern an (Cut_auto(0) @0x01780 bis @0x01981)", raum, zonen_an);
    CHECK(frei_vorher == 0, "%04X: Sperre in %d Bildern vor dem Ende frei", raum, frei_vorher);
    CHECK(bild_tent > 0 && bild_tent <= 12, "%04X: erste Tentakel bei Bild %d statt [T1] t=10 (@0x80101240)", raum, bild_tent);
    CHECK(bild_zug > bild_tent && bild_zug >= 91 && bild_zug <= 120, "%04X: Hereinziehen bei Bild %d (Tentakel %d), soll nach den Tentakeln mit [T2] ab t=91",
          raum, bild_zug, bild_tent);
    CHECK(bild_tuer > bild_zug && bild_tuer < RE15_G5A_BILD_SCHNITT10, "%04X: G5 kommt in Schnitt 16 nicht durch die Tuer (Bild %d)", raum, bild_tuer);
    CHECK(tent_im_bild > 0, "%04X: keine Tentakel-Spitze im Bild von Schnitt 16", raum);
    CHECK(g5_im_bild > 0, "%04X: G5-Front nie im Bild von Schnitt 16", raum);
    /* Lage am Ende des Bilds: gesetzt wird sie vor der Gegner-Schleife, der G5 und die Arme duerfen sie im selben Bild schieben
     * (FUN_80034D0C, enemy_ai_tentakel_g5.c) - wie in RE2, wo derselbe Pos_set vor derselben KI steht. */
    CHECK(cam_bei[0] == RE15_G5A_SCHNITT10 && pl_x_bei[0] >= 6500 && pl_x_bei[0] <= 7100, "%04X: Bild 437 Cut %d Spieler %d statt Schnitt 17 / 6500 (@0x0192A/@0x0193A)",
          raum, cam_bei[0], (int)pl_x_bei[0]);
    CHECK(cam_bei[1] == 0x0C && pl_x_bei[1] >= 13873 && pl_x_bei[1] <= 14473, "%04X: Bild 807 Cut %d Spieler %d statt Cut 12 / 13873",
          raum, cam_bei[1], (int)pl_x_bei[1]);
    CHECK(ende == RE15_G5A_BILD_ENDE, "%04X: Szenenende bei Bild %d statt 1162 (@0x01962)", raum, ende);
    /* Ab Bild 1126 kaempft der G5 schon (Sub 1, 36 Bilder vor der Freigabe, wie in RE2): seine Arme duerfen den Spieler im Bild der
     * zweiten Setzung schon wieder ziehen (gemessen -46 / +230). Die Kamera kommt aus der gesetzten Lage 13873 = Cut 12. */
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
    test_ablauf("STAGE5/ROOM5090.RDT", 0x5090u);
    test_ablauf("STAGE5/ROOM5091.RDT", 0x5091u);
    free(s_ems);
    if (fails) { fprintf(stderr, "test_r41_g5auftritt: %d Fehler\n", fails); return 1; }
    fprintf(stderr, "test_r41_g5auftritt: OK\n");
    return 0;
}
