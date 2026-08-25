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
#include "re15_skeleton.h"   /* re15_skel_compute_pose + g_anim_pose_actor (Wurzelhoehen-Messung) */

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

/* ===== WELLE G: HYBRID-RIG (RE1.5-Geometrie unter RE2-Keyframes) ======== *
 * Alle Messwerte + die Begruendung der Tabellen stehen im Kopf von re2_ems.h.  */

/* Zombie-Familie 0x10/0x11/0x12/0x16/0x18 — RE1.5 hat Kopf auf Slot 14, Arme 8..13. */
static const int8_t k_perm_zombie[15] = { 7,0,1,2,3,4,5,6, 14, 8, 9,10,11,12,13 };
/* Zombie Girl 0x13 — RE1.5 hat Kopf auf Slot 8, Arme 9..14 (= RE2-Reihenfolge ab 8). */
static const int8_t k_perm_zgirl [15] = { 7,0,1,2,3,4,5,6,  8, 9,10,11,12,13,14 };
/* Hund 0x20 — RE2-Slots 7 und 10 (drittes Vorderbein-Segment) haben kein RE1.5-Gegenstueck. */
static const int8_t k_perm_dog   [17] = { 0,1,2,3,4,5,6,-1, 7, 8,-1, 9,10,11,12,13,14 };
/* Kraehe/Spinne/Baby — parent[] und Bone-Offsets in beiden Spielen identisch. */
static const int8_t k_perm_ident [20] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 };

int re2_hybrid_perm(int kind, const int8_t **out_perm)
{
    const int8_t *p = NULL; int n = -1;
    switch (kind) {
    case 0x10: case 0x11: case 0x12: case 0x16: case 0x18: p = k_perm_zombie; n = 15; break;
    case 0x13:                                             p = k_perm_zgirl;  n = 15; break;
    case 0x20:                                             p = k_perm_dog;    n = 17; break;
    case 0x21:                                             p = k_perm_ident;  n = 13; break;
    case 0x25:                                             p = k_perm_ident;  n = 20; break;
    case 0x26:                                             p = k_perm_ident;  n =  1; break;
    default: break;
    }
    if (out_perm) *out_perm = p;
    return n;
}

/* Bind-Positionen EINER Skelett-Kopie umsetzen (Hierarchie = RE2, Laengen = RE1.5). */
static int re2_hybrid_rig_skel(re15_emd_skeleton_t *dst, const int8_t *perm, int n,
                               const re15_emd_skeleton_t *s15)
{
    int unmapped = 0;
    for (int i = 0; i < n; i++) {
        int p = (int)dst->bone_parent[i];
        if (p < 0) continue;             /* Wurzel: bone_relative_pos[0] wird nie gelesen —
                                          * der Root-Trans kommt aus dem Keyframe
                                          * (skeleton_common.c:680, FUN_8001f3bc.c:28-37). */
        int mi = (int)perm[i];
        int mp = (p < n) ? (int)perm[p] : -1;
        if (mi < 0 || mp < 0) continue;  /* Slot ohne RE1.5-Gegenstueck (Hundepfoten) */
        if ((int)s15->bone_parent[mi] == mp) {
            dst->bone_relative_pos[i][0] = s15->bone_relative_pos[mi][0];
            dst->bone_relative_pos[i][1] = s15->bone_relative_pos[mi][1];
            dst->bone_relative_pos[i][2] = s15->bone_relative_pos[mi][2];
        } else if ((int)s15->bone_parent[mp] == mi) {
            /* umgekehrte Kante = die Um-Wurzelung Huefte<->Brust; in beiden Rigs (0,0,0). */
            dst->bone_relative_pos[i][0] = (int16_t)(-s15->bone_relative_pos[mp][0]);
            dst->bone_relative_pos[i][1] = (int16_t)(-s15->bone_relative_pos[mp][1]);
            dst->bone_relative_pos[i][2] = (int16_t)(-s15->bone_relative_pos[mp][2]);
        } else {
            unmapped++;                  /* RE2-Wert bleibt stehen — Test pinnt 0 */
        }
    }
    return unmapped;
}

/* ===== HYBRID-WURZELHOEHE ================================================================
 *
 * Nutzer-Befund: "der Zombie in 10D0 schwebt nach dem Aufstehen leicht in der Luft" — gemessen
 * global (ROOM1030: RE1.5-Flavor Sohle +6..+127 = auf dem Boden, RE2-Flavor -133..-219).
 * Herleitung und Beleg-Kette stehen am Feld `root_y_fix` in re15_emd.h; kurz:
 *   y = e->y + Wurzel_Y(Keyframe) + SUM ueber die Kette ( R_Eltern * Bind_Offset ).y
 * Der Hybrid tauscht nur den zweiten Summanden. Da die Rotationen identisch sind (sie kommen
 * aus dem RE2-Keyframe), ist die Differenz algebraisch exakt
 *   K(kf) = SUM ueber die Kette ( R_Eltern(kf) * (Bind_RE2 - Bind_RE15) ).y
 * und pro Keyframe messbar, indem dieselbe Pose zweimal aufgebaut wird. Genau das tut der
 * Block hier — er laeuft an der einzigen Stelle, an der BEIDE Bind-Tabellen und BEIDE MD1
 * gleichzeitig vorliegen: vor dem Ueberschreiben in re2_hybrid_apply.
 *
 * Bezugspunkt ist der tiefste VERTEX, nicht der tiefste Bone ("K_mesh"): der Nutzer sieht
 * Geometrie, nicht Gelenke. Das setzt Mesh-Index == Bone-Index fuer die RE2-Zombie-MD1
 * voraus — belegt, weil EM010 17 Meshes bei 15 Bones hat und die beiden ueberzaehligen die
 * GORE-Reserven sind (Slots 0..14 sind 1:1). Gegengemessen reproduziert HYB+K_mesh die
 * RE2-Bodenlage Zeile fuer Zeile (Clip 0: RE2 -21..+47, HYB heute -206..-129, HYB+K -21..+47).
 *
 * KEINE In-Place-Korrektur der Asset-Bytes: die Bank aliast das residente EMS, der
 * Keyframe-Pool wird von mehreren Kopien geteilt. Deshalb eine Seiten-Tabelle. */
#define RE2_ROOTFIX_POOL   40960u          /* int16-Eintraege; gemessener Bedarf s.u. */
static int16_t  s_rootfix_pool[RE2_ROOTFIX_POOL];
static uint32_t s_rootfix_used = 0;
static int      s_rootfix_overflow = 0;

void re2_hybrid_rootfix_reset(void)
{
    s_rootfix_used = 0;
    s_rootfix_overflow = 0;
}

int re2_hybrid_rootfix_stats(int *out_used, int *out_capacity)
{
    if (out_used)     *out_used     = (int)s_rootfix_used;
    if (out_capacity) *out_capacity = (int)RE2_ROOTFIX_POOL;
    return s_rootfix_overflow;
}

/* Tiefster Welt-Vertex einer Pose. PSX-Y zeigt nach UNTEN, groesser = tiefer.
 * Exakt die Renderer-Kette aus platform/pc/main.c (Bone-Klammer inkl. der mesh_count-Klemme
 * fuer den Nicht-Remap-Fall), damit gemessen wird, was auch gezeichnet wird. */
/* Tiefster BONE-URSPRUNG einer Pose. PSX-Y zeigt nach UNTEN, groesser = tiefer.
 *
 * ⛔ BONE, NICHT VERTEX — gemessen 2026-08-27, und das ist der Unterschied zwischen einem
 * Fix und einer neuen Regression:
 * Die Korrektur darf AUSSCHLIESSLICH den Bindlaengen-Tausch ausgleichen. Der Bone-Ursprung
 * haengt nur an Hierarchie + Bindlaengen + Rotationen — genau die drei Groessen, von denen
 * der Hybrid eine tauscht. Der tiefste VERTEX haengt zusaetzlich an der Mesh-Geometrie, und
 * die ist zwischen RE1.5 und RE2 nicht dieselbe.
 * Beim ZOMBIE faellt das kaum auf (Meshes 1:1, RE1.5-Sohle ~20 flacher). Beim HUND 0x20 ist es
 * fatal: Sonden-Sweep `probe_rig_sohle sweep 20` zeigt
 *     K_bone  +0..+3      (die Kette ist praktisch identisch)
 *     K_mesh  -93..+181   (die Pfoten-Meshes entsprechen sich nicht)
 * und sein HYB liegt schon OHNE Korrektur auf der RE2-Bodenlage (Clip 0: RE2 14..15,
 * HYB 13..15). Eine Mesh-basierte Korrektur haette den Hund um bis zu 238 Einheiten
 * verschoben, obwohl er nichts braucht — gemessen, nicht befuerchtet.
 * Restfehler von K_bone beim Zombie: die genuine Mesh-Tiefendifferenz (~20 Einheiten =
 * 0,7 % der Koerperhoehe). Das Ergebnis liegt damit innerhalb des Bandes, das der
 * RE1.5-Flavor mit seinen eigenen Clips selbst einnimmt (ROOM1030: +6..+127). */
static int32_t re2_lowest_bone_y(const re15_emd_skeleton_t *sk, const re15_md1_t *md,
                                 int has_remap, int kf, int *ok)
{
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor;
    g_anim_pose_actor = NULL;                       /* kein Crossfade waehrend der Messung */
    int rv = re15_skel_compute_pose(sk, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) { *ok = 0; return 0; }
    int nb = sk->bone_count;
    if (!has_remap && md && nb > md->mesh_count) nb = md->mesh_count;   /* main.c:6916-Klemme */
    if (nb > RE15_EMD_MAX_BONES) nb = RE15_EMD_MAX_BONES;
    if (nb <= 0) { *ok = 0; return 0; }
    int32_t best = -0x7fffffff;
    for (int b = 0; b < nb; b++)
        if (poses[b].trans[1] > best) best = poses[b].trans[1];
    *ok = 1;
    return best;
}

/* Eine Skelett-Kopie mit ihrer Korrektur-Tabelle versehen.
 * `hyb` traegt bereits die RE1.5-Bindlaengen, `re2bind` ist die gesicherte RE2-Fassung
 * DESSELBEN Skeletts (gleiche Hierarchie, gleicher Keyframe-Pool). */
static void re2_build_rootfix(re15_emd_skeleton_t *hyb, const re15_emd_skeleton_t *re2bind,
                              const re15_md1_t *md2, const re15_md1_t *md15,
                              const int8_t *remap)
{
    hyb->root_y_fix = NULL;
    hyb->root_y_fix_count = 0;
    int n = hyb->keyframe_count;
    if (n <= 0) return;
    if (s_rootfix_used + (uint32_t)n > RE2_ROOTFIX_POOL) { s_rootfix_overflow = 1; return; }
    int16_t *tab = &s_rootfix_pool[s_rootfix_used];
    for (int kf = 0; kf < n; kf++) {
        int oka = 0, okb = 0;
        /* Identische Rotationen, identische Hierarchie, identische Bone-Klammer — der
         * einzige Unterschied ist die Bindlaengen-Tabelle. Genau das soll K messen. */
        int32_t a = re2_lowest_bone_y(re2bind, md2,  0, kf, &oka);   /* RE2-Bindlaengen   */
        int32_t b = re2_lowest_bone_y(hyb,     md15, 1, kf, &okb);   /* RE1.5-Bindlaengen */
        int32_t k = (oka && okb) ? (a - b) : 0;
        if (k >  32767) k =  32767;
        if (k < -32768) k = -32768;
        tab[kf] = (int16_t)k;
    }
    s_rootfix_used += (uint32_t)n;
    hyb->root_y_fix = tab;
    hyb->root_y_fix_count = n;
}

int re2_hybrid_apply(re15_enemy_bank_t *eb, int kind,
                     const re15_md1_t *md15, const re15_emd_skeleton_t *skel15,
                     int *out_unmapped)
{
    if (out_unmapped) *out_unmapped = 0;
    if (!eb || !md15 || !skel15) return -1;
    const int8_t *perm = NULL;
    int n = re2_hybrid_perm(kind, &perm);
    if (n <= 0 || !perm)                       return -2;
    if (n != eb->skel.bone_count)              return -3;   /* RE2-Bone-Zahl muss passen */
    if (n > RE15_EMD_MAX_BONES)                return -4;
    for (int i = 0; i < n; i++) {
        int m = (int)perm[i];
        if (m >= skel15->bone_count)           return -5;
        if (m >= md15->mesh_count)             return -6;
    }

    /* Die RE2-Fassung der drei Skelette sichern, BEVOR die Bindlaengen ueberschrieben werden —
     * und das RE2-MD1, bevor es unten durch `eb->md1 = *md15` ersetzt wird. Beides ist die
     * Bezugsseite der Wurzelhoehen-Korrektur. Die Kopien halten nur Werte plus die Zeiger auf
     * den gemeinsamen Keyframe-Pool; es wird nichts dupliziert, was der Bank gehoert. */
    re15_emd_skeleton_t re2_skel      = eb->skel;
    re15_emd_skeleton_t re2_skel_loco = eb->skel_loco;
    re15_emd_skeleton_t re2_skel_own  = eb->skel_own;
    const re15_md1_t    re2_md1       = eb->md1;
    re2_skel.root_y_fix = NULL;      re2_skel.root_y_fix_count = 0;
    re2_skel_loco.root_y_fix = NULL; re2_skel_loco.root_y_fix_count = 0;
    re2_skel_own.root_y_fix = NULL;  re2_skel_own.root_y_fix_count = 0;

    int um = 0;
    um += re2_hybrid_rig_skel(&eb->skel,        perm, n, skel15);
    if (eb->loco_ok)   um += re2_hybrid_rig_skel(&eb->skel_loco,   perm, n, skel15);
    if (eb->own_ok)    um += re2_hybrid_rig_skel(&eb->skel_own,    perm, n, skel15);

    /* Erst JETZT die Korrektur-Tabellen bauen: `eb->skel*` traegt die RE1.5-Bindlaengen,
     * `re2_skel*` die RE2-Fassung — beide mit denselben Keyframes. skel_victim bleibt aussen
     * vor, der posiert LEON und wurde vom Hybrid bewusst nicht umgebaut. */
    {   const int8_t *rm = perm;
        re2_build_rootfix(&eb->skel, &re2_skel, &re2_md1, md15, rm);
        if (eb->loco_ok) re2_build_rootfix(&eb->skel_loco, &re2_skel_loco, &re2_md1, md15, rm);
        if (eb->own_ok)  re2_build_rootfix(&eb->skel_own,  &re2_skel_own,  &re2_md1, md15, rm);
    }
    /* skel_victim (Paar 3) posiert LEON, nicht den Gegner — sein Rig ist PL00-kompatibel
     * (RE1.5 PL00.PLD parent[] == RE2 EM010 parent[], selbst gemessen). Er bleibt daher
     * UNVERAENDERT: Leons Modell ist in beiden Modi dasselbe RE1.5-PL00. */

    for (int i = 0; i < RE15_EMD_MAX_BONES; i++)
        eb->mesh_remap[i] = (i < n) ? perm[i] : (int8_t)-1;
    eb->remap_ok = 1;
    eb->md1 = *md15;
    if (out_unmapped) *out_unmapped = um;
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

/* ===== ENEMSE-BANK-WAHL — byte-true FUN_80052b38 ========================
 *
 * ⚠️ STAND 2026-08-17: DER MECHANISMUS IST BYTE-TRUE, ABER (NOCH) NICHT VERDRAHTET.
 * Er ist hier abgelegt + unit-gepinnt (tests/unit/test_re2_enemse_bank.c), damit niemand
 * ihn erneut erarbeiten muss. Warum er das Spiel noch nicht steuert:
 *   Die Bank haengt am RAUM-Byte `Spawn-Record +7` (SOUND-ID), NICHT am Gegner-kind
 *   (`Spawn-Record +3`, siehe `jal 0x8001b710` @0x800571f0 + 0x10..0x1F-Klemme @0x8001b738-40;
 *   der Modell-Binder FUN_8001aaa8 liest den kind aus entity+0x8 @0x8001aac8, waehrend
 *   FUN_80052b38 entity+0x1FA = record+7 vergleicht). Beides sind VERSCHIEDENE Bytes, und die
 *   Paar-Tabelle @0x800a7400 fuehrt 15 ids < 0x10, die im kind-Raum (Minimum 0x10) nicht
 *   existieren. RE1.5-Raeume liefern kein record+7, und die RE2-Raumdaten liegen nicht im Repo
 *   (info/re2leon = COMMON/BIN + PL0 + ZMOVIE + PSX.EXE). Ein kind->Bank-Mapping waere damit
 *   geraten — deshalb bleiben die drei Brains bei ihren empirischen Bank-Konstanten
 *   (Begruendung + Refutation im Kopf von enemy_ai_re2_zombie.c).
 *
 * Original (RE2_Quellcode_V2/FUN_80052b38.c + Disasm @0x80052b40-0x80052c2c; Aufruf aus dem
 * Raum-Setup FUN_80053528 @0x80053610):
 *   pcVar5/cVar2 = Zeile.kindA, pcVar4 = Zeile.kindB, Schrittweite 2, Abbruch bei kindA == -1;
 *   DAT_800d424b (= die Bank-Nummer) zaehlt die Zeilen mit.
 *     (a) `cVar2 == DAT_800d8cd0 && (DAT_800d8cd1 == 0 || DAT_800d8cd1 == *pcVar4)` -> Treffer,
 *         bVar1 = false  (Raum-kindA liegt in der ERSTEN Haelfte)
 *     (b) `*pcVar4 == DAT_800d8cd0 && (DAT_800d8cd1 == 0 || DAT_800d8cd1 == cVar2)` -> Treffer,
 *         bVar1 = true   (Raum-kindA liegt in der ZWEITEN Haelfte)
 *   kein Treffer / DAT_800d8cd0 == 0 -> DAT_800d424b = 0xFF (Lader FUN_8005a09c bricht bei
 *   0xFF ab: `if (uVar4 == 0xff) break;` — dann gibt es GAR KEINE Gegner-Bank).
 *   Anschliessend Entity-Schleife @LAB_80052c2c: jede aktive Entity, deren kind (+0x1FA)
 *   die ZWEITE Haelfte des Treffers ist, bekommt `word0 |= 0x2000` — genau das Bit, das der
 *   SE-Trigger FUN_8005bd6c als "+0x10-Map-Haelfte" liest.
 * `*out_flag_kind` liefert deshalb den kind, der im Spiel das 0x2000-Bit traegt (-1 = keiner). */
int re2_enemse_select_bank(int kindA, int kindB, int *out_flag_kind)
{
    if (out_flag_kind) *out_flag_kind = -1;
    if (kindA <= 0) return 0xFF;                      /* `if (_DAT_800d8cd0 != 0)` @0x80052b44 */
    for (int bank = 0; bank < RE2_ENEMSE_BANK_COUNT + 1; bank++) {
        int k0 = s_re2_enemse_pairs[bank * 2], k1 = s_re2_enemse_pairs[bank * 2 + 1];
        if (k0 == 0xFF) break;                        /* `while (cVar2 != -1)` @0x80052bec */
        if (k0 == kindA && (kindB == 0 || kindB == k1)) {
            if (out_flag_kind) *out_flag_kind = k1;   /* bVar1 == false -> 2. Haelfte = kindB */
            return bank;
        }
        if (k1 == kindA && (kindB == 0 || kindB == k0)) {
            if (out_flag_kind) *out_flag_kind = kindA;/* bVar1 == true  -> 2. Haelfte = kindA */
            return bank;
        }
    }
    return 0xFF;                                      /* `DAT_800d424b = 0xff` @0x80052c24 */
}

int re2_enemse_pair_row(int bank, int *out_k0, int *out_k1)
{
    if (bank < 0 || bank >= RE2_ENEMSE_BANK_COUNT + 1) return -1;
    if (out_k0) *out_k0 = s_re2_enemse_pairs[bank * 2];
    if (out_k1) *out_k1 = s_re2_enemse_pairs[bank * 2 + 1];
    return 0;
}

/* Raum-kind-Paar (DAT_800d8cd0 / DAT_800d8cd1) — byte-true Enemy-Spawn @0x8005728c-0x800572b8
 * (identischer zweiter Pfad @0x80057920-0x8005794c):
 *      lbu v1,DAT_800d8cd0 ; beq v1,kind -> fertig            (schon kindA)
 *      beq v1,zero -> sb kind,DAT_800d8cd0                     (A noch leer -> A = kind)
 *      sb kind,DAT_800d8cd1                                    (sonst B = kind, ueberschreibend)
 * Clear beim Raum-Init FUN_80052f3c (`DAT_800d8cd0 = 0; DAT_800d8cd1 = 0;` @0x80053028/30). */
static uint8_t s_room_kindA = 0, s_room_kindB = 0;

void re2_enemse_room_reset(void) { s_room_kindA = 0; s_room_kindB = 0; }

void re2_enemse_room_add_kind(int kind)
{
    uint8_t k = (uint8_t)kind;
    if (k == 0) return;
    if (s_room_kindA == k) return;                    /* beq v1,v0 @0x80057298 */
    if (s_room_kindA == 0) { s_room_kindA = k; return; }  /* @0x800572a0 -> @0x800572b8 */
    s_room_kindB = k;                                 /* sb v0,DAT_800d8cd1 @0x800572ac */
}

void re2_enemse_room_kinds(int *out_a, int *out_b)
{
    if (out_a) *out_a = s_room_kindA;
    if (out_b) *out_b = s_room_kindB;
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
