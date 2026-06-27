/*
 * Unit tests for the MD1 parser (Phase 4.5).
 *
 * Synthesizes minimal MD1 blobs and verifies the parser extracts the
 * right counts + offsets + sample values.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "re15_md1.h"

#define TEST(cond, msg)                                                \
    do {                                                                \
        if (!(cond)) {                                                  \
            fprintf(stderr, "FAIL %s:%d %s — %s\n",                     \
                    __FILE__, __LINE__, __func__, msg);                 \
            return 1;                                                   \
        }                                                               \
    } while (0)

static void w32(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)(v & 0xFF);
    p[1] = (uint8_t)((v >> 8) & 0xFF);
    p[2] = (uint8_t)((v >> 16) & 0xFF);
    p[3] = (uint8_t)((v >> 24) & 0xFF);
}
static void w16(uint8_t *p, uint16_t v) {
    p[0] = (uint8_t)(v & 0xFF);
    p[1] = (uint8_t)((v >> 8) & 0xFF);
}

/* ---- Test 1: empty (0 meshes) ------------------------------------------ */
static int test_empty(void)
{
    uint8_t buf[12];
    w32(buf + 0, 12);
    w32(buf + 4, 0);
    w32(buf + 8, 0);

    re15_md1_t md;
    int r = re15_md1_parse(buf, sizeof(buf), &md);
    TEST(r == 0, "parse empty");
    TEST(md.mesh_count == 0, "zero meshes");
    TEST(md.object_count == 0, "object count");
    return 0;
}

/* ---- Test 2: single mesh with 3 triangle verts + 1 triangle ------------- */
static int test_single_triangle(void)
{
    /* Layout:
     *   [0..11]  Header: length=68, unknown=0, object_count=2 (=1 mesh)
     *   [12..67] Mesh header (56B)
     *
     *   Block offsets (relative to base = 12):
     *     verts at +56  → 3 verts × 8B = 24B, ends +80
     *     normals at +80 → 1 normal × 8B = 8B, ends +88
     *     faces at +88 → 1 tri × 12B = 12B, ends +100
     *     uvs at +100 → 1 uv × 12B = 12B, ends +112
     *
     * Absolute offsets: verts at file +68, normals +92, faces +100, uvs +112
     * Total file size = 124B
     */
    uint8_t buf[124];
    memset(buf, 0, sizeof(buf));

    w32(buf + 0, 124);
    w32(buf + 4, 0);
    w32(buf + 8, 2);    /* object_count → mesh_count = 1 */

    /* Mesh header at +12 */
    uint8_t *mh = buf + 12;
    w32(mh +  0, 56);   /* t_vertex_offset */
    w32(mh +  4, 3);    /* t_vertex_count */
    w32(mh +  8, 80);   /* t_normal_offset */
    w32(mh + 12, 1);    /* t_normal_count */
    w32(mh + 16, 88);   /* t_face_offset */
    w32(mh + 20, 1);    /* t_face_count */
    w32(mh + 24, 100);  /* t_uv_offset */
    /* Quad section all zeros = no quads */

    /* Vertices at absolute +68 (= base 12 + 56) */
    uint8_t *v = buf + 68;
    w16(v + 0, 100);   w16(v + 2, 200); w16(v + 4, 300);     /* v0 */
    w16(v + 8, 400);   w16(v +10, 500); w16(v +12, 600);     /* v1 */
    w16(v +16, 700);   w16(v +18, 800); w16(v +20, 900);     /* v2 */

    /* Normal at absolute +92 */
    uint8_t *n = buf + 92;
    w16(n + 0, 0); w16(n + 2, 0); w16(n + 4, 1000);

    /* Triangle at absolute +100 */
    uint8_t *t = buf + 100;
    w16(t + 0, 0); w16(t + 2, 0);   /* n0, v0 */
    w16(t + 4, 0); w16(t + 6, 1);   /* n1, v1 */
    w16(t + 8, 0); w16(t +10, 2);   /* n2, v2 */

    /* UV at absolute +112 (12 bytes) — minimal init */
    /* Triangle UV layout matches struct: u0, v0, clut(2B), u1, v1, page(2B), u2, v2 */
    uint8_t *u = buf + 112;
    u[0] = 10; u[1] = 20;   w16(u + 2, 0x1234);
    u[4] = 30; u[5] = 40;   w16(u + 6, 0x5678);
    u[8] = 50; u[9] = 60;

    re15_md1_t md;
    int r = re15_md1_parse(buf, sizeof(buf), &md);
    TEST(r == 0, "parse ok");
    TEST(md.mesh_count == 1, "1 mesh");

    const re15_md1_mesh_t *m = &md.meshes[0];
    TEST(m->tri_vertex_count == 3, "3 tri verts");
    TEST(m->tri_normal_count == 1, "1 tri normal");
    TEST(m->triangle_count == 1, "1 triangle");
    TEST(m->quad_count == 0, "0 quads");

    TEST(m->tri_vertices[0].x == 100, "v0.x");
    TEST(m->tri_vertices[2].z == 900, "v2.z");
    TEST(m->tri_normals[0].z == 1000, "n0.z");
    TEST(m->triangles[0].v0 == 0, "tri v0 idx");
    TEST(m->triangles[0].v1 == 1, "tri v1 idx");
    TEST(m->triangles[0].v2 == 2, "tri v2 idx");

    TEST(m->triangle_uvs[0].u0 == 10, "uv0.u");
    TEST(m->triangle_uvs[0].clut == 0x1234, "clut");
    TEST(m->triangle_uvs[0].page == 0x5678, "page");
    return 0;
}

/* ---- Test 3: truncated header fails ------------------------------------ */
static int test_truncated_header(void)
{
    uint8_t buf[8];
    memset(buf, 0, sizeof(buf));
    re15_md1_t md;
    int r = re15_md1_parse(buf, 8, &md);
    TEST(r == -1, "should fail with truncated header");
    return 0;
}

/* ---- Test 4 (Phase 4.5.3): single mesh with 4 quad verts + 1 quad ----- */
static int test_single_quad(void)
{
    /* Layout (offsets relative to base=12):
     *   Mesh header all-zero except quad section
     *   Quad section:
     *     quad_verts at +56  → 4 verts × 8B = 32B, ends +88
     *     quad_normals at +88 → 1 normal × 8B = 8B, ends +96
     *     quads at +96 → 1 quad × 16B = 16B, ends +112
     *     quad_uvs at +112 → 1 uv × 16B = 16B, ends +128
     *
     * Absolute: verts @ +68, normals @ +100, quads @ +108, uvs @ +124
     * Total = 140
     */
    uint8_t buf[140];
    memset(buf, 0, sizeof(buf));

    w32(buf + 0, 140);
    w32(buf + 8, 2);

    uint8_t *mh = buf + 12;
    /* Triangle section: all zero (no tris) */
    /* Quad section starts at offset 28 within the mesh header */
    w32(mh + 28, 56);    /* q_vertex_offset */
    w32(mh + 32, 4);     /* q_vertex_count */
    w32(mh + 36, 88);    /* q_normal_offset */
    w32(mh + 40, 1);     /* q_normal_count */
    w32(mh + 44, 96);    /* q_face_offset */
    w32(mh + 48, 1);     /* q_face_count */
    w32(mh + 52, 112);   /* q_uv_offset */

    /* 4 vertices at +68 */
    uint8_t *v = buf + 68;
    w16(v +  0, 10); w16(v +  2, 20); w16(v +  4, 30);
    w16(v +  8, 40); w16(v + 10, 50); w16(v + 12, 60);
    w16(v + 16, 70); w16(v + 18, 80); w16(v + 20, 90);
    w16(v + 24, 100); w16(v + 26, 110); w16(v + 28, 120);

    /* 1 normal at +100 */
    uint8_t *n = buf + 100;
    w16(n + 0, 0); w16(n + 2, 0); w16(n + 4, 4096);

    /* 1 quad at +108 (16 bytes: 8 × u16) */
    uint8_t *q = buf + 108;
    w16(q + 0, 0); w16(q + 2, 0);  /* n0, v0 */
    w16(q + 4, 0); w16(q + 6, 1);  /* n1, v1 */
    w16(q + 8, 0); w16(q +10, 2);  /* n2, v2 */
    w16(q +12, 0); w16(q +14, 3);  /* n3, v3 */

    re15_md1_t md;
    int r = re15_md1_parse(buf, sizeof(buf), &md);
    TEST(r == 0, "parse quad ok");
    TEST(md.mesh_count == 1, "1 mesh");

    const re15_md1_mesh_t *m = &md.meshes[0];
    TEST(m->triangle_count == 0, "no tris");
    TEST(m->quad_count == 1, "1 quad");
    TEST(m->quad_vertex_count == 4, "4 quad verts");
    TEST(m->quads[0].v0 == 0, "quad v0 idx");
    TEST(m->quads[0].v3 == 3, "quad v3 idx");
    TEST(m->quad_vertices[3].x == 100, "v3.x");
    TEST(m->quad_vertices[3].z == 120, "v3.z");
    return 0;
}

/* ---- Test (Phase 4.6.0 regression fix): UV struct sizeof matches the
 * on-disk file strides (12 for triangle, 16 for quad). If sizeof drifts,
 * subsequent uvs[1..] entries read from wrong byte offsets — what caused
 * the "triangle-shaped holes" the user reported on textured DOOR rendering. */
static int test_uv_struct_strides(void)
{
    TEST(sizeof(re15_md1_tri_uv_t) == 12,
         "TriangleUV struct must be exactly 12 bytes (file stride)");
    TEST(sizeof(re15_md1_quad_uv_t) == 16,
         "QuadUV struct must be exactly 16 bytes (file stride)");
    return 0;
}

/* ---- Test: parse two consecutive QuadUVs and verify u3/v3 of second
 * entry is read from the correct offset (relies on struct stride = 16). */
static int test_quad_uv_second_entry(void)
{
    /* Layout: header + 1 mesh + 4 quad verts + 1 normal + 2 quads + 2 quad_uvs
     *   header           12B  → ends at +12
     *   mesh header      56B  → ends at +68
     *   quad_verts(4×8)  32B  → starts +68, ends +100
     *   quad_normals(1×8) 8B  → starts +100, ends +108
     *   quads(2×16)      32B  → starts +108, ends +140
     *   quad_uvs(2×16)   32B  → starts +140, ends +172
     * All offsets relative to base=12 in mesh header. */
    uint8_t buf[172];
    memset(buf, 0, sizeof(buf));
    w32(buf + 0, 172);
    w32(buf + 8, 2);                  /* 1 mesh */

    uint8_t *mh = buf + 12;
    /* Quad section header (mesh-relative offsets, base=12) */
    w32(mh + 28, 56);                 /* qv_offset (56 from base = file +68) */
    w32(mh + 32, 4);                  /* qv_count */
    w32(mh + 36, 88);                 /* qn_offset (file +100) */
    w32(mh + 40, 1);                  /* qn_count */
    w32(mh + 44, 96);                 /* qf_offset (file +108) */
    w32(mh + 48, 2);                  /* qf_count = 2 quads */
    w32(mh + 52, 128);                /* qu_offset (file +140) */

    /* Vertices, normal, quads — non-zero so bounds-checks succeed.
     * Indices must be < qv_count(=4). */
    for (int i = 0; i < 4; i++) {
        uint8_t *v = buf + 68 + i * 8;
        w16(v + 0, (uint16_t)(i * 100));
        w16(v + 2, (uint16_t)(i * 100));
        w16(v + 4, (uint16_t)(i * 100));
    }
    /* Two quads referencing verts 0..3 */
    for (int q = 0; q < 2; q++) {
        uint8_t *qp = buf + 108 + q * 16;
        w16(qp + 0, 0); w16(qp + 2, 0);
        w16(qp + 4, 0); w16(qp + 6, 1);
        w16(qp + 8, 0); w16(qp +10, 2);
        w16(qp +12, 0); w16(qp +14, 3);
    }

    /* QuadUV #0 at +140: distinctive UVs */
    uint8_t *u0p = buf + 140;
    u0p[0] = 10; u0p[1] = 20;     /* u0,v0 */
    u0p[4] = 11; u0p[5] = 21;     /* u1,v1 */
    u0p[8] = 12; u0p[9] = 22;     /* u2,v2 */
    u0p[12] = 13; u0p[13] = 23;   /* u3,v3 */
    /* QuadUV #1 at +156 (must be 16 bytes after #0 — that's what the test
     * proves: if struct stride is wrong, this entry's UVs are misread) */
    uint8_t *u1p = buf + 156;
    u1p[0] = 30; u1p[1] = 40;
    u1p[4] = 31; u1p[5] = 41;
    u1p[8] = 32; u1p[9] = 42;
    u1p[12] = 33; u1p[13] = 43;

    re15_md1_t md;
    int r = re15_md1_parse(buf, sizeof(buf), &md);
    TEST(r == 0, "parse 2-quad");
    TEST(md.meshes[0].quad_count == 2, "2 quads parsed");

    /* QuadUV #0 — verify all 4 corners read correctly */
    TEST(md.meshes[0].quad_uvs[0].u0 == 10, "uv0.u0");
    TEST(md.meshes[0].quad_uvs[0].v0 == 20, "uv0.v0");
    TEST(md.meshes[0].quad_uvs[0].u3 == 13, "uv0.u3 (this was the bug)");
    TEST(md.meshes[0].quad_uvs[0].v3 == 23, "uv0.v3");

    /* QuadUV #1 — proves the stride is correct; if struct was 12 bytes,
     * uvs[1] would start at +152 instead of +156 → garbage */
    TEST(md.meshes[0].quad_uvs[1].u0 == 30, "uv1.u0 (stride check)");
    TEST(md.meshes[0].quad_uvs[1].u3 == 33, "uv1.u3 (stride check)");
    TEST(md.meshes[0].quad_uvs[1].v3 == 43, "uv1.v3 (stride check)");
    return 0;
}

/* ---- Test 5: out-of-bounds offset fails ------------------------------- */
static int test_oob_offset(void)
{
    uint8_t buf[68];   /* just header + mesh header, no blocks */
    memset(buf, 0, sizeof(buf));
    w32(buf + 0, 68);
    w32(buf + 8, 2);   /* 1 mesh */
    uint8_t *mh = buf + 12;
    w32(mh + 16, 9000);  /* t_face_offset way past file */
    w32(mh + 20, 1);     /* t_face_count = 1 */

    re15_md1_t md;
    int r = re15_md1_parse(buf, sizeof(buf), &md);
    TEST(r == -1, "OOB offset should fail");
    return 0;
}

int main(void)
{
    int fails = 0;
    fails += test_empty();
    fails += test_single_triangle();
    fails += test_single_quad();              /* Phase 4.5.3 */
    fails += test_uv_struct_strides();        /* Phase 4.6.0 regression */
    fails += test_quad_uv_second_entry();     /* Phase 4.6.0 regression */
    fails += test_truncated_header();
    fails += test_oob_offset();

    if (fails == 0) {
        printf("test_md1: all 7 tests PASSED\n");
        return 0;
    } else {
        printf("test_md1: %d test(s) FAILED\n", fails);
        return 1;
    }
}
