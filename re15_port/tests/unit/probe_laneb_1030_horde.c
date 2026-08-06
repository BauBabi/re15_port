/* probe_laneb_1030_horde.c — MESSUNG (kein Fix). Nutzer-Report 2026-08-06 zu ROOM1030:
 *   (1) "Die Zombies kriechen im ORIGINAL unter dem Tor durch. Bei uns nicht."
 *   (2) "Im ORIGINAL sind nur SECHS Zombies sichtbar. Bei uns sind es Dutzende."
 *
 * Diese Sonde MISST NUR den Live-Zustand des Ports (Banks geladen, AI wirklich getickt),
 * damit aus den Zahlen ueberhaupt geschlossen werden darf (Lehre aus der stair-Sonde).
 *
 * Gemessen wird:
 *   A  Der RDT-Rohbefund: alle Sce_em_set(0x44)-Records aus ROOM1030.RDT direkt aus der
 *      Datei dekodiert (Slot/Typ/behavior/killflag/Position/Yaw) — Referenz.
 *   B  Der Port-Roster nach main00/sub00: WIE VIELE Aktoren leben, wo, mit welchem
 *      grid/state/motion. Insbesondere: welche Script-Slots faellt der Port unter den
 *      Tisch (RE15_ACTOR_MAX=16, SCRIPT_SLOT_TO_ACTOR = slot+1).
 *   C  Die em16-Bank: clip_count + frame_count je Clip (welcher Clip koennte "kriechen"?)
 *   D  Live-AI ueber 600 Ticks MIT geladener Bank: bewegt sich irgendwer? Wechselt
 *      motion/state/sub_state? -> beantwortet "kriecht bei uns nicht".
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_aot.h"
#include "re15_room.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

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

static int16_t le16(const uint8_t *p) { return (int16_t)(p[0] | (p[1] << 8)); }

static void dump_rdt_emsets(const uint8_t *d, size_t sz)
{
    printf("== A: Sce_em_set(0x44)-Records direkt aus ROOM1030.RDT ==\n");
    /* Bekannter Block: Datei 0x1de6, 20-Byte-Stride. Wir scannen konservativ den
     * Bereich 0x1c00..0x2f00 nach dem Muster `44 <slot> 16 0d` (Typ 0x16/beh 0x0d). */
    int n = 0;
    for (size_t o = 0x1c00; o + 20 <= sz && o < 0x2f00; o++) {
        if (d[o] != 0x44) continue;
        if (d[o + 2] != 0x16) continue;
        printf("  @0x%04zx slot=%2u type=0x%02X beh=0x%02X pc4=%u pc5=%u pc6=%u pc7=%u "
               "pos=(%6d,%6d,%6d) dirY=%5d pc18=%u pc19=%u\n",
               o, d[o + 1], d[o + 2], d[o + 3], d[o + 4], d[o + 5], d[o + 6], d[o + 7],
               le16(&d[o + 8]), le16(&d[o + 10]), le16(&d[o + 12]), le16(&d[o + 16]),
               d[o + 18], d[o + 19]);
        n++;
        o += 19;
    }
    printf("  -> %d Records\n\n", n);
}

static void roster(const char *tag)
{
    int live = 0;
    printf("-- %s --\n", tag);
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *a = &g_actors[s];
        if (!a->active) continue;
        live++;
        printf("   slot=%2d type=0x%02X grid=0x%02X st=%u s1=0x%02x s2=%u s3=%u mo=%-3d fr=%-3u "
               "hp=%-4d flags=0x%02x floor=%u pos=(%7ld,%7ld,%7ld) ry=%d\n",
               s, a->type, a->grid_id, a->state, a->sub_state_1, a->sub_state_2, a->sub_state_3,
               (int)a->motion, a->anim_frame, (int)a->hp, a->flags, a->floor,
               (long)a->x, (long)a->y, (long)a->z, (int)a->rot_y);
    }
    printf("   LIVE=%d (inkl. Spieler slot0), g_actor_count=%u, RE15_ACTOR_MAX=%d\n\n",
           live, g_actor_count, RE15_ACTOR_MAX);
}

static uint8_t s_scratch[0x80000];

static void load_bank(const char *base, uint8_t type)
{
    char emsp[600]; size_t ems_size = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(emsp, &ems_size);
    if (!ems) { printf("  (CDEMD0.EMS nicht lesbar)\n"); return; }
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0 && len <= sizeof s_scratch) {
        re15_enemy_bank_t *eb = re15_enemy_alloc(type);
        if (eb) {
            memcpy(s_scratch, ems + off, len);
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(s_scratch, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                printf("== C: EM%03X-Bank: %d Clips, %d Bones ==\n", type, eb->anim.clip_count,
                       eb->skel.bone_count);
                printf("   (rootY: PSX-Y ist INVERTIERT -> je GROESSER, desto TIEFER am Boden.\n"
                       "    sumSpd = Summe der Keyframe-Speed-Vorwaertskomponente ueber den Clip.)\n");
                for (int c = 0; c < eb->anim.clip_count; c++) {
                    int fc = eb->anim.clips[c].frame_count;
                    long ymin = 1 << 30, ymax = -(1 << 30), ysum = 0;
                    long sx = 0, sy = 0, sz = 0;
                    for (int f = 0; f < fc; f++) {
                        uint32_t ent = re15_emd_get_frame_entry(&eb->anim, c, f);
                        int kf = (int)(ent & 0xFFF);
                        int16_t px, py, pz, vx, vy, vz;
                        re15_emd_get_keyframe_position(&eb->skel, kf, &px, &py, &pz);
                        re15_emd_get_keyframe_speed(&eb->skel, kf, &vx, &vy, &vz);
                        if (py < ymin) ymin = py;
                        if (py > ymax) ymax = py;
                        ysum += py;
                        sx += vx; sy += vy; sz += vz;
                    }
                    printf("   clip 0x%02X (%2d): frames=%-4d rootY min=%-6ld max=%-6ld avg=%-6ld  "
                           "sumSpd=(%ld,%ld,%ld)\n",
                           c, c, fc, ymin, ymax, fc ? ysum / fc : 0, sx, sy, sz);
                }
                printf("\n");
            } else printf("  EMD-Parse FAIL\n");
        }
    } else printf("  EMS-Entry FAIL (idx=%d)\n", idx);
    free(ems);
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    size_t size = 0;

    snprintf(path, sizeof path, "%s/STAGE1/ROOM1030.RDT", base);
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }

    dump_rdt_emsets(data, size);

    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }
    printf("ROOM1030: cuts=%d zones=%d sub_scd_count=%d\n\n", rdt.cut_count, rdt.zone_count,
           rdt.sub_scd_count);

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1030;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -25000; pl->y = 0; pl->z = -20000; pl->rot_y = 2048;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);

    load_bank(base, 0x16);

    /* ---- SCA-Dump: welche Kollisions-Entries liegen zwischen dem "Pferch"
     *      (x -19300..-14000, z -28000) und dem Hauptraum? ---- */
    printf("== SCA (Kollision) nach Raum-Load: %d Entries, rgn=[%d,%d,%d,%d,%d] ==\n",
           rdt.sca_count, rdt.sca_rgn[0], rdt.sca_rgn[1], rdt.sca_rgn[2], rdt.sca_rgn[3],
           rdt.sca_rgn[4]);
    {
        int base = 0;
        for (int r = 0; r < 5; r++) {
            for (int i = 0; i < rdt.sca_rgn[r]; i++) {
                const re15_sca_entry_t *e = &rdt.sca[base + i];
                printf("   rgn=%d idx=%2d  type=%2u u0=0x%02X u1=0x%02X floor=%u  "
                       "corner=(%6d,%6d) w=%5u d=%5u  -> x[%6d..%6d] z[%6d..%6d]\n",
                       r, i, e->type, e->u0, e->u1, e->floor, e->x, e->z, e->width, e->density,
                       e->x, e->x + (int)e->width, e->z, e->z + (int)e->density);
            }
            base += rdt.sca_rgn[r];
        }
    }
    printf("\n");

    printf("== B: Roster direkt nach Raum-Load ==\n");
    roster("B0: nach scd_room_reenter");

    for (int f = 0; f < 8; f++) {
        scd_vm_tick();
        re15_aot_scan(pl->x, pl->z, 0);
        if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
    }
    roster("B1: nach 8 VM-Ticks (main00/sub00 durch)");

    /* ---- D: Live-AI ---- */
    printf("== D: 600 Ticks LIVE-AI (Banks geladen, re15_enemy_ai_run_all) ==\n");
    long x0[RE15_ACTOR_MAX], z0[RE15_ACTOR_MAX];
    uint8_t mo0[RE15_ACTOR_MAX], gr0[RE15_ACTOR_MAX], s10[RE15_ACTOR_MAX];
    for (int s = 0; s < RE15_ACTOR_MAX; s++) {
        x0[s] = (long)g_actors[s].x; z0[s] = (long)g_actors[s].z;
        mo0[s] = g_actors[s].motion; gr0[s] = g_actors[s].grid_id; s10[s] = g_actors[s].sub_state_1;
    }
    for (int t = 0; t < 600; t++) {
        scd_vm_tick();
        re15_aot_scan(pl->x, pl->z, 0);
        if (g_aot.fired_event_id_this_frame) scd_event_fire(g_aot.fired_event_id_this_frame);
        re15_actor_step_all_walkers();
        re15_actors_anim_advance();
        re15_enemy_ai_run_all(0);
        if (t == 29 || t == 149 || t == 599) {
            char tag[48]; snprintf(tag, sizeof tag, "D t=%d", t);
            roster(tag);
        }
    }
    printf("== D-Delta nach 600 Ticks ==\n");
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!g_actors[s].active) continue;
        const re15_actor_t *a = &g_actors[s];
        printf("   slot=%2d dx=%6ld dz=%6ld  motion %d->%d  grid 0x%02X->0x%02X  s1 0x%02x->0x%02x\n",
               s, (long)a->x - x0[s], (long)a->z - z0[s], (int)mo0[s], (int)a->motion,
               gr0[s], a->grid_id, s10[s], a->sub_state_1);
    }

    free(data);
    return 0;
}
