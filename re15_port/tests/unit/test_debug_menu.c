/* test_debug_menu.c — nagelt das portierte ORIGINAL-Debug-Menue an die EXE-Bytes.
 *
 * Quelle: PSX.EXE, Menue-Funktion @0x80014444. Jede Erwartung unten zitiert ihre Adresse.
 * Zweck des Menues im Port: Original und Port muessen einen Raum ueber DENSELBEN Weg betreten,
 * sonst vergleicht der Paritaets-Harness zwei verschiedene Situationen.
 */
#include "re15_debug_menu.h"
#include "debug_jump_table.h"
#include <stdio.h>

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

#define PAD_UP    0x1000
#define PAD_DOWN  0x4000
#define PAD_RIGHT 0x2000
#define PAD_LEFT  0x8000
#define PAD_EXIT  0x0040
#define EDGE_STAGE 0x0010
#define EDGE_LOAD  0x0080

/* held-Halbwort 0x800AC760, edge-Halbwort 0x800AC762 */
#define TICK(h)   re15_debug_menu_tick((h), 0)
#define TICKE(e)  re15_debug_menu_tick(0, (e))

int main(void)
{
    re15_debug_menu_reset();
    const re15_debug_menu_t *m = re15_debug_menu_state();

    CHECK(!re15_debug_menu_open(), "das Menue startet geschlossen");
    re15_debug_menu_toggle();
    CHECK(re15_debug_menu_open(), "toggle oeffnet das Menue");

    /* Auswahlzeile: 0..2, Untergrenze @0x8001460C, Obergrenze `sltiu v0,v0,0x2` @0x80014644 */
    CHECK(m->row == 0, "Startzeile 0");
    TICK(PAD_UP);
    CHECK(m->row == 0, "Zeile 0 darf nicht unter 0 (@0x8001460C beq v0,zero)");
    TICK(PAD_DOWN);
    CHECK(m->row == 1, "runter -> Zeile 1 (JUMP, @0x80014698)");
    TICK(PAD_DOWN);
    CHECK(m->row == 2, "runter -> Zeile 2 (MEMORY VIEWER, @0x800146A8)");
    TICK(PAD_DOWN);
    CHECK(m->row == 2, "Zeile 2 ist das Maximum (@0x80014644 sltiu 0x2)");
    TICK(PAD_UP);
    CHECK(m->row == 1, "hoch -> zurueck auf JUMP");

    /* JUMP-Zeile: Rechts erhoeht den Raumindex, LEERE SLOTS WERDEN UEBERSPRUNGEN (@0x800147B0). */
    CHECK(m->room_idx[0] == 0, "Raumindex startet bei 0");
    TICK(PAD_RIGHT);
    CHECK(m->room_idx[0] == 1, "Rechts -> Raumindex 1 (@0x800146D8)");
    for (int i = 0; i < 200; i++) {
        TICK(PAD_RIGHT);
        CHECK(RE15_DBG_SLOT_USED(0, m->room_idx[0]),
              "Rechts landet nie auf einem leeren Slot (@0x800147B0), idx=0x%02x", m->room_idx[0]);
        CHECK(m->room_idx[0] < 0x31, "Index bleibt unter 0x31 (@0x80014734), got 0x%02x", m->room_idx[0]);
    }
    for (int i = 0; i < 200; i++) {
        TICK(PAD_LEFT);
        CHECK(RE15_DBG_SLOT_USED(0, m->room_idx[0]),
              "Links landet nie auf einem leeren Slot, idx=0x%02x", m->room_idx[0]);
    }

    /* Die drei hardware-gemessenen Anker der Liste (re15-room-capture): BRIEFING ROOM ist Index 0x14
     * und damit Raum 0x114 (= Port-ROOM1140), OPENING liegt 16 Schritte weiter (0x24), SEWER EXIT
     * 10 Schritte vor OPENING (0x1A), BATH-LOCKERS ist Index 0 (Raum 0x100). */
    CHECK(RE15_DBG_JUMP_ROOM(0, 0x14) == 0x114, "Index 0x14 -> Raum 0x114 (@0x8001D644/60)");
    CHECK(RE15_DBG_JUMP_ROOM(0, 0x24) == 0x124, "Index 0x24 -> Raum 0x124 OPENING");
    CHECK(RE15_DBG_JUMP_ROOM(1, 0x00) == 0x200, "Stage 1 Index 0 -> Raum 0x200");

    /* Laden: Edge-Bit 0x80 (@0x80014A38) meldet want_load und den aufgeloesten Raum. */
    while (m->room_idx[0] != 0x14) TICK(PAD_RIGHT);
    CHECK(TICKE(EDGE_LOAD) == 1, "Quadrat loest das Laden aus (@0x80014A38)");
    CHECK(m->load_room == 0x114, "geladen wird 0x114 BRIEFING ROOM, got 0x%03x", m->load_room);
    re15_debug_menu_toggle();
    TICK(PAD_DOWN);

    /* Dreieck wechselt die Stage, Wrap bei 6 (@0x800148E4 sltiu 0x6). */
    CHECK(m->stage == 0, "Stage startet bei 0");
    for (int i = 0; i < 6; i++) TICKE(EDGE_STAGE);
    CHECK(m->stage == 0, "6 x Dreieck ist ein voller Ring (@0x800148E4/F0), got %d", m->stage);

    /* Der Raumindex haengt an der STAGE (0x800BBE5F + stage, @0x800146F4) — Stage 1 ist unberuehrt. */
    CHECK(m->room_idx[1] == 0, "Stage 1 hat einen EIGENEN Raumindex");

    /* In Zeile 2 (MEMORY VIEWER) darf Rechts den Raumindex NICHT bewegen: die Verzweigung
     * @0x80014698 fuehrt nur fuer Zeile 1 in den JUMP-Zweig. */
    TICK(PAD_DOWN);
    CHECK(m->row == 2, "auf Zeile 2 fuer den Gegentest");
    uint8_t before = m->room_idx[0];
    TICK(PAD_RIGHT);
    CHECK(m->room_idx[0] == before, "ausserhalb der JUMP-Zeile aendert Rechts nichts");

    /* Verlassen: @0x8001466C prueft Bit 0x40 */
    TICK(PAD_EXIT);
    CHECK(!re15_debug_menu_open(), "Bit 0x40 schliesst das Menue (@0x8001466C)");

    if (fails == 0) printf("test_debug_menu: OK\n");
    else            printf("test_debug_menu: %d FEHLER\n", fails);
    return fails ? 1 : 0;
}
