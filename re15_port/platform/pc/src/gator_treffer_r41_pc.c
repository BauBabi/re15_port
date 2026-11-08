/*
 * gator_treffer_r41_pc.c - Runde 41 Spur G2-RE2 NB4 (T): PC-Leser der RE2-Raumbank fuer die Trefferreaktion der Figur
 * (engine/src/gator_treffer_r41.c, include/re15_gator_treffer_r41.h).
 *
 * shared_assets/RE2/GATOR40A0.RBJ = RE2 ROOM40A0.RDT @0x65E8 (RDT+0x60, 14328 B), bytegleich ausgeschnitten mit
 * tools/r41_g2nb4/gator40a0_rbj_export.py. RE2 bindet diese Bank als PL+0x190/+0x194 (FUN_80018e08 @0x80018ea4/@0x80018eb8), ein eigener
 * Kanal neben der Basisbank - darum hier kein Raum-RBJ (main.c rbj_buf) fuer 2090/2091, sondern nur dieser Zeiger fuer den Zeichner.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "re15_gator_treffer_r41.h"
#include "asset_root_pc.h"   /* re15_pc_read_re2 */

static uint8_t *s_rbj = NULL;
static int      s_rbj_n = 0, s_versucht = 0;

static const uint8_t *rbj_40a0(int *size)
{
    if (!s_versucht) {
        s_versucht = 1;
        s_rbj = re15_pc_read_re2("GATOR40A0.RBJ", &s_rbj_n);
        if (!s_rbj || s_rbj_n <= 0) { free(s_rbj); s_rbj = NULL; s_rbj_n = 0; }
        fprintf(stderr, "[rbj] Runde 41 G2-RE2 NB4: RE2-Raumbank ROOM40A0 (GATOR40A0.RBJ) %s (%d B) - Trefferreaktion Clips 17-20\n",
                s_rbj ? "gelesen" : "FEHLT -> Reaktion posiert ohne Raumbank", s_rbj_n);
    }
    if (size) *size = s_rbj_n;
    return s_rbj;   /* bleibt resident */
}

void re15_pc_gator_treffer_r41_init(void) { re15_gator_treffer_r41_rbj_hook = rbj_40a0; }
