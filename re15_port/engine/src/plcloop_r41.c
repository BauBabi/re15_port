/*
 * RE1.5 Rebuilt — Runde 41 Spur E "kleinkram_b", Punkt 1: Clip-Ende des Spielers im Kommandowort 4 (Plc_motion):
 * Schleife bei +0x1c4 & 4, sonst Halten — wie 0x80050cb8 Phase 1/2, auf die FUN_80030660 den Spieler fuehrt
 * (@0x800306a4 PTR_LAB_80073e30[+0x5] = 0x80050cb8). Herleitung: include/re15_plcloop_r41.h; Dossier
 * analysis/befunde_runde41/E_kleinkram_b.md.
 */
#include "re15_plcloop_r41.h"

#include <stddef.h>

#include "re15_player.h"
#include "re15_enemy.h"
#include "re15_szenenleon_r36.h"   /* re15_szl_schub_spieler_phase */
#include "re15_rbj_binder_r41.h"   /* Runde 41 Spur K (NB1 M1): Spieler-Record = Binder FUN_8001b3f8 */

static unsigned s_umlaeufe, s_halte;

static uint32_t rd32(const uint8_t *p) { return (uint32_t)(p[0] | (p[1] << 8) | (p[2] << 16) | ((uint32_t)p[3] << 24)); }
static uint32_t rd16(const uint8_t *p) { return (uint32_t)(p[0] | (p[1] << 8)); }

/* Bilderzahl von Clip `clip` des Raum-RBJ-Records mit Marker-Bit 0 (= Spieler-Slot; Binder FUN_8001b3f8 bindet
 * Records ueber dieses Maskenwort, enemy_common.c rbj_record_marker). RBJ-Aufbau wie tools/r34n_d/rbj_zensus.py:
 * [0] total, [4] nrec, Trailer @total + 8*r = (EMR-Praefix, EDD-Offset); EDD: u16 count0, u16 off0 (= 4*Clipzahl),
 * je Clip u16 count / u16 offset. 0 = kein RBJ / kein Spieler-Record / Clip ausser Bereich. */
static int rbj_spieler_clip_len(int clip)
{
    size_t size = 0;
    const uint8_t *rbj = re15_rbj_room(&size);
    if (!rbj || size < 0x20 || clip < 0) return 0;
    uint32_t total = rd32(rbj), nrec = rbj[4];
    if (total == 0 || total > size || nrec == 0 || nrec > 8) return 0;
    /* Runde 41 Spur K (Nachbesserung 1, M1): der Clip-Kopf, den 0x80050cb8 an f314 gibt, ist der des Spieler-KANALS
     * (+0x180/+0x184: `lw a0,384(v0)` @0x80050d40, `lw a1,388(v0)` @0x80050d48). Den Kanal schreibt der Binder
     * FUN_8001b3f8 fuer JEDEN Record mit Marker-Bit 0 (`andi v0,a0,0x1` @0x8001b440, `sw v0,0(t3)` @0x8001b450 /
     * `sw v0,4(t3)` @0x8001b460, t3 = 0x800acbd4) - der LETZTE gewinnt, nicht der erste. ROOM3091: rec0 (Marker 1,
     * 1 Clip) und rec2 (Marker 3) -> Kanal = rec2, Clip 13 = 40 Bilder (Original-RAM w180 = RBJ+rec2). */
    int pr = re15_rbj_binder_spieler(rbj, size);
    if (pr < 0) return 0;
    for (uint32_t r = 0; r < nrec; r++) {
        if ((int)r != pr) continue;                             /* Runde 41 Spur K: Record des Binders */
        size_t toff = (size_t)total + (size_t)r * 8;
        if (toff + 8 > size) return 0;
        uint32_t pre = rd32(rbj + toff), edd = rd32(rbj + toff + 4);
        if (pre == 0 || (size_t)pre + 4 > size || edd == 0 || (size_t)edd + 4 > size) continue;
        if (!(rd32(rbj + pre) & 1u)) continue;                  /* Marker-Bit 0 = Spieler */
        uint32_t off0 = rd16(rbj + edd + 2);
        uint32_t ncl  = off0 / 4;
        if ((uint32_t)clip >= ncl || (size_t)edd + 4 * (size_t)clip + 2 > size) return 0;
        return (int)rd16(rbj + edd + 4 * (size_t)clip);
    }
    return 0;
}

int re15_plcloop_r41_clip_len(const re15_actor_t *p)
{
    if (!p) return 0;
    int clip = (int)p->motion;
    if (clip < 0) return 0;
    /* Bankwahl wie anim_select_common.c re15_actor_anim_select: anim_use_pl00 -> PL00 (COMMON), sonst die
     * Raum-RBJ (def_*); ohne Raum-RBJ ist def_* == PL00. */
    if (!p->anim_use_pl00) {
        int n = rbj_spieler_clip_len(clip);
        if (n > 0) return n;
    }
    return re15_player_pl00_clip_frames(clip);
}

void re15_plcloop_r41_takt(re15_actor_t *p)
{
    /* Nur Kommandowort 4 UND Sub 0: FUN_80030660 dispatcht ueber +0x5 (@0x8003068c lbu DAT_800aca59) — allein
     * Sub 0 ist die Bewegungs-FSM 0x80050cb8; Plc_dest-Subs (z.B. 0x13 = Container-Schub ROOM11B0, eigener
     * Bildzaehler @0x80052ac0) und Laeufe zaehlen selbst (dasselbe Gate wie re15_actor_clip_len fuer NPCs). */
    if (!p || p->state != 4 || p->sub_state_1 != 0 || p->walk_active) return;
    /* Port-seitige Besitzer des Spieler-Bildzaehlers (sie fuehren +0x5 nicht im Aktor): der Skript-Schub ROOM11B0
     * (szenenleon_r36.c, Sub 0x13 @0x80052864, eigener Umlauf @0x80052ac0) und der Substate-8-Schub (player_common.c). */
    if (re15_szl_schub_spieler_phase() >= 0 || re15_player_push_substate()) return;
    int fc = re15_plcloop_r41_clip_len(p);
    if (fc <= 0 || (int)p->anim_frame < fc) return;         /* Clip laeuft noch / Laenge unbekannt: wie bisher */
    if (p->anim_flags & 0x04u) { p->anim_frame = 0; s_umlaeufe++; }           /* SCHLEIFE @0x80050dbc-c8, @0x8001f63c */
    else                       { p->anim_frame = (uint16_t)(fc - 1); s_halte++; } /* HALTEN  @0x80050da4 (fc-1 posiert) */
}

unsigned re15_plcloop_r41_umlaeufe(void) { return s_umlaeufe; }
unsigned re15_plcloop_r41_halte(void)    { return s_halte; }
