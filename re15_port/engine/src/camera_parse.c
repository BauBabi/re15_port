/**
 * @file camera_parse.c
 * @brief RID- und RVD-Parser — Kameradefinitionen und Trigger-Zonen
 *
 * Parst RID (Room Image Data) für Kameradefinitionen (Position, Blickziel,
 * FOV pro Cut) und RVD (Room View Data) für Kamera-Trigger-Zonen
 * (4-Punkt-Polygone mit cam_from/cam_to).
 *
 * Daten werden aus Little-Endian-Rohdaten in die Zielstrukturen kopiert.
 */

#include "re15_camera.h"
#include "re15_error.h"

#include <string.h>

/* ============================================================================
 * Hilfsfunktionen: Little-Endian Byte-Array lesen
 * ========================================================================= */

static uint16_t read_le16(const uint8_t* p)
{
    return (uint16_t)p[0]
         | ((uint16_t)p[1] << 8);
}

static int16_t read_le16s(const uint8_t* p)
{
    return (int16_t)read_le16(p);
}

static uint32_t read_le32(const uint8_t* p)
{
    return (uint32_t)p[0]
         | ((uint32_t)p[1] << 8)
         | ((uint32_t)p[2] << 16)
         | ((uint32_t)p[3] << 24);
}

static int32_t read_le32s(const uint8_t* p)
{
    return (int32_t)read_le32(p);
}

/* ============================================================================
 * re15_rid_parse — Parst RID-Kameradaten
 *
 * Format:
 *   [0..3]  uint32_t count
 *   [4..]   count × 28 Bytes pro Eintrag:
 *             int32_t cam_x, cam_y, cam_z
 *             int32_t target_x, target_y, target_z
 *             uint32_t fov
 * ========================================================================= */

int re15_rid_parse(const uint8_t* data, int data_size, re15_rid_data_t* out)
{
    uint32_t count;
    uint32_t i;
    int required_size;
    const uint8_t* entry;

    /* --- Eingabe-Validierung --- */
    if (!data || !out) {
        return RE15_IO_INVALID_ARG;
    }

    /* Mindestens 4 Bytes für den Count-Header */
    if (data_size < 4) {
        RE15_ERROR("RID", "Daten zu klein fuer Header: %d Bytes (min. 4)", data_size);
        return RE15_IO_READ_ERROR;
    }

    /* --- Ausgabe initialisieren --- */
    memset(out, 0, sizeof(re15_rid_data_t));

    /* --- Count lesen --- */
    count = read_le32(data);

    if (count == 0) {
        out->count = 0;
        return RE15_IO_OK;
    }

    /* Overflow-sichere Größenprüfung */
    if (count > RE15_RID_MAX_CAMERAS) {
        RE15_WARN("RID", "Kamera-Count %u ueberschreitet Maximum (%d), wird begrenzt",
                  count, RE15_RID_MAX_CAMERAS);
        count = RE15_RID_MAX_CAMERAS;
    }

    required_size = 4 + (int)(count * RE15_RID_ENTRY_SIZE);
    if (data_size < required_size) {
        RE15_ERROR("RID", "Daten zu klein: %d Bytes (benoetigt %d fuer %u Kameras)",
                   data_size, required_size, count);
        return RE15_IO_READ_ERROR;
    }

    /* --- Einträge parsen --- */
    out->count = count;

    for (i = 0; i < count; i++) {
        entry = data + 4 + (i * RE15_RID_ENTRY_SIZE);

        out->cameras[i].cam_x    = read_le32s(entry + 0);
        out->cameras[i].cam_y    = read_le32s(entry + 4);
        out->cameras[i].cam_z    = read_le32s(entry + 8);
        out->cameras[i].target_x = read_le32s(entry + 12);
        out->cameras[i].target_y = read_le32s(entry + 16);
        out->cameras[i].target_z = read_le32s(entry + 20);
        out->cameras[i].fov      = read_le32(entry + 24);
    }

    RE15_INFO("RID", "%u Kameras geparst", count);
    return RE15_IO_OK;
}

/* ============================================================================
 * re15_rvd_parse — Parst RVD-Trigger-Zonen
 *
 * Format:
 *   [0..1]  uint16_t count
 *   [2..]   count × 22 Bytes pro Eintrag:
 *             uint8_t  cam_from
 *             uint8_t  cam_to
 *             int16_t  trigger_x[4]  (8 Bytes)
 *             int16_t  trigger_z[4]  (8 Bytes)
 *             uint16_t flags         (2 Bytes)
 * ========================================================================= */

int re15_rvd_parse(const uint8_t* data, int data_size, re15_rvd_data_t* out)
{
    uint16_t count;
    uint16_t i;
    int j;
    int required_size;
    const uint8_t* entry;

    /* --- Eingabe-Validierung --- */
    if (!data || !out) {
        return RE15_IO_INVALID_ARG;
    }

    /* Mindestens 2 Bytes für den Count-Header */
    if (data_size < 2) {
        RE15_ERROR("RVD", "Daten zu klein fuer Header: %d Bytes (min. 2)", data_size);
        return RE15_IO_READ_ERROR;
    }

    /* --- Ausgabe initialisieren --- */
    memset(out, 0, sizeof(re15_rvd_data_t));

    /* --- Count lesen --- */
    count = read_le16(data);

    if (count == 0) {
        out->count = 0;
        return RE15_IO_OK;
    }

    /* Overflow-sichere Größenprüfung */
    if (count > RE15_RVD_MAX_ZONES) {
        RE15_WARN("RVD", "Zonen-Count %u ueberschreitet Maximum (%d), wird begrenzt",
                  count, RE15_RVD_MAX_ZONES);
        count = RE15_RVD_MAX_ZONES;
    }

    required_size = 2 + (int)(count * RE15_RVD_ENTRY_SIZE);
    if (data_size < required_size) {
        RE15_ERROR("RVD", "Daten zu klein: %d Bytes (benoetigt %d fuer %u Zonen)",
                   data_size, required_size, count);
        return RE15_IO_READ_ERROR;
    }

    /* --- Einträge parsen --- */
    out->count = count;

    for (i = 0; i < count; i++) {
        entry = data + 2 + (i * RE15_RVD_ENTRY_SIZE);

        out->zones[i].cam_from = entry[0];
        out->zones[i].cam_to   = entry[1];

        /* 4× int16_t trigger_x (Offset 2..9) */
        for (j = 0; j < 4; j++) {
            out->zones[i].trigger_x[j] = read_le16s(entry + 2 + (j * 2));
        }

        /* 4× int16_t trigger_z (Offset 10..17) */
        for (j = 0; j < 4; j++) {
            out->zones[i].trigger_z[j] = read_le16s(entry + 10 + (j * 2));
        }

        /* uint16_t flags (Offset 18..19) — but entry is 22 bytes total:
         * cam_from(1) + cam_to(1) + trigger_x(8) + trigger_z(8) + flags(2) = 20
         * Wait — design says 22 bytes. Let me recalculate:
         * 1 + 1 + 4*2 + 4*2 + 2 = 1+1+8+8+2 = 20. That's 20, not 22.
         * The design specifies 22 bytes per entry. There may be 2 padding bytes.
         * We'll read flags at offset 18 and skip 2 bytes of padding after. */
        out->zones[i].flags = read_le16(entry + 18);
    }

    RE15_INFO("RVD", "%u Trigger-Zonen geparst", count);
    return RE15_IO_OK;
}
