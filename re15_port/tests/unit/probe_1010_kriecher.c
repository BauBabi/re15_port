/* probe_1010_kriecher.c — MESSUNG (keine Behauptung): was tut der Port in ROOM1010 mit den
 * beiden Sce_em_set-Records, die den Deskriptor 0x81 tragen?
 *
 * NUTZER-REPORT 2026-08-24 (FINDING 2, RE2-KI): "room 1010 die Zombies kriechen im original".
 *
 * DATENLAGE (eigener Byte-Scan von ROOM1010.RDT, sub00 @Datei 0x0930):
 *   Datei 0x09D8  Switch work_vars[0x0A]   (= EINTRITTS-CUT der durchschrittenen Tuer)
 *   Datei 0x09DC   Case 0:
 *   Datei 0x09E2     Sce_em_set slot=0 type=0x10 beh=0x00 pos=( 3750,0, 5000) dirY=3072
 *   Datei 0x09F6     Sce_em_set slot=1 type=0x10 beh=0x00 pos=( 1200,0, 7150) dirY=0
 *   Datei 0x0A0C   Case 4:
 *   Datei 0x0A12     Sce_em_set slot=2 type=0x10 beh=0x81 pos=(  950,0,-1700) dirY=0
 *   Datei 0x0A26     Sce_em_set slot=3 type=0x10 beh=0x81 pos=(  -50,0,-3800) dirY=3584
 *   (kein Default -> jeder andere Eintritts-Cut spawnt in ROOM1010 GAR NICHTS)
 * Die Tuer, die Cut 4 liefert: ROOM1020 main00 @0x1C82 Door_aot_set slot 0 ->
 *   stage 0 / room 0x01 / CUT 4 / Ziel (3650,0,-3950). Die andere Tuer (slot 1 @0x1CA2)
 *   liefert CUT 0 / Ziel (3650,0,6900) = die aufrechte Haelfte.
 *
 * ORIGINAL-KETTE fuer beh 0x81 (STAGE1.BIN, selbst disassembliert):
 *   +0x09 = 0x81            (Sce_em_set FUN_800420a0, Deskriptor -> Entity+0x9)
 *   INIT  @0x80100ca4-b0    Gate `lbu v1,9(v0); andi v0,v1,0x80; beq -> 0x80100e30`
 *   INIT  @0x80100d3c-58    sel {1,3} -> +0x94 = 0x0C
 *   INIT  @0x80100d64-68    sel {1,3} -> +0x05 = 5
 *   Tick  @0x8011f80c[+0x9 & 0xf = 1] = FUN_80101708 = GRID-WURZEL 1 (die Kriech-Maschine)
 *   DECIDE  @0x8011F8E0[5] = 0x8010466C   (Naeherungs-Wecker: dist<0x4B0, arc 0x200, gleiche
 *                                          Etage, player.hit_react==0 -> Wort ((face+1)<<8)|1)
 *   ANIMATE @0x8011F920[5] = 0x80104808   (Clip 0x0C, +0x06=1, +0x8F=7, +0x1D8 &= 0xFFEF)
 *   danach  @0x8011F920[0] = 0x801036DC   (Kriech-Lokomotion: +0x94 = 0x1A, +0x1D7 = 8)
 *
 * PORT-STAND (Behauptung, die diese Sonde PRUEFT): enemy_ai_common.c Zeile ~4180/4206 markiert
 * die Zeilen DECIDE[5]/ANIMATE[5] als "OFFEN: nicht portiert. No-op."  Wenn das stimmt, bleibt
 * der Aktor auf +0x05 = 5 stehen und tut NICHTS.
 *
 * Gemessen wird je Flavor ueber die ersten Sekunden: Typ, +0x04/+0x05/+0x06, +0x09, +0x94,
 * +0x1D7, +0x10E, Position. Zusaetzlich ein Lauf mit dem Spieler DIREKT NEBEN dem Kriecher
 * (dist < 0x4B0), der den Original-Wecker ausloesen wuerde.
 *
 * KEIN add_test — reine Messsonde, ctest-Zahl bleibt unveraendert. */
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

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

static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static int load_bank_re2(uint8_t type)
{
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_re2_n);
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

static uint8_t *s_re15_ems = NULL; static size_t s_re15_n = 0;
static uint8_t  s_blob[0x80000];
static int load_bank_re15(uint8_t type)
{
    if (!s_re15_ems) s_re15_ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_re15_n);
    if (!s_re15_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_re15_ems, s_re15_n, idx, &off, &len) != 0) return 0;
    if (len > sizeof s_blob) return 0;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    memcpy(s_blob, s_re15_ems + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) {
        eb->type = 0; return 0;
    }
    eb->ok = 1; eb->buf = NULL;
    re15_emd_parse_own_bank(s_blob, len, &eb->skel_own, &eb->anim_own);
    eb->own_ok = (eb->anim_own.clip_count > 0);
    eb->loco_ok = (re15_emd_parse_loco_bank(s_blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
    return 1;
}

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

static int is_zombie(const re15_actor_t *e)
{
    return e->active && re15_re2z_owns_type(e->type);
}

static void dump(const char *tag, int frame)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!is_zombie(e)) continue;
        printf("   %s f%-4d slot%-2d type=0x%02X  +04state=%u +05=%2u +06=%2u  +09grid=0x%02X"
               "  +94clip=0x%02X +95=%3u  +1D7sca=%u  +1C4=0x%04X  +10E=0x%04X  hp=%d"
               "  pos=(%6ld,%6ld) ry=%4d  dist=%u\n",
               tag, frame, s, e->type, e->state, e->sub_state_1, e->sub_state_2,
               e->grid_id, e->motion, (unsigned)e->anim_frame, e->sca_mask,
               (unsigned)e->anim_flags, (unsigned)e->re2z_f10e, (int)e->hp,
               (long)e->x, (long)e->z, (int)e->rot_y, (unsigned)e->ai_dist);
    }
}

static void run(re15_ai_flavor_t fl, const char *name, uint8_t entry_cut,
                int32_t px, int32_t pz, int frames)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(fl);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x1010;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz;
    re15_collision_set_band(0);
    re15_inv_load_briefing();

    scd_room_reenter(&s_rdt, pl->x, pl->z, entry_cut);

    if (fl == RE15_AI_FLAVOR_RE2) { (void)load_bank_re2(0x10);  (void)load_bank_re2(0x11); }
    else                          { (void)load_bank_re15(0x10); (void)load_bank_re15(0x11); }

    printf("\n=== %s  Eintritts-Cut=%u  Spieler=(%ld,%ld) ===\n",
           name, (unsigned)entry_cut, (long)px, (long)pz);
    printf("  -- direkt nach dem Spawn (vor jedem Tick):\n");
    dump(name, -1);

    int marks[] = { 0, 1, 2, 5, 30, 120, 300, 600 };
    int mi = 0;
    for (int f = 0; f < frames; f++) {
        frame_step();
        if (mi < (int)(sizeof marks / sizeof marks[0]) && f == marks[mi]) {
            dump(name, f);
            mi++;
        }
    }
    printf("  -- Endstand nach %d Frames:\n", frames);
    dump(name, frames);
}

int main(void)
{
    printf("== ROOM1010: was macht der Port aus den beh-0x81-Records? ==\n");

    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1010.RDT", &n);
    if (!buf) { printf("FAIL: ROOM1010.RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;

    /* A: Kontrolle — Eintritt ueber CUT 0 (ROOM1020-Tuer slot 1). Erwartung: Case 0,
     * zwei AUFRECHTE Zombies (beh 0x00). Zeigt, dass der Switch-Pfad ueberhaupt greift. */
    run(RE15_AI_FLAVOR_RE15, "RE1.5-cut0", 0, 3650, 6900, 300);
    re15_enemy_reset();

    /* B: DER FALL DES NUTZERS — Eintritt ueber CUT 4 (ROOM1020-Tuer slot 0, Ziel
     * (3650,0,-3950)). Erwartung im ORIGINAL: zwei KRIECHER (beh 0x81). */
    run(RE15_AI_FLAVOR_RE15, "RE1.5-cut4", 4, 3650, -3950, 700);
    re15_enemy_reset();
    run(RE15_AI_FLAVOR_RE2,  "RE2-cut4",   4, 3650, -3950, 700);

    /* C: wie B, aber der Spieler steht DIREKT NEBEN dem ersten Kriecher (950,-1700) —
     * dist ~800 < 0x4B0 = 1200, also innerhalb des Original-Weckers DECIDE[5]
     * @0x801046a4 (`sltiu v0,v0,0x4b0`). */
    re15_enemy_reset();
    run(RE15_AI_FLAVOR_RE15, "RE1.5-nah", 4, 950, -900, 700);
    re15_enemy_reset();
    run(RE15_AI_FLAVOR_RE2,  "RE2-nah",   4, 950, -900, 700);

    /* D (2026-08-25): der Kriecher in Slot 3 steht auf (950,-1700) mit ry=0. Im Port ist
     * rot_y=0 = Blick auf +X (actor_locomotion.c), der Spieler aus Fall C (950,-900) liegt
     * also 90 Grad SEITLICH — ausserhalb des Original-Arcs +-0x200 (45 Grad), den DECIDE[5]
     * @0x801046B4 / @0x8010472C verlangt. Fall C zeigt darum korrekt KEIN Aufwachen.
     * Dieser Fall setzt den Spieler VOR den Kriecher (+X, ~1000 Einheiten) und ist damit die
     * eigentliche Gegenprobe fuer den neu portierten Wecker. */
    re15_enemy_reset();
    run(RE15_AI_FLAVOR_RE15, "RE1.5-vorne", 4, 1950, -1700, 700);
    re15_enemy_reset();
    run(RE15_AI_FLAVOR_RE2,  "RE2-vorne",   4, 1950, -1700, 700);

    printf("\n(Sonde: reine Messung, kein PASS/FAIL)\n");
    return 0;
}
