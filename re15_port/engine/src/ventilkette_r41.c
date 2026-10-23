/*
 * RE1.5 Rebuilt — Runde 41 Spur C "ventilkette": Leiter 2000 / Bruecke + Ventile 2030 / Flut -> 20B0 /
 * Schaltpult 2050 / Rechner 2080.
 *
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
static int      s_spritzer_takt = 0;
/* 2050 Pult */
static int      s_panel_aktiv = 0;
static unsigned s_schalter = 0;
static int      s_ruhe = 0;
static int      s_panel_phase = 0;  /* 0 frei, 1 geloest: Satz laeuft, 2 Ausstieg angefordert */
static int      s_cursor_vorher = -1;

static const int k_gewichte[RE15_R41C_PANEL_N] = RE15_R41C_PANEL_GEWICHTE;
static const int32_t k_zone_x0[RE15_R41C_PANEL_N] = RE15_R41C_PANEL_X0;

static int ist(uint16_t room, uint16_t leon) { return (room & 0xFFFEu) == leon; }
static int bit(int b) { return re15_game_flag_get(RE15_R41C_BANK, (uint8_t)b) ? 1 : 0; }
static void bit_setzen(int b) { re15_game_flag_set(RE15_R41C_BANK, (uint8_t)b, 1); }

/* ---- Texte ------------------------------------------------------------------------------------------------- */
static void text_einsetzen(uint8_t id, const uint8_t *m, int n)
{
    re15_msg_install_text(id, m, (size_t)n);
    int d = re15_msg_compute_duration(m, (size_t)n, 0);   /* Dauer-Eintrag wie re15_msg_load_room_block */
    if (d > 0 && d < 65535) re15_msg_install_durations(id, d);
}

const uint8_t *re15_ventilkette_r41_text(uint16_t room_id, uint8_t id, int *out_len)
{
    uint16_t gruppe = (uint16_t)(room_id & 0xFFFEu);
    if (gruppe == RE15_R41C_RAUM_20B0) gruppe = RE15_R41C_RAUM_2000;   /* Leiter-Satz gilt in beiden */
    for (int i = 0; i < RE15_R41C_TEXTE_N; i++)
        if (re15_r41c_texte[i].raum == gruppe && re15_r41c_texte[i].id == id) {
            if (out_len) *out_len = re15_r41c_texte[i].n;
            return re15_r41c_texte[i].b;
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
    uint16_t gruppe = (uint16_t)(room_id & 0xFFFEu);
    if (gruppe == RE15_R41C_RAUM_20B0) gruppe = RE15_R41C_RAUM_2000;
    for (int i = 0; i < RE15_R41C_TEXTE_N; i++)
        if (re15_r41c_texte[i].raum == gruppe)
            text_einsetzen(re15_r41c_texte[i].id, re15_r41c_texte[i].b, re15_r41c_texte[i].n);
    if (gruppe == RE15_R41C_RAUM_2050) {
        text_einsetzen(RE15_R41C_MSG_PULT_FRAGE, re15_r41c_org_11f0_msg1, (int)sizeof re15_r41c_org_11f0_msg1);
        text_einsetzen(RE15_R41C_MSG_PULT_OK,    re15_r41c_org_11f0_msg2, (int)sizeof re15_r41c_org_11f0_msg2);
    }
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
static int wasser_anlegen(int k, int32_t y, int32_t z)
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
    g_scd.props[p].x = RE15_R41C_WASSER_X; g_scd.props[p].y = y; g_scd.props[p].z = z;
    g_scd.props[p].rot_x = g_scd.props[p].rot_y = g_scd.props[p].rot_z = 0;
    s_wasser_prop[k] = p;
    return p;
}

int re15_ventilkette_r41_wasser_prop(int k) { return (k == 0 || k == 1) ? s_wasser_prop[k] : -1; }

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
    /* Die drei Stellen. */
    ereignis_platz(RE15_R41C_SLOT_PULT,     RE15_R41C_PULT_RECT,     RE15_R41C_PULT_BAND,     RE15_R41C_EV_PULT);
    ereignis_platz(RE15_R41C_SLOT_VENTIL13, RE15_R41C_VENTIL13_RECT, RE15_R41C_VENTIL13_BAND, RE15_R41C_EV_VENTIL13);
    ereignis_platz(RE15_R41C_SLOT_VENTILE,  RE15_R41C_VENTILE_RECT,  RE15_R41C_VENTILE_BAND,  RE15_R41C_EV_VENTILE);
    /* Nach der Flut: Wasser 1800 ueber dem Ring, Lift-Rufzonen oben = Original-Satz 1, bis der Rechner lief. */
    if (bit(RE15_R41C_BIT_FLUT_GELAUFEN) && !bit(RE15_R41C_BIT_2030_LEER)) {
        wasser_anlegen(0, RE15_R41C_WASSER_Y_RUHE, RE15_R41C_WASSER_ZA);
        wasser_anlegen(1, RE15_R41C_WASSER_Y_RUHE, RE15_R41C_WASSER_ZB);
        static const int k_ruf[2] = { 4, 6 };   /* sub00 @0x01D20 (Band 6) / @0x01DC2 (Band 9) */
        for (int i = 0; i < 2; i++)
            if (g_aot.slots[k_ruf[i]].active && g_aot.slots[k_ruf[i]].type != RE15_AOT_TYPE_NONE)
                sperrtext(k_ruf[i], RE15_R41C_MSG_LIFT_ORIGINAL);
        LOG("[ventilkette41] ROOM%04X: unter Wasser (y %d), Lift-Rufzonen 4/6 -> msg 1\n", (unsigned)room_id, RE15_R41C_WASSER_Y_RUHE);
    }
    tueren_umlenken(room_id);
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

static void install_2080(uint16_t room_id)
{
    texte_einsetzen(room_id);
    if (bit(RE15_R41C_BIT_FLUT_GELAUFEN) && !bit(RE15_R41C_BIT_2030_LEER) && re15_game_flag_get(3, 245)) {
        if (satz_anlegen(re15_r41c_org_2080_slot4))
            LOG("[ventilkette41] ROOM%04X: Rechner Slot 4 (@0x00C1A) erneut angelegt (2030 entleeren)\n", (unsigned)room_id);
    }
}

void re15_ventilkette_r41_install(uint16_t room_id, const re15_rdt_t *rdt)
{
    /* Raumwechsel waehrend des Zaehlers = Flut gelaufen (Nutzer: "Verlaesst man den Raum fuehrt das zu 20B0"). */
    if (s_timer && !ist(room_id, RE15_R41C_RAUM_2030)) {
        bit_setzen(RE15_R41C_BIT_FLUT_GELAUFEN);
        bit_setzen(RE15_R41C_BIT_LEITER_FREI);
        LOG("[ventilkette41] Zaehler lief beim Verlassen von 2030 -> Bits 103 + 100 (Flut gelaufen, Leiter frei)\n");
    }
    s_timer = 0; s_timer_gesehen = 0; s_flut = 0; s_ertrunken = 0; s_spritzer_takt = 0;
    s_wasser_prop[0] = s_wasser_prop[1] = -1;
    s_panel_aktiv = 0; s_schalter = 0; s_ruhe = 0; s_panel_phase = 0; s_cursor_vorher = -1;
    if (s_fade_an) { re15_fade_kill(RE15_R41C_FADE_KANAL); s_fade_an = 0; }
    re15_game_flag_set(5, RE15_R41C_FLAG_TIMER, 0);   /* Raum-Scratch, wie das Raumladen (FUN_8003ecec @0x8003ed74) */
    re15_game_flag_set(5, RE15_R41C_FLAG_FLUT, 0);
    s_v13_vorher = bit(RE15_R41C_BIT_VENTIL13);
    s_letzter_raum = room_id;

    if (ist(room_id, RE15_R41C_RAUM_2000) || ist(room_id, RE15_R41C_RAUM_20B0)) install_2000(room_id);
    else if (ist(room_id, RE15_R41C_RAUM_2030)) install_2030(room_id, rdt);
    else if (ist(room_id, RE15_R41C_RAUM_2050)) install_2050(room_id);
    else if (ist(room_id, RE15_R41C_RAUM_2080)) install_2080(room_id);
    else if (ist(room_id, RE15_R41C_RAUM_2010) || ist(room_id, RE15_R41C_RAUM_2070) || ist(room_id, RE15_R41C_RAUM_1260))
        tueren_umlenken(room_id);
}

void re15_ventilkette_r41_reset(void)
{
    if (s_fade_an) { re15_fade_kill(RE15_R41C_FADE_KANAL); s_fade_an = 0; }
    s_timer = 0; s_timer_gesehen = 0; s_flut = 0; s_ertrunken = 0; s_spritzer_takt = 0; s_v13_vorher = -1; s_letzter_raum = 0xFFFFu;
    s_wasser_prop[0] = s_wasser_prop[1] = -1;
    s_panel_aktiv = 0; s_schalter = 0; s_ruhe = 0; s_panel_phase = 0; s_cursor_vorher = -1;
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
        else if (event_id == RE15_R41C_EV_VENTILE)
            name = (bit(RE15_R41C_BIT_FLUT_GELAUFEN) || s_timer) ? "2030_ventile_zustand" : "2030_ventile";
    } else if (ist(room_id, RE15_R41C_RAUM_2050)) {
        if (event_id == RE15_R41C_EV_2050_HEBEL && !bit(RE15_R41C_BIT_PULT2050)) name = "2050_hebel_sperre";
        else if (event_id == RE15_R41C_EV_2050_PULT)
            name = bit(RE15_R41C_BIT_PULT2050) ? "2050_pult_zustand" : "2050_pult_ein";
    } else if (ist(room_id, RE15_R41C_RAUM_2080)) {
        if (event_id == RE15_R41C_EV_2080_RECHNER && bit(RE15_R41C_BIT_FLUT_GELAUFEN) && !bit(RE15_R41C_BIT_2030_LEER))
            name = re15_game_flag_get(3, 245) ? "2080_rechner_nur2030" : "2080_rechner_erw";
    }
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
    /* Spritzer am Spieler, sobald die Flaeche seine Fuesse erreicht (Wasser-Schritt-Effekt FUN_80045630) */
    if (wy <= pl->y && wy > pl->y - RE15_R41C_ERTRINK_TIEFE) {
        if (++s_spritzer_takt >= RE15_R41C_SPRITZER_TAKT) {
            s_spritzer_takt = 0;
            const uint32_t code = RE15_WASSER_R38_CODE_BODEN;
            re15_esp_fx_spawn_rows(re15_esp_room_bank(), (uint8_t)(code >> 24), (uint8_t)((code >> 16) & 0xffu),
                                   (uint16_t)(code & 0xffffu), pl->x, wy, pl->z, INT32_MAX, pl->rot_y);
        }
    }
    if (wy <= pl->y - RE15_R41C_ERTRINK_TIEFE) {
        s_ertrunken = 1;
        s_timer = 0;                                        /* Tod: keine Flut-Bits beim naechsten Raumstart */
        if (pl->hp >= 0) pl->hp = -1;                       /* tot = hp < 0 (main.c RE15_KILL_AT, game_step_common.c) */
        re15_fade_config(RE15_R41C_FADE_KANAL, RE15_R41C_FADE_ABR, RE15_R41C_FADE_MASKE, 0, RE15_R41C_FADE_BUCKET);
        re15_fade_kick(RE15_R41C_FADE_KANAL, RE15_R41C_FADE_LEVEL);
        s_fade_an = 1;
        LOG("[ventilkette41] ERTRUNKEN: Wasser y %d, Spieler y %d -> hp -1, blaue Blende\n", (int)wy, (int)pl->y);
    }
}

int re15_ventilkette_r41_timer_laeuft(void) { return s_timer; }
int re15_ventilkette_r41_flut_laeuft(void)  { return s_flut; }
int re15_ventilkette_r41_ertrunken(void)    { return s_ertrunken; }

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
            if (g_aot.slots[RE15_R41C_SLOT_2030_OSTTUER].type == RE15_AOT_TYPE_DOOR)
                tuer_nach_20b0(RE15_R41C_SLOT_2030_OSTTUER, 5950, -1800, -6400, 2048, 11, "Zaehler laeuft");
            wasser_anlegen(0, RE15_R41C_WASSER_PARK_Y, RE15_R41C_WASSER_PARK_Z);
            wasser_anlegen(1, RE15_R41C_WASSER_PARK_Y, RE15_R41C_WASSER_PARK_Z);
            LOG("[ventilkette41] Zaehler laeuft: 1800 Takte, Ost-Tuer -> 20B0, Wasser geparkt\n");
        }
        if (!s_flut && re15_game_flag_get(5, RE15_R41C_FLAG_FLUT)) { s_flut = 1; LOG("[ventilkette41] FLUT: Wasser steigt\n"); }
        if (s_flut) flut_tick();
        if (g_actors[RE15_ACTOR_SLOT_PLAYER].hp < 0) s_timer = 0;
    } else if (ist(room, RE15_R41C_RAUM_2050)) {
        panel_tick();
    }
}

/* ---- main.c: Wasserflaeche + Zeichner ---------------------------------------------------------------------- */
int re15_ventilkette_r41_obj_id(uint16_t room_id, int k)
{
    if (!ist(room_id, RE15_R41C_RAUM_2030)) return -1;
    return k == 0 ? RE15_R41C_WASSER_OBJ_A : (k == 1 ? RE15_R41C_WASSER_OBJ_B : -1);
}

const uint8_t *re15_ventilkette_r41_md1_bytes(int *out_size)
{
#ifdef RE15_PLATFORM_PC
    if (out_size) *out_size = (int)sizeof re15_r41c_wasser_md1;
    return re15_r41c_wasser_md1;
#else
    if (out_size) *out_size = 0;
    return NULL;
#endif
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
