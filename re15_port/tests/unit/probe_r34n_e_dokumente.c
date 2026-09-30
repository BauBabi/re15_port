/* probe_r34n_e_dokumente.c — Runde 34 Nacht, Spur E (vier neue Dokumente), REINE MESSUNG.
 *
 * Raum-generische Fassung der Runde-30-Sonde probe_r30_irons-diary-welt.c: dieselben
 * Engine-Funktionen, die das Spiel benutzt (re15_rdt_parse, re15_camera_build_view,
 * scd_room_reenter, re15_aot_scan, re15_collision_constrain, re15_pri_parse_section,
 * re15_light_setup_actor), damit jede Zahl im Dossier das ist, was der Port tatsaechlich
 * rechnet - keine Nachbildung in Python.
 *
 * Aufrufe (RAUM = vierstellig hex, z.B. 1050):
 *   kamera   RAUM                          Kameratabelle + Sichtmatrix je Cut (Q12, trans, H)
 *   projekt  RAUM x y z                    Weltpunkt -> Bildpunkt + Kamera-z + OT-Bucket je Cut
 *   strahl   RAUM cut sx sy h              Bildpunkt -> Weltpunkt auf y=h (ECHTE Inverse der
 *                                          Engine-Matrix) + Vorwaertsprobe
 *   sicht    RAUM x y z                    je Cut: Bildpunkt und JEDE Maske ueber diesem Pixel
 *                                          (Original-Sektion; bei NULL die MSK-Seitendaten wie
 *                                          main.c), Tiefe, Schwelle, verdeckt ja/nein
 *   aots     RAUM                          nach dem ECHTEN Raumstart (scd_room_reenter mit allen
 *                                          Port-Installern): aktive AOT-Slots + Props
 *   zonen    RAUM x z                      RVD-Kamerazonen, die den Punkt enthalten
 *   stand    RAUM x0 x1 z0 z1 schritt      begehbare x-Abschnitte je z (Klemmpfad des Spielers)
 *   abdeckung RAUM X Z W D SLOT x0 x1 z0 z1 schritt
 *                                          Aufhebe-Rechteck (Ecke X,Z Groesse W,D, sat 0x31) in
 *                                          SLOT; ZAEHLT Standorte x 64 Blickrichtungen, deren
 *                                          FORWARD-620-Punkt trifft, und jeden Treffer, den ein
 *                                          FRUEHERER Aktions-Satz (kleinerer Slot) an sich zieht
 *   druck    RAUM x z rot X Z W D SLOT ITEM  EIN echter Aktionsdruck ueber re15_aot_scan
 *   licht    RAUM x y z                    Raumlicht je Cut am Ort -> Vertexfarbe je Achsnormale
 *
 * Dossier: analysis/befunde_runde34_nacht/E_dokumente.md
 */
#include <math.h>
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
#include "re15_light.h"
#include "re15_pri.h"
#include "re15_menu.h"

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
static uint16_t   s_raum;

static int lade(const char *raum)
{
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE%c/ROOM%s.RDT", RE15_ASSET_PSX_DIR, raum[0], raum);
    s_buf = read_file(path, &s_size);
    if (!s_buf) { fprintf(stderr, "RDT nicht lesbar: %s\n", path); return 77; }
    if (re15_rdt_parse(s_buf, s_size, &s_rdt) != 0) { fprintf(stderr, "parse fail\n"); return 1; }
    s_raum = (uint16_t)strtoul(raum, NULL, 16);
    return 0;
}

/* Bildpunkt wie der PC-Renderer (camera_common.c): sx = 160 + H*vx/vz, sy = 120 + H*vy/vz. */
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
        printf("PROJ cut=%d welt=(%ld,%ld,%ld) -> sx=%.2f sy=%.2f vz=%ld bucket=%d %s\n", c, x, y, z,
               sx, sy, vz, vz > 0 ? re15_pri_bucket_of_vz(vz) : -1,
               (vz > 0 && sx >= 0 && sx < 320 && sy >= 0 && sy < 240) ? "IM BILD" : "ausserhalb");
    }
    return 0;
}

/* 3x3-Inverse (Doppelt) der Engine-Matrix: die Q12-Matrix ist wegen SquareRoot0 NICHT exakt
 * orthonormal, R^T ist nicht die Inverse (Runde 30 irons-diary-welt 2.4: 110 Einheiten). */
static int inv3(const double m[9], double o[9])
{
    double a = m[0], b = m[1], c = m[2], d = m[3], e = m[4], f = m[5], g = m[6], h = m[7], i = m[8];
    double A = e*i - f*h, B = -(d*i - f*g), C = d*h - e*g;
    double det = a*A + b*B + c*C;
    if (fabs(det) < 1e-12) return -1;
    o[0] = A / det; o[1] = -(b*i - c*h) / det; o[2] = (b*f - c*e) / det;
    o[3] = B / det; o[4] = (a*i - c*g) / det;  o[5] = -(a*f - c*d) / det;
    o[6] = C / det; o[7] = -(a*h - b*g) / det; o[8] = (a*e - b*d) / det;
    return 0;
}

static int cmd_strahl(int cut, double sx, double sy, double hoehe)
{
    if (cut < 0 || cut >= s_rdt.cut_count) return 2;
    re15_camera_view_t v;
    if (re15_camera_build_view(&s_rdt.cuts[cut], &v) != 0) return 3;
    double R[9], Ri[9];
    for (int i = 0; i < 9; i++) R[i] = (double)v.rot[i] / 4096.0;
    if (inv3(R, Ri) != 0) return 4;
    double H = (double)v.fov_screen_dist;
    double d[3] = { (sx - 160.0) / H, (sy - 120.0) / H, 1.0 };
    double T[3] = { -(double)v.trans[0], -(double)v.trans[1], -(double)v.trans[2] };
    double wd[3], wo[3];
    for (int r = 0; r < 3; r++) {
        wd[r] = Ri[3*r]*d[0] + Ri[3*r+1]*d[1] + Ri[3*r+2]*d[2];
        wo[r] = Ri[3*r]*T[0] + Ri[3*r+1]*T[1] + Ri[3*r+2]*T[2];
    }
    if (wd[1] == 0.0) return 5;
    double s = (hoehe - wo[1]) / wd[1];
    double w[3] = { wo[0] + s*wd[0], hoehe, wo[2] + s*wd[2] };
    double px, py;
    long vz = projiziere(&v, lround(w[0]), lround(w[1]), lround(w[2]), &px, &py);
    printf("STRAHL cut=%d bild=(%.2f,%.2f) ebene y=%.1f -> welt=(%.1f,%.1f,%.1f) | gerundet zurueck "
           "(%.2f,%.2f) vz=%ld, Abstand %.2f px | Kamera-Ursprung (%.1f,%.1f,%.1f)\n",
           cut, sx, sy, hoehe, w[0], w[1], w[2], px, py, vz, hypot(px - sx, py - sy),
           wo[0], wo[1], wo[2]);
    return 0;
}

/* Masken eines Cuts: Original-Sektion, bei NULL die nachgezeichneten Seitendaten (R15M) wie
 * platform/pc/main.c ("MASKS/ROOM%04X.MSK", nur wenn das Original eine NULL-Sektion fuehrt). */
static int masken_des_cuts(int cut, re15_pri_cut_t *pri, int *nachgezeichnet)
{
    memset(pri, 0, sizeof *pri);
    *nachgezeichnet = 0;
    int n = re15_pri_parse_section(s_buf, s_size, s_rdt.cuts[cut].pri_offset, pri);
    if (n > 0) return n;
    char path[600];
    snprintf(path, sizeof path, "%s/MASKS/ROOM%04X.MSK", RE15_ASSET_PSX_DIR, (unsigned)s_raum);
    size_t ms = 0;
    uint8_t *mb = read_file(path, &ms);
    if (!mb) return 0;
    uint32_t off = re15_pri_msk_section_offset(mb, ms, cut);
    if (off) n = re15_pri_parse_section(mb, ms, off, pri);
    free(mb);
    if (n > 0) *nachgezeichnet = 1;
    return n;
}

static int cmd_sicht(long x, long y, long z)
{
    for (int c = 0; c < s_rdt.cut_count; c++) {
        re15_camera_view_t v;
        if (re15_camera_build_view(&s_rdt.cuts[c], &v) != 0) continue;
        double sx, sy;
        long vz = projiziere(&v, x, y, z, &sx, &sy);
        if (!(vz > 0 && sx >= 0 && sx < 320 && sy >= 0 && sy < 240)) continue;
        re15_pri_cut_t pri; int nz = 0;
        int n = masken_des_cuts(c, &pri, &nz);
        printf("SICHT cut=%d welt=(%ld,%ld,%ld) -> (%.2f,%.2f) vz=%ld bucket=%d | Masken im Cut: %d%s\n",
               c, x, y, z, sx, sy, vz, re15_pri_bucket_of_vz(vz), n,
               nz ? " (NACHGEZEICHNET, MSK-Seitendaten)" : (n ? " (Original-Sektion)" : ""));
        int ueber = 0, verdeckt = 0;
        for (int k = 0; k < pri.mask_count; k++) {
            const re15_pri_mask_t *m = &pri.masks[k];
            int dx = (int16_t)m->dstX, dy = (int16_t)m->dstY;
            if (sx < dx || sx >= dx + m->width || sy < dy || sy >= dy + m->height) continue;
            ueber++;
            int occ = re15_pri_mask_occludes(m->depth, vz);
            verdeckt += occ;
            printf("   Maske %2d Bild x %d..%d y %d..%d Tiefe %u -> verdeckt ab vz %.1f : %s\n",
                   k, dx, dx + m->width - 1, dy, dy + m->height - 1, (unsigned)m->depth,
                   re15_pri_mask_camera_z(m->depth), occ ? "VERDECKT" : "nicht");
        }
        printf("   -> Masken ueber dem Pixel: %d, davon verdeckend: %d\n", ueber, verdeckt);
    }
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

static const char *typname(int t)
{
    static const char *n[] = { "GENERIC", "DOOR", "ITEM", "CAM", "STAIR", "MESSAGE", "AUTO",
                               "EXAMINE", "FLAG", "WATER", "RAMP", "NONE" };
    return (t >= 0 && t <= 11) ? n[t] : "?";
}

static int cmd_aots(void)
{
    re15_game_state_init();
    raum_hochfahren(s_raum, 0, 0);
    int n = 0, frei_ab = -1;
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active) continue;
        n++;
        printf("AOT slot=%2d %-8s flags=0x%02X band=0x%02X evt=%3d x[%ld..%ld] z[%ld..%ld]%s",
               i, typname(a->type), (unsigned)a->sce_flags, (unsigned)a->band, (int)a->event_id,
               (long)(a->x - a->half_w), (long)(a->x + a->half_w),
               (long)(a->z - a->half_h), (long)(a->z + a->half_h), a->has_quad ? " QUAD" : "");
        if (a->type == RE15_AOT_TYPE_ITEM)
            printf("  item=0x%02X x%u bit=%u prop=%u", (unsigned)g_aot.item_params[i].item_type,
                   (unsigned)g_aot.item_params[i].amount, (unsigned)g_aot.item_params[i].taken_bit,
                   (unsigned)g_aot.item_params[i].taken_prop);
        printf("\n");
    }
    for (int i = 0; i < 48; i++) if (!g_aot.slots[i].active) { frei_ab = i; break; }
    printf("aktive AOT-Slots: %d; erster freier Slot < 48: %d\n", n, frei_ab);
    printf("prop_count=%d (Pool %d)\n", (int)g_scd.prop_count, (int)RE15_SCD_MAX_PROPS);
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        printf("PROP slot=%d obj_id=%d active=%d type=%d band=%d parent=%d pos=(%ld,%ld,%ld) rot=(%d,%d,%d) flags=0x%04X\n",
               k, (int)g_scd.props[k].obj_id, (int)g_scd.props[k].active, (int)g_scd.props[k].obj_type,
               (int)g_scd.props[k].band, (int)g_scd.props[k].parent_obj,
               (long)g_scd.props[k].x, (long)g_scd.props[k].y, (long)g_scd.props[k].z,
               (int)g_scd.props[k].rot_x, (int)g_scd.props[k].rot_y, (int)g_scd.props[k].rot_z,
               (unsigned)g_scd.props[k].flags);
    printf("RDT nOmodel=%d  Nachrichten im RDT-Block: %d Byte\n", (int)s_buf[2], (int)s_rdt.messages_size);
    int gesetzt = 0;
    for (int b = 0; b < 256; b++) if (re15_game_flag_get(9, b)) gesetzt++;
    printf("Zone-9-Bits nach dem Raumstart gesetzt: %d; Bits 57..60: %d %d %d %d\n", gesetzt,
           re15_game_flag_get(9, 57), re15_game_flag_get(9, 58), re15_game_flag_get(9, 59),
           re15_game_flag_get(9, 60));
    return 0;
}

static int cmd_zonen(long x, long z)
{
    printf("Zonen gesamt: %d\n", s_rdt.zone_count);
    for (int i = 0; i < s_rdt.zone_count; i++) {
        const re15_rdt_zone_t *q = &s_rdt.zones[i];
        int drin = re15_aot_point_in_quad((int32_t)x, (int32_t)z, q->xs, q->zs);
        if (!drin) continue;
        printf("ZONE %2d von Cut %d nach Cut %d floor=0x%02X quad=(%d,%d)(%d,%d)(%d,%d)(%d,%d)  <-- enthaelt (%ld,%ld)\n",
               i, (int)q->cam_from, (int)q->cam_to, (unsigned)q->floor,
               (int)q->xs[0], (int)q->zs[0], (int)q->xs[1], (int)q->zs[1],
               (int)q->xs[2], (int)q->zs[2], (int)q->xs[3], (int)q->zs[3], x, z);
    }
    return 0;
}

static int cmd_zonenliste(void)
{
    printf("Zonen gesamt: %d (RVD-Tabelle in Dateireihenfolge)\n", s_rdt.zone_count);
    for (int i = 0; i < s_rdt.zone_count; i++) {
        const re15_rdt_zone_t *q = &s_rdt.zones[i];
        printf("ZONE %2d von Cut %d nach Cut %d floor=0x%02X quad=(%d,%d)(%d,%d)(%d,%d)(%d,%d)\n",
               i, (int)q->cam_from, (int)q->cam_to, (unsigned)q->floor,
               (int)q->xs[0], (int)q->zs[0], (int)q->xs[1], (int)q->zs[1],
               (int)q->xs[2], (int)q->zs[2], (int)q->xs[3], (int)q->zs[3]);
    }
    return 0;
}

/* SCA-Kollisionszellen, die das Gebiet schneiden (Welt-x/z direkt, re15_collision.c). Die
 * Zelle traegt KEINE Hoehe - nur Grundriss, Form, Flagbytes und Etagenstufe. */
static int cmd_sca(long x0, long x1, long z0, long z1)
{
    printf("SCA gesamt: %d (Decke %u/%u)\n", s_rdt.sca_count, (unsigned)s_rdt.ceiling_x, (unsigned)s_rdt.ceiling_z);
    for (int i = 0; i < s_rdt.sca_count; i++) {
        const re15_sca_entry_t *e = &s_rdt.sca[i];
        long ex0 = e->x, ex1 = (long)e->x + e->width, ez0 = e->z, ez1 = (long)e->z + e->density;
        if (ex1 < x0 || ex0 > x1 || ez1 < z0 || ez0 > z1) continue;
        printf("SCA %3d x[%ld..%ld] z[%ld..%ld] typ=%u u0=0x%02X u1=0x%02X floor=%u\n", i,
               ex0, ex1, ez0, ez1, (unsigned)e->type, (unsigned)e->u0, (unsigned)e->u1, (unsigned)e->floor);
    }
    return 0;
}

static int standort_gueltig(int32_t x, int32_t z)
{
    int32_t nx = x, nz = z;
    re15_collision_constrain(&s_rdt, x, z, &nx, &nz);
    return nx == x && nz == z;
}

static int cmd_stand(int32_t x0, int32_t x1, int32_t z0, int32_t z1, int32_t st)
{
    re15_game_state_init();
    raum_hochfahren(s_raum, (x0 + x1) / 2, (z0 + z1) / 2);
    re15_collision_ensure_band(0);
    for (int32_t z = z0; z <= z1; z += st) {
        printf("z=%6ld :", (long)z);
        int32_t a = 0; int offen = 0;
        for (int32_t x = x0; x <= x1; x += st) {
            int g = standort_gueltig(x, z);
            if (g && !offen) { a = x; offen = 1; }
            if (!g && offen) { printf("  x[%ld..%ld]", (long)a, (long)(x - st)); offen = 0; }
        }
        if (offen) printf("  x[%ld..%ld]", (long)a, (long)x1);
        printf("\n");
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

/* Wuerde ein FRUEHERER (kleinerer Slot) Aktions-Satz denselben Druck bekommen? Geometrie wie
 * re15_aot_scan: Tuer = nur FORWARD-620 gegen das exakte Rechteck (+ Band); sonst CENTRE bei
 * 0x40, danach FORWARD bei 0x20; nur Saetze mit Aktionsbit 0x10. */
static int frueherer(int slot, int32_t x, int32_t z, int32_t fx, int32_t fz)
{
    for (int i = 0; i < slot; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active) continue;
        if (a->type == RE15_AOT_TYPE_CAM_SWITCH || a->type == RE15_AOT_TYPE_STAIR ||
            a->type == RE15_AOT_TYPE_NONE) continue;
        if (a->type == RE15_AOT_TYPE_DOOR) {
            if (punkt_im_record(a, fx, fz)) return i;
            continue;
        }
        if (a->sce_flags && !(a->sce_flags & 0x10)) continue;          /* nur ACTION */
        int h = 0;
        if ((a->sce_flags == 0 || (a->sce_flags & 0x40)) && punkt_im_record(a, x, z)) h = 1;
        if (!h && (a->sce_flags == 0 || (a->sce_flags & 0x20)) && punkt_im_record(a, fx, fz)) h = 1;
        if (h) return i;
    }
    return -1;
}

static int cmd_abdeckung(int32_t X, int32_t Z, int32_t Wd, int32_t Dp, int slot,
                         int32_t x0, int32_t x1, int32_t z0, int32_t z1, int32_t st)
{
    re15_game_state_init();
    raum_hochfahren(s_raum, (x0 + x1) / 2, (z0 + z1) / 2);
    re15_collision_ensure_band(0);
    if (g_aot.slots[slot].active) { printf("Slot %d ist BELEGT (%s)\n", slot, typname(g_aot.slots[slot].type)); return 3; }
    re15_aot_set_item_tk_prop(slot, X + Wd / 2, Z + Dp / 2, Wd / 2, Dp / 2, 0x48, 1, 0, 0xFF);
    g_aot.slots[slot].sce_flags = 0x31;
    g_aot.slots[slot].band      = 0;
    long standorte = 0, st_treffer = 0, kombis = 0, treffer = 0, verdeckt = 0;
    int  verdeckt_von[RE15_AOT_MAX]; memset(verdeckt_von, 0, sizeof verdeckt_von);
    long minx = 1 << 30, maxx = -(1 << 30), minz = 1 << 30, maxz = -(1 << 30);
    for (int32_t x = x0; x <= x1; x += st) {
        for (int32_t z = z0; z <= z1; z += st) {
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
                int vor = frueherer(slot, x, z, fx, fz);
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
    printf("RECHTECK Ecke(%ld,%ld) Groesse(%ld,%ld) Slot %d sat 0x31 band 0 -> x[%ld..%ld] z[%ld..%ld]\n",
           (long)X, (long)Z, (long)Wd, (long)Dp, slot, (long)(X), (long)(X + Wd), (long)Z, (long)(Z + Dp));
    printf("Raster %ld: x %ld..%ld, z %ld..%ld; gueltige Standorte: %ld\n",
           (long)st, (long)x0, (long)x1, (long)z0, (long)z1, standorte);
    printf("Standorte mit MINDESTENS einer treffenden Blickrichtung (ohne fruehere Saetze): %ld", st_treffer);
    if (st_treffer) printf("   Huelle x[%ld..%ld] z[%ld..%ld]", minx, maxx, minz, maxz);
    printf("\nStandort x Blickrichtung (64 je Standort): %ld, Treffer: %ld, von einem FRUEHEREN Satz "
           "abgefangen: %ld\n", kombis, treffer, verdeckt);
    for (int i = 0; i < RE15_AOT_MAX; i++)
        if (verdeckt_von[i]) printf("   abgefangen von Slot %d (%s, evt %d): %d\n", i,
                                    typname(g_aot.slots[i].type), (int)g_aot.slots[i].event_id,
                                    verdeckt_von[i]);
    return 0;
}

static int cmd_druck(int32_t x, int32_t z, int rot, int32_t X, int32_t Z, int32_t Wd, int32_t Dp,
                     int slot, int item)
{
    re15_game_state_init();
    raum_hochfahren(s_raum, x, z);
    re15_collision_ensure_band(0);
    if (g_aot.slots[slot].active) { printf("Slot %d ist BELEGT\n", slot); return 3; }
    re15_aot_set_item_tk_prop(slot, X + Wd / 2, Z + Dp / 2, Wd / 2, Dp / 2, (uint8_t)item, 1, 0, 0xFF);
    g_aot.slots[slot].sce_flags = 0x31;
    g_aot.slots[slot].band      = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int f = 0; f < 12; f++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); }
    g_scd.message_display_frames = 0; g_scd.message_query = 0; g_scd.message_active = 0;
    re15_pauseflags_clear();
    pl->x = x; pl->z = z; pl->rot_y = (int16_t)rot;
    for (int f = 0; f < 3; f++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, 0); }
    g_aot_action_pressed = 1;
    scd_vm_tick();
    re15_aot_scan(pl->x, pl->z, 0);
    g_aot_action_pressed = 0;
    printf("DRUCK Stand (%ld,%ld) rot=%d -> Item-Modal=%d Leser=%d msg_frames=%d msg_id=%d event=%d\n",
           (long)x, (long)z, rot, re15_item_modal_active(), re15_menu_doc_active(),
           (int)g_scd.message_display_frames, (int)g_scd.message_id,
           (int)g_aot.fired_event_id_this_frame);
    return 0;
}

static int cmd_licht(long x, long y, long z)
{
    re15_light_set_t ls;
    if (re15_light_parse(s_rdt.lights, (size_t)s_rdt.lights_size, &ls) != 0) {
        printf("light parse fail (size %d)\n", s_rdt.lights_size); return 1; }
    int32_t rot[9] = { 4096, 0, 0,  0, 4096, 0,  0, 0, 4096 };
    static const int16_t N[6][3] = { {0,-4096,0}, {0,4096,0}, {4096,0,0}, {-4096,0,0}, {0,0,4096}, {0,0,-4096} };
    static const char *nn[6] = { "oben(-Y)", "unten(+Y)", "+X", "-X", "+Z", "-Z" };
    for (int k = 0; k < ls.cut_count; k++) {
        const re15_light_cut_t *lc = &ls.cuts[k];
        int32_t pos[3] = { (int32_t)x, (int32_t)y, (int32_t)z };
        re15_actor_lightctx_t w, b;
        g_re15_active_cut = k;
        re15_light_setup_actor(lc, pos, NULL, &w);
        re15_light_ctx_rotate_for_bone(&w, rot, &b);
        printf("CUT %d ambient=(%u,%u,%u) aktive Lichter am Ort: %d |", k,
               (unsigned)lc->ambient[0], (unsigned)lc->ambient[1], (unsigned)lc->ambient[2], b.active_lights);
        for (int n = 0; n < 6; n++) {
            uint8_t r, g, bl;
            re15_light_shade_vertex(&b, N[n][0], N[n][1], N[n][2], &r, &g, &bl);
            printf(" %s=(%u,%u,%u)", nn[n], (unsigned)r, (unsigned)g, (unsigned)bl);
        }
        printf("\n");
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 3) { fprintf(stderr, "Aufruf: probe BEFEHL RAUM ...\n"); return 2; }
    const char *cmd = argv[1];
    int r = lade(argv[2]);
    if (r) return r;
    if (!strcmp(cmd, "kamera")) return cmd_kamera();
    if (!strcmp(cmd, "projekt") && argc >= 6) return cmd_projekt(atol(argv[3]), atol(argv[4]), atol(argv[5]));
    if (!strcmp(cmd, "strahl") && argc >= 7)
        return cmd_strahl(atoi(argv[3]), atof(argv[4]), atof(argv[5]), atof(argv[6]));
    if (!strcmp(cmd, "sicht") && argc >= 6) return cmd_sicht(atol(argv[3]), atol(argv[4]), atol(argv[5]));
    if (!strcmp(cmd, "aots")) return cmd_aots();
    if (!strcmp(cmd, "zonen") && argc >= 5) return cmd_zonen(atol(argv[3]), atol(argv[4]));
    if (!strcmp(cmd, "zonenliste")) return cmd_zonenliste();
    if (!strcmp(cmd, "sca") && argc >= 7)
        return cmd_sca(atol(argv[3]), atol(argv[4]), atol(argv[5]), atol(argv[6]));
    if (!strcmp(cmd, "stand") && argc >= 8)
        return cmd_stand(atol(argv[3]), atol(argv[4]), atol(argv[5]), atol(argv[6]), atol(argv[7]));
    if (!strcmp(cmd, "abdeckung") && argc >= 13)
        return cmd_abdeckung(atol(argv[3]), atol(argv[4]), atol(argv[5]), atol(argv[6]), atoi(argv[7]),
                             atol(argv[8]), atol(argv[9]), atol(argv[10]), atol(argv[11]), atol(argv[12]));
    if (!strcmp(cmd, "druck") && argc >= 12)
        return cmd_druck(atol(argv[3]), atol(argv[4]), atoi(argv[5]), atol(argv[6]), atol(argv[7]),
                         atol(argv[8]), atol(argv[9]), atoi(argv[10]), (int)strtol(argv[11], NULL, 0));
    if (!strcmp(cmd, "licht") && argc >= 6) return cmd_licht(atol(argv[3]), atol(argv[4]), atol(argv[5]));
    fprintf(stderr, "unbekannt/Argumente fehlen: %s\n", cmd);
    return 2;
}
