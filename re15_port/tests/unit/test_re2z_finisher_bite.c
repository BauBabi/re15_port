/* test_re2z_finisher_bite.c — PIN `unit_re2z_finisher_bite`.
 *
 * NUTZER-REPORT 2026-08-24 (RE2-KI-Modus): "... als auch der finisher biss sound des zombies
 * fehlt." (Der Biss WAEHREND der Fress-/Kollaps-Sequenz.)
 *
 * SOLL — RE1.5 FUN_80102bd8 (DEVOUR-FINISH-Animate, Dispatch @0x8011F890[+0x5=5/6]),
 * selbst disassembliert aus info/Re1.5/PSX/BIN/STAGE1.BIN (RAW @0x80100000):
 *     ; Eintritt (+0x6 == 0)
 *     80102c8c: jal   0x800453d0      ; snd1 SE 4 = Devour-Einstiegs-SE
 *     80102c90: ori   a0,zero,0x4     ; (Delay-Slot)
 *     ; pro Tick
 *     80102c94: lui   v0,0x800b
 *     80102c98: lw    v0,-14460(v0)   ; v0 = g_entity(cur) = DER ZOMBIE
 *     80102ca0: lbu   v1,149(v0)      ; v1 = entity+0x95 = ZOMBIE-Anim-Frame (NICHT player+0x95)
 *     80102ca4: ori   v0,zero,0x28    ; <== FRAME-GATE 40 — die einzige zitierbare Konstante
 *     80102ca8: bne   v1,v0,0x80102cb8
 *     80102cb0: jal   0x800453d0      ; ** snd1 (Bank 3) SE 3 = DER FINISHER-BISS **
 *     80102cb4: ori   a0,zero,0x3     ; (Delay-Slot)
 * Identisches Gate im Leichen-Fress-Executor: `ori v0,zero,0x28` @0x801045F0 /
 * `bne` @0x801045F4 / `jal 0x800453d0` @0x801045FC / `ori a0,zero,0x3` @0x80104600.
 * SE 3 == "Biss" ist datenseitig belegt: der Frame-Wort-SFX-Dekoder FUN_8001b38c
 * (`lw v0,360(v0)` @0x8001b3a4, `srl s0,v0,22` @0x8001b3b4, `jal 0x800453d0` @0x8001b3cc)
 * traegt in der RE1.5-Zombiebank EM10 genau auf den BISS-ANGRIFFS-Clips 0x27 (f18, f47) und
 * 0x28 (f21) das Bit SE 3; die Devour-Clips 9/0x0A tragen KEINE SFX-Bits.
 *
 * RE2 hat an dieser Stelle KEINEN eigenen Biss: vollstaendiger jal-Scan 0x0C016F5B
 * (`jal 0x8005bd6c` = RE2-ENEMSE) ueber info/re2leon/COMMON/BIN/EMOVL10_S0.BIN — EXEC[6]
 * @0x80103970 hat GENAU EINEN Treffer, @0x801039F0 mit `addiu a0,zero,11` @0x801039E8 /
 * `addiu a0,zero,10` @0x801039EC = MOAN; die RE2-Spieler-Kollaps-Maschine
 * 0x8010B464-0x8010B7D0 enthaelt NULL ENEMSE-Aufrufe. Unter dem SOUND-MANDAT des Nutzers
 * ("Entscheidungen aus RE2, Sounds/Praesentation aus RE1.5") gilt darum die RE1.5-Seite.
 *
 * IST vor dem Fix (gemessen, echter game_step, ROOM1140, geladene RE2-Baenke): zwischen dem
 * Kollaps-Eintritt und dem ersten snd1-SE lagen 148 Frames (~5 s) voellige Stille — der erste
 * SE 3 fiel erst im Fress-Loop EXEC[8].
 *
 * GEPRUEFT WIRD (RE2-Flavor, ROOM1140, echter game_step):
 *   (A) Waehrend EXEC[6] (Zombie +0x5 == 6, +0x6 != 0) faellt snd1 SE 3 — mindestens einmal.
 *   (B) Er faellt auf GENAU dem Frame-Slot 0x28 = 40 des laufenden Zombie-Clips (@0x80102CA4).
 *   (C) Er faellt GENAU EINMAL pro Finisher (Gleichheits-Tor auf monotonem Zaehler; der
 *       Port-Advancer koennte einen Slot ueber mehrere Ticks halten -> Ein-Schuss-Latch).
 *   (D) Der ERSTE snd1-SE nach dem Kollaps-Eintritt IST dieser Biss — vor dem Fix war es
 *       erst der Chomp des Fress-Loops EXEC[8], 148 Frames spaeter.
 *   (E) REGRESSIONSWACHE: der RE1.5-Zweig bleibt unveraendert (dort feuert derselbe SE 3 auf
 *       demselben Gate 0x28, enemy_ai_common.c re15_enemy_ai_live_devour).
 * (A)-(D) werden rot, sobald der neue Aufruf in re2z_exec_six P1 entfaellt.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
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
#include "re15_tim.h"
#include "re15_skeleton.h"
#include "re2_ems.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void     re15_player_aim_reset(void);
extern void     re15_player_set_aim_clip_len(int fc);
extern uint32_t re15_re2_rand(void);
extern void     re15_re2z_onesave_reset(void);
extern void     re15_player_victim_reset(void);
extern int      g_test_room_se_log[];
extern int      g_test_room_se_n;

static int s_fail = 0;
#define CHECK(cond, ...) do { \
    if (cond) { printf("  OK   " __VA_ARGS__); printf("\n"); } \
    else      { printf("  FAIL " __VA_ARGS__); printf("\n"); s_fail++; } \
} while (0)

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_ems = NULL; static size_t s_ems_n = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int load_bank2(uint8_t type)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems_n);
    if (!s_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_ems, s_ems_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return 1; }
    eb->type = 0; return 0;
}

/* RE1.5-Baenke aus dem eigenen EMS-Container (fuer den RE1.5-Gegenprobe-Lauf) — inkl.
 * Opfer-Bank (dir[3]), ohne die der Opfer-Latch gar nicht erst einsteigt. */
static uint8_t s_blob15[0x80000];
static int load_bank15(uint8_t type)
{
    static uint8_t *ems15 = NULL; static size_t ems15_n = 0;
    if (!ems15) ems15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &ems15_n);
    if (!ems15) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(ems15, ems15_n, idx, &off, &len) != 0) return 0;
    if (len > sizeof s_blob15) return 0;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    memcpy(s_blob15, ems15 + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_blob15, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) {
        eb->type = 0; return 0;
    }
    eb->ok = 1; eb->buf = NULL;
    re15_emd_parse_own_bank(s_blob15, len, &eb->skel_own, &eb->anim_own);
    eb->own_ok  = (eb->anim_own.clip_count > 0);
    eb->loco_ok = (re15_emd_parse_loco_bank(s_blob15, len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(s_blob15, len,
                                                &eb->skel_victim, &eb->anim_victim) == 0);
    return 1;
}

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
}

/* aktueller Loop-Slot des laufenden Zombie-Clips (Original +0x14D; der Port-Zaehler ist
 * monoton, der Renderer nimmt slot = frame % frame_count). */
static int frame_slot(const re15_actor_t *e)
{
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || !b->ok || (int)e->motion >= b->anim.clip_count) return (int)e->anim_frame;
    int fc = b->anim.clips[e->motion].frame_count;
    if (fc <= 0) return (int)e->anim_frame;
    return (int)(e->anim_frame % (uint32_t)fc);
}

/* Ergebnis eines Finisher-Laufs. */
typedef struct {
    int reached;        /* EXEC[6]/Devour ueberhaupt erreicht */
    int bites;          /* SE-3-Aufrufe waehrend des Finishers */
    int slot_first;     /* Frame-Slot des ERSTEN Bisses (-1 = keiner) */
    int slot_bad;       /* Bisse auf einem anderen Slot als 0x28 */
    int f_first_bite;   /* Tick des ersten Bisses IM Finisher */
    int f_collapse;     /* Tick des Kollaps-Eintritts (Opfer-Zustand -> 2), -1 = nie */
    int f_first_se3;    /* Tick des ERSTEN snd1-SE-3 ab dem Kollaps-Eintritt, egal aus
                         * welchem Zustand (vor dem Fix war das der Fress-Loop EXEC[8]) */
} finisher_t;

static int run_finisher(int re2, int seed, int budget, finisher_t *out)
{
    memset(out, 0, sizeof *out);
    out->slot_first = -1; out->f_first_bite = -1;
    out->f_collapse = -1; out->f_first_se3 = -1;

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_player_victim_reset();
    re15_re2z_onesave_reset();
    re15_damage_seed_rng(0x0badf00du + (uint32_t)seed * 0x9E3779B9u);
    g_current_room_id = 0x1140;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    if (re2) { if (!load_bank2(0x10)) return 0; load_bank2(0x11); load_bank2(0x16); }
    else     { if (!load_bank15(0x10)) return 0; load_bank15(0x11); load_bank15(0x16); }

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();
    for (int i = 0; i < seed * 13; i++) (void)re15_re2_rand();

    int zs = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type <= 0x16) {
            int nib = g_actors[s].grid_id & 0x0f;
            if (zs < 0 && nib != 7 && nib != 8) zs = s;
        }
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (s != zs && g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type <= 0x16)
            g_actors[s].active = 0;
    if (zs < 0) return 0;
    re15_actor_t *z = &g_actors[zs];
    {   int ang = (seed * 4096) / 4;
        pl->x = z->x + (int32_t)((900 * (int64_t)re15_cos_q12(ang)) >> 12);
        pl->z = z->z - (int32_t)((900 * (int64_t)re15_sin_q12(ang)) >> 12);
        pl->y = z->y;
        pl->rot_y = (int16_t)((seed * 337 + 700) & 0xfff);
    }

    int dead = 0;
    for (int f = 0; f < budget; f++) {
        pl->floor = z->floor;
        /* Der Biss (Schaden aus @0x80100014) toetet beim naechsten Biss-Frame -> Finisher. */
        if (!dead && re15_player_victim_state() != 2 && pl->hp > 0) pl->hp = 0;

        int n0 = g_test_room_se_n;
        frame_step();
        if (re15_player_victim_state() == 2) dead = 1;

        /* RE2: EXEC[6] = +0x5 == 6 && +0x6 != 0. RE1.5: Devour = +0x5 in {5,6} && +0x6 != 0. */
        int in_finish = (z->sub_state_2 != 0) &&
                        (re2 ? (z->sub_state_1 == 6)
                             : (z->sub_state_1 == 5 || z->sub_state_1 == 6));
        if (re15_player_victim_state() == 2 && out->f_collapse < 0) out->f_collapse = f;
        if (out->f_collapse >= 0 && out->f_first_se3 < 0)
            for (int i = n0; i < g_test_room_se_n; i++)
                if (g_test_room_se_log[i] == 3) { out->f_first_se3 = f; break; }
        if (!in_finish) continue;
        out->reached = 1;
        for (int i = n0; i < g_test_room_se_n; i++) {
            if (g_test_room_se_log[i] != 3) continue;
            out->bites++;
            int slot = frame_slot(z);
            if (out->slot_first < 0) { out->slot_first = slot; out->f_first_bite = f; }
            if (slot != 0x28) out->slot_bad++;
        }
    }
    return 1;
}

int main(void)
{
    printf("== PIN: RE2-Zombie-FINISHER-BISS — snd1 SE 3 auf Zombie-Frame 0x28 @0x80102CB0 ==\n");

    size_t sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", &sz);
    if (!buf || re15_rdt_parse(buf, sz, &s_rdt) != 0) {
        printf("SKIP: ROOM1140.RDT fehlt\n"); return 77;
    }

    /* ---- RE2 (der Report-Fall) --------------------------------------------------------- */
    int runs = 0, reached = 0, no_bite = 0, slot_bad = 0, multi = 0, late = 0, setup = 0;
    int scored = 0;
    for (int seed = 0; seed < 4; seed++) {
        finisher_t r;
        if (!run_finisher(1, seed, 900, &r)) { setup++; continue; }
        runs++;
        if (!r.reached) continue;
        reached++;
        printf("       RE2 seed %d: Bisse=%d ersterSlot=%d ersterTick=%d | Kollaps f%d, "
               "erster snd1-SE-3 danach f%d\n",
               seed, r.bites, r.slot_first, r.f_first_bite, r.f_collapse, r.f_first_se3);
        if (r.bites == 0) { no_bite++; continue; }
        scored++;
        if (r.slot_first != 0x28 || r.slot_bad) slot_bad++;
        if (r.bites != 1) multi++;
        if (r.f_first_se3 != r.f_first_bite) late++;
    }
    if (setup == 4) { printf("SKIP: RE2-Bank EM010 / ROOM1140-Spawns fehlen\n"); return 77; }

    CHECK(reached > 0, "RE2-Finisher (EXEC[6]) erreicht: %d von %d Laeufen", reached, runs);
    CHECK(reached > 0 && no_bite == 0,
          "(A) JEDER RE2-Finisher spielt snd1 SE 3: %d von %d stumm (vor dem Fix: ALLE — "
          "148 Frames Stille) [jal 0x800453d0 @0x80102CB0, a0=3 @0x80102CB4]",
          no_bite, reached);
    /* ⛔ (B)/(C)/(D) MUESSEN AN DIE ZAHL DER AUSGEWERTETEN LAEUFE GEKOPPELT SEIN
     * (Skeptiker-Befund 2026-08-25): `if (r.bites == 0) { no_bite++; continue; }` ueberspringt
     * die drei Zaehler, also blieben sie bei 0 und meldeten GRUEN — auf der Baseline stand
     * woertlich "(D) ... (0 Laeufe)" als OK, direkt unter der eigenen Protokollzeile
     * "Bisse=0". Drei Pins, die nur dann etwas pruefen, wenn ohnehin schon (A) haelt, sind
     * keine Wache. Deshalb: `scored` mitzaehlen und verlangen, dass ueberhaupt bewertet wurde. */
    CHECK(scored > 0,
          "(B-D) kein einziger Lauf mit Biss ausgewertet (%d erreicht, %d stumm) — die "
          "folgenden Pins waeren vakuant", reached, no_bite);
    CHECK(scored > 0 && slot_bad == 0,
          "(B) Der Biss faellt auf Zombie-Frame-Slot 0x28 = 40 (%d Abweichungen in %d "
          "bewerteten Laeufen) [lbu v1,149 @0x80102CA0 / ori v0,zero,0x28 @0x80102CA4 / "
          "bne @0x80102CA8]", slot_bad, scored);
    CHECK(scored > 0 && multi == 0,
          "(C) GENAU EIN Biss pro Finisher (%d von %d Laeufen mit Mehrfach-Feuer) — "
          "Gleichheits-Tor auf monotonem Zaehler, im Port ueber das Ein-Schuss-Latch "
          "re2z_sfx_slot", multi, scored);
    CHECK(scored > 0 && late == 0,
          "(D) Der ERSTE snd1-SE nach dem Kollaps-Eintritt IST der Finisher-Biss (%d von %d "
          "Laeufen, in denen der erste Ton erst spaeter faellt) — vor dem Fix war es der Chomp "
          "des Fress-Loops EXEC[8], 148 Frames spaeter", late, scored);

    /* ---- (E) RE1.5-Regressionswache ---------------------------------------------------- */
    {
        int r15_reached = 0, r15_bite = 0, r15_slot_bad = 0;
        for (int seed = 0; seed < 3; seed++) {
            finisher_t r;
            if (!run_finisher(0, seed, 900, &r)) continue;
            if (!r.reached) continue;
            r15_reached++;
            if (r.bites > 0) r15_bite++;
            if (r.bites > 0 && (r.slot_first != 0x28 || r.slot_bad)) r15_slot_bad++;
            printf("       RE1.5 seed %d: Bisse=%d ersterSlot=%d\n",
                   seed, r.bites, r.slot_first);
        }
        CHECK(r15_reached == 0 || (r15_bite == r15_reached && r15_slot_bad == 0),
              "(E) RE1.5-Zweig unveraendert: %d von %d Finishern mit SE 3 auf Slot 0x28 "
              "[enemy_ai_common.c re15_enemy_ai_live_devour, dasselbe Gate @0x80102CA4]",
              r15_bite, r15_reached);
    }

    re15_player_victim_reset();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    printf(s_fail ? "\nFAIL: %d Pruefungen rot\n" : "\nOK: alle Pruefungen gruen\n", s_fail);
    return s_fail ? 1 : 0;
}
