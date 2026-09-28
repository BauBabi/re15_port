/* title_pulse.c — Titelmenue: Helligkeitspuls der aktiven Zeile + Titel-Tick.
 *
 * Runde 30 / Thema D. Belege und Herleitung: include/re15_title_pulse.h und
 * analysis/befunde_runde30/titel-blinken.md. Kein SDL, kein Dateizugriff — reiner Zustand.
 */
#include "re15_title_pulse.h"

/* Die beiden Datenworte hinter FUN_801028ec: Pulswert @0x80102944, Zaehler @0x80102946
 * (TITLE.BIN Datei 0x2944 = 0x0080, Datei 0x2946 = 0x0000). Im Original 16 Bit breit
 * (`lhu` @0x801028f0/f8, `sh` @0x80102930/38). */
static uint16_t s_pulse_val = RE15_TITLE_PULSE_START;
static uint16_t s_pulse_ctr = 0;

void re15_title_pulse_reset(void)
{
    s_pulse_ctr = 0;                          /* Datei 0x2946 */
    s_pulse_val = RE15_TITLE_PULSE_START;     /* Datei 0x2944 */
}

/* FUN_801028ec, Befehl fuer Befehl:
 *   801028f0  lhu   v1,0x2946(v1)      ; v1 = Zaehler
 *   801028f8  lhu   a0,0x2944(a0)      ; a0 = Pulswert
 *   801028fc  sltiu v0,v1,0x1f         ; Zaehler < 31 ?   (VOR dem Inkrement)
 *   80102900  beq   v0,zero,0x80102910
 *   8010290c  addiu a0,a0,2            ;   ja:   Pulswert += 2
 *   80102910  addiu a0,a0,-2           ;   nein: Pulswert -= 2
 *   80102914  addiu v1,v1,1            ; Zaehler += 1
 *   80102918  ori   v0,zero,0x3c
 *   8010291c  bne   v1,v0,0x8010292c   ; Zaehler == 60 ?
 *   80102924  ori   v1,zero,0          ;   Zaehler  := 0
 *   80102928  ori   a0,zero,0x80       ;   Pulswert := 0x80
 *   80102930  sh    v1,0x2946(at)
 *   80102938  sh    a0,0x2944(at)
 * Die Welle ist KEIN symmetrisches Dreieck: 31 Schritte hoch (0x82 ... 0xBE), 28 runter
 * (0xBC ... 0x86), dann der Sprung auf 0x80. */
void re15_title_pulse_step(void)
{
    uint32_t ctr = s_pulse_ctr;
    uint32_t val = s_pulse_val;
    if (ctr < RE15_TITLE_PULSE_UP_BELOW) val += RE15_TITLE_PULSE_DELTA;     /* @0x801028fc / @0x8010290c */
    else                                 val -= RE15_TITLE_PULSE_DELTA;     /* @0x80102910 */
    ctr += 1;                                                               /* @0x80102914 */
    if (ctr == RE15_TITLE_PULSE_PERIOD) {                                   /* @0x80102918-1c: bne = GLEICHHEIT */
        ctr = 0;                                                            /* @0x80102924 */
        val = RE15_TITLE_PULSE_START;                                       /* @0x80102928 */
    }
    s_pulse_ctr = (uint16_t) ctr;                                           /* sh @0x80102930 */
    s_pulse_val = (uint16_t) val;                                           /* sh @0x80102938 */
}

void re15_title_pulse_advance(uint64_t n)
{
    /* 0x3c Aufrufe fuehren jeden Zustand der Folge auf sich selbst zurueck (@0x80102918). */
    unsigned k = (unsigned) (n % (uint64_t) RE15_TITLE_PULSE_PERIOD);
    while (k--) re15_title_pulse_step();
}

int re15_title_pulse_value(void)   { return (int) s_pulse_val; }
int re15_title_pulse_counter(void) { return (int) s_pulse_ctr; }

/* ---- Titel-Tick ------------------------------------------------------------------------- */

uint64_t re15_title_tick_count(uint64_t elapsed_us)
{
    /* Durchgaenge = Zeit * VBlank-Rate / VBlanks je Durchgang
     *             = elapsed_us * 59826 mHz / (2 * 1000 mHz/Hz * 1000000 us/s).
     * 2 @0x8002130c-14 (DAT_800b5456), 59826 = psx-spx NTSC non-interlaced.
     * 64 Bit reichen fuer elapsed_us < 2^64 / 59826 (knapp 10 Jahre). */
    const uint64_t den = (uint64_t) RE15_TITLE_VBLANKS_PER_TICK * 1000ull * 1000000ull;
    return (elapsed_us * (uint64_t) RE15_TITLE_VBLANK_MILLIHZ) / den;
}

void re15_title_clock_start(re15_title_clock_t *c, uint64_t now_us)
{
    c->start_us = now_us;
    c->ticks    = 0;
}

uint64_t re15_title_clock_poll(re15_title_clock_t *c, uint64_t now_us)
{
    /* Die Durchgaenge werden stets aus der GESAMTEN Zeit seit dem Nullpunkt gerechnet, nicht
     * aus aufaddierten Bild-Abstaenden — so sammelt sich kein Rundungsfehler an. */
    uint64_t total = (now_us > c->start_us) ? re15_title_tick_count(now_us - c->start_us) : 0;
    if (total < c->ticks) return 0;            /* Zeitquelle lief rueckwaerts: nichts faellig */
    uint64_t due = total - c->ticks;
    c->ticks = total;
    return due;
}

/* ---- Titel-Einblende -------------------------------------------------------------------- */

int re15_title_fadein_level(uint64_t tick)
{
    /* Pegel nach `tick` Integrationen: 0x7fff (@0x80021718) + tick * (-0x400) (@0x80102058).
     * Nach 32 Schritten ist er -1 -> Vorzeichenbit -> der Kanal zeichnet nicht mehr
     * (`bltz` @0x800218cc). */
    if (tick > (uint64_t) (0x7fff / 0x400)) return 0;   /* ab tick 32: 0x7fff - 32*0x400 = -1 */
    int32_t level = 0x7fff - (int32_t) tick * 0x400;
    return (int) (level >> 7);                       /* `sra a0,v0,23` nach `sll v0,v0,16` @0x800218c8-d0 */
}
