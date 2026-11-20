/* test_r42_ton_gauss.c — Strang "ton", Stufe 5 (2026-11-20): Pitch-Zaehler und 4-Punkt-Gauss-Glaettung der SPU fuer
 * Geraeusch- und Musik-Stimmen.
 *
 * Nutzer 19.11.2026: "Bei der PC Version klingen die Sounds irgendwie dumpfer/anders als in der PSX Version",
 * "Zum Beispiel beim öffnen des Rolltors im Flur. Klingt ganz leicht anders."
 * Messbericht U5: der Port nahm bei Geraeuschen den naechsten Wert (Treppenstufen), bei Musik eine gerade Linie; die SPU
 * glaettet mit der Gauss-Tabelle (psx-spx soundprocessingunitspu.md "Pitch Counter" Z. 187-201, "4-Point Gaussian
 * Interpolation" Z. 215-290).
 *
 * Der Riegel rechnet gegen die Bankdaten und gegen die psx-spx-Formel, die er SELBST aus der Hardware-Beschreibung liest
 * (Tabelle aus der Datei RE15_PSXSPX_SPU_MD, nicht aus dem Port-Kopf re15_spu_gauss.h):
 *   (1) Rolltor-Rumpeln ROOM1130 snd0 Satz 10 (Ton 7, VAG 4, Pitch 0x1a5 = note2pitch2(66, 105, 107, 105), Schleife ab
 *       Probe 3556) am ECHTEN Mischer (audio_pc.c, RE15_AUDIO_CAP_SYNC, Regler 100), 190 Spielbilder = 6,3 s, also ueber
 *       zwei Schleifen-Spruenge hinweg: jeder Abtastwert = Gauss(psx-spx) * Register 16383 (Q15 0x7ffe) * Haupt-
 *       lautstaerke 0x7ffe, ohne einen einzigen abweichenden Wert.
 *   (2) Rolltor-Stopp ROOM1130 Satz 11 (Ton 8, VAG 5, Pitch 0x196, Einmal-Probe): ebenso Wert fuer Wert, und nach der
 *       letzten Probe ist die Stimme sofort stumm (psx-spx Z. 131 "Force Release and set ADSR Level to Zero").
 *   (3) [RE15_TON_S5_HAKEN] die 512 Werte des Port-Kopfs re15_spu_gauss.h = die Tabelle der Hardware-Beschreibung.
 *   (4) [RE15_TON_S5_HAKEN] Musik-Stimme durch den echten ss_mix-Pfad (Messhaken re15_ton_s5_musik_stimme, Huellkurve
 *       fest auf 0x7fff): Rumpel-Probe mit Schleife bei Pitch 0x1000 (Rotor-Rate, Messbericht), 0x1a5, 0x2a3b und 0x5000
 *       (psx-spx: "IF Step>3FFFh then Step=4000h"), Stopp-Probe 0x196 ohne Schleife - Wert fuer Wert gleich der Formel,
 *       Ende am selben Abtastwert.
 *   (5) Rechenzeit je Spielbild: ROOM1170 Musik MAIN32 + Rotor SUB_15 + Rolltor-Rumpeln, 300 Spielbilder; gemessen und
 *       gedruckt. Grenze 10 ms je Spielbild (30 % von 33,3 ms): der Testbau ist ohne Optimierung (-O0) und laeuft neben
 *       anderen Bau-Straengen - gemessen 2,2 bis 4,1 ms (-O0) bzw. 0,6 bis 1,2 ms (-O2) unter Last; vor Stufe 5 ebenso
 *       (2,0 bis 3,7 / 0,6 bis 1,2 ms; Bestwert -O0 2028 gegen 2216 us, -O2 607 gegen 597 us). Die Grenze faengt nur einen
 *       groben Ausreisser (z. B. eine Schleife je Probe statt je Ueberlauf).
 * Nebenher gedruckt: wie weit der Mitschnitt von der alten Port-Rechnung "naechster Wert" entfernt ist (zeigt, dass (1)
 * zwischen alt und neu unterscheidet).
 *
 * GEGENPROBE: derselbe Quelltext ohne -DRE15_TON_S5_HAKEN mit dem audio_pc.c von r42/story 71caea668 (dort gibt es weder
 * den Kopf noch den Messhaken) faellt in (1) und (2): der Mitschnitt ist dort die Treppenstufe "naechster Wert".
 */
#include "re15_audio.h"
#include "re15_engine.h"
#include "re15_vab.h"
#include "re15_rdt.h"
#include "re15_room.h"
#include "re15_actor.h"
#include "re15_scd.h"
#ifdef RE15_TON_S5_HAKEN
#include "re15_spu_gauss.h"
#include "re15_spu_adsr.h"   /* Strang ton Stufe 7: Huellkurve der Geraeusch-Stimmen */
#endif
#define SDL_MAIN_HANDLED                 /* eigenes main(), SDL nur fuer die Uhr (SDL_GetPerformanceCounter) */
#include <SDL.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_PSXSPX_SPU_MD
#define RE15_PSXSPX_SPU_MD "soundprocessingunitspu.md"
#endif
#define RATE      44100
#define PER_TICK  (RATE / 30)

/* wie test_r42_ton_mischer.c: das ECHTE audio_pc.c, diese Symbole kommen sonst aus main.c */
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

static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

/* ---- Gauss-Tabelle aus der Hardware-Beschreibung lesen (psx-spx Z. 226-290: 64 Zeilen zu 8 Werten "-001h,...,0001h ;"
 *      nach "The Gauss table contains the following values", vor "The PSX table is a bit different") ---------------- */
static int s_g[512];
static int tabelle_lesen(void)
{
    FILE *f = fopen(RE15_PSXSPX_SPU_MD, "rb");
    if (!f) return 0;
    static char txt[1 << 20];
    const size_t n = fread(txt, 1, sizeof txt - 1, f);
    fclose(f);
    txt[n] = 0;
    const char *a = strstr(txt, "The Gauss table contains the following values");
    const char *b = a ? strstr(a, "The PSX table is a bit different") : NULL;
    if (!a || !b) return 0;
    int k = 0;
    const char *p = a;
    while (p < b && k <= 512) {
        const char *zende = strchr(p, '\n');
        if (!zende || zende > b) zende = b;
        const char *semi = memchr(p, ';', (size_t)(zende - p));
        const char *stop = semi ? semi : zende;          /* Kommentarspalte ";\ entry 000h..07Fh" nicht mitlesen */
        for (const char *q = p; q < stop; q++) {
            int neg = 0;
            const char *r = q;
            if (*r == '-') { neg = 1; r++; }
            int v = 0, dig = 0;
            while (r < stop && ((*r >= '0' && *r <= '9') || (*r >= 'A' && *r <= 'F') || (*r >= 'a' && *r <= 'f'))) {
                v = v * 16 + ((*r <= '9') ? *r - '0' : ((*r | 0x20) - 'a' + 10)); r++; dig++;
            }
            if (dig >= 3 && dig <= 4 && r < stop && *r == 'h' && (q == p || q[-1] == ' ' || q[-1] == ',' || neg)) {
                if (k < 512) s_g[k] = neg ? -v : v;
                k++;
                q = r;
            }
        }
        p = zende + 1;
    }
    return k == 512;
}

/* ---- Referenz: eine SPU-Stimme nach psx-spx, unabhaengig vom Port-Kopf geschrieben ------------------------------
 *  "Pitch Counter": Step = VxPitch; IF Step>3FFFh then Step=4000h; Counter = Counter + Step; Bit12+ = Probe,
 *                   Bit4..11 = Index i.
 *  "4-Point Gaussian Interpolation": die vier zuletzt geladenen Proben new, old, older, oldest.
 *  Probe hinter dem Ende: Schleife -> Wiederhol-Adresse, sonst Stimme aus (Pegel 0). */
typedef struct {
    const int16_t *pcm; int n, loop; uint32_t step, counter; int32_t h[4]; int idx, aus;
} ref_t;
static void ref_start(ref_t *r, const int16_t *pcm, int n, int loop, uint32_t pitch)
{
    memset(r, 0, sizeof *r);
    r->pcm = pcm; r->n = n; r->loop = loop;
    r->step = pitch > 0x3fff ? 0x4000 : pitch;
    r->h[3] = pcm[0]; r->idx = 1;                         /* hist = [0, 0, 0, pcm[0]] (Referenzmodell spu_ref.py) */
}
/* Rueckgabe: Gauss-Wert; *an = 0, sobald die Stimme aus ist */
static int32_t ref_wert(ref_t *r, int *an)
{
    if (r->aus) { *an = 0; return 0; }
    *an = 1;
    const int i = (int)((r->counter >> 4) & 0xff);
    const int32_t out = ((s_g[0x0ff - i] * r->h[0]) >> 15) + ((s_g[0x1ff - i] * r->h[1]) >> 15)
                      + ((s_g[0x100 + i] * r->h[2]) >> 15) + ((s_g[0x000 + i] * r->h[3]) >> 15);
    r->counter += r->step;
    while (r->counter >= 0x1000) {
        r->counter -= 0x1000;
        if (r->idx >= r->n) {
            if (r->loop >= 0) r->idx = r->loop;
            else { r->aus = 1; break; }
        }
        r->h[0] = r->h[1]; r->h[1] = r->h[2]; r->h[2] = r->h[3]; r->h[3] = r->pcm[r->idx++];
    }
    return out;
}

/* Probe vag0 einer RDT-Bank snd0 dekodieren (RDT-Kopf +0x0c VH, +0x10 VB, +0x14 Ende) */
static int16_t *probe_von(const uint8_t *d, int dn, int vag0, int *n_out, int *loop_out)
{
    static re15_vab_t vab;
    const uint32_t vh = rd32(d + 0x0c), vb = rd32(d + 0x10), vb_end = rd32(d + 0x14) > vb ? rd32(d + 0x14) : (uint32_t)dn;
    if (re15_vab_parse(d + vh, vb - vh, &vab) != 0 || vag0 >= vab.vag_count) return NULL;
    const uint32_t off = vab.samples[vag0].offset, sz = vab.samples[vag0].size;
    if (vb + off + sz > vb_end) return NULL;
    const size_t cap = (sz / 16) * 28;
    int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
    *n_out = re15_vag_adpcm_decode(d + vb + off, sz, pcm, cap);
    /* Schleifenpunkt selbst aus den Flag-Bytes (psx-spx Z. 128-132: Byte 1 je 16-Byte-Block, Bit2 Loop Start, Bit0 Loop
     * End, Bit1 Loop Repeat) - nicht ueber re15_vag_loop_start des Ports */
    *loop_out = -1;
    {   int start = 0;
        for (uint32_t b = 0; b + 16 <= sz; b += 16) {
            const uint8_t fl = d[vb + off + b + 1];
            if (fl & 4) start = (int)(b / 16);
            if (fl & 1) { if (fl & 2) *loop_out = start * 28; break; }
        } }
    return pcm;
}

static void raum_laden(const char *rel, uint8_t **rdt, int *rn)
{
    int n = 0;
    uint8_t *d = re15_asset_read_file(rel, &n);
    if (!d || re15_rdt_parse(d, (size_t)n, &g_room_rdt) != 0) { printf("FAIL: %s nicht ladbar\n", rel); fails++; return; }
    g_room_rdt_ok = 1;
    g_scd.work_vars[0x0A] = 9;
    memset(&g_actors[0], 0, sizeof g_actors[0]);
    g_actors[0].active = 1; g_actors[0].x = -1614; g_actors[0].z = 17282;
    re15_audio_load_room_banks();                          /* der RDT-Puffer bleibt bestehen (Baenke zeigen hinein) */
    *rdt = d; *rn = n;
}

static const char *s_cap = "r42_ton_gauss.pcm";
static long s_takt = 0;
static long laufen(int ticks) { const long t0 = s_takt; for (int i = 0; i < ticks; i++) re15_audio_tick(); s_takt += ticks; return t0; }
static int16_t *s_pcm = NULL; static long s_pcm_n = 0;

/* Geraeusch-Stimme im Mitschnitt gegen die Referenz: Beginn im Fenster [takt0, takt0+3) suchen (kleinste Abweichung
 * ueber die ersten 4000 Werte), dann ueber n_aus Werte zaehlen, wie viele Abtastwerte (L und R) abweichen.
 * reg_q15 = Stimmen-Register * 2; danach Hauptlautstaerke 0x7ffe (Mischer audio_callback). */
typedef struct { long start; long abw; int max_abw; long n; double alt_rms_db; int ende_ok; } vgl_t;
static int32_t stimme(int32_t g, int reg_q15) { int32_t v = (g * reg_q15) >> 15; return (v * 0x7ffe) >> 15; }
static vgl_t vergleich(long takt0, const int16_t *pcm, int n, int loop, uint32_t pitch, int reg_q15, long n_aus)
{
    vgl_t e; memset(&e, 0, sizeof e); e.start = -1; e.max_abw = 99999;
    int32_t *soll = (int32_t *)calloc((size_t)n_aus, sizeof(int32_t));
    int32_t *alt = (int32_t *)calloc((size_t)n_aus, sizeof(int32_t));
    ref_t r; ref_start(&r, pcm, n, loop, pitch);
    long n_an = n_aus;
    /* Strang ton Stufe 7: seit der Mischer die Huellkurve rechnet (re15_spu_adsr.h), gehoert sie in die Formel. Beide Rolltor-
     * Toene (ROOM1130 snd0 Ton 7 / 8) tragen ADSR 0x80ff/0x1fc0 (VagAtr+0x10/+0x12; im Original-RAM @0x80123794 / @0x801237b4
     * gelesen, ton/s7/adsr_patch_ort.py): Anstieg in 4 Proben, dann Halten bei 32767 -> Probe * 32767 >> 15. */
    re15_spu_adsr_t hk; re15_spu_adsr_start(&hk, 0x80ff, 0x1fc0);
    for (long k = 0; k < n_aus; k++) { int an; int32_t g = ref_wert(&r, &an); if (!an && n_an == n_aus) n_an = k;
        g = (g * re15_spu_adsr_takt(&hk)) >> 15; soll[k] = an ? stimme(g, reg_q15) : 0; }
    /* alte Port-Rechnung "naechster Wert" (Messbericht U5; Port bis 1731b889c): pcm[pos], pos += Q16-Schritt pitch << 4 */
    {   uint32_t fr = 0; long pos = 0;
        for (long k = 0; k < n_aus; k++) {
            if (pos >= n) { if (loop >= 0) pos = loop + (pos - n); else break; }
            alt[k] = stimme(pcm[pos], reg_q15);
            fr += pitch << 4; pos += fr >> 16; fr &= 0xffff;
        } }
    long best = -1; double best_d = 1e300;
    const long kmax = n_aus < 4000 ? n_aus : 4000;
    for (long o = takt0 * PER_TICK; o < (takt0 + 3) * PER_TICK; o++) {
        if (o + n_aus > s_pcm_n) break;
        double d = 0;
        for (long k = 0; k < kmax; k++) d += fabs((double)s_pcm[(o + k) * 2] - soll[k]);
        if (d < best_d) { best_d = d; best = o; }
    }
    e.start = best; e.n = n_aus; e.max_abw = 0;
    if (best < 0) { free(soll); free(alt); e.max_abw = 99999; return e; }
    double ad = 0, as = 0;
    for (long k = 0; k < n_aus; k++) {
        for (int c = 0; c < 2; c++) {
            const int d = abs(s_pcm[(best + k) * 2 + c] - soll[k]);
            if (d) e.abw++;
            if (d > e.max_abw) e.max_abw = d;
        }
        ad += ((double)s_pcm[(best + k) * 2] - alt[k]) * ((double)s_pcm[(best + k) * 2] - alt[k]);
        as += (double)soll[k] * soll[k];
    }
    e.alt_rms_db = 10.0 * log10((ad + 1e-9) / (as + 1e-9));
    /* Ende: nach der letzten Referenz-Probe still (nur sinnvoll fuer Einmal-Proben) */
    e.ende_ok = 1;
    if (n_an < n_aus) for (long k = n_an; k < n_aus; k++) if (s_pcm[(best + k) * 2] || s_pcm[(best + k) * 2 + 1]) { e.ende_ok = 0; break; }
    free(soll); free(alt);
    return e;
}

#ifdef RE15_TON_S5_HAKEN
int re15_ton_s5_musik_stimme(const int16_t *pcm, int len, int loop_start, uint32_t pitch, int16_t *out, int frames, int neu);
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
    printf("== Strang ton Stufe 5: Pitch-Zaehler und 4-Punkt-Gauss der SPU ==\n");

    const int tab_ok = tabelle_lesen();
    CHECK(tab_ok, "Gauss-Tabelle aus %s nicht lesbar (512 Werte erwartet)", RE15_PSXSPX_SPU_MD);
    if (!tab_ok) { printf("ROT: ohne Tabelle keine Referenz\n"); return 1; }
    {   int sum_min = 1 << 30, sum_max = -(1 << 30);           /* psx-spx Z. 296-300: je i Summe 7F7Fh..7F81h */
        for (int i = 0; i < 256; i++) {
            const int su = s_g[i] + s_g[0xff - i] + s_g[0x100 + i] + s_g[0x1ff - i];
            if (su < sum_min) sum_min = su;
            if (su > sum_max) sum_max = su;
        }
        printf("  Tabelle gelesen: g[0]=%d g[0x100]=0x%x g[0x1ff]=0x%x, Summen je Index 0x%x..0x%x\n", s_g[0], s_g[0x100], s_g[0x1ff], sum_min, sum_max);
        CHECK(sum_min >= 0x7f7f && sum_max <= 0x7f81, "Tabelle: Summen 0x%x..0x%x, psx-spx nennt 7F7Fh..7F81h", sum_min, sum_max);
    }

#ifdef RE15_TON_S5_HAKEN
    {   int abw = 0;
        for (int i = 0; i < 512; i++) if (re15_spu_gauss[i] != s_g[i]) { if (abw < 5) printf("  (3) Index 0x%03x: Port %d, psx-spx %d\n", i, re15_spu_gauss[i], s_g[i]); abw++; }
        CHECK(abw == 0, "(3) re15_spu_gauss.h weicht in %d von 512 Werten von der psx-spx-Tabelle ab", abw);
    }
#endif

    re15_audio_init();
    uint8_t *rdt = NULL; int rn = 0;
    raum_laden("STAGE1/ROOM1130.RDT", &rdt, &rn);
    enum { N_RUMPELN = 190, N_STOPP = 45 };
    (void)laufen(4);
    re15_audio_room_se_snd0(10); const long t_rumpeln = laufen(N_RUMPELN);
    re15_audio_room_se_snd0(11); const long t_stopp = laufen(N_STOPP);

    /* (5) Rechenzeit: Musik + Rotor + Rumpeln */
    re15_audio_start_room_bgm(0, 0x17);                          /* ROOM1170: MAIN32 + SUB_15 */
    re15_audio_seq_ctl(0, 1); re15_audio_seq_ctl(1, 1);
    re15_audio_room_se_snd0(10);
    (void)laufen(30);
    const Uint64 c0 = SDL_GetPerformanceCounter();
    (void)laufen(300);
    const Uint64 c1 = SDL_GetPerformanceCounter();
    const double us_je_bild = (double)(c1 - c0) * 1e6 / (double)SDL_GetPerformanceFrequency() / 300.0;
    re15_audio_shutdown();

    {   FILE *f = fopen(s_cap, "rb");
        if (f) {
            fseek(f, 0, SEEK_END); const long sz = ftell(f); fseek(f, 0, SEEK_SET);
            s_pcm = (int16_t *)malloc((size_t)sz);
            if (s_pcm && fread(s_pcm, 1, (size_t)sz, f) == (size_t)sz) s_pcm_n = sz / 4;
            fclose(f);
        }
        CHECK(s_pcm_n >= s_takt * PER_TICK, "Mitschnitt %s: %ld Stereo-Bilder, erwartet %ld", s_cap, s_pcm_n, s_takt * PER_TICK); }

    if (rdt) {
        int n4 = 0, l4 = -1, n5 = 0, l5 = -1;
        int16_t *p4 = probe_von(rdt, rn, 3, &n4, &l4);           /* Ton 7 -> VAG 4 */
        int16_t *p5 = probe_von(rdt, rn, 4, &n5, &l5);           /* Ton 8 -> VAG 5 */
        CHECK(p4 && p5 && l4 == 127 * 28 && l5 == -1, "Proben ROOM1130 VAG 4/5: Laenge %d/%d, Schleife %d/%d (Soll 3556 / -1)", n4, n5, l4, l5);
        if (p4 && p5) {
            /* (1) Register 16383 (SpuVmKeyOnNow @0x80055d10, im Original gemessen) -> Q15 32766; bis 3 Takte nach dem Ruf */
            const long n1 = (long)(N_RUMPELN - 4) * PER_TICK;
            vgl_t e = vergleich(t_rumpeln, p4, n4, l4, 0x1a5, 32766, n1);
            printf("  (1) Rolltor-Rumpeln: Beginn Probe %ld (Takt %ld + %ld), %ld Werte (%.2f s, %d Schleifen-Spruenge), abweichend %ld, "
                   "groesste Abweichung %d; Rest der alten Rechnung 'naechster Wert' gegen die Formel %.1f dB (Energie Rest / Formel)\n",
                   e.start, t_rumpeln, e.start - t_rumpeln * PER_TICK, e.n, (double)e.n / RATE,
                   (int)(((long long)e.n * 0x1a5 / 0x1000 - n4) > 0 ? 1 + (((long long)e.n * 0x1a5 / 0x1000 - n4) / (n4 - l4)) : 0),
                   e.abw, e.max_abw, e.alt_rms_db);
            CHECK(e.start >= 0 && e.abw == 0, "(1) Rolltor-Rumpeln ROOM1130 Satz 10 (Pitch 0x1a5, Schleife ab 3556): %ld von %ld Werten "
                  "weichen von Gauss(psx-spx) * Register 16383 ab (groesste Abweichung %d)", e.abw, 2 * e.n, e.max_abw);
            /* (2) Stopp: Register 10268 (Ton-vol 113, test_r42_ton_mischer (2)) -> Q15 20536 */
            const long n2 = (long)(N_STOPP - 4) * PER_TICK;
            e = vergleich(t_stopp, p5, n5, -1, 0x196, 20536, n2);
            printf("  (2) Rolltor-Stopp: Beginn Probe %ld, abweichend %ld von %ld, groesste Abweichung %d, nach der letzten Probe still: %s; "
                   "Rest der alten Rechnung %.1f dB\n", e.start, e.abw, 2 * e.n, e.max_abw, e.ende_ok ? "ja" : "NEIN", e.alt_rms_db);
            CHECK(e.start >= 0 && e.abw == 0 && e.ende_ok, "(2) Rolltor-Stopp ROOM1130 Satz 11 (Pitch 0x196, Einmal-Probe): %ld abweichende Werte "
                  "(groesste %d), Ende still: %d", e.abw, e.max_abw, e.ende_ok);

#ifdef RE15_TON_S5_HAKEN
            /* (4) Musik-Stimme durch ss_mix: Huellkurve 0x7fff, Ebene und Stimme 0x7ffe (Reihenfolge wie ss_mix) */
            const struct { const int16_t *pcm; int n, loop; uint32_t pitch; const char *was; } fall[] = {
                { p4, n4, l4, 0x1000, "Rumpel-Probe, Pitch 0x1000 (Rotor-Rate)" },
                { p4, n4, l4, 0x01a5, "Rumpel-Probe, Pitch 0x1a5" },
                { p4, n4, l4, 0x2a3b, "Rumpel-Probe, Pitch 0x2a3b" },
                { p4, n4, l4, 0x5000, "Rumpel-Probe, Pitch 0x5000 (-> 0x4000)" },
                { p5, n5, -1, 0x0196, "Stopp-Probe, Pitch 0x196, Einmal-Probe" },
            };
            static int16_t blk[PER_TICK * 2];
            for (size_t fi = 0; fi < sizeof fall / sizeof fall[0]; fi++) {
                ref_t r; ref_start(&r, fall[fi].pcm, fall[fi].n, fall[fi].loop, fall[fi].pitch);
                long abw = 0, k = 0, ende_port = -1, ende_ref = -1; int maxd = 0;
                for (int b = 0; b < 90; b++) {
                    const int an = re15_ton_s5_musik_stimme(fall[fi].pcm, fall[fi].n, fall[fi].loop, fall[fi].pitch, blk, PER_TICK, b == 0);
                    for (int j = 0; j < PER_TICK; j++, k++) {
                        int ra; const int32_t g = ref_wert(&r, &ra);
                        if (!ra && ende_ref < 0) ende_ref = k;
                        int32_t v = (g * 0x7fff) >> 15;          /* Huellkurve */
                        v = (v * 0x7ffe) >> 15;                  /* Ebene (mvol) */
                        v = (v * 0x7ffe) >> 15;                  /* Stimme */
                        if (!ra) v = 0;
                        for (int c = 0; c < 2; c++) { const int d = abs(blk[j * 2 + c] - v); if (d) abw++; if (d > maxd) maxd = d; }
                    }
                    if (!an && ende_port < 0) ende_port = k;     /* Ende des Spielbilds, in dem die Stimme verstummt ist */
                }
                const int ende_gleich = (ende_ref < 0) ? (ende_port < 0) : (ende_port >= ende_ref && ende_port - PER_TICK < ende_ref);
                printf("  (4) Musik-Stimme %-44s: %ld Werte, abweichend %ld, groesste Abweichung %d, Ende Formel %ld / Port-Bild bis %ld\n",
                       fall[fi].was, k, abw, maxd, ende_ref, ende_port);
                CHECK(abw == 0 && ende_gleich, "(4) Musik-Stimme %s: %ld abweichende Werte (groesste %d), Ende Formel %ld / Port %ld",
                      fall[fi].was, abw, maxd, ende_ref, ende_port);
            }
#endif
        }
        free(p4); free(p5);
    }

    printf("  (5) Rechenzeit: %.0f us je Spielbild (Musik MAIN32 + Rotor + Rumpeln, Mitschnitt mitgezaehlt), Grenze 10000 us\n", us_je_bild);
    CHECK(us_je_bild < 10000.0, "(5) Rechenzeit %.0f us je Spielbild, Grenze 10000 us", us_je_bild);

    remove(s_cap);
    free(s_pcm);
    free(rdt);
    printf("%s: %d/%d Pruefungen bestanden\n", fails ? "ROT" : "GRUEN", checks - fails, checks);
    return fails ? 1 : 0;
}
