/* probe_r16_objekte_paket.c — MESSSONDE Runde 16 (2026-09-19), Thema "objekte-paket".
 *
 * Nutzer-Befund: "In der paketierten Version fehlen diverse Dinge in ROOM 11F0 mindestens
 * die Schalter vom Raetsel, in ROOM 1050 die zerteilte Leiche, in ROOM 1090 die Kiste usw."
 *
 * Alle drei Objekte sind Obj_model_set-Props (RDT-Props, 0x2D):
 *   ROOM1050: obj 0x01 main00 @0x0BB0 (14775,0,-10125), obj 0x00 sub00 @0x0C36 (15432,0,-10424)
 *             = die zwei Haelften der Leiche unter dem Rolltor (Cut 3/4/5).
 *   ROOM1090: obj 0x00 main00 @0x216E (-8136,-1800,4753) Typ 4, obj 0x01 @0x2190 Typ 4 = Kisten.
 *   ROOM11F0: obj 0x02..0x0B sub00 @0x0E76..0x0FA8 = die 10 Schalter (Cut 10), obj 0x00 = Cursor.
 *
 * Gemessen mit der Paket-exe v0.8.4 (Dossier analysis/befunde_2026-09-19/objekte-paket.md):
 * frisch per Debug-JUMP betreten werden ALLE Props gezeichnet ([prop-render] je Prop). Erst
 * NACH einem Raum, dessen Item_aot_set ein bereits genommenes Item traegt (Flag-Zone 9), fehlen
 * im Folgeraum die Props mit derselben obj_id — 0 [prop-render] in ROOM1050 nach ROOM1000.
 *
 * Ursache im Port (scd_vm.c): `static uint32_t s_prop_taken_hidden` wird NUR in scd_vm_init()
 * (= Boot) genullt, NICHT in scd_room_reenter() (scd_room_setup.c:211 memset(&g_scd) — die
 * Maske liegt ausserhalb von g_scd). Item_aot_set eines genommenen Items (@0x800406d4-0x80040718
 * im Original: pool[tk_prop].flags = 0x80000000, RAUMLOKAL, weil FUN_8003ea7c beim Raumladen
 * (Aufruf aus FUN_800396fc) alle 32 Pool-Eintraege nullt: @0x8003eab0-8003eacc
 * `sw zero,0(at); addiu s0,s0,1; sltiu v0,s0,0x20; bne ...; addiu v1,v1,148`) setzt im Port das
 * Bit 1<<tk_prop prozessweit; jeder spaetere Obj_model_set mit obj_id == tk_prop wird dann mit
 * active=0 angelegt (scd_vm.c:3803).
 *
 * Diese Sonde belegt das direkt an g_scd.props[].active — OHNE Renderer:
 *   A  Kontrolle: frischer Boot -> ROOM1050: obj 0/1 aktiv.
 *   B  Boot + Flags 9:166/9:167 (ROOM1000-Items tk_prop 0/1 genommen) -> ROOM1000 -> ROOM1050:
 *      Original: obj 0/1 aktiv (Pool wird beim Laden genullt). Port-Ist: 0.
 *   C  Boot + Flags 9:140/141/142 (ROOM1010-Items tk_prop 0/1/2) -> ROOM1010 -> ROOM11F0:
 *      Original: Cursor 0, obj 1, Schalter 2 aktiv. Port-Ist: 0.
 *   D  Boot + Flags 9:166/167 -> ROOM1000 -> ROOM1090: Kisten obj 0/1 aktiv. Port-Ist: 0.
 *   E  Gegenprobe, die der Fix ERHALTEN muss: Boot + Flags 9:166/167 -> ROOM1000 selbst:
 *      obj 0/1 (die genommenen Items) NICHT aktiv (Original @0x80040718 pool.flags=0x80000000).
 *
 * Rueckgabe != 0, solange der Defekt besteht (Soll = Original). Nach dem Fix als add_test
 * registrieren (probes/r16_objekte-paket.cmake). */
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

static int g_fail = 0;
#define CHECK(cond, ...) do { \
    if (!(cond)) { g_fail++; printf("  FAIL: " __VA_ARGS__); printf("\n"); } \
    else         {           printf("  ok:   " __VA_ARGS__); printf("\n"); } \
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

/* Boot-Sequenz wie main.c (re15_actor_init/re15_aot_init/scd_vm_init) */
static void boot(void)
{
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
}

/* Raum betreten wie ueber Tuer/Debug-JUMP: scd_room_reenter (registriert das RDT, startet
 * main00 + sub00), dann N Ticks, damit die Installer-Opcodes laufen. */
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
    return -1;   /* nicht installiert */
}

static void dump_props(const char *tag)
{
    printf("  [%s] prop_count=%u:", tag, (unsigned)g_scd.prop_count);
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        printf(" obj%02X=%s", g_scd.props[i].obj_id, g_scd.props[i].active ? "AN" : "aus");
    printf("\n");
}

int main(void)
{
    room_t r1000, r1010, r1050, r1090, r11f0;
    if (!room_load(&r1000, 0x1000) || !room_load(&r1010, 0x1010) || !room_load(&r1050, 0x1050) ||
        !room_load(&r1090, 0x1090) || !room_load(&r11f0, 0x11F0)) return 2;

    printf("== A Kontrolle: Boot -> ROOM1050\n");
    boot();
    enter(&r1050, 20600, 12350, 60);
    dump_props("1050 frisch");
    CHECK(prop_active(0x00) == 1 && prop_active(0x01) == 1, "A ROOM1050 Leiche obj0/obj1 aktiv");

    printf("== B Boot + Flags(9,166),(9,167) -> ROOM1000 -> ROOM1050\n");
    boot();
    re15_game_flag_set(9, 166, 1);   /* ROOM1000 sub00 @0x0C3A Item 0x24 tk_bit 166 tk_prop 0 */
    re15_game_flag_set(9, 167, 1);   /* ROOM1000 sub00 @0x0C50 Item 0x25 tk_bit 167 tk_prop 1 */
    enter(&r1000, 21850, -13400, 60);
    dump_props("1000 mit genommenen Items");
    printf("== E Gegenprobe im SELBEN Raum (muss der Fix erhalten)\n");
    CHECK(prop_active(0x00) == 0 && prop_active(0x01) == 0,
          "E ROOM1000 obj0/obj1 (genommene Items) NICHT sichtbar (@0x80040718 flags=0x80000000)");
    enter(&r1050, 20600, 12350, 60);
    dump_props("1050 nach 1000");
    CHECK(prop_active(0x00) == 1 && prop_active(0x01) == 1,
          "B ROOM1050 Leiche obj0/obj1 aktiv (Original: Pool @0x8003eab0-cc je Raum genullt)");

    printf("== C Boot + Flags(9,140/141/142) -> ROOM1010 -> ROOM11F0\n");
    boot();
    re15_game_flag_set(9, 140, 1);   /* ROOM1010 sub00 @0x0996 Item 0x22 tk_prop 0 */
    re15_game_flag_set(9, 141, 1);   /* ROOM1010 sub00 @0x09AC Item 0x15 tk_prop 1 */
    re15_game_flag_set(9, 142, 1);   /* ROOM1010 sub00 @0x09C2 Item 0x16 tk_prop 2 */
    enter(&r1010, 0, 0, 60);
    dump_props("1010 mit genommenen Items");
    enter(&r11f0, 250, 250, 60);
    dump_props("11F0 nach 1010");
    CHECK(prop_active(0x00) == 1, "C ROOM11F0 Cursor obj0 aktiv");
    CHECK(prop_active(0x01) == 1, "C ROOM11F0 obj1 aktiv");
    CHECK(prop_active(0x02) == 1, "C ROOM11F0 Schalter obj2 aktiv");
    for (unsigned o = 3; o <= 0x0B; o++)
        if (prop_active(o) != 1) { g_fail++; printf("  FAIL: C ROOM11F0 Schalter obj%X aktiv\n", o); }

    printf("== D Boot + Flags(9,166/167) -> ROOM1000 -> ROOM1090\n");
    boot();
    re15_game_flag_set(9, 166, 1);
    re15_game_flag_set(9, 167, 1);
    enter(&r1000, 21850, -13400, 60);
    enter(&r1090, -10200, 1400, 60);
    dump_props("1090 nach 1000");
    CHECK(prop_active(0x00) == 1 && prop_active(0x01) == 1, "D ROOM1090 Kisten obj0/obj1 aktiv");

    printf("%s: %d Abweichung(en) vom Original\n", g_fail ? "DEFEKT" : "OK", g_fail);
    return g_fail ? 1 : 0;
}
