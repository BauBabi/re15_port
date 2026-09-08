/* probe_los_divergenz.c - Gibt es die Divergenz ueberhaupt?
 *
 * diag_crow_stuck.md Ursache B behauptet: der Port-Sichtstrahl (RE1.5-Boden-REGIONEN)
 * meldet "frei", wo die SCA-Zellen den Flug KLEMMEN - dadurch findet eine Kraehe in
 * Sub 11/13 keinen Ausweg. Das ist eine Behauptung ueber Geometrie, also messbar:
 * beide Strahlen ueber ein Gitter von Standort-Paaren laufen lassen und zaehlen, wie
 * oft sie sich widersprechen.
 *
 * ⛔ ANLASS: mein erster Fix-Nachweis (probe_re2_crow_1120) zeigte in BEIDEN Staenden
 * dasselbe Ergebnis - dort blockt der alte Strahl bereits. Ein Fix, dessen Wirkung ich
 * nicht zeigen kann, ist kein Fix.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_collision.h"
#include "re15_ai_flavor.h"

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

static void raum(const char *name)
{
    char path[600];
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    snprintf(path, sizeof path, "%s/STAGE1/%s.RDT", base, name);
    size_t size = 0;
    uint8_t *data = read_file(path, &size);
    if (!data) { printf("  %s: nicht lesbar\n", name); return; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { printf("  %s: parse\n", name); free(data); return; }

    memcpy((void *)&g_room_rdt, &rdt, sizeof(g_room_rdt));
    g_room_rdt_ok = 1;
    re15_collision_set_band(0);
    re15_actor_init();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *c  = &g_actors[1];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    memset(c, 0, sizeof(*c));
    c->active = 1; c->type = 0x21; c->hp = 10; c->sca_mask = 4; c->y = -1200;

    /* Ausdehnung des Raums aus den Zellen */
    int32_t X0 = 32000, X1 = -32000, Z0 = 32000, Z1 = -32000;
    for (int i = 0; i < rdt.sca_count; i++) {
        const re15_sca_entry_t *e = &rdt.sca[i];
        if ((int32_t)e->x < X0) X0 = e->x;
        if ((int32_t)e->x + e->width  > X1) X1 = (int32_t)e->x + e->width;
        if ((int32_t)e->z < Z0) Z0 = e->z;
        if ((int32_t)e->z + e->density > Z1) Z1 = (int32_t)e->z + e->density;
    }
    long paare = 0, nur_zellen = 0, nur_alt = 0, beide = 0;
    const int S = 1500;
    for (int32_t px = X0; px <= X1; px += S)
    for (int32_t pz = Z0; pz <= Z1; pz += S) {
        pl->x = px; pl->z = pz; pl->y = 0;
        for (int32_t cx = X0; cx <= X1; cx += S)
        for (int32_t cz = Z0; cz <= Z1; cz += S) {
            int32_t dx = cx - px, dz = cz - pz;
            if ((long)dx*dx + (long)dz*dz > 6000L*6000L) continue;   /* Kraehen-Reichweite */
            c->x = cx; c->z = cz;
            re15_re2_los_cells_enable(0);
            int alt = !re15_re2_los_clear(c, pl);
            re15_re2_los_cells_enable(1);
            int neu = !re15_re2_los_clear(c, pl);
            int zellen = re15_re2_los_cells_blocked(&g_room_rdt, cx, cz, px, pz,
                                                    re15_collision_band_from_y(c->y), 4u);
            (void)neu;
            paare++;
            if (zellen && !alt) nur_zellen++;
            else if (!zellen && alt) nur_alt++;
            else if (zellen && alt) beide++;
        }
    }
    printf("  %-9s %7ld Paare:  beide blockieren %6ld | NUR die Zellen %6ld (%.1f %%) | nur der alte %5ld\n",
           name, paare, beide, nur_zellen, paare ? 100.0*nur_zellen/paare : 0.0, nur_alt);
    g_room_rdt_ok = 0; re15_collision_reset_band();
    free(data);
}

int main(void)
{
    printf("Sichtlinien-Vergleich: RE1.5-Regionenstrahl gegen die Kollisionszellen\n");
    printf("(nur wo die ZELLEN blockieren und der alte Strahl NICHT, kann der Fix wirken)\n");
    const char *raeume[] = { "ROOM1120", "ROOM10C0", "ROOM1130", "ROOM10D0", "ROOM10E0",
                             "ROOM1140", "ROOM1110", "ROOM1170",
                             "ROOM1190", "ROOM1230" };   /* die Hunde-Raeume */
    for (unsigned i = 0; i < sizeof raeume / sizeof raeume[0]; i++) raum(raeume[i]);
    return 0;
}
