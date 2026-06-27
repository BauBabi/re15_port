/* =============================================================================
 * RE 1.5 Port — PSX I/O-Backend (CD-Dateisystem)
 * =============================================================================
 * Implementiert re15_io_backend_t für PSX über PSn00bSDK:
 *   - CdSearchFile zum Lokalisieren von Dateien auf der CD
 *   - CdControl(CdlSetloc) + CdRead für sektorbasiertes Lesen
 *   - 2048-Byte Sektor-Alignment und 4-Byte DMA-Alignment
 *
 * Maximal 4 gleichzeitig geöffnete Dateien (statische Slot-Verwaltung).
 * ========================================================================== */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <psxcd.h>

#include "re15_types.h"
#include "re15_io.h"
#include "re15_error.h"

/* ============================================================================
 * Alignment-Makros
 * ========================================================================= */

/** CD-Sektorgröße in Bytes */
#define SECTOR_SIZE 2048

/** Rundet size auf nächstes Vielfaches von SECTOR_SIZE auf */
#define ALIGN_SECTOR(x) (((x) + SECTOR_SIZE - 1) & ~(SECTOR_SIZE - 1))

/** Richtet einen Pointer auf 4-Byte-DMA-Grenze aus */
#define ALIGN_DMA(ptr) (((uintptr_t)(ptr) + 3) & ~(uintptr_t)3)

/* ============================================================================
 * Datei-Slot-Verwaltung (max. 4 gleichzeitige Handles)
 * ========================================================================= */

#define IO_PSX_MAX_SLOTS 4

typedef struct {
    CdlFILE cd_file;    /**< CdSearchFile-Ergebnis (Position + Größe) */
    uint8_t in_use;     /**< 1 = Slot belegt, 0 = frei                */
} io_psx_slot_t;

static io_psx_slot_t s_slots[IO_PSX_MAX_SLOTS];

/* ============================================================================
 * Hilfsfunktionen
 * ========================================================================= */

/**
 * Findet einen freien Slot.
 * @return Index des Slots oder -1 wenn alle belegt.
 */
static int slot_alloc(void)
{
    int i;
    for (i = 0; i < IO_PSX_MAX_SLOTS; i++) {
        if (!s_slots[i].in_use) {
            s_slots[i].in_use = 1;
            return i;
        }
    }
    return -1;
}

/**
 * Gibt einen Slot frei.
 */
static void slot_free(int idx)
{
    if (idx >= 0 && idx < IO_PSX_MAX_SLOTS) {
        s_slots[idx].in_use = 0;
    }
}

/**
 * Validiert ob ein Handle auf einen gültigen, belegten Slot zeigt.
 * @return Slot-Index oder -1 bei ungültigem Handle.
 */
static int slot_from_handle(void* handle)
{
    uintptr_t idx;
    if (handle == NULL) {
        return -1;
    }
    idx = (uintptr_t)handle - 1;
    if (idx >= IO_PSX_MAX_SLOTS) {
        return -1;
    }
    if (!s_slots[idx].in_use) {
        return -1;
    }
    return (int)idx;
}

/* ============================================================================
 * I/O-Backend-Funktionen
 * ========================================================================= */

/**
 * Öffnet eine Datei auf der CD.
 *
 * Der Pfad wird direkt an CdSearchFile übergeben. Erwartet wird das
 * PSX-CD-Format, z.B. "\\STAGE1\\ROOM1170.RDT;1".
 *
 * @param path   Pfad zur Datei im CD-Dateisystem-Format
 * @param handle Ausgabe: opakes Handle (Slot-Index + 1, als Pointer kodiert)
 * @return RE15_IO_OK bei Erfolg, negativer Fehlercode bei Fehler.
 */
static int io_psx_open(const char* path, void** handle)
{
    int slot;
    CdlFILE* fp;

    if (path == NULL || handle == NULL) {
        RE15_ERROR("IO_PSX", "open: Ungültiges Argument (NULL)");
        return RE15_IO_INVALID_ARG;
    }

    *handle = NULL;

    /* Freien Slot suchen */
    slot = slot_alloc();
    if (slot < 0) {
        RE15_ERROR("IO_PSX", "open: Alle %d Datei-Slots belegt", IO_PSX_MAX_SLOTS);
        return RE15_IO_BUFFER_FULL;
    }

    fp = &s_slots[slot].cd_file;

    /* Datei auf CD suchen */
    if (!CdSearchFile(fp, (char*)path)) {
        RE15_ERROR("IO_PSX", "open: Datei nicht gefunden: %s", path);
        slot_free(slot);
        return RE15_IO_NOT_FOUND;
    }

    /* Handle = Slot-Index + 1 (damit NULL als ungültig gilt) */
    *handle = (void*)((uintptr_t)slot + 1);

    return RE15_IO_OK;
}

/**
 * Liest Daten aus einer geöffneten CD-Datei.
 *
 * Die Lesemenge wird auf Sektorgrenzen (2048 Bytes) aufgerundet.
 * Der Puffer muss 4-Byte-aligned sein (DMA-Anforderung).
 *
 * @param handle     Opakes Datei-Handle (von open)
 * @param buf        Zielpuffer (muss 4-Byte-aligned sein)
 * @param size       Gewünschte Anzahl Bytes
 * @param bytes_read Ausgabe: Tatsächlich gelesene Bytes (= size bei Erfolg)
 * @return RE15_IO_OK bei Erfolg, negativer Fehlercode bei Fehler.
 */
static int io_psx_read(void* handle, uint8_t* buf, int size, int* bytes_read)
{
    int slot;
    int sectors;
    int aligned_size;

    if (buf == NULL || bytes_read == NULL) {
        RE15_ERROR("IO_PSX", "read: Ungültiges Argument (NULL)");
        return RE15_IO_INVALID_ARG;
    }

    *bytes_read = 0;

    slot = slot_from_handle(handle);
    if (slot < 0) {
        RE15_ERROR("IO_PSX", "read: Ungültiges Handle");
        return RE15_IO_INVALID_ARG;
    }

    if (size <= 0) {
        return RE15_IO_OK;
    }

    /* DMA-Alignment prüfen */
    if (((uintptr_t)buf & 3) != 0) {
        RE15_WARN("IO_PSX", "read: Puffer nicht 4-Byte-aligned (0x%08X)",
                  (unsigned int)(uintptr_t)buf);
    }

    /* Auf Sektorgröße aufrunden */
    aligned_size = (int)ALIGN_SECTOR(size);
    sectors = aligned_size / SECTOR_SIZE;

    /* Leseposition setzen */
    CdControl(CdlSetloc, (uint8_t*)&s_slots[slot].cd_file.pos, NULL);

    /* Sektoren lesen (CdlModeSpeed = Double-Speed-Modus) */
    CdRead(sectors, (uint32_t*)buf, CdlModeSpeed);

    /* Synchron auf Abschluss warten */
    if (CdReadSync(0, NULL) < 0) {
        RE15_ERROR("IO_PSX", "read: CD-Lesefehler (Slot %d, %d Sektoren)",
                   slot, sectors);
        return RE15_IO_READ_ERROR;
    }

    /* Tatsächlich angeforderte Bytes als gelesen melden */
    *bytes_read = size;

    return RE15_IO_OK;
}

/**
 * Ermittelt die Dateigröße.
 *
 * @param handle   Opakes Datei-Handle
 * @param size_out Ausgabe: Dateigröße in Bytes
 * @return RE15_IO_OK bei Erfolg, negativer Fehlercode bei Fehler.
 */
static int io_psx_get_size(void* handle, int* size_out)
{
    int slot;

    if (size_out == NULL) {
        RE15_ERROR("IO_PSX", "get_size: Ungültiges Argument (NULL)");
        return RE15_IO_INVALID_ARG;
    }

    slot = slot_from_handle(handle);
    if (slot < 0) {
        RE15_ERROR("IO_PSX", "get_size: Ungültiges Handle");
        *size_out = 0;
        return RE15_IO_INVALID_ARG;
    }

    *size_out = s_slots[slot].cd_file.size;

    return RE15_IO_OK;
}

/**
 * Schließt ein Datei-Handle und gibt den Slot frei.
 *
 * Darf mit NULL-Handle aufgerufen werden (no-op).
 *
 * @param handle Opakes Datei-Handle
 */
static void io_psx_close(void* handle)
{
    int slot;

    if (handle == NULL) {
        return; /* no-op per Spezifikation */
    }

    slot = slot_from_handle(handle);
    if (slot < 0) {
        RE15_WARN("IO_PSX", "close: Ungültiges Handle ignoriert");
        return;
    }

    slot_free(slot);
}

/* ============================================================================
 * Exportierte Backend-Instanz
 * ========================================================================= */

const re15_io_backend_t g_io_psx_backend = {
    io_psx_open,
    io_psx_read,
    io_psx_get_size,
    io_psx_close
};
