/*
 * hund1121_r39.c — Runde 39 Spur R "hund1121": Elzas Panikphase, ROOM1121 — ein Hund springt durch das Fenster, durch das bei
 * Leon in ROOM1120 die Kraehe hereinfliegt. Herleitung, Belege und Port-Wahlen: include/re15_hund1121_r39.h und
 * analysis/befunde_runde39/R_hund1121.md.
 */
#include "re15_hund1121_r39.h"
#include "re15_fenster1120.h"   /* Leons Band (RE15_FENSTER_ZONE_*), Splitter-Saetze, Abbildung, Knall-Haken */
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_scd.h"
#include "re15_rumble.h"
#include "re15_room.h"         /* g_room_rdt (Wandzellen) */
#include "re15_rdt.h"
#include "re15_ai_flavor.h"
#include "re15_engine.h"        /* g_engine.frame_count (Messschiene) */
#include "re2_fx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern unsigned g_current_room_id;
extern uint8_t re15_em_status_zone(void);   /* enemy_ai_common.c: Stage 1-3 -> 7 (@0x800420ec) */

/* ---- Zustand ------------------------------------------------------------------------------------ */
static int s_phase = RE15_R39R_AUS;
static int s_slot = -1;
static int s_takt = -1;
static int s_glas = -1;
static int s_splitter = 0;
static int s_knalle = 0;
static int s_re2 = 1;
static uint8_t s_prog[56];
static int s_wand_offen = 0;                       /* 1 = Sca_id_set hat die Band-1-Rueckwand freigegeben        */
static uint8_t s_wand_u0[RE15_R39R_WAND_REGIONEN]; /* Originalwerte (0xFF, ROOM1121.RDT @0x003D1 ...)            */
static int s_wand_ok = 0;                          /* 1 = Region r Index 15 ist die Band-1-Rueckwand (geprueft)  */

int re15_hund1121_r39_phase(void)     { return s_phase; }
int re15_hund1121_r39_slot(void)      { return s_slot; }
int re15_hund1121_r39_takt(void)      { return s_takt; }
int re15_hund1121_r39_glas_takt(void) { return s_glas; }
int re15_hund1121_r39_splitter(void)  { return s_splitter; }
int re15_hund1121_r39_knalle(void)    { return s_knalle; }
int re15_hund1121_r39_re2(void)       { return s_re2; }

/* Messschiene fuer den Echtlauf (die GUI-exe hat kein stderr): RE15_HUND1121_LOG=<pfad>. */
static void logf_(const char *fmt, int a, int b, int c, int d, int e)
{
    static int s_init = 0; static const char *s_pfad = NULL;
    if (!s_init) { s_init = 1; s_pfad = getenv("RE15_HUND1121_LOG"); if (s_pfad && !*s_pfad) s_pfad = NULL; }
    if (!s_pfad) return;
    FILE *f = fopen(s_pfad, "a");
    if (!f) return;
    fprintf(f, "F%u ", (unsigned)g_engine.frame_count);
    fprintf(f, fmt, a, b, c, d, e);
    fputc('\n', f);
    fclose(f);
}

static void s16_setzen(uint8_t *p, int32_t v) { p[0] = (uint8_t)(v & 0xff); p[1] = (uint8_t)((v >> 8) & 0xff); }

/* Die Band-1-Rueckwand je Region (Adressierung wie sca_entry_at in scd_vm.c: Region-Basis + Index). */
static re15_sca_entry_t *wand_zelle(int region)
{
    if (!g_room_rdt.sca || region < 0 || region >= RE15_R39R_WAND_REGIONEN) return NULL;
    int base = 0;
    for (int i = 0; i < region; i++) base += g_room_rdt.sca_rgn[i];
    const int fi = base + RE15_R39R_WAND_INDEX;
    if (fi < 0 || fi >= g_room_rdt.sca_count) return NULL;
    return (re15_sca_entry_t *)&g_room_rdt.sca[fi];
}
static int wand_pruefen(void)
{
    for (int r = 0; r < RE15_R39R_WAND_REGIONEN; r++) {
        const re15_sca_entry_t *c = wand_zelle(r);
        if (!c || c->x != -2700 || c->z != RE15_R39R_GLAS_Z || c->width != 11350 || c->density != 2000 ||
            (c->floor >> 4) != 1) return 0;
        s_wand_u0[r] = c->u0;
    }
    return 1;
}
static void wand_schliessen(const re15_actor_t *h)
{
    if (!s_wand_offen) return;
    for (int r = 0; r < RE15_R39R_WAND_REGIONEN; r++) {
        re15_sca_entry_t *c = wand_zelle(r);
        if (c) c->u0 = s_wand_u0[r];
    }
    s_wand_offen = 0;
    logf_("[hund1121] Wand Band 1 wieder zu (u0 0x%02X), Hund z %d Radius %d %d %d", (int)s_wand_u0[0],
          h ? (int)h->z : 0, h ? (int)h->hit_radius_min : 0, 0, 0);
}

/* ---- Port-Programm (RE1.5-Opcodes, laeuft in der vorhandenen VM) ----------------------------------
 *   5 x Sca_id_set(Region r, Index 15, 0): die Band-1-Rueckwand frei (Herleitung RE15_R39R_WAND_INDEX im Header).
 *   Sce_em_set (20 B, Handler FUN_800420a0; Satzform wie ROOM1120 main00 @0x00D0C `44 00 21 00 00 00 00 a7 ...`):
 *     Gegner-Slot 4, Typ 0x20, Verhalten 0 (RE2-Hirn; die Sub-14-Freigabe macht der Takt wie ROOM21A0 sub07) bzw. 0x40
 *     (RE1.5-Hirn, INIT @0x8010db88-98), pc[4] = 0 (Boden 0), Kill-Flag pc[7] = 0xAE, Lage/Richtung s. Header.
 *   Set(Zone 7, 0xAE, 1) (`22 07 ae 01`, RE1.5-Set wie ROOM1120 sub01 @0x00D6C `22 03 38 01`) — NACH dem Spawn, damit das
 *     Spawn-Tor @0x80042128-38 den Hund noch durchlaesst.
 *   Aot_reset(4) auf sce 0 (`46 04 00 00 00 00 00 00 00 00`, LAB_80040738; Form RE2 room1090 sub15 @0x0004 `46 06 00`).
 *   Evt_end. */
static void programm_bauen(void)
{
    memset(s_prog, 0, sizeof s_prog);
    uint8_t *p = s_prog;
    /* 5 x Sca_id_set(Region r, Index 15, 0) — Satzform ROOM1130 sub02 @0x00AC8 `37 00 0a 00` .. @0x00AD8 `37 04 0a 00`. */
    for (int r = 0; r < RE15_R39R_WAND_REGIONEN; r++, p += 4) {
        p[0] = 0x37; p[1] = (uint8_t)r; p[2] = RE15_R39R_WAND_INDEX; p[3] = 0x00;
    }
    p[0] = 0x44;
    p[1] = RE15_R39R_EM;
    p[2] = 0x20;
    p[3] = re15_ai_re2_for_type(0x20u) ? 0x00 : RE15_R39R_RE15_GRID;
    p[7] = RE15_R39R_KILL;
    s16_setzen(&p[8],  RE15_R39R_X);
    s16_setzen(&p[10], RE15_R39R_Y);
    s16_setzen(&p[12], RE15_R39R_Z);
    s16_setzen(&p[16], RE15_R39R_DIR);
    p += 20;
    p[0] = 0x22; p[1] = re15_em_status_zone(); p[2] = RE15_R39R_KILL; p[3] = 0x01; p += 4;
    p[0] = 0x46; p[1] = RE15_R39R_SLOT; p += 10;        /* sce 0, Flags 0, Nutzlast 0 */
    p[0] = 0x01; p[1] = 0x00;
}

const uint8_t *re15_hund1121_r39_programm(int *out_len)
{
    programm_bauen();
    if (out_len) *out_len = (int)sizeof s_prog;
    return s_prog;
}

static int schon_gesprungen(void) { return re15_game_flag_get(re15_em_status_zone(), RE15_R39R_KILL); }

void re15_hund1121_r39_install(uint16_t room_id)
{
    s_phase = RE15_R39R_AUS; s_slot = -1; s_takt = -1; s_glas = -1; s_splitter = 0; s_knalle = 0; s_re2 = 1;
    s_wand_offen = 0; s_wand_ok = 0;
    if (room_id != RE15_R39R_RAUM) return;
    if (!re15_game_flag_get(RE15_R39R_PANIK_BANK, RE15_R39R_PANIK_BIT) || schon_gesprungen()) {
        logf_("[hund1121] ROOM1121 aus: (9,97)=%d (%d,0x%02X)=%d %d",
              re15_game_flag_get(RE15_R39R_PANIK_BANK, RE15_R39R_PANIK_BIT), (int)re15_em_status_zone(),
              RE15_R39R_KILL, schon_gesprungen(), 0);
        return;
    }
    /* Slot 4 = AUTO-Ereignis 30 auf Leons Band (dieselben Aufrufe wie re15_fenster1120_install). */
    re15_aot_set(RE15_R39R_SLOT, RE15_AOT_TYPE_GENERIC, RE15_R39R_EREIGNIS,
                 (RE15_FENSTER_ZONE_X0 + RE15_FENSTER_ZONE_X1) / 2,
                 (RE15_FENSTER_ZONE_Z0 + RE15_FENSTER_ZONE_Z1) / 2,
                 (RE15_FENSTER_ZONE_X1 - RE15_FENSTER_ZONE_X0) / 2,
                 (RE15_FENSTER_ZONE_Z1 - RE15_FENSTER_ZONE_Z0) / 2);
    g_aot.slots[RE15_R39R_SLOT].sce_flags = 0;
    g_aot.slots[RE15_R39R_SLOT].band = 0;               /* Gang = Band 0 (Boden y 0) */
    re15_aot_retype(RE15_R39R_SLOT, RE15_FENSTER_SCE, RE15_FENSTER_SAT, RE15_FENSTER_P0,
                    (uint16_t)((RE15_R39R_EREIGNIS << 8) | (RE15_FENSTER_P1 & 0xff)), 0);
    s_wand_ok = wand_pruefen();
    s_phase = RE15_R39R_SCHARF;
    logf_("[hund1121] ROOM1121 scharf: Slot %d Band x %d..%d z %d..%d", RE15_R39R_SLOT,
          RE15_FENSTER_ZONE_X0, RE15_FENSTER_ZONE_X1, RE15_FENSTER_ZONE_Z0, RE15_FENSTER_ZONE_Z1);
    logf_("[hund1121] Wand Band 1 Region 0..4 Index %d: gefunden %d u0 0x%02X %d %d", RE15_R39R_WAND_INDEX,
          s_wand_ok, (int)s_wand_u0[0], 0, 0);
}

const uint8_t *re15_hund1121_r39_ereignis(uint16_t room_id, uint8_t event_id)
{
    if (room_id != RE15_R39R_RAUM || event_id != RE15_R39R_EREIGNIS) return NULL;
    if (s_phase != RE15_R39R_SCHARF || schon_gesprungen() || !s_wand_ok) return NULL;
    programm_bauen();
    s_wand_offen = 1;                                   /* die ersten 5 Saetze geben die Wand frei */
    logf_("[hund1121] Ausloeser: Spieler (%d,%d) im Band, Ereignis %d, Verhalten 0x%02X %d",
          (int)g_actors[RE15_ACTOR_SLOT_PLAYER].x, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].z,
          RE15_R39R_EREIGNIS, (int)s_prog[3], 0);
    return s_prog;
}

static void glas_brechen(const re15_actor_t *h)
{
    re15_rumble_small(3, 3);                           /* sub15 @0x003A `8a 00 03 00 03 00` */
    re15_rumble_small(3, 7);                           /* sub15 @0x0040 `8a 00 03 00 07 00` */
    re15_rumble_large(8, 0xfa, 0);                     /* sub15 @0x0046 `8b fa 08 00 00 00` */
    for (int i = 0; i < RE15_FENSTER_SPLITTER_N; i++) {   /* sub15 @0x0054..0x01D4, Fenster 1 */
        const re15_fenster_splitter_t *s = re15_fenster1120_splitter(i);
        int32_t p[3];
        re15_fenster1120_abbilden(s->x, s->y, s->z, p);
        int16_t ofs[4] = { (int16_t)p[0], (int16_t)p[1], (int16_t)p[2], 0 };
        uint32_t a0 = ((uint32_t)s->bank << 24) | ((uint32_t)s->sub << 16) | s->skala;
        int r = re2fx_spawn_sofort(a0, (int16_t)((RE15_FENSTER_RE2_GIER - 0x800) & 0xfff), re2fx_einheitsmatrix, ofs);
        if (r >= 0 && r < RE2FX_PLAETZE) s_splitter++;
    }
    logf_("[hund1121] Glas: Hund bei (%d,%d,%d) erreicht die Glasebene z %d, %d Splitter",
          (int)h->x, (int)h->y, (int)h->z, RE15_R39R_GLAS_Z, s_splitter);
}

static void knall(void)
{
    s_knalle++;
    if (re15_fenster1120_se_hook) re15_fenster1120_se_hook(RE15_FENSTER_SE_SATZ);   /* sub15 @0x0208/@0x0218 */
    logf_("[hund1121] Knall %d: Raumbank-Satz 0x%02X (RE2 Se_on 0x02210001), %d Bilder nach dem Glas %d %d",
          s_knalle, RE15_FENSTER_SE_SATZ, s_takt - s_glas, 0, 0);
}

void re15_hund1121_r39_tick(void)
{
    if (s_phase == RE15_R39R_AUS || s_phase == RE15_R39R_FERTIG) return;
    if (g_current_room_id != RE15_R39R_RAUM) { s_phase = RE15_R39R_AUS; return; }
    const int slot = RE15_R39R_EM + 1;                 /* SCRIPT_SLOT_TO_ACTOR (scd_vm.c) */
    re15_actor_t *h = &g_actors[slot];
    if (s_takt >= 0) s_takt++;

    if (s_phase == RE15_R39R_SCHARF) {
        /* Das Programm laeuft in der VM VOR game_step: in diesem Bild steht der Hund schon (Zustand 0), sein INIT laeuft
         * gleich in re15_enemy_ai_run_all. */
        if (h->active && h->type == 0x20 && h->state == 0 && h->em_flag_id == RE15_R39R_KILL) {
            s_slot = slot; s_takt = 0; s_re2 = re15_ai_re2_for_type(0x20u);
            s_phase = RE15_R39R_GESPAWNT;
            logf_("[hund1121] Hund Slot %d gesetzt bei (%d,%d,%d) RE2-Hirn %d", slot, (int)h->x, (int)h->y, (int)h->z,
                  s_re2);
        }
        return;
    }
    if (!h->active || h->type != 0x20) {               /* erlegt/entfernt, bevor die Kette durch ist */
        logf_("[hund1121] Hund weg in Phase %d, Takt %d %d %d %d", s_phase, s_takt, 0, 0, 0);
        wand_schliessen(NULL);
        s_phase = RE15_R39R_FERTIG;
        return;
    }
    if (s_phase == RE15_R39R_GESPAWNT) {
        if (s_re2 && h->state == 1) {
            /* ROOM21A0 sub07 @0x020FE..0x0210A: Member_set +4 = 1, +5 = 14, +6 = 0, +7 = 0 (Pos/Dir stehen aus dem Spawn). */
            h->state = RE15_R39R_RE2_STATE; h->sub_state_1 = RE15_R39R_RE2_SUB; h->sub_state_2 = 0; h->sub_state_3 = 0;
            s_phase = RE15_R39R_WARTET;
            logf_("[hund1121] RE2: Sub %d Phase %d (ROOM21A0 sub07 @0x020FE), Takt %d %d %d",
                  RE15_R39R_RE2_SUB, 0, s_takt, 0, 0);
        } else if (!s_re2 && h->state == 4 && h->sub_state_1 == 0) {
            s_phase = RE15_R39R_WARTET;                /* INIT @0x8010db88-98 hat grid 0x40 auf Zustand 4 gelegt */
            logf_("[hund1121] RE1.5: Zustand %d Sub %d wartet auf grid 0x%02X, Takt %d %d",
                  (int)h->state, (int)h->sub_state_1, RE15_R39R_RE15_LOS, s_takt, 0);
        }
        return;
    }
    if (s_phase == RE15_R39R_WARTET) {
        /* Evt_next @0x0211E: die Freigabe kommt ein Bild nach dem Umstellen. */
        if (s_re2) h->sub_state_3 = RE15_R39R_RE2_LOS;     /* @0x02122 `34 05 01 00` */
        else       h->grid_id = RE15_R39R_RE15_LOS;        /* ROOM1190 sub10 @0x027E6 `34 0c 43 00` */
        s_phase = RE15_R39R_SPRUNG;
        logf_("[hund1121] Freigabe (RE2=%d), Takt %d, Hund (%d,%d,%d)", s_re2, s_takt, (int)h->x, (int)h->y, (int)h->z);
        return;
    }
    /* SPRUNG */
    logf_("[hund1121] Sprung T%d (%d,%d,%d) st=%d", s_takt, (int)h->x, (int)h->y, (int)h->z,
          ((int)h->state << 16) | ((int)h->sub_state_1 << 8) | ((int)h->sub_state_2 << 4) | (int)h->motion);
    if (s_glas < 0 && h->z <= RE15_R39R_GLAS_Z) { s_glas = s_takt; glas_brechen(h); }
    if (s_glas >= 0) {
        const int d = s_takt - s_glas;
        if (d == RE15_R39R_T_KNALL1 || d == RE15_R39R_T_KNALL2) knall();
    }
    if (s_wand_offen && h->z + (int32_t)h->hit_radius_min < RE15_R39R_GLAS_Z) wand_schliessen(h);
    /* durch = beide Knalle und der Hund hat den Sprung verlassen (RE2: Sub 14 -> 2 @0x80102E2C; RE1.5: Zustand 4 ->
     * Wort 0x201 @0x8011162c). */
    const int gelandet = s_re2 ? (h->sub_state_1 != RE15_R39R_RE2_SUB) : (h->state != 4);
    if (s_knalle >= 2 && gelandet) {
        wand_schliessen(h);
        s_phase = RE15_R39R_FERTIG;
        logf_("[hund1121] fertig T%d: Hund (%d,%d) Zustand %d Sub %d", s_takt, (int)h->x, (int)h->z,
              (int)h->state, (int)h->sub_state_1);
    }
}

int re15_hund1121_r39_schatten_aus(const re15_actor_t *e)
{
    /* PORT-WAHL: jenseits der Glasebene stuende der Bodenschatten (Bezug +0x1C2 = Raumboden 0) hinter der Rueckwand am
     * Wandfuss unter dem Fenster — derselbe Grund, aus dem Runde 35 der versteckten Fensterkraehe den Schatten nimmt. */
    if (!e || g_current_room_id != RE15_R39R_RAUM || s_slot <= 0 || e != &g_actors[s_slot]) return 0;
    return e->type == 0x20 && e->z > RE15_R39R_GLAS_Z;
}
