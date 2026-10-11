/* test_r36_inventar.c — Runde 36 Spur I "inventar" (Dossier analysis/befunde_runde36/I_inventar.md).
 *
 * Nutzer (AUFTRAG.md Z.28/29): "Mit Nutzung von Memory Cards beim Speichern soll sich die Anzahl
 * reduzieren. Sind Memory Cards oder Handgranaten 0 sollen sie aus dem Player inventory verschwinden"
 * und "Bei den Debug Menu ... schickst du, wenn man mit Elza spielt Elza in Leons Raeume. Wenn man mit
 * Elza spielt, sind die Raeume mit '1' hinten korrekt, zum Beispiel ROOM 1071."
 *
 * Gemessen wird die MECHANIK:
 *   (A) letzte Memory Card: Block-Abzug (RE2 MEM_CARD.BIN @0x801c11ac/b8, bei 0 @0x801c11d0/dc +
 *       verdichten @0x801c11e0) == Live-Abzug (re15_inv_remove_slot = @0x8004aee4-af2c + FUN_8004dadc),
 *       Platz fuer Platz, Ausruest-Platz zieht mit, Pruefsumme stimmt.
 *   (B) Memory Card x3 -> x2, Platz bleibt.
 *   (C) ohne Memory Card: Block und Inventar unveraendert.
 *   (D) leere ausgeruestete Handgranate: Tick entfernt den Platz, legt ab (0x80, @0x8004ad44-4c) und
 *       setzt die Waffe nach der Commit-Regel auf das Messer (@0x80046654-88).
 *   (E) leere NICHT ausgeruestete Saeure-Granate + leere Memory Card: beide weg, der ausgeruestete
 *       Browning wandert mit (@0x8004dbac-c8).
 *   (F) Granate noch im Effekt-Pool (CORE00.ESP, re15_esp_granate_spawn): Platz faellt sofort, die
 *       Waffen-Id wechselt erst, wenn der Granatenplatz weg ist.
 *   (G) volle Granate bleibt, Waffe bleibt.
 *   (H) Debug-Menue: fuer ALLE belegten Saetze der JUMP-Tabelle (DEBUG.BIN @0x263a) landet der Sprung
 *       ueber re15_debug_menu_tick (Quadrat-Flanke, derselbe Pfad wie der Nutzer) bei Leon auf
 *       ROOMxxx0, bei Elza auf ROOMxxx1 (@0x800397e4 srl a0,a0,31 / @0x800397ec addu), und die Datei
 *       existiert.
 */
#include "re15_inventar_r36.h"
#include "re15_inventory.h"
#include "re15_savedata.h"
#include "re15_damage.h"
#include "re15_esp.h"
#include "re15_debug_menu.h"
#include "re15_gameflow.h"
#include "re15_messer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int s_fail = 0;
#define CHECK(c, ...) do { if (!(c)) { s_fail = 1; printf("  FAIL: " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *datei(const char *rel, long *n_out)
{
    char pfad[600];
    snprintf(pfad, sizeof pfad, "%s/%s", RE15_ASSET_PSX_DIR, rel);
    FILE *fp = fopen(pfad, "rb");
    if (!fp) return NULL;
    fseek(fp, 0, SEEK_END); long n = ftell(fp); fseek(fp, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)n);
    if (!b || fread(b, 1, (size_t)n, fp) != (size_t)n) { fclose(fp); free(b); return NULL; }
    fclose(fp);
    if (n_out) *n_out = n;
    return b;
}

static int existiert(const char *rel)
{
    char pfad[600];
    snprintf(pfad, sizeof pfad, "%s/%s", RE15_ASSET_PSX_DIR, rel);
    FILE *fp = fopen(pfad, "rb");
    if (!fp) return 0;
    fclose(fp);
    return 1;
}

static void platz(int i, uint8_t id, uint8_t qty)
{
    g_inv.slots[i].id = id; g_inv.slots[i].qty = qty; g_inv.slots[i].flags = 0; g_inv.slots[i].pad = 0;
}

static void druck(const char *was, const re15_inv_slot_t *s, int eq)
{
    printf("    %-10s", was);
    for (int i = 0; i < 5; i++) printf(" %02x:%u", (unsigned)s[i].id, (unsigned)s[i].qty);
    printf("  25c8=0x%02x\n", (unsigned)eq);
}

/* ---- (A)/(B)/(C) Speichern ---------------------------------------------------------------- */
static void teil_speichern(void)
{
    /* (A) letzte Karte in Platz 0, Browning in Platz 1 AUSGERUESTET, Patronen Platz 2 */
    re15_inv_init();
    platz(0, RE15_R36_KARTE_ID, 1);
    platz(1, 0x03, 15);
    platz(2, 0x15, 50);
    re15_inv_set_equipped_slot(1);
    re15_player_set_equipped_weapon(0x03);
    CHECK(re15_inv_equipped_slot() == 1, "(A) Vorbedingung: Browning nicht in Platz 1 ausgeruestet");
    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 0);
    int k = re15_inventar_r36_karte_im_block(&sd);
    CHECK(k == 0, "(A) Kartenplatz im Block %d, Soll 0", k);
    CHECK(sd.checksum == re15_savedata_checksum(&sd), "(A) Pruefsumme des Blocks nicht neu gerechnet");
    CHECK(sd.inv[0].id == 0x03 && sd.inv[0].qty == 15, "(A) Block Platz 0 = %02x:%u, Soll Browning 03:15",
          sd.inv[0].id, sd.inv[0].qty);
    CHECK(sd.inv[1].id == 0x15 && sd.inv[1].qty == 50, "(A) Block Platz 1 = %02x:%u, Soll Patronen 15:50",
          sd.inv[1].id, sd.inv[1].qty);
    CHECK(sd.inv[2].id == 0 && sd.inv[2].qty == 0, "(A) Block Platz 2 nicht leer");
    CHECK(sd.equipped_slot == 0, "(A) Block-Ausruestplatz %u, Soll 0 (zieht mit, @0x8004dbac-c8)",
          (unsigned)sd.equipped_slot);
    CHECK(re15_inv_find_item(RE15_R36_KARTE_ID) == 0, "(A) Live-Inventar VOR dem Erfolg schon veraendert");
    int rest = re15_inventar_r36_karte_verbrauchen();
    CHECK(rest == 0, "(A) Restmenge %d, Soll 0", rest);
    CHECK(memcmp(sd.inv, g_inv.slots, sizeof sd.inv) == 0, "(A) Block != Live-Inventar nach dem Abzug");
    CHECK(re15_inv_equipped_slot() == (int)sd.equipped_slot, "(A) Ausruestplatz live %d != Block %u",
          re15_inv_equipped_slot(), (unsigned)sd.equipped_slot);
    CHECK(re15_inv_find_item(RE15_R36_KARTE_ID) < 0, "(A) Memory Card noch im Inventar");
    druck("(A) Block", sd.inv, sd.equipped_slot);
    druck("(A) live", g_inv.slots, re15_inv_equipped_slot());

    /* (B) drei Karten in Platz 2 -> zwei, Platz bleibt */
    re15_inv_init();
    platz(0, 0x03, 15);
    platz(1, 0x15, 50);
    platz(2, RE15_R36_KARTE_ID, 3);
    re15_inv_set_equipped_slot(0);
    re15_player_set_equipped_weapon(0x03);
    re15_savedata_capture(&sd, 0, 0);
    k = re15_inventar_r36_karte_im_block(&sd);
    rest = re15_inventar_r36_karte_verbrauchen();
    CHECK(k == 2 && rest == 2, "(B) Platz %d Rest %d, Soll 2/2", k, rest);
    CHECK(sd.inv[2].id == RE15_R36_KARTE_ID && sd.inv[2].qty == 2, "(B) Block Platz 2 = %02x:%u, Soll 21:2",
          sd.inv[2].id, sd.inv[2].qty);
    CHECK(memcmp(sd.inv, g_inv.slots, sizeof sd.inv) == 0, "(B) Block != Live-Inventar");
    CHECK(sd.checksum == re15_savedata_checksum(&sd), "(B) Pruefsumme");

    /* (C) ohne Karte: nichts */
    re15_inv_init();
    platz(0, 0x03, 15);
    re15_inv_set_equipped_slot(0);
    re15_savedata_capture(&sd, 0, 0);
    re15_savedata_t vorher = sd;
    re15_inventory_t inv_vorher = g_inv;
    k = re15_inventar_r36_karte_im_block(&sd);
    rest = re15_inventar_r36_karte_verbrauchen();
    CHECK(k == -1 && rest == -1, "(C) ohne Karte: Platz %d Rest %d, Soll -1/-1", k, rest);
    CHECK(memcmp(&vorher, &sd, sizeof sd) == 0, "(C) Block ohne Karte veraendert");
    CHECK(memcmp(&inv_vorher, &g_inv, sizeof g_inv) == 0, "(C) Inventar ohne Karte veraendert");
    if (!s_fail) printf("  (A)-(C) Memory Card: Abzug Block == live, letzte Karte weg + verdichtet, x3 -> x2, ohne Karte nichts\n");
}

/* ---- (D)-(G) leere Plaetze ------------------------------------------------------------------- */
static re15_esp_t s_core;

static void teil_leer(void)
{
    /* (D) leere Handgranate ausgeruestet */
    re15_esp_fx_reset();
    re15_inv_init();
    platz(0, 0x03, 15);
    platz(1, 0x15, 50);
    platz(2, 0x09, 0);
    platz(3, 0x24, 1);
    re15_inv_set_equipped_slot(2);
    re15_player_set_equipped_weapon(0x09);
    CHECK(re15_inv_equipped_slot() == 2, "(D) Vorbedingung");
    int weg = re15_inventar_r36_tick();
    CHECK(weg == 1, "(D) %d Plaetze entfernt, Soll 1", weg);
    CHECK(re15_inv_find_item(0x09) < 0, "(D) leere Handgranate noch im Inventar");
    CHECK(g_inv.slots[2].id == 0x24 && g_inv.slots[3].id == 0, "(D) nicht verdichtet: Platz 2 = %02x, 3 = %02x",
          g_inv.slots[2].id, g_inv.slots[3].id);
    CHECK(re15_inv_equipped_slot() == RE15_MESSER_NICHTS, "(D) Ausruestplatz 0x%02x, Soll 0x80 (@0x8004ad44-4c)",
          (unsigned)re15_inv_equipped_slot());
    CHECK(re15_player_equipped_weapon() == RE15_MESSER_ID, "(D) Waffe %d, Soll 1 = Messer (@0x8004666c)",
          re15_player_equipped_weapon());
    druck("(D) danach", g_inv.slots, re15_inv_equipped_slot());
    weg = re15_inventar_r36_tick();
    CHECK(weg == 0 && re15_player_equipped_weapon() == RE15_MESSER_ID, "(D) zweiter Tick aendert noch etwas");

    /* (E) leere Saeure-Granate + leere Karte, Browning ausgeruestet dahinter */
    re15_inv_init();
    platz(0, 0x0A, 0);
    platz(1, RE15_R36_KARTE_ID, 0);
    platz(2, 0x0B, 4);
    platz(3, 0x03, 15);
    re15_inv_set_equipped_slot(3);
    re15_player_set_equipped_weapon(0x03);
    weg = re15_inventar_r36_tick();
    CHECK(weg == 2, "(E) %d Plaetze entfernt, Soll 2", weg);
    CHECK(g_inv.slots[0].id == 0x0B && g_inv.slots[0].qty == 4, "(E) Platz 0 = %02x:%u, Soll Brandgranate 0b:4",
          g_inv.slots[0].id, g_inv.slots[0].qty);
    CHECK(g_inv.slots[1].id == 0x03, "(E) Platz 1 = %02x, Soll Browning", g_inv.slots[1].id);
    CHECK(re15_inv_equipped_slot() == 1, "(E) Ausruestplatz %d, Soll 1 (zieht zweimal mit)", re15_inv_equipped_slot());
    CHECK(re15_player_equipped_weapon() == 0x03, "(E) Waffe %d, Soll 3", re15_player_equipped_weapon());

    /* (F) Granate fliegt noch: Platz faellt sofort, Waffe erst nach dem Effekt */
    re15_esp_fx_reset();
    re15_inv_init();
    platz(0, 0x09, 0);
    re15_inv_set_equipped_slot(0);
    re15_player_set_equipped_weapon(0x09);
    re15_esp_fx_t *g = re15_esp_granate_spawn(&s_core, 2, 0, -2474, 0, 0);
    CHECK(g != NULL && g->granate_art == 2, "(F) Granatenplatz nicht gespawnt");
    weg = re15_inventar_r36_tick();
    CHECK(weg == 1 && re15_inv_find_item(0x09) < 0, "(F) Platz nicht sofort entfernt");
    CHECK(re15_player_equipped_weapon() == 0x09, "(F) Waffe %d wechselte, waehrend die Granate fliegt (Soll 9)",
          re15_player_equipped_weapon());
    re15_esp_fx_reset();
    (void)re15_inventar_r36_tick();
    CHECK(re15_player_equipped_weapon() == RE15_MESSER_ID, "(F) Waffe %d nach dem Effekt, Soll 1",
          re15_player_equipped_weapon());

    /* (G) volle Granate bleibt */
    re15_inv_init();
    platz(0, 0x09, 1);
    re15_inv_set_equipped_slot(0);
    re15_player_set_equipped_weapon(0x09);
    weg = re15_inventar_r36_tick();
    CHECK(weg == 0 && g_inv.slots[0].id == 0x09 && re15_player_equipped_weapon() == 0x09,
          "(G) volle Granate angefasst");
    if (!s_fail) printf("  (D)-(G) leere Granaten/Karten weg + verdichtet, Ablegen 0x80 -> Messer erst nach dem Wurf\n");
}

/* ---- (H) Debug-Menue ------------------------------------------------------------------------ */
static void teil_debugmenue(void)
{
    long n = 0;
    uint8_t *db = datei("BIN/DEBUG.BIN", &n);
    CHECK(db != NULL, "(H) DEBUG.BIN fehlt");
    if (!db) return;
    re15_debug_menu_set_bin(db, (unsigned)n);
    int saetze = 0, fehler = 0;
    for (int ch = 0; ch <= 4; ch += 4) {
        g_gameflow.character = ch;
        for (int st = 0; st < 6; st++) {
            for (int idx = 0; idx < 0x31; idx++) {
                unsigned off = re15_debug_menu_table_offset((unsigned)st, (unsigned)idx) * 2u + 0x263au;
                unsigned hw = (unsigned)db[off] | ((unsigned)db[off + 1] << 8);
                if (hw == 0) continue;
                unsigned basis = ((unsigned)(st + 1) << 12) | ((unsigned)idx << 4);
                re15_debug_menu_reset();
                if (!re15_debug_menu_point_at(basis)) { fehler++; continue; }
                /* derselbe Pfad wie ein Quadrat in der JUMP-Zeile (@0x80014A38) */
                if (!re15_debug_menu_tick(0, RE15_DBG_EDGE_LOAD)) { fehler++; continue; }
                unsigned ziel = re15_debug_menu_ziel_id();
                unsigned soll = basis | (ch ? 1u : 0u);
                char rel[64];
                snprintf(rel, sizeof rel, "STAGE%d/ROOM%04X.RDT", st + 1, ziel);
                if (ziel != soll || !existiert(rel)) {
                    if (fehler < 5) printf("  FAIL: (H) Charakter %d Satz %d/%02x -> ROOM%04X (Soll %04X)%s\n",
                                           ch, st, idx, ziel, soll, existiert(rel) ? "" : " Datei fehlt");
                    fehler++;
                }
                saetze++;
            }
        }
    }
    CHECK(fehler == 0, "(H) %d falsche Sprungziele", fehler);
    CHECK(saetze == 2 * 101, "(H) %d Saetze geprueft, Soll 202 (101 je Charakter)", saetze);
    g_gameflow.character = 4;
    re15_debug_menu_reset();
    re15_debug_menu_point_at(0x1070);
    re15_debug_menu_tick(0, RE15_DBG_EDGE_LOAD);
    printf("    Elza: Menue 107 -> ROOM%04X\n", (unsigned)re15_debug_menu_ziel_id());
    CHECK(re15_debug_menu_ziel_id() == 0x1071, "(H) Elza 107 -> %04X, Soll 1071 (Nutzer-Beispiel)",
          (unsigned)re15_debug_menu_ziel_id());
    g_gameflow.character = 0;
    re15_debug_menu_reset();
    re15_debug_menu_point_at(0x1070);
    re15_debug_menu_tick(0, RE15_DBG_EDGE_LOAD);
    CHECK(re15_debug_menu_ziel_id() == 0x1070, "(H) Leon 107 -> %04X, Soll 1070",
          (unsigned)re15_debug_menu_ziel_id());
    if (!s_fail) printf("  (H) Debug-Menue: %d Saetze, Leon xxx0 / Elza xxx1, alle Dateien vorhanden\n", saetze);
    /* db bleibt bis Programmende (das Menue haelt eine Kopie, aber sicher ist sicher) */
}

int main(void)
{
    long n = 0;
    uint8_t *core = datei("DATA/CORE00.ESP", &n);
    if (!core || re15_esp_parse_global(core, (size_t)n, &s_core) != 0) {
        printf("FAIL: CORE00.ESP nicht ladbar\n");
        return 1;
    }
    re15_esp_set_global_bank(&s_core);
    re15_esp_set_room_bank(NULL);

    printf("[r36 inventar] Memory Card beim Speichern\n");  teil_speichern();
    printf("[r36 inventar] leere Plaetze\n");             teil_leer();
    printf("[r36 inventar] Debug-Menue Elza\n");          teil_debugmenue();

    printf(s_fail ? "=== ROT ===\n" : "=== GRUEN ===\n");
    return s_fail ? 1 : 0;
}
