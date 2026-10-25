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
 *   Z  ZENSUS DES ORIGINALS: alle ausgelieferten RDTs (shared_assets/PSX/STAGE1..6/ROOM*.RDT), main- und sub-SCD,
 *      gelaufen mit dem Laengen-Vorschub des Motors selbst (scd_opcode_size_at = s_opcode_sizes in scd_vm.c, schon
 *      exportiert fuer r21_discard — keine zweite Laengentabelle). Gesammelt wird jede Bank-9-Bitnummer, die das
 *      Original anfasst: Item_aot_set-Taken-Bit (u16 pc[18] bzw. pc[26]), Ck(9,n) (@0x8003fd2c), Set(9,n)
 *      (@0x8003fe04), FLAG_CHG-Saetze sce 4 mit Bank 9 (Aot_set/Aot_reset, LAB_80043120 @0x80043140); Flag-Ck2/
 *      Flag-Set2 (0x58 @0x8003fda0 / 0x59 @0x8003fed0) mit Bank 9 tragen das Bit erst zur Laufzeit und duerfen
 *      deshalb NICHT vorkommen (sonst ist der Satz unvollstaendig). Andere Leser/Schreiber der Bank 9 hat das
 *      Original nicht (Zensus-Datei analysis/befunde_runde36/N_belege/bank9_zensus.txt: Ghidra-XREF, Overlays).
 *      GEGENPROBE des Zensus: 206 RDTs mit Kopf, 0 Desync, 164 Item_aot_set, und die bekannten Original-Bits
 *      85, 86, 90, 91, 92 (+ die Kartenleser-Tore 52/136/138) muessen gefunden werden.
 *   P  PORT: explizite Liste ALLER Bank-9-Bits des Ports (je Bit Makro + Quelle, Runde 30..36; die Lageplan-Bits
 *      live aus der Tabelle k_r36k_lage) — KEINES darf im Original-Satz liegen, und kein Lageplan-Bit darf ein
 *      anderes Port-Bit sein. GEGENPROBE: die alte Spur-N-Belegung 87..91 (Stand 7a0fd453) liefert mit DERSELBEN
 *      Pruefung genau die Treffer {90, 91} — der Riegel waere mit ihr rot.
 *   S  ALT-SPIELSTAENDE: die eingecheckte Nutzer-Speicherkarte (2026-09-27) traegt 77/78 in keinem Platz; ein
 *      v0.8.23-Stand mit genommenen Items 0x3F/0x32 (Bits 90/91) zeigt nach dem Laden KEINEN Lageplan als besessen.
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
#include "re15_savedata.h"
#include "re15_memcard.h"
/* Teil P: die Bank-9-Bits des Ports stehen als Makros in diesen Koepfen (Runde 30..36). */
#include "re15_sicherung.h"
#include "re15_irons_tisch.h"
#include "re15_granate.h"
#include "re15_dokumente.h"
#include "re15_leiche.h"
#include "re15_rolltor.h"
#include "re15_adaruf.h"
#include "re15_cut10f0.h"
#include "re15_irons_tod.h"
#include "re15_fenster1120.h"
#include "re15_inhalt_r35.h"
#include "re15_schluessel_r36.h"
#include "re15_szenenleon_r36.h"
#include "re15_tuer1060.h"
#include "re15_tagebuch_r38.h"   /* Runde 38 Spur G: Bit 68 Roys Diary */
#include "re15_dining_r38.h"   /* Runde 38 Spur F: Bank-9-Bits 64/66/67 (VERTRAG Runde 38 1.1) */
#include "re15_inventar_r39.h" /* Runde 39 Spur C: Bank-9-Bit 69 (VERTRAG Runde 39 1.1) */
#include "re15_dining_r39.h"   /* Runde 39 Spur J: Bank-9-Bit 94 (VERTRAG Runde 39 1.1) */
#include "re15_funk10f1_r39.h" /* Runde 39 Spur I: Bank-9-Bit 70 (VERTRAG Runde 39 1.1) */
#include "re15_elzazombies_r39.h" /* Runde 39 Spur Q: Bank-9-Bits 95 + 96 (VERTRAG Runde 39 1.1; 96 Nachbesserung 3) */
#include "re15_johnruf_r39.h"  /* Runde 39 Spur P: Bank-9-Bit 97 (VERTRAG Runde 39 1.1) */
#include "re15_ventilkette_r41.h" /* Runde 41 Spur C: Bank-9-Bits 100-103, 106, 107 (VERTRAG Runde 41 1.1; 107 = Dokument, Nachbesserung 4) */
#include "re15_gatorszene_r41.h" /* Runde 41 Spur F: Bank-9-Bits 110-113 (VERTRAG Runde 41 1.1) */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_kanister2080_r41.h"   /* Runde 41 Spur G: Bits 114-117 */

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

/* ================================================================= Z */

typedef struct {
    long rdts, stummel, regionen, opcodes, desync;
    long item_aot_set, ck9, set9, flagchg9, op58_9, op59_9, item_bit_hoch;
    uint8_t  belegt[256];                  /* 1 = das Original fasst dieses Bank-9-Bit an */
    char     erste[256][48];               /* erste Fundstelle je Bit (fuer die Ausgabe) */
} zensus_t;

static zensus_t s_z;

static uint16_t le16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }
static int16_t  les16(const uint8_t *p) { return (int16_t)le16(p); }
static uint32_t le32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static void bit_merken(uint32_t bit, unsigned room, const char *tag, int idx, uint32_t pc, const char *was)
{
    if (bit >= 256) { s_z.item_bit_hoch++; return; }
    if (!s_z.belegt[bit])
        snprintf(s_z.erste[bit], sizeof s_z.erste[bit], "ROOM%04X %s%02d @0x%05X %s", room, tag, idx,
                 (unsigned)pc, was);
    s_z.belegt[bit] = 1;
}

/* Sektionsende und Regionen exakt wie tools/scd_dump_room.py (rdt_section_end / section_regions), damit der
 * C-Zensus und bank9_zensus.py dieselben Opcodes sehen (Gegenprobe: dieselbe Opcode-Zahl). */
static uint32_t ptr_ausdehnung(const uint8_t *d, size_t n, uint32_t s)
{
    if (s == 0 || s + 2 > n) return 0;
    uint16_t first = le16(d + s);
    if (first < 2 || (first & 1) || s + first > n) return 0;
    uint32_t mx = 0;
    for (uint32_t i = 0; i < first / 2u; i++) {
        uint16_t o = le16(d + s + 2 * i);
        if (o == 0 || s + o >= n) continue;
        if (o > mx) mx = o;
    }
    return mx;
}

static uint32_t sektion_ende(const uint8_t *d, size_t n, uint32_t s)
{
    uint32_t boden = s + ptr_ausdehnung(d, n, s), best = 0;
    for (uint32_t o = 0x40; o < 0x60 && o + 4 <= n; o += 4) {
        uint32_t v = le32(d + o);
        if (v > boden && v <= n && (best == 0 || v < best)) best = v;
    }
    return best ? best : (uint32_t)n;
}

static uint32_t sprungziel(const uint8_t *d, uint32_t pc)      /* scd_dump_room.py fwd_target; 0 = keins */
{
    uint8_t op = d[pc];
    if (op == 0x06) return pc + 4 + le16(d + pc + 2);
    if (op == 0x07) return (uint32_t)((int32_t)pc + les16(d + pc + 2));
    if (op == 0x0D || op == 0x1B) return (uint32_t)((int32_t)pc + 6 + les16(d + pc + 2));
    if (op == 0x0F || op == 0x11 || op == 0x13) return (uint32_t)((int32_t)pc + 4 + les16(d + pc + 2));
    if (op == 0x14) return pc + 6 + le16(d + pc + 2);
    if (op == 0x17) { int16_t o = les16(d + pc + 4); return o > 0 ? (uint32_t)((int32_t)pc + o) : 0; }
    return 0;
}

static void region_laufen(const uint8_t *d, uint32_t start, uint32_t ende, unsigned room, const char *tag, int idx)
{
    uint32_t pc = start, maxf = start;
    while (pc < ende) {
        int sz = scd_opcode_size_at(d + pc);
        if (sz <= 0 || pc + (uint32_t)sz > ende) { s_z.desync++; return; }
        s_z.opcodes++;
        const uint8_t op = d[pc];
        if (op == 0x50) {                                  /* Item_aot_set @0x80040644 */
            const int lang = (d[pc + 3] & 0x80) != 0;
            const uint32_t bit = le16(d + pc + (lang ? 26 : 18));   /* @0x80040670 / @0x80040680 */
            s_z.item_aot_set++;
            bit_merken(bit, room, tag, idx, pc, "Item_aot_set");
        } else if ((op == 0x21 || op == 0x22) && d[pc + 1] == 9) {   /* Ck @0x8003fd2c / Set @0x8003fe04 */
            if (op == 0x21) s_z.ck9++; else s_z.set9++;
            bit_merken(d[pc + 2], room, tag, idx, pc, op == 0x21 ? "Ck(9)" : "Set(9)");
        } else if (op == 0x58 && d[pc + 1] == 9) {
            s_z.op58_9++;                                  /* Bit aus work_vars: statisch unbestimmt */
        } else if (op == 0x59 && d[pc + 1] == 9) {
            s_z.op59_9++;
        } else if (op == 0x2C && d[pc + 2] == 4) {         /* Aot_set sce 4 FLAG_CHG (LAB_80043120) */
            const uint32_t b = pc + ((d[pc + 3] & 0x80) ? 22 : 14);
            if (le16(d + b) == 9) { s_z.flagchg9++; bit_merken(le16(d + b + 2), room, tag, idx, pc, "Aot_set FLAG_CHG"); }
        } else if (op == 0x46 && d[pc + 2] == 4) {         /* Aot_reset auf sce 4 */
            if (le16(d + pc + 4) == 9) { s_z.flagchg9++; bit_merken(le16(d + pc + 6), room, tag, idx, pc, "Aot_reset FLAG_CHG"); }
        }
        uint32_t t = sprungziel(d, pc);
        if (t > maxf) maxf = t;
        if (op == 0x01 && pc + 2 > maxf) return;           /* Evt_end hinter dem weitesten Vorwaertsziel */
        pc += (uint32_t)sz;
    }
}

static void zensus_raum(const uint8_t *d, size_t n, unsigned room)
{
    static const char *tags[2] = { "main", "sub" };
    for (int k = 0; k < 2; k++) {
        uint32_t ss = le32(d + 0x40 + 4 * k);
        if (ss == 0 || ss >= n || ss + 2 > n) continue;
        uint32_t se = sektion_ende(d, n, ss);
        uint16_t first = le16(d + ss);
        if (first < 2 || (first & 1) || ss + first > n) continue;
        uint32_t offs[512]; int idx_von[512]; int cnt = 0;
        for (uint32_t i = 0; i < first / 2u && cnt < 512; i++) {
            uint16_t o = le16(d + ss + 2 * i);
            if (!o || ss + o >= n) continue;
            int dup = 0;
            for (int j = 0; j < cnt; j++) if (offs[j] == o) dup = 1;
            if (!dup) { offs[cnt] = o; idx_von[cnt] = (int)i; cnt++; }
        }
        for (int i = 0; i < cnt; i++)
            for (int j = i + 1; j < cnt; j++)
                if (offs[j] < offs[i]) {
                    uint32_t t = offs[i]; offs[i] = offs[j]; offs[j] = t;
                    int ti = idx_von[i]; idx_von[i] = idx_von[j]; idx_von[j] = ti;
                }
        for (int i = 0; i < cnt; i++) {
            uint32_t e = (i + 1 < cnt) ? offs[i + 1] : se - ss;
            if (e > se - ss) e = se - ss;
            if (e > offs[i]) { s_z.regionen++; region_laufen(d, ss + offs[i], ss + e, room, tags[k], idx_von[i]); }
        }
    }
}

static void teil_z(void)
{
    printf("=== Z: Zensus der Bank-9-Bits des Originals (alle ausgelieferten RDTs) ===\n");
    memset(&s_z, 0, sizeof s_z);
    for (unsigned st = 1; st <= 6; st++)
        for (unsigned hi = 0; hi < 0x100; hi++)
            for (unsigned lo = 0; lo < 2; lo++) {
                unsigned room = (st << 12) | (hi << 4) | lo;
                char p[600]; size_t n = 0;
                snprintf(p, sizeof p, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, st, room);
                uint8_t *d = slurp(p, &n);
                if (!d) continue;
                if (n < 0x48) { s_z.stummel++; free(d); continue; }
                s_z.rdts++;
                zensus_raum(d, n, room);
                free(d);
            }
    int belegt = 0;
    for (int b = 0; b < 256; b++) belegt += s_z.belegt[b];
    printf("  Abdeckung: %ld RDTs mit Kopf (+%ld Stummel), %ld Regionen, %ld Opcodes, %ld Desync\n",
           s_z.rdts, s_z.stummel, s_z.regionen, s_z.opcodes, s_z.desync);
    printf("  %ld Item_aot_set, %ld Ck(9,n), %ld Set(9,n), %ld FLAG_CHG(9,n), 0x58/0x59 mit Bank 9: %ld/%ld -> %d von 256 Bits\n",
           s_z.item_aot_set, s_z.ck9, s_z.set9, s_z.flagchg9, s_z.op58_9, s_z.op59_9, belegt);
    printf("  belegt:");
    for (int b = 0; b < 256; b++) if (s_z.belegt[b]) printf(" %d", b);
    printf("\n");
    PRUEFE(s_z.rdts == 206, "206 RDTs mit Kopf gelesen (%ld)", s_z.rdts);
    PRUEFE(s_z.desync == 0, "0 Desync-Stopps (%ld) — der Walk ist synchron", s_z.desync);
    PRUEFE(s_z.opcodes == 40674, "40674 Opcodes = dieselbe Zahl wie bank9_zensus.py (%ld)", s_z.opcodes);
    PRUEFE(s_z.item_aot_set == 164, "164 Item_aot_set = r21_discard TEIL C (%ld)", s_z.item_aot_set);
    PRUEFE(s_z.op58_9 == 0 && s_z.op59_9 == 0,
           "kein Flag-Ck2/Flag-Set2 (0x58/0x59) mit Bank 9 — sonst traegt das Original Bits, die erst zur Laufzeit "
           "feststehen (%ld/%ld)", s_z.op58_9, s_z.op59_9);
    PRUEFE(s_z.item_bit_hoch == 0, "kein Item-Taken-Bit >= 256 (%ld)", s_z.item_bit_hoch);
    PRUEFE(belegt == 85, "85 von 256 Bits belegt (= bank9_zensus.py und Runde-30-Zensus r30_idw_zensus.py): %d", belegt);
    /* GEGENPROBE: der Zensus sieht die bekannten Original-Bits (sonst ist er selbst kaputt). */
    static const struct { uint8_t bit; const char *was; } k_bekannt[] = {
        { 85, "ROOM3040 sub00 @0x014B2 Item 0x36 Green Keycard" },
        { 86, "ROOM4040 main00 @0x00F66 Item 0x41 Spark Plug" },
        { 90, "ROOM2090 main00 @0x00A12 Item 0x3F Pocket Watch" },
        { 91, "ROOM2040 main00 @0x01364 Item 0x32 Head of Akuma" },
        { 92, "ROOM3050 main00 @0x01EC6 Item 0x07 Super Redhawk" },
        { 52, "Blue Keycard 0x38 - Tor Ck(9,52) ROOM10D0 sub20 @0x019C0" },
        { 136, "Red Keycard 0x37 - Tor Ck(9,136) ROOM1230 sub20 @0x01488" },
        { 138, "Yellow Keycard 0x39 - Tor Ck(9,138) ROOM11E0 sub20 @0x01FA8" },
        { 21, "einziges reines Skript-Bit: ROOM3091 sub06 @0x016C2 Set(9,21,1)" },
    };
    for (size_t i = 0; i < sizeof k_bekannt / sizeof k_bekannt[0]; i++)
        PRUEFE(s_z.belegt[k_bekannt[i].bit], "Zensus findet Bit %u (%s): erste Stelle %s",
               (unsigned)k_bekannt[i].bit, k_bekannt[i].was, s_z.erste[k_bekannt[i].bit]);
}

/* ================================================================= P */

typedef struct { unsigned bank; unsigned bit; const char *makro; const char *quelle; } port_bit_t;

/* ALLE Bank-9-Bits des Ports (Zensus 2026-10-12, bank9_zensus.py Teil port; Runde 30..36). Die Werte kommen aus
 * den Koepfen — aendert jemand ein Makro auf ein Original-Bit, wird dieser Test rot. Neue Port-Bits: HIER
 * eintragen (und VERTRAG.md 1.1). Die Lageplan-Bits der Spur N kommen live aus k_r36k_lage (teil_p). */
static const port_bit_t k_port[] = {
    { 9, RE15_SICHERUNG_TAKEN_BIT,       "RE15_SICHERUNG_TAKEN_BIT",       "Sicherung genommen, sicherung_1150.c (Runde 30/34)" },
    { 9, RE15_IRONS_DIARY_TAKEN_BIT,     "RE15_IRONS_DIARY_TAKEN_BIT",     "Irons Diary genommen, irons_tisch_1150.c (Runde 30)" },
    { 9, RE15_IRONS_KARTE_TAKEN_BIT,     "RE15_IRONS_KARTE_TAKEN_BIT",     "Memory Card Irons-Tisch, irons_tisch_1150.c (Runde 30)" },
    { 9, RE15_GRANATE_TAKEN_BIT,         "RE15_GRANATE_TAKEN_BIT",         "Granate 1150 genommen, granate_1150.c (Runde 30)" },
    { 9, RE15_DOK1_BIT,                  "RE15_DOK1_BIT",                  "Dokument 1 ROOM1050, dokumente_r34.c (Runde 34)" },
    { 9, RE15_DOK2_BIT,                  "RE15_DOK2_BIT",                  "Dokument 2 ROOM1000, dokumente_r34.c (Runde 34)" },
    { 9, RE15_DOK3_BIT,                  "RE15_DOK3_BIT",                  "Dokument 3 ROOM1020, dokumente_r34.c (Runde 34)" },
    { 9, RE15_DOK4_BIT,                  "RE15_DOK4_BIT",                  "Dokument 4 ROOM1010, dokumente_r34.c (Runde 34)" },
    { 9, RE15_LEICHE_1110_BIT,           "RE15_LEICHE_1110_BIT",           "Leichen-Munition ROOM1110, leiche_1110_1230.c (Runde 34)" },
    { 9, RE15_LEICHE_1230_BIT,           "RE15_LEICHE_1230_BIT",           "Leichen-Munition ROOM1230, leiche_1110_1230.c (Runde 34)" },
    { RE15_ROLLTOR_EINGESETZT_BANK, RE15_ROLLTOR_EINGESETZT_BIT, "RE15_ROLLTOR_EINGESETZT_BIT", "Sicherung eingesetzt, rolltor_1050.c Set @+2A (Runde 34)" },
    { RE15_ADARUF_GESEHEN_BANK, RE15_ADARUF_GESEHEN_BIT, "RE15_ADARUF_GESEHEN_BIT", "Ada-Ruf gesehen, adaruf_1050.c Set @+00 (Runde 34)" },
    { RE15_CUT10F0_GESEHEN_BANK, RE15_CUT10F0_GESEHEN_BIT, "RE15_CUT10F0_GESEHEN_BIT", "10F0-Szene gesehen, gen/cut10f0_szene.inc Set @+0000 (Runde 35 K)" },
    { RE15_CUT10F0_ZIEL2_BESUCHT_BANK, RE15_CUT10F0_ZIEL2_BESUCHT_BIT, "RE15_CUT10F0_ZIEL2_BESUCHT_BIT", "10F0 Ziel 2 besucht, cut_10f0.c (Runde 35 K)" },
    { RE15_CUT10F0_BGM_START_BANK, RE15_CUT10F0_BGM_START_BIT, "RE15_CUT10F0_BGM_START_BIT", "liest Irons-Tod 73, cut_10f0.c (Runde 35)" },
    { RE15_IT_K_BANK, RE15_IT_K_BIT,     "RE15_IT_K_BIT",                  "liest 10F0-Szene 71, irons_tod_1150.c (Runde 35 L)" },
    { RE15_IT_BANK, RE15_IT_BIT_GESEHEN, "RE15_IT_BIT_GESEHEN",            "Irons-Todesszene gesehen, irons_tod_1150.c OP_SET (Runde 35 L)" },
    { RE15_IT_BANK, RE15_IT_BIT_1130,    "RE15_IT_BIT_1130",               "Spawn 1130 verbraucht, irons_tod_1150.c (Runde 35 L)" },
    { RE15_IT_BANK, RE15_IT_BIT_1040,    "RE15_IT_BIT_1040",               "Spawn 1040 verbraucht, irons_tod_1150.c (Runde 35 L)" },
    { RE15_IT_BANK, RE15_IT_BIT_1030,    "RE15_IT_BIT_1030",               "Spawn 1030 verbraucht, irons_tod_1150.c (Runde 35 L)" },
    { RE15_FENSTER_BANK, RE15_FENSTER_BIT, "RE15_FENSTER_BIT",             "Fenster 1120 kaputt, fenster_1120.c (Runde 35 M)" },
    { RE15_FENSTER_BANK, RE15_FENSTER_TOR_BIT, "RE15_FENSTER_TOR_BIT",     "liest Irons-Tod 73, fenster_1120.c (Runde 35)" },
    { 9, RE15_R35I_KARTE_BIT,            "RE15_R35I_KARTE_BIT",            "Memory Card ROOM1010, inhalt_r35.c (Runde 35 F)" },
    { 9, RE15_R35I_SCHROT_BIT,           "RE15_R35I_SCHROT_BIT",           "Schrot ROOM1090, inhalt_r35.c (Runde 35 F)" },
    { 9, RE15_R36Q_PARK_BIT,             "RE15_R36Q_PARK_BIT",             "Parking-Lot-Key genommen, schluessel_r36.c (Runde 36 Q)" },
    { 9, RE15_R36Q_PRISON_BIT,           "RE15_R36Q_PRISON_BIT",           "Prison Key genommen, schluessel_r36.c (Runde 36 Q)" },
    { 9, RE15_R36Q_TUER_BIT,             "RE15_R36Q_TUER_BIT",             "Parkplatz-Tuer aufgeschlossen, schluessel_r36.c (Runde 36 Q)" },
    { RE15_SZL_BANK_ZU, RE15_SZL_BIT_ZU, "RE15_SZL_BIT_ZU",                "Werkstatt-Tuer verschlossen, gen/szl11b0_szene.inc Set 0x54 (Runde 36 J)" },
    { RE15_TUER1060_K_BANK, RE15_TUER1060_K_BIT, "RE15_TUER1060_K_BIT",    "liest 10F0-Szene 71, tuer1060_1040.c (Runde 35)" },
    { RE15_TUER1060_TOD_BANK, RE15_TUER1060_TOD_BIT, "RE15_TUER1060_TOD_BIT", "liest Irons-Tod 73, tuer1060_1040.c (Runde 35)" },
    { RE15_TAGEBUCH_R38_BANK, RE15_TAGEBUCH_R38_BIT, "RE15_TAGEBUCH_R38_BIT", "Roys Diary abgelegt, tagebuch_r38.c (Runde 38 G)" },
    { 9, RE15_R38F_BIT_GENOMMEN,          "RE15_R38F_BIT_GENOMMEN",          "Dining Room Key genommen, dining_r38.c (Runde 38 F)" },
    { 9, RE15_R38F_BIT_TUER,              "RE15_R38F_BIT_TUER",              "Tuer 1131->1141 aufgeschlossen, dining_r38.c (Runde 38 F)" },
    { 9, RE15_R38F_BIT_JOHN_WEG,          "RE15_R38F_BIT_JOHN_WEG",          "John hat 1141 verlassen, gen/dining_r38_programme.inc (Runde 38 F)" },
    { 9, RE15_R39C_MUNITION_BIT,          "RE15_R39C_MUNITION_BIT",          "Handgun-Munition im Spind ROOM1000, inventar_r39.c (Runde 39 C)" },
    { 9, RE15_R39J_BIT_REAKTION,          "RE15_R39J_BIT_REAKTION",          "Elza hat auf den ersten Hilferuf reagiert, gen/dining_r39_programme.inc (Runde 39 J)" },
    { RE15_FUNK10F1_GESEHEN_BANK, RE15_FUNK10F1_GESEHEN_BIT, "RE15_FUNK10F1_GESEHEN_BIT", "Funkszene 10F1 gesehen, gen/funk10f1_r39_szene.inc Set @+0000 (Runde 39 I)" },
    { RE15_R39Q_BANK, RE15_R39Q_BIT_MONTAGE, "RE15_R39Q_BIT_MONTAGE", "Elza-Montage gelaufen (Kopien), elzazombies_r39.c umzug_vorbereiten (Runde 39 Q)" },
    { RE15_R39Q_BANK, RE15_R39Q_BIT_LEON_1060, "RE15_R39Q_BIT_LEON_1060", "Leon: Umzug 10E0 -> 1060 gelaufen, elzazombies_r39.c re15_r39q_leon_umzug (Runde 39 Q, NB3)" },
    { RE15_R39P_BIT_BANK, RE15_R39P_BIT_PANIK, "RE15_R39P_BIT_PANIK", "Elzas Panikphase (1041-Ruf bis Johns Rettung), gen/johnruf_r39_programme.inc Set @+0048 (Runde 39 P)" },
    { RE15_R41C_BANK, RE15_R41C_BIT_LEITER_FREI,   "RE15_R41C_BIT_LEITER_FREI",   "Leiter 2000/20B0 nach 2050 frei, ventilkette_r41.c install (Runde 41 C)" },
    { RE15_R41C_BANK, RE15_R41C_BIT_VENTIL13,      "RE15_R41C_BIT_VENTIL13",      "Zulaufventil 13 in 2030 offen, gen/ventilkette_r41_programme.inc Set (Runde 41 C)" },
    { RE15_R41C_BANK, RE15_R41C_BIT_BRUECKE_UNTEN, "RE15_R41C_BIT_BRUECKE_UNTEN", "Bruecke 2030 gesenkt, gen/ventilkette_r41_programme.inc Set (Runde 41 C)" },
    { RE15_R41C_BANK, RE15_R41C_BIT_FLUT_GELAUFEN, "RE15_R41C_BIT_FLUT_GELAUFEN", "Flut in 2030 gelaufen -> 2000 wird 20B0, ventilkette_r41.c install (Runde 41 C)" },
    { RE15_R41C_BANK, RE15_R41C_BIT_PULT2050,      "RE15_R41C_BIT_PULT2050",      "Schaltpult 2050 auf 80 V, ventilkette_r41.c panel_tick (Runde 41 C)" },
    { RE15_R41C_BANK, RE15_R41C_BIT_DOKUMENT,      "RE15_R41C_BIT_DOKUMENT",      "Dokument Technician's Note genommen (Taken-Bit Item 0x4E), ventilkette_r41.c install_2030 (Runde 41 C)" },
    { RE15_R41C_BANK, RE15_R41C_BIT_KASTEN,        "RE15_R41C_BIT_KASTEN",        "Sicherung im Ventil-Sicherungskasten 2010, gen/ventilkette_r41_programme.inc Set (Runde 41 C, Nachtrag 16)" },
    { RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN, "RE15_GSZ_BIT_GESEHEN", "2090-Ankunftsszene gesehen, gatorszene_r41.c Programm 3 (Runde 41 F)" },
    { RE15_GSZ_BANK, RE15_GSZ_BIT_LIEGT, "RE15_GSZ_BIT_LIEGT", "Water Key liegt beim toten Alligator, gatorszene_r41.c (Runde 41 F)" },
    { RE15_GSZ_BANK, RE15_GSZ_BIT_GENOMMEN, "RE15_GSZ_BIT_GENOMMEN", "Water Key genommen (Item-Zone Taken-Bit), gatorszene_r41.c (Runde 41 F)" },
    { RE15_GSZ_BANK, RE15_GSZ_BIT_TOR, "RE15_GSZ_BIT_TOR", "Tor 2090<->20A0 aufgeschlossen, gatorszene_r41.c (Runde 41 F)" },
    { RE15_K2080_BANK, RE15_K2080_BIT_ANKUNFT, "RE15_K2080_BIT_ANKUNFT", "2080: Knall (Knall-Bank Satz 0) beim ersten Betreten nach Bit 110, kanister2080_r41.c (Runde 41 G)" },
    { RE15_K2080_BANK, RE15_K2080_BIT_SZENE, "RE15_K2080_BIT_SZENE", "Marvin/John-Kanister-Szene gelaufen, kanister2080_r41.c (Runde 41 G)" },
    { RE15_K2080_BANK, RE15_K2080_BIT_KANISTER, "RE15_K2080_BIT_KANISTER", "Kanister liegt im 2090-Becken, kanister2080_r41.c (Runde 41 G)" },
    { RE15_K2080_BANK, RE15_K2080_BIT_SUEDWURF, "RE15_K2080_BIT_SUEDWURF", "Kanister-Wurf ins Suedbecken (sonst Nord), kanister2080_r41.c (Runde 41 G NB2)" },
};
#define N_PORT ((int)(sizeof k_port / sizeof k_port[0]))

/* Die Pruefung selbst: wie viele der Bits liegen im Original-Satz? (Treffer werden aufgezaehlt.) */
static int kollisionen(const unsigned *bits, int n, unsigned *treffer, int max_treffer)
{
    int k = 0;
    for (int i = 0; i < n; i++)
        if (bits[i] < 256 && s_z.belegt[bits[i]]) { if (k < max_treffer) treffer[k] = bits[i]; k++; }
    return k;
}

static void teil_p(void)
{
    printf("=== P: Bank-9-Bits des Ports gegen den Original-Satz ===\n");
    unsigned alle[64]; int n = 0;
    for (int i = 0; i < N_PORT; i++) {
        PRUEFE(k_port[i].bank == 9, "%s steht in Bank 9 (%u)", k_port[i].makro, k_port[i].bank);
        int im_original = k_port[i].bit < 256 && s_z.belegt[k_port[i].bit];
        PRUEFE(!im_original, "Bit %3u %-30s %s — %s%s", k_port[i].bit, k_port[i].makro, k_port[i].quelle,
               im_original ? "KOLLISION mit " : "im Original frei", im_original ? s_z.erste[k_port[i].bit] : "");
        alle[n++] = k_port[i].bit;
    }
    const int n_plaene = re15_kartenitems_r36_anzahl();
    for (int k = 0; k < n_plaene; k++) {
        const re15_kartenitem_r36_t *z = re15_kartenitems_r36_zeile(k);
        int im_original = s_z.belegt[z->bit];
        PRUEFE(RE15_R36K_BANK == 9 && !im_original, "Bit %3u Lageplan Blatt %u ROOM%04X (k_r36k_lage, Spur N) — %s%s",
               (unsigned)z->bit, (unsigned)z->blatt, (unsigned)z->raum,
               im_original ? "KOLLISION mit " : "im Original frei", im_original ? s_z.erste[z->bit] : "");
        int doppelt = 0;
        for (int i = 0; i < N_PORT; i++) if (k_port[i].bit == z->bit) doppelt = 1;
        for (int j = 0; j < n_plaene; j++)
            if (j != k && re15_kartenitems_r36_zeile(j)->bit == z->bit) doppelt = 1;
        PRUEFE(!doppelt, "Lageplan-Bit %u ist kein anderes Port-Bit", (unsigned)z->bit);
        alle[n++] = z->bit;
    }
    unsigned treffer[16];
    int k = kollisionen(alle, n, treffer, 16);
    PRUEFE(k == 0, "%d Port-Bits gegen 85 Original-Bits: %d Kollision(en)", n, k);

    /* GEGENPROBE: die Spur-N-Belegung bis zum Nachtrag (gen/kartenitems_r36.inc Stand 7a0fd453: 87,88,89,90,91)
     * mit DERSELBEN Pruefung — sie muss genau {90, 91} melden, sonst koennte der Riegel den Befund nicht sehen. */
    static const unsigned k_alt_n[5] = { 87, 88, 89, 90, 91 };
    unsigned alt[16]; int na = kollisionen(k_alt_n, 5, alt, 16);
    PRUEFE(na == 2 && alt[0] == 90 && alt[1] == 91,
           "Gegenprobe alte Belegung 87..91: %d Treffer {%u, %u} — erwartet genau {90, 91} (Riegel waere rot)",
           na, na > 0 ? alt[0] : 0u, na > 1 ? alt[1] : 0u);
    if (na >= 2)
        printf("        90: %s\n        91: %s\n", s_z.erste[90], s_z.erste[91]);
}

/* ================================================================= S */

static int sd_bit(const re15_savedata_t *sd, unsigned bank, unsigned bit)
{   /* dieselbe Anordnung wie g_game.flags: Wort = Bit>>5, Maske 0x80000000 >> (Bit&31) (FUN_8004efe4) */
    return (sd->flags[bank][bit >> 5] & (0x80000000u >> (bit & 31u))) != 0;
}

/* P5 ALT-SPIELSTAENDE.
 * S1 Die eingecheckte Speicherkarte des Nutzers (2026-09-27, Spielstand v8, Fixture von test_r30_karte_nutzerstand):
 *    in keinem belegten Platz steht eines der neuen Lageplan-Bits 77/78 (kein Code hat sie je gesetzt — Dossier 9.6).
 * S2 Ein v0.8.23-Spielstand (Format v9; re15_savedata.h/.c seit v0.8.23 unveraendert), in dem "Pocket Watch" 0x3F
 *    (ROOM2090 @0x00A12, Bit 90) und "Head of Akuma" 0x32 (ROOM2040 @0x01364, Bit 91) genommen sind: ueber die echte
 *    Speicherkarte geschrieben und gelesen -> kein Polizei-Lageplan im Besitz, beide Plaene haengen in ROOM1200/
 *    11D0, und die zwei Original-Items bleiben genommen (Satz still) — der Spielfortschritt bleibt erhalten. */
static void teil_s(void)
{
    printf("=== S: Alt-Spielstaende ===\n");
    /* Karten: die eingecheckte Nutzerkarte; optional eine weitere per RE15_R36KB_KARTE (Diagnose, z.B. die
     * aktuelle Karte neben der exe: re15_port/build/platform/pc/re15_card.mcr - wird nur GELESEN). */
    const char *karten[2] = { NULL, getenv("RE15_R36KB_KARTE") };
#ifdef RE15_R36KB_NUTZERKARTE
    karten[0] = RE15_R36KB_NUTZERKARTE;
#endif
    for (int kk = 0; kk < 2; kk++) {
        if (!karten[kk]) continue;
        int used[RE15_SAVE_SLOTS]; char titles[RE15_SAVE_SLOTS][RE15_MC_TITLE_LEN];
        int n = re15_memcard_list(karten[kk], used, titles);
        PRUEFE(n > 0, "%s gelesen: %d belegte Plaetze", kk == 0 ? "Nutzerkarte 2026-09-27 (Fixture)" : karten[kk], n);
        for (int slot = 0; slot < RE15_SAVE_SLOTS; slot++) {
            if (!used[slot]) continue;
            re15_savedata_t sd;
            memset(&sd, 0, sizeof sd);
            if (re15_memcard_load(karten[kk], slot, &sd) != 0) continue;
            char liste[600]; int len = 0;
            liste[0] = 0;
            for (unsigned b = 0; b < 256; b++)
                if (sd_bit(&sd, 9, b) && len < (int)sizeof liste - 8) len += snprintf(liste + len, sizeof liste - (size_t)len, " %u", b);
            PRUEFE(!sd_bit(&sd, 9, 77) && !sd_bit(&sd, 9, 78),
                   "Platz %d (ROOM%04X, v%u): Bank 9 gesetzt {%s } — 77/78 nicht gesetzt", slot, (unsigned)sd.room,
                   (unsigned)sd.version, liste);
        }
    }
    /* S2: v0.8.23-Stand mit genommenen Items 0x3F/0x32 ueber die Speicherkarte. */
    re15_game_state_init();
    for (int i = 0; i < N_ORIG; i++) re15_game_flag_set(BANK9, (uint8_t)k_orig[i].bit, 1);
    g_current_room_id = 0x2040;
    re15_savedata_t sd, zurueck;
    re15_savedata_capture(&sd, 0, 1);
    PRUEFE(sd.version == 9 && RE15_SAVE_VERSION == 9,
           "Spielstand-Format v%u = v0.8.23 (RE15_SAVE_VERSION 9, re15_savedata.h/.c seit v0.8.23 unveraendert)",
           (unsigned)sd.version);
    const char *karte = "r36_kartenbits_v0823.mcr";
    remove(karte);
    int ok_w = re15_memcard_save(karte, 0, &sd, "R36 KARTENBITS") == 0;
    re15_game_state_init();
    memset(&zurueck, 0, sizeof zurueck);
    uint16_t raum = 0;
    int ok_r = ok_w && re15_memcard_load(karte, 0, &zurueck) == 0 && re15_savedata_restore(&zurueck, &raum) == 0;
    PRUEFE(ok_r && re15_game_flag_get(BANK9, 90) && re15_game_flag_get(BANK9, 91),
           "Stand mit Items 0x3F/0x32 genommen ueber die Speicherkarte geschrieben und geladen (Bits 90/91 gesetzt)");
    PRUEFE((re15_map_owned_bits() & 0x1Fu) == 0,
           "nach dem Laden: kein Polizei-Blatt im Besitz (Maske 0x%02X) — mit der alten Belegung waere es 0x03",
           (unsigned)(re15_map_owned_bits() & 0x1Fu));
    uint8_t stand[256];
    bank9_lesen(stand);
    for (int k = 0; k < re15_kartenitems_r36_anzahl(); k++) {
        const re15_kartenitem_r36_t *z = re15_kartenitems_r36_zeile(k);
        if (z->blatt > 1) continue;                    /* B2 (1) und B1 (0) */
        raum_t r;
        if (raum_laden(&r, z->raum) != 0) continue;
        bank9_schreiben(stand);
        raum_start(&r, z->rect_x + RE15_R36K_RECT / 2, z->rect_z + RE15_R36K_RECT / 2);
        PRUEFE(prop_slot(RE15_R36K_OBJ) >= 0 && g_aot.slots[z->aot_slot].event_id == RE15_R36K_EREIGNIS,
               "nach dem Laden: Plan Blatt %u haengt in ROOM%04X (aufnehmbar, setzt dann Bit %u)",
               (unsigned)z->blatt, (unsigned)z->raum, (unsigned)z->bit);
        free(r.buf);
    }
    for (int i = 0; i < N_ORIG; i++) {
        bank9_schreiben(stand);
        PRUEFE(!orig_raum_scharf(i), "nach dem Laden: ROOM%04X Item 0x%02X bleibt genommen (Satz still, @0x800406f4)",
               (unsigned)k_orig[i].raum, (unsigned)k_orig[i].item);
    }
    remove(karte);
}

int main(void)
{
    printf("=== Runde 36 Nachtrag N: Lageplan-Bits gegen die Bank-9-Bits des Originals ===\n");
    teil_z();
    teil_p();
    teil_v();
    teil_s();
    for (int i = 0; i < N_ORIG; i++) free(s_orig_raum[i].buf);
    if (g_fail) { printf("FAIL\n"); return 1; }
    printf("OK\n");
    return 0;
}
