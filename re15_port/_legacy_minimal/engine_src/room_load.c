/**
 * @file room_load.c
 * @brief Raum-Lade-System — Vollständige Lade-/Entlade-Logik
 *
 * Implementiert den Room-Manager für den RE 1.5 Port:
 *
 * Entladen (re15_room_unload):
 *   - RDT-Puffer freigeben
 *   - AOT-Slots zurücksetzen
 *   - SCD-VM reinitialisieren
 *   - Zustandsvariablen nullen
 *
 * Laden (re15_room_load):
 *   1. Aktuellen Raum entladen
 *   2. RDT-Pfad konstruieren (STAGE{N}/ROOM{S}{R}{P}.RDT)
 *   3. Asset-Root + relativen Pfad zu vollem Pfad verbinden
 *   4. Datei via I/O-Backend öffnen, Größe ermitteln, lesen
 *   5. RDT in-place parsen (Pointer in den Puffer)
 *   6. SCD-VM initialisieren, Main-Thread starten (wenn main_scd != NULL)
 *   7. Kamera auf Cut 0 setzen
 *
 * Null-Offset-Behandlung: Wenn eine Sektion nicht vorhanden ist (Offset 0
 * in der RDT-Adresstabelle → NULL-Pointer), wird das Subsystem ohne Daten
 * initialisiert (z.B. kein SCD-Thread gestartet, kein Audio geladen).
 */

#include "re15_room.h"
#include "re15_engine.h"
#include "re15_rdt.h"
#include "re15_io.h"
#include "re15_io_common.h"
#include "re15_aot.h"
#include "re15_scd.h"
#include "re15_error.h"
#include "re15_bss.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ============================================================================
 * Globale Engine-Instanz (definiert hier, deklariert extern in re15_engine.h)
 *
 * TODO: In eigene engine_init.c verschieben wenn Engine-Initialisierung
 * als separater Task implementiert wird.
 * ========================================================================= */

re15_game_t g_game;

/* ============================================================================
 * Separatoren für Pfad-Konstruktion (plattformabhängig)
 * ========================================================================= */

#ifdef _WIN32
  #define PATH_SEP '\\'
  #define PATH_SEP_STR "\\"
#else
  #define PATH_SEP '/'
  #define PATH_SEP_STR "/"
#endif

/* ============================================================================
 * re15_room_unload — Aktuellen Raum vollständig entladen
 * ========================================================================= */

void re15_room_unload(void)
{
    /* 1. RDT-Puffer freigeben */
    if (g_game.rdt_buffer != NULL) {
        free(g_game.rdt_buffer);
        g_game.rdt_buffer = NULL;
    }

    /* 2. AOT-Slots zurücksetzen (alle inaktiv) */
    re15_aot_init();

    /* 3. SCD-VM reinitialisieren (alle Threads inaktiv, Handler neu registriert) */
    scd_vm_init();

    /* 4. Puffer-Größe zurücksetzen */
    g_game.rdt_buffer_size = 0;

    /* 5. Aktuelle RDT-Struktur nullen */
    if (g_game.current_rdt != NULL) {
        memset(g_game.current_rdt, 0, sizeof(re15_rdt_t));
    }
}

/* ============================================================================
 * re15_room_load — Raum vollständig laden
 * ========================================================================= */

int re15_room_load(uint8_t stage, uint8_t room_id, uint8_t player_id)
{
    char rel_path[RE15_MAX_PATH];
    char full_path[RE15_MAX_PATH];
    const char* assets_root;
    void* handle = NULL;
    int file_size = 0;
    int bytes_read = 0;
    uint8_t* buffer = NULL;
    int rc;

    /* --- 1. Aktuellen Raum entladen --- */
    re15_room_unload();

    /* --- 2. RDT-Pfad konstruieren --- */
    rc = re15_rdt_build_path((int)stage, (int)room_id, (int)player_id,
                             rel_path, sizeof(rel_path));
    if (rc != 0) {
        RE15_ERROR("ROOM", "Ungültige Parameter: stage=%d, room=0x%02X, player=%d",
                   stage, room_id, player_id);
        return RE15_IO_INVALID_ARG;
    }

    /* --- 3. Vollständigen Pfad zusammensetzen (assets_root + rel_path) --- */
    assets_root = re15_assets_get_root();
    if (assets_root == NULL) {
        RE15_ERROR("ROOM", "Asset-Root nicht konfiguriert (RE15_ASSETS_PATH)");
        return RE15_IO_NOT_FOUND;
    }

    rc = snprintf(full_path, sizeof(full_path), "%s" PATH_SEP_STR "%s",
                  assets_root, rel_path);
    if (rc < 0 || rc >= (int)sizeof(full_path)) {
        RE15_ERROR("ROOM", "Pfad zu lang: %s/%s", assets_root, rel_path);
        return RE15_IO_BUFFER_FULL;
    }

    /* --- 4. Datei via I/O-Backend öffnen --- */
    if (g_io == NULL) {
        RE15_ERROR("ROOM", "I/O-Backend nicht initialisiert (g_io == NULL)");
        return RE15_IO_INVALID_ARG;
    }

    rc = g_io->open(full_path, &handle);
    if (rc != RE15_IO_OK) {
        RE15_ERROR("ROOM", "Datei nicht gefunden: %s", full_path);
        return RE15_IO_NOT_FOUND;
    }

    /* --- 5. Dateigröße ermitteln --- */
    rc = g_io->get_size(handle, &file_size);
    if (rc != RE15_IO_OK || file_size <= 0) {
        RE15_ERROR("ROOM", "Dateigröße nicht ermittelbar: %s", full_path);
        g_io->close(handle);
        return RE15_IO_READ_ERROR;
    }

    /* --- 6. Puffer allokieren --- */
    buffer = (uint8_t*)malloc((size_t)file_size);
    if (buffer == NULL) {
        RE15_ERROR("ROOM", "Speicherallokation fehlgeschlagen: %d Bytes", file_size);
        g_io->close(handle);
        return RE15_IO_READ_ERROR;
    }

    /* --- 7. Datei lesen --- */
    rc = g_io->read(handle, buffer, file_size, &bytes_read);
    if (rc != RE15_IO_OK || bytes_read != file_size) {
        RE15_ERROR("ROOM", "Lesefehler: %s (gelesen: %d/%d)",
                   full_path, bytes_read, file_size);
        free(buffer);
        g_io->close(handle);
        return RE15_IO_READ_ERROR;
    }

    /* --- 8. Handle schließen --- */
    g_io->close(handle);

    /* --- 9. RDT in-place parsen --- */
    if (g_game.current_rdt == NULL) {
        RE15_ERROR("ROOM", "current_rdt Zeiger nicht initialisiert");
        free(buffer);
        return RE15_IO_INVALID_ARG;
    }

    rc = re15_rdt_parse(buffer, file_size, g_game.current_rdt);
    if (rc != RE15_IO_OK) {
        RE15_ERROR("ROOM", "RDT-Parse fehlgeschlagen: %s (rc=%d)", full_path, rc);
        free(buffer);
        return rc;
    }

    /* --- 10. Puffer und Zustand im Engine-State speichern --- */
    g_game.rdt_buffer = buffer;
    g_game.rdt_buffer_size = file_size;
    g_game.current_stage = stage;
    g_game.current_room = room_id;
    g_game.current_player = player_id;

    /* --- 11. SCD-VM initialisieren und Main-Thread starten --- */
    /*
     * scd_vm_init() wurde bereits in re15_room_unload() aufgerufen.
     * Hier starten wir nur den Main-Thread wenn main_scd vorhanden ist.
     *
     * Null-Offset-Behandlung: Wenn main_scd == NULL (Sektion nicht vorhanden),
     * wird kein Thread gestartet — das SCD-Subsystem bleibt idle.
     */
    if (g_game.current_rdt->main_scd != NULL) {
        /* Die SCD-Sektion beginnt mit einer u16-Pointer-Tabelle; sub00 (der
         * Init-Thread) liegt bei base + Tabelleneintrag[0]. NICHT bei base
         * starten, sonst werden die Tabellen-Bytes als Opcodes fehlinterpretiert. */
        const uint8_t* scd = g_game.current_rdt->main_scd;
        uint16_t sub0_off = (uint16_t)(scd[0] | (scd[1] << 8));
        scd_thread_start(0, scd + sub0_off);
    }

    /* --- 12. Kamera auf Cut 0 setzen --- */
    g_game.current_cut = 0;

    /* --- 13. BSS-Hintergrund (Cut 0) in g_game.bss_chunk laden --- */
    if (re15_bss_load_chunk(stage, room_id, 0, g_game.bss_chunk) != RE15_IO_OK) {
        memset(g_game.bss_chunk, 0, RE15_BSS_CHUNK_SIZE);  /* Fallback: schwarz */
    }

    /* --- Erfolg --- */
    RE15_INFO("ROOM", "Raum geladen: Stage %d, Room 0x%02X, Player %d (%d Bytes)",
              stage, room_id, player_id, file_size);

    return RE15_IO_OK;
}
