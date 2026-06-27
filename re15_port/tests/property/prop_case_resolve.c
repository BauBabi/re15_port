/**
 * @file prop_case_resolve.c
 * @brief Property-Test: Case-insensitive Pfadauflösung
 *
 * **Property 2: Case-insensitive Pfadauflösung**
 *
 * Für jede beliebige Groß-/Kleinschreibungs-Permutation eines gültigen
 * Dateinamens innerhalb eines Verzeichnisses soll re15_io_resolve_ci()
 * denselben physischen Dateipfad zurückliefern wie für den Original-Dateinamen.
 *
 * **Validates: Requirements 2.4**
 *
 * Vorgehen:
 * 1. Erstelle ein temporäres Verzeichnis mit einer bekannten Datei (z.B. "ROOM1170.RDT")
 * 2. Generiere N zufällige Groß-/Kleinschreibungs-Permutationen des Dateinamens
 * 3. Rufe re15_io_resolve_ci() für jede Permutation auf
 * 4. Prüfe dass alle Permutationen denselben Dateinamen liefern
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #include <direct.h>
  #define MKDIR(path) _mkdir(path)
  #define RMDIR(path) _rmdir(path)
#else
  #include <sys/stat.h>
  #include <unistd.h>
  #define MKDIR(path) mkdir(path, 0755)
  #define RMDIR(path) rmdir(path)
#endif

#include "re15_io_common.h"

/* =========================================================================
 * Test-Framework (minimale Inline-Version analog zu test_main.c)
 * ========================================================================= */

#define TEST_ASSERT(cond)                                                    \
    do {                                                                      \
        if (!(cond)) {                                                        \
            printf("  FAIL: %s\n", #cond);                                    \
            printf("        at %s:%d\n", __FILE__, __LINE__);                 \
            return 1;                                                          \
        }                                                                      \
    } while (0)

#define TEST_ASSERT_MSG(cond, msg)                                           \
    do {                                                                      \
        if (!(cond)) {                                                        \
            printf("  FAIL: %s\n", (msg));                                    \
            printf("        condition: %s\n", #cond);                         \
            printf("        at %s:%d\n", __FILE__, __LINE__);                 \
            return 1;                                                          \
        }                                                                      \
    } while (0)

/* =========================================================================
 * Konfiguration
 * ========================================================================= */

/** Anzahl der zufälligen Permutationen pro Testlauf */
#define NUM_ITERATIONS 100

/** Bekannter Dateiname zum Testen (typisch für RE1.5 Asset) */
static const char* KNOWN_FILENAME = "ROOM1170.RDT";

/* =========================================================================
 * Hilfsfunktionen
 * ========================================================================= */

/**
 * Generiere eine zufällige Groß-/Kleinschreibungs-Permutation von `src`
 * und schreibe das Ergebnis nach `dst`.
 *
 * Für jedes Zeichen wird zufällig entschieden, ob Groß- oder Kleinschreibung.
 * Nicht-alphabetische Zeichen (Ziffern, Punkt) bleiben unverändert.
 */
static void random_case_permutation(const char* src, char* dst, size_t max_len)
{
    size_t i;
    size_t len = strlen(src);

    if (len >= max_len) {
        len = max_len - 1;
    }

    for (i = 0; i < len; i++) {
        if (isalpha((unsigned char)src[i])) {
            if (rand() % 2 == 0) {
                dst[i] = (char)toupper((unsigned char)src[i]);
            } else {
                dst[i] = (char)tolower((unsigned char)src[i]);
            }
        } else {
            dst[i] = src[i];
        }
    }
    dst[len] = '\0';
}

/**
 * Erstelle ein temporäres Verzeichnis und lege eine Datei mit dem
 * angegebenen Namen darin an.
 *
 * @param tmp_dir     Ausgabepuffer für den Verzeichnispfad
 * @param max_len     Puffergröße
 * @param filename    Name der anzulegenden Datei
 * @return 0 bei Erfolg, -1 bei Fehler
 */
static int setup_temp_dir(char* tmp_dir, size_t max_len, const char* filename)
{
    char filepath[RE15_MAX_PATH];

#ifdef _WIN32
    /* Nutze %TEMP% oder Fallback */
    const char* tmp_base = getenv("TEMP");
    if (!tmp_base) {
        tmp_base = getenv("TMP");
    }
    if (!tmp_base) {
        tmp_base = ".";
    }

    snprintf(tmp_dir, max_len, "%s\\re15_pbt_ci_%u", tmp_base, (unsigned)GetCurrentProcessId());
#else
    snprintf(tmp_dir, max_len, "/tmp/re15_pbt_ci_%u", (unsigned)getpid());
#endif

    /* Verzeichnis erstellen (ignoriere Fehler falls bereits vorhanden) */
    MKDIR(tmp_dir);

    /* Datei erstellen */
#ifdef _WIN32
    snprintf(filepath, sizeof(filepath), "%s\\%s", tmp_dir, filename);
#else
    snprintf(filepath, sizeof(filepath), "%s/%s", tmp_dir, filename);
#endif

    FILE* f = fopen(filepath, "wb");
    if (!f) {
        printf("  ERROR: Kann Testdatei nicht erstellen: %s\n", filepath);
        return -1;
    }
    /* Schreibe minimalen Inhalt */
    fprintf(f, "TEST");
    fclose(f);

    return 0;
}

/**
 * Entferne die Testdatei und das temporäre Verzeichnis.
 */
static void cleanup_temp_dir(const char* tmp_dir, const char* filename)
{
    char filepath[RE15_MAX_PATH];

#ifdef _WIN32
    snprintf(filepath, sizeof(filepath), "%s\\%s", tmp_dir, filename);
    DeleteFileA(filepath);
    RemoveDirectoryA(tmp_dir);
#else
    snprintf(filepath, sizeof(filepath), "%s/%s", tmp_dir, filename);
    unlink(filepath);
    RMDIR(tmp_dir);
#endif
}

/* =========================================================================
 * Property-Test
 * ========================================================================= */

/**
 * Property 2: Case-insensitive Pfadauflösung
 *
 * Für jede zufällige Groß-/Kleinschreibungs-Permutation eines bekannten
 * Dateinamens liefert re15_io_resolve_ci() denselben physischen Dateinamen.
 *
 * **Validates: Requirements 2.4**
 */
static int prop_case_insensitive_resolve(void)
{
    char tmp_dir[RE15_MAX_PATH];
    char reference_out[RE15_MAX_PATH];
    char permutation[RE15_MAX_PATH];
    char resolved_out[RE15_MAX_PATH];
    int result;
    int i;

    /* Setup: Temporäres Verzeichnis mit bekannter Datei */
    result = setup_temp_dir(tmp_dir, sizeof(tmp_dir), KNOWN_FILENAME);
    TEST_ASSERT_MSG(result == 0, "Setup: Temporäres Verzeichnis konnte nicht erstellt werden");

    /* Referenz: Löse den originalen Dateinamen auf */
    result = re15_io_resolve_ci(tmp_dir, KNOWN_FILENAME, reference_out);
    if (result != 0) {
        cleanup_temp_dir(tmp_dir, KNOWN_FILENAME);
        TEST_ASSERT_MSG(0, "Referenz-Auflösung mit Original-Dateinamen fehlgeschlagen");
    }

    /* Property-Test: N zufällige Permutationen müssen denselben Namen liefern */
    for (i = 0; i < NUM_ITERATIONS; i++) {
        random_case_permutation(KNOWN_FILENAME, permutation, sizeof(permutation));

        memset(resolved_out, 0, sizeof(resolved_out));
        result = re15_io_resolve_ci(tmp_dir, permutation, resolved_out);

        if (result != 0) {
            printf("  Iteration %d: Permutation '%s' konnte nicht aufgelöst werden\n",
                   i, permutation);
            cleanup_temp_dir(tmp_dir, KNOWN_FILENAME);
            TEST_ASSERT_MSG(0, "Case-insensitive Auflösung fehlgeschlagen für eine Permutation");
        }

        /* Kernprüfung: Aufgelöster Name muss identisch zur Referenz sein */
        if (strcmp(resolved_out, reference_out) != 0) {
            printf("  Iteration %d: Permutation '%s'\n", i, permutation);
            printf("    Erwartet: '%s'\n", reference_out);
            printf("    Erhalten: '%s'\n", resolved_out);
            cleanup_temp_dir(tmp_dir, KNOWN_FILENAME);
            TEST_ASSERT_MSG(0,
                "Aufgelöster Dateiname weicht von der Referenz ab — "
                "case-insensitive Eigenschaft verletzt");
        }
    }

    /* Cleanup */
    cleanup_temp_dir(tmp_dir, KNOWN_FILENAME);

    printf("  OK: %d Permutationen von '%s' liefern alle '%s'\n",
           NUM_ITERATIONS, KNOWN_FILENAME, reference_out);
    return 0;
}

/**
 * Zusatz-Property: NULL-Parameter verursachen keinen Absturz (Robustheit).
 */
static int prop_null_params_safe(void)
{
    char out[RE15_MAX_PATH];
    int result;

    /* NULL dir */
    result = re15_io_resolve_ci(NULL, "test.txt", out);
    TEST_ASSERT(result == -1);

    /* NULL filename */
    result = re15_io_resolve_ci(".", NULL, out);
    TEST_ASSERT(result == -1);

    /* NULL out */
    result = re15_io_resolve_ci(".", "test.txt", NULL);
    TEST_ASSERT(result == -1);

    return 0;
}

/**
 * Zusatz-Property: Nicht existierende Datei liefert -1.
 */
static int prop_nonexistent_file_returns_error(void)
{
    char tmp_dir[RE15_MAX_PATH];
    char out[RE15_MAX_PATH];
    int result;

    /* Setup: Leeres temporäres Verzeichnis */
    result = setup_temp_dir(tmp_dir, sizeof(tmp_dir), KNOWN_FILENAME);
    TEST_ASSERT_MSG(result == 0, "Setup fehlgeschlagen");

    /* Suche nach einer Datei die nicht existiert */
    result = re15_io_resolve_ci(tmp_dir, "NONEXISTENT_FILE.XYZ", out);
    TEST_ASSERT(result == -1);

    /* Cleanup */
    cleanup_temp_dir(tmp_dir, KNOWN_FILENAME);
    return 0;
}

/* =========================================================================
 * Main — Test-Runner
 * ========================================================================= */

int main(int argc, char** argv)
{
    int passed = 0;
    int failed = 0;

    (void)argc;
    (void)argv;

    /* Zufallsgenerator mit Zeit seeden */
    srand((unsigned int)time(NULL));

    printf("=== Property-Test: Case-insensitive Pfadauflösung ===\n");
    printf("    (Property 2, Validates: Requirements 2.4)\n\n");

    /* Test 1: Kern-Property */
    printf("[1/3] prop_case_insensitive_resolve (%d Iterationen) ... ", NUM_ITERATIONS);
    if (prop_case_insensitive_resolve() == 0) {
        printf("PASS\n");
        passed++;
    } else {
        printf("FAIL\n");
        failed++;
    }

    /* Test 2: NULL-Parameter-Robustheit */
    printf("[2/3] prop_null_params_safe ... ");
    if (prop_null_params_safe() == 0) {
        printf("PASS\n");
        passed++;
    } else {
        printf("FAIL\n");
        failed++;
    }

    /* Test 3: Nicht-existierende Datei */
    printf("[3/3] prop_nonexistent_file_returns_error ... ");
    if (prop_nonexistent_file_returns_error() == 0) {
        printf("PASS\n");
        passed++;
    } else {
        printf("FAIL\n");
        failed++;
    }

    /* Summary */
    printf("\n=== Ergebnis: %d/%d bestanden", passed, passed + failed);
    if (failed > 0) {
        printf(", %d fehlgeschlagen", failed);
    }
    printf(" ===\n");

    return (failed > 0) ? 1 : 0;
}
