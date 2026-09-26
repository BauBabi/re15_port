/* test_r27_ada_cutscene_1050.c — RIEGEL zum Nutzer-Befund (2026-09-26, nach v0.8.13):
 * "beim Start der cutscene mit leon und ada beim Wechsel von room 1090 zu room 1050
 *  erneut am Anfang adas animation mehrfach ausgeloest."
 *
 * GEMESSEN (echte exe, echter Weg ROOM1090 -> Tuer-Slot 0 -> ROOM1050, RE15_ANIM_TRACE,
 * Rohdaten analysis/befunde_2026-09-27/): Ada (Typ 0x42) posierte in der Cutscene
 *
 *   F0..F5   Clip 2  Bild 0        Tuer-Blende (Pausemaske 0xFF000000)
 *   F6..F8   Clip 5  Bild 1,2,3    Sub-9-Turn   (Plc_dest @RDT 0x0DAA, mode 9)
 *   F9..F24  Clip 1  Bild 1..15,0  Sub-6 EVENT-REACH (Turn ausgerichtet @0x80051dac)
 *   F25..F34 Clip 2  Bild 1..10    Sub-6-Ruhe
 *   F35..    Clip 0  Bild 1..      Sub-5-Lauf   (sub04 @RDT 0x0E04, mode 5)
 *
 * JEDER Clip begann also bei Bild 1 statt 0, und der abspiel-einmal-Clip 1 endete auf
 * seinem Bild **0** statt auf Bild 15: die Geste sprang in ihrem letzten Bild sichtbar
 * auf die ANFANGSPOSE zurueck, bevor der naechste Clip anfing — das ist die gemeldete
 * mehrfache Ausloesung.
 *
 * ORIGINAL (selbst disassembliert aus info/Re1.5/PSX.EXE): anim_set FUN_8001f314 posiert
 * das AKTUELLE +0x95 und schiebt erst danach vor —
 *     8001f35c  lbu v0,149(t0)     ; Index = +0x95, unveraendert
 *     8001f36c  sw  a2,360(t0)     ; +0x168 = Frame-Zeiger  <- die POSE
 *     8001f610  lbu v0,149(v1) / 8001f618 addiu v0,v0,1 / 8001f61c sb v0,149(v1)
 *     8001f624  sltu v0,v0,s4 / 8001f628 bne -> weiter
 *     8001f63c  sb  zero,149(v1)   ; sonst +0x95 = 0 und RUECKGABE 1 = Clip zu Ende
 * Die Phase-0-Zweige der Executor-Subs setzen +0x95 = 0 und fallen im SELBEN Tick in
 * ihren Body durch, der f314 ruft (Sub 9 @0x80051d28-5c -> Body @0x80051d60,
 * Sub 5 @0x800514d4-38 -> Body @0x8005153c, Sub 6 @0x80051844 -> @0x80051878).
 * Der erste Tick eines Clips posiert deshalb Bild 0, der letzte Bild fc-1.
 *
 * DER RIEGEL faehrt den Eintritt wie test_r18_ada_eintritt_1050 (ROOM1050.RDT,
 * Flag(3,0x6e), scd_room_reenter) und tickt danach in der Original-Reihenfolge
 * SCD-VM (@0x8001cdec) -> Entity-Schleife (@0x8001ce04). Geprueft wird der Posen-Strom:
 *   M1 erster Tick von Clip 5 (Sub 9)  -> Bild 0
 *   M2 erster Tick von Clip 1 (Sub 6)  -> Bild 0
 *   M3 letzter Tick von Clip 1         -> Bild 15 (= fc-1), NICHT 0
 *   M4 erster Tick von Clip 2 (Sub 6)  -> Bild 0
 *
 * GEGENPROBE am alten Stand (re15_npc_anim schiebt VOR dem Posieren vor): M1/M2/M4
 * liefern 1 und M3 liefert 0 -> vier rote Pruefungen, Exit 1.
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
#include "re15_enemy_ai.h"

extern scd_vm_t g_scd;

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

/* Original-Reihenfolge des Haupt-Loops: SCD-VM @0x8001cdec, danach Entity-/KI-Schleife
 * @0x8001ce04. (Die uebrigen Passes des Loops beruehren +0x94/+0x95 eines State-4-NPC
 * nicht — der globale Advancer laesst NPC-Familie in State 4/1 aus, player_common.c.) */
static void frame_step(void)
{
    scd_vm_tick();
    re15_enemy_ai_run_all(0);
}

#define NTICK 60

int main(void)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM1050.RDT", RE15_ASSET_PSX_DIR);
    size_t sz = 0;
    uint8_t *raw = read_file(rp, &sz);
    if (!raw) { printf("SKIP: %s fehlt\n", rp); return 77; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(raw, sz, &rdt) < 0) { printf("FAIL: RDT-Parse\n"); free(raw); return 1; }

    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_enemy_ai_set_paused(0);
    g_current_room_id = 0x1050;
    g_room_change.pending = 0;

    re15_game_flag_set(3, 0x6e, 1);          /* Tor aus ROOM1090 sub03 @0x24D6 */

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 19850; pl->y = 0; pl->z = -22300; pl->rot_y = 3072;   /* Door_aot_set @0x211A */

    scd_room_reenter(&rdt, pl->x, pl->z, 6);

    re15_actor_t *ada = &g_actors[1];
    if (!ada->active || ada->type != 0x42) {
        printf("FAIL: ROOM1050 sub00 @0x0C8E hat Ada (Typ 0x42) nicht gespawnt\n");
        free(raw); return 1;
    }

    uint8_t mo[NTICK], fr[NTICK];
    for (int t = 0; t < NTICK; t++) {
        frame_step();
        mo[t] = ada->motion; fr[t] = ada->anim_frame;
    }
    free(raw);

    printf("[Posen-Strom Ada]");
    for (int t = 0; t < NTICK; t++) printf(" %d:%d/%d", t, (int)mo[t], (int)fr[t]);
    printf("\n");

    /* erster/letzter Tick je Clip-Block */
    int i5 = -1, i1 = -1, l1 = -1, i2 = -1;
    for (int t = 0; t < NTICK; t++) {
        if (i5 < 0 && mo[t] == 5) i5 = t;
        if (i5 >= 0 && i1 < 0 && mo[t] == 1) i1 = t;
        if (i1 >= 0 && mo[t] == 1) l1 = t;
        if (i1 >= 0 && i2 < 0 && t > i1 && mo[t] == 2) i2 = t;
    }
    int fails = 0;
    if (i5 < 0 || i1 < 0 || i2 < 0) {
        printf("FAIL: Clip-Kette 5 -> 1 -> 2 nicht gefahren (i5=%d i1=%d i2=%d)\n", i5, i1, i2);
        return 1;
    }
    if (fr[i5] != 0) {
        printf("FAIL M1: erster Tick von Clip 5 (Sub-9-Turn, +0x94=5 @0x80051d3c / +0x95=0 "
               "@0x80051d4c) posiert Bild %d, erwartet 0 (f314 posiert +0x95 @0x8001f35c/"
               "@0x8001f36c und schiebt erst @0x8001f618 vor)\n", (int)fr[i5]);
        fails++;
    }
    if (fr[i1] != 0) {
        printf("FAIL M2: erster Tick von Clip 1 (Sub-6 Phase 0 @0x80051844/@0x80051854) "
               "posiert Bild %d, erwartet 0\n", (int)fr[i1]);
        fails++;
    }
    if (fr[l1] != 15) {
        printf("FAIL M3: letzter Tick von Clip 1 posiert Bild %d, erwartet 15 = fc-1 "
               "(f314 gibt erst NACH dem Posieren des letzten Bildes 1 zurueck, "
               "@0x8001f624-3c). Bild 0 heisst: die Geste springt im letzten Bild auf "
               "ihre Anfangspose zurueck.\n", (int)fr[l1]);
        fails++;
    }
    if (fr[i2] != 0) {
        printf("FAIL M4: erster Tick von Clip 2 (Sub-6 Phase 2 @0x800518b4/@0x800518c8) "
               "posiert Bild %d, erwartet 0\n", (int)fr[i2]);
        fails++;
    }

    if (fails) { printf("FAIL: %d Pruefung(en)\n", fails); return 1; }
    printf("OK: Adas Cutscene-Clips starten auf Bild 0 und enden auf fc-1 "
           "(Clip 5 @%d, Clip 1 @%d..%d, Clip 2 @%d)\n", i5, i1, l1, i2);
    return 0;
}
