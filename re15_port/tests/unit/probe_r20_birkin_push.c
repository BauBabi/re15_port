/* probe_r20_birkin_push.c — Runde 20 (2026-09-21): "von birkin rausgeschoben ausserhalb des
 * begehbaren Bereiches" (ROOM5090, Marke F962, pos=(11102,0,-19232)).
 *
 * Gefahren wird derselbe ECHTE Ablauf wie probe_r17_birkin_1zu1.c (ROOM5090.RDT -> SCD-VM ->
 * re15_game_step mit den RE2-Baenken EM036/EM037).
 *
 * Modi:
 *   zellen   — Band-0-SCA-Zellen im Bereich der Marke ausgeben (Wanddicke messen)
 *   repro    — Bild fuer Bild: Pos vor Schub, Schubvektor je Segment, Pos nach Schub,
 *              Pos nach Klemme, ob die Klemme etwas getan hat
 *   pin      — Riegel (siehe unten)
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

extern void re15_g5_push_stats(int *rufe, int *wirkungen, int32_t *maxschub);
extern int  re15_g5_push_flips(void);
extern void re15_g5_push_stats_reset(void);
extern int  re15_g5_boss_zustand(int *sub, int *ph, int *timer);
extern int  re15_g5_body_segment(int idx, int32_t *wx, int32_t *wy, int32_t *wz, int32_t *r);
extern int  re15_g5_body_push_player(re15_actor_t *pl);

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

/* Wahrheit fuer "begehbar".
 *
 * Teil 1 ist der Klemmpfad selbst (wie probe_p2_floor_dump, Herkunftsmarke
 * "# QUELLE klemmpfad re15_collision_constrain PR=450 solid=1"): ein Punkt ist begehbar,
 * wenn re15_collision_constrain ihn NICHT verschiebt.
 *
 * ⛔ Teil 2 musste dazu, weil Teil 1 ALLEIN an der Stelle des Nutzers LUEGT (gemessen
 * 2026-09-21, Abschnitt C dieser Sonde). Der Marken-Punkt (11102,-19232) ist ein
 * FIXPUNKT der Klemme: ROOM5090 hat zwei UEBERLAPPENDE Band-0-Zellen — #28
 * z -21700..-19700 und #23 z -19870..-16864. push_rect schiebt aus #23 nach
 * z = -19870-450-0x12 = -20338, und die naechste Zelle #28 schiebt aus ihrer
 * Rueckseite nach z = -19700+450+0x12 = -19232 zurueck. Weil FUN_8003b0a4 kein `break`
 * hat und die LETZTE treffende Zelle gewinnt (@0x8003b510-20), ist die Summe genau 0 —
 * die Klemme laesst den Spieler dort stehen, und Teil 1 wuerde "begehbar" melden,
 * obwohl er IN der Wand steht. -19232 ist kein Zufallswert: es ist exakt die Zahl aus
 * dem Protokoll des Nutzers (befund.log F856..F1246).
 * Teil 2 ist deshalb der Containment-Scan FUN_8003b7f0 (re15_collision_on_floor): liegt
 * der Punkt INNERHALB einer soliden Zelle des Bandes, ist er nicht begehbar — die
 * SCA-Zellen sind die HINDERNISSE (Memory reai-v2-kollisionszellen-sind-waende). */
static int begehbar(int32_t x, int32_t z, int band)
{
    int32_t nx = x, nz = z;
    re15_collision_set_band(band);
    if (re15_collision_on_floor(&s_rdt, x, z)) return 0;        /* IN einer Wandzelle */
    re15_collision_constrain(&s_rdt, x, z, &nx, &nz);
    return (nx == x && nz == z);
}

static void dump_zellen(void)
{
    printf("-- Band-0-SCA-Zellen im Bereich der Marke (x 5000..14000, z -26500..-16000) --\n");
    for (int i = 0; i < s_rdt.sca_count; i++) {
        const re15_sca_entry_t *e = &s_rdt.sca[i];
        int32_t x0 = e->x, z0 = e->z;
        int32_t x1 = x0 + (int32_t)e->width, z1 = z0 + (int32_t)e->density;
        if ((e->floor >> 4) != 0) continue;
        if (x1 < 5000 || x0 > 14000 || z1 < -26500 || z0 > -16000) continue;
        printf("  #%3d typ=%u u0=%u u1=%u band=%d  x %6d..%6d (b=%5u)  z %6d..%6d (d=%5u)\n",
               i, e->type, e->u0, e->u1, e->floor >> 4,
               (int)x0, (int)x1, e->width, (int)z0, (int)z1, e->density);
    }
    printf("-- Begehbarkeits-Profil der Marken-Spalte x=11102 (Band 0, 200er-Schritte) --\n");
    for (int32_t z = -26600; z <= -15000; z += 200) {
        int b = begehbar(11102, z, 0);
        printf("  x=11102 z=%6d  %s\n", (int)z, b ? "BEGEHBAR" : "-");
    }
}

int main(int argc, char **argv)
{
    size_t rsz = 0;
    const char *mode = (argc > 1) ? argv[1] : "pin";
    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE5/ROOM5090.RDT", &rsz);
    if (!raw) { printf("FEHLT: ROOM5090.RDT\n"); return 77; }
    if (re15_rdt_parse(raw, rsz, &s_rdt) != 0) { printf("FEHLT: RDT-Parse\n"); return 77; }

    printf("=== R20 birkin-push (%s): \"von birkin rausgeschoben\" ROOM5090 ===\n", mode);

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

    if (!strcmp(mode, "zellen")) { dump_zellen(); return 0; }
    int lauf = !strcmp(mode, "lauf");
    int mess = !strcmp(mode, "mess") || !strcmp(mode, "pin");

    /* --- Boss hochfahren (wie r17): bis Cut 12, dann Intro auslaufen ------------------- */
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

    /* ================================================================================== *
     * LAUF) Der ECHTE Ablauf: der Spieler steht im Korridor (Position aus befund.log
     *       F811), der Boss zieht sich heran. Bild fuer Bild mit RE15_PUSH_LOG=1.
     * ================================================================================== */
    if (lauf) {
        int raus_ab = -1;
        pl->x = 9000; pl->z = -23550; pl->y = 0; pl->hp = 100; pl->rot_y = 2055;
        printf("-- LAUF: Spieler (9000,-23550) aus befund.log F811, Boss zieht heran ------\n");
        for (int f = 0; f < 900; f++) {
            int ok;
            frame();
            re15_collision_set_band(0);
            ok = begehbar(pl->x, pl->z, 0);
            printf("  f%3d Spieler=(%6d,%6d) %s hp=%d cmd=%d ss=%d/%d clip=%d hr=%d"
                   "   Boss=(%6d,%6d) clip=%d\n",
                   f, (int)pl->x, (int)pl->z, ok ? "begehbar" : "RAUS    ",
                   (int)pl->hp, (int)pl->state, (int)pl->sub_state_1, (int)pl->sub_state_2,
                   (int)pl->motion, (int)pl->hit_react,
                   (int)e->x, (int)e->z, (int)e->motion);
            if (!ok && raus_ab < 0) raus_ab = f;
            if (raus_ab >= 0 && f > raus_ab + 40) break;
        }
        printf("  ERSTES Bild ausserhalb: %d\n", raus_ab);
        return raus_ab >= 0 ? 0 : 0;
    }

    /* ================================================================================== *
     * MESS) Der ECHTE Ablauf ueber viele Startplaetze im Kampfkorridor. Gezaehlt wird
     *       nur, was das Spiel selbst tut (re15_game_step): kein Teleport waehrend der
     *       Messung, jeder Startplatz ist begehbar UND ausserhalb beider Segmente.
     * ================================================================================== */
    int m_bilder = 0, m_raus = 0, m_starts = 0, m_verworfen = 0;
    int32_t m_maxsprung = 0; int32_t m_raus_x = 0, m_raus_z = 0, m_sprung_x = 0, m_sprung_z = 0;
    int m_flinch_kontakt = 0, m_flinch_schub = 0, m_flips = 0, m_spiegel_falsch = 0;
    if (mess) {
        /* Die Startplaetze werden aus der LEBENDEN Bossposition abgeleitet (Rand von
         * Segment 0 + Sicherheitsabstand), damit sie waehrend des ganzen Kampfes
         * gueltig bleiben — der Boss faehrt im Korridor auf und ab. z-Spur deckt den
         * begehbaren Korridor von Wand zu Wand ab. */
        static const int32_t zspur[] = { -22300, -22900, -23400, -23900, -24600 };
        static const int32_t abstand[] = { 200, 700, 1400, 2400 };
        /* Die ERSTEN Plaetze sind die Stelle des Nutzers (befund.log F811 = (9000,-23550))
         * und ihre Nachbarn an beiden Korridorwaenden — dort tritt der Defekt auf. */
        static const int32_t fest[][2] = {
            { 9000, -23550}, { 9000, -22400}, { 9000, -24600}, {11000, -23550},
            {11000, -22300}, {11000, -24600}, {13000, -23550}, {10000, -22300},
        };
        const int N_FEST = (int)(sizeof fest / sizeof fest[0]);
        const int N_START = 40;
        printf("-- MESS: %d Startplaetze x 200 Bilder, echter re15_game_step ----------\n",
               N_START);
        for (int si = 0; si < N_START; si++) {
            int32_t g0x = 0, g0z = 0, g0r = 0, g1x = 0, g1z = 0, g1r = 0;
            int32_t sx, sz, lx = 0, lz = 0;
            int frei = 1;
            if (!re15_g5_body_segment(0, &g0x, NULL, &g0z, &g0r)) { m_verworfen++; continue; }
            re15_g5_body_segment(1, &g1x, NULL, &g1z, &g1r);
            sz = zspur[si % (int)(sizeof zspur / sizeof zspur[0])];
            sx = g0x + g0r + 450 + abstand[(si / 5) % 4];
            if (si < N_FEST) {
                int64_t fd0 = (int64_t)(fest[si][0] - g0x) * (fest[si][0] - g0x)
                            + (int64_t)(fest[si][1] - g0z) * (fest[si][1] - g0z);
                if (fd0 >= (int64_t)(g0r + 450) * (g0r + 450) &&
                    begehbar(fest[si][0], fest[si][1], 0)) {
                    sx = fest[si][0]; sz = fest[si][1];
                }
            }
            if (!begehbar(sx, sz, 0)) { m_verworfen++; continue; }
            /* Startplatz muss AUSSERHALB beider Segmente liegen — sonst messen wir einen
             * Zustand, in den das Spiel den Spieler nie bringt (der Schub laeuft ja jedes
             * Bild und laesst eine solche Ueberdeckung gar nicht entstehen). */
            {
                int64_t d0 = (int64_t)(sx - g0x) * (sx - g0x) + (int64_t)(sz - g0z) * (sz - g0z);
                int64_t d1 = (int64_t)(sx - g1x) * (sx - g1x) + (int64_t)(sz - g1z) * (sz - g1z);
                int64_t r0 = (int64_t)(g0r + 450) * (g0r + 450);
                int64_t r1 = (int64_t)(g1r + 450) * (g1r + 450);
                if (d0 < r0 || d1 < r1) frei = 0;
            }
            if (!frei) { m_verworfen++; continue; }
            m_starts++;
            pl->x = sx; pl->z = sz; pl->y = 0; pl->hp = 400; pl->rot_y = 2048;
            lx = pl->x; lz = pl->z;
            for (int f = 0; f < 200; f++) {
                int rufe0 = 0, wirk0 = 0, rufe1 = 0, wirk1 = 0;
                int32_t sp;
                int hr, flip0, flip1;
                int32_t vx = pl->x, vy = pl->y, vz = pl->z;   /* Bildanfangs-Position */
                re15_g5_push_stats(&rufe0, &wirk0, NULL);
                flip0 = re15_g5_push_flips();
                hr = (int)pl->hit_react;
                frame();
                re15_g5_push_stats(&rufe1, &wirk1, NULL);
                flip1 = re15_g5_push_flips();
                if (pl->hp < 0) break;                     /* gefressen/tot: kein Standplatz mehr */
                m_bilder++;
                if (flip1 != flip0) m_flips++;
                /* Der Spiegel MUSS nach dem Bild die Position VOM BILDANFANG tragen
                 * (@0x8001d0d4-e4 schreibt ihn im Schleifenkopf). */
                if (pl->pos_s_x != (uint16_t)(int32_t)vx ||
                    pl->pos_s_y != (uint16_t)(int32_t)vy ||
                    pl->pos_s_z != (uint16_t)(int32_t)vz) m_spiegel_falsch++;
                sp = (pl->x > lx ? pl->x - lx : lx - pl->x)
                   + (pl->z > lz ? pl->z - lz : lz - pl->z);
                if (sp > m_maxsprung) { m_maxsprung = sp; m_sprung_x = pl->x; m_sprung_z = pl->z; }
                lx = pl->x; lz = pl->z;
                re15_collision_set_band(0);
                if (!begehbar(pl->x, pl->z, 0)) {
                    if (!m_raus) { m_raus_x = pl->x; m_raus_z = pl->z; }
                    m_raus++;
                }
                /* GEGENPROBE-Zaehler: Bilder, in denen der Koerper-Schub GAR NICHT gerufen
                 * wurde, obwohl der Boss lebt und der Spieler lebt. Im Original ist
                 * FUN_8002b544 @0x80031cbc NICHT an das Kommandowort gebunden (es steht
                 * NACH dem `jalr v0`-Dispatch @0x80031cb4 und hat als einzige Schranke
                 * `bltz g_pauseflags` @0x80031c78) — jedes solche Bild ist eine Divergenz. */
                if (rufe1 == rufe0) m_flinch_kontakt++;
                if (hr && wirk1 > wirk0) m_flinch_schub++;
                (void)hr;
            }
        }
        printf("  Startplaetze %d (verworfen %d) ; gemessene Bilder %d\n",
               m_starts, m_verworfen, m_bilder);
        printf("  Bilder auf UNBEGEHBAREM Punkt: %d%s\n", m_raus,
               m_raus ? "" : "  (keins)");
        if (m_raus) printf("    erster: (%d,%d)\n", (int)m_raus_x, (int)m_raus_z);
        printf("  groesster Ein-Bild-Weg des Spielers: %d  (nach (%d,%d))\n",
               (int)m_maxsprung, (int)m_sprung_x, (int)m_sprung_z);
        printf("  Bilder OHNE Schub-Aufruf (Port ueberspringt den Spieler-Schritt): %d\n",
               m_flinch_kontakt);
        printf("  Bilder mit Treffer-Reaktion UND wirksamem Schub: %d\n", m_flinch_schub);
        printf("  Vorzeichen-Drehungen des Schubs (+-2*r): %d ;"
               " Bilder mit falschem Positions-Spiegel: %d\n", m_flips, m_spiegel_falsch);
    }

    /* ================================================================================== *
     * A) REPRODUKTION: EIN Bild Schub + Klemme, von der Stelle des Nutzers aus.
     *    Gefahren wird die Reihenfolge von game_step_common.c:1380/1387 exakt nach:
     *      ox/oz = Pos VOR player_tick -> push -> constrain(rdt, ox, oz, &nx, &nz)
     * ================================================================================== */
    int rausgeschoben = 0, max_sprung = 0;
    int32_t worst_x = 0, worst_z = 0, worst_start_x = 0, worst_start_z = 0;
    int klemme_stumm = 0, klemme_aktiv = 0, faelle = 0;

    if (!strcmp(mode, "repro")) {
        printf("-- A) Ein-Bild-Schub aus der Umgebung der Marke -------------------------\n");
        printf("   Start           nach Schub       d(Schub)     nach Klemme     Klemme  begehbar?\n");
    }
    for (int32_t sz = -25400; sz <= -21400; sz += 250) {
        for (int32_t sx = 1000; sx <= 13000; sx += 250) {
            int32_t ox, oz, px, pz, nx, nz;
            int kontakt, tat, vorher_ok, nachher_ok;
            /* Startpunkt muss begehbar sein — sonst messen wir einen Ort, an dem der
             * Spieler nie stand. */
            if (!begehbar(sx, sz, 0)) continue;
            pl->x = sx; pl->z = sz; pl->y = 0; pl->hp = 100;
            ox = pl->x; oz = pl->z;               /* game_step_common.c:1375 (vor player_tick) */
            kontakt = re15_g5_body_push_player(pl);
            px = pl->x; pz = pl->z;
            if (!kontakt) continue;
            faelle++;
            nx = px; nz = pz;
            re15_collision_set_band(0);           /* re15_collision_ensure_band(pl->y=0) */
            re15_collision_constrain(&s_rdt, ox, oz, &nx, &nz);
            tat = (nx != px || nz != pz);
            if (tat) klemme_aktiv++; else klemme_stumm++;
            vorher_ok  = 1;                        /* per Filter oben */
            nachher_ok = begehbar(nx, nz, 0);
            if (!nachher_ok) {
                int sprung = abs((int)(nx - ox)) + abs((int)(nz - oz));
                rausgeschoben++;
                if (sprung > max_sprung) {
                    max_sprung = sprung; worst_x = nx; worst_z = nz;
                    worst_start_x = ox;  worst_start_z = oz;
                }
            }
            if (!strcmp(mode, "repro") && !nachher_ok) {
                printf("  (%6d,%6d) -> (%6d,%6d) d=(%6d,%6d) -> (%6d,%6d) %s  %s\n",
                       (int)ox, (int)oz, (int)px, (int)pz, (int)(px - ox), (int)(pz - oz),
                       (int)nx, (int)nz, tat ? "aktiv " : "STUMM ",
                       nachher_ok ? "ja" : "NEIN");
            }
            (void)vorher_ok;
        }
    }
    /* ⛔ Diese Zahl ist KEIN Riegel: die Startpunkte sind hier per Zuweisung TIEF in den
     * Segmenten, ein Zustand, in den das laufende Spiel den Spieler nicht bringt (der
     * Schub laeuft jedes Bild). Sie zeigt nur, wie weit EIN Schub reichen KANN. */
    printf("  [Diagnose, kein Riegel — gesetzte Tiefpositionen] Standplaetze mit Boss-Kontakt:"
           " %d ; Klemme aktiv %d / stumm %d ; nach Schub+Klemme UNBEGEHBAR: %d\n",
           faelle, klemme_aktiv, klemme_stumm, rausgeschoben);
    if (rausgeschoben)
        printf("  groesster Sprung: (%d,%d) -> (%d,%d)  Manhattan %d\n",
               (int)worst_start_x, (int)worst_start_z, (int)worst_x, (int)worst_z, max_sprung);

    /* ================================================================================== *
     * B) Wie gross wird EIN Schub maximal?  (Kandidat a: Sprung ueber die Wand)
     * ================================================================================== */
    {
        int32_t maxd = 0, mx = 0, mz = 0;
        for (int32_t sz = -27000; sz <= -19000; sz += 100) {
            for (int32_t sx = 0; sx <= 14000; sx += 100) {
                int32_t d;
                pl->x = sx; pl->z = sz; pl->y = 0; pl->hp = 100;
                if (!re15_g5_body_push_player(pl)) continue;
                d = abs((int)(pl->x - sx)) + abs((int)(pl->z - sz));
                if (d > maxd) { maxd = d; mx = sx; mz = sz; }
            }
        }
        printf("-- B) groesster Ein-Bild-Schub im Raster: %d (Manhattan) ab (%d,%d)\n",
               (int)maxd, (int)mx, (int)mz);
    }

    /* ================================================================================== *
     * D) DER POSITIONS-SPIEGEL +0x40/+0x42/+0x44. FUN_80034D0C hat einen
     *    VORZEICHEN-DREH-ZWEIG, dessen Hoehentest gegen den Spiegel laeuft; dreht er,
     *    ist der Schub +-2*r = +-12000 statt +-over. Das Original schreibt den Spiegel
     *    JEDES Bild aus der Spielerposition:
     *      8001d0b4 lhu v1,-13688(v1)   v1 = *(u16*)0x800aca88 = Spieler+0x34 (X)
     *      8001d0bc lhu a0,-13684(a0)   a0 = *(u16*)0x800aca8c = Spieler+0x38 (Y)
     *      8001d0c4 lhu a1,-13680(a1)   a1 = *(u16*)0x800aca90 = Spieler+0x3c (Z)
     *      8001d0d4 sh  v1,-13676(at)   -> 0x800aca94 = Spieler+0x40
     *      8001d0dc sh  a0,-13674(at)   -> 0x800aca96 = Spieler+0x42
     *      8001d0e4 sh  a1,-13672(at)   -> 0x800aca98 = Spieler+0x44
     *    Gemessen wird, wie oft der Dreh-Zweig feuert: mit korrektem Spiegel nie
     *    (h2 = y_vorher-1530-(boss_y-1500) = -30, |h2| < 3030), mit einem STEHEN-
     *    GEBLIEBENEN Leiter-Wert (climb_common.c ist im Port der einzige Schreiber) sehr wohl.
     * ================================================================================== */
    {
        int32_t g0x = 0, g0z = 0, g0r = 0;
        int flip_spiegel = 0, flip_null = 0, flip_alt = 0, treffer = 0;
        int32_t max_spiegel = 0, max_null = 0, max_alt = 0;
        re15_g5_body_segment(0, &g0x, NULL, &g0z, &g0r);
        for (int32_t dz2 = -2000; dz2 <= 2000; dz2 += 100) {
            for (int32_t ax = 200; ax <= 2000; ax += 100) {
                int32_t sx0 = g0x + g0r + 450 - ax, sz0 = g0z + dz2;   /* leicht IN Segment 0 */
                int f0, f1; int32_t d;
                if (!begehbar(sx0, sz0, 0)) continue;
                treffer++;
                /* (a) korrekter Spiegel = Position vom Bildanfang (@0x8001d0d4-e4) */
                pl->x = sx0; pl->z = sz0; pl->y = 0; pl->hp = 100;
                pl->pos_s_x = (uint16_t)sx0; pl->pos_s_y = (uint16_t)0; pl->pos_s_z = (uint16_t)sz0;
                f0 = re15_g5_push_flips(); re15_g5_body_push_player(pl); f1 = re15_g5_push_flips();
                flip_spiegel += (f1 - f0);
                d = labs((long)(pl->x - sx0)) + labs((long)(pl->z - sz0));
                if (d > max_spiegel) max_spiegel = d;
                /* (b) Port-Stand ohne Schreiber: alles 0 */
                pl->x = sx0; pl->z = sz0; pl->y = 0;
                pl->pos_s_x = 0; pl->pos_s_y = 0; pl->pos_s_z = 0;
                f0 = re15_g5_push_flips(); re15_g5_body_push_player(pl); f1 = re15_g5_push_flips();
                flip_null += (f1 - f0);
                d = labs((long)(pl->x - sx0)) + labs((long)(pl->z - sz0));
                if (d > max_null) max_null = d;
                /* (c) STEHENGEBLIEBENER Leiter-Wert: 2. Etage, anderer Raum */
                pl->x = sx0; pl->z = sz0; pl->y = 0;
                pl->pos_s_x = (uint16_t)(int16_t)-8000;
                pl->pos_s_y = (uint16_t)(int16_t)-3600;
                pl->pos_s_z = (uint16_t)(int16_t)-8000;
                f0 = re15_g5_push_flips(); re15_g5_body_push_player(pl); f1 = re15_g5_push_flips();
                flip_alt += (f1 - f0);
                d = labs((long)(pl->x - sx0)) + labs((long)(pl->z - sz0));
                if (d > max_alt) max_alt = d;
            }
        }
        printf("-- D) Positions-Spiegel: %d begehbare Kontaktpunkte geprueft --\n", treffer);
        printf("     (a) Spiegel = Bildanfang (@0x8001d0d4-e4): Drehungen %d, groesster Schub %d\n",
               flip_spiegel, (int)max_spiegel);
        printf("     (b) Spiegel 0 (Port ohne Schreiber):       Drehungen %d, groesster Schub %d\n",
               flip_null, (int)max_null);
        printf("     (c) Spiegel = alter Leiter-Wert:           Drehungen %d, groesster Schub %d\n",
               flip_alt, (int)max_alt);
        pl->pos_s_x = pl->pos_s_y = pl->pos_s_z = 0;
    }

    /* ================================================================================== *
     * C) Die ZWEI-ZELLEN-FALLE an der Stelle des Nutzers. Reine Messung, KEIN Riegel:
     *    sie ist Original-Geometrie + die byte-true Schleife von FUN_8003b0a4 (kein
     *    `break`, die LETZTE treffende Zelle gewinnt, @0x8003b510-20).
     * ================================================================================== */
    {
        int32_t nx = 11102, nz = -19232;
        re15_collision_set_band(0);
        re15_collision_constrain(&s_rdt, 11102, -19232, &nx, &nz);
        printf("-- C) Marke des Nutzers (11102,-19232): Klemme -> (%d,%d)  begehbar=%s\n",
               (int)nx, (int)nz, begehbar(nx, nz, 0) ? "ja" : "NEIN");
    }

    /* Die DUENNSTE Band-0-Wand zwischen Kampfkorridor und Halle, aus den SCA-Zellen
     * gemessen (Spielerradius 450 + Saum 0x12 auf jeder Seite, push_rect). */
    int32_t wand_dick = 0x7fffffff;
    for (int i = 0; i < s_rdt.sca_count; i++) {
        const re15_sca_entry_t *ce = &s_rdt.sca[i];
        if ((ce->floor >> 4) != 0 || ce->type != 1) continue;
        if (ce->x > 14000 || (int32_t)ce->x + ce->width < 5000) continue;
        if ((int32_t)ce->density > 6000) continue;       /* nur Waende, keine Grossflaechen */
        {   int32_t d = (int32_t)ce->density + 2 * (450 + 0x12);
            if (d < wand_dick) wand_dick = d;
        }
    }
    printf("  duennste Band-0-Wand im Kampfstreifen (mit Radius+Saum): %d\n", (int)wand_dick);
    {   int rufe = 0, wirk = 0; int32_t maxs = 0;
        re15_g5_push_stats(&rufe, &wirk, &maxs);
        printf("  Schub-Statistik ueber den ganzen Lauf: gerufen %d, gewirkt %d,"
               " groesster Ein-Bild-Schub %d\n", rufe, wirk, (int)maxs);
    }

    if (!strcmp(mode, "pin")) {
        printf("-- RIEGEL --\n");
        PIN(m_starts >= 35 && m_bilder >= 7000,
            "ABDECKUNG: %d Startplaetze x 200 Bilder = %d gemessene Bilder im echten"
            " re15_game_step (Boss lebendig, Spieler lebendig)", m_starts, m_bilder);
        /* DIE GEGENPROBE. Das Original ruft FUN_8002b544 @0x80031cbc NACH dem
         * Kommando-Dispatch `jalr v0` @0x80031cb4 und damit fuer JEDES Kommandowort;
         * einzige Schranke ist `bltz g_pauseflags` @0x80031c78. Vor dem Fix zaehlte diese
         * Zahl 22 (die Treffer-Reaktion des Bosses), danach 0. */
        PIN(m_flinch_kontakt == 0,
            "kein Bild ohne Koerper-Schub-Aufruf (@0x80031cbc steht AUSSERHALB der"
            " Kommando-Handler): %d Bilder", m_flinch_kontakt);
        PIN(m_raus == 0,
            "kein gemessenes Bild setzt den Spieler auf einen unbegehbaren Punkt"
            " (Wahrheit = Klemmpfad wie probe_p2_floor_dump): %d von %d", m_raus, m_bilder);
        PIN(m_maxsprung < wand_dick,
            "groesster Ein-Bild-Weg des Spielers %d bleibt unter der duennsten Wand %d"
            " — ein Sprung darueber landet hinter der Wand, wo die Klemme nichts mehr"
            " loest", (int)m_maxsprung, (int)wand_dick);
        PIN(m_flinch_schub >= 5,
            "der Schub wirkt jetzt AUCH in der Treffer-Reaktion: %d Bilder"
            " (vor dem Fix: 1)", m_flinch_schub);
        /* ZWEITE GEGENPROBE: der Positions-Spiegel Spieler+0x40/+0x42/+0x44 traegt nach
         * jedem Bild die Position VOM BILDANFANG (@0x8001d0d4-e4). Vor dem Fix schrieb ihn
         * NUR climb_common.c; in ROOM5090 stand er auf 0 bzw. auf einem stehengebliebenen
         * Leiter-Wert, und mit einem solchen Wert dreht der Schub 741 Mal und erreicht
         * 23911 Einheiten (Abschnitt D). */
        PIN(m_spiegel_falsch == 0,
            "der Positions-Spiegel traegt in jedem Bild die Bildanfangs-Position"
            " (@0x8001d0d4-e4): %d Abweichungen von %d", m_spiegel_falsch, m_bilder);
        PIN(m_flips == 0,
            "kein Bild dreht das Schub-Vorzeichen auf +-2*r (der Hoehentest gegen den"
            " Spiegel @0x80034ec4 bleibt im Band): %d Bilder", m_flips);
    }
    printf(s_fail ? "=== FEHLT (%d) ===\n" : "=== OK ===\n", s_fail);
    return s_fail ? 1 : 0;
}
