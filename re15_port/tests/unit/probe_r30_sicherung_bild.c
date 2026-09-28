/* probe_r30_sicherung_bild.c — RIEGEL fuer Item-Bild und Icon der Sicherung (Runde 30, Thema H).
 *
 * Dossier: analysis/befunde_runde30/sicherung.md §3.5, §5.4.
 *
 * WAS ER FESTHAELT
 *   1. Die eingebackenen Bytes sind GENAU die Soll-Fassung: ITPS-Block 0x3000 B
 *      (soll/weg2n_itps_block_40.bin, sha256 33d01cb1...) = FNV-1a-64 0x48209C5BE598679A,
 *      Icon-Tile 1200 B (soll/weg2n_icon_tile_40.bin, sha256 d6a3248f...) =
 *      FNV-1a-64 0xB3949DDE2281728A.
 *   2. Der Block traegt die Rechtecke des RE1.5-Statusschirms: crect (0,489) 256x1,
 *      prect (832,256) 56x72 — das Fenster, in das der Foto-Lader @0x800c0258 laedt
 *      (LoadImage der EINGEBETTETEN Rechtecke @0x800c0280 / @0x800c02a0). Der
 *      ausgelieferte Block 0x40 traegt (0,480) / (0,0) = RE2-Format, deshalb war das
 *      CHECK-Foto leer.
 *   3. Bei +0x21A0 fuehrt der Block das Icon ein zweites Mal (wie 66 der 72
 *      ausgelieferten Bloecke).
 *   4. Einsetzen in KOPIEN der ausgelieferten Dateien veraendert in ITPS.ITP nur Bytes in
 *      0xC0000..0xC2FFF (gemessen 8904) und in ITEMALL.PIX nur 0x12C00..0x130AF
 *      (gemessen 389). Die Nachbarn 0x3F "Pocket Watch" und 0x41 "Spark Plug" bleiben
 *      byte-true.
 *   5. Ein zu kurzer Puffer bleibt ganz unberuehrt.
 *   6. Die FAULEN Lader der Engine (re15_itps_load / re15_itemall_load) setzen ebenfalls
 *      ein: re15_itps_pixel(0x40,0,0) = (224,224,224) (Rahmenwort 0x739C),
 *      re15_itps_pixel(0x40,4,4) = (0,0,56) (Hintergrundwort 0x1C00), und das rohe Tile
 *      0x40 ist das eingebackene Icon.
 *   7. Die PRUEF-FUNKTIONEN der Ladestellen (re15_sicherung.h, "PRUEFUNG AN DEN
 *      LADESTELLEN") unterscheiden: der ausgelieferte Stand weicht ab (Block 8904 Bytes,
 *      Tile 389 Bytes, Modal-Leser > 0 Punkte), der eingesetzte Stand in 0. Der Fall
 *      "Ladestelle ohne Einsetzen" wird nachgestellt, indem die UNVERAENDERTEN Dateibytes
 *      ueber re15_itps_set_data / re15_itemall_set_pix uebergeben werden — genau das, was
 *      main.c tut, wenn der Einsetz-Aufruf fehlt (Mutationsprobe M2 des Gegenpruefers).
 *      ⛔ Dieser Riegel prueft die FUNKTIONEN. Dass die vier PC-Ladestellen sie benutzen
 *      und 0 melden, prueft integration_r30_sicherung_bild mit der echten exe.
 *
 * Rueckgabe 0 = alles bestanden.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_itps.h"
#include "re15_item_icon.h"
#include "re15_sicherung.h"

extern uint8_t *re15_asset_read_file(const char *path, int *out_size);

#define BLOCK   0x3000
#define TILE    1200
#define ITPS_AB 0xC0000            /* 0x40 * 0x3000 */
#define PIX_AB  0x12C00            /* 0x40 * 1200   */

static int fehler = 0;

static void pruefe(const char *was, int ok)
{
    printf("   [%s] %s\n", ok ? "OK  " : "FEHL", was);
    if (!ok) fehler++;
}

static uint64_t fnv1a64(const uint8_t *b, int n)
{
    uint64_t h = 0xcbf29ce484222325ull;
    for (int i = 0; i < n; i++) { h ^= b[i]; h *= 0x100000001b3ull; }
    return h;
}

static unsigned u16(const uint8_t *b) { return (unsigned)b[0] | ((unsigned)b[1] << 8); }

/* zaehlt die abweichenden Bytes und merkt das erste/letzte */
static int abweichung(const uint8_t *a, const uint8_t *b, int n, int *erstes, int *letztes)
{
    int k = 0;
    *erstes = -1; *letztes = -1;
    for (int i = 0; i < n; i++)
        if (a[i] != b[i]) { if (*erstes < 0) *erstes = i; *letztes = i; k++; }
    return k;
}

int main(void)
{
    /* ---- 6. ZUERST die faulen Lader (prozessweiter Zustand: vor jedem set_data) ---- */
    printf("== 6. faule Lader der Engine ==\n");
    {
        uint8_t r = 1, g = 2, b = 3;
        int ok = re15_itps_pixel(RE15_SICHERUNG_ITEM, 0, 0, &r, &g, &b);
        printf("   re15_itps_pixel(0x40,0,0) -> %d (%d,%d,%d)\n", ok, r, g, b);
        pruefe("Rahmenpunkt (0,0) = (224,224,224) = Wort 0x739C", ok && r == 224 && g == 224 && b == 224);
        r = 1; g = 2; b = 3;
        ok = re15_itps_pixel(RE15_SICHERUNG_ITEM, 4, 4, &r, &g, &b);
        printf("   re15_itps_pixel(0x40,4,4) -> %d (%d,%d,%d)\n", ok, r, g, b);
        pruefe("Innenpunkt (4,4) = (0,0,56) = Wort 0x1C00", ok && r == 0 && g == 0 && b == 56);

        int tn = 0;
        const uint8_t *soll_tile = re15_sicherung_icon_tile_bytes(&tn);
        const uint8_t *roh = re15_itemall_tile_raw(RE15_SICHERUNG_ITEM);
        pruefe("re15_itemall_tile_raw(0x40) liefert das eingebackene Icon",
               roh != NULL && tn == TILE && memcmp(roh, soll_tile, TILE) == 0);
    }

    /* ---- 1. Bytes ---- */
    printf("\n== 1. eingebackene Bytes ==\n");
    int bn = 0, tn = 0;
    const uint8_t *blk = re15_sicherung_itps_block_bytes(&bn);
    const uint8_t *til = re15_sicherung_icon_tile_bytes(&tn);
    printf("   Block %d B FNV 0x%016llX ; Tile %d B FNV 0x%016llX\n",
           bn, (unsigned long long)fnv1a64(blk, bn), tn, (unsigned long long)fnv1a64(til, tn));
    pruefe("Block 0x3000 B, FNV-1a-64 0x48209C5BE598679A",
           bn == BLOCK && fnv1a64(blk, bn) == 0x48209C5BE598679Aull);
    pruefe("Tile 1200 B, FNV-1a-64 0xB3949DDE2281728A",
           tn == TILE && fnv1a64(til, tn) == 0xB3949DDE2281728Aull);

    /* ---- 2. Kopf ---- */
    printf("\n== 2. Kopf des Blocks ==\n");
    printf("   magic 0x%X flag 0x%X | crect (%u,%u) %ux%u | prect (%u,%u) %ux%u\n",
           u16(blk), u16(blk + 4), u16(blk + 12), u16(blk + 14), u16(blk + 16), u16(blk + 18),
           u16(blk + 0x218), u16(blk + 0x21A), u16(blk + 0x21C), u16(blk + 0x21E));
    pruefe("TIM 8bpp + CLUT (magic 0x10, flag 0x09)", u16(blk) == 0x10 && u16(blk + 4) == 0x09);
    pruefe("crect (0,489) 256x1", u16(blk + 12) == 0 && u16(blk + 14) == 489 &&
                                  u16(blk + 16) == 256 && u16(blk + 18) == 1);
    pruefe("prect (832,256) 56x72", u16(blk + 0x218) == 832 && u16(blk + 0x21A) == 256 &&
                                    u16(blk + 0x21C) == 56 && u16(blk + 0x21E) == 72);
    {
        int null_wort = 0;
        for (int i = 0; i < 256; i++) if (u16(blk + 0x14 + i * 2) == 0) null_wort++;
        pruefe("kein CLUT-Wort ist 0x0000 (waere durchsichtig, itps_common.c)", null_wort == 0);
    }

    /* ---- 3. Icon-Kopie ---- */
    printf("\n== 3. Icon im Block ==\n");
    pruefe("Block +0x21A0 = Icon-Tile (1200 von 1200 B)", memcmp(blk + 0x21A0, til, TILE) == 0);

    /* ---- 4. Einsetzen in Kopien der ausgelieferten Dateien ---- */
    printf("\n== 4. Einsetzen ==\n");
    int isz = 0, psz = 0;
    uint8_t *itps = re15_asset_read_file("shared_assets/PSX/ITEM/ITPS.ITP", &isz);
    uint8_t *pix  = re15_asset_read_file("shared_assets/PSX/DATA/ITEMALL.PIX", &psz);
    if (!itps || !pix) { printf("   ausgelieferte Dateien nicht lesbar\n"); return 77; }
    pruefe("ITPS.ITP 884736 B, ITEMALL.PIX 86400 B", isz == 884736 && psz == 86400);
    pruefe("der AUSGELIEFERTE Block 0x40 traegt crect (0,480) / prect (0,0) (RE2-Format)",
           u16(itps + ITPS_AB + 14) == 480 && u16(itps + ITPS_AB + 0x218) == 0 &&
           u16(itps + ITPS_AB + 0x21A) == 0);
    uint8_t *itps2 = (uint8_t *)malloc((size_t)isz);
    uint8_t *pix2  = (uint8_t *)malloc((size_t)psz);
    if (!itps2 || !pix2) return 1;
    memcpy(itps2, itps, (size_t)isz);
    memcpy(pix2, pix, (size_t)psz);

    re15_sicherung_bild_einsetzen(itps2, isz);
    re15_sicherung_icon_einsetzen(pix2, psz);
    int e, l;
    int k = abweichung(itps, itps2, isz, &e, &l);
    printf("   ITPS.ITP:    %d Bytes veraendert, erstes 0x%05X, letztes 0x%05X\n", k, e, l);
    pruefe("ITPS.ITP: nur Bytes in 0xC0000..0xC2FFF veraendert", k > 0 && e >= ITPS_AB && l <= ITPS_AB + BLOCK - 1);
    pruefe("ITPS.ITP: 8904 Bytes veraendert", k == 8904);
    pruefe("ITPS.ITP: Block 0x40 der Kopie = eingebackener Block", memcmp(itps2 + ITPS_AB, blk, BLOCK) == 0);
    k = abweichung(pix, pix2, psz, &e, &l);
    printf("   ITEMALL.PIX: %d Bytes veraendert, erstes 0x%05X, letztes 0x%05X\n", k, e, l);
    pruefe("ITEMALL.PIX: nur Bytes in 0x12C00..0x130AF veraendert", k > 0 && e >= PIX_AB && l <= PIX_AB + TILE - 1);
    pruefe("ITEMALL.PIX: 389 Bytes veraendert", k == 389);
    pruefe("ITEMALL.PIX: Tile 0x40 der Kopie = eingebackenes Tile", memcmp(pix2 + PIX_AB, til, TILE) == 0);

    /* zweimal einsetzen aendert nichts mehr */
    {
        uint8_t *itps3 = (uint8_t *)malloc((size_t)isz);
        if (!itps3) return 1;
        memcpy(itps3, itps2, (size_t)isz);
        re15_sicherung_bild_einsetzen(itps3, isz);
        pruefe("zweites Einsetzen aendert nichts", memcmp(itps3, itps2, (size_t)isz) == 0);
        free(itps3);
    }

    /* ---- 5. zu kurzer Puffer ---- */
    printf("\n== 5. zu kurzer Puffer ==\n");
    memcpy(itps2, itps, (size_t)isz);
    memcpy(pix2, pix, (size_t)psz);
    re15_sicherung_bild_einsetzen(itps2, ITPS_AB + BLOCK - 1);
    re15_sicherung_icon_einsetzen(pix2, PIX_AB + TILE - 1);
    pruefe("ITPS-Puffer 1 Byte zu kurz: unberuehrt", memcmp(itps2, itps, (size_t)isz) == 0);
    pruefe("ITEMALL-Puffer 1 Byte zu kurz: unberuehrt", memcmp(pix2, pix, (size_t)psz) == 0);
    re15_sicherung_bild_einsetzen(NULL, isz);
    re15_sicherung_icon_einsetzen(NULL, psz);
    re15_sicherung_bild_einsetzen(itps2, -1);
    pruefe("NULL und negative Groesse: kein Absturz, Puffer unberuehrt",
           memcmp(itps2, itps, (size_t)isz) == 0);

    /* ---- 7. Pruef-Funktionen der Ladestellen ---- */
    printf("\n== 7. Pruefung an den Ladestellen ==\n");
    {
        /* Modal-/Icon-Leser nach den faulen Ladern aus Abschnitt 6 */
        int m_faul = re15_sicherung_modal_bild_abweichung();
        int i_faul = re15_sicherung_icon_leser_abweichung();

        memcpy(itps2, itps, (size_t)isz);
        memcpy(pix2, pix, (size_t)psz);
        int b_roh = re15_sicherung_bild_abweichung(itps2, isz);
        int t_roh = re15_sicherung_icon_abweichung(pix2, psz);
        re15_sicherung_bild_einsetzen(itps2, isz);
        re15_sicherung_icon_einsetzen(pix2, psz);
        int b_bau = re15_sicherung_bild_abweichung(itps2, isz);
        int t_bau = re15_sicherung_icon_abweichung(pix2, psz);
        printf("   Puffer: Block ausgeliefert %d / eingesetzt %d Bytes ; Tile ausgeliefert %d / "
               "eingesetzt %d Bytes\n", b_roh, b_bau, t_roh, t_bau);
        pruefe("Block 0x40 ausgeliefert: 8904 Bytes Abweichung", b_roh == 8904);
        pruefe("Tile 0x40 ausgeliefert: 389 Bytes Abweichung", t_roh == 389);
        pruefe("eingesetzt: Block 0 und Tile 0", b_bau == 0 && t_bau == 0);
        pruefe("zu kurz / NULL: -1",
               re15_sicherung_bild_abweichung(itps2, ITPS_AB + BLOCK - 1) == -1 &&
               re15_sicherung_icon_abweichung(pix2, PIX_AB + TILE - 1) == -1 &&
               re15_sicherung_bild_abweichung(NULL, isz) == -1 &&
               re15_sicherung_icon_abweichung(NULL, psz) == -1);

        /* Ladestelle OHNE Einsetzen: die unveraenderten Dateibytes gehen an die Leser */
        re15_itps_set_data(itps, isz);
        re15_itemall_set_pix(pix, psz);
        int m_roh = re15_sicherung_modal_bild_abweichung();
        int i_roh = re15_sicherung_icon_leser_abweichung();
        /* Ladestelle MIT Einsetzen */
        re15_itps_set_data(itps2, isz);
        re15_itemall_set_pix(pix2, psz);
        int m_bau = re15_sicherung_modal_bild_abweichung();
        int i_bau = re15_sicherung_icon_leser_abweichung();
        printf("   Leser: faul %d/%d, ohne Einsetzen %d/%d, mit Einsetzen %d/%d "
               "(Modal-Punkte von %d / Icon-Bytes von 1200)\n",
               m_faul, i_faul, m_roh, i_roh, m_bau, i_bau, RE15_ITPS_W * RE15_ITPS_H);
        pruefe("faule Lader: Modal-Leser 0, Icon-Leser 0", m_faul == 0 && i_faul == 0);
        pruefe("Ladestelle ohne Einsetzen: Modal-Leser > 0 Punkte", m_roh > 0);
        pruefe("Ladestelle ohne Einsetzen: Icon-Leser 389 Bytes", i_roh == 389);
        pruefe("Ladestelle mit Einsetzen: Modal-Leser 0, Icon-Leser 0", m_bau == 0 && i_bau == 0);
    }

    /* ⛔ Die Leser zeigen jetzt auf itps2/pix2 — danach wird nichts mehr gelesen. */
    free(itps2); free(pix2); free(itps); free(pix);
    printf("\n%s - %d Pruefung(en) gerissen\n", fehler ? "FEHLGESCHLAGEN" : "ALLES BESTANDEN", fehler);
    return fehler ? 1 : 0;
}
