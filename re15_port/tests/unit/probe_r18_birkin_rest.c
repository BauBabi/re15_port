/* ============================================================================================
 * probe_r18_birkin_rest.c — Runde 18 (2026-09-21): die drei Restluecken des G5-Endkampfs
 * Dossier: analysis/befunde_2026-09-21/birkin-restluecken.md
 *
 *   A) RUMBLE-RINGE   — byte-treuer Zwilling von FUN_800396FC @0x800396fc + der drei
 *                       Alloc-Funktionen @0x8003947c / @0x80039514 / @0x800395b8.
 *                       Gepinnt wird die EXAKTE Aktuator-Kurve, Bild fuer Bild.
 *   B) BLUT-SPAWNS    — die vier FUN_8001BF10-Aufrufe der Trefferroutine
 *                       (@0x801027ec/@0x80102848/@0x801028f0/@0x80102928).
 *   C) OPFERMASCHINE  — Spieler-Routine 6 (0x80103908): Clip 0, Blend-Zaehler 7,
 *                       Drehen mit 256/Bild auf Boss-Yaw+2048, Schnapp bei Bild 8.
 *
 * Aufruf:  probe_r18_birkin_rest [pin]
 * Rueckgabe 77 = SKIP (Assets fehlen), 0 = alle Pins gehalten, 1 = Pin gerissen.
 * ========================================================================================== */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_fade.h"
#include "re15_esp.h"
#include "re15_rumble.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int  re15_g5_boss_zustand(int *sub, int *ph, int *timer);
extern int  re15_g5_devour_opfer_phase(void);
extern int  re15_g5_blut_kamera(int32_t *yaw, int32_t *dirx, int32_t *dirz);
extern int  re15_g5_blut_zaehler(int idx);
extern int32_t re15_g5_blut_diag(int idx);
extern int32_t re15_g5_tent_dreh_mess(int idx);
extern void re15_g5_devour_opfer_start(re15_actor_t *pl, const re15_actor_t *e);

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

/* ---------------------------------------------------------------------------------------- */
/* A) RUMBLE — reine Ring-Arithmetik, braucht keinen Raum.                                    */
/* ---------------------------------------------------------------------------------------- */
static void teil_a_rumble(void)
{
    uint8_t a0, a1;
    int f;

    printf("-- A) Rumble-Ringe (FUN_800396FC @0x800396fc) --\n");

    /* A1: kleiner Motor, 3 Bilder, kein Vorlauf (FUN_8003947C @0x8003947c). Slot[1] = 1,
     *     Schritt 0 -> der Tick ueberschreibt nie -> [1,1,1,0]. */
    re15_rumble_reset();
    re15_rumble_small(3, 0);
    {
        int ok = 1;
        for (f = 1; f <= 4; f++) {
            re15_rumble_tick(&a0, &a1);
            if (a0 != (f <= 3 ? 1 : 0)) ok = 0;
            if (a1 != 0) ok = 0;
        }
        PIN(ok, "small(3,0) liefert Aktuator0 = 1,1,1,0 (`sb v0,1(v1)` @0x800394cc,"
                " Schritt 0 @0x800394e4)");
    }

    /* A2: Vorlauf. small(1,120) -> erst Bild 121 zieht an (`lhu v0,1(a1)` @0x8003971c,
     *     `j 0x80039794` @0x8003972c ueberspringt das Maximum). */
    re15_rumble_reset();
    re15_rumble_small(1, 120);
    {
        int an = -1;
        for (f = 1; f <= 130; f++) {
            re15_rumble_tick(&a0, &a1);
            if (a0 && an < 0) an = f;
        }
        PIN(an == 121, "small(1,120) zieht erst auf Bild 121 an (gemessen %d)", an);
    }

    /* A3: grosser Motor konstant (FUN_80039514 @0x80039514). */
    re15_rumble_reset();
    re15_rumble_large(5, 200, 0);
    {
        int ok = 1;
        for (f = 1; f <= 6; f++) {
            re15_rumble_tick(&a0, &a1);
            if (a1 != (f <= 5 ? 200 : 0)) ok = 0;
        }
        PIN(ok, "large(5,200,0) haelt Aktuator1 = 200 fuer genau 5 Bilder");
    }

    /* A4: RAMPE (FUN_800395B8 @0x800395b8). Schritt = ((0-180)*0x80)/150 = -153,
     *     Akku startet 180<<7 = 23040. Erstes nicht-verzoegertes Bild ist 251:
     *     (23040-153)>>7 = 178. Letztes (Bild 400): (23040-150*153)>>7 = 90>>7 = 0. */
    re15_rumble_reset();
    re15_rumble_ramp(150, 180, 0, 250);
    {
        uint8_t bei250 = 0xFF, bei251 = 0xFF, bei400 = 0xFF;
        for (f = 1; f <= 401; f++) {
            re15_rumble_tick(&a0, &a1);
            if (f == 250) bei250 = a1;
            if (f == 251) bei251 = a1;
            if (f == 400) bei400 = a1;
        }
        PIN(bei250 == 0,   "ramp(150,180,0,250): Bild 250 noch still (%u)", bei250);
        PIN(bei251 == 178, "ramp(150,180,0,250): Bild 251 = 178 = (23040-153)>>7 (%u)", bei251);
        PIN(bei400 == 0,   "ramp(150,180,0,250): Bild 400 = 0, die Rampe laeuft aus (%u)", bei400);
    }

    /* A5: KASKADE 1 (Todes-Uebergang 0->1, @0x80103104-@0x801031b8) — 10 Slots in Ring B,
     *     Bild 1 = der Grundton 180, Bild 21 = erster Puls-Schritt
     *     ((180<<7)+512)>>7 = 184, und das Maximum gewinnt (`sltu v0,a3,v1` @0x80039784). */
    re15_rumble_reset();
    re15_rumble_large(250, 180, 0);
    re15_rumble_ramp(150, 180, 0, 250);
    re15_rumble_ramp(10, 180, 220, 20);
    re15_rumble_ramp(10, 220, 180, 30);
    re15_rumble_ramp(10, 180, 220, 40);
    re15_rumble_ramp(10, 220, 180, 50);
    re15_rumble_ramp(10, 180, 220, 65);
    re15_rumble_ramp(10, 220, 180, 75);
    re15_rumble_ramp(10, 180, 220, 230);
    re15_rumble_ramp(10, 220, 180, 240);
    PIN(re15_rumble_belegt(1) == 10, "Kaskade 1 belegt 10 Slots in Ring B (%d)",
        re15_rumble_belegt(1));
    PIN(re15_rumble_belegt(0) == 0,  "Kaskade 1 ruehrt Ring A nicht an (%d)",
        re15_rumble_belegt(0));
    {
        uint8_t b1 = 0, b21 = 0, b405 = 0xFF;
        for (f = 1; f <= 405; f++) {
            re15_rumble_tick(&a0, &a1);
            if (f == 1)  b1  = a1;
            if (f == 21) b21 = a1;
            if (f == 405) b405 = a1;
        }
        PIN(b1 == 180,  "Kaskade 1 Bild 1   = 180 (Grundton)                      (%u)", b1);
        PIN(b21 == 184, "Kaskade 1 Bild 21  = 184 (erster Puls ueberlagert)       (%u)", b21);
        PIN(b405 == 0,  "Kaskade 1 Bild 405 = 0   (alles ausgelaufen)             (%u)", b405);
    }

    /* A6: KASKADE 2 (Todes-Uebergang 2->3, @0x801034c4-@0x80103568) — 12 Stoesse im
     *     kleinen Motor, Ring A. Die Stakkato-Bilder sind 1..3, 6..8, 16..17, 61..63,
     *     66..68, 76..77, 91..93, 96..98, 106..107, 111..112, 116..117, 121. */
    re15_rumble_reset();
    re15_rumble_large(5, 200, 0);
    re15_rumble_ramp(200, 200, 0, 5);
    re15_rumble_small(3, 0);   re15_rumble_small(3, 5);   re15_rumble_small(2, 15);
    re15_rumble_small(3, 60);  re15_rumble_small(3, 65);  re15_rumble_small(2, 75);
    re15_rumble_small(3, 90);  re15_rumble_small(3, 95);  re15_rumble_small(2, 105);
    re15_rumble_small(2, 110); re15_rumble_small(2, 115); re15_rumble_small(1, 120);
    PIN(re15_rumble_belegt(0) == 12, "Kaskade 2 belegt 12 Slots in Ring A (%d)",
        re15_rumble_belegt(0));
    {
        int an_bilder = 0, ok_start = 0, ok_ende = 0;
        for (f = 1; f <= 210; f++) {
            re15_rumble_tick(&a0, &a1);
            if (a0) an_bilder++;
            if (f == 1   && a0 == 1 && a1 == 200) ok_start = 1;
            if (f == 121 && a0 == 1)              ok_ende  = 1;
        }
        /* 3+3+2+3+3+2+3+3+2+2+2+1 = 29 Bilder mit aktivem kleinen Motor. */
        PIN(an_bilder == 29, "Kaskade 2: kleiner Motor zieht auf genau 29 Bildern an (%d)",
            an_bilder);
        PIN(ok_start, "Kaskade 2 Bild 1: klein=1 UND gross=200 gleichzeitig");
        PIN(ok_ende,  "Kaskade 2 Bild 121: der letzte Stoss (small(1,120)) sitzt");
    }

    /* A7: GEGENPROBE — ohne Kaskade bleibt alles still (sonst waere der Pin wertlos). */
    re15_rumble_reset();
    {
        int still = 1;
        for (f = 1; f <= 60; f++) {
            re15_rumble_tick(&a0, &a1);
            if (a0 || a1) still = 0;
        }
        PIN(still, "GEGENPROBE: frisch zurueckgesetzte Ringe liefern 60 Bilder lang 0/0");
    }
    re15_rumble_reset();
}

int main(int argc, char **argv)
{
    size_t rsz = 0;
    int pin_mode = (argc > 1 && strcmp(argv[1], "pin") == 0);
    (void)pin_mode;

    printf("=== R18 birkin-restluecken: Rumble / Blut / Opfermaschine ===\n");

    teil_a_rumble();

    /* ------------------------------------------------------------------------------------ */
    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE5/ROOM5090.RDT", &rsz);
    if (!raw) { printf("FEHLT: ROOM5090.RDT\n"); return 77; }
    if (re15_rdt_parse(raw, rsz, &s_rdt) != 0) { printf("FEHLT: RDT-Parse\n"); return 77; }
    /* Die Blut-Routine liest die Kamera ueber g_room_rdt + g_scd.cam_id — im echten Spiel
     * setzt room_common.c beides (`*c->active_cuts = g_room_rdt.cuts`, room_common.c:320).
     * Die Sonde muss denselben Zustand herstellen, sonst misst sie ihre eigene Luecke. */
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;

    memset(&s_cam, 0, sizeof s_cam);
    memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 14;

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    re15_esp_fx_reset();
    g_current_room_id = 0x5090; g_room_change.pending = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = 25600; pl->y = 0; pl->z = -23350; pl->rot_y = 1024;
    re15_collision_set_band(0);

    if (!load_re2_banks()) { printf("FEHLT: RE2-Baenke (CDEMD0.EMS)\n"); return 77; }

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
    for (int f = 0; f < 1500 && e->motion != 5; f++) frame();

    /* -------------------------------------------------------------------------------- */
    /* B) BLUT-SPAWNS der Trefferroutine                                                 */
    /* -------------------------------------------------------------------------------- */
    printf("-- B) Blut-Spawns (FUN_8001BF10 @0x8001bf10, vier Aufrufe) --\n");
    {
        re15_enemy_bank_t *b36 = re15_enemy_find(0x36u);
        PIN(b36 && b36->victim_ok && b36->anim_victim.clip_count > 0,
            "0x36 traegt eine Opferbank (EMD-Paar 3, dir[5]/dir[6]) mit %d Clips",
            b36 ? b36->anim_victim.clip_count : -1);

        /* Die Blut-Routine haengt an der BLICKRICHTUNG DES SPIELERS: `slti v0,a1,-64`
         * @0x80102640 laesst sie nur zu, wenn die X-Komponente der um PL+0x76 gedrehten Achse
         * (4096,0,0) unter -64 liegt. 0x800CFC6E ist der Spieler-Yaw, nicht die Kamera — der
         * Spieler-Zustandslader schreibt X/Y/Z/Yaw in einem Block nach 0x800CFC30/34/38/6E
         * (@0x80026ccc/@0x80026ce8/@0x80026d0c/@0x80026d18), und 0x800CFBF8+0x38/0x3C/0x40/0x76
         * sind genau diese Adressen. Gemessen wird deshalb BEIDES: ein Yaw, bei dem die Wache
         * offen ist, und einer, bei dem sie zu ist (GEGENPROBE). */
        int16_t yaw_offen = -1, yaw_zu = -1;
        for (int a = 0; a < 4096; a += 64) {
            int32_t yy = 0, dx = 0, dz = 0;
            pl->rot_y = (int16_t)a;
            if (!re15_g5_blut_kamera(&yy, &dx, &dz)) continue;
            if (dx < -64 && yaw_offen < 0) yaw_offen = (int16_t)a;
            if (dx >= -64 && yaw_zu    < 0) yaw_zu    = (int16_t)a;
        }
        printf("  Spieler-Yaw: Wache OFFEN ab %d, Wache ZU bei %d\n",
               (int)yaw_offen, (int)yaw_zu);
        PIN(yaw_offen >= 0 && yaw_zu >= 0,
            "es gibt Blickrichtungen fuer BEIDE Seiten der Wache @0x80102640");

        int delta_offen = -1, delta_zu = -1;
        for (int durchlauf = 0; durchlauf < 2; durchlauf++) {
            int16_t yaw = durchlauf ? yaw_zu : yaw_offen;
            re15_esp_fx_reset();
            int vorher = re15_esp_fx_count();
            int getroffen = 0;
            for (int f = 0; f < 240 && !getroffen; f++) {
                int sub = -1, ph = -1, t = 0;
                re15_g5_boss_zustand(&sub, &ph, &t);
                pl->rot_y = yaw;
                if (ph == 0 && sub != 0xF) { e->hit_react |= 1u; getroffen = 1; }
                frame();
            }
            int d = re15_esp_fx_count() - vorher;
            printf("  Yaw %d (%s): Treffer %d -> %d neue ESP-Partikel"
                   "  [Zweig %d, Routine %d, Wache %d, Spawns %d; dirx %d]\n",
                   (int)yaw, durchlauf ? "Wache ZU" : "Wache OFFEN", getroffen, d,
                   re15_g5_blut_zaehler(0), re15_g5_blut_zaehler(1),
                   re15_g5_blut_zaehler(2), re15_g5_blut_zaehler(3),
                   (int)re15_g5_blut_diag(2));
            if (durchlauf) delta_zu = d; else delta_offen = d;
        }

        PIN(delta_offen == 2 || delta_offen == 4,
            "Wache OFFEN: der Treffer spawnt genau 2 (ausserhalb) oder 4 (Korridor"
            " Z in [-23999,-22501]) Blut-Partikel — gemessen %d", delta_offen);
        PIN(delta_zu == 0,
            "GEGENPROBE Wache ZU: derselbe Treffer spawnt NICHTS (`slti v0,a1,-64`"
            " @0x80102640) — gemessen %d", delta_zu);
    }

    /* -------------------------------------------------------------------------------- */
    /* C) OPFERMASCHINE — Spieler-Routine 6 (0x80103908)                                 */
    /* -------------------------------------------------------------------------------- */
    printf("-- C) Devour-Opfermaschine (0x80103908) --\n");
    {
        int16_t ziel = (int16_t)((e->rot_y + 2048) & 0xFFF);
        int16_t start_yaw;
        pl->rot_y = (int16_t)((ziel + 2000) & 0xFFF);   /* bewusst weit weg vom Ziel */
        start_yaw = pl->rot_y;
        re15_g5_devour_opfer_start(pl, e);
        PIN(re15_g5_devour_opfer_phase() == 0,
            "Start setzt PL+0x06 = 0 (@0x80103908, Phase 0 faellt danach durch)");

        frame();                                        /* Phase 0 -> 1 in EINEM Bild */
        PIN(re15_g5_devour_opfer_phase() == 1,
            "nach einem Bild steht die Maschine in Phase 1 (`sb a2,6(s1)` @0x8010397c)");
        PIN(pl->motion == 0,
            "der Opfer-Clip ist Clip 0 (`lui v0,0x7`+`sw v0,332(s1)` @0x80103968/@0x80103978)"
            " — gemessen %d", (int)pl->motion);
        PIN(pl->anim_frac <= 7,
            "der Blend-Zaehler startete auf 7 (+0x14E) und laeuft ab — gemessen %d",
            (int)pl->anim_frac);
        PIN(pl->rot_y != start_yaw,
            "Phase 1 dreht den Spieler (FUN_8001569C @0x801039e0, Schritt 256 @0x801039ac)");

        /* Bis zum Schnapp laufen lassen: Bild 8 -> Phase 2, Yaw = Ziel (@0x80103a24). */
        for (int f = 0; f < 60 && re15_g5_devour_opfer_phase() == 1; f++) frame();
        PIN(re15_g5_devour_opfer_phase() >= 2,
            "die Maschine erreicht Phase 2 (`lbu v1,333(s1)` == 8 @0x801039fc-04)");
        PIN(pl->rot_y == ziel,
            "beim Uebergang 1->2 schnappt der Yaw auf Boss-Yaw+2048 (`sh v1,118(s1)`"
            " @0x80103a24) — Ziel %d, gemessen %d", (int)ziel, (int)pl->rot_y);
    }

    /* -------------------------------------------------------------------------------- */
    /* D) TENTAKEL: Lokal-Y des Spieler-Segments (Vorbefund Runde 17 §5 Punkt 4)          */
    /* -------------------------------------------------------------------------------- */
    printf("-- D) Tentakel-Dreh-Zweig: Segment+0x14 = -1530 statt 0 --\n");
    {
        /* Erst den Kampf laufen lassen, damit die Arme wirklich am Spieler sind. */
        for (int f = 0; f < 900; f++) frame();
        int32_t mit = re15_g5_tent_dreh_mess(0);
        int32_t ohne = re15_g5_tent_dreh_mess(1);
        int32_t erreicht = re15_g5_tent_dreh_mess(2);
        printf("  Hoehenpruefung erreicht: %d mal; Dreh-Zweig feuert mit -1530: %d,"
               " mit der alten 0: %d\n", (int)erreicht, (int)mit, (int)ohne);
        PIN(erreicht > 0,
            "die Kind-Kollision des Tentakels wurde im Kampf ueberhaupt erreicht (%d)",
            (int)erreicht);
        printf("  -> die Aenderung ist %s (Differenz %d von %d Durchlaeufen)\n",
               (mit == ohne) ? "im gemessenen Lauf WIRKUNGSLOS"
                             : "im gemessenen Lauf WIRKSAM",
               (int)(mit - ohne), (int)erreicht);
    }

    printf(s_fail ? "=== R18 ERGEBNIS: %d Pin(s) GERISSEN ===\n"
                  : "=== R18 ERGEBNIS: alle Pins halten (%d Fehler) ===\n", s_fail);
    return s_fail ? 1 : 0;
}
