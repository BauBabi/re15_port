/*
 * re2_fx.c — Runde 34 VERTRAG V3 (C0): die RE2-FX-MASCHINE, hier NUR STUBS.
 *
 * Belege und Semantik je Symbol stehen in include/re2_fx.h. In C0 ruft niemand diese Funktionen
 * und niemand liest die Zeiger (keine Verhaltensaenderung). Die Umsetzung (Bank-Registrierung
 * FUN_8001bca0, Spawner FUN_8001BF10/FUN_8001cbe8, Pumpe FUN_8001d300, Ops 0/1/2/19/25/27/28/29/
 * 30/40/46/48/49/50/58, RNG-Strom) gehoert Spur D (BAUPLAN §3.3 C5, Orchestrator-Teilung C/D).
 */
#include "re2_fx.h"

int  (*re2fx_applier)(const int32_t p[3], int16_t gier, const int16_t box[4], uint32_t hitcode) = NULL;
void (*re2fx_se_hook)(uint32_t code, const int32_t pos[3]) = NULL;

int re2fx_register_core(const uint8_t *raw, size_t size)
{
    (void)raw; (void)size;
    return -1;   /* Stub: nichts registriert (Spur D) */
}

void re2fx_reset(void)
{
    /* Stub: kein Pool (Spur D) */
}

void re2fx_aufschlag(int re2_art, const int32_t q[3], int16_t gier)
{
    (void)re2_art; (void)q; (void)gier;   /* Stub: kein Aufschlag (Spur D) */
}

void re2fx_tick(void)
{
    /* Stub: keine Pumpe (Spur D) */
}
