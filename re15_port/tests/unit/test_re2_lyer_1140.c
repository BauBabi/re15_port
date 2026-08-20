/**
 * @file test_re2_lyer_1140.c
 * @brief PIN — der LIEGENDE Zombie in ROOM1140 (Dining Room) bleibt liegen. In JEDEM Modus.
 *
 * NUTZER-REPORT v0.3.4+: "Im Dining Room muss der Zombie, der gefressen wird, am Boden auch bei
 * RE2 AI und RE2 AI + Model am Boden liegen bleiben wie bei RE1.5 AI, und nicht aufstehen."
 *
 * ---- WER ER IST (echte Raumdaten, nicht behauptet) -----------------------------------------
 * ROOM1140.RDT sub00, Erstbesuch-Zweig (Ck zone3 flag 0xd2 clear): 5x Sce_em_set (0x44).
 *   slot 1: Typ 0x16 @(-800,0,-20600) yaw 1024 Deskriptor **0x88** -> Nibble 8 = LIEGEND
 *   slot 2..5: Typ 0x10/0x11, Deskriptor 0x86 -> Nibble 6 = FRESSEND
 * Der Liegende ist der, an dem die vier Fresser fressen.
 *
 * ---- DIE SOLLSEITE IST DER RE1.5-ZWILLING --------------------------------------------------
 * RE2 kennt ROOM1140 nicht; der Liege-Zustand kommt aus RE1.5-SPAWNDATEN. Die Frage ist also
 * nicht "was macht RE2 mit ihm", sondern: der RE2-Brain muss den Eingangszustand aus den
 * Spawndaten genauso uebernehmen wie der RE1.5-Brain. Selbst disassembliert aus
 * info/Re1.5/PSX/BIN/STAGE1.BIN (Overlay roh @0x80100000, kein 0x800-Header):
 *
 *   Sub-Modus-Dispatcher @0x8011F80C[+0x9 & 0xf]:
 *       [5] = [6] = 0x801018F8   (FRESSEND)
 *       [7] = [8] = 0x80101974   (LIEGEND)
 *       [9] = [10] = 0x801019F0  (SKRIPT-GEWECKT)
 *
 *   0x80101974 ist ein DOPPEL-Dispatch (DECIDE, dann ANIMATE):
 *       80101990: lui   at,0x8012
 *       80101994: addiu at,at,-1576     ; at = 0x8011F9D8  = DECIDE-Basis
 *       80101998: addu  at,at,v0        ; v0 = (+0x5)*4
 *       8010199c: lw    v0,0(at)
 *       801019a4: jalr  v0
 *       801019c8: addiu at,at,-1580     ; at = 0x8011F9D4  = ANIMATE-Basis
 *       801019d8: jalr  v0
 *
 *   DECIDE-Zeile @0x8011F9D8[0] = 0x801039F4 — und das ist ein LEERER STUB:
 *       801039f4: 03e00008  jr ra        (Rohbytes 08 00 e0 03)
 *       801039f8: 00000000  nop          (Rohbytes 00 00 00 00)
 *   => **Nibble 7/8 weckt sich NIE selbst** — weder nach Naehe noch nach Zeit.
 *
 *   Gegenprobe, dass die Nachbarzeilen ECHTE Funktionen sind (also kein Tabellen-Lesefehler):
 *       @0x8011F9D0[0] = 0x80103980 (FRESSEND) — dort steht der Naehe-Wecker:
 *           80103990: lw    v0,464(v1)   ; +0x1D0 = Spielerabstand
 *           80103998: sltiu v0,v0,4000   ; < 0xFA0
 *           8010399c: beq   v0,zero,0x801039e4
 *           801039b8: sb    v0,6(v1)     ; +0x6 = 1 -> Aufsteh-Kette
 *       @0x8011F9DC[0] = 0x801039FC (SKRIPT-GEWECKT) = Sofort-Wecker.
 *   Der EINZIGE Weckruf fuer 7/8 kommt vom SKRIPT, das den Deskriptor per
 *   Member_set(12, 0x89/0x8A) auf Nibble 9/10 hebt (`sb a2,9(a0)` @0x800411F8).
 *
 * ---- WAS DER PORT FALSCH MACHTE ------------------------------------------------------------
 * Der RE2-Liege-Executor EXEC[7] (@0x80103780) haelt den Spawn im Original ueber das Limpet-
 * Latch +0x10E & 0x4000 (P1 @0x8010381C-28); der einzige Overlay-Clear sitzt in EXEC[15]
 * @0x80104F0C und hat keinen RE'ten Produzenten. Der Port hatte dort ERSATZWEISE einen
 * Naehe-Wecker (dist < 0xBB8) eingesetzt — das ist der Zwilling des RE1.5-SCHLAEFERS (sel 0x0E,
 * ROOM10D0/10D1), NICHT der des Liege-Spawns. Damit stand der ROOM1140-Liegende auf.
 * GEMESSEN (probe_lyer_1140, echte RDT + echte Baenke): f44 faellt der Abstand erstmals unter
 * 0xBB8 (2996) -> +0x10E 0x4002 -> 0x0002 -> f45 Aufsteh-Clip -> f113 `0x101` = er steht.
 *
 * ---- WAS DIESER PIN PRUEFT -----------------------------------------------------------------
 * NICHT nur ein Flag: gemessen wird die TATSAECHLICHE LAGE = der hoechste Punkt der GERENDERTEN
 * Skelett-Pose (kleinstes Welt-Y ueber alle Bones; PSX-Y zeigt nach unten) zusammen mit
 * Clip, Zustand und Sub-Zustand — ueber 900 Frames, waehrend der Spieler bis auf ~650 Einheiten
 * herangeht. Drei Modi: RE1.5 (Regressionswache), RE2 AI (+RE2-Modelle) und RE2 AI + RE1.5-
 * Modelle (Hybrid-Rig, re2_hybrid_apply).
 * Dazu zwei Gegenproben, damit der Pin nicht auf einem toten Tick-Loop gruen wird:
 *   (P) POSITIV: im SELBEN Lauf muss mindestens ein FRESSER aufstehen (Nibble 6 hat den
 *       byte-belegten Naehe-Wecker @0x80103980).
 *   (N) NEGATIV: derselbe Liegende MUSS aufstehen, sobald das Skript seinen Deskriptor auf
 *       Nibble 9 hebt (Member_set(12,0x89) @0x800411F8) — der Halt ist also die
 *       Deskriptor-Regel und kein Einfrieren.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

extern int re15_actor_clip_len(const re15_actor_t *a);

/* Lage-Schwellen. Gemessen in dieser Sonde (probe_lyer_1140): liegend -224..-947,
 * stehend -2286..-2790. Die Schwellen liegen bewusst weit dazwischen, damit sie weder auf
 * Rig-Unterschiede (RE2 gegen Hybrid) noch auf Clip-Frames reagieren. */
#define LYING_MAX_TOP   (-1500)   /* liegend: hoechster Punkt TIEFER als das (also > -1500) */
#define STANDING_TOP    (-1900)   /* stehend: hoechster Punkt HOEHER als das (also < -1900) */

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* ---- Asset-Blobs (einmal geladen, bleiben resident — die Baenke aliasen hinein) ---------- */
static uint8_t *s_ems15, *s_ems2; static size_t s_ems15_n, s_ems2_n;
static uint8_t  s_scratch[3][0x80000];      /* RE1.5-Container brauchen eine eigene Kopie */
static const uint8_t s_ztypes[3] = { 0x10, 0x11, 0x16 };

static int load_bank15(uint8_t type, int slot)
{
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (!s_ems15 || idx < 0) return 0;
    if (re15_ems_get_entry(s_ems15, s_ems15_n, idx, &off, &len) != 0) return 0;
    if (len > sizeof s_scratch[0]) return 0;
    re15_enemy_bank_t *eb = re15_enemy_alloc(type); if (!eb) return 0;
    memcpy(s_scratch[slot], s_ems15 + off, len);
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (re15_emd_parse_container(s_scratch[slot], len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0)
        return 0;
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok = (re15_emd_parse_loco_bank(s_scratch[slot], len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->own_ok  = (re15_emd_parse_own_bank (s_scratch[slot], len, &eb->skel_own,  &eb->anim_own)  == 0);
    return 1;
}

/* RE2-Bank; `hybrid` != 0 baut sie auf das RE1.5-Rig um (== "RE2 AI" ohne "+ Model",
 * pc_enemy_load -> pc_enemy_hybrid_re15_models). */
static re15_md1_t           s_md15;
static re15_emd_skeleton_t  s_sk15;
static re15_emd_animation_t s_an15;
static uint8_t              s_hyb_scratch[0x80000];

static int load_bank2(uint8_t type, int hybrid)
{
    if (!s_ems2) return 0;
    re15_enemy_bank_t *eb = re15_enemy_alloc(type); if (!eb) return 0;
    if (re2_ems_load_bank(s_ems2, s_ems2_n, (int)type, eb, NULL) != 0) { eb->type = 0; return 0; }
    eb->buf = NULL; eb->ok = 1;
    if (!hybrid) return 1;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (!s_ems15 || idx < 0) return 1;
    if (re15_ems_get_entry(s_ems15, s_ems15_n, idx, &off, &len) != 0) return 1;
    if (len > sizeof s_hyb_scratch) return 1;
    memcpy(s_hyb_scratch, s_ems15 + off, len);
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (re15_emd_parse_container(s_hyb_scratch, len, &s_md15, &s_sk15, &s_an15, &tim) != 0) return 1;
    int unmapped = -1;
    (void)re2_hybrid_apply(eb, (int)type, &s_md15, &s_sk15, &unmapped);
    return 1;
}

/* Hoechster Punkt der GERADE posierten Skelett-Pose (kleinstes Welt-Y ueber alle Bones).
 * Das ist die eigentliche LAGE — rig-unabhaengig und unabhaengig davon, welches Flag gerade
 * gesetzt ist. Faellt auf die Aktor-Wurzel zurueck, wenn keine Bank da ist (dann schlaegt die
 * Bank-Pruefung weiter unten zuerst an). */
static int32_t pose_top_y(const re15_actor_t *e)
{
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || !b->ok || b->skel.bone_count <= 0) return e->y;
    int32_t best = 0x7fffffff;
    for (int i = 0; i < b->skel.bone_count; i++) {
        int32_t w[3]; re15_enemy_bone_world_pos(e, i, w);
        if (w[1] < best) best = w[1];
    }
    return (best == 0x7fffffff) ? e->y : best;
}

static void frame(void) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

/* Raum hochfahren; liefert den Slot des Liegenden (Deskriptor-Nibble 8) oder -1. */
static int bringup(const re15_rdt_t *rdt, int zslots[], int *nz)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_re2z_rng_reset();
    re15_damage_seed_rng(0x0badf00du);
    if (rdt->main_scd) scd_thread_start(0, rdt->main_scd);
    scd_thread_start(1, rdt->sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    int lyer = -1; *nz = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active) continue;
        if (e->type != 0x10 && e->type != 0x11 && e->type != 0x16) continue;
        zslots[(*nz)++] = s;
        if ((e->grid_id & 0x0fu) == 8u) lyer = s;        /* Deskriptor 0x88 = Liege-Spawn */
    }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    return lyer;
}

/* ---- ein kompletter Modus-Durchlauf ------------------------------------------------------ */
typedef struct {
    const char *name;
    int         re2;          /* 1 = RE2-Brain */
    int         hybrid;       /* 1 = RE1.5-Modelle unter RE2-Brain ("RE2 AI") */
    int         script_wake;  /* 1 = NEGATIV-Test: Skript hebt den Deskriptor auf Nibble 9 */
} mode_t;

static void run_mode(const re15_rdt_t *rdt, const mode_t *m)
{
    re15_ai_flavor_set(m->re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15);
    re15_ai_models_set(m->hybrid ? RE15_AI_MODELS_RE15 : RE15_AI_MODELS_RE2);

    int zslots[RE15_ACTOR_MAX], nz = 0;
    int lyer = bringup(rdt, zslots, &nz);
    CHECK(nz == 5, "%s: 5 Zombies erwartet, %d gespawnt", m->name, nz);
    CHECK(lyer >= 0, "%s: kein Liege-Spawn (Deskriptor-Nibble 8) im Raum", m->name);
    if (lyer < 0) return;
    CHECK(g_actors[lyer].type == 0x16 && g_actors[lyer].grid_id == 0x88,
          "%s: der Liegende ist Typ 0x16 mit Deskriptor 0x88, gemessen Typ 0x%02X grid 0x%02X",
          m->name, g_actors[lyer].type, g_actors[lyer].grid_id);

    /* Baenke: ohne sie ist re15_actor_clip_len()==0 und JEDE clip-getriebene Phase endet im
     * Setz-Tick — dann misst der Pin eine Fiktion. */
    for (int i = 0; i < 3; i++) {
        int ok = m->re2 ? load_bank2(s_ztypes[i], m->hybrid) : load_bank15(s_ztypes[i], i);
        CHECK(ok, "%s: Bank 0x%02X laedt nicht", m->name, s_ztypes[i]);
    }
    {   re15_enemy_bank_t *b = re15_enemy_find(0x16);
        CHECK(b && b->ok && b->skel.bone_count > 1 && b->anim.clip_count > 0,
              "%s: Bank 0x16 unbrauchbar (Bones=%d Clips=%d) — Lage-Messung waere blind",
              m->name, b ? b->skel.bone_count : -1, b ? b->anim.clip_count : -1);
        /* GEGENPROBE, dass die drei Modi wirklich DREI verschiedene Setups sind und der
         * Hybrid nicht still auf die RE2-Bank zurueckfaellt (sonst waere "RE2 AI" nur eine
         * Kopie von "RE2 AI + Model"): der Hybrid traegt die RE1.5-Geometrie (EM16 = 15
         * Meshes gegen RE2-EM016 = 17) und die Permutationstabelle. */
        if (b && b->ok && m->re2)
            CHECK(m->hybrid ? (b->remap_ok != 0) : (b->remap_ok == 0),
                  "%s: Hybrid-Rig-Zustand falsch (remap_ok=%d, Meshes=%d) — der Modus misst "
                  "nicht das, was er behauptet", m->name, b->remap_ok, b->md1.mesh_count);
        if (b && b->ok)
            printf("  [%s] Bank 0x16: %d Bones, %d Clips, %d Meshes, remap_ok=%d, "
                   "Bind[0]=(%d,%d,%d)\n", m->name, b->skel.bone_count, b->anim.clip_count,
                   b->md1.mesh_count, b->remap_ok, b->skel.bone_relative_pos[0][0],
                   b->skel.bone_relative_pos[0][1], b->skel.bone_relative_pos[0][2]); }

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *L  = &g_actors[lyer];
    pl->x = L->x; pl->z = L->z + 12000;                 /* Raumeingang -> er geht hin */

    if (m->script_wake) {
        /* NEGATIV-Test: das Weck-Signal des Spiels. ROOM1100 sub02 fuehrt es pro Zombie als
         * `Work_set(2,n); Member_set(12, 0x89/0x8A)` aus; Member 12 ist entity+0x9, der
         * Sprungtabellen-Fall endet mit `sb a2,9(a0)` @0x800411F8. */
        L->grid_id = (uint8_t)((L->grid_id & 0xf0u) | 9u);
    }

    int32_t top_min = 0x7fffffff, top_max = -0x7fffffff;   /* min = HOECHSTER Punkt */
    int  bad_state = -1, bad_pose = -1, rose = -1;
    int  feeder_rose = 0;
    uint8_t clip_seen[256]; memset(clip_seen, 0, sizeof clip_seen);

    for (int f = 0; f < 900; f++) {
        if (pl->z > L->z + 500) pl->z -= 200;             /* langsam heran, dann daneben */
        pl->motion = 100;
        if (pl->hp < 30) { pl->hp = 100; pl->state = 0; pl->hit_react = 0; }
        frame();

        int32_t top = pose_top_y(L);
        if (top < top_min) top_min = top;
        if (top > top_max) top_max = top;
        clip_seen[L->motion] = 1;

        /* Zustands-Seite: der Liege-Eingangszustand darf nicht verlassen werden.
         * RE2  : ACTIVE sub 7 = EXEC[7] (INIT-Commit 0x701 @0x801009E8-0x80100A84)
         * RE1.5: ACTIVE mit unveraendertem Deskriptor-Nibble 8 (Dispatch @0x8011F80C[8]) */
        int lying_state = m->re2 ? (L->state == 1 && L->sub_state_1 == 7)
                                 : (L->state == 1 && (L->grid_id & 0x0fu) == 8u);
        if (!lying_state && rose < 0) rose = f;
        if (!lying_state && bad_state < 0) bad_state = f;
        /* LAGE-Seite: der hoechste Punkt der gerenderten Pose bleibt unten. */
        if (top < LYING_MAX_TOP && bad_pose < 0) bad_pose = f;

        for (int i = 0; i < nz; i++)                       /* (P) Positiv-Kontrolle */
            if (zslots[i] != lyer && pose_top_y(&g_actors[zslots[i]]) < STANDING_TOP)
                feeder_rose = 1;
    }

    printf("  [%s] Liegender slot %d: Endzustand state=%d +0x5=%d +0x6=%d grid=0x%02X clip=%d "
           "(len %d) | Pose-Top %d..%d | Aufstehen bei Frame %d | Fresser aufgestanden=%d\n",
           m->name, lyer, L->state, L->sub_state_1, L->sub_state_2, L->grid_id, (int)L->motion,
           re15_actor_clip_len(L), top_min, top_max, rose, feeder_rose);

    if (m->script_wake) {
        /* (N) Der Skript-Wecker MUSS ihn hochholen — sonst ist der Halt ein Einfrieren. */
        CHECK(rose >= 0,
              "%s NEGATIV-TEST: nach Member_set(12,0x89) (@0x800411F8) muss der Liegende den "
              "Liege-Zustand verlassen — er tat es in 900 Frames NICHT", m->name);
        CHECK(top_min < STANDING_TOP,
              "%s NEGATIV-TEST: nach dem Skript-Wecker muss die Pose STEHEND werden "
              "(hoechster Punkt < %d), gemessen bestenfalls %d",
              m->name, STANDING_TOP, top_min);
        return;
    }

    CHECK(bad_state < 0,
          "%s: der Liege-Spawn (Deskriptor 0x88) hat den Liege-Zustand bei Frame %d verlassen — "
          "RE1.5-Zwilling: DECIDE @0x8011F9D8[0] = 0x801039F4 = `jr ra; nop`, Nibble 7/8 weckt "
          "sich NIE selbst", m->name, bad_state);
    CHECK(bad_pose < 0,
          "%s: die GERENDERTE Pose des Liegenden steht bei Frame %d auf (hoechster Punkt %d < %d)",
          m->name, bad_pose, top_min, LYING_MAX_TOP);
    CHECK(top_min > LYING_MAX_TOP,
          "%s: hoechster Punkt der Liege-Pose ueber 900 Frames = %d, muss unterhalb von %d "
          "bleiben", m->name, top_min, LYING_MAX_TOP);
    CHECK(L->state == 1 && (m->re2 ? (L->sub_state_1 == 7) : ((L->grid_id & 0x0fu) == 8u)),
          "%s: Endzustand ist nicht mehr der Liege-Zustand (state=%d +0x5=%d grid=0x%02X)",
          m->name, L->state, L->sub_state_1, L->grid_id);
    if (m->re2)
        CHECK((L->re2z_f10e & 0x4000u) != 0,
              "%s: das Limpet-Latch +0x10E & 0x4000 (P1 @0x8010381C-28) wurde geloest — genau "
              "dieser Clear (andi 0xbfff @0x80104F0C) ist der Aufsteh-Ausloeser, +0x10E=0x%04X",
              m->name, L->re2z_f10e);
    CHECK(feeder_rose,
          "%s POSITIV-KONTROLLE: kein einziger FRESSER ist aufgestanden — dann misst der Pin "
          "einen toten Tick-Loop, nicht das Liegenbleiben (Nibble 6 hat den byte-belegten "
          "Naehe-Wecker @0x80103980, dist < 0xFA0)", m->name);
}

int main(void)
{
    size_t sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", &sz);
    if (!buf) { printf("FAIL: ROOM1140.RDT nicht lesbar\n"); return 1; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(buf, sz, &rdt) != 0 || !rdt.sub_scd[0]) {
        printf("FAIL: RDT-Parse/sub00\n"); free(buf); return 1;
    }
    s_ems15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS",     &s_ems15_n);
    s_ems2  = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS",  &s_ems2_n);
    CHECK(s_ems15 != NULL, "RE1.5 EMD/CDEMD0.EMS fehlt");
    CHECK(s_ems2  != NULL, "RE2 shared_assets/RE2/CDEMD0.EMS fehlt");

    /* ---- DER DATENBEWEIS: ROOM1140 hat GAR KEINEN Wecker ---------------------------------
     * Nibble 7/8 kann nur das SKRIPT wecken (Member_set(12, 0x89/0x8A) -> `sb a2,9(a0)`
     * @0x800411F8). Byte-Zensus ueber die ausgelieferten RDTs (Muster 0x34,0x0C,0x89|0x8A):
     *   ROOM1100 / ROOM1101 : je 5 Records  (die "Leichen erwachen"-Kaskade)
     *   ROOM1140            : 0 Records
     * Der Dining-Room-Liegende wird also von NIEMANDEM geweckt — weder von sich selbst
     * (DECIDE-Stub) noch vom Skript. Er liegt bis zum Raumwechsel. */
    {
        size_t n1140 = 0, n1100 = 0;
        for (int k = 0; k < 2; k++) {
            const char *rp = k ? RE15_ASSET_PSX_DIR "/STAGE1/ROOM1100.RDT"
                               : RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT";
            size_t bn = 0; uint8_t *bb = slurp(rp, &bn);
            size_t hits = 0;
            for (size_t i = 0; bb && i + 3 < bn; i++)
                if (bb[i] == 0x34 && bb[i+1] == 12 && (bb[i+2] == 0x89 || bb[i+2] == 0x8A)) hits++;
            if (k) n1100 = hits; else n1140 = hits;
            free(bb);
        }
        printf("  [Daten] Member_set(12,0x89/0x8A): ROOM1140 = %u, ROOM1100 = %u\n",
               (unsigned)n1140, (unsigned)n1100);
        CHECK(n1140 == 0,
              "ROOM1140 duerfte KEINEN Skript-Wecker fuer den Liege-Spawn enthalten, "
              "gefunden %u", (unsigned)n1140);
        CHECK(n1100 == 5,
              "GEGENPROBE: ROOM1100 MUSS die 5 Skript-Wecker-Records tragen (sonst sucht der "
              "Zensus das falsche Muster), gefunden %u", (unsigned)n1100);
    }

    static const mode_t modes[4] = {
        { "RE1.5 AI",            0, 0, 0 },   /* Regressionswache: war schon immer richtig  */
        { "RE2 AI + Model",      1, 0, 0 },   /* RE2-Brain + RE2-Modelle                    */
        { "RE2 AI",              1, 1, 0 },   /* RE2-Brain + RE1.5-Modelle (Hybrid-Rig)     */
        { "RE2 Skript-Wecker",   1, 0, 1 },   /* NEGATIV-Test                               */
    };
    for (int i = 0; i < 4; i++) run_mode(&rdt, &modes[i]);

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_ai_models_set(RE15_AI_MODELS_RE2);
    free(buf);
    if (fails == 0) printf("test_re2_lyer_1140: OK\n");
    else            printf("test_re2_lyer_1140: %d FAILURES\n", fails);
    return fails ? 1 : 0;
}
