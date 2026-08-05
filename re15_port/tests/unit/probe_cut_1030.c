/* probe_cut_1030.c — MESSUNG (kein Fix): Nutzer-Report
 *   "In ROOM1030 triggert die Cutscene mit den Zombies komisch und man erhaelt
 *    die Kontrolle nicht (zurueck)."
 *
 * Zweck: den ECHTEN Port-Zustand protokollieren, BEVOR irgendein Fix vorgeschlagen
 * wird. Gemessen wird:
 *   - welche AOTs main00 installiert (Typ/Pool-Maske/Rect)
 *   - der Aktor-Roster nach main00/sub00
 *   - ob/wann Flags (5,32)/(5,33)/(5,34) gesetzt werden
 *   - ob sub01 (der Cutscene-Poll) je wieder laeuft
 *   - ob sub08 (die Cutscene) je startet, und ob Flag(4,15) gesetzt wird
 *   - den main.c-Cine-Gate-Ausdruck  flag(1,27)||flag(2,7)  ueber die Zeit
 *
 * ORIGINAL-Referenz (ROOM1030.RDT, Datei-Offsets; main_scd@0x1c5c, sub_scd@0x1fd0):
 *   main00 +0x006c  Aot_set slot=3 sce=4 flags=0x41 rect(-19900,-9200,3700,1800)
 *                   data=[5,0x20,1]   -> Set flag(5,32)=1 bei SPIELER im Rect
 *   main00 +0x0080  Aot_set slot=4 sce=4 flags=0x42 rect(-14800,-22500,12000,2200)
 *                   data=[5,0x21,1]   -> Set flag(5,33)=1 bei GEGNER im Rect
 *   main00 +0x0094  Aot_set slot=5 sce=4 flags=0x42 rect(-12900,-25300,9200,1100)
 *                   data=[5,0x22,1]   -> Set flag(5,34)=1 bei GEGNER im Rect
 *   sub01  +0x0000  if Ck(3,116)==1 && Ck(4,15)==0 && Ck(5,32)==1
 *                     if Ck(5,34)==1 { Set(4,15)=1; Evt_exec(0xff,0x18,sub08) }
 *   sub08  +0x0000  Set(2,7)=1 ; Set(1,27)=1 ; ... Sleep-Kette ... ;
 *                   +0x006e Set(2,7)=0 ; +0x0072 Set(1,27)=0 ; +0x0076 Set(5,20)=1
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
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

static const char *aot_type_name(uint8_t t)
{
    switch (t) {
    case RE15_AOT_TYPE_GENERIC:          return "GENERIC";
    case RE15_AOT_TYPE_DOOR:             return "DOOR";
    case RE15_AOT_TYPE_ITEM:             return "ITEM";
    case RE15_AOT_TYPE_CAM_SWITCH:       return "CAM_SWITCH";
    case RE15_AOT_TYPE_STAIR:            return "STAIR";
    case RE15_AOT_TYPE_MESSAGE:          return "MESSAGE";
    case RE15_AOT_TYPE_AUTO_EVENT:       return "AUTO_EVENT";
    case RE15_AOT_TYPE_EXAMINE_WORKVAR:  return "EXAMINE_WV";
    case RE15_AOT_TYPE_FLAG_CHG:         return "FLAG_CHG";
    case RE15_AOT_TYPE_WATER:            return "WATER";
    case RE15_AOT_TYPE_RAMP:             return "RAMP";
    case RE15_AOT_TYPE_NONE:             return "NONE";
    default:                             return "?";
    }
}

static void dump_aots(void)
{
    printf("== AOT-Tabelle ==\n");
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active) continue;
        if (a->type == RE15_AOT_TYPE_CAM_SWITCH) continue;   /* RVD-Rauschen ausblenden */
        printf("  slot=%2d %-11s ev=%3u sce_flags=0x%02x band=0x%02x rect=(%ld,%ld) half=(%ld,%ld)",
               i, aot_type_name(a->type), a->event_id, a->sce_flags, a->band,
               (long)a->x, (long)a->z, (long)a->half_w, (long)a->half_h);
        if (a->type == RE15_AOT_TYPE_FLAG_CHG)
            printf("  FLAG(zone=%u,bit=%u,on=%u)",
                   g_aot.flag_params[i].group, g_aot.flag_params[i].bit, g_aot.flag_params[i].on);
        printf("\n");
    }
}

static void dump_roster(const char *tag)
{
    int n = 0;
    printf("== Roster %s ==\n", tag);
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *a = &g_actors[s];
        if (!a->active) continue;
        n++;
        if (n <= 6 || s == RE15_ACTOR_SLOT_PLAYER)
            printf("  slot=%2d type=0x%02X grid=0x%02X st=%u pos=(%ld,%ld,%ld) rot=%d floor=%u\n",
                   s, a->type, a->grid_id, a->state,
                   (long)a->x, (long)a->y, (long)a->z, (int)a->rot_y, a->floor);
    }
    printf("  ... total aktiv = %d\n", n);
}

static void dump_threads(const char *tag)
{
    printf("[threads %s]", tag);
    for (int i = 0; i < SCD_THREAD_COUNT; i++)
        if (g_scd.threads[i].active) printf(" %d", i);
    printf("\n");
}

static void dump_flags(const char *tag)
{
    printf("[flags %-16s] (3,116)=%d (4,15)=%d (5,20)=%d (5,32)=%d (5,33)=%d (5,34)=%d "
           "| (1,27)=%d (2,7)=%d -> cine_active=%d | pmode=%d lb=%d cam=%u\n",
           tag,
           re15_game_flag_get(3, 116), re15_game_flag_get(4, 15), re15_game_flag_get(5, 20),
           re15_game_flag_get(5, 32),  re15_game_flag_get(5, 33), re15_game_flag_get(5, 34),
           re15_game_flag_get(1, 27),  re15_game_flag_get(2, 7),
           re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7),
           g_scd.player_mode, g_scd.letterbox_countdown, g_scd.cam_id);
}

/* Nachbau des main.c-Cine-Gates (platform/pc/main.c:3012-3035) — genau der Ausdruck,
 * der im echten Lauf player_mode setzt. Wir fahren ihn hier mit, um zu MESSEN, was
 * der Spieler-Modus im echten Loop taete. */
static int s_cine_was_active = 0;
static void cine_gate_tick(void)
{
    int cine_active = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
    if (cine_active) {
        g_scd.player_mode = 2;
        g_scd.letterbox_countdown = -1;
    } else if (s_cine_was_active) {
        g_scd.letterbox_countdown = 15;
    }
    s_cine_was_active = cine_active;
    if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0)
        g_scd.player_mode = 0;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    size_t sz = 0;

    snprintf(path, sizeof path, "%s/STAGE1/ROOM1030.RDT", base);
    uint8_t *dat = read_file(path, &sz);
    if (!dat) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(dat, sz, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }
    printf("ROOM1030: cuts=%d zones=%d sub_scd_count=%d\n",
           rdt.cut_count, rdt.zone_count, rdt.sub_scd_count);

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1030;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    /* Spawn: Tuer 0 des Raums (main00 +0x000c Door_aot_set slot=0) — wir setzen den
     * Spieler bewusst NICHT in eine der Flag-Zonen, damit der Erst-Zustand sauber ist. */
    pl->x = -25000; pl->y = 0; pl->z = -20000; pl->rot_y = 2048;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);

    dump_aots();
    dump_roster("nach main00/sub00");
    dump_threads("nach Entry");
    dump_flags("Entry");

    /* ---- Phase 1: 120 Ticks Leerlauf (Spieler ausserhalb aller Flag-Zonen) ---- */
    for (int t = 0; t < 120; t++) {
        scd_vm_tick();
        re15_aot_scan(pl->x, pl->z, 0);
        cine_gate_tick();
    }
    dump_flags("nach 120 idle");
    dump_threads("nach 120 idle");

    /* ---- Phase 2: Spieler in AOT3 (sce=4, Pool=Spieler) -> Flag(5,32) ---- */
    printf("\n== Phase 2: Spieler in AOT-3-Rect (Flag 5,32) ==\n");
    pl->x = -19900; pl->z = -9200;
    for (int t = 0; t < 60; t++) {
        scd_vm_tick();
        re15_aot_scan(pl->x, pl->z, 0);
        cine_gate_tick();
    }
    dump_flags("nach AOT3");

    /* ---- Phase 3: einen GEGNER in AOT5 setzen (Pool=Gegner) -> Flag(5,34) ---- */
    printf("\n== Phase 3: Gegner-Slot 1 in AOT-5-Rect (Flag 5,34) ==\n");
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (g_actors[s].active) {
            g_actors[s].x = -12900; g_actors[s].z = -25300;
            g_actors[s].floor = 0;
            printf("  Gegner slot=%d nach (-12900,-25300) gesetzt (floor=%u)\n", s, g_actors[s].floor);
            break;
        }
    }
    for (int t = 0; t < 60; t++) {
        scd_vm_tick();
        re15_aot_scan(pl->x, pl->z, 0);
        cine_gate_tick();
    }
    dump_flags("nach AOT5");
    dump_threads("nach AOT5");

    /* ---- Phase 4: 900 Ticks weiter — kommt die Cutscene? kommt die Kontrolle zurueck? ---- */
    printf("\n== Phase 4: 900 Ticks (Cutscene-Dauer sub08 = 5+20+10+15+20+180+200 = 450 Ticks) ==\n");
    int prev_c415 = re15_game_flag_get(4, 15);
    int prev_cine = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
    int prev_pmode = g_scd.player_mode;
    uint8_t prev_cam = g_scd.cam_id;
    for (int t = 0; t < 900; t++) {
        scd_vm_tick();
        re15_aot_scan(pl->x, pl->z, 0);
        cine_gate_tick();
        int c415 = re15_game_flag_get(4, 15);
        int cine = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
        if (c415 != prev_c415 || cine != prev_cine || g_scd.player_mode != prev_pmode ||
            g_scd.cam_id != prev_cam || (t % 150) == 0) {
            char tag[32]; snprintf(tag, sizeof tag, "t=%d", t);
            dump_flags(tag);
            prev_c415 = c415; prev_cine = cine; prev_pmode = g_scd.player_mode;
            prev_cam = g_scd.cam_id;
        }
    }
    dump_flags("Ende");
    dump_threads("Ende");

    /* ---- Phase 5: sub08 ZWANGSSTART (Evt_exec-Aequivalent), um den Cutscene-Ablauf
     *      selbst zu messen — unabhaengig vom sub01-Poll. ---- */
    printf("\n== Phase 5: sub08 direkt starten (was TUT die Cutscene im Port?) ==\n");
    if (rdt.sub_scd[8]) {
        int slot = -1;
        for (int i = SCD_EVENT_SLOT_FIRST; i <= SCD_EVENT_SLOT_LAST; i++)
            if (!g_scd.threads[i].active) { slot = i; break; }
        printf("  sub08 -> thread slot %d\n", slot);
        if (slot >= 0) scd_thread_start(slot, rdt.sub_scd[8]);
        prev_cine = -1; prev_pmode = -1; prev_cam = 0xFF;
        for (int t = 0; t < 900; t++) {
            scd_vm_tick();
            re15_aot_scan(pl->x, pl->z, 0);
            cine_gate_tick();
            int cine = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
            if (cine != prev_cine || g_scd.player_mode != prev_pmode ||
                g_scd.cam_id != prev_cam || (t % 150) == 0) {
                char tag[32]; snprintf(tag, sizeof tag, "c t=%d", t);
                dump_flags(tag);
                prev_cine = cine; prev_pmode = g_scd.player_mode; prev_cam = g_scd.cam_id;
            }
        }
        dump_flags("nach sub08");
        dump_threads("nach sub08");
    }

    /* ================= SZENARIO B: RE-ENTRY mit stehenden Flags =================
     * Frisch aufsetzen: Flags (5,32)+(5,34) sind gesetzt (der Spieler war frueher
     * in AOT3, ein Zombie in AOT5) — aber (4,15) noch 0, weil die Cutscene im Port
     * live nie feuerte. Jetzt den Raum NEU laden (Tuer rein). scd_room_reenter
     * startet sub01 EINMAL -> beide Bedingungen stehen -> Cutscene beim Betreten. */
    printf("\n\n##### SZENARIO B: Raum-Neubetretung mit stehenden Flags (5,32)+(5,34) #####\n");
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1030;
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -25000; pl->y = 0; pl->z = -20000; pl->rot_y = 2048;
    re15_game_flag_set(5, 32, 1);
    re15_game_flag_set(5, 34, 1);
    s_cine_was_active = 0;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    dump_flags("direkt nach Raum-Load");
    dump_threads("direkt nach Raum-Load");
    {
        int prev_cine2 = -1, prev_pm2 = -1; uint8_t prev_cam2 = 0xFF;
        for (int t = 0; t < 600; t++) {
            scd_vm_tick();
            re15_aot_scan(pl->x, pl->z, 0);
            cine_gate_tick();
            int cine = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
            if (cine != prev_cine2 || g_scd.player_mode != prev_pm2 ||
                g_scd.cam_id != prev_cam2 || (t % 200) == 0) {
                char tag[32]; snprintf(tag, sizeof tag, "B t=%d", t);
                dump_flags(tag);
                prev_cine2 = cine; prev_pm2 = g_scd.player_mode; prev_cam2 = g_scd.cam_id;
            }
        }
    }

    /* ================= SZENARIO C: Raumwechsel MITTEN in der Cutscene =============
     * Cutscene laeuft (Flags 1,27 + 2,7 gesetzt) -> Tuer -> scd_room_reenter loescht
     * bank1 idx16..31 (also 1,27) UND memset(g_scd) => sub08-Thread ist tot, aber
     * Flag (2,7) ueberlebt (das Original loescht auf Raum-Load nur DAT_800aca3c,
     * @0x80039710-30 `and 0xffff0000` — bank2/0x800aca40 bleibt stehen).
     * Der main.c-Gate liest (2,7) weiter -> player_mode wird JEDEN Frame auf 2
     * gezogen: Kontrolle kommt NIE zurueck. */
    printf("\n\n##### SZENARIO C: Raumwechsel mitten in der Cutscene #####\n");
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1030;
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -25000; pl->y = 0; pl->z = -20000; pl->rot_y = 2048;
    s_cine_was_active = 0;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    if (rdt.sub_scd[8]) {
        int slot = -1;
        for (int i = SCD_EVENT_SLOT_FIRST; i <= SCD_EVENT_SLOT_LAST; i++)
            if (!g_scd.threads[i].active) { slot = i; break; }
        if (slot >= 0) scd_thread_start(slot, rdt.sub_scd[8]);
    }
    for (int t = 0; t < 60; t++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); cine_gate_tick(); }
    dump_flags("C mitten in Cutscene");
    /* Tuer: das Aequivalent von re15_room_apply_pending (room_common.c:120-140) */
    {
        re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        p->motion = 0; p->anim_frame = 0; p->anim_flags = 0;
        p->state = 1; g_scd.player_mode = 0;
    }
    scd_room_reenter(&rdt, pl->x, pl->z, 0);   /* gleicher Raum reicht fuer die Messung */
    dump_flags("C direkt nach Raumwechsel");
    dump_threads("C direkt nach Raumwechsel");
    for (int t = 0; t < 600; t++) {
        scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); cine_gate_tick();
        if (t == 0 || t == 30 || t == 200 || t == 599) {
            char tag[32]; snprintf(tag, sizeof tag, "C t=%d", t);
            dump_flags(tag);
        }
    }
    printf("C-BEFUND: player_mode=%d (2 = SKRIPT = keine Kontrolle), flag(2,7)=%d\n",
           g_scd.player_mode, re15_game_flag_get(2, 7));

    free(dat);
    return 0;
}
