/*
 * RIEGEL Runde 19 / Marke 1 — ROOM10D0 Cut 1: die Tischplatte muss VOR dem Spieler
 * liegen, nicht hinter ihm.
 *
 * ⛔ WOZU (Nutzer-Marke F162, 2026-09-21, befund.log Zeile 13147):
 * Der Spieler steht an (3575, 0, -4568) HINTER dem hinteren Klapptisch. Gemessen im
 * Bild (analysis/befunde_2026-09-21/pri-runde19/marke1-10D0-C1.md): 133 seiner 255
 * gezeichneten Punkte im Koerperkasten x150..170 y82..117 lagen SICHTBAR auf der
 * gruenen Tischplatte, waehrend das METALLGESTELL desselben Tisches ihn vollstaendig
 * verdeckte (162 von 162 Punkten). Ursache: das Tiefenmodell "aufrecht": true der
 * Platte projizierte ihre eigene Vorderkante (Bildpunkt 156,108) auf den FUSSBODEN
 * y0=0 und landete bei Welt(4306,-5736) = Tiefe 201..206, also 5732 Welteinheiten
 * HINTER dem eigenen Gestell (Tiefe 111..172).
 *
 * ⛔ SCHRANKE AUS DER EIGENEN ZELLE, NICHT AUS DEM SPIELERKOPF (verengt 2026-09-21).
 * Die erste Fassung lief ueber ALLE Masken des Cuts und verbot jeder eine Tiefe
 * >= 181 (dem Eimer des Spielerkopfes an dieser EINEN Marke). Das ist zu grob: am
 * fernen Korridorende dieses Winkels stehen Schreibtisch und Pflanze bei Kamera-z um
 * 13000. Bekaeme einer davon spaeter eine Freistellung, haette der Riegel eine
 * RICHTIG gebaute Maske zu Fall gebracht — ein Riegel, der korrekte Daten verbietet,
 * ist schlimmer als keiner.
 *
 * Die Schranke kommt jetzt aus dem Gegenstand selbst, in der Form des
 * Marke-3-Riegels (r19_marke3_1050_c1_tiefe.c):
 *   Zelle der Tischreihe, im RDT nachgewiesen: SCA-Eintrag Typ 1, Band 0,
 *   x=2900 w=1300 -> x2900..4200, z=-4100 d=7900 -> z-4100..3800.
 *   Bandhoehe 0..-1800 ist byte-verankert ueber band = -(Y/0x708), 0x708 = 1800.
 *   Die FERNSTE der acht Ecken liefert per Kamerasatz des Cuts ihr Kamera-z, und
 *   dessen Eimer (1023*vz)>>16 ist die Grenze. Keine Maske, die zu diesem
 *   Gegenstand gehoert, darf dahinter liegen.
 *   Zugehoerigkeit ebenfalls aus der Geometrie: geprueft werden nur die Masken,
 *   deren Rechteck den auf den Schirm projizierten Grundriss dieser Zelle trifft.
 *   Eine Maske am fernen Korridorende liegt ausserhalb und wird uebersprungen.
 *
 * Verdeckungsregel (re15_pri.h): eine Maske der Tiefe t verdeckt einen Figurpunkt der
 * Kamera-z vz genau dann, wenn t < (1023*vz)>>16. Der Spieler an der Marke hat
 * Kamera-z Fuss 12047 / Kopf 11644 (Log 12057 / 11625), sein Kopf-Eimer ist 181 —
 * die Zellgrenze liegt darunter, der Riegel ist also strenger als die Marke verlangt
 * und bleibt zugleich fuer kuenftige ferne Objekte durchlaessig.
 *   Vorher: groesste Tiefe der Plattenmasken 206 (Riegel FAELLT).
 *   Nachher: groesste Tiefe 172 (Riegel HAELT).
 *
 * Zweiter Teil: die Silhouette bleibt die Freistellung des NUTZERS. Die Soll-Deckung
 * steht als P4-Bitmap in ROOM10D0_PRI01.PBM (1021 gesetzte Punkte = 01.png bei
 * Alphaschwelle 110). Der Riegel prueft, dass der Maskensatz genau diese Punkte deckt —
 * kein Punkt mehr, kein Punkt weniger. Damit kann ein spaeteres Tiefenmodell die
 * Freistellung nicht heimlich veraendern.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "re15_rdt.h"
#include "re15_pri.h"
#include "re15_camera.h"

#define CUT            1
#define SPIELER_KOPF_VZ 11644.0   /* Welt(3575,-1400,-4568), Log-Kopf 11625 */
#define SOLL_PUNKTE    1021       /* pri/STAGE1/10D0/01.png, Alphaschwelle 110 */

/* Die Zelle, die die Tischreihe IST — im RDT NACHGEWIESEN, nicht geglaubt.
 * SCA-Eintrag Typ 1, Band 0: x=2900 width=1300, z=-4100 density=7900. */
#define ZX0    2900
#define ZX1    4200
#define ZZ0   (-4100)
#define ZZ1    3800
/* Bandhoehe: band = -(Y/0x708) mit 0x708 = 1800, Band 0 spannt also 0..-1800.
 * Byte-verankert, keine an die Kunst gefittete Zahl. */
#define ZY_OBEN (-1800)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *n = (size_t)sz;
    return b;
}

/* (1023*vz)>>16 — der OT-Index der Figur (@0x8002565c, ZSF3 @0x80066c70) */
static int figur_bucket(double vz) { return (int)((1023.0 * vz) / 65536.0); }

/* P4-Bitmap 320x240 lesen (Soll-Deckung). -> 1 bei Erfolg */
static int pbm_lesen(const char *pfad, uint8_t *aus /* 320*240 */)
{
    size_t n = 0;
    uint8_t *b = slurp(pfad, &n);
    if (!b) return 0;
    /* Kopf: "P4\n<w> <h>\n" — Kommentarzeilen mit '#' ueberspringen */
    size_t i = 0;
    if (n < 2 || b[0] != 'P' || b[1] != '4') { free(b); return 0; }
    i = 2;
    int feld[2] = { 0, 0 }, gelesen = 0;
    while (i < n && gelesen < 2) {
        while (i < n && (b[i] == ' ' || b[i] == '\n' || b[i] == '\r' || b[i] == '\t')) i++;
        if (i < n && b[i] == '#') { while (i < n && b[i] != '\n') i++; continue; }
        int v = 0, ziffern = 0;
        while (i < n && b[i] >= '0' && b[i] <= '9') { v = v * 10 + (b[i] - '0'); i++; ziffern++; }
        if (!ziffern) break;
        feld[gelesen++] = v;
    }
    if (gelesen != 2 || feld[0] != 320 || feld[1] != 240) { free(b); return 0; }
    i++;   /* das EINE Trennzeichen nach der Hoehe */
    const size_t zeile = 320 / 8;
    if (i + zeile * 240 > n) { free(b); return 0; }
    for (int y = 0; y < 240; y++)
        for (int x = 0; x < 320; x++)
            aus[y * 320 + x] = (b[i + y * zeile + (x >> 3)] >> (7 - (x & 7))) & 1;
    free(b);
    return 1;
}

int main(void)
{
    const char *msk = RE15_ASSET_PSX_DIR "/MASKS/ROOM10D0.MSK";
    const char *pbm = RE15_ASSET_PSX_DIR "/MASKS/ROOM10D0_PRI01.PBM";
    size_t n = 0;
    uint8_t *blob = slurp(msk, &n);
    if (!blob) { printf("FAIL: %s nicht lesbar\n", msk); return 1; }

    uint32_t off = re15_pri_msk_section_offset(blob, n, CUT);
    if (!off) { printf("FAIL: Container hat keine Sektion fuer Cut %d\n", CUT); free(blob); return 1; }

    re15_pri_cut_t cut;
    int m = re15_pri_parse_section(blob, n, off, &cut);
    if (m <= 0) { printf("FAIL: Sektion Cut %d nicht parsebar (%d)\n", CUT, m); free(blob); return 1; }

    /* --- Teil 1: keine Maske der Tischreihe darf hinter ihrer EIGENEN Zelle liegen --- */
    int fehler = 0;
    int schwelle = -1;              /* Eimer der fernsten Zellecke */
    int fx = 0, fy = 0, fz = 0;     /* diese Ecke, fuer das Protokoll */
    long vz_fern = -0x7FFFFFFFL;
    int gx0 = 320, gy0 = 240, gx1 = -1, gy1 = -1;   /* projizierter Grundriss */

    {
        size_t rdt_n = 0;
        uint8_t *rdt = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM10D0.RDT", &rdt_n);
        re15_rdt_t room;
        int gefunden = 0;
        if (!rdt) { printf("FAIL: ROOM10D0.RDT nicht lesbar\n"); free(blob); return 1; }
        memset(&room, 0, sizeof room);
        if (re15_rdt_parse(rdt, rdt_n, &room) != 0) {
            printf("FAIL: ROOM10D0.RDT nicht parsebar\n"); free(rdt); free(blob); return 1;
        }
        /* (1a) Die Zelle im RDT nachweisen. Ohne sie ist die Schranke unbelegt. */
        for (int i = 0; i < room.sca_count; i++) {
            const re15_sca_entry_t *s = &room.sca[i];
            if ((s->type & 0x0F) != 1) continue;
            if ((s->floor >> 4) != 0) continue;
            if (s->x == ZX0 && s->z == ZZ0 &&
                (int)s->width == (ZX1 - ZX0) && (int)s->density == (ZZ1 - ZZ0)) {
                gefunden = 1;
                printf("SCA-Eintrag %d: Typ 1 Band 0 x%d..%d z%d..%d\n",
                       i, s->x, s->x + s->width, s->z, s->z + s->density);
                break;
            }
        }
        if (!gefunden) {
            printf("FAIL: Zelle der Tischreihe (x%d..%d z%d..%d, Typ 1, Band 0) steht NICHT "
                   "im RDT — die Schranke waere unbelegt\n", ZX0, ZX1, ZZ0, ZZ1);
            free(rdt); free(blob); return 1;
        }
        /* (1b) Kamerasatz des Cuts -> fernste Ecke + projizierter Grundriss. */
        if (room.nCut <= CUT || !room.cuts) {
            printf("FAIL: ROOM10D0 hat keinen Cut %d\n", CUT);
            free(rdt); free(blob); return 1;
        }
        re15_camera_view_t view;
        if (re15_camera_build_view(&room.cuts[CUT], &view) != 0) {
            printf("FAIL: Kamerasatz Cut %d nicht aufbaubar\n", CUT);
            free(rdt); free(blob); return 1;
        }
        const int xs[2] = { ZX0, ZX1 };
        const int ys[2] = { 0, ZY_OBEN };
        const int zs[2] = { ZZ0, ZZ1 };
        for (int a = 0; a < 2; a++) for (int b = 0; b < 2; b++) for (int c = 0; c < 2; c++) {
            long vx = ((long long)xs[a] * view.rot[0] + (long long)ys[b] * view.rot[1]
                     + (long long)zs[c] * view.rot[2]) / 4096 + view.trans[0];
            long vy = ((long long)xs[a] * view.rot[3] + (long long)ys[b] * view.rot[4]
                     + (long long)zs[c] * view.rot[5]) / 4096 + view.trans[1];
            long vz = ((long long)xs[a] * view.rot[6] + (long long)ys[b] * view.rot[7]
                     + (long long)zs[c] * view.rot[8]) / 4096 + view.trans[2];
            if (vz > vz_fern) { vz_fern = vz; fx = xs[a]; fy = ys[b]; fz = zs[c]; }
            if (vz > 1) {   /* sx-160 = H*vx/vz, sy-120 = H*vy/vz (geom.py:126) */
                int sx = 160 + (int)((double)view.fov_screen_dist * (double)vx / (double)vz);
                int sy = 120 + (int)((double)view.fov_screen_dist * (double)vy / (double)vz);
                if (sx < gx0) gx0 = sx; if (sx > gx1) gx1 = sx;
                if (sy < gy0) gy0 = sy; if (sy > gy1) gy1 = sy;
            }
        }
        free(rdt);
        if (vz_fern <= 64) {
            printf("FAIL: fernste Zellecke liegt nicht vor der Kamera (Kamera-z %ld)\n", vz_fern);
            free(blob); return 1;
        }
        schwelle = figur_bucket((double)vz_fern);
        printf("fernste Zellecke (%d,%d,%d): Kamera-z %ld -> Eimer %d\n",
               fx, fy, fz, vz_fern, schwelle);
        printf("projizierter Grundriss der Zelle: x%d..%d y%d..%d "
               "(nur Masken darin werden geprueft)\n", gx0, gx1, gy0, gy1);
    }

    int tmin = 1 << 30, tmax = -1, zu_fern = 0, schlimmste = -1, geprueft = 0, ueberspr = 0;
    for (int i = 0; i < m; i++) {
        int t  = (int)cut.masks[i].depth;
        int mx0 = (int16_t)cut.masks[i].dstX, my0 = (int16_t)cut.masks[i].dstY;
        int mx1 = mx0 + (int)cut.masks[i].width  - 1;
        int my1 = my0 + (int)cut.masks[i].height - 1;
        /* Zugehoerigkeit: Rechteck trifft den projizierten Zellgrundriss? */
        if (mx1 < gx0 || mx0 > gx1 || my1 < gy0 || my0 > gy1) { ueberspr++; continue; }
        geprueft++;
        if (t < tmin) tmin = t;
        if (t > tmax) tmax = t;
        if (t > schwelle) { zu_fern++; if (t > schlimmste) schlimmste = t; }
    }
    printf("Cut %d: %d Masken, davon %d im Zellgrundriss (Tiefe %d..%d), %d ausserhalb "
           "uebersprungen\n", CUT, m, geprueft, geprueft ? tmin : -1, tmax, ueberspr);

    /* ABDECKUNG: ein Riegel ohne geprueften Fall ist kein Riegel. */
    if (geprueft == 0) {
        printf("FAIL: keine einzige Maske liegt im Zellgrundriss — der Riegel prueft nichts "
               "(Abdeckung 0), das ist selbst ein Defekt\n");
        fehler = 1;
    } else if (zu_fern) {
        printf("FAIL: %d von %d Masken der Tischreihe liegen mit Tiefe > %d HINTER der "
               "fernsten Ecke ihrer EIGENEN Zelle (schlimmste %d) — der Tisch stand hinter "
               "sich selbst und verdeckte den Spieler nicht, der hinter ihm steht\n",
               zu_fern, geprueft, schwelle, schlimmste);
        fehler = 1;
    } else {
        printf("OK  : alle %d Masken der Tischreihe mit Tiefe <= %d, also nicht hinter der "
               "eigenen Zelle\n", geprueft, schwelle);
    }

    /* --- Teil 1b: das gemeldete SYMPTOM selbst, an der gemessenen Marke ---
     * Die Zellschranke aus Teil 1 ist die allgemeine, zukunftsfeste Regel, aber sie ist
     * an dieser Marke gemessen EINEN Eimer lockerer als der Spielerkopf (Zellecke 182
     * gegen Kopf 181). Eine Maske mit genau 182 wuerde Teil 1 also bestehen und den Kopf
     * trotzdem freilassen — genau der Befund des Nutzers. Deshalb zusaetzlich der
     * Symptom-Pin: jede Maske der Tischreihe, die den gemessenen Koerperkasten der Marke
     * beruehrt, MUSS den Kopf verdecken (Regel re15_pri.h: t < (1023*vz)>>16).
     * Der Pin ist auf den Zellgrundriss UND den Koerperkasten beschraenkt und kann eine
     * kuenftige ferne Maske am Korridorende deshalb nicht zu Fall bringen. */
    {
        const int kbx0 = 150, kbx1 = 170, kby0 = 82, kby1 = 117;  /* Log F162 */
        const int kopf = figur_bucket(SPIELER_KOPF_VZ);           /* 181 */
        int im_kasten = 0, frei = 0, schlimmster = -1;
        for (int i = 0; i < m; i++) {
            int t  = (int)cut.masks[i].depth;
            int mx0 = (int16_t)cut.masks[i].dstX, my0 = (int16_t)cut.masks[i].dstY;
            int mx1 = mx0 + (int)cut.masks[i].width  - 1;
            int my1 = my0 + (int)cut.masks[i].height - 1;
            if (mx1 < gx0 || mx0 > gx1 || my1 < gy0 || my0 > gy1) continue;   /* fremdes Objekt */
            if (mx1 < kbx0 || mx0 > kbx1 || my1 < kby0 || my0 > kby1) continue;
            im_kasten++;
            if (t >= kopf) { frei++; if (t > schlimmster) schlimmster = t; }
        }
        printf("Symptom-Pin: %d Masken der Tischreihe im Koerperkasten x%d..%d y%d..%d, "
               "Kopf-Eimer %d\n", im_kasten, kbx0, kbx1, kby0, kby1, kopf);
        if (im_kasten == 0) {
            printf("FAIL: keine Maske der Tischreihe beruehrt den Koerperkasten der Marke "
                   "(Abdeckung 0) — der Pin prueft nichts\n");
            fehler = 1;
        } else if (frei) {
            printf("FAIL: %d von %d Masken im Koerperkasten lassen den Kopf FREI "
                   "(Tiefe >= %d, schlimmste %d) — genau der Nutzer-Befund F162\n",
                   frei, im_kasten, kopf, schlimmster);
            fehler = 1;
        } else {
            printf("OK  : alle %d Masken im Koerperkasten verdecken den Kopf (Tiefe < %d)\n",
                   im_kasten, kopf);
        }
    }

    /* --- Teil 2: die Silhouette ist bitgenau die Freistellung des Nutzers --- */
    static uint8_t soll[320 * 240];
    static uint8_t ist[320 * 240];
    if (!pbm_lesen(pbm, soll)) { printf("FAIL: %s nicht lesbar/kein 320x240-P4\n", pbm); free(blob); return 1; }
    memset(ist, 0, sizeof ist);
    for (int i = 0; i < m; i++) {
        const re15_pri_mask_t *k = &cut.masks[i];
        for (int y = 0; y < (int)k->height; y++) {
            int yy = (int)k->dstY + y;
            if (yy < 0 || yy >= 240) continue;
            for (int x = 0; x < (int)k->width; x++) {
                int xx = (int)k->dstX + x;
                if (xx < 0 || xx >= 320) continue;
                ist[yy * 320 + xx] = 1;
            }
        }
    }
    int soll_n = 0, fehlt = 0, zuviel = 0;
    for (int i = 0; i < 320 * 240; i++) {
        soll_n += soll[i];
        if (soll[i] && !ist[i]) fehlt++;
        if (!soll[i] && ist[i]) zuviel++;
    }
    printf("Soll-Deckung %d px (erwartet %d) | Rechtecke decken: fehlt %d, zuviel %d\n",
           soll_n, SOLL_PUNKTE, fehlt, zuviel);
    if (soll_n != SOLL_PUNKTE) {
        printf("FAIL: die Soll-Deckung hat %d statt %d Punkten — die Freistellung des "
               "Nutzers wurde veraendert\n", soll_n, SOLL_PUNKTE);
        fehler = 1;
    }
    /* Die Rechtecke sind Kaesten; sie duerfen MEHR ueberspannen (der Atlas stanzt die
     * Form), aber KEIN Sollpunkt darf ausserhalb aller Rechtecke liegen. */
    if (fehlt) {
        printf("FAIL: %d Punkte der Freistellung liegen in KEINEM Maskenrechteck\n", fehlt);
        fehler = 1;
    }

    free(blob);
    printf(fehler ? "=== RIEGEL r19-marke1 FAELLT ===\n" : "=== RIEGEL r19-marke1 HAELT ===\n");
    return fehler;
}
