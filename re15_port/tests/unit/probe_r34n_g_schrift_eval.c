/* probe_r34n_g_schrift_eval.c — Spur G2 (Runde 34 Nacht), AUSWERTER fuer den Integrations-Riegel
 * integration_r34n_g_schrift1150 (tests/integration/test_r34n_g_schrift1150.cmake). Kein add_test.
 *
 * Eingaben:
 *   <ref.ppm>      ein RE15_FRAMEDUMP-Bild desselben Cuts mit RE15_NO_PRI=1 (KEINE Maske gezeichnet)
 *                  = Hintergrund pur. Im Schrift-Rechteck x139..216 y25..40 (Vereinigung der Ziele der
 *                  Records 48..53 = Gruppen 6..11, ROOM1150/1151 sprite.pri Cut 2 @0x0066C; keine
 *                  andere Maske ueberlappt es) ist das genau der AUS-Zustand.
 *   <praefix>      RE15_FRAMEDUMP-Serie "<von>-<bis>/1:<praefix>" des Laufs OHNE RE15_NO_PRI.
 *   <mg.log>       RE15_MG_LOG desselben Laufs: Aufbau- und Opcode-0x45-Zeilen (masken_gruppen.c).
 *   <raum>         1150 oder 1151 (Hex wie im Log).
 *
 * Erwartung je Bild (Orakel = Log, geprueft wird das BILD):
 *   letzter Eintrag vor/in Bild F:  "Aufbau Cut 2"         -> AN  (FUN_800392d4, alle Masken an)
 *                                   "Gruppe 6 := 0" (>= 1 Treffer) -> AUS (FUN_800396a8, Opcode 0x45)
 *                                   "Gruppe 6 := 1" (>= 1 Treffer) -> AN
 *   beobachtet: Rechteck == Referenz -> AUS, sonst AN; alle AN-Bilder im Rechteck bitgleich.
 *   Takt: jeder vollstaendige Lauf gleichen Zustands ist genau 20 Bilder lang (sub05 Sleep 20,
 *   ROOM1150 @0x010C8/@0x010DE; 1 SCD-Takt je Bild der 30-Hz-Schleife).
 *
 * Aufruf: probe_r34n_g_schrift_eval <ref.ppm> <praefix> <von> <bis> <mg.log> <raum>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int w, h; unsigned char *px; } bild_t;

static int lies_zahl(FILE *f)
{
    int c, v = 0, hat = 0;
    for (;;) {
        c = fgetc(f);
        if (c == '#') { while (c != '\n' && c != EOF) c = fgetc(f); continue; }
        if (c == EOF) return -1;
        if (c >= '0' && c <= '9') { v = v * 10 + (c - '0'); hat = 1; }
        else if (hat) return v;
    }
}

static int lade(const char *pfad, bild_t *b)
{
    FILE *f = fopen(pfad, "rb");
    b->px = NULL;
    if (!f) { printf("FEHLER: %s nicht lesbar\n", pfad); return -1; }
    if (fgetc(f) != 'P' || fgetc(f) != '6') { printf("FEHLER: %s ist kein P6\n", pfad); fclose(f); return -1; }
    b->w = lies_zahl(f); b->h = lies_zahl(f);
    int mx = lies_zahl(f);
    if (b->w < 320 || b->h < 240 || mx != 255) { printf("FEHLER: %s Kopf\n", pfad); fclose(f); return -1; }
    size_t n = (size_t)b->w * (size_t)b->h * 3u;
    b->px = (unsigned char *)malloc(n);
    if (!b->px || fread(b->px, 1, n, f) != n) { printf("FEHLER: %s kurz\n", pfad); fclose(f); return -1; }
    fclose(f);
    return 0;
}

#define RX0 139
#define RY0 25
#define RX1 216
#define RY1 40

/* Zahl verschiedener Bildpunkte im Rechteck (in 320x240-Einheiten: ein Punkt zaehlt, wenn einer
 * seiner k*k Pixel abweicht). */
static int diff_rechteck(const bild_t *a, const bild_t *b)
{
    int k = a->w / 320, n = 0;
    for (int y = RY0; y <= RY1; y++)
        for (int x = RX0; x <= RX1; x++) {
            int d = 0;
            for (int yy = 0; yy < k && !d; yy++)
                for (int xx = 0; xx < k && !d; xx++) {
                    size_t i = ((size_t)(y * k + yy) * (size_t)a->w + (size_t)(x * k + xx)) * 3u;
                    if (memcmp(a->px + i, b->px + i, 3) != 0) d = 1;
                }
            n += d;
        }
    return n;
}

#define MAXF 4096

int main(int argc, char **argv)
{
    if (argc < 7) {
        printf("Aufruf: %s <ref.ppm> <praefix> <von> <bis> <mg.log> <raum>\n", argv[0]);
        return 2;
    }
    const char *praefix = argv[2];
    int von = atoi(argv[3]), bis = atoi(argv[4]);
    const char *raum = argv[6];
    if (von < 0 || bis < von || bis - von + 1 > MAXF) { printf("FEHLER: Bereich\n"); return 2; }

    bild_t ref;
    if (lade(argv[1], &ref) != 0) return 1;

    /* Orakel aus dem Log. */
    static signed char soll[MAXF];
    for (int i = 0; i < MAXF; i++) soll[i] = -1;
    int aufbau_cut2 = 0, schalt = 0;
    {
        FILE *lf = fopen(argv[5], "rb");
        if (!lf) { printf("FEHLER: %s nicht lesbar\n", argv[5]); return 1; }
        char z[512], rbuf[16];
        unsigned fr;
        int zustand = -1, letztes_f = -1;
        while (fgets(z, sizeof z, lf)) {
            if (sscanf(z, "[maskgrp] F%u Raum %15s", &fr, rbuf) != 2) continue;
            if (strcmp(rbuf, raum) != 0) continue;
            int neu = zustand;
            const char *p;
            if ((p = strstr(z, "Aufbau Cut ")) != NULL) {
                int cut = atoi(p + 11);
                neu = (cut == 2) ? 1 : -1;
                if (cut == 2 && strstr(z, "Zahl 54 ")) aufbau_cut2++;
            } else if ((p = strstr(z, "Gruppe 6 := ")) != NULL) {
                int v = atoi(p + 12), treffer = 0;
                const char *q = strstr(p, "(");
                if (q) treffer = atoi(q + 1);
                if (treffer < 1) continue;
                neu = v & 1;
                if ((int)fr >= von && (int)fr <= bis) schalt++;
            } else continue;
            /* Zustand gilt ab Bild fr (Aufbau am Bildanfang, Opcode im SCD-Takt, beides vor dem
             * Zeichnen desselben Bilds: main.c pc_cam_present_apply -> scd_vm_tick -> end_frame). */
            for (int f = (letztes_f < 0 ? 0 : letztes_f); f < (int)fr && f <= bis; f++)
                if (f >= von) soll[f - von] = (signed char)zustand;
            zustand = neu;
            letztes_f = (int)fr;
        }
        for (int f = (letztes_f < 0 ? von : letztes_f); f <= bis; f++)
            if (f >= von) soll[f - von] = (signed char)zustand;
        fclose(lf);
    }
    int fails = 0;
    if (aufbau_cut2 < 1) { printf("FAIL: kein 'Aufbau Cut 2 ... Zahl 54' fuer Raum %s im Log\n", raum); fails++; }
    if (schalt < 4) { printf("FAIL: nur %d Umschaltungen (Gruppe 6, >= 1 Treffer) in F%d..F%d\n", schalt, von, bis); fails++; }

    bild_t an_ref = { 0, 0, NULL };
    int ist_prev = -1, lauf_start = -1, lauf_n = 0, laeufe_20 = 0, laeufe_falsch = 0;
    int n_an = 0, n_aus = 0, abweich = 0, an_diff = -1, an_uneinig = 0;
    for (int f = von; f <= bis; f++) {
        char pfad[600];
        snprintf(pfad, sizeof pfad, "%s%06d.ppm", praefix, f);
        bild_t b;
        if (lade(pfad, &b) != 0) { fails++; break; }
        if (b.w != ref.w || b.h != ref.h) { printf("FAIL: %s Groesse\n", pfad); fails++; free(b.px); break; }
        int d = diff_rechteck(&b, &ref);
        int ist = d ? 1 : 0;
        if (ist) {
            n_an++;
            if (!an_ref.px) { an_ref = b; b.px = NULL; an_diff = d; }
            else if (diff_rechteck(&b, &an_ref) != 0) an_uneinig++;
        } else n_aus++;
        int s = soll[f - von];
        if (s != ist) {
            if (abweich < 8) printf("  F%d: beobachtet %s, erwartet %s\n", f, ist ? "AN" : "AUS",
                                    s < 0 ? "?" : (s ? "AN" : "AUS"));
            abweich++;
        }
        if (ist != ist_prev) {
            if (ist_prev >= 0 && lauf_start > von) {       /* vollstaendiger Lauf */
                if (lauf_n == 20) laeufe_20++;
                else { laeufe_falsch++; printf("  Lauf %s F%d..F%d: %d Bilder (Soll 20)\n",
                                              ist_prev ? "AN" : "AUS", lauf_start, f - 1, lauf_n); }
            }
            lauf_start = f; lauf_n = 0; ist_prev = ist;
        }
        lauf_n++;
        free(b.px);
    }
    printf("Raum %s F%d..F%d: %d AN, %d AUS; AN weicht im Rechteck in %d Bildpunkten vom "
           "Hintergrund ab\n", raum, von, bis, n_an, n_aus, an_diff);
    if (abweich) { printf("FAIL: %d Bilder weichen vom Log-Orakel ab\n", abweich); fails++; }
    else printf("  PASS: jedes Bild zeigt den Zustand, den Aufbau/Opcode 0x45 im Log vorgeben\n");
    if (n_an < 20 || n_aus < 20) { printf("FAIL: zu wenige AN (%d) oder AUS (%d) Bilder\n", n_an, n_aus); fails++; }
    if (an_uneinig) { printf("FAIL: %d AN-Bilder weichen vom ersten AN-Bild ab\n", an_uneinig); fails++; }
    else if (n_an) printf("  PASS: alle AN-Bilder im Rechteck bitgleich (Buchstaben-Masken)\n");
    if (laeufe_falsch || laeufe_20 < 3) {
        printf("FAIL: %d vollstaendige Laeufe mit 20 Bildern, %d mit anderer Laenge\n", laeufe_20, laeufe_falsch);
        fails++;
    } else printf("  PASS: %d vollstaendige Laeufe, jeder genau 20 Bilder (Sleep 20)\n", laeufe_20);
    free(ref.px); free(an_ref.px);
    printf(fails ? "schrift_eval: FEHLER\n" : "schrift_eval: OK\n");
    return fails ? 1 : 0;
}
