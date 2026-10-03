/*
 * RE1.5 Rebuilt — Runde 35 Spur J "affen": ROOM11C0 Gorilla-Boss (Typ 0x27) + Ada-Szene.
 * Belege und Messungen: include/re15_affen.h (Kopf) und analysis/befunde_runde35/J_affen.md.
 */
#include "re15_affen.h"
#include <string.h>

/* (1) NPC-Wandklemme: FUN_8003b0a4 vergleicht `entity+0x82` mit `floor >> 4` der Zelle
 *     (`lbu v1,130(a3)` @0x8003b228-3c; Zonen-Zwilling `sra v0,v0,28` @0x8003ba04). Das Byte wird
 *     byte-true aus Sce_em_set pc[4] geseedet (@0x800421c8-d0) und per Member_set 0x12 (FUN_8004116c
 *     Fall 0x12 -> +0x82) vom Skript gesetzt — ROOM11C0 sub07 @0x1C66 `Member_set 12 = 2`. */
int re15_affen_npc_band(const re15_actor_t *e)
{
    return (int)e->floor;                               /* +0x82 */
}

/* (2) Parts ohne Knochen: FUN_8001e5b0 (EXE) — Schleife ueber +0x83 = Mesh-Zahl Parts; je Part
 *     rel = EMR[8 + 6*i] (`local_38`/`puVar14` laufen 6 Byte je Part weiter, OHNE Obergrenze
 *     Knochenzahl); ab Part >= EMR+4 (`uVar21 == 0`) Elternmatrix = &DAT_80072d4c (Identitaet,
 *     t = 0, re15_disasm read 0x80072d4c) und Eltern-Record = 0. FUN_8001f3bc posiert nur
 *     EMR+4 Knochen (`uVar7 = *(byte *)(param_1 + 4)`), die lokale Rotation bleibt die
 *     Binder-Identitaet. FUN_8001e9ec: m1 = Eltern * Lokal, Vertex = View * m1 -> weltfest. */
int re15_affen_surplus_part_world(const re15_emd_skeleton_t *sk, int part,
                                  int32_t rot[9], int32_t trans[3])
{
    if (!sk || part < sk->bone_count) return 0;
    if (!sk->emr_raw || (size_t)(8 + 6 * part + 6) > sk->emr_raw_size) return 0;
    const uint8_t *p = sk->emr_raw + 8 + 6 * part;      /* EMR[8+6i] wie @FUN_8001e5b0 */
    memset(rot, 0, 9 * sizeof(int32_t));
    rot[0] = rot[4] = rot[8] = 0x1000;                  /* DAT_80072d4c = Identitaet (Q12) */
    for (int k = 0; k < 3; k++)
        trans[k] = (int32_t)(int16_t)((uint16_t)p[2*k] | ((uint16_t)p[2*k + 1] << 8));
    return 1;
}

/* (3) Trefferzaehler fuer den Vergeltungs-Sprung (NUTZER-VORGABE 3). */
void re15_affen_treffer_zaehlen(re15_actor_t *e)
{
    if (e->mag_hit_ctr < 255u) e->mag_hit_ctr++;        /* ein Flinch-Eintritt = ein Treffer (+0x93-Latch) */
}

uint8_t re15_affen_flinch_exit_sub(re15_actor_t *e)
{
    if (e->mag_1e3 != 0) return 9;                      /* byte-true Variante @0x8011b1c8-d8 (nie gesetzt) */
    if (e->mag_hit_ctr >= RE15_AFFEN_TREFFER_BIS_SPRUNG) {
        e->mag_hit_ctr = 0;
        return 7;                                       /* Vergeltungs-Sprung @0x8011b188-98 */
    }
    return 3;                                           /* PORT-WAHL: zurueck in die Jagd (A[3]/B[3]) */
}
