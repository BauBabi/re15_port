/* probe_r16_sk_objekte_paket.c — SKEPTIKER-Gegensonde Runde 16 (2026-09-19), Thema
 * "objekte-paket" (Dossier analysis/befunde_2026-09-19/objekte-paket.md).
 *
 * Prueft NICHT nur, ob die Dossier-Zahlen reproduzierbar sind, sondern isoliert die URSACHE:
 *
 *   S1  Kontrolle: Boot -> ROOM1050 frisch: obj 0/1 aktiv.
 *   S2  Replikat Dossier B: Boot + Flags 9:166/167 -> ROOM1000 -> ROOM1050: obj 0/1 aus (Port-Ist).
 *   S3  ISOLATION: wie S2, aber zwischen ROOM1000 und ROOM1050 ein scd_vm_init() (nullt NUR
 *       g_scd + s_prop_taken_hidden; die Flag-Zonen liegen in game_state.c und bleiben gesetzt).
 *       Sind die Props danach AN, ist der prozessweite Rest ausserhalb von g_scd (= die Maske)
 *       die einzige Ursache; sind sie weiterhin AUS, liegt die Ursache woanders (g_aot, Flags,
 *       Renderer) und das Dossier haette die falsche Stelle benannt.
 *   S4  Reihenfolge: Boot + Flags -> ROOM1050 (AN?) -> ROOM1000 -> ROOM1050 (AUS?) -> ROOM1000
 *       -> ... zeigt, dass der Zustand ab dem ersten "genommenen" Raum klebt und nicht vom
 *       Eintritts-Pfad abhaengt.
 *   S5  Kein Flag gesetzt, aber Raumkette 1000 -> 1050: Props AN (die Flags sind der Ausloeser,
 *       nicht die Raumkette selbst).
 *   S6  Nur EIN Bit (9:166, tk_prop 0): in ROOM1050 muss dann NUR obj 0 fehlen, obj 1 bleibt AN
 *       (bitgenaue Kopplung obj_id <-> tk_prop, wie im Dossier 1.3 fuer 11F0/obj 2 behauptet).
 *
 * Rueckgabe: 0 = Dossier-Ursache bestaetigt (S2 zeigt Defekt, S3 hebt ihn auf, S5/S6 wie
 * erwartet); != 0 = eine Dossier-Behauptung ist widerlegt (Ausgabe sagt welche). */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t g_scd;

static int g_bad = 0;
#define EXPECT(cond, ...) do { \
    if (!(cond)) { g_bad++; printf("  WIDERLEGT: " __VA_ARGS__); printf("\n"); } \
    else         {          printf("  bestaetigt: " __VA_ARGS__); printf("\n"); } \
} while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

typedef struct { unsigned id; re15_rdt_t rdt; uint8_t *buf; } room_t;

static int room_load(room_t *r, unsigned id)
{
    char path[256]; size_t n = 0;
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE%u/ROOM%04X.RDT", (id >> 12) & 0xF, id);
    r->id = id; r->buf = slurp(path, &n);
    if (!r->buf) { printf("  FEHLER: %s nicht lesbar\n", path); return 0; }
    if (re15_rdt_parse(r->buf, n, &r->rdt) != 0) { printf("  FEHLER: RDT-Parse %s\n", path); return 0; }
    return 1;
}

static void boot(void)
{
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
}

static void enter(room_t *r, int32_t px, int32_t pz, int ticks)
{
    extern unsigned g_current_room_id;
    g_current_room_id = r->id;
    scd_room_reenter(&r->rdt, px, pz, 0);
    for (int i = 0; i < ticks; i++) scd_vm_tick();
}

static int prop_active(unsigned obj_id)
{
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        if (g_scd.props[i].obj_id == obj_id) return g_scd.props[i].active ? 1 : 0;
    return -1;
}

static void dump(const char *tag)
{
    printf("  [%s] prop_count=%u:", tag, (unsigned)g_scd.prop_count);
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        printf(" slot%d/obj%02X=%s", i, g_scd.props[i].obj_id, g_scd.props[i].active ? "AN" : "aus");
    printf("\n");
}

int main(void)
{
    room_t r1000, r1050;
    if (!room_load(&r1000, 0x1000) || !room_load(&r1050, 0x1050)) return 2;

    printf("== S1 Kontrolle: Boot -> ROOM1050\n");
    boot();
    enter(&r1050, 20600, 12350, 60);
    dump("1050 frisch");
    EXPECT(prop_active(0) == 1 && prop_active(1) == 1, "S1 frisch: obj0/obj1 AN");

    printf("== S2 Replikat: Boot + 9:166/167 -> 1000 -> 1050\n");
    boot();
    re15_game_flag_set(9, 166, 1);
    re15_game_flag_set(9, 167, 1);
    enter(&r1000, 21850, -13400, 60);
    dump("1000");
    EXPECT(prop_active(0) == 0 && prop_active(1) == 0, "S2 in 1000 selbst: obj0/obj1 AUS");
    enter(&r1050, 20600, 12350, 60);
    dump("1050 nach 1000");
    EXPECT(prop_active(0) == 0 && prop_active(1) == 0, "S2 Dossier-Defekt reproduziert (1050 obj0/obj1 AUS)");

    printf("== S3 Isolation: wie S2, aber scd_vm_init() zwischen 1000 und 1050 (Flags bleiben)\n");
    boot();
    re15_game_flag_set(9, 166, 1);
    re15_game_flag_set(9, 167, 1);
    enter(&r1000, 21850, -13400, 60);
    scd_vm_init();               /* einzige Stelle, die s_prop_taken_hidden nullt; wischt aber
                                  * via re15_game_state_init() auch g_game (Flags), daher: */
    re15_game_flag_set(9, 166, 1);
    re15_game_flag_set(9, 167, 1);
    EXPECT(re15_game_flag_get(9, 166) && re15_game_flag_get(9, 167), "S3 Flags nach vm_init wieder gesetzt");
    enter(&r1050, 20600, 12350, 60);
    dump("1050 nach 1000 + vm_init");
    EXPECT(prop_active(0) == 1 && prop_active(1) == 1,
           "S3 nach Nullung der Maske: obj0/obj1 AN -> Maske ist die alleinige Ursache");

    printf("== S4 Reihenfolge: Boot + Flags -> 1050 -> 1000 -> 1050 -> 1000 -> 1050\n");
    boot();
    re15_game_flag_set(9, 166, 1);
    re15_game_flag_set(9, 167, 1);
    enter(&r1050, 20600, 12350, 60);
    dump("1050 #1");
    EXPECT(prop_active(0) == 1 && prop_active(1) == 1, "S4 1050 vor jedem Item-Raum: AN");
    enter(&r1000, 21850, -13400, 60);
    enter(&r1050, 20600, 12350, 60);
    dump("1050 #2");
    EXPECT(prop_active(0) == 0 && prop_active(1) == 0, "S4 1050 nach 1000: AUS (klebt)");
    enter(&r1000, 21850, -13400, 60);
    enter(&r1050, 20600, 12350, 60);
    dump("1050 #3");
    EXPECT(prop_active(0) == 0 && prop_active(1) == 0, "S4 bleibt AUS");

    printf("== S5 Ohne Flags: 1000 -> 1050\n");
    boot();
    enter(&r1000, 21850, -13400, 60);
    dump("1000 ohne Flags");
    enter(&r1050, 20600, 12350, 60);
    dump("1050 nach 1000 ohne Flags");
    EXPECT(prop_active(0) == 1 && prop_active(1) == 1, "S5 ohne Taken-Bits: obj0/obj1 AN");

    printf("== S6 Nur 9:166 (tk_prop 0): 1000 -> 1050\n");
    boot();
    re15_game_flag_set(9, 166, 1);
    enter(&r1000, 21850, -13400, 60);
    dump("1000 nur 166");
    enter(&r1050, 20600, 12350, 60);
    dump("1050 nach 1000 nur 166");
    EXPECT(prop_active(0) == 0 && prop_active(1) == 1, "S6 nur obj0 AUS, obj1 AN (Bit-Kopplung)");

    printf("%s: %d Widerlegung(en)\n", g_bad ? "ABWEICHUNG" : "DOSSIER-URSACHE BESTAETIGT", g_bad);
    return g_bad ? 1 : 0;
}
