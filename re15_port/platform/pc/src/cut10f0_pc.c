/* cut10f0_pc.c — PC-Seite der Szene ROOM10F0 (Runde 35, Spur K): die Gestenblock-Leihe.
 *
 * Herleitung und Belege: include/re15_cut10f0.h, analysis/befunde_runde35/K_cut10f0.md. Zustand, Programm
 * und Weichen liegen plattformfrei in engine/src/cut_10f0.c; hier nur das Lesen der fremden RDT.
 * (Nachbesserung 1: aus platform/pc/main.c hierher verlegt — VERTRAG §1.4, in main.c bleiben Haken.)
 *
 * ROOM10F0 hat keinen Animationsblock (RDT+0x5C = 0). Solange die Szene aussteht, liefert
 * re15_cut10f0_rbj_quelle den Raum, dessen Block (RDT @0x5C) zu leihen ist: ROOM11B0 (RDT @0x1CB0,
 * 48168 B; Record 0 = Leons Gestenbibliothek, Record 1 = die NPC-Bibliothek, Clips 15..24).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "re15_cut10f0.h"
#include "re15_rdt.h"
#include "asset_root_pc.h"   /* re15_pc_read_any: dieselbe Wurzelliste wie main.c pc_read_shared */
#include "re15_entladen.h"    /* Runde 35 Spur I (N2): Generation + Freigabe an jeder Grenze */

/* Den Animationsblock leihen, den die ausstehende Szene im Raum room_id braucht. Rueckgabe: Zeiger IN
 * den residenten Dateipuffer der Quell-RDT (bleibt bis zur naechsten Leihe gueltig; der Aufrufer behandelt
 * ihn wie einen RDT-Alias — rbj_borrowed = 1, nie free) und *size, oder NULL (keine Leihe noetig/moeglich). */
/* Runde 35 Spur I (N2): Leihe auf Dateiebene, damit jede Grenze (raum/spielstart/spielende) sie freigibt —
 * vorher lebte die ROOM11B0-RDT bis zur naechsten Leihe weiter (Original: Block = RDT+0x5C @0x8001b404 der
 * NEUEN RDT, Arena-Reset @0x80039738). */
static uint8_t   *s_leih_buf = NULL;
static re15_rdt_t s_leih_rdt;
static unsigned   s_leih_gen = 0;
void re15_cut10f0_pc_rbj_freigeben(void) { free(s_leih_buf); s_leih_buf = NULL; s_leih_rdt.animation = NULL; }
int  re15_cut10f0_pc_rbj_belegt(unsigned *gen) { if (gen) *gen = s_leih_gen; return s_leih_buf != NULL; }

uint8_t *re15_cut10f0_pc_rbj_leihen(unsigned room_id, int *size)
{
    const unsigned quelle = re15_cut10f0_rbj_quelle(room_id);
    char rel[48];
    int n = 0;
    if (!quelle || !size) return NULL;                 /* *size bleibt dann unberuehrt */
    snprintf(rel, sizeof rel, "STAGE%u/ROOM%04X.RDT", (quelle >> 12) & 0xFu, quelle);
    uint8_t *b = re15_pc_read_any(rel, &n);
    if (!b) return NULL;
    if (re15_rdt_parse(b, (size_t)n, &s_leih_rdt) != 0 || !s_leih_rdt.animation ||
        s_leih_rdt.animation_size <= 0) { free(b); return NULL; }
    free(s_leih_buf);
    s_leih_buf = b;
    s_leih_gen = g_re15_entladen_gen;   /* Runde 35 Spur I (N2) */
    *size = s_leih_rdt.animation_size;
    fprintf(stderr, "[rbj] Animationsblock von ROOM%04X geliehen (%d B, Runde 35 Spur K)\n", quelle, *size);
    return (uint8_t *)s_leih_rdt.animation;
}
