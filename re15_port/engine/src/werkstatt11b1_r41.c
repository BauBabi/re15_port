/*
 * RE1.5 Rebuilt — Runde 41 Spur E "kleinkram_b", Punkt 3: Elzas Werkstatt-Tuer ROOM11B1 bleibt dauerhaft zu.
 * Herleitung, Belege und alle Konstanten: include/re15_werkstatt11b1_r41.h; Dossier
 * analysis/befunde_runde41/E_kleinkram_b.md. Muster: szenenleon_r36.c (re15_szl_init_lauf / re15_szl_ereignis,
 * gen/szl11b0_szene.inc k_szl_tuerprog = Leons Gegenstueck in ROOM11B0) und tuersperre_r39.c (RDT-Anker).
 */
#include "re15_werkstatt11b1_r41.h"

#include <string.h>

#include "re15_scd.h"
#include "re15_room.h"
#include "re15_engine.h"
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

static unsigned s_starts;

/* Ereignis 24 (ROOM11B1): den offenen Zweig von sub00 umkehren. Jede Zeile traegt ihr Vorbild.
 *  +0x00  4b 00 07 00                 Cut_replace(0,7)+Nop  = sub00 @0x0110E/@0x01111 noch einmal (selbstinvers,
 *                                     LAB_80040414 @0x80040458..@0x80040494): RVD-Zonen zeigen wieder Bild 0 (zu)
 *  +0x04  4b 02 08 00                 Cut_replace(2,8)+Nop  = sub00 @0x01112/@0x01115 noch einmal -> Bild 2 (zu)
 *  +0x08  37 03 0c ff                 Sca_id_set(3,0x0C)=0xff   Durchgangszelle solid (RDT @0x00EC8 u0=ff;
 *                                     Umkehr von sub00 @0x01116; Form Leon gen/szl11b0_szene.inc +0x0000)
 *  +0x0C  39 03 0c 03                 Sca_floor_set(3,0x0C)=0x03 (RDT @0x00EC8 fl=03; Umkehr von @0x0111A)
 *  +0x10  2c 08 01 31 00 00 80 a8 d0 bc b0 04 a4 06 0c 00 ff ff 00 00
 *                                     Aot_set Platz 8 = sub00 @0x0112A Byte fuer Byte: Text msg 12 @0x179F
 *                                     "The door is locked.", Rechteck (-22400,-17200,1200,1700)
 *  +0x24  06 00 0c 00                 Ifel_ck (Laenge 0x0C)
 *  +0x28  23 00 0a 00 07 00           Cmp(work_vars[0x0A]==7)  angezeigter Cut (sub00 @0x01102 Form; Cut_chg
 *                                     LAB_800402a0 @0x800402fc schreibt work_vars[0x0A])
 *  +0x2E  29 00                       Cut_chg 0   (sub00 @0x01108 `29 07` umgekehrt)
 *  +0x30  3c 01                       Cut_auto 1  (sub00 @0x0110A)
 *  +0x32  08 00                       Endif
 *  +0x34  06 00 0c 00                 Ifel_ck (Laenge 0x0C)
 *  +0x38  23 00 0a 00 08 00           Cmp(work_vars[0x0A]==8)
 *  +0x3E  29 02                       Cut_chg 2
 *  +0x40  3c 01                       Cut_auto 1
 *  +0x42  08 00                       Endif
 *  +0x44  01 00                       Evt_end */
static const uint8_t k_wk11b1_prog[70] = {
    0x4b, 0x00, 0x07, 0x00, 0x4b, 0x02, 0x08, 0x00, 0x37, 0x03, 0x0c, 0xff, 0x39, 0x03, 0x0c, 0x03,
    0x2c, 0x08, 0x01, 0x31, 0x00, 0x00, 0x80, 0xa8, 0xd0, 0xbc, 0xb0, 0x04, 0xa4, 0x06, 0x0c, 0x00,
    0xff, 0xff, 0x00, 0x00, 0x06, 0x00, 0x0c, 0x00, 0x23, 0x00, 0x0a, 0x00, 0x07, 0x00, 0x29, 0x00,
    0x3c, 0x01, 0x08, 0x00, 0x06, 0x00, 0x0c, 0x00, 0x23, 0x00, 0x0a, 0x00, 0x08, 0x00, 0x29, 0x02,
    0x3c, 0x01, 0x08, 0x00, 0x01, 0x00,
};

/* RDT-Anker: ROOM11B1 sub00 @0x010F6..@0x0115C (102 B) im AUSGELIEFERTEN Stand. Weicht die RDT ab (fremde Datei,
 * kuenftige Skript-Aenderung), tut das Modul nichts — dann stimmen auch die Umkehr-Offsets oben nicht mehr. */
static const uint8_t k_wk11b1_sub00[RE15_WK11B1_ANKER_LEN] = {
    0x06, 0x00, 0x30, 0x00, 0x21, 0x03, 0x83, 0x01, 0x06, 0x00, 0x0c, 0x00, 0x23, 0x00, 0x0a, 0x00,
    0x00, 0x00, 0x29, 0x07, 0x3c, 0x01, 0x08, 0x00, 0x4b, 0x00, 0x07, 0x00, 0x4b, 0x02, 0x08, 0x00,
    0x37, 0x03, 0x0c, 0x00, 0x39, 0x03, 0x0c, 0x40, 0x2e, 0x03, 0x00, 0x00, 0x34, 0x02, 0x58, 0xb4,
    0x07, 0x00, 0x34, 0x00, 0x2c, 0x08, 0x01, 0x31, 0x00, 0x00, 0x80, 0xa8, 0xd0, 0xbc, 0xb0, 0x04,
    0xa4, 0x06, 0x0c, 0x00, 0xff, 0xff, 0x00, 0x00, 0x46, 0x05, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x37, 0x02, 0x04, 0x00, 0x39, 0x02, 0x04, 0x40, 0x37, 0x03, 0x09, 0x00, 0x39, 0x03,
    0x09, 0x40, 0x00, 0x00, 0x01, 0x00,
};

int re15_werkstatt11b1_r41_anker_ok(void)
{
    const uint8_t *raw = g_room_rdt.raw;
    if ((unsigned)g_current_room_id != RE15_WK11B1_RAUM) return 0;
    if (!g_room_rdt_ok || !raw || (uint32_t)g_room_rdt.raw_size < RE15_WK11B1_ANKER_OFF + RE15_WK11B1_ANKER_LEN) return 0;
    return memcmp(raw + RE15_WK11B1_ANKER_OFF, k_wk11b1_sub00, RE15_WK11B1_ANKER_LEN) == 0;
}

void re15_werkstatt11b1_r41_init_lauf(uint16_t room_id)
{
    if (room_id != (uint16_t)RE15_WK11B1_RAUM) return;                       /* Leon 11B0 + alle anderen Raeume */
    if (!re15_game_flag_get(RE15_WK11B1_FLAG_BANK, RE15_WK11B1_FLAG_BIT)) return;   /* sub00 nimmt den Else-Zweig: zu */
    if (!re15_werkstatt11b1_r41_anker_ok()) {
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[wk11b1-r41] ROOM11B1: RDT weicht vom Anker sub00 @0x010F6 ab -> nichts\n");
#endif
        return;
    }
    {
        int slot = scd_event_fire(RE15_WK11B1_EREIGNIS);
        if (slot >= 0) s_starts++;
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, slot >= 0
                ? "[wk11b1-r41] ROOM11B1 (3,0x83)=1: Ereignis %d im Init-Lauf hinter sub00 — Werkstatt-Tuer bleibt zu "
                  "(Zelle (3,0x0C) u0=ff, Text-Platz 8 msg 12, Cut_replace zurueck)\n"
                : "[wk11b1-r41] ROOM11B1: KEIN freier Faden fuer Ereignis %d\n", RE15_WK11B1_EREIGNIS);
#endif
    }
}

const uint8_t *re15_werkstatt11b1_r41_ereignis(uint16_t room_id, uint8_t event_id)
{
    if (room_id != (uint16_t)RE15_WK11B1_RAUM || event_id != RE15_WK11B1_EREIGNIS) return NULL;
    return k_wk11b1_prog;
}

const uint8_t *re15_werkstatt11b1_r41_programm(int *out_len)
{
    if (out_len) *out_len = (int)sizeof k_wk11b1_prog;
    return k_wk11b1_prog;
}

unsigned re15_werkstatt11b1_r41_starts(void) { return s_starts; }
