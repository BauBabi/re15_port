/**
 * @file test_re2_zombie_teardeath.c
 * @brief PIN — die ZERREISS-TODE des RE2-Zombies (die vier DEATH-Zellen jenseits von
 *        FUN_80108530) + die Regressionswache fuer alles, was schon byte-true war.
 *
 * ⛔ AUSGANGSLAGE: Commit 397b9a60 baute die DEATH-Wurzel FUN_80108250 neu (vier Zweige,
 * 2D-Dispatch `tbl[0x8010CC24][+0x5 *36][+0x1D2 *4]`, `jalr v0` @0x80108514) und liess die
 * fuenf schweren Zellen bewusst offen — sie fielen auf die alte Clip-7-Kette zurueck. Diese
 * Datei pinnt die Nachreichung.
 *
 * DIE FUENF ROUTINEN, selbst disassembliert (EMOVL10_S0.BIN, Modul @0x80100000):
 *   0x80107438  = der HURT-Knockdown-Handler; sein `lbu v1,4 / bne v0(=3)`-Zweig @0x8010777C-84
 *                 ist nur aus der DEATH-Wurzel erreichbar: `rand & 3` != 0 -> HP = 10
 *                 (@0x8010778C-B0), sonst entscheidet +0x16B (Arm ab @0x80107524) zwischen
 *                 CORPSE (`sw 7,4` @0x801077E4) und HP = 10.
 *   0x801066FC  = der HURT-Ragdoll-Handler; DEATH-Zweig P0 @0x801067F4-834 (3/4 -> +0x231 = 1 ->
 *                 FUN_80109610) und P2 @0x80106ACC-E8 (HP = 10 -> Kriecher).
 *   0x801092C4  = MAGNUM: der KOPF platzt ab. Phasentabelle @0x8010014C (5 Phasen). P0 setzt
 *                 Part 8 `flags |= 0x4A` (@0x80109460-98), +0x9C = 400 / +0x9A = -100 /
 *                 +0x9E = 10 / +0xA4 = -50 / +0x98 = Peilung+2048 / +0x70 = 0x00101040
 *                 (@0x80109468-8C) und den Stumpf-Marker auf Part 0 (@0x801094B8-C8).
 *                 50% (@0x801094D8-F0) laeuft der KOPFLOSE noch (rand&0x3F)+30 Frames weiter.
 *   0x80108BEC  = ZERREISSEN: Kopf ab + Bein(e) ab + RUMPF-STUMPF. Phasentabelle @0x8010012C
 *                 (7 Phasen). P0: Kopf wie oben mit Tinte 0x00101030 (@0x80108DD4-E3C),
 *                 R-Bein Part 9 `flags |= 0x1062` (@0x80108E54-60), L-Bein Part 12 mit 50%
 *                 (@0x80108EE8-F70), Stumpf-Marker auf 10/11/13/0 und
 *                 `Part-16-Geometrie -> Part 0` (@0x80108FB8-FDC).
 *   0x80109610  = WEGSCHLEUDERN: Clip 1/2 ab FRAME 15 (Wort 0x000F0F00 + clipsel[dir]
 *                 @0x80109840-64), +0x144 = 400 (@0x80109888) mit -50 je Frame (@0x8010996C-88)
 *                 und Schub um +0x16A<<11 gedreht (@0x8010995C/64); P2 -> +0x4 = 7 (CORPSE,
 *                 v0 aus dem DELAY-SLOT @0x80109684, `sw v0,4` @0x801099B0).
 *
 * GEMESSEN (probe_re2_ztear, ROOM1140, echter Weg re15_game_step + Pad R1/SQUARE, GELADENE
 * RE2-Bank EM010, 17 Meshes):
 *   Waffe 18 Rakete (Zeile 17) f132 TOD -> Zelle 8BEC
 *     parts [0]=0081(mesh16) [8]=004B [9]=1063 [10]=0081 [11]=0081 [12]=1063 [13]=0081
 *     f132..175 P1 (Clip 4, +0x15A 9->0) | f176 P2 -> P5 | f177 P6 +0x158=19 | f197 P3 Clip 1
 *     f256 P4 | f257 st=7 | f258 Leiche Clip 23. Kopf flog (-2014,-2618,-20126)->(-1000,-878,-20389)
 *   Waffe 7 Magnum (Zeile 5) f0 TOD -> Zelle 92C4
 *     parts [0]=0081 [8]=004B ; s2 0->3 (die 50%-Weiche), +0x158 = 53, f55 -> P1 Clip 1,
 *     f114 P2, f115 st=7, f116 Leiche Clip 23. Kopf flog (-1612,-2826,-19725)->(-562,-1086,-19725)
 *   Waffe 8 Schrot (Zeile 7) f0 TOD -> Zelle 66FC
 *     +0x231 = 1 -> BURST: Clip 1 ab af=15, spd 400/350/300/.../0 (genau -50), f44 P2, Leiche 23
 *     mit verschobener RNG-Folge (Zusatz-Warmlauf 2): +0x231 = 0 -> normaler Ragdoll, Endstand
 *     st=1 hp=10 (die Wiederbelebung als KRIECHER)
 *   Waffe 13 SPAS (Zeile 8): Warmlauf 0/1 -> BURST, ab 2 -> voller Zerreiss-Satz wie die Rakete
 *   Waffe 3 Pistole (Zeile 3): Zelle MAIN, KEIN Part-Flag, mesh[0] = 0  (Regressionswache)
 *
 * ⚠ EINE BELEGTE FERNWIRKUNG (2026-08-21, einvariabler A/B-Lauf): der Zensus
 * `unit_re2_weapon_rows` druckt fuer w=14/20/21 andere Hurt-Clip-Indizes als vorher. Ursache ist
 * NICHT der Trefferpfad, sondern der gemeinsame Wurf-Strom: dort toetet genau EINE Waffe (w=7
 * Magnum), ihr Todes-Tick zog frueher EINEN Wurf (Stand-in re2z_death_crawler, @0x80108AD8-F4)
 * und zieht jetzt ZWEI bis DREI (FUN_801092C4 @0x80109330 / @0x801094D8 / @0x80109554). Dreht
 * man AUSSCHLIESSLICH die Zelle 0x801092C4 zurueck, ist die Zensus-Ausgabe wieder zeichengleich
 * die alte; alle MAIN-Waffen VOR w=7 und der ganze RE1.5-Block sind ohnehin unveraendert. Die
 * volle Herleitung steht im Kopf von test_re2_weapon_rows.c. Auf der Quellseite: beide Eingriffe
 * in die HURT-Handler stehen hinter `if (death)`, und der neue `re2z_rand()` ist der LINKE
 * Operand von `&&` (wird also unbedingt gezogen) — die Wurfzahl des HURT-Pfades ist unveraendert.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_emd.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_actor_clip_len(const re15_actor_t *a);
extern int  re15_re2z_last_death_handler(void);
extern int  re15_re2z_death_cell(unsigned row, unsigned col);
extern int  re15_inv_find_item(uint8_t id);
extern int  re15_inv_equipped_slot(void);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static void bringup(void)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1140;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
}

static uint8_t *s_ems = NULL; static long s_ems_sz = 0;
static int load_re2_bank(uint8_t type)
{
    if (!s_ems) { size_t n = 0;
        s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n); s_ems_sz = (long)n; }
    if (!s_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_ems, (size_t)s_ems_sz, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return 1;
    }
    eb->type = 0; return 0;
}

static int standing_zombie(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type <= 0x18
            && !(g_actors[s].grid_id & 0x80)) return s;
    return -1;
}

/* ---- EIN Todesverlauf ---------------------------------------------------------------------- */
typedef struct {
    int ok;                       /* Zombie ist gestorben (state 3 gesehen)                     */
    int cell;                     /* re15_re2z_last_death_handler() im ersten DEATH-Frame        */
    int row, col;                 /* +0x5 / +0x1D2 beim Tod                                     */
    int first_clip, first_af, first_frac, first_sub2;
    int first_speed;
    uint16_t pflags[16]; uint8_t pmesh[16]; uint32_t ptint[16];
    int16_t  w9c8, w9a8, w9e8, wa48, yaw988; uint16_t life8;
    int16_t  yaw980, w9a0, w9c0, w9e0;
    int      rag231;
    int      speed_seq[12]; int speed_n;   /* +0x144 der ersten Frames (Burst-Abbau -50)        */
    int      corpse_clip, corpse_seen, corpse_flags21a;
    int      end_state, end_hp, end_f10e;
    int      arms_torn;                    /* Part 3 traegt 0x4A (nur 0x80107438)               */
    int      frames, hits, hp0;
} kill_t;

/* Der PRISTINE-Schnappschuss: EIN aufrechter, unbeschaedigter Zombie, gezogen NACH dem
 * Hochlauf und dem Warten auf "aufrecht". Jeder Lauf stellt ihn wortgleich wieder her.
 * WARUM ein voller Struct-Schnappschuss und kein Feld-fuer-Feld-Reset: der Tod nullt u.a. die
 * Trefferbox (die DEATH-/CORPSE-Kette schreibt +0x90..+0x9E) — ohne sie findet die Ziel-Klammer
 * FUN_8003703C (re15_player_aim_target: `hit_radius_min <= 0` -> ueberspringen) den Zombie nie
 * wieder, und der zweite Lauf feuerte ins Leere (gemessen: 0 Treffer in 700 Frames).
 * WARUM AUFRECHT: die DEATH-Wurzel prueft VOR dem 2D-Dispatch +0x10E Bit 0 (@0x801083B0),
 * +0x21A Bit 0x10 (@0x801083EC) und +0x21A Bit 0x2 (@0x80108408); der ROOM1140-Spawn startet
 * FRESSEND (Bit 0x10 @0x80103D00), da gewinnt Zweig 2 und die Zerreiss-Zelle laeuft nie. */
static re15_actor_t s_pristine;
static int          s_pristine_ok = 0;

static void reset_zombie(int slot)
{
    if (s_pristine_ok) g_actors[slot] = s_pristine;
}

static void run_kill(int slot, int weapon, int extra_warm, int force_col0, int budget, kill_t *t)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    memset(t, 0, sizeof *t);

    if (re15_inv_find_item((uint8_t)weapon) < 0) (void)re15_inv_grant((uint8_t)weapon, 99);
    re15_player_set_equipped_weapon(weapon);
    {   int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 200; }

    reset_zombie(slot);
    pl->x = e->x - 2600; pl->z = e->z; pl->y = e->y; pl->rot_y = 1024; pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); }
    for (int w = 0; w < extra_warm; w++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); }

    int dead = 0, hp_last = e->hp; t->hp0 = e->hp;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        uint16_t cur = RE15_PAD_BIT_R1, edge = 0;
        if (!dead) { cur |= RE15_PAD_BIT_SQUARE; edge = RE15_PAD_BIT_SQUARE; }
        if (force_col0 && e->state == 3) e->re2z_hits1d2 = 0;
        frame(cur, edge);
        t->frames = f;
        if (e->hp < hp_last) t->hits++;
        hp_last = e->hp;

        if (!dead && e->state == 3) {
            dead = 1; t->ok = 1;
            t->row = e->sub_state_1; t->col = e->re2z_hits1d2;
            t->cell = re15_re2z_last_death_handler();
            t->first_clip = (int)e->motion; t->first_af = (int)e->anim_frame;
            t->first_frac = (int)e->anim_frac; t->first_sub2 = e->sub_state_2;
            t->first_speed = (int)e->speed_h;
            t->rag231 = e->re2z_rag231;
            memcpy(t->pflags, e->re2z_part_flags, sizeof t->pflags);
            memcpy(t->pmesh,  e->re2z_part_mesh,  sizeof t->pmesh);
            memcpy(t->ptint,  e->re2z_part_tint,  sizeof t->ptint);
            t->w9c8 = e->re2z_part_w9c[8]; t->w9a8 = e->re2z_part_w9a[8];
            t->w9e8 = e->re2z_part_w9e[8]; t->wa48 = e->re2z_part_wa4[8];
            t->yaw988 = e->re2z_part_yaw98[8]; t->life8 = e->re2z_part_life[8];
            t->yaw980 = e->re2z_part_yaw98[0]; t->w9a0 = e->re2z_part_w9a[0];
            t->w9c0   = e->re2z_part_w9c[0];   t->w9e0 = e->re2z_part_w9e[0];
            if (e->re2z_part_flags[3] & 0x4Au) t->arms_torn = 1;
        }
        if (dead && t->speed_n < 12) t->speed_seq[t->speed_n++] = (int)e->speed_h;
        if (dead && e->state == 7 && e->sub_state_1 == 1 && !t->corpse_seen) {
            t->corpse_seen = 1; t->corpse_clip = (int)e->motion;
            t->corpse_flags21a = (int)e->re2z_flags21a;
        }
        if (dead && e->state != 3 && e->state != 7 && e->hp > 0) break;   /* wiederbelebt */
        if (t->corpse_seen) break;
    }
    t->end_state = e->state; t->end_hp = e->hp; t->end_f10e = (int)e->re2z_f10e;
}

/* Der ROOM1140-Zombie ist ein FRESSER: er kehrt im Sekundentakt in die Fress-Schleife zurueck
 * und setzt dort +0x21A Bit 0x10 (@0x80103D00). Faellt der toedliche Treffer genau dann, gewinnt
 * Zweig 2 der Wurzel (@0x801083EC) und die Zerreiss-Zelle laeuft gar nicht. Statt eine
 * RNG-Verschiebung zu RATEN, die "zufaellig passt", wird die Verschiebung GESUCHT: jeder
 * Zusatz-Warmlauf verschiebt die deterministische Wurf-Folge (re2z_rand, EIN Zustandswort), und
 * der erste Lauf, der die gesuchte Zelle dispatcht, wird gepinnt. Findet keiner sie, ist das
 * ein echter Regressions-Befund. */
static int find_kill(int slot, int weapon, int want_cell, int want_231, int budget, kill_t *t)
{
    for (int w = 0; w < 16; w++) {
        run_kill(slot, weapon, w, 0, budget, t);
        if (t->ok && t->cell == want_cell && (want_231 < 0 || t->rag231 == want_231)) return 1;
    }
    return 0;
}

/* POSITIV-KONTROLLE fuer die Spalte 0 (Zelle 0x80107438): der Port-Applier stempelt IMMER die
 * Zone 1 (re15_re2_stamp_hit), Spalte 0 ist im Spiel also unerreichbar — dieselbe Lage wie
 * Handler 5 der HURT-Tabelle. Der Todes-Zustand wird deshalb direkt gestellt (+0x4 = 3, Zeile,
 * Spalte) und danach laeuft die ECHTE Wurzel re2z_death ueber den normalen Tick. Ohne diese
 * Kontrolle waere "unerreichbar" eine Behauptung ohne Beleg. */
static void run_forced_death(int slot, unsigned row, unsigned col, int budget, kill_t *t)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    memset(t, 0, sizeof *t);
    reset_zombie(slot);
    e->hp = -1; e->state = 3; e->sub_state_1 = (uint8_t)row; e->sub_state_2 = 0;
    e->sub_state_3 = 0; e->re2z_hits1d2 = (uint8_t)col;
    t->hp0 = e->hp;

    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        if (e->state == 3) e->re2z_hits1d2 = (uint8_t)col;   /* die Spalte festhalten */
        frame(RE15_PAD_BIT_R1, 0);
        t->frames = f;
        if (f == 0) {
            t->ok = 1; t->row = e->sub_state_1; t->col = e->re2z_hits1d2;
            t->cell = re15_re2z_last_death_handler();
            t->first_clip = (int)e->motion; t->first_af = (int)e->anim_frame;
            t->first_frac = (int)e->anim_frac; t->first_sub2 = e->sub_state_2;
            t->first_speed = (int)e->speed_h; t->rag231 = e->re2z_rag231;
            memcpy(t->pflags, e->re2z_part_flags, sizeof t->pflags);
            memcpy(t->pmesh,  e->re2z_part_mesh,  sizeof t->pmesh);
            memcpy(t->ptint,  e->re2z_part_tint,  sizeof t->ptint);
            if (e->re2z_part_flags[3] & 0x4Au) t->arms_torn = 1;
        }
        if (e->state == 7 && e->sub_state_1 == 1 && !t->corpse_seen) {
            t->corpse_seen = 1; t->corpse_clip = (int)e->motion;
            t->corpse_flags21a = (int)e->re2z_flags21a; break;
        }
        if (e->state != 3 && e->state != 7 && e->hp > 0) break;    /* wiederbelebt */
    }
    t->end_state = e->state; t->end_hp = e->hp; t->end_f10e = (int)e->re2z_f10e;
}

static void dump(const char *tag, const kill_t *t)
{
    printf("  %-22s cell=%d row=%d col=%d clip=%d af=%d frac=%d s2=%d spd=%d 231=%d "
           "end(st=%d hp=%d 10E=%04X) corpse_clip=%d hp0=%d hits=%d f=%d\n",
           tag, t->cell, t->row, t->col, t->first_clip, t->first_af, t->first_frac,
           t->first_sub2, t->first_speed, t->rag231, t->end_state, t->end_hp,
           (unsigned)t->end_f10e, t->corpse_clip, t->hp0, t->hits, t->frames);
    printf("      parts:");
    for (int p = 0; p < 16; p++)
        if (t->pflags[p] != 1u || t->pmesh[p] != (uint8_t)p)
            printf(" [%d]=%04X%s", p, t->pflags[p],
                   (t->pmesh[p] != (uint8_t)p) ? "*" : "");
    printf("  mesh0=%u\n", t->pmesh[0]);
}

int main(void)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("SKIP: %s nicht lesbar\n", path); return 0; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    /* ===== PIN 0 — die DEATH-Tabelle @0x8010CC24, Zelle fuer Zelle gegen den eigenen Dump ===
     * (`re2_disasm.py table 0x8010cc24 170 --bin EMOVL10_S0.BIN`). */
    struct { unsigned r, c; int want; const char *why; } cells[] = {
        { 3, 1, 1, "Browning HP  -> 0x80108530 MAIN   @0x8010CC94" },
        { 5, 1, 3, "Magnum       -> 0x801092C4        @0x8010CCDC" },
        { 6, 1, 3, "Custom Magnum-> 0x801092C4        @0x8010CCE8" },
        { 7, 0, 2, "Schrot Zone0 -> 0x80107438        @0x8010CD20" },
        { 7, 1, 4, "Schrot       -> 0x801066FC        @0x8010CD24" },
        { 8, 1, 5, "Custom Schrot-> 0x80108BEC        @0x8010CD48" },
        { 8, 4, 6, "Custom Schrot-> 0x80109610        @0x8010CD54" },
        { 9, 1, 5, "GL Explosiv  -> 0x80108BEC        @0x8010CD6C" },
        { 9, 2, 5, "GL Explosiv  -> 0x80108BEC        @0x8010CD70" },
        {12, 1, 6, "Bowgun       -> 0x80109610        @0x8010CDD8" },
        {17, 1, 5, "Rakete       -> 0x80108BEC        @0x8010CE8C" },
        {17, 4, 6, "Rakete       -> 0x80109610        @0x8010CE98" },
    };
    for (unsigned i = 0; i < sizeof cells / sizeof cells[0]; i++)
        CHECK(re15_re2z_death_cell(cells[i].r, cells[i].c) == cells[i].want,
              "DEATH-Zelle [%u][%u] = %d, erwartet %d (%s)",
              cells[i].r, cells[i].c, re15_re2z_death_cell(cells[i].r, cells[i].c),
              cells[i].want, cells[i].why);

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    bringup();
    re15_inv_load_briefing();
    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
    if (!load_re2_bank(0x10)) { printf("SKIP: RE2-Bank EM010 fehlt\n"); free(buf); return 0; }
    int slot = standing_zombie();
    if (slot < 0) { printf("SKIP: kein stehender Zombie in ROOM1140\n"); free(buf); return 0; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    {   re15_actor_t probe = g_actors[slot]; probe.motion = 1;
        if (re15_actor_clip_len(&probe) <= 0) { printf("SKIP: keine Clip-Laengen\n");
            free(buf); return 0; } }
    {   re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        re15_actor_t *e  = &g_actors[slot];
        int w = 0;
        while (w < 400 && ((e->re2z_flags21a & 0x12u) != 0u || (e->re2z_f10e & 1u) != 0u)) {
            pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); w++;
        }
        s_pristine = *e; s_pristine_ok = 1;
        printf("  PRISTINE nach %d Frames: st=%d s1=%u hp=%d 21A=%04X 10E=%04X box=%d\n",
               w, e->state, e->sub_state_1, e->hp, e->re2z_flags21a, e->re2z_f10e,
               (int)e->hit_radius_min);
        CHECK((e->re2z_flags21a & 0x12u) == 0u && (e->re2z_f10e & 1u) == 0u,
              "PRISTINE: Zombie nicht aufrecht (21A=%04X 10E=%04X)",
              e->re2z_flags21a, e->re2z_f10e);
    }

    kill_t t;

    /* ===== PIN 1 — 0x801092C4 MAGNUM (Waffe 7 -> Zeile 5): DER KOPF PLATZT AB ============== */
    CHECK(find_kill(slot, 7, 3, -1, 900, &t),
          "PIN1: in 16 RNG-Verschiebungen kein Treffer der Zelle 3 (0x801092C4)");
    dump("PIN1 Magnum", &t);
    CHECK(t.ok && t.cell == 3, "PIN1: Zelle %d statt 3 (0x801092C4)", t.cell);
    CHECK(t.row == 5, "PIN1: Zeile %d statt 5", t.row);
    CHECK((t.pflags[8] & 0x4Au) == 0x4Au,
          "PIN1: Kopf-Flagwort %04X ohne 0x4A (`ori v0,v0,0x4a` @0x801094... /sw 1376 @0x80109490)",
          t.pflags[8]);
    CHECK(t.ptint[8] == 0x00101040u,
          "PIN1: Kopf-Tinte %08X statt 0x00101040 (`sw t0,1488` @0x8010948C)", t.ptint[8]);
    CHECK(t.w9c8 == 400 && t.w9a8 == -100 && t.w9e8 == 10 && t.wa48 == -50 && t.life8 == 0,
          "PIN1: Kopf-Driftfelder +0x9C/%d +0x9A/%d +0x9E/%d +0xA4/%d +0xA0/%u statt "
          "400/-100/10/-50/0 (@0x80109468-88)", t.w9c8, t.w9a8, t.w9e8, t.wa48, t.life8);
    CHECK((t.pflags[0] & 0x80u) != 0 && t.yaw980 == 64 && t.w9a0 == 0
          && t.w9c0 == 0x1010 && t.w9e0 == 0x0010,
          "PIN1: Part-0-Stumpfmarker flags=%04X +0x98=%d +0x9A=%d +0x9C=%d +0x9E=%d statt "
          "0x80/64/0/0x1010/0x10 (zwei WORT-Stores @0x801094B8/BC)",
          t.pflags[0], t.yaw980, t.w9a0, t.w9c0, t.w9e0);
    CHECK(t.pmesh[0] == 0u,
          "PIN1: Magnum stempelt KEINEN Rumpf-Stumpf, mesh0 ist aber %u", t.pmesh[0]);
    CHECK((t.pflags[9] & 0x1062u) == 0 && (t.pflags[12] & 0x1062u) == 0,
          "PIN1: Magnum reisst KEIN Bein ab (9=%04X 12=%04X)", t.pflags[9], t.pflags[12]);
    CHECK((t.first_clip == 1 || t.first_clip == 2) && t.first_frac == 3,
          "PIN1: Clip %d Rate %d statt 1/2 mit Rate 3 (`lui v1,0x3` @0x8010938C)",
          t.first_clip, t.first_frac);
    CHECK(t.corpse_seen && (t.corpse_clip == 22 || t.corpse_clip == 23),
          "PIN1: Leichenpose %d (corpse_seen=%d)", t.corpse_clip, t.corpse_seen);
    CHECK(t.corpse_seen &&
          t.corpse_clip == (((unsigned)t.corpse_flags21a & 0x4u) ? 22 : 23),
          "PIN1: Leichenpose %d passt nicht zu +0x21A=%04X (`? 22 : 23` @0x8010A490-BC)",
          t.corpse_clip, (unsigned)t.corpse_flags21a);

    /* ===== PIN 2 — 0x80108BEC RAKETE (Waffe 18 -> Zeile 17): KOPF + BEIN(E) + RUMPF-STUMPF == */
    CHECK(find_kill(slot, 18, 5, -1, 700, &t),
          "PIN2: in 16 RNG-Verschiebungen kein Treffer der Zelle 5 (0x80108BEC)");
    dump("PIN2 Rakete", &t);
    CHECK(t.ok && t.cell == 5, "PIN2: Zelle %d statt 5 (0x80108BEC)", t.cell);
    CHECK(t.row == 17, "PIN2: Zeile %d statt 17", t.row);
    CHECK((t.pflags[8] & 0x4Au) == 0x4Au && t.ptint[8] == 0x00101030u,
          "PIN2: Kopf flags=%04X tint=%08X statt 0x4A / 0x00101030 (@0x80108DDC/@0x80108E08)",
          t.pflags[8], t.ptint[8]);
    CHECK((t.pflags[9] & 0x1062u) == 0x1062u,
          "PIN2: R-Oberschenkel %04X ohne 0x1062 (`ori v0,v0,0x1062` @0x80108E5C)", t.pflags[9]);
    CHECK((t.pflags[10] & 0x80u) && (t.pflags[11] & 0x80u),
          "PIN2: Stumpfmarker fehlen (10=%04X 11=%04X, @0x80108EB4/@0x80108F90)",
          t.pflags[10], t.pflags[11]);
    CHECK(t.pmesh[0] == 16u,
          "PIN2: Rumpf-Stumpf mesh0=%u statt 16 (Part-16-Geometrie -> Part 0 @0x80108FB8-FDC)",
          t.pmesh[0]);
    CHECK((t.first_clip == 3 || t.first_clip == 4) && t.first_frac == 3,
          "PIN2: Clip %d Rate %d statt 3/4 mit Rate 3 (`lui a1,0x3 / ori 0x4 / subu`"
          " @0x80108D74-98)", t.first_clip, t.first_frac);
    CHECK(t.first_speed == -450 || t.first_speed == 250,
          "PIN2: +0x144 = %d statt -450 (Front) bzw. 250 (Ruecken) (sp+16/18 @0x80108C14-20)",
          t.first_speed);
    CHECK(t.corpse_seen && (t.corpse_clip == 22 || t.corpse_clip == 23),
          "PIN2: keine Leiche (corpse_seen=%d clip=%d)", t.corpse_seen, t.corpse_clip);

    /* ===== PIN 3 — 0x80109610 WEGSCHLEUDERN (Waffe 8 -> Zeile 7 -> 3/4-Burst) ============== */
    CHECK(find_kill(slot, 8, 4, 1, 500, &t),
          "PIN3: in 16 RNG-Verschiebungen kein Burst-Ausstieg (+0x231 = 1) der Zeile 7");
    dump("PIN3 Schrot/Burst", &t);
    CHECK(t.ok && t.cell == 4, "PIN3: Zelle %d statt 4 (0x801066FC)", t.cell);
    CHECK(t.row == 7, "PIN3: Zeile %d statt 7", t.row);
    CHECK(t.rag231 == 1,
          "PIN3: +0x231 = %d statt 1 — der 3/4-Ausstieg in FUN_80109610 (@0x801067F4-828) "
          "hat nicht gezuendet", t.rag231);
    CHECK((t.first_clip == 1 || t.first_clip == 2) && t.first_af == 15,
          "PIN3: Clip %d ab Frame %d statt 1/2 ab Frame 15 (Wort 0x000F0F00 @0x80109840-64)",
          t.first_clip, t.first_af);
    CHECK(t.first_speed == 350,
          "PIN3: +0x144 im Todes-Tick = %d; erwartet 350 = 400 (@0x80109888) minus EIN "
          "Abbauschritt 50 (@0x8010996C-78), weil P0 in P1 DURCHFAELLT (@0x80109894)",
          t.first_speed);
    {   int ok = 1;
        for (int i = 1; i < t.speed_n; i++) {
            int prev = t.speed_seq[i-1], now = t.speed_seq[i];
            if (!((prev >= 50 && now == prev - 50) || (prev == 0 && now == 0))) ok = 0;
        }
        CHECK(ok, "PIN3: +0x144-Abbau nicht exakt -50 je Frame (%d %d %d %d %d %d)",
              t.speed_seq[0], t.speed_seq[1], t.speed_seq[2], t.speed_seq[3],
              t.speed_seq[4], t.speed_seq[5]);
    }
    CHECK(t.pmesh[0] == 0u && (t.pflags[8] & 0x40u) == 0,
          "PIN3: FUN_80109610 reisst KEINE Model-Parts ab (nur Effekt-Brocken), "
          "mesh0=%u kopf=%04X", t.pmesh[0], t.pflags[8]);
    CHECK(t.corpse_seen && t.end_state == 7,
          "PIN3: Endzustand %d statt 7 (`sw v0(=7),4` @0x801099B0)", t.end_state);

    /* ===== PIN 4 — 0x801066FC, der ANDERE Viertel-Zweig: HP = 10, Wiederbelebung ========== */
    CHECK(find_kill(slot, 8, 4, 0, 500, &t),
          "PIN4: in 16 RNG-Verschiebungen kein Ragdoll-Zweig (+0x231 = 0) der Zeile 7");
    dump("PIN4 Schrot/Ragdoll", &t);
    CHECK(t.ok && t.cell == 4, "PIN4: Zelle %d statt 4", t.cell);
    CHECK(t.rag231 == 0, "PIN4: +0x231 = %d — hier soll der Burst NICHT zuenden", t.rag231);
    CHECK(t.end_hp == 10 && t.end_state != 7,
          "PIN4: Endstand hp=%d st=%d statt hp=10 + lebendig (`sh 10,342` @0x80106AE8)",
          t.end_hp, t.end_state);
    CHECK((t.end_f10e & 1) != 0,
          "PIN4: +0x10E = %04X ohne Kriecher-Bit 0 (`sh 8193,270` @0x80106B0C)",
          (unsigned)t.end_f10e);

    /* ===== PIN 5 — 0x80108BEC ueber die ANDERE Weiche (Waffe 13 SPAS, Zeile 8) ============= */
    CHECK(find_kill(slot, 13, 5, 0, 700, &t),
          "PIN5: in 16 RNG-Verschiebungen kein Zerreiss-Zweig der Zeile 8");
    dump("PIN5 SPAS/Zerreiss", &t);
    CHECK(t.ok && t.cell == 5, "PIN5: Zelle %d statt 5", t.cell);
    CHECK(t.row == 8, "PIN5: Zeile %d statt 8", t.row);
    CHECK((t.pflags[8] & 0x4Au) == 0x4Au && t.pmesh[0] == 16u,
          "PIN5: kein Zerreiss-Satz (kopf=%04X mesh0=%u)", t.pflags[8], t.pmesh[0]);
    /* Gegenprobe DERSELBEN Zeile: der 3/4-Ausstieg @0x80108C8C muss auch dort erreichbar sein. */
    CHECK(find_kill(slot, 13, 5, 1, 500, &t),
          "PIN5b: Zeile 8 erreicht den Burst-Ausstieg (+0x231 = 1, @0x80108D28-34) in keiner "
          "der 16 RNG-Verschiebungen");
    dump("PIN5b SPAS/Burst", &t);
    CHECK(t.first_af == 15,
          "PIN5b: Burst-Startframe %d statt 15 (Wort 0x000F0F00 @0x80109840-64)", t.first_af);

    /* ===== PIN 6 — 0x80107438 (Spalte 0), POSITIV-KONTROLLE ============================== */
    run_forced_death(slot, 7, 0, 400, &t);
    dump("PIN6 Spalte0/7438", &t);
    CHECK(t.ok && t.cell == 2,
          "PIN6: Zelle %d statt 2 — die Positiv-Kontrolle fuer 0x80107438 hat die Spalte 0 "
          "nicht erreicht", t.cell);
    CHECK(t.first_clip == 1,
          "PIN6: Clip %d statt 1 (`lui v0,0xf / ori 1 / sw v0,332` @0x80107490-C8)",
          t.first_clip);
    CHECK(t.end_state == 7 || t.end_hp == 10,
          "PIN6: Ausgang st=%d hp=%d — erwartet CORPSE (`sw 7,4` @0x801077E4) ODER "
          "HP = 10 (`sh 10,342` @0x801077B0)", t.end_state, t.end_hp);
    /* NEGATIVKONTROLLE zur Positiv-Kontrolle: DIESELBE Zeile mit der Spalte, die der Port
     * wirklich stempelt, geht in die Ragdoll-Zelle — die Spalte ist also der Unterschied. */
    run_forced_death(slot, 7, 1, 400, &t);
    dump("PIN6b Spalte1/66FC", &t);
    CHECK(t.cell == 4, "PIN6b: Zeile 7 Spalte 1 dispatcht %d statt 4 (0x801066FC)", t.cell);

    /* ===== PIN 7 — REGRESSIONSWACHE: die schon byte-truen Waffen ========================== */
    {   const int wpn[] = { 3, 4, 12, 19, 20 };   /* Browning HP, Beretta, Ingram, MC51, Colt */
        for (unsigned i = 0; i < sizeof wpn / sizeof wpn[0]; i++) {
            run_kill(slot, wpn[i], 0, 0, 900, &t);
            printf("  PIN7 Waffe %-2d -> ", wpn[i]); dump("", &t);
            CHECK(t.ok, "PIN7: Waffe %d hat den Zombie nicht getoetet", wpn[i]);
            /* Die Zeilen dieser Waffen (2/3/13/15/18) tragen in JEDER Spalte die Zelle 1
             * (re2z_death_tbl, gegen den Dump gepinnt). Erreicht der Zombie die Wurzel
             * liegend/kriechend, gewinnen die drei VORGESCHALTETEN Zweige (@0x801083B0 /
             * @0x801083EC / @0x80108408, Diagnosewerte -1/-2/-3) — auch das ist byte-true.
             * Was NIE passieren darf: eine der Zerreiss-Zellen 2..6. */
            CHECK(t.cell == 1 || t.cell < 0,
                  "PIN7: Waffe %d dispatcht Zelle %d — nur MAIN (1) oder die Vorzweige (<0) "
                  "sind hier erlaubt, REGRESSION", wpn[i], t.cell);
            CHECK(t.cell < 2,
                  "PIN7: Waffe %d landet in einer ZERREISS-Zelle (%d) — REGRESSION",
                  wpn[i], t.cell);
            if (t.cell == 1)
                CHECK((t.first_clip == 1 || t.first_clip == 2) && t.first_frac == 0xF,
                      "PIN7: Waffe %d Clip %d Rate %d statt 1/2 mit Rate 15 (@0x8010860C-30)",
                      wpn[i], t.first_clip, t.first_frac);
            CHECK(t.pmesh[0] == 0u, "PIN7: Waffe %d stempelt einen Rumpf-Stumpf (mesh0=%u)",
                  wpn[i], t.pmesh[0]);
            for (int p = 0; p < 15; p++)
                CHECK((t.pflags[p] & 0x40u) == 0,
                      "PIN7: Waffe %d reisst Part %d ab (flags=%04X) — REGRESSION",
                      wpn[i], p, t.pflags[p]);
        }
    }

    /* ===== PIN 8 — NEGATIVTEST RE1.5-MODUS ==============================================
     * Gemessen wird DERSELBE Reiz wie in PIN 2 (Zeile 17 / Spalte 1 -> Zelle 0x80108BEC, die
     * im RE2-Modus Kopf + Bein(e) + Rumpf-Stumpf stempelt), nur mit RE15_AI_FLAVOR_RE15. Dort
     * laeuft das RE1.5-Gehirn (enemy_ai_common.c), die RE2-Wurzel wird gar nicht betreten —
     * der Modellblock muss also unberuehrt bleiben. Die POSITIV-KONTROLLE dazu ist PIN 2/5:
     * derselbe Reiz reisst im RE2-Modus nachweislich ab. Der Test haengt bewusst NICHT am
     * Landen eines Schusses (im Unit-Harness gibt es fuer RE1.5 keine geladene Bank). */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    bringup();
    re15_inv_load_briefing();
    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
    {   int s15 = standing_zombie();
        if (s15 < 0) { printf("SKIP PIN8: kein Zombie\n"); }
        else {
            for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != s15) g_actors[s].active = 0;
            s_pristine = g_actors[s15]; s_pristine_ok = 1;   /* eigener Schnappschuss */
            re15_actor_t *e15 = &g_actors[s15];
            run_forced_death(s15, 17, 1, 400, &t);
            dump("PIN8 RE1.5/Zeile17", &t);
            printf("      (cell oben ist der STEHENGEBLIEBENE Diagnosewert der RE2-Laeufe — im "
                   "RE1.5-Modus schreibt ihn niemand; Kriterium sind die Part-Felder:)\n");
            printf("      RE1.5-Endstand: st=%d s1=%u hp=%d mesh0=%u kopf=%04X\n",
                   e15->state, e15->sub_state_1, e15->hp, e15->re2z_part_mesh[0],
                   e15->re2z_part_flags[8]);
            for (int p = 0; p < 16; p++) {
                CHECK((e15->re2z_part_flags[p] & 0x40u) == 0,
                      "PIN8: RE1.5-Modus hat Part %d abgerissen (flags=%04X) — REGRESSION",
                      p, e15->re2z_part_flags[p]);
                CHECK(e15->re2z_part_mesh[p] == 0u || e15->re2z_part_mesh[p] == (uint8_t)p,
                      "PIN8: RE1.5-Modus hat Part %d auf Mesh %u getauscht — REGRESSION",
                      p, e15->re2z_part_mesh[p]);
            }
            CHECK(e15->re2z_part_mesh[0] != 16u,
                  "PIN8: RE1.5-Modus hat den Rumpf-Stumpf (Mesh 16) gestempelt — REGRESSION");
        }
    }
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);

    free(buf);
    if (fails) { printf("test_re2_zombie_teardeath: %d FAIL\n", fails); return 1; }
    printf("test_re2_zombie_teardeath: OK\n");
    return 0;
}
