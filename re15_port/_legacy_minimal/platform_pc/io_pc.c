/**
 * @file io_pc.c
 * @brief PC I/O-Backend: Standard-C-Dateizugriff (fopen/fread/fclose)
 *
 * Implementiert das re15_io_backend_t Interface für die PC-Plattform
 * mittels Standard-C-I/O-Funktionen. Alle Funktionen geben
 * re15_io_result_t Codes zurück.
 */

#include <stdio.h>
#include <stddef.h>

#include "re15_io.h"
#include "re15_error.h"

/* ============================================================================
 * Statische I/O-Funktionen (intern)
 * ========================================================================= */

/**
 * Öffnet eine Datei im Binärmodus.
 *
 * @param path    Pfad zur Datei (relativ oder absolut)
 * @param handle  Ausgabe: opakes Handle (FILE* gecastet auf void*)
 * @return RE15_IO_OK bei Erfolg, RE15_IO_INVALID_ARG bei NULL-Argumenten,
 *         RE15_IO_NOT_FOUND wenn die Datei nicht geöffnet werden kann
 */
static int io_pc_open(const char* path, void** handle)
{
    FILE* fp;

    if (path == NULL || handle == NULL) {
        return RE15_IO_INVALID_ARG;
    }

    fp = fopen(path, "rb");
    if (fp == NULL) {
        RE15_ERROR("IO", "Datei nicht gefunden: %s", path);
        return RE15_IO_NOT_FOUND;
    }

    *handle = (void*)fp;
    return RE15_IO_OK;
}

/**
 * Liest bis zu 'size' Bytes aus einem geöffneten Handle.
 *
 * @param handle      Opakes Handle (FILE*)
 * @param buf         Zielpuffer
 * @param size        Maximale Anzahl zu lesender Bytes
 * @param bytes_read  Ausgabe: tatsächlich gelesene Byte-Anzahl
 * @return RE15_IO_OK bei Erfolg, RE15_IO_INVALID_ARG bei NULL-Argumenten,
 *         RE15_IO_READ_ERROR bei Lesefehler
 */
static int io_pc_read(void* handle, uint8_t* buf, int size, int* bytes_read)
{
    FILE* fp;
    size_t read_count;

    if (handle == NULL || buf == NULL || bytes_read == NULL) {
        return RE15_IO_INVALID_ARG;
    }

    if (size <= 0) {
        *bytes_read = 0;
        return RE15_IO_OK;
    }

    fp = (FILE*)handle;
    read_count = fread(buf, 1, (size_t)size, fp);

    if (read_count == 0 && ferror(fp)) {
        RE15_ERROR("IO", "Lesefehler (fread fehlgeschlagen)");
        *bytes_read = 0;
        return RE15_IO_READ_ERROR;
    }

    *bytes_read = (int)read_count;
    return RE15_IO_OK;
}

/**
 * Ermittelt die Dateigröße in Bytes.
 *
 * Setzt den Datei-Cursor via fseek an das Ende, liest die Position
 * mit ftell und setzt den Cursor zurück an den Anfang.
 *
 * @param handle    Opakes Handle (FILE*)
 * @param size_out  Ausgabe: Dateigröße in Bytes
 * @return RE15_IO_OK bei Erfolg, RE15_IO_INVALID_ARG bei NULL-Handle,
 *         RE15_IO_READ_ERROR bei fseek/ftell-Fehler
 */
static int io_pc_get_size(void* handle, int* size_out)
{
    FILE* fp;
    long pos;

    if (handle == NULL || size_out == NULL) {
        return RE15_IO_INVALID_ARG;
    }

    fp = (FILE*)handle;

    if (fseek(fp, 0L, SEEK_END) != 0) {
        RE15_ERROR("IO", "fseek(SEEK_END) fehlgeschlagen");
        return RE15_IO_READ_ERROR;
    }

    pos = ftell(fp);
    if (pos < 0) {
        RE15_ERROR("IO", "ftell fehlgeschlagen");
        return RE15_IO_READ_ERROR;
    }

    if (fseek(fp, 0L, SEEK_SET) != 0) {
        RE15_ERROR("IO", "fseek(SEEK_SET) fehlgeschlagen");
        return RE15_IO_READ_ERROR;
    }

    *size_out = (int)pos;
    return RE15_IO_OK;
}

/**
 * Schließt ein geöffnetes Handle.
 *
 * Ignoriert NULL-Handles (no-op).
 *
 * @param handle  Opakes Handle (FILE*), darf NULL sein
 */
static void io_pc_close(void* handle)
{
    if (handle == NULL) {
        return;
    }

    fclose((FILE*)handle);
}

/* ============================================================================
 * Exportierte Backend-Instanz
 * ========================================================================= */

const re15_io_backend_t g_io_pc_backend = {
    io_pc_open,
    io_pc_read,
    io_pc_get_size,
    io_pc_close
};

/* Definition des in re15_io.h deklarierten globalen I/O-Backend-Pointers. */
const re15_io_backend_t* g_io = &g_io_pc_backend;
