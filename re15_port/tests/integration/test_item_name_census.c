/* ============================================================================
 *  test_item_name_census.c — WACHE fuer die Item-Namenstabelle des Prompts.
 *
 *  NUTZER-BEFUND 2026-08-27: "beim nehmen des Feuerloeschers room 1000 steht noch
 *  'will you take ?' statt 'will you take fire extinguisher'."
 *
 *  Ursache war eine ABGESCHNITTENE vendorte Tabelle (48 statt 102 Eintraege). Der
 *  Feuerloescher ist Item 0x31 = 49 und fiel heraus; item_prompt_common.c setzte den
 *  Namen dann stillschweigend nicht ein. Das ist genau die Klasse Fehler, die nur
 *  auffaellt, wenn jemand das betroffene Item im Spiel aufhebt — also pinnen wir sie.
 *
 *  BELEGE (volle Disasm-Zitate im Kopf von item_prompt_common.c):
 *    - Namens-Reader FUN_80028840 @0x80028840: `andi a0,a0,0xff` / `sll a0,a0,1` /
 *      `lhu 0x800c495c + id*2` / `addu 0x800c4a28` — KEINE Bereichspruefung. Die
 *      Offsettabelle endet physisch am Blob: (0x800c4a28-0x800c495c)/2 = 102.
 *    - Item-ID des Feuerloeschers: ROOM1000.RDT @0xC24 Item_aot_set (Opcode 0x50),
 *      pc[14] @0xC32 = 0x31, pc[16] @0xC34 = 0x01.
 *
 *  DIE WACHE IST NICHT VAKUANT: sie faehrt die ECHTE SCD-VM ueber alle ausgelieferten
 *  RDTs aller sechs Stages, sammelt jede Item-ID, die ein Item_aot_set wirklich setzt,
 *  und verlangt fuer JEDE einen nicht-leeren Namen. Zusaetzlich muss der Zensus eine
 *  Mindestzahl ausgewerteter Faelle erreichen UND den Feuerloescher enthalten — sonst
 *  meldet er FAIL statt "gruen bei 0 Faellen".
 * ==========================================================================*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_item_prompt.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)
#define RDT_MIN_SIZE 0x60

/* Der Feuerloescher — der gemeldete Fall. */
#define ITEM_FIRE_EXTINGUISHER 0x31
/* Gemessener Zensus-Umfang (Lauf 2026-08-27). Untere Schranken gegen eine kollabierte
 * Sammelschleife: wird der Sweep oder der Walker kaputt gemacht, faellt die Zahl. */
#define MIN_DISTINCT_ITEMS 20
#define MIN_ROOMS          80

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f); fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz; return buf;
}

/* Laenge des eingesetzten Namens = Gesamt-Glyphen des Prompts minus die des Skripts mit
 * dem LEEREN Namen (Item 0x00 ist im Original der leere Eintrag, Offset 0x0000 -> 0x07). */
static int name_glyphs(uint8_t id)
{
    int empty = re15_item_prompt_walk(1, 0x00, 0, NULL, NULL);
    int with  = re15_item_prompt_walk(1, id,   0, NULL, NULL);
    return with - empty;
}

static void collect_glyphs(void *ctx, unsigned char g, int attr, int nl)
{
    (void)attr;
    char *o = (char *)ctx;
    size_t n = strlen(o);
    if (n >= 190) return;
    if (nl)                          o[n] = '\n';
    else if (g == 0x00)              o[n] = ' ';
    else if (g >= 0x1d && g <= 0x36) o[n] = (char)('A' + g - 0x1d);
    else if (g >= 0x3d && g <= 0x56) o[n] = (char)('a' + g - 0x3d);
    else if (g >= 0x0d && g <= 0x16) o[n] = (char)('0' + g - 0x0d);
    else                             o[n] = '?';
    o[n + 1] = '\0';
}

static int seen[256];

static int census_stage(const char *base, int stage, unsigned lo, unsigned hi)
{
    int rooms = 0;
    for (unsigned rid = lo; rid <= hi; rid += 0x10) {
        char path[600];
        snprintf(path, sizeof path, "%s/STAGE%d/ROOM%04X.RDT", base, stage, rid);
        size_t sz = 0;
        uint8_t *buf = read_file(path, &sz);
        if (!buf) continue;
        if (sz < RDT_MIN_SIZE) { free(buf); continue; }
        re15_rdt_t rdt;
        if (re15_rdt_parse(buf, sz, &rdt) != 0) { free(buf); continue; }

        scd_vm_init();                              /* nullt g_aot (aot_common.c:38) */
        g_current_room_id = rid;
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 0; pl->hp = 100;
        scd_register_room_events(&rdt);
        scd_room_reenter(&rdt, 0, 0, /*entry_scenario=*/0);
        for (int f = 0; f < 8; f++) scd_vm_tick();
        for (int e = 0; e < rdt.sub_scd_count; e++) {
            if (!rdt.sub_scd[e]) continue;
            scd_event_fire((uint8_t)e);
            for (int f = 0; f < 6; f++) scd_vm_tick();
        }
        for (int s = 0; s < RE15_AOT_MAX; s++) {
            uint8_t t = g_aot.item_params[s].item_type;
            if (t) seen[t] = 1;                     /* 0x00 = leerer Eintrag, kein Item */
        }
        rooms++;
        free(buf);
    }
    return rooms;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    int fail = 0, rooms = 0;

    printf("=== Item-Namen-Zensus (Prompt-Tabelle @0x800c495c/@0x800c4a28) ===\n");

    /* --- Pin 1: die Tabellen-Ausdehnung selbst -------------------------------------- */
    if (re15_item_prompt_name_count() != 102) {
        printf("FAIL: NIDS = %d, erwartet 102 = (0x800c4a28-0x800c495c)/2 (FUN_80028840)\n",
               re15_item_prompt_name_count());
        fail = 1;
    } else {
        printf("  PASS: Namenstabelle 102 Eintraege (Ausdehnung @0x800c495c..0x800c4a28)\n");
    }
    if (re15_item_prompt_script_count() != 8) {
        printf("FAIL: NSCRIPTS = %d, erwartet 8 (u16[0]=0x10 @0x800c4fc6 = Tabellenende)\n",
               re15_item_prompt_script_count());
        fail = 1;
    } else {
        printf("  PASS: 8 Prompt-Skripte (Selektor FUN_80027e68 @0x80027f30-f44)\n");
    }

    /* --- Pin 2: der gemeldete Fall, wortwoertlich ----------------------------------- */
    {
        char out[256]; out[0] = '\0';
        re15_item_prompt_walk(1, ITEM_FIRE_EXTINGUISHER, 200, collect_glyphs, out);
        printf("  ROOM1000-Prompt (Item 0x%02x): \"%s\"\n", ITEM_FIRE_EXTINGUISHER, out);
        if (!strstr(out, "Fire Extinguisher")) {
            printf("FAIL: Item 0x%02x setzt \"Fire Extinguisher\" nicht ein "
                   "(DEBUG.BIN Blob+0x02b4)\n", ITEM_FIRE_EXTINGUISHER);
            fail = 1;
        } else {
            printf("  PASS: Item 0x%02x -> \"Fire Extinguisher\"\n", ITEM_FIRE_EXTINGUISHER);
        }
    }

    /* --- Pin 3: jede ID, die ein ausgeliefertes Item_aot_set wirklich setzt ---------- */
    memset(seen, 0, sizeof seen);
    rooms += census_stage(base, 1, 0x1000, 0x1270);
    rooms += census_stage(base, 2, 0x2000, 0x2270);
    rooms += census_stage(base, 3, 0x3000, 0x3270);
    rooms += census_stage(base, 4, 0x4000, 0x4270);
    rooms += census_stage(base, 5, 0x5000, 0x5270);
    rooms += census_stage(base, 6, 0x6000, 0x6270);

    int distinct = 0, nameless = 0, fx_seen = 0;
    for (int id = 1; id < 256; id++) {
        if (!seen[id]) continue;
        distinct++;
        if (id == ITEM_FIRE_EXTINGUISHER) fx_seen = 1;
        int len = name_glyphs((uint8_t)id);
        if (len <= 0) {
            char out[256]; out[0] = '\0';
            re15_item_prompt_walk(1, (uint8_t)id, 200, collect_glyphs, out);
            printf("FAIL: Item 0x%02x hat KEINEN Namen -> \"%s\"\n", id, out);
            nameless++;
            fail = 1;
        }
    }
    printf("\n%d Raeume gefahren, %d verschiedene Item-IDs ausgewertet, %d ohne Namen.\n",
           rooms, distinct, nameless);

    /* --- Nicht-vakuant: der Zensus MUSS etwas ausgewertet haben --------------------- */
    if (rooms < MIN_ROOMS) {
        printf("FAIL: nur %d Raeume geladen (erwartet >= %d) — Zensus kollabiert, "
               "gruen waere hier bedeutungslos\n", rooms, MIN_ROOMS);
        fail = 1;
    }
    if (distinct < MIN_DISTINCT_ITEMS) {
        printf("FAIL: nur %d Item-IDs ausgewertet (erwartet >= %d) — Sammelschleife "
               "kollabiert\n", distinct, MIN_DISTINCT_ITEMS);
        fail = 1;
    }
    if (!fx_seen) {
        printf("FAIL: der Feuerloescher (0x%02x) kam im Zensus nicht vor — die Wache haette "
               "den gemeldeten Fehler NICHT gefangen\n", ITEM_FIRE_EXTINGUISHER);
        fail = 1;
    }

    if (fail) { printf("ITEM NAME CENSUS: FAIL\n"); return 1; }
    printf("ITEM NAME CENSUS: jede ausgelieferte Item-ID hat einen Namen\n");
    return 0;
}
