/* probe_r21_tentakel_schub.c — Runde 21 (2026-09-22), Spur A zum Nutzer-Befund
 * "Jetzt wurde ich von birkin rausgeschoben ausserhalb des begehbaren BEreiches"
 * (ROOM5090, Marke F962, pos=(11102,0,-19232)).
 *
 * Runde 20 hat den KOERPER-Schub des Bosses geklaert (@0x80031cbc/@0x80031d70 hinter dem
 * `jalr v0` @0x80031cb4). Diese Sonde misst den ZWEITEN, davon unabhaengigen Schub:
 * die KIND-KOLLISION der vier Tentakel (enemy_ai_tentakel_g5.c:808 ->
 * tent_kind_kollision, Original 0x80104F64 + FUN_80034D0C).
 *
 * Reiner Messstand. KEIN add_test, KEINE Aenderung an Engine-Code.
 *
 * Modi:
 *   lauf     — der Weg des Nutzers aus befund.log (F811 (9000,-23550) -> F962), Bild fuer
 *              Bild mit Arm-Zustand, Kontaktbits, Dreh-Zaehlern, Begehbarkeit
 *   mess     — Startplaetze x 900 Bilder im echten re15_game_step; isoliert die Bilder,
 *              in denen NUR der Tentakel geschoben hat (Boss-Schub-Wirkung 0), und misst
 *              deren groessten Ein-Bild-Weg
 *   raster   — Ein-Bild-Schub des Tentakels ueber das begehbare Raster (Diagnose)
 *   schwelle — die Fallen-Schwelle nach derselben Vorschrift wie probe_r20_birkin_push
 *   alles    — (Default) alle vier
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
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
#include "re15_math.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void    re15_g5_push_stats(int *rufe, int *wirkungen, int32_t *maxschub);
extern int     re15_g5_push_flips(void);
extern int     re15_g5_boss_zustand(int *sub, int *ph, int *timer);
extern int     re15_g5_body_segment(int idx, int32_t *wx, int32_t *wy, int32_t *wz, int32_t *r);
extern int     re15_g5_body_push_player(re15_actor_t *pl);
extern int     re15_g5_tentakel_zustand(int idx, int *sub, int *ph, int *ankermodus,
                                        int32_t *scale_x, uint16_t *kontakt);
extern int     re15_g5_tentakel_spitze(int idx, int32_t out[3]);
extern int32_t re15_g5_tent_dreh_mess(int idx);
extern void    re15_g5_tentakel_tick(const re15_actor_t *g5);
extern int     re15_g5_devour_opfer_phase(void);
extern void    re15_g5_devour_opfer_start(re15_actor_t *pl, const re15_actor_t *e);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;

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
/* Pad fuer die Modi, die den Spieler laufen lassen (D2); 0 = alle uebrigen Modi,
 * deren Verhalten damit unveraendert bleibt. */
static uint16_t s_pad = 0, s_pad_prev = 0;
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
    s_ctx.pad_current = s_pad;
    s_ctx.pad_pressed = (uint16_t)(s_pad & ~s_pad_prev);
    s_pad_prev = s_pad;
    re15_game_step(&s_ctx);
}

static int find_boss(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x36) return s;
    return -1;
}

/* Wahrheit fuer "begehbar" — WORTGLEICH zu probe_r20_birkin_push / probe_p2_floor_dump:
 * Teil 1 Containment-Scan (IN einer soliden Zelle = nicht begehbar), Teil 2 Klemmpfad
 * (die Klemme verschiebt den Punkt nicht). */
static int begehbar(int32_t x, int32_t z, int band)
{
    int32_t nx = x, nz = z;
    re15_collision_set_band(band);
    if (re15_collision_on_floor(&s_rdt, x, z)) return 0;
    re15_collision_constrain(&s_rdt, x, z, &nx, &nz);
    return (nx == x && nz == z);
}

static long labs32(int32_t v) { return v < 0 ? -(long)v : (long)v; }

int main(int argc, char **argv)
{
    size_t rsz = 0;
    const char *mode = (argc > 1) ? argv[1] : "alles";
    int do_lauf     = !strcmp(mode, "lauf")     || !strcmp(mode, "alles");
    int do_mess     = !strcmp(mode, "mess")     || !strcmp(mode, "alles");
    int do_raster   = !strcmp(mode, "raster")   || !strcmp(mode, "alles");
    int do_schwelle = !strcmp(mode, "schwelle") || !strcmp(mode, "alles");
    int do_spur     = !strcmp(mode, "spur");
    int do_jagd     = !strcmp(mode, "jagd");
    int do_opfer    = !strcmp(mode, "opfer");
    int do_maul     = !strcmp(mode, "maul");
    int do_seg      = !strcmp(mode, "seg");
    int do_attr     = !strcmp(mode, "attr");
    int do_max      = !strcmp(mode, "max");
    int do_zensus   = !strcmp(mode, "zensus");
    int do_kriech   = !strcmp(mode, "kriech");
    int do_kriechfix= !strcmp(mode, "kriechfix");
    int do_bedarf   = !strcmp(mode, "bedarf");
    int do_phasen   = !strcmp(mode, "phasen");
    int do_druck    = !strcmp(mode, "druck");
    int do_druckfix = !strcmp(mode, "druckfix");
    int do_kettefix = !strcmp(mode, "kettefix");
    int do_kette    = !strcmp(mode, "kette") || do_kettefix;
    int do_verbund  = !strcmp(mode, "verbund");

    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE5/ROOM5090.RDT", &rsz);
    if (!raw) { printf("FEHLT: ROOM5090.RDT\n"); return 77; }
    if (re15_rdt_parse(raw, rsz, &s_rdt) != 0) { printf("FEHLT: RDT-Parse\n"); return 77; }

    printf("=== R21 Tentakel-Schub (%s): ROOM5090, Spur A zum Nutzer-Befund ===\n", mode);

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

    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 14);
    g_scd.cut_auto_enabled = 1;
    s_shown = 14;

    /* --- Boss hochfahren (wie r17/r20): bis Cut 12, dann Intro auslaufen ---------------- */
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
    printf("  Boss bereit: slot %d pos=(%d,%d,%d) clip=%d\n",
           bslot, (int)e->x, (int)e->y, (int)e->z, (int)e->motion);

    /* ====================================================================== *
     * M) DER ZUSTAND, IN DEM DER DREH-ZWEIG TATSAECHLICH FEUERT (Gegenprobe
     *    zu probe_r18_birkin_rest §D: 83 Drehungen aus 203 Durchlaeufen).
     *    Die Opfermaschine wird wie dort explizit gestartet; danach laeuft
     *    der Kampf weiter. Protokolliert JEDE Drehung mit ihrem Weg, der
     *    Spieler-Hoehe und der Begehbarkeit — genau die Zahlen, die der
     *    Dreh-Zweig-Befund braucht.
     * ====================================================================== */
    if (do_maul) {
        printf("-- M) Opfermaschine explizit gestartet, dann 1200 Bilder ---------------\n");
        pl->x = e->x + 1200; pl->z = e->z; pl->y = 0; pl->hp = 40000; pl->rot_y = 2048;
        re15_g5_devour_opfer_start(pl, e);
        int32_t lx = pl->x, lz = pl->z;
        int flips = 0, kontakt = 0, raus_nach_dev = 0, gelistet = 0;
        int32_t maxweg_flip = 0, maxweg = 0;
        for (int f = 0; f < 1200; f++) {
            int32_t dr0 = re15_g5_tent_dreh_mess(0), dr1;
            int32_t e0 = re15_g5_tent_dreh_mess(2), e1;
            int32_t vx = pl->x, vy = pl->y, vz = pl->z;
            uint16_t k[4]; int any_k = 0;
            if (pl->hp < 20000) pl->hp = 40000;
            frame();
            dr1 = re15_g5_tent_dreh_mess(0);
            e1  = re15_g5_tent_dreh_mess(2);
            for (int i = 0; i < 4; i++) {
                k[i] = 0; re15_g5_tentakel_zustand(i, NULL, NULL, NULL, NULL, &k[i]);
                if (k[i] & 6u) any_k = 1;
            }
            int32_t weg = (int32_t)(labs32(pl->x - lx) + labs32(pl->z - lz));
            int dev = re15_g5_devour_opfer_phase(), gr = re15_player_is_grabbed();
            int ok = begehbar(pl->x, pl->z, 0);
            if (any_k) kontakt++;
            if (weg > maxweg) maxweg = weg;
            if (dr1 != dr0) {
                flips += (int)(dr1 - dr0);
                if (weg > maxweg_flip) maxweg_flip = weg;
                if (gelistet < 30) {
                    gelistet++;
                    printf("    FLIP f%4d (%6d,%6d,%6d)->(%6d,%6d,%6d) weg=%5d dreh+%d"
                           " erreicht+%d k=%x%x%x%x dev=%d gr=%d cmd=%d %s\n",
                           f, (int)vx, (int)vy, (int)vz, (int)pl->x, (int)pl->y, (int)pl->z,
                           (int)weg, (int)(dr1 - dr0), (int)(e1 - e0),
                           k[0] & 6u, k[1] & 6u, k[2] & 6u, k[3] & 6u, dev, gr,
                           (int)pl->state, ok ? "begehbar" : "RAUS");
                }
            }
            if (!ok && dev <= 0 && !gr) raus_nach_dev++;
            lx = pl->x; lz = pl->z;
        }
        printf("  M) Bilder mit Tentakel-Kontakt %d ; Drehungen %d\n", kontakt, flips);
        printf("  M) groesster Ein-Bild-Weg gesamt %d ; groesster in einem Dreh-Bild %d\n",
               (int)maxweg, (int)maxweg_flip);
        printf("  M) unbegehbare Bilder OHNE Opfermaschine/Griff: %d\n", raus_nach_dev);
        printf("  M) Endlage des Spielers (%d,%d,%d) %s ; Opfer-Phase %d\n",
               (int)pl->x, (int)pl->y, (int)pl->z,
               begehbar(pl->x, pl->z, 0) ? "begehbar" : "RAUS",
               re15_g5_devour_opfer_phase());
        printf("  M) Dreh-Zaehler gesamt: mit -1530 %d, mit 0 %d, Hoehenpruefung erreicht %d\n",
               (int)re15_g5_tent_dreh_mess(0), (int)re15_g5_tent_dreh_mess(1),
               (int)re15_g5_tent_dreh_mess(2));
    }

    /* ====================================================================== *
     * O) DER ZUSTAND, IN DEM RUNDE 18 IHRE 15 DREHUNGEN GEMESSEN HAT:
     *    Spieler in der Bahn des Bosses, kein Teleport, langer Lauf. Hier
     *    laeuft auch die Opfermaschine (Spieler im Maul, pl->y != 0) — genau
     *    dort liegen Tentakel-Segmente auf und unter der Spieler-Hoehe.
     *    Protokolliert JEDE Vorzeichen-Drehung mit ihrem Weg.
     * ====================================================================== */
    if (do_opfer) {
        printf("-- O) Spieler in der Boss-Bahn (9000,-23400), 4000 Bilder --------------\n");
        pl->x = 9000; pl->z = -23400; pl->y = 0; pl->hp = 4000; pl->rot_y = 2048;
        int32_t lx = pl->x, lz = pl->z;
        int flips = 0, kontakt = 0, raus = 0, raus_ohne = 0;
        int32_t maxweg_flip = 0, maxweg_kont = 0;
        for (int f = 0; f < 4000; f++) {
            int rufe0 = 0, wirk0 = 0, rufe1 = 0, wirk1 = 0;
            int32_t dr0 = re15_g5_tent_dreh_mess(0), dr1;
            int32_t vx = pl->x, vy = pl->y, vz = pl->z;
            uint16_t k[4]; int any_k = 0;
            if (pl->hp < 2000) pl->hp = 4000;             /* am Leben halten, kein Tod */
            re15_g5_push_stats(&rufe0, &wirk0, NULL);
            frame();
            re15_g5_push_stats(&rufe1, &wirk1, NULL);
            dr1 = re15_g5_tent_dreh_mess(0);
            for (int i = 0; i < 4; i++) {
                k[i] = 0; re15_g5_tentakel_zustand(i, NULL, NULL, NULL, NULL, &k[i]);
                if (k[i] & 6u) any_k = 1;
            }
            int32_t weg = (int32_t)(labs32(pl->x - lx) + labs32(pl->z - lz));
            int dev = re15_g5_devour_opfer_phase(), gr = re15_player_is_grabbed();
            if (any_k) { kontakt++; if (weg > maxweg_kont) maxweg_kont = weg; }
            if (dr1 != dr0) {
                flips += (int)(dr1 - dr0);
                if (weg > maxweg_flip) maxweg_flip = weg;
                if (flips <= 40)
                    printf("    FLIP f%4d (%6d,%6d,%6d)->(%6d,%6d,%6d) weg=%5d"
                           " dreh+%d k=%x%x%x%x dev=%d gr=%d cmd=%d %s\n",
                           f, (int)vx, (int)vy, (int)vz, (int)pl->x, (int)pl->y, (int)pl->z,
                           (int)weg, (int)(dr1 - dr0),
                           k[0] & 6u, k[1] & 6u, k[2] & 6u, k[3] & 6u, dev, gr,
                           (int)pl->state, begehbar(pl->x, pl->z, 0) ? "begehbar" : "RAUS");
            }
            if (!begehbar(pl->x, pl->z, 0)) {
                raus++;
                if (dev <= 0 && !gr) raus_ohne++;
            }
            lx = pl->x; lz = pl->z;
        }
        printf("  O) Bilder mit Tentakel-Kontakt %d ; groesster Weg dabei %d\n",
               kontakt, (int)maxweg_kont);
        printf("  O) Vorzeichen-Drehungen %d ; groesster Weg in einem Dreh-Bild %d\n",
               flips, (int)maxweg_flip);
        printf("  O) unbegehbare Bilder %d (davon ohne Opfermaschine/Griff %d)\n",
               raus, raus_ohne);
        printf("  O) Dreh-Zaehler gesamt: mit -1530 %d, mit 0 %d, Hoehenpruefung erreicht %d\n",
               (int)re15_g5_tent_dreh_mess(0), (int)re15_g5_tent_dreh_mess(1),
               (int)re15_g5_tent_dreh_mess(2));
    }

    /* ====================================================================== *
     * J) JAGD auf den VORZEICHEN-DREH-ZWEIG. Der Port feuert ihn (mit
     *    pl->y == pos_s_y) genau dann, wenn ein Tentakel-Segment auf oder
     *    unter der Spieler-Hoehe liegt (Herleitung + Adressen im Dossier).
     *    Gemessen wird, ob er im echten Kampf ueberhaupt feuert und wie gross
     *    der Schub dann wird. Je Bild wird der Spieler auf einen anderen
     *    begehbaren Rasterpunkt gesetzt -> jedes Bild ist eine unabhaengige
     *    Probe (Arm-Pose x Standplatz). Der Sprung selbst wird NICHT als Weg
     *    gezaehlt: gemessen wird nur die Strecke, die das Bild hinzufuegt.
     * ====================================================================== */
    if (do_jagd) {
        int32_t gx[4096]; int32_t gz[4096]; int ng = 0;
        for (int32_t sx = 3400; sx <= 14000 && ng < 4096; sx += 400)
            for (int32_t sz = -26400; sz <= -17000 && ng < 4096; sz += 400)
                if (begehbar(sx, sz, 0)) { gx[ng] = sx; gz[ng] = sz; ng++; }
        printf("-- J) JAGD: %d begehbare Rasterpunkte, 30000 Bilder -------------------\n", ng);
        int proben = 0, kontakt = 0, flips = 0, raus = 0, uebersprungen = 0;
        int32_t maxweg = 0, maxweg_flip = 0, mx = 0, mz = 0, mnx = 0, mnz = 0;
        for (int f = 0; f < 30000 && ng > 0; f++) {
            int32_t sx = gx[f % ng], sz = gz[f % ng];
            int rufe0 = 0, wirk0 = 0, rufe1 = 0, wirk1 = 0;
            int32_t dr0, dr1, weg;
            uint16_t k[4]; int any_k = 0;
            pl->x = sx; pl->z = sz; pl->y = 0; pl->hp = 400;
            pl->pos_s_x = (uint16_t)(int32_t)sx;
            pl->pos_s_y = (uint16_t)(int32_t)0;
            pl->pos_s_z = (uint16_t)(int32_t)sz;
            re15_g5_push_stats(&rufe0, &wirk0, NULL);
            dr0 = re15_g5_tent_dreh_mess(0);
            frame();
            re15_g5_push_stats(&rufe1, &wirk1, NULL);
            dr1 = re15_g5_tent_dreh_mess(0);
            for (int i = 0; i < 4; i++) {
                k[i] = 0; re15_g5_tentakel_zustand(i, NULL, NULL, NULL, NULL, &k[i]);
                if (k[i] & 6u) any_k = 1;
            }
            if (re15_g5_devour_opfer_phase() > 0 || re15_player_is_grabbed() ||
                pl->state != 1 || pl->hit_react != 0) { uebersprungen++; continue; }
            proben++;
            if (!any_k) continue;
            if (wirk1 != wirk0) continue;                 /* Boss-Koerper hat mitgeschoben */
            kontakt++;
            weg = (int32_t)(labs32(pl->x - sx) + labs32(pl->z - sz));
            if (weg > maxweg) { maxweg = weg; mx = sx; mz = sz; mnx = pl->x; mnz = pl->z; }
            if (dr1 != dr0) {
                flips++;
                if (weg > maxweg_flip) maxweg_flip = weg;
                if (flips <= 20)
                    printf("    FLIP f%5d (%6d,%6d)->(%6d,%6d) weg=%5d k=%x%x%x%x\n",
                           f, (int)sx, (int)sz, (int)pl->x, (int)pl->z, (int)weg,
                           k[0] & 6u, k[1] & 6u, k[2] & 6u, k[3] & 6u);
            }
            if (!begehbar(pl->x, pl->z, 0)) raus++;
        }
        printf("  J) %d brauchbare Bilder (%d uebersprungen), %d mit reinem Tentakel-Kontakt\n",
               proben, uebersprungen, kontakt);
        printf("  J) Vorzeichen-Drehungen: %d ; groesster Weg mit Drehung: %d\n",
               flips, (int)maxweg_flip);
        printf("  J) groesster Ein-Bild-Weg (reiner Tentakel): %d  (%d,%d)->(%d,%d)\n",
               (int)maxweg, (int)mx, (int)mz, (int)mnx, (int)mnz);
        printf("  J) davon danach UNBEGEHBAR: %d von %d\n", raus, kontakt);
        printf("  J) Dreh-Zaehler gesamt: mit -1530 %d, mit 0 %d, Hoehenpruefung erreicht %d\n",
               (int)re15_g5_tent_dreh_mess(0), (int)re15_g5_tent_dreh_mess(1),
               (int)re15_g5_tent_dreh_mess(2));
    }

    /* ====================================================================== *
     * S) DIE EINE SPUR, die in Abschnitt B unbegehbar endete: Start
     *    (9000,-22300), 300 Bilder, JEDES Bild protokolliert. Zusaetzlich:
     *    was die Wandklemme aus dem Bildanfangspunkt gemacht HAETTE
     *    (@0x80031d70 laeuft im Port VOR dem Tentakel-Schub, nicht danach).
     * ====================================================================== */
    if (do_spur) {
        printf("-- S) Einzelspur Start (9000,-22300), 300 Bilder ------------------------\n");
        printf("   Band-0-SCA-Zellen im Zielbereich x 10500..12500 / z -23000..-21000:\n");
        for (int i = 0; i < s_rdt.sca_count; i++) {
            const re15_sca_entry_t *sc = &s_rdt.sca[i];
            int32_t x0 = sc->x, z0 = sc->z;
            int32_t x1 = x0 + (int32_t)sc->width, z1 = z0 + (int32_t)sc->density;
            if ((sc->floor >> 4) != 0) continue;
            if (x1 < 10500 || x0 > 12500 || z1 < -23000 || z0 > -21000) continue;
            printf("     #%3d typ=%u x %6d..%6d z %6d..%6d\n", i, sc->type,
                   (int)x0, (int)x1, (int)z0, (int)z1);
        }
        pl->x = 9000; pl->z = -22300; pl->y = 0; pl->hp = 400; pl->rot_y = 2048;
        int32_t lx = pl->x, lz = pl->z;
        int erst_raus = -1, raus_mit_k = 0, raus_ohne_k = 0;
        for (int f = 0; f < 300; f++) {
            int rufe0 = 0, wirk0 = 0, rufe1 = 0, wirk1 = 0;
            int32_t vx = pl->x, vz = pl->z;
            int vor_ok = begehbar(vx, vz, 0);
            uint16_t k[4]; int32_t sc4[4]; int any_k = 0;
            int32_t kx, kz;
            re15_g5_push_stats(&rufe0, &wirk0, NULL);
            frame();
            re15_g5_push_stats(&rufe1, &wirk1, NULL);
            for (int i = 0; i < 4; i++) {
                k[i] = 0; sc4[i] = 0;
                re15_g5_tentakel_zustand(i, NULL, NULL, NULL, &sc4[i], &k[i]);
                if (k[i] & 6u) any_k = 1;
            }
            int32_t weg = (int32_t)(labs32(pl->x - lx) + labs32(pl->z - lz));
            int nach_ok = begehbar(pl->x, pl->z, 0);
            /* Was die Klemme aus dem ENDPUNKT machen wuerde, wenn sie mit dem
             * BILDANFANGSPUNKT als Bezug NACH dem Schub liefe (= die Reihenfolge des
             * Originals: Entity-Schleife @0x8001ce04 vor Spieler-Verteiler @0x8001ce0c). */
            kx = pl->x; kz = pl->z;
            re15_collision_set_band(0);
            re15_collision_constrain(&s_rdt, vx, vz, &kx, &kz);
            if (!nach_ok || any_k || weg > 150 || f < 5)
                printf("  f%3d (%6d,%6d)->(%6d,%6d) weg=%5d %s vor=%s"
                       "  Klemme(vorher)->(%6d,%6d)%s  k=%x%x%x%x sc=%4d%5d%5d%5d"
                       "  wirk+%d cmd=%d hp=%d\n",
                       f, (int)vx, (int)vz, (int)pl->x, (int)pl->z, (int)weg,
                       nach_ok ? "begehbar" : "RAUS    ", vor_ok ? "ok" : "RAUS",
                       (int)kx, (int)kz,
                       (kx == pl->x && kz == pl->z) ? " STUMM" : " aktiv",
                       k[0] & 6u, k[1] & 6u, k[2] & 6u, k[3] & 6u,
                       (int)sc4[0], (int)sc4[1], (int)sc4[2], (int)sc4[3],
                       wirk1 - wirk0, (int)pl->state, (int)pl->hp);
            if (!nach_ok) {
                if (erst_raus < 0) erst_raus = f;
                if (any_k) raus_mit_k++; else raus_ohne_k++;
            }
            lx = pl->x; lz = pl->z;
            if (pl->hp < 0) break;
        }
        printf("  S) erstes Bild ausserhalb: %d ; unbegehbare Bilder mit Tentakel-Kontakt %d,"
               " ohne %d\n", erst_raus, raus_mit_k, raus_ohne_k);
        printf("  S) Dreh-Zaehler: mit -1530 %d, mit 0 %d, Hoehenpruefung erreicht %d\n",
               (int)re15_g5_tent_dreh_mess(0), (int)re15_g5_tent_dreh_mess(1),
               (int)re15_g5_tent_dreh_mess(2));
    }

    /* ====================================================================== *
     * A) DER WEG DES NUTZERS. befund.log F811 = (9000,-23550), von dort klebt
     *    z ab F856 auf -19232 und x laeuft auf 11102. Gefahren mit pad 0.
     * ====================================================================== */
    if (do_lauf) {
        int raus_ab = -1, kontakt_bilder = 0, tent_bilder = 0;
        int32_t maxweg_tent = 0, maxweg_all = 0;
        int32_t m_tx = 0, m_tz = 0;
        printf("-- A) LAUF ab der Nutzer-Position (9000,-23550), 900 Bilder --------------\n");
        pl->x = 9000; pl->z = -23550; pl->y = 0; pl->hp = 100; pl->rot_y = 2055;
        int32_t lx = pl->x, lz = pl->z;
        for (int f = 0; f < 900; f++) {
            int rufe0 = 0, wirk0 = 0, rufe1 = 0, wirk1 = 0;
            int32_t dreh0 = re15_g5_tent_dreh_mess(0), erreicht0 = re15_g5_tent_dreh_mess(2);
            int32_t vx = pl->x, vz = pl->z;
            int vor_ok = begehbar(vx, vz, 0);
            uint16_t k[4]; int32_t sc[4]; int sb[4], ph[4];
            re15_g5_push_stats(&rufe0, &wirk0, NULL);
            frame();
            re15_g5_push_stats(&rufe1, &wirk1, NULL);
            for (int i = 0; i < 4; i++) {
                k[i] = 0; sc[i] = 0; sb[i] = -1; ph[i] = -1;
                re15_g5_tentakel_zustand(i, &sb[i], &ph[i], NULL, &sc[i], &k[i]);
            }
            int32_t dreh1 = re15_g5_tent_dreh_mess(0), erreicht1 = re15_g5_tent_dreh_mess(2);
            int any_k = (k[0] | k[1] | k[2] | k[3]) & 6u;
            int32_t weg = (int32_t)(labs32(pl->x - lx) + labs32(pl->z - lz));
            int nach_ok = begehbar(pl->x, pl->z, 0);
            if (weg > maxweg_all) maxweg_all = weg;
            if (any_k) kontakt_bilder++;
            /* ISOLATION: Boss-Koerper-Schub hat NICHT gewirkt, Startpunkt war begehbar,
             * mindestens ein Arm hatte Kontakt -> der Weg gehoert dem Tentakel. */
            if (any_k && wirk1 == wirk0 && vor_ok) {
                tent_bilder++;
                if (weg > maxweg_tent) { maxweg_tent = weg; m_tx = pl->x; m_tz = pl->z; }
            }
            if (f < 40 || any_k || !nach_ok || weg > 200 || dreh1 != dreh0)
                printf("  f%3d (%6d,%6d)->(%6d,%6d) weg=%5d %s hp=%3d cmd=%d"
                       "  arm sub/ph/scale/k: %d/%d/%4d/%x %d/%d/%4d/%x %d/%d/%4d/%x %d/%d/%4d/%x"
                       "  dreh+%d erreicht+%d  Boss=(%6d,%6d) clip=%d\n",
                       f, (int)vx, (int)vz, (int)pl->x, (int)pl->z, (int)weg,
                       nach_ok ? "begehbar" : "RAUS    ", (int)pl->hp, (int)pl->state,
                       sb[0], ph[0], (int)sc[0], k[0] & 6u,
                       sb[1], ph[1], (int)sc[1], k[1] & 6u,
                       sb[2], ph[2], (int)sc[2], k[2] & 6u,
                       sb[3], ph[3], (int)sc[3], k[3] & 6u,
                       (int)(dreh1 - dreh0), (int)(erreicht1 - erreicht0),
                       (int)e->x, (int)e->z, (int)e->motion);
            if (!nach_ok && raus_ab < 0) raus_ab = f;
            lx = pl->x; lz = pl->z;
            if (pl->hp < 0) { printf("  (Spieler tot ab Bild %d)\n", f); break; }
        }
        printf("  A) Bilder mit Tentakel-Kontakt: %d ; davon ohne Boss-Koerper-Wirkung: %d\n",
               kontakt_bilder, tent_bilder);
        printf("  A) groesster Ein-Bild-Weg gesamt: %d ; groesster mit Tentakel-Kontakt"
               " ohne Boss-Wirkung: %d (nach (%d,%d))\n",
               (int)maxweg_all, (int)maxweg_tent, (int)m_tx, (int)m_tz);
        printf("  A) erstes Bild ausserhalb: %d\n", raus_ab);
        printf("  A) Dreh-Zaehler gesamt: mit -1530 %d, mit 0 %d, Hoehenpruefung erreicht %d\n",
               (int)re15_g5_tent_dreh_mess(0), (int)re15_g5_tent_dreh_mess(1),
               (int)re15_g5_tent_dreh_mess(2));
        for (int i = 0; i < 4; i++) {
            int32_t tip[3] = { 0, 0, 0 };
            if (re15_g5_tentakel_spitze(i, tip) == 0)
                printf("  A) Arm %d Spitze=(%d,%d,%d) Wurzel=(%d,%d,%d)\n", i,
                       (int)tip[0], (int)tip[1], (int)tip[2], 0, 0, 0);
        }
    }

    /* ====================================================================== *
     * B) Startplaetze x Bilder im echten re15_game_step.
     * ====================================================================== */
    if (do_mess) {
        static const int32_t zspur[] = { -22300, -22900, -23400, -23900, -24600 };
        static const int32_t xspur[] = {  9000, 10000, 11000, 12000, 13000,  8000,  7000,  6000 };
        int m_bilder = 0, m_raus = 0, m_starts = 0, m_verworfen = 0;
        int m_tent_bilder = 0, m_kontakt_bilder = 0;
        int32_t m_maxweg = 0, m_maxweg_tent = 0;
        int32_t m_raus_x = 0, m_raus_z = 0, m_tx = 0, m_tz = 0;
        int32_t d0 = re15_g5_tent_dreh_mess(0), d2 = re15_g5_tent_dreh_mess(2);
        /* Aufschluesselung der Bilder auf unbegehbarem Punkt nach Spieler-Zustand:
         * [0] Opfermaschine laeuft (Spieler ist IM Maul = kein Defekt)
         * [1] gegriffen
         * [2] Treffer-Reaktion / Knockdown (cmd 2)
         * [3] tot (cmd 3/7)
         * [4] KEINES davon = echter Aussetzer */
        int raus_klasse[5] = { 0, 0, 0, 0, 0 };
        int weg_klasse[5]  = { 0, 0, 0, 0, 0 };
        int gross_gelistet = 0, raus_gelistet = 0;
        printf("-- B) MESS: %d Startplaetze x 300 Bilder, echter re15_game_step ---------\n",
               (int)(sizeof xspur / sizeof xspur[0]) * (int)(sizeof zspur / sizeof zspur[0]));
        for (int xi = 0; xi < (int)(sizeof xspur / sizeof xspur[0]); xi++) {
            for (int zi = 0; zi < (int)(sizeof zspur / sizeof zspur[0]); zi++) {
                int32_t sx = xspur[xi], sz = zspur[zi];
                if (!begehbar(sx, sz, 0)) { m_verworfen++; continue; }
                m_starts++;
                pl->x = sx; pl->z = sz; pl->y = 0; pl->hp = 400; pl->rot_y = 2048;
                int32_t lx = pl->x, lz = pl->z;
                for (int f = 0; f < 300; f++) {
                    int rufe0 = 0, wirk0 = 0, rufe1 = 0, wirk1 = 0;
                    int32_t vx = pl->x, vz = pl->z;
                    int vor_ok = begehbar(vx, vz, 0);
                    uint16_t k[4]; int any_k = 0;
                    re15_g5_push_stats(&rufe0, &wirk0, NULL);
                    frame();
                    re15_g5_push_stats(&rufe1, &wirk1, NULL);
                    if (pl->hp < 0) break;
                    for (int i = 0; i < 4; i++) {
                        k[i] = 0; re15_g5_tentakel_zustand(i, NULL, NULL, NULL, NULL, &k[i]);
                        if (k[i] & 6u) any_k = 1;
                    }
                    m_bilder++;
                    int32_t weg = (int32_t)(labs32(pl->x - lx) + labs32(pl->z - lz));
                    if (weg > m_maxweg) m_maxweg = weg;
                    if (any_k) m_kontakt_bilder++;
                    if (any_k && wirk1 == wirk0 && vor_ok) {
                        m_tent_bilder++;
                        if (weg > m_maxweg_tent) { m_maxweg_tent = weg; m_tx = pl->x; m_tz = pl->z; }
                    }
                    lx = pl->x; lz = pl->z;
                    {
                        int dev = re15_g5_devour_opfer_phase();
                        int gr  = re15_player_is_grabbed();
                        int kl  = (dev > 0) ? 0
                                : gr ? 1
                                : (pl->state == 2) ? 2
                                : (pl->state == 3 || pl->state == 7) ? 3 : 4;
                        int ok  = begehbar(pl->x, pl->z, 0);
                        if (!ok) {
                            if (!m_raus) { m_raus_x = pl->x; m_raus_z = pl->z; }
                            m_raus++; raus_klasse[kl]++;
                            if (kl == 4 && raus_gelistet < 25) {
                                raus_gelistet++;
                                printf("    RAUS-OHNE-ERKLAERUNG f%3d start=(%6d,%6d)"
                                       " (%6d,%6d)->(%6d,%6d) weg=%5d cmd=%d motion=%d"
                                       " hr=%d dev=%d gr=%d hp=%d k=%x%x%x%x Boss=(%6d,%6d)\n",
                                       f, (int)sx, (int)sz, (int)vx, (int)vz,
                                       (int)pl->x, (int)pl->z, (int)weg, (int)pl->state,
                                       (int)pl->motion, (int)pl->hit_react, dev, gr,
                                       (int)pl->hp, k[0] & 6u, k[1] & 6u, k[2] & 6u, k[3] & 6u,
                                       (int)e->x, (int)e->z);
                            }
                        }
                        if (weg > 1250) {
                            weg_klasse[kl]++;
                            if (gross_gelistet < 25) {
                                gross_gelistet++;
                                printf("    GROSS f%3d (%6d,%6d)->(%6d,%6d) weg=%5d %s"
                                       "  cmd=%d motion=%d hr=%d dev=%d gr=%d hp=%d"
                                       "  k=%x%x%x%x  Boss=(%6d,%6d)\n",
                                       f, (int)vx, (int)vz, (int)pl->x, (int)pl->z, (int)weg,
                                       ok ? "begehbar" : "RAUS", (int)pl->state,
                                       (int)pl->motion, (int)pl->hit_react, dev, gr,
                                       (int)pl->hp, k[0] & 6u, k[1] & 6u, k[2] & 6u, k[3] & 6u,
                                       (int)e->x, (int)e->z);
                            }
                        }
                    }
                }
            }
        }
        printf("  B) Startplaetze %d (verworfen %d) ; gemessene Bilder %d\n",
               m_starts, m_verworfen, m_bilder);
        printf("  B) Bilder mit Tentakel-Kontakt: %d ; davon ohne Boss-Koerper-Wirkung: %d\n",
               m_kontakt_bilder, m_tent_bilder);
        printf("  B) groesster Ein-Bild-Weg gesamt %d ; groesster mit Tentakel-Kontakt"
               " ohne Boss-Wirkung %d (nach (%d,%d))\n",
               (int)m_maxweg, (int)m_maxweg_tent, (int)m_tx, (int)m_tz);
        printf("  B) Bilder auf UNBEGEHBAREM Punkt: %d%s\n", m_raus, m_raus ? "" : "  (keins)");
        if (m_raus) printf("     erster: (%d,%d)\n", (int)m_raus_x, (int)m_raus_z);
        printf("  B) davon: Opfermaschine %d | gegriffen %d | Treffer/Knockdown %d |"
               " tot %d | OHNE Erklaerung %d\n",
               raus_klasse[0], raus_klasse[1], raus_klasse[2], raus_klasse[3], raus_klasse[4]);
        printf("  B) Bilder mit Weg > 1250 (Fallen-Schwelle r20): Opfermaschine %d |"
               " gegriffen %d | Treffer/Knockdown %d | tot %d | OHNE Erklaerung %d\n",
               weg_klasse[0], weg_klasse[1], weg_klasse[2], weg_klasse[3], weg_klasse[4]);
        printf("  B) Dreh-Zaehler-Zuwachs in diesem Abschnitt: dreh+%d erreicht+%d\n",
               (int)(re15_g5_tent_dreh_mess(0) - d0), (int)(re15_g5_tent_dreh_mess(2) - d2));
    }

    /* ====================================================================== *
     * C) RASTER: EIN Tentakel-Tick ueber begehbare Standplaetze. Diagnose,
     *    kein Riegel — der Arm tickt je Probe einmal weiter, die Stichprobe
     *    deckt also viele (Arm-Pose x Standplatz)-Paare ab.
     * ====================================================================== */
    if (do_raster) {
        int32_t maxd = 0, mx = 0, mz = 0, mnx = 0, mnz = 0;
        int proben = 0, treffer = 0, raus = 0, flips = 0;
        int32_t d0 = re15_g5_tent_dreh_mess(0);
        printf("-- C) RASTER: ein re15_g5_tentakel_tick je Standplatz (Diagnose) --------\n");
        for (int runde = 0; runde < 6; runde++) {
            /* zwischen den Runden den echten Kampf weiterlaufen lassen, damit die Arme
             * wieder ausfahren (scale_x > 0 ist die Schranke der Kind-Kollision). */
            for (int f = 0; f < 120; f++) frame();
            for (int32_t sz = -26000; sz <= -18000; sz += 200) {
                for (int32_t sx = 3000; sx <= 14000; sx += 200) {
                    int32_t nx, nz, dd;
                    int fl0, fl1;
                    if (!begehbar(sx, sz, 0)) continue;
                    pl->x = sx; pl->z = sz; pl->y = 0; pl->hp = 400;
                    pl->pos_s_x = (uint16_t)(int32_t)sx;
                    pl->pos_s_y = (uint16_t)(int32_t)0;
                    pl->pos_s_z = (uint16_t)(int32_t)sz;
                    fl0 = (int)re15_g5_tent_dreh_mess(0);
                    proben++;
                    re15_g5_tentakel_tick(e);
                    fl1 = (int)re15_g5_tent_dreh_mess(0);
                    dd = (int32_t)(labs32(pl->x - sx) + labs32(pl->z - sz));
                    if (dd == 0) continue;
                    treffer++;
                    if (fl1 != fl0) flips++;
                    nx = pl->x; nz = pl->z;
                    re15_collision_set_band(0);
                    re15_collision_constrain(&s_rdt, sx, sz, &nx, &nz);
                    if (!begehbar(nx, nz, 0)) raus++;
                    if (dd > maxd) {
                        maxd = dd; mx = sx; mz = sz; mnx = nx; mnz = nz;
                    }
                }
            }
        }
        printf("  C) %d Standplaetze geprueft, %d mit Tentakel-Schub, %d davon mit"
               " Vorzeichen-Drehung\n", proben, treffer, flips);
        printf("  C) groesster Ein-Bild-Schub: %d ab (%d,%d), nach Klemme (%d,%d)\n",
               (int)maxd, (int)mx, (int)mz, (int)mnx, (int)mnz);
        printf("  C) Schub+Klemme landet UNBEGEHBAR: %d von %d\n", raus, treffer);
        printf("  C) Dreh-Zaehler-Zuwachs: %d\n", (int)(re15_g5_tent_dreh_mess(0) - d0));
    }

    /* ====================================================================== *
     * D) FALLEN-SCHWELLE — dieselbe Vorschrift wie probe_r20_birkin_push
     *    (Vergleichszahl: dort 1250 aus 172860 Proben).
     * ====================================================================== */
    if (do_schwelle) {
        static const int SIN12[16] = {    0,  1567,  2896,  3784,  4096,  3784,  2896,  1567,
                                          0, -1567, -2896, -3784, -4096, -3784, -2896, -1567 };
        int32_t schwelle = 0x7fffffff;
        long proben = 0, faelle = 0;
        for (int32_t sx = 5200; sx <= 13800; sx += 400) {
            for (int32_t sz = -26400; sz <= -16400; sz += 400) {
                if (!begehbar(sx, sz, 0)) continue;
                for (int w = 0; w < 16; w++) {
                    int32_t dx1 = SIN12[(w + 4) & 15], dz1 = SIN12[w];
                    for (int32_t d = 200; d <= 4000; d += 50) {
                        int32_t tx = sx + (int32_t)(((long long)dx1 * d) / 4096);
                        int32_t tz = sz + (int32_t)(((long long)dz1 * d) / 4096);
                        int32_t nx = tx, nz = tz;
                        proben++;
                        re15_collision_set_band(0);
                        re15_collision_constrain(&s_rdt, sx, sz, &nx, &nz);
                        if (!begehbar(nx, nz, 0)) {
                            faelle++;
                            if (d < schwelle) schwelle = d;
                            break;
                        }
                    }
                }
            }
        }
        printf("-- D) FALLEN-SCHWELLE: kleinster Ein-Bild-Weg, der nach der Klemme"
               " unbegehbar endet = %d  (%ld Proben, %ld Faelle)\n",
               (int)schwelle, proben, faelle);
    }

    /* ====================================================================== *
     * G) GEOMETRIE der vier Kollisionssegmente je Arm. Rechnet tent_posen +
     *    tent_welt (enemy_ai_tentakel_g5.c) mit den OEFFENTLICHEN Posen-APIs
     *    NACH — kein Engine-Eingriff. Gemessen wird, WO die Segmente liegen
     *    (Abstand zur Arm-Wurzel und zum Boss), ob die Hoehenpruefung sie
     *    greifen laesst und wie gross der Ein-Bild-Schub dann wird.
     * ====================================================================== */
    if (do_seg) {
        re15_enemy_bank_t *bk = re15_enemy_find(0x37u);
        int32_t maxab_w = 0, maxab_b = 0; int mf = -1, mi = -1, mk = -1;
        int32_t maxsegx = -0x7fffffff, minsegx = 0x7fffffff;
        int hoehe_ok = 0, hoehe_nein = 0, kontakt = 0, gelistet = 0;
        int32_t maxover = 0, maxschub = 0;
        int segy_ge0 = 0, segy_lt0 = 0;
        int32_t maxtr[4] = { 0, 0, 0, 0 };
        int32_t maxtr_x[4] = { 0, 0, 0, 0 };
        int erst_gemeldet = 0;
        if (!bk || !bk->ok) { printf("FEHLT: keine 0x37-Bank\n"); return 77; }
        printf("  G) Skelett 0x37: %d Bones, Clips %d\n",
               (int)bk->skel.bone_count, (int)bk->anim.clip_count);
        printf("-- G) SEGMENT-GEOMETRIE, 900 Bilder ------------------------------------\n");
        pl->x = 9000; pl->z = -23550; pl->y = 0; pl->hp = 30000; pl->rot_y = 2055;
        pl->pos_s_x = (uint16_t)(int32_t)pl->x;
        pl->pos_s_y = 0;
        pl->pos_s_z = (uint16_t)(int32_t)pl->z;
        for (int f = 0; f < 900; f++) {
            int32_t vx = pl->x, vz = pl->z;
            if (pl->hp < 15000) pl->hp = 30000;
            frame();
            int ai = 0;
            for (int s = 1; s < RE15_ACTOR_MAX && ai < 4; s++) {
                re15_actor_t *a = &g_actors[s];
                re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
                int32_t E[9], w[4][3], seg[4][3];
                int32_t rr[4] = { 800, 600, 600, 600 };
                int kf, idx;
                void *sav;
                if (!a->active || a->type != 0x37u) continue;
                idx = ai++;
                kf = re15_compute_actor_kf(&bk->anim, &bk->skel, a, -1, a->anim_frame);
                sav = g_anim_pose_actor;
                g_anim_pose_actor = NULL;
                g_anim_query_actor = (void *)a;
                if (re15_skel_compute_pose(&bk->skel, kf, poses) != 0) {
                    g_anim_query_actor = NULL; g_anim_pose_actor = sav; continue;
                }
                g_anim_query_actor = NULL; g_anim_pose_actor = sav;
                re15_skel_euler_matrix((int)a->rot_x, (int)a->rot_y, (int)a->rot_z, E);
                for (int b2 = 1; b2 <= 3; b2++) {
                    const int32_t *p = poses[b2].trans;
                    w[b2][0] = a->x + (int32_t)(((int64_t)E[0]*p[0] + (int64_t)E[1]*p[1] +
                                                 (int64_t)E[2]*p[2]) >> 12);
                    w[b2][1] = a->y + (int32_t)(((int64_t)E[3]*p[0] + (int64_t)E[4]*p[1] +
                                                 (int64_t)E[5]*p[2]) >> 12);
                    w[b2][2] = a->z + (int32_t)(((int64_t)E[6]*p[0] + (int64_t)E[7]*p[1] +
                                                 (int64_t)E[8]*p[2]) >> 12);
                }
                memcpy(seg[0], w[3], sizeof w[3]);
                memcpy(seg[1], w[2], sizeof w[2]);
                memcpy(seg[2], w[1], sizeof w[1]);
                for (int c = 0; c < 3; c++)
                    seg[3][c] = w[1][c] + ((int16_t)(w[2][c] - w[1][c]) >> 1);
                for (int b2 = 0; b2 <= 3; b2++) {
                    int32_t m = (int32_t)(labs32(poses[b2].trans[0]) +
                                          labs32(poses[b2].trans[1]) +
                                          labs32(poses[b2].trans[2]));
                    if (m > maxtr[b2]) maxtr[b2] = m;
                    if (labs32(poses[b2].trans[0]) > maxtr_x[b2])
                        maxtr_x[b2] = (int32_t)labs32(poses[b2].trans[0]);
                }
                if (!erst_gemeldet && idx == 3 && f > 5) {
                    erst_gemeldet = 1;
                    printf("  G) Beispiel-Pose Arm3 Bild %d clip=%d bild=%u rot=(%d,%d,%d):\n",
                           f, (int)a->motion, (unsigned)a->anim_frame,
                           (int)a->rot_x, (int)a->rot_y, (int)a->rot_z);
                    for (int b2 = 0; b2 <= 3; b2++)
                        printf("       Bone %d trans=(%7d,%7d,%7d)\n", b2,
                               (int)poses[b2].trans[0], (int)poses[b2].trans[1],
                               (int)poses[b2].trans[2]);
                }
                for (int k = 0; k < 4; k++) {
                    int32_t abw = (int32_t)(labs32(seg[k][0] - a->x) + labs32(seg[k][2] - a->z));
                    int32_t abb = (int32_t)(labs32(seg[k][0] - e->x) + labs32(seg[k][2] - e->z));
                    int32_t rs = rr[k] + 450;
                    int32_t dx = pl->x - seg[k][0], dz = pl->z - seg[k][2];
                    int32_t dist, over, dy;
                    if (abw > maxab_w) { maxab_w = abw; mf = f; mi = idx; mk = k; }
                    if (abb > maxab_b) maxab_b = abb;
                    if (seg[k][0] > maxsegx) maxsegx = seg[k][0];
                    if (seg[k][0] < minsegx) minsegx = seg[k][0];
                    if (seg[k][1] >= 0) segy_ge0++; else segy_lt0++;
                    if ((uint32_t)(dx + rs) > (uint32_t)(rs * 2)) continue;
                    if ((uint32_t)(dz + rs) > (uint32_t)(rs * 2)) continue;
                    dist = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
                    over = rs - dist;
                    if (over <= 0) continue;
                    dy = pl->y - seg[k][1];
                    if (!(-1530 < dy && dy < 1530)) { hoehe_nein++; continue; }
                    hoehe_ok++; kontakt++;
                    if (over > maxover) maxover = over;
                    if (gelistet < 25) {
                        gelistet++;
                        printf("    KONTAKT f%3d arm%d seg%d r=%4d seg=(%7d,%6d,%7d)"
                               " Wurzel=(%7d,%6d,%7d) Boss=(%6d,%7d)"
                               " |seg-Wurzel|=%6d |seg-Boss|=%6d over=%5d dy=%6d\n",
                               f, idx, k, (int)rr[k],
                               (int)seg[k][0], (int)seg[k][1], (int)seg[k][2],
                               (int)a->x, (int)a->y, (int)a->z, (int)e->x, (int)e->z,
                               (int)abw, (int)abb, (int)over, (int)dy);
                    }
                }
            }
            {
                int32_t weg = (int32_t)(labs32(pl->x - vx) + labs32(pl->z - vz));
                if (weg > maxschub) maxschub = weg;
            }
            pl->pos_s_x = (uint16_t)(int32_t)pl->x;
            pl->pos_s_y = 0;
            pl->pos_s_z = (uint16_t)(int32_t)pl->z;
        }
        printf("  G) groesster |Segment - Arm-Wurzel| (xz, Manhattan): %d  (Bild %d, Arm %d,"
               " Segment %d)\n", (int)maxab_w, mf, mi, mk);
        printf("  G) groesster |Segment - Boss| (xz, Manhattan): %d\n", (int)maxab_b);
        printf("  G) Segment-x-Spanne ueber alle Bilder: %d .. %d"
               "  (begehbares Band 0 liegt bei x 3400..14000)\n",
               (int)minsegx, (int)maxsegx);
        printf("  G) Segment-Welt-y: %d Stichproben >= 0 (Boden/darunter), %d < 0 (darueber)\n",
               segy_ge0, segy_lt0);
        printf("  G) Kreis-Kontakte mit bestandener Hoehenpruefung: %d ; an der Hoehe"
               " gescheitert: %d ; groesstes over: %d\n", kontakt, hoehe_nein, (int)maxover);
        printf("  G) groesster Ein-Bild-Weg des Spielers: %d\n", (int)maxschub);
        for (int b2 = 0; b2 <= 3; b2++)
            printf("  G) Bone %d: groesstes |trans| (Manhattan) %d ; groesstes |trans.x| %d\n",
                   b2, (int)maxtr[b2], (int)maxtr_x[b2]);
        printf("  G) Dreh-Zaehler: mit -1530 %d, mit 0 %d, Hoehenpruefung erreicht %d\n",
               (int)re15_g5_tent_dreh_mess(0), (int)re15_g5_tent_dreh_mess(1),
               (int)re15_g5_tent_dreh_mess(2));
    }

    /* ====================================================================== *
     * H) ZURECHNUNG. tent_tick rechnet die Kind-Kollision GANZ OBEN, also mit
     *    der Arm-Pose vom ENDE des Vorbildes. Genau die kann die Sonde VOR
     *    dem Bild selbst ausrechnen — damit ist der Tentakel-Anteil am
     *    Ein-Bild-Weg exakt zurechenbar, ohne einen Engine-Eingriff.
     *    Verglichen wird: vorhergesagter Tentakel-Schub <-> tatsaechlicher
     *    Weg des Spielers. Der Rest gehoert anderen Bewegern.
     * ====================================================================== */
    if (do_attr) {
        re15_enemy_bank_t *bk = re15_enemy_find(0x37u);
        int32_t sx0 = (argc > 2) ? (int32_t)atoi(argv[2]) : 9000;
        int32_t sz0 = (argc > 3) ? (int32_t)atoi(argv[3]) : -22300;
        int nbild = (argc > 4) ? atoi(argv[4]) : 300;
        int32_t maxtent = 0, maxrest = 0;
        int tent_bilder = 0, raus_mit_tent = 0, raus_ohne_tent = 0, gelistet = 0;
        int32_t maxtent_x = 0, maxtent_z = 0;
        if (!bk || !bk->ok) { printf("FEHLT: keine 0x37-Bank\n"); return 77; }
        printf("-- H) ZURECHNUNG ab (%d,%d), %d Bilder ---------------------------------\n",
               (int)sx0, (int)sz0, nbild);
        pl->x = sx0; pl->z = sz0; pl->y = 0; pl->hp = 30000; pl->rot_y = 2048;
        pl->pos_s_x = (uint16_t)(int32_t)pl->x;
        pl->pos_s_y = 0;
        pl->pos_s_z = (uint16_t)(int32_t)pl->z;
        for (int f = 0; f < nbild; f++) {
            int32_t vx = pl->x, vz = pl->z;
            int32_t sim_x = pl->x, sim_z = pl->z;    /* Schattenspieler fuer die Vorhersage */
            int vor_ok = begehbar(vx, vz, 0);
            int ai = 0, treffer = 0;
            if (pl->hp < 15000) pl->hp = 30000;
            /* --- Vorhersage: Kind-Kollision aller vier Arme, Reihenfolge wie im Port --- */
            for (int s = 1; s < RE15_ACTOR_MAX && ai < 4; s++) {
                re15_actor_t *a = &g_actors[s];
                re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
                int32_t E[9], w[4][3], seg[4][3];
                int32_t rr[4] = { 800, 600, 600, 600 };
                int32_t sc = 0;
                int kf, idx; void *sav;
                if (!a->active || a->type != 0x37u) continue;
                idx = ai++;
                re15_g5_tentakel_zustand(idx, NULL, NULL, NULL, &sc, NULL);
                if (sc <= 0) continue;                       /* Port-Schranke Zeile 808 */
                kf = re15_compute_actor_kf(&bk->anim, &bk->skel, a, -1, a->anim_frame);
                sav = g_anim_pose_actor;
                g_anim_pose_actor = NULL; g_anim_query_actor = (void *)a;
                if (re15_skel_compute_pose(&bk->skel, kf, poses) != 0) {
                    g_anim_query_actor = NULL; g_anim_pose_actor = sav; continue;
                }
                g_anim_query_actor = NULL; g_anim_pose_actor = sav;
                re15_skel_euler_matrix((int)a->rot_x, (int)a->rot_y, (int)a->rot_z, E);
                for (int b2 = 1; b2 <= 3; b2++) {
                    const int32_t *p = poses[b2].trans;
                    w[b2][0] = a->x + (int32_t)(((int64_t)E[0]*p[0] + (int64_t)E[1]*p[1] +
                                                 (int64_t)E[2]*p[2]) >> 12);
                    w[b2][1] = a->y + (int32_t)(((int64_t)E[3]*p[0] + (int64_t)E[4]*p[1] +
                                                 (int64_t)E[5]*p[2]) >> 12);
                    w[b2][2] = a->z + (int32_t)(((int64_t)E[6]*p[0] + (int64_t)E[7]*p[1] +
                                                 (int64_t)E[8]*p[2]) >> 12);
                }
                memcpy(seg[0], w[3], sizeof w[3]);
                memcpy(seg[1], w[2], sizeof w[2]);
                memcpy(seg[2], w[1], sizeof w[1]);
                for (int c = 0; c < 3; c++)
                    seg[3][c] = w[1][c] + ((int16_t)(w[2][c] - w[1][c]) >> 1);
                for (int k = 0; k < 4; k++) {
                    int32_t rs = rr[k] + 450;
                    int32_t dx = sim_x - seg[k][0];
                    int32_t dz = sim_z - seg[k][2];
                    int32_t dist, over, dy, d1, px, pz;
                    if ((uint32_t)(dx + rs) > (uint32_t)(rs * 2)) continue;
                    if ((uint32_t)(dz + rs) > (uint32_t)(rs * 2)) continue;
                    dist = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
                    over = rs - dist;
                    if (over <= 0) continue;
                    dy = pl->y - seg[k][1];
                    if (!(-1530 < dy && dy < 1530)) continue;
                    d1 = dist + 1;
                    px = (dx * over) / d1; pz = (dz * over) / d1;
                    sim_x += px; sim_z += pz;
                    treffer++;
                }
            }
            {
                int32_t tent_x = sim_x - vx, tent_z = sim_z - vz;
                int32_t tent = (int32_t)(labs32(tent_x) + labs32(tent_z));
                int32_t weg, rest_x, rest_z, rest;
                frame();
                weg = (int32_t)(labs32(pl->x - vx) + labs32(pl->z - vz));
                rest_x = (pl->x - vx) - tent_x; rest_z = (pl->z - vz) - tent_z;
                rest = (int32_t)(labs32(rest_x) + labs32(rest_z));
                if (tent > maxtent) { maxtent = tent; maxtent_x = tent_x; maxtent_z = tent_z; }
                if (rest > maxrest) maxrest = rest;
                if (treffer) tent_bilder++;
                if (!begehbar(pl->x, pl->z, 0)) {
                    if (treffer) raus_mit_tent++; else raus_ohne_tent++;
                }
                if ((treffer || !begehbar(pl->x, pl->z, 0)) && gelistet < 40) {
                    gelistet++;
                    printf("  f%3d (%6d,%6d)->(%6d,%6d) weg=%5d  Tentakel=(%+5d,%+5d)|%4d|"
                           "  Rest=(%+6d,%+6d)|%5d|  Treffer=%d vor=%s nach=%s\n",
                           f, (int)vx, (int)vz, (int)pl->x, (int)pl->z, (int)weg,
                           (int)tent_x, (int)tent_z, (int)tent,
                           (int)rest_x, (int)rest_z, (int)rest, treffer,
                           vor_ok ? "ok" : "RAUS",
                           begehbar(pl->x, pl->z, 0) ? "ok" : "RAUS");
                }
            }
            pl->pos_s_x = (uint16_t)(int32_t)pl->x;
            pl->pos_s_y = 0;
            pl->pos_s_z = (uint16_t)(int32_t)pl->z;
            if (pl->hp < 0) break;
        }
        printf("  H) Bilder mit Tentakel-Kontakt: %d\n", tent_bilder);
        printf("  H) groesster vorhergesagter Tentakel-Schub: %d  (dx %+d, dz %+d)\n",
               (int)maxtent, (int)maxtent_x, (int)maxtent_z);
        printf("  H) groesster Rest (andere Beweger): %d\n", (int)maxrest);
        printf("  H) unbegehbare Endlagen: mit Tentakel-Kontakt %d, ohne %d\n",
               raus_mit_tent, raus_ohne_tent);
    }

    /* ====================================================================== *
     * Z) ZENSUS DER REIHENFOLGE — das A/B, auf das Spur A hinauslaeuft.
     *
     *    Der Tentakel-Schub (tent_kind_kollision, "pl->x += px; pl->z += pz")
     *    laeuft im Port in re15_enemy_ai_run_all (game_step_common.c:2096) und
     *    damit NACH der Wandklemme desselben Bildes (game_step_common.c:1495).
     *    Im Original laeuft die Entitaeten-Schleife VOR dem Spieler-FSM:
     *        8001ce04  jal 0x8001a50c     Entitaeten (Tentakel-Kind-Kollision)
     *        8001ce0c  jal 0x80031c44     Spieler-FSM
     *          80031cb4  jalr v0          Kommando-Handler
     *          80031cbc  jal 0x8002b544   Koerper-Schub
     *          80031d70  jal 0x8003b0a4   WANDKLEMME
     *    Gemessen wird deshalb JE SCHUB zweierlei:
     *      PORT     = Endlage des Bildes, so wie sie stehen bleibt
     *      ORIGINAL = dieselbe Endlage, aber mit der Klemme DIESES Bildes
     *                 (constrain vom Bildanfangspunkt aus) noch dahinter
     *    und zusaetzlich, was die Klemme des FOLGEBILDES aus einer bereits
     *    ausserhalb liegenden Endlage macht (ox/oz = die Aussenlage selbst).
     * ====================================================================== */
    if (do_zensus) {
        static int32_t gx[8192], gz[8192]; int ng = 0;
        const int ZN = 30000;
        for (int32_t sx = 3400; sx <= 14200 && ng < 8192; sx += 150)
            for (int32_t sz = -26600; sz <= -17000 && ng < 8192; sz += 150)
                if (begehbar(sx, sz, 0)) { gx[ng] = sx; gz[ng] = sz; ng++; }
        printf("-- Z) ZENSUS: %d begehbare Rasterpunkte, %d Bilder ------------------\n",
               ng, ZN);
        static const int32_t ziel[5] = { 0, 300, 600, 900, 1200 };
        int n_mess = 0, n_uebersprungen = 0, n_tent = 0, n_boss = 0, n_pur = 0;
        int raus_port = 0, raus_orig = 0, falle = 0, erholt = 0;
        int32_t maxweg = 0, mw_sx = 0, mw_sz = 0, mw_ex = 0, mw_ez = 0;
        int32_t schwelle = 0x7fffffff;
        int gelistet = 0;
        int flips = 0, flip_raus = 0; int32_t flip_max = 0;
        int32_t dr0 = 0, dr1 = 0;
        for (int f = 0; f < ZN && ng > 0; f++) {
            int32_t tip[3]; int arm = f & 3;
            int32_t sx, sz, ex, ez, nx, nz, fx, fz, weg;
            int rufe0 = 0, wirk0 = 0, rufe1 = 0, wirk1 = 0;
            uint16_t k[4]; int any_k = 0, boss_wirk, port_ok, orig_ok;
            int best = -1; int32_t bestd = 0x7fffffff;
            int32_t zz = ziel[(f / 4) % 5];
            if (re15_g5_tentakel_spitze(arm, tip) == 0) {
                for (int i = 0; i < ng; i++) {
                    int32_t d = (int32_t)(labs32(gx[i] - tip[0]) + labs32(gz[i] - tip[2]));
                    int32_t q = d > zz ? d - zz : zz - d;
                    if (q < bestd) { bestd = q; best = i; }
                }
            }
            if (best < 0) best = f % ng;
            sx = gx[best]; sz = gz[best];
            pl->x = sx; pl->z = sz; pl->y = 0; pl->hp = 400; pl->hit_react = 0;
            pl->pos_s_x = (uint16_t)(int32_t)sx;
            pl->pos_s_y = 0;
            pl->pos_s_z = (uint16_t)(int32_t)sz;
            re15_g5_push_stats(&rufe0, &wirk0, NULL);
            dr0 = re15_g5_tent_dreh_mess(0);
            frame();
            re15_g5_push_stats(&rufe1, &wirk1, NULL);
            dr1 = re15_g5_tent_dreh_mess(0);
            if (re15_g5_devour_opfer_phase() > 0 || re15_player_is_grabbed()) {
                n_uebersprungen++;
                continue;
            }
            n_mess++;
            for (int i = 0; i < 4; i++) {
                k[i] = 0; re15_g5_tentakel_zustand(i, NULL, NULL, NULL, NULL, &k[i]);
                if (k[i] & 6u) any_k = 1;
            }
            boss_wirk = (wirk1 != wirk0);
            ex = pl->x; ez = pl->z;
            weg = (int32_t)(labs32(ex - sx) + labs32(ez - sz));
            if (any_k) n_tent++;
            if (boss_wirk) n_boss++;
            if (!any_k || boss_wirk || weg == 0) continue;
            n_pur++;
            if (weg > maxweg) { maxweg = weg; mw_sx = sx; mw_sz = sz; mw_ex = ex; mw_ez = ez; }
            if (dr1 != dr0) {
                flips++;
                if (weg > flip_max) flip_max = weg;
                if (flips <= 15)
                    printf("    DREH f%5d (%6d,%6d)->(%6d,%6d) weg=%5d\n",
                           f, (int)sx, (int)sz, (int)ex, (int)ez, (int)weg);
            }
            re15_collision_set_band(0);
            port_ok = begehbar(ex, ez, 0);
            nx = ex; nz = ez;
            re15_collision_set_band(0);
            re15_collision_constrain(&s_rdt, sx, sz, &nx, &nz);
            orig_ok = begehbar(nx, nz, 0);
            if (!port_ok) {
                int fok;
                raus_port++;
                if (dr1 != dr0) flip_raus++;
                if (weg < schwelle) schwelle = weg;
                fx = ex; fz = ez;
                re15_collision_set_band(0);
                re15_collision_constrain(&s_rdt, ex, ez, &fx, &fz);
                fok = begehbar(fx, fz, 0);
                if (fok) erholt++; else falle++;
                if (gelistet < 25) {
                    gelistet++;
                    printf("    RAUS f%5d arm%d (%6d,%6d)->(%6d,%6d) weg=%5d k=%x%x%x%x"
                           "  | gleiche Klemme -> (%6d,%6d) %s"
                           "  | Folgebild-Klemme -> (%6d,%6d) %s\n",
                           f, arm, (int)sx, (int)sz, (int)ex, (int)ez, (int)weg,
                           k[0] & 6u, k[1] & 6u, k[2] & 6u, k[3] & 6u,
                           (int)nx, (int)nz, orig_ok ? "begehbar" : "RAUS",
                           (int)fx, (int)fz, fok ? "begehbar" : "FESTGEFAHREN");
                }
            }
            if (!orig_ok) raus_orig++;
        }
        printf("  Z) Bilder gemessen %d (uebersprungen Griff/Maul %d)\n",
               n_mess, n_uebersprungen);
        printf("  Z) davon mit Tentakel-Kontakt %d, mit Boss-Koerper-Wirkung %d,"
               " REINE Tentakel-Schuebe %d\n", n_tent, n_boss, n_pur);
        printf("  Z) groesster reiner Tentakel-Schub: %d  (%d,%d)->(%d,%d)\n",
               (int)maxweg, (int)mw_sx, (int)mw_sz, (int)mw_ex, (int)mw_ez);
        printf("  Z) ENDLAGE UNBEGEHBAR ... PORT (Klemme lief davor): %d von %d\n",
               raus_port, n_pur);
        printf("  Z) ENDLAGE UNBEGEHBAR ... ORIGINAL-Reihenfolge (Klemme dahinter): %d von %d\n",
               raus_orig, n_pur);
        if (raus_port)
            printf("  Z) kleinster Weg, der PORT-seitig hinausfuehrt: %d\n", (int)schwelle);
        printf("  Z) von den %d Aussenlagen holt die Klemme des Folgebildes %d zurueck,"
               " %d bleiben FESTGEFAHREN\n", raus_port, erholt, falle);
        printf("  Z) Bilder MIT Vorzeichen-Drehung: %d ; groesster Weg darin: %d ; davon unbegehbar: %d\n", flips, (int)flip_max, flip_raus);
        printf("  Z) Dreh-Zaehler: mit -1530 %d, mit 0 %d, Hoehenpruefung erreicht %d\n",
               (int)re15_g5_tent_dreh_mess(0), (int)re15_g5_tent_dreh_mess(1),
               (int)re15_g5_tent_dreh_mess(2));
    }

    /* ====================================================================== *
     * K) KRIECHEN DURCH DIE WAND — der Mechanismus, den das Zensus-A/B nur
     *    als Ein-Bild-Ausschlag sieht. Kein Teleport: der Spieler steht am
     *    NORD-Rand des Kampfkorridors (Zelle #28 beginnt bei z -21700), der
     *    Kampf laeuft, und gemessen wird, wie tief ihn die ungeklemmten
     *    Tentakel-Schuebe von Bild zu Bild in die Wand tragen.
     *
     *    Warum das kriechen KANN (push_rect, re15_collision.c:53):
     *      code == 0  (Schubrichtung stimmt auf BEIDEN Achsen mit der
     *      Bewegungsrichtung ueberein, also jenseits der Zellmitte)
     *      -> "*lx = prevx; *lz = prevz" = Ruecksprung auf die VORPOSITION.
     *      Im Port ist die Vorposition (ox/oz, game_step_common.c:1481) die
     *      Endlage des VORBILDES — und die hat der Tentakel-Schub schon in die
     *      Wand gesetzt, weil er NACH der Klemme laeuft (:2096 vs :1495).
     *      Der Riegel haelt ihn dann IN der Wand statt davor.
     *
     *    "kriechfix" laesst denselben Lauf mit der Klemme HINTER dem Schub
     *    laufen (Original-Reihenfolge 8001ce04 -> 8001ce0c/80031d70) — das ist
     *    die A/B-Gegenprobe zum Fix-Vorschlag.
     * ====================================================================== */
    if (do_kriech || do_kriechfix) {
        const int fix = do_kriechfix;
        int32_t sx_l[64], sz_l[64]; int nstart = 0;
        /* Startplaetze: je x der NOERDLICHSTE begehbare Punkt (direkt an #28). */
        for (int32_t sx = 5000; sx <= 14000 && nstart < 64; sx += 500) {
            int32_t best = 0; int have = 0;
            for (int32_t sz = -25000; sz <= -21000; sz += 50)
                if (begehbar(sx, sz, 0)) { best = sz; have = 1; }
            if (have) { sx_l[nstart] = sx; sz_l[nstart] = best; nstart++; }
        }
        printf("-- K) KRIECHEN%s: %d Startplaetze am Nordrand x 1500 Bilder ----------\n",
               fix ? " (FIX: Klemme hinter dem Schub)" : "", nstart);
        int bilder = 0, raus = 0, gegriffen = 0, maxserie = 0, serie = 0;
        int32_t tiefstes_z = -32000, tief_x = 0;
        int fixpunkt = 0, gelistet = 0;
        int32_t maxschub_nach_klemme = 0;
        for (int s = 0; s < nstart; s++) {
            pl->x = sx_l[s]; pl->z = sz_l[s]; pl->y = 0; pl->hp = 400;
            pl->hit_react = 0;
            serie = 0;
            for (int f = 0; f < 1500; f++) {
                int32_t vx = pl->x, vz = pl->z;
                int32_t nx, nz, weg;
                frame();
                if (fix) {
                    nx = pl->x; nz = pl->z;
                    re15_collision_set_band(0);
                    re15_collision_constrain(&s_rdt, vx, vz, &nx, &nz);
                    pl->x = nx; pl->z = nz;
                }
                if (pl->hp < 100) pl->hp = 400;
                if (re15_g5_devour_opfer_phase() > 0 || re15_player_is_grabbed()) {
                    gegriffen++; serie = 0; continue;
                }
                bilder++;
                weg = (int32_t)(labs32(pl->x - vx) + labs32(pl->z - vz));
                if (weg > maxschub_nach_klemme) maxschub_nach_klemme = weg;
                re15_collision_set_band(0);
                if (!begehbar(pl->x, pl->z, 0)) {
                    raus++; serie++;
                    if (serie > maxserie) maxserie = serie;
                    if (pl->z > tiefstes_z) { tiefstes_z = pl->z; tief_x = pl->x; }
                    if (pl->z == -19232) fixpunkt++;
                    if (gelistet < 30) {
                        gelistet++;
                        printf("    RAUS s%02d f%4d (%6d,%6d)->(%6d,%6d) weg=%5d serie=%d\n",
                               s, f, (int)vx, (int)vz, (int)pl->x, (int)pl->z,
                               (int)weg, serie);
                    }
                } else serie = 0;
            }
        }
        printf("  K) gemessene Bilder %d (Griff/Maul uebersprungen %d)\n", bilder, gegriffen);
        printf("  K) Bilder auf UNBEGEHBAREM Punkt: %d ; laengste Serie: %d Bilder\n",
               raus, maxserie);
        printf("  K) noerdlichste erreichte Lage: z=%d (bei x=%d)  [#28 Suedseite -21700,"
               " Nordseite -19700, Klemm-Fixpunkt -19232]\n",
               (int)tiefstes_z, (int)tief_x);
        printf("  K) Bilder EXAKT auf dem Klemm-Fixpunkt z=-19232: %d\n", fixpunkt);
        printf("  K) groesster Ein-Bild-Weg NACH der Klemme: %d\n",
               (int)maxschub_nach_klemme);
        printf("  K) Dreh-Zaehler: mit -1530 %d, mit 0 %d, Hoehenpruefung erreicht %d\n",
               (int)re15_g5_tent_dreh_mess(0), (int)re15_g5_tent_dreh_mess(1),
               (int)re15_g5_tent_dreh_mess(2));
    }

    /* ====================================================================== *
     * B2) BEDARF — wie gross muss ein UNGEKLEMMTER Schub sein, damit der
     *     Spieler in der Falle landet?
     *
     *     Port-Ablauf eines Schubs am Bildende (game_step_common.c:2096,
     *     Klemme lief schon bei :1495):
     *       Bild N   Endlage P1 = Startpunkt + Schub, UNGEKLEMMT
     *       Bild N+1 ox/oz = P1 -> re15_collision_constrain(P1 -> P1) = P2
     *     Gefangen ist er genau dann, wenn P2 selbst unbegehbar ist (dann
     *     wiederholt sich das Bild fuer Bild: Fixpunkt).
     *     Gemessen je x-Spalte: der kleinste +z-Schub ab dem noerdlichsten
     *     begehbaren Punkt, der P2 unbegehbar macht — die echte Fallen-
     *     Schwelle fuer den ungeklemmten Tentakel-Schub.
     * ====================================================================== */
    if (do_bedarf) {
        printf("-- B2) BEDARF: kleinster ungeklemmter +z-Schub, der in der Falle endet ---\n");
        printf("      x      z0(Nordrand)   Bedarf   P1          P2(Folgeklemme)\n");
        int32_t min_bedarf = 0x7fffffff; int32_t mb_x = 0, mb_z = 0;
        for (int32_t sx = 5000; sx <= 14000; sx += 500) {
            int32_t z0 = 0; int have = 0;
            for (int32_t sz = -25000; sz <= -21000; sz += 10)
                if (begehbar(sx, sz, 0)) { z0 = sz; have = 1; }
            if (!have) continue;
            int32_t bed = -1, p1 = 0, p2x = 0, p2z = 0;
            for (int32_t d = 10; d <= 4000; d += 10) {
                int32_t fx = sx, fz = z0 + d;
                int32_t nx = fx, nz = fz;
                re15_collision_set_band(0);
                re15_collision_constrain(&s_rdt, fx, fz, &nx, &nz);
                if (!begehbar(nx, nz, 0)) {
                    bed = d; p1 = fz; p2x = nx; p2z = nz; break;
                }
            }
            if (bed < 0) {
                printf("  %6d   %8d      (keine Falle bis 4000)\n", (int)sx, (int)z0);
            } else {
                printf("  %6d   %8d      %6d   z=%7d   (%6d,%7d)\n",
                       (int)sx, (int)z0, (int)bed, (int)p1, (int)p2x, (int)p2z);
                if (bed < min_bedarf) { min_bedarf = bed; mb_x = sx; mb_z = z0; }
            }
        }
        if (min_bedarf != 0x7fffffff)
            printf("  B2) KLEINSTER Bedarf ueber alle Spalten: %d  (ab (%d,%d))\n",
                   (int)min_bedarf, (int)mb_x, (int)mb_z);
        else
            printf("  B2) keine Spalte fuehrt in eine Falle\n");
    }

    /* ====================================================================== *
     * P2) PHASEN-HISTOGRAMM — warum der Zensus so viele Bilder ueberspringt.
     *     Zaehlt ueber 3000 Bilder des echten Kampfes, wie oft die
     *     Opfermaschine (PL+0x05, re15_g5_devour_opfer_phase) laeuft und wie
     *     oft der Griff haengt, waehrend der Spieler auf der Stelle steht.
     * ====================================================================== */
    if (do_phasen) {
        int hist[8]; memset(hist, 0, sizeof hist);
        int griff = 0, frei = 0;
        pl->x = 9000; pl->z = -23550; pl->y = 0; pl->hp = 400;
        printf("-- P2) PHASEN: 3000 Bilder ab (9000,-23550) ----------------------------\n");
        for (int f = 0; f < 3000; f++) {
            int ph;
            frame();
            if (pl->hp < 100) pl->hp = 400;
            ph = re15_g5_devour_opfer_phase();
            if (ph < 0) ph = 0; if (ph > 7) ph = 7;
            hist[ph]++;
            if (re15_player_is_grabbed()) griff++;
            if (re15_g5_devour_opfer_phase() <= 0 && !re15_player_is_grabbed()) frei++;
        }
        printf("  P2) Opfer-Phase 0..7: %d %d %d %d %d %d %d %d\n",
               hist[0], hist[1], hist[2], hist[3], hist[4], hist[5], hist[6], hist[7]);
        printf("  P2) Bilder mit Griff: %d ; voellig freie Bilder: %d von 3000\n",
               griff, frei);
    }

    /* ====================================================================== *
     * D2) DRUCK GEGEN DIE WAND — der Zustand des Nutzers. Er LIEF (befund.log
     *     F811..F901 bewegt sich), stand nicht still. Genau dann greift der
     *     Riegel von push_rect (re15_collision.c:53) mit code == 0:
     *         "*lx = prevx; *lz = prevz"   = Ruecksprung auf die VORPOSITION
     *     Und die Vorposition ist im Port (game_step_common.c:1481, ox/oz) die
     *     ENDLAGE DES VORBILDES — also schon die vom Tentakel-Schub in die
     *     Wand gesetzte. Der Riegel haelt ihn dann IN der Wand fest, und der
     *     naechste ungeklemmte Schub schiebt von dort weiter.
     *     Gemessen wird mit gehaltenem VORWAERTS gegen die Nordwand.
     * ====================================================================== */
    if (do_druck || do_druckfix) {
        const int fix = do_druckfix;
        int32_t yaw_nord = 0; int32_t best_dz = -0x7fffffff;
        /* Eichung: welcher Yaw laeuft nach +z (auf die Nordwand zu)? */
        for (int32_t y = 0; y < 4096; y += 256) {
            int32_t z0;
            pl->x = 9000; pl->z = -23550; pl->y = 0; pl->hp = 400; pl->rot_y = (int16_t)y;
            z0 = pl->z;
            s_pad = RE15_PAD_BIT_UP;
            for (int f = 0; f < 12; f++) frame();
            s_pad = 0;
            if (pl->z - z0 > best_dz) { best_dz = pl->z - z0; yaw_nord = y; }
        }
        printf("-- D2) DRUCK%s: Yaw %d laeuft nach +z (dz=%d in 12 Bildern) -----------\n",
               fix ? " (FIX: Klemme hinter dem Schub)" : "", (int)yaw_nord, (int)best_dz);
        int bilder = 0, raus = 0, gegriffen = 0, fixpunkt = 0, maxserie = 0, serie = 0;
        int32_t noerdlichst = -32000, n_x = 0;
        int gelistet = 0;
        for (int32_t sx = 5500; sx <= 13500; sx += 500) {
            int32_t z0 = 0; int have = 0;
            for (int32_t sz = -25000; sz <= -21000; sz += 50)
                if (begehbar(sx, sz, 0)) { z0 = sz; have = 1; }
            if (!have) continue;
            pl->x = sx; pl->z = z0 - 400; pl->y = 0; pl->hp = 400;
            pl->rot_y = (int16_t)yaw_nord; pl->hit_react = 0;
            serie = 0;
            for (int f = 0; f < 1200; f++) {
                int32_t vx = pl->x, vz = pl->z, nx, nz;
                s_pad = RE15_PAD_BIT_UP;
                frame();
                s_pad = 0;
                if (fix) {
                    nx = pl->x; nz = pl->z;
                    re15_collision_set_band(0);
                    re15_collision_constrain(&s_rdt, vx, vz, &nx, &nz);
                    pl->x = nx; pl->z = nz;
                }
                if (pl->hp < 100) pl->hp = 400;
                if (re15_g5_devour_opfer_phase() > 0 || re15_player_is_grabbed()) {
                    gegriffen++; serie = 0; continue;
                }
                bilder++;
                re15_collision_set_band(0);
                if (!begehbar(pl->x, pl->z, 0)) {
                    raus++; serie++;
                    if (serie > maxserie) maxserie = serie;
                    if (pl->z > noerdlichst) { noerdlichst = pl->z; n_x = pl->x; }
                    if (pl->z == -19232) fixpunkt++;
                    if (gelistet < 30) {
                        gelistet++;
                        printf("    RAUS x%5d f%4d (%6d,%6d)->(%6d,%6d) serie=%d\n",
                               (int)sx, f, (int)vx, (int)vz, (int)pl->x, (int)pl->z, serie);
                    }
                } else serie = 0;
            }
        }
        printf("  D2) gemessene Bilder %d (Griff/Maul %d)\n", bilder, gegriffen);
        printf("  D2) Bilder UNBEGEHBAR: %d ; laengste Serie: %d\n", raus, maxserie);
        printf("  D2) noerdlichste Lage: z=%d (x=%d) ; Bilder auf dem Fixpunkt -19232: %d\n",
               (int)noerdlichst, (int)n_x, fixpunkt);
        printf("  D2) Dreh-Zaehler: mit -1530 %d, mit 0 %d, Hoehenpruefung erreicht %d\n",
               (int)re15_g5_tent_dreh_mess(0), (int)re15_g5_tent_dreh_mess(1),
               (int)re15_g5_tent_dreh_mess(2));
    }

    /* ====================================================================== *
     * KE) DIE KETTE — wer setzt den Spieler das ERSTE Mal ausserhalb?
     *
     *     Runde 20 hat den Koerper-Schub des Bosses mit der Vorschrift
     *     gemessen, dass der Startplatz AUSSERHALB beider Boss-Segmente liegt
     *     ("sonst messen wir einen Zustand, in den das Spiel den Spieler nie
     *     bringt"). Genau diese Vorschrift prueft hier nach: ob der
     *     UNGEKLEMMTE Tentakel-Schub (game_step_common.c:2096, nach der
     *     Klemme :1495) den Spieler in den Boss-Zylinder setzen kann.
     *
     *     Je Bild wird protokolliert:
     *       B = Koerper-Schub des Bosses hatte Wirkung (re15_g5_push_stats)
     *       T = irgendein Arm hatte Kind-Kollision (kontakt & 6)
     *       I = Endlage liegt IN einem Boss-Segment (r + 450)
     *     Beim ERSTEN unbegehbaren Bild werden die letzten 10 Bilder gezeigt.
     * ====================================================================== */
    if (do_kette) {
        typedef struct { int32_t vx, vz, ex, ez; int B, T, I0, I1, ok; } ring_t;
        ring_t ring[12]; int rn = 0;
        int32_t yaw_nord = 0; int32_t best_dz = -0x7fffffff;
        for (int32_t y = 0; y < 4096; y += 256) {
            int32_t z0;
            pl->x = 9000; pl->z = -23550; pl->y = 0; pl->hp = 400; pl->rot_y = (int16_t)y;
            z0 = pl->z;
            s_pad = RE15_PAD_BIT_UP;
            for (int f = 0; f < 12; f++) frame();
            s_pad = 0;
            if (pl->z - z0 > best_dz) { best_dz = pl->z - z0; yaw_nord = y; }
        }
        printf("-- KE) KETTE: Yaw %d = nach +z ; Starts am Nordrand, AUSSERHALB beider"
               " Boss-Segmente -------\n", (int)yaw_nord);
        int starts = 0, verworfen = 0, erste_raus = 0;
        int ursache_tent = 0, ursache_body = 0, ursache_beides = 0, ursache_keins = 0;
        int tent_in_zylinder = 0;
        int nach_raus = -1, raus_nachlauf = 0, fixpunkt_erreicht = 0;
        int starts_gezeigt = 0;
        for (int32_t sx = 5500; sx <= 13500; sx += 250) {
            int32_t z0 = 0; int have = 0;
            int32_t g0x = 0, g0z = 0, g0r = 0, g1x = 0, g1z = 0, g1r = 0;
            for (int32_t sz = -25000; sz <= -21000; sz += 50)
                if (begehbar(sx, sz, 0)) { z0 = sz; have = 1; }
            if (!have) { verworfen++; continue; }
            {   /* Vorschrift aus probe_r20_birkin_push: der Startplatz muss AUSSERHALB
                 * beider Boss-Segmente liegen. Der Boss patrouilliert den Korridor, also
                 * warten statt verwerfen: bis zu 900 Bilder laufen lassen, bis der Platz
                 * frei ist. */
                int frei = 0;
                for (int w = 0; w < 900 && !frei; w++) {
                    int64_t d0, d1, r0, r1;
                    if (!re15_g5_body_segment(0, &g0x, NULL, &g0z, &g0r)) { frame(); continue; }
                    re15_g5_body_segment(1, &g1x, NULL, &g1z, &g1r);
                    d0 = (int64_t)(sx - g0x) * (sx - g0x) + (int64_t)(z0 - g0z) * (z0 - g0z);
                    d1 = (int64_t)(sx - g1x) * (sx - g1x) + (int64_t)(z0 - g1z) * (z0 - g1z);
                    r0 = (int64_t)(g0r + 450) * (g0r + 450);
                    r1 = (int64_t)(g1r + 450) * (g1r + 450);
                    if (d0 >= r0 && d1 >= r1) { frei = 1; break; }
                    pl->x = sx; pl->z = z0; pl->y = 0; pl->hp = 400;
                    frame();
                    if (pl->hp < 100) pl->hp = 400;
                }
                if (!frei) { verworfen++; continue; }
                if (starts == 0)
                    printf("     (Segment-Radien: seg0 r=%d, seg1 r=%d)\n",
                           (int)g0r, (int)g1r);
            }
            starts++;
            pl->x = sx; pl->z = z0; pl->y = 0; pl->hp = 400;
            pl->rot_y = (int16_t)yaw_nord; pl->hit_react = 0;
            rn = 0; nach_raus = -1;
            for (int f = 0; f < 900; f++) {
                int rufe0 = 0, wirk0 = 0, rufe1 = 0, wirk1 = 0;
                int32_t vx = pl->x, vz = pl->z;
                ring_t r;
                re15_g5_push_stats(&rufe0, &wirk0, NULL);
                s_pad = RE15_PAD_BIT_UP;
                frame();
                s_pad = 0;
                re15_g5_push_stats(&rufe1, &wirk1, NULL);
                if (do_kettefix) {
                    /* FIX-GEGENPROBE: die Wandklemme HINTER dem Schub, also in der
                     * Original-Reihenfolge (Entitaeten @0x8001ce04 vor Spieler-FSM
                     * @0x8001ce0c, dessen Wandpass `jal 0x8003b0a4` @0x80031d70 ist). */
                    int32_t kx = pl->x, kz = pl->z;
                    re15_collision_set_band(0);
                    re15_collision_constrain(&s_rdt, vx, vz, &kx, &kz);
                    pl->x = kx; pl->z = kz;
                }
                if (pl->hp < 100) pl->hp = 400;
                if (re15_g5_devour_opfer_phase() > 0 || re15_player_is_grabbed()) { rn = 0; continue; }
                r.vx = vx; r.vz = vz; r.ex = pl->x; r.ez = pl->z;
                r.B = (wirk1 != wirk0);
                r.T = 0;
                for (int i = 0; i < 4; i++) {
                    uint16_t k = 0; re15_g5_tentakel_zustand(i, NULL, NULL, NULL, NULL, &k);
                    if (k & 6u) r.T = 1;
                }
                r.I0 = r.I1 = 0;
                if (re15_g5_body_segment(0, &g0x, NULL, &g0z, &g0r)) {
                    int64_t d = (int64_t)(pl->x - g0x) * (pl->x - g0x)
                              + (int64_t)(pl->z - g0z) * (pl->z - g0z);
                    r.I0 = (d < (int64_t)(g0r + 450) * (g0r + 450));
                }
                if (re15_g5_body_segment(1, &g1x, NULL, &g1z, &g1r)) {
                    int64_t d = (int64_t)(pl->x - g1x) * (pl->x - g1x)
                              + (int64_t)(pl->z - g1z) * (pl->z - g1z);
                    r.I1 = (d < (int64_t)(g1r + 450) * (g1r + 450));
                }
                re15_collision_set_band(0);
                r.ok = begehbar(pl->x, pl->z, 0);
                /* Tentakel-Schub, der in den Boss-Zylinder hinein fuehrt */
                if (r.T && !r.B && (r.I0 || r.I1)) tent_in_zylinder++;
                if (rn < 12) ring[rn++] = r; else { memmove(ring, ring + 1, 11 * sizeof(ring_t)); ring[11] = r; }
                if (!r.ok && nach_raus < 0) {
                    erste_raus++;
                    if (r.T && r.B) ursache_beides++;
                    else if (r.T)   ursache_tent++;
                    else if (r.B)   ursache_body++;
                    else            ursache_keins++;
                    if (erste_raus <= 6) {
                        printf("  >> ERSTES RAUS bei Start x=%d, Bild %d — die letzten %d Bilder:\n",
                               (int)sx, f, rn);
                        for (int q = 0; q < rn; q++)
                            printf("     %s (%6d,%6d)->(%6d,%6d) d=(%+6d,%+6d) B=%d T=%d"
                                   " imZylinder=%d%d %s\n",
                                   q == rn - 1 ? "*" : " ",
                                   (int)ring[q].vx, (int)ring[q].vz,
                                   (int)ring[q].ex, (int)ring[q].ez,
                                   (int)(ring[q].ex - ring[q].vx), (int)(ring[q].ez - ring[q].vz),
                                   ring[q].B, ring[q].T, ring[q].I0, ring[q].I1,
                                   ring[q].ok ? "begehbar" : "RAUS");
                    }
                    nach_raus = 0;                /* ab hier: Nachlauf protokollieren */
                }
                if (nach_raus >= 0 && nach_raus < 70) {
                    if (!r.ok) raus_nachlauf++;
                    if (r.ez == -19232) { fixpunkt_erreicht++; }
                    if (nach_raus < 70 && starts_gezeigt < 3)
                        printf("     n%02d (%6d,%6d)->(%6d,%6d) d=(%+6d,%+6d) B=%d T=%d"
                               " imZyl=%d%d %s\n",
                               nach_raus, (int)r.vx, (int)r.vz, (int)r.ex, (int)r.ez,
                               (int)(r.ex - r.vx), (int)(r.ez - r.vz), r.B, r.T,
                               r.I0, r.I1, r.ok ? "begehbar" : "RAUS");
                    nach_raus++;
                    if (nach_raus == 70) { starts_gezeigt++; break; }
                }
            }
        }
        printf("  KE) Starts %d (verworfen %d) ; Starts, die ausserhalb endeten: %d\n",
               starts, verworfen, erste_raus);
        printf("  KE) Ursache des ERSTEN Aussen-Bildes: nur Tentakel %d, nur Koerper %d,"
               " beide %d, keiner von beiden %d\n",
               ursache_tent, ursache_body, ursache_beides, ursache_keins);
        printf("  KE) Bilder, in denen ein REINER Tentakel-Schub in einem Boss-Zylinder"
               " endete: %d\n", tent_in_zylinder);
        printf("  KE) Nachlauf nach dem ersten Aussen-Bild: %d weitere Aussen-Bilder, %d Bilder EXAKT auf dem Klemm-Fixpunkt z=-19232\n", raus_nachlauf, fixpunkt_erreicht);
    }

    /* ====================================================================== *
     * VB) DIE VERBUND-MESSUNG — offener Punkt 8.1 des Dossiers
     *
     *     Behauptet war die Kette: ungeklemmter Tentakel-Schub setzt den
     *     Spieler IN den Boss-Zylinder -> der Koerper-Schub des FOLGEBILDES
     *     wirft ihn ueber die Wand -> die Klemme desselben Bildes landet auf
     *     dem Fixpunkt z=-19232 (der Marke des Nutzers).
     *
     *     `kette` hat davon nur das erste Glied gesehen (3 Bilder im Zylinder)
     *     und nie die Landung. Das lag an der Stichprobe: EIN Yaw, EIN
     *     Tastenbild (nur VORWAERTS), 6 ueberlebende Startspalten.
     *
     *     Hier wird dieselbe Kette mit einer BREITEREN Fuehrung gefahren und
     *     jedes EINTRITTS-Ereignis in den Zylinder einzeln verfolgt:
     *       - Ursache des Eintritts (nur Tentakel / nur Koerper / beides / keins)
     *       - die naechsten VB_NACH Bilder: wird der Spieler unbegehbar?
     *         landet er EXAKT auf z=-19232?
     *
     *     ⛔ SELBSTPRUEFUNG: sind die Eintritte 0, ist die Kette an ihrem
     *     ERSTEN Glied widerlegt und nicht etwa "nicht gemessen". Das steht
     *     dann so in der Ausgabe.
     * ====================================================================== */
    if (do_verbund) {
        enum { VB_NACH = 12 };
        int32_t yaw_nord = 0; int32_t best_dz = -0x7fffffff;
        for (int32_t y = 0; y < 4096; y += 256) {
            int32_t z0;
            pl->x = 9000; pl->z = -23550; pl->y = 0; pl->hp = 400; pl->rot_y = (int16_t)y;
            z0 = pl->z;
            s_pad = RE15_PAD_BIT_UP;
            for (int f = 0; f < 12; f++) frame();
            s_pad = 0;
            if (pl->z - z0 > best_dz) { best_dz = pl->z - z0; yaw_nord = y; }
        }
        static const uint16_t pads[] = {
            RE15_PAD_BIT_UP,
            (uint16_t)(RE15_PAD_BIT_UP | RE15_PAD_BIT_LEFT),
            (uint16_t)(RE15_PAD_BIT_UP | RE15_PAD_BIT_RIGHT),
            0
        };
        const int npad = (int)(sizeof pads / sizeof pads[0]);
        printf("-- VB) VERBUND: Yaw-Nord %d ; %d Tastenbilder x 4 Blickrichtungen"
               " je Startspalte, je %d Bilder ------\n", (int)yaw_nord, npad, 600);

        long bilder = 0, gegriffen = 0, raus = 0, fix = 0;
        long eintritte = 0, e_tent = 0, e_body = 0, e_beides = 0, e_keins = 0;
        long e_zu_raus = 0, e_zu_fix = 0;
        int  maxserie = 0, serie = 0;
        int  laeufe = 0, verworfen = 0, gezeigt = 0;
        int32_t weitester = -0x7fffffff, w_x = 0;
        long vb_ohne_zelle = 0, vb_boss_drauf = 0;

        for (int32_t sx = 5500; sx <= 13500; sx += 250) {
            int32_t z0 = 0; int have = 0;
            int32_t g0x = 0, g0z = 0, g0r = 0, g1x = 0, g1z = 0, g1r = 0;
            /* ⛔ ERSTER LAUF DIESES MODUS: 17 von 17 Spalten verworfen, 0 Bilder gemessen.
             * Ursache war diese Suche: sie nahm NUR den LETZTEN begehbaren z-Wert der Spalte
             * und wartete dann darauf, dass GENAU DER ausserhalb beider Boss-Segmente liegt.
             * Der Boss patrouilliert den Nordrand, also war gerade dieser Platz meist belegt.
             * Jetzt werden ALLE begehbaren z der Spalte gesammelt und je Wartebild ALLE
             * geprueft; der erste freie gewinnt. */
            {
                int32_t kand[96]; int nk = 0;
                for (int32_t sz = -25000; sz <= -21000; sz += 50)
                    if (begehbar(sx, sz, 0) && nk < 96) kand[nk++] = sz;
                have = (nk > 0);
                if (!have) { verworfen++; vb_ohne_zelle++; continue; }
                int frei = 0;
                for (int w = 0; w < 400 && !frei; w++) {
                    if (!re15_g5_body_segment(0, &g0x, NULL, &g0z, &g0r)) { frame(); continue; }
                    re15_g5_body_segment(1, &g1x, NULL, &g1z, &g1r);
                    int64_t r0 = (int64_t)(g0r + 450) * (g0r + 450);
                    int64_t r1 = (int64_t)(g1r + 450) * (g1r + 450);
                    for (int ki = 0; ki < nk && !frei; ki++) {
                        int64_t d0 = (int64_t)(sx - g0x) * (sx - g0x)
                                   + (int64_t)(kand[ki] - g0z) * (kand[ki] - g0z);
                        int64_t d1 = (int64_t)(sx - g1x) * (sx - g1x)
                                   + (int64_t)(kand[ki] - g1z) * (kand[ki] - g1z);
                        if (d0 >= r0 && d1 >= r1) { z0 = kand[ki]; frei = 1; }
                    }
                    if (frei) break;
                    pl->x = sx; pl->z = kand[nk - 1]; pl->y = 0; pl->hp = 400;
                    frame();
                    if (pl->hp < 100) pl->hp = 400;
                }
                if (!frei) { verworfen++; vb_boss_drauf++; continue; }
            }
            for (int pi = 0; pi < npad; pi++) {
                for (int yi = 0; yi < 4; yi++) {
                    int32_t yaw = (yaw_nord + yi * 1024) & 0xfff;
                    int im_zyl_vorher = 0;
                    int verfolge = -1;              /* Bilder seit dem letzten Eintritt */
                    int v_raus = 0, v_fix = 0;
                    laeufe++;
                    serie = 0;
                    pl->x = sx; pl->z = z0; pl->y = 0; pl->hp = 400;
                    pl->rot_y = (int16_t)yaw; pl->hit_react = 0;
                    for (int f = 0; f < 600; f++) {
                        int rufe0 = 0, wirk0 = 0, rufe1 = 0, wirk1 = 0;
                        int32_t vx = pl->x, vz = pl->z;
                        int B, T = 0, I0 = 0, I1 = 0, ok, im_zyl;
                        re15_g5_push_stats(&rufe0, &wirk0, NULL);
                        s_pad = pads[pi];
                        frame();
                        s_pad = 0;
                        re15_g5_push_stats(&rufe1, &wirk1, NULL);
                        if (pl->hp < 100) pl->hp = 400;
                        if (re15_g5_devour_opfer_phase() > 0 || re15_player_is_grabbed()) {
                            gegriffen++; serie = 0; im_zyl_vorher = 0; continue;
                        }
                        bilder++;
                        B = (wirk1 != wirk0);
                        for (int i = 0; i < 4; i++) {
                            uint16_t k = 0;
                            re15_g5_tentakel_zustand(i, NULL, NULL, NULL, NULL, &k);
                            if (k & 6u) T = 1;
                        }
                        if (re15_g5_body_segment(0, &g0x, NULL, &g0z, &g0r)) {
                            int64_t d = (int64_t)(pl->x - g0x) * (pl->x - g0x)
                                      + (int64_t)(pl->z - g0z) * (pl->z - g0z);
                            I0 = (d < (int64_t)(g0r + 450) * (g0r + 450));
                        }
                        if (re15_g5_body_segment(1, &g1x, NULL, &g1z, &g1r)) {
                            int64_t d = (int64_t)(pl->x - g1x) * (pl->x - g1x)
                                      + (int64_t)(pl->z - g1z) * (pl->z - g1z);
                            I1 = (d < (int64_t)(g1r + 450) * (g1r + 450));
                        }
                        im_zyl = (I0 || I1);
                        re15_collision_set_band(0);
                        ok = begehbar(pl->x, pl->z, 0);
                        if (!ok) {
                            raus++; serie++;
                            if (serie > maxserie) maxserie = serie;
                            if (pl->z > weitester) { weitester = pl->z; w_x = pl->x; }
                        } else serie = 0;
                        if (pl->z == -19232 && !ok) fix++;

                        /* --- EINTRITT in den Zylinder --- */
                        if (im_zyl && !im_zyl_vorher) {
                            eintritte++;
                            if (T && B) e_beides++; else if (T) e_tent++;
                            else if (B)  e_body++;  else e_keins++;
                            verfolge = 0; v_raus = 0; v_fix = 0;
                            if (gezeigt < 8) {
                                gezeigt++;
                                printf("  >> EINTRITT x=%d yaw=%d pad=%04x Bild %d:"
                                       " (%6d,%6d)->(%6d,%6d) B=%d T=%d seg=%d%d %s\n",
                                       (int)sx, (int)yaw, (unsigned)pads[pi], f,
                                       (int)vx, (int)vz, (int)pl->x, (int)pl->z,
                                       B, T, I0, I1, ok ? "begehbar" : "RAUS");
                            }
                        }
                        im_zyl_vorher = im_zyl;

                        /* --- die naechsten VB_NACH Bilder nach einem Eintritt --- */
                        if (verfolge >= 0) {
                            if (!ok) v_raus = 1;
                            if (pl->z == -19232 && !ok) v_fix = 1;
                            if (gezeigt <= 8 && verfolge > 0 && verfolge <= 4)
                                printf("       n%d (%6d,%6d)->(%6d,%6d) d=(%+6d,%+6d)"
                                       " B=%d T=%d %s\n", verfolge,
                                       (int)vx, (int)vz, (int)pl->x, (int)pl->z,
                                       (int)(pl->x - vx), (int)(pl->z - vz), B, T,
                                       ok ? "begehbar" : "RAUS");
                            verfolge++;
                            if (verfolge > VB_NACH) {
                                if (v_raus) e_zu_raus++;
                                if (v_fix)  e_zu_fix++;
                                verfolge = -1;
                            }
                        }
                    }
                    if (verfolge >= 0) {            /* Lauf endete im Verfolgungsfenster */
                        if (v_raus) e_zu_raus++;
                        if (v_fix)  e_zu_fix++;
                    }
                }
            }
        }
        printf("  VB) verworfene Spalten: %ld ohne begehbare Zelle, %ld weil der Boss 400 Bilder lang keinen freien Platz liess\n", vb_ohne_zelle, vb_boss_drauf);
        printf("  VB) Laeufe %d (Startspalten verworfen %d) ; gemessene Bilder %ld"
               " (Griff/Maul %ld)\n", laeufe, verworfen, bilder, gegriffen);
        printf("  VB) Bilder UNBEGEHBAR: %ld ; laengste Serie: %d ; noerdlichste Lage"
               " z=%d (x=%d)\n", raus, maxserie, (int)weitester, (int)w_x);
        printf("  VB) Bilder EXAKT auf dem Klemm-Fixpunkt z=-19232 und unbegehbar: %ld\n", fix);
        printf("  VB) EINTRITTE in einen Boss-Zylinder: %ld"
               " (nur Tentakel %ld, nur Koerper %ld, beides %ld, keins %ld)\n",
               eintritte, e_tent, e_body, e_beides, e_keins);
        printf("  VB) davon binnen %d Bildern UNBEGEHBAR: %ld ; davon auf dem Fixpunkt: %ld\n",
               (int)VB_NACH, e_zu_raus, e_zu_fix);
        /* ⛔ DIE SCHIENE DARF NICHT LUEGEN: 0 Eintritte bei 0 gemessenen Bildern ist
         * KEINE Widerlegung, sondern ein Fehllauf. Genau das ist im ersten Lauf dieses
         * Modus passiert (17/17 Spalten verworfen) — und die alte Fassung dieser Zeile
         * meldete dazu woertlich "die Kette ist widerlegt". */
        if (laeufe == 0 || bilder == 0)
            printf("  VB) ⛔ FEHLLAUF: %d Laeufe, %ld Bilder — dieser Lauf sagt NICHTS"
                   " ueber die Kette, weder dafuer noch dagegen.\n", laeufe, bilder);
        else if (eintritte == 0)
            printf("  VB) ⛔ KEIN EINZIGER EINTRITT in %ld gemessenen Bildern — die"
                   " behauptete Kette ist an ihrem ERSTEN Glied widerlegt.\n", bilder);
        else if (e_zu_fix == 0)
            printf("  VB) ⛔ EINTRITTE JA, LANDUNG NEIN — das zweite Glied der Kette"
                   " (Koerper-Schub des Folgebildes auf den Fixpunkt) ist in dieser"
                   " Stichprobe NICHT eingetreten.\n");
    }

    printf("=== R21 FERTIG ===\n");
    return 0;
}
