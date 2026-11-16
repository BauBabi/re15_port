/* test_pipe_ton_r41.c — Runde 41 Spur "pipe_ton" (Nutzer 2026-11-16): die Pipe soll nicht wie ein Messer klingen, sondern
 * "ein dumpfes metallenes Aufschlag Geraeusch haben. So wie bei Birkin". Belege: include/re15_pipe_r41.h.
 *
 *   (1) WAHL: Pipe (Waffe 2) + ARMS-Satz 8 (Treffer @0x800123D4 / Ziehen @0x800355f8) -> Birkin-Satz 7 (@0x801177dc),
 *       Pipe + ARMS-Satz 5 (Schwung @0x8003537c) -> Birkin-Satz 4 (@0x80117878); Messer (Waffe 1) und alle anderen
 *       Waffen/Saetze -> -1 (unveraendert aus der ARMS-Bank).
 *   (2) KLANG: der zusammengesetzte Satz loest ueber die Birkin-Bank STAGE3/ROOM3070.RDT snd1 auf GENAU die Welle auf,
 *       die RE2-G1 als Rohr-Treffer (ENEMSE Bank 20 se 6, VBS @0x16d010, 1808 B) bzw. beim Angriffsbeginn (se 3,
 *       VBS @0x16b030, 4128 B) spielt — Bytevergleich gegen shared_assets/RE2/ENEMSE.VBS (unabhaengige Quelle).
 *       Gegenprobe: die Messer-Wellen (ARMS01 Satz 8 / 5) sind andere Bytes.
 *   (3) KANAL: Stimme und Prioritaet des zusammengesetzten Satzes = Messer-Satz (ARMS01.EDH @0x20 `00 00 43 11`,
 *       @0x14 `00 00 13 11`): Stimme 1, Prio-Nibble 3; Ton/Programm = Birkin-Satz (`00 00 82 15` / `00 00 51 16`).
 *   (4) KLANGBILD: die dekodierte Treffer-Welle ist tief (Nulldurchgaenge/s deutlich unter der Messer-Welle) und
 *       kurz (< 0,4 s) — "dumpf" gegen das helle Messer.
 */
#include "re15_pipe_r41.h"
#include "re15_vab.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

static re15_vab_t s_vab_b, s_vab_m;

/* Welle (VAG-Bytes) des Satzes `rec` in der Bank (edt-Zeiger zeigt auf den Satz selbst, se_id 0). */
static const uint8_t *welle(const uint8_t *rec, const re15_vab_t *vab, const uint8_t *vb, uint32_t *len)
{
    int vags[4], tones[4];
    int n = re15_edt_resolve_layers_ex(rec, vab, 0, vags, tones, 4);
    if (n != 1) return NULL;
    *len = vab->samples[vags[0]].size;
    return vb + vab->samples[vags[0]].offset;
}

/* Nulldurchgaenge je Sekunde der dekodierten Welle bei der Abspielrate des Tons (note == center -> 44100 Hz). */
static double nulldurchgaenge(const uint8_t *vag, uint32_t len, const re15_vab_tone_t *t, double *dauer)
{
    size_t cap = (len / 16) * 28;
    int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
    int n = re15_vag_adpcm_decode(vag, len, pcm, cap);
    double rate = 44100.0;
    { int d = (int)t->min_note - (int)t->center_note; double f = 1.0;
      while (d < 0) { f /= 1.0594630943592953; d++; } while (d > 0) { f *= 1.0594630943592953; d--; } rate *= f; }
    int z = 0;
    for (int i = 1; i < n; i++) if ((pcm[i - 1] < 0) != (pcm[i] < 0)) z++;
    free(pcm);
    *dauer = n / rate;
    return n ? z / (n / rate) : 0.0;
}

int main(void)
{
    /* ---- (1) WAHL ---- */
    CHECK(re15_pipe_r41_rohr_satz(2, 8) == 7, "(1) Pipe Treffer/Ziehen (ARMS 8) -> %d statt Birkin-Satz 7", re15_pipe_r41_rohr_satz(2, 8));
    CHECK(re15_pipe_r41_rohr_satz(2, 5) == 4, "(1) Pipe Schwung (ARMS 5) -> %d statt Birkin-Satz 4", re15_pipe_r41_rohr_satz(2, 5));
    CHECK(re15_pipe_r41_rohr_satz(1, 8) == -1 && re15_pipe_r41_rohr_satz(1, 5) == -1,
          "(1) Messer darf NICHT umgeleitet werden (Messer bleibt unveraendert)");
    for (int w = 0; w < 22; w++)
        for (int s = 0; s < 10; s++) {
            int r = re15_pipe_r41_rohr_satz(w, s);
            int soll = (w == 2 && s == 8) ? 7 : (w == 2 && s == 5) ? 4 : -1;
            CHECK(r == soll, "(1) Waffe %d Satz %d -> %d statt %d", w, s, r, soll);
        }

    /* ---- Baenke lesen ---- */
    char p[600];
    size_t n_rdt = 0, n_edh = 0, n_vb = 0, n_vbs = 0;
    snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, RE15_PIPE_R41_ROHR_RDT);
    uint8_t *rdt = slurp(p, &n_rdt);
    snprintf(p, sizeof p, "%s/SOUND/ARMS01.EDH", RE15_ASSET_PSX_DIR);
    uint8_t *edh = slurp(p, &n_edh);
    snprintf(p, sizeof p, "%s/SOUND/ARMS01.VB", RE15_ASSET_PSX_DIR);
    uint8_t *avb = slurp(p, &n_vb);
    snprintf(p, sizeof p, "%s/../RE2/ENEMSE.VBS", RE15_ASSET_PSX_DIR);
    uint8_t *vbs = slurp(p, &n_vbs);
    if (!rdt || !edh || !avb || !vbs) { printf("FAIL: Asset fehlt (rdt %p edh %p vb %p vbs %p)\n",
                                               (void *)rdt, (void *)edh, (void *)avb, (void *)vbs); return 1; }
    const uint32_t e = rd32(rdt + 0x14), h = rd32(rdt + 0x18), v = rd32(rdt + 0x1c);   /* snd1-Kopf (rdt_common.c) */
    CHECK(re15_vab_parse(rdt + h, v - h, &s_vab_b) == 0, "(2) Birkin-VH nicht lesbar");
    const uint32_t pb = rd32(edh + n_edh - 8);                                           /* pBAV @ u32[size-8] */
    CHECK(re15_vab_parse(edh + pb, n_edh - pb, &s_vab_m) == 0, "(2) ARMS01-VH nicht lesbar");

    static const uint8_t b7[4] = { 0x00, 0x00, 0x82, 0x15 }, b4[4] = { 0x00, 0x00, 0x51, 0x16 };
    static const uint8_t m8[4] = { 0x00, 0x00, 0x43, 0x11 }, m5[4] = { 0x00, 0x00, 0x13, 0x11 };
    CHECK(memcmp(rdt + e + 7 * 4, b7, 4) == 0 && memcmp(rdt + e + 4 * 4, b4, 4) == 0,
          "(2) ROOM3070 snd1 Satz 7/4 nicht `00 00 82 15` / `00 00 51 16`");
    CHECK(memcmp(edh + 8 * 4, m8, 4) == 0 && memcmp(edh + 5 * 4, m5, 4) == 0,
          "(3) ARMS01 Satz 8/5 nicht `00 00 43 11` / `00 00 13 11`");

    for (int k = 0; k < 2; k++) {
        const int arms = k ? RE15_PIPE_R41_ARMS_SCHWUNG : RE15_PIPE_R41_ARMS_TREFFER;
        const int satz = re15_pipe_r41_rohr_satz(2, arms);
        const char *was = k ? "Schwung" : "Treffer";
        const uint32_t re2_off = k ? 0x16b030u : 0x16d010u, re2_len = k ? 4128u : 1808u;   /* G1 se 3 / se 6 */
        uint8_t rec[4];
        re15_pipe_r41_rohr_rec(rdt + e + (uint32_t)satz * 4, edh + (uint32_t)arms * 4, rec);
        re15_edt_rec_t d;
        re15_edt_decode(rec, 0, &d);
        /* ---- (3) KANAL ---- */
        CHECK(d.voice == 1 && d.prio_nib == 3, "(3) %s: Stimme %d Prio %d statt Messer-Kanal 1/3", was, d.voice, d.prio_nib);
        CHECK(d.prog == 0 && d.tone == (k ? 5 : 8) && d.extra == 0, "(3) %s: Prog %d Ton %d +%d statt Birkin 0/%d/+0",
              was, d.prog, d.tone, d.extra, k ? 5 : 8);
        /* ---- (2) KLANG ---- */
        uint32_t lb = 0, lm = 0;
        const uint8_t *wb = welle(rec, &s_vab_b, rdt + v, &lb);
        const uint8_t *wm = welle(edh + (uint32_t)arms * 4, &s_vab_m, avb, &lm);
        CHECK(wb && lb == re2_len && re2_off + re2_len <= n_vbs && memcmp(wb, vbs + re2_off, re2_len) == 0,
              "(2) %s: Welle (%u B) != RE2-G1 ENEMSE @0x%x (%u B)", was, lb, re2_off, re2_len);
        CHECK(wm && !(lm == lb && memcmp(wm, wb, lb) == 0), "(2) %s: Rohr-Welle gleich der Messer-Welle", was);
        if (!k && wb && wm) {
            /* ---- (4) KLANGBILD ---- */
            int vg[1], tb[1], tm[1];
            re15_edt_resolve_layers_ex(rec, &s_vab_b, 0, vg, tb, 1);
            re15_edt_resolve_layers_ex(edh + 8 * 4, &s_vab_m, 0, vg, tm, 1);
            double db = 0, dm = 0;
            double zb = nulldurchgaenge(wb, lb, &s_vab_b.tones[tb[0]], &db);
            double zm = nulldurchgaenge(wm, lm, &s_vab_m.tones[tm[0]], &dm);
            printf("(4) Treffer: Rohr %.3f s, %.0f Nulldurchgaenge/s | Messer %.3f s, %.0f Nulldurchgaenge/s\n", db, zb, dm, zm);
            CHECK(zb * 4.0 < zm, "(4) Rohr-Treffer nicht deutlich tiefer als der Messer-Treffer (%.0f gegen %.0f /s)", zb, zm);
            CHECK(db > 0.2 && db < 0.4, "(4) Rohr-Treffer %.3f s (Birkin-Welle 0,284 s erwartet)", db);
        }
        printf("(2/3) %s: Satz %02x%02x%02x%02x, Welle %u B = RE2-G1 @0x%x\n", was, rec[0], rec[1], rec[2], rec[3], lb, re2_off);
    }
    free(rdt); free(edh); free(avb); free(vbs);
    if (fails) { printf("RIEGEL-ROT: %d Fehler\n", fails); return 1; }
    printf("RIEGEL-GRUEN: Pipe = Birkins Rohr-Toene (Treffer/Ziehen Satz 7, Schwung Satz 4), Messer unveraendert\n");
    return 0;
}
