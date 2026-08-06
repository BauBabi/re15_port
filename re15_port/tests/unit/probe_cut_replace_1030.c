/* probe_cut_replace_1030.c — MESSUNG (kein Fix), Nutzer-Report 2026-08-06:
 *   "Wenn ich ROOM1030 erneut betrete, wird die Kamera nicht auf den SPIELER
 *    gelegt, sondern auf die ZOMBIES."
 *
 * Gemessen wird der Cut_replace-Pfad beim WIEDERBETRETEN, exakt so wie ihn
 * room_common.c faehrt:
 *   Schritt (5)  Tuer-Eintritts-Cut setzen
 *   Schritt (7)  scd_room_reenter()  -> main00 + sub00 laufen
 *   Schritt (8)  wenn die SCD-Init einen ANDEREN Cut angemeldet hat, wird der
 *                geehrt und cut_auto BLEIBT 0 (Schritt 12 wird uebersprungen)
 *
 * ORIGINAL-Referenz:
 *   ROOM1030 sub00 @Datei 0x1fe8:
 *     +0x000 Set(3,116,1)
 *     +0x004 If(0x1e)
 *     +0x008   Ck(4,15,1)              <- "Cutscene ist schon gelaufen"
 *     +0x00c   Cut_replace(0,9)        @Datei 0x1ff4
 *     +0x00f   Cut_replace(3,10)       @Datei 0x1ff7
 *     +0x012   Cut_replace(4,11)       @Datei 0x1ffa
 *     +0x015   Cut_replace(6,12)       @Datei 0x1ffd
 *     +0x018   Sca_id_set(2,6,0xf7) / +0x01c Sca_id_set(3,6,0xf7)
 *     +0x020   Set(5,20,1)
 *     +0x024 Endif
 *   Tueren NACH ROOM1030 (Door_aot_set pc[23]=0x03, pc[24]=cut):
 *     ROOM1040 slot1 cut=8 | ROOM1050 slot0 cut=2 | ROOM1070 slot0 cut=7
 *     ROOM1031 slot19 cut=6 | ROOM1241 slot0 cut=0
 *   Original-Tail LAB_80040414 @0x800404ac-d0:
 *     if (*(u8*)(DAT_800ac794+2) == pc[1]) FUN_800142f4(pc[2]);
 *   Der Port vergleicht stattdessen g_re15_active_cut == pc[1] und macht daraus
 *   einen echten Kamerawechsel. g_re15_active_cut ist zum Zeitpunkt der SCD-Init
 *   noch der Cut des VORRAUMS (light_common.c setzt ihn erst in Schritt 10).
 *   Genau das wird hier durchgemessen: prev_cut = 0..12.
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
#include "re15_light.h"

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
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

static void dump_zone_labels(const re15_rdt_t *rdt, const char *tag)
{
    printf("  [zones %-8s]", tag);
    for (int i = 0; i < rdt->zone_count && i < 12; i++)
        printf(" %d->%d", rdt->zones[i].cam_from, rdt->zones[i].cam_to);
    printf(" ...\n");
}

/* Wieviele CAM_SWITCH-AOTs koennen den Spieler aus `cut` herausfuehren? */
static int exits_for_cut(int cut)
{
    int n = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++)
        if (g_aot.slots[i].active &&
            g_aot.slots[i].type == RE15_AOT_TYPE_CAM_SWITCH &&
            g_aot.slots[i].cam_from_filter == (uint8_t)cut) n++;
    return n;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    size_t sz = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1030.RDT", base);

    /* Die REALEN Tuer-Eintritts-Cuts nach ROOM1030 (aus Door_aot_set pc[24]). */
    const int door_cuts[]  = { 8, 2, 7, 6, 0 };
    const char *door_src[] = { "ROOM1040 slot1", "ROOM1050 slot0", "ROOM1070 slot0",
                               "ROOM1031 slot19", "ROOM1241 slot0" };

    printf("=== ROOM1030 WIEDERBETRETEN (Cutscene lief bereits: flag(4,15)=1) ===\n");
    printf("prev = g_re15_active_cut beim SCD-Init (= letzter Cut des VORRAUMS)\n\n");

    for (int di = 0; di < 5; di++) {
        printf("--- Tuer aus %s -> Eintritts-Cut %d ---\n", door_src[di], door_cuts[di]);
        for (int prev = 0; prev <= 12; prev++) {
            size_t n = 0;
            uint8_t *dat = read_file(path, &n);          /* FRISCHER Parse = echter Raum-Load */
            if (!dat) { fprintf(stderr, "FAIL: %s\n", path); return 1; }
            static re15_rdt_t rdt;
            if (re15_rdt_parse(dat, n, &rdt) != 0) { fprintf(stderr, "FAIL parse\n"); return 1; }

            re15_actor_init();
            scd_vm_init();
            g_current_room_id = 0x1030;
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            pl->active = 1; pl->type = 0; pl->hp = 100;
            pl->x = -18000; pl->y = 0; pl->z = -3500;    /* ROOM1070-Tuer-Spawn */
            /* Zustand NACH der Cutscene: */
            re15_game_flag_set(3, 116, 1);
            re15_game_flag_set(4, 15, 1);

            /* room_common Schritt (5): Tuer-Cut */
            int cut = door_cuts[di];
            /* Der Vorraum-Cut, den light_common zuletzt gesetzt hat: */
            g_re15_active_cut = prev;

            if (prev == 0 && di == 0) dump_zone_labels(&rdt, "vor");

            /* room_common Schritt (7) */
            scd_room_reenter(&rdt, pl->x, pl->z, 0);

            if (prev == 0 && di == 0) dump_zone_labels(&rdt, "nach");

            /* room_common Schritt (8) + (12), woertlich uebernommen */
            int scd_queued = (g_scd.cam_change_pending && (int)g_scd.cam_id != cut)
                           ? (int)g_scd.cam_id : -1;
            int auto_on;
            if (scd_queued >= 0) { cut = scd_queued; auto_on = g_scd.cut_auto_enabled; }
            else                 { auto_on = 1; }

            printf("   prev=%2d -> pending=%d cam_id=%2u | FINAL cut=%2d auto=%d exits=%d%s\n",
                   prev, g_scd.cam_change_pending, g_scd.cam_id, cut, auto_on,
                   exits_for_cut(cut),
                   (scd_queued >= 0) ? "   <== SCD-Cut geehrt (Kamera NICHT auf dem Tuer-Cut)" : "");
            free(dat);
        }
        printf("\n");
    }

    /* Kontrolle: OHNE flag(4,15) (Cutscene noch nicht gelaufen) darf gar nichts passieren. */
    printf("--- KONTROLLE: flag(4,15)=0 (Erstbetreten), Tuer-Cut 7 ---\n");
    for (int prev = 0; prev <= 6; prev++) {
        size_t n = 0;
        uint8_t *dat = read_file(path, &n);
        static re15_rdt_t rdt;
        re15_rdt_parse(dat, n, &rdt);
        re15_actor_init(); scd_vm_init();
        g_current_room_id = 0x1030;
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100;
        pl->x = -18000; pl->z = -3500;
        g_re15_active_cut = prev;
        scd_room_reenter(&rdt, pl->x, pl->z, 0);
        printf("   prev=%2d -> pending=%d cam_id=%2u zones[0]=%d->%d\n",
               prev, g_scd.cam_change_pending, g_scd.cam_id,
               rdt.zones[0].cam_from, rdt.zones[0].cam_to);
        free(dat);
    }
    return 0;
}
