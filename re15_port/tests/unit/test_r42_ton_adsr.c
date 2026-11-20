/* test_r42_ton_adsr.c — Strang "ton", Stufe 7 (2026-11-20): Huellkurve der Geraeusch-Stimmen und Regler in Gleitkomma.
 *
 * Nutzer 19.11.2026: "Bei der PC Version klingen die Sounds irgendwie dumpfer/anders als in der PSX Version."
 * Messbericht U7: (a) der Port spielte Geraeusche ohne Huellkurve (Zensus: 12 % der Geraeusch-Schichten haben einen weichen
 * Einsatz oder fallen nach dem Anschlag ab); (b) "Bei Regler 30 gehen rund 3,5 Bit Aufloesung verloren".
 *
 * (1) re15_spu_adsr.h gegen das ORIGINAL (DuckStation): Pegel der Huellkurve, gemessen als Verhaeltnis zweier Aufnahmen
 *     derselben Probe (ROOM1130 snd0, Rolltor-Toene) - einmal mit gepatchter ADSR, einmal mit 0x80ff (Korrelation
 *     1,000000). Messung: scratchpad ton/s7/adsr_mess.py, Punkte ton/s7/adsr_punkte.py, Aufnahme ton/s7/duck_adsr/ton.wav,
 *     Savestate ton/s7/adsr_patch.sav (VagAtr Ton 7 @0x80123784, Ton 8 @0x801237a4). Zeit = Proben seit Key-On (die
 *     Key-On-Lage ist der einzige angepasste Wert), Pegel = Original, nur Stellen mit |Probe| > 5000 bzw. 12000.
 *     (1a) 0xaaff/0x5fc0 (Anstieg exponentiell, Shift 10, Step 5): 10 Stufen je Probe bis 0x6000, darueber 2,5 je Probe;
 *          psx-spx Z. 412-413 woertlich (Schritt UND Zaehler je / 4) gaebe 0,5 je Probe -> faellt hier.
 *     (1b) 0x80c6/0x0a0c (Abfall Shift 12 bis Halte-Stufe 6 = 0x3800, dann linear aufwaerts Shift 10).
 * (2) Der ECHTE Mischer (audio_pc.c, RE15_AUDIO_CAP_SYNC): ROOM1130 snd0 Satz 31 (Ton mit ADSR 0xaaff) Probe fuer Probe
 *     gegen die Rechnung Gauss * Huellkurve * Register * Hauptlautstaerke (Reihenfolge wie spu_ref.py). Ohne Huellkurve
 *     (Stand vor Stufe 7) liegt der Einsatz bis zu 32767/5000 daneben.
 * (3) Regler 30 (q = 2949, NUTZERWUNSCH Nachtrag 34 N-E2, quadratisch): RE15_AUDIO_CAP_F32 = die Gleitkomma-Werte des Geraets.
 *     Jeder Wert ist mischung * q / 2^30 mit GANZZAHLIGER 16-Bit-Mischung (keine Rundung), und die alte 16-Bit-Rechnung
 *     (RE15_AUDIO_CAP, mischung * q >> 15) ist daraus genau ableitbar. Gemeldet wird der Rundungsabstand der alten Rechnung.
 *
 * GEGENPROBE: (1) braucht re15_spu_adsr.h - auf aaea5f8c2 / 71caea668 / e46ff64d5 gibt es den Kopf nicht. (2) und (3) mit
 * dem audio_pc.c von e46ff64d5 (Stufe 6): (2) faellt (keine Huellkurve), (3) faellt (kein RE15_AUDIO_CAP_F32).
 */
#include "re15_audio.h"
#include "re15_engine.h"
#include "re15_vab.h"
#include "re15_spu_gauss.h"
#include "re15_spu_adsr.h"
#include "re15_rdt.h"
#include "re15_room.h"
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_lautstaerke_r41.h"

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
    snprintf(buf, sizeof buf, "%s/%s", RE15_ASSET_PSX_DIR, path);
    FILE *f = fopen(buf, "rb");
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
static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

/* Pegel der Huellkurve zur Probe t (seit Key-On) */
static int32_t huelle_bei(uint16_t a1, uint16_t a2, int t)
{
    re15_spu_adsr_t h;
    re15_spu_adsr_start(&h, a1, a2);
    int32_t p = 0;
    for (int i = 0; i <= t; i++) p = re15_spu_adsr_takt(&h);
    return p;
}

/* DuckStation-Messpunkte {Probe seit Key-On, Pegel} (ton/s7/adsr_punkte.py) */
static const int ANSTIEG_AAFF[][2] = {
    {510, 5103}, {1540, 15400}, {2058, 20573}, {2434, 24343}, {2685, 25143}, {3087, 26147}, {3370, 26862}, {3715, 27724},
    {4027, 28498}, {4676, 30125}, {4926, 30751}, {5176, 31374}, {5426, 32004}, {5811, 32767}, {6142, 32767} };
static const int STOPP_80C6[][2] = {
    {1091, 28453}, {1542, 26872}, {3832, 19808}, {4732, 17560}, {5687, 15413}, {6087, 14612}, {6652, 20298},
    {7143, 27170}, {7543, 32767}, {7983, 32767} };

static const char *s_cap = "r42_ton_adsr.pcm";
static const char *s_capf = "r42_ton_adsr.f32";
static long s_takt = 0;
static long laufen(int ticks) { const long t0 = s_takt; for (int i = 0; i < ticks; i++) re15_audio_tick(); s_takt += ticks; return t0; }
static void *datei(const char *p, long *n)
{
    FILE *f = fopen(p, "rb"); if (!f) { *n = 0; return NULL; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    void *b = malloc((size_t)sz + 1);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; sz = 0; }
    fclose(f); *n = sz; return b;
}

int main(void)
{
    static char e1[1024], e2[1024], e3[] = "RE15_NO_INTRO=1";
    remove(s_cap); remove(s_capf);
    snprintf(e1, sizeof e1, "RE15_AUDIO_CAP_SYNC=%s", s_cap); putenv(e1);
    snprintf(e2, sizeof e2, "RE15_AUDIO_CAP_F32=%s", s_capf); putenv(e2);
    putenv(e3);
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("== Strang ton Stufe 7: Huellkurve der Geraeusche, Regler in Gleitkomma ==\n");

    /* ---- (1) Kopf gegen das Original ----------------------------------------------------------------------- */
    {   int maxd = 0;
        for (size_t i = 0; i < sizeof ANSTIEG_AAFF / sizeof ANSTIEG_AAFF[0]; i++) {
            const int d = abs((int)huelle_bei(0xaaff, 0x5fc0, ANSTIEG_AAFF[i][0]) - ANSTIEG_AAFF[i][1]);
            if (d > maxd) maxd = d;
        }
        CHECK(maxd <= 40, "(1a) Anstieg 0xaaff gegen DuckStation: groesste Abweichung %d Pegelstufen (erlaubt 40)", maxd);
        printf("(1a) Anstieg 0xaaff: %zu Messpunkte, groesste Abweichung %d\n", sizeof ANSTIEG_AAFF / sizeof ANSTIEG_AAFF[0], maxd);
        int voll = -1; re15_spu_adsr_t h; re15_spu_adsr_start(&h, 0xaaff, 0x5fc0);
        for (int t = 0; t < 20000 && voll < 0; t++) if (re15_spu_adsr_takt(&h) >= 0x7fff) voll = t;
        CHECK(voll >= 5700 && voll <= 5760, "(1a) 0xaaff voll nach %d Proben (%.1f ms), gemessen ~5734 (130 ms)", voll, voll / 44.1);
        maxd = 0;
        for (size_t i = 0; i < sizeof STOPP_80C6 / sizeof STOPP_80C6[0]; i++) {
            const int d = abs((int)huelle_bei(0x80c6, 0x0a0c, STOPP_80C6[i][0]) - STOPP_80C6[i][1]);
            if (d > maxd) maxd = d;
        }
        CHECK(maxd <= 40, "(1b) Abfall/Halten 0x80c6/0x0a0c gegen DuckStation: groesste Abweichung %d (erlaubt 40)", maxd);
        printf("(1b) Abfall/Halten 0x80c6/0x0a0c: groesste Abweichung %d\n", maxd);
        /* sofortiger Anstieg 0x80ff/0x1fc0 (83,7 %% der Schichten): nach 3 Proben voll, danach konstant */
        CHECK(huelle_bei(0x80ff, 0x1fc0, 4) == 0x7fff && huelle_bei(0x80ff, 0x1fc0, 44100) == 0x7fff,
              "(1c) 0x80ff/0x1fc0: Probe 4 %d, nach 1 s %d (Soll 32767; ENVX 32767 im laufenden Original)",
              (int)huelle_bei(0x80ff, 0x1fc0, 4), (int)huelle_bei(0x80ff, 0x1fc0, 44100));
    }

    re15_audio_init();
    re15_lautstaerke_r41_setzen(100);

    /* ---- (2) Mischer: ROOM1130 snd0 Satz 31 ---------------------------------------------------------------- */
    int n = 0;
    uint8_t *rdt = re15_asset_read_file("STAGE1/ROOM1130.RDT", &n);
    long t_se = -1, t_reg = -1; int reg[2] = { -1, -1 }, lage[2];
    int16_t *ref = NULL; int ref_n = 0; uint16_t a1 = 0, a2 = 0;
    if (!rdt || re15_rdt_parse(rdt, (size_t)n, &g_room_rdt) != 0) { printf("FAIL: ROOM1130 nicht ladbar\n"); fails++; }
    else {
        g_room_rdt_ok = 1;
        g_scd.work_vars[0x0A] = 9;
        memset(&g_actors[0], 0, sizeof g_actors[0]);
        g_actors[0].active = 1; g_actors[0].x = -1614; g_actors[0].z = 17282;
        re15_audio_load_room_banks();
        const uint8_t *edt = rdt + rd32(rdt + 0x08), *vh = rdt + rd32(rdt + 0x0c), *vb = rdt + rd32(rdt + 0x10);
        re15_vab_t vab;
        int vags[8], tones[8];
        if (re15_vab_parse(vh, rd32(rdt + 0x10) - rd32(rdt + 0x0c), &vab) != 0 ||
            re15_edt_resolve_layers_ex(edt, &vab, 31, vags, tones, 8) != 1) { printf("FAIL: Satz 31 nicht aufloesbar\n"); fails++; }
        else {
            const re15_vab_tone_t *t = &vab.tones[tones[0]];
            a1 = t->adsr1; a2 = t->adsr2;
            CHECK(a1 == 0xaaff, "(2) ROOM1130 snd0 Satz 31: ADSR %04x/%04x (Zensus: 0xaaff/0x5fc0)", a1, a2);
            const uint32_t off = vab.samples[vags[0]].offset, sz = vab.samples[vags[0]].size;
            const size_t cap = (sz / 16) * 28;
            int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
            const int np = re15_vag_adpcm_decode(vb + off, sz, pcm, cap);
            const int schleife = re15_vag_loop_start(vb + off, sz);
            const uint32_t pitch = re15_vab_note2pitch2(t->min_note, t->pitch_shift, t->center_note, t->pitch_shift);
            ref_n = 12 * PER_TICK;
            ref = (int16_t *)calloc((size_t)ref_n * 2, sizeof(int16_t));   /* [0..ref_n) mit, [ref_n..2ref_n) ohne Huellkurve */
            (void)laufen(3);
            re15_audio_room_se_snd0(31);
            CHECK(re15_audio_se_mess(reg, lage), "(2) Messhaken: kein Register");
            t_se = laufen(14);
            (void)t_reg;
            re15_spu_zaehler_t z; re15_spu_zaehler_start(&z);
            re15_spu_adsr_t h; re15_spu_adsr_start(&h, a1, a2);
            for (int k = 0; k < ref_n; k++) {
                int32_t g = re15_spu_gauss_wert(&z, pcm);
                int32_t ge = (g * re15_spu_adsr_takt(&h)) >> 15;
                int32_t v = (ge * (reg[0] * 2)) >> 15; v = (v * 0x7ffe) >> 15;
                int32_t w = (g * (reg[0] * 2)) >> 15;  w = (w * 0x7ffe) >> 15;
                ref[k] = (int16_t)v; ref[ref_n + k] = (int16_t)w;
                if (!re15_spu_zaehler_weiter(&z, pcm, np, schleife, pitch)) break;
            }
            free(pcm);
        }
    }

    /* ---- (3) Regler 30: Rolltor-Rumpeln (Satz 10) -------------------------------------------------------- */
    re15_lautstaerke_r41_setzen(30);
    const int q = re15_lautstaerke_r41_q15();
    re15_audio_room_se_snd0(10);
    const long t_reg30 = laufen(40);
    re15_audio_shutdown();

    long nb = 0, nf = 0;
    int16_t *cap = (int16_t *)datei(s_cap, &nb);
    float *capf = (float *)datei(s_capf, &nf);
    const long ns = nb / 4;
    CHECK(cap && ns >= s_takt * PER_TICK, "Mitschnitt S16 %ld Stereo-Bilder, erwartet %ld", ns, s_takt * PER_TICK);
    CHECK(capf && nf / 8 == ns, "(3) Gleitkomma-Mitschnitt RE15_AUDIO_CAP_F32: %ld Stereo-Bilder, S16 %ld", nf / 8, ns);

    if (cap && ref && t_se >= 0 && ns >= (t_se + 14) * PER_TICK) {
        /* Einsatz: das Key-On der Pumpe liegt am Ende eines Takts, die Stimme beginnt am Anfang eines folgenden Takts ->
         * Lage s0 + off fuer off in [0, 3 Takte) suchen, dann Probe fuer Probe */
        long a = t_se * PER_TICK, e = (t_se + 14) * PER_TICK, s0 = a + 3 * PER_TICK;
        int best_off = 0, best_err = 1 << 30;
        for (int off = 0; off < 3 * PER_TICK; off++) {
            int err = 0;
            for (int k = 0; k < ref_n && s0 - off + k < e; k++) { const int d = abs(cap[(s0 - off + k) * 2] - ref[k]); if (d > err) err = d; }
            if (err < best_err) { best_err = err; best_off = off; }
        }
        if (getenv("RE15_TON_S7_DIAG") && s0 >= 0)
            for (int k = 0; k < 40; k += 4) printf("  diag k=%d cap %d ref %d (s0-t_se*PER_TICK=%ld, off %d)\n", k,
                   cap[(s0 - best_off + k) * 2], ref[k], s0 - a, best_off);
        int err_ohne = 0;
        for (int k = 0; k < ref_n && s0 >= 0 && s0 - best_off + k < e; k++) { const int d = abs(cap[(s0 - best_off + k) * 2] - ref[ref_n + k]); if (d > err_ohne) err_ohne = d; }
        CHECK(best_err <= 1, "(2) Mischer Satz 31 (ADSR %04x, Register %d/%d) gegen Gauss*Huellkurve*Register: groesste Abweichung %d "
              "ueber %d Proben (erlaubt 1)", a1, reg[0], reg[1], best_err, ref_n);
        printf("(2) Satz 31: Register %d/%d, Abweichung mit Huellkurve %d, OHNE Huellkurve waere sie %d\n", reg[0], reg[1], best_err, err_ohne);
        CHECK(err_ohne > 1000, "(2) Gegenrechnung ohne Huellkurve weicht nur %d ab - der Einsatz traegt die Huellkurve nicht", err_ohne);
    }

    if (cap && capf && nf / 8 == ns && ns >= (t_reg30 + 40) * PER_TICK) {
        long falsch = 0, inkons = 0, n_ok = 0; double fehler_alt = 0.0, signal = 0.0;
        const double kq = (double)q / 1073741824.0;
        for (long i = t_reg30 * PER_TICK * 2; i < (t_reg30 + 40) * PER_TICK * 2; i++) {
            const double m = (double)capf[i] / kq;
            const long mr = lround(m);
            if (fabs(m - (double)mr) > 1e-3 * (fabs(m) + 1.0) || mr < -32768 || mr > 32767) { falsch++; continue; }
            if ((int16_t)((mr * q) >> 15) != cap[i]) inkons++;
            const double genau = (double)mr * q / 32768.0, alt = (double)cap[i];
            fehler_alt += (genau - alt) * (genau - alt); signal += genau * genau; n_ok++;
        }
        CHECK(q == 2949, "(3) Regler 30 -> q %d (Soll 30*30*0x8000/10000 = 2949)", q);
        CHECK(falsch == 0 && n_ok > 0, "(3) %ld von %ld Gleitkomma-Werten sind nicht mischung*q/2^30 mit ganzzahliger 16-Bit-Mischung", falsch, n_ok + falsch);
        CHECK(inkons == 0, "(3) %ld Werte: alte 16-Bit-Rechnung passt nicht zur Gleitkomma-Mischung", inkons);
        printf("(3) Regler 30: %ld Werte exakt; alte 16-Bit-Rechnung: Abstand Signal/Rundung %.1f dB, Gleitkomma: keine Rundung\n",
               n_ok, 10.0 * log10(signal / (fehler_alt + 1e-30)));
    }

    free(cap); free(capf); free(ref); free(rdt);
    remove(s_cap); remove(s_capf);
    printf("%d/%d Pruefungen bestanden\n", checks - fails, checks);
    return fails ? 1 : 0;
}
