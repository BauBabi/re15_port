/* test_r42_lageplaene2.c — Runde 42 Strang lageplaene, Stufe 2: EIN Plan je Gebiet fuer STAGE2..6 wie RE2.
 * Kopf mit allen Belegen: include/re15_kartenitems_r36.h (Abschnitt RUNDE 42 STUFE 2).
 *
 * NUTZERWUNSCH 18.11.2026: je Gebiet ein Plan wie in Resident Evil 2, gut sichtbar wie ein Plakat, frueh auf dem Pflichtweg
 * beider Figuren; Kanalisation "der Wandplan im ersten Kanalraum 2000/2001". Forschungszentrum (4020, r42/rest) und Labor
 * (5030, Original sub02 @0x010AC) werden hier NICHT doppelt gebaut.
 * RE2-Vorbild: room4040 @0x1B9B "There's a sewer map. / Will you take it?", @0x1BCB "You've taken / the sewer map.";
 * room5040 @0x2341 / @0x2376 "factory map".
 *
 * Teile (je eigener ctest-Eintrag). Der Riegel benutzt NUR die API und die Zeilenfelder, die schon auf aaea5f8c2 bestanden
 * (raum, blatt, bit, aot_slot, x/y/z, rect_x/rect_z), damit die Gegenprobe dort BAUT und rot wird:
 *   tabelle  Kanalisation 2000/2001 Blatt 6 Bit 34, Fabrik NUR 30C1 Blatt 7 Bit 35, U-Bahn 6000/6001 Blatt 12 Bit 37; Blatt =
 *            Zonen-Blatt; keine Zeile fuer 4020/4021 (r42/rest) und 5030/5031 (Original-Laborplan)
 *   kanal    2000 UND 2001: Plan haengt; ECHTER Aktionsdruck vor der Wand (re15_aot_scan, Band des Gehwegs) loest
 *            Ereignis 28 aus; Ja -> Bit 34, Blatt 6 im Besitz und waehlbar, kein anderes Plan-Bit; danach haengt er nicht
 *   fabrik   30C1 (Elza): wie kanal mit Bit 35 / Blatt 7. 30C0 (Leon spielt dort Marvin, Konzept L-06): kein Prop, keine
 *            Zone, kein Programm. Mit Leons Uebergabe-Bit 35 (3060, story3a_r42.c) haengt auch in 30C1 nichts
 *   ubahn    6000 UND 6001 (Band des Treppenabsatzes): Bit 37, Blatt 12
 *   texte    RE2-Wortlaut "There's a <Plan>. / Will you take it?" und "You've taken / the <Plan>."
 *   sicht    die Plakatflaeche liegt auf der SCA-Wand (Bytes aus dem RDT an der zitierten Stelle) und ganz im Bild der
 *            zitierten Kamera (re15_camera_build_view), in BEIDEN Fassungen (2000/2001, 30C1, 6000/6001; Nacharbeit);
 *            Kanalisation/U-Bahn 1080 x 720 wie das RE2-MD1, Fabrik 2160 x 1440 (Nacharbeit s = 2)
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_collision.h"
#include "re15_camera.h"
#include "re15_climb.h"
#include "re15_msg.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_map_owned.h"
#include "re15_map_hint.h"
#include "re15_kartenitems_r36.h"

#include <math.h>
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

/* Soll je Gebiet (Kopf re15_kartenitems_r36.h RUNDE 42 STUFE 2). boden = y des Standorts aus der Tuer, die dort ankommt:
 * 1260 Slot 4 @0x011AA next_y `f8 f8` = -1800; 3091 Slot 0 @0x01332 next_y 0; 50B0 Slot 1 @0x004AE next_y `f0 f1` = -3600.
 * Wand = SCA-Zelle (Datei-Offset im RDT, x/z-Kante): 2000 Zelle 7 @0x012A4 z-Kante -19800 (Zelle z[-19800..-17800]);
 * 30C1 Zelle 1 @0x007F8 z-Kante 1100 (z[1100..1600]); 6000 Zelle 17 @0x00828 x-Kante 2025 (x[25..2025]). */
typedef struct {
    uint16_t raum, raum2;  /* geprueft in beiden Fassungen (raum2 = 0: nur raum)                                    */
    uint8_t  blatt, bit;
    int32_t  boden;
    uint32_t sca_off;      /* Datei-Offset des 12-B-SCA-Eintrags                                                     */
    int      achse;        /* 0 = Wand senkrecht zu x (Kante = x0 + w), 1 = senkrecht zu z (Kante = z0)               */
    int32_t  kante;        /* erwartete Wandkoordinate                                                               */
    int      cut;          /* Kamera, in der das Plakat ganz im Bild liegt (Ankunfts-/Pflichtkamera)                  */
    const char *name;
} soll_t;

static const soll_t k_soll[3] = {
    { 0x2000, 0x2001,  6, 34, -1800, 0x012A4, 1, -19800, 11, "sewer map"  },
    { 0x30C1, 0,       7, 35,     0, 0x007F8, 1,   1100, 1, "factory map" },
    { 0x6000, 0x6001, 12, 37, -3600, 0x00828, 0,   2025, 0, "subway map"  },
};
static const uint8_t k_plan_bits[] = { 34, 35, 37, 77, 78, 87, 88, 89 };   /* alle Plan-Bits (Stufe 1 + 2) */

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

/* Raumstart wie der Tuer-Weg (scd_room_reenter mit allen Port-Installern), Bank 9 bleibt; Spieler auf dem Boden des Standorts
 * und im Band, das die Tuer dort setzt (aot_common.c re15_collision_set_band(band_from_y(spawn_y))). */
static void raum_start(raum_t *r, int32_t px, int32_t py, int32_t pz)
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
    pl->state = 0; pl->motion = 0;
    pl->x = px; pl->y = py; pl->z = pz; pl->rot_y = 0;
    re15_collision_set_band(re15_collision_band_from_y(py));
    pl->floor = (uint8_t)re15_collision_band_from_y(py);
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

/* Lage aus der Zeile: Flaechenmitte M und Raumnormale n (Ursprung = M - 85 n, Rechteckmitte = M + 300 n; karten_export.py). */
static int lage(const re15_kartenitem_r36_t *z, double M[3], double n[3])
{
    double ux = z->x, uz = z->z, cx = z->rect_x + RE15_R36K_RECT / 2.0, cz = z->rect_z + RE15_R36K_RECT / 2.0;
    double dx = cx - ux, dz = cz - uz, l = sqrt(dx * dx + dz * dz);
    if (l < 1.0) return -1;
    n[0] = dx / l; n[1] = 0; n[2] = dz / l;
    M[0] = ux + 85.0 * n[0]; M[1] = z->y; M[2] = uz + 85.0 * n[2];
    return 0;
}

static int haengt(uint16_t raum, int32_t boden)
{
    const re15_kartenitem_r36_t *z = re15_kartenitems_r36_des_raums(raum);
    raum_t r;
    if (raum_laden(&r, raum) != 0) return -1;
    raum_start(&r, z ? z->rect_x + RE15_R36K_RECT / 2 : 0, boden, z ? z->rect_z + RE15_R36K_RECT / 2 : 0);
    int h = prop_slot(RE15_R36K_OBJ) >= 0 && karten_zone() >= 0;
    free(r.buf);
    return h;
}

/* ECHTER Aktionsdruck: Spieler 550 vor der Plakatmitte, Blick in die Wand (Vorwaertspunkt 620 @0x80042bd0:
 * fx = x + 620 cos, fz = z - 620 sin), Scan wie game_step_common.c; Ereignis 28 -> Faden mit JA bis zum Ende.
 * Rueckgabe 1 = Ereignis aus dem Scan + Faden zu Ende, 0 = keine Zone, < 0 Fehler. */
static int druck_und_nehmen(uint16_t raum, int32_t boden, int *ereignis)
{
    const re15_kartenitem_r36_t *z = re15_kartenitems_r36_des_raums(raum);
    double M[3], n[3];
    *ereignis = -1;
    if (!z || lage(z, M, n) != 0) return 0;
    raum_t r;
    if (raum_laden(&r, raum) != 0) return -1;
    int32_t sx = (int32_t)lround(M[0] + 550.0 * n[0]), sz = (int32_t)lround(M[2] + 550.0 * n[2]);
    raum_start(&r, sx, boden, sz);
    if (karten_zone() < 0) { free(r.buf); return 0; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* Blick -n: cos r = -n.x, -sin r = -n.z  ->  r = atan2(n.z, -n.x) */
    int rot = (int)lround(atan2(n[2], -n[0]) / (2.0 * 3.14159265358979323846) * 4096.0) & 0xFFF;
    for (int f = 0; f < 12; f++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); }
    g_scd.message_display_frames = 0; g_scd.message_query = 0; g_scd.message_active = 0;
    re15_pauseflags_clear();
    pl->x = sx; pl->y = boden; pl->z = sz; pl->rot_y = (int16_t)rot;
    for (int f = 0; f < 3; f++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); }
    /* Die Aktionstaste geht im Spieler-FSM ZUERST an die Klettersonde FUN_8002d474 (Aufrufer @0x80031fd0/@0x800323ec);
     * verbraucht sie den Druck, laeuft kein Ereignis (gemessen 2000: Leon kletterte bei x 1725 aufs Plakat). Gepruefte Standorte:
     * entlang der Wand -450..+450 um die Mitte (der Kasten 0 des Plakats mit Rand -112 trifft NUR ausserhalb |d| < 112,
     * FUN_8002da4c-Wrap @0x8002da8c-a4; die Mitte allein saehe den Fehler nicht). */
    int klettert = 0;
    for (int k = -2; k <= 2; k++) {
        double t = 225.0 * k;   /* Wandrichtung = (n.z, 0, -n.x) */
        pl->x = (int32_t)lround(sx + t * n[2]); pl->z = (int32_t)lround(sz - t * n[0]); pl->rot_y = (int16_t)rot;
        re15_climb_reset();
        int kk = re15_climb_try_start(&r.rdt, 1);
        re15_climb_reset();
        if (kk) { klettert++; printf("    ROOM%04X: Klettersonde bei (%ld,%ld) -> KLETTERN\n", (unsigned)raum, (long)pl->x, (long)pl->z); }
    }
    pl->x = sx; pl->z = sz; pl->rot_y = (int16_t)rot;
    printf("    ROOM%04X: Klettersonde an 5 Standorten vor dem Plakat -> %d (0 = Druck bleibt fuer den Aktions-Scan)\n",
           (unsigned)raum, klettert);
    if (klettert) { free(r.buf); return -5; }
    g_aot_action_pressed = 1;
    scd_vm_tick();
    re15_aot_scan(pl->x, pl->z, 0);
    g_aot_action_pressed = 0;
    *ereignis = g_aot.fired_event_id_this_frame;
    printf("    ROOM%04X: Stand (%ld,%ld,%ld) Blick %d Band %d -> Ereignis %d\n", (unsigned)raum, (long)sx, (long)boden,
           (long)sz, rot, re15_collision_debug_band(), *ereignis);
    if (*ereignis != RE15_R36K_EREIGNIS) { free(r.buf); return -2; }
    int slot = scd_event_fire((uint8_t)*ereignis);
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

static int blatt_der_zone(uint16_t raum)
{
    for (int k = 0; k < re15_map_zone_count(); k++) {
        const re15_map_zone_t *zn = re15_map_zone_by_index(k);
        if (zn && zn->room == raum) return (int)zn->page;
    }
    return -1;
}

/* ------------------------------------------------------------------------------------------------ tabelle */
static void teil_tabelle(void)
{
    printf("=== tabelle: ein Plan je Gebiet STAGE2..6 (RE2 room4040/room5040) ===\n");
    for (int i = 0; i < 3; i++) {
        const soll_t *s = &k_soll[i];
        for (int v = 0; v < 2; v++) {
            uint16_t raum = v ? s->raum2 : s->raum;
            if (!raum) continue;
            const re15_kartenitem_r36_t *z = re15_kartenitems_r36_des_raums(raum);
            PRUEFE(z && z->blatt == s->blatt && z->bit == s->bit,
                   "ROOM%04X: Plan \"%s\" Blatt %d Bit %d (soll Blatt %u Bit %u)", (unsigned)raum, s->name,
                   z ? z->blatt : -1, z ? z->bit : -1, s->blatt, s->bit);
            int bz = blatt_der_zone(raum);
            PRUEFE(z && bz == (int)z->blatt, "ROOM%04X: Blatt %d == Blatt der Raumzone %d (re15_map_zones)",
                   (unsigned)raum, z ? z->blatt : -1, bz);
        }
    }
    PRUEFE(re15_kartenitems_r36_des_raums(0x30C0) == NULL,
           "ROOM30C0 (Leon spielt Marvin, Konzept L-06; Plan erst in 3060 von Marvin): keine Zeile");
    static const uint16_t nicht[4] = { 0x4020, 0x4021, 0x5030, 0x5031 };
    for (int i = 0; i < 4; i++)
        PRUEFE(re15_kartenitems_r36_des_raums(nicht[i]) == NULL,
               "ROOM%04X: keine Zeile (4020 baut r42/rest, 5030 ist der Original-Laborplan) - nicht doppelt", nicht[i]);
}

/* ------------------------------------------------------------------------------------------------ nehmen */
static void nehmen(const soll_t *s, uint16_t raum)
{
    re15_game_state_init();
    PRUEFE(!re15_map_owned_page(s->blatt), "ROOM%04X frischer Stand: Blatt %u nicht im Besitz", (unsigned)raum, s->blatt);
    PRUEFE(haengt(raum, s->boden) == 1, "ROOM%04X: Plan haengt (Prop obj %d + Zone Ereignis %d)", (unsigned)raum,
           RE15_R36K_OBJ, RE15_R36K_EREIGNIS);
    int ev = -1;
    int r = druck_und_nehmen(raum, s->boden, &ev);
    PRUEFE(r == 1, "ROOM%04X: Aktionsdruck vor der Wand -> Ereignis %d, Port-Programm lief mit Ja zu Ende (%d)",
           (unsigned)raum, ev, r);
    PRUEFE(re15_game_flag_get(BANK9, s->bit), "ROOM%04X: Bit (9,%u) gesetzt", (unsigned)raum, s->bit);
    int andere = 0;
    for (unsigned k = 0; k < sizeof k_plan_bits; k++)
        if (k_plan_bits[k] != s->bit && re15_game_flag_get(BANK9, k_plan_bits[k])) andere++;
    PRUEFE(andere == 0, "ROOM%04X: kein anderes Plan-Bit gesetzt (%d)", (unsigned)raum, andere);
    PRUEFE(re15_map_owned_page(s->blatt) && re15_map_blatt_waehlbar(s->blatt),
           "ROOM%04X: Blatt %u im Besitz und waehlbar", (unsigned)raum, s->blatt);
    PRUEFE(haengt(raum, s->boden) == 0, "ROOM%04X: nach der Aufnahme haengt der Plan nicht mehr", (unsigned)raum);
}

static void teil_kanal(void)
{
    printf("=== kanal: Plan der Kanalisation 2000/2001 (Konzept F-25 \"Wandplan im ersten Kanalraum\") ===\n");
    nehmen(&k_soll[0], 0x2000);
    nehmen(&k_soll[0], 0x2001);
}

static void teil_fabrik(void)
{
    printf("=== fabrik: Fabrikplan nur fuer Elza (30C1), Leon bekommt ihn von Marvin ===\n");
    nehmen(&k_soll[1], 0x30C1);
    re15_game_state_init();
    raum_t r;
    if (raum_laden(&r, 0x30C0) == 0) {
        raum_start(&r, -7600, 0, 500);
        PRUEFE(prop_slot(RE15_R36K_OBJ) < 0 && karten_zone() < 0, "ROOM30C0 (Leon/Marvin): kein Plakat, keine Zone");
        free(r.buf);
    }
    PRUEFE(re15_kartenitems_r36_ereignis(0x30C0, RE15_R36K_EREIGNIS) == NULL && re15_kartenitems_r36_obj_id(0x30C0) < 0,
           "ROOM30C0: kein Programm, kein Modell");
    re15_game_state_init();
    re15_game_flag_set(BANK9, 35, 1);   /* Leons Uebergabe 3060 setzt dasselbe Bit (story3a_r42.c) */
    PRUEFE(haengt(0x30C1, 0) == 0 && re15_map_owned_page(7), "Bit 35 schon gesetzt: in 30C1 haengt nichts, Blatt 7 im Besitz");
}

static void teil_ubahn(void)
{
    printf("=== ubahn: Plan der U-Bahn 6000/6001 (Ankunft aus dem Hangar) ===\n");
    nehmen(&k_soll[2], 0x6000);
    nehmen(&k_soll[2], 0x6001);
}

/* ------------------------------------------------------------------------------------------------ texte */
static void teil_texte(void)
{
    printf("=== texte: RE2-Wortlaut room4040 / room5040 ===\n");
    for (int i = 0; i < 3; i++) {
        const soll_t *s = &k_soll[i];
        char f[200] = "", g[200] = "", sf[120], sg[120];
        int lf = 0, lg = 0;
        const uint8_t *mf = re15_kartenitems_r36_meldung(s->raum, 0, &lf);
        const uint8_t *mg = re15_kartenitems_r36_meldung(s->raum, 1, &lg);
        if (mf) re15_msg_decode_text(mf, (size_t)lf, f, sizeof f);
        if (mg) re15_msg_decode_text(mg, (size_t)lg, g, sizeof g);
        printf("    ROOM%04X Frage \"%s\" | genommen \"%s\"\n", (unsigned)s->raum, f, g);
        snprintf(sf, sizeof sf, "There's a %s.", s->name);
        snprintf(sg, sizeof sg, "the %s.", s->name);
        PRUEFE(strncmp(f, sf, strlen(sf)) == 0 && strstr(f, "Will you take it?"),
               "ROOM%04X Frage \"%s / Will you take it?\" (RE2 room4040 @0x1B9B)", (unsigned)s->raum, sf);
        PRUEFE(strncmp(g, "You've taken", 12) == 0 && strstr(g, sg),
               "ROOM%04X Meldung \"You've taken / %s\" (RE2 room4040 @0x1BCB)", (unsigned)s->raum, sg);
    }
}

/* ------------------------------------------------------------------------------------------------ sicht */
/* Nacharbeit (Strang lageplaene_nach, Pruefer s2 gelb 7): der Riegel prueft BEIDE Fassungen (raum und raum2 = Elzas 2001/6001;
 * die SCA-Zelle steht dort am selben Datei-Offset, ROOM2001 @0x012A4 und ROOM6001 @0x00828 mit denselben Bytes, Kamera 11 bzw. 0
 * dieselbe). Groesse: s aus den MD1-Ecken (Kanalisation/U-Bahn 1080 x 720 = RE2 karte01; Fabrik seit der Nacharbeit s = 2,
 * 2160 x 1440, PORT-WAHL "gut SICHTBAR"); die Flaeche liegt 85 * s vor dem Prop-Ursprung (karten_export.py). */
static void teil_sicht(void)
{
    printf("=== sicht: Plakat auf der SCA-Wand und ganz im Bild der Ankunfts-/Pflichtkamera (Leon- UND Elza-Fassung) ===\n");
    static const int16_t k_soll_breite[3] = { 1080, 2160, 1080 };   /* Fabrik s = 2 (Nacharbeit) */
    for (int i = 0; i < 3; i++) {
        const soll_t *s = &k_soll[i];
        for (int f = 0; f < 2; f++) {
            uint16_t raum = f == 0 ? s->raum : s->raum2;
            if (!raum) continue;
            const re15_kartenitem_r36_t *z = re15_kartenitems_r36_des_raums(raum);
            int msz = 0;
            const uint8_t *md1 = re15_kartenitems_r36_md1_bytes(raum, &msz);
            int16_t ex0 = 0, ex1 = 0, ez0 = 0, ez1 = 0;
            for (int k = 0; md1 && k < 5 && 0x44 + 8 * k + 6 <= msz; k++) {
                int16_t mx = (int16_t)(md1[0x44 + 8 * k] | (md1[0x45 + 8 * k] << 8));
                int16_t mz = (int16_t)(md1[0x48 + 8 * k] | (md1[0x49 + 8 * k] << 8));
                if (mx < ex0) ex0 = mx; if (mx > ex1) ex1 = mx; if (mz < ez0) ez0 = mz; if (mz > ez1) ez1 = mz;
            }
            double sk = (double)(ez1 - ez0) / 1080.0;
            PRUEFE(md1 && ez1 - ez0 == k_soll_breite[i] && 3 * (ex1 - ex0) == 2 * k_soll_breite[i],
                   "ROOM%04X: MD1-Flaeche %d x %d (soll %d x %d, 3:2 wie RE2 karte01)", (unsigned)raum, ez1 - ez0, ex1 - ex0,
                   k_soll_breite[i], k_soll_breite[i] * 2 / 3);
            double M[3], n[3];
            if (!z || lage(z, M, n) != 0) { PRUEFE(0, "ROOM%04X: keine Zeile", (unsigned)raum); continue; }
            M[0] += 85.0 * (sk - 1.0) * n[0]; M[2] += 85.0 * (sk - 1.0) * n[2];   /* Flaeche 85 * s vor dem Ursprung */
            raum_t r;
            if (raum_laden(&r, raum) != 0) continue;
            /* Wand aus den RDT-Bytes: 12-B-Eintrag w u16, d u16, x s16, z s16 (re15_rdt.h re15_sca_entry_t) */
            const uint8_t *e = r.buf + s->sca_off;
            int32_t w = (int32_t)(e[0] | (e[1] << 8)), x0 = (int16_t)(e[4] | (e[5] << 8)), z0 = (int16_t)(e[6] | (e[7] << 8));
            int32_t kante = s->achse == 0 ? x0 + w : z0;
            double wand = s->achse == 0 ? M[0] : M[2];
            PRUEFE(kante == s->kante && fabs(wand - kante) < 1.0,
                   "ROOM%04X: SCA @0x%05X Kante %ld (soll %ld), Plakatflaeche bei %.1f", (unsigned)raum, (unsigned)s->sca_off,
                   (long)kante, (long)s->kante, wand);
            PRUEFE(fabs(M[1] - (s->boden - 2000)) < 1.0, "ROOM%04X: Mitte %.0f = Boden %ld - 2000 (PORT-WAHL Augenhoehe)",
                   (unsigned)raum, M[1], (long)s->boden);
            re15_camera_view_t v;
            int ok = s->cut < r.rdt.cut_count && re15_camera_build_view(&r.rdt.cuts[s->cut], &v) == 0;
            double rx[3] = { n[2], 0, -n[0] };   /* rechts = unten x (-n), unten = (0,1,0) */
            int drin = ok;
            double bx0 = 1e9, bx1 = -1e9, by0 = 1e9, by1 = -1e9;
            for (int a = -1; ok && a <= 1; a += 2)
                for (int b = -1; b <= 1; b += 2) {
                    double p[3] = { M[0] + a * 540.0 * sk * rx[0], M[1] + b * 360.0 * sk, M[2] + a * 540.0 * sk * rx[2] };
                    double vx = (v.rot[0] * p[0] + v.rot[1] * p[1] + v.rot[2] * p[2]) / 4096.0 + v.trans[0];
                    double vy = (v.rot[3] * p[0] + v.rot[4] * p[1] + v.rot[5] * p[2]) / 4096.0 + v.trans[1];
                    double vz = (v.rot[6] * p[0] + v.rot[7] * p[1] + v.rot[8] * p[2]) / 4096.0 + v.trans[2];
                    if (vz <= 0) { drin = 0; continue; }
                    double sx = 160.0 + v.fov_screen_dist * vx / vz, sy = 120.0 + v.fov_screen_dist * vy / vz;
                    if (sx < 0 || sx >= 320 || sy < 0 || sy >= 240) drin = 0;
                    if (sx < bx0) bx0 = sx; if (sx > bx1) bx1 = sx; if (sy < by0) by0 = sy; if (sy > by1) by1 = sy;
                }
            PRUEFE(drin, "ROOM%04X Cut %d: Plakat ganz im Bild x %.0f..%.0f y %.0f..%.0f", (unsigned)raum, s->cut,
                   bx0, bx1, by0, by1);
            free(r.buf);
        }
    }
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    printf("=== Runde 42 Strang lageplaene Stufe 2: STAGE2..6 wie RE2 (Teil %s) ===\n", teil);
    int alle = strcmp(teil, "alle") == 0;
    if (alle || !strcmp(teil, "tabelle")) teil_tabelle();
    if (alle || !strcmp(teil, "kanal"))   teil_kanal();
    if (alle || !strcmp(teil, "fabrik"))  teil_fabrik();
    if (alle || !strcmp(teil, "ubahn"))   teil_ubahn();
    if (alle || !strcmp(teil, "texte"))   teil_texte();
    if (alle || !strcmp(teil, "sicht"))   teil_sicht();
    if (g_fail) { printf("FAIL\n"); return 1; }
    printf("OK\n");
    return 0;
}
