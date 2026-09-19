/* ============================================================================================
 * G5-ENDKAMPF (RE2 EM036/EM037): 2-BONE-SKINNING, AUGEN-UV-WANDERER, KOPF-TRACKING,
 * BONE-WINKEL-HAKEN — Phase 2 des Dossiers analysis/befunde_2026-09-19/birkin-g5.md.
 * --------------------------------------------------------------------------------------------
 * Nutzer: "2. Sind weder die Augen, noch die Zaehne noch sonst irgendwas von ihm animiert."
 *
 * Was RE2 pro Bild am G5 animiert (Dossier 2.2, selbst nachdisassembliert):
 *   (b) Mesh 0 (Kopf+Rumpf, 199 V) haengt NICHT starr an Bone 0: der Per-Frame-Callback
 *       0x80103af4 ruft `jal 0x800197f4(entity, parts, 0x801059d8)` und `(..., 0x80105a44)`
 *       (@0x80103b70/@0x80103b84) = 2-Bone-Skinning: Kopf (30 V, w=4096) und Hals (34 V,
 *       weich) folgen Bone 1, der Rumpf (122 V, w=0) Bone 0, 13 Vertices der Arm-Pose
 *       (Part 3, off_b = (-800,-500,0) @0x80105a70). Ergebnis wird ins MD1-Vertexarray
 *       zurueckgeschrieben; Part 0 traegt Flag 0x4000 (@0x80100628) = Weltraum-Vertices.
 *   (c) Kopf-Tracking: FUN_80017FDC(e, yaw, &delta) @0x80100310, delta auf part1+0x6A
 *       (@0x80100344-354), Matrix, dann wieder abgezogen (@0x80100370). Ctor: +0x1C0 = 1
 *       (aus, @0x80100438), +0x1C1 = 1 (Kopf-Part, @0x8010043c), part1+0x9C = 8 (Schritt,
 *       @0x80100440/50), part1+0xA0 = 212 (Klemme, @0x80100458/5c), part1+0x98 = 0 (Akku,
 *       @0x80100468). Ein: `sb zero,448` @0x801017d4 (Intro-Ende -> sub1); Tod: `|= 2`
 *       @0x801030f0 (Ziel = Keyframe). Spieler-Zielpart: +0x1C1 = 8 (Kopf) @0x8003c268.
 *   (d) Augen = UV-Scroll zweier Primitivlisten von Mesh 2: Wanderer 0x80105064 (beide je
 *       Bild @0x801002bc/c8), Ziele 0x8010503C mit 15 - rng%15 bei (+0x14D & 0xF) == 0 /
 *       == 7 (@0x801001b0-2b0), im Tod (0,0) fuer beide (@0x80103478/88).
 * Die Tentakel (EM037) nutzen dasselbe Skinning fuer ihren Schlauch (drei Nahttabellen,
 * Callback 0x80104CF0) und die Streckung um den Part-0-Rahmen (FUN_80019CD0, Flag 0x400).
 * Tabellen: engine/src/gen/g5_skin_tables.inc (tools/gen_g5_skin_tables.py, byte-geprueft).
 * ========================================================================================== */
#include <stdint.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_g5_skin.h"
#include "re15_md1.h"
#include "re15_skeleton.h"

#include "gen/g5_skin_tables.inc"

/* ---- Skinning-Kern (FUN_800197f4) ------------------------------------------------------ */

#define G5_SKIN_MAX_V 256

static re15_md1_vertex_t s_skin_v[G5_SKIN_MAX_V];
static re15_md1_vertex_t s_skin_n[G5_SKIN_MAX_V];

static int16_t sat16(int32_t v)
{
    if (v >  32767) return  32767;
    if (v < -32768) return -32768;
    return (int16_t)v;
}

/* R*v >> 12 (gte_rtv0 ohne Translation), s16-gesaettigt wie IR1..3. */
static void rot_q12(const int32_t R[9], int32_t x, int32_t y, int32_t z, int32_t out[3])
{
    out[0] = (int32_t)(((int64_t)R[0]*x + (int64_t)R[1]*y + (int64_t)R[2]*z) >> 12);
    out[1] = (int32_t)(((int64_t)R[3]*x + (int64_t)R[4]*y + (int64_t)R[5]*z) >> 12);
    out[2] = (int32_t)(((int64_t)R[6]*x + (int64_t)R[7]*y + (int64_t)R[8]*z) >> 12);
    out[0] = sat16(out[0]); out[1] = sat16(out[1]); out[2] = sat16(out[2]);
}

/* Eine Tabelle anwenden: Puffer A/B wie FUN_80019628 (v+off_a / v+off_b), dann die
 * Rotations-/Translations-Passe und die gewichtete Mischung wie FUN_800197f4. */
static void skin_apply(const re15_g5_skin_tab_t *tab,
                       const re15_skel_pose_t *pa, const re15_skel_pose_t *pb,
                       const re15_md1_mesh_t *m, int nv, int nn)
{
    int i;
    for (i = 0; i < (int)tab->count; i++) {
        int v = tab->pairs[i][0];
        int32_t w = tab->pairs[i][1];
        int32_t A[3], B[3], NA[3], NB[3];
        const re15_md1_vertex_t *bv, *bn = 0;
        if (v >= nv) continue;
        bv = &m->tri_vertices[v];
        if (v < nn) bn = &m->tri_normals[v];
        /* Pass 1 (part_a): rtv0(v+off_a) + Welt-Translation, Normale rtv0(n). */
        rot_q12(pa->rot, (int16_t)(bv->x + tab->off_a[0]), (int16_t)(bv->y + tab->off_a[1]),
                (int16_t)(bv->z + tab->off_a[2]), A);
        A[0] = (int16_t)(A[0] + (int16_t)pa->trans[0]);      /* pSVar10->vx += (short)uVar18 */
        A[1] = (int16_t)(A[1] + (int16_t)pa->trans[1]);
        A[2] = (int16_t)(A[2] + (int16_t)pa->trans[2]);
        /* Pass 2 (part_b). */
        rot_q12(pb->rot, (int16_t)(bv->x + tab->off_b[0]), (int16_t)(bv->y + tab->off_b[1]),
                (int16_t)(bv->z + tab->off_b[2]), B);
        B[0] = (int16_t)(B[0] + (int16_t)pb->trans[0]);
        B[1] = (int16_t)(B[1] + (int16_t)pb->trans[1]);
        B[2] = (int16_t)(B[2] + (int16_t)pb->trans[2]);
        if (bn) { rot_q12(pa->rot, bn->x, bn->y, bn->z, NA); rot_q12(pb->rot, bn->x, bn->y, bn->z, NB); }
        /* Mischung @0x800199c8-a90: w == 0x1000 -> A, w == 0 -> B, sonst gpf12/gpl12. */
        if (w == 0x1000) {
            s_skin_v[v].x = (int16_t)A[0]; s_skin_v[v].y = (int16_t)A[1]; s_skin_v[v].z = (int16_t)A[2];
            if (bn) { s_skin_n[v].x = (int16_t)NA[0]; s_skin_n[v].y = (int16_t)NA[1]; s_skin_n[v].z = (int16_t)NA[2]; }
        } else if (w == 0) {
            s_skin_v[v].x = (int16_t)B[0]; s_skin_v[v].y = (int16_t)B[1]; s_skin_v[v].z = (int16_t)B[2];
            if (bn) { s_skin_n[v].x = (int16_t)NB[0]; s_skin_n[v].y = (int16_t)NB[1]; s_skin_n[v].z = (int16_t)NB[2]; }
        } else {
            int32_t wb = 0x1000 - w;
            s_skin_v[v].x = sat16((w*A[0] + wb*B[0]) >> 12);
            s_skin_v[v].y = sat16((w*A[1] + wb*B[1]) >> 12);
            s_skin_v[v].z = sat16((w*A[2] + wb*B[2]) >> 12);
            if (bn) {
                s_skin_n[v].x = sat16((w*NA[0] + wb*NB[0]) >> 12);
                s_skin_n[v].y = sat16((w*NA[1] + wb*NB[1]) >> 12);
                s_skin_n[v].z = sat16((w*NA[2] + wb*NB[2]) >> 12);
            }
        }
    }
}

static int skin_prepare(const re15_md1_mesh_t *m, int *nv, int *nn)
{
    int v;
    *nv = m->tri_vertex_count; *nn = m->tri_normal_count;
    if (*nv <= 0 || *nv > G5_SKIN_MAX_V || *nn > G5_SKIN_MAX_V) return -1;
    for (v = 0; v < *nv; v++) s_skin_v[v] = m->tri_vertices[v];
    for (v = 0; v < *nn; v++) s_skin_n[v] = m->tri_normals[v];
    return 0;
}

int re15_g5_skin_mesh0(const re15_skel_pose_t poses[], int npose,
                       const re15_md1_mesh_t *m0,
                       const re15_md1_vertex_t **out_v, const re15_md1_vertex_t **out_n)
{
    int nv, nn;
    re15_skel_pose_t arm;
    if (!poses || npose < 2 || !m0 || !m0->tri_vertices) return -1;
    if (skin_prepare(m0, &nv, &nn) != 0) return -1;
    /* Part 3 (Arm) = Kind von Part 0 mit Bind (800,500,0) (@0x801006ac-c4): R3 = R0,
     * t3 = t0 + R0*(800,500,0). */
    {
        int32_t b[3] = { 800, 500, 0 }, r[3];
        arm = poses[0];
        r[0] = (int32_t)(((int64_t)arm.rot[0]*b[0] + (int64_t)arm.rot[1]*b[1] + (int64_t)arm.rot[2]*b[2]) >> 12);
        r[1] = (int32_t)(((int64_t)arm.rot[3]*b[0] + (int64_t)arm.rot[4]*b[1] + (int64_t)arm.rot[5]*b[2]) >> 12);
        r[2] = (int32_t)(((int64_t)arm.rot[6]*b[0] + (int64_t)arm.rot[7]*b[1] + (int64_t)arm.rot[8]*b[2]) >> 12);
        arm.trans[0] += r[0]; arm.trans[1] += r[1]; arm.trans[2] += r[2];
    }
    skin_apply(&s_g5_skin_tabs[0], &poses[1], &poses[0], m0, nv, nn);   /* @0x80103b70 */
    skin_apply(&s_g5_skin_tabs[1], &poses[1], &arm,      m0, nv, nn);   /* @0x80103b84 */
    *out_v = s_skin_v; *out_n = s_skin_n;
    return 0;
}

void re15_g5_tentakel_stretch_point(const re15_skel_pose_t *p0, int32_t scale_x_q12, int32_t p[3])
{
    /* FUN_80019CD0 Z.33-55: W0*S*W0^T*(v - t0) + t0, S = (scaleX, 4096, 4096). */
    const int32_t *R = p0->rot;
    int32_t d[3], l[3];
    d[0] = p[0] - p0->trans[0]; d[1] = p[1] - p0->trans[1]; d[2] = p[2] - p0->trans[2];
    l[0] = (int32_t)(((int64_t)R[0]*d[0] + (int64_t)R[3]*d[1] + (int64_t)R[6]*d[2]) >> 12);  /* R^T */
    l[1] = (int32_t)(((int64_t)R[1]*d[0] + (int64_t)R[4]*d[1] + (int64_t)R[7]*d[2]) >> 12);
    l[2] = (int32_t)(((int64_t)R[2]*d[0] + (int64_t)R[5]*d[1] + (int64_t)R[8]*d[2]) >> 12);
    l[0] = (int32_t)(((int64_t)l[0] * scale_x_q12) >> 12);
    p[0] = (int32_t)(((int64_t)R[0]*l[0] + (int64_t)R[1]*l[1] + (int64_t)R[2]*l[2]) >> 12) + p0->trans[0];
    p[1] = (int32_t)(((int64_t)R[3]*l[0] + (int64_t)R[4]*l[1] + (int64_t)R[5]*l[2]) >> 12) + p0->trans[1];
    p[2] = (int32_t)(((int64_t)R[6]*l[0] + (int64_t)R[7]*l[1] + (int64_t)R[8]*l[2]) >> 12) + p0->trans[2];
}

int re15_g5_tentakel_skin_mesh0(const re15_skel_pose_t poses[], int npose,
                                const re15_md1_mesh_t *m0, int32_t scale_x_q12,
                                const re15_md1_vertex_t **out_v,
                                const re15_md1_vertex_t **out_n)
{
    int nv, nn, v;
    if (!poses || npose < 4 || !m0 || !m0->tri_vertices) return -1;
    if (skin_prepare(m0, &nv, &nn) != 0) return -1;
    /* Callback 0x80104CF0: drei Naehte in Kettenreihenfolge (@0x80104d10/24/38). */
    skin_apply(&s_g5_tent_skin_tabs[0], &poses[0], &poses[1], m0, nv, nn);
    skin_apply(&s_g5_tent_skin_tabs[1], &poses[1], &poses[2], m0, nv, nn);
    skin_apply(&s_g5_tent_skin_tabs[2], &poses[2], &poses[3], m0, nv, nn);
    /* Dann FUN_80019CD0 (@0x80104d44): Streckung um den Part-0-Rahmen (nur Vertices —
     * die Normalen stehen im Original hinter derselben Matrix, S ist fuer sie eine
     * Verzerrung, die der Zeichner nicht nachnormalisiert; hier unveraendert). */
    if (scale_x_q12 != 0x1000) {
        for (v = 0; v < nv; v++) {
            int32_t p[3] = { s_skin_v[v].x, s_skin_v[v].y, s_skin_v[v].z };
            re15_g5_tentakel_stretch_point(&poses[0], scale_x_q12, p);
            s_skin_v[v].x = sat16(p[0]); s_skin_v[v].y = sat16(p[1]); s_skin_v[v].z = sat16(p[2]);
        }
    }
    *out_v = s_skin_v; *out_n = s_skin_n;
    return 0;
}

/* ---- Augen-UV-Wanderer (0x80105064) ---------------------------------------------------- */

typedef struct {                /* Record @0x80105610 + 28*idx */
    int8_t  v_pos, u_pos;       /* +0/+1  */
    int8_t  v_ziel, u_ziel;     /* +2/+3  */
    int16_t v_vel, u_vel;       /* +4/+6  */
    int16_t v_step, u_step;     /* +8/+A  */
} g5_eye_t;

static g5_eye_t s_eye[2];

void re15_g5_eye_reset(void)      /* Ctor @0x801004a4-51c: alle Felder 0 */
{
    memset(s_eye, 0, sizeof s_eye);
}

void re15_g5_eye_set_target(int idx, int v_ziel, int u_ziel)   /* 0x8010503C */
{
    if (idx < 0 || idx > 1) return;
    s_eye[idx].v_ziel = (int8_t)v_ziel;
    s_eye[idx].u_ziel = (int8_t)u_ziel;
}

static int8_t clamp8(int v, int8_t max, int8_t min)
{
    if (max < v) v = max;      /* @0x80105284-98 (lb 21: max, slt) */
    if (v < min) v = min;      /* @0x8010529c-b4 (lb 22: min)      */
    return (int8_t)v;
}

void re15_g5_eye_tick(int idx)
{
    g5_eye_t *r;
    const re15_g5_eye_lists_t *L;
    int16_t old_vstep, old_ustep;
    if (idx < 0 || idx > 1) return;
    r = &s_eye[idx]; L = &s_g5_eye_lists[idx];
    old_vstep = r->v_step; old_ustep = r->u_step;             /* lhu t0,8 / lhu a3,10 */
    /* Schrittrichtung = sign(ziel - pos), je Achse (@0x801050cc-138). */
    if (r->v_ziel < r->v_pos) r->v_step = -1;
    if (r->v_pos < r->v_ziel) r->v_step =  1;
    if (r->u_ziel < r->u_pos) r->u_step = -1;
    if (r->u_pos < r->u_ziel) r->u_step =  1;
    /* Richtungswechsel -> vel = vel*2/3 (mult 0x55555556, Rundung gegen 0 @0x8010513c-70). */
    if (-r->v_step == old_vstep) r->v_vel = (int16_t)((r->v_vel * 2) / 3);
    if (-r->u_step == old_ustep) r->u_vel = (int16_t)((r->u_vel * 2) / 3);
    /* Einrasten: pos == ziel && |vel| < 3 (@0x801051ac-228). */
    if (r->v_pos == r->v_ziel) {
        int a = r->v_vel < 0 ? -r->v_vel : r->v_vel;
        if (a < 3) { r->v_pos = r->v_ziel; r->v_step = 0; r->v_vel = 0; }
    }
    if (r->u_pos == r->u_ziel) {
        int a = r->u_vel < 0 ? -r->u_vel : r->u_vel;
        if (a < 3) { r->u_pos = r->u_ziel; r->u_step = 0; r->u_vel = 0; }
    }
    /* vel += step; pos += (u8)vel; klemmen (@0x8010524c-2ec). */
    r->v_vel = (int16_t)(r->v_vel + r->v_step);
    r->u_vel = (int16_t)(r->u_vel + r->u_step);
    r->u_pos = (int8_t)(uint8_t)((uint8_t)r->u_pos + (uint8_t)r->u_vel);
    r->v_pos = (int8_t)(uint8_t)((uint8_t)r->v_pos + (uint8_t)r->v_vel);
    r->v_pos = clamp8(r->v_pos, L->v_max, L->v_min);
    r->u_pos = clamp8(r->u_pos, L->u_max, L->u_min);
    /* Das Delta (pos - alt) geht im Original AKKUMULIEREND in die GT3/GT4-Pakete
     * (FUN_800171d0/2f8); die Summe der Deltas ist pos selbst (Start 0) -> der Zeichner
     * liest den Stand ueber re15_g5_eye_uv_offset. */
}

int re15_g5_eye_uv_offset(int mesh, int is_quad, int prim_index, int *du, int *dv)
{
    int e, i;
    for (e = 0; e < 2; e++) {
        const re15_g5_eye_lists_t *L = &s_g5_eye_lists[e];
        if (mesh != (int)L->part) continue;
        if (is_quad) {
            for (i = 0; i < (int)L->nquads; i++)
                if ((int)L->quads[i] == prim_index) { *du = s_eye[e].u_pos; *dv = s_eye[e].v_pos; return 1; }
        } else {
            for (i = 0; i < (int)L->ntris; i++)
                if ((int)L->tris[i] == prim_index) { *du = s_eye[e].u_pos; *dv = s_eye[e].v_pos; return 1; }
        }
    }
    return 0;
}

void re15_g5_eye_get(int idx, int *v_pos, int *u_pos)
{
    if (idx < 0 || idx > 1) { *v_pos = *u_pos = 0; return; }
    *v_pos = s_eye[idx].v_pos; *u_pos = s_eye[idx].u_pos;
}

/* ---- Kopf-Tracking (FUN_80017FDC) ------------------------------------------------------ */

void re15_g5_track_init(re15_g5_track_t *t)
{
    t->akku = 0;        /* sh zero,324(v1)  @0x80100468 */
    t->step = 8;        /* sh v0=8,328(v1)  @0x80100450 */
    t->limit = 212;     /* sh v0=212,332(v1) @0x8010045c */
}

int16_t re15_g5_track_tick(re15_g5_track_t *t, int fwd, int16_t e_yaw, int16_t root_kf_yaw,
                           int16_t head_kf_yaw, int32_t hx, int32_t hz,
                           int32_t tx, int32_t tz)
{
    /* 0x8001820c: Peilung Kopf-Part -> Ziel-Part in der 0=+X-Konvention:
     * yaw = (4096 - ratan((dz<<12)/dx) - (dx<0 ? 0x800 : 0)) & 0xfff (@0x800182d0-e8);
     * dz != 0, dx == 0 -> 3072/1024 (@0x8001827c-90). re15_atan2_q12 ist derselbe catan-
     * Aufbau (RE1.5 FUN_8001a6d4) mit +0x400-Versatz -> zurueckgerechnet. */
    int32_t dx = tx - hx, dz = tz - hz;
    int32_t desired;
    int32_t yaw = (int16_t)e_yaw;                       /* s2 (lh)            */
    int32_t root = (uint16_t)root_kf_yaw;               /* s5 (lhu part0+0x6A) */
    int32_t head = (uint16_t)head_kf_yaw;               /* lhu part1+0x6A      */
    int32_t s4 = yaw + root;
    int32_t limit = (int16_t)t->limit, step = (int16_t)t->step;
    int32_t rel, cur, base, delta, akku;
    if (dz == 0 && dx == 0) desired = (uint16_t)(s4 + head);  /* Original laesst 18(sp) stehen;
                                                                * Port: kein Drehen */
    else desired = ((int32_t)re15_atan2_q12(dz, dx) - 0x400) & 0xfff;
    /* +0x1C0 Bit 1 (`andi 0x2` @0x800180c0): Ziel = Keyframe-Blickrichtung
     * (`sh v0,18(sp)` @0x800180dc: yaw + part0.6A + part1.6A) -> der Akku laeuft auf 0. */
    if (fwd) desired = (uint16_t)(s4 + head);
    /* Klemme auf +-limit um (yaw + root) (@0x80018118-160). */
    rel = (int16_t)desired - (int16_t)yaw - (int16_t)root;
    if (((rel + limit) & 0xfff) > limit * 2)
        desired = (uint16_t)((rel & 0x800) ? (s4 - limit) : (limit + s4));
    /* Slew um step, Snap bei |delta| < step (@0x80018164-1cc). */
    cur  = (uint16_t)t->akku;
    base = yaw + head + root;
    delta = (int32_t)(uint16_t)desired - (cur + base);
    if (((step + delta) & 0x800) != 0) akku = cur - step;
    else                               akku = cur + step;
    if (((step + delta) & 0xfff) < step * 2)
        akku = (int32_t)(uint16_t)desired - base;
    t->akku = (int16_t)(uint16_t)akku;                  /* sh v1,152(s0) */
    /* Aufrufer @0x80100318-34: andi 0xfff; < 2049 ? v : v - 4095. */
    {
        int32_t d = (uint16_t)t->akku & 0xfff;
        if (d >= 2049) d -= 4095;
        return (int16_t)d;
    }
}

/* ---- Bone-Winkel-Zuschlaege je Aktor-Slot ---------------------------------------------- */

static struct {
    int16_t ay[RE15_EMD_MAX_BONES];
    int16_t az[RE15_EMD_MAX_BONES];
} s_bone_add[RE15_ACTOR_MAX];

void re15_g5_bone_add_set(int slot, int bone, int16_t ay, int16_t az)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX || bone < 0 || bone >= RE15_EMD_MAX_BONES) return;
    s_bone_add[slot].ay[bone] = ay;
    s_bone_add[slot].az[bone] = az;
}

void re15_g5_bone_add_clear(int slot)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    memset(&s_bone_add[slot], 0, sizeof s_bone_add[slot]);
}

void re15_g5_bone_angle_hook(const void *actor, int bone, int16_t *ay, int16_t *az)
{
    const re15_actor_t *a = (const re15_actor_t *)actor;
    int slot;
    if (!a || (a->type != 0x36u && a->type != 0x37u)) return;
    slot = (int)(a - g_actors);
    if (slot < 0 || slot >= RE15_ACTOR_MAX || bone < 0 || bone >= RE15_EMD_MAX_BONES) return;
    *ay = (int16_t)(*ay + s_bone_add[slot].ay[bone]);
    *az = (int16_t)(*az + s_bone_add[slot].az[bone]);
}
