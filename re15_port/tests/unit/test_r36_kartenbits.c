/* test_r36_kartenbits.c — Runde 36, Nachtrag zu Spur N (Zweig r36/kartenbits): die Lageplan-Bits der Spur N
 * duerfen KEIN Bank-9-Bit des Originals belegen. Dossier: analysis/befunde_runde36/N_kartenitems.md Abschnitt 9.
 *
 * WARUM: Bank 9 = 0x800b1078 (Flag-Tabelle 0x80074664[9], Zeiger @0x80074688) ist im Original die Zone der
 * Item-TAKEN-Bits. Item_aot_set (Opcode 0x50, Handler 0x80040644) liest das Bit als u16 pc[18] (`lhu a1,18(a2)`
 * @0x80040680; Langform pc[26] @0x80040670) und testet es beim Anlegen (`lw a0,0x4688(a0)` @0x800406d8 ->
 * `jal 0x8004efe4` @0x800406dc); gesetzt -> Satz still (`sb zero,0(v0)` @0x800406f4) und Modell weg
 * (@0x800406f8-718). Ck (0x21, @0x8003fcf4) und Set (0x22, @0x8003fdd0) adressieren ueber DIESELBE Tabelle
 * 0x80074664 dasselbe Bit. Ein Port-Bit, das ein Original-Item-Bit trifft, laesst dieses Item verschwinden.
 *
 *   V  VERHALTEN: jeden der fuenf Lageplaene ueber das echte Port-Programm nehmen (Ereignis 28, Ja) und danach die
 *      Stage-2-Raeume starten, deren ausgelieferter Item_aot_set in der alten Spur-N-Belegung lag:
 *        ROOM2040/2041 main00 @0x01364 `50 07 09 31 03 00 a4 06 62 c6 e8 03 e8 03 32 00 01 00 5b 00 ff 00`
 *                      (Slot 7, Item 0x32 "Head of Akuma", Bit 0x5b = 91)
 *        ROOM2090/2091 main00 @0x00A12 `50 05 09 31 01 00 e6 14 c4 be e8 03 e8 03 3f 00 01 00 5a 00 00 00`
 *                      (Slot 5, Item 0x3F "Pocket Watch", Bit 0x5a = 90)
 *      -> der Item-Satz muss scharf bleiben. GEGENPROBE: das Original-Bit des Satzes gesetzt -> Satz still (die
 *      Messung kann den Fehler sehen). RUECKRICHTUNG: diese Original-Items genommen -> kein Polizei-Lageplan im
 *      Besitz, und die Karte haengt in ihrem Raum noch.
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

/* Raumstart wie der Tuer-Weg (scd_room_reenter mit allen Port-Installern, scd_room_setup.c) — OHNE die
 * Spielflags anzufassen: Bank 9 reist von Raum zu Raum wie im Spiel (g_game.flags; das Original nullt seine
 * Flag-Baenke beim Raumladen nicht, scd_room_setup.c "scd_vm_init MINUS re15_game_state_init"). scd_vm_init
 * ruft re15_game_state_init (scd_vm.c) — deshalb wird g_game darum herum aufbewahrt. */
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

static void bank9_lesen(uint8_t b[256])
{
    for (int i = 0; i < 256; i++) b[i] = (uint8_t)re15_game_flag_get(BANK9, (uint8_t)i);
}

static void bank9_schreiben(const uint8_t b[256])
{
    for (int i = 0; i < 256; i++) re15_game_flag_set(BANK9, (uint8_t)i, b[i]);
}

/* ================================================================= V */

typedef struct {
    uint16_t raum; uint32_t off; uint8_t slot, item; uint16_t bit; const char *name;
} orig_item_t;

/* Die ausgelieferten Saetze (Bytes oben im Kopf; geprueft in teil_v gegen das RDT). */
static const orig_item_t k_orig[] = {
    { 0x2040, 0x01364, 7, 0x32, 91, "Head of Akuma" },
    { 0x2041, 0x01364, 7, 0x32, 91, "Head of Akuma" },
    { 0x2090, 0x00A12, 5, 0x3F, 90, "Pocket Watch" },
    { 0x2091, 0x00A12, 5, 0x3F, 90, "Pocket Watch" },
};
#define N_ORIG ((int)(sizeof k_orig / sizeof k_orig[0]))

static raum_t s_orig_raum[N_ORIG];

static int satz_scharf(const orig_item_t *o)
{
    return g_aot.slots[o->slot].active && g_aot.slots[o->slot].type == RE15_AOT_TYPE_ITEM &&
           g_aot.item_params[o->slot].item_type == o->item;
}

/* Den Original-Raum mit dem aktuellen Bank-9-Stand starten; 1 = Item-Satz scharf. */
static int orig_raum_scharf(int i)
{
    raum_start(&s_orig_raum[i], 0, 0);
    return satz_scharf(&k_orig[i]);
}

/* Lageplan k ueber das ECHTE Port-Programm nehmen: Raum starten (Installer legt Prop + Zone an), Ereignis 28
 * feuern (= was der Aktions-Scan des EVENT-Satzes tut, scd_event_fire), jede Ja/Nein-Frage mit JA beantworten
 * (virtuelles Bestaetigen-Bit 0x4000, wie probe_besitz_zensus.c), bis der Faden endet. Rueckgabe: Anzahl der
 * Bank-9-Bits, die das Programm gesetzt hat (< 0 = Ablauf gescheitert); *out_bit = das gesetzte Bit. */
static int plan_nehmen(int k, int *out_bit)
{
    const re15_kartenitem_r36_t *z = re15_kartenitems_r36_zeile(k);
    raum_t r;
    *out_bit = -1;
    if (!z || raum_laden(&r, z->raum) != 0) return -1;
    raum_start(&r, z->rect_x + RE15_R36K_RECT / 2, z->rect_z + RE15_R36K_RECT / 2);
    int zone = g_aot.slots[z->aot_slot].active && g_aot.slots[z->aot_slot].type == RE15_AOT_TYPE_GENERIC &&
               g_aot.slots[z->aot_slot].event_id == RE15_R36K_EREIGNIS;
    if (!zone) { free(r.buf); return -2; }
    uint8_t vor[256], nach[256];
    bank9_lesen(vor);
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
    int lief_zu_ende = !g_scd.threads[slot].active;
    bank9_lesen(nach);
    int n = 0;
    for (int b = 0; b < 256; b++)
        if (nach[b] && !vor[b]) { n++; *out_bit = b; }
    free(r.buf);
    return lief_zu_ende ? n : -4;
}

static void teil_v(void)
{
    printf("=== V: Lageplan genommen -> Original-Items in Stage 2 bleiben (Item_aot_set @0x80040644) ===\n");
    /* V0: die Saetze stehen so im RDT, wie der Kopf sie zitiert. */
    for (int i = 0; i < N_ORIG; i++) {
        const orig_item_t *o = &k_orig[i];
        if (raum_laden(&s_orig_raum[i], o->raum) != 0) return;
        const uint8_t *p = s_orig_raum[i].buf + o->off;
        uint16_t bit = (uint16_t)(p[18] | (p[19] << 8));
        PRUEFE(o->off + 22 <= s_orig_raum[i].n && p[0] == 0x50 && !(p[3] & 0x80) && p[1] == o->slot &&
               p[14] == o->item && bit == o->bit,
               "ROOM%04X @0x%05X Item_aot_set Slot %u Item 0x%02X (%s), Taken-Bit u16 pc[18] = %u",
               (unsigned)o->raum, (unsigned)o->off, (unsigned)p[1], (unsigned)p[14], o->name, (unsigned)bit);
    }
    /* V1: frischer Stand -> scharf; V2 GEGENPROBE: Original-Bit gesetzt -> still (@0x800406f4). */
    for (int i = 0; i < N_ORIG; i++) {
        const orig_item_t *o = &k_orig[i];
        re15_game_state_init();
        int frisch = orig_raum_scharf(i);
        re15_game_state_init();
        re15_game_flag_set(BANK9, (uint8_t)o->bit, 1);
        int still = !orig_raum_scharf(i);
        PRUEFE(frisch && still,
               "ROOM%04X Slot %u: frisch scharf (%d), mit Bit (9,%u) still (%d) — Gegenprobe der Messung",
               (unsigned)o->raum, (unsigned)o->slot, frisch, (unsigned)o->bit, still);
    }
    /* V3: jeden Plan EINZELN nehmen -> alle vier Saetze pruefen (zeigt, welcher Plan welches Item traf). */
    int n_plaene = re15_kartenitems_r36_anzahl();
    for (int k = 0; k < n_plaene; k++) {
        const re15_kartenitem_r36_t *z = re15_kartenitems_r36_zeile(k);
        re15_game_state_init();
        int bit = -1;
        int n = plan_nehmen(k, &bit);
        PRUEFE(n == 1 && bit == z->bit,
               "Plan Blatt %u (ROOM%04X): Port-Programm Ereignis 28 lief zu Ende, setzte %d Bank-9-Bit(s): (9,%d)",
               (unsigned)z->blatt, (unsigned)z->raum, n, bit);
        uint8_t stand[256];
        bank9_lesen(stand);
        for (int i = 0; i < N_ORIG; i++) {
            bank9_schreiben(stand);
            int scharf = orig_raum_scharf(i);
            PRUEFE(scharf, "  nach Plan Blatt %u (Bit 9,%d): ROOM%04X Slot %u Item 0x%02X (%s, Taken-Bit %u) %s",
                   (unsigned)z->blatt, bit, (unsigned)k_orig[i].raum, (unsigned)k_orig[i].slot,
                   (unsigned)k_orig[i].item, k_orig[i].name, (unsigned)k_orig[i].bit,
                   scharf ? "scharf" : "STILL - das Item ist weg");
        }
    }
    /* V4: RUECKRICHTUNG — die Original-Items genommen (ihre Bits gesetzt, wie das Aufnahme-Modal
     * item_modal_common.c bzw. im Original `jal 0x8004ef90` @0x8001e0d0 / @0x80021fd8) -> kein Polizei-Plan im
     * Besitz, und jeder Plan haengt in seinem Raum noch (Prop obj 15 + EVENT-Satz Ereignis 28). */
    re15_game_state_init();
    for (int i = 0; i < N_ORIG; i++) re15_game_flag_set(BANK9, (uint8_t)k_orig[i].bit, 1);
    uint8_t stand[256];
    bank9_lesen(stand);
    uint32_t besitz = re15_map_owned_bits() & 0x1Fu;
    PRUEFE(besitz == 0, "Original-Items 0x32/0x3F genommen (Bits 90/91): Besitz der Polizei-Blaetter 0..4 = 0x%02X",
           (unsigned)besitz);
    for (int k = 0; k < n_plaene; k++) {
        const re15_kartenitem_r36_t *z = re15_kartenitems_r36_zeile(k);
        raum_t r;
        if (raum_laden(&r, z->raum) != 0) continue;
        bank9_schreiben(stand);
        raum_start(&r, z->rect_x + RE15_R36K_RECT / 2, z->rect_z + RE15_R36K_RECT / 2);
        int haengt = prop_slot(RE15_R36K_OBJ) >= 0 && g_aot.slots[z->aot_slot].active &&
                     g_aot.slots[z->aot_slot].type == RE15_AOT_TYPE_GENERIC &&
                     g_aot.slots[z->aot_slot].event_id == RE15_R36K_EREIGNIS;
        PRUEFE(haengt, "  ROOM%04X: Plan Blatt %u haengt noch (Prop obj %d + Zone Slot %u Ereignis %d)",
               (unsigned)z->raum, (unsigned)z->blatt, RE15_R36K_OBJ, (unsigned)z->aot_slot, RE15_R36K_EREIGNIS);
        free(r.buf);
    }
}

int main(void)
{
    printf("=== Runde 36 Nachtrag N: Lageplan-Bits gegen die Bank-9-Bits des Originals ===\n");
    teil_v();
    for (int i = 0; i < N_ORIG; i++) free(s_orig_raum[i].buf);
    if (g_fail) { printf("FAIL\n"); return 1; }
    printf("OK\n");
    return 0;
}
