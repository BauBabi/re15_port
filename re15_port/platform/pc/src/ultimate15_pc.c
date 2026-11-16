/* ultimate15_pc.c — Runde 41 Optionen: PC-Seite der Option "ULTIMATE 1.5" (re15_ultimate15.h).
 * Liest die unveraenderten Disc-Kopien unter shared_assets/PSX/DISC40/ ueber die gemeinsame CD-Wurzel
 * (asset_root_pc.h), baut die ULTIMATE-Puffer und liefert die Disc-Weltmodelle. Fehlt eine Disc-Datei, bleibt
 * alles beim heutigen Stand (Rueckgabe NULL / 0) und eine Zeile geht nach stderr. */
#include "re15_ultimate15.h"
#include "re15_rdt.h"
#include "asset_root_pc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Disc-Dateien: einmal gelesen, Lebensdauer = Programm (die Weltmodelle aliasen die RDT-Bytes). */
typedef struct { const char *rel; uint8_t *buf; int n; int versucht; } disc_datei_t;
static disc_datei_t s_dateien[] = {
    { RE15_U15_DISC_DIR "/DATA/ITEMALL.PIX",   NULL, 0, 0 },
    { RE15_U15_DISC_DIR "/DATA/MIXITEM.PIX",   NULL, 0, 0 },
    { RE15_U15_DISC_DIR "/ITEM/ITPS.ITP",      NULL, 0, 0 },
    { RE15_U15_DISC_DIR "/STAGE1/ROOM1020.RDT", NULL, 0, 0 },
    { RE15_U15_DISC_DIR "/STAGE1/ROOM11B0.RDT", NULL, 0, 0 },
    { RE15_U15_DISC_DIR "/STAGE4/ROOM4010.RDT", NULL, 0, 0 },
};

const uint8_t *re15_ultimate15_pc_disc(const char *rel, int *n)
{
    for (size_t i = 0; i < sizeof s_dateien / sizeof s_dateien[0]; i++) {
        disc_datei_t *d = &s_dateien[i];
        if (strcmp(d->rel, rel) != 0) continue;
        if (!d->versucht) {
            d->versucht = 1;
            d->buf = re15_pc_read_cd(d->rel, &d->n);
            fprintf(stderr, "[ultimate15] %s %s (%d B)\n", d->rel, d->buf ? "gelesen" : "FEHLT", d->buf ? d->n : 0);
        }
        if (n) *n = d->n;
        return d->buf;
    }
    if (n) *n = 0;
    return NULL;
}

/* Puffer bauen (malloc, Lebensdauer = Programm). NULL, wenn die Disc-Datei fehlt — dann nimmt der Leser den
 * heutigen Puffer auch bei AN. */
static uint8_t *bauen(const char *rel, const uint8_t *basis, int n,
                      int (*fn)(uint8_t *, const uint8_t *, int, const uint8_t *, int), const char *was)
{
    int dn = 0;
    const uint8_t *disc = re15_ultimate15_pc_disc(rel, &dn);
    if (!basis || n <= 0 || !disc) return NULL;
    uint8_t *dst = (uint8_t *)malloc((size_t)n);
    if (!dst) return NULL;
    int k = fn(dst, basis, n, disc, dn);
    if (k < 0) { free(dst); return NULL; }
    fprintf(stderr, "[ultimate15] %s: %d Disc-Teile eingesetzt\n", was, k);
    return dst;
}

uint8_t *re15_ultimate15_pc_itemall(const uint8_t *basis, int n)
{
    return bauen(RE15_U15_DISC_DIR "/DATA/ITEMALL.PIX", basis, n, re15_ultimate15_itemall_bauen, "ITEMALL-Icons");
}
uint8_t *re15_ultimate15_pc_mixitem(const uint8_t *basis, int n)
{
    return bauen(RE15_U15_DISC_DIR "/DATA/MIXITEM.PIX", basis, n, re15_ultimate15_mixitem_bauen, "MIXITEM-Icons");
}
uint8_t *re15_ultimate15_pc_itps(const uint8_t *basis, int n)
{
    return bauen(RE15_U15_DISC_DIR "/ITEM/ITPS.ITP", basis, n, re15_ultimate15_itps_bauen, "ITPS-Bilder");
}

/* Disc-Weltmodell eines Gegenstands: MD1 + TIM aus der Disc-RDT (Tabelle RDT+0x30, re15_rdt_parse). 1 = gefunden. */
int re15_ultimate15_pc_modell(unsigned item, const uint8_t **md1, int *md1n, const uint8_t **tim, int *timn)
{
    const char *rel = NULL; int prop = -1, n = 0;
    if (!re15_ultimate15_modell_quelle(item, &rel, &prop)) return 0;
    const uint8_t *buf = re15_ultimate15_pc_disc(rel, &n);
    if (!buf) return 0;
    static re15_rdt_t rdt;   /* nur zum Zerschneiden; Zeiger aliasen den dauerhaften Puffer */
    memset(&rdt, 0, sizeof rdt);
    if (re15_rdt_parse(buf, (size_t)n, &rdt) != 0 || prop >= rdt.prop_count) return 0;
    if (!rdt.prop_md1[prop] || !rdt.prop_tim[prop]) return 0;
    *md1 = rdt.prop_md1[prop]; *md1n = rdt.prop_md1_size[prop];
    *tim = rdt.prop_tim[prop]; *timn = rdt.prop_tim_size[prop];
    return 1;
}
