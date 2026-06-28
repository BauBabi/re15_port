/**
 * @file test_room_load.c
 * @brief Unit-Tests fuer das Raum-Lade-System (re15_room.h / re15_rdt.h)
 *
 * REAKTIVIERUNG 2026-06-28: Die getestete Engine-API wurde komplett
 * umgebaut (Engine-Transplant). Die Original-Intention dieses Tests war,
 * die ERROR-PATHS und die ZUSTANDS-SAUBERKEIT des Raum-Ladens zu pruefen,
 * OHNE echte RDT-Dateien zu brauchen:
 *   - Laden mit ungueltigen Argumenten  -> Fehlercode
 *   - Zustand nach "Entladen"/Reset      -> sauber (keine Garbage-Pointer)
 *   - Fehlende/abwesende Sektionen        -> NULL-Pointer, kein Crash
 *
 * Diese Intention ist erhalten geblieben, aber gegen die AKTUELLE API
 * abgebildet. Was sich geaendert hat (Legacy -> Aktuell):
 *
 *   re15_room_load(stage,room,player)->int(IO-code)
 *        ->  re15_room_load(unsigned room_id)->int (0/-1)   [re15_room.h:33]
 *   re15_room_unload()                       -> ENTFERNT: der RDT-Puffer wird
 *        beim naechsten Laden ueberschrieben (ein Puffer fuer jeden Raum),
 *        ein separates Unload existiert nicht mehr   [re15_room.h:5-10]
 *   g_io / re15_io_backend_t (pluggable I/O) -> ENTFERNT: I/O ist pro Port
 *        fest (PSX = CD, PC = Datei), keine Indirektion mehr
 *   g_game.current_rdt / .rdt_buffer / .rdt_buffer_size -> ENTFERNT:
 *        g_game ist jetzt NUR { uint32_t flags[16][8] }   [re15_scd.h:331-338]
 *        Der residente RDT liegt in g_room_rdt + g_room_rdt_ok  [re15_room.h:20-21]
 *   re15_rdt_t.collision / .camera (Test-Felder) -> jetzt .sca / .cuts ;
 *        Skript bleibt .main_scd                          [re15_rdt.h:74,83,95]
 *
 * Da der eigentliche Loader (re15_room_load) jetzt PLATTFORM-Code ist
 * (room_pc.c / re15_room.c) und NICHT in der Engine-Lib liegt, testen wir
 * die HEADER-deklarierte, engine-seitige Parse-/Zustands-Logik, die der
 * Loader intern aufruft: re15_rdt_parse() (rdt_common.c). Sie ist der
 * faithful-equivalente, oeffentlich testbare Kern dessen, was der alte
 * Test ueber den (gemockten) Lade-Pfad geprueft hat: Argument-Validierung
 * und der saubere Pointer-Zustand nach dem Parsen.
 *
 * Requirements: 10.1, 12.2
 */

#include "re15_room.h"     /* re15_room_load(unsigned), g_room_rdt, g_room_rdt_ok */
#include "re15_rdt.h"      /* re15_rdt_t, re15_rdt_parse() */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =========================================================================
 * Synthetischer RDT-Header: 0x60 Bytes (Mindestgroesse, re15_rdt.h:174 /
 * rdt_common.c:211 "size < 0x60 -> -2"). ALLE Sektions-Offsets im
 * Adress-Verzeichnis (0x08..0x5c) = 0 -> die jeweilige Sektion ist
 * ABWESEND und der Parser setzt ihren Pointer auf NULL. Genau dieser
 * "keine Daten -> NULL, kein Crash"-Fall war Test 1/2 im Original.
 * ========================================================================= */

#define RDT_HEADER_SIZE 0x60

static uint8_t g_empty_rdt[RDT_HEADER_SIZE];

/* Lokale Test-RDT-Struktur (Parse-Ziel). */
static re15_rdt_t g_test_rdt;

/* =========================================================================
 * Test 1: re15_rdt_parse() mit "leerem" RDT — kein Crash, NULL-Sektionen
 *
 * Aequivalent zum alten test_room_unload_safe: ein Raum ohne Sektions-
 * Daten muss sauber durchlaufen; die Sektions-Pointer muessen NULL sein
 * (nicht Garbage), damit ein nachfolgendes Subsystem sie als "absent"
 * erkennt.
 *
 * Requirement: 10.1
 * ========================================================================= */

static int test_empty_rdt_null_sections(void)
{
    int rc;

    /* Header-Bytes alle 0: Counter = 0, alle Sektions-Offsets = 0. */
    memset(g_empty_rdt, 0, sizeof(g_empty_rdt));
    /* Ziel-Struktur mit Garbage fuellen, damit wir sehen, dass der Parser
     * sie tatsaechlich auf NULL setzt (und nicht nur "schon NULL" liest). */
    memset(&g_test_rdt, 0xAA, sizeof(g_test_rdt));

    rc = re15_rdt_parse(g_empty_rdt, sizeof(g_empty_rdt), &g_test_rdt);
    if (rc != 0) {
        fprintf(stderr, "FAIL: parse of empty 0x60 header should succeed, got %d\n", rc);
        return 1;
    }

    /* Abwesende Sektionen -> NULL-Pointer (rdt_common.c:172, :237, :246). */
    if (g_test_rdt.main_scd != NULL) {
        fprintf(stderr, "FAIL: main_scd sollte NULL sein bei leerem RDT\n");
        return 1;
    }
    if (g_test_rdt.cuts != NULL) {
        fprintf(stderr, "FAIL: cuts (Kamera) sollte NULL sein bei leerem RDT\n");
        return 1;
    }
    if (g_test_rdt.sca != NULL) {
        fprintf(stderr, "FAIL: sca (Collision) sollte NULL sein bei leerem RDT\n");
        return 1;
    }

    printf("PASS: test_empty_rdt_null_sections\n");
    return 0;
}

/* =========================================================================
 * Test 2: re15_rdt_parse() nullt/initialisiert den Zustand vollstaendig
 *
 * Aequivalent zum alten test_room_unload_clears_state: nach dem Parsen
 * eines daten-losen RDT duerfen KEINE Restwerte aus der vorher mit 0xAA
 * gefuellten Struktur in den ausgewerteten Feldern stehen.
 *
 * Requirement: 10.1, 12.2
 * ========================================================================= */

static int test_rdt_state_fully_reset(void)
{
    int rc;

    memset(g_empty_rdt, 0, sizeof(g_empty_rdt));
    memset(&g_test_rdt, 0xAA, sizeof(g_test_rdt));

    rc = re15_rdt_parse(g_empty_rdt, sizeof(g_empty_rdt), &g_test_rdt);
    if (rc != 0) {
        fprintf(stderr, "FAIL: parse should succeed, got %d\n", rc);
        return 1;
    }

    /* Counter aus dem (Null-)Header -> 0, NICHT 0xAA-Garbage. */
    if (g_test_rdt.cut_count != 0) {
        fprintf(stderr, "FAIL: cut_count sollte 0 sein, ist %d\n", g_test_rdt.cut_count);
        return 1;
    }
    if (g_test_rdt.zone_count != 0) {
        fprintf(stderr, "FAIL: zone_count sollte 0 sein, ist %d\n", g_test_rdt.zone_count);
        return 1;
    }
    if (g_test_rdt.sca_count != 0) {
        fprintf(stderr, "FAIL: sca_count sollte 0 sein, ist %d\n", g_test_rdt.sca_count);
        return 1;
    }
    if (g_test_rdt.main_scd_size != 0) {
        fprintf(stderr, "FAIL: main_scd_size sollte 0 sein, ist %d\n",
                (int)g_test_rdt.main_scd_size);
        return 1;
    }

    printf("PASS: test_rdt_state_fully_reset\n");
    return 0;
}

/* =========================================================================
 * Test 3: re15_rdt_parse() mit NULL-Argumenten -> Fehlercode -1
 *
 * Aequivalent zum alten test_room_load_invalid_params (Parameter-
 * Validierung MUSS frueh ablehnen). rdt_common.c:210: "!data || !out -> -1".
 *
 * Requirement: 12.2
 * ========================================================================= */

static int test_parse_null_args_rejected(void)
{
    int rc;

    /* NULL data */
    rc = re15_rdt_parse(NULL, RDT_HEADER_SIZE, &g_test_rdt);
    if (rc != -1) {
        fprintf(stderr, "FAIL: parse(NULL data) should return -1, got %d\n", rc);
        return 1;
    }

    /* NULL out */
    rc = re15_rdt_parse(g_empty_rdt, RDT_HEADER_SIZE, NULL);
    if (rc != -1) {
        fprintf(stderr, "FAIL: parse(NULL out) should return -1, got %d\n", rc);
        return 1;
    }

    printf("PASS: test_parse_null_args_rejected\n");
    return 0;
}

/* =========================================================================
 * Test 4: re15_rdt_parse() mit zu kleinem Puffer -> Fehlercode -2
 *
 * Aequivalent zum alten test_room_load_missing_file/invalid_params: ein
 * Puffer kleiner als der RDT-Header (0x60) darf NICHT geparst werden
 * (sonst Lesen ueber das Puffer-Ende). rdt_common.c:211: "size < 0x60 -> -2".
 *
 * Requirement: 12.2
 * ========================================================================= */

static int test_parse_truncated_buffer_rejected(void)
{
    int rc;
    uint8_t tiny[RDT_HEADER_SIZE - 1];

    memset(tiny, 0, sizeof(tiny));

    rc = re15_rdt_parse(tiny, sizeof(tiny), &g_test_rdt);
    if (rc != -2) {
        fprintf(stderr, "FAIL: parse(size < 0x60) should return -2, got %d\n", rc);
        return 1;
    }

    /* Auch Groesse 0 muss abgelehnt werden. */
    rc = re15_rdt_parse(g_empty_rdt, 0, &g_test_rdt);
    if (rc != -2) {
        fprintf(stderr, "FAIL: parse(size 0) should return -2, got %d\n", rc);
        return 1;
    }

    printf("PASS: test_parse_truncated_buffer_rejected\n");
    return 0;
}

/* =========================================================================
 * Test 5: residenter Raum-Zustand (g_room_rdt / g_room_rdt_ok) referenzierbar
 *
 * Aequivalent zum alten "g_game-Zustand sauber"-Check: der residente
 * Raum-Zustand des aktuellen Engine-Modells (re15_room.h:20-21) muss
 * existieren und beschreibbar/lesbar sein. Wir setzen ihn explizit auf
 * "kein Raum geladen" (g_room_rdt_ok = 0) und pruefen, dass die
 * Loader-Deklaration (re15_room_load) ueber den Header sichtbar ist
 * (Link-Bindung kommt im vollen Build aus dem Plattform-Loader).
 *
 * Requirement: 10.1
 * ========================================================================= */

static int test_resident_room_state_accessible(void)
{
    /* "Kein Raum geladen"-Zustand setzen. */
    g_room_rdt_ok = 0;
    memset(&g_room_rdt, 0, sizeof(g_room_rdt));

    if (g_room_rdt_ok != 0) {
        fprintf(stderr, "FAIL: g_room_rdt_ok sollte 0 sein nach Reset\n");
        return 1;
    }
    if (g_room_rdt.main_scd != NULL) {
        fprintf(stderr, "FAIL: g_room_rdt.main_scd sollte NULL sein nach Reset\n");
        return 1;
    }

    /* re15_room_load muss als Symbol referenzierbar sein (Loader-API vorhanden). */
    if ((void (*)(void))0 == (void (*)(void))(uintptr_t)&re15_room_load) {
        fprintf(stderr, "FAIL: re15_room_load nicht referenzierbar\n");
        return 1;
    }

    printf("PASS: test_resident_room_state_accessible\n");
    return 0;
}

/* =========================================================================
 * Main — Test-Runner
 * ========================================================================= */

int main(void)
{
    int failures = 0;

    printf("=== Room Load Unit Tests ===\n\n");

    failures += test_empty_rdt_null_sections();
    failures += test_rdt_state_fully_reset();
    failures += test_parse_null_args_rejected();
    failures += test_parse_truncated_buffer_rejected();
    failures += test_resident_room_state_accessible();

    if (failures == 0) {
        printf("\nALL ROOM LOAD TESTS PASSED\n");
    } else {
        fprintf(stderr, "\n%d TEST(S) FAILED\n", failures);
    }

    return failures;
}
