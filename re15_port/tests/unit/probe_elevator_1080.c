/* probe_elevator_1080.c — FAHRSTUHL ROOM1080: Etagen-Wahl -> Ausgangs-Tür-Ziel.
 *
 * Nutzer-Report: "Der Fahrstuhl bringt mich auf jeder Etage zum gleichen falschen
 * Ausgang" — die Etagen-Wahl ändert das Ziel der Ausgangs-Tür nicht.
 *
 * SOLL-MECHANISMUS (RDT ROOM1080.RDT, mainScd @0x480, subScd @0x4E4 — Bytes selbst
 * gedumpt, Offsets = Datei-Offsets):
 *   main00 @0x482: DREI Null-Rect-Türen
 *     0x482  3b 00 02 31 ... dest=stage0 room0x04 cut5  -> ROOM1040 (1F WEST CORRIDOR)
 *     0x4A2  3b 01 02 31 ... dest=stage0 room0x0C cut0  -> ROOM10C0 (2F WEST SIDE)
 *     0x4C2  3b 02 02 31 ... dest=stage0 room0x12 cut0  -> ROOM1120 (3F WEST SIDE)
 *   sub00: Obj_model_set obj0 = Knopf-CURSOR @(-19554,22684);
 *     Aot_set slot3 sce3 0x31 rect(-12450,-450,800,800) ev-payload {ff 00 18 06}=PANEL
 *     Aot_set slot4..7 sce5 sat0x44 = Knopf-Zellen (Objekt-Pool, CENTRE):
 *       slot4 rect(-20600,20200,2050,2050) -> Etage 1F
 *       slot5 rect(-20600,23050,2050,2050) -> Etage 2F
 *       slot6 rect(-20600,26000,2050,2050) -> Etage 3F
 *       slot7 rect(-19400,15800,2050,2050) -> "aktuelle Etage" (Tür öffnen)
 *   sub06 (Panel-Action): Cut_chg 1 + Set flag(5, 0..8) = Steuer-Modus.
 *   sub01 (jedes Bild): d-pad (0x51-Masken 1/4/2/8) -> sub02..05 = Cursor ±200;
 *     Ck(5,4..7) + Work_set(3,0) + Member_cmp(15==4/5/6/7) + Sce_key_ck(1,0x4000)
 *     -> Gosub sub07/08/09/10.
 *   sub07/08/09 (Fahrt zu 1F/2F/3F): wenn flag(3,54/55/56)==0 -> Fahrt-Choreografie
 *     (Cut_chg 2/3/4, Sleeps, Spieler-Teleport, Plc_dest(mode4,bit0x20) + Do/Edwhile
 *     bis flag(5,0x20)) — dann IMMER `47 00/01/02` Aot_on(slot) = Tür-FIRE-NOW.
 *   sub10 ("aktuelle Etage"): Ck flag(3,54/55/56)==1 -> Set 0 -> Aot_on 0/1/2.
 *   Etagen-Flags z3 b54/55/56 pflegen die Etagen-Räume selbst (1040/10C0/1120 sub01).
 *
 * DIE SONDE fährt den ECHTEN UI-Pfad (Panel-Action-Press, d-pad-held Cursor-Fahrt,
 * Confirm-held 0x4000) und misst pro Etage, welcher Raum-Wechsel ankommt.
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
#include "re15_enemy.h"
#include "re15_enemy_ai.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern scd_vm_t g_scd;
extern re15_aot_state_t g_aot;
extern uint16_t g_scd_pad_held;
extern uint16_t g_scd_pad_edge;
extern uint8_t  g_aot_action_pressed;
extern uint16_t re15_pad_virtual_word(uint16_t phys);   /* pad_common.c (Preset 0 @0x80073dbc) */

static int g_fail = 0;

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

static void tick(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    scd_vm_tick();
    re15_aot_scan(pl->x, pl->z, 0);
    if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
    re15_actor_step_all_walkers();
    re15_actors_anim_advance();
    re15_enemy_ai_run_all(0);
    re15_aot_stamp_entities();      /* Frame-Ende wie re15_game_step (@0x8001ce1c) */
    re15_object_notch_update();
    g_aot_action_pressed = 0;       /* press-edge gilt nur 1 Bild */
    g_scd_pad_edge = 0;
}

static void dump_doors(const char *tag)
{
    printf("-- %s: Tür-AOTs --\n", tag);
    for (int s = 0; s < 3; s++) {
        const re15_aot_t *a = &g_aot.slots[s];
        const re15_aot_door_params_t *d = &g_aot.door_params[s];
        printf("   slot%d type=%u sceflg=0x%02x rect c=(%ld,%ld) half=(%ld,%ld) "
               "dest=st%u rm0x%02X cut=%u spawn=(%d,%d,%d)\n",
               s, a->type, a->sce_flags, (long)a->x, (long)a->z,
               (long)a->half_w, (long)a->half_h,
               d->dest_stage, d->dest_room, d->target_cut,
               d->spawn_x, d->spawn_y, d->spawn_z);
    }
}

static int find_cursor_prop(void)
{
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        if (g_scd.props[i].active && g_scd.props[i].obj_id == 0) return i;
    return -1;
}

/* Raum frisch betreten + Panel aktivieren (echter Pfad: Action-Press am Panel-AOT slot3). */
static int enter_and_open_panel(re15_rdt_t *rdt)
{
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1080;
    g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    /* Eintritts-Spawn der Etagen-Türen (1040/10C0/1120 sub00: spawn -13650,-900). */
    pl->x = -13650; pl->y = 0; pl->z = -900; pl->rot_y = 0;
    scd_room_reenter(rdt, pl->x, pl->z, 0);
    for (int t = 0; t < 8; t++) tick();
    /* Vor das Panel: Rect (-12450,-450,800,800) -> Zentrum (-12050,-50).
     * FORWARD-620: fz = z - 620*sin(ry); ry=3072 -> sin=-4096 -> fz=z+620. */
    pl->x = -12050; pl->z = -670; pl->rot_y = 3072;
    g_aot_action_pressed = 1;
    tick();
    tick(); tick();   /* Event-Thread (sub06) startet im Feuer-Bild, LÄUFT aber erst im Folgebild */
    int mode_bits = 0;
    for (int b = 0; b <= 8; b++) mode_bits += re15_game_flag_get(5, (uint8_t)b);
    printf("   Panel-Press: Steuer-Modus-Bits flag(5,0..8) gesetzt = %d/9, cam=%d\n",
           mode_bits, (int)g_scd.cam_id);
    return mode_bits == 9;
}

/* PHYSISCHE Taste N Ticks halten — echter Weg: virtuelles Wort wie re15_game_step
 * (g_scd_pad_held = re15_pad_virtual_word(pad_current), game_step_common.c). */
static void hold(uint16_t phys_mask, int ticks)
{
    for (int t = 0; t < ticks; t++) { g_scd_pad_held = re15_pad_virtual_word(phys_mask); tick(); }
    g_scd_pad_held = 0;
}

/* Eine Etagen-Fahrt end-to-end: Cursor per d-pad auf die Knopf-Zelle, Confirm SQUARE,
 * dann fahren lassen. Pinnt Ziel-Raum + Spawn + Cut gegen die RDT-Soll-Bytes
 * (Door_aot_set-Payload; Warp liest ihn @0x8001d87c/8bc/930 — kein Override). */
static unsigned ride(re15_rdt_t *rdt, const char *name,
                     uint16_t dpad_mask, int dpad_ticks, uint16_t dpad2_mask, int dpad2_ticks,
                     int expect_notch,
                     int exp_sx, int exp_sz, int exp_cut)
{
    printf("== Fahrt: %s ==\n", name);
    if (!enter_and_open_panel(rdt)) { printf("   FAIL: Panel-Modus nicht aktiv\n"); g_fail = 1; return 0; }
    int cp = find_cursor_prop();
    if (cp < 0) { printf("   FAIL: kein Cursor-Prop (obj0)\n"); g_fail = 1; return 0; }
    printf("   Cursor start=(%ld,%ld) notch=%u\n",
           (long)g_scd.props[cp].x, (long)g_scd.props[cp].z, g_scd.props[cp].member_0b);
    if (dpad_ticks)  hold(dpad_mask, dpad_ticks);
    if (dpad2_ticks) hold(dpad2_mask, dpad2_ticks);
    tick();   /* ein Bild ohne Eingabe: Notch-Pass sieht die End-Position */
    printf("   Cursor nach Fahrt=(%ld,%ld) notch=%u (erwartet %d)\n",
           (long)g_scd.props[cp].x, (long)g_scd.props[cp].z,
           g_scd.props[cp].member_0b, expect_notch);
    if ((int)g_scd.props[cp].member_0b != expect_notch) {
        printf("   FAIL: Cursor-Stempel falsch\n"); g_fail = 1;
    }
    /* Confirm: physisch SQUARE (virtuell Bit 6 = 0x0040, Preset 0 @0x80073dbc[6]=raw 0x0080;
     * sub01 prüft `51 01 40 00` = Maske 0x0040). 1 Bild held genügt. */
    hold(RE15_PAD_BIT_SQUARE, 1);
    /* Fahrt-Choreografie laufen lassen: Sleeps ~150 Bilder + Plc_dest-Walk. */
    unsigned dest = 0;
    for (int t = 0; t < 2000; t++) {
        tick();
        if (g_room_change.pending) {
            dest = g_room_change.room_id;
            printf("   ROOM-CHANGE nach %d Ticks: -> 0x%04X spawn=(%ld,%ld,%ld) cut=%d\n",
                   t, dest, (long)g_room_change.x, (long)g_room_change.y,
                   (long)g_room_change.z, g_room_change.target_cut);
            if (g_room_change.x != exp_sx || g_room_change.z != exp_sz ||
                g_room_change.target_cut != exp_cut) {
                printf("   FAIL: Spawn/Cut != RDT-Soll (%d,%d) cut=%d (Door_aot_set-Payload)\n",
                       exp_sx, exp_sz, exp_cut);
                g_fail = 1;
            }
            break;
        }
    }
    if (!dest) {
        printf("   KEIN Raumwechsel nach 2000 Ticks. Threads:");
        for (int i = 0; i < SCD_THREAD_COUNT; i++)
            if (g_scd.threads[i].active) printf(" %d", i);
        printf("  flag(5,0x20)=%d cam=%d\n",
               re15_game_flag_get(5, 0x20), (int)g_scd.cam_id);
        g_fail = 1;
    }
    return dest;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600]; size_t sz = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1080.RDT", base);
    uint8_t *dat = read_file(path, &sz);
    if (!dat) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(dat, sz, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }

    /* ---- Messung 0: Registrierung ---- */
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1080;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -13650; pl->y = 0; pl->z = -900; pl->rot_y = 0;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    dump_doors("nach main00/sub00");
    printf("-- Knopf-AOTs --\n");
    for (int s = 3; s <= 7; s++) {
        const re15_aot_t *a = &g_aot.slots[s];
        printf("   slot%d active=%d type=%u ev=%u sceflg=0x%02x band=0x%02x c=(%ld,%ld) half=(%ld,%ld)\n",
               s, a->active, a->type, a->event_id, a->sce_flags, a->band,
               (long)a->x, (long)a->z, (long)a->half_w, (long)a->half_h);
    }
    { int cp = find_cursor_prop();
      printf("-- Cursor-Prop idx=%d pos=(%ld,%ld) band=%u --\n", cp,
             cp >= 0 ? (long)g_scd.props[cp].x : 0, cp >= 0 ? (long)g_scd.props[cp].z : 0,
             cp >= 0 ? g_scd.props[cp].band : 0); }

    /* 30 Ticks Leerlauf: feuert irgendetwas von selbst? (Null-Rect-Auto-Tür-Verdacht) */
    for (int t = 0; t < 300; t++) {
        tick();
        if (g_room_change.pending) {
            printf("   SPONTANER Raumwechsel bei t=%d -> 0x%04X (OHNE jede Eingabe!)\n",
                   t, g_room_change.room_id);
            g_fail = 1;
            g_room_change.pending = 0;
            break;
        }
    }

    /* ---- Etagen-Fahrten über den ECHTEN UI-Pfad ----
     * Cursor start z=22684. Knopf 4 (1F): z runter (held 0x0004, -200/Bild).
     * Knopf 5 (2F): z rauf (held 0x0001). Knopf 6 (3F): z rauf 17 Bilder.
     * Knopf 7 (Tür): x+1 (held 0x0002) + z runter 25 Bilder. */
    /* Soll-Spawns/Cuts = Door_aot_set-Payload-Bytes der ROOM1080.RDT:
     * slot0 @0x490: 50 aa|00 00|08 d5 .. 05 = (-21936,0,-11000) cut5
     * slot1 @0x4B0: aa 05|00 00|84 1c .. 00 = (1450,0,7300)     cut0
     * slot2 @0x4D0: 14 05|00 00|84 1c .. 00 = (1300,0,7300)     cut0 */
    unsigned d1 = ride(&rdt, "Knopf 1F (slot4 -> ROOM1040)", RE15_PAD_BIT_DOWN, 4,  0, 0, 4,
                       -21936, -11000, 5);
    unsigned d2 = ride(&rdt, "Knopf 2F (slot5 -> ROOM10C0)", RE15_PAD_BIT_UP,   4,  0, 0, 5,
                       1450, 7300, 0);
    unsigned d3 = ride(&rdt, "Knopf 3F (slot6 -> ROOM1120)", RE15_PAD_BIT_UP,   17, 0, 0, 6,
                       1300, 7300, 0);

    printf("\n===== ETAGEN-TABELLE =====\n");
    printf("   1F: Soll 0x1040, Ist 0x%04X %s\n", d1, d1 == 0x1040 ? "OK" : "FAIL");
    printf("   2F: Soll 0x10C0, Ist 0x%04X %s\n", d2, d2 == 0x10C0 ? "OK" : "FAIL");
    printf("   3F: Soll 0x1120, Ist 0x%04X %s\n", d3, d3 == 0x1120 ? "OK" : "FAIL");
    if (d1 != 0x1040 || d2 != 0x10C0 || d3 != 0x1120) g_fail = 1;

    /* ---- Knopf 7 = "Tür öffnen auf aktueller Etage": von 2F betreten ---- */
    printf("\n== Knopf TÜR (slot7) mit Etagen-Flag 2F (z3 b55) ==\n");
    if (enter_and_open_panel(&rdt)) {
        re15_game_flag_set(3, 54, 0);
        re15_game_flag_set(3, 55, 1);   /* 10C0 sub01 hat gesetzt: Aufzug steht 2F */
        re15_game_flag_set(3, 56, 0);
        hold(RE15_PAD_BIT_RIGHT, 1);    /* x +200 (sub04, virt 0x0002) */
        hold(RE15_PAD_BIT_DOWN, 25);    /* z -5000 (sub03, virt 0x0004) */
        tick();
        int cp = find_cursor_prop();
        printf("   Cursor=(%ld,%ld) notch=%u (erwartet 7)\n",
               (long)g_scd.props[cp].x, (long)g_scd.props[cp].z, g_scd.props[cp].member_0b);
        hold(RE15_PAD_BIT_SQUARE, 1);
        unsigned dest = 0;
        for (int t = 0; t < 600; t++) {
            tick();
            if (g_room_change.pending) { dest = g_room_change.room_id; break; }
        }
        printf("   TÜR-Ausstieg: Soll 0x10C0, Ist 0x%04X %s; z3 b55 danach=%d (Soll 0)\n",
               dest, dest == 0x10C0 ? "OK" : "FAIL", re15_game_flag_get(3, 55));
        if (dest != 0x10C0 || re15_game_flag_get(3, 55) != 0) g_fail = 1;
    } else g_fail = 1;

    printf("\n===== BEFUND: %s =====\n", g_fail ? "FAIL" : "OK");
    free(dat);
    return g_fail;
}
