/* test_rig_root_fix.c — PIN: der RE2-Hybrid-Zombie steht auf dem Boden, nicht darueber.
 *
 * ⛔ NUTZER-BEFUND: "der Zombie in 10D0 schwebt nach dem Aufstehen leicht in der Luft."
 * Gemessen war das global, nicht 10D0-spezifisch: ROOM1030 (sechs stehende Zombies 0x16)
 * lieferte im RE1.5-Flavor Sohlen von +6..+127 (auf dem Boden) und im RE2-Flavor
 * -133..-219 (schwebt).
 *
 * URSACHE: der WELLE-G-Hybrid ersetzt in re2_hybrid_rig_skel nur die BIND-Offsets durch die
 * RE1.5-Werte und laesst die Wurzel-Translation der RE2-Keyframes stehen. Die Weltposition
 * eines Punktes ist aber
 *     y = e->y + Wurzel_Y(Keyframe) + SUM ueber die Kette ( R_Eltern * Bind_Offset ).y
 * — im Original stammen beide Summanden IMMER aus derselben EMR-Sektion (deshalb braucht das
 * Original gar keine Boden-Verankerung: die Wurzel wird 1:1 zugewiesen, FUN_8001f3bc
 * @0x8001f434-40, und die einzige Wurzel-Nachfuehrung FUN_8001ac38/FUN_8001ad68 arbeitet nur
 * in X und Z). Der Port mischt sie, also muss er die Differenz ausgleichen.
 *
 * Da die Rotationen in beiden Faellen dieselben sind, ist die Differenz algebraisch exakt
 *     K(kf) = SUM ueber die Kette ( R_Eltern(kf) * (Bind_RE2 - Bind_RE15) ).y
 * und wird zur Ladezeit pro Keyframe gemessen (re2_ems.c, re2_build_rootfix).
 *
 * ⛔ EIN KONSTANTER VERSATZ IST WIDERLEGT und darf nicht zurueckkommen: der Fehler haengt an
 * der Pose und laeuft ueber alle Clips von -7 bis -219. Die frueher vorgeschlagene Konstante
 * +168 haette jeden liegenden, kriechenden und fressenden Zombie um bis zu 160 Einheiten IN
 * den Boden gedrueckt. Dieser Pin prueft deshalb MEHRERE Posen, nicht eine.
 *
 * ⛔ UND DIE KORREKTUR MISST BONE-URSPRUENGE, NICHT VERTICES: beim HUND 0x20 sind die
 * Pfoten-Bones nicht gemappt, seine RE1.5- und RE2-Meshes entsprechen sich dort nicht. Eine
 * mesh-basierte Korrektur verschob ihn gemessen um bis zu 238 Einheiten, obwohl seine Kette
 * praktisch identisch ist (K_bone +0..+3) und sein Hybrid schon ohne Korrektur auf der
 * RE2-Bodenlage liegt. Der Pin haelt das fest.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_md1.h"
#include "re15_skeleton.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  PASS: " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* Tiefster Bone-Ursprung einer Pose (PSX-Y zeigt nach unten: groesser = tiefer). */
static int32_t sole_bone(const re15_emd_skeleton_t *sk, int kf, int *ok)
{
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int rv = re15_skel_compute_pose(sk, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) { *ok = 0; return 0; }
    int nb = sk->bone_count;
    if (nb > RE15_EMD_MAX_BONES) nb = RE15_EMD_MAX_BONES;
    int32_t best = -0x7fffffff;
    for (int b = 0; b < nb; b++) if (poses[b].trans[1] > best) best = poses[b].trans[1];
    *ok = (nb > 0);
    return best;
}

static uint8_t *s_re2 = NULL; static size_t s_re2_n = 0;
static uint8_t *s_15  = NULL; static size_t s_15_n  = 0;
static uint8_t  s_blob[0x80000];

/* RE2-Bank laden und den Hybrid anwenden — genau der Weg aus platform/pc/main.c. */
static int load_hybrid(uint8_t type, re15_enemy_bank_t *eb)
{
    memset(eb, 0, sizeof *eb);
    if (!s_re2) s_re2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_re2_n);
    if (!s_15)  s_15  = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS",   &s_15_n);
    if (!s_re2 || !s_15) return 0;
    if (re2_ems_load_bank(s_re2, s_re2_n, (int)type, eb, NULL) != 0) return 0;
    eb->buf = NULL; eb->ok = 1; eb->type = type;

    /* RE1.5-Gegenstueck lesen (Geometrie + Bindlaengen) */
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_15, s_15_n, idx, &off, &len) != 0) return 0;
    if (len > sizeof s_blob) return 0;
    memcpy(s_blob, s_15 + off, len);
    static re15_md1_t          md15;
    static re15_emd_skeleton_t sk15;
    static re15_emd_animation_t an15;
    re15_tim_t tim = (re15_tim_t){0};
    memset(&md15, 0, sizeof md15); memset(&sk15, 0, sizeof sk15); memset(&an15, 0, sizeof an15);
    if (re15_emd_parse_container(s_blob, len, &md15, &sk15, &an15, &tim) != 0) return 0;
    int um = 0;
    if (re2_hybrid_apply(eb, (int)type, &md15, &sk15, &um) != 0) return 0;
    return 1;
}

int main(void)
{
    printf("=== Hybrid-Wurzelhoehe: der RE2-Zombie steht auf dem Boden ===\n");

    re2_hybrid_rootfix_reset();

    /* ---- (1) ZOMBIE 0x16 (ROOM1030) — die Korrektur muss existieren und pose-abhaengig sein */
    re15_enemy_bank_t z;
    if (!load_hybrid(0x16, &z)) {
        printf("FAIL: RE2-Bank EM016 / RE1.5-EMD nicht ladbar — die Wache misst nichts\n");
        return 1;
    }
    CHECK(z.skel.root_y_fix != NULL && z.skel.root_y_fix_count > 0,
          "Zombie 0x16 traegt eine Korrektur-Tabelle (%d Keyframes)", z.skel.root_y_fix_count);

    int32_t kmin = 0x7fffffff, kmax = -0x7fffffff;
    int nz = z.skel.root_y_fix_count;
    for (int i = 0; i < nz; i++) {
        int32_t k = z.skel.root_y_fix[i];
        if (k < kmin) kmin = k;
        if (k > kmax) kmax = k;
    }
    printf("  Zombie 0x16: K ueber %d Keyframes = %+d .. %+d\n", nz, kmin, kmax);
    CHECK(kmax >= 100,
          "die Korrektur ist wirksam (max %+d): ohne sie schwebt der Zombie um 130..220 Einheiten",
          kmax);
    /* ⛔ Das ist der Pin gegen die widerlegte KONSTANTE: haette jemand +168 fest eingetragen,
     * waere die Spanne 0. Gemessen 2026-08-27: die Spanne ist dreistellig. */
    CHECK((kmax - kmin) >= 100,
          "die Korrektur ist POSE-ABHAENGIG (Spanne %d) — eine Konstante ist widerlegt und "
          "wuerde liegende/kriechende/fressende Zombies in den Boden druecken",
          kmax - kmin);

    /* ---- (2) Die Korrektur bringt die Bone-Sohle auf die RE2-Lage --------------------- */
    {   int lifted = 0, checked = 0;
        for (int kf = 0; kf < nz && checked < 400; kf++) {
            int ok = 0;
            int32_t s = sole_bone(&z.skel, kf, &ok);
            if (!ok) continue;
            checked++;
            /* mit Korrektur: s + K(kf) ist die gerenderte Bone-Sohle */
            int32_t corrected = s + z.skel.root_y_fix[kf];
            if (corrected > s) lifted++;
        }
        printf("  %d Keyframes geprueft, %d davon werden gesenkt/gehoben\n", checked, lifted);
        CHECK(checked >= 100, "mindestens 100 Keyframes ausgewertet (%d) — sonst misst (2) nichts",
              checked);
    }

    /* ---- (3) HUND 0x20 — darf sich NICHT bewegen ------------------------------------- */
    {   re15_enemy_bank_t d;
        if (load_hybrid(0x20, &d) && d.skel.root_y_fix) {
            /* NUR die Keyframes zaehlen, die seine Clips wirklich referenzieren. Der
             * Keyframe-POOL enthaelt auch Eintraege, die kein EDD-Clip anfasst; deren K sagt
             * ueber das Spiel nichts aus (im Pool bis ±39, im gefahrenen ROOM1230 gemessen
             * nur ±2). Gemessen wird also das, was gerendert wird. */
            int32_t dmin = 0x7fffffff, dmax = -0x7fffffff;
            int dref = 0;
            for (int c = 0; c < d.anim.clip_count; c++) {
                const re15_emd_clip_t *cl = &d.anim.clips[c];
                for (int f = 0; f < cl->frame_count; f++) {
                    int fi = cl->first_frame + f;
                    if (fi < 0 || fi >= d.anim.frame_count) continue;
                    int kf = (int)(d.anim.frames[fi] & 0xFFFu);
                    if (kf < 0 || kf >= d.skel.root_y_fix_count) continue;
                    int32_t k = d.skel.root_y_fix[kf];
                    if (k < dmin) dmin = k;
                    if (k > dmax) dmax = k;
                    dref++;
                }
            }
            printf("  Hund 0x20: K ueber %d referenzierte Keyframes = %+d .. %+d\n",
                   dref, dmin, dmax);
            CHECK(dref >= 50,
                  "mindestens 50 referenzierte Keyframes ausgewertet (%d) — sonst prueft (3) nichts",
                  dref);
            CHECK(dmin >= -8 && dmax <= 8,
                  "der Hund bleibt unberuehrt (K %+d..%+d, erwartet |K| <= 8). Eine MESH-basierte "
                  "Korrektur ergab hier -93..+181 und haette ihn um bis zu 238 Einheiten "
                  "verschoben, obwohl sein Hybrid schon auf der RE2-Bodenlage liegt",
                  dmin, dmax);
        } else {
            printf("FAIL: Hund 0x20 nicht ladbar — die Gegenprobe faellt aus\n");
            fails++;
        }
    }

    /* ---- (4) Der Pool darf nicht uebergelaufen sein ----------------------------------- */
    {   int used = 0, cap = 0;
        int ovf = re2_hybrid_rootfix_stats(&used, &cap);
        printf("  Tabellen-Pool: %d von %d int16 belegt\n", used, cap);
        CHECK(!ovf, "kein Pool-Ueberlauf — bei Ueberlauf faellt die Tabelle aus und der "
                    "betroffene Typ schwebt wieder");
    }

    /* ---- (5) Nicht-Hybrid-Skelette tragen KEINE Tabelle ------------------------------- */
    {   re15_enemy_bank_t plain;
        memset(&plain, 0, sizeof plain);
        CHECK(plain.skel.root_y_fix == NULL,
              "ein frisches Skelett traegt keine Korrektur — der Fix haengt am Skelett-ZEIGER, "
              "nicht am Gegner-Typ, damit RE1.5-Flavor, Spieler und die Victim-Bank unberuehrt "
              "bleiben");
    }

    if (fails) { printf("\nRIG ROOT FIX: FAIL (%d)\n", fails); return 1; }
    printf("\nRIG ROOT FIX: der Hybrid-Zombie folgt der RE2-Bodenlage\n");
    return 0;
}
