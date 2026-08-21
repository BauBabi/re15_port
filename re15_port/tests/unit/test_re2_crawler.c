/* test_re2_crawler.c — PIN: DER RE2-KRIECHER (ROOM1030-Kriechtor unter dem RE2-Flavor).
 *
 * NUTZER-REPORT 2026-08-21: "Bei RE2 AI in der Lobby kriechen die Zombies in der Cutscene nicht
 * unter das Tor (ROOM1030)."  ->  "Na dann baue das Kriechen nach."
 *
 * ---- MESSUNG VOR DEM BAU (probe_re2_crawl_gate, gleicher Aktor, gleiche Vorbedingung) --------
 *   RE1.5: Sub-Modus 0x10 @Tick 1 -> Kriech-Commit @Tick 97 (grid 0x81, sca 8, Clip 0x1A)
 *   RE2  : s1 blieb 0x02, grid 0, sca 4, Clip 4                       = KRIECHT NICHT
 *
 * ---- WAS HIER GEPINNT WIRD (alles byte-gelesen aus EMOVL10_S0.BIN) ---------------------------
 *  P1 EINTRITT      Zustand-1-Wurzel @0x8010114C (`andi 0x3f` @0x8010115C -> Tabelle @0x8010C854,
 *                   14 Eintraege alternieren STRIKT auf Bit 0: ungerade = Kriecher 0x80101210).
 *                   Die Bruecke uebersetzt die RE1.5-Skriptanforderung (+0x1C4 & 0x1000, ROOM1030
 *                   sub07 @Datei 0x2754) in die byte-gelesene RE2-Eintrittskombination
 *                   `sh 1,270` @0x80107A54 + `sw 1,4` @0x80107A58 = Kriecher / Sub 0.
 *  P2 KRIECH-CLIP   EXEC[0] P0 @0x80103064: +0x14C = 0x000F0005 -> Clip 5, Rate 15;
 *                   +0x158 = (rand & 7) + 7 @0x8010308C-98; +0x14D = rand & 0xF @0x8010309C-BC.
 *  P3 BEWEGUNG      EXEC[0] P1 @0x801030C0: Steuer-Gate `+0x144 >= 21` (@0x801030C0-CC),
 *                   steer(+0x1C4/+0x1C6, 24) @0x801030D4-E0, ALT==0-Timer @0x801030E4-F4,
 *                   Bewegungs-PAAR e7c @0x8010312C + 152c8 @0x8010314C.
 *  P4 TOR-TRANSIT   SCA-Maske 8 statt 4 an der echten ROOM1030-Torzelle (RE1.5-Raumdaten;
 *                   +0x1D7 wird im RE2-Overlay NIE geschrieben — Voll-Scan `,471(` = 0 Treffer,
 *                   der Wert 8 stammt aus der RE1.5-Kette @0x801050F4).
 *  P5 AUSSTIEG      das ZWEITE Skript-Bit +0x1C4 & 0x2000 (RE1.5-Gate @0x8010369C-A4) -> Kriecher-
 *                   Bit aus, SCA zurueck auf 4 (@0x801050B4), Wort 0x901 = EXEC[9] @0x80103E48.
 *  P6 GRAB-ENTSCHEID DECIDE[0] @0x80102EE4 committet 0x101 nur bei dist < 0x514 (@0x80102F3C),
 *                   freiem Spieler-Riegel (@0x80102F4C) und gleicher Etage (@0x80102F60-70) —
 *                   und setzt dabei Spieler+0x1D3 |= 0x80 (@0x80102FB8).
 *  N1..N4 NEGATIV   Riegel belegt / andere Etage / zu weit / totes Ziel.
 *  R1 RE1.5-WACHE   die RE1.5-Kriechkette darf sich NICHT aendern (sie ist byte-true).
 *
 * ⚠ ORDNUNG: das Original advanct die Posen VOR der KI (game_step_common.c:1322 ruft
 *   re15_actors_anim_advance vor re15_enemy_ai_run_all, byte-true FUN_80019E20 @0x8001ce2c vor
 *   FUN_8001a50c @0x8001ce04). Der Test faehrt dieselbe Ordnung — ohne sie steht +0x14D fest
 *   und JEDE clip-getriebene Phase misst Muell.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re2_ems.h"
#include "re15_rdt.h"
#include "re15_room.h"
#include "re15_collision.h"
#include "re15_damage.h"    /* re15_damage_seed_rng — den globalen Draw fuer R1 zuruecksetzen */

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_actors_anim_advance(void);

static int s_fails = 0;
static void ck(int cond, const char *what, long got, long want)
{
    if (cond) { printf("  ok   %-58s (%ld)\n", what, got); return; }
    printf("  FAIL %-58s got %ld, want %ld\n", what, got, want);
    s_fails++;
}

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* ---- Baenke: RE2 (shared_assets/RE2) fuer den RE2-Flavor, RE1.5 fuer die Regressionswache --- */
static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static int load_bank_re2(uint8_t type)
{
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_re2_n);
    if (!s_re2_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_re2_ems, s_re2_n, (int)type, eb, NULL) != 0) { eb->type = 0; return 0; }
    eb->buf = NULL; eb->ok = 1;
    return 1;
}

static uint8_t *s_re15_blob = NULL;
static int load_bank_re15(uint8_t type)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0; int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0) {
        free(s_re15_blob);
        s_re15_blob = (uint8_t *)malloc(len);
        memcpy(s_re15_blob, ems + off, len);
        re15_enemy_bank_t *eb = re15_enemy_alloc(type);
        re15_tim_t tim = (re15_tim_t){0};
        if (eb && re15_emd_parse_container(s_re15_blob, len, &eb->md1, &eb->skel,
                                           &eb->anim, &tim) == 0) {
            eb->ok = 1; eb->buf = NULL; ok = 1;
            re15_emd_parse_own_bank(s_re15_blob, len, &eb->skel_own, &eb->anim_own);
            eb->own_ok = (eb->anim_own.clip_count > 0);
        }
    }
    free(ems);
    return ok;
}

/* Ein Aktor im ROOM1030-Ausgangszustand: der wartende Zombie vor dem Tor (§18 des Dossiers). */
static re15_actor_t *setup(re15_ai_flavor_t fl, uint8_t type)
{
    re15_actor_init();
    re15_ai_flavor_set(fl);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[1];
    pl->active = 1; pl->hp = 100; pl->hit_react = 1;   /* blockt jeden Grab-Write */
    pl->x = 0; pl->z = -18000; pl->floor = 0;
    pl->re2z_self1d3 = 0;
    e->active = 1; e->type = type; e->hp = 100;
    e->state = 1; e->sub_state_1 = 2; e->sub_state_2 = 0; e->sub_state_3 = 0;
    e->grid_id = 0; e->ai_flags = 0; e->anim_flags = 0; e->hit_react = 0;
    e->sca_mask = 4; e->floor = 0; e->x = 0; e->z = -24838; e->rot_y = 0;
    e->hit_radius_min = 0;
    e->steer_x = 0; e->steer_z = -18000;               /* Nav-Ziel = Richtung Spieler/Lobby */
    return e;
}

/* Den Spieler VOR den Kriecher setzen. Die Sektor-Konvention des Ports (re2z_sector,
 * enemy_ai_re2_zombie.c) ist `bearing = atan2_q12(dz,dx) - 0x400`, verglichen gegen den ROHEN
 * +0x76-Winkel — der Zwilling des Grab-Schwenks `rot_y = atan2_q12(dz,dx) - 0x400` @0x801026E4-F8.
 * Ein Aktor mit rot_y == 0 schaut also nach +X, nicht nach +Z. */
static void put_player_in_front(re15_actor_t *e, re15_actor_t *pl, int32_t dist)
{
    pl->x = e->x + dist; pl->z = e->z; pl->floor = e->floor;
    e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;
}

static void tick(int n)
{
    for (int i = 0; i < n; i++) {
        re15_actors_anim_advance();          /* game_step_common.c:1322 — VOR der KI */
        re15_enemy_ai_live_tick(1);
    }
}

int main(void)
{
    printf("== PIN: der RE2-Kriecher (ROOM1030-Kriechtor) ==\n\n");

    /* ---------------------------------------------------------------------------------------
     * P1  EINTRITT: RE1.5-Skriptanforderung -> RE2-Kriecher
     * ------------------------------------------------------------------------------------- */
    printf("[P1] Eintritt: +0x1C4 & 0x1000 (ROOM1030 sub07) -> Kriecher-Wurzel 0x80101210\n");
    re15_actor_init();
    int b2 = load_bank_re2(0x16);
    printf("     RE2-Bank EM016: %s\n", b2 ? "geladen" : "FEHLT (Messung wertlos!)");
    ck(b2, "RE2-Bank EM016 geladen (sonst clip_len==0)", b2, 1);
    {
        re15_enemy_bank_t *eb = re15_enemy_find(0x16);
        int c5  = (eb && eb->anim.clip_count > 5)    ? eb->anim.clips[5].frame_count    : -1;
        int c6  = (eb && eb->anim.clip_count > 6)    ? eb->anim.clips[6].frame_count    : -1;
        int c23 = (eb && eb->anim.clip_count > 0x17) ? eb->anim.clips[0x17].frame_count : -1;
        printf("     Clips Haupt=%d  (Kriechen 5: %d f, HURT 6: %d f, Warten 0x17: %d f)\n",
               eb ? eb->anim.clip_count : -1, c5, c6, c23);
        ck(c5 > 1, "Kriech-Clip 5 hat Frames (Bank-Gegenprobe)", c5, 2);
    }

    re15_actor_t *e  = setup(RE15_AI_FLAVOR_RE2, 0x16);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    e->anim_flags |= 0x1000u;                          /* sub07 @Datei 0x2754 */
    tick(1);
    ck(e->state == 1,        "state == 1 (sw 1,4 @0x80107A58)",              e->state, 1);
    ck(e->sub_state_1 == 0,  "+0x5 == 0 (Kriech-LOKOMOTION, EXEC[0])",       e->sub_state_1, 0);
    ck(e->motion == 5,       "+0x94 == 5 (Wort 0x000F0005 @0x80103064-6C)",  e->motion, 5);
    ck(e->anim_frac == 0x0F, "+0x8F == 0x0F (Rate 15 aus demselben Wort)",   e->anim_frac, 0x0F);
    ck(e->sca_mask == 8,     "+0x1D7 == 8 (Torzelle, RE1.5-Bruecke @0x801050F4)", e->sca_mask, 8);
    ck(e->re2z_t158 >= 7 && e->re2z_t158 <= 14,
       "+0x158 in [7,14] = (rand&7)+7 @0x8010308C-98",                       e->re2z_t158, 7);
    ck((int)e->anim_frame <= 15, "+0x14D <= 15 = rand&0xF @0x8010309C-BC",   e->anim_frame, 0);
    ck(pl->re2z_self1d3 == 0,
       "Skript-Kriechen setzt den Spieler-Riegel NICHT (er gehoert dem Angriff)",
       pl->re2z_self1d3, 0);

    /* ---------------------------------------------------------------------------------------
     * P2/P3  KRIECH-BEWEGUNG: das +0x144-Gate, der ALT==0-Timer, echter Vortrieb
     * ------------------------------------------------------------------------------------- */
    printf("\n[P2/P3] Kriech-Bewegung ueber 120 Ticks (Steuer-Gate +0x144 >= 21 @0x801030C8)\n");
    {
        int32_t z0 = e->z, x0 = e->x;
        int gate_open = 0, max144 = -32768, resteer = 0;
        int16_t prev158 = e->re2z_t158;
        for (int t = 0; t < 120; t++) {
            tick(1);
            if ((int)e->re2z_root144 > max144) max144 = (int)e->re2z_root144;
            if ((int)e->re2z_root144 >= 21) gate_open++;
            if (e->re2z_t158 > prev158) resteer++;      /* Neu-Seed nach ALT==0 */
            prev158 = e->re2z_t158;
        }
        int32_t dz = e->z - z0, dx = e->x - x0;
        printf("     dx=%ld dz=%ld  max(+0x144)=%d  Gate offen in %d/120 Ticks  Re-Steer %dx\n",
               (long)dx, (long)dz, max144, gate_open, resteer);
        ck(e->motion == 5,      "Kriech-Clip laeuft weiter (kein Zustandsleck)", e->motion, 5);
        ck(e->sub_state_1 == 0, "bleibt in EXEC[0] (kein Grab: Spieler zu weit)", e->sub_state_1, 0);
        ck(e->sca_mask == 8,    "SCA-Zeile bleibt 8 waehrend des Kriechens",      e->sca_mask, 8);
        ck((dx * dx + dz * dz) > 0, "der Kriecher bewegt sich ueberhaupt (e7c+152c8)",
           (long)(dx * dx + dz * dz), 1);
        ck(dz > 0, "die Bewegung geht Richtung Steuer-Ziel (z waechst Richtung Lobby)", (long)dz, 1);
        ck(gate_open > 0, "das Steuer-Gate +0x144 >= 21 oeffnet mindestens einmal", gate_open, 1);
        ck(resteer > 0,   "der +0x158-Timer laeuft ab und seedet neu (ALT==0-Zweig)", resteer, 1);
    }

    /* ---------------------------------------------------------------------------------------
     * P4  TOR-TRANSIT an der ECHTEN ROOM1030-Kollision
     * ------------------------------------------------------------------------------------- */
    printf("\n[P4] Tor-Transit: SCA-Maske 8 gegen 4 an der echten ROOM1030.RDT\n");
    {
        size_t n = 0;
        uint8_t *rdtbuf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1030.RDT", &n);
        ck(rdtbuf != NULL, "ROOM1030.RDT lesbar", rdtbuf ? 1 : 0, 1);
        if (rdtbuf) {
            static re15_rdt_t rdt;
            int parsed = (re15_rdt_parse(rdtbuf, n, &rdt) == 0);
            ck(parsed, "ROOM1030.RDT geparst", parsed, 1);
            if (parsed) {
                /* Torzelle aus dem Dossier: SCA rgn2/idx6 == rgn3/idx6, z[-24420..-22771].
                 * NICHT geraten, sondern GESUCHT: ein Raster von Nord-Schritten quer durch das
                 * Band; gezaehlt wird, an wie vielen Stellen die aufrechte Maske 4 geklemmt wird
                 * und die Kriecher-Maske 8 durchkommt. */
                /* ZUERST die Daten ansehen, nicht raten: welche Zelle ist fuer die aufrechte
                 * Maske 4 fest und fuer die Kriecher-Maske 8 durchlaessig? Genau das ist
                 * `(u0 & 4) && !(u0 & 8)` — der Dossier-Wert 0xF7 hat Bit 3 als EINZIGES frei. */
                {   int gate_cells = 0, base = 0, all_ff = 1;
                    for (int q = 0; q < 5; q++) {
                        for (int i = base; i < base + rdt.sca_rgn[q] && i < rdt.sca_count; i++) {
                            const re15_sca_entry_t *c = &rdt.sca[i];
                            if (getenv("RE15_SCA_DUMP"))
                                printf("     [%2d] rgn=%d idx=%d u0=0x%02X typ=%u band=%d "
                                       "rect=(%d,%d,%dx%d)\n", i, q, i - base, (unsigned)c->u0,
                                       (unsigned)c->type, (int)(c->floor >> 4),
                                       (int)c->x, (int)c->z, (int)c->width, (int)c->density);
                            if (c->u0 != 0xFFu) all_ff = 0;
                            if ((q == 2 || q == 3) && (i - base) == 6) {
                                gate_cells++;
                                printf("     Torzelle rgn=%d idx=%d u0=0x%02X typ=%u band=%d "
                                       "rect=(%d,%d,%dx%d)\n", q, i - base, (unsigned)c->u0,
                                       (unsigned)c->type, (int)(c->floor >> 4),
                                       (int)c->x, (int)c->z, (int)c->width, (int)c->density);
                            }
                        }
                        base += rdt.sca_rgn[q];
                    }
                    printf("     SCA-Zellen gesamt=%d, alle statisch 0xFF: %s\n",
                           rdt.sca_count, all_ff ? "ja" : "nein");
                    ck(gate_cells == 2, "rgn2/idx6 und rgn3/idx6 existieren (Dossier-Torzellen)",
                       gate_cells, 2);
                    ck(all_ff, "statisch ist JEDE Zelle 0xFF — die 0xF7 kommt vom Skript",
                       all_ff, 1);
                }
                /* ⛔ DIE TORZELLE IST NICHT STATISCH — sie wird VOM SKRIPT geoeffnet.
                 * Statisch stehen alle 35 Zellen auf u0 = 0xFF (oben gedumpt). Das Skript setzt
                 * genau die beiden Torzellen auf 0xF7 — Sca_id_set (0x37, Port op_sca_id_set):
                 *   `37 02 06 f7`  @Datei 0x2000 und @Datei 0x278E
                 *   `37 03 06 f7`  @Datei 0x2004 und @Datei 0x2792
                 * (eigener Byte-Scan von ROOM1030.RDT). 0x278E/0x2792 liegen unmittelbar an
                 * sub07 @Datei 0x2754 — dieselbe Routine, die +0x1C4 |= 0x1000 setzt: das Skript
                 * OEFFNET das Tor fuer Kriecher UND befiehlt das Kriechen im selben Atemzug.
                 * 0xF7 = 0b1111_0111 -> Bit 3 (Wert 8) ist das EINZIGE freie Bit:
                 *   Maske 4 (aufrecht): 0xF7 & 4 = 4 != 0  -> Zelle bleibt fest
                 *   Maske 8 (Kriecher): 0xF7 & 8 = 0       -> Zelle ist durchlaessig
                 * Der Test spielt genau diese vier Stores nach und misst dann. */
                {   int base2 = 0;
                    for (int q = 0; q < 5; q++) {
                        if (q == 2 || q == 3) {
                            int fi = base2 + 6;
                            if (fi < rdt.sca_count)
                                ((re15_sca_entry_t *)rdt.sca)[fi].u0 = 0xF7;  /* Sca_id_set */
                        }
                        base2 += rdt.sca_rgn[q];
                    }
                }
                int only4 = 0, both_pass = 0, both_block = 0, only8 = 0;
                int32_t hit_x = 0, hit_z0 = 0;
                for (int32_t sx = -27000; sx <= 3000; sx += 250) {
                    for (int32_t sz0 = -25200; sz0 <= -23000; sz0 += 200) {
                        int32_t sz1 = sz0 + 900;
                        int32_t nx = sx, nz = sz1;
                        re15_collision_constrain_enemy(&rdt, sx, sz0, &nx, &nz, 0, 0, 4u);
                        int p4 = (nz == sz1);
                        nx = sx; nz = sz1;
                        re15_collision_constrain_enemy(&rdt, sx, sz0, &nx, &nz, 0, 0, 8u);
                        int p8 = (nz == sz1);
                        if (!p4 && p8) { if (!only4) { hit_x = sx; hit_z0 = sz0; } only4++; }
                        else if (p4 && p8)   both_pass++;
                        else if (!p4 && !p8) both_block++;
                        else                 only8++;
                    }
                }
                printf("     Raster: nur-Kriecher=%d  beide-frei=%d  beide-blockiert=%d  "
                       "nur-aufrecht=%d\n", only4, both_pass, both_block, only8);
                if (only4) printf("     erste Kriecher-Stelle: x=%ld  z %ld -> %ld\n",
                                  (long)hit_x, (long)hit_z0, (long)(hit_z0 + 900));
                ck(only4 > 0,
                   "es gibt Stellen, die NUR die Maske 8 passiert (die Torzelle)", only4, 1);
                ck(only8 == 0, "keine Stelle bevorzugt die aufrechte Maske", only8, 0);
            }
            free(rdtbuf);
        }
    }

    /* ---------------------------------------------------------------------------------------
     * P5  AUSSTIEG: das zweite Skript-Bit
     * ------------------------------------------------------------------------------------- */
    printf("\n[P5] Ausstieg: +0x1C4 & 0x2000 (RE1.5-Gate @0x8010369C-A4) -> aufrecht\n");
    e->anim_flags = (uint16_t)((e->anim_flags & ~0x1000u) | 0x2000u);
    tick(1);
    ck((e->re2z_f10e & 1u) == 0u, "+0x10E Bit 0 geloescht (kein Kriecher mehr)",
       (long)(e->re2z_f10e & 1u), 0);
    ck(e->sca_mask == 4,      "+0x1D7 zurueck auf 4 (@0x801050B4)", e->sca_mask, 4);
    ck(e->sub_state_1 == 9,   "+0x5 == 9 = EXEC[9] Aufstehen @0x80103E48", e->sub_state_1, 9);

    /* ---------------------------------------------------------------------------------------
     * P6 + N1..N3  DECIDE[0]: der Angriffs-Entscheider des Kriechers
     * ------------------------------------------------------------------------------------- */
    printf("\n[P6] DECIDE[0] @0x80102EE4: Grab-Commit + Spieler-Riegel\n");
    {
        /* positiv: nah, freier Riegel, gleiche Etage, Spieler direkt vor dem Kriecher */
        e = setup(RE15_AI_FLAVOR_RE2, 0x16);
        pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        put_player_in_front(e, pl, 1000);              /* dist 1000 < 0x514 (1300) */
        e->anim_flags |= 0x1000u;
        /* EIN Tick genuegt: die Kriecher-Wurzel faehrt DECIDE und EXEC im SELBEN Tick
         * (@0x8010122C jalr DECIDE, @0x80101270 jalr EXEC, +0x5 dazwischen FRISCH gelesen
         * @0x80101254) — der Eintritt landet also sofort im Entscheider. */
        tick(1);
        ck((e->re2z_f10e & 1u) != 0u, "Vorbedingung: Kriecher-Bit gesetzt",
           (long)(e->re2z_f10e & 1u), 1);
        printf("     rot_y=%d floor(e/pl)=%u/%u +0x21A=0x%04x dist=%lu\n",
               (int)e->rot_y, e->floor, pl->floor, e->re2z_flags21a,
               (unsigned long)e->ai_dist);
        int grabbed = (e->sub_state_1 == 1);
        printf("     dist=%lu  -> +0x5 = %u,  Spieler+0x1D3 = 0x%02x\n",
               (unsigned long)e->ai_dist, e->sub_state_1, pl->re2z_self1d3);
        ck(grabbed, "nah + Riegel frei + gleiche Etage -> +0x4 = 0x101 (@0x80102FA8)", grabbed, 1);
        ck((pl->re2z_self1d3 & 0x80u) != 0u,
           "Spieler+0x1D3 |= 0x80 = der Ein-Angreifer-Riegel (@0x80102FB8)",
           (long)pl->re2z_self1d3, 0x80);

        /* N1: Riegel belegt */
        e = setup(RE15_AI_FLAVOR_RE2, 0x16);
        pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        put_player_in_front(e, pl, 1000);
        pl->re2z_self1d3 = 0x80;                        /* ein anderer haelt den Riegel */
        e->anim_flags |= 0x1000u;
        tick(2);
        ck(e->sub_state_1 == 0, "N1 Riegel belegt -> KEIN Grab (@0x80102F4C)", e->sub_state_1, 0);

        /* N2: andere Etage */
        e = setup(RE15_AI_FLAVOR_RE2, 0x16);
        pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        put_player_in_front(e, pl, 1000); pl->floor = 1;
        e->anim_flags |= 0x1000u;
        tick(2);
        ck(e->sub_state_1 == 0, "N2 andere Etage -> KEIN Grab (@0x80102F60-70)", e->sub_state_1, 0);

        /* N3: zu weit */
        e = setup(RE15_AI_FLAVOR_RE2, 0x16);
        pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        put_player_in_front(e, pl, 4000);              /* dist 4000 >= 0x514 */
        e->anim_flags |= 0x1000u;
        tick(2);
        ck(e->sub_state_1 == 0, "N3 dist >= 0x514 -> KEIN Grab (@0x80102F3C)", e->sub_state_1, 0);

        /* N4: totes Ziel legt sich nicht auf Skriptbefehl hin */
        e = setup(RE15_AI_FLAVOR_RE2, 0x16);
        e->hp = -1;
        e->anim_flags |= 0x1000u;
        tick(1);
        ck((e->re2z_f10e & 1u) == 0u, "N4 hp < 0 -> kein Kriech-Eintritt",
           (long)(e->re2z_f10e & 1u), 0);
    }

    /* ---------------------------------------------------------------------------------------
     * R1  REGRESSIONSWACHE: die RE1.5-Kriechkette bleibt unveraendert
     * ------------------------------------------------------------------------------------- */
    printf("\n[R1] Regressionswache: die byte-true RE1.5-Kriechkette darf sich NICHT aendern\n");
    {
        re15_actor_init();
        /* ⛔ OHNE DAS HIER MISST R1 DIE FALSCHE BANK: re15_enemy_alloc nimmt einen NEUEN Slot,
         * re15_enemy_find liefert aber den ERSTEN Treffer — die oben geladene RE2-Bank haette
         * weiter gegolten (gemessen: Commit @Tick 66 statt 97, weil anim_loco/loco_ok aus RE2
         * stehenblieben). Das ist genau die Falle "Messen ohne die richtige Bank". */
        re15_enemy_reset();
        int b15 = load_bank_re15(0x16);
        ck(b15, "RE1.5-Bank EM016 geladen", b15, 1);
        /* Die RE1.5-Kette zieht ihren Toggle-Startframe aus dem GLOBALEN Draw
         * (`rand & 3` @0x80104FFC). Ohne Re-Seed haengt der Commit-Tick davon ab, wie viele
         * Wuerfe die RE2-Messungen darueber verbraucht haben — deshalb hier der Ausgangs-Seed
         * aus re15_damage.c, damit die Wache denselben Strom sieht wie probe_re2_crawl_gate. */
        re15_damage_seed_rng(0x2545f491u);
        e = setup(RE15_AI_FLAVOR_RE15, 0x16);
        e->steer_x = 0; e->steer_z = 0;                 /* wie probe_re2_crawl_gate: unberuehrt */
        e->anim_flags |= 0x1000u;
        int sub10 = -1, commit = -1;
        for (int t = 1; t <= 260; t++) {
            re15_enemy_ai_live_tick(1);                 /* RE1.5-Kette advanct SELBST (kein
                                                         * globaler Advance — genau wie im Original,
                                                         * player_common.c-Skip) */
            if (sub10 < 0 && e->sub_state_1 == 0x10) sub10 = t;
            if (commit < 0 && e->grid_id == 0x81)    commit = t;
        }
        printf("     Sub 0x10 @Tick %d,  Commit @Tick %d,  motion=0x%02x sca=%u\n",
               sub10, commit, e->motion, e->sca_mask);
        ck(sub10 == 1,       "RE1.5: Sub-Modus 0x10 weiterhin ab Tick 1", sub10, 1);
        ck(commit == 97,     "RE1.5: Kriech-Commit weiterhin @Tick 97",   commit, 97);
        ck(e->grid_id == 0x81, "RE1.5: +0x09 == 0x81",   e->grid_id, 0x81);
        ck(e->sca_mask == 8,   "RE1.5: +0x1D7 == 8",     e->sca_mask, 8);
        ck(e->motion == 0x1A,  "RE1.5: Clip 0x1A",       e->motion, 0x1A);
    }

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    printf("\n== %s (%d Fehler) ==\n", s_fails ? "FEHLGESCHLAGEN" : "OK", s_fails);
    return s_fails ? 1 : 0;
}
