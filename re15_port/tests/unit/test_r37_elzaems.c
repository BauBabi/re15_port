/* test_r37_elzaems.c — RIEGEL Spur N (Runde 37) "elzaems": das Modellarchiv JEDES Typs wie im Original.
 *
 * Dossier: analysis/befunde_runde37/N_elzaems.md. Belege: include/re15_elzaems_r37.h, engine/src/elzaems_r37.c,
 * platform/pc/src/roy_pc.c. Original: FUN_80022300 `bltz` @0x80022334 auf DAT_800aca3c Bit 31 -> Datei 0x26/0x27
 * (@0x8002233c/@0x800223ac), TOC @0x80072f38/@0x80073178; dasselbe Bit ist SCD-Flag (1,0) (Bank-Tabelle
 * @0x80074664[1] = 0x800aca3c), vier Raumskripte klammern damit einen Birkin-Spawn.
 *
 * Teile (je ein ctest-Eintrag):
 *   toc     beide TOCs gegen die Archive: jeder Typ mit Eintrag hat einen lesbaren Blob; die Port-Kette (re15_ems.c)
 *           liefert fuer CDEMD0 GENAU den TOC-Blob (Leon-Weg == Original), fuer CDEMD1 bei 0x30 NICHT (Sektor 2015,
 *           ans Ende angehaengt); Unterschiede je Typ (n1_zensus): 0x30 Loco-Clip 1 68 -> 27, 0x42 eigene Bank 6 -> 14
 *           Clips, Zombie-Opfer-Bank Clip 2 20 -> 36, 0x29 Opfer-Bank 4 -> 2 Clips, 0x2B 8 -> 11, 0x21 Clip 1 36 -> 31,
 *           0x27 Clip 0 70 -> 63; zehn Typen bytegleich; CRC32 der CDEMD1-Blobs = Werte der debug.log-Zeile.
 *   weiche  Zwilling des Elza-Bits: Figur Leon/Elza, Set(1,0,v) ueber re15_game_flag_set, Figurwechsel verwirft
 *           den Skriptwert.
 *   raeume  Raum-Init am echten VM (scd_room_reenter, main00+sub00): je gespawntem Typ das gemerkte Archiv und das
 *           Bit danach — Elza 3071/50F1 Birkin CDEMD0, Elza 5081 Birkin CDEMD1, Elza 3091 0x42/0x49 CDEMD1,
 *           Elza 1101 Zombie 0x10 CDEMD1; Leon 5080 Birkin CDEMD1 (Original, GDB-Lauf g2), Leon 3070/50E0 Birkin
 *           CDEMD0, Leon 1100 Zombie 0x10 CDEMD0. */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_fade.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_tim.h"
#include "re15_gameflow.h"
#include "re15_roy_r37.h"
#include "re15_elzaems_r37.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (n) *n = (size_t)sz;
    return b;
}

static uint8_t *s_ems[2];
static size_t   s_ems_n[2];
static int archive_laden(void)
{
    for (int a = 0; a < 2; a++) {
        if (s_ems[a]) continue;
        char p[600]; snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, re15_ems_archiv_datei(a));
        s_ems[a] = slurp(p, &s_ems_n[a]);
        if (!s_ems[a]) { printf("  FEHLER: %s nicht lesbar\n", p); g_fail++; return -1; }
    }
    return 0;
}

/* Blob `type` nach TOC (dieselbe Rechnung wie roy_pc.c: Sektor*2048, TOC-Laenge). */
static const uint8_t *toc_blob(int archiv, uint8_t type, size_t *len)
{
    uint32_t s = 0, l = 0;
    if (!re15_ems_toc(archiv, type, &s, &l)) return NULL;
    if ((size_t)s * 2048u + l > s_ems_n[archiv]) return NULL;
    if (len) *len = l;
    return s_ems[archiv] + (size_t)s * 2048u;
}

/* CRC32 wie platform/pc/src/roy_pc.c re15_elzaems_crc32 (IEEE, reflektiert). */
static uint32_t crc32(const uint8_t *p, size_t n)
{
    uint32_t c = 0xFFFFFFFFu;
    for (size_t i = 0; i < n; i++) {
        c ^= p[i];
        for (int k = 0; k < 8; k++) c = (c >> 1) ^ (0xEDB88320u & (0u - (c & 1u)));
    }
    return ~c;
}

static int fc(const re15_emd_animation_t *a, int c) { return (a && c < a->clip_count) ? a->clips[c].frame_count : -1; }

typedef struct {
    re15_md1_t md1; re15_emd_skeleton_t sk, skl, skv, sko;
    re15_emd_animation_t an, anl, anv, ano; int ok, lok, vok, ook;
} banken_t;
static int banken(const uint8_t *b, size_t n, banken_t *k)
{
    static uint8_t buf[0x50000];
    if (n > sizeof buf) return -1;
    memcpy(buf, b, n);
    re15_tim_t tim = (re15_tim_t){0};
    k->ok  = re15_emd_parse_container(buf, n, &k->md1, &k->sk, &k->an, &tim) == 0;
    k->lok = re15_emd_parse_loco_bank(buf, n, &k->skl, &k->anl) == 0;
    k->vok = re15_emd_parse_victim_bank(buf, n, &k->skv, &k->anv) == 0;
    k->ook = re15_emd_parse_own_bank(buf, n, &k->sko, &k->ano) == 0;
    return k->ok ? 0 : -1;
}

/* Beide Archive eines Typs parsen (je ein eigener Puffer: die Banken zeigen in ihren Blob). */
static int beide(uint8_t typ, banken_t *L, banken_t *E)
{
    static uint8_t b0[0x50000], b1[0x50000];
    size_t n0 = 0, n1 = 0;
    const uint8_t *p0 = toc_blob(0, typ, &n0);
    const uint8_t *p1 = toc_blob(1, typ, &n1);
    if (!p0 || !p1 || n0 > sizeof b0 || n1 > sizeof b1) return -1;
    memcpy(b0, p0, n0); memcpy(b1, p1, n1);
    re15_tim_t t0 = (re15_tim_t){0}, t1 = (re15_tim_t){0};
    L->ok  = re15_emd_parse_container(b0, n0, &L->md1, &L->sk, &L->an, &t0) == 0;
    L->lok = re15_emd_parse_loco_bank(b0, n0, &L->skl, &L->anl) == 0;
    L->vok = re15_emd_parse_victim_bank(b0, n0, &L->skv, &L->anv) == 0;
    L->ook = re15_emd_parse_own_bank(b0, n0, &L->sko, &L->ano) == 0;
    E->ok  = re15_emd_parse_container(b1, n1, &E->md1, &E->sk, &E->an, &t1) == 0;
    E->lok = re15_emd_parse_loco_bank(b1, n1, &E->skl, &E->anl) == 0;
    E->vok = re15_emd_parse_victim_bank(b1, n1, &E->skv, &E->anv) == 0;
    E->ook = re15_emd_parse_own_bank(b1, n1, &E->sko, &E->ano) == 0;
    if (!(L->ok && E->ok)) { printf("  FEHLER: Typ 0x%02X parst nicht\n", typ); g_fail++; return -1; }
    return 0;
}

/* ---- toc ----------------------------------------------------------------------------------------------- */
static void teil_toc(void)
{
    printf("== toc ==\n");
    if (archive_laden() != 0) return;
    /* (1) jeder TOC-Eintrag beider Archive liegt im Archiv und parst; Port-Kette == TOC (Leon) */
    int eintraege = 0, kette_gleich = 0, kette_n = 0, parst = 0;
    for (int t = 0x10; t < 0x50; t++) {
        for (int a = 0; a < 2; a++) {
            size_t n = 0; const uint8_t *b = toc_blob(a, (uint8_t)t, &n);
            if (!b) continue;
            eintraege++;
            static banken_t k;
            if (banken(b, n, &k) == 0) parst++;
            else printf("  (Typ 0x%02X Archiv %d: Container parst nicht)\n", t, a);
        }
        int idx = re15_ems_index_for_type((uint8_t)t);
        size_t off = 0, len = 0, n0 = 0;
        const uint8_t *b0 = toc_blob(RE15_EMS_ARCHIV_LEON, (uint8_t)t, &n0);
        if (idx >= 0 && b0) {
            kette_n++;
            if (re15_ems_get_entry(s_ems[0], s_ems_n[0], idx, &off, &len) == 0 && s_ems[0] + off == b0 && len <= n0)
                kette_gleich++;
        }
    }
    PRUEF(eintraege == 50 && parst == 50, "50 TOC-Eintraege (25 Typen x 2 Archive) im Archiv, alle Container parsen (%d/%d)",
          parst, eintraege);
    PRUEF(kette_n == 25 && kette_gleich == 25,
          "Port-Kette CDEMD0 (re15_ems.c, Leon-Weg) == TOC-Blob @0x80072f38 fuer alle 25 Typen (%d/%d)", kette_gleich, kette_n);
    {   /* CDEMD1: Kettenplatz 16 ist NICHT der Birkin der TOC (Sektor 2015) */
        size_t off = 0, len = 0, n1 = 0;
        const uint8_t *b1 = toc_blob(RE15_EMS_ARCHIV_ELZA, 0x30, &n1);
        int k = re15_ems_get_entry(s_ems[1], s_ems_n[1], re15_ems_index_for_type(0x30), &off, &len);
        PRUEF(b1 && k == 0 && s_ems[1] + off != b1 && (size_t)(b1 - s_ems[1]) == 2015u * 2048u && n1 == 228460,
              "CDEMD1 Typ 0x30: TOC Sektor 2015 / 228460 B (@0x80073178[0x20]); Kettenplatz 16 liegt @0x%06X = anderer Blob",
              (unsigned)off);
    }
    /* (2) bytegleiche Typen */
    static const uint8_t gleich[] = { 0x1A, 0x24, 0x26, 0x2D, 0x36, 0x40, 0x45, 0x49, 0x4B, 0x4D };
    int ng = 0;
    for (unsigned i = 0; i < sizeof gleich; i++) {
        size_t n0 = 0, n1 = 0;
        const uint8_t *b0 = toc_blob(0, gleich[i], &n0), *b1 = toc_blob(1, gleich[i], &n1);
        if (b0 && b1 && n0 == n1 && !memcmp(b0, b1, n0)) ng++;
    }
    PRUEF(ng == 10, "zehn Typen in beiden Archiven bytegleich (0x1A 0x24 0x26 0x2D 0x36 0x40 0x45 0x49 0x4B 0x4D): %d/10", ng);
    /* (3) die Unterschiede, die der Spieler sieht (n1_zensus) */
    static banken_t L, E;
#define BEIDE(typ) (beide((typ), &L, &E) == 0)
    if (BEIDE(0x30))
        PRUEF(fc(&L.anl, 1) == 68 && fc(&E.anl, 1) == 27 && L.md1.mesh_count == E.md1.mesh_count,
              "0x30 Birkin: Loco-Clip 1 CDEMD0 %d / CDEMD1 %d (68/27), Meshes %d/%d", fc(&L.anl, 1), fc(&E.anl, 1),
              L.md1.mesh_count, E.md1.mesh_count);
    if (BEIDE(0x42))
        PRUEF(L.ano.clip_count == 6 && E.ano.clip_count == 14 && fc(&E.ano, 13) == 32,
              "0x42: eigene Bank dir[3] CDEMD0 %d / CDEMD1 %d Clips (6/14), CDEMD1 Clip 13 = %d (32)",
              L.ano.clip_count, E.ano.clip_count, fc(&E.ano, 13));
    if (BEIDE(0x10))
        PRUEF(fc(&L.anv, 2) == 20 && fc(&E.anv, 2) == 36 && fc(&L.anv, 6) == 65 && fc(&E.anv, 6) == 70 &&
              fc(&L.an, 29) == 29 && fc(&E.an, 29) == 41,
              "0x10 Zombie: Opfer-Bank Clip 2 %d/%d (20/36), Clip 6 %d/%d (65/70); Aktionsbank Clip 29 %d/%d (29/41)",
              fc(&L.anv, 2), fc(&E.anv, 2), fc(&L.anv, 6), fc(&E.anv, 6), fc(&L.an, 29), fc(&E.an, 29));
    if (BEIDE(0x29))
        PRUEF(L.anv.clip_count == 4 && E.anv.clip_count == 2 && fc(&E.anv, 1) == 110,
              "0x29: Opfer-Bank CDEMD0 %d / CDEMD1 %d Clips (4/2), CDEMD1 Clip 1 = %d (110)",
              L.anv.clip_count, E.anv.clip_count, fc(&E.anv, 1));
    if (BEIDE(0x2B))
        PRUEF(L.anv.clip_count == 8 && E.anv.clip_count == 11, "0x2B: Opfer-Bank %d / %d Clips (8/11)",
              L.anv.clip_count, E.anv.clip_count);
    if (BEIDE(0x21))
        PRUEF(fc(&L.anv, 1) == 36 && fc(&E.anv, 1) == 31, "0x21 Kraehe: Opfer-Clip 1 %d/%d (36/31)", fc(&L.anv, 1), fc(&E.anv, 1));
    if (BEIDE(0x27))
        PRUEF(fc(&L.anv, 0) == 70 && fc(&E.anv, 0) == 63, "0x27: Opfer-Clip 0 %d/%d (70/63)", fc(&L.anv, 0), fc(&E.anv, 0));
    /* (3b) RE2-KI-Standard (WELLE-G-Hybrid, platform/pc/main.c pc_enemy_hybrid_re15_models): von den RE1.5-Daten
     * liest der Hybrid NUR MD1 + TIM + die Bind-Lagen/Hierarchie des Container-Skeletts (re2_hybrid_apply) und fuer
     * Typ 0x10 die Sitz-Clips 0x29/0x2A der Aktions-Bank (SITZ-IMPORT, RE1.5-INIT @0x80100F64-FD4). Was davon
     * unterscheidet sich zwischen den Archiven? (Messung; die Clips/Opfer-Banken kommen im Hybrid aus RE2.) */
    {
        static const uint8_t hyb[] = { 0x10, 0x11, 0x12, 0x13, 0x16, 0x18, 0x20, 0x21, 0x25 };
        int md1_gleich = 0, bind_gleich = 0, n = 0;
        for (unsigned i = 0; i < sizeof hyb; i++) {
            if (!BEIDE(hyb[i])) continue;
            n++;
            int bg = L.sk.bone_count == E.sk.bone_count &&
                     !memcmp(L.sk.bone_relative_pos, E.sk.bone_relative_pos, sizeof L.sk.bone_relative_pos) &&
                     !memcmp(L.sk.bone_parent, E.sk.bone_parent, sizeof L.sk.bone_parent);
            int mg = L.md1.mesh_count == E.md1.mesh_count;
            bind_gleich += bg; md1_gleich += mg;
            if (!bg || !mg) printf("  info: Hybrid-Eingang Typ 0x%02X: Bind %s, MD1-Meshes %d/%d\n", hyb[i],
                                   bg ? "gleich" : "VERSCHIEDEN", L.md1.mesh_count, E.md1.mesh_count);
        }
        PRUEF(n == 9 && bind_gleich == 9 && md1_gleich == 9,
              "RE2-KI-Hybrid: Bind-Lagen + Hierarchie + MD1 der neun RE2-KI-Typen in beiden Archiven gleich (%d/%d, %d/%d)",
              bind_gleich, n, md1_gleich, n);
        if (BEIDE(0x10)) {
            int sitz_gleich = 1;
            for (int c = 0x29; c <= 0x2A; c++) {
                if (fc(&L.an, c) != fc(&E.an, c)) { sitz_gleich = 0; continue; }
                for (int f = 0; f < L.an.clips[c].frame_count; f++) {
                    int kl = (int)(L.an.frames[L.an.clips[c].first_frame + f] & 0xFFFu);
                    int ke = (int)(E.an.frames[E.an.clips[c].first_frame + f] & 0xFFFu);
                    if (memcmp(L.sk.keyframe_data + (size_t)kl * L.sk.keyframe_size_bytes,
                               E.sk.keyframe_data + (size_t)ke * E.sk.keyframe_size_bytes,
                               (size_t)L.sk.keyframe_size_bytes)) sitz_gleich = 0;
                }
            }
            printf("  info: 0x10 Sitz-Clips 0x29/0x2A (SITZ-IMPORT im RE2-KI-Hybrid): Bilder %d/%d und %d/%d, Posen %s\n",
                   fc(&L.an, 0x29), fc(&E.an, 0x29), fc(&L.an, 0x2A), fc(&E.an, 0x2A), sitz_gleich ? "GLEICH" : "VERSCHIEDEN");
        }
    }
#undef BEIDE
    /* (4) CRC32 der CDEMD1-Blobs = die Werte, die roy_pc.c in die debug.log schreibt (Integration prueft sie) */
    static const struct { uint8_t typ; uint32_t crc; } k_crc[] = {
        { 0x30, 0x1145F206u }, { 0x42, 0x91415329u }, { 0x49, 0x285C67D3u }, { 0x10, 0xAE292DA8u },
        { 0x11, 0x06E1989Eu }, { 0x47, 0x04772311u } };
    for (unsigned i = 0; i < sizeof k_crc / sizeof k_crc[0]; i++) {
        size_t n = 0; const uint8_t *b = toc_blob(RE15_EMS_ARCHIV_ELZA, k_crc[i].typ, &n);
        uint32_t c = b ? crc32(b, n) : 0;
        if (k_crc[i].crc) PRUEF(c == k_crc[i].crc, "CRC32 CDEMD1 Typ 0x%02X = %08X (erwartet %08X)", k_crc[i].typ,
                                (unsigned)c, (unsigned)k_crc[i].crc);
        else printf("  info: CRC32 CDEMD1 Typ 0x%02X = %08X (%u B)\n", k_crc[i].typ, (unsigned)c, (unsigned)n);
    }
}

/* ---- weiche -------------------------------------------------------------------------------------------- */
static void teil_weiche(void)
{
    printf("== weiche ==\n");
    re15_game_state_init();
    re15_elzaems_r37_reset();
    g_current_room_id = 0;
    g_gameflow.character = 0;
    int leon_alle = 1, elza_alle = 1;
    for (int t = 0x10; t < 0x50; t++) if (re15_ems_archiv_fuer_typ((uint8_t)t) != RE15_EMS_ARCHIV_LEON) leon_alle = 0;
    PRUEF(re15_elzabit() == 0 && leon_alle, "Leon (Figur 0): Elza-Bit 0, jeder Typ aus CDEMD0 (Datei 0x26 @0x8002233c)");
    g_gameflow.character = 4;
    for (int t = 0x10; t < 0x50; t++) if (re15_ems_archiv_fuer_typ((uint8_t)t) != RE15_EMS_ARCHIV_ELZA) elza_alle = 0;
    PRUEF(re15_elzabit() == 1 && elza_alle, "Elza (Figur 4): Elza-Bit 1, jeder Typ aus CDEMD1 (Datei 0x27 @0x800223ac)");
    re15_game_flag_set(1, 0, 0);
    PRUEF(re15_elzabit() == 0 && re15_ems_archiv_aktiv() == RE15_EMS_ARCHIV_LEON,
          "Elza + Set(1,0,0) (Bank-Tabelle @0x80074664[1] = 0x800aca3c, Maske 0x80000000): Bit 0 -> CDEMD0");
    re15_game_flag_set(1, 0, 1);
    PRUEF(re15_elzabit() == 1, "Elza + Set(1,0,1): Bit wieder 1");
    re15_game_flag_set(1, 0, 0);
    g_gameflow.character = 0;
    PRUEF(re15_elzabit() == 0, "Figurwechsel nach einem Elza-Skriptwert: Leon liest seine Figur (kein Rest)");
    re15_game_flag_set(1, 0, 1);
    PRUEF(re15_elzabit() == 1 && re15_ems_archiv_aktiv() == RE15_EMS_ARCHIV_ELZA,
          "Leon + Set(1,0,1) (ROOM5080 @0x00742): Bit 1 -> CDEMD1");
    re15_game_flag_set(1, 0, 0);
    PRUEF(re15_elzabit() == 0, "Leon + Set(1,0,0) (@0x0075A): Bit 0");
    PRUEF(re15_game_flag_get(1, 27) == 0, "Nachbar-Bit (1,27) unberuehrt");
    re15_elzaems_r37_reset();
}

/* ---- raeume (Raum-Init am VM) ------------------------------------------------------------------------- */
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;

static int raum_init(uint16_t room, int figur, const uint8_t (*flags)[2], int nflags)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) { printf("  FEHLER: %s\n", rp); g_fail++; return -1; }
    re15_game_state_init();
    re15_elzaems_r37_reset();
    g_gameflow.character = (uint8_t)figur;
    re15_actor_init(); re15_aot_init(); scd_vm_init();   /* scd_vm_init -> re15_game_state_init: Flags danach */
    for (int i = 0; i < nflags; i++) re15_game_flag_set(flags[i][0], flags[i][1], 1);
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->z = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    return 0;
}

static int typ_da(uint8_t type)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == type) n++;
    return n;
}

static void raum_ex(uint16_t room, uint8_t type, int erwartet, const char *beleg,
                    const uint8_t (*flags)[2], int nflags, int bilder)
{
    int figur = (room & 1u) ? 4 : 0;
    if (raum_init(room, figur, flags, nflags) != 0) return;
    for (int b = 0; b < bilder; b++) scd_vm_tick();
    int n = typ_da(type);
    int a = re15_ems_archiv_fuer_typ(type);
    PRUEF(n > 0 && a == erwartet && re15_elzabit() == (figur ? 1 : 0),
          "%s ROOM%04X: %d x Typ 0x%02X gespawnt, Archiv beim Sce_em_set %s (%s); Elza-Bit nach dem Init %d (= Figur)",
          figur ? "Elza" : "Leon", (unsigned)room, n, (unsigned)type, a == RE15_EMS_ARCHIV_ELZA ? "CDEMD1" : "CDEMD0",
          beleg, re15_elzabit());
}

static void raum(uint16_t room, uint8_t type, int erwartet, const char *beleg)
{
    raum_ex(room, type, erwartet, beleg, NULL, 0, 0);
}

static void teil_raeume(void)
{
    printf("== raeume ==\n");
    raum(0x3071, 0x30, RE15_EMS_ARCHIV_LEON, "Set(1,0,0) @0x03430 vor Sce_em_set @0x03434; Original g1: Datei 0x26, Blob == CDEMD0");
    raum(0x50F1, 0x30, RE15_EMS_ARCHIV_LEON, "Set(1,0,0) @0x009A8 vor @0x009AC; Original g3: Datei 0x26");
    raum(0x5081, 0x30, RE15_EMS_ARCHIV_ELZA, "kein Set(1,0,v); Original g3/g4: Datei 0x27, Sektor-Feld 5642");
    raum(0x3091, 0x42, RE15_EMS_ARCHIV_ELZA, "Sherry, Sce_em_set @0x014F4; Original g5: Datei 0x27, Blob == CDEMD1");
    raum(0x3091, 0x49, RE15_EMS_ARCHIV_ELZA, "John @0x014E0 (Blob in beiden Archiven gleich)");
    raum(0x1101, 0x10, RE15_EMS_ARCHIV_ELZA, "Zombies @0x009F6..; Original g1: Datei 0x27, Blob == CDEMD1");
    raum(0x5080, 0x30, RE15_EMS_ARCHIV_ELZA, "LEON: Set(1,0,1) @0x00742 vor @0x00746; Original g2: Datei 0x27, Blob == CDEMD1");
    raum(0x3070, 0x30, RE15_EMS_ARCHIV_LEON, "Leon ohne Umschalter (n1_bit31_zensus)");
    raum(0x50E0, 0x30, RE15_EMS_ARCHIV_LEON, "Leon ohne Umschalter");
    raum(0x1100, 0x10, RE15_EMS_ARCHIV_LEON, "Leon-Gegenprobe Zombies (Nutzer-Staende: CDEMD0)");
    raum(0x1011, 0x47, RE15_EMS_ARCHIV_ELZA, "Roy (Spur K) unveraendert");
    raum(0x11B1, 0x42, RE15_EMS_ARCHIV_ELZA, "0x42 geparkt @0x01058; Nutzer-Stand _6: EMR2/EDD3/EMR4 == CDEMD1");
    raum(0x10C1, 0x21, RE15_EMS_ARCHIV_ELZA, "Kraehen; Opfer-Bank CDEMD1 Clip 1 31 / Clip 2 30");
    raum(0x10C0, 0x21, RE15_EMS_ARCHIV_LEON, "Leon-Gegenprobe Kraehen (Nutzer-Stand _9.bak: Opfer-Bank == CDEMD0)");
    {   static const uint8_t f242[1][2] = { { 4, 242 } };   /* Ck(4,242) @0x00B4A -> Sce_em_set @0x00B54/@0x00B68 */
        raum_ex(0x1141, 0x27, RE15_EMS_ARCHIV_ELZA, "0x27 mit Ck(4,242); Nutzer-Stand _7.bak: Opfer-Bank == CDEMD1", f242, 1, 0); }
    raum(0x5091, 0x36, RE15_EMS_ARCHIV_ELZA, "0x30 -> 0x36 (RE2-EM36, NUTZER-VORGABE) - kein Set(1,0,v) in 5091, Bit = Figur");
    /* Leon ROOM1170: Elliot spawnt erst in sub15 nach dem Montage-Gate (unit_r37_roy_leon1170 / integration_r37_roy_leon1170
     * pruefen ihn mit Bank und Zeichenweg) — hier nur die Weiche nach dem Raum-Init. */
    if (raum_init(0x1170, 0, NULL, 0) == 0)
        PRUEF(re15_typ47_ist_elliot(0x47) && !re15_roy_aus_elza_archiv(0x47) &&
              re15_ems_archiv_fuer_typ(0x47) == RE15_EMS_ARCHIV_LEON,
              "Leon ROOM1170: Typ 0x47 = Elliot-Sonderweg (Leon-Archiv), kein Elza-Archiv");
}

/* ---- griff (RE1.5-KI: Zombie-Griff an der Figur, mit der Bank, die der Port JETZT laedt) -----------------
 * J-Messung (J_bissarm.md 2.3, Sonde re15_kontakt der Spur J, Zweig r37/bissarm): RE1.5-Griff FUN_80102548,
 * Biss-Schleife +0x6 == 3 @0x801027dc; Hand rechts 10 / links 13 des Greifers gegen die Schulter links 12 /
 * rechts 9 des Opfers; Opfer-Skelett = PLD/PL00.EMR (Leon) bzw. PLD/PL04.EMR (Elza) mit den Keyframes der
 * Opfer-Bank (wie main.c s_victim_skel), Mittel ueber die Biss-Bilder im Greiferrahmen. Gleicher Aufbau wie J
 * (ROOM1140, Opfer (-1000,-19600), Greifer 900 davor/dahinter), EIN Unterschied: der Blob kommt NICHT fest aus
 * einer Datei, sondern aus der Port-Weiche — Archiv re15_ems_archiv_fuer_typ(Typ) nach dem Raum-Init (gemerkt
 * beim Sce_em_set, jal 0x80022300 @0x80042328), Blob per TOC wie platform/pc/src/roy_pc.c, geparst wie
 * pc_enemy_load_ex (Container + Loco + Opfer + eigene Bank). `zwang` >= 0 erzwingt ein Archiv (= Stand VORHER:
 * 99b4b13f gab Elza CDEMD0). */
#include "re15_ai_flavor.h"
#include "re15_damage.h"
#include "re15_inventory.h"
#include "re15_skeleton.h"
extern void *g_anim_pose_actor;
extern void re15_player_aim_reset(void);             /* engine/src/player_common.c (kein Header) */
extern void re15_player_set_aim_clip_len(int fc);
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static re15_emd_skeleton_t s_opfer_sk;

static void bild(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
}

static int kf_von(const re15_emd_animation_t *a, int clip, int frame)
{
    if (!a || clip < 0 || clip >= a->clip_count) return -1;
    const re15_emd_clip_t *c = &a->clips[clip];
    if (c->frame_count <= 0) return -1;
    int fi = c->first_frame + (frame % c->frame_count), fend = c->first_frame + c->frame_count - 1;
    while ((a->frames[fi] & 0x8000u) && fi < fend) fi++;
    return (int)(a->frames[fi] & 0xFFFu);
}

static void in_greiferrahmen(int16_t yaw, int32_t v[3])
{
    int64_t cs = re15_cos_q12(yaw), sn = re15_sin_q12(yaw);
    int32_t x = (int32_t)((cs * v[0] - sn * v[2]) >> 12), z = (int32_t)((sn * v[0] + cs * v[2]) >> 12);
    v[0] = x; v[2] = z;
}

/* Raum wie J (scd_thread_start main + sub0, 120 VM-Bilder), Figur VOR dem Spawn gesetzt; dann je Typ die Bank
 * aus dem Archiv der Port-Weiche (oder `zwang`). Rueckgabe: Archiv der Bank von `typ` (-1 = Fehler). */
static uint8_t s_rmb_flag[2];                          /* optional EIN Skript-Flag vor dem Raum-Init (Bank, Bit) */
static int     s_rmb_flag_an = 0;
static int raum_mit_banken(uint16_t room, int figur, int zwang, uint8_t typ)
{
    if (archive_laden() != 0) return -1;
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || re15_rdt_parse(s_raw, s_rawsz, &s_rdt) != 0) { printf("  FEHLER: %s\n", rp); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    re15_game_state_init();
    re15_elzaems_r37_reset();
    g_gameflow.character = (uint8_t)figur;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();   /* scd_vm_init -> re15_game_state_init: Flags danach */
    if (s_rmb_flag_an) re15_game_flag_set(s_rmb_flag[0], s_rmb_flag[1], 1);
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    re15_pauseflags_clear();
    g_current_room_id = room; g_room_change.pending = 0;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    int archiv_typ = -1;
    for (int sl = 1; sl < RE15_ACTOR_MAX; sl++) {
        if (!g_actors[sl].active) continue;
        uint8_t t = g_actors[sl].type;
        if (t < 0x10 || t >= 0x50 || re15_enemy_find(t)) continue;
        int ar = zwang >= 0 ? zwang : re15_ems_archiv_fuer_typ(t);
        size_t n = 0; const uint8_t *b = toc_blob(ar, t, &n);
        if (!b) continue;
        re15_enemy_bank_t *eb = re15_enemy_alloc(t);
        if (!eb) continue;
        uint8_t *blob = (uint8_t *)malloc(n); memcpy(blob, b, n);
        re15_tim_t tim = (re15_tim_t){0};
        if (re15_emd_parse_container(blob, n, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) { free(blob); eb->type = 0; continue; }
        eb->buf = blob;                                        /* re15_enemy_reset gibt ihn frei */
        eb->loco_ok   = (re15_emd_parse_loco_bank(blob, n, &eb->skel_loco, &eb->anim_loco) == 0);
        eb->victim_ok = (re15_emd_parse_victim_bank(blob, n, &eb->skel_victim, &eb->anim_victim) == 0);
        eb->own_ok    = (re15_emd_parse_own_bank(blob, n, &eb->skel_own, &eb->anim_own) == 0);
        eb->ok = 1;
        if (t == typ) archiv_typ = ar;
    }
    char ep[600]; size_t en = 0;
    snprintf(ep, sizeof ep, "%s/PLD/%s.EMR", RE15_ASSET_PSX_DIR, figur ? "PL04" : "PL00");
    static uint8_t *s_emr = NULL; free(s_emr); s_emr = slurp(ep, &en);
    if (!s_emr || re15_emd_parse_skeleton(s_emr, en, &s_opfer_sk) != 0) { printf("  FEHLER: %s\n", ep); g_fail++; return -1; }
    return archiv_typ;
}

/* Ein Griff wie J re15_kontakt; m[a][i] = Mittel Hand - Schulter (a 0: Hand10-Schulter12, 1: Hand13-Schulter9). */
static int griff_lauf(int figur, int zwang, int hinten, double m[2][3], int *archiv)
{
    *archiv = raum_mit_banken(0x1140, figur, zwang, 0x10);
    if (*archiv < 0) {
        printf("  FEHLER: ROOM1140: kein Typ 0x10 mit Bank (aktiv %d, Threads %d/%d)" "\n", typ_da(0x10),
               (int)g_scd.threads[0].active, (int)g_scd.threads[1].active);
        g_fail++; return -1;
    }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0); re15_player_set_aim_clip_len(12); re15_inv_load_briefing();
    int zA = -1;
    for (int sl = 1; sl < RE15_ACTOR_MAX; sl++)
        if (g_actors[sl].active && g_actors[sl].type == 0x10 && zA < 0) zA = sl;
    if (zA < 0) { printf("  FEHLER: kein Typ 0x10 in ROOM1140\n"); g_fail++; return -1; }
    for (int sl = 1; sl < RE15_ACTOR_MAX; sl++)
        if (sl != zA && g_actors[sl].active && g_actors[sl].type >= 0x10 && g_actors[sl].type < 0x20) g_actors[sl].active = 0;
    re15_actor_t *z = &g_actors[zA];
    re15_enemy_bank_t *zb = re15_enemy_find(z->type);
    if (!zb || !zb->ok || !zb->victim_ok) { printf("  FEHLER: Bank/Opfer-Bank 0x10 fehlt\n"); g_fail++; return -1; }
    const int32_t px = -1000, pz = -19600; const int rot = 0, abst = 900;
    int32_t sn = re15_sin_q12(rot), cs = re15_cos_q12(rot);
    int vz = hinten ? -1 : 1;
    pl->x = px; pl->z = pz; pl->rot_y = (int16_t)rot;
    z->x = px + vz * (int32_t)(((int64_t)cs * abst) >> 12); z->z = pz - vz * (int32_t)(((int64_t)sn * abst) >> 12);
    z->y = 0; z->rot_y = (int16_t)((rot + (hinten ? 0 : 2048)) & 0x0fff);
    int bilder = 0, erster = -1;
    double sum[2][3] = { { 0 } };
    for (int f = 0; f < 500; f++) {
        int v = re15_player_victim_state();
        if (v == 0 && !(pl->hit_react & 1)) { pl->x = px; pl->z = pz; pl->rot_y = (int16_t)rot; }
        if (pl->hp < 60) pl->hp = 100;
        bild();
        v = re15_player_victim_state();
        if (v != 1) { if (erster >= 0 && f > erster + 60) break; continue; }
        if (erster < 0) erster = f;
        if (z->state != 1 || (z->sub_state_1 != 3 && z->sub_state_1 != 4) || z->sub_state_2 != 3) continue;
        static re15_skel_pose_t ZP[RE15_EMD_MAX_BONES], LP[RE15_EMD_MAX_BONES];
        int kz = kf_von(&zb->anim, z->motion, z->anim_frame);
        static re15_emd_skeleton_t vs; vs = s_opfer_sk;                  /* main.c s_victim_skel */
        vs.keyframe_data = zb->skel_victim.keyframe_data; vs.keyframe_data_size = zb->skel_victim.keyframe_data_size;
        vs.keyframe_count = zb->skel_victim.keyframe_count; vs.keyframe_size_bytes = zb->skel_victim.keyframe_size_bytes;
        int kl = kf_von(&zb->anim_victim, pl->motion, pl->anim_frame);
        void *sav = g_anim_pose_actor; g_anim_pose_actor = NULL;
        int ok = (kz >= 0 && kl >= 0 && re15_skel_compute_pose(&zb->skel, kz, ZP) == 0 && re15_skel_compute_pose(&vs, kl, LP) == 0);
        g_anim_pose_actor = sav;
        if (!ok) continue;
        int32_t h[2][3], sch[2][3];
        re15_skel_bone_to_world(ZP[10].trans, z->rot_y, z->x, z->y, z->z, h[0]);
        re15_skel_bone_to_world(ZP[13].trans, z->rot_y, z->x, z->y, z->z, h[1]);
        re15_skel_bone_to_world(LP[12].trans, pl->rot_y, pl->x, pl->y, pl->z, sch[0]);
        re15_skel_bone_to_world(LP[9].trans,  pl->rot_y, pl->x, pl->y, pl->z, sch[1]);
        for (int a = 0; a < 2; a++) {
            int32_t vv[3] = { h[a][0] - sch[a][0], h[a][1] - sch[a][1], h[a][2] - sch[a][2] };
            in_greiferrahmen(z->rot_y, vv);
            for (int i = 0; i < 3; i++) sum[a][i] += vv[i];
        }
        bilder++;
    }
    if (bilder == 0) { printf("  FEHLER: keine Biss-Schleife (RE1.5-Griff) erreicht\n"); g_fail++; return -1; }
    for (int a = 0; a < 2; a++) for (int i = 0; i < 3; i++) m[a][i] = sum[a][i] / bilder;
    printf("  GRIFF %s %s, Bank aus %s: %d Biss-Bilder | Hand10-Schulter12 (%d,%d,%d) | Hand13-Schulter9 (%d,%d,%d)\n",
           figur ? "ELZA" : "LEON", hinten ? "HINTEN" : "VORN", *archiv ? "CDEMD1" : "CDEMD0", bilder,
           (int)m[0][0], (int)m[0][1], (int)m[0][2], (int)m[1][0], (int)m[1][1], (int)m[1][2]);
    return bilder;
}

static void teil_griff(void)
{
    printf("== griff ==\n");
    for (int hinten = 0; hinten < 2; hinten++) {
        double e[2][3], v[2][3], l[2][3], l0[2][3]; int ae = -1, av = -1, al = -1, al0 = -1;
        int ne = griff_lauf(4, -1, hinten, e, &ae);                 /* Elza, Port-Weiche (NACHHER) */
        int nv = griff_lauf(4, RE15_EMS_ARCHIV_LEON, hinten, v, &av);  /* Elza mit Leons Bank (VORHER) */
        int nl = griff_lauf(0, -1, hinten, l, &al);                 /* Leon, Port-Weiche */
        int nl0 = griff_lauf(0, RE15_EMS_ARCHIV_LEON, hinten, l0, &al0);  /* Leon, CDEMD0 fest (Stand vorher) */
        if (ne <= 0 || nv <= 0 || nl <= 0 || nl0 <= 0) continue;
        const char *s = hinten ? "hinten" : "vorn";
        int ye = (int)(e[0][1] < e[1][1] ? e[0][1] : e[1][1]);      /* hoechste Hand (Y negativ = oben) */
        int yv = (int)(v[0][1] > v[1][1] ? v[0][1] : v[1][1]);      /* tiefste der beiden Haende vorher */
        PRUEF(ae == RE15_EMS_ARCHIV_ELZA, "Elza %s: die Port-Weiche gibt dem Zombie 0x10 die CDEMD1-Bank (Datei 0x27 @0x800223ac)", s);
        PRUEF(ye >= -70 && e[0][1] <= 10 && e[1][1] <= 10,
              "Elza %s, CDEMD1-Bank: beide Haende 0..70 ueber IHRER Schulter (Y %d / %d; J-Messung 20..65)",
              s, (int)e[0][1], (int)e[1][1]);
        PRUEF(yv <= -200, "Gegenprobe Elza %s mit Leons CDEMD0-Bank (Stand vorher): Haende %d / %d ueber der Schulter (J: 229..284)",
              s, (int)v[0][1], (int)v[1][1]);
        PRUEF(al == RE15_EMS_ARCHIV_LEON && nl == nl0 &&
              !memcmp(l, l0, sizeof l),
              "Leon %s: Port-Weiche = CDEMD0, Griff bitgleich zum Stand vorher (%d Biss-Bilder, Y %d / %d)",
              s, nl, (int)l[0][1], (int)l[1][1]);
    }
}

/* ---- opfer27 (0x27-Finisher an Elza: Opfer-Clip 0 aus der Bank der Port-Weiche) ---------------------------
 * ROOM1141 sub00 (Elza): Ck(4,242) @0x00B4A -> Sce_em_set 0x27 @0x00B54/@0x00B68. Der Finisher (Hook 0x8011c414,
 * engine/src/affen_11c0.c re15_affen_finisher_tick) spielt Clip 0 der Opfer-Bank des Greifers (acbcc/acbd0,
 * `pl->motion = 0` @0x8011c490) bis Bild fc-1 (`k + 1 >= fc` @0x8011c548-50) und meldet dort den Tod. Eintritt wie
 * B[8] der KI (enemy_ai_common.c: hp -= 600 @0x80119198-ac, re15_player_victim_devour @0x801191c8-cc).
 * CDEMD0 Clip 0 = 70 Bilder, CDEMD1 = 63 (n1_zensus.txt). Rueckgabe: Bild des Todes-Ereignisses (-1 = keins). */
#include "re15_affen.h"
static int opfer27_lauf(int zwang, int *archiv, int *fc_bank)
{
    s_rmb_flag[0] = 4; s_rmb_flag[1] = 242; s_rmb_flag_an = 1;
    *archiv = raum_mit_banken(0x1141, 4, zwang, 0x27);
    s_rmb_flag_an = 0;
    if (*archiv < 0) {
        printf("  FEHLER: ROOM1141 mit Ck(4,242): kein Typ 0x27 mit Bank (Flag (4,242) = %d, Typ 0x27 aktiv %d)\n",
               re15_game_flag_get(4, 242), typ_da(0x27));
        g_fail++; return -1;
    }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER], *e = NULL;
    for (int sl = 1; sl < RE15_ACTOR_MAX && !e; sl++) if (g_actors[sl].active && g_actors[sl].type == 0x27) e = &g_actors[sl];
    re15_enemy_bank_t *vb = re15_enemy_find(0x27);
    if (!e || !vb || !vb->victim_ok) { printf("  FEHLER: kein 0x27 / keine Opfer-Bank in ROOM1141 (Ck(4,242))\n"); g_fail++; return -1; }
    *fc_bank = vb->anim_victim.clips[0].frame_count;
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = e->floor;
    pl->x = e->x + 500; pl->z = e->z; pl->y = e->y;
    re15_collision_set_band(0); re15_player_set_aim_clip_len(12); re15_inv_load_briefing();
    pl->hp = (int16_t)(pl->hp - 600);
    re15_player_victim_devour(e);
    pl->hit_react |= 1;
    for (int f = 0; f < 400 && re15_affen_finisher_aktiv(); f++) bild();
    int ev[RE15_AFFEN_FIN_LOG_N], n = re15_affen_finisher_ereignisse(ev, RE15_AFFEN_FIN_LOG_N), tod = -1;
    for (int i = 0; i < n; i++) if (ev[i] & RE15_AFFEN_FIN_TOD) tod = ev[i] & 0xFFF;
    printf("  0x27-FINISHER ELZA, Bank aus %s: Opfer-Clip 0 = %d Bilder, Tod bei Bild %d\n",
           *archiv ? "CDEMD1" : "CDEMD0", *fc_bank, tod);
    return tod;
}

static void teil_opfer27(void)
{
    printf("== opfer27 ==\n");
    int a1 = -1, a0 = -1, f1 = 0, f0 = 0;
    int t1 = opfer27_lauf(-1, &a1, &f1);
    int t0 = opfer27_lauf(RE15_EMS_ARCHIV_LEON, &a0, &f0);
    PRUEF(a1 == RE15_EMS_ARCHIV_ELZA && f1 == 63 && t1 == 62,
          "Elza ROOM1141: 0x27 aus CDEMD1 (Port-Weiche), Finisher spielt Opfer-Clip 0 mit %d Bildern, Tod bei Bild %d (63/62)",
          f1, t1);
    PRUEF(f0 == 70 && t0 == 69, "Gegenprobe mit Leons CDEMD0-Bank (Stand vorher): %d Bilder, Tod bei Bild %d (70/69)", f0, t0);
}

/* Teil-Auswahl: ein Name oder eine Komma-Liste ("raeume,griff"); "alle" faehrt jeden Teil. */
static int teil_hat(const char *liste, const char *name)
{
    size_t n = strlen(name);
    for (const char *p = liste; (p = strstr(p, name)) != NULL; p += n)
        if ((p == liste || p[-1] == ',') && (p[n] == 0 || p[n] == ',')) return 1;
    return 0;
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (teil_hat(teil, "toc")    || !strcmp(teil, "alle")) teil_toc();
    if (teil_hat(teil, "weiche") || !strcmp(teil, "alle")) teil_weiche();
    /* griff/opfer27 VOR raeume: scd_room_reenter (raum_init) laesst die AOT-Pruefung des Raums scharf, danach
     * feuert in raum_mit_banken ein sce-3-Ereignis (ROOM1140 sub01) statt sub00 — eigener Prozess je ctest-Teil. */
    if (teil_hat(teil, "griff")  || !strcmp(teil, "alle")) teil_griff();
    if (teil_hat(teil, "opfer27") || !strcmp(teil, "alle")) teil_opfer27();
    if (teil_hat(teil, "raeume") || !strcmp(teil, "alle")) teil_raeume();
    if (!strcmp(teil, "liste") && argc > 2) {          /* Messwerkzeug (kein Riegel): Spawns eines Raums nach dem Init */
        uint16_t r = (uint16_t)strtoul(argv[2], NULL, 16);
        if (raum_mit_banken(r, (r & 1u) ? 4 : 0, -1, 0) >= -1)
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (g_actors[s].active)
                    printf("  ROOM%04X slot %d Typ 0x%02X em_flag %d (%d,%d,%d) rot %d st %d/%d/%d mo %d Archiv %s\n", (unsigned)r, s,
                           g_actors[s].type, (int)g_actors[s].em_flag_id, g_actors[s].x, g_actors[s].y, g_actors[s].z, g_actors[s].rot_y, g_actors[s].state,
                           g_actors[s].sub_state_1, g_actors[s].sub_state_2, g_actors[s].motion,
                           re15_ems_archiv_fuer_typ(g_actors[s].type) ? "CDEMD1" : "CDEMD0");
    }
    printf("%s: %s (%d Fehler)\n", "test_r37_elzaems", g_fail ? "FEHLER" : "OK", g_fail);
    return g_fail ? 1 : 0;
}
