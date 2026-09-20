/* ============================================================================================
 * re15_rumble.c — byte-true Zwilling der zwei RE2-Rumble-Ringe.
 * Die vollstaendige Belegkette steht im Kopf von include/re15_rumble.h.
 * ========================================================================================== */
#include "re15_rumble.h"

#include <string.h>

/* 0x20 = 32 Slots je Ring (`sltiu v0,a2,0x20` @0x8003979c im Tick, @0x800397d0 im Alloc). */
#define RE15_RUMBLE_SLOTS 0x20

/* Das 10-Byte-Slot-Layout des Originals (STRIDE 10, `addiu a1,a1,10` @0x80039798). Der Port
 * fuehrt es als Struct — die FELD-BREITEN sind byte-true nachgebildet (u8/u16/s16), damit die
 * Ueberlaeufe des Originals dieselben bleiben. */
typedef struct {
    uint8_t  belegt;    /* +0 */
    uint8_t  wert;      /* +1 */
    uint16_t delay;     /* +2 */
    uint16_t dauer;     /* +4 — im Original per `lhu`/`sh` gefuehrt (@0x80039734/@0x80039740) */
    int16_t  schritt;   /* +6 — per `lh` gelesen (@0x80039754), also VORZEICHENBEHAFTET */
    uint16_t akku;      /* +8 */
} re15_rumble_slot_t;

/* Ring 0 = @0x800EAAD8 (Aktuator 0), Ring 1 = @0x800EAC28 (Aktuator 1). */
static re15_rumble_slot_t s_ring[2][RE15_RUMBLE_SLOTS];

/* `FUN_800397E4` @0x800397e4 — Fuellstand (zaehlt `slot[0] != 0`). */
static int rumble_belegt(int ring)
{
    int i, n = 0;
    for (i = 0; i < RE15_RUMBLE_SLOTS; i++)
        if (s_ring[ring][i].belegt) n++;
    return n;
}

int re15_rumble_belegt(int ring)
{
    if (ring < 0 || ring > 1) return 0;
    return rumble_belegt(ring);
}

/* `FUN_800397B8` @0x800397b8 — ersten freien Slot suchen. Laeuft der Ring voll, liefert das
 * Original den Zeiger HINTER den Ring; der Aufrufer prueft aber vorher den Fuellstand
 * (`sltiu v0,v0,0x20` @0x80039498 / @0x80039530 / @0x800395d4), also kommt es nie dazu. */
static re15_rumble_slot_t *rumble_alloc(int ring)
{
    int i;
    for (i = 0; i < RE15_RUMBLE_SLOTS; i++)
        if (!s_ring[ring][i].belegt) return &s_ring[ring][i];
    return 0;
}

void re15_rumble_reset(void)
{
    memset(s_ring, 0, sizeof(s_ring));
}

/* `FUN_8003947C` @0x8003947c */
void re15_rumble_small(int dauer, int delay)
{
    re15_rumble_slot_t *s;
    /* `sltiu v0,v0,0x20` @0x80039498 (Ring nicht voll) UND `beq a1,zero` @0x800394a0
     * (dauer != 0) — sonst still verwerfen. */
    if (rumble_belegt(0) >= RE15_RUMBLE_SLOTS || dauer == 0) return;
    s = rumble_alloc(0);
    if (!s) return;
    s->belegt  = 1;                                  /* `sb v0,0(v1)`  @0x800394c0 */
    s->wert    = (uint8_t)(dauer != 0);              /* `sb v0,1(v1)`  @0x800394cc -> immer 1 */
    s->dauer   = (uint16_t)(int16_t)dauer;           /* `sh a1,4(v0)`  @0x800394d4 */
    s->delay   = (uint16_t)delay;                    /* `sh a2,2(v0)`  @0x800394dc */
    s->schritt = 0;                                  /* `sh zero,6(v0)`@0x800394e4 */
    s->akku    = 0;
}

/* `FUN_80039514` @0x80039514 */
void re15_rumble_large(int dauer, int amp, int delay)
{
    re15_rumble_slot_t *s;
    if (rumble_belegt(1) >= RE15_RUMBLE_SLOTS || dauer == 0) return;
    s = rumble_alloc(1);
    if (!s) return;
    s->belegt  = 1;
    s->wert    = (uint8_t)amp;                       /* Slot[1] = die Amplitude, Schritt 0 */
    s->dauer   = (uint16_t)(int16_t)dauer;
    s->delay   = (uint16_t)delay;
    s->schritt = 0;
    s->akku    = 0;
}

/* `FUN_800395B8` @0x800395b8 */
void re15_rumble_ramp(int dauer, int von, int bis, int delay)
{
    re15_rumble_slot_t *s;
    int spanne;
    if (rumble_belegt(1) >= RE15_RUMBLE_SLOTS || dauer == 0) return;
    s = rumble_alloc(1);
    if (!s) return;
    spanne     = (bis - von) * 0x80;                 /* `(param_3 - param_2) * 0x80` */
    s->belegt  = 1;
    s->dauer   = (uint16_t)(int16_t)dauer;
    s->delay   = (uint16_t)delay;
    s->akku    = (uint16_t)(int16_t)(von << 7);      /* Slot[8..9] = von << 7 */
    s->schritt = (int16_t)(spanne / dauer);          /* Slot[6..7] = Spanne / dauer */
    /* Slot[1] setzt das Original hier NICHT — der Tick schreibt ihn beim ersten Bild, an dem
     * der Vorlauf abgelaufen ist (Schritt != 0 -> Zweig @0x8003975c). */
}

/* `FUN_800396FC` @0x800396fc — ein Bild, Rueckgabe = Maximum ueber die Slots. */
static uint8_t rumble_tick_ring(int ring)
{
    uint8_t max = 0;                                 /* `addu a3,zero,zero` @0x80039700 */
    int i;
    for (i = 0; i < RE15_RUMBLE_SLOTS; i++) {
        re15_rumble_slot_t *s = &s_ring[ring][i];
        if (!s->belegt) continue;                    /* @0x80039714 */
        if (s->delay != 0) {                         /* @0x8003971c-24 */
            s->delay = (uint16_t)(s->delay - 1);     /* @0x80039728/@0x80039730 */
            continue;                                /* `j 0x80039794` @0x8003972c — der
                                                      * verzoegerte Slot geht NICHT ins Maximum */
        }
        s->dauer = (uint16_t)(s->dauer - 1);         /* @0x8003973c/@0x80039740 */
        if (s->dauer == 0) s->belegt = 0;            /* @0x80039744-50 — der Slot liefert in
                                                      * DIESEM Bild aber noch seinen Wert */
        if (s->schritt != 0) {                       /* @0x8003975c */
            /* `addu v0,v0,v1` @0x8003976c rechnet in 32 Bit (v0 = lhu Akku, v1 = lh Schritt),
             * `srl v1,v0,7` @0x80039770 schiebt das 32-Bit-Ergebnis LOGISCH, erst der `sh`
             * @0x80039774 schneidet den Akku auf 16 Bit. Genau so nachgebaut, damit ein
             * Unterlauf dieselbe Zahl liefert wie im Original. */
            uint32_t sum = (uint32_t)((int32_t)(uint32_t)s->akku + (int32_t)s->schritt);
            s->akku = (uint16_t)sum;                 /* @0x80039774 */
            s->wert = (uint8_t)(sum >> 7);           /* @0x80039770/@0x80039778 */
        }
        if (s->wert > max) max = s->wert;            /* @0x8003977c-90 */
    }
    return max;
}

void re15_rumble_tick(uint8_t *aktuator0_out, uint8_t *aktuator1_out)
{
    uint8_t a0 = rumble_tick_ring(0);                /* @0x80038da0-a4 -> DAT_800CBC20 */
    uint8_t a1 = rumble_tick_ring(1);                /* @0x80038db0-b4 -> DAT_800CBC21 */
    if (aktuator0_out) *aktuator0_out = a0;          /* `sb v0,0(s0)` @0x80038db8 */
    if (aktuator1_out) *aktuator1_out = a1;          /* `sb v0,1(s0)` @0x80038dc0 */
}
