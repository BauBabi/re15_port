/* ============================================================================================
 * probe_r34_plattform_ton — Runde 34 Spur C, C4 mit dem ECHTEN audio_pc.c (Muster
 * test_r31_tuer_ton: RE15_AUDIO_CAP_SYNC, kein SDL-Geraet, bildsynchrone Mischung in eine Datei).
 *
 * Die Unit-Sonde probe_r34_plattform prueft die Ton-WEICHE gegen Spione; hier wird gemessen, dass
 * der jeweilige Satz wirklich KLINGT (Energie der gemischten PCM im Fenster nach dem Aufruf) und
 * dass leere Saetze stumm bleiben:
 *   1. Zusatzbank ARMS10 Satz 10 (Saeure-Aufschlag, E9; EDH @0x28 `00 00 33 20`)  -> Ton
 *   2. ARMS10 Satz 11 (`ff ff ff ff`)                                            -> stumm
 *   3. Zusatzbank ARMS11 Satz 10 (Brand-Aufschlag)                                 -> Ton
 *   4. re15_pc_esp_se(0x010A0601) mit geladener Granatenbank ARMS09 (Satz 0x0A, EDH @0x28
 *      `00 00 13 10`, Byte1 0x06 wirkungslos)                                     -> Ton
 *   5. re15_pc_esp_se(0x04080001) = CORE00 Satz 8 (Explosion, `00 00 93 00`)       -> Ton
 *   6. re15_pc_esp_se(0x00080001) Bank 0 (nicht resident)                          -> stumm
 *   7. die Zusatzbaenke ersetzen die Waffenbank NICHT: nach 1-3 spielt 4 aus ARMS09
 * Offset je Spielbild = 1470 Stereo-Frames (RE15_AUDIO_RATE/30, audio_pc.c CAP_SYNC).
 * ============================================================================================ */
#include "re15_audio.h"
#include "re15_engine.h"
#include "fx_plattform_pc.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

/* Symbole, die sonst main.c liefert (wie test_r31_tuer_ton / test_rotor_bgm_pin). */
re15_engine_state_t g_engine;
void re15_debug_text(int x, int y, int z, const char *text) { (void)x; (void)y; (void)z; (void)text; }
unsigned char *re15_asset_read_file(const char *path, int *out_size)
{
    char buf[512];
    const char *pfx = "shared_assets/PSX/";
    if (strncmp(path, pfx, strlen(pfx)) == 0)
        snprintf(buf, sizeof buf, "%s/%s", RE15_ASSET_PSX_DIR, path + strlen(pfx));
    else
        snprintf(buf, sizeof buf, "%s/%s", RE15_ASSET_PSX_DIR, path);
    FILE *f = fopen(buf, "rb");
    if (!f) { snprintf(buf, sizeof buf, "%s", path); f = fopen(buf, "rb"); }
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    unsigned char *b = (unsigned char *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b && out_size) *out_size = (int)sz;
    return b;
}

#define CAP_DATEI "r34_plattform_ton.pcm"
#define FRAMES_JE_BILD 1470            /* RE15_AUDIO_RATE / 30, Stereo-Frames */

static int s_fail = 0, s_pass = 0;
#define CHECK(nr, cond, ...) do {                                                  \
        if (cond) { s_pass++; printf("  PASS %2d: ", nr); printf(__VA_ARGS__); printf("\n"); } \
        else { printf("  FAIL %2d: ", nr); printf(__VA_ARGS__); printf("\n");      \
               if (!s_fail) s_fail = nr; } } while (0)

static int s_bild = 0;
static void ticks(int n) { for (int i = 0; i < n; i++) { re15_audio_tick(); s_bild++; } }

/* Summe |L|+|R| ueber die Bilder [von, bis) der Aufnahme (Datei nach weiteren Takten gelesen). */
static long long energie(int von, int bis)
{
    FILE *f = fopen(CAP_DATEI, "rb");
    if (!f) return -1;
    long long e = 0;
    if (fseek(f, (long)von * FRAMES_JE_BILD * 4, SEEK_SET) == 0) {
        int16_t s[2];
        for (long i = 0; i < (long)(bis - von) * FRAMES_JE_BILD; i++) {
            if (fread(s, sizeof s, 1, f) != 1) { e = -2; break; }
            e += (s[0] < 0 ? -s[0] : s[0]) + (s[1] < 0 ? -s[1] : s[1]);
        }
    }
    fclose(f);
    return e;
}

int main(void)
{
    static char capenv[] = "RE15_AUDIO_CAP_SYNC=" CAP_DATEI;
    static char noint[] = "RE15_NO_INTRO=1";
    putenv(capenv);
    putenv(noint);
    re15_audio_init();
    printf("=== probe_r34_plattform_ton (echtes audio_pc.c, CAP_SYNC) ===\n");

    ticks(10);                                                  /* Grundrauschen: Bilder 0..9 */
    int a1 = s_bild; re15_audio_arms_zusatz_se(0x10, 10); ticks(60);
    int a2 = s_bild; re15_audio_arms_zusatz_se(0x10, 11); ticks(60);
    int a3 = s_bild; re15_audio_arms_zusatz_se(0x11, 10); ticks(60);
    re15_audio_prime_weapon(9);                                 /* Granatenbank (RE15_EQUIP / Menue) */
    int a4 = s_bild; re15_pc_esp_se(0x010A0601u, NULL); ticks(60);
    int a5 = s_bild; re15_pc_esp_se(0x04080001u, NULL); ticks(60);
    int a6 = s_bild; re15_pc_esp_se(0x00080001u, NULL); ticks(60);
    ticks(20);                                                  /* Schreibpuffer der Aufnahme leeren */

    long long e0 = energie(0, 10);
    long long e1 = energie(a1, a1 + 30), e1r = energie(a1 + 50, a1 + 60);
    long long e2 = energie(a2, a2 + 30);
    long long e3 = energie(a3, a3 + 30), e3r = energie(a3 + 50, a3 + 60);
    long long e4 = energie(a4, a4 + 30), e4r = energie(a4 + 50, a4 + 60);
    long long e5 = energie(a5, a5 + 30), e5r = energie(a5 + 50, a5 + 60);
    long long e6 = energie(a6, a6 + 30);
    printf("Energie: Ruhe %lld | ARMS10 S10 %lld (Rest %lld) | ARMS10 S11 %lld | ARMS11 S10 %lld (Rest %lld) |"
           " ESP 0x010A0601 %lld (Rest %lld) | ESP 0x04080001 %lld (Rest %lld) | Bank 0 %lld\n",
           e0, e1, e1r, e2, e3, e3r, e4, e4r, e5, e5r, e6);
    CHECK(1, e0 == 0, "Grundzustand stumm (%lld)", e0);
    CHECK(2, e1 > 0 && e1r == 0, "ARMS10 Satz 10 (Saeure) klingt und ist nach 50 Bildern aus");
    CHECK(3, e2 == 0, "ARMS10 Satz 11 (ff ff ff ff) stumm");
    CHECK(4, e3 > 0 && e3r == 0, "ARMS11 Satz 10 (Brand) klingt");
    CHECK(5, e4 > 0 && e4r == 0, "ESP-Haken 0x010A0601 -> ARMS09 Satz 0x0A klingt (Waffenbank unberuehrt von den Zusatzbaenken)");
    CHECK(6, e5 > 0 && e5r == 0, "ESP-Haken 0x04080001 -> CORE00 Satz 8 klingt");
    CHECK(7, e6 == 0, "Bank 0 (nicht resident) stumm");
    CHECK(8, e1 != e3, "Saeure- und Brandton sind verschiedene Samples (ARMS10 VAG 3 10992 B / ARMS11 VAG 3 11664 B)");
    remove(CAP_DATEI);
    printf("=== %s: %d bestanden, erste Verletzung %d ===\n", s_fail ? "ROT" : "GRUEN", s_pass, s_fail);
    return s_fail;
}
