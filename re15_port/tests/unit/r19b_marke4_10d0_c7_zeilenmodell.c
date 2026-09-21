/*
 * RIEGEL (Nutzer-Befund 2026-09-21, F9-Marke 4, befund.log Zeile 13944, Bild
 * befund_10D0_F3843_marke1.bmp):
 *
 *   ROOM10D0 Cut 7 — "Gerade der letzte zeigt jetzt fehlende Stuecke vom Stuhl auf
 *   der rechten Seite", dann, nach dem Blick auf die eigene Freistellung ueber dem
 *   Hintergrund: "der Stuhl ist doch fast korrekt erkannt, aber es fehlt noch
 *   minimal was von den Rahmen".
 *
 * ZWEI ERKLAERUNGEN WURDEN IN DIESER RUNDE WIDERLEGT und duerfen nicht zurueckkommen
 * (Dossier analysis/befunde_2026-09-21/pri-runde19b-marke4.md):
 *
 *   (a) "Die Freistellung des Nutzers kommt nicht vollstaendig an."  Nein:
 *       2677 von 2677 Punkten sind gedeckt, gelesen auf dem ECHTEN Ladeweg
 *       (re15_pri_msk_section_offset -> re15_pri_parse_section -> re15_tim_parse ->
 *       Blit mit Palettenindex != 0).
 *
 *   (b) "Die Spaltenregel liest in den Spalten 69..72 eine Strebe als Bodenkontakt
 *       und setzt sie zu weit weg, dagegen hilft bodenkante."  Nein: dieses Objekt
 *       traegt in der Auswahl "aufrecht": true. geometrie.standpunkte
 *       (tools/maske/geometrie.py:122-139) kehrt fuer aufrecht/fuss VOR der
 *       Spaltenschleife zurueck — die unterste Zeile einer Spalte wird nie gelesen,
 *       "bodenkante" wird erst in Zeile 141 gelesen, also nach dem Return.
 *       Die ausgelieferten Tiefen der Spalten 69..72 sind 57..58, genau die ihrer
 *       Zeilennachbarn.
 *
 * WAS HIER GEPRUEFT WIRD — die Modellklasse und die Starrheit, keine an einer Marke
 * gemessene Zahl:
 *
 *   Ein starrer, STEHENDER Gegenstand wird hier aus EINEM Standpunkt modelliert:
 *   Tiefe = Profil der senkrechten Weltlinie durch diesen Standpunkt. Daraus folgt
 *   ohne jede Konstante:
 *     (B) die Tiefe haengt nur von der BILDZEILE ab — in einer Zeile tragen alle
 *         Punkte des Gegenstands dieselbe Tiefe;
 *     (C) sie ist monoton in der Zeile, weil vz(Y) = R[7]/4096 * Y + const linear
 *         ist; das Vorzeichen wird aus R[7] GEPRUEFT, nicht angenommen.
 *   Und unabhaengig vom Tiefenmodell, allein aus der Starrheit:
 *     (D) NAHT-PROBE — ein starrer Gegenstand darf an seiner eigenen Naht nicht
 *         springen. Zwischen 8-benachbarten Texeln derselben Silhouette darf die
 *         Tiefe um hoechstens EINEN OT-Eimer springen. (Dasselbe Argument tragen die
 *         grund-Texte von ROOM10D0 C1 in analysis/esp_masken_2026-09-03/auswahl.json.)
 *
 *   GEGENPROBE, im Test selbst gerechnet — aus DEMSELBEN Kamerasatz und DERSELBEN
 *   Silhouette, damit der Riegel nicht wirkungslos gruen stehen kann (Memory
 *   reai-v2-schwaches-mass):
 *     * die Spaltenregel verletzt (B) in 114 von 116 Zeilen und (D) mit einem Sprung
 *       von 24 Eimern an der Naht (68,189)=60 -> (69,189)=84;
 *     * die Spaltenregel MIT bodenkante 1..68 verletzt (B) und (D) ebenfalls
 *       (Sprung 13 Eimer) — bodenkante ist hier also nicht die Abhilfe, sondern
 *       verschiebt den Bruch nur;
 *     * der Weltsprung des Bodenpunkts von Spalte 68 nach 69 (1457 Einheiten) ist
 *       groesser als jeder Sprung innerhalb der Spalten 1..68 (778) — die Messung,
 *       auf der die verworfene Deutung ruhte, bleibt als Zahl erhalten.
 *
 *   ⛔ NICHT geprueft wird "keine Tiefe hinter dem eigenen Fussabdruck". Diese
 *   Schranke waere SELBSTBESTAETIGEND: sie ergibt nur mit dem EINEN Standpunkt die
 *   Zahl 62; modellfrei (der Fussabdruck liegt irgendwo unter der Silhouette) lautet
 *   sie 185 und hat gegen die Spaltenregel (max 86) keine Trennkraft. Gemessen in
 *   build/r19b/mess_17_naht.py, im Dossier §6 festgehalten.
 *
 *   ⛔ Wer dieses Objekt kuenftig auf ein Spalten- oder Zellenmodell umstellt, muss
 *   hier vorbei — mit einem Beleg dafuer, dass der Stuhl wirklich ueber 24 OT-Eimer
 *   in die Tiefe laeuft.
 *
 * ABDECKUNG (⛔ Pflicht, Memory reai-v2-schiene-abdeckung): dieser Riegel sieht GENAU
 *   EINEN Cut (ROOM10D0 Cut 7) von 77 geschriebenen, darin GENAU EIN Objekt von drei,
 *   und in ihm 2677 Texel mit 9638 Nachbarpaaren. Ein Pin gegen den Rueckfall dieses
 *   einen Tiefenmodells, keine Flaechendeckung.
 *
 * Tiefenmodell und Original-Adressen: re15_pri.h (Masken-OT = depth*1
 *   @0x80039650-64, Figur-OT = otz>>4 @0x8002565c, ZSF3 = 341 @0x80066c70
 *   -> Eimer(vz) = (1023*vz)>>16).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "re15_rdt.h"
#include "re15_pri.h"
#include "re15_tim.h"
#include "re15_camera.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_PROBE_DIR
#define RE15_PROBE_DIR "tests/unit/probes"
#endif

#define CUT       7
#define SOLL_PX   2677     /* opake Punkte von pri/STAGE1/10D0/07_01.png bei Alpha > 110 */
#define BK_X0     1        /* bodenkante-Variante der Gegenprobe: Spalten mit eigenem  */
#define BK_X1     68       /* Bodenkontakt; 69..72 erben den naechsten (Spalte 68)     */

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", (t)); \
                         else { printf("  FAIL: %s\n", (t)); g_fail = 1; } } while (0)

static uint8_t g_soll[240][320];
static uint8_t g_deck[240][320];
static int16_t g_tief[240][320];

static uint8_t *slurp(const char *pfad, size_t *n)
{
    FILE *f = fopen(pfad, "rb");
    uint8_t *p; long len;
    if (!f) { printf("  ⛔ nicht lesbar: %s\n", pfad); return NULL; }
    fseek(f, 0, SEEK_END); len = ftell(f); fseek(f, 0, SEEK_SET);
    if (len <= 0) { fclose(f); return NULL; }
    p = (uint8_t *) malloc((size_t) len);
    if (!p) { fclose(f); return NULL; }
    if (fread(p, 1, (size_t) len, f) != (size_t) len) { free(p); fclose(f); return NULL; }
    fclose(f);
    if (n) *n = (size_t) len;
    return p;
}

/* P4-PBM 320x240 (tools/maske/abnahme.py:pbm_schreiben), MSB zuerst. */
static int pbm_lesen(const uint8_t *b, size_t n)
{
    const char *kopf = "P4\n320 240\n";
    size_t k = strlen(kopf); int y, x;
    if (n != k + 240 * 40 || memcmp(b, kopf, k) != 0) return 0;
    for (y = 0; y < 240; y++)
        for (x = 0; x < 320; x++)
            g_soll[y][x] = (b[k + (size_t) y * 40 + (size_t) x / 8] >> (7 - (x & 7))) & 1;
    return 1;
}

/* Blit wie bg_pc.c/render_pc.c: Atlas-Index != 0, je Punkt die NAECHSTE Tiefe.
 *
/* ⛔ EINE ZUORDNUNG JE MASKE TRAEGT NICHT - gemessen, nicht angenommen.
 * Der erste Verengungsversuch wollte jede Maske dem Stuhl oder einem Fremdobjekt
 * zuordnen (Anteil ihrer Texel im Soll-Umriss). Das geht NICHT: der Atlas packt
 * Rechtecke, die BEIDE Objekte schneiden. Gemessen an diesem Cut liegen die Anteile
 * von neun Masken mitten im Graubereich (0,182 / 0,194 / 0,249 / 0,279 / 0,354 /
 * 0,400 / 0,431 / 0,526 / 0,570) bei Tiefen 63..104. Eine Schwelle dort waere geraten.
 *
 * Dieser Riegel braucht deshalb GAR KEINE Zuordnung. Gepruefte Aussage ist:
 *   "In JEDER Bildzeile der Silhouette gibt es EINE Tiefe, die an JEDEM
 *    Silhouettenpunkt dieser Zeile geschrieben ist."
 * Das ist genau die Modellklasse "ein Standpunkt => Tiefe = f(Zeile)". Fremde Masken
 * koennen an einem Punkt nur Tiefen HINZUFUEGEN, niemals die des Stuhls entfernen -
 * die Aussage ist gegen sie immun. Damit kann der weiterhin offene Quader
 * "Holztisch mit Klappstuhl" diesen Riegel spaeter nicht zu Fall bringen.
 * (Vorherige Fassung nahm je Punkt das MINIMUM ueber alle 104 Masken; 1362 der 2677
 * Silhouettenpunkte tragen eine fremde Tiefe, kleinste Reserve 6 Eimer - die Aussagen
 * waren damit Aussagen ueber das Komposit, nicht ueber den Stuhl.) */
static uint8_t g_hat[240][320];   /* Arbeitsfeld: deckt EINE Tiefe diesen Punkt? */

/* Markiert alle Punkte, die von einer Maske GENAU dieser Tiefe gedeckt werden. */
static void deckung_der_tiefe(const re15_pri_cut_t *pri, const re15_tim_t *tim, int d)
{
    const uint8_t *idx = (const uint8_t *) tim->pixels;
    int i, x, y;
    memset(g_hat, 0, sizeof g_hat);
    for (i = 0; i < pri->draw_count; i++) {
        const re15_pri_mask_t *m = &pri->masks[i];
        int dx, dy;
        if ((int) m->depth != d) continue;
        dx = (int16_t) m->dstX; dy = (int16_t) m->dstY;
        for (y = 0; y < m->height; y++) {
            int sy = m->srcY + y, ty = dy + y;
            if (sy < 0 || sy >= tim->height || ty < 0 || ty >= 240) continue;
            for (x = 0; x < m->width; x++) {
                int sx = m->srcX + x, tx = dx + x;
                if (sx < 0 || sx >= tim->width || tx < 0 || tx >= 320) continue;
                if (!idx[(size_t) sy * tim->width + sx]) continue;
                g_hat[ty][tx] = 1;
            }
        }
    }
}

/* Die EINE Tiefe der Bildzeile y, oder -1, wenn keine an ALLEN Punkten liegt.
 * Bei mehreren gueltigen die kleinste; *anzahl gibt zurueck, wie viele es waren. */
static int zeilentiefe(const re15_pri_cut_t *pri, const re15_tim_t *tim, int y, int *anzahl)
{
    int kand[256], nk = 0, i, x, treffer = -1;
    *anzahl = 0;
    for (i = 0; i < pri->draw_count; i++) {
        int d = (int) pri->masks[i].depth, j, neu = 1;
        for (j = 0; j < nk; j++) if (kand[j] == d) { neu = 0; break; }
        if (neu && nk < 256) kand[nk++] = d;
    }
    for (i = 0; i < nk; i++) {
        int alle = 1;
        deckung_der_tiefe(pri, tim, kand[i]);
        for (x = 0; x < 320; x++)
            if (g_soll[y][x] && !g_hat[y][x]) { alle = 0; break; }
        if (alle) { (*anzahl)++; if (treffer < 0 || kand[i] < treffer) treffer = kand[i]; }
    }
    return treffer;
}

static void rastern(const re15_pri_cut_t *pri, const re15_tim_t *tim)
{
    const uint8_t *idx = (const uint8_t *) tim->pixels;
    int i, x, y;
    memset(g_deck, 0, sizeof g_deck);
    memset(g_tief, 0, sizeof g_tief);
    for (i = 0; i < pri->draw_count; i++) {
        const re15_pri_mask_t *m = &pri->masks[i];
        int dx = (int16_t) m->dstX, dy = (int16_t) m->dstY;
        for (y = 0; y < m->height; y++) {
            int sy = m->srcY + y, ty = dy + y;
            if (sy < 0 || sy >= tim->height || ty < 0 || ty >= 240) continue;
            for (x = 0; x < m->width; x++) {
                int sx = m->srcX + x, tx = dx + x;
                if (sx < 0 || sx >= tim->width || tx < 0 || tx >= 320) continue;
                if (!idx[(size_t) sy * tim->width + sx]) continue;
                if (!g_deck[ty][tx] || (int16_t) m->depth < g_tief[ty][tx])
                    g_tief[ty][tx] = (int16_t) m->depth;
                g_deck[ty][tx] = 1;
            }
        }
    }
}

/* Eimer(vz) = (1023*vz)>>16 — re15_pri.h, ZSF3 = 341 @0x80066c70, otz>>4 @0x8002565c. */
static long eimer(double vz) { return (long) floor(1023.0 * vz / 65536.0); }

/* Kamera-z des Punktes, an dem der Sehstrahl durch (sx,sy) die Ebene y=y0 trifft
 * (tools/maske/geom.py:vz_at_floor — dieselbe Formel, hier in double). */
static double vz_am_boden(const re15_camera_view_t *v, double sx, double sy, double y0)
{
    double H = (double) v->fov_screen_dist;
    double a = (sx - 160.0) / H, b = (sy - 120.0) / H;
    double k = v->rot[1] * a + v->rot[4] * b + v->rot[7];
    double c = (double) v->rot[1] * v->trans[0]
             + (double) v->rot[4] * v->trans[1]
             + (double) v->rot[7] * v->trans[2];
    if (fabs(k) < 1e-9) return 0.0;
    return (y0 * 4096.0 + c) / k;
}

/* NAHT-PROBE auf einem Tiefenfeld: groesster Tiefensprung zwischen 8-benachbarten
 * Texeln DERSELBEN Silhouette, und die Zahl der Paare mit Sprung > 1. */
static void naht(const int16_t (*D)[320], int *max_out, int *ueber_out, int *paare_out,
                 int *wx0, int *wy0, int *wx1, int *wy1)
{
    static const int dyv[4] = { 0, 1, 1, 1 };
    static const int dxv[4] = { 1, 0, 1, -1 };
    int k, x, y, mx = 0, ueber = 0, paare = 0;
    *wx0 = *wy0 = *wx1 = *wy1 = -1;
    for (y = 0; y < 240; y++)
        for (x = 0; x < 320; x++) {
            if (!g_soll[y][x]) continue;
            for (k = 0; k < 4; k++) {
                int y2 = y + dyv[k], x2 = x + dxv[k], d;
                if (y2 < 0 || y2 >= 240 || x2 < 0 || x2 >= 320) continue;
                if (!g_soll[y2][x2]) continue;
                paare++;
                d = (int) D[y][x] - (int) D[y2][x2];
                if (d < 0) d = -d;
                if (d > 1) ueber++;
                if (d > mx) { mx = d; *wx0 = x; *wy0 = y; *wx1 = x2; *wy1 = y2; }
            }
        }
    *max_out = mx; *ueber_out = ueber; *paare_out = paare;
}

static int16_t g_gp[240][320];      /* Gegenprobe-Tiefenfeld */

int main(void)
{
    char pfad[700];
    size_t rdt_n = 0, msk_n = 0, tim_n = 0, pbm_n = 0;
    uint8_t *rdt = NULL, *msk = NULL, *tb = NULL, *pbm = NULL;
    re15_rdt_t room;
    re15_camera_view_t view;
    re15_pri_cut_t pri;
    re15_tim_t tim;
    uint32_t sect;
    int n, y, x;
    int soll = 0, gedeckt = 0, fehlt = 0;
    int zeilen = 0, zeilen_mehrfach = 0, rueckschritte = 0;
    int tmin = 9999, tmax = -1, vorherige = -1;
    int nmax, nueber, npaare, a0, b0, a1, b1;
    static long gp_eimer[320];
    static double gp_wx[320], gp_wz[320];
    static int gp_yb[320];
    int gp_zeilen_mehrfach, gp_max, bk_zeilen_mehrfach;
    double schritt_innen = 0.0, schritt_6869 = 0.0;

    printf("=== RIEGEL ROOM10D0 C7 — der Stuhl traegt EIN Zeilenmodell ===\n");

    snprintf(pfad, sizeof pfad, "%s/STAGE1/ROOM10D0.RDT", RE15_ASSET_PSX_DIR);
    rdt = slurp(pfad, &rdt_n);
    if (!rdt) return 1;
    memset(&room, 0, sizeof room);
    if (re15_rdt_parse(rdt, rdt_n, &room) < 0 || room.cut_count <= CUT || !room.cuts) {
        printf("  FAIL: ROOM10D0.RDT / Cut %d\n", CUT);
        free(rdt); return 1;
    }
    if (re15_camera_build_view(&room.cuts[CUT], &view) != 0) {
        printf("  FAIL: Kamerasatz Cut %d nicht aufbaubar\n", CUT);
        free(rdt); return 1;
    }
    printf("  Kamerasatz Cut %d: H=%d  R[7]=%d\n", CUT,
           (int) view.fov_screen_dist, (int) view.rot[7]);
    CHECK("R[7] > 0 — die Kamera schaut nach unten, was aufragt ist NAEHER",
          view.rot[7] > 0);

    memset(&pri, 0, sizeof pri);
    CHECK("ROOM10D0 Cut 7 hat im ORIGINAL keine Masken",
          re15_pri_parse_section(rdt, rdt_n, room.cuts[CUT].pri_offset, &pri) <= 0);

    snprintf(pfad, sizeof pfad, "%s/r19b_marke4_stuhl.pbm", RE15_PROBE_DIR);
    pbm = slurp(pfad, &pbm_n);
    if (!pbm || !pbm_lesen(pbm, pbm_n)) {
        printf("  FAIL: Referenz-Silhouette r19b_marke4_stuhl.pbm unlesbar\n");
        free(pbm); free(rdt); return 1;
    }
    free(pbm);

    snprintf(pfad, sizeof pfad, "%s/MASKS/ROOM10D0.MSK", RE15_ASSET_PSX_DIR);
    msk = slurp(pfad, &msk_n);
    snprintf(pfad, sizeof pfad, "%s/MASKS/ROOM10D0_PRI%02d.TIM", RE15_ASSET_PSX_DIR, CUT);
    tb = slurp(pfad, &tim_n);
    sect = msk ? re15_pri_msk_section_offset(msk, msk_n, CUT) : 0;
    memset(&pri, 0, sizeof pri);
    n = sect ? re15_pri_parse_section(msk, msk_n, sect, &pri) : 0;
    if (n <= 0 || !tb || re15_tim_parse(tb, (int) tim_n, &tim) != 0 || tim.bpp != 8) {
        printf("  FAIL: Sektion (%d) oder Atlas von Cut %d fehlt\n", n, CUT);
        free(msk); free(tb); free(rdt); return 1;
    }
    printf("  ROOM10D0.MSK Cut %d: Sektion @0x%X, %d Masken, %d gezeichnet\n",
           CUT, (unsigned) sect, n, pri.draw_count);
    /* Zuordnung VOR dem Rastern; der Riegel besteht nur, wenn sie eindeutig ist. */
    rastern(&pri, &tim);

    /* --- (B) EINE Tiefe je Bildzeile, ohne Zuordnung der Masken ---
     * Fremde Masken koennen an einem Punkt nur Tiefen hinzufuegen, nie die des
     * Stuhls entfernen. Die Aussage ist deshalb gegen den offenen Nachbar-Quader
     * immun; siehe den Kommentar bei deckung_der_tiefe(). */
    {
        int y, zeilen_b = 0, ohne = 0, mehrdeutig = 0;
        int letzte = -1, rueck = 0, sprung_max = 0;
        int erste_y = -1, letzte_y = -1;
        for (y = 0; y < 240; y++) {
            int x, hat = 0, anz = 0, d;
            for (x = 0; x < 320; x++) if (g_soll[y][x]) { hat = 1; break; }
            if (!hat) continue;
            if (erste_y < 0) erste_y = y;
            letzte_y = y;
            zeilen_b++;
            d = zeilentiefe(&pri, &tim, y, &anz);
            if (d < 0) {
                if (ohne < 6)
                    printf("  ⛔ Zeile %d: KEINE Tiefe liegt an allen Silhouettenpunkten\n", y);
                ohne++;
                continue;
            }
            if (anz > 1) mehrdeutig++;
            if (letzte >= 0) {
                int s = d - letzte;
                if (s < 0) rueck++;
                if (s > sprung_max) sprung_max = s;
            }
            letzte = d;
        }
        printf("Zeilenmodell: %d Silhouettenzeilen (y%d..%d), %d ohne durchgehende Tiefe, "
               "%d mehrdeutig, %d Monotonie-Rueckschritte, groesster Zeilensprung %d\n",
               zeilen_b, erste_y, letzte_y, ohne, mehrdeutig, rueck, sprung_max);
        CHECK("(B) jede Silhouettenzeile traegt EINE Tiefe an ALLEN ihren Punkten", ohne == 0);
        CHECK("(C) die Zeilentiefe ist monoton (Vorzeichen aus R[7] geprueft)", rueck == 0);
        CHECK("Abdeckung: der Riegel hat Zeilen zu pruefen", zeilen_b > 0);
    }


    /* (A) ABDECKUNG — die Handarbeit des Nutzers kommt vollstaendig an. */
    for (y = 0; y < 240; y++)
        for (x = 0; x < 320; x++) {
            if (!g_soll[y][x]) continue;
            soll++;
            if (g_deck[y][x]) gedeckt++; else fehlt++;
        }
    printf("  Freistellung 07_01: %d Punkte, gedeckt %d, ungedeckt %d\n", soll, gedeckt, fehlt);
    CHECK("die Referenz-Silhouette hat die erwarteten 2677 Punkte", soll == SOLL_PX);
    CHECK("jeder Punkt der Nutzer-Freistellung ist gedeckt", fehlt == 0);
    if (fehlt) { free(msk); free(tb); free(rdt); return 1; }

    /* (B)+(C) ZEILEN-INVARIANTE und MONOTONIE. */
    for (y = 0; y < 240; y++) {
        int erste = -1, mehrfach = 0;
        for (x = 0; x < 320; x++) {
            if (!g_soll[y][x]) continue;
            if (erste < 0) erste = g_tief[y][x];
            else if (g_tief[y][x] != erste) mehrfach = 1;
            if (g_tief[y][x] < tmin) tmin = g_tief[y][x];
            if (g_tief[y][x] > tmax) tmax = g_tief[y][x];
        }
        if (erste < 0) continue;
        zeilen++;
        if (mehrfach) {
            if (zeilen_mehrfach < 6) printf("    Zeile %d traegt MEHRERE Tiefen\n", y);
            zeilen_mehrfach++;
        }
        if (vorherige >= 0 && erste < vorherige) rueckschritte++;
        vorherige = erste;
    }
    printf("  %d Bildzeilen mit Punkten, Tiefen %d..%d, Zeilen mit mehreren Tiefen %d, "
           "Rueckschritte %d\n", zeilen, tmin, tmax, zeilen_mehrfach, rueckschritte);
    CHECK("jede Bildzeile des Stuhls traegt GENAU EINE Tiefe (ein Standpunkt, kein "
          "Standpunkt je Spalte)", zeilen_mehrfach == 0);
    CHECK("die Tiefe waechst monoton mit der Bildzeile (lineares Profil der Senkrechten)",
          rueckschritte == 0);

    /* (D) NAHT-PROBE auf den ausgelieferten Tiefen. */
    naht((const int16_t (*)[320]) g_tief, &nmax, &nueber, &npaare, &a0, &b0, &a1, &b1);
    printf("  Naht: %d Nachbarpaare, groesster Tiefensprung %d", npaare, nmax);
    if (a0 >= 0) printf("  bei (%d,%d)=%d -> (%d,%d)=%d", a0, b0, g_tief[b0][a0],
                        a1, b1, g_tief[b1][a1]);
    printf(", Paare mit Sprung > 1: %d\n", nueber);
    CHECK("der starre Gegenstand springt an seiner eigenen Naht um hoechstens EINEN Eimer",
          nueber == 0 && nmax <= 1);

    /* ---- GEGENPROBE: die Spaltenregel, aus demselben Kamerasatz gerechnet ---- */
    for (x = 0; x < 320; x++) {
        gp_eimer[x] = -1; gp_yb[x] = -1;
        for (y = 239; y >= 0; y--) if (g_soll[y][x]) { gp_yb[x] = y; break; }
        if (gp_yb[x] < 0) continue;
        {
            double vz = vz_am_boden(&view, x + 0.5, gp_yb[x] + 0.5, 0.0);
            double H = (double) view.fov_screen_dist;
            double vx = (x + 0.5 - 160.0) * vz / H;
            double vy = (gp_yb[x] + 0.5 - 120.0) * vz / H;
            double ax = vx - view.trans[0], ay = vy - view.trans[1], az = vz - view.trans[2];
            gp_eimer[x] = eimer(vz);
            gp_wx[x] = (view.rot[0] * ax + view.rot[3] * ay + view.rot[6] * az) / 4096.0;
            gp_wz[x] = (view.rot[2] * ax + view.rot[5] * ay + view.rot[8] * az) / 4096.0;
        }
    }
    /* Weltsprung der Standpunkte zwischen Nachbarspalten. */
    {
        int vor = -1;
        for (x = 0; x < 320; x++) {
            if (gp_eimer[x] < 0) continue;
            if (vor >= 0) {
                double dx = gp_wx[x] - gp_wx[vor], dz = gp_wz[x] - gp_wz[vor];
                double s = sqrt(dx * dx + dz * dz);
                if (x == 69 && vor == 68) schritt_6869 = s;
                else if (x <= BK_X1 && s > schritt_innen) schritt_innen = s;
            }
            vor = x;
        }
    }
    /* Spaltenregel: jede Spalte traegt ihren eigenen Standpunkt-Eimer. */
    gp_max = -1; gp_zeilen_mehrfach = 0;
    for (y = 0; y < 240; y++)
        for (x = 0; x < 320; x++)
            g_gp[y][x] = (int16_t) (g_soll[y][x] && gp_eimer[x] >= 0 ? gp_eimer[x] : 0);
    for (x = 0; x < 320; x++) if (gp_eimer[x] > gp_max) gp_max = (int) gp_eimer[x];
    for (y = 0; y < 240; y++) {
        int erste = -1, mehrfach = 0;
        for (x = 0; x < 320; x++) {
            if (!g_soll[y][x]) continue;
            if (erste < 0) erste = g_gp[y][x];
            else if (g_gp[y][x] != erste) mehrfach = 1;
        }
        if (erste >= 0 && mehrfach) gp_zeilen_mehrfach++;
    }
    naht((const int16_t (*)[320]) g_gp, &nmax, &nueber, &npaare, &a0, &b0, &a1, &b1);
    printf("  Gegenprobe Spaltenregel: Zeilen mit mehreren Tiefen %d von %d, groesster "
           "Eimer %d,\n    Naht-Sprung %d", gp_zeilen_mehrfach, zeilen, gp_max, nmax);
    if (a0 >= 0) printf(" bei (%d,%d)=%d -> (%d,%d)=%d", a0, b0, g_gp[b0][a0],
                        a1, b1, g_gp[b1][a1]);
    printf(", Paare > 1: %d\n", nueber);
    CHECK("die Spaltenregel wuerde die Zeilen-Invariante verletzen (Riegel kann greifen)",
          gp_zeilen_mehrfach > 0);
    CHECK("die Spaltenregel wuerde die Naht-Probe verletzen (Riegel kann greifen)",
          nmax > 1 && nueber > 0);

    /* Spaltenregel MIT bodenkante 1..68: die Spalten 69..72 erben Spalte 68. */
    for (x = 69; x < 320; x++) if (gp_eimer[x] >= 0) gp_eimer[x] = gp_eimer[BK_X1];
    for (x = 0; x < BK_X0; x++) if (gp_eimer[x] >= 0) gp_eimer[x] = gp_eimer[BK_X0];
    for (y = 0; y < 240; y++)
        for (x = 0; x < 320; x++)
            g_gp[y][x] = (int16_t) (g_soll[y][x] && gp_eimer[x] >= 0 ? gp_eimer[x] : 0);
    bk_zeilen_mehrfach = 0;
    for (y = 0; y < 240; y++) {
        int erste = -1, mehrfach = 0;
        for (x = 0; x < 320; x++) {
            if (!g_soll[y][x]) continue;
            if (erste < 0) erste = g_gp[y][x];
            else if (g_gp[y][x] != erste) mehrfach = 1;
        }
        if (erste >= 0 && mehrfach) bk_zeilen_mehrfach++;
    }
    naht((const int16_t (*)[320]) g_gp, &nmax, &nueber, &npaare, &a0, &b0, &a1, &b1);
    printf("  Gegenprobe Spaltenregel + bodenkante %d..%d: Zeilen mit mehreren Tiefen %d, "
           "Naht-Sprung %d, Paare > 1: %d\n", BK_X0, BK_X1, bk_zeilen_mehrfach, nmax, nueber);
    CHECK("auch bodenkante repariert das nicht — die Zeilen-Invariante bleibt verletzt",
          bk_zeilen_mehrfach > 0 && nmax > 1);

    printf("  Weltsprung des Bodenpunkts 68->69 = %.0f Einheiten, groesster Sprung "
           "innerhalb %d..%d = %.0f\n", schritt_6869, BK_X0, BK_X1, schritt_innen);
    CHECK("der Weltsprung 68->69 ist groesser als jeder Sprung innerhalb 1..68 "
          "(dort ist die unterste Zeile kein Bodenkontakt)", schritt_6869 > schritt_innen);

    /* Der konkrete Pin der Marke: 69..72 tragen die Tiefe ihrer Zeilennachbarn. */
    {
        int abw = 0, vergleich = 0;
        for (y = 0; y < 240; y++) {
            int nachbar = -1;
            if (!g_soll[y][69] && !g_soll[y][70] && !g_soll[y][71] && !g_soll[y][72]) continue;
            for (x = 0; x < 69; x++)
                if (g_soll[y][x]) { nachbar = g_tief[y][x]; break; }
            if (nachbar < 0) continue;
            for (x = 69; x <= 72; x++) {
                if (!g_soll[y][x]) continue;
                vergleich++;
                if (g_tief[y][x] != nachbar) abw++;
            }
        }
        printf("  Spalten 69..72: %d Texel gegen ihre Zeilennachbarn verglichen, %d abweichend\n",
               vergleich, abw);
        CHECK("die Spalten 69..72 tragen dieselbe Tiefe wie ihre Zeilennachbarn",
              vergleich > 0 && abw == 0);
    }

    free(msk); free(tb); free(rdt);
    printf(g_fail ? "=== r19b_marke4_10d0_c7_zeilenmodell FEHLGESCHLAGEN\n"
                  : "=== r19b_marke4_10d0_c7_zeilenmodell OK\n");
    return g_fail ? 1 : 0;
}
