/* test_r42_ton_hall.c — Strang "ton", Stufe 3 (2026-11-20): Hall je Musik-Stimme statt auf der ganzen Musik-Summe.
 *
 * Nutzer 19.11.2026: "Bei der PC Version klingen die Sounds irgendwie dumpfer/anders als in der PSX Version",
 * "Der Helikopter Rotos sound klingt auch leicht anders."
 * Messbericht U3: im Original geht eine Stimme nur dann in den Hall, wenn ihr Ton das Bit 2 im Byte VagAtr.mode traegt
 * (SpuVmKeyOnNow @0x8005611c `lbu v0,DAT_800b5328` / @0x80056124 `andi v0,v0,0x4` / @0x80056128 `beq`; Bit setzen
 * @0x80056154, loeschen @0x8005618c). Der Rotor SUB_15 hat das Bit in keinem Ton -> trocken; im Original folgt nach dem
 * Ausklingen digitale Stille. MAIN32 hat es in 2 von 8 Tonen. Der Port legte den Hall auf die ganze Musik-Summe.
 *
 * Der Riegel linkt das echte audio_pc.c und rendert deterministisch (RE15_AUDIO_CAP_SYNC, wie test_r42_ton_mischer):
 *   (1) ROOM1170, nur der Rotor (SUB-Slot, Sce_bgm_control(1,1) @0x80044e00-48), 150 Spieltakte, dann Sce_bgm_control(1,2):
 *       nach dem Ausklang der Huellkurve (ab Takt 36 = 1,2 s nach dem Abschalten, seit Stufe 4 Runde 2) ist JEDER Abtastwert 0 (keine Hall-Fahne). Vorher lief der Hall des Rotors noch
 *       ~0,4 s nach (Messbericht U3/U4).
 *   (2) [RE15_TON_S3_HAKEN] ROOM1170, nur MAIN32, 300 Spieltakte: das Hall-Bit jeder klingenden Stimme ist gleich Bit 2 des
 *       Bytes VH-Ton+1 aus der Datei SOUND/MAIN32.BGM (Tontabelle = VH + 0x20 + 0x800, 32 Byte je Ton, psx-spx/libsnd
 *       VagAtr), und es klingen Stimmen beider Sorten.
 *   (3) MAIN32 abgeschaltet: der Hall der zwei Hall-Tone klingt nach (Takte 1..10 nach dem Schnitt nicht still) und ist
 *       nach 4 s bis auf den Rundungsrest (Spitze <= 2) still - der Hall ist nicht tot.
 *
 * GEGENPROBE: derselbe Quelltext mit dem audio_pc.c von r42/story 71caea668 (ohne -DRE15_TON_S3_HAKEN, der Messhaken
 * existiert dort nicht) faellt in (1): Hall-Fahne des Rotors nach dem Abschalten.
 */
#include "re15_audio.h"
#include "re15_engine.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#define RATE      44100
#define PER_TICK  (RATE / 30)

re15_engine_state_t g_engine;
void re15_debug_text(int x, int y, int z, const char *text) { (void)x; (void)y; (void)z; (void)text; }
unsigned char *re15_asset_read_file(const char *path, int *out_size)
{
    char buf[600];
    const char *pfx = "shared_assets/PSX/";
    if (strncmp(path, pfx, strlen(pfx)) == 0) snprintf(buf, sizeof buf, "%s/%s", RE15_ASSET_PSX_DIR, path + strlen(pfx));
    else snprintf(buf, sizeof buf, "%s/%s", RE15_ASSET_PSX_DIR, path);
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

static int fails = 0, checks = 0;
#define CHECK(c, ...) do { checks++; if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static const char *s_cap = "r42_ton_hall.pcm";
static long s_takt = 0;
static long laufen(int ticks) { const long t0 = s_takt; for (int i = 0; i < ticks; i++) re15_audio_tick(); s_takt += ticks; return t0; }

static int16_t *s_pcm = NULL; static long s_pcm_n = 0;
/* Spitze und Effektivwert der Takte [takt0, takt0+ticks) */
static void messen(long takt0, int ticks, int *pk, double *rms)
{
    *pk = 0; *rms = 0.0;
    const long a = takt0 * PER_TICK, n = (long)ticks * PER_TICK;
    if (!s_pcm || a + n > s_pcm_n) { printf("FAIL: Mitschnitt zu kurz (%ld, gebraucht %ld)\n", s_pcm_n, a + n); fails++; return; }
    double acc = 0.0;
    for (long i = a; i < a + n; i++) {
        const int l = s_pcm[i * 2], r = s_pcm[i * 2 + 1];
        if (abs(l) > *pk) *pk = abs(l);
        if (abs(r) > *pk) *pk = abs(r);
        acc += ((double)l * l + (double)r * r) / 2.0;
    }
    *rms = sqrt(acc / (double)n);
}

#ifdef RE15_TON_S3_HAKEN
int re15_audio_hall_stimmen(int slot, int *prog, int *tone, int *hall, int max);
#endif

int main(void)
{
    static char capenv[1024];
    remove(s_cap);
    snprintf(capenv, sizeof capenv, "RE15_AUDIO_CAP_SYNC=%s", s_cap);
    putenv(capenv);
    static char noint[] = "RE15_NO_INTRO=1";
    putenv(noint);
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("== Strang ton Stufe 3: Hall je Musik-Stimme nach tone.mode & 4 ==\n");
    re15_audio_init();

    enum { N_ROTOR = 150, N_ROTOR_NACH = 90, N_MAIN = 300, N_MAIN_NACH = 150 };
    re15_audio_start_room_bgm(0, 0x17);                          /* ROOM1170: MAIN32 + SUB_15, beide per Flag stumm */
    (void)laufen(30);
    re15_audio_seq_ctl(1, 1);                                    /* nur der Rotor */
    const long t_rotor = laufen(N_ROTOR);
    re15_audio_seq_ctl(1, 2);                                    /* Rotor aus */
    const long t_rotor_nach = laufen(N_ROTOR_NACH);

#ifdef RE15_TON_S3_HAKEN
    int n_hall = 0, n_trocken = 0, n_falsch = 0;
    int bgm_n = 0;
    unsigned char *bgm = re15_asset_read_file("SOUND/MAIN32.BGM", &bgm_n);
    long vh = -1;
    for (long i = 0; bgm && i + 4 <= bgm_n; i++) if (!memcmp(bgm + i, "pBAV", 4)) { vh = i; break; }
    CHECK(vh >= 0, "(2) SOUND/MAIN32.BGM: kein VAB-Kopf 'pBAV' gefunden");
#endif
    re15_audio_seq_ctl(0, 1);                                    /* nur MAIN32 */
    const long t_main = s_takt;
    for (int i = 0; i < N_MAIN; i++) {
        (void)laufen(1);
#ifdef RE15_TON_S3_HAKEN
        int pr[32], to[32], ha[32];
        const int n = re15_audio_hall_stimmen(0, pr, to, ha, 32);
        for (int k = 0; k < n && vh >= 0; k++) {
            const long o = vh + 0x20 + 0x800 + ((long)pr[k] * 16 + to[k]) * 32 + 1;
            const int soll = (o < bgm_n) ? ((bgm[o] & 4) ? 1 : 0) : -1;
            if (ha[k] != soll) { if (n_falsch < 5) printf("  (2) Stimme Programm %d Ton %d: Hall %d, Datei-Byte @0x%lx -> %d\n", pr[k], to[k], ha[k], o, soll); n_falsch++; }
            else if (soll) n_hall++; else n_trocken++;
        }
#endif
    }
    re15_audio_seq_ctl(0, 2);                                    /* MAIN32 aus */
    const long t_main_nach = laufen(N_MAIN_NACH);
    re15_audio_shutdown();

    {   FILE *f = fopen(s_cap, "rb");
        if (f) {
            fseek(f, 0, SEEK_END); const long sz = ftell(f); fseek(f, 0, SEEK_SET);
            s_pcm = (int16_t *)malloc((size_t)sz);
            if (s_pcm && fread(s_pcm, 1, (size_t)sz, f) == (size_t)sz) s_pcm_n = sz / 4;
            fclose(f);
        }
        CHECK(s_pcm_n >= s_takt * PER_TICK, "Mitschnitt %s: %ld Stereo-Bilder, erwartet %ld", s_cap, s_pcm_n, s_takt * PER_TICK); }

    int pk; double rms;
    messen(t_rotor + 60, N_ROTOR - 60, &pk, &rms);
    printf("  (1) Rotor an:  Spitze %d, RMS %.1f\n", pk, rms);
    CHECK(rms > 300.0, "(1) Rotor klingt nicht (RMS %.1f) - Messung ungueltig", rms);
    /* Stufe 4 (U4): Sce_bgm_control(1,2) ist jetzt SsSeqStop @0x80044e64 = Key-Off -> der Rotor klingt nach seiner Huellkurve
     * aus (ADSR2 0xdced, exponentiell Shift 13; psx-spx-Rechnung mit aufgerundetem Betrag: hoechstens 1,01 s ab Pegel 0x7fff,
     * gemessen 0,986 s; test_r42_ton_seqstop). Der Hall-Riegel misst deshalb NACH dem Ausklang: ab Takt 36 (1,2 s; Stufe 4
     * Runde 2, vorher Takt 60, solange der Ausklang 1,33 s dauerte) muss jeder Abtastwert 0 sein - eine Hall-Fahne liefe dort
     * noch weiter. */
    messen(t_rotor_nach + 36, N_ROTOR_NACH - 36, &pk, &rms);
    printf("  (1) Rotor aus: Spitze %d, RMS %.2f in den Takten 36..%d nach dem Abschalten (Soll 0: Rotor-Tone ohne Hall-Bit)\n",
           pk, rms, N_ROTOR_NACH - 1);
    CHECK(pk == 0, "(1) Rotor SUB_15: nach dem Abschalten noch Spitze %d / RMS %.2f - Hall-Fahne, obwohl kein Rotor-Ton das Hall-Bit "
          "traegt (SpuVmKeyOnNow @0x80056124 `andi v0,v0,0x4`)", pk, rms);

#ifdef RE15_TON_S3_HAKEN
    printf("  (2) MAIN32: Stimmen-Takte mit Hall %d, trocken %d, falsch %d\n", n_hall, n_trocken, n_falsch);
    CHECK(n_falsch == 0 && n_hall > 0 && n_trocken > 0,
          "(2) MAIN32: Hall-Bit der Stimmen gegen VH-Ton+1 & 4: %d falsch, %d mit Hall, %d trocken (Soll 0 / >0 / >0)",
          n_falsch, n_hall, n_trocken);
    free(bgm);
#endif
    messen(t_main + 60, N_MAIN - 60, &pk, &rms);
    const double rms_an = rms;
    double rms_nach1, rms_nach4;
    messen(t_main_nach + 1, 10, &pk, &rms_nach1);
    messen(t_main_nach + 120, 30, &pk, &rms_nach4);
    printf("  (3) MAIN32 an RMS %.1f, Takte 1..10 nach dem Schnitt RMS %.1f, Takte 120..149 RMS %.2f (Spitze %d)\n",
           rms_an, rms_nach1, rms_nach4, pk);
    /* nach 4 s: Spitze hoechstens 2 - die ganzzahlige Rueckkopplung des Halls (psx-spx "SPU Reverb Formula", Ergebnis
     * je Schritt >> 15) laeuft in einen Rest von +-1 aus; gemessen RMS 0,71 / Spitze 1, mehr als 60 dB unter dem Pegel. */
    CHECK(rms_an > 100.0 && rms_nach1 > 1.0 && pk <= 2,
          "(3) MAIN32: Hall-Nachklang nach dem Schnitt RMS %.1f (Soll > 1), nach 4 s Spitze %d (Soll <= 2)", rms_nach1, pk);

    remove(s_cap);
    free(s_pcm);
    printf("%s: %d/%d Pruefungen bestanden\n", fails ? "ROT" : "GRUEN", checks - fails, checks);
    return fails ? 1 : 0;
}
