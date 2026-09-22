/* probe_besitz_zensus.c — MESSUNG (keine Abnahme): erscheint die Wegwerf-Abfrage an
 * einer Benutzungsstelle, OHNE dass der Spieler den Gegenstand traegt?
 *
 * Gefahren wird je Stelle das AUSGELIEFERTE Unterprogramm im ECHTEN VM, mit einem
 * Spieler, der jede Ja/Nein-Frage des Skripts mit JA beantwortet (virtuelles
 * Bestaetigen-Bit 0x4000). Zwei Laeufe je Stelle:
 *   OHNE  — Inventar leer. Verlangt: die Nachricht laeuft, die Abfrage kommt NICHT.
 *   MIT   — Gegenstand mit Anzahl 1 im Inventar. Verlangt: die Abfrage kommt.
 * Ausgegeben wird zusaetzlich, welche Flag-Bits das Unterprogramm dabei SETZT — das
 * ist die Grundlage fuer die Frage "hat ein Ja ein Tor geschlossen?".
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_item_discard.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t g_scd;
void scd_register_current_rdt(const re15_rdt_t *rdt);

#include "gen/discard_sites.inc"

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static const char *stage_dir(unsigned room) { return
    (room >> 12) == 1 ? "STAGE1" : (room >> 12) == 2 ? "STAGE2" :
    (room >> 12) == 3 ? "STAGE3" : (room >> 12) == 4 ? "STAGE4" :
    (room >> 12) == 5 ? "STAGE5" : "STAGE6"; }

static uint8_t *raum_laden(unsigned room, re15_rdt_t *rdt, size_t *n)
{
    char p[600];
    snprintf(p, sizeof p, "%s/%s/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, stage_dir(room), room);
    uint8_t *raw = slurp(p, n);
    if (!raw) return NULL;
    if (re15_rdt_parse(raw, *n, rdt) < 0) { free(raw); return NULL; }
    return raw;
}

static int sub_mit_nachricht(re15_rdt_t *rdt, uint8_t msg_id)
{
    for (int s = 0; s < rdt->sub_scd_count; s++) {
        const uint8_t *p = rdt->sub_scd[s];
        if (!p) continue;
        long grenze = 4096;
        if (s + 1 < rdt->sub_scd_count && rdt->sub_scd[s+1] > p)
            grenze = rdt->sub_scd[s+1] - p;
        for (long o = 0; o < grenze; ) {
            int sz = scd_opcode_size_at(p + o);
            if (sz <= 0) break;
            if (p[o] == 0x2B && p[o+1] == msg_id) return s;
            o += sz;
        }
    }
    return -1;
}

static void grundzustand(void)
{
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_inv_init();
    re15_discard_reset();
    re15_game_state_init();
    memset(&g_scd, 0, sizeof g_scd);
    g_scd.work_slot = -1;
}

/* Ein Lauf: das Unterprogramm fahren, Ja/Nein mit JA beantworten.
 * out_reached = die Nachricht der Stelle stand; return = Abfrage kam. */
static int lauf(re15_rdt_t *rdt, unsigned room, uint8_t msg, int sub,
                int mit_gegenstand, uint8_t item, int *out_reached)
{
    extern uint16_t g_scd_pad_edge, g_scd_pad_held;
    grundzustand();
    g_current_room_id = room;
    re15_msg_load_room_block(rdt->messages, rdt->messages_size);
    scd_register_current_rdt(rdt);
    if (mit_gegenstand) re15_inv_grant(item, 1);
    scd_thread_start(0, rdt->sub_scd[sub]);
    int reached = 0, gefragt = 0;
    for (long fr = 0; fr < 6000; fr++) {
        const unsigned char *r; int l, id;
        g_scd_pad_edge = (fr > 2 && (fr % 4) == 0) ? 0x4000u : 0u;
        g_scd_pad_held = 0;
        if (!re15_discard_frozen()) scd_vm_tick();
        re15_msg_tick(&r, &l, &id);
        g_scd_pad_edge = 0;
        if (re15_pauseflags_belegt() && g_scd.message_id == msg) reached = 1;
        if (re15_discard_active()) re15_discard_tick(0, 0);
        if (re15_discard_active()) gefragt = 1;
        if (re15_discard_prompt(NULL, NULL)) break;
    }
    scd_register_current_rdt(NULL);
    if (out_reached) *out_reached = reached;
    return gefragt;
}

int main(void)
{
    printf("=== ZENSUS: Abfrage OHNE Besitz, je Benutzungsstelle, ausgeliefertes Unterprogramm ===\n");
    printf("%-10s %5s %4s %6s | %-24s | %-24s\n",
           "Raum", "sub", "msg", "item", "OHNE Besitz", "MIT Besitz");
    int ohne_abfrage = 0, mit_abfrage = 0, gefahren = 0, unerreicht = 0;
    for (int i = 0; i < RE15_DISCARD_SITE_COUNT; i++) {
        unsigned room = re15_discard_sites[i].room;
        uint8_t  msg  = re15_discard_sites[i].msg;
        uint8_t  item = re15_discard_sites[i].item;
        re15_rdt_t rdt; size_t n = 0;
        uint8_t *raw = raum_laden(room, &rdt, &n);
        if (!raw) { printf("ROOM%04X   -- SKIP (RDT fehlt)\n", room); continue; }
        int sub = sub_mit_nachricht(&rdt, msg);
        if (sub < 0) { printf("ROOM%04X   -- kein Message_on %u\n", room, msg); free(raw); continue; }

        int r_ohne = 0, r_mit = 0;
        int a_ohne = lauf(&rdt, room, msg, sub, 0, item, &r_ohne);
        int a_mit  = lauf(&rdt, room, msg, sub, 1, item, &r_mit);
        printf("ROOM%04X %5d %4u   0x%02X | erreicht=%d abfrage=%-3s | erreicht=%d abfrage=%-3s\n",
               room, sub, msg, item, r_ohne, a_ohne ? "JA" : "nein",
               r_mit, a_mit ? "JA" : "nein");
        if (r_ohne) { gefahren++; if (a_ohne) ohne_abfrage++; } else unerreicht++;
        if (a_mit) mit_abfrage++;
        free(raw);
    }
    printf("\nSUMME: %d von %d Stellen erreicht (%d nicht erreicht)\n",
           gefahren, RE15_DISCARD_SITE_COUNT, unerreicht);
    printf("OHNE BESITZ eine Abfrage: %d von %d erreichten Stellen\n", ohne_abfrage, gefahren);
    printf("MIT  BESITZ eine Abfrage: %d von %d Stellen\n", mit_abfrage, RE15_DISCARD_SITE_COUNT);
    return 0;
}
