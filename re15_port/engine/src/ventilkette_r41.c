/*
 * RE1.5 Rebuilt — Runde 41 Spur C "ventilkette": Leiter 2000 / Bruecke + Ventile 2030 / Flut -> 20B0 /
 * Schaltpult 2050 / Strom 2060 -> 2030 auf Relinghoehe (Nachbesserung 3).
 *
 * Nachbesserung 4 (Nachtraege 12/13): Pegel mit Strom knapp unter der Absperrung (y -5350), das echte Ventilraetsel
 * (sechs Ventile, Reihenfolge, falsch -> zurueck), Dokument "Technician's Note" und Plakat mit der falschen Anleitung.
 * Nachbesserung 5 (Nachtraege 16/18/19, Abnahme 4): Ventil-Sicherungskasten ROOM2010 (Bit (9,125)) statt der "look
 * around"-Sperre, Standbild der Ventile + Aufschliess-Ton beim Einsetzen, Ventil-Dreh-Animation aus RE2 ROOM1080 sub08,
 * Text 46 im Wortlaut des Nutzers; Plakat waehrend Zaehler/Flut gesperrt (Abnahme 4 Mangel 2).
 * Herleitung, Belege und alle Konstanten: include/re15_ventilkette_r41.h; Dossier
 * analysis/befunde_runde41/C_ventilkette.md. Kein Asset-Patch: die RDTs bleiben byte-true. Die Port-Programme
 * (SCD-Bytecode, jede Zeile mit Vorbild) liegen in gen/ventilkette_r41_programme.inc, die Texte in
 * gen/ventilkette_r41_texte.inc, die Original-Scheiben in gen/ventilkette_r41_original.inc, die Wasserflaeche
 * (ROOM2080 Prop 0) in gen/ventilkette_r41_wasser.inc — alle aus tools/r41_ventilkette/gen_r41_ventilkette.py.
 */
#include "re15_ventilkette_r41.h"

#include <string.h>
#include "re15_aot.h"        /* g_aot, re15_aot_set, re15_aot_retype */
#include "re15_scd.h"        /* g_scd, scd_event_fire, re15_game_flag_* */
#include "re15_actor.h"      /* g_actors, RE15_ACTOR_SLOT_PLAYER */
#include "re15_room.h"       /* g_current_room_id */
#include "re15_msg.h"        /* re15_msg_install_text / _durations / _compute_duration */
#include "re15_audio.h"      /* re15_audio_re2_panel_se, RE15_PANEL_SE_* */
#include "re15_fade.h"       /* re15_fade_config / _kick / _kill */
#include "re15_esp.h"        /* re15_esp_fx_spawn_rows, re15_esp_room_bank */
#include "re15_wasser_r38.h" /* RE15_WASSER_R38_CODE_BODEN (Wasser-Schritt-Effekt FUN_80045630) */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include "re15_engine.h"     /* g_engine.frame_count (Protokoll), re15_render_tile (Zeichner) */
#endif

#include "gen/ventilkette_r41_texte.inc"
#include "gen/ventilkette_r41_original.inc"
#include "gen/ventilkette_r41_programme.inc"
#ifdef RE15_PLATFORM_PC
#include "gen/ventilkette_r41_wasser.inc"
#include "gen/ventilkette_r41_plakat.inc"   /* Nachbesserung 4: Plakat (Nahaufnahme + Aushang) und Dokument-Blatt */
#endif

extern uint16_t g_scd_pad_edge;    /* virtuelle Flanke (SQUARE = 0x40, CROSS = 0x8000; pad_common.c vtbl) */
extern void re15_scd_show_message(uint8_t index, uint32_t pause_mask);

#ifdef RE15_PLATFORM_PC
#define LOG(...) fprintf(stderr, __VA_ARGS__)
#else
#define LOG(...) ((void)0)
#endif

/* ---- Zustand (nicht gespeichert; die Spielbits stehen in Bank 9) ------------------------------------------ */
static int      s_timer   = 0;      /* 2030: Zaehler laeuft (Faden schlaeft 1800 Takte)                      */
static int      s_timer_gesehen = 0;/* 2030: Marker (5,0x1D) schon verbraucht (einmal je Raumstart)          */
static int      s_flut    = 0;      /* 2030: Flut laeuft (Wasser steigt)                                      */
static int      s_ertrunken = 0;
static int      s_fade_an = 0;
static int      s_v13_vorher = -1;  /* Flanke von Bit 101 fuer den Klick                                      */
static unsigned s_letzter_raum = 0xFFFFu;
static int      s_wasser_prop[2] = { -1, -1 };
static int      s_fuesse_nass = 0;  /* 2030: die steigende Flaeche hat die Fuesse erreicht (einmal je Flut)    */
static int      s_spritzer_n = 0;   /* Spritzer-Ereignisse seit dem Raumstart (Riegel)                        */
/* 2050 Pult */
static int      s_panel_aktiv = 0;
static unsigned s_schalter = 0;
static int      s_ruhe = 0;
static int      s_panel_phase = 0;  /* 0 frei, 1 geloest: Satz laeuft, 2 Ausstieg angefordert */
static int      s_cursor_vorher = -1;
/* 2030 Ventilraetsel + Plakat (Nachbesserung 4) */
static int      s_folge[RE15_R41C_DREHUNGEN];   /* Ventil-Index je vermerkter Drehung                          */
static int      s_ventil_gewaehlt = -1;         /* Ventil des zuletzt gefeuerten Platzes (Ereignis 17..22)    */
static int      s_ventile_geloest = 0;          /* richtige Folge -> Ereignis 26 freigegeben                  */
static int      s_falsch_offen = 0;             /* falsche Folge -> Ereignis 27 freigegeben                   */
static int      s_plakat_offen = 0;
static int      s_plakat_takte = 0;
static int      s_dok_vorher = -1;              /* Flanke von Bit 107 (Protokoll der Ablage)                    */
/* Nachbesserung 5 */
static int      s_standbild_offen = 0;          /* 2010: Standbild der Ventile (2030 Cut 7) offen                */
static int      s_standbild_takte = 0;
static unsigned s_ton_n[3];                     /* gespielte Toene je RE15_R41C_TON_* (Riegel)                   */
static int      s_kasten_vorher = -1;           /* Flanke von Bit 125 (Protokoll)                                 */
static int      s_zaehler_takte = 0;            /* Countdown: Takte seit dem Zaehlerstart (Sleep 1800)            */
void (*re15_ventilkette_r41_ton_hook)(int ton) = NULL;

static const int     k_ventil_nr[RE15_R41C_VENTILE_N]     = RE15_R41C_VENTIL_NUMMERN;
static const int32_t k_ventil_x[RE15_R41C_VENTILE_N]      = RE15_R41C_VENTIL_X;
static const int32_t k_ventil_wand_z[RE15_R41C_VENTILE_N] = RE15_R41C_VENTIL_WAND_Z;
static const int     k_loesung[RE15_R41C_DREHUNGEN]       = RE15_R41C_LOESUNG;
static const char   *const k_ventil_prog[RE15_R41C_VENTILE_N] = {
    "2030_ventil_01", "2030_ventil_02", "2030_ventil_03", "2030_ventil_07", "2030_ventil_08", "2030_ventil_09" };

static const int k_gewichte[RE15_R41C_PANEL_N] = RE15_R41C_PANEL_GEWICHTE;
static const int32_t k_zone_x0[RE15_R41C_PANEL_N] = RE15_R41C_PANEL_X0;

static int ist(uint16_t room, uint16_t leon) { return (room & 0xFFFEu) == leon; }
static int bit(int b) { return re15_game_flag_get(RE15_R41C_BANK, (uint8_t)b) ? 1 : 0; }
static void bit_setzen(int b) { re15_game_flag_set(RE15_R41C_BANK, (uint8_t)b, 1); }
static int strom(void) { return re15_game_flag_get(RE15_R41C_STROM_BANK, RE15_R41C_STROM_BIT) ? 1 : 0; }   /* (3,126) @0x012AE */
static int sicherung(void) { return re15_game_flag_get(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT) ? 1 : 0; } /* (3,108) @0x01FF2 */
static int generator(void) { return re15_game_flag_get(RE15_R41C_SICHERUNG_BANK, RE15_R41C_GENERATOR_BIT) ? 1 : 0; } /* (3,144) @0x0169E */

/* Ton ueber den Plattform-Haken (PC: platform/pc/src/ventilkette_r41_pc.c); gezaehlt wird immer (Riegel). */
static void ton(int t)
{
    if (t >= 1 && t <= 2) s_ton_n[t]++;
    if (re15_ventilkette_r41_ton_hook) re15_ventilkette_r41_ton_hook(t);
}

/* Zustand von 2030 nach der Flut (Nachbesserung 3, Nachtraege 7/8): haengt direkt am Original-Bit (3,126), das nur
 * ROOM2060 sub01 @0x012AE setzt und kein Skript loescht (Zensus, Kopf). */
int re15_ventilkette_r41_zustand_2030(void)
{
    if (!bit(RE15_R41C_BIT_FLUT_GELAUFEN)) return RE15_R41C_2030_TROCKEN;
    return strom() ? RE15_R41C_2030_RELING : RE15_R41C_2030_GEFLUTET;
}

/* ---- Texte ------------------------------------------------------------------------------------------------- */
static void text_einsetzen(uint8_t id, const uint8_t *m, int n)
{
    re15_msg_install_text(id, m, (size_t)n);
    int d = re15_msg_compute_duration(m, (size_t)n, 0);   /* Dauer-Eintrag wie re15_msg_load_room_block */
    if (d > 0 && d < 65535) re15_msg_install_durations(id, d);
}

const uint8_t *re15_ventilkette_r41_text(uint16_t room_id, uint8_t id, int *out_len)
{
    const uint16_t gruppe = (uint16_t)(room_id & 0xFFFEu);
    for (int i = 0; i < RE15_R41C_TEXTE_N; i++)   /* eigene Texte der Raumgruppe (20B0: Id 41), dann 2000 (Leiter Id 40) */
        if ((re15_r41c_texte[i].raum == gruppe ||
             (gruppe == RE15_R41C_RAUM_20B0 && re15_r41c_texte[i].raum == RE15_R41C_RAUM_2000)) && re15_r41c_texte[i].id == id) {
            if (out_len) *out_len = re15_r41c_texte[i].n;
            return re15_r41c_texte[i].b;
        }
    {   /* Nachbesserung 5: Original-Saetze (Scheiben, gen/ventilkette_r41_original.inc) */
        const uint8_t *o = NULL; int n = 0;
        if (gruppe == RE15_R41C_RAUM_2010 && id == RE15_R41C_MSG_KASTEN_FRAGE)    { o = re15_r41c_org_2060_msg4; n = (int)sizeof re15_r41c_org_2060_msg4; }
        if (gruppe == RE15_R41C_RAUM_2010 && id == RE15_R41C_MSG_KASTEN_BENUTZT)  { o = re15_r41c_org_2060_msg5; n = (int)sizeof re15_r41c_org_2060_msg5; }
        if (gruppe == RE15_R41C_RAUM_2010 && id == RE15_R41C_MSG_KASTEN_GENOMMEN) { o = re15_r41c_org_2030_msg3; n = (int)sizeof re15_r41c_org_2030_msg3; }
        if (gruppe == RE15_R41C_RAUM_2030 && id == RE15_R41C_MSG_VENTILE_VORHER)  { o = re15_r41c_org_20a0_msg1; n = (int)sizeof re15_r41c_org_20a0_msg1; }
        if (o) { if (out_len) *out_len = n; return o; }
    }
    if (gruppe == RE15_R41C_RAUM_2050 && id == RE15_R41C_MSG_PULT_FRAGE) {
        if (out_len) *out_len = (int)sizeof re15_r41c_org_11f0_msg1; return re15_r41c_org_11f0_msg1; }
    if (gruppe == RE15_R41C_RAUM_2050 && id == RE15_R41C_MSG_PULT_OK) {
        if (out_len) *out_len = (int)sizeof re15_r41c_org_11f0_msg2; return re15_r41c_org_11f0_msg2; }
    if (out_len) *out_len = 0;
    return NULL;
}

static void texte_einsetzen(uint16_t room_id)
{
    const uint16_t gruppe = (uint16_t)(room_id & 0xFFFEu);
    for (int i = 0; i < RE15_R41C_TEXTE_N; i++)   /* 20B0 traegt den Leiter-Satz der 2000 (Id 40) und eigene (Id 41) */
        if (re15_r41c_texte[i].raum == gruppe ||
            (gruppe == RE15_R41C_RAUM_20B0 && re15_r41c_texte[i].raum == RE15_R41C_RAUM_2000))
            text_einsetzen(re15_r41c_texte[i].id, re15_r41c_texte[i].b, re15_r41c_texte[i].n);
    if (gruppe == RE15_R41C_RAUM_2050) {
        text_einsetzen(RE15_R41C_MSG_PULT_FRAGE, re15_r41c_org_11f0_msg1, (int)sizeof re15_r41c_org_11f0_msg1);
        text_einsetzen(RE15_R41C_MSG_PULT_OK,    re15_r41c_org_11f0_msg2, (int)sizeof re15_r41c_org_11f0_msg2);
    }
    if (gruppe == RE15_R41C_RAUM_2010) {   /* Nachbesserung 5 (Nachtrag 16): Kasten, Original-Saetze 2060/2030 */
        text_einsetzen(RE15_R41C_MSG_KASTEN_FRAGE,    re15_r41c_org_2060_msg4, (int)sizeof re15_r41c_org_2060_msg4);
        text_einsetzen(RE15_R41C_MSG_KASTEN_BENUTZT,  re15_r41c_org_2060_msg5, (int)sizeof re15_r41c_org_2060_msg5);
        text_einsetzen(RE15_R41C_MSG_KASTEN_GENOMMEN, re15_r41c_org_2030_msg3, (int)sizeof re15_r41c_org_2030_msg3);
    }
    if (gruppe == RE15_R41C_RAUM_2030)      /* Nachbesserung 5: Ventil ohne Kasten-Sicherung = "It's locked." */
        text_einsetzen(RE15_R41C_MSG_VENTILE_VORHER, re15_r41c_org_20a0_msg1, (int)sizeof re15_r41c_org_20a0_msg1);
}

/* ---- Programme --------------------------------------------------------------------------------------------- */
const uint8_t *re15_ventilkette_r41_programm(const char *name, int *out_len)
{
    for (int i = 0; i < RE15_R41C_PROGRAMME_N; i++)
        if (!strcmp(re15_r41c_programme[i].name, name)) {
            if (out_len) *out_len = re15_r41c_programme[i].n;
            return re15_r41c_programme[i].b;
        }
    if (out_len) *out_len = 0;
    return NULL;
}

/* ---- Plaetze ----------------------------------------------------------------------------------------------- */
static int16_t le_s16(const uint8_t *p) { return (int16_t)(uint16_t)(p[0] | (p[1] << 8)); }

/* Ereignis-Platz in der Form ROOM2050 main00 @0x01D38 `2c ss 03 31 bb 00 x z w d ff 00 18 ee 00 00` (sce 3,
 * sat 0x31, Nutzlast ff 00 18 <Ereignis>). Muster kartenitems_r36.c: re15_aot_set + re15_aot_retype. */
static int ereignis_platz(int slot, int32_t x, int32_t z, int32_t w, int32_t d, uint8_t band, uint8_t ev)
{
    if (slot < 0 || slot >= RE15_AOT_MAX) return 0;
    if (g_aot.slots[slot].active) {
        LOG("[ventilkette41] Platz %d belegt (Typ %d) -> Ereignis %u nicht angelegt\n", slot, g_aot.slots[slot].type, ev);
        return 0;
    }
    re15_aot_set(slot, RE15_AOT_TYPE_GENERIC, ev, x + w / 2, z + d / 2, w / 2, d / 2);
    g_aot.slots[slot].sce_flags = 0;
    g_aot.slots[slot].band      = band;
    re15_aot_retype(slot, 3, RE15_R41C_SAT, 0x00FF, (uint16_t)((ev << 8) | 0x18), 0);
    return 1;
}

/* Aot_set-Satz (20 B, kurze Form) aus den Original-Bytes anlegen (wie untersuchen_r40.c text_platz). */
static int satz_anlegen(const uint8_t *pc)
{
    const int slot = (int)pc[1];
    if (slot >= RE15_AOT_MAX || g_aot.slots[slot].active) return 0;
    const int16_t rx = le_s16(&pc[6]), rz = le_s16(&pc[8]), rw = le_s16(&pc[10]), rd = le_s16(&pc[12]);
    if (pc[2] == 3) return ereignis_platz(slot, rx, rz, rw, rd, pc[4], pc[17]);
    return 0;
}

/* Tuer/Platz -> Sperrtext (Form ROOM2030 main00 @0x01B12 bzw. Aot_reset sub01 @0x01E18 `46 ss 01 31 mm 00 ff ff`). */
static void sperrtext(int slot, uint8_t msg)
{
    re15_aot_retype(slot, 1, RE15_R41C_SAT, msg, RE15_R41C_PAUSE, 0);
}

/* ---- Props ------------------------------------------------------------------------------------------------- */
static int prop_von_obj(int obj_id)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].active && g_scd.props[k].obj_id == obj_id) return k;
    return -1;
}

/* Prop aus einem Obj_model_set-Satz (34 B) anlegen — Felder wie op_obj_model_set (LAB_80040914). */
static int prop_anlegen_satz(const uint8_t *pc)
{
    int k = prop_von_obj(pc[1]);
    if (k >= 0) return k;
    if (g_scd.prop_count >= RE15_SCD_MAX_PROPS) return -1;
    k = (int)g_scd.prop_count++;
    memset(&g_scd.props[k], 0, sizeof g_scd.props[k]);
    g_scd.props[k].active     = 1;
    g_scd.props[k].obj_id     = pc[1];
    g_scd.props[k].obj_type   = pc[2];
    g_scd.props[k].band       = pc[4];
    g_scd.props[k].parent_obj = ((pc[5] & 0xC0) == 0xC0) ? (int8_t)(pc[5] & 0x3F) : (int8_t)-1;
    g_scd.props[k].flags      = (uint16_t)((pc[6] | (pc[7] << 8)) | 1u);
    g_scd.props[k].attr       = (uint16_t)(pc[8] | (pc[9] << 8));
    g_scd.props[k].x = le_s16(&pc[10]); g_scd.props[k].y = le_s16(&pc[12]); g_scd.props[k].z = le_s16(&pc[14]);
    g_scd.props[k].rot_x = le_s16(&pc[16]); g_scd.props[k].rot_y = le_s16(&pc[18]); g_scd.props[k].rot_z = le_s16(&pc[20]);
    g_scd.props[k].box_cx = le_s16(&pc[22]); g_scd.props[k].box_cy = le_s16(&pc[24]); g_scd.props[k].box_cz = le_s16(&pc[26]);
    g_scd.props[k].box_hx = le_s16(&pc[28]); g_scd.props[k].box_hy = le_s16(&pc[30]); g_scd.props[k].box_hz = le_s16(&pc[32]);
    return k;
}

/* Wasserflaeche k (0/1) als Prop obj 4/5 anlegen (Felder = ROOM2080 @0x00C2E, Lage nach Kopf). */
static int wasser_anlegen_bei(int k, int32_t x, int32_t y, int32_t z);
static int wasser_anlegen(int k, int32_t y, int32_t z) { return wasser_anlegen_bei(k, RE15_R41C_WASSER_X, y, z); }
static int wasser_anlegen_bei(int k, int32_t x, int32_t y, int32_t z)
{
    const int obj = k ? RE15_R41C_WASSER_OBJ_B : RE15_R41C_WASSER_OBJ_A;
    int p = prop_von_obj(obj);
    if (p < 0) {
        if (g_scd.prop_count >= RE15_SCD_MAX_PROPS) return -1;
        p = (int)g_scd.prop_count++;
        memset(&g_scd.props[p], 0, sizeof g_scd.props[p]);
        g_scd.props[p].active     = 1;
        g_scd.props[p].obj_id     = (uint8_t)obj;
        g_scd.props[p].obj_type   = RE15_R41C_WASSER_TYP;
        g_scd.props[p].band       = 0;
        g_scd.props[p].parent_obj = -1;
        g_scd.props[p].flags      = RE15_R41C_WASSER_FLAGS;
        g_scd.props[p].attr       = RE15_R41C_WASSER_ATTR;
    }
    g_scd.props[p].x = x; g_scd.props[p].y = y; g_scd.props[p].z = z;
    g_scd.props[p].rot_x = g_scd.props[p].rot_y = g_scd.props[p].rot_z = 0;
    s_wasser_prop[k] = p;
    return p;
}

int re15_ventilkette_r41_wasser_prop(int k) { return (k == 0 || k == 1) ? s_wasser_prop[k] : -1; }

/* Dokument-Blatt (obj 6) / Plakat-Aushang (obj 7) als Prop - Felder wie ein Obj_model_set (LAB_80040914), Werte wie die
 * Dokumente der Runde 34 (re15_dokumente.h: Typ 0, Flags 0x000A | 1 @0x80040998, Welt, Box 0) bis auf das BAND: der Spieler
 * steht am Landeplatz in Band 1, und die Kletter-Sonde FUN_8002d2c0 nimmt jedes aktive Objekt mit Objekt-Band == Spieler-
 * Band (@0x8002d370) - gemessen 2026-10-24 15:3x (Lauf v2_leon, F1858): mit Band 1 stieg Leon an der Suedwand auf das
 * Plakat (mo 230/231, y -1800 -> -3600 -> -5400 auf den Ring). Band 2 liegt weder im Spielerband (Sonde) noch darunter
 * (Standtest obj[0x82] + 1 == Spielerband @0x80031cfc) -> RE15_R41C_BLATT_BAND (PORT-WAHL, Kopf). */
static int blatt_prop(int obj, int32_t x, int32_t y, int32_t z)
{
    int p = prop_von_obj(obj);
    if (p < 0) {
        if (g_scd.prop_count >= RE15_SCD_MAX_PROPS) return -1;
        p = (int)g_scd.prop_count++;
        memset(&g_scd.props[p], 0, sizeof g_scd.props[p]);
        g_scd.props[p].active     = 1;
        g_scd.props[p].obj_id     = (uint8_t)obj;
        g_scd.props[p].obj_type   = 0;
        g_scd.props[p].band       = RE15_R41C_BLATT_BAND;
        g_scd.props[p].parent_obj = -1;
        g_scd.props[p].flags      = 0x000Bu;
    }
    g_scd.props[p].x = x; g_scd.props[p].y = y; g_scd.props[p].z = z;
    g_scd.props[p].rot_x = g_scd.props[p].rot_y = g_scd.props[p].rot_z = 0;
    return p;
}

/* ---- SCA: Steg-Zelle 12 sperren/oeffnen (Form Sca_id_set `37 0r 0c ff`, alle 5 Regionen wie 2050 @0x02346) --- */
static void steg_zelle(const re15_rdt_t *rdt, uint8_t u0)
{
    if (!rdt || !rdt->sca) return;
    int base = 0;
    for (int r = 0; r < 5; r++) {
        int idx = base + RE15_R41C_STEG_ZELLE;
        if (RE15_R41C_STEG_ZELLE < rdt->sca_rgn[r]) {
            re15_sca_entry_t *e = (re15_sca_entry_t *)(void *)&rdt->sca[idx];   /* wie sca_entry_at (scd_vm.c) */
            e->u0 = u0;
        }
        base += rdt->sca_rgn[r];
    }
}

/* ---- Tuer nach 2000 -> 20B0 ------------------------------------------------------------------------------- */
int re15_ventilkette_r41_cut_20b0(int cut_2000)
{
    /* Kameratabellen RDT+0x60 selbst verglichen: 20B0 0..4 = 2000 0..4; 20B0 5..9 = 2000 6..10; 20B0 10 = 2000 14;
     * 2000 Cut 5/11/12/13/15 haben keinen Zwilling (13 -> 7 = Westgang, der RVD-Scan stellt den Rest). */
    static const int8_t k_map[16] = { 0, 1, 2, 3, 4, 4, 5, 6, 7, 8, 9, 0, 0, 7, 10, 0 };
    return (cut_2000 >= 0 && cut_2000 < 16) ? k_map[cut_2000] : 0;
}

static void tuer_nach_20b0(int slot, int16_t sx, int16_t sy, int16_t sz, int16_t yaw, int cut, const char *was)
{
    re15_aot_door_params_t *d = &g_aot.door_params[slot];
    d->dest_stage = 1;                      /* Door_aot_set pc[22] 0-basiert: 1 = STAGE2 */
    d->dest_room  = 0x0B;                   /* ROOM20B0 (|Szenario-Nibble, aot_common.c dest_id) */
    d->target_cut = (uint8_t)cut;
    if (sx || sy || sz) { d->spawn_x = sx; d->spawn_y = sy; d->spawn_z = sz; d->spawn_yaw_4096 = yaw; }
    LOG("[ventilkette41] ROOM%04X: Tuer Slot %d -> ROOM20B0 Cut %d (%s)\n", (unsigned)g_current_room_id, slot, cut, was);
}

static void tueren_umlenken(uint16_t room_id)
{
    if (!bit(RE15_R41C_BIT_FLUT_GELAUFEN)) return;
    for (int s = 0; s < RE15_AOT_MAX; s++) {
        if (!g_aot.slots[s].active || g_aot.slots[s].type != RE15_AOT_TYPE_DOOR) continue;
        const re15_aot_door_params_t *d = &g_aot.door_params[s];
        if (d->dest_stage == 1 && d->dest_room == 0x00)
            tuer_nach_20b0(s, 0, 0, 0, 0, re15_ventilkette_r41_cut_20b0(d->target_cut), "Ziel ROOM2000");
    }
    if (ist(room_id, RE15_R41C_RAUM_2030) && g_aot.slots[RE15_R41C_SLOT_2030_OSTTUER].type == RE15_AOT_TYPE_DOOR)
        tuer_nach_20b0(RE15_R41C_SLOT_2030_OSTTUER, 5950, -1800, -6400, 2048, 11, "Ost-Tuer nach der Flut");
}

/* ---- Ring-Zugaenge des gefluteten 2030 (Orchestrator-Entscheid C-E2) --------------------------------------------- */
/* Tuer -> Sperrtext, solange 2030 ueber dem Ring unter Wasser steht ("geflutet" = Bit 103 ohne Strom (3,126),
 * Nachtrag 7: mit dem Strom ist der Ring wieder trocken und offen); Zensus und Bytes im Kopf
 * (RE15_R41C_SLOT_20B0_RING / _2070_RING_A/B). Form wie die Leiter 2000 (Text-Platz sce 1 im Tuer-Rechteck, ROOM2030
 * main00 @0x01B12); das Band der Tuer (Door_aot_set pc[4]) geht auf den Platz ueber wie in install_2000. */
static void ring_sperren(uint16_t room_id, int slot, uint8_t msg)
{
    if (re15_ventilkette_r41_zustand_2030() != RE15_R41C_2030_GEFLUTET) return;
    re15_aot_t *a = &g_aot.slots[slot];
    const re15_aot_door_params_t *d = &g_aot.door_params[slot];
    if (!a->active || a->type != RE15_AOT_TYPE_DOOR || d->dest_stage != 1 || d->dest_room != RE15_R41C_RAUM_2030_ID) return;
    a->band = d->band;
    sperrtext(slot, msg);
    LOG("[ventilkette41] ROOM%04X: Ring-Tuer Slot %d nach 2030 gesperrt (geflutet) -> Sperrtext %d\n",
        (unsigned)room_id, slot, msg);
}

/* ---- Install je Raum --------------------------------------------------------------------------------------- */
static void install_2000(uint16_t room_id)
{
    texte_einsetzen(room_id);
    if (!bit(RE15_R41C_BIT_LEITER_FREI)) {
        re15_aot_t *a = &g_aot.slots[RE15_R41C_SLOT_2000_LEITER];
        if (a->active && a->type == RE15_AOT_TYPE_DOOR) {
            /* Tuersaetze tragen ihr Band in door_params (Door_aot_set pc[4]); das Band-Gate des Aktions-Scans
             * (FUN_80042cac) liest fuer Text-Plaetze das Slot-Band -> uebertragen wie adaruf_1050.c. */
            a->band = g_aot.door_params[RE15_R41C_SLOT_2000_LEITER].band;
            sperrtext(RE15_R41C_SLOT_2000_LEITER, RE15_R41C_MSG_LEITER);
            LOG("[ventilkette41] ROOM%04X: Leiter Slot 4 (@0x0169E) -> Sperrtext %d (Form ROOM2030 @0x01B12)\n",
                (unsigned)room_id, RE15_R41C_MSG_LEITER);
        }
    }
    tueren_umlenken(room_id);
    if (ist(room_id, RE15_R41C_RAUM_20B0)) ring_sperren(room_id, RE15_R41C_SLOT_20B0_RING, RE15_R41C_MSG_RING_20B0);
}

static void install_2070(uint16_t room_id)
{
    texte_einsetzen(room_id);
    tueren_umlenken(room_id);
    ring_sperren(room_id, RE15_R41C_SLOT_2070_RING_A, RE15_R41C_MSG_RING_2070);
    ring_sperren(room_id, RE15_R41C_SLOT_2070_RING_B, RE15_R41C_MSG_RING_2070);
}

static void install_2030(uint16_t room_id, const re15_rdt_t *rdt)
{
    texte_einsetzen(room_id);
    /* Bruecke: oben, solange Bit 102 = 0 (obj 0 @0x01B6C), Steg-Zelle 12 gesperrt. */
    int p = prop_von_obj(RE15_R41C_BRUECKE_OBJ);
    if (p >= 0 && !bit(RE15_R41C_BIT_BRUECKE_UNTEN)) {
        g_scd.props[p].y = RE15_R41C_BRUECKE_Y_OBEN;
        steg_zelle(rdt, 0xFF);
        LOG("[ventilkette41] ROOM%04X: Bruecke obj 0 oben y %d, Zelle 12 gesperrt\n", (unsigned)room_id, RE15_R41C_BRUECKE_Y_OBEN);
    }
    /* Die Stellen: Pult (Ring) und Ventil 13 (Tank) immer; die Ventile nur vor der Flut — danach liegen sie im
     * gefluteten unteren Bereich (Nachtrag 9: "das ist gewollt"). */
    const int zustand = re15_ventilkette_r41_zustand_2030();
    ereignis_platz(RE15_R41C_SLOT_PULT,     RE15_R41C_PULT_RECT,     RE15_R41C_PULT_BAND,     RE15_R41C_EV_PULT);
    ereignis_platz(RE15_R41C_SLOT_VENTIL13, RE15_R41C_VENTIL13_RECT, RE15_R41C_VENTIL13_BAND, RE15_R41C_EV_VENTIL13);
    if (zustand == RE15_R41C_2030_TROCKEN) {
        /* Nachbesserung 4 (Nachtrag 13): sechs Ventil-Plaetze an den Raedern (Kopf "Ventil-RAEDER"), das Plakat an der
         * Suedwand und - solange nicht genommen - das Dokument neben der Sicherung. */
        for (int k = 0; k < RE15_R41C_VENTILE_N; k++)
            ereignis_platz(RE15_R41C_SLOT_VENTIL0 + k, k_ventil_x[k] - 500, k_ventil_wand_z[k] - 500, 1000, 1000,
                           RE15_R41C_VENTILE_BAND, (uint8_t)(RE15_R41C_EV_VENTIL0 + k));
        ereignis_platz(RE15_R41C_SLOT_PLAKAT, RE15_R41C_PLAKAT_RECT, RE15_R41C_VENTILE_BAND, RE15_R41C_EV_PLAKAT);
        blatt_prop(RE15_R41C_PLAKAT_OBJ, RE15_R41C_PLAKAT_X, RE15_R41C_PLAKAT_Y, RE15_R41C_PLAKAT_Z);
        if (!bit(RE15_R41C_BIT_DOKUMENT)) {
            const int32_t dr[4] = { RE15_R41C_DOK_RECT };
            if (!g_aot.slots[RE15_R41C_SLOT_DOKUMENT].active) {
                /* Item-Zone wie op_item_aot_set / dokumente_r34.c anlegen(): Item 0x4E -> Leser (aot_common.c
                 * aot_item_dokument, Id >= 0x48), Taken-Bit (9,107), Weltmodell obj 6 wird beim Ablegen ausgeblendet. */
                re15_aot_set_item_tk_prop(RE15_R41C_SLOT_DOKUMENT, dr[0] + dr[2] / 2, dr[1] + dr[3] / 2, dr[2] / 2, dr[3] / 2,
                                          RE15_R41C_DOK_ITEM, 1, RE15_R41C_BIT_DOKUMENT, RE15_R41C_DOK_OBJ);
                g_aot.slots[RE15_R41C_SLOT_DOKUMENT].sce_flags = RE15_R41C_SAT;
                g_aot.slots[RE15_R41C_SLOT_DOKUMENT].band      = RE15_R41C_VENTILE_BAND;
            }
            blatt_prop(RE15_R41C_DOK_OBJ, RE15_R41C_DOK_X, RE15_R41C_DOK_Y, RE15_R41C_DOK_Z);
        }
        LOG("[ventilkette41] ROOM%04X: Ventile 01/02/03/07/08/09 (Slots 14-19), Plakat Slot 21, Dokument %s\n",
            (unsigned)room_id, bit(RE15_R41C_BIT_DOKUMENT) ? "genommen" : "Slot 20 (Item 0x4E)");
    }
    if (zustand == RE15_R41C_2030_GEFLUTET) {
        /* Nach der Flut ohne Strom: Wasser 1800 ueber dem Ring, Lift-Rufzonen oben = Original-Satz 1. */
        wasser_anlegen(0, RE15_R41C_WASSER_Y_RUHE, RE15_R41C_WASSER_ZA);
        wasser_anlegen(1, RE15_R41C_WASSER_Y_RUHE, RE15_R41C_WASSER_ZB);
        static const int k_ruf[2] = { 4, 6 };   /* sub00 @0x01D20 (Band 6) / @0x01DC2 (Band 9) */
        for (int i = 0; i < 2; i++)
            if (g_aot.slots[k_ruf[i]].active && g_aot.slots[k_ruf[i]].type != RE15_AOT_TYPE_NONE)
                sperrtext(k_ruf[i], RE15_R41C_MSG_LIFT_ORIGINAL);
        LOG("[ventilkette41] ROOM%04X: unter Wasser (y %d), Lift-Rufzonen 4/6 -> msg 1\n", (unsigned)room_id, RE15_R41C_WASSER_Y_RUHE);
    } else if (zustand == RE15_R41C_2030_RELING) {
        /* Nach der Flut mit Strom (Nachtraege 7/8/12): EINE Flaeche ueber der Schachtoeffnung knapp unter der Absperrung
         * (y -5350, Kopf), Ring trocken (Tueren/Lifte = Original), der einzige Weg hinab = Treppe Slot 8
         * (@0x01B8E sce 12) wird Sperrtext 48 im Band 3 (Form der Leiter 2000; Band wie install_2000 auf den Platz). */
        wasser_anlegen_bei(0, 0, RE15_R41C_WASSER_Y_RELING, 0);
        re15_aot_t *t = &g_aot.slots[RE15_R41C_SLOT_2030_TREPPE];
        if (t->active && t->type == RE15_AOT_TYPE_STAIR) {
            t->band = RE15_R41C_TREPPE_BAND;
            sperrtext(RE15_R41C_SLOT_2030_TREPPE, RE15_R41C_MSG_TREPPE_FLUT);
        }
        LOG("[ventilkette41] ROOM%04X: Strom (3,126) -> Wasser knapp unter der Absperrung y %d (Schachtoeffnung), Treppe Slot 8 -> Sperrtext %d, Ring frei\n",
            (unsigned)room_id, RE15_R41C_WASSER_Y_RELING, RE15_R41C_MSG_TREPPE_FLUT);
    }
    tueren_umlenken(room_id);
}

/* Nachbesserung 5 (Nachtrag 16): der Ventil-Sicherungskasten in ROOM2010/2011 (Lage/Form im Kopf). Vor UND nach der Flut
 * erreichbar (2000 bzw. 20B0 Slot 2 -> 2010), kein Softlock. */
static void install_2010(uint16_t room_id)
{
    texte_einsetzen(room_id);
    const int32_t r[4] = { RE15_R41C_KASTEN_RECT };
    if (ereignis_platz(RE15_R41C_SLOT_KASTEN, r[0], r[1], r[2], r[3], RE15_R41C_KASTEN_BAND, RE15_R41C_EV_KASTEN))
        LOG("[ventilkette41] ROOM%04X: Ventil-Sicherungskasten Slot %d (x %d..%d z %d..%d), Sicherung %s\n", (unsigned)room_id,
            RE15_R41C_SLOT_KASTEN, (int)r[0], (int)(r[0] + r[2]), (int)r[1], (int)(r[1] + r[3]),
            bit(RE15_R41C_BIT_KASTEN) ? "steckt (Bit 125)" : "fehlt");
    tueren_umlenken(room_id);
}

/* Nachbesserung 5 (Nachtrag 16 Folge 3): steckt die Sicherung im Ventilkasten 2010, hat der Spieler sie nicht - der
 * Generator-Kasten 2060 zeigt dann den Satz seines Else-Zweigs @0x010A6 (Text 3 "One of the fuses is missing...") statt
 * Ereignis 18 (@0x010C6, sub18 "Will you use the Fuse?"). Nach dem Herausnehmen (Bit 125 = 0) gilt wieder das Original. */
static void install_2060(uint16_t room_id)
{
    if (!bit(RE15_R41C_BIT_KASTEN) || generator()) return;
    re15_aot_t *a = &g_aot.slots[RE15_R41C_SLOT_2060_KASTEN];
    if (!a->active) return;
    sperrtext(RE15_R41C_SLOT_2060_KASTEN, RE15_R41C_MSG_2060_FEHLT);
    LOG("[ventilkette41] ROOM%04X: Sicherung steckt im Ventilkasten 2010 (Bit 125) -> Generator-Kasten Slot 9 = Text 3 (@0x010A6)\n",
        (unsigned)room_id);
}

static void install_2050(uint16_t room_id)
{
    texte_einsetzen(room_id);
    if (!bit(RE15_R41C_BIT_PULT2050)) {
        /* Slot 8 + Cursor wie main00 @0x01D38/@0x01D4C (dort nur bei (5,7)=1, das nie gesetzt wird). */
        if (satz_anlegen(re15_r41c_org_2050_slot8))
            LOG("[ventilkette41] ROOM%04X: Pult Slot 8 (@0x01D38) angelegt, Ereignis 9\n", (unsigned)room_id);
        if (prop_anlegen_satz(re15_r41c_org_2050_obj8) >= 0)
            LOG("[ventilkette41] ROOM%04X: Cursor obj 8 (@0x01D4C) angelegt\n", (unsigned)room_id);
    } else {
        if (satz_anlegen(re15_r41c_org_2050_slot8)) sperrtext(RE15_R41C_SLOT_2050_PULT, RE15_R41C_MSG_PULT_ZUSTAND);
    }
    tueren_umlenken(room_id);
}

void re15_ventilkette_r41_install(uint16_t room_id, const re15_rdt_t *rdt)
{
    /* Raumwechsel waehrend des Zaehlers = Flut gelaufen (Nutzer: "Verlaesst man den Raum fuehrt das zu 20B0"). */
    if (s_timer && !ist(room_id, RE15_R41C_RAUM_2030)) {
        bit_setzen(RE15_R41C_BIT_FLUT_GELAUFEN);
        bit_setzen(RE15_R41C_BIT_LEITER_FREI);
        LOG("[ventilkette41] Zaehler lief beim Verlassen von 2030 -> Bits 103 + 100 (Flut gelaufen, Leiter frei)\n");
    }
    s_timer = 0; s_timer_gesehen = 0; s_flut = 0; s_ertrunken = 0; s_fuesse_nass = 0; s_spritzer_n = 0;
    s_wasser_prop[0] = s_wasser_prop[1] = -1;
    s_panel_aktiv = 0; s_schalter = 0; s_ruhe = 0; s_panel_phase = 0; s_cursor_vorher = -1;
    s_ventil_gewaehlt = -1; s_ventile_geloest = 0; s_falsch_offen = 0; s_plakat_offen = 0; s_plakat_takte = 0;
    s_standbild_offen = 0; s_standbild_takte = 0; s_zaehler_takte = 0;
    for (int i = 0; i < RE15_R41C_DREHUNGEN; i++) s_folge[i] = -1;
    if (s_fade_an) { re15_fade_kill(RE15_R41C_FADE_KANAL); s_fade_an = 0; }
    if (ist(room_id, RE15_R41C_RAUM_2010))            /* Nachbesserung 5: ROOM2010/2011 benutzen Bank 5 nicht (scd_dump_room.py) */
        re15_game_flag_set(5, RE15_R41C_FLAG_STANDBILD, 0);
    if (ist(room_id, RE15_R41C_RAUM_2030)) {          /* nur der eigene Raum (Nachbesserung 1, M3): anderswo sind
                                                       * (5,0x1D)/(5,0x1E) Original-Bits, z. B. ROOM1171 sub02 @0x013B2
                                                       * `22 05 1e 01` im Init-Takt (Kraehen DIVE-ARM @0x801155e8) */
        re15_game_flag_set(5, RE15_R41C_FLAG_TIMER, 0);   /* Raum-Scratch, wie das Raumladen (FUN_8003ecec @0x8003ed74) */
        re15_game_flag_set(5, RE15_R41C_FLAG_FLUT, 0);
        for (int i = 0; i < RE15_R41C_DREHUNGEN; i++) re15_game_flag_set(5, (uint8_t)(RE15_R41C_FLAG_DREHUNG0 + i), 0);
        re15_game_flag_set(5, RE15_R41C_FLAG_GEDREHT, 0);  /* Nachbesserung 4: Ventile + Plakat */
        re15_game_flag_set(5, RE15_R41C_FLAG_PLAKAT, 0);
        re15_game_flag_set(5, RE15_R41C_FLAG_TON, 0);      /* Nachbesserung 5: Ventil-Ton */
    }
    s_v13_vorher = bit(RE15_R41C_BIT_VENTIL13);
    s_dok_vorher = bit(RE15_R41C_BIT_DOKUMENT);
    s_kasten_vorher = bit(RE15_R41C_BIT_KASTEN);
    s_letzter_raum = room_id;

    if (ist(room_id, RE15_R41C_RAUM_2000) || ist(room_id, RE15_R41C_RAUM_20B0)) install_2000(room_id);
    else if (ist(room_id, RE15_R41C_RAUM_2030)) install_2030(room_id, rdt);
    else if (ist(room_id, RE15_R41C_RAUM_2050)) install_2050(room_id);
    else if (ist(room_id, RE15_R41C_RAUM_2070)) install_2070(room_id);
    else if (ist(room_id, RE15_R41C_RAUM_2010)) install_2010(room_id);     /* Nachbesserung 5: + Ventilkasten */
    else if (ist(room_id, RE15_R41C_RAUM_2060)) install_2060(room_id);     /* Nachbesserung 5: Generator-Kasten */
    else if (ist(room_id, RE15_R41C_RAUM_1260))
        tueren_umlenken(room_id);
}

void re15_ventilkette_r41_reset(void)
{
    if (s_fade_an) { re15_fade_kill(RE15_R41C_FADE_KANAL); s_fade_an = 0; }
    s_timer = 0; s_timer_gesehen = 0; s_flut = 0; s_ertrunken = 0; s_fuesse_nass = 0; s_spritzer_n = 0; s_v13_vorher = -1; s_letzter_raum = 0xFFFFu;
    s_wasser_prop[0] = s_wasser_prop[1] = -1;
    s_panel_aktiv = 0; s_schalter = 0; s_ruhe = 0; s_panel_phase = 0; s_cursor_vorher = -1;
    s_ventil_gewaehlt = -1; s_ventile_geloest = 0; s_falsch_offen = 0; s_plakat_offen = 0; s_plakat_takte = 0;
    s_standbild_offen = 0; s_standbild_takte = 0; s_kasten_vorher = -1; s_zaehler_takte = 0;
    for (int i = 0; i < 3; i++) s_ton_n[i] = 0;
    for (int i = 0; i < RE15_R41C_DREHUNGEN; i++) s_folge[i] = -1;
}

/* ---- Ereignisse (scd_event_fire-Kette) --------------------------------------------------------------------- */
const uint8_t *re15_ventilkette_r41_ereignis(uint16_t room_id, uint8_t event_id)
{
    const char *name = NULL;
    if (ist(room_id, RE15_R41C_RAUM_2030)) {
        if (event_id == RE15_R41C_EV_PULT)
            name = bit(RE15_R41C_BIT_BRUECKE_UNTEN) ? "2030_pult_unten"
                 : bit(RE15_R41C_BIT_VENTIL13)      ? "2030_pult_senken" : "2030_pult_ohne";
        else if (event_id == RE15_R41C_EV_VENTIL13)
            name = bit(RE15_R41C_BIT_VENTIL13) ? "2030_v13_zustand" : "2030_v13_frage";
        else if (event_id >= RE15_R41C_EV_VENTIL0 && event_id < RE15_R41C_EV_VENTIL0 + RE15_R41C_VENTILE_N &&
                 !bit(RE15_R41C_BIT_FLUT_GELAUFEN)) {
            /* Nachbesserung 4 (Nachtrag 13): ein Ventil. Nachtrag 9: erst mit gesenkter Bruecke (Bit 102); Nachtrag 16
             * (ersetzt Nachtrag 10): nur solange die Sicherung im Ventil-Sicherungskasten 2010 steckt (Bit 125), sonst
             * Satz 49 = Original "It's locked."; waehrend des Zaehlers (und zwischen der richtigen Folge und dem
             * Zaehlerstart) die Warnung erneut. Nach der Flut gibt es die Plaetze nicht. */
            s_ventil_gewaehlt = event_id - RE15_R41C_EV_VENTIL0;
            name = (s_timer || s_ventile_geloest) ? "2030_ventile_laeuft"
                 : (bit(RE15_R41C_BIT_BRUECKE_UNTEN) && bit(RE15_R41C_BIT_KASTEN)) ? k_ventil_prog[s_ventil_gewaehlt]
                 : "2030_ventile_vorher";
        }
        else if (event_id == RE15_R41C_EV_FLUT && s_ventile_geloest && !s_timer)
            name = "2030_ventile_flut";                        /* vom C-Takt nach der richtigen Folge */
        else if (event_id == RE15_R41C_EV_FALSCH && s_falsch_offen) {
            s_falsch_offen = 0;
            name = "2030_ventile_falsch";                      /* vom C-Takt nach vier Drehungen in falscher Folge */
        }
        else if (event_id == RE15_R41C_EV_PLAKAT && !s_plakat_offen && !bit(RE15_R41C_BIT_FLUT_GELAUFEN) &&
                 !s_timer && !s_ventile_geloest && !s_flut)
            name = "2030_plakat";   /* Abnahme 4 Mangel 2: waehrend Zaehler/Flut kein Plakat (deren Pad-Sperre bleibt) */
    } else if (ist(room_id, RE15_R41C_RAUM_2010)) {
        /* Nachbesserung 5 (Nachtrag 16): Ventil-Sicherungskasten - Zustaende wie der Original-Kasten 2060 sub00
         * @0x0109E..@0x010DE: steckt (Bit 125) -> Text + Frage zum Herausnehmen; aufgehoben ((3,108)) und nicht im
         * Generator ((3,144)) -> Frage zum Einsetzen; sonst nur der Text "One of the fuses is missing...". */
        if (event_id == RE15_R41C_EV_KASTEN)
            name = bit(RE15_R41C_BIT_KASTEN) ? "2010_kasten_drin"
                 : (sicherung() && !generator()) ? "2010_kasten_frage" : "2010_kasten_leer";
    } else if (ist(room_id, RE15_R41C_RAUM_2050)) {
        /* Hebel-Platz Slot 6: Ereignis 4 (sub00 @0x01E28 -> sub04 @0x022E6) UND Ereignis 2 (Kisten im Becken, sub01
         * @0x0201A -> sub02 @0x0220E, Speed -20 For 180 @0x0227C) heben das Wasser -> beide erst nach dem Pult
         * (Nachbesserung 1, M1). */
        if ((event_id == RE15_R41C_EV_2050_HEBEL || event_id == RE15_R41C_EV_2050_KISTEN) && !bit(RE15_R41C_BIT_PULT2050))
            name = "2050_hebel_sperre";
        else if (event_id == RE15_R41C_EV_2050_PULT)
            name = bit(RE15_R41C_BIT_PULT2050) ? "2050_pult_zustand" : "2050_pult_ein";
    }   /* 2080: Rechner = nur Original (Nachtrag 7) */
    if (!name) return NULL;
    const uint8_t *p = re15_ventilkette_r41_programm(name, NULL);
    LOG("[ventilkette41] ROOM%04X Ereignis %u -> Port-Programm %s\n", (unsigned)room_id, (unsigned)event_id, name);
    return p;
}

/* ---- 20B0: die drei Strahlen an der Leiter (Nutzer: nach der Flut kein Wasser mehr) --------------------------- */
int re15_ventilkette_r41_espr_unterdrueckt(uint16_t room_id, const uint8_t *pc, const uint8_t *raw)
{
    if (!ist(room_id, RE15_R41C_RAUM_20B0) || !raw || !pc) return 0;
    if (!bit(RE15_R41C_BIT_LEITER_FREI)) return 0;
    const uintptr_t off = (uintptr_t)(pc - raw);
    return off == 0x01E96u || off == 0x01EA6u || off == 0x01EB6u;   /* sub00 Else-Zweig, `3a 00 0b ...` */
}

/* ---- 2050 Pult --------------------------------------------------------------------------------------------- */
int re15_ventilkette_r41_spannung_aus_maske(unsigned maske)
{
    int v = 0;
    for (int i = 0; i < RE15_R41C_PANEL_N; i++) if (maske & (1u << i)) v += k_gewichte[i];
    return v;
}
unsigned re15_ventilkette_r41_schalter(void) { return s_schalter; }
int re15_ventilkette_r41_spannung(void) { return re15_ventilkette_r41_spannung_aus_maske(s_schalter); }
int re15_ventilkette_r41_panel_aktiv(void) { return s_panel_aktiv; }

int re15_ventilkette_r41_panel_zone(int32_t x, int32_t z)
{
    if (z < RE15_R41C_PANEL_ZONE_Z0 || z >= RE15_R41C_PANEL_ZONE_Z0 + RE15_R41C_PANEL_ZONE_D) return -1;
    for (int i = 0; i < RE15_R41C_PANEL_N; i++)
        if (x >= k_zone_x0[i] && x < k_zone_x0[i] + RE15_R41C_PANEL_ZONE_W) return i;
    return -1;
}

int re15_ventilkette_r41_zeiger_x(int wert)
{
    if (wert < 0) wert = 0;
    if (wert > 100) wert = 100;
    return (RE15_R41C_SKALA_X0_100 + RE15_R41C_SKALA_PX_100 * wert + 50) / 100;   /* 165.5 + 0.91 * Wert */
}

static void panel_tick(void)
{
    const int aktiv = re15_game_flag_get(5, 4) ? 1 : 0;   /* sub09 @0x023FE Set(5,4)=1, sub10 @0x02426 := 0 */
    if (!aktiv) {
        if (s_panel_aktiv) LOG("[ventilkette41] Pult 2050 verlassen (Schalter 0x%X, %d V)\n", s_schalter, re15_ventilkette_r41_spannung());
        s_panel_aktiv = 0; s_panel_phase = 0; s_ruhe = 0; s_cursor_vorher = -1;
        return;
    }
    s_panel_aktiv = 1;
    if (bit(RE15_R41C_BIT_PULT2050)) {
        /* geloest: Satz laeuft, danach Ausstieg (Original sub10 @0x0240C) */
        if (s_panel_phase == 1 && g_scd.message_display_frames <= 0) { scd_event_fire(RE15_R41C_EV_2050_AUS); s_panel_phase = 2; }
        return;
    }
    const int c = prop_von_obj(RE15_R41C_CURSOR_OBJ);
    const int zone = (c >= 0) ? re15_ventilkette_r41_panel_zone(g_scd.props[c].x, g_scd.props[c].z) : -1;
    if (g_scd.message_display_frames <= 0) {
        if (g_scd_pad_edge & 0x0040u) {                    /* SQUARE-Flanke (Sce_key_ck 0x40, 2060 @0x011D2) */
            if (zone >= 0) {
                s_schalter ^= (1u << zone);
                s_ruhe = 0;
                re15_audio_re2_panel_se(RE15_PANEL_SE_KLICK);  /* RE2 ROOM2130 sub04 @0x01192 */
                LOG("[ventilkette41] Pult 2050: Hebel %d -> %s, Schalter 0x%X, %d V\n", zone + 1,
                    (s_schalter >> zone) & 1u ? "EIN" : "AUS", s_schalter, re15_ventilkette_r41_spannung());
            }
        } else if (g_scd_pad_edge & 0x8000u) {             /* CROSS-Flanke = Abbruch (Form hebetisch_cursor) */
            LOG("[ventilkette41] Pult 2050: Abbruch\n");
            scd_event_fire(RE15_R41C_EV_2050_AUS);
            return;
        }
    }
    if (s_schalter == RE15_R41C_PANEL_LOESUNG && re15_ventilkette_r41_spannung() == RE15_R41C_PANEL_ZIEL) {
        if (++s_ruhe >= RE15_R41C_PANEL_RUHE) {           /* RE2 @0x0171C sleep 30, dann @0x01752 cmp == 80 */
            bit_setzen(RE15_R41C_BIT_PULT2050);
            re15_audio_re2_panel_se(RE15_PANEL_SE_BESTAET); /* RE2 @0x01762 se 0x0C */
            re15_scd_show_message(RE15_R41C_MSG_PULT_OK, (uint32_t)RE15_R41C_PAUSE << 16);
            s_panel_phase = 1;
            LOG("[ventilkette41] Pult 2050 GELOEST: 80 V -> Bit 106, \"Power supply OK.\"\n");
        }
    } else s_ruhe = 0;
}

/* ---- 2030 Takt --------------------------------------------------------------------------------------------- */
static void flut_tick(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (s_wasser_prop[0] < 0) { s_wasser_prop[0] = prop_von_obj(RE15_R41C_WASSER_OBJ_A); s_wasser_prop[1] = prop_von_obj(RE15_R41C_WASSER_OBJ_B); }
    if (s_wasser_prop[0] < 0) return;
    const int32_t wy = g_scd.props[s_wasser_prop[0]].y;
    if (s_ertrunken) return;
    /* Spritzer: das Original setzt den Wasser-Schritt-Effekt nur je Fuss-Aufsetzer (Bildwort-Bit 0x4000
     * `andi v0,v1,0x4000` @0x8003253c -> `jal 0x80045630` @0x8003255c; dort Wasserboden `andi v0,s0,0x80`
     * @0x800456ac -> EIN Effekt 0x06050D00 `lui a0,0x605`/`ori a0,a0,0xd00` @0x800456b8/bc, Gier +0x6a @0x800456d0,
     * `jal 0x80019700` @0x80045710). Waehrend der Flut steht der Spieler (Pad-Sperre), es gibt keinen Aufsetzer;
     * der Port setzt den Effekt deshalb als EREIGNIS genau einmal, wenn die steigende Flaeche die Fuesse erreicht
     * (PORT-WAHL ohne eigene Rate; Anker = Spielerlage auf der Flaeche statt Part[Fuss]+0x40 + (0,100,0)). */
    if (!s_fuesse_nass && wy <= pl->y) {
        s_fuesse_nass = 1;
        s_spritzer_n++;
        const uint32_t code = RE15_WASSER_R38_CODE_BODEN;
        re15_esp_fx_spawn_rows(re15_esp_room_bank(), (uint8_t)(code >> 24), (uint8_t)((code >> 16) & 0xffu),
                               (uint16_t)(code & 0xffffu), pl->x, wy, pl->z, INT32_MAX, pl->rot_y);
        LOG("[ventilkette41] SPRITZER: Wasser y %d erreicht die Fuesse (Spieler y %d), Effekt 0x06050D00\n",
            (int)wy, (int)pl->y);
    }
    /* Ertrinken, sobald die Flaeche 900 ueber den Fuessen steht — spaetestens auf der Tuerebene (Endlage -16200):
     * "Zeit abgelaufen und noch im Raum: Tot" gilt auf jeder Ebene (NUTZER-VORGABE). */
    if (wy <= pl->y - RE15_R41C_ERTRINK_TIEFE || wy <= RE15_R41C_WASSER_Y_ENDE) {
        s_ertrunken = 1;
        s_timer = 0;                                        /* Tod: keine Flut-Bits beim naechsten Raumstart */
        if (pl->hp >= 0) pl->hp = -1;                       /* tot = hp < 0 (main.c RE15_KILL_AT, game_step_common.c) */
        re15_fade_config(RE15_R41C_FADE_KANAL, RE15_R41C_FADE_ABR, RE15_R41C_FADE_MASKE, 0, RE15_R41C_FADE_BUCKET);
        re15_fade_kick(RE15_R41C_FADE_KANAL, RE15_R41C_FADE_LEVEL);
        s_fade_an = 1;
        LOG("[ventilkette41] ERTRUNKEN: Wasser y %d, Spieler y %d -> hp -1, blaue Blende\n", (int)wy, (int)pl->y);
    }
}

/* ---- 2030 Ventilraetsel (Nachbesserung 4): Drehung vermerken, nach der vierten pruefen -------------------------------- */
int re15_ventilkette_r41_drehungen(void)
{
    int n = 0;
    while (n < RE15_R41C_DREHUNGEN && re15_game_flag_get(5, (uint8_t)(RE15_R41C_FLAG_DREHUNG0 + n))) n++;
    return n;
}
int re15_ventilkette_r41_folge(int i) { return (i >= 0 && i < RE15_R41C_DREHUNGEN) ? s_folge[i] : -1; }
int re15_ventilkette_r41_ventil_nummer(int i) { return (i >= 0 && i < RE15_R41C_VENTILE_N) ? k_ventil_nr[i] : -1; }
int re15_ventilkette_r41_plakat_offen(void) { return s_plakat_offen; }

static void ventile_tick(void)
{
    if (!re15_game_flag_get(5, RE15_R41C_FLAG_GEDREHT)) return;
    re15_game_flag_set(5, RE15_R41C_FLAG_GEDREHT, 0);           /* Handshake verbraucht */
    const int n = re15_ventilkette_r41_drehungen();
    if (n >= RE15_R41C_DREHUNGEN || s_ventil_gewaehlt < 0) return;
    s_folge[n] = s_ventil_gewaehlt;
    re15_game_flag_set(5, (uint8_t)(RE15_R41C_FLAG_DREHUNG0 + n), 1);   /* Form der Schalterbits ROOM11F0 (5,13..22) */
    re15_audio_re2_panel_se(RE15_PANEL_SE_KLICK);                         /* RE2 ROOM2130 sub04 @0x01192 */
    LOG("[ventilkette41] Ventil %02d gedreht (Drehung %d/%d)\n", k_ventil_nr[s_ventil_gewaehlt], n + 1, RE15_R41C_DREHUNGEN);
    if (n + 1 < RE15_R41C_DREHUNGEN) return;
    /* Pruefung erst nach der letzten Drehung (RE2 ROOM2130 sub04 @0x01752): keine Rueckmeldung je Schritt. */
    int richtig = 1;
    for (int i = 0; i < RE15_R41C_DREHUNGEN; i++) if (s_folge[i] != k_loesung[i]) richtig = 0;
    LOG("[ventilkette41] Folge %02d %02d %02d %02d -> %s\n", k_ventil_nr[s_folge[0]], k_ventil_nr[s_folge[1]],
        k_ventil_nr[s_folge[2]], k_ventil_nr[s_folge[3]], richtig ? "RICHTIG: Warnung, Zaehler, Flut" : "FALSCH: zurueckgesetzt");
    if (richtig) {
        s_ventile_geloest = 1;
        (void)scd_event_fire(RE15_R41C_EV_FLUT);
    } else {
        for (int i = 0; i < RE15_R41C_DREHUNGEN; i++) s_folge[i] = -1;
        re15_audio_re2_panel_se(RE15_R41C_SE_FEHLER);                    /* RE2 ROOM2130 sub04 @0x01800 */
        s_falsch_offen = 1;
        (void)scd_event_fire(RE15_R41C_EV_FALSCH);                       /* (5,0x10..0x13) := 0 + Text 45 */
    }
}

/* ---- 2030 Plakat-Nahaufnahme (Nachbesserung 4): offen ab dem Marker, zu mit der Bestaetigen-/Abbrechen-Flanke -------- */
static void plakat_tick(void)
{
    if (!s_plakat_offen) {
        if (!re15_game_flag_get(5, RE15_R41C_FLAG_PLAKAT)) return;
        s_plakat_offen = 1; s_plakat_takte = 0;
        LOG("[ventilkette41] Plakat-Nahaufnahme offen\n");
        return;
    }
    s_plakat_takte++;
    /* Erst ab dem Folgebild (die oeffnende Flanke ist verbraucht); virtuell 0x4000/0x8000 = die Tasten, mit denen das
     * Original eine Nachricht wegdrueckt (andi 0xc000 @0x80028698, msg_common.c dismiss_edge). */
    if (s_plakat_takte > 1 && (g_scd_pad_edge & 0xC000u)) {
        s_plakat_offen = 0;
        re15_game_flag_set(5, RE15_R41C_FLAG_PLAKAT, 0);
        /* Abnahme 4 Mangel 2: nur die EIGENE Sperre zuruecknehmen - laeuft die Flut (deren Programm setzt (2,7)/(2,2)
         * selbst, sub-Form ROOM2030 sub02 @0x01E30), bleibt sie stehen. */
        if (!s_flut && !re15_game_flag_get(5, RE15_R41C_FLAG_FLUT)) {
            re15_game_flag_set(2, 7, 0);                                 /* Form ROOM1051 sub03 @0x00DD2 */
            re15_game_flag_set(2, 2, 0);                                 /* Form ROOM11F0 sub17 @0x016EC */
        }
        LOG("[ventilkette41] Plakat-Nahaufnahme zu (nach %d Bildern)\n", s_plakat_takte);
    }
}

#ifdef RE15_PLATFORM_PC
static uint32_t s_plakat_rgba[RE15_R41C_PLAKAT_W * RE15_R41C_PLAKAT_H];
static int      s_plakat_dekodiert = 0;
#endif
const uint32_t *re15_ventilkette_r41_plakat_rgba(void)
{
#ifdef RE15_PLATFORM_PC
    if (!s_plakat_offen) return NULL;
    if (!s_plakat_dekodiert) {
        for (int i = 0; i < RE15_R41C_PLAKAT_W * RE15_R41C_PLAKAT_H; i++) {
            const uint8_t b = re15_r41c_plakat_4bpp[i >> 1];
            const int v = (i & 1) ? (b >> 4) : (b & 15);
            const uint8_t *c = &re15_r41c_plakat_pal[v * 3];
            s_plakat_rgba[i] = ((uint32_t)c[0] << 24) | ((uint32_t)c[1] << 16) | ((uint32_t)c[2] << 8) | 0xFFu;
        }
        s_plakat_dekodiert = 1;
    }
    return s_plakat_rgba;
#else
    return NULL;
#endif
}

/* ---- 2010 Ventilkasten (Nachbesserung 5): Standbild der Ventile ab dem Marker, zu mit der Bestaetigen-/Abbrechen-Flanke -- */
static void kasten_tick(void)
{
    const int k = bit(RE15_R41C_BIT_KASTEN);
    if (s_kasten_vorher >= 0 && k != s_kasten_vorher)
        LOG("[ventilkette41] Ventil-Sicherungskasten 2010: Sicherung %s -> Bit (9,125) = %d\n", k ? "eingesetzt" : "genommen", k);
    s_kasten_vorher = k;
    if (!s_standbild_offen) {
        if (!re15_game_flag_get(5, RE15_R41C_FLAG_STANDBILD)) return;
        s_standbild_offen = 1; s_standbild_takte = 0;
        ton(RE15_R41C_TON_AUF);                                          /* RE2 Satz 0x26 im selben Bild (@0x800515f8) */
        LOG("[ventilkette41] Standbild der Ventile offen (ROOM2030 Cut %d) + Aufschliess-Ton\n", RE15_R41C_STANDBILD_CUT);
        return;
    }
    s_standbild_takte++;
    if (s_standbild_takte > 1 && (g_scd_pad_edge & 0xC000u)) {        /* wie die Plakat-Nahaufnahme (@0x80028698) */
        s_standbild_offen = 0;
        re15_game_flag_set(5, RE15_R41C_FLAG_STANDBILD, 0);             /* das Programm laeuft weiter (While-Ende) */
        LOG("[ventilkette41] Standbild der Ventile zu (nach %d Bildern)\n", s_standbild_takte);
    }
}

int re15_ventilkette_r41_standbild(unsigned *raum, int *cut)
{
    if (!s_standbild_offen || !ist((uint16_t)g_current_room_id, RE15_R41C_RAUM_2010)) return 0;
    if (raum) *raum = RE15_R41C_STANDBILD_RAUM;
    if (cut) *cut = RE15_R41C_STANDBILD_CUT;
    return 1;
}
unsigned re15_ventilkette_r41_ton_zaehler(int t) { return (t >= 1 && t <= 2) ? s_ton_n[t] : 0u; }
int re15_ventilkette_r41_ventil_rbj(unsigned room_id) { return ist((uint16_t)room_id, RE15_R41C_RAUM_2030); }

/* Countdown (Nachbesserung 5, Nachtrag 18 Punkt 4): sichtbar, solange in 2030 der Zaehler laeuft und die Flut noch nicht
 * begonnen hat; Restbilder = 1800 - Takte seit dem Zaehlerstart (der Sleep 1800 desselben Fadens, Kopf). */
int re15_ventilkette_r41_countdown(int *rest_bilder)
{
    if (!s_timer || s_flut || s_ertrunken || !ist((uint16_t)g_current_room_id, RE15_R41C_RAUM_2030)) return 0;
    int rest = RE15_R41C_ZAEHLER_BILDER - s_zaehler_takte;
    if (rest < 0) rest = 0;
    if (rest_bilder) *rest_bilder = rest;
    return 1;
}

int re15_ventilkette_r41_countdown_text(int rest, uint8_t *out, int cap)
{
    if (!out || cap < 14) return 0;
    if (rest < 0) rest = 0;
    const int min = rest / (60 * 30), sek = (rest / 30) % 60, hund = (rest % 30) * 100 / 30;
    const int z[6] = { min / 10, min % 10, sek / 10, sek % 10, hund / 10, hund % 10 };
    int n = 0;
    out[n++] = 0x05; out[n++] = RE15_R41C_COUNTDOWN_FARBE;              /* Farbe 2 wie ROOM2040 msg 4 @0x19A6 */
    for (int i = 0; i < 6; i++) {
        if (i == 2 || i == 4) out[n++] = 0x16;                           /* ':' */
        out[n++] = (uint8_t)(0x0C + z[i]);                               /* '0'..'9' */
    }
    out[n++] = 0x05; out[n++] = 0x00;
    out[n++] = 0x01; out[n++] = 0x00;
    return n;
}

int re15_ventilkette_r41_timer_laeuft(void) { return s_timer; }
int re15_ventilkette_r41_flut_laeuft(void)  { return s_flut; }
int re15_ventilkette_r41_ertrunken(void)    { return s_ertrunken; }
int re15_ventilkette_r41_spritzer(void)     { return s_spritzer_n; }

void re15_ventilkette_r41_tick(void)
{
    const uint16_t room = (uint16_t)g_current_room_id;
    if (room != s_letzter_raum) {                   /* Raumwechsel ohne Install-Weg (Sonden): nachziehen */
        re15_ventilkette_r41_install(room, NULL);
    }
    if (ist(room, RE15_R41C_RAUM_2030)) {
        /* Klick beim Oeffnen von Ventil 13 (RE2-Panelklick wie lift2030_r37 / hebetisch) */
        const int v13 = bit(RE15_R41C_BIT_VENTIL13);
        if (s_v13_vorher == 0 && v13) re15_audio_re2_panel_se(RE15_PANEL_SE_KLICK);
        s_v13_vorher = v13;
        /* Zaehler gestartet (Programm 2030_ventile Set(5,0x1D), genau einmal je Raumstart): Ost-Tuer -> 20B0,
         * Wasser parken. s_timer_gesehen bleibt stehen, auch wenn der Tod s_timer loescht. */
        if (!s_timer_gesehen && re15_game_flag_get(5, RE15_R41C_FLAG_TIMER)) {
            s_timer_gesehen = 1;
            s_timer = 1;
            s_zaehler_takte = 0;
            if (g_aot.slots[RE15_R41C_SLOT_2030_OSTTUER].type == RE15_AOT_TYPE_DOOR)
                tuer_nach_20b0(RE15_R41C_SLOT_2030_OSTTUER, 5950, -1800, -6400, 2048, 11, "Zaehler laeuft");
            wasser_anlegen(0, RE15_R41C_WASSER_PARK_Y, RE15_R41C_WASSER_PARK_Z);
            wasser_anlegen(1, RE15_R41C_WASSER_PARK_Y, RE15_R41C_WASSER_PARK_Z);
            LOG("[ventilkette41] Zaehler laeuft: 1800 Takte, Ost-Tuer -> 20B0, Wasser geparkt\n");
        }
        if (re15_game_flag_get(5, RE15_R41C_FLAG_TON)) {   /* Nachbesserung 5: Ventil-Ton (RE2 ROOM1080 se_on(2,5)) */
            re15_game_flag_set(5, RE15_R41C_FLAG_TON, 0);
            ton(RE15_R41C_TON_VENTIL);
        }
        ventile_tick();                                  /* Nachbesserung 4: Drehungen vermerken / pruefen */
        plakat_tick();
        { const int dok = bit(RE15_R41C_BIT_DOKUMENT);  /* Ablage durch den Dokument-Leser (menu_common.c, Bit (9,107)) */
          if (s_dok_vorher == 0 && dok) LOG("[ventilkette41] Dokument 6 \"Technician's Note\" abgelegt: Bit (9,107) = 1\n");
          s_dok_vorher = dok; }
        if (s_timer && !s_flut) s_zaehler_takte++;     /* Countdown (Nachbesserung 5) */
        if (!s_flut && re15_game_flag_get(5, RE15_R41C_FLAG_FLUT)) {
            s_flut = 1;
            LOG("[ventilkette41] FLUT: Wasser steigt (Countdown-Takte %d von %d)\n", s_zaehler_takte, RE15_R41C_ZAEHLER_BILDER);
        }
        if (s_flut) flut_tick();
        if (g_actors[RE15_ACTOR_SLOT_PLAYER].hp < 0) s_timer = 0;
    } else if (ist(room, RE15_R41C_RAUM_2050)) {
        panel_tick();
    } else if (ist(room, RE15_R41C_RAUM_2010)) {
        kasten_tick();                                   /* Nachbesserung 5 */
    }
}

/* ---- main.c: Wasserflaeche + Zeichner ---------------------------------------------------------------------- */
/* Relinghoehe (Nachtrag 8): nur obj 4 mit dem Netz der Schachtoeffnung; sonst obj 4/5 mit ROOM2080 Prop 0. */
static int reling_jetzt(void)
{
    return ist((uint16_t)g_current_room_id, RE15_R41C_RAUM_2030) && re15_ventilkette_r41_zustand_2030() == RE15_R41C_2030_RELING;
}

int re15_ventilkette_r41_obj_id(uint16_t room_id, int k)
{
    if (!ist(room_id, RE15_R41C_RAUM_2030)) return -1;
    if (k == 1 && reling_jetzt()) return -1;
    if (k == 2) return RE15_R41C_DOK_OBJ;        /* Nachbesserung 4: Dokument-Blatt */
    if (k == 3) return RE15_R41C_PLAKAT_OBJ;     /* Nachbesserung 4: Plakat-Aushang */
    return k == 0 ? RE15_R41C_WASSER_OBJ_A : (k == 1 ? RE15_R41C_WASSER_OBJ_B : -1);
}

const uint8_t *re15_ventilkette_r41_md1_bytes_k(int k, int *out_size)
{
#ifdef RE15_PLATFORM_PC
    if (k == 2) { if (out_size) *out_size = (int)sizeof re15_r41c_blatt_md1;   return re15_r41c_blatt_md1; }
    if (k == 3) { if (out_size) *out_size = (int)sizeof re15_r41c_aushang_md1; return re15_r41c_aushang_md1; }
#endif
    if (k == 0 || k == 1) return re15_ventilkette_r41_md1_bytes(out_size);
    if (out_size) *out_size = 0;
    return NULL;
}

const uint8_t *re15_ventilkette_r41_tim_bytes_k(int k, int *out_size)
{
#ifdef RE15_PLATFORM_PC
    if (k == 2) { if (out_size) *out_size = (int)sizeof re15_r41c_blatt_tim;   return re15_r41c_blatt_tim; }
    if (k == 3) { if (out_size) *out_size = (int)sizeof re15_r41c_aushang_tim; return re15_r41c_aushang_tim; }
#endif
    if (k == 0 || k == 1) return re15_ventilkette_r41_tim_bytes(out_size);
    if (out_size) *out_size = 0;
    return NULL;
}

const uint8_t *re15_ventilkette_r41_md1_bytes_fuer(int reling, int *out_size)
{
#ifdef RE15_PLATFORM_PC
    if (reling) { if (out_size) *out_size = (int)sizeof re15_r41c_wasser_reling_md1; return re15_r41c_wasser_reling_md1; }
    if (out_size) *out_size = (int)sizeof re15_r41c_wasser_md1;
    return re15_r41c_wasser_md1;
#else
    (void)reling;
    if (out_size) *out_size = 0;
    return NULL;
#endif
}

const uint8_t *re15_ventilkette_r41_md1_bytes(int *out_size)
{
    return re15_ventilkette_r41_md1_bytes_fuer(reling_jetzt(), out_size);
}

const uint8_t *re15_ventilkette_r41_tim_bytes(int *out_size)
{
#ifdef RE15_PLATFORM_PC
    if (out_size) *out_size = (int)sizeof re15_r41c_wasser_tim;
    return re15_r41c_wasser_tim;
#else
    if (out_size) *out_size = 0;
    return NULL;
#endif
}

/* Zeichner-Abfrage (main.c zeichnet mit re15_render_tile; die Engine-Bibliothek kennt den Zeichner nicht):
 * nr 0 = Zeiger (roter Laeufer auf der Schiene; RE2 ROOM2130: Zeiger unter der waagerechten Skala, Rot wie der
 * 11F0-Zeiger esp16.tim CLUT 0 @0x22), nr 1..4 = gruene Marke unter dem EIN-Hebel nr (PORT-WAHL, Farbe der
 * 11F0-Lampe CLUT-Zeile 2). Rueckgabe 1 = zeichnen, Rechteck in *x0/*y0/*w/*h (320x240). */
int re15_ventilkette_r41_sicht(int nr, int *x0, int *y0, int *w, int *h)
{
    if (!s_panel_aktiv || !ist((uint16_t)g_current_room_id, RE15_R41C_RAUM_2050)) return 0;
    if (g_scd.cam_id != RE15_R41C_PANEL_CUT) return 0;
    static const int k_x[RE15_R41C_PANEL_N] = { 88, 133, 178, 238 };
    if (nr == 0) {
        const int zx = re15_ventilkette_r41_zeiger_x(re15_ventilkette_r41_spannung());
        if (x0) *x0 = zx - 1; if (y0) *y0 = RE15_R41C_SKALA_Y; if (w) *w = 3; if (h) *h = RE15_R41C_SKALA_H;
        return 1;
    }
    if (nr < 1 || nr > RE15_R41C_PANEL_N || !(s_schalter & (1u << (nr - 1)))) return 0;
    if (x0) *x0 = k_x[nr - 1] - 4; if (y0) *y0 = RE15_R41C_LAMPE_Y; if (w) *w = 8; if (h) *h = 3;
    return 1;
}
