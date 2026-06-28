/**
 * @file rdt_common.c
 * @brief RDT (Room Definition Table) Parser — In-Place-Sektions-Parsing
 *
 * Parst die 21-Einträge-Adresstabelle einer RDT-Datei (Offset 0x08-0x5C)
 * und konvertiert die LE-Offsets zu Pointern in den geladenen Puffer.
 * Null-Offsets (0x00000000) ergeben NULL-Pointer (Sektion nicht vorhanden).
 *
 * Parsing-Strategie: In-Place — kein Kopieren, Pointer zeigen direkt
 * in den übergebenen Puffer. Der Puffer muss für die gesamte Lebensdauer
 * der re15_rdt_t-Struktur gültig bleiben.
 */

#include "re15_rdt.h"
#include "re15_error.h"

#include <string.h>

/* Mindestgröße eines gültigen RDT-Puffers: 8 Byte Header + 21×4 Byte Tabelle */
#define RDT_MIN_SIZE 0x60

/* ============================================================================
 * Hilfsfunktion: 32-Bit Little-Endian aus Byte-Array lesen
 * ========================================================================= */

static uint32_t read_le32(const uint8_t* p)
{
    return (uint32_t)p[0]
         | ((uint32_t)p[1] << 8)
         | ((uint32_t)p[2] << 16)
         | ((uint32_t)p[3] << 24);
}

/* ============================================================================
 * re15_rdt_parse — Parst eine geladene RDT-Datei (In-Place)
 * ========================================================================= */

int re15_rdt_parse(uint8_t* buffer, int size, re15_rdt_t* out_rdt)
{
    int i;
    int table_offset;
    uint32_t offset;

    /*
     * Pointer-Array auf die 21 Felder der re15_rdt_t-Struktur.
     * Die Felder sind sequenziell angeordnet (alle uint8_t*), daher können
     * wir sie über einen Array von Pointer-zu-Pointer adressieren.
     */
    uint8_t** sections[RE15_RDT_SECTION_COUNT];

    /* --- Eingabe-Validierung --- */
    if (!buffer || !out_rdt) {
        return RE15_IO_INVALID_ARG;
    }

    if (size < RDT_MIN_SIZE) {
        RE15_ERROR("RDT", "Puffer zu klein: %d Bytes (min. %d)", size, RDT_MIN_SIZE);
        return RE15_IO_READ_ERROR;
    }

    /* --- Ausgabe initialisieren --- */
    memset(out_rdt, 0, sizeof(re15_rdt_t));

    /* --- Pointer-Array auf struct-Felder aufbauen --- */
    sections[0]  = &out_rdt->snd0_edt;
    sections[1]  = &out_rdt->snd0_vh;
    sections[2]  = &out_rdt->snd0_vb;
    sections[3]  = &out_rdt->snd1_edt;
    sections[4]  = &out_rdt->snd1_vh;
    sections[5]  = &out_rdt->snd1_vb;
    sections[6]  = &out_rdt->collision;
    sections[7]  = &out_rdt->camera;
    sections[8]  = &out_rdt->zone;
    sections[9]  = &out_rdt->light;
    sections[10] = &out_rdt->md1_ptr;
    sections[11] = &out_rdt->floor;
    sections[12] = &out_rdt->block;
    sections[13] = &out_rdt->message;
    sections[14] = &out_rdt->main_scd;
    sections[15] = &out_rdt->sub_scd;
    sections[16] = &out_rdt->extra_scd;
    sections[17] = &out_rdt->effect;
    sections[18] = &out_rdt->esp_tim;
    sections[19] = &out_rdt->model_tim;
    sections[20] = &out_rdt->animation;

    /* --- 21 Sektions-Offsets parsen (0x08 bis 0x5C) --- */
    for (i = 0; i < RE15_RDT_SECTION_COUNT; i++) {
        table_offset = 0x08 + (i * 4);
        offset = read_le32(buffer + table_offset);

        if (offset == 0) {
            /* Null-Offset: Sektion nicht vorhanden */
            *sections[i] = NULL;
        } else if ((int)offset < size) {
            /* Gültiger Offset: Pointer in den Puffer setzen */
            *sections[i] = buffer + offset;
        } else {
            /* Offset außerhalb des Puffers: als fehlend behandeln */
            RE15_WARN("RDT", "Sektion %d: Offset 0x%08X überschreitet Puffergröße (%d)",
                      i, offset, size);
            *sections[i] = NULL;
        }
    }

    return RE15_IO_OK;
}
