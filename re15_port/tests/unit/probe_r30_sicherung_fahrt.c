/* probe_r30_sicherung_fahrt.c — MESS-SONDE (Runde 30, Thema H, Fortsetzungs-Agent).
 *
 * ⛔ KEIN SPIELCODE, KEIN TEST DER SUITE. Wird nur mit -DRE15_R30_SICHERUNG_VARIANTE=ON
 * gebaut (probes/r30_sicherung.cmake) und prueft NICHTS — sie druckt Messzeilen.
 *
 * WOZU: Das Dossier (analysis/befunde_runde30/sicherung.md) braucht den ZEITPLAN der
 * Hebetisch-Szene sub04 (ROOM1150/1151 @0x0F96-0x10B6) aus der laufenden Engine:
 *   - wie weit oeffnen die Deckelhaelften (obj 1/2) WIRKLICH?  Der Vorgaenger rechnete mit
 *     240 ("For 24"), der For-Record @0x0FC0 ist aber `0d 00 18 00 0f 00` = Blocklaenge
 *     0x18, Zaehler 0x0F (For-Handler @0x8003f564 `lh t1,2(t0)` = Laenge,
 *     @0x8003f568 `lhu a1,4(t0)` = Zaehler).
 *   - in welchem Bild nach dem Ausloesen ist der Deckel offen, die Plattform an der
 *     Modal-Schranke -1100, am Hochpunkt, wieder unten, wieder geparkt?
 *   - gilt das in BEIDEN Raumvarianten (ROOM1150 und ROOM1151)?
 *
 * Das Modal wird hier absichtlich NICHT ausgeloest (re15_sicherung_tick wird nicht
 * gerufen) — gemessen wird die ungestoerte Szene.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_sicherung.h"
#include "re15_item_modal.h"

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

static int slot_von_obj(uint8_t obj_id)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].obj_id == obj_id) return k;
    return -1;
}

static int raum(uint16_t rid, const char *datei)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/%s", base, datei);
    size_t sz = 0;
    uint8_t *buf = read_file(path, &sz);
    if (!buf) { printf("RDT nicht lesbar: %s\n", path); return 77; }
    static re15_rdt_t rdt;
    memset(&rdt, 0, sizeof rdt);
    if (re15_rdt_parse(buf, sz, &rdt) != 0) { printf("parse fail %s\n", datei); return 1; }

    re15_game_state_init();
    scd_vm_init(); re15_actor_init();
    g_current_room_id = rid;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -21000; pl->y = 0; pl->z = -18500; pl->rot_y = 2048; pl->state = 1;
    g_scd.player_mode = 0;
    { extern void re15_msg_load_room_block(const uint8_t *b, int n);
      re15_msg_load_room_block(rdt.messages, rdt.messages_size); }
    scd_register_room_events(&rdt);
    scd_room_reenter(&rdt, -21000, -18500, 0);

    printf("\n==== %s (%u B), nOmodel=%d ====\n", datei, (unsigned)sz, (int)rdt.prop_count);
    printf("MESS prop_count=%d\n", (int)g_scd.prop_count);
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        printf("MESS pool[%d] obj_id=%d type=%d parent=%d active=%d flags=0x%04X "
               "pos=(%ld,%ld,%ld) rot=(%d,%d,%d)\n",
               k, g_scd.props[k].obj_id, g_scd.props[k].obj_type,
               (int)g_scd.props[k].parent_obj, g_scd.props[k].active,
               (unsigned)g_scd.props[k].flags,
               (long)g_scd.props[k].x, (long)g_scd.props[k].y, (long)g_scd.props[k].z,
               g_scd.props[k].rot_x, g_scd.props[k].rot_y, g_scd.props[k].rot_z);

    int p0 = slot_von_obj(0), p1 = slot_von_obj(1), p2 = slot_von_obj(2);
    if (p0 < 0 || p1 < 0 || p2 < 0) { printf("Props 0/1/2 fehlen\n"); free(buf); return 2; }

    scd_event_fire(4);
    long ly = g_scd.props[p0].y, l1 = g_scd.props[p1].z, l2 = g_scd.props[p2].z;
    int cut_alt = (int)g_scd.cam_id;
    printf("MESS Bild  -1: Plattform y=%ld  Deckel1 z=%ld  Deckel2 z=%ld  cut=%d\n",
           ly, l1, l2, cut_alt);
    int schranke_bild = -1, hoch_bild = -1;
    long y_min = 0x7fffffff;
    for (int f = 0; f < 520; f++) {
        scd_vm_tick();
        long py = g_scd.props[p0].y, z1 = g_scd.props[p1].z, z2 = g_scd.props[p2].z;
        int cut = (int)g_scd.cam_id;
        int bewegt_alt = (ly != py) || (l1 != z1) || (l2 != z2);
        /* nur die WENDEPUNKTE drucken: Beginn/Ende jeder Bewegung und Cut-Wechsel */
        static int lief = 0;
        if (f == 0) lief = 0;
        if (bewegt_alt != lief || cut != cut_alt) {
            printf("MESS Bild %3d: Plattform y=%6ld  Deckel1 z=%5ld  Deckel2 z=%5ld  cut=%d  %s\n",
                   f, py, z1, z2, cut, bewegt_alt ? "BEWEGT SICH" : "steht");
            lief = bewegt_alt;
        }
        if (py > -5000 && py <= -1100 && schranke_bild < 0) {
            schranke_bild = f;
            printf("MESS Bild %3d: Plattform y=%6ld  <= -1100 = Modal-Schranke (OBEN_BIS) "
                   "Deckel1 z=%ld Deckel2 z=%ld\n", f, py, z1, z2);
        }
        if (py > -5000 && py < y_min) { y_min = py; hoch_bild = f; }
        ly = py; l1 = z1; l2 = z2; cut_alt = cut;
    }
    printf("MESS Hochpunkt y=%ld zuerst in Bild %d; Modal-Schranke in Bild %d\n",
           y_min, hoch_bild, schranke_bild);
    free(buf);
    return 0;
}

int main(void)
{
    int a = raum(0x1150, "ROOM1150.RDT");
    int b = raum(0x1151, "ROOM1151.RDT");
    return a ? a : b;
}
