/*
 * Runde 41 Spur E "kleinkram_b", Nachbesserung 1 (Abnahme 0 Hinweis H3): Auswerter fuer die Integrations-Riegel mit der
 * ECHTEN exe (tests/integration/test_r41_kleinkram_b.cmake). Kein Engine-Code, nur Dateien lesen.
 *
 *   marker <map.bmp>                    Kartenschirm-Abzug (RE15_INV_FB_SHOT, 24 bit, 320x240): Mittelpunkt des 8x8-
 *                                       Markers (re15_inv_screen.c sprt uv 224,128, Puls r=g=b, b~0) im Kasten
 *                                       (60,40)-(260,200). Ausgabe "MARKER <x> <y> <texel>".
 *   kurbel <debug.log> <state.txt> <anim.txt>
 *                                       ROOM2020 sub02: Bilder von Pos_set @0x0A8C, Plc_motion @0x0A9C, Plc_ret @0x0B16 aus
 *                                       dem SCD-Trace; zwischen Pos_set und Plc_ret steht der Spieler JEDES Bild bei
 *                                       (3650,-3018) (Pos_set `32 00 42 0e f8 f8 93 f4` + Kollision, Original-RAM nach der
 *                                       Szene ebenso, Abnahme §3.6a); Spieler-Clip 0 (Raum-RBJ, 40 Bilder) laeuft mit
 *                                       Plc_flg(0,4) als SCHLEIFE: >= 3 Umlaeufe 39 -> 0 (Original Periode 40, Abnahme
 *                                       §3.7) und hoechstens 2 Bilder am Stueck auf Slot 39 (vorher 138: Standbild).
 *   segment <state.txt>                 letzter Raum-Abschnitt des State-Logs (Bildzaehler beginnt je Raum neu):
 *                                       "SEGMENT bilder=<n> kameras=<c,..> letzte_kamera=<c> min_x=<x> max_x=<x>".
 * Nachbesserung 2 (Abnahme 1 Maengel 1-3, master-AUFTRAG E-N1/E-N2):
 *   gleich <praefix> <von> <bis>       RE15_FRAMEDUMP-Serie <praefix>NNNNNN.ppm: Zahl der Folgepaare ohne Pixel-
 *                                       unterschied ("GLEICH <gleiche> von <paare>"); 0 = alle gleich (stehendes Bild).
 *   kamwechsel <state.txt> <von> <bis> letzter Raum-Abschnitt: Zahl der cam=-Wechsel im Bildbereich.
 *   takt <debug.log> <state.txt>       Kurbel im GEHEN (ROOM2020 sub02): Geh-Bild unveraendert von Message_on 0
 *                                       @0x0A5C bis Message_on 1 @0x0A62 ([mot]-Spur), Pos_set @0x0A8C im ersten Bild
 *                                       nach dem Schliessen der Frage und im selben Skript-Takt wie Ifel_ck @0x0A68,
 *                                       dieses Bild gehalten ([kamerahalt] HALTEN), Spieler dort (3650,-3018) mo 200.
 * Rueckgabe 0 = bestanden, 1 = durchgefallen, 2 = Eingabefehler.
 */
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXF 20000

static int marker(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) { printf("FEHLER %s nicht lesbar\n", path); return 2; }
    unsigned char h[54];
    if (fread(h, 1, 54, f) != 54 || h[0] != 'B' || h[1] != 'M') { fclose(f); printf("FEHLER kein BMP\n"); return 2; }
    int off = h[10] | (h[11] << 8) | (h[12] << 16) | (h[13] << 24);
    int w = h[18] | (h[19] << 8) | (h[20] << 16) | (h[21] << 24);
    int hh = h[22] | (h[23] << 8) | (h[24] << 16) | (h[25] << 24);
    int bpp = h[28] | (h[29] << 8);
    int bottom_up = hh > 0;
    if (hh < 0) hh = -hh;
    if (bpp != 24 || w <= 0 || hh <= 0 || w > 4096 || hh > 4096) { fclose(f); printf("FEHLER Format %dx%d %d bpp\n", w, hh, bpp); return 2; }
    int stride = (w * 3 + 3) & ~3;
    unsigned char *pix = (unsigned char *)malloc((size_t)stride * (size_t)hh);
    if (!pix) { fclose(f); return 2; }
    fseek(f, off, SEEK_SET);
    size_t got = fread(pix, 1, (size_t)stride * (size_t)hh, f);
    fclose(f);
    if (got != (size_t)stride * (size_t)hh) { free(pix); printf("FEHLER kurz\n"); return 2; }
    int x0 = 1 << 30, y0 = 1 << 30, x1 = -1, y1 = -1, n = 0;
    for (int y = 40; y < 200 && y < hh; y++) {
        int row = bottom_up ? (hh - 1 - y) : y;
        for (int x = 60; x < 260 && x < w; x++) {
            const unsigned char *p = pix + (size_t)row * stride + (size_t)x * 3;   /* B G R */
            int b = p[0], g = p[1], r = p[2];
            if (b <= 0x08 && abs(r - g) <= 0x08 && r >= 0x18) {
                if (x < x0) x0 = x;
                if (x > x1) x1 = x;
                if (y < y0) y0 = y;
                if (y > y1) y1 = y;
                n++;
            }
        }
    }
    free(pix);
    if (n == 0) { printf("MARKER keiner\n"); return 1; }
    printf("MARKER %d %d %d\n", (x0 + x1) / 2, (y0 + y1) / 2, n);
    return 0;
}

/* Bildnummer der LETZTEN SCD-Trace-Zeile "[scd] F<n> slot=<s> @<adr>" (der Zielraum 2020 ist der letzte Raum). */
static long scd_bild(const char *log, const char *adr)
{
    FILE *f = fopen(log, "rb");
    if (!f) return -2;
    char line[1024];
    long res = -1;
    while (fgets(line, sizeof line, f)) {
        long fr; char a[32];
        if (strncmp(line, "[scd] F", 7) != 0) continue;
        if (sscanf(line + 7, "%ld slot=%*d %31s", &fr, a) == 2 && strcmp(a, adr) == 0) res = fr;
    }
    fclose(f);
    return res;
}

static int kurbel(const char *dlog, const char *state, const char *anim)
{
    long f_pos = scd_bild(dlog, "@0x0A8C"), f_mot = scd_bild(dlog, "@0x0A9C"), f_ret = scd_bild(dlog, "@0x0B16");
    printf("SCD Pos_set F%ld Plc_motion F%ld Plc_ret F%ld\n", f_pos, f_mot, f_ret);
    if (f_pos < 0 || f_mot <= f_pos || f_ret <= f_mot || f_ret >= MAXF) { printf("FAIL Szene nicht vollstaendig\n"); return 1; }

    /* state.txt: "F<n> pad=xxxx PL(x,z,rot=..)" — Bildzaehler beginnt je Raum neu, der letzte Eintrag je Bild gilt. */
    static int sx[MAXF], sz[MAXF], sv[MAXF];
    memset(sv, 0, sizeof sv);
    FILE *f = fopen(state, "rb");
    if (!f) { printf("FEHLER %s\n", state); return 2; }
    char line[4096];
    while (fgets(line, sizeof line, f)) {
        long fr; int x, z;
        if (line[0] != 'F') continue;
        const char *pl = strstr(line, "PL(");
        if (!pl || sscanf(line + 1, "%ld", &fr) != 1 || sscanf(pl + 3, "%d,%d", &x, &z) != 2) continue;
        if (fr < 0 || fr >= MAXF) continue;
        sx[fr] = x; sz[fr] = z; sv[fr] = 1;
    }
    fclose(f);
    int falsch = 0, geprueft = 0;
    for (long fr = f_pos + 1; fr < f_ret; fr++) {
        if (!sv[fr]) continue;
        geprueft++;
        if (sx[fr] != 3650 || sz[fr] != -3018) {
            if (falsch < 5) printf("FAIL F%ld Spieler bei (%d,%d) statt (3650,-3018)\n", fr, sx[fr], sz[fr]);
            falsch++;
        }
    }
    printf("Lage: %d Bilder geprueft, %d abweichend\n", geprueft, falsch);

    /* anim.txt (RE15_ANIM_TRACE): "frame actor type motion clip fc cur slot ..." — Aktor 0 = Spieler. */
    static int am[MAXF], ac[MAXF], afc[MAXF], asl[MAXF], av[MAXF];
    memset(av, 0, sizeof av);
    f = fopen(anim, "rb");
    if (!f) { printf("FEHLER %s\n", anim); return 2; }
    while (fgets(line, sizeof line, f)) {
        long fr; int actor, type, motion, clip, fc, slot; unsigned cur;
        if (sscanf(line, "%ld %d %d %d %d %d %u %d", &fr, &actor, &type, &motion, &clip, &fc, &cur, &slot) != 8) continue;
        if (actor != 0 || fr < 0 || fr >= MAXF) continue;
        am[fr] = motion; ac[fr] = clip; afc[fr] = fc; asl[fr] = slot; av[fr] = 1;
    }
    fclose(f);
    int umlaeufe = 0, lauf39 = 0, max39 = 0, kurbelbilder = 0, fc40 = 0;
    for (long fr = f_mot; fr < f_ret; fr++) {
        if (!av[fr] || am[fr] != 0 || ac[fr] != 0) { lauf39 = 0; continue; }
        kurbelbilder++;
        if (afc[fr] == 40) fc40++;
        if (asl[fr] == 39) { lauf39++; if (lauf39 > max39) max39 = lauf39; } else lauf39 = 0;
        if (fr > f_mot && av[fr - 1] && am[fr - 1] == 0 && ac[fr - 1] == 0 && asl[fr - 1] == 39 && asl[fr] == 0) umlaeufe++;
    }
    printf("Kurbel-Clip: %d Bilder (fc 40: %d), Umlaeufe 39->0: %d, laengster Halt auf Slot 39: %d Bilder\n",
           kurbelbilder, fc40, umlaeufe, max39);
    int ok = 1;
    if (falsch > 0 || geprueft < (int)(f_ret - f_pos) / 2) { printf("FAIL Lage\n"); ok = 0; }
    if (kurbelbilder < (int)(f_ret - f_mot) - 2 || fc40 != kurbelbilder) { printf("FAIL Clip 0 (40 Bilder) nicht durchgehend\n"); ok = 0; }
    if (umlaeufe < 3) { printf("FAIL weniger als 3 Umlaeufe\n"); ok = 0; }
    if (max39 > 2) { printf("FAIL Standbild auf Slot 39 (%d Bilder)\n", max39); ok = 0; }
    printf(ok ? "BESTANDEN\n" : "DURCHGEFALLEN\n");
    return ok ? 0 : 1;
}

static int segment(const char *state)
{
    FILE *f = fopen(state, "rb");
    if (!f) { printf("FEHLER %s\n", state); return 2; }
    static long fr_[MAXF * 4]; static int cam_[MAXF * 4], x_[MAXF * 4];
    int n = 0;
    char line[4096];
    while (fgets(line, sizeof line, f) && n < MAXF * 4) {
        long fr; int x, z, c;
        if (line[0] != 'F' || sscanf(line + 1, "%ld", &fr) != 1) continue;
        const char *pl = strstr(line, "PL("), *cm = strstr(line, " cam=");
        if (!pl || !cm || sscanf(pl + 3, "%d,%d", &x, &z) != 2 || sscanf(cm + 5, "%d", &c) != 1) continue;
        fr_[n] = fr; cam_[n] = c; x_[n] = x; n++;
    }
    fclose(f);
    if (n == 0) { printf("SEGMENT leer\n"); return 1; }
    int a = 0;
    for (int i = 1; i < n; i++) if (fr_[i] < fr_[i - 1]) a = i;
    int seen[64] = {0}, mnx = 1 << 30, mxx = -(1 << 30);
    for (int i = a; i < n; i++) {
        if (cam_[i] >= 0 && cam_[i] < 64) seen[cam_[i]] = 1;
        if (x_[i] < mnx) mnx = x_[i];
        if (x_[i] > mxx) mxx = x_[i];
    }
    printf("SEGMENT bilder=%d kameras=", n - a);
    int erst = 1;
    for (int c = 0; c < 64; c++) if (seen[c]) { printf(erst ? "%d" : ",%d", c); erst = 0; }
    printf(" letzte_kamera=%d min_x=%d max_x=%d\n", cam_[n - 1], mnx, mxx);
    return 0;
}

/* ---- Nachbesserung 2 ------------------------------------------------------------------------------------ */
static unsigned char *ppm_lesen(const char *pfad, int *w, int *h)
{
    FILE *f = fopen(pfad, "rb");
    if (!f) return NULL;
    int mx = 0;
    if (fscanf(f, "P6 %d %d %d", w, h, &mx) != 3 || *w <= 0 || *h <= 0 || *w > 8192 || *h > 8192) { fclose(f); return NULL; }
    fgetc(f);
    size_t n = (size_t)(*w) * (size_t)(*h) * 3u;
    unsigned char *b = (unsigned char *)malloc(n);
    if (!b || fread(b, 1, n, f) != n) { free(b); fclose(f); return NULL; }
    fclose(f);
    return b;
}

static int gleich(const char *praefix, long von, long bis)
{
    unsigned char *vor = NULL; int vw = 0, vh = 0, paare = 0, gleiche = 0, fehlt = 0;
    for (long fr = von; fr <= bis; fr++) {
        char p[1024]; int w = 0, h = 0;
        snprintf(p, sizeof p, "%s%06ld.ppm", praefix, fr);
        unsigned char *b = ppm_lesen(p, &w, &h);
        if (!b) { fehlt++; continue; }
        if (vor && w == vw && h == vh) {
            paare++;
            if (memcmp(vor, b, (size_t)w * (size_t)h * 3u) == 0) gleiche++;
        }
        free(vor); vor = b; vw = w; vh = h;
    }
    free(vor);
    printf("GLEICH %d von %d (fehlend %d)\n", gleiche, paare, fehlt);
    return (paare > 0 && gleiche == paare && fehlt == 0) ? 0 : 1;
}

static int kamwechsel(const char *state, long von, long bis)
{
    FILE *f = fopen(state, "rb");
    if (!f) { printf("FEHLER %s\n", state); return 2; }
    static long fr_[MAXF * 4]; static int cam_[MAXF * 4];
    int n = 0; char line[4096];
    while (fgets(line, sizeof line, f) && n < MAXF * 4) {
        long fr; int c; const char *cm = strstr(line, " cam=");
        if (line[0] != 'F' || !cm || sscanf(line + 1, "%ld", &fr) != 1 || sscanf(cm + 5, "%d", &c) != 1) continue;
        fr_[n] = fr; cam_[n] = c; n++;
    }
    fclose(f);
    int a = 0;
    for (int i = 1; i < n; i++) if (fr_[i] < fr_[i - 1]) a = i;
    int wechsel = 0;
    for (int i = a + 1; i < n; i++) if (fr_[i] > von && fr_[i] <= bis && cam_[i] != cam_[i - 1]) wechsel++;
    printf("KAMWECHSEL %d\n", wechsel);
    return 0;
}

/* Bild der ERSTEN SCD-Trace-Zeile mit dieser Adresse im letzten Raum (nach "loaded room2020"). */
static long scd_bild_erst(const char *log, const char *adr)
{
    FILE *f = fopen(log, "rb");
    if (!f) return -2;
    char line[1024]; long res = -1;
    while (fgets(line, sizeof line, f)) {
        long fr; char a[32];
        if (strstr(line, "loaded room2020.rdt")) res = -1;
        if (strncmp(line, "[scd] F", 7) != 0) continue;
        if (res < 0 && sscanf(line + 7, "%ld slot=%*d %31s", &fr, a) == 2 && strcmp(a, adr) == 0) res = fr;
    }
    fclose(f);
    return res;
}

typedef struct { int x, z, msg_a, msg_id, gueltig; } zustand_t;
static zustand_t s_z[MAXF];
static int zustand_lesen(const char *state)
{
    memset(s_z, 0, sizeof s_z);
    FILE *f = fopen(state, "rb");
    if (!f) return -1;
    char line[4096];
    while (fgets(line, sizeof line, f)) {
        long fr; int x, z, a = 0, id = 0;
        if (line[0] != 'F' || sscanf(line + 1, "%ld", &fr) != 1 || fr < 0 || fr >= MAXF) continue;
        const char *pl = strstr(line, "PL("), *m = strstr(line, "msg(a=");
        if (!pl || sscanf(pl + 3, "%d,%d", &x, &z) != 2) continue;
        if (m) { const char *ip = strstr(m, " id="); a = atoi(m + 6); if (ip) id = atoi(ip + 4); }
        s_z[fr].x = x; s_z[fr].z = z; s_z[fr].msg_a = a; s_z[fr].msg_id = id; s_z[fr].gueltig = 1;
    }
    fclose(f);
    return 0;
}

/* [mot]-Spur (RE15_MOTRACE): "[mot] F<n> cut=<c> mo=<m> af=<a> ..." und [kamerahalt]-Spur — letzter Raum. */
static int s_mo[MAXF], s_af[MAXF], s_mv[MAXF], s_halt[MAXF];
static void mot_lesen(const char *log)
{
    memset(s_mv, 0, sizeof s_mv); memset(s_halt, 0, sizeof s_halt);
    FILE *f = fopen(log, "rb");
    if (!f) return;
    char line[1024];
    while (fgets(line, sizeof line, f)) {
        long fr; int c, mo, af;
        if (strstr(line, "loaded room2020.rdt")) { memset(s_mv, 0, sizeof s_mv); memset(s_halt, 0, sizeof s_halt); }
        if (sscanf(line, "[mot] F%ld cut=%d mo=%d af=%d", &fr, &c, &mo, &af) == 4 && fr >= 0 && fr < MAXF) {
            s_mo[fr] = mo; s_af[fr] = af; s_mv[fr] = 1;
        }
        if (sscanf(line, "[kamerahalt] F%ld", &fr) == 1 && fr >= 0 && fr < MAXF && strstr(line, "HALTEN")) s_halt[fr] = 1;
    }
    fclose(f);
}

static int takt(const char *dlog, const char *state)
{
    long f_m0 = scd_bild_erst(dlog, "@0x0A5C"), f_m1 = scd_bild_erst(dlog, "@0x0A62");
    long f_if = scd_bild(dlog, "@0x0A68"), f_pos = scd_bild(dlog, "@0x0A8C");
    printf("SCD Message_on0 F%ld Message_on1 F%ld Ifel_ck F%ld Pos_set F%ld\n", f_m0, f_m1, f_if, f_pos);
    if (f_m0 < 0 || f_m1 <= f_m0 || f_pos <= f_m1 || f_pos >= MAXF - 1) { printf("FAIL Szene nicht vollstaendig\n"); return 1; }
    if (zustand_lesen(state) != 0) { printf("FEHLER %s\n", state); return 2; }
    mot_lesen(dlog);
    int ok = 1, wechsel = 0;
    for (long fr = f_m0 + 1; fr <= f_m1; fr++)
        if (s_mv[fr] && s_mv[fr - 1] && (s_af[fr] != s_af[fr - 1] || s_mo[fr] != s_mo[fr - 1])) wechsel++;
    printf("Spieler-Pose zwischen Message_on 0 und 1: %d Wechsel\n", wechsel);
    if (wechsel) { printf("FAIL Luecken-Bild (Original: Spieler angehalten, n9..n47 Clip 5 Bild 9)\n"); ok = 0; }
    long f_zu = -1;
    for (long fr = f_m1; fr < f_pos; fr++)
        if (s_z[fr].gueltig && s_z[fr].msg_a == 1 && s_z[fr].msg_id == 1 && s_z[fr + 1].gueltig && s_z[fr + 1].msg_a == 0) f_zu = fr + 1;
    printf("Frage geschlossen in F%ld, Pos_set F%ld\n", f_zu, f_pos);
    if (f_zu < 0 || f_pos != f_zu + 1) { printf("FAIL Pos_set nicht im ersten Bild nach dem Schliessen (Original n94 -> n95)\n"); ok = 0; }
    if (f_if != f_pos) { printf("FAIL Ifel_ck und Pos_set nicht im selben Skript-Takt\n"); ok = 0; }
    if (!s_halt[f_pos]) { printf("FAIL Pos_set-Bild (Cut_chg @0x0A7E) nicht gehalten\n"); ok = 0; }
    if (!s_z[f_pos].gueltig || s_z[f_pos].x != 3650 || s_z[f_pos].z != -3018 || !s_mv[f_pos] || s_mo[f_pos] != 200) {
        printf("FAIL im Pos_set-Bild nicht (3650,-3018) mo 200: (%d,%d) mo %d\n", s_z[f_pos].x, s_z[f_pos].z, s_mo[f_pos]); ok = 0; }
    printf(ok ? "BESTANDEN\n" : "DURCHGEFALLEN\n");
    return ok ? 0 : 1;
}

int main(int argc, char **argv)
{
    if (argc >= 3 && strcmp(argv[1], "segment") == 0) return segment(argv[2]);
    if (argc >= 3 && strcmp(argv[1], "marker") == 0) return marker(argv[2]);
    if (argc >= 5 && strcmp(argv[1], "kurbel") == 0) return kurbel(argv[2], argv[3], argv[4]);
    if (argc >= 5 && strcmp(argv[1], "gleich") == 0) return gleich(argv[2], atol(argv[3]), atol(argv[4]));
    if (argc >= 5 && strcmp(argv[1], "kamwechsel") == 0) return kamwechsel(argv[2], atol(argv[3]), atol(argv[4]));
    if (argc >= 4 && strcmp(argv[1], "takt") == 0) return takt(argv[2], argv[3]);
    fprintf(stderr, "Aufruf: %s marker <bmp> | kurbel <debug.log> <state.txt> <anim.txt> | segment <state.txt>\n",
            argv[0]);
    return 2;
}
