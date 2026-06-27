/*
 * RE1.5 Rebuilt — MD1 parser (Phase 4.5, 2026-05-18).
 *
 * Platform-agnostic. Mirrors de.re15.extractors.md1.MD1File.java semantics.
 *
 * Parser is non-allocating: out-pointers reference into the source buffer.
 */
#include <stddef.h>
#include "re15_md1.h"

static uint16_t rd16(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t rd32(const uint8_t *p)
{
    return (uint32_t)p[0]
         | ((uint32_t)p[1] << 8)
         | ((uint32_t)p[2] << 16)
         | ((uint32_t)p[3] << 24);
}

#define HEADER_SIZE       12
#define MESH_HEADER_SIZE  56
#define VERTEX_STRIDE      8
#define NORMAL_STRIDE      8
#define TRI_STRIDE        12
#define QUAD_STRIDE       16
#define TRI_UV_STRIDE     12
#define QUAD_UV_STRIDE    16

/* Bounds check helper. Returns 0 if [start, start+size) is inside data. */
static int in_range(int data_size, int start, int size)
{
    if (start < 0 || size < 0) return 0;
    long end = (long)start + (long)size;
    return end <= (long)data_size;
}

int re15_md1_parse(const uint8_t *data, int size, re15_md1_t *out)
{
    if (!data || !out || size < HEADER_SIZE) return -1;

    out->raw = data;
    out->raw_size = size;
    out->length       = rd32(data + 0);
    out->unknown      = rd32(data + 4);
    out->object_count = rd32(data + 8);

    int mesh_count = (int)(out->object_count / 2);
    if (mesh_count < 0) mesh_count = 0;
    if (mesh_count > MD1_MAX_MESHES) mesh_count = MD1_MAX_MESHES;
    out->mesh_count = mesh_count;

    const int base = HEADER_SIZE;   /* mesh-relative offsets start here */

    for (int i = 0; i < mesh_count; i++) {
        int hdr = HEADER_SIZE + i * MESH_HEADER_SIZE;
        if (!in_range(size, hdr, MESH_HEADER_SIZE)) return -1;

        const uint8_t *h = data + hdr;
        uint32_t tv_off = rd32(h +  0), tv_cnt = rd32(h +  4);
        uint32_t tn_off = rd32(h +  8), tn_cnt = rd32(h + 12);
        uint32_t tf_off = rd32(h + 16), tf_cnt = rd32(h + 20);
        uint32_t tu_off = rd32(h + 24);
        uint32_t qv_off = rd32(h + 28), qv_cnt = rd32(h + 32);
        uint32_t qn_off = rd32(h + 36), qn_cnt = rd32(h + 40);
        uint32_t qf_off = rd32(h + 44), qf_cnt = rd32(h + 48);
        uint32_t qu_off = rd32(h + 52);

        re15_md1_mesh_t *m = &out->meshes[i];

        /* Initialize all to safe defaults */
        m->tri_vertices = NULL; m->tri_vertex_count = 0;
        m->tri_normals  = NULL; m->tri_normal_count = 0;
        m->triangles    = NULL; m->triangle_count   = 0;
        m->triangle_uvs = NULL;
        m->quad_vertices = NULL; m->quad_vertex_count = 0;
        m->quad_normals  = NULL; m->quad_normal_count = 0;
        m->quads         = NULL; m->quad_count        = 0;
        m->quad_uvs      = NULL;

        if (tf_cnt > 0) {
            int v_bytes = (int)(tv_cnt * VERTEX_STRIDE);
            int n_bytes = (int)(tn_cnt * NORMAL_STRIDE);
            int f_bytes = (int)(tf_cnt * TRI_STRIDE);
            int u_bytes = (int)(tf_cnt * TRI_UV_STRIDE);
            if (!in_range(size, base + (int)tv_off, v_bytes)) return -1;
            if (!in_range(size, base + (int)tn_off, n_bytes)) return -1;
            if (!in_range(size, base + (int)tf_off, f_bytes)) return -1;
            if (!in_range(size, base + (int)tu_off, u_bytes)) return -1;
            m->tri_vertices     = (const re15_md1_vertex_t *)  (data + base + tv_off);
            m->tri_vertex_count = (int)tv_cnt;
            m->tri_normals      = (const re15_md1_vertex_t *)  (data + base + tn_off);
            m->tri_normal_count = (int)tn_cnt;
            m->triangles        = (const re15_md1_triangle_t *)(data + base + tf_off);
            m->triangle_count   = (int)tf_cnt;
            m->triangle_uvs     = (const re15_md1_tri_uv_t *)  (data + base + tu_off);
        }

        if (qf_cnt > 0) {
            int v_bytes = (int)(qv_cnt * VERTEX_STRIDE);
            int n_bytes = (int)(qn_cnt * NORMAL_STRIDE);
            int f_bytes = (int)(qf_cnt * QUAD_STRIDE);
            int u_bytes = (int)(qf_cnt * QUAD_UV_STRIDE);
            if (!in_range(size, base + (int)qv_off, v_bytes)) return -1;
            if (!in_range(size, base + (int)qn_off, n_bytes)) return -1;
            if (!in_range(size, base + (int)qf_off, f_bytes)) return -1;
            if (!in_range(size, base + (int)qu_off, u_bytes)) return -1;
            m->quad_vertices     = (const re15_md1_vertex_t *) (data + base + qv_off);
            m->quad_vertex_count = (int)qv_cnt;
            m->quad_normals      = (const re15_md1_vertex_t *) (data + base + qn_off);
            m->quad_normal_count = (int)qn_cnt;
            m->quads             = (const re15_md1_quad_t *)   (data + base + qf_off);
            m->quad_count        = (int)qf_cnt;
            m->quad_uvs          = (const re15_md1_quad_uv_t *)(data + base + qu_off);
        }
    }

    return 0;
}
