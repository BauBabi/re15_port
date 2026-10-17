/* test_r38_npckante.c — Runde 38 Spur M "npckante" (Dossier analysis/befunde_runde38/M_npckante.md).
 *
 * NUTZER (2026-10-16): "Kendo springt am Ende von ROOM 10B0 nicht die Kante runter, wie im Original, sondern laeuft
 * in die Luft weiter" / "Bei Room 1260 Springt Kendo auch nicht die Kanten runter. Machen Marvin und Ada in Leons
 * Szenario uebrigens auch immer nicht."
 *
 * MECHANIK (info/Re1.5/PSX.EXE, Belege in include/re15_npckante_r38.h und engine/src/npckante_r38.c): die NPC-
 * Plc_dest-Subs 4/5 rufen nach jedem Schritt die Kantensonde FUN_8002d7d8 (@0x80051374 / @0x800516c4); an einer
 * Absprungkante (SCA-Zellwort Bit 1) springt der NPC ueber Sub 0x12/0x16 -> 0x18/0x17 (0x80052c14) hinunter, an einer
 * Kletterkante (Bit 0) klettert er in Sub 0x12/0x16 (0x80052508) hinauf.
 *
 * Teile (argv[1]):
 *   clips     die Kanten-Clips der Loco-Bank (+0x84/+0x16c) je NPC: Clip 2 (Klettern, Bild 0x11/0x1d/0x25 erreichbar),
 *             Clip 3 (Absprung, Bild 0xd erreichbar), Clip 4 (Flug), Clip 5 (Landung) — Bildzahl und Wurzel-Y
 *   sonde     FUN_8002d7d8-Zwilling gegen die SCA-Daten an den Stellen aus der Vorher-Messung (exe, 2026-10-16)
 *   john1261  Elza ROOM1261 Eintritt Cut 15 (3,0x41): John springt suedlich vom Steg, laeuft unten, klettert hoch
 *   ada1260   Leon ROOM1260 Eintritt Cut 7 (4,0xF4): Ada UND Marvin springen nordlich vom Steg, klettern auf den Block
 *   john10b1  Elza ROOM10B1 sub02 (3,0x80), Cut 2: John springt vom Band-5-Steg (Zelle [31] u1=0x0e) hinunter
 *   gegen     Gegenprobe ohne Kante: Ada ROOM1050 (sub03 Plc_dest 9/5 nach Eintritt Cut 6) — kein Kanten-Sub
 * Riegel je Raum: waehrend Sub 4/5/6/9 steht der NPC auf dem Boden unter sich (y == room_coll-Boden seines Bandes),
 * der Kanten-Sub laeuft mit den Original-Clips, Band und y folgen dem Boden, und das Skript laeuft zu Ende.
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
#include "re15_roy_r37.h"
#include "re15_gameflow.h"
#include "re15_engine.h"
#include "re15_npckante_r38.h"
#include "re15_fade.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int g_fail = 0;
#define PRUEF(c, ...) do { if (c) { printf("  OK   "); printf(__VA_ARGS__); printf("\n"); } \
                           else  { printf("  FAIL "); printf(__VA_ARGS__); printf("\n"); g_fail++; } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b && n) *n = (size_t)sz; return b;
}

/* ---- Modellarchive (CDEMD0 = Leon, CDEMD1 = Elza; Wahl FUN_80022300 @0x80022334) --------------------------------- */
static uint8_t *s_ems[2]; static size_t s_ems_n[2];
static uint8_t  s_blob[4][0x60000]; static int s_blob_n = 0;

static re15_enemy_bank_t *bank_laden(int archiv, uint8_t type)
{
    if (!s_ems[archiv]) {
        char p[600]; snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, re15_ems_archiv_datei(archiv));
        s_ems[archiv] = slurp(p, &s_ems_n[archiv]);
        if (!s_ems[archiv]) { printf("  FEHLER: %s nicht lesbar\n", p); g_fail++; return NULL; }
    }
    uint32_t sek = 0, len = 0;
    if (!re15_ems_toc(archiv, type, &sek, &len) || (size_t)sek * 2048u + len > s_ems_n[archiv] ||
        len > sizeof s_blob[0] || s_blob_n >= 4) { printf("  FEHLER: Typ 0x%02X Archiv %d\n", type, archiv); g_fail++; return NULL; }
    uint8_t *b = s_blob[s_blob_n++];
    memcpy(b, s_ems[archiv] + (size_t)sek * 2048u, len);
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return NULL;
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(b, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) { eb->type = 0; return NULL; }
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok = (re15_emd_parse_loco_bank(b, len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->own_ok  = (re15_emd_parse_own_bank(b, len, &eb->skel_own, &eb->anim_own) == 0);
    return eb;
}

static int clip_fc(const re15_emd_animation_t *a, int c)
{
    return (a && c >= 0 && c < a->clip_count) ? a->clips[c].frame_count : -1;
}

/* Wurzel-Y (Keyframe-Position py) des Bildes f von Clip c */
static int wurzel(const re15_emd_skeleton_t *sk, const re15_emd_animation_t *an, int c, int f, int achse)
{
    uint32_t w = re15_emd_get_frame_entry(an, c, f);
    int16_t x = 0, y = 0, z = 0;
    if (!re15_emd_get_keyframe_position(sk, (int)(w & 0xFFFu), &x, &y, &z)) return 0;
    return achse == 0 ? x : achse == 1 ? y : z;
}

/* ================================================================================================================== */
static void teil_clips(void)
{
    printf("== clips: Kanten-Clips der Loco-Bank (+0x84/+0x16c = dir[2]/dir[1], FUN_80022300 @0x800224a8/98) ==\n");
    struct { int archiv; uint8_t typ; const char *wer; } k[] = {
        { RE15_EMS_ARCHIV_ELZA, 0x49, "John (Elza, CDEMD1)" },
        { RE15_EMS_ARCHIV_LEON, 0x40, "Marvin (Leon, CDEMD0)" },
        { RE15_EMS_ARCHIV_LEON, 0x42, "Ada (Leon, CDEMD0)" },
    };
    for (int i = 0; i < 3; i++) {
        re15_enemy_reset(); s_blob_n = 0;
        re15_enemy_bank_t *eb = bank_laden(k[i].archiv, k[i].typ);
        if (!eb || !eb->loco_ok) { PRUEF(0, "%s: Loco-Bank fehlt", k[i].wer); continue; }
        const re15_emd_animation_t *an = &eb->anim_loco; const re15_emd_skeleton_t *sk = &eb->skel_loco;
        printf("  %s: Loco-Bank %d Clips, eigene Bank %d Clips (Clip 5 = %d Bilder)\n", k[i].wer, an->clip_count,
               eb->own_ok ? eb->anim_own.clip_count : -1, eb->own_ok ? clip_fc(&eb->anim_own, 5) : -1);
        for (int c = 2; c <= 5; c++) {
            int fc = clip_fc(an, c);
            printf("    Clip %d: %d Bilder; Wurzel (x,y,z) je Bild:", c, fc);
            for (int f = 0; f < fc; f++)
                printf(" %d:(%d,%d,%d)", f, wurzel(sk, an, c, f, 0), wurzel(sk, an, c, f, 1), wurzel(sk, an, c, f, 2));
            printf("\n");
            /* Frame-Wort-SE-Bits (FUN_8001b38c: `srl s0,v0,22` @0x8001b3b4, Wurzel-Tail 0x49 @0x8011dd60) und das
             * Schritt-Bit 0x4000 (Kanten-Sub [1] @0x800525b4, nur Clip 5 der eigenen Bank) */
            printf("    Clip %d Frame-Wort >> 22 (SE-Bits) je Bild:", c);
            for (int f = 0; f < fc; f++) {
                uint32_t w = re15_emd_get_frame_entry(an, c, f);
                if (w >> 22) printf(" %d:0x%03x", f, (unsigned)(w >> 22));
            }
            printf("\n");
        }
        if (eb->own_ok && clip_fc(&eb->anim_own, 5) > 0) {
            printf("    eigene Bank Clip 5 (Rast, Kanten-Sub [1]) Frame-Wort & 0x4000 / >> 22 je Bild:");
            for (int f = 0; f < clip_fc(&eb->anim_own, 5); f++) {
                uint32_t w = re15_emd_get_frame_entry(&eb->anim_own, 5, f);
                if ((w & 0x4000u) || (w >> 22)) printf(" %d:%s0x%03x", f, (w & 0x4000u) ? "S+" : "", (unsigned)(w >> 22));
            }
            printf("\n");
        }
        PRUEF(clip_fc(an, 2) > 0x25, "%s: Kletter-Clip 2 hat %d Bilder (> 0x25: Bild 0x11/0x1d/0x25 @0x800526d8/704/7c8)",
              k[i].wer, clip_fc(an, 2));
        PRUEF(clip_fc(an, 3) > 0x0c, "%s: Absprung-Clip 3 hat %d Bilder (Bild 0xd @0x80052cbc erreichbar)",
              k[i].wer, clip_fc(an, 3));
        PRUEF(clip_fc(an, 4) > 0 && clip_fc(an, 5) > 0, "%s: Flug-Clip 4 (%d) und Lande-Clip 5 (%d) vorhanden",
              k[i].wer, clip_fc(an, 4), clip_fc(an, 5));
    }
}

/* ================================================================================================================== */
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_rdt_buf = NULL;

static int raum_laden(const char *rel)
{
    char p[600]; snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, rel);
    size_t n = 0;
    free(s_rdt_buf);
    s_rdt_buf = slurp(p, &n);
    if (!s_rdt_buf) { printf("  FEHLER: %s fehlt\n", p); g_fail++; return 0; }
    if (re15_rdt_parse(s_rdt_buf, n, &g_room_rdt) != 0) { printf("  FEHLER: RDT-Parse %s\n", p); g_fail++; return 0; }
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;
    return 1;
}

static void sonde_bei(uint8_t typ, int32_t x, int32_t y, int32_t z, int rot, int band, int soll, const char *wo)
{
    re15_actor_t e; memset(&e, 0, sizeof e);
    e.active = 1; e.type = typ; e.x = x; e.y = y; e.z = z; e.rot_y = (int16_t)rot; e.floor = (uint8_t)band;
    e.hit_radius_min = 450;
    g_scd.prop_count = 0;
    int r = re15_npckante_sonde(&e);
    PRUEF(r == soll, "%s: Sonde (%d,%d,%d) Kurs %d Band %d = %d (soll %d)", wo, (int)x, (int)y, (int)z, rot, band, r, soll);
}

static void teil_sonde(void)
{
    printf("== sonde: FUN_8002d7d8 gegen die SCA-Daten ==\n");
    scd_vm_init();
    if (raum_laden("STAGE1/ROOM1261.RDT")) {
        /* Vorher-Messung exe F143: John (-20477,-1800,-17374) Kurs 979 Band 1 -> Probe z-800 in Zelle [29]
         * (Band 1, u1=0x02, z -21250..-18100) */
        sonde_bei(0x49, -20477, -1800, -17374, 979, 1, 2, "ROOM1261 Absprung Sued (Zelle [29])");
        sonde_bei(0x49, -20487, -1800, -17226, 979, 1, 0, "ROOM1261 ein Schritt davor");
        /* unten am Oststeg nach Norden: Probe z+800 in Zelle [15] (Band 0, u1=0x01, z -18100..-2100) */
        sonde_bei(0x49, 20300, 0, -18900, 3072, 0, 1, "ROOM1261 Kletterkante Ost (Zelle [15])");
        sonde_bei(0x49, 20300, 0, -19000, 3072, 0, 0, "ROOM1261 Kletterkante noch 900 entfernt");
    }
    if (raum_laden("STAGE1/ROOM1260.RDT")) {
        /* Vorher-Messung exe F53: Ada (26750,-1800,14500) Kurs 3072 Band 1 -> Zelle [28] (z 15250..18350) */
        sonde_bei(0x42, 26750, -1800, 14500, 3072, 1, 2, "ROOM1260 Absprung Nord (Zelle [28])");
        /* Ada unten nach Sueden (Kurs 1024 = -z): Probe z-800 in Zelle [12] (Band 0, u1=0x01, z bis 15250) */
        sonde_bei(0x42, -7600, 0, 16025, 1024, 0, 1, "ROOM1260 Kletterkante Mitte (Zelle [12])");
    }
    if (raum_laden("STAGE1/ROOM10B1.RDT")) {
        /* Vorher-Messung exe F115: John (-3661,-9000,-8275) Kurs 2167 Band 5 -> Zelle [31] (Band 5, u1=0x0e);
         * room_coll(Probe, 1, 5, 0x100) = Oberkante Block [6] (Band 0) = -1800, -9000 < -1800 -> 2 */
        sonde_bei(0x49, -3661, -9000, -8275, 2167, 5, 2, "ROOM10B1 Absprung Band-5-Steg (Zelle [31])");
        /* auf Block [6] (Band 1): Zelle [18] (Band 1, u1=0x02, x -9070..-7162), Boden dahinter 0 */
        sonde_bei(0x49, -6500, -1800, -7710, 2048, 1, 2, "ROOM10B1 Absprung Block [6] (Zelle [18])");
        /* gleiche Stelle, aber y = 0 (schon unten): kein Sprung (y < Boden falsch) */
        sonde_bei(0x49, -6500, 0, -7710, 2048, 1, 0, "ROOM10B1 Zelle [18], NPC schon auf Bodenhoehe");
    }
}

/* ================================================================================================================== */
typedef struct {
    int seen_kante, seen_sprung, seen_hoch, seen_land;   /* Phasen beobachtet */
    int luft;             /* Bilder in Sub 4/5/6/9 mit y != Boden */
    int luft_vor_fix;     /* (nur Info) Bilder mit y < Boden - 100 */
    int y_min_sprung, y_max_sprung;
    int clip_sprung[8];   /* je Clip-Index im Sprung-Sub gesehen */
    int clip_hoch;        /* Clip 2 im Kanten-Sub Phase 3 gesehen */
    int band_end; int32_t x_end, y_end, z_end; int sub_end;
    int band_wechsel;     /* Zahl der Bandwechsel */
    int sprung51, sprung10, hoch01;   /* Bandwechsel 5->1, 1->0 (Sprung), 0->1 (Klettern) */
    /* MECHANIK (Fortsetzung 2026-10-17): Fallkurve, Landung und Kletterstufe Bild fuer Bild gegen die Original-Konstanten */
    int32_t y_vor, x_vor, z_vor; int s1_vor, s2_vor, band_vor, hat_vor;
    int fall_k;                     /* Fall-Bild-Index im laufenden Sprung (1 = erstes Bild nach dem Schub)           */
    int fall_bilder, fall_fehler;   /* Schub-Bild: dy = 500 + 90 (@0x80052d68 + @0x80052e34); danach dy = 90 + 50 k
                                     * (@0x80052e34-5c, k = +0x9c)                                                    */
    int lande_ok, lande_fehler;     /* Landebild [4]: y = +0x1ba (@0x80052efc)                                        */
    int stufe_ok, stufe_fehler;     /* Kletterstufe [3] Bild 0x1d: dy = -1800 (@0x80052730), Weg 0x47c (@0x80052710),
                                     * Band + 1 (@0x80052768); sonst KEINE y-Aenderung im Kanten-Sub                  */
    int32_t x_sprung_unten[8]; int n_sprung_unten;   /* Lage nach jedem Sprung auf Band 0 (Landebild)                  */
    int32_t x_stufe[8]; int n_stufe;                  /* Lage nach jeder Kletterstufe                                   */
} spur_t;

/* "In der Luft" = was die Engine-Schwerkraft FUN_8001bd60 sofort fallen liesse: y auf der Bandhoehe -(Band*1800)
 * (@0x8001bdc8-ec) und der Mittelpunkt mindestens den Radius tief in einer Absprungzelle (Bit 1) des eigenen Bandes
 * (FUN_8003b7f0(&+0x34, -r, +0x82) @0x8001bd94-b4), ODER y gar nicht auf der Bandhoehe. */
static int in_der_luft(const re15_actor_t *e)
{
    if ((int)e->y != -(int)e->floor * 1800) return 1;
    uint16_t w = re15_collision_floor_typeword(&g_room_rdt, e->x, e->z, (int)e->floor, -(int32_t)e->hit_radius_min);
    return (w & 2u) ? 1 : 0;
}

static void spur_tick(spur_t *sp, const re15_actor_t *e, int *letztes_band, int f, int log)
{
    int s1 = e->sub_state_1;
    if (e->state == 4 && (s1 == 0x12 || s1 == 0x16)) {
        sp->seen_kante++;
        if (e->sub_state_2 >= 3 && e->motion == 2) sp->clip_hoch++;
        if (e->sub_state_2 >= 3) sp->seen_hoch++;
    }
    if (e->state == 4 && (s1 == 0x17 || s1 == 0x18)) {
        sp->seen_sprung++;
        if (e->motion >= 0 && e->motion < 8) sp->clip_sprung[e->motion]++;
        if (e->y < sp->y_min_sprung) sp->y_min_sprung = (int)e->y;
        if (e->y > sp->y_max_sprung) sp->y_max_sprung = (int)e->y;
        if (e->sub_state_2 >= 5) sp->seen_land++;
    }
    if (e->state == 4 && (s1 == 4 || s1 == 5 || s1 == 6 || s1 == 9) && in_der_luft(e)) {
        sp->luft++;
        if (log && sp->luft < 6)
            printf("    F%d Slot %ld Sub %d: in der Luft, y %d bei (%d,%d) Band %d\n", f, (long)(e - g_actors), s1,
                   (int)e->y, (int)e->x, (int)e->z, (int)e->floor);
    }
    if (*letztes_band != (int)e->floor) {
        if (*letztes_band >= 0) {
            sp->band_wechsel++;
            if (*letztes_band == 5 && e->floor == 1) sp->sprung51++;
            if (*letztes_band == 1 && e->floor == 0) sp->sprung10++;
            if (*letztes_band == 0 && e->floor == 1) sp->hoch01++;
            if (log) printf("    F%d Slot %ld: Band %d -> %d, y %d, (%d,%d), Sub 0x%02X Phase %d\n", f,
                            (long)(e - g_actors), *letztes_band, (int)e->floor, (int)e->y, (int)e->x, (int)e->z, s1,
                            e->sub_state_2);
        }
        *letztes_band = (int)e->floor;
    }
    if (sp->hat_vor && e->state == 4) {
        int sprung_jetzt = (s1 == 0x17 || s1 == 0x18), sprung_vor = (sp->s1_vor == 0x17 || sp->s1_vor == 0x18);
        int kante_jetzt = (s1 == 0x12 || s1 == 0x16),  kante_vor = (sp->s1_vor == 0x12 || sp->s1_vor == 0x16);
        int32_t dy = e->y - sp->y_vor;
        if (sprung_jetzt && sprung_vor) {
            if (sp->s2_vor == 2 && (e->sub_state_2 == 3 || e->sub_state_2 == 4)) {          /* Schub-Bild [2] -> [3] */
                sp->fall_k = 1; sp->fall_bilder++;
                if (dy != 500 + 90) { sp->fall_fehler++; if (log) printf("    F%d Schub-Bild dy %d (soll 590)\n", f, (int)dy); }
            } else if (sp->s2_vor == 3 && (e->sub_state_2 == 3 || e->sub_state_2 == 4)) {   /* Fall-Bild [3] */
                sp->fall_bilder++;
                if (dy != 90 + 50 * sp->fall_k) {
                    sp->fall_fehler++;
                    if (log) printf("    F%d Fall-Bild %d dy %d (soll %d)\n", f, sp->fall_k, (int)dy, 90 + 50 * sp->fall_k);
                }
                sp->fall_k++;
            } else if (sp->s2_vor == 4 && e->sub_state_2 == 5) {                           /* Landebild [4] */
                if (e->y == (int32_t)e->dog_floor_y) sp->lande_ok++; else sp->lande_fehler++;
                if (e->floor == 0 && sp->n_sprung_unten < 8) sp->x_sprung_unten[sp->n_sprung_unten++] = e->x;
            }
        }
        if (kante_jetzt && kante_vor && dy != 0) {
            double weg = sqrt((double)(e->x - sp->x_vor) * (e->x - sp->x_vor) + (double)(e->z - sp->z_vor) * (e->z - sp->z_vor));
            if (dy == -1800 && weg > 1146.0 && weg < 1150.0 && (int)e->floor == sp->band_vor + 1) {
                sp->stufe_ok++;
                if (sp->n_stufe < 8) sp->x_stufe[sp->n_stufe++] = e->x;
            } else {
                sp->stufe_fehler++;
                if (log) printf("    F%d Kanten-Sub dy %d Weg %.1f Band %d -> %d\n", f, (int)dy, weg, sp->band_vor, (int)e->floor);
            }
        }
    }
    sp->y_vor = e->y; sp->x_vor = e->x; sp->z_vor = e->z; sp->s1_vor = s1; sp->s2_vor = e->sub_state_2;
    sp->band_vor = e->floor; sp->hat_vor = 1;
    sp->band_end = e->floor; sp->x_end = e->x; sp->y_end = e->y; sp->z_end = e->z; sp->sub_end = s1;
}

/* > 0: Viereck alle n Bilder (= Nachricht bestaetigen: msg_common.c CONFIRM virt. 0x4000 <- RAW SQUARE) */
static int s_druck_takt = 0;
/* 1: Szenen-Schritt wie test_r36_szenenleon.c frame() (Letterbox/Cine-Sperre, Kamera-Apply) — fuer Szenen, in denen
 * auch der SPIELER per Plc_dest gefuehrt wird (ROOM2000 sub02) */
static int s_szene = 0, s_cine_was = 0;

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    uint16_t druck = (s_druck_takt > 0 && g_engine.frame_count > 0 && (g_engine.frame_count % (uint32_t)s_druck_takt) == 0)
                   ? (uint16_t)RE15_PAD_BIT_SQUARE : (uint16_t)0;
    s_ctx.pad_current = druck; s_ctx.pad_pressed = druck;
    scd_vm_tick();
    re15_actor_step_all_walkers();   /* Spieler-Plc_dest-Walker wie main.c nach scd_vm_tick (ROOM2000 sub02 dreht Leon) */
    if (s_szene) {
        re15_letterbox_tick(re15_game_flag_get(1, 27));
        int cine = re15_cine_active();
        if (cine) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was) g_scd.letterbox_countdown = 15;
        s_cine_was = cine;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
            g_scd.player_mode = 0;
            re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        }
        if (re15_cam_present_tick()) s_ctx.active_cut = (int)g_scd.cam_id;
    }
    re15_game_step(&s_ctx);
    g_engine.frame_count++;
}

static void raum_start(unsigned room, int elza, uint8_t cut, int32_t px, int32_t py, int32_t pz, int prot)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = room;
    g_gameflow.character = elza ? 4 : 0;
    g_engine.frame_count = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0;
    pl->x = px; pl->y = py; pl->z = pz; pl->rot_y = (int16_t)prot;
    int band = re15_collision_band_from_y(py);
    re15_collision_set_band(band); pl->floor = (uint8_t)band;
    re15_inv_load_briefing();
}

/* Laeuft Raum-Eintritt + n Bilder; protokolliert je NPC-Slot. */
static void raum_lauf(int frames, int slot_a, spur_t *a, int slot_b, spur_t *b)
{
    int la = -1, lb = -1;
    for (int f = 0; f < frames; f++) {
        frame_step();
        if (slot_a > 0 && g_actors[slot_a].active) spur_tick(a, &g_actors[slot_a], &la, f, 1);
        if (slot_b > 0 && b && g_actors[slot_b].active) spur_tick(b, &g_actors[slot_b], &lb, f, 1);
    }
}

static void spur_init(spur_t *s) { memset(s, 0, sizeof *s); s->y_min_sprung = 1 << 30; s->y_max_sprung = -(1 << 30); }

static void spur_bericht(const char *wer, const spur_t *s)
{
    printf("  %s: Kanten-Sub %d Bilder (Kletterphase %d, Clip 2 %d), Sprung-Sub %d Bilder (Clips 3/4/5 = %d/%d/%d, "
           "y %d..%d, Landung %d), Bandwechsel %d, Luft-Bilder %d, Ende (%d,%d,%d) Band %d Sub %d\n",
           wer, s->seen_kante, s->seen_hoch, s->clip_hoch, s->seen_sprung, s->clip_sprung[3], s->clip_sprung[4],
           s->clip_sprung[5], s->y_min_sprung, s->y_max_sprung, s->seen_land, s->band_wechsel, s->luft,
           (int)s->x_end, (int)s->y_end, (int)s->z_end, s->band_end, s->sub_end);
}

/* Mechanik eines Laufs: Fallkurve (Schub 500 + 90, dann 90 + 50 k), Landung auf +0x1ba, Kletterstufe 1800 / 0x47c / Band+1 */
static void mechanik(const char *wer, const spur_t *s, int mit_klettern)
{
    PRUEF(s->fall_bilder > 0 && s->fall_fehler == 0,
          "%s: Fallkurve Bild fuer Bild = Original (Schub-Bild +590 @0x80052d68/@0x80052e34, dann +90+50k @0x80052e34-5c): "
          "%d Bilder, %d Abweichungen", wer, s->fall_bilder, s->fall_fehler);
    PRUEF(s->lande_ok > 0 && s->lande_fehler == 0, "%s: Landung y = +0x1ba (@0x80052efc): %d ok, %d falsch",
          wer, s->lande_ok, s->lande_fehler);
    if (mit_klettern)
        PRUEF(s->stufe_ok > 0 && s->stufe_fehler == 0,
              "%s: Kletterstufe dy -1800 (@0x80052730), Weg 0x47c (@0x80052710), Band +1 (@0x80052768): %d ok, %d falsch",
              wer, s->stufe_ok, s->stufe_fehler);
}

/* 1261: John nach dem Kanaldeckel (Elza). Eintritt ueber ROOM10B1 Tuer Slot 1 @0x016F2 -> Cut 15 bei
 * (-20250,-1800,-6700); sub03 @0x01276 setzt John (-21100,-1800,-8050) Band 1 und startet sub04 @0x012F0. */
static void teil_john1261(void)
{
    printf("== john1261: Elza ROOM1261, Eintritts-Cut 15, Flag (3,0x41) ==\n");
    if (!raum_laden("STAGE1/ROOM1261.RDT")) return;
    raum_start(0x1261, 1, 15, -20250, -1800, -6700, 1024);
    re15_game_flag_set(3, 0x41, 1);
    scd_room_reenter(&s_rdt, -20250, -6700, 15);
    bank_laden(RE15_EMS_ARCHIV_ELZA, 0x49);
    int js = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == 0x49) js = s;
    PRUEF(js > 0, "John (Typ 0x49) gespawnt (sub03 @0x01276), Slot %d", js);
    if (js <= 0) return;
    spur_t a; spur_init(&a);
    raum_lauf(1200, js, &a, -1, NULL);
    spur_bericht("John", &a);
    PRUEF(a.seen_sprung > 0 && a.clip_sprung[3] > 0 && a.clip_sprung[4] > 0 && a.clip_sprung[5] > 0,
          "John springt (Sub 0x18 aus Sub 4, Clips 3/4/5 der Loco-Bank)");
    PRUEF(a.seen_hoch > 0 && a.clip_hoch > 0, "John klettert am Oststeg hinauf (Sub 0x12 Phase 3, Clip 2)");
    PRUEF(a.luft == 0, "John geht/laeuft nie in der Luft (Sub 4/5/6/9 auf Bandhoehe, ausserhalb jeder Absprungzelle: %d)", a.luft);
    PRUEF(a.band_end == 1 && a.y_end == -1800 && a.x_end > 19000 && a.z_end > -6500 && a.z_end < -3500,
          "John endet oben am Oststeg (Band 1, y -1800) nahe dem Ziel (20900,-5000) -> (%d,%d,%d)",
          (int)a.x_end, (int)a.y_end, (int)a.z_end);
    PRUEF(a.sprung10 == 1 && a.hoch01 == 1, "genau ein Sprung (Band 1 -> 0: %d) und ein Hochklettern (0 -> 1: %d)",
          a.sprung10, a.hoch01);
    mechanik("John", &a, 1);
}

/* 1260: Ada (Slot 0, Typ 0x42) und Marvin (Slot 1, Typ 0x40), Leon, Eintritt ueber ROOM11B0 Tuer Slot 5 @0x01034 ->
 * Cut 7 bei (26750,-1800,9400); sub02 @0x01254/@0x01268 + Evt_exec sub04/sub05. */
static void teil_ada1260(void)
{
    printf("== ada1260: Leon ROOM1260, Eintritts-Cut 7, Flag (4,0xF4) ==\n");
    if (!raum_laden("STAGE1/ROOM1260.RDT")) return;
    raum_start(0x1260, 0, 7, 26750, -1800, 9400, 1024);
    re15_game_flag_set(4, 0xF4, 1);
    scd_room_reenter(&s_rdt, 26750, 9400, 7);
    bank_laden(RE15_EMS_ARCHIV_LEON, 0x42);
    bank_laden(RE15_EMS_ARCHIV_LEON, 0x40);
    int sa = -1, sm = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (g_actors[s].active && g_actors[s].type == 0x42) sa = s;
        if (g_actors[s].active && g_actors[s].type == 0x40) sm = s;
    }
    PRUEF(sa > 0 && sm > 0, "Ada (0x42, Slot %d) und Marvin (0x40, Slot %d) gespawnt (sub02 @0x01254/@0x01268)", sa, sm);
    if (sa <= 0 || sm <= 0) return;
    spur_t a, m; spur_init(&a); spur_init(&m);
    raum_lauf(1200, sa, &a, sm, &m);
    spur_bericht("Ada", &a);
    spur_bericht("Marvin", &m);
    PRUEF(a.seen_sprung > 0 && a.clip_sprung[3] > 0 && a.clip_sprung[5] > 0, "Ada springt nordlich vom Steg (Sub 0x18)");
    PRUEF(m.seen_sprung > 0 && m.clip_sprung[3] > 0 && m.clip_sprung[5] > 0, "Marvin springt nordlich vom Steg (Sub 0x18)");
    PRUEF(a.clip_hoch > 0 && m.clip_hoch > 0, "Ada und Marvin klettern auf den Mittelblock (Sub 0x12, Clip 2)");
    PRUEF(a.luft == 0 && m.luft == 0, "keiner geht in der Luft (Ada %d, Marvin %d Abweichungen)", a.luft, m.luft);
    PRUEF(a.band_end == 1 && a.y_end == -1800 && m.band_end == 1 && m.y_end == -1800,
          "beide enden oben auf dem Mittelblock (Band 1): Ada (%d,%d,%d), Marvin (%d,%d,%d)",
          (int)a.x_end, (int)a.y_end, (int)a.z_end, (int)m.x_end, (int)m.y_end, (int)m.z_end);
    mechanik("Ada", &a, 1);
    mechanik("Marvin", &m, 1);
}

/* 10B1: John (Elza) laeuft in sub02 vom Band-5-Steg nach Westen; nach `While(Cmp(0x0A != 2))` @0x01888 zum Ziel
 * (-6694,-7709) @0x01896 ueber die Absprungzelle [31] (Band 5, u1 0x0e). */
static void teil_john10b1(void)
{
    printf("== john10b1: Elza ROOM10B1 sub02, Flags (3,0x80)(3,0x82), Cut 2 ==\n");
    if (!raum_laden("STAGE1/ROOM10B1.RDT")) return;
    raum_start(0x10B1, 1, 2, 0, -9000, -12000, 2048);
    re15_game_flag_set(3, 0x80, 1);
    re15_game_flag_set(3, 0x82, 1);
    scd_room_reenter(&s_rdt, 0, -12000, 2);
    g_scd.cam_id = 2; g_scd.work_vars[0x0A] = 2;   /* Kamera steht auf Cut 2 (Lade-Weg mit camera_cut 2) */
    bank_laden(RE15_EMS_ARCHIV_ELZA, 0x49);
    int js = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == 0x49) js = s;
    PRUEF(js > 0, "John (Typ 0x49) gespawnt, Slot %d", js);
    if (js <= 0) return;
    spur_t a; spur_init(&a);
    raum_lauf(700, js, &a, -1, NULL);
    spur_bericht("John", &a);
    PRUEF(a.sprung51 == 1, "John springt vom Band-5-Steg auf den Block [6] (Band 5 -> 1, 1+3 Baender: %d mal)",
          a.sprung51);
    PRUEF(a.sprung10 >= 1, "John springt vom Block [6] an Zelle [18] auf den Boden (Band 1 -> 0: %d mal)", a.sprung10);
    /* ORIGINAL (DuckStation, M_belege/orig_10b1_john.log, Code RAM == PSX.EXE): danach im Wechsel Sprung auf x -7460 und
     * Klettern auf x -6314 (Ziel @0x01896 liegt 298 vom Westrand; Sonde @0x800516c4 vor dem Ankunftstest @0x80051764),
     * bis Cut 8 sub03 startet. Der Port muss dieselben Lagen treffen. */
    {
        int unten_ok = a.n_sprung_unten > 0, stufe_ok = a.n_stufe > 0;
        for (int i = 0; i < a.n_sprung_unten; i++) if (a.x_sprung_unten[i] < -7465 || a.x_sprung_unten[i] > -7455) unten_ok = 0;
        for (int i = 0; i < a.n_stufe; i++) if (a.x_stufe[i] < -6319 || a.x_stufe[i] > -6309) stufe_ok = 0;
        PRUEF(unten_ok, "jeder Sprung von Block [6] landet bei x -7460 wie am Original (%d Spruenge, erster x %d)",
              a.n_sprung_unten, a.n_sprung_unten ? (int)a.x_sprung_unten[0] : 0);
        PRUEF(stufe_ok, "jedes Hochklettern endet bei x -6314 wie am Original (%d Stufen, erste x %d)",
              a.n_stufe, a.n_stufe ? (int)a.x_stufe[0] : 0);
        PRUEF(a.hoch01 >= 1 && (a.sprung10 == a.hoch01 || a.sprung10 == a.hoch01 + 1),
              "Wechsel Sprung/Klettern wie am Original (Spruenge %d, Kletterstufen %d)", a.sprung10, a.hoch01);
    }
    mechanik("John", &a, 1);
    PRUEF(a.clip_sprung[3] > 0 && a.clip_sprung[4] > 0 && a.clip_sprung[5] > 0, "Sprung mit den Clips 3/4/5");
    PRUEF(a.luft == 0, "John laeuft nie in der Luft (Sub 4/5/6/9 auf Bandhoehe, ausserhalb jeder Absprungzelle: %d)",
          a.luft);
}

/* Gegenprobe: Ada betritt ROOM1050 (Leon) — sub03 laeuft Plc_dest 9 und 5 ohne Kante. */
static void teil_gegen(void)
{
    printf("== gegen: Leon ROOM1050 Ada (Flag (3,0x6e), Cut 6) — Weg ohne Kante ==\n");
    if (!raum_laden("STAGE1/ROOM1050.RDT")) return;
    raum_start(0x1050, 0, 6, 19850, 0, -22300, 3072);
    re15_game_flag_set(3, 0x6e, 1);
    scd_room_reenter(&s_rdt, 19850, -22300, 6);
    bank_laden(RE15_EMS_ARCHIV_LEON, 0x42);
    int sa = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == 0x42) sa = s;
    PRUEF(sa > 0, "Ada gespawnt, Slot %d", sa);
    if (sa <= 0) return;
    spur_t a; spur_init(&a);
    int walk = 0;
    int la = -1;
    for (int f = 0; f < 600; f++) {
        frame_step();
        const re15_actor_t *e = &g_actors[sa];
        if (e->state == 4 && (e->sub_state_1 == 4 || e->sub_state_1 == 5)) walk++;
        spur_tick(&a, e, &la, f, 1);
    }
    spur_bericht("Ada", &a);
    PRUEF(walk > 0, "Ada laeuft (Sub 4/5, %d Bilder)", walk);
    PRUEF(a.seen_kante == 0 && a.seen_sprung == 0, "kein Kanten-Sub ohne Kante (Kante %d / Sprung %d Bilder)",
          a.seen_kante, a.seen_sprung);
    PRUEF(a.luft == 0 && a.band_wechsel == 0, "y bleibt auf dem Boden, kein Bandwechsel");
}

/* 2000 (Leon, Stage 2, "immer"): Eintritt ueber ROOM1260 Tuer Slot 1 @0x0114A -> (4350,-1800,-20500) Cut 0 Band 1.
 * sub00 @0x01794 Ck(4,0xF4) -> Sce_em_set Ada (Slot 0, Typ 0x42) / Marvin (Slot 1, Typ 0x40) + Evt_exec sub02 (Szene mit
 * 12 Message_on, Viereck alle 25 Bilder wie im exe-Lauf); Marvin sub02 @0x01A44 und Ada sub03 @0x01A8A Plc_dest 4 nach
 * (-8350,-26100) queren die Absprungkante (Band 1) und klettern am Gegenufer (Band 0 -> 1). */
static void teil_j2000(void)
{
    printf("== j2000: Leon ROOM2000 (Stage 2), Flag (4,0xF4), Szene sub02/sub03 ==\n");
    if (!raum_laden("STAGE2/ROOM2000.RDT")) return;
    raum_start(0x2000, 0, 0, 4350, -1800, -20500, 1024);
    re15_game_flag_set(4, 0xF4, 1);
    scd_room_reenter(&s_rdt, 4350, -20500, 0);
    bank_laden(RE15_EMS_ARCHIV_LEON, 0x42);
    bank_laden(RE15_EMS_ARCHIV_LEON, 0x40);
    int sa = -1, sm = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (g_actors[s].active && g_actors[s].type == 0x42) sa = s;
        if (g_actors[s].active && g_actors[s].type == 0x40) sm = s;
    }
    PRUEF(sa > 0 && sm > 0, "Ada (0x42, Slot %d) und Marvin (0x40, Slot %d) gespawnt (sub00 @0x017A0/@0x017B4)", sa, sm);
    if (sa <= 0 || sm <= 0) return;
    spur_t a, m; spur_init(&a); spur_init(&m);
    s_druck_takt = 25; s_szene = 1; s_cine_was = 0;
    raum_lauf(2400, sa, &a, sm, &m);
    s_druck_takt = 0; s_szene = 0;
    spur_bericht("Ada", &a);
    spur_bericht("Marvin", &m);
    PRUEF(a.sprung10 == 1 && a.clip_sprung[3] > 0 && a.clip_sprung[4] > 0 && a.clip_sprung[5] > 0,
          "Ada springt in den Kanal (Band 1 -> 0: %d, Clips 3/4/5)", a.sprung10);
    PRUEF(m.sprung10 == 1 && m.clip_sprung[3] > 0 && m.clip_sprung[4] > 0 && m.clip_sprung[5] > 0,
          "Marvin springt in den Kanal (Band 1 -> 0: %d, Clips 3/4/5)", m.sprung10);
    PRUEF(a.hoch01 == 1 && a.clip_hoch > 0 && m.hoch01 == 1 && m.clip_hoch > 0,
          "beide klettern am Gegenufer hinauf (Ada %d, Marvin %d, Clip 2)", a.hoch01, m.hoch01);
    PRUEF(a.luft == 0 && m.luft == 0, "keiner geht in der Luft (Ada %d, Marvin %d)", a.luft, m.luft);
    mechanik("Ada", &a, 1);
    mechanik("Marvin", &m, 1);
}

/* Schwerkraft FUN_8001bd60 in der NPC-Wurzel (0x49: `jal 0x8001bd60` @0x8011dcc4, a0 = -10 @0x8011dcb4, a1 = 0x14
 * @0x8011dcac; alle 17 NPC-Wurzeln gleich): eine NPC, die im Stand mindestens ihren Radius (Box +6 = 450) tief in einer
 * Absprungzelle ihres Bandes steht (FUN_8003b7f0(&+0x34, -r, +0x82) & 2 @0x8001bd94-b4, y == -(Band*1800) @0x8001bdc8-ec),
 * faellt: +0x1ba += 1800 je Band (1 + ((u1>>2)&3)), y += -10 + 20 t (@0x8001be78-a4), Landung y = +0x1ba (@0x8001becc).
 * ROOM1260: Ada nach ihrer Szene (Sub 6, Band 1) mitten in Zelle [28] (Band 1, u1 0x02) gesetzt. Der Schatten liegt
 * waehrend des Falls auf +0x1ba (`lh a1,442` @0x8011dd78). */
static void teil_schwerkraft(void)
{
    printf("== schwerkraft: NPC-Wurzel ruft FUN_8001bd60 (Ada ROOM1260 in Zelle [28]) ==\n");
    if (!raum_laden("STAGE1/ROOM1260.RDT")) return;
    raum_start(0x1260, 0, 7, 26750, -1800, 9400, 1024);
    re15_game_flag_set(4, 0xF4, 1);
    scd_room_reenter(&s_rdt, 26750, 9400, 7);
    bank_laden(RE15_EMS_ARCHIV_LEON, 0x42);
    bank_laden(RE15_EMS_ARCHIV_LEON, 0x40);
    int sa = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == 0x42) sa = s;
    PRUEF(sa > 0, "Ada gespawnt, Slot %d", sa);
    if (sa <= 0) return;
    for (int f = 0; f < 1200; f++) frame_step();            /* Szene zu Ende: Ada steht oben auf dem Mittelblock */
    re15_actor_t *e = &g_actors[sa];
    PRUEF(e->state == 4 && e->sub_state_1 == 6 && e->floor == 1 && e->y == -1800 && e->dog_floor_y == -1800,
          "Ausgang: Ada steht (Sub 6) auf Band 1, y -1800, +0x1ba -1800 (St %d Sub %d Band %d y %d +0x1ba %d)",
          e->state, e->sub_state_1, (int)e->floor, (int)e->y, (int)e->dog_floor_y);
    e->x = 10000; e->z = 16800;                                /* Zelle [28] x -10550..28400 z 15250..18350, Band 1 */
    int32_t soll_y = -1800, n = 0, kurve_ok = 1, schatten_ok = 1, fallbit = 0;
    for (int t = 0; t < 40; t++) {
        frame_step();
        if (!(e->fall_1c0 & 0x8000u) && e->y == 0) break;
        n++;
        soll_y += -10 + 20 * t;
        if (soll_y > 0) soll_y = 0;
        if (e->y != soll_y) { kurve_ok = 0; printf("    Fall-Bild %d: y %d (soll %d)\n", t, (int)e->y, (int)soll_y); }
        if (e->fall_1c0 & 0x8000u) fallbit++;
        int32_t sy = 1;
        if (!(e->fall_1c0 & 0x8000u) || !re15_npckante_schatten_y(e, &sy) || sy != 0) schatten_ok = 0;
        if (e->y == 0) break;
    }
    printf("  Fall: %d Bilder, Ende y %d Band %d +0x1ba %d +0x1c0 %04x\n", n, (int)e->y, (int)e->floor,
           (int)e->dog_floor_y, (unsigned)e->fall_1c0);
    PRUEF(n > 0 && kurve_ok, "Fallkurve y += -10 + 20 t (a0 -10 @0x8011dcb4, a1 0x14 @0x8011dcac) ueber %d Bilder", n);
    PRUEF(e->y == 0 && e->floor == 0 && e->dog_floor_y == 0 && !(e->fall_1c0 & 0x8000u),
          "Landung auf dem Kanalboden: y 0, Band 0, +0x1ba 0, Fallbit geloescht (@0x8001bee4)");
    PRUEF(fallbit > 0 && schatten_ok, "Schatten waehrend des Falls auf +0x1ba = 0 (@0x8011dd78), %d Fall-Bilder", fallbit);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    int alle = strcmp(teil, "alle") == 0;
    if (alle || !strcmp(teil, "clips"))    teil_clips();
    if (alle || !strcmp(teil, "sonde"))    teil_sonde();
    if (alle || !strcmp(teil, "john1261")) teil_john1261();
    if (alle || !strcmp(teil, "ada1260"))  teil_ada1260();
    if (alle || !strcmp(teil, "john10b1")) teil_john10b1();
    if (alle || !strcmp(teil, "gegen"))    teil_gegen();
    if (alle || !strcmp(teil, "j2000"))    teil_j2000();
    if (alle || !strcmp(teil, "schwerkraft")) teil_schwerkraft();
    if (g_fail) { printf("FAIL: %d Pruefung(en)\n", g_fail); return 1; }
    printf("OK\n");
    return 0;
}
