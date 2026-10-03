/*
 * RE1.5 Rebuilt — Tuer ROOM1060 -> ROOM1040 gesperrt, bis Leon den Chief geholt hat
 * (Runde 35, Spur L, Punkt 1).
 *
 * Herleitung, Belege und alle Konstanten: include/re15_tuer1060.h und
 * analysis/befunde_runde35/L_cut1150.md. Kein Asset-Patch: die RDT bleibt byte-true, der Port
 * widmet nach dem Init-Lauf von main00 den Tuer-Slot so um, wie es ein Aot_reset tun wuerde
 * (gleiche Bauart wie tuer1120_1130.c).
 */
#include "re15_tuer1060.h"

#include "re15_aot.h"
#include "re15_msg.h"
#include "re15_scd.h"
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

/* "I have to get the Chief first..." als .msg-Rohbytes (tools/r35_l/texte_bauen.py).
 * ⛔ NEUER TEXT (Nutzervorgabe AUFTRAG Z. 68), kein Original-Satz. Nur die FORM ist belegt:
 *   Kopf `04 02` und Ende `01 00` wie ROOM1130 msg 1 @0x0B46 ("It's not necessary to go back.");
 *   Glyphen = Umkehrung der Port-Tabelle msg_common.c re15_msg_glyph; "..." = 3 x 0x57
 *   (ROOM1170 msg 6 @0x1AA4 "great..."); "I have to " ROOM1170 @0x1C4B; " first" ROOM11B0 @0x1BAE.
 * Eine Zeile, 199 px (Vorschubtabelle include/font_width.h; Grenze 271 px, tuer1120_1130.c). */
static const uint8_t k_meldung[] = {
    0x04, 0x02,
    /* I   _  h    a    v    e   _  t    o   _  g    e    t   _  t    h    e   _ */
    0x25,0x00,0x44,0x3d,0x52,0x41,0x00,0x50,0x4b,0x00,0x43,0x41,0x50,0x00,0x50,0x44,0x41,0x00,
    /* C    h    i    e    f   _  f    i    r    s    t    .    .    .  */
    0x1f,0x44,0x45,0x41,0x42,0x00,0x42,0x45,0x4e,0x4f,0x50,0x57,0x57,0x57,
    0x01, 0x00,
};

static uint8_t s_gesperrt;

const uint8_t *re15_tuer1060_meldung(int *out_len)
{
    if (out_len) *out_len = (int)sizeof k_meldung;
    return k_meldung;
}

int re15_tuer1060_gesperrt(void) { return s_gesperrt; }

int re15_tuer1060_sperre_aktiv(void)
{
    if (re15_game_flag_get(RE15_TUER1060_TOD_BANK, RE15_TUER1060_TOD_BIT)) return 0;   /* Chief geholt */
    if (re15_game_flag_get(RE15_TUER1060_K_BANK, RE15_TUER1060_K_BIT)) return 1;       /* Plan gefasst */
    return re15_game_flag_get(RE15_TUER1060_IRONS_BANK, RE15_TUER1060_IRONS_BIT) ? 0 : 1; /* nie beim Chief */
}

void re15_tuer1060_install(uint16_t room_id)
{
    s_gesperrt = 0;
    if (room_id != RE15_TUER1060_RAUM) return;
    if (!re15_tuer1060_sperre_aktiv()) return;

    /* Nur den echten Tuer-Satz umwidmen: main00 @0x00D52 hat Slot 2 als Tuer angelegt. Steht dort
     * etwas anderes (kuenftige Skript-Aenderung, Test mit leerem Raum), bleibt alles, wie es ist. */
    const re15_aot_t *a = &g_aot.slots[RE15_TUER1060_SLOT];
    if (!a->active || a->type != RE15_AOT_TYPE_DOOR) return;

    re15_msg_install_text(RE15_TUER1060_MSG_ID, k_meldung, sizeof k_meldung);
    {
        int d = re15_msg_compute_duration(k_meldung, sizeof k_meldung, 0);
        if (d > 0 && d < 65535) re15_msg_install_durations(RE15_TUER1060_MSG_ID, d);
    }
    /* Aot_reset-Semantik (LAB_80040738): sce 1, flags 0x31, Nutzlast (msg, 0xffff, 0); Rechteck und
     * Band bleiben die der Tuer. Das Band steht im Original in rec[2] und wird von Aot_reset nicht
     * geschrieben (@0x8004076c-78 fasst nur rec[0]/rec[1] an); der Port haelt das Tuer-Band in
     * door_params (op_door_aot_set: pc[4] = Etage 0 @0x00D56), der Text-Platz liest a->band. */
    g_aot.slots[RE15_TUER1060_SLOT].band = g_aot.door_params[RE15_TUER1060_SLOT].band;
    re15_aot_retype(RE15_TUER1060_SLOT, RE15_TUER1060_SCE_TEXT, RE15_TUER1060_FLAGS,
                    RE15_TUER1060_MSG_ID, RE15_TUER1060_MASKE, 0);
    s_gesperrt = 1;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[tuer1060] ROOM%04X Slot %d -> Text-Platz msg %d ((9,73)=%d (9,71)=%d (3,94)=%d)\n",
            (unsigned)room_id, RE15_TUER1060_SLOT, RE15_TUER1060_MSG_ID,
            re15_game_flag_get(9, 73), re15_game_flag_get(9, 71), re15_game_flag_get(3, 94));
#endif
}
