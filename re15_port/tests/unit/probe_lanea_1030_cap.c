/* probe_lanea_1030_cap.c — MESSUNG (kein Fix). LANE A zum Nutzer-Report 2026-08-06 ROOM1030:
 *   "Im ORIGINAL sind nur SECHS Zombies sichtbar. Bei uns sind es Dutzende."
 *
 * Diese Sonde misst NUR. Sie beantwortet drei Fragen am LIVE-Port (Raum wirklich geladen,
 * main00/sub00 wirklich gelaufen, AOT-Scan wirklich getickt):
 *
 *  A) Wieviele Aktoren instanziiert der Port aus ROOM1030s 20 Sce_em_set-Records?
 *  B) Was steht nach dem Raum-Init in g_scd.work_vars[0x11] (= PSX DAT_800b0ff2, der
 *     LEBEND-ZAEHLER) und work_vars[0x12] (= PSX DAT_800b0ff4, das MAXIMUM)?
 *       work_vars-Basis = DAT_800b0fd0 (Save-Handler @0x80040018: `sh a1, 0x800b0fd0+idx*2`)
 *       -> 0x800b0fd0 + 0x11*2 = 0x800b0ff2   (Zaehler, gelesen @0x80042214)
 *       -> 0x800b0fd0 + 0x12*2 = 0x800b0ff4   (Maximum, gelesen @0x8004221c)
 *     ROOM1030 main00 @Datei 0x1de2 fuehrt `24 12 06 00` = Save(0x12, 6) aus, DIREKT vor
 *     den 20 Sce_em_set-Records. Das Original lehnt damit Record 7..20 ab
 *     (@0x80042228 `bne`, @0x80042230 `ori v0,0x8000`, @0x80042238 `sw v0,0(s0)`).
 *  C) Setzt der Port entity+0x0B (Member-ID 0x0F) beim AOT-Scan? Das Original schreibt
 *     dort den Index der AOT-Zone, in der der Gegner steht (FUN_80042bac @0x80042fc4
 *     `sb v0,11(s1)` mit v0 = slot-1); FUN_800436a8 @0x8004371c loescht ihn jeden Frame.
 *     ROOM1030s Skript (sub03/04/06/09) pollt genau dieses Byte per Member_cmp(0x0F,...).
 *
 * Referenz-Simulation: dieselben 20 Records werden offline mit der byte-true Regel
 * `if (count >= max) reject; else count++` durchgerechnet -> Soll-Zahl des Originals.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern scd_vm_t g_scd;
extern re15_aot_state_t g_aot;

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

static int live_actors(void)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active) n++;
    return n;
}

/* Offline-Referenz: die byte-true Sce_em_set-Limit-Regel auf die Rohdaten anwenden. */
static void simulate_original(const uint8_t *d, size_t sz, uint32_t em_off, int n_rec, int max)
{
    (void)sz;
    int count = 0, spawned = 0, rejected = 0;
    printf("   Simulation (max=%d aus Save(0x12,..)):\n", max);
    for (int i = 0; i < n_rec; i++) {
        const uint8_t *r = d + em_off + (size_t)i * 20;
        int ok = (count < max);            /* @0x80042224 slt count,max */
        if (ok) { count++; spawned++; }    /* @0x8004223c sh count+1 */
        else    { rejected++; }            /* @0x80042230-38 entity+0x0 = 0x8000 */
        if (i < 8 || !ok)
            printf("     rec %2d slot=%2u -> %s (count danach %d)\n",
                   i, r[1], ok ? "SPAWN" : "REJECT(entity+0x0=0x8000)", count);
        if (!ok && i == 6) printf("     ... (alle weiteren ebenfalls REJECT)\n");
        if (!ok && i > 6) { rejected = n_rec - spawned; break; }
    }
    printf("   => ORIGINAL: %d gespawnt, %d abgelehnt\n\n", spawned, n_rec - spawned);
}

static void room_probe(const char *base, const char *room, uint32_t em_off, int n_rec)
{
    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/STAGE1/%s.RDT", base, room);
    uint8_t *data = read_file(path, &size);
    if (!data) { printf("FAIL: %s nicht lesbar\n", path); return; }

    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { printf("FAIL: parse %s\n", room); free(data); return; }

    re15_actor_init();
    scd_vm_init();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -25000; pl->y = 0; pl->z = -20000; pl->rot_y = 2048;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 8; f++) {
        scd_vm_tick();
        re15_aot_scan(pl->x, pl->z, 0);
        if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
    }

    printf("== %s ==\n", room);
    printf("   PORT: aktive Gegner-Aktoren = %d   (RE15_ACTOR_MAX=%d, SCRIPT_SLOT_TO_ACTOR=slot+1)\n",
           live_actors(), RE15_ACTOR_MAX);
    printf("   PORT: work_vars[0x11] (PSX 0x800b0ff2 Zaehler) = %d\n", (int)g_scd.work_vars[0x11]);
    printf("   PORT: work_vars[0x12] (PSX 0x800b0ff4 Maximum) = %d\n", (int)g_scd.work_vars[0x12]);
    if (n_rec > 0)
        simulate_original(data, size, em_off, n_rec, (int)g_scd.work_vars[0x12]);

    /* C: member_0b (entity+0x0B) nach dem AOT-Scan.
     * WICHTIG (Lehre): die Spawn-Positionen der ROOM1030-Zombies (z=-28000) liegen AUSSERHALB
     * aller drei Zonen — ein member_0b==0 waere dort NICHT aussagekraeftig. Deshalb werden zwei
     * Aktoren GEZIELT in die Zonen-Mitten von AOT-Slot 6 bzw. 5 gesetzt und erst dann gescannt. */
    if (g_actors[1].active && g_actors[2].active && g_aot.slots[6].active && g_aot.slots[5].active) {
        g_actors[1].x = g_aot.slots[6].x; g_actors[1].z = g_aot.slots[6].z;   /* Zonen-Mitte AOT 6 */
        g_actors[2].x = g_aot.slots[5].x; g_actors[2].z = g_aot.slots[5].z;   /* Zonen-Mitte AOT 5 */
        uint8_t max1 = 0, max2 = 0;
        for (int f = 0; f < 16; f++) {
            scd_vm_tick();
            re15_aot_scan(pl->x, pl->z, 0);
            if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
            if (g_actors[1].member_0b > max1) max1 = g_actors[1].member_0b;
            if (g_actors[2].member_0b > max2) max2 = g_actors[2].member_0b;
        }
        /* ⚠ Slot 6 wird mit sce=0x00 INSTALLIERT (das Rechteck stimmt: x[-27300,3300]
         * z[-24500,-700]). Er stempelt erst, nachdem sub02 ihn per
         * Aot_reset(6, sce=5, flags=0x42) einschaltet — und sub02 haengt an flag(5,0x14), das
         * ohne den Story-Zustand nie gesetzt wird. AM ORIGINAL GEMESSEN (PCSX-Redux,
         * analysis/room1030_crawl_mechanism.md §11): dort trat ebenfalls NUR 5 auf, NIE 6.
         * In diesem Zustand ist 0 also der RICHTIGE Wert, nicht 6. */
        printf("   PORT: Aktor 1 MITTEN in AOT-Slot 6 -> member_0b max ueber 16 Frames = %u  (ORIGINAL in diesem Zustand: 0 — Zone 6 ist aus)\n", max1);
        printf("   PORT: Aktor 2 MITTEN in AOT-Slot 5 -> member_0b max ueber 16 Frames = %u  (ORIGINAL: 5 — am Geraet gemessen)\n", max2);
    }
    printf("   PORT: member_0b je Aktor:");
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active) printf(" [%d]=%u", s, g_actors[s].member_0b);
    printf("\n");

    /* AOT-Tabelle: Slot-Index -> Rect (fuer die +0x0B-Zonen 4/5/6 von ROOM1030). */
    printf("   AOT-Slots (idx: typ/sce_flags/band rect x[+-w] z[+-d]):\n");
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active) continue;
        printf("     %2d: t=%u sce=0x%02X band=0x%02X  x=%ld+-%ld z=%ld+-%ld\n",
               i, a->type, a->sce_flags, a->band,
               (long)a->x, (long)a->half_w, (long)a->z, (long)a->half_h);
    }
    printf("\n");
    free(data);
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    /* ROOM1030: 20 Records ab Datei-Offset 0x1de6; ROOM1140 als Kontrollraum ohne Save(0x12,..). */
    room_probe(base, "ROOM1030", 0x1de6, 20);
    room_probe(base, "ROOM1140", 0, 0);
    return 0;
}
