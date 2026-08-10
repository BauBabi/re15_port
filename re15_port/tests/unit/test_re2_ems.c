/* =============================================================================
 * test_re2_ems — RE2-Asset-Infrastruktur WELLE A (re2_ems.c) gegen die ECHTEN
 * Bytes: die vendorte CDEMD0-Sektor-TOC (EXE @0x8009adf4, Datei 0x8B5F4) gegen
 * die drei Lane-I-Anker (Zombie 0x10 / Hund 0x20 / Kraehe 0x21) und der
 * RE2-EMD-Splitter gegen shared_assets/RE2/CDEMD0.EMS (EM010, byte-verifizierte
 * Soll-Werte aus Lane I §1: dir@0x23DAC count=8, Paar-1-EDD 8 Clips/clip0=65,
 * EMR-Struktur {0x64,0xB0,15,0x50}, MD1 nObj=34, TIM type 9).
 * ===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re2_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_tim.h"

#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

static int g_fail = 0;
#define CHECK(cond, msg, ...) do { if (!(cond)) { \
    fprintf(stderr, "FAIL: " msg "\n", ##__VA_ARGS__); g_fail = 1; } } while (0)

static uint8_t *slurp(const char *path, size_t *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return NULL; }
    fclose(f);
    if (out_sz) *out_sz = (size_t)sz;
    return b;
}

static void check_toc(int kind, int rec, uint32_t want_sector, uint32_t want_size)
{
    uint32_t off = 0, size = 0;
    int rc = re2_ems_toc_entry(kind, rec, &off, &size);
    CHECK(rc == 0, "toc_entry(0x%02X,%d) rc=%d", kind, rec, rc);
    CHECK(off == want_sector * 0x800u && size == want_size,
          "toc_entry(0x%02X,%d) = (0x%X,0x%X), erwartet (0x%X,0x%X)",
          kind, rec, off, size, want_sector * 0x800u, want_size);
}

int main(void)
{
    printf("=== RE2 CDEMD0.EMS TOC + EMD-Splitter (WELLE A) ===\n");

    /* (1) TOC-Anker — Rohbyte-verifiziert @EXE-Datei 0x8B5F4 (= 0x8009adf4). */
    check_toc(0x10, RE2_EMS_REC_OVL_SLOT1, 0,    53068);    /* == EMZ0.BIN-Groesse 0xCF4C */
    check_toc(0x10, RE2_EMS_REC_OVL_SLOT0, 26,   53068);    /* EMS[0xD000:] == EMZ0.BIN   */
    check_toc(0x10, RE2_EMS_REC_TIM,       52,   66592);    /* 0x10420 @0x1A000           */
    check_toc(0x10, RE2_EMS_REC_EMD,       85,   146892);   /* 0x23DCC @0x2A800           */
    check_toc(0x20, RE2_EMS_REC_OVL_SLOT0, 1206, 22266);    /* Hund ModB (Lane-Anker)     */
    check_toc(0x20, RE2_EMS_REC_TIM,       1217, 66592);
    check_toc(0x20, RE2_EMS_REC_EMD,       1250, 121144);
    check_toc(0x21, RE2_EMS_REC_OVL_SLOT1, 0x51E, 0x4A88);  /* Kraehe Code-Kopien         */
    check_toc(0x21, RE2_EMS_REC_OVL_SLOT0, 0x528, 0x4A88);
    check_toc(0x21, RE2_EMS_REC_TIM,       0x532, 0x8220);
    check_toc(0x21, RE2_EMS_REC_EMD,       0x543, 0x7E28);
    /* Grenzen: kinds 0x11-0x13 haben KEINE eigene Overlay-Kopie (Klemme FUN_8001b710),
     * aber TIM+EMD; ausserhalb der Tabelle -> -1. */
    CHECK(re2_ems_toc_entry(0x11, RE2_EMS_REC_OVL_SLOT0, NULL, NULL) == -1,
          "kind 0x11 Overlay muss leer sein");
    CHECK(re2_ems_toc_entry(0x11, RE2_EMS_REC_EMD, NULL, NULL) == 0,
          "kind 0x11 EMD muss existieren");
    CHECK(re2_ems_toc_entry(0x0F, 3, NULL, NULL) == -1 &&
          re2_ems_toc_entry(0x5B, 3, NULL, NULL) == -1, "kind-Grenzen 0x10..0x5A");
    if (!g_fail) printf("  (1) TOC-Anker Zombie/Hund/Kraehe + Grenzen OK\n");

    /* (2) EMD-Splitter gegen das echte Archiv. */
    size_t ems_sz = 0;
    uint8_t *ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &ems_sz);
    if (!ems) {
        fprintf(stderr, "FAIL: %s/CDEMD0.EMS nicht lesbar (Nutzer-Entscheidung: Assets liegen dort)\n",
                RE15_ASSET_RE2_DIR);
        return 1;
    }
    CHECK(ems_sz == 0xA9C000, "EMS-Groesse 0x%zX != 0xA9C000", ems_sz);

    const uint8_t *emd = NULL; size_t emd_len = 0;
    CHECK(re2_ems_locate(ems, ems_sz, 0x10, RE2_EMS_REC_EMD, &emd, &emd_len) == 0,
          "EM010-EMD-Record nicht lokalisierbar");
    if (emd) {
        /* Record-Kopf byte-exakt (EM010 @EMS+0x2A800: ac 3d 02 00 08 00 00 00). */
        uint32_t dir_off = (uint32_t)emd[0] | (emd[1]<<8) | (emd[2]<<16) | ((uint32_t)emd[3]<<24);
        uint32_t dir_cnt = (uint32_t)emd[4] | (emd[5]<<8) | (emd[6]<<16) | ((uint32_t)emd[7]<<24);
        CHECK(dir_off == 0x23DAC && dir_cnt == 8,
              "EM010 dir_off/count = 0x%X/%u, erwartet 0x23DAC/8", dir_off, dir_cnt);
        /* Directory byte-exakt (Lane I §1, hier gegen die Rohbytes im Test re-gelesen). */
        static const uint32_t want_dir[8] =
            { 0x8, 0xC, 0x69C, 0x506C, 0x64A8, 0x17300, 0x17B80, 0x1EE38 };
        for (int i = 0; i < 8; i++) {
            uint32_t d = (uint32_t)emd[dir_off+i*4] | (emd[dir_off+i*4+1]<<8)
                       | (emd[dir_off+i*4+2]<<16) | ((uint32_t)emd[dir_off+i*4+3]<<24);
            CHECK(d == want_dir[i], "EM010 dir[%d] = 0x%X, erwartet 0x%X", i, d, want_dir[i]);
        }

        static re15_enemy_bank_t eb;             /* gross (frames[1664] x4) — kein Stack-Local */
        memset(&eb, 0, sizeof eb);
        int rc = re2_emd_parse_bank(emd, emd_len, &eb);
        CHECK(rc == 0, "re2_emd_parse_bank rc=%d", rc);
        if (rc == 0) {
            /* Skelett: EMR-Struktur dir[2] {0x64,0xB0,15,0x50} -> 15 Bones, kf 80 B. */
            CHECK(eb.skel.bone_count == 15, "bones=%d != 15", eb.skel.bone_count);
            CHECK(eb.skel.keyframe_size_bytes == 0x50, "kf_size=%d != 0x50",
                  eb.skel.keyframe_size_bytes);
            /* Paar 1 (loco-Feld): 8 Clips, clip0 = 65 Frames (EDD @dir[1], Tabelle 0x20). */
            CHECK(eb.loco_ok, "Paar 1 nicht geparst");
            CHECK(eb.anim_loco.clip_count == 8, "Paar-1-Clips=%d != 8", eb.anim_loco.clip_count);
            CHECK(eb.anim_loco.clips[0].frame_count == 65, "Paar-1-clip0=%d != 65",
                  eb.anim_loco.clips[0].frame_count);
            /* Haupt-Bank = groesstes Paar = Paar 2 (dir[3]: Tabelle 0x7C -> 31 Clips). */
            CHECK(eb.anim.clip_count == 31, "Haupt-Clips=%d != 31", eb.anim.clip_count);
            CHECK(eb.own_ok && eb.anim_own.clip_count == 31, "own-Feld (Paar 2) != 31 Clips");
            /* Paar 3 (victim-Feld): dir[5]-Tabelle 0x44 -> 17 Clips. */
            CHECK(eb.victim_ok && eb.anim_victim.clip_count == 17,
                  "victim-Feld (Paar 3) = %d Clips != 17", eb.anim_victim.clip_count);
            /* MD1 dir[7]: Header {0x35F4, 0, nObj=34} -> mesh_count = 34/2 = 17. */
            CHECK(eb.md1.mesh_count == 17, "MD1 mesh_count=%d != 17 (nObj 34)", eb.md1.mesh_count);
            /* Splitter-Offsets byte-genau: Paar-1-Pool = dir[2]+kf_ofs(0xB0);
             * Paar-2-Pool re-pointed auf dir[4]+kf_ofs(8) (Pool-Header {0x64,8,15,0x50}). */
            CHECK(eb.skel_loco.keyframe_data == emd + 0x69C + 0xB0,
                  "Paar-1-Pool != dir[2]+0xB0");
            CHECK(eb.skel_own.keyframe_data == emd + 0x64A8 + 8,
                  "Paar-2-Pool != dir[4]+8");
            CHECK(eb.skel_victim.keyframe_data == emd + 0x17B80 + 8,
                  "Paar-3-Pool != dir[6]+8");
            if (!g_fail) printf("  (2) EM010: 15 Bones, Paar1 8/65, Haupt 31, Paar3 17, "
                                "MD1 17 Meshes, Pools byte-genau\n");
        }

        /* (3) TIM = separater TOC-Record (type 9 = 8bpp+CLUT, magic 0x10 @Sektor 52). */
        const uint8_t *tim = NULL; size_t tim_len = 0;
        CHECK(re2_ems_locate(ems, ems_sz, 0x10, RE2_EMS_REC_TIM, &tim, &tim_len) == 0,
              "EM010-TIM-Record nicht lokalisierbar");
        if (tim) {
            uint32_t magic = (uint32_t)tim[0] | (tim[1]<<8) | (tim[2]<<16) | ((uint32_t)tim[3]<<24);
            uint32_t type  = (uint32_t)tim[4] | (tim[5]<<8) | (tim[6]<<16) | ((uint32_t)tim[7]<<24);
            CHECK(magic == 0x10 && type == 9, "TIM magic/type = 0x%X/0x%X != 0x10/9", magic, type);
            re15_tim_t t; memset(&t, 0, sizeof t);
            CHECK(re15_tim_parse(tim, (int)tim_len, &t) == 0 && t.width > 0 && t.height > 0,
                  "EM010-TIM parst nicht (w=%d h=%d)", t.width, t.height);
            if (!g_fail) printf("  (3) EM010-TIM: type 9, %dx%d\n", t.width, t.height);
        }

        /* (4) Komplett-Lader. */
        static re15_enemy_bank_t eb2;
        memset(&eb2, 0, sizeof eb2);
        re15_tim_t t2; memset(&t2, 0, sizeof t2);
        CHECK(re2_ems_load_bank(ems, ems_sz, 0x10, &eb2, &t2) == 0 && t2.width > 0,
              "re2_ems_load_bank(0x10) fehlgeschlagen");
        if (!g_fail) printf("  (4) re2_ems_load_bank OK\n");
    }
    free(ems);

    if (g_fail) { fprintf(stderr, "test_re2_ems: FAILED\n"); return 1; }
    printf("test_re2_ems: OK\n");
    return 0;
}
