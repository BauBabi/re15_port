/* probe_1030_bandsweep.c — GEGENPRUEFUNG zum Befund "room1030-fall":
 * Sweept ueber x und testet, ob ein Aktor mit SCA-Maske 4 bzw. 8 das Torband
 * (SCA-Zelle #28, u0 = 0xF7 nach `37 03 06 f7` @ROOM1030.RDT 0x2004) von Sueden
 * nach Norden passieren kann. Radius 400 (Box @STAGE1.BIN 0x1f778), band 0.
 * Die u0-Setzung wird hier HAND-angelegt (die Skript-VM laeuft nicht mit), damit
 * der Test genau die Frage "haelt das Band ueber die ganze Breite?" beantwortet. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_rdt.h"
#include "re15_collision.h"

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long s = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)s);
    if (fread(b, 1, (size_t)s, f) != (size_t)s) { free(b); fclose(f); return NULL; }
    fclose(f); *n = (size_t)s; return b;
}

int main(void)
{
    size_t n = 0;
    uint8_t *d = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1030.RDT", &n);
    if (!d) { printf("RDT fehlt\n"); return 1; }

    /* --- Roh-Beleg: die zwei Sca_id_set-Woerter --------------------------------- */
    printf("RDT 0x2000: %02x %02x %02x %02x   0x2004: %02x %02x %02x %02x\n",
           d[0x2000],d[0x2001],d[0x2002],d[0x2003], d[0x2004],d[0x2005],d[0x2006],d[0x2007]);

    re15_rdt_t r; memset(&r,0,sizeof r);
    if (re15_rdt_parse(d, n, &r) != 0) { printf("parse failed\n"); return 1; }
    printf("SCA-Zeiger @0x20 = 0x%04x  counts=%d/%d/%d/%d/%d  total=%d\n",
           (unsigned)(d[0x20]|(d[0x21]<<8)|(d[0x22]<<16)|(d[0x23]<<24)),
           r.sca_rgn[0],r.sca_rgn[1],r.sca_rgn[2],r.sca_rgn[3],r.sca_rgn[4], r.sca_count);

    int rgn3_start = r.sca_rgn[0]+r.sca_rgn[1]+r.sca_rgn[2];
    int cell28 = rgn3_start + 6;
    int cell18 = r.sca_rgn[0]+r.sca_rgn[1] + 6;
    printf("rgn3 beginnt bei #%d -> rgn3/idx6 = Zelle #%d ; rgn2/idx6 = Zelle #%d\n",
           rgn3_start, cell28, cell18);

    const re15_sca_entry_t *c = &r.sca[cell28];
    printf("Zelle #%d ON-DISC: typ=%u u0=0x%02x u1=0x%02x floor=0x%02x "
           "x[%d..%d] z[%d..%d]\n", cell28, c->type, c->u0, c->u1, c->floor,
           c->x, c->x + (int)c->width, c->z, c->z + (int)c->density);

    /* Sca_id_set nachziehen (Handler LAB_8004175c: sb a1,9(v0) @0x8004179c) */
    ((re15_sca_entry_t *)&r.sca[cell18])->u0 = 0xf7;
    ((re15_sca_entry_t *)&r.sca[cell28])->u0 = 0xf7;
    printf("nach Sca_id_set: #%d u0=0x%02x, #%d u0=0x%02x\n",
           cell18, r.sca[cell18].u0, cell28, r.sca[cell28].u0);

    /* --- Sweep: von z = -24838 (Suedklemme) 400 nach Norden, je Maske ----------- */
    const int32_t R = 400;
    for (unsigned mask = 4; mask <= 8; mask += 4) {
        int blocked = 0, through = 0, first_ok = 999999, last_ok = -999999;
        for (int32_t x = -21000; x <= 5500; x += 25) {
            int32_t px = x, pz = -24838 + 400;      /* Schritt +400z nach Norden */
            re15_collision_constrain_enemy(&r, x, -24838, &px, &pz, R, 0, mask);
            if (pz >= -24438) { through++; if (x<first_ok) first_ok=x; if (x>last_ok) last_ok=x;
                 if (mask==4) printf("    frei(maske4) x=%d\n",(int)x); }
            else blocked++;
        }
        printf("Maske %u (0xF7 & %u = %u): %d x-Positionen geklemmt, %d frei",
               mask, mask, 0xf7u & mask, blocked, through);
        if (through) printf("   (frei von x=%d bis x=%d)", first_ok, last_ok);
        printf("\n");
    }

    /* --- Konkret: Positiv-Kontroll-Punkt x = -12100 ----------------------------- */
    for (unsigned mask = 4; mask <= 8; mask += 4) {
        int32_t px = -12100, pz = -24838 + 400;
        re15_collision_constrain_enemy(&r, -12100, -24838, &px, &pz, R, 0, mask);
        printf("x=-12100 maske=%u: (-12100,-24438) -> (%d,%d)\n", mask, px, pz);
    }
    /* --- und der gemessene Steckenbleiber-Ort ---------------------------------- */
    for (unsigned mask = 4; mask <= 8; mask += 4) {
        int32_t px = -10737, pz = -24838 + 400;
        re15_collision_constrain_enemy(&r, -10737, -24838, &px, &pz, R, 0, mask);
        printf("x=-10737 maske=%u: (-10737,-24438) -> (%d,%d)\n", mask, px, pz);
    }
    return 0;
}
