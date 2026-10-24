/*
 * gator_re2_pc.c — Runde 41 Spur B "re2gator", PC-Seite:
 *   (1) der GASKANISTER als Prop: ROOM40A0 Modell 1 (MD1 @RDT 0x4024, TIM @0x2fcfc, CLUT-Zeile 481 =
 *       Warnstreifen/Umbrella), byteidentisch als shared_assets/RE2/KANISTER40A0.{MD1,TIM}; Lage/Drehung
 *       liefert die Engine (gator_re2_r41.c: frei im Wasser bzw. an der Part-6-Weltmatrix des Gators,
 *       T2 @0x801018d0-60). Gezeichnet wie ein RDT-Prop (main.c Prop-Pass: view x Welt, RTPT/NCLIP ueber
 *       objpoly_r38, Licht des aktiven Cuts), eigener TIM-Platz 53 (frei: render_pc.c RE15_TIM_SLOT_MAX 56,
 *       52 = RE2FX_TIM_SLOT, 50/51 = FX-Seiten).
 *   (2) die RE2-RAUMBANK-Saetze von ROOM40A0 (Mini-Bank GATOR40A0.VBS, tools/r41_re2gator/gator40a0_se_cut.py):
 *       Satz 1 = Explosionsbrueller `Se_on(0x020f0001)` @0x8010237c-88 (R1), Satz 0 = Todesbrueller
 *       `Se_on(0x020e0001)` @0x8010280c (R2, im Port nicht erreichbar). Laderpaar wie TUERSE (audio_pc.c 5c),
 *       Wiedergabe ueber dieselbe Stimmen-/Prioritaetsmaschine (re15_audio_se_play_bank).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_gator_re2_r41.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_camera.h"
#include "re15_light.h"
#include "re15_objpoly_r38.h"
#include "re15_engine.h"        /* re15_render_textured_tri_lit */
#include "re15_audio.h"
#include "re15_vab.h"
#include "asset_root_pc.h"      /* re15_pc_read_re2 */
#include "../../../engine/src/gen/r41_gator40a0_bank.inc"
#include <SDL_timer.h>   /* SDL_GetTicks (Messzeile der Zeitlupe) */
#include "re2_fx.h"       /* re2fx_quads (Messzeile Putz-Sicht) */
#include "re2fx_pc.h"     /* re2fx_pc_letzte_quads */
#include "re15_boss_gator.h"  /* Nachbesserung 3: re15_gator_boss_untergetaucht */
#include "re15_room.h"        /* Nachbesserung 3: g_current_room_id (Messzeilen je Raumeintritt) */
#include "re15_math.h"        /* Nachbesserung 3: re15_squareroot0 (Messzeile Lagesprung) */
#include "re15_zopf_r38.h"    /* Nachbesserung 6: Elzas Zopf-Glied 0 (Messzeile Maul-Sitz) */
#include "re15_gameflow.h"    /* Nachbesserung 6: re15_char_variant (Elza) */

#define R41_KANISTER_TIM_SLOT 53

extern void re15_render_pc_bind_tim_slot(int slot);
extern void re15_audio_se_play_bank(const uint8_t *edt, const re15_vab_t *vab, int16_t *const *decoded,
                                    const int *decoded_len, int satz);

/* ---- (1) Kanister-Modell --------------------------------------------------------------------- */
static int        s_modell_state;       /* 0 ungeprueft, 1 geladen, -1 fehlt */
static uint8_t   *s_md1_raw;
static re15_md1_t s_md1;

static int modell_laden(void)
{
    if (s_modell_state) return s_modell_state > 0;
    s_modell_state = -1;
    int n = 0;
    s_md1_raw = re15_pc_read_re2("KANISTER40A0.MD1", &n);
    if (!s_md1_raw || re15_md1_parse(s_md1_raw, n, &s_md1) != 0 || s_md1.mesh_count < 1) {
        fprintf(stderr, "[gator-re2] shared_assets/RE2/KANISTER40A0.MD1 fehlt/unlesbar -> kein Kanister\n");
        return 0;
    }
    int tn = 0;
    uint8_t *tim_raw = re15_pc_read_re2("KANISTER40A0.TIM", &tn);
    re15_tim_t tim;
    if (!tim_raw || re15_tim_parse(tim_raw, tn, &tim) != 0) {
        fprintf(stderr, "[gator-re2] shared_assets/RE2/KANISTER40A0.TIM fehlt/unlesbar -> kein Kanister\n");
        free(tim_raw); return 0;
    }
    re15_render_pc_upload_tim_slot(&tim, R41_KANISTER_TIM_SLOT);
    free(tim_raw);                          /* der Platz haelt eine Kopie (Slot-Textur) */
    s_modell_state = 1;
    fprintf(stderr, "[gator-re2] Kanister-Modell geladen: %d Meshes, TIM-Platz %d\n",
            s_md1.mesh_count, R41_KANISTER_TIM_SLOT);
    return 1;
}

/* (4) MESSZEILE Putz-Sicht (Nachbesserung 1 M2, Fortsetzung 3): ab dem ersten Putz-Spawn 120 Bilder lang je
 * 5. Bild die RE2-FX-Quads des Pools in der angezeigten Kamera (re2fx_quads ohne Regions-Test), wie viele davon
 * den Filter des Zeichners passieren (re2fx_pc.c: TPage 0x1E/0x1F, CLUT-Spalte 0x11, Zeilen 480..484) und die
 * im Vorbild tatsaechlich gezeichneten (re2fx_pc_letzte_quads, mit Regions-Test). Reine Messung. */
static void putz_sicht(const re15_camera_view_t *cam, int cx, int cy)
{
    static int s_aktiv = 0; static unsigned s_f0 = 0;
    if (!cam || re15_gr2_putz_spawns() <= 0) return;
    if (!s_aktiv) { s_aktiv = 1; s_f0 = (unsigned)g_engine.frame_count; }
    unsigned df = (unsigned)g_engine.frame_count - s_f0;
    if (df > 120 || (df % 5u) != 0) return;
    static re2fx_quad_t q[384];
    extern int pc_fx_camf(void);
    int n = re2fx_quads(cam, cx, cy, pc_fx_camf(), 0, NULL, NULL, q, 384), ok = 0, ix = -1;
    for (int i = 0; i < n; i++) {
        unsigned seite = q[i].tpage & 0x1Fu, zeile = (q[i].clut >> 6) & 0x1FFu;
        if ((seite == 0x1E || seite == 0x1F) && (q[i].clut & 0x3Fu) == 0x11u && zeile >= 480 && zeile < 485) ok++;
        if (ix < 0 || q[i].y0 < q[ix].y0) ix = i;                  /* hoechstes Quad (Putz faellt von oben) */
    }
    if (ix >= 0) {
        const uint8_t *pb = re2fx_platz(q[ix].platz);
        int wx = pb ? (int16_t)(pb[0x34] | (pb[0x35] << 8)) : 0, wy = pb ? (int16_t)(pb[0x36] | (pb[0x37] << 8)) : 0;
        int wz = pb ? (int16_t)(pb[0x38] | (pb[0x39] << 8)) : 0;
        fprintf(stderr, "[gator-re2] Putz-Sicht F%u spawns=%d quads=%d filter_ok=%d gezeichnet_vorbild=%d hoechstes=(%d,%d)-(%d,%d) tpage=%x clut=%x platz=%d welt=(%d,%d,%d)\n",
                (unsigned)g_engine.frame_count, re15_gr2_putz_spawns(), n, ok, re2fx_pc_letzte_quads(),
                q[ix].x0, q[ix].y0, q[ix].x1, q[ix].y1, q[ix].tpage, q[ix].clut, q[ix].platz, wx, wy, wz);
    }
    else
        fprintf(stderr, "[gator-re2] Putz-Sicht F%u spawns=%d quads=0 gezeichnet_vorbild=%d\n",
                (unsigned)g_engine.frame_count, re15_gr2_putz_spawns(), re2fx_pc_letzte_quads());
}

/* (6) MESSZEILE Kanister-Sitz (Nachbesserung 4, Abnahme 3 M1): nach dem NPC-Pass je Bild |Part6(gezeichnete Pose) x Lokallage -
 * gezeichnete Kanister-Lage| (re15_gr2_kanister_sitz, aus dem Ueberblend-Schnappschuss, den der Zeichner eben geschrieben hat).
 * debug.log "[gator-re2] Kanister-Sitz F<n>: angehaengt <k> Bilder, Abweichung max <m> ..." alle 30 angehaengten Bilder und
 * bei jedem neuen Hoechstwert. RE2: Kind der Part-6-Weltmatrix (@0x801018d0-e0) = Abweichung 0 in jedem Bild. */
static void kanister_sitz(void)
{
    static int s_n = 0; static int32_t s_max = -1; static unsigned s_fmax = 0, s_raum = 0xffffffffu;
    if (s_raum != (unsigned)g_current_room_id) { s_raum = (unsigned)g_current_room_id; s_n = 0; s_max = -1; s_fmax = 0; }
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *a = &g_actors[i];
        if (!a->active || a->type != 0x23u || !re15_gator_boss_active(a) || re15_gator_boss_untergetaucht(a)) continue;
        int32_t abw = 0;
        if (!re15_gr2_kanister_sitz(a, &abw)) return;
        unsigned f = (unsigned)g_engine.frame_count;
        s_n++;
        int neu = abw > s_max;
        if (neu) { s_max = abw; s_fmax = f; }
        if (neu || (s_n % 30) == 0)
            fprintf(stderr, "[gator-re2] Kanister-Sitz F%u: angehaengt %d Bilder, Abweichung max %d (F%u), jetzt %d clip=%d bild=%d gier=%d frac=%d\n",
                    f, s_n, (int)s_max, s_fmax, (int)abw, (int)a->motion, (int)a->anim_frame, (int)(a->rot_y & 0xfff),
                    (int)a->anim_frac);
        return;
    }
}

/* (7) MESSZEILE Maul-Sitz (Nachbesserung 5, Abnahme 4 M1): je Bild des Fress-Finishers (FRESSEN P1-P3) |Kopf (Bone 6) der
 * gezeichneten Pose - Kopf an der Anker-Lage ohne Zusaetze| (re15_gr2_fress_sitz; RE2 FUN_80015cb8 setzt Gator und Opfer vom
 * gemeinsamen Anker, ohne Neigung und ohne Wand = Opfer im Maul wie authored), ob Kopf und Leons Wurzel im Bild der GEZEICHNETEN
 * Kamera liegen (Projektion wie der Zeichner: view x Welt, H/z) und ob beide im Wasser sind (Beckenquelle des Boss-Moduls).
 * Dazu der Abstand Leon <-> gezeichneter Kiefer (Abnahme 4 M1): |(Leons Wurzel - Part 7) gezeichnet - dieselbe Groesse der
 * authored Finisher-Bahn| (re15_gr2_fress_sitz, Bahn aus den Bankdaten relativ zur Buehne). "oben hinaus" = Leons Wurzel ueber dem
 * oberen Bildrand bei Bild-x im Bild (der Wurf in P2 Bild 60-75 hebt ihn aus dem POS-Kanal bis y -9177 roh - in RE2 Cut 7 ebenso).
 * debug.log "[gator-re2] Maul-Sitz F<n>: Bilder <k> (P1 a P2 b P3 c), Abweichung max <m> (F..), Kiefer-Abstand max <j> (F..),
 * Kopf im Bild <h>/<k>, Leon im Bild <l>/<k> + oben hinaus <o> (Cut <c>), im Wasser <w>/<k> | jetzt ..." beim ersten Bild, bei
 * jedem neuen Hoechstwert, bei jedem Bild ausserhalb Bild/Wasser und alle 30 Bilder; im ersten Bild nach dem Finisher die
 * Summenzeile "[gator-re2] Maul-Sitz Ende F<n>: ..." (integration_r41_re2gator Lauf belagerung). Reine Messung.
 * Nachbesserung 6 (Abnahme 5): P3 endet nicht mehr (RE2 @0x801011c4-e4) - die Summenzeile steht jetzt im Bild der Game-Over-
 * Uebergabe (g_gameover_active: letztes Bild des Raums, danach Titel) bzw. im ersten Bild eines anderen Raums, und traegt dazu
 * "Opfer gehalten" (Bilder mit Opfer-Modus = re15_player_is_grabbed, RE2 Spieler +0x1D3 |= 0x80 @0x80102f24-28) und "Opfer frei"
 * (M1/M3: Bilder, in denen kein Knochen-Ursprung Leons (re15_gr2_leon_knochen) im Bild von einem gezeichneten Raum-Objekt verdeckt
 * ist, re15_gr2_raum_verdeckt) - "Leon im Bild" prueft nur die Projektion der Wurzel. */
static int fress_proj(const re15_camera_view_t *cam, int cx, int cy, const int32_t p[3], int *ox, int *oy)
{
    if (ox) *ox = -1;
    if (oy) *oy = -1;
    if (!cam) return 0;
    int64_t px = p[0], py = p[1], pz = p[2];
    int32_t vx = (int32_t)(((int64_t)cam->rot[0]*px + (int64_t)cam->rot[1]*py + (int64_t)cam->rot[2]*pz) >> 12) + cam->trans[0];
    int32_t vy = (int32_t)(((int64_t)cam->rot[3]*px + (int64_t)cam->rot[4]*py + (int64_t)cam->rot[5]*pz) >> 12) + cam->trans[1];
    int32_t vz = (int32_t)(((int64_t)cam->rot[6]*px + (int64_t)cam->rot[7]*py + (int64_t)cam->rot[8]*pz) >> 12) + cam->trans[2];
    if (vz <= 0) return 0;
    int x = cx + (int)(((int64_t)vx * cam->fov_screen_dist) / vz), y = cy + (int)(((int64_t)vy * cam->fov_screen_dist) / vz);
    if (ox) *ox = x;
    if (oy) *oy = y;
    return x >= 0 && x < 320 && y >= 0 && y < 240;
}
static void fress_sitz(const re15_camera_view_t *cam, int cx, int cy)
{
    static int s_n, s_pn[4], s_kopf_bild, s_leon_bild, s_leon_oben, s_wasser, s_ende, s_gehalten, s_frei, s_vpkt_max, s_cut = -1;
    static int32_t s_max = -1, s_kmax = -1, s_zopf_neu = -1, s_zopf_alt = -1;
    static unsigned s_fmax = 0, s_fkmax = 0, s_fvpkt = 0, s_raum = 0xffffffffu, s_fletzt = 0;
    extern int re15_player_is_grabbed(void);   /* enemy_ai_common.c: Opfer-Modus 4 = Spieler +0x1D3 & 0x80 */
#define R41_MAUL_ENDE(grund) \
    do { s_ende = 1; \
         fprintf(stderr, "[gator-re2] Maul-Sitz Ende F%u: Bilder %d (P1 %d P2 %d P3 %d), Abweichung max %d (F%u), Kiefer-Abstand max %d (F%u), " \
                 "Kopf im Bild %d/%d, Leon im Bild %d/%d + oben hinaus %d (Cut %d), im Wasser %d/%d, Opfer gehalten %d/%d, Opfer frei %d/%d " \
                 "(Punkte verdeckt max %d F%u), Zopf-Glied 0 zum Kopf-Knochen max %d (ohne Massstab %d), letztes Finisher-Bild F%u (%s)\n", \
                 (unsigned)g_engine.frame_count, s_n, s_pn[1], s_pn[2], s_pn[3], (int)s_max, s_fmax, (int)s_kmax, s_fkmax, \
                 s_kopf_bild, s_n, s_leon_bild, s_n, s_leon_oben, s_cut, s_wasser, s_n, s_gehalten, s_n, s_frei, s_n, \
                 s_vpkt_max, s_fvpkt, (int)s_zopf_neu, (int)s_zopf_alt, s_fletzt, (grund)); } while (0)
    if (s_raum != (unsigned)g_current_room_id) {
        if (s_n > 0 && !s_ende) R41_MAUL_ENDE("Raumwechsel");   /* NB6: Finisher bis zum letzten Bild des Raums */
        s_raum = (unsigned)g_current_room_id; s_n = 0; s_kopf_bild = 0; s_leon_bild = 0; s_leon_oben = 0; s_wasser = 0; s_ende = 0;
        s_max = -1; s_kmax = -1; s_fmax = 0; s_fkmax = 0; s_gehalten = 0; s_frei = 0; s_vpkt_max = 0; s_fvpkt = 0; s_cut = -1;
        s_zopf_neu = -1; s_zopf_alt = -1;
        memset(s_pn, 0, sizeof s_pn);
    }
    int im_fressen = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *a = &g_actors[i];
        if (a->active && a->type == 0x23u && re15_gator_boss_im_fressen(a)) { im_fressen = 1; break; }
    }
    if (!im_fressen && s_n > 0 && !s_ende) {             /* Summenzeile, falls der Finisher im Raum endet (seit NB6 nicht vorgesehen) */
        R41_MAUL_ENDE("Finisher-Ende");
        return;
    }
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *a = &g_actors[i];
        if (!a->active || a->type != 0x23u || !re15_gator_boss_im_fressen(a)) continue;
        int32_t abw = 0, kab = -1, kopf[3], leon[3]; int ph = 0;
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        if (!re15_gr2_fress_sitz(a, pl, &abw, kopf, leon, &kab, &ph)) return;
        unsigned f = (unsigned)g_engine.frame_count;
        int lx = -1, ly = -1;
        int kb = fress_proj(cam, cx, cy, kopf, NULL, NULL);
        int lb = fress_proj(cam, cx, cy, leon, &lx, &ly);
        int lo = !lb && lx >= 0 && lx < 320 && ly < 0;
        int wasser = re15_gr2_becken_draussen(kopf[0], kopf[2], 0) == 0 && re15_gr2_becken_draussen(leon[0], leon[2], 0) == 0;
        int gehalten = re15_player_is_grabbed();
        /* NB6 (M3): Leons Knochen + Kopf gegen die in diesem Cut gezeichneten Raum-Objekte (Strahl vom Kamera-Ort) */
        int32_t pk[20][3]; int np = re15_gr2_leon_knochen(a, pl, pk, 18), nim = 0;
        if (np <= 0) { memcpy(pk[0], leon, sizeof leon); np = 1; }
        memcpy(pk[np], kopf, sizeof kopf); np++;
        int nverd = re15_gr2_raum_verdeckt((int)g_re15_active_cut, (const int32_t (*)[3])pk, np, &nim);
        /* NB6 (Elza): Zopf-Glied 0 (Weltmatrix zopf_r38.c, gezeichnet mit re15_gr2_zopf_massstab) gegen Elzas gezeichneten Kopf-Knochen
         * (Teil 8 = RE15_ZOPF_KOPF_TEIL, Elternteil des 1. Glieds @0x80024c98) - "ohne Massstab" = was der Zeichner vor NB6 zeigte */
        if (re15_char_variant() && np - 1 > RE15_ZOPF_KOPF_TEIL) {
            int32_t zr[9], zt[3], zs[3], zr2[9];
            if (re15_zopf_r38_welt(0, zr, zt)) {
                memcpy(zs, zt, sizeof zt); memcpy(zr2, zr, sizeof zr);
                re15_gr2_zopf_massstab(pl, zr2, zs);
                int64_t q1 = 0, q2 = 0;
                for (int i = 0; i < 3; i++) {
                    int64_t d1 = (int64_t)zs[i] - pk[RE15_ZOPF_KOPF_TEIL][i], d2 = (int64_t)zt[i] - pk[RE15_ZOPF_KOPF_TEIL][i];
                    q1 += d1 * d1; q2 += d2 * d2;
                }
                int32_t a1 = (int32_t)re15_squareroot0((uint32_t)(q1 > 0xffffffffLL ? 0xffffffffLL : q1));
                int32_t a2 = (int32_t)re15_squareroot0((uint32_t)(q2 > 0xffffffffLL ? 0xffffffffLL : q2));
                if (a1 > s_zopf_neu) s_zopf_neu = a1;
                if (a2 > s_zopf_alt) s_zopf_alt = a2;
            }
        }
        s_n++; s_pn[ph & 3]++; s_cut = (int)g_re15_active_cut; s_fletzt = f;
        if (kb) s_kopf_bild++;
        if (lb) s_leon_bild++;
        if (lo) s_leon_oben++;
        if (wasser) s_wasser++;
        if (gehalten) s_gehalten++;
        if (nverd == 0) s_frei++;
        int neu = abw > s_max;
        if (neu) { s_max = abw; s_fmax = f; }
        if (nverd > s_vpkt_max) { s_vpkt_max = nverd; s_fvpkt = f; neu = 1; }
        int32_t kw = kab < 0 ? 0x7fffffff : kab;          /* nicht bestimmbar = Hoechstwert (faellt im Riegel auf) */
        if (kw > s_kmax) { s_kmax = kw; s_fkmax = f; neu = 1; }
        if (neu || s_n == 1 || (s_n % 30) == 0 || !kb || !(lb || lo) || !wasser || !gehalten || nverd > 0)
            fprintf(stderr, "[gator-re2] Maul-Sitz F%u: Bilder %d (P1 %d P2 %d P3 %d), Abweichung max %d (F%u), Kiefer-Abstand max %d (F%u), "
                    "Kopf im Bild %d/%d, Leon im Bild %d/%d + oben hinaus %d (Cut %d), im Wasser %d/%d, Opfer gehalten %d/%d, Opfer frei "
                    "%d/%d | jetzt P%d %d %d kopf=(%d,%d,%d) leon=(%d,%d,%d) leon_bild=(%d,%d) verdeckt %d/%d\n",
                    f, s_n, s_pn[1], s_pn[2], s_pn[3], (int)s_max, s_fmax, (int)s_kmax, s_fkmax, s_kopf_bild, s_n, s_leon_bild, s_n,
                    s_leon_oben, (int)g_re15_active_cut, s_wasser, s_n, s_gehalten, s_n, s_frei, s_n, ph, (int)abw, (int)kab,
                    kopf[0], kopf[1], kopf[2], leon[0], leon[1], leon[2], lx, ly, nverd, nim);
        /* NB6 (M1): das Bild der Game-Over-Uebergabe (game_step_common.c sub 6 ctr 0x6d -> g_gameover_active, main.c verlaesst danach
         * die Spielschleife zum Titel) ist das letzte Bild dieses Raums - hier die Summenzeile. */
        if (g_gameover_active && !s_ende) R41_MAUL_ENDE("Game-Over-Uebergabe");
        return;
    }
#undef R41_MAUL_ENDE
}

void re15_gr2_pc_zeichnen(const void *cam_view_v, int cx, int cy)
{
    putz_sicht((const re15_camera_view_t *)cam_view_v, cx, cy);   /* Messzeile (M2) */
    kanister_sitz();                                               /* Messzeile Nachbesserung 4 (M1) */
    fress_sitz((const re15_camera_view_t *)cam_view_v, cx, cy);   /* Messzeile Nachbesserung 5 (M1) */
    int32_t pos[3], rot[9];
    if (!re15_gr2_kanister_welt(pos, rot)) return;
    if (!modell_laden()) return;
    const re15_camera_view_t *cam_view = (const re15_camera_view_t *)cam_view_v;
    re15_render_pc_bind_tim_slot(R41_KANISTER_TIM_SLOT);

    re15_actor_lightctx_t lw, lctx;
    if (g_re15_room_lights_ok && g_re15_active_cut >= 0 && g_re15_active_cut < g_re15_room_lights.cut_count) {
        re15_light_setup_actor(&g_re15_room_lights.cuts[g_re15_active_cut], pos, NULL, &lw);
        re15_light_ctx_rotate_for_bone(&lw, rot, &lctx);
    } else {
        memset(&lctx, 0, sizeof lctx);
    }
    int32_t bm[9], bt[3];
    re15_camera_compose_view_bone(cam_view, rot, pos, bm, bt);

    /* MESSSCHIENE (stderr, kein Verhalten): die ersten drei Bilder und jeder Stufenwechsel - Lage, Drehung,
     * Bildschirm-Kasten, Tiefe, Licht. Grund: Sichtlauf e 2026-10-23 - RE15_TRILOG zeigte 110 Dreiecke je Bild
     * auf Platz 53 im Kasten (98..115, 91..107) bei z 10875..13512, im Framedump war dort nur Dunkel. Ob das
     * Beleuchtung oder Verdeckung ist, entscheidet diese Zeile, nicht das Auge. */
    static int s_diag_n, s_diag_stufe = -2;
    const int stufe = re15_gr2_kanister_stufe();
    const int diag = (s_diag_n < 3) || (stufe != s_diag_stufe);
    int d_tris = 0, d_weg = 0, d_x0 = 9999, d_x1 = -9999, d_y0 = 9999, d_y1 = -9999, d_z0 = 99999, d_z1 = -1;
    int d_c[3] = { -1, -1, -1 };

    for (int mi = 0; mi < s_md1.mesh_count; mi++) {
        const re15_md1_mesh_t *hm = &s_md1.meshes[mi];
        for (int ti = 0; ti < hm->triangle_count; ti++) {
            const re15_md1_triangle_t *tr = &hm->triangles[ti];
            if (tr->v0 >= (uint32_t)hm->tri_vertex_count || tr->v1 >= (uint32_t)hm->tri_vertex_count
                || tr->v2 >= (uint32_t)hm->tri_vertex_count) continue;
            const re15_md1_vertex_t *vp[3] = { &hm->tri_vertices[tr->v0], &hm->tri_vertices[tr->v1], &hm->tri_vertices[tr->v2] };
            int ax[3], ay[3], wz = 0;
            if (!re15_objpoly_r38_tri_mesh(vp, bm, bt, cam_view->fov_screen_dist, cx, cy, ax, ay, &wz)) { d_weg++; continue; }
            d_tris++;
            const re15_md1_tri_uv_t *uv = &hm->triangle_uvs[ti];
            int po = (int)((uv->page & 0x000F) * 128);
            uint8_t c[3][3];
            for (int k = 0; k < 3; k++) {
                uint32_t ni = (k == 0) ? tr->n0 : (k == 1) ? tr->n1 : tr->n2;
                if (hm->tri_normals && ni < (uint32_t)hm->tri_normal_count)
                    re15_light_shade_vertex(&lctx, hm->tri_normals[ni].x, hm->tri_normals[ni].y, hm->tri_normals[ni].z, &c[k][0], &c[k][1], &c[k][2]);
                else { c[k][0] = g_re15_light_tint[0]; c[k][1] = g_re15_light_tint[1]; c[k][2] = g_re15_light_tint[2]; }
            }
            re15_render_textured_tri_lit(ax[0], ay[0], (int)uv->u0 + po, (int)uv->v0,
                                         ax[1], ay[1], (int)uv->u1 + po, (int)uv->v1,
                                         ax[2], ay[2], (int)uv->u2 + po, (int)uv->v2,
                                         (int)uv->page, (int)uv->clut, wz,
                                         c[0][0], c[0][1], c[0][2], c[1][0], c[1][1], c[1][2], c[2][0], c[2][1], c[2][2]);
        }
        for (int qi = 0; qi < hm->quad_count; qi++) {
            const re15_md1_quad_t *qd = &hm->quads[qi];
            if (qd->v0 >= (uint32_t)hm->quad_vertex_count || qd->v1 >= (uint32_t)hm->quad_vertex_count
                || qd->v2 >= (uint32_t)hm->quad_vertex_count || qd->v3 >= (uint32_t)hm->quad_vertex_count) continue;
            const re15_md1_vertex_t *vp[4] = { &hm->quad_vertices[qd->v0], &hm->quad_vertices[qd->v1],
                                               &hm->quad_vertices[qd->v2], &hm->quad_vertices[qd->v3] };
            int ax[4], ay[4], wz = 0; uint8_t ok[2];
            if (!re15_objpoly_r38_quad_mesh(vp, bm, bt, cam_view->fov_screen_dist, cx, cy, ax, ay, &wz, ok)) { d_weg++; continue; }
            d_tris++;
            const re15_md1_quad_uv_t *uv = &hm->quad_uvs[qi];
            int po = (int)((uv->page & 0x000F) * 128);
            uint8_t c[4][3];
            for (int k = 0; k < 4; k++) {
                uint32_t ni = (k == 0) ? qd->n0 : (k == 1) ? qd->n1 : (k == 2) ? qd->n2 : qd->n3;
                if (hm->quad_normals && ni < (uint32_t)hm->quad_normal_count)
                    re15_light_shade_vertex(&lctx, hm->quad_normals[ni].x, hm->quad_normals[ni].y, hm->quad_normals[ni].z, &c[k][0], &c[k][1], &c[k][2]);
                else { c[k][0] = g_re15_light_tint[0]; c[k][1] = g_re15_light_tint[1]; c[k][2] = g_re15_light_tint[2]; }
            }
            if (diag) {
                for (int k = 0; k < 4; k++) {
                    if (ax[k] < d_x0) d_x0 = ax[k]; if (ax[k] > d_x1) d_x1 = ax[k];
                    if (ay[k] < d_y0) d_y0 = ay[k]; if (ay[k] > d_y1) d_y1 = ay[k];
                }
                if (wz < d_z0) d_z0 = wz; if (wz > d_z1) d_z1 = wz;
                if (d_c[0] < 0 || (int)c[0][0] + c[0][1] + c[0][2] > d_c[0] + d_c[1] + d_c[2]) {
                    d_c[0] = c[0][0]; d_c[1] = c[0][1]; d_c[2] = c[0][2];   /* hellster Scheitel 0 */
                }
            }
            if (ok[0])
                re15_render_textured_tri_lit(ax[0], ay[0], (int)uv->u0 + po, (int)uv->v0,
                                             ax[1], ay[1], (int)uv->u1 + po, (int)uv->v1,
                                             ax[3], ay[3], (int)uv->u3 + po, (int)uv->v3,
                                             (int)uv->page, (int)uv->clut, wz,
                                             c[0][0], c[0][1], c[0][2], c[1][0], c[1][1], c[1][2], c[3][0], c[3][1], c[3][2]);
            if (ok[1])
                re15_render_textured_tri_lit(ax[0], ay[0], (int)uv->u0 + po, (int)uv->v0,
                                             ax[3], ay[3], (int)uv->u3 + po, (int)uv->v3,
                                             ax[2], ay[2], (int)uv->u2 + po, (int)uv->v2,
                                             (int)uv->page, (int)uv->clut, wz,
                                             c[0][0], c[0][1], c[0][2], c[3][0], c[3][1], c[3][2], c[2][0], c[2][1], c[2][2]);
        }
    }
    if (diag) {
        s_diag_n++; s_diag_stufe = stufe;
        fprintf(stderr, "[gator-re2] Kanister Stufe %d pos=(%d,%d,%d) rot=[%d %d %d|%d %d %d|%d %d %d] "
                        "Kasten=(%d..%d,%d..%d) z=%d..%d Dreiecke %d weg %d hellster=(%d,%d,%d) "
                        "ambient=(%d,%d,%d) tint=(%d,%d,%d) cut=%d\n",
                stufe, pos[0], pos[1], pos[2], rot[0], rot[1], rot[2], rot[3], rot[4], rot[5], rot[6], rot[7], rot[8],
                d_x0, d_x1, d_y0, d_y1, d_z0, d_z1, d_tris, d_weg, d_c[0], d_c[1], d_c[2],
                (int)lctx.ambient[0], (int)lctx.ambient[1], (int)lctx.ambient[2],
                (int)g_re15_light_tint[0], (int)g_re15_light_tint[1], (int)g_re15_light_tint[2], g_re15_active_cut);
    }
}

/* ---- (2) Raumbank ROOM40A0 (Mini-Bank) --------------------------------------------------------- */
static int        s_bank_state;         /* 0 ungeprueft, 1 geladen, -1 fehlt */
static re15_vab_t s_vab;
static uint8_t   *s_edt;
static int16_t   *s_decoded[RE15_VAB_MAX_SAMPLES];
static int        s_decoded_len[RE15_VAB_MAX_SAMPLES];

static int bank_laden(void)
{
    if (s_bank_state) return s_bank_state > 0;
    s_bank_state = -1;
    int sz = 0;
    uint8_t *vbs = re15_pc_read_re2("GATOR40A0.VBS", &sz);
    if (!vbs) { fprintf(stderr, "[gator-re2] shared_assets/RE2/GATOR40A0.VBS fehlt -> Raumbrueller stumm\n"); return 0; }
    if ((unsigned)sz < R41_GATOR_VBD_OFF + R41_GATOR_VBD_SIZE || R41_GATOR_EDT_SIZE < 12) { free(vbs); return 0; }
    s_edt = (uint8_t *)malloc(R41_GATOR_EDT_SIZE);
    if (!s_edt) { free(vbs); return 0; }
    memcpy(s_edt, vbs, R41_GATOR_EDT_SIZE);
    uint32_t vh_off = (uint32_t)s_edt[R41_GATOR_EDT_SIZE-8] | ((uint32_t)s_edt[R41_GATOR_EDT_SIZE-7] << 8)
                    | ((uint32_t)s_edt[R41_GATOR_EDT_SIZE-6] << 16) | ((uint32_t)s_edt[R41_GATOR_EDT_SIZE-5] << 24);
    if (vh_off + 0x20u > R41_GATOR_EDT_SIZE
        || re15_vab_parse(s_edt + vh_off, (size_t)R41_GATOR_EDT_SIZE - vh_off, &s_vab) != 0) { free(vbs); return 0; }
    const uint8_t *vb = vbs + R41_GATOR_VBD_OFF;
    for (int i = 0; i < s_vab.vag_count && i < RE15_VAB_MAX_SAMPLES; i++) {
        uint32_t off = s_vab.samples[i].offset, vsz = s_vab.samples[i].size;
        if (off + vsz > R41_GATOR_VBD_SIZE) continue;
        size_t cap = (vsz / 16) * 28;
        int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
        if (!pcm) continue;
        s_decoded[i] = pcm;
        s_decoded_len[i] = re15_vag_adpcm_decode(vb + off, vsz, pcm, cap);
    }
    free(vbs);
    s_bank_state = 1;
    return 1;
}

static void raum_se(int satz_re2)
{
    int satz = (satz_re2 == 15) ? R41_GATOR_SE_EXPLOSION : (satz_re2 == 14) ? R41_GATOR_SE_TOD : -1;
    int ok = bank_laden();
    /* Messzeile (Abnahme 0, M6): der Raum-Brueller war in der exe nicht nachweisbar - debug.log traegt
     * jetzt jeden Ruf des Hakens (15 = Explosion @0x8010237c-88, 14 = Tod). */
    fprintf(stderr, "[gator-re2] Raum-SE %d -> GATOR40A0.VBS Satz %d%s\n", satz_re2, satz,
            ok ? "" : " (Bank fehlt, stumm)");
    if (!ok || satz < 0) return;
    re15_audio_se_play_bank(s_edt, &s_vab, s_decoded, s_decoded_len, satz);
}

void re15_pc_gator_re2_init(void)
{
    re15_gr2_raum_se_hook = raum_se;
}

/* (3) RE2-Zeitlupe der Kiefer-Explosion: die RE2-Hauptschleife wartet VSync(DAT_800dfc1a) (FUN_8002b968
 * @0x8002b990-98); R1 setzt 4 statt 2 (@0x8010235c-64 / zurueck @0x80102668/7c) = doppelte Bilddauer. Der Port-
 * Deckel (main.c, 1000/RE15_FPS ms je Bild = VSync(2)) wird im selben Verhaeltnis gestreckt. */
/* (5) Nachbesserung 3 (Abnahme 2 M1, Orchestrator-Entscheid B9): der Boss-Alligator ist bis zum Durchbruch UNTERGETAUCHT
 * wie RE2 (ROOM40A0 Sce_em_set y = +3000 @RDT 0x25A0, erst der Pos_set @SCD 0x0736 hebt ihn im Intro) - der NPC-Zeichner
 * (main.c, vor Schatten und Mesh) laesst den Aktor aus, solange re15_gator_boss_untergetaucht gilt. Messzeilen (debug.log):
 *   "[gator-re2] Sicht F<n> untergetaucht|aufgetaucht pos=(x,y,z) clip=<c> bild=<b>"        je Wechsel,
 *   "[gator-re2] Sicht erstes Bild F<n> gezeichnet pos=... clip=<c> bild=<b> (untergetaucht vorher <k> Bilder)",
 *   "[gator-re2] Sicht Lagesprung max <d> F<n> (x0,z0)->(x1,z1) clip=<c> bild=<b>"            groesster Wurzelschritt
 *   zwischen zwei aufeinanderfolgenden aufgetauchten Bildern (gezeichnet oder vom Regions-Cull verworfen); der Sprung
 *   vom Versteck auf die Intro-Startlage zaehlt nicht (er faellt in ein untergetauchtes Bild). */
int re15_gr2_pc_untergetaucht(const re15_actor_t *npc, int gecullt)
{
    static int s_war = -1, s_erst = 0, s_unter_n = 0;
    static unsigned s_lf = 0, s_raum = 0xffffffffu;
    static int32_t s_lx = 0, s_lz = 0, s_max = 0;
    if (!npc || npc->type != 0x23u) return 0;
    unsigned f = (unsigned)g_engine.frame_count;
    int u = re15_gator_boss_untergetaucht(npc);
    if (s_raum != (unsigned)g_current_room_id || (u && npc->state == 0 && s_war == 0)) {
        s_raum = (unsigned)g_current_room_id;              /* Raumeintritt / Neuspawn: Messung neu */
        s_war = -1; s_erst = 0; s_unter_n = 0; s_max = 0; s_lf = 0;
    }
    if (u != s_war) {
        fprintf(stderr, "[gator-re2] Sicht F%u %s pos=(%d,%d,%d) clip=%d bild=%d\n", f, u ? "untergetaucht" : "aufgetaucht",
                npc->x, npc->y, npc->z, (int)npc->motion, (int)npc->anim_frame);
        s_war = u;
    }
    if (u) { s_unter_n++; s_lf = 0; return 1; }
    if (!gecullt && !s_erst) {
        s_erst = 1;
        fprintf(stderr, "[gator-re2] Sicht erstes Bild F%u gezeichnet pos=(%d,%d,%d) clip=%d bild=%d (untergetaucht vorher %d Bilder)\n",
                f, npc->x, npc->y, npc->z, (int)npc->motion, (int)npc->anim_frame, s_unter_n);
    }
    if (s_lf != 0 && f == s_lf + 1u) {
        int64_t dx = (int64_t)npc->x - s_lx, dz = (int64_t)npc->z - s_lz, d2 = dx * dx + dz * dz;
        int32_t d = (int32_t)re15_squareroot0((uint32_t)(d2 > 0xffffffffLL ? 0xffffffffLL : d2));
        if (d > s_max) {
            s_max = d;
            fprintf(stderr, "[gator-re2] Sicht Lagesprung max %d F%u (%d,%d)->(%d,%d) clip=%d bild=%d\n", (int)d, f,
                    (int)s_lx, (int)s_lz, npc->x, npc->z, (int)npc->motion, (int)npc->anim_frame);
        }
    }
    s_lf = f; s_lx = npc->x; s_lz = npc->z;
    return 0;
}

unsigned re15_gr2_pc_bildbudget(unsigned grund_ms)
{
    static int s_alt = 2;
    int v = re15_gr2_vsync();
    if (v != s_alt) {
        fprintf(stderr, "[gator-re2] VSync %d (Bildbudget %u ms) ab F%u t=%u ms\n", v, grund_ms * (unsigned)v / 2u,
                (unsigned)g_engine.frame_count, (unsigned)SDL_GetTicks());
        s_alt = v;
    }
    return grund_ms * (unsigned)v / 2u;
}
