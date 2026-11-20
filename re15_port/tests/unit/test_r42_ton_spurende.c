/* test_r42_ton_spurende.c — Strang "ton", Stufe 7 Runde 2 (2026-11-20): Spurende einer Musik-Sequenz wie libsnd.
 *
 * Pruefbericht Stufe 7 Runde 1 (rot 3): im echten Fenster uebersteuert die Musik der Eingangshalle ROOM1030 (MAIN01 +
 * SUB_00) an ~31 Stellen in 53 s. Gemessen im Original (DuckStation, scratchpad ton/s7r2/orig/o1030): SUB_00 klingt nur
 * EINMAL (18,26 s, keine NRPN-Schleife im Stueck); 69 s nach dem Raumwechsel ist seq 1 aus (+0x90 = 0, Lesezeiger =
 * Anfang). Der Port wickelte jede Sequenz am Spurende endlos von vorn -> SUB_00 alle 18,26 s neu, jedes Mal mit dem
 * Schlussakkord an der Vollaussteuerung.
 *
 * Original-Rechnung (ghidra1_V2.txt, selbst disassembliert):
 *   SsSeqPlay(Handle, Modus, l_count = 1): `ori a2,zero,0x1` @0x80044748 (MAIN) / @0x80044984 (SUB SEQ0) / @0x800449c4
 *   (SEQ#2) / @0x80044e44 (Sce_bgm_control Fall 1); Snd_SetPlayMode `sh a3,0x46(t0)` @0x8005ffb0.
 *   _SsGetMetaEvent Spurende (0x2f) @0x8005f140-0x8005f278: +0x48 += 1; l_count 0 -> von vorn; +0x48 < l_count -> von vorn;
 *   sonst Spiel-Bits weg, |= 4 @0x8005f224, SpuVmSeqKeyOff @0x8005f268; SsSeqCalledTbyT @0x8006060c -> _SsSndStop @0x80060618.
 *
 * Der Riegel linkt das echte audio_pc.c und rendert deterministisch (RE15_AUDIO_CAP_SYNC):
 *   (1) ROOM1030 nur SUB_00 (Sce_bgm_control(1,1); MAIN01 bleibt per Flag angehalten): klingt in den ersten Sekunden;
 *   (2) ist 24..30 s nach dem Einschalten still (18,26 s Stueck + Ausklang der Huellkurven + Hall). Vorher: klingt weiter;
 *   (3) die Laenge stimmt: letzte hoerbare Stelle (> -60 dB unter dem Dauerpegel) zwischen 18,26 s und 21 s;
 *   (4) eine Sequenz MIT eigener NRPN-Endlosschleife (ROOM1170 MAIN32, 23,94 s bis Spurende) klingt nach 30 s weiter.
 *
 * GEGENPROBE: audio_pc.c von ffe8ad7d3 (Stufe 7 Runde 1) -> (2) und (3) rot (scratchpad ton/s7r2/gegen/).
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

static long s_takt = 0;
static long laufen(int ticks) { const long t0 = s_takt; for (int i = 0; i < ticks; i++) re15_audio_tick(); s_takt += ticks; return t0; }

static int16_t *s_pcm = NULL; static long s_pcm_n = 0;
static int lies(const char *p)
{
    free(s_pcm); s_pcm = NULL; s_pcm_n = 0;
    FILE *f = fopen(p, "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END); const long sz = ftell(f); fseek(f, 0, SEEK_SET);
    s_pcm = (int16_t *)malloc((size_t)sz);
    if (s_pcm && fread(s_pcm, 1, (size_t)sz, f) == (size_t)sz) s_pcm_n = sz / 4;
    fclose(f);
    return s_pcm_n > 0;
}
static double rms_bilder(long a, long n)
{
    if (!s_pcm || a < 0 || a + n > s_pcm_n) { printf("FAIL: Mitschnitt zu kurz (%ld, gebraucht %ld)\n", s_pcm_n, a + n); fails++; return 0.0; }
    double acc = 0.0;
    for (long i = a; i < a + n; i++) { const double l = s_pcm[i * 2], r = s_pcm[i * 2 + 1]; acc += (l * l + r * r) / 2.0; }
    return sqrt(acc / (double)n);
}
static int spitze_bilder(long a, long n)
{
    int pk = 0;
    if (!s_pcm || a < 0 || a + n > s_pcm_n) return 99999;
    for (long i = a * 2; i < (a + n) * 2; i++) if (abs(s_pcm[i]) > pk) pk = abs(s_pcm[i]);
    return pk;
}

/* Raum laden, Bank aufgeschoben laden lassen (90 Takte), Ebene `slot` einschalten, `sek` Sekunden rendern.
 * Beide Raeume laufen in EINER Sitzung hintereinander (ein Mitschnitt; ein zweites re15_audio_init schreibt keinen). */
static long lauf(int raum, int slot, int sek)
{
    re15_audio_start_room_bgm(0, raum);
    (void)laufen(90);
    re15_audio_seq_ctl(slot, 1);
    return laufen(sek * 30);
}

int main(void)
{
    static char noint[] = "RE15_NO_INTRO=1";
    putenv(noint);
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("== Strang ton Stufe 7 Runde 2: Spurende wie _SsGetMetaEvent (l_count 1) ==\n");
    static char capenv[1024];
    const char *cap = "r42_ton_spurende.pcm";
    remove(cap);
    snprintf(capenv, sizeof capenv, "RE15_AUDIO_CAP_SYNC=%s", cap);
    putenv(capenv);
    re15_audio_init();
    const long a = lauf(0x03, 1, 30) * PER_TICK;      /* ROOM1030, nur SUB_00 (MAIN01 bleibt per Flag angehalten) */
    const long b = lauf(0x17, 0, 32) * PER_TICK;      /* ROOM1170, nur MAIN32 */
    re15_audio_shutdown();
    if (!lies(cap) || s_pcm_n < b + 32L * RATE) { printf("FAIL: Mitschnitt fehlt/zu kurz (%ld, gebraucht %ld)\nROT\n", s_pcm_n, b + 32L * RATE); return 1; }

    /* (1)-(3) ROOM1030, nur SUB_00 */
    const double dauer = rms_bilder(a + 2L * RATE, 8L * RATE);
    printf("  (1) SUB_00 2..10 s nach dem Einschalten: RMS %.1f\n", dauer);
    CHECK(dauer > 300.0, "(1) SUB_00 klingt nicht (RMS %.1f) - Messung ungueltig", dauer);
    const int pk_spaet = spitze_bilder(a + 24L * RATE, 6L * RATE);
    printf("  (2) 24..30 s nach dem Einschalten: Spitze %d (Soll klein: Stueck nach 18,26 s zu Ende)\n", pk_spaet);
    CHECK(pk_spaet <= 4, "(2) SUB_00 klingt 24..30 s nach dem Einschalten noch (Spitze %d): Spurende wickelt von vorn statt "
          "l_count 1 (_SsGetMetaEvent @0x8005f174/@0x8005f178)", pk_spaet);
    /* (3) letzte 100-ms-Stelle ueber -60 dB (Leistung) unter dem Dauerpegel */
    double t_ende = -1.0;
    for (long k = 30L * 10 - 1; k >= 0; k--) {
        const double v = rms_bilder(a + k * (RATE / 10), RATE / 10);
        if (v > 0.0 && 20.0 * log10(v / dauer) > -60.0) { t_ende = (k + 1) * 0.1; break; }
    }
    printf("  (3) letzte hoerbare Stelle (> -60 dB): %.1f s nach dem Einschalten (Stueck 18,26 s bis Spurende)\n", t_ende);
    CHECK(t_ende >= 18.2 && t_ende <= 21.0, "(3) Ende bei %.1f s ausserhalb [18,2; 21,0]", t_ende);

    /* (4) ROOM1170, nur MAIN32 (NRPN-Endlosschleife, CC6 127): klingt nach 30 s weiter */
    const double m_anf = rms_bilder(b + 2L * RATE, 8L * RATE), m_spaet = rms_bilder(b + 26L * RATE, 6L * RATE);
    printf("  (4) MAIN32 2..10 s RMS %.1f, 26..32 s RMS %.1f (Schleife im Stueck, laeuft weiter)\n", m_anf, m_spaet);
    CHECK(m_anf > 300.0 && m_spaet > 0.3 * m_anf, "(4) MAIN32 mit NRPN-Endlosschleife verstummt (%.1f -> %.1f)", m_anf, m_spaet);

    free(s_pcm);
    remove(cap);
    printf("%d/%d Pruefungen gruen\n%s\n", checks - fails, checks, fails ? "ROT" : "GRUEN");
    return fails ? 1 : 0;
}
