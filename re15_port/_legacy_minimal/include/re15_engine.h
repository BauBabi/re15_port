/**
 * @file re15_engine.h
 * @brief Globale Engine-Zustandsstruktur (re15_game_t)
 *
 * Definiert die zentrale Datenstruktur, die den gesamten Engine-Zustand
 * bündelt: Spieler, aktueller Raum, SCD-VM, Game-Flags und Puffer.
 */
#ifndef RE15_ENGINE_H
#define RE15_ENGINE_H

#include "re15_types.h"

/* ============================================================================
 * Forward-Deklarationen
 *
 * Die vollständigen Strukturdefinitionen folgen in späteren Tasks:
 * - re15_player.h    → re15_player_t
 * - re15_rdt.h       → re15_rdt_t
 * - re15_scd.h       → scd_vm_t
 * - (game_state)     → re15_game_state_t
 * ========================================================================= */
typedef struct re15_player_s     re15_player_t;
typedef struct re15_rdt_s        re15_rdt_t;
typedef struct scd_vm_s          scd_vm_t;
typedef struct re15_game_state_s re15_game_state_t;

/* ============================================================================
 * Globale Engine-Zustandsstruktur
 * ========================================================================= */

/** BSS-Chunk-Größe: 64 KB (ein Kamera-Hintergrund) */
#define RE15_BSS_CHUNK_SIZE 65536

/**
 * Zentrale Engine-Struktur — enthält den gesamten Spielzustand.
 * Wird als globale Instanz (g_game) bereitgestellt.
 */
typedef struct {
    re15_player_t*      player;         /**< Zeiger auf aktiven Spieler        */
    re15_rdt_t*         current_rdt;    /**< Zeiger auf aktuell geladenen Raum */
    scd_vm_t*           scd;            /**< Zeiger auf SCD-VM-Instanz         */
    re15_game_state_t*  game_flags;     /**< Zeiger auf globale Game-Flags     */

    uint8_t             current_stage;  /**< Aktuelle Stage (1-6)              */
    uint8_t             current_room;   /**< Aktuelle Room-ID (0x00-0x27)      */
    uint8_t             current_cut;    /**< Aktive Kamera-ID                  */
    uint8_t             current_player; /**< 0=Leon, 1=Elza                    */

    uint8_t*            rdt_buffer;     /**< Puffer für geladene RDT-Daten     */
    int                 rdt_buffer_size;/**< Größe des RDT-Puffers in Bytes    */
    uint8_t             bss_chunk[RE15_BSS_CHUNK_SIZE]; /**< 64 KB BSS-Chunk   */
} re15_game_t;

/** Globale Engine-Instanz */
extern re15_game_t g_game;

#endif /* RE15_ENGINE_H */
