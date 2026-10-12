/*
 * RE1.5 Rebuilt — "I have to get the Chief first..." zusaetzlich an der Tuer ROOM1050 -> ROOM10A0
 * (Runde 37, Spur F "sperre1050").
 *
 * Herleitung, Belege und alle Konstanten: include/re15_sperre1050_r37.h und
 * analysis/befunde_runde37/F_sperre1050.md. Kein Asset-Patch: die RDT bleibt byte-true, der Port widmet nach
 * dem Init-Lauf von main00 (und nach re15_adaruf_install) den Tuer-Slot so um, wie es ein Aot_reset tut —
 * dieselbe Bauart wie tuer1060_1040.c, mit DENSELBEN Nachrichten-Bytes und denselben Story-Bits.
 */
#include "re15_sperre1050_r37.h"

#include "re15_tuer1060.h"   /* re15_tuer1060_meldung — derselbe Text */
#include "re15_aot.h"
#include "re15_msg.h"
#include "re15_scd.h"
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

static uint8_t s_gesperrt;

int re15_sperre1050_r37_gesperrt(void) { return s_gesperrt; }

int re15_sperre1050_r37_aktiv(void)
{
    /* Vor der Ada-Rettung gilt allein die Ada-Logik (re15_adaruf.h) — hier nichts. */
    if (!re15_game_flag_get(RE15_S1050_ADA_BANK, RE15_S1050_ADA_BIT)) return 0;
    if (!re15_game_flag_get(RE15_S1050_PLAN_BANK, RE15_S1050_PLAN_BIT)) return 0;   /* Plan noch nicht gefasst */
    return re15_game_flag_get(RE15_S1050_TOD_BANK, RE15_S1050_TOD_BIT) ? 0 : 1;     /* Chief geholt -> frei    */
}

void re15_sperre1050_r37_install(uint16_t room_id)
{
    s_gesperrt = 0;
    if (room_id != RE15_S1050_RAUM) return;
    if (!re15_sperre1050_r37_aktiv()) return;

    /* Nur den echten Tuer-Satz umwidmen: main00 @0x00B5A hat Slot 4 als Tuer angelegt, und re15_adaruf_install
     * hat ihn bei (3,0xBB)=1 so gelassen. Steht dort etwas anderes (Ada-Logik, kuenftige Skript-Aenderung,
     * Test mit leerem Raum), bleibt alles, wie es ist. */
    const re15_aot_t *a = &g_aot.slots[RE15_S1050_SLOT];
    if (!a->active || a->type != RE15_AOT_TYPE_DOOR) return;

    {
        int n = 0;
        const uint8_t *m = re15_tuer1060_meldung(&n);   /* "I have to get the Chief first..." (Bytes der 1060-Sperre) */
        if (!m || n <= 0) return;
        re15_msg_install_text(RE15_S1050_MSG_ID, m, (size_t)n);
        {
            int d = re15_msg_compute_duration(m, (size_t)n, 0);
            if (d > 0 && d < 65535) re15_msg_install_durations(RE15_S1050_MSG_ID, d);
        }
    }
    /* Aot_reset-Semantik (LAB_80040738): sce 1, flags 0x31, Nutzlast (msg, 0xffff, 0); Rechteck und Band bleiben
     * die der Tuer. Das Band steht im Original in rec[2] und wird von Aot_reset nicht geschrieben (@0x8004076c-78
     * fasst nur rec[0]/rec[1] an); der Port haelt das Tuer-Band in door_params (op_door_aot_set: pc[4] = Band 0
     * @0x00B5E), der Text-Platz liest a->band — also hier uebertragen (wie re15_tuer1060_install). */
    g_aot.slots[RE15_S1050_SLOT].band = g_aot.door_params[RE15_S1050_SLOT].band;
    re15_aot_retype(RE15_S1050_SLOT, RE15_S1050_SCE_TEXT, RE15_S1050_FLAGS,
                    RE15_S1050_MSG_ID, RE15_S1050_MASKE, 0);
    s_gesperrt = 1;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[sperre1050] ROOM%04X Slot %d -> Text-Platz msg %d ((3,0xBB)=%d (9,73)=%d (9,71)=%d (3,94)=%d)\n",
            (unsigned)room_id, RE15_S1050_SLOT, RE15_S1050_MSG_ID,
            re15_game_flag_get(RE15_S1050_ADA_BANK, RE15_S1050_ADA_BIT), re15_game_flag_get(9, 73),
            re15_game_flag_get(9, 71), re15_game_flag_get(3, 94));
#endif
}
