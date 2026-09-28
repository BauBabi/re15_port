/* probe_r30_irons-diary-welt.c — Runde 30, Thema E2 (Irons Diary: Welt-Prop + Memory Card).
 *
 * REINE MESSUNG, kein Bau. Die Sonde rechnet mit DENSELBEN Engine-Funktionen, die das
 * Spiel benutzt (re15_rdt_parse, re15_camera_build_view), damit die im Dossier
 * genannten Bildpunkte keine Nachbildung in Python sind, sondern das, was der Port
 * tatsaechlich zeichnen wuerde.
 *
 * Aufrufe:
 *   probe kamera [raum]            Kameratabelle + Sichtmatrix je Cut (Q12, trans, H)
 *   probe projekt x y z [raum]     Weltpunkt -> Bildpunkt in JEDEM Cut
 *   probe strahl cut sx sy hoehe   Bildpunkt -> Weltpunkt auf der Ebene y=hoehe
 *   probe pool [raum]              Raum hochfahren: Props (obj_id, Lage, Eltern)
 *   probe bits [raum]              Zone-9-Bits, die nach dem Hochfahren gesetzt sind
 *   probe aots [raum]              aktive AOT-Slots nach dem Hochfahren (wer ist frei?)
 *   probe anlauf                   Standlinie vor dem Schreibtisch: Spieler laeuft in -X an,
 *                                  bis re15_collision_constrain ihn haelt (Radius 450)
 *   probe abdeckung X Z W D SLOT   Aufhebe-Rechteck (Ecke X,Z Groesse W,D, flags 0x31) in
 *                                  SLOT legen und ZAEHLEN, von wie vielen Standorten x
 *                                  Blickrichtungen der FORWARD-620-Punkt es trifft; dazu
 *                                  jede Verdeckung durch einen frueheren ACTION-Record
 *   probe druck x z rot X Z W D SLOT   EIN echter Aktionsdruck ueber re15_aot_scan
 *   probe re2prop MD1 TIM          laedt die Engine das RE2-Prop UNVERAENDERT? (Parser, bbox, UV)
 *   probe licht x y z ry           Raumlicht je Cut am Ort des Props -> Vertexfarbe je Normale
 *   probe zonen x z                RVD-Kamerazonen, die den Punkt enthalten (welcher Cut ist aktiv?)
 *   probe reihe                    begehbare x-Abschnitte je z zwischen Hebetisch und Schreibtisch
 *
 * Dossier: analysis/befunde_runde30/irons-diary-welt.md
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_camera.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_aot.h"
#include "re15_collision.h"
#include "re15_skeleton.h"
#include "re15_item_modal.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_light.h"

extern re15_aot_state_t g_aot;
extern uint8_t          g_aot_action_pressed;

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

static re15_rdt_t s_rdt;
static uint8_t   *s_buf;
static size_t     s_size;

static int lade(const char *raum)
{
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE%c/ROOM%s.RDT", RE15_ASSET_PSX_DIR, raum[0], raum);
    s_buf = read_file(path, &s_size);
    if (!s_buf) { fprintf(stderr, "RDT nicht lesbar: %s\n", path); return 77; }
    if (re15_rdt_parse(s_buf, s_size, &s_rdt) != 0) { fprintf(stderr, "parse fail\n"); return 1; }
    return 0;
}

/* Bildpunkt wie der PC-Renderer: sx = 160 + H*vx/vz, sy = 120 + H*vy/vz (RTPS-Form,
 * camera_common.c Kopfkommentar). Rueckgabe vz (<=0 = hinter der Kamera). */
static long projiziere(const re15_camera_view_t *v, long x, long y, long z, double *sx, double *sy)
{
    long long vx = ((long long)v->rot[0]*x + (long long)v->rot[1]*y + (long long)v->rot[2]*z) >> 12;
    long long vy = ((long long)v->rot[3]*x + (long long)v->rot[4]*y + (long long)v->rot[5]*z) >> 12;
    long long vz = ((long long)v->rot[6]*x + (long long)v->rot[7]*y + (long long)v->rot[8]*z) >> 12;
    vx += v->trans[0]; vy += v->trans[1]; vz += v->trans[2];
    if (vz <= 0) { *sx = *sy = -1; return (long)vz; }
    *sx = 160.0 + (double)v->fov_screen_dist * (double)vx / (double)vz;
    *sy = 120.0 + (double)v->fov_screen_dist * (double)vy / (double)vz;
    return (long)vz;
}

static int cmd_kamera(void)
{
    printf("nCut=%d\n", s_rdt.cut_count);
    for (int c = 0; c < s_rdt.cut_count; c++) {
        const re15_camera_cut_t *k = &s_rdt.cuts[c];
        re15_camera_view_t v;
        int rc = re15_camera_build_view(k, &v);
        printf("CUT %d @0x%04X flag=%u fov=%u pos=%ld %ld %ld tgt=%ld %ld %ld pri=0x%lX rc=%d\n",
               c, (unsigned)(0x60 + 32*c), (unsigned)k->flag, (unsigned)k->fov,
               (long)k->pos_x, (long)k->pos_y, (long)k->pos_z,
               (long)k->target_x, (long)k->target_y, (long)k->target_z,
               (unsigned long)k->pri_offset, rc);
        if (rc == 0)
            printf("VIEW %d R= %ld %ld %ld  %ld %ld %ld  %ld %ld %ld  T= %ld %ld %ld  H= %d\n", c,
                   (long)v.rot[0], (long)v.rot[1], (long)v.rot[2],
                   (long)v.rot[3], (long)v.rot[4], (long)v.rot[5],
                   (long)v.rot[6], (long)v.rot[7], (long)v.rot[8],
                   (long)v.trans[0], (long)v.trans[1], (long)v.trans[2],
                   (int)v.fov_screen_dist);
    }
    return 0;
}

static int cmd_projekt(long x, long y, long z)
{
    for (int c = 0; c < s_rdt.cut_count; c++) {
        re15_camera_view_t v;
        if (re15_camera_build_view(&s_rdt.cuts[c], &v) != 0) continue;
        double sx, sy;
        long vz = projiziere(&v, x, y, z, &sx, &sy);
        printf("PROJ cut=%d welt=(%ld,%ld,%ld) -> sx=%.2f sy=%.2f vz=%ld %s\n", c, x, y, z, sx, sy, vz,
               (vz > 0 && sx >= 0 && sx < 320 && sy >= 0 && sy < 240) ? "IM BILD" : "ausserhalb");
    }
    return 0;
}

/* Sehstrahl durch (sx,sy) gegen die waagerechte Ebene y = hoehe.
 * Kamerapunkt p = (vx,vy,vz) mit vx=(sx-160)*vz/H, vy=(sy-120)*vz/H; Welt = R^T (p - T). */
static int cmd_strahl(int cut, double sx, double sy, double hoehe)
{
    if (cut < 0 || cut >= s_rdt.cut_count) return 2;
    re15_camera_view_t v;
    if (re15_camera_build_view(&s_rdt.cuts[cut], &v) != 0) return 3;
    double R[9]; for (int i = 0; i < 9; i++) R[i] = (double)v.rot[i] / 4096.0;
    double H = (double)v.fov_screen_dist;
    double dx = (sx - 160.0) / H, dy = (sy - 120.0) / H, dz = 1.0;
    double wd[3] = { R[0]*dx + R[3]*dy + R[6]*dz, R[1]*dx + R[4]*dy + R[7]*dz, R[2]*dx + R[5]*dy + R[8]*dz };
    double tx = -(double)v.trans[0], ty = -(double)v.trans[1], tz = -(double)v.trans[2];
    double wo[3] = { R[0]*tx + R[3]*ty + R[6]*tz, R[1]*tx + R[4]*ty + R[7]*tz, R[2]*tx + R[5]*ty + R[8]*tz };
    if (wd[1] == 0.0) return 4;
    double s = (hoehe - wo[1]) / wd[1];
    printf("STRAHL cut=%d bild=(%.2f,%.2f) ebene y=%.1f -> welt=(%.1f,%.1f,%.1f) vz=%.1f  ursprung=(%.1f,%.1f,%.1f)\n",
           cut, sx, sy, hoehe, wo[0] + s*wd[0], hoehe, wo[2] + s*wd[2], s, wo[0], wo[1], wo[2]);
    return 0;
}

static void raum_hochfahren(uint16_t room_id, int32_t px, int32_t pz)
{
    scd_vm_init(); re15_actor_init();
    g_current_room_id = room_id;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = px; pl->y = 0; pl->z = pz; pl->rot_y = 2048; pl->state = 1;
    g_scd.player_mode = 0;
    { extern void re15_msg_load_room_block(const uint8_t *b, int n);
      re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size); }
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, px, pz, 0);
}

static int cmd_pool(uint16_t room_id)
{
    re15_game_state_init();
    raum_hochfahren(room_id, -19000, -23000);
    printf("prop_count=%d (Pool %d)\n", (int)g_scd.prop_count, (int)RE15_SCD_MAX_PROPS);
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        printf("PROP slot=%d obj_id=%d active=%d type=%d band=%d parent=%d pos=(%ld,%ld,%ld) rot=(%d,%d,%d) flags=0x%04X\n",
               k, (int)g_scd.props[k].obj_id, (int)g_scd.props[k].active, (int)g_scd.props[k].obj_type,
               (int)g_scd.props[k].band, (int)g_scd.props[k].parent_obj,
               (long)g_scd.props[k].x, (long)g_scd.props[k].y, (long)g_scd.props[k].z,
               (int)g_scd.props[k].rot_x, (int)g_scd.props[k].rot_y, (int)g_scd.props[k].rot_z,
               (unsigned)g_scd.props[k].flags);
    return 0;
}

static int cmd_bits(uint16_t room_id)
{
    re15_game_state_init();
    raum_hochfahren(room_id, -19000, -23000);
    int n = 0;
    for (int b = 0; b < 256; b++)
        if (re15_game_flag_get(9, b)) { printf("ZONE9 bit %d gesetzt\n", b); n++; }
    printf("ZONE9 gesetzt nach Hochfahren: %d\n", n);
    return 0;
}

static int cmd_aots(uint16_t room_id)
{
    re15_game_state_init();
    raum_hochfahren(room_id, -19000, -23000);
    int n = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active) continue;
        n++;
        printf("AOT slot=%2d type=%d flags=0x%02X band=0x%02X evt=%d mitte=(%ld,%ld) halb=(%ld,%ld) quad=%d"
               "  -> x[%ld..%ld] z[%ld..%ld]\n",
               i, (int)a->type, (unsigned)a->sce_flags, (unsigned)a->band, (int)a->event_id,
               (long)a->x, (long)a->z, (long)a->half_w, (long)a->half_h, (int)a->has_quad,
               (long)(a->x - a->half_w), (long)(a->x + a->half_w),
               (long)(a->z - a->half_h), (long)(a->z + a->half_h));
    }
    printf("aktive AOT-Slots: %d von %d; Spieler-Band=%d\n", n, (int)RE15_AOT_MAX,
           re15_collision_debug_band());
    return 0;
}

/* Standlinie: der Spieler laeuft in Schritten von 30 Einheiten in -X auf den Tisch zu.
 * re15_collision_constrain ist der Spieler-Klemmpfad (Radius 450, Maske 1, s. dort). */
static int cmd_anlauf(void)
{
    re15_game_state_init();
    raum_hochfahren(0x1150, -22300, -18400);
    re15_collision_ensure_band(0);
    printf("Spieler-Band=%d\n", re15_collision_debug_band());
    for (int32_t z = -20800; z <= -16200; z += 200) {
        int32_t x = -22300, zz = z;
        for (int k = 0; k < 200; k++) {
            int32_t nx = x - 30, nz = zz;
            re15_collision_constrain(&s_rdt, x, zz, &nx, &nz);
            if (nx == x && nz == zz) break;
            x = nx; zz = nz;
        }
        printf("ANLAUF start z=%ld -> Stand (%ld,%ld)  FORWARD-620 bei Blick -X: (%ld,%ld)\n",
               (long)z, (long)x, (long)zz, (long)(x - 620), (long)zz);
    }
    return 0;
}

static int punkt_im_record(const re15_aot_t *a, int32_t x, int32_t z)
{
    if (a->has_quad) return re15_aot_point_in_quad(x, z, a->xs, a->zs);
    long dx = (long)x - a->x, dz = (long)z - a->z;
    if (dx < 0) dx = -dx;
    if (dz < 0) dz = -dz;
    return dx <= a->half_w && dz <= a->half_h;
}

/* steht der Spieler hier, ohne dass ihn der Klemmpfad wegschiebt? */
static int standort_gueltig(int32_t x, int32_t z)
{
    int32_t nx = x, nz = z;
    re15_collision_constrain(&s_rdt, x, z, &nx, &nz);
    return nx == x && nz == z;
}

static int cmd_abdeckung(int32_t X, int32_t Z, int32_t Wd, int32_t Dp, int slot)
{
    re15_game_state_init();
    raum_hochfahren(0x1150, -22300, -18400);
    re15_collision_ensure_band(0);
    if (g_aot.slots[slot].active) { printf("Slot %d ist BELEGT\n", slot); return 3; }
    re15_aot_set_item_tk_prop(slot, X + Wd / 2, Z + Dp / 2, Wd / 2, Dp / 2, 0x21, 1, 0, 0xFF);
    g_aot.slots[slot].sce_flags = 0x31;   /* FORWARD + ACTION + Spieler, wie 160 von 164 Items */
    g_aot.slots[slot].band      = 0;
    long standorte = 0, st_treffer = 0, kombis = 0, treffer = 0, verdeckt = 0;
    int  verdeckt_von[RE15_AOT_MAX]; memset(verdeckt_von, 0, sizeof verdeckt_von);
    long minx = 1 << 30, maxx = -(1 << 30), minz = 1 << 30, maxz = -(1 << 30);
    for (int32_t x = -23400; x <= -21600; x += 50) {
        for (int32_t z = -21000; z <= -15800; z += 50) {
            if (!standort_gueltig(x, z)) continue;
            standorte++;
            int irgendeine = 0;
            for (int r = 0; r < 64; r++) {
                int ry = r * 64;
                int32_t c = re15_cos_q12(ry), s = re15_sin_q12(ry);
                int32_t fx = x + (int32_t)((620 * c) >> 12);
                int32_t fz = z - (int32_t)((620 * s) >> 12);
                kombis++;
                if (!punkt_im_record(&g_aot.slots[slot], fx, fz)) continue;
                /* ein Record mit KLEINEREM Index, der denselben Druck zuerst bekaeme? */
                int vor = -1;
                for (int i = 0; i < slot; i++) {
                    const re15_aot_t *a = &g_aot.slots[i];
                    if (!a->active) continue;
                    if (!(a->sce_flags & 0x10)) continue;            /* nur ACTION */
                    if (a->type == RE15_AOT_TYPE_CAM_SWITCH) continue;
                    int h = 0;
                    if ((a->sce_flags & 0x40) && punkt_im_record(a, x, z))   h = 1;
                    if (!h && (a->sce_flags & 0x20) && punkt_im_record(a, fx, fz)) h = 1;
                    if (h) { vor = i; break; }
                }
                if (vor >= 0) { verdeckt++; verdeckt_von[vor]++; continue; }
                treffer++; irgendeine = 1;
            }
            if (irgendeine) {
                st_treffer++;
                if (x < minx) minx = x;
                if (x > maxx) maxx = x;
                if (z < minz) minz = z;
                if (z > maxz) maxz = z;
            }
        }
    }
    printf("RECHTECK Ecke(%ld,%ld) Groesse(%ld,%ld) Slot %d flags 0x31 band 0\n",
           (long)X, (long)Z, (long)Wd, (long)Dp, slot);
    printf("gueltige Standorte im Raster (50er, x -23400..-21600, z -21000..-15800): %ld\n", standorte);
    printf("Standorte mit MINDESTENS einer treffenden Blickrichtung: %ld\n", st_treffer);
    printf("   deren Huelle: x[%ld..%ld] z[%ld..%ld]\n", minx, maxx, minz, maxz);
    printf("Standort x Blickrichtung (64 je Standort): %ld, davon Treffer: %ld, von einem "
           "frueheren Record verdeckt: %ld\n", kombis, treffer, verdeckt);
    for (int i = 0; i < RE15_AOT_MAX; i++)
        if (verdeckt_von[i]) printf("   verdeckt von Slot %d: %d\n", i, verdeckt_von[i]);
    return 0;
}

/* EIN echter Druck ueber den Scan-Pfad des Spiels. */
static int cmd_druck(int32_t x, int32_t z, int rot, int32_t X, int32_t Z, int32_t Wd, int32_t Dp, int slot)
{
    re15_game_state_init();
    raum_hochfahren(0x1150, x, z);
    re15_collision_ensure_band(0);
    re15_aot_set_item_tk_prop(slot, X + Wd / 2, Z + Dp / 2, Wd / 2, Dp / 2, 0x21, 1, 0, 0xFF);
    g_aot.slots[slot].sce_flags = 0x31;
    g_aot.slots[slot].band      = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int f = 0; f < 12; f++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); }
    g_scd.message_display_frames = 0; g_scd.message_query = 0; g_scd.message_active = 0;
    re15_pauseflags_clear();
    pl->x = x; pl->z = z; pl->rot_y = (int16_t)rot;
    for (int f = 0; f < 3; f++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); }
    printf("vor dem Druck: modal aktiv=%d msg_frames=%d\n", re15_item_modal_active(),
           (int)g_scd.message_display_frames);
    g_aot_action_pressed = 1;
    scd_vm_tick();
    re15_aot_scan(pl->x, pl->z, 0);
    g_aot_action_pressed = 0;
    printf("DRUCK Stand (%ld,%ld) rot=%d -> modal aktiv=%d (Zustand %d), msg_frames=%d msg_id=%d, event=%d\n",
           (long)x, (long)z, rot, re15_item_modal_active(), (int)re15_item_modal_state(),
           (int)g_scd.message_display_frames, (int)g_scd.message_id,
           (int)g_aot.fired_event_id_this_frame);
    return re15_item_modal_active() ? 0 : 5;
}

/* Laedt der Port das RE2-Prop UNVERAENDERT? Beide Dateien durch die Engine-Parser. */
static int cmd_re2prop(const char *md1_pfad, const char *tim_pfad)
{
    size_t mn = 0, tn = 0;
    uint8_t *mb = read_file(md1_pfad, &mn);
    uint8_t *tb = read_file(tim_pfad, &tn);
    if (!mb || !tb) { fprintf(stderr, "Datei nicht lesbar\n"); return 77; }
    static re15_md1_t md1;
    re15_tim_t tim;
    int rm = re15_md1_parse(mb, (int)mn, &md1);
    int rt = re15_tim_parse(tb, (int)tn, &tim);
    printf("MD1 %lu B: parse=%d length=%lu unknown=%lu object_count=%lu mesh_count=%d\n",
           (unsigned long)mn, rm, (unsigned long)md1.length, (unsigned long)md1.unknown,
           (unsigned long)md1.object_count, md1.mesh_count);
    if (rm == 0) {
        for (int i = 0; i < md1.mesh_count; i++) {
            const re15_md1_mesh_t *m = &md1.meshes[i];
            printf("  Mesh %d: tri=%d (v=%d n=%d) quad=%d (v=%d n=%d)\n", i,
                   m->triangle_count, m->tri_vertex_count, m->tri_normal_count,
                   m->quad_count, m->quad_vertex_count, m->quad_normal_count);
            int lo[3] = { 1 << 30, 1 << 30, 1 << 30 }, hi[3] = { -(1 << 30), -(1 << 30), -(1 << 30) };
            for (int k = 0; k < m->quad_vertex_count; k++) {
                int v[3] = { m->quad_vertices[k].x, m->quad_vertices[k].y, m->quad_vertices[k].z };
                for (int a = 0; a < 3; a++) { if (v[a] < lo[a]) lo[a] = v[a]; if (v[a] > hi[a]) hi[a] = v[a]; }
            }
            printf("  bbox x %d..%d  y %d..%d  z %d..%d\n", lo[0], hi[0], lo[1], hi[1], lo[2], hi[2]);
            for (int q = 0; q < m->quad_count; q++) {
                const re15_md1_quad_uv_t *u = &m->quad_uvs[q];
                const re15_md1_quad_t    *f = &m->quads[q];
                printf("  Quad %d v=(%u,%u,%u,%u) n=(%u,%u,%u,%u) clut=0x%04X page=0x%04X uv=(%u,%u)(%u,%u)(%u,%u)(%u,%u)"
                       "  Normalenlaenge^2=%ld\n", q,
                       (unsigned)f->v0, (unsigned)f->v1, (unsigned)f->v2, (unsigned)f->v3,
                       (unsigned)f->n0, (unsigned)f->n1, (unsigned)f->n2, (unsigned)f->n3,
                       (unsigned)u->clut, (unsigned)u->page,
                       (unsigned)u->u0, (unsigned)u->v0, (unsigned)u->u1, (unsigned)u->v1,
                       (unsigned)u->u2, (unsigned)u->v2, (unsigned)u->u3, (unsigned)u->v3,
                       (long)m->quad_normals[f->n0].x * m->quad_normals[f->n0].x +
                       (long)m->quad_normals[f->n0].y * m->quad_normals[f->n0].y +
                       (long)m->quad_normals[f->n0].z * m->quad_normals[f->n0].z);
            }
        }
    }
    printf("TIM %lu B: parse=%d bpp=%d clut=%d @VRAM(%d,%d) Eintraege=%d | Bild %dx%d @VRAM(%d,%d)\n",
           (unsigned long)tn, rt, tim.bpp, tim.has_clut, tim.clut_x, tim.clut_y, tim.clut_entries,
           tim.width, tim.height, tim.data_x, tim.data_y);
    if (rt == 0 && tim.bpp == 8 && tim.has_clut) {
        /* Farbschluessel-Zensus: wie viele Texel der vom Mesh benutzten Flaeche sind 0x0000? */
        const uint8_t *px = (const uint8_t *)tim.pixels;
        long n0 = 0, n = 0;
        for (int y = 0; y < 64 && y < tim.height; y++)
            for (int x = 0; x < tim.width; x++) {
                n++;
                if (tim.clut[px[y * tim.width + x]] == 0) n0++;
            }
        printf("  obere 64 Zeilen (vom Mesh benutzt, v 0..63): %ld Texel, davon Wert 0x0000 (= nicht gezeichnet): %ld\n", n, n0);
    }
    return (rm == 0 && rt == 0) ? 0 : 6;
}

/* LICHT: was macht die Raumbeleuchtung aus einem Prop an dieser Stelle? Gerechnet mit den
 * Engine-Funktionen des Prop-Zeichners (main.c: re15_light_setup_actor mit rot=NULL, dann
 * re15_light_ctx_rotate_for_bone, dann re15_light_shade_vertex je Normale). Ausgegeben wird
 * je Cut die Vertex-Farbe fuer die sechs Achsnormalen des Modells; die Texel-Modulation des
 * Renderers ist farbe/128 * Texel (RE15_FACE_RGB_CODE = 128, re15_light.h). */
static int cmd_licht(long x, long y, long z, int ry)
{
    re15_light_set_t ls;
    if (re15_light_parse(s_rdt.lights, (size_t)s_rdt.lights_size, &ls) != 0) {
        printf("light parse fail (size %d)\n", s_rdt.lights_size); return 1; }
    int32_t c = re15_cos_q12(ry), s = re15_sin_q12(ry);
    int32_t rot[9] = { c, 0, s,  0, 4096, 0,  -s, 0, c };   /* pc_prop_rot_q12, reines rot_y */
    static const int16_t N[6][3] = { {0,-4096,0}, {0,4096,0}, {4096,0,0}, {-4096,0,0}, {0,0,4096}, {0,0,-4096} };
    static const char *nn[6] = { "oben(-Y)", "unten(+Y)", "+X", "-X", "+Z", "-Z" };
    for (int k = 0; k < ls.cut_count; k++) {
        const re15_light_cut_t *lc = &ls.cuts[k];
        printf("CUT %d scale=%u typ=(%u,%u,%u) ambient=(%u,%u,%u)\n", k, (unsigned)lc->global_scale,
               (unsigned)lc->type_flags[0], (unsigned)lc->type_flags[1], (unsigned)lc->type_flags[2],
               (unsigned)lc->ambient[0], (unsigned)lc->ambient[1], (unsigned)lc->ambient[2]);
        for (int i = 0; i < 3; i++)
            printf("   Licht %d farbe=(%u,%u,%u) pos=(%d,%d,%d) staerke=%u\n", i,
                   (unsigned)lc->colors[i][0], (unsigned)lc->colors[i][1], (unsigned)lc->colors[i][2],
                   (int)lc->positions[i][0], (int)lc->positions[i][1], (int)lc->positions[i][2],
                   (unsigned)lc->brightness[i]);
        int32_t pos[3] = { (int32_t)x, (int32_t)y, (int32_t)z };
        re15_actor_lightctx_t w, b;
        g_re15_active_cut = k;
        re15_light_setup_actor(lc, pos, NULL, &w);
        re15_light_ctx_rotate_for_bone(&w, rot, &b);
        printf("   aktive Lichter am Ort (%ld,%ld,%ld): %d\n", x, y, z, b.active_lights);
        for (int n = 0; n < 6; n++) {
            uint8_t r, g, bl;
            re15_light_shade_vertex(&b, N[n][0], N[n][1], N[n][2], &r, &g, &bl);
            printf("   Normale %-9s -> Vertexfarbe (%3u,%3u,%3u)  = Texel x (%.2f, %.2f, %.2f)\n", nn[n],
                   (unsigned)r, (unsigned)g, (unsigned)bl, r / 128.0, g / 128.0, bl / 128.0);
        }
    }
    return 0;
}

/* KAMERAZONEN: welche RVD-Zonen enthalten den Punkt (x,z), und wohin schalten sie?
 * Beantwortet "welchen Cut sieht der Spieler, wenn er HIER steht". */
static int cmd_zonen(long x, long z)
{
    printf("Zonen gesamt: %d\n", s_rdt.zone_count);
    for (int i = 0; i < s_rdt.zone_count; i++) {
        const re15_rdt_zone_t *q = &s_rdt.zones[i];
        int drin = re15_aot_point_in_quad((int32_t)x, (int32_t)z, q->xs, q->zs);
        printf("ZONE %2d von Cut %d nach Cut %d floor=0x%02X quad=(%d,%d)(%d,%d)(%d,%d)(%d,%d)%s\n", i,
               (int)q->cam_from, (int)q->cam_to, (unsigned)q->floor,
               (int)q->xs[0], (int)q->zs[0], (int)q->xs[1], (int)q->zs[1],
               (int)q->xs[2], (int)q->zs[2], (int)q->xs[3], (int)q->zs[3],
               drin ? "   <-- enthaelt den Punkt" : "");
    }
    return 0;
}

/* REIHE: fuer feste z-Werte die x-Abschnitte, in denen der Spieler STEHEN kann (der
 * Klemmpfad schiebt ihn nicht weg). Zeigt den Gang zwischen Hebetisch und Schreibtisch. */
static int cmd_reihe(void)
{
    re15_game_state_init();
    raum_hochfahren(0x1150, -22300, -18400);
    re15_collision_ensure_band(0);
    for (int32_t z = -21400; z <= -15600; z += 200) {
        printf("z=%6ld :", (long)z);
        int32_t a = 0; int offen = 0;
        for (int32_t x = -27000; x <= -17000; x += 25) {
            int g = standort_gueltig(x, z);
            if (g && !offen) { a = x; offen = 1; }
            if (!g && offen) { printf("  x[%ld..%ld]", (long)a, (long)(x - 25)); offen = 0; }
        }
        if (offen) printf("  x[%ld..-17000]", (long)a);
        printf("\n");
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 2) { fprintf(stderr, "Aufruf: probe kamera|projekt|strahl|pool|bits ...\n"); return 2; }
    const char *cmd = argv[1];
    const char *raum = "1150";
    if (!strcmp(cmd, "kamera"))  { if (argc > 2) raum = argv[2]; int r = lade(raum); return r ? r : cmd_kamera(); }
    if (!strcmp(cmd, "projekt")) {
        if (argc < 5) return 2;
        if (argc > 5) raum = argv[5];
        int r = lade(raum); if (r) return r;
        return cmd_projekt(atol(argv[2]), atol(argv[3]), atol(argv[4]));
    }
    if (!strcmp(cmd, "strahl")) {
        if (argc < 6) return 2;
        if (argc > 6) raum = argv[6];
        int r = lade(raum); if (r) return r;
        return cmd_strahl(atoi(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]));
    }
    if (!strcmp(cmd, "pool")) { if (argc > 2) raum = argv[2]; int r = lade(raum); return r ? r : cmd_pool((uint16_t)strtoul(raum, NULL, 16)); }
    if (!strcmp(cmd, "re2prop")) { if (argc < 4) return 2; return cmd_re2prop(argv[2], argv[3]); }
    if (!strcmp(cmd, "aots")) { if (argc > 2) raum = argv[2]; int r = lade(raum); return r ? r : cmd_aots((uint16_t)strtoul(raum, NULL, 16)); }
    if (!strcmp(cmd, "anlauf")) { int r = lade(raum); return r ? r : cmd_anlauf(); }
    if (!strcmp(cmd, "abdeckung")) {
        if (argc < 7) return 2;
        int r = lade(raum); if (r) return r;
        return cmd_abdeckung(atol(argv[2]), atol(argv[3]), atol(argv[4]), atol(argv[5]), atoi(argv[6]));
    }
    if (!strcmp(cmd, "druck")) {
        if (argc < 10) return 2;
        int r = lade(raum); if (r) return r;
        return cmd_druck(atol(argv[2]), atol(argv[3]), atoi(argv[4]),
                         atol(argv[5]), atol(argv[6]), atol(argv[7]), atol(argv[8]), atoi(argv[9]));
    }
    if (!strcmp(cmd, "reihe")) { int r = lade(raum); return r ? r : cmd_reihe(); }
    if (!strcmp(cmd, "zonen")) {
        if (argc < 4) return 2;
        int r = lade(raum); if (r) return r;
        return cmd_zonen(atol(argv[2]), atol(argv[3]));
    }
    if (!strcmp(cmd, "licht")) {
        if (argc < 6) return 2;
        int r = lade(raum); if (r) return r;
        return cmd_licht(atol(argv[2]), atol(argv[3]), atol(argv[4]), atoi(argv[5]));
    }
    if (!strcmp(cmd, "bits")) { if (argc > 2) raum = argv[2]; int r = lade(raum); return r ? r : cmd_bits((uint16_t)strtoul(raum, NULL, 16)); }
    fprintf(stderr, "unbekannt: %s\n", cmd);
    return 2;
}
