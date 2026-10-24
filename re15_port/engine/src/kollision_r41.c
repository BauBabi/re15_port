/*
 * RE1.5 Rebuilt — Runde 41 Spur H "kollision". Herleitung, Messungen und Bytes: include/re15_kollision_r41.h und
 * analysis/befunde_runde41/H_kollision.md. Jede Konstante traegt die Adresse ihrer Instruktion (info/Re1.5/PSX.EXE
 * bzw. info/Re1.5/PSX/BIN/STAGEn.BIN).
 *
 * RE15_KOLLISION_LOG=<datei> (Diagnose, kein Verhalten): je wirksamem NPC-Schub eine Zeile NPC-SCHUB, je vom
 * Objekt-Pass gesperrtem Kisten-Schub eine Zeile OBJEKT-GESPERRT (Gegenstueck zum Haltepunkt @0x8002c080 mit
 * v0 != 0 am Original).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "re15_kollision_r41.h"
#include "re15_actor.h"
#include "re15_enemy_ai.h"   /* re15_body_push = FUN_8002aec4, RE15_AI_STATE_CORPSE */
#include "re15_engine.h"     /* g_engine.frame_count (nur Log) */
#include "re15_room.h"       /* g_current_room_id (nur Log) */
#include "re15_scd.h"        /* g_scd.props = Objekt-Pool 0x800b3f98 */

static FILE *kollision_log(void)
{
    static int init = 0;
    static FILE *f = NULL;
    if (!init) {
        init = 1;
        const char *p = getenv("RE15_KOLLISION_LOG");
        if (p && *p) f = fopen(p, "w");
    }
    return f;
}

/* ================================ (1) NPC-WURZEL-SCHWANZ ================================================
 * Selbst gescannt: alle 24 NPC-Wurzeln der sechs Stage-Overlays (Registrierung `sw v0,0x2cXX(at)` auf
 * 0x80072bac + typ*4) tragen nach dem Zustands-`jalr` b498 -> aec4(Spieler, NPC) -> b544 -> b0a4, z.B.
 *   STAGE1 0x40 0x8011c5a0: aec4 @0x8011c66c (a0 = s0+20 @0x8011c670), b544 @0x8011c674, b0a4 @0x8011c694
 *   STAGE1 0x42 0x8011cb70: aec4 @0x8011cc3c (a0 = s0+20 @0x8011cc40), b544 @0x8011cc44, b0a4 @0x8011cc64
 *   STAGE3 0x40 0x8011b724: aec4 @0x8011b7f0, b544 @0x8011b7f8   STAGE3 0x42 0x8011bcf4: aec4 @0x8011bdc0, b544 @0x8011bdc8
 * (vollstaendige Liste im Dossier). Die Wurzel springt bei globalem Freeze (0x800aca40 & 0x20000000) oder
 * +0x9 & 0x20 ueber den GANZEN Schwanz (`bne ... 0x8011cc7c` @0x8011cb90 / @0x8011cbb0) — der Port kehrt in
 * re15_npc_ai_tick an derselben Stelle vorher zurueck, der Haken liegt dahinter.
 *
 * FUN_8002aec4(a0, a1) BEWEGT a1 (re15_body_push(pusher, r, pushee, r)); Radien = Kasten +6 beider Seiten
 * (`lhu t4,6(s6)` @0x8002af40 / `lhu t4,6(s5)` @0x8002af4c) = hit_radius_min (NPC 450 Kasten @0x80121658,
 * 0x45 500 @0x80121728, 0x4b 300 @0x801218c8; Spieler 450 PSX.EXE Datei 0x64694). Gates im Callee:
 * Bit 2 an einem der beiden Woerter / Bit 4 am Geschobenen / Bit 0x1000 an BEIDEN (@0x8002af04-af30) —
 * im Port wie bei re15_enemy_body_push_tail ueber tote Spieler (hp < 0) und Leichen (Zustand 7) abgebildet.
 *
 * FUN_8002b544 (@0x8002b544-b5c8): s1 = g_active_count (`lbu s1,-13746(s1)` @0x8002b550), s0 = 0x800acc2c
 * (@0x8002b564), je aktiver Entitaet (`andi v0,v0,0x1` @0x8002b578) ausser der eigenen (`beq a1,s0`
 * @0x8002b590): `jal 0x8002aec4` @0x8002b598 mit a0 = andere (Delay-Slot @0x8002b59c), a1 = g_entity(cur);
 * Stride 500 (@0x8002b5ac). Keine Typ-, Zustands- oder Bandabfrage ausser den aec4-Gates. */
void re15_kollision_r41_npc_schwanz(int slot)
{
    if (slot <= RE15_ACTOR_SLOT_PLAYER || slot >= RE15_ACTOR_MAX) return;
    re15_actor_t *e = &g_actors[slot];
    if (!e->active || e->hit_radius_min == 0) return;
    FILE *lf = kollision_log();

    /* aec4(&Spieler, NPC) @0x8011cc3c (a0 = 0x800aca40 + 20 = 0x800aca54 im Delay-Slot @0x8011cc40). */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (pl->active && pl->hp >= 0 && pl->hit_radius_min != 0) {
        const int32_t vx = e->x, vz = e->z;
        if (re15_body_push(pl, (int32_t)pl->hit_radius_min, e, (int32_t)e->hit_radius_min) && lf)
            fprintf(lf, "F%u room=%04X NPC-SCHUB slot=%d typ=%02x von=SPIELER vor=(%d,%d) nach=(%d,%d)\n",
                    g_engine.frame_count, g_current_room_id, slot, (unsigned)e->type, (int)vx, (int)vz,
                    (int)e->x, (int)e->z);
    }

    /* b544 @0x8011cc44: NPC aus jeder anderen aktiven Entitaet, aufsteigend wie 0x800acc2c + 500*k. */
    for (int o = RE15_ACTOR_SLOT_PLAYER + 1; o < RE15_ACTOR_MAX; o++) {
        if (o == slot) continue;                                       /* @0x8002b590 */
        re15_actor_t *z2 = &g_actors[o];
        if (!z2->active || z2->hit_radius_min == 0) continue;          /* @0x8002b578 (+ Kasten) */
        if (z2->state == (uint8_t)RE15_AI_STATE_CORPSE) continue;      /* Port-Abbild Bit 2 (wie re15_enemy_body_push_tail) */
        const int32_t vx = e->x, vz = e->z;
        if (re15_body_push(z2, (int32_t)z2->hit_radius_min, e, (int32_t)e->hit_radius_min) && lf)
            fprintf(lf, "F%u room=%04X NPC-SCHUB slot=%d typ=%02x von=%d typ=%02x vor=(%d,%d) nach=(%d,%d) "
                        "andere=(%d,%d)\n",
                    g_engine.frame_count, g_current_room_id, slot, (unsigned)e->type, o, (unsigned)z2->type,
                    (int)vx, (int)vz, (int)e->x, (int)e->z, (int)z2->x, (int)z2->z);
    }
    if (lf) fflush(lf);
}

/* ================================ (2) OBJEKT-GEGEN-OBJEKT-PASS ===========================================
 * Spiegel obj+0x42 (y): das Original schreibt ihn am Ende JEDER Objekt-Runde des Passes FUN_8002bd44
 * (`lhu v1,56(s2)` @0x8002c0ec -> `sh v1,66(s2)` @0x8002c0f8, zusammen mit +0x40/+0x44, die der Port in
 * zellschub_r36.c fuehrt). Der Port fuehrt den y-Anteil hier, geschrieben NACH dem Objekt-Pass jedes Bildes
 * (Haken game_step_common.c) — zum Zeitpunkt des naechsten Schubs ist er damit "y am Ende des letzten Passes",
 * wie im Original fuer das geschobene Objekt selbst und fuer alle Objekte, deren Runde im laufenden Pass noch
 * nicht dran war. Abweichung nur fuer ein Objekt, dessen y sich im laufenden Pass VOR dem Schubtest aendert
 * (fallende Kiste im selben Bild) — Dossier OFFEN. */
static int16_t s_spiegel_y[RE15_SCD_MAX_PROPS];

void re15_kollision_r41_objspiegel(void)
{
    for (int p = 0; p < RE15_SCD_MAX_PROPS && p < (int)g_scd.prop_count; p++)
        if (g_scd.props[p].active)                                     /* @0x8002bdcc: nur aktive Runden */
            s_spiegel_y[p] = (int16_t)g_scd.props[p].y;                /* sh v1,66(s2) @0x8002c0f8 */
}

/* FUN_8002cdf4(a0 = t2 = anderes Objekt a, a1 = a3 = geschobenes Objekt b) — selbst disassembliert
 * @0x8002cdf4-cfd0. Kasten = *(obj+0x78): +0 cx, +2 cy, +4 cz (lh), +6 hx, +8 hy, +10 hz (lhu). */
int re15_kollision_r41_8002cdf4(int a, int b, int32_t bx, int32_t bz, int32_t alt_ya, int32_t alt_yb,
                                int32_t *neu_yb)
{
    if (a < 0 || a >= RE15_SCD_MAX_PROPS || b < 0 || b >= RE15_SCD_MAX_PROPS) return 0;
    const int32_t a_cx = g_scd.props[a].box_cx, a_cy = g_scd.props[a].box_cy, a_cz = g_scd.props[a].box_cz;
    const int32_t b_cx = g_scd.props[b].box_cx, b_cy = g_scd.props[b].box_cy, b_cz = g_scd.props[b].box_cz;
    const uint32_t a_hx = (uint16_t)g_scd.props[a].box_hx, a_hy = (uint16_t)g_scd.props[a].box_hy,
                   a_hz = (uint16_t)g_scd.props[a].box_hz;
    const uint32_t b_hx = (uint16_t)g_scd.props[b].box_hx, b_hy = (uint16_t)g_scd.props[b].box_hy,
                   b_hz = (uint16_t)g_scd.props[b].box_hz;

    /* `lw v0,0(t2)` / `lw v1,0(a3)` / `or` / `andi v0,v0,0x40` / `bne -> return 0` @0x8002ce0c-ce24 */
    if (((uint32_t)g_scd.props[a].flags | (uint32_t)g_scd.props[b].flags) & 0x40u) return 0;

    const int32_t s1 = a_cx + g_scd.props[a].x;                        /* addu s1,s2,v1 @0x8002ce4c */
    const int32_t t7 = (b_cx + bx) - s1;                               /* dX: subu t7,v0,s1 @0x8002ce50 */
    const int32_t t9 = a_cz + g_scd.props[a].z;                        /* addu t9,s0,v1 @0x8002ce64 */
    const int32_t t5 = (b_cz + bz) - t9;                               /* dZ: subu t5,v0,t9 @0x8002ce68 */
    const uint32_t sx = a_hx + b_hx;                                   /* a1 = hx+hx @0x8002ce78 */
    const uint32_t sz = a_hz + b_hz;                                   /* t3 = hz+hz @0x8002ce7c */
    if ((sx << 1) < (uint32_t)(t7 + (int32_t)sx)) return 0;            /* sll/sltu/bne @0x8002ce80-8c */
    if ((sz << 1) < (uint32_t)(t5 + (int32_t)sz)) return 0;            /* @0x8002ce94-a0 */

    const int32_t t8 = a_cy + g_scd.props[a].y;                        /* addu t8,t4,v1 @0x8002cec0 */
    const int32_t t6 = (b_cy + g_scd.props[b].y) - t8;                 /* dY: subu t6,v0,t8 @0x8002cec8 */
    const uint32_t sy = a_hy + b_hy;                                   /* a0 = hy+hy @0x8002cecc */
    const uint32_t t0 = sy << 1;                                       /* sll t0,a0,1 @0x8002ced4 */
    const int jetzt = !(t0 < (uint32_t)(t6 + (int32_t)sy));            /* sltu / xori 1 @0x8002ced8-dc */
    if (!jetzt) return 0;                                              /* beq a2,zero @0x8002cee0 */

    /* Vor-y aus dem Spiegel: `lh v1,66(a3)` / `lh v0,66(t2)` @0x8002cee8-ec (s16). */
    const int32_t alt_dy = ((int32_t)(int16_t)alt_yb + b_cy) - ((int32_t)(int16_t)alt_ya + a_cy);   /* @0x8002cef0-f8 */
    const int vorher = !(t0 < (uint32_t)(alt_dy + (int32_t)sy));       /* @0x8002cefc-f04 */
    if ((vorher ^ jetzt) & jetzt) {                                    /* xor / and / beq @0x8002cf08-10 */
        /* vertikaler Eintritt: b.y = a-Mitte +- (sy + 1) - b.cy (@0x8002cf18-3c, `sw v0,56(a3)`) */
        if (neu_yb)
            *neu_yb = (t6 >= 0) ? (t8 - b_cy + 1 + (int32_t)sy)       /* bgez t6 -> @0x8002cf30-3c */
                                : (t8 - b_cy - 1 - (int32_t)sy);      /* @0x8002cf1c-2c */
        return 1;
    }
    /* Waagrecht: @0x8002cf14-cfb4 schiebt b.x bzw. b.z aus a heraus — der Aufrufer setzt x/z danach auf die
     * gesicherte Lage zurueck (@0x8002c0bc-d4), die Ausschiebung bleibt also ohne Wirkung; nur die Rueckgabe
     * (`ori v0,zero,0x1` @0x8002cfb8) zaehlt. */
    return 1;
}

int re15_kollision_r41_kisten_pass(const re15_rdt_t *rdt, int p, int32_t nx, int32_t nz)
{
    if (!rdt || p < 0 || p >= RE15_SCD_MAX_PROPS) return 0;
    /* Schranke = RDT-Kopf +2 (`lw v0,0x800ac778` / `lbu v0,2(v0)` @0x8002c03c-48), leer -> Schub bleibt
     * (`beq v0,zero,0x8002c0d8` @0x8002c050). Der Pool wird ueber die obj-Nummer indiziert (LAB_80040914
     * @0x8004093c-58, Stride 148); der Durchlauf geht die Nummern 0..nOmodel-1 aufsteigend (s0 = fp + 148*s1,
     * @0x8002c058 / @0x8002c0a4) und bricht beim ersten Treffer ab (`bne v0,zero,0x8002c0b0` @0x8002c080). */
    const int n = (int)rdt->nOmodel;
    for (int id = 0; id < n; id++) {
        int q = -1;
        for (int k = 0; k < RE15_SCD_MAX_PROPS && k < (int)g_scd.prop_count; k++)
            if (g_scd.props[k].active && (int)g_scd.props[k].obj_id == id) { q = k; break; }
        if (q < 0) continue;                                           /* `andi v0,v0,0x1` / `beq` @0x8002c064-70 */
        if (q == p) continue;                                          /* `beq s2,s0,0x8002c088` @0x8002c05c */
        int32_t neu_y = g_scd.props[p].y;
        if (!re15_kollision_r41_8002cdf4(q, p, nx, nz, s_spiegel_y[q], s_spiegel_y[p], &neu_y)) continue;
        g_scd.props[p].y = neu_y;   /* nur der Zweig "vertikaler Eintritt" aendert y; die Ruecksetzung @0x8002c0bc-d4
                                     * stellt nur x/z wieder her (`sw v0,52(s2)` / `sw v0,60(s2)`) */
        FILE *lf = kollision_log();
        if (lf) {
            fprintf(lf, "F%u room=%04X OBJEKT-GESPERRT p=%d obj=%d ziel=(%d,%d) gegen p=%d obj=%d lage=(%d,%d,%d) "
                        "dX=%d dZ=%d sumX=%u sumZ=%u\n",
                    g_engine.frame_count, g_current_room_id, p, (int)g_scd.props[p].obj_id, (int)nx, (int)nz, q,
                    (int)g_scd.props[q].obj_id, (int)g_scd.props[q].x, (int)g_scd.props[q].y, (int)g_scd.props[q].z,
                    (int)((nx + g_scd.props[p].box_cx) - (g_scd.props[q].x + g_scd.props[q].box_cx)),
                    (int)((nz + g_scd.props[p].box_cz) - (g_scd.props[q].z + g_scd.props[q].box_cz)),
                    (unsigned)((uint16_t)g_scd.props[q].box_hx + (uint16_t)g_scd.props[p].box_hx),
                    (unsigned)((uint16_t)g_scd.props[q].box_hz + (uint16_t)g_scd.props[p].box_hz));
            fflush(lf);
        }
        return 1;
    }
    return 0;
}
