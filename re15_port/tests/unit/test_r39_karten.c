/* =============================================================================
 * Runde 39 Spur G "karten" — Mess-Sonde und Riegel
 * =============================================================================
 * Nutzer 2026-10-17 (analysis/befunde_runde39/AUFTRAG.md, woertlich):
 *   1. "Bei den Maps des Police Departement passt es noch nicht so ganz. [...]
 *      map3F_correct.png, map2F_correct.png, map1F_correct.png, mapB2_correct.png.
 *      Dort ist immer links, wie es bei uns aussieht und rechts wie es eigentlich
 *      aussehen sollte. Keine Doppelwaende, Tuer immer genau auf Wand platziert,
 *      treppen nicht in der Wand platziert usw. Und natuerlich muss der Spielermarker
 *      da entsprechend passen. Der Room B1 den habe ich nicht gemacht. [...] Die Karte
 *      von B1 ist noch ziemlicher schrott."
 *   2. "In ROOM 1090 ist Leons Positionsmarker der Map ausserhalb des aktuellen Raums."
 * Dossier: analysis/befunde_runde39/G_karten.md.
 *
 * Aufruf: probe_r39_karten <teil> ...
 *   ops <seite> <hexraum> <x> <z> [band]   Op-Liste des Kartenschirms (alle Blaetter
 *                                          aufgedeckt) - Messwerkzeug, kein Urteil
 * ========================================================================== */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_aot.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_collision.h"
#include "re15_inv_screen.h"
#include "re15_scd.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_esp.h"
#include "re15_msg.h"

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", (name)); g_fail = 1; } \
                               else printf("  PASS: %s\n", (name)); } while (0)

static re15_rdt_t s_rdt;
static uint8_t *s_roh;

static uint8_t *slurp(const char *pfad, size_t *n)
{
    FILE *f = fopen(pfad, "rb");
    long sz;
    uint8_t *b;
    if (!f) return 0;
    fseek(f, 0, SEEK_END); sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return 0; }
    b = (uint8_t *)malloc((size_t)sz);
    if (!b) { fclose(f); return 0; }
    if (fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return 0; }
    fclose(f); *n = (size_t)sz; return b;
}

/* Dieselbe Kette wie test_map_raum_live.c / test_r36_karte.c betrete(). */
static int betrete(unsigned rid, int32_t px, int32_t pz, int band)
{
    char pfad[600];
    size_t n = 0;
    re15_actor_t *pl;
    snprintf(pfad, sizeof pfad, "%s/STAGE%u/ROOM%04X.RDT",
             RE15_ASSET_PSX_DIR, (rid >> 12) & 0xF, rid);
    if (s_roh) { free(s_roh); s_roh = 0; }
    s_roh = slurp(pfad, &n);
    if (!s_roh) return 0;
    if (re15_rdt_parse(s_roh, n, &s_rdt) < 0) return 0;
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_enemy_reset();
    re15_enemy_ai_set_paused(1);
    re15_esp_fx_reset();
    g_current_room_id = rid;
    g_room_change.pending = 0;
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = px; pl->y = 0; pl->z = pz;
    re15_collision_reset_band();
    if (band >= 0) re15_collision_set_band(band);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    return 1;
}

static re15_inv_op_t s_ops[RE15_INV_MAX_OPS];

/* Kartenschirm fuer <seite> bauen (alle Blaetter aufgedeckt, wie RE15_MAP_SHOT_SWEEP). */
static int baue_seite(int seite)
{
    int p;
    for (p = 0; p < 13; p++) re15_map_debug_reveal_page((unsigned)p);
    re15_inv_screen_open();
    g_inv_screen.substate = 1;
    g_inv_screen.item_state = 1;
    g_inv_screen.map_page = (uint8_t)seite;
    return re15_inv_screen_build(&g_inv_screen, s_ops, RE15_INV_MAX_OPS);
}

static int teil_ops(int argc, char **argv)
{
    int seite, n, i, band = -1;
    unsigned rid;
    if (argc < 6) { printf("FAIL: ops <seite> <hexraum> <x> <z> [band]\n"); return 2; }
    seite = atoi(argv[2]);
    rid = (unsigned)strtoul(argv[3], NULL, 16);
    if (argc > 6) band = atoi(argv[6]);
    if (!betrete(rid, atoi(argv[4]), atoi(argv[5]), band)) { printf("FAIL: Raum\n"); return 1; }
    re15_map_visited_mark(rid);
    re15_map_zone_update(rid, atoi(argv[4]), atoi(argv[5]));
    n = baue_seite(seite);
    printf("# ops %d seite %d\n", n, seite);
    for (i = 0; i < n; i++) {
        const re15_inv_op_t *o = &s_ops[i];
        printf("op %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", i, o->kind, o->page, o->clut,
               o->abe, o->x, o->y, o->w, o->h, o->u, o->v, o->r, o->g, o->b);
    }
    return 0;
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "";
    if (!strcmp(teil, "ops")) return teil_ops(argc, argv);
    printf("FAIL: unbekannter Teil '%s'\n", teil);
    (void)g_fail;
    return 2;
}
