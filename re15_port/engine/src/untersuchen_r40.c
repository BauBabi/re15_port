/*
 * RE1.5 Rebuilt — Runde 40 "untersuchen1120": Text-Plaetze fuer ROOM1120/1121 msg 1/3/4.
 *
 * Herleitung, Belege und alle Konstanten: include/re15_untersuchen_r40.h und
 * analysis/untersuchungstexte_2026-10-23/README.md. Kein Asset-Patch: die RDTs bleiben byte-true.
 */
#include "re15_untersuchen_r40.h"

#include "re15_aot.h"        /* g_aot, re15_aot_set */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

/* Form ROOM1120 main00 @0x00CF8 `2c 03 01 31 00 00 b2 0c 9e ee e8 03 e8 03 06 00 ff ff 00 00`;
 * getauscht: Platz (pc[1]), Rechteck (pc[6..13]), msg (pc[14]). */
const uint8_t re15_untersuchen_r40_saetze[3][20] = {
    /* Bild Suedwand: (-200, 1150) (1150, 1000) -> msg 1 */
    { 0x2c, RE15_U40_SLOT_BILD, 0x01, RE15_U40_SAT, 0x00, 0x00,
      0x38, 0xff, 0x7e, 0x04, 0x7e, 0x04, 0xe8, 0x03,
      RE15_U40_MSG_BILD, 0x00, 0xff, 0xff, 0x00, 0x00 },
    /* Jalousiefenster Ostwand: (6150, -1050) (1000, 4900) -> msg 3 */
    { 0x2c, RE15_U40_SLOT_FENSTER, 0x01, RE15_U40_SAT, 0x00, 0x00,
      0x06, 0x18, 0xe6, 0xfb, 0xe8, 0x03, 0x24, 0x13,
      RE15_U40_MSG_FENSTER, 0x00, 0xff, 0xff, 0x00, 0x00 },
    /* Aktenschrank Zelle 10: (4600, -5800) (1350, 1850) -> msg 4 */
    { 0x2c, RE15_U40_SLOT_AKTEN, 0x01, RE15_U40_SAT, 0x00, 0x00,
      0xf8, 0x11, 0x58, 0xe9, 0x46, 0x05, 0x3a, 0x07,
      RE15_U40_MSG_AKTEN, 0x00, 0xff, 0xff, 0x00, 0x00 },
};

static int16_t le_s16(const uint8_t *p) { return (int16_t)(uint16_t)(p[0] | (p[1] << 8)); }

/* Einen kurzen sce-1-Satz so ablegen, wie op_aot_set (scd_vm.c, sce-1-Zweig) es tut: Ecke pc[6..9] +
 * Ausdehnung pc[10..13] -> Mitte/halbe Ausdehnung, msg = pc[14], Pausemaske = u16 pc[16..17],
 * band = pc[4], sce_flags = pc[3] (Form wie story_r36.c text_platz, kurzer Zweig). */
static void text_platz(const uint8_t *pc)
{
    const int slot = (int)pc[1];
    const int16_t rx = le_s16(&pc[6]), rz = le_s16(&pc[8]);
    const int16_t rw = le_s16(&pc[10]), rd = le_s16(&pc[12]);
    const int32_t cx = (int32_t)rx + (int32_t)rw / 2;
    const int32_t cz = (int32_t)rz + (int32_t)rd / 2;
    const int32_t hw = (int32_t)(rw < 0 ? -rw : rw) / 2;
    const int32_t hh = (int32_t)(rd < 0 ? -rd : rd) / 2;
    re15_aot_set(slot, RE15_AOT_TYPE_MESSAGE, pc[14], cx, cz, hw, hh);
    g_aot.slots[slot].pause_mask16 = (uint16_t)(pc[16] | (pc[17] << 8));
    g_aot.slots[slot].band         = pc[4];
    g_aot.slots[slot].sce_flags    = pc[3];
}

void re15_untersuchen_r40_install(uint16_t room_id)
{
    if (room_id != RE15_U40_RAUM_LEON && room_id != RE15_U40_RAUM_ELZA) return;
    for (int i = 0; i < 3; i++) {
        const uint8_t *pc = re15_untersuchen_r40_saetze[i];
        if (g_aot.slots[pc[1]].active) {          /* fremder Satz im Platz: nichts anfassen */
#ifdef RE15_PLATFORM_PC
            fprintf(stderr, "[untersuchen40] ROOM%04X: Platz %d belegt (Typ %d) -> msg %d nicht angelegt\n",
                    room_id, pc[1], g_aot.slots[pc[1]].type, pc[14]);
#endif
            continue;
        }
        text_platz(pc);
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[untersuchen40] ROOM%04X: Text-Platz %d -> msg %d (Form @0x00CF8)\n", room_id, pc[1], pc[14]);
#endif
    }
}
