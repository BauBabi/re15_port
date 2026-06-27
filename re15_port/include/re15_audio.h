/**
 * @file re15_audio.h
 * @brief Audio-Abstraktionsschicht: Backend-Interface für VAB, SEQ, SFX und Voice
 *
 * Definiert die plattformunabhängige Funktionszeiger-Tabelle (audio_backend_t),
 * über die beide Plattformen (PSX via SPU/libsnd, PC via SDL2-Audio) bedient
 * werden. Die Engine ruft ausschließlich über g_audio→xyz() auf — die
 * konkrete Implementierung wird beim Start eingehängt.
 *
 * Anforderung: Requirement 6.1
 */
#ifndef RE15_AUDIO_H
#define RE15_AUDIO_H

#include "re15_types.h"

/* ============================================================================
 * Konstanten
 * ========================================================================= */

/** Maximale Anzahl gleichzeitig geladener VAB-Bänke (0-15) */
#define RE15_AUDIO_MAX_BANKS    16

/** Maximale Sample-ID pro Bank (0-127) */
#define RE15_AUDIO_MAX_SAMPLES  128

/** Maximale Lautstärke (linear, 0 = stumm, 127 = max) */
#define RE15_AUDIO_VOLUME_MAX   127

/** Pan-Mitte (0 = links, 64 = Mitte, 127 = rechts) */
#define RE15_AUDIO_PAN_CENTER   64

/** Maximale SEQ-Playback-Slots */
#define RE15_AUDIO_MAX_SEQ_SLOTS 4

/* ============================================================================
 * Audio-Backend Funktionszeiger-Tabelle
 *
 * Identische Signatur für PSX (SPU) und PC (SDL2-Audio).
 * Die Engine greift ausschließlich über dieses Interface auf Audio zu.
 * ========================================================================= */

/**
 * Audio-Backend — Funktionszeiger-Tabelle.
 *
 * Jede Plattform stellt eine statische Instanz bereit und setzt g_audio
 * beim Start auf die eigene Tabelle.
 */
typedef struct {
    /**
     * Initialisiert das Audio-Subsystem.
     * Muss vor allen anderen Audio-Funktionen aufgerufen werden.
     */
    void (*init)(void);

    /**
     * Fährt das Audio-Subsystem herunter und gibt Ressourcen frei.
     */
    void (*shutdown)(void);

    /**
     * Lädt eine VAB-Sound-Bank (VH-Header + VB-Body) in das Audio-System.
     *
     * @param vh        Zeiger auf den VH-Header-Puffer (VAB-Header)
     * @param vh_size   Größe des VH-Puffers in Bytes
     * @param vb        Zeiger auf den VB-Body-Puffer (ADPCM-Samples)
     * @param vb_size   Größe des VB-Puffers in Bytes
     * @param bank_id   Bank-Slot (0 bis RE15_AUDIO_MAX_BANKS-1)
     * @return          0 bei Erfolg, negativer Fehlercode bei Fehlschlag
     */
    int (*vab_load)(const uint8_t* vh, int vh_size,
                    const uint8_t* vb, int vb_size, int bank_id);

    /**
     * Entlädt eine VAB-Sound-Bank und gibt deren Ressourcen frei.
     *
     * @param bank_id   Bank-Slot (0 bis RE15_AUDIO_MAX_BANKS-1)
     */
    void (*vab_unload)(int bank_id);

    /**
     * Startet SEQ-Playback (BGM) in einem Slot.
     *
     * SEQ-Daten werden als roher PSX-SEQ-Stream übergeben.
     * Bei loop != 0 wird der Track endlos wiederholt bis explizit gestoppt
     * oder durch einen neuen Track im selben Slot ersetzt.
     *
     * @param slot      Playback-Slot (0 bis RE15_AUDIO_MAX_SEQ_SLOTS-1)
     * @param seq_data  Zeiger auf SEQ-Daten
     * @param seq_size  Größe der SEQ-Daten in Bytes
     * @param loop      0 = einmalig, != 0 = Loop
     */
    void (*seq_play)(int slot, const uint8_t* seq_data, int seq_size, int loop);

    /**
     * Stoppt SEQ-Playback in einem Slot.
     *
     * @param slot      Playback-Slot (0 bis RE15_AUDIO_MAX_SEQ_SLOTS-1)
     */
    void (*seq_stop)(int slot);

    /**
     * Triggert einen Soundeffekt (SFX) — wird vom SCD-Opcode Se_on (0x36) aufgerufen.
     *
     * @param bank      VAB-Bank (0 bis RE15_AUDIO_MAX_BANKS-1)
     * @param sample_id Sample-Index innerhalb der Bank (0-127)
     * @param volume    Lautstärke (0-127, linear)
     * @param pan       Stereo-Position (0=links, 64=Mitte, 127=rechts)
     */
    void (*sfx_play)(int bank, int sample_id, int volume, int pan);

    /**
     * Spielt eine Voice-Nachricht (Sprach-Clip) für einen bestimmten Raum ab.
     *
     * @param room_id   Raum-ID (bestimmt Voice-Datei-Lookup)
     * @param msg_id    Nachrichten-Index innerhalb des Raums
     */
    void (*voice_play)(int room_id, int msg_id);

    /**
     * Setzt die Lautstärke für einen Audio-Kanal.
     *
     * @param channel       Kanal-Nummer
     * @param volume_0_127  Lautstärke (0 = stumm, 127 = Maximum, linear)
     */
    void (*set_volume)(int channel, int volume_0_127);

} audio_backend_t;

/* ============================================================================
 * Globaler Audio-Backend-Zeiger
 *
 * Wird beim Plattform-Init auf das jeweilige Backend gesetzt:
 * - PSX: &g_audio_psx  (aus platform/psx/src/audio_psx.c)
 * - PC:  &g_audio_pc   (aus platform/pc/src/audio_pc.c)
 * ========================================================================= */

/** Aktives Audio-Backend — zur Laufzeit gesetzt */
extern const audio_backend_t* g_audio;

/* ============================================================================
 * Plattform-Backend-Instanzen
 *
 * Jede Plattform definiert ihre eigene Backend-Instanz. Der Init-Code setzt
 * g_audio auf die passende Instanz.
 * ========================================================================= */

#ifdef RE15_PLATFORM_PC
/** PC Audio-Backend (SDL2 + ADPCM-Decoder, definiert in platform/pc/src/audio_pc.c) */
extern const audio_backend_t audio_pc_backend;
#endif

#ifdef RE15_PLATFORM_PSX
/** PSX Audio-Backend (SPU/libsnd, definiert in platform/psx/src/audio_psx.c) */
extern const audio_backend_t* g_audio_psx;
#endif

#endif /* RE15_AUDIO_H */
