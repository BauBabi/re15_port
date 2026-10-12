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
 *   L  NACHBESSERUNG 1, PLATTFORM-SPERRE LIFT A (PORT-WAHL nach RE2 ROOM6030 sub10 @0x02F70-7A, Belege
 *      re15_itemband_r37.h (3)) ueber den ECHTEN re15_aot_scan in ROOM2030 UND ROOM2031: Druck aus dem Kabineneingang
 *      (6605,10770)/(6605,10850) Blick Sued bzw. knapp suedlich der oberen Kabine (6605,13245)/(6605,13400) Blick Nord
 *      trifft den Satz (sat 0x31), faehrt aber NICHT; aus der Kabine faehrt sub02; in der Kabine mit Blick nach aussen
 *      trifft der Satz wie im Original nicht (die Sperre fuegt keine Ausloesung hinzu). Lift B / 3050 / 50D0 ohne Sperre.
 *   F  FALLEN-ZENSUS aller Lifte mit Member 0x12 am Spieler (2030/2031 A, 2030 B, 3050, 50D0, je auf/ab) mit der
 *      ECHTEN Port-Kollision (re15_collision_constrain, Radius 450, Kollisionsband): alle Ausloesestellen (25er-Raster
 *      um das Rechteck, 256 Blickrichtungen, Standplatz frei und erreichbar auf dem Startband), Endlage = Ausschiebung
 *      auf dem Zielband NACH der echten Fahrt (Skriptwirkungen wie ROOM3050 Sca_id_set der Schachtzelle), Erreichbarkeit
 *      (Flut im 50er-Raster) gegen die Endlage des vorgesehenen Drucks (an der exe
 *      gefahren; fuer Lift A muss deren Komponente die SPAS-12 bzw. den Tuer-slot-0-Standplatz enthalten). MIT der
 *      Sperre: 0 Fallen ueberall. GEGENPROBE: ohne die Sperre findet der Zensus die Fallen an Lift A (auf UND ab); an
 *      Lift B, 3050, 50D0 gibt es auch ohne Sperre keine -> dort keine Sperre.
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
#include "re15_skeleton.h"   /* re15_cos_q12 / re15_sin_q12 = die Drehung des Scans (Punkt 620 voraus) */

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

/* ------------------------------------------------------------------ L: Plattform-Sperre Lift A ueber den echten Scan */
/* Ein Aktionsdruck an (x,z) mit Blick rot auf Band b durch re15_aot_scan. Rueckgabe = das gefeuerte Unterprogramm
 * (fired_event_id_this_frame, 0 = keines). */
static int lift_druck(int32_t x, int32_t z, int rot, int band)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = x; pl->z = z; pl->y = -(int32_t)band * 1800; pl->rot_y = (int16_t)rot;
    re15_collision_set_band(band);
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    g_scd.message_query = 0; g_scd.message_display_frames = 0;
    g_aot_action_pressed = 1;
    re15_aot_scan(pl->x, pl->z, (uint8_t)g_scd.cam_id);
    g_aot_action_pressed = 0;
    return (int)g_aot.fired_event_id_this_frame;
}

/* Raum mit gesetzten Story-Flags starten; Rueckgabe 0 = ok. */
static int lift_raum(raum_t *r, uint16_t raum, const uint8_t (*flags)[3], int n_flags, int32_t x, int32_t y, int32_t z)
{
    if (raum_laden(r, raum) != 0) return -1;
    re15_game_state_init();
    for (int k = 0; k < n_flags; k++) re15_game_flag_set(flags[k][0], flags[k][1], flags[k][2]);
    raum_start(r, x, y, z);
    return 0;
}

static void teil_l(void)
{
    printf("=== L: Plattform-Sperre Lift A ROOM2030/2031 (PORT-WAHL nach RE2 6030 sub10 @0x02F70-7A), echter Scan ===\n");
    static const uint16_t raeume[2] = { 0x2030, 0x2031 };
    for (int ri = 0; ri < 2; ri++) {
        raum_t r;
        /* Kabine unten: (3,126) Strom (sub01 @0x01E14), (3,222)=0 -> sub00 @0x01CE6 slot 5 sce 3 floor 3. */
        const uint8_t unten[2][3] = { { 3, 126, 1 }, { 3, 222, 0 } };
        if (lift_raum(&r, raeume[ri], unten, 2, 6605, -5400, 11970) != 0) return;
        const re15_aot_t *a5 = &g_aot.slots[5];
        PRUEFE(a5->active && a5->type == RE15_AOT_TYPE_GENERIC && a5->event_id == 2 && a5->band == 3 &&
               a5->sce_flags == 0x31 && a5->x == 6600 && a5->z == 12100 && a5->half_w == 750 && a5->half_h == 750,
               "ROOM%04X slot 5 = sub00 @0x01CE6 (GENERIC, Unterprogramm 2, floor 3, sat 0x31, Rechteck 5850..7350 x "
               "11350..12850)", raeume[ri]);
        PRUEFE(re15_itemband_r37_ist_liftsperre(raeume[ri], 5), "ROOM%04X slot 5 traegt die Plattform-Sperre", raeume[ri]);
        const int z0 = re15_itemband_r37_liftsperre_zahl();
        int ev = lift_druck(6605, 10770, 3072, 3);      /* Kabineneingang, Blick Sued: Punkt 620 voraus z 11390 */
        PRUEFE(ev == 0 && re15_itemband_r37_liftsperre_zahl() == z0 + 1,
               "ROOM%04X Eingang (6605,10770) Blick Sued Band 3: Satz getroffen, Mitte ausserhalb -> KEINE Fahrt (ev %d)",
               raeume[ri], ev);
        ev = lift_druck(6605, 10850, 3072, 3);
        PRUEFE(ev == 0, "ROOM%04X (6605,10850) Blick Sued: keine Fahrt (ev %d)", raeume[ri], ev);
        ev = lift_druck(6605, 11970, 3072, 3);          /* in der Kabine (vorgesehener Weg) */
        PRUEFE(ev == 2, "ROOM%04X Kabine (6605,11970) Blick Sued Band 3: Fahrt sub02 (ev %d)", raeume[ri], ev);
        ev = lift_druck(6605, 11400, 1024, 3);          /* in der Kabine, Blick Nord: Punkt 620 voraus ausserhalb */
        PRUEFE(ev == 0, "ROOM%04X Kabine (6605,11400) Blick Nord: Satz (sat 0x31) nicht getroffen wie im Original (ev %d)",
               raeume[ri], ev);
        free(r.buf);

        /* Kabine oben: (3,222)=1 -> sub00 @0x01D20 slot 4 sce 3 floor 6. */
        const uint8_t oben[2][3] = { { 3, 126, 1 }, { 3, 222, 1 } };
        if (lift_raum(&r, raeume[ri], oben, 2, 6605, -10800, 12061) != 0) return;
        const re15_aot_t *a4 = &g_aot.slots[4];
        PRUEFE(a4->active && a4->type == RE15_AOT_TYPE_GENERIC && a4->event_id == 2 && a4->band == 6 &&
               a4->x == 6600 && a4->z == 12100, "ROOM%04X slot 4 = sub00 @0x01D20 (GENERIC, Unterprogramm 2, floor 6)",
               raeume[ri]);
        ev = lift_druck(6605, 13245, 1024, 6);          /* knapp suedlich der oberen Kabine, Blick Nord */
        PRUEFE(ev == 0, "ROOM%04X (6605,13245) Blick Nord Band 6: Mitte ausserhalb -> KEINE Fahrt (ev %d)", raeume[ri],
               ev);
        ev = lift_druck(6605, 13400, 1024, 6);
        PRUEFE(ev == 0, "ROOM%04X (6605,13400) Blick Nord Band 6: keine Fahrt (ev %d)", raeume[ri], ev);
        ev = lift_druck(6597, 12061, 1024, 6);
        PRUEFE(ev == 2, "ROOM%04X Kabine (6597,12061) Blick Nord Band 6: Fahrt sub02 (ev %d)", raeume[ri], ev);
        free(r.buf);
    }
    /* Kein anderer Lift traegt die Sperre (Zensus Teil F: dort gibt es keine Fallenstellen). */
    raum_t r;
    const uint8_t b_unten[2][3] = { { 3, 126, 1 }, { 3, 220, 0 } };
    if (lift_raum(&r, 0x2030, b_unten, 2, -9800, -5400, 3600) != 0) return;
    PRUEFE(g_aot.slots[7].active && g_aot.slots[7].event_id == 4 && !re15_itemband_r37_ist_liftsperre(0x2030, 7),
           "ROOM2030 Lift B (sub00 @0x01D88 slot 7 -> sub04): keine Sperre");
    free(r.buf);
    const uint8_t l3050[1][3] = { { 3, 82, 0 } };
    if (lift_raum(&r, 0x3050, l3050, 1, -1900, -7200, -8150) != 0) return;
    PRUEFE(g_aot.slots[4].active && g_aot.slots[4].event_id == 4 && !re15_itemband_r37_ist_liftsperre(0x3050, 4),
           "ROOM3050 Lift (sub00 @0x0206C slot 4 -> sub04): keine Sperre");
    free(r.buf);
    const uint8_t l50d0[1][3] = { { 3, 83, 0 } };
    if (lift_raum(&r, 0x50D0, l50d0, 1, -17600, -5400, -14900) != 0) return;
    PRUEFE(g_aot.slots[5].active && g_aot.slots[5].event_id == 4 && !re15_itemband_r37_ist_liftsperre(0x50D0, 5),
           "ROOM50D0 Lift (sub00 @0x012FA slot 5 -> sub04): keine Sperre");
    free(r.buf);
}

/* ------------------------------------------------------------------ F: Fallen-Zensus aller Lifte (echte Kollision) */
/* Netz im 50er-Raster um die Kabine; ein Punkt ist begehbar, wenn re15_collision_constrain (der Spieler-Pfad: Radius
 * PR 450, Maske 1, Kollisionsband) ihn nicht verschiebt. Flut = 4er-Nachbarschaft. */
#define FZ_SCHRITT 50
#define FZ_HALB    16000
#define FZ_N       (2 * FZ_HALB / FZ_SCHRITT + 1)
typedef struct { int32_t x0, z0; uint8_t *v; const re15_rdt_t *rdt; int band; } fz_netz_t;

static int fz_frei(const re15_rdt_t *rdt, int32_t x, int32_t z)
{
    int32_t nx = x, nz = z;
    re15_collision_constrain(rdt, x, z, &nx, &nz);
    return nx == x && nz == z;
}

static void fz_setzen(const re15_rdt_t *rdt, int32_t *x, int32_t *z)
{
    for (int k = 0; k < 16; k++) {
        int32_t nx = *x, nz = *z;
        re15_collision_constrain(rdt, *x, *z, &nx, &nz);
        if (nx == *x && nz == *z) return;
        *x = nx; *z = nz;
    }
}

/* Flut ab dem Punkt (sx,sz) (er muss frei sein): markiert die erreichbaren Netzpunkte. Rueckgabe = Anzahl. */
static long fz_flut(fz_netz_t *g, int32_t sx, int32_t sz)
{
    re15_collision_set_band(g->band);
    memset(g->v, 0, (size_t)FZ_N * FZ_N);
    int *q = (int *)malloc(sizeof(int) * (size_t)FZ_N * FZ_N);
    if (!q) return 0;
    long kopf = 0, ende = 0;
    int i0 = (int)((sx - g->x0) / FZ_SCHRITT), j0 = (int)((sz - g->z0) / FZ_SCHRITT);
    for (int di = 0; di <= 1; di++) for (int dj = 0; dj <= 1; dj++) {
        int i = i0 + di, j = j0 + dj;
        if (i < 0 || j < 0 || i >= FZ_N || j >= FZ_N || g->v[j * FZ_N + i]) continue;
        if (!fz_frei(g->rdt, g->x0 + i * FZ_SCHRITT, g->z0 + j * FZ_SCHRITT)) continue;
        g->v[j * FZ_N + i] = 1; q[ende++] = j * FZ_N + i;
    }
    while (kopf < ende) {
        int k = q[kopf++], i = k % FZ_N, j = k / FZ_N;
        static const int d[4][2] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };
        for (int n = 0; n < 4; n++) {
            int ni = i + d[n][0], nj = j + d[n][1];
            if (ni < 0 || nj < 0 || ni >= FZ_N || nj >= FZ_N || g->v[nj * FZ_N + ni]) continue;
            if (!fz_frei(g->rdt, g->x0 + ni * FZ_SCHRITT, g->z0 + nj * FZ_SCHRITT)) { g->v[nj * FZ_N + ni] = 2; continue; }
            g->v[nj * FZ_N + ni] = 1; q[ende++] = nj * FZ_N + ni;
        }
    }
    free(q);
    return ende;
}

/* Liegt der freie Punkt (x,z) in der gefluteten Komponente? (einer der 4 Netzpunkte um ihn ist erreicht) */
static int fz_in(const fz_netz_t *g, int32_t x, int32_t z)
{
    int i0 = (int)((x - g->x0) / FZ_SCHRITT), j0 = (int)((z - g->z0) / FZ_SCHRITT);
    for (int di = 0; di <= 1; di++) for (int dj = 0; dj <= 1; dj++) {
        int i = i0 + di, j = j0 + dj;
        if (i >= 0 && j >= 0 && i < FZ_N && j < FZ_N && g->v[j * FZ_N + i] == 1) return 1;
    }
    return 0;
}

typedef struct {
    uint16_t raum; uint8_t slot, sub, band_von, band_zu;
    uint8_t flag_bank, flag_bit, flag_wert;   /* Lift-Stellung (sub00-Verzweigung) */
    int32_t ref_x, ref_z;                     /* Druckstelle des vorgesehenen Wegs (an der exe gefahren) */
    int32_t ziel_x, ziel_z;                   /* 0,0 = keins; sonst muss die Referenz-Komponente es enthalten */
    const char *was;
} fz_fahrt_t;

static const fz_fahrt_t k_fahrten[] = {
    { 0x2030, 5, 2, 3, 6, 3, 222, 0,   6605,  11970,  -637, 14499, "2030 Lift A auf  (sub00 @0x01CE6 -> sub02 @0x01E66)" },
    { 0x2030, 4, 2, 6, 3, 3, 222, 1,   6597,  12061,  6535,    11, "2030 Lift A ab   (sub00 @0x01D20 -> sub02 @0x01EA8)" },
    { 0x2031, 5, 2, 3, 6, 3, 222, 0,   6605,  11970,  -637, 14499, "2031 Lift A auf  (Elza, gleiche Offsets)" },
    { 0x2031, 4, 2, 6, 3, 3, 222, 1,   6597,  12061,  6535,    11, "2031 Lift A ab   (Elza, gleiche Offsets)" },
    { 0x2030, 7, 4, 3, 9, 3, 220, 0,  -9800,   3600,     0,     0, "2030 Lift B auf  (sub00 @0x01D88 -> sub04 @0x01F40)" },
    { 0x2030, 6, 4, 9, 3, 3, 220, 1,  -9800,   3600,     0,     0, "2030 Lift B ab   (sub00 @0x01DC2 -> sub04 @0x01F82)" },
    { 0x3050, 4, 4, 4, 0, 3,  82, 0,  -1900,  -8150,     0,     0, "3050 Lift ab     (sub00 @0x0206C -> sub04 @0x023D0)" },
    { 0x3050, 5, 2, 0, 4, 3,  82, 1,  -1900,  -8150,     0,     0, "3050 Lift auf    (sub00 @0x0211E -> sub02 @0x0232C)" },
    { 0x50D0, 5, 4, 3, 0, 3,  83, 0, -17600, -14900,     0,     0, "50D0 Lift ab     (sub00 @0x012FA -> sub04 @0x0144C)" },
    { 0x50D0, 6, 2, 0, 3, 3,  83, 1, -17600, -14900,     0,     0, "50D0 Lift auf    (sub00 @0x0135C -> sub02 @0x013D4)" },
};

typedef struct { int32_t x, z; uint8_t mitte; } fz_stelle_t;

static void teil_f(void)
{
    printf("=== F: Fallen-Zensus der Lifte mit Member 0x12 (echte Port-Kollision, Ausloesestellen im 25er-Raster, "
           "256 Blickrichtungen) ===\n");
    uint8_t *v_von = (uint8_t *)malloc((size_t)FZ_N * FZ_N), *v_zu = (uint8_t *)malloc((size_t)FZ_N * FZ_N);
    fz_stelle_t *st = (fz_stelle_t *)malloc(sizeof(fz_stelle_t) * 60000);
    if (!v_von || !v_zu || !st) { printf("  FAIL: Speicher\n"); g_fail = 1; free(v_von); free(v_zu); free(st); return; }
    for (size_t f = 0; f < sizeof k_fahrten / sizeof k_fahrten[0]; f++) {
        const fz_fahrt_t *F = &k_fahrten[f];
        raum_t r;
        const uint8_t fl[2][3] = { { 3, 126, 1 }, { F->flag_bank, F->flag_bit, F->flag_wert } };
        if (lift_raum(&r, F->raum, fl, 2, F->ref_x, -(int32_t)F->band_von * 1800, F->ref_z) != 0) { g_fail = 1; continue; }
        const re15_aot_t *a = &g_aot.slots[F->slot];
        if (!(a->active && a->type == RE15_AOT_TYPE_GENERIC && a->event_id == F->sub && a->band == F->band_von)) {
            printf("  FAIL: %s: Ausloeser slot %u nicht scharf (aktiv %d typ %d ev %u band %u)\n", F->was, F->slot,
                   a->active, a->type, a->event_id, a->band);
            g_fail = 1; free(r.buf); continue;
        }
        /* Satz VOR der Fahrt festhalten (die Fahrt setzt die Slots per Aot_reset neu). */
        const int32_t mx = a->x, mz = a->z, hw = a->half_w, hh = a->half_h;
        const uint8_t sat = a->sce_flags;
        const int sperre = re15_itemband_r37_ist_liftsperre(F->raum, F->slot);
        fz_netz_t von = { (mx - FZ_HALB) / FZ_SCHRITT * FZ_SCHRITT, (mz - FZ_HALB) / FZ_SCHRITT * FZ_SCHRITT, v_von,
                          &r.rdt, F->band_von };
        fz_netz_t zu  = { von.x0, von.z0, v_zu, &r.rdt, F->band_zu };
        /* (1) Startband im Zustand VOR der Fahrt: Komponente der vorgesehenen Druckstelle, alle Ausloesestellen. */
        re15_collision_set_band(F->band_von);
        const int ref_frei = fz_frei(&r.rdt, F->ref_x, F->ref_z);
        const long n_von = fz_flut(&von, F->ref_x, F->ref_z);
        int stellen = 0;
        for (int32_t px = mx - hw - 700; px <= mx + hw + 700; px += 25) {
            for (int32_t pz = mz - hh - 700; pz <= mz + hh + 700 && stellen < 60000; pz += 25) {
                if (!fz_frei(&r.rdt, px, pz) || !fz_in(&von, px, pz)) continue;   /* Standplatz auf dem Startband */
                const int mitte = (abs(px - mx) <= hw) && (abs(pz - mz) <= hh);
                int trifft = 0;
                for (int d = 0; d < 256 && !trifft; d++) {                             /* Blick 0..4095, Schritt 16 */
                    const int rot = d * 16;
                    if ((sat & 0x40) && mitte) trifft = 1;                             /* @0x80042ea8 */
                    if (sat & 0x20) {                                                  /* @0x80042ef8, 620 voraus */
                        const int32_t c = re15_cos_q12(rot), s = re15_sin_q12(rot);
                        const int32_t qx = px + (int32_t)((620 * c) >> 12), qz = pz - (int32_t)((620 * s) >> 12);
                        if (abs(qx - mx) <= hw && abs(qz - mz) <= hh) trifft = 1;
                    }
                }
                if (!trifft) continue;
                st[stellen].x = px; st[stellen].z = pz; st[stellen].mitte = (uint8_t)mitte; stellen++;
            }
        }
        /* (2) Die ECHTE Fahrt (Unterprogramm mit allen Skriptwirkungen, z.B. ROOM3050 Sca_id_set der Schachtzelle). */
        const int rc = sub_fahren(F->sub, 4000);
        const int band_nach = re15_collision_debug_band();
        /* (3) Zielband im Zustand NACH der Fahrt: Endlage der vorgesehenen Druckstelle und ihre Komponente. */
        re15_collision_set_band(F->band_zu);
        int32_t ex = F->ref_x, ez = F->ref_z;
        fz_setzen(&r.rdt, &ex, &ez);
        const long n_zu = fz_flut(&zu, ex, ez);
        const int ziel_ok = (F->ziel_x == 0 && F->ziel_z == 0) || fz_in(&zu, F->ziel_x, F->ziel_z);
        int fallen_ohne = 0, fallen_mit = 0, gesperrt = 0;
        int32_t fx0 = 1 << 30, fx1 = -(1 << 30), fz0 = 1 << 30, fz1 = -(1 << 30), ez0 = 0, ez1 = 0;
        for (int k = 0; k < stellen; k++) {
            const int sperrt = sperre && !st[k].mitte;
            if (sperrt) gesperrt++;
            int32_t qx = st[k].x, qz = st[k].z;
            fz_setzen(&r.rdt, &qx, &qz);
            if (fz_in(&zu, qx, qz)) continue;
            fallen_ohne++;
            if (!sperrt) fallen_mit++;
            if (st[k].x < fx0) fx0 = st[k].x;
            if (st[k].x > fx1) fx1 = st[k].x;
            if (st[k].z < fz0) { fz0 = st[k].z; ez0 = qz; }
            if (st[k].z > fz1) { fz1 = st[k].z; ez1 = qz; }
        }
        printf("  %s: Referenz (%d,%d) frei %d -> Fahrt rc %d Band %d -> Endlage (%d,%d); Komponenten %ld/%ld Netzpunkte, "
               "Ziel erreichbar %d\n", F->was, F->ref_x, F->ref_z, ref_frei, rc, band_nach, ex, ez, n_von, n_zu, ziel_ok);
        printf("    Ausloesestellen %d, Sperre %s (%d gesperrt); Fallen ohne Sperre %d", stellen, sperre ? "ja" : "nein",
               gesperrt, fallen_ohne);
        if (fallen_ohne) printf(" (x %d..%d z %d..%d -> Endlage z %d / %d)", fx0, fx1, fz0, fz1, ez0, ez1);
        printf(", mit Sperre %d\n", fallen_mit);
        PRUEFE(ref_frei && rc == 0 && band_nach == F->band_zu && n_von > 100 && n_zu > 100 && ziel_ok && stellen > 0,
               "%s: vorgesehene Fahrt endet in der Komponente des Ziels", F->was);
        PRUEFE(fallen_mit == 0, "%s: KEINE Ausloesestelle fuehrt in eine Falle (%d)", F->was, fallen_mit);
        if (sperre)
            PRUEFE(fallen_ohne > 0, "%s: GEGENPROBE ohne Sperre gaebe es Fallen (%d) — der Zensus misst", F->was,
                   fallen_ohne);
        else
            PRUEFE(fallen_ohne == 0, "%s: auch ohne Sperre keine Falle (%d) -> keine Sperre noetig", F->was, fallen_ohne);
        free(r.buf);
    }
    free(v_von); free(v_zu); free(st);
}

int main(void)
{
    printf("=== Runde 37 Spur D: Items nur vom eigenen Band (@0x80042cac-ccc) + Member 0x12 = Band (@0x80041228) ===\n");
    teil_z();
    teil_m();
    teil_g();
    teil_l();
    teil_f();
    if (g_fail) { printf("FAIL\n"); return 1; }
    printf("OK\n");
    return 0;
}
