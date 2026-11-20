/* re15_spu_adsr.h - Strang "ton", Stufe 7 (2026-11-20): Huellkurve (ADSR) des PSX-Tonchips (SPU) fuer die
 * GERAEUSCH-Stimmen des Port-Mischers (audio_pc.c audio_callback). Die Musik-Stimmen haben ihre eigene, aeltere
 * Huellkurve (ss_env_update) - unveraendert, siehe Bericht s7_r1 "offen".
 *
 * Nutzer 19.11.2026: "Bei der PC Version klingen die Sounds irgendwie dumpfer/anders als in der PSX Version."
 * Messbericht U7 (scratchpad ton_dumpf/BERICHT.md): der Port spielte Geraeusche ohne Huellkurve (immer voll). Zensus
 * aller Geraeusch-Schichten (skripte/zensus_adsr.py, 5525 Schichten): 87,9 % steigen sofort an und halten konstant -
 * fuer sie ist "keine Huellkurve" gleichwertig; der Rest hat einen weichen Einsatz (0xaaff 342x ~130 ms, 0xb4ff 174x
 * ~743 ms, 0xa8ff, 0xa0ff, ...) oder faellt nach dem Anschlag ab (0x80c6/0x0a0c 80x: Abfall auf 0x3800, dann wieder auf).
 *
 * Herkunft der Register: libsnd SpuVmDoAllocate schreibt sie bei JEDEM Key-On aus dem Ton (VagAtr):
 *   RE1.5 PSX.EXE @0x8005637c `lhu v0,0x10(v0)` -> @0x8005638c `sh v0,-0x94c(at)` (DAT_8008f6b4 = ADSR1),
 *         @0x800563b4 `lhu v0,0x12(v0)` + @0x800563bc `lhu a0,DAT_800b2540` -> @0x800563c4 `addu` -> @0x800563d0
 *         `sh v0,-0x94a(at)` (DAT_8008f6b6 = ADSR2 + Daempfer); Schmutz-Bits `ori v0,v0,0x30` @0x800563ec.
 *   RE2 (die RE2-Baenke des Ports) gleich: @0x80081e70 `lhu v0,0x10(v0)` -> @0x80081e80 (DAT_800dccb8),
 *         @0x80081ea8 `lhu v0,0x12(v0)` + @0x80081eb0 `lhu a0,DAT_800d52b0` -> @0x80081ec4 (DAT_800dccba).
 *   Daempfer DAT_800b2540: 0 aus SpuVmInit (@0x800577fc `sh zero`) und SpuVmDamperOff (@0x8005fb04); nur
 *   SpuVmDamperOn (@0x8005f8a4, 2) setzt ihn - gerufen allein aus der Sequenz-Steuerung (MIDI-Regler 64, @0x8005dd0c).
 *   Der Port fuehrt den Daempfer nicht (die Geraeusch-Stimmen bekommen ADSR2 aus dem Ton).
 *
 * Hardware-Beschreibung: info/Resident_Evil_und_Playstation_Information/psx-spx.github.io-master/docs/soundprocessingunitspu.md
 *   Register-Aufbau Z. 330-351 (Attack Mode/Shift/Step, Decay Shift, Sustain Level (N+1)*800h, Sustain Mode/Direction/
 *   Shift/Step, Release Mode/Shift), Key ON Z. 495 ("initializes ADSR Volume to zero"),
 *   "Envelope Operation depending on Shift/Step/Mode/Direction" Z. 398-430:
 *     AdsrStep = 7 - StepValue; abnehmend: NOT AdsrStep (-8..-5); AdsrStep SHL Max(0,11-Shift);
 *     CounterIncrement = 8000h SHR Max(0,Shift-11);
 *     exponentiell + zunehmend + Pegel > 6000h: Shift < 10 -> Schritt / 4; Shift >= 11 -> Zaehler / 4; sonst (Shift 10)
 *       BEIDES geteilt;
 *     exponentiell + abnehmend: AdsrStep = AdsrStep * AdsrLevel / 8000h;
 *     Rate != alle Bits -> CounterIncrement mindestens 1; Counter += CounterIncrement, Schritt nur bei Counter.Bit15;
 *     Pegel: zunehmend hoechstens 7FFFh, abnehmend mindestens 0.
 *
 * GEMESSEN am Original (DuckStation, Strang ton Stufe 7, scratchpad ton/s7/adsr_mess.py, adsr_patch.sav): der
 * Rolltor-Ton (ROOM1130 snd0 Ton 7, VagAtr im RAM @0x80123784) mit ADSR1 0xaaff statt 0x80ff gegen die Aufnahme mit
 * 0x80ff (dieselben Proben, Korrelation 1,000000): Anstieg 10,00 Pegelstufen je Probe bis 0x6000, darueber 2,50 je
 * Probe -> im Fall Shift 10 sind Schritt UND Zaehler zusammen durch 4 geteilt, also JE durch 2 (psx-spx Z. 412-413
 * schreibt je "/= 4"; woertlich genommen waeren es 0,5 je Probe und 427 ms statt 130 ms bis voll: mittlere Abweichung
 * 3600 Pegelstufen gegen 5,9 mit "je / 2"). Der Stopp-Ton (Ton 8) mit 0x80c6/0x0a0c: Abfall exponentiell bis zur
 * Halte-Stufe 0x3800, dann linear zurueck auf 0x7fff - dieses Modell trifft die Aufnahme auf 7,4 Pegelstufen im Mittel
 * (die Halte-Phase beginnt OHNE Abschneiden auf die Halte-Stufe).
 *
 * Reihenfolge je Ausgabeprobe: Probe mit dem aktuellen Pegel, danach ein Huellkurven-Schritt (Key-On: Pegel 0). Ob die
 * Hardware den Schritt vor oder nach der ersten Probe macht, ist nicht gemessen (hoechstens 1 Probe = 23 us; offen).
 */
#ifndef RE15_SPU_ADSR_H
#define RE15_SPU_ADSR_H

#include <stdint.h>

enum { RE15_ADSR_ANSTIEG = 0, RE15_ADSR_ABFALL = 1, RE15_ADSR_HALTEN = 2, RE15_ADSR_AUSKLANG = 3 };

typedef struct {
    int32_t  pegel;      /* aktueller ADSR-Pegel 0..0x7fff (psx-spx 1F801C0Ch "Current ADSR Volume") */
    uint32_t zaehler;    /* Counter (Schritt bei Bit 15) */
    uint16_t adsr1, adsr2;
    uint8_t  phase;
} re15_spu_adsr_t;

/* Key-On (psx-spx Z. 495): Pegel 0, Anstieg. */
static inline void re15_spu_adsr_start(re15_spu_adsr_t *h, uint16_t adsr1, uint16_t adsr2)
{
    h->pegel = 0; h->zaehler = 0; h->adsr1 = adsr1; h->adsr2 = adsr2; h->phase = RE15_ADSR_ANSTIEG;
}

/* Key-Off (psx-spx Z. 498-505): sofort in den Ausklang. */
static inline void re15_spu_adsr_aus(re15_spu_adsr_t *h)
{
    h->phase = RE15_ADSR_AUSKLANG; h->zaehler = 0;
}

/* Gibt den Pegel fuer DIESE Ausgabeprobe zurueck und rueckt danach einen 44,1-kHz-Takt weiter. */
static inline int32_t re15_spu_adsr_takt(re15_spu_adsr_t *h)
{
    const int32_t jetzt = h->pegel;
    int shift, stepval, expo, ab, alle_bits;
    switch (h->phase) {
    case RE15_ADSR_ANSTIEG:   /* Z. 333-336: Mode Bit15, Shift Bit14-10, Step Bit9-8, immer zunehmend */
        shift = (h->adsr1 >> 10) & 0x1f; stepval = (h->adsr1 >> 8) & 3; expo = (h->adsr1 >> 15) & 1; ab = 0;
        alle_bits = ((stepval | (shift << 2)) == 0x7f); break;
    case RE15_ADSR_ABFALL:    /* Z. 337-340: immer exponentiell, abnehmend, Step fest -8, Shift Bit7-4 */
        shift = (h->adsr1 >> 4) & 0xf; stepval = 0; expo = 1; ab = 1; alle_bits = 0; break;
    case RE15_ADSR_HALTEN:    /* Z. 342-347: Mode Bit31, Richtung Bit30, Shift Bit28-24, Step Bit23-22 */
        shift = (h->adsr2 >> 8) & 0x1f; stepval = (h->adsr2 >> 6) & 3; expo = (h->adsr2 >> 15) & 1;
        ab = (h->adsr2 >> 14) & 1; alle_bits = ((stepval | (shift << 2)) == 0x7f); break;
    default:                  /* Z. 348-351: Mode Bit21, Shift Bit20-16, abnehmend, Step fest -8 */
        shift = h->adsr2 & 0x1f; stepval = 0; expo = (h->adsr2 >> 5) & 1; ab = 1; alle_bits = (shift == 0x1f); break;
    }
    /* Z. 401-405 */
    int32_t schritt = 7 - stepval;
    if (ab) schritt = ~schritt;                                   /* +7..+4 -> -8..-5 */
    schritt = schritt * (1 << (shift < 11 ? 11 - shift : 0));     /* SHL (auch fuer negative Werte) */
    uint32_t inc = 0x8000u >> (shift > 11 ? shift - 11 : 0);         /* Shift <= 31: hoechstens >> 20 */
    if (expo && !ab && h->pegel > 0x6000) {                       /* Z. 406-413 */
        if (shift < 10)       schritt >>= 2;
        else if (shift >= 11) inc >>= 2;
        else { schritt >>= 1; inc >>= 1; }                        /* Shift 10: GEMESSEN je / 2 (Kopf) */
    } else if (expo && ab) {                                      /* Z. 414-415: AdsrStep*AdsrLevel/8000h */
        /* negativ * Pegel, arithmetisch geschoben = Abrunden gegen minus unendlich (wie ss_env_update, Stufe 4 Runde 2) */
        schritt = -(int32_t)(((int64_t)(-schritt) * h->pegel + 0x7fff) >> 15);
    }
    if (!alle_bits && inc < 1u) inc = 1u;                         /* Z. 417-418 */
    h->zaehler += inc;                                            /* Z. 421 */
    if (h->zaehler & 0x8000u) {                                   /* sonst kein Schritt in diesem Takt */
        h->zaehler = 0;
        h->pegel += schritt;                                      /* Z. 425-430 */
        if (!ab) { if (h->pegel > 0x7fff) h->pegel = 0x7fff; }
        else     { if (h->pegel < 0) h->pegel = 0; }
    }
    /* Phasenwechsel: Anstieg "until Level 7FFFh" (Z. 334), Abfall "until Sustain Level" (Z. 338, 341) - in JEDEM Takt
     * geprueft, auch ohne Schritt: der Rolltor-Ton 0x80ff/0x1fc0 (Halte-Stufe 0xf = 0x8000, ueber dem Hoechstwert) steht
     * im laufenden Original auf ENVX 32767 (Messbericht Abschnitt 5, Savestate ton_dumpf/orig/b_rolltor1130), also ohne
     * einen Abfall-Schritt auf 32759. */
    if (h->phase == RE15_ADSR_ANSTIEG && h->pegel >= 0x7fff) h->phase = RE15_ADSR_ABFALL;
    else if (h->phase == RE15_ADSR_ABFALL && h->pegel <= (((h->adsr1 & 0xf) + 1) << 11)) h->phase = RE15_ADSR_HALTEN;
    return jetzt;
}

#endif /* RE15_SPU_ADSR_H */
