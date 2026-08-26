/* test_stage6_hintergrund.c — PIN: JEDER Raum hat einen erreichbaren Hintergrund.
 *
 * NUTZER-REPORT 2026-08-26: "Die Stage 6 Sachen laedt er irgendwie Raum und Hintergrund
 * nicht, wenn ich den im Debug Mode anwaehle."
 *
 * GEMESSENE URSACHE: der vorgeschnittene Baum BSS/ROOM####/BG##.BSS ist unvollstaendig.
 * Verzeichnisse je Stage: 1 -> 78, 2 -> 24, 3 -> 30, 4 -> 22, 5 -> 2, 6 -> NULL.
 * Der BG-Lader kannte nur diesen Pfad (bg_pc.c) — fuer STAGE6 gab es also nichts zu laden.
 * Die ORIGINALE liegen dagegen fuer alle Stages vollstaendig als STAGE%u/ROOM%03X.BSS da
 * (eine Datei je Raum-PAAR, 40+16+16+16+24+8 = 120 Dateien fuer 240 RDTs).
 *
 * SCHNITTREGEL, gemessen: Schnitt n = Original[n * 0x10000], Laenge 0x10000. Gegenprobe
 * ueber den gesamten vorhandenen Baum: 1688 Schnitte, ALLE byte-identisch, 0 Abweichungen.
 *
 * DIESE WACHE prueft die DATENLAGE, auf der der Rueckfall aufsetzt — und zwar fuer ALLE
 * Stages, nicht nur fuer die gemeldete:
 *   (1) jedes RDT laesst sich parsen und meldet seine Zahl der Kamera-Schnitte,
 *   (2) fuer JEDEN Schnitt ist eine Quelle erreichbar: entweder der vorgeschnittene
 *       BG##.BSS oder die Scheibe cut*0x10000 in der Original-BSS,
 *   (3) wo BEIDE da sind, muessen sie byte-gleich sein — sonst waere die Schnittregel
 *       falsch und der Rueckfall wuerde ein anderes Bild zeigen als der Baum.
 * Nicht vakuant: sie zaehlt die tatsaechlich geprueften Schnitte und faellt durch, wenn es
 * zu wenige sind.
 */
#include "re15_rdt.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

#define BSS_STRIDE 0x10000u

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  PASS: " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, long *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = sz; return b;
}

int main(void)
{
    printf("=== Hintergruende: hat jeder Raum eine Quelle? ===\n");

    int rooms = 0, cuts_total = 0, cuts_ok = 0, cuts_missing = 0;
    int both = 0, both_diff = 0;
    int stage_rooms[7] = {0}, stage_missing[7] = {0}, stage_stubs[7] = {0};
    int stubs = 0;

    for (int stage = 1; stage <= 6; stage++) {
        for (int idx = 0; idx <= 0x27; idx++) {
            for (int var = 0; var <= 1; var++) {
                unsigned rid = ((unsigned)stage << 12) | ((unsigned)idx << 4) | (unsigned)var;
                char path[512];
                snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE%d/ROOM%04X.RDT", stage, rid);
                long rsz = 0;
                uint8_t *rbuf = slurp(path, &rsz);
                if (!rbuf) continue;
                /* ⛔ STUMMEL: 34 der 240 RDTs sind 4-Byte-Platzhalter (Inhalt 00 00 00 00) —
                 * Raeume, die es im Auslieferungsstand NICHT gibt. Gemessen je Stage:
                 * 1 -> 2 (ROOM1270/1271), 2 -> 8 (20C0..20F1), 3 -> 2 (30F0/30F1),
                 * 4 -> 10 (4060/4061, 40C0..40F1), 5 -> 6 (5150..5171),
                 * 6 -> 6 (ROOM6050/6051, 6060/6061, 6070/6071).
                 * Das ist KEIN Fehler, sondern der Auslieferungsstand. Die Wache zaehlt sie,
                 * damit ein spaeterer Asset-Verlust trotzdem auffaellt. */
                if (rsz < 0x40) { stubs++; stage_stubs[stage]++; free(rbuf); continue; }
                re15_rdt_t rdt;
                if (re15_rdt_parse(rbuf, (size_t)rsz, &rdt) != 0) {
                    printf("     ROOM%04X: RDT-Parse gescheitert (%ld B)\n", rid, rsz);
                    free(rbuf); fails++; continue;
                }
                int ncut = rdt.cut_count;
                rooms++; stage_rooms[stage]++;

                /* Original-BSS des Raum-PAARES */
                char obss[512];
                snprintf(obss, sizeof obss, RE15_ASSET_PSX_DIR "/STAGE%d/ROOM%03X.BSS",
                         stage, rid >> 4);
                long osz = 0;
                uint8_t *obuf = slurp(obss, &osz);

                for (int c = 0; c < ncut; c++) {
                    cuts_total++;
                    char tpath[512];
                    snprintf(tpath, sizeof tpath, RE15_ASSET_PSX_DIR "/BSS/ROOM%04X/BG%02d.BSS",
                             rid, c);
                    long tsz = 0;
                    uint8_t *tbuf = slurp(tpath, &tsz);
                    size_t off = (size_t)c * BSS_STRIDE;
                    int have_slice = (obuf && off + BSS_STRIDE <= (size_t)osz);

                    if (tbuf || have_slice) cuts_ok++;
                    else { cuts_missing++; stage_missing[stage]++; }

                    if (tbuf && have_slice) {
                        both++;
                        size_t n = (size_t)tsz < BSS_STRIDE ? (size_t)tsz : BSS_STRIDE;
                        if (memcmp(tbuf, obuf + off, n) != 0) {
                            both_diff++;
                            if (both_diff <= 3)
                                printf("     ROOM%04X Schnitt %d: Baum != Original-Scheibe\n", rid, c);
                        }
                    }
                    if (tbuf) free(tbuf);
                }
                if (obuf) free(obuf);
                free(rbuf);
            }
        }
    }

    printf("  %d Raeume, %d Kamera-Schnitte insgesamt\n", rooms, cuts_total);
    for (int s = 1; s <= 6; s++)
        printf("     STAGE%d: %d Raeume, %d Schnitte ohne Quelle\n", s, stage_rooms[s], stage_missing[s]);
    printf("  Schnitte mit Quelle: %d, ohne: %d; in BEIDEN Formen vorhanden: %d (davon abweichend: %d)\n",
           cuts_ok, cuts_missing, both, both_diff);

    CHECK(rooms >= 200,
          "es wurden ueberhaupt genug Raeume geprueft (%d) - sonst misst die Wache nichts",
          rooms);
    printf("  4-Byte-Stummel insgesamt: %d (STAGE6: %d)\n", stubs, stage_stubs[6]);
    CHECK(stage_rooms[6] == 10 && stage_stubs[6] == 6,
          "STAGE6 hat GENAU 10 echte Raeume und 6 Stummel (gemessen %d / %d) - die drei "
          "Stummel-Paare ROOM6050/6060/6070 sind der zweite Teil des Nutzer-Reports: das "
          "Debug-Menue bietet ACHT STAGE6-Eintraege an, aber drei davon zeigen auf Raeume, "
          "die es im Auslieferungsstand nicht gibt",
          stage_rooms[6], stage_stubs[6]);
    CHECK(stubs == 34,
          "und game-weit sind es 34 Stummel (%d) - faellt die Zahl, ist ein RDT verloren "
          "gegangen; steigt sie, wurde ein echter Raum ueberschrieben", stubs);
    CHECK(cuts_missing == 0,
          "JEDER der %d Kamera-Schnitte hat eine Quelle (%d ohne) - entweder den "
          "vorgeschnittenen BG##.BSS oder die Scheibe cut*0x10000 der Original-BSS",
          cuts_total, cuts_missing);
    CHECK(both > 1000,
          "und %d Schnitte liegen in BEIDEN Formen vor - genug, um die Schnittregel "
          "zu pruefen", both);
    CHECK(both_diff == 0,
          "wo beide Formen da sind, sind sie BYTE-GLEICH (%d Abweichungen) - damit zeigt "
          "der Rueckfall dasselbe Bild wie der Baum", both_diff);

    if (fails) { printf("\nSTAGE6-BG: FAIL (%d)\n", fails); return 1; }
    printf("\nSTAGE6-BG: jeder Raum jeder Stage hat einen erreichbaren Hintergrund\n");
    return 0;
}
