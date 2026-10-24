/*
 * RE1.5 Rebuilt — Runde 41 Spur J: RBJ-Marker-Binder byte-true FUN_8001b3f8.
 * Belege (Adressen + Bytes) in include/re15_rbj_binder_r41.h und analysis/befunde_runde41/J_stage3szenen_a.md §0.
 * Datengetrieben; keine Raum-Tabelle.
 */
#include <string.h>
#include "re15_rbj_binder_r41.h"

static uint32_t le32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* Record-Zahl + Marker aller Records; -1 bei kaputtem Block. Gleiche Lesart wie
 * enemy_common.c rbj_record_marker / emd_common.c re15_emd_parse_rbj_record:
 * u32 @+0 = Trailer-Offset (@0x8001b41c), Byte @+4 = Record-Zahl (@0x8001b420 `lbu t2,4(a2)`),
 * Trailer je Record (u32 emr_prefix, u32 edd_off), Marker = u32 @emr_prefix (@0x8001b438). */
static int marker_lesen(const uint8_t *rbj, size_t size, uint32_t mk[RE15_RBJ_BINDER_MAX_REC])
{
    if (!rbj || size < 0x20) return -1;
    uint32_t total = le32(rbj);
    uint32_t nrec  = rbj[4];
    if (total == 0 || total > size || nrec == 0 || nrec > RE15_RBJ_BINDER_MAX_REC) return -1;
    for (uint32_t r = 0; r < nrec; r++) {
        size_t toff = (size_t)total + (size_t)r * 8;
        if (toff + 8 > size) return -1;
        uint32_t pre = le32(rbj + toff);
        if (pre == 0 || (size_t)pre + 4 > size) return -1;
        mk[r] = le32(rbj + pre);
    }
    return (int)nrec;
}

int re15_rbj_binder(const uint8_t *rbj, size_t size, re15_rbj_bindung_t *out)
{
    if (!out) return -1;
    out->spieler = -1;
    memset(out->gegner, 0xFF, sizeof out->gegner);
    uint32_t mk[RE15_RBJ_BINDER_MAX_REC];
    int nrec = marker_lesen(rbj, size, mk);
    if (nrec <= 0) return -1;
    uint32_t a3 = 0;                                        /* @0x8001b410 addu a3,zero,zero (einmal) */
    for (int r = 0; r < nrec; r++) {                        /* @0x8001b4cc..d4 Record-Schleife */
        uint32_t a0 = mk[r];
        if (a0 & 1u) out->spieler = r;                      /* @0x8001b440/@0x8001b450: letzter gewinnt */
        a0 >>= 1;                                           /* @0x8001b448 srl a0,a0,1 */
        while (a0) {                                        /* @0x8001b464 beq a0,zero / @0x8001b4c0 bne */
            if (a0 & 1u) {                                  /* @0x8001b470/@0x8001b474 */
                uint32_t i = a3 & 0xFFu;                    /* @0x8001b47c andi v0,a3,0xff */
                if (i < RE15_RBJ_BINDER_MAX_GEGNER)         /* PORT-WAHL Schutzgrenze (Header) */
                    out->gegner[i] = (int8_t)r;             /* @0x8001b4a0 sw t1 -> +0x180 */
            }
            a0 >>= 1;                                       /* @0x8001b478 srl a0,a0,1 */
            a3++;                                           /* @0x8001b4c4 Delay-Slot addiu a3,a3,1 */
        }
    }
    return 0;
}

int re15_rbj_binder_spieler(const uint8_t *rbj, size_t size)
{
    re15_rbj_bindung_t b;
    return re15_rbj_binder(rbj, size, &b) == 0 ? b.spieler : -1;
}

int re15_rbj_binder_gegner(const uint8_t *rbj, size_t size, int gegner_index)
{
    re15_rbj_bindung_t b;
    if (gegner_index < 0 || gegner_index >= RE15_RBJ_BINDER_MAX_GEGNER) return -1;
    return re15_rbj_binder(rbj, size, &b) == 0 ? (int)b.gegner[gegner_index] : -1;
}

int re15_rbj_binder_alt_gegner(const uint8_t *rbj, size_t size, int gegner_index)
{
    uint32_t mk[RE15_RBJ_BINDER_MAX_REC];
    int nrec = marker_lesen(rbj, size, mk);
    if (nrec <= 0 || gegner_index < 0 || gegner_index > 30) return -1;
    for (int r = 0; r < nrec; r++)
        if (mk[r] & (1u << (1 + gegner_index))) return r;
    return -1;
}

/* ---- Messzeilen ------------------------------------------------------------------------------------- */
#include <stdio.h>
extern unsigned g_current_room_id;   /* re15_room.h */

void re15_rbj_binder_raum_protokoll(const uint8_t *rbj, size_t size)
{
    re15_rbj_bindung_t b;
    if (re15_rbj_binder(rbj, size, &b) != 0) {
        fprintf(stderr, "[rbj-binder] ROOM%04X RBJ ungueltig -> keine Bindung\n", g_current_room_id);
        return;
    }
    char zeile[512]; int n = 0;
    n += snprintf(zeile + n, sizeof zeile - (size_t)n, "[rbj-binder] ROOM%04X Spieler rec%d (alt rec0) Gegner",
                  g_current_room_id, b.spieler);
    for (int i = 0; i < RE15_RBJ_BINDER_MAX_GEGNER && n < (int)sizeof zeile - 32; i++) {
        int alt = re15_rbj_binder_alt_gegner(rbj, size, i);
        if (b.gegner[i] < 0 && alt < 0) continue;
        n += snprintf(zeile + n, sizeof zeile - (size_t)n, " %d:rec%d(alt rec%d)", i, (int)b.gegner[i], alt);
    }
    fprintf(stderr, "%s\n", zeile);
}

void re15_rbj_binder_aktor_protokoll(int slot, int typ, int marker_slot, int rec, int gueltig)
{
    fprintf(stderr, "[rbj-binder] ROOM%04X Aktor %d Typ 0x%02X Marker-Slot %d -> rec%d %s\n",
            g_current_room_id, slot, typ & 0xFF, marker_slot, rec, gueltig ? "gebunden" : "ohne Bank/Parse");
}
