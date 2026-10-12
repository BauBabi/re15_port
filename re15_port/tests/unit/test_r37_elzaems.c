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
    for (int i = 0; i < nflags; i++) re15_game_flag_set(flags[i][0], flags[i][1], 1);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
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
    /* Leon ROOM1170: Elliot spawnt erst in sub15 nach dem Montage-Gate (unit_r37_roy_leon1170 / integration_r37_roy_leon1170
     * pruefen ihn mit Bank und Zeichenweg) — hier nur die Weiche nach dem Raum-Init. */
    if (raum_init(0x1170, 0, NULL, 0) == 0)
        PRUEF(re15_typ47_ist_elliot(0x47) && !re15_roy_aus_elza_archiv(0x47) &&
              re15_ems_archiv_fuer_typ(0x47) == RE15_EMS_ARCHIV_LEON,
              "Leon ROOM1170: Typ 0x47 = Elliot-Sonderweg (Leon-Archiv), kein Elza-Archiv");
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "toc")    || !strcmp(teil, "alle")) teil_toc();
    if (!strcmp(teil, "weiche") || !strcmp(teil, "alle")) teil_weiche();
    if (!strcmp(teil, "raeume") || !strcmp(teil, "alle")) teil_raeume();
    printf("%s: %s (%d Fehler)\n", "test_r37_elzaems", g_fail ? "FEHLER" : "OK", g_fail);
    return g_fail ? 1 : 0;
}
