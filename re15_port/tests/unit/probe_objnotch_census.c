/* probe_objnotch_census.c — MESSUNG (kein Fix): weichen die beiden Record-Auswahlen fuer den
 * OBJEKT-Notch irgendwo im ausgelieferten Spiel voneinander ab?
 *
 * Hintergrund: `re15_object_notch_update` waehlte die Records frueher ueber den Typ
 *   ALT:      a->type == RE15_AOT_TYPE_EXAMINE_WORKVAR      (sce == 5)
 * Das Original waehlt sie in FUN_80042bac ausschliesslich ueber rec[1]:
 *   @0x80042c8c  and  v0,v0,a3    ; Pool-Maske; der Objekt-Pass uebergibt a1 = 4
 *   @0x80042ca4  bne  v0,s6,...   ; rec[1] & 0x10 muss == a2 sein; alle Pool-Aufrufe: a2 = 0
 *   @0x80042ea0  lbu  v1,1(s0)    ; 0x40 CENTRE / 0x20 FORWARD — ohne beide stempelt es nie
 *   ORIGINAL: (sce_flags & 0x04) && !(sce_flags & 0x10) && (sce_flags & 0x60)
 * ⚠ Der AUTO-Pfad hat KEINEN sce-Filter (den hat nur ACTION, @0x80042f48) — der Typ spielt
 *   im Original also gar keine Rolle.
 *
 * Fuer ROOM1230 (Kombinationsschloss) fallen beide zusammen: die Zellen tragen gemessen
 * sce=0x44 = CENTRE|Objekt-Pool. Diese Sonde prueft, ob das GAME-WEIT so ist, oder ob es Raeume
 * gibt, in denen der Port Records greift, die das Original nicht greift (oder umgekehrt).
 *
 * Ausgabe: je Raum nur die ABWEICHUNGEN, am Ende eine Bilanz.
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

static int g_raeume = 0, g_nur_port = 0, g_nur_orig = 0, g_beide = 0, g_abw_raeume = 0;

static void pruefe_raum(const char *base, int stage, const char *room)
{
    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/STAGE%d/%s.RDT", base, stage, room);
    uint8_t *data = read_file(path, &size);
    if (!data) return;

    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { free(data); return; }

    re15_actor_init();
    scd_vm_init();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 0; pl->y = 0; pl->z = 0; pl->rot_y = 0;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 4; f++) {
        scd_vm_tick();
        re15_aot_scan(pl->x, pl->z, 0);
        if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
    }
    g_raeume++;

    int kopf = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active) continue;
        int port = (a->type == RE15_AOT_TYPE_EXAMINE_WORKVAR);   /* die ALTE Abkuerzung */
        int orig = ((a->sce_flags & 0x04) != 0)
                && ((a->sce_flags & 0x10) == 0)
                && ((a->sce_flags & 0x60) != 0);
        if (port && orig) { g_beide++; continue; }
        if (!port && !orig) continue;
        if (!kopf) { printf("== STAGE%d/%s ==\n", stage, room); kopf = 1; g_abw_raeume++; }
        if (port) { g_nur_port++;
            printf("   Slot %2d: NUR TYP    t=%u sce=0x%02X band=0x%02X\n", i, a->type, a->sce_flags, a->band); }
        else      { g_nur_orig++;
            printf("   Slot %2d: NUR rec[1] t=%u sce=0x%02X band=0x%02X\n", i, a->type, a->sce_flags, a->band); }
    }
    free(data);
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    printf("=== Objekt-Notch: TYP-Abkuerzung (type==sce5) vs BYTE-TRUE rec[1] ===\n");
    printf("    rec[1] = (sce_flags & 0x04) && !(sce_flags & 0x10) && (sce_flags & 0x60)\n");
    printf("    HISTORISCHER BELEG: so wurde die Abweichung gemessen, die zur Umstellung von\n");
    printf("    re15_object_notch_update auf die rec[1]-Auswahl gefuehrt hat. Der Port NUTZT\n");
    printf("    seit 2026-08-08 die rec[1]-Spalte; die TYP-Spalte ist nur noch der Vergleich.\n\n");

    /* Alle Raeume aller Stages durchgehen. Raumnummern sind stage-basiert:
     * STAGE n -> ROOM<n><xx>0/1. Wir probieren das volle Raster und ueberspringen Fehlschlaege. */
    char room[32];
    for (int stage = 1; stage <= 6; stage++) {
        for (int r = 0; r <= 0x3F; r++) {
            for (int v = 0; v <= 1; v++) {
                snprintf(room, sizeof room, "ROOM%d%02X%d", stage, r, v);
                pruefe_raum(base, stage, room);
            }
        }
    }

    printf("\n=== BILANZ ===\n");
    printf("  Raeume geladen              : %d\n", g_raeume);
    printf("  Raeume mit Abweichung       : %d\n", g_abw_raeume);
    printf("  Records: beide Auswahlen    : %d\n", g_beide);
    printf("  NUR TYP-Abkuerzung          : %d  (haette der Port zu viel gestempelt)\n", g_nur_port);
    printf("  NUR rec[1]                  : %d  (haette der Port verpasst)\n", g_nur_orig);
    if (g_nur_port == 0 && g_nur_orig == 0)
        printf("  => Die TYP-Abkuerzung waere game-weit deckungsgleich gewesen.\n");
    else
        printf("  => Die TYP-Abkuerzung wich game-weit ab — deshalb entscheidet jetzt rec[1]\n"
               "     (aot_obj_record_ok in aot_common.c). ROOM1230 war nie betroffen (sce=0x44).\n");
    return 0;
}
