/*
 * schrank2010_r41.c — Runde 41 Spur N "nachtrag30": der Ventil-Sicherungskasten ROOM2010/2011 klappt auf und zeigt die Nahansichten des Kastens 2060.
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
#define LOG(...) fprintf(stderr, __VA_ARGS__)
#else
#define LOG(...) ((void)0)
#endif

static int s_winkel = 0, s_laeuft = 0, s_offen = 0, s_nah_takt = -1;

void re15_schrank2010_r41_reset(void)
{
    s_winkel = 0; s_laeuft = 0; s_offen = 0; s_nah_takt = -1;
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
    /* Nahansichten des Ja-Zweigs: Cut 8, dann Cut 9, wie ROOM2060 sub18 @0x0168C -> sub19 @0x016A2. */
    if (re15_game_flag_get(5, RE15_R41C_FLAG_NAHBILD)) {
        if (s_nah_takt < 0) { s_nah_takt = 0; LOG("[schrank2010] Nahansicht ROOM2060 Cut %d\n", RE15_R41N_NAH_LEER); }
        else if (++s_nah_takt == RE15_R41N_NAHBILD_TAKTE) LOG("[schrank2010] Nahansicht ROOM2060 Cut %d\n", RE15_R41N_NAH_DRIN);
        if (s_nah_takt >= 2 * RE15_R41N_NAHBILD_TAKTE) {
            s_nah_takt = -1;
            re15_game_flag_set(5, RE15_R41C_FLAG_NAHBILD, 0);
        }
    }
}

int re15_schrank2010_r41_nahbild(int *cut)
{
    if (s_nah_takt < 0) return 0;
    if (cut) *cut = s_nah_takt < RE15_R41N_NAHBILD_TAKTE ? RE15_R41N_NAH_LEER : RE15_R41N_NAH_DRIN;
    return 1;
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
