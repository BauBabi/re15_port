/* layout_check.c — BEWEIST per _Static_assert, dass jede Struktur, die 1:1 auf
 * Original-Dateibytes liegt, auf dem Zielcompiler exakt die Datei-Schrittweite hat.
 * Uebersetzt fuer x86_64 UND aarch64; schlaegt der Build fehl, stimmt das Layout nicht.
 * (Das Projekt benutzt bewusst KEIN __attribute__((packed)) — re15_md1.h:61 —,
 *  sondern explizite Fuellglieder. Genau das wird hier nachgemessen.) */
#include <stddef.h>
#include <stdint.h>
#include "re15_md1.h"
#include "re15_camera.h"
#include "re15_rdt.h"

/* --- MD1: Datei-Schrittweiten aus md1_common.c (VERTEX_STRIDE=8, TRI_STRIDE=12,
 *     QUAD_STRIDE=16, TRI_UV_STRIDE=12, QUAD_UV_STRIDE=16) ------------------ */
_Static_assert(sizeof(re15_md1_vertex_t)   ==  8, "MD1 vertex stride != 8");
_Static_assert(sizeof(re15_md1_triangle_t) == 12, "MD1 triangle stride != 12");
_Static_assert(sizeof(re15_md1_quad_t)     == 16, "MD1 quad stride != 16");
_Static_assert(sizeof(re15_md1_tri_uv_t)   == 12, "MD1 tri-uv stride != 12");
_Static_assert(sizeof(re15_md1_quad_uv_t)  == 16, "MD1 quad-uv stride != 16");
_Static_assert(offsetof(re15_md1_tri_uv_t,  clut) == 2,  "tri-uv clut offset");
_Static_assert(offsetof(re15_md1_tri_uv_t,  page) == 6,  "tri-uv page offset");
_Static_assert(offsetof(re15_md1_quad_uv_t, u3)   == 12, "quad-uv u3 offset");

/* --- RID/Kamera-Schnitt: 32 B je Eintrag (rdt_common.c:241 nCut*32) -------- */
_Static_assert(sizeof(re15_camera_cut_t) == 32, "camera cut stride != 32");
_Static_assert(offsetof(re15_camera_cut_t, pos_x)      ==  4, "cut pos_x offset");
_Static_assert(offsetof(re15_camera_cut_t, target_x)   == 16, "cut target_x offset");
_Static_assert(offsetof(re15_camera_cut_t, pri_offset) == 28, "cut pri_offset offset");

/* --- SCA-Kollisionszelle: 12 B je Eintrag (rdt_common.c:269 total*12) ------ */
_Static_assert(sizeof(re15_sca_entry_t) == 12, "sca entry stride != 12");
_Static_assert(offsetof(re15_sca_entry_t, x)     == 4,  "sca x offset");
_Static_assert(offsetof(re15_sca_entry_t, type)  == 8,  "sca type offset");
_Static_assert(offsetof(re15_sca_entry_t, floor) == 11, "sca floor offset");

int main(void) { return 0; }
