/**
 * @file mem_budget.c
 * @brief Speicher-Budget-Verwaltung (PSX Streaming-Fallback)
 *
 * Trackt die Speicherbelegung pro Raum und erzwingt das PSX-Limit
 * von 1,5 MB. Bei Überschreitung auf PSX wechselt die Lade-Strategie
 * auf Streaming: nur aktueller Raum + ein vorgeladener Nachbar-Raum.
 *
 * Auf PC dient das Tracking ausschließlich Debug-Zwecken — das Budget
 * wird nie als überschritten gemeldet.
 */

#include "re15_mem.h"
#include "re15_error.h"

#include <string.h>

/* ============================================================================
 * Internes State — Speicher-Nutzung des aktuellen Raums
 * ========================================================================= */

static re15_mem_usage_t s_usage;

/* ============================================================================
 * re15_mem_budget_reset — Budget-Tracking zurücksetzen
 * ========================================================================= */

void re15_mem_budget_reset(void)
{
    memset(&s_usage, 0, sizeof(s_usage));
}

/* ============================================================================
 * re15_mem_budget_add — Speicherbelegung registrieren
 * ========================================================================= */

void re15_mem_budget_add(const char* tag, int32_t size)
{
    if (tag == NULL) {
        s_usage.total += size;
        return;
    }

    /* Tag-basierte Zuordnung zu spezifischen Feldern */
    if (strcmp(tag, "rdt") == 0) {
        s_usage.rdt_buffer_size += size;
    } else if (strcmp(tag, "bss") == 0) {
        s_usage.bss_chunk_size += size;
    } else if (strcmp(tag, "vab") == 0) {
        s_usage.vab_size += size;
    }

    /* Immer zum Gesamtverbrauch addieren */
    s_usage.total += size;
}

/* ============================================================================
 * re15_mem_budget_get — Aktuelle Speicher-Nutzung abfragen
 * ========================================================================= */

re15_mem_usage_t re15_mem_budget_get(void)
{
    return s_usage;
}

/* ============================================================================
 * re15_mem_budget_exceeded — Budget-Überschreitung prüfen
 *
 * PSX: Gibt 1 zurück wenn total > 1,5 MB (Streaming-Fallback nötig)
 * PC:  Gibt immer 0 zurück (unbegrenzter RAM)
 * ========================================================================= */

int re15_mem_budget_exceeded(void)
{
#ifdef RE15_PLATFORM_PSX
    if (s_usage.total > RE15_PSX_ROOM_BUDGET) {
        RE15_WARN("MEM", "Budget ueberschritten: %d / %d Bytes (Streaming-Fallback aktiv)",
                  s_usage.total, RE15_PSX_ROOM_BUDGET);
        return 1;
    }
#endif

    /* PC: Kein Limit — nur Tracking für Debug */
    (void)s_usage;  /* Suppress unused warning on PC when total <= budget */
    return 0;
}
