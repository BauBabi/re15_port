/*
 * re15_map_visited.c — "Raum besucht"-Verwaltung fuer das RE2-Kartensystem im Port.
 *
 * NUTZER-AUFTRAG 2026-08-30: das RE2-Retail-Kartensystem (aktueller Raum ROT, besuchte
 * GRUEN, unbesuchte SCHWARZ) "bezogen auf unsere Map". Das RE1.5-Original hat KEINEN
 * Besucht-Speicher (der MAP-Screen liest nur DAT_800b260d/0e + Spieler-X/Z; beide liegen
 * ausserhalb des Save-GSB [0x800b0fbc,0x800b21ec) — analysis/nutzer_batch_2026-08-30b/
 * map-port-und-daten.md §3). Dieses Modul ist also PORT-ERWEITERUNG nach RE2-Vorbild,
 * klar getrennt von byte-true Bestand.
 *
 * Traeger: 1 Bit je Eintrag der Raumliste re15_room_ids[240] (re15_room_list.h) =
 * 30 Byte, als 32-Byte-Block im Save (v6, re15_savedata.c). Szenario-Varianten
 * (0xSRR0/0xSRR1) teilen sich bewusst KEIN Bit — der Index ist der Listen-Index,
 * damit bleibt die Zuordnung trivial und stabil.
 *
 * Markiert wird am EINZIGEN Raum-Lade-Choke-Point (scd_room_reenter) — er laeuft bei
 * Boot, Tuer-Wechsel, Selbst-Tuer und Load gleichermassen.
 */
#include <stdint.h>
#include <string.h>
#include "re15_room.h"
#include "re15_room_list.h"

static uint8_t s_visited[32];   /* 240 Bits, Index = re15_room_ids[]-Index */

void re15_map_visited_reset(void) { memset(s_visited, 0, sizeof s_visited); }

static int room_index(unsigned room_id)
{
    for (int i = 0; i < RE15_ROOM_COUNT; i++)
        if (re15_room_ids[i] == room_id) return i;
    return -1;
}

void re15_map_visited_mark(unsigned room_id)
{
    int i = room_index(room_id);
    if (i < 0) return;
    s_visited[i >> 3] |= (uint8_t)(1u << (i & 7));
}

int re15_map_visited(unsigned room_id)
{
    int i = room_index(room_id);
    if (i < 0) return 0;
    return (s_visited[i >> 3] >> (i & 7)) & 1;
}

void re15_map_visited_export(uint8_t out[32]) { memcpy(out, s_visited, 32); }
void re15_map_visited_import(const uint8_t in[32]) { memcpy(s_visited, in, 32); }

/* ---- Rect-Zustand fuer den MAP-Screen (Zuordnung Raum->Rect: re15_map_rooms.h) ----
 * Mehrere Raeume (auch Szenario-Varianten) duerfen dasselbe Rect teilen; es gewinnt
 * der staerkste Zustand: aktueller Raum > besucht > unbesucht. Rects ohne Zuordnung
 * melden 0 (der Zeichner laesst sie im Stock-Neutralton stehen). */
#include <stdlib.h>
#include "re15_map_rooms.h"
#include "re15_map_row_fix.h"

static int s_stock_mode = -1;   /* -1 = noch nicht aus der Umgebung gelesen */

int re15_map_stock_mode(void)
{
    if (s_stock_mode < 0) {
        const char *e = getenv("RE15_MAP_STOCK");
        s_stock_mode = (e && e[0] == '1') ? 1 : 0;
    }
    return s_stock_mode;
}

/* Nur fuer Tests: Modus direkt setzen (-1 = beim naechsten Aufruf wieder Umgebung). */
void re15_map_stock_set(int v) { s_stock_mode = v; }

const re15_map_row_fix_t *re15_map_row_fix_find(unsigned slot)
{
    for (size_t k = 0; k < sizeof s_map_row_fixes / sizeof s_map_row_fixes[0]; k++)
        if (s_map_row_fixes[k].slot == slot) return &s_map_row_fixes[k];
    return 0;
}

int re15_map_rect_state(unsigned page, unsigned rect_idx)
{
    int state = RE15_MAP_RECT_UNMAPPED;
    for (size_t k = 0; k < sizeof s_map_room_rects / sizeof s_map_room_rects[0]; k++) {
        const re15_map_room_rect_t *m = &s_map_room_rects[k];
        if (m->page != page || m->rect != rect_idx) continue;
        if (m->room == (unsigned short)g_current_room_id) return RE15_MAP_RECT_CURRENT;
        if (state < RE15_MAP_RECT_VISITED && re15_map_visited(m->room))
            state = RE15_MAP_RECT_VISITED;
        else if (state < RE15_MAP_RECT_UNVISITED)
            state = RE15_MAP_RECT_UNVISITED;
    }
    return state;
}
