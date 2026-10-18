/* test_r39_elzazombies.c — RIEGEL Runde 39 Spur Q "elzazombies": Elza, Zombie-Durchbrueche in der Panikphase (Montage-
 * Form wie Leon nach Irons' Tod). Dossier analysis/befunde_runde39/Q_elzazombies.md, Mechanik include/re15_elzazombies_r39.h
 * und engine/src/elzazombies_r39.c.
 *
 * NUTZER (AUFTRAG.md Nachtrag 10, woertlich): "Und auch hier sollen jetzt Zombies durchbrechen - einmal in den Flur hinein
 * von 10C1 aus - so viele wie im medical room - also 10E1 am Leben gelassen wurden., die sollen dann, wenn man in ROOM
 * 1061 die Etage von unten ueber die Treppe erreicht aber moeglichst weit weg an der Seite stehen. Dann das gleiche
 * nochmal wie bei Leon in den ROOM 1071, falls die Zombies nicht besiegt wurden und im ROOM 1031 ueber die
 * Absperrung/Das Rollo, so wie bei der cutscene."
 *
 * Verfahren wie test_r38_montage.c (ausgelieferte RDT, Raumaufbau ueber scd_room_reenter = derselbe Einhaengepunkt wie im
 * Spiel, Bilder in der Reihenfolge von platform/pc/main.c, RE2-Gegnerbaenke wie die exe), Elza = g_gameflow.character 4.
 *
 * Teile (argv[1]):
 *   zensus    Quell-Records (ROOM10E1 @0x00D56.. Tot-Bits 0xe0..0xe3, ROOM1071 @0x015CA.. 0xc6..0xca) = Konstanten; die
 *             Kopie-Bits (Zone 7) kommen in KEINEM Sce_em_set von STAGE1-3 vor und nicht in Leons Kopien; die Zeilen des
 *             1031-Programms = RDT-Bytes ROOM1031 sub08 (@0x02858/@0x02862/@0x0286E/@0x0287A/@0x02882..), sub09 = ROOM1030
 *             sub09 (Kriech-Regel); Kamera-Saetze = RID-Bytes; Tuer-Slot 20 / Ereignis 30 / Bank-5-Bits 28/29 frei.
 *   kette_<a><b>   echte Kette im Prozess: ROOM1041 (Elza), Flanke (9,97) -> Raum fuer Raum bis zur Rueckkehr; <a> = lebende
 *             10E1-Zombies (0..4), <b> = lebende 1071-Zombies (0..5). Prueft Raumfolge, Schnitte, Zahl der Kopien (= so viele
 *             wie leben), jede Kopie kommt ihrer Kamera naeher (>= 400 = Koerperradius), Spieler-HP unveraendert, Rueckkehr an
 *             die gemerkte Stelle mit Steuerung, (9,95)=1, Quell-Bits gesetzt. Gegenprobe (Modul aus): keine Kette.
 *   nach1061  Nachspawn: die 10E1-Kopien stehen an der Westseite (moeglichst weit weg vom gemessenen Ankunftspunkt der
 *             Treppe), gehalten solange die Spielerin unten ist (200 Bilder Etage 0, Weg 0), frei auf Etage 4 (Abstand
 *             >= 4000 beim Freigeben), danach kommen sie naeher; tote Kopien erscheinen nicht.
 *   nach1031  Nachspawn ROOM1031 nach der Montage: 1071-Kopien + 3 Kriecher erscheinen; ohne (9,95) nicht.
 *   ohne      ohne Flanke keine Kette (Bit 97 beim Laden gesetzt = keine Flanke); Leon (Figur 0) mit Flanke und (9,95):
 *             nichts; Modul aus: nichts.
 */
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
#include "re15_ai_flavor.h"
#include "re15_fade.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_tim.h"
#include "re2_ems.h"
#include "re15_damage.h"
#include "re15_gameflow.h"
#include "re15_irons_tod.h"
#include "re15_elzazombies_r39.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif

void re15_actor_step_all_walkers(void);

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

#define KOERPERRADIUS 400   /* +0x78-Satz +6 (FUN_8003a31c), Leons R38_NAEHER_HALB */

static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0, s_cine_was = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (n) *n = (size_t)sz;
    return b;
}
static uint16_t le16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }
static int16_t  les16(const uint8_t *p) { return (int16_t)le16(p); }
static uint32_t le32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

static uint8_t *raum_bytes(uint16_t room, size_t *n)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    return slurp(rp, n);
}

/* Frisch von der Platte in g_room_rdt (Navigator + Wandklemme lesen ihn; Sca_id_set aendert ihn, test_r38_montage.c). */
static int rdt_laden(uint16_t room)
{
    free(s_raw); s_raw = raum_bytes(room, &s_rawsz); g_room_rdt_ok = 0;
    if (!s_raw || s_rawsz < 0x100) return -1;
    if (re15_rdt_parse(s_raw, s_rawsz, &g_room_rdt) < 0) return -1;
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;
    return 0;
}

static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static void bank_re2(uint8_t type)
{
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_re2_n);
    if (!s_re2_ems) return;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return;
    if (re2_ems_load_bank(s_re2_ems, s_re2_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; }
    else eb->type = 0;
}
static void banken(void) { bank_re2(0x10); bank_re2(0x11); bank_re2(0x16); }

/* Ein Spielbild in der Reihenfolge von platform/pc/main.c (wie test_r38_montage.c). */
static void frame(void)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_letterbox_tick(re15_game_flag_get(1, 27));
    {
        int cine = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
        if (cine) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was) g_scd.letterbox_countdown = 15;
        s_cine_was = cine;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
            g_scd.player_mode = 0;
            re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        }
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = 0;
    s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e)) { }
}

/* Raumaufbau wie der Tuer-Weg (scd_room_reenter mit allen Port-Installern); g_game bleibt (reist von Raum zu Raum). */
static int room_boot(uint16_t room, int32_t px, int32_t py, int32_t pz, int16_t rot, uint8_t cut)
{
    if (rdt_laden(room) != 0) { printf("  FEHLER: ROOM%04X nicht ladbar\n", room); g_fail++; return -1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    banken();
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    re15_damage_seed_rng(0x2545f491u);
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->y = py;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    pl->floor = (uint8_t)re15_collision_band_from_y(py);
    re15_collision_set_band(pl->floor);
    s_shown = cut; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    g_scd.cam_id = cut;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    return 0;
}

/* Elza-Grundzustand nach dem Intro: (3,125) Erzaehler durch (ROOM1031 main00 @0x01E54), (3,193) Intro-Tuer durch
 * (@0x0204E), (3,116) ROOM1011 besucht (sub02 @0x00B06). */
static void elza_flags(void)
{
    re15_game_state_init();
    g_gameflow.character = 4;
    re15_game_flag_set(3, 125, 1); re15_game_flag_set(3, 193, 1); re15_game_flag_set(3, 116, 1);
}

static int aktive_gegner(void)
{
    int n = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) if (g_actors[i].active && g_actors[i].type) n++;
    return n;
}

/* ================================================================================================================== */
/* Teil zensus                                                                                                        */
/* ================================================================================================================== */
/* SCD-Regionen wie tools/scd_dump_room.py (Muster test_r36_kartenbits.c region_laufen/zensus_raum). */
static uint8_t s_bit_belegt[256];
static int     s_em_saetze = 0, s_desync = 0;
static uint32_t ptr_ausdehnung(const uint8_t *d, size_t n, uint32_t s)
{
    if (s == 0 || s + 2 > n) return 0;
    uint16_t first = le16(d + s);
    if (first < 2 || (first & 1) || s + first > n) return 0;
    uint32_t mx = 0;
    for (uint32_t i = 0; i < first / 2u; i++) { uint16_t o = le16(d + s + 2 * i); if (o && s + o < n && o > mx) mx = o; }
    return mx;
}
static uint32_t sektion_ende(const uint8_t *d, size_t n, uint32_t s)
{
    uint32_t boden = s + ptr_ausdehnung(d, n, s), best = 0;
    for (uint32_t o = 0x40; o < 0x60 && o + 4 <= n; o += 4) { uint32_t v = le32(d + o); if (v > boden && v <= n && (best == 0 || v < best)) best = v; }
    return best ? best : (uint32_t)n;
}
static uint32_t sprungziel(const uint8_t *d, uint32_t pc)
{
    uint8_t op = d[pc];
    if (op == 0x06) return pc + 4 + le16(d + pc + 2);
    if (op == 0x07) return (uint32_t)((int32_t)pc + les16(d + pc + 2));
    if (op == 0x0D || op == 0x1B) return (uint32_t)((int32_t)pc + 6 + les16(d + pc + 2));
    if (op == 0x0F || op == 0x11 || op == 0x13) return (uint32_t)((int32_t)pc + 4 + les16(d + pc + 2));
    if (op == 0x14) return pc + 6 + le16(d + pc + 2);
    if (op == 0x17) { int16_t o = les16(d + pc + 4); return o > 0 ? (uint32_t)((int32_t)pc + o) : 0; }
    return 0;
}
static void region_laufen(const uint8_t *d, uint32_t start, uint32_t ende)
{
    uint32_t pc = start, maxf = start;
    while (pc < ende) {
        int sz = scd_opcode_size_at(d + pc);
        if (sz <= 0 || pc + (uint32_t)sz > ende) { s_desync++; return; }
        if (d[pc] == 0x44) { s_em_saetze++; if (d[pc + 7] != 0xff) s_bit_belegt[d[pc + 7]] = 1; }
        uint32_t t = sprungziel(d, pc);
        if (t > maxf) maxf = t;
        if (d[pc] == 0x01 && pc + 2 > maxf) return;
        pc += (uint32_t)sz;
    }
}
static void zensus_raum(const uint8_t *d, size_t n)
{
    for (int k = 0; k < 2; k++) {
        uint32_t ss = le32(d + 0x40 + 4 * k);
        if (ss == 0 || ss + 2 > n) continue;
        uint32_t se = sektion_ende(d, n, ss);
        uint16_t first = le16(d + ss);
        if (first < 2 || (first & 1) || ss + first > n) continue;
        uint32_t offs[512]; int cnt = 0;
        for (uint32_t i = 0; i < first / 2u && cnt < 512; i++) {
            uint16_t o = le16(d + ss + 2 * i);
            if (!o || ss + o >= n) continue;
            int dup = 0; for (int j = 0; j < cnt; j++) if (offs[j] == o) dup = 1;
            if (!dup) offs[cnt++] = o;
        }
        for (int i = 0; i < cnt; i++) for (int j = i + 1; j < cnt; j++) if (offs[j] < offs[i]) { uint32_t t = offs[i]; offs[i] = offs[j]; offs[j] = t; }
        for (int i = 0; i < cnt; i++) {
            uint32_t e = (i + 1 < cnt) ? offs[i + 1] : se - ss;
            if (e > se - ss) e = se - ss;
            if (e > offs[i]) region_laufen(d, ss + offs[i], ss + e);
        }
    }
}

static int bytes_gleich(const uint8_t *a, const uint8_t *b, int n) { return memcmp(a, b, (size_t)n) == 0; }
/* Sucht die n Bytes ab Datei-Offset `off` im Laufprogramm. */
static int im_programm(const uint8_t *prog, int plen, const uint8_t *datei, uint32_t off, int n)
{
    for (int o = 0; o + n <= plen; o++) if (bytes_gleich(prog + o, datei + off, n)) return 1;
    return 0;
}

static void teil_zensus(void)
{
    printf("== zensus ==\n");
    size_t n10 = 0, n71 = 0, n31 = 0, n30 = 0, n61 = 0;
    uint8_t *d10 = raum_bytes(0x10E1, &n10), *d71 = raum_bytes(0x1071, &n71), *d31 = raum_bytes(0x1031, &n31);
    uint8_t *d30 = raum_bytes(0x1030, &n30), *d61 = raum_bytes(0x1061, &n61);
    if (!d10 || !d71 || !d31 || !d30 || !d61) { PRUEF(0, "RDTs ladbar"); return; }
    /* Quell-Records */
    int ok10 = 1;
    for (int i = 0; i < RE15_R39Q_N_10E1; i++) {
        const uint8_t *a = d10 + 0x00D56 + 20 * i, *b = d10 + 0x00DBE + 20 * i;
        ok10 &= a[0] == 0x44 && a[2] == 0x10 && a[7] == RE15_R39Q_BIT_10E1_0 + i && b[0] == 0x44 && b[7] == RE15_R39Q_BIT_10E1_0 + i;
    }
    PRUEF(ok10, "ROOM10E1 sub00 @0x00D56.. (Fresser) und @0x00DBE.. (Steher): vier Typ-0x10-Records, Tot-Bits 0x%02x..0x%02x",
          RE15_R39Q_BIT_10E1_0, RE15_R39Q_BIT_10E1_0 + 3);
    int ok71 = 1;
    static const uint8_t typ71[5] = { 0x10, 0x10, 0x10, 0x11, 0x11 };
    for (int i = 0; i < RE15_R39Q_N_1071; i++) {
        const uint8_t *a = d71 + 0x015CA + 20 * i, *b = d71 + 0x01632 + 20 * i;
        ok71 &= a[0] == 0x44 && a[2] == typ71[i] && a[7] == RE15_R39Q_BIT_1071_0 + i && b[0] == 0x44 && b[3] == 0x00 &&
                b[7] == RE15_R39Q_BIT_1071_0 + i;
    }
    PRUEF(ok71, "ROOM1071 sub00 @0x015CA.. / @0x01632..: Typen 10,10,10,11,11, Tot-Bits 0x%02x..0x%02x", RE15_R39Q_BIT_1071_0,
          RE15_R39Q_BIT_1071_0 + 4);
    /* Kopie-Bits frei in allen Sce_em_set STAGE1-3 (Zone 7 = Stage 1-3) und nicht Leons Kopien */
    memset(s_bit_belegt, 0, sizeof s_bit_belegt); s_em_saetze = 0; s_desync = 0;
    int raeume = 0;
    for (unsigned st = 1; st <= 3; st++)
        for (unsigned hi = 0; hi < 0x100; hi++)
            for (unsigned lo = 0; lo < 2; lo++) {
                size_t n = 0; uint8_t *d = raum_bytes((uint16_t)((st << 12) | (hi << 4) | lo), &n);
                if (!d) continue;
                if (n >= 0x100) { zensus_raum(d, n); raeume++; }
                free(d);
            }
    int belegt = 0; for (int b = 0; b < 256; b++) belegt += s_bit_belegt[b];
    printf("  Zensus: %d RDTs, %d Sce_em_set, %d Bits belegt, %d Desync\n", raeume, s_em_saetze, belegt, s_desync);
    const uint8_t *kb = NULL; int nkb = re15_elzazombies_r39_kopiebits(&kb);
    static const int leon[] = { 40, 41, 42, 43, 46, 47, 48, 49, 75, 76, 77, 78, 79, 83, 84, 90, 91, 95 };
    int frei = 1, fremd = 1, doppelt = 0;
    for (int i = 0; i < nkb; i++) {
        if (s_bit_belegt[kb[i]]) { frei = 0; printf("    Bit %u im Original belegt\n", kb[i]); }
        for (unsigned j = 0; j < sizeof leon / sizeof leon[0]; j++) if (kb[i] == leon[j]) fremd = 0;
        for (int j = i + 1; j < nkb; j++) if (kb[i] == kb[j]) doppelt++;
    }
    PRUEF(raeume >= 130 && s_desync == 0 && nkb == 12, "Zensus vollstaendig (%d RDTs, 0 Desync), 12 Kopie-Bits", raeume);
    PRUEF(frei && fremd && !doppelt, "Kopie-Bits frei im Original, nicht Leons Kopien, paarweise verschieden");
    PRUEF(s_bit_belegt[RE15_R39Q_BIT_10E1_0] && s_bit_belegt[RE15_R39Q_BIT_1071_0], "Gegenprobe: die Quell-Bits SIND belegt");
    /* 1031 = 1030 dort, wo das Programm Leons Form uebernimmt */
    PRUEF(bytes_gleich(d31 + 0x028CC, d30 + 0x027E0, 0x5A), "ROOM1031 sub09 @0x028CC = ROOM1030 sub09 @0x027E0 (Kriech-Regel, 90 B)");
    PRUEF(bytes_gleich(d31 + 0x025B4, d30 + 0x024C8, 0x28C) && bytes_gleich(d31 + 0x02840, d30 + 0x02754, 16),
          "ROOM1031 sub06 @0x025B4 / sub07 @0x02840 = ROOM1030 sub06 @0x024C8 / sub07 @0x02754");
    PRUEF(d31[0x025CC] == 0x3e && d31[0x025CC + 2] == 0x0f && d31[0x025CC + 4] == 0x05 && d31[0x02844] == 0x26 && le16(d31 + 0x02848) == 0x1000 && d31[0x025D4] == 0x22,
          "Konstanten der Kriech-Regel: Stempel 5 @0x025CC, Bit 0x1000 @0x02844, Set(5,i) @0x025D4");
    /* Kamera-Saetze */
    static const struct { uint16_t raum; int cut; int32_t kx, kz, zx, zz; } kam[3] = {
        { 0x1061, 2, 20199, 21273, 24670, 27767 }, { 0x1031, 7, -26820, -2574, -15858, -5040 }, { 0x1031, 12, -11790, -15282, -8640, -26478 } };
    int kok = 1;
    for (int i = 0; i < 3; i++) {
        const uint8_t *d = kam[i].raum == 0x1061 ? d61 : d31;
        const uint8_t *r = d + le32(d + 0x24) + 32 * kam[i].cut;
        kok &= (int32_t)le32(r + 4) == kam[i].kx && (int32_t)le32(r + 12) == kam[i].kz && (int32_t)le32(r + 16) == kam[i].zx &&
               (int32_t)le32(r + 24) == kam[i].zz;
    }
    PRUEF(kok, "Kamera-Saetze 1061 Cut 2 @0x000A0, 1031 Cut 7 @0x00140, Cut 12 @0x001E0 = RID-Bytes");
    /* Tuer ROOM10C1 -> 1061: Spawn (26000,-7200,25300) Gier 2048 Cut 2 Etage 4 */
    {
        size_t nc = 0; uint8_t *dc = raum_bytes(0x10C1, &nc);
        const uint8_t *t = dc ? dc + 0x00E42 : NULL;
        PRUEF(t && t[0] == 0x3b && les16(t + 14) == 26000 && les16(t + 16) == RE15_R39Q_Y_1061 && les16(t + 18) == 25300 &&
              le16(t + 20) == 2048 && t[23] == 0x06 && t[24] == RE15_R39Q_CUT_1061 && t[25] == RE15_R39Q_ETAGE_1061,
              "ROOM10C1 Tuer @0x00E42 -> 1061: (26000,%d,25300) Gier 2048 Cut %d Etage %d", RE15_R39Q_Y_1061, RE15_R39Q_CUT_1061,
              RE15_R39Q_ETAGE_1061);
        free(dc);
    }
    /* Programm C im Bau: die 1031-Zeilen woertlich */
    elza_flags();
    re15_elzazombies_r39_reset();
    re15_elzazombies_r39_zustand_setzen(RE15_R39Q_S1031);
    g_current_room_id = 0x1031;
    for (int s = 0; s < SCD_THREAD_COUNT; s++) g_scd.threads[s].active = 0;
    const uint8_t *pg = re15_elzazombies_r39_ereignis(0x1031, RE15_R39Q_EREIGNIS);
    int plen = 0; re15_elzazombies_r39_laufprogramm(&plen);
    PRUEF(pg && plen > 0, "Programm C (1031) baut (%d B)", plen);
    if (pg) {
        PRUEF(im_programm(pg, plen, d31, 0x02858, 10) && im_programm(pg, plen, d31, 0x0286E, 12) && im_programm(pg, plen, d31, 0x0287A, 8),
              "Rollo bricht: Aot_reset @0x02858, Cut_replace @0x0286E..@0x02877, Sca_id_set @0x0287A/@0x0287E woertlich");
        PRUEF(im_programm(pg, plen, d31, 0x02882, 8), "Save var5/var7 @0x02882/@0x02886 woertlich");
        static const uint8_t gosub4[8] = { 0x18, 0x09, 0x18, 0x09, 0x18, 0x09, 0x18, 0x09 };
        int g = 0; for (int o = 0; o + 8 <= plen; o++) if (bytes_gleich(pg + o, gosub4, 8)) g = 1;
        PRUEF(g && d31[0x0288A] == 0x18 && d31[0x0288B] == 0x09, "viermal Gosub sub09 (Original @0x0288A `18 09`)");
        PRUEF(im_programm(pg, plen, d31, 0x02896, 2), "Cut_chg 12 @0x02896 woertlich");
    }
    re15_elzazombies_r39_zustand_setzen(RE15_R39Q_AUS);
    /* Tuer-Slot / Ereignis / Bank 5: frei in 1041, 1061, 1031 */
    {
        static const uint16_t raeume3[3] = { 0x1041, 0x1061, 0x1031 };
        int slot_frei = 1, ev_frei = 1, b5_frei = 1;
        for (int r = 0; r < 3; r++) {
            size_t n = 0; uint8_t *d = raum_bytes(raeume3[r], &n);
            re15_rdt_t rd; memset(&rd, 0, sizeof rd);
            if (!d || re15_rdt_parse(d, n, &rd) < 0) { slot_frei = 0; continue; }
            if (rd.sub_scd_count > RE15_R39Q_EREIGNIS) ev_frei = 0;
            /* alle Opcodes: Aot-Slots und Bank-5-Bits */
            memset(s_bit_belegt, 0, sizeof s_bit_belegt);
            uint32_t ss[2] = { le32(d + 0x40), le32(d + 0x44) };
            for (int k = 0; k < 2; k++) {
                uint32_t a = ss[k], e = sektion_ende(d, n, a);
                uint16_t first = le16(d + a);
                for (uint32_t i = 0; i < first / 2u; i++) {
                    uint32_t pc = a + le16(d + a + 2 * i), maxf = pc;
                    while (pc < e) {
                        int sz = scd_opcode_size_at(d + pc);
                        if (sz <= 0) break;
                        const uint8_t op = d[pc];
                        if ((op == 0x2c || op == 0x3b || op == 0x50 || op == 0x46 || op == 0x47) && d[pc + 1] == RE15_R39Q_TUER_SLOT) slot_frei = 0;
                        if ((op == 0x21 || op == 0x22) && d[pc + 1] == 5 &&
                            (d[pc + 2] == RE15_R39Q_SIG_KNALL_TUER || d[pc + 2] == RE15_R39Q_SIG_KNALL_1030)) b5_frei = 0;
                        uint32_t t = sprungziel(d, pc); if (t > maxf) maxf = t;
                        if (op == 0x01 && pc + 2 > maxf) break;
                        pc += (uint32_t)sz;
                    }
                }
            }
            free(d);
        }
        PRUEF(slot_frei && ev_frei && b5_frei, "Tuer-Slot %d, Ereignis %d (> Sub-Zahl), Bank-5-Bits %d/%d frei in 1041/1061/1031",
              RE15_R39Q_TUER_SLOT, RE15_R39Q_EREIGNIS, RE15_R39Q_SIG_KNALL_TUER, RE15_R39Q_SIG_KNALL_1030);
    }
    free(d10); free(d71); free(d31); free(d30); free(d61);
}

/* ================================================================================================================== */
/* Teil kette_<a><b>                                                                                                  */
/* ================================================================================================================== */
typedef struct { uint16_t raum; int bilder, cut_erst; int spawns; } schritt_t;

/* Fahrt bis zur Tuer-Anforderung (oder max Bilder); Ergebnis: Bilder, Zielraum/-lage. */
static int fahren(int max, int *bilder)
{
    for (int f = 0; f < max; f++) {
        frame();
        if (g_room_change.pending) { if (bilder) *bilder = f + 1; return 1; }
    }
    if (bilder) *bilder = max;
    return 0;
}

static void teil_kette(int lebend_10e1, int lebend_1071, int an)
{
    printf("== kette %d%d%s (10E1 lebend %d, 1071 lebend %d) ==\n", lebend_10e1, lebend_1071, an ? "" : " GEGENPROBE Modul aus",
           lebend_10e1, lebend_1071);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    elza_flags();
    for (int i = lebend_10e1; i < RE15_R39Q_N_10E1; i++) re15_game_flag_set(7, (uint8_t)(RE15_R39Q_BIT_10E1_0 + i), 1);
    for (int i = lebend_1071; i < RE15_R39Q_N_1071; i++) re15_game_flag_set(7, (uint8_t)(RE15_R39Q_BIT_1071_0 + i), 1);
    re15_elzazombies_r39_reset();
    re15_elzazombies_r39_enable(an);
    if (room_boot(0x1041, -21008, 0, -13134, 2112, 0) != 0) return;
    for (int f = 0; f < 30; f++) frame();
    re15_game_flag_set(9, 97, 1);                              /* die Flanke (Spur P setzt das Bit beim 1041-Ruf) */
    uint16_t folge[8]; int nfolge = 0, cut_erst[8] = {0}, spawn_erst[8] = {0}, bilder[8] = {0};
    int hp_min = 100;
    for (int k = 0; k < 6; k++) {
        int b = 0;
        int wechsel = fahren(1200, &b);
        if (g_actors[RE15_ACTOR_SLOT_PLAYER].hp < hp_min) hp_min = g_actors[RE15_ACTOR_SLOT_PLAYER].hp;
        if (nfolge > 0) bilder[nfolge - 1] = b;
        if (!wechsel) break;
        re15_elzazombies_r39_abschnitt_beenden();
        const re15_room_change_t rc = g_room_change;
        uint16_t ziel = (uint16_t)rc.room_id;
        if (nfolge < 8) folge[nfolge] = ziel;
        if (room_boot(ziel, rc.x, rc.y, rc.z, (int16_t)rc.yaw_4096, (uint8_t)rc.target_cut) != 0) return;
        if (nfolge < 8) { cut_erst[nfolge] = (int)g_scd.cam_id; spawn_erst[nfolge] = aktive_gegner(); }
        if (ziel == 0x1031) {                                   /* Programm C: Original-Se_on ROOM1031 sub08 @0x02862 */
            size_t n31 = 0; uint8_t *d31 = raum_bytes(0x1031, &n31);
            int plen = 0; const uint8_t *pg = re15_elzazombies_r39_laufprogramm(&plen);
            const int knall = d31 && im_programm(pg, plen, d31, 0x02862, 12);
            PRUEF(knall == (lebend_1071 > 0), "Programm C %s den Original-Knall Se_on @0x02862 woertlich (%s)",
                  knall ? "traegt" : "traegt nicht", lebend_1071 > 0 ? "mit Cut-7-Teil" : "ohne Cut-7-Teil: Knall Satz 1 am Ende des vorigen Schritts");
            free(d31);
        }
        nfolge++;
        if (ziel == 0x1041) {                                   /* Rueckkehr: bis die Steuerung frei ist */
            for (int f = 0; f < 200 && re15_elzazombies_r39_zustand() != RE15_R39Q_AUS; f++) frame();
            break;
        }
    }
    if (!an) {
        PRUEF(nfolge == 0 && !re15_game_flag_get(9, 95), "Gegenprobe: ohne Modul keine Kette (Raumwechsel %d), (9,95)=0", nfolge);
        re15_elzazombies_r39_enable(1);
        return;
    }
    /* erwartete Folge */
    uint16_t soll[4]; int nsoll = 0;
    if (lebend_10e1 > 0) soll[nsoll++] = 0x1061;
    soll[nsoll++] = 0x1031;
    soll[nsoll++] = 0x1041;
    int folge_ok = nfolge == nsoll;
    for (int i = 0; i < nsoll && folge_ok; i++) folge_ok = folge[i] == soll[i];
    printf("  Raumfolge:"); for (int i = 0; i < nfolge; i++) printf(" %04X(Cut %d, %d Gegner, %d Bilder)", folge[i], cut_erst[i], spawn_erst[i], bilder[i]); printf("\n");
    PRUEF(folge_ok, "Raumfolge 1041 -> %s1031 -> 1041", lebend_10e1 > 0 ? "1061 -> " : "");
    int i1031 = lebend_10e1 > 0 ? 1 : 0;
    if (lebend_10e1 > 0)
        PRUEF(cut_erst[0] == RE15_R39Q_CUT_1061 && spawn_erst[0] == lebend_10e1,
              "1061: erstes Bild Cut %d mit %d Kopien (= lebende 10E1-Zombies %d)", cut_erst[0], spawn_erst[0], lebend_10e1);
    PRUEF(cut_erst[i1031] == (lebend_1071 > 0 ? RE15_R39Q_CUT_1031_TUER : RE15_R39Q_CUT_1031_TOR),
          "1031: erstes Bild Cut %d (%s)", cut_erst[i1031], lebend_1071 > 0 ? "Cut 7, Kopien vor der 1071-Tuer" : "gleich Cut 12, 1071 leer");
    /* Spuren: jede Kopie kommt ihrer Kamera naeher */
    const re15_r39q_spur_t *sp = NULL; int n = re15_elzazombies_r39_spuren(&sp);
    int k1061 = 0, n1061 = 0, k1031 = 0, n1031 = 0, kriech = 0, nkriech = 0;
    for (int i = 0; i < n; i++) {
        const re15_r39q_spur_t *p = &sp[i];
        const int naeher = p->d0 - p->d1;
        if (p->raum == 0x1061 && p->bit >= 124 && p->bit <= 127) { n1061++; if (naeher >= KOERPERRADIUS && !p->halt) k1061++; }
        if (p->raum == 0x1031 && p->cut == 7 && p->bit >= 128 && p->bit <= 132) { n1031++; if (naeher >= KOERPERRADIUS) k1031++; }
        if (p->raum == 0x1031 && p->cut == 12 && (p->bit == 133 || p->bit == 144 || p->bit == 145)) { nkriech++; if (naeher >= KOERPERRADIUS) kriech++; }
        printf("    Spur ROOM%04X Cut %d Bit %d: %d -> %d (%+d) angehalten %d\n", p->raum, p->cut, p->bit, p->d0, p->d1, -naeher, p->halt);
    }
    if (lebend_10e1 > 0) PRUEF(n1061 == lebend_10e1 && k1061 == n1061, "1061 Cut 2: alle %d Kopien >= %d naeher an der Kamera, keine hinter ihr (%d)", n1061, KOERPERRADIUS, k1061);
    else PRUEF(n1061 == 0, "1061 entfaellt: keine 10E1-Kopie");
    if (lebend_1071 > 0) PRUEF(n1031 == lebend_1071 && k1031 == n1031, "1031 Cut 7: alle %d Kopien >= %d naeher (%d)", n1031, KOERPERRADIUS, k1031);
    else PRUEF(n1031 == 0, "1031 Cut 7 entfaellt: keine 1071-Kopie");
    PRUEF(nkriech == RE15_R39Q_N_KRIECHER && kriech == nkriech, "1031 Cut 12: drei Kriecher kommen >= %d naeher (%d von %d)", KOERPERRADIUS, kriech, nkriech);
    PRUEF(hp_min == 100 && re15_elzazombies_r39_hp_min() == 100, "Spieler-HP waehrend der Kette unveraendert (min %d / Takt-Messung %d)",
          hp_min, re15_elzazombies_r39_hp_min());
    /* Rueckkehr */
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    PRUEF(g_current_room_id == 0x1041 && pl->x == -21008 && pl->z == -13134 && pl->rot_y == 2112 &&
          re15_elzazombies_r39_zustand() == RE15_R39Q_AUS && !re15_game_flag_get(2, 7) && !re15_game_flag_get(1, 27),
          "Rueckkehr nach 1041 an die gemerkte Stelle (%d,%d) Gier %d, Kette AUS, Riegel (2,7)/(1,27) los", (int)pl->x, (int)pl->z, (int)pl->rot_y);
    int quellen = 1;
    for (int i = 0; i < RE15_R39Q_N_10E1; i++) quellen &= re15_game_flag_get(7, (uint8_t)(RE15_R39Q_BIT_10E1_0 + i));
    for (int i = 0; i < RE15_R39Q_N_1071; i++) quellen &= re15_game_flag_get(7, (uint8_t)(RE15_R39Q_BIT_1071_0 + i));
    PRUEF(re15_game_flag_get(9, 95) && quellen, "(9,95)=1, alle Quell-Records stillgelegt (umgezogen oder tot)");
    /* zweite Flanke: keine zweite Kette */
    re15_game_flag_set(9, 97, 0); frame(); re15_game_flag_set(9, 97, 1);
    int b2 = 0; int w2 = fahren(300, &b2);
    PRUEF(!w2, "zweite Flanke von (9,97) nach der Montage: keine zweite Kette ((9,95)=1)");
}

/* ================================================================================================================== */
/* Teil nach1061                                                                                                      */
/* ================================================================================================================== */
static void teil_nach1061(void)
{
    printf("== nach1061 ==\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    elza_flags();
    re15_game_flag_set(9, 95, 1);
    for (int i = 0; i < RE15_R39Q_N_10E1; i++) re15_game_flag_set(7, (uint8_t)(RE15_R39Q_BIT_10E1_0 + i), 1);
    re15_game_flag_set(7, 126, 1);                             /* eine Kopie wurde schon getoetet: erscheint nicht */
    re15_elzazombies_r39_reset();
    /* Eintritt von unten (ROOM1041 Tuer Slot 2 @0x010D8: (26000,0,25300) Gier 2048 Cut 7) */
    if (room_boot(0x1061, 26000, 0, 25300, 2048, 7) != 0) return;
    const re15_r39q_lage_t *lagen = NULL; re15_elzazombies_r39_lagen(1, &lagen);
    int n = 0, an_lage = 0;
    int32_t x0[RE15_ACTOR_MAX] = {0}, z0[RE15_ACTOR_MAX] = {0};
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *a = &g_actors[i];
        if (!a->active || a->em_flag_id < 124 || a->em_flag_id > 127) continue;
        n++; x0[i] = a->x; z0[i] = a->z;
        const int k = a->em_flag_id - 124;
        if (a->x == lagen[k].x && a->z == lagen[k].z && a->y == RE15_R39Q_Y_1061 && a->floor == RE15_R39Q_ETAGE_1061) an_lage++;
    }
    PRUEF(n == 3 && an_lage == 3, "drei Kopien erscheinen (die getoetete Bit 126 nicht), an ihren Westseiten-Lagen auf Etage 4 (%d/%d)", an_lage, n);
    PRUEF(re15_elzazombies_r39_1061_gehalten(), "gehalten, solange die Spielerin unten ist");
    for (int f = 0; f < 200; f++) frame();
    int still = 1;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *a = &g_actors[i];
        if (!a->active || a->em_flag_id < 124 || a->em_flag_id > 127) continue;
        if (labs((long)a->x - x0[i]) + labs((long)a->z - z0[i]) > 50) still = 0;
    }
    PRUEF(still && re15_elzazombies_r39_1061_gehalten(), "200 Bilder mit der Spielerin auf Etage 0: die Kopien stehen (Weg <= 50)");
    /* Ankunft von unten auf Etage 4: gemessener Ankunftspunkt (Lauf e1061e) */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = 25023; pl->y = RE15_R39Q_Y_1061; pl->z = 24568; pl->rot_y = 3072;
    pl->floor = RE15_R39Q_ETAGE_1061; re15_collision_set_band(RE15_R39Q_ETAGE_1061);
    frame();
    int32_t px = 0, pz = 0, dmin = -1;
    int nf = re15_elzazombies_r39_freigabe(&px, &pz, &dmin);
    PRUEF(nf == 3 && dmin >= 4000 && !re15_elzazombies_r39_1061_gehalten(),
          "auf Etage 4: %d Kopien frei, kleinster Abstand %d >= 4000 (\"moeglichst weit weg\")", nf, dmin);
    int32_t d_vorher = dmin;
    for (int f = 0; f < 90; f++) frame();
    int32_t d_nach = 1 << 30;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *a = &g_actors[i];
        if (!a->active || a->em_flag_id < 124 || a->em_flag_id > 127) continue;
        int32_t d = (int32_t)lround(sqrt((double)(a->x - pl->x) * (a->x - pl->x) + (double)(a->z - pl->z) * (a->z - pl->z)));
        if (d < d_nach) d_nach = d;
    }
    PRUEF(d_nach < d_vorher - KOERPERRADIUS, "nach der Freigabe kommen sie naeher (%d -> %d in 90 Bildern, RE2-KI Gang)", d_vorher, d_nach);
}

/* ================================================================================================================== */
/* Teil nach1031 / ohne                                                                                               */
/* ================================================================================================================== */
static int kopien_im_raum(int von, int bis)
{
    int n = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) if (g_actors[i].active && g_actors[i].em_flag_id >= von && g_actors[i].em_flag_id <= bis) n++;
    return n;
}
static void teil_nach1031(void)
{
    printf("== nach1031 ==\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    elza_flags(); re15_game_flag_set(4, 15, 1); re15_game_flag_set(9, 95, 1);
    re15_elzazombies_r39_reset();
    if (room_boot(0x1031, -8200, 0, -22500, 1024, 0) != 0) return;
    const int k = kopien_im_raum(128, 132), kr = kopien_im_raum(133, 133) + kopien_im_raum(144, 145);
    PRUEF(k == 5 && kr == 3, "mit (9,95): fuenf 1071-Kopien und drei Kriecher in ROOM1031 (%d/%d)", k, kr);
    elza_flags(); re15_game_flag_set(4, 15, 1);
    re15_elzazombies_r39_reset();
    if (room_boot(0x1031, -8200, 0, -22500, 1024, 0) != 0) return;
    PRUEF(kopien_im_raum(128, 145) == 0, "ohne (9,95): keine Kopie");
}

static void teil_ohne(void)
{
    printf("== ohne ==\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    /* (a) Bit 97 beim Laden schon gesetzt: keine Flanke */
    elza_flags(); re15_game_flag_set(9, 97, 1);
    re15_elzazombies_r39_reset();
    if (room_boot(0x1041, -21008, 0, -13134, 2112, 0) != 0) return;
    re15_elzazombies_r39_geladen(0x1041);
    int b = 0; int w = fahren(300, &b);
    PRUEF(!w && re15_elzazombies_r39_zustand() == RE15_R39Q_AUS && !re15_game_flag_get(9, 95),
          "(9,97) im Spielstand: keine Flanke, keine Kette, (9,95)=0");
    /* (b) ohne Bit 97 nichts */
    elza_flags();
    re15_elzazombies_r39_reset();
    if (room_boot(0x1041, -21008, 0, -13134, 2112, 0) != 0) return;
    w = fahren(300, &b);
    PRUEF(!w && !re15_game_flag_get(9, 95), "ohne (9,97): keine Kette");
    /* (c) Leon: Flanke und (9,95) wirken nicht */
    re15_game_state_init(); g_gameflow.character = 0;
    re15_elzazombies_r39_reset();
    if (room_boot(0x1040, -21008, 0, -13134, 2112, 0) != 0) return;
    for (int f = 0; f < 10; f++) frame();
    re15_game_flag_set(9, 97, 1);
    w = fahren(300, &b);
    PRUEF(!w && !re15_game_flag_get(9, 95), "Leon (Figur 0) ROOM1040: Flanke (9,97) startet nichts");
    re15_game_state_init(); g_gameflow.character = 0; re15_game_flag_set(9, 95, 1);
    if (room_boot(0x1060, 26000, 0, 25300, 2048, 7) != 0) return;
    PRUEF(kopien_im_raum(124, 145) == 0, "Leon ROOM1060 mit (9,95): keine Kopie");
    g_gameflow.character = 4;
}

/* ================================================================================================================== */
/* auswerten_ton <laut.raw> <stumm.raw>: integration_r39_elzazombies_knall — zwei RE15_AUDIO_CAP_SYNC-Mitschnitte desselben  */
/* Laufs, einmal mit RE15_R39Q_KNALL_STUMM (Signal verbraucht, nicht gespielt). Je Spielbild 1470 Stereo-Abtastungen s16  */
/* (audio_pc.c, Offset = Bild * 1470). Die Differenz = die Knaelle der Kette (Satz 0: Auftakt 1041, Ende 1061).          */
/* ================================================================================================================== */
static int auswerten_ton(const char *pa, const char *pb)
{
    size_t na = 0, nb = 0;
    uint8_t *a = slurp(pa, &na), *b = slurp(pb, &nb);
    if (!a || !b) { printf("  FEHLER: Mitschnitt fehlt (%s / %s)\n", pa, pb); return 1; }
    const size_t F = 1470 * 4;
    size_t n = (na < nb ? na : nb) / F;
    int gruppen = 0, g_von = -10, g_bis = -10, lang_ok = 0; double g_max = 0;
    for (size_t f = 0; f < n; f++) {
        const int16_t *x = (const int16_t *)(a + f * F), *y = (const int16_t *)(b + f * F);
        double e = 0; for (size_t i = 0; i < F / 2; i++) { double d = (double)x[i] - y[i]; e += d * d; }
        e = sqrt(e / (double)(F / 2));
        if (e <= 0) continue;
        if ((int)f - g_bis > 3) {
            if (g_bis >= 0) { printf("  Knall Bilder %d..%d (%d), Differenz-RMS max %.0f\n", g_von, g_bis, g_bis - g_von + 1, g_max);
                              if (g_bis - g_von + 1 >= 20 && g_max >= 1000) lang_ok++; }
            gruppen++; g_von = (int)f; g_max = 0;
        }
        g_bis = (int)f; if (e > g_max) g_max = e;
    }
    if (g_bis >= 0) { printf("  Knall Bilder %d..%d (%d), Differenz-RMS max %.0f\n", g_von, g_bis, g_bis - g_von + 1, g_max);
                      if (g_bis - g_von + 1 >= 20 && g_max >= 1000) lang_ok++; }
    PRUEF(gruppen == 2 && lang_ok == 2, "zwei hoerbare Tuerknaelle (Knall-Bank Satz 0) in der Kette: %d Gruppen, %d >= 20 Bilder und RMS >= 1000",
          gruppen, lang_ok);
    free(a); free(b);
    return g_fail;
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "auswerten_ton")) {
        if (argc < 4) { printf("auswerten_ton <laut.raw> <stumm.raw>\n"); return 2; }
        int f = auswerten_ton(argv[2], argv[3]);
        printf("%s: %d Fehler\n", teil, f);
        return f ? 1 : 0;
    }
    int alle = !strcmp(teil, "alle");
    if (alle || !strcmp(teil, "zensus"))    teil_zensus();
    if (alle || !strcmp(teil, "kette_45"))  { teil_kette(4, 5, 1); teil_kette(4, 5, 0); }
    if (alle || !strcmp(teil, "kette_05"))  teil_kette(0, 5, 1);
    if (alle || !strcmp(teil, "kette_00"))  teil_kette(0, 0, 1);
    if (alle || !strcmp(teil, "kette_21"))  teil_kette(2, 1, 1);
    if (alle || !strcmp(teil, "nach1061"))  teil_nach1061();
    if (alle || !strcmp(teil, "nach1031"))  teil_nach1031();
    if (alle || !strcmp(teil, "ohne"))      teil_ohne();
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
