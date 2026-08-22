/* probe_re15_poise_re2.c — MESSREIHE zur STURZ-KADENZ des RE1.5-Zombies (ROOM1140).
 *
 * NUTZER-ENTSCHEIDUNG 2026-08-22 (woertlich): "Ja, naja, der fette Zombie muss nicht jeden
 * 2. Schuss umfallen. Das ist unschoen."  -> BEWUSSTE ABWEICHUNG vom RE1.5-Original, die aus
 * RE2 UEBERNOMMEN wird (wie der schon bestellte RE2-Schadens-/HP-Import), NICHT geraten.
 *
 * ---- ORIGINAL-SEITE, beide Regeln selbst disassembliert -------------------------------------
 * RE1.5 (STAGE1.BIN roh @0x80100000) = POISE-Leiste +0x1DC:
 *   80100824 jal 0x8001af20 / 8010082c andi 0x3 / 80100838 addiu 4 / 8010083c sh v0,476(v1)
 *       => +0x1DC = (rng&3)+4 = 4..7 beim INIT
 *   Schritt-Tabelle @0x8011FE30: [3]..[6] -> 0x80105A6C `addiu v0,v0,-3`  (Pistole = Waffe 3)
 *       Aufruf im Phase-0-Block des Staggers @0x80105D28 = EINMAL PRO TREFFER
 *   Sturz-Tor @0x80105B18-68: `lh +0x1DC` @0x80105B24 / `bgez -> return` @0x80105B2C, sonst
 *       +0x4=1 / +0x5=0x11 / +0x6=0  =  KNOCKDOWN
 *   Aufstehen Phase[6] @0x8010544C-5C laedt wieder (rng&3)+4 (Wurf hier konstant 0 -> 4)
 *   => Sturz beim 2. bzw. 3. Pistolentreffer, nach dem Aufstehen ALLE 2 TREFFER.
 *
 * RE2 (EMOVL10_S0.BIN roh @0x80100000) = RESISTENZ-Leiste +0x223 MIT HP-TOR:
 *   INIT   @0x8010089C  `sb v0,547(s2)`  = 16 + (rand&0xf)      (jal 0x80015FE8 @0x80100888)
 *   Kosten @0x801055D0-EC  `lui at,0x8011 / addu at,at,v0 / lbu v1,-13261(at)` = 0x8010CC33+row
 *          `subu a0,a0,v1` @0x801055E0 / `sb a0,547(s1)` @0x801055EC
 *          Tabelle 0x8010CC33 = {_,9,15,15,35,0,0,0,0,0,0,0,0,20,0,0,0,0}; Pistole = Zeile 3 -> 15
 *   NACHLADEN @0x801055E4-620:
 *          801055e4 andi 0x40 (+0x10E) / 801055e8 bne -> ueberspringen
 *          801055f0 sll v0,a0,24 / 801055f4 bgtz  -> Leiste noch > 0 -> ueberspringen
 *          801055fc lh v0,342(s1) (= HP) / 80105604 slti v0,v0,81 / 80105608 bne -> HP<81:
 *                   NICHT nachladen
 *          80105610 jal 0x80015fe8 / 80105618 andi 0xf / 8010561c addiu 16 / 80105620 sb 547
 *   STURZ-TOR @0x8010503C-AC: nur Phase 0 (`lbu 6` @0x8010505C / `bne` @0x80105064),
 *          `lb v0,547` @0x8010506C / `slt v0,v1,v0` @0x80105074 / `bne -> raus` @0x80105078,
 *          dann +0x222=1 @0x801050A0 und `sw 1281,4(s1)` @0x801050A4-AC = 0x501 = EXEC[5] STURZ
 *   => solange HP >= 81 wird die Leiste bei jedem Erschoepfen SOFORT neu gefuellt, das Sturz-Tor
 *      feuert also NIE. Erst im Endspiel (HP < 81) faellt der Zombie.
 *
 * ---- WAS DIESE SONDE MISST -------------------------------------------------------------------
 * ECHTER Weg: re15_game_step() + Pad (R1 halten -> QUADRAT-Flanke), echte ROOM1140-RDT/SCD-Spawns,
 * GELADENE Gegner-Baenke EM010/EM011/EM016 (ohne Bank ist re15_actor_clip_len()==0 und der
 * Aufsteh-Zweig laeuft nie fertig -> Frame-Zahlen waeren Artefakte).
 * Je Typ 0x10/0x11/0x16 und je Schalterstellung des RE2-Imports:
 *   Treffer bis Tod | Zahl der Stuerze | Treffer/Sturz | Frames bis Tod.
 * Schalter AUS = byte-true RE1.5-Auslieferungsstand (NEGATIV-TEST: muss die alte Kadenz zeigen).
 *
 * Kein add_test — reine Messsonde.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_esp.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_tim.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_msg.h"
#include "re15_inventory.h"
#include "re15_collision.h"
#include "re15_room.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void    re15_player_aim_reset(void);
extern void    re15_player_set_aim_clip_len(int fc);
extern int16_t re15_atan2_q12(int32_t dz, int32_t dx);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_room_id  = 0x1140;
static int                s_fire_sub = -1;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* ---- GEGNER-BANK laden (ZWINGEND, s. Kopf) ------------------------------------------------- */
static uint8_t *s_blob[3];
static int      s_bank_ok[3];
static const uint8_t s_types[3] = { 0x10, 0x11, 0x16 };

static void load_banks(const char *base)
{
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = slurp(p, &n);
    if (!ems) { printf("!! CDEMD0.EMS nicht lesbar (%s)\n", p); return; }
    for (int i = 0; i < 3; i++) {
        int idx = re15_ems_index_for_type(s_types[i]);
        size_t off = 0, len = 0;
        if (idx < 0 || re15_ems_get_entry(ems, n, idx, &off, &len) != 0) continue;
        free(s_blob[i]); s_blob[i] = (uint8_t *)malloc(len);
        if (!s_blob[i]) continue;
        memcpy(s_blob[i], ems + off, len);
        re15_enemy_bank_t *eb = re15_enemy_find(s_types[i]);
        if (!eb) eb = re15_enemy_alloc(s_types[i]);
        if (!eb) continue;
        re15_tim_t tim = (re15_tim_t){0};
        if (re15_emd_parse_container(s_blob[i], len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0)
            continue;
        eb->ok = 1; eb->buf = NULL;
        eb->loco_ok = (re15_emd_parse_loco_bank(s_blob[i], len, &eb->skel_loco, &eb->anim_loco) == 0);
        eb->own_ok  = (re15_emd_parse_own_bank (s_blob[i], len, &eb->skel_own,  &eb->anim_own ) == 0);
        s_bank_ok[i] = 1;
    }
    free(ems);
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static void bringup(uint32_t seed)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_player_victim_reset();
    re15_esp_fx_reset(); re15_wound_reset();
    re15_damage_seed_rng(seed);
    g_current_room_id = (uint16_t)s_room_id;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    if (s_fire_sub >= 0 && s_rdt.sub_scd_count > s_fire_sub && s_rdt.sub_scd[s_fire_sub])
        scd_thread_start(2, s_rdt.sub_scd[s_fire_sub]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
}

static int load_room(const char *base, const char *sub, int room_id, int fsub)
{
    char path[600];
    snprintf(path, sizeof path, "%s/%s", base, sub);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("RDT fehlt: %s\n", path); return 0; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("RDT-Parse: %s\n", path); return 0; }
    s_room_id = room_id; s_fire_sub = fsub;
    return 1;
}

/* Rueckstoss nachfuehren (sonst misst man REICHWEITE statt Schaden) — dasselbe Verfahren wie
 * probe_dogwound_zknock.c / probe_re2_hp_model.c. Der SCHUSS-WEG bleibt echt. */
static void track(int slot, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    int32_t dx = e->x - pl->x, dz = e->z - pl->z;
    int64_t q  = (int64_t)dx*dx + (int64_t)dz*dz;
    double  d  = q > 0 ? __builtin_sqrt((double)q) : 0.0;
    if (d > 1.0) { pl->x = e->x - (int32_t)((double)dx / d * back);
                   pl->z = e->z - (int32_t)((double)dz / d * back); }
    else         { pl->x = e->x - back; pl->z = e->z; }
    pl->y = e->y;
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
}

static int pick_type(int want_type)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == (uint8_t)want_type
            && !(g_actors[s].grid_id & 0x80)) return s;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == (uint8_t)want_type) return s;
    return -1;
}

typedef struct { int hits, knocks, frames, hp0, dmg1, poise0, dead, clip_len;
                 int end_state, end_sub, end_hp, end_band, end_grid; } run_t;

static void zombie_run(uint32_t seed, int want_type, int import_on, run_t *r)
{
    memset(r, 0, sizeof *r);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_re15_re2z_import_set(import_on);
    bringup(seed);
    re15_inv_load_briefing();
    g_inv.slots[1].id = 3; g_inv.slots[1].qty = 250;
    re15_player_set_equipped_weapon(3);                 /* Browning HP = Waffe 3 */
    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }

    int slot = pick_type(want_type);
    r->hits = -1;
    if (slot < 0) return;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    track(slot, 2600);
    r->hp0 = e->hp; r->poise0 = e->hit_stun;
    r->clip_len = re15_actor_clip_len(e);
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) {
        pl->hp = 100; track(slot, 2600); frame(RE15_PAD_BIT_R1, 0);
    }
    int hits = 0, hp_last = e->hp, was_knocked = 0;
    for (int f = 0; f < 40000; f++) {
        pl->hp = 100;
        g_inv.slots[1].qty = 250;   /* Magazin nachfuellen: OHNE das klemmt die Feuer-Sperre
                                     * re15_ammo_mag_nonzero (@0x80033300-84) nach ~250 Schuessen
                                     * und die Messung meldet faelschlich "stirbt nicht". */
        track(slot, 2600);
        int on = ((f >> 1) & 1) == 0;
        /* ELEVATION MITZIEHEN — genau das, was der Spieler tut: der liegende/fressende Zombie
         * (Band 1) wird nur vom TIEFEN Schuss getroffen, das Band-4-Ziel nur vom hohen
         * (Vergleich Spieler-Band gegen +0x90-Band, probe_1140_feeder_shot.c). Ohne das misst
         * man bei Typ 0x16 REICHWEITE/BAND statt der Sturz-Kadenz. */
        uint16_t elev = (e->aim_band == 1) ? RE15_PAD_BIT_DOWN
                      : (e->aim_band == 4) ? RE15_PAD_BIT_UP : 0;
        frame((uint16_t)(RE15_PAD_BIT_R1 | elev | (on ? RE15_PAD_BIT_SQUARE : 0)),
              (uint16_t)(on && ((f & 3) == 0) ? RE15_PAD_BIT_SQUARE : 0));
        if (e->hp < hp_last) { if (!hits) r->dmg1 = hp_last - e->hp; hits++; }
        hp_last = e->hp;
        /* STURZ = ACTIVE(+0x4=1) mit +0x5 == 0x11 (Sturz-Tor @0x80105B48-68). */
        int knocked = (e->state == 1 && e->sub_state_1 == 0x11);
        if (knocked && !was_knocked) r->knocks++;
        was_knocked = knocked;
        r->frames = f + 1;
        if (e->hp < 0 || e->state == 3 || e->state == 7) { r->dead = 1; break; }
    }
    r->hits = hits;
    r->end_state = e->state; r->end_sub = e->sub_state_1; r->end_hp = e->hp;
    r->end_band = e->aim_band; r->end_grid = e->grid_id;
}

static void sweep(int type, int import_on, int nseeds)
{
    int nsum=0, ksum=0, fsum=0, cnt=0, nmin=9999, nmax=0, kmin=9999, kmax=0;
    int hp_min=9999, hp_max=0, dmg=0, p_min=9999, p_max=-9999, miss=0, clip=0;
    for (int s = 0; s < nseeds; s++) {
        run_t r;
        zombie_run(0x2000u + (uint32_t)s * 0x9e3779b9u, type, import_on, &r);
        if (r.hits <= 0 || !r.dead) {
            if (miss == 0)
                printf("    [diag Typ 0x%02X %s seed %d] Treffer=%d tot=%d state=%d sub=%d hp=%d "
                       "band=%d grid=0x%02X\n", type, import_on ? "AN" : "AUS", s, r.hits, r.dead,
                       r.end_state, r.end_sub, r.end_hp, r.end_band, r.end_grid);
            miss++; continue;
        }
        cnt++; nsum += r.hits; ksum += r.knocks; fsum += r.frames; dmg = r.dmg1; clip = r.clip_len;
        if (r.hits < nmin) nmin = r.hits;   if (r.hits > nmax) nmax = r.hits;
        if (r.knocks < kmin) kmin = r.knocks; if (r.knocks > kmax) kmax = r.knocks;
        if (r.hp0 < hp_min) hp_min = r.hp0; if (r.hp0 > hp_max) hp_max = r.hp0;
        if (r.poise0 < p_min) p_min = r.poise0; if (r.poise0 > p_max) p_max = r.poise0;
    }
    if (!cnt) { printf("  Typ 0x%02X %-9s n/a (kein Treffer/Tod, %d Laeufe)\n",
                       type, import_on ? "IMPORT-AN" : "IMPORT-AUS", miss); return; }
    printf("  Typ 0x%02X %-10s hp0 %d..%d  Leiste0 %d..%d  dmg %2d  clip_len %d | "
           "Treffer %d..%d (M %.1f) | Stuerze %d..%d (M %.2f) | Treffer/Sturz %.2f | "
           "Frames M %.0f (%.1f s) | n/a %d\n",
           type, import_on ? "IMPORT-AN" : "IMPORT-AUS",
           hp_min, hp_max, p_min, p_max, dmg, clip,
           nmin, nmax, (double)nsum / cnt, kmin, kmax, (double)ksum / cnt,
           ksum ? (double)nsum / ksum : 0.0, (double)fsum / cnt, (double)fsum / cnt / 30.0, miss);
}

int main(int argc, char **argv)
{
    const char *envb = getenv("RE15_ASSET_DIR");
    const char *base = (envb && *envb) ? envb : RE15_ASSET_PSX_DIR;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    int nseeds = (argc > 1) ? atoi(argv[1]) : 64;
    if (nseeds < 1) nseeds = 64;

    re15_enemy_reset();
    load_banks(base);
    printf("Baenke: EM010 %s  EM011 %s  EM016 %s\n",
           s_bank_ok[0] ? "OK" : "FEHLT", s_bank_ok[1] ? "OK" : "FEHLT", s_bank_ok[2] ? "OK" : "FEHLT");

    if (!load_room(base, "STAGE1/ROOM1140.RDT", 0x1140, -1)) return 77;

    /* ZENSUS: welche Typen stellt ROOM1140 ueberhaupt auf? (sonst liest sich ein fehlender Typ
     * faelschlich als "Messrahmen kaputt"). */
    re15_re15_re2z_import_set(1); re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    bringup(0x2000u);
    printf("ROOM1140-Spawns:");
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active) printf(" [%d]=0x%02X%s", s, g_actors[s].type,
                                       (g_actors[s].grid_id & 0x80) ? "(liegend)" : "");
    printf("\n");

    printf("\n===== ROOM1140, PISTOLE (Waffe 3), RE1.5-KI-MODUS, %d Seeds =====\n", nseeds);
    printf("RE1.5-Regel : Poise +0x1DC = (rng&3)+4 @0x8010082C-3C, -3/Treffer @0x8011FE3C->0x80105A6C,\n"
           "              Tor `bgez` @0x80105B2C -> Sturz beim 2./3. Treffer, Aufstehen re-armt 4 @0x8010544C\n"
           "RE2-Regel   : Resistenz +0x223 = 16+(rand&0xf) @0x8010089C, -cost[Zeile] @0x8010CC33 (Pistole 15),\n"
           "              Nachladen NUR bei HP >= 81 (`slti 81` @0x80105604) -> kein Sturz solange gesund\n");
    for (int i = 0; i < 3; i++) {
        printf("\n");
        sweep(s_types[i], 0, nseeds);   /* NEGATIV-TEST: byte-true RE1.5 */
        sweep(s_types[i], 1, nseeds);   /* Import (Nutzer-Default) */
    }
    /* Typ 0x16 steht in ROOM1140 nur als KRIECHER (grid 0x80, aim_band 0) und ist dort in KEINER
     * Schalterstellung beschiessbar — vorbestehende Eigenschaft des Aktors, kein Messrahmen-
     * Fehler (s. Diagnosezeile). Die Kontrollmessung fuer 0x16 laeuft deshalb zusaetzlich in
     * ROOM1030. */
    if (load_room(base, "STAGE1/ROOM1030.RDT", 0x1030, -1)) {
        re15_re15_re2z_import_set(1); re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        bringup(0x2000u);
        printf("\nROOM1030-Spawns:");
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active) printf(" [%d]=0x%02X%s", s, g_actors[s].type,
                                           (g_actors[s].grid_id & 0x80) ? "(liegend)" : "");
        printf("\n===== ROOM1030, PISTOLE, RE1.5-KI-MODUS, %d Seeds =====\n", nseeds);
        for (int i = 0; i < 3; i++) {
            printf("\n");
            sweep(s_types[i], 0, nseeds);
            sweep(s_types[i], 1, nseeds);
        }
    }
    for (int i = 0; i < 3; i++) free(s_blob[i]);
    return 0;
}
