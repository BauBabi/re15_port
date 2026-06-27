/**
 * @file stage_change.c
 * @brief Stage-Wechsel-Logik: Overlay laden + VAB-Bank wechseln
 *
 * Beim Übergang zwischen Stages (z.B. Stage 1 → Stage 2) werden zusätzliche
 * Schritte über den normalen Raumwechsel hinaus durchgeführt:
 *
 *   1. Neues Stage-Overlay laden (STAGE{N}.BIN — enthält stage-spezifischen
 *      Code/Daten). Auf PSX wird das Overlay nach 0x80100000 geladen;
 *      auf PC ist dies ein Stub (kein Overlay-Relocation erforderlich).
 *
 *   2. VAB-Sound-Bank wechseln: Alte Bank entladen, neue stage-spezifische
 *      Bank laden (über g_audio Backend, falls verfügbar).
 *
 *   3. Raumspezifische BGM-Tabelle für den neuen Stage aktivieren.
 *
 * Da das tatsächliche Overlay-Laden hochgradig plattformspezifisch ist
 * (PSX: Executable-Relocation, PC: nicht direkt anwendbar), sind diese
 * Teile als Stubs implementiert, die loggen, was sie tun würden.
 *
 * Anforderung: Requirements 4.4, 10.4
 */

#include "re15_room.h"
#include "re15_engine.h"
#include "re15_audio.h"
#include "re15_error.h"

#include <stdint.h>

/* ============================================================================
 * Interne Konstanten
 * ========================================================================= */

/** VAB-Bank-Slot der für stage-übergreifende BGM/SFX verwendet wird */
#define STAGE_VAB_BANK_SLOT  0

/** Maximale gültige Stage-Nummer */
#define STAGE_MAX            6

/** Minimale gültige Stage-Nummer */
#define STAGE_MIN            1

/* ============================================================================
 * Interne Hilfsfunktionen (Stubs)
 * ========================================================================= */

/**
 * Stub: Stage-Overlay laden.
 *
 * Auf PSX würde hier STAGE{N}.BIN nach 0x80100000 geladen werden.
 * Auf PC ist kein Overlay-Relocation nötig — wir loggen nur die Aktion.
 *
 * @param stage  Stage-Nummer (1-6)
 * @return       0 bei Erfolg (Stub: immer erfolgreich)
 */
static int stage_overlay_load(uint8_t stage)
{
    RE15_INFO("STAGE", "Overlay laden: STAGE%d.BIN (Stub — kein Relocation auf PC)",
              (int)stage);

    /*
     * TODO: PSX-Implementierung:
     *   1. Pfad konstruieren: "STAGE{N}/STAGE{N}.BIN"
     *   2. Datei via I/O-Backend lesen
     *   3. An Zieladresse 0x80100000 laden (PSX RAM)
     *   4. Instruction-Cache flushen
     *
     * PC-Implementierung:
     *   Nicht direkt anwendbar — Stage-spezifische Logik wird auf PC
     *   direkt in die Engine kompiliert (keine Runtime-Relocation).
     */

    return 0;
}

/**
 * Stub: Stage-spezifische VAB-Bank laden.
 *
 * Entlädt die aktuelle VAB-Bank im STAGE_VAB_BANK_SLOT und lädt die
 * neue Bank für den angegebenen Stage. Nutzt g_audio wenn verfügbar.
 *
 * @param stage  Stage-Nummer (1-6)
 * @return       0 bei Erfolg, -1 wenn Audio-Backend nicht verfügbar
 */
static int stage_vab_switch(uint8_t stage)
{
    if (g_audio == NULL) {
        RE15_WARN("STAGE", "Audio-Backend nicht verfügbar — VAB-Bank-Wechsel "
                  "für Stage %d übersprungen", (int)stage);
        return -1;
    }

    /* Alte Bank entladen */
    RE15_INFO("STAGE", "VAB-Bank entladen: Slot %d", STAGE_VAB_BANK_SLOT);
    g_audio->vab_unload(STAGE_VAB_BANK_SLOT);

    /*
     * TODO: Neue VAB-Bank laden.
     * Die tatsächlichen VH/VB-Daten müssten aus dem Dateisystem geladen
     * werden (z.B. "STAGE{N}/STAGE{N}.VH" + "STAGE{N}/STAGE{N}.VB").
     * Hier ist ein Stub — die eigentliche Implementierung erfolgt wenn
     * das Audio-System vollständig integriert ist.
     */
    RE15_INFO("STAGE", "VAB-Bank laden: Stage %d → Slot %d (Stub — VH/VB-Dateien "
              "noch nicht geladen)", (int)stage, STAGE_VAB_BANK_SLOT);

    return 0;
}

/**
 * Stub: Raumspezifische BGM-Tabelle für den neuen Stage aktivieren.
 *
 * Jeder Stage hat eine eigene BGM-Zuordnungstabelle, die bestimmt welche
 * SEQ-Dateien für welche Räume gespielt werden. Beim Stage-Wechsel wird
 * die Tabelle auf den neuen Stage umgeschaltet.
 *
 * @param stage  Stage-Nummer (1-6)
 */
static void stage_bgm_table_activate(uint8_t stage)
{
    RE15_INFO("STAGE", "BGM-Tabelle aktiviert: Stage %d (Stub — Tabellen "
              "noch nicht implementiert)", (int)stage);

    /*
     * TODO: Implementierung wenn BGM-System vollständig ist:
     *   1. BGM-Tabelle für Stage aus ROM/Datei laden oder statisch selektieren
     *   2. Aktuellen BGM stoppen (g_audio->seq_stop)
     *   3. Neue Tabelle als aktiv markieren
     *   4. BGM wird beim nächsten room_load aus der neuen Tabelle gestartet
     */
}

/* ============================================================================
 * Öffentliche API
 * ========================================================================= */

/**
 * Führt einen vollständigen Stage-Wechsel durch.
 *
 * Wird aufgerufen wenn ein Tür-Übergang (Door_aot_set_4p) in einen Raum
 * einer anderen Stage führt. Führt die folgenden Schritte aus:
 *
 *   1. Prüfe ob tatsächlich ein Stage-Wechsel nötig ist (no-op bei gleicher Stage)
 *   2. Logge den Stage-Übergang
 *   3. Lade das neue Stage-Overlay (Stub)
 *   4. Wechsle die VAB-Sound-Bank (entladen + neu laden)
 *   5. Aktiviere die raumspezifische BGM-Tabelle des neuen Stages
 *   6. Aktualisiere g_game.current_stage
 *
 * Nach dem Stage-Wechsel muss der Aufrufer re15_room_load() für den
 * Zielraum aufrufen.
 *
 * @param new_stage  Ziel-Stage (1-6)
 * @return           0 bei Erfolg, negativer Wert bei Fehler
 */
int re15_stage_change(uint8_t new_stage)
{
    /* Parametervalidierung */
    if (new_stage < STAGE_MIN || new_stage > STAGE_MAX) {
        RE15_ERROR("STAGE", "Ungültige Stage-Nummer: %d (gültig: %d-%d)",
                   (int)new_stage, STAGE_MIN, STAGE_MAX);
        return -1;
    }

    /* 1. No-op wenn gleiche Stage */
    if (new_stage == g_game.current_stage) {
        return 0;
    }

    /* 2. Stage-Übergang loggen */
    RE15_INFO("STAGE", "Stage-Wechsel: %d → %d",
              (int)g_game.current_stage, (int)new_stage);

    /* 3. Stage-Overlay laden (Stub) */
    stage_overlay_load(new_stage);

    /* 4. VAB-Bank wechseln */
    stage_vab_switch(new_stage);

    /* 5. BGM-Tabelle für neuen Stage aktivieren */
    stage_bgm_table_activate(new_stage);

    /* 6. Engine-Zustand aktualisieren */
    g_game.current_stage = new_stage;

    RE15_INFO("STAGE", "Stage-Wechsel abgeschlossen: aktive Stage = %d",
              (int)new_stage);

    return 0;
}
