/* test_dog_grab_anchor.c — PIN: der SPIELER-ANKER des Hunde-Grabs (Nutzer-Report 2026-08-21:
 * "Wenn Leon von den Hunden angegriffen wird, verschwindet er teilweise. Ausserdem bekommt er
 * keine Blutwunde/Effekt vom Angriff.")
 *
 * ===================== SOLLSEITE (byte-gelesen aus STAGE1.BIN) =============================
 * Der Hunde-Grab-Einstieg schreibt ZWEI Anker, nicht einen:
 *   Sub 9 (Grab von VORN, FUN_8010f80c) Schritt 0:
 *     8010f8b0: lhu v1,-372(a0)     ; playerX (0x800ACA88)      a0 = 0x800ACBFC
 *     8010f8b8: sh  v1,160(v0)      ; HUND+0xA0  := playerX
 *     8010f8c8: lhu v0,-13680(v0)   ; playerZ (0x800ACA90)
 *     8010f8d0: sh  v0,162(v1)      ; HUND+0xA2  := playerZ
 *     8010f8d4: lhu v0,-372(a0)     ; playerX
 *     8010f8dc: lhu v1,-13680(v1)   ; playerZ
 *     8010f8ec: sh  v0,-13580(at)   ; 0x800ACAF4 = **SPIELER+0xA0** := playerX
 *     8010f8f4: sh  v1,-13578(at)   ; 0x800ACAF6 = **SPIELER+0xA2** := playerZ
 *   Sub 10 (Grab von HINTEN, FUN_8010fc60) Schritt 0 — Zwilling, Bytes identisch
 *   (0xA422CAF4 / 0xA423CAF6): @0x8010fd0c/@0x8010fd24 (Hund) und
 *   @0x8010fd40/@0x8010fd48 (Spieler).
 * Spielerblock-Basis = 0x800ACA54 (`addiu a0,a0,-13740` @0x80111a84, direkt vor dem
 * ad68-Aufruf auf den Spieler) -> +0xA0 = 0x800ACAF4, +0xA2 = 0x800ACAF6.
 *
 * WER LIEST DEN ANKER: FUN_8001ad68 (Port: re15_clip_root_motion_abs), das die Opfer-FSM in
 * JEDEM Grab-/Fress-Frame auf den SPIELER anwendet (`jal 0x8001ad68` @0x80111a88, Maschine A
 * Phase 0):
 *     8001adf4: lh   v0,160(s1)     ; Anker-X (+0xA0)
 *     8001ae00: addu v0,v0,v1       ; + rotierter Keyframe-Offset
 *     8001ae04: sw   v0,52(s1)      ; -> Spieler+0x34 = X
 *
 * GAME-WEITE ZAEHLUNG: die absoluten Stores auf 0x800ACAF4/F6 (Wortmuster 0xA42?CAF4 /
 * 0xA42?CAF6, Scan ueber ALLE STAGE*.BIN) haben genau 14 Treffer — ausschliesslich die
 * Hunde-Grab-Zwillinge (STAGE1 0x8010F8EC/F4 + 0x8010FD40/48 sowie die Kopien in
 * STAGE3/4/5). Der Spieler-Anker wird also NUR hier gesetzt.
 *
 * ===================== IST-LAGE VOR DEM FIX (gemessen, nicht modelliert) ====================
 * probe_dog_attack_live (ROOM1190, echter re15_game_step + Pad, echte EM020-Bank):
 *   f675 Grab-Commit    Leon=(9628,-19020)  Hund=(7860,-19020)  Spieler-Anker=(0,0)
 *   f676 1. Opfer-Tick  Leon=(0,-189)                            <- 9628 / 19000 Einheiten weg
 *   ... Leon blieb bei ~(340,0) durch Struggle UND Fress-Kollaps; der Hund frass ins Leere,
 *       und das an Spieler-Bone 8 verankerte Fress-Blut (Frames 0x29/0x3a) spawnte mit ihm
 *       in der Raumecke statt an der Angriffsstelle.
 * Nach dem Fix: Leon bleibt bei (9628..9965, -19020), Blut bei (9288,-692,-19248).
 *
 * Der Test pinnt beide Grab-Varianten, hat eine NEGATIV-KONTROLLE (vergifteter Anker vor dem
 * Grab — genau die Vor-Fix-Lage) und prueft den Blut-Anker als beobachtbare Folge.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_esp.h"
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_math.h"
#include "re15_ai_flavor.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_player.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { printf("FAIL: " __VA_ARGS__); \
    printf("\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, size_t *out)
{
    FILE *f = fopen(path, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *out = (size_t)sz; return b;
}

/* Die ECHTE EM020-Bank inkl. Opfer-Bank — ohne sie ist clip_len == 0 und jede Phase endet im
 * Setz-Tick (die Messung waere wertlos). */
static uint8_t *s_em20 = NULL;
static re15_enemy_bank_t *load_em020(void)
{
    re15_enemy_bank_t *eb = re15_enemy_find(0x20);
    if (eb && eb->ok) return eb;
    if (!eb) eb = re15_enemy_alloc(0x20);
    if (!eb) return NULL;
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) return NULL;
    int idx = re15_ems_index_for_type(0x20);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(ems, n, idx, &off, &len) != 0) { free(ems); return NULL; }
    if (!s_em20) { s_em20 = (uint8_t *)malloc(len); memcpy(s_em20, ems + off, len); }
    free(ems);
    if (re15_emd_parse_container(s_em20, len, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) return NULL;
    eb->ok = 1; eb->buf = NULL;
    eb->victim_ok = (re15_emd_parse_victim_bank(s_em20, len, &eb->skel_victim,
                                                &eb->anim_victim) == 0);
    return eb;
}

/* Eine Grab-Sequenz fahren. sub = 9 (vorn) / 10 (hinten). poison != 0 -> den Spieler-Anker
 * VOR dem Grab auf einen fernen Wert setzen (= die Vor-Fix-Lage: Anker != Spielerposition).
 * Liefert die groesste Abweichung von der Commit-Position ueber Struggle + Kollaps. */
typedef struct {
    int32_t commit_x, commit_z;      /* Spielerposition im Commit-Tick                     */
    int32_t anchor_x, anchor_z;      /* Spieler-Anker unmittelbar nach dem Commit          */
    int32_t max_drift;               /* max |Leon - Commit-Position| (Manhattan) danach    */
    int32_t max_dog_dist;            /* max |Leon - Hund| (Manhattan) waehrend des Griffs  */
    int     victim_seen, collapse_seen;
    int     n_blood; int32_t blood_x, blood_y, blood_z;
    int32_t bone8_x, bone8_y, bone8_z;   /* Renderer-Komposit Bone 8 im Blut-Tick */
    int     bone8_ok;
} grabrun_t;

/* Leons EIGENES Rig — der Renderer bindet es als def/pl00, und game_step spiegelt es per
 * re15_player_set_pl00_banks in die Engine. Ohne die Spiegelung faellt die Opfer-Bone-Abfrage
 * auf die Greifer-Hierarchie zurueck (dokumentierter Rueckfallpfad). */
static re15_emd_animation_t s_pl00_anim;
static re15_emd_skeleton_t  s_pl00_skel;
static int load_pl00(void)
{
    size_t a = 0, b = 0;
    uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &a);
    uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &b);
    return (edd && emr &&
            re15_emd_parse_animation(edd, a, &s_pl00_anim) == 0 &&
            re15_emd_parse_skeleton (emr, b, &s_pl00_skel) == 0);
}

/* Die Komposition, die der Renderer fuer den Grab-Override baut (platform/pc/main.c:
 * s_victim_skel = pl00_skel, nur keyframe_* aus skel_victim) = der Spieler-Part-Pool
 * *(Spieler+0x188), an dem das Original die Effekte ankert (Part 8 = +0x5A0). */
static int render_bone8(const re15_actor_t *pl, int32_t out[3])
{
    re15_enemy_bank_t *vb = re15_enemy_find(0x20);
    if (!vb || !vb->victim_ok) return 0;
    re15_emd_skeleton_t vs = s_pl00_skel;
    vs.keyframe_data       = vb->skel_victim.keyframe_data;
    vs.keyframe_data_size  = vb->skel_victim.keyframe_data_size;
    vs.keyframe_count      = vb->skel_victim.keyframe_count;
    vs.keyframe_size_bytes = vb->skel_victim.keyframe_size_bytes;
    int kf = re15_compute_actor_kf(&vb->anim_victim, &vs, pl, -1, (uint32_t)pl->anim_frame);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int rv = re15_skel_compute_pose(&vs, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    re15_skel_bone_to_world(poses[8].trans, pl->rot_y, pl->x, pl->y, pl->z, out);
    return 1;
}

static void run_grab(int sub, int poison, grabrun_t *out)
{
    memset(out, 0, sizeof *out);
    re15_actor_init();
    re15_esp_fx_reset();
    re15_wound_reset();
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->rot_y = 0;
    /* Auffaellige, NICHT-NULL Position: ein stehen gebliebener Anker (0,0) waere sonst
     * zufaellig richtig und der Fehler unsichtbar. */
    pl->x = 9628; pl->y = 0; pl->z = -19020;
    if (poison) { pl->anchor_x = 41000; pl->anchor_z = 41000; }

    const int DS = 1;
    re15_actor_t *d = &g_actors[DS];
    memset(d, 0, sizeof *d);
    d->active = 1; d->type = 0x20; d->state = 0;
    d->x = pl->x - 1768; d->y = 0; d->z = pl->z;
    d->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(d, 0x20);
    d->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - d->z, pl->x - d->x) - 0x400) & 0xfff);

    re15_enemy_ai_run_all(1);                       /* INIT -> ACTIVE */
    d->hp = 100;
    /* Byte-true Eskalationsziel des Bisses: +0x5 = 9 + facing (@0x8010f47c-84). Direkt setzen
     * wie probe_dog_devour — der Weg dorthin ist dort schon gepinnt. */
    d->state = 1; d->sub_state_1 = (uint8_t)sub; d->sub_state_2 = 0; d->sub_state_3 = 0;

    int committed = 0;
    int fx_prev = re15_esp_fx_count();
    for (int t = 0; t < 400; t++) {
        /* Position VOR dem AI-Tick: genau der Wert, den der Grab-Commit in den Anker schreibt
         * (@0x8010f8d4/@0x8010f8dc lesen Spieler+0x34/+0x3C). Nach dem Tick hat die Opfer-FSM
         * den Spieler bereits um den Clip-Root-Offset versetzt. */
        int32_t pre_x = pl->x, pre_z = pl->z;
        re15_enemy_ai_run_all(1);
        re15_player_victim_tick();
        if (!committed && re15_player_victim_state() != 0) {
            committed = 1;
            out->commit_x = pre_x; out->commit_z = pre_z;
            out->anchor_x = pl->anchor_x; out->anchor_z = pl->anchor_z;
        }
        if (re15_player_victim_state() == 1) out->victim_seen = 1;
        if (re15_player_victim_state() == 2) out->collapse_seen = 1;
        if (committed) {
            int32_t dx = pl->x - out->commit_x, dz = pl->z - out->commit_z;
            int32_t drift = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
            if (drift > out->max_drift) out->max_drift = drift;
            int32_t ex = pl->x - d->x, ez = pl->z - d->z;
            int32_t dd = (ex < 0 ? -ex : ex) + (ez < 0 ? -ez : ez);
            if (dd > out->max_dog_dist) out->max_dog_dist = dd;
        }
        int fx_now = re15_esp_fx_count();
        if (fx_now > fx_prev) {
            for (int i = RE15_ESP_FX_MAX - 1; i >= 0; i--) {
                const re15_esp_fx_t *f = re15_esp_fx_get(i);
                if (f) { out->blood_x = f->x; out->blood_y = f->y; out->blood_z = f->z; break; }
            }
            /* Referenz im SELBEN Tick: motion/anim_frame stehen schon auf dem Wert, mit dem der
             * Spawn gerechnet hat (der Frame-Advance laeuft vor dem Spawn). */
            int32_t b8[3];
            if (render_bone8(pl, b8)) {
                out->bone8_x = b8[0]; out->bone8_y = b8[1]; out->bone8_z = b8[2];
                out->bone8_ok = 1;
            }
            out->n_blood += (fx_now - fx_prev);
            fx_prev = fx_now;
        }
        if (pl->state == 7 && t > 300) break;
    }
}

int main(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init();
    if (!load_em020() || !re15_enemy_find(0x20)->victim_ok) {
        printf("WARN: EM020(-Opfer)-Bank fehlt — Messung uebersprungen\n");
        return 0;
    }
    if (!load_pl00()) { printf("WARN: PL00-Rig fehlt — Messung uebersprungen\n"); return 0; }
    re15_player_set_pl00_banks(&s_pl00_skel, &s_pl00_anim);   /* wie game_step_common.c:535 */
    printf("=== PIN: Spieler-Anker des Hunde-Grabs (@0x8010f8ec/@0x8010f8f4, "
           "sub10 @0x8010fd40/@0x8010fd48) ===\n");

    /* ---------------- TEIL 1: Grab von VORN (sub 9, FUN_8010f80c) ---------------- */
    grabrun_t r9; run_grab(9, 0, &r9);
    printf("[sub 9  vorn ] commit=(%d,%d) anker=(%d,%d) max-drift=%d max-dist-zum-Hund=%d "
           "struggle=%d kollaps=%d blut=%dx bei (%d,%d)\n",
           r9.commit_x, r9.commit_z, r9.anchor_x, r9.anchor_z, r9.max_drift,
           r9.max_dog_dist, r9.victim_seen, r9.collapse_seen, r9.n_blood,
           r9.blood_x, r9.blood_z);
    CHECK(r9.victim_seen, "(sub9) der Grab muss die Opfer-FSM latchen");
    CHECK(r9.anchor_x == r9.commit_x && r9.anchor_z == r9.commit_z,
          "(sub9) Spieler-Anker MUSS die Spielerposition sein (@0x8010f8ec/@0x8010f8f4) — "
          "ist (%d,%d), erwartet (%d,%d)", r9.anchor_x, r9.anchor_z, r9.commit_x, r9.commit_z);
    CHECK(r9.max_drift < 3000,
          "(sub9) Leon darf beim Grab NICHT wegteleportieren (FUN_8001ad68 setzt "
          "Spieler+0x34/+0x3C = Anker + Clip-Offset) — max-drift=%d", r9.max_drift);

    /* ---------------- TEIL 2: Grab von HINTEN (sub 10, FUN_8010fc60) ---------------- */
    grabrun_t r10; run_grab(10, 0, &r10);
    printf("[sub 10 hinten] commit=(%d,%d) anker=(%d,%d) max-drift=%d max-dist-zum-Hund=%d "
           "struggle=%d kollaps=%d\n",
           r10.commit_x, r10.commit_z, r10.anchor_x, r10.anchor_z, r10.max_drift,
           r10.max_dog_dist, r10.victim_seen, r10.collapse_seen);
    CHECK(r10.victim_seen, "(sub10) der Grab muss die Opfer-FSM latchen");
    CHECK(r10.anchor_x == r10.commit_x && r10.anchor_z == r10.commit_z,
          "(sub10) Spieler-Anker MUSS die Spielerposition sein (Zwilling @0x8010fd40/@0x8010fd48) — "
          "ist (%d,%d), erwartet (%d,%d)", r10.anchor_x, r10.anchor_z, r10.commit_x, r10.commit_z);
    CHECK(r10.max_drift < 3000,
          "(sub10) Leon darf beim Grab NICHT wegteleportieren — max-drift=%d", r10.max_drift);

    /* ---------------- TEIL 3: NEGATIV-KONTROLLE (vergifteter Anker = Vor-Fix-Lage) -------
     * Der Anker wird VOR dem Grab auf (41000,41000) gesetzt. Genau so sah es vor dem Fix aus
     * (dort: der nie gesetzte Anker (0,0)). Der Grab-Commit MUSS ihn ueberschreiben; tut er
     * es nicht, reisst FUN_8001ad68 Leon im ersten Opfer-Tick dorthin. */
    grabrun_t rp; run_grab(9, 1, &rp);
    printf("[negativ    ] Anker vorher (41000,41000) -> nach Commit (%d,%d), max-drift=%d\n",
           rp.anchor_x, rp.anchor_z, rp.max_drift);
    CHECK(rp.anchor_x == rp.commit_x && rp.anchor_z == rp.commit_z,
          "(negativ) der Commit MUSS einen fremden Anker ueberschreiben — ist (%d,%d)",
          rp.anchor_x, rp.anchor_z);
    CHECK(rp.max_drift < 3000,
          "(negativ) ein vergifteter Anker darf Leon NICHT verschleppen — max-drift=%d "
          "(ohne den Fix wandert er ~30000 Einheiten)", rp.max_drift);

    /* ---------------- TEIL 4: das FRESS-BLUT haengt am selben Anker -----------------------
     * Die Kollaps-Kadenz spawnt bei Leon-Frame 0x29/0x3a an Spieler-BONE 8 (@0x80111e30 /
     * @0x80111ddc, Anker acbdc+0x5A0). Steht Leon falsch, steht auch das Blut falsch — das
     * ist die zweite Haelfte des Nutzer-Befunds ("keine Blutwunde/Effekt vom Angriff"). */
    CHECK(r9.collapse_seen, "(blut) der Feed-Timeout muss in den Fress-Kollaps muenden (@0x8010fa70)");
    CHECK(r9.n_blood >= 2, "(blut) 2 Kollaps-Spawns erwartet (Frames 0x29/0x3a), sind %d", r9.n_blood);
    {
        int32_t bdx = r9.blood_x - r9.commit_x, bdz = r9.blood_z - r9.commit_z;
        int32_t bd  = (bdx < 0 ? -bdx : bdx) + (bdz < 0 ? -bdz : bdz);
        printf("[blut       ] Spawn (%d,%d,%d), Abstand zur Grab-Stelle = %d\n",
               r9.blood_x, r9.blood_y, r9.blood_z, bd);
        CHECK(bd < 3000, "(blut) das Fress-Blut muss AN der Angriffsstelle spawnen — Abstand %d "
              "(vor dem Fix landete es mit Leon in der Raumecke)", bd);
        /* ---- TEIL 5: der Blut-ANKER ist LEONS Part 8, nicht der Knochen des Greifers -------
         * Original: a2 = *(Spieler+0x188) + 0x5A0 (@0x80111dd4/@0x80111de0, identisch
         * @0x80111e28/@0x80111e34 und @0x80111b38/@0x80111b5c). Der Spieler-Part-Pool wird aus
         * LEONS Skelett gestellt; die Opfer-Bank liefert nur Clips+Keyframes. Mit der
         * dir[2]-EMR des HUNDES gerechnet lag der Anker 1457 Einheiten daneben
         * (probe_dog_victim_pose A/B). */
        CHECK(r9.bone8_ok, "(anker) Renderer-Komposit Bone 8 musste berechenbar sein");
        int32_t ax = r9.blood_x - r9.bone8_x, ay = r9.blood_y - r9.bone8_y,
                az = r9.blood_z - r9.bone8_z;
        int32_t ad = (ax<0?-ax:ax) + (ay<0?-ay:ay) + (az<0?-az:az);
        printf("[anker      ] Blut (%d,%d,%d) vs. Renderer-Bone8 (%d,%d,%d) -> Abweichung %d\n",
               r9.blood_x, r9.blood_y, r9.blood_z, r9.bone8_x, r9.bone8_y, r9.bone8_z, ad);
        CHECK(ad == 0,
              "(anker) das Blut MUSS exakt an Leons Part 8 haengen (*(Spieler+0x188)+0x5A0, "
              "@0x80111dd4/@0x80111de0) — Abweichung %d (mit der Greifer-Hierarchie: ~1457)", ad);
    }

    if (fails == 0) printf("OK: Hunde-Grab-Anker byte-true (beide Varianten) + Negativ-Kontrolle\n");
    else            printf("FAILS: %d\n", fails);
    return fails ? 1 : 0;
}
