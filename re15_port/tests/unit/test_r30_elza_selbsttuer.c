/* test_r30_elza_selbsttuer.c — RIEGEL Runde 30, Thema G (Elza-Intro), Teil (ii): Engine ohne Fenster.
 *
 * Dossier: analysis/befunde_runde30/elza-intro.md (§3.3, §3.5, §4.3, §5.3, §5.4 (ii)).
 * Herkunft: probe_r30_elza-intro.c Teil M2 (die Messsonde bleibt daneben bestehen).
 *
 * NUTZER-BEFUND (AUFTRAG.md Abschnitt G): "nach dem Intro stehe ich in der Lobby ... und ihre
 * Cutscene spielt nicht ab wie sie soll."
 *
 * WAS GEPRUEFT WIRD: ROOM1031 (Elzas Lobby, Variante 1) im Zustand des Originals nach dem
 * Erstbesuch — flag(3,193)=1 (ROOM1031 sub12 @0x02976 `22 03 c1 01`), flag(3,125)=0. main00
 * nimmt dann den Erzaehler-Zweig:
 *     @0x02076  07 00 34 00   Else_ck
 *     @0x0207E  21 03 7d 00   Ck(3,125,0)
 *     @0x02082  3b 13 02 31 .. f8 df 00 00 1c a8 00 04 00 03 06
 *               Door_aot_set Slot 19 -> Stage 0 / Raum 0x03 (= ROOM1031 SELBST), Cut 6
 *     @0x020A2  04 ff 18 0f   Evt_exec sub15 (Set(3,125,1) @0x02A68, Set(3,207,1) @0x02A6C,
 *               Set(2,7,1) @0x02A70, Cut_chg 13, Message_on 0x14..0x17, Aot_on(19) @0x02A9E)
 * Die Selbst-Tuer muss den Raum NEU EINSTEIGEN — im Original tut das jede Tuer:
 *     FUN_8001d600  8001d968  beq v1,v0,0x8001d988   ; nur die STAGE wird verglichen
 *                   8001d988  jal 0x800396fc         ; Raumlader, unbedingt
 *     FUN_800396fc  800397e4  srl a0,a0,31 / 800397ec addu a0,a0,v0   ; Datei = Basis + Elza-Bit
 *                   80039a00  jal 0x8003ef6c         ; SCD-Raum-Init (main00 + sub00 neu)
 * Beim dritten main00-Durchlauf startet Ck(3,207,1) @0x020AE -> Evt_exec sub13 @0x020B2 =
 * Elzas Lobby-Szene; sub13 loescht (3,207) sofort (@0x02982 `22 03 cf 00`) und schliesst am
 * Ende das Szenenfenster (Set(2,7,0) @0x02A4A, Set(1,27,0) @0x02A4E, Plc_ret @0x02A56).
 *
 * VORHER (Auslieferungsstand 8d83a025, gemessen mit der Sonde, M2): Selbst-Tuer in Bild 440,
 * Neueinstieg=0, Akteure=0, flag(3,207)=1, flag(2,7)=1, pmode=2 — der Spieler steht fest.
 * Ursache: aot_common.c verglich (0x1000 | raum<<4) = 0x1030 gegen 0x1031.
 *
 * HAKEN:
 *   H0  die Belegbytes stehen in der ausgelieferten ROOM1031.RDT (sonst misst der Haken nichts).
 *   H1  der Raum wird waehrend des ganzen Laufs NICHT verlassen (kein Wechsel nach 0x1241 —
 *       der Erstbesuchs-Zweig sub12 waere genau das).
 *   H2  die Selbst-Tuer feuert (Spieler auf next_pos X = -8200 ODER Neueinstieg gemeldet)
 *       innerhalb von 700 Bildern.
 *   H3  in DEMSELBEN Bild ist g_scd_self_reenter_fired == 1 (der Defekt: 0).
 *   H4  flag(3,207) und flag(2,7) fallen innerhalb von 600 Bildern nach der Tuer auf 0
 *       (gemessen in der Gegenprobe M3 des Dossiers: 550 Bilder) = sub13 ist durchgelaufen.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_fade.h"
#include "re15_gameflow.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t         g_scd;

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was_active = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* Bild-Ablauf wie probe_r30_elza-intro.c / probe_1090_gate_selfdoor.c:
 * scd_vm_tick -> Walker -> cine/pmode -> re15_msg_tick -> re15_cam_present_tick -> re15_game_step. */
static void frame(void)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    {
        int cine_active = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
        re15_letterbox_tick(re15_game_flag_get(1, 27));
        if (cine_active) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was_active) { g_scd.letterbox_countdown = 15; }
        s_cine_was_active = cine_active;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
            g_scd.player_mode = 0;
            re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                               g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        }
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = 0;
    s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
}

static int gegner_aktiv(void)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active) n++;
    return n;
}

static void zeile(const char *tag, int f)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    printf("%-7s f%-4d pos=(%6ld,%5ld,%6ld) cam=%2u | f3.193=%d f3.125=%d f3.207=%d f2.7=%d "
           "f1.27=%d pmode=%d | akteure=%d reenter=%d wechsel=%d->%04x\n",
           tag, f, (long)pl->x, (long)pl->y, (long)pl->z, (unsigned)g_scd.cam_id,
           re15_game_flag_get(3, 193), re15_game_flag_get(3, 125), re15_game_flag_get(3, 207),
           re15_game_flag_get(2, 7), re15_game_flag_get(1, 27), (int)g_scd.player_mode,
           gegner_aktiv(), g_scd_self_reenter_fired,
           g_room_change.pending, (unsigned)g_room_change.room_id);
}

/* Eintritt wie re15_room_apply_pending: Spawn + Band + Eintritts-Cut; die Flags bleiben stehen
 * (der Raumlader des Originals fasst die Story-Baenke nicht an, Dossier §3.1). */
static void betrete_1031(int32_t x, int32_t y, int32_t z, int yaw, int entry_cut)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    s_cine_was_active = 0;
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = 0x1031; g_room_change.pending = 0; g_room_change.room_id = 0;
    g_scd_pending_scenario = -1; g_scd_self_reenter_fired = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = x; pl->y = y; pl->z = z; pl->rot_y = (int16_t)yaw;
    re15_collision_set_band(re15_collision_band_from_y(pl->y));
    s_shown = entry_cut;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, (uint8_t)entry_cut);
    g_scd.cut_auto_enabled = 1;
}

static int bytes_gleich(const uint8_t *raw, size_t n, size_t off, const uint8_t *soll, size_t k,
                        const char *was)
{
    if (off + k > n || memcmp(raw + off, soll, k) != 0) {
        fprintf(stderr, "FAIL(H0): ROOM1031.RDT @0x%05lX (%s) weicht vom Beleg ab\n",
                (unsigned long)off, was);
        return 0;
    }
    return 1;
}

int main(void)
{
    int fail = 0;
    char rp[600];
    size_t rawsz = 0;
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM1031.RDT", RE15_ASSET_PSX_DIR);
    uint8_t *raw = slurp(rp, &rawsz);
    if (!raw) { fprintf(stderr, "FAIL: %s nicht lesbar\n", rp); return 1; }
    if (re15_rdt_parse(raw, rawsz, &s_rdt) < 0) { fprintf(stderr, "FAIL: RDT-Parse\n"); return 1; }
    printf("=== Riegel r30 Elza: Selbst-Tuer ROOM1031 Slot 19 steigt neu ein ===\n");

    /* ---- H0  Belegbytes ---- */
    {
        static const uint8_t ck193[]  = { 0x21, 0x03, 0xc1, 0x00 };   /* @0x0204E */
        static const uint8_t ck125[]  = { 0x21, 0x03, 0x7d, 0x00 };   /* @0x0207E */
        static const uint8_t door19[] = { 0x3b, 0x13 };               /* @0x02082 */
        static const uint8_t ziel19[] = { 0x00, 0x03, 0x06 };         /* @0x02098 Stage/Raum/Cut */
        static const uint8_t evt15[]  = { 0x04, 0xff, 0x18, 0x0f };   /* @0x020A2 */
        static const uint8_t ck207[]  = { 0x21, 0x03, 0xcf, 0x01 };   /* @0x020AE */
        static const uint8_t evt13[]  = { 0x04, 0xff, 0x18, 0x0d };   /* @0x020B2 */
        static const uint8_t aot19[]  = { 0x47, 0x13 };               /* @0x02A9E */
        int ok = 1;
        ok &= bytes_gleich(raw, rawsz, 0x0204E, ck193, 4, "Ck(3,193,0)");
        ok &= bytes_gleich(raw, rawsz, 0x0207E, ck125, 4, "Ck(3,125,0)");
        ok &= bytes_gleich(raw, rawsz, 0x02082, door19, 2, "Door_aot_set Slot 19");
        ok &= bytes_gleich(raw, rawsz, 0x02098, ziel19, 3, "Slot 19 Ziel Stage 0/Raum 0x03/Cut 6");
        ok &= bytes_gleich(raw, rawsz, 0x020A2, evt15, 4, "Evt_exec sub15");
        ok &= bytes_gleich(raw, rawsz, 0x020AE, ck207, 4, "Ck(3,207,1)");
        ok &= bytes_gleich(raw, rawsz, 0x020B2, evt13, 4, "Evt_exec sub13");
        ok &= bytes_gleich(raw, rawsz, 0x02A9E, aot19, 2, "Aot_on(19)");
        printf("H0  Belegbytes ROOM1031.RDT: %s\n", ok ? "ok" : "ABWEICHEND");
        if (!ok) fail = 1;
    }

    /* ---- Zustand herstellen: Elza, Neues Spiel, nach der Montage ---- */
    scd_vm_init();
    g_gameflow.character = 4;                                  /* Elza, @0x801024cc/@0x801024d4 */
    g_scd.work_vars[0x10] = (int16_t)(g_gameflow.character & 0x0F);   /* @0x8001d558 */
    re15_inv_load_briefing();
    {
        static const uint8_t b3[] = { 193, 125, 207, 111, 112 };
        for (unsigned i = 0; i < sizeof b3; i++) re15_game_flag_set(3, b3[i], 0);
        re15_game_flag_set(2, 7, 0); re15_game_flag_set(1, 27, 0); re15_game_flag_set(4, 15, 0);
    }
    re15_game_flag_set(3, 111, 1);                  /* ROOM1241 sub02 @0x055A */
    re15_game_flag_set(3, 112, 1);                  /* ROOM1241 sub02 @0x055E */
    re15_game_flag_set(3, 193, 1);                  /* ROOM1031 sub12 @0x02976 (Vorlauf, main.c) */
    betrete_1031(-26214, 0, -3861, 0, 0);           /* Payload ROOM1241 Slot 0 @0x051A */
    zeile("[ein]", 0);

    /* ---- H1/H2/H3  bis zur Selbst-Tuer ---- */
    int ft = -1, verlassen = 0;
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int f = 1; f <= 700; f++) {
        frame();
        if (g_room_change.pending) { zeile("[WECHS]", f); verlassen = 1; break; }
        /* Die Tuer setzt den Spieler auf next_pos X = -8200 (@0x02082+14 `f8 df`). Steigt der
         * Raum im selben Bild neu ein, kann sub13 ihn schon umgesetzt haben (Member_set
         * @0x02994) — deshalb zaehlt auch die Neueinstiegs-Meldung als Tuer-Bild. */
        if (pl->x == -8200 || g_scd_self_reenter_fired) { ft = f; zeile("[TUER]", f); break; }
        if ((f % 100) == 0) zeile("[..]", f);
    }
    if (verlassen) {
        fprintf(stderr, "FAIL(H1): ROOM1031 fordert einen Raumwechsel nach %04X an "
                        "(Erstbesuchs-Zweig sub12?)\n", (unsigned)g_room_change.room_id);
        fail = 1;
    }
    if (!verlassen && ft < 0) {
        fprintf(stderr, "FAIL(H2): die Selbst-Tuer Slot 19 hat in 700 Bildern nicht gefeuert\n");
        fail = 1;
    }
    if (ft > 0 && !g_scd_self_reenter_fired) {
        fprintf(stderr, "FAIL(H3): Selbst-Tuer in Bild %d, aber KEIN Neueinstieg "
                        "(g_scd_self_reenter_fired=0) — main00 laeuft nicht zum dritten Mal, "
                        "sub13 (Elzas Szene) startet nie (@0x8001d988 jal 0x800396fc)\n", ft);
        fail = 1;
    }

    /* ---- H4  sub13 laeuft durch ---- */
    int ende = -1;
    if (ft > 0 && g_scd_self_reenter_fired) {
        for (int f = ft + 1; f <= ft + 600; f++) {
            frame();
            if (g_room_change.pending) {
                zeile("[WECHS]", f);
                fprintf(stderr, "FAIL(H1): Raumwechsel waehrend der Szene nach %04X\n",
                        (unsigned)g_room_change.room_id);
                fail = 1;
                break;
            }
            if (((f - ft) % 100) == 0) zeile("[szene]", f);
            if (re15_game_flag_get(3, 207) == 0 && re15_game_flag_get(2, 7) == 0) {
                ende = f; zeile("[ENDE]", f);
                break;
            }
        }
        if (ende < 0) {
            fprintf(stderr, "FAIL(H4): flag(3,207)=%d / flag(2,7)=%d stehen 600 Bilder nach der "
                            "Tuer noch — sub13 ist nicht durchgelaufen (@0x02982 / @0x02A4A)\n",
                    re15_game_flag_get(3, 207), re15_game_flag_get(2, 7));
            fail = 1;
        }
    }
    printf("ERGEBNIS: Tuer-Bild %d, Neueinstieg=%d, Szenenende Bild %d (%d nach der Tuer), "
           "Akteure=%d\n", ft, g_scd_self_reenter_fired, ende, ende > 0 ? ende - ft : -1,
           gegner_aktiv());

    free(raw);
    if (fail) { fprintf(stderr, "test_r30_elza_selbsttuer: FEHLER\n"); return 1; }
    printf("test_r30_elza_selbsttuer OK\n");
    return 0;
}
