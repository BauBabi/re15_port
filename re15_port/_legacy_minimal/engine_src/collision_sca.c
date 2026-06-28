/**
 * @file collision_sca.c
 * @brief SCA (Scalar Collision Area) Parser
 *
 * Parst die SCA-Kollisionsdaten aus dem RDT-Puffer (rdt.collision-Sektion).
 * Die SCA-Daten enthalten Raum-Begrenzungen als Rechtecke, Kreise und
 * Treppen-Segmente, gegen die der Spieler-Kollisionskörper geprüft wird.
 *
 * Binärformat (Little-Endian):
 *   [0..1]  uint16_t count     — Anzahl der Einträge
 *   [2..3]  uint16_t reserved  — Reserviert (immer 0)
 *   [4..]   count × 14 Bytes   — Einträge (type, corner_x, corner_z,
 *                                  width, depth, density, flags)
 *
 * Parsing-Strategie: Die Einträge werden aus dem Puffer in ein statisches
 * Array konvertiert (Byte-Swapping von LE auf Host-Endianness). Der
 * entries-Pointer in der Ausgabestruktur zeigt auf dieses statische Array.
 */

#include "re15_collision.h"
#include "re15_error.h"

#include <string.h>

/* Maximale Anzahl SCA-Einträge (typische RE1.5-Räume: 10-80) */
#define SCA_MAX_ENTRIES 256

/* Statisches Array für geparste Einträge (vermeidet dynamische Allokation) */
static re15_sca_entry_t s_sca_entries[SCA_MAX_ENTRIES];

/* ============================================================================
 * Hilfsfunktionen: Little-Endian Byte-Lesen
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

/* ============================================================================
 * re15_sca_parse — Parst SCA-Kollisionsdaten aus dem RDT-Puffer
 * ========================================================================= */

int re15_sca_parse(const uint8_t* data, int data_size, re15_sca_data_t* out)
{
    uint16_t count;
    int required_size;
    int i;
    const uint8_t* entry_ptr;

    /* --- Eingabe-Validierung --- */
    if (!data || !out) {
        return RE15_IO_INVALID_ARG;
    }

    if (data_size < RE15_SCA_HEADER_SIZE) {
        RE15_ERROR("SCA", "Daten zu klein fuer Header: %d Bytes (min. %d)",
                   data_size, RE15_SCA_HEADER_SIZE);
        return RE15_IO_READ_ERROR;
    }

    /* --- Header lesen --- */
    count = read_le16(data);
    out->reserved = read_le16(data + 2);

    /* --- Plausibilitaetspruefung --- */
    if (count == 0) {
        out->count = 0;
        out->entries = NULL;
        return RE15_IO_OK;
    }

    if (count > SCA_MAX_ENTRIES) {
        RE15_WARN("SCA", "Eintragszahl %u ueberschreitet Maximum (%d), wird begrenzt",
                  count, SCA_MAX_ENTRIES);
        count = SCA_MAX_ENTRIES;
    }

    /* --- Groesse pruefen: Header + count * Entry-Size --- */
    required_size = RE15_SCA_HEADER_SIZE + (int)count * RE15_SCA_ENTRY_SIZE;
    if (data_size < required_size) {
        RE15_ERROR("SCA", "Daten zu klein: %d Bytes (benoetigt %d fuer %u Eintraege)",
                   data_size, required_size, count);
        return RE15_IO_READ_ERROR;
    }

    /* --- Eintraege parsen --- */
    out->count = count;
    out->entries = s_sca_entries;

    for (i = 0; i < (int)count; i++) {
        entry_ptr = data + RE15_SCA_HEADER_SIZE + (i * RE15_SCA_ENTRY_SIZE);

        s_sca_entries[i].type     = read_le16(entry_ptr + 0);
        s_sca_entries[i].corner_x = read_le16s(entry_ptr + 2);
        s_sca_entries[i].corner_z = read_le16s(entry_ptr + 4);
        s_sca_entries[i].width    = read_le16s(entry_ptr + 6);
        s_sca_entries[i].depth    = read_le16s(entry_ptr + 8);
        s_sca_entries[i].density  = read_le16(entry_ptr + 10);
        s_sca_entries[i].flags    = read_le16(entry_ptr + 12);
    }

    RE15_INFO("SCA", "%u Kollisionseintraege geparst", count);

    return RE15_IO_OK;
}
