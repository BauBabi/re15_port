/*
 * Runde 37 (2026-10-12), Spur P "masken36" — Riegel ueber die ECHTEN Engine-Leser.
 * Dossier: analysis/befunde_runde37/P_masken36.md. Orchestrator-Auftrag P (AUFTRAG.md, Regression 53741fe5).
 *
 * Befund: die ergaenzten Vordergrund-Masken der Stages 3..6 (30 MSK-Dateien, Generator 2026-09-03 b8db7b2b/02c62294)
 * wurden seit 53741fe5 fuer niemanden gezeichnet: re15_pri_load_cut_atlas_ex verlangte fuer JEDE R15M-Sektion einen
 * eigenen Atlas MASKS/ROOM%04X_PRI%02d.TIM, den nur die Freistellungs-Raeume haben -> fg_atlas=0. Die Generator-Masken
 * tragen src im BSS-Atlas des Cuts — der Mechanismus des Originals: ein Atlas je Cut aus dem BSS-Block, an VRAM (320,256)
 * geladen (@0x80021de8 ori v0,zero,0x15 / @0x80021dec jal 0x8004ee78 / @0x8004eea8..@0x8004eee0 LoadImage), jede Maske
 * tastet ihn ab (u @0x80039408/@0x800394dc, v @0x80039418/@0x800394e0, CLUT 0x7800 @0x80039498, tpage 0x95 @0x80039630).
 * Regel jetzt (include/re15_masken36_r37.h re15_pri_atlas_quelle, vom Lader bg_pc.c benutzt): R15M MIT eigenem Atlas ->
 * eigener Atlas, R15M OHNE eigenen Atlas -> BSS-Atlas. Dazu der Ausschluss (engine/src/masken36_r37.c): Generator-Rechtecke,
 * die an einem erreichbaren Stehplatz eine Figur VOR jedem Objekt uebermalen, bleiben aus.
 *
 *   test_r37_masken36 zensus      ALLE MASKS/ROOM*.MSK (Stage 1..6, Leon und Elza): je Datei/Cut-Sektion Original-Sektion
 *                                 NULL, Atlas-Quelle nach der Lader-Regel, der Atlas liegt vor (KEIN toter Cut), BLIT-PRUEFUNG
 *                                 wie analysis/esp_masken_2026-09-03/pruef_gen.py (Masken durch den Atlas auf den dekodierten
 *                                 Hintergrund: Mittel |dR|+|dG|+|dB| <= MITTEL_MAX, Anteil > 48 <= ANTEIL_MAX). Gegenproben:
 *                                 alte Regel (53741fe5) -> die 216 Stage-3..6-Sektionen tot; wo beide Atlanten vorliegen
 *                                 (ROOM1220 Cut 0/2/4/6/8, Freistellungen), faellt der falsche durch.
 *   test_r37_masken36 ausschluss  rechnet die Ausschluss-Tabelle aus den Dateien nach (== gen/masken36_ausschluss_r37.inc,
 *                                 Rechteck fuer Rechteck); danach verdeckt keine Generator-Maske eine Figur ohne Zelle davor,
 *                                 und je Stage 3..6 verdecken die Masken weiterhin Figuren HINTER Zellen (Gegenprobe: ohne
 *                                 Ausschluss gibt es Fehlverdeckungen).
 *   test_r37_masken36 verdeckung  die an der exe gemessenen Szenen (Dossier §4): HINTER -> Fuss und >= halbe Figur verdeckt,
 *                                 DAVOR -> Figur ueberlappt die Maskenflaeche, kein Punkt verdeckt (nach dem Ausschluss).
 *   test_r37_masken36 erzeuge <datei.inc>  schreibt die Ausschluss-Tabelle (Werkzeug, kein Riegel).
 *
 * MESSAUFBAU (keine Spielkonstanten):
 *   - Blit-Schwellen (Dossier §2.1): gemessen Mittel max 23.05 / Anteil > 48 max 6.51 % (BSS, 224 Sektionen) und 8.61 /
 *     0.54 % (eigener Atlas, 99); falscher Atlas min 55.87 / 44.8 % -> MITTEL_MAX 40, ANTEIL_MAX 20 %.
 *   - Figur = Leon, an der exe gemessen (RE15_TRILOG + Rueckprojektion, Dossier §3.2): ROOM6010 Cut 12 (-14737,-6720)
 *     y -2999..39; ROOM3040 Cut 1 (-22900,-9900) rot 1024 y -2996..-7, quer 706 -> FIGUR_HOCH 3000, FIGUR_HALB 350.
 *   - Stehplatz: Gitter 100, Spielerradius PR 450 (DAT_80073e94[6], re15_collision.c PR), Band-Boden y = -Band*0x708
 *     (re15_collision.h), solide Zelle = u0 & 1 und floor>>4 == Band (FUN_8003b0a4, re15_collision.c Kopf), erreichbar =
 *     Flutfuellung je Band ab den AOTs des Raums (Engine-SCD main00+sub00 wie probe_1190_props) und den Tuer-Ankuenften
 *     aller Raeume, innerhalb der Kamerazonen des Bands (RVD, zone.floor == Band oder 0xFF), Akteur-Cull im Regionsviereck
 *     des Cuts (re15_rdt_get_region_quad, FUN_8002c18c).
 *   - Nachbesserung 1 (Abnahme 0 M1/M2, Dossier "Nachbesserung 1"): Tuer-Ankuenfte aus JEDEM Door_aot_set-Satz der RDTs
 *     (nicht nur dem VM-Stand des ersten Besuchs - ROOM3091 +0x14B2 machte die Tuer nach ROOM30A1 dort zur Nachricht);
 *     Saaten je Raumpaar xxx0/xxx1 (ein Hintergrund @0x80021c90, Tuerziel ohne Variante @0x8001d94c/@0x8001d960);
 *     nicht-rechteckige Zellen ueber den Kollisions-Resolver des Ports (form_drin) statt als Kasten (schraege Gaenge
 *     ROOM4040/4080). Sektionen ohne Stehplatz nur noch belegt (OHNE_STEHPLATZ_BELEGT).
 *   Werkzeug-Modi (kein Riegel): saaten <raum> | cut <raum> <cut> | fehl <raum> <cut> [x z] | proj <raum> <cut> <x> <z> [band]
 *   | soll | suche [prozent] [stage] | erzeuge <datei.inc>.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "re15_rdt.h"
#include "re15_camera.h"
#include "re15_pri.h"
#include "re15_tim.h"
#include "re15_sld.h"
#include "re15_bss.h"
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_masken36_r37.h"
#include "re15_pri1100_r37.h"
#include "re15_collision.h"

#define MITTEL_MAX   40.0
#define ANTEIL_MAX   0.20
#define CHUNK        0x10000
#define FIGUR_HOCH   3000.0
#define FIGUR_HALB   350.0
#define PR           450
#define BAND_Y       0x708
#define SCHRITT      100
#define GMAX         720
#define SAAT_MAX     4096

extern void scd_vm_init(void);
extern void scd_vm_tick(void);
extern int  scd_thread_start(int slot, const uint8_t *pc);
extern void scd_register_room_events(const re15_rdt_t *rdt);
extern void scd_vm_set_room_init(int on);

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

static uint8_t *slurp(const char *pfad, size_t *n)
{
    FILE *f = fopen(pfad, "rb");
    uint8_t *p; long len;
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); len = ftell(f); fseek(f, 0, SEEK_SET);
    if (len <= 0) { fclose(f); return NULL; }
    p = (uint8_t *) malloc((size_t) len);
    if (!p) { fclose(f); return NULL; }
    if (fread(p, 1, (size_t) len, f) != (size_t) len) { free(p); fclose(f); return NULL; }
    fclose(f);
    if (n) *n = (size_t) len;
    return p;
}

#define PSX RE15_PORT_SRC_DIR "/shared_assets/PSX"

/* Hintergrund-Block (Raum, Cut) wie bg_pc.c re15_bg_load_room_cut: BSS/ROOM%04X/BG%02d.BSS, sonst die Scheibe Cut*0x10000
 * der Stage-Datei STAGE%u/ROOM%03X.BSS (Original @0x80021cd8 sll a3,a3,5 = Cut*32 Sektoren). */
static uint8_t *bg_block(unsigned raum, int cut, size_t *n)
{
    char p[700]; uint8_t *b, *sb; size_t nsb = 0;
    snprintf(p, sizeof p, "%s/BSS/ROOM%04X/BG%02d.BSS", PSX, raum, cut);
    b = slurp(p, n);
    if (b) return b;
    snprintf(p, sizeof p, "%s/STAGE%u/ROOM%03X.BSS", PSX, raum >> 12, raum >> 4);
    sb = slurp(p, &nsb);
    if (!sb || (size_t) (cut + 1) * CHUNK > nsb) { free(sb); *n = 0; return NULL; }
    b = (uint8_t *) malloc(CHUNK);
    if (b) memcpy(b, sb + (size_t) cut * CHUNK, CHUNK);
    free(sb);
    *n = b ? CHUNK : 0;
    return b;
}

/* Hintergrundbild 320x240 RGB aus dem Block (Engine-Dekoder wie bg_pc.c / probe_bg_dump). */
static int bg_rgb(const uint8_t *blk, size_t n, uint8_t *rgb)
{
    re15_bss_chunk_t ch; int16_t *co; size_t cap; int k, rv;
    if (!blk || !re15_bss_parse_chunk(blk, n, &ch) || !re15_bss_chunk_has_video(&ch)) return 0;
    cap = ((size_t) ch.run_length_words + 2) * 8 + 4096;
    co = (int16_t *) malloc(cap * sizeof(int16_t));
    if (!co) return 0;
    k = re15_bss_vlc_decode(ch.vlc_payload, ch.vlc_payload_size, ch.run_length_words, ch.quant, ch.version, co, cap);
    rv = (k >= 0) ? re15_bss_mdec_decode(co, (size_t) k, RE15_BSS_FRAME_WIDTH, RE15_BSS_FRAME_HEIGHT, rgb) : -1;
    free(co);
    return rv == 0;
}

typedef struct { uint8_t *roh; re15_tim_t tim; int da; const char *quelle; } atlas_t;
static uint8_t g_sld[RE15_SLD_MAX_UNPACKED];

/* BSS-Atlas wie bg_pc.c: 1. Laufzeit-Auszug aus dem Block (re15_pri_cache_from_chunk: Laenge L aus der Stage-Tabelle,
 * re15_sld_atlas_from_chunk), 2. Rueckfall BSS/ROOM%04X/PRI%02d.TIM. */
static int atlas_bss(unsigned raum, int cut, atlas_t *a)
{
    char p[700]; size_t nb = 0, ns = 0; uint8_t *blk, *sb; uint16_t L = 0; int len = 0, rv;
    memset(a, 0, sizeof *a);
    blk = bg_block(raum, cut, &nb);
    snprintf(p, sizeof p, "%s/BIN/STAGE%u.BIN", PSX, raum >> 12);
    sb = slurp(p, &ns);
    if (blk && sb) {
        rv = re15_sld_used_len(sb, (int) ns, (int) (raum >> 12), (int) ((raum >> 4) & 0xFFu), cut, &L);
        if (rv == RE15_SLD_OK) rv = re15_sld_atlas_from_chunk(blk, (int) nb, L, g_sld, RE15_SLD_MAX_UNPACKED, &len);
        if (rv == RE15_SLD_OK && re15_tim_parse(g_sld, len, &a->tim) == 0 && a->tim.bpp == 8 && a->tim.has_clut) {
            a->da = 1; a->quelle = "SLD";
        }
    }
    free(blk); free(sb);
    if (a->da) return 1;
    snprintf(p, sizeof p, "%s/BSS/ROOM%04X/PRI%02d.TIM", PSX, raum, cut);
    a->roh = slurp(p, &nb);
    if (a->roh && re15_tim_parse(a->roh, (int) nb, &a->tim) == 0 && a->tim.bpp == 8 && a->tim.has_clut) {
        a->da = 1; a->quelle = "BSS-PRI";
        return 1;
    }
    free(a->roh); a->roh = NULL;
    return 0;
}

static int atlas_eigen(unsigned raum, int cut, atlas_t *a)
{
    char p[700]; size_t n = 0;
    memset(a, 0, sizeof *a);
    snprintf(p, sizeof p, "%s/MASKS/ROOM%04X_PRI%02d.TIM", PSX, raum, cut);
    a->roh = slurp(p, &n);
    if (a->roh && re15_tim_parse(a->roh, (int) n, &a->tim) == 0 && a->tim.bpp == 8 && a->tim.has_clut) {
        a->da = 1; a->quelle = "eigen";
        return 1;
    }
    free(a->roh); a->roh = NULL;
    return 0;
}

/* Blit wie pruef_gen.py / der Zeichner (SDL_RenderCopy klippt je Punkt): Masken durch den Atlas auf den Hintergrund.
 * Rueckgabe gemalte Punkte; *mittel = mittlere |dR|+|dG|+|dB|, *anteil = Anteil > 48. */
static int blit(const re15_pri_cut_t *pri, const atlas_t *a, const uint8_t *rgb, double *mittel, double *anteil)
{
    static uint8_t gemalt[240][320];
    static uint16_t farbe[240][320];
    const uint8_t *idx = (const uint8_t *) a->tim.pixels;
    long summe = 0; int n = 0, gross = 0, m, x, y;
    memset(gemalt, 0, sizeof gemalt);
    for (m = 0; m < pri->draw_count; m++) {
        const re15_pri_mask_t *q = &pri->masks[m];
        int dx = (int16_t) q->dstX, dy = (int16_t) q->dstY;
        for (y = 0; y < q->height; y++)
            for (x = 0; x < q->width; x++) {
                int ax = q->srcX + x, ay = q->srcY + y, tx = dx + x, ty = dy + y;
                if (ax >= a->tim.width || ay >= a->tim.height || tx < 0 || ty < 0 || tx >= 320 || ty >= 240) continue;
                if (!idx[ay * a->tim.width + ax]) continue;          /* Palettenindex 0 = durchsichtig */
                gemalt[ty][tx] = 1;
                farbe[ty][tx] = a->tim.clut[idx[ay * a->tim.width + ax]];
            }
    }
    for (y = 0; y < 240; y++)
        for (x = 0; x < 320; x++) {
            int r, g, b, d;
            if (!gemalt[y][x]) continue;
            r = (farbe[y][x] & 0x1f) << 3; g = ((farbe[y][x] >> 5) & 0x1f) << 3; b = ((farbe[y][x] >> 10) & 0x1f) << 3;
            d = abs(r - rgb[(y * 320 + x) * 3]) + abs(g - rgb[(y * 320 + x) * 3 + 1]) + abs(b - rgb[(y * 320 + x) * 3 + 2]);
            summe += d; n++;
            if (d > 48) gross++;
        }
    *mittel = n ? (double) summe / n : 0.0;
    *anteil = n ? (double) gross / n : 0.0;
    return n;
}

static int proj(const re15_camera_view_t *v, double x, double y, double z, double *sx, double *sy, double *vz)
{
    double vx = (x * v->rot[0] + y * v->rot[1] + z * v->rot[2]) / 4096.0 + v->trans[0];
    double vy = (x * v->rot[3] + y * v->rot[4] + z * v->rot[5]) / 4096.0 + v->trans[1];
    *vz = (x * v->rot[6] + y * v->rot[7] + z * v->rot[8]) / 4096.0 + v->trans[2];
    if (*vz <= 64) return 0;
    *sx = 160 + vx * v->fov_screen_dist / *vz; *sy = 120 + vy * v->fov_screen_dist / *vz;
    return 1;
}

/* Kameraort in Weltkoordinaten: exaktes Inverses der Q12-Matrix (wie test_r37_pri1100.c kamera_ort). */
static void kamera_ort(const re15_camera_view_t *v, double c[3])
{
    const double a = v->rot[0], b = v->rot[1], cc = v->rot[2], d = v->rot[3], e = v->rot[4], f = v->rot[5],
                 g = v->rot[6], h = v->rot[7], k = v->rot[8];
    double det = a * (e * k - f * h) - b * (d * k - f * g) + cc * (d * h - e * g);
    double inv[9], t0 = -4096.0 * v->trans[0], t1 = -4096.0 * v->trans[1], t2 = -4096.0 * v->trans[2];
    inv[0] = (e * k - f * h) / det; inv[1] = (cc * h - b * k) / det; inv[2] = (b * f - cc * e) / det;
    inv[3] = (f * g - d * k) / det; inv[4] = (a * k - cc * g) / det; inv[5] = (cc * d - a * f) / det;
    inv[6] = (d * h - e * g) / det; inv[7] = (b * g - a * h) / det; inv[8] = (a * e - b * d) / det;
    c[0] = inv[0] * t0 + inv[1] * t1 + inv[2] * t2;
    c[1] = inv[3] * t0 + inv[4] * t1 + inv[5] * t2;
    c[2] = inv[6] * t0 + inv[7] * t1 + inv[8] * t2;
}

/* ---- Figur ------------------------------------------------------------------------------------------------------ */
/* Die Figur = Kasten quer zur Kamera: quer -FIGUR_HALB .. +FIGUR_HALB entlang der Kamera-x-Achse (lx, lz), hoch 0 ..
 * FIGUR_HOCH ueber dem Band-Boden. Nachbesserung 2 (Abnahme 1 M2): PIXELGENAU abgetastet - so viele Spalten, wie der Kasten
 * Bildspalten breit ist (mindestens 9, ungerade, Raender eingeschlossen), und je Spalte genau ein Punkt in JEDER Bildzeile
 * von der Kopfoberkante bis zum Fuss (beide eingeschlossen). Vorher 9 x 9 Punkte mit ZeilenMITTEN FIGUR_HOCH/9 * (z + 0.5)
 * = 167 .. 2833: die Baender 0..167 und 2833..3000 (Haar) wurden nie gefragt, duenne Masken zwischen zwei Zeilen auch
 * nicht (ROOM30A0 Cut 0 (-25400,13550): der Rahmen-Balken der Buero-Trennwand schnitt Leon VOR der Trennwand das Haar ab,
 * Abnahme 1: 4/7 Punkte an der exe; Werkzeug punkt: alte Abtastung 0, neue 5 Punkte auf Maske 20).
 * Werkzeug-Optionen (kein Riegel): --alt = alte Abtastung, --zeilen=N [--spalten=M] = festes Gitter mit Raendern
 * (Konvergenzmessung Dossier NB2.2).
 * Spaltenzahl: hoechstens SP_MAX = 321 = eine Spalte je Bildspalte ueber die volle Bildbreite 320 (Messaufbau, keine
 * Spielkonstante; vorher 129 - ein Kasten breiter als 128 Bildpunkte waere mit Abstand > 1 Punkt abgetastet worden).
 * g_sp_gekappt zaehlt Kaesten, die breiter als das Bild sind (Ausgabe in ausschluss/erzeuge). */
typedef struct { int16_t x, y; double vz; uint16_t spalte; uint8_t fuss; } figpt_t;
#define SP_MAX    321
#define FIGPT_MAX (SP_MAX * 240)
static int g_fz, g_fs = 9, g_fz_alt;     /* g_fz == 0 und !g_fz_alt: pixelgenau (Riegel und Tabelle) */
static long g_sp_kaesten, g_sp_gekappt;  /* pixelgenau abgetastete Kaesten / davon breiter als SP_MAX - 1 Bildpunkte */
static double g_sp_breit_max;            /* breitester abgetasteter Kasten in Bildpunkten */
static int g_raster_gekappt;             /* letzter figur_raster-Aufruf: Spalten gekappt (Abstand > 1 Bildpunkt) */
static long g_sp_gekappt_offen;          /* gekappte Kaesten, an denen eine Maske verdecken KOENNTE (s. sektion_pruefen) */
static double g_raster_halb_vz;          /* letzter Aufruf: groesstes vz einer Spalte, die halb hinter der Kamera liegt (0 = keine) */
static long g_sp_halb, g_sp_halb_offen;  /* Kaesten mit solchen Spalten / davon Maske koennte verdecken */

/* quer-Versatz der Spalte j von n */
static double spalte_o(int j, int n) { return (j - (n - 1) / 2) * (2.0 * FIGUR_HALB / (double) (n - 1)); }

static void ansicht(const re15_camera_view_t *v, double x, double y, double z, double o[3])
{
    o[0] = (x * v->rot[0] + y * v->rot[1] + z * v->rot[2]) / 4096.0 + v->trans[0];
    o[1] = (x * v->rot[3] + y * v->rot[4] + z * v->rot[5]) / 4096.0 + v->trans[1];
    o[2] = (x * v->rot[6] + y * v->rot[7] + z * v->rot[8]) / 4096.0 + v->trans[2];
}

/* Figurpunkte an (wx, wz) auf Band band in pt (nur Bildzeilen 0..239; x kann ausserhalb liegen). *n_sp = Spaltenzahl,
 * *gesamt = Zahl aller Punkte einschliesslich der Zeilen ausserhalb des Bilds (fuer "Figur im Bild"). Punkte hinter der
 * Kamera (vz <= 64, wie proj) fehlen. */
static int figur_raster(const re15_camera_view_t *v, double wx, double wz, int band, double lx, double lz,
                        figpt_t *pt, int *n_sp, int *gesamt)
{
    const double boden = -(double) band * BAND_Y, f = v->fov_screen_dist;
    int nc, j, n = 0;
    *gesamt = 0;
    g_raster_gekappt = 0;
    g_raster_halb_vz = 0;
    if (g_fz_alt || g_fz) {     /* festes Gitter (Werkzeug/Gegenprobe) */
        int nz = g_fz_alt ? 9 : g_fz, ns = g_fz_alt ? 9 : g_fs, k;
        *n_sp = ns;
        for (k = 0; k < nz * ns; k++) {
            int zl = k / ns, sp = k % ns; double h, o = spalte_o(sp, ns), sx, sy, vz;
            h = g_fz_alt ? (FIGUR_HOCH / 9.0) * (zl + 0.5) : FIGUR_HOCH * zl / (double) (nz - 1);
            if (!proj(v, wx + o * lx, boden - h, wz + o * lz, &sx, &sy, &vz)) continue;
            (*gesamt)++;
            if (sy < 0 || sy >= 240 || sx < -1000 || sx > 1000) continue;
            pt[n].x = (int16_t) floor(sx); pt[n].y = (int16_t) sy; pt[n].vz = vz;
            pt[n].spalte = (uint16_t) sp; pt[n].fuss = (k == (ns - 1) / 2); n++;
        }
        return n;
    }
    {   /* Spaltenzahl = Breite des Kastens in Bildpunkten (Fuss- und Kopfkante, die breitere) */
        double b = 0, sa, sb, sy, vz; int e;
        for (e = 0; e < 2; e++) {
            double hy = boden - (e ? FIGUR_HOCH : 0.0);
            if (proj(v, wx - FIGUR_HALB * lx, hy, wz - FIGUR_HALB * lz, &sa, &sy, &vz)
                && proj(v, wx + FIGUR_HALB * lx, hy, wz + FIGUR_HALB * lz, &sb, &sy, &vz) && fabs(sb - sa) > b)
                b = fabs(sb - sa);
        }
        nc = (int) ceil(b) + 1;
        if (nc < 9) nc = 9;
        if (!(nc & 1)) nc++;
        g_sp_kaesten++;
        if (b > g_sp_breit_max) g_sp_breit_max = b;
        g_raster_gekappt = nc > SP_MAX;
        if (nc > SP_MAX) { nc = SP_MAX; g_sp_gekappt++; }
    }
    *n_sp = nc;
    for (j = 0; j < nc; j++) {
        double o = spalte_o(j, nc), p0[3], p1[3], d[3], y0, y1, ylo, yhi; int iy, ia, ib, ifuss;
        ansicht(v, wx + o * lx, boden, wz + o * lz, p0);                 /* Fuss */
        ansicht(v, wx + o * lx, boden - FIGUR_HOCH, wz + o * lz, p1);    /* Kopfoberkante */
        if (p0[2] <= 64 || p1[2] <= 64) {   /* (teilweise) hinter der Kamera: Schranke in sektion_pruefen */
            double vzm = p0[2] > p1[2] ? p0[2] : p1[2];
            if (vzm > g_raster_halb_vz) g_raster_halb_vz = vzm;
            continue;
        }
        d[0] = p1[0] - p0[0]; d[1] = p1[1] - p0[1]; d[2] = p1[2] - p0[2];
        y0 = 120 + p0[1] * f / p0[2];
        y1 = 120 + p1[1] * f / p1[2];
        ylo = y0 < y1 ? y0 : y1; yhi = y0 < y1 ? y1 : y0;
        ia = (int) floor(ylo); ib = (int) floor(yhi); ifuss = (int) floor(y0);
        *gesamt += ib - ia + 1;
        if (ia < 0) ia = 0;
        if (ib > 239) ib = 239;
        for (iy = ia; iy <= ib; iy++) {
            /* Punkt der senkrechten Kante in Bildzeile iy (Zeilenmitte, an die Kantenenden geklemmt), perspektivisch
             * richtig: (ys - 120) * (p0z + t dz) = f * (p0y + t dy) */
            double ys = iy + 0.5, den, t, vz, sx;
            if (ys < ylo) ys = ylo;
            if (ys > yhi) ys = yhi;
            den = f * d[1] - (ys - 120) * d[2];
            t = fabs(den) > 1e-12 ? ((ys - 120) * p0[2] - f * p0[1]) / den : 0.0;
            if (t < 0) t = 0;
            if (t > 1) t = 1;
            vz = p0[2] + t * d[2];
            sx = 160 + (p0[0] + t * d[0]) * f / vz;
            if (sx < -1000 || sx > 1000) continue;
            pt[n].x = (int16_t) floor(sx); pt[n].y = (int16_t) iy; pt[n].vz = vz;
            pt[n].spalte = (uint16_t) j; pt[n].fuss = (j == (nc - 1) / 2 && iy == ifuss); n++;
        }
    }
    return n;
}

/* ---- Erreichbarkeit -------------------------------------------------------------------------------------------- */
typedef struct { unsigned raum; int x, z, band; char art; unsigned von; uint32_t off; } saat_t;   /* art: A = AOT, T = Tuer (VM), S = Tuer (Satz statisch) */
static saat_t g_saat[SAAT_MAX];
static int g_saat_n;

static void saat_dazu(unsigned raum, int x, int z, int band, char art, unsigned von, uint32_t off)
{
    if (g_saat_n < SAAT_MAX && band >= 0 && band < 16) {
        g_saat[g_saat_n].raum = raum; g_saat[g_saat_n].x = x; g_saat[g_saat_n].z = z; g_saat[g_saat_n].band = band;
        g_saat[g_saat_n].art = art; g_saat[g_saat_n].von = von; g_saat[g_saat_n].off = off;
        g_saat_n++;
    }
}

/* ---- Tuer-Ankuenfte aus JEDEM Door_aot_set-Satz der RDT (Nachbesserung 1, Mangel M1) ------------------------------
 * Der VM-Lauf oben sieht nur die AOTs, die nach main00+sub00 (erster Besuch, frische Flags) noch als Tuer stehen. In
 * ROOM3091 (Datei-Offsets der RDT) setzt main00 die Tuer nach ROOM30A1 (+0x1398 Door_aot_set Slot 2, Nutzlast Stage 2 /
 * Raum 0x0A, Ankunft (-25500,0,11700)); sub00 macht sie beim ersten Besuch (+0x1492 Ck(4,16)=0, +0x149A Ck(4,8)=0) zur
 * Nachrichtenflaeche (+0x14B2 Aot_reset Slot 2 sce 1); das Elza-Ereignis sub07 setzt (4,16) (+0x16D0) und schaltet sie
 * zurueck (+0x1D7C Aot_reset Slot 2 sce 2), ab dann bleibt sie Tuer. Die Ankunft in ROOM30A1 fehlte deshalb als Saat ->
 * 0 Stehplaetze -> kein Ausschluss (Abnahme 0 M1). Jetzt zaehlt jeder Tuersatz des Spiels:
 * Satz 0x3B, Laenge 32 bzw. 40 bei pc[3] & 0x80 (@0x80040618/@0x80040630), Nutzlast ab pc+14 bzw. pc+22 (@0x80042fb8 /
 * @0x80042f90), darin +0/+2/+4 Ankunft, +8 Stage, +9 Raum (Warp FUN_8001d600); Variante = die des Quellraums
 * (aot_common.c dest_id). Gelaufen wie scripts/door_graph.py / test_r36_kartenbits.c (Regionen der main-/sub-Sektion,
 * Laengen aus scd_opcode_size_at = Vorschub der VM, Ende hinter dem weitesten Vorwaertsziel). Satz mit Ankunft (0,0,0)
 * zaehlt nicht (aot_common.c: ungueltige Tuer). */
static uint16_t m36_le16(const uint8_t *p) { return (uint16_t) (p[0] | (p[1] << 8)); }
static int16_t  m36_les16(const uint8_t *p) { return (int16_t) m36_le16(p); }
static uint32_t m36_le32(const uint8_t *p) { return (uint32_t) p[0] | ((uint32_t) p[1] << 8) | ((uint32_t) p[2] << 16) | ((uint32_t) p[3] << 24); }

static uint32_t m36_sektion_ende(const uint8_t *d, size_t n, uint32_t s)
{
    uint32_t boden = s, best = 0, o, mx = 0, i;
    uint16_t first = m36_le16(d + s);
    if (first >= 2 && !(first & 1) && s + first <= n)
        for (i = 0; i < first / 2u; i++) {
            uint16_t q = m36_le16(d + s + 2 * i);
            if (q && s + q < n && q > mx) mx = q;
        }
    boden += mx;
    for (o = 0x40; o < 0x60 && o + 4 <= n; o += 4) {
        uint32_t v = m36_le32(d + o);
        if (v > boden && v <= n && (best == 0 || v < best)) best = v;
    }
    return best ? best : (uint32_t) n;
}

static uint32_t m36_sprungziel(const uint8_t *d, uint32_t pc)
{
    uint8_t op = d[pc];
    if (op == 0x06) return pc + 4 + m36_le16(d + pc + 2);
    if (op == 0x07) return (uint32_t) ((int32_t) pc + m36_les16(d + pc + 2));
    if (op == 0x0D || op == 0x1B) return (uint32_t) ((int32_t) pc + 6 + m36_les16(d + pc + 2));
    if (op == 0x0F || op == 0x11 || op == 0x13) return (uint32_t) ((int32_t) pc + 4 + m36_les16(d + pc + 2));
    if (op == 0x14) return pc + 6 + m36_le16(d + pc + 2);
    if (op == 0x17) { int16_t o = m36_les16(d + pc + 4); return o > 0 ? (uint32_t) ((int32_t) pc + o) : 0; }
    return 0;
}

static void m36_tueren_region(const uint8_t *d, uint32_t start, uint32_t ende, unsigned raum)
{
    uint32_t pc = start, maxf = start;
    while (pc < ende) {
        int sz = scd_opcode_size_at(d + pc);
        uint32_t t;
        if (sz <= 0 || pc + (uint32_t) sz > ende) return;
        if (d[pc] == 0x01 && pc + 2 > maxf) return;                 /* Evt_end hinter dem weitesten Vorwaertsziel */
        if (d[pc] == 0x3B) {
            const uint8_t *nl = (d[pc + 3] & 0x80) ? d + pc + 22 : d + pc + 14;
            int nx = m36_les16(nl), ny = m36_les16(nl + 2), nz = m36_les16(nl + 4);
            if (nl[8] <= 6 && (nx || ny || nz)) {
                unsigned ziel = ((unsigned) (nl[8] + 1) << 12) | ((unsigned) nl[9] << 4) | (raum & 0xFu);
                int zb = ny < 0 ? (-ny + BAND_Y / 2) / BAND_Y : 0;
                saat_dazu(ziel, nx, nz, zb, 'S', raum, pc);
            }
        }
        t = m36_sprungziel(d, pc);
        if (t > maxf) maxf = t;
        pc += (uint32_t) sz;
    }
}

static void m36_tueren_statisch(const uint8_t *d, size_t n, unsigned raum)
{
    int k;
    if (n < 0x48) return;
    for (k = 0; k < 2; k++) {
        uint32_t ss = m36_le32(d + 0x40 + 4 * k), se, offs[512]; uint16_t first; int cnt = 0, i, j;
        if (ss == 0 || ss + 2 > n) continue;
        se = m36_sektion_ende(d, n, ss);
        first = m36_le16(d + ss);
        if (first < 2 || (first & 1) || ss + first > n) continue;
        for (i = 0; i < first / 2 && cnt < 512; i++) {
            uint16_t o = m36_le16(d + ss + 2 * i); int dup = 0;
            if (!o || ss + o >= n) continue;
            for (j = 0; j < cnt; j++) if (offs[j] == o) dup = 1;
            if (!dup) offs[cnt++] = o;
        }
        for (i = 0; i < cnt; i++)
            for (j = i + 1; j < cnt; j++)
                if (offs[j] < offs[i]) { uint32_t t = offs[i]; offs[i] = offs[j]; offs[j] = t; }
        for (i = 0; i < cnt; i++) {
            uint32_t e = (i + 1 < cnt) ? offs[i + 1] : se - ss;
            if (e > se - ss) e = se - ss;
            if (e > offs[i]) m36_tueren_region(d, ss + offs[i], ss + e, raum);
        }
    }
}

/* Saaten aller RDTs (Stage 1..6): AOT-Mitten mit eigenem Band (alle Typen ausser GENERIC/CAM_SWITCH/AUTO_EVENT;
 * Band 0x80 = jedes Band -> kein Beleg) und je Tuer die Ankunft im Zielraum (Band aus Ziel-y, Boden y = -Band*0x708). */
static void saaten_sammeln(void)
{
    int stage, rr, var;
    static re15_rdt_t rdt;
    g_saat_n = 0;
    for (stage = 1; stage <= 6; stage++)
        for (rr = 0; rr < 0x40; rr++)
            for (var = 0; var < 2; var++) {
                unsigned raum = ((unsigned) stage << 12) | ((unsigned) rr << 4) | (unsigned) var;
                char p[700]; size_t n = 0; uint8_t *buf; int t, i;
                snprintf(p, sizeof p, "%s/STAGE%u/ROOM%04X.RDT", PSX, stage, raum);
                buf = slurp(p, &n);
                if (!buf || n < 0x60 || re15_rdt_parse(buf, n, &rdt) != 0) { free(buf); continue; }
                memset(&g_aot, 0, sizeof g_aot);
                scd_vm_init();
                scd_register_room_events(&rdt);
                if (rdt.main_scd)   scd_thread_start(0, rdt.main_scd);
                if (rdt.sub_scd[0]) scd_thread_start(1, rdt.sub_scd[0]);
                scd_vm_set_room_init(1);
                for (t = 0; t < 4; t++) scd_vm_tick();
                scd_vm_set_room_init(0);
                for (t = 0; t < 300; t++) scd_vm_tick();
                for (i = 0; i < RE15_AOT_MAX; i++) {
                    const re15_aot_t *q = &g_aot.slots[i];
                    int x = q->x, z = q->z;
                    if (!q->active) continue;
                    if (q->has_quad) {
                        x = (q->xs[0] + q->xs[1] + q->xs[2] + q->xs[3]) / 4;
                        z = (q->zs[0] + q->zs[1] + q->zs[2] + q->zs[3]) / 4;
                    }
                    if (q->type == RE15_AOT_TYPE_DOOR) {
                        const re15_aot_door_params_t *dp = &g_aot.door_params[i];
                        unsigned ziel = ((unsigned) (dp->dest_stage + 1) << 12) | ((unsigned) dp->dest_room << 4) | (raum & 1u);
                        int zb = dp->spawn_y < 0 ? (-dp->spawn_y + BAND_Y / 2) / BAND_Y : 0;
                        saat_dazu(ziel, dp->spawn_x, dp->spawn_z, zb, 'T', raum, 0);
                    }
                    if (q->band & 0x80) continue;
                    /* Zonen ohne Stehplatz-Beleg: GENERIC (Ereignisflaeche), CAM_SWITCH, AUTO_EVENT */
                    if (q->type == RE15_AOT_TYPE_GENERIC || q->type == RE15_AOT_TYPE_CAM_SWITCH
                        || q->type == RE15_AOT_TYPE_AUTO_EVENT) continue;
                    saat_dazu(raum, x, z, q->band, 'A', raum, 0);
                }
                m36_tueren_statisch(buf, n, raum);          /* Nachbesserung 1 (M1): jeder Tuersatz, nicht nur der VM-Stand */
                free(buf);
            }
}

static int solide(const re15_sca_entry_t *e, int band) { return (e->u0 & 1) && (e->floor >> 4) == band; }

/* Nachbesserung 1 (M2): FORM der Zelle. Rechtecke (type 1) sind ihr Kasten; alle anderen Formen (2/4/5/6/7 schraeg,
 * 3 Kreis, 8/9 Kapsel) fragt das Modell beim Kollisions-Resolver des Ports selbst (re15_collision_constrain_contact_band
 * = FUN_8003b0a4 mit Spieler-Maske 1 und Radius 450, Formen push_diag2..7 / push_circle / push_caps8/9 in
 * re15_collision.c), mit genau DIESER Zelle in jeder Quadranten-Partition. Vorher galt jede Zelle als ihr Kasten: die
 * schraegen Gaenge in ROOM4040 Cut 9..12 / ROOM4080 Cut 5..8 waren damit ganz zu (0 freie Punkte im Regionsviereck) und
 * die Sektionen ohne Stehplatz - der Riegel war dort leer wahr. drin = der Resolver verschiebt einen dort stehenden Spieler. */
static int form_drin(const re15_sca_entry_t *e, int x, int z, int r)
{
    static re15_rdt_t f; static re15_sca_entry_t k[5]; static int bereit;
    int32_t px = x, pz = z; int i;
    for (i = 0; i < 5; i++) k[i] = *e;
    if (!bereit) {                                       /* der Resolver liest nur sca, sca_count, sca_rgn, ceiling_x/z */
        memset(&f, 0, sizeof f);
        f.sca = k; f.sca_count = 5;
        for (i = 0; i < 5; i++) f.sca_rgn[i] = 1;        /* jede Quadranten-Partition (FUN_8003aea0) traegt die Zelle */
        bereit = 1;
    }
    /* alte Lage = neue Lage: ein Spieler, der hier STEHT. push_diag4..7 bestimmen die Seite der Schraege aus der alten Lage
     * (z.B. push_diag4 Kreuzprodukt mit prevx/prevz); mit der eigenen Lage gilt die Seite, auf der der Punkt liegt. Gemessen
     * gegen die Variante 'aus jeder der vier Achsrichtungen um 1 heranruecken': gleiche Ausschluss-Bits in allen 216
     * Sektionen, einzig ROOM4040/4041 Cut 11 zaehlt 5426 statt 5073 Stehplaetze. */
    re15_collision_constrain_contact_band(&f, x, z, &px, &pz, r, e->floor >> 4, 1u, NULL, NULL);
    return px != x || pz != z;
}

/* Punkt (x,z) in der Zelle, um r aufgeblasen? Kasten-Vortest, Form wie oben. */
static int in_zelle(const re15_sca_entry_t *e, double x, double z, int r)
{
    if (x < e->x - r || x >= e->x + e->width + r || z < e->z - r || z >= e->z + e->density + r) return 0;
    if (e->type == 1) return 1;
    return form_drin(e, (int) floor(x + 0.5), (int) floor(z + 0.5), r);
}

/* Die SCA-Liste traegt jede Zelle je Quadranten-Partition (FUN_8003aea0) - das Modell prueft jede Zelle einmal. */
#define ZEL_MAX 1024
#define RASTER  25                   /* Abtastweite der Strecke und Raster der Form (Radius 0) */
typedef struct { re15_sca_entry_t z[ZEL_MAX]; int n; uint8_t *ras[ZEL_MAX]; int rw[ZEL_MAX], rd[ZEL_MAX]; } zellen_t;

static void zellen_frei(zellen_t *Z)
{
    int i;
    for (i = 0; i < ZEL_MAX; i++) { free(Z->ras[i]); Z->ras[i] = NULL; }
}

/* Punkt in der FORM von Zelle i (Radius 0)? Nicht-Rechtecke ueber ein Raster der Weite RASTER, einmal je Raum aus form_drin
 * erzeugt (der Resolver je Abtastpunkt war zu langsam: ROOM4040/4080 je 10 s). */
static int form0(zellen_t *Z, int i, double x, double z)
{
    const re15_sca_entry_t *e = &Z->z[i];
    int ii, jj;
    if (x < e->x || x >= e->x + e->width || z < e->z || z >= e->z + e->density) return 0;
    if (e->type == 1) return 1;
    if (!Z->ras[i]) {
        int a, b;
        Z->rw[i] = e->width / RASTER + 1; Z->rd[i] = e->density / RASTER + 1;
        Z->ras[i] = (uint8_t *) malloc((size_t) Z->rw[i] * (size_t) Z->rd[i]);
        if (!Z->ras[i]) return form_drin(e, (int) floor(x + 0.5), (int) floor(z + 0.5), 0);
        for (b = 0; b < Z->rd[i]; b++)
            for (a = 0; a < Z->rw[i]; a++)
                Z->ras[i][b * Z->rw[i] + a] = (uint8_t) form_drin(e, e->x + a * RASTER, e->z + b * RASTER, 0);
    }
    ii = (int) ((x - e->x) / RASTER + 0.5); jj = (int) ((z - e->z) / RASTER + 0.5);
    if (ii >= Z->rw[i]) ii = Z->rw[i] - 1;
    if (jj >= Z->rd[i]) jj = Z->rd[i] - 1;
    return Z->ras[i][jj * Z->rw[i] + ii];
}

static void zellen_entdoppeln(const re15_rdt_t *r, zellen_t *Z)
{
    int i, j;
    zellen_frei(Z);
    Z->n = 0;
    for (i = 0; i < r->sca_count && Z->n < ZEL_MAX; i++) {
        for (j = 0; j < Z->n; j++) if (!memcmp(&Z->z[j], &r->sca[i], sizeof r->sca[i])) break;
        if (j == Z->n) Z->z[Z->n++] = r->sca[i];
    }
}

static int frei(const zellen_t *Z, int band, int x, int z)
{
    int i;
    for (i = 0; i < Z->n; i++) {
        const re15_sca_entry_t *e = &Z->z[i];
        if (!solide(e, band)) continue;
        if (in_zelle(e, x, z, PR)) return 0;
    }
    return 1;
}

static int in_zonen(const re15_rdt_t *r, int band, int x, int z)
{
    int i;
    if (r->zone_count <= 0) return 1;
    for (i = 0; i < r->zone_count; i++)
        if ((r->zones[i].floor == 0xFF || r->zones[i].floor == band) && re15_aot_point_in_quad(x, z, r->zones[i].xs, r->zones[i].zs))
            return 1;
    return 0;
}

/* kreuzt die Grundriss-Strecke Kamera -> Figurpunkt eine solide Zelle des Bands? (Zellen mit Kamera oder Ziel darin zaehlen
 * nicht). Rechteck: Strecke gegen den Kasten geschnitten; andere Formen: der geschnittene Teil wird in Schritten von hoechstens
 * 25 abgetastet und jeder Punkt der Form gefragt (in_zelle, Radius 0). */
static int zelle_dazwischen(zellen_t *Z, int band, double ax, double az, double bx, double bz)
{
    int i;
    for (i = 0; i < Z->n; i++) {
        const re15_sca_entry_t *e = &Z->z[i];
        double x0 = e->x, x1 = e->x + e->width, z0 = e->z, z1 = e->z + e->density, t0 = 0.0, t1 = 1.0, dx = bx - ax, dz = bz - az;
        if (!solide(e, band)) continue;
        if (fabs(dx) < 1e-9) { if (ax < x0 || ax >= x1) continue; }
        else { double u0 = (x0 - ax) / dx, u1 = (x1 - ax) / dx; if (u0 > u1) { double t = u0; u0 = u1; u1 = t; } if (u0 > t0) t0 = u0; if (u1 < t1) t1 = u1; }
        if (fabs(dz) < 1e-9) { if (az < z0 || az >= z1) continue; }
        else { double u0 = (z0 - az) / dz, u1 = (z1 - az) / dz; if (u0 > u1) { double t = u0; u0 = u1; u1 = t; } if (u0 > t0) t0 = u0; if (u1 < t1) t1 = u1; }
        if (!(t0 < t1)) continue;
        if (form0(Z, i, ax, az)) continue;
        if (form0(Z, i, bx, bz)) continue;
        if (e->type == 1) return 1;
        {
            double len = (t1 - t0) * sqrt(dx * dx + dz * dz); int n = (int) (len / RASTER) + 1, k;
            for (k = 0; k <= n; k++) {
                double t = t0 + (t1 - t0) * k / n;
                if (form0(Z, i, ax + t * dx, az + t * dz)) return 1;
            }
        }
    }
    return 0;
}

typedef struct {
    re15_rdt_t rdt; uint8_t *roh; size_t n;
    int x0, z0, nx, nz;
    uint8_t err[16][GMAX][GMAX];
    uint8_t fm[16][GMAX][GMAX];      /* frei-Merker je Gitterpunkt: 0 unbekannt, 1 frei, 2 belegt */
    zellen_t zel;
} raum_t;

static int frei_g(raum_t *R, int band, int gx, int gz)
{
    if (!R->fm[band][gx][gz])
        R->fm[band][gx][gz] = frei(&R->zel, band, R->x0 + gx * SCHRITT, R->z0 + gz * SCHRITT) ? 1 : 2;
    return R->fm[band][gx][gz] == 1;
}

static void flut(raum_t *R, int band, int sx, int sz)
{
    static int qx[GMAX * GMAX], qz[GMAX * GMAX];
    int qa = 0, qe = 0, ix, iz, rr, dx, dz, gef = 0;
    ix = (sx - R->x0 + SCHRITT / 2) / SCHRITT; iz = (sz - R->z0 + SCHRITT / 2) / SCHRITT;
    if (ix < 0 || iz < 0 || ix >= R->nx || iz >= R->nz) return;
    for (rr = 0; rr <= 3 && !gef; rr++)                /* Saat am Wandrand: naechster freier Gitterpunkt (<= 300) */
        for (dx = -rr; dx <= rr && !gef; dx++)
            for (dz = -rr; dz <= rr && !gef; dz++) {
                int nx = ix + dx, nz = iz + dz, wx = R->x0 + nx * SCHRITT, wz = R->z0 + nz * SCHRITT;
                if (nx < 0 || nz < 0 || nx >= R->nx || nz >= R->nz) continue;
                if (frei_g(R, band, nx, nz) && in_zonen(&R->rdt, band, wx, wz)) { ix = nx; iz = nz; gef = 1; }
            }
    if (!gef || R->err[band][ix][iz]) return;
    R->err[band][ix][iz] = 1; qx[qe] = ix; qz[qe] = iz; qe++;
    while (qa < qe) {
        int cx = qx[qa], cz = qz[qa], d; qa++;
        for (d = 0; d < 4; d++) {
            int nx = cx + (d == 0) - (d == 1), nz = cz + (d == 2) - (d == 3), wx, wz;
            if (nx < 0 || nz < 0 || nx >= R->nx || nz >= R->nz || R->err[band][nx][nz]) continue;
            wx = R->x0 + nx * SCHRITT; wz = R->z0 + nz * SCHRITT;
            if (!frei_g(R, band, nx, nz) || !in_zonen(&R->rdt, band, wx, wz)) continue;
            R->err[band][nx][nz] = 1; qx[qe] = nx; qz[qe] = nz; qe++;
        }
    }
}

/* RDT laden + Erreichbarkeit je Band. raum = die Raum-Id der RDT (Kamera, Kollision, Zonen). */
static int raum_laden(raum_t *R, unsigned raum)
{
    char p[700]; int i, X1 = -(1 << 30), Z1 = -(1 << 30);
    zellen_frei(&R->zel);
    memset(R, 0, sizeof *R);
    snprintf(p, sizeof p, "%s/STAGE%u/ROOM%04X.RDT", PSX, raum >> 12, raum);
    R->roh = slurp(p, &R->n);
    if (!R->roh || re15_rdt_parse(R->roh, R->n, &R->rdt) < 0) return 0;
    R->x0 = 1 << 30; R->z0 = 1 << 30;
    for (i = 0; i < R->rdt.sca_count; i++) {
        const re15_sca_entry_t *e = &R->rdt.sca[i];
        if (e->x < R->x0) R->x0 = e->x;
        if (e->z < R->z0) R->z0 = e->z;
        if (e->x + e->width > X1) X1 = e->x + e->width;
        if (e->z + e->density > Z1) Z1 = e->z + e->density;
    }
    if (R->rdt.sca_count <= 0) return 1;
    zellen_entdoppeln(&R->rdt, &R->zel);
    R->nx = (X1 - R->x0) / SCHRITT + 1; R->nz = (Z1 - R->z0) / SCHRITT + 1;
    if (R->nx > GMAX) R->nx = GMAX;
    if (R->nz > GMAX) R->nz = GMAX;
    /* Nachbesserung 1 (M1): Saaten des RAUMPAARS (Leon xxx0 / Elza xxx1). Beide sind derselbe Raum: ein Hintergrund (BSS
     * ohne Variantenziffer: Sektor-Tabelle @0x8007438c[Stage @0x80021ca8] + Raum @0x80021c90, Cut @0x80021c88/@0x80021cd8),
     * Tuerziele ohne Variante (Warp FUN_8001d600 +8 Stage / +9 Raum), die Generator-MSK der Stages 3..6 je Paar bytegleich.
     * Die Flutfuellung laeuft in der Kollision der EIGENEN RDT (frei/in_zonen), also nur, wo diese Variante stehen kann. */
    for (i = 0; i < g_saat_n; i++)
        if (g_saat[i].raum == raum || g_saat[i].raum == (raum ^ 1u)) flut(R, g_saat[i].band, g_saat[i].x, g_saat[i].z);
    return 1;
}

/* ---- Ausschluss einer Sektion ------------------------------------------------------------------------------------ */
typedef struct { long stehplaetze, echt, falsch; int aus[RE15_PRI_MAX_MASKS_PER_CUT], n_aus; } ergebnis_t;

/* Fehlverdeckung = ein Figurpunkt an einem erreichbaren Stehplatz liegt unter einem Maskenpunkt mit depth < bucket(vz)
 * (Original-Urteil re15_pri_mask_occludes: @0x8002565c otz>>4, ZSF3 @0x80066c70, Masken-OT = depth @0x80039650), ohne
 * dass die Grundriss-Strecke Kamera -> Figurpunkt eine solide Zelle des Bands kreuzt. mit_ausschluss: Iteration bis keine
 * Fehlverdeckung bleibt (Rechteck = vorderste Maske an diesem Punkt raus). aus_vorgabe: feste Liste statt Iteration. */
static void sektion_pruefen(raum_t *R, int cut, const re15_pri_cut_t *pri, const atlas_t *a, int iterieren,
                            const int *aus_vorgabe, ergebnis_t *E)
{
    static int16_t welch[240][320];
    static uint16_t tief[240][320];
    static int sat[241][321];            /* Summentabelle "Maske da" fuer den Grobsprung */
    static figpt_t pt[FIGPT_MAX];
    re15_camera_view_t v; double cam[3], lx, lz, ll; int16_t qx[4], qz[4]; int has_q, runde, m, x, y, b, tmin;
    const uint8_t *idx = (const uint8_t *) a->tim.pixels;
    long n_steh = 0; uint8_t *davor = NULL, *nochmal = NULL;
    memset(E, 0, sizeof *E);
    if (aus_vorgabe) for (m = 0; m < RE15_PRI_MAX_MASKS_PER_CUT; m++) E->aus[m] = aus_vorgabe[m];
    if (re15_camera_build_view(&R->rdt.cuts[cut], &v) != 0) return;
    kamera_ort(&v, cam);
    has_q = re15_rdt_get_region_quad(&R->rdt, cut, qx, qz);
    lx = v.rot[0] / 4096.0; lz = v.rot[2] / 4096.0; ll = sqrt(lx * lx + lz * lz);
    if (ll > 0) { lx /= ll; lz /= ll; }
    {   /* Merker "Zelle davor" je Stehplatz und Figurspalte (die Grundriss-Strecke Kamera -> Figurpunkt haengt nicht von der
         * Hoehe ab; gilt fuer alle Runden), Merker "Fehlverdeckung in der Vorrunde" je Stehplatz */
        int gx, gz;
        for (b = 0; b < 16; b++)
            for (gx = 0; gx < R->nx; gx++)
                for (gz = 0; gz < R->nz; gz++)
                    if (R->err[b][gx][gz] && (!has_q || re15_aot_point_in_quad(R->x0 + gx * SCHRITT, R->z0 + gz * SCHRITT, qx, qz)))
                        n_steh++;
        davor = (uint8_t *) calloc((size_t) n_steh * SP_MAX + 1, 1);
        nochmal = (uint8_t *) calloc((size_t) n_steh + 1, 1);
    }
    for (runde = 0; runde < 64; runde++) {
        int falsch_m[RE15_PRI_MAX_MASKS_PER_CUT], neu = 0;
        long si = 0;
        memset(falsch_m, 0, sizeof falsch_m);
        for (y = 0; y < 240; y++) for (x = 0; x < 320; x++) { welch[y][x] = -1; tief[y][x] = 0xFFFF; }
        for (m = 0; m < pri->draw_count; m++) {
            const re15_pri_mask_t *q = &pri->masks[m];
            int dx = (int16_t) q->dstX, dy = (int16_t) q->dstY, xx, yy;
            if (E->aus[m]) continue;
            for (yy = 0; yy < q->height; yy++)
                for (xx = 0; xx < q->width; xx++) {
                    int ax = q->srcX + xx, ay = q->srcY + yy, tx = dx + xx, ty = dy + yy;
                    if (ax >= a->tim.width || ay >= a->tim.height || tx < 0 || ty < 0 || tx >= 320 || ty >= 240) continue;
                    if (!idx[ay * a->tim.width + ax]) continue;
                    if (q->depth < tief[ty][tx]) { tief[ty][tx] = q->depth; welch[ty][tx] = (int16_t) m; }
                }
        }
        for (y = 0; y <= 240; y++) sat[y][0] = 0;
        for (x = 0; x <= 320; x++) sat[0][x] = 0;
        for (y = 0; y < 240; y++)
            for (x = 0; x < 320; x++)
                sat[y + 1][x + 1] = sat[y][x + 1] + sat[y + 1][x] - sat[y][x] + (welch[y][x] >= 0);
        tmin = 0xFFFF;
        for (y = 0; y < 240; y++)
            for (x = 0; x < 320; x++)
                if (welch[y][x] >= 0 && tief[y][x] < tmin) tmin = tief[y][x];
        E->stehplaetze = E->echt = E->falsch = 0;
        for (b = 0; b < 16; b++) {
            int gx, gz;
            for (gx = 0; gx < R->nx; gx++)
                for (gz = 0; gz < R->nz; gz++) {
                    int wx = R->x0 + gx * SCHRITT, wz = R->z0 + gz * SCHRITT, np, nsp, ges, i;
                    if (!R->err[b][gx][gz]) continue;
                    if (has_q && !re15_aot_point_in_quad(wx, wz, qx, qz)) continue;   /* Akteur-Cull (FUN_8002c18c) */
                    E->stehplaetze++;
                    si++;
                    /* spaetere Runden: nur Plaetze mit Fehlverdeckung in der Vorrunde (Masken fallen nur weg - ein Punkt ohne
                     * Fehlverdeckung bekommt keine; die Zelle davor haengt nicht von den Masken ab) */
                    if (runde > 0 && !nochmal[si - 1]) continue;
                    nochmal[si - 1] = 0;
                    {   /* Grobsprung: keine Maske im Bildrechteck des Kastens -> kein Punkt verdeckt */
                        double sx, sy, vz, x0 = 1e9, y0 = 1e9, x1 = -1e9, y1 = -1e9; int e, alle = 1, X0, Y0, X1, Y1;
                        for (e = 0; e < 4; e++) {
                            double o = (e & 1) ? FIGUR_HALB : -FIGUR_HALB, hy = -(double) b * BAND_Y - ((e & 2) ? FIGUR_HOCH : 0.0);
                            if (!proj(&v, wx + o * lx, hy, wz + o * lz, &sx, &sy, &vz)) { alle = 0; break; }
                            if (sx < x0) x0 = sx; if (sx > x1) x1 = sx; if (sy < y0) y0 = sy; if (sy > y1) y1 = sy;
                        }
                        if (alle) {
                            X0 = (int) floor(x0) - 1; Y0 = (int) floor(y0) - 1; X1 = (int) floor(x1) + 1; Y1 = (int) floor(y1) + 1;
                            if (X0 < 0) X0 = 0; if (Y0 < 0) Y0 = 0; if (X1 > 319) X1 = 319; if (Y1 > 239) Y1 = 239;
                            if (X0 > X1 || Y0 > Y1) continue;
                            if (sat[Y1 + 1][X1 + 1] - sat[Y0][X1 + 1] - sat[Y1 + 1][X0] + sat[Y0][X0] == 0) continue;
                        }
                    }
                    np = figur_raster(&v, wx, wz, b, lx, lz, pt, &nsp, &ges);
                    if (g_raster_gekappt && runde == 0) {
                        /* Kasten breiter als das Bild (nur dicht vor der Kamera): Spaltenabstand > 1 Bildpunkt. Statt dichter
                         * abzutasten eine Schranke: kein Punkt des Kastens kann verdeckt sein, wenn jede Maskentiefe im
                         * Bildrechteck des Kastens >= bucket(vz der fernsten Kastenecke) ist (vz ist in der Kastenebene
                         * linear, re15_pri_mask_occludes ist in vz monoton). Spaetere Runden haben nur weniger Masken. */
                        double c3[3], vzmax = 0, sx, sy, vz, x0 = 1e9, y0 = 1e9, x1 = -1e9, y1 = -1e9;
                        int e, alle = 1, X0 = 0, Y0 = 0, X1 = 319, Y1 = 239, dmin = 0xFFFF, xx, yy, thr;
                        for (e = 0; e < 4; e++) {
                            double o = (e & 1) ? FIGUR_HALB : -FIGUR_HALB, hy = -(double) b * BAND_Y - ((e & 2) ? FIGUR_HOCH : 0.0);
                            ansicht(&v, wx + o * lx, hy, wz + o * lz, c3);
                            if (c3[2] > vzmax) vzmax = c3[2];
                            if (!proj(&v, wx + o * lx, hy, wz + o * lz, &sx, &sy, &vz)) { alle = 0; continue; }
                            if (sx < x0) x0 = sx; if (sx > x1) x1 = sx; if (sy < y0) y0 = sy; if (sy > y1) y1 = sy;
                        }
                        if (alle) {
                            X0 = (int) floor(x0) - 1; Y0 = (int) floor(y0) - 1; X1 = (int) floor(x1) + 1; Y1 = (int) floor(y1) + 1;
                            if (X0 < 0) X0 = 0; if (Y0 < 0) Y0 = 0; if (X1 > 319) X1 = 319; if (Y1 > 239) Y1 = 239;
                        }
                        thr = re15_pri_bucket_of_vz((long) vzmax);
                        if (tmin < thr) {
                            for (yy = Y0; yy <= Y1; yy++)
                                for (xx = X0; xx <= X1; xx++)
                                    if (welch[yy][xx] >= 0 && tief[yy][xx] < dmin) dmin = tief[yy][xx];
                            if (dmin < thr) g_sp_gekappt_offen++;
                        }
                    }
                    if (g_raster_halb_vz > 64 && runde == 0) {
                        /* Spalten, deren eines Ende hinter der Kamera liegt (vz <= 64, wie proj), fehlen im Raster. Schranke:
                         * keine Maske im ganzen Bild mit depth < bucket(groesstes vz dieser Spalten). */
                        g_sp_halb++;
                        if (tmin < re15_pri_bucket_of_vz((long) g_raster_halb_vz)) g_sp_halb_offen++;
                    }
                    for (i = 0; i < np; i++) {
                        int px_ = pt[i].x, py_ = pt[i].y, mi;
                        if (px_ < 0 || px_ >= 320 || py_ < 0 || py_ >= 240) continue;
                        mi = welch[py_][px_];
                        if (mi < 0 || !re15_pri_mask_occludes(tief[py_][px_], (long) pt[i].vz)) continue;
                        {
                            uint8_t *d = (davor && si <= n_steh) ? &davor[(si - 1) * SP_MAX + pt[i].spalte] : NULL, dv;
                            if (d && *d) dv = *d;
                            else {
                                double o = spalte_o(pt[i].spalte, nsp);
                                dv = zelle_dazwischen(&R->zel, b, cam[0], cam[2], wx + o * lx, wz + o * lz) ? 1 : 2;
                                if (d) *d = dv;
                            }
                            if (dv == 1) E->echt++;
                            else { E->falsch++; falsch_m[mi]++; if (si <= n_steh) nochmal[si - 1] = 1; }
                        }
                    }
                }
        }
        if (!iterieren) break;
        for (m = 0; m < pri->draw_count; m++) if (!E->aus[m] && falsch_m[m]) { E->aus[m] = 1; neu++; }
        if (!neu) break;
    }
    free(davor); free(nochmal);
    /* Nachbesserung 2 (Abnahme 1 M1): Sektion OHNE Stehplatz -> alle Rechtecke aus. Dort steht keine Figur, fuer die die
     * Generator-Masken da sind; was sie treffen, ist etwas anderes (ROOM3050/3051 Cut 15: der Konsolen-Cursor, Objekt 3,
     * sub11..14 Work_set, liegt VOR dem Monitorbild - an der exe 60 Punkte seines Innenrands uebermalt). Das Original hat
     * fuer diese Cuts KEINE Masken: Kamera-Eintrag Cut 15 ROOM3050.RDT +0x25C -> +0x84C = FF FF FF FF = NULL-Sektion
     * (@0x80039324 lw / @0x80039328-2c Vergleich mit -1 / @0x80039338 sb zero: keine Masken). Ganz aus = Original. */
    if (iterieren && !E->stehplaetze)
        for (m = 0; m < pri->draw_count; m++) E->aus[m] = 1;
    E->n_aus = 0;
    for (m = 0; m < pri->draw_count; m++) E->n_aus += E->aus[m] ? 1 : 0;
}

/* Fuer jede MSK-Datei und jede Sektion OHNE eigenen Atlas (BSS-Atlas, Generator) den Ausschluss rechnen.
 * fn(raum, cut, pri, E) bekommt das Ergebnis. */
typedef void (*sektion_fn)(unsigned raum, int cut, const re15_pri_cut_t *pri, const ergebnis_t *E, void *ctx);

static void alle_sektionen(int iterieren, int mit_tabelle, sektion_fn fn, void *ctx)
{
    static raum_t R;
    int stage, rr, var;
    for (stage = 1; stage <= 6; stage++)
        for (rr = 0; rr < 0x40; rr++)
            for (var = 0; var < 2; var++) {
                unsigned raum = ((unsigned) stage << 12) | ((unsigned) rr << 4) | (unsigned) var;
                char p[700]; uint8_t *msk; size_t mn = 0; int c, ncut, geladen = 0;
                snprintf(p, sizeof p, "%s/MASKS/ROOM%04X.MSK", PSX, raum);
                msk = slurp(p, &mn);
                if (!msk) continue;
                ncut = (mn >= 12) ? (int) (msk[8] | (msk[9] << 8)) : 0;
                for (c = 0; c < ncut; c++) {
                    re15_pri_cut_t pri; atlas_t eig, bss; ergebnis_t E; uint32_t off; int vorgabe[RE15_PRI_MAX_MASKS_PER_CUT], m;
                    off = re15_pri_msk_section_offset(msk, mn, c);
                    if (!off || re15_pri_parse_section(msk, mn, off, &pri) <= 0) continue;
                    if (atlas_eigen(raum, c, &eig)) { free(eig.roh); continue; }     /* Freistellung: nicht Gegenstand */
                    if (!atlas_bss(raum, c, &bss)) continue;
                    if (!geladen) { geladen = raum_laden(&R, raum) ? 1 : -1; }
                    if (geladen < 0 || c >= R.rdt.cut_count) { free(bss.roh); continue; }
                    for (m = 0; m < RE15_PRI_MAX_MASKS_PER_CUT; m++)
                        vorgabe[m] = mit_tabelle ? re15_masken36_ausgeschlossen(re15_pri_masken_raum(raum), c, m) : 0;
                    sektion_pruefen(&R, c, &pri, &bss, iterieren, vorgabe, &E);
                    fn(raum, c, &pri, &E, ctx);
                    free(bss.roh);
                }
                free(R.roh); R.roh = NULL;
                free(msk);
            }
}

/* ---- erzeuge ---------------------------------------------------------------------------------------------------- */
static void erzeuge_zeile(unsigned raum, int cut, const re15_pri_cut_t *pri, const ergebnis_t *E, void *ctx)
{
    FILE *f = (FILE *) ctx; uint32_t bits[4] = {0, 0, 0, 0}; int m;
    if (!E->n_aus) return;
    for (m = 0; m < pri->draw_count && m < 128; m++) if (E->aus[m]) bits[m >> 5] |= 1u << (m & 31);
    fprintf(f, "    { 0x%04X, %2d, %3d, { 0x%08Xu, 0x%08Xu, 0x%08Xu, 0x%08Xu } },   /* %d von %d Masken, Stehplaetze %ld */\n",
            raum, cut, E->n_aus, bits[0], bits[1], bits[2], bits[3], E->n_aus, pri->draw_count, E->stehplaetze);
}

static void teil_erzeuge(const char *pfad)
{
    FILE *f = fopen(pfad, "w");
    if (!f) { printf("  FEHLER: %s nicht schreibbar\n", pfad); g_fail = 1; return; }
    saaten_sammeln();
    fprintf(f, "/* Erzeugt von tests/unit/test_r37_masken36.c (Modus \"erzeuge\") - NICHT von Hand aendern.\n"
               " * Runde 37 Spur P: Generator-Rechtecke (R15M ohne eigenen Atlas), die an einem erreichbaren Stehplatz einen\n"
               " * Figurpunkt ohne solide Zelle davor verdecken. Grundlagen und Messaufbau: Kopf von test_r37_masken36.c,\n"
               " * Dossier analysis/befunde_runde37/P_masken36.md. Spalten: Raum der MSK-Datei, Cut, Zahl, Bitfeld je Rechteck. */\n"
               "static const re15_masken36_zeile_t re15_masken36_zeilen[] = {\n");
    alle_sektionen(1, 0, erzeuge_zeile, f);
    fprintf(f, "    { 0x0000, 0xFF, 0, { 0u, 0u, 0u, 0u } }   /* Ende */\n};\n");
    fclose(f);
    printf("  geschrieben: %s (Saaten %d)\n", pfad, g_saat_n);
    printf("  Figurraster: %ld Kaesten pixelgenau, breitester %.1f Bildpunkte, breiter als %d: %ld (davon Maske koennte "
           "verdecken: %ld), Spalten halb hinter der Kamera: %ld (davon Maske koennte verdecken: %ld)\n", g_sp_kaesten,
           g_sp_breit_max, SP_MAX - 1, g_sp_gekappt, g_sp_gekappt_offen, g_sp_halb, g_sp_halb_offen);
}

/* ---- ausschluss (Riegel) ---------------------------------------------------------------------------------------- */
typedef struct {
    int sektionen, abweichungen, rest_falsch, aus_masken, masken, stehplatz_los, soll_zeilen, zeige_leer, leer_offen;
    long echt_stage[7], falsch_ohne[7];
    unsigned leer_raum[64]; int leer_cut[64];
} ausschluss_ctx;

/* Nachbesserung 1 (M2): Sektionen OHNE Stehplatz sind fuer "keine Fehlverdeckung" leer wahr. Sie stehen deshalb hier
 * einzeln, mit Beleg, und jede weitere faellt durch (vorher 32 stillschweigend, darunter ROOM30A1 Cut 0..5 = Mangel M1).
 *   ROOM3050/3051 Cut 15: keine RVD-Zone schaltet auf Cut 15 (nur Zone 38 15 -> 0 = das Regionsviereck); einzig das
 *   Ereignis sub10 schaltet ihn (ROOM3050.RDT +0x2596 Cut_chg 15, ausgeloest am Pruef-AOT Slot 10, +0x1FB4 Aot_set
 *   Rechteck (-25900,-16900) 1000 x 2000). Die Figur steht dabei ausserhalb des Regionsvierecks (z >= 12500) und wird vom
 *   Akteur-Cull (FUN_8002c18c) nicht gezeichnet.
 * Nachbesserung 2 (Abnahme 1 M1): "keine Figur" heisst NICHT "nichts zu uebermalen" - Cut 15 ist die Monitor-Nahaufnahme
 * der Kran-Konsole, in der der Spieler den Ziel-Cursor steuert (sub11..14 Work_set Objekt 3); die 36 Generator-Masken
 * uebermalten dessen dunkelgruenen Innenrand (Abnahme 1: 60 Punkte, Leon und Elza). Das Original hat fuer den Cut KEINE
 * Masken (Kamera-Eintrag ROOM3050.RDT +0x25C -> +0x84C = FF FF FF FF, NULL-Sektion @0x80039324-@0x80039338). Regel jetzt
 * (sektion_pruefen): Sektion ohne Stehplatz -> ALLE Rechtecke aus (= Original); der Riegel verlangt das unten. */
static const struct { unsigned raum; int cut; } OHNE_STEHPLATZ_BELEGT[] = { { 0x3050, 15 }, { 0x3051, 15 } };

static void pruefe_zeile(unsigned raum, int cut, const re15_pri_cut_t *pri, const ergebnis_t *E, void *vctx)
{
    ausschluss_ctx *x = (ausschluss_ctx *) vctx; int m, diff = 0;
    x->sektionen++;
    x->masken += pri->draw_count;
    if (E->n_aus) x->soll_zeilen++;
    for (m = 0; m < pri->draw_count; m++) {
        int tab = re15_masken36_ausgeschlossen(re15_pri_masken_raum(raum), cut, m);
        if (tab != E->aus[m]) diff++;
        x->aus_masken += tab;
    }
    if (diff) {
        x->abweichungen++;
        printf("  ABWEICHUNG ROOM%04X Cut %d: Nachrechnung %d Rechtecke, Tabelle anders an %d Stellen\n", raum, cut, E->n_aus, diff);
    }
    if (E->falsch) x->rest_falsch++;
    if (!E->stehplaetze) {
        int an = 0;
        for (m = 0; m < pri->draw_count; m++) an += !re15_masken36_ausgeschlossen(re15_pri_masken_raum(raum), cut, m);
        if (x->stehplatz_los < 64) { x->leer_raum[x->stehplatz_los] = raum; x->leer_cut[x->stehplatz_los] = cut; }
        x->stehplatz_los++;
        if (an) x->leer_offen++;
        if (x->zeige_leer) printf("  ohne Stehplatz: ROOM%04X Cut %d (%d Masken, laut Tabelle noch an: %d)\n", raum, cut,
                                  pri->draw_count, an);
    }
    x->echt_stage[(raum >> 12) & 7] += E->echt;
}

static void ohne_zeile(unsigned raum, int cut, const re15_pri_cut_t *pri, const ergebnis_t *E, void *vctx)
{
    ausschluss_ctx *x = (ausschluss_ctx *) vctx;
    (void) cut; (void) pri;
    x->falsch_ohne[(raum >> 12) & 7] += E->falsch;
}

/* Raumpaare Leon xxx0 / Elza xxx1 der Stages 3..6 mit beiden MSK-Dateien: gleiche Tabellenzeilen? (M2) */
static void paare_pruefen(int *paare, int *paare_gleich, int *bytegleich, int *bytegleich_gleich)
{
    int stage, rr, c, i;
    *paare = *paare_gleich = *bytegleich = *bytegleich_gleich = 0;
    for (stage = 3; stage <= 6; stage++)
        for (rr = 0; rr < 0x40; rr++) {
            unsigned l = ((unsigned) stage << 12) | ((unsigned) rr << 4), e = l | 1u;
            char p[700]; uint8_t *ml, *me, *rl, *re_; size_t nml = 0, nme = 0, nrl = 0, nre = 0; int gleich = 1, bg;
            snprintf(p, sizeof p, "%s/MASKS/ROOM%04X.MSK", PSX, l); ml = slurp(p, &nml);
            snprintf(p, sizeof p, "%s/MASKS/ROOM%04X.MSK", PSX, e); me = slurp(p, &nme);
            snprintf(p, sizeof p, "%s/STAGE%u/ROOM%04X.RDT", PSX, (unsigned) stage, l); rl = slurp(p, &nrl);
            snprintf(p, sizeof p, "%s/STAGE%u/ROOM%04X.RDT", PSX, (unsigned) stage, e); re_ = slurp(p, &nre);
            if (ml && me && rl && re_) {
                bg = nml == nme && !memcmp(ml, me, nml) && nrl == nre && !memcmp(rl, re_, nrl);
                for (c = 0; c < 64; c++)
                    for (i = 0; i < 128; i++)
                        if (re15_masken36_ausgeschlossen(l, c, i) != re15_masken36_ausgeschlossen(e, c, i)) gleich = 0;
                (*paare)++; *paare_gleich += gleich;
                if (bg) { (*bytegleich)++; *bytegleich_gleich += gleich; }
                if (!gleich) printf("  Paar ROOM%04X/%04X: Tabellenzeilen verschieden%s\n", l, e, bg ? " - RDT und MSK BYTEGLEICH" : "");
            }
            free(ml); free(me); free(rl); free(re_);
        }
}

static void teil_ausschluss(void)
{
    ausschluss_ctx a, o; int s, stages_echt = 1, nach_sekt, nach_abw, nach_zeilen, i, j, leer_ok = 1, leer_aus, leer_null;
    int paare, paare_gleich, bytegleich, bytegleich_gleich; long gekappt_offen;
    printf("== r37_masken36 ausschluss: Tabelle aus den Dateien nachgerechnet, Fehlverdeckung danach, Verdeckung bleibt ==\n");
    saaten_sammeln();
    memset(&a, 0, sizeof a); memset(&o, 0, sizeof o);
    a.zeige_leer = 1;
    alle_sektionen(1, 0, pruefe_zeile, &a);            /* Nachrechnung (iteriert) gegen die Tabelle */
    printf("  Saaten %d | Sektionen %d, Masken %d, davon ausgeschlossen %d | Abweichungen %d | ohne Stehplatz %d\n",
           g_saat_n, a.sektionen, a.masken, a.aus_masken, a.abweichungen, a.stehplatz_los);
    printf("  Figurraster (Nachbesserung 2): %ld Kaesten pixelgenau, breitester %.1f Bildpunkte, breiter als %d: %ld (davon "
           "Maske koennte verdecken: %ld), Spalten halb hinter der Kamera: %ld (davon Maske koennte verdecken: %ld)\n",
           g_sp_kaesten, g_sp_breit_max, SP_MAX - 1, g_sp_gekappt, g_sp_gekappt_offen, g_sp_halb, g_sp_halb_offen);
    gekappt_offen = g_sp_gekappt_offen + g_sp_halb_offen;
    /* Ergebnis der Nachrechnung sichern (der Lauf mit Tabelle unten vergleicht die Tabelle nur mit sich selbst) */
    nach_sekt = a.sektionen; nach_abw = a.abweichungen; nach_zeilen = a.soll_zeilen;
    leer_aus = a.stehplatz_los > 0 && a.leer_offen == 0;
    {   /* ... und das Original hat fuer diese Cuts keine Masken (NULL-Sektion FF FF FF FF, @0x80039324-@0x80039338) */
        int nb = (int) (sizeof OHNE_STEHPLATZ_BELEGT / sizeof OHNE_STEHPLATZ_BELEGT[0]);
        leer_null = 1;
        for (j = 0; j < nb; j++) {
            static re15_rdt_t rdt; char p[700]; uint8_t *roh; size_t rn = 0; unsigned r = OHNE_STEHPLATZ_BELEGT[j].raum;
            int c = OHNE_STEHPLATZ_BELEGT[j].cut, ok = 0;
            snprintf(p, sizeof p, "%s/STAGE%u/ROOM%04X.RDT", PSX, r >> 12, r);
            roh = slurp(p, &rn);
            if (roh && re15_rdt_parse(roh, rn, &rdt) >= 0 && c < rdt.cut_count && rdt.cuts[c].pri_offset + 4 <= rn) {
                const uint8_t *q = roh + rdt.cuts[c].pri_offset;
                ok = q[0] == 0xFF && q[1] == 0xFF && q[2] == 0xFF && q[3] == 0xFF;
                printf("  ROOM%04X Cut %d: Original-Sektion +0x%X = %02X %02X %02X %02X (%s)\n", r, c,
                       (unsigned) rdt.cuts[c].pri_offset, q[0], q[1], q[2], q[3], ok ? "NULL - keine Masken" : "NICHT NULL");
            }
            if (!ok) leer_null = 0;
            free(roh);
        }
    }
    {   /* Sektionen ohne Stehplatz == belegte Liste (keine mehr, keine weniger) */
        int nb = (int) (sizeof OHNE_STEHPLATZ_BELEGT / sizeof OHNE_STEHPLATZ_BELEGT[0]);
        if (a.stehplatz_los != nb || a.stehplatz_los > 64) leer_ok = 0;
        for (i = 0; i < a.stehplatz_los && i < 64; i++) {
            int bekannt = 0;
            for (j = 0; j < nb; j++)
                if (OHNE_STEHPLATZ_BELEGT[j].raum == a.leer_raum[i] && OHNE_STEHPLATZ_BELEGT[j].cut == a.leer_cut[i]) bekannt = 1;
            if (!bekannt) { leer_ok = 0; printf("  UNBELEGT ohne Stehplatz: ROOM%04X Cut %d\n", a.leer_raum[i], a.leer_cut[i]); }
        }
    }
    paare_pruefen(&paare, &paare_gleich, &bytegleich, &bytegleich_gleich);
    printf("  Raumpaare xxx0/xxx1 mit MSK: %d, davon gleiche Tabellenzeilen %d | RDT+MSK bytegleich %d, davon gleich %d\n",
           paare, paare_gleich, bytegleich, bytegleich_gleich);
    printf("  Tabellenzeilen %d (ohne Endzeile), nachgerechnete Sektionen mit Ausschluss %d\n",
           re15_masken36_zeilen_zahl() - 1, nach_zeilen);
    memset(&a, 0, sizeof a);
    alle_sektionen(0, 1, pruefe_zeile, &a);            /* mit der Engine-Tabelle: keine Fehlverdeckung bleibt */
    alle_sektionen(0, 0, ohne_zeile, &o);              /* Gegenprobe ohne Ausschluss */
    for (s = 3; s <= 6; s++) {
        printf("  Stage %d: mit Ausschluss Figurpunkte HINTER Zellen verdeckt %ld | ohne Ausschluss Fehlverdeckungen %ld\n",
               s, a.echt_stage[s], o.falsch_ohne[s]);
        if (a.echt_stage[s] <= 0) stages_echt = 0;
    }
    CHECK("Ausschluss-Tabelle == Nachrechnung aus MSK/BSS/RDT (Rechteck fuer Rechteck)", nach_sekt > 0 && nach_abw == 0);
    CHECK("keine verwaiste Tabellenzeile (Zeilen == nachgerechnete Sektionen mit Ausschluss)",
          re15_masken36_zeilen_zahl() - 1 == nach_zeilen);
    CHECK("mit der Tabelle verdeckt keine Generator-Maske eine Figur ohne Zelle davor", a.rest_falsch == 0);
    CHECK("Sektionen ohne Stehplatz == belegte Liste (ROOM3050/3051 Cut 15), sonst ist der Check oben dort leer wahr", leer_ok);
    CHECK("NB2 M1: Sektionen ohne Stehplatz sind GANZ ausgeschlossen (Cut 15: Konsolen-Cursor nicht uebermalt)",
          leer_aus && leer_null);
    CHECK("NB2 M2: Figurraster pixelgenau - wo es nicht pixelgenau ist (Kasten breiter als das Bild, Spalte halb hinter der "
          "Kamera), kann keine Maske verdecken", gekappt_offen == 0);
    CHECK("RDT- und MSK-bytegleiche Raumpaare xxx0/xxx1 haben dieselben Tabellenzeilen (M1: 30A0/30A1)",
          bytegleich > 0 && bytegleich_gleich == bytegleich);
    CHECK("jedes Raumpaar mit MSK hat dieselben Tabellenzeilen (Saaten je Raumpaar)", paare > 0 && paare_gleich == paare);
    CHECK("je Stage 3..6 verdecken die Masken weiterhin Figuren HINTER Zellen", stages_echt);
    CHECK("Gegenprobe: ohne Ausschluss gaebe es Fehlverdeckungen (Stage 3, 5)", o.falsch_ohne[3] > 0 && o.falsch_ohne[5] > 0);
}

/* ---- zensus ----------------------------------------------------------------------------------------------------- */
static void teil_zensus(void)
{
    static uint8_t rgb[320 * 240 * 3];
    int stage, rr, var, dateien = 0, sekt = 0, tot = 0, passt = 0, nicht_null = 0, n_eigen = 0, n_bss = 0;
    int s36_dateien = 0, s36_sekt = 0, s36_bss = 0, alt_tot = 0, s36_alt_tot = 0, gegen_n = 0, gegen_faellt = 0;
    double max_mittel_bss = 0, max_mittel_eigen = 0;
    printf("== r37_masken36 zensus: alle MASKS/ROOM*.MSK, Atlas-Quelle nach re15_pri_atlas_quelle + Blit-Pruefung ==\n");
    for (stage = 1; stage <= 6; stage++)
        for (rr = 0; rr < 0x40; rr++)
            for (var = 0; var < 2; var++) {
                unsigned raum = ((unsigned) stage << 12) | ((unsigned) rr << 4) | (unsigned) var;
                char p[700]; uint8_t *msk, *roh; size_t mn = 0, rn = 0; re15_rdt_t rdt; int hat_rdt, c, ncut;
                int d_sekt = 0, d_bss = 0, d_eigen = 0, d_tot = 0;
                snprintf(p, sizeof p, "%s/MASKS/ROOM%04X.MSK", PSX, raum);
                msk = slurp(p, &mn);
                if (!msk) continue;
                dateien++;
                if (stage >= 3) s36_dateien++;
                snprintf(p, sizeof p, "%s/STAGE%u/ROOM%04X.RDT", PSX, raum >> 12, raum);
                roh = slurp(p, &rn);
                hat_rdt = roh && re15_rdt_parse(roh, rn, &rdt) >= 0;
                ncut = (mn >= 12) ? (int) (msk[8] | (msk[9] << 8)) : 0;
                for (c = 0; c < ncut; c++) {
                    re15_pri_cut_t pri, orig; uint32_t off; int n, quelle, eigen_da, ok_blit = 0;
                    atlas_t eig, bss, *wahl; double mi = 0, an = 0;
                    off = re15_pri_msk_section_offset(msk, mn, c);
                    if (!off) continue;
                    n = re15_pri_parse_section(msk, mn, off, &pri);
                    if (n <= 0) continue;
                    sekt++; d_sekt++;
                    if (stage >= 3) s36_sekt++;
                    /* Vorrang des Originals (main.c: R15M nur bei pri_n == 0) */
                    if (!hat_rdt || c >= rdt.cut_count
                        || re15_pri_parse_section(roh, rn, rdt.cuts[c].pri_offset, &orig) > 0) {
                        nicht_null++;
                        printf("  ROOM%04X Cut %d: Original-Sektion nicht NULL (R15M wird nie gelesen)\n", raum, c);
                        continue;
                    }
                    eigen_da = atlas_eigen(raum, c, &eig);
                    atlas_bss(raum, c, &bss);
                    if (!eigen_da) { alt_tot++; if (stage >= 3) s36_alt_tot++; }   /* alte Regel 53741fe5: tot */
                    quelle = re15_pri_atlas_quelle(1, eigen_da);
                    wahl = (quelle == RE15_PRI_ATLAS_EIGEN) ? &eig : &bss;
                    if (quelle == RE15_PRI_ATLAS_EIGEN) { n_eigen++; d_eigen++; }
                    else { n_bss++; d_bss++; if (stage >= 3) s36_bss++; }
                    if (!wahl->da) {
                        tot++; d_tot++;
                        printf("  TOT: ROOM%04X Cut %d (%d Masken): Atlas-Quelle %s fehlt\n", raum, c, pri.draw_count,
                               quelle == RE15_PRI_ATLAS_EIGEN ? "eigen" : "BSS");
                    } else {
                        size_t bn = 0; uint8_t *blk = bg_block(raum, c, &bn);
                        if (bg_rgb(blk, bn, rgb)) {
                            int gm = blit(&pri, wahl, rgb, &mi, &an);
                            ok_blit = gm > 0 && mi <= MITTEL_MAX && an <= ANTEIL_MAX;
                            if (quelle == RE15_PRI_ATLAS_EIGEN) { if (mi > max_mittel_eigen) max_mittel_eigen = mi; }
                            else if (mi > max_mittel_bss) max_mittel_bss = mi;
                            if (!ok_blit)
                                printf("  BLIT FAELLT: ROOM%04X Cut %d Atlas %s: %d Punkte, Mittel %.2f, Anteil>48 %.1f %%\n",
                                       raum, c, wahl->quelle, gm, mi, 100.0 * an);
                            if (eig.da && bss.da) {                       /* Gegenprobe: der andere Atlas faellt durch */
                                double mi2 = 0, an2 = 0;
                                const atlas_t *anders = (wahl == &eig) ? &bss : &eig;
                                int gm2 = blit(&pri, anders, rgb, &mi2, &an2);
                                gegen_n++;
                                if (!(gm2 > 0 && mi2 <= MITTEL_MAX && an2 <= ANTEIL_MAX)) gegen_faellt++;
                                printf("  Gegenprobe ROOM%04X Cut %d: gewaehlt %s Mittel %.2f / anderer Atlas %s Mittel %.2f, "
                                       "Anteil>48 %.1f %%\n", raum, c, wahl->quelle, mi, anders->quelle, mi2, 100.0 * an2);
                            }
                        } else {
                            printf("  ROOM%04X Cut %d: Hintergrund nicht dekodierbar\n", raum, c);
                        }
                        free(blk);
                        if (ok_blit) passt++;
                    }
                    free(eig.roh); free(bss.roh);
                }
                printf("  ROOM%04X: %d Sektionen, Atlas eigen %d / BSS %d, tot %d\n", raum, d_sekt, d_eigen, d_bss, d_tot);
                free(msk); free(roh);
            }
    printf("  %d Dateien, %d Sektionen: eigen %d, BSS %d, tot %d, Blit passt %d, Original nicht NULL %d\n",
           dateien, sekt, n_eigen, n_bss, tot, passt, nicht_null);
    printf("  Blit Mittel max: BSS %.2f, eigen %.2f (Grenze %.0f)\n", max_mittel_bss, max_mittel_eigen, MITTEL_MAX);
    printf("  alte Regel (53741fe5, R15M nur mit eigenem Atlas): %d Sektionen tot, davon Stages 3..6 %d\n", alt_tot, s36_alt_tot);
    printf("  Stages 3..6: %d Dateien, %d Sektionen, BSS-Atlas %d\n", s36_dateien, s36_sekt, s36_bss);
    CHECK("jede R15M-Sektion steht auf einer NULL-Sektion des Originals", nicht_null == 0);
    CHECK("kein toter Cut: jede R15M-Sektion hat ihren Atlas (eigener oder BSS)", sekt > 0 && tot == 0);
    CHECK("Blit-Pruefung: jede Sektion passt zu dem Atlas, den der Lader waehlt", passt == sekt);
    CHECK("Stages 3..6: 30 Dateien, 216 Sektionen, alle mit dem BSS-Atlas",
          s36_dateien == 30 && s36_sekt == 216 && s36_bss == 216);
    CHECK("Gegenprobe alte Regel: die 216 Stage-3..6-Sektionen waeren ohne eigenen Atlas tot", s36_alt_tot == 216);
    CHECK("Gegenprobe Trennschaerfe: wo beide Atlanten vorliegen, faellt der falsche durch (>= 5 Sektionen)",
          gegen_n >= 5 && gegen_faellt == gegen_n);
}

/* ---- verdeckung: die an der exe gemessenen Szenen (Dossier §4) -------------------------------------------------- */
typedef struct { const char *name; unsigned raum; int cut; int x, z, band; int hinter; } szene_t;
#include "test_r37_masken36_szenen.inc"

static void teil_verdeckung(void)
{
    static uint8_t deck[240][320];
    static uint16_t tief[240][320];
    static figpt_t pt[FIGPT_MAX];
    int i, n_ok = 0, n = (int) (sizeof SZENEN / sizeof SZENEN[0]), np = 0, nsp = 9, ges = 0, n_kf = 0, kopf_alt_30A0 = -1;
    int stage_h[8] = {0}, stage_v[8] = {0};
    printf("== r37_masken36 verdeckung: Figur hinter / vor dem Objekt (Lader-Atlas, nach dem Ausschluss) ==\n");
    for (i = 0; i < n; i++) {
        const szene_t *s = &SZENEN[i];
        char p[700]; uint8_t *roh, *msk; size_t rn = 0, mn = 0; re15_rdt_t rdt; re15_camera_view_t v;
        re15_pri_cut_t pri; atlas_t eig, bss, *a; uint32_t off; int m, x, y, k, on = 0, cov = 0, occ = 0, fuss_occ = 0;
        int fuss_cov = 0, ok, durch, occ_roh = 0;
        double lx, lz, ll;
        unsigned mraum = re15_pri_masken_raum(s->raum);
        snprintf(p, sizeof p, "%s/STAGE%u/ROOM%04X.RDT", PSX, s->raum >> 12, s->raum);
        roh = slurp(p, &rn);
        snprintf(p, sizeof p, "%s/MASKS/ROOM%04X.MSK", PSX, mraum);
        msk = slurp(p, &mn);
        if (!roh || !msk || re15_rdt_parse(roh, rn, &rdt) < 0 || s->cut >= rdt.cut_count
            || re15_camera_build_view(&rdt.cuts[s->cut], &v) != 0
            || !(off = re15_pri_msk_section_offset(msk, mn, s->cut)) || re15_pri_parse_section(msk, mn, off, &pri) <= 0) {
            printf("  FEHLER %s: Daten fehlen\n", s->name); g_fail = 1; free(roh); free(msk); continue;
        }
        atlas_eigen(mraum, s->cut, &eig);
        atlas_bss(s->raum, s->cut, &bss);
        a = (re15_pri_atlas_quelle(1, eig.da) == RE15_PRI_ATLAS_EIGEN) ? &eig : &bss;
        lx = v.rot[0] / 4096.0; lz = v.rot[2] / 4096.0; ll = sqrt(lx * lx + lz * lz);
        if (ll > 0) { lx /= ll; lz /= ll; }
        /* FREI (Nachbesserung 1): erst UNGEFILTERT zaehlen - die Szene muss ohne den Ausschluss fehlverdeckt sein, sonst
         * prueft sie den Ausschluss nicht (Gegenprobe). Danach wie der Lader (main.c) gefiltert. */
        for (durch = (s->hinter == 2) ? 0 : 1; durch <= 1; durch++) {
        if (durch == 1) re15_masken36_filter(mraum, s->cut, &pri);  /* wie der Lader (main.c) */
        on = cov = occ = fuss_occ = fuss_cov = 0;
        memset(deck, 0, sizeof deck); memset(tief, 0, sizeof tief);
        if (a->da) {
            const uint8_t *idx = (const uint8_t *) a->tim.pixels;
            for (m = 0; m < pri.draw_count; m++) {
                const re15_pri_mask_t *q = &pri.masks[m];
                int dx = (int16_t) q->dstX, dy = (int16_t) q->dstY;
                for (y = 0; y < q->height; y++)
                    for (x = 0; x < q->width; x++) {
                        int ax = q->srcX + x, ay = q->srcY + y, tx = dx + x, ty = dy + y;
                        if (ax >= a->tim.width || ay >= a->tim.height || tx < 0 || ty < 0 || tx >= 320 || ty >= 240) continue;
                        if (!idx[ay * a->tim.width + ax]) continue;
                        deck[ty][tx] = 1;
                        if (!tief[ty][tx] || q->depth < tief[ty][tx]) tief[ty][tx] = q->depth;
                    }
            }
        }
        np = figur_raster(&v, s->x, s->z, s->band, lx, lz, pt, &nsp, &ges);
        for (k = 0; k < np; k++) {
            int sx = pt[k].x, sy = pt[k].y;
            if (sx < 0 || sy < 0 || sx >= 320 || sy >= 240) continue;
            on++;
            if (!deck[sy][sx]) continue;
            cov++;
            if (pt[k].fuss) fuss_cov = 1;
            if (re15_pri_mask_occludes(tief[sy][sx], (long) pt[k].vz)) { occ++; if (pt[k].fuss) fuss_occ = 1; }
        }
        if (durch == 0) occ_roh = occ;
        }
        {   /* NB2 M2: das Raster tastet Kopfoberkante und Fuss ab - die mittlere Spalte hat Punkte in den Bildzeilen
             * floor(y_kopf) und floor(y_fuss) (soweit im Bild). Gegenprobe: die alte Abtastung (Zeilenmitten 167..2833)
             * verfehlt in f30A0c0k die Zeile der Kopfoberkante. */
            double fx, fy, fz, kx, ky, kz; int j2, hat_k = 0, hat_f = 0, hat_k_alt = 0, mitte = (nsp - 1) / 2, ik, iff;
            if (proj(&v, s->x, -(double) s->band * BAND_Y, s->z, &fx, &fy, &fz)
                && proj(&v, s->x, -(double) s->band * BAND_Y - FIGUR_HOCH, s->z, &kx, &ky, &kz)) {
                ik = (int) floor(ky); iff = (int) floor(fy);
                for (j2 = 0; j2 < np; j2++)
                    if (pt[j2].spalte == mitte) { hat_k |= pt[j2].y == ik; hat_f |= pt[j2].y == iff; }
                if ((hat_k || ik < 0 || ik > 239) && (hat_f || iff < 0 || iff > 239)) n_kf++;
                else printf("  %s: Raster ohne Kopf-/Fusszeile (Kopf %d %s, Fuss %d %s)\n", s->name, ik, hat_k ? "ja" : "NEIN",
                            iff, hat_f ? "ja" : "NEIN");
                if (!strcmp(s->name, "f30A0c0k")) {
                    int na, alt = g_fz_alt, nsp2, ges2;
                    g_fz_alt = 1;
                    na = figur_raster(&v, s->x, s->z, s->band, lx, lz, pt, &nsp2, &ges2);
                    g_fz_alt = alt;
                    for (j2 = 0; j2 < na; j2++) if (pt[j2].spalte == 4 && pt[j2].y == ik) hat_k_alt = 1;
                    kopf_alt_30A0 = hat_k_alt ? 1 : 0;
                    printf("  %s: Kopfoberkante Bildzeile %d - Raster neu %s, alte Abtastung %s\n", s->name, ik,
                           hat_k ? "ja" : "NEIN", hat_k_alt ? "ja" : "nein (Mangel M2)");
                }
            }
        }
        if (s->hinter == 1)      ok = a->da && fuss_occ && occ * 2 >= on && on > 0;
        else if (s->hinter == 0) ok = a->da && on > 0 && !fuss_cov && cov >= 10 && occ == 0;   /* cov: Punkte auf der Maske */
        else                     ok = a->da && on > 0 && occ == 0 && occ_roh > 0;   /* FREI: ungefiltert verdeckt, jetzt nicht */
        printf("  %s ROOM%04X Cut %d (%d,%d) %s: Atlas %s, im Bild %d/%d Figurpunkten, auf Maske %d, verdeckt %d, Fuss %s",
               s->name, s->raum, s->cut, s->x, s->z, s->hinter == 1 ? "HINTER" : (s->hinter == 0 ? "DAVOR" : "FREI"),
               a->da ? a->quelle : "FEHLT", on, ges, cov,
               occ, fuss_occ ? "verdeckt" : (fuss_cov ? "auf Maske" : "frei"));
        if (s->hinter == 2) printf(" | ungefiltert verdeckt %d", occ_roh);
        printf(" -> %s\n", ok ? "ok" : "FALSCH");
        if (ok) { n_ok++; if (s->hinter == 1) stage_h[s->raum >> 12]++; else if (s->hinter == 0) stage_v[s->raum >> 12]++; }
        free(eig.roh); free(bss.roh); free(roh); free(msk);
    }
    CHECK("alle Szenen wie an der exe gemessen", n > 0 && n_ok == n);
    CHECK("NB2 M2: das Figurraster tastet in jeder Szene Kopfoberkante und Fuss ab (mittlere Spalte)", n_kf == n);
    CHECK("NB2 M2 Gegenprobe: die alte Abtastung (Zeilenmitten) verfehlt in f30A0c0k die Kopfoberkante", kopf_alt_30A0 == 0);
    CHECK("je Stage 3..6 mindestens eine Szene HINTER einem Objekt (verdeckt)",
          stage_h[3] > 0 && stage_h[4] > 0 && stage_h[5] > 0 && stage_h[6] > 0);
    CHECK("je Stage 3..6 mindestens eine Szene DAVOR (nicht verdeckt)",
          stage_v[3] > 0 && stage_v[4] > 0 && stage_v[5] > 0 && stage_v[6] > 0);
}

/* ---- suche (Werkzeug, kein Riegel): Szenen je Cut nach dem Ausschluss ---------------------------------------------- */
static int g_such_prozent = 60;   /* HINTER: Fuss verdeckt und >= g_such_prozent % der Figurpunkte (Aufruf: suche [prozent] [stage]) */
static int g_such_stage;          /* 0 = Stages 3..6, sonst nur diese Stage */
static void such_zeile(unsigned raum, int cut, const re15_pri_cut_t *pri0, const ergebnis_t *E0, void *ctx)
{
    static raum_t R2; static uint8_t deck[240][320]; static uint16_t tief[240][320]; static figpt_t pt[FIGPT_MAX];
    int np, nsp, ges;
    re15_pri_cut_t pri = *pri0; atlas_t bss; re15_camera_view_t v; double cam[3], lx, lz, ll; int16_t qx[4], qz[4];
    int has_q, m, x, y, k, b, gx, gz, bh = -1, bv = -1, hx = 0, hz = 0, hb = 0, vx = 0, vz2 = 0, vb = 0;
    (void) E0; (void) ctx;
    if ((raum & 1u) || (raum >> 12) < 3 || (g_such_stage && (int) (raum >> 12) != g_such_stage)) return;
    re15_masken36_filter(raum, cut, &pri);
    if (!atlas_bss(raum, cut, &bss)) return;
    if (!raum_laden(&R2, raum) || re15_camera_build_view(&R2.rdt.cuts[cut], &v) != 0) { free(bss.roh); free(R2.roh); return; }
    kamera_ort(&v, cam);
    has_q = re15_rdt_get_region_quad(&R2.rdt, cut, qx, qz);
    memset(deck, 0, sizeof deck); memset(tief, 0, sizeof tief);
    {
        const uint8_t *idx = (const uint8_t *) bss.tim.pixels;
        for (m = 0; m < pri.draw_count; m++) {
            const re15_pri_mask_t *q = &pri.masks[m];
            int dx = (int16_t) q->dstX, dy = (int16_t) q->dstY;
            for (y = 0; y < q->height; y++)
                for (x = 0; x < q->width; x++) {
                    int ax = q->srcX + x, ay = q->srcY + y, tx = dx + x, ty = dy + y;
                    if (ax >= bss.tim.width || ay >= bss.tim.height || tx < 0 || ty < 0 || tx >= 320 || ty >= 240) continue;
                    if (!idx[ay * bss.tim.width + ax]) continue;
                    deck[ty][tx] = 1;
                    if (!tief[ty][tx] || q->depth < tief[ty][tx]) tief[ty][tx] = q->depth;
                }
        }
    }
    lx = v.rot[0] / 4096.0; lz = v.rot[2] / 4096.0; ll = sqrt(lx * lx + lz * lz);
    if (ll > 0) { lx /= ll; lz /= ll; }
    for (b = 0; b < 16; b++)
        for (gx = 0; gx < R2.nx; gx++)
            for (gz = 0; gz < R2.nz; gz++) {
                int wx = R2.x0 + gx * SCHRITT, wz = R2.z0 + gz * SCHRITT, on = 0, cov = 0, occ = 0, fo = 0, fc = 0, hp;
                double f1x, f1y, f1z, h1x, h1y, h1z;
                if (!R2.err[b][gx][gz] || (has_q && !re15_aot_point_in_quad(wx, wz, qx, qz))) continue;
                if (!proj(&v, wx, -(double) b * BAND_Y, wz, &f1x, &f1y, &f1z) ||
                    !proj(&v, wx, -(double) b * BAND_Y - FIGUR_HOCH, wz, &h1x, &h1y, &h1z)) continue;
                hp = (int) (f1y - h1y);
                np = figur_raster(&v, wx, wz, b, lx, lz, pt, &nsp, &ges);
                for (k = 0; k < np; k++) {
                    int sx = pt[k].x, sy = pt[k].y;
                    if (sx < 0 || sy < 0 || sx >= 320 || sy >= 240) continue;
                    on++;
                    if (!deck[sy][sx]) continue;
                    cov++; if (pt[k].fuss) fc = 1;
                    if (re15_pri_mask_occludes(tief[sy][sx], (long) pt[k].vz)) { occ++; if (pt[k].fuss) fo = 1; }
                }
                if (on * 9 < ges * 8) continue;   /* >= 8/9 der Figurpunkte im Bild (vorher 72 von 81) */
                if (fo && occ * 100 >= on * g_such_prozent && hp * 1000 + occ > bh) { bh = hp * 1000 + occ; hx = wx; hz = wz; hb = b; }
                if (!fc && occ == 0 && cov >= 20 && hp * 1000 + cov > bv) { bv = hp * 1000 + cov; vx = wx; vz2 = wz; vb = b; }
            }
    printf("  ROOM%04X Cut %2d: %2d Masken nach Ausschluss | HINTER %s", raum, cut, pri.draw_count, bh >= 0 ? "" : "-");
    if (bh >= 0) printf("(%d,%d) Band %d, %d px, %d verdeckt", hx, hz, hb, bh / 1000, bh % 1000);
    printf(" | DAVOR %s", bv >= 0 ? "" : "-");
    if (bv >= 0) printf("(%d,%d) Band %d, %d px, %d auf Maske", vx, vz2, vb, bv / 1000, bv % 1000);
    printf("\n");
    free(bss.roh); free(R2.roh); R2.roh = NULL;
}

static figpt_t g_pt[FIGPT_MAX];   /* Werkzeug fehl */

/* Werkzeug punkt: eine beliebige Lage (nicht nur Gitterpunkte) - je Figurpunkt die vorderste Maske, ob sie verdeckt und ob
 * eine Zelle davor steht, ungefiltert und mit der Tabelle (wie der Lader). */
static int teil_punkt(unsigned r, int c, int x, int z, int b)
{
    static raum_t R; static int16_t welch[2][240][320]; static uint16_t tief[2][240][320]; static figpt_t pt[FIGPT_MAX];
    int np, nsp, ges;
    re15_camera_view_t v; double cam[3], lx, lz, ll; char p[700]; uint8_t *msk; size_t mn = 0;
    re15_pri_cut_t pri; atlas_t bss; uint32_t off; const uint8_t *idx; int m, k, xx, yy, g, f[2] = {0, 0}, s = 0;
    saaten_sammeln();
    snprintf(p, sizeof p, "%s/MASKS/ROOM%04X.MSK", PSX, re15_pri_masken_raum(r));
    msk = slurp(p, &mn);
    if (!msk || !(off = re15_pri_msk_section_offset(msk, mn, c)) || re15_pri_parse_section(msk, mn, off, &pri) <= 0
        || !atlas_bss(r, c, &bss) || !raum_laden(&R, r) || re15_camera_build_view(&R.rdt.cuts[c], &v) != 0) {
        printf("FEHLER\n"); return 1;
    }
    idx = (const uint8_t *) bss.tim.pixels;
    kamera_ort(&v, cam);
    lx = v.rot[0] / 4096.0; lz = v.rot[2] / 4096.0; ll = sqrt(lx * lx + lz * lz);
    if (ll > 0) { lx /= ll; lz /= ll; }
    for (g = 0; g < 2; g++) {
        for (yy = 0; yy < 240; yy++) for (xx = 0; xx < 320; xx++) { welch[g][yy][xx] = -1; tief[g][yy][xx] = 0xFFFF; }
        for (m = 0; m < pri.draw_count; m++) {
            const re15_pri_mask_t *q = &pri.masks[m]; int dx = (int16_t) q->dstX, dy = (int16_t) q->dstY, u, w;
            if (g == 1 && re15_masken36_ausgeschlossen(re15_pri_masken_raum(r), c, m)) continue;
            for (w = 0; w < q->height; w++)
                for (u = 0; u < q->width; u++) {
                    int ax = q->srcX + u, ay = q->srcY + w, tx = dx + u, ty = dy + w;
                    if (ax >= bss.tim.width || ay >= bss.tim.height || tx < 0 || ty < 0 || tx >= 320 || ty >= 240) continue;
                    if (!idx[ay * bss.tim.width + ax]) continue;
                    if (q->depth < tief[g][ty][tx]) { tief[g][ty][tx] = q->depth; welch[g][ty][tx] = (int16_t) m; }
                }
        }
    }
    np = figur_raster(&v, x, z, b, lx, lz, pt, &nsp, &ges);
    printf("  ROOM%04X Cut %d (%d,%d) Band %d: Figur %s, %d Spalten, %d Punkte, %d Masken\n", r, c, x, z, b,
           g_fz_alt ? "9 x 9 Zeilenmitten (alt)" : (g_fz ? "festes Gitter" : "pixelgenau"), nsp, ges, pri.draw_count);
    for (k = 0; k < np; k++) {
        int sx = pt[k].x, sy = pt[k].y, zd; double o = spalte_o(pt[k].spalte, nsp);
        if (sx < 0 || sy < 0 || sx >= 320 || sy >= 240) continue;
        s++;
        zd = zelle_dazwischen(&R.zel, b, cam[0], cam[2], x + o * lx, z + o * lz);
        for (g = 0; g < 2; g++) {
            int mi = welch[g][sy][sx], occ = mi >= 0 && re15_pri_mask_occludes(tief[g][sy][sx], (long) pt[k].vz);
            if (occ && !zd) {
                f[g]++;
                printf("    %s Spalte %3d (%d,%d) vz %.0f Maske %d Tiefe %u Stufe %ld VERDECKT frei%s\n", g ? "Tabelle   " : "ungefiltert",
                       pt[k].spalte, sx, sy, pt[k].vz, mi, (unsigned) tief[g][sy][sx], (1023L * (long) pt[k].vz) >> 16,
                       (!g && re15_masken36_ausgeschlossen(re15_pri_masken_raum(r), c, mi)) ? " (Tabelle: aus)" : "");
            }
        }
    }
    printf("  im Bild %d von %d Figurpunkten | ohne Zelle davor verdeckt: ungefiltert %d, mit Tabelle %d\nOK\n", s, ges, f[0], f[1]);
    free(bss.roh); free(msk);
    return 0;
}

int main(int argc, char **argv)
{
    const char *teil;
    {   /* Werkzeug-Optionen (kein Riegel): --zeilen=N Figurzeilen, --alt alte Abtastung (Zeilenmitten, Nachbesserung 2) */
        int i, j = 1;
        for (i = 1; i < argc; i++) {
            if (!strcmp(argv[i], "--alt")) g_fz_alt = 1;
            else if (!strncmp(argv[i], "--zeilen=", 9) && atoi(argv[i] + 9) >= 2) g_fz = atoi(argv[i] + 9);
            else if (!strncmp(argv[i], "--spalten=", 10) && atoi(argv[i] + 10) >= 3) {
                g_fs = atoi(argv[i] + 10) | 1;
                if (g_fs > SP_MAX) g_fs = SP_MAX;
            }
            else argv[j++] = argv[i];
        }
        argc = j;
    }
    teil = (argc > 1) ? argv[1] : "alle";
    if (!strcmp(teil, "punkt") && argc > 5)
        return teil_punkt((unsigned) strtoul(argv[2], NULL, 16), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]),
                          argc > 6 ? atoi(argv[6]) : 0);
    if (!strcmp(teil, "saaten") && argc > 2) {          /* Werkzeug: Saaten eines Raums mit Herkunft */
        unsigned r = (unsigned) strtoul(argv[2], NULL, 16); int i, na = 0, nt = 0, ns = 0;
        saaten_sammeln();
        for (i = 0; i < g_saat_n; i++) {
            if (g_saat[i].raum != r) continue;
            na += g_saat[i].art == 'A'; nt += g_saat[i].art == 'T'; ns += g_saat[i].art == 'S';
            printf("  %c (%d,%d) Band %d von ROOM%04X +0x%X\n", g_saat[i].art, g_saat[i].x, g_saat[i].z, g_saat[i].band,
                   g_saat[i].von, (unsigned) g_saat[i].off);
        }
        printf("  ROOM%04X: AOT %d, Tuer (VM) %d, Tuer (Satz) %d | alle Saaten %d\nOK\n", r, na, nt, ns, g_saat_n);
        return 0;
    }
    if (!strcmp(teil, "soll")) {                        /* Werkzeug: Maskenzahl je Generator-Sektion nach dem Ausschluss (exe-Liste) */
        int stage, rr, var, c;
        for (stage = 3; stage <= 6; stage++)
            for (rr = 0; rr < 0x40; rr++)
                for (var = 0; var < 2; var++) {
                    unsigned raum = ((unsigned) stage << 12) | ((unsigned) rr << 4) | (unsigned) var;
                    char p[700]; uint8_t *msk; size_t mn = 0; int ncut;
                    snprintf(p, sizeof p, "%s/MASKS/ROOM%04X.MSK", PSX, raum);
                    msk = slurp(p, &mn);
                    if (!msk) continue;
                    ncut = (mn >= 12) ? (int) (msk[8] | (msk[9] << 8)) : 0;
                    for (c = 0; c < ncut; c++) {
                        re15_pri_cut_t pri; atlas_t eig; uint32_t off = re15_pri_msk_section_offset(msk, mn, c); int roh;
                        if (!off || re15_pri_parse_section(msk, mn, off, &pri) <= 0) continue;
                        if (atlas_eigen(raum, c, &eig)) { free(eig.roh); continue; }
                        roh = pri.draw_count;
                        re15_masken36_filter(re15_pri_masken_raum(raum), c, &pri);
                        printf("SOLL %04X %d %d %d\n", raum, c, roh, pri.draw_count);
                    }
                    free(msk);
                }
        return 0;
    }
    if (!strcmp(teil, "proj") && argc > 5) {            /* Werkzeug: Figurfenster (Projektion Hoehe 3000 / +-350, +4) fuer die exe */
        static re15_rdt_t rdt; re15_camera_view_t v; char p[700]; uint8_t *roh; size_t rn = 0;
        unsigned r = (unsigned) strtoul(argv[2], NULL, 16); int c = atoi(argv[3]), x = atoi(argv[4]), z = atoi(argv[5]);
        int b = argc > 6 ? atoi(argv[6]) : 0, k; double lx, lz, ll, x0 = 1e9, y0 = 1e9, x1 = -1e9, y1 = -1e9;
        snprintf(p, sizeof p, "%s/STAGE%u/ROOM%04X.RDT", PSX, r >> 12, r);
        roh = slurp(p, &rn);
        if (!roh || re15_rdt_parse(roh, rn, &rdt) < 0 || c >= rdt.cut_count || re15_camera_build_view(&rdt.cuts[c], &v) != 0) {
            printf("FEHLER\n"); return 1;
        }
        lx = v.rot[0] / 4096.0; lz = v.rot[2] / 4096.0; ll = sqrt(lx * lx + lz * lz);
        if (ll > 0) { lx /= ll; lz /= ll; }
        for (k = 0; k < 4; k++) {
            double h = -(double) b * BAND_Y - ((k & 1) ? FIGUR_HOCH : 0.0), o = (k & 2) ? FIGUR_HALB : -FIGUR_HALB, sx, sy, vz;
            if (!proj(&v, x + o * lx, h, z + o * lz, &sx, &sy, &vz)) continue;
            if (sx < x0) x0 = sx; if (sx > x1) x1 = sx; if (sy < y0) y0 = sy; if (sy > y1) y1 = sy;
        }
        printf("FENSTER %d %d %d %d\n", (int) x0 - 4, (int) y0 - 4, (int) x1 + 5, (int) y1 + 5);
        free(roh);
        return 0;
    }
    if (!strcmp(teil, "fehl") && argc > 3) {            /* Werkzeug: Stehplaetze, an denen die UNGEFILTERTEN Masken eine Figur
                                                          * ohne Zelle davor verdecken (meiste Punkte zuerst), fuer die exe */
        static raum_t R; static int16_t welch[240][320]; static uint16_t tief[240][320];
        unsigned r = (unsigned) strtoul(argv[2], NULL, 16); int c = atoi(argv[3]), m, x, y, k, b, gx, gz, has_q, n_out = 0;
        re15_camera_view_t v; double cam[3], lx, lz, ll; int16_t qx[4], qz[4]; char p[700]; uint8_t *msk; size_t mn = 0;
        re15_pri_cut_t pri; atlas_t bss; uint32_t off; const uint8_t *idx;
        int best_f[8] = {0}, best_x[8], best_z[8], best_b[8], best_s[8];
        saaten_sammeln();
        snprintf(p, sizeof p, "%s/MASKS/ROOM%04X.MSK", PSX, re15_pri_masken_raum(r));
        msk = slurp(p, &mn);
        if (!msk || !(off = re15_pri_msk_section_offset(msk, mn, c)) || re15_pri_parse_section(msk, mn, off, &pri) <= 0
            || !atlas_bss(r, c, &bss) || !raum_laden(&R, r) || re15_camera_build_view(&R.rdt.cuts[c], &v) != 0) {
            printf("FEHLER\n"); return 1;
        }
        idx = (const uint8_t *) bss.tim.pixels;
        kamera_ort(&v, cam);
        has_q = re15_rdt_get_region_quad(&R.rdt, c, qx, qz);
        lx = v.rot[0] / 4096.0; lz = v.rot[2] / 4096.0; ll = sqrt(lx * lx + lz * lz);
        if (ll > 0) { lx /= ll; lz /= ll; }
        for (y = 0; y < 240; y++) for (x = 0; x < 320; x++) { welch[y][x] = -1; tief[y][x] = 0xFFFF; }
        for (m = 0; m < pri.draw_count; m++) {
            const re15_pri_mask_t *q = &pri.masks[m]; int dx = (int16_t) q->dstX, dy = (int16_t) q->dstY, xx, yy;
            for (yy = 0; yy < q->height; yy++)
                for (xx = 0; xx < q->width; xx++) {
                    int ax = q->srcX + xx, ay = q->srcY + yy, tx = dx + xx, ty = dy + yy;
                    if (ax >= bss.tim.width || ay >= bss.tim.height || tx < 0 || ty < 0 || tx >= 320 || ty >= 240) continue;
                    if (!idx[ay * bss.tim.width + ax]) continue;
                    if (q->depth < tief[ty][tx]) { tief[ty][tx] = q->depth; welch[ty][tx] = (int16_t) m; }
                }
        }
        for (b = 0; b < 16; b++)
            for (gx = 0; gx < R.nx; gx += 3)
                for (gz = 0; gz < R.nz; gz += 3) {
                    int wx = R.x0 + gx * SCHRITT, wz = R.z0 + gz * SCHRITT, f = 0, s = 0, j;
                    if (!R.err[b][gx][gz] || (has_q && !re15_aot_point_in_quad(wx, wz, qx, qz))) continue;
                    {
                        int np, nsp, ges, sp_zd[SP_MAX];
                        np = figur_raster(&v, wx, wz, b, lx, lz, g_pt, &nsp, &ges);
                        for (k = 0; k < nsp; k++) sp_zd[k] = -1;
                        for (k = 0; k < np; k++) {
                            int sx = g_pt[k].x, sy = g_pt[k].y, mi, j2 = g_pt[k].spalte;
                            if (sx < 0 || sy < 0 || sx >= 320 || sy >= 240) continue;
                            s++;
                            mi = welch[sy][sx];
                            if (mi < 0 || !re15_pri_mask_occludes(tief[sy][sx], (long) g_pt[k].vz)) continue;
                            if (sp_zd[j2] < 0)
                                sp_zd[j2] = zelle_dazwischen(&R.zel, b, cam[0], cam[2], wx + spalte_o(j2, nsp) * lx, wz + spalte_o(j2, nsp) * lz);
                            if (!sp_zd[j2]) {
                                f++;
                                if (argc > 5 && wx == atoi(argv[4]) && wz == atoi(argv[5]))   /* Einzelstelle: je Figurpunkt */
                                    printf("    Spalte %3d (%d,%d) vz %.0f Maske %d Tiefe %u Stufe %ld VERDECKT frei\n", j2, sx, sy,
                                           g_pt[k].vz, mi, (unsigned) tief[sy][sx], (1023L * (long) g_pt[k].vz) >> 16);
                            }
                        }
                        if (s * 9 < ges * 8) continue;
                    }
                    for (j = 0; j < 8; j++)
                        if (f > best_f[j]) {
                            int t;
                            for (t = 7; t > j; t--) { best_f[t] = best_f[t - 1]; best_x[t] = best_x[t - 1]; best_z[t] = best_z[t - 1]; best_b[t] = best_b[t - 1]; best_s[t] = best_s[t - 1]; }
                            best_f[j] = f; best_x[j] = wx; best_z[j] = wz; best_b[j] = b; best_s[j] = s;
                            break;
                        }
                }
        for (k = 0; k < 8; k++)
            if (best_f[k]) { printf("  ROOM%04X Cut %d (%d,%d) Band %d: %d von %d Figurpunkten ohne Zelle davor verdeckt\n", r, c,
                                    best_x[k], best_z[k], best_b[k], best_f[k], best_s[k]); n_out++; }
        printf("  %d Masken ungefiltert, %d Stellen\nOK\n", pri.draw_count, n_out);
        free(bss.roh); free(msk);
        return 0;
    }
    if (!strcmp(teil, "cut") && argc > 3) {             /* Werkzeug: warum hat ein Cut keinen Stehplatz? */
        static raum_t R; unsigned r = (unsigned) strtoul(argv[2], NULL, 16); int c = atoi(argv[3]), i, b, gx, gz;
        int16_t qx[4], qz[4]; int hq;
        saaten_sammeln();
        if (!raum_laden(&R, r) || c >= R.rdt.cut_count) { printf("FEHLER\n"); return 1; }
        hq = re15_rdt_get_region_quad(&R.rdt, c, qx, qz);
        printf("  ROOM%04X Cut %d: Regionsviereck %s (%d,%d) (%d,%d) (%d,%d) (%d,%d), Zonen %d\n", r, c, hq ? "ja" : "nein",
               qx[0], qz[0], qx[1], qz[1], qx[2], qz[2], qx[3], qz[3], R.rdt.zone_count);
        for (i = 0; i < R.rdt.zone_count; i++) {
            const re15_rdt_zone_t *z = &R.rdt.zones[i];
            if (z->cam_from != c && z->cam_to != c) continue;
            printf("    Zone %2d %2d -> %2d Boden 0x%02X (%d,%d) (%d,%d) (%d,%d) (%d,%d)\n", i, z->cam_from, z->cam_to, z->floor,
                   z->xs[0], z->zs[0], z->xs[1], z->zs[1], z->xs[2], z->zs[2], z->xs[3], z->zs[3]);
        }
        for (b = 0; b < 16; b++) {
            long frei_q = 0, err_q = 0, zellen = 0;
            for (i = 0; i < R.rdt.sca_count; i++) zellen += (R.rdt.sca[i].floor >> 4) == b;
            for (gx = 0; gx < R.nx; gx++)
                for (gz = 0; gz < R.nz; gz++) {
                    int wx = R.x0 + gx * SCHRITT, wz = R.z0 + gz * SCHRITT;
                    if (hq && !re15_aot_point_in_quad(wx, wz, qx, qz)) continue;
                    if (frei_g(&R, b, gx, gz) && in_zonen(&R.rdt, b, wx, wz)) frei_q++;
                    if (R.err[b][gx][gz]) err_q++;
                }
            if (zellen || err_q) {
                int typ[16] = {0}, t;
                for (i = 0; i < R.rdt.sca_count; i++)
                    if ((R.rdt.sca[i].floor >> 4) == b && (R.rdt.sca[i].u0 & 1)) typ[R.rdt.sca[i].type & 15]++;
                printf("    Band %2d: SCA-Zellen %ld | im Viereck frei+Zone %ld, erreicht %ld | solide je Typ:", b, zellen, frei_q, err_q);
                for (t = 0; t < 16; t++) if (typ[t]) printf(" %d:%d", t, typ[t]);
                printf("\n");
            }
        }
        printf("OK\n"); return 0;
    }
    if (!strcmp(teil, "suche")) {
        if (argc > 2) g_such_prozent = atoi(argv[2]);
        if (argc > 3) g_such_stage = atoi(argv[3]);
        saaten_sammeln(); alle_sektionen(0, 1, such_zeile, NULL); printf("OK\n"); return 0;
    }
    if (!strcmp(teil, "zensus")) teil_zensus();
    else if (!strcmp(teil, "ausschluss")) teil_ausschluss();
    else if (!strcmp(teil, "verdeckung")) teil_verdeckung();
    else if (!strcmp(teil, "erzeuge") && argc > 2) teil_erzeuge(argv[2]);
    else { teil_zensus(); teil_ausschluss(); teil_verdeckung(); }
    printf(g_fail ? "FAIL\n" : "OK\n");
    return g_fail;
}
