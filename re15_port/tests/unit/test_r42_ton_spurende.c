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
 * Stufe 7 Runde 3 (Pruefbericht Runde 2, rot): ein zu Ende gespieltes Stueck muss beim naechsten SsSeqPlay WIEDER VON VORN
 * klingen. _SsSndStop setzt den Lesezeiger auf den Anfang (`sw a1,0x4(s0)` @0x80060394, Quelle `lw a1,0x8(s0)`
 * @0x80060340); SsSeqPlay -> Snd_SetPlayMode Fall 1 (@0x8005ffd4 / @0x8005ffe8) fasst ihn nicht an. Savestate
 * s7r2/orig/o1030/r1030_spaet.sav (SUB_00 zu Ende), Sequenz-Objekt 0x800b8820: +4 = +8 = 0x801eed10 = pQES 0x801eed00 + 16
 * (Kopf 15 + erste Zeitangabe `00`), gemessen mit scratchpad ton/s7r3/zeiger.py.
 *   (5) ROOM1030 SUB_00 nach dem Stueckende noch einmal Sce_bgm_control(1,1): klingt wie beim ersten Mal
 *       (RMS 0..2 s und 2..10 s je 0,9..1,1 des ersten Spiels). Vorher: 25 s RMS 0 (Lesezeiger 18 statt 16).
 *   (6) Spielweg ROOM4000 -> ROOM4000 (Door_aot_set Slot 7 @0x0114C, cut 5): MAIN12 (31,6 s, ohne Wiederholmarke) zu Ende,
 *       Raum mit gleichem MAIN/SUB-Eintrag neu betreten (@0x80044280: nichts laden), main00 schaltet MAIN12 wieder ein
 *       (`54 00 01` @0x012CE) -> klingt wieder (RMS 2..8 s >= 0,9 des ersten Spiels).
 *
 * GEGENPROBE: audio_pc.c von ffe8ad7d3 (Stufe 7 Runde 1) -> (2) und (3) rot (scratchpad ton/s7r2/gegen/);
 *             audio_pc.c von 225fbfae9 (Stufe 7 Runde 2) -> (5) und (6) rot (scratchpad ton/s7r3/gegen/).
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
static long lauf(int stufe, int raum, int slot, int sek)
{
    re15_audio_start_room_bgm(stufe, raum);
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
    const long a = lauf(0, 0x03, 1, 30) * PER_TICK;   /* ROOM1030, nur SUB_00 (MAIN01 bleibt per Flag angehalten) */
    re15_audio_seq_ctl(1, 1);                         /* (5) Stueck ist zu Ende: SsSeqPlay noch einmal */
    const long a2 = laufen(25 * 30) * PER_TICK;
    const long b = lauf(0, 0x17, 0, 32) * PER_TICK;   /* ROOM1170, nur MAIN32 */
    const long c = lauf(3, 0x00, 0, 36) * PER_TICK;   /* (6) ROOM4000, MAIN12 (31,6 s) zu Ende spielen */
    const long c2 = lauf(3, 0x00, 0, 10) * PER_TICK;  /* ROOM4000 cut 5: gleicher Eintrag, MAIN12 wieder ein */
    re15_audio_shutdown();
    if (!lies(cap) || s_pcm_n < c2 + 10L * RATE) { printf("FAIL: Mitschnitt fehlt/zu kurz (%ld, gebraucht %ld)\nROT\n", s_pcm_n, c2 + 10L * RATE); return 1; }

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

    /* (5) SUB_00 nach dem Stueckende noch einmal eingeschaltet */
    const double e_anf = rms_bilder(a, 2L * RATE), z_anf = rms_bilder(a2, 2L * RATE);
    const double z_dauer = rms_bilder(a2 + 2L * RATE, 8L * RATE);
    printf("  (5) SUB_00 zweites Spiel: 0..2 s RMS %.1f (erstes %.1f), 2..10 s RMS %.1f (erstes %.1f)\n",
           z_anf, e_anf, z_dauer, dauer);
    CHECK(e_anf > 0.0 && z_anf >= 0.9 * e_anf && z_anf <= 1.1 * e_anf && z_dauer >= 0.9 * dauer && z_dauer <= 1.1 * dauer,
          "(5) SUB_00 klingt nach dem Stueckende beim zweiten SsSeqPlay nicht wie beim ersten (%.1f/%.1f gegen %.1f/%.1f): "
          "Lesezeiger nach _SsSndStop nicht am Anfang (@0x80060394)", z_anf, z_dauer, e_anf, dauer);

    /* (6) ROOM4000: MAIN12 zu Ende, Raum neu betreten, MAIN12 wieder ein */
    const double v_dauer = rms_bilder(c + 2L * RATE, 6L * RATE), v_ende = rms_bilder(c + 34L * RATE, 2L * RATE);
    const double w_dauer = rms_bilder(c2 + 2L * RATE, 6L * RATE);
    printf("  (6) MAIN12 erstes Spiel 2..8 s RMS %.1f, 34..36 s RMS %.1f (zu Ende); nach Neubetreten 2..8 s RMS %.1f\n",
           v_dauer, v_ende, w_dauer);
    CHECK(v_dauer > 300.0, "(6) MAIN12 klingt nicht (RMS %.1f) - Messung ungueltig", v_dauer);
    CHECK(v_ende < 0.05 * v_dauer, "(6) MAIN12 ist nach 34 s nicht zu Ende (RMS %.1f) - Messung ungueltig", v_ende);
    CHECK(w_dauer >= 0.9 * v_dauer, "(6) MAIN12 bleibt nach dem Neubetreten von ROOM4000 stumm (RMS %.1f gegen %.1f)",
          w_dauer, v_dauer);

    free(s_pcm);
    remove(cap);
    printf("%d/%d Pruefungen gruen\n%s\n", checks - fails, checks, fails ? "ROT" : "GRUEN");
    return fails ? 1 : 0;
}
