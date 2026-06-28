/**
 * @file io_common.c
 * @brief Gemeinsame I/O-Hilfsfunktionen: Case-insensitive Pfadauflösung,
 *        Asset-Root-Ermittlung und RDT-Pfad-Konstruktion
 *
 * Plattformübergreifende Implementierung:
 * - Windows (_WIN32): Nutzt FindFirstFileA (native Case-Insensitivität)
 * - POSIX (Linux/macOS): opendir/readdir mit strcasecmp-Match
 */

#include "re15_io_common.h"
#include "re15_error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#else
  #include <dirent.h>
  #include <strings.h>  /* strcasecmp */
#endif

/* ============================================================================
 * re15_io_resolve_ci — Case-insensitive Dateiauflösung
 * ========================================================================= */

int re15_io_resolve_ci(const char* dir, const char* filename, char* out)
{
    if (!dir || !filename || !out) {
        return -1;
    }

#ifdef _WIN32
    /*
     * Windows: Dateisystem ist nativ case-insensitiv.
     * Nutze FindFirstFileA um den tatsächlichen Dateinamen zu ermitteln.
     */
    char search_path[RE15_MAX_PATH];
    int written = snprintf(search_path, sizeof(search_path), "%s\\%s", dir, filename);
    if (written < 0 || written >= (int)sizeof(search_path)) {
        return -1;
    }

    WIN32_FIND_DATAA find_data;
    HANDLE h = FindFirstFileA(search_path, &find_data);
    if (h == INVALID_HANDLE_VALUE) {
        return -1;
    }

    /* Kopiere den tatsächlichen Dateinamen (mit korrekter Groß-/Kleinschreibung) */
    strncpy(out, find_data.cFileName, RE15_MAX_PATH - 1);
    out[RE15_MAX_PATH - 1] = '\0';

    FindClose(h);
    return 0;

#else
    /*
     * POSIX: Dateisystem ist case-sensitiv.
     * Iteriere das Verzeichnis und finde einen case-insensitiven Match.
     */
    DIR* d = opendir(dir);
    if (!d) {
        return -1;
    }

    struct dirent* entry;
    while ((entry = readdir(d)) != NULL) {
        if (strcasecmp(entry->d_name, filename) == 0) {
            strncpy(out, entry->d_name, RE15_MAX_PATH - 1);
            out[RE15_MAX_PATH - 1] = '\0';
            closedir(d);
            return 0;
        }
    }

    closedir(d);
    return -1;  /* Nicht gefunden */
#endif
}

/* ============================================================================
 * re15_assets_get_root — Asset-Root-Pfad ermitteln
 * ========================================================================= */

const char* re15_assets_get_root(void)
{
    /*
     * Priorität 1: Compile-Time-Define RE15_ASSETS_PATH
     * Wird über CMake als -DRE15_ASSETS_PATH="..." gesetzt.
     */
#ifdef RE15_ASSETS_PATH
    /* Stringifizierung des Makro-Werts */
    #define RE15_STRINGIFY_HELPER(x) #x
    #define RE15_STRINGIFY(x) RE15_STRINGIFY_HELPER(x)
    return RE15_STRINGIFY(RE15_ASSETS_PATH);
    #undef RE15_STRINGIFY
    #undef RE15_STRINGIFY_HELPER
#else
    /*
     * Priorität 2: Umgebungsvariable RE15_ASSETS_PATH
     */
    const char* env = getenv("RE15_ASSETS_PATH");
    if (env && env[0] != '\0') {
        return env;
    }

    return NULL;
#endif
}

/* ============================================================================
 * re15_rdt_build_path — RDT-Dateipfad konstruieren
 * ========================================================================= */

int re15_rdt_build_path(int stage, int room_hex, int player, char* out_path, int max_len)
{
    /* Parameter-Validierung */
    if (!out_path || max_len <= 0) {
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

    /* Format: STAGE{N}/ROOM{Stage}{RoomHex}{Player}.RDT
     * Beispiel: STAGE1/ROOM1170.RDT (Stage 1, Room 0x17, Player 0) */
    int written = snprintf(out_path, max_len,
                           "STAGE%d/ROOM%d%02X%d.RDT",
                           stage, stage, room_hex, player);

    if (written < 0 || written >= max_len) {
        /* Puffer zu klein */
        out_path[0] = '\0';
        return -1;
    }

    return 0;
}
