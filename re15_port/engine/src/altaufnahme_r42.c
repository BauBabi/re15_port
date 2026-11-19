/*
 * RE1.5 Rebuilt — Runde 42 Strang "namen", Stufe 6: veraltete Aufnahmen (alter Satz unter neuem Untertitel) bleiben stumm, bis die
 * neue Aufnahme da ist. Befund, PORT-WAHL und Nachweis: include/re15_altaufnahme_r42.h. Tabelle: gen/altaufnahme_r42_tabelle.inc
 * (tools/r42_namen/altaufnahmen_bauen.py, Stand Tag v0.8.30). Aufrufer: platform/pc/src/audio_pc.c re15_voice_load_clip.
 */
#include <stddef.h>
#include <stdint.h>

#include "re15_altaufnahme_r42.h"

static const re15_altaufnahme_r42_t k_alt[] = {
#include "gen/altaufnahme_r42_tabelle.inc"
};
#define ALT_N ((int)(sizeof k_alt / sizeof k_alt[0]))

int re15_altaufnahme_r42_anzahl(void) { return ALT_N; }

const re15_altaufnahme_r42_t *re15_altaufnahme_r42_eintrag(int i)
{
    return (i >= 0 && i < ALT_N) ? &k_alt[i] : NULL;
}

uint64_t re15_altaufnahme_r42_summe(const uint8_t *bytes, size_t n)
{
    uint64_t h = 0xcbf29ce484222325ull;
    for (size_t i = 0; i < n; i++) h = (h ^ bytes[i]) * 0x100000001b3ull;
    return h;
}

/* Pfadvergleich ohne Gross/Klein, Rueckstrich wie '/'; die Tabelle ist klein geschrieben. */
static int pfad_gleich(const char *a, const char *klein)
{
    for (;; a++, klein++) {
        char c = *a;
        if (c == 0x5C) c = '/';   /* Rueckstrich */
        if (c >= 'A' && c <= 'Z') c = (char)(c + ('a' - 'A'));
        if (c != *klein) return 0;
        if (!c) return 1;
    }
}

int re15_altaufnahme_r42_ist(const char *relpfad, const uint8_t *bytes, size_t n)
{
    if (!relpfad || !bytes || n == 0) return 0;
    for (int i = 0; i < ALT_N; i++) {
        if (k_alt[i].groesse != n || !pfad_gleich(relpfad, k_alt[i].pfad)) continue;
        return re15_altaufnahme_r42_summe(bytes, n) == k_alt[i].fnv;
    }
    return 0;
}
