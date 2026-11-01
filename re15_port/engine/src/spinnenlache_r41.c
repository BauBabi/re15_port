/*
 * RE1.5 Rebuilt — Runde 41 Spur S, Nachbesserung 2: Lache der Spinnenleiche und Fleck der toten RE2-Babyspinne.
 * Alle Adressen und Bytes im Kopf include/re15_spinnenlache_r41.h; RE1.5 STAGE2.BIN in Mod und Disc gleich, RE2
 * EMS25.BIN / EMS26.BIN / PSX.EXE aus info/re2leon.
 *
 * PORT-WAHL (Gore-Maximum, Nutzerregel 2026-11-01): keine. Die Lache steht in BEIDEN Originalen; sie haengt deshalb nicht
 * am Schalter RE15_KEIN_80PROZENT_GORE (wie die Lachen von Hund, Kraehe und Gorilla).
 * PORT-WAHL (Basis): der Record waechst von dem Mass, das der Port zeichnet. Das ist fuer 0x25 in beiden Gehirnen das
 * RE1.5-Mass 1300 x 1400 (Spur L, INIT FUN_8001af5c @0x80110d78/84). Der RE2-Record der Adult waere 800 x 800
 * (`lui a2,0x320` / `ori a2,a2,0x320` @0x80100368-6c, EMS25).
 */
#include "re15_spinnenlache_r41.h"
#include "re15_spinnenanim_r41.h"   /* re15_spinnenanim_r41_schatten_groesse: 1300 x 1400 @0x80110d78/84 */
#include "re15_aot.h"               /* re15_aot_water_at == FUN_800527B4 */
#include "re15_audio.h"             /* re15_audio_room_se == FUN_800453d0 */
#include "re15_ai_flavor.h"         /* re15_re2spider_baby_owns */
#include "re15_engine.h"              /* g_engine.frame_count (nur der Messhaken) */
#include <stdlib.h>
#include <string.h>

/* Tinte je Platz: wer die Farbwoerter zuletzt umgefaerbt hat (RE1.5- oder RE2-Gehirn). Gelesen nur bei crow_pool != 0,
 * und jede Stelle, die crow_pool setzt, setzt hier zuerst die Tinte. */
static uint32_t s_tinte[RE15_ACTOR_MAX];

typedef struct { int32_t pos[3]; int16_t yaw; int32_t hx, hz; uint32_t tinte; } fleck_t;
static fleck_t s_flecken[RE15_SPINNENLACHE_FLECKEN_MAX];
static int     s_flecken_n = 0;

static int platz_von(const re15_actor_t *e)
{
    long s = (long)(e - g_actors);
    return (s >= 0 && s < RE15_ACTOR_MAX) ? (int)s : -1;
}

void re15_spinnenlache_tinte_rgb(uint32_t tinte, uint8_t *r, uint8_t *g, uint8_t *b)
{
    if (r) *r = (uint8_t)(tinte & 0xffu);
    if (g) *g = (uint8_t)((tinte >> 8) & 0xffu);
    if (b) *b = (uint8_t)((tinte >> 16) & 0xffu);
}

/* Record einfaerben: (alt & 0xff000000) | Tinte. Der Port haelt das Farbwort nicht, nur "umgefaerbt" + die Tinte.
 * Liegt noch kein Record-Mass im Port-Kanal (crow_shadow_w == 0), uebernimmt er das gezeichnete Mass. */
static void einfaerben(re15_actor_t *e, uint32_t tinte)
{
    int s = platz_von(e);
    if (s >= 0) s_tinte[s] = tinte;
    if (e->crow_shadow_w == 0) {
        int32_t hx = 500, hz = 600;                         /* main.c-Standard; fuer 0x25 ueberschreibt Spur L */
        if (e->type == 0x26u) hx = hz = RE15_SPINNENLACHE_BABY_HALB;   /* EMS26 @0x8010024c-50 */
        re15_spinnenanim_r41_schatten_groesse(e, &hx, &hz);
        e->crow_shadow_w = (uint16_t)hx;
        e->crow_shadow_h = (uint16_t)hz;
    }
    e->crow_pool = 1;
}

static void wachsen(re15_actor_t *e)
{
    e->crow_shadow_w = (uint16_t)(e->crow_shadow_w + RE15_SPINNENLACHE_SCHRITT);
    e->crow_shadow_h = (uint16_t)(e->crow_shadow_h + RE15_SPINNENLACHE_SCHRITT);
}

/* ---- RE1.5 ------------------------------------------------------------------------------------------------------- */
uint8_t re15_spinnenlache_r15_eintritt(int platzweg)
{
    return platzweg ? 0u : 3u;                              /* @0x80114578-80 bzw. @0x80114d4c-60 */
}

void re15_spinnenlache_r15_leiche(re15_actor_t *e)
{
    if (!e) return;
    switch (e->sub_state_3) {                               /* `lbu v1,7(v0)` @0x801153e4, Tabelle @0x801002a4 */
    case 0:                                                 /* 0x80115414 */
        e->grab_kill_ctr = RE15_SPINNENLACHE_R15_TAKTE_0;   /* +0x9E = 0x5a @0x8011541c-20 */
        e->sub_state_3 = 1;                                 /* +0x7 = 1 @0x8011542c-30 */
        /* word0 |= 2 (@0x80115448) und |= 0x40 (@0x80115464): Flag-Wort ohne Port-Kanal (wie beim Hund) */
        /* faellt in Phase 1 */
    case 1:                                                 /* 0x8011546c */
        einfaerben(e, RE15_SPINNENLACHE_TINTE_R15);         /* @0x8011546c-a0 */
        wachsen(e);                                         /* +0xBC/+0xBE += 8 @0x801154a8-b8 */
        break;
    case 3:                                                 /* 0x801154bc */
        e->grab_kill_ctr = RE15_SPINNENLACHE_R15_TAKTE_3;   /* +0x9E = 0x78 @0x801154c4-c8 */
        e->sub_state_3 = 4;                                 /* +0x7 = 4 @0x801154d4-d8 */
        /* Clip 5 (`sb v0,148` @0x80115520) und anim_set @0x80115594 laufen im Aufrufer (Port: Clip 5 in jeder Leiche) */
        /* faellt in Phase 4 */
    case 4:                                                 /* 0x80115534 */
        einfaerben(e, RE15_SPINNENLACHE_TINTE_R15);         /* @0x80115534-68 */
        wachsen(e);                                         /* @0x80115570-7c */
        break;
    default:                                                /* Phase 2 0x801155d4 = Ruecksprung */
        return;
    }
    /* Zaehler @0x8011559c-d0: `lbu a0,158(v1)` / `addiu v0,a0,-1` / `bne a0,zero` / `sb v0,158(v1)` (Delay-Slot) */
    uint8_t alt = (uint8_t)e->grab_kill_ctr;
    e->grab_kill_ctr = (int16_t)(uint8_t)(alt - 1u);
    if (alt == 0u) {
        e->sub_state_3 = 2;                                 /* +0x7 = 2 @0x801155c8-d0 */
        re15_audio_room_se(9);                              /* `ori a0,zero,0x9` @0x801155bc, `jal 0x800453d0` @0x801155cc */
    }
}

/* ---- RE2 Adult ----------------------------------------------------------------------------------------------------- */
void re15_spinnenlache_re2_boden(re15_actor_t *e)
{
    if (!e) return;
    switch (e->re2s_gs225) {                                /* `lbu v1,549(s0)` @0x80104dfc, Tabelle @0x801000b4 */
    case 2:                                                 /* 0x80104e68 */
        if (re15_aot_water_at(e->x, e->z) != 0) return;     /* `jal 0x800527b4` @0x80104e70, `bne v0,zero` @0x80104e78 */
        e->re2s_gs225 = 3;                                  /* @0x80104e84-8c */
        einfaerben(e, RE15_SPINNENLACHE_TINTE_RE2);         /* @0x80104e7c-b0 */
        e->re2z_gaitrow = RE15_SPINNENLACHE_RE2_TAKTE;      /* +0x16B = 90 @0x80104eac-b4 */
        /* word0 &= ~0x400 (@0x80104eb8-c4): Schatten wieder zeigen — der Port zeigt ihn ohnehin */
        /* faellt in [3] */
    case 3:                                                 /* 0x80104ec8 */
        wachsen(e);                                         /* rec+4/+6 += 8 @0x80104ed0-dc */
        e->re2z_gaitrow = (uint8_t)(e->re2z_gaitrow - 1u);  /* @0x80104ee0-ec */
        if (e->re2z_gaitrow == 0u) e->re2s_gs225 = 4;       /* `bne` @0x80104ef4, `sb v0(4),549` @0x80104efc */
        return;
    default:                                                /* [0]/[1] Schatten-Verbergen (kein Port-Kanal), [4] nichts */
        return;
    }
}

/* ---- RE2 Babyspinne ------------------------------------------------------------------------------------------------ */
void re15_spinnenlache_baby_platsch(re15_actor_t *e)
{
    if (!e) return;
    einfaerben(e, RE15_SPINNENLACHE_TINTE_RE2);             /* @0x80100cb4-e0 (kein Wachsen) */
}

void re15_spinnenlache_baby_ende(const re15_actor_t *e)
{
    if (!e || !e->crow_pool) return;                        /* nur der gruen gefaerbte Record ist Gore (s. Dossier) */
    if (s_flecken_n >= RE15_SPINNENLACHE_FLECKEN_MAX) return;   /* Pool voll: FUN_80016480 findet keinen Platz mehr */
    int s = platz_von(e);
    fleck_t *f = &s_flecken[s_flecken_n++];
    f->pos[0] = e->x;
    f->pos[1] = (int32_t)e->dog_floor_y;                   /* rec+0xA = a1 = +0x1C2 (`lh a1,450(s0)` @0x800268c8, `sh s2,10(s0)` @0x80016948) */
    f->pos[2] = e->z;
    f->yaw    = (int16_t)e->rot_y;                         /* rec+0x10 = a2 = +0x76 (`lh a2,118(s0)` @0x800268d0, `sh s1,16(s0)` @0x8001693c) */
    f->hx     = e->crow_shadow_w;
    f->hz     = e->crow_shadow_h;
    f->tinte  = (s >= 0) ? s_tinte[s] : RE15_SPINNENLACHE_TINTE_RE2;
}

/* ---- Zeichner ------------------------------------------------------------------------------------------------------ */
int re15_spinnenlache_render(const re15_actor_t *npc, int32_t *nhx, int32_t *nhz, int *pool,
                             uint8_t *sh_r, uint8_t *sh_g, uint8_t *sh_b)
{
    if (!npc) return 0;
    int baby = re15_re2spider_baby_owns(npc);
    if (npc->type != 0x25u && !baby) return 0;
    if (npc->crow_shadow_w != 0) {
        if (nhx) *nhx = npc->crow_shadow_w;
        if (nhz) *nhz = npc->crow_shadow_h;
    } else if (baby) {
        if (nhx) *nhx = RE15_SPINNENLACHE_BABY_HALB;        /* EMS26 @0x8010024c-50 */
        if (nhz) *nhz = RE15_SPINNENLACHE_BABY_HALB;
    } else {
        return 0;
    }
    if (npc->crow_pool) {
        int s = platz_von(npc);
        if (pool) *pool = 1;
        re15_spinnenlache_tinte_rgb(s >= 0 ? s_tinte[s] : RE15_SPINNENLACHE_TINTE_RE2, sh_r, sh_g, sh_b);
    }
    return 1;
}

int re15_spinnenlache_flecken_zahl(void) { return s_flecken_n; }

int re15_spinnenlache_fleck(int i, int32_t pos[3], int16_t *yaw, int32_t *hx, int32_t *hz, uint8_t rgb[3])
{
    if (i < 0 || i >= s_flecken_n) return 0;
    const fleck_t *f = &s_flecken[i];
    if (pos) { pos[0] = f->pos[0]; pos[1] = f->pos[1]; pos[2] = f->pos[2]; }
    if (yaw) *yaw = f->yaw;
    if (hx) *hx = f->hx;
    if (hz) *hz = f->hz;
    if (rgb) re15_spinnenlache_tinte_rgb(f->tinte, &rgb[0], &rgb[1], &rgb[2]);
    return 1;
}

void re15_spinnenlache_raum_reset(void)
{
    s_flecken_n = 0;
    memset(s_flecken, 0, sizeof s_flecken);
}

/* MESSHAKEN RE15_BABYTOD_TEST=<Spielbild> (nur Messlaeufe, nie im Spiel gesetzt): ab diesem Bild loest jede RE2-Babyspinne,
 * die am Boden steht (ACTIVE Sub 1, +0x222 == 0), die WIRKUNG des Zertret-Tors aus: +0x4 = 3 mit +0x5 = 0 (@0x80100448).
 * Grund: im Port stirbt kein Baby (Zielhilfe waehlt sie nicht, das Zertret-Tor @0x801003D4-460 greift im Echtlauf nicht;
 * Dossier S, Nachbesserung 2). Ohne den Haken gaebe es keinen Fensterbeleg fuer den Fleck. */
int re15_spinnenlache_test_babytod(const re15_actor_t *e)
{
    static int s_init = 0; static long s_ab = -1;
    if (!s_init) { s_init = 1; const char *v = getenv("RE15_BABYTOD_TEST"); if (v && *v) s_ab = atol(v); }
    if (s_ab < 0 || !e || (long)g_engine.frame_count < s_ab) return 0;
    return e->state == 1u && e->sub_state_1 == 1u && e->re2s_mode222 == 0u;
}
