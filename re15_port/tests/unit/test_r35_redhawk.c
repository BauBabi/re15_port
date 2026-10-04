/*
 * test_r35_redhawk.c — Runde 35 Spur D "redhawk".
 *
 * Nutzer (AUFTRAG.md Z.17): "Wenn ich mit der Super Redhawk auf die Hunde schiesse bleiben die
 * Fleisch Effekte die sich rausloesen permanent da in loop."
 *
 * Dossier: analysis/befunde_runde35/D_redhawk.md.
 *
 * Aufruf: test_r35_redhawk [teil]
 *   zensus   Routinen-Zensus aller Raum-ESP-Baenke (STAGE1..6): je (Raum, Effekt-Id, Sub, Stream,
 *            Zeile) die Routinen-Waehler A (+0x00) und B (+0x02), Flags (+0x0e), +0x16, +0x1e,
 *            +0x26 und die Anim-Records (Dauer/Schleife). Reines Messwerkzeug, immer 0.
 */
#include "re15_esp.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static uint32_t u32le(const uint8_t *b, size_t o)
{
    return (uint32_t)b[o] | ((uint32_t)b[o + 1] << 8) | ((uint32_t)b[o + 2] << 16) | ((uint32_t)b[o + 3] << 24);
}
static uint16_t u16le(const uint8_t *b, int o) { return (uint16_t)(b[o] | (b[o + 1] << 8)); }

/* Raum-ESP parsen wie pc_load_room_esp (platform/pc/main.c): RDT+0x4C/50/54/58. */
static int room_esp(const uint8_t *rdt, size_t n, re15_esp_t *out)
{
    if (n < 0x5C) return -1;
    return re15_esp_parse(rdt, n, u32le(rdt, 0x4C), u32le(rdt, 0x50), u32le(rdt, 0x54),
                          u32le(rdt, 0x58), out);
}

/* ===================================== ZENSUS ============================================ */
static void zensus_bank(const char *tag, const re15_esp_t *esp, unsigned seenA[64], unsigned seenB[64])
{
    for (int ei = 0; ei < esp->id_count; ei++) {
        const re15_esp_eff_t *e = &esp->eff[ei];
        printf("%s id=%u count_a=%u count_b=%u anim:", tag, e->effect_id, e->count_a, e->count_b);
        for (int k = 0; k < e->count_a && k < 40; k++) {
            re15_esp_anim_t a;
            if (re15_esp_anim(esp, ei, k, &a) != 0) break;
            printf(" [%d d%04x p%04x]", k, a.desc, a.param);
        }
        printf("\n");
        for (int sub = 0; sub < 8; sub++) {
            int ns = re15_esp_row_streams(esp, ei, sub);
            if (ns <= 0 || ns > 32) continue;
            for (int s = 0; s < ns; s++) {
                int nr = 0;
                const uint8_t *r = re15_esp_row_stream(esp, ei, sub, s, &nr);
                if (!r) { printf("%s id=%u sub=%d st=%d: (kein Stream)\n", tag, e->effect_id, sub, s); continue; }
                for (int k = 0; k < nr && k < 32; k++) {
                    const uint8_t *row = r + k * 40;
                    unsigned A = u16le(row, 0x00), B = u16le(row, 0x02);
                    if (A < 64) seenA[A]++;
                    if (B < 64) seenB[B]++;
                    printf("%s id=%u sub=%d st=%d/%d row=%d/%d A=%u B=%u acc=(%d,%d,%d) f0e=%02x vel=(%d,%d,%d)"
                           " p16=%u p1e=%u g26=%u\n",
                           tag, e->effect_id, sub, s, ns, k, nr, A, B,
                           (int16_t)u16le(row, 0x08), (int16_t)u16le(row, 0x0a), (int16_t)u16le(row, 0x0c),
                           row[0x0e],
                           (int16_t)u16le(row, 0x10), (int16_t)u16le(row, 0x12), (int16_t)u16le(row, 0x14),
                           u16le(row, 0x16), u16le(row, 0x1e), u16le(row, 0x26));
                }
            }
        }
    }
}

static int teil_zensus(void)
{
    static unsigned seenA[64], seenB[64];
    memset(seenA, 0, sizeof seenA); memset(seenB, 0, sizeof seenB);
    int rooms = 0;
    for (int st = 1; st <= 6; st++) {
        for (int r = 0; r < 0x100; r++) {
            char path[512];
            int id = (st << 12) | (r << 4);                  /* ROOMs000..sFF0 */
            snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE%d/ROOM%04X.RDT", st, id);
            size_t n = 0; uint8_t *b = slurp(path, &n);
            if (!b) continue;
            re15_esp_t esp; memset(&esp, 0, sizeof esp);
            if (room_esp(b, n, &esp) == 0) {
                char tag[32]; snprintf(tag, sizeof tag, "R%04X", id);
                printf("%s ids:", tag);
                for (int ei = 0; ei < esp.id_count; ei++) printf(" %u", esp.eff[ei].effect_id);
                printf("\n");
                zensus_bank(tag, &esp, seenA, seenB);
                rooms++;
            }
            free(b);
        }
    }
    printf("ZENSUS %d Raeume mit ESP. Routinen A:", rooms);
    for (int i = 0; i < 64; i++) if (seenA[i]) printf(" %d(%u)", i, seenA[i]);
    printf("\nZENSUS Routinen B:");
    for (int i = 0; i < 64; i++) if (seenB[i]) printf(" %d(%u)", i, seenB[i]);
    printf("\n");
    return 0;
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "zensus")) return teil_zensus();
    printf("unbekannter Teil %s\n", teil);
    return 2;
}
