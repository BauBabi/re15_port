/**
 * @file re15_mem.h
 * @brief Speicher-Budget-Verwaltung für das Raum-Lade-System
 *
 * Trackt die Speicherbelegung des aktuell geladenen Raums und prüft
 * gegen das PSX-Budget (max 1,5 MB pro Raum bei 2 MB RAM gesamt).
 *
 * Auf PSX: re15_mem_budget_exceeded() gibt 1 zurück wenn das Budget
 *          überschritten ist — die Lade-Strategie wechselt auf Streaming
 *          (nur aktueller Raum + ein vorgeladener Nachbar-Raum).
 * Auf PC:  re15_mem_budget_exceeded() gibt immer 0 zurück (kein Limit),
 *          die Tracking-Funktionen arbeiten trotzdem für Debug-Zwecke.
 */
#ifndef RE15_MEM_H
#define RE15_MEM_H

#include "re15_types.h"

/* ============================================================================
 * Konstanten
 * ========================================================================= */

/** Maximales Speicher-Budget pro Raum auf PSX: 1,5 MB (1536 * 1024 Bytes) */
#define RE15_PSX_ROOM_BUDGET (1536 * 1024)

/* ============================================================================
 * Speicher-Nutzungs-Struktur
 *
 * Hält die aktuelle Speicherbelegung aufgeschlüsselt nach Kategorien.
 * ========================================================================= */

typedef struct {
    int32_t rdt_buffer_size;    /**< Größe des geladenen RDT-Puffers        */
    int32_t bss_chunk_size;     /**< BSS-Chunk-Größe (immer 65536)          */
    int32_t vab_size;           /**< VAB-Sound-Bank-Größe im Speicher       */
    int32_t total;              /**< Gesamtbelegung (Summe aller Einträge)   */
} re15_mem_usage_t;

/* ============================================================================
 * Funktions-Deklarationen
 * ========================================================================= */

/**
 * Setzt das Speicher-Budget-Tracking zurück (alle Einträge auf 0).
 * Wird beim Entladen eines Raums aufgerufen.
 */
void re15_mem_budget_reset(void);

/**
 * Registriert eine Speicherbelegung mit einem beschreibenden Tag.
 *
 * @param tag   Beschreibung der Allokation (z.B. "rdt", "bss", "vab")
 * @param size  Größe in Bytes (positiv = hinzufügen)
 *
 * Bekannte Tags und ihre Zuordnung:
 *   "rdt" → rdt_buffer_size
 *   "bss" → bss_chunk_size
 *   "vab" → vab_size
 *   Alles andere → nur in total aufaddiert
 */
void re15_mem_budget_add(const char* tag, int32_t size);

/**
 * Gibt die aktuelle Speicher-Nutzung zurück.
 *
 * @return Kopie der aktuellen Speicher-Nutzungs-Struktur
 */
re15_mem_usage_t re15_mem_budget_get(void);

/**
 * Prüft ob das Speicher-Budget überschritten ist.
 *
 * @return Auf PSX: 1 wenn total > RE15_PSX_ROOM_BUDGET, sonst 0
 *         Auf PC:  Immer 0 (kein Limit)
 */
int re15_mem_budget_exceeded(void);

#endif /* RE15_MEM_H */
