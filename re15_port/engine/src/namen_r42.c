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
#include "gen/zweizeilig_r42_texte.inc"   /* Stufe 5: RDT-Seiten mit drei Zeilen -> hoechstens zwei (tools/r42_namen/zweizeilig_s5.py) */

const uint8_t *re15_namen_r42_text(uint16_t room_id, uint8_t id, int *out_len)
{
    for (int i = 0; i < NM_TEXTE_N; i++)
        if (k_nm_texte[i].raum == room_id && k_nm_texte[i].id == id) {
            if (out_len) *out_len = k_nm_texte[i].n;
            return k_nm_texte[i].b;
        }
    for (int i = 0; i < ZZ_TEXTE_N; i++)
        if (k_zz_texte[i].raum == room_id && k_zz_texte[i].id == id) {
            if (out_len) *out_len = k_zz_texte[i].n;
            return k_zz_texte[i].b;
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
    /* Stufe 5: jede Seite hoechstens zwei Zeilen wie die echte 40%-Disc (Zeilenwechsel y = 180 + 16 fest @0x800289a0/@0x800289ac,
     * 0 von 700 Seiten mit drei Zeilen; Letterbox-Balken ab y 216 @0x80072f06). Dieselbe Form wie oben: Text + Dauer. */
    int z = 0;
    for (int i = 0; i < ZZ_TEXTE_N; i++)
        if (k_zz_texte[i].raum == room_id) {
            re15_msg_install_text(k_zz_texte[i].id, k_zz_texte[i].b, (size_t)k_zz_texte[i].n);
            const int d = re15_msg_compute_duration(k_zz_texte[i].b, k_zz_texte[i].n, 0);
            if (d > 0 && d < 65535) re15_msg_install_durations(k_zz_texte[i].id, d);
            z++;
        }
    if (z) fprintf(stderr, "[namen] F%u ROOM%04X: %d Nachrichten mit drei Zeilen auf hoechstens zwei je Seite verteilt (Stufe 5)\n",
                   (unsigned)g_engine.frame_count, (unsigned)room_id, z);
}
