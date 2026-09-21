/*
 * RIEGEL (Nutzer-Befund 2026-09-21, F9-Marke 3 in befund.log Zeile 13287):
 *
 *   ROOM1050 Cut 1, Leon bei (16732,0,6350) — sein linkes Bein hing 244 Bildpunkte
 *   weit VOR der Schreibtischplatte, obwohl er 450 Welteinheiten HINTER dem
 *   Tischende steht. Die Freistellung des Nutzers war richtig (Deckung bitgenau,
 *   test_pri_silhouette); falsch war die TIEFE: 32 der 100 Rechtecke behaupteten
 *   eine Tiefe von 82..136, der Tisch stand damit hinter sich selbst.
 *
 * WAS HIER GEPRUEFT WIRD — und warum es nicht sein eigenes Modell prueft:
 *
 *   Der Gegenstand der Maske IST die SCA-Sperrzelle x16100..17200 / z1500..5900
 *   aus ROOM1050.RDT (Typ 1, Band 0; gemessene Quaderhoehe -1575: IoU 0,809
 *   gegen die Freistellung, Oberkanten-Abstand 1,3 px, 8651 der 8681
 *   Freistellungspunkte im Quader — analysis/befunde_2026-09-21/pri-runde19/
 *   marke3-1050-C1.md §3). Liegt der ganze Gegenstand in diesem Kasten, dann hat
 *   KEIN Punkt von ihm ein Kamera-z groesser als das seiner FERNSTEN ECKE. Eine
 *   Maskentiefe darueber ist ohne jedes Tiefenmodell falsch.
 *
 *   Die Schranke ist deshalb bewusst LOCKER (die fernste Ecke ist die schwaechste
 *   noch beweisbare Grenze) — ein Riegel darf nicht strenger sein als der Befund.
 *   Zelle, Kamerasatz und Maskentiefen kommen alle aus den ausgelieferten Dateien;
 *   nichts davon stammt aus dem Werkzeug, das die Maske gebaut hat.
 *
 *   Gemessen (analysis/.../marke3-1050-C1/s11_riegel.py):
 *     fernste Ecke (17200, 0, 5900) -> Kamera-z 5246 -> OT-Eimer 81
 *     VORHER:  482 von 8681 Texeln ueber der Grenze (Tiefen 82..136), 32 Rechtecke
 *     NACHHER:   0 von 8681
 *
 * ABDECKUNG (⛔ Pflicht, Memory reai-v2-schiene-abdeckung): dieser Riegel sieht
 * GENAU EINEN der 77 geschriebenen Cuts und 1 der 4 Nutzer-Marken dieser Runde.
 * Er ist ein Pin gegen den Rueckfall genau dieses Cuts, keine Flaechendeckung.
 *
 * Tiefenmodell und Original-Adressen: re15_pri.h (Masken-OT = depth*1
 * @0x80039650-64, Figur-OT = otz>>4 @0x8002565c, ZSF3 = 341 @0x80066c70
 * -> Eimer(vz) = (1023*vz)>>16).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "re15_rdt.h"
#include "re15_pri.h"
#include "re15_camera.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

/* Die Zelle, die der Gegenstand IST — Koordinaten werden im RDT NACHGEWIESEN,
 * nicht geglaubt. */
#define ZX0   16100
#define ZX1   17200
#define ZZ0    1500
#define ZZ1    5900
/* Gemessene Quaderhoehe (PSX-Y nach unten): IoU-Maximum 0,809 bei -1575,
 * Gegenprobe Oberkante 1,3 px. Nur die FERNSTE Ecke geht in die Schranke ein,
 * und die liegt bei y=0 (der Deckel ist naeher) — die Zahl steht hier, damit die
 * Messung nachvollziehbar ist. */
#define ZHOEHE (-1575)
#define CUT    1

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", (t)); \
                         else { printf("  FAIL: %s\n", (t)); g_fail = 1; } } while (0)

static uint8_t *slurp(const char *rel, size_t *n)
{
    char path[512];
    FILE *f;
    long len;
    uint8_t *b;
    snprintf(path, sizeof path, "%s/%s", RE15_ASSET_PSX_DIR, rel);
    f = fopen(path, "rb");
    if (!f) { printf("  ⛔ nicht lesbar: %s\n", path); return NULL; }
    fseek(f, 0, SEEK_END); len = ftell(f); fseek(f, 0, SEEK_SET);
    if (len <= 0) { fclose(f); return NULL; }
    b = (uint8_t *) malloc((size_t) len);
    if (!b) { fclose(f); return NULL; }
    if (fread(b, 1, (size_t) len, f) != (size_t) len) { free(b); fclose(f); return NULL; }
    fclose(f);
    if (n) *n = (size_t) len;
    return b;
}

/* Eimer(vz) = (1023*vz)>>16 — re15_pri.h, ZSF3 = 341 @0x80066c70, otz>>4 @0x8002565c. */
static long eimer(long vz) { return ((long long) 1023 * vz) >> 16; }

int main(void)
{
    size_t rdt_n = 0, msk_n = 0;
    uint8_t *rdt, *msk;
    re15_rdt_t room;
    const re15_camera_cut_t *cut;
    re15_camera_view_t view;
    int i, gefunden = 0;
    long vz_fern = -0x7FFFFFFFL;
    int fx = 0, fy = 0, fz = 0;
    long grenze;
    uint32_t sect;
    re15_pri_cut_t pri;
    int n, ueber = 0, maxdepth = 0;

    printf("=== RIEGEL ROOM1050 C1 — Maskentiefe gegen die eigene SCA-Zelle ===\n");

    rdt = slurp("STAGE1/ROOM1050.RDT", &rdt_n);
    if (!rdt) return 1;
    memset(&room, 0, sizeof room);
    if (re15_rdt_parse(rdt, rdt_n, &room) != 0) {
        printf("  FAIL: ROOM1050.RDT nicht parsebar\n");
        free(rdt);
        return 1;
    }

    /* (1) Die Zelle im RDT NACHWEISEN. Ohne sie ist die Schranke unbelegt. */
    for (i = 0; i < room.sca_count; i++) {
        const re15_sca_entry_t *s = &room.sca[i];
        if ((s->type & 0x0F) != 1) continue;
        if ((s->floor >> 4) != 0) continue;
        if (s->x == ZX0 && s->z == ZZ0 &&
            (int) s->width == (ZX1 - ZX0) && (int) s->density == (ZZ1 - ZZ0)) {
            gefunden = 1;
            printf("  SCA-Eintrag %d: Typ %u x%d..%d z%d..%d (Band %u)\n",
                   i, (unsigned) (s->type & 0x0F), s->x, s->x + s->width,
                   s->z, s->z + s->density, (unsigned) (s->floor >> 4));
            break;
        }
    }
    CHECK("Sperrzelle x16100..17200 z1500..5900 steht im RDT", gefunden);

    /* (2) Kamerasatz des Cuts -> fernste Ecke des Quaders. */
    CHECK("ROOM1050 hat Cut 1", room.nCut > CUT && room.cuts != NULL);
    if (!room.cuts || room.nCut <= CUT) { free(rdt); return 1; }
    cut = &room.cuts[CUT];
    if (re15_camera_build_view(cut, &view) != 0) {
        printf("  FAIL: Kamerasatz Cut %d nicht aufbaubar\n", CUT);
        free(rdt);
        return 1;
    }
    {
        const int xs[2] = { ZX0, ZX1 };
        const int ys[2] = { 0, ZHOEHE };
        const int zs[2] = { ZZ0, ZZ1 };
        int a, b, c;
        for (a = 0; a < 2; a++) for (b = 0; b < 2; b++) for (c = 0; c < 2; c++) {
            long vz = ((long long) xs[a] * view.rot[6]
                     + (long long) ys[b] * view.rot[7]
                     + (long long) zs[c] * view.rot[8]) / 4096 + view.trans[2];
            if (vz > vz_fern) { vz_fern = vz; fx = xs[a]; fy = ys[b]; fz = zs[c]; }
        }
    }
    grenze = eimer(vz_fern);
    printf("  fernste Ecke (%d,%d,%d): Kamera-z %ld -> OT-Eimer %ld\n",
           fx, fy, fz, vz_fern, grenze);
    CHECK("fernste Ecke liegt vor der Kamera (Kamera-z > 64)", vz_fern > 64);

    /* (3) Die geschriebenen Maskentiefen dieses Cuts gegen die Schranke. */
    msk = slurp("MASKS/ROOM1050.MSK", &msk_n);
    if (!msk) { free(rdt); return 1; }
    sect = re15_pri_msk_section_offset(msk, msk_n, CUT);
    memset(&pri, 0, sizeof pri);
    n = sect ? re15_pri_parse_section(msk, msk_n, sect, &pri) : 0;
    printf("  ROOM1050.MSK Cut %d: Sektion @0x%X, %d Masken, %d gezeichnet\n",
           CUT, (unsigned) sect, n, pri.draw_count);
    CHECK("Cut 1 traegt Masken", n > 0 && pri.draw_count > 0);

    for (i = 0; i < pri.draw_count && i < n; i++) {
        int d = (int) pri.masks[i].depth;
        if (d > maxdepth) maxdepth = d;
        if ((long) d > grenze) {
            if (ueber < 8)
                printf("    ueber der Grenze: Rechteck %d x%d y%d %ux%u Tiefe %d (> %ld)\n",
                       i, (int16_t) pri.masks[i].dstX, (int16_t) pri.masks[i].dstY,
                       (unsigned) pri.masks[i].width, (unsigned) pri.masks[i].height,
                       d, grenze);
            ueber++;
        }
    }
    printf("  groesste Tiefe %d, Grenze %ld, darueber %d von %d Rechtecken\n",
           maxdepth, grenze, ueber, pri.draw_count);
    CHECK("kein Rechteck behauptet eine Tiefe hinter der eigenen Zelle", ueber == 0);

    /* (4) Gegenprobe, dass die Schranke ueberhaupt greifen KANN: der Befund lag bei
     *     Tiefe 136, die Grenze muss also deutlich darunter liegen. Sonst waere der
     *     Riegel wirkungslos gruen (Memory reai-v2-schwaches-mass). */
    CHECK("Grenze liegt unter der beanstandeten Tiefe 136 (Riegel kann greifen)",
          grenze < 136);

    free(msk);
    free(rdt);
    printf(ueber == 0 && !g_fail ? "=== r19_marke3_1050_c1_tiefe OK\n"
                                 : "=== r19_marke3_1050_c1_tiefe FEHLGESCHLAGEN\n");
    return g_fail ? 1 : 0;
}
