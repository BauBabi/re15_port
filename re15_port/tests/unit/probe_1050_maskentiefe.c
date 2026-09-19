/*
 * MESSSONDE (Nutzer-Befund 2026-09-14, befund.log #1):
 *   "Leon ist teilweise transparent bei den PRI Overlays."  ROOM1050, Cut 6.
 *
 * Diese Sonde AENDERT NICHTS. Sie misst, welche der nachgezeichneten Masken-Kacheln
 * den Spielerkasten ueberhaupt beruehren und welche davon ihn nach dem GETEILTEN
 * Tiefenmodell (re15_pri.h) verdecken.
 *
 * Tiefenmodell, beide Adressen im Ghidra-Dump nachgeschlagen:
 *   Masken-OT-Wortindex = depth * 1
 *       80039650  lh   a0,0x2(s3)     ; die geschriebene Tiefen-Halbwort
 *       80039658  sll  a0,a0,0x2      ; *4 BYTES = *1 WORT
 *       8003965c  addu a0,a0,s6       ; s6 = OT-Basis
 *       80039660  jal  AddPrim
 *   Figuren-OT-Wortindex = otz >> 4
 *       80025648  stotz t4
 *       80025654  sra  v0,v1,0x6      ; otz < 64 -> Polygon faellt weg
 *       8002565c  sra  v1,v1,0x4      ; <<< der Schieber
 *   otz = AVSZ3 mit ZSF3 = 0x155 = 341   (80066c70  li t0,0x155 ; gte_ldZSF3)
 *       -> otz = (1023*vz)>>12
 *   Eine Maske verdeckt ein Figuren-Dreieck GENAU DANN, wenn sie SPAETER gezeichnet
 *   wird, also depth < otz>>4.  Auf dem PC ist der Sortierschluessel roh vz, deshalb
 *   re15_pri_mask_camera_z(depth) = depth*64 (render_pc.c Zeile 874/907).
 *
 * Die drei vz-Messpunkte und der Bildkasten stammen aus dem Vorlauf des Nutzer-Befunds.
 */
#include "re15_pri.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *slurp(const char *rel, long *size_out)
{
    char path[512];
    snprintf(path, sizeof path, "%s/%s", RE15_ASSET_PSX_DIR, rel);
    FILE *f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "  ⛔ nicht lesbar: %s\n", path); return NULL; }
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)n);
    if (b && fread(b, 1, (size_t)n, f) != (size_t)n) { free(b); b = NULL; }
    fclose(f);
    if (size_out) *size_out = n;
    return b;
}

/* Der Kasten des Spielers im Bild und seine drei gemessenen Kamera-Z. */
#define P_X0 59
#define P_X1 83
#define P_Y0 103
#define P_Y1 145
static const int g_player_vz[3] = { 9120, 9352, 9584 };

static int otz_bucket(int vz) { return (int)(((long long)1023 * vz) >> 12) >> 4; }

int main(void)
{
    printf("=== SONDE ROOM1050 Cut 6 — Maskentiefe gegen Spielertiefe ===\n");

    /* (a) Fuehrt das ORIGINAL fuer diesen Cut ueberhaupt Masken? */
    long rdt_n = 0;
    uint8_t *rdt = slurp("STAGE1/ROOM1050.RDT", &rdt_n);
    if (!rdt) return 1;
    unsigned n_cut = rdt[1];
    unsigned cam   = (unsigned)(rdt[0x24] | (rdt[0x25] << 8) | (rdt[0x26] << 16) | ((unsigned)rdt[0x27] << 24));
    printf("ROOM1050.RDT: nCut=%u  Kopf-Byte[7]=%u  cameraStart=0x%X\n", n_cut, rdt[7], cam);
    for (unsigned c = 0; c < n_cut; c++) {
        unsigned off = cam + c * 32 + 0x1C;
        unsigned pri = (unsigned)(rdt[off] | (rdt[off+1] << 8) | (rdt[off+2] << 16) | ((unsigned)rdt[off+3] << 24));
        re15_pri_cut_t cut = {0};
        int n = (pri + 4 <= (unsigned)rdt_n)
              ? re15_pri_parse_section(rdt, (size_t)rdt_n, pri, &cut) : 0;
        printf("  Cut %u: pri_offset=0x%08X  Bytes %02X %02X %02X %02X  -> %d Masken\n",
               c, pri, rdt[pri], rdt[pri+1], rdt[pri+2], rdt[pri+3], n);
    }

    /* (b) Die nachgereichten Masken. */
    long msk_n = 0;
    uint8_t *msk = slurp("MASKS/ROOM1050.MSK", &msk_n);
    if (!msk) { free(rdt); return 1; }
    uint32_t sect = re15_pri_msk_section_offset(msk, (size_t)msk_n, 6);
    re15_pri_cut_t pri = {0};
    int n = re15_pri_parse_section(msk, (size_t)msk_n, sect, &pri);
    printf("\nROOM1050.MSK Cut 6: Sektion @0x%X, %d Masken gebaut, %d gezeichnet\n",
           sect, n, pri.draw_count);

    printf("\nSpielerkasten Bild x%d..%d y%d..%d, Kamera-Z %d/%d/%d"
           "  (OT-Eimer %d/%d/%d)\n",
           P_X0, P_X1, P_Y0, P_Y1,
           g_player_vz[0], g_player_vz[1], g_player_vz[2],
           otz_bucket(g_player_vz[0]), otz_bucket(g_player_vz[1]), otz_bucket(g_player_vz[2]));

    int beruehrt = 0, immer = 0, nie = 0, teils = 0;
    printf("\n  Kachel                       Tiefe   z=Tiefe*64   verdeckt bei vz %d/%d/%d\n",
           g_player_vz[0], g_player_vz[1], g_player_vz[2]);
    for (int i = 0; i < pri.draw_count && i < n; i++) {
        const re15_pri_mask_t *m = &pri.masks[i];
        int x = (int16_t)m->dstX, y = (int16_t)m->dstY;
        int x1 = x + m->width, y1 = y + m->height;
        if (x1 <= P_X0 || x > P_X1 || y1 <= P_Y0 || y > P_Y1) continue;
        beruehrt++;
        int mz = re15_pri_mask_camera_z(m->depth);
        int v[3], s = 0;
        for (int k = 0; k < 3; k++) {
            /* PSX: depth < otz>>4.  PC: depth*64 < vz.  Beide werden ausgegeben. */
            v[k] = (m->depth < otz_bucket(g_player_vz[k])) ? 1 : 0;
            s += v[k];
        }
        if (s == 3) immer++; else if (s == 0) nie++; else teils++;
        printf("  x%3d..%3d y%3d..%3d %2dx%-2d  %4u   %6d      %s %s %s   %s\n",
               x, x1 - 1, y, y1 - 1, m->width, m->height, m->depth, mz,
               v[0] ? "JA" : "--", v[1] ? "JA" : "--", v[2] ? "JA" : "--",
               s == 3 ? "" : (s == 0 ? "<<< DECKT NIE" : "<<< DECKT NUR TEILWEISE"));
    }
    printf("\n  Kacheln im Spielerkasten: %d — immer deckend %d, teilweise %d, nie %d\n",
           beruehrt, immer, teils, nie);

    /* (c) Welche Tiefe waere noetig, damit JEDE Kachel den ganzen Spieler deckt? */
    int need = otz_bucket(g_player_vz[0]);           /* der NAECHSTE Messpunkt bindet */
    printf("  Damit alle Kacheln den ganzen Spieler decken, muss depth <= %d sein"
           " (= Kamera-Z <= %d).\n", need - 1, (need - 1) * 64);

    /* (d) Loecher: Bildpunkte des Spielerkastens, die GAR KEINE Kachel traegt. */
    {
        static unsigned char deck[240][320];
        memset(deck, 0, sizeof deck);
        for (int i = 0; i < pri.draw_count && i < n; i++) {
            const re15_pri_mask_t *m = &pri.masks[i];
            int x = (int16_t)m->dstX, y = (int16_t)m->dstY;
            for (int yy = y; yy < y + (int)m->height; yy++)
                for (int xx = x; xx < x + (int)m->width; xx++)
                    if (yy >= 0 && yy < 240 && xx >= 0 && xx < 320) deck[yy][xx] = 1;
        }
        int leer = 0, ges = 0;
        for (int yy = P_Y0; yy <= P_Y1; yy++)
            for (int xx = P_X0; xx <= P_X1; xx++) { ges++; if (!deck[yy][xx]) leer++; }
        printf("  Spielerkasten: %d von %d Bildpunkten tragen GAR KEINE Kachel.\n", leer, ges);
    }

    free(msk); free(rdt);
    return 0;
}
