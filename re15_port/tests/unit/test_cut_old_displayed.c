/* test_cut_old_displayed.c — PIN: Cut_old stellt den ANGEZEIGTEN Cut wieder her, und der
 * RVD-Zonen-Scan prueft gegen den ANGEFORDERTEN Cut.
 *
 * NEGATIV-TEST zum Nutzer-Befund ROOM11F0 ("nach dem Generator-Raetsel und direktem
 * Weglaufen wechselt die Kamera teilweise nicht"): beide Faelle treten NUR in dem Bild
 * auf, in dem der RVD-Scan den angeforderten Cut schon geaendert hat, der
 * Praesentations-Apply (re15_cam_present_tick) aber noch nicht gelaufen ist. Genau das
 * passiert beim SCHNELLEN Weglaufen — der Spieler durchquert eine Zone in dem Bild, in
 * dem ein Skript-Cut gesetzt bzw. der Zonen-Scan gefahren wird.
 *
 * BELEGE (selbst disassembliert, ghidra1_V2.txt):
 *
 * T1  Cut_chg merkt den ANGEZEIGTEN Cut (LAB_800402a0):
 *       @0x800402c0 `lbu a1,offset DAT_800b0fe4(a1)`   = work_vars[0x0A] (ANGEZEIGT)
 *       @0x800402e4 `sb  a1,offset DAT_800b3f7b(at)`
 *     und Cut_old liest genau den zurueck (LAB_8004032c):
 *       @0x8004033c `lbu a0,offset DAT_800b3f7b(a0)`
 *       @0x80040364 `sh  a0,offset DAT_800b0fe4(at)`   work_vars[0x0A] := gemerkter Cut
 *       @0x80040368 `jal FUN_800142f4`                 DAT_800afbb5   := gemerkter Cut
 *     DAT_800b3f7b hat game-weit GENAU ZWEI XREFs: 800402e4(W), 8004033c(R).
 *
 * T2  Der RVD-Zonen-Scan prueft gegen den ANGEFORDERTEN Cut (FUN_80014230):
 *       @0x8001423c `lbu v0,-0x44b(v0)=>DAT_800afbb5`  (Gruppen-Eintrittstest)
 *       @0x800142c8 `lbu v0,-0x44b(v0)=>DAT_800afbb5`  (Schleifen-Fortsetzung)
 *     DAT_800b0fe4 kommt im Scan NICHT vor.
 *
 * T3  Der Praesentations-Apply selbst (FUN_8002137c / FUN_80021bbc):
 *       @0x800214f8 `lh v1,DAT_800b0fe4` / @0x80021500 `lbu v0,DAT_800afbb5`
 *       @0x80021508 beq -> gleich = nichts / @0x80021514 `sb 1,DAT_800b5457`
 *       @0x80021bf4 `sh v0,DAT_800b0fe8` / @0x80021bfc `sh v1,DAT_800b0fe4`
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
#include "re15_collision.h"

extern scd_vm_t g_scd;
extern int scd_thread_start(int slot, const uint8_t *pc);

static int g_fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { printf("FAIL: "); printf(__VA_ARGS__); \
                              printf("\n"); g_fail = 1; } } while (0)

int main(void)
{
    re15_actor_init();
    scd_vm_init();
    re15_aot_init();
    g_current_room_id = 0x11F0;
    g_room_change.pending = 0;

    /* ---- T1: Cut_old muss den ANGEZEIGTEN Cut wiederherstellen -------------------
     * Ausgangslage = das Bild NACH einem RVD-Zonenwechsel, VOR dem Apply:
     *   angezeigt (work_vars[0x0A]) = 2, angefordert (cam_id) = 8.
     * Cut_chg(10) muss die 2 merken (@0x800402c0/@0x800402e4), Cut_old muss die 2
     * zurueckgeben (@0x8004033c/@0x80040364). Der Port merkte sich frueher die 8. */
    {
        static const uint8_t bc_chg[] = { 0x29, 0x0A, 0x01, 0x00 };   /* Cut_chg(10); Evt_end */
        static const uint8_t bc_old[] = { 0x2A, 0x00, 0x01, 0x00 };   /* Cut_old; Nop; Evt_end */
        g_scd.work_vars[0x0A]    = 2;      /* DAT_800b0fe4 — ANGEZEIGT */
        g_scd.cam_id             = 8;      /* DAT_800afbb5 — ANGEFORDERT (Scan war schneller) */
        g_scd.cam_change_pending = 1;
        g_scd.cut_auto_enabled   = 1;

        scd_thread_start(3, bc_chg);
        scd_vm_tick();
        CHECK(g_scd.cam_id == 10, "Cut_chg(10) hat cam_id nicht auf 10 gesetzt (%u)",
              (unsigned)g_scd.cam_id);
        CHECK(g_scd.work_vars[0x0A] == 10, "Cut_chg(10) hat work_vars[0x0A] nicht gesetzt (%d)",
              (int)g_scd.work_vars[0x0A]);
        CHECK(g_scd.work_vars[0x0C] == 2,
              "Cut_chg muss work_vars[0x0C] auf den ALTEN ANGEZEIGTEN Cut 2 setzen "
              "(@0x800402d0 lhu / @0x800402ec sh) — ist %d", (int)g_scd.work_vars[0x0C]);
        CHECK(g_scd.cut_auto_enabled == 0, "Cut_chg muss den Auto-Scan abschalten (@0x800402d4)");
        CHECK(g_scd.cam_id_prev == 2,
              "DAT_800b3f7b muss der ANGEZEIGTE Cut 2 sein (@0x800402c0 `lbu a1,DAT_800b0fe4` "
              "+ @0x800402e4 `sb a1,DAT_800b3f7b`) — ist %u", (unsigned)g_scd.cam_id_prev);

        scd_thread_start(4, bc_old);
        scd_vm_tick();
        CHECK(g_scd.cam_id == 2,
              "Cut_old muss auf den ANGEZEIGTEN Cut 2 zurueck (@0x8004033c/@0x80040368) — ist %u",
              (unsigned)g_scd.cam_id);
        CHECK(g_scd.work_vars[0x0A] == 2,
              "Cut_old muss work_vars[0x0A] = 2 setzen (@0x80040364) — ist %d",
              (int)g_scd.work_vars[0x0A]);
        CHECK(g_scd.cut_auto_enabled == 1,
              "Cut_old muss den Auto-Scan wieder scharf schalten (@0x80040378-84)");
    }

    /* ---- T2: der RVD-Zonen-Scan haengt am ANGEFORDERTEN Cut ---------------------
     * Zwei Zonen: eine mit cam_from = ANGEFORDERT, eine mit cam_from = ANGEZEIGT.
     * Das Original testet die Gruppe des ANGEFORDERTEN (@0x8001423c / @0x800142c8);
     * der Port testete die des ANGEZEIGTEN (Parameter `active_cut`). */
    {
        re15_aot_init();
        g_scd.cut_auto_enabled = 1;
        /* Slot 62 (hoch = frueh in RVD-Tabellenreihenfolge): cam_from 5 -> 6 */
        re15_aot_set_cam_switch(62, 0, 0, 1000, 1000, /*from*/5, /*to*/6);
        /* Slot 61: cam_from 3 -> 9 (dieselbe Flaeche) */
        re15_aot_set_cam_switch(61, 0, 0, 1000, 1000, /*from*/3, /*to*/9);

        /* angefordert = 5 (der Scan hat gerade auf 5 geschaltet),
         * angezeigt   = 3 (der Apply kommt erst im naechsten Bild) */
        g_scd.cam_id             = 5;
        g_scd.work_vars[0x0A]    = 3;
        g_scd.cam_change_pending = 0;
        re15_aot_scan(0, 0, /*active_cut (ANGEZEIGT)*/ 3);
        CHECK(g_scd.cam_id == 6,
              "Der RVD-Scan muss die Gruppe des ANGEFORDERTEN Cuts 5 fahren "
              "(@0x8001423c `lbu DAT_800afbb5`) und auf 6 schalten — ist %u",
              (unsigned)g_scd.cam_id);
    }

    /* ---- T3: die Selbstheilung bleibt intakt ------------------------------------ */
    {
        g_scd.work_vars[0x0A]    = 4;
        g_scd.cam_id             = 7;
        g_scd.cam_change_pending = 0;      /* Flag verloren gegangen */
        int applied = re15_cam_present_tick();
        CHECK(applied == 1, "Selbstheilung @0x800214f4-514 hat nicht nachgezogen");
        CHECK(g_scd.work_vars[0x0A] == 7, "Apply hat work_vars[0x0A] nicht auf 7 gesetzt (%d)",
              (int)g_scd.work_vars[0x0A]);
        CHECK(g_scd.work_vars[0x0C] == 4, "Apply hat work_vars[0x0C] nicht auf 4 gesetzt (%d)",
              (int)g_scd.work_vars[0x0C]);
        CHECK(re15_cam_present_tick() == 0, "Apply feuert erneut, obwohl beide gleich sind");
    }

    printf(g_fail ? "test_cut_old_displayed: ABWEICHUNG\n" : "test_cut_old_displayed: OK\n");
    return g_fail;
}
