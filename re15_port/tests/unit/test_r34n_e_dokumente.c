/* test_r34n_e_dokumente.c — RIEGEL Runde 34 Nacht, Spur E "Vier neue Dokumente".
 * Dossier: analysis/befunde_runde34_nacht/E_dokumente.md (5, 6.1, 9), Konstanten und Belege:
 * include/re15_dokumente.h, engine/src/re15_files.c.
 *
 * Jeder Teil faehrt den ECHTEN Raumstart des Ports (scd_room_reenter mit der ausgelieferten
 * RDT, main00/sub00-Init-Lauf, dann alle Port-Installer) — keine Nachbildung.
 *
 *   T  Tabelle: Dokument n = Item 0x48+n, Bildsatz 25+n, max_page, H, Listenname-Bytes;
 *      FILEnn_p<max> vorhanden, p<max+1> NICHT; H == Bildhoehe der Titelseite.
 *   P  Prop in BEIDEN Varianten nach dem Raumstart: obj, Ort, Drehung, Typ 0, Band 1,
 *      Eltern -1, Flags 0x000B, Nullbox. Andere Raeume: kein Dokument.
 *   Z  Zone: Slot, Rechteck (Ecken), ITEM, sat 0x31, floor 0, Item, Menge 1, Bit, obj.
 *   G  Bit (9,n) vor dem Raumstart gesetzt -> weder Prop noch Zone.
 *   D  Echter Aktionsdruck (re15_aot_scan, Pruefpunkt 620 voraus @0x80042bd0) vom gemessenen
 *      Standort -> Aufnahme-Leser (kein Item-Modal); Leser -> Schliessen -> Meldung ->
 *      Bestaetigen: FILE-Liste traegt n, Bit gesetzt, Zone aus, Prop aus.
 *      Gegenprobe Blick weg -> nichts.
 *   S  Speicher-Rundlauf (v9): files[] und Bits ueber capture/restore gleich.
 *   V  Sicht: Prop-Mitte im Nutzer-Cut im Bild und NICHT vom Regions-Test verworfen
 *      (re15_prop_culled = FUN_8002c18c -> FUN_80014368 gegen den Anker des Cuts).
 *   B  Eingebackene Modelle = die RE2-Quellen (md5 in tools/r34n_e/dokumente_engine_export.py):
 *      Groesse, parsebar, CLUT-Zeile der Dreiecke liegt in der TIM (Mehrzeilen-CLUT).
 *
 *   M  Marke: die Prop-Mitte liegt im Nutzer-Cut in der roten Marke des Nutzerbilds.
 *   K  Tiefen-Klemmen: je (Raum, Cut) = kleinste Tiefe der Original-Masken ueber der Mitte, ohne
 *      Klemme verdeckt (re15_pri_mask_occludes), mit Klemme davor; sonst Irons-Wert durchgereicht.
 *
 * Dokument-spezifisch:
 *   1  ROOM1051 (Elza): der Leichen-Satz Slot 11 (sce 3, @0x00C0E) fuehrt; nach Bit (9,165)
 *      legt sub01 @0x00CB2 ihn still und der Druck liefert das Tagebuch.
 *   3  ROOM1020/1021: die Tisch-Nachricht (Slot 10 @0x01F0E / 11 @0x01F6C) zieht fuer die
 *      Liegezeit nach Slot 15 (Original-Regel "Gegenstand im kleineren Slot"); mit Bit (9,59)
 *      bleibt sie; Satz-Waechter; nach dem Aufheben liefert derselbe Druck wieder die Nachricht.
 *   4  ROOM1010/1011: wo sich Spray-Zone (Slot 2 @0x00996) und Blatt-Zone decken, gibt der
 *      kleinere Slot das Spray (Item-Modal); ausserhalb der Spray-Zone der Leser.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "re15_dokumente.h"
#include "re15_rdt.h"
#include "re15_camera.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_aot.h"
#include "re15_collision.h"
#include "re15_item_modal.h"
#include "re15_inventory.h"
#include "re15_menu.h"
#include "re15_inv_screen.h"
#include "re15_files.h"
#include "re15_fade.h"
#include "re15_player.h"
#include "re15_pri.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_savedata.h"
#include "re15_skeleton.h"

extern re15_aot_state_t g_aot;
extern uint8_t          g_aot_action_pressed;

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  PASS: " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *datei(const char *pfad, size_t *n)
{
    FILE *f = fopen(pfad, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *n = (size_t)sz;
    return b;
}

typedef struct { uint8_t *buf; size_t n; re15_rdt_t rdt; uint16_t id; } raum_t;

static int raum_laden(raum_t *r, uint16_t id)
{
    char p[600];
    snprintf(p, sizeof p, "%s/STAGE%X/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(id >> 12), (unsigned)id);
    r->id = id;
    r->buf = datei(p, &r->n);
    if (!r->buf) { printf("FAIL: RDT nicht lesbar: %s\n", p); return -1; }
    if (re15_rdt_parse(r->buf, r->n, &r->rdt) != 0) { printf("FAIL: parse %s\n", p); return -1; }
    return 0;
}

/* Raumstart = der Tuerweg des Ports (wie test_r30_irons_tisch.c hochfahren). bits: Bank 9. */
static void hochfahren(raum_t *r, int32_t px, int32_t pz, const int *bits)
{
    re15_game_state_init();
    re15_inv_init();
    re15_files_reset();
    re15_aot_init();
    scd_vm_init(); re15_actor_init();
    for (; bits && *bits; bits++) re15_game_flag_set(9, (uint8_t)*bits, 1);
    g_current_room_id = r->id;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = px; pl->y = 0; pl->z = pz; pl->rot_y = 0; pl->state = 1;
    g_scd.player_mode = 0;
    { extern void re15_msg_load_room_block(const uint8_t *b, int n);
      re15_msg_load_room_block(r->rdt.messages, r->rdt.messages_size); }
    scd_register_room_events(&r->rdt);
    scd_room_reenter(&r->rdt, px, pz, 0);
}

static int slot_von(uint8_t oid)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++) if (g_scd.props[k].obj_id == oid) return k;
    return -1;
}

/* Soll-Daten eines Dokuments (unabhaengig vom Kopf aufgeschrieben: der Riegel soll eine
 * Aenderung am Kopf bemerken). */
typedef struct {
    int nr; uint16_t raum; uint8_t item, bit, slot, obj, bildsatz, max_page, h;
    int32_t x, y, z; int16_t rot;
    int32_t rx, rz, rw, rd;                /* Rechteck Ecke/Groesse */
    const char *name;                      /* Listenname (ASCII) */
    int32_t stand_x, stand_z; int16_t stand_rot;   /* gemessener Druck-Standort (Dossier 3.4) */
    int user_cut;                          /* Cut des Nutzerbilds bzw. der Leiche */
    int md1_n, tim_n;
    int mx0, mx1, my0, my1;                /* rote Marke im Nutzerbild (Cut user_cut), 0 = keine */
} soll_t;

static const soll_t k_soll[] = {
    { 1, 0x1050, 0x49, 57, 15, 2, 26, 3, 144, 16474, -360, -6592, 0,
      15250, -7250, 1000, 1000, "Police Officer's Final Diary Entry",
      14900, -6750, 0, 3, 372, 34848, 0, 0, 0, 0 },
    { 2, 0x1000, 0x4A, 58, 10, 2, 27, 4, 144, 19226, -398, -11723, 0,
      18726, -12223, 1000, 1000, "Elliot's Diary",
      18250, -11723, 0, 0, 372, 17440, 182, 203, 162, 182 },      /* elliot.bmp, marken.py */
    { 3, 0x1020, 0x4B, 59, 14, 7, 28, 2, 176, -9975, -1410, -16428, 0,
      -11100, -16928, 2200, 1000, "Marvin's Notes",
      -11500, -16428, 0, 6, 404, 34848, 151, 161, 101, 109 },     /* marvin.bmp, marken.py */
    { 4, 0x1010, 0x4C, 60, 9, 3, 29, 2, 176, 450, -1600, 5600, 3840,
      -50, 5100, 1000, 1000, "Armory Notice",
      700, 6400, 1024, 0, 404, 34848, 0, 0, 0, 0 },     /* Marke: siehe teil_dok4 (Blatt NEBEN der Dose) */
};
#define N_SOLL ((int)(sizeof k_soll / sizeof k_soll[0]))

static int prop_ok(const soll_t *s)
{
    int k = slot_von(s->obj);
    if (k < 0) return 0;
    const __typeof__(g_scd.props[0]) *p = &g_scd.props[k];
    return p->active == 1 && p->obj_type == 0 && p->band == 1 && p->parent_obj == -1 &&
           p->x == s->x && p->y == s->y && p->z == s->z && p->rot_x == 0 && p->rot_y == s->rot &&
           p->rot_z == 0 && p->flags == 0x000B && p->box_hx == 0 && p->box_hy == 0 &&
           p->box_hz == 0;
}

static int zone_ok(const soll_t *s)
{
    const re15_aot_t *a = &g_aot.slots[s->slot];
    const re15_aot_item_params_t *p = &g_aot.item_params[s->slot];
    return a->active && a->type == RE15_AOT_TYPE_ITEM && a->sce_flags == 0x31 && a->band == 0 &&
           a->x - a->half_w == s->rx && a->x + a->half_w == s->rx + s->rw &&
           a->z - a->half_h == s->rz && a->z + a->half_h == s->rz + s->rd &&
           p->item_type == s->item && p->amount == 1 && p->taken_bit == s->bit &&
           p->taken_prop == s->obj;
}

static void kodiere(const char *name, uint8_t *out, int *n)
{
    int k = 0;
    for (; *name; name++)
        out[k++] = (*name == ' ') ? 0x00 : (*name == '\'') ? 0x3A : (uint8_t)(*name - 0x24);
    out[k++] = 0x07;
    *n = k;
}

/* ------------------------------------------------------------------ T */
static void teil_t(const soll_t *s)
{
    printf("\n[T] Dokument %d: Tabelle und Bildsatz FILE%02d\n", s->nr, s->bildsatz);
    const re15_file_doc_t *d = re15_files_doc(s->nr);
    CHECK(d && d->item_id == s->item && d->bildsatz == s->bildsatz && d->max_page == s->max_page &&
          d->page_h == s->h,
          "T Dok %d: Item 0x%02X, Bildsatz %d, max_page %d, H %d", s->nr,
          d ? d->item_id : 0, d ? d->bildsatz : 0, d ? d->max_page : 0, d ? d->page_h : 0);
    CHECK(re15_files_doc_from_item(s->item) == s->nr, "T Item 0x%02X -> Dokument %d", s->item,
          re15_files_doc_from_item(s->item));
    uint8_t soll[64]; int n = 0;
    kodiere(s->name, soll, &n);
    int gleich = d && d->name && memcmp(d->name, soll, (size_t)n) == 0;
    CHECK(gleich, "T Listenname \"%s\" (%d Bytes, Kodierung wie @0x800c4e04)", s->name, n);
    char p[600]; size_t sz = 0; uint8_t *b;
    snprintf(p, sizeof p, "%s/FILES/FILE%02d_p%02d_page.TIM", RE15_ASSET_RE2_DIR, s->bildsatz, s->max_page);
    b = datei(p, &sz);
    int da = b != NULL; free(b);
    snprintf(p, sizeof p, "%s/FILES/FILE%02d_p%02d_page.TIM", RE15_ASSET_RE2_DIR, s->bildsatz, s->max_page + 1);
    b = datei(p, &sz);
    int weg = b == NULL; free(b);
    CHECK(da && weg, "T FILE%02d_p%02d vorhanden, p%02d nicht (max_page %d gemessen am Satz)",
          s->bildsatz, s->max_page, s->max_page + 1, s->max_page);
    snprintf(p, sizeof p, "%s/FILES/FILE%02d_title_page.TIM", RE15_ASSET_RE2_DIR, s->bildsatz);
    b = datei(p, &sz);
    re15_tim_t t; memset(&t, 0, sizeof t);
    int hok = b && re15_tim_parse(b, (int)sz, &t) == 0 && t.height == s->h;
    CHECK(hok, "T Titelseite FILE%02d: Bildhoehe %d = H", s->bildsatz, b ? t.height : -1);
    free(b);
}

/* ------------------------------------------------------------------ P / Z / G */
static void teil_pzg(const soll_t *s, raum_t *rr[2], raum_t *fremd)
{
    printf("\n[P/Z/G] Dokument %d: Prop und Zone nach dem Raumstart\n", s->nr);
    for (int i = 0; i < 2; i++) {
        static int bits[2]; bits[0] = s->bit; bits[1] = 0;
        hochfahren(rr[i], s->stand_x, s->stand_z, bits);
        int pool_ohne = (int)g_scd.prop_count;
        hochfahren(rr[i], s->stand_x, s->stand_z, NULL);
        CHECK(prop_ok(s), "P ROOM%04X obj %d bei (%d,%d,%d) rot %d, Flags 0x000B, Band 1, Nullbox",
              rr[i]->id, s->obj, s->x, s->y, s->z, s->rot);
        CHECK((int)g_scd.prop_count == pool_ohne + 1 && slot_von(s->obj) == pool_ohne,
              "P ROOM%04X Pool %d Eintraege = Raum-Props %d + Dokument (hinten angehaengt)",
              rr[i]->id, (int)g_scd.prop_count, pool_ohne);
        CHECK(zone_ok(s), "Z ROOM%04X Slot %d: ITEM x[%d..%d] z[%d..%d] Item 0x%02X x1 Bit %d obj %d, "
              "sat 0x31, floor 0", rr[i]->id, s->slot, s->rx, s->rx + s->rw, s->rz, s->rz + s->rd,
              s->item, s->bit, s->obj);
        hochfahren(rr[i], s->stand_x, s->stand_z, bits);
        CHECK(slot_von(s->obj) < 0 &&
              !(g_aot.slots[s->slot].active && g_aot.slots[s->slot].type == RE15_AOT_TYPE_ITEM &&
                g_aot.item_params[s->slot].item_type == s->item),
              "G ROOM%04X Bit (9,%d) gesetzt: weder Prop noch Zone", rr[i]->id, s->bit);
    }
    hochfahren(fremd, 0, 0, NULL);
    int fremd_zone = 0;
    for (int k = 0; k < RE15_AOT_MAX; k++)
        if (g_aot.slots[k].active && g_aot.slots[k].type == RE15_AOT_TYPE_ITEM &&
            g_aot.item_params[k].item_type == s->item) fremd_zone = 1;
    CHECK(re15_dokumente_obj_id(fremd->id) < 0 && !fremd_zone,
          "P ROOM%04X (fremd): kein Dokument, keine Zone mit Item 0x%02X", fremd->id, s->item);
}

/* ------------------------------------------------------------------ D */
static void menue_bild(uint16_t pressed, uint16_t held)
{
    re15_menu_start_poll(pressed, 1);
    if (re15_menu_gameplay_frozen())
        re15_menu_fsm_tick(pressed, held);
    if (re15_menu_is_open())
        re15_inv_screen_ecg_tick();
    re15_fade_tick();
}

/* Raum hochfahren, einschwingen, Meldungen weg, dann EIN Aktionsdruck ueber re15_aot_scan. */
static void druck(raum_t *r, int32_t x, int32_t z, int16_t rot, const int *bits)
{
    hochfahren(r, x, z, bits);
    re15_collision_ensure_band(0);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int f = 0; f < 12; f++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); }
    g_scd.message_display_frames = 0; g_scd.message_query = 0; g_scd.message_active = 0;
    re15_pauseflags_clear();
    pl->x = x; pl->z = z; pl->rot_y = rot;
    for (int f = 0; f < 3; f++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); }
    g_aot.fired_event_id_this_frame = 0;
    g_aot_action_pressed = 1;
    scd_vm_tick();
    re15_aot_scan(pl->x, pl->z, 0);
    g_aot_action_pressed = 0;
}

/* Leser -> Schliessen -> Meldung -> Bestaetigen -> Menue zu, ohne Pruefungen (wie
 * test_r30_irons_tisch.c leser_abschliessen): der Menue-Zustand ist prozessweit, ein nur
 * angeforderter Leser bliebe sonst fuer den naechsten Druck "aktiv". */
static void leser_abschliessen(void)
{
    int n = 0;
    while (n++ < 300 && !(re15_menu_phase() == 1 && g_inv_screen.item_state == 3))
        menue_bild(0, 0);
    menue_bild(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
    uint8_t mid = 0; int rev = -1, total = re15_menu_doc_msg_total();
    n = 0;
    while (n++ < 400 && re15_menu_doc_msg(&mid, &rev) && rev < total) menue_bild(0, 0);
    menue_bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    n = 0;
    while (n++ < 200 && (re15_menu_is_open() || re15_menu_stage() != 0)) menue_bild(0, 0);
}

static void teil_d(const soll_t *s, raum_t *r)
{
    printf("\n[D] Dokument %d: Aufheben mit der Aktionstaste (ROOM%04X)\n", s->nr, r->id);
    druck(r, s->stand_x, s->stand_z, s->stand_rot, NULL);
    CHECK(!re15_item_modal_active() && re15_menu_doc_active() && re15_menu_stage() != 0,
          "D Stand (%d,%d) rot %d -> KEIN Item-Modal, Aufnahme-Leser angefordert",
          s->stand_x, s->stand_z, s->stand_rot);
    int bild = 0;
    while (bild < 300 && !(re15_menu_phase() == 1 && g_inv_screen.item_state == 3)) {
        menue_bild(0, 0); bild++;
    }
    CHECK(g_inv_screen.item_state == 3 && g_inv_screen.file_reader_page == 0 &&
          re15_files_get(0) == s->nr,
          "D Leser liest nach %d Bildern (Titelseite), FILE-Platz 0 = Dokument %d", bild,
          re15_files_get(0));
    CHECK(g_inv_screen.file_bildsatz == s->bildsatz && g_inv_screen.file_end == s->max_page + 1,
          "D Leser zeigt Bildsatz %d mit %d Seiten (Soll %d / max_page+1 = %d)",
          (int)g_inv_screen.file_bildsatz, (int)g_inv_screen.file_end, s->bildsatz, s->max_page + 1);
    CHECK(!re15_game_flag_get(9, s->bit) && g_aot.slots[s->slot].active &&
          g_scd.props[slot_von(s->obj)].active,
          "D beim Lesen liegt das Dokument noch (Zone, Prop, Bit frei)");
    menue_bild(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
    uint8_t mid = 0; int rev = -1, total = re15_menu_doc_msg_total(), n = 0;
    while (n < 400 && re15_menu_doc_msg(&mid, &rev) && rev < total) { menue_bild(0, 0); n++; }
    CHECK(mid == s->item && rev == total && g_aot.slots[s->slot].active,
          "D Meldung \"filed\" steht (Id 0x%02X), noch nichts abgeraeumt", mid);
    menue_bild(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(re15_game_flag_get(9, s->bit) && !g_aot.slots[s->slot].active &&
          !g_scd.props[slot_von(s->obj)].active,
          "D Bestaetigen -> Bit (9,%d), Zone %d aus, obj %d aus", s->bit, s->slot, s->obj);
    int zu = 0;
    while (zu < 200 && (re15_menu_is_open() || re15_menu_stage() != 0)) { menue_bild(0, 0); zu++; }
    CHECK(!re15_menu_is_open() && re15_files_get(0) == s->nr && re15_files_count() == 1,
          "D Menue zu nach %d Bildern, Dokument %d bleibt in der FILE-Liste", zu, s->nr);

    /* Gegenprobe: Blick weg (Gegenrichtung) -> nichts */
    druck(r, s->stand_x, s->stand_z, (int16_t)((s->stand_rot + 2048) & 4095), NULL);
    CHECK(!re15_item_modal_active() && !re15_menu_doc_active(),
          "D Gegenprobe Blick rot %d (weg vom Dokument): nichts", (s->stand_rot + 2048) & 4095);
}

/* ------------------------------------------------------------------ S */
static void teil_s(const soll_t *s)
{
    printf("\n[S] Dokument %d: Speicher-Rundlauf\n", s->nr);
    re15_savedata_t sd; uint16_t room = 0;
    re15_game_state_init();
    re15_files_reset();
    re15_files_add(s->nr);
    re15_game_flag_set(9, s->bit, 1);
    g_current_room_id = s->raum;
    re15_savedata_capture(&sd, 1, 1);
    re15_files_reset();
    re15_game_flag_set(9, s->bit, 0);
    int ok = re15_savedata_restore(&sd, &room) == 0;
    CHECK(ok && re15_files_get(0) == s->nr && re15_files_count() == 1 &&
          re15_game_flag_get(9, s->bit) && sd.files[0] == s->nr,
          "S capture/restore: files[0] = %d, Bit (9,%d) = %d", re15_files_get(0), s->bit,
          re15_game_flag_get(9, s->bit));
}

/* ------------------------------------------------------------------ V */
static double proj(const re15_camera_view_t *v, double x, double y, double z,
                   double *sx, double *sy)
{
    double vx = (v->rot[0]*x + v->rot[1]*y + v->rot[2]*z) / 4096.0 + v->trans[0];
    double vy = (v->rot[3]*x + v->rot[4]*y + v->rot[5]*z) / 4096.0 + v->trans[1];
    double vz = (v->rot[6]*x + v->rot[7]*y + v->rot[8]*z) / 4096.0 + v->trans[2];
    *sx = 160.0 + v->fov_screen_dist * vx / vz;
    *sy = 120.0 + v->fov_screen_dist * vy / vz;
    return vz;
}

static int sichtbar_im_cut(raum_t *r, const soll_t *s, int cut, double *sx, double *sy)
{
    re15_camera_view_t v;
    if (re15_camera_build_view(&r->rdt.cuts[cut], &v) != 0) return 0;
    double vz = proj(&v, s->x, s->y, s->z, sx, sy);
    int16_t xs[4], zs[4];
    int reg = re15_rdt_get_region_quad(&r->rdt, cut, xs, zs);
    int culled = re15_prop_culled(0, s->x, s->z, reg, xs, zs);
    return vz > 0 && *sx >= 0 && *sx < 320 && *sy >= 0 && *sy < 240 && !culled;
}

static void teil_v(const soll_t *s, raum_t *rr[2])
{
    printf("\n[V] Dokument %d: im Nutzer-Cut %d zu sehen\n", s->nr, s->user_cut);
    for (int i = 0; i < 2; i++) {
        double sx = -1, sy = -1;
        int ok = sichtbar_im_cut(rr[i], s, s->user_cut, &sx, &sy);
        CHECK(ok, "V ROOM%04X Cut %d: Mitte (%.1f ; %.1f) im Bild, Regions-Test (Anker des Cuts, "
              "FUN_80014368) behaelt das Prop", rr[i]->id, s->user_cut, sx, sy);
        if (s->mx1 > 0)
            CHECK(sx >= s->mx0 && sx <= s->mx1 + 1 && sy >= s->my0 && sy <= s->my1 + 1,
                  "M ROOM%04X Cut %d: Mitte (%.2f ; %.2f) in der roten Marke x%d..%d y%d..%d",
                  rr[i]->id, s->user_cut, sx, sy, s->mx0, s->mx1, s->my0, s->my1);
    }
}

/* ------------------------------------------------------------------ K */
typedef struct { uint16_t raum; int cut; int tiefe; int nr; } klemme_soll_t;
static const klemme_soll_t k_klemmen[] = {
    { 0x1020, 3, 258, 3 }, { 0x1021, 3, 258, 3 },     /* Original-Maske 35 @0xD28 (Tischplatte) */
    { 0x1010, 0, 40, 4 }, { 0x1010, 1, 53, 4 },       /* nachgezeichnete Masken MASKS/ROOM1010.MSK */
    { 0x1010, 6, 60, 4 }, { 0x1010, 8, 51, 4 },
};
#define N_KLEMMEN ((int)(sizeof k_klemmen / sizeof k_klemmen[0]))

static const soll_t *soll_nr(int nr)
{
    for (int i = 0; i < N_SOLL; i++) if (k_soll[i].nr == nr) return &k_soll[i];
    return NULL;
}

/* Masken eines Cuts wie platform/pc/main.c: Original-Sektion, nur bei NULL die nachgezeichneten
 * Seitendaten MASKS/ROOM%04X.MSK (R15M). *herkunft: 1 = Original, 2 = nachgezeichnet, 0 = keine. */
static int masken_des_cuts(raum_t *r, int cut, re15_pri_cut_t *pri, int *herkunft)
{
    memset(pri, 0, sizeof *pri);
    *herkunft = 0;
    int n = re15_pri_parse_section(r->buf, r->n, r->rdt.cuts[cut].pri_offset, pri);
    if (n > 0) { *herkunft = 1; return n; }
    char p[600]; size_t ms = 0;
    snprintf(p, sizeof p, "%s/MASKS/ROOM%04X.MSK", RE15_ASSET_PSX_DIR, (unsigned)r->id);
    uint8_t *mb = datei(p, &ms);
    if (!mb) return 0;
    uint32_t off = re15_pri_msk_section_offset(mb, ms, cut);
    if (off) n = re15_pri_parse_section(mb, ms, off, pri);
    free(mb);
    if (n > 0) *herkunft = 2;
    return n;
}

/* Huelle des Dokument-Modells (bbox der eingebetteten MD1, gedreht wie pc_prop_rot_q12 = reines
 * rot_y: Modell (mx,my,mz) -> Welt (c*mx + s*mz, my, -s*mx + c*mz)) im Cut: Bild-bbox und vz-Bereich.
 * Rueckgabe: kleinste Tiefe unter den Masken, die die Huelle schneiden und ihre FERNSTE Ecke
 * verdecken (re15_pri_mask_occludes = otz>>4 @0x8002565c gegen die Tiefe), sonst 99999. */
static int huelle_klemme(raum_t *r, const soll_t *s, int cut, int *herkunft, long *vmax_out)
{
    static re15_md1_t md;
    int n = 0;
    const uint8_t *m = re15_dokumente_md1_bytes(s->raum, &n);
    *herkunft = 0; *vmax_out = 0;
    if (!m || re15_md1_parse(m, (size_t)n, &md) != 0) return -1;
    int lo[3] = { 1 << 30, 1 << 30, 1 << 30 }, hi[3] = { -(1 << 30), -(1 << 30), -(1 << 30) };
    for (int i = 0; i < md.mesh_count; i++) {
        const re15_md1_mesh_t *hm = &md.meshes[i];
        for (int k = 0; k < hm->tri_vertex_count; k++) {
            int v[3] = { hm->tri_vertices[k].x, hm->tri_vertices[k].y, hm->tri_vertices[k].z };
            for (int a = 0; a < 3; a++) { if (v[a] < lo[a]) lo[a] = v[a]; if (v[a] > hi[a]) hi[a] = v[a]; }
        }
        for (int k = 0; k < hm->quad_vertex_count; k++) {
            int v[3] = { hm->quad_vertices[k].x, hm->quad_vertices[k].y, hm->quad_vertices[k].z };
            for (int a = 0; a < 3; a++) { if (v[a] < lo[a]) lo[a] = v[a]; if (v[a] > hi[a]) hi[a] = v[a]; }
        }
    }
    re15_camera_view_t v;
    if (re15_camera_build_view(&r->rdt.cuts[cut], &v) != 0) return -1;
    int32_t c = re15_cos_q12(s->rot), sn = re15_sin_q12(s->rot);
    double x0 = 1e9, x1 = -1e9, y0 = 1e9, y1 = -1e9; long vmin = 1L << 30, vmax = -(1L << 30);
    for (int e = 0; e < 8; e++) {
        long mx = (e & 1) ? hi[0] : lo[0], my = (e & 2) ? hi[1] : lo[1], mz = (e & 4) ? hi[2] : lo[2];
        double wx = s->x + (c * mx + sn * mz) / 4096.0, wy = s->y + my,
               wz = s->z + (-sn * mx + c * mz) / 4096.0, sx, sy;
        long vz = (long)proj(&v, wx, wy, wz, &sx, &sy);
        if (vz <= 0) return -1;
        if (sx < x0) x0 = sx;
        if (sx > x1) x1 = sx;
        if (sy < y0) y0 = sy;
        if (sy > y1) y1 = sy;
        if (vz < vmin) vmin = vz;
        if (vz > vmax) vmax = vz;
    }
    *vmax_out = vmax;
    static re15_pri_cut_t pri;
    int nm = masken_des_cuts(r, cut, &pri, herkunft);
    int tief = 99999;
    for (int k = 0; k < nm && k < pri.mask_count; k++) {
        const re15_pri_mask_t *q = &pri.masks[k];
        int dx = (int16_t)q->dstX, dy = (int16_t)q->dstY;
        if (dx + q->width <= x0 || dx > x1 || dy + q->height <= y0 || dy > y1) continue;
        if (re15_pri_mask_occludes(q->depth, vmax) && q->depth < tief) tief = q->depth;
    }
    return tief;
}

static void teil_k(raum_t *alle[], int n_alle)
{
    printf("\n[K] Tiefen-Klemmen\n");
    for (int k = 0; k < N_KLEMMEN; k++) {
        const klemme_soll_t *kl = &k_klemmen[k];
        const soll_t *s = soll_nr(kl->nr);
        raum_t *r = NULL;
        for (int i = 0; i < n_alle; i++) if (alle[i]->id == kl->raum) r = alle[i];
        if (!s || !r) { CHECK(0, "K Raum %04X nicht geladen", kl->raum); continue; }
        int herkunft = 0; long vmax = 0;
        int tief = huelle_klemme(r, s, kl->cut, &herkunft, &vmax);
        CHECK(tief == kl->tiefe,
              "K ROOM%04X Cut %d: kleinste Tiefe der Masken (%s), die die fernste Ecke der Huelle verdecken "
              "= %d (Soll %d, vz max %ld)", kl->raum, kl->cut,
              herkunft == 1 ? "Original" : herkunft == 2 ? "nachgezeichnet MSK" : "keine", tief, kl->tiefe, vmax);
        int km = re15_dokumente_sort_max_mit(-1, kl->raum, kl->cut, s->obj);
        CHECK(km == (int)re15_pri_mask_camera_z(kl->tiefe) - 1 && (float)km < re15_pri_mask_camera_z(kl->tiefe),
              "K ROOM%04X Cut %d obj %d: Klemme %d = re15_pri_mask_camera_z(%d) - 1", kl->raum, kl->cut,
              s->obj, km, kl->tiefe);
        CHECK(re15_dokumente_sort_max_mit(-1, kl->raum, kl->cut, s->obj + 1) == -1 &&
              re15_dokumente_sort_max_mit(4711, kl->raum, kl->cut, s->obj + 1) == 4711,
              "K ROOM%04X Cut %d: anderes obj -> keine Klemme, Irons-Wert durchgereicht", kl->raum, kl->cut);
    }
    const soll_t *s3 = soll_nr(3);
    if (s3)
        CHECK(re15_dokumente_sort_max_mit(-1, 0x1020, 6, s3->obj) == -1 &&
              re15_dokumente_sort_max_mit(-1, 0x1020, 5, s3->obj) == -1,
              "K ROOM1020 Cut 6 (Nutzerbild) und Cut 5: keine Klemme");
    /* Cuts OHNE Klemme, in denen das Dokument zu sehen ist: keine Maske verdeckt seine Huelle. */
    static const struct { uint16_t raum; int cut; int nr; } k_frei[] = {
        { 0x1050, 3, 1 }, { 0x1051, 3, 1 }, { 0x1000, 0, 2 }, { 0x1001, 0, 2 },
        { 0x1020, 6, 3 }, { 0x1021, 6, 3 }, { 0x1011, 0, 4 }, { 0x1011, 1, 4 },
    };
    for (int k = 0; k < (int)(sizeof k_frei / sizeof k_frei[0]); k++) {
        const soll_t *s = soll_nr(k_frei[k].nr);
        raum_t *r = NULL;
        for (int i = 0; i < n_alle; i++) if (alle[i]->id == k_frei[k].raum) r = alle[i];
        if (!s || !r) { CHECK(0, "K Raum %04X nicht geladen", k_frei[k].raum); continue; }
        int herkunft = 0; long vmax = 0;
        int tief = huelle_klemme(r, s, k_frei[k].cut, &herkunft, &vmax);
        CHECK(tief == 99999 && re15_dokumente_sort_max_mit(-1, k_frei[k].raum, k_frei[k].cut, s->obj) == -1,
              "K ROOM%04X Cut %d: keine Maske (%s) verdeckt die Huelle, keine Klemme", k_frei[k].raum,
              k_frei[k].cut, herkunft == 1 ? "Original" : herkunft == 2 ? "nachgezeichnet" : "keine");
    }
    CHECK(re15_dokumente_sort_max_mit(5636, 0x1150, 2, 5) == 5636 &&
          re15_dokumente_sort_max_mit(-1, 0x1150, 2, 5) == -1,
          "K ROOM1150 (Irons): Wert unveraendert durchgereicht");
}

/* ------------------------------------------------------------------ B */
static void teil_b(const soll_t *s)
{
    printf("\n[B] Dokument %d: eingebackenes Modell\n", s->nr);
    static re15_md1_t md;
    re15_tim_t t; memset(&t, 0, sizeof t);
    int n = 0, tn = 0;
    const uint8_t *m = re15_dokumente_md1_bytes(s->raum, &n);
    const uint8_t *tb = re15_dokumente_tim_bytes((uint16_t)(s->raum | 1), &tn);
    int mok = m && n == s->md1_n && re15_md1_parse(m, (size_t)n, &md) == 0 && md.mesh_count >= 1;
    int tok = tb && tn == s->tim_n && re15_tim_parse(tb, tn, &t) == 0 && t.has_clut;
    CHECK(mok && tok, "B MD1 %d B / TIM %d B (beide Varianten), parsebar", n, tn);
    /* CLUT-Zeile jedes Dreiecks/Vierecks muss in der TIM liegen: render_pc.c waehlt
     * clut_idx = clut_y - tim.clut_y; ausserhalb faellt es auf Zeile 0 zurueck (falsche Farbe). */
    int row_w = (t.bpp == 4) ? 16 : 256, n_cluts = tok ? t.clut_entries / row_w : 0, schlecht = 0,
        gesehen = 0, zeile = -1;
    for (int mi = 0; mok && tok && mi < md.mesh_count; mi++) {
        const re15_md1_mesh_t *hm = &md.meshes[mi];
        for (int k = 0; k < hm->triangle_count; k++) {
            int cy = (hm->triangle_uvs[k].clut >> 6) & 0x1FF, idx = cy - t.clut_y;
            gesehen++; zeile = idx;
            if (idx < 0 || idx >= n_cluts) schlecht++;
        }
        for (int k = 0; k < hm->quad_count; k++) {
            int cy = (hm->quad_uvs[k].clut >> 6) & 0x1FF, idx = cy - t.clut_y;
            gesehen++; zeile = idx;
            if (idx < 0 || idx >= n_cluts) schlecht++;
        }
    }
    CHECK(gesehen > 0 && schlecht == 0, "B %d Flaechen, CLUT-Zeile %d von %d, %d ausserhalb",
          gesehen, zeile, n_cluts, schlecht);
}

/* ------------------------------------------------------------------ Dok 1: ROOM1051 */
static void teil_dok1_elza(raum_t *r1051)
{
    const soll_t *s = &k_soll[0];
    printf("\n[1] Dokument 1 in ROOM1051 (Elza): Leichen-Satz fuehrt\n");
    druck(r1051, s->stand_x, s->stand_z, s->stand_rot, NULL);
    CHECK(!re15_menu_doc_active() && !re15_item_modal_active() &&
          g_aot.fired_event_id_this_frame == 3,
          "1 ROOM1051 ohne Bit (9,165): Druck feuert den Leichen-Satz Slot 11 (event %d, Soll 3 "
          "= sub03 @0x00DA4), KEIN Leser", (int)g_aot.fired_event_id_this_frame);
    static const int waffe[] = { 165, 0 };
    druck(r1051, s->stand_x, s->stand_z, s->stand_rot, waffe);
    CHECK(g_aot.slots[11].active == 0 || g_aot.slots[11].sce_flags == 0,
          "1 ROOM1051 mit Bit (9,165): sub01 @0x00CB2 hat Slot 11 stillgelegt (sat 0x%02X)",
          (unsigned)g_aot.slots[11].sce_flags);
    CHECK(re15_menu_doc_active() && !re15_item_modal_active(),
          "1 ROOM1051 mit Bit (9,165): derselbe Druck -> Aufnahme-Leser (Dokument 1)");
    leser_abschliessen();
    CHECK(!re15_menu_doc_active() && re15_files_get(0) == 1 && re15_game_flag_get(9, 57),
          "1 ROOM1051 Leser abgeschlossen: FILE-Platz 0 = Dokument 1, Bit (9,57)");
}

/* ------------------------------------------------------------------ Dok 3: Tisch-Nachricht */
static int nachricht_in(int slot, int msg)
{
    const re15_aot_t *a = &g_aot.slots[slot];
    return a->active && a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == msg &&
           a->sce_flags == 0x31 && a->pause_mask16 == 0xFFFF &&
           a->x - a->half_w == -11100 && a->x + a->half_w == -8900 &&
           a->z - a->half_h == -18100 && a->z + a->half_h == -14800;
}

static void teil_dok3_nachricht(raum_t *r1020, raum_t *r1021)
{
    const soll_t *s = soll_nr(3);
    raum_t *rr[2] = { r1020, r1021 };
    static const int quelle[2] = { 10, 11 }, msg[2] = { 3, 12 };
    static const int bit59[] = { 59, 0 };
    printf("\n[3] Dokument 3: Tisch-Nachricht und Vorrang\n");
    for (int i = 0; i < 2; i++) {
        hochfahren(rr[i], s->stand_x, s->stand_z, NULL);
        CHECK(nachricht_in(15, msg[i]) && !g_aot.slots[quelle[i]].active,
              "3 ROOM%04X Bit (9,59) frei: Nachricht %d in Slot 15, Slot %d frei", rr[i]->id, msg[i],
              quelle[i]);
        hochfahren(rr[i], s->stand_x, s->stand_z, bit59);
        CHECK(nachricht_in(quelle[i], msg[i]) && !g_aot.slots[15].active,
              "3 ROOM%04X Bit (9,59) gesetzt: Nachricht %d bleibt in Slot %d, Slot 15 frei", rr[i]->id,
              msg[i], quelle[i]);
        /* Satz-Waechter: veraenderter Satz (andere Nachricht) -> kein Umzug */
        g_aot.slots[quelle[i]].event_id = (uint8_t)(msg[i] + 1);
        re15_game_flag_set(9, 59, 0);
        re15_dokumente_install(rr[i]->id);
        CHECK(!g_aot.slots[15].active && g_aot.slots[quelle[i]].active &&
              g_aot.slots[quelle[i]].event_id == msg[i] + 1 && zone_ok(s),
              "3 ROOM%04X Waechter: Satz mit Nachricht %d zieht NICHT um (Dokument-Zone trotzdem da)",
              rr[i]->id, msg[i] + 1);
        /* Druck vom Dokument-Standort */
        CHECK(!re15_menu_doc_active(), "3 ROOM%04X vor dem Druck kein Leser offen", rr[i]->id);
        druck(rr[i], s->stand_x, s->stand_z, s->stand_rot, NULL);
        CHECK(re15_menu_doc_active() && !g_scd.message_active,
              "3 ROOM%04X Druck (%d,%d) Blick +x -> Leser (die Nachricht in Slot 15 kommt danach)",
              rr[i]->id, s->stand_x, s->stand_z);
        leser_abschliessen();
        CHECK(!re15_menu_doc_active() && !g_aot.slots[s->slot].active && nachricht_in(15, msg[i]),
              "3 ROOM%04X nach dem Aufheben: Dokument-Zone aus, Nachricht %d weiter in Slot 15",
              rr[i]->id, msg[i]);
        g_scd.message_display_frames = 0; g_scd.message_query = 0; g_scd.message_active = 0;
        re15_pauseflags_clear();
        {
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            for (int f = 0; f < 3; f++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); }
            g_aot_action_pressed = 1; scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0);
            g_aot_action_pressed = 0;
        }
        CHECK(!re15_menu_doc_active() && g_scd.message_active && g_scd.message_id == msg[i],
              "3 ROOM%04X im selben Raumbesuch: zweiter Druck -> Nachricht %d (aus Slot 15)",
              rr[i]->id, (int)g_scd.message_id);
        druck(rr[i], s->stand_x, s->stand_z, s->stand_rot, bit59);
        CHECK(!re15_menu_doc_active() && g_scd.message_active && g_scd.message_id == msg[i],
              "3 ROOM%04X Dokument genommen: derselbe Druck -> Nachricht %d \"It's Lieutenant "
              "Branagh's desk.\"", rr[i]->id, (int)g_scd.message_id);
    }
    /* Die Nachricht deckt weiter den ganzen Tisch: Druck an der Tisch-Nordseite (ausserhalb des
     * Dokument-Rechtecks z -16928..-15928) liefert die Nachricht aus Slot 15. */
    druck(r1020, -11500, -15300, 0, NULL);
    CHECK(!re15_menu_doc_active() && g_scd.message_active && g_scd.message_id == 3,
          "3 ROOM1020 Druck (-11500,-15300) Blick +x (ausserhalb der Dokument-Zone) -> Nachricht %d",
          (int)g_scd.message_id);
}

/* ------------------------------------------------------------------ Dok 4: Spray-Vorrang */
static void teil_dok4_spray(raum_t *r1010, raum_t *r1011)
{
    const soll_t *s = soll_nr(4);
    raum_t *rr[2] = { r1010, r1011 };
    static const uint8_t spray[2] = { 0x22, 0x39 };    /* @0x00996 (1010) / @0x00954 (1011) +14 */
    printf("\n[4] Dokument 4: Spray und Blatt auf dem Verhoertisch\n");
    for (int i = 0; i < 2; i++) {
        druck(rr[i], 300, 6400, 1024, NULL);
        uint8_t typ = 0; int wahl = -1, w = 0;
        int modal = re15_item_modal_active(), doc = re15_menu_doc_active();
        while (re15_item_modal_active() && !re15_item_modal_prompt_ready() && w++ < 800)
            re15_item_modal_tick(0, 0);
        re15_item_modal_prompt(&typ, &wahl);
        CHECK(modal && !doc && typ == spray[i],
              "4 ROOM%04X Druck (300,6400) Blick -z (beide Zonen decken): Item-Modal 0x%02X (Soll 0x%02X, "
              "Slot 2 < Slot 9), kein Leser", rr[i]->id, typ, spray[i]);
        w = 0;
        while (re15_item_modal_active() && w++ < 1600) re15_item_modal_tick((uint16_t)0x4000u, 0);
        druck(rr[i], s->stand_x, s->stand_z, s->stand_rot, NULL);
        CHECK(re15_menu_doc_active() && !re15_item_modal_active(),
              "4 ROOM%04X Druck (%d,%d) Blick -z (nur Blatt-Zone): Leser", rr[i]->id, s->stand_x, s->stand_z);
        leser_abschliessen();
    }
    /* Marke: das Blatt liegt NEBEN der Dose unter der Marke (lage_1010.py); die Ursprungs-
     * Projektion liegt in der Marke x208..231 y166..187 des Nutzerbilds. */
    re15_camera_view_t v; double sx = -1, sy = -1;
    if (re15_camera_build_view(&r1010->rdt.cuts[0], &v) == 0) proj(&v, s->x, s->y, s->z, &sx, &sy);
    CHECK(sx >= 208 && sx <= 232 && sy >= 166 && sy <= 188,
          "M ROOM1010 Cut 0: Ursprung (%.2f ; %.2f) in der Marke x208..231 y166..187 (interrogation.bmp)", sx, sy);
}

int main(void)
{
    static raum_t r1050, r1051, r1040, r1000, r1001, r1020, r1021, r1010, r1011;
    if (raum_laden(&r1050, 0x1050) || raum_laden(&r1051, 0x1051) || raum_laden(&r1040, 0x1040) ||
        raum_laden(&r1000, 0x1000) || raum_laden(&r1001, 0x1001) ||
        raum_laden(&r1020, 0x1020) || raum_laden(&r1021, 0x1021) ||
        raum_laden(&r1010, 0x1010) || raum_laden(&r1011, 0x1011))
        return 1;
    raum_t *raeume[4][2] = { { &r1050, &r1051 }, { &r1000, &r1001 }, { &r1020, &r1021 },
                             { &r1010, &r1011 } };
    raum_t *alle[] = { &r1050, &r1051, &r1000, &r1001, &r1020, &r1021, &r1010, &r1011 };
    for (int i = 0; i < N_SOLL; i++) {
        const soll_t *s = &k_soll[i];
        teil_t(s);
        teil_pzg(s, raeume[i], &r1040);
        teil_d(s, raeume[i][0]);
        teil_s(s);
        teil_v(s, raeume[i]);
        teil_b(s);
    }
    teil_dok1_elza(&r1051);
    teil_dok3_nachricht(&r1020, &r1021);
    teil_dok4_spray(&r1010, &r1011);
    teil_k(alle, (int)(sizeof alle / sizeof alle[0]));
    printf("\n%s: %d Fehler\n", fails ? "FAIL" : "OK", fails);
    return fails ? 1 : 0;
}
