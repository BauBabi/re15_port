/* probe_atlas_kollision.c - Wie viele Blickwinkel waren vom Atlas-Verwechsler betroffen?
 *
 * Nachgezeichnete Masken (R15M-Container) tragen srcX/srcY, die fuer den EDITOR-Atlas
 * (MASKS/ROOM..._PRI##.TIM) gepackt wurden. Der Lader nahm aber zuerst den Atlas aus dem
 * BSS-Klotz. Betroffen ist also jeder Cut, der BEIDES hat: eine nachgezeichnete Sektion
 * UND einen Original-Atlas. Genau die zeigten Masken, die nichts verdecken.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_pri.h"
#include "re15_room.h"
#include "re15_sld.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

int main(void)
{
    static const char *raeume[] = {
        "ROOM1000","ROOM1010","ROOM1020","ROOM1030","ROOM1040","ROOM1050","ROOM1060",
        "ROOM1070","ROOM1080","ROOM1090","ROOM10A0","ROOM10B0","ROOM10C0","ROOM10D0",
        "ROOM10E0","ROOM10F0","ROOM1100","ROOM1110","ROOM1120","ROOM1130","ROOM1140",
        "ROOM1150","ROOM1160","ROOM1170","ROOM1180","ROOM1190","ROOM11A0","ROOM11B0",
        "ROOM11C0","ROOM1200","ROOM1210","ROOM1220","ROOM1230","ROOM1240" };
    long cuts = 0, original = 0, nachgez = 0, betroffen = 0;
    printf("Cuts mit nachgezeichneten Masken UND vorhandenem Original-Atlas:\n");
    for (unsigned r = 0; r < sizeof raeume / sizeof raeume[0]; r++) {
        char p[600]; size_t sz = 0;
        snprintf(p, sizeof p, "%s/STAGE1/%s.RDT", RE15_XSTR(RE15_ASSETS_PATH), raeume[r]);
        uint8_t *rdt_b = read_file(p, &sz);
        if (!rdt_b) continue;
        re15_rdt_t rdt;
        if (re15_rdt_parse(rdt_b, sz, &rdt) != 0) { free(rdt_b); continue; }
        size_t msz = 0;
        snprintf(p, sizeof p, "%s/MASKS/%s.MSK", RE15_XSTR(RE15_ASSETS_PATH), raeume[r]);
        uint8_t *msk = read_file(p, &msz);
        int r_bet = 0;
        for (int c = 0; c < rdt.cut_count; c++) {
            cuts++;
            re15_pri_cut_t o = {0};
            int n_orig = re15_pri_parse_section(rdt_b, sz, rdt.cuts[c].pri_offset, &o);
            if (n_orig > 0) { original++; continue; }      /* Original fuehrt die Geometrie */
            if (!msk) continue;
            uint32_t moff = re15_pri_msk_section_offset(msk, msz, c);
            if (!moff) continue;
            re15_pri_cut_t m = {0};
            if (re15_pri_parse_section(msk, msz, moff, &m) <= 0) continue;
            nachgez++;
            /* Hat der Cut einen Original-Atlas? Massgeblich ist die SLD-Tabelle des
             * Stage-Overlays - der Laufzeit-Auszug aus dem BSS-Klotz gewinnt gegen die
             * Seitendatei UND gegen den nachgezeichneten Atlas. */
            uint16_t used = 0;
            unsigned rid = 0; sscanf(raeume[r] + 4, "%x", &rid);
            if (re15_sld_used_len_tab(1, (int)((rid >> 4) & 0xFF), c, &used) == RE15_SLD_OK
                && used > 0) { betroffen++; r_bet++; }
        }
        if (r_bet) printf("   %-9s %d betroffene Winkel\n", raeume[r], r_bet);
        free(msk); free(rdt_b);
    }
    printf("\n%ld Winkel gesamt | %ld mit Original-Geometrie | %ld nachgezeichnet | "
           "davon %ld mit Original-Atlas (= betroffen)\n", cuts, original, nachgez, betroffen);
    return 0;
}
