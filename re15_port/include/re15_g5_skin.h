/*
 * RE1.5 Rebuilt — G5-Endkampf (RE2 EM036/EM037): 2-Bone-Skinning, Augen-UV-Wanderer,
 * Kopf-Tracking, Bone-Winkel-Haken. Phase 2 des Dossiers
 * analysis/befunde_2026-09-19/birkin-g5.md (+ birkin-g5.skeptiker.md).
 *
 * Alles hier ist ein Port von RE2-Retail-Code (PSX.EXE Leon + CDEMD0.EMS-Overlays,
 * gelinkt @0x80100000); jede Konstante traegt ihre @0x-Adresse im Modul.
 */
#ifndef RE15_G5_SKIN_H
#define RE15_G5_SKIN_H

#include <stdint.h>
#include "re15_md1.h"
#include "re15_skeleton.h"

/* 2-Bone-Skinning-Tabelle (FUN_80019628 / FUN_800197f4; Format Dossier 2.2b):
 * Vertex v der Paarliste wird zu  w*(R_a*(v+off_a)+t_a) + (4096-w)*(R_b*(v+off_b)+t_b) >> 12
 * (w == 0x1000 -> nur a, w == 0 -> nur b; GTE gpf12/gpl12 @0x80019a58-a90). */
typedef struct {
    uint8_t  part_a, part_b;
    uint16_t count;
    int16_t  off_a[3], off_b[3];
    const uint16_t (*pairs)[2];      /* {vertex, weight} */
} re15_g5_skin_tab_t;

/* Augen-Primitivlisten eines Wanderer-Records (Dossier 2.2d; Records @0x80105610/0x8010562C). */
typedef struct {
    const uint8_t *tris;  uint8_t ntris;
    const uint8_t *quads; uint8_t nquads;
    uint8_t part;                       /* +0x14 = 2 (die Masse) */
    int8_t  v_max, v_min, u_max, u_min; /* +0x15..+0x18 = +15/-15/+15/-15 */
} re15_g5_eye_lists_t;

/* ---- Skinning (Zeichner) ------------------------------------------------------------
 * Ergebnis liegt im MODELLRAUM des Aktors (Bone-Posen wie re15_skel_compute_pose), also
 * das "Weltraum-Vertexarray" des Originals OHNE Entity-Transformation: der Zeichner
 * transformiert es nur noch mit der Entity-Matrix (Part-Flag 0x4000 @0x80100628: keine
 * Part-Matrix). out_v: alle Mesh-Vertices (ungelistete = Basis), out_n: alle Normalen
 * (gelistete Indizes rotiert/gemischt wie im Original, `piVar16[2]` @0x80019a04-a10).
 * Rueckgabe 0 ok / -1 (kein Puffer, Mesh zu gross). */
int re15_g5_skin_mesh0(const re15_skel_pose_t poses[], int npose,
                       const re15_md1_mesh_t *m0,
                       const re15_md1_vertex_t **out_v, const re15_md1_vertex_t **out_n);

/* Tentakel (EM037) Mesh 0: drei Nahttabellen (@0x801058b8/0x80105940/0x801059b8) auf die
 * Kettenposen 0..3, danach die Streckung um den Part-0-Rahmen entlang lokal X
 * (FUN_80019CD0, Flag 0x400: W0*S*W0^T*(v-t0)+t0; scale_x_q12 = part0+0x8C). */
int re15_g5_tentakel_skin_mesh0(const re15_skel_pose_t poses[], int npose,
                                const re15_md1_mesh_t *m0, int32_t scale_x_q12,
                                const re15_md1_vertex_t **out_v,
                                const re15_md1_vertex_t **out_n);

/* Streckung eines beliebigen Modellraum-Punkts um den Part-0-Rahmen (fuer die Stummel-
 * Meshes 1..3 des Tentakels, damit sie an den gestreckten Schlauch-Enden bleiben). */
void re15_g5_tentakel_stretch_point(const re15_skel_pose_t *pose0, int32_t scale_x_q12,
                                    int32_t p[3]);

/* ---- Augen-UV-Wanderer (0x80105064, Ziele 0x8010503C) -------------------------------- */
void re15_g5_eye_reset(void);
void re15_g5_eye_set_target(int idx, int v_ziel, int u_ziel);   /* 0x8010503C */
void re15_g5_eye_tick(int idx);                                  /* 0x80105064 */
/* UV-Versatz (u,v) fuer ein Primitiv von Mesh `mesh` (Tri-Index bzw. Quad-Index); 0 = keins. */
int  re15_g5_eye_uv_offset(int mesh, int is_quad, int prim_index, int *du, int *dv);
/* Diagnose: aktueller Wanderer-Stand (v_pos, u_pos). */
void re15_g5_eye_get(int idx, int *v_pos, int *u_pos);

/* ---- Kopf-Tracking (FUN_80017FDC, Aufruf @0x80100310) -------------------------------- */
typedef struct {
    int16_t akku;       /* part1+0x98: der nachgefuehrte Kopf-Yaw (Ctor 0 @0x80100468) */
    int16_t step;       /* part1+0x9C: 8  @0x80100440/50                                 */
    int16_t limit;      /* part1+0xA0: 212 @0x80100458/5c                                */
} re15_g5_track_t;
void re15_g5_track_init(re15_g5_track_t *t);
/* Ein Bild: e_yaw = Entity-Yaw (+0x76), root_kf_yaw = part0+0x6A, head_kf_yaw = part1+0x6A,
 * (hx,hz) = Kopf-Part-Welt-XZ (part1+0x5C/+0x64), (tx,tz) = Ziel-Part-Welt-XZ.
 * Rueckgabe = der Yaw-Zuschlag fuer part1+0x6A (@0x80100344-354). */
int16_t re15_g5_track_tick(re15_g5_track_t *t, int fwd, int16_t e_yaw, int16_t root_kf_yaw,
                           int16_t head_kf_yaw, int32_t hx, int32_t hz,
                           int32_t tx, int32_t tz);   /* fwd = +0x1C0 Bit 1 (Ziel = Keyframe) */

/* ---- Bone-Winkel-Haken fuer den Pose-Builder ----------------------------------------
 * Das Original addiert Zuschlaege DIREKT auf die Part-Winkel (+0x68/+0x6A/+0x6C), bevor
 * RotMatrix die Part-Matrix baut (G5 Kopf @0x80100354; Tentakel-Einrollwinkel sub9 ph5/7,
 * sub11 ph5/7 auf +0x6C je Part). re15_skel_compute_pose ruft diesen Haken je Bone
 * unmittelbar vor mat3_from_euler; die G5-Module tragen sich hier ein. */
void re15_g5_bone_angle_hook(const void *actor, int bone, int16_t *ay, int16_t *az);

/* Zuschlaege je Aktor-Slot (vom G5-/Tentakel-Tick gesetzt, vom Haken gelesen). */
void re15_g5_bone_add_set(int slot, int bone, int16_t ay, int16_t az);
void re15_g5_bone_add_clear(int slot);

#endif /* RE15_G5_SKIN_H */
