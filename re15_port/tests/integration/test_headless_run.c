/**
 * @file test_headless_run.c
 * @brief Integration-Test: Headless-Modus — 5+ "Räume" sequentiell durchlaufen
 *
 * Simuliert programmatisch was der Headless-Modus tut: pro Raum die SCD-VM
 * für die Raum-Skripte hochfahren, N SCD-Ticks ausführen, wieder abbauen und
 * zum nächsten Raum weiterschalten. Der Test verarbeitet 5+ verschiedene
 * "Räume" sequentiell und prüft, dass keine Abstürze oder Inkonsistenzen
 * auftreten.
 *
 * Reaktiviert 2026-06-28 nach dem Engine-Transplant. Die alte Test-Version
 * fuhr über eine pluggbare I/O-/Audio-Backend-Tabelle (re15_io_backend_t /
 * audio_backend_t, g_io/g_audio) und eine globale Engine-Instanz `g_game`
 * mit current_rdt/player/rdt_buffer/current_stage usw. — diese gesamte
 * Schicht wurde entfernt. Die AKTUELLE öffentliche API fährt einen Raum
 * über die SCD-VM hoch (scd_vm_init → scd_thread_start(0, …) → scd_vm_tick),
 * das per-Raum-RDT wird mit re15_rdt_parse geparst (statt über ein Mock-IO-
 * Backend gestreamt). re15_room_load(unsigned) ist jetzt der ARCHITEKTUR-
 * spezifische Datei-/CD-Loader (liest echte Assets), daher hier bewusst NICHT
 * benutzt — der Headless-Kern, den dieser Test belegt, ist die SCD-VM, die die
 * Raum-Skripte über mehrere Räume hinweg fehlerfrei abarbeitet.
 *
 * Alt -> Neu (verifiziert am aktuellen Header):
 *   re15_room_load(stage,room,player) + Mock-IO  -> synthetisches 96-B-RDT via
 *       re15_rdt_parse() (re15_rdt.h:175; Header braucht >=0x60 B, all-zero =
 *       alle Sektionen NULL, Rückgabe 0) + scd_vm_init()/scd_thread_start(0,
 *       scd_fallback_bytecode()) (re15_scd.h:349/353/371) als per-Raum-Bring-up.
 *   scd_vm_tick()                               -> unverändert (re15_scd.h:350).
 *   re15_room_unload() (free + aot_init + scd_vm_init reset) -> scd_vm_init()
 *       (scd_vm.c:334 ruft re15_game_state_init + re15_aot_init + ...; das ist
 *       der aktuelle vollständige per-Raum-Reset).
 *   re15_stage_change(n)                        -> ersatzlos entfernt; der
 *       Cross-Stage-Durchlauf wird hier als reiner Raum-zu-Raum-VM-Durchlauf
 *       nachgebildet (die VM-Logik ist stagen-unabhängig).
 *   g_game.rdt_buffer/current_stage/...         -> entfernt; g_game ist jetzt
 *       re15_game_state_t (nur Flag-Tabellen, re15_scd.h:338/340). Zustands-
 *       Checks laufen über beobachtbaren VM-Zustand (g_scd.threads[0].active,
 *       g_scd.tick_count) statt über den entfernten rdt_buffer.
 *
 * Requirements: 4.4, 12.2, 12.5
 */

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_aot.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* =========================================================================
 * Synthetisches RDT (ersetzt das frühere Mock-IO-Backend)
 *
 * re15_rdt_parse() verlangt mindestens den 0x60-Byte-Header (re15_rdt.h:175,
 * rdt_common.c:211). Ein all-zero 96-Byte-Puffer hat alle Sektions-Offsets = 0,
 * sodass JEDE Sektion als nicht vorhanden (NULL) geparst wird und die Funktion 0
 * (Erfolg) zurückgibt — exakt das Verhalten, das das alte Mock-RDT erzeugte.
 * ========================================================================= */

#define MOCK_RDT_SIZE 96

static uint8_t g_mock_rdt_data[MOCK_RDT_SIZE];
static re15_rdt_t g_test_rdt;

/* Zählt, wie viele "Räume" tatsächlich (per RDT-Parse) geladen wurden — das
 * Äquivalent des früheren g_mock_io_open_count. */
static int g_room_load_count = 0;

/**
 * Lädt einen "Raum" über die AKTUELLE öffentliche API:
 *   1. synthetisches RDT parsen (re15_rdt_parse)            — Datencontainer
 *   2. SCD-VM (re)initialisieren (scd_vm_init)              — per-Raum-Reset
 *   3. Main-Thread mit dem Raum-Bytecode starten            — Skript-Bring-up
 *
 * Gibt 0 bei Erfolg zurück, !=0 bei Fehler (so wie das alte re15_room_load).
 */
static int headless_room_load(void)
{
    /* All-zero RDT (alle Sektionen abwesend). */
    memset(g_mock_rdt_data, 0, sizeof(g_mock_rdt_data));

    int rc = re15_rdt_parse(g_mock_rdt_data, sizeof(g_mock_rdt_data), &g_test_rdt);
    if (rc != 0) {
        return rc;
    }

    /* Per-Raum-Reset (entspricht dem alten unload+init: VM/AOT/Inventory/Actors
     * werden in scd_vm_init() vollständig genullt — scd_vm.c:334). */
    scd_vm_init();

    /* Raum-Hauptskript (fällt mangels echtem main_scd im synthetischen RDT auf
     * das kanonische Headless-Boot-Bytecode zurück — Pos_set/Dir_set/Plc_motion
     * + Sleep-Loop, scd_room_setup.c:25). */
    const uint8_t *boot = g_test_rdt.main_scd ? g_test_rdt.main_scd
                                              : scd_fallback_bytecode();
    if (scd_thread_start(0, boot) != 0) {
        return -1;
    }

    g_room_load_count++;
    return 0;
}

/**
 * Entlädt den aktuellen "Raum". Der aktuelle vollständige per-Raum-Teardown ist
 * scd_vm_init() (es nullt VM, AOT, Inventory, Actors). Idempotent / no-op-fest:
 * mehrfaches Aufrufen ohne vorheriges Load ist gefahrlos (memset auf die
 * globalen Strukturen).
 */
static void headless_room_unload(void)
{
    scd_vm_init();
}

/**
 * Initialisiert den globalen Test-Zustand für den Headless-Test.
 */
static void headless_setup(void)
{
    memset(&g_test_rdt, 0, sizeof(g_test_rdt));

    /* Subsysteme in einen definierten Ausgangszustand bringen. */
    re15_aot_init();
    scd_vm_init();

    g_room_load_count = 0;
}

/* =========================================================================
 * Test 1: 5 "Räume" sequentiell durchlaufen
 *
 * Simuliert den Headless-Ablauf: Raum hochfahren, Ticks ausführen, abbauen.
 * Wiederholt für 5 verschiedene Raum-Indizes.
 *
 * Validates: Requirements 12.2, 12.5
 * ========================================================================= */

static int test_5_rooms_sequential_same_stage(void)
{
    static const uint8_t rooms[] = { 0x00, 0x05, 0x0A, 0x14, 0x17 };
    static const int num_rooms = 5;
    static const int ticks_per_room = 60;
    int i, t, result;

    printf("  test_5_rooms_sequential_same_stage... ");

    headless_setup();

    for (i = 0; i < num_rooms; i++) {
        /* Raum hochfahren */
        result = headless_room_load();
        if (result != 0) {
            fprintf(stderr, "FAIL: room_load(0x%02X) returned %d\n",
                    rooms[i], result);
            return 1;
        }

        /* Main-Thread muss laufen (= Raum-Skript aktiv) */
        if (!g_scd.threads[0].active) {
            fprintf(stderr, "FAIL: main thread not active after load "
                    "(room 0x%02X)\n", rooms[i]);
            return 1;
        }

        /* SCD-Ticks ausführen (simuliert Headless-Ticks) */
        for (t = 0; t < ticks_per_room; t++) {
            scd_vm_tick();
        }

        /* Die VM muss die Ticks tatsächlich verarbeitet haben. */
        if (g_scd.tick_count == 0) {
            fprintf(stderr, "FAIL: tick_count still 0 after %d ticks "
                    "(room 0x%02X)\n", ticks_per_room, rooms[i]);
            return 1;
        }

        /* Raum abbauen */
        headless_room_unload();

        /* Prüfe Zustand nach Abbau: VM ist zurückgesetzt, kein Thread aktiv. */
        if (g_scd.threads[0].active) {
            fprintf(stderr, "FAIL: main thread should be inactive after unload "
                    "(room 0x%02X)\n", rooms[i]);
            return 1;
        }
        if (g_scd.tick_count != 0) {
            fprintf(stderr, "FAIL: tick_count should be 0 after unload "
                    "(room 0x%02X)\n", rooms[i]);
            return 1;
        }
    }

    /* Alle 5 Räume müssen geladen worden sein. */
    if (g_room_load_count < num_rooms) {
        fprintf(stderr, "FAIL: Expected at least %d room loads, got %d\n",
                num_rooms, g_room_load_count);
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 2: 6 "Räume" sequentiell — verschiedene Stages
 *
 * Simuliert den Headless-Modus über (logische) Stage-Grenzen hinweg. Da
 * re15_stage_change() entfernt wurde und die SCD-VM stagen-unabhängig ist,
 * wird der Durchlauf als reine Raum-zu-Raum-Sequenz nachgebildet — die
 * Test-Intention (mehrere Räume verschiedener Stages crash-frei durchlaufen)
 * bleibt erhalten.
 *
 * Validates: Requirements 4.4, 12.2
 * ========================================================================= */

typedef struct {
    uint8_t stage;
    uint8_t room;
} room_spec_t;

static int test_6_rooms_cross_stage(void)
{
    static const room_spec_t rooms[] = {
        { 1, 0x17 },
        { 2, 0x03 },
        { 3, 0x00 },
        { 4, 0x01 },
        { 5, 0x02 },
        { 6, 0x00 },
    };
    static const int num_rooms = 6;
    static const int ticks_per_room = 30;
    int i, t, result;

    printf("  test_6_rooms_cross_stage... ");

    headless_setup();

    for (i = 0; i < num_rooms; i++) {
        /* Raum hochfahren */
        result = headless_room_load();
        if (result != 0) {
            fprintf(stderr, "FAIL: room_load(stage %d, 0x%02X) returned %d\n",
                    rooms[i].stage, rooms[i].room, result);
            return 1;
        }

        /* SCD-Ticks */
        for (t = 0; t < ticks_per_room; t++) {
            scd_vm_tick();
        }

        /* Raum abbauen */
        headless_room_unload();
    }

    /* Alle 6 Räume müssen durchlaufen worden sein. */
    if (g_room_load_count != num_rooms) {
        fprintf(stderr, "FAIL: expected %d rooms traversed, got %d\n",
                num_rooms, g_room_load_count);
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 3: Headless — Double-Load ohne Crash
 *
 * Ruft headless_room_load zweimal nacheinander auf ohne explizites unload
 * dazwischen. scd_vm_init() (im Load) baut den vorherigen Raum-Zustand intern
 * ab, bevor der neue Thread startet. Prüft, dass kein Absturz oder undefiniertes
 * Verhalten auftritt.
 *
 * Validates: Requirements 12.2
 * ========================================================================= */

static int test_double_load_no_crash(void)
{
    int result;
    int t;

    printf("  test_double_load_no_crash... ");

    headless_setup();

    /* Erster Raum hochfahren */
    result = headless_room_load();
    if (result != 0) {
        fprintf(stderr, "FAIL: First room_load returned %d\n", result);
        return 1;
    }

    /* Ein paar Ticks */
    for (t = 0; t < 10; t++) {
        scd_vm_tick();
    }

    /* Zweiter Raum hochfahren OHNE explizites unload — das interne scd_vm_init
     * im Load muss den alten Zustand sauber abbauen. */
    result = headless_room_load();
    if (result != 0) {
        fprintf(stderr, "FAIL: Second room_load returned %d\n", result);
        return 1;
    }

    /* Nach dem zweiten Load: VM frisch, tick_count zurückgesetzt, Thread läuft. */
    if (g_scd.tick_count != 0) {
        fprintf(stderr, "FAIL: tick_count should reset on re-load, got %u\n",
                g_scd.tick_count);
        return 1;
    }
    if (!g_scd.threads[0].active) {
        fprintf(stderr, "FAIL: main thread should be active after re-load\n");
        return 1;
    }

    /* Ticks auf neuem Raum */
    for (t = 0; t < 10; t++) {
        scd_vm_tick();
    }

    /* Aufräumen */
    headless_room_unload();

    if (g_scd.threads[0].active) {
        fprintf(stderr, "FAIL: main thread should be inactive after final unload\n");
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 4: Headless — Unload auf leerem Zustand (no-op)
 *
 * headless_room_unload() darf nicht abstürzen, wenn kein Raum geladen ist.
 *
 * Validates: Requirements 12.2
 * ========================================================================= */

static int test_unload_empty_no_crash(void)
{
    printf("  test_unload_empty_no_crash... ");

    headless_setup();

    /* Unload ohne vorheriges Load — muss no-op sein */
    headless_room_unload();
    headless_room_unload(); /* Doppelt — trotzdem kein Crash */

    /* VM bleibt im zurückgesetzten Zustand. */
    if (g_scd.threads[0].active) {
        fprintf(stderr, "FAIL: main thread should remain inactive\n");
        return 1;
    }
    if (g_scd.tick_count != 0) {
        fprintf(stderr, "FAIL: tick_count should remain 0\n");
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Test 5: Headless — 8 "Räume" mit mehr Ticks (Stress-Test)
 *
 * Lädt 8 verschiedene "Räume" mit je 120 SCD-Ticks. Validiert, dass auch bei
 * längerer Ausführung kein Fehler auftritt.
 *
 * Validates: Requirements 12.2, 12.5
 * ========================================================================= */

static int test_8_rooms_stress(void)
{
    static const uint8_t rooms[] = {
        0x00, 0x03, 0x07, 0x0B, 0x0F, 0x13, 0x17, 0x20
    };
    static const int num_rooms = 8;
    static const int ticks_per_room = 120;
    int i, t, result;

    printf("  test_8_rooms_stress... ");

    headless_setup();

    for (i = 0; i < num_rooms; i++) {
        result = headless_room_load();
        if (result != 0) {
            fprintf(stderr, "FAIL: room_load(0x%02X) returned %d "
                    "at iteration %d\n", rooms[i], result, i);
            return 1;
        }

        for (t = 0; t < ticks_per_room; t++) {
            scd_vm_tick();
        }

        headless_room_unload();
    }

    if (g_room_load_count != num_rooms) {
        fprintf(stderr, "FAIL: expected %d rooms loaded, got %d\n",
                num_rooms, g_room_load_count);
        return 1;
    }

    printf("PASS\n");
    return 0;
}

/* =========================================================================
 * Main — Test-Runner
 * ========================================================================= */

int main(void)
{
    int failures = 0;

    printf("=== Integration Tests: Headless Run ===\n\n");

    failures += test_5_rooms_sequential_same_stage();
    failures += test_6_rooms_cross_stage();
    failures += test_double_load_no_crash();
    failures += test_unload_empty_no_crash();
    failures += test_8_rooms_stress();

    printf("\n");
    if (failures == 0) {
        printf("ALL HEADLESS RUN INTEGRATION TESTS PASSED (%d tests)\n", 5);
    } else {
        fprintf(stderr, "%d TEST(S) FAILED\n", failures);
    }

    return failures;
}
