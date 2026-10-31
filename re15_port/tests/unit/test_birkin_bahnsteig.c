/* test_birkin_bahnsteig.c — Bahnsteig-Birkin: Asset gegen RE2-G4, Bodenhoehe, Abmessung und die G4-Ablaeufe.
 *
 * Asset (shared_assets/RE2/BAHNSTEIG_G4.EMD/.TIM, tools/birkin_bahnsteig.py): RE2 CDEMD0.EMS kind 0x34 mit gleicher
 * Hierarchie, gleichem EDD (29 Clips samt Flags), gleicher Topologie und Textur; geaendert sind nur Vertices,
 * Bindoffsets und Keyframes. Der tiefste Punkt jeder Pose liegt auf y = 0 ("steckt nicht im Boden"), Clip 5 ist so
 * gross, wie die Video-Standbilder es gegen Leon und die Frau im Kittel zeigen.
 *
 * Ablaeufe (engine/src/birkin_bahnsteig.c): Auftritt Clip 10 -> Gehen Clip 5, Vortrieb aus der Keyframe-Spur, Hieb an
 * den EDD-Trefferbildern, Griff, Weichen rueckwaerts bei zu nahem Spieler, Trefferzaehler < 12 ohne Zucken, Tod ->
 * Rueckstoss-Szene -> G5, in ROOM5090 (Leon) und ROOM5091 (Elza).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_room.h"
#include "re15_skeleton.h"
#include "re15_birkin_formen.h"
#include "re15_birkin_opfer.h"
#include "re2_ems.h"
#include "re15_anim_select.h"
#include "re15_collision.h"
#include "re15_math.h"

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, size_t *out)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)n);
    if (b && fread(b, 1, (size_t)n, f) != (size_t)n) { free(b); b = NULL; }
    fclose(f);
    if (b) *out = (size_t)n;
    return b;
}

#define RE2_DIR RE15_ASSET_PSX_DIR "/../RE2"

static uint8_t *s_emd, *s_tim, *s_ems2, *s_em30;
static size_t s_emd_n, s_tim_n, s_ems2_n, s_em30_n;

static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

/* Bank wie pc_enemy_load_bahnsteig: G4-Form plus Opfer-Bank von EM30. */
static re15_enemy_bank_t *form_bank(void)
{
    re15_enemy_bank_t *eb = re15_enemy_find(0x30u);
    if (!eb) eb = re15_enemy_alloc(0x30u);
    if (!eb) return NULL;
    {
        uint8_t typ = eb->type;
        memset(eb, 0, sizeof *eb); eb->type = typ;
    }
    if (re2_emd_parse_bank(s_emd, s_emd_n, eb) != 0) return NULL;
    eb->re2_rig = 1; eb->ok = 1; eb->buf = NULL;
    eb->victim_ok = re15_emd_parse_victim_bank(s_em30, s_em30_n, &eb->skel_victim, &eb->anim_victim) == 0;
    return eb;
}

/* ---- Asset ---------------------------------------------------------------------------------- */
static void test_asset(void)
{
    const uint8_t *r = NULL, *t = NULL; size_t rn = 0, tn = 0;
    CHECK(re2_ems_locate(s_ems2, s_ems2_n, 0x34, RE2_EMS_REC_EMD, &r, &rn) == 0, "RE2-G4 EMD nicht im TOC");
    CHECK(re2_ems_locate(s_ems2, s_ems2_n, 0x34, RE2_EMS_REC_TIM, &t, &tn) == 0, "RE2-G4 TIM nicht im TOC");
    if (!r || !t) return;
    CHECK(rn == s_emd_n && rn == 273812u, "EMD-Laenge %zu != RE2 %zu", s_emd_n, rn);
    CHECK(tn == s_tim_n && !memcmp(t, s_tim, tn), "TIM weicht von RE2 ab");
    {
        uint32_t dir = rd32(r);
        uint32_t D[8];
        for (int i = 0; i < 8; i++) D[i] = rd32(r + dir + (size_t)i * 4u);
        CHECK(!memcmp(r + D[1], s_emd + D[1], D[2] - D[1]), "EDD Paar 1 weicht ab (Clips/Flags muessen G4 bleiben)");
        CHECK(!memcmp(r + D[3], s_emd + D[3], D[7] - D[3]), "Paar 2/3 (Opfer) weichen ab");
    }
    static re15_enemy_bank_t ra, fo;
    CHECK(re2_emd_parse_bank(r, rn, &ra) == 0 && re2_emd_parse_bank(s_emd, s_emd_n, &fo) == 0, "Parse");
    CHECK(fo.skel.bone_count == 27 && fo.anim.clip_count == 29, "Knochen %d Clips %d", fo.skel.bone_count, fo.anim.clip_count);
    for (int b = 0; b < fo.skel.bone_count; b++)
        CHECK(fo.skel.bone_parent[b] == ra.skel.bone_parent[b], "Parent %d", b);
    {
        long anders = 0;
        for (int m = 0; m < fo.md1.mesh_count; m++) {
            const re15_md1_mesh_t *a = &ra.md1.meshes[m], *b = &fo.md1.meshes[m];
            CHECK(a->triangle_count == b->triangle_count && a->quad_count == b->quad_count, "Mesh %d Flaechen", m);
            if (a->triangle_count)
                CHECK(!memcmp(a->triangles, b->triangles, sizeof *a->triangles * (size_t)a->triangle_count) &&
                      !memcmp(a->triangle_uvs, b->triangle_uvs, sizeof *a->triangle_uvs * (size_t)a->triangle_count), "Mesh %d Tri", m);
            if (a->quad_count)
                CHECK(!memcmp(a->quads, b->quads, sizeof *a->quads * (size_t)a->quad_count) &&
                      !memcmp(a->quad_uvs, b->quad_uvs, sizeof *a->quad_uvs * (size_t)a->quad_count), "Mesh %d Quad", m);
            for (int v = 0; v < a->tri_vertex_count; v++)
                anders += memcmp(&a->tri_vertices[v], &b->tri_vertices[v], sizeof *a->tri_vertices) != 0;
        }
        CHECK(anders > 500, "kaum umgeformt: %ld Vertices", anders);
    }
}

/* Tiefster und hoechster Punkt einer Pose: Y zeigt nach unten. */
static void pose_grenzen(const re15_enemy_bank_t *b, int kf, int32_t *oben, int32_t *unten, int32_t knochen_z[27])
{
    static re15_skel_pose_t po[RE15_EMD_MAX_BONES];
    *oben = 0x7fffffff; *unten = -0x7fffffff;
    g_anim_pose_actor = NULL;
    CHECK(re15_skel_compute_pose(&b->skel, kf, po) == 0, "Pose kf %d", kf);
    for (int m = 0; m < b->skel.bone_count && m < b->md1.mesh_count; m++) {
        const re15_md1_mesh_t *me = &b->md1.meshes[m];
        const int32_t *r = po[m].rot;
        if (knochen_z) knochen_z[m] = po[m].trans[2];
        for (int v = 0; v < me->tri_vertex_count; v++) {
            int32_t x = me->tri_vertices[v].x, y = me->tri_vertices[v].y, z = me->tri_vertices[v].z;
            int32_t wy = po[m].trans[1] + ((r[3] * x + r[4] * y + r[5] * z) >> 12);
            if (wy < *oben) *oben = wy;
            if (wy > *unten) *unten = wy;
        }
    }
}

static void test_masse(void)
{
    static re15_enemy_bank_t fo;
    CHECK(re2_emd_parse_bank(s_emd, s_emd_n, &fo) == 0, "Parse");
    /* Bodenhoehe: alle Clips, die der Tick benutzt (engine/src/birkin_bahnsteig.c BB_C_*) */
    static const int benutzt[] = { 0, 3, 4, 5, 8, 9, 10, 26, 27 };
    int32_t max_unten = 0, max_schweben = 0;
    for (unsigned c = 0; c < sizeof benutzt / sizeof benutzt[0]; c++) {
        const re15_emd_clip_t *cl = &fo.anim.clips[benutzt[c]];
        for (int f = 0; f < cl->frame_count; f++) {
            int32_t o, u;
            pose_grenzen(&fo, (int)(fo.anim.frames[cl->first_frame + f] & 0xFFFu), &o, &u, NULL);
            if (u > max_unten) max_unten = u;
            if (-u > max_schweben) max_schweben = -u;
        }
    }
    /* Generator rechnet mit Gleitkomma-Rotation, die Pose mit Q12: +-25 ist Rundung, nicht Haltung */
    CHECK(max_unten <= 25 && max_schweben <= 25, "Boden: tiefster Punkt bis %d unter / %d ueber y=0", max_unten, max_schweben);

    /* Abmessung Clip 5 gegen die Standbilder (Mensch ~2750 hoch: Leons Kopfknochen y -2514 im Lauf). Kuppel ~1,1 x Mensch. */
    {
        const re15_emd_clip_t *cl = &fo.anim.clips[5];
        int64_t summe = 0; int n = 0;
        for (int f = 0; f < cl->frame_count; f += 4) {
            int32_t o, u;
            pose_grenzen(&fo, (int)(fo.anim.frames[cl->first_frame + f] & 0xFFFu), &o, &u, NULL);
            summe += -o; n++;
        }
        int32_t hoehe = (int32_t)(summe / n);
        CHECK(hoehe >= 2850 && hoehe <= 3350, "Kuppelhoehe Clip 5 = %d", hoehe);
        fprintf(stderr, "Form: Kuppelhoehe Clip 5 im Mittel %d, Boden-Abweichung %d/%d\n", hoehe, max_unten, max_schweben);
    }
}

/* ---- Ablaeufe ------------------------------------------------------------------------------- */
static re15_rdt_t s_rdt;
static uint8_t *raum_laden(const char *rel, unsigned id)
{
    size_t sz = 0;
    char p[512]; snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/%s", rel);
    uint8_t *buf = slurp(p, &sz);
    if (!buf || re15_rdt_parse(buf, sz, &s_rdt) != 0) return NULL;
    extern void scd_register_current_rdt(const re15_rdt_t *);
    scd_register_current_rdt(NULL);
    g_current_room_id = id;
    re15_enemy_reset();
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 60; i++) scd_vm_tick();
    scd_register_current_rdt(&s_rdt);
    return buf;
}

static re15_actor_t *birkin_finden(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x30u) return &g_actors[s];
    return NULL;
}

/* Spieler relativ zur Form (+X = vor ihr, Gier 0): direkt vor der Maul-Seite. */
static void stelle(re15_actor_t *b, re15_actor_t *pl, int32_t abstand)
{
    b->x = -3000; b->z = -23400; b->rot_y = 0; b->y = 0;
    pl->x = b->x + abstand; pl->z = b->z; pl->hp = 100; pl->hit_react = 0;
}

static void tick(int n) { for (int i = 0; i < n; i++) re15_enemy_ai_run_all(0); }

/* Sperren ablaufen lassen: Spieler 7000 voraus (kein Angriff, kein Rennen), Form kommt in den Gang. */
static void beruhigen(re15_actor_t *b, re15_actor_t *pl, int n)
{
    b->state = 1; b->sub_state_1 = 3; b->sub_state_2 = 0; b->sub_state_3 = 0;
    pl->hit_react = 0; b->hit_react = 0;
    for (int i = 0; i < n; i++) { stelle(b, pl, 7000); tick(1); }
}

static void test_ablauf(unsigned raum, const char *rdt)
{
    uint8_t *buf = raum_laden(rdt, raum);
    CHECK(buf != NULL, "%s nicht ladbar", rdt);
    if (!buf) return;
    re15_bf_modell_hook(NULL);
    re15_actor_t *b = birkin_finden();
    CHECK(b != NULL, "kein 0x30 in %s", rdt);
    if (!b) { free(buf); return; }
    re15_enemy_bank_t *eb = form_bank();
    CHECK(eb && re15_bb_bank_da(b), "Form-Bank nicht erkannt");
    if (!eb) { free(buf); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_bk_opfer_reset();
    b->grid_id = 0x13; b->state = 1; b->sub_state_1 = 6; b->hp = 150; b->no_draw = 0;
    stelle(b, pl, 6000);

    /* Auftritt: Clip 10 (T0[6] @0x80101528), ein Bild je Tick (126 Bilder), dann Gang Clip 5 */
    tick(1);
    CHECK(b->motion == 10 && b->sub_state_1 == 0, "Auftritt: Clip %d sub %d", b->motion, b->sub_state_1);
    for (int i = 0; i < 130; i++) { stelle(b, pl, 6000); tick(1); }
    CHECK(b->motion == 5 && b->sub_state_1 == 3, "nach dem Auftritt kein Gang: Clip %d sub %d", b->motion, b->sub_state_1);

    /* Vortrieb aus der Keyframe-Spur: Clip 5 traegt 2666 Einheiten je Durchlauf (kumulierte Spur, 89 Bilder) */
    {
        int32_t x0, weg = 0;
        for (int i = 0; i < 89; i++) { pl->x = b->x + 6000; pl->z = b->z; x0 = b->x; tick(1); weg += b->x - x0; }
        CHECK(b->motion == 5 && weg > 2200 && weg < 3100, "Gang: %d Einheiten in 89 Ticks (Clip %d)", weg, b->motion);
        fprintf(stderr, "%s: Gang %d Einheiten je Clip-5-Durchlauf\n", rdt, weg);
    }

    /* Griff zuerst: biss_rest = 0 @0x80100758, Griff-Bedingung Kegel 256, dist < 0xed8 */
    beruhigen(b, pl, 5);
    stelle(b, pl, 3000);
    {
        int griff = 0, opfer = 0;
        for (int f = 0; f < 80 && !griff; f++) { tick(1); griff = (b->motion == 27); }
        CHECK(griff, "kein Griff auf 3000 Abstand: Clip %d sub %d", b->motion, b->sub_state_1);
        for (int f = 0; f < 60; f++) { tick(1); if (re15_bk_opfer_cmd()) opfer = 1; }
        CHECK(opfer, "Griff packt den Spieler nicht");
        CHECK(pl->hp < 100, "Griff ohne Schaden (hp %d)", pl->hp);
        re15_bk_opfer_reset();
        re15_player_clear_hit_guard(pl);
    }

    /* Biss/Hieb brauchen die Sperre +0x21e: nach dem Griff 150 Bilder. Danach Hieb (Reichweite 0xfa0) */
    beruhigen(b, pl, 170);
    stelle(b, pl, 3700);
    {
        int hieb = 0, treffer = 0, fenster = 0;
        for (int f = 0; f < 140; f++) {
            int vor = pl->hp;
            tick(1);
            if (b->motion == 3 || b->motion == 4) {
                hieb = 1;
                if (re15_emd_get_frame_entry(&eb->anim, b->motion, b->anim_frame) & 0x10000u) fenster = 1;
            }
            if (pl->hp < vor) treffer = 1;
            pl->x = b->x + 2600; pl->z = b->z;    /* in Reichweite halten: Armpunkt Teil 13/18, Radius 1700 */
        }
        CHECK(hieb, "kein Hieb");
        CHECK(fenster, "Hieb erreicht die EDD-Trefferbilder nicht");
        CHECK(treffer, "Hieb trifft nicht (hp %d)", pl->hp);
    }

    /* Weichen: Spieler zu nah, Sperren laufen -> Gang rueckwaerts statt schieben (Sub 8 @0x8010419c) */
    beruhigen(b, pl, 3);
    b->state = 1; b->sub_state_1 = 3; b->sub_state_2 = 0;
    tick(1);
    /* Griff und Biss liegen in Sperre, sobald sie eben liefen: Griff frisch ausgeloest genuegt nicht, darum
     * hier nur die Bedingung "keine Sperre laeuft ab" pruefen: Hieb/Biss/Griff haben Vorrang, Weichen ist der Rest. */
    stelle(b, pl, 1600);
    {
        int weichen = 0;
        int32_t x0 = b->x;
        for (int f = 0; f < 200 && !weichen; f++) {
            pl->x = b->x + 1600; pl->z = b->z; pl->hit_react = 0;
            tick(1);
            if (b->sub_state_1 == 8 && (b->anim_flags & 0x80u)) weichen = 1;
            if (re15_bk_opfer_cmd()) { re15_bk_opfer_reset(); re15_player_clear_hit_guard(pl); }
        }
        CHECK(weichen, "kein Weichen rueckwaerts bei Abstand 1600 (sub %d)", b->sub_state_1);
        (void)x0;
    }

    /* Trefferzaehler: ein Pistolentreffer (Waffe 3, Zuwachs 3 @0x8010b0fb) unterbricht nicht; ab 12 Reaktion Clip 8 ab Bild 5 */
    beruhigen(b, pl, 200);
    stelle(b, pl, 7000);
    b->state = 2; b->sub_state_1 = 3; b->sub_state_2 = 1;     /* Port-Trefferpfad @80013018, +5 = Waffen-Id */
    tick(1);
    CHECK(b->state == 1 && b->motion != 8, "ein Treffer darf nicht zucken (Clip %d)", b->motion);
    for (int i = 0; i < 3; i++) { stelle(b, pl, 7000); b->state = 2; b->sub_state_1 = 3; b->sub_state_2 = 1; tick(1); }
    CHECK(b->motion == 8, "kein Zucken nach vier Pistolentreffern (Clip %d)", b->motion);

    /* Tod: Rueckstoss-Szene mit Clip 8, dann G5 */
    beruhigen(b, pl, 30);
    b->hp = -1; b->state = 3; b->sub_state_3 = 0;
    {
        int f, szene = 0;
        for (f = 0; f < 900 && b->type == 0x30u; f++) {
            tick(1);
            if (re15_bf_rueckstoss_phase() == 1) { szene = 1; CHECK(b->motion == 8, "Rueckstoss: Clip %d", b->motion); }
        }
        CHECK(szene, "keine Rueckstoss-Szene");
        CHECK(b->type == 0x36u && b->grid_id == 0x13u, "kein G5 (Typ 0x%02X)", b->type);
        fprintf(stderr, "%s: Rueckstoss + G5 nach %d Ticks\n", rdt, f);
    }
    free(buf);
}

/* ---- Nachbesserung nach Abnahme 1 (analysis/befunde_runde41/R_abnahme_1.md, Dossier R_bahnsteig.md "Nachbesserung") --
 * Je ein Riegel fuer einen Mangel; alle waren vor dem jeweiligen Fix rot (Lauf im Dossier). Adressen: G4-Overlay
 * CDEMD0.EMS rec 1 @0x80100000, RE2-EXE SLUS_007.48, RE1.5 STAGE3.BIN (STAGE5 = +0x814). */
static re15_enemy_bank_t *em30_bank(void)
{
    re15_enemy_bank_t *eb = re15_enemy_find(0x30u);
    if (!eb) eb = re15_enemy_alloc(0x30u);
    if (!eb) return NULL;
    {
        uint8_t typ = eb->type;
        memset(eb, 0, sizeof *eb); eb->type = typ;
    }
    if (re15_emd_parse_container(s_em30, s_em30_n, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) return NULL;
    eb->ok = 1; eb->buf = NULL;
    eb->victim_ok = re15_emd_parse_victim_bank(s_em30, s_em30_n, &eb->skel_victim, &eb->anim_victim) == 0;
    return eb;
}

/* Wie der Plattform-Haken pc_enemy_modell_neu: die Bank wechselt synchron im KI-Tick. */
static void hook_form(uint8_t type) { if (type == 0x30u) (void)form_bank(); }

static re15_actor_t *form_start(uint8_t **buf, unsigned raum, const char *rdt)
{
    *buf = raum_laden(rdt, raum);
    if (!*buf) { CHECK(0, "%s nicht ladbar", rdt); return NULL; }
    re15_bf_modell_hook(NULL);
    re15_actor_t *b = birkin_finden();
    if (!b || !form_bank()) { CHECK(0, "%s: kein 0x30 oder keine Form-Bank", rdt); free(*buf); return NULL; }
    re15_bk_opfer_reset();
    b->grid_id = 0x13; b->state = 1; b->sub_state_1 = 6; b->hp = 150; b->no_draw = 0;
    stelle(b, &g_actors[RE15_ACTOR_SLOT_PLAYER], 6000);
    tick(1);
    return b;
}

static void halten(re15_actor_t *b, re15_actor_t *pl, int32_t abstand, int n)
{
    for (int i = 0; i < n; i++) { pl->x = b->x + abstand; pl->z = b->z; tick(1); }
}

/* Treffer-Applier wie re15_damage.c @0x800124bc: +0x5 = Waffen-Id, Zustand 2. */
static void treffer(re15_actor_t *b, uint8_t waffe)
{
    b->state = 2; b->sub_state_1 = waffe; b->sub_state_2 = 1; b->sub_state_3 = 0;
    b->anim_flags &= (uint16_t)~0x80u;
}

/* Bis zur Mutation mit der EM30-Bank, Hook wie im Spiel: Lage wie sub04, Spieler weit vorn (kein Angriff). */
static re15_actor_t *em30_start(uint8_t **buf, unsigned raum, const char *rdt)
{
    *buf = raum_laden(rdt, raum);
    if (!*buf) { CHECK(0, "%s nicht ladbar", rdt); return NULL; }
    re15_actor_t *b = birkin_finden();
    if (!b || !em30_bank()) { CHECK(0, "%s: kein 0x30 oder keine EM30-Bank", rdt); free(*buf); return NULL; }
    re15_bk_opfer_reset();
    b->x = 1200; b->z = -23350; b->grid_id = 0x13; b->no_draw = 0; b->birkin_flags = 0;
    b->state = 1; b->sub_state_1 = 1; b->sub_state_2 = 0; b->sub_state_3 = 0; b->hp = 300; b->hit_react = 0;
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = b->x + 7000; g_actors[RE15_ACTOR_SLOT_PLAYER].z = b->z;
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; g_actors[RE15_ACTOR_SLOT_PLAYER].hit_react = 0;
    return b;
}

/* M1: Treffer zwischen Modelltausch und erstem Formen-Tick (Waffe laeuft vor der KI, game_step_common.c). */
static void test_m1_tauschbild(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = em30_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!b) return;
    re15_bf_modell_hook(hook_form);
    b->hp = 90;                                         /* HP < 100 -> Sub 6 @0x80116684 */
    int m;
    for (m = 0; m < 900 && re15_bf_mutation_offen(); m++) { pl->x = b->x + 7000; pl->z = b->z; tick(1); }
    CHECK(!re15_bf_mutation_offen() && re15_bb_bank_da(b), "M1 %s: kein Modelltausch nach %d Ticks", rdt, m);
    CHECK(re15_enemy_take_damage(b, 5) == 1, "M1 %s: Schuss im Tauschbild prallt ab", rdt);
    tick(1);
    CHECK(b->birkin_hurt_cd > 0, "M1 %s: Treffer im Tauschbild verworfen (state %d cd %d)", rdt, b->state, b->birkin_hurt_cd);
    halten(b, pl, 7000, 12);
    CHECK(!(b->hit_react & 1u), "M1 %s: Riegel +0x93 bleibt stehen (0x%02X)", rdt, b->hit_react);
    {
        int hp = b->hp;
        CHECK(re15_enemy_take_damage(b, 5) == 1 && b->hp < hp, "M1 %s: Form unverwundbar", rdt);
    }
    re15_bf_modell_hook(NULL);
    free(buf);
}

/* M2: Zucken des 0x30 @0x8011a284: Bit 0 je Bild gesetzt @0x8011a39c, Phase 2 loescht es @0x8011a3c0. Gilt fuer jeden
 * 0x30-Kampf; die Bank ist hier EM30, der Raum spielt nur ueber die Stage eine Rolle (RE15-Tick je Raum gleich). */
static void test_m2_zucken(unsigned raum, const char *rdt)
{
    uint8_t *buf = raum_laden(rdt, raum);
    CHECK(buf != NULL, "M2 %s nicht ladbar", rdt);
    if (!buf) return;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER], *b = birkin_finden();
    if (!b) {                                           /* Raum spawnt erst auf Skript-Flag: Aktor wie Sce_em_set */
        b = &g_actors[3];
        memset(b, 0, sizeof *b);
        b->active = 1; b->type = 0x30u; b->em_flag_id = 0xFF;
        re15_enemy_apply_hitbox(b, 0x30u);
    }
    CHECK(em30_bank() != NULL, "M2 %s: EM30-Bank", rdt);
    re15_bk_opfer_reset();
    b->grid_id = 0x10; b->state = 1; b->sub_state_1 = 1; b->sub_state_2 = 0; b->sub_state_3 = 0;
    b->hp = 300; b->hit_react = 0; b->birkin_hurt_cd = 0; b->birkin_flags = 0; b->no_draw = 0;
    b->x = 1000; b->z = -20000; b->rot_y = 0; b->y = 0;
    pl->x = b->x + 7000; pl->z = b->z; pl->hp = 100; pl->hit_react = 0;
    b->hit_stun = 0;                                    /* naechster Treffer zuckt (+0x1dc == 0 @0x8011a1f4) */
    halten(b, pl, 7000, 2);
    CHECK(re15_enemy_take_damage(b, 0) == 1, "M2 %s: erster Treffer prallt ab", rdt);
    tick(1);
    CHECK(b->state == 2 && b->sub_state_3 == 1 && b->motion == 7, "M2 %s: kein Zucken (st %d/%d clip %d)", rdt, b->state, b->sub_state_3, b->motion);
    {
        int n = 0, frei = 0, trotz = 0;
        for (n = 0; n < 80 && b->state == 2; n++) {
            pl->x = b->x + 7000; pl->z = b->z;
            if (!(b->hit_react & 1u)) frei++;
            if (n % 10 == 9 && re15_enemy_take_damage(b, 0)) trotz++;
            tick(1);
        }
        CHECK(frei == 0, "M2 %s: Riegel in %d Zuck-Bildern offen (@0x8011a39c setzt ihn je Bild)", rdt, frei);
        CHECK(trotz == 0, "M2 %s: %d Treffer im Zucken angenommen", rdt, trotz);
        CHECK(b->state == 1 && b->sub_state_1 == 1, "M2 %s: Zucken endet nicht (st %d/%d, %d Ticks)", rdt, b->state, b->sub_state_1, n);
    }
    CHECK(!(b->hit_react & 1u), "M2 %s: nach dem Zucken bleibt der Riegel stehen (0x%02X, cd %d)", rdt, b->hit_react, b->birkin_hurt_cd);
    {
        int hp = b->hp;
        CHECK(re15_enemy_take_damage(b, 0) == 1 && b->hp < hp, "M2 %s: unverwundbar nach dem Zucken", rdt);
    }
    free(buf);
}

/* ROOM3071 (Elza): der Auftritt Sub 10 fuehrt Birkin durch das Tor in die Halle: Fuss-Vortrieb FUN_8011b5c4(1,1) @0x8011960c und
 * keine Wandklemme (+0x0 |= 8 @0x801168d0, FUN_8003b0a4 @0x8003b134-40). Sonst stand er fuer immer hinter SCA 17
 * (x -27055..-26505), ohne Sichtlinie, und der Kampf war mit keiner Waffe zu gewinnen. */
static void test_sub10_tor(void)
{
    uint8_t *buf = raum_laden("STAGE3/ROOM3071.RDT", 0x3071u);
    CHECK(buf != NULL, "ROOM3071 nicht ladbar");
    if (!buf) return;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER], *b = birkin_finden();
    CHECK(b != NULL, "ROOM3071: kein 0x30");
    if (!b) { free(buf); return; }
    CHECK(em30_bank() != NULL, "ROOM3071: EM30-Bank");
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;              /* Wandklemme der Wurzel scharf */
    b->x = -29000; b->z = -11850; b->rot_y = 0; b->y = 0; b->no_draw = 0;
    b->grid_id = 0x01; b->state = 1; b->sub_state_1 = 10; b->sub_state_2 = 0; b->sub_state_3 = 0;   /* sub02 Member_set(0x0c,1) @0x03645 */
    pl->x = -23544; pl->z = -7004; pl->hp = 100; pl->hit_react = 0;
    {
        int n;
        for (n = 0; n < 900 && b->sub_state_1 == 10; n++) tick(1);
        CHECK(b->sub_state_1 != 10, "Sub 10 endet nicht (%d Ticks, Phase %d)", n, b->sub_state_2);
        CHECK(b->x > -26505, "Birkin bleibt hinter dem Tor (x %d), Halle ab x -26505", b->x);
        fprintf(stderr, "ROOM3071: Auftritt endet nach %d Ticks bei (%d,%d)\n", n, b->x, b->z);
    }
    g_room_rdt_ok = 0;
    free(buf);
}

/* M3: Dauerfeuer in der Mutation: Tor @0x801164e8 ist nach Phase 0 zu (grid |= 0x10 @0x80118054), Clip 0x14 laeuft einmal. */
static void test_m3_mutation(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = em30_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!b) return;
    re15_bf_modell_hook(hook_form);
    b->hp = 90;
    int n, neu = 0, alt = -1, treffer_n = 0;
    for (n = 0; n < 3000 && re15_bf_mutation_offen(); n++) {
        pl->x = b->x + 7000; pl->z = b->z; pl->hit_react = 0;
        if (n > 3 && n % 12 == 0 && re15_enemy_take_damage(b, 5)) treffer_n++;
        tick(1);
        if (b->sub_state_1 == 6 && b->motion == 0x14) {
            if ((int)b->anim_frame < alt) neu++;
            alt = b->anim_frame;
        }
    }
    CHECK(treffer_n >= 5, "M3 %s: nur %d Treffer in der Mutation", rdt, treffer_n);
    CHECK(neu <= 1, "M3 %s: Clip 0x14 %d-mal neu gestartet", rdt, neu - 1);     /* einmal wickelt das Clip-Ende */
    CHECK(!re15_bf_mutation_offen(), "M3 %s: Mutation endet unter Dauerfeuer nicht (%d Ticks)", rdt, n);
    CHECK(n < 260, "M3 %s: Mutation dauert %d Ticks unter Beschuss", rdt, n);
    fprintf(stderr, "%s M3: Mutation %d Ticks unter Dauerfeuer, %d Treffer, HP %d\n", rdt, n, treffer_n, b->hp);
    /* Die Form bringt eigene HP mit (RE2-Ctor +0x156 @0x8010067c, Port 0x96 @0x80118094), nicht den Mutationsrest */
    CHECK(b->hp < 150, "M3 %s: Vorbedingung, Dauerfeuer hat die HP nicht gesenkt (%d)", rdt, b->hp);
    tick(1);
    CHECK(b->hp == 150 && b->motion == 10, "M3 %s: Form kommt mit HP %d (Clip %d) statt 150", rdt, b->hp, b->motion);
    re15_bf_modell_hook(NULL);
    free(buf);
}

/* M4: toedlicher Erst-Treffer: Riegel wie im Revive-Zweig (+0x1de = 9 @0x8011a458, +0x93 &= 0xfe @0x8011a54c-50). */
static void test_m4_toedlich(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = em30_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!b) return;
    re15_bf_modell_hook(hook_form);
    CHECK(re15_enemy_take_damage(b, 9) == 1 && b->state == 3, "M4 %s: Rakete nicht toedlich (hp %d st %d)", rdt, b->hp, b->state);
    int m;
    for (m = 0; m < 900 && re15_bf_mutation_offen(); m++) { pl->x = b->x + 7000; pl->z = b->z; tick(1); }
    CHECK(!re15_bf_mutation_offen() && re15_bb_bank_da(b), "M4 %s: kein Modelltausch", rdt);
    halten(b, pl, 7000, 3);
    CHECK(!(b->hit_react & 1u), "M4 %s: Form kommt mit gesetztem Riegel (0x%02X)", rdt, b->hit_react);
    {
        int hp = b->hp;
        CHECK(re15_enemy_take_damage(b, 5) == 1 && b->hp < hp, "M4 %s: Form unverwundbar", rdt);
    }
    re15_bf_modell_hook(NULL);
    free(buf);
}

/* M5/M6: Bild +1 je Tick (@0x80029b28-34), a3 ist das Ueberblendgewicht (@0x800296b8-bc). */
static void test_m5_takt(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = form_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!b) return;
    re15_enemy_bank_t *eb = re15_enemy_find(0x30u);
    const int fc10 = eb->anim.clips[10].frame_count, fc27 = eb->anim.clips[27].frame_count, fc8 = eb->anim.clips[8].frame_count;
    {   /* Auftritt @0x801015d0 a3 0x200 */
        int n = 0;
        CHECK(b->motion == 10 && b->anim_blend_rate == 0x200, "M6 %s: Auftritt Rate 0x%x (Clip %d)", rdt, b->anim_blend_rate, b->motion);
        while (b->motion == 10 && n < 400) { stelle(b, pl, 6000); tick(1); n++; }
        CHECK(n >= fc10 - 2 && n <= fc10 + 2, "M5 %s: Auftritt %d Ticks, Clip hat %d Bilder", rdt, n, fc10);
    }
    {   /* Griff @0x801053ac a3 0x200, Bild 8 @0x80105408-10 */
        int griff_t = -1, griff_n = 0, griff = 0;
        beruhigen(b, pl, 5);
        stelle(b, pl, 3000);
        for (int f = 0; f < 80 && !griff; f++) { tick(1); griff = (b->motion == 27); }
        CHECK(griff, "M5 %s: kein Griff", rdt);
        CHECK(b->anim_blend_rate == 0x200, "M6 %s: Griff Rate 0x%x", rdt, b->anim_blend_rate);
        for (griff_n = 1; griff_n < 400 && b->motion == 27; griff_n++) {
            int hp = pl->hp;
            tick(1);
            if (griff_t < 0 && pl->hp < hp) griff_t = griff_n;
        }
        CHECK(griff_n >= fc27 - 2 && griff_n <= fc27 + 2, "M5 %s: Griff %d Ticks, Clip hat %d Bilder", rdt, griff_n, fc27);
        CHECK(griff_t >= 7 && griff_t <= 9, "M5 %s: Griff greift nach %d Ticks statt 8", rdt, griff_t);
        re15_bk_opfer_reset(); re15_player_clear_hit_guard(pl);
    }
    {   /* Zucken @0x80107240 a3 0x200, Clip 8 ab Bild 5 */
        int n = 0;
        beruhigen(b, pl, 30);
        treffer(b, 7); tick(1);
        treffer(b, 7); tick(1);
        CHECK(b->motion == 8, "M5 %s: kein Zucken nach zwei Magnum-Treffern (Clip %d)", rdt, b->motion);
        CHECK(b->anim_blend_rate == 0x200, "M6 %s: Zucken Rate 0x%x", rdt, b->anim_blend_rate);
        while (b->motion == 8 && n < 200) { stelle(b, pl, 7000); tick(1); n++; }
        CHECK(n >= fc8 - 5 - 3 && n <= fc8 - 5 + 2, "M5 %s: Zucken %d Ticks, Clip %d ab Bild 5", rdt, n, fc8);
    }
    {   /* Weichen @0x80104280 a3 0x10080: ein Bild je Tick rueckwaerts, Gewicht 0x80, Uhr 40 @0x801041f0 */
        int w = 0, schritte = 0, sprung = 0, alt = -1;
        beruhigen(b, pl, 3);
        for (int f = 0; f < 400 && !w; f++) {
            pl->x = b->x + 1600; pl->z = b->z; pl->hit_react = 0;
            tick(1);
            if (b->sub_state_1 == 8) w = 1;
            if (re15_bk_opfer_cmd()) { re15_bk_opfer_reset(); re15_player_clear_hit_guard(pl); }
        }
        CHECK(w, "M5 %s: kein Weichen", rdt);
        CHECK(b->anim_blend_rate == 0x80, "M6 %s: Weichen Rate 0x%x", rdt, b->anim_blend_rate);
        alt = b->anim_frame;
        while (b->sub_state_1 == 8 && schritte < 60) {
            pl->x = b->x + 1600; pl->z = b->z; pl->hit_react = 0;
            tick(1); schritte++;
            if (b->sub_state_1 == 8 && (int)b->anim_frame != alt + 1) sprung++;
            alt = b->anim_frame;
        }
        CHECK(sprung == 0, "M5 %s: Weichen %d Bilder ohne Schritt +1", rdt, sprung);
    }
    free(buf);
}

/* M6: gerenderte Pose steht nicht: a3 je Stelle @0x80104284 (0x80) / @0x80102a34, @0x80101b34 (0x100). */
static re15_skel_pose_t s_po[RE15_EMD_MAX_BONES];
static void pose_folgen(re15_actor_t *b, re15_enemy_bank_t *eb)
{
    int kf = re15_compute_actor_kf(&eb->anim, &eb->skel, b, -1, b->anim_frame);
    g_anim_pose_actor = b; re15_skel_compute_pose(&eb->skel, kf, s_po); g_anim_pose_actor = NULL;
}
static int pose_abstand(const re15_actor_t *b, int16_t alt[32][3])
{
    int m = 0;
    for (int bn = 0; bn < 27; bn++)
        for (int c = 0; c < 3; c++) {
            int d = (b->prev_angles[bn][c] - alt[bn][c]) & 0xfff;
            if (d > 0x800) d = 0x1000 - d;
            if (d > m) m = d;
        }
    memcpy(alt, b->prev_angles, sizeof b->prev_angles[0] * 32);
    return m;
}
static void test_m6_pose(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = form_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!b) return;
    re15_enemy_bank_t *eb = re15_enemy_find(0x30u);
    int16_t alt[32][3];
    beruhigen(b, pl, 170);
    stelle(b, pl, 7000);
    pose_folgen(b, eb);
    {   /* Gehen: kein Standbild am Clip-Eintritt */
        int still = 0;
        b->state = 1; b->sub_state_1 = 3; b->sub_state_2 = 0;
        memcpy(alt, b->prev_angles, sizeof alt);
        for (int i = 0; i < 30; i++) { stelle(b, pl, 7000); tick(1); pose_folgen(b, eb); if (pose_abstand(b, alt) == 0) still++; }
        CHECK(b->anim_blend_rate == 0x100, "M6 %s: Gehen Rate 0x%x", rdt, b->anim_blend_rate);
        CHECK(still <= 1, "M6 %s: Gehen %d von 30 Ticks Standbild", rdt, still);
    }
    {   /* Weichen */
        int w = 0, still = 0, n = 0;
        for (int f = 0; f < 400 && !w; f++) {
            pl->x = b->x + 1600; pl->z = b->z; pl->hit_react = 0;
            tick(1); pose_folgen(b, eb);
            if (b->sub_state_1 == 8) w = 1;
            if (re15_bk_opfer_cmd()) { re15_bk_opfer_reset(); re15_player_clear_hit_guard(pl); }
        }
        CHECK(w, "M6 %s: kein Weichen", rdt);
        memcpy(alt, b->prev_angles, sizeof alt);
        while (b->sub_state_1 == 8 && n < 60) {
            pl->x = b->x + 1600; pl->z = b->z; pl->hit_react = 0;
            tick(1); pose_folgen(b, eb); n++;
            if (pose_abstand(b, alt) == 0) still++;
        }
        CHECK(still * 4 <= n, "M6 %s: Weichen %d von %d Ticks Standbild", rdt, still, n);
        fprintf(stderr, "%s M6: Weichen %d von %d Ticks ohne Pose-Aenderung\n", rdt, still, n);
    }
    free(buf);
}

/* M7/M8: Richtung ist Argument je Aufruf (a3 = 1 @0x80104248, 0x10080 @0x80104280), kein Zustand. */
static void test_m7_weichen_treffer(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = form_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!b) return;
    beruhigen(b, pl, 170);
    {
        int w = 0;
        for (int f = 0; f < 400 && !(w && b->anim_frame >= 13); f++) {
            pl->x = b->x + 1600; pl->z = b->z; pl->hit_react = 0;
            tick(1);
            w = (b->sub_state_1 == 8);
            if (re15_bk_opfer_cmd()) { re15_bk_opfer_reset(); re15_player_clear_hit_guard(pl); }
        }
        CHECK(w, "M7 %s: kein Weichen", rdt);
    }
    CHECK(re15_enemy_take_damage(b, 0) == 1, "M7 %s: Treffer prallt ab", rdt);
    {
        int32_t maxs = 0;
        for (int i = 0; i < 10 && (b->sub_state_1 == 8 || b->state == 2); i++) {
            int32_t x0 = b->x, z0 = b->z;
            pl->x = b->x + 1600; pl->z = b->z; pl->hit_react = 0;
            tick(1);
            int32_t dx = b->x - x0, dz = b->z - z0, s = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
            if (s > maxs) maxs = s;
            if (b->sub_state_1 == 8) CHECK(b->anim_flags & 0x80u, "M7 %s: Weichen laeuft nach dem Treffer vorwaerts", rdt);
        }
        CHECK(maxs <= 60, "M7/M8 %s: Sprung %d Einheiten je Tick nach Treffer im Weichen", rdt, maxs);
    }
    free(buf);
}

/* M10/M17: Doppelschlag ruft FUN_80015e7c zweimal (@0x80101c4c, @0x80101c60): Schritt 0, kein Spur-Sprung. */
static void test_m10_doppelschlag(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = form_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!b) return;
    int wechsel = 0, runden = 0;
    int32_t maxs = 0;
    for (int r = 0; r < 12 && wechsel < 3; r++) {
        int alt = -1, im_hieb = 0;
        beruhigen(b, pl, 170);
        stelle(b, pl, 3700);
        pl->hit_react = 1;                              /* kein Griff, kein Biss: der Spieler ist "in Reaktion" */
        b->state = 1; b->sub_state_1 = 2; b->sub_state_2 = 0;
        for (int f = 0; f < 90; f++) {
            int32_t x0 = b->x, z0 = b->z;
            tick(1);
            if (b->sub_state_1 == 2 && (b->motion == 3 || b->motion == 4)) {
                int32_t dx = b->x - x0, dz = b->z - z0, s = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
                if (im_hieb && s > maxs) maxs = s;
                im_hieb = 1;
                if (alt >= 0 && alt != b->motion) wechsel++;
                alt = b->motion;
            }
            pl->x = b->x + 2600; pl->z = b->z;
        }
        runden++;
    }
    pl->hit_react = 0;
    CHECK(wechsel >= 1, "M10 %s: kein Doppelschlag in %d Runden", rdt, runden);
    CHECK(maxs < 300, "M10/M17 %s: Schritt %d im Hieb (Doppelschlag-Sprung), %d Wechsel", rdt, maxs, wechsel);
    fprintf(stderr, "%s M10: %d Wechsel, groesster Schritt im Hieb %d\n", rdt, wechsel, maxs);
    free(buf);
}

/* M11: Rennen @0x80102b90 kein Lenken nach dem Treffer, beim Treffer steer -0x80 @0x80102f8c, +0x15a = 6 @0x80102f68, je
 * Tick steer -0x10 @0x80103014. */
static int gier_abstand(const re15_actor_t *b, int32_t x, int32_t z)
{
    int soll = ((int)re15_atan2_q12(z - b->z, x - b->x) - 0x400) & 0x0fff;
    int d = (soll - (int)b->rot_y) & 0xfff;
    return d > 0x800 ? 0x1000 - d : d;
}
static void test_m11_rennen(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = form_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!b) return;
    beruhigen(b, pl, 170);
    stelle(b, pl, 9000);
    {
        int32_t px = pl->x, pz = pl->z;
        int f, getroffen = -1;
        for (f = 0; f < 200; f++) {
            int hp = pl->hp;
            tick(1);
            if (pl->hp < hp && getroffen < 0) getroffen = f;
            pl->x = px; pl->z = pz;
            if (getroffen >= 0 && f == getroffen + 7) break;  /* Treffer-Ruck + sechs Bilder Abdrehen */
        }
        CHECK(getroffen >= 0, "M11 %s: Rennen trifft nicht", rdt);
        CHECK(b->motion == 0, "M11 %s: nach dem Treffer kein Rennen (Clip %d)", rdt, b->motion);
        CHECK(gier_abstand(b, px, pz) >= 0x60, "M11 %s: lenkt nach dem Treffer weiter zum Spieler (Abweichung %d)", rdt, gier_abstand(b, px, pz));
    }
    free(buf);
}

/* Abnahme 2, Nachtrag M11 + Diagonale: echter Ablauf aus Spieler-Pass (Schub, Klemme @0x80031cbc/@0x80031d70) und Gegner-Pass, Wandklemme der
 * Wurzel scharf. Form-Wurzel @0x8010052c schiebt die FORM aus dem Spieler, sonst traegt sie ihn an der Wand mit. */
static void bild_mit_klemme(re15_actor_t *pl)
{
    int32_t ax = pl->x, az = pl->z;
    re15_body_push_player();
    re15_collision_set_band(0);
    re15_collision_constrain(&s_rdt, ax, az, &pl->x, &pl->z);
    tick(1);
}

static void rennen_start(re15_actor_t *b, re15_actor_t *pl, int32_t bx, int32_t bz, int gier, int32_t px, int32_t pz)
{
    beruhigen(b, pl, 170);
    b->x = bx; b->z = bz; b->rot_y = (int16_t)gier; b->hit_react = 0;
    b->state = 1; b->sub_state_1 = 4; b->sub_state_2 = 0; b->sub_state_3 = 0;
    pl->x = px; pl->z = pz; pl->y = 0; pl->hp = 100; pl->hit_react = 0;
}

static int32_t weg_nach_treffer(re15_actor_t *b, re15_actor_t *pl, int bilder, int *treffer, int32_t *z_min)
{
    int t0 = -1, n = 0, frei_ab = -1;
    int32_t weg = 0, ax = pl->x, az = pl->z;
    *treffer = 0;
    *z_min = pl->z;
    for (int f = 0; f < 600 && (t0 < 0 || f < t0 + bilder); f++) {
        int hp = pl->hp;
        bild_mit_klemme(pl);
        if (pl->hp < hp) { (*treffer)++; if (t0 < 0) t0 = f; frei_ab = f + 23; }   /* Treffer-Reaktion wie im Lauf: F739 -> F762 */
        if (f == frei_ab) pl->hit_react = 0;
        if (t0 >= 0) {
            int32_t dx = pl->x - ax, dz = pl->z - az;
            weg += (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
            n++;
        }
        ax = pl->x; az = pl->z;
        if (pl->z < *z_min) *z_min = pl->z;
    }
    return weg;
}

static void test_r2_koerper(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = form_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!b) return;
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    {   /* Nordspur: Leon an der Wand z -22168, Form rennt nach Osten, Lage s. Lauf r_13000_22300 */
        int treffer = 0; int32_t z_min = 0;
        rennen_start(b, pl, 7400, -22800, 4035, 13000, -22300);
        int32_t weg = weg_nach_treffer(b, pl, 40, &treffer, &z_min);
        CHECK(treffer >= 1, "M11 %s Nordspur: Rennen trifft nicht", rdt);
        CHECK(weg < 1500, "M11 %s Nordspur: Leon %d Einheiten getragen, %d Treffer", rdt, weg, treffer);
        fprintf(stderr, "%s Nordspur: Leon %d Einheiten nach dem Treffer, %d Treffer, Lage (%d,%d)\n", rdt, weg, treffer, pl->x, pl->z);
    }
    {   /* Diagonale Box 30: Form rennt nach Westen auf z -24182, Leon steht an der Diagonale. Lage aus Lauf rw_24700 F759-F763: dort kam
         * er mit dem Rennen an, bei d 914 versetzte ihn der Schub ueber die Linie (z -25630 -> -26664 -> -27668). */
        int treffer = 0, fest; int32_t z_min = 0, nx, nz;
        rennen_start(b, pl, 9000, -24182, 1733, 3085, -25630);
        (void)weg_nach_treffer(b, pl, 120, &treffer, &z_min);
        nx = pl->x; nz = pl->z;
        re15_collision_constrain(&s_rdt, nx, nz, &nx, &nz);       /* Klemme verschiebt nicht = begehbar, Typ-6-Kasten ist dreieckig */
        fest = (nx != pl->x || nz != pl->z);
        CHECK(treffer >= 1, "Diagonale %s: Rennen trifft nicht", rdt);
        CHECK(z_min > -26000, "Diagonale %s: Leon durch die Wand gedrueckt, z %d", rdt, z_min);
        CHECK(!fest, "Diagonale %s: Leon steht im festen Bereich (%d,%d)", rdt, pl->x, pl->z);
        fprintf(stderr, "%s Diagonale: Leon z min %d, Lage (%d,%d), %d Treffer\n", rdt, z_min, pl->x, pl->z, treffer);
    }
    g_room_rdt_ok = 0;
    free(buf);
}

/* Abnahme 2, Pose am Clip-Ende: das Original posiert im Ende-Tick das letzte Bild, +0x14d faellt erst danach (@0x80029b28-4c); der Port zeichnet
 * anim_frame nach dem Vorschub. Knochen-Verschiebung im Ende-Tick gegen den Tick davor, Form rund 3000 hoch. */
static int32_t knochen_weg(re15_enemy_bank_t *eb, re15_actor_t *b, int32_t alt[27][3], int merken)
{
    int32_t m = 0;
    pose_folgen(b, eb);
    for (int i = 0; i < 27 && i < eb->skel.bone_count; i++) {
        for (int c = 0; c < 3; c++) {
            int32_t d = s_po[i].trans[c] - alt[i][c];
            if (d < 0) d = -d;
            if (d > m) m = d;
            if (merken) alt[i][c] = s_po[i].trans[c];
        }
    }
    return m;
}
static int32_t clip_ende_sprung(re15_enemy_bank_t *eb, re15_actor_t *b, re15_actor_t *pl, int clip, int32_t abstand)
{
    static int32_t alt[27][3];
    int32_t letzter = -1;
    (void)knochen_weg(eb, b, alt, 1);
    for (int f = 0; f < 400; f++) {
        int war = (b->motion == clip);
        int bild = b->anim_frame;
        pl->x = b->x + abstand; pl->z = b->z; pl->hit_react = 1;   /* kein neuer Angriff, Clip laeuft zu Ende */
        tick(1);
        if (!war) { (void)knochen_weg(eb, b, alt, 1); continue; }
        int32_t s = knochen_weg(eb, b, alt, 1);
        if (b->motion != clip || bild + 1 >= eb->anim.clips[clip].frame_count) { letzter = s; break; }
    }
    pl->hit_react = 0;
    return letzter;
}
static void test_r2_clip_ende(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = form_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!b) return;
    re15_enemy_bank_t *eb = re15_enemy_find(0x30u);
    static const struct { int sub, clip; int32_t abstand; const char *name; } fall[] = {
        { 14, 26, 1900, "Biss" }, { 15, 27, 3000, "Griff" },
    };
    {   /* Auftritt laeuft schon */
        int32_t s = clip_ende_sprung(eb, b, pl, 10, 6000);
        CHECK(s >= 0 && s < 150, "Clip-Ende %s Auftritt: Knochen %d Einheiten im Ende-Tick", rdt, s);
    }
    for (unsigned i = 0; i < sizeof fall / sizeof fall[0]; i++) {
        beruhigen(b, pl, 170);
        stelle(b, pl, fall[i].abstand);
        b->state = 1; b->sub_state_1 = (uint8_t)fall[i].sub; b->sub_state_2 = 0; b->sub_state_3 = 0;
        tick(1);
        CHECK(b->motion == fall[i].clip, "Clip-Ende %s %s: Clip %d", rdt, fall[i].name, b->motion);
        int32_t s = clip_ende_sprung(eb, b, pl, fall[i].clip, fall[i].abstand);
        CHECK(s >= 0 && s < 150, "Clip-Ende %s %s: Knochen %d Einheiten im Ende-Tick", rdt, fall[i].name, s);
        fprintf(stderr, "%s Clip-Ende %s: %d\n", rdt, fall[i].name, s);
        re15_bk_opfer_reset(); re15_player_clear_hit_guard(pl);
    }
    {   /* Hieb: Clip 3 oder 4 aus der Seite +0x16a */
        beruhigen(b, pl, 170);
        stelle(b, pl, 3700);
        b->state = 1; b->sub_state_1 = 2; b->sub_state_2 = 0; b->sub_state_3 = 0;
        tick(1);
        CHECK(b->motion == 3 || b->motion == 4, "Clip-Ende %s Hieb: Clip %d", rdt, b->motion);
        int32_t s = clip_ende_sprung(eb, b, pl, b->motion, 4200);   /* dist >= 0xfa0: kein Doppelschlag @0x80101b80-c4 */
        CHECK(s >= 0 && s < 150, "Clip-Ende %s Hieb: Knochen %d Einheiten im Ende-Tick", rdt, s);
        fprintf(stderr, "%s Clip-Ende Hieb: %d\n", rdt, s);
    }
    {   /* Zucken: Clip 8 ab Bild 5 */
        beruhigen(b, pl, 30);
        treffer(b, 7); tick(1);
        treffer(b, 7); tick(1);
        CHECK(b->motion == 8, "Clip-Ende %s Zucken: Clip %d", rdt, b->motion);
        int32_t s = clip_ende_sprung(eb, b, pl, 8, 7000);
        CHECK(s >= 0 && s < 150, "Clip-Ende %s Zucken: Knochen %d Einheiten im Ende-Tick", rdt, s);
    }
    {   /* Schleifen laufen weiter: Gang-Clip wickelt auf 0, kein Stehen */
        int gewickelt = 0, alt = -1;
        beruhigen(b, pl, 3);
        b->state = 1; b->sub_state_1 = 3; b->sub_state_2 = 0;
        for (int f = 0; f < 200 && !gewickelt; f++) {
            stelle(b, pl, 7000); tick(1);
            if (b->motion == 5 && alt > 10 && (int)b->anim_frame < alt) gewickelt = 1;
            alt = b->anim_frame;
        }
        CHECK(gewickelt, "Clip-Ende %s: Gang haelt am Clip-Ende, Schleife wickelt nicht", rdt);
    }
    free(buf);
}

/* Abnahme 3 N4: Austritt aus dem Weichen. A[8] setzt nur 0x201 @0x80104158, die Richtung ist Argument des FUN_8002959c (@0x8010427c-84), der
 * Austritts-Tick zeichnet also dieselbe Pose. Solange bb_weichen Bit 0x80 loeschte, kam im Austritt Slot N statt 88-N: Abnahme Mittel 1230.
 * Gemessen wird der Knochen-Weg im Austritts-Tick, normal in Clip 5 sind hoechstens 211 je Bild. Der Tick danach ist die Ueberblendung in den
 * neuen Clip (Gewicht 14 * 0x100 / 4096), sie ist in jedem Hieb-Eintritt dieselbe und gehoert nicht zum Austritt. */
static void weichen_stellen(re15_actor_t *b, re15_actor_t *pl, re15_enemy_bank_t *eb, int32_t alt[27][3], int ticks)
{
    beruhigen(b, pl, 3);
    b->state = 1; b->sub_state_1 = 8; b->sub_state_2 = 0; b->sub_state_3 = 0;
    (void)knochen_weg(eb, b, alt, 1);
    for (int f = 0; f < ticks; f++) {
        pl->x = b->x + 1600; pl->z = b->z; pl->hit_react = 0;
        tick(1);
        (void)knochen_weg(eb, b, alt, 1);
    }
}
static void test_n4_weichen_austritt(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = form_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!b) return;
    re15_enemy_bank_t *eb = re15_enemy_find(0x30u);
    static int32_t alt[27][3];
    int32_t max_hieb = -1, max_gehen = -1;
    int n_hieb = 0, n_gehen = 0;
    beruhigen(b, pl, 170);
    for (int r = 0; r < 6; r++) {
        /* in den Hieb: dist 2501..3999, Kegel 512, Sperre 0 @0x80104130-58 */
        weichen_stellen(b, pl, eb, alt, 12 + 3 * r);
        CHECK(b->sub_state_1 == 8, "N4 %s: kein Weichen (Sub %d)", rdt, b->sub_state_1);
        for (int f = 0; f < 3 && b->sub_state_1 == 8; f++) {
            pl->x = b->x + 3000; pl->z = b->z; pl->hit_react = 1;
            tick(1);
            if (b->sub_state_1 == 2) {
                int32_t s = knochen_weg(eb, b, alt, 1);
                if (s > max_hieb) max_hieb = s;
                n_hieb++;
            } else (void)knochen_weg(eb, b, alt, 1);
        }
        pl->hit_react = 0;
        /* ins Gehen: Uhr 40 @0x80104294-a8, Spieler bleibt nah */
        beruhigen(b, pl, 170);
        weichen_stellen(b, pl, eb, alt, 4 + r);
        for (int f = 0; f < 60 && b->sub_state_1 == 8; f++) {
            pl->x = b->x + 1600; pl->z = b->z; pl->hit_react = 1;
            tick(1);
            if (b->sub_state_1 != 8) {
                int32_t s = knochen_weg(eb, b, alt, 1);
                if (s > max_gehen) max_gehen = s;
                n_gehen++;
            } else (void)knochen_weg(eb, b, alt, 1);
        }
        pl->hit_react = 0;
        beruhigen(b, pl, 170);
    }
    fprintf(stderr, "%s N4: Weichen -> Hieb %d Austritte, Sprung %d; -> Gehen %d Austritte, Sprung %d\n", rdt, n_hieb, (int)max_hieb, n_gehen, (int)max_gehen);
    CHECK(n_hieb >= 3, "N4 %s: nur %d Austritte in den Hieb", rdt, n_hieb);
    CHECK(n_gehen >= 3, "N4 %s: nur %d Austritte ins Gehen", rdt, n_gehen);
    CHECK(max_hieb >= 0 && max_hieb < 300, "N4 %s: Weichen -> Hieb springt %d", rdt, (int)max_hieb);
    CHECK(max_gehen >= 0 && max_gehen < 300, "N4 %s: Weichen -> Gehen springt %d", rdt, (int)max_gehen);
    free(buf);
}

/* Abnahme 3, Gering: Rennen a1 = 1 @0x80102f10 toetet nie; Doppelschlag loest die Sperre @0x80101c94-a8; Hieb-Eintritt nullt +0x232 @0x80101b00. */
static void test_n_rennen_doppel(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = form_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!b) return;
    {   /* Rennen bei HP 3: Schaden 5, FUN_800401d4(20, 1) klemmt auf 0 */
        int getroffen = 0;
        beruhigen(b, pl, 170);
        stelle(b, pl, 6000);
        pl->hp = 3;
        b->state = 1; b->sub_state_1 = 4; b->sub_state_2 = 0; b->sub_state_3 = 0;
        for (int f = 0; f < 240 && !getroffen; f++) {
            tick(1);
            if (pl->hp != 3) getroffen = 1;
            else { pl->x = b->x + 1500; pl->z = b->z; }
        }
        CHECK(getroffen, "Rennen %s: trifft nicht (Clip %d)", rdt, b->motion);
        CHECK(b->anim_blend_rate == 0x100, "Rennen %s: Rate 0x%x statt 0x100 (a3 @0x80102d08)", rdt, b->anim_blend_rate);
        CHECK(pl->hp == 0, "Rennen %s: HP %d statt 0, a1 = 1 @0x80102f10 toetet nie", rdt, pl->hp);
        CHECK(!re15_player_is_dead(), "Rennen %s: Spieler tot", rdt);
        pl->hp = 100; pl->hit_react = 0; re15_player_clear_hit_guard(pl);
    }
    {   /* Doppelschlag: Spieler in der Reaktion, der Clip-Wechsel loest die Sperre, ein zweiter Treffer ist moeglich */
        int geloest = 0, wechsel = 0;
        for (int r = 0; r < 12 && !wechsel; r++) {
            int alt = -1;
            beruhigen(b, pl, 170);
            stelle(b, pl, 3700);
            b->state = 1; b->sub_state_1 = 2; b->sub_state_2 = 0;
            for (int f = 0; f < 90; f++) {
                pl->hit_react = 1;
                pl->x = b->x + 2600; pl->z = b->z;
                tick(1);
                if (b->sub_state_1 == 2 && (b->motion == 3 || b->motion == 4)) {
                    if (alt >= 0 && alt != b->motion) { wechsel = 1; geloest = (pl->hit_react & 1) == 0; break; }
                    alt = b->motion;
                }
            }
        }
        CHECK(wechsel, "Doppelschlag %s: kein Clip-Wechsel", rdt);
        CHECK(b->anim_blend_rate == 0x100, "Hieb %s: Rate 0x%x statt 0x100 (a3 @0x80101b34)", rdt, b->anim_blend_rate);
        CHECK(geloest, "Doppelschlag %s: Sperre bleibt, der zweite Hieb laeuft ins Leere (@0x80101c94-a8)", rdt);
        pl->hit_react = 0; re15_player_clear_hit_guard(pl);
    }
    {   /* Hieb-Eintritt: `sb zero,562(s2)` @0x80101b00 */
        beruhigen(b, pl, 170);
        re15_bb_cd_lauf_test(90);
        stelle(b, pl, 3700);
        b->state = 1; b->sub_state_1 = 2; b->sub_state_2 = 0;
        tick(1);
        CHECK(re15_bb_cd_lauf_test(-1) == 0, "Hieb %s: Sperre +0x232 steht auf %d, Hieb bleibt stumm", rdt, re15_bb_cd_lauf_test(-1));
    }
    free(buf);
}

/* M12: Tabelle @0x8010b0fb je Waffe (+0x5 im Trefferzustand @0x80107130), nicht je KI-Unterphase. */
static void test_m12_waffe(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = form_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!b) return;
    static const struct { uint8_t waffe, zuwachs; } soll[] = {
        { 3, 3 }, { 4, 3 }, { 7, 10 }, { 8, 5 }, { 12, 2 }, { 13, 8 }, { 19, 4 }, { 20, 10 },   /* Zeile 3,2,5,7,15,8,18,5 */
    };
    for (unsigned i = 0; i < sizeof soll / sizeof soll[0]; i++) {
        beruhigen(b, pl, 220);
        stelle(b, pl, 7000);
        CHECK(re15_bb_zaehler() == 0, "M12 %s: Zaehler baut nicht ab (%d)", rdt, re15_bb_zaehler());
        treffer(b, soll[i].waffe); tick(1);
        CHECK(re15_bb_zaehler() == soll[i].zuwachs, "M12 %s: Waffe %d zaehlt %d statt %d", rdt, soll[i].waffe, re15_bb_zaehler(), soll[i].zuwachs);
    }
    /* Dieselbe Waffe im Biss (Sub 14): kein Zucken beim ersten Pistolentreffer */
    beruhigen(b, pl, 220);
    stelle(b, pl, 7000);
    b->state = 1; b->sub_state_1 = 14; b->sub_state_2 = 0; tick(1);
    treffer(b, 3); tick(1);
    CHECK(b->sub_state_1 == 14 && b->motion == 26, "M12 %s: Pistole bricht den Biss ab (Sub %d Clip %d)", rdt, b->sub_state_1, b->motion);
    free(buf);
}

/* M13/M16: Treffer-Blut Handler 2 (Bodenphase): vier jal 0x8001bf10 @0x80107454/94/b8/f8, nicht bei +5 == 0x10/0xc. */
static void test_m13_blut(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = form_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!b) return;
    beruhigen(b, pl, 30);
    stelle(b, pl, 7000);
    {
        int n0 = re15_bb_blut_zaehler();
        treffer(b, 3); tick(1);
        CHECK(re15_bb_blut_zaehler() == n0 + 4, "M13 %s: %d Blutspawns statt 4", rdt, re15_bb_blut_zaehler() - n0);
        n0 = re15_bb_blut_zaehler();
        treffer(b, 14); tick(1);                        /* Port-14 -> RE2-Zeile 16 = 0x10 @0x80107414 */
        CHECK(re15_bb_blut_zaehler() == n0, "M13 %s: Waffe 14 darf nicht bluten", rdt);
    }
    free(buf);
}

/* M14: Griff und Biss toeten wie RE2 (FUN_800401d4 Rueckgabe 2, @0x801052c4-d0 / @0x80105028), keine Klemme auf 1. */
static void test_m14_tod(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = form_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!b) return;
    {   /* Griff bei niedriger HP: Griff-Gate hp < 15 @0x801024b4 */
        beruhigen(b, pl, 5);
        stelle(b, pl, 3000);
        pl->hp = 14;
        for (int f = 0; f < 120 && pl->hp >= 0; f++) { tick(1); pl->x = b->x + 3000; pl->z = b->z; }
        CHECK(pl->hp < 0, "M14 %s: Griff toetet nicht (hp %d, Clip %d)", rdt, pl->hp, b->motion);
        CHECK(re15_player_is_dead(), "M14 %s: Griff: Spieler nicht tot", rdt);
        CHECK(!re15_bk_opfer_cmd(), "M14 %s: toter Spieler im Rueckstoss-Haken", rdt);
        re15_bk_opfer_reset(); re15_player_clear_hit_guard(pl);
    }
    {   /* Biss */
        beruhigen(b, pl, 170);
        stelle(b, pl, 1900);
        pl->hp = 5; pl->hit_react = 0;
        b->state = 1; b->sub_state_1 = 14; b->sub_state_2 = 0;
        for (int f = 0; f < 120 && pl->hp >= 5; f++) { tick(1); pl->x = b->x + 1900; pl->z = b->z; }
        CHECK(pl->hp < 0, "M14 %s: Biss toetet nicht (hp %d)", rdt, pl->hp);
    }
    pl->hp = 100; pl->hit_react = 0;
    free(buf);
}

/* M15: Griff von hinten: Spieler zeigt der Form das Gesicht (Haken [0] "von vorn" @0x8011b018 schiebt rueckwaerts
 * @0x8011b3f0), sonst flog er in die Form hinein. */
static void test_m15_griff_hinten(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = form_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!b) return;
    beruhigen(b, pl, 5);
    stelle(b, pl, 3000);
    pl->rot_y = 0;                                      /* Blick +X = von der Form weg, Ruecken zur Form */
    {
        int griff = 0;
        for (int f = 0; f < 90 && !re15_bk_opfer_cmd(); f++) { tick(1); griff = griff || (b->motion == 27); }
        CHECK(griff && re15_bk_opfer_cmd(), "M15 %s: Griff packt nicht (Clip %d)", rdt, b->motion);
        int soll = ((int)re15_atan2_q12(b->z - pl->z, b->x - pl->x) - 0x400) & 0x0fff;
        int d = (soll - (int)pl->rot_y) & 0xfff; if (d > 0x800) d = 0x1000 - d;
        CHECK(d < 0x40, "M15 %s: Spieler zeigt nicht zur Form (Gier %d, soll %d)", rdt, pl->rot_y, soll);
    }
    {   /* Haken [0]: Rueckstoss entgegen der Gier, also von der Form weg */
        int32_t d0 = pl->x - b->x;
        for (int f = 0; f < 3; f++) re15_player_victim_tick();
        CHECK(pl->x - b->x > d0, "M15 %s: Griff wirft zur Form (Abstand %d -> %d)", rdt, d0, pl->x - b->x);
    }
    re15_bk_opfer_reset(); re15_player_clear_hit_guard(pl);
    free(buf);
}

int main(void)
{
    s_emd  = slurp(RE2_DIR "/BAHNSTEIG_G4.EMD", &s_emd_n);
    s_tim  = slurp(RE2_DIR "/BAHNSTEIG_G4.TIM", &s_tim_n);
    s_ems2 = slurp(RE2_DIR "/CDEMD0.EMS", &s_ems2_n);
    {
        size_t esz = 0, off = 0, len = 0;
        uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &esz);
        if (ems && re15_ems_get_entry(ems, esz, re15_ems_index_for_type(0x30), &off, &len) == 0) {
            s_em30 = (uint8_t *)malloc(len); memcpy(s_em30, ems + off, len); s_em30_n = len;
        }
        free(ems);
    }
    if (!s_ems2 || !s_em30) { fprintf(stderr, "SKIP: Original-Archive fehlen\n"); return 0; }
    CHECK(s_emd && s_tim, "BAHNSTEIG_G4.EMD/.TIM fehlt (python re15_port/tools/birkin_bahnsteig.py)");
    if (!s_emd || !s_tim) return 1;
    test_asset();
    test_masse();
    test_ablauf(0x5090u, "STAGE5/ROOM5090.RDT");
    test_ablauf(0x5091u, "STAGE5/ROOM5091.RDT");
    {
        static const struct { unsigned id; const char *rdt; } g_kampf[] = {
            { 0x3070u, "STAGE3/ROOM3070.RDT" }, { 0x3071u, "STAGE3/ROOM3071.RDT" },
            { 0x5080u, "STAGE5/ROOM5080.RDT" }, { 0x5081u, "STAGE5/ROOM5081.RDT" },
            { 0x5090u, "STAGE5/ROOM5090.RDT" }, { 0x5091u, "STAGE5/ROOM5091.RDT" },
        };
        for (unsigned i = 0; i < sizeof g_kampf / sizeof g_kampf[0]; i++) test_m2_zucken(g_kampf[i].id, g_kampf[i].rdt);
    }
    test_sub10_tor();
    for (int r = 0; r < 2; r++) {
        const unsigned id = r ? 0x5091u : 0x5090u;
        const char *rdt = r ? "STAGE5/ROOM5091.RDT" : "STAGE5/ROOM5090.RDT";
        test_m1_tauschbild(id, rdt);
        test_m3_mutation(id, rdt);
        test_m4_toedlich(id, rdt);
        test_m5_takt(id, rdt);
        test_m6_pose(id, rdt);
        test_m7_weichen_treffer(id, rdt);
        test_m10_doppelschlag(id, rdt);
        test_m11_rennen(id, rdt);
        test_r2_koerper(id, rdt);
        test_r2_clip_ende(id, rdt);
        test_n4_weichen_austritt(id, rdt);
        test_n_rennen_doppel(id, rdt);
        test_m12_waffe(id, rdt);
        test_m13_blut(id, rdt);
        test_m14_tod(id, rdt);
        test_m15_griff_hinten(id, rdt);
    }
    if (fails) { fprintf(stderr, "test_birkin_bahnsteig: %d Fehler\n", fails); return 1; }
    fprintf(stderr, "test_birkin_bahnsteig: OK\n");
    return 0;
}
