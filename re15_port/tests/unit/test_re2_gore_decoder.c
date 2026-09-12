/* test_re2_gore_decoder.c — FIXTURE fuer den gepackten Effekt-Id-DEKODER (Runde 4,
 * analysis/befunde_runde4_2026-09-12/gore-vollausbau.md §1/§2.1, Punkt 7ii/iii).
 *
 * Beide Engines packen identisch (RE2 FUN_8001bf10.c:23-28 == RE1.5 FUN_80019700.c:23-28):
 *   id = packed>>24, sub = (packed>>16)&0xFF, scale16 = packed&0xFFFF.
 * Uebersetzungs-SOLL (jede Zeile traegt ihren Beleg im Dossier):
 *   (8,0,7000)  0x08001B58 -> RAUM-Id 5, Sub 0   (byte-identischer Sprite-Body §1.2)
 *   (9,2,2000)              -> RAUM-Id 7, Sub 2   (byte-identisch §1.2)
 *   (0,2,8000)  0x00021F40 -> GLOBAL-Id 0, Sub 2 (FONTAENE, CORE00; ROOM1140 traegt kein Id 0)
 *   (5,3,10000) 0x05032710 -> GLOBAL-Id 8, Sub 3 (FEUER; Klassen-Zuordnung §2.1)
 *   (4,15,6000) 0x040F1770 -> GLOBAL-Id 0, Sub 3 (Einzel-Klecks; RE1.5 hat kein Aetz-Sheet)
 *   (10,0,4096) 0x0A001000 -> Id 0, Sub 0        (Beiss-Blut)
 *   (6,0,x) / (4,12,x)      -> KEIN Spawn        (Spark Shot, Zeile 14 im Port unerreichbar)
 * Anker: Part-Weltposition + a3-Versatz (92C4-Halsanker: Kopf +300 / -400, §2). Bankfrei
 * faellt re15_enemy_bone_world_pos auf die Aktor-Wurzel zurueck — der Versatz bleibt messbar
 * ueber re15_re2z_last_fx_pos. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_esp.h"

static int g_fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { g_fail = 1; \
    fprintf(stderr, "FAIL: " __VA_ARGS__); fprintf(stderr, "\n"); } } while (0)

void re15_re2z_gore_fx_test(re15_actor_t *e, int part, uint32_t packed, int16_t yaw,
                            int32_t ofs_x, int32_t ofs_y, int32_t ofs_z);
int  re15_re2z_last_fx_part(void);
void re15_re2z_last_fx_pos(int32_t out[3]);

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); *out_sz = sz; return b;
}

static uint32_t rd32(const uint8_t *p, size_t o)
{
    return (uint32_t)p[o] | ((uint32_t)p[o+1] << 8) | ((uint32_t)p[o+2] << 16) | ((uint32_t)p[o+3] << 24);
}

/* Aktive Slots zaehlen (der Pool wird vor jedem Fall zurueckgesetzt; spawn_rows legt
 * einen Slot je Stream an). Erster aktiver Slot in *out. */
static int alle_fx(const re15_esp_fx_t **out)
{
    int n = 0; *out = NULL;
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (!f) continue;
        if (!*out) *out = f;
        n++;
    }
    return n;
}

int main(void)
{
    printf("=== GORE-DEKODER: gepackte RE2-Effekt-Ids -> RE1.5-Baenke ===\n");

    /* Raum-Bank ROOM1140 (ids {5,7} — der Zombie-Raum des Briefings). */
    long rsz = 0;
    uint8_t *rdt = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", &rsz);
    if (!rdt) { fprintf(stderr, "FAIL: ROOM1140.RDT nicht lesbar\n"); return 1; }
    static re15_esp_t esp;
    if (re15_esp_parse(rdt, (size_t)rsz, rd32(rdt, 0x4C), rd32(rdt, 0x50),
                       rd32(rdt, 0x54), rd32(rdt, 0x58), &esp) != 0) {
        fprintf(stderr, "FAIL: ESP-Parse ROOM1140\n"); return 1;
    }
    CHECK(re15_esp_find_id(&esp, 5) >= 0 && re15_esp_find_id(&esp, 7) >= 0,
          "ROOM1140-Bank traegt nicht {5,7}");
    re15_esp_set_room_bank(&esp);

    /* Global-Bank CORE00.ESP (ids {3,8,0,2,4}). */
    long gsz = 0;
    uint8_t *core = slurp(RE15_ASSET_PSX_DIR "/DATA/CORE00.ESP", &gsz);
    if (!core) { fprintf(stderr, "FAIL: CORE00.ESP nicht lesbar\n"); return 1; }
    static re15_esp_t gesp;
    if (re15_esp_parse_global(core, (size_t)gsz, &gesp) != 0) {
        fprintf(stderr, "FAIL: CORE00-Parse\n"); return 1;
    }
    re15_esp_set_global_bank(&gesp);

    re15_actor_init();
    re15_actor_t *z = &g_actors[1];
    memset(z, 0, sizeof *z);
    z->active = 1; z->type = 0x10; z->flags = 1;
    z->x = 1000; z->y = -200; z->z = 2000; z->rot_y = 512;

    struct {
        uint32_t packed; int part; int32_t oy;
        uint8_t want_id, want_sub; uint16_t want_scale;
        const re15_esp_t *want_bank;      /* NULL = KEIN Spawn erwartet */
        const char *name;
    } CASES[] = {
        { 0x08001B58u, 0, -400, 5, 0, 7000, &esp,  "92C4 #2 Fleischbrocken (8->Raum-5)" },
        { 0x09020000u | 2000u, 0, 0, 7, 2, 2000, &esp,  "Burst-Brocken (9->Raum-7)" },
        { 0x00021F40u, 8, -400, 0, 2, 8000, &gesp, "92C4 #3 Fontaene (0,2 -> CORE00)" },
        { 0x05032710u, 0, 200, 8, 3, 10000, &gesp, "Brand (5,3 -> CORE00-FEUER 8,3)" },
        { 0x040F1770u, 0, 0, 0, 3, 6000, &gesp, "Aetz (4,15 -> 0,3 Einzel-Klecks)" },
        { 0x0A001000u, 8, 0, 0, 0, 4096, &gesp, "Beiss-Blut (10 -> 0,0)" },
        { 0x06000000u | 3096u, 0, 0, 0, 0, 0, NULL, "Spark (6,x) -> kein Spawn" },
        { 0x040C1000u, 0, 0, 0, 0, 0, NULL, "Spark (4,12) -> kein Spawn" },
    };

    for (size_t c = 0; c < sizeof CASES / sizeof CASES[0]; c++) {
        re15_esp_fx_reset();
        re15_re2z_gore_fx_test(z, CASES[c].part, CASES[c].packed, (int16_t)z->rot_y,
                               0, CASES[c].oy, 0);
        int n = re15_esp_fx_count();
        const re15_esp_fx_t *f = NULL;
        (void)alle_fx(&f);
        if (!CASES[c].want_bank) {
            CHECK(n == 0, "%s: %d Slots (erwartet 0)", CASES[c].name, n);
            printf("  %-42s -> 0 Slots (SOLL)\n", CASES[c].name);
            continue;
        }
        CHECK(n > 0 && f, "%s: kein Spawn", CASES[c].name);
        if (!f) continue;
        CHECK(f->effect_id == CASES[c].want_id && f->sub_index == CASES[c].want_sub,
              "%s: (id %u, sub %u), erwartet (%u,%u)", CASES[c].name,
              f->effect_id, f->sub_index, CASES[c].want_id, CASES[c].want_sub);
        CHECK(f->scale16 == CASES[c].want_scale, "%s: scale %u != %u",
              CASES[c].name, f->scale16, CASES[c].want_scale);
        CHECK(f->bank == CASES[c].want_bank, "%s: falsche Bank (Room/Global vertauscht)",
              CASES[c].name);
        CHECK(f->eff_idx >= 0, "%s: eff_idx unaufgeloest", CASES[c].name);
        CHECK(f->rows_base != NULL, "%s: kein Row-Programm (spawn_rows-Pfad?)", CASES[c].name);
        /* Anker-Pin (7iii): bankfrei == Aktor-Wurzel + Versatz. */
        int32_t lp[3]; re15_re2z_last_fx_pos(lp);
        CHECK(lp[0] == z->x && lp[1] == z->y + CASES[c].oy && lp[2] == z->z,
              "%s: Anker (%d,%d,%d), erwartet (%d,%d,%d)", CASES[c].name,
              lp[0], lp[1], lp[2], z->x, z->y + CASES[c].oy, z->z);
        CHECK(re15_re2z_last_fx_part() == CASES[c].part, "%s: Anker-Part %d != %d",
              CASES[c].name, re15_re2z_last_fx_part(), CASES[c].part);
        printf("  %-42s -> %d Slot(s) (id %u sub %u scale %u, %s, Anker dy=%d)\n",
               CASES[c].name, n, f->effect_id, f->sub_index, f->scale16,
               f->bank == &esp ? "RAUM" : "GLOBAL", (int)CASES[c].oy);
    }

    /* Raum ohne Ziel-Id: ROOM1190 traegt nur {7} — Id 5 darf NIRGENDS aufloesen
     * (CORE00 hat keine 5; Flammen-Fehlgriff 8->8 waere hier sichtbar geworden). */
    {
        long r2sz = 0;
        uint8_t *r2 = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1190.RDT", &r2sz);
        if (r2) {
            static re15_esp_t esp1190;
            if (re15_esp_parse(r2, (size_t)r2sz, rd32(r2, 0x4C), rd32(r2, 0x50),
                               rd32(r2, 0x54), rd32(r2, 0x58), &esp1190) == 0) {
                re15_esp_set_room_bank(&esp1190);
                re15_esp_fx_reset();
                re15_re2z_gore_fx_test(z, 0, 0x08001B58u, 0, 0, 0, 0);
                CHECK(re15_esp_fx_count() == 0,
                      "ROOM1190 ohne Id 5: %d Slots (erwartet 0 — kein Feuer-Fehlgriff)",
                      re15_esp_fx_count());
                printf("  ROOM1190 (nur {7}): Id-5-Spawn unterbleibt (SOLL)\n");
            }
            free(r2);
        }
    }

    free(rdt); free(core);
    if (g_fail) { printf("=== FAIL ===\n"); return 1; }
    printf("=== PASS: Dekoder-Uebersetzung, Banken, Skalen, Anker-Versatz ===\n");
    return 0;
}
