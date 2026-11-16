/* ultimate15_pc.c — Runde 41 Optionen: PC-Seite der Option "ULTIMATE 1.5" (re15_ultimate15.h).
 * Liest die unveraenderten Disc-Kopien unter shared_assets/PSX/DISC40/ ueber die gemeinsame CD-Wurzel
 * (asset_root_pc.h), baut die ULTIMATE-Puffer und liefert die Disc-Weltmodelle. Fehlt eine Disc-Datei, bleibt
 * alles beim heutigen Stand (Rueckgabe NULL / 0) und eine Zeile geht nach stderr. */
#include "re15_ultimate15.h"
#include "re15_rdt.h"
#include "re15_room.h"         /* g_room_rdt / g_room_rdt_ok / g_current_room_id */
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_objabe_r38.h"   /* re15_objabe_r38_pc_textur — STP-Teile wie beim RDT-Prop (Runde 38 Spur W) */
#include "re15_inhalt_r35.h"   /* Schrot 1090/1091: Port-Prop mit festem Gegenstand 0x16 */
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

/* WELT (include/re15_ultimate15.h): die Props des residenten Raums, an denen ein Item_aot_set einen Gegenstand mit
 * Disc-Weltmodell zeigt (re15_ultimate15_prop_items ueber main- und sub-SCD), bekommen MD1 + TIM aus der Disc-RDT.
 * Dazu das eine Port-Prop mit festem Gegenstand derselben Art: Schrot ROOM1090/1091 (re15_inhalt_r35.h, Item
 * RE15_R35I_SCHROT_ITEM auf obj RE15_R35I_SCHROT_OBJ). Lage, Drehung, Box, Aufheben und Verschwinden bleiben
 * unberuehrt — getauscht werden nur Gitter und Textur im selben Slot (slot[op] = RE15_TIM_SLOT_PROP(op), main.c).
 * Aufruf am Ende von main.c pc_load_room_prop_set. AUS -> sofort zurueck, nichts angefasst (bitgleich). */
extern const unsigned char *re15_room_pc_bytes(int *size);
extern void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);

void re15_ultimate15_pc_props(re15_md1_t *md1, int *ok, int nprops, const int *slot)
{
    if (!re15_ultimate15()) return;
    uint8_t item[RE15_RDT_MAX_PROPS];
    int n = 0;
    const unsigned char *raum = re15_room_pc_bytes(&n);
    if (raum && g_room_rdt_ok && g_room_rdt.main_scd >= raum && g_room_rdt.main_scd < raum + n)
        re15_ultimate15_prop_items(raum, n, item, RE15_RDT_MAX_PROPS);
    else
        memset(item, 0xFF, sizeof item);
    for (int op = nprops; op < RE15_RDT_MAX_PROPS; op++) item[op] = 0xFF;   /* nur RDT-Props aus dem Zensus */
    if ((g_current_room_id == 0x1090 || g_current_room_id == 0x1091) &&
        re15_inhalt_r35_obj_id((uint16_t)g_current_room_id) == RE15_R35I_SCHROT_OBJ &&
        RE15_R35I_SCHROT_OBJ < RE15_RDT_MAX_PROPS && ok[RE15_R35I_SCHROT_OBJ])
        item[RE15_R35I_SCHROT_OBJ] = RE15_R35I_SCHROT_ITEM;
    for (int op = 0; op < RE15_RDT_MAX_PROPS; op++) {
        const uint8_t *mb = NULL, *tb = NULL; int msz = 0, tsz = 0;
        if (item[op] == 0xFF || !re15_ultimate15_pc_modell(item[op], &mb, &msz, &tb, &tsz)) continue;
        re15_md1_t neu; re15_tim_t tt;
        if (re15_md1_parse(mb, msz, &neu) != 0 || re15_tim_parse(tb, tsz, &tt) != 0) {
            fprintf(stderr, "[ultimate15] Raum %04X obj %d: Disc-Modell Item 0x%02X nicht lesbar - bleibt\n",
                    (unsigned)g_current_room_id, op, item[op]);
            continue;
        }
        md1[op] = neu; ok[op] = 1;
        re15_render_pc_upload_tim_slot(&tt, slot[op]);
        re15_objabe_r38_pc_textur(&tt, slot[op]);
        fprintf(stderr, "[ultimate15] Raum %04X obj %d: Item 0x%02X -> Disc-Weltmodell (MD1 %d B, TIM %d B)\n",
                (unsigned)g_current_room_id, op, item[op], msz, tsz);
    }
}
