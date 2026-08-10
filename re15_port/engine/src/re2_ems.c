/*
 * RE1.5 Rebuilt — RE2-Asset-Infrastruktur: CDEMD0.EMS-TOC + EMD-Splitter + ENEMSE-TOC.
 * (WELLE A des RE2-Modus-Vollausbaus, 2026-08-10. PORT-OPTION, PC-only —
 * siehe re2_ems.h-Kopf; alle Byte-Belege dort + in tools/gen_re2_ems_toc.py.)
 *
 * PC-only gegatet: die vendorten Tabellen (~3.7 KB .rodata) und der Loader duerfen
 * das RAM-kritische PSX-Target nicht belasten (re15_enemy.h: _end < 0x80200000).
 */
#ifdef RE15_PLATFORM_PC

#include <string.h>
#include "re2_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"

#include "gen/re2_ems_toc.inc"   /* s_re2_cdemd0_toc[600] + s_re2_enemse_toc[292] */

/* ===== CDEMD0.EMS Sektor-TOC ============================================ */

int re2_ems_toc_entry(int kind, int rec, uint32_t *out_off, uint32_t *out_size)
{
    if (kind < RE2_EMS_KIND_MIN || kind >= RE2_EMS_KIND_MIN + RE2_EMS_KIND_COUNT)
        return -1;
    if (rec < 0 || rec > 3) return -1;
    /* Index-Basis (kind-0x10)*4 | rec — Binder FUN_8001aaa8 @0x8001ab4c-50 (|2 TIM)
     * und @0x8001ab7c-80 (|3 EMD); Overlay-Lader FUN_8001b710 (+0/+1). */
    int i = ((kind - RE2_EMS_KIND_MIN) * 4 + rec) * 2;
    uint32_t sector = s_re2_cdemd0_toc[i], size = s_re2_cdemd0_toc[i + 1];
    if (size == 0) return -1;         /* leerer Slot (z.B. Overlay bei kinds 0x11-0x13) */
    if (out_off)  *out_off  = sector * 0x800u;   /* CD-Sektor-Granularitaet */
    if (out_size) *out_size = size;
    return 0;
}

int re2_ems_locate(const uint8_t *ems, size_t ems_size, int kind, int rec,
                   const uint8_t **out_ptr, size_t *out_len)
{
    uint32_t off = 0, size = 0;
    if (!ems || re2_ems_toc_entry(kind, rec, &off, &size) != 0) return -1;
    if ((size_t)off + (size_t)size > ems_size) return -1;
    if (out_ptr) *out_ptr = ems + off;
    if (out_len) *out_len = (size_t)size;
    return 0;
}

/* ===== RE2-EMD-Splitter ================================================= */

static uint32_t rd_u32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16)
         | ((uint32_t)p[3] << 24);
}
static uint16_t rd_u16(const uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

/* Ein (EDD, kf-Pool)-Paar parsen: EDD via re15_emd_parse_animation, Bone-STRUKTUR
 * immer aus dir[2] (struct_off), Keyframe-Pool auf den Paar-eigenen EMR re-pointen
 * mit dessen EIGENER Geometrie (kf_ofs/bone_count/kf_size u16 @+2/+4/+6) — exakt
 * die Regel von re15_emd_parse_own_bank (emd_common.c:612; RE2-EMRs tragen dieselbe
 * Konvention: EM010 Paar-2/3-Header {0x64,8,15,0x50}/{0,8,15,0x50}, Paar 1
 * {0x64,0xB0,15,0x50} — Lane-I §1, byte-verifiziert). */
static int re2_parse_pair(const uint8_t *emd, size_t emd_size,
                          uint32_t edd_off, uint32_t struct_off, uint32_t pool_off,
                          re15_emd_skeleton_t *out_skel, re15_emd_animation_t *out_anim)
{
    if (!edd_off || edd_off >= emd_size)       return -1;
    if (!struct_off || struct_off >= emd_size) return -1;
    int ok_anim = re15_emd_parse_animation(emd + edd_off, emd_size - edd_off, out_anim) == 0;
    int ok_skel = re15_emd_parse_skeleton (emd + struct_off, emd_size - struct_off, out_skel) == 0;
    if (ok_anim && out_anim->clip_count <= 0) ok_anim = 0;
    if (ok_skel && pool_off && pool_off != struct_off &&
        (size_t)pool_off + 8 < emd_size && out_skel->keyframe_size_bytes > 0) {
        int kf_off       = (int)rd_u16(emd + pool_off + 2);
        int pool_bones   = (int)rd_u16(emd + pool_off + 4);
        int pool_kf_size = (int)rd_u16(emd + pool_off + 6);
        if (pool_kf_size > 12 && pool_bones > 0 && pool_bones <= RE15_EMD_MAX_BONES)
            out_skel->keyframe_size_bytes = pool_kf_size;
        if ((size_t)(pool_off + kf_off) < emd_size) {
            out_skel->keyframe_data      = emd + pool_off + kf_off;
            out_skel->keyframe_data_size = emd_size - (size_t)(pool_off + kf_off);
            out_skel->keyframe_count     = (int)(out_skel->keyframe_data_size /
                                                 (size_t)out_skel->keyframe_size_bytes);
        }
    }
    return (ok_anim && ok_skel) ? 0 : -1;
}

int re2_emd_parse_bank(const uint8_t *emd, size_t emd_size, re15_enemy_bank_t *eb)
{
    if (!emd || !eb || emd_size < 12) return -1;
    uint32_t dir_off = rd_u32(emd + 0);
    uint32_t dir_cnt = rd_u32(emd + 4);
    /* RE2-EMD: dir_count == 8 (EM010 @EMS+0x2A800: 0x23DAC / 8 — kein TIM im EMD). */
    if (dir_cnt != 8) return -2;
    if ((size_t)dir_off + 8 * 4 > emd_size) return -3;
    uint32_t D[8];
    for (int i = 0; i < 8; i++) D[i] = rd_u32(emd + dir_off + i * 4);

    /* dir[7] = MD1 (Binder-Zuweisung Entity+0x14 @0x8001ac14; EM010: nObj=34). */
    if (!D[7] || D[7] >= emd_size ||
        re15_md1_parse(emd + D[7], (int)(emd_size - D[7]), &eb->md1) != 0)
        return -4;

    /* Paar 1 (dir[1]/[2] — Entity+0x17C/+0x108 @0x8001aba0/abb0) -> loco-Feld.
     * dir[2] ist zugleich der Struktur-EMR; Pool == Struktur, kein Re-Point. */
    eb->loco_ok = (re2_parse_pair(emd, emd_size, D[1], D[2], 0,
                                  &eb->skel_loco, &eb->anim_loco) == 0);
    /* Paar 2 (dir[3]/[4] — Entity+0x184/+0x180 @0x8001abc0/abd0) -> own-Feld. */
    eb->own_ok = (re2_parse_pair(emd, emd_size, D[3], D[2], D[4],
                                 &eb->skel_own, &eb->anim_own) == 0);
    /* Paar 3 (dir[5]/[6] — Entity+0x18C/+0x188 @0x8001abe0/abf0) -> victim-Feld. */
    eb->victim_ok = (re2_parse_pair(emd, emd_size, D[5], D[2], D[6],
                                    &eb->skel_victim, &eb->anim_victim) == 0);

    /* Haupt-Bank (skel/anim) = das Paar mit den MEISTEN EDD-Clips — dieselbe
     * PORT-Regel wie re15_emd_parse_container (emd_common.c:482); die Renderer/
     * anim_select konsumieren nur dieses Feld. EM010: Paar 2 (31 Clips). */
    int best = -1, best_clips = -1;
    const uint32_t edd_of_pair[3]  = { D[1], D[3], D[5] };
    const uint32_t pool_of_pair[3] = { 0,    D[4], D[6] };
    for (int p = 0; p < 3; p++) {
        uint32_t eoff = edd_of_pair[p];
        if (!eoff || (size_t)eoff + 4 > emd_size) continue;
        int clips = (int)rd_u16(emd + eoff + 2) / 4;   /* EDD clip_table/4 == Clip-Zahl */
        if (clips > best_clips) { best_clips = clips; best = p; }
    }
    if (best < 0) return -5;
    if (re2_parse_pair(emd, emd_size, edd_of_pair[best], D[2], pool_of_pair[best],
                       &eb->skel, &eb->anim) != 0)
        return -6;
    return 0;
}

int re2_ems_load_bank(const uint8_t *ems, size_t ems_size, int kind,
                      re15_enemy_bank_t *eb, re15_tim_t *out_tim)
{
    const uint8_t *emd = NULL; size_t emd_len = 0;
    if (re2_ems_locate(ems, ems_size, kind, RE2_EMS_REC_EMD, &emd, &emd_len) != 0)
        return -1;
    int rc = re2_emd_parse_bank(emd, emd_len, eb);
    if (rc != 0) return rc;
    if (out_tim) {
        /* TIM = eigener TOC-Record (Binder-Index |2 @0x8001ab4c; EM010: type-9-TIM
         * 8bpp+CLUT @Sektor 52, 0x10420 B — byte-verifiziert). Optional: Parse-Fehler
         * macht die Bank nicht kaputt (Renderer prueft width/height). */
        const uint8_t *tim = NULL; size_t tim_len = 0;
        memset(out_tim, 0, sizeof *out_tim);
        if (re2_ems_locate(ems, ems_size, kind, RE2_EMS_REC_TIM, &tim, &tim_len) == 0)
            re15_tim_parse(tim, (int)tim_len, out_tim);
    }
    return 0;
}

/* ===== ENEMSE.VBS Bank-TOC + SE-Map ===================================== */

int re2_enemse_toc_entry(int bank, re2_enemse_rec_t *out)
{
    if (bank < 0 || bank >= RE2_ENEMSE_BANK_COUNT || !out) return -1;
    const uint32_t *e = &s_re2_enemse_toc[bank * 4];
    /* Subeintrag {u32 groesse, u24 rel_sektor (+u8 CD-Flags @+7, maskiert)} —
     * FUN_8005a09c: DAT_800d5308 = +0, DAT_800d5314 = u16@+4 + u8@+6 * 0x10000. */
    out->edt_size = e[0];
    out->edt_off  = (e[1] & 0xFFFFFFu) * 0x800u;
    out->vbd_size = e[2];
    out->vbd_off  = (e[3] & 0xFFFFFFu) * 0x800u;
    return 0;
}

void re2_enemse_decode_entry(uint32_t entry, re2_enemse_se_t *out)
{
    memset(out, 0, sizeof *out);
    out->vab_override = -1;
    if (entry == 0xFFFFFFFFu) { out->silent = 1; return; }   /* FUN_8005bd6c: *map != -1 */
    uint8_t b0 = (uint8_t)(entry & 0xFF);
    uint8_t b1 = (uint8_t)((entry >> 8) & 0xFF);
    uint8_t b2 = (uint8_t)((entry >> 16) & 0xFF);
    uint8_t b3 = (uint8_t)((entry >> 24) & 0xFF);
    if (b0 & 0x80) out->vab_override = b0 & 0x7F;   /* (*pbVar13 & 0x80) -> uVar15 = &0x7f */
    out->prog  = b1 & 0x7F;                          /* pbVar13[1] & 0x7f                   */
    out->tone  = b2 >> 4;                            /* pbVar13[2] >> 4                     */
    out->prio  = b2 & 0x0F;                          /* FUN_8005c92c(chan, b2 & 0xf)-Gate   */
    out->chan  = b3 & 0x1F;                          /* pbVar13[3] & 0x1f                   */
    out->extra = b3 >> 5;                            /* Extra-Tone/Kanal-Schleife am Ende   */
}

#endif /* RE15_PLATFORM_PC */
