/*
 * Runde 36 Spur P "story" — Riegel (Teil ueber das Argument). Dossier analysis/befunde_runde36/P_story.md,
 * Konstanten include/re15_story_r36.h, Code engine/src/story_r36.c (+ gen/story_r36_texte.inc).
 *
 *   roy        P1: echter Raumstart ROOM1020 (msg-Block + scd_room_reenter, alle Port-Installer):
 *              ohne Strom (4,0xF3) Roy obj 6 unsichtbar + Slot 5 inert, Aktionsdruck vor Roy -> KEIN Text;
 *              mit Strom obj 6 sichtbar + Slot 5 = Original-Ereignis sub02, Aktionsdruck -> msg 0
 *              "It's the RPD officer, Roy..."; Elzas ROOM1021 unberuehrt (eigenes Gate Ck(3,0x5F))
 *   zelle      P2: ROOM1210 Slot 9 = Elzas Zellentuer-Platz ROOM1211 Slot 8 (Rechteck/Band/Flags/Maske gleich,
 *              gemessen in ROOM1211 selbst), msg 20 = neuer Text, Aktionsdruck im Gang vor der Zelle -> msg 20
 *   schuppen   P3: ROOM1091 Slot 5 = Leons Feuer-Platz ROOM1090 Slot 2 (Viereck/Band/Flags/Maske gleich,
 *              gemessen in ROOM1090 selbst), msg 20 = neuer Text, Aktionsdruck vor dem Schuppen -> msg 20
 *   texte      P4: jede Tabellenzeile: Original an off_alt = alt, nach dem Laden steht neu, Kopf/Schwanz
 *              bytegleich, uebrige Nachrichten unveraendert, <= 128 B, Dauer gleich
 *   sweep      P4: ALLE RDTs beider Szenarien geladen (Port-Lader + Overrides): "Elza" in Leon-Raeumen und
 *              "Leon" in Elza-Raeumen nur noch an den belegten Stellen; die alten Wortlaute nirgends
 *   reihenfolge  Tuer-Weg (scd_room_reenter VOR dem msg-Laden, room_common.c Schritt 7/11) und Boot-Weg
 *              (msg-Laden VOR den Installern, main.c): beide enden mit Overrides + Id 20
 *   mess       (kein add_test) Standort-Suche: Aktionsdruck auf einem Raster um einen Platz
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_story_r36.h"
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_aot.h"
#include "re15_msg.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_files.h"
#include "re15_gameflow.h"

extern re15_aot_state_t g_aot;
extern uint8_t          g_aot_action_pressed;

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

static int fails = 0, checks = 0;
#define CHECK(c, ...) do { checks++; if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("ok:   " __VA_ARGS__); printf("\n"); } } while (0)

typedef struct { uint8_t *buf; size_t n; re15_rdt_t rdt; uint16_t id; } raum_t;

static int raum_laden(raum_t *r, uint16_t id)
{
    char p[600];
    snprintf(p, sizeof p, "%s/STAGE%X/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(id >> 12), (unsigned)id);
    FILE *f = fopen(p, "rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    r->buf = (uint8_t *)malloc((size_t)sz);
    if (!r->buf || fread(r->buf, 1, (size_t)sz, f) != (size_t)sz) { fclose(f); return -1; }
    fclose(f);
    r->n = (size_t)sz; r->id = id;
    return re15_rdt_parse(r->buf, r->n, &r->rdt) == 0 ? 0 : -1;
}

typedef struct { uint8_t bank, bit; } flag_t;

/* Raumstart wie scd_room_reenter am Boot-/Testweg (msg-Block zuerst) bzw. am Tuer-Weg (tuer=1: Raumaufbau
 * zuerst, msg-Block danach — room_common.c Schritt 7 scd_room_reenter, Schritt 11 re15_msg_load_room_block). */
static void hochfahren_ex(raum_t *r, int32_t px, int32_t py, int32_t pz, int16_t rot,
                          const flag_t *fl, int nfl, int tuer)
{
    re15_game_state_init();
    re15_inv_init();
    re15_files_reset();
    re15_msg_clear_room_block();
    re15_aot_init();
    scd_vm_init(); re15_actor_init();
    g_gameflow.character = (uint8_t)((r->id & 1u) ? 4 : 0);    /* Elza = 4 (DAT_800ACA5C), re15_gameflow.h */
    for (int i = 0; i < nfl; i++) re15_game_flag_set(fl[i].bank, fl[i].bit, 1);
    g_current_room_id = r->id;
    g_room_rdt = r->rdt; g_room_rdt_ok = 1;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = px; pl->y = py; pl->z = pz; pl->rot_y = rot; pl->state = 1;
    re15_collision_set_band(re15_collision_band_from_y(py));
    pl->floor = (uint8_t)re15_collision_band_from_y(py);
    g_scd.player_mode = 0;
    if (!tuer) re15_msg_load_room_block(r->rdt.messages, r->rdt.messages_size);
    scd_register_room_events(&r->rdt);
    scd_room_reenter(&r->rdt, px, pz, 0);
    if (tuer) re15_msg_load_room_block(r->rdt.messages, r->rdt.messages_size);
}

static void hochfahren(raum_t *r, int32_t px, int32_t py, int32_t pz, int16_t rot, const flag_t *fl, int nfl)
{
    hochfahren_ex(r, px, py, pz, rot, fl, nfl, 0);
}

static int prop_von(uint8_t oid)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++) if (g_scd.props[k].obj_id == oid) return k;
    return -1;
}

/* Standort begehbar? Der Klemmpfad (re15_collision_constrain, Muster probe_r30_irons-diary-welt.c
 * standort_gueltig) darf den Spieler nicht verschieben. Band wie beim Raumstart (aus y). */
static int standort_gueltig(const raum_t *r, int32_t x, int32_t y, int32_t z)
{
    int32_t nx = x, nz = z;
    re15_collision_set_band(re15_collision_band_from_y(y));
    re15_collision_constrain(&r->rdt, x, z, &nx, &nz);
    return nx == x && nz == z;
}

/* Ein Spielbild aus Sicht der Plaetze: Scan + Ereignis-Abgabe wie re15_game_step (game_step_common.c
 * "Dispatch any AOT event fired this frame": scd_event_fire(g_aot.fired_event_id_this_frame)). */
static void scan(const re15_actor_t *pl)
{
    re15_aot_scan(pl->x, pl->z, 0);
    if (g_aot.fired_event_id_this_frame != 0) (void)scd_event_fire(g_aot.fired_event_id_this_frame);
}

/* Echter Aktionsdruck (Muster test_r35_inhalt_items.c): Raum hochfahren, einschwingen, Spieler an den
 * Standort, eine Flanke der Aktionstaste, dann bis zu `nach` Bilder weiterlaufen. Liefert 1, sobald
 * eine Nachricht offen ist; *id = deren Id. */
static int druck(raum_t *r, int32_t x, int32_t y, int32_t z, int16_t rot, const flag_t *fl, int nfl,
                 int nach, int *id)
{
    hochfahren(r, x, y, z, rot, fl, nfl);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int f = 0; f < 12; f++) { scd_vm_tick(); scan(pl); }
    g_scd.message_display_frames = 0; g_scd.message_query = 0; g_scd.message_active = 0;
    re15_pauseflags_clear();
    pl->x = x; pl->y = y; pl->z = z; pl->rot_y = rot;
    pl->floor = (uint8_t)re15_collision_band_from_y(y);
    for (int f = 0; f < 3; f++) { scd_vm_tick(); scan(pl); }
    g_scd.message_active = 0;
    g_aot.fired_event_id_this_frame = 0;
    g_aot_action_pressed = 1;
    scd_vm_tick();
    scan(pl);
    g_aot_action_pressed = 0;
    for (int f = 0; f <= nach; f++) {
        if (g_scd.message_active) { if (id) *id = (int)g_scd.message_id; return 1; }
        scd_vm_tick();
        scan(pl);
    }
    if (g_scd.message_active) { if (id) *id = (int)g_scd.message_id; return 1; }
    if (id) *id = -1;
    return 0;
}

static const re15_story_r36_text_t *zeile(uint16_t raum, int id)
{
    int n = 0;
    const re15_story_r36_text_t *t = re15_story_r36_tabelle(&n);
    for (int i = 0; i < n; i++) if (t[i].raum == raum && t[i].id == id) return &t[i];
    return NULL;
}

static int text_ist(int id, const char *soll)
{
    const char *t = re15_msg_get_text(id);
    return t && soll && strcmp(t, soll) == 0;
}

/* ---------------------------------------------------------------------------------------------- */
static int teil_roy(void)
{
    raum_t r, e;
    if (raum_laden(&r, 0x1020) || raum_laden(&e, 0x1021)) { printf("FAIL: ROOM1020/1021\n"); return 1; }
    const flag_t strom = { RE15_S36_STROM_BANK, RE15_S36_STROM_BIT };
    /* Standort: vor Roy, Blick -Z (Gierung 1024); 620 voraus (-4350,-9320) liegt im Platz
     * x[-4850..-3850] z[-10100..-9100] (ROOM1020 @0x01DDA). */
    const int32_t sx = -4350, sz = -8700; const int16_t srot = 1024;

    /* ohne Strom */
    hochfahren(&r, sx, 0, sz, srot, NULL, 0);
    int p = prop_von(RE15_S36_ROY_OBJ);
    CHECK(p >= 0 && !g_scd.props[p].active && g_scd.props[p].x == -4488 && g_scd.props[p].z == -10684,
          "P1 ohne Strom: obj 6 (Roy, @0x01DEE (-4488,0,-10684)) im Pool, Zeichen-Bit AUS");
    const re15_aot_t *a = &g_aot.slots[RE15_S36_ROY_SLOT];
    CHECK(a->active && a->type == RE15_AOT_TYPE_NONE && a->x == -4350 && a->z == -9600,
          "P1 ohne Strom: Slot 5 = Aot_reset(5, sce 0) (inert, Rechteck bleibt)");
    CHECK(re15_story_r36_roy_sichtbar() == 0, "P1 ohne Strom: Entscheidung = versteckt");
    CHECK(standort_gueltig(&r, sx, 0, sz), "P1 Standort (%d,%d) begehbar (Klemmpfad schiebt nicht)", sx, sz);
    int id = -2;
    int offen = druck(&r, sx, 0, sz, srot, NULL, 0, 120, &id);
    CHECK(!offen, "P1 ohne Strom: Aktionsdruck vor Roy oeffnet KEINEN Text (offen=%d id=%d)", offen, id);

    /* mit Strom */
    hochfahren(&r, sx, 0, sz, srot, &strom, 1);
    p = prop_von(RE15_S36_ROY_OBJ);
    CHECK(p >= 0 && g_scd.props[p].active && g_scd.props[p].rot_y == 3072,
          "P1 mit Strom (4,0xF3): obj 6 sichtbar wie im Original (rot y 3072)");
    a = &g_aot.slots[RE15_S36_ROY_SLOT];
    CHECK(a->active && a->type == RE15_AOT_TYPE_GENERIC && a->event_id == 2 && a->sce_flags == 0x31,
          "P1 mit Strom: Slot 5 = Original-Ereignis sub02 (Flags 0x31)");
    CHECK(re15_story_r36_roy_sichtbar() == 1, "P1 mit Strom: Entscheidung = liegt da");
    offen = druck(&r, sx, 0, sz, srot, &strom, 1, 120, &id);
    CHECK(offen && id == 0 && text_ist(0, "It's the RPD officer, Roy.He appears to have died recently."),
          "P1 mit Strom: Aktionsdruck vor Roy -> sub02 -> msg 0 \"%s\" (id=%d)",
          re15_msg_get_text(0) ? re15_msg_get_text(0) : "(null)", id);

    /* Elza ROOM1021 unberuehrt: keine Entscheidung, Leiche nach ihrem eigenen Gate Ck(3,0x5F) */
    const flag_t elza_roy[2] = { { 3, 0x5F }, { 0, 0 } };
    hochfahren(&e, sx, 0, sz, srot, elza_roy, 1);
    p = prop_von(RE15_S36_ROY_OBJ);
    CHECK(re15_story_r36_roy_sichtbar() == -1 && p >= 0 && g_scd.props[p].active,
          "P1 Elza ROOM1021 mit (3,0x5F): kein Eingriff, Roy (obj 6 @0x01E40) sichtbar");
    return 0;
}

/* ---------------------------------------------------------------------------------------------- */
static int teil_zelle(void)
{
    raum_t l, e;
    if (raum_laden(&l, 0x1210) || raum_laden(&e, 0x1211)) { printf("FAIL: ROOM1210/1211\n"); return 1; }
    /* Vorlage am Original gemessen: ROOM1211 mit John noch in der Zelle ((4,0xEC)=0) */
    hochfahren(&e, -20600, 0, -21650, 2048, NULL, 0);
    re15_aot_t vor = g_aot.slots[8];
    CHECK(vor.active && vor.type == RE15_AOT_TYPE_MESSAGE && vor.event_id == 10,
          "P2 Vorlage ROOM1211 Slot 8 (sub00 @0x01DEE): Text-Platz msg 10");
    hochfahren(&l, -20600, 0, -21650, 2048, NULL, 0);
    const re15_aot_t *a = &g_aot.slots[RE15_S36_ZELLE_SLOT];
    CHECK(a->active && a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == RE15_S36_ZELLE_MSG &&
          a->x == vor.x && a->z == vor.z && a->half_w == vor.half_w && a->half_h == vor.half_h &&
          a->band == vor.band && a->sce_flags == vor.sce_flags && a->pause_mask16 == vor.pause_mask16 &&
          a->has_quad == vor.has_quad,
          "P2 ROOM1210 Slot 9 = ROOM1211 Slot 8 (x[%d..%d] z[%d..%d] Band %d Flags 0x%02X Maske 0x%04X), msg 20",
          a->x - a->half_w, a->x + a->half_w, a->z - a->half_h, a->z + a->half_h, a->band, a->sce_flags,
          a->pause_mask16);
    const re15_story_r36_text_t *t = zeile(0x1210, 20);
    int rl = 0; const unsigned char *raw = re15_msg_get_raw(20, &rl);
    CHECK(t && text_ist(20, t->neu) && raw && rl == t->len && memcmp(raw, t->bytes, (size_t)rl) == 0,
          "P2 msg 20 abgelegt: \"%s\"", re15_msg_get_text(20) ? re15_msg_get_text(20) : "(null)");
    CHECK(t && strncmp(t->neu, "The cell door is open...", 24) == 0,
          "P2 Wortlaut beginnt mit dem Zitat des Nutzers \"The cell door is open...\"");
    CHECK(standort_gueltig(&l, -20600, 0, -21650), "P2 Standort (-20600,-21650) begehbar");
    int id = -2;
    int offen = druck(&l, -20600, 0, -21650, 2048, NULL, 0, 10, &id);
    CHECK(offen && id == RE15_S36_ZELLE_MSG, "P2 Aktionsdruck im Gang (-20600,-21650) Blick -X -> msg %d", id);
    /* Elza-Raum: der Port legt dort nichts an (Slot 9 ist dort ein Item-Platz @0x01DA8) */
    hochfahren(&e, -20600, 0, -21650, 2048, NULL, 0);
    CHECK(!(g_aot.slots[9].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[9].event_id == 20) &&
          re15_msg_get_text(20) == NULL, "P2 ROOM1211 (Elza): kein Port-Platz, keine msg 20");
    return 0;
}

/* ---------------------------------------------------------------------------------------------- */
/* Standort P3 (gefunden mit `test_r36_story mess 1091 -1000 1500 -1500 1500 250 -1800 0`): westlich vor dem
 * Viereck (linke Kante bei z 250: x 535), Blick +X -> 620 voraus (620,250) im Viereck; Boden y -1800 = Band 1. */
static const int32_t k_sch_x = 0, k_sch_y = -1800, k_sch_z = 250;
static const int16_t k_sch_rot = 0;                                    /* Blick +X                 */

static int teil_schuppen(void)
{
    raum_t l, e;
    if (raum_laden(&l, 0x1090) || raum_laden(&e, 0x1091)) { printf("FAIL: ROOM1090/1091\n"); return 1; }
    /* Vorlage am Original gemessen: Leons ROOM1090 vor dem Loeschen ((3,0x85)=0, (3,0x81)=0) */
    hochfahren(&l, k_sch_x, k_sch_y, k_sch_z, k_sch_rot, NULL, 0);
    re15_aot_t vor = g_aot.slots[2];
    CHECK(vor.active && vor.type == RE15_AOT_TYPE_MESSAGE && vor.event_id == 7 && vor.has_quad,
          "P3 Vorlage ROOM1090 Slot 2 (sub00 @0x0230E): Viereck-Text-Platz msg 7");
    hochfahren(&e, k_sch_x, k_sch_y, k_sch_z, k_sch_rot, NULL, 0);
    const re15_aot_t *a = &g_aot.slots[RE15_S36_SCHUPPEN_SLOT];
    int quad_gleich = 1;
    for (int k = 0; k < 4; k++) if (a->xs[k] != vor.xs[k] || a->zs[k] != vor.zs[k]) quad_gleich = 0;
    CHECK(a->active && a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == RE15_S36_SCHUPPEN_MSG &&
          a->has_quad && quad_gleich && a->band == vor.band && a->sce_flags == vor.sce_flags &&
          a->pause_mask16 == vor.pause_mask16 && a->x == vor.x && a->z == vor.z,
          "P3 ROOM1091 Slot 5 = ROOM1090 Slot 2 (Viereck (%d,%d)(%d,%d)(%d,%d)(%d,%d) Band %d Flags 0x%02X), msg 20",
          a->xs[0], a->zs[0], a->xs[1], a->zs[1], a->xs[2], a->zs[2], a->xs[3], a->zs[3], a->band, a->sce_flags);
    const re15_story_r36_text_t *t = zeile(0x1091, 20);
    int rl = 0; const unsigned char *raw = re15_msg_get_raw(20, &rl);
    CHECK(t && text_ist(20, t->neu) && raw && rl == t->len && memcmp(raw, t->bytes, (size_t)rl) == 0,
          "P3 msg 20 abgelegt: \"%s\"", re15_msg_get_text(20) ? re15_msg_get_text(20) : "(null)");
    CHECK(t && strstr(t->neu, "burnt-out shed") && !strstr(t->neu, "xtinguisher"),
          "P3 Wortlaut traegt \"burnt-out shed\", KEIN Feuerloescher");
    /* Slot 4 (r35 Spur F Shotgun Shells) bleibt */
    CHECK(g_aot.slots[4].active && g_aot.slots[4].type == RE15_AOT_TYPE_ITEM, "P3 Slot 4 (Shotgun Shells r35) bleibt");
    CHECK(standort_gueltig(&e, k_sch_x, k_sch_y, k_sch_z), "P3 Standort (%d,%d) begehbar", k_sch_x, k_sch_z);
    int id = -2;
    int offen = druck(&e, k_sch_x, k_sch_y, k_sch_z, k_sch_rot, NULL, 0, 10, &id);
    CHECK(offen && id == RE15_S36_SCHUPPEN_MSG, "P3 Aktionsdruck (%d,%d) Blick +X (Band 1) -> msg %d",
          k_sch_x, k_sch_z, id);
    /* Leon nach dem Loeschen ((3,0x81)=1): Original legt keinen Platz an — der Port auch nicht */
    /* (3,0x85) Loescher genommen (ROOM1000 sub01 @0x00D0C), (3,0x81) geloescht (ROOM1090 sub06 @0x0271E) */
    const flag_t geloescht[2] = { { 3, 0x85 }, { 3, 0x81 } };
    hochfahren(&l, k_sch_x, k_sch_y, k_sch_z, k_sch_rot, geloescht, 2);
    CHECK(!(g_aot.slots[2].active && g_aot.slots[2].type == RE15_AOT_TYPE_MESSAGE) &&
          !(g_aot.slots[5].active && g_aot.slots[5].type == RE15_AOT_TYPE_MESSAGE) && re15_msg_get_text(20) == NULL,
          "P3 Leon ROOM1090 nach dem Loeschen: kein Text-Platz (Original), keine Port-Nachricht");
    return 0;
}

/* ---------------------------------------------------------------------------------------------- */
static int teil_texte(void)
{
    int n = 0;
    const re15_story_r36_text_t *tab = re15_story_r36_tabelle(&n);
    int umgeschrieben = 0;
    for (int i = 0; i < n; i++) {
        const re15_story_r36_text_t *t = &tab[i];
        char dec[256];
        int dn = re15_msg_decode_text(t->bytes, t->len, dec, sizeof dec);
        CHECK(dn > 0 && strcmp(dec, t->neu) == 0 && t->len <= 128,
              "ROOM%04X msg %d: Bytes dekodieren zu \"%s\" (%d B <= 128)", t->raum, t->id, dec, t->len);
        if (t->off_alt == 0) continue;
        umgeschrieben++;
        raum_t r;
        if (raum_laden(&r, t->raum)) { CHECK(0, "ROOM%04X lesbar", t->raum); continue; }
        /* Original an der Id: Offset aus der Nachrichtentabelle (RDT+0x3C) */
        const uint8_t *blk = r.rdt.messages;
        int bs = (int)r.rdt.messages_size;
        int o0 = blk[t->id * 2] | (blk[t->id * 2 + 1] << 8);
        uint32_t datei = (uint32_t)((blk + o0) - r.buf);
        char alt[256];
        re15_msg_decode_text(blk + o0, (size_t)(bs - o0), alt, sizeof alt);
        CHECK(datei == t->off_alt && strcmp(alt, t->alt) == 0,
              "ROOM%04X msg %d: Original @0x%05X = \"%s\"", t->raum, t->id, datei, alt);
        /* Kopf/Schwanz bytegleich mit dem Original */
        const uint8_t *orig = blk + o0;
        int kopf = 0;
        for (int k = 0; k + 3 < t->len; k++)
            if (t->bytes[k] == 0x16 && t->bytes[k + 1] == 0x05 && t->bytes[k + 2] == 0x00 && t->bytes[k + 3] == 0x00) {
                kopf = k + 4; break; }
        int olen = 0;
        while (o0 + olen < bs && orig[olen] != 0x01) {
            uint8_t b = orig[olen];
            olen += (b == 0x02 || b == 0x04 || b == 0x05 || b == 0x06 || b == 0x09 || b == 0x0A || b == 0x0B) ? 2 : 1;
        }
        olen += 2;
        /* Runde 37 Spur F: Untersuchungstext (ROOM50B1 msg 0) = Kopf `04 02` + Schwanz `01 00` (texte_bauen.py UNTERSUCHEN) */
        const int unters = (t->len >= 4 && t->bytes[0] == 0x04 && t->bytes[1] == 0x02);
        const int schwanz = unters ? 2 : 4;
        if (unters) kopf = 2;
        CHECK((unters || kopf > 4) && memcmp(t->bytes, orig, (size_t)kopf) == 0 &&
              memcmp(t->bytes + t->len - schwanz, orig + olen - schwanz, (size_t)schwanz) == 0,
              "ROOM%04X msg %d: Kopf %d B und Schwanz %d B = Original", t->raum, t->id, kopf, schwanz);
        CHECK(re15_msg_compute_duration(t->bytes, t->len, 0) == re15_msg_compute_duration(orig, (size_t)olen, 0),
              "ROOM%04X msg %d: Dauer %d Bilder = Original", t->raum, t->id,
              re15_msg_compute_duration(t->bytes, t->len, 0));
        /* Nach dem Laden mit der Raum-Id: neu steht, die anderen Ids unveraendert */
        g_current_room_id = t->raum;
        re15_msg_clear_room_block();
        re15_msg_load_room_block(blk, bs);
        int rl = 0;
        const unsigned char *raw = re15_msg_get_raw(t->id, &rl);
        CHECK(text_ist(t->id, t->neu) && raw && rl == t->len && memcmp(raw, t->bytes, (size_t)rl) == 0,
              "ROOM%04X msg %d nach dem Laden: \"%s\"", t->raum, t->id, re15_msg_get_text(t->id));
        int nmsg = (blk[0] | (blk[1] << 8)) / 2, fremd = 0;
        for (int k = 0; k < nmsg; k++) {
            if (zeile(t->raum, k)) continue;
            int ok0 = blk[k * 2] | (blk[k * 2 + 1] << 8);
            int ok1 = (k + 1 < nmsg) ? (blk[(k + 1) * 2] | (blk[(k + 1) * 2 + 1] << 8)) : bs;
            if (ok1 <= ok0) ok1 = bs;
            int kl = 0; const unsigned char *kr = re15_msg_get_raw(k, &kl);
            int soll = ok1 - ok0; if (soll > 192) soll = 192;
            if (!kr || kl != soll || memcmp(kr, blk + ok0, (size_t)kl) != 0) fremd++;
        }
        CHECK(fremd == 0, "ROOM%04X: alle anderen %d Nachrichten bytegleich zur RDT", t->raum, nmsg - 1);
        free(r.buf);
    }
    CHECK(umgeschrieben == 12, "P4: %d umgeschriebene Nachrichten (30C1 3/4/7, 6030 1/2/12, 6031 0/1, 50B0 1, 6000 0/3; "
          "Runde 37 Spur F: 50B1 0)", umgeschrieben);
    return 0;
}

/* ---------------------------------------------------------------------------------------------- */
/* Belegte Restfundstellen nach dem Umschreiben (Dossier P4, "Sweep nachher"). */
typedef struct { uint16_t raum; int id; const char *grund; } erlaubt_t;
static const erlaubt_t k_erlaubt[] = {
    { 0x3060, 1, "unbenutzt (einziges Message_on @0x01034 = msg 9)" },
    { 0x3060, 2, "unbenutzt" }, { 0x3060, 3, "unbenutzt" }, { 0x3060, 4, "unbenutzt" },
    { 0x3060, 5, "unbenutzt" }, { 0x3060, 6, "unbenutzt" }, { 0x3060, 7, "unbenutzt" }, { 0x3060, 8, "unbenutzt" },
    { 0x6030, 2, "Sherry nennt Elza als Fremde (neu)" },
    { 0x30C1, 2, "Marvin nennt seinen Begleiter" },
    { 0x30C1, 3, "Elza: kennt ihn nicht (neu)" },
};
static const char *const k_verboten[] = {
    "How is he", "separated", "Where's Elza", "Where's Leon", "He wants you", "Your other friends",
    "Your friends are", "join your friends", "tell him that me", "Keeping the monsters at bay",
};

static int erlaubt(uint16_t raum, int id)
{
    for (size_t i = 0; i < sizeof k_erlaubt / sizeof k_erlaubt[0]; i++)
        if (k_erlaubt[i].raum == raum && k_erlaubt[i].id == id) return 1;
    return 0;
}

static int teil_sweep(void)
{
    int raeume = 0, treffer = 0, unerlaubt = 0, verboten = 0;
    for (int st = 1; st <= 6; st++)
        for (int rm = 0; rm < 0x40; rm++)
            for (int v = 0; v < 2; v++) {
                const uint16_t id = (uint16_t)((st << 12) | (rm << 4) | v);
                raum_t r;
                if (raum_laden(&r, id)) continue;
                raeume++;
                g_current_room_id = id;
                re15_msg_clear_room_block();
                if (r.rdt.messages && r.rdt.messages_size >= 4) {
                    re15_msg_load_room_block(r.rdt.messages, (int)r.rdt.messages_size);
                    const char *anderer = v ? "Leon" : "Elza";
                    for (int k = 0; k < 256; k++) {
                        const char *t = re15_msg_get_text(k);
                        if (!t) continue;
                        if (strstr(t, anderer)) {
                            treffer++;
                            int ok = erlaubt(id, k);
                            if (!ok) unerlaubt++;
                            printf("  %s ROOM%04X msg %d: %s\n", ok ? "erlaubt " : "UNERLAUBT", id, k, t);
                        }
                        for (size_t q = 0; q < sizeof k_verboten / sizeof k_verboten[0]; q++)
                            if (strstr(t, k_verboten[q]) && !erlaubt(id, k)) {
                                verboten++;
                                printf("  VERBOTEN ROOM%04X msg %d (\"%s\"): %s\n", id, k, k_verboten[q], t);
                            }
                    }
                }
                free(r.buf);
            }
    CHECK(raeume >= 200, "Sweep: %d RDTs beider Szenarien geladen", raeume);
    CHECK(unerlaubt == 0, "Sweep: %d Fundstellen des jeweils anderen Namens, alle belegt erlaubt", treffer);
    CHECK(verboten == 0, "Sweep: kein alter Begegnungs-Wortlaut mehr (%zu Muster)",
          sizeof k_verboten / sizeof k_verboten[0]);
    return 0;
}

/* ---------------------------------------------------------------------------------------------- */
static int teil_reihenfolge(void)
{
    raum_t z, s, m;
    if (raum_laden(&z, 0x1210) || raum_laden(&s, 0x1091) || raum_laden(&m, 0x6030)) { printf("FAIL: Raeume\n"); return 1; }
    for (int tuer = 0; tuer < 2; tuer++) {
        const char *weg = tuer ? "Tuer-Weg (Aufbau, dann msg-Block)" : "Boot-Weg (msg-Block, dann Aufbau)";
        hochfahren_ex(&z, -20600, 0, -21650, 2048, NULL, 0, tuer);
        CHECK(text_ist(20, zeile(0x1210, 20)->neu) && g_aot.slots[9].event_id == 20,
              "%s ROOM1210: msg 20 + Slot 9 stehen", weg);
        hochfahren_ex(&s, k_sch_x, k_sch_y, k_sch_z, k_sch_rot, NULL, 0, tuer);
        CHECK(text_ist(20, zeile(0x1091, 20)->neu) && g_aot.slots[5].event_id == 20,
              "%s ROOM1091: msg 20 + Slot 5 stehen", weg);
        hochfahren_ex(&m, -18930, 0, -22420, 0, NULL, 0, tuer);
        CHECK(text_ist(1, zeile(0x6030, 1)->neu) && text_ist(2, zeile(0x6030, 2)->neu),
              "%s ROOM6030: msg 1/2 umgeschrieben", weg);
    }
    return 0;
}

/* ---------------------------------------------------------------------------------------------- */
static int teil_mess(int argc, char **argv)
{
    /* mess <hexraum> <x0> <x1> <z0> <z1> <schritt> <y> <rot> [bank:bit] */
    if (argc < 10) { printf("mess <hexraum> <x0> <x1> <z0> <z1> <schritt> <y> <rot> [bank:bit]\n"); return 2; }
    raum_t r;
    uint16_t id = (uint16_t)strtoul(argv[2], NULL, 16);
    if (raum_laden(&r, id)) { printf("FAIL: ROOM%04X\n", id); return 1; }
    int x0 = atoi(argv[3]), x1 = atoi(argv[4]), z0 = atoi(argv[5]), z1 = atoi(argv[6]), st = atoi(argv[7]);
    int y = atoi(argv[8]); int16_t rot = (int16_t)atoi(argv[9]);
    flag_t fl = { 0, 0 }; int nfl = 0;
    if (argc > 10) { fl.bank = (uint8_t)atoi(argv[10]); const char *c = strchr(argv[10], ':');
                     if (c) { fl.bit = (uint8_t)strtoul(c + 1, NULL, 0); nfl = 1; } }
    for (int z = z0; z <= z1; z += st)
        for (int x = x0; x <= x1; x += st) {
            int mid = -2;
            int o = druck(&r, x, y, z, rot, nfl ? &fl : NULL, nfl, 60, &mid);
            int32_t lx = g_actors[0].x, lz = g_actors[0].z;
            printf("(%d,%d) rot %d -> %s msg %d  Spieler danach (%d,%d) begehbar=%d\n", x, z, rot, o ? "TEXT" : "-",
                   mid, (int)lx, (int)lz, standort_gueltig(&r, x, y, z));
        }
    return 0;
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    int rc = 0;
    if (!strcmp(teil, "mess")) return teil_mess(argc, argv);
    if (!strcmp(teil, "roy") || !strcmp(teil, "alle"))         rc |= teil_roy();
    if (!strcmp(teil, "zelle") || !strcmp(teil, "alle"))       rc |= teil_zelle();
    if (!strcmp(teil, "schuppen") || !strcmp(teil, "alle"))    rc |= teil_schuppen();
    if (!strcmp(teil, "texte") || !strcmp(teil, "alle"))       rc |= teil_texte();
    if (!strcmp(teil, "sweep") || !strcmp(teil, "alle"))       rc |= teil_sweep();
    if (!strcmp(teil, "reihenfolge") || !strcmp(teil, "alle")) rc |= teil_reihenfolge();
    printf("\n%d Pruefungen, %d Fehler\n", checks, fails);
    return (fails || rc) ? 1 : 0;
}
