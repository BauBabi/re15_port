/* probe_re2_crow_1120.c - Nutzer-Report 2026-09-08 (RE2-Modus):
 *   "sie folgen Leon stumpf linear, wenn ein Tresen im Weg ist, fliegen sie stumpf linear
 *    Richtung Leon, auch wenn der Tresen zwischen beiden die Kraehe blockt. Die Kraehe
 *    muesste erkennen, dass sie lange geblockt wurde und dann etwas anderes probieren."
 *
 * Misst genau das: Spieler und Kraehe auf die zwei Seiten einer soliden Zelle setzen und
 * zaehlen, wie lange die Kraehe im Anflug (Sub 11/13) steht, ohne voranzukommen.
 *
 * Das Original hat den Ausweg ueber die SICHTLINIE: Sub 11/13 haben nur Kegel und
 * +0x22A&0x2 als geometrische Ausgaenge, der Wand-Prober laeuft nur in Sub 4/5/6. Im
 * Original bricht das blockierende Hindernis auch die Sicht (0x80050858 tastet dieselbe
 * Kulissen-Familie ab, die die Bewegung stoppt) -> Bit faellt -> Exit nach Sub 4 -> der
 * Wand-Prober laesst sie ausweichen. Der Port zog bis 2026-09-08 den RE1.5-Regionen-Ray,
 * der die SCA-Zellen NICHT kennt.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_player.h"
#include "re15_collision.h"
#include "re15_ai_flavor.h"
#include "re2_ems.h"

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

static void bank_laden(void)
{
    size_t esz = 0;
    uint8_t *ems = read_file(RE15_XSTR(RE15_ASSETS_PATH) "/../RE2/CDEMD0.EMS", &esz);
    if (!ems) { printf("  [Bank] CDEMD0.EMS fehlt\n"); return; }
    re15_enemy_bank_t *eb = re15_enemy_find(0x21);
    if (!eb) eb = re15_enemy_alloc(0x21);
    re15_tim_t tim = {0};
    if (eb && !eb->victim_ok && re2_ems_load_bank(ems, esz, 0x21, eb, &tim) == 0) eb->ok = 1;
}

static void lauf(const re15_rdt_t *rdtq, int zellstrahl)
{
    (void)rdtq;
    char path[600];
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1120.RDT", base);
    size_t size = 0;
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return; }

    /* Die Tresen-Zelle aus der Maskenarbeit: x1100..6650, z-3950..-2350 (ROOM1120 Cut 2).
     * Spieler auf die eine Seite, Kraehe auf die andere - der Tresen liegt dazwischen. */
    /* Ein Paar suchen, an dem die ZELLEN blockieren und der alte Regionen-Strahl NICHT -
     * nur dort kann der Fix ueberhaupt wirken. (Mein erster Versuch nahm ein Paar, an dem
     * beide blockieren; beide Staende lieferten dasselbe Ergebnis und bewiesen nichts.
     * probe_los_divergenz: in ROOM1120 sind 1394 von 7423 Paaren genau so.) */
    int32_t PX = 0, PZ = 0, CX = 0, CZ = 0;

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1120;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_re2_los_cells_enable(zellstrahl);
    bank_laden();

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = PX; pl->y = 0; pl->z = PZ;
    pl->hp = 100; pl->rot_y = 0; pl->hit_react = 0;
    memcpy((void *)&g_room_rdt, &rdt, sizeof(g_room_rdt));
    g_room_rdt_ok = 1;
    re15_collision_set_band(0);
    scd_register_room_events(&rdt);
    scd_room_reenter(&rdt, 0, 0, 0);

    {   /* Divergenz-Paar suchen */
        int32_t X0=32000,X1=-32000,Z0=32000,Z1=-32000;
        for (int i=0;i<rdt.sca_count;i++){ const re15_sca_entry_t *e=&rdt.sca[i];
            if ((int32_t)e->x<X0) X0=e->x; if ((int32_t)e->x+e->width>X1) X1=(int32_t)e->x+e->width;
            if ((int32_t)e->z<Z0) Z0=e->z; if ((int32_t)e->z+e->density>Z1) Z1=(int32_t)e->z+e->density; }
        re15_actor_t *cc=&g_actors[1];
        cc->active=1; cc->type=0x21; cc->hp=10; cc->sca_mask=4; cc->y=-1200;
        int gefunden=0;
        for (int32_t px=X0; px<=X1 && !gefunden; px+=800)
        for (int32_t pz=Z0; pz<=Z1 && !gefunden; pz+=800) {
            pl->x=px; pl->z=pz;
            for (int32_t cx=X0; cx<=X1 && !gefunden; cx+=800)
            for (int32_t cz=Z0; cz<=Z1 && !gefunden; cz+=800) {
                int32_t dx=cx-px, dz=cz-pz;
                long d2=(long)dx*dx+(long)dz*dz;
                if (d2 < 2000L*2000L || d2 > 4500L*4500L) continue;
                cc->x=cx; cc->z=cz;
                re15_re2_los_cells_enable(0);
                int a=!re15_re2_los_clear(cc,pl);
                re15_re2_los_cells_enable(1);
                int n=!re15_re2_los_clear(cc,pl);
                if (n && !a) { PX=px; PZ=pz; CX=cx; CZ=cz; gefunden=1; }
            }
        }
        re15_re2_los_cells_enable(zellstrahl);
        if (!gefunden) { printf("   kein Divergenz-Paar gefunden\n"); g_room_rdt_ok=0; free(data); return; }
    }

    /* Eine Kraehe von Hand setzen (ROOM1120 spawnt keine) und wecken. */
    re15_actor_t *c = &g_actors[1];
    memset(c, 0, sizeof(*c));
    c->active = 1; c->type = 0x21; c->hp = 10;
    c->x = CX; c->y = -1200; c->z = CZ;
    c->state = 1; c->sub_state_1 = 4;      /* wach, im Reiseflug - der Weck-Test (dist<=1800
                                            * @0x80100704) greift bei 4300 Abstand nicht */
    c->sca_mask = 4;
    c->crow_vol90 = 350;

    printf("\n=== Zell-Sichtstrahl %s ===\n", zellstrahl ? "AN (jetzt)" : "AUS (Stand vor dem Fix)");
    printf("ROOM1120: Spieler (%d,%d), Kraehe (%d,%d) - der Tresen x1100..6650 z-3950..-2350\n",
           (long)PX, (long)PZ, (long)CX, (long)CZ);
    printf("   Sichtlinie durch die Kollisionszellen blockiert: %d\n",
           re15_re2_los_cells_blocked(&g_room_rdt, c->x, c->z, pl->x, pl->z,
                                      re15_collision_band_from_y(c->y), 4u));

    long anflug = 0, still = 0, weg = 0;
    uint32_t subs = 0;
    int32_t lx = c->x, lz = c->z, ly = c->y;
    for (int f = 0; f < 3600; f++) {
        pl->hit_react = 0;
        scd_vm_tick(); re15_enemy_ai_run_all(1); re15_actors_anim_advance();
        if (!c->active) break;
        int32_t dx = c->x - lx, dz = c->z - lz, dy = c->y - ly;
        long s = (dx<0?-dx:dx) + (dz<0?-dz:dz) + (dy<0?-dy:dy);
        weg += s;
        lx = c->x; lz = c->z; ly = c->y;
        if (c->state == 1 && c->sub_state_1 < 32) subs |= (1u << c->sub_state_1);
        if (c->state == 1 && (c->sub_state_1 == 11 || c->sub_state_1 == 13)) {
            anflug++;
            if (s < 8) still++;
        }
    }
    printf("   nach 3600 Ticks: Weg=%ld  Sub-Maske=0x%05lx  Anflug-Ticks(11/13)=%ld, davon "
           "ohne Bewegung=%ld\n", weg, (unsigned long)subs, anflug, still);
    printf("   jetzt: st=%d ss=%d pos=(%ld,%ld,%ld)  LOS-Bit=%d\n",
           c->state, c->sub_state_1, (long)c->x, (long)c->y, (long)c->z,
           (c->re2c_flags22a & 2u) ? 1 : 0);
    g_room_rdt_ok = 0; re15_collision_reset_band();
    free(data);
}

int main(void)
{
    lauf(NULL, 0);        /* wie bisher: nur der RE1.5-Regionen-Strahl */
    lauf(NULL, 1);        /* neu: zusaetzlich die Kollisionszellen     */
    return 0;
}
