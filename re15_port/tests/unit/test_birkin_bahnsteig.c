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

    /* Auftritt: Clip 10 (T0[6] @0x80101528), doppelter Takt, dann Gang Clip 5 */
    tick(1);
    CHECK(b->motion == 10 && b->sub_state_1 == 0, "Auftritt: Clip %d sub %d", b->motion, b->sub_state_1);
    for (int i = 0; i < 70; i++) { stelle(b, pl, 6000); tick(1); }
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

    /* Trefferzaehler: ein Treffer im Gang (Zuwachs 3 @0x8010b0fb) unterbricht nicht; dann Reaktion Clip 8 ab Bild 5 */
    beruhigen(b, pl, 200);
    stelle(b, pl, 7000);
    b->state = 2; b->sub_state_1 = 1; b->sub_state_2 = 1;     /* Port-Trefferpfad @80013018 */
    tick(1);
    CHECK(b->state == 1 && b->motion != 8, "ein Treffer darf nicht zucken (Clip %d)", b->motion);
    for (int i = 0; i < 6; i++) { stelle(b, pl, 7000); b->state = 2; b->sub_state_1 = 1; b->sub_state_2 = 1; tick(1); }
    CHECK(b->motion == 8, "kein Zucken nach sieben Treffern (Clip %d)", b->motion);

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
    if (fails) { fprintf(stderr, "test_birkin_bahnsteig: %d Fehler\n", fails); return 1; }
    fprintf(stderr, "test_birkin_bahnsteig: OK\n");
    return 0;
}
