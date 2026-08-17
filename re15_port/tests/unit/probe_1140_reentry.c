/* probe_1140_reentry.c — MESSUNG (Nutzer-Report 2026-08-17):
 * "Wenn man in die Zombie-Menge im Dinner-Raum (ROOM1140) reinlaeuft, den Raum
 *  verlaesst und wieder reingeht, sind die Zombies broken / verhalten sich komisch."
 *
 * Die BEDINGUNG des Reports ist "erst REINLAUFEN". Das ist im Original ein
 * DATEN-Ereignis, kein Zufall (ROOM1140.RDT sub00, eigene SCD-Disassembly):
 *
 *   f0x0b78  Ifel_ck  06 00 96 00      -> ELSE-Rumpf @f0x0c12
 *   f0x0b7c  Ck       21 03 d2 00      ; Flag Zone3 / Bit 0xD2 == 0 ?
 *   IF (Erstbesuch, Flag CLEAR):
 *     f0x0b80  Aot_set  2c 06 03 41 00 00 80 f3 02 a4 88 13 d4 17 ff 00 18 04 00 00
 *              = AOT-Slot 6, sce 3 (Event), Rect x=-3200 z=-23550 w=5000 d=6100,
 *                Event -> sub04.   ** Dieses Rechteck LIEGT UEBER DER ZOMBIE-MENGE **
 *     f0x0baa..f0x0bfa  5x Sce_em_set  beh 0x88/0x86 (liegend/fressend)
 *   ELSE (Wiedereintritt, Flag SET):
 *     f0x0c12..f0x0c62  5x Sce_em_set  beh 0x02 (STEHENDE Schlaefer), andere Positionen
 *
 *   sub04 @f0x0d52:  Aot_reset(6) / Sleep / f0x0d60  Set 22 03 d2 01  = Flag(3,0xD2)=1
 *
 * => "In die Menge reinlaufen" SETZT das Flag; der Raum spawnt beim Wiedereintritt
 *    den behavior-0x02-Zweig. Genau diese Sequenz fahren wir hier nach.
 *
 * Ablauf: Entry#1 (Tuerspawn) -> in die Menge laufen (AOT-Scan!) -> Kampf-Ticks ->
 *         Tuer 0 nach ROOM1130 -> zurueck nach ROOM1140 -> Zustand messen.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_collision.h"
#include "re15_damage.h"   /* re15_damage_seed_rng */

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

/* Banks fuer die drei ROOM1140-Zombietypen (nach jedem re15_enemy_reset noetig) */
static void load_bank(const char *base, uint8_t type, uint8_t *scratch, size_t scratch_sz)
{
    char emsp[600]; size_t ems_size = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(emsp, &ems_size);
    if (!ems) return;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0 && len <= scratch_sz) {
        re15_enemy_bank_t *eb = re15_enemy_alloc(type);
        if (eb) {
            memcpy(scratch, ems + off, len);
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(scratch, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok = (re15_emd_parse_loco_bank(scratch, len, &eb->skel_loco, &eb->anim_loco) == 0);
                eb->own_ok  = (re15_emd_parse_own_bank(scratch, len, &eb->skel_own, &eb->anim_own) == 0);
            }
        }
    }
    free(ems);
}

static uint8_t s_b16[0x80000], s_b10[0x80000], s_b11[0x80000];
static void load_banks(const char *base)
{
    load_bank(base, 0x16, s_b16, sizeof s_b16);
    load_bank(base, 0x10, s_b10, sizeof s_b10);
    load_bank(base, 0x11, s_b11, sizeof s_b11);
}

/* Kern von re15_room_apply_pending (room_common.c) — der echte Tuerpfad. */
static void door_transition(const re15_rdt_t *rdt, unsigned room_id,
                            int32_t x, int32_t z, int16_t yaw)
{
    const int16_t  keep_hp     = g_actors[RE15_ACTOR_SLOT_PLAYER].hp;
    const uint16_t keep_status = g_actors[RE15_ACTOR_SLOT_PLAYER].status_flags;
    re15_actor_init();
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp           = keep_hp;
    g_actors[RE15_ACTOR_SLOT_PLAYER].status_flags = keep_status;
    {
        re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        p->active = 1; p->type = 0;
        p->motion = 0; p->anim_frame = 0; p->anim_flags = 0;
        p->sub_state_1 = 0; p->sub_state_2 = 0; p->sub_state_3 = 0;
        p->state = 1;
        g_scd.player_mode = 0;
        p->x = x; p->y = 0; p->z = z; p->rot_y = yaw;
    }
    re15_enemy_reset();
    re15_player_cmd_reset();
    g_current_room_id = room_id;
    scd_room_reenter(rdt, x, z, 0);
}

static void tick_once(void)
{
    re15_aot_scan(g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z, 0);
    /* wie re15_game_step (game_step_common.c:1146): das im Scan gelatchte AOT-Event zünden */
    if (g_aot.fired_event_id_this_frame != 0)
        (void)scd_event_fire(g_aot.fired_event_id_this_frame);
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_actors_anim_advance();
    re15_enemy_ai_run_all(0);
}

static void dump_aots(const char *tag)
{
    printf("== AOTs %s ==\n", tag);
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active) continue;
        printf("  aot=%d type=%d evt=0x%02x flags=0x%02x band=0x%02x c=(%ld,%ld) half=(%ld,%ld) quad=%d\n",
               i, (int)a->type, a->event_id, a->sce_flags, a->band,
               (long)a->x, (long)a->z, (long)a->half_w, (long)a->half_h, a->has_quad);
    }
}

static void dump_roster(const char *tag)
{
    printf("== Roster %s ==  Ck(3,0xd2)=%d Ck(3,0x6a)=%d emzone=%d\n", tag,
           re15_game_flag_get(3, 0xd2), re15_game_flag_get(3, 0x6a),
           (int)re15_em_status_zone());
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *a = &g_actors[s];
        if (!a->active || s == RE15_ACTOR_SLOT_PLAYER) continue;
        printf("  slot=%d type=0x%02X grid=0x%02X st=%u s1=0x%02x s2=%u s3=%u mo=%-3d fr=%-3u "
               "frac=%-3u af=0x%04x hp=%-4d emflag=0x%02x pos=(%ld,%ld) rot=%d tmr=%d\n",
               s, a->type, a->grid_id, a->state, a->sub_state_1, a->sub_state_2, a->sub_state_3,
               (int)a->motion, a->anim_frame, a->anim_frac, a->anim_flags, (int)a->hp,
               a->em_flag_id, (long)a->x, (long)a->z, (int)a->rot_y, (int)a->ai_timer);
    }
}

/* Kontroll-Zustands-Trace: druckt fuer JEDEN Gegner jede Aenderung von st/s1/s2/s3/mo/grid. */
static uint8_t s_tr[RE15_ACTOR_MAX][6];
static void trace_reset(void) { memset(s_tr, 0xFF, sizeof s_tr); }
static void trace_tick(const char *tag, int t)
{
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *a = &g_actors[s];
        if (!a->active || s == RE15_ACTOR_SLOT_PLAYER) continue;
        uint8_t n[6] = { a->state, a->sub_state_1, a->sub_state_2, a->sub_state_3,
                         (uint8_t)a->motion, a->grid_id };
        if (memcmp(n, s_tr[s], 6) != 0) {
            printf("  [%s t=%4d] slot=%d st=%u s1=0x%02x s2=%u s3=%u mo=%-3d grid=0x%02x "
                   "fr=%-3u dist=%u pos=(%ld,%ld)\n",
                   tag, t, s, n[0], n[1], n[2], n[3], (int)n[4], n[5],
                   a->anim_frame, (unsigned)a->ai_dist, (long)a->x, (long)a->z);
            memcpy(s_tr[s], n, 6);
        }
    }
}

/* Bewegungs-/Aktivitaets-Zensus: wieviele Zombies aendern ueber N Ticks ihre Pose/Position? */
static void census(const char *tag, int nticks)
{
    typedef struct { int32_t x, z; uint16_t fr; uint8_t s1, s2, mo; } snap_t;
    snap_t first[RE15_ACTOR_MAX];
    int    movers = 0, posers = 0, statech = 0;
    int32_t maxmove[RE15_ACTOR_MAX];
    uint16_t frmin[RE15_ACTOR_MAX], frmax[RE15_ACTOR_MAX];
    memset(maxmove, 0, sizeof maxmove);
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        first[s].x = g_actors[s].x; first[s].z = g_actors[s].z;
        first[s].fr = g_actors[s].anim_frame; first[s].s1 = g_actors[s].sub_state_1;
        first[s].s2 = g_actors[s].sub_state_2; first[s].mo = (uint8_t)g_actors[s].motion;
        frmin[s] = 0xFFFF; frmax[s] = 0;
    }
    trace_reset();
    for (int t = 0; t < nticks; t++) {
        tick_once();
        trace_tick(tag, t);
        for (int s = 0; s < RE15_ACTOR_MAX; s++) {
            if (!g_actors[s].active || s == RE15_ACTOR_SLOT_PLAYER) continue;
            int32_t dx = g_actors[s].x - first[s].x, dz = g_actors[s].z - first[s].z;
            int32_t d = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
            if (d > maxmove[s]) maxmove[s] = d;
            if (g_actors[s].anim_frame < frmin[s]) frmin[s] = g_actors[s].anim_frame;
            if (g_actors[s].anim_frame > frmax[s]) frmax[s] = g_actors[s].anim_frame;
        }
    }
    printf("== Zensus %s (%d Ticks) ==\n", tag, nticks);
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        if (!g_actors[s].active || s == RE15_ACTOR_SLOT_PLAYER) continue;
        int poses = (frmax[s] > frmin[s]);
        int sc = (g_actors[s].sub_state_1 != first[s].s1 || g_actors[s].sub_state_2 != first[s].s2 ||
                  (uint8_t)g_actors[s].motion != first[s].mo);
        if (maxmove[s] > 8) movers++;
        if (poses) posers++;
        if (sc) statech++;
        printf("  slot=%d grid=0x%02X s1=0x%02x->0x%02x s2=%u->%u mo=%d->%d fr[%u..%u] "
               "move=%ld hp=%d\n",
               s, g_actors[s].grid_id, first[s].s1, g_actors[s].sub_state_1,
               first[s].s2, g_actors[s].sub_state_2, first[s].mo, (int)g_actors[s].motion,
               frmin[s], frmax[s], (long)maxmove[s], (int)g_actors[s].hp);
    }
    printf("  -> movers=%d posers=%d statechanged=%d\n", movers, posers, statech);
}

/* ===================== DISKRIMINATOR-LAUF =============================================
 * Vergleicht den ECHTEN Wiedereintritt (nach "in die Menge laufen" + Kampf + Tuerkette)
 * gegen einen KALTSTART mit demselben Flag-Stand. Beide Laeufe: gleicher RNG-Seed
 * unmittelbar vor dem Raumeintritt, gleiche Spieler-Startpose, gleicher Spielerpfad.
 * Jede Differenz in der Ausgabe = Zustand, der den Raumwechsel UEBERLEBT hat.
 * (Das Original wischt beim Raumwechsel den Entity-Pool, und Sce_em_set initialisiert
 *  jeden Record neu — der Port darf hier also NICHTS mitschleppen.) */
static void ab_dump(int t)
{
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *a = &g_actors[s];
        if (!a->active || s == RE15_ACTOR_SLOT_PLAYER) continue;
        printf("T%04d s%d st%u s1_%02x s2_%u s3_%u mo%-3d fr%-3u fc%-3u af%04x hp%-4d "
               "g%02x x%-7ld z%-7ld r%-5d spd%-5d tmr%-5d flg%04x ctc%02x sca%02x\n",
               t, s, a->state, a->sub_state_1, a->sub_state_2, a->sub_state_3,
               (int)a->motion, a->anim_frame, a->anim_frac, a->anim_flags, (int)a->hp,
               a->grid_id, (long)a->x, (long)a->z, (int)a->rot_y, (int)a->speed_h,
               (int)a->ai_timer, a->ai_flags, a->ai_contact, a->sca_mask);
    }
}

/* Deterministischer Spielerpfad ab dem Tuerspawn — identisch in BEIDEN Laeufen. */
static void ab_player_path(int nticks)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int t = 0; t < nticks; t++) {
        int32_t tx, tz;
        if (t < 200) { tx = -1300; tz = -15950; }
        else         { tx = g_actors[2].x; tz = g_actors[2].z + 1400; }
        int32_t dx = tx - pl->x, dz = tz - pl->z;
        if (dx >  40) dx =  40; if (dx < -40) dx = -40;
        if (dz >  40) dz =  40; if (dz < -40) dz = -40;
        pl->x += dx; pl->z += dz;
        tick_once();
        if ((t % 20) == 0) ab_dump(t);
    }
}

int main(int argc, char **argv)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600]; size_t sz40 = 0, sz30 = 0;
    int walk_in = !(argc > 1 && argv[1][0] == 'N');   /* N = ohne Reinlaufen (Kontrolle) */

    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", base);
    uint8_t *d40 = read_file(path, &sz40);
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1130.RDT", base);
    uint8_t *d30 = read_file(path, &sz30);
    if (!d40 || !d30) { fprintf(stderr, "FAIL: RDT\n"); return 1; }
    static re15_rdt_t r40, r30;
    if (re15_rdt_parse(d40, sz40, &r40) != 0 || re15_rdt_parse(d30, sz30, &r30) != 0) {
        fprintf(stderr, "FAIL: parse\n"); return 1; }

    /* ---- Modus C: KALTSTART mit bereits gesetztem Flag(3,0xD2) ---- */
    if (argc > 1 && argv[1][0] == 'C') {
        re15_actor_init(); scd_vm_init();
        re15_game_flag_set(3, 0xd2, 1);       /* wie sub04 f0x0d60 `22 03 d2 01` */
        g_current_room_id = 0x1140;
        re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        p->active = 1; p->type = 0; p->hp = 100;
        p->x = -1300; p->y = 0; p->z = -13950; p->rot_y = 2048;
        re15_damage_seed_rng(0x13570ACEu);
        scd_room_reenter(&r40, p->x, p->z, 0);
        load_banks(base);
        for (int f = 0; f < 8; f++) tick_once();
        printf("##### MODUS C (Kaltstart, Flag gesetzt) #####\n");
        ab_player_path(600);
        return 0;
    }
    /* ---- Modus R: echter Wiedereintritt nach dem Reinlaufen ---- */
    if (argc > 1 && argv[1][0] == 'R') {
        re15_actor_init(); scd_vm_init();
        g_current_room_id = 0x1140;
        re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        p->active = 1; p->type = 0; p->hp = 100;
        p->x = -1300; p->y = 0; p->z = -13950; p->rot_y = 2048;
        scd_room_reenter(&r40, p->x, p->z, 0);
        load_banks(base);
        for (int f = 0; f < 8; f++) tick_once();
        { int32_t tx = -800, tz = -20600;              /* in die Menge laufen */
          for (int i = 0; i < 300; i++) {
            int32_t dx = tx - p->x, dz = tz - p->z;
            if (dx > 40) dx = 40; if (dx < -40) dx = -40;
            if (dz > 40) dz = 40; if (dz < -40) dz = -40;
            p->x += dx; p->z += dz; tick_once(); } }
        for (int t = 0; t < 200; t++) tick_once();     /* Kontakt/Kampf */
        p->hp = 100; p->state = 1; p->sub_state_1 = 0; p->sub_state_2 = 0;
        p->motion = 0; p->anim_frame = 0; re15_player_cmd_reset();
        door_transition(&r30, 0x1130, 0, 0, 0);        /* raus */
        for (int t = 0; t < 120; t++) tick_once();
        re15_damage_seed_rng(0x13570ACEu);             /* SELBER Seed wie Modus C */
        door_transition(&r40, 0x1140, -1300, -13950, 2048);   /* zurueck */
        load_banks(base);
        for (int f = 0; f < 8; f++) tick_once();
        printf("##### MODUS R (echter Wiedereintritt) #####\n");
        ab_player_path(600);
        return 0;
    }

    printf("##### ROOM1140 Re-Entry — %s #####\n",
           walk_in ? "MIT Reinlaufen in die Menge" : "OHNE Reinlaufen (Kontrolle)");

    /* --- Erstbesuch: Tuer-0-Spawn aus main00 (next=(-1300,0,-13950) yaw 2048) --- */
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1140;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -1300; pl->y = 0; pl->z = -13950; pl->rot_y = 2048;
    scd_room_reenter(&r40, pl->x, pl->z, 0);
    load_banks(base);
    for (int f = 0; f < 8; f++) tick_once();
    dump_roster("Entry#1");
    dump_aots("Entry#1");

    if (walk_in) {
        /* IN DIE MENGE: die Zombies stehen um (-800..200, -21600..-19600).
         * Wir schieben den Spieler in 40er-Schritten dorthin und ticken dabei —
         * so laeuft der AOT-Scan ueber das Event-Rechteck (x -3200..1800,
         * z -23550..-17450) und sub04 setzt Flag(3,0xD2). */
        int32_t tx = -800, tz = -20600;
        for (int i = 0; i < 400; i++) {
            int32_t dx = tx - pl->x, dz = tz - pl->z;
            if (dx > 40) dx = 40; if (dx < -40) dx = -40;
            if (dz > 40) dz = 40; if (dz < -40) dz = -40;
            pl->x += dx; pl->z += dz;
            tick_once();
            if (i % 50 == 0)
                printf("[walk %3d] pl=(%ld,%ld) aot6.active=%d Ck(3,0xd2)=%d thr1=%d "
                       "pmode=%d lbox=%d msgq=%d msgd=%d band=%d\n",
                       i, (long)pl->x, (long)pl->z, g_aot.slots[6].active,
                       re15_game_flag_get(3, 0xd2), g_scd.threads[1].active,
                       (int)g_scd.player_mode, (int)g_scd.letterbox_countdown,
                       (int)g_scd.message_query, (int)g_scd.message_display_frames,
                       re15_collision_debug_band());
        }
        printf("[walk] Spieler in der Menge @(%ld,%ld)  Ck(3,0xd2)=%d\n",
               (long)pl->x, (long)pl->z, re15_game_flag_get(3, 0xd2));
        /* Kampf-Ticks: die Zombies wachen auf, packen, beissen. */
        census("Erstbesuch-Kampf", 600);
        dump_roster("Erstbesuch nach Kampf");
        printf("[player] hp=%d st=%u s1=%u mo=%d victim=%d\n",
               (int)pl->hp, pl->state, pl->sub_state_1, (int)pl->motion,
               re15_player_victim_state());
    } else {
        census("Erstbesuch-fern", 300);
    }

    /* --- Raus nach ROOM1130 (main00 Door 0: next=(-1300,0,-13950)?? -> hier egal) --- */
    /* Der Nutzer VERLAESST den Raum LEBEND — HP zurueck auf voll. Sonst liefe der ganze
     * Wiedereintritt unter "Spieler tot", und dann greift BYTE-TRUE der Leichen-Frass-Zweig
     * (`dist<0x5dc && player->hp<0 -> Wort 0xc01`, enemy_ai_common.c:384 = FUN_80102058) —
     * ein Messartefakt, kein Bug. */
    printf("\n== Tuer 0 -> ROOM1130 ==\n");
    pl->hp = 100; pl->state = 1; pl->sub_state_1 = 0; pl->sub_state_2 = 0;
    pl->motion = 0; pl->anim_frame = 0;
    re15_player_cmd_reset();
    door_transition(&r30, 0x1130, 0, 0, 0);
    for (int t = 0; t < 120; t++) tick_once();

    /* --- Zurueck --- */
    printf("== zurueck nach ROOM1140 ==\n");
    door_transition(&r40, 0x1140, -1300, -13950, 2048);
    load_banks(base);
    for (int f = 0; f < 8; f++) tick_once();
    dump_roster("Re-Entry");
    census("Re-Entry fern", 300);
    dump_roster("Re-Entry nach 300");

    /* --- Und jetzt nah ran (Aufwach-Test) --- */
    {
        int32_t tx = g_actors[2].x, tz = g_actors[2].z + 1200;
        trace_reset();
        for (int i = 0; i < 400; i++) {
            int32_t dx = tx - pl->x, dz = tz - pl->z;
            if (dx > 40) dx = 40; if (dx < -40) dx = -40;
            if (dz > 40) dz = 40; if (dz < -40) dz = -40;
            pl->x += dx; pl->z += dz;
            tick_once();
            trace_tick("walk2", i);
        }
        printf("[walk2] Spieler @(%ld,%ld)\n", (long)pl->x, (long)pl->z);
        census("Re-Entry nah", 600);
        dump_roster("Re-Entry nah");
        printf("[player] hp=%d st=%u s1=%u mo=%d victim=%d\n",
               (int)pl->hp, pl->state, pl->sub_state_1, (int)pl->motion,
               re15_player_victim_state());
    }
    return 0;
}
