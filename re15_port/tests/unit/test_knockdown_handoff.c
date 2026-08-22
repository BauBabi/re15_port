/* test_knockdown_handoff.c — PINS fuer den Ablauf SCHUSS -> TAUMEL -> STURZ.
 *
 * NUTZER-REPORT 2026-08-17 (nach v0.2.4): "Beim Schiessen und Zurueckfallen auf einen Zombie
 * wurde die Animation drin noch einmal wiederholt und erst dann ging die Animation weiter und
 * er ist gefallen."
 *
 * Gemessen (probe_stagger_fall_1140, A/B gegen v0.2.2 = 3a488891): die Frame-Folge des
 * Sturzes selbst ist in beiden Staenden IDENTISCH (Clip 0x0B, 55 Frames, kf 274..301, jeder
 * Slot genau einmal). EIN echter Defekt kam dabei heraus — der POISE-BREAK-UEBERGABETICK.
 *
 * ORIGINAL (selbst disassembliert, STAGE1.BIN roh @0x80100000):
 *   Der Steh-Stagger posiert BANK 0:
 *     80105d38  addu a2,zero,zero      ; Richtung VORWAERTS
 *     80105d3c  lw   a0,132(v0)        ; +0x84  = BANK 0 (Skelett)
 *     80105d40  lw   a1,364(v0)        ; +0x16c = BANK 0 (Anim)
 *     80105d44  jal  0x8001f314        ; (Phase 3 identisch @0x80105ddc-e8)
 *   anim_set legt den Zeiger auf DAS GERADE POSIERTE Frame-Wort in +0x168 ab:
 *     8001f36c  sw   a2,360(t0)
 *   Der Router-Ausgang beim Poise-Bruch schreibt NUR State-Bytes und ruft KEIN anim_set:
 *     80105b24  lh   v0,476(v0)        ; +0x1dc
 *     80105b2c  bgez v0,0x80105b6c
 *     80105b34-40  lbu +0x9 ; andi 0x80 ; bne     (nicht am Boden)
 *     80105b48  sb   v0,4(v1)          ; +0x4 = 1
 *     80105b58  sb   v0,5(v1)          ; +0x5 = 0x11
 *     80105b68  sb   zero,6(v1)        ; +0x6 = 0
 *   Erst der Knockdown-Entry im NAECHSTEN Tick stellt auf BANK 1 um:
 *     80105190-94  ori v0,0xb ; sb v0,148(v1)   ; +0x94 = 0x0B (Sturz-Clip)
 *     801051a4     sb  zero,149(v0)             ; +0x95 = 0
 *     801051b4     sb  v0(=1),6(v1)             ; +0x6 = 1
 *     80105240-48  lw a0,368 / lw a1,372 / jal 0x8001f314   ; BANK 1, a2=0 @0x8010523c
 *   => Auf dem Uebergabetick posiert das Original UNVERAENDERT BANK 0 (+0x168 zeigt noch
 *      dorthin). Der Port leitet die Bank pro Frame aus (State, +0x5) her und kippte dort auf
 *      die Aktions-Bank, obwohl +0x94 noch der Loco-Walk-Clip (2..5) ist.
 *   GEMESSEN (v0.2.4, probe_stagger_fall_1140 engage):
 *      t=43 s1=0x02      LOCO Clip2 kf= 93
 *      t=44 s1=0x11 s2=0 ACT  Clip2 kf= 46    <<< ein Frame FREMDE Animation
 *      t=45 s1=0x11 s2=1 ACT  Clip0x0B kf=274 (Sturz)
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_player.h"      /* re15_actors_anim_advance */
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_damage.h"
#include "re15_anim_select.h"
#include "re15_ai_flavor.h"   /* Default ist seit 2026-08-22 RE2 — dieser PIN misst RE1.5 */

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  ok: " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long s = ftell(f); fseek(f, 0, SEEK_SET);
    if (s <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)s);
    if (b && fread(b, 1, (size_t)s, f) != (size_t)s) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)s; return b;
}

static uint8_t s_blob[0x80000];
static int load_em10(void)
{
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/EMD/CDEMD0.EMS", RE15_XSTR(RE15_ASSETS_PATH));
    uint8_t *ems = slurp(p, &n); if (!ems) return -1;
    int idx = re15_ems_index_for_type(0x10);
    size_t off = 0, len = 0; int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= sizeof s_blob) {
        re15_enemy_bank_t *eb = re15_enemy_alloc(0x10);
        if (eb) {
            memcpy(s_blob, ems + off, len);
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(s_blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok = (re15_emd_parse_loco_bank(s_blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
                eb->own_ok  = (re15_emd_parse_own_bank(s_blob, len, &eb->skel_own, &eb->anim_own) == 0);
                ok = eb->loco_ok;
            }
        }
    }
    free(ems);
    return ok ? 0 : -1;
}

/* Render-Sicht wie platform/pc/main.c: anim_select + Loco-Override. */
static int render_kf(const re15_actor_t *a, int *out_loco, int *out_fc, int *out_slot)
{
    re15_anim_banks_t banks; memset(&banks, 0, sizeof banks);
    re15_anim_view_t av;
    re15_actor_anim_select(a, 0, &banks, &av);
    const re15_emd_animation_t *an = av.anim;
    const re15_emd_skeleton_t  *sk = av.skel;
    int co = av.clip_override, loco = 0;
    if (re15_actor_uses_loco_bank(a)) {
        re15_enemy_bank_t *lb = re15_enemy_find(a->type);
        if (lb && lb->loco_ok && (int)a->motion < lb->anim_loco.clip_count) {
            sk = &lb->skel_loco; an = &lb->anim_loco; co = (int)a->motion; loco = 1;
        }
    }
    *out_loco = loco;
    if (!an || an->clip_count <= 0) { *out_fc = 0; *out_slot = -1; return -1; }
    int ci = (co >= 0) ? co : (int)a->motion;
    if (ci < 0) ci = 0;
    if (ci >= an->clip_count) ci = ci % an->clip_count;
    *out_fc = an->clips[ci].frame_count;
    if (*out_fc <= 0) { *out_slot = -1; return -1; }
    uint32_t cur = (uint32_t)a->anim_frame;
    *out_slot = (co >= 0) ? (int)(cur % (uint32_t)*out_fc)
                          : (int)((cur >= (uint32_t)*out_fc) ? (uint32_t)(*out_fc - 1) : cur);
    return re15_compute_actor_kf(an, sk, a, co, cur);
}

/* ---- PIN 1: Bank auf dem POISE-BREAK-UEBERGABETICK ---------------------------------- */
static int pin_handoff_bank(void)
{
    re15_actor_t a; memset(&a, 0, sizeof a);
    a.type = 0x10; a.active = 1;
    re15_enemy_bank_t *b = re15_enemy_find(0x10);
    int loco_fc = b->anim_loco.clips[2].frame_count;
    int act_fc  = b->anim.clips[2].frame_count;
    CHECK(loco_fc != act_fc, "Bank-Diskriminator vorhanden: LOCO[2]=%d != ACT[2]=%d",
          loco_fc, act_fc);

    /* Der Router-Zustand (@0x80105b48-68): +0x4=1, +0x5=0x11, +0x6=0, +0x9&0x80 noch CLEAR
     * (das Bit setzt erst der Knockdown-Entry). +0x94 traegt noch den Walk-Clip 2..5. */
    for (int gr = 0; gr < 2; gr++) {
        a.state = 1; a.sub_state_1 = 0x11; a.sub_state_2 = 0;
        a.grid_id = gr ? 0x02 : 0x00; a.motion = 2;
        CHECK(re15_actor_uses_loco_bank(&a) == 1,
              "Uebergabetick grid 0x%02x -> BANK 0 (Stagger posierte @0x80105d3c-44, "
              "Router @0x80105b48-68 ruft kein anim_set)", a.grid_id);
        CHECK(re15_actor_clip_len(&a) == loco_fc,
              "Uebergabetick: Clip-Uhr = LOCO-Laenge %d", loco_fc);
    }

    /* NEGATIV-DISKRIMINATOR 1: sobald der Knockdown-Entry lief (+0x6=1 @0x801051b4 und
     * +0x9|=0x80), posiert das Original BANK 1 (@0x80105240-48) -> ACT. */
    a.state = 1; a.sub_state_1 = 0x11; a.sub_state_2 = 1; a.grid_id = 0x82; a.motion = 0x0b;
    CHECK(re15_actor_uses_loco_bank(&a) == 0,
          "Knockdown-Playout (+0x6=1, +0x9&0x80) -> BANK 1 (@0x80105240-48)");
    /* NEGATIV-DISKRIMINATOR 2: der DOWNED-Flinch-Weg in den Get-up (@0x801069a0 setzt +0x6=4,
     * +0x9&0x80 gesetzt) darf NICHT auf die Loco-Bank kippen. */
    a.sub_state_2 = 4; a.grid_id = 0x82; a.motion = 0x12;
    CHECK(re15_actor_uses_loco_bank(&a) == 0,
          "Downed-Flinch -> Get-up (+0x6=4, +0x9&0x80) bleibt BANK 1");
    /* NEGATIV-DISKRIMINATOR 3: +0x6=0 aber bereits am Boden -> der Router kann diesen
     * Zustand nicht erzeugen (@0x80105b34-40 gated auf +0x9&0x80 == 0). */
    a.sub_state_2 = 0; a.grid_id = 0x82; a.motion = 2;
    CHECK(re15_actor_uses_loco_bank(&a) == 0,
          "+0x6=0 MIT +0x9&0x80 -> BANK 1 (Router-Gate @0x80105b34-40)");
    return fails;
}

/* ---- PIN 2: die VOLLSTAENDIGE Sturz-Frame-Folge (keine Wiederholung) ------------------ */
static int pin_fall_sequence(void)
{
    re15_actor_init();
    re15_damage_seed_rng(0x2026u);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->z = 3000; pl->rot_y = 0;

    re15_actor_t *z = &g_actors[1];
    memset(z, 0, sizeof *z);
    z->active = 1; z->type = 0x10; z->hp = 4000; z->flags = 1; z->em_flag_id = 0xFF;
    z->state = 1; z->sub_state_1 = 0x11; z->sub_state_2 = 0;   /* Router-Ausgang @0x80105b48-68 */
    z->grid_id = 0x00; z->motion = 2; z->anim_frame = 6;       /* +0x94 noch der Walk-Clip */
    z->anim_flags = 0x04; z->hit_stun = -3;                    /* Poise gebrochen */

    re15_enemy_bank_t *b = re15_enemy_find(0x10);
    int fall_fc = b->anim.clips[0x0b].frame_count;

    /* Tick 0 = der Uebergabetick: Pose MUSS noch der Walk-Clip aus BANK 0 sein. */
    int loco, fc, slot;
    int kf0 = render_kf(z, &loco, &fc, &slot);
    CHECK(loco == 1 && fc == b->anim_loco.clips[2].frame_count && slot == 6,
          "Uebergabetick posiert LOCO Clip2 slot 6 (fc=%d, kf=%d) — kein Bank-Sprung", fc, kf0);

    /* Danach: Knockdown-Maschine + globaler Advancer, exakt die Reihenfolge aus
     * re15_game_step (anim_advance VOR run_all). */
    static uint8_t seen[256];
    memset(seen, 0, sizeof seen);
    int prev_slot = -1, backjumps = 0, ticks = 0, ended = 0;
    for (int t = 0; t < 200; t++) {
        re15_enemy_ai_live_tick(1);
        if (z->state != 1 || z->sub_state_1 != 0x11) break;
        /* Render NACH dem AI-Tick (Reihenfolge aus main.c) — der Tick, in dem +0x6 auf 2 geht,
         * posiert noch den letzten Sturz-Frame (Phase [2] @0x80105278 fasst +0x94/+0x95 nicht an). */
        (void)render_kf(z, &loco, &fc, &slot);
        if (z->motion == 0x0b && slot >= 0 && slot < 256) {
            if (seen[slot]) backjumps++;                        /* Slot ZWEIMAL = Wiederholung */
            seen[slot] = 1;
            if (prev_slot >= 0 && slot < prev_slot) backjumps++;
            prev_slot = slot;
            ticks++;
        }
        if (z->sub_state_2 >= 2) { ended = 1; break; }          /* Sturz durch -> Liege-Timer */
        re15_actors_anim_advance();
    }
    CHECK(ended, "Sturz endet regulaer im Liege-Zweig (+0x6 -> 2, @0x80105278)");
    CHECK(backjumps == 0, "KEINE Wiederholung: kein Slot doppelt, kein Rueckwaertssprung "
                          "(%d gefunden)", backjumps);
    CHECK(ticks == fall_fc, "Sturz-Clip 0x0B laeuft GENAU %d Frames (gemessen %d) — "
                            "+0x95 0..%d, dann +0x6 += anim_set-Rueckgabe (@0x80105248-6c)",
          fall_fc, ticks, fall_fc - 1);
    CHECK(prev_slot == fall_fc - 1, "letzter posierter Slot = %d (Clip-Ende genau einmal)",
          fall_fc - 1);
    return fails;
}

int main(int argc, char **argv)
{
    /* ⛔ Der Default-Flavor ist seit 2026-08-22 RE2 (Nutzer-Entscheidung). Dieser PIN misst
     * byte-true RE1.5-Verhalten und muss den Modus deshalb EXPLIZIT setzen. */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    const char *which = (argc > 1) ? argv[1] : "all";
    if (load_em10() != 0) { printf("FAIL: EM010-Bank/Loco-Bank nicht ladbar\n"); return 1; }
    if (!strcmp(which, "bank") || !strcmp(which, "all")) pin_handoff_bank();
    if (!strcmp(which, "fall") || !strcmp(which, "all")) pin_fall_sequence();
    printf(fails ? "\nFEHLGESCHLAGEN (%d)\n" : "\nOK\n", fails);
    return fails ? 1 : 0;
}
