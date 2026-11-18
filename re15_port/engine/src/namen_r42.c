/*
 * RE1.5 Rebuilt — Runde 42 Strang "namen", Stufe 2: Mod-Fortsetzungszeilen 10D0/1150/2000 am Satzende der Aufnahme geteilt, jeder
 * Untertitel mit Sprechernamen. Belege: include/re15_namen_r42.h, Texte gen/namen_r42_texte.inc (tools/r42_namen/fortsetzung_bauen.py).
 * PORT-TEXT, kein Asset-Patch: die RDTs bleiben bytegleich.
 */
#include <stdint.h>
#include <stdio.h>

#include "re15_namen_r42.h"
#include "re15_msg.h"
#include "re15_engine.h"

#include "gen/namen_r42_texte.inc"

const uint8_t *re15_namen_r42_text(uint16_t room_id, uint8_t id, int *out_len)
{
    for (int i = 0; i < NM_TEXTE_N; i++)
        if (k_nm_texte[i].raum == room_id && k_nm_texte[i].id == id) {
            if (out_len) *out_len = k_nm_texte[i].n;
            return k_nm_texte[i].b;
        }
    if (out_len) *out_len = 0;
    return NULL;
}

void re15_namen_r42_texte(uint16_t room_id)
{
    int n = 0;
    for (int i = 0; i < NM_TEXTE_N; i++)
        if (k_nm_texte[i].raum == room_id) {
            re15_msg_install_text(k_nm_texte[i].id, k_nm_texte[i].b, (size_t)k_nm_texte[i].n);
            const int d = re15_msg_compute_duration(k_nm_texte[i].b, k_nm_texte[i].n, 0);   /* wie re15_msg_load_room_block */
            if (d > 0 && d < 65535) re15_msg_install_durations(k_nm_texte[i].id, d);
            n++;
        }
    if (n) fprintf(stderr, "[namen] F%u ROOM%04X: %d Zeilen am Satzende der Aufnahme geteilt (nach den RDT-Texten)\n",
                   (unsigned)g_engine.frame_count, (unsigned)room_id, n);
}
