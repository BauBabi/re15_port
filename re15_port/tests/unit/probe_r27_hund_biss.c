/* probe_r27_hund_biss.c — Runde 27, Nutzer-Befund nach v0.8.13:
 *   "In 11d0, nachdem ich die Waffe aufhebe, und die Hunde angreifen haben es die Hunde
 *    mit dem Angriff wieder geschafft mich in die Wand zu treiben."
 *   Praezisierung: "der Spieler wurde durch den BISS der Hunde in die Wand getrieben.
 *    Nicht die hunde durch den Spieler."
 *
 * Reiner MESSSTAND. Kein Fix. Die einzige Engine-Beigabe ist die Stations-Schiene
 * (re15_schritt_station in game_step_common.c) — sie zeichnet nur auf.
 *
 * Modi:
 *   diag   — Raum hochfahren, Aktoren + SCA-Geometrie von ROOM11D0 auflisten
 *   lauf   — Spieler an einen Platz stellen, Hunde beissen lassen, JE BILD:
 *            Lage, Begehbarkeit, Hunde-Zustand, Biss und die STATIONEN des Bildes
 *   raster — derselbe Lauf ueber viele Plaetze am Wandsaum (Abdeckung)
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
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
#include "re15_esp.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

/* Stations-Schiene aus game_step_common.c (Deklaration: re15_game_step.h) */
static const char *k_station[] = {
    "anfang", "tick", "schub", "klemme", "zweig", "nachKI", "opfer", "griff", "ende"
};
#define STATIONEN RE15_SCHRITT_STATIONEN

static re15_rdt_t           s_rdt;
static re15_camera_view_t   s_cam;
static re15_game_ctx_t      s_ctx;
static re15_emd_animation_t s_pl00_anim;
static re15_emd_skeleton_t  s_pl00_skel;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int load_pl00(void)
{
    size_t a = 0, b = 0;
    uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &a);
    uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &b);
    return edd && emr &&
           re15_emd_parse_animation(edd, a, &s_pl00_anim) == 0 &&
           re15_emd_parse_skeleton (emr, b, &s_pl00_skel) == 0;
}

static uint8_t *s_ems2 = NULL;  static size_t s_ems2_n = 0;
static uint8_t *s_ems15 = NULL; static size_t s_ems15_n = 0;
static re15_enemy_bank_t *load_bank(uint8_t type, int re2)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return eb;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return NULL;
    if (re2) {
        if (!s_ems2) s_ems2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems2_n);
        if (s_ems2 && re2_ems_load_bank(s_ems2, s_ems2_n, (int)type, eb, NULL) == 0) {
            eb->buf = NULL; eb->ok = 1; return eb;
        }
    } else {
        if (!s_ems15) s_ems15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_ems15_n);
        int idx = s_ems15 ? re15_ems_index_for_type(type) : -1;
        size_t off = 0, len = 0;
        if (idx >= 0 && re15_ems_get_entry(s_ems15, s_ems15_n, idx, &off, &len) == 0) {
            uint8_t *blob = (uint8_t *)malloc(len);
            memcpy(blob, s_ems15 + off, len);
            if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, NULL) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->victim_ok = (re15_emd_parse_victim_bank(blob, len, &eb->skel_victim,
                                                            &eb->anim_victim) == 0);
                return eb;
            }
        }
    }
    eb->type = 0;
    return NULL;
}

static void frame(void)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    re15_schritt_station_reset();
    re15_game_step(&s_ctx);
}

/* Wahrheit fuer "begehbar" — WORTGLEICH zu probe_r20_birkin_push/probe_r21_tentakel_schub:
 * Teil 1 Containment (IN einer soliden Zelle), Teil 2 Klemmpfad (Klemme bewegt nicht). */
static int begehbar(int32_t x, int32_t z, int band)
{
    int32_t nx = x, nz = z;
    re15_collision_set_band(band);
    if (re15_collision_on_floor(&s_rdt, x, z)) return 0;
    re15_collision_constrain(&s_rdt, x, z, &nx, &nz);
    return (nx == x && nz == z);
}

static int s_zwinger = 0;   /* 1 = Case-0-Satz (Hunde in den Zwingern) statt des freien Satzes */

static void bringup(int sub_extra)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_victim_reset();
    re15_esp_fx_reset();
    re15_damage_seed_rng(0x0badf00du);
    { extern void re15_re2z_rng_reset(void); re15_re2z_rng_reset(); }  /* jeder Platz gleich */
    /* ⛔ DIE ENGINE-GLOBALEN DES RAUMS — ohne sie steigt JEDE Gegner-Wandklemme
     * (re15_enemy_sca_clamp: `if (!g_room_rdt_ok) return 0;`) sofort aus, und die Sonde
     * misst Hunde, die frei durch die Waende laufen. Gemessen, bevor diese Zeile stand:
     * 32939 von 186000 Hunde-Bildern IN einer soliden Zelle, 31451 ausserhalb des Raums. */
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    g_current_room_id = 0x11D0; g_room_change.pending = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    /* ⛔ WELCHE HUNDE? ROOM11D0 traegt ZWEI Saetze, und main00 waehlt sie aus
     * (RDT-Byte-Offsets, selbst gelesen und mit der Opcode-Tabelle des Ports dekodiert):
     *   @0x12AC  13 0a 40 03      Switch work_var 10, Blocklaenge 0x340
     *   @0x12B0  14 00 38 03 00 00  Case 0 (Laenge 0x338)
     *   @0x12B6  06 00 38 01      If, Rumpflaenge 0x138
     *   @0x12BA  21 03 98 00      Ck(Bank 3, Bit 152, Soll 0)  -> wahr, solange das Bit 0 ist
     *   @0x12BE..0x130E           fuenf Hunde Verhalten 0x41 an (-9500,-15000)/(-9500,-17800)/
     *                             (-4000,-13000)/(-4000,-15800)/(-4000,-18500) = IN DEN ZWINGERN
     *   @0x13EE  07 00 fe 01      Else (Laenge 0x1fe)
     *   @0x13F2..0x1442           fuenf Hunde Verhalten 0x00 an (-6100,-15000)/(-7650,-19100)/
     *                             (-7600,-13100)/(-3400,-21500)/(3200,-17700) = IM GANG, FREI
     * Die Zwinger-Plaetze liegen in vollstaendig umschlossenen Zellen-Kaefigen (gemessen:
     * mit gesetzter Raum-Klemme kommt dort in 1500 Bildern kein Hund heraus, 0 Bisse). Der
     * Nutzer wurde gebissen, also ist SEIN Fall der ELSE-Zweig = Bit 3:152 gesetzt.
     * "zwinger" als argv[2] waehlt stattdessen den Case-0-Satz. */
    re15_game_flag_set(3, 152, s_zwinger ? 0 : 1);
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    if (sub_extra > 0 && s_rdt.sub_scd_count > sub_extra && s_rdt.sub_scd[sub_extra])
        scd_thread_start(2, s_rdt.sub_scd[sub_extra]);
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = -300; pl->z = -17400; pl->rot_y = 1024;     /* re15_room_spawns.h ROOM11D0 */
    re15_collision_set_band(0);
}

static int hunde(int *slots, int max)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x20 && n < max) slots[n++] = s;
    return n;
}

static void diag(void)
{
    printf("-- SCA von ROOM11D0: %d Zellen (Regionen %d/%d/%d/%d/%d)\n",
           s_rdt.sca_count, s_rdt.sca_rgn[0], s_rdt.sca_rgn[1], s_rdt.sca_rgn[2],
           s_rdt.sca_rgn[3], s_rdt.sca_rgn[4]);
    for (int i = 0; i < s_rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &s_rdt.sca[i];
        printf("   #%-3d typ=%-2u u0=%02x u1=%02x fl=%u  x %6d..%-6d  z %6d..%-6d\n",
               i, c->type, c->u0, c->u1, c->floor,
               (int)c->x, (int)c->x + (int)c->width, (int)c->z, (int)c->z + (int)c->density);
    }
    printf("-- Aktoren nach dem Hochfahren:\n");
    for (int s = 0; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active)
            printf("   slot %-2d typ=0x%02X pos=(%d,%d,%d) st=%d s1=%d hp=%d grid=0x%02X\n",
                   s, g_actors[s].type, (int)g_actors[s].x, (int)g_actors[s].y,
                   (int)g_actors[s].z, g_actors[s].state, g_actors[s].sub_state_1,
                   g_actors[s].hp, g_actors[s].grid_id);
}

/* ---------------------------------------------------------------------------------- */
typedef struct {
    int bilder, bisse, latsch, angriffsbilder;
    int unbegehbar, unbegehbar_ohne_griff, unbegehbar_nach_biss;
    int im_zellinneren;              /* HART: Punkt liegt OHNE Radius in einer soliden Zelle */
    int hund_in_zelle;               /* Hunde-Bilder mit dem HUND in einer soliden Zelle */
    int hund_bilder;                 /* Hunde-Bilder gesamt (Abdeckung fuer die Zeile darueber) */
    int hund_ausserhalb;             /* Hunde-Bilder ausserhalb des Raum-Rechtecks */
    int hund_klemme_haette;          /* davon: die Hunde-Klemme haette ihn herausgeholt */
    int hund_fremdes_band;           /* davon: Hund traegt ein anderes Band (+0x82 != 0) */
    int32_t tiefe_max;               /* wie weit die erneute Klemme den Punkt noch schiebt */
    int32_t groesster_weg;
    int station_schuld[STATIONEN];   /* welche Station hat den Punkt unbegehbar gemacht */
    int schuld_hart[STATIONEN];      /* erste Station, deren Punkt IN einer soliden Zelle liegt */
    int32_t weg_je_glied[STATIONEN]; /* Summe der Verschiebung JE GLIED (Station i-1 -> i) */
    int32_t max_je_glied[STATIONEN];
    int     zahl_je_glied[STATIONEN];
} bilanz_t;

/* wie tief steckt der Punkt noch im Klemm-Saum? (0 = Fixpunkt der Klemme) */
static int32_t klemmtiefe(int32_t x, int32_t z)
{
    int32_t nx = x, nz = z;
    re15_collision_set_band(0);
    re15_collision_constrain(&s_rdt, x, z, &nx, &nz);
    int32_t dx = nx - x, dz = nz - z;
    return (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
}

static void lauf(int32_t sx, int32_t sz, int16_t yaw, int nframes, int ausfuehrlich,
                 bilanz_t *bl)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = sx; pl->z = sz; pl->y = 0; pl->rot_y = yaw; pl->hp = 100;
    pl->floor = 0; pl->state = 0; pl->motion = 0;
    re15_collision_set_band(0);

    int hs[8]; int nh = hunde(hs, 8);
    int nach_biss = 0;
    int32_t hund_vx[8], hund_vz[8];
    for (int i = 0; i < nh; i++) { hund_vx[i] = g_actors[hs[i]].x; hund_vz[i] = g_actors[hs[i]].z; }

    /* DIE HUNDE LOSLASSEN — der echte Weg des Raums: ROOM11D0 spawnt sie als Zwinger-/
     * Fensterhunde (grid 0x41 -> Zustand 4 Sub 1, enemy_ai_re2_dog.c:2212), und das SKRIPT
     * gibt sie mit der Marke 0x42 frei (`beq +0x9,0x42` @0x8011170c-14, Verbrauch
     * @0x80111718, Ausgang Wort 0xc01 @0x8011172c). Dieselbe Marke zuendet die Sonde. */
    for (int i = 0; i < nh; i++)
        if (g_actors[hs[i]].grid_id == 0x41) g_actors[hs[i]].grid_id = 0x42;

    for (int f = 0; f < nframes; f++) {
        int hp0 = pl->hp;
        int32_t vx = pl->x, vz = pl->z;
        int gr0 = re15_player_is_grabbed();
        if (pl->hp < 30) pl->hp = 100;        /* am Leben halten: der Nutzer ueberlebte auch */
        frame();
        int hit = (pl->hp < hp0);
        int gr  = re15_player_is_grabbed();
        int vs  = re15_player_victim_state();
        int ok  = begehbar(pl->x, pl->z, 0);
        int32_t weg = (pl->x - vx < 0 ? vx - pl->x : pl->x - vx)
                    + (pl->z - vz < 0 ? vz - pl->z : pl->z - vz);
        if (weg > bl->groesster_weg) bl->groesster_weg = weg;
        bl->bilder++;
        if (hit) { bl->bisse++; nach_biss = 1; }
        if (gr && !gr0) bl->latsch++;
        if (!ok) {
            bl->unbegehbar++;
            if (!gr && vs == 0) bl->unbegehbar_ohne_griff++;
            if (nach_biss)      bl->unbegehbar_nach_biss++;
            /* erste Station des Bildes, die auf einem unbegehbaren Punkt steht */
            for (int i = 0; i < STATIONEN; i++) {
                int32_t x, z;
                if (!re15_schritt_station_hole(i, &x, &z)) continue;
                if (!begehbar(x, z, 0)) { bl->station_schuld[i]++; break; }
            }
        }
        re15_collision_set_band(0);
        if (re15_collision_on_floor(&s_rdt, pl->x, pl->z)) {
            bl->im_zellinneren++;
            for (int i = 0; i < STATIONEN; i++) {   /* HARTE Schuld: erste Station IN der Zelle */
                int32_t x, z;
                re15_collision_set_band(0);
                if (!re15_schritt_station_hole(i, &x, &z)) continue;
                if (re15_collision_on_floor(&s_rdt, x, z)) { bl->schuld_hart[i]++; break; }
            }
        }
        re15_collision_set_band(0);
        {   int32_t t = klemmtiefe(pl->x, pl->z);
            if (t > bl->tiefe_max) bl->tiefe_max = t; }
        /* WEG JE GLIED: welcher Abschnitt des Schritts hat den Spieler bewegt? */
        {   int32_t px = 0, pz = 0, haben = 0;
            for (int i = 0; i < STATIONEN; i++) {
                int32_t x, z;
                if (!re15_schritt_station_hole(i, &x, &z)) continue;
                if (haben) {
                    int32_t d = (x - px < 0 ? px - x : x - px) + (z - pz < 0 ? pz - z : z - pz);
                    if (d) {
                        bl->weg_je_glied[i] += d;
                        bl->zahl_je_glied[i]++;
                        if (d > bl->max_je_glied[i]) bl->max_je_glied[i] = d;
                    }
                }
                px = x; pz = z; haben = 1;
            }
        }
        if (ausfuehrlich && (hit || !ok || gr != gr0 || weg > 0)) {
            re15_collision_set_band(0);
            printf("  f%-4d (%6d,%6d)->(%6d,%6d) weg=%-5d hp=%-4d %s%s%s%s vs=%d gr=%d cmd=%d mot=%d\n",
                   f, (int)vx, (int)vz, (int)pl->x, (int)pl->z, (int)weg, pl->hp,
                   hit ? "BISS " : "     ", ok ? "begehbar" : "  RAUS  ",
                   re15_collision_on_floor(&s_rdt, pl->x, pl->z) ? " IN-ZELLE" : "",
                   (gr && !gr0) ? " LATCH" : "", vs, gr, (int)pl->state, (int)pl->motion);
            int32_t px = 0, pz = 0, first = 1;
            for (int i = 0; i < STATIONEN; i++) {
                int32_t x, z;
                if (!re15_schritt_station_hole(i, &x, &z)) continue;
                if (first || x != px || z != pz) {
                    int bg = begehbar(x, z, 0);
                    re15_collision_set_band(0);
                    printf("        %-7s (%6d,%6d)%s%s\n", k_station[i], (int)x, (int)z,
                           bg ? "" : "  <-- unbegehbar",
                           re15_collision_on_floor(&s_rdt, x, z) ? " IN-ZELLE" : "");
                }
                px = x; pz = z; first = 0;
            }
            for (int i = 0; i < nh; i++) {
                re15_actor_t *d = &g_actors[hs[i]];
                if (!d->active) continue;
                printf("        hund%d slot%-2d st=%d s1=%-2d s2=%d pos=(%6d,%6d,%6d) hp=%d\n",
                       i, hs[i], d->state, d->sub_state_1, d->sub_state_2,
                       (int)d->x, (int)d->y, (int)d->z, d->hp);
            }
        }
        if (ausfuehrlich && (f % 100) == 0) {
            printf("  [f%-4d] Spieler (%6d,%6d)\n", f, (int)pl->x, (int)pl->z);
            for (int i = 0; i < nh; i++) {
                re15_actor_t *d = &g_actors[hs[i]];
                printf("        hund%d slot%-2d st=%d s1=%-2d s2=%d pos=(%6d,%6d,%6d)"
                       " yaw=%4d spd=%d aktiv=%d\n", i, hs[i], d->state, d->sub_state_1,
                       d->sub_state_2, (int)d->x, (int)d->y, (int)d->z, (int)d->rot_y,
                       (int)d->speed_h, d->active);
            }
        }
        for (int i = 0; i < nh; i++)
            if (g_actors[hs[i]].active && g_actors[hs[i]].sub_state_1 == 7) bl->angriffsbilder++;
        /* STEHEN DIE HUNDE SELBST IN DER GEOMETRIE? (ihre eigene Klemme ist
         * re15_enemy_sca_clamp mit Maske 4, gerufen im run_all-Tail) */
        for (int i = 0; i < nh; i++) {
            re15_actor_t *d = &g_actors[hs[i]];
            if (!d->active) continue;
            bl->hund_bilder++;
            re15_collision_set_band(re15_collision_band_from_y(d->y));
            if (re15_collision_on_floor(&s_rdt, d->x, d->z)) {
                bl->hund_in_zelle++;
                /* Wuerde die HUNDE-Klemme (Radius r_min, Maske 4, eigenes Band) ihn
                 * ueberhaupt herausholen? Wenn ja, ist sie nicht gelaufen. */
                int32_t hx = d->x, hz = d->z;
                re15_collision_constrain_enemy(&s_rdt, hund_vx[i], hund_vz[i], &hx, &hz,
                                               (int32_t)d->hit_radius_min, d->y, 4u);
                if (hx != d->x || hz != d->z) bl->hund_klemme_haette++;
                if (d->floor != 0) bl->hund_fremdes_band++;
            }
            if (d->x < -12500 || d->x > 8500 || d->z < -25000 || d->z > -10000)
                bl->hund_ausserhalb++;
            hund_vx[i] = d->x; hund_vz[i] = d->z;
        }
        re15_collision_set_band(0);
    }
}

static void bilanz_zeigen(const bilanz_t *bl)
{
    printf("ABDECKUNG: Bilder %d | Bisse %d | Latch %d | Biss-Latch-Bilder %d\n",
           bl->bilder, bl->bisse, bl->latsch, bl->angriffsbilder);
    printf("ERGEBNIS : unbegehbare Bilder %d (ohne Griff %d, nach dem ersten Biss %d)"
           " | groesster Ein-Bild-Weg %d\n",
           bl->unbegehbar, bl->unbegehbar_ohne_griff, bl->unbegehbar_nach_biss,
           (int)bl->groesster_weg);
    printf("HART     : Bilder MIT dem Punkt IN einer soliden Zelle: %d"
           " | groesste Rest-Klemmtiefe %d\n", bl->im_zellinneren, (int)bl->tiefe_max);
    printf("HUNDE    : Hunde-Bilder %d, davon MIT dem Hund in einer soliden Zelle %d,"
           " ausserhalb des Raumrechtecks %d\n",
           bl->hund_bilder, bl->hund_in_zelle, bl->hund_ausserhalb);
    printf("HUNDE    : von den Zellen-Bildern haette die Hunde-Klemme %d geloest;"
           " %d trugen ein fremdes Band\n", bl->hund_klemme_haette, bl->hund_fremdes_band);
    printf("SCHULD-HART:");
    for (int i = 0; i < STATIONEN; i++)
        if (bl->schuld_hart[i]) printf(" %s=%d", k_station[i], bl->schuld_hart[i]);
    printf("\n");
    printf("SCHULD   :");
    for (int i = 0; i < STATIONEN; i++)
        if (bl->station_schuld[i]) printf(" %s=%d", k_station[i], bl->station_schuld[i]);
    printf("\n");
    printf("WEG JE GLIED (Summe/Max/Zahl):\n");
    for (int i = 1; i < STATIONEN; i++)
        if (bl->zahl_je_glied[i])
            printf("   ->%-7s %8d /%6d /%5d\n", k_station[i], (int)bl->weg_je_glied[i],
                   (int)bl->max_je_glied[i], bl->zahl_je_glied[i]);
    if (bl->bisse == 0) printf("FEHLLAUF: 0 Bisse — die Sonde hat nichts gemessen\n");
}

int main(int argc, char **argv)
{
    const char *mode = (argc > 1) ? argv[1] : "diag";
    size_t rsz = 0;
    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM11D0.RDT", &rsz);
    if (!raw) { printf("FEHLT: ROOM11D0.RDT\n"); return 77; }
    if (re15_rdt_parse(raw, rsz, &s_rdt) != 0) { printf("FEHLT: RDT-Parse\n"); return 77; }
    if (!load_pl00()) { printf("FEHLT: PL00-Rig\n"); return 77; }

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    s_ctx.pl00_skel = &s_pl00_skel; s_ctx.pl00_anim = &s_pl00_anim;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);         /* Auslieferungs-Default seit 2026-08-22 */
    int sub_extra = (argc > 2) ? atoi(argv[2]) : 0;
    /* Bank VOR dem ersten Hochfahren: der Hunde-INIT zieht Cliplaengen aus ihr, ohne sie
     * laeuft der erste Lauf anders als jeder spaetere (gemessen: 10 statt 6 Bisse). */
    if (!load_bank(0x20, 1)) printf("WARNUNG: EM020-RE2-Bank fehlt\n");
    bringup(sub_extra);

    printf("=== R27 Hunde-Biss ROOM11D0 (%s, sub_extra=%d) ===\n", mode, sub_extra);
    if (!strcmp(mode, "diag")) { diag(); return 0; }

    int hs[8]; int nh = hunde(hs, 8);
    printf("Hunde gespawnt: %d\n", nh);
    for (int i = 0; i < nh; i++)
        printf("   hund%d slot%-2d pos=(%d,%d,%d) st=%d s1=%d r_min=%d r_max=%d h=%d"
               " sca_mask=%d\n", i, hs[i],
               (int)g_actors[hs[i]].x, (int)g_actors[hs[i]].y, (int)g_actors[hs[i]].z,
               g_actors[hs[i]].state, g_actors[hs[i]].sub_state_1,
               (int)g_actors[hs[i]].hit_radius_min, (int)g_actors[hs[i]].hit_radius_max,
               (int)g_actors[hs[i]].hit_height, (int)g_actors[hs[i]].sca_mask);
    if (nh == 0) { printf("FEHLLAUF: kein Hund im Raum — nichts gemessen\n"); return 1; }

    bilanz_t bl; memset(&bl, 0, sizeof bl);
    if (!strcmp(mode, "subs")) {
        /* Welcher SCD-Sub spawnt WELCHE Hunde? ROOM11D0 traegt ZWEI Hunde-Saetze
         * (RDT-Byte-Scan): @0x12BE-0x130E fuenf mit Verhalten 0x41 (Zwinger/Fenster)
         * und @0x13F2-0x1442 fuenf mit Verhalten 0x00 (frei laufend) + @0x15FA einen. */
        printf("Subs im RDT: %d ; main @0x%04X\n", s_rdt.sub_scd_count,
               s_rdt.main_scd ? (unsigned)(s_rdt.main_scd - raw) : 0u);
        for (int sb = 0; sb < s_rdt.sub_scd_count; sb++)
            printf("   sub%-2d @0x%04X\n", sb,
                   s_rdt.sub_scd[sb] ? (unsigned)(s_rdt.sub_scd[sb] - raw) : 0u);
        for (int sb = 0; sb < s_rdt.sub_scd_count && sb < 24; sb++) {
            bringup(sb);
            int hs2[8]; int n2 = hunde(hs2, 8);
            if (!n2) continue;
            printf("  sub%-2d: %d Hunde ->", sb, n2);
            for (int i = 0; i < n2; i++)
                printf(" [slot%d grid=0x%02X (%d,%d)]", hs2[i], g_actors[hs2[i]].grid_id,
                       (int)g_actors[hs2[i]].x, (int)g_actors[hs2[i]].z);
            printf("\n");
        }
        return 0;
    }
    if (!strcmp(mode, "punkt")) {
        /* Ein Punkt und sein Klemmpfad: Quadrant, treffende Zellen, Fixpunkt-Test. */
        int32_t px = (argc > 3) ? atoi(argv[3]) : 0, pz = (argc > 4) ? atoi(argv[4]) : 0;
        int32_t vx = (argc > 5) ? atoi(argv[5]) : px, vz = (argc > 6) ? atoi(argv[6]) : pz;
        re15_collision_set_band(0);
        printf("Decke (Quadranten-Teiler): x=%d z=%d ; Regionen %d/%d/%d/%d/%d\n",
               (int)s_rdt.ceiling_x, (int)s_rdt.ceiling_z, s_rdt.sca_rgn[0], s_rdt.sca_rgn[1],
               s_rdt.sca_rgn[2], s_rdt.sca_rgn[3], s_rdt.sca_rgn[4]);
        int32_t nx = px, nz = pz;
        re15_collision_constrain(&s_rdt, vx, vz, &nx, &nz);
        int32_t cx = nx, cz = nz;
        re15_collision_constrain(&s_rdt, nx, nz, &cx, &cz);
        printf("Klemme von (%d,%d) nach (%d,%d) -> (%d,%d) ; erneut geklemmt -> (%d,%d)\n",
               (int)vx, (int)vz, (int)px, (int)pz, (int)nx, (int)nz, (int)cx, (int)cz);
        printf("in Zelle (ohne Radius): Ziel %d, nach Klemme %d\n",
               re15_collision_on_floor(&s_rdt, px, pz),
               re15_collision_on_floor(&s_rdt, nx, nz));
        for (int runde = 0; runde < 2; runde++) {
            int32_t qx = runde ? nx : px, qz = runde ? nz : pz;
            unsigned zb = (unsigned)(qz - (int32_t)s_rdt.ceiling_z) & 0x80000000u;
            unsigned xb = (unsigned)(qx - (int32_t)s_rdt.ceiling_x) & 0x80000000u;
            int q = (int)((zb | (xb >> 1)) >> 30);
            int st = 0; for (int i = 0; i < q && i < 5; i++) st += s_rdt.sca_rgn[i];
            int en = st + (q < 5 ? s_rdt.sca_rgn[q] : 0);
            printf("  %s (%d,%d): Quadrant %d, Zellen %d..%d\n",
                   runde ? "nach Klemme" : "Ziel      ", (int)qx, (int)qz, q, st, en - 1);
            for (int i = st; i < en && i < s_rdt.sca_count; i++) {
                const re15_sca_entry_t *e = &s_rdt.sca[i];
                if (0 != (e->floor >> 4)) continue;
                int breit = ((unsigned)(qx - ((int32_t)e->x - 450)) < (unsigned)((int32_t)e->width + 900) &&
                             (unsigned)(qz - ((int32_t)e->z - 450)) < (unsigned)((int32_t)e->density + 900));
                int drin  = ((unsigned)(qx - (int32_t)e->x) < (unsigned)e->width &&
                             (unsigned)(qz - (int32_t)e->z) < (unsigned)e->density);
                if (breit || drin)
                    printf("      Zelle #%-3d typ=%u u0=%02x x %6d..%-6d z %6d..%-6d %s%s\n",
                           i, e->type, e->u0, (int)e->x, (int)e->x + (int)e->width,
                           (int)e->z, (int)e->z + (int)e->density,
                           breit ? "[Broadphase]" : "", drin ? " [DRIN]" : "");
            }
        }
        return 0;
    }
    if (!strcmp(mode, "karte")) {
        /* Begehbarkeits-Karte Band 0, Schrittweite 500 — zur Wahl der Wandplaetze */
        for (int32_t z = -24500; z <= -10500; z += 500) {
            printf("%7d ", (int)z);
            for (int32_t x = -12500; x <= 8500; x += 500)
                putchar(begehbar(x, z, 0) ? '.' : '#');
            putchar('\n');
        }
        printf("        ");
        for (int32_t x = -12500; x <= 8500; x += 500)
            putchar((x % 5000 == 0) ? '|' : ' ');
        printf("\n        x von -12500 bis 8500 (Schritt 500); '|' alle 5000\n");
        return 0;
    }
    if (!strcmp(mode, "raster")) {
        /* Abdeckung: viele Startplaetze, je ein Lauf. Ein Platz zaehlt nur, wenn er
         * mindestens einen Biss gesehen hat (sonst ist er ein Fehllauf). */
        int plaetze = 0, gemessen = 0, betroffen = 0;
        int frames = (argc > 3) ? atoi(argv[3]) : 400;
        for (int32_t z = -23500; z <= -11500; z += 1000) {
            for (int32_t x = -11500; x <= 7500; x += 1000) {
                if (!begehbar(x, z, 0)) continue;
                plaetze++;
                bilanz_t b; memset(&b, 0, sizeof b);
                bringup(sub_extra);
                lauf(x, z, 1024, frames, 0, &b);
                if (b.bisse == 0) continue;
                gemessen++;
                bl.bilder += b.bilder; bl.bisse += b.bisse; bl.latsch += b.latsch;
                bl.unbegehbar += b.unbegehbar;
                bl.unbegehbar_ohne_griff += b.unbegehbar_ohne_griff;
                bl.unbegehbar_nach_biss  += b.unbegehbar_nach_biss;
                if (b.groesster_weg > bl.groesster_weg) bl.groesster_weg = b.groesster_weg;
                bl.im_zellinneren += b.im_zellinneren;
                bl.hund_in_zelle  += b.hund_in_zelle;
                bl.hund_bilder    += b.hund_bilder;
                bl.hund_ausserhalb += b.hund_ausserhalb;
                bl.hund_klemme_haette += b.hund_klemme_haette;
                bl.hund_fremdes_band  += b.hund_fremdes_band;
                if (b.tiefe_max > bl.tiefe_max) bl.tiefe_max = b.tiefe_max;
                for (int i = 0; i < STATIONEN; i++) {
                    bl.station_schuld[i] += b.station_schuld[i];
                    bl.schuld_hart[i]    += b.schuld_hart[i];
                    bl.weg_je_glied[i]   += b.weg_je_glied[i];
                    bl.zahl_je_glied[i]  += b.zahl_je_glied[i];
                    if (b.max_je_glied[i] > bl.max_je_glied[i])
                        bl.max_je_glied[i] = b.max_je_glied[i];
                }
                if (b.unbegehbar) {
                    betroffen++;
                    printf("  PLATZ (%6d,%6d): %d unbegehbare Bilder von %d, Bisse %d,"
                           " groesster Weg %d\n",
                           (int)x, (int)z, b.unbegehbar, b.bilder, b.bisse,
                           (int)b.groesster_weg);
                }
            }
        }
        printf("PLAETZE  : %d begehbare Startplaetze, davon %d mit mindestens einem Biss,"
               " %d mit unbegehbaren Bildern\n", plaetze, gemessen, betroffen);
        bilanz_zeigen(&bl);
        return 0;
    }
    if (!strcmp(mode, "lauf")) {
        int32_t sx = (argc > 3) ? (int32_t)atoi(argv[3]) : -300;
        int32_t sz = (argc > 4) ? (int32_t)atoi(argv[4]) : -17400;
        lauf(sx, sz, 1024, (argc > 5) ? atoi(argv[5]) : 600, 1, &bl);
        bilanz_zeigen(&bl);
        return 0;
    }
    printf("unbekannter Modus\n");
    return 1;
}
