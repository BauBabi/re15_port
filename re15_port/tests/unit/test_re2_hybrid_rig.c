/* =============================================================================
 * test_re2_hybrid_rig — WELLE G: "RE2 AI" mit RE1.5-MODELLEN (re2_hybrid_apply).
 *
 * Gepinnt wird gegen die ECHTEN Archive (shared_assets/PSX/EMD/CDEMD0.EMS und
 * shared_assets/RE2/CDEMD0.EMS), nicht gegen abgeschriebene Zahlen:
 *   (1) Die Rig-Messung selbst: Bone-Zahlen, kf-Groessen und die parent[]-Ketten
 *       beider Spiele; die Permutationstabellen muessen JEDE Kante des RE2-Rigs
 *       auf eine existierende RE1.5-Kante abbilden (unmapped == 0).
 *   (2) Der Hybrid-Umbau an einer echten Bank: RE1.5-Geometrie, RE2-Animation,
 *       RE2-Hierarchie, RE1.5-Bind-Laengen in RE2-Slot-Ordnung.
 *   (3) REGRESSIONSWACHEN: die beiden bestehenden Modi bleiben unveraendert
 *       (Default-Flags, Orthogonalitaet, remap_ok==0 ohne Hybrid-Aufruf,
 *       kein Hybrid fuer Typen ausserhalb der Tabelle).
 *   (4) Die beiden dokumentierten GRENZEN als Messung, nicht als Behauptung:
 *       kein Stumpf-Mesh im RE1.5-MD1, und der Schrittlaengen-/Beinlaengen-
 *       Versatz der RE2-Wurzelbewegung.
 * ===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re2_ems.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"

#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif
#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int g_fail = 0;
#define CHECK(cond, msg, ...) do { if (!(cond)) { \
    fprintf(stderr, "FAIL: " msg "\n", ##__VA_ARGS__); g_fail = 1; } } while (0)

static uint8_t *slurp(const char *path, size_t *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return NULL; }
    fclose(f);
    if (out_sz) *out_sz = (size_t)sz;
    return b;
}

/* --- die RE1.5-Seite: Blob aus CDEMD0.EMS + Container-Parse (wie pc_enemy_load) --- */
static uint8_t     *s_re15_ems; static size_t s_re15_ems_sz;
static re15_md1_t           s_md15;
static re15_emd_skeleton_t  s_sk15;
static re15_emd_animation_t s_an15;

static int load_re15(uint8_t type)
{
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0) return 0;
    if (re15_ems_get_entry(s_re15_ems, s_re15_ems_sz, idx, &off, &len) != 0) return 0;
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    return re15_emd_parse_container(s_re15_ems + off, len, &s_md15, &s_sk15, &s_an15, &tim) == 0;
}

static uint8_t *s_re2_ems; static size_t s_re2_ems_sz;
static re15_enemy_bank_t s_bank;      /* gross (4x frames[1664]) — nie Stack-Local */

static int load_re2(uint8_t type)
{
    memset(&s_bank, 0, sizeof s_bank);
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    return re2_ems_load_bank(s_re2_ems, s_re2_ems_sz, type, &s_bank, &tim) == 0;
}

/* ===== (1) Rig-Messung + Tabellen-Konsistenz ============================ */

static void pin_rig(uint8_t type, int want_bones2, int want_kf2,
                    int want_bones15, int want_kf15)
{
    if (!load_re2(type))  { CHECK(0, "EM0%02X: RE2-Bank laedt nicht", type);  return; }
    if (!load_re15(type)) { CHECK(0, "EM%02X: RE1.5-EMD laedt nicht", type);  return; }

    CHECK(s_bank.skel.bone_count == want_bones2 &&
          s_bank.skel.keyframe_size_bytes == want_kf2,
          "RE2 EM0%02X: %d Bones / kf %d, erwartet %d / %d", type,
          s_bank.skel.bone_count, s_bank.skel.keyframe_size_bytes, want_bones2, want_kf2);
    CHECK(s_sk15.bone_count == want_bones15 && s_sk15.keyframe_size_bytes == want_kf15,
          "RE1.5 EM%02X: %d Bones / kf %d, erwartet %d / %d", type,
          s_sk15.bone_count, s_sk15.keyframe_size_bytes, want_bones15, want_kf15);

    const int8_t *perm = NULL;
    int n = re2_hybrid_perm((int)type, &perm);
    CHECK(n == want_bones2 && perm != NULL,
          "EM%02X: Permutation %d Slots, erwartet %d", type, n, want_bones2);
    if (n != want_bones2 || !perm) return;

    /* Jeder Zielindex muss im RE1.5-Rig existieren, und die Abbildung muss INJEKTIV sein
     * (zwei RE2-Slots duerfen nie dasselbe RE1.5-Mesh beanspruchen). */
    int seen[RE15_EMD_MAX_BONES]; memset(seen, 0, sizeof seen);
    for (int i = 0; i < n; i++) {
        int m = perm[i];
        if (m < 0) continue;
        CHECK(m < s_sk15.bone_count && m < s_md15.mesh_count,
              "EM%02X perm[%d]=%d ausserhalb RE1.5 (%d Bones / %d Meshes)",
              type, i, m, s_sk15.bone_count, s_md15.mesh_count);
        if (m >= 0 && m < RE15_EMD_MAX_BONES) {
            CHECK(!seen[m], "EM%02X: RE1.5-Slot %d doppelt belegt (perm[%d])", type, m, i);
            seen[m] = 1;
        }
    }

    /* JEDE RE2-Kante muss eine RE1.5-Entsprechung haben (gleich oder umgekehrt). */
    int unmapped = 0;
    re15_md1_t md_copy = s_bank.md1;                       /* Ausgangs-MD1 sichern */
    int rc = re2_hybrid_apply(&s_bank, (int)type, &s_md15, &s_sk15, &unmapped);
    CHECK(rc == 0, "EM%02X: re2_hybrid_apply rc=%d", type, rc);
    CHECK(unmapped == 0, "EM%02X: %d RE2-Kanten ohne RE1.5-Entsprechung (Soll 0)",
          type, unmapped);
    CHECK(md_copy.mesh_count != s_bank.md1.mesh_count ||
          md_copy.raw != s_bank.md1.raw, "EM%02X: MD1 wurde nicht getauscht", type);
    CHECK(s_bank.remap_ok == 1, "EM%02X: remap_ok nicht gesetzt", type);
    for (int i = 0; i < n; i++)
        CHECK(s_bank.mesh_remap[i] == perm[i],
              "EM%02X mesh_remap[%d]=%d != perm %d", type, i, s_bank.mesh_remap[i], perm[i]);
    for (int i = n; i < RE15_EMD_MAX_BONES; i++)
        CHECK(s_bank.mesh_remap[i] == -1, "EM%02X mesh_remap[%d] nicht -1", type, i);
}

int main(void)
{
    printf("=== WELLE G: RE2-Gehirn + RE1.5-Modelle (Hybrid-Rig) ===\n");

    s_re15_ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_re15_ems_sz);
    s_re2_ems  = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS",     &s_re2_ems_sz);
    if (!s_re15_ems || !s_re2_ems) {
        fprintf(stderr, "FAIL: CDEMD0.EMS (RE1.5 und/oder RE2) nicht lesbar\n");
        return 1;
    }

    /* ---- (1) Rig-Messung je Hybrid-Typ ---------------------------------- */
    pin_rig(0x10, 15, 80, 15, 80);      /* Zombie                                   */
    pin_rig(0x11, 15, 80, 15, 80);
    pin_rig(0x12, 15, 80, 15, 80);
    pin_rig(0x13, 15, 80, 15, 80);      /* Zombie Girl (eigene Bone-Ordnung)         */
    pin_rig(0x16, 15, 80, 15, 80);
    pin_rig(0x18, 15, 80, 15, 80);
    pin_rig(0x20, 17, 92, 15, 80);      /* HUND: RE2 hat zwei Bones mehr             */
    pin_rig(0x21, 13, 72, 13, 72);      /* Kraehe                                    */
    pin_rig(0x25, 20, 104, 20, 104);    /* Adult Spider                              */
    /* BABY SPIDER: die STRUKTUR (dir[2]) ist in beiden Spielen 1 Bone / kf 20. Die kf-GROESSE
     * der Hauptbank ist im RE2-Fall 80, weil re2_emd_parse_bank die Bank mit den MEISTEN
     * EDD-Clips waehlt und das bei EM026 als EINZIGEM Typ Paar 3 ist (Clips p1=1, p2=0, p3=3;
     * Paar-3-Pool-Header {0,8,15,80} = die 15-Bone-Spielerbank). GEMESSEN, hier nur
     * festgehalten — der Hybrid braucht davon nur bone_count, und der Spinnen-Pfad gehoert
     * gerade einem anderen Arbeitsstrang. */
    pin_rig(0x26,  1, 80,  1, 20);      /* Baby Spider                               */
    if (!g_fail) printf("  (1) 10 Hybrid-Typen: Bone-Zahlen, Injektivitaet, 0 offene Kanten\n");

    /* ---- (2) Der Umbau an EM10 im Detail -------------------------------- */
    if (load_re2(0x10) && load_re15(0x10)) {
        /* RE2-parent[] VOR dem Umbau festhalten. */
        int8_t par2[RE15_EMD_MAX_BONES];
        memcpy(par2, s_bank.skel.bone_parent, sizeof par2);
        static const int8_t k_par2[15] = { -1,0,1,2,3,1,5,6,0,0,9,10,0,12,13 };
        static const int8_t k_par15[15]= { -1,0,1,2,0,4,5,0,7,8,9,7,11,12,7 };
        for (int i = 0; i < 15; i++) {
            CHECK(par2[i] == k_par2[i], "RE2 EM010 parent[%d]=%d != %d", i, par2[i], k_par2[i]);
            CHECK(s_sk15.bone_parent[i] == k_par15[i],
                  "RE1.5 EM10 parent[%d]=%d != %d", i, s_sk15.bone_parent[i], k_par15[i]);
        }
        /* Die Um-Wurzelungs-Kante Huefte<->Brust ist in BEIDEN Rigs (0,0,0) — genau deshalb
         * ist das Um-Wurzeln geometrisch kostenlos. */
        CHECK(s_sk15.bone_relative_pos[7][0] == 0 && s_sk15.bone_relative_pos[7][1] == 0 &&
              s_sk15.bone_relative_pos[7][2] == 0,
              "RE1.5 EM10 Bone 7 (Brust an Huefte) != (0,0,0)");
        CHECK(s_bank.skel.bone_relative_pos[1][0] == 0 &&
              s_bank.skel.bone_relative_pos[1][1] == 0 &&
              s_bank.skel.bone_relative_pos[1][2] == 0,
              "RE2 EM010 Bone 1 (Huefte an Brust) != (0,0,0)");

        /* Geometrie-Anker der Zuordnung (Tri-Vertices, Mesh-Index == Bone-Index). */
        CHECK(s_md15.mesh_count == 15, "RE1.5 EM10 MD1 = %d Meshes != 15", s_md15.mesh_count);
        CHECK(s_bank.md1.mesh_count == 17, "RE2 EM010 MD1 = %d Meshes != 17",
              s_bank.md1.mesh_count);
        CHECK(s_md15.meshes[7].tri_vertex_count  == 29 &&   /* Brust  <- RE2 Slot 0  */
              s_md15.meshes[0].tri_vertex_count  == 18 &&   /* Huefte <- RE2 Slot 1  */
              s_md15.meshes[14].tri_vertex_count == 42 &&   /* Kopf   <- RE2 Slot 8  */
              s_md15.meshes[3].tri_vertex_count  == 16,     /* Fuss   <- RE2 Slot 4  */
              "RE1.5-EM10-Geometrie-Anker verschoben (7/0/14/3 = %d/%d/%d/%d)",
              s_md15.meshes[7].tri_vertex_count, s_md15.meshes[0].tri_vertex_count,
              s_md15.meshes[14].tri_vertex_count, s_md15.meshes[3].tri_vertex_count);

        int clips_main = s_bank.anim.clip_count;
        int clips_loco = s_bank.anim_loco.clip_count;
        int loco0      = s_bank.anim_loco.clips[0].frame_count;
        int unmapped = -1;
        CHECK(re2_hybrid_apply(&s_bank, 0x10, &s_md15, &s_sk15, &unmapped) == 0,
              "EM10-Hybrid schlug fehl");

        /* GEOMETRIE ist jetzt RE1.5 ... */
        CHECK(s_bank.md1.mesh_count == 15, "Hybrid-MD1 = %d Meshes != 15 (RE1.5)",
              s_bank.md1.mesh_count);
        CHECK(s_bank.md1.meshes[7].tri_vertex_count == 29 &&
              s_bank.md1.meshes[14].tri_vertex_count == 42,
              "Hybrid-MD1 traegt nicht die RE1.5-Meshes");
        /* ... ANIMATION + HIERARCHIE bleiben RE2 ... */
        CHECK(s_bank.anim.clip_count == clips_main && clips_main == 31,
              "Hybrid: Haupt-Clips %d (vorher %d) != 31 — Animation muss RE2 bleiben",
              s_bank.anim.clip_count, clips_main);
        CHECK(s_bank.anim_loco.clip_count == clips_loco && clips_loco == 8 && loco0 == 65,
              "Hybrid: Loco-Bank %d Clips / clip0 %d != 8 / 65", clips_loco, loco0);
        CHECK(s_bank.skel.bone_count == 15, "Hybrid: Bone-Zahl != 15");
        for (int i = 0; i < 15; i++)
            CHECK(s_bank.skel.bone_parent[i] == k_par2[i],
                  "Hybrid: parent[%d] wurde veraendert (%d != %d)",
                  i, s_bank.skel.bone_parent[i], k_par2[i]);
        /* ... und die BIND-LAENGEN sind die RE1.5-Werte, in RE2-Slot-Ordnung. */
        static const int8_t k_perm[15] = { 7,0,1,2,3,4,5,6,14,8,9,10,11,12,13 };
        for (int i = 1; i < 15; i++) {                    /* Slot 0 = Wurzel, ungenutzt */
            int p = k_par2[i], mi = k_perm[i], mp = k_perm[p];
            int16_t wx, wy, wz;
            if (s_sk15.bone_parent[mi] == mp) {
                wx = s_sk15.bone_relative_pos[mi][0];
                wy = s_sk15.bone_relative_pos[mi][1];
                wz = s_sk15.bone_relative_pos[mi][2];
            } else {
                wx = (int16_t)-s_sk15.bone_relative_pos[mp][0];
                wy = (int16_t)-s_sk15.bone_relative_pos[mp][1];
                wz = (int16_t)-s_sk15.bone_relative_pos[mp][2];
            }
            CHECK(s_bank.skel.bone_relative_pos[i][0] == wx &&
                  s_bank.skel.bone_relative_pos[i][1] == wy &&
                  s_bank.skel.bone_relative_pos[i][2] == wz,
                  "Hybrid EM10 Bind-Pos Slot %d = (%d,%d,%d), erwartet (%d,%d,%d)", i,
                  s_bank.skel.bone_relative_pos[i][0], s_bank.skel.bone_relative_pos[i][1],
                  s_bank.skel.bone_relative_pos[i][2], wx, wy, wz);
        }
        /* Konkrete Anker (aus dem Archiv gelesen, nicht abgeschrieben): */
        CHECK(s_bank.skel.bone_relative_pos[2][1] == 228 &&
              s_bank.skel.bone_relative_pos[2][2] == -160,
              "Hybrid Slot 2 (R-Oberschenkel) != RE1.5 (0,228,-160)");
        CHECK(s_bank.skel.bone_relative_pos[8][0] == -50 &&
              s_bank.skel.bone_relative_pos[8][1] == -667,
              "Hybrid Slot 8 (Kopf) != RE1.5 (-50,-667,0)");
        /* Leons Victim-Bank bleibt UNVERAENDERT (PL00-Rig == RE2-Rig). */
        CHECK(s_bank.victim_ok && s_bank.anim_victim.clip_count == 17,
              "Hybrid: Victim-Bank (Leon) veraendert");
        if (!g_fail) printf("  (2) EM10-Hybrid: RE1.5-Geometrie + RE2-Rig/Clips, Bind-Pos permutiert\n");
    }

    /* ---- (2b) HUND: die zwei RE2-Slots ohne RE1.5-Gegenstueck ------------ */
    if (load_re2(0x20) && load_re15(0x20)) {
        int unmapped = -1;
        CHECK(re2_hybrid_apply(&s_bank, 0x20, &s_md15, &s_sk15, &unmapped) == 0,
              "EM20-Hybrid schlug fehl");
        CHECK(unmapped == 0, "EM20: %d offene Kanten", unmapped);
        CHECK(s_bank.mesh_remap[7] == -1 && s_bank.mesh_remap[10] == -1,
              "EM20: Slots 7/10 (drittes Vorderbein-Segment) muessen -1 sein (%d/%d)",
              s_bank.mesh_remap[7], s_bank.mesh_remap[10]);
        CHECK(s_bank.mesh_remap[16] == 14,
              "EM20: letzter Slot 16 muss auf RE1.5-Mesh 14 zeigen (%d) — die alte "
              "min(bones,meshes)-Klammer haette ihn verschluckt", s_bank.mesh_remap[16]);
        CHECK(s_bank.md1.mesh_count == 15, "EM20-Hybrid-MD1 = %d != 15", s_bank.md1.mesh_count);
        if (!g_fail) printf("  (2b) EM20-Hund: 15 RE1.5-Meshes auf 17 RE2-Slots, 7/10 leer\n");
    }

    /* ---- (3) REGRESSIONSWACHEN ------------------------------------------ */
    {
        /* Default: RE1.5-Gehirn. ⛔ 2026-08-21: die dritte Menue-Stufe ("AI RE2 MODELS") ist
         * auf Nutzer-Entscheidung entfernt — der PC-Loader faehrt unter dem RE2-Flavor IMMER
         * den Hybrid (platform/pc/main.c pc_enemy_load) und liest re15_ai_models() nicht mehr.
         * Das Flag bleibt als TEST-HAKEN fuer genau diesen Rig-Umbau; sein Default (RE2 =
         * "unumgebaute Bank") ist der Ausgangszustand, den die Faelle oben voraussetzen. */
        /* ⛔ 2026-08-22: der AUSLIEFERUNGS-Default ist jetzt RE2 (Nutzer-Entscheidung). Im
         * TESTKONTEXT gilt weiterhin RE1.5 — der Konstruktor in tests/test_support.c stellt das
         * einmal fuer alle Tests her, statt den Setter in Dutzende Dateien zu streuen. Dieser
         * PIN prueft genau diesen Testkontext (und darunter die Orthogonalitaet der Setter). */
        CHECK(re15_ai_flavor() == RE15_AI_FLAVOR_RE15, "Testkontext-Flavor != RE1.5");
        CHECK(re15_ai_models() == RE15_AI_MODELS_RE2,
              "Default-Modellherkunft != RE2 (Test-Haken-Ausgangszustand)");
        /* Orthogonalitaet: keins der beiden Flags zieht das andere mit (der Rig-Test setzt sie
         * einzeln; ein stilles Mitziehen wuerde die Faelle oben verfaelschen). */
        re15_ai_models_set(RE15_AI_MODELS_RE15);
        CHECK(re15_ai_flavor() == RE15_AI_FLAVOR_RE15, "models_set hat den Flavor veraendert");
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        CHECK(re15_ai_models() == RE15_AI_MODELS_RE15, "flavor_set hat die Modelle veraendert");
        re15_ai_models_set(RE15_AI_MODELS_RE2);
        CHECK(re15_ai_flavor() == RE15_AI_FLAVOR_RE2, "models_set hat den Flavor veraendert");
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);

        /* Eine NICHT umgebaute Bank traegt remap_ok == 0 -> der Renderer laeuft Byte fuer
         * Byte den alten Pfad (mesh_idx == bone_idx). */
        CHECK(load_re2(0x10), "EM010 laedt nicht");
        CHECK(s_bank.remap_ok == 0, "frische RE2-Bank hat remap_ok != 0");
        CHECK(s_bank.md1.mesh_count == 17, "frische RE2-Bank ist nicht das RE2-MD1");

        /* Typen ausserhalb der Tabelle bekommen NIE einen Hybrid (Gorilla 0x27, Birkin 0x30,
         * NPC 0x47) — deren byte-true RE1.5-Kampagnen bleiben unberuehrt. */
        const int8_t *p = (const int8_t *)1;
        CHECK(re2_hybrid_perm(0x27, &p) < 0 && p == NULL, "0x27 darf keinen Hybrid haben");
        CHECK(re2_hybrid_perm(0x30, NULL) < 0, "0x30 darf keinen Hybrid haben");
        CHECK(re2_hybrid_perm(0x47, NULL) < 0, "0x47 darf keinen Hybrid haben");
        CHECK(re2_hybrid_apply(&s_bank, 0x27, &s_md15, &s_sk15, NULL) < 0,
              "re2_hybrid_apply(0x27) haette scheitern muessen");
        CHECK(s_bank.remap_ok == 0, "gescheiterter Hybrid hat die Bank angefasst");
        if (!g_fail) printf("  (3) Regressionswachen: Defaults, Orthogonalitaet, remap_ok, Typ-Gate\n");
    }

    /* ---- (4) Die beiden dokumentierten GRENZEN als Messung --------------- */
    {
        /* (4a) GORE-STUMPF: der Zerleger stempelt part_mesh = RESERVE-Mesh 15
         * (@0x8010531C-50). Im RE1.5-MD1 gibt es das nicht — Beleg: mesh_count == bone_count
         * fuer JEDEN Zombie-Typ, also kein ungenutztes Reserve-Mesh. */
        static const uint8_t zomb[6] = { 0x10, 0x11, 0x12, 0x13, 0x16, 0x18 };
        for (int i = 0; i < 6; i++) {
            if (!load_re15(zomb[i])) { CHECK(0, "EM%02X laedt nicht", zomb[i]); continue; }
            CHECK(s_md15.mesh_count == s_sk15.bone_count && s_md15.mesh_count == 15,
                  "RE1.5 EM%02X: %d Meshes / %d Bones — Reserve-Mesh vorhanden?",
                  zomb[i], s_md15.mesh_count, s_sk15.bone_count);
            if (!load_re2(zomb[i])) continue;
            CHECK(s_bank.md1.mesh_count == 17,
                  "RE2 EM0%02X: %d Meshes != 17 (15 Bones + Stumpf 15 + Rumpf-Variante 16)",
                  zomb[i], s_bank.md1.mesh_count);
        }
        printf("  (4a) GRENZE Gore: RE1.5-Zombie-MD1 hat 15 Meshes == 15 Bones, KEIN Reserve-\n"
               "       Mesh -> im Hybrid bleibt der Oberschenkel intakt (kein Stumpf).\n");

        /* (4c) GRENZE Baby-Spinne: das RE1.5-Modell EM26 ist ein EINZIGES Dreieck (3 Vertices,
         * 1 Face) — ein Platzhalter. Das ist KEINE Hybrid-Regression: der byte-true RE1.5-Modus
         * zeichnet exakt dasselbe (pc_enemy_load liest denselben CDEMD0.EMS-Blob, Index 11). */
        if (load_re15(0x26) && load_re2(0x26)) {
            CHECK(s_md15.mesh_count == 1 && s_md15.meshes[0].tri_vertex_count == 3 &&
                  s_md15.meshes[0].triangle_count == 1,
                  "RE1.5 EM26: %d Meshes / %d Vertices / %d Faces (erwartet 1/3/1)",
                  s_md15.mesh_count, s_md15.meshes[0].tri_vertex_count,
                  s_md15.meshes[0].triangle_count);
            CHECK(s_bank.md1.meshes[0].tri_vertex_count == 18 &&
                  s_bank.md1.meshes[0].triangle_count == 12,
                  "RE2 EM026: %d Vertices / %d Faces (erwartet 18/12)",
                  s_bank.md1.meshes[0].tri_vertex_count, s_bank.md1.meshes[0].triangle_count);
            printf("  (4c) GRENZE Baby-Spinne: RE1.5-EM26 = 1 Dreieck (3 v) gegen RE2 12 Faces\n"
                   "       (18 v) -> im Hybrid praktisch unsichtbar, GENAU wie im RE1.5-Modus.\n");
        }

        /* (4b) WURZEL-TRANSLATION: die RE2-Clips bewegen den Aktor ueber das Speed-Feld der
         * Keyframes (+6..+11, re15_clip_root_motion_delta). Die Schrittweite ist damit fuer die
         * RE2-BEINLAENGE autoriert. Verhaeltnis messen: Bein = Huefte->Oberschenkel->Schien->Fuss. */
        if (load_re2(0x10) && load_re15(0x10)) {
            int leg2  = s_bank.skel.bone_relative_pos[2][1]      /* RE2 Slot 2/3/4 = R-Bein  */
                      + s_bank.skel.bone_relative_pos[3][1]
                      + s_bank.skel.bone_relative_pos[4][1];
            int leg15 = s_sk15.bone_relative_pos[1][1]           /* RE1.5 Slot 1/2/3         */
                      + s_sk15.bone_relative_pos[2][1]
                      + s_sk15.bone_relative_pos[3][1];
            CHECK(leg2 == 1843 && leg15 == 1675,
                  "Beinlaengen verschoben: RE2 %d (erw. 1843) / RE1.5 %d (erw. 1675)",
                  leg2, leg15);
            /* Wurzelweg des RE2-Gehwerk-Clips (Paar 1, Clip 0) ueber das Speed-Feld. */
            const re15_emd_clip_t *c = &s_bank.anim_loco.clips[0];
            int16_t sx0, sy, sz, sx1;
            int kf0 = (int)(s_bank.anim_loco.frames[c->first_frame] & 0xFFFu);
            int kf1 = (int)(s_bank.anim_loco.frames[c->first_frame + c->frame_count - 1] & 0xFFFu);
            re15_emd_get_keyframe_speed(&s_bank.skel_loco, kf0, &sx0, &sy, &sz);
            re15_emd_get_keyframe_speed(&s_bank.skel_loco, kf1, &sx1, &sy, &sz);
            int stride = (int)sx1 - (int)sx0;
            CHECK(c->frame_count == 65 && stride == 1120,
                  "RE2-Gehwerk-Clip 0: %d Frames / %d Einheiten (erw. 65 / 1120)",
                  c->frame_count, stride);
            /* Der Fuss legt bei UNVERAENDERTEN Gelenkwinkeln proportional zur Beinlaenge
             * zurueck; die Wurzel weiter die vollen `stride`. Differenz = Fussrutschen. */
            int slip = stride - (int)((long)stride * leg15 / leg2);
            printf("  (4b) GRENZE Wurzelbewegung: RE2-Bein %d, RE1.5-Bein %d (%.1f%%), "
                   "Gehwerk-Clip 0 = %d Frames / %d Einheiten\n"
                   "       -> rechnerisches Fussrutschen %d Einheiten je Zyklus "
                   "(%.2f Einheiten/Frame, %.1f%%).\n",
                   leg2, leg15, 100.0 * leg15 / leg2, c->frame_count, stride,
                   slip, (double)slip / c->frame_count, 100.0 * slip / stride);
            CHECK(slip > 0 && slip < 200,
                  "Fussrutschen %d ausserhalb des gemessenen Bereichs", slip);
        }
    }

    free(s_re15_ems); free(s_re2_ems);
    if (g_fail) { fprintf(stderr, "test_re2_hybrid_rig: FAILED\n"); return 1; }
    printf("test_re2_hybrid_rig: OK\n");
    return 0;
}
