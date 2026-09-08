/* probe_nav_wirkung.c - Wie oft liefert der Navigator etwas ANDERES als die Spielerposition?
 *
 * Der Navigator (re15_nav_update_steer, RE1.5-Zwilling von RE2s FUN_8004A808) gibt bei
 * GLEICHER Zone die rohe Zielposition zurueck und bei ZONENFREMDEM Ziel die
 * First-Hop-Kreuzung um die Kulisse. Nur im zweiten Fall aendert der Anschluss etwas.
 *
 * ⛔ ANLASS: beim Kraehen-Sichtstrahl hatte ich einen "Fix" gezeigt, dessen Wirkung in der
 * Sonde gar nicht auftrat. Vor jeder Behauptung also erst messen, wie gross der betroffene
 * Anteil ueberhaupt ist.
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
    snprintf(path, sizeof path, "%s/STAGE1/%s.RDT", RE15_XSTR(RE15_ASSETS_PATH), name);
    size_t size = 0;
    uint8_t *data = read_file(path, &size);
    if (!data) { printf("  %-9s nicht lesbar\n", name); return; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { printf("  %-9s parse\n", name); free(data); return; }
    memcpy((void *)&g_room_rdt, &rdt, sizeof(g_room_rdt));
    g_room_rdt_ok = 1;
    re15_collision_set_band(0);
    re15_actor_init();
    re15_actor_t *e = &g_actors[1];
    memset(e, 0, sizeof(*e));
    e->active = 1; e->type = 0x20; e->hp = 100;

    int zonen = re15_nav_zone_count();
    int32_t X0 = 32000, X1 = -32000, Z0 = 32000, Z1 = -32000;
    for (int i = 0; i < rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &rdt.sca[i];
        if ((int32_t)c->x < X0) X0 = c->x;
        if ((int32_t)c->x + c->width  > X1) X1 = (int32_t)c->x + c->width;
        if ((int32_t)c->z < Z0) Z0 = c->z;
        if ((int32_t)c->z + c->density > Z1) Z1 = (int32_t)c->z + c->density;
    }
    long paare = 0, anders = 0; long summe = 0;
    for (int32_t ex = X0; ex <= X1; ex += 1200)
    for (int32_t ez = Z0; ez <= Z1; ez += 1200) {
        e->x = ex; e->z = ez; e->y = 0; e->state = 1;
        for (int32_t tx = X0; tx <= X1; tx += 1200)
        for (int32_t tz = Z0; tz <= Z1; tz += 1200) {
            int32_t dx = tx - ex, dz = tz - ez;
            if ((long)dx*dx + (long)dz*dz > 9000L*9000L) continue;
            e->repath_timer = 0;                    /* den DFS erzwingen (0-Tick) */
            re15_nav_update_steer(e, (int16_t)tx, (int16_t)tz, 0, 0);
            paare++;
            int32_t sx = e->steer_x, sz = e->steer_z;
            if (sx != (int16_t)tx || sz != (int16_t)tz) {
                anders++;
                int32_t ax = sx - (int16_t)tx, az = sz - (int16_t)tz;
                long d = (long)((ax<0?-ax:ax) + (az<0?-az:az));
                summe += d;
            }
        }
    }
    printf("  %-9s %2d Zonen | %6ld Paare | Umleitung bei %6ld (%.1f %%) | mittlere Abweichung %ld\n",
           name, zonen, paare, anders, paare ? 100.0*anders/paare : 0.0,
           anders ? summe/anders : 0);
    g_room_rdt_ok = 0; re15_collision_reset_band();
    free(data);
}

int main(void)
{
    printf("Navigator-Wirkung: wie oft ist das Steuerziel NICHT die rohe Zielposition?\n");
    const char *r[] = { "ROOM1190", "ROOM1230", "ROOM10C0", "ROOM1120", "ROOM1140",
                        "ROOM1130", "ROOM10E0", "ROOM1110" };
    for (unsigned i = 0; i < sizeof r / sizeof r[0]; i++) raum(r[i]);
    return 0;
}
