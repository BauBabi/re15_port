/* probe_1210_masken.c — MESSUNG zum zweiten Teil des Nutzer-Reports 2026-08-31:
 *   "Ausserdem clippen sie teilweise vollstaendig durch die Wand ... es soll lediglich
 *    der Obere Teil bei den Gitterstaeben 'Durchsichtig' sein."
 *
 * Die Geometrie ist bereits gemessen (probe_1210_wandgriff): die Arme verlassen die Wand
 * NIE — ihr Rumpf endet 1744 Einheiten VOR der begehbaren Flurkante, nur das Mesh ragt
 * 1671 Einheiten weiter. Wenn der Spieler trotzdem den ganzen Koerper sieht, ist das kein
 * Positions-, sondern ein VERDECKUNGS-Problem: die Wand muss den Rumpf ueberdecken.
 *
 * Diese Sonde misst deshalb, was ROOM1210 an Vordergrund-Masken (sprite.pri) mitbringt:
 * wie viele Kamera-Schnitte es gibt, welche davon ueberhaupt eine Masken-Sektion haben,
 * und wie viele Masken mit welcher Tiefe darin stehen. Sie behauptet nichts.
 */
#include "re15_rdt.h"
#include "re15_camera.h"
#include "re15_pri.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static re15_rdt_t s_rdt;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void report(const char *room, const char *stage)
{
    char path[600];
    snprintf(path, sizeof path, "%s/%s/%s.RDT", RE15_ASSET_PSX_DIR, stage, room);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("  %s: nicht lesbar\n", room); return; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("  %s: Parse-Fehler\n", room); free(buf); return; }

    int ncuts = (int)s_rdt.cut_count;
    int mit = 0, ohne = 0, masken = 0;
    printf("  %s: %d Kamera-Schnitte\n", room, ncuts);
    for (int c = 0; c < ncuts; c++) {
        const re15_camera_cut_t *cu = &s_rdt.cuts[c];
        re15_pri_cut_t pc;
        memset(&pc, 0, sizeof pc);
        int n = 0;
        if (cu->pri_offset != 0xFFFFFFFFu && cu->pri_offset != 0)
            n = re15_pri_parse_section(s_rdt.raw, sz, cu->pri_offset, &pc);
        if (n > 0) { mit++; masken += n; } else ohne++;
        (void)ohne;
        printf("     cut %2d pri_offset 0x%08X -> %2d Masken", c, (unsigned)cu->pri_offset, n);
        for (int m = 0; m < n && m < 8; m++)
            printf(" [d=%d]", pc.masks[m].depth);
        printf("\n");
    }
    printf("     SUMME: %d von %d Schnitten mit Masken, %d Masken gesamt\n",
           mit, ncuts, masken);
    free(buf);
}

int main(void)
{
    printf("=== Vordergrund-Masken (sprite.pri) ===\n");
    printf("-- der gemeldete Raum --\n");
    report("ROOM1210", "STAGE1");
    printf("-- Vergleichsraeume (haben die ueberhaupt welche?) --\n");
    report("ROOM1240", "STAGE1");   /* Startraum */
    report("ROOM1170", "STAGE1");   /* Helipad, dort ist eine Masken-Lampe belegt */
    report("ROOM1090", "STAGE1");
    report("ROOM1211", "STAGE1");
    return 0;
}
