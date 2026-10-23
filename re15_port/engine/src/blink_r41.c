/* blink_r41.c — Runde 41 Spur D: Helligkeitspuls aufnehmbarer Port-Gegenstaende. Belege und Herleitung:
 * include/re15_blink_r41.h, analysis/befunde_runde41/D_kleinkram_a.md §2. */
#include "re15_blink_r41.h"

#include <string.h>

typedef struct { uint16_t raum; int16_t obj; uint8_t an; uint8_t belegt; } blink_eintrag_t;
static blink_eintrag_t s_eintraege[RE15_BLINK_R41_EINTRAEGE];

/* FUN_801028ec, Schritt fuer Schritt nachgefahren (keine geschlossene Formel, damit die Folge bytegleich zum
 * Titelpuls bleibt): Zaehler < 0x1f -> +2, sonst -2; Zaehler == 0x3c -> Reset. */
int re15_blink_r41_wert(unsigned takt)
{
    unsigned n = takt % RE15_BLINK_R41_PERIODE;
    unsigned ctr = 0, val = RE15_BLINK_R41_START;
    while (n--) {
        if (ctr < RE15_BLINK_R41_UP_BELOW) val += RE15_BLINK_R41_DELTA;   /* @0x801028fc / @0x8010290c */
        else                               val -= RE15_BLINK_R41_DELTA;   /* @0x80102910 */
        ctr += 1;                                                          /* @0x80102914 */
        if (ctr == RE15_BLINK_R41_PERIODE) { ctr = 0; val = RE15_BLINK_R41_START; }   /* @0x80102918-28 */
    }
    return (int)val;
}

static blink_eintrag_t *finden(uint16_t raum, int obj_id)
{
    for (int i = 0; i < RE15_BLINK_R41_EINTRAEGE; i++)
        if (s_eintraege[i].belegt && s_eintraege[i].raum == raum && s_eintraege[i].obj == (int16_t)obj_id)
            return &s_eintraege[i];
    return NULL;
}

int re15_blink_r41_setzen(uint16_t raum, int obj_id, int an)
{
    blink_eintrag_t *e = finden(raum, obj_id);
    if (!e) {
        for (int i = 0; i < RE15_BLINK_R41_EINTRAEGE && !e; i++)
            if (!s_eintraege[i].belegt) e = &s_eintraege[i];
        if (!e) return -1;
        e->belegt = 1; e->raum = raum; e->obj = (int16_t)obj_id;
    }
    e->an = an ? 1u : 0u;
    return 0;
}

int re15_blink_r41_aktiv(uint16_t raum, int obj_id)
{
    const blink_eintrag_t *e = finden(raum, obj_id);
    return (e && e->an) ? 1 : 0;
}

void re15_blink_r41_reset(void) { memset(s_eintraege, 0, sizeof s_eintraege); }

int re15_blink_r41_faktor_q8(uint16_t raum, int obj_id, unsigned takt)
{
    if (!re15_blink_r41_aktiv(raum, obj_id)) return 256;
    return re15_blink_r41_wert(takt) * 256 / RE15_BLINK_R41_START;      /* 0x80 -> 256, 0xBE -> 380 */
}

int re15_blink_r41_lichtctx(uint16_t raum, int obj_id, unsigned takt, re15_actor_lightctx_t *ctx)
{
    if (!ctx || !re15_blink_r41_aktiv(raum, obj_id)) return RE15_BLINK_R41_START;
    const int w = re15_blink_r41_wert(takt);
    for (int c = 0; c < 3; c++) {
        int a = (int)ctx->ambient[c] * w / RE15_BLINK_R41_START;
        ctx->ambient[c] = (uint8_t)(a > 255 ? 255 : a);
        for (int l = 0; l < 3; l++) ctx->C[l][c] = ctx->C[l][c] * w / RE15_BLINK_R41_START;
    }
    return w;
}
