/**
 * test_main.c — Minimaler Test-Runner für RE 1.5 Port
 *
 * Stellt ein einfaches Test-Framework bereit:
 * - TEST_ASSERT Macro für Assertions
 * - Automatische Registrierung und Ausführung von Test-Funktionen
 * - Pass/Fail-Summary mit Exit-Code-Logik (0 = alle bestanden, 1 = Fehler)
 *
 * Requirements: 12.1, 12.5, 12.7
 */

#include <stdio.h>
#include <string.h>

/* =========================================================================
 * Test-Framework Definitionen
 * ========================================================================= */

/** Maximale Anzahl registrierbarer Tests */
#define TEST_MAX_TESTS 256

/** Assertion-Macro: Bricht den aktuellen Test bei Fehlschlag ab */
#define TEST_ASSERT(cond)                                                    \
    do {                                                                      \
        if (!(cond)) {                                                        \
            printf("  FAIL: %s\n", #cond);                                    \
            printf("        at %s:%d\n", __FILE__, __LINE__);                 \
            return 1;                                                          \
        }                                                                      \
    } while (0)

/** Assertion mit benutzerdefinierter Nachricht */
#define TEST_ASSERT_MSG(cond, msg)                                           \
    do {                                                                      \
        if (!(cond)) {                                                        \
            printf("  FAIL: %s\n", (msg));                                    \
            printf("        condition: %s\n", #cond);                         \
            printf("        at %s:%d\n", __FILE__, __LINE__);                 \
            return 1;                                                          \
        }                                                                      \
    } while (0)

/** Assertion für Integer-Gleichheit mit Soll/Ist-Ausgabe */
#define TEST_ASSERT_EQ(expected, actual)                                      \
    do {                                                                      \
        long long _exp = (long long)(expected);                               \
        long long _act = (long long)(actual);                                 \
        if (_exp != _act) {                                                   \
            printf("  FAIL: %s == %s\n", #expected, #actual);                 \
            printf("        expected: %lld, actual: %lld\n", _exp, _act);     \
            printf("        at %s:%d\n", __FILE__, __LINE__);                 \
            return 1;                                                          \
        }                                                                      \
    } while (0)

/** Test-Funktions-Signatur: Gibt 0 bei Erfolg, non-zero bei Fehler zurück */
typedef int (*test_func_t)(void);

/** Ein registrierter Test */
typedef struct {
    const char*  name;
    test_func_t  func;
} test_entry_t;

/* =========================================================================
 * Test-Registry
 * ========================================================================= */

static test_entry_t g_tests[TEST_MAX_TESTS];
static int          g_test_count = 0;

/**
 * Registriere einen Test. Wird typischerweise über das TEST_REGISTER Macro
 * oder manuell in test_register_all() aufgerufen.
 */
static void test_register(const char* name, test_func_t func)
{
    if (g_test_count < TEST_MAX_TESTS) {
        g_tests[g_test_count].name = name;
        g_tests[g_test_count].func = func;
        g_test_count++;
    } else {
        printf("[WARN] Test-Registry voll, kann '%s' nicht registrieren.\n", name);
    }
}

/* =========================================================================
 * Beispiel-Test (Platzhalter, wird durch echte Tests ersetzt)
 * ========================================================================= */

static int test_placeholder(void)
{
    /* Einfacher Selbsttest des Test-Frameworks */
    TEST_ASSERT(1 == 1);
    TEST_ASSERT_EQ(42, 42);
    return 0;
}

/* =========================================================================
 * Test-Registrierung
 * ========================================================================= */

/**
 * Registriere alle Tests hier.
 * Neue Test-Dateien fügen ihre Tests in dieser Funktion hinzu.
 */
static void test_register_all(void)
{
    test_register("placeholder", test_placeholder);

    /* Weitere Tests hier registrieren:
     * test_register("rdt_parser_basic", test_rdt_parser_basic);
     * test_register("scd_vm_nop", test_scd_vm_nop);
     * ...
     */
}

/* =========================================================================
 * Test-Runner
 * ========================================================================= */

int main(int argc, char** argv)
{
    int passed  = 0;
    int failed  = 0;
    int i;

    (void)argc;
    (void)argv;

    printf("=== RE 1.5 Port — Test Suite ===\n\n");

    test_register_all();

    if (g_test_count == 0) {
        printf("Keine Tests registriert.\n");
        return 0;
    }

    for (i = 0; i < g_test_count; i++) {
        printf("[%d/%d] %s ... ", i + 1, g_test_count, g_tests[i].name);

        int result = g_tests[i].func();

        if (result == 0) {
            printf("PASS\n");
            passed++;
        } else {
            printf("FAIL\n");
            failed++;
        }
    }

    /* Summary */
    printf("\n=== Ergebnis: %d/%d bestanden", passed, g_test_count);
    if (failed > 0) {
        printf(", %d fehlgeschlagen", failed);
    }
    printf(" ===\n");

    /* Exit-Code: 0 = alle bestanden, 1 = mindestens ein Fehler */
    return (failed > 0) ? 1 : 0;
}
