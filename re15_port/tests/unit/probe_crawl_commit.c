/* probe_crawl_commit.c — MESSUNG (kein ctest-Gate): die ROOM1030-Kriech-Lokomotion mit der
 * ECHTEN Typ-0x16-Bank aus CDEMD0.EMS (Blob @Byte 0xD5800 = Sektor 427, 213584 B — EM-Tabelle
 * @0x80072f38 + (0x16-0x10)*8, B3 §6). Gemessen wird:
 *   (D1) Cliplaengen der eigenen BANK 1 (dir[3]/dir[4]): Soll 0x12=98f / 0x1A=99f (B3 §6)
 *   (D2) Toggle-Playout mit echter Bank: Ticks bis zum Kriech-Commit (~98)
 *   (D3) Kriech-VortrIEB: 130 Ticks FUN_801036dc-Aequivalent (Grid 0x81 / Zeile 0) —
 *        Positions-Deltas aus dem HAND-LOCK (FUN_80109470, 5x CompMatrix). Der Betrag wird
 *        GELOGGT, NICHT geprueft: die byte-true Soll-Geschwindigkeit ist erst per Savestate C
 *        messbar (Dossier §5 / B3 §8.1 — Rundungs-Verifikation folgt dynamisch).
 * Der Root-Kanal von Clip 0x1A ist px=0/pz=0 konstant (B3 §6) — JEDER gemessene XZ-Vortrieb
 * stammt also aus dem Hand-Lock, nicht aus Keyframe-Root-Motion. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_emd.h"
#include "re15_ems.h"

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

static uint8_t s_em[0x80000];

static re15_enemy_bank_t *load_em016(const char *base)
{
    char emsp[600]; size_t ems_size = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(emsp, &ems_size);
    if (!ems) { printf("WARN: CDEMD0.EMS nicht lesbar (%s)\n", emsp); return NULL; }
    int idx = re15_ems_index_for_type(0x16);
    size_t off = 0, len = 0;
    re15_enemy_bank_t *eb = NULL;
    if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0 && len <= sizeof s_em) {
        eb = re15_enemy_alloc(0x16);
        if (eb) {
            memcpy(s_em, ems + off, len);
            if (re15_emd_parse_container(s_em, len, &eb->md1, &eb->skel, &eb->anim, NULL) == 0)
                eb->ok = 1;
            eb->buf = NULL;
            eb->loco_ok = (re15_emd_parse_loco_bank(s_em, len, &eb->skel_loco, &eb->anim_loco) == 0);
            eb->own_ok  = (re15_emd_parse_own_bank (s_em, len, &eb->skel_own,  &eb->anim_own)  == 0);
        }
    }
    free(ems);
    return eb;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    re15_actor_init();
    re15_enemy_bank_t *eb = load_em016(base);
    if (!eb || !eb->own_ok) { printf("WARN: EM016-Bank nicht geladen — Messung uebersprungen\n"); return 0; }

    printf("== D1: BANK 1 (dir[3]/dir[4]) der Typ-0x16-Bank ==\n");
    printf("  clip_count=%d  fc[0x12]=%d (Soll 98)  fc[0x1A]=%d (Soll 99)  bones=%d (Soll 15)\n",
           eb->anim_own.clip_count,
           eb->anim_own.clip_count > 0x12 ? eb->anim_own.clips[0x12].frame_count : -1,
           eb->anim_own.clip_count > 0x1A ? eb->anim_own.clips[0x1A].frame_count : -1,
           eb->skel_own.bone_count);
    {   /* Frame-Flag-Zensus Clip 0x1A (Soll B3 §6: 0x2000 auf 0-21+63-98; 0x10000 auf 0,24-45,74-98) */
        int n2000 = 0, n10000 = 0, fc = eb->anim_own.clips[0x1A].frame_count;
        for (int f = 0; f < fc; f++) {
            uint32_t w = re15_emd_get_frame_entry(&eb->anim_own, 0x1A, f);
            if (w & 0x2000u)  n2000++;
            if (w & 0x10000u) n10000++;
        }
        printf("  Clip 0x1A Flag-Zensus: 0x2000 auf %d Frames (Soll 58), 0x10000 auf %d (Soll 48)\n",
               n2000, n10000);
    }
    {   /* Root-Kanal-Kontrolle: kf-Position von Clip 0x1A Frame 0 (Soll (0,-175,0), B3 §6) */
        int kf = (int)(re15_emd_get_frame_entry(&eb->anim_own, 0x1A, 0) & 0xFFFu);
        int16_t px = 0, py = 0, pz = 0;
        re15_emd_get_keyframe_position(&eb->skel_own, kf, &px, &py, &pz);
        printf("  Clip 0x1A Frame 0 Root-kf: (%d,%d,%d)  (Soll 0,-175,0)\n", px, py, pz);
    }

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[1];
    pl->active = 1; pl->hp = 100; pl->hit_react = 1;   /* DECIDE[0]-Grab-Write stumm halten */
    pl->x = 0; pl->z = 20000;

    printf("== D2: Toggle-Playout mit echter Bank (fc 0x12 = 98) ==\n");
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x16; e->hp = 100;
    e->state = 1; e->sub_state_1 = 0x10; e->grid_id = 0; e->sca_mask = 4;
    {
        int t = 0;
        while (e->grid_id != 0x81 && t < 200) { re15_enemy_ai_live_active(1); t++; }
        printf("  Kriech-Commit nach %d Ticks (Start rng&3; grid=0x%02x sca=%u Wort=%u/%u/%u)\n",
               t, e->grid_id, e->sca_mask, e->state, e->sub_state_1, e->sub_state_2);
    }

    printf("== D3: Kriech-Vortrieb ueber den Hand-Lock (Grid 0x81 / Zeile 0, 130 Ticks) ==\n");
    /* Blickrichtung auf den Spieler (+Z), Steer-Ziel = Spieler-Snapshot. */
    e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0x0fff);
    e->steer_x = (int16_t)pl->x; e->steer_z = (int16_t)pl->z;
    {
        int32_t x0 = e->x, z0 = e->z, xb = e->x, zb = e->z;
        int32_t lock_dx = 0, lock_dz = 0;
        int first_lock_tick = -1;
        for (int t = 1; t <= 130; t++) {
            int32_t px = e->x, pz = e->z;
            re15_enemy_ai_live_active(1);
            if (e->x != px || e->z != pz) {
                lock_dx += e->x - px; lock_dz += e->z - pz;
                if (first_lock_tick < 0) first_lock_tick = t;
            }
            if (t % 10 == 0) {
                printf("  t=%3d frame=%2u frac=%2u pos=(%6ld,%6ld) d10=(%ld,%ld) rot=%d\n",
                       t, (unsigned)e->anim_frame, e->anim_frac,
                       (long)e->x, (long)e->z, (long)(e->x - xb), (long)(e->z - zb), (int)e->rot_y);
                xb = e->x; zb = e->z;
            }
        }
        printf("  SUMME 130 Ticks: dx=%ld dz=%ld (erster Lock-Tick=%d; Blend-Gate 15 Ticks)\n",
               (long)(e->x - x0), (long)(e->z - z0), first_lock_tick);
        printf("  Hand-Lock-Beitrag: dx=%ld dz=%ld — Betrag NUR geloggt, Soll erst per\n"
               "  Savestate C (Dossier §5) — Rundungs-Verifikation folgt dynamisch.\n",
               (long)lock_dx, (long)lock_dz);
        if (lock_dz != 0 || lock_dx != 0)
            printf("  -> Hand-Lock erzeugt VORTRIEB (|dz|=%ld ueber %d Ticks)\n",
                   (long)(lock_dz < 0 ? -lock_dz : lock_dz), 130 - (first_lock_tick > 0 ? first_lock_tick : 0));
        else
            printf("  -> KEIN Vortrieb gemessen (Hand-Lock lieferte 0-Delta) — pruefen!\n");
    }
    return 0;
}
