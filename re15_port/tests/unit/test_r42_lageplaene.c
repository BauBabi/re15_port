/* test_r42_lageplaene.c — Runde 42 Strang lageplaene, Stufe 1: STAGE1-Lageplaene in der RE2-Form.
 * Kopf mit allen Belegen: include/re15_kartenitems_r36.h (Abschnitt RUNDE 42).
 *
 * NUTZERWUNSCH 18.11.2026 woertlich: "Ich will das es mehr funktioniert wie in Resident Evil 2." und
 * "nicht benötigte maps kommen weg."
 * RE2-Vorbild room20B0 sub10: EIN Plan setzt Set(33,2) @0x037DA, Set(33,3) @0x037DE, Set(33,4) @0x037E2
 * (Besitz 1F/2F/3F) in EINEM Ereignis; Text "A police station map." RDT @0x3B92, "You've taken a / police
 * station map." @0x3DB8. B1-Plan room2130 @0x1B32 / @0x1D57.
 *
 * Teile (je eigener ctest-Eintrag; der Riegel benutzt NUR die API, die schon auf aaea5f8c2 bestand, damit die
 * Gegenprobe dort LAEUFT und rot wird statt nicht zu bauen):
 *   tabelle  genau zwei Plaene: Revierplan ROOM1030, Kellerplan ROOM1200
 *   revier   Revierplan ueber das echte Port-Programm (Ereignis 28, Ja) in 1030 UND 1031: setzt 89/88/87,
 *            Blaetter 2/3/4 im Besitz, Kellerblaetter 0/1 nicht
 *   keller   Kellerplan in 1200 UND 1201: setzt 77/78, Blaetter 1/0 im Besitz, Revierblaetter nicht
 *   weg      1140/1141, 10D0/10D1, 11D0/11D1: kein Karten-Prop (obj 15), keine Zone Ereignis 28, kein Programm,
 *            und die Raumlage (Props + AOT-Saetze) ist dieselbe wie ganz ohne Karten-Installer
 *   texte    RE2-Wortlaut: "A police station map." / "You've taken a ... police station map.";
 *            Keller "A police basement map." / "You've taken the ... police basement map." (Name PORT-WAHL)
 *   alt      alte Spielstaende (Runde 36: ein Bit je Etage): nur 87 (3F) -> Revierplan haengt in 1030, Blatt 4 im
 *            Besitz, Blatt 2/3 nicht, Aufnahme setzt die fehlenden; nur 89 (1F) -> Plan haengt trotzdem; alle drei
 *            -> haengt nicht; nur 78 (B1) -> Kellerplan haengt in 1200
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_map_owned.h"
#include "re15_map_hint.h"
#include "re15_kartenitems_r36.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t         g_scd;
extern re15_aot_state_t g_aot;
extern unsigned         g_current_room_id;
extern uint16_t         g_scd_pad_edge, g_scd_pad_held;

static int g_fail = 0;
#define PRUEFE(cond, ...) do { if (!(cond)) { printf("  FAIL: " __VA_ARGS__); printf("\n"); g_fail = 1; } \
                               else { printf("  ok:   " __VA_ARGS__); printf("\n"); } } while (0)
#define BANK9 9u

typedef struct { uint8_t *buf; size_t n; re15_rdt_t rdt; uint16_t id; } raum_t;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int raum_laden(raum_t *r, uint16_t id)
{
    char p[600];
    snprintf(p, sizeof p, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(id >> 12), (unsigned)id);
    memset(r, 0, sizeof *r);
    r->id = id;
    r->buf = slurp(p, &r->n);
    if (!r->buf) { printf("  FAIL: %s fehlt\n", p); g_fail = 1; return -1; }
    if (re15_rdt_parse(r->buf, r->n, &r->rdt) < 0) { printf("  FAIL: parse %s\n", p); g_fail = 1; return -1; }
    return 0;
}

/* Raumstart wie der Tuer-Weg (scd_room_reenter mit allen Port-Installern), Bank 9 bleibt erhalten
 * (Muster test_r36_kartenbits.c raum_start). */
static void raum_start(raum_t *r, int32_t px, int32_t pz)
{
    re15_game_state_t flags = g_game;
    scd_vm_init();
    g_game = flags;
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = r->id; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = px; pl->y = 0; pl->z = pz; pl->rot_y = 0;
    re15_collision_set_band(re15_collision_band_from_y(pl->y));
    re15_msg_load_room_block(r->rdt.messages, r->rdt.messages_size);
    scd_room_reenter(&r->rdt, px, pz, 0);
}

static int prop_slot(uint8_t oid)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].active && g_scd.props[k].obj_id == oid) return k;
    return -1;
}

static int karten_zone(void)
{
    for (int s = 0; s < RE15_AOT_MAX; s++)
        if (g_aot.slots[s].active && g_aot.slots[s].type == RE15_AOT_TYPE_GENERIC &&
            g_aot.slots[s].event_id == RE15_R36K_EREIGNIS) return s;
    return -1;
}

static const re15_kartenitem_r36_t *zeile_des_raums(uint16_t raum)
{
    for (int k = 0; k < re15_kartenitems_r36_anzahl(); k++)
        if (re15_kartenitems_r36_zeile(k)->raum == (uint16_t)(raum & 0xFFFE)) return re15_kartenitems_r36_zeile(k);
    return NULL;
}

/* Plan im Raum `raum` ueber das ECHTE Port-Programm nehmen (Ereignis 28 feuern wie der Aktions-Scan, jede Frage mit
 * JA = virtuelles Bestaetigen-Bit 0x4000). Rueckgabe: 1 = Faden lief zu Ende, 0 = keine Zone, < 0 Fehler. */
static int plan_nehmen(uint16_t raum)
{
    const re15_kartenitem_r36_t *z = zeile_des_raums(raum);
    raum_t r;
    if (raum_laden(&r, raum) != 0) return -1;
    int32_t px = z ? z->rect_x + RE15_R36K_RECT / 2 : 0, pz = z ? z->rect_z + RE15_R36K_RECT / 2 : 0;
    raum_start(&r, px, pz);
    if (karten_zone() < 0) { free(r.buf); return 0; }
    int slot = scd_event_fire(RE15_R36K_EREIGNIS);
    if (slot < 0) { free(r.buf); return -3; }
    for (long fr = 0; fr < 4000 && g_scd.threads[slot].active; fr++) {
        const unsigned char *raw; int len, id;
        g_scd_pad_edge = (fr > 2 && (fr % 4) == 0) ? 0x4000u : 0u;
        g_scd_pad_held = 0;
        scd_vm_tick();
        re15_msg_tick(&raw, &len, &id);
        g_scd_pad_edge = 0;
    }
    int ok = !g_scd.threads[slot].active;
    free(r.buf);
    return ok ? 1 : -4;
}

static int haengt(uint16_t raum)
{
    const re15_kartenitem_r36_t *z = zeile_des_raums(raum);
    raum_t r;
    if (raum_laden(&r, raum) != 0) return -1;
    raum_start(&r, z ? z->rect_x + RE15_R36K_RECT / 2 : 0, z ? z->rect_z + RE15_R36K_RECT / 2 : 0);
    int h = prop_slot(RE15_R36K_OBJ) >= 0 && karten_zone() >= 0;
    free(r.buf);
    return h;
}

static unsigned besitz(void) { return (unsigned)(re15_map_owned_bits() & 0x1Fu); }

/* ------------------------------------------------------------------------------------------------ tabelle */
static void teil_tabelle(void)
{
    printf("=== tabelle: EIN Revierplan + EIN Kellerplan (RE2 room20B0 / room2130) ===\n");
    int n = re15_kartenitems_r36_anzahl();
    /* Stufe 2: dazu Kanalisation/Fabrik/U-Bahn (Riegel unit_r42_lageplaene2_*); STAGE1 bleibt bei genau zwei Plaenen */
    int n_s1 = 0;
    for (int k = 0; k < n; k++) if ((re15_kartenitems_r36_zeile(k)->raum >> 12) == 1) n_s1++;
    PRUEFE(n_s1 == 2, "Anzahl Plaene in STAGE1 %d (soll 2; Runde 36 hatte 5 Etagenplaene), alle Gebiete %d", n_s1, n);
    const re15_kartenitem_r36_t *a = re15_kartenitems_r36_zeile(0), *b = re15_kartenitems_r36_zeile(1);
    PRUEFE(a && a->raum == 0x1030 && a->blatt == 2 && a->bit == 89,
           "Zeile 0 = Revierplan ROOM1030 (Fundstelle 1F, Blatt 2, Bit 89): ROOM%04X Blatt %d Bit %d",
           a ? a->raum : 0u, a ? a->blatt : -1, a ? a->bit : -1);
    PRUEFE(b && b->raum == 0x1200 && b->blatt == 1 && b->bit == 77,
           "Zeile 1 = Kellerplan ROOM1200 (Fundstelle B2, Blatt 1, Bit 77): ROOM%04X Blatt %d Bit %d",
           b ? b->raum : 0u, b ? b->blatt : -1, b ? b->bit : -1);
}

/* ------------------------------------------------------------------------------------------------ revier */
static void teil_revier(void)
{
    printf("=== revier: Revierplan schaltet 1F + 2F + 3F zugleich frei (RE2 Set(33,2..4) @0x037DA-E2) ===\n");
    static const uint16_t raeume[2] = { 0x1030, 0x1031 };
    for (int i = 0; i < 2; i++) {
        re15_game_state_init();
        PRUEFE(besitz() == 0, "ROOM%04X frischer Stand: kein Polizei-Blatt im Besitz (0x%02X)", raeume[i], besitz());
        int r = plan_nehmen(raeume[i]);
        PRUEFE(r == 1, "ROOM%04X: Zone da, Port-Programm lief mit Ja zu Ende (%d)", raeume[i], r);
        int b87 = re15_game_flag_get(BANK9, 87), b88 = re15_game_flag_get(BANK9, 88), b89 = re15_game_flag_get(BANK9, 89);
        PRUEFE(b87 && b88 && b89, "ROOM%04X: Bits (9,89) 1F=%d (9,88) 2F=%d (9,87) 3F=%d gesetzt", raeume[i], b89, b88, b87);
        PRUEFE(besitz() == 0x1C, "ROOM%04X: Besitz-Maske 0x%02X (soll 0x1C = Blatt 2/3/4, Keller 0/1 nicht)",
               raeume[i], besitz());
        PRUEFE(re15_map_owned_page(2) && re15_map_owned_page(3) && re15_map_owned_page(4) &&
               re15_map_blatt_waehlbar(3) && re15_map_blatt_waehlbar(4),
               "ROOM%04X: Blatt 2F/3F ohne Besuch im Besitz und waehlbar (RE2 Set(35,2..4) @0x037E6-EE)", raeume[i]);
        PRUEFE(!re15_game_flag_get(BANK9, 77) && !re15_game_flag_get(BANK9, 78),
               "ROOM%04X: Kellerbits 77/78 unberuehrt", raeume[i]);
        PRUEFE(haengt(raeume[i]) == 0, "ROOM%04X: nach der Aufnahme haengt der Plan nicht mehr", raeume[i]);
    }
}

/* ------------------------------------------------------------------------------------------------ keller */
static void teil_keller(void)
{
    printf("=== keller: Kellerplan schaltet B2 + B1 zugleich frei ===\n");
    static const uint16_t raeume[2] = { 0x1200, 0x1201 };
    for (int i = 0; i < 2; i++) {
        re15_game_state_init();
        int r = plan_nehmen(raeume[i]);
        PRUEFE(r == 1, "ROOM%04X: Zone da, Port-Programm lief mit Ja zu Ende (%d)", raeume[i], r);
        int b77 = re15_game_flag_get(BANK9, 77), b78 = re15_game_flag_get(BANK9, 78);
        PRUEFE(b77 && b78, "ROOM%04X: Bits (9,77) B2=%d (9,78) B1=%d gesetzt", raeume[i], b77, b78);
        PRUEFE(besitz() == 0x03, "ROOM%04X: Besitz-Maske 0x%02X (soll 0x03 = Blatt 1/0, Revier nicht)", raeume[i], besitz());
        PRUEFE(re15_map_blatt_waehlbar(0) && re15_map_blatt_waehlbar(1),
               "ROOM%04X: Blatt B1 ohne Besuch im Besitz und waehlbar", raeume[i]);
        PRUEFE(!re15_game_flag_get(BANK9, 87) && !re15_game_flag_get(BANK9, 88) && !re15_game_flag_get(BANK9, 89),
               "ROOM%04X: Revierbits 87/88/89 unberuehrt", raeume[i]);
    }
}

/* ------------------------------------------------------------------------------------------------ weg */
typedef struct { int n_props; uint8_t obj[64]; int32_t px[64]; int n_aot; uint8_t typ[RE15_AOT_MAX]; uint8_t ev[RE15_AOT_MAX]; } lage_t;

static void lage_lesen(lage_t *l)
{
    memset(l, 0, sizeof *l);
    for (int k = 0; k < (int)g_scd.prop_count && l->n_props < 64; k++)
        if (g_scd.props[k].active) { l->obj[l->n_props] = g_scd.props[k].obj_id; l->px[l->n_props++] = g_scd.props[k].x; }
    for (int s = 0; s < RE15_AOT_MAX; s++)
        if (g_aot.slots[s].active) { l->n_aot++; l->typ[s] = (uint8_t)(1 + g_aot.slots[s].type); l->ev[s] = g_aot.slots[s].event_id; }
}

static void teil_weg(void)
{
    printf("=== weg: die Etagenplaene 3F/2F/B1 sind entfernt (\"nicht benötigte maps kommen weg.\") ===\n");
    static const uint16_t raeume[6] = { 0x1140, 0x1141, 0x10D0, 0x10D1, 0x11D0, 0x11D1 };
    /* Stelle der Runde-36-Karte (Rechteckmitte, gen/kartenitems_r36.inc Stand aaea5f8c2) als Standort */
    static const int32_t pos[6][2] = { { -2051, -9010 }, { -2051, -9010 }, { 2134, 21924 }, { 2134, 21924 },
                                       { 1360, -19030 }, { 1360, -19030 } };
    for (int i = 0; i < 6; i++) {
        raum_t r;
        if (raum_laden(&r, raeume[i]) != 0) continue;
        re15_game_state_init();
        raum_start(&r, pos[i][0], pos[i][1]);
        int ps = prop_slot(RE15_R36K_OBJ), zs = karten_zone();
        PRUEFE(ps < 0, "ROOM%04X: kein Karten-Prop obj %d (Slot %d)", raeume[i], RE15_R36K_OBJ, ps);
        PRUEFE(zs < 0, "ROOM%04X: keine Zone Ereignis %d (Slot %d)", raeume[i], RE15_R36K_EREIGNIS, zs);
        PRUEFE(re15_kartenitems_r36_ereignis(raeume[i], RE15_R36K_EREIGNIS) == NULL,
               "ROOM%04X: Ereignis %d liefert kein Programm", raeume[i], RE15_R36K_EREIGNIS);
        PRUEFE(re15_kartenitems_r36_obj_id(raeume[i]) < 0 && re15_kartenitems_r36_md1_bytes(raeume[i], NULL) == NULL,
               "ROOM%04X: Plattform-Lader bekommt kein Kartenmodell", raeume[i]);
        /* Gegenstueck: dieselbe Lage wie ein Raumstart, bei dem der Installer nichts tun KANN (alle Plan-Bits gesetzt). */
        lage_t mit, ohne;
        lage_lesen(&mit);
        re15_game_state_init();
        for (int b = 77; b <= 78; b++) re15_game_flag_set(BANK9, (uint8_t)b, 1);
        for (int b = 87; b <= 89; b++) re15_game_flag_set(BANK9, (uint8_t)b, 1);
        raum_start(&r, pos[i][0], pos[i][1]);
        lage_lesen(&ohne);
        PRUEFE(memcmp(&mit, &ohne, sizeof mit) == 0,
               "ROOM%04X: Props (%d) und AOT-Saetze (%d) gleich wie ohne Karten-Installer = Wand wie ausgeliefert",
               raeume[i], mit.n_props, mit.n_aot);
        free(r.buf);
    }
}

/* ------------------------------------------------------------------------------------------------ texte */
static void text_von(uint16_t raum, int welche, char *out, size_t n)
{
    int len = 0;
    const uint8_t *m = re15_kartenitems_r36_meldung(raum, welche, &len);
    out[0] = 0;
    if (m) re15_msg_decode_text(m, (size_t)len, out, n);
}

static void teil_texte(void)
{
    printf("=== texte: RE2-Wortlaut ===\n");
    char f[200], g[200];
    text_von(0x1030, 0, f, sizeof f); text_von(0x1030, 1, g, sizeof g);
    printf("    1030 Frage \"%s\" | genommen \"%s\"\n", f, g);
    PRUEFE(strncmp(f, "A police station map.", 21) == 0 && strstr(f, "Will you take it?"),
           "Revierplan-Frage = RE2 room20B0 msg 0 @0x3B92 \"A police station map. / Will you take it?\"");
    PRUEFE(strncmp(g, "You've taken a", 14) == 0 && strstr(g, "police station map.") && !strstr(g, "taken the"),
           "Revierplan-Meldung = RE2 room20B0 msg 17 @0x3DB8 \"You've taken a / police station map.\"");
    text_von(0x1200, 0, f, sizeof f); text_von(0x1200, 1, g, sizeof g);
    printf("    1200 Frage \"%s\" | genommen \"%s\"\n", f, g);
    PRUEFE(strncmp(f, "A police basement map.", 22) == 0 && strstr(f, "Will you take it?"),
           "Kellerplan-Frage im Rahmen RE2 room2130 @0x1B32, Name \"police basement map\" (PORT-WAHL)");
    PRUEFE(strncmp(g, "You've taken the", 16) == 0 && strstr(g, "police basement map."),
           "Kellerplan-Meldung im Rahmen RE2 room2130 @0x1D57");
}

/* ------------------------------------------------------------------------------------------------ alt */
static void teil_alt(void)
{
    printf("=== alt: Spielstaende mit Einzelbits der Runde 36 bleiben gueltig ===\n");
    /* nur 3F (Bit 87, alter Plan ROOM1140) */
    re15_game_state_init();
    re15_game_flag_set(BANK9, 87, 1);
    PRUEFE(besitz() == 0x10, "nur Bit 87: Blatt 4 (3F) im Besitz, 2/3 nicht (0x%02X)", besitz());
    PRUEFE(haengt(0x1030) == 1, "nur Bit 87: Revierplan haengt in ROOM1030 (2F/1F fehlen noch)");
    PRUEFE(plan_nehmen(0x1031) == 1 && besitz() == 0x1C, "nur Bit 87: Aufnahme in 1031 -> Blatt 2/3/4 (0x%02X)", besitz());
    /* nur 1F (Bit 89, alter Plan ROOM1030) */
    re15_game_state_init();
    re15_game_flag_set(BANK9, 89, 1);
    PRUEFE(besitz() == 0x04, "nur Bit 89: Blatt 2 (1F) im Besitz (0x%02X)", besitz());
    PRUEFE(haengt(0x1030) == 1, "nur Bit 89 (alter 1F-Plan genommen): Revierplan haengt trotzdem in ROOM1030");
    /* alle drei */
    re15_game_state_init();
    for (int b = 87; b <= 89; b++) re15_game_flag_set(BANK9, (uint8_t)b, 1);
    PRUEFE(haengt(0x1030) == 0 && haengt(0x1031) == 0, "Bits 87/88/89: Revierplan haengt nicht mehr");
    /* nur B1 (Bit 78, alter Plan ROOM11D0) */
    re15_game_state_init();
    re15_game_flag_set(BANK9, 78, 1);
    PRUEFE(besitz() == 0x01, "nur Bit 78: Blatt 0 (B1) im Besitz (0x%02X)", besitz());
    PRUEFE(haengt(0x1200) == 1 && haengt(0x1201) == 1, "nur Bit 78: Kellerplan haengt in ROOM1200/1201");
    PRUEFE(plan_nehmen(0x1200) == 1 && besitz() == 0x03, "nur Bit 78: Aufnahme -> Blatt 0/1 (0x%02X)", besitz());
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    printf("=== Runde 42 Strang lageplaene: STAGE1 wie RE2 (Teil %s) ===\n", teil);
    int alle = strcmp(teil, "alle") == 0;
    if (alle || !strcmp(teil, "tabelle")) teil_tabelle();
    if (alle || !strcmp(teil, "revier"))  teil_revier();
    if (alle || !strcmp(teil, "keller"))  teil_keller();
    if (alle || !strcmp(teil, "weg"))     teil_weg();
    if (alle || !strcmp(teil, "texte"))   teil_texte();
    if (alle || !strcmp(teil, "alt"))     teil_alt();
    if (g_fail) { printf("FAIL\n"); return 1; }
    printf("OK\n");
    return 0;
}
