/* ============================================================================================
 * probe_r38_werkstatt11b0_ton — Runde 38 Spur C, Punkt 2 (Schiebe-Ton ROOM11B0) mit dem ECHTEN audio_pc.c und
 * platform/pc/src/werkstatt11b0_pc.c (Muster probe_r34_plattform_ton: RE15_AUDIO_CAP_SYNC, kein SDL-Geraet,
 * bildsynchrone Mischung in eine Datei, 1470 Stereo-Frames je Spielbild).
 *
 * Gemessen wird, dass der Satz, den der Schub-Sub anfordert (re15_audio_room_se_snd0(7) = FUN_80045024(0x02070000)
 * @0x80052ad8/@0x80052bcc), in ROOM11B0 KLINGT und dass es DERSELBE Ton ist wie Leons Kistenschieben in ROOM1090:
 *   1. ROOM11B0, Haken aus (= Stand vor Runde 38 = Original: Satz 7 @0xE4A0 leer)   -> stumm
 *   2. ROOM11B0, Haken an (re15_pc_w11b0_init, Ergaenzung aus ROOM1171)           -> Ton, nach 50 Bildern aus
 *   3. ROOM1090 (eigener Satz 7, Haken an, greift nicht)                            -> Ton
 *   4. Fenster 2 und 3 sind Stereo-Frame fuer Stereo-Frame GLEICH (Satz/Ton/VAG/Stimme bytegleich)
 *   5. ROOM11B1 (Elza, Satz 7 leer, keine Schub-Szene): Haken greift nicht        -> stumm
 *   6. ROOM11B0 Satz 0 (Schritt, eigener Satz): Haken greift nicht (Zaehler bleibt)
 *   7. Mess-Haken RE15_W11B0_SCHUB_STUMM wird nur gelesen, wenn gesetzt (hier nicht): Zaehler = 2 Aufrufe
 * ============================================================================================ */
/* putenv ist POSIX: unter -std=c11 ohne Erweiterungen (CMAKE_C_EXTENSIONS OFF) deklariert glibc es nur mit
 * _DEFAULT_SOURCE (Memory reai-v2-linux-test-portabilitaet, Muster asset_root_pc.c). */
#if !defined(_WIN32) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE 1
#endif
#include "re15_audio.h"
#include "re15_engine.h"
#include "re15_rdt.h"
#include "re15_room.h"
#include "re15_werkstatt11b0_r38.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

/* Symbole, die sonst main.c liefert (wie probe_r34_plattform_ton / test_r31_tuer_ton). */
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

void re15_pc_w11b0_init(void);                 /* werkstatt11b0_pc.c */
unsigned re15_pc_w11b0_gespielt(void);

#define CAP_DATEI "r38_werkstatt11b0_ton.pcm"
#define FRAMES_JE_BILD 1470

static int s_fail = 0, s_pass = 0;
#define CHECK(nr, cond, ...) do {                                                  \
        if (cond) { s_pass++; printf("  PASS %2d: ", nr); printf(__VA_ARGS__); printf("\n"); } \
        else { printf("  FAIL %2d: ", nr); printf(__VA_ARGS__); printf("\n");      \
               if (!s_fail) s_fail = nr; } } while (0)

static int s_bild = 0;
static void ticks(int n) { for (int i = 0; i < n; i++) { re15_audio_tick(); s_bild++; } }

static uint8_t *s_rdt_buf = NULL;
static int raum(uint16_t id)
{
    char p[600];
    snprintf(p, sizeof p, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(id >> 12), (unsigned)id);
    FILE *f = fopen(p, "rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { fclose(f); free(b); return -1; }
    fclose(f);
    memset(&g_room_rdt, 0, sizeof g_room_rdt);
    if (re15_rdt_parse(b, (size_t)sz, &g_room_rdt) != 0) { free(b); return -1; }
    free(s_rdt_buf); s_rdt_buf = b;            /* g_room_rdt zeigt hinein */
    g_room_rdt_ok = 1;
    g_current_room_id = id;
    re15_audio_load_room_banks();              /* Raumlader: snd0 + snd1 aus der RDT (FUN_80043eac/FUN_80043fb0) */
    return 0;
}

static int16_t *fenster(int von, int bis)       /* Stereo-Samples der Bilder [von, bis) */
{
    FILE *f = fopen(CAP_DATEI, "rb");
    if (!f) return NULL;
    size_t n = (size_t)(bis - von) * FRAMES_JE_BILD * 2;
    int16_t *s = (int16_t *)malloc(n * sizeof(int16_t));
    if (s && (fseek(f, (long)von * FRAMES_JE_BILD * 4, SEEK_SET) != 0 || fread(s, sizeof(int16_t), n, f) != n)) {
        free(s); s = NULL;
    }
    fclose(f);
    return s;
}
static long long energie(int von, int bis)
{
    int16_t *s = fenster(von, bis);
    if (!s) return -1;
    long long e = 0;
    for (long i = 0; i < (long)(bis - von) * FRAMES_JE_BILD * 2; i++) e += s[i] < 0 ? -s[i] : s[i];
    free(s);
    return e;
}

int main(void)
{
    static char capenv[] = "RE15_AUDIO_CAP_SYNC=" CAP_DATEI;
    static char noint[] = "RE15_NO_INTRO=1";
    putenv(capenv);
    putenv(noint);
    re15_audio_init();
    printf("=== probe_r38_werkstatt11b0_ton (echtes audio_pc.c + werkstatt11b0_pc.c, CAP_SYNC) ===\n");
    if (raum(0x11B0) != 0) { printf("  FAIL: ROOM11B0 nicht ladbar\n"); return 1; }

    ticks(10);                                                          /* Ruhe: Bilder 0..9 */
    int a1 = s_bild; re15_audio_room_se_snd0(RE15_W11B0_SCHUB_SATZ); ticks(60);   /* Haken aus */
    re15_pc_w11b0_init();
    int a2 = s_bild; re15_audio_room_se_snd0(RE15_W11B0_SCHUB_SATZ); ticks(60);   /* Haken an  */
    unsigned n_nach_2 = re15_pc_w11b0_gespielt();
    int a6 = s_bild; re15_audio_room_se_snd0(0); ticks(5);                         /* Satz 0 = eigener Satz */
    unsigned n_nach_6 = re15_pc_w11b0_gespielt();
    ticks(55);
    if (raum(0x1090) != 0) { printf("  FAIL: ROOM1090 nicht ladbar\n"); return 1; }
    int a3 = s_bild; re15_audio_room_se_snd0(RE15_W11B0_SCHUB_SATZ); ticks(60);
    unsigned n_nach_3 = re15_pc_w11b0_gespielt();
    if (raum(0x11B1) != 0) { printf("  FAIL: ROOM11B1 nicht ladbar\n"); return 1; }
    int a5 = s_bild; re15_audio_room_se_snd0(RE15_W11B0_SCHUB_SATZ); ticks(60);
    unsigned n_nach_5 = re15_pc_w11b0_gespielt();
    ticks(20);                                                          /* Schreibpuffer leeren */

    long long e0 = energie(0, 10);
    long long e1 = energie(a1, a1 + 50);
    long long e2 = energie(a2, a2 + 40), e2r = energie(a2 + 50, a2 + 60);
    long long e3 = energie(a3, a3 + 40), e3r = energie(a3 + 50, a3 + 60);
    long long e5 = energie(a5, a5 + 50);
    int16_t *w2 = fenster(a2, a2 + 50), *w3 = fenster(a3, a3 + 50);
    long gleich = -1, erste_abw = -1;
    if (w2 && w3) {
        gleich = 0;
        for (long i = 0; i < 50L * FRAMES_JE_BILD * 2; i++) {
            if (w2[i] == w3[i]) gleich++;
            else if (erste_abw < 0) erste_abw = i;
        }
    }
    long ton_ende = -1;
    if (w2) for (long i = 50L * FRAMES_JE_BILD * 2 - 1; i >= 0; i--) if (w2[i]) { ton_ende = i / (FRAMES_JE_BILD * 2); break; }
    printf("Energie: Ruhe %lld | 11B0 Haken aus %lld | 11B0 Haken an %lld (Rest %lld, letzter Ton in Bild %ld) | 1090 %lld "
           "(Rest %lld) | 11B1 %lld | Zaehler %u/%u/%u/%u | gleiche Samples %ld von %ld (erste Abweichung %ld)\n",
           e0, e1, e2, e2r, ton_ende, e3, e3r, e5, n_nach_2, n_nach_6, n_nach_3, n_nach_5, gleich, 50L * FRAMES_JE_BILD * 2,
           erste_abw);
    CHECK(1, e0 == 0 && e1 == 0, "ROOM11B0 ohne Ergaenzung stumm (Satz 7 @0xE4A0 leer = Original) (%lld)", e1);
    CHECK(2, e2 > 0 && e2r == 0 && n_nach_2 == 1, "ROOM11B0 mit Ergaenzung: Satz 7 klingt (ROOM1171-snd0), nach 50 Bildern aus");
    CHECK(3, e3 > 0 && e3r == 0 && n_nach_3 == 1, "ROOM1090: eigener Satz 7 klingt, die Ergaenzung greift nicht");
    CHECK(4, gleich == 50L * FRAMES_JE_BILD * 2, "11B0-Schiebe-Ton == Leons Kisten-Ton (ROOM1090) Sample fuer Sample");
    CHECK(5, e5 == 0 && n_nach_5 == 1, "ROOM11B1 (Elza): keine Ergaenzung, stumm wie ausgeliefert");
    CHECK(6, n_nach_6 == 1, "ROOM11B0 Satz 0 (Schritt): Ergaenzung greift nicht");
    CHECK(7, ton_ende >= 20 && ton_ende < 40, "Tonlaenge ~1 s (VAG 4 6688 B ADPCM = 11704 Samples bei 11025 Hz), letzter Ton in Bild %ld", ton_ende);
    free(w2); free(w3);
    remove(CAP_DATEI);
    printf("=== %s: %d bestanden, erste Verletzung %d ===\n", s_fail ? "ROT" : "GRUEN", s_pass, s_fail);
    return s_fail;
}
