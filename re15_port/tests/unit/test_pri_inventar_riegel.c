/* Das Inventar darf die Maskenliste nicht DAUERHAFT loeschen.
 *
 * ⛔ NUTZER-BEFUND 2026-09-05: "wenn ich im Raum das player inventory oeffne, sind
 * saemtliche pri nicht mehr funktional." Gemessen im laufenden Spiel (RE15_PRI_LOG,
 * ROOM1060 Cut 0, Skript "W30,S1,W8,S1,W20" = warten, START, warten, START):
 *
 *     16 x  Atlas 1, Sektion 81  ->  81 Masken     (vor dem Inventar)
 *     12 x  Atlas 1, Sektion 81  ->   0 Masken     (nach dem Inventar, bis Laufende)
 *
 * Die Sektion lieferte weiter 81 Masken - die Liste im Zeichner war 0.
 *
 * URSACHE: re15_render_pc_clear_scene_overlays() behandelte drei Dinge gleich, von
 * denen nur zwei bildweise Warteschlangen sind:
 *     s_textri_count       - wird jedes Bild neu gefuellt   -> loeschen ist richtig
 *     s_shadow_quad_count  - dito                           -> loeschen ist richtig
 *     s_pri_rect_count     - DAUERHAFTER Zustand, gesetzt NUR beim Wechsel von
 *                            (Raum, Cut) (main.c: `if ((int)g_current_room_id !=
 *                            (int)s_pri_room || active_cut_idx != s_pri_cut)`)
 * Wer den dritten auf 0 setzt, loescht ihn fuer immer: nach dem Schliessen des
 * Inventars aendert sich weder Raum noch Cut, also fuellt ihn niemand nach.
 *
 * FIX: ein Riegel fuer GENAU EIN Bild (s_pri_suppress), den re15_render_begin_frame()
 * zuruecknimmt. Die Liste selbst bleibt stehen.
 *
 * ⛔ WARUM DIESER TEST DIE QUELLE LIEST statt zu rendern: der Maskenpfad lebt in
 * platform/pc und braucht einen SDL-Renderer; ein Unit-Test hat keinen. Geprueft wird
 * deshalb die INVARIANTE im Quelltext - dieselbe Bauart wie test_audio_sperren.c. Die
 * Wirkung ist am laufenden Spiel gemessen (oben) und steht im Commit.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

static char *slurp(const char *rel, size_t *n)
{
    char pfad[600];
    FILE *f;
    char *p;
    long len;
    snprintf(pfad, sizeof pfad, "%s/%s", RE15_PORT_SRC_DIR, rel);
    f = fopen(pfad, "rb");
    if (!f) { printf("  (nicht lesbar: %s)\n", pfad); return NULL; }
    fseek(f, 0, SEEK_END); len = ftell(f); fseek(f, 0, SEEK_SET);
    if (len <= 0) { fclose(f); return NULL; }
    p = (char *)malloc((size_t)len + 1);
    if (!p) { fclose(f); return NULL; }
    if (fread(p, 1, (size_t)len, f) != (size_t)len) { free(p); fclose(f); return NULL; }
    fclose(f);
    p[len] = 0;
    if (n) *n = (size_t)len;
    return p;
}

/* Rumpf einer Funktion: von ihrem Namen bis zur schliessenden Klammer in Spalte 0. */
static char *rumpf(char *src, const char *name)
{
    char *a = strstr(src, name);
    char *e;
    if (!a) return NULL;
    a = strchr(a, '{');
    if (!a) return NULL;
    e = strstr(a, "\n}");
    return e ? a : NULL;
}

static int enthaelt(char *src, const char *name, const char *nadel)
{
    char *a = rumpf(src, name), *e;
    size_t n;
    char *kopie;
    int r;
    if (!a) return -1;
    e = strstr(a, "\n}");
    n = (size_t)(e - a);
    kopie = (char *)malloc(n + 1);
    if (!kopie) return -1;
    memcpy(kopie, a, n); kopie[n] = 0;
    r = strstr(kopie, nadel) != NULL;
    free(kopie);
    return r;
}

int main(void)
{
    size_t n = 0;
    char *r = slurp("platform/pc/src/render_pc.c", &n);
    char *m = slurp("platform/pc/main.c", NULL);

    printf("== Inventar-Riegel fuer die Maskenliste ==\n");
    CHECK("render_pc.c lesbar", r != NULL);
    CHECK("main.c lesbar", m != NULL);
    if (!r || !m) return 1;

    CHECK("clear_scene_overlays loescht die BILDWEISE Dreieck-Warteschlange",
          enthaelt(r, "re15_render_pc_clear_scene_overlays", "s_textri_count = 0") == 1);
    CHECK("clear_scene_overlays loescht die BILDWEISEN Schattenflecken",
          enthaelt(r, "re15_render_pc_clear_scene_overlays", "s_shadow_quad_count = 0") == 1);
    CHECK("clear_scene_overlays loescht die MASKENLISTE NICHT (sie ist dauerhaft)",
          enthaelt(r, "re15_render_pc_clear_scene_overlays", "s_pri_rect_count = 0") == 0);
    CHECK("clear_scene_overlays setzt stattdessen den Ein-Bild-Riegel",
          enthaelt(r, "re15_render_pc_clear_scene_overlays", "s_pri_suppress = 1") == 1);
    CHECK("re15_render_begin_frame nimmt den Riegel zurueck",
          enthaelt(r, "void re15_render_begin_frame", "s_pri_suppress = 0") == 1);
    CHECK("der Maskendurchgang fragt den Riegel ab",
          strstr(r, "!s_pri_suppress && s_pri_atlas_tex") != NULL);

    /* Die Vorbedingung, die den Fehler ueberhaupt toedlich machte: die Liste wird nur
     * bei einem Wechsel von (Raum, Cut) gesetzt. Bleibt das so, MUSS der Riegel oben
     * gelten; wird es eines Tages ein Aufruf pro Bild, ist dieser Test zu lockern -
     * dann aber bewusst. */
    CHECK("main.c setzt die Maskenliste weiterhin nur beim Wechsel von (Raum, Cut)",
          strstr(m, "!= (int)s_pri_room || active_cut_idx != s_pri_cut") != NULL);

    free(r); free(m);
    return g_fail;
}
