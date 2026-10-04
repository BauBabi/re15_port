/*
 * fenster_1120.c — Runde 35 Spur M: ROOM1120 Cut 1, das hintere Fenster zerbricht (RE2-Glassplitter
 * + RE2-Knall), eine Kraehe fliegt herein, das Fenster bleibt beschaedigt.
 * Herleitung, Belege und Port-Wahlen: include/re15_fenster1120.h und
 * analysis/befunde_runde35/M_cut11c0_fenster.md.
 */
#include "re15_fenster1120.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_scd.h"
#include "re15_rumble.h"
#include "re2_fx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern unsigned g_current_room_id;

void (*re15_fenster1120_se_hook)(int satz) = NULL;

/* ---- Die 13 Splitter (RE2 room1090 sub15, Fenster 1) — Rohfelder woertlich aus
 *      info/re2leon/PL0/RDT/room1090/scd/sub15.scd:
 *        @0x0054 3a 00 10 02 00 00 00 28 68 f7 68 f7 e0 ca 00 0c   ...  ---------------------------- */
static const re15_fenster_splitter_t k_splitter[RE15_FENSTER_SPLITTER_N] = {
    { 0x0054, 0x10, 2, 0x2800, -2200, -2200, -13600 },
    { 0x0064, 0x10, 3, 0x2600, -2200, -2100, -14000 },
    { 0x0074, 0x10, 4, 0x2700, -2200, -2250, -14100 },
    { 0x0084, 0x11, 2, 0x2000, -2200, -2150, -13500 },
    { 0x0094, 0x11, 3, 0x1800, -2200, -2100, -13900 },
    { 0x00A4, 0x11, 4, 0x2100, -2200, -2200, -14200 },
    { 0x00B4, 0x13, 0, 0x2000, -2700, -3600, -14400 },
    { 0x00E4, 0x12, 1, 0x2000, -2800, -3200, -14500 },
    { 0x0114, 0x11, 2, 0x1800, -2800, -3000, -14400 },
    { 0x0144, 0x10, 3, 0x1800, -2550, -2700, -14600 },
    { 0x0174, 0x10, 4, 0x1000, -2600, -2100, -14400 },
    { 0x01A4, 0x11, 4, 0x1000, -2700, -1800, -14400 },
    { 0x01D4, 0x11, 3, 0x1000, -2800, -1900, -14600 },
};

const re15_fenster_splitter_t *re15_fenster1120_splitter(int i)
{
    return (i >= 0 && i < RE15_FENSTER_SPLITTER_N) ? &k_splitter[i] : NULL;
}

void re15_fenster1120_abbilden(int16_t x2, int16_t y2, int16_t z2, int32_t out[3])
{
    out[0] = RE15_FENSTER_X - ((int32_t)x2 - RE15_FENSTER_RE2_X);
    out[1] = (int32_t)y2;
    out[2] = RE15_FENSTER_WAND_Z - ((int32_t)z2 - RE15_FENSTER_RE2_Z);
}

/* ---- Port-Programme (RE1.5-Opcodes, laufen in der vorhandenen VM) -------------------------------
 * SPAWN: EIN RE1.5-Sce_em_set (20 B, Handler 0x800420a0, Satzform wie ROOM1120 main00 @0x00D0C
 *   `44 00 21 00 00 00 00 a7 82 14 00 00 52 1c 00 00 00 0c 00 00`): Gegner-Slot 3, Typ 0x21,
 *   Verhalten 0, Kill-Flag 0xFF (Gate aus, @0x80042128), Lage/Richtung = Abbildung der RE2-Kraehe 0
 *   (5400,-2500,12100) dir 0x0418 (s. Header). Dann Evt_end.
 * AUSLOESER: Form von RE2 sub15 @0x0000/@0x0004 `22 04 31 01` / `46 06 00 ...`:
 *   Set(9,79,1) (`22 09 4f 01`, RE1.5-Set wie ROOM1120 sub01 @0x00D6C `22 03 38 01`) und
 *   Aot_reset(4) auf sce 0 (`46 04 00 00 00 00 00 00 00 00`, LAB_80040738), Evt_end. */
static uint8_t s_prog_spawn[22];
static const uint8_t k_prog_ausloeser[16] = {
    0x22, RE15_FENSTER_BANK, RE15_FENSTER_BIT, 0x01,
    0x46, RE15_FENSTER_SLOT, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00 };

static void s16_setzen(uint8_t *p, int32_t v) { p[0] = (uint8_t)(v & 0xff); p[1] = (uint8_t)((v >> 8) & 0xff); }

static void spawn_programm_bauen(void)
{
    int32_t lage[3];
    re15_fenster1120_abbilden(RE15_FENSTER_KRAEHE_RE2_X, RE15_FENSTER_KRAEHE_RE2_Y,
                              RE15_FENSTER_KRAEHE_RE2_Z, lage);
    memset(s_prog_spawn, 0, sizeof s_prog_spawn);
    s_prog_spawn[0] = 0x44;                            /* Sce_em_set                              */
    s_prog_spawn[1] = RE15_FENSTER_KRAEHE_EM;          /* Gegner-Slot 3                            */
    s_prog_spawn[2] = 0x21;                            /* Kraehe                                   */
    s_prog_spawn[7] = 0xFF;                            /* Kill-Flag aus                            */
    s16_setzen(&s_prog_spawn[8],  lage[0]);
    s16_setzen(&s_prog_spawn[10], lage[1]);
    s16_setzen(&s_prog_spawn[12], lage[2]);
    s16_setzen(&s_prog_spawn[16], (RE15_FENSTER_KRAEHE_RE2_DIR - 0x800) & 0xfff);
    s_prog_spawn[20] = 0x01;                           /* Evt_end                                  */
    s_prog_spawn[21] = 0x00;
}

/* ---- Zustand ------------------------------------------------------------------------------------ */
static int s_phase = RE15_FENSTER_AUS;
static int s_spawn_offen = 0;       /* Spawn-Programm angefordert, Kraehe noch nicht ausgestattet */
static int s_kraehe = -1;
static int s_takt = -1;
static int s_splitter = 0;
static int s_knalle = 0;

int re15_fenster1120_phase(void)              { return s_phase; }
int re15_fenster1120_takt(void)               { return s_takt; }
int re15_fenster1120_kraehe_slot(void)        { return s_kraehe; }
int re15_fenster1120_splitter_gespawnt(void)  { return s_splitter; }
int re15_fenster1120_knalle(void)             { return s_knalle; }

const uint8_t *re15_fenster1120_programm(int welches, int *len)
{
    if (welches == 0) { spawn_programm_bauen(); if (len) *len = (int)sizeof s_prog_spawn; return s_prog_spawn; }
    if (len) *len = (int)sizeof k_prog_ausloeser;
    return k_prog_ausloeser;
}

/* Messschiene fuer den Echtlauf (die GUI-exe hat kein stderr): RE15_FENSTER_LOG=<pfad>. */
static void logf_(const char *fmt, int a, int b, int c, int d)
{
    static int s_init = 0; static const char *s_pfad = NULL;
    if (!s_init) { s_init = 1; s_pfad = getenv("RE15_FENSTER_LOG"); if (s_pfad && !*s_pfad) s_pfad = NULL; }
    if (!s_pfad) return;
    FILE *f = fopen(s_pfad, "a");
    if (!f) return;
    fprintf(f, fmt, a, b, c, d);
    fputc('\n', f);
    fclose(f);
}

static int tor_offen(void)
{
    return re15_game_flag_get(RE15_FENSTER_BANK, RE15_FENSTER_TOR_BIT) &&
          !re15_game_flag_get(RE15_FENSTER_BANK, RE15_FENSTER_BIT);
}

void re15_fenster1120_install(uint16_t room_id)
{
    s_phase = RE15_FENSTER_AUS; s_spawn_offen = 0; s_kraehe = -1;
    s_takt = -1; s_splitter = 0; s_knalle = 0;
    if (room_id != RE15_FENSTER_RAUM) return;
    if (!tor_offen()) {
        logf_("[fenster] ROOM1120 aus: (9,73)=%d (9,79)=%d %d %d",
              re15_game_flag_get(RE15_FENSTER_BANK, RE15_FENSTER_TOR_BIT),
              re15_game_flag_get(RE15_FENSTER_BANK, RE15_FENSTER_BIT), 0, 0);
        return;
    }
    /* Slot 4 = AUTO-Ereignis 24 auf dem Band (re15_aot_set + Aot_reset-Semantik re15_aot_retype). */
    re15_aot_set(RE15_FENSTER_SLOT, RE15_AOT_TYPE_GENERIC, RE15_FENSTER_EREIGNIS,
                 (RE15_FENSTER_ZONE_X0 + RE15_FENSTER_ZONE_X1) / 2,
                 (RE15_FENSTER_ZONE_Z0 + RE15_FENSTER_ZONE_Z1) / 2,
                 (RE15_FENSTER_ZONE_X1 - RE15_FENSTER_ZONE_X0) / 2,
                 (RE15_FENSTER_ZONE_Z1 - RE15_FENSTER_ZONE_Z0) / 2);
    g_aot.slots[RE15_FENSTER_SLOT].sce_flags = 0;
    g_aot.slots[RE15_FENSTER_SLOT].band = 0;          /* Gang = Band 0 (Boden y 0, SCA Band 0) */
    re15_aot_retype(RE15_FENSTER_SLOT, RE15_FENSTER_SCE, RE15_FENSTER_SAT,
                    RE15_FENSTER_P0, RE15_FENSTER_P1, 0);
    /* Die Fensterkraehe versteckt hinter dem Fenster (RE2 sub03 @0x0028 Gosub sub09 beim Raumaufbau). */
    s_spawn_offen = 1;
    s_phase = RE15_FENSTER_SCHARF;
    (void)scd_event_fire(RE15_FENSTER_EREIGNIS);
    logf_("[fenster] ROOM1120 scharf: Slot %d Band x %d..%d z 4300..6400 Ereignis %d",
          RE15_FENSTER_SLOT, RE15_FENSTER_ZONE_X0, RE15_FENSTER_ZONE_X1, RE15_FENSTER_EREIGNIS);
}

const uint8_t *re15_fenster1120_ereignis(uint16_t room_id, uint8_t event_id)
{
    if (room_id != RE15_FENSTER_RAUM || event_id != RE15_FENSTER_EREIGNIS) return NULL;
    if (s_phase != RE15_FENSTER_SCHARF) return NULL;
    if (s_spawn_offen == 1) {
        s_spawn_offen = 2;                             /* Programm unterwegs, Ausstattung im Tick */
        spawn_programm_bauen();
        return s_prog_spawn;
    }
    if (re15_game_flag_get(RE15_FENSTER_BANK, RE15_FENSTER_BIT)) return NULL;
    logf_("[fenster] Ausloeser: Spieler (%d,%d) im Band, Ereignis %d %d",
          (int)g_actors[RE15_ACTOR_SLOT_PLAYER].x, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].z,
          RE15_FENSTER_EREIGNIS, 0);
    return k_prog_ausloeser;
}

static void splitter_spawnen(void)
{
    /* sub15 @0x0054..0x01D4: je Satz FUN_8001bf10(rec[2]<<24 | rec[3]<<16 | Skala, Gier,
     * Einheitsmatrix, {x,y,z}) — Sce_espr_on 0x800565a4 (@0x800565cc-0x80056614). */
    for (int i = 0; i < RE15_FENSTER_SPLITTER_N; i++) {
        const re15_fenster_splitter_t *s = &k_splitter[i];
        int32_t p[3];
        re15_fenster1120_abbilden(s->x, s->y, s->z, p);
        int16_t ofs[4] = { (int16_t)p[0], (int16_t)p[1], (int16_t)p[2], 0 };
        uint32_t a0 = ((uint32_t)s->bank << 24) | ((uint32_t)s->sub << 16) | s->skala;
        int r = re2fx_spawn_sofort(a0, (int16_t)((RE15_FENSTER_RE2_GIER - 0x800) & 0xfff),
                                   re2fx_einheitsmatrix, ofs);
        if (r >= 0 && r < RE2FX_PLAETZE) s_splitter++;
        logf_("[fenster] Splitter Bank 0x%02X Platz %d Lage x %d z %d", s->bank, r, (int)p[0], (int)p[2]);
    }
}

static void knall(void)
{
    s_knalle++;
    if (re15_fenster1120_se_hook) re15_fenster1120_se_hook(RE15_FENSTER_SE_SATZ);
    logf_("[fenster] Knall %d: Raumbank-Satz 0x%02X (RE2 Se_on 0x02210001) T+%d %d",
          s_knalle, RE15_FENSTER_SE_SATZ, s_takt, 0);
}

void re15_fenster1120_tick(void)
{
    if (s_phase == RE15_FENSTER_AUS || s_phase == RE15_FENSTER_FERTIG) return;
    if (g_current_room_id != RE15_FENSTER_RAUM) { s_phase = RE15_FENSTER_AUS; return; }

    /* Ausstattung der frisch gespawnten Kraehe — VOR der KI (dieser Haken steht vor
     * re15_enemy_ai_run_all), damit der erste INIT-Tick schon +0x10E = 0x4002 sieht. */
    if (s_spawn_offen == 2) {
        int slot = RE15_FENSTER_KRAEHE_EM + 1;         /* SCRIPT_SLOT_TO_ACTOR (scd_vm.c) */
        re15_actor_t *k = &g_actors[slot];
        if (k->active && k->type == 0x21 && k->state == 0) {
            k->re2z_f10e = (uint16_t)RE15_FENSTER_KRAEHE_F10E;
            re15_re2crow_zwang(slot, 1);
            re15_re2crow_befehl(slot, 0);
            s_kraehe = slot;
            s_spawn_offen = 0;
            logf_("[fenster] Kraehe Slot %d versteckt bei (%d,%d,%d)", slot, (int)k->x, (int)k->y, (int)k->z);
        }
    }

    if (s_phase == RE15_FENSTER_SCHARF) {
        if (!re15_game_flag_get(RE15_FENSTER_BANK, RE15_FENSTER_BIT)) return;
        s_phase = RE15_FENSTER_LAEUFT;                 /* Kante (9,79) 0->1 = sub15 @0x0000 */
        s_takt = 0;
    } else {
        s_takt++;
    }

    if (s_takt == 0 && s_kraehe > 0)
        re15_re2crow_befehl(s_kraehe, 4);              /* sub15 @0x0012..0x002A Member_set(0x17,4) */
    if (s_takt == RE15_FENSTER_T_SPLITTER) {
        re15_rumble_small(3, 3);                       /* @0x003A `8a 00 03 00 03 00` -> 0x8003947c(3,3) */
        re15_rumble_small(3, 7);                       /* @0x0040 `8a 00 03 00 07 00` -> 0x8003947c(3,7) */
        re15_rumble_large(8, 0xfa, 0);                 /* @0x0046 `8b fa 08 00 00 00` -> 0x80039514(8,250,0) */
        splitter_spawnen();
    }
    if (s_takt == RE15_FENSTER_T_KNALL1) knall();      /* @0x0208 */
    if (s_takt == RE15_FENSTER_T_KNALL2) knall();      /* @0x0218 */
    if (s_takt >= RE15_FENSTER_T_ENDE) {
        s_phase = RE15_FENSTER_FERTIG;                 /* @0x024A Evt_end */
        logf_("[fenster] Zeitlinie fertig T+%d Splitter %d Knalle %d %d", s_takt, s_splitter, s_knalle, 0);
    }
}

int re15_fenster1120_schaden_sichtbar(int cut)
{
    if (g_current_room_id != RE15_FENSTER_RAUM || cut != 1) return 0;
    if (s_phase == RE15_FENSTER_LAEUFT) return s_takt >= RE15_FENSTER_T_SPLITTER;
    return re15_game_flag_get(RE15_FENSTER_BANK, RE15_FENSTER_BIT);
}
