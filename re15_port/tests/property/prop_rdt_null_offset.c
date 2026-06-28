/**
 * @file prop_rdt_null_offset.c
 * @brief Property-Test: RDT Null-Offset-Sektionsbehandlung
 *
 * **Validates: Requirements 10.8**
 *
 * Property 15: Für jede RDT-Datei mit beliebiger Kombination von Null-Offsets
 * (0x00000000) in der Adresstabelle soll der Parser diese Sektionen als nicht
 * vorhanden behandeln (Pointer = NULL setzen) und alle übrigen Sektionen
 * korrekt parsen, ohne Absturz oder undefiniertes Verhalten.
 *
 * ---------------------------------------------------------------------------
 * API-MIGRATION (Engine-Transplant, re15_rdt.h / rdt_common.c, 2026-06):
 * ---------------------------------------------------------------------------
 * Der frühere Parser exportierte eine UNIFORME 21-Einträge-Adresstabelle, bei
 * der jeder Offset 0x08..0x5C 1:1 auf ein flaches struct-Pointer-Feld abbildete
 * (snd0_edt, snd0_vh, ..., collision, camera, zone, light, md1_ptr, floor,
 * block, message, main_scd, sub_scd, extra_scd, effect, esp_tim, model_tim,
 * animation) und die Konstante RE15_RDT_SECTION_COUNT (=21) definierte.
 *
 * Der AKTUELLE Parser (re15_rdt_parse @ rdt_common.c) hat KEINE uniforme
 * 21:1-Tabelle mehr. Stattdessen:
 *   - RE15_RDT_SECTION_COUNT ist ENTFERNT.
 *   - Viele Sektionen werden mit sektions-SPEZIFISCHER Semantik geparst und
 *     sind NICHT mehr (buffer + offset):
 *       * camera  -> cuts (re15_camera_cut_t*, gated über header nCut*32)
 *       * collision-> sca  (data + offset + 24-B-Header, gated über count[5])
 *       * zone    -> zones[] (zur Parse-Zeit in AABB/Quad konvertiert)
 *       * light   -> lights (data + offset, GRÖSSE = cut_count*40, gated)
 *       * message -> messages (data + offset, sized bis nächste Boundary)
 *       * main_scd-> main_scd = data + offset + first_off (POINTER-VERSCHOBEN)
 *       * sub_scd -> sub_scd[] (pointer-table, NICHT buffer+offset)
 *   - md1_ptr / extra_scd / effect / esp_tim / model_tim / animation sind
 *     ERSATZLOS ENTFERNT (bzw. als prop_tim[]/prop_md1[] aus der 0x30-Tabelle
 *     neu modelliert, ebenfalls keine 1:1-Offset-Abbildung).
 *   - block -> blocks bleibt KANONISCH IMMER NULL (block.blk @0x38 wird vom
 *     Original nie dereferenziert), taugt also NICHT als "valid offset"-Probe.
 *
 * Es gibt aber Sektionen, bei denen die GETESTETE PROPERTY exakt erhalten ist
 * — Pointer == (buffer + offset) bei gültigem Offset, NULL bei Null-Offset:
 *   - snd_edt[0/1], snd_vh[0/1], snd_vb[0/1]  (offsets 0x08..0x1c)
 *       -> alt: snd0_edt/snd0_vh/snd0_vb + snd1_edt/snd1_vh/snd1_vb
 *       Quelle: parse_audio() — `out->snd_*[b] = data + off` wenn off!=0 && off<size.
 *   - flr  (offset 0x34)   -> alt: floor
 *       Quelle: re15_rdt_parse — `out->flr = data + flr_start` wenn !=0 && +2<=size.
 *
 * Dieser Test prüft die ursprüngliche Property GENAU auf diesen 7 Pointern, die
 * die "Null-Offset -> NULL, gültiger Offset -> buffer+offset"-Garantie 1:1
 * erhalten haben, getrieben durch die ÖFFENTLICHE Funktion re15_rdt_parse().
 * Damit bleibt die Test-INTENTION (Null-Offset-Robustheit + korrekte
 * Pointer-Auflösung + kein Absturz bei beliebigen Null-Mustern) erhalten,
 * ohne entfernte interne Felder zu referenzieren.
 *
 * Teststrategie:
 * - Generiere synthetische RDT-Buffer (0x60 Header + Datenbereich)
 * - Für N Iterationen (200): setze zufällig einige der 7 geprobten Offsets auf
 *   0x00000000, andere auf gültige Offsets innerhalb des Puffers
 * - Prüfe: Null-Offset → Pointer == NULL
 * - Prüfe: Gültiger Offset → Pointer == buffer + offset
 * - Prüfe: Kein Absturz / re15_rdt_parse == 0 bei beliebigen Null-Mustern
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "re15_rdt.h"

/* =========================================================================
 * Test-Infrastruktur
 * ========================================================================= */

static int g_passed = 0;
static int g_failed = 0;

#define PROP_ASSERT(cond, fmt, ...)                                          \
    do {                                                                      \
        if (!(cond)) {                                                        \
            printf("  FAIL: " fmt "\n", ##__VA_ARGS__);                       \
            printf("        at %s:%d\n", __FILE__, __LINE__);                 \
            g_failed++;                                                        \
            return 1;                                                          \
        }                                                                      \
    } while (0)

/* =========================================================================
 * PRNG — Einfacher xorshift32 für reproduzierbare Zufallszahlen
 * ========================================================================= */

static uint32_t prng_state = 0;

static void prng_seed(uint32_t seed)
{
    prng_state = seed ? seed : 1;
}

static uint32_t prng_next(void)
{
    uint32_t x = prng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    prng_state = x;
    return x;
}


/* =========================================================================
 * Hilfsfunktion: Little-Endian 32-Bit-Wert in Buffer schreiben
 * ========================================================================= */

static void write_le32(uint8_t* p, uint32_t val)
{
    p[0] = (uint8_t)(val & 0xFF);
    p[1] = (uint8_t)((val >> 8) & 0xFF);
    p[2] = (uint8_t)((val >> 16) & 0xFF);
    p[3] = (uint8_t)((val >> 24) & 0xFF);
}

/* =========================================================================
 * Konstanten
 * ========================================================================= */

/** Puffergröße: 0x60 Bytes Header/Tabelle + 256 Bytes Datenbereich */
#define TEST_BUFFER_SIZE  (0x60 + 256)

/** Beginn des Datenbereichs (nach der Adresstabelle) */
#define DATA_AREA_START   0x60

/** Anzahl der Property-Test-Iterationen */
#define NUM_ITERATIONS    200

/* -------------------------------------------------------------------------
 * Die 7 geprobten Sektionen, deren "Null-Offset -> NULL / gültiger Offset ->
 * buffer+offset"-Property im aktuellen Parser exakt erhalten ist.
 *
 * `rdt_off` = Byte-Offset des u32-Offsets in der RDT-Adresstabelle.
 * Die Pointer werden zur Laufzeit aus dem geparsten re15_rdt_t gelesen (siehe
 * collect_probes()), weil sie in Arrays (snd_*) bzw. einem Skalar (flr) liegen.
 * ------------------------------------------------------------------------- */
typedef struct {
    const char *name;     /* Diagnose-Name                                    */
    uint32_t    rdt_off;  /* Offset des u32 in der RDT-Header-Tabelle          */
} probe_desc_t;

/* Reihenfolge + RDT-Offsets exakt wie in parse_audio() / re15_rdt_parse():
 *   snd0: EDT@0x08 VH@0x0c VB@0x10   snd1: EDT@0x14 VH@0x18 VB@0x1c   FLR@0x34 */
static const probe_desc_t PROBES[] = {
    { "snd_edt[0]", 0x08 },
    { "snd_vh[0]",  0x0c },
    { "snd_vb[0]",  0x10 },
    { "snd_edt[1]", 0x14 },
    { "snd_vh[1]",  0x18 },
    { "snd_vb[1]",  0x1c },
    { "flr",        0x34 },
};

#define NUM_PROBES ((int)(sizeof(PROBES) / sizeof(PROBES[0])))

/* Liest die 7 geprobten Pointer aus dem geparsten rdt in ein flaches Array,
 * in derselben Reihenfolge wie PROBES[]. */
static void collect_probes(const re15_rdt_t *rdt, const uint8_t *ptrs_out[NUM_PROBES])
{
    ptrs_out[0] = rdt->snd_edt[0];
    ptrs_out[1] = rdt->snd_vh[0];
    ptrs_out[2] = rdt->snd_vb[0];
    ptrs_out[3] = rdt->snd_edt[1];
    ptrs_out[4] = rdt->snd_vh[1];
    ptrs_out[5] = rdt->snd_vb[1];
    ptrs_out[6] = rdt->flr;
}

/* =========================================================================
 * Property-Test: Eine Iteration mit zufälligem Null-Offset-Muster
 * ========================================================================= */

/**
 * Führt eine Iteration durch:
 * 1. Erstellt einen synthetischen RDT-Buffer (gültiger 0x60-Header)
 * 2. Setzt zufällig einige der geprobten Offsets auf 0, andere auf gültige Werte
 * 3. Parst den Buffer mit re15_rdt_parse (öffentliche API)
 * 4. Verifiziert die 7 geprobten Sektions-Pointer
 *
 * @param iteration  Aktuelle Iterationsnummer (für Fehlerausgabe)
 * @param null_mask  Bitmaske: Bit i gesetzt → Probe i wird auf 0 gesetzt
 * @return 0 bei Erfolg, 1 bei Fehler
 */
static int test_iteration(int iteration, uint32_t null_mask)
{
    uint8_t buffer[TEST_BUFFER_SIZE];
    re15_rdt_t rdt;
    int result;
    int i;
    uint32_t offsets[NUM_PROBES];
    const uint8_t *ptrs[NUM_PROBES];

    /* Buffer mit nicht-null Wert füllen (erleichtert Debugging) */
    memset(buffer, 0xAB, TEST_BUFFER_SIZE);

    /* Header-Bytes (0x00-0x06): die Zähler. Auf 0 setzen, damit count-gegatete
     * Sektionen (cuts/light) garantiert deaktiviert sind und NICHT in den von
     * uns belegten Datenbereich hineinparsen / Boundaries verschieben. Die hier
     * geprobten Sektionen (Audio/FLR) sind von diesen Zählern unabhängig. */
    buffer[0x00] = 0x00; /* nSprite */
    buffer[0x01] = 0x00; /* nCut    */
    buffer[0x02] = 0x00; /* nOmodel */
    buffer[0x03] = 0x00; /* nItem   */
    buffer[0x04] = 0x00; /* nDoor   */
    buffer[0x05] = 0x00; /* nRoom_at*/
    buffer[0x06] = 0x00; /* reverb  */
    buffer[0x07] = 0x00;

    /* GESAMTE Adresstabelle (0x08..0x5c) zunächst auf 0 → alle nicht-geprobten
     * Sektionen sind "nicht vorhanden" und können den Test nicht stören. */
    for (i = 0x08; i <= 0x5c; i += 4) {
        write_le32(buffer + i, 0x00000000);
    }

    /* Die 7 geprobten Offsets setzen */
    for (i = 0; i < NUM_PROBES; i++) {
        if (null_mask & (1u << i)) {
            /* Dieser Offset ist 0 → Sektion nicht vorhanden */
            offsets[i] = 0x00000000;
        } else {
            /* Gültiger Offset: zeigt in den Datenbereich.
             * Verteile die Offsets über den Datenbereich. Muss strikt < size
             * sein (parse_audio: off<size; flr: off+2<=size) — bei i*12 max
             * 0x60+72 < 0x160, passt. */
            offsets[i] = DATA_AREA_START + (uint32_t)(i * 12);
        }
        write_le32(buffer + PROBES[i].rdt_off, offsets[i]);
    }

    /* Parser aufrufen (öffentliche API) */
    result = re15_rdt_parse(buffer, TEST_BUFFER_SIZE, &rdt);

    PROP_ASSERT(result == 0,
        "Iteration %d (mask=0x%08X): re15_rdt_parse returned %d (expected 0)",
        iteration, null_mask, result);

    /* Geprobte Pointer einsammeln */
    collect_probes(&rdt, ptrs);

    /* Verifikation: Jede geprobte Sektion prüfen */
    for (i = 0; i < NUM_PROBES; i++) {
        if (null_mask & (1u << i)) {
            /* Null-Offset → Pointer muss NULL sein */
            PROP_ASSERT(ptrs[i] == NULL,
                "Iteration %d (mask=0x%08X): Sektion %d (%s) hat Null-Offset "
                "aber Pointer ist nicht NULL (ptr=%p)",
                iteration, null_mask, i, PROBES[i].name, (void*)ptrs[i]);
        } else {
            /* Gültiger Offset → Pointer muss buffer + offset sein */
            const uint8_t* expected = buffer + offsets[i];
            PROP_ASSERT(ptrs[i] == expected,
                "Iteration %d (mask=0x%08X): Sektion %d (%s) hat Offset 0x%08X "
                "aber Pointer=%p (expected %p, buffer=%p)",
                iteration, null_mask, i, PROBES[i].name, offsets[i],
                (void*)ptrs[i], (void*)expected, (void*)buffer);
        }
    }

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property-Test: Spezialfälle (Extremwerte)
 * ========================================================================= */

/** Bitmaske mit allen NUM_PROBES Bits gesetzt. */
#define ALL_PROBES_MASK  ((1u << NUM_PROBES) - 1u)

/**
 * Teste: Alle geprobten Offsets sind NULL → alle Pointer müssen NULL sein.
 */
static int test_all_null(void)
{
    return test_iteration(-1, ALL_PROBES_MASK);
}

/**
 * Teste: Kein Offset ist NULL → alle Pointer müssen gültig sein.
 */
static int test_none_null(void)
{
    return test_iteration(-2, 0x00000000);
}

/**
 * Teste: Nur jeweils ein einzelner Offset ist NULL (NUM_PROBES Tests).
 */
static int test_single_null(void)
{
    int i;
    for (i = 0; i < NUM_PROBES; i++) {
        uint32_t mask = 1u << i;
        if (test_iteration(-100 - i, mask) != 0) {
            return 1;
        }
    }
    return 0;
}

/* =========================================================================
 * Main — Randomisierter Property-Test
 * ========================================================================= */

int main(void)
{
    int i;
    uint32_t seed;
    uint32_t null_mask;

    printf("=== Property-Test: RDT Null-Offset-Sektionsbehandlung ===\n");
    printf("    Validates: Requirements 10.8\n");
    printf("    Geprobte Sektionen (buffer+offset-erhaltend): %d\n\n", NUM_PROBES);

    /* Deterministischer Seed (reproduzierbar), kann via Env überschrieben werden */
    seed = (uint32_t)time(NULL);
    printf("    PRNG Seed: %u\n\n", seed);
    prng_seed(seed);

    /* --- Phase 1: Spezialfälle --- */
    printf("[1/4] Teste Spezialfall: Alle Offsets NULL...\n");
    if (test_all_null() != 0) {
        printf("    FEHLER!\n");
        goto done;
    }
    printf("    OK.\n\n");

    printf("[2/4] Teste Spezialfall: Kein Offset NULL...\n");
    if (test_none_null() != 0) {
        printf("    FEHLER!\n");
        goto done;
    }
    printf("    OK.\n\n");

    printf("[3/4] Teste Spezialfall: Jeweils ein einzelner Offset NULL...\n");
    if (test_single_null() != 0) {
        printf("    FEHLER!\n");
        goto done;
    }
    printf("    OK (%d Einzeltests bestanden).\n\n", NUM_PROBES);

    /* --- Phase 2: Randomisierte Tests --- */
    printf("[4/4] Teste %d zufaellige Null-Offset-Kombinationen...\n", NUM_ITERATIONS);
    for (i = 0; i < NUM_ITERATIONS; i++) {
        /* Generiere zufällige Bitmaske für NUM_PROBES Bits */
        null_mask = prng_next() & ALL_PROBES_MASK;
        if (test_iteration(i, null_mask) != 0) {
            printf("    FEHLER bei Iteration %d (mask=0x%08X)!\n", i, null_mask);
            goto done;
        }
    }
    printf("    %d Iterationen bestanden.\n\n", NUM_ITERATIONS);

done:
    /* Zusammenfassung */
    printf("=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
           g_passed, g_failed);

    return (g_failed > 0) ? 1 : 0;
}
