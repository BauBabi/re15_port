/* test_r35_inventar1050.c — RIEGEL Runde 35 Spur E, Punkt 2: Kampfmesser raus aus dem Inventar, immer
 * Rueckfall ohne Waffe. Dossier analysis/befunde_runde35/E_inventar1050.md, Konstanten und Belege
 * include/re15_messer.h + engine/src/messer_rueckfall.c.
 *
 * Teile (je ein ctest-Eintrag, registriert in probes/r35_inventar1050.cmake):
 *   start       Neues Spiel Leon: Starttabelle @0x80074bb8 ohne Messer -> Platz 0 BROWNING x15, Platz 1
 *               H.GUN BULLETS x50, kein Id 1, 25c8 = 0x80, 25c9 = 0x80, Waffen-Id 1 (Messer, Nahkampf).
 *   elza        Neues Spiel Elza (Charakter 4, Weiche @0x80045e28): Tabelle @0x80074bc4 = nur Messer ->
 *               leeres Inventar, Waffe 1.
 *   ablegen     Pistole ausruesten -> Waffe 3; im Statusschirm ablegen (25c8 := 0x80) -> Commit beim
 *               Schliessen @0x80046668/@0x8004666c -> Waffe 1; wieder ausruesten -> Waffe 3.
 *   altstand    Spielstand mit dem ORIGINAL-Briefing (Messer Platz 0, ausgeruestet) laden: Messer weg,
 *               Pistole Platz 0, Munition Platz 1, 25c8 = 0x80, Waffe 1.
 *   altpistole  Alter Stand mit ausgeruesteter Pistole (Platz 1): danach Pistole Platz 0, 25c8 = 0, Waffe 3.
 *   kiste       Alter Stand mit Messer in der Kiste: nach dem Laden ist es dort weg.
 *   breit       Breite Waffe 0x13 aufnehmen ohne Ausruestung: 25c8 bleibt 0x80 (RE2 @0x8006999c), mit
 *               Pistole in Platz 0 wandert 25c8 mit (+2 @0x8004dc94).
 *   reserve     Munition in Platz 0 wird als Reserve erkannt (RE2 FUN_8006a23c @0x8006a2a0) und nachgeladen.
 *   karte <p>   (Werkzeug, kein Test) schreibt eine Speicherkarte mit altem Stand (Messer Platz 0) in
 *               ROOM1000 fuer integration_r35_inventar1050.
 */
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_damage.h"
#include "re15_gameflow.h"
#include "re15_inventory.h"
#include "re15_itembox.h"
#include "re15_memcard.h"
#include "re15_menu.h"
#include "re15_messer.h"
#include "re15_savedata.h"
#include "re15_scd.h"
#include "re15_room.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static void inv_zeigen(const char *was)
{
    printf("  %s:", was);
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
        if (g_inv.slots[i].id) printf(" [%d]%02x x%u f%u", i, g_inv.slots[i].id, g_inv.slots[i].qty,
                                      g_inv.slots[i].flags);
    printf(" | 25c8=0x%02x 25c9=0x%02x Waffe %d\n", (unsigned)re15_inv_equipped_slot(),
           (unsigned)re15_inv_prev_equip_slot(), re15_player_equipped_weapon());
}

static void grund(int charakter)
{
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_itembox_init();
    g_gameflow.character = charakter;
    g_current_room_id = 0x1000;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 22230; pl->y = 0; pl->z = -13400; pl->rot_y = 0;
}

static int kein_messer(void) { return re15_inv_find_item(RE15_MESSER_ID) < 0; }

static void teil_start(void)
{
    printf("[start] Leon, Charakter 0\n");
    grund(0);
    re15_inv_load_briefing();                 /* Vorzustand = alte Startausruestung (Messer Platz 0)  */
    re15_messer_startinventar();
    inv_zeigen("Startinventar");
    PRUEF(kein_messer(), "kein Messer (Id 1) im Inventar");
    PRUEF(g_inv.slots[0].id == 0x03 && g_inv.slots[0].qty == 15 && g_inv.slots[1].id == 0x15 &&
          g_inv.slots[1].qty == 50 && g_inv.slots[2].id == 0,
          "Starttabelle @0x80074bb8/@0x80074bd0 ohne Messer: BROWNING x15, H.GUN BULLETS x50");
    PRUEF(re15_inv_equipped_slot() == RE15_MESSER_NICHTS && re15_inv_prev_equip_slot() == 0x80,
          "25c8 = 0x80 (nichts), 25c9 = 0x80 (@0x80045fe0)");
    PRUEF(re15_player_equipped_weapon() == RE15_MESSER_ID && re15_player_equipped_weapon() < 3,
          "Waffen-Id 1 = Messer (Commit 0x80 -> 1 @0x8004666c), Nahkampfklasse < 3");
    /* Statusschirm auf/zu ohne Aenderung: der Commit laesst das Messer stehen */
    re15_menu_toggle(); re15_menu_toggle();
    PRUEF(re15_player_equipped_weapon() == RE15_MESSER_ID && re15_inv_equipped_slot() == 0x80,
          "Statusschirm auf/zu: Waffe bleibt 1, 25c8 bleibt 0x80");
    PRUEF(re15_ammo_reserve_slot() == -1, "Messer hat keine Reserve (-1)");
}

static void teil_elza(void)
{
    printf("[elza] Charakter 4\n");
    grund(4);
    re15_messer_startinventar();
    inv_zeigen("Startinventar");
    int leer = 1;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) if (g_inv.slots[i].id) leer = 0;
    PRUEF(leer, "Elza-Tabelle @0x80074bc4 = nur Messer -> Inventar leer");
    PRUEF(re15_inv_equipped_slot() == 0x80 && re15_player_equipped_weapon() == RE15_MESSER_ID,
          "25c8 0x80, Waffe 1 (Messer-Rueckfall)");
}

static void teil_ablegen(void)
{
    printf("[ablegen] Pistole an, ablegen, wieder an\n");
    grund(0);
    re15_messer_startinventar();
    re15_menu_toggle();                       /* auf: Schnappschuss 25ce */
    re15_inv_set_equipped_slot(0);            /* EQUIP-Commit 25c8 := 25bd @0x8004abd0 (Pistole Platz 0) */
    re15_menu_toggle();                       /* zu: Commit @0x80046654-88 */
    inv_zeigen("ausgeruestet");
    PRUEF(re15_player_equipped_weapon() == 0x03, "Pistole ausgeruestet -> Waffe 3");
    re15_menu_toggle();
    re15_inv_set_equipped_slot(0x80);         /* UNEQUIP 25c8 := 0x80 (menu_common.c state5, @0x8004aaec) */
    re15_menu_toggle();
    inv_zeigen("abgelegt");
    PRUEF(re15_player_equipped_weapon() == RE15_MESSER_ID && re15_inv_equipped_slot() == 0x80,
          "abgelegt -> Commit 0x80 -> Waffe 1 (Messer) @0x80046668/@0x8004666c");
    PRUEF(kein_messer(), "Messer dabei NICHT im Inventar");
    re15_menu_toggle();
    re15_inv_set_equipped_slot(0);
    re15_menu_toggle();
    PRUEF(re15_player_equipped_weapon() == 0x03, "wieder ausgeruestet -> Waffe 3");
}

/* Alter Stand: das ORIGINAL-Briefing (Messer Platz 0) = re15_inv_load_briefing. */
static re15_savedata_t alt_stand(int pistole)
{
    grund(0);
    re15_inv_load_briefing();
    re15_inv_set_equipped_slot(0);
    re15_player_set_equipped_weapon(pistole ? 0x03 : 0x01);
    if (!pistole) re15_inv_set_equipped_slot(0);   /* Messer in Platz 0 ausgeruestet (Savestate 25c8 = 00) */
    inv_zeigen("alter Stand");
    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    return sd;
}

static void laden(const re15_savedata_t *sd)
{
    grund(0);
    re15_messer_startinventar();              /* wie der Programmstart vor CONTINUE */
    uint16_t raum = 0;
    re15_savedata_restore(sd, &raum);
}

static void teil_altstand(void)
{
    printf("[altstand] Messer Platz 0 ausgeruestet\n");
    re15_savedata_t sd = alt_stand(0);
    PRUEF(sd.inv[0].id == 0x01 && sd.equipped_slot == 0 && sd.weapon_id == 1,
          "Spielstand traegt das Messer in Platz 0, 25c8 = 0, Waffe 1");
    laden(&sd);
    inv_zeigen("geladen");
    PRUEF(kein_messer(), "nach dem Laden kein Messer im Inventar");
    PRUEF(g_inv.slots[0].id == 0x03 && g_inv.slots[1].id == 0x15 && g_inv.slots[2].id == 0,
          "verdichtet: Pistole Platz 0, Munition Platz 1 (FUN_8004dadc)");
    PRUEF(re15_inv_equipped_slot() == 0x80 && re15_player_equipped_weapon() == 1,
          "25c8 = 0x80, Waffe 1 = Messer-Rueckfall");
}

static void teil_altpistole(void)
{
    printf("[altpistole] alter Stand, Pistole Platz 1 ausgeruestet\n");
    re15_savedata_t sd = alt_stand(1);
    PRUEF(sd.equipped_slot == 1 && sd.weapon_id == 3, "Spielstand: 25c8 = 1, Waffe 3");
    laden(&sd);
    inv_zeigen("geladen");
    PRUEF(kein_messer() && g_inv.slots[0].id == 0x03, "Messer weg, Pistole rutscht auf Platz 0");
    PRUEF(re15_inv_equipped_slot() == 0 && re15_player_equipped_weapon() == 3,
          "25c8 zieht mit (1 -> 0, @0x8004dbb4-c8), Waffe bleibt 3");
}

static void teil_kiste(void)
{
    printf("[kiste] alter Stand mit Messer in der Kiste\n");
    grund(0);
    re15_inv_load_briefing();
    re15_inv_remove_slot(0);                  /* Messer aus dem Inventar ... */
    re15_inv_set_equipped_slot(0x80);
    re15_inv_slot_t box[RE15_BOX_SLOTS];
    re15_itembox_export(box);
    box[5].id = 0x01; box[5].qty = 0;         /* ... in die Kiste gelegt */
    re15_itembox_import(box);
    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    laden(&sd);
    re15_itembox_export(box);
    int in_kiste = 0;
    for (int i = 0; i < RE15_BOX_SLOTS; i++) if (box[i].id == 0x01) in_kiste++;
    PRUEF(in_kiste == 0, "kein Messer mehr in der Kiste (%d)", in_kiste);
    PRUEF(kein_messer() && re15_player_equipped_weapon() == 1, "Inventar ohne Messer, Waffe 1");
}

static void teil_breit(void)
{
    printf("[breit] breite Waffe 0x13 (ROOM1110 @0x00B5A) aufnehmen\n");
    grund(0);
    re15_messer_startinventar();
    re15_inv_grant(0x13, 100);
    inv_zeigen("ohne Ausruestung");
    PRUEF(re15_inv_equipped_slot() == 0x80, "25c8 bleibt 0x80 (RE2 FUN_800698b4 @0x8006999c), nicht 0x82");
    PRUEF(g_inv.slots[0].id == 0x13 && g_inv.slots[0].flags == 1 && g_inv.slots[1].flags == 2 &&
          g_inv.slots[2].id == 0x03, "Zwei-Zellen-Vorschub wie FUN_8004dc4c (Pistole jetzt Platz 2)");
    grund(0);
    re15_messer_startinventar();
    re15_player_set_equipped_weapon(0x03);   /* Pistole Platz 0 */
    re15_inv_grant(0x13, 100);
    inv_zeigen("mit Pistole");
    PRUEF(re15_inv_equipped_slot() == 2 && g_inv.slots[2].id == 0x03,
          "25c8 wandert mit der Pistole (+2 @0x8004dc94)");
}

static void teil_reserve(void)
{
    printf("[reserve] Munition in Platz 0\n");
    grund(0);
    re15_inv_init();
    g_inv.slots[0].id = 0x15; g_inv.slots[0].qty = 50;
    g_inv.slots[1].id = 0x03; g_inv.slots[1].qty = 0;
    re15_player_set_equipped_weapon(0x03);
    PRUEF(re15_inv_equipped_slot() == 1, "Pistole Platz 1 ausgeruestet");
    PRUEF(re15_ammo_reserve_slot() == 0, "Reserve in Platz 0 erkannt (RE2 @0x8006a294 nor / @0x8006a2a0 srl 31)");
    re15_ammo_reload_exec();
    PRUEF(g_inv.slots[1].qty == 15 && g_inv.slots[0].qty == 35, "nachgeladen: Magazin 15, Rest 35 (Chunk 15 @0x80074da8)");
}

static int karte(const char *pfad)
{
    re15_savedata_t sd = alt_stand(0);
    if (re15_memcard_save(pfad, 0, &sd, "LEON  ALT") != 0) { printf("FAIL: Karte %s\n", pfad); return 1; }
    re15_savedata_t back;
    if (re15_memcard_load(pfad, 0, &back) != 0 || back.inv[0].id != 0x01) {
        printf("FAIL: Ruecklesen\n"); return 1;
    }
    printf("Karte %s: ROOM%04X, Platz 0 = %02x, 25c8 = %u, Waffe %u\n", pfad, (unsigned)back.room,
           back.inv[0].id, back.equipped_slot, back.weapon_id);
    return 0;
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "start";
    if (strcmp(teil, "karte") == 0) return karte(argc > 2 ? argv[2] : "re15_card.mcr");
    if      (strcmp(teil, "start") == 0)      teil_start();
    else if (strcmp(teil, "elza") == 0)       teil_elza();
    else if (strcmp(teil, "ablegen") == 0)    teil_ablegen();
    else if (strcmp(teil, "altstand") == 0)   teil_altstand();
    else if (strcmp(teil, "altpistole") == 0) teil_altpistole();
    else if (strcmp(teil, "kiste") == 0)      teil_kiste();
    else if (strcmp(teil, "breit") == 0)      teil_breit();
    else if (strcmp(teil, "reserve") == 0)    teil_reserve();
    else { printf("unbekannter Teil '%s'\n", teil); return 2; }
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
