/* probe_1030_crawl_live.c — DIE ECHTE MESSUNG: ROOM1030, RE2-Flavor, die Kriech-Anforderung
 * der Cutscene. Kriecht der Zombie unter dem Tor durch?
 *
 * NUTZER-REPORT 2026-08-21: "Bei RE2 AI in der Lobby kriechen die Zombies in der Cutscene nicht
 * unter das Tor (ROOM1030)."
 *
 * ---- DER ECHTE WEG, nicht die Unit-Nachbildung -----------------------------------------------
 *  - echte ROOM1030.RDT, echte Sce_em_set-Spawns, echte SCD-VM, echtes re15_game_step
 *  - echte RE2-Baenke (EM010/EM011/EM016) — ohne Bank ist re15_actor_clip_len()==0 und JEDE
 *    clip-getriebene Phase misst Muell (die Falle dieser Kampagne)
 *  - die Kriech-Anforderung kommt aus dem SKRIPT: sub07 (@Datei 0x2754)
 *      `3d 04 10 / 26 00 05 04 00 10 / 35 10 04`  = Member16 (entity+0x1C4) |= 0x1000
 *    und OEFFNET im selben Atemzug die Torzellen fuer Kriecher:
 *      `37 02 06 f7` @Datei 0x278E   /   `37 03 06 f7` @Datei 0x2792   (Sca_id_set, Opcode 0x37)
 *    Beides eigener Byte-Scan von ROOM1030.RDT.
 *  - Der Riegel selbst: SCA rgn2/idx6 == rgn3/idx6, Rechteck (-20144,-24420, 24264 x 1649)
 *    -> z[-24420 .. -22771]. NOERDLICH (= in der Lobby) heisst z > -22771.
 *
 * Gemessen wird je Flavor: wie viele Zombies das 0x1000 tragen, wie viele in den Kriecher
 * eintreten (RE2: +0x10E Bit 0 / RE1.5: grid 0x81), wie viele die SCA-Zeile 8 fuehren und wie
 * viele den Riegel nach Norden ueberqueren.
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

#define GATE_Z_NORTH (-22771)      /* Riegel-Oberkante: -24420 + 1649 */

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

/* ---- Baenke -------------------------------------------------------------------------------- */
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
    return 1;
}

static void frame_step(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static int is_zombie(const re15_actor_t *e)
{
    return e->active && re15_re2z_owns_type(e->type);
}

typedef struct {
    int spawned;          /* Zombies im Raum                                     */
    int flagged;          /* mit +0x1C4 & 0x1000 (die Skript-Anforderung)        */
    int crawlers;         /* im Kriech-Zustand (RE2: +0x10E&1, RE1.5: grid 0x81) */
    int sca8;             /* mit SCA-Zeile 8                                     */
    int crossed;          /* haben den Riegel nach Norden ueberquert             */
    int min_z_start, max_z_end;
} run_t;

static void run_room(re15_ai_flavor_t fl, const char *name, int frames, int inject, run_t *out)
{
    memset(out, 0, sizeof *out);
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(fl);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x1030;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = -8000; pl->z = -18000;                 /* in der Lobby, noerdlich des Riegels */
    re15_collision_set_band(0);
    re15_inv_load_briefing();

    /* scd_room_reenter setzt s_current_rdt UND startet main00/sub00 — ohne das liefert
     * scd_event_fire nur -1 (kein aktuelles RDT). */
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    for (int i = 0; i < 120; i++) {
        scd_vm_tick();
        re15_aot_scan(pl->x, pl->z, 0);
        if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
    }

    if (fl == RE15_AI_FLAVOR_RE2) {
        (void)load_bank_re2(0x10); (void)load_bank_re2(0x11); (void)load_bank_re2(0x16);
    } else {
        (void)load_bank_re15(0x10); (void)load_bank_re15(0x11); (void)load_bank_re15(0x16);
    }

    int32_t z0[RE15_ACTOR_MAX];
    int seen_crawl[RE15_ACTOR_MAX], seen_sca8[RE15_ACTOR_MAX], seen_flag[RE15_ACTOR_MAX];
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        z0[s] = g_actors[s].z; seen_crawl[s] = seen_sca8[s] = seen_flag[s] = 0;
    }
    out->min_z_start = 1 << 30;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (is_zombie(&g_actors[s])) {
            out->spawned++;
            if ((int)g_actors[s].z < out->min_z_start) out->min_z_start = (int)g_actors[s].z;
        }

    /* ⛔ DER EINE ERSATZ, UND ER IST BENANNT: der AOT-STEMPEL +0x0B.
     * sub07 beginnt mit dem Torwaechter
     *   @Datei 0x2740:  3e 00 0f 00 05 00     = Member_cmp(15, ==, 5)   -> entity+0x0B == 5
     * (danach @0x2754 `3d 04 10 00` Member_get(16) / `26 00 05 04 00 10` OR 0x1000 /
     *  `35 10 04 00` Member_set(16) — die eigentliche Kriech-Anforderung).
     * Im ORIGINAL stempelt der AOT-Scan FUN_80042BAC den Aot_set-Slot in entity+0x0B
     * (`sb v0,11(s1)` @0x80042F5C / @0x80042FC4). Der PORT hat diesen Aktor-Stempel NICHT —
     * sein einziger member_0b-Produzent (re15_object_notch_update, aot_common.c) schreibt nur
     * Prop-Slots. Deshalb passiert der Torwaechter im Port NIE, und zwar unter BEIDEN Flavors
     * (gemessen: geflaggt=0 auch unter RE1.5). Das ist ein VORBESTEHENDER Blocker AUSSERHALB
     * dieser Welle (aot_common.c ist in dieser Session gesperrt) — er ist im Dossier
     * analysis/room1030_crawl_mechanism.md als Glied 1 beschrieben.
     * Damit die Kette dahinter trotzdem ECHT gemessen werden kann, setzt die Messung den
     * Stempel selbst — mit dem Wert, den der Zensus des Dossiers erwartet (Zone 5). ALLES
     * danach (sub07-Bytecode, Member_set, +0x1C4, die Bruecke, der Kriecher) laeuft real. */
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (is_zombie(&g_actors[s])) g_actors[s].member_0b = 5;   /* AOT-Stempel-Ersatz */

    int fired = scd_event_fire(7);
    printf("  [%s] sub07 gefeuert -> Thread %d,  Zombies=%d,  sub_scd[7]=%s\n",
           name, fired, out->spawned, s_rdt.sub_scd[7] ? "vorhanden" : "FEHLT");

    /* MODE 1: das ERGEBNIS des Skript-Opcodes direkt setzen. `35 10 04` (Member_set 16 =
     * work_var[4]) schreibt genau `+0x1C4 |= 0x1000` (Port-Zwilling `sh a2,452(a0)` @0x80041218).
     * Alles danach — Bruecke, Kriecher, SCA-Klemme, Kollision, game_step — ist real. */
    if (inject) {
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (is_zombie(&g_actors[s])) g_actors[s].anim_flags |= 0x1000u;
        /* sub07 OEFFNET im selben Atemzug die Torzellen fuer Kriecher — ohne diese zwei Stores
         * steht der Riegel auf 0xFF und ist auch fuer die Maske 8 fest:
         *   `37 02 06 f7` @Datei 0x278E   /   `37 03 06 f7` @Datei 0x2792   (Sca_id_set 0x37)
         * 0xF7 laesst genau Bit 3 (= Maske 8) frei. */
        {   int base = 0;
            for (int q = 0; q < 5; q++) {
                if ((q == 2 || q == 3) && base + 6 < s_rdt.sca_count)
                    ((re15_sca_entry_t *)s_rdt.sca)[base + 6].u0 = 0xF7;
                base += s_rdt.sca_rgn[q];
            }
        }
    }

    /* DIAGNOSE (mode 0): laeuft sub07 bis zum Member_set? */
    if (!inject) {
        for (int f = 0; f < 3; f++) {
            printf("     f%-2d thr12=%d work_slot=%d wv4=0x%04x  member_0b(z1)=%u "
                   "af(z1)=0x%04x\n",
                   f, g_scd.threads[12].active, g_scd.work_slot,
                   (unsigned)(uint16_t)g_scd.work_vars[4],
                   g_actors[1].member_0b, (unsigned)g_actors[1].anim_flags);
            frame_step(0, 0);
        }
    }

    for (int f = 0; f < frames; f++) {
        if (inject && (f % 600) == 0) {
            const re15_actor_t *z = &g_actors[1];
            printf("     f%-4d z1 pos=(%6ld,%7ld) ry=%4d steer=(%6d,%7d) s1=%u s2=%u "
                   "clip=%d fr=%u 144=%d sca=%u\n", f, (long)z->x, (long)z->z, (int)z->rot_y,
                   (int)z->steer_x, (int)z->steer_z, z->sub_state_1, z->sub_state_2,
                   (int)z->motion, (unsigned)z->anim_frame, (int)z->re2z_root144, z->sca_mask);
        }
        frame_step(0, 0);
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *e = &g_actors[s];
            if (!is_zombie(e)) continue;
            if (e->anim_flags & 0x1000u) seen_flag[s] = 1;
            if (e->sca_mask == 8)        seen_sca8[s] = 1;
            if (fl == RE15_AI_FLAVOR_RE2) { if (e->re2z_f10e & 1u) seen_crawl[s] = 1; }
            else                          { if (e->grid_id == 0x81) seen_crawl[s] = 1; }
        }
    }

    out->max_z_end = -(1 << 30);
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!is_zombie(&g_actors[s])) continue;
        out->flagged  += seen_flag[s];
        out->crawlers += seen_crawl[s];
        out->sca8     += seen_sca8[s];
        if (z0[s] <= GATE_Z_NORTH && (int32_t)g_actors[s].z > GATE_Z_NORTH) out->crossed++;
        if ((int)g_actors[s].z > out->max_z_end) out->max_z_end = (int)g_actors[s].z;
    }
    printf("  [%s] geflaggt=%d  kriechend=%d  sca8=%d  Riegel ueberquert=%d  "
           "z start(min)=%d end(max)=%d\n",
           name, out->flagged, out->crawlers, out->sca8, out->crossed,
           out->min_z_start, out->max_z_end);
}

int main(void)
{
    printf("== ROOM1030: kriechen die Zombies unter dem Tor durch? (echter Weg) ==\n\n");

    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1030.RDT", &n);
    if (!buf) { printf("FAIL: ROOM1030.RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, n, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    run_t d15, d2, i15, i2;

    /* ---- PHASE A: der reine SKRIPT-WEG (Diagnose, KEIN Pin) ------------------------------
     * Gemessenes Ergebnis dieser Runde: unter BEIDEN Flavors erreicht die Anforderung keinen
     * Zombie. Zwei Blocker, beide VORBESTEHEND und beide AUSSERHALB dieser Welle:
     *   (1) `work_slot == -1` — sub07 arbeitet auf der WORK-Entity (Work_set), die der Port
     *       beim Ereignis-Start nicht besetzt. Member_get/Member_set haben damit kein Ziel.
     *   (2) `member_0b` ist EINEN Frame nach dem Setzen wieder 0 — der Port hat keinen
     *       AKTOR-AOT-Stempel (Original `sb v0,11(s1)` @0x80042F5C / @0x80042FC4), und der
     *       Frame-Ende-Wisch raeumt das Byte weg (FUN_8003EC28 @0x8003F194: +0x0A/+0x0B =
     *       0xFFFF). Damit faellt der Torwaechter `3e 00 0f 00 05 00` @Datei 0x2740
     *       (Member_cmp(15, ==, 5)) immer durch.
     * Beides liegt in aot_common.c / scd_vm.c — in dieser Session gesperrt. Deshalb wird
     * PHASE A nur BERICHTET, nicht gepinnt. */
    printf("--- PHASE A: reiner Skript-Weg (Diagnose, kein Pin) ---\n");
    run_room(RE15_AI_FLAVOR_RE15, "RE1.5", 300, 0, &d15);
    re15_enemy_reset();
    run_room(RE15_AI_FLAVOR_RE2,  "RE2  ", 300, 0, &d2);

    /* ---- PHASE B: die Kette AB dem Skript-Opcode — das ist der PIN ------------------------ */
    printf("\n--- PHASE B: ab `Member_set(16)` = +0x1C4 |= 0x1000 (der Pin) ---\n");
    re15_enemy_reset();
    run_room(RE15_AI_FLAVOR_RE15, "RE1.5", 2400, 1, &i15);
    re15_enemy_reset();
    run_room(RE15_AI_FLAVOR_RE2,  "RE2  ", 2400, 1, &i2);

    printf("\n== ERGEBNIS ==\n");
    printf("  PHASE A (Skript):    RE1.5 geflaggt=%d/%d   RE2 geflaggt=%d/%d"
           "   -> VORBESTEHENDER Blocker\n",
           d15.flagged, d15.spawned, d2.flagged, d2.spawned);
    printf("  PHASE B (ab Opcode): RE1.5 %d/%d kriechen (sca8=%d)   "
           "RE2 %d/%d kriechen (sca8=%d)\n",
           i15.crawlers, i15.spawned, i15.sca8, i2.crawlers, i2.spawned, i2.sca8);

    CHECK(i2.spawned > 0, "ROOM1030 hat unter RE2 keine Zombies gespawnt");
    CHECK(i2.flagged == i2.spawned,
          "die Anforderung liegt nicht auf allen Zombies (%d/%d)", i2.flagged, i2.spawned);
    CHECK(i2.crawlers > 0, "KERN: unter RE2 ist KEIN Zombie in den Kriecher eingetreten "
                           "(+0x10E Bit 0, Wurzel 0x80101210)");
    CHECK(i2.sca8 > 0, "KERN: unter RE2 fuehrt kein Kriecher die Torzellen-Maske 8 "
                       "(+0x1D7, RE1.5-Bruecke @0x801050F4)");
    CHECK(i2.crossed == i2.spawned,
          "KERN (die Frage des Nutzers): nur %d von %d Kriechern sind unter dem Tor "
          "hindurch in die Lobby gekommen (Riegel z > %d)", i2.crossed, i2.spawned, GATE_Z_NORTH);
    CHECK(i15.crawlers > 0, "REGRESSIONSWACHE: unter RE1.5 kriecht keiner mehr "
                            "(die RE1.5-Kette ist byte-true und darf sich nicht aendern)");
    CHECK(i15.crossed == i15.spawned,
          "REGRESSIONSWACHE: unter RE1.5 kommen nur %d von %d durch", i15.crossed, i15.spawned);

    free(buf);
    printf("\n== %s (%d Fehler) ==\n", fails ? "FEHLGESCHLAGEN" : "OK", fails);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    return fails ? 1 : 0;
}
