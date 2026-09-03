/* test_sld_atlas.c — Regressionstest fuer den Laufzeit-Vordergrundatlas (2026-09-03).
 *
 * WAS HIER GEPRUEFT WIRD
 * Der Port zog den sprite.pri-Vordergrundatlas bis 2026-09-03 aus vorextrahierten
 * Dateien BSS/ROOM####/PRI%02d.TIM. Die gab es nur fuer 209 der 359 Cuts, die im
 * Original einen Vordergrund tragen — fuer die uebrigen fiel die Verdeckung
 * ersatzlos aus (188 Cuts mit 6530 Maskenrecords, ROOM1020/1030/1040/1070 und
 * STAGE5 komplett). Jetzt kommt der Atlas aus dem BSS-Chunk selbst.
 *
 * Der Test faehrt genau den Weg des Originals (FUN_80021bbc, s. re15_sld.h):
 *   L        = u16 STAGE<n>.BIN[tabelle + raum*0x20 + cut*2]
 *   present  = u32 chunk[L-4]        (@0x80021d6c/74)
 *   sld_off  = u32 chunk[L-8]        (@0x80021da4)
 *   Atlas    = FUN_800c47e8(chunk + sld_off + 4, u32 chunk[sld_off])
 *
 * ORAKEL: Wo eine PRI##.TIM existiert, MUSS der Laufzeit-Auszug byte-identisch
 * sein. Das ist kein Selbstvergleich — die Dateien stammen aus dem unabhaengigen
 * Java-Extraktor (SldDecoder.java), der Laufzeitpfad aus der Disassembly.
 *
 * Erwartungswerte (gemessen ueber alle 6 Stages / 120 BSS-Dateien / 1119 Chunks):
 *   359 Cuts mit Vordergrund, 735 ohne, 0 Dekodierfehler.
 * Der Test prueft die 359 als UNTERGRENZE und verlangt 0 Fehler — eine kaputte
 * Assetkopie faellt damit auf, ohne dass eine spaetere Asset-Erweiterung ihn bricht.
 */
#include "re15_sld.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

#define CHUNK_SIZE 0x10000

static uint8_t *slurp(const char *p, long *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *n = sz;
    return b;
}

static uint8_t s_atlas[RE15_SLD_MAX_UNPACKED];

int main(void)
{
    int total_fg = 0, total_none = 0, fails = 0, compared = 0, mismatch = 0;
    int rooms_seen = 0;

    for (int stage = 1; stage <= 6; stage++) {
        char p[600];
        long sbsz = 0;
        snprintf(p, sizeof p, "%s/BIN/STAGE%d.BIN", RE15_ASSET_PSX_DIR, stage);
        uint8_t *sb = slurp(p, &sbsz);
        if (!sb) { printf("  STAGE%d.BIN nicht lesbar (%s)\n", stage, p); fails++; continue; }

        /* Raumnummern je Stage: ROOM<stage><rr>.BSS, rr = 0x00..0xFF. Nicht
         * vorhandene Dateien werden uebersprungen (die Stages sind unterschiedlich
         * gross: 40/16/16/16/24/8 BSS-Dateien, 120 insgesamt). */
        for (int rr = 0; rr <= 0xFF; rr++) {
            int room_index = rr;
            long bsz = 0;
            snprintf(p, sizeof p, "%s/STAGE%d/ROOM%X%02X.BSS",
                     RE15_ASSET_PSX_DIR, stage, stage, rr);
            uint8_t *bss = slurp(p, &bsz);
            if (!bss) continue;
            rooms_seen++;

            int chunks = (int)(bsz / CHUNK_SIZE);
            if (chunks > RE15_SLD_TBL_COLS) chunks = RE15_SLD_TBL_COLS;

            for (int cut = 0; cut < chunks; cut++) {
                uint16_t L = 0;
                /* DRIFTSCHUTZ: die erzeugte Tabelle (gen/sld_lentab.inc) ist eine KOPIE
                 * der Overlay-Daten, damit die PSX nicht 137 KB fuer 2 Byte von CD holen
                 * muss. Eine Kopie von Spieldaten darf nie unbemerkt auseinanderlaufen —
                 * hier wird sie gegen die Originaldatei geprueft. */
                {
                    uint16_t Lt = 0;
                    if (re15_sld_used_len_tab(stage, room_index, cut, &Lt) == RE15_SLD_OK) {
                        uint16_t Lf = 0;
                        if (re15_sld_used_len(sb, (int)sbsz, stage, room_index, cut, &Lf)
                                == RE15_SLD_OK && Lt != Lf) {
                            printf("  TABELLENDRIFT STAGE%d ROOM%X%02X cut%d: "
                                   "erzeugt 0x%04X, Datei 0x%04X\n",
                                   stage, stage, rr, cut, Lt, Lf);
                            fails++;
                        }
                    }
                }
                if (re15_sld_used_len(sb, (int)sbsz, stage, room_index, cut, &L)
                        != RE15_SLD_OK) {
                    printf("  STAGE%d ROOM%X%02X cut%d: Tabellenzeile unlesbar\n",
                           stage, stage, rr, cut);
                    fails++;
                    continue;
                }

                const uint8_t *chunk = bss + (long)cut * CHUNK_SIZE;
                int len = 0;
                int rv  = re15_sld_atlas_from_chunk(chunk, CHUNK_SIZE, L,
                                                    s_atlas, (int)sizeof s_atlas, &len);
                if (rv == RE15_SLD_NO_FOREGROUND) { total_none++; continue; }
                if (rv != RE15_SLD_OK) {
                    printf("  STAGE%d ROOM%X%02X cut%d: L=0x%04X rv=%d\n",
                           stage, stage, rr, cut, L, rv);
                    fails++;
                    continue;
                }
                total_fg++;

                /* Sony-TIM-Magic 10 00 00 00 (`out[0]==0x10` ist die Pruefung,
                 * die auch der Java-Extraktor als Gueltigkeitstest fuehrt). */
                if (!(len >= 8 && s_atlas[0] == 0x10 && s_atlas[1] == 0
                      && s_atlas[2] == 0 && s_atlas[3] == 0)) {
                    printf("  STAGE%d ROOM%X%02X cut%d: kein TIM-Magic\n",
                           stage, stage, rr, cut);
                    fails++;
                    continue;
                }

                /* ORAKEL: gegen die vorextrahierte Datei beider Spielervarianten. */
                for (int var = 0; var <= 1; var++) {
                    long fsz = 0;
                    snprintf(p, sizeof p, "%s/BSS/ROOM%X%02X%d/PRI%02d.TIM",
                             RE15_ASSET_PSX_DIR, stage, rr, var, cut);
                    uint8_t *ref = slurp(p, &fsz);
                    if (!ref) continue;
                    compared++;
                    if (fsz != len || memcmp(ref, s_atlas, (size_t)len) != 0) {
                        printf("  ABWEICHUNG %s: Datei %ld B, Laufzeit %d B\n",
                               p, fsz, len);
                        mismatch++;
                    }
                    free(ref);
                }
            }
            free(bss);
        }
        free(sb);
    }

    printf("  BSS-Dateien: %d | Vordergrund: %d | ohne: %d | Dekodierfehler: %d\n",
           rooms_seen, total_fg, total_none, fails);
    printf("  gegen vorextrahierte PRI##.TIM verglichen: %d | Abweichungen: %d\n",
           compared, mismatch);

    if (rooms_seen == 0) {
        printf("  UEBERSPRUNGEN: keine BSS-Assets unter %s\n", RE15_ASSET_PSX_DIR);
        return 0;                      /* kein falsches Gruen, aber auch kein Fehlalarm */
    }
    if (fails)    { printf("  FAIL: %d Dekodierfehler\n", fails); return 1; }
    if (mismatch) { printf("  FAIL: %d Atlanten weichen vom Extraktor ab\n", mismatch); return 1; }
    if (total_fg < 359) {
        printf("  FAIL: nur %d Vordergrund-Cuts gefunden, erwartet >= 359\n", total_fg);
        return 1;
    }
    if (compared < 200) {
        printf("  FAIL: nur %d Orakel-Vergleiche — die Referenzdateien fehlen\n", compared);
        return 1;
    }
    printf("  OK\n");
    return 0;
}
