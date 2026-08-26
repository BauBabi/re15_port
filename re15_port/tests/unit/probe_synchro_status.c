/* probe_synchro_status.c — ARBEITSLISTE fuer die Sprachaufnahmen.
 *
 * Geht alle ausgelieferten RDTs durch, dekodiert ihre Message-Tabelle MIT DEM DEKODER DES
 * PORTS (re15_msg_load_room_block / re15_msg_get_text — kein Nachbau) und zeigt je Message,
 * ob unter synchro/ schon eine Aufnahme liegt.
 *
 * ABLAGE-REGEL (die der Loader baut, audio_pc.c re15_voice_load_clip):
 *     synchro/STAGE<n>/room<RAUM-ID 4 Hex>/main<MESSAGE-ID 2 Dezimal>.wav
 *
 * Aufruf:
 *     re15_port/build/tests/unit/probe_synchro_status.exe            # nur Raeume mit Luecken
 *     re15_port/build/tests/unit/probe_synchro_status.exe --alle     # alle Raeume, mit Texten
 *     re15_port/build/tests/unit/probe_synchro_status.exe --texte    # Luecken MIT Texten
 * KEIN add_test.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "re15_rdt.h"
#include "re15_msg.h"

static uint8_t *slurp(const char *path, long *out)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return NULL; }
    fclose(f); *out = sz; return b;
}

static int hat_wav(unsigned stage, unsigned rid, int msg)
{
    char p[512];
    snprintf(p, sizeof p, "%s/STAGE%u/room%04X/main%02d.wav", RE15_SYNCHRO_DIR, stage, rid, msg);
    FILE *f = fopen(p, "rb");
    if (f) { fclose(f); return 1; }
    /* Klein-Hex — derselbe Rueckfall wie im Loader (case-sensitive Dateisysteme). */
    snprintf(p, sizeof p, "%s/STAGE%u/room%04x/main%02d.wav", RE15_SYNCHRO_DIR, stage, rid, msg);
    f = fopen(p, "rb");
    if (f) { fclose(f); return 1; }
    return 0;
}

static re15_rdt_t s_rdt;

int main(int argc, char **argv)
{
    int alle   = (argc > 1 && strcmp(argv[1], "--alle")  == 0);
    int texte  = alle || (argc > 1 && strcmp(argv[1], "--texte") == 0);

    printf("== Sprachaufnahmen: Stand und Luecken ==\n");
    printf("Ablage: synchro/STAGE<n>/room<ID 4 Hex>/main<Message-ID 2 Dezimal>.wav\n\n");

    int g_msgs = 0, g_haben = 0, g_raeume = 0, g_raeume_teil = 0;

    for (unsigned st = 1; st <= 6; st++) {
        char dirp[512];
        snprintf(dirp, sizeof dirp, "%s/STAGE%u", RE15_ASSET_PSX_DIR, st);
        DIR *d = opendir(dirp);
        if (!d) continue;
        /* Namen erst sammeln, damit die Ausgabe sortiert ist. */
        char names[512][32]; int n = 0;
        struct dirent *e;
        while ((e = readdir(d)) != NULL && n < 512) {
            size_t ln = strlen(e->d_name);
            if (ln < 5 || strcmp(e->d_name + ln - 4, ".RDT") != 0) continue;
            snprintf(names[n++], sizeof names[0], "%s", e->d_name);
        }
        closedir(d);
        for (int i = 0; i < n; i++)
            for (int j = i + 1; j < n; j++)
                if (strcmp(names[i], names[j]) > 0) {
                    char t[32]; snprintf(t, sizeof t, "%s", names[i]);
                    snprintf(names[i], sizeof names[0], "%s", names[j]);
                    snprintf(names[j], sizeof names[0], "%s", t);
                }

        for (int i = 0; i < n; i++) {
            char fp[1024];
            snprintf(fp, sizeof fp, "%s/%s", dirp, names[i]);
            long sz = 0;
            uint8_t *b = slurp(fp, &sz);
            if (!b || sz < 0x48) { free(b); continue; }
            if (re15_rdt_parse(b, (size_t)sz, &s_rdt) != 0) { free(b); continue; }
            unsigned rid = 0;
            sscanf(names[i] + 4, "%4x", &rid);

            re15_msg_clear_room_block();
            re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);

            int msgs = 0, haben = 0;
            for (int m = 0; m < 64; m++) {
                const char *t = re15_msg_get_text(m);
                if (!t || !t[0]) continue;
                msgs++;
                if (hat_wav(st, rid, m)) haben++;
            }
            if (msgs == 0) { free(b); continue; }
            g_raeume++; g_msgs += msgs; g_haben += haben;
            if (haben > 0 && haben < msgs) g_raeume_teil++;

            if (!alle && haben == 0) { free(b); continue; }   /* voellig unvertonte Raeume kuerzen */

            printf("%-14s STAGE%u room%04X  %2d/%2d vertont%s\n",
                   names[i], st, rid, haben, msgs,
                   haben == msgs ? "  ✓ vollstaendig" : "");
            if (haben < msgs) {
                for (int m = 0; m < 64; m++) {
                    const char *t = re15_msg_get_text(m);
                    if (!t || !t[0]) continue;
                    if (hat_wav(st, rid, m)) continue;
                    if (texte) printf("    fehlt: main%02d.wav   \"%s\"\n", m, t);
                    else       printf("    fehlt: main%02d.wav\n", m);
                }
            }
            free(b);
        }
    }
    printf("\n== Summe: %d Messages in %d Raeumen mit Text, %d vertont (%d Raeume teilweise) ==\n",
           g_msgs, g_raeume, g_haben, g_raeume_teil);
    if (!alle)
        printf("(nur Raeume mit mindestens einer Aufnahme; --alle zeigt jeden Raum, "
               "--texte zeigt die fehlenden Zeilen im Wortlaut)\n");
    return 0;
}
