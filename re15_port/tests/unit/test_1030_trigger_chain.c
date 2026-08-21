/* test_1030_trigger_chain.c — PIN: die ROOM1030-Kriech-Ausloesekette ueber den ECHTEN WEG.
 *
 * NUTZER-REPORT (2026-08-21): "Bei RE2-KI kriechen die Zombies in der Cutscene nicht unter das
 * Tor (ROOM1030)." Commit 4484fb3a hat das Kriecher-BRAIN gebaut und gemessen, aber gemeldet, der
 * SKRIPT-AUSLOESER feuere unter KEINEM Flavor — mit zwei benannten Blockern (`work_slot == -1`,
 * `member_0b` einen Frame spaeter gewischt). BEIDE BLOCKER EXISTIEREN NICHT. Sie waren Artefakte
 * der damaligen Sonde. Dieser Test faehrt die Kette so, wie das Spiel sie faehrt, und pinnt sie.
 *
 * ============================ DIE KETTE, Bytes selbst gedumpt ============================
 * Sub-Offset-Tabelle @Datei 0x1fd0 (Basis 0x1fd0, 12 Eintraege — selbst geparst):
 *   sub00 0x1fe8 · sub01 0x2180 · sub02 0x21a6 · sub03 0x21f8 · sub04 0x2226 · sub05 0x24b2
 *   sub06 0x24c8 · sub07 0x2754 · sub08 0x2764 · sub09 0x27e0 · sub10 0x283a · sub11 0x2878
 *
 * sub00 @0x1fe8   `22 03 74 01` Set(3,0x74,1)  … Tail @0x2172 `04 ff 18 0b`, @0x2176 `04 ff 18 02`
 *                 -> Evt_exec spawnt sub11 UND sub02 (den Kriech-Zyklus) BEDINGUNGSLOS.
 * sub01 @0x2180   `06 00 20 00 / 21 03 74 01 / 21 04 0f 00 / 21 05 20 01`
 *                 `06 00 0e 00 / 21 05 22 01 / 22 04 0f 01 / 04 ff 18 08 / 08 00 08 00 / 01 00`
 *                 = wenn Story-Flag UND Spieler in AOT-3 UND Gegner in AOT-5:
 *                   Set(4,0x0f) + Evt_exec sub08.  sub01 wird in JEDEM Gameplay-Frame in
 *                   Thread-Slot 1 neu geseedet (FUN_8003f038 @0x8003f064-84).
 * sub08 @0x2764   die Cutscene: `22 01 1b 01` Kino an, Tor-Zellen `37 02 06 f7`/`37 03 06 f7`,
 *                 4x `18 09` (SOLO-Trigger), Kamera-Cuts, am Ende `22 05 14 01` Set(5,0x14).
 * sub09 @0x27e0   SOLO: `53 02 05 00` (indirekter Work_set ueber wv5) / `3e 00 0f 00 05 00`
 *                 (Member_cmp 15 == 5) / INLINE `3d 04 10 / 26 00 05 04 00 10 / 35 10 04`.
 * sub02 @0x21a6   `18 04 / 06 00 2e 00 / 21 05 14 01 / 46 04 … / 46 06 05 42 … / 02 00 /
 *                 18 03 / 06 00 0c 00 / 23 00 07 03 04 00 / 02 00 / 18 06 / 08 00 08 00 /
 *                 46 04 04 42 … / 46 06 00 00 … / 02 00 / 17 ff ff 00 b6 ff` = der Gruppen-Zyklus.
 * sub06 @0x24c8   20 Bloecke a 32 Byte: `21 05 i 00` (Latch frei) / `2e 02 i` Work_set(kind 2,
 *                 Slot i) / `06 00 0e 00` / `3e 00 0f 00 05 00` Member_cmp(15,==,5) /
 *                 `18 07` Gosub 7 / `22 05 i 01`.
 * sub07 @0x2754   `3d 04 10 / 26 00 05 04 00 10 / 35 10 04 / 01 00` = entity+0x1C4 |= 0x1000.
 *
 * ⛔ DER TORWAECHTER UND DER Work_set LIEGEN IM AUFRUFER, NICHT IN sub07. Wer sub07 per
 * `scd_event_fire(7)` als eigenen Thread startet, ueberspringt `2e 02 i` — dann ist
 * `work_slot == -1` und `Member_get/Member_set` haben kein Ziel. Das war Blocker (1).
 * Blocker (2) — "`member_0b` wird einen Frame spaeter gewischt" — war derselbe Fehler von der
 * anderen Seite: ein von Hand gesetzter Stempel ohne echten Zonen-Treffer faellt im byte-true
 * Aktiv-Clear (@0x80043704 `andi v0,v0,0x1` / @0x8004371c `sb zero,0(at)`) auf 0 zurueck.
 *
 * ============================ WAS DIE SONDE BRAUCHT ============================
 * Drei Dinge, ohne die diese Kette in JEDEM Testrahmen tot aussieht (alle drei fehlten):
 *   1. `scd_vm_tick()` PRO FRAME und VOR `re15_game_step()`. Die VM haengt NICHT an game_step;
 *      die Haupt-Schleife ruft sie selbst (platform/pc/main.c:3578) — byte-true die Reihenfolge
 *      des Originals: @0x8001cdec `jal 0x8003f038` (VM) vor @0x8001ce04 (Gegner-AI),
 *      @0x8001ce0c (Spieler), @0x8001ce1c (AOT-Scan/Stempel).
 *   2. Die LOCO-Bank (`re15_emd_parse_loco_bank`). `re15_enemy_footlock_step` steigt bei
 *      `!bank->loco_ok` sofort aus — ohne sie animieren die Zombies auf der Stelle und
 *      erreichen die AOT-Zone 5 NIE.
 *   3. `g_room_rdt` + `g_room_rdt_ok` — daraus zieht der Gegner-SCA-Wandclamp seine Zellen
 *      (`re15_collision_constrain_enemy(&g_room_rdt, …)`). Ohne das ist das Rolltor offen.
 *
 * ============================ HARDWARE-SOLL ============================
 * tools/redux/crawl_cycle_out.txt (PCSX-Redux, Exec-BP @0x80030444), gf 4342..4370:
 *   gf 4342  alle 6 Zombies Typ 0x16 auf den Spawn-Positionen, `94=27`, `04/05/06 = 01/02/00`
 *   gf 4343  alle 6 `94=02`/`05`, `04/05/06 = 01/02/01`  -> sie LAUFEN los
 *   gf 4369  Slot 4 bei (-7239,-25285), `0B=05`          -> Zone-5-Stempel
 *   gf 4370  B4W0=0x00010000 = flag(4,0x0f), tor2/tor3 = f7, `1C4=1000`  -> die Kette lebt
 *   sub08-Ende `Set(5,0x14)` bei t+450 (2x gemessen).
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

/* Riegel-Oberkante: SCA rgn2/idx6 == rgn3/idx6, Rechteck (-20144,-24420, 24264 x 1649)
 * -> z[-24420 .. -22771]. NOERDLICH (= in der Lobby) heisst z > -22771. */
#define GATE_Z_NORTH (-22771)
/* AOT-Slot 3 (sce_flags 0x41 = Pool SPIELER | CENTRE), Traeger von flag(5,0x20):
 * x[-19900..-16200] z[-9200..-7400]. Mitte: */
#define TRIG_X (-18050)
#define TRIG_Z (-8300)
/* Die Debug-Sprung-Position aus dem Hardware-Lauf — am Tor, AUSSERHALB von AOT-3. */
#define GATE_X (-8200)
#define GATE_Z (-22303)

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: "); printf(__VA_ARGS__); printf("\n"); fails++; } } while (0)

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

/* ---- Baenke (MIT Loco — sonst bewegt sich kein Zombie) --------------------------------------- */
static uint8_t  s_blob[0x80000];
static uint8_t *s_re15_ems = NULL; static size_t s_re15_n = 0;
static uint8_t *s_re2_ems  = NULL; static size_t s_re2_n  = 0;

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
    eb->own_ok  = (eb->anim_own.clip_count > 0);
    eb->loco_ok = (re15_emd_parse_loco_bank(s_blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
    return eb->loco_ok;
}

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

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();          /* platform/pc/main.c:3578 — VOR game_step (@0x8001cdec vor 0x8001ce04) */
    re15_game_step(&s_ctx);
}

static int is_zombie(const re15_actor_t *e) { return e->active && e->type == 0x16; }
static int crawling(const re15_actor_t *e, re15_ai_flavor_t fl)
{
    return (fl == RE15_AI_FLAVOR_RE2) ? ((e->re2z_f10e & 1u) != 0) : (e->grid_id == 0x81);
}

typedef struct {
    int spawned;
    int f_stamp5;      /* erster Frame mit entity+0x0B == 5                        */
    int f_flag40f;     /* erster Frame mit flag(4,0x0f)  = sub01 @0x2198           */
    int f_bit1000;     /* erster Frame mit +0x1C4 & 0x1000 = sub07/sub09           */
    int f_crawl;       /* erster Frame im Kriech-Zustand                            */
    int f_flag514;     /* erster Frame mit flag(5,0x14)  = sub08-Ende               */
    int flagged;       /* Zombies, die das 0x1000 jemals trugen                     */
    int crawlers;
    int sca8;
    int crossed;       /* Zombies, die den Riegel nach Norden ueberquert haben      */
} run_t;

/* freeze_south != 0: die Zombies werden jeden Frame auf ihre Spawn-Position zurueckgesetzt.
 * Alle sechs Spawn-Positionen liegen SUEDLICH der AOT-Zone 5 (z <= -25655 gegen z >= -25300),
 * also wird flag(5,0x22) nie gesetzt — die Negativprobe fuer die zweite Gatter-Haelfte. */
static void scenario(re15_ai_flavor_t fl, const char *name, int32_t px, int32_t pz,
                     int frames, int freeze_south, run_t *out)
{
    memset(out, 0, sizeof *out);
    out->f_stamp5 = out->f_flag40f = out->f_bit1000 = out->f_crawl = out->f_flag514 = -1;

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(fl);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x1030;
    /* ⛔ OHNE GELADENE BANK misst diese Sonde Muell: ohne LOCO steht jeder Zombie still und
     * erreicht die AOT-Zone 5 nie. Deshalb hart pruefen statt still weiterlaufen. */
    {
        int ok = (fl == RE15_AI_FLAVOR_RE2) ? load_bank_re2(0x16) : load_bank_re15(0x16);
        re15_enemy_bank_t *eb = re15_enemy_find(0x16);
        CHECK(ok && eb && eb->loco_ok, "%s: EM016-Bank (inkl. LOCO) nicht geladen — "
              "die Messung waere wertlos", name);
    }

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = 1024;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);

    int32_t sx[RE15_ACTOR_MAX], sz[RE15_ACTOR_MAX];
    int seen_flag[RE15_ACTOR_MAX], seen_crawl[RE15_ACTOR_MAX], seen_sca8[RE15_ACTOR_MAX];
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        sx[s] = g_actors[s].x; sz[s] = g_actors[s].z;
        seen_flag[s] = seen_crawl[s] = seen_sca8[s] = 0;
    }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (is_zombie(&g_actors[s])) out->spawned++;

    for (int f = 0; f < frames; f++) {
        pl->x = px; pl->z = pz;                       /* der Spieler steht — reine Eingabe */
        if (freeze_south)
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (is_zombie(&g_actors[s])) { g_actors[s].x = sx[s]; g_actors[s].z = sz[s]; }
        frame_step();

        if (out->f_flag40f < 0 && re15_game_flag_get(4, 0x0f)) out->f_flag40f = f;
        if (out->f_flag514 < 0 && re15_game_flag_get(5, 0x14)) out->f_flag514 = f;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *e = &g_actors[s];
            if (!is_zombie(e)) continue;
            if (out->f_stamp5  < 0 && e->member_0b == 5)      out->f_stamp5  = f;
            if (e->anim_flags & 0x1000u) {
                seen_flag[s] = 1;
                if (out->f_bit1000 < 0) out->f_bit1000 = f;
            }
            if (crawling(e, fl)) {
                seen_crawl[s] = 1;
                if (out->f_crawl < 0) out->f_crawl = f;
            }
            if (e->sca_mask == 8) seen_sca8[s] = 1;
        }
    }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!is_zombie(&g_actors[s])) continue;
        out->flagged  += seen_flag[s];
        out->crawlers += seen_crawl[s];
        out->sca8     += seen_sca8[s];
        if (sz[s] <= GATE_Z_NORTH && (int32_t)g_actors[s].z > GATE_Z_NORTH) out->crossed++;
    }
    printf("  [%-42s] Zombies=%d  Stempel5@%d  flag(4,0x0f)@%d  0x1000@%d  kriecht@%d  "
           "flag(5,0x14)@%d  geflaggt=%d kriechend=%d sca8=%d durch=%d\n",
           name, out->spawned, out->f_stamp5, out->f_flag40f, out->f_bit1000, out->f_crawl,
           out->f_flag514, out->flagged, out->crawlers, out->sca8, out->crossed);
}

int main(void)
{
    printf("== ROOM1030: feuert die Kriech-Ausloesekette ueber den ECHTEN WEG? ==\n\n");

    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1030.RDT", &n);
    if (!buf) { printf("FAIL: ROOM1030.RDT nicht lesbar\n"); return 1; }
    /* Der Gegner-SCA-Wandclamp liest g_room_rdt, NICHT den ctx-RDT. */
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;

    run_t p15, p2, n15, n2, s15, s2;

    printf("--- POSITIV: Spieler im Ausloese-Rechteck AOT-3 (%d,%d) ---\n", TRIG_X, TRIG_Z);
    scenario(RE15_AI_FLAVOR_RE15, "RE1.5 Spieler in AOT-3", TRIG_X, TRIG_Z, 1400, 0, &p15);
    scenario(RE15_AI_FLAVOR_RE2,  "RE2   Spieler in AOT-3", TRIG_X, TRIG_Z, 1400, 0, &p2);

    printf("\n--- NEGATIV 1: Spieler am Tor, NICHT in AOT-3 -> flag(5,0x20) fehlt ---\n");
    scenario(RE15_AI_FLAVOR_RE15, "RE1.5 Spieler am Tor", GATE_X, GATE_Z, 900, 0, &n15);
    scenario(RE15_AI_FLAVOR_RE2,  "RE2   Spieler am Tor", GATE_X, GATE_Z, 900, 0, &n2);

    printf("\n--- NEGATIV 2: Spieler in AOT-3, Zombies suedlich von Zone 5 festgehalten ---\n");
    scenario(RE15_AI_FLAVOR_RE15, "RE1.5 kein Gegner in Zone 5", TRIG_X, TRIG_Z, 900, 1, &s15);
    scenario(RE15_AI_FLAVOR_RE2,  "RE2   kein Gegner in Zone 5", TRIG_X, TRIG_Z, 900, 1, &s2);

    printf("\n== PRUEFUNG ==\n");
    /* --- Positiv: die Kette laeuft in BEIDEN Flavors ueber den echten Weg --------------------- */
    const struct { const char *tag; run_t *r; } pos[2] = { { "RE1.5", &p15 }, { "RE2", &p2 } };
    for (int k = 0; k < 2; k++) {
        run_t *r = pos[k].r; const char *t = pos[k].tag;
        CHECK(r->spawned == 6, "%s: ROOM1030 spawnt %d statt 6 Zombies (Cap work_var[18]=6 "
                               "@Datei 0x1de2 `24 12 06 00`)", t, r->spawned);
        CHECK(r->f_stamp5 >= 0, "%s: kein Zombie bekam je den AOT-Stempel +0x0B == 5 "
                                "(FUN_80042bac `sb v0,11(s1)` @0x80042fc4)", t);
        CHECK(r->f_flag40f >= 0, "%s: sub01 @0x2198 `22 04 0f 01` hat flag(4,0x0f) nie gesetzt "
                                 "— die Cutscene sub08 startet nie", t);
        CHECK(r->f_bit1000 >= 0, "%s: KERN — kein Zombie bekam je +0x1C4 |= 0x1000 "
                                 "(sub07 @0x2754 / sub09 @0x27e0)", t);
        CHECK(r->f_crawl >= 0, "%s: KERN — kein Zombie ist in den Kriecher eingetreten", t);
        CHECK(r->sca8 >= 1, "%s: kein Kriecher fuehrt die Torzellen-Maske 8 (+0x1D7 @0x801050F4)", t);
        CHECK(r->crossed >= 1, "%s: KERN (die Frage des Nutzers) — kein Kriecher ist unter dem Tor "
                               "hindurch in die Lobby gekommen (Riegel z > %d)", t, GATE_Z_NORTH);
        CHECK(r->f_flag514 >= 0, "%s: sub08 hat sein `22 05 14 01` (Set(5,0x14) am Ende, "
                                 "Hardware t+450) nie erreicht", t);
        /* Reihenfolge der Kette: Stempel -> flag(4,0x0f) -> 0x1000 -> Kriechen. */
        CHECK(r->f_stamp5 <= r->f_flag40f, "%s: der Stempel kam NACH flag(4,0x0f) (%d > %d) — "
              "die Reihenfolge der Kette stimmt nicht", t, r->f_stamp5, r->f_flag40f);
        CHECK(r->f_bit1000 >= r->f_flag40f, "%s: 0x1000 kam VOR flag(4,0x0f) (%d < %d)",
              t, r->f_bit1000, r->f_flag40f);
        /* Nicht ">" : das Original konsumiert das Bit IM SELBEN FRAME
         * (Hardware §18: "0x1000 -> Steer -> 0x1001 -> Toggle im SELBEN Frame"). Unter RE1.5
         * faellt der Kriech-COMMIT erst 95 Frames spaeter (Toggle-Clip 0x12), unter RE2 markiert
         * +0x10E Bit 0 schon den Eintritt — beides ist >= und nie davor. */
        CHECK(r->f_crawl >= r->f_bit1000, "%s: Kriech-Eintritt kam VOR dem 0x1000 (%d < %d)",
              t, r->f_crawl, r->f_bit1000);
    }
    /* --- Negativ 1: ohne flag(5,0x20) (Spieler nicht im Rechteck) darf nichts feuern ---------- */
    const struct { const char *tag; run_t *r; } neg[4] = {
        { "NEG1 RE1.5 (Spieler am Tor)",    &n15 }, { "NEG1 RE2 (Spieler am Tor)",    &n2 },
        { "NEG2 RE1.5 (kein Gegner Zone5)", &s15 }, { "NEG2 RE2 (kein Gegner Zone5)", &s2 } };
    for (int k = 0; k < 4; k++) {
        run_t *r = neg[k].r; const char *t = neg[k].tag;
        CHECK(r->f_flag40f < 0, "%s: flag(4,0x0f) wurde gesetzt (f=%d) — das Gatter @0x2180 "
                                "ist aufgerissen", t, r->f_flag40f);
        CHECK(r->f_bit1000 < 0, "%s: +0x1C4 |= 0x1000 feuerte trotz fehlender Vorbedingung (f=%d)",
              t, r->f_bit1000);
        CHECK(r->f_crawl < 0, "%s: ein Zombie kroch trotz fehlender Vorbedingung (f=%d)",
              t, r->f_crawl);
    }
    /* NEG2 muss den Stempel trotzdem VERWEIGERN (alle 6 Spawn-Positionen liegen suedlich von
     * Zone 5, z <= -25655 gegen z >= -25300) — sonst haette die Negativprobe gar nichts isoliert. */
    CHECK(s15.f_stamp5 < 0, "NEG2 RE1.5: ein festgehaltener Zombie bekam Stempel 5 (f=%d) — "
                            "die Negativprobe isoliert die Gatter-Haelfte nicht", s15.f_stamp5);
    CHECK(s2.f_stamp5  < 0, "NEG2 RE2: ein festgehaltener Zombie bekam Stempel 5 (f=%d)",
          s2.f_stamp5);
    /* NEG1 muss den Stempel dagegen SEHR WOHL vergeben (die Zombies stehen am Tor in Zone 5) —
     * das beweist, dass NEG1 wirklich nur an flag(5,0x20) scheitert und nicht an einem toten Raum. */
    CHECK(n15.f_stamp5 >= 0, "NEG1 RE1.5: kein Stempel 5 — die Negativprobe ist wertlos "
                             "(Positiv-Kontrolle fehlt)");
    CHECK(n2.f_stamp5  >= 0, "NEG1 RE2: kein Stempel 5 — die Negativprobe ist wertlos");

    free(buf);
    g_room_rdt_ok = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    printf("\n== %s (%d Fehler) ==\n", fails ? "FEHLGESCHLAGEN" : "OK", fails);
    return fails ? 1 : 0;
}
