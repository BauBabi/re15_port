/* test_debug_menu.c — nagelt das portierte ORIGINAL-Debug-Menue an die EXE-Bytes.
 *
 * Quelle: PSX.EXE, Menue-Funktion @0x80014444. Jede Erwartung unten zitiert ihre Adresse.
 * Zweck des Menues im Port: Original und Port muessen einen Raum ueber DENSELBEN Weg betreten,
 * sonst vergleicht der Paritaets-Harness zwei verschiedene Situationen.
 */
#include "re15_debug_menu.h"
#include <stdio.h>

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

#define PAD_UP    0x1000
#define PAD_DOWN  0x4000
#define PAD_RIGHT 0x2000
#define PAD_EXIT  0x0040

int main(void)
{
    re15_debug_menu_reset();
    const re15_debug_menu_t *m = re15_debug_menu_state();

    CHECK(!re15_debug_menu_open(), "das Menue startet geschlossen");
    re15_debug_menu_toggle();
    CHECK(re15_debug_menu_open(), "toggle oeffnet das Menue");

    /* Auswahlzeile: 0..2, Untergrenze @0x8001460C, Obergrenze `sltiu v0,v0,0x2` @0x80014644 */
    CHECK(m->row == 0, "Startzeile 0");
    re15_debug_menu_tick(PAD_UP);
    CHECK(m->row == 0, "Zeile 0 darf nicht unter 0 (@0x8001460C beq v0,zero)");
    re15_debug_menu_tick(PAD_DOWN);
    CHECK(m->row == 1, "runter -> Zeile 1 (JUMP, @0x80014698)");
    re15_debug_menu_tick(PAD_DOWN);
    CHECK(m->row == 2, "runter -> Zeile 2 (MEMORY VIEWER, @0x800146A8)");
    re15_debug_menu_tick(PAD_DOWN);
    CHECK(m->row == 2, "Zeile 2 ist das Maximum (@0x80014644 sltiu 0x2)");
    re15_debug_menu_tick(PAD_UP);
    CHECK(m->row == 1, "hoch -> zurueck auf JUMP");

    /* JUMP-Zeile: Rechts erhoeht den Raumindex der aktuellen Stage, Wrap bei 0x31 (@0x80014734) */
    CHECK(m->room_idx[0] == 0, "Raumindex startet bei 0");
    re15_debug_menu_tick(PAD_RIGHT);
    CHECK(m->room_idx[0] == 1, "Rechts -> Raumindex 1 (@0x800146D8)");
    for (int i = 0; i < 0x2F; i++) re15_debug_menu_tick(PAD_RIGHT);
    CHECK(m->room_idx[0] == 0x30, "Raumindex laeuft bis 0x30, got 0x%02x", m->room_idx[0]);
    re15_debug_menu_tick(PAD_RIGHT);
    CHECK(m->room_idx[0] == 0, "0x31 wrappt auf 0 (@0x80014734 sltiu 0x31 / @0x8001474C sb zero), "
                               "got 0x%02x", m->room_idx[0]);

    /* Der Raumindex haengt an der STAGE (0x800BBE5F + stage, @0x800146F4) — Stage 1 ist unberuehrt. */
    CHECK(m->room_idx[1] == 0, "Stage 1 hat einen EIGENEN Raumindex");

    /* In Zeile 2 (MEMORY VIEWER) darf Rechts den Raumindex NICHT bewegen: die Verzweigung
     * @0x80014698 fuehrt nur fuer Zeile 1 in den JUMP-Zweig. */
    re15_debug_menu_tick(PAD_DOWN);
    CHECK(m->row == 2, "auf Zeile 2 fuer den Gegentest");
    uint8_t before = m->room_idx[0];
    re15_debug_menu_tick(PAD_RIGHT);
    CHECK(m->room_idx[0] == before, "ausserhalb der JUMP-Zeile aendert Rechts nichts");

    /* Verlassen: @0x8001466C prueft Bit 0x40 */
    re15_debug_menu_tick(PAD_EXIT);
    CHECK(!re15_debug_menu_open(), "Bit 0x40 schliesst das Menue (@0x8001466C)");

    if (fails == 0) printf("test_debug_menu: OK\n");
    else            printf("test_debug_menu: %d FEHLER\n", fails);
    return fails ? 1 : 0;
}
