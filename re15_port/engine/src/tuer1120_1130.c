/*
 * RE1.5 Rebuilt — Tuer ROOM1130 -> ROOM1120 erst nach der ersten Irons-Szene (Runde 33, Thema R).
 *
 * Herleitung, Belege und alle Konstanten: include/re15_tuer1120.h und
 * analysis/befunde_runde33/tuer_1130_1120.md. Kein Asset-Patch: die RDT bleibt byte-true, der
 * Port retypt nach dem Init-Lauf von main00 den Tuer-Slot so, wie es ein Aot_reset tun wuerde.
 */
#include "re15_tuer1120.h"

#include "re15_aot.h"
#include "re15_msg.h"
#include "re15_scd.h"
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

/* "I have to report the situation / to the chief first..." als .msg-Rohbytes.
 * ⛔ NEUER TEXT (Nutzervorgabe), kein Original-Satz. Nur die FORM ist belegt:
 *   Kopf `04 02` und Ende `01 00` wie ROOM1130 msg 1 @0x0B46
 *     (`04 02 25 50 3a 4f 00 … 3e 3d 3f 47 57 01 00` = "It's not necessary to go back.");
 *   Zeilenumbruch 0x08 (FUN_80028868, msg_common.c re15_msg_layout).
 * Die Glyphen sind die des ausgelieferten Satzes (Tabelle msg_common.c re15_msg_glyph), jedes
 * Wort liegt so im Auslieferungsstand vor:
 *   "I have to report"  ROOM1170.RDT @0x1C4B (msg 16)  25 00 44 3d 52 41 00 50 4b 00 4e 41 4c 4b 4e 50
 *   "the situation"     ROOM10D0.RDT @0x2310           50 44 41 00 4f 45 50 51 3d 50 45 4b 4a
 *   "to the chief"      ROOM1170.RDT @0x1C61 (msg 16)  50 4b 00 50 44 41 00 3f 44 45 41 42
 *   " first"            ROOM11B0.RDT @0x1BAE           00 42 45 4e 4f 50
 *   "..." = 3 x 0x57    ROOM1170.RDT @0x1AA4 (msg 6 "great...")
 * Umbruch nach "situation": Zeile 1 = 199 px, Zeile 2 = 127 px (Vorschubtabelle
 * include/font_width.h = DEBUG.BIN[0x4416+code]); in einem Stueck waeren es 330 px. Von 2286
 * ausgelieferten Zeilen (alle RDTs, Umbruch 0x08/0x02) messen 99 % <= 271 px, die breiteste
 * regulaer umbrochene 286 px (ROOM1070 msg 1); ROOM1130 msg 1 misst 198 px. */
static const uint8_t k_meldung[] = {
    0x04, 0x02,
    /* I   _  h    a    v    e   _  t    o   _  r    e    p    o    r    t  */
    0x25,0x00,0x44,0x3d,0x52,0x41,0x00,0x50,0x4b,0x00,0x4e,0x41,0x4c,0x4b,0x4e,0x50,
    /* _  t    h    e   _  s    i    t    u    a    t    i    o    n  */
    0x00,0x50,0x44,0x41,0x00,0x4f,0x45,0x50,0x51,0x3d,0x50,0x45,0x4b,0x4a,
    0x08,
    /* t   o   _  t    h    e   _  c    h    i    e    f   _  f    i    r    s    t    .    .    .  */
    0x50,0x4b,0x00,0x50,0x44,0x41,0x00,0x3f,0x44,0x45,0x41,0x42,0x00,0x42,0x45,0x4e,0x4f,0x50,
    0x57,0x57,0x57,
    0x01, 0x00,
};

static uint8_t s_gesperrt;

const uint8_t *re15_tuer1120_meldung(int *out_len)
{
    if (out_len) *out_len = (int)sizeof k_meldung;
    return k_meldung;
}

int re15_tuer1120_gesperrt(void) { return s_gesperrt; }

void re15_tuer1120_install(uint16_t room_id)
{
    s_gesperrt = 0;
    if (room_id != RE15_TUER1120_RAUM) return;
    if (re15_game_flag_get(RE15_TUER1120_FLAG_BANK, RE15_TUER1120_FLAG_BIT)) return;

    /* Nur den echten Tuer-Satz umwidmen: main00 @0x008AE hat Slot 1 als Tuer angelegt. Steht dort
     * etwas anderes (kuenftige Skript-Aenderung, Test mit leerem Raum), bleibt alles, wie es ist. */
    const re15_aot_t *a = &g_aot.slots[RE15_TUER1120_SLOT];
    if (!a->active || a->type != RE15_AOT_TYPE_DOOR) return;

    re15_msg_install_text(RE15_TUER1120_MSG_ID, k_meldung, sizeof k_meldung);
    {
        int d = re15_msg_compute_duration(k_meldung, sizeof k_meldung, 0);
        if (d > 0 && d < 65535) re15_msg_install_durations(RE15_TUER1120_MSG_ID, d);
    }
    /* Aot_reset-Semantik (LAB_80040738): sce 1, flags 0x31, Nutzlast (msg, 0xffff, 0); Rechteck und
     * Band bleiben die der Tuer. Das Band steht im Original in rec[2] und wird von Aot_reset nicht
     * geschrieben (@0x8004076c-78 fasst nur rec[0]/rec[1] an); der Port haelt das Tuer-Band in
     * door_params (op_door_aot_set: pc[4]), der Text-Platz liest a->band — also hier uebertragen.
     * Der Text-Handler (@0x80043084, Port re15_scd_show_message) oeffnet nur die Nachricht — kein
     * Raumwechsel, keine Tuersequenz (die haengt an aot_fire_door). */
    g_aot.slots[RE15_TUER1120_SLOT].band = g_aot.door_params[RE15_TUER1120_SLOT].band;
    re15_aot_retype(RE15_TUER1120_SLOT, RE15_TUER1120_SCE_TEXT, RE15_TUER1120_FLAGS,
                    RE15_TUER1120_MSG_ID, RE15_TUER1120_MASKE, 0);
    s_gesperrt = 1;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[tuer1120] ROOM%04X Slot %d -> Text-Platz msg %d (Flag (%d,%d) = 0)\n",
            (unsigned)room_id, RE15_TUER1120_SLOT, RE15_TUER1120_MSG_ID,
            RE15_TUER1120_FLAG_BANK, RE15_TUER1120_FLAG_BIT);
#endif
}
