/* probe_load_after_death.c — DIAGNOSE (kein ctest): Nutzer-Report 2026-08-05
 * "Wurde man getoetet und laedt das Spiel, stirbt man irgendwie gleich wieder.
 *  Das Laden klappt nicht richtig."
 *
 * GEMESSENE URSACHE (Live-Repro, analysis/bug_load_after_death.md):
 *   Der Spieler-Kommando-Zustand DEVOUR (Port: g_player_victim = 2, enemy_ai_common.c)
 *   ueberlebt Tod -> Title -> LOAD, weil der CONTINUE-Boot in platform/pc/main.c
 *   re15_enemy_reset() (-> re15_player_victim_reset()) NICHT ruft. Der einzige Caller
 *   ist room_common.c:142 = re15_room_apply_pending (Tuer/Debug-Sprung).
 *   Folge im ersten getickten Frame nach dem Laden:
 *     - re15_player_victim_tick() laeuft weiter in der COLLAPSE-Phase,
 *     - re15_clip_root_motion_abs() setzt die Position ABSOLUT aus anchor_x/z
 *       (nach scd_vm_init/re15_actor_init = 0) -> Spieler steht am Weltursprung,
 *     - bei anim_frame == 0x23 setzt derselbe Tick hp = -1 / state = 7
 *       (byte-true FUN_8010a6f8 @0x8010a7e8/@0x8010a80c) -> sofortiger 2. Tod.
 *
 * ORIGINAL-BELEG (der Reset, den der Port an dieser Stelle nicht hat):
 *   FUN_800314b0 (Spieler-Load) @0x80031518: `sw zero, 0x800aca58`
 *     -> WORD-Null auf das Kommandoregister aca58/aca59/aca5a
 *        (cmd / Variante / Phase = genau g_player_victim + variant + phase).
 *   Einziger Caller: FUN_800396fc (Raumlader) @0x80039788 `jal FUN_800314b0`
 *     -> im Original loescht JEDER Raum-Load das Devour-Kommando.
 *   Dieselbe Funktion setzt @0x80031710-18 `ori v0,0x64 ; sh v0,0x800acaee` (HP=100)
 *   und @0x80031720 `sh zero,0x800acaec` (Status/Gift = 0).
 *
 * Diese Sonde faehrt den Ablauf ENGINE-ONLY nach (kein SDL, kein Fenster):
 *   M1  Devour-Tod herstellen (EM10-Victim-Bank aus CDEMD0.EMS) -> hp<0, victim=2
 *   M2  CONTINUE-Boot exakt wie main.c: scd_vm_init() + Boot-Spawn/hp=100 +
 *       re15_savedata_restore() -> victim-State messen (IST: 2 = BUG)
 *   M3  Victim-FSM weitertickten -> misst den Teleport + den 2. Tod bei Frame 0x23
 *   M4  Gegenprobe: re15_enemy_reset() (= der Raumwechsel-Reset) raeumt auf
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_room.h"
#include "re15_scd.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_savedata.h"
#include "re15_collision.h"

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

/* EM10-Victim-Bank (Typ 0x10 = der ROOM1140-Live-Zombie) aus CDEMD0.EMS laden —
 * live macht das die Plattform in pc_enemy_load. Ohne victim_ok nimmt
 * re15_player_victim_devour() den Fallback-Zweig und setzt g_player_victim gar nicht. */
static uint8_t *s_em10_blob = NULL;
static int load_zombie_victim_bank(void)
{
    re15_enemy_bank_t *eb = re15_enemy_find(0x10);
    if (!eb) eb = re15_enemy_alloc(0x10);
    if (!eb) return 0;
    if (!s_em10_blob) {
        char path[600]; size_t ems_size = 0;
        snprintf(path, sizeof path, "%s/EMD/CDEMD0.EMS", RE15_ASSET_PSX_DIR);
        uint8_t *ems = read_file(path, &ems_size);
        if (!ems) { printf("!! CDEMD0.EMS nicht lesbar\n"); return 0; }
        int idx = re15_ems_index_for_type(0x10);
        size_t off = 0, len = 0;
        if (idx < 0 || re15_ems_get_entry(ems, ems_size, idx, &off, &len) != 0) {
            printf("!! EM10-Blob nicht gefunden\n"); free(ems); return 0;
        }
        s_em10_blob = (uint8_t *)malloc(len + sizeof(size_t));
        memcpy(s_em10_blob, &len, sizeof(size_t));
        memcpy(s_em10_blob + sizeof(size_t), ems + off, len);
        free(ems);
    }
    size_t len; memcpy(&len, s_em10_blob, sizeof(size_t));
    eb->victim_ok = (re15_emd_parse_victim_bank(s_em10_blob + sizeof(size_t), len,
                                                &eb->skel_victim, &eb->anim_victim) == 0);
    eb->ok = 1; eb->buf = NULL;   /* Blob bleibt statisch resident (enemy_reset free()t ihn nicht) */
    printf("EM10 victim bank: ok=%d clips=%d\n", eb->victim_ok, eb->anim_victim.clip_count);
    return eb->victim_ok;
}

int main(void)
{
    int fails = 0;
    printf("== probe_load_after_death ==\n");

    /* ---------- Vorlauf: Spielstand wie ein Phone-Save in ROOM1140 ---------- */
    scd_vm_init();
    g_current_room_id = 0x1140;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0;
    pl->x = -2800; pl->y = 0; pl->z = -19600; pl->rot_y = -96; pl->hp = 100;
    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 0);
    printf("[SAVE] room=%04x pos=(%d,%d,%d) hp=%d\n",
           sd.room, sd.player_x, sd.player_y, sd.player_z, (int)sd.player_hp);

    if (!load_zombie_victim_bank()) { printf("SKIP: keine EM10-Victim-Bank\n"); return 0; }

    /* ---------- M1: Devour-Tod (das, was der Live-Lauf gezeigt hat) ---------- */
    re15_actor_t *z = &g_actors[1];
    z->active = 1; z->type = 0x10; z->state = 1; z->sub_state_1 = 5;   /* (+0x5)-5 = Variante 0 */
    z->x = -3300; z->y = 0; z->z = -19600; z->rot_y = 2047;
    pl->x = -3757; pl->z = -19595; pl->rot_y = 2047;
    re15_player_victim_devour(z);
    printf("[M1] nach devour: victim=%d hp=%d\n", re15_player_victim_state(), (int)pl->hp);
    for (int i = 0; i < 80 && pl->hp >= 0; i++) re15_player_victim_tick();
    printf("[M1] Tod erreicht: victim=%d hp=%d state=%d pos=(%d,%d) af=%d\n",
           re15_player_victim_state(), (int)pl->hp, (int)pl->state,
           pl->x, pl->z, (int)pl->anim_frame);
    if (pl->hp >= 0) { printf("FAIL: Devour-Tod nicht erreicht\n"); return 1; }
    if (re15_player_victim_state() != 2) { printf("FAIL: victim != 2 (COLLAPSE)\n"); return 1; }

    /* ---------- M2: CONTINUE-Boot exakt wie platform/pc/main.c ----------
     * main.c: RDT-Install -> RAUM-INIT-RESET (der Fix) -> pc_load_room_esp -> scd_vm_init()
     * (-> re15_actor_init(): memset ALLER Slots, anchor=0) -> Boot-Spawn + hp=100
     * -> re15_savedata_restore().
     *
     * Der RAUM-INIT-RESET ist das Port-Gegenstueck zu `sw zero,0x800aca58` @0x80031518
     * (FUN_800314b0, gerufen vom Raumlader FUN_800396fc @0x80039788) bzw. `sb zero,0x800aca58`
     * @0x8001cbdc (Karten-Screen-Exit). Er fehlte auf dem Boot-/Lade-Pfad — DAS war der Bug. */
    re15_enemy_reset();        /* Victim-/Crow-/Spawn-Count-Reset + Banks der Vor-Session */
    re15_player_cmd_reset();   /* Knockdown-/Flinch-/Event-Reach-Statics */
    scd_vm_init();
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0;
    pl->x = -7600; pl->y = 0; pl->z = -17600; pl->rot_y = -96;   /* ROOM1140-Boot-Spawn */
    pl->hp = 100;                                                /* main.c:2552 */
    re15_actor_set_motion(pl, 200);
    uint16_t rr = 0;
    re15_savedata_restore(&sd, &rr);
    printf("[M2] nach LOAD: room=%04x pos=(%d,%d) hp=%d motion=%d anchor=(%d,%d) victim=%d\n",
           rr, pl->x, pl->z, (int)pl->hp, (int)pl->motion,
           pl->anchor_x, pl->anchor_z, re15_player_victim_state());
    if (re15_player_victim_state() != 0) {
        printf("BUG BESTAETIGT: der Devour-Kommando-Zustand (victim=%d) hat den LOAD ueberlebt.\n"
               "               Original: FUN_800314b0 @0x80031518 `sw zero,0x800aca58` bei JEDEM\n"
               "               Raum-Load (einziger Caller: Raumlader FUN_800396fc @0x80039788).\n",
               re15_player_victim_state());
        fails++;
    }

    /* ---------- M3: was der erste getickte Frame nach dem Laden tut ---------- */
    int32_t x0 = pl->x, z0 = pl->z;
    int death_frame = -1;
    for (int i = 0; i < 60; i++) {
        re15_player_victim_tick();
        if (i == 0)
            printf("[M3] Tick 1: pos (%d,%d) -> (%d,%d)  motion=%d af=%d\n",
                   x0, z0, pl->x, pl->z, (int)pl->motion, (int)pl->anim_frame);
        if (pl->hp < 0 && death_frame < 0) death_frame = i;
    }
    printf("[M3] nach 60 Ticks: hp=%d state=%d pos=(%d,%d) af=%d | 2. Tod bei Tick %d\n",
           (int)pl->hp, (int)pl->state, pl->x, pl->z, (int)pl->anim_frame, death_frame);
    if (death_frame >= 0)
        printf("BUG BESTAETIGT: Spieler stirbt %d Ticks nach dem Laden erneut "
               "(Devour-Kill @0x8010a7e8 anim_frame==0x23).\n", death_frame);

    /* ---------- M4: derselbe Defekt auf dem NEW-GAME-Pfad ----------
     * re15_gameflow_new_game() (re15_gameflow.c:36-49) ruft NUR re15_wound_reset() —
     * ein NEW GAME direkt nach einem Devour-Tod erbt denselben Kommando-Zustand. */
    {
        extern void re15_gameflow_new_game(int character);
        re15_gameflow_new_game(0);
        re15_enemy_reset();        /* derselbe Raum-Init-Reset — NEW GAME laeuft durch denselben Block */
        re15_player_cmd_reset();
        scd_vm_init();
        pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0;
        pl->x = -26214; pl->y = 0; pl->z = -3861; pl->rot_y = 0; pl->hp = 100;
        re15_actor_set_motion(pl, 200);
        printf("[M4] nach NEW GAME (Boot ROOM1240): victim=%d\n", re15_player_victim_state());
        if (re15_player_victim_state() != 0) {
            printf("BUG BESTAETIGT: auch NEW GAME nach dem Tod erbt den Devour-Zustand.\n");
            fails++;
        }
        int df = -1;
        for (int i = 0; i < 60; i++) { re15_player_victim_tick(); if (pl->hp < 0 && df < 0) df = i; }
        printf("[M4] nach 60 Ticks im frischen Spiel: hp=%d pos=(%d,%d) motion=%d | Tod bei Tick %d\n",
               (int)pl->hp, pl->x, pl->z, (int)pl->motion, df);
    }

    /* ---------- M5: Gegenprobe — der Raumwechsel-Reset raeumt auf ---------- */
    re15_enemy_reset();                       /* room_common.c:142 (re15_room_apply_pending) */
    printf("[M5] nach re15_enemy_reset(): victim=%d  (0 = der Fix-Effekt)\n",
           re15_player_victim_state());
    if (re15_player_victim_state() != 0) { printf("FAIL: enemy_reset raeumt nicht auf\n"); fails++; }

    printf(fails ? "\nERGEBNIS: BUG reproduziert (%d Befunde)\n"
                 : "\nERGEBNIS: kein Befund (Fix aktiv?)\n", fails);
    return 0;   /* Diagnose-Sonde: kein ctest-Fail */
}
