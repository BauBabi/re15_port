/* probe_stair_hwtruth_1060.c — Port-Treppenstrecke gegen die HARDWARE-Wahrheit.
 *
 * GROUND TRUTH (DuckStation, ORIGINAL-EXE, vom Nutzer gemessen 2026-08-06; die MZD-Disc traegt
 * eine EXE, die byte-identisch zu info/Re1.5/PSX.EXE ist, und der gesamte Treppen-/Kamera-Pfad ist
 * im Image unveraendert — beides selbst verifiziert). Zehn Savestates ueber den kompletten
 * ROOM1060-Abstieg, Spieler jeweils IN RUHE am Absatz:
 *
 *   Treppe 8->6 (Ost) : z 24568 -> 19532   dz = -5036   y -14400 -> -10800
 *   Treppe 6->4 (West): z 20332 -> 25368   dz = +5036   y -10800 ->  -7200
 *   Treppe 4->2 (Ost) : z 24568 -> 19532   dz = -5036   y  -7200 ->  -3600
 *   Treppe 2->0 (West): z 20332 -> 25368   dz = +5036   y  -3600 ->      0
 *
 * Also VIERMAL identisch 5036 Einheiten, in beide Richtungen. Das ist der Zielwert.
 *
 * BESTAETIGT DAMIT die geometrische Herleitung: Schachtzelle 4100 + 2x(Body-Reach 450 + Skin 18)
 * = 5036, und die vorhergesagten Start-/Landepunkte 24568/19532 (Ost) bzw. 20332/25368 (West)
 * stimmen auf die Einheit mit der Hardware ueberein.
 *
 * KALIBRIERUNG (2026-08-06): Die Sonde brauchte zwingend das PL00-Skelett — die Treppenstrecke
 * entsteht groesstenteils aus dem FK-Fuss-Lock FUN_800390e0, der ohne Skelett nichts beitraegt.
 * Mit `re15_stair_tick(rdt, NULL, NULL)` mass sie nur die Vorwaerts-10 und lieferte 2380-2480.
 *
 * ERGEBNIS NACH DER KALIBRIERUNG: Der Port trifft alle vier Hardware-Strecken EXAKT (-5036,
 * +5036, -5036, +5036, Differenz 0). Gegenprobe mit temporaer zurueckgenommenem Cursor-Fix
 * (77db4118): ebenfalls exakt 5036 — die Strecke war also schon vorher richtig.
 *
 * DAMIT WIDERLEGT: die Annahme "der Port laeuft ~700-900 Einheiten zu kurz". Die Werte 3761 und
 * 4134/4355 stammten aus Messaufbauten OHNE Skelett bzw. aus umgeschriebenen Agenten-Sonden.
 * Die zwei noch offenen byte-true Divergenzen (Fussreferenz pro Fuss @0x80039148-58, unbedingter
 * Zug @0x80038d50) verursachen NACHWEISLICH KEINEN Streckenfehler — sie einzubauen wuerde eine
 * Strecke gefaehrden, die aktuell exakt stimmt.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_stair.h"
#include "re15_collision.h"

#include "re15_emd.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

/* Das PL00-Skelett ist fuer die Messung ZWINGEND: die Treppenstrecke entsteht zum grossen Teil
 * aus dem FK-Fuss-Lock (FUN_800390e0), der ohne Skelett gar nichts beitraegt — dann bleibt nur
 * die Vorwaerts-10 uebrig und die Sonde misst rund die Haelfte. */
static re15_emd_skeleton_t  g_skel;
static re15_emd_animation_t g_anim;
static int g_skel_ok = 0;

static uint8_t *read_file(const char *path, size_t *out)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    size_t rd = b ? fread(b, 1, (size_t)sz, f) : 0;
    fclose(f);
    if (!b || rd != (size_t)sz) { free(b); return NULL; }
    *out = (size_t)sz; return b;
}

/* Ein Abstieg: Spieler an den gemessenen Startpunkt, Treppe ausloesen, bis zum Stillstand ticken. */
static int run_one(const re15_rdt_t *rdt, const char *tag,
                   int32_t x0, int32_t z0, int32_t y0, uint16_t yaw, int band0,
                   int32_t z_soll)
{
    re15_actor_init();
    scd_vm_init();
    re15_aot_init();
    scd_register_room_events(rdt);
    scd_room_reenter(rdt, x0, z0, 0);

    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    p->active = 1; p->type = 0; p->hp = 100; p->state = 1;
    p->x = x0; p->y = y0; p->z = z0; p->rot_y = yaw;
    re15_collision_set_band((uint8_t)band0);
    g_scd.player_mode = 0;

    /* Aktionstaste-Flanke auf den Treppen-Scan (re15_stair_try_start = das Port-Gegenstueck zum
     * Action-Scan FUN_80042bac). */
    int started = re15_stair_try_start(rdt, 1) && re15_stair_active();
    if (!started) { printf("  %-18s KEIN START (yaw=%u band=%d)\n", tag, (unsigned)yaw, band0); return 1; }

    int32_t zs = p->z;
    for (int f = 0; f < 400 && re15_stair_active(); f++)
        re15_stair_tick(rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
    int32_t dz = p->z - zs;
    int32_t soll = z_soll - z0;
    printf("  %-18s Start z=%6d -> Ende z=%6d   dz=%+6d   SOLL %+6d   Differenz %+5d %s\n",
           tag, zs, p->z, dz, soll, dz - soll,
           (dz == soll) ? "  <== TREFFER" : "");
    return (dz == soll) ? 0 : 1;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1060.RDT", base);
    size_t sz = 0;
    uint8_t *buf = read_file(path, &sz);
    if (!buf) { fprintf(stderr, "ROOM1060.RDT nicht lesbar: %s\n", path); return 77; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(buf, sz, &rdt) != 0) { fprintf(stderr, "parse fail\n"); return 1; }
    g_current_room_id = 0x1060;

    {   /* PL00-Skelett + Animation laden — ohne die traegt der FK-Fuss-Lock nichts bei. */
        char p1[600], p2[600]; size_t s1 = 0, s2 = 0;
        snprintf(p1, sizeof p1, "%s/PLD/PL00.EDD", base);
        snprintf(p2, sizeof p2, "%s/PLD/PL00.EMR", base);
        uint8_t *edd = read_file(p1, &s1);
        uint8_t *emr = read_file(p2, &s2);
        if (edd && emr && re15_emd_parse_animation(edd, s1, &g_anim) == 0 &&
            re15_emd_parse_skeleton(emr, s2, &g_skel) == 0) g_skel_ok = 1;
        printf("PL00-Skelett: %s\n", g_skel_ok ? "geladen" : "FEHLT — Messung ungueltig!");
    }

    printf("=== Port-Treppenstrecke gegen die DuckStation-Messung (ORIGINAL-EXE) ===\n");
    printf("Zielwert aus 4 Hardware-Messungen: 5036 Einheiten pro Treppe.\n\n");

    int bad = 0;
    /* yaw: Ost-Treppen laufen nach -z (yaw 1024), West-Treppen nach +z (yaw 3072). */
    /* Erst die AOT-Lage zeigen, dann alle vier Kardinalrichtungen durchprobieren. */
    {
        re15_actor_init(); scd_vm_init(); re15_aot_init();
        scd_register_room_events(&rdt); scd_room_reenter(&rdt, 25178, 24568, 0);
        printf("STAIR-AOTs in ROOM1060:\n");
        for (int i = 0; i < 32; i++) {
            const re15_aot_t *a = &g_aot.slots[i];
            if (!a->active || a->type != RE15_AOT_TYPE_STAIR) continue;
            printf("   slot %2d  band=%3d sce_flags=0x%02x  z[%6ld..%6ld] x[%6ld..%6ld]\n",
                   i, a->band, a->sce_flags,
                   (long)(a->z - a->half_h), (long)(a->z + a->half_h),
                   (long)(a->x - a->half_w), (long)(a->x + a->half_w));
        }
        printf("   aktuelles Band nach reenter = %d\n\n", re15_collision_debug_band());
    }
    /* Blickrichtung FEST, nicht gesucht: die Ost-Treppen laufen bei yaw 0 abwaerts, die
     * West-Treppen bei yaw 2048 (Port-Konvention x+=cos, z-=sin). Ein Yaw-SWEEP waere
     * selbsterfuellend — er wuerde so lange probieren, bis eine Richtung zufaellig den
     * Sollwert trifft. */
    struct { const char *tag; int32_t x,z,y; uint16_t yaw; int band; int32_t soll; } RUNS[4] = {
        { "8->6 Ost",  25178, 24568, -14400,    0, 8, 19532 },
        { "6->4 West", 21585, 20332, -10800, 2048, 6, 25368 },
        { "4->2 Ost",  25283, 24568,  -7200,    0, 4, 19532 },
        { "2->0 West", 21588, 20332,  -3600, 2048, 2, 25368 },
    };
    for (int i = 0; i < 4; i++)
        bad += run_one(&rdt, RUNS[i].tag, RUNS[i].x, RUNS[i].z, RUNS[i].y,
                       RUNS[i].yaw, RUNS[i].band, RUNS[i].soll);
    if (!g_skel_ok) { printf("\nFAIL: ohne PL00-Skelett ist die Messung wertlos.\n"); bad++; }

    printf("\n%s\n", bad ? "ERGEBNIS: Port weicht von der Hardware ab."
                         : "ERGEBNIS: Port trifft die Hardware-Strecke exakt.");
    return bad ? 1 : 0;
}
