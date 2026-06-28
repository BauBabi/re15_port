/**
 * @file audio_pc.c
 * @brief PC Audio-Backend: SDL2 Audio-Callback + PSX-ADPCM-Decoder + 16-Kanal-Mixer
 *
 * Implementiert die audio_backend_t Schnittstelle für die PC-Plattform.
 * - PSX-ADPCM-Dekodierung: 4-bit Nibbles → 16-bit PCM (Filter-Koeffizienten f0/f1)
 * - SDL2 Audio-Callback bei 44100 Hz, Stereo (S16)
 * - 16-Kanal-Mixer mit linearem Volume (0-127) und Pan (0-127, 64=Mitte)
 * - VAB-Laden: VH-Header parsen, VB-Body als ADPCM-Blöcke speichern
 * - SEQ→MIDI→Sample-Playback mit Loop-Unterstützung
 * - SFX via sfx_play (Bank 0-15, ID 0-127, Vol 0-127, Pan 0-127)
 *
 * Fehlerfall: VAB nicht ladbar → Warnung auf stderr, ohne Audio für Bank fortfahren.
 *
 * Requirements: 6.1, 6.3, 6.4, 6.5, 6.6
 */

#include "re15_audio.h"
#include "re15_error.h"

#include <SDL.h>
#include <string.h>
#include <stdlib.h>

/* ============================================================================
 * Konstanten
 * ========================================================================= */

/** Ausgabe-Samplerate */
#define AUDIO_SAMPLE_RATE       44100

/** Ausgabe-Kanäle (Stereo) */
#define AUDIO_CHANNELS          2

/** Audio-Buffer-Größe in Samples (pro Callback-Aufruf) */
#define AUDIO_BUFFER_SAMPLES    1024

/** Maximale Mixer-Kanäle */
#define MIXER_CHANNELS          16

/** PSX-ADPCM: Samples pro 16-Byte-Block */
#define ADPCM_SAMPLES_PER_BLOCK 28

/** PSX-ADPCM: Block-Größe in Bytes */
#define ADPCM_BLOCK_SIZE        16

/** PSX Original-Samplerate (ca. 44100 Hz für viele Samples) */
#define PSX_BASE_SAMPLE_RATE    44100

/** VAB Magic "pBAV" in little-endian */
#define VAB_MAGIC               0x56414270  /* "pBAV" */

/** Maximale Samples pro VAB-Bank */
#define VAB_MAX_TONES           128

/** Maximale Programme pro VAB */
#define VAB_MAX_PROGRAMS        128

/** SEQ Playback-Slots */
#define SEQ_MAX_SLOTS           RE15_AUDIO_MAX_SEQ_SLOTS

/* ============================================================================
 * PSX-ADPCM Filter-Koeffizienten (f0, f1)
 *
 * Die PSX-SPU nutzt 5 Filterkombinationen für prädiktive Dekodierung.
 * Werte als Q12 (Fixkomma * 4096):
 *   Filter 0: f0=0,     f1=0
 *   Filter 1: f0=60/64, f1=0
 *   Filter 2: f0=115/64,f1=-52/64
 *   Filter 3: f0=98/64, f1=-55/64
 *   Filter 4: f0=122/64,f1=-60/64
 * ========================================================================= */

static const int16_t s_adpcm_filter_f0[5] = {
       0,   60,  115,   98,  122
};
static const int16_t s_adpcm_filter_f1[5] = {
       0,    0,  -52,  -55,  -60
};

/* ============================================================================
 * Datenstrukturen
 * ========================================================================= */

/** Dekodiertes Sample (PCM-Daten aus ADPCM) */
typedef struct {
    int16_t* pcm_data;      /**< Dekodierte PCM-Samples (16-bit signed) */
    int      sample_count;  /**< Anzahl PCM-Samples */
    int      sample_rate;   /**< Samplerate (typisch 44100 oder 22050) */
    int      loop_start;    /**< Loop-Startpunkt in Samples (-1 = kein Loop) */
    int      loop_end;      /**< Loop-Endpunkt in Samples */
} pcm_sample_t;

/** VAB-Bank: Enthält dekodierte Samples für eine Sound-Bank */
typedef struct {
    int         loaded;                     /**< 1 wenn Bank geladen */
    pcm_sample_t samples[VAB_MAX_TONES];    /**< Dekodierte Samples */
    int         sample_count;               /**< Tatsächliche Anzahl Samples */
} vab_bank_t;

/** Mixer-Kanal-Zustand */
typedef struct {
    int         active;         /**< 1 = Kanal spielt ab */
    int         bank_id;        /**< Zugehörige VAB-Bank */
    int         sample_id;      /**< Sample-Index in der Bank */
    int         position;       /**< Aktuelle Abspielposition (in Samples) */
    int         volume;         /**< Lautstärke 0-127 */
    int         pan;            /**< Stereo-Pan 0-127 (64=Mitte) */
    int         looping;        /**< 1 = Loop-Modus */
} mixer_channel_t;

/** SEQ-Playback-Zustand (vereinfachter MIDI-Sequencer) */
typedef struct {
    int             active;         /**< 1 = Slot aktiv */
    const uint8_t*  seq_data;       /**< Zeiger auf SEQ-Rohdaten */
    int             seq_size;       /**< Größe der SEQ-Daten */
    int             loop;           /**< 1 = endlos wiederholen */
    int             position;       /**< Aktuelle Byte-Position im SEQ-Stream */
    int             tick_counter;   /**< Ticks bis zum nächsten Event */
    int             tempo;          /**< Mikrosekunden pro Quarter-Note */
    int             ticks_per_qn;   /**< Ticks pro Quarter-Note (Resolution) */
    int             samples_per_tick; /**< Audio-Samples pro SEQ-Tick */
    int             sample_counter; /**< Sample-Countdown bis nächster Tick */
    int             bank_id;        /**< Zugeordnete VAB-Bank */
} seq_slot_t;

/* ============================================================================
 * Backend-State (statisch, Thread-sicher über SDL_LockAudioDevice)
 * ========================================================================= */

static struct {
    SDL_AudioDeviceID   device;
    int                 initialized;

    /* VAB-Bänke (0-15) */
    vab_bank_t          banks[RE15_AUDIO_MAX_BANKS];

    /* 16-Kanal-Mixer */
    mixer_channel_t     channels[MIXER_CHANNELS];

    /* SEQ-Playback-Slots */
    seq_slot_t          seq_slots[SEQ_MAX_SLOTS];

    /* Master-Volume */
    int                 master_volume;  /* 0-127 */
} s_audio;

/* ============================================================================
 * PSX-ADPCM-Dekodierung
 *
 * Ein ADPCM-Block ist 16 Bytes:
 *   Byte 0: Shift/Range (untere 4 Bits) + Filter-Index (obere 4 Bits >> 4)
 *   Byte 1: Flags (Loop-Start, Loop-End, etc.)
 *   Bytes 2-15: 28 Nibble-Paare (4-bit Samples, low nibble zuerst)
 *
 * Pro Nibble:
 *   1. Sign-Extend von 4-bit auf 16-bit
 *   2. Left-Shift um (12 - shift)
 *   3. Addiere Filter-Prediktion: (f0*prev0 + f1*prev1) / 64
 *   4. Clamp auf [-32768, 32767]
 * ========================================================================= */

/**
 * Dekodiert einen einzelnen 16-Byte PSX-ADPCM-Block zu 28 PCM-Samples.
 *
 * @param block     Zeiger auf 16-Byte ADPCM-Block
 * @param out       Ausgabe-Puffer für 28 int16_t Samples
 * @param prev0     Zeiger auf previous sample 0 (wird aktualisiert)
 * @param prev1     Zeiger auf previous sample 1 (wird aktualisiert)
 * @return          Flags-Byte (Byte 1 des Blocks)
 */
static uint8_t adpcm_decode_block(const uint8_t* block, int16_t* out,
                                   int32_t* prev0, int32_t* prev1)
{
    int shift  = (int)(block[0] & 0x0F);
    int filter = (int)((block[0] >> 4) & 0x0F);
    uint8_t flags = block[1];
    int f0, f1;
    int i;

    /* Filter clampen auf gültigen Bereich 0-4 */
    if (filter > 4) filter = 4;

    /* Shift clampen auf 0-12 (PSX-SPU-Spezifikation) */
    if (shift > 12) shift = 9;  /* PSX-Verhalten bei shift > 12 */

    f0 = s_adpcm_filter_f0[filter];
    f1 = s_adpcm_filter_f1[filter];

    for (i = 0; i < ADPCM_SAMPLES_PER_BLOCK; i++) {
        int byte_idx = 2 + (i / 2);
        int nibble;
        int32_t sample;
        int32_t predicted;

        /* Low nibble zuerst, dann High nibble */
        if ((i & 1) == 0) {
            nibble = (int)(block[byte_idx] & 0x0F);
        } else {
            nibble = (int)((block[byte_idx] >> 4) & 0x0F);
        }

        /* Sign-Extend von 4-bit auf 32-bit */
        if (nibble >= 8) {
            nibble -= 16;
        }

        /* Shift anwenden: left-shift um (12 - shift) */
        sample = (int32_t)(nibble << (12 - shift));

        /* Filter-Prediktion addieren */
        predicted = (f0 * (*prev0) + f1 * (*prev1) + 32) / 64;
        sample += predicted;

        /* Clamp auf 16-bit Bereich */
        if (sample > 32767) sample = 32767;
        if (sample < -32768) sample = -32768;

        out[i] = (int16_t)sample;

        /* History aktualisieren */
        *prev1 = *prev0;
        *prev0 = sample;
    }

    return flags;
}

/**
 * Dekodiert einen vollständigen ADPCM-Stream (VB-Body für ein Sample) zu PCM.
 *
 * @param adpcm_data    Zeiger auf ADPCM-Rohdaten
 * @param adpcm_size    Größe in Bytes (muss Vielfaches von 16 sein)
 * @param out_sample    Ausgabe: pcm_sample_t wird befüllt
 * @return              0 bei Erfolg, -1 bei Fehler
 */
static int adpcm_decode_stream(const uint8_t* adpcm_data, int adpcm_size,
                                pcm_sample_t* out_sample)
{
    int num_blocks;
    int total_samples;
    int block_idx;
    int32_t prev0 = 0, prev1 = 0;
    int loop_start = -1;

    if (!adpcm_data || adpcm_size < ADPCM_BLOCK_SIZE || !out_sample) {
        return -1;
    }

    num_blocks = adpcm_size / ADPCM_BLOCK_SIZE;
    total_samples = num_blocks * ADPCM_SAMPLES_PER_BLOCK;

    out_sample->pcm_data = (int16_t*)malloc((size_t)total_samples * sizeof(int16_t));
    if (!out_sample->pcm_data) {
        return -1;
    }

    for (block_idx = 0; block_idx < num_blocks; block_idx++) {
        const uint8_t* block = adpcm_data + (block_idx * ADPCM_BLOCK_SIZE);
        int16_t* dest = out_sample->pcm_data + (block_idx * ADPCM_SAMPLES_PER_BLOCK);
        uint8_t flags;

        flags = adpcm_decode_block(block, dest, &prev0, &prev1);

        /* Flag-Bit 2: Loop-Start-Punkt */
        if (flags & 0x04) {
            loop_start = block_idx * ADPCM_SAMPLES_PER_BLOCK;
        }

        /* Flag-Bit 0: End of sample (+ possible loop) */
        if (flags & 0x01) {
            total_samples = (block_idx + 1) * ADPCM_SAMPLES_PER_BLOCK;
            break;
        }
    }

    out_sample->sample_count = total_samples;
    out_sample->sample_rate = PSX_BASE_SAMPLE_RATE;
    out_sample->loop_start = loop_start;
    out_sample->loop_end = (loop_start >= 0) ? total_samples : -1;

    return 0;
}

/* ============================================================================
 * VAB-Parser
 *
 * VAB-Format (VH = Header, VB = Body):
 * VH-Header (32 Bytes):
 *   Offset 0x00: uint32_t magic ("pBAV" = 0x56414270)
 *   Offset 0x04: uint32_t version
 *   Offset 0x08: uint32_t vab_id
 *   Offset 0x0C: uint32_t total_size
 *   Offset 0x10: uint16_t reserved
 *   Offset 0x12: uint16_t num_programs
 *   Offset 0x14: uint16_t num_tones
 *   Offset 0x16: uint16_t num_vags
 *   Offset 0x18: uint8_t  master_vol
 *   Offset 0x19: uint8_t  master_pan
 *   Offset 0x1A: uint8_t  bank_attr1
 *   Offset 0x1B: uint8_t  bank_attr2
 *   Offset 0x1C: uint32_t reserved2
 *
 * Nach dem 32-Byte-Header folgen:
 *   - Program-Attribute-Tabelle (16 Bytes × num_programs)
 *   - Tone-Attribute-Tabelle (32 Bytes × 16 × num_programs)
 *   - VAG-Offset-Tabelle (2 Bytes × (num_vags+1), Offsets in 8-Byte-Einheiten)
 *
 * VB-Body:
 *   Rohe ADPCM-Daten, aufgeteilt gemäß VAG-Offset-Tabelle.
 * ========================================================================= */

/** VAB-Header-Struktur (32 Bytes) */
typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t vab_id;
    uint32_t total_size;
    uint16_t reserved;
    uint16_t num_programs;
    uint16_t num_tones;
    uint16_t num_vags;
    uint8_t  master_vol;
    uint8_t  master_pan;
    uint8_t  bank_attr1;
    uint8_t  bank_attr2;
    uint32_t reserved2;
} vab_header_t;

/**
 * Parst den VH-Header und dekodiert VB-Samples in die Bank.
 *
 * @param vh        VH-Header-Daten
 * @param vh_size   Größe des VH-Puffers
 * @param vb        VB-Body-Daten (ADPCM)
 * @param vb_size   Größe des VB-Puffers
 * @param bank      Ziel-Bank (wird befüllt)
 * @return          0 bei Erfolg, -1 bei Fehler
 */
static int vab_parse_and_decode(const uint8_t* vh, int vh_size,
                                 const uint8_t* vb, int vb_size,
                                 vab_bank_t* bank)
{
    const vab_header_t* header;
    const uint16_t* vag_offsets;
    int programs_size;
    int tones_size;
    int vag_table_offset;
    int num_vags;
    int i;

    if (vh_size < (int)sizeof(vab_header_t)) {
        RE15_WARN("AUDIO", "VH-Puffer zu klein (%d Bytes, brauche %d)",
                  vh_size, (int)sizeof(vab_header_t));
        return -1;
    }

    header = (const vab_header_t*)vh;

    /* Magic prüfen */
    if (header->magic != VAB_MAGIC) {
        RE15_WARN("AUDIO", "VAB-Header ungueltig: Magic 0x%08X erwartet 0x%08X",
                  header->magic, VAB_MAGIC);
        return -1;
    }

    num_vags = (int)header->num_vags;
    if (num_vags <= 0 || num_vags > VAB_MAX_TONES) {
        RE15_WARN("AUDIO", "VAB: Ungueltiger num_vags=%d", num_vags);
        return -1;
    }

    /* Berechne Offset der VAG-Offset-Tabelle im VH */
    programs_size = (int)header->num_programs * 16;
    tones_size = (int)header->num_programs * 16 * 32;
    vag_table_offset = (int)sizeof(vab_header_t) + programs_size + tones_size;

    /* Prüfe ob VAG-Tabelle in den VH-Puffer passt */
    if (vag_table_offset + (num_vags + 1) * 2 > vh_size) {
        RE15_WARN("AUDIO", "VH-Puffer zu klein fuer VAG-Offset-Tabelle");
        return -1;
    }

    vag_offsets = (const uint16_t*)(vh + vag_table_offset);

    /* Samples dekodieren */
    bank->sample_count = 0;
    for (i = 0; i < num_vags; i++) {
        /* Offsets sind in 8-Byte-Einheiten (PSX-VAB-Konvention) */
        int start_offset = (int)vag_offsets[i] * 8;
        int end_offset   = (int)vag_offsets[i + 1] * 8;
        int sample_size;

        /* Erster Eintrag ist oft 0 (Null-Sample), überspringe */
        if (start_offset == end_offset || start_offset >= vb_size) {
            bank->samples[i].pcm_data = NULL;
            bank->samples[i].sample_count = 0;
            bank->samples[i].loop_start = -1;
            bank->samples[i].loop_end = -1;
            bank->sample_count++;
            continue;
        }

        if (end_offset > vb_size) {
            end_offset = vb_size;
        }

        sample_size = end_offset - start_offset;
        if (sample_size < ADPCM_BLOCK_SIZE) {
            bank->samples[i].pcm_data = NULL;
            bank->samples[i].sample_count = 0;
            bank->samples[i].loop_start = -1;
            bank->samples[i].loop_end = -1;
            bank->sample_count++;
            continue;
        }

        if (adpcm_decode_stream(vb + start_offset, sample_size,
                                 &bank->samples[i]) != 0) {
            bank->samples[i].pcm_data = NULL;
            bank->samples[i].sample_count = 0;
        }
        bank->sample_count++;
    }

    bank->loaded = 1;
    return 0;
}

/* ============================================================================
 * SEQ-Sequencer (vereinfachter MIDI-Playback)
 *
 * PSX-SEQ-Format:
 *   Header (15 Bytes):
 *     Offset 0x00: "pQES" (4 Bytes, Magic)
 *     Offset 0x04: uint32_t version
 *     Offset 0x08: uint16_t resolution (Ticks per quarter-note)
 *     Offset 0x0A: uint8_t  tempo[3] (24-bit, Mikrosekunden pro QN)
 *     Offset 0x0D: uint16_t rhythm  (Taktmaß)
 *   Track-Daten: MIDI-ähnliche Events (delta-time + MIDI-Befehl)
 * ========================================================================= */

#define SEQ_MAGIC           0x53455170  /* "pQES" */
#define SEQ_HEADER_SIZE     15
#define SEQ_DEFAULT_TEMPO   500000      /* 120 BPM = 500000 µs/QN */

/**
 * Liest eine Variable-Length-Quantity (VLQ) aus dem SEQ-Stream.
 * @param data      SEQ-Daten
 * @param pos       Aktuelle Position (wird vorgerückt)
 * @param max_pos   Maximale Position
 * @return          Dekodierter VLQ-Wert
 */
static int seq_read_vlq(const uint8_t* data, int* pos, int max_pos)
{
    int value = 0;
    int byte_val;

    while (*pos < max_pos) {
        byte_val = data[*pos];
        (*pos)++;
        value = (value << 7) | (byte_val & 0x7F);
        if (!(byte_val & 0x80)) {
            break;
        }
    }
    return value;
}

/**
 * Initialisiert einen SEQ-Slot für Playback.
 */
static void seq_init_slot(seq_slot_t* slot, const uint8_t* seq_data,
                           int seq_size, int loop)
{
    int tempo;

    slot->seq_data = seq_data;
    slot->seq_size = seq_size;
    slot->loop = loop;
    slot->position = SEQ_HEADER_SIZE;  /* Überspringe Header */
    slot->tick_counter = 0;

    /* Header parsen */
    if (seq_size >= SEQ_HEADER_SIZE) {
        /* Resolution (Ticks per QN) bei Offset 0x08 */
        slot->ticks_per_qn = (int)((seq_data[8] << 8) | seq_data[9]);
        if (slot->ticks_per_qn <= 0) slot->ticks_per_qn = 120;

        /* Tempo (24-bit) bei Offset 0x0A */
        tempo = (seq_data[10] << 16) | (seq_data[11] << 8) | seq_data[12];
        if (tempo <= 0) tempo = SEQ_DEFAULT_TEMPO;
        slot->tempo = tempo;
    } else {
        slot->ticks_per_qn = 120;
        slot->tempo = SEQ_DEFAULT_TEMPO;
    }

    /* Berechne Audio-Samples pro SEQ-Tick */
    /* samples_per_tick = (sample_rate * tempo_us) / (ticks_per_qn * 1000000) */
    slot->samples_per_tick = (int)((int64_t)AUDIO_SAMPLE_RATE * slot->tempo /
                              ((int64_t)slot->ticks_per_qn * 1000000));
    if (slot->samples_per_tick <= 0) slot->samples_per_tick = 1;
    slot->sample_counter = slot->samples_per_tick;

    slot->bank_id = 0;  /* Default Bank */
    slot->active = 1;
}

/**
 * Findet einen freien Mixer-Kanal.
 * @return Kanal-Index (0-15) oder -1 wenn kein Kanal frei.
 */
static int mixer_find_free_channel(void)
{
    int i;
    for (i = 0; i < MIXER_CHANNELS; i++) {
        if (!s_audio.channels[i].active) {
            return i;
        }
    }
    /* Kein freier Kanal — ältesten Kanal überschreiben (Kanal 0) */
    return 0;
}

/**
 * Startet ein Sample auf einem Mixer-Kanal.
 */
static void mixer_play_sample(int bank_id, int sample_id,
                               int volume, int pan, int looping)
{
    int ch;
    vab_bank_t* bank;

    if (bank_id < 0 || bank_id >= RE15_AUDIO_MAX_BANKS) return;
    bank = &s_audio.banks[bank_id];
    if (!bank->loaded) return;
    if (sample_id < 0 || sample_id >= bank->sample_count) return;
    if (!bank->samples[sample_id].pcm_data) return;

    ch = mixer_find_free_channel();
    if (ch < 0) return;

    s_audio.channels[ch].active    = 1;
    s_audio.channels[ch].bank_id   = bank_id;
    s_audio.channels[ch].sample_id = sample_id;
    s_audio.channels[ch].position  = 0;
    s_audio.channels[ch].volume    = volume;
    s_audio.channels[ch].pan       = pan;
    s_audio.channels[ch].looping   = looping;
}

/**
 * Verarbeitet SEQ-Events für einen Tick (ein Sequencer-Schritt).
 * Triggert Note-On-Events als Sample-Playback im Mixer.
 */
static void seq_process_tick(seq_slot_t* slot)
{
    if (!slot->active || !slot->seq_data) return;

    while (slot->position < slot->seq_size) {
        int delta;
        uint8_t status;
        int pos_save = slot->position;

        /* Delta-Time lesen */
        delta = seq_read_vlq(slot->seq_data, &slot->position, slot->seq_size);

        if (delta > 0) {
            /* Noch nicht Zeit für dieses Event — Delta als Tick-Counter setzen */
            slot->tick_counter = delta;
            slot->position = pos_save;
            return;
        }

        /* MIDI-Event verarbeiten */
        if (slot->position >= slot->seq_size) break;
        status = slot->seq_data[slot->position];

        if (status == 0xFF) {
            /* Meta-Event: überspringen */
            int meta_type, meta_len;
            slot->position++;
            if (slot->position >= slot->seq_size) break;
            meta_type = slot->seq_data[slot->position++];
            if (slot->position >= slot->seq_size) break;
            meta_len = seq_read_vlq(slot->seq_data, &slot->position, slot->seq_size);
            (void)meta_type;

            /* Tempo-Change (Meta-Event 0x51) */
            if (meta_type == 0x51 && meta_len >= 3 &&
                slot->position + 3 <= slot->seq_size) {
                slot->tempo = (slot->seq_data[slot->position] << 16) |
                              (slot->seq_data[slot->position + 1] << 8) |
                               slot->seq_data[slot->position + 2];
                slot->samples_per_tick = (int)((int64_t)AUDIO_SAMPLE_RATE *
                    slot->tempo / ((int64_t)slot->ticks_per_qn * 1000000));
                if (slot->samples_per_tick <= 0) slot->samples_per_tick = 1;
            }
            slot->position += meta_len;
        } else if ((status & 0xF0) == 0x90) {
            /* Note On */
            int note, velocity;
            slot->position++;
            if (slot->position + 2 > slot->seq_size) break;
            note = slot->seq_data[slot->position++];
            velocity = slot->seq_data[slot->position++];
            if (velocity > 0) {
                /* Trigger Sample: note als sample_id, velocity als Volume */
                int vol = (velocity > 127) ? 127 : velocity;
                mixer_play_sample(slot->bank_id, note % VAB_MAX_TONES,
                                   vol, RE15_AUDIO_PAN_CENTER, 0);
            }

        } else if ((status & 0xF0) == 0x80) {
            /* Note Off — ignorieren (Sample spielt bis Ende) */
            slot->position++;
            if (slot->position + 2 > slot->seq_size) break;
            slot->position += 2;
        } else if ((status & 0xF0) == 0xB0) {
            /* Control Change — ignorieren */
            slot->position++;
            if (slot->position + 2 > slot->seq_size) break;
            slot->position += 2;
        } else if ((status & 0xF0) == 0xC0) {
            /* Program Change — setze Bank-Referenz */
            slot->position++;
            if (slot->position + 1 > slot->seq_size) break;
            slot->position += 1;
        } else if ((status & 0xF0) == 0xE0) {
            /* Pitch Bend — ignorieren */
            slot->position++;
            if (slot->position + 2 > slot->seq_size) break;
            slot->position += 2;
        } else {
            /* Unbekanntes Event — überspringe ein Byte */
            slot->position++;
        }
    }

    /* Ende der SEQ-Daten erreicht */
    if (slot->loop) {
        /* Loop: Zurück zum Anfang (nach Header) */
        slot->position = SEQ_HEADER_SIZE;
        slot->tick_counter = 0;
    } else {
        slot->active = 0;
    }
}

/* ============================================================================
 * SDL2 Audio-Callback (wird im Audio-Thread aufgerufen)
 *
 * Mischt alle aktiven Kanäle zusammen und schreibt das Ergebnis als
 * S16 Stereo PCM in den SDL-Audio-Puffer.
 * ========================================================================= */

static void audio_callback(void* userdata, Uint8* stream, int len)
{
    int16_t* out = (int16_t*)stream;
    int num_samples = len / (int)(sizeof(int16_t) * AUDIO_CHANNELS);
    int i, ch;
    int master_vol;

    (void)userdata;

    /* Stream leeren */
    memset(stream, 0, (size_t)len);

    master_vol = s_audio.master_volume;
    if (master_vol <= 0) return;

    /* SEQ-Sequencer Ticks verarbeiten */
    for (i = 0; i < SEQ_MAX_SLOTS; i++) {
        seq_slot_t* slot = &s_audio.seq_slots[i];
        int s_idx;
        if (!slot->active) continue;

        for (s_idx = 0; s_idx < num_samples; s_idx++) {
            slot->sample_counter--;
            if (slot->sample_counter <= 0) {
                slot->sample_counter = slot->samples_per_tick;
                if (slot->tick_counter > 0) {
                    slot->tick_counter--;
                } else {
                    seq_process_tick(slot);
                }
            }
        }
    }

    /* Mixer: Alle aktiven Kanäle mischen */
    for (ch = 0; ch < MIXER_CHANNELS; ch++) {
        mixer_channel_t* channel = &s_audio.channels[ch];
        vab_bank_t* bank;
        pcm_sample_t* sample;
        int vol_l, vol_r;

        if (!channel->active) continue;

        bank = &s_audio.banks[channel->bank_id];
        if (!bank->loaded) {
            channel->active = 0;
            continue;
        }
        if (channel->sample_id >= bank->sample_count) {
            channel->active = 0;
            continue;
        }

        sample = &bank->samples[channel->sample_id];
        if (!sample->pcm_data || sample->sample_count <= 0) {
            channel->active = 0;
            continue;
        }

        /* Volume mit Pan berechnen (linear) */
        /* Pan: 0=links, 64=mitte, 127=rechts */
        {
            int vol = (channel->volume * master_vol) / 127;
            int pan = channel->pan;
            /* Links: volle Lautstärke bei pan=0, halbe bei pan=127 */
            vol_l = vol * (127 - pan) / 127;
            /* Rechts: volle Lautstärke bei pan=127, halbe bei pan=0 */
            vol_r = vol * pan / 127;
            /* Pan=64 ergibt ca. 50% auf beiden Seiten → normalisiere auf voll */
            vol_l = (vol_l * 2 > vol) ? vol : vol_l * 2;
            vol_r = (vol_r * 2 > vol) ? vol : vol_r * 2;
        }

        /* Samples in den Output-Puffer mischen */
        for (i = 0; i < num_samples; i++) {
            int32_t pcm_val;
            int32_t left, right;
            int pos = channel->position;

            if (pos >= sample->sample_count) {
                /* Sample-Ende erreicht */
                if (channel->looping && sample->loop_start >= 0) {
                    channel->position = sample->loop_start;
                    pos = channel->position;
                } else {
                    channel->active = 0;
                    break;
                }
            }

            pcm_val = (int32_t)sample->pcm_data[pos];

            /* Volume anwenden (linear, 0-127) */
            left  = (pcm_val * vol_l) / 127;
            right = (pcm_val * vol_r) / 127;

            /* Additive Mischung (Clipping wird am Ende behandelt) */
            {
                int32_t mix_l = (int32_t)out[i * 2]     + left;
                int32_t mix_r = (int32_t)out[i * 2 + 1] + right;

                /* Clamp auf 16-bit */
                if (mix_l > 32767) mix_l = 32767;
                if (mix_l < -32768) mix_l = -32768;
                if (mix_r > 32767) mix_r = 32767;
                if (mix_r < -32768) mix_r = -32768;

                out[i * 2]     = (int16_t)mix_l;
                out[i * 2 + 1] = (int16_t)mix_r;
            }

            channel->position++;
        }
    }
}

/* ============================================================================
 * Backend-Implementierung (audio_backend_t Funktionen)
 * ========================================================================= */

/**
 * Initialisiert SDL2-Audio mit 44100 Hz, S16 Stereo.
 */
static void pc_audio_init(void)
{
    SDL_AudioSpec desired, obtained;

    if (s_audio.initialized) return;

    memset(&s_audio, 0, sizeof(s_audio));
    s_audio.master_volume = RE15_AUDIO_VOLUME_MAX;

    /* SDL Audio-Subsystem initialisieren */
    if (!SDL_WasInit(SDL_INIT_AUDIO)) {
        if (SDL_Init(SDL_INIT_AUDIO) != 0) {
            RE15_ERROR("AUDIO", "SDL_Init(AUDIO) fehlgeschlagen: %s",
                       SDL_GetError());
            return;
        }
    }

    /* Audio-Device-Spezifikation */
    memset(&desired, 0, sizeof(desired));
    desired.freq     = AUDIO_SAMPLE_RATE;
    desired.format   = AUDIO_S16SYS;
    desired.channels = (Uint8)AUDIO_CHANNELS;
    desired.samples  = AUDIO_BUFFER_SAMPLES;
    desired.callback = audio_callback;
    desired.userdata = NULL;

    s_audio.device = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, 0);
    if (s_audio.device == 0) {
        RE15_ERROR("AUDIO", "SDL_OpenAudioDevice fehlgeschlagen: %s",
                   SDL_GetError());
        return;
    }

    /* Audio-Device starten (Unpause) */
    SDL_PauseAudioDevice(s_audio.device, 0);

    s_audio.initialized = 1;
    RE15_INFO("AUDIO", "PC Audio-Backend initialisiert (%d Hz, %d Kanäle, Buffer %d)",
              obtained.freq, obtained.channels, obtained.samples);
}

/**
 * Fährt das Audio-Subsystem herunter: Device schließen, Samples freigeben.
 */
static void pc_audio_shutdown(void)
{
    int b, s;

    if (!s_audio.initialized) return;

    /* Audio-Device stoppen und schließen */
    if (s_audio.device != 0) {
        SDL_PauseAudioDevice(s_audio.device, 1);
        SDL_CloseAudioDevice(s_audio.device);
        s_audio.device = 0;
    }

    /* Alle VAB-Bänke freigeben */
    for (b = 0; b < RE15_AUDIO_MAX_BANKS; b++) {
        if (s_audio.banks[b].loaded) {
            for (s = 0; s < s_audio.banks[b].sample_count; s++) {
                if (s_audio.banks[b].samples[s].pcm_data) {
                    free(s_audio.banks[b].samples[s].pcm_data);
                    s_audio.banks[b].samples[s].pcm_data = NULL;
                }
            }
            s_audio.banks[b].loaded = 0;
            s_audio.banks[b].sample_count = 0;
        }
    }

    s_audio.initialized = 0;
    RE15_INFO("AUDIO", "PC Audio-Backend heruntergefahren");
}

/**
 * Lädt eine VAB-Sound-Bank.
 * Bei Fehler: Warnung ausgeben, ohne Audio für diese Bank fortfahren.
 */
static int pc_audio_vab_load(const uint8_t* vh, int vh_size,
                              const uint8_t* vb, int vb_size, int bank_id)
{
    vab_bank_t* bank;

    if (!s_audio.initialized) return -1;

    if (bank_id < 0 || bank_id >= RE15_AUDIO_MAX_BANKS) {
        RE15_WARN("AUDIO", "vab_load: Ungueltiger bank_id=%d", bank_id);
        return -1;
    }

    if (!vh || vh_size <= 0 || !vb || vb_size <= 0) {
        RE15_WARN("AUDIO", "vab_load: Ungueltige Daten (vh=%p, vb=%p)",
                  (const void*)vh, (const void*)vb);
        return -1;
    }

    bank = &s_audio.banks[bank_id];

    /* Falls bereits geladen → erst entladen */
    if (bank->loaded) {
        int s;
        SDL_LockAudioDevice(s_audio.device);
        for (s = 0; s < bank->sample_count; s++) {
            if (bank->samples[s].pcm_data) {
                free(bank->samples[s].pcm_data);
                bank->samples[s].pcm_data = NULL;
            }
        }
        bank->loaded = 0;
        bank->sample_count = 0;
        SDL_UnlockAudioDevice(s_audio.device);
    }

    /* VAB parsen und dekodieren */
    if (vab_parse_and_decode(vh, vh_size, vb, vb_size, bank) != 0) {
        RE15_WARN("AUDIO", "VAB-Bank %d konnte nicht geladen werden — "
                  "ohne Audio fuer diese Bank fortfahren", bank_id);
        return -1;
    }

    RE15_INFO("AUDIO", "VAB-Bank %d geladen (%d Samples)",
              bank_id, bank->sample_count);
    return 0;
}

/**
 * Entlädt eine VAB-Sound-Bank und gibt Ressourcen frei.
 */
static void pc_audio_vab_unload(int bank_id)
{
    vab_bank_t* bank;
    int s;

    if (!s_audio.initialized) return;
    if (bank_id < 0 || bank_id >= RE15_AUDIO_MAX_BANKS) return;

    bank = &s_audio.banks[bank_id];
    if (!bank->loaded) return;

    SDL_LockAudioDevice(s_audio.device);

    /* Alle Kanäle stoppen die diese Bank nutzen */
    for (s = 0; s < MIXER_CHANNELS; s++) {
        if (s_audio.channels[s].active && s_audio.channels[s].bank_id == bank_id) {
            s_audio.channels[s].active = 0;
        }
    }

    /* Samples freigeben */
    for (s = 0; s < bank->sample_count; s++) {
        if (bank->samples[s].pcm_data) {
            free(bank->samples[s].pcm_data);
            bank->samples[s].pcm_data = NULL;
        }
    }
    bank->loaded = 0;
    bank->sample_count = 0;

    SDL_UnlockAudioDevice(s_audio.device);

    RE15_INFO("AUDIO", "VAB-Bank %d entladen", bank_id);
}

/**
 * Startet SEQ-Playback (BGM) in einem Slot.
 * Loop bis explizit gestoppt oder durch anderen Track ersetzt.
 */
static void pc_audio_seq_play(int slot, const uint8_t* seq_data,
                               int seq_size, int loop)
{
    seq_slot_t* seq;

    if (!s_audio.initialized) return;
    if (slot < 0 || slot >= SEQ_MAX_SLOTS) return;
    if (!seq_data || seq_size < SEQ_HEADER_SIZE) return;

    seq = &s_audio.seq_slots[slot];

    SDL_LockAudioDevice(s_audio.device);

    /* Vorherigen Playback im Slot stoppen */
    seq->active = 0;

    /* Neuen SEQ initialisieren */
    seq_init_slot(seq, seq_data, seq_size, loop);

    SDL_UnlockAudioDevice(s_audio.device);

    RE15_INFO("AUDIO", "SEQ gestartet: Slot %d, %d Bytes, Loop=%d",
              slot, seq_size, loop);
}

/**
 * Stoppt SEQ-Playback in einem Slot.
 */
static void pc_audio_seq_stop(int slot)
{
    if (!s_audio.initialized) return;
    if (slot < 0 || slot >= SEQ_MAX_SLOTS) return;

    SDL_LockAudioDevice(s_audio.device);
    s_audio.seq_slots[slot].active = 0;
    SDL_UnlockAudioDevice(s_audio.device);

    RE15_INFO("AUDIO", "SEQ gestoppt: Slot %d", slot);
}

/**
 * Triggert einen Soundeffekt (Se_on Opcode).
 *
 * @param bank      VAB-Bank (0-15)
 * @param sample_id Sample-Index (0-127)
 * @param volume    Lautstärke (0-127, linear)
 * @param pan       Stereo-Position (0=links, 64=Mitte, 127=rechts)
 */
static void pc_audio_sfx_play(int bank, int sample_id, int volume, int pan)
{
    if (!s_audio.initialized) return;
    if (bank < 0 || bank >= RE15_AUDIO_MAX_BANKS) return;
    if (sample_id < 0 || sample_id >= RE15_AUDIO_MAX_SAMPLES) return;

    /* Volume clampen */
    if (volume < 0) volume = 0;
    if (volume > RE15_AUDIO_VOLUME_MAX) volume = RE15_AUDIO_VOLUME_MAX;

    /* Pan clampen */
    if (pan < 0) pan = 0;
    if (pan > 127) pan = 127;

    SDL_LockAudioDevice(s_audio.device);
    mixer_play_sample(bank, sample_id, volume, pan, 0);
    SDL_UnlockAudioDevice(s_audio.device);
}

/**
 * Spielt eine Voice-Nachricht (Sprach-Clip).
 * Voice-Playback nutzt eine dedizierte VAB-Bank (Bank 15 per Konvention).
 */
static void pc_audio_voice_play(int room_id, int msg_id)
{
    if (!s_audio.initialized) return;

    /* Voice nutzt Bank 15 (letzte Bank) als Voice-Bank
     * sample_id = msg_id, Volume = max, Pan = Mitte */
    SDL_LockAudioDevice(s_audio.device);
    mixer_play_sample(RE15_AUDIO_MAX_BANKS - 1, msg_id,
                       RE15_AUDIO_VOLUME_MAX, RE15_AUDIO_PAN_CENTER, 0);
    SDL_UnlockAudioDevice(s_audio.device);

    (void)room_id;  /* room_id bestimmt welche Voice-Datei geladen wird */
}

/**
 * Setzt die Lautstärke für einen Audio-Kanal (oder Master).
 *
 * @param channel       Kanal-Nummer (0-15) oder -1 für Master
 * @param volume_0_127  Lautstärke (0=stumm, 127=Maximum)
 */
static void pc_audio_set_volume(int channel, int volume_0_127)
{
    if (!s_audio.initialized) return;

    /* Clamp */
    if (volume_0_127 < 0) volume_0_127 = 0;
    if (volume_0_127 > RE15_AUDIO_VOLUME_MAX) volume_0_127 = RE15_AUDIO_VOLUME_MAX;

    SDL_LockAudioDevice(s_audio.device);

    if (channel < 0) {
        /* Master-Volume */
        s_audio.master_volume = volume_0_127;
    } else if (channel < MIXER_CHANNELS) {
        s_audio.channels[channel].volume = volume_0_127;
    }

    SDL_UnlockAudioDevice(s_audio.device);
}

/* ============================================================================
 * Exportierte Backend-Instanz
 * ========================================================================= */

const audio_backend_t audio_pc_backend = {
    .init       = pc_audio_init,
    .shutdown   = pc_audio_shutdown,
    .vab_load   = pc_audio_vab_load,
    .vab_unload = pc_audio_vab_unload,
    .seq_play   = pc_audio_seq_play,
    .seq_stop   = pc_audio_seq_stop,
    .sfx_play   = pc_audio_sfx_play,
    .voice_play = pc_audio_voice_play,
    .set_volume = pc_audio_set_volume,
};
