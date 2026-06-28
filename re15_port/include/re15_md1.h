/*
 * RE1.5 Rebuilt — MD1 mesh parser API (Phase 4.5, 2026-05-18).
 *
 * MD1 = Capcom's PSX mesh format. Mirrors lib_bio's lib_md1.py reference.
 * The Java parser at `de.re15.extractors.md1.MD1File` is the authoritative
 * format reference; this C parser is a straight port for the new engine.
 *
 * File layout:
 *   [0x00] u32 length
 *   [0x04] u32 unknown
 *   [0x08] u32 object_count   (mesh_count = object_count / 2)
 *
 *   Per mesh i (header at offset 12 + i*56, 56 bytes / 14 u32):
 *     +0  t_vertex_offset    +4  t_vertex_count
 *     +8  t_normal_offset    +12 t_normal_count
 *     +16 t_face_offset      +20 t_face_count
 *     +24 t_uv_offset
 *     +28 q_vertex_offset    +32 q_vertex_count
 *     +36 q_normal_offset    +40 q_normal_count
 *     +44 q_face_offset      +48 q_face_count
 *     +52 q_uv_offset
 *
 *   All block offsets are absolute from (file_start + 12).
 *
 *   Vertex (8B): s16 x, s16 y, s16 z, s16 pad
 *   Normal (8B): s16 x, s16 y, s16 z, s16 pad
 *   Triangle (12B): u16 n0, v0, n1, v1, n2, v2
 *   Quad (16B):     u16 n0, v0, n1, v1, n2, v2, n3, v3
 *   TriangleUV (12B): u8 u0, v0; u16 clut; u8 u1, v1; u16 page; u8 u2, v2
 *   QuadUV (16B):     u8 u0, v0; u16 clut; u8 u1, v1; u16 page; u8 u2, v2, u3, v3
 *
 * The parser does NOT copy mesh data — out-pointers reference the source
 * buffer directly. Caller must keep the source alive while using the
 * parsed result.
 */
#ifndef RE15_MD1_H
#define RE15_MD1_H

#include <stdint.h>

#define MD1_MAX_MESHES 32     /* hard cap for static allocation */

typedef struct {
    int16_t x, y, z, pad;
} re15_md1_vertex_t;

typedef struct {
    uint16_t n0, v0, n1, v1, n2, v2;
} re15_md1_triangle_t;

typedef struct {
    uint16_t n0, v0, n1, v1, n2, v2, n3, v3;
} re15_md1_quad_t;

/* IMPORTANT: these structs must match the on-disk byte layout EXACTLY
 * because the parser hands them out as pointers into the source buffer
 * (no copying / no per-field reads). The padding bytes the file contains
 * are explicitly declared so sizeof(struct) equals the file stride —
 * otherwise array indexing `uvs[i]` reads from wrong offsets.
 *
 * Cross-compiler note: we don't use __attribute__((packed)) because MSVC
 * doesn't accept it. Explicit padding members with u16 (natural 2-byte
 * alignment) produce identical layout on gcc, clang, and MSVC. */
typedef struct {
    uint8_t  u0, v0;       /* +0..+1 */
    uint16_t clut;         /* +2..+3 */
    uint8_t  u1, v1;       /* +4..+5 */
    uint16_t page;         /* +6..+7 */
    uint8_t  u2, v2;       /* +8..+9 */
    uint16_t _pad_a;       /* +10..+11 — present in the file, ignored */
} re15_md1_tri_uv_t;       /* total = 12 bytes (file stride) */

typedef struct {
    uint8_t  u0, v0;       /* +0..+1 */
    uint16_t clut;         /* +2..+3 */
    uint8_t  u1, v1;       /* +4..+5 */
    uint16_t page;         /* +6..+7 */
    uint8_t  u2, v2;       /* +8..+9 */
    uint16_t _pad_a;       /* +10..+11 — present in the file */
    uint8_t  u3, v3;       /* +12..+13 */
    uint16_t _pad_b;       /* +14..+15 — present in the file */
} re15_md1_quad_uv_t;       /* total = 16 bytes (file stride) */

typedef struct {
    /* Triangle section */
    const re15_md1_vertex_t   *tri_vertices;
    int                        tri_vertex_count;
    const re15_md1_vertex_t   *tri_normals;
    int                        tri_normal_count;
    const re15_md1_triangle_t *triangles;
    int                        triangle_count;
    const re15_md1_tri_uv_t   *triangle_uvs;
    /* Quad section */
    const re15_md1_vertex_t   *quad_vertices;
    int                        quad_vertex_count;
    const re15_md1_vertex_t   *quad_normals;
    int                        quad_normal_count;
    const re15_md1_quad_t     *quads;
    int                        quad_count;
    const re15_md1_quad_uv_t  *quad_uvs;
} re15_md1_mesh_t;

typedef struct {
    /* Source buffer (caller owned). */
    const uint8_t *raw;
    int            raw_size;

    /* Header */
    uint32_t length;
    uint32_t unknown;
    uint32_t object_count;
    int      mesh_count;       /* = object_count / 2, clamped to MD1_MAX_MESHES */

    re15_md1_mesh_t meshes[MD1_MAX_MESHES];
} re15_md1_t;

/* Parse an MD1 blob in memory. Returns 0 on success, -1 on truncation /
 * sanity error. Fields in `out` reference into `data`. */
int re15_md1_parse(const uint8_t *data, int size, re15_md1_t *out);

#endif
