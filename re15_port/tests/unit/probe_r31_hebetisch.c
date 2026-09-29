/* probe_r31_hebetisch.c — RIEGEL Runde 31 / H: Hebetisch in Irons' Buero (ROOM1150/1151).
 *
 * Auftrag (analysis/befunde_runde31/AUFTRAG.md): "packe mir die Granate links in die hochfahrende
 * Box und die Sicherung rechts. Ausserdem starte mit dem Aufnahme Dialog der items erst wenn das
 * Modell wirklich komplett hochgefahren ist." Dossier analysis/befunde_runde31/hebetisch.md.
 *
 * PRUEFUNGEN (je ROOM1150.RDT und ROOM1151.RDT):
 *  Ruhe-Fenster (include/re15_hebetisch.h)
 *    1  das Fenster liegt bei [0x101A,0x1042) (1150) bzw. [0x0FF8,0x1020) (1151); an seinem Beginn
 *       steht Sleep 30 `09 0a 1e 00`, 10 Byte davor der Setzen-For `0d 00 04 00 0a 00`, an seinem
 *       Ende der Abfahrt-For `0d 00 04 00 5a 00`
 *  Sitz: 2..7 entfallen in Runde 32 (Sitz jetzt in den unteren Faechern, Riegel
 *    unit_r32_hebetisch_faecher); die Nummern 8..13 bleiben, damit alte Protokolle lesbar bleiben
 *  Zeitpunkt (Bildschleife in der Reihenfolge des Spiels: SCD-Tick nur ohne offene Aufnahme,
 *  dann Sicherung-, dann Granaten-Tick, dann Aufnahme-Tick):
 *    8  die Sicherungs-Aufnahme geht im ERSTEN Ruhebild auf: y = -1205, im Bild davor -1205,
 *       zwei Bilder davor -1206 (letztes Add_speed des Setzens), sub04-PC = Fensterbeginn + 1
 *    9  die Granaten-Aufnahme geht danach auf, ebenfalls in der Ruhe (y = -1205, PC im Fenster)
 *   10  in JEDEM Bild mit offener Aufnahme steht die Plattform auf -1205
 *   11  Ruhebilder ohne offene Aufnahme = 40 = Sleep 30 @0x101A + Sleep 10 @0x102E: die Dialoge
 *       verbrauchen keinen Skript-Takt, die Abfahrt kommt erst danach
 *   12  Yes/Yes: Flags (9,53)/(9,56), beide Props weg, beide im Inventar
 *  Negativ-Kontrolle
 *   13  ein anderer Raum (ROOM1140) traegt kein Ruhe-Fenster, re15_hebetisch_ruht_oben() = 0
 *
 * Rueckgabe 0 = alles bestanden, sonst die Nummer der ersten gerissenen Pruefung.
 */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_md1.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_skeleton.h"
#include "re15_camera.h"
#include "re15_sicherung.h"
#include "re15_granate.h"
#include "re15_hebetisch.h"
#include "re15_item_modal.h"
#include "re15_inventory.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static int fehler = 0, erste = 0;
static void pruefe(int nr, const char *was, int ok_)
{
    printf("   [%s] %d. %s\n", ok_ ? "OK " : "FEHL", nr, was);
    if (!ok_) { fehler++; if (!erste) erste = nr; }
}

static uint8_t *datei(const char *rel, size_t *n)
{
    char p[700];
    snprintf(p, sizeof p, "%s/%s", RE15_XSTR(RE15_ASSETS_PATH), rel);
    FILE *f = fopen(p, "rb");
    if (!f) { fprintf(stderr, "nicht lesbar: %s\n", p); return NULL; }
    fseek(f, 0, SEEK_END); long s = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)s);
    if (!b || fread(b, 1, (size_t)s, f) != (size_t)s) { fclose(f); free(b); return NULL; }
    fclose(f); *n = (size_t)s; return b;
}

/* ------------------------------------------------------------ Raum ------------------------ */
static void raum_frisch(re15_rdt_t *rdt, uint16_t rid)
{
    re15_game_state_init();
    re15_inv_init();
    scd_vm_init(); re15_actor_init();
    g_current_room_id = rid;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -21000; pl->y = 0; pl->z = -18500; pl->rot_y = 2048; pl->state = 1;
    g_scd.player_mode = 0;
    { extern void re15_msg_load_room_block(const uint8_t *b, int n);
      re15_msg_load_room_block(rdt->messages, rdt->messages_size); }
    scd_register_room_events(rdt);
    scd_room_reenter(rdt, -21000, -18500, 0);
}

static void fenster_pruefen(const re15_rdt_t *rdt, uint16_t rid)
{
    long von = re15_hebetisch_fenster_von(), bis = re15_hebetisch_fenster_bis();
    long soll_von = rid == 0x1150 ? 0x101A : 0x0FF8, soll_bis = rid == 0x1150 ? 0x1042 : 0x1020;
    static const uint8_t sleep30[4] = { 0x09, 0x0a, 0x1e, 0x00 };
    static const uint8_t setzen[6]  = { 0x0d, 0x00, 0x04, 0x00, 0x0a, 0x00 };
    static const uint8_t abfahrt[6] = { 0x0d, 0x00, 0x04, 0x00, 0x5a, 0x00 };
    int ok = von == soll_von && bis == soll_bis && bis + 6 <= rdt->raw_size &&
             memcmp(rdt->raw + von, sleep30, 4) == 0 && memcmp(rdt->raw + von - 10, setzen, 6) == 0 &&
             memcmp(rdt->raw + bis, abfahrt, 6) == 0;
    printf("   Ruhe-Fenster [0x%04lX, 0x%04lX)\n", von, bis);
    pruefe(1, "Ruhe-Fenster = [Sleep 30, For-Abfahrt), 10 Byte davor der Setzen-For", ok);
}

/* ------------------------------------------------------------ Sitz ------------------------ */
/* Runde 32: die Pruefungen 2..7 (Sitz OBEN in der Kuppel: Kuppelboden -1036, Luft unter der
 * geschlossenen Kuppel / den offenen Deckeln, Achteck, Granate <-> Sicherung, links/rechts)
 * entfallen. Der Nutzer will beide UNTEN in den Faechern ("liegt aktuell oben drauf. Aber sie
 * sollen unten, in den hochfahrenden Fach liegen - ein item links ein item rechts",
 * analysis/befunde_runde32/AUFTRAG.md). Sitz, Durchstoss, Sichtbarkeit und links/rechts haelt
 * jetzt unit_r32_hebetisch_faecher fest (tests/unit/probe_r32_hebetisch_faecher.c). */

/* ------------------------------------------------------------ Zeitpunkt ------------------- */
static int slot_von_obj(uint8_t o)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++) if (g_scd.props[k].obj_id == o) return k;
    return -1;
}
static int menge(uint8_t id)
{
    int n = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) if (g_inv.slots[i].id == id) n += g_inv.slots[i].qty;
    return n;
}

static void zeit_pruefen(re15_rdt_t *rdt, uint16_t rid)
{
    printf("\n== %04X Zeitpunkt der Aufnahmen (Yes/Yes) ==\n", rid);
    raum_frisch(rdt, rid);
    long von = re15_hebetisch_fenster_von();
    long y[1400]; memset(y, 0, sizeof y);
    int f_si = -1, f_gr = -1, pc_si = -1, pc_gr = -1, modal_falsch = 0, modal_bilder = 0;
    int ruhe_ohne = 0, abfahrt = -1, aktuell = 0;
    scd_event_fire(4);
    for (int f = 0; f < 1400; f++) {
        int skript = !re15_item_modal_active();
        if (skript) scd_vm_tick();                                   /* main.c Freeze-Zweig */
        int p = slot_von_obj(0);
        y[f] = p >= 0 ? (long)g_scd.props[p].y : 0;
        if (skript && re15_hebetisch_ruht_oben()) ruhe_ohne++;
        if (re15_sicherung_tick()) { f_si = f; pc_si = (int)re15_hebetisch_ruhe_pc_off(); aktuell = 1; }
        if (re15_granate_tick())   { f_gr = f; pc_gr = (int)re15_hebetisch_ruhe_pc_off(); aktuell = 2; }
        if (re15_item_modal_active()) {
            modal_bilder++;
            if (y[f] != -1205) modal_falsch++;
            uint16_t edge = 0; uint8_t typ = 0; int wahl = 0;
            int art = re15_item_modal_prompt(&typ, &wahl);
            if (re15_item_modal_prompt_ready() && art >= 1) edge = 0x4000;   /* Yes */
            re15_item_modal_tick(edge, edge);
        }
        if (f_gr >= 0 && abfahrt < 0 && !re15_item_modal_active() && y[f] > -1205 && y[f] < -300) abfahrt = f;
        if (abfahrt >= 0 && f > abfahrt + 5) break;
    }
    (void)aktuell;
    printf("   MESS Sicherung Bild %d (y %ld, davor %ld, %ld) PC 0x%04X | Granate Bild %d (y %ld) PC 0x%04X | "
           "Bilder mit Aufnahme %d, davon y != -1205: %d | Ruhebilder ohne Aufnahme %d | Abfahrt Bild %d\n",
           f_si, f_si >= 0 ? y[f_si] : 0, f_si >= 1 ? y[f_si - 1] : 0, f_si >= 2 ? y[f_si - 2] : 0, pc_si,
           f_gr, f_gr >= 0 ? y[f_gr] : 0, pc_gr, modal_bilder, modal_falsch, ruhe_ohne, abfahrt);
    pruefe(8, "Sicherungs-Aufnahme im ERSTEN Ruhebild (-1206 -> -1205 -> -1205, PC = Sleep 30 + 1)",
           f_si >= 2 && y[f_si] == -1205 && y[f_si - 1] == -1205 && y[f_si - 2] == -1206 && pc_si == von + 1);
    pruefe(9, "Granaten-Aufnahme danach, ebenfalls in der Ruhe (y -1205, PC im Fenster)",
           f_gr > f_si && y[f_gr] == -1205 && pc_gr >= von && pc_gr < re15_hebetisch_fenster_bis());
    pruefe(10, "in JEDEM Bild mit offener Aufnahme steht die Plattform auf -1205",
           modal_bilder > 0 && modal_falsch == 0);
    pruefe(11, "40 Ruhebilder ohne Aufnahme (Sleep 30 @0x101A + Sleep 10 @0x102E), erst dann die Abfahrt",
           ruhe_ohne == 40 && abfahrt > f_gr);
    pruefe(12, "Yes/Yes: beide Flags, beide Props weg, beide im Inventar",
           re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT) && re15_game_flag_get(9, RE15_GRANATE_TAKEN_BIT) &&
           !g_scd.props[slot_von_obj(RE15_SICHERUNG_OBJ_ID)].active &&
           !g_scd.props[slot_von_obj(RE15_GRANATE_OBJ_ID)].active &&
           menge(RE15_SICHERUNG_ITEM) == 1 && menge(RE15_GRANATE_ITEM) == RE15_GRANATE_MENGE);
}

static int ein_raum(uint16_t rid, const char *rel)
{
    size_t n = 0;
    uint8_t *buf = datei(rel, &n);
    if (!buf) return 77;
    static re15_rdt_t rdt;
    memset(&rdt, 0, sizeof rdt);
    if (re15_rdt_parse(buf, n, &rdt) != 0) { free(buf); return 1; }
    printf("\n== %04X Ruhe-Fenster ==\n", rid);
    raum_frisch(&rdt, rid);
    fenster_pruefen(&rdt, rid);
    zeit_pruefen(&rdt, rid);
    free(buf);
    return 0;
}

int main(void)
{
    if (ein_raum(0x1150, "STAGE1/ROOM1150.RDT") == 77) return 77;
    if (ein_raum(0x1151, "STAGE1/ROOM1151.RDT") == 77) return 77;
    {   /* Negativ-Kontrolle: ein anderer Raum traegt kein Ruhe-Fenster */
        size_t n = 0;
        uint8_t *buf = datei("STAGE1/ROOM1140.RDT", &n);
        if (!buf) return 77;
        static re15_rdt_t rdt;
        memset(&rdt, 0, sizeof rdt);
        int ok = re15_rdt_parse(buf, n, &rdt) == 0;
        if (ok) { raum_frisch(&rdt, 0x1140); for (int f = 0; f < 60; f++) scd_vm_tick(); }
        printf("\n== 1140 Negativ-Kontrolle: Fenster %ld..%ld, ruht_oben %d ==\n",
               re15_hebetisch_fenster_von(), re15_hebetisch_fenster_bis(), re15_hebetisch_ruht_oben());
        pruefe(13, "ROOM1140: kein Ruhe-Fenster, ruht_oben = 0",
               ok && re15_hebetisch_fenster_von() < 0 && re15_hebetisch_fenster_bis() < 0 && !re15_hebetisch_ruht_oben());
        free(buf);
    }
    printf("\n%s — %d Pruefung(en) gerissen\n", fehler ? "FEHLGESCHLAGEN" : "ALLES BESTANDEN", fehler);
    return fehler ? erste : 0;
}
