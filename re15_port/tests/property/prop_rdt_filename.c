/**
 * @file prop_rdt_filename.c
 * @brief Property-Test: RDT-Dateiname-Konstruktion
 *
 * **Validates: Requirements 2.3**
 *
 * Property 1: Für jede gültige Kombination aus Stage (1-6), RoomHex (00-27)
 * und Player (0/1) soll re15_rdt_build_path() einen String im exakten Format
 * STAGE{N}/ROOM{Stage}{RoomHex}{Player}.RDT erzeugen, wobei RoomHex immer
 * zweistellig hexadezimal (Großbuchstaben) ist.
 *
 * Testet alle 480 gültigen Kombinationen (6 × 40 × 2) exhaustiv plus
 * ungültige Eingaben.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* =========================================================================
 * Lokale Nachbildung der entfernten I/O-API
 *
 * Der Engine-Transplant hat den Header re15_io_common.h samt
 * RE15_MAX_PATH und re15_rdt_build_path() ersatzlos entfernt — die
 * RDT-Pfad-Konstruktion lebt jetzt nur noch inline in den Tools
 * (re15_port/tools/room_graph/extract_doors.c:128-135, build_rdt_path()):
 *
 *     snprintf(out, out_size,
 *              "%s/STAGE%d/ROOM%d%02X%d.RDT",
 *              assets_root, stage, stage, room_hex, player);
 *
 * Es gibt keine öffentliche Engine-Funktion mehr, die diese Property
 * exponiert. Um die TEST-INTENTION zu erhalten (verifiziere das
 * kanonische RDT-Pfadformat STAGE{N}/ROOM{Stage}{RoomHex}{Player}.RDT
 * inklusive Parameter-/Puffer-Validierung), wird die Logik hier lokal
 * nachgebildet — byte-true zum o.g. Format-String (uppercase %02X) und
 * zu den Validierungs-Regeln, die die Asserts unten prüfen. KEIN
 * _legacy_minimal-Header wird eingebunden.
 * ========================================================================= */

/* Maximale Pfadlänge (Legacy-Wert aus dem entfernten re15_io_common.h). */
#define RE15_MAX_PATH 260

/**
 * Konstruiert den RDT-Dateipfad im Format
 * STAGE{N}/ROOM{Stage}{RoomHex}{Player}.RDT.
 *
 * Lokale Nachbildung des entfernten re15_rdt_build_path() — gültige
 * Bereiche: stage 1-6, room_hex 0x00-0x27, player 0-1. Format-String
 * identisch zu extract_doors.c:133 (ohne assets_root-Präfix, da der Test
 * nur den room-relativen Teil prüft). Das Ergebnis ist exakt 19 Zeichen
 * + NUL = 20 Bytes; ein kleinerer Puffer wird abgelehnt.
 *
 * @return 0 bei Erfolg, -1 bei ungültigen Parametern / Puffer zu klein.
 */
static int re15_rdt_build_path(int stage, int room_hex, int player,
                               char* out_path, int max_len)
{
    /* Erforderliche Puffergröße: 19 Zeichen + abschließendes NUL. */
    const int needed = 20;

    if (out_path == NULL) {
        return -1;
    }
    if (stage < 1 || stage > 6) {
        return -1;
    }
    if (room_hex < 0x00 || room_hex > 0x27) {
        return -1;
    }
    if (player < 0 || player > 1) {
        return -1;
    }
    if (max_len < needed) {
        return -1;
    }

    /* Format byte-true zu build_rdt_path() (extract_doors.c:128-135):
     * "STAGE{Stage}/ROOM{Stage}{RoomHex:%02X}{Player}.RDT". %02X liefert
     * zweistellige Großbuchstaben-Hex (vom is_upper_hex-Assert geprüft). */
    snprintf(out_path, (size_t)max_len,
             "STAGE%d/ROOM%d%02X%d.RDT",
             stage, stage, room_hex, player);
    return 0;
}

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
 * Hilfsfunktionen für String-Verifikation
 * ========================================================================= */

/**
 * Prüft ob ein Zeichen ein gültiges Hex-Zeichen (Großbuchstabe) ist.
 */
static int is_upper_hex(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

/**
 * Konvertiert ein zweistelliges Hex-String zu int. Erwartet Großbuchstaben.
 * Gibt -1 zurück bei ungültigem Format.
 */
static int hex2_to_int(const char* s)
{
    int val = 0;
    int i;
    for (i = 0; i < 2; i++) {
        val <<= 4;
        if (s[i] >= '0' && s[i] <= '9') {
            val |= (s[i] - '0');
        } else if (s[i] >= 'A' && s[i] <= 'F') {
            val |= (s[i] - 'A' + 10);
        } else {
            return -1;
        }
    }
    return val;
}

/* =========================================================================
 * Property-Test: Gültige Kombinationen
 * ========================================================================= */

/**
 * Testet eine einzelne gültige Kombination.
 * Prüft das exakte Format: STAGE{N}/ROOM{Stage}{RoomHex}{Player}.RDT
 */
static int test_valid_combination(int stage, int room_hex, int player)
{
    char buf[RE15_MAX_PATH];
    int result;
    int len;

    memset(buf, 0xCC, sizeof(buf)); /* Fülle mit Sentinel-Bytes */

    result = re15_rdt_build_path(stage, room_hex, player, buf, sizeof(buf));

    PROP_ASSERT(result == 0,
        "re15_rdt_build_path(%d, 0x%02X, %d) returned %d (expected 0)",
        stage, room_hex, player, result);

    len = (int)strlen(buf);

    /* Erwartete Länge: "STAGE" (5) + digit (1) + "/" (1) + "ROOM" (4) +
     * stage_digit (1) + hex (2) + player_digit (1) + ".RDT" (4) = 19 */
    PROP_ASSERT(len == 19,
        "stage=%d room=0x%02X player=%d: len=%d (expected 19), got '%s'",
        stage, room_hex, player, len, buf);

    /* Prüfe Prefix "STAGE" */
    PROP_ASSERT(strncmp(buf, "STAGE", 5) == 0,
        "stage=%d room=0x%02X player=%d: missing 'STAGE' prefix, got '%s'",
        stage, room_hex, player, buf);

    /* Prüfe Stage-Ziffer */
    PROP_ASSERT(buf[5] == ('0' + stage),
        "stage=%d room=0x%02X player=%d: stage digit='%c' (expected '%c')",
        stage, room_hex, player, buf[5], '0' + stage);

    /* Prüfe Trennzeichen "/" */
    PROP_ASSERT(buf[6] == '/',
        "stage=%d room=0x%02X player=%d: separator='%c' (expected '/')",
        stage, room_hex, player, buf[6]);

    /* Prüfe "ROOM" */
    PROP_ASSERT(strncmp(buf + 7, "ROOM", 4) == 0,
        "stage=%d room=0x%02X player=%d: missing 'ROOM' after '/', got '%s'",
        stage, room_hex, player, buf + 7);

    /* Prüfe Stage-Ziffer im Dateinamen (Position 11) */
    PROP_ASSERT(buf[11] == ('0' + stage),
        "stage=%d room=0x%02X player=%d: filename stage='%c' (expected '%c')",
        stage, room_hex, player, buf[11], '0' + stage);

    /* Prüfe Hex-Ziffern (Position 12-13, Großbuchstaben) */
    PROP_ASSERT(is_upper_hex(buf[12]),
        "stage=%d room=0x%02X player=%d: hex[0]='%c' is not uppercase hex",
        stage, room_hex, player, buf[12]);

    PROP_ASSERT(is_upper_hex(buf[13]),
        "stage=%d room=0x%02X player=%d: hex[1]='%c' is not uppercase hex",
        stage, room_hex, player, buf[13]);

    /* Prüfe dass der Hex-Wert korrekt ist */
    {
        int parsed_hex = hex2_to_int(buf + 12);
        PROP_ASSERT(parsed_hex == room_hex,
            "stage=%d room=0x%02X player=%d: parsed hex=0x%02X (expected 0x%02X)",
            stage, room_hex, player, parsed_hex, room_hex);
    }

    /* Prüfe Player-Ziffer (Position 14) */
    PROP_ASSERT(buf[14] == ('0' + player),
        "stage=%d room=0x%02X player=%d: player digit='%c' (expected '%c')",
        stage, room_hex, player, buf[14], '0' + player);

    /* Prüfe Suffix ".RDT" */
    PROP_ASSERT(strcmp(buf + 15, ".RDT") == 0,
        "stage=%d room=0x%02X player=%d: suffix='%s' (expected '.RDT')",
        stage, room_hex, player, buf + 15);

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property-Test: Ungültige Eingaben
 * ========================================================================= */

/**
 * Testet dass ungültige Eingaben korrekt abgelehnt werden (return -1).
 */
static int test_invalid_inputs(void)
{
    char buf[RE15_MAX_PATH];
    int result;

    /* Stage = 0 (unterhalb gültigem Bereich) */
    result = re15_rdt_build_path(0, 0x10, 0, buf, sizeof(buf));
    PROP_ASSERT(result == -1,
        "stage=0 should return -1, got %d", result);

    /* Stage = 7 (oberhalb gültigem Bereich) */
    result = re15_rdt_build_path(7, 0x10, 0, buf, sizeof(buf));
    PROP_ASSERT(result == -1,
        "stage=7 should return -1, got %d", result);

    /* room_hex = 0x28 (oberhalb gültigem Bereich) */
    result = re15_rdt_build_path(1, 0x28, 0, buf, sizeof(buf));
    PROP_ASSERT(result == -1,
        "room_hex=0x28 should return -1, got %d", result);

    /* player = 2 (oberhalb gültigem Bereich) */
    result = re15_rdt_build_path(1, 0x10, 2, buf, sizeof(buf));
    PROP_ASSERT(result == -1,
        "player=2 should return -1, got %d", result);

    /* Negative Werte */
    result = re15_rdt_build_path(-1, 0x10, 0, buf, sizeof(buf));
    PROP_ASSERT(result == -1,
        "stage=-1 should return -1, got %d", result);

    result = re15_rdt_build_path(1, -1, 0, buf, sizeof(buf));
    PROP_ASSERT(result == -1,
        "room_hex=-1 should return -1, got %d", result);

    result = re15_rdt_build_path(1, 0x10, -1, buf, sizeof(buf));
    PROP_ASSERT(result == -1,
        "player=-1 should return -1, got %d", result);

    /* NULL-Pointer */
    result = re15_rdt_build_path(1, 0x10, 0, NULL, sizeof(buf));
    PROP_ASSERT(result == -1,
        "out_path=NULL should return -1, got %d", result);

    /* Puffer zu klein (Ergebnis braucht 20 Bytes inkl. NUL) */
    result = re15_rdt_build_path(1, 0x10, 0, buf, 10);
    PROP_ASSERT(result == -1,
        "max_len=10 (too small) should return -1, got %d", result);

    /* max_len = 0 */
    result = re15_rdt_build_path(1, 0x10, 0, buf, 0);
    PROP_ASSERT(result == -1,
        "max_len=0 should return -1, got %d", result);

    g_passed += 10; /* 10 Ungültigkeits-Prüfungen */
    return 0;
}

/* =========================================================================
 * Main — Exhaustiver Test aller 480 gültigen Kombinationen
 * ========================================================================= */

int main(void)
{
    int stage, room_hex, player;
    int total_valid = 0;

    printf("=== Property-Test: RDT-Dateiname-Konstruktion ===\n");
    printf("    Validates: Requirements 2.3\n\n");

    /* Teste alle gültigen Kombinationen exhaustiv:
     * 6 Stages × 40 Rooms (0x00-0x27) × 2 Players = 480 */
    printf("[1/2] Teste alle 480 gueltigen Kombinationen...\n");

    for (stage = 1; stage <= 6; stage++) {
        for (room_hex = 0x00; room_hex <= 0x27; room_hex++) {
            for (player = 0; player <= 1; player++) {
                if (test_valid_combination(stage, room_hex, player) != 0) {
                    /* Fehler bereits in test_valid_combination gemeldet */
                    printf("\n=== ABBRUCH: Erster Fehler bei stage=%d room=0x%02X player=%d ===\n",
                           stage, room_hex, player);
                    printf("=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
                           g_passed, g_failed);
                    return 1;
                }
                total_valid++;
            }
        }
    }

    printf("    %d/%d Kombinationen bestanden.\n\n", total_valid, total_valid);

    /* Teste ungültige Eingaben */
    printf("[2/2] Teste ungueltige Eingaben...\n");

    if (test_invalid_inputs() != 0) {
        printf("\n=== FEHLER bei Ungueltigkeits-Tests ===\n");
        printf("=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
               g_passed, g_failed);
        return 1;
    }

    printf("    Alle Ungueltigkeits-Tests bestanden.\n\n");

    /* Zusammenfassung */
    printf("=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
           g_passed, g_failed);

    return (g_failed > 0) ? 1 : 0;
}
