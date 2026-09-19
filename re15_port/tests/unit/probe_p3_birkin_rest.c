/* probe_p3_birkin_rest.c — PIN (ctest) fuer die Nacharbeit an den offenen Punkten des
 * Birkin-Endkampfs (analysis/befunde_2026-09-19/birkin-g5.md §7).
 *
 * Gefahren wird derselbe ECHTE Ablauf wie in probe_p2_birkin_g5.c (ROOM5090.RDT -> SCD-VM ->
 * re15_game_step -> re15_enemy_ai_run_all mit den RE2-Baenken EM036/EM037).
 *
 *  A) TIMER-KANTE der Intro-Sender: [T4]/[T5]/[T10] lesen +0x158 VOR dem +1
 *     (@0x801013a4 / @0x80101430ff / @0x80101640ff; erhoeht wird erst @0x801013e8 /
 *     @0x801014c8 / @0x801016d8). Gemessen wird der Stand des Boss-Zeitgebers in dem Bild,
 *     in dem der Arm sein Kommando bekommt: er muss um 1 HOEHER sein als die Sendemarke
 *     (20 -> 21), weil die Erhoehung erst nach dem Senden kommt. [T1] dagegen erhoeht
 *     ZUERST (@0x80101214-2c) — dort ist die Marke selbst der Stand.
 *  B) PHASENBYTE statt Routine-Wort: [T4] t==30 setzt alle vier Arme auf Phase 8
 *     (`sb v0=8,6(a2)` @0x801013bc/c8/d4/e0); der Arm-Zeitgeber +0x158 laeuft dabei
 *     ungebrochen weiter (ph4/ph5/ph6 arbeiten alle auf ihm, @0x80101e24/@0x80101e90/
 *     @0x80101e94, Erhoehung @0x80101f9c).
 *  C) GRIFF-OPFERMASCHINEN: die Treffer der Arme starten die Spieler-Routine 5
 *     (0x800CE300[0x37] = 0x80104288 @0x801006fc, Variantentabelle @0x8010582c). Gepinnt:
 *     es laeuft ueberhaupt eine Maschine, sie benutzt NUR Clips der 0x37-Opferbank
 *     (6 Clips, EMD dir[5]/dir[6]), der Spieler steht dabei im Opfer-Modus 4, und jede
 *     Maschine endet wieder (kein Dauergriff).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_fade.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int  re15_g5_boss_zustand(int *sub, int *ph, int *timer);
extern int  re15_g5_tentakel_zustand(int idx, int *sub, int *ph, int *ankermodus,
                                     int32_t *scale_x, uint16_t *kontakt);
extern int  re15_g5_opfer_variante(void);
extern int  re15_g5_opfer_phase(void);
extern void re15_g5_opfer_start(re15_actor_t *pl, int variante);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_fail = 0;

#define PIN(cond, ...) do { if (!(cond)) { s_fail++; printf("  PIN FEHLT: " __VA_ARGS__); printf("\n"); } \
                            else { printf("  PIN ok: " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int load_re2_banks(void)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
    if (!ems) return 0;
    for (int k = 0; k < 2; k++) {
        uint8_t type = k ? 0x37u : 0x36u;
        re15_enemy_bank_t *eb = re15_enemy_find(type);
        if (!eb) eb = re15_enemy_alloc(type);
        if (!eb) return 0;
        if (re2_ems_load_bank(ems, n, type, eb, NULL) != 0) return 0;
        eb->buf = NULL; eb->ok = 1;
    }
    return 1;
}

static int s_cine_was_active = 0;
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

static int find_boss(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x36) return s;
    return -1;
}

int main(int argc, char **argv)
{
    size_t rsz = 0;
    (void)argc; (void)argv;
    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE5/ROOM5090.RDT", &rsz);
    if (!raw) { printf("FEHLT: ROOM5090.RDT\n"); return 77; }
    if (re15_rdt_parse(raw, rsz, &s_rdt) != 0) { printf("FEHLT: RDT-Parse\n"); return 77; }

    printf("=== P3 birkin-rest PIN: Timer-Kante / Phasenbyte / Griff-Opfermaschinen ===\n");

    memset(&s_cam, 0, sizeof s_cam);
    memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 14;

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x5090; g_room_change.pending = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = 25600; pl->y = 0; pl->z = -23350; pl->rot_y = 1024;
    re15_collision_set_band(0);

    if (!load_re2_banks()) { printf("FEHLT: RE2-Baenke (CDEMD0.EMS)\n"); return 77; }
    {   re15_enemy_bank_t *vb = re15_enemy_find(0x37u);
        printf("  EM037-Opferbank: victim_ok=%d, %d Clips (EMD dir[5]/dir[6])\n",
               vb ? (int)vb->victim_ok : -1,
               (vb && vb->victim_ok) ? vb->anim_victim.clip_count : -1);
        PIN(vb && vb->victim_ok && vb->anim_victim.clip_count == 6,
            "EM037 hat eine EIGENE Opferbank mit 6 Clips (kein Leihgeber noetig)");
    }

    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 14);
    g_scd.cut_auto_enabled = 1;
    s_shown = 14;

    for (int f = 0; f < 30; f++) frame();
    int trigger_f = -1;
    for (int f = 30; f < 500; f++) {
        int32_t nx = pl->x - 75, nz = pl->z;
        re15_collision_set_band(0);
        re15_collision_constrain(&s_rdt, pl->x, pl->z, &nx, &nz);
        pl->x = nx; pl->z = nz;
        frame();
        if (trigger_f < 0 && s_shown == 12) trigger_f = f;
        if (trigger_f >= 0 && f > trigger_f + 3) break;
    }
    int bslot = find_boss();
    if (bslot < 0) { printf("FEHLT: kein Boss\n"); return 77; }
    re15_actor_t *e = &g_actors[bslot];

    /* ---------------- A/B) Intro: Sender-Kante + Phasenbyte ----------------------------- */
    printf("-- A/B) Intro-Sender --\n");
    int t5_boss_timer = -1, t10_boss_timer = -1, t4_boss_timer = -1;
    int t1_boss_timer = -1;
    int ph6_frames = 0, ph6_gesehen = 0, ph6_ende_timer = -1;
    int arm_sub_prev[4] = { -1, -1, -1, -1 }, arm_ph_prev[4] = { -1, -1, -1, -1 };
    int alle_ph8 = 0, ph8_gesehen[4] = { 0, 0, 0, 0 };
    for (int f = 0; f < 1500; f++) {
        frame();
        int bsub = -1, bph = -1, bt = -1;
        re15_g5_boss_zustand(&bsub, &bph, &bt);
        int n8 = 0;
        for (int i = 0; i < 4; i++) {
            int sub = -1, ph = -1;
            if (re15_g5_tentakel_zustand(i, &sub, &ph, NULL, NULL, NULL) != 0) continue;
            if (i == 0) {
                if (sub == 8 && ph != 0 && arm_ph_prev[0] == 0 && t1_boss_timer < 0)
                    t1_boss_timer = bt;                 /* [T1] t==10 -> Phase 1 (die Phase
                                                         * selbst haelt kein Bild: ph1 setzt
                                                         * sofort ph2 @0x80101d90-ac) */
                if (sub == 9 && arm_sub_prev[0] != 9 && t5_boss_timer < 0)
                    t5_boss_timer = bt;                 /* [T5] t==20 -> 0x901  */
                if (sub == 10 && arm_sub_prev[0] != 10 && t10_boss_timer < 0)
                    t10_boss_timer = bt;                /* [T10] t==20 -> 0xA01 */
                if (sub == 8 && ph == 6) { ph6_gesehen = 1; ph6_frames++; }
                if (sub == 8 && ph == 7 && arm_ph_prev[0] == 6 && ph6_ende_timer < 0)
                    ph6_ende_timer = ph6_frames;
            }
            /* [T4] t==30: ALLE VIER verlassen im selben Bild die Warte-Phase 7. Arm 1
             * wurde in [T1] nie ausgefahren (nur 0/2/3 bekommen Phase 1, @0x80101240/
             * @0x80101268/@0x80101290), seine Streckung ist 0 — er faellt in Phase 8
             * sofort nach Phase 9 durch (@0x80101fa8-...) und ist deshalb nie MIT Phase 8
             * zu sehen. Gemessen wird darum der AUSTRITT aus Phase 7. */
            if (arm_ph_prev[i] == 7 && ph != 7 && ph8_gesehen[i] == 0) {
                ph8_gesehen[i] = 1;
                if (t4_boss_timer < 0) t4_boss_timer = bt;
            }
            if (sub == 8 && ph == 8) n8++;
            arm_sub_prev[i] = sub; arm_ph_prev[i] = ph;
        }
        (void)n8;
        alle_ph8 = ph8_gesehen[0] && ph8_gesehen[1] && ph8_gesehen[2] && ph8_gesehen[3];
        if (e->motion == 5) break;                      /* Intro fertig (Clip 5 = sub1) */
    }
    printf("  Boss-Zeitgeber beim Kommando: [T1]->Ph1 %d, [T4]->Ph8 %d, [T5]->sub9 %d, [T10]->sub10 %d\n",
           t1_boss_timer, t4_boss_timer, t5_boss_timer, t10_boss_timer);
    printf("  Arm 0 in sub8 Phase 6: %d Bilder (Zittern 0..180)\n", ph6_ende_timer);
    PIN(t1_boss_timer == 10, "[T1] erhoeht ZUERST (@0x80101214-2c): Stand 10 beim Wecken (ist %d)",
        t1_boss_timer);
    PIN(t4_boss_timer == 31, "[T4] liest VOR dem +1 (@0x801013a4, Erhoehung @0x801013e8): Stand"
                             " 31 nach der Marke 30 (ist %d)", t4_boss_timer);
    PIN(t5_boss_timer == 21, "[T5] liest VOR dem +1 (@0x80101430, Erhoehung @0x801014c8): Stand"
                             " 21 nach der Marke 20 (ist %d)", t5_boss_timer);
    PIN(t10_boss_timer == 21, "[T10] liest VOR dem +1 (@0x80101640, Erhoehung @0x801016d8):"
                              " Stand 21 nach der Marke 20 (ist %d)", t10_boss_timer);
    PIN(alle_ph8, "[T4] t==30 holt ALLE VIER aus der Warte-Phase 7 (`sb v0=8,6(a2)`"
        " @0x801013bc/c8/d4/e0): %d/%d/%d/%d", ph8_gesehen[0], ph8_gesehen[1], ph8_gesehen[2],
        ph8_gesehen[3]);
    PIN(ph6_gesehen && ph6_ende_timer == 180,
        "sub8 Phase 6 laeuft ueber +0x158 (@0x80101e94-f9c, in Phase 5 genullt @0x80101e90):"
        " 180 Bilder mit sichtbarer Phase 6 — im Bild mit Zaehlerstand 180 steht schon Phase 7"
        " (ist %d)", ph6_ende_timer);

    /* ---------------- C) Griff-Opfermaschinen ------------------------------------------- */
    printf("-- C) Griff-Opfermaschinen (Spieler-Routine 5) --\n");
    int var_hist[5] = { 0, 0, 0, 0, 0 };
    int starts = 0, ende = 0, aktiv_frames = 0, clip_max = -1, clip_bad = 0;
    int victim4_frames = 0, laengste = 0, lauf = 0;
    int prev_var = -1;
    /* Zwei Lagen wie in probe_p2_birkin_g5: 6500 oestlich (Peitsche/Zug) und 9000 oestlich
     * (Spiess 0xD01 — der wird erst ab dist >= 7000 gewuerfelt, `sltiu 0x1b58` @0x80100a78). */
    for (int f = 0; f < 8000; f++) {
        int32_t abstand = (f < 4000) ? 6500 : 9000;
        pl->x = e->x + abstand; pl->z = e->z; pl->y = 0;
        pl->hp = 100;
        if (re15_g5_opfer_variante() < 0) pl->hit_react = 0;
        frame();
        int v = re15_g5_opfer_variante();
        if (v >= 0 && prev_var < 0) { starts++; if (v < 5) var_hist[v]++; lauf = 0; }
        if (v < 0 && prev_var >= 0) { ende++; if (lauf > laengste) laengste = lauf; }
        if (v >= 0) {
            aktiv_frames++; lauf++;
            if (re15_player_victim_state() == 4) victim4_frames++;
            if (v >= 2) {                       /* 2/3/4 posieren aus der Opferbank */
                int c = (int)pl->motion;
                if (c > clip_max) clip_max = c;
                if (c < 0 || c > 5) clip_bad++;
            }
        }
        prev_var = v;
    }
    printf("  8000 Bilder (4000 bei 6500, 4000 bei 9000): %d Griffe gestartet, %d beendet, %d Bilder aktiv (laengster Lauf %d);"
           " Varianten 0=%d 1=%d 2=%d 3=%d 4=%d; groesster Opferclip %d, ausserhalb 0..5: %d;"
           " Opfer-Modus 4 in %d Bildern\n",
           starts, ende, aktiv_frames, laengste, var_hist[0], var_hist[1], var_hist[2],
           var_hist[3], var_hist[4], clip_max, clip_bad, victim4_frames);
    PIN(starts > 0, "die Treffer starten eine Opfermaschine (ist %d in 8000 Bildern)", starts);
    PIN(ende >= starts - 1, "jede Maschine endet wieder (gestartet %d, beendet %d)", starts, ende);
    PIN(clip_bad == 0, "die Varianten 2/3/4 spielen NUR Clips der 6er-Opferbank (%d Ausreisser)",
        clip_bad);
    PIN(victim4_frames > 0 || var_hist[0] + var_hist[1] == starts,
        "waehrend einer Opferbank-Maschine steht der Spieler im Opfer-Modus 4 (%d Bilder)",
        victim4_frames);
    PIN(laengste < 600, "kein Dauergriff (laengster Lauf %d Bilder)", laengste);

    /* Die drei Opferbank-Maschinen (2/3/4) kommen in DIESER Lage nicht vor (der Boss
     * entscheidet sich hier fuer den Zug); gefahren werden sie deshalb direkt am Eingang
     * der Griff-Stelle — dieselbe Funktion, die Peitsche und Spiess aufrufen. Geprueft wird
     * die CLIP-FOLGE der Maschine, weil genau sie die Opferhaltung ist:
     *   Variante 2 (@0x80104454): 2 -> 0 -> 1 -> Ende (@0x801044a0/@0x80104584/@0x801045e0)
     *   Variante 3 (@0x801046ac): 4 -> 3 -> 1 -> Ende (@0x801046fc/@0x801047a8/@0x80104830)
     *   Variante 4 (@0x801048f4): 5 -> 3 -> 1 -> Ende (@0x80104944/@0x801049f0/@0x80104aa4) */
    for (int var = 2; var <= 4; var++) {
        int folge[8]; int nf = 0, letzter = -1, frames = 0;
        pl->hp = 100; pl->hit_react = 0; pl->re2z_self1d3 = 0;
        re15_g5_opfer_start(pl, var);
        for (int f = 0; f < 600 && re15_g5_opfer_variante() >= 0; f++) {
            pl->x = e->x + 6500; pl->z = e->z; pl->y = 0; pl->hp = 100;
            frame();
            frames++;
            if (re15_g5_opfer_variante() >= 0 && re15_player_victim_state() == 4) {
                int c = (int)pl->motion;
                if (c != letzter && nf < 8) { folge[nf++] = c; letzter = c; }
            }
        }
        printf("  Variante %d: %d Bilder, Clipfolge", var, frames);
        for (int i = 0; i < nf; i++) printf(" %d", folge[i]);
        printf(" (Ende: var=%d)\n", re15_g5_opfer_variante());
        {   const int soll2[3] = { 2, 0, 1 }, soll3[3] = { 4, 3, 1 }, soll4[3] = { 5, 3, 1 };
            const int *soll = (var == 2) ? soll2 : (var == 3) ? soll3 : soll4;
            int ok = (nf == 3) && folge[0] == soll[0] && folge[1] == soll[1] && folge[2] == soll[2];
            PIN(ok, "Variante %d spielt die Opferclips %d -> %d -> %d (ist %d Stufen)",
                var, soll[0], soll[1], soll[2], nf);
            PIN(re15_g5_opfer_variante() < 0, "Variante %d endet wieder (nach %d Bildern)",
                var, frames);
        }
    }

    printf("=== %s (%d Pins verletzt) ===\n", s_fail ? "FEHLER" : "OK", s_fail);
    return s_fail ? 1 : 0;
}
