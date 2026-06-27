/**
 * @file bss_load.c
 * @brief BSS-Hintergrund-Laden — 64KB-Chunk-Selektion nach Kamera-ID
 *
 * BSS-Dateien enthalten MDEC-kodierte Hintergründe (VLC-Bitstream, 320x240),
 * einen 64KB-Chunk pro Kamera-Winkel. Der Chunk für Kamera-ID N beginnt
 * bei Offset N * 65536 in der BSS-Datei.
 *
 * Lade-Strategie (PC und PSX kompatibel):
 *   Da das I/O-Backend immer ab Position 0 liest (kein Seek), wird die
 *   gesamte Datei bis zum Ende des benötigten Chunks gelesen und dann
 *   der korrekte 64KB-Bereich per memcpy in den Ausgabepuffer kopiert.
 *
 * Fehlerbehandlung:
 *   - BSS-Datei fehlt → RE15_IO_NOT_FOUND + Warnung (Aufrufer füllt schwarz)
 *   - Kamera-ID > verfügbare Chunks → RE15_IO_INVALID_ARG + Warnung
 *   - Lesefehler → RE15_IO_READ_ERROR + Warnung
 */

#include "re15_bss.h"
#include "re15_engine.h"
#include "re15_io.h"
#include "re15_io_common.h"
#include "re15_error.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ============================================================================
 * Plattform-Pfadtrenner
 * ========================================================================= */

#ifdef _WIN32
  #define BSS_PATH_SEP_STR "\\"
#else
  #define BSS_PATH_SEP_STR "/"
#endif

/* ============================================================================
 * re15_bss_build_path — BSS-Dateipfad konstruieren
 * ========================================================================= */

int re15_bss_build_path(int stage, int room_hex, char* out_path, int max_len)
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

    /* Format: STAGE{N}/ROOM{Stage}{RoomHex}.BSS
     * Beispiel: STAGE1/ROOM117.BSS (Stage 1, Room 0x17) */
    int written = snprintf(out_path, max_len,
                           "STAGE%d/ROOM%d%02X.BSS",
                           stage, stage, room_hex);

    if (written < 0 || written >= max_len) {
        out_path[0] = '\0';
        return -1;
    }

    return 0;
}

/* ============================================================================
 * re15_bss_load_chunk — BSS-Chunk für Kamera-ID laden
 * ========================================================================= */

int re15_bss_load_chunk(uint8_t stage, uint8_t room_id, uint8_t camera_id,
                        uint8_t* out_chunk)
{
    char rel_path[RE15_MAX_PATH];
    char full_path[RE15_MAX_PATH];
    const char* assets_root;
    void* handle = NULL;
    int file_size = 0;
    int bytes_read = 0;
    int chunk_offset;
    int read_size;
    uint8_t* temp_buffer = NULL;
    int rc;

    /* --- 0. Parameter-Prüfung --- */
    if (out_chunk == NULL) {
        RE15_ERROR("BSS", "out_chunk ist NULL");
        return RE15_IO_INVALID_ARG;
    }

    /* --- 1. BSS-Dateipfad konstruieren --- */
    rc = re15_bss_build_path((int)stage, (int)room_id, rel_path, sizeof(rel_path));
    if (rc != 0) {
        RE15_WARN("BSS", "Ungültige Parameter: stage=%d, room=0x%02X",
                  stage, room_id);
        return RE15_IO_INVALID_ARG;
    }

    /* --- 2. Vollständigen Pfad zusammensetzen (assets_root + rel_path) --- */
    assets_root = re15_assets_get_root();
    if (assets_root == NULL) {
        RE15_WARN("BSS", "Asset-Root nicht konfiguriert (RE15_ASSETS_PATH)");
        return RE15_IO_NOT_FOUND;
    }

    rc = snprintf(full_path, sizeof(full_path), "%s" BSS_PATH_SEP_STR "%s",
                  assets_root, rel_path);
    if (rc < 0 || rc >= (int)sizeof(full_path)) {
        RE15_WARN("BSS", "Pfad zu lang: %s/%s", assets_root, rel_path);
        return RE15_IO_BUFFER_FULL;
    }

    /* --- 3. I/O-Backend prüfen und Datei öffnen --- */
    if (g_io == NULL) {
        RE15_WARN("BSS", "I/O-Backend nicht initialisiert (g_io == NULL)");
        return RE15_IO_INVALID_ARG;
    }

    rc = g_io->open(full_path, &handle);
    if (rc != RE15_IO_OK) {
        RE15_WARN("BSS", "BSS-Datei nicht gefunden: %s (schwarzer Hintergrund)",
                  full_path);
        return RE15_IO_NOT_FOUND;
    }

    /* --- 4. Dateigröße ermitteln --- */
    rc = g_io->get_size(handle, &file_size);
    if (rc != RE15_IO_OK || file_size <= 0) {
        RE15_WARN("BSS", "Dateigröße nicht ermittelbar: %s", full_path);
        g_io->close(handle);
        return RE15_IO_READ_ERROR;
    }

    /* --- 5. Kamera-ID validieren: camera_id * 65536 + 65536 <= file_size --- */
    chunk_offset = (int)camera_id * RE15_BSS_CHUNK_SIZE;
    if (chunk_offset + RE15_BSS_CHUNK_SIZE > file_size) {
        RE15_WARN("BSS", "Kamera-ID %d überschreitet verfügbare Chunks "
                  "(Datei: %d Bytes, benötigt: %d, Datei: %s)",
                  camera_id, file_size, chunk_offset + RE15_BSS_CHUNK_SIZE,
                  full_path);
        g_io->close(handle);
        return RE15_IO_INVALID_ARG;
    }

    /* --- 6. Datei bis zum Ende des benötigten Chunks lesen ---
     *
     * Da unser I/O-Backend immer ab Position 0 liest (kein Seek-Support),
     * lesen wir vom Dateianfang bis zum Ende des benötigten Chunks.
     * Auf PSX (CdRead) ist dies ohnehin das native Verhalten.
     */
    read_size = chunk_offset + RE15_BSS_CHUNK_SIZE;

    temp_buffer = (uint8_t*)malloc((size_t)read_size);
    if (temp_buffer == NULL) {
        RE15_WARN("BSS", "Speicherallokation fehlgeschlagen: %d Bytes", read_size);
        g_io->close(handle);
        return RE15_IO_READ_ERROR;
    }

    rc = g_io->read(handle, temp_buffer, read_size, &bytes_read);
    if (rc != RE15_IO_OK || bytes_read < read_size) {
        RE15_WARN("BSS", "Lesefehler: %s (gelesen: %d/%d)",
                  full_path, bytes_read, read_size);
        free(temp_buffer);
        g_io->close(handle);
        return RE15_IO_READ_ERROR;
    }

    /* --- 7. 64KB-Chunk an korrektem Offset nach out_chunk kopieren --- */
    memcpy(out_chunk, temp_buffer + chunk_offset, RE15_BSS_CHUNK_SIZE);

    /* --- 8. Aufräumen --- */
    free(temp_buffer);
    g_io->close(handle);

    RE15_INFO("BSS", "BSS-Chunk geladen: Stage %d, Room 0x%02X, Kamera %d "
              "(Offset: 0x%X)",
              stage, room_id, camera_id, chunk_offset);

    return RE15_IO_OK;
}
