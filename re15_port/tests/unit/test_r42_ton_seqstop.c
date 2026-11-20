/* test_r42_ton_seqstop.c — Strang "ton", Stufe 4 (2026-11-20): Musik-Ebenen stoppen und starten wie SsSeqStop / SsSeqPlay.
 *
 * Nutzer 19.11.2026: "Der Helikopter Rotos sound klingt auch leicht anders."
 * Messbericht U4: im Original schaltet Sce_bgm_control (SCD 0x54 -> FUN_80044da4) eine Musik-Ebene so:
 *   Fall 1: SsSeqSetVol @0x80044e24 + SsSeqPlay(Handle, 1, 1) @0x80044e40. Snd_SetPlayMode @0x8005ff30 setzt nur das
 *           Spiel-Bit (+0x90 |= 1 @0x8005ffd4); den Lesezeiger fasst es nicht an. Nach dem Laden mit Flag (SsSeqPlay(.., 0 =
 *           SSPLAY_PAUSE, ..) @0x80044980 fuer SUB SEQ0) steht er am Anfang -> die erste Note setzt mit ihrer Huellkurve ein.
 *   Fall 2: SsSeqStop @0x80044e64 -> _SsSndStop @0x80060270: SpuVmSeqKeyOff @0x80060320 (Key-Off -> Ausklang der Huellkurve),
 *           Lesezeiger auf den Anfang (`lw a1,0x8(s0)` @0x80060340 -> `sw a1,0x4(s0)` @0x80060394).
 * Der Port drehte nur die Lautstaerke der Ebene auf 0 bzw. voll: harter Schnitt, harter Einsatz mitten in einer Note.
 *
 * Der Riegel linkt das echte audio_pc.c und rendert deterministisch (RE15_AUDIO_CAP_SYNC), ROOM1170, nur der Rotor (SUB_15 SEQ0):
 *   (1) Einsatz: der erste 50-ms-Block nach Sce_bgm_control(1,1) liegt deutlich unter dem Dauerpegel, und der Pegel steigt an
 *       (SUB_15 Ton-ADSR1 0xafee = exponentieller Anstieg, Shift 11). Vorher: sofort voll.
 *   (2) Ausklang: nach Sce_bgm_control(1,2) klingt der Rotor weiter (Block 0..50 ms laut, bei 0,5 s noch hoerbar) und ist nach
 *       2 s still (ADSR2 0xdced = exponentieller Ausklang, Shift 13). Vorher: Stille ab dem ersten Block.
 *   (3) Neustart vom Anfang: dieselben 120 Takte nach einem zweiten Sce_bgm_control(1,1) sind Abtastwert fuer Abtastwert gleich
 *       den ersten 120 Takten nach dem ersten (Lesezeiger stand nach dem Stopp wieder am Anfang).
 * (Die erste Note steht in SUB_15 SEQ0 bei Tick 100 von 480 je Viertel, Tempo 500000 us -> 0,104 s nach dem Start.)
 * Gibt die Einsatz- und Ausklangzeit aus (Messbericht Original: Einsatz ~0,25 s, Ausklang ~1,0 s).
 *
 * GEGENPROBE: derselbe Quelltext mit dem audio_pc.c von r42/story 71caea668 faellt in (1), (2) und (3).
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
#define BLOCK     (RATE / 20)            /* 50-ms-Block */

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

static const char *s_cap = "r42_ton_seqstop.pcm";
static long s_takt = 0;
static long laufen(int ticks) { const long t0 = s_takt; for (int i = 0; i < ticks; i++) re15_audio_tick(); s_takt += ticks; return t0; }

static int16_t *s_pcm = NULL; static long s_pcm_n = 0;
/* Effektivwert von n Stereo-Bildern ab Bild a (Mittel beider Kanaele) */
static double rms_bilder(long a, long n)
{
    if (!s_pcm || a < 0 || a + n > s_pcm_n) { printf("FAIL: Mitschnitt zu kurz (%ld, gebraucht %ld)\n", s_pcm_n, a + n); fails++; return 0.0; }
    double acc = 0.0;
    for (long i = a; i < a + n; i++) {
        const double l = s_pcm[i * 2], r = s_pcm[i * 2 + 1];
        acc += (l * l + r * r) / 2.0;
    }
    return sqrt(acc / (double)n);
}
static int spitze_bilder(long a, long n)
{
    int pk = 0;
    if (!s_pcm || a < 0 || a + n > s_pcm_n) return 99999;
    for (long i = a * 2; i < (a + n) * 2; i++) if (abs(s_pcm[i]) > pk) pk = abs(s_pcm[i]);
    return pk;
}
static double db(double x, double ref) { return (x > 0.0 && ref > 0.0) ? 20.0 * log10(x / ref) : -200.0; }

int main(void)
{
    static char capenv[1024];
    remove(s_cap);
    snprintf(capenv, sizeof capenv, "RE15_AUDIO_CAP_SYNC=%s", s_cap);
    putenv(capenv);
    static char noint[] = "RE15_NO_INTRO=1";
    putenv(noint);
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("== Strang ton Stufe 4: Musik-Ebenen stoppen/starten wie SsSeqStop / SsSeqPlay (Rotor ROOM1170) ==\n");
    re15_audio_init();

    enum { N_AN = 120, N_AUS = 90 };
    re15_audio_start_room_bgm(0, 0x17);                          /* ROOM1170: MAIN32 + SUB_15, beide per Flag angehalten */
    (void)laufen(90);                                            /* der Port laedt die Bank aufgeschoben (s_bgm_pending, ~60 Takte);
                                                                  * erst danach trifft Sce_bgm_control die Sequenz selbst */
    re15_audio_seq_ctl(1, 1);                                    /* Rotor an (Fall 1) */
    const long t_an1 = laufen(N_AN);
    re15_audio_seq_ctl(1, 2);                                    /* Rotor aus (Fall 2) */
    const long t_aus = laufen(N_AUS);
    re15_audio_seq_ctl(1, 1);                                    /* Rotor wieder an */
    const long t_an2 = laufen(N_AN);
    re15_audio_shutdown();

    {   FILE *f = fopen(s_cap, "rb");
        if (f) {
            fseek(f, 0, SEEK_END); const long sz = ftell(f); fseek(f, 0, SEEK_SET);
            s_pcm = (int16_t *)malloc((size_t)sz);
            if (s_pcm && fread(s_pcm, 1, (size_t)sz, f) == (size_t)sz) s_pcm_n = sz / 4;
            fclose(f);
        }
        CHECK(s_pcm_n >= s_takt * PER_TICK, "Mitschnitt %s: %ld Stereo-Bilder, erwartet %ld", s_cap, s_pcm_n, s_takt * PER_TICK); }
    if (s_pcm_n < s_takt * PER_TICK) { printf("ROT\n"); return 1; }

    /* Dauerpegel: Takte 60..119 nach dem Einschalten */
    const double dauer = rms_bilder((t_an1 + 60) * PER_TICK, 60L * PER_TICK);
    printf("  Dauerpegel Rotor RMS %.1f\n", dauer);
    CHECK(dauer > 300.0, "Rotor klingt nicht (RMS %.1f) - Messung ungueltig", dauer);

    /* (1) Einsatz: 50-ms-Bloecke ab dem Einschalten */
    const long a1 = t_an1 * PER_TICK;
    double e0 = rms_bilder(a1, BLOCK);
    double t_einsatz = -1.0;
    printf("  (1) Einsatz, 50-ms-Bloecke relativ zum Dauerpegel [dB]:");
    for (int b = 0; b < 12; b++) {
        const double v = db(rms_bilder(a1 + (long)b * BLOCK, BLOCK), dauer);
        printf(" %.1f", v);
        if (t_einsatz < 0.0 && v >= -3.0) t_einsatz = (b + 0.5) * 0.05;
    }
    printf("\n      Zeit bis -3 dB unter Dauerpegel: %.3f s (Original laut Messbericht U4 ~0,25 s)\n", t_einsatz);
    CHECK(db(e0, dauer) < -6.0, "(1) erster 50-ms-Block nach dem Einschalten schon %.1f dB (Soll < -6: Anstieg der Huellkurve, "
          "SsSeqPlay @0x80044e40 setzt am Sequenzanfang ein)", db(e0, dauer));
    CHECK(t_einsatz > 0.05 && t_einsatz < 0.6, "(1) Einsatzzeit %.3f s ausserhalb (0,05; 0,6)", t_einsatz);

    /* (2) Ausklang: 50-ms-Bloecke ab dem Abschalten */
    const long a2 = t_aus * PER_TICK;
    const double pegel_vor = rms_bilder(a2 - 10L * PER_TICK, 10L * PER_TICK);
    double t_ausklang = -1.0;
    printf("  (2) Ausklang, 50-ms-Bloecke relativ zum Pegel davor [dB]:");
    for (int b = 0; b < 30; b++) {
        const double v = db(rms_bilder(a2 + (long)b * BLOCK, BLOCK), pegel_vor);
        if (b < 24) printf(" %.1f", v);
        if (t_ausklang < 0.0 && v <= -24.0) t_ausklang = (b + 0.5) * 0.05;
    }
    printf("\n      Zeit bis -24 dB: %.3f s (Original laut Messbericht U4: 7-9-kHz-Band -42 -> -66 dB in ~1,0 s)\n", t_ausklang);
    const double v0 = db(rms_bilder(a2, BLOCK), pegel_vor);
    const double v05 = db(rms_bilder(a2 + 10L * BLOCK, BLOCK), pegel_vor);
    const int pk_still = spitze_bilder((t_aus + 60) * PER_TICK, 30L * PER_TICK);
    CHECK(v0 > -6.0, "(2) erster 50-ms-Block nach dem Abschalten %.1f dB (Soll > -6: Key-Off -> Ausklang, SpuVmSeqKeyOff @0x80060320)", v0);
    CHECK(v05 > -40.0, "(2) bei 0,5 s nach dem Abschalten %.1f dB (Soll > -40: der Ausklang laeuft noch)", v05);
    CHECK(pk_still == 0, "(2) 2,0..3,0 s nach dem Abschalten Spitze %d (Soll 0: Ausklang beendet, Rotor trocken)", pk_still);

    /* (3) Neustart vom Sequenzanfang: beide Einschalt-Abschnitte bitgleich */
    long diff = 0;
    for (long i = 0; i < (long)N_AN * PER_TICK * 2; i++)
        if (s_pcm[t_an1 * PER_TICK * 2 + i] != s_pcm[t_an2 * PER_TICK * 2 + i]) diff++;
    printf("  (3) zweites Einschalten gegen erstes: %ld von %ld Abtastwerten verschieden (Soll 0)\n", diff, (long)N_AN * PER_TICK * 2);
    CHECK(diff == 0, "(3) zweites Einschalten weicht in %ld Abtastwerten ab - Lesezeiger nicht am Anfang "
          "(_SsSndStop @0x80060394 `sw a1,0x4(s0)`)", diff);

    if (!getenv("RE15_TON_S4_BEHALTEN")) remove(s_cap);   /* Messlauf fuer Hoerbeispiele: Mitschnitt behalten */
    free(s_pcm);
    printf("%s: %d/%d Pruefungen bestanden\n", fails ? "ROT" : "GRUEN", checks - fails, checks);
    return fails ? 1 : 0;
}
