/* =============================================================================
 * RE 1.5 Port — PSX Audio-Backend (SPU/libsnd via PSn00bSDK)
 * =============================================================================
 * Implementiert die audio_backend_t Schnittstelle für die PSX-Plattform.
 * Verwendet die SPU-Hardware (Sound Processing Unit) über PSn00bSDK:
 *   - SpuSetVoiceAttr: Voice-Attribute konfigurieren (Pitch, Volume, ADSR)
 *   - SpuSetKey: Voices ein-/ausschalten (Key On/Off)
 *   - SpuSetTransferStartAddr + SpuWrite: ADPCM-Daten in SPU-RAM laden
 *
 * SPU-Speicher: 512 KB, davon 0x1000 Bytes reserviert (Header).
 * Voices: 24 Hardware-Voices (0-23).
 * Format: PSX-ADPCM (4-bit, 28 Samples/Block + 16-Byte Header).
 *
 * VAB-Format:
 *   - VH (VAB Header): Programm-/Tone-Definitionen (Instrument-Mapping)
 *   - VB (VAB Body): ADPCM-Sample-Daten (direkt SPU-kompatibel)
 *
 * SEQ-Format:
 *   - MIDI-ähnlicher Sequencer-Stream (PSX-proprietär)
 *   - Playback über PSn00bSDK-Sequencer-API (SsSeqOpen, SsSeqPlay)
 *
 * Anforderung: Requirements 6.1, 6.2, 6.4, 6.5
 * ========================================================================== */

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <psxspu.h>
#include <psxapi.h>

#include "re15_types.h"
#include "re15_audio.h"
#include "re15_error.h"

/* ============================================================================
 * Konstanten
 * ========================================================================= */

/** SPU-RAM-Gesamtgröße (512 KB) */
#define SPU_RAM_SIZE        (512 * 1024)

/** Reservierter SPU-RAM-Bereich (System-Header, nicht benutzbar) */
#define SPU_RAM_RESERVED    0x1000

/** Erster freier SPU-RAM-Offset für Sample-Upload */
#define SPU_RAM_START       SPU_RAM_RESERVED

/** Maximale Voices auf der SPU-Hardware */
#define SPU_MAX_VOICES      24

/** Voices reserviert für SEQ-Playback (0-15 typisch) */
#define SPU_SEQ_VOICES      16

/** Voices für SFX (16-23) */
#define SPU_SFX_VOICE_START 16
#define SPU_SFX_VOICE_COUNT (SPU_MAX_VOICES - SPU_SFX_VOICE_START)

/** ADPCM-Block-Größe in Bytes */
#define ADPCM_BLOCK_SIZE    16

/** Default ADSR: Quick Attack, Sustain, Medium Release */
#define DEFAULT_ADSR1       0x80FF
#define DEFAULT_ADSR2       0x5FC0

/* ============================================================================
 * VAB-Header-Strukturen (VH-Format)
 *
 * VH-Datei enthält:
 *   - VAB-Header (32 Bytes): Magic, Version, Bank-ID, Größe, etc.
 *   - 128 Programme à 16 Tones (Instrument-Definitionen)
 *   - Tone-Attribute: Sample-Offset, Pitch, Volume, ADSR
 * ========================================================================= */

/** VAB-File-Header (erste 32 Bytes der VH-Datei) */
typedef struct {
    uint32_t magic;         /**< "VABp" (0x70424156)                */
    uint32_t version;       /**< VAB-Version (typisch 6 oder 7)     */
    uint32_t vab_id;        /**< Bank-Identifier                     */
    uint32_t file_size;     /**< Gesamtgröße VH + VB                */
    uint16_t reserved0;
    uint16_t num_programs;  /**< Anzahl Programme (max 128)          */
    uint16_t num_tones;     /**< Anzahl Tones total                  */
    uint16_t num_vags;      /**< Anzahl VAG-Samples (Waveforms)      */
    uint8_t  master_vol;    /**< Master-Volume (0-127)               */
    uint8_t  master_pan;    /**< Master-Pan (0-127, 64=Mitte)        */
    uint8_t  attr1;         /**< Bank-Attribute 1                    */
    uint8_t  attr2;         /**< Bank-Attribute 2                    */
    uint32_t reserved1;
} vab_header_t;

/** VAB Tone-Attribute (16 Bytes pro Tone) */
typedef struct {
    uint8_t  priority;      /**< Priorität (0-127)                   */
    uint8_t  mode;          /**< Reverb-Modus                        */
    uint8_t  volume;        /**< Tone-Volume (0-127)                 */
    uint8_t  pan;           /**< Tone-Pan (0-127)                    */
    uint8_t  center_note;   /**< Grundton (MIDI-Note, 60=C4)         */
    uint8_t  center_fine;   /**< Feinstimmung                        */
    uint8_t  note_min;      /**< Niedrigste spielbare Note           */
    uint8_t  note_max;      /**< Höchste spielbare Note              */
    uint8_t  vibrato_width; /**< Vibrato-Breite                      */
    uint8_t  vibrato_time;  /**< Vibrato-Geschwindigkeit             */
    uint8_t  portamento_w;  /**< Portamento-Breite                   */
    uint8_t  portamento_t;  /**< Portamento-Zeit                     */
    uint8_t  pitchbend_min; /**< Pitch-Bend Minimum                  */
    uint8_t  pitchbend_max; /**< Pitch-Bend Maximum                  */
    uint8_t  reserved1;
    uint8_t  reserved2;
    uint16_t adsr1;         /**< ADSR Attack/Decay/Sustain Level     */
    uint16_t adsr2;         /**< ADSR Sustain Rate/Release Rate      */
    int16_t  prog_num;      /**< Programm-Nummer (Parent)            */
    int16_t  vag_id;        /**< VAG-Sample-Index (0-basiert)        */
    int16_t  reserved3[4];
} vab_tone_attr_t;

/* ============================================================================
 * Interner Zustand
 * ========================================================================= */

/** Bank-Informationen pro geladener VAB-Bank */
typedef struct {
    uint8_t  loaded;            /**< 1 = Bank geladen, 0 = frei          */
    uint16_t num_programs;      /**< Anzahl Programme in dieser Bank      */
    uint16_t num_tones;         /**< Anzahl Tones                         */
    uint16_t num_vags;          /**< Anzahl VAG-Samples                   */
    uint32_t spu_base_addr;     /**< Start-Adresse der Bank im SPU-RAM    */
    uint32_t spu_size;          /**< Belegte Bytes im SPU-RAM             */
    uint32_t vag_offsets[128];  /**< SPU-RAM-Offsets pro VAG-Sample       */
} audio_psx_bank_t;

/** SEQ-Playback-Slot */
typedef struct {
    uint8_t  active;            /**< 1 = aktiv, 0 = inaktiv              */
    uint8_t  loop;              /**< 1 = Endlosschleife                  */
    int      seq_handle;        /**< SEQ-Handle (von SsSeqOpen)          */
} audio_psx_seq_slot_t;

/** Gesamter Audio-Zustand */
static struct {
    audio_psx_bank_t banks[RE15_AUDIO_MAX_BANKS];
    audio_psx_seq_slot_t seq_slots[RE15_AUDIO_MAX_SEQ_SLOTS];

    /** Nächste freie SPU-RAM-Adresse für Uploads */
    uint32_t spu_alloc_ptr;

    /** Round-Robin Voice-Index für SFX */
    uint8_t  sfx_voice_rr;

    /** Initialisiert-Flag */
    uint8_t  initialized;
} s_apsx;

/* ============================================================================
 * SPU-RAM-Allokation
 *
 * Einfacher linearer Allokator: Neue Banks werden hintereinander in den
 * SPU-RAM geladen. Bei vab_unload wird der Speicher nur freigegeben wenn
 * die Bank die "letzte" im SPU-RAM ist (Stack-Muster). Andernfalls wird
 * der Speicher als fragmentiert akzeptiert (RE2-Verhalten).
 * ========================================================================= */

/**
 * Reserviert size Bytes im SPU-RAM.
 * @param size  Anzahl Bytes (muss auf ADPCM_BLOCK_SIZE aligned sein)
 * @return SPU-RAM-Adresse oder 0 bei Fehler (kein Platz).
 */
static uint32_t spu_ram_alloc(uint32_t size)
{
    uint32_t addr;

    /* Auf 16-Byte ADPCM-Block-Grenze aufrunden */
    size = (size + (ADPCM_BLOCK_SIZE - 1)) & ~(ADPCM_BLOCK_SIZE - 1);

    if (s_apsx.spu_alloc_ptr + size > SPU_RAM_SIZE) {
        RE15_ERROR("AUDIO_PSX", "SPU-RAM voll: benötigt %u Bytes, "
                   "verfügbar %u", size,
                   SPU_RAM_SIZE - s_apsx.spu_alloc_ptr);
        return 0;
    }

    addr = s_apsx.spu_alloc_ptr;
    s_apsx.spu_alloc_ptr += size;

    return addr;
}

/* ============================================================================
 * SPU Transfer-Hilfsfunktionen
 * ========================================================================= */

/**
 * Überträgt ADPCM-Daten in den SPU-RAM.
 *
 * Verwendet SpuSetTransferStartAddr + SpuWrite für den DMA-Transfer.
 * Wartet synchron auf Abschluss (SpuIsTransferCompleted).
 *
 * @param spu_addr  Ziel-Adresse im SPU-RAM
 * @param data      Quelldaten (ADPCM-Samples)
 * @param size      Größe in Bytes
 * @return 0 bei Erfolg, -1 bei Fehler.
 */
static int spu_upload(uint32_t spu_addr, const uint8_t* data, uint32_t size)
{
    if (!data || size == 0) return -1;

    /* Transfer-Modus: DMA */
    SpuSetTransferMode(SPU_TRANSFER_BY_DMA);

    /* Startadresse im SPU-RAM setzen */
    SpuSetTransferStartAddr(spu_addr);

    /* Daten übertragen */
    SpuWrite((uint32_t*)data, size);

    /* Auf Abschluss warten (synchron) */
    while (!SpuIsTransferCompleted(SPU_TRANSFER_WAIT))
        ;

    return 0;
}

/* ============================================================================
 * Backend-Implementierung: init / shutdown
 * ========================================================================= */

/**
 * Initialisiert das SPU-Audio-Subsystem.
 *
 * Konfiguriert die SPU-Hardware:
 *   - SpuInit: SPU zurücksetzen und initialisieren
 *   - Master-Volume auf Maximum setzen
 *   - SPU-RAM-Allokator auf ersten freien Offset setzen
 *   - Alle Banks und SEQ-Slots als unbenutzt markieren
 */
static void psx_audio_init(void)
{
    int i;

    if (s_apsx.initialized) return;

    /* SPU-Hardware initialisieren */
    SpuInit();

    /* Master-Volume auf Maximum setzen (links + rechts) */
    SpuSetCommonAttr(SPU_COMMON_MVOLL | SPU_COMMON_MVOLR,
                     0x3FFF, 0x3FFF, 0, 0, 0);

    /* SPU-RAM-Allokator initialisieren */
    s_apsx.spu_alloc_ptr = SPU_RAM_START;

    /* Round-Robin Voice-Index für SFX */
    s_apsx.sfx_voice_rr = SPU_SFX_VOICE_START;

    /* Alle Banks zurücksetzen */
    for (i = 0; i < RE15_AUDIO_MAX_BANKS; i++) {
        memset(&s_apsx.banks[i], 0, sizeof(audio_psx_bank_t));
    }

    /* Alle SEQ-Slots zurücksetzen */
    for (i = 0; i < RE15_AUDIO_MAX_SEQ_SLOTS; i++) {
        s_apsx.seq_slots[i].active = 0;
        s_apsx.seq_slots[i].loop = 0;
        s_apsx.seq_slots[i].seq_handle = -1;
    }

    s_apsx.initialized = 1;

    RE15_INFO("AUDIO_PSX", "SPU initialisiert (512KB RAM, 24 Voices)");
}

/**
 * Fährt das SPU-Audio-Subsystem herunter.
 *
 * Stoppt alle Voices und SEQ-Playback, gibt SPU-RAM-Tracking frei.
 */
static void psx_audio_shutdown(void)
{
    int i;

    if (!s_apsx.initialized) return;

    /* Alle SEQ-Slots stoppen */
    for (i = 0; i < RE15_AUDIO_MAX_SEQ_SLOTS; i++) {
        if (s_apsx.seq_slots[i].active) {
            SsSeqStop(s_apsx.seq_slots[i].seq_handle);
            SsSeqClose(s_apsx.seq_slots[i].seq_handle);
            s_apsx.seq_slots[i].active = 0;
        }
    }

    /* Alle 24 Voices abschalten */
    SpuSetKey(SPU_OFF, 0x00FFFFFF);

    s_apsx.initialized = 0;
    RE15_INFO("AUDIO_PSX", "SPU heruntergefahren");
}

/* ============================================================================
 * Backend-Implementierung: VAB Load / Unload
 * ========================================================================= */

/**
 * Lädt eine VAB-Sound-Bank (VH-Header + VB-Body) in SPU-RAM.
 *
 * Ablauf:
 *   1. VH-Header parsen → Anzahl VAG-Samples und deren Größen ermitteln
 *   2. SPU-RAM für alle Samples reservieren (linearer Allokator)
 *   3. VB-Daten (ADPCM-Samples) via DMA in SPU-RAM hochladen
 *   4. Per-Sample SPU-RAM-Offsets in der Bank-Tabelle speichern
 *
 * PSX-ADPCM-Samples im VB-Body sind bereits im SPU-nativen Format
 * (4-bit ADPCM, 28 Samples/Block à 16 Bytes). Kein Transcoding nötig.
 *
 * @param vh        VH-Header-Daten
 * @param vh_size   Größe des VH-Headers
 * @param vb        VB-Body-Daten (ADPCM-Samples, konkateniert)
 * @param vb_size   Gesamtgröße des VB-Body
 * @param bank_id   Ziel-Bank-Slot (0-15)
 * @return 0 bei Erfolg, -1 bei Fehler.
 */
static int psx_audio_vab_load(const uint8_t* vh, int vh_size,
                               const uint8_t* vb, int vb_size,
                               int bank_id)
{
    const vab_header_t* hdr;
    audio_psx_bank_t* bank;
    uint32_t spu_addr;
    const uint16_t* vag_sizes;
    uint32_t offset_in_vb;
    int i;

    /* Parameter-Validierung */
    if (!vh || !vb || vh_size < (int)sizeof(vab_header_t)) {
        RE15_ERROR("AUDIO_PSX", "vab_load: Ungültige Parameter");
        return -1;
    }

    if (bank_id < 0 || bank_id >= RE15_AUDIO_MAX_BANKS) {
        RE15_ERROR("AUDIO_PSX", "vab_load: Bank-ID %d außerhalb [0,%d)",
                   bank_id, RE15_AUDIO_MAX_BANKS);
        return -1;
    }

    bank = &s_apsx.banks[bank_id];

    /* Falls Bank bereits belegt: erst entladen */
    if (bank->loaded) {
        RE15_WARN("AUDIO_PSX", "vab_load: Bank %d bereits geladen, "
                  "wird überschrieben", bank_id);
        /* Hinweis: SPU-RAM wird nicht freigegeben (Fragment akzeptiert) */
        bank->loaded = 0;
    }

    /* VH-Header parsen */
    hdr = (const vab_header_t*)vh;

    /* Magic prüfen: "VABp" = 0x70424156 (Little-Endian) */
    if (hdr->magic != 0x70424156) {
        RE15_ERROR("AUDIO_PSX", "vab_load: Ungültiger VAB-Magic 0x%08X "
                   "(erwartet 0x70424156)", hdr->magic);
        return -1;
    }

    bank->num_programs = hdr->num_programs;
    bank->num_tones    = hdr->num_tones;
    bank->num_vags     = hdr->num_vags;

    if (bank->num_vags == 0 || bank->num_vags > 128) {
        RE15_WARN("AUDIO_PSX", "vab_load: Bank %d hat %d VAGs",
                  bank_id, bank->num_vags);
        if (bank->num_vags == 0) return -1;
        if (bank->num_vags > 128) bank->num_vags = 128;
    }

    /*
     * VAG-Größen-Tabelle: Im VH-Header nach dem Programm-/Tone-Block.
     * Position: Offset 32 (Header) + 128*16 (Programs) + num_tones*32 (Tones)
     * Jeder Eintrag: uint16_t, Größe in 8-Byte-Einheiten (×8 für Bytes)
     *
     * Vereinfachte Berechnung des Offsets zur VAG-Size-Tabelle:
     * Standard PSX-VAB: Header(32) + Programs(128×4) + Tones(16×32) = 32+512+512 = 1056
     * Für variables num_tones: 32 + 512 + num_tones*32
     */
    {
        uint32_t vag_table_offset = 32 + (128 * 4) + (bank->num_tones * 32);

        if ((int)(vag_table_offset + bank->num_vags * 2) > vh_size) {
            RE15_ERROR("AUDIO_PSX", "vab_load: VH zu klein für VAG-Tabelle "
                       "(offset=%u, vh_size=%d)", vag_table_offset, vh_size);
            return -1;
        }

        vag_sizes = (const uint16_t*)(vh + vag_table_offset);
    }

    /* SPU-RAM für gesamten VB-Body reservieren */
    spu_addr = spu_ram_alloc((uint32_t)vb_size);
    if (spu_addr == 0) {
        RE15_ERROR("AUDIO_PSX", "vab_load: Kein SPU-RAM für Bank %d "
                   "(%d Bytes)", bank_id, vb_size);
        return -1;
    }

    bank->spu_base_addr = spu_addr;
    bank->spu_size = (uint32_t)vb_size;

    /* VB-Body in SPU-RAM hochladen (ein zusammenhängender Block) */
    if (spu_upload(spu_addr, vb, (uint32_t)vb_size) < 0) {
        RE15_ERROR("AUDIO_PSX", "vab_load: SPU-Transfer fehlgeschlagen "
                   "(Bank %d)", bank_id);
        return -1;
    }

    /* Per-VAG-Offsets berechnen (relativ zum SPU-RAM-Basis) */
    offset_in_vb = 0;
    for (i = 0; i < (int)bank->num_vags; i++) {
        /* VAG-Größe: Tabellenwert × 8 ergibt tatsächliche Byte-Größe */
        uint32_t vag_byte_size = (uint32_t)vag_sizes[i] * 8;

        bank->vag_offsets[i] = spu_addr + offset_in_vb;
        offset_in_vb += vag_byte_size;

        /* Sicherheits-Check: Nicht über VB-Grenzen hinaus */
        if (offset_in_vb > (uint32_t)vb_size) {
            RE15_WARN("AUDIO_PSX", "vab_load: VAG %d überschreitet VB "
                      "(offset=%u, vb_size=%d)", i, offset_in_vb, vb_size);
            bank->num_vags = (uint16_t)i;
            break;
        }
    }

    bank->loaded = 1;

    RE15_INFO("AUDIO_PSX", "VAB-Bank %d geladen: %d Progs, %d Tones, "
              "%d VAGs, SPU@0x%05X (%d Bytes)",
              bank_id, bank->num_programs, bank->num_tones,
              bank->num_vags, spu_addr, vb_size);

    return 0;
}

/**
 * Entlädt eine VAB-Sound-Bank.
 *
 * Markiert die Bank als frei. SPU-RAM wird nur freigegeben wenn die
 * Bank die zuletzt allokierte war (Stack-Deallokation). Andernfalls
 * bleibt ein Fragment — dies ist akzeptiert (RE2-Verhalten: Banks
 * werden pro Stage geladen und gemeinsam freigegeben).
 *
 * @param bank_id  Bank-Slot (0-15)
 */
static void psx_audio_vab_unload(int bank_id)
{
    audio_psx_bank_t* bank;

    if (bank_id < 0 || bank_id >= RE15_AUDIO_MAX_BANKS) return;

    bank = &s_apsx.banks[bank_id];

    if (!bank->loaded) return;

    /* Stack-Deallokation: Wenn diese Bank die letzte im SPU-RAM ist,
     * Allokator zurücksetzen */
    if (bank->spu_base_addr + bank->spu_size == s_apsx.spu_alloc_ptr) {
        s_apsx.spu_alloc_ptr = bank->spu_base_addr;
    }

    bank->loaded = 0;
    bank->num_programs = 0;
    bank->num_tones = 0;
    bank->num_vags = 0;
    bank->spu_base_addr = 0;
    bank->spu_size = 0;

    RE15_INFO("AUDIO_PSX", "VAB-Bank %d entladen", bank_id);
}

/* ============================================================================
 * Backend-Implementierung: SEQ Playback
 * ========================================================================= */

/**
 * Startet SEQ-Playback (BGM) in einem Slot.
 *
 * Verwendet PSn00bSDK Sequencer-API:
 *   - SsSeqOpen: SEQ-Daten registrieren, Handle erhalten
 *   - SsSeqPlay: Playback starten (mit Loop-Flag)
 *
 * Falls der Slot bereits aktiv ist, wird die laufende Sequenz gestoppt
 * und durch die neue ersetzt.
 *
 * @param slot      Playback-Slot (0-3)
 * @param seq_data  SEQ-Datenstrom
 * @param seq_size  Größe in Bytes
 * @param loop      0 = einmalig, != 0 = Endlosschleife
 */
static void psx_audio_seq_play(int slot, const uint8_t* seq_data,
                                int seq_size, int loop)
{
    audio_psx_seq_slot_t* ss;

    if (slot < 0 || slot >= RE15_AUDIO_MAX_SEQ_SLOTS) {
        RE15_WARN("AUDIO_PSX", "seq_play: Slot %d ungültig", slot);
        return;
    }

    if (!seq_data || seq_size <= 0) {
        RE15_WARN("AUDIO_PSX", "seq_play: Ungültige SEQ-Daten");
        return;
    }

    ss = &s_apsx.seq_slots[slot];

    /* Laufende Sequenz im Slot stoppen */
    if (ss->active) {
        SsSeqStop(ss->seq_handle);
        SsSeqClose(ss->seq_handle);
        ss->active = 0;
    }

    /* SEQ-Daten beim Sequencer registrieren */
    ss->seq_handle = SsSeqOpen((uint32_t*)seq_data, 0);
    if (ss->seq_handle < 0) {
        RE15_ERROR("AUDIO_PSX", "seq_play: SsSeqOpen fehlgeschlagen "
                   "(Slot %d)", slot);
        return;
    }

    /* Playback starten */
    ss->loop = (loop != 0) ? 1 : 0;
    SsSeqPlay(ss->seq_handle, SSPLAY_PLAY, ss->loop ? SSPLAY_INFINITY : 1);
    ss->active = 1;

    RE15_INFO("AUDIO_PSX", "SEQ-Play Slot %d (handle=%d, loop=%d, %d Bytes)",
              slot, ss->seq_handle, ss->loop, seq_size);
}

/**
 * Stoppt SEQ-Playback in einem Slot.
 *
 * @param slot  Playback-Slot (0-3)
 */
static void psx_audio_seq_stop(int slot)
{
    audio_psx_seq_slot_t* ss;

    if (slot < 0 || slot >= RE15_AUDIO_MAX_SEQ_SLOTS) return;

    ss = &s_apsx.seq_slots[slot];

    if (!ss->active) return;

    SsSeqStop(ss->seq_handle);
    SsSeqClose(ss->seq_handle);

    ss->active = 0;
    ss->seq_handle = -1;

    RE15_INFO("AUDIO_PSX", "SEQ-Stop Slot %d", slot);
}

/* ============================================================================
 * Backend-Implementierung: SFX Playback
 * ========================================================================= */

/**
 * Triggert einen Soundeffekt über die SPU.
 *
 * Verwendet eine der SFX-reservierten Voices (16-23) im Round-Robin-Verfahren.
 * Konfiguriert die Voice mit:
 *   - SPU-RAM-Adresse des Samples (aus VAB-Bank vag_offsets)
 *   - Lautstärke (Volume links/rechts berechnet aus volume + pan)
 *   - Standard-Pitch (44100 Hz = 0x1000 = Basis-Samplerate)
 *   - Standard-ADSR (Quick Attack, Sustain, Medium Release)
 *
 * Danach: SpuSetKey(SPU_ON) für die gewählte Voice.
 *
 * @param bank       VAB-Bank (0-15)
 * @param sample_id  Sample-Index innerhalb der Bank (0-127)
 * @param volume     Lautstärke (0-127)
 * @param pan        Stereo-Position (0=links, 64=Mitte, 127=rechts)
 */
static void psx_audio_sfx_play(int bank, int sample_id,
                                int volume, int pan)
{
    audio_psx_bank_t* b;
    SpuVoiceAttr attr;
    uint8_t voice;
    int vol_left, vol_right;

    /* Parameter-Validierung */
    if (bank < 0 || bank >= RE15_AUDIO_MAX_BANKS) return;
    if (sample_id < 0 || sample_id >= RE15_AUDIO_MAX_SAMPLES) return;

    b = &s_apsx.banks[bank];
    if (!b->loaded) {
        RE15_WARN("AUDIO_PSX", "sfx_play: Bank %d nicht geladen", bank);
        return;
    }

    if (sample_id >= (int)b->num_vags) {
        RE15_WARN("AUDIO_PSX", "sfx_play: Sample %d nicht in Bank %d "
                  "(max %d)", sample_id, bank, b->num_vags);
        return;
    }

    /* Round-Robin Voice-Zuweisung (Voices 16-23 für SFX) */
    voice = s_apsx.sfx_voice_rr;
    s_apsx.sfx_voice_rr++;
    if (s_apsx.sfx_voice_rr >= SPU_SFX_VOICE_START + SPU_SFX_VOICE_COUNT) {
        s_apsx.sfx_voice_rr = SPU_SFX_VOICE_START;
    }

    /* Volume clamping */
    if (volume < 0) volume = 0;
    if (volume > RE15_AUDIO_VOLUME_MAX) volume = RE15_AUDIO_VOLUME_MAX;
    if (pan < 0) pan = 0;
    if (pan > 127) pan = 127;

    /*
     * Stereo-Volume aus pan berechnen:
     *   pan=0   → links=max, rechts=0
     *   pan=64  → links=max, rechts=max
     *   pan=127 → links=0, rechts=max
     *
     * SPU-Volume-Range: 0x0000 - 0x3FFF
     * Skalierung: (volume/127) × (pan_factor) × 0x3FFF
     */
    {
        int vol_scale = (volume * 0x3FFF) / RE15_AUDIO_VOLUME_MAX;

        if (pan <= 64) {
            vol_left  = vol_scale;
            vol_right = (vol_scale * pan) / 64;
        } else {
            vol_left  = (vol_scale * (127 - pan)) / 63;
            vol_right = vol_scale;
        }
    }

    /* Voice-Attribute konfigurieren */
    memset(&attr, 0, sizeof(SpuVoiceAttr));

    attr.voice       = (1 << voice);
    attr.mask        = SPU_VOICE_VOLL | SPU_VOICE_VOLR |
                       SPU_VOICE_PITCH | SPU_VOICE_ADDR |
                       SPU_VOICE_ADSR1 | SPU_VOICE_ADSR2;
    attr.volume.left  = (int16_t)vol_left;
    attr.volume.right = (int16_t)vol_right;
    attr.pitch        = 0x1000;  /* Basis-Samplerate (44100 Hz) */
    attr.addr         = b->vag_offsets[sample_id];
    attr.adsr1        = DEFAULT_ADSR1;
    attr.adsr2        = DEFAULT_ADSR2;

    SpuSetVoiceAttr(&attr);

    /* Voice starten (Key On) */
    SpuSetKey(SPU_ON, (1 << voice));
}

/* ============================================================================
 * Backend-Implementierung: Voice Playback
 * ========================================================================= */

/**
 * Spielt eine Voice-Nachricht (Sprach-Clip) für einen bestimmten Raum ab.
 *
 * Voice-Clips werden auf der PSX als separate ADPCM-Samples in einer
 * dedizierten Voice-Bank geladen. Das Mapping room_id+msg_id → Sample-ID
 * erfolgt über eine Lookup-Tabelle (hier: direkte Berechnung).
 *
 * @param room_id  Raum-ID
 * @param msg_id   Nachrichten-Index
 */
static void psx_audio_voice_play(int room_id, int msg_id)
{
    /*
     * Voice-Playback: Auf der PSX werden Sprach-Clips typischerweise
     * aus einer separaten Voice-Bank (Bank 15 per Konvention) abgespielt.
     * Das Mapping ist raumspezifisch.
     *
     * Für den Moment: SFX-Play auf der dedizierten Voice-Bank verwenden.
     * Die Voice-Bank muss vorher als Bank 15 geladen sein.
     */
    int voice_bank = RE15_AUDIO_MAX_BANKS - 1;  /* Bank 15 = Voice-Bank */
    int voice_sample;

    /* Einfaches Mapping: msg_id direkt als Sample-Index */
    voice_sample = msg_id;

    RE15_INFO("AUDIO_PSX", "Voice: Room %d, Msg %d → Bank %d, Sample %d",
              room_id, msg_id, voice_bank, voice_sample);

    /* Über reguläre SFX-Pipeline abspielen (zentriert, volle Lautstärke) */
    psx_audio_sfx_play(voice_bank, voice_sample,
                       RE15_AUDIO_VOLUME_MAX, RE15_AUDIO_PAN_CENTER);

    (void)room_id;  /* Room-ID für zukünftiges erweitertes Mapping */
}

/* ============================================================================
 * Backend-Implementierung: Volume Control
 * ========================================================================= */

/**
 * Setzt die Lautstärke für einen Audio-Kanal.
 *
 * Kanal 0 = Master-Volume (beeinflusst SPU-Master-Volume).
 * Kanal 1-23 = einzelne SPU-Voices.
 *
 * @param channel       Kanal (0=Master, 1-23=Voice)
 * @param volume_0_127  Lautstärke (0=stumm, 127=Maximum)
 */
static void psx_audio_set_volume(int channel, int volume_0_127)
{
    int spu_vol;

    /* Clamping */
    if (volume_0_127 < 0) volume_0_127 = 0;
    if (volume_0_127 > RE15_AUDIO_VOLUME_MAX) volume_0_127 = RE15_AUDIO_VOLUME_MAX;

    /* Auf SPU-Range skalieren (0-0x3FFF) */
    spu_vol = (volume_0_127 * 0x3FFF) / RE15_AUDIO_VOLUME_MAX;

    if (channel == 0) {
        /* Master-Volume (beide Kanäle) */
        SpuSetCommonAttr(SPU_COMMON_MVOLL | SPU_COMMON_MVOLR,
                         (int16_t)spu_vol, (int16_t)spu_vol, 0, 0, 0);
    } else if (channel >= 1 && channel <= SPU_MAX_VOICES) {
        /* Einzelne Voice-Lautstärke */
        SpuVoiceAttr attr;
        int voice_idx = channel - 1;

        memset(&attr, 0, sizeof(SpuVoiceAttr));
        attr.voice        = (1 << voice_idx);
        attr.mask         = SPU_VOICE_VOLL | SPU_VOICE_VOLR;
        attr.volume.left  = (int16_t)spu_vol;
        attr.volume.right = (int16_t)spu_vol;

        SpuSetVoiceAttr(&attr);
    }
}

/* ============================================================================
 * Exportierte Backend-Instanz
 * ========================================================================= */

static const audio_backend_t s_audio_psx_backend = {
    .init       = psx_audio_init,
    .shutdown   = psx_audio_shutdown,
    .vab_load   = psx_audio_vab_load,
    .vab_unload = psx_audio_vab_unload,
    .seq_play   = psx_audio_seq_play,
    .seq_stop   = psx_audio_seq_stop,
    .sfx_play   = psx_audio_sfx_play,
    .voice_play = psx_audio_voice_play,
    .set_volume = psx_audio_set_volume,
};

/** Globaler Zeiger auf das PSX-Audio-Backend (extern deklariert in re15_audio.h) */
const audio_backend_t* g_audio_psx = &s_audio_psx_backend;
