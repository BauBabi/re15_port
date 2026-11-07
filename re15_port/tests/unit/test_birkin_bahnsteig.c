/* test_birkin_bahnsteig.c — Bahnsteig-Birkin (Riesenform): Asset, Formtreue, Clips gegen RE2-G4, Boden, Ablaeufe.
 *
 * Asset seit Stufe 3 (shared_assets/RE2/BAHNSTEIG_FORM.EMD/.TIM, Generator analysis/befunde_runde41/form/stufe3/gen):
 * die Riesenform J nach den zwei 80%-Screenshots. Geprueft: Gestalt, Textur und die drei Posen von J unveraendert
 * (form_j_posen.h), jeder Tick-Clip hat Bildzahl, Spur und EDD-Treffer-/Tonbits des G4-Clips gleicher Nummer
 * (RE2 CDEMD0.EMS kind 0x34), Mindestlaengen der Bildfenster, Wurzeltempo fuer Lenken/Ueberrennen, Boden.
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
#include "form_j_posen.h"

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

/* Bank wie pc_enemy_load_bahnsteig: Riesenform plus Opfer-Bank von EM30, Bank-Bit bb_form wie der Lader. */
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
    eb->re2_rig = 1; eb->ok = 1; eb->buf = NULL; eb->bb_form = 1;
    eb->victim_ok = re15_emd_parse_victim_bank(s_em30, s_em30_n, &eb->skel_victim, &eb->anim_victim) == 0;
    return eb;
}

/* ---- Asset ---------------------------------------------------------------------------------- */
static uint32_t crc32_lauf(const uint8_t *p, size_t n)
{
    uint32_t c = 0xffffffffu;
    for (size_t i = 0; i < n; i++) {
        c ^= p[i];
        for (int k = 0; k < 8; k++) c = (c >> 1) ^ (0xedb88320u & (0u - (c & 1u)));
    }
    return ~c;
}

/* Keyframe kf gegen eine Pose von J: Wurzel und alle 32 Winkeltripel (12 Bit). */
static int pose_gleich(const re15_enemy_bank_t *b, int kf, const int16_t w[3], const int16_t a[32][3])
{
    int16_t x, y, z;
    if (!re15_emd_get_keyframe_position(&b->skel, kf, &x, &y, &z) || x != w[0] || y != w[1] || z != w[2]) return 0;
    for (int k = 0; k < 32; k++) {
        int16_t ax, ay, az;
        if (!re15_emd_get_keyframe_angles(&b->skel, kf, k, &ax, &ay, &az)) return 0;
        if (((ax - a[k][0]) & 0xfff) || ((ay - a[k][1]) & 0xfff) || ((az - a[k][2]) & 0xfff)) return 0;
    }
    return 1;
}

static int kf_von(const re15_enemy_bank_t *b, int clip, int bild)
{
    return (int)(re15_emd_get_frame_entry(&b->anim, clip, bild) & 0xFFFu);
}

/* Die Clips, die der Tick fest benutzt (engine/src/birkin_bahnsteig.c BB_C_*), mit G4-Rolle. */
static const int k_tick_clips[] = { 0, 3, 4, 5, 8, 9, 10, 26, 27 };

static void test_asset(void)
{
    static re15_enemy_bank_t fo, g4;
    const uint8_t *r = NULL; size_t rn = 0;
    CHECK(re2_emd_parse_bank(s_emd, s_emd_n, &fo) == 0, "BAHNSTEIG_FORM.EMD parst nicht");
    CHECK(fo.skel.bone_count == 32 && fo.md1.mesh_count == 32 && fo.anim.clip_count == 28,
          "Knochen %d Meshes %d Clips %d (J: 32/32, Stufe 3: 28 Clips)", fo.skel.bone_count, fo.md1.mesh_count, fo.anim.clip_count);
    /* Formtreue: MD1 und TIM bytegleich mit der Referenz (seit Nachbesserung 1: J mit Kopf K3, varianten/K_alligator/K3_fit), die
     * drei Posen als Keyframes (Ruhe = Ruhe von J mit Kiefer zu, Bild 1/Bild 2 mit den Kopfwinkeln von K3; form_j_posen.h) */
    {
        uint32_t dir = rd32(s_emd), md1 = rd32(s_emd + dir + 7u * 4u);
        size_t md1_n = (size_t)dir - md1;
        CHECK(md1_n == FORM_J_MD1_LEN && crc32_lauf(s_emd + md1, md1_n) == FORM_J_MD1_CRC, "MD1 weicht von J ab (%zu Byte)", md1_n);
        CHECK(s_tim_n == FORM_J_TIM_LEN && crc32_lauf(s_tim, s_tim_n) == FORM_J_TIM_CRC, "TIM weicht von J ab (%zu Byte)", s_tim_n);
    }
    CHECK(pose_gleich(&fo, kf_von(&fo, 2, 0), FORM_J_RUHE_WURZEL, FORM_J_RUHE_WINKEL), "Clip 2 Bild 0 ist nicht die Ruhepose von J");
    CHECK(pose_gleich(&fo, kf_von(&fo, 0, 11), FORM_J_BILD1_WURZEL, FORM_J_BILD1_WINKEL), "Rennen Bild 11 ist nicht Pose Bild 1");
    for (int f = 45; f <= 52; f++)
        CHECK(pose_gleich(&fo, kf_von(&fo, 10, f), FORM_J_BILD2_WURZEL, FORM_J_BILD2_WINKEL), "Drohen Bild %d ist nicht Pose Bild 2", f);
    CHECK(pose_gleich(&fo, kf_von(&fo, 12, 0), FORM_J_BILD2_WURZEL, FORM_J_BILD2_WINKEL), "Sturz Bild 0 ist nicht Pose Bild 2");
    /* Teile, die der Tick fest anspricht: 10 Brust (BB_TEIL_RUMPF), 13/18 Haende (Hieb), alle mit Mesh */
    CHECK(fo.md1.meshes[10].tri_vertex_count > 0 && fo.md1.meshes[13].tri_vertex_count > 0 && fo.md1.meshes[18].tri_vertex_count > 0,
          "Teile 10/13/18 ohne Mesh");
    /* Clips gegen RE2-G4 (CDEMD0.EMS kind 0x34 rec 3): Bildzahl, EDD-Bits 16..31 und Spur je Bild */
    CHECK(re2_ems_locate(s_ems2, s_ems2_n, 0x34, RE2_EMS_REC_EMD, &r, &rn) == 0 && re2_emd_parse_bank(r, rn, &g4) == 0, "RE2-G4 nicht lesbar");
    if (!r) return;
    for (unsigned i = 0; i < sizeof k_tick_clips / sizeof k_tick_clips[0]; i++) {
        const int c = k_tick_clips[i];
        const int n = fo.anim.clips[c].frame_count;
        int bits = 0, spur = 0;
        CHECK(n == g4.anim.clips[c].frame_count, "Clip %d: %d Bilder, G4 %d", c, n, g4.anim.clips[c].frame_count);
        if (n != g4.anim.clips[c].frame_count) continue;
        for (int f = 0; f < n; f++) {
            uint32_t a = re15_emd_get_frame_entry(&fo.anim, c, f), b = re15_emd_get_frame_entry(&g4.anim, c, f);
            int16_t ax, ay, az, bx, by, bz;
            bits += (a & 0xFFFF0000u) != (b & 0xFFFF0000u);
            (void)re15_emd_get_keyframe_speed(&fo.skel, (int)(a & 0xFFFu), &ax, &ay, &az);
            (void)re15_emd_get_keyframe_speed(&g4.skel, (int)(b & 0xFFFu), &bx, &by, &bz);
            spur += (ax != bx || ay != by || az != bz);
        }
        CHECK(bits == 0, "Clip %d: %d Bilder mit anderen EDD-Bits als G4", c, bits);
        /* Clip 10: Pose Bild 2 steht in Bild 45-52 still, das Tempo dieser Bilder liegt in 53-60 (Summe gleich, STUFE3.md) */
        if (c == 10) {
            int16_t ax, ay, az, bx, by, bz;
            (void)re15_emd_get_keyframe_speed(&fo.skel, kf_von(&fo, c, n - 1), &ax, &ay, &az);
            (void)re15_emd_get_keyframe_speed(&g4.skel, kf_von(&g4, c, n - 1), &bx, &by, &bz);
            CHECK(ax == bx && az == bz && spur <= 16, "Clip 10: Spur-Summe %d/%d gegen G4 %d/%d, %d Bilder anders", ax, az, bx, bz, spur);
        } else
            CHECK(spur == 0, "Clip %d: %d Bilder mit anderer Spur als G4", c, spur);
    }
    /* Mindestlaengen der Bildfenster des Ticks: k_doppel (Clip 3 Bild 30, Clip 4 Bild 19, Einstieg 20/7), Zucken ab Bild 5, Griff Bild 8 */
    CHECK(fo.anim.clips[3].frame_count >= 31 && fo.anim.clips[4].frame_count >= 21 && fo.anim.clips[8].frame_count >= 6 &&
          fo.anim.clips[27].frame_count >= 9, "Mindestlaengen 3/4/8/27: %d %d %d %d", fo.anim.clips[3].frame_count,
          fo.anim.clips[4].frame_count, fo.anim.clips[8].frame_count, fo.anim.clips[27].frame_count);
    for (int c = 0; c < fo.anim.clip_count; c++)
        CHECK(fo.anim.clips[c].frame_count >= 1 && fo.anim.clips[c].frame_count <= 256, "Clip %d: %d Bilder (1..256)", c, fo.anim.clips[c].frame_count);
    /* Trefferbilder (Bit 16) in Hieb 3/4 und Biss 26 */
    {
        static const int tc[] = { 3, 4, 26 };
        for (int i = 0; i < 3; i++) {
            int t = 0;
            for (int f = 0; f < fo.anim.clips[tc[i]].frame_count; f++) t += (re15_emd_get_frame_entry(&fo.anim, tc[i], f) & 0x10000u) != 0;
            CHECK(t > 0, "Clip %d ohne Trefferbild", tc[i]);
        }
    }
    /* Wurzeltempo: Gehen erreicht die Lenkschwellen 21 und 31 je Bild (@0x80102924-68), Rennen + 10 die Ueberrenn-Schwelle 201 (@0x80102e80) */
    {
        int max5 = 0, max0 = 0, alt = 0;
        for (int f = 0; f < fo.anim.clips[5].frame_count; f++) {
            int16_t x, y, z; (void)re15_emd_get_keyframe_speed(&fo.skel, kf_von(&fo, 5, f), &x, &y, &z);
            if (f && x - alt > max5) max5 = x - alt; alt = x;
        }
        alt = 0;
        for (int f = 0; f < fo.anim.clips[0].frame_count; f++) {
            int16_t x, y, z; (void)re15_emd_get_keyframe_speed(&fo.skel, kf_von(&fo, 0, f), &x, &y, &z);
            if (f && x - alt > max0) max0 = x - alt; alt = x;
        }
        CHECK(max5 >= 31 && max0 + 10 >= 201, "Tempo Gehen max %d (>= 31), Rennen max %d + 10 (>= 201)", max5, max0);
    }
    /* Stufe-3-Clips: Stehen, Flucht (endet oben), Sturz (endet unter dem Wagen) */
    {
        int16_t x, y, z;
        CHECK(fo.anim.clips[2].frame_count >= 30, "Stehen: %d Bilder", fo.anim.clips[2].frame_count);
        (void)re15_emd_get_keyframe_position(&fo.skel, kf_von(&fo, 11, fo.anim.clips[11].frame_count - 1), &x, &y, &z);
        CHECK(fo.anim.clips[11].frame_count >= 30 && y < -5000, "Flucht: %d Bilder, Wurzel am Ende y %d", fo.anim.clips[11].frame_count, y);
        (void)re15_emd_get_keyframe_position(&fo.skel, kf_von(&fo, 12, fo.anim.clips[12].frame_count - 1), &x, &y, &z);
        CHECK(fo.anim.clips[12].frame_count >= 30 && y > 2000, "Sturz: %d Bilder, Wurzel am Ende y %d", fo.anim.clips[12].frame_count, y);
        /* Sturz im Wagen (Nachbesserung 1, M1): bis Bild 30 traegt die Spur die Form rund 3200 rueckwaerts (Tuer x -5683 bei Start x -2500) */
        (void)re15_emd_get_keyframe_speed(&fo.skel, kf_von(&fo, 12, 30), &x, &y, &z);
        CHECK(fo.anim.clips[12].frame_count > 30 && x <= -3000 && x >= -3500, "Sturz: Spur bis Bild 30 %d (Soll rund -3200)", x);
    }
    /* Taumeln Clip 13 (Nachbesserung 1, M5, PORT-WAHL): Rueckstoss-Schleife, 40 Bilder, Spur -80 je Bild rueckwaerts (Tick schiebt sie
     * in Rueckstoss-Phase 1, birkin_formen.c) */
    {
        int alt = 0, gleich = 1;
        CHECK(fo.anim.clips[13].frame_count == 40, "Taumeln: %d Bilder (40)", fo.anim.clips[13].frame_count);
        for (int f = 0; f < fo.anim.clips[13].frame_count; f++) {
            int16_t x, y, z; (void)re15_emd_get_keyframe_speed(&fo.skel, kf_von(&fo, 13, f), &x, &y, &z);
            gleich &= (x - alt == -80) && z == 0; alt = x;
        }
        CHECK(gleich, "Taumeln: Spur nicht -80 je Bild");
    }
}

/* Tiefster und hoechster Punkt einer Pose: Y zeigt nach unten. */
static void pose_grenzen(const re15_enemy_bank_t *b, int kf, int32_t *oben, int32_t *unten, int32_t knochen_z[32])
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
    /* Boden: alle Tick-Clips. Ausnahmen mit Grund (STUFE3.md): Rennen Bild 11 = Pose Bild 1 von J (der linke Fuss liegt dort
     * 334 unter der Ebene, so wie die Kamera von Bild 1 es verlangt, varianten/VARIANTEN.md Abschnitt 12), Drohen Bild 31-60
     * (Hinterbeine auf dem Weg in die Hocke von Bild 2 und zurueck; seit Nachbesserung 1 hebt die Bodenkorrektur zuerst den
     * Unterschenkel, Rest bis 70 unter die Ebene, gemessen 67 in Bild 55; vorher Bild 33-38 bis 250, gemessen 242). */
    int32_t max_unten = 0, max_schweben = 0;
    for (unsigned c = 0; c < sizeof k_tick_clips / sizeof k_tick_clips[0]; c++) {
        const int clip = k_tick_clips[c];
        const re15_emd_clip_t *cl = &fo.anim.clips[clip];
        for (int f = 0; f < cl->frame_count; f++) {
            int32_t o, u;
            pose_grenzen(&fo, (int)(fo.anim.frames[cl->first_frame + f] & 0xFFFu), &o, &u, NULL);
            if (clip == 0 && f == 11) { CHECK(u <= 340, "Rennen Bild 11 (Pose Bild 1): %d unter y=0", u); continue; }
            if (clip == 10 && f >= 31 && f <= 60) { CHECK(u <= 70, "Drohen Bild %d: %d unter y=0", f, u); continue; }
            if (u > max_unten) max_unten = u;
            if (-u > max_schweben) max_schweben = -u;
        }
    }
    /* Generator rechnet mit Gleitkomma-Rotation, die Pose mit Q12: +-30 ist Rundung, nicht Haltung. Schweben bis 120: im
     * Ausfall der Hiebe (Clip 3 Bild 29-31, Clip 4 Bild 10-11) und im Aufbaeumen (Clip 10 Bild 76) ist kurz kein Teil am Boden,
     * Generator 90, Port-Pose 101 (stufe3/gen/s3_pruef.py). */
    CHECK(max_unten <= 30 && max_schweben <= 120, "Boden: tiefster Punkt bis %d unter / %d ueber y=0", max_unten, max_schweben);
    /* Abmessung Clip 5 (Formtreue J): Hoehe im Mittel, gemessen am Asset (STUFE3.md), +-60 */
    {
        const re15_emd_clip_t *cl = &fo.anim.clips[5];
        int64_t summe = 0; int n = 0;
        for (int f = 0; f < cl->frame_count; f += 4) {
            int32_t o, u;
            pose_grenzen(&fo, (int)(fo.anim.frames[cl->first_frame + f] & 0xFFFu), &o, &u, NULL);
            summe += -o; n++;
        }
        int32_t hoehe = (int32_t)(summe / n);
        CHECK(hoehe >= FORM_HOEHE_GEHEN - 60 && hoehe <= FORM_HOEHE_GEHEN + 60, "Hoehe Clip 5 = %d (J %d)", hoehe, FORM_HOEHE_GEHEN);
        fprintf(stderr, "Form: Hoehe Clip 5 im Mittel %d, Boden-Abweichung %d/%d\n", hoehe, max_unten, max_schweben);
    }
}

/* ---- Ablaeufe ------------------------------------------------------------------------------- */
static re15_rdt_t s_rdt;
static uint8_t *raum_laden(const char *rel, unsigned id)
{
    size_t sz = 0;
    char p[512]; snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/%s", rel);
    uint8_t *buf = slurp(p, &sz);
    /* Nachbesserung 1: g_room_rdt traegt nach test_sub10_tor/test_r2_koerper Zeiger in den schon freigegebenen Puffer des vorigen
     * Raums (nur _ok wurde 0); die NPC-Wandklemme (re15_npc_wall_clamp -> re15_collision_constrain_contact_band) las ihn trotzdem.
     * Mit dem groesseren Asset K3 lag der Puffer anders im Speicher -> SIGSEGV in test_m5_takt. Je Raumladung leeren. */
    memset(&g_room_rdt, 0, sizeof g_room_rdt); g_room_rdt_ok = 0;
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

    /* Tod: Rueckstoss-Szene, dann G5. Nachbesserung 1 M5: Phase 1 taumelt mit Clip 13 (Spur -80 je Bild) statt auf Clip 8 zu
     * gleiten (vorher 147 je Bild); Clip 8 nur noch am Platz an der Sturzgrenze. M1: der Sturz (Phase 3) beginnt im Wagen bei
     * x <= -2500, nicht erst hinter der Tuer (-6283). */
    beruhigen(b, pl, 30);
    b->x = 6000; b->z = -23400; pl->x = b->x + 7000; pl->z = b->z;   /* Kampflage wie im Lauf l5090_feuer (Form x 5994 bei HP 0) */
    b->hp = -1; b->state = 3; b->sub_state_3 = 0;
    {
        int f, szene = 0, taumel = 0, sturz_x_ok = -1;
        int32_t schritt_max = 0;
        for (f = 0; f < 1200 && b->type == 0x30u; f++) {
            const int ph0 = re15_bf_rueckstoss_phase();
            const int32_t x0 = b->x, z0 = b->z;
            tick(1);
            if (re15_bf_rueckstoss_phase() == 1) {
                szene = 1;
                CHECK(b->motion == 13 || b->motion == 8, "Rueckstoss: Clip %d", b->motion);
                if (b->motion == 13) {
                    int32_t dx = b->x - x0, dz = b->z - z0, s = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
                    taumel++;
                    if (s > schritt_max) schritt_max = s;
                }
            }
            if (ph0 == 1 && re15_bf_rueckstoss_phase() == 3) sturz_x_ok = (b->x <= -2500 && b->x > -3500);
        }
        CHECK(szene, "keine Rueckstoss-Szene");
        CHECK(taumel > 20, "Rueckstoss: kaum Taumeln (%d Bilder Clip 13)", taumel);
        CHECK(schritt_max <= 100, "Rueckstoss: Schritt %d je Bild (Gleiten wie vorher 147?)", schritt_max);
        CHECK(sturz_x_ok == 1, "Rueckstoss: Sturz nicht im Wagen begonnen (%d)", sturz_x_ok);
        /* G5-Auftritt Nachbesserung 1 (M7): erst RE2s Vorlauf (Leon laeuft per Plc_dest-Walker von -3800 nach 1400, sub06 @0x018B0),
         * dann die Armierung @0x018DE (grid 0x13). Der Walker laeuft in main.c vor dem Spielschritt, der Szenenmodus kommt aus
         * flag(1,27)||flag(2,7). */
        for (int v = 0; v < 200 && b->type == 0x36u && b->grid_id != 0x13u; v++) {
            if (re15_cine_active()) g_scd.player_mode = 2;
            re15_actor_step_all_walkers();
            tick(1);
        }
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
    /* Die Form bringt eigene HP mit (RE2-Ctor +0x156 @0x8010067c = RE15_BB_FORM_HP 1000, GR NB3), nicht den Mutationsrest */
    CHECK(b->hp < RE15_BIRKIN_HP_MUTATION, "M3 %s: Vorbedingung, Dauerfeuer hat die HP nicht gesenkt (%d)", rdt, b->hp);
    tick(1);
    CHECK(b->hp == RE15_BB_FORM_HP && b->motion == 10, "M3 %s: Form kommt mit HP %d (Clip %d) statt %d", rdt, b->hp, b->motion, RE15_BB_FORM_HP);
    re15_bf_modell_hook(NULL);
    free(buf);
}

/* M4: toedlicher Erst-Treffer: Riegel wie im Revive-Zweig (+0x1de = 9 @0x8011a458, +0x93 &= 0xfe @0x8011a54c-50). */
static void test_m4_toedlich(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = em30_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!b) return;
    re15_bf_modell_hook(hook_form);
    /* Runde 41 Spur WS: die Rakete macht an Form 1 RE2-G1 200 (Zeile 17 K0 @0x800A58B0, re15_rakete_schaden) statt 1000. Thema
     * ist der toedliche ERST-Treffer -> Form 1 vorher auf 199 (unter 200: 199 - 200 = -1 < 0, Zustand 3). */
    b->hp = 199;
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
    for (int bn = 0; bn < 32; bn++)
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
static int32_t knochen_weg(re15_enemy_bank_t *eb, re15_actor_t *b, int32_t alt[32][3], int merken)
{
    int32_t m = 0;
    pose_folgen(b, eb);
    for (int i = 0; i < 32 && i < eb->skel.bone_count; i++) {
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
    static int32_t alt[32][3];
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
static void weichen_stellen(re15_actor_t *b, re15_actor_t *pl, re15_enemy_bank_t *eb, int32_t alt[32][3], int ticks)
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
    static int32_t alt[32][3];
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

/* ---- Stufe 3: Flucht in 5080/5081 ------------------------------------------------------------------ *
 * Bei HP <= 0 Flucht (Clip 11) statt Tod: weg vom Spieler, Treffer halten sie nicht auf, am Clip-Ende Flag (5,0x1c) wie der
 * Birkin-Tod @0x8011aecc (ROOM5080 sub01 @0x0078A wartet darauf) und der Aktor ist fort. */
static void test_flucht(unsigned raum, const char *rdt)
{
    uint8_t *buf = raum_laden(rdt, raum);
    CHECK(buf != NULL, "Flucht: %s nicht ladbar", rdt);
    if (!buf) return;
    re15_bf_modell_hook(NULL);
    CHECK(re15_bf_riesenform_raum() && re15_bf_flucht_raum() && re15_bf_bahnsteig_bank_faellig(0x30u) && !re15_bf_bahnsteig_aktiv(),
          "Flucht %s: Raumwahl falsch", rdt);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER], *b = birkin_finden();
    CHECK(b != NULL, "Flucht %s: kein 0x30", rdt);
    if (!b || !form_bank()) { free(buf); return; }
    re15_bk_opfer_reset();
    re15_game_flag_set(5, 0x1c, 0);
    b->x = -18100; b->z = -17800; b->y = 0; b->rot_y = 0; b->grid_id = 0x13; b->state = 1; b->sub_state_1 = 6; b->hp = 150;
    b->no_draw = 0;
    pl->x = b->x + 5000; pl->z = b->z; pl->hp = 100; pl->hit_react = 0;
    tick(3);
    CHECK(b->motion == 10, "Flucht %s: kein Auftritt (Clip %d)", rdt, b->motion);
    b->hp = -1; b->state = 3; b->sub_state_3 = 0;
    tick(1);
    /* Nachbesserung 1 M6: Spieler 5000 voraus (< BB_FLUCHT_ABSTAND 6000) -> zuerst zurueckweichen (Clip 5 rueckwaerts, Phase 1),
     * dann mit Schritten drehen (Clip 5, Phase 2), erst dann Clip 11. Vorher: Clip 11 sofort, 128 je Bild auf stehenden Fuessen. */
    CHECK(b->active && b->motion == 5 && b->sub_state_1 == 21 && b->sub_state_2 == 1 && (b->anim_flags & 0x80u),
          "Flucht %s: Tod statt Flucht oder kein Zurueckweichen (Clip %d sub %d ph %d)", rdt, b->motion, b->sub_state_1, b->sub_state_2);
    CHECK(re15_game_flag_get(5, 0x1c) == 0, "Flucht %s: Schalter schon beim Fluchtbeginn", rdt);
    {
        int32_t d0 = re15_enemy_player_dist(b, pl), schwanz_min = 0x7fffffff;
        int f, getroffen = 0, drehen = 0, clip11 = 0, dreh_max = 0, dreh11_max = 0, alt = b->rot_y;
        for (f = 0; f < 400 && b->active; f++) {
            pl->x = -13100; pl->z = -17800; pl->hit_react = 0;
            if (f == 10 || f == 20) { treffer(b, 3); getroffen++; }   /* Pistole: Flucht laeuft weiter */
            tick(1);
            if (!b->active) break;
            {
                int d = ((int)b->rot_y - alt) & 0xfff; if (d > 0x800) d = 0x1000 - d;
                alt = b->rot_y;
                if (b->sub_state_2 == 2) { drehen++; if (d > dreh_max) dreh_max = d; CHECK(b->motion == 5, "Flucht %s: Drehen ohne Gang (Clip %d)", rdt, b->motion); }
                if (b->motion == 11) { clip11++; if (d > dreh11_max) dreh11_max = d; }
            }
            {   /* Schwanz (Glieder 4-6) nie im Spieler (r 450) */
                static const int32_t n3[3] = { 0, 0, 0 };
                for (int k = 4; k <= 6; k++) {
                    int32_t w[3];
                    if (re15_enemy_part_point(b, k, n3, w)) {
                        int32_t dx = w[0] - pl->x, dz = w[2] - pl->z, s = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
                        if (s < schwanz_min) schwanz_min = s;
                    }
                }
            }
            if (f == 25) CHECK(b->sub_state_1 == 21, "Flucht %s: Treffer bricht die Flucht ab (sub %d)", rdt, b->sub_state_1);
            if (f == 25) CHECK(re15_enemy_player_dist(b, pl) > d0, "Flucht %s: entfernt sich nicht (%d -> %d)", rdt, d0, re15_enemy_player_dist(b, pl));
        }
        CHECK(!b->active, "Flucht %s: Aktor nach %d Ticks noch da", rdt, f);
        CHECK(re15_game_flag_get(5, 0x1c) == 1, "Flucht %s: Schalter (5,0x1c) fehlt", rdt);
        CHECK(drehen > 0 && dreh_max <= 40, "Flucht %s: Drehen %d Bilder, groesster Gierschritt %d (Rate 32)", rdt, drehen, dreh_max);
        CHECK(clip11 > 0 && dreh11_max <= 40, "Flucht %s: Clip 11 dreht %d je Bild (vorher 128)", rdt, dreh11_max);
        CHECK(schwanz_min > 900, "Flucht %s: Schwanz %d am Spieler vorbei (r 450)", rdt, schwanz_min);
        fprintf(stderr, "%s: Flucht nach %d Ticks fort, Treffer %d, Drehen %d Bilder (max %d je Bild), Clip 11 %d Bilder, Schwanz min %d\n",
                rdt, f, getroffen, drehen, dreh_max, clip11, schwanz_min);
    }
    free(buf);
}

/* ---- Nachbesserung 1 (Abnahme 1 der Stufe 3, M3/M6/M7/M8a, Kopf-Folgen D) ------------------------------------------- */

/* D: Biss am Kiefer (Teil 23 + (928,-85,0) = Zahnreihen-Mitte Kopf K4 (NB2; K3: (911,-75,0)), Radius 1500 @0x80104f88-a4). Gegenprobe: am alten Brustpunkt Teil 10 + (1400,0,0) haette
 * der Spieler getroffen werden muessen, am Kiefer nicht -> kein Treffer. Und: Spieler an der gezeichneten Kieferspitze -> Treffer. */
static int biss_lauf(re15_actor_t *b, re15_actor_t *pl, int32_t abstand, int32_t *kiefer_min, int32_t *brust_min)
{
    static const int32_t spitze[3] = { 928, -85, 0 }, brust[3] = { 1400, 0, 0 };
    int hp0, f, getroffen = 0;
    beruhigen(b, pl, 30);
    stelle(b, pl, abstand);
    pl->hp = 100; hp0 = pl->hp;
    b->state = 1; b->sub_state_1 = 14; b->sub_state_2 = 0; b->sub_state_3 = 0;
    *kiefer_min = *brust_min = 0x7fffffff;
    for (f = 0; f < 34 && !getroffen; f++) {
        int32_t w[3], dx, dz, s;
        pl->x = b->x + abstand; pl->z = b->z; pl->hit_react = 0;
        tick(1);
        if (pl->hp < hp0) getroffen = 1;
        if (b->motion == 26 && b->anim_frame >= 16 && b->anim_frame <= 31) {
            if (re15_enemy_part_point(b, 23, spitze, w)) {
                dx = w[0] - pl->x; dz = w[2] - pl->z; s = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
                if (s < *kiefer_min) *kiefer_min = s;
            }
            if (re15_enemy_part_point(b, 10, brust, w)) {
                dx = w[0] - pl->x; dz = w[2] - pl->z; s = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
                if (s < *brust_min) *brust_min = s;
            }
        }
    }
    re15_player_clear_hit_guard(pl);
    return getroffen;
}

static void test_n1_biss_kiefer(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = form_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int nah_ok = 0, fern_ok = 0;
    if (!b) return;
    for (int32_t a = 2600; a <= 6400; a += 200) {
        int32_t km, bm;
        const int t = biss_lauf(b, pl, a, &km, &bm);
        if (t) CHECK(km < 1500, "D %s: Biss trifft bei %d, Kiefer aber %d entfernt", rdt, a, km);
        if (!t && bm < 1500) fern_ok++;                         /* Gegenprobe: alter Brustpunkt haette getroffen, der Kiefer nicht */
        if (t && km < 1500) nah_ok++;
        fprintf(stderr, "%s D: Abstand %d Treffer %d Kiefer-min %d Brustpunkt-min %d\n", rdt, a, t, km, bm);
    }
    CHECK(nah_ok > 0, "D %s: Biss trifft nie am Kiefer", rdt);
    CHECK(fern_ok > 0, "D %s: keine Gegenprobe (alter Brustpunkt haette getroffen, Kiefer nicht)", rdt);
    free(buf);
}

/* M3: G4-EDD-Ids 9-15 (Bank 24 FFFFFFFF) -> Raum-Ton 10; Clip 10 traegt Id 11/10/12/11 in Bild 30/51/81/113 (G4-Daten). */
static void test_n1_gebruell(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = form_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!b) return;
    stelle(b, pl, 9000);
    b->state = 1; b->sub_state_1 = 0; b->sub_state_2 = 0;
    {
        const int n0 = re15_bb_ton_zaehler();
        for (int f = 0; f < 130 && b->sub_state_1 == 0; f++) { pl->x = b->x + 9000; pl->z = b->z; tick(1); }
        CHECK(re15_bb_ton_zaehler() - n0 == 4, "M3 %s: Auftritt %d Gebruelle (Raum-Ton 10) statt 4", rdt, re15_bb_ton_zaehler() - n0);
    }
    free(buf);
}

/* M7: der Spieler steht nicht unter dem Bauch. Spieler auf Brust und Huefte gesetzt -> Spieler-Pass schiebt ihn hinaus. */
static void test_n1_koerper(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = form_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    static const int32_t n3[3] = { 0, 0, 0 };
    if (!b) return;
    beruhigen(b, pl, 30);
    for (int teil = 0; teil <= 10; teil += 10) {
        int32_t w[3], dx, dz, s;
        stelle(b, pl, 7000);
        b->state = 1; b->sub_state_1 = 3;
        (void)re15_enemy_part_point(b, teil, n3, w);
        pl->x = w[0] + 200; pl->z = w[2] + 300; pl->y = 0;
        for (int i = 0; i < 4; i++) re15_body_push_player();
        (void)re15_enemy_part_point(b, teil, n3, w);
        dx = pl->x - w[0]; dz = pl->z - w[2]; s = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
        CHECK(s >= 1400, "M7 %s: Spieler %d von Teil %d (Rumpf r 1000/1100 + 450)", rdt, s, teil);
    }
    CHECK(b->hit_offset_x == 1700 && b->hit_radius_min == 2600 && b->hit_radius_max == 1600, "D %s: Kasten %d/%d/%d", rdt,
          b->hit_offset_x, b->hit_radius_min, b->hit_radius_max);
    CHECK(re15_bb_klemme_r(b) == 1500, "M7 %s: Wandklemme r %d", rdt, re15_bb_klemme_r(b));
    {
        int32_t x0, x1, hz;
        re15_bb_schatten(&x0, &x1, &hz);
        CHECK(x0 <= -1000 && x1 >= 4000 && hz >= 1500, "D %s: Schatten %d..%d +-%d deckt Huefte bis Schnauze nicht", rdt, x0, x1, hz);
    }
    free(buf);
}

/* M6: klemmt die Wurzel im Weichen (Wand), bricht es ab, statt 8 s auf der Stelle zu laufen. Wand hier: die Lage wird nach jedem
 * Tick zurueckgesetzt (wie eine Wandklemme, die den Schub ganz nimmt). */
static void test_n1_weichen_klemme(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = form_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int f, weichen_bilder = 0;
    if (!b) return;
    beruhigen(b, pl, 30);
    stelle(b, pl, 1600);
    b->state = 1; b->sub_state_1 = 8; b->sub_state_2 = 0;
    for (f = 0; f < 120; f++) {
        const int32_t x0 = b->x, z0 = b->z;
        pl->x = b->x + 600; pl->z = b->z + 1500; pl->hit_react = 0;
        tick(1);
        b->x = x0; b->z = z0;                               /* Wand: Wurzel kommt nicht weg */
        if (b->sub_state_1 == 8) weichen_bilder++;
        if (re15_bk_opfer_cmd()) { re15_bk_opfer_reset(); re15_player_clear_hit_guard(pl); }
    }
    CHECK(weichen_bilder <= 30, "M6 %s: %d Bilder Weichen an der Wand (vorher Wechsel Weichen/Gehen 8 s)", rdt, weichen_bilder);
    free(buf);
}

/* M8a: Treffer-Blut am Kopf (Teil 22), ueber Leons Kopfhoehe, nicht an der Brust hinter ihm. */
static void test_n1_blut_kopf(unsigned raum, const char *rdt)
{
    uint8_t *buf; re15_actor_t *b = form_start(&buf, raum, rdt), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!b) return;
    beruhigen(b, pl, 30);
    stelle(b, pl, 7000);
    treffer(b, 3); tick(1);
    {
        int32_t p[3];
        re15_bb_blut_letzte(p);
        CHECK(p[1] < -1500, "M8a %s: Blut auf Hoehe %d (Kopf liegt 1600-2800 hoch)", rdt, -p[1]);
        CHECK(p[0] - b->x > 1800, "M8a %s: Blut %d vor der Wurzel (Kopf ab 1900)", rdt, p[0] - b->x);
    }
    free(buf);
}

/* Nachbesserung 3, N1 der AN-Abnahme 2: Wagenweg A -> B -> C. Die Wagentueren in 5090/5091 fuehren in denselben Raum
 * (Selbst-Tuer, scd_room_reenter; kein re15_enemy_reset), Sce_em_set legt die Form jedes Mal neu in Slot 2 an (+0x4 = 0
 * `sw zero,4(s0)` @0x800421e0, HP 0). Das Original ruft dann den G4-Ctor (Tabelle @0x8010af60[0] = 0x8010062c), der die HP
 * schreibt (`sh v0,342(s1)` @0x8010067c). Gemessen vor der Behebung (Lauf nb3/vorher_kette, exe 917195879): A 300 (EM30-INIT,
 * Bank noch nicht da), B 150, C 0. Raumladung wie die 6030-Tuer bzw. der Debug-Sprung (raum_laden: re15_enemy_reset), dann die
 * Tueren B (Schnitt 9) und C (Schnitt 14) wie im Lauf. Gegenprobe: im selben Raum setzt der Eintritt die HP nicht zurueck. */
static void test_n3_wagenweg(unsigned raum, const char *rdt)
{
    static const struct { int32_t x, z; uint8_t schnitt; const char *name; } k_tuer[] = {
        { 24700,  -9250,  9, "B" },                    /* DOOR FIRE slot 0 -> cut 9, spawn (24700,0,-9250) */
        { 24800, -23550, 14, "C" },                    /* DOOR FIRE slot 2 -> cut 14, spawn (24800,0,-23550) */
        { 24800, -23550, 14, "C2" },                   /* noch eine Selbst-Tuer: die Form ist wieder neu */
    };
    uint8_t *buf = raum_laden(rdt, raum);
    CHECK(buf != NULL, "Wagenweg: %s nicht ladbar", rdt);
    if (!buf) return;
    re15_actor_t *b = birkin_finden(), *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!b || !form_bank()) { CHECK(0, "Wagenweg %s: kein 0x30 / keine Bank", rdt); free(buf); return; }
    fprintf(stderr, "%s Wagenweg: A hp %d st %d/%d (Bank fehlte beim Spawn)\n", rdt, b->hp, b->state, b->sub_state_1);
    for (unsigned w = 0; w < sizeof k_tuer / sizeof k_tuer[0]; w++) {
        pl->x = k_tuer[w].x; pl->z = k_tuer[w].z;
        scd_room_reenter(&s_rdt, pl->x, pl->z, k_tuer[w].schnitt);
        for (int i = 0; i < 4; i++) scd_vm_tick();
        b = birkin_finden();
        CHECK(b != NULL, "Wagenweg %s Wagen %s: kein 0x30", rdt, k_tuer[w].name);
        if (!b) break;
        tick(2);                                       /* grid 0x33: Wurzel friert ein, die Form wartet */
        fprintf(stderr, "%s Wagenweg: %s hp %d st %d/%d grid 0x%02x\n", rdt, k_tuer[w].name, b->hp, b->state, b->sub_state_1, b->grid_id);
        CHECK(b->hp == RE15_BB_FORM_HP, "Wagenweg %s Wagen %s: Form mit HP %d statt %d (G4-Ctor @0x8010067c)", rdt, k_tuer[w].name,
              b->hp, RE15_BB_FORM_HP);
        CHECK(b->state == 1 && b->sub_state_1 == 0, "Wagenweg %s Wagen %s: Zustand %d/%d statt Auftritt 1/0 (+0x4 = 1 @0x80100654)",
              rdt, k_tuer[w].name, b->state, b->sub_state_1);
    }
    if (b) {   /* Gegenprobe im selben Raum: Freigabe (sub04 Member_set(0x0c,0x13) @0x130A), Treffer, weiter ticken */
        b->grid_id = 0x13; b->no_draw = 0;
        stelle(b, pl, 7000);
        tick(3);
        int hp0 = b->hp;
        CHECK(re15_enemy_take_damage(b, 5) == 1 && b->hp < hp0, "Wagenweg %s: Form nimmt keinen Treffer (hp %d)", rdt, b->hp);
        int hp1 = b->hp;
        for (int i = 0; i < 30; i++) { stelle(b, pl, 7000); tick(1); }
        CHECK(b->hp == hp1, "Wagenweg %s: HP im selben Raum von %d auf %d gesetzt (Eintritt ohne neue Anlage)", rdt, hp1, b->hp);
    }
    free(buf);
}

/* Stufe 3: grid 0x33 friert die Wurzel ein (@0x80116a7c-a88); die Riesenform zeigt dann den Stehen-Clip im Kreis. */
static void test_wartet(unsigned raum, const char *rdt)
{
    uint8_t *buf = raum_laden(rdt, raum);
    CHECK(buf != NULL, "Warten: %s nicht ladbar", rdt);
    if (!buf) return;
    re15_actor_t *b = birkin_finden();
    if (!b || !form_bank()) { CHECK(0, "Warten %s: kein 0x30 / keine Bank", rdt); free(buf); return; }
    b->grid_id = 0x33; b->state = 1; b->sub_state_1 = 9;
    tick(1);
    {
        int bild = b->anim_frame, x = b->x, z = b->z;
        tick(5);
        CHECK(b->motion == 2 && b->anim_frame != bild, "Warten %s: Clip %d Bild %d -> %d", rdt, b->motion, bild, b->anim_frame);
        CHECK(b->x == x && b->z == z && b->state == 1 && b->sub_state_1 == 9, "Warten %s: Zustand oder Lage geaendert", rdt);
    }
    free(buf);
}

int main(void)
{
    s_emd  = slurp(RE2_DIR "/BAHNSTEIG_FORM.EMD", &s_emd_n);
    s_tim  = slurp(RE2_DIR "/BAHNSTEIG_FORM.TIM", &s_tim_n);
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
    CHECK(s_emd && s_tim, "BAHNSTEIG_FORM.EMD/.TIM fehlt (analysis/befunde_runde41/form/stufe3/gen/s3_clips.py)");
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
        /* M1/M3/M4 (Tausch EM30 -> Form nach der Mutation in 5090) entfallen seit Stufe 3: die Riesenform ist ab der
         * Raumladung da, es gibt in 5090 keine Mutation mehr (re15_bf_mutation_offen = 0, STUFE3.md). */
        test_wartet(id, rdt);
        test_n3_wagenweg(id, rdt);
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
        test_n1_biss_kiefer(id, rdt);
        test_n1_gebruell(id, rdt);
        test_n1_koerper(id, rdt);
        test_n1_weichen_klemme(id, rdt);
        test_n1_blut_kopf(id, rdt);
        test_m15_griff_hinten(id, rdt);
    }
    test_flucht(0x5080u, "STAGE5/ROOM5080.RDT");
    test_flucht(0x5081u, "STAGE5/ROOM5081.RDT");
    test_wartet(0x5080u, "STAGE5/ROOM5080.RDT");
    if (fails) { fprintf(stderr, "test_birkin_bahnsteig: %d Fehler\n", fails); return 1; }
    fprintf(stderr, "test_birkin_bahnsteig: OK\n");
    return 0;
}
