/*
 * schrank2010_r41.c — Runde 41 Spur N "nachtrag30": der Ventil-Sicherungskasten ROOM2010/2011 klappt auf, die Sicherung wird wie in ROOM2060 mit dem Cursor eingesetzt.
 * Belege und Lage: include/re15_schrank2010_r41.h, Dossier analysis/befunde_runde41/N_nachtrag30.md §1.
 */
#include "re15_schrank2010_r41.h"

#include <stdio.h>
#include <string.h>
#include "re15_audio.h"          /* re15_audio_re2_panel_se */
#include "re15_light.h"          /* re15_actor_lightctx_t */
#include "re15_scd.h"            /* g_scd.props, re15_game_flag_get/_set */
#include "re15_ventilkette_r41.h"

#ifdef RE15_PLATFORM_PC
#include "gen/schrank2010_cursor_r41.inc"
#define LOG(...) fprintf(stderr, __VA_ARGS__)
#else
#define LOG(...) ((void)0)
#endif

extern uint16_t g_scd_pad_held;   /* gehaltener virtueller Pad (scd_vm.c), Lesewort von Sce_key_ck */

#define NAH_AUS     0
#define NAH_CURSOR  1             /* Cut 8 + Cursor, wartet ohne Zeitgrenze (sub18) */
#define NAH_DRIN    2             /* Cut 9 + Text bis zum Standbild der Ventile (sub19) */

static int     s_winkel = 0, s_laeuft = 0, s_offen = 0, s_nah = NAH_AUS;
static int32_t s_cx = RE15_R41N_CUR_X0, s_cz = RE15_R41N_CUR_Z0;

void re15_schrank2010_r41_reset(void)
{
    s_winkel = 0; s_laeuft = 0; s_offen = 0; s_nah = NAH_AUS;
    s_cx = RE15_R41N_CUR_X0; s_cz = RE15_R41N_CUR_Z0;
}

int re15_schrank2010_r41_prop(int obj)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].active && g_scd.props[k].obj_id == obj) return k;
    return -1;
}

static int prop_anlegen(int obj, int32_t x, int32_t z)
{
    int p = re15_schrank2010_r41_prop(obj);
    if (p < 0) {
        if (g_scd.prop_count >= RE15_SCD_MAX_PROPS) return -1;
        p = (int)g_scd.prop_count++;
        memset(&g_scd.props[p], 0, sizeof g_scd.props[p]);
        g_scd.props[p].active     = 1;
        g_scd.props[p].obj_id     = (uint8_t)obj;
        g_scd.props[p].obj_type   = 0;
        g_scd.props[p].band       = RE15_R41N_SCHRANK_BAND;
        g_scd.props[p].parent_obj = -1;
        g_scd.props[p].flags      = 0x000Bu;         /* wie die Blaetter 2030 (Dokumente Runde 34, @0x80040998) */
    }
    g_scd.props[p].x = x; g_scd.props[p].y = 0; g_scd.props[p].z = z;
    g_scd.props[p].rot_x = g_scd.props[p].rot_y = g_scd.props[p].rot_z = 0;
    return p;
}

static void tuer_stellen(void)
{
    const int a = re15_schrank2010_r41_prop(RE15_R41N_SCHRANK_OBJ_AUSSEN), b = re15_schrank2010_r41_prop(RE15_R41N_SCHRANK_OBJ_TUER);
    if (a >= 0) g_scd.props[a].rot_y = (int16_t)s_winkel;
    if (b >= 0) g_scd.props[b].rot_y = (int16_t)s_winkel;
}

int re15_schrank2010_r41_auf_zone(void)
{
    const int32_t dx = s_cx - RE15_R41N_ZONE_X, dz = s_cz - RE15_R41N_ZONE_Z;
    return dx <= RE15_R41N_ZONE_HALB && dx >= -RE15_R41N_ZONE_HALB && dz <= RE15_R41N_ZONE_HALB && dz >= -RE15_R41N_ZONE_HALB;   /* wie re15_object_notch_update (CENTRE) */
}

/* Ein Bild Cursor-Schritt: erst die Bestaetigung (Member_cmp liest die Zelle des Vorbilds, der Stempel steht vor der SCD), dann die vier Polls. */
static void cursor_takt(void)
{
    const uint16_t held = g_scd_pad_held;
    if ((held & RE15_R41N_TASTE_SETZEN) && re15_schrank2010_r41_auf_zone()) {       /* @0x01324 + @0x0132E -> sub19 */
        s_nah = NAH_DRIN;
        re15_game_flag_set(5, RE15_R41C_FLAG_NAHBILD, 0);                           /* While des Programms endet: Bit 125, Text 42 */
        re15_audio_re2_panel_se(RE15_PANEL_SE_KLICK);                               /* Port-Regel op_sce_key_ck, RE2 ROOM2130 @0x01192 */
        LOG("[schrank2010] Nahansicht ROOM2060 Cut %d, Cursor (%d,%d) auf Zone 7\n", RE15_R41N_NAH_DRIN, (int)s_cx, (int)s_cz);
        return;
    }
    if (held & RE15_R41N_TASTE_HOCH)   s_cz += RE15_R41N_CUR_SCHRITT;
    if (held & RE15_R41N_TASTE_RUNTER) s_cz -= RE15_R41N_CUR_SCHRITT;
    if (held & RE15_R41N_TASTE_RECHTS) s_cx += RE15_R41N_CUR_SCHRITT;
    if (held & RE15_R41N_TASTE_LINKS)  s_cx -= RE15_R41N_CUR_SCHRITT;
    if (s_cx < RE15_R41N_CUR_X_MIN) s_cx = RE15_R41N_CUR_X_MIN;
    if (s_cx > RE15_R41N_CUR_X_MAX) s_cx = RE15_R41N_CUR_X_MAX;
    if (s_cz < RE15_R41N_CUR_Z_MIN) s_cz = RE15_R41N_CUR_Z_MIN;
    if (s_cz > RE15_R41N_CUR_Z_MAX) s_cz = RE15_R41N_CUR_Z_MAX;
}

void re15_schrank2010_r41_tick(void)
{
    if (re15_game_flag_get(5, RE15_R41C_FLAG_SCHRANK)) {
        if (s_offen) {
            re15_game_flag_set(5, RE15_R41C_FLAG_SCHRANK, 0);        /* schon auf: das Programm laeuft sofort weiter */
        } else if (!s_laeuft) {
            s_laeuft = 1;
            (void)prop_anlegen(RE15_R41N_SCHRANK_OBJ_INNEN, 0, 0);    /* Netz in Weltkoordinaten */
            (void)prop_anlegen(RE15_R41N_SCHRANK_OBJ_AUSSEN, RE15_R41N_ANGEL_X, RE15_R41N_ANGEL_Z);
            (void)prop_anlegen(RE15_R41N_SCHRANK_OBJ_TUER, RE15_R41N_ANGEL_X, RE15_R41N_ANGEL_Z);
            re15_audio_re2_panel_se(RE15_PANEL_SE_KLICK);             /* RE2 ROOM2130 sub04 @0x01192 */
            LOG("[schrank2010] Tuer klappt auf\n");
        }
    }
    if (s_laeuft) {
        s_winkel += RE15_R41N_AUF_SCHRITT;                            /* ROOM11F0 sub06 @0x01332 */
        tuer_stellen();
        if (s_winkel >= RE15_R41N_AUF_TAKTE * RE15_R41N_AUF_SCHRITT) {
            s_laeuft = 0; s_offen = 1;
            re15_game_flag_set(5, RE15_R41C_FLAG_SCHRANK, 0);         /* While des Programms endet */
            LOG("[schrank2010] Tuer auf (rot_y %d)\n", s_winkel);
        }
    }
    /* Einsetz-Ansicht des Ja-Zweigs wie ROOM2060 sub18 @0x0168C -> sub19 @0x016A2. */
    if (s_nah == NAH_AUS) {
        if (re15_game_flag_get(5, RE15_R41C_FLAG_NAHBILD)) {
            s_nah = NAH_CURSOR;
            s_cx = RE15_R41N_CUR_X0; s_cz = RE15_R41N_CUR_Z0;
            LOG("[schrank2010] Nahansicht ROOM2060 Cut %d, Cursor obj 4 bei (%d,%d)\n", RE15_R41N_NAH_LEER, (int)s_cx, (int)s_cz);
        }
    } else if (s_nah == NAH_CURSOR) {
        cursor_takt();
    } else if (re15_game_flag_get(5, RE15_R41C_FLAG_STANDBILD)) {
        s_nah = NAH_AUS;                                              /* das Standbild der Ventile uebernimmt die Vollbild-Ebene */
    }
}

int re15_schrank2010_r41_nahbild(int *cut)
{
    if (s_nah == NAH_AUS) return 0;
    if (cut) *cut = s_nah == NAH_CURSOR ? RE15_R41N_NAH_LEER : RE15_R41N_NAH_DRIN;
    return 1;
}

void re15_schrank2010_r41_cursor(int32_t *x, int32_t *z) { if (x) *x = s_cx; if (z) *z = s_cz; }

static int bild_versatz(int32_t welt)
{
    const int32_t v = welt * RE15_R41N_CUR_BILD_ZAEHLER;
    return (int)((v >= 0 ? v + RE15_R41N_CUR_BILD_NENNER / 2 : v - RE15_R41N_CUR_BILD_NENNER / 2) / RE15_R41N_CUR_BILD_NENNER);   /* gerundet: F606 -> F607 links 3 Bildpunkte */
}

void re15_schrank2010_r41_cursor_bild(int *x, int *y)
{
    if (x) *x = RE15_R41N_CUR_BILD_X + bild_versatz(s_cx - RE15_R41N_CUR_X0);
    if (y) *y = RE15_R41N_CUR_BILD_Y - bild_versatz(s_cz - RE15_R41N_CUR_Z0);
}

#ifdef RE15_PLATFORM_PC
static uint32_t s_cursor_rgba[320 * 240];

const uint32_t *re15_schrank2010_r41_cursor_rgba(const uint32_t *grund)
{
    if (s_nah != NAH_CURSOR || !grund) return NULL;
    memcpy(s_cursor_rgba, grund, sizeof s_cursor_rgba);
    int mx = 0, my = 0;
    re15_schrank2010_r41_cursor_bild(&mx, &my);
    for (int i = 0; i < (int)(sizeof re15_r41n_schrank_cursor / sizeof re15_r41n_schrank_cursor[0]); i++) {
        const re15_r41n_stempelpx_t *c = &re15_r41n_schrank_cursor[i];
        const int x = mx + c->dx, y = my + c->dy;
        if ((unsigned)x < 320u && (unsigned)y < 240u)
            s_cursor_rgba[y * 320 + x] = ((uint32_t)c->r << 24) | ((uint32_t)c->g << 16) | ((uint32_t)c->b << 8) | 0xFFu;
    }
    return s_cursor_rgba;
}
#else
const uint32_t *re15_schrank2010_r41_cursor_rgba(const uint32_t *grund) { (void)grund; return NULL; }
#endif

int re15_schrank2010_r41_verdeckt(uint16_t raum, int cut, int obj)
{
    return (raum & 0xFFFEu) == RE15_R41C_RAUM_2010 && obj >= RE15_R41N_SCHRANK_OBJ_INNEN && obj <= RE15_R41N_SCHRANK_OBJ_TUER && cut != RE15_R41N_SCHRANK_CUT;
}

/* Die drei Texturen stammen aus fertig beleuchteten Hintergruenden (BG06, BG03): Licht neutral, damit sie unveraendert erscheinen (Umgebung 255 = Farbe 0x80 =
 * Textur 1:1, RE15_FACE_RGB_CODE, re15_light.h) und die Tuer geschlossen im Hintergrund verschwindet. PORT-WAHL: die Original-Props der Raeume werden beleuchtet. */
void re15_schrank2010_r41_licht(uint16_t raum, int obj_id, re15_actor_lightctx_t *ctx)
{
    if (!ctx || (raum & 0xFFFEu) != RE15_R41C_RAUM_2010) return;
    if (obj_id < RE15_R41N_SCHRANK_OBJ_INNEN || obj_id > RE15_R41N_SCHRANK_OBJ_TUER) return;
    for (int c = 0; c < 3; c++) {
        ctx->ambient[c] = 255;
        for (int l = 0; l < 3; l++) ctx->C[l][c] = 0;
    }
}

int re15_schrank2010_r41_offen(void)  { return s_offen; }
int re15_schrank2010_r41_winkel(void) { return s_winkel; }
