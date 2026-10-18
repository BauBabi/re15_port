/* test_r39_treffer.c — Runde 39 Spur D "treffer" (Dossier analysis/befunde_runde39/D_treffer.md).
 *
 * Nutzer (AUFTRAG.md): (1) "Zombies die halb am aufstehen sind, sind nicht treffbar. Das ist so im Original
 * Resident Evil 2 auf jeden Fall nicht." (2) "Zombies sind ueber den Tisch hinweg nicht treffbar. Das darf so
 * nicht sein" (3) "Das Auto Aim darf nicht gelten fuer Gegner die der Spieler sonst garnicht sehen koennte ..."
 *
 * Mess- und Riegelmodi (Aufruf: test_r39_treffer <modus> [...]):
 *   spawns <raum-hex>            Zensus der Sce_em_set-Spawns eines Raums (Typ, Lage, grid) — reine Messung
 *   mess_aufsteher <eben|tief> [gap]   RE2-KI, ROOM1140: Boden-Aufsteher EXEC[5] P6/P7 (Clip 8/9) unter Dauerfeuer
 *                                (Pistole, Abstand `gap`, Standard 6): je Schuss Clip-Bild, Teile-Maske, HP -> Treffer
 *   mess_fresser <eben|tief> [gap]     RE2-KI, ROOM1140: Fresser-Aufsteher EXEC[8] P3/P4 (Clip 0x15) unter Dauerfeuer
 *   tisch                        Riegel Punkt 2 (echter Spielschritt ROOM1140): Pistole ueber den Konferenztisch [6],
 *                                im freien Gang, Gegenprobe alter Stand (Moebel-Liste aus)
 *   zielhilfe                    Riegel Punkt 3: ROOM1100 (L-Flur) Zielhilfe ueberspringt den Zombie hinter der Wand;
 *                                ROOM1140 Zielhilfe ueber den Tisch; Gegenproben alter Stand
 *   moebel                       Riegel der Moebel-Liste gegen die RDT-Bytes (Art 3, Geometrie, keine Huelle, Elza-Paar,
 *                                Obermenge der Runde-36-Tabelle k_r36_moebel)
 *   aufsteher                    Riegel Punkt 1 (Nachbesserung 1, Abnahme-0 M1): dieselben Laeufe wie mess_aufsteher /
 *                                mess_fresser, aber mit CHECKs. (a) Sturz-Aufsteher EXEC[5] EBEN: Maske vor Clip-Bild 55
 *                                == 1 (nur Beine @0x801032B8-FC), 0 Treffer vor Bild 55 (EBEN-Zeile [2,0,0] = Rumpf,
 *                                DAT_800a6db4+9+3, Maskentor @0x800413C4-D8), erster Treffer in EXEC[5] >= Bild 55 bei
 *                                Maske 3 (@0x801036D0-F0) ueber FUN_80107A78; (b) TIEF: Treffer vor Bild 55 bei Maske 1
 *                                (DOWN-Zeile [1,2,4] = DAT_800a6db4+6, Beine zuerst); (c) Fresser-Aufsteher EXEC[8]
 *                                EBEN: Treffer vor Bild 55 bei Maske 3; (d) Gegenprobe: dieselbe Pose wie (a) bei
 *                                Bild 11, Maske per Sonde auf 3 -> EBEN trifft (nur die Maske sperrt, nicht Fenster/Box).
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
#include "re15_ems.h"
#include "re2_ems.h"
#include "re15_treffer_r39.h"
#include "re15_schuss_r36.h"
#include "re15_zombieki_r36.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_player_aim_elevation(void);
extern int  re15_re2z_last_hit_handler(void);
extern int  re15_player_aim_target(int32_t radius, int32_t *tx, int32_t *tz);   /* re15_damage.c (FUN_8003703c) */

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_rdt_buf = NULL;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static int load_bank_re2(uint8_t type)
{
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &s_re2_n);
    if (!s_re2_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_re2_ems, s_re2_n, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return 1;
    }
    eb->type = 0; return 0;
}

static int rdt_load(unsigned room)
{
    char p[256]; size_t n = 0;
    snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/STAGE%u/ROOM%04X.RDT", (room >> 12) & 0xfu, room);
    if (s_rdt_buf) { free(s_rdt_buf); s_rdt_buf = NULL; }
    s_rdt_buf = slurp(p, &n);
    if (!s_rdt_buf) { printf("FAIL: %s nicht lesbar\n", p); fails++; return 0; }
    if (re15_rdt_parse(s_rdt_buf, n, &g_room_rdt) != 0) { printf("FAIL: RDT-Parse %s\n", p); fails++; return 0; }
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;
    g_current_room_id = room;
    return 1;
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

static int32_t s_px, s_pz; static int16_t s_pry;
static int s_pin_rot = 1;   /* 0 = Blick NICHT pinnen (die Zielhilfe dreht den Spieler selbst, FUN_8001a8f8) */
static void pin_player(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->hp = 100; pl->x = s_px; pl->z = s_pz; pl->y = 0;
    if (s_pin_rot) pl->rot_y = s_pry;
}
static void face(int32_t tx, int32_t tz)
{
    s_pry = (int16_t)(((int)re15_atan2_q12(tz - s_pz, tx - s_px) - 0x400) & 0x0fff);
}

static int is_zombie(uint8_t t)
{
    return t == 0x10 || t == 0x11 || t == 0x12 || t == 0x13 || t == 0x16 || t == 0x18;
}

/* Raum laden, SCD 120 Takte (Spawns), Spieler + Pistole aufsetzen, RE2-Baenke der Zombie-Typen laden.
 * Rueckgabe: Zahl der aktiven Gegner. */
static int bringup(unsigned room, re15_ai_flavor_t flavor)
{
    if (!rdt_load(room)) return -1;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(flavor);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x2545f491u);
    re15_schuss_r36_sichtlinie_reset();
    re15_treffer_r39_zaehler_reset();
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active) continue;
        n++;
        if (flavor == RE15_AI_FLAVOR_RE2 && is_zombie(e->type)) load_bank_re2(e->type);
    }
    re15_player_set_aim_clip_len(12);
    re15_player_set_equipped_weapon(3);                       /* Pistole = RE1.5-Waffe 3 -> RE2-Zeile 3 */
    { int es = re15_inv_equipped_slot();
      if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
    return n;
}

/* ersten aufrechten Zombie behalten, alle anderen Gegner deaktivieren; Rueckgabe Slot */
static int keep_one_zombie(void)
{
    int slot = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active) continue;
        if (slot < 0 && (e->type == 0x10 || e->type == 0x11)) slot = s;
    }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    return slot;
}

static int frame_slot(const re15_actor_t *e)
{
    int fc = re15_actor_clip_len(e);
    if (fc <= 0) return (int)e->anim_frame;
    return (int)(e->anim_frame % (uint32_t)fc);
}

static void aim(int *f, int down)
{
    uint16_t cur = (uint16_t)(RE15_PAD_BIT_R1 | (down ? RE15_PAD_BIT_DOWN : 0));
    for (int k = 0; k < 40 && !re15_player_aim_ready(); k++, (*f)++) { pin_player(); frame(cur, 0); }
    CHECK(re15_player_aim_ready() && re15_player_aim_elevation() == (down ? -1 : 0),
          "Zielen nicht bereit (ready=%d elev=%d)", re15_player_aim_ready(), re15_player_aim_elevation());
}

/* ------------------------------------------------------------------------------------------------- spawns */
static int mode_spawns(unsigned room)
{
    int n = bringup(room, RE15_AI_FLAVOR_RE2);
    printf("ROOM%04X: %d aktive Gegner\n", room, n);
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *e = &g_actors[s];
        if (!e->active) continue;
        printf("  slot %2d typ 0x%02x grid 0x%02x st=%u/%u/%u @(%d,%d,y=%d) rot=%d hp=%d floor=%u\n",
               s, e->type, e->grid_id, e->state, e->sub_state_1, e->sub_state_2, e->x, e->z, e->y,
               (int)e->rot_y, (int)e->hp, (unsigned)e->floor);
    }
    return 0;
}

/* --------------------------------------------------------------------------------- Aufsteher-Messung */
/* Den Zombie WACH machen und mit dem Flinch-Wort 0x501 (@0x801050A4-AC) in EXEC[5] schicken (r16-Harness). */
static void wake_and_knock(re15_actor_t *e)
{
    for (int k = 0; k < 3; k++) { pin_player(); frame(0, 0); }
    e->state = 1; e->grid_id = 0; e->sub_state_1 = 1; e->sub_state_2 = 0; e->hp = 2000;
    e->re2z_f10e &= (uint16_t)~0x4001u;
    re15_ai_set_state_word(e, 0x501);
}
static int tick_until(re15_actor_t *e, int *f, unsigned s1, unsigned s2, int max)
{
    for (int k = 0; k < max; k++, (*f)++) {
        pin_player(); frame(0, 0);
        if (e->state == 1 && e->sub_state_1 == s1 && e->sub_state_2 == s2) return 1;
    }
    return 0;
}

typedef struct {
    int shots, hits, first_hit_slot, last_miss_slot, stood_f;
    int hits_lt55, miss_ge55, shots_lt55, shots_ge55;      /* nur Schuesse, die in EXEC[exec] fielen */
    /* Nachbesserung 1 (Riegel aufsteher), ebenfalls nur Schuesse in EXEC[exec]: */
    int      shots_exec, hits_exec;
    int      first_hit_exec_slot;                           /* Clip-Bild des ersten Treffers in EXEC[exec], -1 = keiner */
    unsigned parts_lt55_or;                                 /* OR der Teile-Masken (word0>>26&7) bei Schuessen vor Bild 55 */
    unsigned parts_first_hit;                               /* Teile-Maske beim ersten Treffer in EXEC[exec] */
    int      hnd_first_hit;                                 /* re15_re2z_last_hit_handler() beim ersten Treffer, -1 = keiner */
} dauer_t;

/* Dauerfeuer, solange der Zombie in EXEC[`exec`] ist (+ `tail` Bilder danach). Je Schuss eine Zeile. */
static dauer_t dauerfeuer(re15_actor_t *e, int *f, int down, int gap, unsigned exec, int tail)
{
    dauer_t r; memset(&r, 0, sizeof r); r.first_hit_slot = -1; r.last_miss_slot = -1; r.stood_f = -1;
    r.first_hit_exec_slot = -1; r.hnd_first_hit = -1;
    uint16_t hold = (uint16_t)(RE15_PAD_BIT_R1 | (down ? RE15_PAD_BIT_DOWN : 0));
    int hp_last = e->hp, k = 0, after = 0;
    for (; k < 4000; k++, (*f)++) {
        pin_player();
        int in_exec = (e->state == 1 && e->sub_state_1 == exec);
        if (!in_exec) { if (r.stood_f < 0) r.stood_f = *f; if (++after > tail) break; }
        int fire = (k % gap == 0);
        unsigned s2b = e->sub_state_2; int slotb = frame_slot(e); unsigned mob = (unsigned)e->motion;
        unsigned partsb = (unsigned)e->re2z_parts;
        frame((uint16_t)(hold | (fire ? RE15_PAD_BIT_SQUARE : 0)), (uint16_t)(fire ? RE15_PAD_BIT_SQUARE : 0));
        int hit = (e->hp < hp_last);
        if (fire) {
            r.shots++;
            if (hit) { r.hits++; if (r.first_hit_slot < 0) r.first_hit_slot = slotb; }
            else r.last_miss_slot = slotb;
            if (in_exec) {
                r.shots_exec++;
                if (hit) {
                    r.hits_exec++;
                    if (r.first_hit_exec_slot < 0) {
                        r.first_hit_exec_slot = slotb; r.parts_first_hit = partsb;
                        r.hnd_first_hit = re15_re2z_last_hit_handler();
                    }
                }
                if (slotb < 55) { r.shots_lt55++; r.parts_lt55_or |= partsb; if (hit) r.hits_lt55++; }
                else            { r.shots_ge55++; if (!hit) r.miss_ge55++; }
            }
            printf("  %s f%-4d st=%u/%u/%u mo=0x%02x bild=%3d maske=%u elev=%+d hp %d->%d %s hnd=%d\n",
                   hit ? "HIT " : "miss", *f, e->state, e->sub_state_1, s2b, mob, slotb, partsb,
                   re15_player_aim_elevation(), hp_last, (int)e->hp, in_exec ? "" : "(steht)",
                   re15_re2z_last_hit_handler());
        }
        hp_last = e->hp;
        if (e->hp < 0) break;
    }
    printf("  SUMME %s: %d Schuesse, %d Treffer; in EXEC[%u] Bild<55: %d/%d Treffer, Bild>=55: %d/%d Fehl; "
           "erster Treffer Bild %d, aufgestanden f%d\n", down ? "TIEF" : "EBEN", r.shots, r.hits, exec,
           r.hits_lt55, r.shots_lt55, r.miss_ge55, r.shots_ge55, r.first_hit_slot, r.stood_f);
    printf("  SUMME EXEC[%u]: %d Schuesse, %d Treffer; Masken vor Bild 55 OR=%u; erster Treffer in EXEC Bild %d "
           "(Maske %u, hnd %d)\n", exec, r.shots_exec, r.hits_exec, r.parts_lt55_or, r.first_hit_exec_slot,
           r.parts_first_hit, r.hnd_first_hit);
    return r;
}

/* Sturz-Aufsteher: ROOM1140 (RE2-KI), einen Zombie behalten, Spieler 1500 davor, per 0x501 in EXEC[5] schicken, bis P6
 * ticken, zielen (EBEN/TIEF), Dauerfeuer. Rueckgabe 1 = gemessen (*out), 0 = Aufbau gescheitert (FAIL gezaehlt). */
static int lauf_aufsteher(int down, int gap, dauer_t *out)
{
    printf("== aufsteher %s gap=%d (RE2-KI, ROOM1140, Pistole, Sturz-Aufsteher EXEC[5]) ==\n", down ? "TIEF" : "EBEN", gap);
    if (bringup(0x1140, RE15_AI_FLAVOR_RE2) < 0) return 0;
    int slot = keep_one_zombie(); CHECK(slot >= 0, "kein Zombie"); if (slot < 0) return 0;
    re15_actor_t *e = &g_actors[slot];
    s_px = e->x - 1500; s_pz = e->z; face(e->x, e->z); pin_player();
    int f = 0;
    wake_and_knock(e);
    int p6 = tick_until(e, &f, 5, 6, 3000);
    CHECK(p6, "P6 nicht erreicht"); if (!p6) return 0;
    aim(&f, down);
    printf("  PRE f%d st=%u/%u/%u mo=0x%02x bild=%d maske=%u hp=%d dist=%d\n", f, e->state, e->sub_state_1,
           e->sub_state_2, (unsigned)e->motion, frame_slot(e), (unsigned)e->re2z_parts, (int)e->hp,
           (int)re15_enemy_player_dist(e, &g_actors[RE15_ACTOR_SLOT_PLAYER]));
    *out = dauerfeuer(e, &f, down, gap, 5u, 40);
    return 1;
}

/* Fresser-Aufsteher: der ROOM1140-Fresser (EXEC[8] P3/P4, Clip 0x15) unter Dauerfeuer. */
static int lauf_fresser(int down, int gap, dauer_t *out)
{
    printf("== fresser %s gap=%d (RE2-KI, ROOM1140, Fresser-Aufsteher EXEC[8]) ==\n", down ? "TIEF" : "EBEN", gap);
    if (bringup(0x1140, RE15_AI_FLAVOR_RE2) < 0) return 0;
    int slot = keep_one_zombie(); CHECK(slot >= 0, "kein Zombie"); if (slot < 0) return 0;
    re15_actor_t *e = &g_actors[slot];
    s_px = e->x - 1500; s_pz = e->z; face(e->x, e->z); pin_player();
    int f = 0;
    for (int k = 0; k < 3; k++, f++) { pin_player(); frame(0, 0); }
    aim(&f, down);
    printf("  PRE f%d st=%u/%u/%u mo=0x%02x bild=%d maske=%u hp=%d 1d3=0x%02x\n", f, e->state, e->sub_state_1,
           e->sub_state_2, (unsigned)e->motion, frame_slot(e), (unsigned)e->re2z_parts, (int)e->hp,
           (unsigned)e->re2z_self1d3);
    *out = dauerfeuer(e, &f, down, gap, 8u, 40);
    return 1;
}

static int mode_mess_aufsteher(int down, int gap)
{
    dauer_t r; return lauf_aufsteher(down, gap, &r) ? 0 : 1;
}

static int mode_mess_fresser(int down, int gap)
{
    dauer_t r; return lauf_fresser(down, gap, &r) ? 0 : 1;
}

/* ------------------------------------------------------------------------------------------------ tisch */
/* Gegner stehend wach machen (0x101) und je Bild an (zx,zz) festhalten; Spieler an (px,pz) mit Blick auf ihn;
 * `shots` Schuesse EBEN im Abstand 20. Rueckgabe Treffer; *verworfen = Sichtlinien-Verwuerfe, *zelle = letzte Zelle. */
static int schiessen(re15_actor_t *e, int32_t px, int32_t pz, int32_t zx, int32_t zz, int shots,
                     int *verworfen, int *zelle)
{
    int f = 0, hits = 0, v0 = re15_schuss_r36_sichtlinie_verworfen();
    const int gap = 45;                                    /* laenger als die Treffer-Reaktion (+0x93-Latch) */
    s_px = px; s_pz = pz; face(zx, zz); s_pin_rot = 1; pin_player();
    e->state = 1; e->grid_id = 0; e->sub_state_1 = 1; e->sub_state_2 = 0; e->hp = 2000;
    e->re2z_f10e &= (uint16_t)~0x4001u;
    re15_ai_set_state_word(e, 0x101);
    e->x = zx; e->z = zz;
    re15_player_aim_reset();
    for (int k = 0; k < 3; k++, f++) { pin_player(); e->x = zx; e->z = zz; frame(0, 0); }
    s_pin_rot = 0;                                         /* ab hier dreht die Zielhilfe den Spieler (echter Weg) */
    aim(&f, 0);
    int hp_last = e->hp; *zelle = -1;
    for (int k = 0; k < shots * gap; k++, f++) {
        pin_player(); e->x = zx; e->z = zz; e->y = 0;
        int fire = (k % gap == 0);
        if (fire) { re15_ai_set_state_word(e, 0x101); e->hit_react = 0; }   /* jeder Schuss trifft einen Gehenden */
        frame((uint16_t)(RE15_PAD_BIT_R1 | (fire ? RE15_PAD_BIT_SQUARE : 0)), (uint16_t)(fire ? RE15_PAD_BIT_SQUARE : 0));
        if (fire) {
            int lz = re15_treffer_r39_los_letzte_zelle();
            if (lz >= 0 && g_room_rdt.sca_rgn[0] > 0) *zelle = lz % g_room_rdt.sca_rgn[0];   /* Partitionen wiederholen die Zellen */
            if (e->hp < hp_last) hits++;
        }
        hp_last = e->hp;
    }
    s_pin_rot = 1;
    *verworfen = re15_schuss_r36_sichtlinie_verworfen() - v0;
    return hits;
}

static int mode_tisch(void)
{
    printf("== tisch: ROOM1140 Konferenztisch [6] 12100x6100 @(-4650,-17450), RE2-KI, Pistole EBEN ==\n");
    if (bringup(0x1140, RE15_AI_FLAVOR_RE2) < 0) return 1;
    int slot = keep_one_zombie(); CHECK(slot >= 0, "kein Zombie"); if (slot < 0) return 1;
    re15_actor_t *e = &g_actors[slot];
    int v, z, h;
    /* A: ueber den Tisch — Spieler (1400,-10300) suedlich, Zombie (1400,-19000) noerdlich der Platte */
    re15_treffer_r39_moebel_aus(0);
    h = schiessen(e, 1400, -10300, 1400, -19000, 5, &v, &z);
    printf("  A ueber den Tisch, Moebel-Liste AN : %d/5 Treffer, Sichtlinie verworfen %d, letzte Zelle %d, Moebel uebersprungen %u\n",
           h, v, z, re15_treffer_r39_zaehler(2));
    CHECK(h == 5, "A: ueber den Tisch nur %d/5 Treffer (Nutzer: 'ueber den Tisch hinweg nicht treffbar. Das darf so nicht sein')", h);
    CHECK(v == 0, "A: Sichtlinie verwarf %d Schuesse ueber den Tisch", v);
    /* A': Gegenprobe alter Stand (jede Art-3-Zelle sperrt, FUN_8003dcc4 ohne Ausnahme) */
    re15_treffer_r39_moebel_aus(1);
    h = schiessen(e, 1400, -10300, 1400, -19000, 5, &v, &z);
    printf("  A' ueber den Tisch, Moebel-Liste AUS: %d/5 Treffer, Sichtlinie verworfen %d, letzte Zelle %d\n", h, v, z);
    CHECK(h == 0 && v == 5 && z == 6, "A': alter Stand muss ueber den Tisch 0 Treffer / 5 Verwuerfe an Zelle 6 liefern (h=%d v=%d z=%d)", h, v, z);
    re15_treffer_r39_moebel_aus(0);
    /* B: freier Gang westlich des Tischs — Spieler (-6700,-12000), Zombie (-6700,-20000) */
    h = schiessen(e, -6700, -12000, -6700, -20000, 5, &v, &z);
    printf("  B freier Gang: %d/5 Treffer, verworfen %d, letzte Zelle %d\n", h, v, z);
    CHECK(h == 5 && v == 0, "B: freier Gang %d/5 Treffer, verworfen %d", h, v);
    /* C: Wand — Spieler im Raum (-2500,-10300), Zombie in der Tuernische (-7200,-3000): Strahl kreuzt den Huellenblock [3]
     *    (x -5750..16700, z -7800..1750, Art 3, kein Moebel) -> gesperrt, mit und ohne Liste */
    h = schiessen(e, -2500, -10300, -7200, -3000, 5, &v, &z);
    printf("  C hinter dem Huellenblock [3]: %d/5 Treffer, verworfen %d, letzte Zelle %d\n", h, v, z);
    CHECK(h == 0 && v == 5 && z == 3, "C: Wand muss sperren (h=%d v=%d zelle=%d, soll 0/5/3)", h, v, z);
    return 0;
}

/* -------------------------------------------------------------------------------------------- zielhilfe */
static void fake_zombie(int slot, int32_t x, int32_t z)
{
    re15_actor_t *e = &g_actors[slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x10; e->state = 1; e->sub_state_1 = 1; e->hp = 100;
    e->hit_radius_min = 400; e->x = x; e->z = z; e->y = 0; e->floor = 0;
}
static int ziel_slot(int32_t radius)
{
    int32_t tx = 0, tz = 0;
    if (!re15_player_aim_target(radius, &tx, &tz)) return -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].x == tx && g_actors[s].z == tz) return s;
    return -2;
}

static int mode_zielhilfe(void)
{
    printf("== zielhilfe ==\n");
    /* ROOM1100, L-Flur: Spieler (-14000,-13000) am Suedende des N-S-Gangs (x -15600..-12600). Zombie A (-18500,-24500) im
     * Nordwest-Gang hinter der Wandzelle [2] (x -17600..-15600, z -23200..-12100), Abstand 12349; Zombie B (-14000,-26000)
     * am Nordende des N-S-Gangs, frei sichtbar, Abstand 13000 (weiter weg als A). */
    if (!rdt_load(0x1100)) return 1;
    re15_actor_init();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = -14000; pl->z = -13000; pl->y = 0; pl->floor = 0;
    fake_zombie(2, -18500, -24500);
    fake_zombie(3, -14000, -26000);
    re15_treffer_r39_zielhilfe_aus(0); re15_treffer_r39_moebel_aus(0);
    {   int bl = re15_treffer_r39_los_blockiert(pl, &g_actors[2]);
        int lz = re15_treffer_r39_los_letzte_zelle(); int lzi = (lz >= 0) ? lz % g_room_rdt.sca_rgn[0] : -1;
        CHECK(bl == 1 && (lzi == 1 || lzi == 2), "1100: Strahl zu A muss an der Wand [1]/[2] sperren (zelle=%d)", lzi);
        CHECK(lz < 0 || !re15_treffer_r39_zelle_moebel(0x1100, &g_room_rdt.sca[lz]), "1100: sperrende Zelle ist als Moebel gelistet"); }
    CHECK(re15_treffer_r39_los_blockiert(pl, &g_actors[3]) == 0, "1100: Strahl zu B muss frei sein (zelle=%d)",
          re15_treffer_r39_los_letzte_zelle());
    int zs = ziel_slot(30000);
    printf("  1100 Zielhilfe NEU: Ziel = Slot %d (A=2 hinter der Wand, naeher; B=3 sichtbar, weiter)\n", zs);
    CHECK(zs == 3, "1100: Zielhilfe muss den sichtbaren B (3) nehmen, nahm %d (Nutzer: 'nicht ... um die Ecke ... Wand davor')", zs);
    g_actors[3].active = 0;
    zs = ziel_slot(30000);
    printf("  1100 nur der verdeckte A: Ziel = %d (soll -1 = kein Ziel)\n", zs);
    CHECK(zs == -1, "1100: nur der verdeckte A darf KEIN Ziel sein (RE2 FUN_80042144 @0x800422a8-b0), war %d", zs);
    g_actors[3].active = 1;
    re15_treffer_r39_zielhilfe_aus(1);
    zs = ziel_slot(30000);
    printf("  1100 Zielhilfe ALT (ohne Sichtlinie, FUN_8003703c): Ziel = %d (soll 2 = der naechste)\n", zs);
    CHECK(zs == 2, "1100: Gegenprobe alter Stand muss den naechsten A (2) nehmen, nahm %d", zs);
    re15_treffer_r39_zielhilfe_aus(0);
    /* ROOM1140: ueber den Tisch wird angevisiert (Moebel durchlaessig), hinter dem Huellenblock nicht */
    if (!rdt_load(0x1140)) return 1;
    re15_actor_init();
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 1400; pl->z = -10300; pl->y = 0; pl->floor = 0;
    fake_zombie(2, 1400, -19000);
    zs = ziel_slot(30000);
    printf("  1140 ueber den Tisch: Ziel = %d (soll 2)\n", zs);
    CHECK(zs == 2, "1140: Zombie ueber den Tisch muss Ziel sein, war %d", zs);
    re15_treffer_r39_moebel_aus(1);
    zs = ziel_slot(30000);
    printf("  1140 ueber den Tisch, Moebel-Liste AUS: Ziel = %d (soll -1)\n", zs);
    CHECK(zs == -1, "1140: ohne Moebel-Liste sperrt der Tisch die Zielhilfe (war %d)", zs);
    re15_treffer_r39_moebel_aus(0);
    pl->x = -2500; pl->z = -10300; g_actors[2].x = -7200; g_actors[2].z = -3000;
    zs = ziel_slot(30000);
    printf("  1140 hinter dem Huellenblock [3]: Ziel = %d (soll -1)\n", zs);
    CHECK(zs == -1, "1140: Zombie hinter dem Huellenblock darf kein Ziel sein (war %d)", zs);
    return 0;
}

/* ----------------------------------------------------------------------------------------------- moebel */
static int mode_moebel(void)
{
    int n = 0;
    const re15_treffer_r39_moebel_t *t = re15_treffer_r39_moebel_tabelle(&n);
    printf("== moebel: %d Eintraege ==\n", n);
    CHECK(n > 0, "Moebel-Tabelle leer");
    unsigned last_room = 0;
    for (int i = 0; i < n; i++) {
        const re15_treffer_r39_moebel_t *m = &t[i];
        if (m->room != last_room) { if (!rdt_load(m->room)) return 1; last_room = m->room; }
        int found = -1;
        for (int c = 0; c < g_room_rdt.sca_rgn[0] && c < g_room_rdt.sca_count; c++) {
            const re15_sca_entry_t *s = &g_room_rdt.sca[c];
            if (s->x == m->x && s->z == m->z && s->width == m->w && s->density == m->d) { found = c; break; }
        }
        CHECK(found >= 0, "ROOM%04X [%d] %s: keine Zelle mit dieser Geometrie", m->room, m->idx, m->was);
        if (found < 0) continue;
        const re15_sca_entry_t *s = &g_room_rdt.sca[found];
        CHECK((s->floor & 0x0f) == 3, "ROOM%04X [%d] %s: Art-Nibble %u != 3", m->room, m->idx, m->was, s->floor & 0x0f);
        CHECK(s->type == m->type && (s->floor >> 4) == m->band, "ROOM%04X [%d]: Form/Band weichen ab", m->room, m->idx);
        CHECK(re15_treffer_r39_zelle_moebel(m->room, s) == 1, "ROOM%04X [%d]: Laufzeit-Zuordnung findet die Zelle nicht", m->room, m->idx);
        /* keine Huelle: die Zelle beruehrt die Band-Bounding-Box nicht (Toleranz 150 wie r39_treffer_inseln_bild.py) */
        int32_t x0 = 0x7fffffff, z0 = 0x7fffffff, x1 = -0x7fffffff, z1 = -0x7fffffff;
        for (int c = 0; c < g_room_rdt.sca_rgn[0] && c < g_room_rdt.sca_count; c++) {
            const re15_sca_entry_t *q = &g_room_rdt.sca[c];
            if ((q->floor >> 4) != m->band) continue;
            if (q->x < x0) x0 = q->x; if (q->z < z0) z0 = q->z;
            if (q->x + (int32_t)q->width > x1) x1 = q->x + (int32_t)q->width;
            if (q->z + (int32_t)q->density > z1) z1 = q->z + (int32_t)q->density;
        }
        int huelle = (s->x <= x0 + 150) || (s->z <= z0 + 150) || (s->x + (int32_t)s->width >= x1 - 150) ||
                     (s->z + (int32_t)s->density >= z1 - 150);
        CHECK(!huelle, "ROOM%04X [%d] %s: beruehrt die Huelle (Wand), darf kein Moebel sein", m->room, m->idx, m->was);
        /* Leon-Raum: die Elza-Variante muss (bei byte-gleicher Zelle) als eigener Eintrag folgen */
        if ((m->room & 1u) == 0) {
            int ok = (i + 1 < n) && t[i + 1].room == (uint16_t)(m->room + 1) && t[i + 1].x == m->x && t[i + 1].z == m->z;
            char p[256]; size_t nn = 0;
            snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/STAGE%u/ROOM%04X.RDT", (m->room >> 12) & 0xfu, m->room + 1);
            uint8_t *eb = slurp(p, &nn);
            if (eb) { CHECK(ok, "ROOM%04X [%d]: Elza-Variante ROOM%04X fehlt in der Liste", m->room, m->idx, m->room + 1); free(eb); }
        }
    }
    /* Obermenge der Runde-36-Tabelle (zombieki_r36.c k_r36_moebel: Zellen mit belegter Oberkante) */
    static const unsigned k_r36_rooms[] = { 0x1010, 0x1220 };
    for (unsigned r = 0; r < sizeof k_r36_rooms / sizeof k_r36_rooms[0]; r++) {
        if (!rdt_load(k_r36_rooms[r])) return 1;
        for (int c = 0; c < g_room_rdt.sca_rgn[0] && c < g_room_rdt.sca_count; c++) {
            const re15_sca_entry_t *s = &g_room_rdt.sca[c];
            int grund = 0;
            if (re15_r36_zelle_oberkante(s, &grund) == 0) continue;
            CHECK(re15_treffer_r39_zelle_moebel(k_r36_rooms[r], s) == 1,
                  "ROOM%04X Zelle %d (k_r36_moebel, Oberkante belegt) fehlt in der r39-Moebelliste", k_r36_rooms[r], c);
        }
    }
    return 0;
}

/* --------------------------------------------------------------------------------------------- aufsteher */
#define HND_GETUP 7   /* re15_re2z_last_hit_handler(): FUN_80107A78 (re2z_getup_hurt), Weiche +0x21A&0x10 @0x80105014-38 */

/* Riegel Punkt 1 (Nachbesserung 1, Abnahme-0 M1). Pinnt das RE2-Modell B5 am Port-Spielschritt (RE2-KI, ROOM1140):
 *  (a) Sturz-Aufsteher EXEC[5] mit EBEN: Teile-Maske vor Clip-Bild 55 = 1 (nur Beine: `lui a2,0xf3ff` @0x801032B8 /
 *      `and` @0x801032DC / `lui v0,0x400 / or` @0x801032E8-EC / `sw v1,0(s2)` @0x801032FC), 0 Treffer vor Bild 55 (EBEN-
 *      Zeile 3 des Pistolen-Records = [2,0,0] Rumpf, DAT_800a6db4+9+3; Maskentor `lw v0,0(s2)` @0x800413C4 / `srl 26`
 *      @0x800413CC / `andi 7` @0x800413D4 / `beq` @0x800413D8), erster Treffer in EXEC[5] erst ab Bild 55 (Rumpf-Bit
 *      `lbu v1,333(s2)` / `addiu v0,zero,55` / `bne` @0x801036D0-D8, `lui v1,0xc00 / or / sw` @0x801036DC-F0) bei
 *      Maske 3 ueber FUN_80107A78.
 *  (b) TIEF: Treffer vor Bild 55 bei Maske 1 (DOWN-Record @0x800A6618 = 00 04 02 01 -> Zeile 6 = [1,2,4] DAT_800a6db4+6,
 *      Beine zuerst).
 *  (c) Fresser-Aufsteher EXEC[8] mit EBEN: Treffer vor Bild 55 bei Maske 3 (EXEC[8] setzt nie "nur Beine").
 * Die Zahlen (Schuesse/Treffer/Bilder) stehen als Messung im Dossier; die CHECKs sind Schwellen, keine Bildnummern. */
/* (d) Gegenprobe "die Maske ist die einzige Sperre": Aufbau wie lauf_aufsteher EBEN bis zum ersten Schuss in P7
 * (Clip-Bild 11, Maske 1); unmittelbar vor dem Schuss wird die Teile-Maske per Sonde auf 3 gesetzt (das tut das Original
 * erst bei Bild 55 @0x801036D0-F0). Trifft EBEN dann, sind Hoehenfenster und XZ-Box (FUN_800410CC) in dieser Pose erfuellt
 * und allein die Maske (Zeile [2,0,0] gegen Maske 1) sperrte in (a). Rueckgabe 1 = gemessen. */
static int lauf_gegenprobe_maske(int *bild, int *hit, int *hnd)
{
    printf("== gegenprobe EBEN: Pose wie (a), Maske vor dem ersten Schuss per Sonde auf 3 ==\n");
    *bild = -1; *hit = 0; *hnd = -1;
    if (bringup(0x1140, RE15_AI_FLAVOR_RE2) < 0) return 0;
    int slot = keep_one_zombie(); CHECK(slot >= 0, "kein Zombie"); if (slot < 0) return 0;
    re15_actor_t *e = &g_actors[slot];
    s_px = e->x - 1500; s_pz = e->z; face(e->x, e->z); pin_player();
    int f = 0;
    wake_and_knock(e);
    int p6 = tick_until(e, &f, 5, 6, 3000);
    CHECK(p6, "P6 nicht erreicht"); if (!p6) return 0;
    aim(&f, 0);
    *bild = frame_slot(e);
    int ok = (e->state == 1 && e->sub_state_1 == 5 && *bild < 55 && e->re2z_parts == 1u);
    CHECK(ok, "Gegenprobe: Ausgangslage nicht wie (a) (st=%u/%u/%u bild=%d maske=%u)", e->state, e->sub_state_1,
          e->sub_state_2, *bild, (unsigned)e->re2z_parts);
    if (!ok) return 0;
    int hp0 = e->hp;
    e->re2z_parts = 3u;                                            /* SONDE: Rumpf-Bit vorzeitig (Original: Bild 55) */
    pin_player();
    frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE), RE15_PAD_BIT_SQUARE);
    f++;
    *hit = (e->hp < hp0); *hnd = re15_re2z_last_hit_handler();
    printf("  %s f%-4d st=%u/%u/%u mo=0x%02x bild=%3d maske=3(Sonde) elev=%+d hp %d->%d hnd=%d\n", *hit ? "HIT " : "miss", f,
           e->state, e->sub_state_1, e->sub_state_2, (unsigned)e->motion, *bild, re15_player_aim_elevation(), hp0, (int)e->hp, *hnd);
    return 1;
}

static int mode_aufsteher(void)
{
    printf("== aufsteher (Riegel Punkt 1: RE2-KI, ROOM1140, Spielschritt) ==\n");
    dauer_t a, b, c; memset(&a, 0, sizeof a); memset(&b, 0, sizeof b); memset(&c, 0, sizeof c);
    a.first_hit_exec_slot = b.first_hit_exec_slot = c.first_hit_exec_slot = -1;
    /* (a) Sturz-Aufsteher, Zielhoehe EBEN */
    if (lauf_aufsteher(0, 6, &a)) {
        CHECK(a.shots_lt55 >= 4, "(a) EBEN: nur %d Schuesse vor Clip-Bild 55 in EXEC[5] (Messung braucht >= 4)", a.shots_lt55);
        CHECK(a.parts_lt55_or == 1u, "(a) EBEN: Teile-Maske vor Bild 55 OR=%u, soll 1 = nur Beine (@0x801032B8-FC)", a.parts_lt55_or);
        CHECK(a.hits_lt55 == 0, "(a) EBEN traf den Sturz-Aufsteher %d/%d-mal vor Clip-Bild 55 - RE2: Zeile [2,0,0] (Rumpf) gegen "
              "Maske 1 (nur Beine) = kein Teil (DAT_800a6db4+9+3, Tor @0x800413C4-D8)", a.hits_lt55, a.shots_lt55);
        CHECK(a.shots_ge55 >= 1, "(a) EBEN: kein Schuss ab Bild 55 in EXEC[5] (%d)", a.shots_ge55);
        CHECK(a.first_hit_exec_slot >= 55, "(a) EBEN: erster Treffer in EXEC[5] bei Clip-Bild %d, soll >= 55 (Rumpf-Bit "
              "@0x801036D0-F0; -1 = gar keiner)", a.first_hit_exec_slot);
        CHECK(a.parts_first_hit == 3u, "(a) EBEN: Maske beim ersten Treffer %u, soll 3 (Beine+Rumpf)", a.parts_first_hit);
        CHECK(a.hnd_first_hit == HND_GETUP, "(a) EBEN: erster Treffer nahm Handler %d, soll %d = FUN_80107A78 (@0x80105014-38)",
              a.hnd_first_hit, HND_GETUP);
    }
    /* (b) Sturz-Aufsteher, Zielhoehe TIEF (D-Pad unten gehalten) */
    if (lauf_aufsteher(1, 6, &b)) {
        CHECK(b.shots_lt55 >= 1, "(b) TIEF: kein Schuss vor Bild 55 in EXEC[5]");
        CHECK(b.hits_lt55 >= 1, "(b) TIEF traf den Sturz-Aufsteher vor Bild 55 nicht (%d/%d) - DOWN-Zeile [1,2,4] "
              "(DAT_800a6db4+6, Record @0x800A6618) trifft die Beine (Maske 1)", b.hits_lt55, b.shots_lt55);
        CHECK(b.first_hit_exec_slot >= 0 && b.first_hit_exec_slot < 55, "(b) TIEF: erster Treffer in EXEC[5] bei Bild %d, soll < 55",
              b.first_hit_exec_slot);
        CHECK(b.parts_first_hit == 1u, "(b) TIEF: Maske beim ersten Treffer %u, soll 1 (nur Beine)", b.parts_first_hit);
        CHECK(b.hnd_first_hit == HND_GETUP, "(b) TIEF: erster Treffer nahm Handler %d, soll %d = FUN_80107A78", b.hnd_first_hit, HND_GETUP);
    }
    /* (c) Fresser-Aufsteher EXEC[8], Zielhoehe EBEN */
    if (lauf_fresser(0, 6, &c)) {
        CHECK(c.first_hit_exec_slot >= 0 && c.first_hit_exec_slot < 55, "(c) Fresser EBEN: erster Treffer in EXEC[8] bei Bild %d, "
              "soll vor 55 (Maske 3, kein 'nur Beine' in EXEC[8])", c.first_hit_exec_slot);
        CHECK(c.parts_first_hit == 3u, "(c) Fresser EBEN: Maske beim ersten Treffer %u, soll 3", c.parts_first_hit);
        CHECK(c.hnd_first_hit == HND_GETUP, "(c) Fresser EBEN: erster Treffer nahm Handler %d, soll %d = FUN_80107A78", c.hnd_first_hit, HND_GETUP);
    }
    /* (d) Gegenprobe: nur die Maske sperrt */
    int gb = -1, gh = 0, gn = -1;
    if (lauf_gegenprobe_maske(&gb, &gh, &gn)) {
        CHECK(gh, "(d) Gegenprobe: mit Maske 3 traf EBEN den Aufsteher bei Bild %d NICHT - dann sperrt nicht die Maske, sondern "
              "Hoehenfenster/Box (FUN_800410CC), und (a) belegt das RE2-Modell nicht", gb);
        CHECK(gn == HND_GETUP, "(d) Gegenprobe: Treffer nahm Handler %d, soll %d = FUN_80107A78", gn, HND_GETUP);
    }
    printf("  RIEGEL: (a) EBEN %d/%d Treffer vor Bild 55 (Masken OR=%u), erster Treffer in EXEC[5] Bild %d (Maske %u, hnd %d); "
           "(b) TIEF %d/%d vor 55, erster Bild %d (Maske %u); (c) Fresser EBEN erster Treffer in EXEC[8] Bild %d (Maske %u); "
           "(d) Maske-3-Sonde bei Bild %d: %s (hnd %d)\n",
           a.hits_lt55, a.shots_lt55, a.parts_lt55_or, a.first_hit_exec_slot, a.parts_first_hit, a.hnd_first_hit,
           b.hits_lt55, b.shots_lt55, b.first_hit_exec_slot, b.parts_first_hit, c.first_hit_exec_slot, c.parts_first_hit,
           gb, gh ? "Treffer" : "kein Treffer", gn);
    return 0;
}

int main(int argc, char **argv)
{
    const char *m = (argc > 1) ? argv[1] : "";
    int rc = 0;
    if (!strcmp(m, "spawns") && argc > 2)      rc = mode_spawns((unsigned)strtoul(argv[2], NULL, 16));
    else if (!strcmp(m, "mess_aufsteher"))     rc = mode_mess_aufsteher(argc > 2 && !strcmp(argv[2], "tief"), argc > 3 ? atoi(argv[3]) : 6);
    else if (!strcmp(m, "mess_fresser"))       rc = mode_mess_fresser(argc > 2 && !strcmp(argv[2], "tief"), argc > 3 ? atoi(argv[3]) : 6);
    else if (!strcmp(m, "tisch"))              rc = mode_tisch();
    else if (!strcmp(m, "zielhilfe"))          rc = mode_zielhilfe();
    else if (!strcmp(m, "moebel"))             rc = mode_moebel();
    else if (!strcmp(m, "aufsteher"))          rc = mode_aufsteher();
    else { printf("Modus? spawns|mess_aufsteher|mess_fresser|tisch|zielhilfe|moebel|aufsteher\n"); return 2; }
    if (rc) return rc;
    if (fails) { printf("\n%d FAIL(s)\n", fails); return 1; }
    printf("\nPASS test_r39_treffer %s\n", m);
    return 0;
}
