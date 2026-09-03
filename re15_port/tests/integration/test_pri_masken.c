/* NACHGEZEICHNETE VORDERGRUND-MASKEN — der Ladepfad, gemessen statt geglaubt.
 *
 * Befund vom 2026-09-03 an der Auslieferung v0.3.97: KEINE der 266 gruenen Pruefungen
 * laedt eine .MSK, prueft den R15M-Container oder den Riegel "das Original hat Vorrang".
 * Das Gruen belegte nur, dass der Rest nicht kaputtging — ueber die 32 ausgelieferten
 * Maskendateien sagte es nichts. Dieser Test schliesst genau diese Luecke.
 *
 * Vier Phasen, von der reinen Funktion zur ausgelieferten Datenlage:
 *
 *   1  Der Container-Leser gegen ABSICHTLICH kaputte Eingaben. Ein beschaedigter
 *      Container muss 0 liefern (= kein Nachschlag), nicht einen zufaelligen Offset in
 *      den Original-Parser geben.
 *   2  Jede ausgelieferte MASKS/ROOM*.MSK laedt und parst.
 *   3  ⛔ DER VORRANG-RIEGEL. Eine nachgereichte Sektion darf NUR dort greifen, wo das
 *      Original eine NULL-Sektion (FF FF FF FF) fuehrt. Traefe sie je einen Cut, den die
 *      Kuenstler bearbeitet haben, waere dieser Cut nicht mehr byte-true — und genau das
 *      ist die Zusage in re15_pri.h. Geprueft wird gegen die echten RDTs.
 *   4  Die geparsten Masken sind geometrisch plausibel (Kapazitaet, Ausdehnung).
 *
 * ⛔ ZUR ABGRENZUNG: der Test prueft den LADEPFAD und die DATENLAGE, nicht die
 * Bildwirkung. Ob eine nachgezeichnete Maske das Richtige verdeckt, ist eine optische
 * Frage und bleibt bei der gdigrab-A/B-Messung (RE15_NO_PRI=1). Was hier gruen ist,
 * heisst: der Container ist heil, keine Maske ueberschreibt ein Original, und die
 * Geometrie liegt im Rahmen.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_pri.h"
#include "re15_rdt.h"
#include "re15_camera.h"

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

static unsigned char *slurp(const char *pfad, size_t *n)
{
    FILE *f = fopen(pfad, "rb");
    unsigned char *p;
    long len;
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); len = ftell(f); fseek(f, 0, SEEK_SET);
    if (len <= 0) { fclose(f); return NULL; }
    p = (unsigned char *)malloc((size_t)len);
    if (!p) { fclose(f); return NULL; }
    if (fread(p, 1, (size_t)len, f) != (size_t)len) { free(p); fclose(f); return NULL; }
    fclose(f);
    *n = (size_t)len;
    return p;
}

static void u32le(unsigned char *p, uint32_t v)
{
    p[0] = (unsigned char)(v & 0xFF);       p[1] = (unsigned char)((v >> 8) & 0xFF);
    p[2] = (unsigned char)((v >> 16) & 0xFF); p[3] = (unsigned char)((v >> 24) & 0xFF);
}

int main(void)
{
    int st, r, dateien = 0, cuts_gesamt = 0, sektionen = 0, masken_gesamt = 0;
    int roh_fehlt = 0, ueberschreibt = 0, unparsbar = 0, unplausibel = 0;
    int groesste = 0;
    /* NEGATIV-KONTROLLE: wie viele Cuts DERSELBEN Raeume tragen Original-Masken?
     * Waere diese Zahl 0, koennte der Vorrang-Riegel gar nicht anschlagen und waere
     * leeres Gruen — er wuerde dann nur belegen, dass der Original-Parser nichts
     * findet, nicht dass keine Maske ein Original ueberschreibt. */
    int orig_mit_masken = 0, orig_geprueft = 0;

    /* ================= PHASE 1: der Container-Leser =========================
     * Ein gueltiger Container mit zwei Cuts; Cut 0 traegt eine Sektion bei 0x18,
     * Cut 1 keine. Danach wird gezielt je EIN Feld verdorben. */
    {
        unsigned char gut[0x20];
        unsigned char kaputt[0x20];
        uint32_t off;
        int n_ok = 0, n_abgewehrt = 0;

        memset(gut, 0, sizeof gut);
        memcpy(gut, "R15M", 4);
        u32le(gut + 4, 1);          /* version   */
        u32le(gut + 8, 2);          /* cut_count */
        u32le(gut + 12, 0x18);      /* Cut 0 -> Sektion bei 0x18 */
        u32le(gut + 16, 0);         /* Cut 1 -> nichts           */

        off = re15_pri_msk_section_offset(gut, sizeof gut, 0);
        if (off == 0x18u) n_ok++;
        printf("  [Leser] gueltiger Container, Cut 0: Offset 0x%x (erwartet 0x18)\n",
               (unsigned)off);
        if (re15_pri_msk_section_offset(gut, sizeof gut, 1) == 0u) n_ok++;

        /* Jede dieser Eingaben MUSS 0 liefern. */
        {
            struct { const char *was; int cut; size_t size; } faelle[] = {
                { "falsche Kennung",        0, sizeof gut },
                { "Version 2",              0, sizeof gut },
                { "Cut-Index negativ",     -1, sizeof gut },
                { "Cut-Index zu gross",     2, sizeof gut },
                { "Tabelle abgeschnitten",  0, 14 },
                { "Offset hinter dem Ende", 0, sizeof gut },
                { "Container zu kurz",      0, 11 },
            };
            size_t i;
            for (i = 0; i < sizeof faelle / sizeof faelle[0]; i++) {
                memcpy(kaputt, gut, sizeof gut);
                switch (i) {
                case 0: kaputt[1] = 'X'; break;
                case 1: u32le(kaputt + 4, 2); break;
                case 5: u32le(kaputt + 12, 0xFFFFFFF0u); break;
                default: break;               /* 2/3/4/6 variieren Index bzw. Groesse */
                }
                off = re15_pri_msk_section_offset(kaputt, faelle[i].size, faelle[i].cut);
                if (off == 0u) n_abgewehrt++;
                else printf("     DURCHGELASSEN: %s -> Offset 0x%x\n",
                            faelle[i].was, (unsigned)off);
            }
            printf("  [Leser] %d von 7 kaputten Eingaben abgewehrt\n", n_abgewehrt);
            CHECK("der gueltige Container liefert die richtigen Offsets", n_ok == 2);
            CHECK("jede beschaedigte Eingabe liefert 0 statt eines Offsets",
                  n_abgewehrt == 7);
            CHECK("ein NULL-Zeiger stuerzt nicht ab",
                  re15_pri_msk_section_offset(NULL, 100, 0) == 0u);
        }
    }

    /* ================= PHASEN 2-4: die AUSGELIEFERTEN Masken ================
     * Aufgezaehlt wird wie die Engine selbst: MASKS/ROOM%04X.MSK je Raum. */
    for (st = 1; st <= 6; st++) {
      for (r = 0; r < 0x400; r += 0x10) {
        int var;
        /* ⛔ BEIDE SZENARIO-VARIANTEN. Die Engine setzt den Dateinamen aus
         * g_current_room_id zusammen, und das TRAEGT das Szenario-Bit im untersten
         * Nibble (MASKS/ROOM%04X.MSK, main.c:4478). Nur ueber r += 0x10 zu laufen saehe
         * die Haelfte — 16 der 32 ausgelieferten Dateien — und waere gruen, ohne eine
         * einzige Variante angefasst zu haben. */
        for (var = 0; var < 2; var++) {
            unsigned rid = (unsigned)(st << 12) | (unsigned)r | (unsigned)var;
            char pfad[600];
            unsigned char *msk, *roh;
            size_t n_msk = 0, n_roh = 0;
            re15_rdt_t rdt;
            uint32_t cuts;
            int c, hat_rdt;

            snprintf(pfad, sizeof pfad, "%s/MASKS/ROOM%04X.MSK", RE15_ASSET_PSX_DIR, rid);
            msk = slurp(pfad, &n_msk);
            if (!msk) continue;
            dateien++;

            cuts = (uint32_t)msk[8] | ((uint32_t)msk[9] << 8)
                 | ((uint32_t)msk[10] << 16) | ((uint32_t)msk[11] << 24);
            cuts_gesamt += (int)cuts;

            /* Das RDT desselben Raums — fuer den Vorrang-Riegel. */
            snprintf(pfad, sizeof pfad, "%s/STAGE%u/ROOM%04X.RDT",
                     RE15_ASSET_PSX_DIR, (rid >> 12) & 0xF, rid);
            roh = slurp(pfad, &n_roh);
            hat_rdt = (roh && re15_rdt_parse(roh, n_roh, &rdt) >= 0);
            if (!hat_rdt) roh_fehlt++;

            /* NEGATIV-KONTROLLE (s.o.): tragen Cuts DIESER Raeume ueberhaupt
             * Original-Masken? Nur dann kann der Vorrang-Riegel anschlagen. */
            if (hat_rdt) {
                int k;
                for (k = 0; k < rdt.cut_count; k++) {
                    re15_pri_cut_t o;
                    memset(&o, 0, sizeof o);
                    orig_geprueft++;
                    if (re15_pri_parse_section(roh, n_roh, rdt.cuts[k].pri_offset, &o) > 0)
                        orig_mit_masken++;
                }
            }

            for (c = 0; c < (int)cuts; c++) {
                re15_pri_cut_t nach;
                uint32_t off = re15_pri_msk_section_offset(msk, n_msk, c);
                int n, i;
                if (!off) continue;
                sektionen++;

                memset(&nach, 0, sizeof nach);
                n = re15_pri_parse_section(msk, n_msk, off, &nach);
                if (n <= 0) {
                    unparsbar++;
                    printf("     UNPARSBAR: ROOM%04X Cut %d bei 0x%x\n", rid, c, (unsigned)off);
                    continue;
                }
                masken_gesamt += n;
                if (n > groesste) groesste = n;

                /* --- PHASE 3: hat das ORIGINAL hier wirklich nichts? --------
                 * Der Ladepfad in main.c reicht die Seitendaten nur nach, wenn der
                 * Original-Parser 0 Masken liefert. Traefe eine nachgereichte Sektion
                 * einen bearbeiteten Cut, waere er nicht mehr byte-true. */
                if (hat_rdt && c < rdt.cut_count) {
                    re15_pri_cut_t orig;
                    int n_orig;
                    memset(&orig, 0, sizeof orig);
                    n_orig = re15_pri_parse_section(roh, n_roh,
                                                    rdt.cuts[c].pri_offset, &orig);
                    if (n_orig > 0) {
                        ueberschreibt++;
                        printf("     UEBERSCHREIBT ORIGINAL: ROOM%04X Cut %d "
                               "(Original %d Masken @0x%x)\n",
                               rid, c, n_orig, (unsigned)rdt.cuts[c].pri_offset);
                    }
                }

                /* --- PHASE 4: Geometrie im Rahmen -------------------------- */
                if (n > RE15_PRI_MAX_MASKS_PER_CUT || nach.draw_count > n) {
                    unplausibel++;
                    printf("     ZAHLEN SCHIEF: ROOM%04X Cut %d n=%d draw=%d\n",
                           rid, c, n, nach.draw_count);
                    continue;
                }
                for (i = 0; i < n; i++) {
                    const re15_pri_mask_t *m = &nach.masks[i];
                    /* Der Vordergrund-Atlas ist eine TIM-Seite; eine Maske, die groesser
                     * als eine ganze Bildschirmseite ist oder keine Ausdehnung hat, ist
                     * kaputt. 320x240 ist das PSX-Bild. */
                    if (m->width == 0 || m->height == 0 ||
                        m->width > 320 || m->height > 240 ||
                        m->dstX > 320 || m->dstY > 240) {
                        unplausibel++;
                        printf("     MASKE SCHIEF: ROOM%04X Cut %d #%d "
                               "%ux%u nach (%u,%u)\n", rid, c, i,
                               (unsigned)m->width, (unsigned)m->height,
                               (unsigned)m->dstX, (unsigned)m->dstY);
                        break;
                    }
                }
            }
            free(msk);
            free(roh);
        }
      }
    }

    printf("  [Masken] %d Dateien, %d Cuts, %d nachgereichte Sektionen, %d Masken "
           "(groesste Sektion %d)\n",
           dateien, cuts_gesamt, sektionen, masken_gesamt, groesste);
    printf("  [Masken] %d unparsbar, %d ueberschreiben ein Original, %d unplausibel, "
           "%d ohne RDT\n", unparsbar, ueberschreibt, unplausibel, roh_fehlt);

    /* ⛔ ABDECKUNG ZUERST. Eine Schiene, die nichts gesehen hat, ist gruen und wertlos —
     * genau so blieb die Maskenarbeit bis v0.3.97 ungeprueft. */
    /* Gemessen am 2026-09-03: 32 Dateien (16 Raeume x 2 Szenario-Varianten), 232
     * nachgereichte Sektionen. Die Schranke steht auf diesem Stand, damit eine stille
     * Halbierung auffaellt — etwa wenn der Generator nur die geraden Varianten neu
     * schreibt und Szenario B seine Verdeckungen verliert. */
    CHECK("beide Szenario-Varianten aller Maskenraeume sind da", dateien >= 32);
    CHECK("sie reichen auch wirklich Sektionen nach", sektionen >= 200);
    CHECK("jede nachgereichte Sektion laesst sich parsen", unparsbar == 0);
    printf("  [Kontrolle] von %d Original-Cuts dieser Raeume tragen %d eigene Masken\n",
           orig_geprueft, orig_mit_masken);
    /* ⛔ ERST DIE KONTROLLE, DANN DER RIEGEL. Faende der Original-Parser in diesen
     * Raeumen NIRGENDS Masken, waere die Null darunter leeres Gruen. */
    CHECK("der Original-Parser findet in diesen Raeumen ueberhaupt Masken",
          orig_mit_masken > 0);
    CHECK("KEINE nachgereichte Sektion trifft einen Cut mit Original-Masken",
          ueberschreibt == 0);
    CHECK("jede Maske liegt in Kapazitaet und Bildflaeche", unplausibel == 0);
    CHECK("zu jeder Maskendatei gibt es das RDT ihres Raums", roh_fehlt == 0);

    printf(g_fail ? "FAIL\n" : "OK\n");
    return g_fail;
}
