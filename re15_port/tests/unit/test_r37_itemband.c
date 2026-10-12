/* test_r37_itemband.c — Runde 37 Spur D "itemband". Dossier: analysis/befunde_runde37/D_itemband.md.
 *
 * Nutzer (AUFTRAG.md #8): "Items auf erhoehten Flaechen lassen sich vom falschen Stockwerk aufnehmen, z.B. die Super
 * Redhawk in ROOM3050 ohne die Kiste." Zwei Original-Mechanismen (Belege include/re15_itemband_r37.h):
 *   (1) FUN_80042bac prueft fuer JEDEN Satz rec[2] == Entity+0x82 (@0x80042cac-ccc, Bit 0x80 = egal) VOR dem
 *       sce-Dispatch — auch fuer ITEM (sce 9 -> 0x8007469c[9] = 0x80043328); Item_aot_set legt rec = pc+2 an
 *       (@0x800406cc), rec[2] = pc[4] = floor.
 *   (2) Member_set 0x12 -> FUN_8004116c Tabelle 0x80010c8c[18] -> `80041228 sb a2,130(a0)`: nach Work_set(1,0)
 *       (@0x80040da4-b0 = 0x800ACA54) ist das 0x800ACAD6 = das Kollisionsband des Spielers.
 *
 *   Z  ZENSUS ueber alle ausgelieferten RDTs (Laengen = scd_opcode_size_at, Walk wie test_r36_kartenbits.c /
 *      tools/scd_dump_room.py): 206 RDTs, 40674 Opcodes, 0 Desync, 164 Item_aot_set; GENAU die 12 Saetze mit
 *      floor != 0 (Liste k_hoch, = D_belege/zensus_itemband.py), keiner mit Bit 0x80; 22 Schreiber des Members 0x12,
 *      alle Opcode 0x34; GENAU die 17 am Spieler (letztes Work_set der Region kind 1) mit ihren Werten (k_spieler).
 *   G  GATE je Item-Satz ueber das ECHTE Programm: Raum starten (scd_room_reenter), den Item_aot_set-Satz mit dem
 *      echten Opcode-Handler ausfuehren (Mini-Faden: Satz + Evt_end), alle anderen AOT-Slots aus (nur der Satz zaehlt),
 *      Spieler so stellen, dass die Geometrie des Satzes trifft (sat 0x40: Mitte; sonst Punkt 620 voraus), Aktion,
 *      re15_aot_scan: das Item-Modal startet GENAU bei Spielerband == floor. Fuer die 12 erhoehten Saetze laeuft das
 *      Modal bis zum Ende (Taken-Bit (9,tk) gesetzt). GEGENPROBE: ohne den Haken waere jedes Band FREI — der Zaehler
 *      re15_itemband_r37_gesperrt_zahl() muss steigen.
 *   M  MEMBER 0x12 ueber die ECHTEN Subs: ROOM3050 sub04 (Lift ab, @0x023D0 `34 12 00 00`) -> Band 0 / y 0 / (3,82)=1,
 *      sub02 (Lift auf, @0x0232C) -> Band 4; ROOM2030 sub02 (Lift A @0x01E66/@0x01EA8) 3 -> 6 -> 3, sub04 (Lift B
 *      @0x01F40/@0x01F82) 3 -> 9 -> 3; ROOM50D0 sub02/sub04 (Frage mit JA) 0 -> 3 -> 0. GEGENPROBE: Member 0x12 an
 *      einem Gegner-Slot laesst das Kollisionsband stehen.
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
#include "re15_inventory.h"
#include "re15_item_modal.h"
#include "re15_itemband_r37.h"

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
extern uint8_t          g_aot_action_pressed;

static int g_fail = 0;
#define PRUEFE(cond, ...) do { if (!(cond)) { printf("  FAIL: " __VA_ARGS__); printf("\n"); g_fail = 1; } \
                               else { printf("  ok:   " __VA_ARGS__); printf("\n"); } } while (0)

#define PAD_CONFIRM 0x4000u   /* virtuelles Bestaetigen-Bit (probe_r17_munition.c) */

/* ------------------------------------------------------------------ Raum laden / starten (wie test_r36_kartenbits) */
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

/* Raumstart wie der Tuer-Weg (scd_room_reenter mit allen Port-Installern), Spielflags bleiben (g_game). */
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
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = px; pl->y = py; pl->z = pz; pl->rot_y = 0;
    re15_collision_set_band(re15_collision_band_from_y(pl->y));
    re15_msg_load_room_block(r->rdt.messages, r->rdt.messages_size);
    scd_room_reenter(&r->rdt, px, pz, 0);
}

/* Faden bis zum Ende ticken; Ja/Nein-Fragen mit JA (0x4000 alle 4 Bilder, wie test_r36_kartenbits plan_nehmen). */
static int faden_laufen(int slot, long max_bilder)
{
    for (long fr = 0; fr < max_bilder && g_scd.threads[slot].active; fr++) {
        const unsigned char *raw; int len, id;
        g_scd_pad_edge = (fr > 2 && (fr % 4) == 0) ? (uint16_t)PAD_CONFIRM : 0u;
        g_scd_pad_held = 0;
        scd_vm_tick();
        re15_msg_tick(&raw, &len, &id);
        g_scd_pad_edge = 0;
    }
    return !g_scd.threads[slot].active;
}

/* ------------------------------------------------------------------ Z: Zensus */
static uint16_t le16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }
static int16_t  les16(const uint8_t *p) { return (int16_t)le16(p); }
static uint32_t le32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

typedef struct {                     /* ein Item_aot_set aus dem Zensus */
    uint16_t raum; uint32_t pc; uint8_t slot, sce, sat, floor, item, menge; uint16_t tk;
    int16_t x, z, w, d; uint8_t bytes[30]; uint8_t len;
} item_satz_t;

#define MAX_ITEMS 200
static item_satz_t s_items[MAX_ITEMS];
static int         s_n_items = 0;

typedef struct { uint16_t raum; uint32_t pc; uint8_t op; int16_t wert; int kind, idx; } member_satz_t;
#define MAX_MEMBER 64
static member_satz_t s_member[MAX_MEMBER];
static int           s_n_member = 0;

static long s_rdts = 0, s_opcodes = 0, s_desync = 0;

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

static uint32_t sprungziel(const uint8_t *d, uint32_t pc)
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

static void region_laufen(const uint8_t *d, uint32_t start, uint32_t ende, unsigned room)
{
    uint32_t pc = start, maxf = start;
    int ws_kind = -1, ws_idx = -1;                  /* letztes Work_set (0x2E) dieser Region */
    while (pc < ende) {
        int sz = scd_opcode_size_at(d + pc);
        if (sz <= 0 || pc + (uint32_t)sz > ende) { s_desync++; return; }
        s_opcodes++;
        const uint8_t op = d[pc];
        if (op == 0x2E) { ws_kind = d[pc + 1]; ws_idx = d[pc + 2]; }
        else if (op == 0x50 && s_n_items < MAX_ITEMS) {          /* Item_aot_set @0x80040644 */
            item_satz_t *it = &s_items[s_n_items++];
            const int lang = (d[pc + 3] & 0x80) != 0;
            it->raum = (uint16_t)room; it->pc = pc; it->slot = d[pc + 1]; it->sce = d[pc + 2];
            it->sat = d[pc + 3]; it->floor = d[pc + 4];
            it->x = les16(d + pc + 6); it->z = les16(d + pc + 8); it->w = les16(d + pc + 10); it->d = les16(d + pc + 12);
            it->item  = d[pc + (lang ? 22 : 14)];
            it->menge = d[pc + (lang ? 24 : 16)];
            it->tk    = le16(d + pc + (lang ? 26 : 18));
            it->len = (uint8_t)sz;
            memcpy(it->bytes, d + pc, (size_t)sz);
        } else if (op == 0x34 || op == 0x35 || op == 0x5A || op == 0x5B) {
            const uint8_t mid = (op == 0x34 || op == 0x35) ? d[pc + 1] : (op == 0x5A ? d[pc + 3] : d[pc + 2]);
            if (mid == RE15_R37D_MEMBER_BAND && s_n_member < MAX_MEMBER) {
                member_satz_t *m = &s_member[s_n_member++];
                m->raum = (uint16_t)room; m->pc = pc; m->op = op;
                m->wert = (op == 0x34) ? les16(d + pc + 2) : (int16_t)0x7fff;
                m->kind = ws_kind; m->idx = ws_idx;
            }
        }
        uint32_t t = sprungziel(d, pc);
        if (t > maxf) maxf = t;
        if (op == 0x01 && pc + 2 > maxf) return;
        pc += (uint32_t)sz;
    }
}

static void zensus_raum(const uint8_t *d, size_t n, unsigned room)
{
    for (int k = 0; k < 2; k++) {
        uint32_t ss = le32(d + 0x40 + 4 * k);
        if (ss == 0 || ss >= n || ss + 2 > n) continue;
        uint32_t se = sektion_ende(d, n, ss);
        uint16_t first = le16(d + ss);
        if (first < 2 || (first & 1) || ss + first > n) continue;
        uint32_t offs[512]; int cnt = 0;
        for (uint32_t i = 0; i < first / 2u && cnt < 512; i++) {
            uint16_t o = le16(d + ss + 2 * i);
            if (!o || ss + o >= n) continue;
            int dup = 0;
            for (int j = 0; j < cnt; j++) if (offs[j] == o) dup = 1;
            if (!dup) offs[cnt++] = o;
        }
        for (int i = 0; i < cnt; i++)
            for (int j = i + 1; j < cnt; j++)
                if (offs[j] < offs[i]) { uint32_t t = offs[i]; offs[i] = offs[j]; offs[j] = t; }
        for (int i = 0; i < cnt; i++) {
            uint32_t e = (i + 1 < cnt) ? offs[i + 1] : se - ss;
            if (e > se - ss) e = se - ss;
            if (e > offs[i]) region_laufen(d, ss + offs[i], ss + e, room);
        }
    }
}

/* Die 12 Item_aot_set mit floor != 0 (D_belege/zensus_itemband.py; Taken-Bits = Bank 9, @0x80040680). */
static const struct { uint16_t raum; uint32_t pc; uint8_t slot, floor, item; uint16_t tk; const char *was; } k_hoch[] = {
    { 0x2030, 0x01BC0, 10, 1, 0x16, 108, "SHOTGUN SHELLS x14 (Band-1-Gang, Treppe slot 8)" },
    { 0x2030, 0x01BD6, 11, 6, 0x0D, 109, "SPAS-12 (Band 6, Lift A sub02)" },
    { 0x2031, 0x01BC0, 10, 1, 0x16, 108, "SHOTGUN SHELLS x14 (Elza)" },
    { 0x2031, 0x01BD6, 11, 6, 0x0D, 109, "SPAS-12 (Elza)" },
    { 0x2040, 0x01364,  7, 3, 0x32,  91, "Head of Akuma (Halle Band 3)" },
    { 0x2041, 0x01364,  7, 3, 0x32,  91, "Head of Akuma (Elza)" },
    { 0x2090, 0x00A12,  5, 1, 0x3F,  90, "Pocket Watch (Plattform Band 1, Tuer aus 20A0)" },
    { 0x2091, 0x00A12,  5, 1, 0x3F,  90, "Pocket Watch (Elza)" },
    { 0x3010, 0x02094,  5, 1, 0x16,  20, "SHOTGUN SHELLS x7 (Steg Band 1, Klettern r0[4])" },
    { 0x3011, 0x02094,  5, 1, 0x16,  20, "SHOTGUN SHELLS x7 (Elza)" },
    { 0x3050, 0x01EC6,  6, 1, 0x07,  92, "SUPER REDHAWK (Kistendach Band 1)" },
    { 0x3051, 0x01EC6,  6, 1, 0x07,  92, "SUPER REDHAWK (Elza)" },
};
#define N_HOCH ((int)(sizeof k_hoch / sizeof k_hoch[0]))

/* Die 17 Member_set 0x12 am Spieler (Work_set kind 1 @0x80040da4) mit ihren Werten. */
static const struct { uint16_t raum; uint32_t pc; int16_t wert; } k_spieler[] = {
    { 0x1170, 0x01756, 4 },                                                    /* sub14 Pos_set Hof (y -7200) */
    { 0x2030, 0x01E66, 6 }, { 0x2030, 0x01EA8, 3 }, { 0x2030, 0x01F40, 9 }, { 0x2030, 0x01F82, 3 },
    { 0x2031, 0x01E66, 6 }, { 0x2031, 0x01EA8, 3 }, { 0x2031, 0x01F40, 9 }, { 0x2031, 0x01F82, 3 },
    { 0x3050, 0x0232C, 4 }, { 0x3050, 0x023D0, 0 }, { 0x3051, 0x0232C, 4 }, { 0x3051, 0x023D0, 0 },
    { 0x50D0, 0x013D4, 3 }, { 0x50D0, 0x0144C, 0 }, { 0x50D1, 0x013D4, 3 }, { 0x50D1, 0x0144C, 0 },
};
#define N_SPIELER ((int)(sizeof k_spieler / sizeof k_spieler[0]))

static void teil_z(void)
{
    printf("=== Z: Zensus Item_aot_set (floor) + Member 0x12 ueber alle ausgelieferten RDTs ===\n");
    s_n_items = 0; s_n_member = 0; s_rdts = s_opcodes = s_desync = 0;
    for (unsigned st = 1; st <= 6; st++)
        for (unsigned hi = 0; hi < 0x100; hi++)
            for (unsigned lo = 0; lo < 2; lo++) {
                unsigned room = (st << 12) | (hi << 4) | lo;
                char p[600]; size_t n = 0;
                snprintf(p, sizeof p, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, st, room);
                uint8_t *d = slurp(p, &n);
                if (!d) continue;
                if (n < 0x48) { free(d); continue; }
                s_rdts++;
                zensus_raum(d, n, room);
                free(d);
            }
    printf("  %ld RDTs, %ld Opcodes, %ld Desync, %d Item_aot_set, %d Schreiber Member 0x12\n",
           s_rdts, s_opcodes, s_desync, s_n_items, s_n_member);
    PRUEFE(s_rdts == 206 && s_desync == 0 && s_opcodes == 40674,
           "Walk vollstaendig: 206 RDTs / 0 Desync / 40674 Opcodes (= test_r36_kartenbits)");
    PRUEFE(s_n_items == 164, "164 Item_aot_set (%d)", s_n_items);

    int hoch = 0, egal = 0, gefunden = 0;
    for (int i = 0; i < s_n_items; i++) {
        const item_satz_t *it = &s_items[i];
        if (it->floor & RE15_R37D_BAND_EGAL) egal++;
        if (it->floor == 0) continue;
        hoch++;
        int bekannt = 0;
        for (int k = 0; k < N_HOCH; k++)
            if (k_hoch[k].raum == it->raum && k_hoch[k].pc == it->pc && k_hoch[k].slot == it->slot &&
                k_hoch[k].floor == it->floor && k_hoch[k].item == it->item && k_hoch[k].tk == it->tk) bekannt = 1;
        gefunden += bekannt;
        printf("  ROOM%04X @0x%05X slot %u floor %u item 0x%02X tk %u rect (%d,%d,%d,%d) %s\n", it->raum,
               (unsigned)it->pc, it->slot, it->floor, it->item, it->tk, it->x, it->z, it->w, it->d,
               bekannt ? "" : "<- NICHT IN k_hoch");
    }
    PRUEFE(hoch == N_HOCH && gefunden == N_HOCH, "genau die %d erhoehten Saetze der Liste (%d, davon bekannt %d)",
           N_HOCH, hoch, gefunden);
    PRUEFE(egal == 0, "kein Item_aot_set mit Bit 0x80 (Band egal, @0x80042cb4): %d", egal);

    int n34 = 0, spieler = 0, spieler_bekannt = 0, gegner = 0;
    for (int i = 0; i < s_n_member; i++) {
        const member_satz_t *m = &s_member[i];
        if (m->op == 0x34) n34++;
        if (m->kind == 1) {
            spieler++;
            int bekannt = 0;
            for (int k = 0; k < N_SPIELER; k++)
                if (k_spieler[k].raum == m->raum && k_spieler[k].pc == m->pc && k_spieler[k].wert == m->wert)
                    bekannt = 1;
            spieler_bekannt += bekannt;
            printf("  Spieler: ROOM%04X @0x%05X Member_set 0x12 = %d%s\n", m->raum, (unsigned)m->pc, m->wert,
                   bekannt ? "" : "  <- NICHT IN k_spieler");
        } else if (m->kind == 2) {
            gegner++;
        }
    }
    PRUEFE(s_n_member == 22 && n34 == 22, "22 Schreiber des Members 0x12, alle Member_set 0x34 (%d / %d)",
           s_n_member, n34);
    PRUEFE(spieler == N_SPIELER && spieler_bekannt == N_SPIELER,
           "genau die %d Schreiber am Spieler (Work_set kind 1) mit ihren Werten (%d, bekannt %d)",
           N_SPIELER, spieler, spieler_bekannt);
    PRUEFE(gegner == 5, "5 Schreiber an Figuren (Work_set kind 2: ROOM10B1 sub00/sub03, ROOM11C0 sub02/04/07): %d",
           gegner);
}

/* ------------------------------------------------------------------ G: Gate je Item-Satz */
static int modal_fertig(void)
{
    int wache = 0;
    while (re15_item_modal_active() && !re15_item_modal_prompt_ready() && wache++ < 2000)
        re15_item_modal_tick(0, 0);
    wache = 0;
    while (re15_item_modal_active() && wache++ < 2000)
        re15_item_modal_tick((uint16_t)PAD_CONFIRM, 0);
    return !re15_item_modal_active();
}

/* Satz `it` mit dem echten Handler ausfuehren (Mini-Faden: Satzbytes + Evt_end). */
static int satz_ausfuehren(const item_satz_t *it)
{
    static uint8_t prog[40];
    memset(prog, 0, sizeof prog);
    memcpy(prog, it->bytes, it->len);
    prog[it->len] = 0x01;                       /* Evt_end */
    prog[it->len + 1] = 0x00;
    int slot = -1;
    for (int s = SCD_EVENT_SLOT_FIRST; s <= SCD_EVENT_SLOT_LAST; s++)
        if (!g_scd.threads[s].active) { slot = s; break; }
    if (slot < 0 || scd_thread_start(slot, prog) != 0) return -1;
    for (int k = 0; k < 8 && g_scd.threads[slot].active; k++) scd_vm_tick();
    return g_scd.threads[slot].active ? -2 : 0;
}

/* Ein Aktionsdruck bei Band b. Rueckgabe 1 = das Item-Modal ist gestartet. */
static int druck_bei_band(const item_satz_t *it, int band)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int32_t cx = (int32_t)it->x + (int32_t)it->w / 2, cz = (int32_t)it->z + (int32_t)it->d / 2;
    if (it->sat & 0x40) { pl->x = cx; pl->z = cz; pl->rot_y = 0; }          /* Mitte @0x80042ea8 */
    else                { pl->x = cx - 620; pl->z = cz; pl->rot_y = 0; }    /* 620 voraus, Blick +X @0x80042ef8 */
    pl->y = -(int32_t)band * 1800;
    re15_collision_set_band(band);
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    g_scd.message_query = 0; g_scd.message_display_frames = 0;
    g_aot_action_pressed = 1;
    re15_aot_scan(pl->x, pl->z, (uint8_t)g_scd.cam_id);
    g_aot_action_pressed = 0;
    return re15_item_modal_active();
}

static void teil_g(void)
{
    printf("=== G: Band-Gate @0x80042cac-ccc je Item-Satz (echter Handler, re15_aot_scan) ===\n");
    int geprueft = 0, null_rect = 0, sce0 = 0, falsch = 0, hoch_ok = 0, erwartet = 0;
    const int sperr0 = re15_itemband_r37_gesperrt_zahl();
    for (int i = 0; i < s_n_items; i++) {
        const item_satz_t *it = &s_items[i];
        if (it->w == 0 && it->d == 0) { null_rect++; continue; }      /* (0,0,0,0): Geometrie trifft nie */
        if (it->sce == 0) { sce0++; continue; }                        /* traege Erstinstallation (Aot_reset spaeter) */
        raum_t r;
        if (raum_laden(&r, it->raum) != 0) return;
        re15_game_state_init();
        re15_inv_init();
        raum_start(&r, 0, -(int32_t)it->floor * 1800, 0);
        if (satz_ausfuehren(it) != 0) { printf("  FAIL: ROOM%04X @0x%05X Faden\n", it->raum, (unsigned)it->pc);
                                        g_fail = 1; free(r.buf); continue; }
        const re15_aot_t *a = &g_aot.slots[it->slot];
        if (!(a->active && a->type == RE15_AOT_TYPE_ITEM && a->band == it->floor)) {
            printf("  FAIL: ROOM%04X @0x%05X slot %u nicht scharf (aktiv %d typ %d band %u)\n", it->raum,
                   (unsigned)it->pc, it->slot, a->active, a->type, a->band);
            g_fail = 1; free(r.buf); continue;
        }
        for (int s = 0; s < RE15_AOT_MAX; s++) if (s != it->slot) g_aot.slots[s].active = 0;   /* nur dieser Satz */
        int baender[4], nb = 0;
        baender[nb++] = it->floor + 1;
        if (it->floor > 0) baender[nb++] = it->floor - 1;
        if (it->floor != 4) baender[nb++] = 4;
        int ok = 1;
        erwartet += nb;
        for (int k = 0; k < nb; k++)
            if (druck_bei_band(it, baender[k])) {
                printf("  FAIL: ROOM%04X @0x%05X slot %u floor %u: Modal bei Band %d\n", it->raum, (unsigned)it->pc,
                       it->slot, it->floor, baender[k]);
                ok = 0; modal_fertig();
            }
        const int frei = druck_bei_band(it, it->floor);
        if (!frei) {
            printf("  FAIL: ROOM%04X @0x%05X slot %u floor %u: KEIN Modal bei Band %u (Geometrie/Gate)\n", it->raum,
                   (unsigned)it->pc, it->slot, it->floor, it->floor);
            ok = 0;
        } else if (it->floor != 0) {
            const int fertig = modal_fertig();
            const int genommen = it->tk ? re15_game_flag_get(9, (uint8_t)it->tk) : 1;
            printf("  ROOM%04X @0x%05X slot %u floor %u item 0x%02X: Band %u -> Modal, fertig %d, (9,%u) = %d; "
                   "Baender", it->raum, (unsigned)it->pc, it->slot, it->floor, it->item, it->floor, fertig, it->tk,
                   genommen);
            for (int k = 0; k < nb; k++) printf(" %d", baender[k]);
            printf(" -> kein Modal\n");
            if (fertig && genommen) hoch_ok++; else ok = 0;
        } else {
            modal_fertig();
        }
        if (!ok) falsch++;
        geprueft++;
        free(r.buf);
    }
    const int sperr = re15_itemband_r37_gesperrt_zahl() - sperr0;
    printf("  %d Saetze geprueft, %d mit Rechteck (0,0,0,0), %d mit sce 0 uebersprungen; %d gesperrte Druecke\n",
           geprueft, null_rect, sce0, sperr);
    PRUEFE(geprueft == 153 && null_rect == 9 && sce0 == 2, "153 Saetze mit Geometrie (+9 Null-Rechteck, +2 sce 0)");
    PRUEFE(falsch == 0, "jeder Satz: Modal GENAU bei Spielerband == floor (falsch: %d)", falsch);
    PRUEFE(hoch_ok == N_HOCH, "die %d erhoehten Saetze: auf ihrem Band aufgenommen, Taken-Bit gesetzt (%d)",
           N_HOCH, hoch_ok);
    PRUEFE(sperr == erwartet, "GEGENPROBE: der Haken hat JEDEN Druck auf falschem Band gesperrt (%d von %d)", sperr,
           erwartet);
}

/* ------------------------------------------------------------------ M: Member 0x12 ueber die echten Subs */
static int sub_fahren(uint8_t ereignis, long max_bilder)
{
    int slot = scd_event_fire(ereignis);
    if (slot < 0) return -1;
    return faden_laufen(slot, max_bilder) ? 0 : -2;
}

static void teil_m(void)
{
    printf("=== M: Member_set 0x12 am Spieler = Kollisionsband (@0x80041228 sb a2,130(a0)) ===\n");
    raum_t r;
    /* ROOM3050: Lift sub04 (ab) / sub02 (auf). */
    if (raum_laden(&r, 0x3050) != 0) return;
    re15_game_state_init();
    raum_start(&r, -1900, -7200, -8150);
    re15_collision_set_band(4);
    const int m0 = re15_itemband_r37_member_zahl();
    int rc = sub_fahren(4, 3000);
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    printf("  ROOM3050 sub04: rc %d, y %d, Band %d, floor %u, (3,82) = %d\n", rc, pl->y, re15_collision_debug_band(),
           pl->floor, re15_game_flag_get(3, 82));
    PRUEFE(rc == 0 && pl->y == 0 && re15_collision_debug_band() == 0 && pl->floor == 0 && re15_game_flag_get(3, 82) == 1,
           "ROOM3050 sub04 (Lift ab, @0x023D0 34 12 00 00): Band 4 -> 0, y 0, (3,82) = 1 — Original O2 C: band 0");
    rc = sub_fahren(2, 3000);
    printf("  ROOM3050 sub02: rc %d, y %d, Band %d, (3,82) = %d\n", rc, pl->y, re15_collision_debug_band(),
           re15_game_flag_get(3, 82));
    PRUEFE(rc == 0 && pl->y == -7200 && re15_collision_debug_band() == 4 && re15_game_flag_get(3, 82) == 0,
           "ROOM3050 sub02 (Lift auf, @0x0232C 34 12 04 00): Band 0 -> 4, y -7200");
    free(r.buf);

    /* ROOM2030: Lift A sub02 (Band 3 <-> 6, (3,222)), Lift B sub04 (Band 3 <-> 9, (3,220)). */
    if (raum_laden(&r, 0x2030) != 0) return;
    re15_game_state_init();
    raum_start(&r, 6600, -5400, 12100);
    re15_collision_set_band(3);
    rc = sub_fahren(2, 3000);
    printf("  ROOM2030 sub02 auf: rc %d, y %d, Band %d\n", rc, pl->y, re15_collision_debug_band());
    PRUEFE(rc == 0 && pl->y == -10800 && re15_collision_debug_band() == 6,
           "ROOM2030 sub02 (Lift A auf, @0x01E66 34 12 06 00): Band 3 -> 6, y -10800");
    rc = sub_fahren(2, 3000);
    printf("  ROOM2030 sub02 ab: rc %d, y %d, Band %d\n", rc, pl->y, re15_collision_debug_band());
    PRUEFE(rc == 0 && pl->y == -5400 && re15_collision_debug_band() == 3,
           "ROOM2030 sub02 (Lift A ab, @0x01EA8 34 12 03 00): Band 6 -> 3, y -5400");
    rc = sub_fahren(4, 3000);
    printf("  ROOM2030 sub04 auf: rc %d, y %d, Band %d\n", rc, pl->y, re15_collision_debug_band());
    PRUEFE(rc == 0 && pl->y == -16200 && re15_collision_debug_band() == 9,
           "ROOM2030 sub04 (Lift B auf, @0x01F40 34 12 09 00): Band 3 -> 9, y -16200");
    rc = sub_fahren(4, 3000);
    printf("  ROOM2030 sub04 ab: rc %d, y %d, Band %d\n", rc, pl->y, re15_collision_debug_band());
    PRUEFE(rc == 0 && pl->y == -5400 && re15_collision_debug_band() == 3,
           "ROOM2030 sub04 (Lift B ab, @0x01F82 34 12 03 00): Band 9 -> 3, y -5400");
    free(r.buf);

    /* ROOM50D0: Lift mit Frage (Message_on 0, Ck (12,31)=0 = JA): sub02 auf (Band 3), sub04 ab (Band 0). */
    if (raum_laden(&r, 0x50D0) != 0) return;
    re15_game_state_init();
    raum_start(&r, -18200 + 600, 0, -14400 + 600);
    re15_collision_set_band(0);
    rc = sub_fahren(2, 4000);
    printf("  ROOM50D0 sub02: rc %d, y %d, Band %d\n", rc, pl->y, re15_collision_debug_band());
    PRUEFE(rc == 0 && pl->y == -5400 && re15_collision_debug_band() == 3,
           "ROOM50D0 sub02 (@0x013D4 34 12 03 00): Band 0 -> 3, y -5400 (Tueren slot 0/2 floor 3)");
    rc = sub_fahren(4, 4000);
    printf("  ROOM50D0 sub04: rc %d, y %d, Band %d\n", rc, pl->y, re15_collision_debug_band());
    PRUEFE(rc == 0 && pl->y == 0 && re15_collision_debug_band() == 0,
           "ROOM50D0 sub04 (@0x0144C 34 12 00 00): Band 3 -> 0, y 0 (Tueren slot 1/3 floor 0)");
    free(r.buf);
    PRUEFE(re15_itemband_r37_member_zahl() - m0 >= 8, "Haken re15_itemband_r37_member_band lief je Fahrt (%d)",
           re15_itemband_r37_member_zahl() - m0);

    /* GEGENPROBE: Member 0x12 an einem Figuren-Slot (Work_set kind 2, z.B. ROOM11C0 sub07 @0x01C66) aendert das
     * Kollisionsband des Spielers NICHT, nur +0x82 der Figur. */
    re15_collision_set_band(5);
    int npc = re15_actor_alloc(0x10);
    if (npc > 0) {
        re15_actor_set_member(npc, RE15_R37D_MEMBER_BAND, 2);
        PRUEFE(re15_collision_debug_band() == 5 && g_actors[npc].floor == 2,
               "GEGENPROBE Figur-Slot %d: Member 0x12 = 2 -> Figur.floor 2, Kollisionsband bleibt 5 (%d)", npc,
               re15_collision_debug_band());
        re15_actor_free(npc);
    }
    re15_actor_set_member(RE15_ACTOR_SLOT_PLAYER, RE15_R37D_MEMBER_BAND, 1);
    PRUEFE(re15_collision_debug_band() == 1 && g_actors[RE15_ACTOR_SLOT_PLAYER].floor == 1 &&
           re15_actor_get_member(RE15_ACTOR_SLOT_PLAYER, RE15_R37D_MEMBER_BAND) == 1,
           "Spieler-Slot: Member 0x12 = 1 -> Kollisionsband 1, +0x82 (Member_cmp liest denselben Wert) 1");
}

int main(void)
{
    printf("=== Runde 37 Spur D: Items nur vom eigenen Band (@0x80042cac-ccc) + Member 0x12 = Band (@0x80041228) ===\n");
    teil_z();
    teil_m();
    teil_g();
    if (g_fail) { printf("FAIL\n"); return 1; }
    printf("OK\n");
    return 0;
}
